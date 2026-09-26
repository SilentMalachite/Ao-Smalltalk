#include "Session.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/ClassPool.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Image.hpp"
#include "ao/Interpreter.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"
#include "ao/kernel/Install.hpp"

#include "ao_abi.h"

#include <algorithm>
#include <climits>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ao {
namespace {

std::unique_ptr<Session> g_session;
// SPEC §3.10: the transcript hook the ABI set. Every session boot and load make gets it.
HostOopHook g_transcriptHook = nullptr;
// SPEC §3.13: capture on or off, as the ABI set it (off by default). Every session boot and load
// make gets it.
bool g_debugCapture = false;
// SPEC §3.13: the live mode, as the ABI set it (post-mortem by default). Each ao_eval reads it.
int g_debugMode = AO_DEBUG_POSTMORTEM;
// SPEC §3.10: the ao_debug_* prints and inspects running (DebugEntry). While one runs, capture stays
// off and a new setting waits for its end.
int g_debugEntries = 0;
// SPEC §3.10 ao_debug_generation: process-wide, so a snapshot of a later session never shows an
// earlier session's number.
std::uint32_t g_debugGeneration = 0;

// ao_eval compiles "doIt\n" + source (SPEC §3.10): spans and errors take this off.
constexpr std::uint32_t kDoItPrefix = 5;

void bumpDebugGeneration() {
  g_debugGeneration = (g_debugGeneration + 1) & 0x7FFFFFFFu;
}

Oop workspaceBinding(CallContext& ctx, std::string_view name);

void installEmptyCache(Session& session, HostOopHook transcript, HostOopHook inspect) {
  // The scheduler's base process runs on ctx: it goes before ctx does and comes back with it.
  session.scheduler.reset();
  session.cache = std::make_unique<ClassMethodCache>();
  session.cache->addRoots(session.roots);
  session.ctx = std::unique_ptr<CallContext>(new CallContext{
      session.heap, session.roots, session.wk, session.cache.get()});
  session.ctx->transcriptHook = transcript;
  session.ctx->inspectHook = inspect;
  session.ctx->bindingHook = workspaceBinding;
  // SPEC §3.13: the session is the sink while capture is on. The scheduler copies it into the
  // fibers.
  session.ctx->debug = g_debugCapture ? &session : nullptr;
  // SPEC §3.10: one scheduler per session. adoptImage comes after boot or load.
  session.scheduler = std::make_unique<Scheduler>(*session.ctx);
}

bool installEmptyWorkspace(Session& session) {
  if (session.ctx == nullptr || !session.wk.dictionaryClass.isHeap()) {
    return false;
  }
  if (!session.workspaceRooted) {
    session.roots.add(&session.workspace);
    session.workspaceRooted = true;
  }
  const Oop sel = session.wk.intern("new");
  if (!sel.isHeap()) {
    return false;
  }
  // SPEC §3.4: making the workspace is an outermost entry. What an earlier entry left over is not
  // blamed on it, and an abort it ends in is its failure, read and cleared here. Neither clearing
  // nor refreshing collects.
  CallContext& ctx = *session.ctx;
  clearUnwinding(ctx);
  refreshStackLimit(ctx);
  const Oop dict = send(ctx, session.wk.dictionaryClass, sel, nullptr, 0, nullptr);
  const bool aborted = unwinding(ctx);
  clearUnwinding(ctx);
  if (aborted || !dict.isHeap()) {
    return false;
  }
  session.workspace = dict;
  return true;
}

bool loadedImageProbes(Session& session) {
  // SPEC §3.4 / §3.10: each probe is an outermost entry, and an abort is a failed probe.
  CallContext& ctx = *session.ctx;
  clearUnwinding(ctx);
  refreshStackLimit(ctx);
  Oop arg = Oop::fromSmallInteger(2);
  const Oop three = send(ctx, Oop::fromSmallInteger(1), session.wk.intern("+"), &arg, 1, nullptr);
  const bool added = !unwinding(ctx) && three.isSmallInteger() && three.smallIntegerValue() == 3;
  clearUnwinding(ctx);
  if (!added) {
    return false;
  }
  refreshStackLimit(ctx);
  const Oop isNil = send(ctx, Oop::nil(), session.wk.intern("isNil"), nullptr, 0, nullptr);
  const bool answered = !unwinding(ctx) && isNil.isTrue();
  clearUnwinding(ctx);
  return answered;
}

// An entry's slots: method, text, then its blocks. Removed in the reverse order (Roots::remove
// looks from the newest registration back). All or nothing (SPEC §3.10): the room comes first, so
// std::bad_alloc leaves before any slot is registered and none of the adds can throw.
void rootEntry(Roots& roots, Session::MethodSource& entry) {
  roots.reserveSlots(2 + entry.blocks.size());
  roots.add(&entry.method);
  roots.add(&entry.text);
  for (Oop& block : entry.blocks) {
    roots.add(&block);
  }
}

void unrootEntry(Roots& roots, Session::MethodSource& entry) {
  for (auto it = entry.blocks.rbegin(); it != entry.blocks.rend(); ++it) {
    roots.remove(&*it);
  }
  roots.remove(&entry.text);
  roots.remove(&entry.method);
}

// The CompiledMethod at literal `index` of `method`; the empty Oop when it is not one.
Oop blockLiteral(const Session& s, Oop method, std::size_t index) {
  const Oop lits = s.heap.slotAt(method, kCmSlotLiterals);
  if (!lits.isHeap() || (s.heap.flags(lits) & kFlagBytes) != 0 || index >= s.heap.size(lits)) {
    return Oop{};
  }
  const Oop block = s.heap.slotAt(lits, static_cast<std::uint32_t>(index));
  return block.isHeap() && s.heap.klass(block) == s.wk.compiledMethodClass ? block : Oop{};
}

// Fills entry.blocks and entry.debug from `image`, the image entry.method was boxed from
// (boxMethodImage): a Method literal at index i is the block CompiledMethod at literal i of the
// boxed method, and nested blocks follow in preorder. Only reads the heap (no GC). May throw
// std::bad_alloc before it changes the entry.
void attachBlocks(const Session& s, Session::MethodSource& entry,
                  const compiler::MethodImage& image) {
  auto info = std::make_shared<DebugInfo>();
  std::vector<Oop> blocks;
  info->bodies.push_back(MethodDebugInfo{image.pcMap, image.temps});
  struct Walk {
    Oop method;
    const compiler::MethodImage* image;
    std::size_t next;
  };
  std::vector<Walk> stack{Walk{entry.method, &image, 0}};
  while (!stack.empty()) {
    Walk& walk = stack.back();
    if (walk.next >= walk.image->literals.size()) {
      stack.pop_back();
      continue;
    }
    const std::size_t index = walk.next++;
    const compiler::Literal& lit = walk.image->literals[index];
    if (lit.kind != compiler::LitKind::Method || lit.method == nullptr) {
      continue;
    }
    const Oop block = blockLiteral(s, walk.method, index);
    if (block.isEmpty()) {
      continue;
    }
    blocks.push_back(block);
    info->bodies.push_back(MethodDebugInfo{lit.method->pcMap, lit.method->temps});
    stack.push_back(Walk{block, lit.method.get(), 0});
  }
  entry.blocks = std::move(blocks);
  entry.debug = std::move(info);
}

// The entry whose method or block is `method`: the accepted methods', then the doIt's. *index is
// 0 for the method, k for blocks[k - 1].
const Session::MethodSource* findEntry(const Session& s, Oop method, std::uint32_t* index) {
  auto holds = [&](const Session::MethodSource& entry) {
    if (entry.method == method) {
      *index = 0;
      return true;
    }
    for (std::size_t k = 0; k < entry.blocks.size(); ++k) {
      if (entry.blocks[k] == method) {
        *index = static_cast<std::uint32_t>(k + 1);
        return true;
      }
    }
    return false;
  };
  for (const auto& entry : s.methodSources) {
    if (holds(*entry)) {
      return entry.get();
    }
  }
  if (s.doItDebug != nullptr && holds(*s.doItDebug)) {
    return s.doItDebug.get();
  }
  return nullptr;
}

void unrootMethodSources(Session& session) {
  for (auto& entry : session.methodSources) {
    unrootEntry(session.roots, *entry);
  }
}

void dropDoItDebug(Session& session) {
  if (session.doItDebug != nullptr) {
    unrootEntry(session.roots, *session.doItDebug);
    session.doItDebug.reset();
  }
}

void releaseMethodSources(Session& session) {
  unrootMethodSources(session);
  session.methodSources.clear();
  dropDoItDebug(session);
}

}  // namespace

Session::Session(bool bootstrap) : wk(heap, roots) {
  if (!bootstrap) {
    return;
  }
  installEmptyCache(*this, nullptr, nullptr);
  Bootstrap::run(heap, roots, wk);
  // SPEC §3.4 ベースプロセス: Processor's activeProcess right after boot.
  scheduler->adoptImage();
}

Session::~Session() {
  // SPEC §3.4 abandon, §3.10: the processes left go first, while the heap and the roots are there.
  scheduler.reset();
  releaseMethodSources(*this);
}

void Session::onAbort(CallContext& aborting) noexcept {
  // SPEC §3.10, §3.13: only the live session captures. ao_image_load's new session aborts (its
  // workspace creation, its probes) before the swap; a failed load must not move the generation.
  if (this != g_session.get()) {
    return;
  }
  // A fiber runs on its own CallContext; the base on ctx. SPEC §3.13: an evaluating process's
  // failure is the evaluation's, so it replaces the snapshot as the base's does.
  const bool base = &aborting == ctx.get() ||
                    (scheduler != nullptr && scheduler->runningEval());
  if (!base && !debug.empty()) {
    return;
  }
  debug.capture(aborting);
  bumpDebugGeneration();
}

Session* session() { return g_session.get(); }

int sessionBoot() {
  if (g_session != nullptr) {
    return 1;
  }
  g_session = std::make_unique<Session>(true);
  g_session->ctx->transcriptHook = g_transcriptHook;
  if (!installEmptyWorkspace(*g_session)) {
    g_session.reset();
    return 1;
  }
  clearMethodSources();
  return 0;
}

int sessionShutdown() {
  // SPEC §3.13: shutdown clears the snapshot.
  sessionDebugClear();
  g_session.reset();
  return 0;
}

void setSessionTranscriptHook(HostOopHook hook) {
  g_transcriptHook = hook;
  if (g_session != nullptr && g_session->ctx != nullptr) {
    g_session->ctx->transcriptHook = hook;
  }
}

void setSessionDebugCapture(bool on) {
  g_debugCapture = on;
  // SPEC §3.10: inside an ao_debug_* print or inspect (a hook calling in), the setting is only
  // recorded; ~DebugEntry applies it, so an abort later in that print is still not captured.
  if (g_debugEntries == 0 && g_session != nullptr && g_session->ctx != nullptr) {
    g_session->ctx->debug = on ? g_session.get() : nullptr;
  }
}

void setSessionDebugMode(int mode) {
  if (mode == AO_DEBUG_POSTMORTEM || mode == AO_DEBUG_LIVE) {
    g_debugMode = mode;
  }
}

DebugSnapshot* sessionDebugSnapshot() {
  return g_session != nullptr ? &g_session->debug : nullptr;
}

int sessionDebugGeneration() {
  return g_session != nullptr ? static_cast<int>(g_debugGeneration) : -1;
}

void sessionDebugClear() {
  if (g_session != nullptr) {
    g_session->debug.clear();
    bumpDebugGeneration();
  }
}

int sessionImageSave(const char* path) {
  if (g_session == nullptr || path == nullptr) {
    return 1;
  }
  // Workspace bindings are session state. Drop the root so Image::save does not trace them.
  struct HideWorkspace {
    Session* session = nullptr;
    bool hidden = false;
    explicit HideWorkspace(Session* s) : session(s) {
      if (session != nullptr && session->workspaceRooted) {
        session->roots.remove(&session->workspace);
        hidden = true;
      }
    }
    ~HideWorkspace() {
      if (hidden && session != nullptr) {
        session->roots.add(&session->workspace);
      }
    }
  } hide(g_session.get());
  // Source text, the blocks and the doIt the table holds, and the snapshot are session state
  // (SPEC §3.10, §3.13). Drop their roots so Image::save does not trace them; Image::save does not
  // move objects, so the slots are still right when they come back.
  struct HideMethodSources {
    Roots* roots = nullptr;
    std::vector<const Oop*> slots;
    Oop* snapFirst = nullptr;
    std::size_t snapCount = 0;
    explicit HideMethodSources(Session* s) {
      if (s != nullptr) {
        roots = &s->roots;
        // The table's are LIFO slots; the snapshot's are one pinned range.
        slots = methodSourceRootSlots(false);
        for (auto it = slots.rbegin(); it != slots.rend(); ++it) {
          roots->remove(const_cast<Oop*>(*it));
        }
        snapFirst = s->debug.rootFirst();
        snapCount = s->debug.rootCount();
        roots->unpinRange(snapFirst, snapCount);
      }
    }
    ~HideMethodSources() {
      if (roots != nullptr) {
        roots->pinRange(snapFirst, snapCount);
        for (const Oop* slot : slots) {
          roots->add(const_cast<Oop*>(slot));
        }
      }
    }
  } hideSources(g_session.get());
  return Image::save(g_session->heap, g_session->roots, g_session->wk, path) ? 0 : 1;
}

int sessionImageLoad(const char* path, std::string* reason) {
  auto fail = [reason](const char* why) {
    if (reason != nullptr && reason->empty()) {
      *reason = why;
    }
    return 1;
  };
  if (reason != nullptr) {
    reason->clear();
  }
  if (g_session == nullptr || path == nullptr) {
    return fail("image load failed");
  }
  auto next = std::make_unique<Session>(false);
  if (!Image::load(next->heap, next->roots, next->wk, path, reason)) {
    return fail("image load failed");
  }
  HostOopHook inspect = nullptr;
  if (g_session->ctx != nullptr) {
    inspect = g_session->ctx->inspectHook;
  }
  installEmptyCache(*next, g_transcriptHook, inspect);
  // SPEC §3.4, §3.11: the base is Processor's activeProcess right after the load, and the ready
  // queue starts empty; the image's other Processes cannot run.
  next->scheduler->adoptImage();
  if (!installEmptyWorkspace(*next)) {
    return fail("image load failed");
  }
  // SPEC §3.10: the natives and the probes run on the new session. Only when both pass does it
  // replace the current one; otherwise the current session stays as it was, its processes too.
  ensureKernelNatives(*next);
  if (!loadedImageProbes(*next)) {
    return fail("image probes failed");
  }
  // Dropping the current session abandons its processes (~Session).
  g_session = std::move(next);
  clearMethodSources();
  // SPEC §3.13: the load replaces the snapshot with an empty one (a probe may have filled it).
  sessionDebugClear();
  return 0;
}

int sessionWorkspaceReset() {
  if (g_session == nullptr) {
    return 1;
  }
  return installEmptyWorkspace(*g_session) ? 0 : 1;
}

int sessionFileInLoadOrder(const char* path) {
  if (g_session == nullptr || g_session->ctx == nullptr || path == nullptr) {
    return 1;
  }
  // SPEC §3.10 / §3.12: fails on an unreadable path or on any error DEFERRED.md does not list.
  std::vector<FileInError> errors;
  return fileInLoadOrder(*g_session->ctx, path, errors) ? 0 : 1;
}

void ensureKernelNatives(Session& s) {
  // An image may have a Transcript metaclass without a dictionary; the class-side natives need one.
  const Oop meta = s.wk.transcriptMetaclass;
  if (meta.isHeap() && !s.heap.slotAt(meta, kClassSlotMethodDict).isHeap()) {
    const Oop dict = MethodDictionary::create(s.heap, s.wk, 8);
    if (dict.isHeap()) {
      s.heap.slotAtPut(meta, kClassSlotMethodDict, dict);
    }
  }
  // SPEC §3.10: add the Kernel natives the image lacks (those added after it was saved) and keep
  // every method it has.
  kernel::installMissing(s.heap, s.roots, s.wk, s.cache.get());
  // SPEC §3.5: installMissing may have added one of the eight; a kept user method may hide one.
  s.wk.checkSmallIntegerFastPath();
}

namespace {

struct ClassRow {
  Oop cls;
  std::string name;
  std::string category;
};

struct ListedMethod {
  std::string selector;
  bool native = false;
  Oop method;
};

bool utf8Less(const std::string& a, const std::string& b) {
  const auto* ab = reinterpret_cast<const unsigned char*>(a.data());
  const auto* bb = reinterpret_cast<const unsigned char*>(b.data());
  return std::lexicographical_compare(ab, ab + a.size(), bb, bb + b.size());
}

bool pointerSlots(Heap& heap, Oop obj, std::uint32_t minSlots) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0 && heap.size(obj) >= minSlots;
}

std::string byteText(Heap& heap, Oop obj) {
  if (!obj.isHeap() || (heap.flags(obj) & kFlagBytes) == 0) {
    return {};
  }
  const auto n = heap.size(obj);
  if (n == 0) {
    return {};
  }
  const char* bytes = reinterpret_cast<const char*>(heap.bytes(obj));
  return std::string(bytes, static_cast<std::size_t>(n));
}

int writeBuf(const std::string& text, char* buf, int len) {
  if (buf == nullptr || len <= 0) {
    return AO_ERR;
  }
  const auto room = static_cast<std::size_t>(len - 1);
  const bool fits = text.size() <= room;
  const std::size_t n = fits ? text.size() : room;
  if (n != 0) {
    std::memcpy(buf, text.data(), n);
  }
  buf[n] = '\0';
  return fits ? AO_OK : AO_ERR_RANGE;
}

std::string classNameOf(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotName + 1)) {
    return {};
  }
  return byteText(heap, heap.slotAt(cls, kClassSlotName));
}

std::string categoryHeading(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotCategory + 1)) {
    return "Kernel";
  }
  const std::string text = byteText(heap, heap.slotAt(cls, kClassSlotCategory));
  return text.empty() ? "Kernel" : text;
}

std::string definitionCategory(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotCategory + 1)) {
    return {};
  }
  return byteText(heap, heap.slotAt(cls, kClassSlotCategory));
}

std::string instVarList(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotInstVarNames + 1)) {
    return {};
  }
  const Oop names = heap.slotAt(cls, kClassSlotInstVarNames);
  if (!pointerSlots(heap, names, 0)) {
    return {};
  }
  std::string out;
  const auto n = heap.size(names);
  for (std::uint32_t i = 0; i < n; ++i) {
    const std::string one = byteText(heap, heap.slotAt(names, i));
    if (one.empty()) {
      continue;
    }
    if (!out.empty()) {
      out.push_back(' ');
    }
    out += one;
  }
  return out;
}

// SPEC §3.10: the class's own class variables (its classPool's names, not the superclasses'), in
// byte order, one blank apart.
std::string classVarList(Heap& heap, const WellKnown& wk, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotClassPool + 1)) {
    return {};
  }
  std::string out;
  for (const std::string& name : ClassPool::names(heap, wk, heap.slotAt(cls, kClassSlotClassPool))) {
    if (!out.empty()) {
      out.push_back(' ');
    }
    out += name;
  }
  return out;
}

std::vector<ClassRow> classRows(Session& s) {
  struct Baton {
    Session* session;
    std::vector<ClassRow>* rows;
  };
  std::vector<ClassRow> rows;
  Baton baton{&s, &rows};
  s.wk.eachClass(
      [](void* p, Oop cls) {
        auto* b = static_cast<Baton*>(p);
        if (!pointerSlots(b->session->heap, cls, kClassSlotCount)) {
          return;
        }
        ClassRow row;
        row.cls = cls;
        row.name = classNameOf(b->session->heap, cls);
        row.category = categoryHeading(b->session->heap, cls);
        b->rows->push_back(std::move(row));
      },
      &baton);
  std::sort(rows.begin(), rows.end(), [](const ClassRow& a, const ClassRow& b) {
    return utf8Less(a.name, b.name);
  });
  return rows;
}

const ClassRow* findClass(const std::vector<ClassRow>& rows, const char* name) {
  if (name == nullptr) {
    return nullptr;
  }
  for (const auto& row : rows) {
    if (row.name == name) {
      return &row;
    }
  }
  return nullptr;
}

Oop sideOf(Session& s, Oop cls, int meta) {
  if (meta == 0 || !cls.isHeap()) {
    return cls;
  }
  return s.heap.klass(cls);
}

std::vector<ListedMethod> methodsOf(Session& s, Oop side) {
  std::vector<ListedMethod> out;
  if (!pointerSlots(s.heap, side, kClassSlotMethodDict + 1)) {
    return out;
  }
  const Oop dict = s.heap.slotAt(side, kClassSlotMethodDict);
  if (!pointerSlots(s.heap, dict, kDictSlotArray + 1)) {
    return out;
  }
  const Oop inner = s.heap.slotAt(dict, kDictSlotArray);
  if (!pointerSlots(s.heap, inner, 0)) {
    return out;
  }
  const auto n = s.heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = s.heap.slotAt(inner, i);
    const Oop value = s.heap.slotAt(inner, i + 1);
    if (!key.isHeap() || !value.isHeap()) {
      continue;
    }
    ListedMethod row;
    row.selector = byteText(s.heap, key);
    if (row.selector.empty() && s.heap.size(key) != 0) {
      continue;
    }
    row.native = s.heap.klass(value) == s.wk.nativeMethodClass;
    row.method = value;
    out.push_back(std::move(row));
  }
  std::sort(out.begin(), out.end(), [](const ListedMethod& a, const ListedMethod& b) {
    return utf8Less(a.selector, b.selector);
  });
  return out;
}

std::vector<std::string> protocolsOf(const std::vector<ListedMethod>& methods) {
  bool native = false;
  bool user = false;
  for (const auto& method : methods) {
    if (method.native) {
      native = true;
    } else {
      user = true;
    }
  }
  std::vector<std::string> out;
  if (native) {
    out.emplace_back("native");
  }
  if (user) {
    out.emplace_back("user");
  }
  return out;
}

bool knownProtocol(const char* protocol) {
  return protocol != nullptr &&
         (std::strcmp(protocol, "native") == 0 || std::strcmp(protocol, "user") == 0);
}

std::vector<ListedMethod> selectorsFor(const std::vector<ListedMethod>& methods,
                                       const char* protocol) {
  std::vector<ListedMethod> out;
  if (!knownProtocol(protocol)) {
    return out;
  }
  const bool native = std::strcmp(protocol, "native") == 0;
  for (const auto& method : methods) {
    if (method.native == native) {
      out.push_back(method);
    }
  }
  return out;
}

// SPEC §3.10: true with the source-table text, or false with the placeholder for a method without
// source: one comment on one line, without a selector pattern, so accepting it fails to compile.
// className is "<Name> class" on the class side.
bool sourceOf(Session& s, const std::string& className, const ListedMethod& method,
              std::string& text) {
  if (!method.native && methodSource(method.method, text)) {
    return true;
  }
  text = "\"";
  text += className;
  text += ">>";
  text += method.selector;
  if (method.native) {
    const std::string_view sym = NativeMethod::nameBytes(s.heap, method.method);
    text += " native ";
    text.append(sym.data(), sym.size());
  } else {
    text += " source not available";
  }
  text += "\"";
  return false;
}

std::string superclassName(Session& s, Oop cls, int meta) {
  const Oop side = sideOf(s, cls, meta);
  if (!pointerSlots(s.heap, side, kClassSlotSuperclass + 1)) {
    return {};
  }
  const Oop sup = s.heap.slotAt(side, kClassSlotSuperclass);
  if (!sup.isHeap()) {
    return {};
  }
  // A metaclass's superclass is another class object. Name the instance class when it has one.
  if (s.heap.klass(sup) == s.wk.metaclassClass &&
      pointerSlots(s.heap, sup, kClassSlotThisClass + 1)) {
    const Oop thisClass = s.heap.slotAt(sup, kClassSlotThisClass);
    if (thisClass.isHeap()) {
      return classNameOf(s.heap, thisClass);
    }
  }
  return classNameOf(s.heap, sup);
}

std::string definitionOf(Session& s, const ClassRow& row) {
  std::string text;
  if (!pointerSlots(s.heap, row.cls, kClassSlotSuperclass + 1)) {
    text = "nil";
  } else {
    const Oop sup = s.heap.slotAt(row.cls, kClassSlotSuperclass);
    text = sup.isHeap() ? classNameOf(s.heap, sup) : std::string("nil");
  }
  text += " subclass: #";
  text += row.name;
  text += "\n  instanceVariableNames: '";
  text += instVarList(s.heap, row.cls);
  text += "'\n  classVariableNames: '";
  text += classVarList(s.heap, s.wk, row.cls);
  text += "'\n  poolDictionaries: ''\n  category: '";
  // SPEC §3.10: a string literal in a chunk, so accepting the text again keeps the category.
  for (const char c : definitionCategory(s.heap, row.cls)) {
    if (c == '\'' || c == '!') {
      text.push_back(c);
    }
    text.push_back(c);
  }
  text += "'";
  return text;
}

std::vector<std::string> subclassNames(Session& s, const std::string& name,
                                       const std::vector<ClassRow>& rows) {
  std::vector<std::string> out;
  for (const auto& row : rows) {
    if (!pointerSlots(s.heap, row.cls, kClassSlotSuperclass + 1)) {
      continue;
    }
    const Oop sup = s.heap.slotAt(row.cls, kClassSlotSuperclass);
    if (!sup.isHeap()) {
      continue;
    }
    if (classNameOf(s.heap, sup) == name) {
      out.push_back(row.name);
    }
  }
  return out;
}

// SPEC §3.10: a count function's failure. AO_ERR (1) would read as one row.
constexpr int kCountFailed = -1;

bool metaOk(int meta) { return meta == 0 || meta == 1; }

struct NameBag {
  Heap* heap = nullptr;
  std::vector<std::string>* names = nullptr;
};

// SPEC §3.10: the keys of Smalltalk, the fixed globals and those subclass: and at:put: added.
void collectKnownGlobals(Session& session, std::vector<std::string>* names) {
  names->clear();
  NameBag bag{&session.heap, names};
  Globals::each(
      session.wk,
      [](void* baton, Oop key, Oop) {
        auto* b = static_cast<NameBag*>(baton);
        b->names->push_back(byteText(*b->heap, key));
      },
      &bag);
}

// SPEC §3.10: cached, and rebuilt only after a class definition or Smalltalk at:put:.
const std::vector<std::string>& knownGlobals(Session& session) {
  const std::uint64_t version = session.wk.globalsVersion();
  if (!session.knownGlobalsCached || session.knownGlobalsVersion != version) {
    collectKnownGlobals(session, &session.knownGlobals);
    session.knownGlobalsVersion = version;
    session.knownGlobalsCached = true;
  }
  return session.knownGlobals;
}

// SPEC §3.10: the workspace binding for name. Made with value nil and kept in the workspace
// dictionary on first use, so every evaluation sees the same Association.
Oop workspaceBinding(CallContext& ctx, std::string_view name) {
  Session* s = g_session.get();
  if (s == nullptr || !s->workspace.isHeap()) {
    return Oop{};
  }
  Root key(ctx.roots, Str::fromUtf8(ctx, name));
  Root has(ctx.roots, ctx.wk.intern("includesKey:"));
  Root at(ctx.roots, ctx.wk.intern("at:"));
  Root atPut(ctx.roots, ctx.wk.intern("at:put:"));
  if (!key.slot.isHeap() || !has.slot.isHeap() || !at.slot.isHeap() || !atPut.slot.isHeap()) {
    return Oop{};
  }
  if (send(ctx, s->workspace, has.slot, &key.slot, 1, nullptr).isTrue()) {
    const Oop found = send(ctx, s->workspace, at.slot, &key.slot, 1, nullptr);
    if (found.isHeap() && ctx.heap.klass(found) == ctx.wk.associationClass) {
      return found;
    }
  }
  RootedArray kv(ctx.roots, 2);
  kv[0] = key.slot;
  kv[1] = allocateRetry(ctx, ctx.wk.associationClass, 2, 0);
  if (!kv[1].isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(kv[1], kAssocKey, kv[0]);
  ctx.heap.slotAtPut(kv[1], kAssocValue, Oop::nil());
  if (send(ctx, s->workspace, atPut.slot, kv.ptr(), 2, nullptr).isEmpty()) {
    return Oop{};
  }
  return kv[1];
}

void blankOut(char* out, int outLen) {
  if (outLen > 0 && out != nullptr) {
    out[0] = '\0';
  }
}

// Sends printString to `value` (a rooted slot, read again after the send) and puts the answer's
// UTF-8 in *utf8. False when the send fails or aborts, the answer is not a bytes object, or the
// heap ran out (SPEC §3.2: an evaluation that ran out of memory answers only the error).
bool printStringOf(Session& session, const Oop& value, std::string* utf8) {
  const Oop sel = session.wk.intern("printString");
  if (!sel.isHeap()) {
    return false;
  }
  Root printed(session.roots, send(*session.ctx, value, sel, nullptr, 0, nullptr));
  if (!printed.slot.isHeap() || (session.heap.flags(printed.slot) & kFlagBytes) == 0) {
    return false;
  }
  *utf8 = Str::toUtf8(session.heap, printed.slot);
  return !session.heap.outOfMemory();
}

// SPEC §3.10 Inspect it (ao_eval's and ao_debug_inspect's): sends inspect to `value` (a rooted
// slot), then printString, then hands the class name and the printString to the hook. False when
// a send fails or the heap ran out: the hook is not called, so no Inspector opens for a failure.
// A printString of INT_MAX bytes or more cannot be handed over as an int with its NUL: the hook is
// not called for it either. *utf8 gets the printString.
bool inspectValue(Session& session, const Oop& value, AoInspectFn inspect, void* inspectUser,
                  std::string* utf8) {
  const Oop sel = session.wk.intern("inspect");
  if (!sel.isHeap() || send(*session.ctx, value, sel, nullptr, 0, nullptr).isEmpty()) {
    return false;
  }
  if (!printStringOf(session, value, utf8)) {
    return false;
  }
  if (inspect != nullptr && utf8->size() < static_cast<std::size_t>(INT_MAX)) {
    const std::string cls = classNameOf(session.heap, session.wk.classOf(value));
    inspect(cls.c_str(), utf8->c_str(), static_cast<int>(utf8->size()), inspectUser);
  }
  return true;
}

// *ran becomes true once the doIt is applied: the evaluation ran (SPEC §3.4 評価の終わり).
// *printedOut gets a Print it's or Inspect it's whole printString (nullopt when it is INT_MAX
// bytes or more); other outcomes leave it as it was.
// SPEC §3.10: the answer for a doIt whose value is `result` (a rooted slot): empty for a Do it,
// else the printString (and inspect first, for an Inspect it). *printedOut gets a Print it's or
// Inspect it's whole printString (nullopt when it is INT_MAX bytes or more).
int answerEval(Session& session, int mode, const Oop& result, char* out, int outLen,
               AoInspectFn inspect, void* inspectUser, std::optional<std::string>* printedOut) {
  if (mode != AO_EVAL_PRINTIT && mode != AO_EVAL_INSPECTIT) {
    return writeBuf("", out, outLen);
  }
  std::string utf8;
  const bool printedOk = mode == AO_EVAL_INSPECTIT
                             ? inspectValue(session, result, inspect, inspectUser, &utf8)
                             : printStringOf(session, result, &utf8);
  if (!printedOk) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }
  // SPEC §3.10 評価結果: a printString of INT_MAX bytes or more is not kept; out is cut
  // (AO_ERR_RANGE).
  const bool fitsInt = utf8.size() < static_cast<std::size_t>(INT_MAX);
  const int rc = writeBuf(utf8, out, outLen);
  if (fitsInt) {
    *printedOut = std::move(utf8);
  } else {
    *printedOut = std::nullopt;
  }
  return rc;
}

// *ran becomes true once the doIt is applied: the evaluation ran (SPEC §3.4 評価の終わり).
// *printedOut gets a Print it's or Inspect it's whole printString (nullopt when it is INT_MAX
// bytes or more); other outcomes leave it as it was. *failure gets the reason of a live-mode
// evaluation that failed on its process (SPEC §3.13); the base is then not unwinding.
int evalBody(const char* source, int sourceLen, int mode, char* out, int outLen, AoSpan* err,
             AoInspectFn inspect, void* inspectUser, bool* ran,
             std::optional<std::string>* printedOut, std::string* failure) {
  if (err != nullptr) {
    err->start = 0;
    err->end = 0;
    err->message[0] = '\0';
  }
  if (g_session == nullptr || g_session->ctx == nullptr) {
    blankOut(out, outLen);
    return AO_ERR;
  }
  if (mode != AO_EVAL_DOIT && mode != AO_EVAL_PRINTIT && mode != AO_EVAL_INSPECTIT) {
    blankOut(out, outLen);
    return AO_ERR;
  }
  // SPEC §3.10: without a place for the answer nothing is compiled or evaluated, so a caller
  // that retries does not run the side effects twice.
  if (out == nullptr || outLen < 1) {
    return AO_ERR;
  }
  if (sourceLen < 0 || (source == nullptr && sourceLen != 0)) {
    blankOut(out, outLen);
    return AO_ERR;
  }

  Session& session = *g_session;
  std::string text = "doIt\n";
  if (sourceLen > 0) {
    text.append(source, static_cast<std::size_t>(sourceLen));
  }

  compiler::CompileEnv env;
  env.undeclaredAreBindings = true;
  env.knownGlobals = knownGlobals(session);
  const compiler::CompileResult compiled = compiler::compileMethod(text, env);
  if (!compiled.ok) {
    if (err != nullptr) {
      unsigned start = compiled.error.span.start;
      unsigned end = compiled.error.span.end;
      err->start = start >= kDoItPrefix ? start - kDoItPrefix : 0;
      err->end = end >= kDoItPrefix ? end - kDoItPrefix : 0;
      const std::size_t n = std::min(compiled.error.message.size(), sizeof(err->message) - 1);
      if (n != 0) {
        std::memcpy(err->message, compiled.error.message.data(), n);
      }
      err->message[n] = '\0';
    }
    blankOut(out, outLen);
    return AO_ERR_COMPILE;
  }

  const compiler::MethodImage& image = compiled.image;
  if (image.numArgs != 0) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }

  // Boxing the Binding literals finds or makes the workspace's Associations (SPEC §3.10), so
  // assignments land in the workspace dictionary directly and nothing is written back.
  Root method(session.roots, boxMethodImage(*session.ctx, image, session.wk.compiledMethodClass));
  if (!method.slot.isHeap()) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }
  // SPEC §3.10: the doIt's entry (method, blocks, the user's text, debug info). Plain C++
  // memory and root slots, no GC; without the memory the doIt just has no entry.
  try {
    auto entry = std::make_unique<Session::MethodSource>();
    entry->method = method.slot;
    entry->doItText.assign(text, kDoItPrefix, std::string::npos);
    entry->sourceOffset = kDoItPrefix;
    attachBlocks(session, *entry, image);
    rootEntry(session.roots, *entry);
    session.doItDebug = std::move(entry);
  } catch (const std::bad_alloc&) {
  }

  *ran = true;
  Root result(session.roots);
  if (g_debugMode == AO_DEBUG_LIVE && session.scheduler != nullptr) {
    // SPEC §3.13 評価プロセス: the doIt runs on its own process; the base waits for it.
    Scheduler& sched = *session.scheduler;
    const std::uint64_t pid = sched.forkEval(*session.ctx, method.slot, mode);
    if (pid == 0) {
      blankOut(out, outLen);
      return AO_ERR_EVAL;
    }
    if (sched.awaitEval(pid) != Scheduler::EvalEnd::Finished) {
      *failure = sched.evalReason();
      blankOut(out, outLen);
      return AO_ERR_EVAL;
    }
    result.slot = sched.evalValue();
    sched.clearEvalValue();
  } else {
    result.slot = applyMethod(*session.ctx, method.slot, Oop::nil(), nullptr, 0, Oop::nil());
  }
  if (result.slot.isEmpty()) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }
  return answerEval(session, mode, result.slot, out, outLen, inspect, inspectUser, printedOut);
}

}  // namespace

int sessionEval(const char* source, int sourceLen, int mode, char* out, int outLen, AoSpan* err,
                AoInspectFn inspect, void* inspectUser) {
  // SPEC §3.10 評価結果: an ao_eval past the busy check starts with an empty result, so a hook
  // that reads it during this evaluation, or any outcome that does not put one in, sees empty.
  if (g_session != nullptr) {
    g_session->evalResult = std::string();
    // SPEC §3.10, §3.13: the last doIt's entry and the snapshot go when an ao_eval starts.
    dropDoItDebug(*g_session);
    sessionDebugClear();
  }
  // 評価の前に立っていたフラグ（accept や file-in の途中のもの）を、この評価のせいにしない。
  if (g_session != nullptr && g_session->ctx != nullptr) {
    g_session->heap.clearOutOfMemory();
    clearUnwinding(*g_session->ctx);
    refreshStackLimit(*g_session->ctx);
    // SPEC §3.13: a cleanup an escaped C++ exception left counted must not stop capture for good.
    g_session->ctx->abortSetAside = 0;
  }
  bool ran = false;
  std::optional<std::string> printed = std::string();
  std::string failure;
  const int rc = evalBody(source, sourceLen, mode, out, outLen, err, inspect, inspectUser, &ran,
                          &printed, &failure);
  return finishEval(rc, ran, std::move(printed), failure, out, outLen, err);
}

// SPEC §3.4, §3.10: the end of an evaluation, once its answer is made (rc, out, *printed): the
// base's abort (or the live evaluation's `failure`) is read and cleared, the ready queue drains
// when the evaluation ran, and the result is kept.
int finishEval(int rc, bool ran, std::optional<std::string> printed, const std::string& failure,
               char* out, int outLen, AoSpan* err) {
  if (g_session == nullptr || g_session->ctx == nullptr) {
    return rc;
  }
  // SPEC §3.4: abort は最外で理由を読んで消す。SPEC §3.2: old の上限で割り当てられず、それが
  // abort にならずに走り切ったときも「out of memory」。SPEC §3.10: AO_ERR_EVAL の理由は空にしない。
  CallContext& ctx = *g_session->ctx;
  std::string reason;
  if (!failure.empty()) {
    reason = failure;
  } else if (ctx.aborting) {
    reason = abortReasonText(ctx);
    if (reason.empty()) {
      reason = "evaluation aborted";
    }
  } else if (g_session->heap.outOfMemory()) {
    reason = "out of memory";
  } else if (rc == AO_ERR_EVAL) {
    reason = "evaluation failed";
  }
  clearUnwinding(ctx);
  // SPEC §3.4 評価の終わり: the answer (out, the reason) is fixed and the base's abort is read and
  // cleared; then the ready queue drains. Nothing in the drain changes the answer, and what it
  // writes to the Transcript reaches the hook before this returns.
  if (ran && g_session->scheduler != nullptr) {
    g_session->scheduler->drain(Scheduler::kDrainRounds);
  }
  if (reason.empty()) {
    // SPEC §3.10 評価結果: kept after the drain, and only when ao_eval answers AO_OK or
    // AO_ERR_RANGE. A Do it leaves printed empty.
    if (rc == AO_OK || rc == AO_ERR_RANGE) {
      g_session->evalResult = std::move(printed);
    }
    return rc;
  }
  g_session->heap.clearOutOfMemory();
  blankOut(out, outLen);
  if (err != nullptr) {
    const std::size_t n = std::min(reason.size(), sizeof(err->message) - 1);
    err->start = 0;
    err->end = 0;
    std::memcpy(err->message, reason.data(), n);
    err->message[n] = '\0';
  }
  return AO_ERR_EVAL;
}

int sessionEvalResultLength() {
  if (g_session == nullptr || !g_session->evalResult.has_value()) {
    return -1;
  }
  // Only a result shorter than INT_MAX bytes is kept (evalBody).
  return static_cast<int>(g_session->evalResult->size());
}

int sessionEvalResultCopy(char* buf, int bufLen) {
  if (g_session == nullptr || !g_session->evalResult.has_value()) {
    blankOut(buf, bufLen);
    return AO_ERR;
  }
  return writeBuf(*g_session->evalResult, buf, bufLen);
}

void rememberMethodSource(Oop method, Oop text, Oop replaced, const compiler::MethodImage* image) {
  Session* s = session();
  if (s == nullptr || !method.isHeap() || !text.isHeap()) {
    return;
  }
  // SPEC §3.10: the replaced method's entry goes with its blocks. So does an entry `method`
  // already had.
  for (const Oop gone : {replaced, method}) {
    if (!gone.isHeap()) {
      continue;
    }
    for (auto it = s->methodSources.begin(); it != s->methodSources.end(); ++it) {
      if ((*it)->method == gone) {
        unrootEntry(s->roots, **it);
        s->methodSources.erase(it);
        break;
      }
    }
  }
  // Nothing below collects: `method` and `text` stay valid until they are in the rooted slots.
  // SPEC §3.10: plain C++ memory and root slots. Without the memory the method, which is installed
  // already, has no entry (as the doIt), and the accept still succeeds: nothing is rooted before
  // the last step that can throw, and the table has room for the entry before it is rooted.
  try {
    auto entry = std::make_unique<Session::MethodSource>();
    entry->method = method;
    entry->text = text;
    if (image != nullptr) {
      attachBlocks(*s, *entry, *image);
    }
    s->methodSources.reserve(s->methodSources.size() + 1);
    rootEntry(s->roots, *entry);
    s->methodSources.push_back(std::move(entry));
  } catch (const std::bad_alloc&) {
  }
}

bool moveMethodSource(Oop from, Oop to, const compiler::MethodImage& image) {
  Session* s = session();
  if (s == nullptr || !from.isHeap() || !to.isHeap()) {
    return false;
  }
  for (auto& entry : s->methodSources) {
    if (entry->method != from) {
      continue;
    }
    // SPEC §3.9, §3.10: `to` was boxed from `image`, so its blocks and debug info come from there.
    // The new entry is built and rooted (all or nothing) before the old one lets go. Nothing here
    // collects, so `to` and the text stay valid until they are in the rooted slots.
    try {
      auto moved = std::make_unique<Session::MethodSource>();
      moved->method = to;
      moved->text = entry->text;
      moved->sourceOffset = entry->sourceOffset;
      attachBlocks(*s, *moved, image);
      rootEntry(s->roots, *moved);
      unrootEntry(s->roots, *entry);
      entry = std::move(moved);
    } catch (const std::bad_alloc&) {
      // Without the memory the entry still moves, with its text only. None of this allocates.
      for (auto it = entry->blocks.rbegin(); it != entry->blocks.rend(); ++it) {
        s->roots.remove(&*it);
      }
      entry->blocks.clear();
      entry->debug.reset();
      entry->method = to;
    }
    return true;
  }
  return false;
}

bool methodSource(Oop method, std::string& utf8) {
  Session* s = session();
  if (s == nullptr || !method.isHeap()) {
    return false;
  }
  std::uint32_t index = 0;
  const Session::MethodSource* entry = findEntry(*s, method, &index);
  if (entry == nullptr) {
    return false;
  }
  if (entry->text.isHeap()) {
    utf8 = Str::toUtf8(s->heap, entry->text);
    return true;
  }
  if (entry == s->doItDebug.get()) {
    utf8 = entry->doItText;
    return true;
  }
  return false;
}

DebugInfoRef debugInfoFor(Oop method) {
  DebugInfoRef ref;
  Session* s = session();
  if (s == nullptr || !method.isHeap()) {
    return ref;
  }
  std::uint32_t index = 0;
  const Session::MethodSource* entry = findEntry(*s, method, &index);
  if (entry == nullptr || entry->debug == nullptr || index >= entry->debug->bodies.size()) {
    return ref;
  }
  ref.source = entry;
  ref.body = &entry->debug->bodies[index];
  ref.index = index;
  return ref;
}

bool debugSpanAt(Oop method, std::uint32_t pc, std::uint32_t& start, std::uint32_t& end) {
  const DebugInfoRef ref = debugInfoFor(method);
  if (!ref) {
    return false;
  }
  const std::vector<compiler::PcSpan>& map = ref.body->pcMap;
  // Ascending by pc: the last entry at or before pc.
  auto it = std::upper_bound(map.begin(), map.end(), pc,
                             [](std::uint32_t p, const compiler::PcSpan& e) { return p < e.pc; });
  if (it == map.begin()) {
    return false;
  }
  --it;
  const std::uint32_t offset = ref.source->sourceOffset;
  start = it->start >= offset ? it->start - offset : 0;
  end = it->end >= offset ? it->end - offset : 0;
  return true;
}

std::vector<const Oop*> methodSourceRootSlots(bool withSnapshot) {
  std::vector<const Oop*> slots;
  if (Session* s = session()) {
    auto add = [&slots](const Session::MethodSource& entry) {
      slots.push_back(&entry.method);
      slots.push_back(&entry.text);
      for (const Oop& block : entry.blocks) {
        slots.push_back(&block);
      }
    };
    for (const auto& entry : s->methodSources) {
      add(*entry);
    }
    if (s->doItDebug != nullptr) {
      add(*s->doItDebug);
    }
    if (withSnapshot) {
      const std::vector<const Oop*> snapshot = s->debug.rootSlots();
      slots.insert(slots.end(), snapshot.begin(), snapshot.end());
    }
  }
  return slots;
}

void clearMethodSources() {
  if (Session* s = session()) {
    releaseMethodSources(*s);
  }
}

int browserClassCount() {
  Session* s = session();
  if (s == nullptr) {
    return kCountFailed;
  }
  return static_cast<int>(classRows(*s).size());
}

int browserClassAt(int index, char* name, int nameLen, char* category, int categoryLen) {
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  if (index < 0 || static_cast<std::size_t>(index) >= rows.size()) {
    return AO_ERR;
  }
  const ClassRow& row = rows[static_cast<std::size_t>(index)];
  const int nameRc = writeBuf(row.name, name, nameLen);
  const int categoryRc = writeBuf(row.category, category, categoryLen);
  if (nameRc == AO_ERR || categoryRc == AO_ERR) {
    return AO_ERR;
  }
  if (nameRc == AO_ERR_RANGE || categoryRc == AO_ERR_RANGE) {
    return AO_ERR_RANGE;
  }
  return AO_OK;
}

int browserProtocolCount(const char* className, int meta) {
  Session* s = session();
  if (s == nullptr || className == nullptr || !metaOk(meta)) {
    return kCountFailed;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return kCountFailed;
  }
  return static_cast<int>(protocolsOf(methodsOf(*s, sideOf(*s, row->cls, meta))).size());
}

int browserProtocolAt(const char* className, int meta, int index, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto protocols = protocolsOf(methodsOf(*s, sideOf(*s, row->cls, meta)));
  if (index < 0 || static_cast<std::size_t>(index) >= protocols.size()) {
    return AO_ERR;
  }
  return writeBuf(protocols[static_cast<std::size_t>(index)], buf, len);
}

int browserSelectorCount(const char* className, int meta, const char* protocol) {
  Session* s = session();
  if (s == nullptr || className == nullptr || protocol == nullptr || !metaOk(meta)) {
    return kCountFailed;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return kCountFailed;
  }
  if (!knownProtocol(protocol)) {
    return 0;
  }
  const auto methods = methodsOf(*s, sideOf(*s, row->cls, meta));
  return static_cast<int>(selectorsFor(methods, protocol).size());
}

int browserSelectorAt(const char* className, int meta, const char* protocol, int index, char* buf,
                      int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || protocol == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto selectors = selectorsFor(methodsOf(*s, sideOf(*s, row->cls, meta)), protocol);
  if (index < 0 || static_cast<std::size_t>(index) >= selectors.size()) {
    return AO_ERR;
  }
  return writeBuf(selectors[static_cast<std::size_t>(index)].selector, buf, len);
}

int browserSource(const char* className, int meta, const char* selector, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || selector == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  const std::string shownName = meta == 1 ? row->name + " class" : row->name;
  const auto methods = methodsOf(*s, sideOf(*s, row->cls, meta));
  for (const auto& method : methods) {
    if (method.selector == selector) {
      std::string text;
      if (sourceOf(*s, shownName, method, text)) {
        return writeBuf(text, buf, len);
      }
      // SPEC §3.10: AO_ERR_NOSOURCE wins over AO_ERR_RANGE; writeBuf still cuts and ends in NUL.
      return writeBuf(text, buf, len) == AO_ERR ? AO_ERR : AO_ERR_NOSOURCE;
    }
  }
  return AO_ERR;
}

int browserClassDefinition(const char* className, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  return writeBuf(definitionOf(*s, *row), buf, len);
}

int browserSuperclass(const char* className, int meta, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClass(rows, className);
  if (row == nullptr) {
    return AO_ERR;
  }
  return writeBuf(superclassName(*s, row->cls, meta), buf, len);
}

int browserSubclassCount(const char* className) {
  Session* s = session();
  if (s == nullptr || className == nullptr) {
    return kCountFailed;
  }
  const auto rows = classRows(*s);
  if (findClass(rows, className) == nullptr) {
    return kCountFailed;
  }
  return static_cast<int>(subclassNames(*s, className, rows).size());
}

int browserSubclassAt(const char* className, int index, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || className == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  if (findClass(rows, className) == nullptr) {
    return AO_ERR;
  }
  const auto names = subclassNames(*s, className, rows);
  if (index < 0 || static_cast<std::size_t>(index) >= names.size()) {
    return AO_ERR;
  }
  return writeBuf(names[static_cast<std::size_t>(index)], buf, len);
}

namespace {

// The snapshot of the current session; null without one.
DebugSnapshot* snapshotOf(Session* s) { return s != nullptr ? &s->debug : nullptr; }

bool frameInRange(const DebugSnapshot& snap, int i) {
  return i >= 0 && static_cast<std::uint32_t>(i) < snap.count();
}

// A CompiledMethod's class and selector, or the doIt's. The doIt is boxed with CompiledMethod as
// its class and doIt as its selector (evalBody); the last one is also known by its source entry.
struct MethodName {
  std::string cls;
  std::string selector;
  bool doIt = false;
};

MethodName methodNameOf(Session& s, Oop method) {
  MethodName name;
  if (!pointerSlots(s.heap, method, kCmSlotCount)) {
    return name;
  }
  const Oop cls = s.heap.slotAt(method, kCmSlotMethodClass);
  name.selector = byteText(s.heap, s.heap.slotAt(method, kCmSlotSelector));
  name.doIt = (s.doItDebug != nullptr && s.doItDebug->method == method) ||
              (cls == s.wk.compiledMethodClass && name.selector == "doIt");
  if (!name.doIt) {
    // A metaclass is named "<Name> class" (SPEC §3.6), so this is the class side's label too.
    name.cls = classNameOf(s.heap, cls);
  }
  return name;
}

// "Foo>>bar", "Foo class>>bar" or "doIt" (SPEC §3.13).
std::string methodLabel(const MethodName& name) {
  return name.doIt ? std::string("doIt") : name.cls + ">>" + name.selector;
}

// The home method of block frame i: the method of its home context (the context ^ returns from),
// else the method whose source entry holds the block; the empty Oop when neither is known.
Oop homeMethodOf(Session& s, std::uint32_t i) {
  const Oop block = s.debug.context(i);
  if (pointerSlots(s.heap, block, kBlockSlotCount)) {
    const Oop home = s.heap.slotAt(block, kBlockHome);
    if (home.isHeap() && s.heap.klass(home) == s.wk.methodContextClass &&
        pointerSlots(s.heap, home, kCtxMethod + 1)) {
      const Oop method = s.heap.slotAt(home, kCtxMethod);
      if (method.isHeap() && s.heap.klass(method) == s.wk.compiledMethodClass) {
        return method;
      }
    }
  }
  std::uint32_t index = 0;
  if (const Session::MethodSource* entry = findEntry(s, s.debug.method(i), &index)) {
    return entry->method;
  }
  return Oop{};
}

// What frame i's label and placeholder name: its method, a block's home method, or (kind 2) the
// NativeMethod, nil for a DNU.
Oop labelMethodOf(Session& s, std::uint32_t i) {
  return s.debug.kind(i) == kDebugFrameBlock ? homeMethodOf(s, i) : s.debug.method(i);
}

std::string frameLabel(Session& s, std::uint32_t i) {
  const Oop method = labelMethodOf(s, i);
  switch (s.debug.kind(i)) {
    case kDebugFrameMethod:
      return methodLabel(methodNameOf(s, method));
    case kDebugFrameBlock:
      // No home known: the block's own class, with no selector.
      return "[] in " + methodLabel(methodNameOf(s, method.isEmpty() ? s.debug.method(i) : method));
    default: {
      const std::string selector = byteText(s.heap, s.debug.selector(i));
      if (!pointerSlots(s.heap, method, kNativeSlotCount)) {
        return "#" + selector + " (doesNotUnderstand:)";
      }
      const std::string_view sym = NativeMethod::nameBytes(s.heap, method);
      return classNameOf(s.heap, s.heap.slotAt(method, kNativeSlotMethodClass)) + ">>" + selector +
             " native " + std::string(sym);
    }
  }
}

// The debug info of interpreted frame i; null for kind 2 or a method without it.
const MethodDebugInfo* frameDebugInfo(const Session& s, std::uint32_t i) {
  if (s.debug.kind(i) == kDebugFrameNative) {
    return nullptr;
  }
  return debugInfoFor(s.debug.method(i)).body;
}

// The argument count in the header of the CompiledMethod `method`; 0 when it is not one.
std::uint32_t argCountOf(Session& s, Oop method) {
  if (!pointerSlots(s.heap, method, kCmSlotCount)) {
    return 0;
  }
  const Oop header = s.heap.slotAt(method, kCmSlotHeader);
  return header.isSmallInteger() ? static_cast<std::uint32_t>(header.smallIntegerValue() & 0xFF)
                                 : 0;
}

std::uint32_t frameTempCount(const Session& s, std::uint32_t i) {
  if (const MethodDebugInfo* info = frameDebugInfo(s, i)) {
    return static_cast<std::uint32_t>(info->temps.size());
  }
  return s.debug.tempCount(i);
}

bool tempInRange(const Session& s, int i, int j) {
  return frameInRange(s.debug, i) && j >= 0 &&
         static_cast<std::uint32_t>(j) < frameTempCount(s, static_cast<std::uint32_t>(i));
}

// The receiver (j == -1) or temp j of frame i; a named temp is read through the temp vector that
// holds it (SPEC §3.8). False when i or j is out of range. Only reads (no GC).
bool frameValue(Session& s, int i, int j, Oop* value) {
  if (!frameInRange(s.debug, i) || j < -1) {
    return false;
  }
  const auto fi = static_cast<std::uint32_t>(i);
  if (j == -1) {
    *value = s.debug.receiver(fi);
    return true;
  }
  if (!tempInRange(s, i, j)) {
    return false;
  }
  const auto tj = static_cast<std::uint32_t>(j);
  const MethodDebugInfo* info = frameDebugInfo(s, fi);
  Oop v = s.debug.temp(fi, tj);
  if (info != nullptr) {
    const compiler::TempName& name = info->temps[tj];
    v = s.debug.temp(fi, name.slot);
    if (name.vecIndex >= 0) {
      const auto at = static_cast<std::uint32_t>(name.vecIndex);
      v = pointerSlots(s.heap, v, at + 1) ? s.heap.slotAt(v, at) : Oop::nil();
    }
  }
  *value = v.isEmpty() ? Oop::nil() : v;
  return true;
}

// SPEC §3.4, §3.10: one outermost ao_debug_* entry that sends printString (and inspect). It clears
// what an earlier entry left and takes the stack range again; while it runs the base has no debug
// sink (the scheduler copies the base's into the processes), so its own abort does not replace the
// snapshot. The end reads and clears the abort, without a drain, and puts the sink back as the
// capture setting then says.
class DebugEntry {
 public:
  explicit DebugEntry(Session& s) : s_(s) {
    CallContext& ctx = *s_.ctx;
    s_.heap.clearOutOfMemory();
    clearUnwinding(ctx);
    refreshStackLimit(ctx);
    ctx.debug = nullptr;
    ++g_debugEntries;
  }
  ~DebugEntry() {
    CallContext& ctx = *s_.ctx;
    clearUnwinding(ctx);
    s_.heap.clearOutOfMemory();
    // The capture setting as the ABI last set it; a hook may have changed it meanwhile.
    if (--g_debugEntries == 0) {
      ctx.debug = g_debugCapture ? &s_ : nullptr;
    }
  }
  DebugEntry(const DebugEntry&) = delete;
  DebugEntry& operator=(const DebugEntry&) = delete;
  // Its sends aborted or ran out of memory.
  bool failed() const { return unwinding(*s_.ctx) || s_.heap.outOfMemory(); }

 private:
  Session& s_;
};

int debugPrint(int i, int j, char* classBuf, int classLen, char* buf, int len) {
  Session* s = session();
  Oop v;
  if (s == nullptr || s->ctx == nullptr || classBuf == nullptr || classLen < 1 ||
      buf == nullptr || len < 1 || !frameValue(*s, i, j, &v)) {
    blankOut(classBuf, classLen);
    blankOut(buf, len);
    return AO_ERR;
  }
  Root value(s->roots, v);
  const std::string cls = classNameOf(s->heap, s->wk.classOf(value.slot));
  std::string utf8;
  {
    const DebugEntry entry(*s);
    // SPEC §3.10: a printString that aborts leaves the print empty; the class name still shows.
    if (!printStringOf(*s, value.slot, &utf8) || entry.failed()) {
      utf8.clear();
    }
  }
  const int classRc = writeBuf(cls, classBuf, classLen);
  const int printRc = writeBuf(utf8, buf, len);
  return classRc == AO_OK && printRc == AO_OK ? AO_OK : AO_ERR_RANGE;
}

}  // namespace

int debugFrameCount() {
  const DebugSnapshot* snap = snapshotOf(session());
  return snap != nullptr ? static_cast<int>(snap->count()) : -1;
}

int debugFrameTotal() {
  const DebugSnapshot* snap = snapshotOf(session());
  return snap != nullptr ? static_cast<int>(snap->total()) : -1;
}

int debugReason(char* buf, int len) {
  const DebugSnapshot* snap = snapshotOf(session());
  if (snap == nullptr || snap->empty()) {
    blankOut(buf, len);
    return AO_ERR;
  }
  return writeBuf(snap->reason(), buf, len);
}

int debugFrameKind(int i) {
  const DebugSnapshot* snap = snapshotOf(session());
  return snap != nullptr && frameInRange(*snap, i) ? snap->kind(static_cast<std::uint32_t>(i)) : -1;
}

int debugFrameLabel(int i, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || !frameInRange(s->debug, i)) {
    blankOut(buf, len);
    return AO_ERR;
  }
  return writeBuf(frameLabel(*s, static_cast<std::uint32_t>(i)), buf, len);
}

int debugFramePc(int i) {
  const DebugSnapshot* snap = snapshotOf(session());
  if (snap == nullptr || !frameInRange(*snap, i)) {
    return -1;
  }
  const auto fi = static_cast<std::uint32_t>(i);
  if (snap->kind(fi) == kDebugFrameNative) {
    return -1;
  }
  return static_cast<int>(std::min<std::uint32_t>(snap->pc(fi), INT_MAX));
}

int debugFrameSource(int i, char* buf, int len, AoSpan* highlight) {
  if (highlight != nullptr) {
    highlight->start = 0;
    highlight->end = 0;
    highlight->message[0] = '\0';
  }
  Session* s = session();
  if (s == nullptr || !frameInRange(s->debug, i) || buf == nullptr || len < 1) {
    blankOut(buf, len);
    return AO_ERR;
  }
  const auto fi = static_cast<std::uint32_t>(i);
  const int kind = s->debug.kind(fi);
  std::string text;
  if (kind != kDebugFrameNative) {
    const Oop method = s->debug.method(fi);
    // The entry that holds the method or the block has the home method's text, the doIt's too.
    if (methodSource(method, text)) {
      std::uint32_t start = 0;
      std::uint32_t end = 0;
      if (highlight != nullptr && debugSpanAt(method, s->debug.pc(fi), start, end)) {
        highlight->start = start;
        highlight->end = end;
      }
      return writeBuf(text, buf, len);
    }
  }
  // SPEC §3.10: the browser's placeholder (sourceOf) for a method or a native; the doIt and the
  // DNU frame take the same shape with their labels.
  const Oop method = labelMethodOf(*s, fi);
  const MethodName name = methodNameOf(*s, method);
  if (kind == kDebugFrameNative && pointerSlots(s->heap, method, kNativeSlotCount)) {
    const std::string cls = classNameOf(s->heap, s->heap.slotAt(method, kNativeSlotMethodClass));
    sourceOf(*s, cls, ListedMethod{byteText(s->heap, s->debug.selector(fi)), true, method}, text);
  } else if (kind != kDebugFrameNative && !method.isEmpty() && !name.doIt) {
    sourceOf(*s, name.cls, ListedMethod{name.selector, false, method}, text);
  } else {
    const std::string label = kind == kDebugFrameNative ? frameLabel(*s, fi) : methodLabel(name);
    text = "\"" + label + " source not available\"";
  }
  // SPEC §3.10: AO_ERR_NOSOURCE wins over AO_ERR_RANGE; writeBuf still cuts and ends in NUL.
  writeBuf(text, buf, len);
  return AO_ERR_NOSOURCE;
}

int debugTempCount(int i) {
  const Session* s = session();
  if (s == nullptr || !frameInRange(s->debug, i)) {
    return -1;
  }
  return static_cast<int>(frameTempCount(*s, static_cast<std::uint32_t>(i)));
}

int debugTempName(int i, int j, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || !tempInRange(*s, i, j)) {
    blankOut(buf, len);
    return AO_ERR;
  }
  const auto fi = static_cast<std::uint32_t>(i);
  const auto tj = static_cast<std::uint32_t>(j);
  if (const MethodDebugInfo* info = frameDebugInfo(*s, fi)) {
    return writeBuf(info->temps[tj].name, buf, len);
  }
  // SPEC §3.10: without debug info, arg1 ... then t1 ...; a native frame's are all arguments.
  const std::uint32_t args = s->debug.kind(fi) == kDebugFrameNative
                                 ? s->debug.tempCount(fi)
                                 : argCountOf(*s, s->debug.method(fi));
  const std::string name =
      tj < args ? "arg" + std::to_string(tj + 1) : "t" + std::to_string(tj - args + 1);
  return writeBuf(name, buf, len);
}

int debugReceiverPrint(int i, char* classBuf, int classLen, char* buf, int len) {
  return debugPrint(i, -1, classBuf, classLen, buf, len);
}

int debugTempPrint(int i, int j, char* classBuf, int classLen, char* buf, int len) {
  if (j < 0) {
    blankOut(classBuf, classLen);
    blankOut(buf, len);
    return AO_ERR;
  }
  return debugPrint(i, j, classBuf, classLen, buf, len);
}

int debugInspect(int i, int j, AoInspectFn inspect, void* inspectUser) {
  Session* s = session();
  Oop v;
  if (s == nullptr || s->ctx == nullptr || !frameValue(*s, i, j, &v)) {
    return AO_ERR;
  }
  Root value(s->roots, v);
  const DebugEntry entry(*s);
  std::string utf8;
  // SPEC §3.10: as Inspect it; an abort in inspect or printString calls no hook.
  const bool ok = inspectValue(*s, value.slot, inspect, inspectUser, &utf8);
  return ok && !entry.failed() ? AO_OK : AO_ERR;
}

int debugClear() {
  if (session() == nullptr) {
    return AO_ERR;
  }
  sessionDebugClear();
  return AO_OK;
}

}  // namespace ao

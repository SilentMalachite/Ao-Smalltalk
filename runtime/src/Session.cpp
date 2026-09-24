#include "Session.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/ClassPool.hpp"
#include "ao/Compile.hpp"
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
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ao {
namespace {

std::unique_ptr<Session> g_session;

Oop workspaceBinding(CallContext& ctx, std::string_view name);

void installEmptyCache(Session& session, HostOopHook transcript, HostOopHook inspect) {
  session.cache = std::make_unique<ClassMethodCache>();
  session.cache->addRoots(session.roots);
  session.ctx = std::unique_ptr<CallContext>(new CallContext{
      session.heap, session.roots, session.wk, session.cache.get()});
  session.ctx->transcriptHook = transcript;
  session.ctx->inspectHook = inspect;
  session.ctx->bindingHook = workspaceBinding;
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

void unrootMethodSources(Session& session) {
  for (auto& pair : session.methodSources) {
    session.roots.remove(&pair->method);
    session.roots.remove(&pair->text);
  }
}

void rerootMethodSources(Session& session) {
  for (auto& pair : session.methodSources) {
    session.roots.add(&pair->method);
    session.roots.add(&pair->text);
  }
}

void releaseMethodSources(Session& session) {
  unrootMethodSources(session);
  session.methodSources.clear();
}

}  // namespace

Session::Session(bool bootstrap) : wk(heap, roots) {
  if (!bootstrap) {
    return;
  }
  installEmptyCache(*this, nullptr, nullptr);
  Bootstrap::run(heap, roots, wk);
}

Session::~Session() { releaseMethodSources(*this); }

Session* session() { return g_session.get(); }

int sessionBoot() {
  if (g_session != nullptr) {
    return 1;
  }
  g_session = std::make_unique<Session>(true);
  if (!installEmptyWorkspace(*g_session)) {
    g_session.reset();
    return 1;
  }
  clearMethodSources();
  return 0;
}

int sessionShutdown() {
  g_session.reset();
  return 0;
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
  // Source text is session state. Drop the roots so Image::save does not trace it.
  struct HideMethodSources {
    Session* session = nullptr;
    explicit HideMethodSources(Session* s) : session(s) {
      if (session != nullptr) {
        unrootMethodSources(*session);
      }
    }
    ~HideMethodSources() {
      if (session != nullptr) {
        rerootMethodSources(*session);
      }
    }
  } hideSources(g_session.get());
  return Image::save(g_session->heap, g_session->roots, g_session->wk, path) ? 0 : 1;
}

int sessionImageLoad(const char* path) {
  if (g_session == nullptr || path == nullptr) {
    return 1;
  }
  auto next = std::make_unique<Session>(false);
  if (!Image::load(next->heap, next->roots, next->wk, path)) {
    return 1;
  }
  HostOopHook transcript = nullptr;
  HostOopHook inspect = nullptr;
  if (g_session->ctx != nullptr) {
    transcript = g_session->ctx->transcriptHook;
    inspect = g_session->ctx->inspectHook;
  }
  installEmptyCache(*next, transcript, inspect);
  if (!installEmptyWorkspace(*next)) {
    return 1;
  }
  // SPEC §3.10: the natives and the probes run on the new session. Only when both pass does it
  // replace the current one; otherwise the current session stays as it was.
  ensureKernelNatives(*next);
  if (!loadedImageProbes(*next)) {
    return 1;
  }
  g_session = std::move(next);
  clearMethodSources();
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
// order, one blank apart.
std::string classVarList(Heap& heap, Oop cls) {
  if (!pointerSlots(heap, cls, kClassSlotClassPool + 1)) {
    return {};
  }
  std::string out;
  for (const std::string& name : ClassPool::names(heap, heap.slotAt(cls, kClassSlotClassPool))) {
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
  text += classVarList(s.heap, row.cls);
  text += "'\n  poolDictionaries: ''\n  category: '";
  text += definitionCategory(s.heap, row.cls);
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

int evalBody(const char* source, int sourceLen, int mode, char* out, int outLen, AoSpan* err,
             AoInspectFn inspect, void* inspectUser) {
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
      constexpr unsigned kDoItPrefix = 5;
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

  Root result(session.roots,
              applyMethod(*session.ctx, method.slot, Oop::nil(), nullptr, 0, Oop::nil()));
  if (result.slot.isEmpty()) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }

  if (mode == AO_EVAL_DOIT) {
    return writeBuf("", out, outLen);
  }

  if (mode == AO_EVAL_INSPECTIT) {
    const Oop sel = session.wk.intern("inspect");
    if (!sel.isHeap()) {
      blankOut(out, outLen);
      return AO_ERR_EVAL;
    }
    if (send(*session.ctx, result.slot, sel, nullptr, 0, nullptr).isEmpty()) {
      blankOut(out, outLen);
      return AO_ERR_EVAL;
    }
  }

  const Oop printSel = session.wk.intern("printString");
  if (!printSel.isHeap()) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }
  Root printed(session.roots,
                   send(*session.ctx, result.slot, printSel, nullptr, 0, nullptr));
  if (!printed.slot.isHeap() || (session.heap.flags(printed.slot) & kFlagBytes) == 0) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }
  const std::string utf8 = Str::toUtf8(session.heap, printed.slot);
  // out of memory になった評価は、エラーだけを返す（文言は sessionEval が入れる）。Inspector を
  // 開かないよう、フックより先に判定する。
  if (session.heap.outOfMemory()) {
    blankOut(out, outLen);
    return AO_ERR_EVAL;
  }
  if (mode == AO_EVAL_INSPECTIT && inspect != nullptr) {
    const std::string cls = classNameOf(session.heap, session.wk.classOf(result.slot));
    inspect(cls.c_str(), utf8.c_str(), inspectUser);
  }
  return writeBuf(utf8, out, outLen);
}

}  // namespace

int sessionEval(const char* source, int sourceLen, int mode, char* out, int outLen, AoSpan* err,
                AoInspectFn inspect, void* inspectUser) {
  // 評価の前に立っていたフラグ（accept や file-in の途中のもの）を、この評価のせいにしない。
  if (g_session != nullptr && g_session->ctx != nullptr) {
    g_session->heap.clearOutOfMemory();
    clearUnwinding(*g_session->ctx);
    refreshStackLimit(*g_session->ctx);
  }
  const int rc = evalBody(source, sourceLen, mode, out, outLen, err, inspect, inspectUser);
  if (g_session == nullptr || g_session->ctx == nullptr) {
    return rc;
  }
  // SPEC §3.4: abort は最外で理由を読んで消す。SPEC §3.2: old の上限で割り当てられず、それが
  // abort にならずに走り切ったときも「out of memory」。SPEC §3.10: AO_ERR_EVAL の理由は空にしない。
  CallContext& ctx = *g_session->ctx;
  std::string reason;
  if (ctx.aborting) {
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
  if (reason.empty()) {
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

void rememberMethodSource(Oop method, Oop text, Oop replaced) {
  Session* s = session();
  if (s == nullptr || !method.isHeap() || !text.isHeap()) {
    return;
  }
  if (replaced.isHeap()) {
    for (auto it = s->methodSources.begin(); it != s->methodSources.end(); ++it) {
      if ((*it)->method == replaced) {
        s->roots.remove(&(*it)->method);
        s->roots.remove(&(*it)->text);
        s->methodSources.erase(it);
        break;
      }
    }
  }
  for (auto& pair : s->methodSources) {
    if (pair->method == method) {
      s->roots.remove(&pair->text);
      pair->text = text;
      s->roots.add(&pair->text);
      return;
    }
  }
  auto pair = std::make_unique<Session::MethodSource>();
  pair->method = method;
  pair->text = text;
  s->roots.add(&pair->method);
  s->roots.add(&pair->text);
  s->methodSources.push_back(std::move(pair));
}

bool moveMethodSource(Oop from, Oop to) {
  Session* s = session();
  if (s == nullptr || !from.isHeap() || !to.isHeap()) {
    return false;
  }
  for (auto& pair : s->methodSources) {
    if (pair->method == from) {
      // The slot itself is the root, so the new method is rooted as soon as it is stored.
      pair->method = to;
      return true;
    }
  }
  return false;
}

bool methodSource(Oop method, std::string& utf8) {
  Session* s = session();
  if (s == nullptr || !method.isHeap()) {
    return false;
  }
  for (const auto& pair : s->methodSources) {
    if (pair->method == method && pair->text.isHeap()) {
      utf8 = Str::toUtf8(s->heap, pair->text);
      return true;
    }
  }
  return false;
}

std::vector<const Oop*> methodSourceRootSlots() {
  std::vector<const Oop*> slots;
  if (Session* s = session()) {
    slots.reserve(2 * s->methodSources.size());
    for (const auto& pair : s->methodSources) {
      slots.push_back(&pair->method);
      slots.push_back(&pair->text);
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

}  // namespace ao

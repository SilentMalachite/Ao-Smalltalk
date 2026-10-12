#include "ChangeSet.hpp"

#include "Session.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Runtime.hpp"
#include "ao/Send.hpp"

#include <cstdint>
#include <ctime>
#include <fstream>
#include <initializer_list>
#include <string>
#include <string_view>

namespace ao {
namespace {

constexpr const char* kFailed = "change set failed";
constexpr const char* kUnavailable = "change sets unavailable";
constexpr const char* kUnknown = "unknown change set";

// SPEC §3.10 チェンジセット (P17): one outermost stretch of sends to the model on the session's
// base context. It clears what an earlier entry left and takes the stack range again; while it
// runs the base has no debug sink and may not halt, so a failure in the model neither replaces
// the snapshot nor stops a process (as DebugEntry: the sink goes back as the capture setting then
// says, not as it was at the start). The end reads and clears the abort. After the
// first failed send the others answer the empty Oop without sending.
class ModelSend {
 public:
  explicit ModelSend(Session& s) : s_(s), ctx_(*s.ctx) {
    s_.heap.clearOutOfMemory();
    clearUnwinding(ctx_);
    refreshStackLimit(ctx_);
    beginDebugFreeEntry(s_);
    ++ctx_.haltSuppressed;
  }
  ~ModelSend() {
    clearUnwinding(ctx_);
    s_.heap.clearOutOfMemory();
    --ctx_.haltSuppressed;
    endDebugFreeEntry(s_);
  }
  ModelSend(const ModelSend&) = delete;
  ModelSend& operator=(const ModelSend&) = delete;

  // receiver and each arg are rooted slots (or immediates), read after the selector is interned
  // (which may collect). The answer, unrooted, or the empty Oop when the send aborted or ran out
  // of memory.
  Oop send(const Oop& receiver, std::string_view selector,
           std::initializer_list<const Oop*> args = {}) {
    if (failed_) {
      return Oop{};
    }
    const Oop sel = s_.wk.intern(selector);
    if (!sel.isHeap() || args.size() > kMaxArgs) {
      failed_ = true;
      return Oop{};
    }
    Oop argv[kMaxArgs];
    std::uint32_t n = 0;
    for (const Oop* a : args) {
      argv[n++] = *a;
    }
    const Oop answer = ao::send(ctx_, receiver, sel, argv, n, nullptr);
    if (answer.isEmpty() || unwinding(ctx_) || s_.heap.outOfMemory()) {
      failed_ = true;
      clearUnwinding(ctx_);
      return Oop{};
    }
    return answer;
  }
  // A String of text (may collect), unrooted; the empty Oop and failed() when it cannot be made.
  Oop string(std::string_view text) {
    const Oop str = failed_ ? Oop{} : Str::fromUtf8(ctx_, text);
    failed_ = failed_ || !str.isHeap();
    return str;
  }
  bool failed() const { return failed_; }

 private:
  static constexpr std::size_t kMaxArgs = 4;
  Session& s_;
  CallContext& ctx_;
  bool failed_ = false;
};

// SPEC §3.10 (P17): the ChangeSet class Smalltalk binds; the empty Oop when there is none.
// Allocates nothing.
Oop modelClass(Session& s) { return boundClassNamed(s, "ChangeSet"); }

// The session for an ABI body: null without a session or its base context.
Session* changesSession() {
  Session* s = session();
  return s == nullptr || s->ctx == nullptr ? nullptr : s;
}

Oop boolean(bool b) { return b ? Oop::true_() : Oop::false_(); }

// v's bytes into *out when it is a byte object (a String or a Symbol).
bool bytesText(Session& s, Oop v, std::string* out) {
  if (!v.isHeap() || (s.heap.flags(v) & kFlagBytes) == 0) {
    return false;
  }
  *out = Str::toUtf8(s.heap, v);
  return true;
}

// The ABI's AO_CHANGE_* for the model's kind (#added, #changed, #removed, nil). Allocates nothing.
int kindCode(Session& s, Oop kind) {
  if (kind.isHeap()) {
    if (kind == s.wk.findSymbol("added")) {
      return AO_CHANGE_ADDED;
    }
    if (kind == s.wk.findSymbol("changed")) {
      return AO_CHANGE_CHANGED;
    }
    if (kind == s.wk.findSymbol("removed")) {
      return AO_CHANGE_REMOVED;
    }
  }
  return AO_CHANGE_NONE;
}

// A count the model answered, or -1 when it failed or is no SmallInteger.
int countOf(const ModelSend& m, Oop n) {
  if (m.failed() || !n.isSmallInteger() || n.smallIntegerValue() < 0 ||
      n.smallIntegerValue() > INT32_MAX) {
    return -1;
  }
  return static_cast<int>(n.smallIntegerValue());
}

// The set with setId into set.slot (nil when the model has none: an ID no SmallInteger can be
// is no set's). False when the send failed.
bool setWithId(ModelSend& m, const Oop& cls, std::int64_t setId, Root& set) {
  if (setId <= 0 || setId > kSmiMax) {
    set.slot = Oop::nil();
    return true;
  }
  const Oop id = Oop::fromSmallInteger(setId);
  set.slot = m.send(cls, "withId:", {&id});
  return !m.failed();
}

// The element at the 0-based index of the rooted collection into out.slot, after checking index
// against its size. False when out of range or a send failed.
bool elementAt(ModelSend& m, const Oop& collection, int index, Root& out) {
  const int n = countOf(m, m.send(collection, "size"));
  if (n < 0 || index < 0 || index >= n) {
    return false;
  }
  const Oop at = Oop::fromSmallInteger(index + 1);
  out.slot = m.send(collection, "at:", {&at});
  return !m.failed();
}

// SPEC §3.9 記録: the session whose heap ctx runs on, when className is not empty; null otherwise.
Session* recordingSession(CallContext& ctx, const std::string& className) {
  Session* s = changesSession();
  if (s == nullptr || &ctx.heap != &s->heap || className.empty()) {
    return nullptr;
  }
  return s;
}

// The local date, YYYY-MM-DD.
std::string today() {
  const std::time_t now = std::time(nullptr);
  std::tm local{};
  localtime_r(&now, &local);
  char buf[16];
  return std::strftime(buf, sizeof buf, "%Y-%m-%d", &local) == 0 ? std::string()
                                                                  : std::string(buf);
}

// For the method reads: className's entry in the set into entry.slot and the selectors of its
// meta side, in byte order, into selectors.slot (both nil when the set has no entry for the
// class). False for what the reads refuse (an unknown set, a failed send).
bool methodSelectors(Session& s, ModelSend& m, const Oop& cls, std::int64_t setId,
                     const char* className, int meta, Root& entry, Root& selectors) {
  Root set(s.roots, Oop::nil());
  if (!setWithId(m, cls, setId, set) || set.slot.isNil()) {
    return false;
  }
  Root name(s.roots, m.string(className));
  entry.slot = m.send(set.slot, "changesFor:", {&name.slot});
  if (m.failed()) {
    return false;
  }
  if (entry.slot.isNil()) {
    selectors.slot = Oop::nil();
    return true;
  }
  const Oop side = boolean(meta == 1);
  selectors.slot = m.send(entry.slot, "selectorsMeta:", {&side});
  return !m.failed();
}

// A write's reason the model answered: AO_OK for nil, AO_ERR with its text otherwise (kFailed
// when it is no String).
int reasonOrOk(Session& s, Oop answer, std::string* reason) {
  if (answer.isNil()) {
    return AO_OK;
  }
  if (!bytesText(s, answer, reason) || reason->empty()) {
    *reason = kFailed;
  }
  return AO_ERR;
}

}  // namespace

void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept {
  try {
    Session* s = recordingSession(ctx, className);
    if (s == nullptr) {
      return;
    }
    Root cls(s->roots, modelClass(*s));
    if (!cls.slot.isHeap()) {
      return;
    }
    ModelSend m(*s);
    Root name(s->roots, m.string(className));
    Root sel(s->roots, m.string(selector));
    const Oop side = boolean(meta);
    const Oop existed = boolean(replaced);
    m.send(cls.slot, "noteMethodChange:selector:meta:existed:",
           {&name.slot, &sel.slot, &side, &existed});
  } catch (...) {
    // SPEC §3.9 記録: without the memory for it the record is not made; the Accept stands.
  }
}

void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept {
  try {
    Session* s = recordingSession(ctx, className);
    if (s == nullptr) {
      return;
    }
    Root cls(s->roots, modelClass(*s));
    if (!cls.slot.isHeap()) {
      return;
    }
    ModelSend m(*s);
    Root name(s->roots, m.string(className));
    Root sel(s->roots, m.string(selector));
    const Oop side = boolean(meta);
    m.send(cls.slot, "noteMethodRemoval:selector:meta:", {&name.slot, &sel.slot, &side});
  } catch (...) {
  }
}

void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept {
  try {
    Session* s = recordingSession(ctx, className);
    if (s == nullptr) {
      return;
    }
    Root cls(s->roots, modelClass(*s));
    if (!cls.slot.isHeap()) {
      return;
    }
    ModelSend m(*s);
    Root name(s->roots, m.string(className));
    const Oop was = boolean(existed);
    m.send(cls.slot, "noteClassChange:existed:", {&name.slot, &was});
  } catch (...) {
  }
}

void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept {
  try {
    Session* s = recordingSession(ctx, className);
    if (s == nullptr) {
      return;
    }
    Root cls(s->roots, modelClass(*s));
    if (!cls.slot.isHeap()) {
      return;
    }
    ModelSend m(*s);
    Root name(s->roots, m.string(className));
    m.send(cls.slot, "noteClassRemoval:", {&name.slot});
  } catch (...) {
  }
}

int sessionChangesCount() {
  Session* s = changesSession();
  if (s == nullptr) {
    return -1;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return 0;
  }
  ModelSend m(*s);
  Root all(s->roots, m.send(cls.slot, "allChangeSets"));
  return countOf(m, m.send(all.slot, "size"));
}

int sessionChangesAt(int index, std::int64_t* setId, char* name, int nameLen) {
  Session* s = changesSession();
  if (s == nullptr || index < 0 || name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return AO_ERR;
  }
  ModelSend m(*s);
  Root all(s->roots, m.send(cls.slot, "allChangeSets"));
  Root set(s->roots, Oop::nil());
  if (!elementAt(m, all.slot, index, set)) {
    return AO_ERR;
  }
  Root text(s->roots, m.send(set.slot, "name"));
  const Oop id = m.send(set.slot, "id");
  std::string utf8;
  if (m.failed() || !id.isSmallInteger() || !bytesText(*s, text.slot, &utf8)) {
    return AO_ERR;
  }
  if (setId != nullptr) {
    *setId = id.smallIntegerValue();
  }
  return writeBuf(utf8, name, nameLen);
}

std::int64_t sessionChangesCurrent() {
  Session* s = changesSession();
  if (s == nullptr) {
    return 0;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return 0;
  }
  ModelSend m(*s);
  Root set(s->roots, m.send(cls.slot, "current"));
  const Oop id = m.send(set.slot, "id");
  return m.failed() || !id.isSmallInteger() ? 0 : id.smallIntegerValue();
}

int sessionChangesNew(const char* name, std::int64_t* setId, std::string* reason) {
  Session* s = changesSession();
  if (s == nullptr) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    *reason = kUnavailable;
    return AO_ERR;
  }
  ModelSend m(*s);
  Root text(s->roots, m.string(name == nullptr ? "" : name));
  Root refusal(s->roots, m.send(cls.slot, "reasonToCreate:", {&text.slot}));
  if (m.failed()) {
    *reason = kFailed;
    return AO_ERR;
  }
  if (reasonOrOk(*s, refusal.slot, reason) != AO_OK) {
    return AO_ERR;
  }
  // SPEC §3.9 Change Sorter: a new set is current at once.
  Root set(s->roots, m.send(cls.slot, "newNamed:", {&text.slot}));
  const Oop id = m.send(set.slot, "id");
  if (m.failed() || !id.isSmallInteger()) {
    *reason = kFailed;
    return AO_ERR;
  }
  if (setId != nullptr) {
    *setId = id.smallIntegerValue();
  }
  return AO_OK;
}

int sessionChangesSetCurrent(std::int64_t setId) {
  Session* s = changesSession();
  if (s == nullptr) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return AO_ERR;
  }
  ModelSend m(*s);
  Root set(s->roots, Oop::nil());
  if (!setWithId(m, cls.slot, setId, set) || set.slot.isNil()) {
    return AO_ERR;
  }
  m.send(set.slot, "makeCurrent");
  return m.failed() ? AO_ERR : AO_OK;
}

int sessionChangesClassCount(std::int64_t setId) {
  Session* s = changesSession();
  if (s == nullptr) {
    return -1;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return -1;
  }
  ModelSend m(*s);
  Root set(s->roots, Oop::nil());
  if (!setWithId(m, cls.slot, setId, set) || set.slot.isNil()) {
    return -1;
  }
  Root classes(s->roots, m.send(set.slot, "classChanges"));
  return countOf(m, m.send(classes.slot, "size"));
}

int sessionChangesClassAt(std::int64_t setId, int index, char* name, int nameLen, int* kind) {
  Session* s = changesSession();
  if (s == nullptr || index < 0 || name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return AO_ERR;
  }
  ModelSend m(*s);
  Root set(s->roots, Oop::nil());
  if (!setWithId(m, cls.slot, setId, set) || set.slot.isNil()) {
    return AO_ERR;
  }
  Root classes(s->roots, m.send(set.slot, "classChanges"));
  Root entry(s->roots, Oop::nil());
  if (!elementAt(m, classes.slot, index, entry)) {
    return AO_ERR;
  }
  Root text(s->roots, m.send(entry.slot, "className"));
  const Oop definition = m.send(entry.slot, "definitionKind");
  std::string utf8;
  if (m.failed() || !bytesText(*s, text.slot, &utf8)) {
    return AO_ERR;
  }
  // Nothing allocates after the definition kind is answered.
  if (kind != nullptr) {
    *kind = kindCode(*s, definition);
  }
  return writeBuf(utf8, name, nameLen);
}

int sessionChangesMethodCount(std::int64_t setId, const char* className, int meta) {
  Session* s = changesSession();
  if (s == nullptr || className == nullptr || (meta != 0 && meta != 1)) {
    return -1;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return -1;
  }
  ModelSend m(*s);
  Root entry(s->roots, Oop::nil());
  Root selectors(s->roots, Oop::nil());
  if (!methodSelectors(*s, m, cls.slot, setId, className, meta, entry, selectors)) {
    return -1;
  }
  if (selectors.slot.isNil()) {
    return 0;
  }
  return countOf(m, m.send(selectors.slot, "size"));
}

int sessionChangesMethodAt(std::int64_t setId, const char* className, int meta, int index,
                           char* selector, int len, int* kind) {
  Session* s = changesSession();
  if (s == nullptr || className == nullptr || (meta != 0 && meta != 1) || index < 0 ||
      selector == nullptr || len < 1) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return AO_ERR;
  }
  ModelSend m(*s);
  Root entry(s->roots, Oop::nil());
  Root selectors(s->roots, Oop::nil());
  if (!methodSelectors(*s, m, cls.slot, setId, className, meta, entry, selectors) ||
      selectors.slot.isNil()) {
    return AO_ERR;
  }
  Root sel(s->roots, Oop::nil());
  if (!elementAt(m, selectors.slot, index, sel)) {
    return AO_ERR;
  }
  const Oop side = boolean(meta == 1);
  const Oop entryKind = m.send(entry.slot, "kindOf:meta:", {&sel.slot, &side});
  std::string utf8;
  if (m.failed() || !bytesText(*s, sel.slot, &utf8)) {
    return AO_ERR;
  }
  if (kind != nullptr) {
    *kind = kindCode(*s, entryKind);
  }
  return writeBuf(utf8, selector, len);
}

int sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector,
                       std::string* reason) {
  Session* s = changesSession();
  if (s == nullptr || className == nullptr || (selector != nullptr && meta != 0 && meta != 1)) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    *reason = kUnavailable;
    return AO_ERR;
  }
  ModelSend m(*s);
  Root set(s->roots, Oop::nil());
  if (!setWithId(m, cls.slot, setId, set)) {
    *reason = kFailed;
    return AO_ERR;
  }
  if (set.slot.isNil()) {
    *reason = kUnknown;
    return AO_ERR;
  }
  Root name(s->roots, m.string(className));
  if (selector == nullptr) {
    Root refusal(s->roots, m.send(set.slot, "reasonToRemoveClass:", {&name.slot}));
    if (!m.failed() && reasonOrOk(*s, refusal.slot, reason) != AO_OK) {
      return AO_ERR;
    }
    m.send(set.slot, "removeClass:", {&name.slot});
  } else {
    Root sel(s->roots, m.string(selector));
    const Oop side = boolean(meta == 1);
    Root refusal(s->roots, m.send(set.slot, "reasonToRemoveSelector:class:meta:",
                                  {&sel.slot, &name.slot, &side}));
    if (!m.failed() && reasonOrOk(*s, refusal.slot, reason) != AO_OK) {
      return AO_ERR;
    }
    m.send(set.slot, "removeSelector:class:meta:", {&sel.slot, &name.slot, &side});
  }
  if (m.failed()) {
    *reason = kFailed;
    return AO_ERR;
  }
  return AO_OK;
}

int sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta,
                       const char* selector, std::string* reason) {
  Session* s = changesSession();
  if (s == nullptr || className == nullptr || (selector != nullptr && meta != 0 && meta != 1)) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    *reason = kUnavailable;
    return AO_ERR;
  }
  ModelSend m(*s);
  Root from(s->roots, Oop::nil());
  Root to(s->roots, Oop::nil());
  if (!setWithId(m, cls.slot, fromId, from) || !setWithId(m, cls.slot, toId, to)) {
    *reason = kFailed;
    return AO_ERR;
  }
  if (from.slot.isNil() || to.slot.isNil()) {
    *reason = kUnknown;
    return AO_ERR;
  }
  Root name(s->roots, m.string(className));
  if (selector == nullptr) {
    Root refusal(s->roots, m.send(from.slot, "reasonToMoveClass:to:", {&name.slot, &to.slot}));
    if (!m.failed() && reasonOrOk(*s, refusal.slot, reason) != AO_OK) {
      return AO_ERR;
    }
    m.send(from.slot, "moveClass:to:", {&name.slot, &to.slot});
  } else {
    Root sel(s->roots, m.string(selector));
    const Oop side = boolean(meta == 1);
    Root refusal(s->roots, m.send(from.slot, "reasonToMoveSelector:class:meta:to:",
                                  {&sel.slot, &name.slot, &side, &to.slot}));
    if (!m.failed() && reasonOrOk(*s, refusal.slot, reason) != AO_OK) {
      return AO_ERR;
    }
    m.send(from.slot, "moveSelector:class:meta:to:", {&sel.slot, &name.slot, &side, &to.slot});
  }
  if (m.failed()) {
    *reason = kFailed;
    return AO_ERR;
  }
  return AO_OK;
}

int sessionChangesFileOut(std::int64_t setId, const char* path, std::string* reason) {
  Session* s = changesSession();
  if (s == nullptr || path == nullptr) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    *reason = kUnavailable;
    return AO_ERR;
  }
  std::string text;
  {
    ModelSend m(*s);
    Root set(s->roots, Oop::nil());
    if (!setWithId(m, cls.slot, setId, set)) {
      *reason = kFailed;
      return AO_ERR;
    }
    if (set.slot.isNil()) {
      *reason = kUnknown;
      return AO_ERR;
    }
    char version[32] = "";
    version_string(version, static_cast<int>(sizeof version));
    // SPEC §3.10 (P17): Smalltalk has no date of its own, so C++ passes the date and the version.
    Root date(s->roots, m.string(today()));
    Root ao(s->roots, m.string(version));
    const Oop answer = m.send(set.slot, "fileOutStringDate:version:", {&date.slot, &ao.slot});
    if (m.failed() || !bytesText(*s, answer, &text)) {
      *reason = kFailed;
      return AO_ERR;
    }
  }
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(text.data(), static_cast<std::streamsize>(text.size()));
  out.close();
  if (!out) {
    *reason = std::string("cannot write file: ") + path;
    return AO_ERR;
  }
  return AO_OK;
}

}  // namespace ao

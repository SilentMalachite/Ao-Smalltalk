#pragma once

#include "ao/DebugSnapshot.hpp"
#include "ao/MethodImage.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Scheduler.hpp"

#include "ao_abi.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ao {

// SPEC §3.8, §3.13: the pc→source spans and the temp names of one compiled body (a method or a
// block), as the compiler made them. Spans are in the compiled text's coordinates.
struct MethodDebugInfo {
  std::vector<compiler::PcSpan> pcMap;
  std::vector<compiler::TempName> temps;
};

// A method's debug info and its blocks': bodies[0] is the method, bodies[k] the k-th block of the
// preorder walk over the literals (MethodSource::blocks[k - 1]). Plain C++ data: the GC does not
// trace it and the image does not hold it.
struct DebugInfo {
  std::vector<MethodDebugInfo> bodies;
};

// Not a public header. One process session, same objects as the test Boot fixture. It is the
// DebugSink of its processes while capture is on (setSessionDebugCapture).
struct Session final : DebugSink {
  Heap heap;
  Roots roots;
  WellKnown wk;
  std::unique_ptr<ClassMethodCache> cache;
  std::unique_ptr<CallContext> ctx;
  // SPEC §3.4, §3.10: the cooperative scheduler; ctx is its base process. Made with ctx and dropped
  // before it. The destructor drops it first, abandoning the processes left while the heap and the
  // roots are still there.
  std::unique_ptr<Scheduler> scheduler;
  // Session Dictionary of workspace name strings to bindings (Associations). Not part of the image.
  Oop workspace = Oop::nil();
  bool workspaceRooted = false;
  // knownGlobals for workspace compiles, rebuilt when wk.globalsVersion() moves (SPEC §3.10).
  std::vector<std::string> knownGlobals;
  std::uint64_t knownGlobalsVersion = 0;
  bool knownGlobalsCached = false;

  // Method source text. unique_ptr keeps the rooted slots stable when the table grows.
  // Not part of the image (SPEC §3.10 ソースはイメージに書かない).
  struct MethodSource {
    Oop method = Oop::nil();
    // The accepted text as a String; nil for the doIt, whose text is doItText.
    Oop text = Oop::nil();
    std::string doItText;
    // The method's block CompiledMethods, nested ones too, in preorder over the literals. Each
    // slot is a root; the vector is sized before its slots are rooted and never grows while they
    // are.
    std::vector<Oop> blocks;
    // bodies[0] the method, bodies[k] blocks[k - 1]. Null: no debug info.
    std::shared_ptr<const DebugInfo> debug;
    // What to take off a span to index the text (kDoItPrefix for the doIt, 0 otherwise).
    std::uint32_t sourceOffset = 0;
  };
  std::vector<std::unique_ptr<MethodSource>> methodSources;
  // SPEC §3.10: the last ao_eval's doIt (method, blocks, text, debug info). Cleared when an
  // ao_eval starts and put in once the doIt is boxed. Browser reads never see it.
  std::unique_ptr<MethodSource> doItDebug;
  // SPEC §3.13: the one snapshot of the session. Declared after roots, so it is destroyed (and
  // unroots its slots) first.
  DebugSnapshot debug{roots};

  // SPEC §3.10 評価結果: the last ao_eval's printString (UTF-8, may hold NUL bytes). Empty in a new
  // session (boot, load) and from the start of each ao_eval until it answers AO_OK or AO_ERR_RANGE;
  // nullopt when the printString was INT_MAX bytes or more. Plain C++ memory: the GC does not
  // trace it and the image does not hold it.
  std::optional<std::string> evalResult = std::string();

  // true: Bootstrap::run. false: empty old space for Image::load.
  explicit Session(bool bootstrap);
  ~Session() override;

  // SPEC §3.13 スナップショットの寿命: the base's capture always replaces the snapshot; another
  // process's fills it only when it is empty, so a failure in the drain does not hide the base's.
  void onAbort(CallContext& ctx) noexcept override;
};

// SPEC §3.13: what debugInfoFor answers. `source` is the entry (an accepted method's or the
// doIt's) that holds `method` as its method or one of its blocks; `index` says which body
// (0 the method, k blocks[k - 1]); `body` is that body's debug info.
struct DebugInfoRef {
  const Session::MethodSource* source = nullptr;
  const MethodDebugInfo* body = nullptr;
  std::uint32_t index = 0;
  explicit operator bool() const { return body != nullptr; }
};

Session* session();
int sessionBoot();
int sessionShutdown();
// SPEC §3.10: the transcript hook the ABI holds. It goes into the current session's ctx now and
// into every session sessionBoot and sessionImageLoad make afterwards. Null removes it.
void setSessionTranscriptHook(HostOopHook hook);
int sessionImageSave(const char* path);
// SPEC §3.10: nonzero when the load or its probes fail; *reason (when given) then says why.
int sessionImageLoad(const char* path, std::string* reason = nullptr);
int sessionFileInLoadOrder(const char* path);
int sessionWorkspaceReset();
int sessionEval(const char* source, int sourceLen, int mode, char* out, int outLen, AoSpan* err,
                AoInspectFn inspect, void* inspectUser);
// SPEC §3.10 評価結果: the byte count of the last ao_eval's result; -1 with no session or result.
int sessionEvalResultLength();
// SPEC §3.10 評価結果: writes the result as the browser reads do (AO_ERR_RANGE when cut); AO_ERR
// for a NULL buf, bufLen < 1, no session or no result.
int sessionEvalResultCopy(char* buf, int bufLen);
// SPEC §3.13: capture on or off (off by default). Kept across boot, load and shutdown, like the
// transcript hook: it goes into the current session's ctx now and into every later session's.
void setSessionDebugCapture(bool on);
// The current session's snapshot; null without a session.
DebugSnapshot* sessionDebugSnapshot();
// SPEC §3.10 ao_debug_generation: moves by one on every capture and every clear, across sessions
// too (0..INT_MAX, then round again). -1 without a session.
int sessionDebugGeneration();
// SPEC §3.10 ao_debug_clear: empties the snapshot and moves the generation. Nothing without a
// session.
void sessionDebugClear();
// SPEC §3.10 デバッガの読み出し: the ao_debug_* reads of the snapshot, with the ABI's answers (the
// counts -1 without a session; the strings by the buffer rule). They only read (no GC).
int debugFrameCount();
int debugFrameTotal();
int debugReason(char* buf, int len);
int debugFrameKind(int i);
int debugFrameLabel(int i, char* buf, int len);
int debugFramePc(int i);
int debugFrameSource(int i, char* buf, int len, AoSpan* highlight);
int debugTempCount(int i);
int debugTempName(int i, int j, char* buf, int len);
// SPEC §3.10: the outermost ao_debug_* entries (the ABI takes AbiEntry first). They send
// printString (and inspect) with no debug sink on the base, so their aborts are not captured, and
// read and clear the abort without a drain. j -1 is the receiver for debugInspect.
int debugReceiverPrint(int i, char* classBuf, int classLen, char* buf, int len);
int debugTempPrint(int i, int j, char* classBuf, int classLen, char* buf, int len);
int debugInspect(int i, int j, AoInspectFn inspect, void* inspectUser);
// SPEC §3.10 ao_debug_clear: sessionDebugClear, AO_ERR without a session.
int debugClear();
// `replaced`, when a heap object, is dropped from the rooted table (with its blocks) before
// `method` is stored. `image`, when given, is what `method` was boxed from: its blocks are found
// at the same literal indices (nested ones in preorder) and its debug info is kept.
void rememberMethodSource(Oop method, Oop text, Oop replaced, const compiler::MethodImage* image);
// SPEC §3.9: a class whose shape changed takes its methods' sources along. The entry of `from`
// names `to` from now on; the blocks boxed for `to` replace `from`'s in the same preorder, and the
// debug info stays. False when `from` has no source.
bool moveMethodSource(Oop from, Oop to);
// The text of the entry that holds `method` as its method or one of its blocks (the home
// method's), the doIt's too. False when no entry holds it.
bool methodSource(Oop method, std::string& utf8);
// SPEC §3.13: the debug info of `method` (an accepted method, one of its blocks, the doIt or one
// of its blocks); false (empty) when no entry holds it or it has none.
DebugInfoRef debugInfoFor(Oop method);
// SPEC §3.8: the span of the last pc→source entry of `method` at or before pc, in its entry's
// text (sourceOffset taken off). False when there is none.
bool debugSpanAt(Oop method, std::uint32_t pc, std::uint32_t& start, std::uint32_t& end);
// The root slots the method source table adds (each method's, text's and block's, the doIt's) and
// the snapshot's. They are session state no Smalltalk object reaches, so neither the image save
// nor a trace for what is alive (SPEC §3.9) starts from them. Empty outside a session. The table's
// are LIFO slots (Roots::add); the snapshot's are its one pinned range (DebugSnapshot::rootFirst),
// left out when withSnapshot is false.
std::vector<const Oop*> methodSourceRootSlots(bool withSnapshot = true);
void clearMethodSources();
void ensureKernelNatives(Session& s);

int browserClassCount();
int browserClassAt(int index, char* name, int nameLen, char* category, int categoryLen);
int browserProtocolCount(const char* className, int meta);
int browserProtocolAt(const char* className, int meta, int index, char* buf, int len);
int browserSelectorCount(const char* className, int meta, const char* protocol);
int browserSelectorAt(const char* className, int meta, const char* protocol, int index, char* buf,
                      int len);
int browserSource(const char* className, int meta, const char* selector, char* buf, int len);
int browserClassDefinition(const char* className, char* buf, int len);
int browserSuperclass(const char* className, int meta, char* buf, int len);
int browserSubclassCount(const char* className);
int browserSubclassAt(const char* className, int index, char* buf, int len);

}  // namespace ao

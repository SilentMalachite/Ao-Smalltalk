// SPEC §3.13 デバッガ（捕捉）、§3.3 の `halt`、§3.6 `Object>>halt`。
#include "test_support.hpp"

#include "ao/CompiledMethod.hpp"
#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/DebugSnapshot.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

namespace {

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

// Captures every abort it is told of (a session keeps one; P10-04). calls counts them.
struct TestSink final : ao::DebugSink {
  ao::DebugSnapshot snap;
  int calls = 0;
  explicit TestSink(ao::Roots& roots) : snap(roots) {}
  void onAbort(ao::CallContext& ctx) noexcept override {
    ++calls;
    snap.capture(ctx);
  }
};

const char* const kProbe =
    "!Object subclass: #DbgProbe\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'P10-Test'!\n"
    "!DbgProbe methodsFor: 'debug'!\n"
    "outer: x\n"
    "  | y |\n"
    "  y := x copy.\n"
    "  ^self inner: y!\n"
    "inner: s\n"
    "  ^self error: s!\n"
    "blockTemps\n"
    "  | k |\n"
    "  k := 7.\n"
    "  ^#(1 2) do: [:e | | w | w := e + k. w foo]!\n"
    "recur: n\n"
    "  ^self recur: n + 1! !\n";

// A Boot with DbgProbe filed in and the test sink set on the base process.
struct Dbg {
  Boot b;
  TestSink sink{b.roots};
  Dbg() {
    std::vector<ao::compiler::CompileError> errs;
    const bool ok = ao::fileInString(b.ctx, kProbe, errs);
    EXPECT_TRUE(ok && errs.empty()) << (errs.empty() ? "file-in failed" : errs[0].message);
    b.ctx.debug = &sink;
  }
  ~Dbg() { b.ctx.debug = nullptr; }
  ao::Oop sym(const char* s) { return b.wk.intern(s); }
  ao::Oop probeClass() { return b.wk.named("DbgProbe"); }
  const ao::DebugSnapshot& snap() const { return sink.snap; }
  // The selector a compiled or native method was installed under.
  ao::Oop selectorOf(ao::Oop method) {
    if (!method.isHeap()) {
      return ao::Oop{};
    }
    if (b.heap.klass(method) == b.wk.nativeMethodClass) {
      return b.heap.slotAt(method, ao::kNativeSlotSelector);
    }
    return b.heap.slotAt(method, ao::kCmSlotSelector);
  }
};

// Compiles source ("doIt\n...") as a method of Object and runs it outermost on the base process
// with nil as the receiver, like evalExpr in block_test.
ao::Oop runDoIt(Boot& b, const std::string& source) {
  auto img = ao::compiler::compileMethod(source);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  return ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

std::string runAbort(Boot& b, const std::string& source) {
  runDoIt(b, source);
  return takeAbortReason(b);
}

std::string text(Boot& b, ao::Oop s) {
  return s.isHeap() ? ao::Str::toUtf8(b.heap, s) : std::string("<not a string>");
}

}  // namespace

TEST(DebugSnapshot, ErrorInNestedMethodCapturesInnermostFirst) {
  Dbg d;
  const std::string reason = runAbort(d.b, "doIt\n  ^DbgProbe new outer: 'ab'");
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  ASSERT_FALSE(s.empty());
  EXPECT_EQ(reason, s.reason());
  EXPECT_TRUE(s.fromBase());
  ASSERT_EQ(4u, s.count());
  EXPECT_EQ(4u, s.total());
  // 0: error: is a native, synthesized from inner:'s send in flight.
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  EXPECT_EQ(d.b.wk.nativeMethodClass, d.b.heap.klass(s.method(0)));
  EXPECT_EQ(d.sym("error:"), d.selectorOf(s.method(0)));
  EXPECT_EQ(d.sym("error:"), s.selector(0));
  EXPECT_EQ(d.probeClass(), d.b.heap.klass(s.receiver(0)));
  ASSERT_EQ(1u, s.tempCount(0));
  EXPECT_EQ("ab", text(d.b, s.temp(0, 0)));
  // 1: inner:, sending error:.
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(1));
  EXPECT_EQ(d.sym("inner:"), d.selectorOf(s.method(1)));
  EXPECT_EQ(d.sym("error:"), s.selector(1));
  EXPECT_EQ(s.receiver(0), s.receiver(1));
  ASSERT_EQ(1u, s.tempCount(1));
  EXPECT_EQ(s.temp(0, 0), s.temp(1, 0));
  ASSERT_EQ(1u, s.sendArgCount(1));
  EXPECT_EQ(s.temp(1, 0), s.sendArg(1, 0));
  // 2: outer:, its temps x and y, sending inner:.
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(2));
  EXPECT_EQ(d.sym("outer:"), d.selectorOf(s.method(2)));
  EXPECT_EQ(d.sym("inner:"), s.selector(2));
  ASSERT_EQ(2u, s.tempCount(2));
  EXPECT_EQ("ab", text(d.b, s.temp(2, 0)));
  EXPECT_EQ(s.temp(1, 0), s.temp(2, 1));
  EXPECT_NE(s.temp(2, 0), s.temp(2, 1));  // y is x copy
  // 3: the doIt, sending outer:.
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(3));
  EXPECT_EQ(d.sym("outer:"), s.selector(3));
  EXPECT_TRUE(s.receiver(3).isNil());
  // Each interpreted frame's pc is the start of its Send, inside its method.
  for (std::uint32_t i = 1; i < 4; ++i) {
    EXPECT_GT(s.pc(i), 0u) << i;
    ASSERT_TRUE(s.context(i).isHeap()) << i;
    EXPECT_EQ(s.method(i), d.b.heap.slotAt(s.context(i), ao::kCtxMethod)) << i;
  }
  // Out of range.
  EXPECT_EQ(-1, s.kind(4));
  EXPECT_TRUE(s.method(4).isEmpty());
  EXPECT_EQ(0u, s.tempCount(4));
  EXPECT_TRUE(s.temp(1, 1).isEmpty());
}

TEST(DebugSnapshot, BlockFrameKeepsTempsAndHome) {
  Dbg d;
  EXPECT_EQ("doesNotUnderstand: #foo", runAbort(d.b, "doIt\n  ^DbgProbe new blockTemps"));
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  ASSERT_EQ(4u, s.count());
  // 0: the DNU of foo, sent to w = 1 + 7.
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  EXPECT_EQ(smi(8), s.receiver(0));
  // 1: the block, with e = 1, w = 8 and k = 7 copied in.
  EXPECT_EQ(ao::kDebugFrameBlock, s.kind(1));
  ASSERT_EQ(3u, s.tempCount(1));
  EXPECT_EQ(smi(1), s.temp(1, 0));
  EXPECT_EQ(smi(8), s.temp(1, 1));
  // k is copied in, as its value or as the temp vector that holds it.
  const ao::Oop k = s.temp(1, 2);
  EXPECT_TRUE(k == smi(7) || (k.isHeap() && d.b.heap.slotAt(k, 0) == smi(7)));
  EXPECT_EQ(d.sym("foo"), s.selector(1));
  // 2: its home, blockTemps, sending the native do: (not in the chain).
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(2));
  EXPECT_EQ(d.sym("blockTemps"), d.selectorOf(s.method(2)));
  EXPECT_EQ(d.sym("do:"), s.selector(2));
  EXPECT_EQ(s.context(2), d.b.heap.slotAt(s.context(1), ao::kBlockHome));
  EXPECT_EQ(s.receiver(2), s.receiver(1));
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(3));
}

TEST(DebugSnapshot, FailedNativeSendSynthesizesNativeFrameWithReceiverAndArgs) {
  Dbg d;
  const std::string reason = runAbort(d.b, "doIt\n  ^#(1 2) at: 5");
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  EXPECT_EQ(reason, s.reason());
  ASSERT_EQ(2u, s.count());
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  ASSERT_TRUE(s.method(0).isHeap());
  EXPECT_EQ(d.b.wk.nativeMethodClass, d.b.heap.klass(s.method(0)));
  EXPECT_EQ(d.sym("at:"), d.selectorOf(s.method(0)));
  EXPECT_EQ(d.sym("at:"), s.selector(0));
  ASSERT_TRUE(s.receiver(0).isHeap());
  EXPECT_EQ(d.b.wk.arrayClass, d.b.heap.klass(s.receiver(0)));
  EXPECT_EQ(2u, d.b.heap.size(s.receiver(0)));
  ASSERT_EQ(1u, s.tempCount(0));
  EXPECT_EQ(smi(5), s.temp(0, 0));
  EXPECT_EQ(0u, s.pc(0));
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(1));
  EXPECT_EQ(d.sym("at:"), s.selector(1));
  ASSERT_EQ(1u, s.sendArgCount(1));
  EXPECT_EQ(smi(5), s.sendArg(1, 0));
}

TEST(DebugSnapshot, DoesNotUnderstandSynthesizesFrameWithoutMethod) {
  Dbg d;
  EXPECT_EQ("doesNotUnderstand: #foo", runAbort(d.b, "doIt\n  ^nil foo"));
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  ASSERT_EQ(2u, s.count());
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  EXPECT_TRUE(s.method(0).isNil());
  EXPECT_EQ(d.sym("foo"), s.selector(0));
  EXPECT_TRUE(s.receiver(0).isNil());
  EXPECT_EQ(0u, s.tempCount(0));
  EXPECT_EQ(ao::kDebugFrameMethod, s.kind(1));
  EXPECT_EQ(d.sym("foo"), s.selector(1));
}

TEST(DebugSnapshot, StackOverflowCapturesCappedFramesAndTotal) {
  Dbg d;
  ao::Root obj(d.b.roots, send0(d.b, d.probeClass(), "new"));
  // A stack deep enough for well over 256 levels (a Debug level takes about 4 KiB; ASan frames
  // are larger still).
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
  constexpr std::size_t kStack = std::size_t{16} << 20;
#else
  constexpr std::size_t kStack = std::size_t{4} << 20;
#endif
#else
  constexpr std::size_t kStack = std::size_t{4} << 20;
#endif
  runOnSmallStack([&] { send1(d.b, obj.slot, "recur:", smi(0)); }, kStack);
  ASSERT_TRUE(d.b.ctx.aborting);
  EXPECT_EQ(std::string("stack overflow"), d.b.ctx.abortReason);
  ao::clearUnwinding(d.b.ctx);
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  EXPECT_EQ("stack overflow", s.reason());
  ASSERT_EQ(ao::DebugSnapshot::kMaxFrames, s.count());
  EXPECT_GT(s.total(), s.count());
  // The guard trips before the callee's frame exists: the innermost is the caller, whose send
  // found a CompiledMethod, so nothing is synthesized.
  for (std::uint32_t i = 0; i < s.count(); ++i) {
    ASSERT_EQ(ao::kDebugFrameMethod, s.kind(i)) << i;
    ASSERT_EQ(d.sym("recur:"), s.selector(i)) << i;
  }
  // The outermost recur: has n = 0 (beyond the cap here); inward each n is one more.
  ASSERT_TRUE(s.temp(0, 0).isSmallInteger());
  ASSERT_TRUE(s.temp(1, 0).isSmallInteger());
  EXPECT_EQ(s.temp(1, 0).smallIntegerValue() + 1, s.temp(0, 0).smallIntegerValue());
  EXPECT_EQ(static_cast<std::int64_t>(s.total()) - 1, s.temp(0, 0).smallIntegerValue());
}

TEST(DebugSnapshot, CleanupAbortKeepsFirstSnapshot) {
  Dbg d;
  EXPECT_EQ("doesNotUnderstand: #foo", runAbort(d.b, "doIt\n  ^[nil foo] ensure: [nil bar]"));
  ASSERT_EQ(1, d.sink.calls);
  EXPECT_EQ("doesNotUnderstand: #foo", d.snap().reason());
  ASSERT_GE(d.snap().count(), 1u);
  EXPECT_EQ(d.sym("foo"), d.snap().selector(0));
  EXPECT_EQ(0u, d.b.ctx.abortSetAside);
}

TEST(DebugSnapshot, CleanupFailureAfterNormalEndIsCaptured) {
  Dbg d;
  EXPECT_EQ("doesNotUnderstand: #bar", runAbort(d.b, "doIt\n  ^[1] ensure: [nil bar]"));
  ASSERT_EQ(1, d.sink.calls);
  EXPECT_EQ("doesNotUnderstand: #bar", d.snap().reason());
  ASSERT_GE(d.snap().count(), 2u);
  EXPECT_EQ(d.sym("bar"), d.snap().selector(0));
  EXPECT_EQ(ao::kDebugFrameBlock, d.snap().kind(1));
}

TEST(DebugSnapshot, SelfTerminateDoesNotCapture) {
  Dbg d;
  EXPECT_EQ("<no abort>", runAbort(d.b,
                                   "doIt\n"
                                   "  [[Processor activeProcess terminate] ensure: [3]] fork.\n"
                                   "  Processor yield.\n"
                                   "  ^1"));
  EXPECT_EQ(0u, d.b.scheduler.processFailures());
  EXPECT_EQ(0, d.sink.calls);
  // A terminate from another process is no failure either.
  EXPECT_EQ("<no abort>", runAbort(d.b,
                                   "doIt\n"
                                   "  | p sem |\n"
                                   "  sem := Semaphore new.\n"
                                   "  p := [[sem wait] ensure: [3]] fork.\n"
                                   "  Processor yield.\n"
                                   "  p terminate.\n"
                                   "  ^1"));
  EXPECT_EQ(0u, d.b.scheduler.processFailures());
  EXPECT_EQ(0u, d.b.scheduler.liveFibers());
  EXPECT_EQ(0, d.sink.calls);
  EXPECT_TRUE(d.snap().empty());
}

TEST(DebugSnapshot, TerminateBaseCaptures) {
  Dbg d;
  EXPECT_EQ("process terminated",
            runAbort(d.b, "doIt\n  Processor activeProcess terminate.\n  ^1"));
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  EXPECT_EQ("process terminated", s.reason());
  EXPECT_TRUE(s.fromBase());
  EXPECT_EQ(d.b.scheduler.activeProcess(), s.process());
  ASSERT_EQ(2u, s.count());
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  EXPECT_EQ(d.sym("terminate"), s.selector(0));
  EXPECT_EQ(s.process(), s.receiver(0));
}

TEST(DebugSnapshot, DeadlockOnBaseCaptures) {
  Dbg d;
  EXPECT_EQ("deadlock: no runnable process", runAbort(d.b, "doIt\n  ^Semaphore new wait"));
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  EXPECT_EQ("deadlock: no runnable process", s.reason());
  EXPECT_TRUE(s.fromBase());
  ASSERT_EQ(2u, s.count());
  EXPECT_EQ(d.sym("wait"), s.selector(0));
}

TEST(DebugSnapshot, AbandonDoesNotCapture) {
  Dbg d;
  EXPECT_EQ("<no abort>", runAbort(d.b,
                                   "doIt\n"
                                   "  | sem |\n"
                                   "  sem := Semaphore new.\n"
                                   "  [[sem wait] ensure: [nil foo]] fork.\n"
                                   "  [[Processor activeProcess suspend] ifCurtailed: [nil bar]] fork.\n"
                                   "  Processor yield.\n"
                                   "  ^1"));
  EXPECT_EQ(2u, d.b.scheduler.liveFibers());
  d.b.scheduler.terminateAll(true);
  EXPECT_EQ(0u, d.b.scheduler.liveFibers());
  EXPECT_EQ(0u, d.b.scheduler.processFailures());
  EXPECT_EQ(0, d.sink.calls);
  EXPECT_TRUE(d.snap().empty());
}

TEST(DebugSnapshot, FiberFailureCapturesOnItsOwnChain) {
  Dbg d;
  ao::Root base(d.b.roots, d.b.scheduler.activeProcess());
  EXPECT_EQ("<no abort>", runAbort(d.b,
                                   "doIt\n"
                                   "  [nil foo] fork.\n"
                                   "  Processor yield.\n"
                                   "  ^1"));
  EXPECT_EQ(1u, d.b.scheduler.processFailures());
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  EXPECT_EQ("doesNotUnderstand: #foo", s.reason());
  EXPECT_FALSE(s.fromBase());
  EXPECT_NE(base.slot, s.process());
  // Only the fiber's own frames: the DNU and the forked block, not the base's doIt.
  ASSERT_EQ(2u, s.count());
  EXPECT_EQ(2u, s.total());
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  EXPECT_EQ(d.sym("foo"), s.selector(0));
  EXPECT_EQ(ao::kDebugFrameBlock, s.kind(1));
  // The base's chain is its own again, and empty outside an evaluation.
  EXPECT_EQ(nullptr, d.b.ctx.topFrame);
}

TEST(DebugSnapshot, SnapshotSurvivesNurseryAndOldCollections) {
  Dbg d;
  // The first evaluation of a Boot leaves a lasting root of its own; count from after it.
  runDoIt(d.b, "doIt\n  ^1");
  const std::size_t before = d.b.roots.counts().slots;
  runAbort(d.b, "doIt\n  ^DbgProbe new outer: 'ab'");
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  ASSERT_EQ(4u, s.count());
  EXPECT_GT(d.b.roots.counts().slots, before);
  // Garbage, then both collections: the snapshot's values move and stay.
  for (int i = 0; i < 2000; ++i) {
    send1(d.b, d.b.wk.arrayClass, "new:", smi(16));
  }
  ao::Gc gc(d.b.heap, d.b.roots);
  gc.collectNursery();
  gc.collectOld();
  gc.collectNursery();
  EXPECT_EQ(d.probeClass(), d.b.heap.klass(s.receiver(1)));
  EXPECT_EQ("ab", text(d.b, s.temp(0, 0)));
  EXPECT_EQ("ab", text(d.b, s.temp(2, 0)));
  EXPECT_EQ("ab", text(d.b, s.temp(2, 1)));
  EXPECT_EQ(s.temp(0, 0), s.temp(2, 1));
  EXPECT_NE(s.temp(2, 0), s.temp(2, 1));
  EXPECT_EQ(d.sym("outer:"), d.selectorOf(s.method(2)));
  EXPECT_EQ(d.sym("inner:"), d.selectorOf(s.method(1)));
  EXPECT_EQ(s.method(3), d.b.heap.slotAt(s.context(3), ao::kCtxMethod));
  // rootSlots names every slot the snapshot registered.
  EXPECT_EQ(d.b.roots.counts().slots - before, d.sink.snap.rootSlots().size());
  // Clearing gives the roots back.
  d.sink.snap.clear();
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(0u, s.count());
  EXPECT_EQ(before, d.b.roots.counts().slots);
}

TEST(DebugSnapshot, HaltAbortsWithHaltReason) {
  Dbg d;
  EXPECT_EQ("halt", runAbort(d.b, "doIt\n  3 halt.\n  ^1"));
  ASSERT_EQ(1, d.sink.calls);
  const auto& s = d.snap();
  EXPECT_EQ("halt", s.reason());
  ASSERT_EQ(2u, s.count());
  EXPECT_EQ(ao::kDebugFrameNative, s.kind(0));
  EXPECT_EQ(d.sym("halt"), d.selectorOf(s.method(0)));
  EXPECT_EQ(smi(3), s.receiver(0));
  // Without a sink, halt still aborts with its reason.
  d.b.ctx.debug = nullptr;
  EXPECT_EQ("halt", runAbort(d.b, "doIt\n  ^nil halt"));
  EXPECT_EQ(1, d.sink.calls);
}

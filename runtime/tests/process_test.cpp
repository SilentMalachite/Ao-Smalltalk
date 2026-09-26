// SPEC §3.4「プロセスと協調スケジューラ」、§3.6 Kernel-Processes。
#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Scheduler.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>

namespace {

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

// Compiles source, a method of Object ("doIt", or "doIt: env" taking env), and runs it outermost
// on the base process, like evalExpr in block_test.
ao::Oop run(Boot& b, const std::string& source, ao::Oop env = ao::Oop{}) {
  // Boxing the method may collect: root env first.
  ao::Root arg(b.roots, env);
  auto img = ao::compiler::compileMethod(source);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  const std::uint32_t argc = env.isEmpty() ? 0 : 1;
  return ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), argc == 0 ? nullptr : &arg.slot,
                              argc, ao::Oop::nil());
}

// The printStrings of coll's elements (size and at:), joined by spaces.
std::string itemsOf(Boot& b, ao::Oop coll) {
  ao::Root c(b.roots, coll);
  const ao::Oop n = send0(b, c.slot, "size");
  if (!n.isSmallInteger()) {
    return "<no size>";
  }
  std::string out;
  for (std::int64_t i = 1; i <= n.smallIntegerValue(); ++i) {
    ao::Root e(b.roots, send1(b, c.slot, "at:", smi(i)));
    ao::Root s(b.roots, send0(b, e.slot, "printString"));
    if (!out.empty()) {
      out += ' ';
    }
    out += s.slot.isHeap() ? ao::Str::toUtf8(b.heap, s.slot) : std::string("<?>");
  }
  return out;
}

// Runs source and renders the collection it answers (itemsOf), or "<abort: reason>".
std::string runItems(Boot& b, const std::string& source, ao::Oop env = ao::Oop{}) {
  ao::Root v(b.roots, run(b, source, env));
  if (b.ctx.aborting) {
    return "<abort: " + takeAbortReason(b) + ">";
  }
  return itemsOf(b, v.slot);
}

// Runs source; the reason of the abort it ended in, or "<no abort>".
std::string runAbort(Boot& b, const std::string& source, ao::Oop env = ao::Oop{}) {
  run(b, source, env);
  return takeAbortReason(b);
}

ao::Oop active(Boot& b) { return send0(b, b.wk.processor, "activeProcess"); }

ao::Oop at(Boot& b, ao::Oop coll, std::int64_t i) { return send1(b, coll, "at:", smi(i)); }

ao::Oop newEnv(Boot& b, std::int64_t n) { return send1(b, b.wk.arrayClass, "new:", smi(n)); }

// Processor's quiescentProcesses: the ready queue as Smalltalk sees it (SPEC §3.4).
ao::Oop readyList(Boot& b) { return send1(b, b.wk.processor, "instVarAt:", smi(1)); }

ao::Oop myList(Boot& b, ao::Oop process) { return send1(b, process, "instVarAt:", smi(4)); }

std::int64_t sizeOf(Boot& b, ao::Oop coll) { return send0(b, coll, "size").smallIntegerValue(); }

int& nativeBlockRuns() {
  static int n = 0;
  return n;
}

bool& nativeBlockRanOnFiber() {
  static bool seen = false;
  return seen;
}

}  // namespace

TEST(Process, SchedulerHoldsOneActiveProcess) {
  Boot b;
  auto p = send0(b, b.wk.processor, "activeProcess");
  ASSERT_TRUE(p.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p));
  EXPECT_FALSE(b.scheduler.nonBaseRunning());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

TEST(Process, SemaphoreSignalThenWait) {
  Boot b;
  auto sem = send0(b, b.wk.semaphoreClass, "new");
  send0(b, sem, "signal");
  auto r = send0(b, sem, "wait");
  EXPECT_EQ(sem, r);
}

TEST(Process, NamedProcessorIsSchedulerInstance) {
  Boot b;
  EXPECT_EQ(b.wk.processor, b.wk.named("Processor"));
  EXPECT_EQ(b.wk.processor, ao::Globals::at(b.wk, "Processor"));
  EXPECT_EQ(b.wk.processorSchedulerClass, b.wk.named("ProcessorScheduler"));
  EXPECT_EQ(b.wk.processorSchedulerClass, ao::Globals::at(b.wk, "ProcessorScheduler"));
  EXPECT_NE(b.wk.processor, b.wk.processorSchedulerClass);
  EXPECT_EQ(b.wk.processorSchedulerClass, b.heap.klass(b.wk.processor));
}

// SPEC §3.4: a process suspends itself and is resumed; resume enqueues once. A Process this
// session did not fork (Process new) and a finished one cannot run.
TEST(Process, ResumeYieldSuspendRoundTrip) {
  Boot b;
  ao::Root base(b.roots, active(b));
  ao::Root stray(b.roots, send0(b, b.wk.processClass, "new"));
  ASSERT_TRUE(stray.slot.isHeap());
  EXPECT_TRUE(send0(b, stray.slot, "resume").isEmpty());
  EXPECT_EQ("process cannot run", takeAbortReason(b));

  ao::Root env(b.roots, newEnv(b, 2));
  EXPECT_EQ("1", runItems(b,
                          "doIt: env\n"
                          "  | log |\n"
                          "  log := OrderedCollection new.\n"
                          "  env at: 1 put: log.\n"
                          "  env at: 2 put: [log add: 1. Processor activeProcess suspend. log add: 2] fork.\n"
                          "  Processor yield.\n"
                          "  ^log",
                          env.slot));
  ao::Root p(b.roots, at(b, env.slot, 2));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_TRUE(myList(b, p.slot).isNil());
  EXPECT_EQ(1u, b.scheduler.liveFibers());
  EXPECT_EQ(0, sizeOf(b, readyList(b)));

  EXPECT_EQ(p.slot, send0(b, p.slot, "resume"));
  EXPECT_EQ(p.slot, send0(b, p.slot, "resume"));
  EXPECT_EQ(1, sizeOf(b, readyList(b)));
  EXPECT_EQ(readyList(b), myList(b, p.slot));
  EXPECT_EQ(base.slot, active(b));

  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ("1 2", itemsOf(b, at(b, env.slot, 1)));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_TRUE(send0(b, p.slot, "resume").isEmpty());
  EXPECT_EQ("process cannot run", takeAbortReason(b));
  // Suspending a finished or stray process does nothing.
  EXPECT_EQ(p.slot, send0(b, p.slot, "suspend"));
  EXPECT_EQ(stray.slot, send0(b, stray.slot, "suspend"));
}

TEST(Process, PriorityColonStoresSmallInteger) {
  Boot b;
  auto p = send0(b, b.wk.processClass, "new");
  ASSERT_TRUE(p.isHeap());
  auto five = ao::Oop::fromSmallInteger(5);
  EXPECT_EQ(p, send1(b, p, "priority:", five));
  EXPECT_EQ(five, send1(b, p, "instVarAt:", ao::Oop::fromSmallInteger(3)));
}

// SPEC §3.4 fork: a new Process at the end of the ready queue, whose suspendedContext is the
// block. It runs only once the base yields, on its own fiber.
TEST(Process, BlockContextForkCreatesAndResumesProcess) {
  Boot b;
  ao::Root base(b.roots, active(b));
  nativeBlockRuns() = 0;
  nativeBlockRanOnFiber() = false;
  auto fn = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    ++nativeBlockRuns();
    nativeBlockRanOnFiber() = ctx.scheduler != nullptr && ctx.scheduler->nonBaseRunning();
    return ao::Oop::fromSmallInteger(1);
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, fn, 0));
  ASSERT_TRUE(blk.slot.isHeap());
  ao::Root p(b.roots, send0(b, blk.slot, "fork"));
  ASSERT_TRUE(p.slot.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p.slot));
  EXPECT_EQ(blk.slot, send1(b, p.slot, "instVarAt:", smi(2)));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_EQ(0, nativeBlockRuns());
  ao::Root ready(b.roots, readyList(b));
  ASSERT_EQ(1, sizeOf(b, ready.slot));
  EXPECT_EQ(p.slot, at(b, ready.slot, 1));
  EXPECT_EQ(ready.slot, myList(b, p.slot));
  EXPECT_EQ(1u, b.scheduler.liveFibers());

  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(1, nativeBlockRuns());
  EXPECT_TRUE(nativeBlockRanOnFiber());
  EXPECT_EQ(base.slot, active(b));
  EXPECT_EQ(0, sizeOf(b, readyList(b)));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());

  // Only a block of no arguments forks (SPEC §3.4: failed: #fork).
  ao::Root one(b.roots, ao::makeNativeBlock(b.ctx, fn, 1));
  EXPECT_TRUE(send0(b, one.slot, "fork").isEmpty());
  EXPECT_EQ("failed: #fork", runAbort(b, "doIt\n  ^[:x | x] fork"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §3.4 wait/signal: the waiter sits in the linkedList; signal makes it ready without
// switching; it runs when the signaller yields.
TEST(Process, WaitSuspendsThenSignalResumes) {
  Boot b;
  ao::Root base(b.roots, active(b));
  ao::Root env(b.roots, newEnv(b, 3));
  EXPECT_EQ("1 2", runItems(b,
                            "doIt: env\n"
                            "  | sem log |\n"
                            "  sem := Semaphore new.\n"
                            "  log := OrderedCollection new.\n"
                            "  env at: 1 put: sem.\n"
                            "  env at: 2 put: log.\n"
                            "  env at: 3 put: [log add: 1. sem wait. log add: 3] fork.\n"
                            "  Processor yield.\n"
                            "  log add: 2.\n"
                            "  ^log",
                            env.slot));
  ao::Root sem(b.roots, at(b, env.slot, 1));
  ao::Root p(b.roots, at(b, env.slot, 3));
  ao::Root waitList(b.roots, send1(b, sem.slot, "instVarAt:", smi(2)));
  EXPECT_EQ(waitList.slot, myList(b, p.slot));
  EXPECT_EQ(1, sizeOf(b, waitList.slot));
  EXPECT_EQ(base.slot, active(b));

  EXPECT_EQ(sem.slot, send0(b, sem.slot, "signal"));
  EXPECT_EQ("1 2", itemsOf(b, at(b, env.slot, 2)));
  EXPECT_EQ(0, sizeOf(b, waitList.slot));
  EXPECT_EQ(1, sizeOf(b, readyList(b)));
  EXPECT_EQ(readyList(b), myList(b, p.slot));

  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ("1 2 3", itemsOf(b, at(b, env.slot, 2)));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §3.4 resume: a waiting process stays waiting (no second entry anywhere); signal enqueues
// it once, and resume of a ready one does not enqueue it again.
TEST(Process, WaitKeepsMyListAndResumeDoesNotDoubleEnqueue) {
  Boot b;
  ao::Root base(b.roots, active(b));
  ao::Root env(b.roots, newEnv(b, 3));
  run(b,
      "doIt: env\n"
      "  | sem log |\n"
      "  sem := Semaphore new.\n"
      "  log := OrderedCollection new.\n"
      "  env at: 1 put: sem.\n"
      "  env at: 2 put: log.\n"
      "  env at: 3 put: [sem wait. log add: 1] fork.\n"
      "  Processor yield",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  ao::Root sem(b.roots, at(b, env.slot, 1));
  ao::Root p(b.roots, at(b, env.slot, 3));
  ao::Root waitList(b.roots, send1(b, sem.slot, "instVarAt:", smi(2)));
  ASSERT_TRUE(waitList.slot.isHeap());
  EXPECT_EQ(b.wk.orderedCollectionClass, b.heap.klass(waitList.slot));
  EXPECT_EQ(1, sizeOf(b, waitList.slot));
  EXPECT_EQ(p.slot, at(b, waitList.slot, 1));

  EXPECT_EQ(p.slot, send0(b, p.slot, "resume"));
  EXPECT_EQ(waitList.slot, myList(b, p.slot));
  EXPECT_EQ(1, sizeOf(b, waitList.slot));
  ao::Root ready(b.roots, readyList(b));
  EXPECT_EQ(0, sizeOf(b, ready.slot));
  EXPECT_EQ(base.slot, active(b));

  EXPECT_EQ(sem.slot, send0(b, sem.slot, "signal"));
  EXPECT_EQ(1, sizeOf(b, ready.slot));
  EXPECT_EQ(p.slot, at(b, ready.slot, 1));
  EXPECT_EQ(p.slot, send0(b, p.slot, "resume"));
  EXPECT_EQ(1, sizeOf(b, ready.slot));

  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ("1", itemsOf(b, at(b, env.slot, 2)));
  EXPECT_EQ(base.slot, active(b));
}

TEST(Process, SharedQueueNextPutThenNext) {
  Boot b;
  auto q = send0(b, b.wk.sharedQueueClass, "new");
  ASSERT_TRUE(q.isHeap());
  EXPECT_EQ(b.wk.sharedQueueClass, b.heap.klass(q));
  auto one = ao::Oop::fromSmallInteger(1);
  EXPECT_EQ(one, send1(b, q, "nextPut:", one));
  EXPECT_EQ(one, send0(b, q, "next"));
}

TEST(Process, MethodContextGettersReadSlots) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root ctx(b.roots, send0(b, b.wk.methodContextClass, "new"));
  ASSERT_TRUE(ctx.slot.isHeap());
  EXPECT_EQ(b.wk.methodContextClass, b.heap.klass(ctx.slot));
  ao::Root meth(b.roots, send0(b, ao::Oop::fromSmallInteger(1), "class"));
  ao::Root sender(b.roots, send0(b, b.wk.objectClass, "new"));
  auto rcvr = ao::Oop::fromSmallInteger(7);
  EXPECT_EQ(sender.slot,
            send2(b, ctx.slot, "instVarAt:put:", ao::Oop::fromSmallInteger(1), sender.slot));
  EXPECT_EQ(meth.slot,
            send2(b, ctx.slot, "instVarAt:put:", ao::Oop::fromSmallInteger(4), meth.slot));
  EXPECT_EQ(rcvr, send2(b, ctx.slot, "instVarAt:put:", ao::Oop::fromSmallInteger(5), rcvr));
  EXPECT_EQ(sender.slot, send0(b, ctx.slot, "sender"));
  EXPECT_EQ(meth.slot, send0(b, ctx.slot, "method"));
  EXPECT_EQ(rcvr, send0(b, ctx.slot, "receiver"));
}

// excessSignals は Smalltalk から書き換えられる。SmallInteger の最大値にしてから signal すると、
// +1 が SmallInteger の範囲を超える。abort せず、error: の慣習どおりメッセージ文字列で失敗する。
TEST(Process, SignalWithExcessAtSmiMaxFails) {
  Boot b;
  ao::Root sem(b.roots, send0(b, b.wk.semaphoreClass, "new"));
  ASSERT_TRUE(sem.slot.isHeap());
  const ao::Oop max = ao::Oop::fromSmallInteger(ao::kSmiMax);
  ASSERT_EQ(max, send2(b, sem.slot, "instVarAt:put:", ao::Oop::fromSmallInteger(1), max));
  // SPEC §3.3: the failure aborts with its message instead of answering it.
  EXPECT_TRUE(send0(b, sem.slot, "signal").isEmpty());
  EXPECT_EQ("signal: excess signals out of range", takeAbortReason(b));
  EXPECT_EQ(max, send1(b, sem.slot, "instVarAt:", ao::Oop::fromSmallInteger(1)));
}

// ---- fork と yield ----

TEST(Process, ForkRunsOnlyAfterYield) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 2));
  run(b,
      "doIt: env\n"
      "  | n |\n"
      "  n := 0.\n"
      "  [n := n + 1] fork.\n"
      "  env at: 1 put: n.\n"
      "  Processor yield.\n"
      "  env at: 2 put: n",
      env.slot);
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ("0 1", itemsOf(b, env.slot));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

TEST(Process, ActiveProcessInsideForkIsForked) {
  Boot b;
  ao::Root base(b.roots, active(b));
  ao::Root env(b.roots, newEnv(b, 3));
  run(b,
      "doIt: env\n"
      "  | p inner |\n"
      "  p := [inner := Processor activeProcess] fork.\n"
      "  Processor yield.\n"
      "  env at: 1 put: p.\n"
      "  env at: 2 put: inner.\n"
      "  env at: 3 put: Processor activeProcess",
      env.slot);
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  ASSERT_TRUE(at(b, env.slot, 1).isHeap());
  EXPECT_EQ(at(b, env.slot, 1), at(b, env.slot, 2));
  EXPECT_NE(base.slot, at(b, env.slot, 1));
  EXPECT_EQ(base.slot, at(b, env.slot, 3));
  EXPECT_EQ(base.slot, active(b));
}

// SPEC §3.4: one FIFO. A process forked by a fork goes behind the ones already there.
TEST(Process, ForkFifoOrder) {
  Boot b;
  EXPECT_EQ("1 2 3", runItems(b,
                              "doIt\n"
                              "  | log |\n"
                              "  log := OrderedCollection new.\n"
                              "  [log add: 1] fork.\n"
                              "  [log add: 2] fork.\n"
                              "  [log add: 3] fork.\n"
                              "  Processor yield.\n"
                              "  ^log"));
  EXPECT_EQ("1 2 3 4", runItems(b,
                                "doIt\n"
                                "  | log |\n"
                                "  log := OrderedCollection new.\n"
                                "  [log add: 1. [log add: 4] fork] fork.\n"
                                "  [log add: 2] fork.\n"
                                "  Processor yield.\n"
                                "  log add: 3.\n"
                                "  Processor yield.\n"
                                "  ^log"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

TEST(Process, YieldEmptyReturns) {
  Boot b;
  ao::Root base(b.roots, active(b));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(base.slot, active(b));
  ao::Root got(b.roots, run(b, "doIt\n  ^Processor yield"));
  EXPECT_EQ(b.wk.processor, got.slot);
  EXPECT_EQ(base.slot, active(b));
  EXPECT_FALSE(b.scheduler.nonBaseRunning());
}

// SPEC §3.4: the counters are the session's, whichever process ran the code.
TEST(Process, FiberCountersFoldIntoBase) {
  Boot b;
  const std::uint64_t before = b.ctx.interpretedSends;
  run(b,
      "doIt\n"
      "  [1 to: 1000 do: [:i | i printString]] fork.\n"
      "  Processor yield");
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_GE(b.ctx.interpretedSends - before, 1000u);
}

// ---- ブロックする操作 ----

TEST(Process, WaitBlocksUntilSignal) {
  Boot b;
  EXPECT_EQ("1 2 3 4", runItems(b,
                                "doIt\n"
                                "  | sem log |\n"
                                "  sem := Semaphore new.\n"
                                "  log := OrderedCollection new.\n"
                                "  [log add: 1. sem wait. log add: 4] fork.\n"
                                "  Processor yield.\n"
                                "  log add: 2.\n"
                                "  sem signal.\n"
                                "  log add: 3.\n"
                                "  Processor yield.\n"
                                "  ^log"));
  // Ping-pong between two forks while the base waits.
  EXPECT_EQ("1 10 2 20 3 30", runItems(b,
                                       "doIt\n"
                                       "  | ping pong done log |\n"
                                       "  ping := Semaphore new.\n"
                                       "  pong := Semaphore new.\n"
                                       "  done := Semaphore new.\n"
                                       "  log := OrderedCollection new.\n"
                                       "  [1 to: 3 do: [:i | ping wait. log add: i. pong signal].\n"
                                       "   done signal] fork.\n"
                                       "  [1 to: 3 do: [:i | ping signal. pong wait. log add: i * 10]] fork.\n"
                                       "  done wait.\n"
                                       "  ^log"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §3.4 デッドロック: the base's wait (or its own suspend) with nothing to run is undone and
// aborts; activeProcess stays the base. A fork that blocks with nothing to run switches back to
// the waiting base, which aborts the same way; the fork stays waiting.
TEST(Process, BaseDeadlockIsFailureActiveStaysBase) {
  Boot b;
  ao::Root base(b.roots, active(b));
  ao::Root env(b.roots, newEnv(b, 1));
  EXPECT_EQ("deadlock: no runnable process", runAbort(b,
                                                       "doIt: env\n"
                                                       "  | sem |\n"
                                                       "  sem := Semaphore new.\n"
                                                       "  env at: 1 put: sem.\n"
                                                       "  sem wait.\n"
                                                       "  ^1",
                                                       env.slot));
  ao::Root sem(b.roots, at(b, env.slot, 1));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_EQ(0, sizeOf(b, send1(b, sem.slot, "instVarAt:", smi(2))));
  EXPECT_EQ(smi(0), send1(b, sem.slot, "instVarAt:", smi(1)));
  EXPECT_TRUE(myList(b, base.slot).isNil());

  EXPECT_EQ("deadlock: no runnable process",
            runAbort(b, "doIt\n  Processor activeProcess suspend.\n  ^1"));
  EXPECT_EQ(base.slot, active(b));

  EXPECT_EQ("deadlock: no runnable process", runAbort(b,
                                                       "doIt: env\n"
                                                       "  | s1 s2 |\n"
                                                       "  s1 := Semaphore new.\n"
                                                       "  s2 := Semaphore new.\n"
                                                       "  env at: 1 put: s2.\n"
                                                       "  [s2 wait] fork.\n"
                                                       "  s1 wait.\n"
                                                       "  ^1",
                                                       env.slot));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_FALSE(b.scheduler.nonBaseRunning());
  EXPECT_EQ(1u, b.scheduler.liveFibers());
  ao::Root s2(b.roots, at(b, env.slot, 1));
  EXPECT_EQ(1, sizeOf(b, send1(b, s2.slot, "instVarAt:", smi(2))));
  send0(b, s2.slot, "signal");
  send0(b, b.wk.processor, "yield");
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
}

TEST(Process, SharedQueueProducerConsumer) {
  Boot b;
  EXPECT_EQ("10 20 30", runItems(b,
                                 "doIt\n"
                                 "  | q log done |\n"
                                 "  q := SharedQueue new.\n"
                                 "  log := OrderedCollection new.\n"
                                 "  done := Semaphore new.\n"
                                 "  [1 to: 3 do: [:i | log add: q next]. done signal] fork.\n"
                                 "  [1 to: 3 do: [:i | q nextPut: i * 10. Processor yield]] fork.\n"
                                 "  done wait.\n"
                                 "  ^log"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
}

TEST(Process, SharedQueueEmptyNextDeadlock) {
  Boot b;
  ao::Root base(b.roots, active(b));
  EXPECT_EQ("deadlock: no runnable process", runAbort(b, "doIt\n  ^SharedQueue new next"));
  EXPECT_EQ(base.slot, active(b));
}

// SPEC §3.4: nextPut: never waits; the queue has no bound.
TEST(Process, SharedQueueNextPutDoesNotWait) {
  Boot b;
  EXPECT_EQ("5 1 2 3", runItems(b,
                                "doIt\n"
                                "  | q log |\n"
                                "  q := SharedQueue new.\n"
                                "  log := OrderedCollection new.\n"
                                "  log add: (q nextPut: 5).\n"
                                "  q nextPut: 1.\n"
                                "  q nextPut: 2.\n"
                                "  q nextPut: 3.\n"
                                "  q next.\n"
                                "  log add: q next.\n"
                                "  log add: q next.\n"
                                "  log add: q next.\n"
                                "  ^log"));
}

// ---- 終了とエラー ----

TEST(Process, ForkDnuTerminatesOnlyFork) {
  Boot b;
  EXPECT_EQ("1 5 2 3",
            runItems(b,
                     "doIt\n"
                     "  | log |\n"
                     "  log := OrderedCollection new.\n"
                     "  [[log add: 1. nil aoNoSuchSelector. log add: 99] ensure: [log add: 5]] fork.\n"
                     "  [log add: 2] fork.\n"
                     "  Processor yield.\n"
                     "  log add: 3.\n"
                     "  ^log"));
  EXPECT_EQ(1u, b.scheduler.processFailures());
  EXPECT_EQ("doesNotUnderstand: #aoNoSuchSelector", b.scheduler.lastFailureReason());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §3.4: a ^ whose home lives in another process unwinds the fork to its body and fails it;
// the home's process is not unwound.
TEST(Process, ForkNlrToBaseHomeTerminates) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 1));
  ao::Root got(b.roots, run(b,
                            "doIt: env\n"
                            "  | log |\n"
                            "  log := OrderedCollection new.\n"
                            "  env at: 1 put: log.\n"
                            "  [log add: 1. [^7] ensure: [log add: 2]. log add: 99] fork.\n"
                            "  Processor yield.\n"
                            "  log add: 3.\n"
                            "  ^4",
                            env.slot));
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ(smi(4), got.slot);
  EXPECT_EQ("1 2 3", itemsOf(b, at(b, env.slot, 1)));
  EXPECT_EQ(1u, b.scheduler.processFailures());
  EXPECT_EQ("non-local return to another process", b.scheduler.lastFailureReason());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §3.4 terminate: a waiting process unwinds on itself, running its ensure: and ifCurtailed:
// cleanups; the sender goes on once it has ended. Not a failure; it cannot run afterwards.
TEST(Process, TerminateWaiterRunsEnsure) {
  Boot b;
  ao::Root base(b.roots, active(b));
  ao::Root env(b.roots, newEnv(b, 3));
  EXPECT_EQ("1 3 2 5 4",
            runItems(b,
                     "doIt: env\n"
                     "  | sem log p |\n"
                     "  sem := Semaphore new.\n"
                     "  log := OrderedCollection new.\n"
                     "  env at: 1 put: sem.\n"
                     "  p := [[[log add: 1. sem wait. log add: 99] ifCurtailed: [log add: 2]]\n"
                     "         ensure: [log add: 5]] fork.\n"
                     "  env at: 2 put: p.\n"
                     "  Processor yield.\n"
                     "  log add: 3.\n"
                     "  env at: 3 put: p terminate.\n"
                     "  log add: 4.\n"
                     "  ^log",
                     env.slot));
  ao::Root p(b.roots, at(b, env.slot, 2));
  EXPECT_EQ(p.slot, at(b, env.slot, 3));
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0, sizeOf(b, send1(b, at(b, env.slot, 1), "instVarAt:", smi(2))));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_TRUE(send0(b, p.slot, "resume").isEmpty());
  EXPECT_EQ("process cannot run", takeAbortReason(b));
  EXPECT_EQ(p.slot, send0(b, p.slot, "terminate"));
}

TEST(Process, TerminateSelfEndsWithoutFailure) {
  Boot b;
  EXPECT_EQ("1 3 2",
            runItems(b,
                     "doIt\n"
                     "  | log |\n"
                     "  log := OrderedCollection new.\n"
                     "  [[log add: 1. Processor activeProcess terminate. log add: 99]\n"
                     "     ensure: [log add: 3]] fork.\n"
                     "  Processor yield.\n"
                     "  log add: 2.\n"
                     "  ^log"));
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

TEST(Process, TerminateNotStartedNeverRuns) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 2));
  EXPECT_EQ("1", runItems(b,
                          "doIt: env\n"
                          "  | log p q |\n"
                          "  log := OrderedCollection new.\n"
                          "  p := [log add: 98] fork.\n"
                          "  q := [log add: 99] fork.\n"
                          "  q suspend.\n"
                          "  env at: 1 put: p.\n"
                          "  env at: 2 put: q.\n"
                          "  p terminate.\n"
                          "  q terminate.\n"
                          "  Processor yield.\n"
                          "  log add: 1.\n"
                          "  ^log",
                          env.slot));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0, sizeOf(b, readyList(b)));
  EXPECT_TRUE(send0(b, at(b, env.slot, 1), "resume").isEmpty());
  EXPECT_EQ("process cannot run", takeAbortReason(b));
  EXPECT_TRUE(send0(b, at(b, env.slot, 2), "resume").isEmpty());
  EXPECT_EQ("process cannot run", takeAbortReason(b));
}

// SPEC §3.4: the base never ends. Its own terminate fails its evaluation; a fork's terminate of
// the base fails that fork.
TEST(Process, TerminateBaseFails) {
  Boot b;
  ao::Root base(b.roots, active(b));
  EXPECT_EQ("process terminated",
            runAbort(b, "doIt\n  Processor activeProcess terminate.\n  ^1"));
  EXPECT_EQ(base.slot, active(b));
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_EQ("1", runItems(b,
                          "doIt\n"
                          "  | log base |\n"
                          "  log := OrderedCollection new.\n"
                          "  base := Processor activeProcess.\n"
                          "  [base terminate. log add: 99] fork.\n"
                          "  Processor yield.\n"
                          "  log add: 1.\n"
                          "  ^log"));
  EXPECT_EQ(1u, b.scheduler.processFailures());
  EXPECT_EQ("process terminated", b.scheduler.lastFailureReason());
  EXPECT_EQ(base.slot, active(b));
}

// A started process that is ready (it yielded) unwinds when terminated, and the sender goes on.
TEST(Process, TerminateReadyProcess) {
  Boot b;
  EXPECT_EQ("1 3 2",
            runItems(b,
                     "doIt\n"
                     "  | log p |\n"
                     "  log := OrderedCollection new.\n"
                     "  p := [[log add: 1. Processor yield. log add: 99] ensure: [log add: 3]] fork.\n"
                     "  Processor yield.\n"
                     "  p terminate.\n"
                     "  log add: 2.\n"
                     "  ^log"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
}

// SPEC §3.4 terminate: a cleanup that switches (yield) returns to the sender at once; the rest of
// the unwinding goes on when the process runs next.
TEST(Process, TerminateCleanupYieldReturnsToSender) {
  Boot b;
  EXPECT_EQ("1 2 3 4",
            runItems(b,
                     "doIt\n"
                     "  | sem log p |\n"
                     "  sem := Semaphore new.\n"
                     "  log := OrderedCollection new.\n"
                     "  p := [[sem wait] ensure: [log add: 1. Processor yield. log add: 4]] fork.\n"
                     "  Processor yield.\n"
                     "  [log add: 3] fork.\n"
                     "  p terminate.\n"
                     "  log add: 2.\n"
                     "  Processor yield.\n"
                     "  ^log"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
}

// SPEC §3.4: a fork runs on its own 8 MiB stack with its own guard: unbounded recursion there
// fails that process with stack overflow, and the base goes on.
TEST(Process, RecursionInForkFailsNoCrash) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 1));
  EXPECT_EQ("1 2", runItems(b,
                            "doIt: env\n"
                            "  | r log depth |\n"
                            "  log := OrderedCollection new.\n"
                            "  depth := 0.\n"
                            "  r := nil.\n"
                            "  r := [depth := depth + 1. (r value) + 1].\n"
                            "  [log add: 1. r value. log add: 99] fork.\n"
                            "  Processor yield.\n"
                            "  log add: 2.\n"
                            "  env at: 1 put: depth.\n"
                            "  ^log",
                            env.slot));
  EXPECT_EQ(1u, b.scheduler.processFailures());
  EXPECT_EQ("stack overflow", b.scheduler.lastFailureReason());
  // The guard is the fiber's 8 MiB stack, not the thread's range (outside which the fork would
  // overflow at once). ASan frames are large: a few hundred levels there.
  const ao::Oop depth = at(b, env.slot, 1);
  ASSERT_TRUE(depth.isSmallInteger());
  EXPECT_GT(depth.smallIntegerValue(), 100);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(smi(7), run(b, "doIt\n  ^3 + 4"));
}

TEST(Process, TooManyProcessesFails) {
  Boot b;
  run(b, "doIt\n  1 to: 256 do: [:i | [i] fork]");
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ(256u, b.scheduler.liveFibers());
  EXPECT_EQ("too many processes", runAbort(b, "doIt\n  [3] fork"));
  EXPECT_EQ(256u, b.scheduler.liveFibers());
  EXPECT_EQ(256, sizeOf(b, readyList(b)));
  b.scheduler.drain(1000);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_FALSE(b.ctx.aborting);
  // Room again.
  run(b, "doIt\n  [3] fork. Processor yield");
  EXPECT_EQ("<no abort>", takeAbortReason(b));
}

// SPEC §3.4 abandon: no cleanup runs and nothing counts as a failure, yet each fiber's frames
// unwind, so the roots come back to what they were.
TEST(Process, AbandonSkipsCleanupsAndRestoresRoots) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 1));
  // The first doIt of a Boot leaves one lasting root of its own; count from after it.
  run(b, "doIt: env\n  env at: 1 put: nil", env.slot);
  const ao::Roots::Counts before = b.roots.counts();
  run(b,
      "doIt: env\n"
      "  | sem log |\n"
      "  sem := Semaphore new.\n"
      "  log := OrderedCollection new.\n"
      "  env at: 1 put: log.\n"
      "  [[sem wait] ensure: [log add: 1]] fork.\n"
      "  [[Processor activeProcess suspend] ifCurtailed: [log add: 2]] fork.\n"
      "  [[Processor yield. sem wait] ensure: [log add: 3]] fork.\n"
      "  [log add: 4] fork.\n"
      "  Processor yield.\n"
      "  [log add: 5] fork",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ(4u, b.scheduler.liveFibers());
  b.scheduler.terminateAll(true);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ("4", itemsOf(b, at(b, env.slot, 1)));
  EXPECT_EQ(0, sizeOf(b, readyList(b)));
  const ao::Roots::Counts after = b.roots.counts();
  EXPECT_EQ(before.slots, after.slots);
  EXPECT_EQ(before.ranges, after.ranges);
  EXPECT_EQ(before.frameSlots, after.frameSlots);
  EXPECT_EQ(before.handles, after.handles);
  EXPECT_EQ(before.attachedStacks, after.attachedStacks);
}

// terminateAll(false) is terminate for each: the cleanups run (SPEC §4.4 ao --test).
TEST(Process, TerminateAllRunsCleanups) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 1));
  run(b,
      "doIt: env\n"
      "  | sem log |\n"
      "  sem := Semaphore new.\n"
      "  log := OrderedCollection new.\n"
      "  env at: 1 put: log.\n"
      "  [[sem wait] ensure: [log add: 1]] fork.\n"
      "  [[sem wait] ensure: [log add: 2. sem wait. log add: 99]] fork.\n"
      "  Processor yield.\n"
      "  [log add: 98] fork",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ(3u, b.scheduler.liveFibers());
  b.scheduler.terminateAll(false);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ("1 2", itemsOf(b, at(b, env.slot, 1)));
}

// SPEC §3.4 terminate: a process that a signal made ready, but that has not returned from wait
// yet (ready, or suspended after that), gives the signal back to the semaphore when terminated, as
// if the semaphore were signalled again: the next waiter, or the next wait, gets it. Suspend does
// not give it back (the process returns from wait once resumed), and neither does abandon.
TEST(Process, TerminateSignaledWaiterGivesSignalBack) {
  Boot b;
  // The element the terminated reader was woken for goes to the next next.
  EXPECT_EQ("42", runItems(b,
                           "doIt\n"
                           "  | q log a |\n"
                           "  q := SharedQueue new.\n"
                           "  log := OrderedCollection new.\n"
                           "  a := [log add: q next. log add: 1] fork.\n"
                           "  Processor yield.\n"
                           "  q nextPut: 42.\n"
                           "  a terminate.\n"
                           "  log add: q next.\n"
                           "  ^log"));
  // Given back, the signal wakes the reader waiting behind.
  EXPECT_EQ("42 3", runItems(b,
                              "doIt\n"
                              "  | q log a c |\n"
                              "  q := SharedQueue new.\n"
                              "  log := OrderedCollection new.\n"
                              "  a := [log add: q next. log add: 1] fork.\n"
                              "  c := [log add: q next. log add: 3] fork.\n"
                              "  Processor yield.\n"
                              "  q nextPut: 42.\n"
                              "  a terminate.\n"
                              "  Processor yield.\n"
                              "  ^log"));
  // Suspended after the signal: suspend keeps it, terminate gives it back.
  EXPECT_EQ("0 1 7", runItems(b,
                              "doIt\n"
                              "  | s log a |\n"
                              "  s := Semaphore new.\n"
                              "  log := OrderedCollection new.\n"
                              "  a := [s wait. log add: 1] fork.\n"
                              "  Processor yield.\n"
                              "  s signal.\n"
                              "  a suspend.\n"
                              "  log add: (s instVarAt: 1).\n"
                              "  a terminate.\n"
                              "  log add: (s instVarAt: 1).\n"
                              "  s wait.\n"
                              "  log add: 7.\n"
                              "  ^log"));
  // Suspended and resumed, it returns from the wait it was signalled in.
  EXPECT_EQ("0 1", runItems(b,
                             "doIt\n"
                             "  | s log a |\n"
                             "  s := Semaphore new.\n"
                             "  log := OrderedCollection new.\n"
                             "  a := [s wait. log add: 1] fork.\n"
                             "  Processor yield.\n"
                             "  s signal.\n"
                             "  a suspend.\n"
                             "  log add: (s instVarAt: 1).\n"
                             "  a resume.\n"
                             "  Processor yield.\n"
                             "  ^log"));
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  // Abandon gives nothing back.
  ao::Root env(b.roots, newEnv(b, 1));
  run(b,
      "doIt: env\n"
      "  | s |\n"
      "  s := Semaphore new.\n"
      "  env at: 1 put: s.\n"
      "  [s wait] fork.\n"
      "  Processor yield.\n"
      "  s signal",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ(1u, b.scheduler.liveFibers());
  b.scheduler.terminateAll(true);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(smi(0), send1(b, at(b, env.slot, 1), "instVarAt:", smi(1)));
}

// SPEC §3.10 busy: the interpreter is running while a process other than the base runs, even when
// the base itself is not in Interpreter::run (a drain from C++, as ao_eval does).
TEST(Process, InterpreterRunningWhileForkRunsFromDrain) {
  Boot b;
  static ao::CallContext* base;
  static int seen;
  base = &b.ctx;
  seen = -1;
  b.ctx.inspectHook = [](ao::CallContext&, ao::Oop) {
    seen = (base->depth == 0 && ao::interpreterRunning(*base)) ? 1 : 0;
  };
  run(b, "doIt\n  [3 inspect] fork");
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ(-1, seen);
  b.scheduler.drain(10);
  EXPECT_EQ(1, seen);
  EXPECT_FALSE(ao::interpreterRunning(b.ctx));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §3.4 signal, §3.11: a waiter this session has no record of (Process new, a Process from a
// loaded image) cannot run: signal drops it and looks at the next one.
TEST(Process, SignalDropsWaitersThatCannotRun) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 2));
  run(b,
      "doIt: env\n"
      "  | sem log |\n"
      "  sem := Semaphore new.\n"
      "  log := OrderedCollection new.\n"
      "  env at: 1 put: sem.\n"
      "  env at: 2 put: log.\n"
      "  (sem instVarAt: 2) add: Process new; add: Process new.\n"
      "  [sem wait. log add: 1] fork.\n"
      "  Processor yield",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  ao::Root sem(b.roots, at(b, env.slot, 1));
  ao::Root waitList(b.roots, send1(b, sem.slot, "instVarAt:", smi(2)));
  EXPECT_EQ(3, sizeOf(b, waitList.slot));
  EXPECT_EQ(sem.slot, send0(b, sem.slot, "signal"));
  EXPECT_EQ(0, sizeOf(b, waitList.slot));
  EXPECT_EQ(smi(0), send1(b, sem.slot, "instVarAt:", smi(1)));
  send0(b, b.wk.processor, "yield");
  EXPECT_EQ("1", itemsOf(b, at(b, env.slot, 2)));
  // Nobody waits: the signal is kept.
  send0(b, sem.slot, "signal");
  EXPECT_EQ(smi(1), send1(b, sem.slot, "instVarAt:", smi(1)));
}

// SPEC §3.2: every process's roots survive the GC while it is parked. 50 fibers wait with
// objects only their own frames hold, under GC stress and full GCs, then all end with the right
// values.
TEST(Process, FiftyWaitersSurviveGcStressAndOldGc) {
  Boot b;
  const std::uint32_t stress = b.heap.gcStress();
  ao::Root env(b.roots, newEnv(b, 2));
  run(b,
      "doIt: env\n"
      "  | sem results |\n"
      "  sem := Semaphore new.\n"
      "  results := Array new: 50.\n"
      "  env at: 1 put: sem.\n"
      "  env at: 2 put: results.\n"
      "  1 to: 50 do: [:i |\n"
      "    [| a | a := Array new: 2. a at: 1 put: i printString. sem wait.\n"
      "     a at: 2 put: i * 2. results at: i put: a] fork].\n"
      "  Processor yield",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  ASSERT_EQ(50u, b.scheduler.liveFibers());
  b.heap.setGcStress(1);
  {
    ao::Gc gc(b.heap, b.roots);
    gc.collectNursery();
    gc.collectOld();
  }
  run(b,
      "doIt: env\n"
      "  | sem |\n"
      "  sem := env at: 1.\n"
      "  1 to: 50 do: [:i | sem signal. i \\\\ 2 = 1 ifTrue: [Processor yield]].\n"
      "  Processor yield",
      env.slot);
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  {
    ao::Gc gc(b.heap, b.roots);
    gc.collectOld();
  }
  b.heap.setGcStress(stress);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  std::string want;
  for (int i = 1; i <= 50; ++i) {
    want += (i == 1 ? "" : " ") + std::string("#('") + std::to_string(i) + "' " +
            std::to_string(2 * i) + ")";
  }
  EXPECT_EQ(want, itemsOf(b, at(b, env.slot, 2)));
}

// SPEC §4.1: the OrderedCollections behind the FIFOs (the ready queue, a linkedList) do not keep
// growing.
TEST(Process, ReadyQueueArrayStaysSmall) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 2));
  run(b,
      "doIt: env\n"
      "  | ping pong |\n"
      "  ping := Semaphore new.\n"
      "  pong := Semaphore new.\n"
      "  env at: 1 put: ping.\n"
      "  env at: 2 put: pong.\n"
      "  [1 to: 1000 do: [:i | ping wait. pong signal]] fork.\n"
      "  [1 to: 1000 do: [:i | Processor yield]] fork.\n"
      "  1 to: 1000 do: [:i | ping signal. Processor yield. pong wait]",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  b.scheduler.drain(1000);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  auto arraySize = [&b](ao::Oop list) -> std::uint32_t {
    ao::Root array(b.roots, send1(b, list, "instVarAt:", smi(1)));
    return array.slot.isHeap() ? b.heap.size(array.slot) : 0u;
  };
  EXPECT_LE(arraySize(readyList(b)), 64u);
  EXPECT_LE(arraySize(send1(b, at(b, env.slot, 1), "instVarAt:", smi(2))), 64u);
  EXPECT_LE(arraySize(send1(b, at(b, env.slot, 2), "instVarAt:", smi(2))), 64u);
}

// ---- Codex review of B10 ----

// SPEC §3.4 SharedQueue next: a reader that returns from wait without a signal (suspend and
// resume) takes an element when there is one, and waits again when the queue is empty. The queue
// stays in step with readSynch: a later next on the empty queue waits (here the base deadlocks)
// instead of answering nil.
TEST(Process, SharedQueueNextAfterSuspendResumeWaitsWhenEmpty) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 1));
  EXPECT_EQ("deadlock: no runnable process", runAbort(b,
                                                      "doIt: env\n"
                                                      "  | q p log |\n"
                                                      "  q := SharedQueue new.\n"
                                                      "  log := OrderedCollection new.\n"
                                                      "  env at: 1 put: log.\n"
                                                      "  p := [log add: q next] fork.\n"
                                                      "  Processor yield.\n"
                                                      "  p suspend.\n"
                                                      "  q nextPut: 42.\n"
                                                      "  p resume.\n"
                                                      "  Processor yield.\n"
                                                      "  log add: q next.\n"
                                                      "  ^log",
                                                      env.slot));
  EXPECT_EQ("42", itemsOf(b, at(b, env.slot, 1)));
  // Resumed while the queue is empty, it waits again and takes the next element put.
  EXPECT_EQ("0 7", runItems(b,
                            "doIt\n"
                            "  | q p log |\n"
                            "  q := SharedQueue new.\n"
                            "  log := OrderedCollection new.\n"
                            "  p := [log add: q next] fork.\n"
                            "  Processor yield.\n"
                            "  p suspend.\n"
                            "  p resume.\n"
                            "  Processor yield.\n"
                            "  log add: log size.\n"
                            "  q nextPut: 7.\n"
                            "  Processor yield.\n"
                            "  ^log"));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
}

// SPEC §3.4: the out-of-memory mark is each process's own. A fork that cannot allocate fails
// alone, and the base, which allocated fine, does not see the mark. A fork whose body ends without
// an abort after an allocation failed (the hook stands for one) fails with the same reason.
TEST(Process, OutOfMemoryMarkIsPerProcess) {
  Boot b;
  EXPECT_EQ(smi(7), run(b, "doIt\n  [Array new: 600000000] fork.\n  Processor yield.\n  ^7"));
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_FALSE(b.heap.outOfMemory());
  EXPECT_EQ(1u, b.scheduler.processFailures());
  EXPECT_EQ("out of memory", b.scheduler.lastFailureReason());
  b.ctx.inspectHook = [](ao::CallContext& ctx, ao::Oop) { ctx.heap.setOutOfMemory(); };
  ao::Root env(b.roots, newEnv(b, 1));
  EXPECT_EQ(smi(7), run(b,
                        "doIt: env\n"
                        "  | log |\n"
                        "  log := OrderedCollection new.\n"
                        "  env at: 1 put: log.\n"
                        "  [3 inspect. log add: 1] fork.\n"
                        "  Processor yield.\n"
                        "  log add: 2.\n"
                        "  ^7",
                        env.slot));
  b.ctx.inspectHook = nullptr;
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_FALSE(b.heap.outOfMemory());
  EXPECT_EQ("1 2", itemsOf(b, at(b, env.slot, 1)));
  EXPECT_EQ(2u, b.scheduler.processFailures());
  EXPECT_EQ("out of memory", b.scheduler.lastFailureReason());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

// SPEC §4.4: terminateAll(false) drains after the terminates, so a cleanup that yielded goes on to
// its end, and a process that a cleanup forked is terminated in turn (its cleanup runs too). Only
// what stays blocked would be abandoned.
TEST(Process, TerminateAllFinishesCleanupsThatYieldOrFork) {
  Boot b;
  ao::Root env(b.roots, newEnv(b, 1));
  run(b,
      "doIt: env\n"
      "  | s log |\n"
      "  s := Semaphore new.\n"
      "  log := OrderedCollection new.\n"
      "  env at: 1 put: log.\n"
      "  [[s wait] ensure: [[nil] fork. log add: 1. Processor yield. log add: 2]] fork.\n"
      "  [[s wait] ensure: [[[s wait] ensure: [log add: 4]] fork. log add: 3]] fork.\n"
      "  Processor yield",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  b.scheduler.drain(ao::Scheduler::kDrainRounds);
  EXPECT_EQ(2u, b.scheduler.liveFibers());
  b.scheduler.terminateAll(false);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ("1 3 2 4", itemsOf(b, at(b, env.slot, 1)));
}

// SPEC §3.4 signal, nextPut:: when the ready queue cannot take the waiter (its Array cannot double
// under old's max, SPEC §3.2), the operation fails with out of memory and changes nothing: the
// waiter still waits in its linkedList, and the SharedQueue has no new element. Once the queue can
// take it, the next signal wakes it.
TEST(Process, WakeThatCannotEnqueueChangesNothing) {
  Boot b(1 << 20, 1 << 20, 8 << 20);  // old は 8 MiB で頭打ち
  ao::Root env(b.roots, newEnv(b, 4));
  run(b,
      "doIt: env\n"
      "  | s q log |\n"
      "  s := Semaphore new.\n"
      "  q := SharedQueue new.\n"
      "  log := OrderedCollection new.\n"
      "  env at: 1 put: s; at: 2 put: q; at: 3 put: log.\n"
      "  [s wait. log add: 1] fork.\n"
      "  [log add: q next] fork.\n"
      "  Processor yield",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  ASSERT_EQ(2u, b.scheduler.liveFibers());
  // Processor's ready queue becomes a full OrderedCollection whose Array is too big to double.
  run(b,
      "doIt: env\n"
      "  | full |\n"
      "  full := OrderedCollection new.\n"
      "  full instVarAt: 1 put: (Array new: 600000); instVarAt: 3 put: 600000.\n"
      "  env at: 4 put: (Processor instVarAt: 1).\n"
      "  Processor instVarAt: 1 put: full",
      env.slot);
  ASSERT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ("out of memory", runAbort(b, "doIt: env\n  (env at: 1) signal", env.slot));
  EXPECT_EQ("out of memory", runAbort(b, "doIt: env\n  (env at: 2) nextPut: 5", env.slot));
  b.heap.clearOutOfMemory();
  ao::Root sem(b.roots, at(b, env.slot, 1));
  EXPECT_EQ(1, sizeOf(b, send1(b, sem.slot, "instVarAt:", smi(2))));
  EXPECT_EQ(smi(0), send1(b, sem.slot, "instVarAt:", smi(1)));
  ao::Root q(b.roots, at(b, env.slot, 2));
  EXPECT_EQ(0, sizeOf(b, send1(b, q.slot, "instVarAt:", smi(1))));
  run(b,
      "doIt: env\n"
      "  Processor instVarAt: 1 put: (env at: 4).\n"
      "  (env at: 1) signal.\n"
      "  (env at: 2) nextPut: 6.\n"
      "  Processor yield",
      env.slot);
  EXPECT_EQ("<no abort>", takeAbortReason(b));
  EXPECT_EQ("1 6", itemsOf(b, at(b, env.slot, 3)));
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
}

// SPEC §3.13 評価プロセス: forkEval makes a live process that is in no list; awaitEval runs it,
// and the ready processes whenever it switches, until it ends. It is then gone, its value is the
// evaluation's, and the base is the active process again.
TEST(Process, EvalProcessRunsUntilItEndsAndLeavesNothing) {
  Boot b;
  ao::Root base(b.roots, active(b));
  auto doIt = ao::compiler::compileMethod(
      "doIt | env | env := Array new: 2. env at: 1 put: Processor activeProcess. "
      "[env at: 2 put: 5] fork. Processor yield. ^(env at: 2) + 1");
  ASSERT_TRUE(doIt.ok) << doIt.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, doIt.image, b.wk.objectClass));
  const std::uint64_t pid = b.scheduler.forkEval(b.ctx, cm.slot, 2);
  ASSERT_NE(0u, pid);
  EXPECT_EQ(1u, b.scheduler.liveFibers());
  EXPECT_EQ(0, sizeOf(b, readyList(b)));
  EXPECT_EQ(ao::Scheduler::EvalEnd::Finished, b.scheduler.awaitEval(pid));
  EXPECT_EQ(smi(6), b.scheduler.evalValue());
  b.scheduler.clearEvalValue();
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ(base.slot, active(b));
  // A failure is the evaluation's reason, not a process failure.
  auto bad = ao::compiler::compileMethod("doIt ^nil foo");
  ASSERT_TRUE(bad.ok);
  ao::Root cm2(b.roots, ao::boxMethodImage(b.ctx, bad.image, b.wk.objectClass));
  const std::uint64_t pid2 = b.scheduler.forkEval(b.ctx, cm2.slot, 1);
  ASSERT_NE(pid, pid2);
  EXPECT_EQ(ao::Scheduler::EvalEnd::Failed, b.scheduler.awaitEval(pid2));
  EXPECT_EQ("doesNotUnderstand: #foo", b.scheduler.evalReason());
  EXPECT_EQ(0u, b.scheduler.processFailures());
  EXPECT_EQ(0u, b.scheduler.liveFibers());
}

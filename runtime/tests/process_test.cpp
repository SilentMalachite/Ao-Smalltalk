#include "test_support.hpp"

#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"

#include <gtest/gtest.h>

TEST(Process, SchedulerHoldsOneActiveProcess) {
  Boot b;
  auto p = send0(b, b.wk.processor, "activeProcess");
  ASSERT_TRUE(p.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p));
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

TEST(Process, ResumeYieldSuspendRoundTrip) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root p1(b.roots, send0(b, b.wk.processor, "activeProcess"));
  ASSERT_TRUE(p1.slot.isHeap());
  ao::Root p2(b.roots, send0(b, b.wk.processClass, "new"));
  ASSERT_TRUE(p2.slot.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p2.slot));
  EXPECT_EQ(p2.slot, send0(b, p2.slot, "resume"));
  EXPECT_EQ(p1.slot, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p2.slot, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(p2.slot, send0(b, p2.slot, "suspend"));
  EXPECT_EQ(p1.slot, send0(b, b.wk.processor, "activeProcess"));
}

TEST(Process, PriorityColonStoresSmallInteger) {
  Boot b;
  auto p = send0(b, b.wk.processClass, "new");
  ASSERT_TRUE(p.isHeap());
  auto five = ao::Oop::fromSmallInteger(5);
  EXPECT_EQ(p, send1(b, p, "priority:", five));
  EXPECT_EQ(five, send1(b, p, "instVarAt:", ao::Oop::fromSmallInteger(3)));
}

TEST(Process, BlockContextForkCreatesAndResumesProcess) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root p1(b.roots, send0(b, b.wk.processor, "activeProcess"));
  auto fn = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(1);
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, fn, 0));
  ASSERT_TRUE(blk.slot.isHeap());
  ao::Root p(b.roots, send0(b, blk.slot, "fork"));
  ASSERT_TRUE(p.slot.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p.slot));
  EXPECT_EQ(blk.slot, send1(b, p.slot, "instVarAt:", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(p1.slot, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p.slot, send0(b, b.wk.processor, "activeProcess"));
}

TEST(Process, WaitSuspendsThenSignalResumes) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root p1(b.roots, send0(b, b.wk.processor, "activeProcess"));
  ao::Root p2(b.roots, send0(b, b.wk.processClass, "new"));
  ASSERT_TRUE(p2.slot.isHeap());
  EXPECT_EQ(p2.slot, send0(b, p2.slot, "resume"));
  ao::Root sem(b.roots, send0(b, b.wk.semaphoreClass, "new"));
  ASSERT_TRUE(sem.slot.isHeap());
  EXPECT_EQ(b.wk.semaphoreClass, b.heap.klass(sem.slot));
  EXPECT_EQ(sem.slot, send0(b, sem.slot, "wait"));
  EXPECT_EQ(p2.slot, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(sem.slot, send0(b, sem.slot, "signal"));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p1.slot, send0(b, b.wk.processor, "activeProcess"));
}

TEST(Process, WaitKeepsMyListAndResumeDoesNotDoubleEnqueue) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root p1(b.roots, send0(b, b.wk.processor, "activeProcess"));
  ao::Root p2(b.roots, send0(b, b.wk.processClass, "new"));
  ASSERT_TRUE(p2.slot.isHeap());
  EXPECT_EQ(p2.slot, send0(b, p2.slot, "resume"));
  ao::Root sem(b.roots, send0(b, b.wk.semaphoreClass, "new"));
  ASSERT_TRUE(sem.slot.isHeap());
  EXPECT_EQ(sem.slot, send0(b, sem.slot, "wait"));
  EXPECT_EQ(p2.slot, send0(b, b.wk.processor, "activeProcess"));
  ao::Root waitList(b.roots, send1(b, p1.slot, "instVarAt:", ao::Oop::fromSmallInteger(4)));
  ASSERT_TRUE(waitList.slot.isHeap());
  EXPECT_EQ(b.wk.orderedCollectionClass, b.heap.klass(waitList.slot));
  EXPECT_EQ(1, send0(b, waitList.slot, "size").smallIntegerValue());
  EXPECT_EQ(p1.slot, send1(b, waitList.slot, "at:", ao::Oop::fromSmallInteger(1)));
  EXPECT_EQ(p1.slot, send0(b, p1.slot, "resume"));
  EXPECT_EQ(waitList.slot, send1(b, p1.slot, "instVarAt:", ao::Oop::fromSmallInteger(4)));
  EXPECT_EQ(1, send0(b, waitList.slot, "size").smallIntegerValue());
  ao::Root quiescent(b.roots,
                     send1(b, b.wk.processor, "instVarAt:", ao::Oop::fromSmallInteger(1)));
  ASSERT_TRUE(quiescent.slot.isHeap());
  EXPECT_EQ(0, send0(b, quiescent.slot, "size").smallIntegerValue());
  EXPECT_EQ(p2.slot, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(sem.slot, send0(b, sem.slot, "signal"));
  EXPECT_EQ(1, send0(b, quiescent.slot, "size").smallIntegerValue());
  EXPECT_EQ(p1.slot, send1(b, quiescent.slot, "at:", ao::Oop::fromSmallInteger(1)));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p1.slot, send0(b, b.wk.processor, "activeProcess"));
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

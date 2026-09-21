#include "test_support.hpp"

#include "ao/Globals.hpp"

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
  auto p1 = send0(b, b.wk.processor, "activeProcess");
  ASSERT_TRUE(p1.isHeap());
  auto p2 = send0(b, b.wk.processClass, "new");
  ASSERT_TRUE(p2.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p2));
  EXPECT_EQ(p2, send0(b, p2, "resume"));
  EXPECT_EQ(p1, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p2, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(p2, send0(b, p2, "suspend"));
  EXPECT_EQ(p1, send0(b, b.wk.processor, "activeProcess"));
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
  auto p1 = send0(b, b.wk.processor, "activeProcess");
  auto fn = [](ao::CallContext&, ao::Oop, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(1);
  };
  auto blk = ao::makeNativeBlock(b.ctx, fn, 0);
  ASSERT_TRUE(blk.isHeap());
  auto p = send0(b, blk, "fork");
  ASSERT_TRUE(p.isHeap());
  EXPECT_EQ(b.wk.processClass, b.heap.klass(p));
  EXPECT_EQ(blk, send1(b, p, "instVarAt:", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(p1, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p, send0(b, b.wk.processor, "activeProcess"));
}

TEST(Process, WaitSuspendsThenSignalResumes) {
  Boot b;
  auto p1 = send0(b, b.wk.processor, "activeProcess");
  auto p2 = send0(b, b.wk.processClass, "new");
  ASSERT_TRUE(p2.isHeap());
  EXPECT_EQ(p2, send0(b, p2, "resume"));
  auto sem = send0(b, b.wk.semaphoreClass, "new");
  ASSERT_TRUE(sem.isHeap());
  EXPECT_EQ(b.wk.semaphoreClass, b.heap.klass(sem));
  EXPECT_EQ(sem, send0(b, sem, "wait"));
  EXPECT_EQ(p2, send0(b, b.wk.processor, "activeProcess"));
  EXPECT_EQ(sem, send0(b, sem, "signal"));
  EXPECT_EQ(b.wk.processor, send0(b, b.wk.processor, "yield"));
  EXPECT_EQ(p1, send0(b, b.wk.processor, "activeProcess"));
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
  auto ctx = send0(b, b.wk.methodContextClass, "new");
  ASSERT_TRUE(ctx.isHeap());
  EXPECT_EQ(b.wk.methodContextClass, b.heap.klass(ctx));
  auto meth = send0(b, ao::Oop::fromSmallInteger(1), "class");
  auto sender = send0(b, b.wk.objectClass, "new");
  auto rcvr = ao::Oop::fromSmallInteger(7);
  EXPECT_EQ(sender, send2(b, ctx, "instVarAt:put:", ao::Oop::fromSmallInteger(1), sender));
  EXPECT_EQ(meth, send2(b, ctx, "instVarAt:put:", ao::Oop::fromSmallInteger(4), meth));
  EXPECT_EQ(rcvr, send2(b, ctx, "instVarAt:put:", ao::Oop::fromSmallInteger(5), rcvr));
  EXPECT_EQ(sender, send0(b, ctx, "sender"));
  EXPECT_EQ(meth, send0(b, ctx, "method"));
  EXPECT_EQ(rcvr, send0(b, ctx, "receiver"));
}

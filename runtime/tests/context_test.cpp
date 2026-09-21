#include "test_support.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include <gtest/gtest.h>

namespace {

struct Root {
  ao::Roots& roots;
  ao::Oop slot;
  explicit Root(ao::Roots& r, ao::Oop v = ao::Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

}  // namespace

TEST(ContextGc, MethodContextSurvivesNurseryCollection) {
  Boot b;
  auto cm = b.heap.allocate(b.wk.compiledMethodClass, ao::kCmSlotCount, 0);
  ASSERT_TRUE(cm.isHeap());
  auto ctxOop = ao::Context::createMethod(b.ctx, cm, ao::Oop::fromSmallInteger(1),
                                           ao::Oop::nil(), 0);
  ASSERT_TRUE(ctxOop.isHeap());
  EXPECT_EQ(b.wk.methodContextClass, b.heap.klass(ctxOop));
  EXPECT_EQ(ao::kMethodContextSlotCount, b.heap.size(ctxOop));
  Root held(b.roots, ctxOop);
  ao::Gc gc(b.heap, b.roots);
  gc.collectNursery();
  EXPECT_TRUE(held.slot.isHeap());
  EXPECT_EQ(b.wk.methodContextClass, b.heap.klass(held.slot));
  EXPECT_TRUE(b.heap.slotAt(held.slot, ao::kCtxSender).isNil());
  EXPECT_EQ(0, b.heap.slotAt(held.slot, ao::kCtxArgc).smallIntegerValue());
}

TEST(ContextGc, BlockContextKeepsHomeAndCopied) {
  Boot b;
  auto home = b.heap.allocate(b.wk.methodContextClass, ao::kMethodContextSlotCount, 0);
  auto copied = ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0);
  auto blk = ao::Context::createBlock(b.ctx, ao::Oop::nil(), ao::Oop::fromSmallInteger(7),
                                       home, copied, ao::Oop::nil(), 1);
  ASSERT_TRUE(blk.isHeap());
  EXPECT_EQ(b.wk.blockContextClass, b.heap.klass(blk));
  EXPECT_EQ(ao::kBlockSlotCount, b.heap.size(blk));
  EXPECT_EQ(home, b.heap.slotAt(blk, ao::kBlockHome));
  EXPECT_EQ(copied, b.heap.slotAt(blk, ao::kBlockCopied));
  EXPECT_EQ(7, b.heap.slotAt(blk, ao::kCtxReceiver).smallIntegerValue());
}

TEST(ContextGc, NativeBlockThunkStillValues) {
  Boot b;
  auto body = [](ao::CallContext&, ao::Oop, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(9);
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 0);
  auto got = send0(b, blk, "value");
  EXPECT_EQ(9, got.smallIntegerValue());
}

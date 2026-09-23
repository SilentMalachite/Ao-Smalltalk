#include "test_support.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include <gtest/gtest.h>

TEST(CompiledMethod, LayoutNativeCodeNil) {
  Boot b;
  // CompiledMethod::create は allocateRetry を通るので、それをまたぐ値はルートしておく。
  ao::Root bytes(b.roots, b.heap.allocate(b.wk.byteArrayClass, 1, ao::kFlagBytes));
  ASSERT_TRUE(bytes.slot.isHeap());
  b.heap.bytes(bytes.slot)[0] = std::byte{7};
  ao::Root lits(b.roots, ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0));
  ao::Root sel(b.roots, b.wk.intern("foo"));
  ao::Root cm(b.roots, ao::CompiledMethod::create(b.ctx, 0, 0, 0, lits.slot, bytes.slot, sel.slot,
                                                  b.wk.objectClass));
  ASSERT_TRUE(cm.slot.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(cm.slot));
  EXPECT_TRUE(b.heap.slotAt(cm.slot, ao::kCmSlotNativeCode).isNil());
  EXPECT_EQ(6u, b.heap.size(cm.slot));
  auto n = send0(b, cm.slot, "nativeCode");
  EXPECT_TRUE(n.isNil());
  auto ba = send0(b, cm.slot, "bytecodes");
  EXPECT_EQ(bytes.slot, ba);
}

TEST(CompiledMethod, BoxFromImageNativeCodeNil) {
  Boot b;
  auto img = ao::compiler::compileMethod("foo\n  ^1 + 2");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  ASSERT_TRUE(cm.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(cm));
  EXPECT_TRUE(send0(b, cm, "nativeCode").isNil());
  EXPECT_EQ(0, send0(b, cm, "numArgs").smallIntegerValue());
}

TEST(CompiledMethod, BoxLiteralArrayPseudoObjects) {
  Boot b;
  auto img = ao::compiler::compileMethod("foo\n  ^#(nil true false)");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  ASSERT_TRUE(cm.isHeap());
  auto lits = b.heap.slotAt(cm, ao::kCmSlotLiterals);
  ASSERT_TRUE(lits.isHeap());
  ASSERT_GE(b.heap.size(lits), 1u);
  auto arr = b.heap.slotAt(lits, 0);
  ASSERT_TRUE(arr.isHeap());
  ASSERT_EQ(3u, b.heap.size(arr));
  EXPECT_TRUE(b.heap.slotAt(arr, 0).isNil());
  EXPECT_TRUE(b.heap.slotAt(arr, 1).isTrue());
  EXPECT_TRUE(b.heap.slotAt(arr, 2).isFalse());
}

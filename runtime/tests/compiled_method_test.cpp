#include "test_support.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include <gtest/gtest.h>

TEST(CompiledMethod, LayoutNativeCodeNil) {
  Boot b;
  auto bytes = b.heap.allocate(b.wk.byteArrayClass, 1, ao::kFlagBytes);
  ASSERT_TRUE(bytes.isHeap());
  b.heap.bytes(bytes)[0] = std::byte{7};
  auto lits = ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0);
  auto sel = b.wk.intern("foo");
  auto cm = ao::CompiledMethod::create(b.ctx, 0, 0, 0, lits, bytes, sel, b.wk.objectClass);
  ASSERT_TRUE(cm.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(cm));
  EXPECT_TRUE(b.heap.slotAt(cm, ao::kCmSlotNativeCode).isNil());
  EXPECT_EQ(6u, b.heap.size(cm));
  auto n = send0(b, cm, "nativeCode");
  EXPECT_TRUE(n.isNil());
  auto ba = send0(b, cm, "bytecodes");
  EXPECT_EQ(bytes, ba);
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

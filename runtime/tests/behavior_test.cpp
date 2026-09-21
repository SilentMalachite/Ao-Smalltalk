#include "test_support.hpp"

#include <gtest/gtest.h>

TEST(Behavior, ObjectNewClassIsObject) {
  Boot b;
  auto o = send0(b, b.wk.objectClass, "new");
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.objectClass, send0(b, o, "class"));
  EXPECT_TRUE(send0(b, o, "isNil").isFalse());
}

TEST(Behavior, MetaclassNewIsForbidden) {
  Boot b;
  auto r = send0(b, b.wk.metaclassClass, "new");
  EXPECT_TRUE(r.isHeap() || r.isNil()); // error string or nil; 実装は intern した 'shouldNotImplement'
}

TEST(Behavior, SubclassIsRegistered) {
  Boot b;
  auto name = b.wk.intern("Foo");
  auto empty = b.wk.intern("");
  ao::Oop args[5] = {name, empty, empty, empty, empty};
  auto sel = ao::Symbol::intern(b.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  auto foo = ao::send(b.ctx, b.wk.objectClass, sel, args, 5, nullptr);
  ASSERT_TRUE(foo.isHeap());
  EXPECT_EQ(foo, b.wk.named("Foo"));
  EXPECT_EQ(b.wk.objectClass, b.heap.slotAt(foo, ao::kClassSlotSuperclass));
}

TEST(Behavior, MetaclassNewReturnsShouldNotImplementString) {
  Boot b;
  auto r = send0(b, b.wk.metaclassClass, "new");
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(r));
  EXPECT_EQ("shouldNotImplement", ao::Str::toUtf8(b.heap, r));
}

TEST(Behavior, ArrayNewIsEmptyArray) {
  Boot b;
  auto o = send0(b, b.wk.arrayClass, "new");
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(o));
  EXPECT_EQ(0u, b.heap.size(o));
}

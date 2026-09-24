#include "test_support.hpp"

#include "ao/Globals.hpp"
#include "ao/NativeMethod.hpp"

#include <gtest/gtest.h>

static ao::Oop makeSubclass(Boot& b, const char* name) {
  auto n = b.wk.intern(name);
  auto empty = b.wk.intern("");
  ao::Oop args[5] = {n, empty, empty, empty, empty};
  auto sel = ao::Symbol::intern(
      b.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  return ao::send(b.ctx, b.wk.objectClass, sel, args, 5, nullptr);
}

TEST(Behavior, ObjectNewClassIsObject) {
  Boot b;
  auto o = send0(b, b.wk.objectClass, "new");
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.objectClass, send0(b, o, "class"));
  EXPECT_TRUE(send0(b, o, "isNil").isFalse());
}

TEST(Behavior, MetaclassNewIsForbidden) {
  Boot b;
  // SPEC §3.3: 値を返さず、shouldNotImplement で評価を中断する。
  auto r = send0(b, b.wk.metaclassClass, "new");
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("shouldNotImplement", takeAbortReason(b));
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

TEST(Behavior, MetaclassNewAbortsWithShouldNotImplement) {
  Boot b;
  auto r = send0(b, b.heap.klass(b.wk.objectClass), "new");
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("shouldNotImplement", takeAbortReason(b));
}

TEST(Behavior, ArrayNewIsEmptyArray) {
  Boot b;
  auto o = send0(b, b.wk.arrayClass, "new");
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(o));
  EXPECT_EQ(0u, b.heap.size(o));
}

TEST(Behavior, BasicNewColonAllocatesIndexableSlots) {
  Boot b;
  auto o = send1(b, b.wk.arrayClass, "basicNew:", ao::Oop::fromSmallInteger(3));
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(o));
  EXPECT_EQ(3u, b.heap.size(o));
  auto bytes = send1(b, b.wk.byteArrayClass, "basicNew:", ao::Oop::fromSmallInteger(4));
  ASSERT_TRUE(bytes.isHeap());
  EXPECT_EQ(4u, b.heap.size(bytes));
  EXPECT_NE(0, b.heap.flags(bytes) & ao::kFlagBytes);
}

TEST(Behavior, InstSizeAndFormatBitsArrayVsObject) {
  Boot b;
  auto objSize = send0(b, b.wk.objectClass, "instSize");
  ASSERT_TRUE(objSize.isSmallInteger());
  EXPECT_EQ(0, objSize.smallIntegerValue());
  auto clsSize = send0(b, b.wk.classClass, "instSize");
  ASSERT_TRUE(clsSize.isSmallInteger());
  EXPECT_EQ(static_cast<std::int64_t>(ao::kClassSlotCount), clsSize.smallIntegerValue());

  EXPECT_TRUE(send0(b, b.wk.objectClass, "isVariable").isFalse());
  EXPECT_TRUE(send0(b, b.wk.objectClass, "isBytes").isFalse());
  EXPECT_TRUE(send0(b, b.wk.objectClass, "isPointers").isTrue());
  EXPECT_TRUE(send0(b, b.wk.arrayClass, "isVariable").isTrue());
  EXPECT_TRUE(send0(b, b.wk.arrayClass, "isBytes").isFalse());
  EXPECT_TRUE(send0(b, b.wk.arrayClass, "isPointers").isTrue());
  EXPECT_TRUE(send0(b, b.wk.byteArrayClass, "isVariable").isTrue());
  EXPECT_TRUE(send0(b, b.wk.byteArrayClass, "isBytes").isTrue());
  EXPECT_TRUE(send0(b, b.wk.byteArrayClass, "isPointers").isFalse());
}

TEST(Behavior, InheritsFromWalksSuperclassChain) {
  Boot b;
  EXPECT_TRUE(send1(b, b.wk.arrayClass, "inheritsFrom:", b.wk.collectionClass).isTrue());
  EXPECT_TRUE(send1(b, b.wk.arrayClass, "inheritsFrom:", b.wk.objectClass).isTrue());
  EXPECT_TRUE(send1(b, b.wk.arrayClass, "inheritsFrom:", b.wk.arrayClass).isFalse());
  EXPECT_TRUE(send1(b, b.wk.objectClass, "inheritsFrom:", b.wk.arrayClass).isFalse());
}

TEST(Behavior, ThisClassNameIncludesSelectorCompiledMethodAt) {
  Boot b;
  EXPECT_EQ(b.wk.objectClass, send0(b, b.wk.objectMetaclass, "thisClass"));
  auto name = send0(b, b.wk.objectClass, "name");
  ASSERT_TRUE(name.isHeap());
  EXPECT_EQ("Object", ao::Str::toUtf8(b.heap, name));

  auto classSel = ao::Symbol::intern(b.wk, "class");
  EXPECT_TRUE(send1(b, b.wk.objectClass, "includesSelector:", classSel).isTrue());
  EXPECT_TRUE(send1(b, b.wk.objectClass, "includesSelector:", ao::Symbol::intern(b.wk, "new")).isFalse());

  auto meth = send1(b, b.wk.objectClass, "compiledMethodAt:", classSel);
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(meth));
  EXPECT_EQ("ao_Object_class", ao::NativeMethod::nameBytes(b.heap, meth));
}

TEST(Behavior, EachClassAndGlobalsSeeExtraNamed) {
  Boot b;
  auto foo = makeSubclass(b, "Foo");
  ASSERT_TRUE(foo.isHeap());
  EXPECT_EQ(foo, b.wk.named("Foo"));
  EXPECT_EQ(foo, ao::Globals::at(b.wk, "Foo"));

  struct Seen {
    ao::Oop want;
    bool found = false;
  } seen{foo, false};
  b.wk.eachClass(
      [](void* p, ao::Oop cls) {
        auto* s = static_cast<Seen*>(p);
        if (cls == s->want) {
          s->found = true;
        }
      },
      &seen);
  EXPECT_TRUE(seen.found);
}

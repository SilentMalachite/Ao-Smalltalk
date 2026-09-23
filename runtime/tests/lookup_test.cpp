#include "ao/Bootstrap.hpp"
#include "ao/Heap.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <gtest/gtest.h>

static ao::Oop stubA(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(1);
}

static ao::Oop stubB(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(2);
}

static ao::Oop install(ao::WellKnown& wk, ao::Oop cls, const char* selector, ao::NativeFn fn,
                       std::uint32_t argc, const char* cName) {
  ao::Heap& heap = wk.heap();
  auto dict = heap.slotAt(cls, ao::kClassSlotMethodDict);
  if (!dict.isHeap()) {
    dict = ao::MethodDictionary::create(heap, wk, 8);
    heap.slotAtPut(cls, ao::kClassSlotMethodDict, dict);
  }
  auto sel = ao::Symbol::intern(wk, selector);
  auto idx = ao::NativeRegistry::add(fn);
  auto meth = ao::NativeMethod::create(heap, wk, sel, argc, cName, idx, cls);
  ao::MethodDictionary::atPut(heap, dict, sel, meth);
  return meth;
}

TEST(Lookup, InheritsFromSuperclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto objectFoo = install(wk, wk.objectClass, "foo", stubA, 0, "ao_Object_foo");
  auto found = ao::lookup(heap, wk.trueClass, ao::Symbol::intern(wk, "foo"));
  EXPECT_EQ(objectFoo, found);
}

TEST(Lookup, SubclassEntryShadowsSuperclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto objectFoo = install(wk, wk.objectClass, "foo", stubA, 0, "ao_Object_foo");
  auto trueFoo = install(wk, wk.trueClass, "foo", stubB, 0, "ao_True_foo");
  auto found = ao::lookup(heap, wk.trueClass, ao::Symbol::intern(wk, "foo"));
  EXPECT_EQ(trueFoo, found);
  EXPECT_NE(objectFoo, found);
}

TEST(Lookup, SuperStartsAtMethodClassSuperclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto objectFoo = install(wk, wk.objectClass, "foo", stubA, 0, "ao_Object_foo");
  auto trueFoo = install(wk, wk.trueClass, "foo", stubB, 0, "ao_True_foo");
  auto start = heap.slotAt(wk.trueClass, ao::kClassSlotSuperclass);
  EXPECT_EQ(wk.booleanClass, start);
  auto found = ao::lookup(heap, start, ao::Symbol::intern(wk, "foo"));
  EXPECT_EQ(objectFoo, found);
  EXPECT_NE(trueFoo, found);
}

TEST(Lookup, MissingSelectorIsNil) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_TRUE(ao::lookup(heap, wk.trueClass, ao::Symbol::intern(wk, "noSuch")).isNil());
}

TEST(Lookup, BootstrapInstallsObjectIdentityEquals) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto m = ao::lookup(heap, wk.objectClass, ao::Symbol::intern(wk, "=="));
  ASSERT_TRUE(m.isHeap());
  EXPECT_EQ(wk.nativeMethodClass, heap.klass(m));
}

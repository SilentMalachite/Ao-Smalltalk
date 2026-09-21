#include "ao/Bootstrap.hpp"
#include "ao/Gc.hpp"
#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"
#include <gtest/gtest.h>

TEST(Symbol, InternSameBytesIsIdentical) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto a = ao::Symbol::intern(wk, "foo");
  auto b = ao::Symbol::intern(wk, "foo");
  auto c = ao::Symbol::intern(wk, "bar");
  ASSERT_TRUE(a.isHeap());
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
  EXPECT_EQ(wk.symbolClass, heap.klass(a));
  EXPECT_EQ("foo", ao::Symbol::bytes(heap, a));
}

TEST(Symbol, InternSurvivesNurseryGc) {
  ao::Heap heap(1 << 16, 1 << 18);
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto a = ao::Symbol::intern(wk, "==");
  roots.add(&a);
  ao::Gc gc(heap, roots);
  gc.collectNursery();
  auto b = ao::Symbol::intern(wk, "==");
  EXPECT_EQ(a, b);
  EXPECT_TRUE(heap.inOld(a));
}

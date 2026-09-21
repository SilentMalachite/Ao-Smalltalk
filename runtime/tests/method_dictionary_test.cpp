#include "ao/Bootstrap.hpp"
#include "ao/Heap.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <cstdio>
#include <gtest/gtest.h>

TEST(MethodDictionary, AtPutFindsInternedKey) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto dict = ao::MethodDictionary::create(heap, wk, 2);
  auto k1 = ao::Symbol::intern(wk, "==");
  auto k2 = ao::Symbol::intern(wk, "==");
  auto meth = ao::Oop::fromSmallInteger(7);
  ao::MethodDictionary::atPut(heap, dict, k1, meth);
  EXPECT_EQ(meth, ao::MethodDictionary::at(heap, dict, k2));
  EXPECT_TRUE(ao::MethodDictionary::at(heap, dict, ao::Symbol::intern(wk, "class")).isNil());
  EXPECT_EQ(dict, dict);  // outer oop identity
}

TEST(MethodDictionary, GrowKeepsOuterOopAndEntries) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto dict = ao::MethodDictionary::create(heap, wk, 2);
  const ao::Oop outer = dict;
  for (int i = 0; i < 10; ++i) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "s%d", i);
    ao::MethodDictionary::atPut(heap, dict, ao::Symbol::intern(wk, buf),
                                ao::Oop::fromSmallInteger(i));
  }
  EXPECT_EQ(outer, dict);
  EXPECT_EQ(ao::Oop::fromSmallInteger(9),
            ao::MethodDictionary::at(heap, dict, ao::Symbol::intern(wk, "s9")));
}

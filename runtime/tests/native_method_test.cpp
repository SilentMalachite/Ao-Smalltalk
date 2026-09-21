#include "ao/Bootstrap.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <gtest/gtest.h>

static ao::Oop stubEq(ao::CallContext&, ao::Oop receiver, const ao::Oop* args, std::uint32_t argc) {
  if (argc != 1) return ao::Oop{};
  return receiver == args[0] ? ao::Oop::true_() : ao::Oop::false_();
}

TEST(NativeMethod, NameAndApply) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto sel = ao::Symbol::intern(wk, "==");
  auto idx = ao::NativeRegistry::add(stubEq);
  auto meth = ao::NativeMethod::create(heap, wk, sel, 1, "ao_Object_identityEquals", idx,
                                       wk.objectClass);
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(wk.nativeMethodClass, heap.klass(meth));
  EXPECT_EQ("ao_Object_identityEquals", ao::NativeMethod::nameBytes(heap, meth));
  ao::CallContext ctx{heap, roots, wk, nullptr};
  ao::Oop arg = ao::Oop::fromSmallInteger(1);
  auto recv = ao::Oop::fromSmallInteger(1);
  EXPECT_TRUE(ao::NativeMethod::apply(ctx, meth, recv, &arg, 1).isTrue());
}

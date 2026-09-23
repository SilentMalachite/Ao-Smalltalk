#include "test_support.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <gtest/gtest.h>

static ao::Oop stubEq(ao::CallContext&, const ao::Oop& receiver, const ao::Oop* args,
                      std::uint32_t argc) {
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

TEST(NativeMethod, AddInternsByFunctionPointer) {
  auto idx1 = ao::NativeRegistry::add(stubEq);
  auto idx2 = ao::NativeRegistry::add(stubEq);
  EXPECT_EQ(idx1, idx2);
}

namespace {

// Collects the nursery, then answers the receiver it was given.
ao::Oop gcThenReceiver(ao::CallContext& ctx, const ao::Oop& receiver, const ao::Oop*,
                       std::uint32_t) {
  ao::Gc(ctx.heap, ctx.roots).collectNursery();
  return receiver;
}

// Collects the nursery, then answers its argument.
ao::Oop gcThenArg(ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t argc) {
  if (argc != 1) return ao::Oop{};
  ao::Gc(ctx.heap, ctx.roots).collectNursery();
  return args[0];
}

// A fresh array holding 42, referenced only from a C++ local.
ao::Oop unrootedArray42(ao::CallContext& ctx) {
  const ao::Oop a = ctx.heap.allocate(ctx.wk.arrayClass, 1, 0);
  ctx.heap.slotAtPut(a, 0, ao::Oop::fromSmallInteger(42));
  return a;
}

// Calls gcThenArg directly with a local, the way one native calls another.
ao::Oop callsWithLocal(ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  const ao::Oop local = unrootedArray42(ctx);
  return ao::NativeMethod::invoke(ctx, gcThenArg, ao::Oop::nil(), &local, 1);
}

ao::Oop probeMethod(Boot& b, ao::NativeFn fn, std::uint32_t argc) {
  const auto sel = ao::Symbol::intern(b.wk, argc == 0 ? "gcProbe" : "gcProbe:");
  return ao::NativeMethod::create(b.heap, b.wk, sel, argc, "ao_Test_gcProbe",
                                  ao::NativeRegistry::add(fn), b.wk.objectClass);
}

void expectArray42(Boot& b, ao::Oop obj) {
  ASSERT_TRUE(obj.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(obj));
  EXPECT_EQ(ao::Oop::fromSmallInteger(42), b.heap.slotAt(obj, 0));
}

}  // namespace

TEST(NativeMethod, ReceiverRefTracksMove) {
  Boot b;
  ao::Root obj(b.roots, b.heap.allocate(b.wk.arrayClass, 1, 0));
  ASSERT_TRUE(b.heap.inNursery(obj.slot));
  const ao::Oop before = obj.slot;
  const ao::Oop meth = probeMethod(b, gcThenReceiver, 0);
  const ao::Oop r = ao::NativeMethod::apply(b.ctx, meth, obj.slot, nullptr, 0);
  ASSERT_NE(before, obj.slot);
  EXPECT_EQ(obj.slot, r);
}

TEST(NativeMethod, UnrootedCallerArgSurvivesGc) {
  Boot b;
  const ao::Oop meth = probeMethod(b, gcThenArg, 1);
  const ao::Oop local = unrootedArray42(b.ctx);
  const ao::Oop r = ao::NativeMethod::apply(b.ctx, meth, ao::Oop::nil(), &local, 1);
  EXPECT_NE(local, r);
  expectArray42(b, r);
}

TEST(NativeMethod, InvokeRootsDirectCall) {
  Boot b;
  const ao::Oop meth = probeMethod(b, callsWithLocal, 0);
  expectArray42(b, ao::NativeMethod::apply(b.ctx, meth, ao::Oop::nil(), nullptr, 0));
}

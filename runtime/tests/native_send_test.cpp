#include "ao/Bootstrap.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <gtest/gtest.h>

static ao::Oop stubA(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(1);
}

static ao::Oop stubB(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(2);
}

static ao::Oop trueDnuSentinel(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(99);
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

TEST(NativeSend, IdentityEqualsAndClass) {
  ao::Heap heap; ao::Roots roots; ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache; cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::InlineCache ic;
  auto eq = ao::Symbol::intern(wk, "==");
  auto cls = ao::Symbol::intern(wk, "class");
  ao::Oop one = ao::Oop::fromSmallInteger(1);
  ao::Oop two = ao::Oop::fromSmallInteger(2);
  EXPECT_TRUE(ao::send(ctx, one, eq, &one, 1, &ic).isTrue());
  EXPECT_TRUE(ao::send(ctx, one, eq, &two, 1, &ic).isFalse());
  EXPECT_EQ(wk.smallIntegerClass, ao::send(ctx, one, cls, nullptr, 0, &ic));
  EXPECT_EQ(wk.smallIntegerClass, ic.cachedClass);
}

TEST(NativeSend, StubSmallIntegerAdd) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::InlineCache ic;
  auto plus = ao::Symbol::intern(wk, "+");
  ao::Oop one = ao::Oop::fromSmallInteger(1);
  ao::Oop two = ao::Oop::fromSmallInteger(2);
  auto r = ao::send(ctx, one, plus, &two, 1, &ic);
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(3, r.smallIntegerValue());
}

TEST(NativeSend, DoesNotUnderstandReturnsMessage) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::InlineCache ic;
  // Building the Message may collect, so the selector is held in a root.
  ao::Root selRoot(roots, ao::Symbol::intern(wk, "noSuchSelector"));
  const ao::Oop& sel = selRoot.slot;
  auto r = ao::send(ctx, ao::Oop::fromSmallInteger(1), sel, nullptr, 0, &ic);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(wk.messageClass, heap.klass(r));
  EXPECT_EQ(sel, heap.slotAt(r, 0));
  auto argsArr = heap.slotAt(r, 1);
  ASSERT_TRUE(argsArr.isHeap());
  EXPECT_EQ(wk.arrayClass, heap.klass(argsArr));
  EXPECT_EQ(0u, heap.size(argsArr));

  ao::Oop arg = ao::Oop::fromSmallInteger(7);
  auto r2 = ao::send(ctx, ao::Oop::fromSmallInteger(1), sel, &arg, 1, &ic);
  ASSERT_TRUE(r2.isHeap());
  auto args2 = heap.slotAt(r2, 1);
  ASSERT_TRUE(args2.isHeap());
  EXPECT_EQ(wk.arrayClass, heap.klass(args2));
  ASSERT_EQ(1u, heap.size(args2));
  EXPECT_EQ(arg, heap.slotAt(args2, 0));
}

TEST(NativeSend, DoesNotUnderstandAppliesSubclassNative) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::InlineCache ic;
  install(wk, wk.trueClass, "doesNotUnderstand:", trueDnuSentinel, 1,
          "ao_True_doesNotUnderstand_");
  auto sel = ao::Symbol::intern(wk, "noSuchSelector");
  auto r = ao::send(ctx, ao::Oop::true_(), sel, nullptr, 0, &ic);
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(99, r.smallIntegerValue());
}

TEST(NativeSend, SuperUsesDefiningClassSuperclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  install(wk, wk.objectClass, "foo", stubA, 0, "ao_Object_foo");
  install(wk, wk.trueClass, "foo", stubB, 0, "ao_True_foo");
  auto sel = ao::Symbol::intern(wk, "foo");
  auto r = ao::sendSuper(ctx, ao::Oop::true_(), sel, nullptr, 0, wk.trueClass);
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(1, r.smallIntegerValue());
}

namespace {

// A one-slot array holding n, referenced only from a C++ local.
ao::Oop unrootedBox(ao::Heap& heap, ao::WellKnown& wk, std::int64_t n) {
  const ao::Oop a = heap.allocate(wk.arrayClass, 1, 0);
  heap.slotAtPut(a, 0, ao::Oop::fromSmallInteger(n));
  return a;
}

void expectBox(ao::Heap& heap, ao::WellKnown& wk, ao::Oop box, std::int64_t n) {
  ASSERT_TRUE(box.isHeap());
  EXPECT_EQ(wk.arrayClass, heap.klass(box));
  EXPECT_EQ(ao::Oop::fromSmallInteger(n), heap.slotAt(box, 0));
}

// Allocates through allocateRetry (a collection under GC stress), then answers
// { receiver. argument }.
ao::Oop pairAfterAlloc(ao::CallContext& ctx, const ao::Oop& receiver, const ao::Oop* args,
                       std::uint32_t argc) {
  if (argc != 1) return ao::Oop{};
  const ao::Oop pair = ao::allocateRetry(ctx, ctx.wk.arrayClass, 2, 0);
  if (!pair.isHeap()) return ao::Oop{};
  ctx.heap.slotAtPut(pair, 0, receiver);
  ctx.heap.slotAtPut(pair, 1, args[0]);
  return pair;
}

}  // namespace

TEST(NativeSend, DnuWithFullNurseryReturnsMessage) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::Root sel(roots, ao::Symbol::intern(wk, "zork:"));
  const ao::Oop arg = unrootedBox(heap, wk, 7);
  while (heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
  const ao::Oop r = ao::send(ctx, ao::Oop::fromSmallInteger(3), sel.slot, &arg, 1, nullptr);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(wk.messageClass, heap.klass(r));
  EXPECT_EQ(sel.slot, heap.slotAt(r, 0));
  const ao::Oop msgArgs = heap.slotAt(r, 1);
  ASSERT_TRUE(msgArgs.isHeap());
  ASSERT_EQ(1u, heap.size(msgArgs));
  expectBox(heap, wk, heap.slotAt(msgArgs, 0), 7);
}

TEST(NativeSend, SuperSendUnderStress) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  install(wk, wk.objectClass, "pairWith:", pairAfterAlloc, 1, "ao_Object_pairWith_");
  ao::Root sel(roots, ao::Symbol::intern(wk, "pairWith:"));
  heap.setGcStress(1);
  const ao::Oop rcvr = unrootedBox(heap, wk, 1);
  const ao::Oop arg = unrootedBox(heap, wk, 2);
  const ao::Oop r = ao::sendSuper(ctx, rcvr, sel.slot, &arg, 1, wk.arrayClass);
  ASSERT_TRUE(r.isHeap());
  ASSERT_EQ(wk.arrayClass, heap.klass(r));
  expectBox(heap, wk, heap.slotAt(r, 0), 1);
  expectBox(heap, wk, heap.slotAt(r, 1), 2);
}

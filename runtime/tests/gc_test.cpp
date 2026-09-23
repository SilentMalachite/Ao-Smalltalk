#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <gtest/gtest.h>

TEST(GcNursery, UnrootedObjectIsReclaimed) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto keep = heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(keep.isHeap());
  const auto keepBytes = heap.objectBytes(heap.header(keep));
  roots.add(&keep);
  (void)heap.allocate(ao::Oop::nil(), 1, 0);  // unrooted garbage
  gc.collectNursery();
  ASSERT_TRUE(keep.isHeap());
  EXPECT_TRUE(heap.inOld(keep));
  EXPECT_EQ(keepBytes, heap.oldUsed());
}

TEST(GcNursery, RootedObjectSurvivesAndSlotsUpdate) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto parent = heap.allocate(ao::Oop::nil(), 1, 0);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(parent, 0, child);
  roots.add(&parent);
  gc.collectNursery();
  ASSERT_TRUE(parent.isHeap());
  auto movedChild = heap.slotAt(parent, 0);
  ASSERT_TRUE(movedChild.isHeap());
  EXPECT_TRUE(heap.inOld(parent));
  EXPECT_TRUE(heap.inOld(movedChild));
}

TEST(GcNursery, ImmediateClassIsNotFollowed) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto obj = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.add(&obj);
  gc.collectNursery();
  EXPECT_TRUE(heap.klass(obj).isNil());
}

TEST(GcNursery, OldToNurseryChildSurvives) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto obj = heap.allocate(ao::Oop::nil(), 1, 0);
  roots.add(&obj);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(obj));
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(child.isHeap());
  void* childBefore = child.heapPointer();
  heap.slotAtPut(obj, 0, child);
  gc.collectNursery();
  auto moved = heap.slotAt(obj, 0);
  ASSERT_TRUE(moved.isHeap());
  EXPECT_TRUE(heap.inOld(moved));
  EXPECT_NE(moved.heapPointer(), childBefore);
}

TEST(GcNursery, DeadOldSlotIsNotANurseryRoot) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto dead = heap.allocate(ao::Oop::nil(), 1, 0);
  roots.add(&dead);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(dead));
  const auto deadBytes = heap.oldUsed();
  roots.remove(&dead);

  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(child.isHeap());
  heap.slotAtPut(dead, 0, child);

  auto keep = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(keep.isHeap());
  const auto keepBytes = heap.objectBytes(heap.header(keep));
  roots.add(&keep);
  gc.collectNursery();
  ASSERT_TRUE(keep.isHeap());
  EXPECT_TRUE(heap.inOld(keep));
  EXPECT_EQ(deadBytes + keepBytes, heap.oldUsed());
}

// old が上限でも、スキャベンジは失敗しない。入り切らない生存物は to-space に残り、フリップ後の
// nursery で生き続ける。old に空きができれば次のスキャベンジで昇格する。
TEST(GcNursery, OldAtMaxKeepsSurvivorInToSpace) {
  ao::Heap heap(512, 32, 32);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto first = heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(first.isHeap());
  heap.slotAtPut(first, 0, ao::Oop::fromSmallInteger(1));
  roots.add(&first);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(first));
  ASSERT_EQ(24u, heap.oldUsed());

  auto second = heap.allocate(ao::Oop::nil(), 2, 0);  // 32 バイト: old の残り 8 バイトに入らない
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(second.isHeap());
  ASSERT_TRUE(child.isHeap());
  heap.slotAtPut(second, 0, ao::Oop::fromSmallInteger(2));
  heap.slotAtPut(second, 1, child);
  void* const secondBefore = second.heapPointer();
  roots.add(&second);
  gc.collectNursery();

  ASSERT_TRUE(second.isHeap());
  EXPECT_TRUE(heap.inNursery(second));
  EXPECT_FALSE(heap.inOld(second));
  EXPECT_NE(secondBefore, second.heapPointer());
  EXPECT_TRUE(heap.klass(second).isNil());
  EXPECT_EQ(ao::Oop::fromSmallInteger(2), heap.slotAt(second, 0));
  auto keptChild = heap.slotAt(second, 1);
  ASSERT_TRUE(keptChild.isHeap());
  EXPECT_TRUE(heap.klass(keptChild).isNil());
  EXPECT_TRUE(heap.inOld(first));
  EXPECT_EQ(ao::Oop::fromSmallInteger(1), heap.slotAt(first, 0));
  EXPECT_EQ(24u, heap.oldUsed());

  // first を捨てて old を空けると、次のスキャベンジで second が昇格する。
  roots.remove(&first);
  gc.collectOld();
  ASSERT_EQ(0u, heap.oldUsed());
  gc.collectNursery();
  ASSERT_TRUE(second.isHeap());
  EXPECT_TRUE(heap.inOld(second));
  EXPECT_EQ(ao::Oop::fromSmallInteger(2), heap.slotAt(second, 0));
  auto lastChild = heap.slotAt(second, 1);
  ASSERT_TRUE(lastChild.isHeap());
  EXPECT_TRUE(heap.inNursery(lastChild));  // 残り 0 バイトの old には入らない
  EXPECT_TRUE(heap.klass(lastChild).isNil());
  roots.remove(&second);
}

// docs/claude-review/01 プローブ p1。old が [ゴミ G][A (2 slot)][B (slot0 = 42)] で埋まり、
// A.slot1 だけが nursery の N を指す。N を置くためにスキャベンジの途中で old を圧縮すると、
// A の古い番地（圧縮後は B の slot0）へ書き込んでしまう。上限あり・伸長ありの両方で試す。
TEST(GcNursery, CompactionDuringScavengeDoesNotCorruptSlots) {
  for (const std::size_t oldMax : {std::size_t{80}, std::size_t{1} << 20}) {
    SCOPED_TRACE(oldMax);
    ao::Heap heap(512, 80, oldMax);  // G 24 + A 32 + B 24 = 80 で初期容量ちょうど
    ao::Roots roots;
    ao::Gc gc(heap, roots);
    auto g = heap.allocate(ao::Oop::nil(), 1, 0);
    auto a = heap.allocate(ao::Oop::nil(), 2, 0);
    auto b = heap.allocate(ao::Oop::nil(), 1, 0);
    roots.add(&g);
    roots.add(&a);
    roots.add(&b);
    gc.collectNursery();
    ASSERT_EQ(heap.oldBase(), static_cast<const std::byte*>(g.heapPointer()));
    ASSERT_EQ(static_cast<std::byte*>(g.heapPointer()) + 24,
              static_cast<std::byte*>(a.heapPointer()));
    ASSERT_EQ(static_cast<std::byte*>(a.heapPointer()) + 32,
              static_cast<std::byte*>(b.heapPointer()));
    ASSERT_EQ(80u, heap.oldUsed());
    roots.remove(&g);
    heap.slotAtPut(b, 0, ao::Oop::fromSmallInteger(42));

    auto n = heap.allocate(ao::Oop::nil(), 1, 0);
    ASSERT_TRUE(n.isHeap());
    heap.slotAtPut(n, 0, ao::Oop::fromSmallInteger(7));
    heap.slotAtPut(a, 1, n);
    gc.collectNursery();

    EXPECT_EQ(ao::Oop::fromSmallInteger(42), heap.slotAt(b, 0));
    EXPECT_TRUE(heap.slotAt(a, 0).isNil());
    auto moved = heap.slotAt(a, 1);
    ASSERT_TRUE(moved.isHeap());
    EXPECT_TRUE(heap.klass(moved).isNil());
    EXPECT_EQ(ao::Oop::fromSmallInteger(7), heap.slotAt(moved, 0));
    roots.remove(&b);
    roots.remove(&a);
  }
}

// docs/claude-review/01 プローブ p6。root→N1、root→N2（old に入らない）、root→N3、N3.slot0→N1。
// 途中で昇格できなくなっても、転送済みの N1 が別経路から見えて同一性が割れてはならない。
TEST(GcNursery, NoIdentitySplitWhenOldAtMax) {
  ao::Heap heap(4096, 64, 64);  // cls 16 + N1 24 + N3 24 = 64。N2 は入らない
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto cls = heap.allocateTenured(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(cls.isHeap());
  auto n1 = heap.allocate(cls, 1, 0);
  auto n2 = heap.allocate(cls, 200, 0);
  auto n3 = heap.allocate(cls, 1, 0);
  ASSERT_TRUE(n1.isHeap());
  ASSERT_TRUE(n2.isHeap());
  ASSERT_TRUE(n3.isHeap());
  ASSERT_TRUE(heap.inNursery(n2));
  heap.slotAtPut(n1, 0, ao::Oop::fromSmallInteger(1));
  heap.slotAtPut(n3, 0, n1);
  roots.add(&cls);
  roots.add(&n1);
  roots.add(&n2);
  roots.add(&n3);

  for (int round = 0; round < 2; ++round) {
    SCOPED_TRACE(round);
    gc.collectNursery();
    EXPECT_EQ(cls, heap.klass(n1));
    EXPECT_EQ(cls, heap.klass(n2));
    EXPECT_EQ(cls, heap.klass(n3));
    EXPECT_TRUE(heap.inNursery(n2));
    EXPECT_EQ(200u, heap.size(n2));
    auto viaN3 = heap.slotAt(n3, 0);
    EXPECT_EQ(n1, viaN3);
    EXPECT_EQ(cls, heap.klass(viaN3));
    heap.slotAtPut(n1, 0, ao::Oop::fromSmallInteger(99 + round));
    EXPECT_EQ(ao::Oop::fromSmallInteger(99 + round), heap.slotAt(heap.slotAt(n3, 0), 0));
  }
  roots.remove(&n3);
  roots.remove(&n2);
  roots.remove(&n1);
  roots.remove(&cls);
}

TEST(GcNursery, SharedChildCopiedOnce) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto parent = heap.allocate(ao::Oop::nil(), 2, 0);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(parent, 0, child);
  heap.slotAtPut(parent, 1, child);
  roots.add(&parent);
  gc.collectNursery();
  ASSERT_TRUE(parent.isHeap());
  auto a = heap.slotAt(parent, 0);
  auto b = heap.slotAt(parent, 1);
  ASSERT_TRUE(a.isHeap());
  EXPECT_EQ(a, b);
  EXPECT_TRUE(heap.inOld(a));
}

TEST(GcNursery, ByteObjectPayloadIsNotScannedAsOops) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto obj = heap.allocate(ao::Oop::nil(), 8, ao::kFlagBytes);
  ASSERT_TRUE(obj.isHeap());
  auto* b = heap.bytes(obj);
  for (int i = 0; i < 8; ++i) {
    b[i] = std::byte{0xFF};
  }
  roots.add(&obj);
  gc.collectNursery();
  ASSERT_TRUE(obj.isHeap());
  EXPECT_TRUE(heap.inOld(obj));
  EXPECT_EQ(8u, heap.size(obj));
  EXPECT_EQ(std::byte{0xFF}, heap.bytes(obj)[0]);
  EXPECT_EQ(std::byte{0xFF}, heap.bytes(obj)[7]);
}

TEST(GcOld, NurserySurvivorIsPromoted) {
  ao::Heap heap(512, 8192);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto obj = heap.allocate(ao::Oop::nil(), 1, 0);
  heap.slotAtPut(obj, 0, ao::Oop::fromSmallInteger(9));
  roots.add(&obj);
  gc.collectNursery();
  ASSERT_TRUE(obj.isHeap());
  EXPECT_TRUE(heap.inOld(obj));
  EXPECT_FALSE(heap.inNursery(obj));
  EXPECT_EQ(ao::Oop::fromSmallInteger(9), heap.slotAt(obj, 0));
}

TEST(GcOld, InternalPointersUpdatedAfterCompact) {
  ao::Heap heap(256, 1024);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  // Dead old object must sit at a lower address than live a/b so compact slides them.
  auto garbage = heap.allocate(ao::Oop::nil(), 2, 0);
  roots.add(&garbage);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(garbage));
  roots.remove(&garbage);

  auto a = heap.allocate(ao::Oop::nil(), 1, 0);
  auto b = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(a, 0, b);
  roots.add(&a);
  gc.collectNursery();  // promote a and b above garbage
  ASSERT_TRUE(heap.inOld(a));
  auto childBefore = heap.slotAt(a, 0);
  ASSERT_TRUE(childBefore.isHeap());
  ASSERT_TRUE(heap.inOld(childBefore));
  void* aBefore = a.heapPointer();
  void* bBefore = childBefore.heapPointer();
  ASSERT_LT(static_cast<std::byte*>(garbage.heapPointer()),
            static_cast<std::byte*>(aBefore));

  gc.collectOld();  // reclaim garbage, slide a/b, rewrite a[0]

  ASSERT_TRUE(a.isHeap());
  EXPECT_TRUE(heap.inOld(a));
  EXPECT_NE(a.heapPointer(), aBefore);
  auto child = heap.slotAt(a, 0);
  ASSERT_TRUE(child.isHeap());
  EXPECT_TRUE(heap.inOld(child));
  EXPECT_NE(child.heapPointer(), bBefore);
}

TEST(GcRoots, HandleTableKeepsObject) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto obj = heap.allocate(ao::Oop::nil(), 0, 0);
  auto h = roots.pushHandle(obj);
  gc.collectNursery();
  auto kept = roots.handleAt(h);
  ASSERT_TRUE(kept.isHeap());
  EXPECT_TRUE(heap.inOld(kept));
}

TEST(GcRoots, RemoveDropsOneMatchingRegistration) {
  ao::Roots roots;
  ao::Oop slot = ao::Oop::fromSmallInteger(1);
  roots.add(&slot);
  roots.add(&slot);
  roots.remove(&slot);
  int seen = 0;
  ao::Oop* remaining = nullptr;
  struct Visit {
    int* seen;
    ao::Oop** remaining;
  } rec{&seen, &remaining};
  roots.visitAll(
      [](void* ctx, ao::Oop* p) {
        auto* v = static_cast<Visit*>(ctx);
        ++*v->seen;
        *v->remaining = p;
      },
      &rec);
  EXPECT_EQ(1, seen);
  EXPECT_EQ(&slot, remaining);
}

TEST(GcRoots, StackWalkerKeepsObject) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  ao::Oop stackObj = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.setStackWalker(
      [](void* ctx, ao::Roots::VisitFn visit, void* visitCtx) {
        visit(visitCtx, static_cast<ao::Oop*>(ctx));
      },
      &stackObj);
  gc.collectNursery();
  ASSERT_TRUE(stackObj.isHeap());
  EXPECT_TRUE(heap.inOld(stackObj));
}

namespace {

int countVisitedRoots(ao::Roots& roots) {
  int seen = 0;
  roots.visitAll([](void* ctx, ao::Oop*) { ++*static_cast<int*>(ctx); }, &seen);
  return seen;
}

}  // namespace

TEST(GcRoots, RangeRootIsVisitedAndPopped) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  ao::Oop range[2] = {heap.allocate(ao::Oop::nil(), 0, 0), heap.allocate(ao::Oop::nil(), 1, 0)};
  ASSERT_TRUE(range[0].isHeap());
  ASSERT_TRUE(range[1].isHeap());
  ASSERT_FALSE(heap.inOld(range[0]));
  roots.pushRange(range, 2);
  EXPECT_EQ(2, countVisitedRoots(roots));
  gc.collectNursery();
  ASSERT_TRUE(range[0].isHeap());
  ASSERT_TRUE(range[1].isHeap());
  EXPECT_TRUE(heap.inOld(range[0]));
  EXPECT_TRUE(heap.inOld(range[1]));
  EXPECT_EQ(1u, heap.size(range[1]));
  roots.popRange(range, 2);
  EXPECT_EQ(0, countVisitedRoots(roots));
}

TEST(GcRoots, RootedArrayBeyondInlineSlotsSurvivesGc) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  constexpr std::uint32_t kCount = ao::RootedArray::kInlineSlots + 1;
  {
    ao::RootedArray arr(roots, kCount);
    ASSERT_EQ(kCount, arr.size());
    ASSERT_EQ(&arr[0], arr.ptr());
    for (std::uint32_t i = 0; i < kCount; ++i) {
      EXPECT_TRUE(arr[i].isNil());
      arr[i] = heap.allocate(ao::Oop::nil(), 0, 0);
      ASSERT_TRUE(arr[i].isHeap());
    }
    {
      ao::RootedArray inner(roots, 2);
      EXPECT_EQ(static_cast<int>(kCount) + 2, countVisitedRoots(roots));
    }
    gc.collectNursery();
    for (std::uint32_t i = 0; i < kCount; ++i) {
      ASSERT_TRUE(arr[i].isHeap());
      EXPECT_TRUE(heap.inOld(arr[i]));
    }
  }
  EXPECT_EQ(0, countVisitedRoots(roots));
}

TEST(GcWeak, UnrootedReferentBecomesNil) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(weak.isHeap());
  ASSERT_TRUE(child.isHeap());
  heap.slotAtPut(weak, 0, child);
  roots.add(&weak);
  gc.collectNursery();
  ASSERT_TRUE(weak.isHeap());
  EXPECT_TRUE(heap.inOld(weak));
  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
}

TEST(GcWeak, RootedReferentSurvivesAndSlotUpdates) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(weak, 0, child);
  roots.add(&weak);
  roots.add(&child);
  void* childBefore = child.heapPointer();
  gc.collectNursery();
  ASSERT_TRUE(child.isHeap());
  EXPECT_TRUE(heap.inOld(child));
  EXPECT_NE(child.heapPointer(), childBefore);
  auto slot = heap.slotAt(weak, 0);
  EXPECT_EQ(slot, child);
  EXPECT_TRUE(heap.inOld(slot));
}

TEST(GcWeak, StrongPathKeepsReferentForWeakSlot) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  auto strong = heap.allocate(ao::Oop::nil(), 1, 0);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(weak, 0, child);
  heap.slotAtPut(strong, 0, child);
  roots.add(&weak);
  roots.add(&strong);
  gc.collectNursery();
  auto viaWeak = heap.slotAt(weak, 0);
  auto viaStrong = heap.slotAt(strong, 0);
  ASSERT_TRUE(viaStrong.isHeap());
  EXPECT_EQ(viaWeak, viaStrong);
  EXPECT_TRUE(heap.inOld(viaWeak));
}

TEST(GcWeak, OldToNurseryWeakChildIsCleared) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  roots.add(&weak);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(weak));
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(child.isHeap());
  heap.slotAtPut(weak, 0, child);
  gc.collectNursery();
  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
}

TEST(GcWeak, OldMarkDoesNotKeepWeakReferent) {
  ao::Heap heap(512, 8192);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(weak, 0, child);
  roots.add(&weak);
  roots.add(&child);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(weak));
  ASSERT_TRUE(heap.inOld(child));
  roots.remove(&child);
  gc.collectOld();
  ASSERT_TRUE(weak.isHeap());
  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
}

TEST(GcWeak, KlassSlotStaysStrong) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto cls = heap.allocate(ao::Oop::nil(), 0, 0);
  auto weak = heap.allocate(cls, 1, ao::kFlagWeak);
  roots.add(&weak);
  gc.collectNursery();
  ASSERT_TRUE(weak.isHeap());
  auto movedCls = heap.klass(weak);
  ASSERT_TRUE(movedCls.isHeap());
  EXPECT_TRUE(heap.inOld(movedCls));
}

TEST(GcWeak, NurseryWeakSlotNilsUnmarkedOldReferent) {
  ao::Heap heap(512, 8192);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.add(&child);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(child));
  roots.remove(&child);
  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  ASSERT_TRUE(weak.isHeap());
  ASSERT_TRUE(heap.inNursery(weak));
  heap.slotAtPut(weak, 0, child);
  roots.add(&weak);
  gc.collectOld();
  ASSERT_TRUE(weak.isHeap());
  EXPECT_TRUE(heap.inNursery(weak));
  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
}

TEST(GcWeak, LiveOldReferentRewrittenAcrossCompact) {
  ao::Heap heap(512, 8192);
  ao::Roots roots;
  ao::Gc gc(heap, roots);

  auto garbage = heap.allocate(ao::Oop::nil(), 2, 0);
  roots.add(&garbage);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(garbage));
  roots.remove(&garbage);

  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.add(&child);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(child));

  auto weak = heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  heap.slotAtPut(weak, 0, child);
  roots.add(&weak);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(weak));
  void* childBefore = child.heapPointer();
  ASSERT_LT(static_cast<std::byte*>(garbage.heapPointer()),
            static_cast<std::byte*>(childBefore));

  gc.collectOld();

  ASSERT_TRUE(child.isHeap());
  EXPECT_NE(child.heapPointer(), childBefore);
  EXPECT_EQ(heap.slotAt(weak, 0), child);
  EXPECT_TRUE(heap.inOld(child));
}

// 弱スロット → old の O → nursery の N で、O が弱参照でしか届かないとき、スキャベンジは O を
// たどらず N をコピーしない。O の slot は解放済みの番地を指し、弱スロットから O へ届いてしまう。
// スキャベンジは届く old をすべてたどるので、たどられなかった old を指す弱スロットはその時点で nil。
TEST(GcWeak, WeakOnlyOldReferentClearedAtScavenge) {
  ao::Heap heap(512, 8192);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocateTenured(ao::Oop::nil(), 2, ao::kFlagWeak);
  auto weakOnly = heap.allocateTenured(ao::Oop::nil(), 1, 0);
  auto live = heap.allocateTenured(ao::Oop::nil(), 0, 0);
  auto young = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(heap.inOld(weak));
  ASSERT_TRUE(heap.inOld(weakOnly));
  ASSERT_TRUE(heap.inOld(live));
  ASSERT_TRUE(heap.inNursery(young));
  heap.slotAtPut(weakOnly, 0, young);
  heap.slotAtPut(weak, 0, weakOnly);
  heap.slotAtPut(weak, 1, live);
  roots.add(&weak);
  roots.add(&live);

  gc.collectNursery();

  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
  EXPECT_EQ(live, heap.slotAt(weak, 1));
  roots.remove(&live);
  roots.remove(&weak);
}

TEST(GcOld, ImmovableKeepsAddressAcrossCompact) {
  ao::Heap heap(256, 2048);
  ao::Roots roots;
  ao::Gc gc(heap, roots);

  // Dead object must sit at a lower address than pin so today's compact would slide pin.
  auto garbage = heap.allocate(ao::Oop::nil(), 2, 0);
  roots.add(&garbage);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(garbage));
  roots.remove(&garbage);

  auto pin = heap.allocate(ao::Oop::nil(), 0, ao::kFlagImmovable);
  roots.add(&pin);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(pin));
  void* pinAddr = pin.heapPointer();
  ASSERT_LT(static_cast<std::byte*>(garbage.heapPointer()),
            static_cast<std::byte*>(pinAddr));

  gc.collectOld();

  ASSERT_TRUE(pin.isHeap());
  EXPECT_EQ(pin.heapPointer(), pinAddr);
  EXPECT_TRUE(heap.inOld(pin));
}

TEST(GcOld, MovableAfterPinSlidesAndSlotsUpdate) {
  ao::Heap heap(256, 2048);
  ao::Roots roots;
  ao::Gc gc(heap, roots);

  auto pin = heap.allocate(ao::Oop::nil(), 0, ao::kFlagImmovable);
  roots.add(&pin);
  gc.collectNursery();
  void* pinAddr = pin.heapPointer();

  auto garbage = heap.allocate(ao::Oop::nil(), 2, 0);
  roots.add(&garbage);
  gc.collectNursery();
  roots.remove(&garbage);

  auto parent = heap.allocate(ao::Oop::nil(), 1, 0);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(parent, 0, child);
  roots.add(&parent);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(parent));
  void* parentBefore = parent.heapPointer();
  ASSERT_LT(static_cast<std::byte*>(pinAddr),
            static_cast<std::byte*>(garbage.heapPointer()));
  ASSERT_LT(static_cast<std::byte*>(garbage.heapPointer()),
            static_cast<std::byte*>(parentBefore));

  gc.collectOld();

  EXPECT_EQ(pin.heapPointer(), pinAddr);
  ASSERT_TRUE(parent.isHeap());
  EXPECT_NE(parent.heapPointer(), parentBefore);
  auto movedChild = heap.slotAt(parent, 0);
  ASSERT_TRUE(movedChild.isHeap());
  EXPECT_TRUE(heap.inOld(movedChild));
}

namespace {

bool oldWalkVisits(ao::Heap& heap, void* oldStart, void* needle) {
  auto* scan = static_cast<std::byte*>(oldStart);
  auto* const end = scan + heap.oldUsed();
  int steps = 0;
  while (scan < end && steps++ < 1024) {
    auto* h = reinterpret_cast<ao::ObjectHeader*>(scan);
    const std::size_t n = heap.objectBytes(h);
    if (n == 0 || scan + n > end) {
      return false;
    }
    if (scan == needle) {
      return true;
    }
    scan += n;
  }
  return false;
}

}  // namespace

TEST(GcOld, PartialFillBeforePinLeavesWalkableChain) {
  ao::Heap heap(256, 2048);
  ao::Roots roots;
  ao::Gc gc(heap, roots);

  auto dead = heap.allocate(ao::Oop::nil(), 2, 0);
  roots.add(&dead);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(dead));
  void* oldStart = dead.heapPointer();
  roots.remove(&dead);

  auto mv = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.add(&mv);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(mv));

  auto pin = heap.allocate(ao::Oop::nil(), 0, ao::kFlagImmovable);
  roots.add(&pin);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(pin));
  void* pinAddr = pin.heapPointer();
  ASSERT_LT(static_cast<std::byte*>(oldStart), static_cast<std::byte*>(mv.heapPointer()));
  ASSERT_LT(static_cast<std::byte*>(mv.heapPointer()),
            static_cast<std::byte*>(pinAddr));

  gc.collectOld();

  ASSERT_TRUE(pin.isHeap());
  EXPECT_EQ(pin.heapPointer(), pinAddr);
  EXPECT_TRUE(oldWalkVisits(heap, oldStart, pinAddr));
  EXPECT_TRUE(heap.inOld(pin));

  gc.collectOld();

  ASSERT_TRUE(pin.isHeap());
  EXPECT_EQ(pin.heapPointer(), pinAddr);
  EXPECT_TRUE(heap.inOld(pin));
  EXPECT_TRUE(oldWalkVisits(heap, oldStart, pinAddr));
}

TEST(GcOld, DestJumpPastPinDoesNotOverlap) {
  ao::Heap heap(256, 2048);
  ao::Roots roots;
  ao::Gc gc(heap, roots);

  auto pin1 = heap.allocate(ao::Oop::nil(), 0, ao::kFlagImmovable);
  roots.add(&pin1);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(pin1));
  void* pin1Addr = pin1.heapPointer();

  auto hole = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.add(&hole);
  gc.collectNursery();
  roots.remove(&hole);

  auto pin2 = heap.allocate(ao::Oop::nil(), 0, ao::kFlagImmovable);
  roots.add(&pin2);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(pin2));
  void* pin2Addr = pin2.heapPointer();

  auto garbage = heap.allocate(ao::Oop::nil(), 0, 0);
  roots.add(&garbage);
  gc.collectNursery();
  roots.remove(&garbage);

  // 4 slots = 48 bytes; dest at pin1End overlaps pin2, so dest jumps to pin2End.
  auto mv = heap.allocate(ao::Oop::nil(), 4, 0);
  roots.add(&mv);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(mv));
  void* mvBefore = mv.heapPointer();
  ASSERT_LT(static_cast<std::byte*>(pin1Addr), static_cast<std::byte*>(pin2Addr));
  ASSERT_LT(static_cast<std::byte*>(pin2Addr), static_cast<std::byte*>(mvBefore));

  gc.collectOld();

  EXPECT_EQ(pin1.heapPointer(), pin1Addr);
  EXPECT_EQ(pin2.heapPointer(), pin2Addr);
  ASSERT_TRUE(mv.isHeap());
  EXPECT_TRUE(heap.inOld(mv));
  EXPECT_TRUE(oldWalkVisits(heap, pin1Addr, pin1Addr));
  EXPECT_TRUE(oldWalkVisits(heap, pin1Addr, pin2Addr));

  auto* m = static_cast<std::byte*>(mv.heapPointer());
  const std::size_t mn = heap.objectBytes(heap.header(mv));
  auto* p1 = static_cast<std::byte*>(pin1Addr);
  auto* p2 = static_cast<std::byte*>(pin2Addr);
  const std::size_t p1n = heap.objectBytes(heap.header(pin1));
  const std::size_t p2n = heap.objectBytes(heap.header(pin2));
  EXPECT_FALSE(m < p1 + p1n && m + mn > p1);
  EXPECT_FALSE(m < p2 + p2n && m + mn > p2);
}

// old に入らず to-space に残った弱オブジェクトも、スキャベンジ後に弱スロットを直す:
// 死んだ参照先は nil、生きている参照先は転送先。
TEST(GcWeak, WeakSlotInToSpaceSurvivorCleared) {
  ao::Heap heap(512, 0, 0);  // old に何も置けない
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto weak = heap.allocate(ao::Oop::nil(), 2, ao::kFlagWeak);
  auto dead = heap.allocate(ao::Oop::nil(), 0, 0);
  auto live = heap.allocate(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(weak.isHeap());
  ASSERT_TRUE(dead.isHeap());
  ASSERT_TRUE(live.isHeap());
  heap.slotAtPut(weak, 0, dead);
  heap.slotAtPut(weak, 1, live);
  roots.add(&weak);
  roots.add(&live);
  void* const liveBefore = live.heapPointer();

  gc.collectNursery();

  ASSERT_TRUE(weak.isHeap());
  EXPECT_TRUE(heap.inNursery(weak));
  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
  EXPECT_NE(liveBefore, live.heapPointer());
  EXPECT_EQ(live, heap.slotAt(weak, 1));
  EXPECT_TRUE(heap.klass(heap.slotAt(weak, 1)).isNil());

  roots.remove(&live);
  gc.collectNursery();
  EXPECT_TRUE(heap.slotAt(weak, 0).isNil());
  EXPECT_TRUE(heap.slotAt(weak, 1).isNil());
  roots.remove(&weak);
}

// full GC はスキャベンジの後、oldUsed が閾値（最初は old の初期容量）を超えたときだけ走る。
TEST(GcOld, CollectOldOnlyAfterThreshold) {
  ao::Heap heap(4096, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  constexpr std::uint32_t kKiBSlots = (1024 - 16) / 8;  // 1 KiB の object

  auto junk = heap.allocate(ao::Oop::nil(), kKiBSlots, 0);
  roots.add(&junk);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(junk));
  roots.remove(&junk);

  ao::RootedArray keep(roots, 4);
  for (std::uint32_t i = 0; i < 3; ++i) {
    keep[i] = heap.allocate(ao::Oop::nil(), kKiBSlots, 0);
    gc.collectNursery();
  }
  EXPECT_EQ(4096u, heap.oldUsed());  // 閾値ちょうど: まだ走らない（ゴミも残る）
  EXPECT_EQ(0u, heap.oldCollections());

  keep[3] = heap.allocate(ao::Oop::nil(), kKiBSlots, 0);
  gc.collectNursery();
  EXPECT_EQ(1u, heap.oldCollections());
  EXPECT_EQ(4096u, heap.oldUsed());  // junk の 1 KiB が回収された
  for (std::uint32_t i = 0; i < 4; ++i) {
    ASSERT_TRUE(heap.inOld(keep[i]));
    EXPECT_EQ(kKiBSlots, heap.size(keep[i]));
  }
}

// 生存量が初期容量を超えても、何も回収できない full GC を繰り返さない。閾値は 2×生存量に伸び、
// old が上限で生存物が to-space に残る間も、スキャベンジのたびに full GC を走らせない。
TEST(GcOld, NoRepeatedZeroYieldCollects) {
  ao::Heap heap(4096, 4096, 8192);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  constexpr std::uint32_t kKiBSlots = (1024 - 16) / 8;
  ao::RootedArray keep(roots, 11);
  for (std::uint32_t i = 0; i < 8; ++i) {  // 8 KiB がすべて生存し、old は上限まで埋まる
    keep[i] = heap.allocate(ao::Oop::nil(), kKiBSlots, 0);
    heap.slotAtPut(keep[i], 0, ao::Oop::fromSmallInteger(i));
    gc.collectNursery();
  }
  ASSERT_EQ(8192u, heap.oldUsed());
  const auto afterFill = heap.oldCollections();
  EXPECT_LE(afterFill, 1u);

  for (std::uint32_t i = 8; i < 11; ++i) {  // old に入らない生存物
    keep[i] = heap.allocate(ao::Oop::nil(), kKiBSlots, 0);
    heap.slotAtPut(keep[i], 0, ao::Oop::fromSmallInteger(i));
  }
  for (int round = 0; round < 10; ++round) {
    gc.collectNursery();
  }
  EXPECT_EQ(afterFill, heap.oldCollections());  // oldUsed は閾値（上限）を超えない
  for (std::uint32_t i = 0; i < 11; ++i) {
    ASSERT_TRUE(keep[i].isHeap());
    EXPECT_EQ(i < 8, heap.inOld(keep[i])) << i;
    EXPECT_EQ(ao::Oop::fromSmallInteger(i), heap.slotAt(keep[i], 0)) << i;
  }
}

// 同じスロットを 2 回登録しても、collectOld のルート更新はスロットごとに 1 回だけ行う。
// old が [G][A][B] のとき、B の転送先は A の旧番地なので、2 回引くと A を指してしまう。
TEST(GcRoots, DuplicateRootForwardedOnce) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto g = heap.allocate(ao::Oop::nil(), 1, 0);
  auto a = heap.allocate(ao::Oop::nil(), 1, 0);
  auto b = heap.allocate(ao::Oop::nil(), 1, 0);
  roots.add(&g);
  roots.add(&a);
  roots.add(&b);
  gc.collectNursery();
  ASSERT_EQ(static_cast<std::byte*>(g.heapPointer()) + 24,
            static_cast<std::byte*>(a.heapPointer()));
  ASSERT_EQ(static_cast<std::byte*>(a.heapPointer()) + 24,
            static_cast<std::byte*>(b.heapPointer()));
  heap.slotAtPut(a, 0, ao::Oop::fromSmallInteger(1));
  heap.slotAtPut(b, 0, ao::Oop::fromSmallInteger(2));
  roots.remove(&g);
  roots.add(&b);  // 同じスロットの二重登録

  gc.collectOld();

  EXPECT_EQ(ao::Oop::fromSmallInteger(2), heap.slotAt(b, 0));
  EXPECT_EQ(ao::Oop::fromSmallInteger(1), heap.slotAt(a, 0));
  EXPECT_NE(a, b);
  roots.remove(&b);
  roots.remove(&b);
  roots.remove(&a);
}

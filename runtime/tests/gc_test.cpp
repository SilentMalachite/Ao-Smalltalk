#include "ao/Gc.hpp"
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

TEST(GcNursery, OldOomDoesNotClobberRoot) {
  ao::Heap heap(512, 24);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto first = heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(first.isHeap());
  roots.add(&first);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(first));

  auto second = heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(second.isHeap());
  void* secondBefore = second.heapPointer();
  roots.add(&second);
  gc.collectNursery();
  EXPECT_TRUE(second.isHeap());
  EXPECT_EQ(secondBefore, second.heapPointer());
  EXPECT_TRUE(heap.inOld(first));
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

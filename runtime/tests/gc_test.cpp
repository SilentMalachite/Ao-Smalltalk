#include "ao/Gc.hpp"
#include "ao/Heap.hpp"
#include "ao/Oop.hpp"

#include <gtest/gtest.h>

TEST(GcNursery, UnrootedObjectIsReclaimed) {
  ao::Heap heap(512, 4096);
  ao::Gc gc(heap);
  auto keep = heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(keep.isHeap());
  gc.addRoot(&keep);
  (void)heap.allocate(ao::Oop::nil(), 1, 0);  // unrooted garbage
  gc.collectNursery();
  ASSERT_TRUE(keep.isHeap());
  EXPECT_TRUE(heap.inOld(keep));
  // fill nursery; must succeed because garbage was reclaimed
  int allocated = 0;
  for (int i = 0; i < 20; ++i) {
    auto o = heap.allocate(ao::Oop::nil(), 1, 0);
    if (!o.isHeap()) break;
    ++allocated;
  }
  EXPECT_GT(allocated, 0);
}

TEST(GcNursery, RootedObjectSurvivesAndSlotsUpdate) {
  ao::Heap heap(512, 4096);
  ao::Gc gc(heap);
  auto parent = heap.allocate(ao::Oop::nil(), 1, 0);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(parent, 0, child);
  gc.addRoot(&parent);
  gc.collectNursery();
  ASSERT_TRUE(parent.isHeap());
  auto movedChild = heap.slotAt(parent, 0);
  ASSERT_TRUE(movedChild.isHeap());
  EXPECT_TRUE(heap.inOld(parent));
  EXPECT_TRUE(heap.inOld(movedChild));
}

TEST(GcNursery, ImmediateClassIsNotFollowed) {
  ao::Heap heap(512, 4096);
  ao::Gc gc(heap);
  auto obj = heap.allocate(ao::Oop::nil(), 0, 0);
  gc.addRoot(&obj);
  gc.collectNursery();
  EXPECT_TRUE(heap.klass(obj).isNil());
}

TEST(GcNursery, SharedChildCopiedOnce) {
  ao::Heap heap(512, 4096);
  ao::Gc gc(heap);
  auto parent = heap.allocate(ao::Oop::nil(), 2, 0);
  auto child = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(parent, 0, child);
  heap.slotAtPut(parent, 1, child);
  gc.addRoot(&parent);
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
  ao::Gc gc(heap);
  auto obj = heap.allocate(ao::Oop::nil(), 8, ao::kFlagBytes);
  ASSERT_TRUE(obj.isHeap());
  auto* b = heap.bytes(obj);
  for (int i = 0; i < 8; ++i) {
    b[i] = std::byte{0xFF};
  }
  gc.addRoot(&obj);
  gc.collectNursery();
  ASSERT_TRUE(obj.isHeap());
  EXPECT_TRUE(heap.inOld(obj));
  EXPECT_EQ(8u, heap.size(obj));
  EXPECT_EQ(std::byte{0xFF}, heap.bytes(obj)[0]);
  EXPECT_EQ(std::byte{0xFF}, heap.bytes(obj)[7]);
}

TEST(GcOld, NurserySurvivorIsPromoted) {
  ao::Heap heap(512, 8192);
  ao::Gc gc(heap);
  auto obj = heap.allocate(ao::Oop::nil(), 1, 0);
  heap.slotAtPut(obj, 0, ao::Oop::fromSmallInteger(9));
  gc.addRoot(&obj);
  gc.collectNursery();
  ASSERT_TRUE(obj.isHeap());
  EXPECT_TRUE(heap.inOld(obj));
  EXPECT_FALSE(heap.inNursery(obj));
  EXPECT_EQ(ao::Oop::fromSmallInteger(9), heap.slotAt(obj, 0));
}

TEST(GcOld, InternalPointersUpdatedAfterCompact) {
  ao::Heap heap(256, 1024);
  ao::Gc gc(heap);
  // Dead old object must sit at a lower address than live a/b so compact slides them.
  auto garbage = heap.allocate(ao::Oop::nil(), 2, 0);
  gc.addRoot(&garbage);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(garbage));
  gc.removeRoot(&garbage);

  auto a = heap.allocate(ao::Oop::nil(), 1, 0);
  auto b = heap.allocate(ao::Oop::nil(), 0, 0);
  heap.slotAtPut(a, 0, b);
  gc.addRoot(&a);
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

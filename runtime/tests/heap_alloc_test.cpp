#include "ao/Heap.hpp"
#include "ao/Oop.hpp"

#include <gtest/gtest.h>

TEST(HeapAlloc, PointerObjectHasClassAndSize) {
  ao::Heap heap(4096, 4096);
  auto obj = heap.allocate(ao::Oop::nil(), 2, 0);
  ASSERT_TRUE(obj.isHeap());
  EXPECT_TRUE(heap.klass(obj).isNil());
  EXPECT_EQ(2u, heap.size(obj));
  EXPECT_EQ(0u, heap.flags(obj) & ao::kFlagBytes);
  EXPECT_NE(0u, heap.hash(obj));
  heap.slotAtPut(obj, 0, ao::Oop::fromSmallInteger(11));
  heap.slotAtPut(obj, 1, ao::Oop::true_());
  EXPECT_EQ(ao::Oop::fromSmallInteger(11), heap.slotAt(obj, 0));
  EXPECT_TRUE(heap.slotAt(obj, 1).isTrue());
}

TEST(HeapAlloc, ByteObjectPayload) {
  ao::Heap heap(4096, 4096);
  auto obj = heap.allocate(ao::Oop::nil(), 3, ao::kFlagBytes);
  ASSERT_TRUE(obj.isHeap());
  EXPECT_EQ(3u, heap.size(obj));
  auto* b = heap.bytes(obj);
  b[0] = std::byte{'a'};
  b[1] = std::byte{'b'};
  b[2] = std::byte{'c'};
  EXPECT_EQ(std::byte{'a'}, heap.bytes(obj)[0]);
}

TEST(HeapAlloc, ExhaustionReturnsEmpty) {
  ao::Heap heap(64, 64);  // smaller than header+payload * many
  int n = 0;
  while (n < 100) {
    auto o = heap.allocate(ao::Oop::nil(), 1, 0);
    if (!o.isHeap()) break;
    ++n;
  }
  EXPECT_GT(n, 0);
  EXPECT_LT(n, 100);
}

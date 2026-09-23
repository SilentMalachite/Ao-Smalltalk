#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstddef>
#include <cstdint>

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

namespace {

bool slotsAreNil(ao::Heap& heap, ao::Oop obj) {
  for (std::uint32_t i = 0; i < heap.size(obj); ++i) {
    if (!heap.slotAt(obj, i).isNil()) return false;
  }
  return true;
}

}  // namespace

// objectBytes ≥ min(64 KiB, nursery 半面) は nursery を通さず old に置く。GC はしない。
TEST(HeapAlloc, LargeObjectAllocatedInOld) {
  ao::Heap heap;  // nursery 半面 1 MiB なので閾値は 64 KiB
  const auto nurseryBefore = heap.nurseryRemaining();
  constexpr std::uint32_t kSlots = (64u << 10) / 8;  // 64 KiB + header
  auto big = heap.allocate(ao::Oop::nil(), kSlots, 0);
  ASSERT_TRUE(big.isHeap());
  EXPECT_TRUE(heap.inOld(big));
  EXPECT_FALSE(heap.inNursery(big));
  EXPECT_NE(0u, heap.flags(big) & ao::kFlagOld);
  EXPECT_EQ(kSlots, heap.size(big));
  EXPECT_TRUE(slotsAreNil(heap, big));
  EXPECT_EQ(nurseryBefore, heap.nurseryRemaining());

  // objectBytes がちょうど 64 KiB なら old、8 バイト下なら nursery。
  auto atThreshold = heap.allocate(ao::Oop::nil(), (64u << 10) - 16, ao::kFlagBytes);
  ASSERT_TRUE(atThreshold.isHeap());
  EXPECT_TRUE(heap.inOld(atThreshold));
  auto below = heap.allocate(ao::Oop::nil(), (64u << 10) - 24, ao::kFlagBytes);
  ASSERT_TRUE(below.isHeap());
  EXPECT_TRUE(heap.inNursery(below));

  // nursery 半面が 64 KiB より小さいときは、半面が閾値になる。
  ao::Heap tiny(512, 4096);
  auto half = tiny.allocate(ao::Oop::nil(), (512 - 16) / 8, 0);
  ASSERT_TRUE(half.isHeap());
  EXPECT_TRUE(tiny.inOld(half));
}

TEST(HeapAlloc, ObjectLargerThanNurseryAllocates) {
  ao::Heap heap(4096, 4096);
  auto arr = heap.allocate(ao::Oop::nil(), 1000, 0);  // 8016 バイト: nursery 半面と old の初期容量を超える
  ASSERT_TRUE(arr.isHeap());
  EXPECT_TRUE(heap.inOld(arr));
  EXPECT_EQ(1000u, heap.size(arr));
  EXPECT_TRUE(slotsAreNil(heap, arr));
  heap.slotAtPut(arr, 999, ao::Oop::fromSmallInteger(7));
  EXPECT_EQ(ao::Oop::fromSmallInteger(7), heap.slotAt(arr, 999));

  auto str = heap.allocate(ao::Oop::nil(), 10000, ao::kFlagBytes);
  ASSERT_TRUE(str.isHeap());
  EXPECT_TRUE(heap.inOld(str));
  EXPECT_EQ(10000u, heap.size(str));
  EXPECT_EQ(std::byte{0}, heap.bytes(str)[0]);
  EXPECT_EQ(std::byte{0}, heap.bytes(str)[9999]);
}

TEST(HeapAlloc, OldGrowsPastInitialCapacity) {
  ao::Heap heap(4096, 64u << 10, 16u << 20);
  const std::byte* const base = heap.oldBase();
  EXPECT_EQ(64u << 10, heap.oldCapacity());
  EXPECT_EQ(16u << 20, heap.oldMaxBytes());
  auto first = heap.allocateTenured(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(first.isHeap());
  ASSERT_TRUE(heap.inOld(first));
  heap.slotAtPut(first, 0, ao::Oop::fromSmallInteger(42));

  for (int i = 0; i < 64; ++i) {  // 64 × 48 KiB = 3 MiB
    auto o = heap.allocate(ao::Oop::nil(), 48u << 10, ao::kFlagBytes);
    ASSERT_TRUE(o.isHeap()) << i;
    ASSERT_TRUE(heap.inOld(o)) << i;
    heap.bytes(o)[(48u << 10) - 1] = std::byte{0x5A};
  }
  EXPECT_GT(heap.oldUsed(), 3u << 20);
  EXPECT_GE(heap.oldCapacity(), heap.oldUsed());
  EXPECT_LE(heap.oldCapacity(), heap.oldMaxBytes());
  EXPECT_EQ(base, heap.oldBase());  // 伸ばしてもアドレスは動かない
  EXPECT_TRUE(heap.inOld(first));
  EXPECT_EQ(ao::Oop::fromSmallInteger(42), heap.slotAt(first, 0));
}

TEST(HeapAlloc, OldStopsAtMax) {
  constexpr std::size_t kMax = 256u << 10;
  ao::Heap heap(4096, 64u << 10, kMax);
  ao::Roots roots;
  ao::RootedArray live(roots, 17);  // 上限まで生きている object で埋める
  EXPECT_EQ(kMax, heap.oldMaxBytes());
  std::uint32_t made = 0;
  while (true) {
    auto o = heap.allocateTenured(ao::Oop::nil(), (16u << 10) - 16, ao::kFlagBytes);
    if (!o.isHeap()) break;
    ASSERT_LT(made, 16u);
    live[made++] = o;
  }
  EXPECT_EQ(16u, made);  // 16 KiB ちょうどの object が上限まで 16 個
  EXPECT_EQ(kMax, heap.oldUsed());
  EXPECT_LE(heap.oldCapacity(), kMax);
  EXPECT_FALSE(heap.allocate(ao::Oop::nil(), 1000, 0).isHeap());  // 大きな object の経路も止まる
  EXPECT_FALSE(heap.allocateNoGc(ao::Oop::nil(), 1000, 0).isHeap());
  EXPECT_TRUE(heap.allocate(ao::Oop::nil(), 1, 0).isHeap());  // nursery は使える

  // allocateRetry は nursery → GC → nursery → old の順に試し、すべて失敗したら OOM を立てる。
  ao::WellKnown wk(heap, roots);
  ao::ClassMethodCache cache;
  ao::CallContext ctx{heap, roots, wk, &cache};
  EXPECT_FALSE(heap.outOfMemory());
  EXPECT_FALSE(ao::allocateRetry(ctx, ao::Oop::nil(), 1000, 0).isHeap());
  EXPECT_TRUE(heap.outOfMemory());
  heap.clearOutOfMemory();
  EXPECT_FALSE(heap.outOfMemory());
  EXPECT_TRUE(ao::allocateRetry(ctx, ao::Oop::nil(), 1, 0).isHeap());
  EXPECT_FALSE(heap.outOfMemory());

  // 上限でも、old のゴミは回収してから置く。
  live[0] = ao::Oop::nil();
  auto reused = ao::allocateRetry(ctx, ao::Oop::nil(), (16u << 10) - 16, ao::kFlagBytes);
  ASSERT_TRUE(reused.isHeap());
  EXPECT_TRUE(heap.inOld(reused));
  EXPECT_FALSE(heap.outOfMemory());
}

TEST(HeapAlloc, AllocateNoGcSpillsToOld) {
  ao::Heap heap(512, 4096);
  auto first = heap.allocateNoGc(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(first.isHeap());
  ASSERT_TRUE(heap.inNursery(first));
  void* const firstAt = first.heapPointer();
  while (heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
  const auto oldBefore = heap.oldUsed();
  auto spilled = heap.allocateNoGc(ao::Oop::nil(), 2, 0);
  ASSERT_TRUE(spilled.isHeap());
  EXPECT_TRUE(heap.inOld(spilled));
  EXPECT_EQ(oldBefore + 32, heap.oldUsed());
  EXPECT_TRUE(slotsAreNil(heap, spilled));
  // GC は走っていない: nursery の object は同じ番地のまま。
  EXPECT_TRUE(heap.inNursery(first));
  EXPECT_EQ(firstAt, first.heapPointer());
}

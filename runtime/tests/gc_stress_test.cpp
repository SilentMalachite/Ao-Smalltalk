#include "ao/Gc.hpp"
#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>

namespace {

// AO_GC_STRESS を一時的に書き換え、スコープを抜けるときに元へ戻す。
class ScopedGcStressEnv {
 public:
  ScopedGcStressEnv() {
    if (const char* v = std::getenv("AO_GC_STRESS")) {
      saved_ = v;
    }
  }
  ~ScopedGcStressEnv() {
    if (saved_) {
      setenv("AO_GC_STRESS", saved_->c_str(), 1);
    } else {
      unsetenv("AO_GC_STRESS");
    }
  }
  ScopedGcStressEnv(const ScopedGcStressEnv&) = delete;
  ScopedGcStressEnv& operator=(const ScopedGcStressEnv&) = delete;

 private:
  std::optional<std::string> saved_;
};

std::uint64_t rawWordAt(const void* p) {
  std::uint64_t w = 0;
  std::memcpy(&w, p, sizeof w);
  return w;
}

}  // namespace

TEST(GcStress, EnvEnablesStress) {
  ScopedGcStressEnv restore;
  setenv("AO_GC_STRESS", "3", 1);
  EXPECT_EQ(3u, ao::Heap(512, 4096).gcStress());
  unsetenv("AO_GC_STRESS");
  EXPECT_EQ(0u, ao::Heap(512, 4096).gcStress());
  for (const char* off : {"", "0", "abc", "3x", "-2"}) {
    setenv("AO_GC_STRESS", off, 1);
    EXPECT_EQ(0u, ao::Heap(512, 4096).gcStress()) << "AO_GC_STRESS=\"" << off << "\"";
  }
}

TEST(GcStress, SafepointCollectsNurseryOnEveryNthCall) {
  ao::Heap heap(4096, 16384);
  ao::Roots roots;
  heap.setGcStress(3);
  ASSERT_EQ(3u, heap.gcStress());
  auto obj = heap.allocate(ao::Oop::nil(), 1, 0);
  roots.add(&obj);
  ao::Gc gc(heap, roots);
  gc.safepoint();
  gc.safepoint();
  EXPECT_TRUE(heap.inNursery(obj));
  gc.safepoint();
  EXPECT_TRUE(heap.inOld(obj));
  roots.remove(&obj);
}

TEST(GcStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail) {
  ao::Heap heap(4096, 16384);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  auto obj = heap.allocate(ao::Oop::nil(), 1, 0);
  roots.add(&obj);
  gc.collectNursery();
  ASSERT_TRUE(heap.inOld(obj));
  const auto used = heap.oldUsed();
  const void* oldAddr = obj.heapPointer();
  roots.remove(&obj);  // old space の garbage にする

  heap.setGcStress(1);
  gc.safepoint();
  gc.safepoint();
  gc.safepoint();
  EXPECT_EQ(used, heap.oldUsed());
  gc.safepoint();
  EXPECT_EQ(0u, heap.oldUsed());
  EXPECT_EQ(ao::kGcPoisonWord, rawWordAt(oldAddr));
}

TEST(GcStress, FreedNurseryIsPoisonedAndReallocationIsClean) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  heap.setGcStress(1);
  auto deadPtrs = heap.allocate(ao::Oop::nil(), 2, 0);
  auto deadBytes = heap.allocate(ao::Oop::nil(), 5, ao::kFlagBytes);
  ao::Gc gc(heap, roots);
  gc.safepoint();
  EXPECT_EQ(ao::kGcPoisonWord, rawWordAt(deadPtrs.heapPointer()));
  EXPECT_EQ(ao::kGcPoisonWord, rawWordAt(deadBytes.heapPointer()));

  // 2 回目で両半空間が毒で埋まる。そこへ割り当てても中身は毒を含まない。
  gc.safepoint();
  auto bytes = heap.allocate(ao::Oop::nil(), 5, ao::kFlagBytes);
  ASSERT_TRUE(bytes.isHeap());
  const auto* payload = heap.bytes(bytes);
  for (int i = 0; i < 8; ++i) {
    EXPECT_EQ(std::byte{0}, payload[i]) << "byte " << i;
  }
  auto ptrs = heap.allocate(ao::Oop::nil(), 2, 0);
  ASSERT_TRUE(ptrs.isHeap());
  EXPECT_TRUE(heap.klass(ptrs).isNil());
  EXPECT_TRUE(heap.slotAt(ptrs, 0).isNil());
  EXPECT_TRUE(heap.slotAt(ptrs, 1).isNil());
}

TEST(GcStressDeathTest, StaleHeaderReadAborts) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  heap.setGcStress(1);
  auto stale = heap.allocate(ao::Oop::nil(), 1, 0);
  ao::Gc gc(heap, roots);
  gc.safepoint();
  EXPECT_DEATH((void)heap.header(stale), "stale");
}

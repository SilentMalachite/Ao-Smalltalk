#pragma once

#include "ao/Oop.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace ao {

inline constexpr std::uint16_t kFlagBytes     = 1u << 0;
inline constexpr std::uint16_t kFlagWeak      = 1u << 1;
inline constexpr std::uint16_t kFlagOld       = 1u << 2;
inline constexpr std::uint16_t kFlagImmovable = 1u << 3;
inline constexpr std::uint16_t kFlagMarked    = 1u << 4;
inline constexpr std::uint16_t kFlagForwarded = 1u << 5;

// GC ストレスモードで解放領域を埋める値。解放済みの番地を header として読むと klass がこの語になる。
inline constexpr std::uint8_t kGcPoisonByte = 0xA5;
inline constexpr std::uint64_t kGcPoisonWord = 0xA5A5A5A5A5A5A5A5ull;

struct ObjectHeader {
  Oop klass;
  std::uint32_t size;
  std::uint16_t flags;
  std::uint16_t hash;
};
static_assert(sizeof(ObjectHeader) == 16);

class Heap {
 public:
  explicit Heap(std::size_t nurseryBytes = 1 << 20, std::size_t oldBytes = 4 << 20);

  // size: slot count if pointers, byte count if kFlagBytes.
  // class may be immediate in P1 tests (typically nil).
  // On nursery full: return empty Oop (isHeap()==false). Does not GC.
  Oop allocate(Oop cls, std::uint32_t size, std::uint16_t flags);

  ObjectHeader* header(Oop obj);
  const ObjectHeader* header(Oop obj) const;
  Oop klass(Oop obj) const;
  std::uint32_t size(Oop obj) const;
  std::uint16_t flags(Oop obj) const;
  std::uint16_t hash(Oop obj) const;

  Oop slotAt(Oop obj, std::uint32_t i) const;
  void slotAtPut(Oop obj, std::uint32_t i, Oop value);
  std::byte* bytes(Oop obj);

  std::size_t objectBytes(const ObjectHeader* h) const;  // header + payload, 8-aligned

  bool inNursery(Oop obj) const;
  bool inOld(Oop obj) const;
  std::size_t oldUsed() const;
  std::size_t oldCapacity() const;
  const std::byte* oldBase() const;
  std::size_t nurseryRemaining() const;
  std::size_t nurseryCapacity() const;
  bool adoptOldBytes(const std::byte* src, std::size_t n, std::uint16_t nextHash);
  std::uint16_t hashCursor() const;

  // GC ストレス: safepoint と allocateRetry の n 回に 1 回ナーサリ GC を走らせる（0 は無効）。
  // コンストラクタは環境変数 AO_GC_STRESS（正の十進整数）を読む。
  void setGcStress(std::uint32_t n);
  std::uint32_t gcStress() const { return gcStress_; }

  friend class Gc;

 private:
  void flipNursery();
  std::byte* reserveToSpace(std::size_t n);
  bool containsNurseryFrom(void* p) const;
  bool fitsOld(std::size_t n) const;
  std::byte* reserveOld(std::size_t n);
  bool stressDue() {
    if (gcStress_ == 0 || ++stressTicks_ < gcStress_) {
      return false;
    }
    stressTicks_ = 0;
    return true;
  }
  void poisonFreed(std::byte* begin, std::byte* end);
  void checkNotPoisoned(Oop obj) const;

  std::unique_ptr<std::byte[]> nursery_;
  std::unique_ptr<std::byte[]> old_;
  std::size_t nurseryHalf_ = 0;
  std::byte* fromStart_ = nullptr;
  std::byte* fromEnd_ = nullptr;
  std::byte* fromBump_ = nullptr;
  std::byte* toStart_ = nullptr;
  std::byte* toEnd_ = nullptr;
  std::byte* toBump_ = nullptr;
  std::byte* oldStart_ = nullptr;
  std::byte* oldEnd_ = nullptr;
  std::byte* oldBump_ = nullptr;
  std::uint16_t nextHash_ = 1;
  std::uint32_t gcStress_ = 0;
  std::uint32_t stressTicks_ = 0;
  std::uint32_t stressCollections_ = 0;
};

}  // namespace ao

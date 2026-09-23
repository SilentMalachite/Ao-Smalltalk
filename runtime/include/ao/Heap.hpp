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

// old space は仮想領域を 1 つ予約し、この単位でコミットする。
inline constexpr std::size_t kOldCommitUnit = std::size_t{1} << 20;
// old の既定の上限。イメージヘッダの uint32 heapBytes で表せる最大値をコミット単位に揃えた値
// （4 GiB − 1 MiB）。
inline constexpr std::size_t kOldMaxBytes =
    (std::size_t{UINT32_MAX} / kOldCommitUnit) * kOldCommitUnit;
// この大きさ以上の object は nursery を通さず old に置く（nursery 半面が小さければ半面）。
inline constexpr std::size_t kLargeObjectBytes = std::size_t{64} << 10;

struct ObjectHeader {
  Oop klass;
  std::uint32_t size;
  std::uint16_t flags;
  std::uint16_t hash;
};
static_assert(sizeof(ObjectHeader) == 16);

class VirtualRegion;

class Heap {
 public:
  // old は oldMaxBytes 分のアドレスを予約し、oldBytes（初期容量）から必要な分だけ伸ばす。
  explicit Heap(std::size_t nurseryBytes = 1 << 20, std::size_t oldBytes = 4 << 20,
                std::size_t oldMaxBytes = kOldMaxBytes);
  ~Heap();
  Heap(Heap&&) noexcept;
  Heap& operator=(Heap&&) noexcept;
  Heap(const Heap&) = delete;
  Heap& operator=(const Heap&) = delete;

  // size: slot count if pointers, byte count if kFlagBytes.
  // class may be immediate in P1 tests (typically nil).
  // Objects of largeObjectBytes() or more go straight to old (allocateTenured).
  // On nursery full (or old at its max for a large object): return empty Oop. Does not GC.
  Oop allocate(Oop cls, std::uint32_t size, std::uint16_t flags);
  // Places the object in old, growing its commit up to oldMaxBytes(). Empty Oop at the max.
  // Does not GC.
  Oop allocateTenured(Oop cls, std::uint32_t size, std::uint16_t flags);
  // Nursery, then old when the nursery is full. Does not GC. For Heap-level code. Callers set the
  // out-of-memory flag when it fails (old at its max).
  Oop allocateNoGc(Oop cls, std::uint32_t size, std::uint16_t flags);
  std::size_t largeObjectBytes() const;

  // Set when an allocation finally fails: allocateRetry after its collections and in old, and
  // the allocations that do not collect (intern, method dictionaries, NativeMethod::create) at
  // old's max.
  void setOutOfMemory() { outOfMemory_ = true; }
  bool outOfMemory() const { return outOfMemory_; }
  void clearOutOfMemory() { outOfMemory_ = false; }

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

  // Bytes a new object of this size and flags takes (header + payload, 8-aligned).
  std::size_t objectBytesFor(std::uint32_t size, std::uint16_t flags) const;

  bool inNursery(Oop obj) const;
  bool inOld(Oop obj) const;
  std::size_t oldUsed() const;
  // Bytes old can hold before it next grows (never more than oldMaxBytes()).
  std::size_t oldCapacity() const;
  std::size_t oldMaxBytes() const { return oldMax_; }
  // Number of collectOld runs so far.
  std::uint64_t oldCollections() const { return oldCollections_; }
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
  bool containsNurseryTo(void* p) const;
  bool fitsOld(std::size_t n) const;
  // Bump-allocates n bytes in old, growing the commit when needed. nullptr at oldMaxBytes().
  std::byte* reserveOld(std::size_t n);
  bool growOld(std::size_t neededBytes);
  Oop initObject(std::byte* at, Oop cls, std::uint32_t size, std::uint16_t flags,
                 std::size_t n);
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
  std::unique_ptr<VirtualRegion> old_;
  std::size_t oldInitial_ = 0;
  std::size_t oldMax_ = 0;
  // collectNursery runs collectOld once oldUsed passes this. collectOld resets it to
  // clamp(2 × live, initial, max).
  std::size_t oldThreshold_ = 0;
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
  std::uint64_t oldCollections_ = 0;
  bool outOfMemory_ = false;
};

}  // namespace ao

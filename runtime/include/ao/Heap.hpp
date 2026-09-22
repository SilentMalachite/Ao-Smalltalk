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

  friend class Gc;

 private:
  void flipNursery();
  std::byte* reserveToSpace(std::size_t n);
  bool containsNurseryFrom(void* p) const;
  bool fitsOld(std::size_t n) const;
  std::byte* reserveOld(std::size_t n);

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
};

}  // namespace ao

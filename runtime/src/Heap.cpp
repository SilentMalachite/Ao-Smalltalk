#include "ao/Heap.hpp"

#include <cassert>
#include <cstring>
#include <utility>

namespace ao {

static std::size_t align8(std::size_t n) { return (n + 7u) & ~std::size_t{7}; }

Heap::Heap(std::size_t nurseryBytes, std::size_t oldBytes)
    : nursery_(std::make_unique<std::byte[]>(2 * nurseryBytes)),
      old_(std::make_unique<std::byte[]>(oldBytes)),
      nurseryHalf_(nurseryBytes) {
  fromStart_ = nursery_.get();
  fromEnd_ = fromStart_ + nurseryBytes;
  fromBump_ = fromStart_;
  toStart_ = fromEnd_;
  toEnd_ = toStart_ + nurseryBytes;
  toBump_ = toStart_;
  oldStart_ = old_.get();
  oldEnd_ = oldStart_ + oldBytes;
  oldBump_ = oldStart_;
}

std::size_t Heap::objectBytes(const ObjectHeader* h) const {
  std::size_t payload = (h->flags & kFlagBytes) ? h->size
                                                : static_cast<std::size_t>(h->size) * 8u;
  return align8(sizeof(ObjectHeader) + payload);
}

Oop Heap::allocate(Oop cls, std::uint32_t size, std::uint16_t flags) {
  ObjectHeader probe{};
  probe.size = size;
  probe.flags = flags;
  const std::size_t n = objectBytes(&probe);
  if (fromBump_ + n > fromEnd_) {
    return Oop{};
  }
  auto* h = reinterpret_cast<ObjectHeader*>(fromBump_);
  fromBump_ += n;
  h->klass = cls;
  h->size = size;
  h->flags = static_cast<std::uint16_t>(flags & ~kFlagOld);
  h->hash = nextHash_++;
  if (nextHash_ == 0) nextHash_ = 1;
  if (flags & kFlagBytes) {
    std::memset(reinterpret_cast<std::byte*>(h + 1), 0, size);
  } else {
    auto* slots = reinterpret_cast<Oop*>(h + 1);
    for (std::uint32_t i = 0; i < size; ++i) slots[i] = Oop::nil();
  }
  return Oop::fromHeap(h);
}

ObjectHeader* Heap::header(Oop obj) {
  return static_cast<ObjectHeader*>(obj.heapPointer());
}

const ObjectHeader* Heap::header(Oop obj) const {
  return static_cast<const ObjectHeader*>(obj.heapPointer());
}

Oop Heap::klass(Oop obj) const { return header(obj)->klass; }

std::uint32_t Heap::size(Oop obj) const { return header(obj)->size; }

std::uint16_t Heap::flags(Oop obj) const { return header(obj)->flags; }

std::uint16_t Heap::hash(Oop obj) const { return header(obj)->hash; }

Oop Heap::slotAt(Oop obj, std::uint32_t i) const {
  const ObjectHeader* h = header(obj);
  assert((h->flags & kFlagBytes) == 0);
  assert(i < h->size);
  const auto* slots = reinterpret_cast<const Oop*>(h + 1);
  return slots[i];
}

void Heap::slotAtPut(Oop obj, std::uint32_t i, Oop value) {
  ObjectHeader* h = header(obj);
  assert((h->flags & kFlagBytes) == 0);
  assert(i < h->size);
  auto* slots = reinterpret_cast<Oop*>(h + 1);
  slots[i] = value;
}

std::byte* Heap::bytes(Oop obj) {
  return reinterpret_cast<std::byte*>(header(obj) + 1);
}

bool Heap::inNursery(Oop obj) const {
  if (!obj.isHeap()) {
    return false;
  }
  auto* p = static_cast<std::byte*>(obj.heapPointer());
  auto* start = nursery_.get();
  return p >= start && p < start + 2 * nurseryHalf_;
}

bool Heap::inOld(Oop obj) const {
  if (!obj.isHeap()) {
    return false;
  }
  auto* p = static_cast<std::byte*>(obj.heapPointer());
  return p >= oldStart_ && p < oldEnd_;
}

void Heap::flipNursery() {
  std::swap(fromStart_, toStart_);
  std::swap(fromEnd_, toEnd_);
  fromBump_ = toBump_;
  toBump_ = toStart_;
}

std::byte* Heap::reserveToSpace(std::size_t n) {
  if (toBump_ + n > toEnd_) {
    return nullptr;
  }
  std::byte* dest = toBump_;
  toBump_ += n;
  return dest;
}

bool Heap::containsNurseryFrom(void* p) const {
  auto* b = static_cast<std::byte*>(p);
  return b >= fromStart_ && b < fromEnd_;
}

}  // namespace ao

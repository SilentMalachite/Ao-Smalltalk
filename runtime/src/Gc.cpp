#include "ao/Gc.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace ao {

Gc::Gc(Heap& heap) : heap_(&heap) {}

void Gc::addRoot(Oop* slot) { roots_.push_back(slot); }

void Gc::removeRoot(Oop* slot) {
  roots_.erase(std::remove(roots_.begin(), roots_.end(), slot), roots_.end());
}

Oop Gc::copy(Oop obj) {
  if (!obj.isHeap()) {
    return obj;
  }
  if (heap_->inOld(obj)) {
    return obj;
  }
  if (!heap_->containsNurseryFrom(obj.heapPointer())) {
    return obj;
  }
  ObjectHeader* h = heap_->header(obj);
  if (h->flags & kFlagForwarded) {
    return h->klass;
  }
  const std::size_t n = heap_->objectBytes(h);
  std::byte* dest = heap_->reserveToSpace(n);
  assert(dest != nullptr);
  std::memcpy(dest, h, n);
  Oop nw = Oop::fromHeap(dest);
  h->flags = static_cast<std::uint16_t>(h->flags | kFlagForwarded);
  h->klass = nw;
  return nw;
}

void Gc::collectNursery() {
  for (Oop* slot : roots_) {
    if (slot != nullptr) {
      *slot = copy(*slot);
    }
  }
  std::byte* scan = heap_->toStart_;
  while (scan < heap_->toBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    h->klass = copy(h->klass);
    if ((h->flags & kFlagBytes) == 0) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        slots[i] = copy(slots[i]);
      }
    }
    scan += heap_->objectBytes(h);
  }
  heap_->flipNursery();
}

}  // namespace ao

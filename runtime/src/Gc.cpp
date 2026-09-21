#include "ao/Gc.hpp"

#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
  if (!heap_->fitsOld(n)) {
    collectOld();
    if (!heap_->fitsOld(n)) {
      return Oop{};
    }
  }
  std::byte* dest = heap_->reserveOld(n);
  if (dest == nullptr) {
    return Oop{};
  }
  std::memcpy(dest, h, n);
  auto* nh = reinterpret_cast<ObjectHeader*>(dest);
  nh->flags = static_cast<std::uint16_t>((nh->flags | kFlagOld) & ~kFlagForwarded);
  Oop nw = Oop::fromHeap(dest);
  h->flags = static_cast<std::uint16_t>(h->flags | kFlagForwarded);
  h->klass = nw;
  return nw;
}

void Gc::collectNursery() {
  oldCompacted_ = false;
  std::byte* scan = heap_->oldBump_;
  for (Oop* slot : roots_) {
    if (slot != nullptr) {
      *slot = copy(*slot);
    }
  }
  auto restartIfCompacted = [&]() {
    if (!oldCompacted_) {
      return false;
    }
    oldCompacted_ = false;
    scan = heap_->oldStart_;
    return true;
  };
  restartIfCompacted();
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    const bool bytes = (h->flags & kFlagBytes) != 0;
    const std::uint32_t sz = h->size;
    h->klass = copy(h->klass);
    if (restartIfCompacted()) {
      continue;
    }
    if (!bytes) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      bool restarted = false;
      for (std::uint32_t i = 0; i < sz; ++i) {
        slots[i] = copy(slots[i]);
        if (restartIfCompacted()) {
          restarted = true;
          break;
        }
      }
      if (restarted) {
        continue;
      }
    }
    scan += n;
  }
  heap_->flipNursery();
}

void Gc::collectOld() {
  oldCompacted_ = true;

  std::vector<Oop> stack;
  for (Oop* slot : roots_) {
    if (slot != nullptr) {
      stack.push_back(*slot);
    }
  }

  std::unordered_set<std::uintptr_t> nurserySeen;
  while (!stack.empty()) {
    Oop obj = stack.back();
    stack.pop_back();
    if (!obj.isHeap()) {
      continue;
    }
    if (heap_->inOld(obj)) {
      ObjectHeader* h = heap_->header(obj);
      if (h->flags & kFlagMarked) {
        continue;
      }
      h->flags = static_cast<std::uint16_t>(h->flags | kFlagMarked);
      stack.push_back(h->klass);
      if ((h->flags & kFlagBytes) == 0) {
        auto* slots = reinterpret_cast<Oop*>(h + 1);
        for (std::uint32_t i = 0; i < h->size; ++i) {
          stack.push_back(slots[i]);
        }
      }
      continue;
    }
    if (!heap_->inNursery(obj)) {
      continue;
    }
    auto key = reinterpret_cast<std::uintptr_t>(obj.heapPointer());
    if (!nurserySeen.insert(key).second) {
      continue;
    }
    ObjectHeader* h = heap_->header(obj);
    stack.push_back(h->klass);
    if ((h->flags & kFlagBytes) == 0) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        stack.push_back(slots[i]);
      }
    }
  }

  std::unordered_map<std::uintptr_t, Oop> fwd;
  std::byte* dest = heap_->oldStart_;
  std::byte* scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if (h->flags & kFlagMarked) {
      fwd.emplace(reinterpret_cast<std::uintptr_t>(h), Oop::fromHeap(dest));
      dest += n;
    }
    scan += n;
  }

  auto update = [&](Oop o) -> Oop {
    if (!o.isHeap() || !heap_->inOld(o)) {
      return o;
    }
    auto it = fwd.find(reinterpret_cast<std::uintptr_t>(o.heapPointer()));
    if (it == fwd.end()) {
      return o;
    }
    return it->second;
  };

  for (Oop* slot : roots_) {
    if (slot != nullptr) {
      *slot = update(*slot);
    }
  }

  auto updateObject = [&](ObjectHeader* h) {
    h->klass = update(h->klass);
    if ((h->flags & kFlagBytes) == 0) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        slots[i] = update(slots[i]);
      }
    }
  };

  std::byte* nscan = heap_->fromStart_;
  while (nscan < heap_->fromBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(nscan);
    const std::size_t n = heap_->objectBytes(h);
    updateObject(h);
    nscan += n;
  }

  scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if (h->flags & kFlagMarked) {
      updateObject(h);
    }
    scan += n;
  }

  dest = heap_->oldStart_;
  scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    std::byte* next = scan + n;
    if (h->flags & kFlagMarked) {
      if (dest != scan) {
        std::memmove(dest, scan, n);
      }
      auto* nh = reinterpret_cast<ObjectHeader*>(dest);
      nh->flags = static_cast<std::uint16_t>(nh->flags & ~kFlagMarked);
      dest += n;
    }
    scan = next;
  }
  heap_->oldBump_ = dest;
}

}  // namespace ao

#include "ao/Gc.hpp"

#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ao {

Gc::Gc(Heap& heap, Roots& roots) : heap_(&heap), roots_(&roots) {}

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
      failed_ = true;
      return obj;
    }
  }
  std::byte* dest = heap_->reserveOld(n);
  if (dest == nullptr) {
    failed_ = true;
    return obj;
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
  failed_ = false;
  do {
    oldCompacted_ = false;
    scavengeFromRoots();
  } while (oldCompacted_ && !failed_);
  if (!failed_) {
    clearWeakAfterNursery();
    heap_->flipNursery();
  }
}

void Gc::scavengeFromRoots() {
  std::vector<Oop> stack;
  std::unordered_set<std::uintptr_t> visited;

  struct Ctx {
    Gc* gc;
    std::vector<Oop>* stack;
  } ctx{this, &stack};

  roots_->visitAll(
      [](void* v, Oop* slot) {
        auto* c = static_cast<Ctx*>(v);
        if (slot == nullptr || c->gc->failed_) {
          return;
        }
        *slot = c->gc->copy(*slot);
        if (c->gc->failed_ || c->gc->oldCompacted_) {
          return;
        }
        if (slot->isHeap()) {
          c->stack->push_back(*slot);
        }
      },
      &ctx);

  while (!stack.empty() && !failed_ && !oldCompacted_) {
    Oop obj = stack.back();
    stack.pop_back();
    if (!obj.isHeap() || !heap_->inOld(obj)) {
      continue;
    }
    auto key = reinterpret_cast<std::uintptr_t>(obj.heapPointer());
    if (!visited.insert(key).second) {
      continue;
    }
    ObjectHeader* h = heap_->header(obj);
    h->klass = copy(h->klass);
    if (failed_ || oldCompacted_) {
      return;
    }
    if (h->klass.isHeap()) {
      stack.push_back(h->klass);
    }
    if ((h->flags & kFlagBytes) != 0) {
      continue;
    }
    if ((h->flags & kFlagWeak) != 0) {
      continue;
    }
    auto* slots = reinterpret_cast<Oop*>(h + 1);
    for (std::uint32_t i = 0; i < h->size; ++i) {
      slots[i] = copy(slots[i]);
      if (failed_ || oldCompacted_) {
        return;
      }
      if (slots[i].isHeap()) {
        stack.push_back(slots[i]);
      }
    }
  }
}

void Gc::clearWeakAfterNursery() {
  std::byte* scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if ((h->flags & kFlagWeak) != 0 && (h->flags & kFlagBytes) == 0) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        Oop s = slots[i];
        if (!s.isHeap()) {
          continue;
        }
        if (heap_->inOld(s)) {
          continue;
        }
        if (!heap_->containsNurseryFrom(s.heapPointer())) {
          continue;
        }
        ObjectHeader* ch = heap_->header(s);
        if (ch->flags & kFlagForwarded) {
          slots[i] = ch->klass;
        } else {
          slots[i] = Oop::nil();
        }
      }
    }
    scan += n;
  }
}

void Gc::clearWeakAfterOldMark() {
  std::byte* scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if ((h->flags & kFlagMarked) && (h->flags & kFlagWeak) &&
        (h->flags & kFlagBytes) == 0) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        Oop s = slots[i];
        if (!s.isHeap() || !heap_->inOld(s)) {
          continue;
        }
        if ((heap_->header(s)->flags & kFlagMarked) == 0) {
          slots[i] = Oop::nil();
        }
      }
    }
    scan += n;
  }
}

void Gc::collectOld() {
  oldCompacted_ = true;

  std::vector<Oop> stack;
  roots_->visitAll(
      [](void* ctx, Oop* slot) {
        if (slot != nullptr) {
          static_cast<std::vector<Oop>*>(ctx)->push_back(*slot);
        }
      },
      &stack);

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
      if ((h->flags & kFlagBytes) == 0 && (h->flags & kFlagWeak) == 0) {
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
    if ((h->flags & kFlagBytes) == 0 && (h->flags & kFlagWeak) == 0) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        stack.push_back(slots[i]);
      }
    }
  }

  clearWeakAfterOldMark();

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

  struct UpdateCtx {
    decltype(update)* fn;
  } upd{&update};
  roots_->visitAll(
      [](void* ctx, Oop* slot) {
        if (slot != nullptr) {
          *slot = (*static_cast<UpdateCtx*>(ctx)->fn)(*slot);
        }
      },
      &upd);

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

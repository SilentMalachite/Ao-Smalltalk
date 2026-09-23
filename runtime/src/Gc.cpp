#include "ao/Gc.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ao {

Gc::Gc(Heap& heap, Roots& roots) : heap_(&heap), roots_(&roots) {}

void Gc::safepoint() {
  stressPoint();
  if (heap_->nurseryRemaining() < heap_->nurseryCapacity() / 8) {
    collectNursery();
  }
}

void Gc::stressPoint() {
  if (!heap_->stressDue()) {
    return;
  }
  collectNursery();
  if (++heap_->stressCollections_ % 4 == 0) {
    collectOld();
  }
}

Oop Gc::copy(Oop obj) {
  if (!obj.isHeap() || !heap_->containsNurseryFrom(obj.heapPointer())) {
    return obj;
  }
  ObjectHeader* h = heap_->header(obj);
  if (h->flags & kFlagForwarded) {
    return h->klass;
  }
  const std::size_t n = heap_->objectBytes(h);
  auto* const at = static_cast<std::byte*>(obj.heapPointer());
  if (at >= heap_->fromBump_ || n > static_cast<std::size_t>(heap_->fromBump_ - at)) {
    // 生きている object は fromBump_ より前に収まる。はみ出すのは、解放済みの番地を指したまま
    // 再利用された領域を読んでいる OOP（ルートし忘れ）である。スキャベンジは失敗できない。
    std::fprintf(stderr, "ao: GC: stale reference into the nursery at %p\n", obj.heapPointer());
    std::abort();
  }
  auto flags = static_cast<std::uint16_t>(h->flags | kFlagOld);
  std::byte* dest = heap_->reserveOld(n);
  if (dest == nullptr) {
    // old が上限: to-space に残す。from-space の生存物は同じ大きさの to-space に必ず入る。
    dest = heap_->reserveToSpace(n);
    if (dest == nullptr) {
      std::fprintf(stderr, "ao: GC: to-space overflow during scavenge (stale reference?)\n");
      std::abort();
    }
    flags = static_cast<std::uint16_t>(h->flags & ~kFlagOld);
    spilled_ = true;
  }
  std::memcpy(dest, h, n);
  auto* nh = reinterpret_cast<ObjectHeader*>(dest);
  nh->flags = static_cast<std::uint16_t>(flags & ~kFlagForwarded);
  Oop nw = Oop::fromHeap(dest);
  h->flags = static_cast<std::uint16_t>(h->flags | kFlagForwarded);
  h->klass = nw;
  return nw;
}

void Gc::collectNursery() {
  spilled_ = false;
  scavengeFromRoots();
  clearWeakAfterNursery();
  heap_->flipNursery();
  heap_->poisonFreed(heap_->toStart_, heap_->toEnd_);
  // full GC は閾値を超えたときだけ。old が上限で生存物を to-space に残したときは、前回の
  // full GC から old が増えていれば回収を試す（増えていなければ回収できるものはない）。
  const std::size_t used = heap_->oldUsed();
  if (used > heap_->oldThreshold_ || (spilled_ && used > heap_->oldLive_)) {
    collectOld();
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
        if (slot == nullptr) {
          return;
        }
        *slot = c->gc->copy(*slot);
        if (slot->isHeap()) {
          c->stack->push_back(*slot);
        }
      },
      &ctx);

  // old と to-space のコピーをたどる。どちらもスキャベンジ中は動かないので、h は最後まで有効。
  while (!stack.empty()) {
    Oop obj = stack.back();
    stack.pop_back();
    if (!obj.isHeap() ||
        (!heap_->inOld(obj) && !heap_->containsNurseryTo(obj.heapPointer()))) {
      continue;
    }
    auto key = reinterpret_cast<std::uintptr_t>(obj.heapPointer());
    if (!visited.insert(key).second) {
      continue;
    }
    ObjectHeader* h = heap_->header(obj);
    h->klass = copy(h->klass);
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
      if (slots[i].isHeap()) {
        stack.push_back(slots[i]);
      }
    }
  }
}

void Gc::clearWeakAfterNursery() {
  // from-space を指す弱スロット: 転送済みなら転送先、そうでなければ死んでいるので nil。
  auto fixWeak = [&](std::byte* scan, std::byte* end) {
    while (scan < end) {
      auto* h = reinterpret_cast<ObjectHeader*>(scan);
      const std::size_t n = heap_->objectBytes(h);
      if ((h->flags & kFlagWeak) != 0 && (h->flags & kFlagBytes) == 0) {
        auto* slots = reinterpret_cast<Oop*>(h + 1);
        for (std::uint32_t i = 0; i < h->size; ++i) {
          Oop s = slots[i];
          if (!s.isHeap() || !heap_->containsNurseryFrom(s.heapPointer())) {
            continue;
          }
          ObjectHeader* ch = heap_->header(s);
          slots[i] = (ch->flags & kFlagForwarded) ? ch->klass : Oop::nil();
        }
      }
      scan += n;
    }
  };
  fixWeak(heap_->oldStart_, heap_->oldBump_);
  fixWeak(heap_->toStart_, heap_->toBump_);  // old に入らず to-space に残った弱オブジェクト
}

void Gc::clearWeakAfterOldMark() {
  auto clearUnmarkedOldSlots = [&](ObjectHeader* h) {
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
  };

  std::byte* scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if ((h->flags & kFlagMarked) && (h->flags & kFlagWeak) &&
        (h->flags & kFlagBytes) == 0) {
      clearUnmarkedOldSlots(h);
    }
    scan += n;
  }

  scan = heap_->fromStart_;
  while (scan < heap_->fromBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if ((h->flags & kFlagWeak) != 0 && (h->flags & kFlagBytes) == 0) {
      clearUnmarkedOldSlots(h);
    }
    scan += n;
  }
}

void Gc::collectOld() {
  ++heap_->oldCollections_;

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

  std::vector<std::pair<std::byte*, std::byte*>> pins;
  std::byte* scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if ((h->flags & kFlagMarked) && (h->flags & kFlagImmovable)) {
      pins.emplace_back(scan, scan + n);
    }
    scan += n;
  }

  auto overlapsPin = [&](std::byte* p, std::size_t n) {
    std::byte* end = p + n;
    for (auto [a, b] : pins) {
      if (p < b && end > a) {
        return b;
      }
    }
    return static_cast<std::byte*>(nullptr);
  };

  std::unordered_map<std::uintptr_t, Oop> fwd;
  std::vector<std::byte*> objs;
  std::vector<std::pair<std::byte*, std::byte*>> live;
  std::byte* dest = heap_->oldStart_;
  scan = heap_->oldStart_;
  while (scan < heap_->oldBump_) {
    auto* h = reinterpret_cast<ObjectHeader*>(scan);
    const std::size_t n = heap_->objectBytes(h);
    if (h->flags & kFlagMarked) {
      objs.push_back(scan);
      if (h->flags & kFlagImmovable) {
        fwd.emplace(reinterpret_cast<std::uintptr_t>(h), Oop::fromHeap(scan));
        live.emplace_back(scan, scan + n);
        if (dest < scan + static_cast<std::ptrdiff_t>(n)) {
          dest = scan + n;
        }
      } else {
        while (true) {
          if (auto pinEnd = overlapsPin(dest, n)) {
            dest = pinEnd;
            continue;
          }
          std::byte* nextPin = nullptr;
          std::byte* nextPinEnd = nullptr;
          for (auto [a, b] : pins) {
            if (b > dest) {
              nextPin = a;
              nextPinEnd = b;
              break;
            }
          }
          if (nextPin != nullptr && dest + n < nextPin) {
            const std::size_t rem =
                static_cast<std::size_t>(nextPin - (dest + n));
            if (rem > 0 && rem < sizeof(ObjectHeader)) {
              dest = nextPinEnd;
              continue;
            }
          }
          break;
        }
        fwd.emplace(reinterpret_cast<std::uintptr_t>(h), Oop::fromHeap(dest));
        live.emplace_back(dest, dest + n);
        dest += n;
      }
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

  // update は冪等でない（転送先が別の object の旧番地と重なる）。同じスロットが二重に登録されて
  // いても、書き換えはスロットごとに 1 回だけにする。
  struct UpdateCtx {
    decltype(update)* fn;
    std::unordered_set<Oop*> seen;
  } upd{&update, {}};
  roots_->visitAll(
      [](void* ctx, Oop* slot) {
        auto* u = static_cast<UpdateCtx*>(ctx);
        if (slot != nullptr && u->seen.insert(slot).second) {
          *slot = (*u->fn)(*slot);
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

  std::byte* usedEnd = heap_->oldStart_;
  auto moveOne = [&](std::byte* p) {
    auto* h = reinterpret_cast<ObjectHeader*>(p);
    const std::size_t n = heap_->objectBytes(h);
    auto it = fwd.find(reinterpret_cast<std::uintptr_t>(h));
    std::byte* target = static_cast<std::byte*>(it->second.heapPointer());
    if (target != p) {
      std::memmove(target, p, n);
    }
    auto* nh = reinterpret_cast<ObjectHeader*>(target);
    nh->flags = static_cast<std::uint16_t>(nh->flags & ~kFlagMarked);
    std::byte* objEnd = target + n;
    if (objEnd > usedEnd) {
      usedEnd = objEnd;
    }
  };

  for (auto* p : objs) {
    moveOne(p);
  }

  std::sort(live.begin(), live.end());
  std::byte* cursor = heap_->oldStart_;
  for (auto [a, b] : live) {
    if (a > cursor) {
      // ストレス時は穴全体を毒で埋め、klass 語を毒のまま残す。回収済み object を指す古い Oop が
      // header() で止まる。old space の走査は size と flags しか読まない。
      heap_->poisonFreed(cursor, a);
      const std::size_t rem = static_cast<std::size_t>(a - cursor);
      if (rem >= sizeof(ObjectHeader)) {
        auto* fh = reinterpret_cast<ObjectHeader*>(cursor);
        if (heap_->gcStress() == 0) {
          fh->klass = Oop::nil();
        }
        fh->size = static_cast<std::uint32_t>(rem - sizeof(ObjectHeader));
        fh->flags = kFlagBytes | kFlagOld;
        fh->hash = 0;
      }
    }
    if (b > cursor) {
      cursor = b;
    }
  }
  std::byte* const freedEnd = heap_->oldBump_;
  heap_->oldBump_ = usedEnd;
  heap_->poisonFreed(usedEnd, freedEnd);

  const std::size_t liveBytes = heap_->oldUsed();
  heap_->oldLive_ = liveBytes;
  heap_->oldThreshold_ = std::clamp(2 * liveBytes, heap_->oldInitial_, heap_->oldMax_);
}

}  // namespace ao

#include "ao/ClassPool.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Format.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Lookup.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Natives.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace ao {
namespace ClassPool {
namespace {

// The Kernel Dictionary's slots (SPEC §3.6: tally array). array holds key/value pairs; a nil key
// is an empty pair.
constexpr std::uint32_t kPoolSlotTally = 0;
constexpr std::uint32_t kPoolSlotArray = 1;
constexpr std::uint32_t kPoolMinCapacity = 8;

bool isPointers(const Heap& heap, Oop obj, std::uint32_t size) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0 && heap.size(obj) >= size;
}

bool isBytes(const Heap& heap, Oop obj) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) != 0;
}

// pool's pair array, or the empty Oop when pool is no pool.
Oop pairsOf(const Heap& heap, Oop pool) {
  if (!isPointers(heap, pool, kPoolSlotArray + 1)) {
    return Oop{};
  }
  const Oop pairs = heap.slotAt(pool, kPoolSlotArray);
  return isPointers(heap, pairs, 0) ? pairs : Oop{};
}

std::string keyText(Heap& heap, Oop key) {
  return std::string(reinterpret_cast<const char*>(heap.bytes(key)), heap.size(key));
}

bool keyNames(Heap& heap, Oop key, std::string_view name) {
  if (!isBytes(heap, key) || heap.size(key) != name.size()) {
    return false;
  }
  return name.empty() || std::memcmp(heap.bytes(key), name.data(), name.size()) == 0;
}

std::uint32_t instSizeOf(const Heap& heap, Oop cls) {
  if (!isClassShaped(heap, cls)) {
    return 0;
  }
  return static_cast<std::uint32_t>(Format::instSize(heap.slotAt(cls, kClassSlotFormat)));
}

}  // namespace

Oop make(CallContext& ctx, const std::vector<std::string>& names) {
  std::vector<std::string_view> unique;
  for (const std::string& name : names) {
    if (std::find(unique.begin(), unique.end(), name) == unique.end()) {
      unique.emplace_back(name);
    }
  }
  const auto count = static_cast<std::uint32_t>(unique.size());
  const std::uint32_t dictSize = instSizeOf(ctx.heap, ctx.wk.dictionaryClass);
  const std::uint32_t assocSize = instSizeOf(ctx.heap, ctx.wk.associationClass);
  if (dictSize <= kPoolSlotArray || assocSize <= kAssocValue) {
    return Oop{};
  }
  Root pool(ctx.roots, allocateRetry(ctx, ctx.wk.dictionaryClass, dictSize, 0));
  if (!pool.slot.isHeap()) {
    return Oop{};
  }
  const Oop pairs = allocateRetry(ctx, ctx.wk.arrayClass, std::max(kPoolMinCapacity, 2 * count), 0);
  if (!pairs.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(pool.slot, kPoolSlotTally, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(pool.slot, kPoolSlotArray, pairs);
  for (std::uint32_t i = 0; i < count; ++i) {
    // The pool is rooted. The binding and its Symbol go in right after intern, which does not
    // collect, so neither raw Oop crosses a collection.
    const Oop binding = allocateRetry(ctx, ctx.wk.associationClass, assocSize, 0);
    if (!binding.isHeap()) {
      return Oop{};
    }
    const Oop sym = ctx.wk.intern(unique[i]);
    if (!sym.isHeap()) {
      return Oop{};
    }
    ctx.heap.slotAtPut(binding, kAssocKey, sym);
    ctx.heap.slotAtPut(binding, kAssocValue, Oop::nil());
    const Oop at = ctx.heap.slotAt(pool.slot, kPoolSlotArray);
    ctx.heap.slotAtPut(at, 2 * i, sym);
    ctx.heap.slotAtPut(at, 2 * i + 1, binding);
    ctx.heap.slotAtPut(pool.slot, kPoolSlotTally, Oop::fromSmallInteger(i + 1));
  }
  return pool.slot;
}

std::vector<std::string> names(Heap& heap, Oop pool) {
  std::vector<std::string> out;
  const Oop pairs = pairsOf(heap, pool);
  if (!pairs.isHeap()) {
    return out;
  }
  const std::uint32_t n = heap.size(pairs);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = heap.slotAt(pairs, i);
    if (isBytes(heap, key)) {
      out.push_back(keyText(heap, key));
    }
  }
  return out;
}

Oop bindingAt(Heap& heap, Oop pool, std::string_view name) {
  const Oop pairs = pairsOf(heap, pool);
  if (!pairs.isHeap()) {
    return Oop{};
  }
  const std::uint32_t n = heap.size(pairs);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (keyNames(heap, heap.slotAt(pairs, i), name)) {
      return heap.slotAt(pairs, i + 1);
    }
  }
  return Oop{};
}

void adopt(Heap& heap, Oop pool, Oop from) {
  const Oop pairs = pairsOf(heap, pool);
  if (!pairs.isHeap()) {
    return;
  }
  const std::uint32_t n = heap.size(pairs);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = heap.slotAt(pairs, i);
    if (!isBytes(heap, key)) {
      continue;
    }
    // Whatever from holds for the name, a binding or an entry that is no longer one, so a name
    // whose entry was replaced in place stays that way instead of starting over at nil.
    const Oop kept = bindingAt(heap, from, keyText(heap, key));
    if (!kept.isEmpty()) {
      heap.slotAtPut(pairs, i + 1, kept);
    }
  }
}

Oop owner(const Heap& heap, const WellKnown& wk, Oop cls) {
  if (isClassShaped(heap, cls) && heap.klass(cls) == wk.metaclassClass) {
    const Oop thisClass = heap.slotAt(cls, kClassSlotThisClass);
    if (isClassShaped(heap, thisClass)) {
      return thisClass;
    }
  }
  return cls;
}

std::vector<std::string> visibleNames(Heap& heap, const WellKnown& wk, Oop cls) {
  std::vector<std::string> out;
  SuperclassWalk walk(heap, owner(heap, wk, cls));
  for (Oop c; walk.next(c);) {
    for (std::string& name : names(heap, heap.slotAt(c, kClassSlotClassPool))) {
      if (std::find(out.begin(), out.end(), name) == out.end()) {
        out.push_back(std::move(name));
      }
    }
  }
  return out;
}

Oop visibleBinding(Heap& heap, const WellKnown& wk, Oop cls, std::string_view name) {
  SuperclassWalk walk(heap, owner(heap, wk, cls));
  for (Oop c; walk.next(c);) {
    const Oop binding = bindingAt(heap, heap.slotAt(c, kClassSlotClassPool), name);
    if (!binding.isEmpty()) {
      return binding;
    }
  }
  return Oop{};
}

bool isBinding(const Heap& heap, const WellKnown& wk, Oop obj) {
  return isPointers(heap, obj, kAssocValue + 1) && heap.klass(obj) == wk.associationClass;
}

Oop copy(CallContext& ctx, Oop pool) {
  if (!pairsOf(ctx.heap, pool).isHeap()) {
    return pool;
  }
  Root from(ctx.roots, pool);
  const std::uint32_t dictSize = instSizeOf(ctx.heap, ctx.wk.dictionaryClass);
  if (dictSize <= kPoolSlotArray) {
    return Oop{};
  }
  Root to(ctx.roots, allocateRetry(ctx, ctx.wk.dictionaryClass, dictSize, 0));
  if (!to.slot.isHeap()) {
    return Oop{};
  }
  const std::uint32_t n = ctx.heap.size(pairsOf(ctx.heap, from.slot));
  const Oop pairs = allocateRetry(ctx, ctx.wk.arrayClass, n, 0);
  if (!pairs.isHeap()) {
    return Oop{};
  }
  // Nothing from here on collects, so the raw pair arrays stay where they are.
  const Oop fromPairs = pairsOf(ctx.heap, from.slot);
  for (std::uint32_t i = 0; i < n; ++i) {
    ctx.heap.slotAtPut(pairs, i, ctx.heap.slotAt(fromPairs, i));
  }
  ctx.heap.slotAtPut(to.slot, kPoolSlotTally, ctx.heap.slotAt(from.slot, kPoolSlotTally));
  ctx.heap.slotAtPut(to.slot, kPoolSlotArray, pairs);
  return to.slot;
}

}  // namespace ClassPool
}  // namespace ao

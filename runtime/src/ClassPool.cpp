#include "ao/ClassPool.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Format.hpp"
#include "ao/HandleScope.hpp"
#include "ao/HashedCollection.hpp"
#include "ao/Lookup.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace ao {
namespace ClassPool {
namespace {

// SPEC §3.6: the classPool is a Kernel Dictionary, an entry `key value hash` per name.
constexpr std::uint32_t kWidth = Hashed::kDictionaryWidth;

bool isPointers(const Heap& heap, Oop obj, std::uint32_t size) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0 && heap.size(obj) >= size;
}

bool isBytes(const Heap& heap, Oop obj) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) != 0;
}

// pool's table. False when pool is no pool (nil for a Kernel class), an empty table or a damaged
// one: it then binds no name.
bool tableOf(const Heap& heap, Oop pool, Hashed::Table* t) {
  return Hashed::read(heap, pool, kWidth, t) == Hashed::Shape::Table;
}

// SPEC §3.6: the hash a name's key saves, the one String>>hash and Symbol>>hash answer for its
// bytes. The runtime computes it; it sends nothing.
std::int64_t nameHash(std::string_view name) {
  return bytesValueHash(reinterpret_cast<const unsigned char*>(name.data()), name.size());
}

std::string keyText(Heap& heap, Oop key) {
  return std::string(reinterpret_cast<const char*>(heap.bytes(key)), heap.size(key));
}

// SPEC §3.6: a classPool's names are its Symbol keys. A key of any other kind (a String, a
// LargeInteger, a ByteArray, a Float put with at:put:) names nothing.
bool isName(const Heap& heap, const WellKnown& wk, Oop key) {
  return isBytes(heap, key) && heap.klass(key) == wk.symbolClass;
}

bool keyNames(Heap& heap, const WellKnown& wk, Oop key, std::string_view name) {
  if (!isName(heap, wk, key) || heap.size(key) != name.size()) {
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
  if (dictSize <= Hashed::kSlotArray || assocSize <= kAssocValue) {
    return Oop{};
  }
  Root pool(ctx.roots, allocateRetry(ctx, ctx.wk.dictionaryClass, dictSize, 0));
  if (!pool.slot.isHeap()) {
    return Oop{};
  }
  // Large enough that the names go in without growing (SPEC §3.6: at most 3/4 full).
  const std::uint64_t capacity = Hashed::capacityFor(count);
  const Oop array = Hashed::newArray(ctx, capacity, kWidth);
  if (!array.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(pool.slot, Hashed::kSlotTally, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(pool.slot, Hashed::kSlotArray, array);
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
    const Oop at = ctx.heap.slotAt(pool.slot, Hashed::kSlotArray);
    const std::int64_t hash = nameHash(unique[i]);
    const std::uint32_t entry =
        Hashed::freeEntry(ctx.heap, at, static_cast<std::uint32_t>(capacity), kWidth, hash);
    Hashed::putEntry(ctx.heap, at, kWidth, entry, sym, binding, hash);
    ctx.heap.slotAtPut(pool.slot, Hashed::kSlotTally, Oop::fromSmallInteger(i + 1));
  }
  return pool.slot;
}

std::vector<std::string> names(Heap& heap, const WellKnown& wk, Oop pool) {
  std::vector<std::string> out;
  Hashed::Table t;
  if (!tableOf(heap, pool, &t)) {
    return out;
  }
  for (std::uint32_t i = 0; i < t.capacity; ++i) {
    const Oop key = heap.slotAt(t.array, i * kWidth + Hashed::kEntryKey);
    if (isName(heap, wk, key)) {
      out.push_back(keyText(heap, key));
    }
  }
  // SPEC §3.6: a hash table keeps no order, so the names go in byte order (std::string compares
  // its chars as unsigned).
  std::sort(out.begin(), out.end());
  return out;
}

Oop bindingAt(Heap& heap, const WellKnown& wk, Oop pool, std::string_view name) {
  Hashed::Table t;
  if (!tableOf(heap, pool, &t)) {
    return Oop{};
  }
  // The probe the Dictionary natives make for the name's Symbol, with the bytes compared here
  // instead of sending =. Only a Symbol key is a name (SPEC §3.6).
  const std::int64_t hash = nameHash(name);
  const std::uint32_t mask = t.capacity - 1;
  std::uint32_t i = Hashed::home(hash, t.capacity);
  for (std::uint32_t n = 0; n < t.capacity; ++n, i = (i + 1) & mask) {
    const Oop key = heap.slotAt(t.array, i * kWidth + Hashed::kEntryKey);
    if (key.isNil()) {
      break;
    }
    if (Hashed::savedHashIs(heap, t.array, kWidth, i, hash) && keyNames(heap, wk, key, name)) {
      return heap.slotAt(t.array, i * kWidth + Hashed::kEntryValue);
    }
  }
  return Oop{};
}

void adopt(Heap& heap, const WellKnown& wk, Oop pool, Oop from) {
  Hashed::Table t;
  if (!tableOf(heap, pool, &t)) {
    return;
  }
  for (std::uint32_t i = 0; i < t.capacity; ++i) {
    const Oop key = heap.slotAt(t.array, i * kWidth + Hashed::kEntryKey);
    if (!isName(heap, wk, key)) {
      continue;
    }
    // Whatever from holds for the name, a binding or an entry that is no longer one, so a name
    // whose entry was replaced in place stays that way instead of starting over at nil.
    const Oop kept = bindingAt(heap, wk, from, keyText(heap, key));
    if (!kept.isEmpty()) {
      heap.slotAtPut(t.array, i * kWidth + Hashed::kEntryValue, kept);
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
    for (std::string& name : names(heap, wk, heap.slotAt(c, kClassSlotClassPool))) {
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
    const Oop binding = bindingAt(heap, wk, heap.slotAt(c, kClassSlotClassPool), name);
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
  // SPEC §3.6: a slot without the Dictionary's tally and array (nil for a Kernel class) is
  // answered as it is.
  if (!isPointers(ctx.heap, pool, Hashed::kSlotArray + 1)) {
    return pool;
  }
  Hashed::Table t;
  const Hashed::Shape shape = Hashed::read(ctx.heap, pool, kWidth, &t);
  if (shape == Hashed::Shape::Damaged) {
    return abortEvaluation(ctx, "damaged hashed collection");
  }
  Root from(ctx.roots, pool);
  const std::uint32_t dictSize = instSizeOf(ctx.heap, ctx.wk.dictionaryClass);
  if (dictSize <= Hashed::kSlotArray) {
    return Oop{};
  }
  Root to(ctx.roots, allocateRetry(ctx, ctx.wk.dictionaryClass, dictSize, 0));
  if (!to.slot.isHeap()) {
    return Oop{};
  }
  if (shape == Hashed::Shape::Empty) {
    // A nil array: a new empty Dictionary, as Dictionary new makes it, never the pool itself.
    return Hashed::grow(ctx, to, kWidth) ? to.slot : Oop{};
  }
  const std::uint32_t n = ctx.heap.size(ctx.heap.slotAt(from.slot, Hashed::kSlotArray));
  const Oop array = allocateRetry(ctx, ctx.wk.arrayClass, n, 0);
  if (!array.isHeap()) {
    return Oop{};
  }
  // SPEC §3.6: the tally and the array's contents as they are, so the copy's entries sit where
  // the pool's do. Nothing from here on collects, so the raw arrays stay where they are.
  const Oop fromArray = ctx.heap.slotAt(from.slot, Hashed::kSlotArray);
  for (std::uint32_t i = 0; i < n; ++i) {
    ctx.heap.slotAtPut(array, i, ctx.heap.slotAt(fromArray, i));
  }
  ctx.heap.slotAtPut(to.slot, Hashed::kSlotTally, ctx.heap.slotAt(from.slot, Hashed::kSlotTally));
  ctx.heap.slotAtPut(to.slot, Hashed::kSlotArray, array);
  return to.slot;
}

}  // namespace ClassPool
}  // namespace ao

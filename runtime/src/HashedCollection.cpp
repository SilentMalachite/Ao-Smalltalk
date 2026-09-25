#include "ao/HashedCollection.hpp"

#include "ao/HandleScope.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/WellKnown.hpp"

#include <bit>

namespace ao {
namespace Hashed {
namespace {

// 2^64 / φ, rounded to odd (SPEC §3.6 ホーム).
constexpr std::uint64_t kHomeMultiplier = 0x9E3779B97F4A7C15ULL;

bool isPowerOfTwo(std::uint64_t n) { return n != 0 && (n & (n - 1)) == 0; }

bool isPointers(const Heap& heap, Oop obj) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0;
}

void copyEntry(Heap& heap, Oop from, std::uint32_t fromIndex, Oop to, std::uint32_t toIndex,
               std::uint32_t width) {
  for (std::uint32_t s = 0; s < width; ++s) {
    heap.slotAtPut(to, toIndex * width + s, heap.slotAt(from, fromIndex * width + s));
  }
}

}  // namespace

Shape read(const Heap& heap, Oop coll, std::uint32_t width, Table* out) {
  if (!isPointers(heap, coll) || heap.size(coll) <= kSlotArray || width == 0) {
    return Shape::Damaged;
  }
  const Oop array = heap.slotAt(coll, kSlotArray);
  if (array.isNil()) {
    return Shape::Empty;
  }
  if (!isPointers(heap, array)) {
    return Shape::Damaged;
  }
  const std::uint32_t slots = heap.size(array);
  const std::uint32_t capacity = slots / width;
  if (slots % width != 0 || capacity < kMinCapacity || !isPowerOfTwo(capacity)) {
    return Shape::Damaged;
  }
  const Oop tally = heap.slotAt(coll, kSlotTally);
  if (!tally.isSmallInteger() || tally.smallIntegerValue() < 0 ||
      tally.smallIntegerValue() > static_cast<std::int64_t>(capacity)) {
    return Shape::Damaged;
  }
  out->array = array;
  out->capacity = capacity;
  out->tally = tally.smallIntegerValue();
  return Shape::Table;
}

std::uint32_t home(std::int64_t hash, std::uint32_t capacity) {
  // SPEC §3.6 ホーム: Fibonacci hashing. The top log2(capacity) bits of the product depend on every
  // bit of hash, so hashes whose low bits agree (i * 4096, identity hashes below 65536 in a table
  // of 131072) still spread over the table. capacity is a power of two >= 8, so the shift is <= 61.
  const auto bits = static_cast<std::uint32_t>(std::countr_zero(capacity));
  return static_cast<std::uint32_t>((static_cast<std::uint64_t>(hash) * kHomeMultiplier) >>
                                    (64 - bits));
}

bool savedHashIs(const Heap& heap, Oop array, std::uint32_t width, std::uint32_t index,
                 std::int64_t hash) {
  const Oop saved = heap.slotAt(array, index * width + width - 1);
  return saved.isSmallInteger() && saved.smallIntegerValue() == hash;
}

std::int64_t savedHash(const Heap& heap, Oop array, std::uint32_t width, std::uint32_t index) {
  const Oop saved = heap.slotAt(array, index * width + width - 1);
  return saved.isSmallInteger() ? saved.smallIntegerValue() : 0;
}

std::uint32_t freeEntry(const Heap& heap, Oop array, std::uint32_t capacity, std::uint32_t width,
                        std::int64_t hash) {
  const std::uint32_t mask = capacity - 1;
  std::uint32_t i = home(hash, capacity);
  for (std::uint32_t n = 0; n < capacity; ++n, i = (i + 1) & mask) {
    if (heap.slotAt(array, i * width + kEntryKey).isNil()) {
      return i;
    }
  }
  return kNoEntry;
}

void putEntry(Heap& heap, Oop array, std::uint32_t width, std::uint32_t index, Oop key, Oop value,
              std::int64_t hash) {
  heap.slotAtPut(array, index * width + kEntryKey, key);
  if (width == kDictionaryWidth) {
    heap.slotAtPut(array, index * width + kEntryValue, value);
  }
  heap.slotAtPut(array, index * width + width - 1, Oop::fromSmallInteger(hash));
}

void removeEntry(Heap& heap, Oop array, std::uint32_t capacity, std::uint32_t width,
                 std::uint32_t index) {
  const std::uint32_t mask = capacity - 1;
  std::uint32_t hole = index;
  // Until a free entry, or until j comes back around to the hole: a table whose tally lies may have
  // no free entry at all (SPEC §3.6). Each move brings an entry nearer its home, so this ends.
  for (std::uint32_t j = (index + 1) & mask; j != hole; j = (j + 1) & mask) {
    if (heap.slotAt(array, j * width + kEntryKey).isNil()) {
      break;
    }
    const std::uint32_t h = home(savedHash(heap, array, width, j), capacity);
    // The entry stays when its home lies cyclically in (hole, j]: the probe from there reaches it
    // before the hole.
    const bool stays = hole <= j ? (hole < h && h <= j) : (hole < h || h <= j);
    if (!stays) {
      copyEntry(heap, array, j, array, hole, width);
      hole = j;
    }
  }
  for (std::uint32_t s = 0; s < width; ++s) {
    heap.slotAtPut(array, hole * width + s, Oop::nil());
  }
}

std::uint64_t capacityFor(std::uint64_t count) {
  std::uint64_t capacity = kMinCapacity;
  while (count * 4 > capacity * 3) {
    capacity *= 2;
  }
  return capacity;
}

bool mustGrow(const Table& t) {
  return (t.tally + 1) * 4 > static_cast<std::int64_t>(t.capacity) * 3;
}

Oop newArray(CallContext& ctx, std::uint64_t capacity, std::uint32_t width) {
  const std::uint64_t slots = capacity * width;
  if (slots > UINT32_MAX) {
    ctx.heap.setOutOfMemory();
    return Oop{};
  }
  return allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(slots), 0);
}

bool grow(CallContext& ctx, Root& coll, std::uint32_t width) {
  Table t;
  const Shape shape = read(ctx.heap, coll.slot, width, &t);
  if (shape == Shape::Damaged) {
    return false;
  }
  Root old(ctx.roots, shape == Shape::Table ? t.array : Oop::nil());
  const std::uint64_t capacity =
      shape == Shape::Table ? std::uint64_t{t.capacity} * 2 : std::uint64_t{kMinCapacity};
  const Oop fresh = newArray(ctx, capacity, width);
  if (!fresh.isHeap()) {
    return false;
  }
  // Nothing below collects, so fresh stays where it is. No user code ran either: old is still
  // coll's array.
  std::int64_t count = 0;
  if (old.slot.isHeap()) {
    const std::uint32_t oldCapacity = ctx.heap.size(old.slot) / width;
    for (std::uint32_t i = 0; i < oldCapacity; ++i) {
      if (ctx.heap.slotAt(old.slot, i * width + kEntryKey).isNil()) {
        continue;
      }
      const std::uint32_t j =
          freeEntry(ctx.heap, fresh, static_cast<std::uint32_t>(capacity), width,
                    savedHash(ctx.heap, old.slot, width, i));
      copyEntry(ctx.heap, old.slot, i, fresh, j, width);
      ++count;
    }
  }
  ctx.heap.slotAtPut(coll.slot, kSlotArray, fresh);
  ctx.heap.slotAtPut(coll.slot, kSlotTally, Oop::fromSmallInteger(count));
  return true;
}

}  // namespace Hashed
}  // namespace ao

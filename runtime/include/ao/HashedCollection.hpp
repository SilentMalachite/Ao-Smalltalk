#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"

#include <cstdint>

namespace ao {

struct CallContext;
struct Root;

// SPEC §3.6 Collections (Dictionary と Set): the Kernel Dictionary, IdentityDictionary, Set and
// IdentitySet are open-addressed hash tables with linear probing. The natives
// (kernel/Dictionary.cpp) and the classPool (ClassPool.cpp) read and write their slots only
// through this namespace, so the layout lives in one place. Nothing here sends a message.
namespace Hashed {

// The instance slots (SPEC §3.6: tally array).
inline constexpr std::uint32_t kSlotTally = 0;
inline constexpr std::uint32_t kSlotArray = 1;
// An entry of array is `key value hash` in a Dictionary and `element hash` in a Set. hash is the
// key's hash saved as a SmallInteger, in the entry's last slot. A nil key is a free entry.
inline constexpr std::uint32_t kDictionaryWidth = 3;
inline constexpr std::uint32_t kSetWidth = 2;
inline constexpr std::uint32_t kEntryKey = 0;
inline constexpr std::uint32_t kEntryValue = 1;  // a Dictionary's
// The capacity (entries) is a power of two, at least kMinCapacity.
inline constexpr std::uint32_t kMinCapacity = 8;
inline constexpr std::uint32_t kNoEntry = UINT32_MAX;

// What read finds in a collection's slots.
enum class Shape {
  Empty,    // array is nil: an empty table (Dictionary basicNew)
  Table,    // tally and array fit the layout
  Damaged,  // they do not (instVarAt:put:): the natives fail
};

struct Table {
  Oop array{};
  std::uint32_t capacity = 0;
  std::int64_t tally = 0;
};

// Reads coll's tally and array for entries width slots wide. Table only when array is a pointer
// object of capacity * width slots, capacity a power of two >= kMinCapacity, and tally a
// SmallInteger in [0, capacity]. Does not allocate.
Shape read(const Heap& heap, Oop coll, std::uint32_t width, Table* out);
// The entry the probe for hash starts at: hash bitAnd: capacity - 1.
std::uint32_t home(std::int64_t hash, std::uint32_t capacity);
// Whether entry index saved hash. An entry whose saved hash is no SmallInteger matches no hash.
bool savedHashIs(const Heap& heap, Oop array, std::uint32_t width, std::uint32_t index,
                 std::int64_t hash);
// The hash saved in entry index, 0 when it is no SmallInteger (SPEC §3.6: growth and removal
// place such an entry as if its hash were 0).
std::int64_t savedHash(const Heap& heap, Oop array, std::uint32_t width, std::uint32_t index);
// The first free entry on hash's probe, kNoEntry when every entry is taken.
std::uint32_t freeEntry(const Heap& heap, Oop array, std::uint32_t capacity, std::uint32_t width,
                        std::int64_t hash);
// Fills entry index with key, value (a Dictionary's; ignored for a Set) and hash, which must fit a
// SmallInteger.
void putEntry(Heap& heap, Oop array, std::uint32_t width, std::uint32_t index, Oop key, Oop value,
              std::int64_t hash);
// Frees entry index by backward shift (SPEC §3.6): each later entry of the run up to a free entry
// (or, in a table without one, around to the hole) moves into the hole unless its home lies after
// the hole and not after the entry, and its old place becomes the hole. Leaves the tally alone.
void removeEntry(Heap& heap, Oop array, std::uint32_t capacity, std::uint32_t width,
                 std::uint32_t index);
// The smallest capacity that holds count entries without growing (count * 4 <= capacity * 3).
std::uint64_t capacityFor(std::uint64_t count);
// Whether putting one more entry in t goes past 3/4 of its capacity, so t grows first.
bool mustGrow(const Table& t);
// A new array of capacity free entries. May GC. Empty Oop when it cannot be allocated (the heap's
// out-of-memory flag is then set).
Oop newArray(CallContext& ctx, std::uint64_t capacity, std::uint32_t width);
// Gives coll an array of twice its capacity (kMinCapacity when coll is an empty table) holding
// the same entries, placed again by their saved hashes, and sets the tally to their number. Sends
// nothing. May GC: coll is a Root. False when the array cannot be allocated or coll is damaged.
bool grow(CallContext& ctx, Root& coll, std::uint32_t width);

}  // namespace Hashed
}  // namespace ao

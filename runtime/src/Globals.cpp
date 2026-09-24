#include "ao/Globals.hpp"

#include <cstdint>

namespace ao {
namespace Globals {

static constexpr const char* kNames[kSmalltalkCount] = {
    "Object",
    "Behavior",
    "ClassDescription",
    "Class",
    "Metaclass",
    "UndefinedObject",
    "Boolean",
    "True",
    "False",
    "Magnitude",
    "Number",
    "Integer",
    "SmallInteger",
    "LargePositiveInteger",
    "LargeNegativeInteger",
    "Float",
    "Fraction",
    "Character",
    "Collection",
    "SequenceableCollection",
    "ArrayedCollection",
    "Array",
    "ByteArray",
    "String",
    "Symbol",
    "Interval",
    "Dictionary",
    "IdentityDictionary",
    "Set",
    "IdentitySet",
    "OrderedCollection",
    "Association",
    "Bag",
    "LinkedList",
    "MappedCollection",
    "CompiledMethod",
    "NativeMethod",
    "Message",
    "MethodDictionary",
    "MethodContext",
    "BlockContext",
    "Process",
    "ProcessorScheduler",
    "Processor",
    "Semaphore",
    "SharedQueue",
    "Point",
    "Rectangle",
    "Stream",
    "PositionableStream",
    "ReadStream",
    "WriteStream",
    "ReadWriteStream",
    "Transcript",
    "SmalltalkImage",
    "Date",
    "Time",
};

static_assert(sizeof(kNames) / sizeof(kNames[0]) == kSmalltalkCount);

const char* nameAt(std::uint32_t index) {
  if (index >= kSmalltalkCount) {
    return nullptr;
  }
  return kNames[index];
}

namespace {

// Pairs Smalltalk starts with: the kSmalltalkCount names, Smalltalk, and room for the first classes.
constexpr std::uint32_t kFirstPairs = 64;

// Binds key to value in dict, which has Smalltalk's shape. A full array doubles; a new pair takes
// the first empty one, so the pairs stay in the order they were bound. The tally is recounted.
// Does not GC.
bool bindIn(WellKnown& wk, Oop dict, Oop key, Oop value) {
  if (!key.isHeap()) {
    return false;
  }
  Heap& heap = wk.heap();
  Oop pairs = heap.slotAt(dict, kSmalltalkSlotArray);
  const auto n = heap.size(pairs);
  std::uint32_t empty = UINT32_MAX;
  std::int64_t bound = 0;
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop k = heap.slotAt(pairs, i);
    if (k == key) {
      heap.slotAtPut(pairs, i + 1, value);
      return true;
    }
    if (!k.isNil()) {
      ++bound;
    } else if (empty == UINT32_MAX) {
      empty = i;
    }
  }
  if (empty == UINT32_MAX) {
    const Oop grown = heap.allocateNoGc(wk.arrayClass, n < 2 ? 2 * kFirstPairs : n * 2, 0);
    if (!grown.isHeap()) {
      heap.setOutOfMemory();
      return false;
    }
    for (std::uint32_t i = 0; i < n; ++i) {
      heap.slotAtPut(grown, i, heap.slotAt(pairs, i));
    }
    heap.slotAtPut(dict, kSmalltalkSlotArray, grown);
    pairs = grown;
    empty = n - n % 2;
  }
  heap.slotAtPut(pairs, empty, key);
  heap.slotAtPut(pairs, empty + 1, value);
  heap.slotAtPut(dict, kSmalltalkSlotTally, Oop::fromSmallInteger(bound + 1));
  return true;
}

}  // namespace

void install(Heap& heap, Roots& /*roots*/, WellKnown& wk) {
  // Nothing here collects (allocateNoGc and intern), so dict and pairs stay where they are. Until
  // wk.smalltalk is set, wk.named answers the well-known slots (SPEC §3.7 step 7).
  const Oop dict = heap.allocateNoGc(wk.smalltalkImageClass, kSmalltalkSlotCount, 0);
  const Oop pairs = dict.isHeap() ? heap.allocateNoGc(wk.arrayClass, 2 * kFirstPairs, 0) : Oop{};
  if (!pairs.isHeap()) {
    heap.setOutOfMemory();
    return;
  }
  heap.slotAtPut(dict, kSmalltalkSlotTally, Oop::fromSmallInteger(0));
  heap.slotAtPut(dict, kSmalltalkSlotArray, pairs);
  for (std::uint32_t i = 0; i < kSmalltalkCount; ++i) {
    if (!bindIn(wk, dict, wk.intern(kNames[i]), wk.named(kNames[i]))) {
      return;
    }
  }
  if (!bindIn(wk, dict, wk.intern("Smalltalk"), dict)) {
    return;
  }
  wk.smalltalk = dict;
}

bool isDictionary(const WellKnown& wk, Oop obj) {
  const Heap& heap = wk.heap();
  if (!obj.isHeap() || heap.klass(obj) != wk.smalltalkImageClass ||
      (heap.flags(obj) & kFlagBytes) != 0 || heap.size(obj) != kSmalltalkSlotCount) {
    return false;
  }
  const Oop pairs = heap.slotAt(obj, kSmalltalkSlotArray);
  return heap.slotAt(obj, kSmalltalkSlotTally).isSmallInteger() && pairs.isHeap() &&
         heap.klass(pairs) == wk.arrayClass && (heap.flags(pairs) & kFlagBytes) == 0;
}

Oop lookup(const WellKnown& wk, Oop key) {
  if (!key.isHeap() || !isDictionary(wk, wk.smalltalk)) {
    return Oop{};
  }
  const Heap& heap = wk.heap();
  const Oop pairs = heap.slotAt(wk.smalltalk, kSmalltalkSlotArray);
  const auto n = heap.size(pairs);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(pairs, i) == key) {
      return heap.slotAt(pairs, i + 1);
    }
  }
  return Oop{};
}

bool bind(WellKnown& wk, Oop key, Oop value) {
  return isDictionary(wk, wk.smalltalk) && bindIn(wk, wk.smalltalk, key, value);
}

void each(const WellKnown& wk, void (*fn)(void* baton, Oop key, Oop value), void* baton) {
  if (fn == nullptr || !isDictionary(wk, wk.smalltalk)) {
    return;
  }
  const Heap& heap = wk.heap();
  const Oop pairs = heap.slotAt(wk.smalltalk, kSmalltalkSlotArray);
  const auto n = heap.size(pairs);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = heap.slotAt(pairs, i);
    if (!key.isNil()) {
      fn(baton, key, heap.slotAt(pairs, i + 1));
    }
  }
}

Oop at(const WellKnown& wk, std::string_view name) {
  const Oop key = wk.findSymbol(name);
  const Oop value = key.isHeap() ? lookup(wk, key) : Oop{};
  return value.isEmpty() ? Oop::nil() : value;
}

}  // namespace Globals
}  // namespace ao

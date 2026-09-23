#include "ao/MethodDictionary.hpp"

namespace ao {
namespace MethodDictionary {

Oop create(Heap& heap, WellKnown& wk, std::uint32_t capacity) {
  auto dict = heap.allocateNoGc(wk.methodDictionaryClass, 2, 0);
  if (!dict.isHeap()) {
    return Oop{};
  }
  heap.slotAtPut(dict, kDictSlotTally, Oop::fromSmallInteger(0));
  auto inner = heap.allocateNoGc(Oop::nil(), capacity * 2, 0);
  if (!inner.isHeap()) {
    return Oop{};
  }
  heap.slotAtPut(dict, kDictSlotArray, inner);
  return dict;
}

Oop at(const Heap& heap, Oop dict, Oop key) {
  if (!dict.isHeap()) {
    return Oop::nil();
  }
  const auto inner = heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap()) {
    return Oop::nil();
  }
  const auto n = heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i) == key) {
      return heap.slotAt(inner, i + 1);
    }
  }
  return Oop::nil();
}

static bool growInner(Heap& heap, Oop dict, Oop& inner) {
  const auto n = heap.size(inner);
  const auto next = n == 0 ? 2u : n * 2;
  auto grown = heap.allocateNoGc(Oop::nil(), next, 0);
  if (!grown.isHeap()) {
    return false;
  }
  for (std::uint32_t i = 0; i < n; ++i) {
    heap.slotAtPut(grown, i, heap.slotAt(inner, i));
  }
  heap.slotAtPut(dict, kDictSlotArray, grown);
  inner = grown;
  return true;
}

bool atPut(Heap& heap, Oop dict, Oop key, Oop value) {
  if (!dict.isHeap()) {
    return false;
  }
  auto inner = heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap()) {
    return false;
  }
  auto n = heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i) == key) {
      heap.slotAtPut(inner, i + 1, value);
      return true;
    }
  }
  const auto tallyOop = heap.slotAt(dict, kDictSlotTally);
  const auto tally = tallyOop.isSmallInteger() ? tallyOop.smallIntegerValue() : 0;
  if (tally * 2 == static_cast<std::int64_t>(n)) {
    if (!growInner(heap, dict, inner)) {
      return false;
    }
    n = heap.size(inner);
  }
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i).isNil()) {
      heap.slotAtPut(inner, i, key);
      heap.slotAtPut(inner, i + 1, value);
      heap.slotAtPut(dict, kDictSlotTally, Oop::fromSmallInteger(tally + 1));
      return true;
    }
  }
  return false;
}

}  // namespace MethodDictionary
}  // namespace ao

#include "ao/MethodDictionary.hpp"

namespace ao {
namespace MethodDictionary {

Oop create(Heap& heap, WellKnown& wk, std::uint32_t capacity) {
  // GC しないので、失敗するのは old が上限のとき。out of memory のフラグを立てる（SPEC §3.2）。
  auto dict = heap.allocateNoGc(wk.methodDictionaryClass, 2, 0);
  if (!dict.isHeap()) {
    heap.setOutOfMemory();
    return Oop{};
  }
  heap.slotAtPut(dict, kDictSlotTally, Oop::fromSmallInteger(0));
  auto inner = heap.allocateNoGc(Oop::nil(), capacity * 2, 0);
  if (!inner.isHeap()) {
    heap.setOutOfMemory();
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
    heap.setOutOfMemory();  // old が上限
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
  // キーはヒープの Symbol。空 Oop（intern の失敗）と nil（空きスロットの印）と即値は登録しない。
  if (!dict.isHeap() || !key.isHeap()) {
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
  // tally は Smalltalk から書き換えられる（クラスの instVarAt: で辞書に届く）。+1 が SmallInteger
  // を超えるなら、登録せずに失敗を返す。
  if (tally >= kSmiMax) {
    return false;
  }
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

bool removeKey(Heap& heap, Oop dict, Oop key) {
  // 辞書の枠は instVarAt:put: で何でも入る。バイトのオブジェクトや、配列の枠まで届かない小さな
  // オブジェクト、配列がバイトのものは辞書でない。範囲の外を読まずに false を返す。
  if (!dict.isHeap() || !key.isHeap() || (heap.flags(dict) & kFlagBytes) != 0 ||
      heap.size(dict) <= kDictSlotArray) {
    return false;
  }
  const Oop inner = heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap() || (heap.flags(inner) & kFlagBytes) != 0) {
    return false;
  }
  const auto n = heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i) != key) {
      continue;
    }
    heap.slotAtPut(inner, i, Oop::nil());
    heap.slotAtPut(inner, i + 1, Oop::nil());
    // tally は Smalltalk から書き換えられる（atPut と同じ）。0 未満にはしない。
    const Oop tallyOop = heap.slotAt(dict, kDictSlotTally);
    const auto tally = tallyOop.isSmallInteger() ? tallyOop.smallIntegerValue() : 0;
    heap.slotAtPut(dict, kDictSlotTally, Oop::fromSmallInteger(tally > 0 ? tally - 1 : 0));
    return true;
  }
  return false;
}

}  // namespace MethodDictionary
}  // namespace ao

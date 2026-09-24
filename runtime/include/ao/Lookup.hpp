#pragma once

#include "ao/Bootstrap.hpp"
#include "ao/Heap.hpp"
#include "ao/Oop.hpp"

#include <cstdint>
#include <vector>

namespace ao {

// SPEC §3.3: 親の鎖をたどる段数の上限。実際の階層はこれよりずっと浅い。循環もここで止まる。
inline constexpr std::uint32_t kMaxSuperclassDepth = 1024;

// cls をクラスとして読めるか。ヒープのポインタオブジェクトで、クラスの枠をすべて持つもの。
// 親の枠は instVarAt:put: で何でも入るので、鎖の上のものはこれを確かめてから読む。
inline bool isClassShaped(const Heap& heap, Oop cls) {
  return cls.isHeap() && (heap.flags(cls) & kFlagBytes) == 0 && heap.size(cls) >= kClassSlotCount;
}

// cls の親。cls がクラスの形でなければ nil（鎖の終わり）。
inline Oop superclassOf(const Heap& heap, Oop cls) {
  return isClassShaped(heap, cls) ? heap.slotAt(cls, kClassSlotSuperclass) : Oop::nil();
}

// SPEC §3.3 の鎖を start から親へ 1 段ずつたどる。nil、クラスの形をしていないもの、
// kMaxSuperclassDepth 段目で終わる。循環は段数の上限で止まる（それまで同じクラスを繰り返し返す）。
// 生の Oop を持つので、たどる間に GC を起こさないこと。
//
//   SuperclassWalk walk(heap, start);
//   for (Oop cls; walk.next(cls);) { ... }
class SuperclassWalk {
 public:
  SuperclassWalk(const Heap& heap, Oop start) : heap_(heap), next_(start) {}

  // 次のクラスを cls に入れて true。鎖が終わったら false。
  bool next(Oop& cls) {
    if (depth_ >= kMaxSuperclassDepth || !isClassShaped(heap_, next_)) {
      return false;
    }
    cls = next_;
    next_ = heap_.slotAt(cls, kClassSlotSuperclass);
    ++depth_;
    return true;
  }

 private:
  const Heap& heap_;
  Oop next_;
  std::uint32_t depth_ = 0;
};

// start から始まる鎖に target があるか（isKindOf: の判定）。
bool chainIncludes(const Heap& heap, Oop start, Oop target);

// 鎖のクラスを根（Object）の側から並べる。すでに通ったクラスに戻ったところ（循環）で切る。
std::vector<Oop> superclassChainFromRoot(const Heap& heap, Oop start);

// SPEC §3.6: cls のインスタンスの名前付きスロットの名前を、添字の順に並べる。鎖を根の側からたどり、
// 各クラスは自分の instSize が 1 つ前のクラスより増えた分のスロットに、自分の instVarNames を先頭から
// 当てる。名前の無いスロットは空の Oop である。名前が足りなくなるのは、ユーザーがクラスの
// instVarNames を変えたとき（`instVarAt: 8 put: nil` など）だけである（旧形式のイメージはロードで
// 拒否する）。増えた分より多い名前は使わない。生の Oop を返すので、使い終わるまで GC を起こさないこと。
std::vector<Oop> namedSlotNames(const Heap& heap, Oop cls);

Oop lookup(Heap& heap, Oop startClass, Oop selector);

}  // namespace ao

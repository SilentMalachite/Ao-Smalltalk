#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <cstdint>
#include <unordered_set>

namespace ao {

class Gc {
 public:
  Gc(Heap& heap, Roots& roots);
  // スキャベンジ → 弱スロットの消去 → フリップ → 必要なら collectOld。失敗しない。
  // 生存物は old へ昇格し、old が上限なら to-space に残る。スキャベンジ中に old は動かない。
  void collectNursery();
  void collectOld();
  // SPEC §3.2 の第 2 契機。allocateRetry が bytes の大きな object を old に直置きする前に呼ぶ。
  // oldUsed + bytes が閾値を超えるならスキャベンジし、それでも超えていて、スキャベンジが full GC を
  // 走らせていなければ full GC を走らせる。
  void collectBeforeTenured(std::size_t bytes);
  void safepoint();
  // GC ストレス時だけ n 回に 1 回ナーサリ GC、その 4 回に 1 回 old GC も走らせる。
  void stressPoint();

 private:
  Oop copy(Oop obj);
  // traced には、スキャベンジがたどった old と to-space の object の番地が入る。
  void scavengeFromRoots(std::unordered_set<std::uintptr_t>& traced);
  void clearWeakAfterNursery(const std::unordered_set<std::uintptr_t>& traced);
  void clearWeakAfterOldMark();
  Heap* heap_;
  Roots* roots_;
};

}  // namespace ao

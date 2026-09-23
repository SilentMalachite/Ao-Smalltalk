#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

namespace ao {

class Gc {
 public:
  Gc(Heap& heap, Roots& roots);
  // スキャベンジ → 弱スロットの消去 → フリップ → 必要なら collectOld。失敗しない。
  // 生存物は old へ昇格し、old が上限なら to-space に残る。スキャベンジ中に old は動かない。
  void collectNursery();
  void collectOld();
  void safepoint();
  // GC ストレス時だけ n 回に 1 回ナーサリ GC、その 4 回に 1 回 old GC も走らせる。
  void stressPoint();

 private:
  Oop copy(Oop obj);
  void scavengeFromRoots();
  void clearWeakAfterNursery();
  void clearWeakAfterOldMark();
  Heap* heap_;
  Roots* roots_;
  bool spilled_ = false;  // このスキャベンジで to-space に残した生存物がある
};

}  // namespace ao

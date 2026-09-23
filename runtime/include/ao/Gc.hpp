#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

namespace ao {

class Gc {
 public:
  Gc(Heap& heap, Roots& roots);
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
  bool oldCompacted_ = false;
  bool failed_ = false;
};

}  // namespace ao

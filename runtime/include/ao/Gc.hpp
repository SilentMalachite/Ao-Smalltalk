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

 private:
  Oop copy(Oop obj);
  void scavengeFromRoots();
  Heap* heap_;
  Roots* roots_;
  bool oldCompacted_ = false;
  bool failed_ = false;
};

}  // namespace ao

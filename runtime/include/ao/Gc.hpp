#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"

#include <vector>

namespace ao {

class Gc {
 public:
  explicit Gc(Heap& heap);
  void addRoot(Oop* slot);
  void removeRoot(Oop* slot);
  void collectNursery();
  void collectOld();

 private:
  Oop copy(Oop obj);
  Heap* heap_;
  std::vector<Oop*> roots_;
  bool oldCompacted_ = false;
};

}  // namespace ao

#include "ao/Lookup.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/MethodDictionary.hpp"

namespace ao {

Oop lookup(Heap& heap, Oop startClass, Oop selector) {
  Oop cls = startClass;
  while (cls.isHeap()) {
    const auto dict = heap.slotAt(cls, kClassSlotMethodDict);
    if (dict.isHeap()) {
      const auto meth = MethodDictionary::at(heap, dict, selector);
      if (!meth.isNil()) {
        return meth;
      }
    }
    cls = heap.slotAt(cls, kClassSlotSuperclass);
  }
  return Oop::nil();
}

}  // namespace ao

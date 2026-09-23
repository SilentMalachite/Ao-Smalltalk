#include "ao/Lookup.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/MethodDictionary.hpp"

#include <algorithm>

namespace ao {

bool chainIncludes(const Heap& heap, Oop start, Oop target) {
  SuperclassWalk walk(heap, start);
  for (Oop cls; walk.next(cls);) {
    if (cls == target) {
      return true;
    }
  }
  return false;
}

std::vector<Oop> superclassChainFromRoot(const Heap& heap, Oop start) {
  std::vector<Oop> chain;
  SuperclassWalk walk(heap, start);
  for (Oop cls; walk.next(cls);) {
    if (std::find(chain.begin(), chain.end(), cls) != chain.end()) {
      break;
    }
    chain.push_back(cls);
  }
  std::reverse(chain.begin(), chain.end());
  return chain;
}

Oop lookup(Heap& heap, Oop startClass, Oop selector) {
  SuperclassWalk walk(heap, startClass);
  for (Oop cls; walk.next(cls);) {
    const auto dict = heap.slotAt(cls, kClassSlotMethodDict);
    if (dict.isHeap()) {
      const auto meth = MethodDictionary::at(heap, dict, selector);
      if (!meth.isNil()) {
        return meth;
      }
    }
  }
  return Oop::nil();
}

}  // namespace ao

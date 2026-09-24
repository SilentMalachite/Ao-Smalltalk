#include "ao/Lookup.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Format.hpp"
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

std::vector<Oop> namedSlotNames(const Heap& heap, Oop cls) {
  std::vector<Oop> names;
  for (const Oop c : superclassChainFromRoot(heap, cls)) {
    const auto size = static_cast<std::size_t>(Format::instSize(heap.slotAt(c, kClassSlotFormat)));
    // A class smaller than the one before it keeps only the slots it has.
    if (names.size() > size) {
      names.resize(size);
    }
    const Oop own = heap.slotAt(c, kClassSlotInstVarNames);
    const std::size_t ownCount =
        own.isHeap() && (heap.flags(own) & kFlagBytes) == 0 ? heap.size(own) : 0;
    for (std::size_t i = 0; names.size() < size; ++i) {
      const Oop name = i < ownCount ? heap.slotAt(own, static_cast<std::uint32_t>(i)) : Oop{};
      names.push_back(name.isHeap() && (heap.flags(name) & kFlagBytes) != 0 ? name : Oop{});
    }
  }
  return names;
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

#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <string_view>

namespace ao {

class WellKnown {
 public:
  WellKnown(Heap& heap, Roots& roots);

  Heap& heap() const { return *heap_; }

  Oop nil() const { return Oop::nil(); }
  Oop true_() const { return Oop::true_(); }
  Oop false_() const { return Oop::false_(); }

  Oop named(std::string_view name) const;
  Oop classOf(Oop obj) const;

  Oop objectClass{};
  Oop objectMetaclass{};
  Oop behaviorClass{};
  Oop behaviorMetaclass{};
  Oop classDescriptionClass{};
  Oop classDescriptionMetaclass{};
  Oop classClass{};
  Oop classMetaclass{};
  Oop metaclassClass{};
  Oop metaclassMetaclass{};
  Oop undefinedObjectClass{};
  Oop undefinedObjectMetaclass{};
  Oop booleanClass{};
  Oop booleanMetaclass{};
  Oop trueClass{};
  Oop trueMetaclass{};
  Oop falseClass{};
  Oop falseMetaclass{};
  Oop smallIntegerClass{};
  Oop smallIntegerMetaclass{};
  Oop characterClass{};
  Oop characterMetaclass{};
  Oop smalltalk{};

 private:
  void addRoots(Roots& roots);
  Heap* heap_;
};

}  // namespace ao

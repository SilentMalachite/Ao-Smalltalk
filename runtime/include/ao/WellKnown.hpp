#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <memory>
#include <string_view>

namespace ao {

class WellKnown {
 public:
  WellKnown(Heap& heap, Roots& roots);
  ~WellKnown();
  WellKnown(const WellKnown&) = delete;
  WellKnown& operator=(const WellKnown&) = delete;
  WellKnown(WellKnown&&) = delete;
  WellKnown& operator=(WellKnown&&) = delete;

  Heap& heap() const { return *heap_; }

  Oop nil() const { return Oop::nil(); }
  Oop true_() const { return Oop::true_(); }
  Oop false_() const { return Oop::false_(); }

  Oop named(std::string_view name) const;
  Oop classOf(Oop obj) const;
  Oop intern(std::string_view utf8);

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
  Oop symbolClass{};
  Oop symbolMetaclass{};
  Oop methodDictionaryClass{};
  Oop methodDictionaryMetaclass{};
  Oop nativeMethodClass{};
  Oop nativeMethodMetaclass{};
  Oop messageClass{};
  Oop messageMetaclass{};
  Oop smalltalk{};

 private:
  void addRoots(Roots& roots);
  struct InternTable;
  Heap* heap_;
  Roots* roots_ = nullptr;
  std::unique_ptr<InternTable> intern_;
};

}  // namespace ao

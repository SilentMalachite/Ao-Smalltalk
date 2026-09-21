#include "ao/WellKnown.hpp"

#include <cstring>
#include <deque>
#include <string>
#include <unordered_map>

namespace ao {

struct WellKnown::InternTable {
  std::deque<Oop> table;
  std::unordered_map<std::string, std::size_t> byBytes;
};

WellKnown::WellKnown(Heap& heap, Roots& roots)
    : heap_(&heap), roots_(&roots), intern_(std::make_unique<InternTable>()) {
  objectClass = Oop::nil();
  objectMetaclass = Oop::nil();
  behaviorClass = Oop::nil();
  behaviorMetaclass = Oop::nil();
  classDescriptionClass = Oop::nil();
  classDescriptionMetaclass = Oop::nil();
  classClass = Oop::nil();
  classMetaclass = Oop::nil();
  metaclassClass = Oop::nil();
  metaclassMetaclass = Oop::nil();
  undefinedObjectClass = Oop::nil();
  undefinedObjectMetaclass = Oop::nil();
  booleanClass = Oop::nil();
  booleanMetaclass = Oop::nil();
  trueClass = Oop::nil();
  trueMetaclass = Oop::nil();
  falseClass = Oop::nil();
  falseMetaclass = Oop::nil();
  smallIntegerClass = Oop::nil();
  smallIntegerMetaclass = Oop::nil();
  characterClass = Oop::nil();
  characterMetaclass = Oop::nil();
  symbolClass = Oop::nil();
  symbolMetaclass = Oop::nil();
  methodDictionaryClass = Oop::nil();
  methodDictionaryMetaclass = Oop::nil();
  nativeMethodClass = Oop::nil();
  nativeMethodMetaclass = Oop::nil();
  messageClass = Oop::nil();
  messageMetaclass = Oop::nil();
  smalltalk = Oop::nil();
  addRoots(roots);
}

WellKnown::~WellKnown() = default;

void WellKnown::addRoots(Roots& roots) {
  roots.add(&objectClass);
  roots.add(&objectMetaclass);
  roots.add(&behaviorClass);
  roots.add(&behaviorMetaclass);
  roots.add(&classDescriptionClass);
  roots.add(&classDescriptionMetaclass);
  roots.add(&classClass);
  roots.add(&classMetaclass);
  roots.add(&metaclassClass);
  roots.add(&metaclassMetaclass);
  roots.add(&undefinedObjectClass);
  roots.add(&undefinedObjectMetaclass);
  roots.add(&booleanClass);
  roots.add(&booleanMetaclass);
  roots.add(&trueClass);
  roots.add(&trueMetaclass);
  roots.add(&falseClass);
  roots.add(&falseMetaclass);
  roots.add(&smallIntegerClass);
  roots.add(&smallIntegerMetaclass);
  roots.add(&characterClass);
  roots.add(&characterMetaclass);
  roots.add(&symbolClass);
  roots.add(&symbolMetaclass);
  roots.add(&methodDictionaryClass);
  roots.add(&methodDictionaryMetaclass);
  roots.add(&nativeMethodClass);
  roots.add(&nativeMethodMetaclass);
  roots.add(&messageClass);
  roots.add(&messageMetaclass);
  roots.add(&smalltalk);
}

Oop WellKnown::named(std::string_view name) const {
  if (name == "nil") return nil();
  if (name == "true") return true_();
  if (name == "false") return false_();
  if (name == "Object") return objectClass;
  if (name == "Behavior") return behaviorClass;
  if (name == "ClassDescription") return classDescriptionClass;
  if (name == "Class") return classClass;
  if (name == "Metaclass") return metaclassClass;
  if (name == "UndefinedObject") return undefinedObjectClass;
  if (name == "Boolean") return booleanClass;
  if (name == "True") return trueClass;
  if (name == "False") return falseClass;
  if (name == "SmallInteger") return smallIntegerClass;
  if (name == "Character") return characterClass;
  if (name == "Symbol") return symbolClass;
  if (name == "MethodDictionary") return methodDictionaryClass;
  if (name == "NativeMethod") return nativeMethodClass;
  if (name == "Message") return messageClass;
  if (name == "Smalltalk") return smalltalk;
  return Oop::nil();
}

Oop WellKnown::classOf(Oop obj) const {
  if (obj.isNil()) return undefinedObjectClass;
  if (obj.isTrue()) return trueClass;
  if (obj.isFalse()) return falseClass;
  if (obj.isSmallInteger()) return smallIntegerClass;
  if (obj.isCharacter()) return characterClass;
  if (obj.isHeap()) return heap_->klass(obj);
  return Oop::nil();
}

Oop WellKnown::intern(std::string_view utf8) {
  std::string key(utf8);
  auto it = intern_->byBytes.find(key);
  if (it != intern_->byBytes.end()) {
    return intern_->table[it->second];
  }
  const auto n = static_cast<std::uint32_t>(utf8.size());
  Oop sym = heap_->allocate(symbolClass, n, kFlagBytes);
  if (!sym.isHeap()) {
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(heap_->bytes(sym), utf8.data(), n);
  }
  intern_->table.push_back(sym);
  roots_->add(&intern_->table.back());
  intern_->byBytes.emplace(std::move(key), intern_->table.size() - 1);
  return intern_->table.back();
}

}  // namespace ao

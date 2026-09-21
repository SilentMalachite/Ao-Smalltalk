#include "ao/WellKnown.hpp"

namespace ao {

WellKnown::WellKnown(Heap& heap, Roots& roots) : heap_(&heap) {
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
  smalltalk = Oop::nil();
  addRoots(roots);
}

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

}  // namespace ao

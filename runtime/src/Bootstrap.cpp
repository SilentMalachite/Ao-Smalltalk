#include "ao/Bootstrap.hpp"

#include <cstring>

namespace ao {
namespace Bootstrap {

static Oop allocClass(Heap& heap) {
  return heap.allocate(Oop::nil(), kClassSlotCount, 0);
}

static Oop makeName(Heap& heap, const char* s) {
  const auto n = static_cast<std::uint32_t>(std::strlen(s));
  auto bytes = heap.allocate(Oop::nil(), n, kFlagBytes);
  if (bytes.isHeap()) {
    std::memcpy(heap.bytes(bytes), s, n);
  }
  return bytes;
}

static void wireClass(Heap& heap, Oop cls, Oop meta, Oop superCls, Oop thisClass,
                      std::int64_t instSize, const char* name) {
  heap.header(cls)->klass = meta;
  heap.slotAtPut(cls, kClassSlotSuperclass, superCls);
  heap.slotAtPut(cls, kClassSlotMethodDict, Oop::nil());
  heap.slotAtPut(cls, kClassSlotFormat, Oop::fromSmallInteger(instSize));
  heap.slotAtPut(cls, kClassSlotName, makeName(heap, name));
  heap.slotAtPut(cls, kClassSlotThisClass, thisClass);
}

void allocateSkeletons(Heap& heap, Roots& /*roots*/, WellKnown& wk) {
  wk.objectClass = allocClass(heap);
  wk.objectMetaclass = allocClass(heap);
  wk.behaviorClass = allocClass(heap);
  wk.behaviorMetaclass = allocClass(heap);
  wk.classDescriptionClass = allocClass(heap);
  wk.classDescriptionMetaclass = allocClass(heap);
  wk.classClass = allocClass(heap);
  wk.classMetaclass = allocClass(heap);
  wk.metaclassClass = allocClass(heap);
  wk.metaclassMetaclass = allocClass(heap);
  wk.undefinedObjectClass = allocClass(heap);
  wk.undefinedObjectMetaclass = allocClass(heap);
  wk.booleanClass = allocClass(heap);
  wk.booleanMetaclass = allocClass(heap);
  wk.trueClass = allocClass(heap);
  wk.trueMetaclass = allocClass(heap);
  wk.falseClass = allocClass(heap);
  wk.falseMetaclass = allocClass(heap);
  wk.smallIntegerClass = allocClass(heap);
  wk.smallIntegerMetaclass = allocClass(heap);
  wk.characterClass = allocClass(heap);
  wk.characterMetaclass = allocClass(heap);
}

void wireCycle(Heap& heap, WellKnown& wk) {
  const auto five = static_cast<std::int64_t>(kClassSlotCount);

  wireClass(heap, wk.objectClass, wk.objectMetaclass, Oop::nil(), Oop::nil(), 0, "Object");
  wireClass(heap, wk.behaviorClass, wk.behaviorMetaclass, wk.objectClass, Oop::nil(), five,
            "Behavior");
  wireClass(heap, wk.classDescriptionClass, wk.classDescriptionMetaclass, wk.behaviorClass,
            Oop::nil(), five, "ClassDescription");
  wireClass(heap, wk.classClass, wk.classMetaclass, wk.classDescriptionClass, Oop::nil(), five,
            "Class");
  wireClass(heap, wk.metaclassClass, wk.metaclassMetaclass, wk.classDescriptionClass, Oop::nil(),
            five, "Metaclass");
  wireClass(heap, wk.undefinedObjectClass, wk.undefinedObjectMetaclass, wk.objectClass, Oop::nil(),
            0, "UndefinedObject");
  wireClass(heap, wk.booleanClass, wk.booleanMetaclass, wk.objectClass, Oop::nil(), 0, "Boolean");
  wireClass(heap, wk.trueClass, wk.trueMetaclass, wk.booleanClass, Oop::nil(), 0, "True");
  wireClass(heap, wk.falseClass, wk.falseMetaclass, wk.booleanClass, Oop::nil(), 0, "False");
  wireClass(heap, wk.smallIntegerClass, wk.smallIntegerMetaclass, wk.objectClass, Oop::nil(), 0,
            "SmallInteger");
  wireClass(heap, wk.characterClass, wk.characterMetaclass, wk.objectClass, Oop::nil(), 0,
            "Character");

  wireClass(heap, wk.objectMetaclass, wk.metaclassClass, wk.classClass, wk.objectClass, five,
            "Object class");
  wireClass(heap, wk.behaviorMetaclass, wk.metaclassClass, wk.objectMetaclass, wk.behaviorClass, five,
            "Behavior class");
  wireClass(heap, wk.classDescriptionMetaclass, wk.metaclassClass, wk.behaviorMetaclass,
            wk.classDescriptionClass, five, "ClassDescription class");
  wireClass(heap, wk.classMetaclass, wk.metaclassClass, wk.classDescriptionMetaclass, wk.classClass,
            five, "Class class");
  wireClass(heap, wk.metaclassMetaclass, wk.metaclassClass, wk.classDescriptionMetaclass,
            wk.metaclassClass, five, "Metaclass class");
  wireClass(heap, wk.undefinedObjectMetaclass, wk.metaclassClass, wk.objectMetaclass,
            wk.undefinedObjectClass, five, "UndefinedObject class");
  wireClass(heap, wk.booleanMetaclass, wk.metaclassClass, wk.objectMetaclass, wk.booleanClass, five,
            "Boolean class");
  wireClass(heap, wk.trueMetaclass, wk.metaclassClass, wk.booleanMetaclass, wk.trueClass, five,
            "True class");
  wireClass(heap, wk.falseMetaclass, wk.metaclassClass, wk.booleanMetaclass, wk.falseClass, five,
            "False class");
  wireClass(heap, wk.smallIntegerMetaclass, wk.metaclassClass, wk.objectMetaclass,
            wk.smallIntegerClass, five, "SmallInteger class");
  wireClass(heap, wk.characterMetaclass, wk.metaclassClass, wk.objectMetaclass, wk.characterClass,
            five, "Character class");
}

void run(Heap& heap, Roots& roots, WellKnown& wk) {
  allocateSkeletons(heap, roots, wk);
  wireCycle(heap, wk);
}

}  // namespace Bootstrap
}  // namespace ao

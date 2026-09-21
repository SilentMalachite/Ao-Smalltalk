#include "ao/Bootstrap.hpp"
#include "ao/Gc.hpp"
#include "ao/Globals.hpp"
#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <gtest/gtest.h>

#include <type_traits>

static_assert(!std::is_copy_constructible_v<ao::WellKnown>);
static_assert(!std::is_copy_assignable_v<ao::WellKnown>);
static_assert(!std::is_move_constructible_v<ao::WellKnown>);
static_assert(!std::is_move_assignable_v<ao::WellKnown>);

TEST(Bootstrap, WellKnownImmediatesMatchOopTags) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  EXPECT_EQ(ao::Oop::nil(), wk.nil());
  EXPECT_EQ(ao::Oop::true_(), wk.true_());
  EXPECT_EQ(ao::Oop::false_(), wk.false_());
  EXPECT_TRUE(wk.nil().isNil());
  EXPECT_TRUE(wk.true_().isTrue());
  EXPECT_TRUE(wk.false_().isFalse());
}

TEST(Bootstrap, WellKnownNamedImmediates) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  EXPECT_EQ(ao::Oop::nil(), wk.named("nil"));
  EXPECT_EQ(ao::Oop::true_(), wk.named("true"));
  EXPECT_EQ(ao::Oop::false_(), wk.named("false"));
  EXPECT_TRUE(wk.named("Object").isNil());
  EXPECT_TRUE(wk.named("nope").isNil());
}

TEST(Bootstrap, ImmediateClassSlotsReserved) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  EXPECT_TRUE(wk.undefinedObjectClass.isNil());
  EXPECT_TRUE(wk.trueClass.isNil());
  EXPECT_TRUE(wk.falseClass.isNil());
  EXPECT_TRUE(wk.smallIntegerClass.isNil());
  EXPECT_TRUE(wk.characterClass.isNil());
  EXPECT_TRUE(wk.classOf(ao::Oop::nil()).isNil());
}

TEST(Bootstrap, ClassSkeletonsAreHeapAndNamed) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::allocateSkeletons(heap, roots, wk);

  const char* names[] = {
      "Object",          "Behavior", "ClassDescription", "Class",          "Metaclass",
      "UndefinedObject", "Boolean",  "True",             "False",          "SmallInteger",
      "Character",       "Symbol",   "MethodDictionary", "NativeMethod",   "Message"};
  for (const char* n : names) {
    auto cls = wk.named(n);
    ASSERT_TRUE(cls.isHeap()) << n;
    EXPECT_EQ(ao::kClassSlotCount, heap.size(cls)) << n;
    EXPECT_TRUE(heap.klass(cls).isNil()) << n;
    EXPECT_TRUE(heap.slotAt(cls, ao::kClassSlotSuperclass).isNil()) << n;
  }

  EXPECT_TRUE(wk.objectMetaclass.isHeap());
  EXPECT_TRUE(wk.metaclassMetaclass.isHeap());
  EXPECT_EQ(wk.objectClass, wk.named("Object"));
  EXPECT_EQ(5u, heap.size(wk.objectMetaclass));
}

TEST(Bootstrap, SkeletonSlotsStartNil) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::allocateSkeletons(heap, roots, wk);
  EXPECT_TRUE(heap.slotAt(wk.objectClass, ao::kClassSlotMethodDict).isNil());
  EXPECT_TRUE(heap.slotAt(wk.objectClass, ao::kClassSlotFormat).isNil());
  EXPECT_TRUE(heap.slotAt(wk.objectClass, ao::kClassSlotThisClass).isNil());
}

static ao::Oop clsOf(ao::Heap& heap, ao::Oop o) { return heap.klass(o); }

static ao::Oop superOf(ao::Heap& heap, ao::Oop cls) {
  return heap.slotAt(cls, ao::kClassSlotSuperclass);
}

TEST(Bootstrap, CycleObjectClassClassIsMetaclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, clsOf(heap, wk.objectClass)));
}

TEST(Bootstrap, CycleMetaclassClassClassIsMetaclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, clsOf(heap, wk.metaclassClass)));
}

TEST(Bootstrap, CycleObjectSuperclassIsNil) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_TRUE(superOf(heap, wk.objectClass).isNil());
}

TEST(Bootstrap, CycleObjectClassSuperclassIsClass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.classClass, superOf(heap, wk.objectMetaclass));
}

TEST(Bootstrap, CycleEveryClassIsInstanceOfItsMetaclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.objectMetaclass, clsOf(heap, wk.objectClass));
  EXPECT_EQ(wk.behaviorMetaclass, clsOf(heap, wk.behaviorClass));
  EXPECT_EQ(wk.classDescriptionMetaclass, clsOf(heap, wk.classDescriptionClass));
  EXPECT_EQ(wk.classMetaclass, clsOf(heap, wk.classClass));
  EXPECT_EQ(wk.metaclassMetaclass, clsOf(heap, wk.metaclassClass));
}

TEST(Bootstrap, CycleEveryMetaclassIsInstanceOfMetaclass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, wk.objectMetaclass));
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, wk.behaviorMetaclass));
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, wk.classDescriptionMetaclass));
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, wk.classMetaclass));
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, wk.metaclassMetaclass));
}

TEST(Bootstrap, CycleMetaclassHierarchyParallelsClasses) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.objectClass, superOf(heap, wk.behaviorClass));
  EXPECT_EQ(wk.behaviorClass, superOf(heap, wk.classDescriptionClass));
  EXPECT_EQ(wk.classDescriptionClass, superOf(heap, wk.classClass));
  EXPECT_EQ(wk.objectMetaclass, superOf(heap, wk.behaviorMetaclass));
  EXPECT_EQ(wk.behaviorMetaclass, superOf(heap, wk.classDescriptionMetaclass));
  EXPECT_EQ(wk.classDescriptionMetaclass, superOf(heap, wk.classMetaclass));
}

TEST(Bootstrap, CycleMetaclassInheritsFromClassDescription) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.classDescriptionClass, superOf(heap, wk.metaclassClass));
  EXPECT_EQ(wk.classDescriptionMetaclass, superOf(heap, wk.metaclassMetaclass));
}

TEST(Bootstrap, CycleMethodDictIsNilAndFormatIsSmi) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_TRUE(heap.slotAt(wk.objectClass, ao::kClassSlotMethodDict).isNil());
  EXPECT_TRUE(heap.slotAt(wk.objectClass, ao::kClassSlotFormat).isSmallInteger());
  EXPECT_EQ(0, heap.slotAt(wk.objectClass, ao::kClassSlotFormat).smallIntegerValue());
  EXPECT_EQ(static_cast<std::int64_t>(ao::kClassSlotCount),
            heap.slotAt(wk.classClass, ao::kClassSlotFormat).smallIntegerValue());
  EXPECT_EQ(wk.objectClass, heap.slotAt(wk.objectMetaclass, ao::kClassSlotThisClass));
  EXPECT_TRUE(heap.slotAt(wk.objectClass, ao::kClassSlotThisClass).isNil());
}

TEST(Bootstrap, CycleImmediateClassOf) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  EXPECT_EQ(wk.undefinedObjectClass, wk.classOf(ao::Oop::nil()));
  EXPECT_EQ(wk.trueClass, wk.classOf(ao::Oop::true_()));
  EXPECT_EQ(wk.falseClass, wk.classOf(ao::Oop::false_()));
  EXPECT_EQ(wk.smallIntegerClass, wk.classOf(ao::Oop::fromSmallInteger(3)));
  EXPECT_EQ(wk.characterClass, wk.classOf(ao::Oop::fromCharacter(U'A')));
  EXPECT_EQ(wk.objectClass, superOf(heap, wk.undefinedObjectClass));
  EXPECT_EQ(wk.booleanClass, superOf(heap, wk.trueClass));
  EXPECT_EQ(wk.booleanClass, superOf(heap, wk.falseClass));
  EXPECT_EQ(wk.objectClass, superOf(heap, wk.booleanClass));
  EXPECT_EQ(wk.objectClass, superOf(heap, wk.smallIntegerClass));
  EXPECT_EQ(wk.objectClass, superOf(heap, wk.characterClass));
}

TEST(Bootstrap, CycleSurvivesNurseryGc) {
  // Nursery must fit all skeletons + name byte objects; allocate() does not GC.
  ao::Heap heap(1 << 16, 1 << 18);
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ao::Gc gc(heap, roots);
  gc.collectNursery();
  EXPECT_EQ(wk.metaclassClass, clsOf(heap, clsOf(heap, wk.objectClass)));
  EXPECT_TRUE(superOf(heap, wk.objectClass).isNil());
  EXPECT_TRUE(wk.objectClass.isHeap());
}

TEST(Bootstrap, SmalltalkMapsObjectNameToClass) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ASSERT_TRUE(wk.smalltalk.isHeap());
  EXPECT_EQ(15u, heap.size(wk.smalltalk));
  EXPECT_EQ(wk.objectClass, ao::Globals::at(wk, "Object"));
  EXPECT_EQ(wk.objectClass, heap.slotAt(wk.smalltalk, 0));
  EXPECT_EQ(wk.metaclassClass, ao::Globals::at(wk, "Metaclass"));
  EXPECT_EQ(wk.undefinedObjectClass, ao::Globals::at(wk, "UndefinedObject"));
  EXPECT_EQ(wk.smallIntegerClass, ao::Globals::at(wk, "SmallInteger"));
  EXPECT_EQ(wk.smalltalk, wk.named("Smalltalk"));
  EXPECT_TRUE(ao::Globals::at(wk, "nope").isNil());
}

TEST(Bootstrap, SmalltalkContainsAllNamedClasses) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  const char* names[] = {
      "Object",          "Behavior", "ClassDescription", "Class",        "Metaclass",
      "UndefinedObject", "Boolean",  "True",             "False",        "SmallInteger",
      "Character",       "Symbol",   "MethodDictionary", "NativeMethod", "Message"};
  for (const char* n : names) {
    EXPECT_EQ(wk.named(n), ao::Globals::at(wk, n)) << n;
    EXPECT_TRUE(ao::Globals::at(wk, n).isHeap()) << n;
  }
}

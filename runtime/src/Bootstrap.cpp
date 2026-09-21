#include "ao/Bootstrap.hpp"

namespace ao {
namespace Bootstrap {

static Oop allocClass(Heap& heap) {
  return heap.allocate(Oop::nil(), kClassSlotCount, 0);
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

void wireCycle(Heap& /*heap*/, WellKnown& /*wk*/) {}

void run(Heap& heap, Roots& roots, WellKnown& wk) {
  allocateSkeletons(heap, roots, wk);
  wireCycle(heap, wk);
}

}  // namespace Bootstrap
}  // namespace ao

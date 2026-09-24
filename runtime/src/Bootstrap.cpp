#include "ao/Bootstrap.hpp"

#include "ao/Context.hpp"
#include "ao/Format.hpp"
#include "ao/Globals.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Send.hpp"
#include "ao/kernel/Install.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace ao {
namespace Bootstrap {

namespace {

struct ClassDef {
  Oop WellKnown::* cls;
  Oop WellKnown::* meta;
  Oop WellKnown::* superCls;
  std::int64_t instSize;
  bool indexable;
  bool bytes;
  const char* name;
};

constexpr std::int64_t kClassInst = static_cast<std::int64_t>(kClassSlotCount);

constexpr ClassDef kDefs[] = {
    {&WellKnown::objectClass, &WellKnown::objectMetaclass, nullptr, 0, false, false, "Object"},
    {&WellKnown::behaviorClass, &WellKnown::behaviorMetaclass, &WellKnown::objectClass, kClassInst,
     false, false, "Behavior"},
    {&WellKnown::classDescriptionClass, &WellKnown::classDescriptionMetaclass,
     &WellKnown::behaviorClass, kClassInst, false, false, "ClassDescription"},
    {&WellKnown::classClass, &WellKnown::classMetaclass, &WellKnown::classDescriptionClass,
     kClassInst, false, false, "Class"},
    {&WellKnown::metaclassClass, &WellKnown::metaclassMetaclass, &WellKnown::classDescriptionClass,
     kClassInst, false, false, "Metaclass"},
    {&WellKnown::undefinedObjectClass, &WellKnown::undefinedObjectMetaclass, &WellKnown::objectClass,
     0, false, false, "UndefinedObject"},
    {&WellKnown::booleanClass, &WellKnown::booleanMetaclass, &WellKnown::objectClass, 0, false, false,
     "Boolean"},
    {&WellKnown::trueClass, &WellKnown::trueMetaclass, &WellKnown::booleanClass, 0, false, false,
     "True"},
    {&WellKnown::falseClass, &WellKnown::falseMetaclass, &WellKnown::booleanClass, 0, false, false,
     "False"},
    {&WellKnown::magnitudeClass, &WellKnown::magnitudeMetaclass, &WellKnown::objectClass, 0, false,
     false, "Magnitude"},
    {&WellKnown::numberClass, &WellKnown::numberMetaclass, &WellKnown::magnitudeClass, 0, false, false,
     "Number"},
    {&WellKnown::integerClass, &WellKnown::integerMetaclass, &WellKnown::numberClass, 0, false, false,
     "Integer"},
    {&WellKnown::smallIntegerClass, &WellKnown::smallIntegerMetaclass, &WellKnown::integerClass, 0,
     false, false, "SmallInteger"},
    {&WellKnown::largePositiveIntegerClass, &WellKnown::largePositiveIntegerMetaclass,
     &WellKnown::integerClass, 0, true, true, "LargePositiveInteger"},
    {&WellKnown::largeNegativeIntegerClass, &WellKnown::largeNegativeIntegerMetaclass,
     &WellKnown::integerClass, 0, true, true, "LargeNegativeInteger"},
    {&WellKnown::floatClass, &WellKnown::floatMetaclass, &WellKnown::numberClass, 0, true, true,
     "Float"},
    {&WellKnown::fractionClass, &WellKnown::fractionMetaclass, &WellKnown::numberClass, 2, false,
     false, "Fraction"},
    {&WellKnown::characterClass, &WellKnown::characterMetaclass, &WellKnown::magnitudeClass, 0, false,
     false, "Character"},
    {&WellKnown::collectionClass, &WellKnown::collectionMetaclass, &WellKnown::objectClass, 0, false,
     false, "Collection"},
    {&WellKnown::sequenceableCollectionClass, &WellKnown::sequenceableCollectionMetaclass,
     &WellKnown::collectionClass, 0, false, false, "SequenceableCollection"},
    {&WellKnown::arrayedCollectionClass, &WellKnown::arrayedCollectionMetaclass,
     &WellKnown::sequenceableCollectionClass, 0, false, false, "ArrayedCollection"},
    {&WellKnown::arrayClass, &WellKnown::arrayMetaclass, &WellKnown::arrayedCollectionClass, 0, true,
     false, "Array"},
    {&WellKnown::byteArrayClass, &WellKnown::byteArrayMetaclass, &WellKnown::arrayedCollectionClass, 0,
     true, true, "ByteArray"},
    {&WellKnown::stringClass, &WellKnown::stringMetaclass, &WellKnown::arrayedCollectionClass, 0, true,
     true, "String"},
    {&WellKnown::symbolClass, &WellKnown::symbolMetaclass, &WellKnown::stringClass, 0, true, true,
     "Symbol"},
    {&WellKnown::intervalClass, &WellKnown::intervalMetaclass, &WellKnown::sequenceableCollectionClass,
     3, false, false, "Interval"},
    {&WellKnown::dictionaryClass, &WellKnown::dictionaryMetaclass, &WellKnown::collectionClass, 2,
     false, false, "Dictionary"},
    {&WellKnown::identityDictionaryClass, &WellKnown::identityDictionaryMetaclass,
     &WellKnown::dictionaryClass, 2, false, false, "IdentityDictionary"},
    {&WellKnown::setClass, &WellKnown::setMetaclass, &WellKnown::collectionClass, 2, false, false,
     "Set"},
    {&WellKnown::identitySetClass, &WellKnown::identitySetMetaclass, &WellKnown::setClass, 2, false,
     false, "IdentitySet"},
    {&WellKnown::orderedCollectionClass, &WellKnown::orderedCollectionMetaclass,
     &WellKnown::sequenceableCollectionClass, 3, false, false, "OrderedCollection"},
    {&WellKnown::associationClass, &WellKnown::associationMetaclass, &WellKnown::objectClass, 2, false,
     false, "Association"},
    {&WellKnown::bagClass, &WellKnown::bagMetaclass, &WellKnown::collectionClass, 0, false, false,
     "Bag"},
    {&WellKnown::linkedListClass, &WellKnown::linkedListMetaclass,
     &WellKnown::sequenceableCollectionClass, 0, false, false, "LinkedList"},
    {&WellKnown::mappedCollectionClass, &WellKnown::mappedCollectionMetaclass,
     &WellKnown::sequenceableCollectionClass, 0, false, false, "MappedCollection"},
    {&WellKnown::compiledMethodClass, &WellKnown::compiledMethodMetaclass,
     &WellKnown::objectClass, static_cast<std::int64_t>(kCmSlotCount), false, false,
     "CompiledMethod"},
    {&WellKnown::nativeMethodClass, &WellKnown::nativeMethodMetaclass, &WellKnown::objectClass,
     static_cast<std::int64_t>(kNativeSlotCount), false, false, "NativeMethod"},
    {&WellKnown::messageClass, &WellKnown::messageMetaclass, &WellKnown::objectClass, 2, false, false,
     "Message"},
    {&WellKnown::methodDictionaryClass, &WellKnown::methodDictionaryMetaclass, &WellKnown::objectClass,
     2, false, false, "MethodDictionary"},
    {&WellKnown::methodContextClass, &WellKnown::methodContextMetaclass, &WellKnown::objectClass,
     static_cast<std::int64_t>(kMethodContextSlotCount), false, false, "MethodContext"},
    {&WellKnown::blockContextClass, &WellKnown::blockContextMetaclass, &WellKnown::methodContextClass,
     static_cast<std::int64_t>(kBlockSlotCount), false, false, "BlockContext"},
    {&WellKnown::processClass, &WellKnown::processMetaclass, &WellKnown::objectClass, 4, false, false,
     "Process"},
    {&WellKnown::processorSchedulerClass, &WellKnown::processorSchedulerMetaclass,
     &WellKnown::objectClass, 2, false, false, "ProcessorScheduler"},
    {&WellKnown::semaphoreClass, &WellKnown::semaphoreMetaclass, &WellKnown::objectClass, 2, false,
     false, "Semaphore"},
    {&WellKnown::sharedQueueClass, &WellKnown::sharedQueueMetaclass, &WellKnown::objectClass, 3, false,
     false, "SharedQueue"},
    {&WellKnown::pointClass, &WellKnown::pointMetaclass, &WellKnown::objectClass, 2, false, false,
     "Point"},
    {&WellKnown::rectangleClass, &WellKnown::rectangleMetaclass, &WellKnown::objectClass, 2, false,
     false, "Rectangle"},
    {&WellKnown::streamClass, &WellKnown::streamMetaclass, &WellKnown::objectClass, 0, false, false,
     "Stream"},
    {&WellKnown::positionableStreamClass, &WellKnown::positionableStreamMetaclass,
     &WellKnown::streamClass, 3, false, false, "PositionableStream"},
    {&WellKnown::readStreamClass, &WellKnown::readStreamMetaclass, &WellKnown::positionableStreamClass,
     3, false, false, "ReadStream"},
    {&WellKnown::writeStreamClass, &WellKnown::writeStreamMetaclass,
     &WellKnown::positionableStreamClass, 4, false, false, "WriteStream"},
    {&WellKnown::readWriteStreamClass, &WellKnown::readWriteStreamMetaclass,
     &WellKnown::writeStreamClass, 4, false, false, "ReadWriteStream"},
    {&WellKnown::transcriptClass, &WellKnown::transcriptMetaclass, &WellKnown::streamClass, 0, false,
     false, "Transcript"},
    {&WellKnown::smalltalkImageClass, &WellKnown::smalltalkImageMetaclass, &WellKnown::objectClass, 0,
     false, false, "SmalltalkImage"},
    {&WellKnown::dateClass, &WellKnown::dateMetaclass, &WellKnown::magnitudeClass, 0, false, false,
     "Date"},
    {&WellKnown::timeClass, &WellKnown::timeMetaclass, &WellKnown::magnitudeClass, 0, false, false,
     "Time"},
};

// SPEC §3.6: the names of the slots a Kernel class adds to its superclass's, in slot order, as the
// natives use the slots. A class not listed adds none. The test
// Bootstrap.KernelInstVarNamesFillTheSlotsEachClassAdds checks each list against its instSize.
struct SlotNames {
  Oop WellKnown::* cls;
  const char* names;
};

constexpr SlotNames kSlotNames[] = {
    {&WellKnown::behaviorClass,
     "superclass methodDict format name thisClass category classPool instVarNames"},
    {&WellKnown::fractionClass, "numerator denominator"},
    {&WellKnown::intervalClass, "start stop step"},
    {&WellKnown::dictionaryClass, "tally array"},
    {&WellKnown::setClass, "tally array"},
    {&WellKnown::orderedCollectionClass, "array firstIndex lastIndex"},
    {&WellKnown::associationClass, "key value"},
    {&WellKnown::compiledMethodClass, "header literals bytecodes nativeCode selector methodClass"},
    {&WellKnown::nativeMethodClass, "selector argc primitive name methodClass registryIndex"},
    {&WellKnown::messageClass, "selector args"},
    {&WellKnown::methodDictionaryClass, "tally array"},
    {&WellKnown::methodContextClass, "sender pc stackp method receiver argc"},
    {&WellKnown::blockContextClass, "home copied"},
    {&WellKnown::processClass, "nextLink suspendedContext priority myList"},
    {&WellKnown::processorSchedulerClass, "quiescentProcesses activeProcess"},
    {&WellKnown::semaphoreClass, "excessSignals linkedList"},
    {&WellKnown::sharedQueueClass, "contents readSynch writeSynch"},
    {&WellKnown::pointClass, "x y"},
    {&WellKnown::rectangleClass, "origin corner"},
    {&WellKnown::positionableStreamClass, "collection position readLimit"},
    {&WellKnown::writeStreamClass, "writeLimit"},
};

static Oop allocClass(Heap& heap) { return heap.allocate(Oop::nil(), kClassSlotCount, 0); }

static Oop makeName(Heap& heap, const char* s) {
  const auto n = static_cast<std::uint32_t>(std::strlen(s));
  auto bytes = heap.allocate(Oop::nil(), n, kFlagBytes);
  if (bytes.isHeap()) {
    std::memcpy(heap.bytes(bytes), s, n);
  }
  return bytes;
}

static void wireClass(Heap& heap, Oop cls, Oop meta, Oop superCls, Oop thisClass, Oop format,
                      const char* name) {
  heap.header(cls)->klass = meta;
  heap.slotAtPut(cls, kClassSlotSuperclass, superCls);
  heap.slotAtPut(cls, kClassSlotMethodDict, Oop::nil());
  heap.slotAtPut(cls, kClassSlotFormat, format);
  heap.slotAtPut(cls, kClassSlotName, makeName(heap, name));
  heap.slotAtPut(cls, kClassSlotThisClass, thisClass);
}

static Oop superOf(WellKnown& wk, Oop WellKnown::* superCls) {
  return superCls == nullptr ? Oop::nil() : wk.*superCls;
}

static void internHotSelectors(WellKnown& wk) {
  wk.selValue = wk.intern("value");
  wk.selValue_ = wk.intern("value:");
  wk.selNew = wk.intern("new");
  wk.selBasicNew = wk.intern("basicNew");
  wk.selBasicNew_ = wk.intern("basicNew:");
  wk.selSize = wk.intern("size");
  wk.selAt_ = wk.intern("at:");
  wk.selAt_put_ = wk.intern("at:put:");
  wk.selDo_ = wk.intern("do:");
  wk.selError_ = wk.intern("error:");
  wk.selClass = wk.intern("class");
  wk.selIdentityEquals = wk.intern("==");
  wk.internSpecialSelectors();
}

// An Array of the interned Symbols in the space-separated names, or nil when old is full. Does not
// GC, so the raw Oop stays valid.
static Oop makeSlotNames(Heap& heap, WellKnown& wk, std::string_view names) {
  std::vector<std::string_view> parts;
  for (std::size_t i = 0; i < names.size();) {
    const std::size_t end = std::min(names.find(' ', i), names.size());
    if (end > i) {
      parts.push_back(names.substr(i, end - i));
    }
    i = end + 1;
  }
  const Oop arr = heap.allocateNoGc(wk.arrayClass, static_cast<std::uint32_t>(parts.size()), 0);
  if (!arr.isHeap()) {
    return Oop::nil();
  }
  for (std::uint32_t i = 0; i < parts.size(); ++i) {
    const Oop sym = wk.intern(parts[i]);
    if (!sym.isHeap()) {
      return Oop::nil();
    }
    heap.slotAtPut(arr, i, sym);
  }
  return arr;
}

// SPEC §3.7 step 5: after the cycle is wired, so Array and Symbol are classes.
static void nameKernelSlots(Heap& heap, WellKnown& wk) {
  for (const auto& s : kSlotNames) {
    heap.slotAtPut(wk.*(s.cls), kClassSlotInstVarNames, makeSlotNames(heap, wk, s.names));
  }
}

static void ensureMethodDict(Heap& heap, WellKnown& wk, Oop cls) {
  if (!cls.isHeap()) {
    return;
  }
  if (heap.slotAt(cls, kClassSlotMethodDict).isNil()) {
    auto dict = MethodDictionary::create(heap, wk, 8);
    if (dict.isHeap()) {
      heap.slotAtPut(cls, kClassSlotMethodDict, dict);
    }
  }
}

}  // namespace

void allocateSkeletons(Heap& heap, Roots& /*roots*/, WellKnown& wk) {
  for (const auto& d : kDefs) {
    wk.*(d.cls) = allocClass(heap);
    wk.*(d.meta) = allocClass(heap);
  }
}

void wireCycle(Heap& heap, WellKnown& wk) {
  const Oop classFmt = Format::make(kClassInst, false, false);
  for (const auto& d : kDefs) {
    const Oop cls = wk.*(d.cls);
    const Oop meta = wk.*(d.meta);
    const Oop super = superOf(wk, d.superCls);
    wireClass(heap, cls, meta, super, Oop::nil(), Format::make(d.instSize, d.indexable, d.bytes),
              d.name);
  }
  for (const auto& d : kDefs) {
    const Oop cls = wk.*(d.cls);
    const Oop meta = wk.*(d.meta);
    const Oop super = superOf(wk, d.superCls);
    const Oop metaSuper = super.isNil() ? wk.classClass : heap.klass(super);
    char metaName[128];
    std::snprintf(metaName, sizeof(metaName), "%s class", d.name);
    wireClass(heap, meta, wk.metaclassClass, metaSuper, cls, classFmt, metaName);
  }
}

void installNatives(Heap& heap, Roots& roots, WellKnown& wk) {
  internHotSelectors(wk);
  struct Baton {
    Heap* heap;
    WellKnown* wk;
  } baton{&heap, &wk};
  wk.eachClass(
      [](void* p, Oop cls) {
        auto* b = static_cast<Baton*>(p);
        ensureMethodDict(*b->heap, *b->wk, cls);
        if (cls.isHeap()) {
          ensureMethodDict(*b->heap, *b->wk, b->heap->klass(cls));
        }
      },
      &baton);
  kernel::installAll(heap, roots, wk);
  if (wk.processorSchedulerClass.isHeap()) {
    const auto n = Format::instSize(heap.slotAt(wk.processorSchedulerClass, kClassSlotFormat));
    wk.processor = heap.allocate(wk.processorSchedulerClass, static_cast<std::uint32_t>(n), 0);
    if (wk.processor.isHeap()) {
      CallContext ctx{heap, roots, wk, nullptr};
      auto q = send(ctx, wk.orderedCollectionClass, wk.selNew, nullptr, 0, nullptr);
      heap.slotAtPut(wk.processor, 0, q);
      auto proc = send(ctx, wk.processClass, wk.selNew, nullptr, 0, nullptr);
      heap.slotAtPut(wk.processor, 1, proc);
    }
  }
  if (wk.transcriptClass.isHeap()) {
    const auto n = Format::instSize(heap.slotAt(wk.transcriptClass, kClassSlotFormat));
    wk.transcript = heap.allocate(wk.transcriptClass, static_cast<std::uint32_t>(n), 0);
  }
}

void run(Heap& heap, Roots& roots, WellKnown& wk) {
  allocateSkeletons(heap, roots, wk);
  wireCycle(heap, wk);
  nameKernelSlots(heap, wk);
  installNatives(heap, roots, wk);
  Globals::install(heap, roots, wk);
  wk.checkSmallIntegerFastPath();
}

}  // namespace Bootstrap
}  // namespace ao

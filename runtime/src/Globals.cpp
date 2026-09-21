#include "ao/Globals.hpp"

namespace ao {
namespace Globals {

static constexpr const char* kNames[kSmalltalkCount] = {
    "Object",
    "Behavior",
    "ClassDescription",
    "Class",
    "Metaclass",
    "UndefinedObject",
    "Boolean",
    "True",
    "False",
    "Magnitude",
    "Number",
    "Integer",
    "SmallInteger",
    "LargePositiveInteger",
    "LargeNegativeInteger",
    "Float",
    "Fraction",
    "Character",
    "Collection",
    "SequenceableCollection",
    "ArrayedCollection",
    "Array",
    "ByteArray",
    "String",
    "Symbol",
    "Interval",
    "Dictionary",
    "IdentityDictionary",
    "Set",
    "IdentitySet",
    "OrderedCollection",
    "Association",
    "Bag",
    "LinkedList",
    "MappedCollection",
    "CompiledMethod",
    "NativeMethod",
    "Message",
    "MethodDictionary",
    "MethodContext",
    "BlockContext",
    "Process",
    "ProcessorScheduler",
    "Processor",
    "Semaphore",
    "SharedQueue",
    "Point",
    "Rectangle",
    "Stream",
    "PositionableStream",
    "ReadStream",
    "WriteStream",
    "ReadWriteStream",
    "Transcript",
    "SmalltalkImage",
    "Date",
    "Time",
};

static_assert(sizeof(kNames) / sizeof(kNames[0]) == kSmalltalkCount);

void install(Heap& heap, Roots& /*roots*/, WellKnown& wk) {
  auto dict = heap.allocate(Oop::nil(), kSmalltalkCount, 0);
  if (!dict.isHeap()) {
    wk.smalltalk = Oop::nil();
    return;
  }
  for (std::uint32_t i = 0; i < kSmalltalkCount; ++i) {
    heap.slotAtPut(dict, i, wk.named(kNames[i]));
  }
  wk.smalltalk = dict;
}

Oop at(const WellKnown& wk, std::string_view name) {
  if (wk.smalltalk.isHeap()) {
    for (std::uint32_t i = 0; i < kSmalltalkCount; ++i) {
      if (name == kNames[i]) return wk.heap().slotAt(wk.smalltalk, i);
    }
  }
  return wk.named(name);
}

}  // namespace Globals
}  // namespace ao

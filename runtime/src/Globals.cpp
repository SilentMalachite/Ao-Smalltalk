#include "ao/Globals.hpp"

namespace ao {
namespace Globals {

static constexpr const char* kNames[kSmalltalkCount] = {
    "Object",          "Behavior", "ClassDescription", "Class",        "Metaclass",
    "UndefinedObject", "Boolean",  "True",             "False",        "SmallInteger",
    "Character",       "Symbol",   "MethodDictionary", "NativeMethod", "Message"};

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
  if (!wk.smalltalk.isHeap()) return Oop::nil();
  for (std::uint32_t i = 0; i < kSmalltalkCount; ++i) {
    if (name == kNames[i]) return wk.heap().slotAt(wk.smalltalk, i);
  }
  return Oop::nil();
}

}  // namespace Globals
}  // namespace ao

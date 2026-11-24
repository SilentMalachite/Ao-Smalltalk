#include "ao/Symbol.hpp"

#include "ao/Heap.hpp"

namespace ao {
namespace Symbol {

Oop intern(WellKnown& wk, std::string_view utf8) { return wk.intern(utf8); }

std::string_view bytes(const Heap& heap, Oop sym) {
  // A pointer object's bytes are its slots, not text: empty, as for a value that is not on the heap.
  if (!sym.isHeap() || (heap.flags(sym) & kFlagBytes) == 0) {
    return {};
  }
  const ObjectHeader* h = heap.header(sym);
  return std::string_view(reinterpret_cast<const char*>(h + 1), h->size);
}

}  // namespace Symbol
}  // namespace ao

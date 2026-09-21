#include "ao/Symbol.hpp"

#include "ao/Heap.hpp"

namespace ao {
namespace Symbol {

Oop intern(WellKnown& wk, std::string_view utf8) { return wk.intern(utf8); }

std::string_view bytes(const Heap& heap, Oop sym) {
  if (!sym.isHeap()) {
    return {};
  }
  const ObjectHeader* h = heap.header(sym);
  return std::string_view(reinterpret_cast<const char*>(h + 1), h->size);
}

}  // namespace Symbol
}  // namespace ao

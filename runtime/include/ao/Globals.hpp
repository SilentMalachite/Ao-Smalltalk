#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <string_view>

namespace ao {
namespace Globals {

// SPEC §3.6: Smalltalk is a SmalltalkImage whose two slots hold the global dictionary. tally
// counts the pairs; array is an Array of key/value pairs in the order they were bound, the keys
// interned Symbols, a nil key an empty pair.
inline constexpr std::uint32_t kSmalltalkSlotTally = 0;
inline constexpr std::uint32_t kSmalltalkSlotArray = 1;
inline constexpr std::uint32_t kSmalltalkSlotCount = 2;

// The names Bootstrap binds besides Smalltalk (the Kernel classes and Processor). An image records
// their values to check its dictionary against (SPEC §3.11).
inline constexpr std::uint32_t kSmalltalkCount = 57;

const char* nameAt(std::uint32_t index);
// SPEC §3.7 step 7: makes Smalltalk and binds the kSmalltalkCount names and Smalltalk itself. Does
// not GC. Leaves wk.smalltalk nil when old is at its max.
void install(Heap& heap, Roots& roots, WellKnown& wk);
// Whether obj has the shape of Smalltalk: a SmalltalkImage with a SmallInteger tally and an Array.
bool isDictionary(const WellKnown& wk, Oop obj);
// The value bound to key (an interned Symbol), or the empty Oop when Smalltalk binds none.
Oop lookup(const WellKnown& wk, Oop key);
// Binds key (an interned Symbol) to value, in place when key is bound already, else in a new pair.
// Does not GC: a full array grows with allocateNoGc. False when Smalltalk is no dictionary, key is
// no heap object, or old is at its max (the out-of-memory flag is then set).
bool bind(WellKnown& wk, Oop key, Oop value);
// Calls fn with each key and value, in the order they were bound. fn must not GC or bind.
void each(const WellKnown& wk, void (*fn)(void* baton, Oop key, Oop value), void* baton);
// The value bound to name, or nil when Smalltalk binds none.
Oop at(const WellKnown& wk, std::string_view name);

}  // namespace Globals
}  // namespace ao

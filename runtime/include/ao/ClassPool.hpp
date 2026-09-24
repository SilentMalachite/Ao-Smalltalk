#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/WellKnown.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace ao {

struct CallContext;

// SPEC §3.6: a class's classPool (kClassSlotClassPool) is a Kernel Dictionary from each class
// variable's name (an interned Symbol) to its binding, an Association whose key is the name and
// whose value is the variable's value. Only this namespace reads or writes the Dictionary's slots,
// so its layout can change in one place.
namespace ClassPool {

// A new pool with a fresh binding (value nil) for each name, in order. A repeated name gets one
// binding. May GC. Empty Oop when an allocation fails (the out-of-memory flag is then set).
Oop make(CallContext& ctx, const std::vector<std::string>& names);
// The names pool binds, in order. Nothing when pool is no pool (nil for a Kernel class). Does not GC.
std::vector<std::string> names(Heap& heap, Oop pool);
// The binding pool has for name, or the empty Oop. Does not GC.
Oop bindingAt(Heap& heap, Oop pool, std::string_view name);
// For each name pool binds that from has too, puts from's entry in pool in place of its own, so
// methods holding from's binding share the variable with methods compiled for pool. An entry of
// from that is no binding (replaced in place) is put as it is. Does not GC.
void adopt(Heap& heap, Oop pool, Oop from);
// The class whose pools a method of cls sees: cls, or its thisClass when cls is a metaclass.
Oop owner(const Heap& heap, const WellKnown& wk, Oop cls);
// SPEC §3.6 / §3.8: the class variables a method of cls sees, those of owner(cls) and of its
// superclasses, nearest class first and each name once. Does not GC.
std::vector<std::string> visibleNames(Heap& heap, const WellKnown& wk, Oop cls);
// The binding a method of cls finds for name: the entry of the nearest class whose pool has the
// name, which isBinding tells apart from an entry replaced in place; the empty Oop when no class on
// the chain has it. Does not GC.
Oop visibleBinding(Heap& heap, const WellKnown& wk, Oop cls, std::string_view name);
// SPEC §3.6: whether obj is a binding, an Association LitVar can read and write.
bool isBinding(const Heap& heap, const WellKnown& wk, Oop obj);
// SPEC §3.6: what Class>>classPool answers. A new Dictionary with pool's names in pool's order,
// each bound to the same binding, so at:put: on it leaves pool and every binding as they are.
// pool itself when it is no pool (nil for a Kernel class). May GC; empty Oop when an allocation
// fails (the out-of-memory flag is then set).
Oop copy(CallContext& ctx, Oop pool);

}  // namespace ClassPool
}  // namespace ao

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
// whose value is the variable's value. The Dictionary has the hashed layout of every Kernel
// Dictionary (HashedCollection.hpp); the key hash this namespace saves is the one the name's Symbol
// answers to hash, computed without a send.
namespace ClassPool {

// A new pool with a fresh binding (value nil) for each name. A repeated name gets one binding. May
// GC. Empty Oop when an allocation fails (the out-of-memory flag is then set).
Oop make(CallContext& ctx, const std::vector<std::string>& names);
// The names pool binds, in byte order (SPEC §3.6: the table keeps no order). A name is a Symbol
// key; a key of any other kind (put with at:put:) names nothing. Nothing when pool is no pool
// (nil for a Kernel class) or its table is damaged. Does not GC.
std::vector<std::string> names(Heap& heap, const WellKnown& wk, Oop pool);
// The binding pool has for name under its Symbol key, or the empty Oop. Does not GC.
Oop bindingAt(Heap& heap, const WellKnown& wk, Oop pool, std::string_view name);
// For each name pool binds that from has too, puts from's entry in pool in place of its own, so
// methods holding from's binding share the variable with methods compiled for pool. An entry of
// from that is no binding (replaced in place) is put as it is. Does not GC.
void adopt(Heap& heap, const WellKnown& wk, Oop pool, Oop from);
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
// SPEC §3.6: what Class>>classPool answers. A new Dictionary with pool's tally and a copy of its
// array, so each name is bound to the same binding and at:put: on it leaves pool and every binding
// as they are; a new empty Dictionary when pool's array is nil. Aborts with `damaged hashed
// collection` when pool has the Dictionary's slots but a damaged table. pool itself when it has no
// such slots (nil for a Kernel class). May GC; empty Oop when an allocation fails (the
// out-of-memory flag is then set).
Oop copy(CallContext& ctx, Oop pool);

}  // namespace ClassPool
}  // namespace ao

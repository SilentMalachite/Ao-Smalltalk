#pragma once

#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <string_view>

namespace ao {
namespace kernel {

// False when the method cannot be registered (no dictionary, or it cannot grow). Does not GC.
// SPEC §3.3: a method it puts goes through invalidateMethodCache with cache (null: none).
bool putNative(Heap& heap, WellKnown& wk, ClassMethodCache* cache, Oop cls,
               std::string_view selector, std::uint32_t argc, std::string_view name, NativeFn fn);
// For the install functions below. The cache is the one installMissing was given while it runs,
// otherwise none (Bootstrap::run installs before any send could fill a cache).
bool putNative(Heap& heap, WellKnown& wk, Oop cls, std::string_view selector, std::uint32_t argc,
               std::string_view name, NativeFn fn);
void ensureNativeNames();
void installObject(Heap& heap, WellKnown& wk);
void installBoolean(Heap& heap, WellKnown& wk);
void installBehavior(Heap& heap, WellKnown& wk);
void installMagnitude(Heap& heap, WellKnown& wk);
void installInteger(Heap& heap, WellKnown& wk);
void installCharacter(Heap& heap, WellKnown& wk);
void installFloat(Heap& heap, WellKnown& wk);
void installCollection(Heap& heap, WellKnown& wk);
void installArray(Heap& heap, WellKnown& wk);
void installString(Heap& heap, WellKnown& wk);
void installDictionary(Heap& heap, WellKnown& wk);
void installStream(Heap& heap, WellKnown& wk);
void installProcess(Heap& heap, WellKnown& wk);
void installGeometry(Heap& heap, WellKnown& wk);
void installCompiledMethod(Heap& heap, WellKnown& wk);
void installAll(Heap& heap, Roots& roots, WellKnown& wk);
// installAll for a loaded image (SPEC §3.10): adds only the selectors a dictionary lacks and
// leaves every method already there, native or not. cache is the session's (SPEC §3.3): sends
// may have filled it before, and each selector added is dropped from it.
void installMissing(Heap& heap, Roots& roots, WellKnown& wk, ClassMethodCache* cache);

}  // namespace kernel
}  // namespace ao

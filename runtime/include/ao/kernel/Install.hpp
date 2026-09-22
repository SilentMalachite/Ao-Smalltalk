#pragma once

#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <string_view>

namespace ao {
namespace kernel {

void putNative(Heap& heap, WellKnown& wk, Oop cls, std::string_view selector, std::uint32_t argc,
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

}  // namespace kernel
}  // namespace ao

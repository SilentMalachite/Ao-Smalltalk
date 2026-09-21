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
void installObject(Heap& heap, WellKnown& wk);
void installBoolean(Heap& heap, WellKnown& wk);
void installBehavior(Heap& heap, WellKnown& wk);
void installAll(Heap& heap, Roots& roots, WellKnown& wk);

}  // namespace kernel
}  // namespace ao

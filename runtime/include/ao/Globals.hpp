#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <string_view>

namespace ao {
namespace Globals {

inline constexpr std::uint32_t kSmalltalkCount = 57;

const char* nameAt(std::uint32_t index);
void install(Heap& heap, Roots& roots, WellKnown& wk);
Oop at(const WellKnown& wk, std::string_view name);
void atPut(WellKnown& wk, std::string_view name, Oop value);

}  // namespace Globals
}  // namespace ao

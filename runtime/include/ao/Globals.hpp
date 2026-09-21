#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <string_view>

namespace ao {
namespace Globals {

inline constexpr std::uint32_t kSmalltalkCount = 56;

void install(Heap& heap, Roots& roots, WellKnown& wk);
Oop at(const WellKnown& wk, std::string_view name);

}  // namespace Globals
}  // namespace ao

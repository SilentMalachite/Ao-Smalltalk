#pragma once

#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>

namespace ao {

inline constexpr std::uint32_t kClassSlotSuperclass = 0;
inline constexpr std::uint32_t kClassSlotMethodDict = 1;
inline constexpr std::uint32_t kClassSlotFormat     = 2;
inline constexpr std::uint32_t kClassSlotName       = 3;
inline constexpr std::uint32_t kClassSlotThisClass  = 4;
inline constexpr std::uint32_t kClassSlotCount      = 5;

namespace Bootstrap {
void allocateSkeletons(Heap& heap, Roots& roots, WellKnown& wk);
void wireCycle(Heap& heap, WellKnown& wk);
void installNatives(Heap& heap, Roots& roots, WellKnown& wk);
void run(Heap& heap, Roots& roots, WellKnown& wk);
}

}  // namespace ao

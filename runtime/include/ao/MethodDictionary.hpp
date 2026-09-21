#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>

namespace ao {

inline constexpr std::uint32_t kDictSlotTally = 0;
inline constexpr std::uint32_t kDictSlotArray = 1;

namespace MethodDictionary {
Oop create(Heap& heap, WellKnown& wk, std::uint32_t capacity);
Oop at(const Heap& heap, Oop dict, Oop key);
void atPut(Heap& heap, Oop dict, Oop key, Oop value);
}

}  // namespace ao

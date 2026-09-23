#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>

namespace ao {

inline constexpr std::uint32_t kDictSlotTally = 0;
inline constexpr std::uint32_t kDictSlotArray = 1;

// GC しない（allocateNoGc: nursery、満杯なら old）。old も上限なら失敗する。
namespace MethodDictionary {
// Empty Oop when the nursery is full and old is at its max.
Oop create(Heap& heap, WellKnown& wk, std::uint32_t capacity);
Oop at(const Heap& heap, Oop dict, Oop key);
// False when dict is not a dictionary or its array cannot grow (nursery full, old at its max).
bool atPut(Heap& heap, Oop dict, Oop key, Oop value);
}

}  // namespace ao

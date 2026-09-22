#pragma once

#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <string_view>

namespace ao {

struct Image {
  static bool save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path);
  static bool load(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path);
};

}  // namespace ao

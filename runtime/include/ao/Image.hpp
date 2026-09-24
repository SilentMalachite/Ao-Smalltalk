#pragma once

#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <string>
#include <string_view>

namespace ao {

struct Image {
  static bool save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path);
  // False when the image is refused; *reason (when given) then says why (SPEC §3.11).
  static bool load(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr);
};

}  // namespace ao

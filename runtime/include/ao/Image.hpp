#pragma once

#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace ao {

struct Image {
  // False when nothing was written; *reason (when given) then says why (SPEC §3.11).
  static bool save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr);
  // False when the image is refused; *reason (when given) then says why (SPEC §3.11).
  static bool load(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr);
  // SPEC §3.11: every check load makes before it adopts a heap, on the bytes of an image file.
  // save runs it on the bytes it is about to write, so a save that succeeds writes an image this
  // runtime loads. False when a check fails: *reason (when given) is the load's refusal, *detail
  // (when given) which object or class broke which rule.
  static bool check(const Heap& heap, const WellKnown& wk, const std::vector<std::byte>& file,
                    std::size_t oldMaxBytes, std::string* reason, std::string* detail);
};

}  // namespace ao

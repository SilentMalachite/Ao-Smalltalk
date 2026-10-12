#pragma once

#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace ao {

// SPEC §3.10, §3.11 (P18): one pair of the sources section: a method in the heap and the UTF-8 of
// its source.
struct ImageSource {
  Oop method;
  std::string text;
};

struct Image {
  // False when nothing was written; *reason (when given) then says why (SPEC §3.11).
  // haltedProcesses is how many processes a live debugger holds halted (SPEC §3.13): with any,
  // nothing is written ("halted processes"). sources (P18) go to the sources section, those whose
  // method is in the heap written, in order; none when null.
  static bool save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr, std::size_t haltedProcesses = 0,
                   const std::vector<ImageSource>* sources = nullptr);
  // False when the image is refused; *reason (when given) then says why (SPEC §3.11). On success
  // *sources (when given) is the sources section's pairs with the loaded methods (P18).
  // The returned ImageSource::method values are not rooted: the caller must root them before
  // anything allocates (a collection moves old-space objects).
  static bool load(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr, std::vector<ImageSource>* sources = nullptr);
  // SPEC §3.11: every check load makes before it adopts a heap, on the bytes of an image file.
  // save runs it on the bytes it is about to write, so a save that succeeds writes an image this
  // runtime loads. False when a check fails: *reason (when given) is the load's refusal, *detail
  // (when given) which object or class broke which rule.
  static bool check(const Heap& heap, const WellKnown& wk, const std::vector<std::byte>& file,
                    std::size_t oldMaxBytes, std::string* reason, std::string* detail);
};

}  // namespace ao

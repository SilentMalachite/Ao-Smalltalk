#pragma once

#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace ao {

struct CallContext;

// One GC root slot for the lifetime of the scope. Read `slot` again after anything that can GC.
struct Root {
  Roots& roots;
  Oop slot;
  explicit Root(Roots& r, Oop v = Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

// A fixed run of root slots, all nil at first, registered as one range for the lifetime of the
// scope. Up to kInlineSlots live inside the object; more go to the heap. Scopes nest LIFO.
class RootedArray {
 public:
  static constexpr std::uint32_t kInlineSlots = 6;

  RootedArray(Roots& roots, std::uint32_t count);
  ~RootedArray();
  RootedArray(const RootedArray&) = delete;
  RootedArray& operator=(const RootedArray&) = delete;
  RootedArray(RootedArray&&) = delete;
  RootedArray& operator=(RootedArray&&) = delete;

  // nullptr when empty.
  Oop* ptr() const { return n_ == 0 ? nullptr : data_; }
  Oop& operator[](std::uint32_t i) { return data_[i]; }
  const Oop& operator[](std::uint32_t i) const { return data_[i]; }
  std::uint32_t size() const { return n_; }

 private:
  Roots& roots_;
  std::uint32_t n_;
  Oop* data_;
  std::unique_ptr<Oop[]> spill_;
  Oop inline_[kInlineSlots];
};

// The allocation entry point that may run the GC. A large object first gets a scavenge and, when
// old would pass its threshold, a full collection (Gc::collectBeforeTenured). Then: nursery (or
// old for a large object), then collect the nursery once and retry, then old directly, then old
// once more after a full collection unless one already ran in this call (at most one full
// collection before giving up). A request larger than old's max fails without any collection.
// `cls` is rooted across the collections. When all of these fail, sets the heap's out-of-memory
// flag and returns an empty Oop.
Oop allocateRetry(CallContext& ctx, Oop cls, std::uint32_t size, std::uint16_t flags);

// SPEC §3.6: a new instance of cls for a Kernel class-side constructor (Association key:value:,
// Point x:y:, ...): as many pointer slots as cls's format gives (read as the runtime reads it, 0
// when cls is not class-shaped), never fewer than kernelSlots, all nil, so a subclass that adds
// variables gets them. Runs the GC like allocateRetry: the caller reads its roots again after it.
Oop allocateInstance(CallContext& ctx, Oop cls, std::uint32_t kernelSlots);

}  // namespace ao

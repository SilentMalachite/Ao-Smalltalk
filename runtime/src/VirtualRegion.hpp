#pragma once

#include <cstddef>

namespace ao {

// A range of address space reserved once with no access (mmap PROT_NONE) and made readable and
// writable from its start on demand (mprotect), in whole units of `commitUnit` bytes. Addresses
// never move; committed memory is not given back until the region is destroyed. Move-only.
class VirtualRegion {
 public:
  VirtualRegion() = default;
  ~VirtualRegion();
  VirtualRegion(VirtualRegion&& other) noexcept;
  VirtualRegion& operator=(VirtualRegion&& other) noexcept;
  VirtualRegion(const VirtualRegion&) = delete;
  VirtualRegion& operator=(const VirtualRegion&) = delete;

  // Reserves `bytes` rounded up to whole pages. False (and nothing reserved) when mmap fails.
  bool reserve(std::size_t bytes, std::size_t commitUnit);
  // Ensures the first `bytes` are committed. Commits up to the next unit boundary, but never past
  // the reservation. False when `bytes` exceeds the reservation or mprotect fails.
  bool commit(std::size_t bytes);

  std::byte* base() const { return base_; }
  std::size_t reserved() const { return reserved_; }
  std::size_t committed() const { return committed_; }

 private:
  void release();

  std::byte* base_ = nullptr;
  std::size_t reserved_ = 0;
  std::size_t committed_ = 0;
  std::size_t unit_ = 0;
};

}  // namespace ao

#include "VirtualRegion.hpp"

#include <sys/mman.h>
#include <unistd.h>

#include <algorithm>
#include <utility>

namespace ao {

namespace {

std::size_t roundUp(std::size_t n, std::size_t unit) { return (n + unit - 1) / unit * unit; }

std::size_t pageBytes() {
  const long page = sysconf(_SC_PAGESIZE);
  return page > 0 ? static_cast<std::size_t>(page) : std::size_t{16384};
}

}  // namespace

VirtualRegion::~VirtualRegion() { release(); }

VirtualRegion::VirtualRegion(VirtualRegion&& other) noexcept
    : base_(std::exchange(other.base_, nullptr)),
      reserved_(std::exchange(other.reserved_, 0)),
      committed_(std::exchange(other.committed_, 0)),
      unit_(std::exchange(other.unit_, 0)) {}

VirtualRegion& VirtualRegion::operator=(VirtualRegion&& other) noexcept {
  if (this != &other) {
    release();
    base_ = std::exchange(other.base_, nullptr);
    reserved_ = std::exchange(other.reserved_, 0);
    committed_ = std::exchange(other.committed_, 0);
    unit_ = std::exchange(other.unit_, 0);
  }
  return *this;
}

bool VirtualRegion::reserve(std::size_t bytes, std::size_t commitUnit) {
  release();
  const std::size_t page = pageBytes();
  const std::size_t n = roundUp(std::max<std::size_t>(bytes, 1), page);
  void* p = mmap(nullptr, n, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if (p == MAP_FAILED) {
    return false;
  }
  base_ = static_cast<std::byte*>(p);
  reserved_ = n;
  committed_ = 0;
  unit_ = roundUp(std::max(commitUnit, page), page);
  return true;
}

bool VirtualRegion::commit(std::size_t bytes) {
  if (bytes <= committed_) {
    return true;
  }
  if (base_ == nullptr || bytes > reserved_) {
    return false;
  }
  const std::size_t target = std::min(reserved_, roundUp(bytes, unit_));
  if (mprotect(base_ + committed_, target - committed_, PROT_READ | PROT_WRITE) != 0) {
    return false;
  }
  committed_ = target;
  return true;
}

void VirtualRegion::release() {
  if (base_ != nullptr) {
    munmap(base_, reserved_);
  }
  base_ = nullptr;
  reserved_ = 0;
  committed_ = 0;
  unit_ = 0;
}

}  // namespace ao

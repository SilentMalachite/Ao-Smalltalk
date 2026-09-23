#include "ao/Roots.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace ao {

void Roots::add(Oop* slot) {
  if (slot == nullptr) {
    return;
  }
  slots_.push_back(slot);
}

void Roots::remove(Oop* slot) {
  if (slot == nullptr) {
    return;
  }
  // One registration, most-recent first — nested IcGuard is stack-like.
  auto it = std::find(slots_.rbegin(), slots_.rend(), slot);
  if (it != slots_.rend()) {
    slots_.erase(std::next(it).base());
  }
}

void Roots::pushRange(Oop* first, std::size_t n) {
  if (first == nullptr || n == 0) {
    return;
  }
  ranges_.push_back(Range{first, n});
}

void Roots::popRange(Oop* first, std::size_t n) {
  if (first == nullptr || n == 0) {
    return;
  }
  assert(!ranges_.empty() && ranges_.back().first == first && ranges_.back().n == n &&
         "root ranges must be popped LIFO");
  auto it = std::find_if(ranges_.rbegin(), ranges_.rend(),
                         [&](const Range& r) { return r.first == first && r.n == n; });
  if (it != ranges_.rend()) {
    ranges_.erase(std::next(it).base());
  }
}

std::uint32_t Roots::pushHandle(Oop obj) {
  if (!free_.empty()) {
    const std::uint32_t i = free_.back();
    free_.pop_back();
    handles_[i] = obj;
    live_[i] = 1;
    return i;
  }
  const auto i = static_cast<std::uint32_t>(handles_.size());
  handles_.push_back(obj);
  live_.push_back(1);
  return i;
}

void Roots::dropHandle(std::uint32_t index) {
  if (index >= handles_.size() || live_[index] == 0) {
    return;
  }
  handles_[index] = Oop{};
  live_[index] = 0;
  free_.push_back(index);
}

Oop Roots::handleAt(std::uint32_t index) const {
  if (index >= handles_.size() || live_[index] == 0) {
    return Oop{};
  }
  return handles_[index];
}

void Roots::setStackWalker(StackWalker walker, void* ctx) {
  walker_ = walker;
  walkerCtx_ = ctx;
}

void Roots::visitAll(VisitFn visit, void* ctx) {
  if (visit == nullptr) {
    return;
  }
  for (Oop* slot : slots_) {
    if (slot != nullptr) {
      visit(ctx, slot);
    }
  }
  for (const Range& r : ranges_) {
    for (std::size_t i = 0; i < r.n; ++i) {
      visit(ctx, r.first + i);
    }
  }
  for (std::size_t b = 0; b < frameBlock_; ++b) {
    const FrameBlock& block = frameBlocks_[b];
    for (std::size_t i = 0; i < block.used; ++i) {
      visit(ctx, &block.slots[i]);
    }
  }
  for (std::size_t i = 0; i < frameUsed_; ++i) {
    visit(ctx, frameBase_ + i);
  }
  for (std::size_t i = 0; i < handles_.size(); ++i) {
    if (live_[i] != 0) {
      visit(ctx, &handles_[i]);
    }
  }
  if (walker_ != nullptr) {
    walker_(walkerCtx_, visit, ctx);
  }
}

void Roots::enterNextFrameBlock(std::size_t n) {
  if (frameBase_ != nullptr) {
    // Leave the current block with its frames in place.
    frameBlocks_[frameBlock_].used = frameUsed_;
    ++frameBlock_;
  }
  // Blocks past the current one are empty, so a too-small one can be replaced.
  if (frameBlock_ < frameBlocks_.size() && frameBlocks_[frameBlock_].capacity < n) {
    frameBlocks_.resize(frameBlock_);
  }
  if (frameBlock_ == frameBlocks_.size()) {
    const std::size_t cap = n > kFrameBlockSlots ? n : kFrameBlockSlots;
    frameBlocks_.push_back(FrameBlock{std::make_unique<Oop[]>(cap), cap, 0});
  }
  FrameBlock& block = frameBlocks_[frameBlock_];
  frameBase_ = block.slots.get();
  frameCap_ = block.capacity;
  frameUsed_ = 0;
}

void Roots::returnToPreviousFrameBlock() {
  --frameBlock_;
  const FrameBlock& block = frameBlocks_[frameBlock_];
  frameBase_ = block.slots.get();
  frameCap_ = block.capacity;
  frameUsed_ = block.used;
}

}  // namespace ao

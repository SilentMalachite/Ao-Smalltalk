#include "ao/Roots.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

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

void Roots::pinRange(Oop* first, std::size_t n) {
  if (first == nullptr || n == 0) {
    return;
  }
  pinned_.push_back(Stack::Range{first, n});
}

void Roots::unpinRange(Oop* first, std::size_t n) {
  if (first == nullptr || n == 0) {
    return;
  }
  auto it = std::find_if(pinned_.begin(), pinned_.end(),
                         [&](const Stack::Range& r) { return r.first == first && r.n == n; });
  assert(it != pinned_.end() && "unpinRange of a range that is not pinned");
  if (it != pinned_.end()) {
    // Unordered: the last range takes its place.
    *it = pinned_.back();
    pinned_.pop_back();
  }
}

void Roots::pushRange(Oop* first, std::size_t n) {
  if (first == nullptr || n == 0) {
    return;
  }
  stack_.ranges_.push_back(Stack::Range{first, n});
}

void Roots::popRange(Oop* first, std::size_t n) {
  if (first == nullptr || n == 0) {
    return;
  }
  std::vector<Stack::Range>& ranges = stack_.ranges_;
  assert(!ranges.empty() && ranges.back().first == first && ranges.back().n == n &&
         "root ranges must be popped LIFO");
  auto it = std::find_if(ranges.rbegin(), ranges.rend(),
                         [&](const Stack::Range& r) { return r.first == first && r.n == n; });
  if (it != ranges.rend()) {
    ranges.erase(std::next(it).base());
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
  for (const Stack::Range& r : pinned_) {
    for (std::size_t i = 0; i < r.n; ++i) {
      visit(ctx, r.first + i);
    }
  }
  // The running roots live in stack_ and a parked process's in its attached Stack, never in
  // both, so each slot is visited once (collectOld's forwarding is not idempotent).
  stack_.visit(visit, ctx);
  for (Stack* stack : attached_) {
    stack->visit(visit, ctx);
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

void Roots::switchStack(Stack& parkInto, Stack& activate) {
  assert(&parkInto != &activate && "switchStack takes two different stacks");
  assert(attached(&parkInto) && attached(&activate) && "switched stacks must be attached");
  assert(parkInto.empty() && "the running process's own Stack is empty while it runs");
  // parkInto takes the running roots, stack_ its empty buffers; then stack_ takes activate's
  // roots and activate those empty buffers. Only vectors and pointers change hands.
  stack_.swapWith(parkInto);
  stack_.swapWith(activate);
}

void Roots::attachStack(Stack* stack) {
  assert(stack != nullptr && stack != &stack_ && !attached(stack) &&
         "a stack is attached once, and the running roots never");
  if (stack == nullptr) {
    return;
  }
  attached_.push_back(stack);
}

void Roots::detachStack(Stack* stack) {
  assert(stack != nullptr && stack->empty() && "a stack is detached empty");
  auto it = std::find(attached_.begin(), attached_.end(), stack);
  assert(it != attached_.end() && "detachStack of a stack that is not attached");
  if (it != attached_.end()) {
    attached_.erase(it);
  }
}

bool Roots::attached(const Stack* stack) const {
  return std::find(attached_.begin(), attached_.end(), stack) != attached_.end();
}

Roots::Counts Roots::counts() const {
  Counts c;
  c.slots = slots_.size();
  for (const Stack::Range& r : pinned_) {
    c.pinnedSlots += r.n;
  }
  c.ranges = stack_.rangeCount();
  c.frameSlots = stack_.frameSlotCount();
  for (const Stack* stack : attached_) {
    c.ranges += stack->rangeCount();
    c.frameSlots += stack->frameSlotCount();
  }
  c.handles = handles_.size() - free_.size();
  c.attachedStacks = attached_.size();
  return c;
}

std::size_t Roots::Stack::frameSlotCount() const {
  std::size_t n = frameUsed_;
  for (std::size_t b = 0; b < frameBlock_; ++b) {
    n += frameBlocks_[b].used;
  }
  return n;
}

void Roots::Stack::swapWith(Stack& other) noexcept {
  using std::swap;
  swap(ranges_, other.ranges_);
  swap(frameBlocks_, other.frameBlocks_);
  swap(frameBlock_, other.frameBlock_);
  swap(frameBase_, other.frameBase_);
  swap(frameUsed_, other.frameUsed_);
  swap(frameCap_, other.frameCap_);
}

void Roots::Stack::visit(VisitFn fn, void* ctx) {
  for (const Range& r : ranges_) {
    for (std::size_t i = 0; i < r.n; ++i) {
      fn(ctx, r.first + i);
    }
  }
  for (std::size_t b = 0; b < frameBlock_; ++b) {
    const FrameBlock& block = frameBlocks_[b];
    for (std::size_t i = 0; i < block.used; ++i) {
      fn(ctx, &block.slots[i]);
    }
  }
  for (std::size_t i = 0; i < frameUsed_; ++i) {
    fn(ctx, frameBase_ + i);
  }
}

void Roots::Stack::enterNextFrameBlock(std::size_t n) {
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

void Roots::Stack::returnToPreviousFrameBlock() {
  --frameBlock_;
  const FrameBlock& block = frameBlocks_[frameBlock_];
  frameBase_ = block.slots.get();
  frameCap_ = block.capacity;
  frameUsed_ = block.used;
}

}  // namespace ao

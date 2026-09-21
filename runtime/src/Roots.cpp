#include "ao/Roots.hpp"

#include <algorithm>

namespace ao {

void Roots::add(Oop* slot) {
  if (slot == nullptr) {
    return;
  }
  slots_.push_back(slot);
}

void Roots::remove(Oop* slot) {
  slots_.erase(std::remove(slots_.begin(), slots_.end(), slot), slots_.end());
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
  for (std::size_t i = 0; i < handles_.size(); ++i) {
    if (live_[i] != 0) {
      visit(ctx, &handles_[i]);
    }
  }
  if (walker_ != nullptr) {
    walker_(walkerCtx_, visit, ctx);
  }
}

}  // namespace ao

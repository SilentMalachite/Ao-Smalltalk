#pragma once

#include "ao/Oop.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace ao {

class Roots {
 public:
  void add(Oop* slot);
  void remove(Oop* slot);

  // Registers n contiguous slots as one root range. Ranges are released LIFO.
  void pushRange(Oop* first, std::size_t n);
  void popRange(Oop* first, std::size_t n);

  // Rooted slots for one native call: the receiver in slot 0, then argc arguments. Frames nest
  // LIFO and keep their addresses until popped, so a native may hold references into its frame.
  // This is the hot path of every native call: a bump in a preallocated block, no allocation.
  Oop* pushFrame(Oop receiver, const Oop* args, std::uint32_t argc);
  void popFrame(Oop* frame, std::uint32_t argc);

  // Host handle table. P1: structure only; AppKit は載せない。
  std::uint32_t pushHandle(Oop obj);
  void dropHandle(std::uint32_t index);
  Oop handleAt(std::uint32_t index) const;

  using VisitFn = void (*)(void* ctx, Oop* slot);
  using StackWalker = void (*)(void* ctx, VisitFn visit, void* visitCtx);
  void setStackWalker(StackWalker walker, void* ctx);

  void visitAll(VisitFn visit, void* ctx);

 private:
  struct Range {
    Oop* first;
    std::size_t n;
  };
  std::vector<Oop*> slots_;
  std::vector<Range> ranges_;
  std::vector<Oop> handles_;
  std::vector<std::uint8_t> live_;
  std::vector<std::uint32_t> free_;
  StackWalker walker_ = nullptr;
  void* walkerCtx_ = nullptr;

  // Native frames live in blocks that never move. Only the current block has a bump pointer;
  // earlier blocks keep the fill level they had when the next one was entered.
  struct FrameBlock {
    std::unique_ptr<Oop[]> slots;
    std::size_t capacity = 0;
    std::size_t used = 0;
  };
  static constexpr std::size_t kFrameBlockSlots = 4096;
  void enterNextFrameBlock(std::size_t n);
  void returnToPreviousFrameBlock();
  std::vector<FrameBlock> frameBlocks_;
  std::size_t frameBlock_ = 0;
  Oop* frameBase_ = nullptr;
  std::size_t frameUsed_ = 0;
  std::size_t frameCap_ = 0;
};

inline Oop* Roots::pushFrame(Oop receiver, const Oop* args, std::uint32_t argc) {
  const std::size_t n = std::size_t{argc} + 1;
  if (frameUsed_ + n > frameCap_) {
    enterNextFrameBlock(n);
  }
  Oop* frame = frameBase_ + frameUsed_;
  frameUsed_ += n;
  frame[0] = receiver;
  for (std::uint32_t i = 0; i < argc; ++i) {
    frame[i + 1] = args[i];
  }
  return frame;
}

inline void Roots::popFrame(Oop* frame, std::uint32_t argc) {
  const std::size_t n = std::size_t{argc} + 1;
  assert(frameUsed_ >= n && frame == frameBase_ + (frameUsed_ - n) &&
         "native frames must be popped LIFO");
  (void)frame;
  frameUsed_ -= n;
  if (frameUsed_ == 0 && frameBlock_ > 0) {
    returnToPreviousFrameBlock();
  }
}

}  // namespace ao

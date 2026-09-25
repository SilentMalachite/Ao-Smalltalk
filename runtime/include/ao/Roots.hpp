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
  using VisitFn = void (*)(void* ctx, Oop* slot);
  using StackWalker = void (*)(void* ctx, VisitFn visit, void* visitCtx);

  // The LIFO roots of one call stack: root ranges (RootedArray) and native frames. SPEC §3.4
  // (B10): every process has its own, and only the running one's take pushes and pops. The
  // running one lives in Roots; switchStack parks it in a Stack object and makes a parked one
  // running. A switch moves no buffer, so a native's pointers into its frame stay valid. A Stack
  // is neither copied nor moved: Roots knows the attached ones by address.
  class Stack {
   public:
    Stack() = default;
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    Stack(Stack&&) = delete;
    Stack& operator=(Stack&&) = delete;

    // No ranges and no frames.
    bool empty() const { return ranges_.empty() && frameBlock_ == 0 && frameUsed_ == 0; }
    std::size_t rangeCount() const { return ranges_.size(); }
    std::size_t frameSlotCount() const;

   private:
    friend class Roots;
    struct Range {
      Oop* first;
      std::size_t n;
    };
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
    void swapWith(Stack& other) noexcept;
    void visit(VisitFn fn, void* ctx);
    std::vector<Range> ranges_;
    std::vector<FrameBlock> frameBlocks_;
    std::size_t frameBlock_ = 0;
    Oop* frameBase_ = nullptr;
    std::size_t frameUsed_ = 0;
    std::size_t frameCap_ = 0;
  };

  void add(Oop* slot);
  void remove(Oop* slot);

  // Registers n contiguous slots as one root range of the running Stack. Ranges are released
  // LIFO.
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

  void setStackWalker(StackWalker walker, void* ctx);

  // B10: parks the running Stack's roots in parkInto and makes activate's the running ones.
  // parkInto is the running process's own Stack object, which is empty while it runs; activate
  // is left empty. Both are attached and different. No buffer moves.
  void switchStack(Stack& parkInto, Stack& activate);
  // visitAll visits an attached Stack's roots (a parked process's) until it is detached. A Stack
  // is attached once, and detached empty.
  void attachStack(Stack* stack);
  void detachStack(Stack* stack);
  const Stack& runningStack() const { return stack_; }

  // What visitAll visits, by kind (the stack walker's aside). For tests and Debug checks.
  struct Counts {
    std::size_t slots = 0;           // add
    std::size_t ranges = 0;          // pushRange: the running Stack and the attached ones
    std::size_t frameSlots = 0;      // pushFrame: the running Stack and the attached ones
    std::size_t handles = 0;         // live pushHandle entries
    std::size_t attachedStacks = 0;  // attachStack
  };
  Counts counts() const;

  // Every root once: the slots, the running Stack, each attached Stack, the handles, then the
  // stack walker.
  void visitAll(VisitFn visit, void* ctx);

 private:
  bool attached(const Stack* stack) const;
  std::vector<Oop*> slots_;
  Stack stack_;
  std::vector<Stack*> attached_;
  std::vector<Oop> handles_;
  std::vector<std::uint8_t> live_;
  std::vector<std::uint32_t> free_;
  StackWalker walker_ = nullptr;
  void* walkerCtx_ = nullptr;
};

inline Oop* Roots::pushFrame(Oop receiver, const Oop* args, std::uint32_t argc) {
  const std::size_t n = std::size_t{argc} + 1;
  Stack& s = stack_;
  if (s.frameUsed_ + n > s.frameCap_) {
    s.enterNextFrameBlock(n);
  }
  Oop* frame = s.frameBase_ + s.frameUsed_;
  s.frameUsed_ += n;
  frame[0] = receiver;
  for (std::uint32_t i = 0; i < argc; ++i) {
    frame[i + 1] = args[i];
  }
  return frame;
}

inline void Roots::popFrame(Oop* frame, std::uint32_t argc) {
  const std::size_t n = std::size_t{argc} + 1;
  Stack& s = stack_;
  assert(s.frameUsed_ >= n && frame == s.frameBase_ + (s.frameUsed_ - n) &&
         "native frames must be popped LIFO");
  (void)frame;
  s.frameUsed_ -= n;
  if (s.frameUsed_ == 0 && s.frameBlock_ > 0) {
    s.returnToPreviousFrameBlock();
  }
}

}  // namespace ao

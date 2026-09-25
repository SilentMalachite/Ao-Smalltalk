#pragma once

#include "FiberRegs.h"

#include <cstddef>
#include <cstdint>

// Stackful fibers for the cooperative scheduler (SPEC §3.4): a guarded, pooled machine stack per
// fiber and an AArch64 register switch written in assembly (FiberSwitch_arm64.S). Single thread:
// a fiber runs only on the thread that switches to it, and only one of them runs at a time.
//
// Rules for code that uses these (the scheduler):
// - A fiber's entry function never returns. It ends with fiberSwitchFinal; if it returns anyway,
//   the trampoline traps (brk).
// - The first thing an entry function does is call fiberEntered().
// - C++ exceptions must not leave an entry function, and must not be in flight across a switch.
//   The entry function catches everything (the caller's responsibility); an exception that reaches
//   the trampoline finds no frame to unwind into and ends in std::terminate.
// - A stack is given back (FiberStack::release) only after its fiber has switched away for the
//   last time, and by some other context: a fiber never releases the stack it is running on.

namespace ao {

// Callee-saved registers of a suspended context. The layout is FiberRegs.h; d8-d15 are kept as raw
// bits.
struct FiberRegs {
  std::uint64_t x[10];  // x19-x28
  std::uint64_t fp;     // x29
  std::uint64_t lr;     // x30; where the switch returns to
  std::uint64_t sp;
  std::uint64_t d[8];  // d8-d15
};

static_assert(offsetof(FiberRegs, x) == AO_FIBER_X19);
static_assert(offsetof(FiberRegs, x) + 2 * sizeof(std::uint64_t) == AO_FIBER_X21);
static_assert(offsetof(FiberRegs, x) + 4 * sizeof(std::uint64_t) == AO_FIBER_X23);
static_assert(offsetof(FiberRegs, x) + 6 * sizeof(std::uint64_t) == AO_FIBER_X25);
static_assert(offsetof(FiberRegs, x) + 8 * sizeof(std::uint64_t) == AO_FIBER_X27);
static_assert(offsetof(FiberRegs, fp) == AO_FIBER_FP);
static_assert(offsetof(FiberRegs, lr) == AO_FIBER_LR);
static_assert(offsetof(FiberRegs, sp) == AO_FIBER_SP);
static_assert(offsetof(FiberRegs, d) == AO_FIBER_D8);
static_assert(offsetof(FiberRegs, d) + 2 * sizeof(std::uint64_t) == AO_FIBER_D10);
static_assert(offsetof(FiberRegs, d) + 4 * sizeof(std::uint64_t) == AO_FIBER_D12);
static_assert(offsetof(FiberRegs, d) + 6 * sizeof(std::uint64_t) == AO_FIBER_D14);
static_assert(sizeof(FiberRegs) == AO_FIBER_REGS_SIZE);

// A machine stack as the sanitizer sees it: lowest usable address and usable size.
struct FiberStackBounds {
  const void* bottom = nullptr;
  std::size_t size = 0;
};

// The machine stack of one fiber: kUsableBytes of read-write memory (committed lazily by the
// kernel) above one PROT_NONE guard page (sysconf(_SC_PAGESIZE)), so running off the low end
// faults. Move-only; an empty stack (valid() false) owns nothing. Destroying a stack unmaps it;
// release() instead returns it to a small pool that acquire() takes from first.
class FiberStack {
 public:
  static constexpr std::size_t kUsableBytes = std::size_t{8} << 20;
  static constexpr std::size_t kPoolLimit = 4;

  FiberStack() = default;
  ~FiberStack();
  FiberStack(FiberStack&& other) noexcept;
  FiberStack& operator=(FiberStack&& other) noexcept;
  FiberStack(const FiberStack&) = delete;
  FiberStack& operator=(const FiberStack&) = delete;

  // A pooled stack if there is one, else a newly mapped one. Empty when mapping fails.
  static FiberStack acquire();
  // Gives `stack` back: kept in the pool (its pages handed back with MADV_FREE) while the pool
  // holds fewer than kPoolLimit, unmapped otherwise. Leaves `stack` empty; empty stacks are ignored.
  static void release(FiberStack&& stack);
  // Stacks the pool holds now (tests).
  static std::size_t pooledCount();

  bool valid() const { return base_ != nullptr; }
  // Lowest usable address, just above the guard page.
  std::byte* low() const { return base_ == nullptr ? nullptr : base_ + guard_; }
  // One past the highest usable address: the initial stack pointer (16-byte aligned).
  std::byte* high() const { return base_ == nullptr ? nullptr : base_ + guard_ + kUsableBytes; }
  std::size_t size() const { return base_ == nullptr ? 0 : kUsableBytes; }
  FiberStackBounds bounds() const { return {low(), size()}; }

 private:
  static FiberStack map();
  void unmap();

  std::byte* base_ = nullptr;  // start of the mapping: the guard page
  std::size_t guard_ = 0;
};

// The calling thread's own stack (from pthread), for switches back to the thread's context.
FiberStackBounds threadStackBounds();

// Prepares `regs` so that the first switch to it runs entry(arg) on `stack`, which must stay mapped
// while the fiber can run. `entry` never returns (see the rules above).
void fiberInit(FiberRegs& regs, const FiberStack& stack, void (*entry)(void*), void* arg);

// Saves the running context's registers into `save` and continues the context in `load`. Returns
// when a later switch loads `save`. No sanitizer bookkeeping: under ASan use the overloads below.
void fiberSwitch(FiberRegs& save, const FiberRegs& load);

// fiberSwitch with the sanitizer bookkeeping around it. `to` is the stack `load` runs on:
// FiberStack::bounds() for a fiber, threadStackBounds() for the thread's own context.
void fiberSwitch(FiberRegs& save, const FiberRegs& load, FiberStackBounds to);

// The last switch away from a fiber that never runs again: the sanitizer drops the fiber's fake
// stack. Does not return. The fiber's stack may be released once control is elsewhere.
[[noreturn]] void fiberSwitchFinal(FiberRegs& save, const FiberRegs& load, FiberStackBounds to);

// Completes the sanitizer switch that started the fiber. The first call in every entry function.
void fiberEntered();

// The sanitizer bookkeeping on its own, for code that switches some other way. No-ops unless built
// with AddressSanitizer (__sanitizer_start_switch_fiber / __sanitizer_finish_switch_fiber):
// - Right before a switch: fiberSanitizerStartSwitch(&fakeStack, to), where fakeStack is a local of
//   the switching frame; pass nullptr instead of &fakeStack on the last switch away from a fiber
//   that never runs again. That frees the fiber's fake stack during the call, so the calling frame
//   must not be on it (built without ASan instrumentation) and must switch straight away: prefer
//   fiberSwitchFinal, which does both.
// - Right after control comes back to that frame: fiberSanitizerFinishSwitch(fakeStack).
// - At a fiber's entry, before anything else: fiberSanitizerFinishSwitch(nullptr) (fiberEntered).
void fiberSanitizerStartSwitch(void** fakeStackSave, FiberStackBounds to);
void fiberSanitizerFinishSwitch(void* fakeStack);

}  // namespace ao

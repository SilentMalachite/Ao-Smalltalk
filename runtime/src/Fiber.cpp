#include "Fiber.hpp"

#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

#include <array>
#include <utility>

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define AO_FIBER_ASAN 1
#endif
#endif
#if defined(__SANITIZE_ADDRESS__) && !defined(AO_FIBER_ASAN)
#define AO_FIBER_ASAN 1
#endif

#if defined(AO_FIBER_ASAN)
#include <sanitizer/asan_interface.h>
#include <sanitizer/common_interface_defs.h>
// Keeps a function's frame on the machine stack. __sanitizer_start_switch_fiber(nullptr, ...) frees
// the running fiber's fake stack (detect_stack_use_after_return) before it returns, so no frame
// that is still used afterwards may live there.
#define AO_FIBER_REAL_FRAME __attribute__((no_sanitize("address")))
#else
#define AO_FIBER_REAL_FRAME
#endif

extern "C" {
// FiberSwitch_arm64.S
void ao_fiber_switch(ao::FiberRegs* save, const ao::FiberRegs* load);
void ao_fiber_trampoline();
}

namespace ao {

namespace {

std::size_t pageBytes() {
  const long page = sysconf(_SC_PAGESIZE);
  return page > 0 ? static_cast<std::size_t>(page) : std::size_t{16384};
}

// Frames abandoned on a fiber stack leave their redzones poisoned in ASan's shadow memory. Clear it
// before the memory is handed out again or unmapped, or later users of the addresses see stale
// poison.
void clearShadow(std::byte* low, std::size_t size) {
#if defined(AO_FIBER_ASAN)
  __asan_unpoison_memory_region(low, size);
#else
  (void)low;
  (void)size;
#endif
}

// Per thread: a scheduler and all of its fibers live on one thread (the session's), so no lock.
struct StackPool {
  std::array<FiberStack, FiberStack::kPoolLimit> stacks;
  std::size_t count = 0;
};

thread_local StackPool tPool;

}  // namespace

FiberStack::~FiberStack() { unmap(); }

FiberStack::FiberStack(FiberStack&& other) noexcept
    : base_(std::exchange(other.base_, nullptr)), guard_(std::exchange(other.guard_, 0)) {}

FiberStack& FiberStack::operator=(FiberStack&& other) noexcept {
  if (this != &other) {
    unmap();
    base_ = std::exchange(other.base_, nullptr);
    guard_ = std::exchange(other.guard_, 0);
  }
  return *this;
}

FiberStack FiberStack::acquire() {
  if (tPool.count > 0) {
    --tPool.count;
    return std::move(tPool.stacks[tPool.count]);
  }
  return map();
}

void FiberStack::release(FiberStack&& stack) {
  FiberStack s = std::move(stack);
  if (!s.valid() || tPool.count == kPoolLimit) {
    return;  // `s` unmaps as it goes out of scope
  }
  clearShadow(s.low(), kUsableBytes);
  // Advisory: the pages' contents may go; the addresses stay mapped read-write.
  madvise(s.low(), kUsableBytes, MADV_FREE);
  tPool.stacks[tPool.count] = std::move(s);
  ++tPool.count;
}

std::size_t FiberStack::pooledCount() { return tPool.count; }

FiberStack FiberStack::map() {
  FiberStack s;
  const std::size_t guard = pageBytes();
  void* p = mmap(nullptr, guard + kUsableBytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1,
                 0);
  if (p == MAP_FAILED) {
    return s;
  }
  if (mprotect(p, guard, PROT_NONE) != 0) {
    munmap(p, guard + kUsableBytes);
    return s;
  }
  s.base_ = static_cast<std::byte*>(p);
  s.guard_ = guard;
  return s;
}

void FiberStack::unmap() {
  if (base_ != nullptr) {
    clearShadow(low(), kUsableBytes);
    munmap(base_, guard_ + kUsableBytes);
  }
  base_ = nullptr;
  guard_ = 0;
}

FiberStackBounds threadStackBounds() {
  pthread_t self = pthread_self();
  const auto* high = static_cast<const std::byte*>(pthread_get_stackaddr_np(self));
  const std::size_t size = pthread_get_stacksize_np(self);
  return {high - size, size};
}

void fiberInit(FiberRegs& regs, const FiberStack& stack, void (*entry)(void*), void* arg) {
  regs = FiberRegs{};
  regs.x[0] = reinterpret_cast<std::uint64_t>(entry);  // x19: called by the trampoline
  regs.x[1] = reinterpret_cast<std::uint64_t>(arg);    // x20: its argument
  regs.fp = 0;                                         // ends the frame-pointer chain
  regs.lr = reinterpret_cast<std::uint64_t>(&ao_fiber_trampoline);
  regs.sp = reinterpret_cast<std::uint64_t>(stack.high()) & ~std::uint64_t{15};
}

void fiberSwitch(FiberRegs& save, const FiberRegs& load) { ao_fiber_switch(&save, &load); }

void fiberSwitch(FiberRegs& save, const FiberRegs& load, FiberStackBounds to) {
  // Lives in this frame, which is where control comes back to.
  void* fakeStack = nullptr;
  fiberSanitizerStartSwitch(&fakeStack, to);
  ao_fiber_switch(&save, &load);
  fiberSanitizerFinishSwitch(fakeStack);
}

AO_FIBER_REAL_FRAME void fiberSwitchFinal(FiberRegs& save, const FiberRegs& load,
                                          FiberStackBounds to) {
  fiberSanitizerStartSwitch(nullptr, to);
  ao_fiber_switch(&save, &load);
  __builtin_trap();  // nothing switches back to a finished fiber
}

void fiberEntered() { fiberSanitizerFinishSwitch(nullptr); }

AO_FIBER_REAL_FRAME void fiberSanitizerStartSwitch(void** fakeStackSave, FiberStackBounds to) {
#if defined(AO_FIBER_ASAN)
  __sanitizer_start_switch_fiber(fakeStackSave, to.bottom, to.size);
#else
  (void)fakeStackSave;
  (void)to;
#endif
}

void fiberSanitizerFinishSwitch(void* fakeStack) {
#if defined(AO_FIBER_ASAN)
  __sanitizer_finish_switch_fiber(fakeStack, nullptr, nullptr);
#else
  (void)fakeStack;
#endif
}

}  // namespace ao

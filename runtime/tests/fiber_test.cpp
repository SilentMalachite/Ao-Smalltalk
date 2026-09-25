#include "../src/Fiber.hpp"

#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <set>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace {

constexpr int kRounds = 10000;

// One step of the computation each side carries across its switches. Always inlined so that, in an
// optimized build, `n` and `x` live in callee-saved registers (x19-x28, d8-d15) across the switch.
[[gnu::always_inline]] inline void step(std::uint64_t& n, double& x, int i) {
  n = n * 6364136223846793005ull + 1442695040888963407ull + static_cast<std::uint64_t>(i);
  x = x * 0.75;
  x += static_cast<double>(n >> 52) * 0.001;
}

struct PingPong {
  ao::FiberRegs mainRegs{};
  ao::FiberRegs fiberRegs{};
  ao::FiberStackBounds mainBounds{};
  ao::FiberStackBounds fiberBounds{};
  int turns = 0;  // each side bumps it once per round; checks strict alternation
  bool alternated = true;
  std::uintptr_t fiberFrame = 0;
  std::uint64_t fiberN = 0;
  double fiberX = 0;
};

void pingPongFiber(void* arg) {
  ao::fiberEntered();
  auto* p = static_cast<PingPong*>(arg);
  p->fiberFrame = reinterpret_cast<std::uintptr_t>(__builtin_frame_address(0));
  std::uint64_t n = 7;
  double x = 0.25;
  for (int i = 0; i < kRounds; ++i) {
    step(n, x, i);
    if (p->turns != 2 * i) {
      p->alternated = false;
    }
    ++p->turns;
    ao::fiberSwitch(p->fiberRegs, p->mainRegs, p->mainBounds);
  }
  p->fiberN = n;
  p->fiberX = x;
  ao::fiberSwitchFinal(p->fiberRegs, p->mainRegs, p->mainBounds);
}

}  // namespace

TEST(Fiber, PingPongKeepsIntAndDoubleLocals) {
  ao::FiberStack stack = ao::FiberStack::acquire();
  ASSERT_TRUE(stack.valid());
  PingPong p;
  p.mainBounds = ao::threadStackBounds();
  p.fiberBounds = stack.bounds();
  ao::fiberInit(p.fiberRegs, stack, pingPongFiber, &p);
  std::uint64_t n = 11;
  double x = 0.5;
  for (int i = 0; i < kRounds; ++i) {
    ao::fiberSwitch(p.mainRegs, p.fiberRegs, p.fiberBounds);
    step(n, x, i);
    if (p.turns != 2 * i + 1) {
      p.alternated = false;
    }
    ++p.turns;
  }
  // The fiber is parked in its last switch; one more turn lets it store its results and finish.
  ao::fiberSwitch(p.mainRegs, p.fiberRegs, p.fiberBounds);
  const auto low = reinterpret_cast<std::uintptr_t>(stack.low());
  const auto high = reinterpret_cast<std::uintptr_t>(stack.high());
  ao::FiberStack::release(std::move(stack));

  std::uint64_t mainN = 11;
  std::uint64_t fiberN = 7;
  double mainX = 0.5;
  double fiberX = 0.25;
  for (int i = 0; i < kRounds; ++i) {
    step(mainN, mainX, i);
    step(fiberN, fiberX, i);
  }
  EXPECT_TRUE(p.alternated);
  EXPECT_EQ(2 * kRounds, p.turns);
  EXPECT_EQ(mainN, n);
  EXPECT_EQ(mainX, x);
  EXPECT_EQ(fiberN, p.fiberN);
  EXPECT_EQ(fiberX, p.fiberX);
  EXPECT_LE(low, p.fiberFrame);
  EXPECT_LT(p.fiberFrame, high);
}

namespace {

constexpr int kSavedRegs = 18;  // x19-x28, then d8-d15

struct RegisterSwap {
  ao::FiberRegs mainRegs{};
  ao::FiberRegs fiberRegs{};
  ao::FiberStackBounds mainBounds{};
  std::uint64_t fiberIn[kSavedRegs] = {};
  std::uint64_t fiberOut[kSavedRegs] = {};
};

// Loads x19-x28 and d8-d15 from `in`, switches with ao_fiber_switch, and stores the same registers
// into `out` once control comes back. The registers are set in assembly, so this checks the switch
// itself at any optimization level.
void switchWithRegisters(ao::FiberRegs& save, const ao::FiberRegs& load, ao::FiberStackBounds to,
                         const std::uint64_t* in, std::uint64_t* out) {
  void* fakeStack = nullptr;
  ao::fiberSanitizerStartSwitch(&fakeStack, to);
  register ao::FiberRegs* r0 asm("x0") = &save;
  register const ao::FiberRegs* r1 asm("x1") = &load;
  register const std::uint64_t* r2 asm("x2") = in;
  register std::uint64_t* r3 asm("x3") = out;
  asm volatile(
      "str x3, [sp, #-16]!\n\t"
      "ldp x19, x20, [x2, #0]\n\t"
      "ldp x21, x22, [x2, #16]\n\t"
      "ldp x23, x24, [x2, #32]\n\t"
      "ldp x25, x26, [x2, #48]\n\t"
      "ldp x27, x28, [x2, #64]\n\t"
      "ldp d8, d9, [x2, #80]\n\t"
      "ldp d10, d11, [x2, #96]\n\t"
      "ldp d12, d13, [x2, #112]\n\t"
      "ldp d14, d15, [x2, #128]\n\t"
      "bl _ao_fiber_switch\n\t"
      "ldr x3, [sp], #16\n\t"
      "stp x19, x20, [x3, #0]\n\t"
      "stp x21, x22, [x3, #16]\n\t"
      "stp x23, x24, [x3, #32]\n\t"
      "stp x25, x26, [x3, #48]\n\t"
      "stp x27, x28, [x3, #64]\n\t"
      "stp d8, d9, [x3, #80]\n\t"
      "stp d10, d11, [x3, #96]\n\t"
      "stp d12, d13, [x3, #112]\n\t"
      "stp d14, d15, [x3, #128]\n\t"
      : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3)
      :
      : "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16",
        "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x30", "v0",
        "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14",
        "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27",
        "v28", "v29", "v30", "v31", "memory", "cc");
  ao::fiberSanitizerFinishSwitch(fakeStack);
}

void registerFiber(void* arg) {
  ao::fiberEntered();
  auto* r = static_cast<RegisterSwap*>(arg);
  switchWithRegisters(r->fiberRegs, r->mainRegs, r->mainBounds, r->fiberIn, r->fiberOut);
  ao::fiberSwitchFinal(r->fiberRegs, r->mainRegs, r->mainBounds);
}

}  // namespace

TEST(Fiber, SwitchRestoresEveryCalleeSavedRegister) {
  ao::FiberStack stack = ao::FiberStack::acquire();
  ASSERT_TRUE(stack.valid());
  RegisterSwap r;
  r.mainBounds = ao::threadStackBounds();
  std::uint64_t mainIn[kSavedRegs];
  std::uint64_t mainOut[kSavedRegs] = {};
  for (int i = 0; i < kSavedRegs; ++i) {
    mainIn[i] = 0xa5a5'0000'0000'0000ull | (static_cast<std::uint64_t>(i) << 8) | 0x11;
    r.fiberIn[i] = 0x5a5a'0000'0000'0000ull | (static_cast<std::uint64_t>(i) << 16) | 0x22;
  }
  ao::fiberInit(r.fiberRegs, stack, registerFiber, &r);
  // The fiber loads its own values into every callee-saved register and switches back here.
  switchWithRegisters(r.mainRegs, r.fiberRegs, stack.bounds(), mainIn, mainOut);
  // Back into the fiber: its values must come back the same way. It then finishes.
  ao::fiberSwitch(r.mainRegs, r.fiberRegs, stack.bounds());
  ao::FiberStack::release(std::move(stack));
  for (int i = 0; i < kSavedRegs; ++i) {
    EXPECT_EQ(mainIn[i], mainOut[i]) << "main register " << i;
    EXPECT_EQ(r.fiberIn[i], r.fiberOut[i]) << "fiber register " << i;
  }
}

namespace {

// Protection of the VM region that holds `addr`, or -1 when no region holds it.
int protectionAt(const void* addr) {
  const auto want = reinterpret_cast<mach_vm_address_t>(addr);
  mach_vm_address_t region = want;
  mach_vm_size_t size = 0;
  vm_region_basic_info_data_64_t info{};
  mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
  mach_port_t object = MACH_PORT_NULL;
  const kern_return_t kr =
      mach_vm_region(mach_task_self(), &region, &size, VM_REGION_BASIC_INFO_64,
                     reinterpret_cast<vm_region_info_t>(&info), &count, &object);
  if (kr != KERN_SUCCESS || want < region || want - region >= size) {
    return -1;
  }
  return info.protection;
}

}  // namespace

TEST(Fiber, GuardPageIsProtNone) {
  ao::FiberStack stack = ao::FiberStack::acquire();
  ASSERT_TRUE(stack.valid());
  const auto page = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
  EXPECT_EQ(ao::FiberStack::kUsableBytes, stack.size());
  EXPECT_EQ(stack.low() + stack.size(), stack.high());
  EXPECT_EQ(0u, reinterpret_cast<std::uintptr_t>(stack.high()) % 16);
  EXPECT_EQ(stack.low(), stack.bounds().bottom);
  EXPECT_EQ(stack.size(), stack.bounds().size);
  // One whole page below the usable range is inaccessible; the usable range is read-write.
  EXPECT_EQ(VM_PROT_NONE, protectionAt(stack.low() - page));
  EXPECT_EQ(VM_PROT_NONE, protectionAt(stack.low() - 1));
  EXPECT_EQ(VM_PROT_READ | VM_PROT_WRITE, protectionAt(stack.low()));
  EXPECT_EQ(VM_PROT_READ | VM_PROT_WRITE, protectionAt(stack.high() - 1));
  // Running off the low end of the stack faults instead of writing into a neighbouring mapping.
  auto* const guard = reinterpret_cast<volatile unsigned char*>(stack.low() - 1);
  EXPECT_DEATH(*guard = 1, "");
  ao::FiberStack::release(std::move(stack));
}

TEST(Fiber, PoolReusesStacks) {
  // A stack given back is the next one handed out, and is still usable.
  ao::FiberStack first = ao::FiberStack::acquire();
  ASSERT_TRUE(first.valid());
  std::byte* const low = first.low();
  ao::FiberStack::release(std::move(first));
  EXPECT_FALSE(first.valid());
  ao::FiberStack again = ao::FiberStack::acquire();
  ASSERT_TRUE(again.valid());
  EXPECT_EQ(low, again.low());
  *again.low() = std::byte{1};
  *(again.high() - 1) = std::byte{2};
  EXPECT_EQ(std::byte{1}, *again.low());
  ao::FiberStack::release(std::move(again));

  // Taking more stacks than the pool holds empties it; giving them all back keeps only kPoolLimit.
  std::vector<ao::FiberStack> taken;
  std::set<std::byte*> lows;
  for (std::size_t i = 0; i < ao::FiberStack::kPoolLimit + 2; ++i) {
    taken.push_back(ao::FiberStack::acquire());
    ASSERT_TRUE(taken.back().valid());
    lows.insert(taken.back().low());
  }
  EXPECT_EQ(ao::FiberStack::kPoolLimit + 2, lows.size());
  EXPECT_EQ(0u, ao::FiberStack::pooledCount());
  for (auto& s : taken) {
    ao::FiberStack::release(std::move(s));
  }
  EXPECT_EQ(ao::FiberStack::kPoolLimit, ao::FiberStack::pooledCount());
  std::vector<ao::FiberStack> reused;
  for (std::size_t i = 0; i < ao::FiberStack::kPoolLimit; ++i) {
    reused.push_back(ao::FiberStack::acquire());
    ASSERT_TRUE(reused.back().valid());
    EXPECT_EQ(1u, lows.count(reused.back().low()));
  }
  EXPECT_EQ(0u, ao::FiberStack::pooledCount());
  for (auto& s : reused) {
    ao::FiberStack::release(std::move(s));
  }
}

namespace {

constexpr std::size_t kLevelBytes = 1024;
constexpr int kLevels = 4096;

struct Deep {
  ao::FiberRegs mainRegs{};
  ao::FiberRegs fiberRegs{};
  ao::FiberStackBounds mainBounds{};
  std::uintptr_t lowest = UINTPTR_MAX;
  std::uint64_t sum = 0;
};

// Takes kLevelBytes of real stack per level (alloca stays on the machine stack even under ASan's
// fake stacks), writes to every cache line of it, and records the lowest address reached.
std::uint64_t recurse(Deep& d, int level) {
  auto* buf = static_cast<volatile unsigned char*>(__builtin_alloca(kLevelBytes));
  for (std::size_t i = 0; i < kLevelBytes; i += 64) {
    buf[i] = static_cast<unsigned char>(level);
  }
  const auto here = reinterpret_cast<std::uintptr_t>(buf);
  if (here < d.lowest) {
    d.lowest = here;
  }
  const std::uint64_t below = level > 0 ? recurse(d, level - 1) : 0;
  return below + buf[0];
}

void deepFiber(void* arg) {
  ao::fiberEntered();
  auto* d = static_cast<Deep*>(arg);
  d->sum = recurse(*d, kLevels - 1);
  ao::fiberSwitchFinal(d->fiberRegs, d->mainRegs, d->mainBounds);
}

}  // namespace

TEST(Fiber, DeepRecursionOnFiberStack) {
  ao::FiberStack stack = ao::FiberStack::acquire();
  ASSERT_TRUE(stack.valid());
  Deep d;
  d.mainBounds = ao::threadStackBounds();
  ao::fiberInit(d.fiberRegs, stack, deepFiber, &d);
  ao::fiberSwitch(d.mainRegs, d.fiberRegs, stack.bounds());
  std::uint64_t want = 0;
  for (int level = 0; level < kLevels; ++level) {
    want += static_cast<unsigned char>(level);
  }
  EXPECT_EQ(want, d.sum);
  // More than half of the 8 MiB was in use, all of it inside the usable range.
  EXPECT_LE(reinterpret_cast<std::uintptr_t>(stack.low()), d.lowest);
  EXPECT_LT(d.lowest, reinterpret_cast<std::uintptr_t>(stack.high()) - (std::size_t{4} << 20));
  ao::FiberStack::release(std::move(stack));
}

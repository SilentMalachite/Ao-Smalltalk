// AArch64 context switch for ao fibers (Fiber.hpp). Apple arm64 (Mach-O) only.

#if !defined(__aarch64__)
#error "FiberSwitch_arm64.S is AArch64 only"
#endif

#include "FiberRegs.h"

  .text

// void ao_fiber_switch(FiberRegs* save /* x0 */, const FiberRegs* load /* x1 */)
// Stores the callee-saved state (x19-x28, fp, lr, sp, d8-d15) into *save, loads it from *load and
// returns to the loaded lr on the loaded stack. Everything else is caller-saved under AAPCS64, so
// the caller already treats it as clobbered. x18 is the platform register and is left alone.
  .globl _ao_fiber_switch
  .private_extern _ao_fiber_switch
  .p2align 2
_ao_fiber_switch:
  stp x19, x20, [x0, #AO_FIBER_X19]
  stp x21, x22, [x0, #AO_FIBER_X21]
  stp x23, x24, [x0, #AO_FIBER_X23]
  stp x25, x26, [x0, #AO_FIBER_X25]
  stp x27, x28, [x0, #AO_FIBER_X27]
  stp x29, x30, [x0, #AO_FIBER_FP]
  mov x9, sp
  str x9, [x0, #AO_FIBER_SP]
  stp d8, d9, [x0, #AO_FIBER_D8]
  stp d10, d11, [x0, #AO_FIBER_D10]
  stp d12, d13, [x0, #AO_FIBER_D12]
  stp d14, d15, [x0, #AO_FIBER_D14]

  ldp x19, x20, [x1, #AO_FIBER_X19]
  ldp x21, x22, [x1, #AO_FIBER_X21]
  ldp x23, x24, [x1, #AO_FIBER_X23]
  ldp x25, x26, [x1, #AO_FIBER_X25]
  ldp x27, x28, [x1, #AO_FIBER_X27]
  ldp x29, x30, [x1, #AO_FIBER_FP]
  ldr x9, [x1, #AO_FIBER_SP]
  mov sp, x9
  ldp d8, d9, [x1, #AO_FIBER_D8]
  ldp d10, d11, [x1, #AO_FIBER_D10]
  ldp d12, d13, [x1, #AO_FIBER_D12]
  ldp d14, d15, [x1, #AO_FIBER_D14]
  ret

// The first code a fiber runs. fiberInit leaves the entry function in x19, its argument in x20,
// fp 0 and sp at the top of the fiber stack, with lr pointing here. The entry function never
// returns; if it does, trap. The CFI marks this as the outermost frame, so unwinders stop here and
// a C++ exception that escapes the entry function ends in std::terminate.
  .globl _ao_fiber_trampoline
  .private_extern _ao_fiber_trampoline
  .p2align 2
_ao_fiber_trampoline:
  .cfi_startproc
  .cfi_undefined lr
  mov x0, x20
  blr x19
  brk #0
  .cfi_endproc

  .subsections_via_symbols

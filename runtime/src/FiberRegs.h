#pragma once

// Byte offsets into ao::FiberRegs (Fiber.hpp), the register save area of an AArch64 fiber.
// Plain #defines so that FiberSwitch_arm64.S and C++ read the same numbers; Fiber.hpp checks each
// one with static_assert. The layout keeps every register pair adjacent for ldp/stp.
//
// Saved: the AAPCS64 callee-saved state, x19-x28, fp (x29), lr (x30), sp and d8-d15 (the low 64
// bits of v8-v15). x18 is the platform register on Apple and is never touched.

#define AO_FIBER_X19 0
#define AO_FIBER_X21 16
#define AO_FIBER_X23 32
#define AO_FIBER_X25 48
#define AO_FIBER_X27 64
#define AO_FIBER_FP 80
#define AO_FIBER_LR 88
#define AO_FIBER_SP 96
#define AO_FIBER_D8 104
#define AO_FIBER_D10 120
#define AO_FIBER_D12 136
#define AO_FIBER_D14 152
#define AO_FIBER_REGS_SIZE 168

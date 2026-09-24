#pragma once

#include "ao/NativeMethod.hpp"

#include <cstdint>

namespace ao {

class Interpreter {
 public:
  static Oop run(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                 Oop block);
};

// Aborts with "stack overflow" instead of applying when the C stack is nearly used up.
Oop applyMethod(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                Oop block = Oop::nil());
// Sets ctx's stack guard to the current thread's stack, keeping min(512 KiB, size/4) in reserve.
void refreshStackLimit(CallContext& ctx);

// True while Interpreter::run is on this thread's stack. SPEC §3.10: the ABI refuses a call that
// re-enters the runtime then (B10 moves the depth into the CallContext).
bool interpreterRunning();

}  // namespace ao

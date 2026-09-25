#pragma once

#include "ao/NativeMethod.hpp"

#include <string_view>

namespace ao {

// `ctx` is already booted (heap, WellKnown, Bootstrap, cache roots). The files' processes run on
// ctx.scheduler (adoptImage done); without one, no process can run (SPEC §3.4, §4.4).
int runSmalltalkTests(CallContext& ctx, std::string_view path);

}  // namespace ao

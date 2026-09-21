#pragma once

#include "ao/NativeMethod.hpp"

#include <string_view>

namespace ao {

// `ctx` is already booted (heap, WellKnown, Bootstrap, cache roots).
int runSmalltalkTests(CallContext& ctx, std::string_view path);

}  // namespace ao

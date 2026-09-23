#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>

namespace ao {

struct CallContext;

namespace LargeInteger {

// Does not GC. Empty Oop when a LargeInteger does not fit in the nursery.
Oop fromInt64(Heap& heap, WellKnown& wk, std::int64_t value);
// Boxes a LargeInteger with allocateRetry, so it may GC.
Oop fromInt64(CallContext& ctx, std::int64_t value);
bool isLarge(const WellKnown& wk, Oop o);
bool isInteger(const WellKnown& wk, Oop o);
std::int64_t asInt64IfFits(Heap& heap, WellKnown& wk, Oop o, bool* fits);
int compare(Heap& heap, WellKnown& wk, Oop a, Oop b);
Oop add(CallContext& ctx, Oop a, Oop b);
Oop sub(CallContext& ctx, Oop a, Oop b);
Oop mul(CallContext& ctx, Oop a, Oop b);
Oop floorDiv(CallContext& ctx, Oop a, Oop b);
Oop truncDiv(CallContext& ctx, Oop a, Oop b);
Oop modulo(CallContext& ctx, Oop a, Oop b);
Oop remainder(CallContext& ctx, Oop a, Oop b);
Oop bitAnd(CallContext& ctx, Oop a, Oop b);
Oop bitOr(CallContext& ctx, Oop a, Oop b);
Oop bitXor(CallContext& ctx, Oop a, Oop b);
Oop bitShift(CallContext& ctx, Oop a, Oop n);
Oop gcd(CallContext& ctx, Oop a, Oop b);
Oop neg(CallContext& ctx, Oop a);
bool isZero(Heap& heap, WellKnown& wk, Oop o);

}  // namespace LargeInteger
}  // namespace ao

#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <string_view>

namespace ao {

struct CallContext;

namespace LargeInteger {

// Does not GC. Empty Oop when a LargeInteger does not fit in the nursery.
Oop fromInt64(Heap& heap, WellKnown& wk, std::int64_t value);
// Boxes a LargeInteger with allocateRetry, so it may GC.
Oop fromInt64(CallContext& ctx, std::int64_t value);
// The Integer written [-][<radix>r]<digits> (radix 2 to 36, digits 0-9 and A-Z), the text of a
// compiler Int literal outside int64 (SPEC §3.8). A SmallInteger when it fits. Empty Oop when the
// text is not such an Integer or the allocation fails. Boxes with allocateRetry, so it may GC.
Oop fromText(CallContext& ctx, std::string_view text);
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
// SPEC §3.6: the double nearest to num / den (IEEE754 binary64, ties to even, subnormals
// included), ±inf beyond the range. num / den is rounded once, not num and den apart. False when
// num or den is not an Integer or den is 0. Does not allocate on the heap.
bool ratioToDouble(Heap& heap, WellKnown& wk, Oop num, Oop den, double* out);
// SPEC §3.6: the exact order of num / den and d, -1, 0 or 1 in *out; neither is rounded to the
// other. +inf orders above every ratio and -inf below. False when d is NaN (unordered), or num or
// den is not an Integer, or den is 0. Does not allocate on the heap.
bool compareRatioWithDouble(Heap& heap, WellKnown& wk, Oop num, Oop den, double d, int* out);
// The exact order of n1 / d1 and n2 / d2, -1, 0 or 1 in *out. False when one of them is not an
// Integer or a denominator is 0. Does not allocate on the heap.
bool compareRatios(Heap& heap, WellKnown& wk, Oop n1, Oop d1, Oop n2, Oop d2, int* out);
// SPEC §3.6 hash of an Integer: its value when it fits a SmallInteger, otherwise a value hash of
// its sign and magnitude. False when o is not an Integer.
bool valueHash(Heap& heap, WellKnown& wk, Oop o, std::int64_t* out);

}  // namespace LargeInteger
}  // namespace ao

#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Natives.hpp"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace ao {
namespace {

// receiver はルート済みスロット。メッセージの割り当てで GC が走っても正しい。
Oop div0(CallContext& ctx, const Oop& receiver) {
  Oop s = Str::fromUtf8(ctx, "division by zero");
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

bool isFloat(const WellKnown& wk, Oop o) {
  return o.isHeap() && wk.classOf(o) == wk.floatClass;
}

bool isFrac(const WellKnown& wk, Oop o) {
  return o.isHeap() && wk.classOf(o) == wk.fractionClass;
}

Oop fromDouble(CallContext& ctx, double v) {
  Oop o = allocateRetry(ctx, ctx.wk.floatClass, 8, kFlagBytes);
  if (!o.isHeap()) {
    return Oop{};
  }
  std::memcpy(ctx.heap.bytes(o), &v, sizeof(v));
  return o;
}

double asDouble(Heap& heap, Oop o) {
  double v = 0;
  if (heap.size(o) >= 8) {
    std::memcpy(&v, heap.bytes(o), sizeof(v));
  }
  return v;
}

bool asNumDen(CallContext& ctx, Oop o, Oop* num, Oop* den);

// SPEC §3.6: a Float's value, or the Integer or Fraction rounded once to the nearest double (ties
// to even; the whole LargeInteger range, ±inf beyond it). False for anything else.
bool asFloat(CallContext& ctx, Oop o, double* out) {
  if (isFloat(ctx.wk, o)) {
    *out = asDouble(ctx.heap, o);
    return true;
  }
  Oop num;
  Oop den;
  return asNumDen(ctx, o, &num, &den) &&
         LargeInteger::ratioToDouble(ctx.heap, ctx.wk, num, den, out);
}

Oop makeFraction(CallContext& ctx, Oop num, Oop den) {
  Root n(ctx.roots, num);
  Root d(ctx.roots, den);
  if (LargeInteger::isZero(ctx.heap, ctx.wk, d.slot)) {
    return div0(ctx, n.slot);  // div0 はメッセージを割り当てる（GC する）。ルート済みの n を渡す
  }
  Root g(ctx.roots, LargeInteger::gcd(ctx, n.slot, d.slot));
  if (!g.slot.isSmallInteger() && !LargeInteger::isLarge(ctx.wk, g.slot)) {
    return Oop{};
  }
  n.slot = LargeInteger::truncDiv(ctx, n.slot, g.slot);
  d.slot = LargeInteger::truncDiv(ctx, d.slot, g.slot);
  bool dFits = false;
  const std::int64_t dv = LargeInteger::asInt64IfFits(ctx.heap, ctx.wk, d.slot, &dFits);
  if (dFits && dv < 0) {
    n.slot = LargeInteger::neg(ctx, n.slot);
    d.slot = LargeInteger::neg(ctx, d.slot);
  } else if (!dFits) {
    if (d.slot.isHeap() && ctx.heap.klass(d.slot) == ctx.wk.largeNegativeIntegerClass) {
      n.slot = LargeInteger::neg(ctx, n.slot);
      d.slot = LargeInteger::neg(ctx, d.slot);
    }
  }
  bool oneFits = false;
  const std::int64_t one = LargeInteger::asInt64IfFits(ctx.heap, ctx.wk, d.slot, &oneFits);
  if (oneFits && one == 1) {
    return n.slot;
  }
  Oop f = allocateRetry(ctx, ctx.wk.fractionClass, 2, 0);
  if (!f.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(f, 0, n.slot);
  ctx.heap.slotAtPut(f, 1, d.slot);
  return f;
}

bool asNumDen(CallContext& ctx, Oop o, Oop* num, Oop* den) {
  if (LargeInteger::isInteger(ctx.wk, o)) {
    *num = o;
    *den = Oop::fromSmallInteger(1);
    return true;
  }
  if (!isFrac(ctx.wk, o) || ctx.heap.size(o) < 2) {
    return false;
  }
  *num = ctx.heap.slotAt(o, 0);
  *den = ctx.heap.slotAt(o, 1);
  return true;
}

// Exact arithmetic on Integers and Fractions (SPEC §3.6). a and b are read before anything
// allocates.
Oop fracOp(CallContext& ctx, Oop a, Oop b, NumberOp op) {
  Oop na;
  Oop da;
  Oop nb;
  Oop db;
  if (!asNumDen(ctx, a, &na, &da) || !asNumDen(ctx, b, &nb, &db)) {
    return Oop{};
  }
  Root Na(ctx.roots, na);
  Root Da(ctx.roots, da);
  Root Nb(ctx.roots, nb);
  Root Db(ctx.roots, db);
  switch (op) {
    case NumberOp::Add: {
      Root left(ctx.roots, LargeInteger::mul(ctx, Na.slot, Db.slot));
      Root right(ctx.roots, LargeInteger::mul(ctx, Nb.slot, Da.slot));
      Root num(ctx.roots, LargeInteger::add(ctx, left.slot, right.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Db.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
    case NumberOp::Subtract: {
      Root left(ctx.roots, LargeInteger::mul(ctx, Na.slot, Db.slot));
      Root right(ctx.roots, LargeInteger::mul(ctx, Nb.slot, Da.slot));
      Root num(ctx.roots, LargeInteger::sub(ctx, left.slot, right.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Db.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
    case NumberOp::Multiply: {
      Root num(ctx.roots, LargeInteger::mul(ctx, Na.slot, Nb.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Db.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
    case NumberOp::Divide: {
      Root num(ctx.roots, LargeInteger::mul(ctx, Na.slot, Db.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Nb.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
  }
  return Oop{};
}

Oop floatOp(CallContext& ctx, Oop a, Oop b, NumberOp op) {
  double x = 0;
  double y = 0;
  if (!asFloat(ctx, a, &x) || !asFloat(ctx, b, &y)) {
    return Oop{};
  }
  double z = 0;
  switch (op) {
    case NumberOp::Add:
      z = x + y;
      break;
    case NumberOp::Subtract:
      z = x - y;
      break;
    case NumberOp::Multiply:
      z = x * y;
      break;
    case NumberOp::Divide:
      z = x / y;
      break;
  }
  return fromDouble(ctx, z);
}

enum class NumKind { None, Integer, Fraction, Float };

// The place of o in SPEC §3.6's generality order (Integer < Fraction < Float).
NumKind kindOf(CallContext& ctx, Oop o) {
  if (o.isSmallInteger() || LargeInteger::isLarge(ctx.wk, o)) {
    return NumKind::Integer;
  }
  if (isFloat(ctx.wk, o)) {
    return NumKind::Float;
  }
  if (isFrac(ctx.wk, o)) {
    return NumKind::Fraction;
  }
  return NumKind::None;
}

// SPEC §3.6: the exact order of a and b (-1, 0, 1). False when either is not an Integer, Fraction
// or Float. *unordered is set when a NaN is involved; *order is then meaningless.
bool numberOrder(CallContext& ctx, Oop a, Oop b, int* order, bool* unordered) {
  *unordered = false;
  const NumKind ka = kindOf(ctx, a);
  const NumKind kb = kindOf(ctx, b);
  if (ka == NumKind::None || kb == NumKind::None) {
    return false;
  }
  if (ka == NumKind::Integer && kb == NumKind::Integer) {
    *order = LargeInteger::compare(ctx.heap, ctx.wk, a, b);
    return true;
  }
  if (ka == NumKind::Float && kb == NumKind::Float) {
    const double x = asDouble(ctx.heap, a);
    const double y = asDouble(ctx.heap, b);
    *unordered = std::isnan(x) || std::isnan(y);
    *order = x < y ? -1 : (x > y ? 1 : 0);
    return true;
  }
  Oop num;
  Oop den;
  if (ka == NumKind::Float || kb == NumKind::Float) {
    // One exact number and one Float: compare the Float as the exact value it is.
    const bool floatFirst = ka == NumKind::Float;
    const double x = asDouble(ctx.heap, floatFirst ? a : b);
    if (std::isnan(x)) {
      *unordered = true;
      return true;
    }
    if (!asNumDen(ctx, floatFirst ? b : a, &num, &den) ||
        !LargeInteger::compareRatioWithDouble(ctx.heap, ctx.wk, num, den, x, order)) {
      return false;
    }
    if (floatFirst) {
      *order = -*order;
    }
    return true;
  }
  Oop num2;
  Oop den2;
  return asNumDen(ctx, a, &num, &den) && asNumDen(ctx, b, &num2, &den2) &&
         LargeInteger::compareRatios(ctx.heap, ctx.wk, num, den, num2, den2, order);
}

}  // namespace

Oop numberArith(CallContext& ctx, const Oop& a, const Oop& b, NumberOp op) {
  const NumKind ka = kindOf(ctx, a);
  const NumKind kb = kindOf(ctx, b);
  if (ka == NumKind::None || kb == NumKind::None) {
    return Oop{};
  }
  if (ka == NumKind::Float || kb == NumKind::Float) {
    return floatOp(ctx, a, b, op);
  }
  if (op == NumberOp::Divide || ka == NumKind::Fraction || kb == NumKind::Fraction) {
    return fracOp(ctx, a, b, op);
  }
  switch (op) {
    case NumberOp::Add:
      return LargeInteger::add(ctx, a, b);
    case NumberOp::Subtract:
      return LargeInteger::sub(ctx, a, b);
    case NumberOp::Multiply:
      return LargeInteger::mul(ctx, a, b);
    case NumberOp::Divide:
      break;
  }
  return Oop{};
}


Oop numberCompare(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                  NumberRelation rel, NativeFn fallback) {
  if (argc != 1) {
    return Oop{};
  }
  int order = 0;
  bool unordered = false;
  if (!numberOrder(ctx, receiver, args[0], &order, &unordered)) {
    return fallback != nullptr ? fallback(ctx, receiver, args, argc) : Oop{};
  }
  if (unordered) {
    return Oop::false_();
  }
  bool holds = false;
  switch (rel) {
    case NumberRelation::Less:
      holds = order < 0;
      break;
    case NumberRelation::Greater:
      holds = order > 0;
      break;
    case NumberRelation::LessOrEqual:
      holds = order <= 0;
      break;
    case NumberRelation::GreaterOrEqual:
      holds = order >= 0;
      break;
  }
  return holds ? Oop::true_() : Oop::false_();
}

Oop ao_Integer_divide(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Divide);
}

Oop ao_Float_add(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Add);
}

Oop ao_Float_subtract(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Subtract);
}

Oop ao_Float_multiply(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Multiply);
}

Oop ao_Float_divide(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Divide);
}

Oop ao_Float_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (!isFloat(ctx.wk, receiver) || !isFloat(ctx.wk, args[0])) {
    return Oop::false_();
  }
  return asDouble(ctx.heap, receiver) == asDouble(ctx.heap, args[0]) ? Oop::true_()
                                                                     : Oop::false_();
}

Oop ao_Float_lessThan(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::Less, nullptr);
}

Oop ao_Float_greaterThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::Greater,
                       ao_Magnitude_greaterThan);
}

Oop ao_Float_lessOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::LessOrEqual,
                       ao_Magnitude_lessOrEqual);
}

Oop ao_Float_greaterOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::GreaterOrEqual,
                       ao_Magnitude_greaterOrEqual);
}

Oop ao_Fraction_add(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Add);
}

Oop ao_Fraction_subtract(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Subtract);
}

Oop ao_Fraction_multiply(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Multiply);
}

Oop ao_Fraction_divide(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return numberArith(ctx, receiver, args[0], NumberOp::Divide);
}


// SPEC §3.6: two Fractions are equal when their numerators and their denominators are (both are
// normalized, so equal values have equal parts). Anything else only equals itself.
Oop ao_Fraction_equals(CallContext& ctx, const Oop& receiver, const Oop* args,
                       std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (receiver == args[0]) {
    return Oop::true_();
  }
  Oop n1;
  Oop d1;
  Oop n2;
  Oop d2;
  if (!isFrac(ctx.wk, receiver) || !isFrac(ctx.wk, args[0]) || !asNumDen(ctx, receiver, &n1, &d1) ||
      !asNumDen(ctx, args[0], &n2, &d2) || !LargeInteger::isInteger(ctx.wk, n1) ||
      !LargeInteger::isInteger(ctx.wk, d1) || !LargeInteger::isInteger(ctx.wk, n2) ||
      !LargeInteger::isInteger(ctx.wk, d2)) {
    return Oop::false_();
  }
  return LargeInteger::compare(ctx.heap, ctx.wk, n1, n2) == 0 &&
                 LargeInteger::compare(ctx.heap, ctx.wk, d1, d2) == 0
             ? Oop::true_()
             : Oop::false_();
}

Oop ao_Fraction_lessThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::Less, nullptr);
}

Oop ao_Fraction_greaterThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::Greater,
                       ao_Magnitude_greaterThan);
}

Oop ao_Fraction_lessOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::LessOrEqual,
                       ao_Magnitude_lessOrEqual);
}

Oop ao_Fraction_greaterOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  return numberCompare(ctx, receiver, args, argc, NumberRelation::GreaterOrEqual,
                       ao_Magnitude_greaterOrEqual);
}

Oop ao_Float_printString(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!receiver.isHeap() || ctx.wk.classOf(receiver) != ctx.wk.floatClass) {
    return ao_Object_printString(ctx, receiver, nullptr, 0);
  }
  double value = 0;
  if (ctx.heap.size(receiver) >= sizeof(value)) {
    std::memcpy(&value, ctx.heap.bytes(receiver), sizeof(value));
  }
  char buf[64];
  const int wrote = std::snprintf(buf, sizeof(buf), "%g", value);
  if (wrote <= 0 || static_cast<std::size_t>(wrote) >= sizeof(buf)) {
    return Oop{};
  }
  return Str::fromUtf8(ctx, std::string_view(buf, static_cast<std::size_t>(wrote)));
}

namespace kernel {

void installFloat(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.floatClass, "+", 1, "ao_Float_add", ao_Float_add);
  putNative(heap, wk, wk.floatClass, "-", 1, "ao_Float_subtract", ao_Float_subtract);
  putNative(heap, wk, wk.floatClass, "*", 1, "ao_Float_multiply", ao_Float_multiply);
  putNative(heap, wk, wk.floatClass, "/", 1, "ao_Float_divide", ao_Float_divide);
  putNative(heap, wk, wk.floatClass, "=", 1, "ao_Float_equals", ao_Float_equals);
  putNative(heap, wk, wk.floatClass, "<", 1, "ao_Float_lessThan", ao_Float_lessThan);
  putNative(heap, wk, wk.floatClass, ">", 1, "ao_Float_greaterThan", ao_Float_greaterThan);
  putNative(heap, wk, wk.floatClass, "<=", 1, "ao_Float_lessOrEqual", ao_Float_lessOrEqual);
  putNative(heap, wk, wk.floatClass, ">=", 1, "ao_Float_greaterOrEqual", ao_Float_greaterOrEqual);
  putNative(heap, wk, wk.floatClass, "printString", 0, "ao_Float_printString",
            ao_Float_printString);

  putNative(heap, wk, wk.fractionClass, "+", 1, "ao_Fraction_add", ao_Fraction_add);
  putNative(heap, wk, wk.fractionClass, "-", 1, "ao_Fraction_subtract", ao_Fraction_subtract);
  putNative(heap, wk, wk.fractionClass, "*", 1, "ao_Fraction_multiply", ao_Fraction_multiply);
  putNative(heap, wk, wk.fractionClass, "/", 1, "ao_Fraction_divide", ao_Fraction_divide);
  putNative(heap, wk, wk.fractionClass, "=", 1, "ao_Fraction_equals", ao_Fraction_equals);
  putNative(heap, wk, wk.fractionClass, "<", 1, "ao_Fraction_lessThan", ao_Fraction_lessThan);
  putNative(heap, wk, wk.fractionClass, ">", 1, "ao_Fraction_greaterThan",
            ao_Fraction_greaterThan);
  putNative(heap, wk, wk.fractionClass, "<=", 1, "ao_Fraction_lessOrEqual",
            ao_Fraction_lessOrEqual);
  putNative(heap, wk, wk.fractionClass, ">=", 1, "ao_Fraction_greaterOrEqual",
            ao_Fraction_greaterOrEqual);
}

}  // namespace kernel
}  // namespace ao

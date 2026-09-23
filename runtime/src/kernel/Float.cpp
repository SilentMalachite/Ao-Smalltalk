#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Natives.hpp"

#include <cstdio>
#include <cstring>
#include <string_view>

namespace ao {
namespace {

struct Root {
  Roots& roots;
  Oop slot;
  explicit Root(Roots& r, Oop v = Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

Oop allocateRetry(CallContext& ctx, Oop cls, std::uint32_t size, std::uint16_t flags) {
  if (ctx.heap.gcStress() != 0) {
    Root stressed(ctx.roots, cls);
    Gc(ctx.heap, ctx.roots).stressPoint();
    cls = stressed.slot;
  }
  Oop obj = ctx.heap.allocate(cls, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  Root held(ctx.roots, cls);
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  return ctx.heap.allocate(held.slot, size, flags);
}

Oop div0(CallContext& ctx, Oop receiver) {
  Oop s = Str::fromUtf8(ctx.heap, ctx.wk, "division by zero");
  return ao_Object_error_(ctx, receiver, &s, 1);
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

bool asFloat(CallContext& ctx, Oop o, double* out) {
  if (isFloat(ctx.wk, o)) {
    *out = asDouble(ctx.heap, o);
    return true;
  }
  bool fits = false;
  const std::int64_t v = LargeInteger::asInt64IfFits(ctx.heap, ctx.wk, o, &fits);
  if (fits) {
    *out = static_cast<double>(v);
    return true;
  }
  return false;
}

Oop makeFraction(CallContext& ctx, Oop num, Oop den) {
  if (LargeInteger::isZero(ctx.heap, ctx.wk, den)) {
    return div0(ctx, num);
  }
  Root n(ctx.roots, num);
  Root d(ctx.roots, den);
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

enum class FracOp { Add, Sub, Mul, Div };

Oop fracOp(CallContext& ctx, Oop a, Oop b, FracOp op) {
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
    case FracOp::Add: {
      Root left(ctx.roots, LargeInteger::mul(ctx, Na.slot, Db.slot));
      Root right(ctx.roots, LargeInteger::mul(ctx, Nb.slot, Da.slot));
      Root num(ctx.roots, LargeInteger::add(ctx, left.slot, right.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Db.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
    case FracOp::Sub: {
      Root left(ctx.roots, LargeInteger::mul(ctx, Na.slot, Db.slot));
      Root right(ctx.roots, LargeInteger::mul(ctx, Nb.slot, Da.slot));
      Root num(ctx.roots, LargeInteger::sub(ctx, left.slot, right.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Db.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
    case FracOp::Mul: {
      Root num(ctx.roots, LargeInteger::mul(ctx, Na.slot, Nb.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Db.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
    case FracOp::Div: {
      Root num(ctx.roots, LargeInteger::mul(ctx, Na.slot, Db.slot));
      Root den(ctx.roots, LargeInteger::mul(ctx, Da.slot, Nb.slot));
      return makeFraction(ctx, num.slot, den.slot);
    }
  }
  return Oop{};
}

enum class FlOp { Add, Sub, Mul, Div };

Oop floatOp(CallContext& ctx, Oop a, Oop b, FlOp op) {
  double x = 0;
  double y = 0;
  if (!asFloat(ctx, a, &x) || !asFloat(ctx, b, &y)) {
    return Oop{};
  }
  double z = 0;
  switch (op) {
    case FlOp::Add:
      z = x + y;
      break;
    case FlOp::Sub:
      z = x - y;
      break;
    case FlOp::Mul:
      z = x * y;
      break;
    case FlOp::Div:
      z = x / y;
      break;
  }
  return fromDouble(ctx, z);
}

}  // namespace

Oop ao_Integer_divide(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (isFloat(ctx.wk, args[0])) {
    return floatOp(ctx, receiver, args[0], FlOp::Div);
  }
  return fracOp(ctx, receiver, args[0], FracOp::Div);
}

Oop ao_Float_add(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return floatOp(ctx, receiver, args[0], FlOp::Add);
}

Oop ao_Float_subtract(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return floatOp(ctx, receiver, args[0], FlOp::Sub);
}

Oop ao_Float_multiply(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return floatOp(ctx, receiver, args[0], FlOp::Mul);
}

Oop ao_Float_divide(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return floatOp(ctx, receiver, args[0], FlOp::Div);
}

Oop ao_Float_equals(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (!isFloat(ctx.wk, receiver) || !isFloat(ctx.wk, args[0])) {
    return Oop::false_();
  }
  return asDouble(ctx.heap, receiver) == asDouble(ctx.heap, args[0]) ? Oop::true_()
                                                                     : Oop::false_();
}

Oop ao_Float_lessThan(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  double x = 0;
  double y = 0;
  if (!asFloat(ctx, receiver, &x) || !asFloat(ctx, args[0], &y)) {
    return Oop{};
  }
  return x < y ? Oop::true_() : Oop::false_();
}

Oop ao_Fraction_add(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return fracOp(ctx, receiver, args[0], FracOp::Add);
}

Oop ao_Fraction_subtract(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return fracOp(ctx, receiver, args[0], FracOp::Sub);
}

Oop ao_Fraction_multiply(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return fracOp(ctx, receiver, args[0], FracOp::Mul);
}

Oop ao_Fraction_divide(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return fracOp(ctx, receiver, args[0], FracOp::Div);
}

Oop ao_Float_printString(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
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
  return Str::fromUtf8(ctx.heap, ctx.wk, std::string_view(buf, static_cast<std::size_t>(wrote)));
}

namespace kernel {

void installFloat(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.floatClass, "+", 1, "ao_Float_add", ao_Float_add);
  putNative(heap, wk, wk.floatClass, "-", 1, "ao_Float_subtract", ao_Float_subtract);
  putNative(heap, wk, wk.floatClass, "*", 1, "ao_Float_multiply", ao_Float_multiply);
  putNative(heap, wk, wk.floatClass, "/", 1, "ao_Float_divide", ao_Float_divide);
  putNative(heap, wk, wk.floatClass, "=", 1, "ao_Float_equals", ao_Float_equals);
  putNative(heap, wk, wk.floatClass, "<", 1, "ao_Float_lessThan", ao_Float_lessThan);
  putNative(heap, wk, wk.floatClass, "printString", 0, "ao_Float_printString",
            ao_Float_printString);

  putNative(heap, wk, wk.fractionClass, "+", 1, "ao_Fraction_add", ao_Fraction_add);
  putNative(heap, wk, wk.fractionClass, "-", 1, "ao_Fraction_subtract", ao_Fraction_subtract);
  putNative(heap, wk, wk.fractionClass, "*", 1, "ao_Fraction_multiply", ao_Fraction_multiply);
  putNative(heap, wk, wk.fractionClass, "/", 1, "ao_Fraction_divide", ao_Fraction_divide);
}

}  // namespace kernel
}  // namespace ao

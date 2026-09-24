#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <string>

namespace ao {
namespace {

// receiver はルート済みスロット。メッセージの割り当てで GC が走っても正しい。
Oop div0(CallContext& ctx, const Oop& receiver) {
  Oop s = Str::fromUtf8(ctx, "division by zero");
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

Oop asBool(bool v) { return v ? Oop::true_() : Oop::false_(); }

bool bothInts(const WellKnown& wk, Oop a, Oop b) {
  return LargeInteger::isInteger(wk, a) && LargeInteger::isInteger(wk, b);
}

// SPEC §3.6: to:do: whose receiver or limit is not a SmallInteger (a Float, Fraction or
// LargeInteger limit, a LargeInteger receiver). As the inlined loop does (SPEC §3.5), it sends
// `i <= limit` before each pass, treats a non-Boolean answer as a branch does (mustBeBoolean),
// and steps the Integer i by 1. receiver and args are rooted slots; i lives in a Root.
Oop toDoSendingLessOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args) {
  if (!LargeInteger::isInteger(ctx.wk, receiver)) {
    return Oop{};
  }
  Root i(ctx.roots, receiver);
  Root lessOrEqual(ctx.roots, ctx.wk.intern("<="));
  Gc gc(ctx.heap, ctx.roots);
  for (std::uint64_t pass = 1;; ++pass) {
    const Oop more = send(ctx, i.slot, lessOrEqual.slot, &args[0], 1, nullptr);
    if (unwinding(ctx)) {
      return Oop{};
    }
    if (more.isEmpty()) {
      // SPEC §3.3: the failing send is <=, as in the inlined loop.
      return abortFailedSend(ctx, lessOrEqual.slot);
    }
    bool truth = false;
    if (!truthOf(ctx, more, &truth)) {
      return Oop{};
    }
    if (!truth) {
      return receiver;
    }
    Oop ignored;
    if (!callBlock(ctx, args[1], &i.slot, 1, &ignored)) {
      return Oop{};
    }
    i.slot = LargeInteger::add(ctx, i.slot, Oop::fromSmallInteger(1));
    if (i.slot.isEmpty()) {
      return Oop{};
    }
    if ((pass & 0xFFFF) == 0) {
      gc.safepoint();
    }
  }
}

}  // namespace

Oop ao_SmallInteger_add(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (bothInts(ctx.wk, receiver, args[0])) {
    return LargeInteger::add(ctx, receiver, args[0]);
  }
  // SPEC §3.6: a Fraction or Float argument answers in its type.
  return numberArith(ctx, receiver, args[0], NumberOp::Add);
}

Oop ao_Integer_subtract(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (bothInts(ctx.wk, receiver, args[0])) {
    return LargeInteger::sub(ctx, receiver, args[0]);
  }
  // SPEC §3.6: a Fraction or Float argument answers in its type.
  return numberArith(ctx, receiver, args[0], NumberOp::Subtract);
}

Oop ao_Integer_multiply(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (bothInts(ctx.wk, receiver, args[0])) {
    return LargeInteger::mul(ctx, receiver, args[0]);
  }
  // SPEC §3.6: a Fraction or Float argument answers in its type.
  return numberArith(ctx, receiver, args[0], NumberOp::Multiply);
}

Oop ao_Integer_intDivide(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  if (LargeInteger::isZero(ctx.heap, ctx.wk, args[0])) {
    return div0(ctx, receiver);
  }
  return LargeInteger::floorDiv(ctx, receiver, args[0]);
}

Oop ao_Integer_modulo(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  if (LargeInteger::isZero(ctx.heap, ctx.wk, args[0])) {
    return div0(ctx, receiver);
  }
  return LargeInteger::modulo(ctx, receiver, args[0]);
}

Oop ao_Integer_quo_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  if (LargeInteger::isZero(ctx.heap, ctx.wk, args[0])) {
    return div0(ctx, receiver);
  }
  return LargeInteger::truncDiv(ctx, receiver, args[0]);
}

Oop ao_Integer_rem_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  if (LargeInteger::isZero(ctx.heap, ctx.wk, args[0])) {
    return div0(ctx, receiver);
  }
  return LargeInteger::remainder(ctx, receiver, args[0]);
}

Oop ao_Integer_bitAnd_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return LargeInteger::bitAnd(ctx, receiver, args[0]);
}

Oop ao_Integer_bitOr_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return LargeInteger::bitOr(ctx, receiver, args[0]);
}

Oop ao_Integer_bitXor_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return LargeInteger::bitXor(ctx, receiver, args[0]);
}

Oop ao_Integer_bitShift_(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  if (argc != 1 || !LargeInteger::isInteger(ctx.wk, receiver) ||
      !LargeInteger::isInteger(ctx.wk, args[0])) {
    return Oop{};
  }
  return LargeInteger::bitShift(ctx, receiver, args[0]);
}

Oop ao_Integer_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (!bothInts(ctx.wk, receiver, args[0])) {
    return Oop::false_();
  }
  return asBool(LargeInteger::compare(ctx.heap, ctx.wk, receiver, args[0]) == 0);
}

Oop ao_Integer_lessThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc == 1 && bothInts(ctx.wk, receiver, args[0])) {
    return asBool(LargeInteger::compare(ctx.heap, ctx.wk, receiver, args[0]) < 0);
  }
  // SPEC §3.6: a Fraction or Float compares by exact value; anything else fails.
  return numberCompare(ctx, receiver, args, argc, NumberRelation::Less, nullptr);
}

Oop ao_Integer_greaterThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc) {
  if (argc == 1 && bothInts(ctx.wk, receiver, args[0])) {
    return asBool(LargeInteger::compare(ctx.heap, ctx.wk, receiver, args[0]) > 0);
  }
  return numberCompare(ctx, receiver, args, argc, NumberRelation::Greater,
                       ao_Magnitude_greaterThan);
}

Oop ao_Integer_lessOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc) {
  if (argc == 1 && bothInts(ctx.wk, receiver, args[0])) {
    return asBool(LargeInteger::compare(ctx.heap, ctx.wk, receiver, args[0]) <= 0);
  }
  return numberCompare(ctx, receiver, args, argc, NumberRelation::LessOrEqual,
                       ao_Magnitude_lessOrEqual);
}

Oop ao_Integer_greaterOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc == 1 && bothInts(ctx.wk, receiver, args[0])) {
    return asBool(LargeInteger::compare(ctx.heap, ctx.wk, receiver, args[0]) >= 0);
  }
  return numberCompare(ctx, receiver, args, argc, NumberRelation::GreaterOrEqual,
                       ao_Magnitude_greaterOrEqual);
}


// SPEC §3.6: equal Integers hash equally. A SmallInteger is its own hash.
Oop ao_Integer_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  std::int64_t h = 0;
  if (!LargeInteger::valueHash(ctx.heap, ctx.wk, receiver, &h)) {
    return ao_Object_identityHash(ctx, receiver, args, argc);
  }
  return Oop::fromSmallInteger(h);
}

Oop ao_Integer_to_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root start(ctx.roots, receiver);
  Root stop(ctx.roots, args[0]);
  Oop iv = allocateRetry(ctx, ctx.wk.intervalClass, 3, 0);
  if (!iv.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(iv, 0, start.slot);
  ctx.heap.slotAtPut(iv, 1, stop.slot);
  ctx.heap.slotAtPut(iv, 2, Oop::fromSmallInteger(1));
  return iv;
}

Oop ao_Integer_to_do_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  if (!receiver.isSmallInteger() || !args[0].isSmallInteger()) {
    return toDoSendingLessOrEqual(ctx, receiver, args);
  }
  const auto start = receiver.smallIntegerValue();
  const auto stop = args[0].smallIntegerValue();
  Root blk(ctx.roots, args[1]);
  Gc gc(ctx.heap, ctx.roots);
  for (std::int64_t i = start; i <= stop; ++i) {
    Oop n = Oop::fromSmallInteger(i);
    Oop ignored;
    if (!callBlock(ctx, blk.slot, &n, 1, &ignored)) {
      return Oop{};
    }
    if ((i & 0xFFFF) == 0) {
      gc.safepoint();
    }
  }
  return receiver;
}

// Evaluates the block receiver times and answers the receiver (SPEC §3.6).
Oop ao_Integer_timesRepeat_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1 || !receiver.isSmallInteger()) {
    return Oop{};
  }
  const auto count = receiver.smallIntegerValue();
  Gc gc(ctx.heap, ctx.roots);
  Oop ignored;
  for (std::int64_t i = 1; i <= count; ++i) {
    if (!callBlock(ctx, args[0], nullptr, 0, &ignored)) {
      return Oop{};
    }
    if ((i & 0xFFFF) == 0) {
      gc.safepoint();
    }
  }
  return receiver;
}

// SPEC §3.6: Unicode scalar values only (SPEC §5.9): 0 to 0x10FFFF without the surrogates.
Oop ao_Integer_asCharacter(CallContext&, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isSmallInteger()) {
    return Oop{};
  }
  const auto v = receiver.smallIntegerValue();
  if (v < 0 || v > 0x10FFFF || (v >= 0xD800 && v <= 0xDFFF)) {
    return Oop{};
  }
  return Oop::fromCharacter(static_cast<char32_t>(v));
}

Oop ao_SmallInteger_printString(CallContext& ctx, const Oop& receiver, const Oop*,
                                std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!receiver.isSmallInteger()) {
    return ao_Object_printString(ctx, receiver, nullptr, 0);
  }
  return Str::fromUtf8(ctx, std::to_string(receiver.smallIntegerValue()));
}

namespace kernel {

void installInteger(Heap& heap, WellKnown& wk) {
  struct Spec {
    const char* selector;
    std::uint32_t argc;
    const char* name;
    NativeFn fn;
  };
  const Spec specs[] = {
      {"+", 1, "ao_SmallInteger_add", ao_SmallInteger_add},
      {"-", 1, "ao_Integer_subtract", ao_Integer_subtract},
      {"*", 1, "ao_Integer_multiply", ao_Integer_multiply},
      {"//", 1, "ao_Integer_intDivide", ao_Integer_intDivide},
      {"\\\\", 1, "ao_Integer_modulo", ao_Integer_modulo},
      {"quo:", 1, "ao_Integer_quo_", ao_Integer_quo_},
      {"rem:", 1, "ao_Integer_rem_", ao_Integer_rem_},
      {"bitAnd:", 1, "ao_Integer_bitAnd_", ao_Integer_bitAnd_},
      {"bitOr:", 1, "ao_Integer_bitOr_", ao_Integer_bitOr_},
      {"bitXor:", 1, "ao_Integer_bitXor_", ao_Integer_bitXor_},
      {"bitShift:", 1, "ao_Integer_bitShift_", ao_Integer_bitShift_},
      {"=", 1, "ao_Integer_equals", ao_Integer_equals},
      {"hash", 0, "ao_Integer_hash", ao_Integer_hash},
      {"<", 1, "ao_Integer_lessThan", ao_Integer_lessThan},
      {">", 1, "ao_Integer_greaterThan", ao_Integer_greaterThan},
      {"<=", 1, "ao_Integer_lessOrEqual", ao_Integer_lessOrEqual},
      {">=", 1, "ao_Integer_greaterOrEqual", ao_Integer_greaterOrEqual},
      {"to:", 1, "ao_Integer_to_", ao_Integer_to_},
      {"to:do:", 2, "ao_Integer_to_do_", ao_Integer_to_do_},
      {"timesRepeat:", 1, "ao_Integer_timesRepeat_", ao_Integer_timesRepeat_},
      {"/", 1, "ao_Integer_divide", ao_Integer_divide},
      {"asCharacter", 0, "ao_Integer_asCharacter", ao_Integer_asCharacter},
  };
  for (const auto& s : specs) {
    putNative(heap, wk, wk.integerClass, s.selector, s.argc, s.name, s.fn);
  }
  putNative(heap, wk, wk.smallIntegerClass, "+", 1, "ao_SmallInteger_add", ao_SmallInteger_add);
  putNative(heap, wk, wk.smallIntegerClass, "printString", 0, "ao_SmallInteger_printString",
            ao_SmallInteger_printString);
}

}  // namespace kernel
}  // namespace ao

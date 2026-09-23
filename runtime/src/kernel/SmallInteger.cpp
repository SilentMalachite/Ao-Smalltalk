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

}  // namespace

Oop ao_SmallInteger_add(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return LargeInteger::add(ctx, receiver, args[0]);
}

Oop ao_Integer_subtract(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return LargeInteger::sub(ctx, receiver, args[0]);
}

Oop ao_Integer_multiply(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return LargeInteger::mul(ctx, receiver, args[0]);
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
  if (argc != 1 || !bothInts(ctx.wk, receiver, args[0])) {
    return Oop{};
  }
  return asBool(LargeInteger::compare(ctx.heap, ctx.wk, receiver, args[0]) < 0);
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
  if (argc != 2 || !receiver.isSmallInteger() || !args[0].isSmallInteger()) {
    return Oop{};
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

Oop ao_Integer_asCharacter(CallContext&, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isSmallInteger()) {
    return Oop{};
  }
  const auto v = receiver.smallIntegerValue();
  if (v < 0 || v > 0x10FFFF) {
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
      {"<", 1, "ao_Integer_lessThan", ao_Integer_lessThan},
      {"to:", 1, "ao_Integer_to_", ao_Integer_to_},
      {"to:do:", 2, "ao_Integer_to_do_", ao_Integer_to_do_},
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

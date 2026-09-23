#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

namespace ao {
namespace {

Oop sendBin(CallContext& ctx, Oop rcvr, const char* sel, Oop arg) {
  Oop s = Symbol::intern(ctx.wk, sel);
  return send(ctx, rcvr, s, &arg, 1, nullptr);
}

}  // namespace

Oop ao_Magnitude_greaterThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return sendBin(ctx, args[0], "<", receiver);
}

Oop ao_Magnitude_lessOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  const Oop lt = sendBin(ctx, receiver, "<", args[0]);
  if (lt.isTrue()) {
    return Oop::true_();
  }
  if (!lt.isFalse()) {
    return lt;
  }
  return sendBin(ctx, receiver, "=", args[0]);
}

Oop ao_Magnitude_greaterOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  const Oop lt = sendBin(ctx, receiver, "<", args[0]);
  if (lt.isTrue()) {
    return Oop::false_();
  }
  if (lt.isFalse()) {
    return Oop::true_();
  }
  return lt;
}

Oop ao_Magnitude_between_and_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  const Oop ge = sendBin(ctx, receiver, ">=", args[0]);
  if (!ge.isTrue()) {
    return ge.isFalse() ? Oop::false_() : ge;
  }
  return sendBin(ctx, receiver, "<=", args[1]);
}

namespace kernel {

void installMagnitude(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.magnitudeClass;
  putNative(heap, wk, cls, ">", 1, "ao_Magnitude_greaterThan", ao_Magnitude_greaterThan);
  putNative(heap, wk, cls, "<=", 1, "ao_Magnitude_lessOrEqual", ao_Magnitude_lessOrEqual);
  putNative(heap, wk, cls, ">=", 1, "ao_Magnitude_greaterOrEqual", ao_Magnitude_greaterOrEqual);
  putNative(heap, wk, cls, "between:and:", 2, "ao_Magnitude_between_and_",
            ao_Magnitude_between_and_);
}

}  // namespace kernel
}  // namespace ao

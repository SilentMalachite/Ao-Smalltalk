#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

namespace ao {
namespace {

Oop sendBin(CallContext& ctx, const Oop& rcvr, const char* sel, const Oop& arg) {
  Oop s = Symbol::intern(ctx.wk, sel);
  return send(ctx, rcvr, s, &arg, 1, nullptr);
}

// Blue Book: the receiver when `self > aMagnitude` (max:) or `self < aMagnitude` (min:) is true,
// the argument when it is false. Any other answer fails (SPEC §3.6). receiver and args are rooted
// slots: read after the send, they are where the GC moved them.
Oop pickBy(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
           const char* sel) {
  if (argc != 1) {
    return Oop{};
  }
  const Oop pick = sendBin(ctx, receiver, sel, args[0]);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (pick.isTrue()) {
    return receiver;
  }
  return pick.isFalse() ? args[0] : Oop{};
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


Oop ao_Magnitude_max_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return pickBy(ctx, receiver, args, argc, ">");
}

Oop ao_Magnitude_min_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return pickBy(ctx, receiver, args, argc, "<");
}

namespace kernel {

void installMagnitude(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.magnitudeClass;
  putNative(heap, wk, cls, ">", 1, "ao_Magnitude_greaterThan", ao_Magnitude_greaterThan);
  putNative(heap, wk, cls, "<=", 1, "ao_Magnitude_lessOrEqual", ao_Magnitude_lessOrEqual);
  putNative(heap, wk, cls, ">=", 1, "ao_Magnitude_greaterOrEqual", ao_Magnitude_greaterOrEqual);
  putNative(heap, wk, cls, "between:and:", 2, "ao_Magnitude_between_and_",
            ao_Magnitude_between_and_);
  putNative(heap, wk, cls, "max:", 1, "ao_Magnitude_max_", ao_Magnitude_max_);
  putNative(heap, wk, cls, "min:", 1, "ao_Magnitude_min_", ao_Magnitude_min_);
}

}  // namespace kernel
}  // namespace ao

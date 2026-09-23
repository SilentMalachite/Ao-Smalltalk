#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

namespace ao {
namespace {

Oop sendValue(CallContext& ctx, Oop block) {
  return send(ctx, block, ctx.wk.selValue, nullptr, 0, nullptr);
}

Oop asBool(bool v) { return v ? Oop::true_() : Oop::false_(); }

}  // namespace

Oop ao_Boolean_subclassResponsibility(CallContext& ctx, const Oop&, const Oop*, std::uint32_t) {
  return Str::fromUtf8(ctx, "subclassResponsibility");
}

Oop ao_True_ifTrue_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_True_ifFalse_(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return Oop::nil();
}

Oop ao_True_ifTrue_ifFalse_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_True_ifFalse_ifTrue_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  return sendValue(ctx, args[1]);
}

Oop ao_True_and_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_True_or_(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return Oop::true_();
}

Oop ao_True_not(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::false_();
}

Oop ao_True_and(CallContext&, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return asBool(args[0].isTrue());
}

Oop ao_True_or(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return Oop::true_();
}

Oop ao_True_eqv_(CallContext&, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return asBool(args[0].isTrue());
}

Oop ao_True_xor_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  const Oop eqv = ao_True_eqv_(ctx, receiver, args, argc);
  return eqv.isTrue() ? Oop::false_() : Oop::true_();
}

Oop ao_False_ifTrue_(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return Oop::nil();
}

Oop ao_False_ifFalse_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_False_ifTrue_ifFalse_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  return sendValue(ctx, args[1]);
}

Oop ao_False_ifFalse_ifTrue_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_False_and_(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return Oop::false_();
}

Oop ao_False_or_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_False_not(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::true_();
}

Oop ao_False_and(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return Oop::false_();
}

Oop ao_False_or(CallContext&, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return asBool(args[0].isTrue());
}

Oop ao_False_eqv_(CallContext&, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return asBool(args[0].isFalse());
}

Oop ao_False_xor_(CallContext&, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return asBool(args[0].isTrue());
}

Oop ao_True_printString(CallContext& ctx, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Str::fromUtf8(ctx, "true");
}

Oop ao_False_printString(CallContext& ctx, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Str::fromUtf8(ctx, "false");
}

namespace kernel {

void installBoolean(Heap& heap, WellKnown& wk) {
  struct Spec {
    const char* selector;
    std::uint32_t argc;
    const char* trueName;
    NativeFn trueFn;
    const char* falseName;
    NativeFn falseFn;
  };
  const Spec specs[] = {
      {"ifTrue:", 1, "ao_True_ifTrue_", ao_True_ifTrue_, "ao_False_ifTrue_", ao_False_ifTrue_},
      {"ifFalse:", 1, "ao_True_ifFalse_", ao_True_ifFalse_, "ao_False_ifFalse_", ao_False_ifFalse_},
      {"ifTrue:ifFalse:", 2, "ao_True_ifTrue_ifFalse_", ao_True_ifTrue_ifFalse_,
       "ao_False_ifTrue_ifFalse_", ao_False_ifTrue_ifFalse_},
      {"ifFalse:ifTrue:", 2, "ao_True_ifFalse_ifTrue_", ao_True_ifFalse_ifTrue_,
       "ao_False_ifFalse_ifTrue_", ao_False_ifFalse_ifTrue_},
      {"and:", 1, "ao_True_and_", ao_True_and_, "ao_False_and_", ao_False_and_},
      {"or:", 1, "ao_True_or_", ao_True_or_, "ao_False_or_", ao_False_or_},
      {"not", 0, "ao_True_not", ao_True_not, "ao_False_not", ao_False_not},
      {"&", 1, "ao_True_and", ao_True_and, "ao_False_and", ao_False_and},
      {"|", 1, "ao_True_or", ao_True_or, "ao_False_or", ao_False_or},
      {"eqv:", 1, "ao_True_eqv_", ao_True_eqv_, "ao_False_eqv_", ao_False_eqv_},
      {"xor:", 1, "ao_True_xor_", ao_True_xor_, "ao_False_xor_", ao_False_xor_},
  };
  for (const auto& s : specs) {
    putNative(heap, wk, wk.booleanClass, s.selector, s.argc, "ao_Boolean_subclassResponsibility",
              ao_Boolean_subclassResponsibility);
    putNative(heap, wk, wk.trueClass, s.selector, s.argc, s.trueName, s.trueFn);
    putNative(heap, wk, wk.falseClass, s.selector, s.argc, s.falseName, s.falseFn);
  }
  putNative(heap, wk, wk.trueClass, "printString", 0, "ao_True_printString", ao_True_printString);
  putNative(heap, wk, wk.falseClass, "printString", 0, "ao_False_printString",
            ao_False_printString);
}

}  // namespace kernel
}  // namespace ao

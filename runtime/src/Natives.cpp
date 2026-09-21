#include "ao/Natives.hpp"

namespace ao {

Oop ao_Object_identityEquals(CallContext&, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return receiver == args[0] ? Oop::true_() : Oop::false_();
}

Oop ao_Object_class(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return ctx.wk.classOf(receiver);
}

Oop ao_Object_doesNotUnderstand_(CallContext&, Oop, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return args[0];
}

}  // namespace ao

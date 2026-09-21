#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"

namespace ao {

Oop ao_Character_equals(CallContext&, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return receiver == args[0] ? Oop::true_() : Oop::false_();
}

Oop ao_Character_lessThan(CallContext&, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !receiver.isCharacter() || !args[0].isCharacter()) {
    return Oop{};
  }
  return receiver.characterValue() < args[0].characterValue() ? Oop::true_() : Oop::false_();
}

Oop ao_Character_asciiValue(CallContext&, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isCharacter()) {
    return Oop{};
  }
  return Oop::fromSmallInteger(static_cast<std::int64_t>(receiver.characterValue()));
}

Oop ao_Character_asInteger(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Character_asciiValue(ctx, receiver, args, argc);
}

Oop ao_Character_asCharacter(CallContext&, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return receiver;
}

namespace kernel {

void installCharacter(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.characterClass;
  putNative(heap, wk, cls, "=", 1, "ao_Character_equals", ao_Character_equals);
  putNative(heap, wk, cls, "<", 1, "ao_Character_lessThan", ao_Character_lessThan);
  putNative(heap, wk, cls, "asciiValue", 0, "ao_Character_asciiValue", ao_Character_asciiValue);
  putNative(heap, wk, cls, "asInteger", 0, "ao_Character_asInteger", ao_Character_asInteger);
  putNative(heap, wk, cls, "asCharacter", 0, "ao_Character_asCharacter", ao_Character_asCharacter);
}

}  // namespace kernel
}  // namespace ao

#pragma once

#include "ao/NativeMethod.hpp"

namespace ao {

Oop ao_Object_identityEquals(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_class(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_doesNotUnderstand_(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_SmallInteger_add(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);

}  // namespace ao

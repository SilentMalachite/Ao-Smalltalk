#pragma once

#include "ao/NativeMethod.hpp"

namespace ao {

Oop ao_Object_identityEquals(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_identityNotEquals(CallContext& ctx, Oop receiver, const Oop* args,
                                std::uint32_t argc);
Oop ao_Object_equals(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_class(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_hash(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_identityHash(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_yourself(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_isNil(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_notNil(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_ifNil_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_ifNotNil_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_perform_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_perform_with_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_perform_withArguments_(CallContext& ctx, Oop receiver, const Oop* args,
                                     std::uint32_t argc);
Oop ao_Object_doesNotUnderstand_(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_Object_error_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_subclassResponsibility(CallContext& ctx, Oop receiver, const Oop* args,
                                     std::uint32_t argc);
Oop ao_Object_shouldNotImplement(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_Object_isKindOf_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_isMemberOf_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_respondsTo_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_copy(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_shallowCopy(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_instVarAt_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_instVarAt_put_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_instVarNamed_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_basicSize(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_basicAt_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_basicAt_put_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_printString(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_printOn_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_storeOn_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_inspect(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);

Oop ao_UndefinedObject_isNil(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_UndefinedObject_notNil(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_UndefinedObject_ifNil_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);
Oop ao_UndefinedObject_ifNotNil_(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc);

Oop ao_SmallInteger_add(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);

}  // namespace ao

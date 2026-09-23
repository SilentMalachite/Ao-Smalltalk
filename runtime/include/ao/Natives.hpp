#pragma once

#include "ao/NativeMethod.hpp"

namespace ao {

Oop ao_Object_identityEquals(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_Object_identityNotEquals(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc);
Oop ao_Object_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_class(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_identityHash(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc);
Oop ao_Object_yourself(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_isNil(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_notNil(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_ifNil_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_ifNotNil_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_perform_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_perform_with_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc);
Oop ao_Object_perform_withArguments_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                     std::uint32_t argc);
Oop ao_Object_doesNotUnderstand_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_Object_error_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_subclassResponsibility(CallContext& ctx, const Oop& receiver, const Oop* args,
                                     std::uint32_t argc);
Oop ao_Object_shouldNotImplement(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_Object_isKindOf_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_isMemberOf_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_Object_respondsTo_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_Object_copy(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_shallowCopy(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_Object_instVarAt_(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc);
Oop ao_Object_instVarAt_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_Object_instVarNamed_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc);
Oop ao_Object_basicSize(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_basicAt_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_basicAt_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc);
Oop ao_Object_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_Object_printOn_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_storeOn_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_inspect(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);

Oop ao_UndefinedObject_isNil(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_UndefinedObject_notNil(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc);
Oop ao_UndefinedObject_ifNil_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc);
Oop ao_UndefinedObject_ifNotNil_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_UndefinedObject_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                                   std::uint32_t argc);

Oop ao_True_printString(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_False_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc);

Oop ao_SmallInteger_add(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_SmallInteger_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc);
Oop ao_Integer_divide(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);

Oop ao_Character_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_String_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_Array_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc);
Oop ao_Array_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Float_printString(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc);

}  // namespace ao

#pragma once

#include "ao/NativeMethod.hpp"

#include <cstddef>
#include <cstdint>

namespace ao {

// Association layout. The LitVar bytecodes read and write the value slot (SPEC §3.5).
inline constexpr std::uint32_t kAssocKey = 0;
inline constexpr std::uint32_t kAssocValue = 1;

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
Oop ao_Object_halt(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_error_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Object_mustBeBoolean(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc);
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

// SPEC §3.6 数の演算: a op b for any mix of Integer, Fraction and Float, in the more general type
// (Integer < Fraction < Float). Empty Oop when either is not one of them; an Integer or Fraction
// divided by 0 aborts with "division by zero". a and b are rooted slots (a native's receiver and
// arguments): the answer is allocated after both are read.
enum class NumberOp { Add, Subtract, Multiply, Divide };
Oop numberArith(CallContext& ctx, const Oop& a, const Oop& b, NumberOp op);

// SPEC §3.6 数の比較: receiver rel args[0] by exact value for any mix of Integer, Fraction and
// Float; a NaN answers false. When either is none of them, answers what fallback answers for the
// same receiver and arguments (nullptr: the empty Oop, a failure). Does not allocate.
enum class NumberRelation { Less, Greater, LessOrEqual, GreaterOrEqual };
Oop numberCompare(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                  NumberRelation rel, NativeFn fallback);

// SPEC §3.6 = と hash: a value hash is FNV-1a (64 bit) over the parts of the value, folded to a
// SmallInteger in [0, 2^30). It depends on the contents only, never on an address, so it is the
// same across GCs and images.
inline constexpr std::uint64_t kValueHashSeed = 14695981039346656037ull;
inline std::uint64_t valueHashBytes(std::uint64_t h, const unsigned char* bytes, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    h = (h ^ bytes[i]) * 1099511628211ull;
  }
  return h;
}
inline std::uint64_t valueHashWord(std::uint64_t h, std::uint64_t word) {
  for (int i = 0; i < 8; ++i) {
    h = (h ^ ((word >> (8 * i)) & 0xFFu)) * 1099511628211ull;
  }
  return h;
}
inline std::int64_t valueHashFold(std::uint64_t h) {
  return static_cast<std::int64_t>((h ^ (h >> 30) ^ (h >> 60)) & 0x3FFFFFFFu);
}
// The value hash of a byte sequence: what String>>hash and Symbol>>hash answer for those bytes.
inline std::int64_t bytesValueHash(const unsigned char* bytes, std::size_t n) {
  return valueHashFold(valueHashBytes(kValueHashSeed, bytes, n));
}
// The Array and Point hash natives send hash to their elements only while fewer than
// kMaxHashNesting of them are doing so (CallContext::hashNesting); an Array sends it to its first
// kMaxHashElements elements.
inline constexpr std::uint32_t kMaxHashNesting = 4;
inline constexpr std::uint32_t kMaxHashElements = 16;
struct HashNesting {
  explicit HashNesting(CallContext& c) : ctx(c) { ++ctx.hashNesting; }
  ~HashNesting() { --ctx.hashNesting; }
  HashNesting(const HashNesting&) = delete;
  HashNesting& operator=(const HashNesting&) = delete;
  CallContext& ctx;
};
// Sends hash to obj and puts the answer in *out as an int64 (SPEC §3.6): a SmallInteger's value,
// a LargeInteger's value hash (what its hash answers). Unless the hash obj finds is the Kernel
// Array or Point hash native, hashNesting is 0 during the send and restored after it, so the
// answer does not depend on the depth it is asked from. False when the frames unwind (SPEC §3.4)
// or the answer is not an Integer (a failed send's empty Oop included): the caller then answers
// the empty Oop. May GC. For any native that needs an object's hash: Dictionary, Set.
bool sendHash(CallContext& ctx, Oop obj, std::int64_t* out);
// Sends hash to element (sendHash) and mixes the answer into *h. False as sendHash is.
bool mixElementHash(CallContext& ctx, Oop element, std::uint64_t* h);
Oop ao_Array_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_Point_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);

// Magnitude's defaults (SPEC §3.6): > sends <, <= sends < and =, >= answers the negation of <.
Oop ao_Magnitude_greaterThan(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_Magnitude_lessOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_Magnitude_greaterOrEqual(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc);

// True when the method klass finds for selector (method cache, then lookup) is the native fn. A
// Kernel shortcut that bypasses a send (String>>do:, stream contents and nextPut: on a String)
// takes it only then, so a subclass that overrides the method still gets its send (SPEC §3.6).
bool findsNative(CallContext& ctx, Oop klass, Oop selector, NativeFn fn);

Oop ao_ArrayedCollection_do_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_ArrayedCollection_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_OrderedCollection_size(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc);
Oop ao_OrderedCollection_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc);
Oop ao_String_size(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_String_at_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);
Oop ao_String_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc);

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

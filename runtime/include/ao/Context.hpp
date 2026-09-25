#pragma once

#include "ao/NativeMethod.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace ao {

inline constexpr std::uint32_t kCtxSender   = 0;
inline constexpr std::uint32_t kCtxPc       = 1;
inline constexpr std::uint32_t kCtxStackp   = 2;
inline constexpr std::uint32_t kCtxMethod   = 3;
inline constexpr std::uint32_t kCtxReceiver = 4;
inline constexpr std::uint32_t kCtxArgc     = 5;
inline constexpr std::uint32_t kBlockHome   = 6;
inline constexpr std::uint32_t kBlockCopied = 7;
inline constexpr std::uint32_t kBlockSlotCount = 8;
inline constexpr std::uint32_t kMethodContextSlotCount = 6;

namespace Context {
Oop createMethod(CallContext& ctx, Oop method, Oop receiver, Oop sender, std::uint8_t argc);
Oop createBlock(CallContext& ctx, Oop method, Oop receiver, Oop home, Oop copied, Oop sender,
                std::uint8_t argc);
}

// A BlockContext whose method is a NativeMethod calling fn (a native block thunk). SPEC §3.11: the
// method takes the name fn was registered under (the Kernel registers its thunk functions at
// install), so an image holding the thunk rebinds it at load. A thunk of an unregistered function
// is named ao_NativeBlock_thunk, which does not resolve: saving a heap that holds one fails.
Oop makeNativeBlock(CallContext& ctx, NativeFn fn, std::uint32_t argc);

Oop ao_BlockContext_value(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_BlockContext_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc);
Oop ao_BlockContext_value_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_BlockContext_valueWithArguments_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                        std::uint32_t argc);
Oop ao_BlockContext_value_value_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                       std::uint32_t argc);
Oop ao_BlockContext_value_value_value_value_(CallContext& ctx, const Oop& receiver,
                                             const Oop* args, std::uint32_t argc);
Oop ao_BlockContext_numArgs(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc);
Oop ao_BlockContext_whileTrue_(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc);
Oop ao_BlockContext_whileFalse_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc);
Oop ao_BlockContext_whileTrue(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc);
Oop ao_BlockContext_whileFalse(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc);
Oop ao_BlockContext_repeat(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc);
Oop ao_BlockContext_ensure_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc);
Oop ao_BlockContext_ifCurtailed_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_BlockContext_cannotReturn_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc);

namespace Str {
// Does not GC. Empty Oop when the nursery (or old, for a large string) is full.
Oop fromUtf8(Heap& heap, WellKnown& wk, std::string_view utf8);
// Allocates with allocateRetry, so it may GC. utf8 must not point into the heap.
Oop fromUtf8(CallContext& ctx, std::string_view utf8);
std::string toUtf8(Heap& heap, Oop str);
std::uint32_t codePointCount(Heap& heap, Oop str);
Oop at(Heap& heap, Oop str, std::int64_t oneBased);
// The UTF-8 of a Unicode scalar value in out: 1 to 4 bytes, 0 for a surrogate or past U+10FFFF.
std::uint32_t encodeUtf8(char32_t cp, unsigned char out[4]);
// The bytes of the character at p (remaining > 0) as String>>at: decodes it: 1 to 4, and 1 for a
// byte that starts no valid sequence.
std::uint32_t charBytes(const unsigned char* p, std::uint32_t remaining);
// The byte offset where character chars + 1 starts in the n bytes at p, decoding as String>>at:
// does (n when there are exactly chars characters), or -1 when there are fewer. One pass over the
// bytes it skips.
std::int64_t byteOffsetOfChar(const unsigned char* p, std::uint32_t n, std::int64_t chars);
}

namespace Arr {
Oop fromSlots(Heap& heap, WellKnown& wk, const Oop* p, std::uint32_t n);
}

}  // namespace ao

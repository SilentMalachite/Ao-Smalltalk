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

Oop makeNativeBlock(CallContext& ctx, NativeFn fn, std::uint32_t argc);

Oop ao_BlockContext_value(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc);
Oop ao_BlockContext_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc);
Oop ao_BlockContext_value_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc);
Oop ao_BlockContext_valueWithArguments_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                        std::uint32_t argc);

namespace Str {
// Does not GC. Empty Oop when the nursery (or old, for a large string) is full.
Oop fromUtf8(Heap& heap, WellKnown& wk, std::string_view utf8);
// Allocates with allocateRetry, so it may GC. utf8 must not point into the heap.
Oop fromUtf8(CallContext& ctx, std::string_view utf8);
std::string toUtf8(Heap& heap, Oop str);
std::uint32_t codePointCount(Heap& heap, Oop str);
Oop at(Heap& heap, Oop str, std::int64_t oneBased);
}

namespace Arr {
Oop fromSlots(Heap& heap, WellKnown& wk, const Oop* p, std::uint32_t n);
}

}  // namespace ao

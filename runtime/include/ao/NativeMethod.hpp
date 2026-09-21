#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <string_view>

namespace ao {

inline constexpr std::uint32_t kNativeSlotSelector      = 0;
inline constexpr std::uint32_t kNativeSlotArgc          = 1;
inline constexpr std::uint32_t kNativeSlotPrimitive     = 2;
inline constexpr std::uint32_t kNativeSlotName          = 3;
inline constexpr std::uint32_t kNativeSlotMethodClass   = 4;
inline constexpr std::uint32_t kNativeSlotRegistryIndex = 5;
inline constexpr std::uint32_t kNativeSlotCount         = 6;

struct ClassMethodCache;

struct CallContext {
  Heap& heap;
  Roots& roots;
  WellKnown& wk;
  ClassMethodCache* cache;
};

using NativeFn = Oop (*)(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);

namespace NativeRegistry {
std::uint32_t add(NativeFn fn);
}

namespace NativeMethod {
Oop create(Heap& heap, WellKnown& wk, Oop selector, std::uint32_t argc, std::string_view name,
           std::uint32_t registryIndex, Oop methodClass);
Oop apply(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc);
std::string_view nameBytes(Heap& heap, Oop method);
}

}  // namespace ao

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

struct ClassMethodCache {
  static constexpr std::uint32_t kSize = 256;
  struct Entry {
    Oop klass{};
    Oop selector{};
    Oop method{};
  };
  Entry entries[kSize]{};
  void addRoots(Roots& roots);
  Oop probe(Heap& heap, Oop klass, Oop selector) const;
  void insert(Heap& heap, Oop klass, Oop selector, Oop method);
  void forget(Heap& heap, Oop klass, Oop selector);
};

struct CallContext;

using HostOopHook = void (*)(CallContext& ctx, Oop value);

struct CallContext {
  Heap& heap;
  Roots& roots;
  WellKnown& wk;
  ClassMethodCache* cache;
  HostOopHook inspectHook = nullptr;
  HostOopHook transcriptHook = nullptr;
  Oop activeContext{};
  std::uint64_t interpretedBytecodes = 0;
  int testFailures = 0;
  bool nonlocalReturn = false;
  Oop nonlocalHome{};
  Oop nonlocalValue{};
};

using NativeFn = Oop (*)(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc);

namespace NativeRegistry {
std::uint32_t add(NativeFn fn);
bool addNamed(std::string_view name, NativeFn fn, std::uint32_t* outIndex);
bool findName(std::string_view name, std::uint32_t* outIndex);
std::uint32_t size();
}

namespace NativeMethod {
Oop create(Heap& heap, WellKnown& wk, Oop selector, std::uint32_t argc, std::string_view name,
           std::uint32_t registryIndex, Oop methodClass);
Oop apply(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc);
std::string_view nameBytes(Heap& heap, Oop method);
}

}  // namespace ao

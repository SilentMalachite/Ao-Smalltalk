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
  // Drops every entry for selector, whatever the receiver's class. Does not GC.
  void flushSelector(Oop selector);
  // Drops every entry. Does not GC.
  void flushAll();
};

// SPEC §3.3 (キャッシュの無効化): the one function every change of what a send finds goes
// through. A method added to or replaced in a dictionary (installMethod: Browser accept, file-in
// method chunks; putNative) passes its selector, and every entry for it is dropped. A class
// definition, which may replace the class a name meant, passes the empty Oop, and the whole
// cache is dropped. A null cache (outside a session, Bootstrap) has nothing to drop. Does not GC.
void invalidateMethodCache(ClassMethodCache* cache, Oop selector);

struct CallContext;

using HostOopHook = void (*)(CallContext& ctx, Oop value);
// Finds or makes the workspace binding (an Association) for name; empty Oop on failure.
using BindingHook = Oop (*)(CallContext& ctx, std::string_view name);

struct CallContext {
  Heap& heap;
  Roots& roots;
  WellKnown& wk;
  ClassMethodCache* cache;
  HostOopHook inspectHook = nullptr;
  HostOopHook transcriptHook = nullptr;
  Oop activeContext{};
  std::uint64_t interpretedBytecodes = 0;
  // Sends the interpreter made for Send, SendSuper and SendSpecial (performSend). A SendSpecial
  // that SmallIntegers answer without a send (SPEC §3.5) does not count; mustBeBoolean and
  // cannotReturn: do not either.
  std::uint64_t interpretedSends = 0;
  int testFailures = 0;
  bool nonlocalReturn = false;
  Oop nonlocalHome{};
  Oop nonlocalValue{};
  // SPEC §3.4 abort: a non-local return with no home. Only abortEvaluation sets it; the
  // outermost entry reads the reason (abortReasonText) and clears it. The reason is either a
  // static string (abortReason, set without allocating) or a heap String held in the roots'
  // handle table (abortReasonHandle), so it survives GCs until it is cleared.
  static constexpr std::uint32_t kNoAbortReasonHandle = 0xFFFFFFFFu;
  bool aborting = false;
  const char* abortReason = nullptr;
  std::uint32_t abortReasonHandle = kNoAbortReasonHandle;
  // Stack guard (SPEC §3.4) of the thread that last applied a method: a method may be applied
  // at a frame address in [stackLimit, stackHigh]. Outside it, applyMethod refreshes them, and
  // every outermost entry refreshes them too (a new thread may reuse an old thread's stack).
  // While an ensure: cleanup runs (cleanupDepth > 0) the lower stackCleanupLimit applies, so a
  // cleanup still runs during a stack overflow abort.
  std::uintptr_t stackLimit = 0;
  std::uintptr_t stackHigh = 0;
  std::uintptr_t stackCleanupLimit = 0;
  std::uint32_t cleanupDepth = 0;
  // Boxes LitKind::Binding literals (SPEC §3.10). Null outside a session: such literals fail.
  BindingHook bindingHook = nullptr;
  // SPEC §3.6 = と hash: how many Array and Point hash natives are sending hash to their elements
  // right now (HashNesting). At the limit they stop sending, so a self-holding Array ends.
  std::uint32_t hashNesting = 0;
};

using NativeFn = Oop (*)(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc);

namespace NativeRegistry {
std::uint32_t add(NativeFn fn);
bool addNamed(std::string_view name, NativeFn fn, std::uint32_t* outIndex);
bool findName(std::string_view name, std::uint32_t* outIndex);
// The name the function at idx was first registered under (addNamed), or empty. The view is valid
// until the next add or addNamed.
std::string_view nameAt(std::uint32_t idx);
std::uint32_t size();
}

namespace NativeMethod {
// Does not GC (allocateNoGc). With old at its max: sets the out-of-memory flag, empty Oop.
Oop create(Heap& heap, WellKnown& wk, Oop selector, std::uint32_t argc, std::string_view name,
           std::uint32_t registryIndex, Oop methodClass);
Oop apply(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc);
// Calls fn with receiver and args copied into rooted slots. Use it for native-to-native calls
// whose receiver or arguments are locals of the caller.
Oop invoke(CallContext& ctx, NativeFn fn, Oop receiver, const Oop* args, std::uint32_t argc);
std::string_view nameBytes(Heap& heap, Oop method);
}

}  // namespace ao

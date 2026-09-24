#include "ao/NativeMethod.hpp"

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

namespace ao {

namespace {

std::vector<NativeFn> gNativeFns;
std::vector<std::string> gNativeNames;
std::unordered_map<std::string, std::uint32_t> gNativeByName;

}  // namespace

namespace NativeRegistry {

std::uint32_t add(NativeFn fn) {
  for (std::uint32_t i = 0; i < gNativeFns.size(); ++i) {
    if (gNativeFns[i] == fn) {
      return i;
    }
  }
  const auto i = static_cast<std::uint32_t>(gNativeFns.size());
  gNativeFns.push_back(fn);
  gNativeNames.emplace_back();
  return i;
}

bool addNamed(std::string_view name, NativeFn fn, std::uint32_t* outIndex) {
  const auto idx = add(fn);
  const std::string key(name);
  const auto it = gNativeByName.find(key);
  if (it != gNativeByName.end()) {
    if (it->second != idx) {
      return false;
    }
    if (outIndex != nullptr) {
      *outIndex = idx;
    }
    return true;
  }
  if (gNativeNames[idx].empty()) {
    gNativeNames[idx] = key;
  }
  gNativeByName.emplace(key, idx);
  if (outIndex != nullptr) {
    *outIndex = idx;
  }
  return true;
}

bool findName(std::string_view name, std::uint32_t* outIndex) {
  const auto it = gNativeByName.find(std::string(name));
  if (it == gNativeByName.end()) {
    return false;
  }
  if (outIndex != nullptr) {
    *outIndex = it->second;
  }
  return true;
}

std::string_view nameAt(std::uint32_t idx) {
  if (idx >= gNativeNames.size()) {
    return {};
  }
  return gNativeNames[idx];
}

std::uint32_t size() { return static_cast<std::uint32_t>(gNativeFns.size()); }

}  // namespace NativeRegistry

namespace NativeMethod {

Oop create(Heap& heap, WellKnown& wk, Oop selector, std::uint32_t argc, std::string_view name,
           std::uint32_t registryIndex, Oop methodClass) {
  // Does not GC: the nursery first, then old when it is full. Fails only with old at its max,
  // which is out of memory (SPEC §3.2).
  auto meth = heap.allocateNoGc(wk.nativeMethodClass, kNativeSlotCount, 0);
  if (!meth.isHeap()) {
    heap.setOutOfMemory();
    return Oop{};
  }
  const auto n = static_cast<std::uint32_t>(name.size());
  auto nameObj = heap.allocateNoGc(Oop::nil(), n, kFlagBytes);
  if (!nameObj.isHeap()) {
    heap.setOutOfMemory();
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(heap.bytes(nameObj), name.data(), n);
  }
  heap.slotAtPut(meth, kNativeSlotSelector, selector);
  heap.slotAtPut(meth, kNativeSlotArgc, Oop::fromSmallInteger(static_cast<std::int64_t>(argc)));
  heap.slotAtPut(meth, kNativeSlotPrimitive, Oop::fromSmallInteger(0));
  heap.slotAtPut(meth, kNativeSlotName, nameObj);
  heap.slotAtPut(meth, kNativeSlotMethodClass, methodClass);
  heap.slotAtPut(meth, kNativeSlotRegistryIndex,
                 Oop::fromSmallInteger(static_cast<std::int64_t>(registryIndex)));
  return meth;
}

Oop apply(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (!method.isHeap()) {
    return Oop{};
  }
  if (ctx.heap.klass(method) != ctx.wk.nativeMethodClass) {
    return Oop{};
  }
  const auto storedArgc = ctx.heap.slotAt(method, kNativeSlotArgc);
  if (!storedArgc.isSmallInteger() ||
      storedArgc.smallIntegerValue() != static_cast<std::int64_t>(argc)) {
    return Oop{};
  }
  const auto idxOop = ctx.heap.slotAt(method, kNativeSlotRegistryIndex);
  if (!idxOop.isSmallInteger()) {
    return Oop{};
  }
  const auto idx = idxOop.smallIntegerValue();
  if (idx < 0 || static_cast<std::size_t>(idx) >= gNativeFns.size()) {
    return Oop{};
  }
  NativeFn fn = gNativeFns[static_cast<std::size_t>(idx)];
  if (fn == nullptr) {
    return Oop{};
  }
  return invoke(ctx, fn, receiver, args, argc);
}

Oop invoke(CallContext& ctx, NativeFn fn, Oop receiver, const Oop* args, std::uint32_t argc) {
  // Slot 0 is the receiver, slots 1..argc the arguments. The native reads them through references
  // into these rooted slots, so every GC inside it updates what it sees.
  Oop* frame = ctx.roots.pushFrame(receiver, args, argc);
  const Oop result = fn(ctx, frame[0], frame + 1, argc);
  ctx.roots.popFrame(frame, argc);
  return result;
}

std::string_view nameBytes(Heap& heap, Oop method) {
  if (!method.isHeap()) {
    return {};
  }
  const auto name = heap.slotAt(method, kNativeSlotName);
  if (!name.isHeap()) {
    return {};
  }
  const ObjectHeader* h = heap.header(name);
  return std::string_view(reinterpret_cast<const char*>(h + 1), h->size);
}


NativeFn functionOf(const Heap& heap, const WellKnown& wk, Oop method) {
  if (!method.isHeap() || heap.klass(method) != wk.nativeMethodClass ||
      heap.size(method) < kNativeSlotCount) {
    return nullptr;
  }
  const auto idxOop = heap.slotAt(method, kNativeSlotRegistryIndex);
  if (!idxOop.isSmallInteger()) {
    return nullptr;
  }
  const auto idx = idxOop.smallIntegerValue();
  if (idx < 0 || static_cast<std::size_t>(idx) >= gNativeFns.size()) {
    return nullptr;
  }
  return gNativeFns[static_cast<std::size_t>(idx)];
}

}  // namespace NativeMethod
}  // namespace ao

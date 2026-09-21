#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

namespace ao {

namespace Arr {

Oop fromSlots(Heap& heap, WellKnown& wk, const Oop* p, std::uint32_t n) {
  auto a = heap.allocate(wk.arrayClass, n, 0);
  if (!a.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < n; ++i) {
    heap.slotAtPut(a, i, p[i]);
  }
  return a;
}

}  // namespace Arr

Oop ao_ArrayedCollection_size(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Object_basicSize(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_at_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Object_basicAt_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_at_put_(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc) {
  return ao_Object_basicAt_put_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_basicAt_(CallContext& ctx, Oop receiver, const Oop* args,
                                  std::uint32_t argc) {
  return ao_Object_basicAt_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_basicAt_put_(CallContext& ctx, Oop receiver, const Oop* args,
                                      std::uint32_t argc) {
  return ao_Object_basicAt_put_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_new_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return send(ctx, receiver, ctx.wk.selBasicNew_, args, 1, nullptr);
}

namespace kernel {

void installArray(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.arrayedCollectionClass;
  putNative(heap, wk, cls, "size", 0, "ao_ArrayedCollection_size", ao_ArrayedCollection_size);
  putNative(heap, wk, cls, "at:", 1, "ao_ArrayedCollection_at_", ao_ArrayedCollection_at_);
  putNative(heap, wk, cls, "at:put:", 2, "ao_ArrayedCollection_at_put_",
            ao_ArrayedCollection_at_put_);
  putNative(heap, wk, cls, "basicAt:", 1, "ao_ArrayedCollection_basicAt_",
            ao_ArrayedCollection_basicAt_);
  putNative(heap, wk, cls, "basicAt:put:", 2, "ao_ArrayedCollection_basicAt_put_",
            ao_ArrayedCollection_basicAt_put_);
  putNative(heap, wk, wk.arrayedCollectionMetaclass, "new:", 1, "ao_ArrayedCollection_new_",
            ao_ArrayedCollection_new_);
}

}  // namespace kernel
}  // namespace ao

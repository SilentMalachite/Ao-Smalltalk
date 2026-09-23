#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <string>

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

namespace {

constexpr int kArrayPrintMaxDepth = 4;

struct Root {
  Roots& roots;
  Oop slot;
  explicit Root(Roots& r, Oop v = Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

bool isArray(const CallContext& ctx, Oop obj) {
  return obj.isHeap() && ctx.wk.classOf(obj) == ctx.wk.arrayClass;
}

Oop bytesFrom(CallContext& ctx, std::string_view text) {
  Oop s = Str::fromUtf8(ctx.heap, ctx.wk, text);
  if (s.isHeap()) {
    return s;
  }
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  return Str::fromUtf8(ctx.heap, ctx.wk, text);
}

// Depth 1 is the outermost array. Above 4, print "..." and do not send printString.
Oop arrayPrintString(CallContext& ctx, Oop receiver, int depth) {
  if (depth > kArrayPrintMaxDepth) {
    return bytesFrom(ctx, "...");
  }
  Root arr(ctx.roots, receiver);
  Root sel(ctx.roots, ctx.wk.intern("printString"));
  if (!sel.slot.isHeap()) {
    return Oop{};
  }
  const auto n = ctx.heap.size(arr.slot);
  std::string text;
  text.append("#(");
  for (std::uint32_t i = 0; i < n; ++i) {
    if (i != 0) {
      text.push_back(' ');
    }
    Root elt(ctx.roots, ctx.heap.slotAt(arr.slot, i));
    Oop printed =
        isArray(ctx, elt.slot) ? arrayPrintString(ctx, elt.slot, depth + 1)
                               : send(ctx, elt.slot, sel.slot, nullptr, 0, nullptr);
    if (!printed.isHeap() || (ctx.heap.flags(printed) & kFlagBytes) == 0) {
      return Oop{};
    }
    text += Str::toUtf8(ctx.heap, printed);
  }
  text.push_back(')');
  return bytesFrom(ctx, text);
}

}  // namespace

Oop ao_Array_printString(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!isArray(ctx, receiver)) {
    return ao_Object_printString(ctx, receiver, nullptr, 0);
  }
  return arrayPrintString(ctx, receiver, 1);
}

Oop ao_Array_equals(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (!isArray(ctx, receiver) || !isArray(ctx, args[0])) {
    return Oop::false_();
  }
  Root left(ctx.roots, receiver);
  Root right(ctx.roots, args[0]);
  const auto n = ctx.heap.size(left.slot);
  if (ctx.heap.size(right.slot) != n) {
    return Oop::false_();
  }
  Root sel(ctx.roots, ctx.wk.intern("="));
  if (!sel.slot.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < n; ++i) {
    Root a(ctx.roots, ctx.heap.slotAt(left.slot, i));
    Root b(ctx.roots, ctx.heap.slotAt(right.slot, i));
    const Oop eq = send(ctx, a.slot, sel.slot, &b.slot, 1, nullptr);
    if (!eq.isTrue()) {
      return Oop::false_();
    }
  }
  return Oop::true_();
}

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
  putNative(heap, wk, wk.arrayClass, "printString", 0, "ao_Array_printString",
            ao_Array_printString);
  putNative(heap, wk, wk.arrayClass, "=", 1, "ao_Array_equals", ao_Array_equals);
}

}  // namespace kernel
}  // namespace ao

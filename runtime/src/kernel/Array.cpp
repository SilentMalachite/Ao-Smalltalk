#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <cstring>
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

bool isArray(const CallContext& ctx, Oop obj) {
  return obj.isHeap() && ctx.wk.classOf(obj) == ctx.wk.arrayClass;
}

Oop bytesFrom(CallContext& ctx, std::string_view text) {
  const auto n = static_cast<std::uint32_t>(text.size());
  const Oop s = allocateRetry(ctx, ctx.wk.stringClass, n, kFlagBytes);
  if (s.isHeap() && n != 0) {
    std::memcpy(ctx.heap.bytes(s), text.data(), n);
  }
  return s;
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

Oop ao_Array_printString(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!isArray(ctx, receiver)) {
    return ao_Object_printString(ctx, receiver, nullptr, 0);
  }
  return arrayPrintString(ctx, receiver, 1);
}

Oop ao_Array_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  // An array that holds itself is equal to itself without comparing its elements.
  if (receiver == args[0]) {
    return Oop::true_();
  }
  // Same class (an Array subclass too), as `class ==`. Pointer slots only.
  if (!receiver.isHeap() || (ctx.heap.flags(receiver) & kFlagBytes) != 0 ||
      !args[0].isHeap() || ctx.wk.classOf(args[0]) != ctx.wk.classOf(receiver)) {
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
    if (unwinding(ctx)) {
      return Oop{};
    }
    if (!eq.isTrue()) {
      return Oop::false_();
    }
  }
  return Oop::true_();
}


namespace {

// True when the hash method element finds is the Kernel Array or Point hash native (SPEC §3.6).
bool findsKernelNestingHash(CallContext& ctx, Oop element, Oop selector) {
  // An immediate's class is never an Array or a Point, so its hash is not one of these natives.
  if (!element.isHeap()) {
    return false;
  }
  const Oop klass = ctx.wk.classOf(element);
  Oop method = ctx.cache != nullptr ? ctx.cache->probe(ctx.heap, klass, selector) : Oop{};
  if (!method.isHeap()) {
    method = lookup(ctx.heap, klass, selector);
  }
  const NativeFn fn = NativeMethod::functionOf(ctx.heap, ctx.wk, method);
  return fn == ao_Array_hash || fn == ao_Point_hash;
}

// Sets CallContext::hashNesting to 0 for the scope when reset, and puts the outer count back when
// the scope ends, whatever path leaves it (a failure, an unwind).
struct SavedHashNesting {
  SavedHashNesting(CallContext& c, bool reset) : ctx(c), outer(c.hashNesting) {
    if (reset) {
      ctx.hashNesting = 0;
    }
  }
  ~SavedHashNesting() { ctx.hashNesting = outer; }
  SavedHashNesting(const SavedHashNesting&) = delete;
  SavedHashNesting& operator=(const SavedHashNesting&) = delete;
  CallContext& ctx;
  std::uint32_t outer;
};

}  // namespace

bool sendHash(CallContext& ctx, Oop obj, std::int64_t* out) {
  Root e(ctx.roots, obj);
  const Oop selector = ctx.wk.intern("hash");
  Oop answer;
  {
    // SPEC §3.6: only Kernel Array and Point hashes nested in each other count toward the limit.
    // Any other hash (a user method, another native) runs from 0, whatever depth asks for it.
    const SavedHashNesting saved(ctx, !findsKernelNestingHash(ctx, e.slot, selector));
    answer = send(ctx, e.slot, selector, nullptr, 0, nullptr);
  }
  if (unwinding(ctx)) {
    return false;
  }
  return LargeInteger::valueHash(ctx.heap, ctx.wk, answer, out);
}

bool mixElementHash(CallContext& ctx, Oop element, std::uint64_t* h) {
  std::int64_t v = 0;
  if (!sendHash(ctx, element, &v)) {
    return false;
  }
  *h = valueHashWord(*h, static_cast<std::uint64_t>(v));
  return true;
}

// SPEC §3.6: from the size and the hashes of the first kMaxHashElements elements, as Array>>=
// compares them. Past kMaxHashNesting nested element hashes, from the size alone.
Oop ao_Array_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!receiver.isHeap() || (ctx.heap.flags(receiver) & kFlagBytes) != 0) {
    return ao_Object_identityHash(ctx, receiver, args, argc);
  }
  const std::uint32_t n = ctx.heap.size(receiver);
  std::uint64_t h = valueHashWord(kValueHashSeed, n);
  if (ctx.hashNesting < kMaxHashNesting) {
    HashNesting nesting(ctx);
    for (std::uint32_t i = 0; i < n && i < kMaxHashElements; ++i) {
      // receiver is a rooted slot: after each send it is where the GC moved it.
      if (!mixElementHash(ctx, ctx.heap.slotAt(receiver, i), &h)) {
        return Oop{};
      }
    }
  }
  return Oop::fromSmallInteger(valueHashFold(h));
}

Oop ao_ArrayedCollection_size(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  return ao_Object_basicSize(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  return ao_Object_basicAt_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc) {
  return ao_Object_basicAt_put_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_basicAt_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc) {
  return ao_Object_basicAt_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_basicAt_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                      std::uint32_t argc) {
  return ao_Object_basicAt_put_(ctx, receiver, args, argc);
}

Oop ao_ArrayedCollection_new_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
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
  putNative(heap, wk, wk.arrayClass, "hash", 0, "ao_Array_hash", ao_Array_hash);
}

}  // namespace kernel
}  // namespace ao

#include "ao/Send.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Lookup.hpp"
#include "ao/Symbol.hpp"

namespace ao {
namespace {

struct IcGuard {
  Roots& roots;
  InlineCache* ic;
  IcGuard(Roots& r, InlineCache* i) : roots(r), ic(i) {
    if (ic != nullptr) {
      roots.add(&ic->cachedClass);
      roots.add(&ic->cachedMethod);
    }
  }
  ~IcGuard() {
    if (ic != nullptr) {
      roots.remove(&ic->cachedClass);
      roots.remove(&ic->cachedMethod);
    }
  }
  IcGuard(const IcGuard&) = delete;
  IcGuard& operator=(const IcGuard&) = delete;
};

std::uint32_t cacheIndex(Heap& heap, Oop klass, Oop selector) {
  const auto hk = klass.isHeap() ? static_cast<std::uint32_t>(heap.hash(klass)) : 0u;
  const auto hs = selector.isHeap() ? static_cast<std::uint32_t>(heap.hash(selector)) : 0u;
  return (hk * 33u ^ hs) & 255u;
}

bool icMatches(CallContext& ctx, InlineCache* ic, Oop klass, Oop selector) {
  if (ic == nullptr || ic->cachedClass != klass || !ic->cachedMethod.isHeap()) {
    return false;
  }
  // Same IC object may be reused for another selector; NativeMethod slot 0 is the selector.
  return ctx.heap.slotAt(ic->cachedMethod, kNativeSlotSelector) == selector;
}

void fillCaches(CallContext& ctx, InlineCache* ic, Oop klass, Oop selector, Oop method) {
  if (ic != nullptr) {
    ic->cachedClass = klass;
    ic->cachedMethod = method;
  }
  if (ctx.cache != nullptr) {
    ctx.cache->insert(ctx.heap, klass, selector, method);
  }
}

Oop allocateMessage(Heap& heap, WellKnown& wk, Oop selector, const Oop* args, std::uint32_t argc) {
  auto msg = heap.allocate(wk.messageClass, 2, 0);
  if (!msg.isHeap()) {
    return Oop{};
  }
  heap.slotAtPut(msg, 0, selector);
  auto arr = heap.allocate(Oop::nil(), argc, 0);
  if (!arr.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < argc; ++i) {
    heap.slotAtPut(arr, i, args[i]);
  }
  heap.slotAtPut(msg, 1, arr);
  return msg;
}

Oop doesNotUnderstand(CallContext& ctx, Oop receiver, Oop selector, const Oop* args,
                      std::uint32_t argc) {
  auto msg = allocateMessage(ctx.heap, ctx.wk, selector, args, argc);
  if (!msg.isHeap()) {
    return Oop{};
  }
  auto dnuSel = Symbol::intern(ctx.wk, "doesNotUnderstand:");
  if (!dnuSel.isHeap()) {
    return msg;
  }
  auto meth = lookup(ctx.heap, ctx.wk.classOf(receiver), dnuSel);
  if (!meth.isHeap()) {
    return msg;
  }
  return NativeMethod::apply(ctx, meth, receiver, &msg, 1);
}

}  // namespace

void ClassMethodCache::addRoots(Roots& roots) {
  for (auto& e : entries) {
    roots.add(&e.klass);
    roots.add(&e.selector);
    roots.add(&e.method);
  }
}

Oop ClassMethodCache::probe(Heap& heap, Oop klass, Oop selector) const {
  const auto& e = entries[cacheIndex(heap, klass, selector)];
  if (e.klass == klass && e.selector == selector) {
    return e.method;
  }
  return Oop{};
}

void ClassMethodCache::insert(Heap& heap, Oop klass, Oop selector, Oop method) {
  auto& e = entries[cacheIndex(heap, klass, selector)];
  e.klass = klass;
  e.selector = selector;
  e.method = method;
}

Oop send(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
         InlineCache* ic) {
  IcGuard guard(ctx.roots, ic);
  const Oop klass = ctx.wk.classOf(receiver);
  if (icMatches(ctx, ic, klass, selector)) {
    return NativeMethod::apply(ctx, ic->cachedMethod, receiver, args, argc);
  }
  if (ctx.cache != nullptr) {
    const Oop cached = ctx.cache->probe(ctx.heap, klass, selector);
    if (cached.isHeap()) {
      fillCaches(ctx, ic, klass, selector, cached);
      return NativeMethod::apply(ctx, cached, receiver, args, argc);
    }
  }
  const Oop meth = lookup(ctx.heap, klass, selector);
  if (!meth.isHeap()) {
    return doesNotUnderstand(ctx, receiver, selector, args, argc);
  }
  fillCaches(ctx, ic, klass, selector, meth);
  return NativeMethod::apply(ctx, meth, receiver, args, argc);
}

Oop sendSuper(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
              Oop methodClass) {
  Oop start = Oop::nil();
  if (methodClass.isHeap()) {
    start = ctx.heap.slotAt(methodClass, kClassSlotSuperclass);
  }
  const Oop meth = lookup(ctx.heap, start, selector);
  if (!meth.isHeap()) {
    return doesNotUnderstand(ctx, receiver, selector, args, argc);
  }
  return NativeMethod::apply(ctx, meth, receiver, args, argc);
}

}  // namespace ao

#include "ao/Send.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/DebugSnapshot.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Lookup.hpp"
#include "ao/Scheduler.hpp"
#include "ao/Symbol.hpp"

#include <iterator>
#include <new>
#include <string>

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
  // NativeMethod selector is slot 0. CompiledMethod slot 0 is the header; its selector is slot 4.
  const Oop meth = ic->cachedMethod;
  const Oop mk = ctx.heap.klass(meth);
  if (mk == ctx.wk.nativeMethodClass) {
    return ctx.heap.slotAt(meth, kNativeSlotSelector) == selector;
  }
  if (mk == ctx.wk.compiledMethodClass) {
    return ctx.heap.slotAt(meth, kCmSlotSelector) == selector;
  }
  return false;
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

// in[0] is the receiver, in[1] the selector and in[2..] the arguments, all rooted.
Oop allocateMessage(CallContext& ctx, const RootedArray& in, std::uint32_t argc) {
  Root msg(ctx.roots, allocateRetry(ctx, ctx.wk.messageClass, 2, 0));
  if (!msg.slot.isHeap()) {
    return Oop{};
  }
  const Oop arr = allocateRetry(ctx, ctx.wk.arrayClass, argc, 0);
  if (!arr.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < argc; ++i) {
    ctx.heap.slotAtPut(arr, i, in[i + 2]);
  }
  ctx.heap.slotAtPut(msg.slot, 0, in[1]);
  ctx.heap.slotAtPut(msg.slot, 1, arr);
  return msg.slot;
}

Oop doesNotUnderstand(CallContext& ctx, Oop receiver, Oop selector, const Oop* args,
                      std::uint32_t argc) {
  // Building the Message can collect, so everything it holds is rooted first.
  RootedArray in(ctx.roots, argc + 2);
  in[0] = receiver;
  in[1] = selector;
  for (std::uint32_t i = 0; i < argc; ++i) {
    in[i + 2] = args[i];
  }
  // SPEC §3.3: without a Message or a doesNotUnderstand: to send it to, the send aborts as the
  // default doesNotUnderstand: would. Only out of memory stops the Message, and then the reason
  // cannot be allocated either: it becomes "out of memory".
  Root msg(ctx.roots, allocateMessage(ctx, in, argc));
  if (!msg.slot.isHeap()) {
    return abortDoesNotUnderstand(ctx, in[1]);
  }
  const Oop dnuSel = Symbol::intern(ctx.wk, "doesNotUnderstand:");
  if (!dnuSel.isHeap()) {
    return abortDoesNotUnderstand(ctx, in[1]);
  }
  const Oop meth = lookup(ctx.heap, ctx.wk.classOf(in[0]), dnuSel);
  if (!meth.isHeap()) {
    return abortDoesNotUnderstand(ctx, in[1]);
  }
  return applyMethod(ctx, meth, in[0], &msg.slot, 1);
}

void dropNonlocal(CallContext& ctx) {
  ctx.nonlocalReturn = false;
  ctx.nonlocalHome = Oop{};
  ctx.nonlocalValue = Oop{};
}

// Aborts with prefix followed by the selector's bytes, or with the static fallback when selector
// is not a Symbol. The bytes are copied out before the reason is allocated.
Oop abortWithSelector(CallContext& ctx, const char* prefix, Oop selector, const char* fallback) {
  if (!selector.isHeap() || (ctx.heap.flags(selector) & kFlagBytes) == 0) {
    return abortEvaluation(ctx, fallback);
  }
  std::string reason(prefix);
  reason += Str::toUtf8(ctx.heap, selector);
  return abortEvaluation(ctx, std::string_view(reason));
}

// value, value:, ... for up to four arguments; the empty Oop beyond that.
Oop valueSelector(CallContext& ctx, std::uint32_t n) {
  static constexpr const char* kValueSelectors[] = {
      "value", "value:", "value:value:", "value:value:value:", "value:value:value:value:"};
  if (n >= std::size(kValueSelectors)) {
    return Oop{};
  }
  return n == 0   ? ctx.wk.selValue
         : n == 1 ? ctx.wk.selValue_
                  : ctx.wk.intern(kValueSelectors[n]);
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

void ClassMethodCache::flushSelector(Oop selector) {
  // The index hashes the receiver's class too, so the selector's entries may be in any row.
  for (auto& e : entries) {
    if (e.selector == selector) {
      e = Entry{};
    }
  }
}

void ClassMethodCache::flushAll() {
  for (auto& e : entries) {
    e = Entry{};
  }
}

void invalidateMethodCache(ClassMethodCache* cache, Oop selector) {
  if (cache == nullptr) {
    return;
  }
  if (selector.isEmpty()) {
    cache->flushAll();
  } else {
    cache->flushSelector(selector);
  }
}

Oop send(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
         InlineCache* ic) {
  // SPEC §3.3: the empty Oop is a failure, not a receiver. classOf would answer nil for it.
  if (receiver.isEmpty()) {
    return abortFailedSend(ctx, selector);
  }
  IcGuard guard(ctx.roots, ic);
  const Oop klass = ctx.wk.classOf(receiver);
  if (icMatches(ctx, ic, klass, selector)) {
    return applyMethod(ctx, ic->cachedMethod, receiver, args, argc);
  }
  if (ctx.cache != nullptr) {
    const Oop cached = ctx.cache->probe(ctx.heap, klass, selector);
    if (cached.isHeap()) {
      fillCaches(ctx, ic, klass, selector, cached);
      return applyMethod(ctx, cached, receiver, args, argc);
    }
  }
  const Oop meth = lookup(ctx.heap, klass, selector);
  if (!meth.isHeap()) {
    return doesNotUnderstand(ctx, receiver, selector, args, argc);
  }
  fillCaches(ctx, ic, klass, selector, meth);
  return applyMethod(ctx, meth, receiver, args, argc);
}

Oop sendSuper(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
              Oop methodClass) {
  if (receiver.isEmpty()) {
    return abortFailedSend(ctx, selector);
  }
  const Oop meth = lookup(ctx.heap, superclassOf(ctx.heap, methodClass), selector);
  if (!meth.isHeap()) {
    return doesNotUnderstand(ctx, receiver, selector, args, argc);
  }
  return applyMethod(ctx, meth, receiver, args, argc);
}

bool unwinding(const CallContext& ctx) { return ctx.nonlocalReturn || ctx.aborting; }

namespace {

// The abort itself (SPEC §3.4): the first reason stays. Captures nothing.
void startAbort(CallContext& ctx, const char* reason) {
  if (!ctx.aborting) {
    ctx.aborting = true;
    ctx.abortReason = reason;
  }
  // An abort has no home: it overrides a non-local return still in flight.
  dropNonlocal(ctx);
}

}  // namespace

Oop abortEvaluation(CallContext& ctx, const char* reason) {
  const bool starts = !ctx.aborting;
  startAbort(ctx, reason);
  // SPEC §3.13: the chain is still whole here. Not for an abandon, nor inside a cleanup that runs
  // while an earlier abort is set aside (that abort's capture stays).
  if (starts && ctx.debug != nullptr && !ctx.abandoning && ctx.abortSetAside == 0) {
    ctx.debug->onAbort(ctx);
  }
  return Oop{};
}

Oop abortEvaluation(CallContext& ctx, std::string_view reason) {
  if (!ctx.aborting) {
    // The allocation may collect, and outside the interpreter a non-local return's home and
    // value are not rooted. The abort overrides that return anyway, so it is dropped first.
    dropNonlocal(ctx);
    const Oop text = Str::fromUtf8(ctx, reason);
    if (!text.isHeap()) {
      return abortEvaluation(ctx, "out of memory");
    }
    ctx.abortReasonHandle = ctx.roots.pushHandle(text);
  }
  return abortEvaluation(ctx, static_cast<const char*>(nullptr));
}

Oop abortEvaluationQuiet(CallContext& ctx, const char* reason) {
  startAbort(ctx, reason);
  return Oop{};
}

Oop abortDoesNotUnderstand(CallContext& ctx, Oop selector) {
  return abortWithSelector(ctx, "doesNotUnderstand: #", selector, "doesNotUnderstand:");
}

Oop abortFailedSend(CallContext& ctx, Oop selector) {
  if (unwinding(ctx)) {
    return Oop{};
  }
  // SPEC §3.3: after the heap ran out, that is the reason, and building another string would fail.
  if (ctx.heap.outOfMemory()) {
    return abortEvaluation(ctx, "out of memory");
  }
  return abortWithSelector(ctx, "failed: #", selector, "failed");
}

bool stopOrAbort(CallContext& ctx, const char* reason, bool proceedable) {
  if (ctx.scheduler != nullptr && ctx.scheduler->canHalt(ctx)) {
    std::string text;
    try {
      text = reason;
    } catch (const std::bad_alloc&) {
      abortEvaluation(ctx, reason);
      return false;
    }
    return ctx.scheduler->halt(ctx, std::move(text), proceedable);
  }
  abortEvaluation(ctx, reason);
  return false;
}

bool stopOrAbort(CallContext& ctx, std::string_view reason, bool proceedable) {
  if (ctx.scheduler != nullptr && ctx.scheduler->canHalt(ctx)) {
    std::string text;
    try {
      // SPEC §3.3: a NUL byte becomes the two characters \0, as abortReasonText writes it.
      for (const char c : reason) {
        if (c == '\0') {
          text += "\\0";
        } else {
          text += c;
        }
      }
    } catch (const std::bad_alloc&) {
      abortEvaluation(ctx, "out of memory");
      return false;
    }
    return ctx.scheduler->halt(ctx, std::move(text), proceedable);
  }
  abortEvaluation(ctx, reason);
  return false;
}

bool stopWithSelector(CallContext& ctx, const char* prefix, Oop selector, const char* fallback) {
  if (!selector.isHeap() || (ctx.heap.flags(selector) & kFlagBytes) == 0) {
    return stopOrAbort(ctx, fallback, true);
  }
  std::string reason(prefix);
  reason += Str::toUtf8(ctx.heap, selector);
  return stopOrAbort(ctx, std::string_view(reason), true);
}

bool stopFailedSend(CallContext& ctx, Oop selector) {
  if (unwinding(ctx)) {
    return false;
  }
  if (ctx.heap.outOfMemory()) {
    abortEvaluation(ctx, "out of memory");
    return false;
  }
  return stopWithSelector(ctx, "failed: #", selector, "failed");
}

std::string abortReasonText(const CallContext& ctx) {
  if (!ctx.aborting) {
    return {};
  }
  const Oop text = ctx.roots.handleAt(ctx.abortReasonHandle);
  if (text.isHeap() && (ctx.heap.flags(text) & kFlagBytes) != 0) {
    // SPEC §3.3: NUL は `\0` の 2 文字にして、C 文字列で途切れないようにする。
    std::string reason;
    for (const char c : Str::toUtf8(ctx.heap, text)) {
      if (c == '\0') {
        reason += "\\0";
      } else {
        reason += c;
      }
    }
    return reason;
  }
  return ctx.abortReason != nullptr ? std::string(ctx.abortReason) : std::string();
}

void clearUnwinding(CallContext& ctx) {
  if (ctx.abortReasonHandle != CallContext::kNoAbortReasonHandle) {
    ctx.roots.dropHandle(ctx.abortReasonHandle);
    ctx.abortReasonHandle = CallContext::kNoAbortReasonHandle;
  }
  ctx.aborting = false;
  ctx.abortReason = nullptr;
  dropNonlocal(ctx);
}

bool callBlock(CallContext& ctx, Oop blk, const Oop* args, std::uint32_t n, Oop* out) {
  *out = Oop{};
  const Oop sel = valueSelector(ctx, n);
  if (!sel.isHeap()) {
    // More than four arguments, or no Symbol for the selector (out of memory).
    if (!unwinding(ctx)) {
      abortEvaluation(ctx, ctx.heap.outOfMemory() ? "out of memory" : "too many block arguments");
    }
    return false;
  }
  const Oop result = send(ctx, blk, sel, args, n, nullptr);
  if (unwinding(ctx)) {
    return false;
  }
  if (result.isEmpty()) {
    // SPEC §3.3: the block failed (wrong argument count, ...). Its value is not an element. The
    // selector is fetched again: the call may have moved it.
    abortFailedSend(ctx, valueSelector(ctx, n));
    return false;
  }
  *out = result;
  return true;
}

bool truthOf(CallContext& ctx, Oop value, bool* truth) {
  if (value.isTrue() || value.isFalse()) {
    *truth = value.isTrue();
    return true;
  }
  Root v(ctx.roots, value);
  const Oop sel = ctx.wk.intern("mustBeBoolean");
  if (!sel.isHeap()) {
    abortEvaluation(ctx, "NonBoolean receiver");
    return false;
  }
  const Oop answer = send(ctx, v.slot, sel, nullptr, 0, nullptr);
  if (unwinding(ctx)) {
    return false;
  }
  if (!answer.isTrue() && !answer.isFalse()) {
    // SPEC §3.13: halts without Proceed; only an abort ends it, so this never answers a truth.
    stopOrAbort(ctx, "NonBoolean receiver", false);
    return false;
  }
  *truth = answer.isTrue();
  return true;
}

}  // namespace ao

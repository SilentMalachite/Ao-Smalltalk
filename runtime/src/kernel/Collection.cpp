#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <string_view>

namespace ao {
namespace {

Oop selEquals(WellKnown& wk) { return wk.intern("="); }

Oop makeThunk(CallContext& ctx, NativeFn fn, std::uint32_t argc) {
  return makeNativeBlock(ctx, fn, argc);
}

// error: の慣習どおりメッセージ文字列で失敗する。receiver はルート済みスロット（GC しても正しい）。
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

// thunk の pc（添字・件数）は、ブロックを受け取った do: から書き換えられる。+1 が SmallInteger を
// 超えるなら、その呼び出しを失敗にする。
bool counterAtMax(const Heap& heap, Oop thunk) {
  const Oop n = heap.slotAt(thunk, kCtxPc);
  return n.isSmallInteger() && n.smallIntegerValue() >= kSmiMax;
}

// 利用者のブロックのあとで読み直した値 n を +1 して pc に書く。ブロックも pc を書き換えられるので、
// 入口の検査だけでは足りない。+1 が SmallInteger を超えるなら書かずに false を返す（呼び出し側が
// 失敗にする）。n が SmallInteger でなければ何もしない。
bool bumpCounter(Heap& heap, Oop thunk, Oop n) {
  if (!n.isSmallInteger()) {
    return true;
  }
  if (n.smallIntegerValue() >= kSmiMax) {
    return false;
  }
  heap.slotAtPut(thunk, kCtxPc, Oop::fromSmallInteger(n.smallIntegerValue() + 1));
  return true;
}

Oop ao_Collection_collect_fill(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  if (counterAtMax(ctx.heap, self.slot)) {
    return fail(ctx, self.slot, "collect: index out of range");
  }
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  Root mapped(ctx.roots);
  if (!callBlock(ctx, user, &elt.slot, 1, &mapped.slot)) {
    return Oop{};
  }
  Root arr(ctx.roots, ctx.heap.slotAt(self.slot, kBlockHome));
  const Oop idx = ctx.heap.slotAt(self.slot, kCtxPc);
  Oop put[2] = {idx, mapped.slot};
  send(ctx, arr.slot, ctx.wk.selAt_put_, put, 2, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (!bumpCounter(ctx.heap, self.slot, idx)) {
    return fail(ctx, self.slot, "collect: index out of range");
  }
  return mapped.slot;
}

const char* filterCountMessage(bool keepTrue) {
  return keepTrue ? "select: count out of range" : "reject: count out of range";
}

// SPEC §3.6: the Array select:/reject: gathers into and its count, from the thunk's slots, when
// they are sound: kBlockHome a Kernel Array, kCtxPc a SmallInteger from 0 to its size. The do: that
// gets the thunk can write both. Empty Oop and *count untouched when they are not.
Oop filterBuffer(CallContext& ctx, Oop thunk, std::int64_t* count) {
  const Oop buf = ctx.heap.slotAt(thunk, kBlockHome);
  const Oop n = ctx.heap.slotAt(thunk, kCtxPc);
  if (!buf.isHeap() || ctx.heap.klass(buf) != ctx.wk.arrayClass || !n.isSmallInteger() ||
      n.smallIntegerValue() < 0 ||
      n.smallIntegerValue() > static_cast<std::int64_t>(ctx.heap.size(buf))) {
    return Oop{};
  }
  *count = n.smallIntegerValue();
  return buf;
}

// SPEC §3.6: the one thunk select: and reject: pass to do:. Calls the user's block (kBlockCopied)
// once for the element and, when the answer is true (select:, kCtxStackp true) or false (reject:),
// puts the element after the count kept so far in the Array at kBlockHome, doubling that Array
// when it is full.
Oop ao_Collection_filter_scan(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  const bool keepTrue = ctx.heap.slotAt(self.slot, kCtxStackp).isTrue();
  if (counterAtMax(ctx.heap, self.slot)) {
    return fail(ctx, self.slot, filterCountMessage(keepTrue));
  }
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  Root pred(ctx.roots);
  if (!callBlock(ctx, user, &elt.slot, 1, &pred.slot)) {
    return Oop{};
  }
  const bool keep = keepTrue ? pred.slot.isTrue() : pred.slot.isFalse();
  if (!keep) {
    return pred.slot;
  }
  // The block may have written the thunk's slots: read them again.
  std::int64_t n = 0;
  Root buf(ctx.roots, filterBuffer(ctx, self.slot, &n));
  if (!buf.slot.isHeap()) {
    return fail(ctx, self.slot, filterCountMessage(keepTrue));
  }
  const auto capacity = static_cast<std::int64_t>(ctx.heap.size(buf.slot));
  if (n == capacity) {
    const std::int64_t grown = capacity < 4 ? 8 : capacity * 2;
    if (grown > static_cast<std::int64_t>(UINT32_MAX)) {
      return fail(ctx, self.slot, filterCountMessage(keepTrue));
    }
    const Oop bigger = allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(grown), 0);
    if (!bigger.isHeap()) {
      return Oop{};
    }
    for (std::int64_t i = 0; i < n; ++i) {
      ctx.heap.slotAtPut(bigger, static_cast<std::uint32_t>(i),
                         ctx.heap.slotAt(buf.slot, static_cast<std::uint32_t>(i)));
    }
    buf.slot = bigger;
    ctx.heap.slotAtPut(self.slot, kBlockHome, buf.slot);
  }
  ctx.heap.slotAtPut(buf.slot, static_cast<std::uint32_t>(n), elt.slot);
  ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::fromSmallInteger(n + 1));
  return pred.slot;
}

Oop ao_Collection_detect_scan(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  if (ctx.heap.slotAt(self.slot, kCtxPc).isTrue()) {
    return args[0];
  }
  Root elt(ctx.roots, args[0]);
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  Oop pred;
  if (!callBlock(ctx, user, &elt.slot, 1, &pred)) {
    return Oop{};
  }
  if (pred.isTrue()) {
    ctx.heap.slotAtPut(self.slot, kBlockHome, elt.slot);
    ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::true_());
  }
  return pred;
}

Oop ao_Collection_inject_scan(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  const Oop bin = ctx.heap.slotAt(self.slot, kBlockCopied);
  RootedArray accArgs(ctx.roots, 2);
  accArgs[0] = ctx.heap.slotAt(self.slot, kBlockHome);
  accArgs[1] = elt.slot;
  Root next(ctx.roots);
  if (!callBlock(ctx, bin, accArgs.ptr(), 2, &next.slot)) {
    return Oop{};
  }
  ctx.heap.slotAtPut(self.slot, kBlockHome, next.slot);
  return next.slot;
}

// SPEC §3.6: the thunk includes: passes to do:. Until found (kCtxPc true), an element identical to
// the argument (kBlockHome) is found without a send; otherwise `argument = element` (kCtxSender
// holds =) must answer a Boolean, or includes: fails.
Oop ao_Collection_includes_scan(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  if (ctx.heap.slotAt(self.slot, kCtxPc).isTrue()) {
    return args[0];
  }
  Root elt(ctx.roots, args[0]);
  Root needle(ctx.roots, ctx.heap.slotAt(self.slot, kBlockHome));
  if (needle.slot == elt.slot) {
    ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::true_());
    return Oop::true_();
  }
  const Oop sel = ctx.heap.slotAt(self.slot, kCtxSender);
  const Oop eq = send(ctx, needle.slot, sel, &elt.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (!eq.isTrue() && !eq.isFalse()) {
    return abortFailedSend(ctx, ctx.wk.intern("includes:"));
  }
  if (eq.isTrue()) {
    ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::true_());
  }
  return eq;
}

// SPEC §3.6: one do: with ao_Collection_filter_scan, then the kept elements in a fresh Array.
Oop filterIntoArray(CallContext& ctx, Root& rcvr, Root& blk, bool keepTrue) {
  Root buf(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, 8, 0));
  if (!buf.slot.isHeap()) {
    return Oop{};
  }
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_filter_scan, 1));
  if (!thunk.slot.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockCopied, blk.slot);
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, buf.slot);
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(thunk.slot, kCtxStackp, keepTrue ? Oop::true_() : Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  std::int64_t n = 0;
  buf.slot = filterBuffer(ctx, thunk.slot, &n);
  if (!buf.slot.isHeap()) {
    return fail(ctx, rcvr.slot, filterCountMessage(keepTrue));
  }
  const Oop answer = allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(n), 0);
  if (!answer.isHeap()) {
    return Oop{};
  }
  for (std::int64_t i = 0; i < n; ++i) {
    ctx.heap.slotAtPut(answer, static_cast<std::uint32_t>(i),
                       ctx.heap.slotAt(buf.slot, static_cast<std::uint32_t>(i)));
  }
  return answer;
}

}  // namespace

Oop ao_ArrayedCollection_do_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  Root elt(ctx.roots);
  const Oop nOop = send(ctx, rcvr.slot, ctx.wk.selSize, nullptr, 0, nullptr);
  if (!nOop.isSmallInteger()) {
    return nOop;
  }
  const auto n = nOop.smallIntegerValue();
  Oop ignored;
  for (std::int64_t i = 1; i <= n; ++i) {
    Oop idx = Oop::fromSmallInteger(i);
    elt.slot = send(ctx, rcvr.slot, ctx.wk.selAt_, &idx, 1, nullptr);
    if (unwinding(ctx) || !callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
      return Oop{};
    }
  }
  return rcvr.slot;
}

Oop ao_Collection_collect_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  const Oop nOop = send(ctx, rcvr.slot, ctx.wk.selSize, nullptr, 0, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  Root arr(ctx.roots, send(ctx, ctx.wk.arrayClass, ctx.wk.selBasicNew_, &nOop, 1, nullptr));
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_collect_fill, 1));
  if (!thunk.slot.isHeap() || !arr.slot.isHeap()) {
    return arr.slot;
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockCopied, blk.slot);
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, arr.slot);
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::fromSmallInteger(1));
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return arr.slot;
}

Oop ao_Collection_select_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  return filterIntoArray(ctx, rcvr, blk, true);
}

Oop ao_Collection_reject_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  return filterIntoArray(ctx, rcvr, blk, false);
}

Oop ao_Collection_detect_ifNone_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root pred(ctx.roots, args[0]);
  Root none(ctx.roots, args[1]);
  Oop answer;
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_detect_scan, 1));
  if (!thunk.slot.isHeap()) {
    callBlock(ctx, none.slot, nullptr, 0, &answer);
    return answer;
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockCopied, pred.slot);
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, Oop::nil());
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (ctx.heap.slotAt(thunk.slot, kCtxPc).isTrue()) {
    return ctx.heap.slotAt(thunk.slot, kBlockHome);
  }
  callBlock(ctx, none.slot, nullptr, 0, &answer);
  return answer;
}

Oop ao_Collection_inject_into_(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root acc(ctx.roots, args[0]);
  Root bin(ctx.roots, args[1]);
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_inject_scan, 1));
  if (!thunk.slot.isHeap()) {
    return acc.slot;
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockCopied, bin.slot);
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, acc.slot);
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return ctx.heap.slotAt(thunk.slot, kBlockHome);
}

Oop ao_Collection_includes_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root needle(ctx.roots, args[0]);
  // SPEC §3.6: `anObject = element` through do: (Blue Book), as Dictionary>>includes: compares
  // values. No hash is sent.
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_includes_scan, 1));
  if (!thunk.slot.isHeap()) {
    return Oop::false_();
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, needle.slot);
  ctx.heap.slotAtPut(thunk.slot, kCtxSender, selEquals(ctx.wk));
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return ctx.heap.slotAt(thunk.slot, kCtxPc).isTrue() ? Oop::true_() : Oop::false_();
}

Oop ao_Collection_isEmpty(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  const Oop n = send(ctx, receiver, ctx.wk.selSize, nullptr, 0, nullptr);
  if (!n.isSmallInteger()) {
    return n;
  }
  return n.smallIntegerValue() == 0 ? Oop::true_() : Oop::false_();
}

namespace kernel {

void installCollection(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.arrayedCollectionClass, "do:", 1, "ao_ArrayedCollection_do_",
            ao_ArrayedCollection_do_);
  const Oop cls = wk.collectionClass;
  putNative(heap, wk, cls, "collect:", 1, "ao_Collection_collect_", ao_Collection_collect_);
  putNative(heap, wk, cls, "select:", 1, "ao_Collection_select_", ao_Collection_select_);
  putNative(heap, wk, cls, "reject:", 1, "ao_Collection_reject_", ao_Collection_reject_);
  putNative(heap, wk, cls, "detect:ifNone:", 2, "ao_Collection_detect_ifNone_",
            ao_Collection_detect_ifNone_);
  putNative(heap, wk, cls, "inject:into:", 2, "ao_Collection_inject_into_",
            ao_Collection_inject_into_);
  putNative(heap, wk, cls, "includes:", 1, "ao_Collection_includes_", ao_Collection_includes_);
  putNative(heap, wk, cls, "isEmpty", 0, "ao_Collection_isEmpty", ao_Collection_isEmpty);
  // SPEC §3.11: the thunks (makeThunk) the natives above pass to do: take these names, so an image
  // holding one that a user's do: kept rebinds it at load.
  (void)NativeRegistry::addNamed("ao_Collection_collect_fill", ao_Collection_collect_fill, nullptr);
  (void)NativeRegistry::addNamed("ao_Collection_filter_scan", ao_Collection_filter_scan, nullptr);
  (void)NativeRegistry::addNamed("ao_Collection_detect_scan", ao_Collection_detect_scan, nullptr);
  (void)NativeRegistry::addNamed("ao_Collection_inject_scan", ao_Collection_inject_scan, nullptr);
  (void)NativeRegistry::addNamed("ao_Collection_includes_scan", ao_Collection_includes_scan,
                                 nullptr);
}

}  // namespace kernel
}  // namespace ao

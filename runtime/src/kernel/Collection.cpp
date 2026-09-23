#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Send.hpp"

namespace ao {
namespace {

Oop selEquals(WellKnown& wk) { return wk.intern("="); }
Oop selHash(WellKnown& wk) { return wk.intern("hash"); }
Oop selValueValue(WellKnown& wk) { return wk.intern("value:value:"); }

Oop makeThunk(CallContext& ctx, NativeFn fn, std::uint32_t argc) {
  return makeNativeBlock(ctx, fn, argc);
}

Oop ao_Collection_collect_fill(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  Root mapped(ctx.roots, send(ctx, user, ctx.wk.selValue_, &elt.slot, 1, nullptr));
  Root arr(ctx.roots, ctx.heap.slotAt(self.slot, kBlockHome));
  const Oop idx = ctx.heap.slotAt(self.slot, kCtxPc);
  Oop put[2] = {idx, mapped.slot};
  send(ctx, arr.slot, ctx.wk.selAt_put_, put, 2, nullptr);
  if (idx.isSmallInteger()) {
    ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::fromSmallInteger(idx.smallIntegerValue() + 1));
  }
  return mapped.slot;
}

Oop ao_Collection_filter_count(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  const Oop pred = send(ctx, user, ctx.wk.selValue_, &elt.slot, 1, nullptr);
  const bool keepTrue = ctx.heap.slotAt(self.slot, kCtxStackp).isTrue();
  const bool keep = keepTrue ? pred.isTrue() : pred.isFalse();
  if (keep) {
    const Oop n = ctx.heap.slotAt(self.slot, kCtxPc);
    if (n.isSmallInteger()) {
      ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::fromSmallInteger(n.smallIntegerValue() + 1));
    }
  }
  return pred;
}

Oop ao_Collection_filter_fill(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  const Oop pred = send(ctx, user, ctx.wk.selValue_, &elt.slot, 1, nullptr);
  const bool keepTrue = ctx.heap.slotAt(self.slot, kCtxStackp).isTrue();
  const bool keep = keepTrue ? pred.isTrue() : pred.isFalse();
  if (keep) {
    Root arr(ctx.roots, ctx.heap.slotAt(self.slot, kBlockHome));
    const Oop idx = ctx.heap.slotAt(self.slot, kCtxPc);
    Oop put[2] = {idx, elt.slot};
    send(ctx, arr.slot, ctx.wk.selAt_put_, put, 2, nullptr);
    if (idx.isSmallInteger()) {
      ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::fromSmallInteger(idx.smallIntegerValue() + 1));
    }
  }
  return pred;
}

Oop ao_Collection_detect_scan(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  if (ctx.heap.slotAt(self.slot, kCtxPc).isTrue()) {
    return args[0];
  }
  Root elt(ctx.roots, args[0]);
  const Oop user = ctx.heap.slotAt(self.slot, kBlockCopied);
  const Oop pred = send(ctx, user, ctx.wk.selValue_, &elt.slot, 1, nullptr);
  if (pred.isTrue()) {
    ctx.heap.slotAtPut(self.slot, kBlockHome, elt.slot);
    ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::true_());
  }
  return pred;
}

Oop ao_Collection_inject_scan(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root elt(ctx.roots, args[0]);
  const Oop bin = ctx.heap.slotAt(self.slot, kBlockCopied);
  Oop accArgs[2] = {ctx.heap.slotAt(self.slot, kBlockHome), elt.slot};
  Root next(ctx.roots, send(ctx, bin, selValueValue(ctx.wk), accArgs, 2, nullptr));
  ctx.heap.slotAtPut(self.slot, kBlockHome, next.slot);
  return next.slot;
}

Oop ao_Collection_includes_scan(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  if (ctx.heap.slotAt(self.slot, kCtxPc).isTrue()) {
    return args[0];
  }
  Root elt(ctx.roots, args[0]);
  Root needle(ctx.roots, ctx.heap.slotAt(self.slot, kBlockHome));
  const Oop sel = ctx.heap.slotAt(self.slot, kCtxSender);
  const Oop eq = send(ctx, elt.slot, sel, &needle.slot, 1, nullptr);
  if (eq.isTrue()) {
    ctx.heap.slotAtPut(self.slot, kCtxPc, Oop::true_());
  }
  return eq;
}

Oop filterIntoArray(CallContext& ctx, Root& rcvr, Root& blk, bool keepTrue) {
  Root countThunk(ctx.roots, makeThunk(ctx, ao_Collection_filter_count, 1));
  if (!countThunk.slot.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(countThunk.slot, kBlockCopied, blk.slot);
  ctx.heap.slotAtPut(countThunk.slot, kCtxPc, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(countThunk.slot, kCtxStackp, keepTrue ? Oop::true_() : Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &countThunk.slot, 1, nullptr);
  const Oop nOop = ctx.heap.slotAt(countThunk.slot, kCtxPc);
  Root arr(ctx.roots, send(ctx, ctx.wk.arrayClass, ctx.wk.selBasicNew_, &nOop, 1, nullptr));
  Root fillThunk(ctx.roots, makeThunk(ctx, ao_Collection_filter_fill, 1));
  if (!fillThunk.slot.isHeap() || !arr.slot.isHeap()) {
    return arr.slot;
  }
  ctx.heap.slotAtPut(fillThunk.slot, kBlockCopied, blk.slot);
  ctx.heap.slotAtPut(fillThunk.slot, kBlockHome, arr.slot);
  ctx.heap.slotAtPut(fillThunk.slot, kCtxPc, Oop::fromSmallInteger(1));
  ctx.heap.slotAtPut(fillThunk.slot, kCtxStackp, keepTrue ? Oop::true_() : Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &fillThunk.slot, 1, nullptr);
  return arr.slot;
}

}  // namespace

Oop ao_ArrayedCollection_do_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
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
  for (std::int64_t i = 1; i <= n; ++i) {
    Oop idx = Oop::fromSmallInteger(i);
    elt.slot = send(ctx, rcvr.slot, ctx.wk.selAt_, &idx, 1, nullptr);
    send(ctx, blk.slot, ctx.wk.selValue_, &elt.slot, 1, nullptr);
    if (ctx.nonlocalReturn) {
      return rcvr.slot;
    }
  }
  return rcvr.slot;
}

Oop ao_Collection_collect_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  const Oop nOop = send(ctx, rcvr.slot, ctx.wk.selSize, nullptr, 0, nullptr);
  Root arr(ctx.roots, send(ctx, ctx.wk.arrayClass, ctx.wk.selBasicNew_, &nOop, 1, nullptr));
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_collect_fill, 1));
  if (!thunk.slot.isHeap() || !arr.slot.isHeap()) {
    return arr.slot;
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockCopied, blk.slot);
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, arr.slot);
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::fromSmallInteger(1));
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  return arr.slot;
}

Oop ao_Collection_select_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  return filterIntoArray(ctx, rcvr, blk, true);
}

Oop ao_Collection_reject_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  return filterIntoArray(ctx, rcvr, blk, false);
}

Oop ao_Collection_detect_ifNone_(CallContext& ctx, Oop receiver, const Oop* args,
                                 std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root pred(ctx.roots, args[0]);
  Root none(ctx.roots, args[1]);
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_detect_scan, 1));
  if (!thunk.slot.isHeap()) {
    return send(ctx, none.slot, ctx.wk.selValue, nullptr, 0, nullptr);
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockCopied, pred.slot);
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, Oop::nil());
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (ctx.heap.slotAt(thunk.slot, kCtxPc).isTrue()) {
    return ctx.heap.slotAt(thunk.slot, kBlockHome);
  }
  return send(ctx, none.slot, ctx.wk.selValue, nullptr, 0, nullptr);
}

Oop ao_Collection_inject_into_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
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
  return ctx.heap.slotAt(thunk.slot, kBlockHome);
}

Oop ao_Collection_includes_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root needle(ctx.roots, args[0]);
  send(ctx, needle.slot, selHash(ctx.wk), nullptr, 0, nullptr);
  Root thunk(ctx.roots, makeThunk(ctx, ao_Collection_includes_scan, 1));
  if (!thunk.slot.isHeap()) {
    return Oop::false_();
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, needle.slot);
  ctx.heap.slotAtPut(thunk.slot, kCtxSender, selEquals(ctx.wk));
  ctx.heap.slotAtPut(thunk.slot, kCtxPc, Oop::false_());
  send(ctx, rcvr.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  return ctx.heap.slotAt(thunk.slot, kCtxPc).isTrue() ? Oop::true_() : Oop::false_();
}

Oop ao_Collection_isEmpty(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
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
}

}  // namespace kernel
}  // namespace ao

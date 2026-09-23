#include "ao/Context.hpp"

#include "ao/Gc.hpp"

namespace ao {
namespace {

struct Root {
  Roots& roots;
  Oop slot;
  explicit Root(Roots& r, Oop v = Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

Oop allocateRetry(CallContext& ctx, Oop cls, std::uint32_t size, std::uint16_t flags) {
  if (ctx.heap.gcStress() != 0) {
    Root stressed(ctx.roots, cls);
    Gc(ctx.heap, ctx.roots).stressPoint();
    cls = stressed.slot;
  }
  Oop obj = ctx.heap.allocate(cls, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  Root held(ctx.roots, cls);
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  return ctx.heap.allocate(held.slot, size, flags);
}

}  // namespace

namespace Context {

Oop createMethod(CallContext& ctx, Oop method, Oop receiver, Oop sender, std::uint8_t argc) {
  Root meth(ctx.roots, method);
  Root rcvr(ctx.roots, receiver);
  Root snd(ctx.roots, sender);
  Root obj(ctx.roots,
           allocateRetry(ctx, ctx.wk.methodContextClass, kMethodContextSlotCount, 0));
  if (!obj.slot.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(obj.slot, kCtxSender, snd.slot);
  ctx.heap.slotAtPut(obj.slot, kCtxPc, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(obj.slot, kCtxStackp, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(obj.slot, kCtxMethod, meth.slot);
  ctx.heap.slotAtPut(obj.slot, kCtxReceiver, rcvr.slot);
  ctx.heap.slotAtPut(obj.slot, kCtxArgc, Oop::fromSmallInteger(static_cast<std::int64_t>(argc)));
  return obj.slot;
}

Oop createBlock(CallContext& ctx, Oop method, Oop receiver, Oop home, Oop copied, Oop sender,
                std::uint8_t argc) {
  Root meth(ctx.roots, method);
  Root rcvr(ctx.roots, receiver);
  Root hm(ctx.roots, home);
  Root cp(ctx.roots, copied);
  Root snd(ctx.roots, sender);
  Root obj(ctx.roots, allocateRetry(ctx, ctx.wk.blockContextClass, kBlockSlotCount, 0));
  if (!obj.slot.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(obj.slot, kCtxSender, snd.slot);
  ctx.heap.slotAtPut(obj.slot, kCtxPc, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(obj.slot, kCtxStackp, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(obj.slot, kCtxMethod, meth.slot);
  ctx.heap.slotAtPut(obj.slot, kCtxReceiver, rcvr.slot);
  ctx.heap.slotAtPut(obj.slot, kCtxArgc, Oop::fromSmallInteger(static_cast<std::int64_t>(argc)));
  ctx.heap.slotAtPut(obj.slot, kBlockHome, hm.slot);
  ctx.heap.slotAtPut(obj.slot, kBlockCopied, cp.slot);
  return obj.slot;
}

}  // namespace Context
}  // namespace ao

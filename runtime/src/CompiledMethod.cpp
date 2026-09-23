#include "ao/CompiledMethod.hpp"

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

namespace CompiledMethod {

Oop create(CallContext& ctx, std::uint8_t numArgs, std::uint8_t numTemps, std::uint16_t prim,
           Oop literals, Oop bytes, Oop selector, Oop methodClass) {
  Root lit(ctx.roots, literals);
  Root byt(ctx.roots, bytes);
  Root sel(ctx.roots, selector);
  Root mcls(ctx.roots, methodClass);
  Root cm(ctx.roots, allocateRetry(ctx, ctx.wk.compiledMethodClass, kCmSlotCount, 0));
  if (!cm.slot.isHeap()) {
    return Oop{};
  }
  const std::int64_t hdr = static_cast<std::int64_t>(numArgs) | static_cast<std::int64_t>(numTemps) << 8 |
                           static_cast<std::int64_t>(prim) << 16;
  ctx.heap.slotAtPut(cm.slot, kCmSlotHeader, Oop::fromSmallInteger(hdr));
  ctx.heap.slotAtPut(cm.slot, kCmSlotLiterals, lit.slot);
  ctx.heap.slotAtPut(cm.slot, kCmSlotBytes, byt.slot);
  ctx.heap.slotAtPut(cm.slot, kCmSlotNativeCode, Oop::nil());
  ctx.heap.slotAtPut(cm.slot, kCmSlotSelector, sel.slot);
  ctx.heap.slotAtPut(cm.slot, kCmSlotMethodClass, mcls.slot);
  return cm.slot;
}

}  // namespace CompiledMethod
}  // namespace ao

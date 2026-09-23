#include "ao/CompiledMethod.hpp"

#include "ao/HandleScope.hpp"

namespace ao {

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

#include "ao/kernel/Install.hpp"

#include "ao/CompiledMethod.hpp"

namespace ao {
namespace {

bool isCompiledMethod(CallContext& ctx, Oop o) {
  return o.isHeap() && ctx.heap.klass(o) == ctx.wk.compiledMethodClass &&
         ctx.heap.size(o) >= kCmSlotCount && (ctx.heap.flags(o) & kFlagBytes) == 0;
}

Oop headerWord(CallContext& ctx, Oop receiver) {
  if (!isCompiledMethod(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kCmSlotHeader);
}

}  // namespace

Oop ao_CompiledMethod_numArgs(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  const Oop h = headerWord(ctx, receiver);
  if (!h.isSmallInteger()) {
    return Oop{};
  }
  return Oop::fromSmallInteger(h.smallIntegerValue() & 0xFF);
}

Oop ao_CompiledMethod_numTemps(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  const Oop h = headerWord(ctx, receiver);
  if (!h.isSmallInteger()) {
    return Oop{};
  }
  return Oop::fromSmallInteger((h.smallIntegerValue() >> 8) & 0xFF);
}

Oop ao_CompiledMethod_primitive(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  const Oop h = headerWord(ctx, receiver);
  if (!h.isSmallInteger()) {
    return Oop{};
  }
  return Oop::fromSmallInteger((h.smallIntegerValue() >> 16) & 0xFFFF);
}

Oop ao_CompiledMethod_literals(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isCompiledMethod(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kCmSlotLiterals);
}

Oop ao_CompiledMethod_bytecodes(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isCompiledMethod(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kCmSlotBytes);
}

Oop ao_CompiledMethod_nativeCode(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isCompiledMethod(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kCmSlotNativeCode);
}

namespace kernel {

void installCompiledMethod(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.compiledMethodClass;
  putNative(heap, wk, cls, "numArgs", 0, "ao_CompiledMethod_numArgs", ao_CompiledMethod_numArgs);
  putNative(heap, wk, cls, "numTemps", 0, "ao_CompiledMethod_numTemps", ao_CompiledMethod_numTemps);
  putNative(heap, wk, cls, "primitive", 0, "ao_CompiledMethod_primitive",
            ao_CompiledMethod_primitive);
  putNative(heap, wk, cls, "literals", 0, "ao_CompiledMethod_literals", ao_CompiledMethod_literals);
  putNative(heap, wk, cls, "bytecodes", 0, "ao_CompiledMethod_bytecodes",
            ao_CompiledMethod_bytecodes);
  putNative(heap, wk, cls, "nativeCode", 0, "ao_CompiledMethod_nativeCode",
            ao_CompiledMethod_nativeCode);
}

}  // namespace kernel
}  // namespace ao

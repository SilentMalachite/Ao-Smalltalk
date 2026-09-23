#include "ao/Context.hpp"

#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/Interpreter.hpp"

#include <cstring>
#include <string>
#include <vector>

namespace ao {

namespace {

Oop applyBlock(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (!receiver.isHeap()) {
    return Oop{};
  }
  const Oop meth = ctx.heap.slotAt(receiver, kCtxMethod);
  Oop rcvr = ctx.heap.slotAt(receiver, kCtxReceiver);
  const bool compiled = meth.isHeap() && ctx.heap.klass(meth) == ctx.wk.compiledMethodClass;
  if (!compiled && !rcvr.isHeap()) {
    rcvr = receiver;
  }
  return applyMethod(ctx, meth, rcvr, args, argc, receiver);
}

}  // namespace

Oop makeNativeBlock(CallContext& ctx, NativeFn fn, std::uint32_t argc) {
  Oop sel = (argc == 0) ? ctx.wk.selValue : ctx.wk.selValue_;
  auto idx = NativeRegistry::add(fn);
  auto meth = NativeMethod::create(ctx.heap, ctx.wk, sel, argc, "ao_NativeBlock_thunk", idx,
                                   ctx.wk.blockContextClass);
  auto blk = ctx.heap.allocate(ctx.wk.blockContextClass, kBlockSlotCount, 0);
  if (!blk.isHeap() || !meth.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(blk, kCtxMethod, meth);
  ctx.heap.slotAtPut(blk, kCtxArgc, Oop::fromSmallInteger(static_cast<std::int64_t>(argc)));
  ctx.heap.slotAtPut(blk, kCtxReceiver, blk);
  return blk;
}

Oop ao_BlockContext_value(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  return applyBlock(ctx, receiver, args, argc);
}

Oop ao_BlockContext_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc) {
  return applyBlock(ctx, receiver, args, argc);
}

Oop ao_BlockContext_value_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc) {
  return applyBlock(ctx, receiver, args, argc);
}

Oop ao_BlockContext_valueWithArguments_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                        std::uint32_t argc) {
  if (argc != 1 || !args[0].isHeap()) {
    return Oop{};
  }
  const Oop arr = args[0];
  if ((ctx.heap.flags(arr) & kFlagBytes) != 0) {
    return Oop{};
  }
  const auto n = ctx.heap.size(arr);
  std::vector<Oop> unpacked(n);
  for (std::uint32_t i = 0; i < n; ++i) {
    unpacked[i] = ctx.heap.slotAt(arr, i);
  }
  return applyBlock(ctx, receiver, unpacked.data(), n);
}

namespace Str {

Oop fromUtf8(Heap& heap, WellKnown& wk, std::string_view utf8) {
  const auto n = static_cast<std::uint32_t>(utf8.size());
  auto str = heap.allocate(wk.stringClass, n, kFlagBytes);
  if (!str.isHeap()) {
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(heap.bytes(str), utf8.data(), n);
  }
  return str;
}

Oop fromUtf8(CallContext& ctx, std::string_view utf8) {
  const auto n = static_cast<std::uint32_t>(utf8.size());
  const Oop str = allocateRetry(ctx, ctx.wk.stringClass, n, kFlagBytes);
  if (!str.isHeap()) {
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(ctx.heap.bytes(str), utf8.data(), n);
  }
  return str;
}

std::string toUtf8(Heap& heap, Oop str) {
  if (!str.isHeap()) {
    return {};
  }
  const ObjectHeader* h = heap.header(str);
  return std::string(reinterpret_cast<const char*>(h + 1), h->size);
}

}  // namespace Str
}  // namespace ao

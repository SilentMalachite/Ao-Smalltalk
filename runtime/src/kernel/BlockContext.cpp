#include "ao/Context.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Format.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <cstring>
#include <string>

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

// whileTrue: and its kin (SPEC §3.6). receiver and body are rooted slots of the native. Loops
// while the condition's truth is `wanted`; answers nil, or the empty Oop when unwinding.
Oop whileLoop(CallContext& ctx, const Oop& receiver, const Oop* body, bool wanted) {
  Gc gc(ctx.heap, ctx.roots);
  Oop value;
  for (std::uint64_t i = 1;; ++i) {
    bool truth = false;
    if (!callBlock(ctx, receiver, nullptr, 0, &value) || !truthOf(ctx, value, &truth)) {
      return Oop{};
    }
    if (truth != wanted) {
      return Oop::nil();
    }
    if (body != nullptr && !callBlock(ctx, *body, nullptr, 0, &value)) {
      return Oop{};
    }
    if ((i & 0xFFFF) == 0) {
      gc.safepoint();
    }
  }
}

// Runs cleanup with the unwinding state set aside, then restores it (SPEC §3.4). When cleanup
// starts its own unwinding, that one wins and the saved state is dropped.
void runAside(CallContext& ctx, Oop cleanup) {
  Root blk(ctx.roots, cleanup);
  Root home(ctx.roots, ctx.nonlocalHome);
  Root value(ctx.roots, ctx.nonlocalValue);
  const bool nonlocal = ctx.nonlocalReturn;
  const bool aborting = ctx.aborting;
  const char* reason = ctx.abortReason;
  clearUnwinding(ctx);
  Oop ignored;
  // During a stack overflow abort the cleanup runs past the normal limit (SPEC §3.4).
  ++ctx.cleanupDepth;
  const bool ran = callBlock(ctx, blk.slot, nullptr, 0, &ignored);
  --ctx.cleanupDepth;
  if (!ran) {
    return;
  }
  ctx.nonlocalReturn = nonlocal;
  ctx.nonlocalHome = home.slot;
  ctx.nonlocalValue = value.slot;
  ctx.aborting = aborting;
  ctx.abortReason = reason;
}

}  // namespace

Oop makeNativeBlock(CallContext& ctx, NativeFn fn, std::uint32_t argc) {
  auto idx = NativeRegistry::add(fn);
  // The block may run the GC; the method is made after it without one (NativeMethod::create).
  Root blk(ctx.roots, allocateRetry(ctx, ctx.wk.blockContextClass, kBlockSlotCount, 0));
  if (!blk.slot.isHeap()) {
    return Oop{};
  }
  const Oop sel = (argc == 0) ? ctx.wk.selValue : ctx.wk.selValue_;
  const Oop meth = NativeMethod::create(ctx.heap, ctx.wk, sel, argc, "ao_NativeBlock_thunk", idx,
                                        ctx.wk.blockContextClass);
  if (!meth.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(blk.slot, kCtxMethod, meth);
  ctx.heap.slotAtPut(blk.slot, kCtxArgc, Oop::fromSmallInteger(static_cast<std::int64_t>(argc)));
  ctx.heap.slotAtPut(blk.slot, kCtxReceiver, blk.slot);
  return blk.slot;
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
  if (argc != 1) {
    return Oop{};
  }
  // 02 Low: only an Array (or a subclass) is unpacked, never another object's internal slots.
  const Oop arr = args[0];
  if (!arr.isHeap() || (ctx.heap.flags(arr) & kFlagBytes) != 0 ||
      !chainIncludes(ctx.heap, ctx.wk.classOf(arr), ctx.wk.arrayClass)) {
    Oop msg = Str::fromUtf8(ctx, "valueWithArguments: expects an Array");
    return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &msg, 1);
  }
  // A subclass's named variables come first; only the indexed elements are arguments.
  const Oop cls = ctx.wk.classOf(args[0]);
  const auto named =
      static_cast<std::uint32_t>(Format::instSize(ctx.heap.slotAt(cls, kClassSlotFormat)));
  const auto size = ctx.heap.size(args[0]);
  const auto n = size > named ? size - named : 0;
  RootedArray unpacked(ctx.roots, n);
  for (std::uint32_t i = 0; i < n; ++i) {
    unpacked[i] = ctx.heap.slotAt(args[0], named + i);
  }
  return applyBlock(ctx, receiver, unpacked.ptr(), n);
}

Oop ao_BlockContext_value_value_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                       std::uint32_t argc) {
  return applyBlock(ctx, receiver, args, argc);
}

Oop ao_BlockContext_value_value_value_value_(CallContext& ctx, const Oop& receiver,
                                             const Oop* args, std::uint32_t argc) {
  return applyBlock(ctx, receiver, args, argc);
}

Oop ao_BlockContext_numArgs(CallContext& ctx, const Oop& receiver, const Oop*,
                            std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  const Oop n = ctx.heap.slotAt(receiver, kCtxArgc);
  return n.isSmallInteger() ? n : Oop{};
}

Oop ao_BlockContext_whileTrue_(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return whileLoop(ctx, receiver, &args[0], true);
}

Oop ao_BlockContext_whileFalse_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return whileLoop(ctx, receiver, &args[0], false);
}

Oop ao_BlockContext_whileTrue(CallContext& ctx, const Oop& receiver, const Oop*,
                              std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return whileLoop(ctx, receiver, nullptr, true);
}

Oop ao_BlockContext_whileFalse(CallContext& ctx, const Oop& receiver, const Oop*,
                               std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return whileLoop(ctx, receiver, nullptr, false);
}

// Runs until the block unwinds (a non-local return or an abort).
Oop ao_BlockContext_repeat(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Gc gc(ctx.heap, ctx.roots);
  Oop ignored;
  for (std::uint64_t i = 1;; ++i) {
    if (!callBlock(ctx, receiver, nullptr, 0, &ignored)) {
      return Oop{};
    }
    if ((i & 0xFFFF) == 0) {
      gc.safepoint();
    }
  }
}

// SPEC §3.4: the cleanup runs after a normal end and during unwinding.
Oop ao_BlockContext_ensure_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root result(ctx.roots);
  callBlock(ctx, receiver, nullptr, 0, &result.slot);
  runAside(ctx, args[0]);
  return unwinding(ctx) ? Oop{} : result.slot;
}

// SPEC §3.4: the cleanup runs only when the receiver's block unwinds.
Oop ao_BlockContext_ifCurtailed_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root result(ctx.roots);
  if (callBlock(ctx, receiver, nullptr, 0, &result.slot)) {
    return result.slot;
  }
  runAside(ctx, args[0]);
  return Oop{};
}

// SPEC §3.4: sent to a block activation whose ^ finds its home dead. Answers like error:.
Oop ao_BlockContext_cannotReturn_(CallContext& ctx, const Oop& receiver, const Oop*,
                                  std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Oop msg = Str::fromUtf8(ctx, "cannot return");
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &msg, 1);
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

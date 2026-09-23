#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Send.hpp"

namespace ao {
namespace {

constexpr std::uint32_t kProcNextLink = 0;
constexpr std::uint32_t kProcContext  = 1;
constexpr std::uint32_t kProcPriority = 2;
constexpr std::uint32_t kProcMyList   = 3;

constexpr std::uint32_t kSchedQuiescent = 0;
constexpr std::uint32_t kSchedActive    = 1;

constexpr std::uint32_t kSemExcess = 0;
constexpr std::uint32_t kSemList   = 1;

constexpr std::uint32_t kSqContents = 0;
constexpr std::uint32_t kSqRead     = 1;
constexpr std::uint32_t kSqWrite    = 2;

constexpr std::uint32_t kOcArray = 0;
constexpr std::uint32_t kOcFirst = 1;
constexpr std::uint32_t kOcLast  = 2;

bool hasSlots(Heap& heap, Oop obj, std::uint32_t n) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0 && heap.size(obj) > n;
}

std::int64_t ocSize(Heap& heap, Oop oc) {
  if (!hasSlots(heap, oc, kOcLast)) {
    return 0;
  }
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!first.isSmallInteger() || !last.isSmallInteger()) {
    return 0;
  }
  const auto f = first.smallIntegerValue();
  const auto l = last.smallIntegerValue();
  return l < f ? 0 : l - f + 1;
}

Oop ocAdd(CallContext& ctx, Oop oc, Oop value) {
  Root list(ctx.roots, oc);
  Root v(ctx.roots, value);
  Oop sel = ctx.wk.intern("add:");
  if (!sel.isHeap()) {
    return Oop{};
  }
  return send(ctx, list.slot, sel, &v.slot, 1, nullptr);
}

Oop ocRemoveFirst(Heap& heap, Oop oc) {
  if (!hasSlots(heap, oc, kOcLast)) {
    return Oop::nil();
  }
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!first.isSmallInteger() || !last.isSmallInteger()) {
    return Oop::nil();
  }
  const auto f = first.smallIntegerValue();
  const auto l = last.smallIntegerValue();
  if (l < f) {
    return Oop::nil();
  }
  const Oop arr = heap.slotAt(oc, kOcArray);
  if (!arr.isHeap() || (heap.flags(arr) & kFlagBytes) != 0) {
    return Oop::nil();
  }
  const auto idx = static_cast<std::uint32_t>(f - 1);
  if (idx >= heap.size(arr)) {
    return Oop::nil();
  }
  const Oop value = heap.slotAt(arr, idx);
  heap.slotAtPut(arr, idx, Oop::nil());
  heap.slotAtPut(oc, kOcFirst, Oop::fromSmallInteger(f + 1));
  return value;
}

bool ocRemoveIdentity(Heap& heap, Oop oc, Oop target) {
  if (!hasSlots(heap, oc, kOcLast)) {
    return false;
  }
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!first.isSmallInteger() || !last.isSmallInteger()) {
    return false;
  }
  auto f = first.smallIntegerValue();
  auto l = last.smallIntegerValue();
  if (l < f) {
    return false;
  }
  const Oop arr = heap.slotAt(oc, kOcArray);
  if (!arr.isHeap() || (heap.flags(arr) & kFlagBytes) != 0) {
    return false;
  }
  const auto n = heap.size(arr);
  for (auto i = f; i <= l; ++i) {
    const auto idx = static_cast<std::uint32_t>(i - 1);
    if (idx >= n) {
      return false;
    }
    if (heap.slotAt(arr, idx) != target) {
      continue;
    }
    for (auto j = i; j < l; ++j) {
      const auto dst = static_cast<std::uint32_t>(j - 1);
      const auto src = static_cast<std::uint32_t>(j);
      if (src >= n) {
        break;
      }
      heap.slotAtPut(arr, dst, heap.slotAt(arr, src));
    }
    const auto lastIdx = static_cast<std::uint32_t>(l - 1);
    if (lastIdx < n) {
      heap.slotAtPut(arr, lastIdx, Oop::nil());
    }
    heap.slotAtPut(oc, kOcLast, Oop::fromSmallInteger(l - 1));
    return true;
  }
  return false;
}

Oop ensureOc(CallContext& ctx, Root& holder, std::uint32_t slot) {
  if (!hasSlots(ctx.heap, holder.slot, slot)) {
    return Oop{};
  }
  Oop oc = ctx.heap.slotAt(holder.slot, slot);
  if (oc.isHeap() && ctx.wk.classOf(oc) == ctx.wk.orderedCollectionClass) {
    return oc;
  }
  Oop neu = send(ctx, ctx.wk.orderedCollectionClass, ctx.wk.selNew, nullptr, 0, nullptr);
  if (neu.isHeap()) {
    ctx.heap.slotAtPut(holder.slot, slot, neu);
  }
  return neu;
}

Oop ctxSlot(CallContext& ctx, Oop receiver, std::uint32_t slot) {
  if (!hasSlots(ctx.heap, receiver, slot)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, slot);
}

}  // namespace

Oop ao_Process_resume(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc);
Oop ao_Process_suspend(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc);
Oop ao_Semaphore_signal(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc);
Oop ao_Semaphore_wait(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc);

Oop ao_MethodContext_sender(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return ctxSlot(ctx, receiver, kCtxSender);
}

Oop ao_MethodContext_method(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return ctxSlot(ctx, receiver, kCtxMethod);
}

Oop ao_MethodContext_receiver(CallContext& ctx, const Oop& receiver, const Oop*,
                              std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return ctxSlot(ctx, receiver, kCtxReceiver);
}

Oop ao_ProcessorScheduler_activeProcess(CallContext& ctx, const Oop& receiver, const Oop*,
                                        std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return ctxSlot(ctx, receiver, kSchedActive);
}

Oop ao_Process_priority_(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  if (argc != 1 || !hasSlots(ctx.heap, receiver, kProcPriority)) {
    return Oop{};
  }
  ctx.heap.slotAtPut(receiver, kProcPriority, args[0]);
  return receiver;
}

Oop ao_Process_resume(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kProcMyList)) {
    return Oop{};
  }
  Root proc(ctx.roots, receiver);
  Root sched(ctx.roots, ctx.wk.processor);
  if (!hasSlots(ctx.heap, sched.slot, kSchedActive)) {
    return proc.slot;
  }
  const Oop active = ctx.heap.slotAt(sched.slot, kSchedActive);
  if (proc.slot == active) {
    return proc.slot;
  }
  if (ctx.heap.slotAt(proc.slot, kProcMyList).isHeap()) {
    return proc.slot;
  }
  if (!active.isHeap()) {
    ctx.heap.slotAtPut(sched.slot, kSchedActive, proc.slot);
    ctx.heap.slotAtPut(proc.slot, kProcMyList, Oop::nil());
    return proc.slot;
  }
  Root q(ctx.roots, ensureOc(ctx, sched, kSchedQuiescent));
  if (!q.slot.isHeap()) {
    return proc.slot;
  }
  ocAdd(ctx, q.slot, proc.slot);
  ctx.heap.slotAtPut(proc.slot, kProcMyList, q.slot);
  return proc.slot;
}

Oop ao_Process_suspend(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kProcMyList)) {
    return Oop{};
  }
  Root proc(ctx.roots, receiver);
  Root sched(ctx.roots, ctx.wk.processor);
  if (!hasSlots(ctx.heap, sched.slot, kSchedActive)) {
    return proc.slot;
  }
  const Oop active = ctx.heap.slotAt(sched.slot, kSchedActive);
  if (proc.slot == active) {
    Oop q = ctx.heap.slotAt(sched.slot, kSchedQuiescent);
    Oop next = ocRemoveFirst(ctx.heap, q);
    ctx.heap.slotAtPut(sched.slot, kSchedActive, next);
    if (hasSlots(ctx.heap, next, kProcMyList)) {
      ctx.heap.slotAtPut(next, kProcMyList, Oop::nil());
    }
    return proc.slot;
  }
  Oop q = ctx.heap.slotAt(sched.slot, kSchedQuiescent);
  const Oop myList = ctx.heap.slotAt(proc.slot, kProcMyList);
  if (myList.isHeap() && myList != q) {
    return proc.slot;
  }
  ocRemoveIdentity(ctx.heap, q, proc.slot);
  ctx.heap.slotAtPut(proc.slot, kProcMyList, Oop::nil());
  return proc.slot;
}

Oop ao_ProcessorScheduler_yield(CallContext& ctx, const Oop& receiver, const Oop*,
                                std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSchedActive)) {
    return Oop{};
  }
  Root sched(ctx.roots, receiver);
  Root q(ctx.roots, ensureOc(ctx, sched, kSchedQuiescent));
  if (ocSize(ctx.heap, q.slot) == 0) {
    return sched.slot;
  }
  Root active(ctx.roots, ctx.heap.slotAt(sched.slot, kSchedActive));
  if (active.slot.isHeap()) {
    ocAdd(ctx, q.slot, active.slot);
    if (hasSlots(ctx.heap, active.slot, kProcMyList)) {
      ctx.heap.slotAtPut(active.slot, kProcMyList, q.slot);
    }
  }
  Oop next = ocRemoveFirst(ctx.heap, q.slot);
  ctx.heap.slotAtPut(sched.slot, kSchedActive, next);
  if (hasSlots(ctx.heap, next, kProcMyList)) {
    ctx.heap.slotAtPut(next, kProcMyList, Oop::nil());
  }
  return sched.slot;
}

Oop ao_Semaphore_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Oop obj = send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr);
  Root sem(ctx.roots, obj);
  if (!hasSlots(ctx.heap, sem.slot, kSemList)) {
    return sem.slot;
  }
  ctx.heap.slotAtPut(sem.slot, kSemExcess, Oop::fromSmallInteger(0));
  Oop list = send(ctx, ctx.wk.orderedCollectionClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(sem.slot, kSemList, list);
  return sem.slot;
}

Oop ao_Semaphore_signal(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSemList)) {
    return Oop{};
  }
  Root sem(ctx.roots, receiver);
  Oop list = ctx.heap.slotAt(sem.slot, kSemList);
  if (ocSize(ctx.heap, list) == 0) {
    const Oop excess = ctx.heap.slotAt(sem.slot, kSemExcess);
    const auto n = excess.isSmallInteger() ? excess.smallIntegerValue() : 0;
    ctx.heap.slotAtPut(sem.slot, kSemExcess, Oop::fromSmallInteger(n + 1));
    return sem.slot;
  }
  Oop waiter = ocRemoveFirst(ctx.heap, list);
  if (hasSlots(ctx.heap, waiter, kProcMyList)) {
    ctx.heap.slotAtPut(waiter, kProcMyList, Oop::nil());
  }
  NativeMethod::invoke(ctx, ao_Process_resume, waiter, nullptr, 0);
  return sem.slot;
}

Oop ao_Semaphore_wait(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSemList)) {
    return Oop{};
  }
  Root sem(ctx.roots, receiver);
  const Oop excess = ctx.heap.slotAt(sem.slot, kSemExcess);
  const auto n = excess.isSmallInteger() ? excess.smallIntegerValue() : 0;
  if (n > 0) {
    ctx.heap.slotAtPut(sem.slot, kSemExcess, Oop::fromSmallInteger(n - 1));
    return sem.slot;
  }
  Root list(ctx.roots, ensureOc(ctx, sem, kSemList));
  Root sched(ctx.roots, ctx.wk.processor);
  Root active(ctx.roots, hasSlots(ctx.heap, sched.slot, kSchedActive)
                             ? ctx.heap.slotAt(sched.slot, kSchedActive)
                             : Oop{});
  if (active.slot.isHeap()) {
    ocAdd(ctx, list.slot, active.slot);
    NativeMethod::invoke(ctx, ao_Process_suspend, active.slot, nullptr, 0);
    if (hasSlots(ctx.heap, active.slot, kProcMyList)) {
      ctx.heap.slotAtPut(active.slot, kProcMyList, list.slot);
    }
  }
  return sem.slot;
}

Oop ao_SharedQueue_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Oop obj = send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr);
  Root q(ctx.roots, obj);
  if (!hasSlots(ctx.heap, q.slot, kSqWrite)) {
    return q.slot;
  }
  Oop contents = send(ctx, ctx.wk.orderedCollectionClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(q.slot, kSqContents, contents);
  Oop read = send(ctx, ctx.wk.semaphoreClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(q.slot, kSqRead, read);
  Oop write = send(ctx, ctx.wk.semaphoreClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(q.slot, kSqWrite, write);
  NativeMethod::invoke(ctx, ao_Semaphore_signal, write, nullptr, 0);
  return q.slot;
}

Oop ao_SharedQueue_nextPut_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1 || !hasSlots(ctx.heap, receiver, kSqWrite)) {
    return Oop{};
  }
  Root q(ctx.roots, receiver);
  Root value(ctx.roots, args[0]);
  Oop write = ctx.heap.slotAt(q.slot, kSqWrite);
  NativeMethod::invoke(ctx, ao_Semaphore_wait, write, nullptr, 0);
  Oop contents = ensureOc(ctx, q, kSqContents);
  ocAdd(ctx, contents, value.slot);
  Oop read = ctx.heap.slotAt(q.slot, kSqRead);
  NativeMethod::invoke(ctx, ao_Semaphore_signal, read, nullptr, 0);
  return value.slot;
}

Oop ao_SharedQueue_next(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSqWrite)) {
    return Oop{};
  }
  Root q(ctx.roots, receiver);
  Oop read = ctx.heap.slotAt(q.slot, kSqRead);
  NativeMethod::invoke(ctx, ao_Semaphore_wait, read, nullptr, 0);
  Oop contents = ctx.heap.slotAt(q.slot, kSqContents);
  Oop value = ocRemoveFirst(ctx.heap, contents);
  Oop write = ctx.heap.slotAt(q.slot, kSqWrite);
  NativeMethod::invoke(ctx, ao_Semaphore_signal, write, nullptr, 0);
  return value;
}

Oop ao_BlockContext_fork(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  Root blk(ctx.roots, receiver);
  Oop obj = send(ctx, ctx.wk.processClass, ctx.wk.selNew, nullptr, 0, nullptr);
  Root proc(ctx.roots, obj);
  if (!hasSlots(ctx.heap, proc.slot, kProcMyList)) {
    return proc.slot;
  }
  ctx.heap.slotAtPut(proc.slot, kProcContext, blk.slot);
  ctx.heap.slotAtPut(proc.slot, kProcPriority, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(proc.slot, kProcNextLink, Oop::nil());
  NativeMethod::invoke(ctx, ao_Process_resume, proc.slot, nullptr, 0);
  return proc.slot;
}

namespace kernel {

void installProcess(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.processClass, "resume", 0, "ao_Process_resume", ao_Process_resume);
  putNative(heap, wk, wk.processClass, "suspend", 0, "ao_Process_suspend", ao_Process_suspend);
  putNative(heap, wk, wk.processClass, "priority:", 1, "ao_Process_priority_", ao_Process_priority_);

  putNative(heap, wk, wk.processorSchedulerClass, "activeProcess", 0,
            "ao_ProcessorScheduler_activeProcess", ao_ProcessorScheduler_activeProcess);
  putNative(heap, wk, wk.processorSchedulerClass, "yield", 0, "ao_ProcessorScheduler_yield",
            ao_ProcessorScheduler_yield);

  putNative(heap, wk, wk.semaphoreMetaclass, "new", 0, "ao_Semaphore_new", ao_Semaphore_new);
  putNative(heap, wk, wk.semaphoreClass, "signal", 0, "ao_Semaphore_signal", ao_Semaphore_signal);
  putNative(heap, wk, wk.semaphoreClass, "wait", 0, "ao_Semaphore_wait", ao_Semaphore_wait);

  putNative(heap, wk, wk.sharedQueueMetaclass, "new", 0, "ao_SharedQueue_new", ao_SharedQueue_new);
  putNative(heap, wk, wk.sharedQueueClass, "nextPut:", 1, "ao_SharedQueue_nextPut_",
            ao_SharedQueue_nextPut_);
  putNative(heap, wk, wk.sharedQueueClass, "next", 0, "ao_SharedQueue_next", ao_SharedQueue_next);

  putNative(heap, wk, wk.methodContextClass, "sender", 0, "ao_MethodContext_sender",
            ao_MethodContext_sender);
  putNative(heap, wk, wk.methodContextClass, "method", 0, "ao_MethodContext_method",
            ao_MethodContext_method);
  putNative(heap, wk, wk.methodContextClass, "receiver", 0, "ao_MethodContext_receiver",
            ao_MethodContext_receiver);

  putNative(heap, wk, wk.blockContextClass, "fork", 0, "ao_BlockContext_fork", ao_BlockContext_fork);
}

}  // namespace kernel
}  // namespace ao

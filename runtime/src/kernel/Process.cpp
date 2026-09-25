#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Natives.hpp"
#include "ao/Scheduler.hpp"
#include "ao/Send.hpp"

namespace ao {
namespace {

bool hasSlots(Heap& heap, Oop obj, std::uint32_t n) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) == 0 && heap.size(obj) > n;
}

Oop ctxSlot(CallContext& ctx, Oop receiver, std::uint32_t slot) {
  if (!hasSlots(ctx.heap, receiver, slot)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, slot);
}

// SPEC §3.4: the operations are the session scheduler's. A context outside a session has none,
// and there no process can be scheduled.
Oop noScheduler(CallContext& ctx) { return abortEvaluation(ctx, "process cannot run"); }

}  // namespace

// The natives below are thin wrappers of Scheduler (SPEC §3.4). receiver and args are rooted
// slots: after a call that may switch or collect, they hold the current addresses.

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
  // SPEC §3.4: the running process, whatever Smalltalk wrote into the slot.
  if (ctx.scheduler != nullptr && receiver == ctx.wk.processor) {
    return ctx.scheduler->activeProcess();
  }
  return ctxSlot(ctx, receiver, kSchedulerSlotActive);
}

Oop ao_Process_priority_(CallContext& ctx, const Oop& receiver, const Oop* args,
                         std::uint32_t argc) {
  if (argc != 1 || !hasSlots(ctx.heap, receiver, kProcessSlotPriority)) {
    return Oop{};
  }
  // SPEC §3.4: kept, never used for ordering.
  ctx.heap.slotAtPut(receiver, kProcessSlotPriority, args[0]);
  return receiver;
}

Oop ao_Process_resume(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kProcessSlotMyList)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->resume(ctx, receiver) ? receiver : Oop{};
}

Oop ao_Process_suspend(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kProcessSlotMyList)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->suspend(ctx, receiver) ? receiver : Oop{};
}

Oop ao_Process_terminate(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kProcessSlotMyList)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->terminate(ctx, receiver) ? receiver : Oop{};
}

Oop ao_ProcessorScheduler_yield(CallContext& ctx, const Oop& receiver, const Oop*,
                                std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSchedulerSlotActive)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->yield(ctx) ? receiver : Oop{};
}

Oop ao_Semaphore_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Oop obj = send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr);
  Root sem(ctx.roots, obj);
  if (!hasSlots(ctx.heap, sem.slot, kSemaphoreSlotList)) {
    return sem.slot;
  }
  ctx.heap.slotAtPut(sem.slot, kSemaphoreSlotExcess, Oop::fromSmallInteger(0));
  Oop list = send(ctx, ctx.wk.orderedCollectionClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(sem.slot, kSemaphoreSlotList, list);
  return sem.slot;
}

Oop ao_Semaphore_signal(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSemaphoreSlotList)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->signal(ctx, receiver) ? receiver : Oop{};
}

Oop ao_Semaphore_wait(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSemaphoreSlotList)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->wait(ctx, receiver) ? receiver : Oop{};
}

// SPEC §3.4 SharedQueue: readSynch counts the elements no reader has claimed. writeSynch is kept
// for the layout; nextPut: never waits on it.
Oop ao_SharedQueue_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Oop obj = send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr);
  Root q(ctx.roots, obj);
  if (!hasSlots(ctx.heap, q.slot, kSharedQueueSlotWrite)) {
    return q.slot;
  }
  Oop contents = send(ctx, ctx.wk.orderedCollectionClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(q.slot, kSharedQueueSlotContents, contents);
  Oop read = send(ctx, ctx.wk.semaphoreClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(q.slot, kSharedQueueSlotRead, read);
  Oop write = send(ctx, ctx.wk.semaphoreClass, ctx.wk.selNew, nullptr, 0, nullptr);
  ctx.heap.slotAtPut(q.slot, kSharedQueueSlotWrite, write);
  return q.slot;
}

Oop ao_SharedQueue_nextPut_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1 || !hasSlots(ctx.heap, receiver, kSharedQueueSlotWrite)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->nextPut(ctx, receiver, args[0]) ? args[0] : Oop{};
}

Oop ao_SharedQueue_next(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !hasSlots(ctx.heap, receiver, kSharedQueueSlotWrite)) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->next(ctx, receiver);
}

Oop ao_BlockContext_fork(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  if (ctx.scheduler == nullptr) {
    return noScheduler(ctx);
  }
  return ctx.scheduler->fork(ctx, receiver);
}

namespace kernel {

void installProcess(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.processClass, "resume", 0, "ao_Process_resume", ao_Process_resume);
  putNative(heap, wk, wk.processClass, "suspend", 0, "ao_Process_suspend", ao_Process_suspend);
  putNative(heap, wk, wk.processClass, "terminate", 0, "ao_Process_terminate",
            ao_Process_terminate);
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

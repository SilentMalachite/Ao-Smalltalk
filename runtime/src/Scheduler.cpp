#include "ao/Scheduler.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include "Fiber.hpp"
#include "InterpFrame.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string_view>
#include <utility>

namespace ao {
namespace {

// OrderedCollection slots (SPEC §3.6): array firstIndex lastIndex.
constexpr std::uint32_t kOcArray = 0;
constexpr std::uint32_t kOcFirst = 1;
constexpr std::uint32_t kOcLast = 2;

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

// SPEC §3.13: the scheduler's own sends (its lists) never halt the process that runs them: a
// halt there would leave a list and the C++ queue half updated.
struct NoHalt {
  CallContext& ctx;
  explicit NoHalt(CallContext& c) : ctx(c) { ++ctx.haltSuppressed; }
  ~NoHalt() { --ctx.haltSuppressed; }
  NoHalt(const NoHalt&) = delete;
  NoHalt& operator=(const NoHalt&) = delete;
};

// Sends add: (the Kernel native). May collect; unwinding afterwards when it failed, and then the
// collection is as it was.
Oop ocAdd(CallContext& ctx, Oop oc, Oop value) {
  const NoHalt noHalt(ctx);
  Root list(ctx.roots, oc);
  Root v(ctx.roots, value);
  Root sel(ctx.roots, ctx.wk.intern("add:"));
  if (!sel.slot.isHeap()) {
    return abortEvaluation(ctx, "out of memory");
  }
  const Oop added = send(ctx, list.slot, sel.slot, &v.slot, 1, nullptr);
  // A native that fails answers the empty Oop without an abort (an Array that cannot grow: out
  // of memory). Here that is a failed send too (SPEC §3.3), so the caller sees it unwinding.
  if (added.isEmpty() && !unwinding(ctx)) {
    return abortFailedSend(ctx, sel.slot);
  }
  return added;
}

// The first element, left in place; nil when there is none. Does not collect.
Oop ocFirst(Heap& heap, Oop oc) {
  if (ocSize(heap, oc) == 0) {
    return Oop::nil();
  }
  const auto f = heap.slotAt(oc, kOcFirst).smallIntegerValue();
  const Oop arr = heap.slotAt(oc, kOcArray);
  if (f < 1 || !arr.isHeap() || (heap.flags(arr) & kFlagBytes) != 0 ||
      static_cast<std::uint64_t>(f) > heap.size(arr)) {
    return Oop::nil();
  }
  return heap.slotAt(arr, static_cast<std::uint32_t>(f - 1));
}

// The first element, taken out; nil when there is none. Does not collect.
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
  if (f < 1 || idx >= heap.size(arr)) {
    return Oop::nil();
  }
  const Oop value = heap.slotAt(arr, idx);
  heap.slotAtPut(arr, idx, Oop::nil());
  heap.slotAtPut(oc, kOcFirst, Oop::fromSmallInteger(f + 1));
  return value;
}

// Takes the last element out: undoes an add: when what came after it failed. Does not collect.
void ocRemoveLast(Heap& heap, Oop oc) {
  if (!hasSlots(heap, oc, kOcLast)) {
    return;
  }
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!first.isSmallInteger() || !last.isSmallInteger()) {
    return;
  }
  const auto f = first.smallIntegerValue();
  const auto l = last.smallIntegerValue();
  const Oop arr = heap.slotAt(oc, kOcArray);
  if (l < f || l < 1 || !arr.isHeap() || (heap.flags(arr) & kFlagBytes) != 0 ||
      static_cast<std::uint64_t>(l) > heap.size(arr)) {
    return;
  }
  heap.slotAtPut(arr, static_cast<std::uint32_t>(l - 1), Oop::nil());
  heap.slotAtPut(oc, kOcLast, Oop::fromSmallInteger(l - 1));
}

// Takes the first element identical to target out, closing the gap. Does not collect.
bool ocRemoveIdentity(Heap& heap, Oop oc, Oop target) {
  if (!hasSlots(heap, oc, kOcLast)) {
    return false;
  }
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!first.isSmallInteger() || !last.isSmallInteger()) {
    return false;
  }
  const auto f = first.smallIntegerValue();
  const auto l = last.smallIntegerValue();
  if (l < f || f < 1) {
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

// The OrderedCollection in holder's slot, made (OrderedCollection new) when the slot holds
// something else. Empty when holder has no such slot, or when making one failed (unwinding).
Oop ensureOc(CallContext& ctx, Root& holder, std::uint32_t slot) {
  const NoHalt noHalt(ctx);
  if (!hasSlots(ctx.heap, holder.slot, slot)) {
    return Oop{};
  }
  Oop oc = ctx.heap.slotAt(holder.slot, slot);
  if (oc.isHeap() && ctx.wk.classOf(oc) == ctx.wk.orderedCollectionClass) {
    return oc;
  }
  Oop neu = send(ctx, ctx.wk.orderedCollectionClass, ctx.wk.selNew, nullptr, 0, nullptr);
  if (neu.isHeap() && !unwinding(ctx)) {
    ctx.heap.slotAtPut(holder.slot, slot, neu);
    return neu;
  }
  return Oop{};
}

void setMyList(Heap& heap, Oop process, Oop list) {
  if (hasSlots(heap, process, kProcessSlotMyList)) {
    heap.slotAtPut(process, kProcessSlotMyList, list);
  }
}

bool isProcess(Heap& heap, const WellKnown& wk, Oop obj) {
  if (!hasSlots(heap, obj, kProcessSlotMyList)) {
    return false;
  }
  SuperclassWalk walk(heap, wk.classOf(obj));
  for (Oop cls; walk.next(cls);) {
    if (cls == wk.processClass) {
      return true;
    }
  }
  return false;
}

// SPEC §3.4 fork: a block of no arguments.
bool isNoArgBlock(Heap& heap, const WellKnown& wk, Oop obj) {
  return hasSlots(heap, obj, kCtxArgc) && heap.klass(obj) == wk.blockContextClass &&
         heap.slotAt(obj, kCtxArgc) == Oop::fromSmallInteger(0);
}

// error: with msg, as the Kernel's failures with a message do (SPEC §3.3). receiver is a rooted
// slot.
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

}  // namespace

// One process. The base's runs on the session's context and the thread's stack; a fiber's on its
// own context, stack and Roots::Stack. process, block, waitingOn and signaledBy are GC roots for
// as long as the record exists (SPEC §3.2: a live process is never collected).
struct Scheduler::Record {
  Scheduler* owner = nullptr;
  std::uint64_t id = 0;
  Oop process = Oop::nil();
  Oop block = Oop::nil();      // the forked block (an evaluating process's doIt); nil for the base
  Oop waitingOn = Oop::nil();  // the Semaphore whose linkedList holds it, while it waits on one
  // The Semaphore whose signal took it out of the linkedList, until it runs again (returns from
  // wait): a terminate meanwhile gives the signal back (SPEC §3.4).
  Oop signaledBy = Oop::nil();
  std::unique_ptr<CallContext> ownCtx;  // null for the base
  CallContext* ctx = nullptr;
  FiberStack stack;
  FiberRegs regs{};
  Roots::Stack rootStack;  // its LIFO roots while it is parked; empty while it runs
  State state = State::Suspended;
  bool started = false;             // the block has begun (the base: always)
  // Waiting for the process it switched to (a terminate, or the base's evaluation) to end or
  // switch away.
  bool awaitingTerminate = false;
  bool terminateRequested = false;  // unwind with "process terminated" when it runs next
  bool terminated = false;          // ends by terminate: not a failure
  bool abandon = false;             // ends without cleanups (SPEC §3.4 abandon)
  // Abort its blocked operation with the deadlock when it runs next: the base's (SPEC §3.4), or
  // an evaluating process's (SPEC §3.13).
  bool deadlockPending = false;
  bool outOfMemory = false;         // its out-of-memory mark while it does not run (SPEC §3.4)
  bool isEval = false;              // an evaluating process (SPEC §3.13): block is the doIt
  int evalMode = 0;                 // the ao_eval mode it answers for
  // While Halted (SPEC §3.13): why, and whether Proceed and Step may go on.
  std::string haltReason;
  bool proceedable = false;
  Record* resumeTo = nullptr;       // who it goes back to when it next switches away
  bool isBase() const { return ownCtx == nullptr; }
};;

namespace {

void rootRecord(Roots& roots, Oop* process, Oop* block, Oop* waitingOn, Oop* signaledBy) {
  roots.add(process);
  roots.add(block);
  roots.add(waitingOn);
  roots.add(signaledBy);
}

void unrootRecord(Roots& roots, Oop* process, Oop* block, Oop* waitingOn, Oop* signaledBy) {
  roots.remove(signaledBy);
  roots.remove(waitingOn);
  roots.remove(block);
  roots.remove(process);
}

// SPEC §3.10: a pid is never used again in this OS process, not even by a later session.
std::uint64_t nextProcessId() {
  static std::uint64_t next = 1;
  return next++;
}

}  // namespace

Scheduler::Scheduler(CallContext& base) : base_(base) {
  auto rec = std::make_unique<Record>();
  rec->owner = this;
  rec->id = nextProcessId();
  rec->ctx = &base_;
  rec->state = State::Running;
  rec->started = true;
  rootRecord(base_.roots, &rec->process, &rec->block, &rec->waitingOn, &rec->signaledBy);
  base_.roots.attachStack(&rec->rootStack);
  base_.roots.add(&evalValue_);
  current_ = rec.get();
  records_.push_back(std::move(rec));
  base_.scheduler = this;
}

Scheduler::~Scheduler() {
  assert(current_ == &base() && "a scheduler is destroyed from its base process");
  if (current_ == &base()) {
    terminateAll(true);
  }
  Record& b = base();
  base_.roots.remove(&evalValue_);
  unrootRecord(base_.roots, &b.process, &b.block, &b.waitingOn, &b.signaledBy);
  base_.roots.detachStack(&b.rootStack);
  if (base_.scheduler == this) {
    base_.scheduler = nullptr;
  }
}

void Scheduler::adoptImage() {
  assert(current_ == &base() && "adoptImage runs on the base process");
  if (liveFibers() > 0) {
    terminateAll(true);
  }
  CallContext& ctx = base_;
  Heap& heap = ctx.heap;
  Record& b = base();
  Root sched(ctx.roots, ctx.wk.processor);
  const bool shaped = hasSlots(heap, sched.slot, kSchedulerSlotActive);
  Oop proc = shaped ? heap.slotAt(sched.slot, kSchedulerSlotActive) : Oop::nil();
  if (!isProcess(heap, ctx.wk, proc)) {
    // SPEC §3.4 ベースプロセス: not a Process there, so a new one becomes the base.
    proc = send(ctx, ctx.wk.processClass, ctx.wk.selNew, nullptr, 0, nullptr);
    if (unwinding(ctx)) {
      clearUnwinding(ctx);
    }
    if (!proc.isHeap()) {
      proc = Oop::nil();
    }
  }
  b.process = proc;
  setMyList(heap, b.process, Oop::nil());
  if (hasSlots(heap, sched.slot, kSchedulerSlotActive)) {
    heap.slotAtPut(sched.slot, kSchedulerSlotActive, b.process);
    // SPEC §3.11: the ready queue starts empty; what the image had there cannot run.
    const Oop q = heap.slotAt(sched.slot, kSchedulerSlotQuiescent);
    for (auto n = ocSize(heap, q); n > 0; --n) {
      const Oop p = ocRemoveFirst(heap, q);
      if (hasSlots(heap, p, kProcessSlotMyList) && heap.slotAt(p, kProcessSlotMyList) == q) {
        setMyList(heap, p, Oop::nil());
      }
    }
  }
  ready_.clear();
  b.state = State::Running;
}

Oop Scheduler::fork(CallContext& ctx, Oop block) {
  Heap& heap = ctx.heap;
  WellKnown& wk = ctx.wk;
  Root blk(ctx.roots, block);
  if (!isNoArgBlock(heap, wk, blk.slot)) {
    return Oop{};
  }
  if (current_->abandon) {
    return Oop{};
  }
  // SPEC §3.4: no Process is made past the limit.
  if (liveFibers() >= kMaxFibers) {
    return abortEvaluation(ctx, "too many processes");
  }
  Root proc(ctx.roots, send(ctx, wk.processClass, wk.selNew, nullptr, 0, nullptr));
  if (unwinding(ctx) || !hasSlots(heap, proc.slot, kProcessSlotMyList)) {
    return Oop{};
  }
  heap.slotAtPut(proc.slot, kProcessSlotSuspendedContext, blk.slot);
  Record* r = addFiber(ctx, proc.slot, blk.slot);
  if (r == nullptr) {
    return Oop{};
  }
  if (!enqueue(ctx, *r)) {
    r->state = State::Dead;
    reapDead();
    return Oop{};
  }
  return r->process;
}

// A new fiber's record for the Process `process` (its slots set here but the suspendedContext)
// running `block`, left suspended in no list. Null when no stack can be had: ctx then aborts with
// too many processes (SPEC §3.4 fork).
Scheduler::Record* Scheduler::addFiber(CallContext& ctx, Oop process, Oop block) {
  Heap& heap = ctx.heap;
  heap.slotAtPut(process, kProcessSlotNextLink, Oop::nil());
  heap.slotAtPut(process, kProcessSlotPriority, Oop::fromSmallInteger(0));
  heap.slotAtPut(process, kProcessSlotMyList, Oop::nil());
  FiberStack stack = FiberStack::acquire();
  if (!stack.valid()) {
    abortEvaluation(ctx, "too many processes");
    return nullptr;
  }
  auto rec = std::make_unique<Record>();
  Record& r = *rec;
  r.owner = this;
  r.id = nextProcessId();
  r.stack = std::move(stack);
  r.ownCtx.reset(new CallContext{base_.heap, base_.roots, base_.wk, base_.cache});
  r.ctx = r.ownCtx.get();
  r.ctx->scheduler = this;
  r.ctx->fiberStackLow = reinterpret_cast<std::uintptr_t>(r.stack.low());
  r.ctx->fiberStackHigh = reinterpret_cast<std::uintptr_t>(r.stack.high());
  r.process = process;
  r.block = block;
  rootRecord(base_.roots, &r.process, &r.block, &r.waitingOn, &r.signaledBy);
  base_.roots.attachStack(&r.rootStack);
  fiberInit(r.regs, r.stack, &Scheduler::fiberEntry, &r);
  records_.push_back(std::move(rec));
  return &r;
}

std::uint64_t Scheduler::forkEval(CallContext& ctx, Oop method, int mode, bool debugIt) {
  assert(current_ == &base() && "forkEval runs on the base process");
  Heap& heap = ctx.heap;
  WellKnown& wk = ctx.wk;
  Root meth(ctx.roots, method);
  // SPEC §3.13: an evaluating process counts against the limit as a fork does.
  if (liveFibers() >= kMaxFibers) {
    abortEvaluation(ctx, "too many processes");
    return 0;
  }
  Root proc(ctx.roots, send(ctx, wk.processClass, wk.selNew, nullptr, 0, nullptr));
  if (unwinding(ctx)) {
    return 0;
  }
  if (!hasSlots(heap, proc.slot, kProcessSlotMyList)) {
    abortEvaluation(ctx, "evaluation failed");
    return 0;
  }
  heap.slotAtPut(proc.slot, kProcessSlotSuspendedContext, Oop::nil());
  Record* r = addFiber(ctx, proc.slot, meth.slot);
  if (r == nullptr) {
    return 0;
  }
  r->isEval = true;
  r->evalMode = mode;
  if (debugIt) {
    r->ctx->stepMode = StepMode::DebugIt;
  }
  return r->id;
}

Scheduler::EvalEnd Scheduler::awaitEval(std::uint64_t pid) {
  assert(current_ == &base() && "awaitEval runs on the base process");
  awaited_ = pid;
  evalEnd_ = EvalEnd::Failed;
  evalReason_ = "evaluation failed";
  evalValue_ = Oop::nil();
  if (Record* r = findId(pid); r != nullptr && r->state != State::Dead) {
    runAwaited(*r);
  }
  for (;;) {
    Record* r = findId(pid);
    if (r == nullptr || r->state == State::Dead) {
      break;  // runFiber set the outcome
    }
    if (r->state == State::Halted) {
      evalEnd_ = EvalEnd::Halted;
      evalReason_ = r->haltReason;
      break;
    }
    // The base's own abort (a deadlock while it sat in the queue) is not the evaluation's.
    if (unwinding(base_)) {
      clearUnwinding(base_);
    }
    if (ready_.empty()) {
      // SPEC §3.13: it waits and nothing else can run. Its operation is undone and it aborts with
      // the deadlock, on itself, so its cleanups run there.
      leaveLists(*r);
      r->deadlockPending = true;
      runAwaited(*r);
      continue;
    }
    yield(base_);
  }
  if (unwinding(base_)) {
    clearUnwinding(base_);
  }
  awaited_ = 0;
  return evalEnd_;
}

// Switches from the base to r (in no list) until r ends or switches away: the base waits for it
// the way a terminate waits (takeNext brings it back).
void Scheduler::runAwaited(Record& r) {
  Record& b = base();
  r.resumeTo = &b;
  b.state = State::Waiting;
  b.awaitingTerminate = true;
  switchTo(r);
  afterResume(base_, b);
}

bool Scheduler::runningEval() const { return current_->isEval; }


bool Scheduler::canHalt(const CallContext& ctx) const {
  const Record& me = *current_;
  return me.isEval && me.id == awaited_ && me.ctx == &ctx && !ctx.aborting && !ctx.abandoning &&
         ctx.abortSetAside == 0 && ctx.haltSuppressed == 0 && !me.abandon &&
         !me.terminateRequested && haltedCount() < kMaxHalted;
}

bool Scheduler::halt(CallContext& ctx, std::string reason, bool proceedable) {
  Record& me = *current_;
  assert(canHalt(ctx) && "halt needs canHalt");
  // Any halt ends a step: the next Step sets its own mark.
  ctx.stepMode = StepMode::None;
  me.haltReason = std::move(reason);
  me.proceedable = proceedable;
  me.state = State::Halted;
  me.resumeTo = nullptr;
  lastHalted_ = me.id;
  // SPEC §3.13: back to the base, which waits for this process or sits in the ready queue.
  Record& b = base();
  leaveLists(b);
  switchTo(b);
  me.haltReason.clear();
  me.proceedable = false;
  if (!afterResume(ctx, me)) {
    return false;
  }
  ++ctx.haltProceeds;
  ctx.proceededAt = ctx.topFrame;
  return true;
}

std::size_t Scheduler::haltedCount() const {
  std::size_t n = 0;
  for (const auto& r : records_) {
    if (r->state == State::Halted) {
      ++n;
    }
  }
  return n;
}

std::uint64_t Scheduler::lastHaltedPid() const { return isHalted(lastHalted_) ? lastHalted_ : 0; }

const CallContext* Scheduler::haltedContext(std::uint64_t pid, const std::string** reason) const {
  const Record* r = pid != 0 ? findId(pid) : nullptr;
  if (r == nullptr || r->state != State::Halted) {
    return nullptr;
  }
  if (reason != nullptr) {
    *reason = &r->haltReason;
  }
  return r->ctx;
}

bool Scheduler::canProceed(std::uint64_t pid) const {
  const Record* r = pid != 0 ? findId(pid) : nullptr;
  return r != nullptr && r->state == State::Halted && r->proceedable;
}

bool Scheduler::isHalted(std::uint64_t pid) const {
  const Record* r = pid != 0 ? findId(pid) : nullptr;
  return r != nullptr && r->state == State::Halted;
}


int Scheduler::evalModeOf(std::uint64_t pid) const {
  const Record* r = pid != 0 ? findId(pid) : nullptr;
  return r != nullptr && r->isEval ? r->evalMode : 0;
}

Scheduler::EvalEnd Scheduler::proceed(std::uint64_t pid) {
  assert(current_ == &base() && "proceed runs on the base process");
  Record* r = findId(pid);
  assert(r != nullptr && r->state == State::Halted && r->proceedable && "proceed needs canProceed");
  // In no list: awaitEval switches to it, and its halt returns true.
  r->state = State::Suspended;
  return awaitEval(pid);
}

Scheduler::EvalEnd Scheduler::step(std::uint64_t pid, StepMode mode) {
  Record* r = findId(pid);
  assert(r != nullptr && r->state == State::Halted && r->proceedable && "step needs canProceed");
  CallContext& c = *r->ctx;
  c.stepMode = mode;
  c.stepDepth = c.topFrame != nullptr ? c.topFrame->depth : 0;
  return proceed(pid);
}

bool Scheduler::abortHalted(std::uint64_t pid) {
  assert(current_ == &base() && "abortHalted runs on the base process");
  Record* r = findId(pid);
  if (r == nullptr || r->state != State::Halted) {
    return false;
  }
  // SPEC §3.13: a terminate from the base. It comes back when the process ends or first switches
  // away (a cleanup that waits); the rest runs in the drains to come.
  terminate(base_, r->process);
  if (unwinding(base_)) {
    clearUnwinding(base_);
  }
  return true;
}

bool Scheduler::yield(CallContext& ctx) {
  Record& me = *current_;
  if (ready_.empty() || me.abandon) {
    return !me.abandon;
  }
  if (!enqueue(ctx, me)) {
    return false;
  }
  // The queue held another process before this one went to its end.
  Record* next = takeNext(me);
  switchTo(*next);
  return afterResume(ctx, me);
}

bool Scheduler::wait(CallContext& ctx, Oop semaphore) {
  Heap& heap = ctx.heap;
  Root sem(ctx.roots, semaphore);
  if (!hasSlots(heap, sem.slot, kSemaphoreSlotList)) {
    return false;
  }
  const Oop excess = heap.slotAt(sem.slot, kSemaphoreSlotExcess);
  const auto n = excess.isSmallInteger() ? excess.smallIntegerValue() : 0;
  if (n > 0) {
    heap.slotAtPut(sem.slot, kSemaphoreSlotExcess, Oop::fromSmallInteger(n - 1));
    return true;
  }
  Record& me = *current_;
  if (me.abandon) {
    return false;
  }
  Root list(ctx.roots, ensureOc(ctx, sem, kSemaphoreSlotList));
  if (unwinding(ctx) || !list.slot.isHeap()) {
    return false;
  }
  ocAdd(ctx, list.slot, me.process);
  if (unwinding(ctx)) {
    return false;
  }
  setMyList(heap, me.process, list.slot);
  me.state = State::Waiting;
  me.waitingOn = sem.slot;
  return block(ctx, me);
}

bool Scheduler::signal(CallContext& ctx, Oop semaphore) {
  Heap& heap = ctx.heap;
  Root sem(ctx.roots, semaphore);
  if (!hasSlots(heap, sem.slot, kSemaphoreSlotList)) {
    return false;
  }
  for (auto n = ocSize(heap, heap.slotAt(sem.slot, kSemaphoreSlotList)); n > 0; --n) {
    const Oop list = heap.slotAt(sem.slot, kSemaphoreSlotList);
    const Oop waiter = ocFirst(heap, list);
    Record* r = find(waiter);
    if (r != nullptr && r->state == State::Waiting && r->waitingOn == sem.slot) {
      // Ready at the end of the queue; the signaller goes on (SPEC §3.4). The queue takes it
      // first: when it cannot (out of memory), the waiter still waits and nothing changed.
      if (!enqueue(ctx, *r)) {
        return false;
      }
      ocRemoveIdentity(heap, heap.slotAt(sem.slot, kSemaphoreSlotList), r->process);
      r->waitingOn = Oop::nil();
      r->signaledBy = sem.slot;
      return true;
    }
    // SPEC §3.4, §3.11: a waiter that cannot run is dropped.
    ocRemoveFirst(heap, list);
    if (hasSlots(heap, waiter, kProcessSlotMyList) &&
        heap.slotAt(waiter, kProcessSlotMyList) == list) {
      setMyList(heap, waiter, Oop::nil());
    }
  }
  const Oop excess = heap.slotAt(sem.slot, kSemaphoreSlotExcess);
  const auto n = excess.isSmallInteger() ? excess.smallIntegerValue() : 0;
  // excessSignals は Smalltalk から書き換えられる。+1 が SmallInteger を超えるなら失敗する。
  if (n >= kSmiMax) {
    fail(ctx, sem.slot, "signal: excess signals out of range");
    return false;
  }
  heap.slotAtPut(sem.slot, kSemaphoreSlotExcess, Oop::fromSmallInteger(n + 1));
  return true;
}

bool Scheduler::suspend(CallContext& ctx, Oop process) {
  Record* r = find(process);
  // SPEC §3.13: a halted process stays halted; only the debugger or a terminate moves it.
  if (r == nullptr || r->state == State::Dead || r->state == State::Suspended ||
      r->state == State::Halted) {
    return true;
  }
  if (r != current_) {
    // Out of the ready queue or the linkedList; a waiter resumed later returns from wait.
    leaveLists(*r);
    return true;
  }
  if (r->abandon) {
    return false;
  }
  r->state = State::Suspended;
  return block(ctx, *r);
}

bool Scheduler::resume(CallContext& ctx, Oop process) {
  Record* r = find(process);
  if (r == nullptr || r->state == State::Dead) {
    abortEvaluation(ctx, "process cannot run");
    return false;
  }
  // Running, ready and waiting processes stay as they are: never twice in the queue.
  if (r->state != State::Suspended) {
    return true;
  }
  return enqueue(ctx, *r);
}

bool Scheduler::terminate(CallContext& ctx, Oop process) {
  Record* r = find(process);
  if (r == nullptr || r->state == State::Dead) {
    return true;
  }
  Record& me = *current_;
  if (r->isBase() || r == &me) {
    // SPEC §3.4: the base never ends; sending this fails the sender. A fiber's own terminate
    // unwinds it to its body, which is no failure (SPEC §3.13: not captured).
    if (r == &me && !me.isBase()) {
      me.terminated = true;
      abortEvaluationQuiet(ctx, "process terminated");
    } else {
      abortEvaluation(ctx, "process terminated");
    }
    return false;
  }
  if (me.abandon) {
    return false;
  }
  if (r->signaledBy.isHeap()) {
    // SPEC §3.4 terminate: signalled but not back from wait yet, it gives the signal back, as a
    // second signal would. When that fails, the process stays as it was.
    Root sem(ctx.roots, r->signaledBy);
    if (!signal(ctx, sem.slot)) {
      return false;
    }
    r->signaledBy = Oop::nil();
  }
  leaveLists(*r);
  if (!r->started) {
    r->state = State::Dead;
    r->terminated = true;
    reapDead();
    return true;
  }
  // It unwinds on itself, and comes back here when it ends or first switches away.
  r->terminateRequested = true;
  r->resumeTo = &me;
  me.state = State::Waiting;
  me.awaitingTerminate = true;
  switchTo(*r);
  return afterResume(ctx, me);
}

bool Scheduler::nextPut(CallContext& ctx, Oop queue, Oop value) {
  // SPEC §3.13: the element is taken out again when the signal fails; no halt in between.
  const NoHalt noHalt(ctx);
  Heap& heap = ctx.heap;
  Root q(ctx.roots, queue);
  Root v(ctx.roots, value);
  if (!hasSlots(heap, q.slot, kSharedQueueSlotWrite) ||
      !hasSlots(heap, heap.slotAt(q.slot, kSharedQueueSlotRead), kSemaphoreSlotList)) {
    return false;
  }
  Root contents(ctx.roots, ensureOc(ctx, q, kSharedQueueSlotContents));
  if (unwinding(ctx) || !contents.slot.isHeap()) {
    return false;
  }
  ocAdd(ctx, contents.slot, v.slot);
  if (unwinding(ctx)) {
    return false;
  }
  // SPEC §3.4: never waits (writeSynch is not used); wakes one reader, if any. When that fails,
  // the element goes out again: the failed nextPut: adds nothing. An exception (the ready queue
  // could not grow) takes it out too on its way to the fiber's entry or the ABI.
  const Oop read = heap.slotAt(q.slot, kSharedQueueSlotRead);
  bool woke = false;
  try {
    woke = hasSlots(heap, read, kSemaphoreSlotList) && signal(ctx, read);
  } catch (...) {
    ocRemoveLast(heap, contents.slot);
    throw;
  }
  if (!woke) {
    ocRemoveLast(heap, contents.slot);
    return false;
  }
  return true;
}

Oop Scheduler::next(CallContext& ctx, Oop queue) {
  Heap& heap = ctx.heap;
  Root q(ctx.roots, queue);
  if (!hasSlots(heap, q.slot, kSharedQueueSlotWrite)) {
    return Oop{};
  }
  for (;;) {
    const Oop read = heap.slotAt(q.slot, kSharedQueueSlotRead);
    if (!hasSlots(heap, read, kSemaphoreSlotList)) {
      return Oop{};
    }
    if (!wait(ctx, read)) {
      return Oop{};
    }
    // Other processes ran: read the queue again. A waiter resumed without a signal (suspend and
    // resume, SPEC §3.4) takes an element when there is one, and waits again when it is empty.
    const Oop contents = heap.slotAt(q.slot, kSharedQueueSlotContents);
    if (ocSize(heap, contents) > 0) {
      return ocRemoveFirst(heap, contents);
    }
  }
}

void Scheduler::drain(int rounds) {
  assert(current_ == &base() && "drain runs on the base process");
  for (int i = 0; i < rounds && !ready_.empty(); ++i) {
    yield(base_);
    if (unwinding(base_)) {
      clearUnwinding(base_);
    }
  }
  if (unwinding(base_)) {
    clearUnwinding(base_);
  }
}

void Scheduler::terminateAll(bool abandon) {
  assert(current_ == &base() && "terminateAll runs on the base process");
  // SPEC §4.4: each live process not terminated yet is terminated (its cleanups run on it). A
  // cleanup that switched left it ready or blocked, and may have forked others: the drain lets
  // the ready ones go on, and the next pass terminates what the cleanups forked. The passes stop
  // once one neither terminates nor ends a process, or after kDrainRounds of them.
  for (int pass = 0; !abandon && pass < kDrainRounds; ++pass) {
    const std::size_t liveBefore = liveFibers();
    std::vector<std::uint64_t> ids;
    for (const auto& r : records_) {
      if (!r->isBase() && r->state != State::Dead && !r->terminated) {
        ids.push_back(r->id);
      }
    }
    bool terminatedOne = false;
    for (const std::uint64_t id : ids) {
      Record* r = findId(id);
      if (r == nullptr || r->state == State::Dead || r->terminated) {
        continue;
      }
      terminate(base_, r->process);
      if (unwinding(base_)) {
        clearUnwinding(base_);
      }
      r = findId(id);
      terminatedOne = terminatedOne || r == nullptr || r->state == State::Dead || r->terminated;
    }
    drain(kDrainRounds);
    const std::size_t liveAfter = liveFibers();
    if (liveAfter == 0 || (!terminatedOne && liveAfter >= liveBefore)) {
      break;
    }
  }
  // What a cleanup left blocked or the passes did not finish, or everything when abandoning.
  abandonAll();
  if (unwinding(base_)) {
    clearUnwinding(base_);
  }
}

bool Scheduler::nonBaseRunning() const { return current_ != &base(); }

std::size_t Scheduler::liveFibers() const {
  std::size_t n = 0;
  for (const auto& r : records_) {
    if (!r->isBase() && r->state != State::Dead) {
      ++n;
    }
  }
  return n;
}

Oop Scheduler::activeProcess() const { return current_->process; }

void Scheduler::fiberEntry(void* arg) {
  fiberEntered();
  Record& me = *static_cast<Record*>(arg);
  Scheduler& s = *me.owner;
  s.runFiber(me);
  s.finishFiber(me);
}

// SPEC §3.4 最外: the body of a forked process. Clears what is left over, takes the stack range
// of the fiber, and sends value to the block. An abort that ends it is its failure, unless it was
// terminated or abandoned.
void Scheduler::runFiber(Record& me) {
  reapDead();
  me.started = true;
  CallContext& ctx = *me.ctx;
  assert(base_.roots.runningStack().empty() && "a fiber starts with no LIFO roots");
  try {
    clearUnwinding(ctx);
    refreshStackLimit(ctx);
    if (me.isEval) {
      // SPEC §3.13: the doIt is applied, not sent value; its outcome is the base's to answer.
      const Oop result = applyMethod(ctx, me.block, Oop::nil(), nullptr, 0, Oop::nil());
      endEval(me, ctx, result);
    } else {
      const Oop result = send(ctx, me.block, base_.wk.selValue, nullptr, 0, nullptr);
      if (!me.terminated && !me.abandon) {
        if (result.isEmpty() && ctx.aborting) {
          recordFailure(abortReasonText(ctx));
        } else if (base_.heap.outOfMemory()) {
          // SPEC §3.4: an allocation failed, yet the body ran to its end without an abort.
          recordFailure("out of memory");
        } else if (result.isEmpty()) {
          // Not an abort: a ^ whose home is in another process came back to the body (SPEC §3.4).
          recordFailure("non-local return to another process");
        }
      }
    }
  } catch (...) {
    // SPEC §3.4 プロセスの失敗: no exception crosses a fiber's entry (Fiber.hpp). The native
    // frames on the way were popped as it unwound.
    if (me.isEval) {
      clearUnwinding(ctx);
      abortEvaluationQuiet(ctx, "internal error");
      endEval(me, ctx, Oop{});
    } else if (!me.terminated && !me.abandon) {
      recordFailure("internal error");
    }
  }
  clearUnwinding(ctx);
  ctx.abandoning = false;
  base_.heap.clearOutOfMemory();
  assert(base_.roots.runningStack().empty() && "a fiber ends with no LIFO roots");
}

// SPEC §3.13: how the evaluating process me ended, for the base that waits for it. Nothing when
// no one waits (it was abandoned, or aborted while halted).
void Scheduler::endEval(Record& me, CallContext& ctx, Oop result) {
  if (me.abandon || me.id != awaited_) {
    return;
  }
  evalEnd_ = EvalEnd::Failed;
  if (me.terminated) {
    evalReason_ = "process terminated";
  } else if (ctx.aborting) {
    evalReason_ = abortReasonText(ctx);
    if (evalReason_.empty()) {
      evalReason_ = "evaluation aborted";
    }
  } else if (base_.heap.outOfMemory()) {
    evalReason_ = "out of memory";
  } else if (result.isEmpty()) {
    evalReason_ = "evaluation failed";
  } else {
    evalEnd_ = EvalEnd::Finished;
    evalValue_ = result;
  }
}

// The fiber is done: it switches away for good, to whoever it goes back to, else to the next
// ready process, else to the base (SPEC §3.4 デッドロック). The one it switches to releases its
// stack (reapDead). No root may live in this frame: it is never unwound.
void Scheduler::finishFiber(Record& me) {
  me.state = State::Dead;
  setMyList(base_.heap, me.process, Oop::nil());
  Record* next = takeNext(me);
  if (next == nullptr) {
    Record& b = base();
    leaveLists(b);
    b.deadlockPending = true;
    next = &b;
  }
  switchTo(*next);
  std::abort();  // a finished fiber is never switched back to
}

Scheduler::Record& Scheduler::base() const { return *records_.front(); }

Scheduler::Record* Scheduler::find(Oop process) const {
  for (const auto& r : records_) {
    if (r->process == process) {
      return r.get();
    }
  }
  return nullptr;
}

Scheduler::Record* Scheduler::findId(std::uint64_t id) const {
  for (const auto& r : records_) {
    if (r->id == id) {
      return r.get();
    }
  }
  return nullptr;
}

// Puts r at the end of the ready queue: the C++ queue, and quiescentProcesses for Smalltalk. May
// collect; false (unwinding) when the OrderedCollection could not take it, and r is unchanged.
bool Scheduler::enqueue(CallContext& ctx, Record& r) {
  Heap& heap = ctx.heap;
  // The C++ queue first: if growing it throws, nothing has changed yet.
  ready_.push_back(&r);
  Root sched(ctx.roots, ctx.wk.processor);
  if (hasSlots(heap, sched.slot, kSchedulerSlotActive)) {
    Root list(ctx.roots, ensureOc(ctx, sched, kSchedulerSlotQuiescent));
    if (unwinding(ctx)) {
      ready_.pop_back();
      return false;
    }
    if (list.slot.isHeap()) {
      ocAdd(ctx, list.slot, r.process);
      if (unwinding(ctx)) {
        ready_.pop_back();
        return false;
      }
      setMyList(heap, r.process, list.slot);
    }
  }
  r.state = State::Ready;
  return true;
}

// Takes r out of the list it is in (ready queue, linkedList, or waiting for a terminate) and
// leaves it suspended, in none. Does not collect.
void Scheduler::leaveLists(Record& r) {
  Heap& heap = base_.heap;
  const Oop sched = base_.wk.processor;
  if (r.state == State::Ready) {
    ready_.erase(std::remove(ready_.begin(), ready_.end(), &r), ready_.end());
    if (hasSlots(heap, sched, kSchedulerSlotActive)) {
      ocRemoveIdentity(heap, heap.slotAt(sched, kSchedulerSlotQuiescent), r.process);
    }
  } else if (r.state == State::Waiting) {
    if (hasSlots(heap, r.waitingOn, kSemaphoreSlotList)) {
      ocRemoveIdentity(heap, heap.slotAt(r.waitingOn, kSemaphoreSlotList), r.process);
    }
    r.waitingOn = Oop::nil();
    if (r.awaitingTerminate) {
      for (const auto& o : records_) {
        if (o->resumeTo == &r) {
          o->resumeTo = nullptr;
        }
      }
      r.awaitingTerminate = false;
    }
  }
  setMyList(heap, r.process, Oop::nil());
  if (r.state != State::Dead) {
    r.state = State::Suspended;
  }
}

// Whom me switches to when it stops running: the process waiting for it to end or switch (a
// terminate), else the first ready one. Null when there is none.
Scheduler::Record* Scheduler::takeNext(Record& me) {
  Record* to = me.resumeTo;
  me.resumeTo = nullptr;
  if (to != nullptr && to->state == State::Waiting && to->awaitingTerminate) {
    to->awaitingTerminate = false;
    to->state = State::Suspended;
    return to;
  }
  if (ready_.empty()) {
    return nullptr;
  }
  Record* next = ready_.front();
  leaveLists(*next);
  return next;
}

// me (running) has just gone into a list or out of all of them: switch to the next process. With
// none, SPEC §3.4 デッドロック: the base's operation is undone and the base aborts, on the spot
// when me is the base, else once this switches back to it.
bool Scheduler::block(CallContext& ctx, Record& me) {
  Record* next = takeNext(me);
  if (next == nullptr) {
    Record& b = base();
    leaveLists(b);
    if (&me == &b) {
      b.state = State::Running;
      abortEvaluation(ctx, "deadlock: no runnable process");
      return false;
    }
    b.deadlockPending = true;
    next = &b;
  }
  switchTo(*next);
  return afterResume(ctx, me);
}

// me runs again, after a switch to it. Others have run and collected. Starts the unwinding asked
// for meanwhile: abandon, terminate or a deadlock of the base.
bool Scheduler::afterResume(CallContext& ctx, Record& me) {
  reapDead();
  if (me.abandon) {
    // SPEC §3.4 abandon: no cleanup, no Smalltalk code, no hook.
    ctx.abandoning = true;
    ctx.transcriptHook = nullptr;
    ctx.inspectHook = nullptr;
    ctx.bindingHook = nullptr;
    ctx.debug = nullptr;
    abortEvaluationQuiet(ctx, "process terminated");
    return false;
  }
  if (me.terminateRequested) {
    // Terminated by another process: no failure (SPEC §3.13: not captured).
    me.terminateRequested = false;
    me.terminated = true;
    abortEvaluationQuiet(ctx, "process terminated");
    return false;
  }
  if (me.deadlockPending) {
    me.deadlockPending = false;
    abortEvaluation(ctx, "deadlock: no runnable process");
    return false;
  }
  return true;
}

// Makes `to` the running process: counters, hooks, Smalltalk's view, the LIFO roots, then the
// machine registers. Allocates nothing and runs no Smalltalk, so nothing collects in between.
// Returns when some later switch comes back to the process that called it; a Dead one never
// comes back.
void Scheduler::switchTo(Record& to) {
  Record& from = *current_;
  assert(&to != &from && to.state != State::Dead && "switchTo takes another live process");
  Heap& heap = base_.heap;
  if (!from.isBase()) {
    // SPEC §3.4: the counters are the session's.
    CallContext& f = *from.ctx;
    base_.interpretedBytecodes += f.interpretedBytecodes;
    base_.interpretedSends += f.interpretedSends;
    base_.testFailures += f.testFailures;
    f.interpretedBytecodes = 0;
    f.interpretedSends = 0;
    f.testFailures = 0;
  }
  if (!to.isBase() && !to.abandon) {
    to.ctx->transcriptHook = base_.transcriptHook;
    to.ctx->inspectHook = base_.inspectHook;
    to.ctx->bindingHook = base_.bindingHook;
    to.ctx->debug = base_.debug;
    to.ctx->statementHook = base_.statementHook;
  }
  if (hasSlots(heap, base_.wk.processor, kSchedulerSlotActive)) {
    heap.slotAtPut(base_.wk.processor, kSchedulerSlotActive, to.process);
  }
  setMyList(heap, to.process, Oop::nil());
  // SPEC §3.4: the out-of-memory mark is each process's own, like its abort. The heap holds the
  // running one's.
  from.outOfMemory = heap.outOfMemory();
  if (to.outOfMemory) {
    heap.setOutOfMemory();
  } else {
    heap.clearOutOfMemory();
  }
  // Back from its wait (or unwinding): a signal it took is its own now.
  to.signaledBy = Oop::nil();
  to.state = State::Running;
  current_ = &to;
  base_.roots.switchStack(from.rootStack, to.rootStack);
  const FiberStackBounds bounds = to.isBase() ? threadStackBounds() : to.stack.bounds();
  if (from.state == State::Dead) {
    fiberSwitchFinal(from.regs, to.regs, bounds);
  }
  fiberSwitch(from.regs, to.regs, bounds);
}

// Releases the stacks and roots of finished fibers other than the running one: a fiber's last
// switch runs on its own stack, so whoever runs after it frees it.
void Scheduler::reapDead() {
  for (auto it = records_.begin(); it != records_.end();) {
    Record& r = **it;
    if (&r == current_ || r.isBase() || r.state != State::Dead) {
      ++it;
      continue;
    }
    for (const auto& o : records_) {
      if (o->resumeTo == &r) {
        o->resumeTo = nullptr;
      }
    }
    base_.roots.detachStack(&r.rootStack);
    unrootRecord(base_.roots, &r.process, &r.block, &r.waitingOn, &r.signaledBy);
    FiberStack::release(std::move(r.stack));
    it = records_.erase(it);
  }
}

// SPEC §3.4 abandon: each live fiber unwinds its C++ frames (the abort runs no cleanup) and ends;
// one not started just ends. Only from the base.
void Scheduler::abandonAll() {
  Record& b = base();
  for (;;) {
    Record* r = nullptr;
    for (const auto& o : records_) {
      if (!o->isBase() && o->state != State::Dead) {
        r = o.get();
        break;
      }
    }
    if (r == nullptr) {
      return;
    }
    leaveLists(*r);
    r->abandon = true;
    if (!r->started) {
      r->state = State::Dead;
      reapDead();
      continue;
    }
    r->resumeTo = &b;
    b.state = State::Waiting;
    b.awaitingTerminate = true;
    switchTo(*r);
    reapDead();
  }
}

void Scheduler::recordFailure(std::string reason) {
  ++failures_;
  lastFailure_ = reason.empty() ? std::string("evaluation aborted") : std::move(reason);
}

}  // namespace ao

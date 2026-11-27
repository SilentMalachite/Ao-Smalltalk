#include "ao/Scheduler.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/DebugSnapshot.hpp"
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
#include <iterator>
#include <string_view>
#include <utility>

namespace ao {
namespace {

// What a Scheduler list (a linkedList, the ready queue, a SharedQueue's contents) says, read
// without a send by its slots; its class is not consulted (SPEC §3.4). One with fewer than three
// pointer slots (nil, a String, #(7 8)) has no elements; otherwise as ocRead (SPEC §3.6), so
// #(7 8 9) is damaged. Damaged: the natives that read the list fail (signal and wait on their
// linkedList, next on its contents, suspend and terminate of a process that is not running), but
// a running process that waits or stops switches past a damaged ready queue, and Processor yield
// with no other ready process returns without reading it; the send-less walks see no elements;
// adoption resets a damaged ready queue that is an instance of OrderedCollection itself (§3.4).
OcShape listShape(const Heap& heap, const WellKnown& wk, Oop oc, OcSlots* out) {
  if (!hasSlots(heap, oc, kOcLast)) {
    return OcShape::Empty;
  }
  return ocRead(heap, wk, oc, out);
}

std::int64_t ocSize(Heap& heap, const WellKnown& wk, Oop oc) {
  OcSlots s;
  return listShape(heap, wk, oc, &s) == OcShape::Sound ? s.last - s.first + 1 : 0;
}

// SPEC §3.13: the scheduler never halts the running process while it updates its lists. They send
// nothing (SPEC §3.4), but the add: native's failure on a damaged list goes through error:, and a
// halt there would leave a list and the C++ queue half updated.
struct NoHalt {
  CallContext& ctx;
  explicit NoHalt(CallContext& c) : ctx(c) { ++ctx.haltSuppressed; }
  ~NoHalt() { --ctx.haltSuppressed; }
  NoHalt(const NoHalt&) = delete;
  NoHalt& operator=(const NoHalt&) = delete;
};

// Adds value to oc, an instance of OrderedCollection itself (what ensureOc answers). SPEC §3.4:
// the add: native is called directly, with no lookup and no send, so no Smalltalk runs while a
// process is half queued, whatever a method dictionary was swapped for. NoHalt still matters: the
// native's failure on a damaged oc goes through error:. May collect; unwinding afterwards when it
// failed, and then the collection is as it was.
Oop ocAdd(CallContext& ctx, Oop oc, Oop value) {
  const NoHalt noHalt(ctx);
  // invoke roots oc and value for the native.
  const Oop added = NativeMethod::invoke(ctx, ao_OrderedCollection_add_, oc, &value, 1);
  // The native answers the empty Oop without an abort when the Array cannot grow (out of
  // memory). Here that is a failed add: (SPEC §3.3), so the caller sees it unwinding.
  if (added.isEmpty() && !unwinding(ctx)) {
    const Oop sel = ctx.wk.intern("add:");
    return sel.isHeap() ? abortFailedSend(ctx, sel) : abortEvaluation(ctx, "out of memory");
  }
  return added;
}

// The first element, left in place; nil when there is none. Does not collect.
Oop ocFirst(Heap& heap, const WellKnown& wk, Oop oc) {
  OcSlots s;
  if (listShape(heap, wk, oc, &s) != OcShape::Sound || s.last < s.first) {
    return Oop::nil();
  }
  return heap.slotAt(s.array, static_cast<std::uint32_t>(s.first - 1));
}

// The first element, taken out; nil when there is none. Does not collect.
Oop ocRemoveFirst(Heap& heap, const WellKnown& wk, Oop oc) {
  OcSlots s;
  if (listShape(heap, wk, oc, &s) != OcShape::Sound || s.last < s.first) {
    return Oop::nil();
  }
  const auto idx = static_cast<std::uint32_t>(s.first - 1);
  const Oop value = heap.slotAt(s.array, idx);
  heap.slotAtPut(s.array, idx, Oop::nil());
  heap.slotAtPut(oc, kOcFirst, Oop::fromSmallInteger(s.first + 1));
  return value;
}

// Takes the last element out: undoes an add: when what came after it failed. Does not collect.
void ocRemoveLast(Heap& heap, const WellKnown& wk, Oop oc) {
  OcSlots s;
  if (listShape(heap, wk, oc, &s) != OcShape::Sound || s.last < s.first) {
    return;
  }
  heap.slotAtPut(s.array, static_cast<std::uint32_t>(s.last - 1), Oop::nil());
  heap.slotAtPut(oc, kOcLast, Oop::fromSmallInteger(s.last - 1));
}

// Takes the first element identical to target out, closing the gap. Does not collect.
bool ocRemoveIdentity(Heap& heap, const WellKnown& wk, Oop oc, Oop target) {
  OcSlots s;
  if (listShape(heap, wk, oc, &s) != OcShape::Sound) {
    return false;
  }
  // Sound: every index from first to last is inside the array.
  for (auto i = s.first; i <= s.last; ++i) {
    if (heap.slotAt(s.array, static_cast<std::uint32_t>(i - 1)) != target) {
      continue;
    }
    for (auto j = i; j < s.last; ++j) {
      heap.slotAtPut(s.array, static_cast<std::uint32_t>(j - 1),
                     heap.slotAt(s.array, static_cast<std::uint32_t>(j)));
    }
    heap.slotAtPut(s.array, static_cast<std::uint32_t>(s.last - 1), Oop::nil());
    heap.slotAtPut(oc, kOcLast, Oop::fromSmallInteger(s.last - 1));
    return true;
  }
  return false;
}

// The OrderedCollection in holder's slot, made when the slot holds something else. Empty when
// holder has no such slot, or when making one failed (out of memory: unwinding). SPEC §3.4: made
// without a send (neither OrderedCollection new nor basicNew), so no Smalltalk runs here: an
// instance of OrderedCollection itself in the empty form (array nil, firstIndex 1, lastIndex 0),
// whose first add: makes its Array.
Oop ensureOc(CallContext& ctx, Root& holder, std::uint32_t slot) {
  if (!hasSlots(ctx.heap, holder.slot, slot)) {
    return Oop{};
  }
  Oop oc = ctx.heap.slotAt(holder.slot, slot);
  if (oc.isHeap() && ctx.wk.classOf(oc) == ctx.wk.orderedCollectionClass) {
    return oc;
  }
  // May collect: holder is a Root, and nothing else here lives across it.
  const Oop neu = allocateRetry(ctx, ctx.wk.orderedCollectionClass, kOcLast + 1, 0);
  if (!neu.isHeap()) {
    return abortEvaluation(ctx, "out of memory");
  }
  ctx.heap.slotAtPut(neu, kOcArray, Oop::nil());
  ctx.heap.slotAtPut(neu, kOcFirst, Oop::fromSmallInteger(1));
  ctx.heap.slotAtPut(neu, kOcLast, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(holder.slot, slot, neu);
  return neu;
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
  // While Halted (SPEC §3.13): why, whether Proceed and Step may go on, whether the innermost
  // send in flight is a synthesized frame, and whether that frame is a DNU (both decided at the
  // stop).
  std::string haltReason;
  bool proceedable = false;
  bool haltSynthesizes = false;
  bool haltSynthesizesDnu = false;
  Record* resumeTo = nullptr;       // who it goes back to when it next switches away
  bool isBase() const { return ownCtx == nullptr; }
};;

namespace {

void rootRecord(Roots& roots, Oop* process, Oop* block, Oop* waitingOn, Oop* signaledBy) {
  // SPEC §3.2 (O1): all or nothing. Without the room it throws before any slot is registered;
  // a throw half way would leave slots of a record that is then freed.
  roots.reserveSlots(4);
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
  // SPEC §3.2: all or nothing. A constructor that throws runs no destructor, so every step that
  // can throw comes before the first registration: the room in records_, the room for the five
  // slots below (the record's four and evalValue_), and the attached stack (still empty).
  records_.reserve(1);
  base_.roots.reserveSlots(5);
  base_.roots.attachStack(&rec->rootStack);
  rootRecord(base_.roots, &rec->process, &rec->block, &rec->waitingOn, &rec->signaledBy);
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
    // SPEC §3.11: the ready queue starts empty; what the image had there cannot run. A damaged
    // OrderedCollection there is reset to the empty form without allocating, so the natives can
    // use it again. Whatever the object's class, the walk below then takes out what its slots
    // read as (ocRemoveFirst); a damaged object of another class keeps its slots. The next enqueue
    // replaces anything that is not an instance of OrderedCollection itself (SPEC §3.4).
    const Oop q = heap.slotAt(sched.slot, kSchedulerSlotQuiescent);
    OcSlots slots;
    if (ctx.wk.classOf(q) == ctx.wk.orderedCollectionClass &&
        listShape(heap, ctx.wk, q, &slots) == OcShape::Damaged) {
      heap.slotAtPut(q, kOcArray, Oop::nil());
      heap.slotAtPut(q, kOcFirst, Oop::fromSmallInteger(1));
      heap.slotAtPut(q, kOcLast, Oop::fromSmallInteger(0));
    }
    for (auto n = ocSize(heap, ctx.wk, q); n > 0; --n) {
      const Oop p = ocRemoveFirst(heap, ctx.wk, q);
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
  // SPEC §3.2 (O1): the half-made process goes when enqueue fails, and when it throws too (it has
  // then taken its ready entry out again).
  bool queued = false;
  try {
    queued = enqueue(ctx, *r);
  } catch (...) {
    r->state = State::Dead;
    reapDead();
    throw;
  }
  if (!queued) {
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
  // SPEC §3.2 (O1): the steps after the record is rooted do not throw (the room in records_ comes
  // first), or take the roots out again (attachStack), so a throw never leaves a freed record's
  // slots or stack registered.
  records_.reserve(records_.size() + 1);
  rootRecord(base_.roots, &r.process, &r.block, &r.waitingOn, &r.signaledBy);
  try {
    base_.roots.attachStack(&r.rootStack);
  } catch (...) {
    unrootRecord(base_.roots, &r.process, &r.block, &r.waitingOn, &r.signaledBy);
    throw;
  }
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
         !ctx.restarting && ctx.abortSetAside == 0 && ctx.haltSuppressed == 0 && !me.abandon &&
         !me.terminateRequested && haltedCount() < kMaxHalted;
}

bool Scheduler::halt(CallContext& ctx, std::string reason, bool proceedable) {
  Record& me = *current_;
  assert(canHalt(ctx) && "halt needs canHalt");
  // Any halt ends a step: the next Step sets its own mark (and checkInterrupt its skip).
  ctx.stepMode = StepMode::None;
  ctx.stepSkipOnce = false;
  me.haltReason = std::move(reason);
  me.proceedable = proceedable;
  me.haltSynthesizes = LiveFrames::synthesizes(ctx, &me.haltSynthesizesDnu);
  me.state = State::Halted;
  me.resumeTo = nullptr;
  lastHalted_ = me.id;
  // SPEC §3.13: back to the base, which waits for this process or sits in the ready queue.
  Record& b = base();
  leaveLists(b);
  switchTo(b);
  me.haltReason.clear();
  me.proceedable = false;
  me.haltSynthesizes = false;
  me.haltSynthesizesDnu = false;
  if (!afterResume(ctx, me)) {
    ctx.restartFrame = nullptr;
    ctx.restarting = false;
    disarmAccept(ctx);
    return false;
  }
  if (ctx.restartFrame != nullptr) {
    ctx.restarting = true;
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

bool Scheduler::haltSynthesizes(const CallContext& ctx, bool* dnu) const {
  *dnu = false;
  for (const auto& r : records_) {
    if (r->ctx == &ctx && r->state == State::Halted) {
      *dnu = r->haltSynthesizesDnu;
      return r->haltSynthesizes;
    }
  }
  return false;
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

Scheduler::EvalEnd Scheduler::proceed(std::uint64_t pid, const Frame* restartFrame,
                                      const Oop* acceptSlots, std::size_t acceptCount) {
  assert(current_ == &base() && "proceed runs on the base process");
  Record* r = findId(pid);
  assert(r != nullptr && r->state == State::Halted &&
         (r->proceedable || restartFrame != nullptr) && "proceed needs canProceed or a restart");
  assert((acceptCount == 0 || restartFrame != nullptr) && "an Accept is a restart");
  if (acceptCount > 0) {
    // SPEC §3.10: armAccept armed and pinned the slots before the install; writing the accepted
    // method into them allocates nothing.
    assert(r->ctx->acceptSlots.size() == acceptCount && "an Accept is armed first");
    std::copy_n(acceptSlots, std::min(acceptCount, r->ctx->acceptSlots.size()),
                r->ctx->acceptSlots.begin());
  }
  // Set only now, so a refused or failed entry before this never leaves a restart armed.
  r->ctx->restartFrame = restartFrame;
  // In no list: awaitEval switches to it, and its halt returns (true, or false to restart).
  r->state = State::Suspended;
  return awaitEval(pid);
}

void Scheduler::armAccept(std::uint64_t pid, const Oop* slots, std::size_t count) {
  Record* r = findId(pid);
  assert(r != nullptr && r->state == State::Halted && r->ctx->acceptSlots.empty() &&
         "an Accept arms a halted process once");
  // assign may throw before anything changes; no GC runs between it and the pin. pinRange can
  // throw too (its push_back); then the slots are cleared so none stays armed but unpinned.
  r->ctx->acceptSlots.assign(slots, slots + count);
  try {
    r->ctx->roots.pinRange(r->ctx->acceptSlots.data(), count);
  } catch (...) {
    r->ctx->acceptSlots.clear();
    throw;
  }
}

void Scheduler::unarmAccept(std::uint64_t pid) {
  // A process that has left the halt disarmed its slots itself (review-agy H7).
  Record* r = findId(pid);
  if (r != nullptr && r->state == State::Halted) {
    disarmAccept(*r->ctx);
  }
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
  // away (a cleanup that waits); the rest runs in the drains to come. No send: a damaged list does
  // not stop it (SPEC §3.4).
  terminateProcess(base_, r->process, false);
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
  // SPEC §3.4: a linkedList whose slots do not fit (instVarAt:put:) fails the wait, whatever
  // excessSignals holds, and changes nothing.
  OcSlots slots;
  if (listShape(heap, ctx.wk, heap.slotAt(sem.slot, kSemaphoreSlotList), &slots) ==
      OcShape::Damaged) {
    fail(ctx, sem.slot, "damaged ordered collection");
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
  // SPEC §3.4: a linkedList whose slots do not fit (instVarAt:put:) fails the signal, which
  // changes nothing. The loop below then stays inside its array.
  OcSlots slots;
  if (listShape(heap, ctx.wk, heap.slotAt(sem.slot, kSemaphoreSlotList), &slots) ==
      OcShape::Damaged) {
    fail(ctx, sem.slot, "damaged ordered collection");
    return false;
  }
  for (auto n = ocSize(heap, ctx.wk, heap.slotAt(sem.slot, kSemaphoreSlotList)); n > 0; --n) {
    const Oop list = heap.slotAt(sem.slot, kSemaphoreSlotList);
    const Oop waiter = ocFirst(heap, ctx.wk, list);
    Record* r = find(waiter);
    if (r != nullptr && r->state == State::Waiting && r->waitingOn == sem.slot) {
      // Ready at the end of the queue; the signaller goes on (SPEC §3.4). The queue takes it
      // first: when it cannot (out of memory), the waiter still waits and nothing changed.
      if (!enqueue(ctx, *r)) {
        return false;
      }
      ocRemoveIdentity(heap, ctx.wk, heap.slotAt(sem.slot, kSemaphoreSlotList), r->process);
      r->waitingOn = Oop::nil();
      r->signaledBy = sem.slot;
      return true;
    }
    // SPEC §3.4, §3.11: a waiter that cannot run is dropped.
    ocRemoveFirst(heap, ctx.wk, list);
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
    // SPEC §3.4: a damaged list it would leave fails the suspend; the process stays as it was.
    if (listsDamaged(*r)) {
      Root p(ctx.roots, process);
      fail(ctx, p.slot, "damaged ordered collection");
      return false;
    }
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
  return terminateProcess(ctx, process, true);
}

bool Scheduler::terminateProcess(CallContext& ctx, Oop process, bool fromSend) {
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
  // SPEC §3.4: from a send, a damaged list it would leave fails the terminate before anything
  // changes (the signal it would give back included); nothing unwinds.
  if (fromSend && listsDamaged(*r)) {
    Root p(ctx.roots, process);
    fail(ctx, p.slot, "damaged ordered collection");
    return false;
  }
  if (r->signaledBy.isHeap()) {
    // SPEC §3.4 terminate: signalled but not back from wait yet, it gives the signal back, as a
    // second signal would. When that fails, the process stays as it was. Without a send, a
    // damaged linkedList cannot take the signal back: it is dropped and the terminate goes on.
    Root sem(ctx.roots, r->signaledBy);
    OcSlots slots;
    const bool dropped =
        !fromSend && hasSlots(ctx.heap, sem.slot, kSemaphoreSlotList) &&
        listShape(ctx.heap, ctx.wk, ctx.heap.slotAt(sem.slot, kSemaphoreSlotList), &slots) ==
            OcShape::Damaged;
    if (!dropped && !signal(ctx, sem.slot)) {
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
    ocRemoveLast(heap, ctx.wk, contents.slot);
    throw;
  }
  if (!woke) {
    ocRemoveLast(heap, ctx.wk, contents.slot);
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
    // SPEC §3.4: contents whose slots do not fit (instVarAt:put:) fail next before it waits, so
    // readSynch keeps its signal. Damaged while it waited, they read as empty and come back here.
    OcSlots slots;
    if (listShape(heap, ctx.wk, heap.slotAt(q.slot, kSharedQueueSlotContents), &slots) ==
        OcShape::Damaged) {
      fail(ctx, q.slot, "damaged ordered collection");
      return Oop{};
    }
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
    if (ocSize(heap, ctx.wk, contents) > 0) {
      return ocRemoveFirst(heap, ctx.wk, contents);
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
      terminateProcess(base_, r->process, false);
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
  // review-agy H7: no accept slot stays armed or pinned past the fiber, however it ended.
  disarmAccept(ctx);
  ctx.reactivating = false;
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
  // SPEC §3.2, §3.4: a failure below (out of memory, a damaged list), or a C++ exception, takes
  // this call's entry out again, so the queue never keeps a record its caller then drops. No
  // Smalltalk runs below (ensureOc and ocAdd send nothing), so no other process is queued in
  // between; the undo still finds the entry by identity, from the end, and does nothing if it is
  // gone. Does not throw.
  const auto takeBack = [this, &r] {
    const auto it = std::find(ready_.rbegin(), ready_.rend(), &r);
    if (it != ready_.rend()) {
      ready_.erase(std::next(it).base());
    }
  };
  try {
    Root sched(ctx.roots, ctx.wk.processor);
    if (hasSlots(heap, sched.slot, kSchedulerSlotActive)) {
      Root list(ctx.roots, ensureOc(ctx, sched, kSchedulerSlotQuiescent));
      if (unwinding(ctx)) {
        takeBack();
        return false;
      }
      if (list.slot.isHeap()) {
        ocAdd(ctx, list.slot, r.process);
        if (unwinding(ctx)) {
          takeBack();
          return false;
        }
        setMyList(heap, r.process, list.slot);
      }
    }
  } catch (...) {
    takeBack();
    throw;
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
      ocRemoveIdentity(heap, base_.wk, heap.slotAt(sched, kSchedulerSlotQuiescent), r.process);
    }
  } else if (r.state == State::Waiting) {
    if (hasSlots(heap, r.waitingOn, kSemaphoreSlotList)) {
      ocRemoveIdentity(heap, base_.wk, heap.slotAt(r.waitingOn, kSemaphoreSlotList), r.process);
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

// The lists leaveLists reads, under the same conditions.
bool Scheduler::listsDamaged(const Record& r) const {
  const Heap& heap = base_.heap;
  const WellKnown& wk = base_.wk;
  OcSlots slots;
  if (r.state == State::Ready) {
    return hasSlots(heap, wk.processor, kSchedulerSlotActive) &&
           listShape(heap, wk, heap.slotAt(wk.processor, kSchedulerSlotQuiescent), &slots) ==
               OcShape::Damaged;
  }
  if (r.state == State::Waiting) {
    return hasSlots(heap, r.waitingOn, kSemaphoreSlotList) &&
           listShape(heap, wk, heap.slotAt(r.waitingOn, kSemaphoreSlotList), &slots) ==
               OcShape::Damaged;
  }
  return false;
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

#include "ao/DebugSnapshot.hpp"

#include "ao/Bytecode.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Scheduler.hpp"
#include "ao/Send.hpp"
#include "InterpFrame.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace ao {
namespace {

// The pc in the frame's context: the start of the bytecode the frame runs (mirror writes it
// before each one), so a caller's is the start of its Send. The frame's own when unreadable.
std::uint32_t contextPc(const CallContext& ctx, const Frame& f) {
  Heap& heap = ctx.heap;
  if (f.context.isHeap() && (heap.flags(f.context) & kFlagBytes) == 0 &&
      heap.size(f.context) > kCtxPc) {
    const Oop pc = heap.slotAt(f.context, kCtxPc);
    if (pc.isSmallInteger() && pc.smallIntegerValue() >= 0 &&
        pc.smallIntegerValue() <= std::numeric_limits<std::uint32_t>::max()) {
      return static_cast<std::uint32_t>(pc.smallIntegerValue());
    }
  }
  return f.pc;
}

// The bytecode at pc is a SendSuper.
bool sentToSuper(const CallContext& ctx, const Frame& f, std::uint32_t pc) {
  Heap& heap = ctx.heap;
  if (!f.method.isHeap() || (heap.flags(f.method) & kFlagBytes) != 0 ||
      heap.size(f.method) <= kCmSlotMethodClass) {
    return false;
  }
  const Oop bytes = heap.slotAt(f.method, kCmSlotBytes);
  if (!bytes.isHeap() || (heap.flags(bytes) & kFlagBytes) == 0 || pc >= heap.size(bytes)) {
    return false;
  }
  return static_cast<std::uint8_t>(heap.bytes(bytes)[pc]) ==
         static_cast<std::uint8_t>(compiler::Op::SendSuper);
}

// The class the frame's send in flight looks up from: the receiver's, or for a SendSuper the
// superclass of the sending method's class.
Oop sendStart(const CallContext& ctx, const Frame& f, std::uint32_t pc) {
  if (sentToSuper(ctx, f, pc)) {
    return superclassOf(ctx.heap, ctx.heap.slotAt(f.method, kCmSlotMethodClass));
  }
  return ctx.wk.classOf(*f.sendReceiver);
}

// What the frame's send in flight finds (SPEC §3.3 探索, which allocates nothing): a method, or
// nil when it found none (doesNotUnderstand:).
Oop sendTarget(const CallContext& ctx, const Frame& f, std::uint32_t pc) {
  return lookup(ctx.heap, sendStart(ctx, f, pc), *f.sendSelector);
}

// SPEC §3.13: the native a frame synthesized at the stop halted in. The chain the send looks up,
// walked like lookup: past nil and past a CompiledMethod (a class below the native's that defined
// the selector while halted; a Kernel native is neither overridden in its own class nor removed);
// the first other value is the answer when it is a NativeMethod. nil otherwise, the DNU's form, so
// a DNU stays one, also when its lookup found a value that is no method (instVarAt:put:).
// lookup's chain step (SuperclassWalk); allocates nothing.
Oop haltedNative(const CallContext& ctx, const Frame& f, std::uint32_t pc) {
  SuperclassWalk walk(ctx.heap, sendStart(ctx, f, pc));
  for (Oop cls; walk.next(cls);) {
    const Oop dict = ctx.heap.slotAt(cls, kClassSlotMethodDict);
    const Oop found = dict.isHeap() ? MethodDictionary::at(ctx.heap, dict, *f.sendSelector)
                                    : Oop::nil();
    if (found.isNil() ||
        (found.isHeap() && ctx.heap.klass(found) == ctx.wk.compiledMethodClass)) {
      continue;
    }
    return found.isHeap() && ctx.heap.klass(found) == ctx.wk.nativeMethodClass ? found
                                                                               : Oop::nil();
  }
  return Oop::nil();
}

bool hasSendInFlight(const Frame& f) {
  return f.sendReceiver != nullptr && f.sendSelector != nullptr;
}

// SPEC §3.13: a send in flight in the innermost frame that found a native (or nothing, a DNU) is
// where the failure is: true, and *method is that native (nil for a DNU). One that found a
// CompiledMethod is a stack guard abort before the callee's frame: nothing to synthesize.
bool synthesizesNative(const CallContext& ctx, const Frame* top, Oop* method) {
  *method = Oop::nil();
  if (top == nullptr || !hasSendInFlight(*top)) {
    return false;
  }
  const Oop found = sendTarget(ctx, *top, contextPc(ctx, *top));
  if (!found.isHeap()) {
    return true;
  }
  if (ctx.heap.klass(found) == ctx.wk.nativeMethodClass) {
    *method = found;
    return true;
  }
  return false;
}

}  // namespace

bool DebugSnapshot::capture(CallContext& ctx) noexcept {
  clear();
  try {
    const Frame* const top = ctx.topFrame;
    std::uint64_t interpreted = 0;
    for (const Frame* f = top; f != nullptr; f = f->prev) {
      ++interpreted;
    }
    Oop synthMethod = Oop::nil();
    const bool synthesize = synthesizesNative(ctx, top, &synthMethod);
    const std::uint64_t all = interpreted + (synthesize ? 1 : 0);
    total_ = static_cast<std::uint32_t>(
        std::min<std::uint64_t>(all, std::numeric_limits<std::uint32_t>::max()));
    const auto n = static_cast<std::uint32_t>(std::min<std::uint64_t>(all, kMaxFrames));

    // The layout first, then one block for every slot (and the process, last).
    frames_.reserve(n);
    std::uint32_t used = 0;
    if (synthesize) {
      frames_.push_back(Rec{kDebugFrameNative, 0, used, top->sendArgc, 0});
      used += kFixedSlots + top->sendArgc;
    }
    for (const Frame* f = top; f != nullptr && frames_.size() < n; f = f->prev) {
      const std::uint32_t temps = f->temps != nullptr ? f->temps->n : 0;
      const std::uint32_t args = hasSendInFlight(*f) ? f->sendArgc : 0;
      frames_.push_back(Rec{f->isBlock ? kDebugFrameBlock : kDebugFrameMethod, contextPc(ctx, *f),
                            used, temps, args});
      used += kFixedSlots + temps + args;
    }
    slotCount_ = used + 1;
    slots_.reset(new Oop[slotCount_]);

    std::uint32_t at = 0;
    auto put = [&](Oop v) { slots_[at++] = v; };
    if (synthesize) {
      put(synthMethod);
      put(*top->sendReceiver);
      put(Oop::nil());
      put(*top->sendSelector);
      for (std::uint32_t j = 0; j < top->sendArgc; ++j) {
        put(top->sendArgs[j]);
      }
    }
    const Frame* f = top;
    for (std::size_t i = synthesize ? 1 : 0; i < frames_.size(); ++i, f = f->prev) {
      const Rec& r = frames_[i];
      put(f->method);
      put(f->receiver);
      put(f->context);
      put(hasSendInFlight(*f) ? *f->sendSelector : Oop::nil());
      for (std::uint32_t j = 0; j < r.tempCount; ++j) {
        put(f->temps->slots[j]);
      }
      for (std::uint32_t j = 0; j < r.argCount; ++j) {
        put(f->sendArgs[j]);
      }
    }
    put(ctx.scheduler != nullptr ? ctx.scheduler->activeProcess() : Oop::nil());

    reason_ = abortReasonText(ctx);
    fromBase_ = ctx.scheduler == nullptr || !ctx.scheduler->nonBaseRunning();

    // Nothing above allocated on the Smalltalk heap, so no GC ran between the reads and here.
    // One pinned range, not LIFO slots: the frames still to unwind remove theirs without
    // scanning past the snapshot.
    roots_.pinRange(slots_.get(), slotCount_);
    registered_ = slotCount_;
    held_ = true;
    return true;
  } catch (...) {
    // SPEC §3.13: out of C++ memory, the capture is dropped; the evaluation fails as it would.
    clear();
    return false;
  }
}

void DebugSnapshot::clear() noexcept {
  if (registered_ > 0) {
    roots_.unpinRange(slots_.get(), registered_);
    registered_ = 0;
  }
  slots_.reset();
  slotCount_ = 0;
  frames_.clear();
  total_ = 0;
  reason_.clear();
  fromBase_ = false;
  held_ = false;
}

std::vector<const Oop*> DebugSnapshot::rootSlots() const {
  std::vector<const Oop*> out;
  out.reserve(registered_);
  for (std::uint32_t k = 0; k < registered_; ++k) {
    out.push_back(&slots_[k]);
  }
  return out;
}

Oop DebugSnapshot::slotAt(std::uint32_t i, std::uint32_t k) const {
  if (i >= frames_.size()) {
    return Oop{};
  }
  return slots_[frames_[i].base + k];
}

int DebugSnapshot::kind(std::uint32_t i) const {
  return i < frames_.size() ? frames_[i].kind : -1;
}

Oop DebugSnapshot::method(std::uint32_t i) const { return slotAt(i, 0); }

Oop DebugSnapshot::receiver(std::uint32_t i) const { return slotAt(i, 1); }

Oop DebugSnapshot::context(std::uint32_t i) const { return slotAt(i, 2); }

Oop DebugSnapshot::selector(std::uint32_t i) const { return slotAt(i, 3); }

std::uint32_t DebugSnapshot::pc(std::uint32_t i) const {
  return i < frames_.size() ? frames_[i].pc : 0;
}

std::uint32_t DebugSnapshot::tempCount(std::uint32_t i) const {
  return i < frames_.size() ? frames_[i].tempCount : 0;
}

Oop DebugSnapshot::temp(std::uint32_t i, std::uint32_t j) const {
  if (i >= frames_.size() || j >= frames_[i].tempCount) {
    return Oop{};
  }
  return slotAt(i, kFixedSlots + j);
}

std::uint32_t DebugSnapshot::sendArgCount(std::uint32_t i) const {
  return i < frames_.size() ? frames_[i].argCount : 0;
}

Oop DebugSnapshot::sendArg(std::uint32_t i, std::uint32_t j) const {
  if (i >= frames_.size() || j >= frames_[i].argCount) {
    return Oop{};
  }
  return slotAt(i, kFixedSlots + frames_[i].tempCount + j);
}

Oop DebugSnapshot::process() const { return held_ ? slots_[slotCount_ - 1] : Oop::nil(); }

LiveFrames::LiveFrames(const CallContext& ctx, const std::string& reason)
    : ctx_(ctx), reason_(reason) {
  const Frame* const top = ctx.topFrame;
  // SPEC §3.13: decided at the stop (Scheduler::halt), not by looking the selector up again.
  synth_ = top != nullptr && hasSendInFlight(*top) && ctx.scheduler != nullptr &&
           ctx.scheduler->haltSynthesizes(ctx);
  std::uint64_t all = synth_ ? 1 : 0;
  for (const Frame* f = top; f != nullptr; f = f->prev) {
    ++all;
    if (frames_.size() + (synth_ ? 1 : 0) < DebugSnapshot::kMaxFrames) {
      frames_.push_back(f);
    }
  }
  count_ = frames_.size() + (synth_ ? 1 : 0);
  total_ = static_cast<std::uint32_t>(
      std::min<std::uint64_t>(all, std::numeric_limits<std::uint32_t>::max()));
}

bool LiveFrames::synthesizes(const CallContext& ctx) {
  Oop ignored;
  return synthesizesNative(ctx, ctx.topFrame, &ignored);
}

const Frame* LiveFrames::frameAt(std::uint32_t i) const {
  if (i >= count_ || synthesized(i)) {
    return nullptr;
  }
  return frames_[synth_ ? i - 1 : i];
}

int LiveFrames::kind(std::uint32_t i) const {
  if (i >= count_) {
    return -1;
  }
  if (synthesized(i)) {
    return kDebugFrameNative;
  }
  return frameAt(i)->isBlock ? kDebugFrameBlock : kDebugFrameMethod;
}

Oop LiveFrames::method(std::uint32_t i) const {
  if (synthesized(i)) {
    // The native it halted in, even if a class below defines the selector now; nil for a DNU.
    return haltedNative(ctx_, *ctx_.topFrame, contextPc(ctx_, *ctx_.topFrame));
  }
  const Frame* f = frameAt(i);
  return f != nullptr ? f->method : Oop{};
}

Oop LiveFrames::receiver(std::uint32_t i) const {
  if (synthesized(i)) {
    return *ctx_.topFrame->sendReceiver;
  }
  const Frame* f = frameAt(i);
  return f != nullptr ? f->receiver : Oop{};
}

Oop LiveFrames::context(std::uint32_t i) const {
  if (synthesized(i)) {
    return Oop::nil();
  }
  const Frame* f = frameAt(i);
  return f != nullptr ? f->context : Oop{};
}

std::uint32_t LiveFrames::pc(std::uint32_t i) const {
  const Frame* f = frameAt(i);
  return f != nullptr ? contextPc(ctx_, *f) : 0;
}

Oop LiveFrames::selector(std::uint32_t i) const {
  if (synthesized(i)) {
    return *ctx_.topFrame->sendSelector;
  }
  const Frame* f = frameAt(i);
  if (f == nullptr) {
    return Oop{};
  }
  return hasSendInFlight(*f) ? *f->sendSelector : Oop::nil();
}

std::uint32_t LiveFrames::tempCount(std::uint32_t i) const {
  if (synthesized(i)) {
    return ctx_.topFrame->sendArgc;
  }
  const Frame* f = frameAt(i);
  return f != nullptr && f->temps != nullptr ? f->temps->n : 0;
}

Oop LiveFrames::temp(std::uint32_t i, std::uint32_t j) const {
  if (j >= tempCount(i)) {
    return Oop{};
  }
  if (synthesized(i)) {
    return ctx_.topFrame->sendArgs[j];
  }
  return frameAt(i)->temps->slots[j];
}

std::uint32_t LiveFrames::sendArgCount(std::uint32_t i) const {
  const Frame* f = frameAt(i);
  return f != nullptr && hasSendInFlight(*f) ? f->sendArgc : 0;
}

Oop LiveFrames::sendArg(std::uint32_t i, std::uint32_t j) const {
  if (j >= sendArgCount(i)) {
    return Oop{};
  }
  return frameAt(i)->sendArgs[j];
}

}  // namespace ao

#include "ao/DebugSnapshot.hpp"

#include "ao/Bytecode.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/Lookup.hpp"
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

// What the frame's send in flight finds (SPEC §3.3 探索, which allocates nothing): a method, or
// nil when it found none (doesNotUnderstand:).
Oop sendTarget(CallContext& ctx, const Frame& f, std::uint32_t pc) {
  Oop start = ctx.wk.classOf(*f.sendReceiver);
  if (sentToSuper(ctx, f, pc)) {
    start = superclassOf(ctx.heap, ctx.heap.slotAt(f.method, kCmSlotMethodClass));
  }
  return lookup(ctx.heap, start, *f.sendSelector);
}

bool hasSendInFlight(const Frame& f) {
  return f.sendReceiver != nullptr && f.sendSelector != nullptr;
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
    // SPEC §3.13: a send in flight in the innermost frame that found a native (or nothing, a
    // DNU) is where the failure is. One that found a CompiledMethod is a stack guard abort
    // before the callee's frame: nothing to synthesize.
    bool synthesize = false;
    Oop synthMethod = Oop::nil();
    if (top != nullptr && hasSendInFlight(*top)) {
      const Oop found = sendTarget(ctx, *top, contextPc(ctx, *top));
      if (!found.isHeap()) {
        synthesize = true;
      } else if (ctx.heap.klass(found) == ctx.wk.nativeMethodClass) {
        synthesize = true;
        synthMethod = found;
      }
    }
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

}  // namespace ao

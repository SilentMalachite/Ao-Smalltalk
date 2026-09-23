#include "ao/Interpreter.hpp"

#include "ao/Bytecode.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

#include <pthread.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>

namespace ao {
namespace {

struct OperandStack {
  Roots* roots = nullptr;
  std::deque<Oop> slots;
  explicit OperandStack(Roots& r) : roots(&r) {}
  OperandStack(const OperandStack&) = delete;
  OperandStack& operator=(const OperandStack&) = delete;
  ~OperandStack() {
    while (!slots.empty()) {
      roots->remove(&slots.back());
      slots.pop_back();
    }
  }
  void push(Oop v) {
    slots.push_back(v);
    roots->add(&slots.back());
  }
  bool pop(Oop* out) {
    if (slots.empty()) {
      return false;
    }
    *out = slots.back();
    roots->remove(&slots.back());
    slots.pop_back();
    return true;
  }
  bool top(Oop* out) const {
    if (slots.empty()) {
      return false;
    }
    *out = slots.back();
    return true;
  }
  // The value fromTop slots below the top (0 is the top), left on the stack.
  bool peek(std::uint32_t fromTop, Oop* out) const {
    if (fromTop >= slots.size()) {
      return false;
    }
    *out = slots[slots.size() - 1 - fromTop];
    return true;
  }

  // Overwrites the top in place. Its slot stays rooted, so Roots is not touched.
  bool replaceTop(Oop v) {
    if (slots.empty()) {
      return false;
    }
    slots.back() = v;
    return true;
  }
  std::uint32_t depth() const { return static_cast<std::uint32_t>(slots.size()); }
};

struct Temps {
  Roots* roots = nullptr;
  std::unique_ptr<Oop[]> slots;
  std::uint32_t n = 0;
  Temps(Roots& r, std::uint32_t count) : roots(&r), n(count) {
    if (n == 0) {
      return;
    }
    slots.reset(new Oop[n]);
    for (std::uint32_t i = 0; i < n; ++i) {
      slots[i] = Oop::nil();
      roots->add(&slots[i]);
    }
  }
  ~Temps() {
    if (!slots) {
      return;
    }
    for (std::uint32_t i = 0; i < n; ++i) {
      roots->remove(&slots[i]);
    }
  }
  Temps(const Temps&) = delete;
  Temps& operator=(const Temps&) = delete;
  bool at(std::uint32_t i, Oop* out) const {
    if (i >= n) {
      return false;
    }
    *out = slots[i];
    return true;
  }
  bool put(std::uint32_t i, Oop v) {
    if (i >= n) {
      return false;
    }
    slots[i] = v;
    return true;
  }
};

struct Frame {
  Oop method{};
  Oop receiver{};
  Oop context{};
  std::uint32_t pc = 0;
  bool isBlock = false;
};

struct FieldRoots {
  Roots& roots;
  Frame* frame;
  FieldRoots(Roots& r, Frame* f) : roots(r), frame(f) {
    roots.add(&frame->method);
    roots.add(&frame->receiver);
    roots.add(&frame->context);
  }
  ~FieldRoots() {
    roots.remove(&frame->context);
    roots.remove(&frame->receiver);
    roots.remove(&frame->method);
  }
  FieldRoots(const FieldRoots&) = delete;
  FieldRoots& operator=(const FieldRoots&) = delete;
};

// Marks the frame's context dead (nil pc and sender) on every way out of the frame (SPEC §3.4).
// Declare it after FieldRoots, so the context is still rooted when it runs.
struct ContextExitGuard {
  CallContext& ctx;
  Frame& frame;
  ContextExitGuard(CallContext& c, Frame& f) : ctx(c), frame(f) {}
  ~ContextExitGuard() {
    if (frame.context.isHeap()) {
      ctx.heap.slotAtPut(frame.context, kCtxPc, Oop::nil());
      ctx.heap.slotAtPut(frame.context, kCtxSender, Oop::nil());
    }
  }
  ContextExitGuard(const ContextExitGuard&) = delete;
  ContextExitGuard& operator=(const ContextExitGuard&) = delete;
};

// A context whose frame has not ended yet: it still has an integer pc.
bool contextAlive(CallContext& ctx, Oop context) {
  if (!context.isHeap() || (ctx.heap.flags(context) & kFlagBytes) != 0 ||
      ctx.heap.size(context) <= kCtxPc) {
    return false;
  }
  return ctx.heap.slotAt(context, kCtxPc).isSmallInteger();
}

struct ActiveGuard {
  CallContext& ctx;
  Oop saved;
  bool rootShared;
  // saved is this guard's own slot. activeContext is one CallContext field:
  // only the outermost run may register it. A second registration is forwarded
  // twice by collectOld.
  ActiveGuard(CallContext& c, Oop next, bool rootShared)
      : ctx(c), saved(c.activeContext), rootShared(rootShared) {
    ctx.roots.add(&saved);
    ctx.activeContext = next;
    if (rootShared) {
      ctx.roots.add(&ctx.activeContext);
    }
  }
  ~ActiveGuard() {
    if (rootShared) {
      ctx.roots.remove(&ctx.activeContext);
    }
    ctx.activeContext = saved;
    ctx.roots.remove(&saved);
  }
  ActiveGuard(const ActiveGuard&) = delete;
  ActiveGuard& operator=(const ActiveGuard&) = delete;
};

struct NonlocalGuard {
  CallContext& ctx;
  bool rootShared;
  explicit NonlocalGuard(CallContext& c, bool rootShared) : ctx(c), rootShared(rootShared) {
    if (!rootShared) {
      return;
    }
    ctx.roots.add(&ctx.nonlocalHome);
    ctx.roots.add(&ctx.nonlocalValue);
  }
  ~NonlocalGuard() {
    if (!rootShared) {
      return;
    }
    ctx.roots.remove(&ctx.nonlocalValue);
    ctx.roots.remove(&ctx.nonlocalHome);
  }
  NonlocalGuard(const NonlocalGuard&) = delete;
  NonlocalGuard& operator=(const NonlocalGuard&) = delete;
};

struct DepthGuard {
  int& depth;
  bool outermost;
  explicit DepthGuard(int& d) : depth(d), outermost(d == 0) { ++depth; }
  ~DepthGuard() { --depth; }
  DepthGuard(const DepthGuard&) = delete;
  DepthGuard& operator=(const DepthGuard&) = delete;
};

thread_local int gInterpreterDepth = 0;

struct Leave {
  bool leave = false;
  Oop value{};
};

Leave hit(Oop value) {
  Leave r;
  r.leave = true;
  r.value = value;
  return r;
}

Leave miss() {
  Leave r;
  r.leave = true;
  r.value = Oop{};
  return r;
}

void clearNonlocal(CallContext& ctx) {
  ctx.nonlocalReturn = false;
  ctx.nonlocalHome = Oop{};
  ctx.nonlocalValue = Oop{};
}

Leave consumeNonlocal(CallContext& ctx, bool isMethod, Oop methodContext, bool outermost) {
  // An abort has no home: no frame stops it (SPEC §3.4). The outermost entry clears it.
  if (ctx.aborting) {
    return miss();
  }
  if (!ctx.nonlocalReturn) {
    return {};
  }
  if (isMethod && ctx.nonlocalHome == methodContext) {
    const Oop value = ctx.nonlocalValue;
    clearNonlocal(ctx);
    return hit(value);
  }
  if (outermost) {
    clearNonlocal(ctx);
  }
  return miss();
}

bool decodeHeader(CallContext& ctx, Oop method, std::uint8_t* numArgs, std::uint8_t* numTemps) {
  const Oop hdr = ctx.heap.slotAt(method, kCmSlotHeader);
  if (!hdr.isSmallInteger()) {
    return false;
  }
  const auto h = hdr.smallIntegerValue();
  if (h < 0) {
    return false;
  }
  *numArgs = static_cast<std::uint8_t>(h & 0xFF);
  *numTemps = static_cast<std::uint8_t>((h >> 8) & 0xFF);
  return true;
}

std::uint32_t byteCount(CallContext& ctx, Oop method) {
  const Oop bytes = ctx.heap.slotAt(method, kCmSlotBytes);
  if (!bytes.isHeap() || (ctx.heap.flags(bytes) & kFlagBytes) == 0) {
    return 0;
  }
  return ctx.heap.size(bytes);
}

bool readByte(CallContext& ctx, Oop method, std::uint32_t index, std::uint8_t* out) {
  const Oop bytes = ctx.heap.slotAt(method, kCmSlotBytes);
  if (!bytes.isHeap() || (ctx.heap.flags(bytes) & kFlagBytes) == 0) {
    return false;
  }
  if (index >= ctx.heap.size(bytes)) {
    return false;
  }
  *out = static_cast<std::uint8_t>(ctx.heap.bytes(bytes)[index]);
  return true;
}

bool literalAt(CallContext& ctx, Oop method, std::uint8_t index, Oop* out) {
  const Oop lits = ctx.heap.slotAt(method, kCmSlotLiterals);
  if (!lits.isHeap() || (ctx.heap.flags(lits) & kFlagBytes) != 0) {
    return false;
  }
  if (index >= ctx.heap.size(lits)) {
    return false;
  }
  *out = ctx.heap.slotAt(lits, index);
  return true;
}

void mirror(CallContext& ctx, const Frame& frame, std::uint32_t depth) {
  ctx.heap.slotAtPut(frame.context, kCtxPc, Oop::fromSmallInteger(frame.pc));
  ctx.heap.slotAtPut(frame.context, kCtxStackp,
                     Oop::fromSmallInteger(static_cast<std::int64_t>(depth)));
}

bool instSlot(CallContext& ctx, Oop receiver, std::uint8_t index) {
  if (!receiver.isHeap() || (ctx.heap.flags(receiver) & kFlagBytes) != 0) {
    return false;
  }
  return index < ctx.heap.size(receiver);
}

bool jumpTo(CallContext& ctx, Gc& gc, Frame& frame, std::int16_t rel) {
  // Base is the byte after this 3-byte instruction (opcode + 3), matching patchJump.
  const auto target = static_cast<std::int64_t>(frame.pc) + rel;
  const auto n = static_cast<std::int64_t>(byteCount(ctx, frame.method));
  if (target < 0 || target > n) {
    return false;
  }
  frame.pc = static_cast<std::uint32_t>(target);
  if (rel < 0) {
    gc.safepoint();
  }
  return true;
}

std::int16_t rel16(std::uint8_t lo, std::uint8_t hi) {
  const auto bits = static_cast<std::uint16_t>(lo | (static_cast<unsigned>(hi) << 8));
  return static_cast<std::int16_t>(bits);
}

Oop boolean(bool v) { return v ? Oop::true_() : Oop::false_(); }

// The answer of special selector k for SmallInteger values a and b, when k is one of
// + - * < > <= >= = and the answer is a Boolean or a SmallInteger.
bool smallIntegerAnswer(std::uint8_t k, std::int64_t a, std::int64_t b, Oop* answer) {
  std::int64_t n = 0;
  bool overflow = false;
  switch (k) {
    case compiler::kSpecialAdd:
      overflow = __builtin_add_overflow(a, b, &n);
      break;
    case compiler::kSpecialSubtract:
      overflow = __builtin_sub_overflow(a, b, &n);
      break;
    case compiler::kSpecialMultiply:
      overflow = __builtin_mul_overflow(a, b, &n);
      break;
    case compiler::kSpecialLess:
      *answer = boolean(a < b);
      return true;
    case compiler::kSpecialGreater:
      *answer = boolean(a > b);
      return true;
    case compiler::kSpecialLessEqual:
      *answer = boolean(a <= b);
      return true;
    case compiler::kSpecialGreaterEqual:
      *answer = boolean(a >= b);
      return true;
    case compiler::kSpecialEqual:
      *answer = boolean(a == b);
      return true;
    default:
      return false;
  }
  if (overflow || n < kSmiMin || n > kSmiMax) {
    return false;
  }
  *answer = Oop::fromSmallInteger(n);
  return true;
}

// SPEC §3.5: SendSpecial k with one argument, a SmallInteger receiver and a SmallInteger argument
// answers without a send when smallIntegerAnswer has the answer and the session has checked that
// SmallInteger finds natives for these selectors (they answer the same). An old image that hides
// one turns this off. Replaces the two operands by the answer; otherwise
// leaves the stack as it was, and the caller sends. Allocates nothing, so no GC runs.
bool answerWithoutSend(const WellKnown& wk, OperandStack& stack, std::uint8_t k,
                       std::uint8_t argc) {
  Oop rcvr;
  Oop arg;
  Oop answer;
  if (!wk.smallIntegerFastPath() || argc != 1 || !stack.peek(1, &rcvr) || !stack.peek(0, &arg) ||
      !rcvr.isSmallInteger() || !arg.isSmallInteger() ||
      !smallIntegerAnswer(k, rcvr.smallIntegerValue(), arg.smallIntegerValue(), &answer)) {
    return false;
  }
  stack.pop(&arg);
  return stack.replaceTop(answer);
}

Leave performSend(CallContext& ctx, Frame& frame, OperandStack& stack, std::uint8_t argc, Oop selector,
                  bool isSuper, bool outermost) {
  ++ctx.interpretedSends;
  RootedArray argv(ctx.roots, argc);
  for (std::uint32_t k = 0; k < argc; ++k) {
    const std::uint32_t i = argc - 1 - k;
    if (!stack.pop(&argv.ptr()[i])) {
      return miss();
    }
  }
  Root rcvr(ctx.roots);
  if (!stack.pop(&rcvr.slot)) {
    return miss();
  }
  Root sel(ctx.roots, selector);
  const Oop* argp = argc == 0 ? nullptr : argv.ptr();
  Oop result;
  if (isSuper) {
    const Oop methodClass = ctx.heap.slotAt(frame.method, kCmSlotMethodClass);
    result = sendSuper(ctx, rcvr.slot, sel.slot, argp, argc, methodClass);
  } else {
    result = send(ctx, rcvr.slot, sel.slot, argp, argc, nullptr);
  }
  const Leave nl = consumeNonlocal(ctx, !frame.isBlock, frame.context, outermost);
  if (nl.leave) {
    return nl;
  }
  stack.push(result);
  return {};
}

// JumpTrue / JumpFalse (SPEC §3.5). A non-Boolean gets mustBeBoolean, and the answer must be
// a Boolean; otherwise the evaluation aborts. Leaves when the send unwound or aborted.
Leave branchTruth(CallContext& ctx, Frame& frame, Oop value, bool outermost, bool* truth) {
  if (truthOf(ctx, value, truth)) {
    return {};
  }
  // mustBeBoolean unwound: a non-local return may end at this frame; an abort never does.
  const Leave nl = consumeNonlocal(ctx, !frame.isBlock, frame.context, outermost);
  return nl.leave ? nl : miss();
}

// The temp vector held in temp t, when it is a pointer object with slot i.
bool remoteSlot(CallContext& ctx, const Temps& temps, std::uint8_t t, std::uint8_t i, Oop* vec) {
  if (!temps.at(t, vec) || !vec->isHeap() || (ctx.heap.flags(*vec) & kFlagBytes) != 0) {
    return false;
  }
  return i < ctx.heap.size(*vec);
}

// Literal `index` when it is an Association-shaped binding (a pointer object with a value slot).
bool litVar(CallContext& ctx, Oop method, std::uint8_t index, Oop* assoc) {
  if (!literalAt(ctx, method, index, assoc) || !assoc->isHeap() ||
      (ctx.heap.flags(*assoc) & kFlagBytes) != 0) {
    return false;
  }
  return kAssocValue < ctx.heap.size(*assoc);
}

}  // namespace

Oop Interpreter::run(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                     Oop block) {
  // No frame starts while frames unwind (SPEC §3.4), even under a native that missed it.
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (!method.isHeap() || ctx.heap.klass(method) != ctx.wk.compiledMethodClass) {
    return Oop{};
  }
  std::uint8_t numArgs = 0;
  std::uint8_t numTemps = 0;
  if (!decodeHeader(ctx, method, &numArgs, &numTemps)) {
    return Oop{};
  }
  if (static_cast<std::uint32_t>(numArgs) != argc || numTemps < numArgs) {
    return Oop{};
  }
  if (argc > 0 && args == nullptr) {
    return Oop{};
  }

  auto frame = std::make_unique<Frame>();
  frame->method = method;
  frame->receiver = receiver;
  FieldRoots fields(ctx.roots, frame.get());
  Root blockHold(ctx.roots, block);
  RootedArray argHold(ctx.roots, argc);
  for (std::uint32_t i = 0; i < argc; ++i) {
    argHold.ptr()[i] = args[i];
  }
  DepthGuard depth(gInterpreterDepth);
  if (depth.outermost) {
    // The stack range may belong to an earlier thread whose stack this one now reuses.
    refreshStackLimit(ctx);
  }
  NonlocalGuard nonlocal(ctx, depth.outermost);
  // Root inputs before the safepoint. applyMethod's caller may hold the method only in a register.
  Gc gc(ctx.heap, ctx.roots);
  gc.safepoint();

  frame->isBlock = blockHold.slot.isHeap() &&
                   ctx.heap.klass(blockHold.slot) == ctx.wk.blockContextClass;
  // activeContext defaults to empty, not nil. Top-level sender is nil.
  const Oop caller = ctx.activeContext.isHeap() ? ctx.activeContext : Oop::nil();
  if (frame->isBlock) {
    // A fresh activation per call; the closure itself is never written (SPEC §3.4).
    frame->context = Context::createBlock(
        ctx, frame->method, frame->receiver, ctx.heap.slotAt(blockHold.slot, kBlockHome),
        ctx.heap.slotAt(blockHold.slot, kBlockCopied), caller, static_cast<std::uint8_t>(argc));
  } else {
    frame->context = Context::createMethod(ctx, frame->method, frame->receiver, caller,
                                           static_cast<std::uint8_t>(argc));
  }
  if (!frame->context.isHeap()) {
    return Oop{};
  }
  ContextExitGuard exited(ctx, *frame);

  ActiveGuard active(ctx, frame->context, depth.outermost);
  Temps temps(ctx.roots, numTemps);
  for (std::uint32_t i = 0; i < argc; ++i) {
    temps.put(i, argHold.ptr()[i]);
  }
  if (frame->isBlock) {
    const Oop copied = ctx.heap.slotAt(frame->context, kBlockCopied);
    if (copied.isHeap()) {
      if ((ctx.heap.flags(copied) & kFlagBytes) != 0) {
        return Oop{};
      }
      const std::uint32_t cn = ctx.heap.size(copied);
      if (cn > numTemps) {
        return Oop{};
      }
      for (std::uint32_t i = 0; i < cn; ++i) {
        temps.put(numTemps - cn + i, ctx.heap.slotAt(copied, i));
      }
    } else if (!copied.isNil()) {
      return Oop{};
    }
  }

  OperandStack stack(ctx.roots);
  for (;;) {
    if (!frame->context.isHeap()) {
      return Oop{};
    }
    mirror(ctx, *frame, stack.depth());
    const std::uint32_t pc = frame->pc;
    const std::uint32_t nBytes = byteCount(ctx, frame->method);
    if (pc >= nBytes) {
      return Oop{};
    }
    std::uint8_t opb = 0;
    if (!readByte(ctx, frame->method, pc, &opb)) {
      return Oop{};
    }
    if (opb > static_cast<std::uint8_t>(compiler::kLastOp)) {
      return Oop{};
    }
    const auto op = static_cast<compiler::Op>(opb);
    const std::uint8_t width = compiler::operandBytes(op);
    if (static_cast<std::uint64_t>(pc) + 1u + width > nBytes) {
      return Oop{};
    }
    std::uint8_t argb[2] = {0, 0};
    for (std::uint8_t i = 0; i < width; ++i) {
      if (!readByte(ctx, frame->method, pc + 1u + i, &argb[i])) {
        return Oop{};
      }
    }
    ++ctx.interpretedBytecodes;
    frame->pc = pc + 1u + width;

    switch (op) {
      case compiler::Op::PushReceiver:
        stack.push(frame->receiver);
        break;
      case compiler::Op::PushTrue:
        stack.push(Oop::true_());
        break;
      case compiler::Op::PushFalse:
        stack.push(Oop::false_());
        break;
      case compiler::Op::PushNil:
        stack.push(Oop::nil());
        break;
      case compiler::Op::PushThisContext:
        stack.push(ctx.activeContext);
        break;
      case compiler::Op::PushMinusOne:
        stack.push(Oop::fromSmallInteger(-1));
        break;
      case compiler::Op::PushZero:
        stack.push(Oop::fromSmallInteger(0));
        break;
      case compiler::Op::PushOne:
        stack.push(Oop::fromSmallInteger(1));
        break;
      case compiler::Op::PushTwo:
        stack.push(Oop::fromSmallInteger(2));
        break;
      case compiler::Op::PushTemp: {
        Oop v;
        if (!temps.at(argb[0], &v)) {
          return Oop{};
        }
        stack.push(v);
        break;
      }
      case compiler::Op::PushInstVar: {
        if (!instSlot(ctx, frame->receiver, argb[0])) {
          return Oop{};
        }
        stack.push(ctx.heap.slotAt(frame->receiver, argb[0]));
        break;
      }
      case compiler::Op::PushLiteral: {
        Oop lit;
        if (!literalAt(ctx, frame->method, argb[0], &lit)) {
          return Oop{};
        }
        stack.push(lit);
        break;
      }
      case compiler::Op::PushGlobal: {
        Oop lit;
        if (!literalAt(ctx, frame->method, argb[0], &lit)) {
          return Oop{};
        }
        stack.push(ctx.wk.named(Symbol::bytes(ctx.heap, lit)));
        break;
      }
      case compiler::Op::Pop: {
        Oop v;
        if (!stack.pop(&v)) {
          return Oop{};
        }
        break;
      }
      case compiler::Op::Dup: {
        Oop v;
        if (!stack.top(&v)) {
          return Oop{};
        }
        stack.push(v);
        break;
      }
      case compiler::Op::StoreTemp: {
        Oop v;
        if (!stack.top(&v) || !temps.put(argb[0], v)) {
          return Oop{};
        }
        break;
      }
      case compiler::Op::PopStoreTemp: {
        Oop v;
        if (!stack.pop(&v) || !temps.put(argb[0], v)) {
          return Oop{};
        }
        break;
      }
      case compiler::Op::StoreInstVar:
      case compiler::Op::PopStoreInstVar: {
        if (!instSlot(ctx, frame->receiver, argb[0])) {
          return Oop{};
        }
        Oop v;
        if (op == compiler::Op::PopStoreInstVar) {
          if (!stack.pop(&v)) {
            return Oop{};
          }
        } else if (!stack.top(&v)) {
          return Oop{};
        }
        ctx.heap.slotAtPut(frame->receiver, argb[0], v);
        break;
      }
      case compiler::Op::Send:
      case compiler::Op::SendSuper:
      case compiler::Op::SendSpecial: {
        Oop selector;
        if (op == compiler::Op::SendSpecial) {
          if (answerWithoutSend(ctx.wk, stack, argb[0], argb[1])) {
            break;
          }
          selector = ctx.wk.specialSelector(argb[0]);
          if (!selector.isHeap()) {
            return Oop{};
          }
        } else if (!literalAt(ctx, frame->method, argb[0], &selector)) {
          return Oop{};
        }
        const Leave sent =
            performSend(ctx, *frame, stack, argb[1], selector, op == compiler::Op::SendSuper,
                        depth.outermost);
        if (sent.leave) {
          return sent.value;
        }
        break;
      }
      case compiler::Op::Jump:
      case compiler::Op::JumpTrue:
      case compiler::Op::JumpFalse: {
        bool take = true;
        if (op != compiler::Op::Jump) {
          Oop v;
          if (!stack.pop(&v)) {
            return Oop{};
          }
          bool truth = false;
          const Leave nl = branchTruth(ctx, *frame, v, depth.outermost, &truth);
          if (nl.leave) {
            return nl.value;
          }
          take = (op == compiler::Op::JumpTrue) == truth;
        }
        if (take && !jumpTo(ctx, gc, *frame, rel16(argb[0], argb[1]))) {
          return Oop{};
        }
        break;
      }
      case compiler::Op::ReturnReceiver:
        return frame->receiver;
      case compiler::Op::ReturnTrue:
        return Oop::true_();
      case compiler::Op::ReturnFalse:
        return Oop::false_();
      case compiler::Op::ReturnNil:
        return Oop::nil();
      case compiler::Op::ReturnTop: {
        Oop v;
        if (!stack.pop(&v)) {
          return Oop{};
        }
        return v;
      }
      case compiler::Op::ReturnBlock: {
        Oop v;
        if (!stack.pop(&v)) {
          return Oop{};
        }
        if (!frame->isBlock) {
          return v;
        }
        const Oop home = ctx.heap.slotAt(frame->context, kBlockHome);
        if (!contextAlive(ctx, home)) {
          // SPEC §3.4: the home has returned. cannotReturn:'s answer is this block's value, and
          // no unrelated caller is unwound.
          Root value(ctx.roots, v);
          Root sel(ctx.roots, ctx.wk.intern("cannotReturn:"));
          if (!sel.slot.isHeap()) {
            return Oop{};
          }
          const Oop answer = send(ctx, frame->context, sel.slot, &value.slot, 1, nullptr);
          const Leave nl = consumeNonlocal(ctx, false, frame->context, depth.outermost);
          return nl.leave ? nl.value : answer;
        }
        ctx.nonlocalReturn = true;
        ctx.nonlocalHome = home;
        ctx.nonlocalValue = v;
        return consumeNonlocal(ctx, false, frame->context, depth.outermost).value;
      }
      case compiler::Op::CreateBlock: {
        Oop lit;
        if (!literalAt(ctx, frame->method, argb[0], &lit)) {
          return Oop{};
        }
        Root blockMethod(ctx.roots, lit);
        if (!blockMethod.slot.isHeap() ||
            ctx.heap.klass(blockMethod.slot) != ctx.wk.compiledMethodClass) {
          return Oop{};
        }
        std::uint8_t blockArgs = 0;
        std::uint8_t blockTemps = 0;
        if (!decodeHeader(ctx, blockMethod.slot, &blockArgs, &blockTemps) || blockTemps < blockArgs) {
          return Oop{};
        }
        const std::uint32_t ncopy = argb[1];
        RootedArray copies(ctx.roots, ncopy);
        for (std::uint32_t k = 0; k < ncopy; ++k) {
          const std::uint32_t i = ncopy - 1 - k;
          if (!stack.pop(&copies.ptr()[i])) {
            return Oop{};
          }
        }
        Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, ncopy, 0));
        if (!arr.slot.isHeap()) {
          return Oop{};
        }
        for (std::uint32_t i = 0; i < ncopy; ++i) {
          ctx.heap.slotAtPut(arr.slot, i, copies.ptr()[i]);
        }
        const Oop home = frame->isBlock ? ctx.heap.slotAt(frame->context, kBlockHome) : frame->context;
        // A closure has no sender; each call's activation gets one (SPEC §3.4).
        Root created(ctx.roots,
                     Context::createBlock(ctx, blockMethod.slot, frame->receiver, home, arr.slot,
                                          Oop::nil(), blockArgs));
        if (!created.slot.isHeap()) {
          return Oop{};
        }
        stack.push(created.slot);
        break;
      }
      case compiler::Op::Primitive:
        break;
      case compiler::Op::PushNewArray: {
        const Oop arr = allocateRetry(ctx, ctx.wk.arrayClass, argb[0], 0);
        if (!arr.isHeap()) {
          return Oop{};
        }
        stack.push(arr);
        break;
      }
      case compiler::Op::PushRemoteTemp: {
        Oop vec;
        if (!remoteSlot(ctx, temps, argb[1], argb[0], &vec)) {
          return Oop{};
        }
        stack.push(ctx.heap.slotAt(vec, argb[0]));
        break;
      }
      case compiler::Op::StoreRemoteTemp:
      case compiler::Op::PopStoreRemoteTemp: {
        Oop vec;
        Oop v;
        if (!remoteSlot(ctx, temps, argb[1], argb[0], &vec)) {
          return Oop{};
        }
        const bool popped =
            op == compiler::Op::PopStoreRemoteTemp ? stack.pop(&v) : stack.top(&v);
        if (!popped) {
          return Oop{};
        }
        ctx.heap.slotAtPut(vec, argb[0], v);
        break;
      }
      case compiler::Op::PushLitVar: {
        Oop assoc;
        if (!litVar(ctx, frame->method, argb[0], &assoc)) {
          return Oop{};
        }
        stack.push(ctx.heap.slotAt(assoc, kAssocValue));
        break;
      }
      case compiler::Op::StoreLitVar:
      case compiler::Op::PopStoreLitVar: {
        Oop assoc;
        Oop v;
        if (!litVar(ctx, frame->method, argb[0], &assoc)) {
          return Oop{};
        }
        const bool popped = op == compiler::Op::PopStoreLitVar ? stack.pop(&v) : stack.top(&v);
        if (!popped) {
          return Oop{};
        }
        ctx.heap.slotAtPut(assoc, kAssocValue, v);
        break;
      }
    }
  }
}

Oop applyMethod(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                Oop block) {
  if (!method.isHeap()) {
    return Oop{};
  }
  // SPEC §3.4: every send, native or compiled, passes here, so unbounded recursion stops here
  // with a reserve left for unwinding. A frame outside the known range is another thread.
  const auto sp = reinterpret_cast<std::uintptr_t>(__builtin_frame_address(0));
  const auto limit = [&ctx] {
    return ctx.cleanupDepth > 0 ? ctx.stackCleanupLimit : ctx.stackLimit;
  };
  if (sp < limit() || sp > ctx.stackHigh) {
    refreshStackLimit(ctx);
    if (sp < limit()) {
      return abortEvaluation(ctx, "stack overflow");
    }
  }
  const Oop k = ctx.heap.klass(method);
  if (k == ctx.wk.nativeMethodClass) {
    return NativeMethod::apply(ctx, method, receiver, args, argc);
  }
  if (k == ctx.wk.compiledMethodClass) {
    return Interpreter::run(ctx, method, receiver, args, argc, block);
  }
  return Oop{};
}

void refreshStackLimit(CallContext& ctx) {
  pthread_t self = pthread_self();
  const auto high = reinterpret_cast<std::uintptr_t>(pthread_get_stackaddr_np(self));
  const std::size_t size = pthread_get_stacksize_np(self);
  const std::size_t reserve = std::min<std::size_t>(std::size_t{512} * 1024, size / 4);
  ctx.stackHigh = high;
  ctx.stackLimit = high - size + reserve;
  // ensure: cleanups may use half of the reserve; the rest is left for unwinding.
  ctx.stackCleanupLimit = high - size + reserve / 2;
}

}  // namespace ao

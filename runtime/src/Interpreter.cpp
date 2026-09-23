#include "ao/Interpreter.hpp"

#include "ao/Bytecode.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

#include <cstdint>
#include <deque>
#include <memory>

namespace ao {
namespace {

struct Root {
  Roots& roots;
  Oop slot;
  explicit Root(Roots& r, Oop v = Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

struct RootedArray {
  Roots& roots;
  std::unique_ptr<Oop[]> data;
  std::uint32_t n = 0;
  RootedArray(Roots& r, std::uint32_t count) : roots(r), n(count) {
    if (n == 0) {
      return;
    }
    data.reset(new Oop[n]);
    for (std::uint32_t i = 0; i < n; ++i) {
      data[i] = Oop::nil();
      roots.add(&data[i]);
    }
  }
  ~RootedArray() {
    if (!data) {
      return;
    }
    for (std::uint32_t i = 0; i < n; ++i) {
      roots.remove(&data[i]);
    }
  }
  RootedArray(const RootedArray&) = delete;
  RootedArray& operator=(const RootedArray&) = delete;
  Oop* ptr() const { return data.get(); }
};

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

// Hides hostTemps for the duration of this frame so a nested run cannot alias them.
// Writes the slots back when this frame's temps die, including early returns.
struct HostTempGuard {
  CallContext* ctx = nullptr;
  Temps* temps = nullptr;
  Oop* ptr = nullptr;
  std::uint32_t count = 0;
  Oop* prevPtr = nullptr;
  std::uint32_t prevCount = 0;
  bool active = false;

  HostTempGuard(CallContext& c, Temps& t, bool use) : ctx(&c), temps(&t) {
    if (!use) {
      return;
    }
    active = true;
    ptr = c.hostTemps;
    count = c.hostTempCount;
    prevPtr = c.hostTemps;
    prevCount = c.hostTempCount;
    c.hostTemps = nullptr;
    c.hostTempCount = 0;
  }

  ~HostTempGuard() {
    if (!active || ptr == nullptr) {
      return;
    }
    for (std::uint32_t i = 0; i < count; ++i) {
      Oop v;
      if (temps->at(i, &v)) {
        ptr[i] = v;
      }
    }
    ctx->hostTemps = prevPtr;
    ctx->hostTempCount = prevCount;
  }

  HostTempGuard(const HostTempGuard&) = delete;
  HostTempGuard& operator=(const HostTempGuard&) = delete;
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

Oop allocateRetry(CallContext& ctx, Oop cls, std::uint32_t size, std::uint16_t flags) {
  Oop obj = ctx.heap.allocate(cls, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  Root held(ctx.roots, cls);
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  return ctx.heap.allocate(held.slot, size, flags);
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

Leave performSend(CallContext& ctx, Frame& frame, OperandStack& stack, std::uint8_t argc, Oop selector,
                  bool isSuper, bool outermost) {
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

}  // namespace

Oop Interpreter::run(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                     Oop block) {
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
  NonlocalGuard nonlocal(ctx, depth.outermost);
  // Root inputs before the safepoint. applyMethod's caller may hold the method only in a register.
  Gc gc(ctx.heap, ctx.roots);
  gc.safepoint();

  frame->isBlock = blockHold.slot.isHeap() &&
                   ctx.heap.klass(blockHold.slot) == ctx.wk.blockContextClass;
  // activeContext defaults to empty, not nil. Top-level sender is nil.
  const Oop caller = ctx.activeContext.isHeap() ? ctx.activeContext : Oop::nil();
  if (frame->isBlock) {
    frame->context = blockHold.slot;
    ctx.heap.slotAtPut(frame->context, kCtxSender, caller);
  } else {
    frame->context = Context::createMethod(ctx, frame->method, frame->receiver, caller,
                                           static_cast<std::uint8_t>(argc));
    if (!frame->context.isHeap()) {
      return Oop{};
    }
  }

  ActiveGuard active(ctx, frame->context, depth.outermost);
  Temps temps(ctx.roots, numTemps);
  const bool useHost = !frame->isBlock && ctx.hostTemps != nullptr &&
                       ctx.hostTempCount == static_cast<std::uint32_t>(numTemps);
  if (useHost) {
    for (std::uint32_t i = 0; i < numTemps; ++i) {
      temps.put(i, ctx.hostTemps[i]);
    }
  }
  HostTempGuard hostBack(ctx, temps, useHost);
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
    if (opb > static_cast<std::uint8_t>(compiler::Op::Primitive)) {
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
          const char* name = compiler::specialSelector(argb[0]);
          if (name == nullptr) {
            return Oop{};
          }
          selector = ctx.wk.intern(name);
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
          take = (op == compiler::Op::JumpTrue) ? v.isTrue() : v.isFalse();
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
        ctx.nonlocalReturn = true;
        ctx.nonlocalHome =
            frame->isBlock ? ctx.heap.slotAt(frame->context, kBlockHome) : Oop::nil();
        ctx.nonlocalValue = v;
        const Leave nl =
            consumeNonlocal(ctx, !frame->isBlock, frame->context, depth.outermost);
        return nl.leave ? nl.value : v;
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
        Root created(ctx.roots,
                     Context::createBlock(ctx, blockMethod.slot, frame->receiver, home, arr.slot,
                                          frame->context, blockArgs));
        if (!created.slot.isHeap()) {
          return Oop{};
        }
        stack.push(created.slot);
        break;
      }
      case compiler::Op::Primitive:
        break;
    }
  }
}

Oop applyMethod(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                Oop block) {
  if (!method.isHeap()) {
    return Oop{};
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

}  // namespace ao

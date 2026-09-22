#include "ao/Compile.hpp"

#include "ao/CompiledMethod.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/MethodImage.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/Vendor.hpp"

#include <cstring>
#include <string>
#include <vector>

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

Oop boxBytes(CallContext& ctx, Oop cls, const std::uint8_t* p, std::uint32_t n) {
  Root o(ctx.roots, allocateRetry(ctx, cls, n, kFlagBytes));
  if (!o.slot.isHeap()) {
    return Oop{};
  }
  if (n != 0 && p != nullptr) {
    std::memcpy(ctx.heap.bytes(o.slot), p, n);
  }
  return o.slot;
}

Oop boxLiteral(CallContext& ctx, const compiler::Literal& lit, Oop methodClass) {
  switch (lit.kind) {
    case compiler::LitKind::Nil:
      return Oop::nil();
    case compiler::LitKind::True:
      return Oop::true_();
    case compiler::LitKind::False:
      return Oop::false_();
    case compiler::LitKind::Int:
      if (lit.intValue >= kSmiMin && lit.intValue <= kSmiMax) {
        return Oop::fromSmallInteger(lit.intValue);
      }
      return LargeInteger::fromInt64(ctx.heap, ctx.wk, lit.intValue);
    case compiler::LitKind::Float: {
      Root o(ctx.roots, allocateRetry(ctx, ctx.wk.floatClass, 8, kFlagBytes));
      if (!o.slot.isHeap()) {
        return Oop{};
      }
      std::memcpy(ctx.heap.bytes(o.slot), &lit.floatValue, sizeof(lit.floatValue));
      return o.slot;
    }
    case compiler::LitKind::Char:
      return Oop::fromCharacter(static_cast<char32_t>(lit.intValue));
    case compiler::LitKind::String: {
      Oop s = Str::fromUtf8(ctx.heap, ctx.wk, lit.text);
      if (!s.isHeap() && !lit.text.empty()) {
        Gc gc(ctx.heap, ctx.roots);
        gc.collectNursery();
        s = Str::fromUtf8(ctx.heap, ctx.wk, lit.text);
      }
      return s;
    }
    case compiler::LitKind::Symbol:
      return ctx.wk.intern(lit.text);
    case compiler::LitKind::Array: {
      const auto n = static_cast<std::uint32_t>(lit.elements.size());
      Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, n, 0));
      if (!arr.slot.isHeap()) {
        return Oop{};
      }
      Root mcls(ctx.roots, methodClass);
      for (std::uint32_t i = 0; i < n; ++i) {
        Oop e = boxLiteral(ctx, lit.elements[i], mcls.slot);
        ctx.heap.slotAtPut(arr.slot, i, e);
      }
      return arr.slot;
    }
    case compiler::LitKind::ByteArray:
      return boxBytes(ctx, ctx.wk.byteArrayClass, lit.bytes.data(),
                      static_cast<std::uint32_t>(lit.bytes.size()));
    case compiler::LitKind::Method:
      if (lit.method == nullptr) {
        return Oop{};
      }
      return boxMethodImage(ctx, *lit.method, methodClass);
  }
  return Oop{};
}

Oop boxUtf8(CallContext& ctx, std::string_view utf8) {
  Oop s = Str::fromUtf8(ctx.heap, ctx.wk, utf8);
  if (s.isHeap()) {
    return s;
  }
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  return Str::fromUtf8(ctx.heap, ctx.wk, utf8);
}

void fillInstVars(CallContext& ctx, Oop cls, compiler::CompileEnv& env) {
  std::vector<Oop> chain;
  Oop c = cls;
  while (c.isHeap()) {
    chain.push_back(c);
    c = ctx.heap.slotAt(c, kClassSlotSuperclass);
  }
  for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
    const Oop names = ctx.heap.slotAt(*it, kClassSlotInstVarNames);
    if (!names.isHeap() || (ctx.heap.flags(names) & kFlagBytes) != 0) {
      continue;
    }
    const auto n = ctx.heap.size(names);
    for (std::uint32_t i = 0; i < n; ++i) {
      const Oop name = ctx.heap.slotAt(names, i);
      if (!name.isHeap()) {
        continue;
      }
      env.instVarNames.push_back(Str::toUtf8(ctx.heap, name));
    }
  }
}

bool refusesKernelRedefinition(const WellKnown& wk, std::string_view className,
                               std::vector<compiler::CompileError>& errors) {
  if (wk.isCatalogName(className) && !isVendorStub(className)) {
    errors.push_back(
        compiler::CompileError{{}, "refusing to redefine kernel class: " + std::string(className)});
    return true;
  }
  return false;
}

bool applyClassDef(CallContext& ctx, const compiler::ChunkAction& action,
                   std::vector<compiler::CompileError>& errors) {
  if (refusesKernelRedefinition(ctx.wk, action.className, errors)) {
    return false;
  }
  Root super(ctx.roots, ctx.wk.named(action.superName));
  if (!super.slot.isHeap()) {
    errors.push_back(compiler::CompileError{{}, "missing class: " + action.superName});
    return false;
  }
  Root name(ctx.roots, ctx.wk.intern(action.className));
  Root ivars(ctx.roots, boxUtf8(ctx, action.instVars));
  Root cvars(ctx.roots, boxUtf8(ctx, action.classVars));
  Root pools(ctx.roots, boxUtf8(ctx, action.pools));
  Root cat(ctx.roots, boxUtf8(ctx, action.category));
  if (!name.slot.isHeap() || !ivars.slot.isHeap() || !cvars.slot.isHeap() || !pools.slot.isHeap() ||
      !cat.slot.isHeap()) {
    errors.push_back(compiler::CompileError{{}, "class definition allocation failed: " + action.className});
    return false;
  }
  Oop args[5] = {name.slot, ivars.slot, cvars.slot, pools.slot, cat.slot};
  const Oop sel = Symbol::intern(
      ctx.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  const Oop created = send(ctx, super.slot, sel, args, 5, nullptr);
  if (!created.isHeap()) {
    errors.push_back(compiler::CompileError{{}, "subclass failed: " + action.className});
    return false;
  }
  if (isVendorStub(action.className) && !ctx.wk.rebind(action.className, created)) {
    errors.push_back(compiler::CompileError{{}, "rebind failed: " + action.className});
    return false;
  }
  return true;
}

bool applyMethodsFor(CallContext& ctx, const compiler::ChunkAction& action,
                     std::vector<compiler::CompileError>& errors) {
  if (refusesKernelRedefinition(ctx.wk, action.className, errors)) {
    return false;
  }
  Root cls(ctx.roots, ctx.wk.named(action.className));
  if (!cls.slot.isHeap()) {
    errors.push_back(compiler::CompileError{{}, "missing class: " + action.className});
    return false;
  }
  const Oop target = action.meta ? ctx.heap.klass(cls.slot) : cls.slot;
  if (!target.isHeap()) {
    errors.push_back(compiler::CompileError{{}, "missing class: " + action.className});
    return false;
  }
  Root tgt(ctx.roots, target);
  compiler::CompileEnv env;
  fillInstVars(ctx, tgt.slot, env);
  for (const auto& m : action.methods) {
    compiler::CompileResult cr = compiler::compileMethod(m.source, env);
    if (!cr.ok) {
      compiler::CompileError e = std::move(cr.error);
      e.span.start += m.span.start;
      e.span.end += m.span.start;
      errors.push_back(std::move(e));
      continue;
    }
    const Oop dict = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
    const Oop sel = ctx.wk.intern(cr.image.selector);
    if (dict.isHeap() && sel.isHeap()) {
      const Oop existing = MethodDictionary::at(ctx.heap, dict, sel);
      if (existing.isHeap() && ctx.heap.klass(existing) == ctx.wk.nativeMethodClass) {
        errors.push_back(compiler::CompileError{
            m.span, "native selector overwrite refused: " + cr.image.selector});
        continue;
      }
    }
    const Oop installed = installMethod(ctx, tgt.slot, cr.image);
    if (!installed.isHeap()) {
      errors.push_back(compiler::CompileError{m.span, "install failed"});
    }
  }
  return true;
}

}  // namespace

Oop boxMethodImage(CallContext& ctx, const compiler::MethodImage& image, Oop methodClass) {
  Root mcls(ctx.roots, methodClass);
  const auto n = static_cast<std::uint32_t>(image.literals.size());
  Root lits(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, n, 0));
  if (!lits.slot.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < n; ++i) {
    Oop e = boxLiteral(ctx, image.literals[i], mcls.slot);
    ctx.heap.slotAtPut(lits.slot, i, e);
  }
  Root bytes(ctx.roots, boxBytes(ctx, ctx.wk.byteArrayClass, image.bytes.data(),
                                 static_cast<std::uint32_t>(image.bytes.size())));
  Root sel(ctx.roots, image.selector.empty() ? Oop::nil() : ctx.wk.intern(image.selector));
  return CompiledMethod::create(ctx, image.numArgs, image.numTemps, image.primitive, lits.slot,
                                bytes.slot, sel.slot, mcls.slot);
}

Oop installMethod(CallContext& ctx, Oop cls, const compiler::MethodImage& image) {
  Root c(ctx.roots, cls);
  Root cm(ctx.roots, boxMethodImage(ctx, image, c.slot));
  if (!cm.slot.isHeap() || !c.slot.isHeap()) {
    return Oop{};
  }
  const Oop dict = ctx.heap.slotAt(c.slot, kClassSlotMethodDict);
  if (!dict.isHeap()) {
    return Oop{};
  }
  Root d(ctx.roots, dict);
  const Oop sel = ctx.heap.slotAt(cm.slot, kCmSlotSelector);
  MethodDictionary::atPut(ctx.heap, d.slot, sel, cm.slot);
  return cm.slot;
}

bool applyChunks(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                 std::vector<compiler::CompileError>& errors) {
  for (const auto& action : actions) {
    switch (action.kind) {
      case compiler::ChunkKind::ClassDef:
        if (!applyClassDef(ctx, action, errors)) {
          return false;
        }
        break;
      case compiler::ChunkKind::MethodsFor:
        if (!applyMethodsFor(ctx, action, errors)) {
          return false;
        }
        break;
      case compiler::ChunkKind::DoIt:
        break;
    }
  }
  return true;
}

}  // namespace ao

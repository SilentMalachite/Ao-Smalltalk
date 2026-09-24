#include "ao/Compile.hpp"

#include "Session.hpp"
#include "ao/CompiledMethod.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Bytecode.hpp"
#include "ao/ClassPool.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/Format.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/MethodImage.hpp"
#include "ao/Natives.hpp"
#include "ao/Parser.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/Vendor.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace ao {
namespace {

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

// boxLiteral の結果が使えるか。即値の種類はそのまま使え、それ以外はヒープ Oop でなければならない
// （割り当てや intern の失敗は空 Oop になる）。
bool boxedOk(const compiler::Literal& lit, Oop boxed) {
  switch (lit.kind) {
    case compiler::LitKind::Nil:
    case compiler::LitKind::True:
    case compiler::LitKind::False:
    case compiler::LitKind::Char:
      return true;
    case compiler::LitKind::Int:
      return boxed.isSmallInteger() || boxed.isHeap();
    case compiler::LitKind::Float:
    case compiler::LitKind::String:
    case compiler::LitKind::Symbol:
    case compiler::LitKind::Array:
    case compiler::LitKind::ByteArray:
    case compiler::LitKind::Method:
    case compiler::LitKind::Binding:
    case compiler::LitKind::ClassVariable:
      return boxed.isHeap();
  }
  return boxed.isHeap();
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
      if (!lit.text.empty()) {
        // SPEC §3.8: outside int64, the compiler keeps the digits.
        return LargeInteger::fromText(ctx, lit.text);
      }
      if (lit.intValue >= kSmiMin && lit.intValue <= kSmiMax) {
        return Oop::fromSmallInteger(lit.intValue);
      }
      return LargeInteger::fromInt64(ctx, lit.intValue);
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
    case compiler::LitKind::String:
      return boxBytes(ctx, ctx.wk.stringClass,
                      reinterpret_cast<const std::uint8_t*>(lit.text.data()),
                      static_cast<std::uint32_t>(lit.text.size()));
    case compiler::LitKind::Symbol:
      return ctx.wk.intern(lit.text);
    case compiler::LitKind::Array: {
      const auto n = static_cast<std::uint32_t>(lit.elements.size());
      // methodClass は値で受けている。割り当て（GC）の前にルートに載せる。
      Root mcls(ctx.roots, methodClass);
      Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, n, 0));
      if (!arr.slot.isHeap()) {
        return Oop{};
      }
      for (std::uint32_t i = 0; i < n; ++i) {
        Oop e = boxLiteral(ctx, lit.elements[i], mcls.slot);
        if (!boxedOk(lit.elements[i], e)) {
          return Oop{};
        }
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
    case compiler::LitKind::Binding:
      // SPEC §3.10: the session finds or makes the workspace's Association for the name.
      if (ctx.bindingHook == nullptr) {
        return Oop{};
      }
      return ctx.bindingHook(ctx, lit.text);
    case compiler::LitKind::ClassVariable: {
      // SPEC §3.8: the binding in the classPool of methodClass (thisClass for a metaclass) or of
      // a superclass. It must be an Association, which LitVar reads and writes; the compile paths
      // refuse a method whose entry is not one first (unboundClassVariable).
      const Oop binding = ClassPool::visibleBinding(ctx.heap, ctx.wk, methodClass, lit.text);
      return ClassPool::isBinding(ctx.heap, ctx.wk, binding) ? binding : Oop{};
    }
  }
  return Oop{};
}

Oop boxUtf8(CallContext& ctx, std::string_view utf8) {
  return boxBytes(ctx, ctx.wk.stringClass, reinterpret_cast<const std::uint8_t*>(utf8.data()),
                  static_cast<std::uint32_t>(utf8.size()));
}

bool isKernelClass(const WellKnown& wk, Oop cls);

// SPEC §3.6: how many leading slots of an instance of cls a Kernel class adds, the instSize of the
// nearest Kernel class on cls's chain (cls too; on the class side a Kernel metaclass, whose slots
// are Behavior's). Source cannot assign them (SPEC §3.8). Does not collect.
std::size_t kernelSlotCount(CallContext& ctx, Oop cls) {
  SuperclassWalk walk(ctx.heap, cls);
  for (Oop c; walk.next(c);) {
    if (isKernelClass(ctx.wk, c)) {
      const std::int64_t size = Format::instSize(ctx.heap.slotAt(c, kClassSlotFormat));
      return size > 0 ? static_cast<std::size_t>(size) : 0;
    }
  }
  return 0;
}

// SPEC §3.6: the instance variables of cls, one per named slot (namedSlotNames). A slot with no
// name gets "<slot N>", which no identifier matches, so source cannot name it and the names after
// it keep their slots. The ones a Kernel class adds come first and are read-only (SPEC §3.8).
void fillInstVars(CallContext& ctx, Oop cls, compiler::CompileEnv& env) {
  const std::vector<Oop> names = namedSlotNames(ctx.heap, cls);
  for (std::size_t i = 0; i < names.size(); ++i) {
    if (names[i].isHeap()) {
      env.instVarNames.push_back(Str::toUtf8(ctx.heap, names[i]));
    } else {
      env.instVarNames.push_back("<slot " + std::to_string(i + 1) + ">");
    }
  }
  env.kernelInstVarCount = std::min(kernelSlotCount(ctx, cls), names.size());
}

// SPEC §3.8: the class variables a method of cls sees, those of cls (its thisClass for a
// metaclass) and of its superclasses, nearest first. Does not collect.
void fillClassVars(CallContext& ctx, Oop cls, compiler::CompileEnv& env) {
  env.classVarNames = ClassPool::visibleNames(ctx.heap, ctx.wk, cls);
}

// SPEC §3.6: the first class variable image, or a block in it at any depth, names whose entry a
// method of cls finds is not a binding (its classPool was changed in place), or "". Boxing that
// literal would fail, so the compile paths refuse the method with this name first. Does not
// collect.
std::string unboundClassVariable(CallContext& ctx, const compiler::MethodImage& image, Oop cls) {
  for (const compiler::Literal& lit : image.literals) {
    if (lit.kind == compiler::LitKind::ClassVariable &&
        !ClassPool::isBinding(ctx.heap, ctx.wk,
                              ClassPool::visibleBinding(ctx.heap, ctx.wk, cls, lit.text))) {
      return lit.text;
    }
    if (lit.kind == compiler::LitKind::Method && lit.method != nullptr) {
      std::string inner = unboundClassVariable(ctx, *lit.method, cls);
      if (!inner.empty()) {
        return inner;
      }
    }
  }
  return {};
}

std::string unboundClassVariableMessage(const std::string& var) {
  return "class variable " + var + " is not bound to an Association";
}

// A Kernel class (SPEC §3.6): file-in does not redefine it (SPEC §3.12), and accept does not
// hide a native it finds (SPEC §3.10). A vendor stub is not one.
bool isKernelClassName(const WellKnown& wk, std::string_view className) {
  return wk.isCatalogName(className) && !isVendorStub(className);
}

// SPEC §3.10 / §3.12: whether cls, the class a name resolved to, is a Kernel class. It compares
// identities, so an alias global (Smalltalk at: #IntegerAlias put: SmallInteger) is one too. A
// catalog class that is not a vendor stub is a Kernel class, and so is its metaclass.
bool isKernelClass(const WellKnown& wk, Oop cls) {
  struct Probe {
    const Heap& heap;
    Oop cls;
    bool found;
  } probe{wk.heap(), cls, false};
  wk.eachNativeRequiredClass(
      [](void* baton, Oop kernel) {
        auto* p = static_cast<Probe*>(baton);
        p->found = p->found || kernel == p->cls ||
                   (kernel.isHeap() && p->heap.klass(kernel) == p->cls);
      },
      &probe);
  return probe.found;
}

// The file is filled in by whoever knows it (fileInLoadOrder). method is set for a method-level
// error only (SPEC §3.12).
void addError(std::vector<FileInError>& errors, compiler::CompileError error,
              std::string method = {}) {
  errors.push_back(FileInError{{}, std::move(method), std::move(error)});
}

// SPEC §3.12: how DEFERRED.md names a method, `Class>>selector` or `Class class>>selector`.
std::string methodKey(const compiler::ChunkAction& action, std::string_view selector) {
  std::string key = action.className;
  if (action.meta) {
    key += " class";
  }
  key += ">>";
  key += selector;
  return key;
}

bool refusesKernelRedefinition(bool kernel, const compiler::ChunkAction& action,
                               std::vector<FileInError>& errors) {
  if (kernel) {
    addError(errors, {action.span, "refusing to redefine kernel class: " + action.className});
    return true;
  }
  return false;
}

bool applyClassDef(CallContext& ctx, const compiler::ChunkAction& action,
                   std::vector<FileInError>& errors) {
  // subclass: makes a new class and rebinds only the name (an alias too), never a Kernel class in
  // place. A catalog name is refused: define keeps its well-known slot.
  if (refusesKernelRedefinition(isKernelClassName(ctx.wk, action.className), action, errors)) {
    return false;
  }
  Root super(ctx.roots, ctx.wk.named(action.superName));
  if (!super.slot.isHeap()) {
    addError(errors, {action.span, "missing class: " + action.superName});
    return false;
  }
  Root name(ctx.roots, ctx.wk.intern(action.className));
  Root ivars(ctx.roots, boxUtf8(ctx, action.instVars));
  Root cvars(ctx.roots, boxUtf8(ctx, action.classVars));
  Root pools(ctx.roots, boxUtf8(ctx, action.pools));
  Root cat(ctx.roots, boxUtf8(ctx, action.category));
  if (!name.slot.isHeap() || !ivars.slot.isHeap() || !cvars.slot.isHeap() || !pools.slot.isHeap() ||
      !cat.slot.isHeap()) {
    addError(errors, {action.span, "class definition allocation failed: " + action.className});
    return false;
  }
  const Oop sel = Symbol::intern(
      ctx.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  // SPEC §3.4 / §3.12: the definition's send is an outermost evaluation. What an earlier one left
  // over is not blamed on it, and an abort it ends in is this chunk's error: the reason goes into
  // the message and the abort is cleared here, not carried into the next evaluation. Neither
  // clearing nor refreshing collects.
  clearUnwinding(ctx);
  refreshStackLimit(ctx);
  // Read the rooted slots after the last allocation. The send roots its own copies on entry.
  const Oop args[5] = {name.slot, ivars.slot, cvars.slot, pools.slot, cat.slot};
  const Oop created = send(ctx, super.slot, sel, args, 5, nullptr);
  if (unwinding(ctx)) {
    const std::string reason = abortReasonText(ctx);
    clearUnwinding(ctx);
    addError(errors, {action.span,
                      "subclass failed: " + action.className + ": " +
                          (reason.empty() ? std::string("evaluation aborted") : reason)});
    return false;
  }
  if (!created.isHeap()) {
    addError(errors, {action.span, "subclass failed: " + action.className});
    return false;
  }
  if (isVendorStub(action.className) && !ctx.wk.rebind(action.className, created)) {
    addError(errors, {action.span, "rebind failed: " + action.className});
    return false;
  }
  return true;
}

bool applyMethodsFor(CallContext& ctx, const compiler::ChunkAction& action,
                     std::vector<FileInError>& errors) {
  Root cls(ctx.roots, ctx.wk.named(action.className));
  if (!cls.slot.isHeap()) {
    addError(errors, {action.span, "missing class: " + action.className});
    return false;
  }
  if (refusesKernelRedefinition(isKernelClass(ctx.wk, cls.slot), action, errors)) {
    return false;
  }
  const Oop target = action.meta ? ctx.heap.klass(cls.slot) : cls.slot;
  if (!target.isHeap()) {
    addError(errors, {action.span, "missing class: " + action.className});
    return false;
  }
  Root tgt(ctx.roots, target);
  compiler::CompileEnv env;
  fillInstVars(ctx, tgt.slot, env);
  fillClassVars(ctx, tgt.slot, env);
  // SPEC §3.12: a method-level error names its method and does not stop the rest.
  for (const auto& m : action.methods) {
    compiler::CompileResult cr = compiler::compileMethod(m.source, env);
    if (!cr.ok) {
      compiler::CompileError e = std::move(cr.error);
      e.span = compiler::fileSpan(m, e.span);
      // The pattern parses even when the body does not, so the partial method has its selector.
      addError(errors, std::move(e),
               methodKey(action, compiler::parseMethod(m.source).method.name));
      continue;
    }
    const Oop dict = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
    const Oop sel = ctx.wk.intern(cr.image.selector);
    if (dict.isHeap() && sel.isHeap()) {
      const Oop existing = MethodDictionary::at(ctx.heap, dict, sel);
      if (existing.isHeap() && ctx.heap.klass(existing) == ctx.wk.nativeMethodClass) {
        addError(errors, {m.span, "native selector overwrite refused: " + cr.image.selector},
                 methodKey(action, cr.image.selector));
        continue;
      }
    }
    if (const std::string var = unboundClassVariable(ctx, cr.image, tgt.slot); !var.empty()) {
      addError(errors, {m.span, unboundClassVariableMessage(var)},
               methodKey(action, cr.image.selector));
      continue;
    }
    const Oop installed = installMethod(ctx, tgt.slot, cr.image);
    if (!installed.isHeap()) {
      addError(errors, {m.span, "install failed"}, methodKey(action, cr.image.selector));
    }
  }
  return true;
}

// SPEC §3.9: the instance variable names a definition gives, split as subclass: splits them.
std::vector<std::string> definedInstVarNames(std::string_view spec) {
  const auto blank = [](char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };
  std::vector<std::string> names;
  std::size_t i = 0;
  while (i < spec.size()) {
    while (i < spec.size() && blank(spec[i])) {
      ++i;
    }
    const std::size_t start = i;
    while (i < spec.size() && !blank(spec[i])) {
      ++i;
    }
    if (start < i) {
      names.emplace_back(spec.substr(start, i - start));
    }
  }
  return names;
}

// SPEC §3.6: the class variable names a definition gives, split the same way, each name once.
std::vector<std::string> definedClassVarNames(std::string_view spec) {
  std::vector<std::string> names;
  for (std::string& name : definedInstVarNames(spec)) {
    if (std::find(names.begin(), names.end(), name) == names.end()) {
      names.push_back(std::move(name));
    }
  }
  return names;
}

// The names in cls's own instVarNames slot, in order. cls is class-shaped.
std::vector<std::string> ownInstVarNames(CallContext& ctx, Oop cls) {
  std::vector<std::string> names;
  const Oop arr = ctx.heap.slotAt(cls, kClassSlotInstVarNames);
  if (!arr.isHeap() || (ctx.heap.flags(arr) & kFlagBytes) != 0) {
    return names;
  }
  const auto n = ctx.heap.size(arr);
  for (std::uint32_t i = 0; i < n; ++i) {
    const Oop name = ctx.heap.slotAt(arr, i);
    if (name.isHeap()) {
      names.push_back(Str::toUtf8(ctx.heap, name));
    }
  }
  return names;
}

// The name cls carries itself. cls is class-shaped.
std::string ownClassName(CallContext& ctx, Oop cls) {
  const Oop name = ctx.heap.slotAt(cls, kClassSlotName);
  if (!name.isHeap() || (ctx.heap.flags(name) & kFlagBytes) == 0) {
    return {};
  }
  return Str::toUtf8(ctx.heap, name);
}

// SPEC §3.9: every class alive now, whether Smalltalk binds a name to it or not (the class of an
// instance a variable holds, a class a reshape left behind). A class is alive when a live object
// reaches it, traced the way Gc::collectOld marks: from the roots, through each object's class and
// its pointer slots, not through weak ones. The method cache and the method source table are roots
// no Smalltalk object reaches, and they keep methods (so their classes) nothing else does, so the
// trace does not start from them. A class no other root reaches is not answered, collected yet or
// not; nor is a metaclass. Does not collect: the answer is raw Oops.
std::vector<Oop> liveClasses(CallContext& ctx) {
  struct Start {
    std::unordered_set<const Oop*> hidden;
    std::uintptr_t cacheBegin = 0;
    std::uintptr_t cacheEnd = 0;
    std::vector<Oop> work;
  } start;
  for (const Oop* slot : methodSourceRootSlots()) {
    start.hidden.insert(slot);
  }
  if (ctx.cache != nullptr) {
    start.cacheBegin = reinterpret_cast<std::uintptr_t>(std::begin(ctx.cache->entries));
    start.cacheEnd = reinterpret_cast<std::uintptr_t>(std::end(ctx.cache->entries));
  }
  ctx.roots.visitAll(
      [](void* p, Oop* slot) {
        auto* s = static_cast<Start*>(p);
        const auto at = reinterpret_cast<std::uintptr_t>(slot);
        if (slot != nullptr && (at < s->cacheBegin || at >= s->cacheEnd) &&
            s->hidden.count(slot) == 0) {
          s->work.push_back(*slot);
        }
      },
      &start);
  std::vector<Oop>& work = start.work;
  std::unordered_set<std::uintptr_t> seen;
  std::vector<Oop> classes;
  while (!work.empty()) {
    const Oop obj = work.back();
    work.pop_back();
    if (!obj.isHeap() || !seen.insert(reinterpret_cast<std::uintptr_t>(obj.heapPointer())).second) {
      continue;
    }
    const Oop meta = ctx.heap.klass(obj);
    work.push_back(meta);
    if ((ctx.heap.flags(obj) & (kFlagBytes | kFlagWeak)) != 0) {
      continue;
    }
    const std::uint32_t n = ctx.heap.size(obj);
    for (std::uint32_t i = 0; i < n; ++i) {
      work.push_back(ctx.heap.slotAt(obj, i));
    }
    // A class is the thisClass of its metaclass, which is an instance of Metaclass.
    if (isClassShaped(ctx.heap, obj) && isClassShaped(ctx.heap, meta) &&
        ctx.heap.klass(meta) == ctx.wk.metaclassClass &&
        ctx.heap.slotAt(meta, kClassSlotThisClass) == obj) {
      classes.push_back(obj);
    }
  }
  return classes;
}

// SPEC §3.9: whether a live class, named or not, has cls as its superclass. Does not collect.
bool hasSubclass(CallContext& ctx, Oop cls) {
  const std::vector<Oop> live = liveClasses(ctx);
  return std::any_of(live.begin(), live.end(),
                     [&](Oop each) { return superclassOf(ctx.heap, each) == cls; });
}

// Binds name to cls again, the way subclass: and applyClassDef bound it (a vendor stub through its
// well-known slot), and drops the method cache (SPEC §3.3).
void rebindClassName(CallContext& ctx, const std::string& name, Oop cls) {
  if (isVendorStub(name)) {
    ctx.wk.rebind(name, cls);
  } else {
    ctx.wk.define(name, cls);
  }
  invalidateMethodCache(ctx.cache, Oop{});
}

// A method a shape change carries over to the new class (SPEC §3.9): its side, its selector, its
// source from the source table and that source compiled for the new shape.
struct CarriedMethod {
  bool meta = false;
  std::string selector;
  std::string source;
  compiler::MethodImage image;
};

// `Name>>selector` or `Name class>>selector`, the way a refusal names the method.
std::string carriedMethodName(const std::string& className, const CarriedMethod& m) {
  std::string key = className;
  if (m.meta) {
    key += " class";
  }
  key += ">>";
  key += m.selector;
  return key;
}

// Calls visit(m) for method, a CompiledMethod, then for each CompiledMethod among its literals (its
// blocks) and among theirs, at any depth, each once, until visit answers true. Whether one did.
// Does not collect.
template <typename Visit>
bool anyMethodIn(CallContext& ctx, Oop method, Visit&& visit) {
  std::vector<Oop> work{method};
  std::vector<Oop> seen;
  while (!work.empty()) {
    const Oop m = work.back();
    work.pop_back();
    if (!m.isHeap() || ctx.heap.klass(m) != ctx.wk.compiledMethodClass ||
        ctx.heap.size(m) <= kCmSlotLiterals ||
        std::find(seen.begin(), seen.end(), m) != seen.end()) {
      continue;
    }
    seen.push_back(m);
    if (visit(m)) {
      return true;
    }
    const Oop lits = ctx.heap.slotAt(m, kCmSlotLiterals);
    if (!lits.isHeap() || (ctx.heap.flags(lits) & kFlagBytes) != 0) {
      continue;
    }
    const std::uint32_t n = ctx.heap.size(lits);
    for (std::uint32_t i = 0; i < n; ++i) {
      work.push_back(ctx.heap.slotAt(lits, i));
    }
  }
  return false;
}

// SPEC §3.9: the first variable that method, a CompiledMethod, or a block inside it at any depth
// reads or writes as it was compiled and that the change removes, or "". An instance variable is
// used by its slot (PushInstVar, StoreInstVar, PopStoreInstVar), which slotNames names; a class
// variable through its binding (PushLitVar, StoreLitVar, PopStoreLitVar), whose key names it. The
// compiled code tells what a name meant, so a name the new shape resolves to a variable of the
// other kind still counts, and sends, Symbols, locals and globals never do. Does not collect.
std::string usedRemovedVariable(CallContext& ctx, Oop method,
                                const std::vector<std::string>& slotNames,
                                const std::vector<std::string>& removedInstVars,
                                const std::vector<std::string>& removedClassVars) {
  auto removes = [](const std::vector<std::string>& removed, const std::string& name) {
    return std::find(removed.begin(), removed.end(), name) != removed.end();
  };
  std::string used;
  anyMethodIn(ctx, method, [&](Oop m) {
    const Oop code = ctx.heap.size(m) > kCmSlotBytes ? ctx.heap.slotAt(m, kCmSlotBytes) : Oop{};
    if (!code.isHeap() || (ctx.heap.flags(code) & kFlagBytes) == 0) {
      return false;
    }
    const Oop lits = ctx.heap.slotAt(m, kCmSlotLiterals);
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(ctx.heap.bytes(code));
    const std::uint32_t n = ctx.heap.size(code);
    constexpr auto kLastOpByte = static_cast<std::uint8_t>(compiler::kLastOp);
    for (std::uint32_t pc = 0; pc < n && bytes[pc] <= kLastOpByte;) {
      const auto op = static_cast<compiler::Op>(bytes[pc]);
      const std::uint32_t width = compiler::operandBytes(op);
      if (pc + width >= n) {
        break;
      }
      const std::uint8_t operand = width == 0 ? 0 : bytes[pc + 1];
      pc += 1 + width;
      if (op == compiler::Op::PushInstVar || op == compiler::Op::StoreInstVar ||
          op == compiler::Op::PopStoreInstVar) {
        if (operand < slotNames.size() && removes(removedInstVars, slotNames[operand])) {
          used = slotNames[operand];
          return true;
        }
      } else if (op == compiler::Op::PushLitVar || op == compiler::Op::StoreLitVar ||
                 op == compiler::Op::PopStoreLitVar) {
        if (!lits.isHeap() || (ctx.heap.flags(lits) & kFlagBytes) != 0 ||
            operand >= ctx.heap.size(lits)) {
          continue;
        }
        const Oop binding = ctx.heap.slotAt(lits, operand);
        if (!binding.isHeap() || (ctx.heap.flags(binding) & kFlagBytes) != 0 ||
            ctx.heap.size(binding) <= kAssocKey) {
          continue;
        }
        const Oop key = ctx.heap.slotAt(binding, kAssocKey);
        if (key.isHeap() && (ctx.heap.flags(key) & kFlagBytes) != 0) {
          std::string name = Str::toUtf8(ctx.heap, key);
          if (removes(removedClassVars, name)) {
            used = std::move(name);
            return true;
          }
        }
      }
    }
    return false;
  });
  return used;
}

// Whether method, a CompiledMethod, or a block inside it at any depth holds one of bindings as a
// literal (a class variable it reads or writes); *which is then its index. Does not collect.
bool holdsBinding(CallContext& ctx, Oop method, const std::vector<Oop>& bindings,
                  std::size_t* which) {
  return anyMethodIn(ctx, method, [&](Oop m) {
    const Oop lits = ctx.heap.slotAt(m, kCmSlotLiterals);
    if (!lits.isHeap() || (ctx.heap.flags(lits) & kFlagBytes) != 0) {
      return false;
    }
    const std::uint32_t n = ctx.heap.size(lits);
    for (std::uint32_t i = 0; i < n; ++i) {
      const auto found = std::find(bindings.begin(), bindings.end(), ctx.heap.slotAt(lits, i));
      if (found != bindings.end()) {
        *which = static_cast<std::size_t>(found - bindings.begin());
        return true;
      }
    }
    return false;
  });
}

// SPEC §3.9: the first method of a live class, on either side, that holds the binding of one of
// `dropped` (names in cls's classPool): "<Class>>><selector>" (" class" on the class side) and the
// name, or "" when no method holds one. The classes that can hold one are cls, the classes below
// it, named or not, and the old versions of those a reshape left behind, which share the bindings.
// Does not collect.
std::pair<std::string, std::string> methodHoldingDropped(CallContext& ctx, Oop cls,
                                                         const std::vector<std::string>& dropped) {
  std::vector<Oop> bindings;
  std::vector<std::string> names;
  const Oop pool = ctx.heap.slotAt(cls, kClassSlotClassPool);
  for (const std::string& name : dropped) {
    const Oop binding = ClassPool::bindingAt(ctx.heap, pool, name);
    if (binding.isHeap()) {
      bindings.push_back(binding);
      names.push_back(name);
    }
  }
  if (bindings.empty()) {
    return {};
  }
  // cls first, then the named classes below it, then every other live class.
  struct Probe {
    const Heap& heap;
    Oop cls;
    std::vector<Oop> classes;
  } probe{ctx.heap, cls, {cls}};
  ctx.wk.eachClass(
      [](void* baton, Oop each) {
        auto* p = static_cast<Probe*>(baton);
        if (chainIncludes(p->heap, each, p->cls) &&
            std::find(p->classes.begin(), p->classes.end(), each) == p->classes.end()) {
          p->classes.push_back(each);
        }
      },
      &probe);
  const auto named = static_cast<std::ptrdiff_t>(probe.classes.size());
  for (const Oop each : liveClasses(ctx)) {
    const auto namedEnd = probe.classes.begin() + named;
    if (std::find(probe.classes.begin(), namedEnd, each) == namedEnd) {
      probe.classes.push_back(each);
    }
  }
  for (const Oop c : probe.classes) {
    for (const bool meta : {false, true}) {
      const Oop side = meta ? ctx.heap.klass(c) : c;
      const Oop dict = isClassShaped(ctx.heap, side) ? ctx.heap.slotAt(side, kClassSlotMethodDict)
                                                     : Oop::nil();
      if (!dict.isHeap() || (ctx.heap.flags(dict) & kFlagBytes) != 0 ||
          ctx.heap.size(dict) <= kDictSlotArray) {
        continue;
      }
      const Oop inner = ctx.heap.slotAt(dict, kDictSlotArray);
      if (!inner.isHeap() || (ctx.heap.flags(inner) & kFlagBytes) != 0) {
        continue;
      }
      const std::uint32_t n = ctx.heap.size(inner);
      for (std::uint32_t i = 0; i + 1 < n; i += 2) {
        const Oop key = ctx.heap.slotAt(inner, i);
        std::size_t which = 0;
        if (key.isHeap() && holdsBinding(ctx, ctx.heap.slotAt(inner, i + 1), bindings, &which)) {
          std::string where = ownClassName(ctx, c);
          if (meta) {
            where += " class";
          }
          where += ">>";
          where += Str::toUtf8(ctx.heap, key);
          return {where, names[which]};
        }
      }
    }
  }
  return {};
}

// SPEC §3.9: the same shape keeps the class object, its method dictionaries, its metaclass and
// its instances. The category changes, and the classPool takes the definition's class variables:
// a kept name keeps its binding and value, a new name gets a binding with nil, and a dropped name
// is refused while a method of the class or of a class below it holds its binding.
bool recategorizeClass(CallContext& ctx, Root& cls, const compiler::ChunkAction& action,
                       std::vector<FileInError>& errors) {
  const std::vector<std::string> classVars = definedClassVarNames(action.classVars);
  const std::vector<std::string> current =
      ClassPool::names(ctx.heap, ctx.heap.slotAt(cls.slot, kClassSlotClassPool));
  // SPEC §3.9: a dropped name whose binding a method holds would leave that method reading and
  // writing a variable the classPool no longer has. Checked before anything changes.
  std::vector<std::string> dropped;
  for (const std::string& name : current) {
    if (std::find(classVars.begin(), classVars.end(), name) == classVars.end()) {
      dropped.push_back(name);
    }
  }
  if (!dropped.empty()) {
    const auto [method, var] = methodHoldingDropped(ctx, cls.slot, dropped);
    if (!method.empty()) {
      addError(errors, {action.span, "class variable change refused: " + method +
                                         " refers to removed class variable " + var});
      return false;
    }
  }
  Root cat(ctx.roots, boxUtf8(ctx, action.category));
  if (!cat.slot.isHeap()) {
    addError(errors, {action.span, "class definition allocation failed: " + action.className});
    return false;
  }
  // A kept name keeps its binding (adopt does not collect); a new one gets a fresh binding. The
  // names compare as sets: ClassPool::names lists them in byte order (SPEC §3.6), whatever order
  // the definition writes them in.
  Root pool(ctx.roots, Oop::nil());
  std::vector<std::string> wanted = classVars;
  std::sort(wanted.begin(), wanted.end());
  const bool poolChanges = wanted != current;
  if (poolChanges) {
    pool.slot = ClassPool::make(ctx, classVars);
    if (!pool.slot.isHeap()) {
      addError(errors, {action.span, "class definition allocation failed: " + action.className});
      return false;
    }
    ClassPool::adopt(ctx.heap, pool.slot, ctx.heap.slotAt(cls.slot, kClassSlotClassPool));
  }
  // subclass: puts the category on the class and on its metaclass.
  ctx.heap.slotAtPut(cls.slot, kClassSlotCategory, cat.slot);
  const Oop meta = ctx.heap.klass(cls.slot);
  if (isClassShaped(ctx.heap, meta)) {
    ctx.heap.slotAtPut(meta, kClassSlotCategory, cat.slot);
  }
  if (poolChanges) {
    ctx.heap.slotAtPut(cls.slot, kClassSlotClassPool, pool.slot);
  }
  return true;
}

// SPEC §3.9: compiles each carried method in the given shape (instance side, class side) and
// refuses one that does not compile, an instance-side one whose old method (oldMethods, in the
// order of carried) reads or writes an instance variable the old class has (inherited ones too)
// and the shape lacks, or one on either side whose old method reads or writes a class variable the
// old class sees and the shape does not. It allocates nothing.
bool compileCarried(CallContext& ctx, const Root& old, const RootedArray& oldMethods,
                    const compiler::CompileEnv& instanceEnv, const compiler::CompileEnv& classEnv,
                    std::vector<CarriedMethod>& carried, const compiler::ChunkAction& action,
                    std::vector<FileInError>& errors) {
  const std::string refused = "shape change refused: ";
  for (CarriedMethod& m : carried) {
    compiler::CompileResult cr = compiler::compileMethod(m.source, m.meta ? classEnv : instanceEnv);
    if (!cr.ok) {
      addError(errors, {action.span, refused + carriedMethodName(action.className, m) +
                                         " does not compile: " + cr.error.message});
      return false;
    }
    m.image = std::move(cr.image);
  }
  // A removed variable compiles to a global, or to the other kind of variable of that name, in the
  // new shape, so the method would change meaning silently. The old method, as it was compiled,
  // tells which variables it uses.
  compiler::CompileEnv oldEnv;
  fillInstVars(ctx, old.slot, oldEnv);
  std::vector<std::string> removed;
  for (const std::string& name : oldEnv.instVarNames) {
    const std::vector<std::string>& kept = instanceEnv.instVarNames;
    if (std::find(kept.begin(), kept.end(), name) == kept.end()) {
      removed.push_back(name);
    }
  }
  for (std::uint32_t i = 0; i < carried.size() && !removed.empty(); ++i) {
    const std::string var =
        carried[i].meta ? std::string{}
                        : usedRemovedVariable(ctx, oldMethods[i], oldEnv.instVarNames, removed, {});
    if (!var.empty()) {
      addError(errors, {action.span, refused + carriedMethodName(action.className, carried[i]) +
                                         " refers to removed instance variable " + var});
      return false;
    }
  }
  // So does a class variable the old class sees and the new definition does not, on either side.
  std::vector<std::string> removedClassVars;
  for (std::string& name : ClassPool::visibleNames(ctx.heap, ctx.wk, old.slot)) {
    const std::vector<std::string>& kept = instanceEnv.classVarNames;
    if (std::find(kept.begin(), kept.end(), name) == kept.end()) {
      removedClassVars.push_back(std::move(name));
    }
  }
  for (std::uint32_t i = 0; i < carried.size() && !removedClassVars.empty(); ++i) {
    const std::string var = usedRemovedVariable(ctx, oldMethods[i], {}, {}, removedClassVars);
    if (!var.empty()) {
      addError(errors, {action.span, refused + carriedMethodName(action.className, carried[i]) +
                                         " refers to removed class variable " + var});
      return false;
    }
  }
  return true;
}

// A new method dictionary with the pairs of dict, or the empty Oop when dict is none or the copy
// cannot be allocated (old at its max). Does not collect.
Oop copyMethodDictionary(CallContext& ctx, Oop dict) {
  if (!dict.isHeap() || (ctx.heap.flags(dict) & kFlagBytes) != 0 ||
      ctx.heap.size(dict) <= kDictSlotArray) {
    return Oop{};
  }
  const Oop inner = ctx.heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap() || (ctx.heap.flags(inner) & kFlagBytes) != 0) {
    return Oop{};
  }
  const std::uint32_t n = ctx.heap.size(inner);
  const Oop copy = MethodDictionary::create(ctx.heap, ctx.wk, std::max<std::uint32_t>(n / 2, 1));
  if (!copy.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = ctx.heap.slotAt(inner, i);
    const Oop method = ctx.heap.slotAt(inner, i + 1);
    if (key.isHeap() && !MethodDictionary::atPut(ctx.heap, copy, key, method)) {
      return Oop{};
    }
  }
  return copy;
}

// SPEC §3.9: a new shape. Every check runs before anything changes: the class has no live
// subclass, each method on either side has its source in the source table, each source compiles
// for the new shape, and no old method reads or writes, as it was compiled, an instance variable
// the new shape drops (instance side) or a class variable it drops (either side). Then
// applyClassDef makes the new class and binds the name to it, and the checks run again when the
// class the send answered has another layout or other class variables. Only then does that class
// change: its classPool takes the old bindings of the names it keeps, the methods go in, and their
// sources move over. When the subclass: send, that second check or an allocation fails (old at its
// max), the name goes back to the old class, whose methods were never touched, and the class the
// send answered gets its classPool and method dictionaries back.
bool reshapeClass(CallContext& ctx, Root& old, const std::vector<std::string>& instVars,
                  const compiler::ChunkAction& action, std::vector<FileInError>& errors) {
  const std::string refused = "shape change refused: ";
  if (hasSubclass(ctx, old.slot)) {
    addError(errors, {action.span, refused + action.className + " has subclasses"});
    return false;
  }
  // Gathered without collecting, then rooted before anything allocates.
  std::vector<CarriedMethod> carried;
  std::vector<Oop> found;
  for (const bool meta : {false, true}) {
    const Oop side = meta ? ctx.heap.klass(old.slot) : old.slot;
    const Oop dict = isClassShaped(ctx.heap, side) ? ctx.heap.slotAt(side, kClassSlotMethodDict)
                                                   : Oop::nil();
    if (!dict.isHeap() || (ctx.heap.flags(dict) & kFlagBytes) != 0 ||
        ctx.heap.size(dict) <= kDictSlotArray) {
      continue;
    }
    const Oop inner = ctx.heap.slotAt(dict, kDictSlotArray);
    if (!inner.isHeap() || (ctx.heap.flags(inner) & kFlagBytes) != 0) {
      continue;
    }
    const auto n = ctx.heap.size(inner);
    for (std::uint32_t i = 0; i + 1 < n; i += 2) {
      const Oop key = ctx.heap.slotAt(inner, i);
      const Oop method = ctx.heap.slotAt(inner, i + 1);
      if (!key.isHeap() || !method.isHeap()) {
        continue;
      }
      CarriedMethod m;
      m.meta = meta;
      m.selector = Str::toUtf8(ctx.heap, key);
      // A NativeMethod has no source either.
      if (ctx.heap.klass(method) == ctx.wk.nativeMethodClass || !methodSource(method, m.source)) {
        addError(errors,
                 {action.span, refused + carriedMethodName(action.className, m) + " has no source"});
        return false;
      }
      carried.push_back(std::move(m));
      found.push_back(method);
    }
  }
  const auto count = static_cast<std::uint32_t>(carried.size());
  RootedArray oldMethods(ctx.roots, count);
  for (std::uint32_t i = 0; i < count; ++i) {
    oldMethods[i] = found[i];
  }
  found.clear();

  // The new class's variables are its superclass chain's and then its own; its metaclass adds
  // none to the superclass's metaclass chain (see subclass:). Both sides see the class variables
  // the definition declares and then its superclasses' (SPEC §3.6).
  compiler::CompileEnv instanceEnv;
  compiler::CompileEnv classEnv;
  {
    const Oop super = ctx.wk.named(action.superName);
    fillInstVars(ctx, super, instanceEnv);
    instanceEnv.instVarNames.insert(instanceEnv.instVarNames.end(), instVars.begin(),
                                    instVars.end());
    fillInstVars(ctx, ctx.heap.klass(super), classEnv);
    instanceEnv.classVarNames = definedClassVarNames(action.classVars);
    for (std::string& name : ClassPool::visibleNames(ctx.heap, ctx.wk, super)) {
      std::vector<std::string>& seen = instanceEnv.classVarNames;
      if (std::find(seen.begin(), seen.end(), name) == seen.end()) {
        seen.push_back(std::move(name));
      }
    }
    classEnv.classVarNames = instanceEnv.classVarNames;
  }
  if (!compileCarried(ctx, old, oldMethods, instanceEnv, classEnv, carried, action, errors)) {
    return false;
  }

  if (!applyClassDef(ctx, action, errors)) {
    // SPEC §3.9: the send may have bound the name to a new class before it failed (a superclass's
    // class-side override that sends super, then answers no class or aborts). The name goes back.
    if (ctx.wk.named(action.className) != old.slot) {
      rebindClassName(ctx, action.className, old.slot);
    }
    return false;
  }
  Root fresh(ctx.roots, ctx.wk.named(action.className));
  if (!isClassShaped(ctx.heap, fresh.slot) ||
      !isClassShaped(ctx.heap, ctx.heap.klass(fresh.slot)) || fresh.slot == old.slot) {
    rebindClassName(ctx, action.className, old.slot);
    addError(errors, {action.span, "subclass failed: " + action.className});
    return false;
  }
  // SPEC §3.9: the methods move in the shape and with the class variables the send answered,
  // which a superclass's class-side override can make differ from the definition's. Checked before
  // the class the send answered, which may be an existing class, changes at all.
  {
    compiler::CompileEnv freshInstanceEnv;
    compiler::CompileEnv freshClassEnv;
    fillInstVars(ctx, fresh.slot, freshInstanceEnv);
    fillClassVars(ctx, fresh.slot, freshInstanceEnv);
    fillInstVars(ctx, ctx.heap.klass(fresh.slot), freshClassEnv);
    fillClassVars(ctx, ctx.heap.klass(fresh.slot), freshClassEnv);
    if ((freshInstanceEnv.instVarNames != instanceEnv.instVarNames ||
         freshClassEnv.instVarNames != classEnv.instVarNames ||
         freshInstanceEnv.kernelInstVarCount != instanceEnv.kernelInstVarCount ||
         freshClassEnv.kernelInstVarCount != classEnv.kernelInstVarCount ||
         freshInstanceEnv.classVarNames != instanceEnv.classVarNames ||
         freshClassEnv.classVarNames != classEnv.classVarNames) &&
        !compileCarried(ctx, old, oldMethods, freshInstanceEnv, freshClassEnv, carried, action,
                        errors)) {
      rebindClassName(ctx, action.className, old.slot);
      return false;
    }
  }

  // From here on, only an allocation (old at its max) or a classPool entry that is not a binding
  // (SPEC §3.6) stops the move. The class the send answered takes a new classPool and new method
  // dictionaries, copies of its own that the moves go into, and gets its own objects back when the
  // move stops.
  Root pool(ctx.roots, ctx.heap.slotAt(fresh.slot, kClassSlotClassPool));
  Root dict(ctx.roots, ctx.heap.slotAt(fresh.slot, kClassSlotMethodDict));
  Root metaDict(ctx.roots, ctx.heap.slotAt(ctx.heap.klass(fresh.slot), kClassSlotMethodDict));
  auto putBack = [&](const std::string& message) {
    ctx.heap.slotAtPut(fresh.slot, kClassSlotClassPool, pool.slot);
    ctx.heap.slotAtPut(fresh.slot, kClassSlotMethodDict, dict.slot);
    ctx.heap.slotAtPut(ctx.heap.klass(fresh.slot), kClassSlotMethodDict, metaDict.slot);
    rebindClassName(ctx, action.className, old.slot);
    addError(errors, {action.span, message});
    return false;
  };
  const std::string failed = "shape change failed: ";
  // SPEC §3.9: a name the old classPool has keeps its entry in the new class, so its value and the
  // old class's methods stay shared, and the moved methods box that binding. adopt does not
  // collect.
  const std::vector<std::string> poolNames = ClassPool::names(ctx.heap, pool.slot);
  if (std::any_of(poolNames.begin(), poolNames.end(), [&](const std::string& name) {
        return !ClassPool::bindingAt(ctx.heap, ctx.heap.slotAt(old.slot, kClassSlotClassPool), name)
                    .isEmpty();
      })) {
    const Oop copy = ClassPool::make(ctx, poolNames);
    if (!copy.isHeap()) {
      return putBack(failed + action.className + " classPool allocation failed");
    }
    ClassPool::adopt(ctx.heap, copy, pool.slot);
    ClassPool::adopt(ctx.heap, copy, ctx.heap.slotAt(old.slot, kClassSlotClassPool));
    ctx.heap.slotAtPut(fresh.slot, kClassSlotClassPool, copy);
  }
  // SPEC §3.6: a moved method must find a binding for each class variable it names, in the pool
  // the new class now has and in its superclasses'.
  for (const CarriedMethod& m : carried) {
    const Oop side = m.meta ? ctx.heap.klass(fresh.slot) : fresh.slot;
    if (const std::string var = unboundClassVariable(ctx, m.image, side); !var.empty()) {
      return putBack("shape change refused: " + carriedMethodName(action.className, m) + ": " +
                     unboundClassVariableMessage(var));
    }
  }
  for (const bool meta : {false, true}) {
    const Oop side = meta ? ctx.heap.klass(fresh.slot) : fresh.slot;
    const Oop copy = copyMethodDictionary(ctx, ctx.heap.slotAt(side, kClassSlotMethodDict));
    if (!copy.isHeap()) {
      return putBack(failed + action.className + (meta ? " class" : "") +
                     " methodDict allocation failed");
    }
    ctx.heap.slotAtPut(side, kClassSlotMethodDict, copy);
  }
  RootedArray installed(ctx.roots, count);
  for (std::uint32_t i = 0; i < count; ++i) {
    const Oop side = carried[i].meta ? ctx.heap.klass(fresh.slot) : fresh.slot;
    const Oop method = installMethod(ctx, side, carried[i].image);
    if (!method.isHeap()) {
      return putBack(failed + carriedMethodName(action.className, carried[i]) + " install failed");
    }
    installed[i] = method;
  }
  for (std::uint32_t i = 0; i < count; ++i) {
    moveMethodSource(oldMethods[i], installed[i]);
  }
  // SPEC §3.3: the class was replaced. installMethod dropped each selector already.
  invalidateMethodCache(ctx.cache, Oop{});
  return true;
}

// SPEC §3.9「クラス定義の再 Accept」: a class definition through ao_accept_class. When the name
// already names its own class (not a Kernel class), the same shape (the superclass the name
// resolves to, the same instance variable names in the same order) keeps that class and changes
// its category, and a new shape is reshapeClass's. A superclass that is the class the name names,
// or a class below it, is refused. Any other definition (a new name, a Kernel name, an alias, a
// superclass that is not a class) is applyClassDef's, as for file-in.
bool acceptClassDef(CallContext& ctx, const compiler::ChunkAction& action,
                    std::vector<FileInError>& errors) {
  if (isKernelClassName(ctx.wk, action.className) || !namesBehavior(ctx, action.className) ||
      !namesBehavior(ctx, action.superName)) {
    return applyClassDef(ctx, action, errors);
  }
  Root old(ctx.roots, ctx.wk.named(action.className));
  // SPEC §3.9: a superclass that is the class the name names now (an alias too), or below it,
  // would put the new class under the old one, one level deeper on every re-accept.
  if (chainIncludes(ctx.heap, ctx.wk.named(action.superName), old.slot)) {
    addError(errors, {action.span, "superclass refused: " + action.superName + " is " +
                                       action.className + " or its subclass"});
    return false;
  }
  if (isKernelClass(ctx.wk, old.slot) || ownClassName(ctx, old.slot) != action.className) {
    return applyClassDef(ctx, action, errors);
  }
  const std::vector<std::string> instVars = definedInstVarNames(action.instVars);
  if (ctx.heap.slotAt(old.slot, kClassSlotSuperclass) == ctx.wk.named(action.superName) &&
      ownInstVarNames(ctx, old.slot) == instVars) {
    return recategorizeClass(ctx, old, action, errors);
  }
  return reshapeClass(ctx, old, instVars, action, errors);
}

// False when a chunk stopped the file-in (a class definition failed, a methodsFor: was refused
// or named no class). Errors that do not stop it are only added to errors.
bool applyChunkActions(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                       std::vector<FileInError>& errors, bool accept = false) {
  for (const auto& action : actions) {
    switch (action.kind) {
      case compiler::ChunkKind::ClassDef:
        // SPEC §3.9: only ao_accept_class keeps or reshapes an existing class; file-in does not.
        if (!(accept ? acceptClassDef(ctx, action, errors) : applyClassDef(ctx, action, errors))) {
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

bool readSource(const std::filesystem::path& path, std::string* out) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    return false;
  }
  std::ostringstream buf;
  buf << in.rdbuf();
  if (!in && !in.eof()) {
    return false;
  }
  *out = buf.str();
  return true;
}

// One file a LOAD_ORDER lists. False when it cannot be read or a chunk stopped it.
bool fileInListedFile(CallContext& ctx, const std::filesystem::path& path,
                      std::vector<FileInError>& errors) {
  std::string src;
  if (!readSource(path, &src)) {
    addError(errors, {{}, "cannot read: " + path.string()});
    return false;
  }
  std::vector<compiler::CompileError> parseErrors;
  const std::vector<compiler::ChunkAction> actions = compiler::parseChunks(src, parseErrors);
  for (auto& e : parseErrors) {
    addError(errors, std::move(e));
  }
  return applyChunkActions(ctx, actions, errors);
}

bool isClassName(std::string_view name) {
  if (name.empty() || std::isalpha(static_cast<unsigned char>(name.front())) == 0) {
    return false;
  }
  return std::all_of(name.begin(), name.end(), [](char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
  });
}

// A DEFERRED.md line (SPEC §3.12). `Class>>selector: reason` or `Class class>>selector: reason`
// answers `Class>>selector` (`Class class>>selector`); the selector is the text up to the first
// ": ", so a keyword selector is written `Class>>at:put:: reason`. Any other line is a note and
// answers "".
std::string deferredListing(std::string_view line) {
  while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t')) {
    line.remove_suffix(1);
  }
  const std::size_t arrows = line.find(">>");
  if (arrows == std::string_view::npos) {
    return {};
  }
  const std::string_view owner = line.substr(0, arrows);
  constexpr std::string_view kClassSide = " class";
  std::string_view name = owner;
  if (name.size() > kClassSide.size() &&
      name.substr(name.size() - kClassSide.size()) == kClassSide) {
    name.remove_suffix(kClassSide.size());
  }
  if (!isClassName(name)) {
    return {};
  }
  const std::string_view rest = line.substr(arrows + 2);
  const std::size_t colon = rest.find(": ");
  if (colon == 0 || colon == std::string_view::npos) {
    return {};
  }
  const std::string_view selector = rest.substr(0, colon);
  if (selector.find_first_of(" \t") != std::string_view::npos) {
    return {};
  }
  std::string key(owner);
  key += ">>";
  key += selector;
  return key;
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
    if (!boxedOk(image.literals[i], e)) {
      return Oop{};
    }
    ctx.heap.slotAtPut(lits.slot, i, e);
  }
  Root bytes(ctx.roots, boxBytes(ctx, ctx.wk.byteArrayClass, image.bytes.data(),
                                 static_cast<std::uint32_t>(image.bytes.size())));
  if (!bytes.slot.isHeap()) {
    return Oop{};
  }
  // intern は GC しない。old も上限なら空 Oop になり、そのセレクタのメソッドは作らない。
  Root sel(ctx.roots, image.selector.empty() ? Oop::nil() : ctx.wk.intern(image.selector));
  if (!image.selector.empty() && !sel.slot.isHeap()) {
    return Oop{};
  }
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
  const Oop sel = ctx.heap.slotAt(cm.slot, kCmSlotSelector);
  if (!sel.isHeap()) {
    return Oop{};
  }
  // atPut は GC しない。辞書を伸ばせなければ（old が上限）登録せずに失敗を返す。
  if (!MethodDictionary::atPut(ctx.heap, dict, sel, cm.slot)) {
    return Oop{};
  }
  // SPEC §3.3: accept and file-in method chunks both come here. atPut did not GC, so sel is valid.
  invalidateMethodCache(ctx.cache, sel);
  return cm.slot;
}

bool applyChunks(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                 std::vector<compiler::CompileError>& errors) {
  std::vector<FileInError> found;
  const bool completed = applyChunkActions(ctx, actions, found);
  for (auto& e : found) {
    errors.push_back(std::move(e.error));
  }
  return completed && found.empty();
}

bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors) {
  const std::size_t before = errors.size();
  const std::vector<compiler::ChunkAction> actions = compiler::parseChunks(src, errors);
  return applyChunks(ctx, actions, errors) && errors.size() == before;
}

bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors) {
  std::string src;
  if (!readSource(path, &src)) {
    errors.push_back(compiler::CompileError{{}, "cannot read: " + path.string()});
    return false;
  }
  return fileInString(ctx, src, errors);
}

std::vector<std::string> deferredMethods(const std::filesystem::path& deferredMd) {
  std::vector<std::string> listed;
  std::ifstream in(deferredMd, std::ios::binary);
  std::string line;
  while (in && std::getline(in, line)) {
    std::string key = deferredListing(line);
    if (!key.empty()) {
      listed.push_back(std::move(key));
    }
  }
  return listed;
}

bool fileInLoadOrder(CallContext& ctx, const std::filesystem::path& loadOrder,
                     std::vector<FileInError>& errors, std::vector<FileInError>* deferred) {
  const std::size_t before = errors.size();
  std::ifstream in(loadOrder);
  if (!in) {
    errors.push_back(FileInError{loadOrder.string(), {}, {{}, "cannot read: " + loadOrder.string()}});
    return false;
  }
  const std::filesystem::path base = loadOrder.parent_path();
  const std::vector<std::string> listed = deferredMethods(base / "DEFERRED.md");
  std::vector<FileInError> found;
  bool completed = true;
  std::string line;
  while (completed && std::getline(in, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    std::size_t begin = 0;
    while (begin < line.size() && (line[begin] == ' ' || line[begin] == '\t')) {
      ++begin;
    }
    std::size_t end = line.size();
    while (end > begin && (line[end - 1] == ' ' || line[end - 1] == '\t')) {
      --end;
    }
    if (begin == end || line[begin] == '#') {
      continue;
    }
    const std::filesystem::path path = base / line.substr(begin, end - begin);
    const std::size_t first = found.size();
    completed = fileInListedFile(ctx, path, found);
    for (std::size_t i = first; i < found.size(); ++i) {
      found[i].file = path.string();
    }
  }
  if (completed && !in.eof()) {
    found.push_back(FileInError{loadOrder.string(), {}, {{}, "cannot read: " + loadOrder.string()}});
    completed = false;
  }
  // SPEC §3.12: the error of a method DEFERRED.md lists is neither counted nor reported.
  for (FileInError& e : found) {
    const bool isDeferred =
        !e.method.empty() && std::find(listed.begin(), listed.end(), e.method) != listed.end();
    if (!isDeferred) {
      errors.push_back(std::move(e));
    } else if (deferred != nullptr) {
      deferred->push_back(std::move(e));
    }
  }
  return completed && errors.size() == before;
}

void assignError(compiler::CompileError* error, std::string message) {
  if (error == nullptr) {
    return;
  }
  error->span = {};
  error->message = std::move(message);
}

bool namesBehavior(CallContext& ctx, std::string_view className) {
  // A class inherits from Behavior through its metaclass, a metaclass through Metaclass.
  // Processor, Smalltalk, nil and other globals do not (SPEC §3.10).
  const Oop obj = ctx.wk.named(className);
  return isClassShaped(ctx.heap, obj) &&
         chainIncludes(ctx.heap, ctx.heap.klass(obj), ctx.wk.behaviorClass);
}

bool acceptMethodSource(CallContext& ctx, std::string_view className, bool meta,
                        std::string_view source, compiler::CompileError* error) {
  if (error != nullptr) {
    *error = {};
  }
  if (!namesBehavior(ctx, className)) {
    assignError(error, "missing class: " + std::string(className));
    return false;
  }
  Root cls(ctx.roots, ctx.wk.named(className));
  const Oop side = meta ? ctx.heap.klass(cls.slot) : cls.slot;
  if (!side.isHeap()) {
    assignError(error, "missing class: " + std::string(className));
    return false;
  }
  Root tgt(ctx.roots, side);
  compiler::CompileEnv env;
  fillInstVars(ctx, tgt.slot, env);
  fillClassVars(ctx, tgt.slot, env);
  compiler::CompileResult cr = compiler::compileMethod(source, env);
  if (!cr.ok) {
    if (error != nullptr) {
      *error = std::move(cr.error);
    }
    return false;
  }

  Root old(ctx.roots, Oop::nil());
  bool findsNative = false;
  {
    const Oop dict = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
    const Oop sel = ctx.wk.intern(cr.image.selector);
    if (dict.isHeap() && sel.isHeap()) {
      old.slot = MethodDictionary::at(ctx.heap, dict, sel);
    }
    // SPEC §3.10: in a Kernel class, a native the selector finds through the superclasses is
    // not hidden either. Kernel-ness is the class the name resolved to, so an alias is one too.
    // Neither intern nor lookup GCs, so the raw Oops stay valid.
    const Oop found = sel.isHeap() && isKernelClass(ctx.wk, cls.slot)
                          ? lookup(ctx.heap, tgt.slot, sel)
                          : old.slot;
    findsNative = found.isHeap() && ctx.heap.klass(found) == ctx.wk.nativeMethodClass;
  }
  if (findsNative) {
    assignError(error, "native selector overwrite refused: " + cr.image.selector);
    return false;
  }
  if (const std::string var = unboundClassVariable(ctx, cr.image, tgt.slot); !var.empty()) {
    assignError(error, unboundClassVariableMessage(var));
    return false;
  }

  Root text(ctx.roots, boxUtf8(ctx, source));
  if (!text.slot.isHeap()) {
    assignError(error, "method source allocation failed");
    return false;
  }
  Root kept(ctx.roots, installMethod(ctx, tgt.slot, cr.image));
  if (!kept.slot.isHeap()) {
    assignError(error, "install failed");
    return false;
  }
  const Oop selNow = ctx.heap.slotAt(kept.slot, kCmSlotSelector);
  const Oop dictNow = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
  if (!dictNow.isHeap() || !selNow.isHeap() ||
      MethodDictionary::at(ctx.heap, dictNow, selNow) != kept.slot) {
    assignError(error, "install failed");
    return false;
  }
  const Oop replaced = old.slot.isHeap() ? old.slot : Oop{};
  rememberMethodSource(kept.slot, text.slot, replaced);
  return true;
}

bool acceptClassSource(CallContext& ctx, std::string_view source, compiler::CompileError* error) {
  if (error != nullptr) {
    *error = {};
  }
  std::vector<compiler::CompileError> errors;
  const std::vector<compiler::ChunkAction> actions = compiler::parseChunks(source, errors);
  // SPEC §3.10: only class definitions and methodsFor: chunks. Every chunk is checked before any
  // is applied, so a stray expression or method body leaves the image as it was. A definition
  // chunk is its message alone, and a chunk after the `! !` that ended a methodsFor: section is an
  // expression (a DoIt, SPEC §3.8), not one of its methods.
  const bool definitionsOnly =
      !actions.empty() &&
      std::all_of(actions.begin(), actions.end(), [](const compiler::ChunkAction& action) {
        switch (action.kind) {
          case compiler::ChunkKind::ClassDef:
            return action.soleDefinition;
          case compiler::ChunkKind::MethodsFor:
            return true;
          case compiler::ChunkKind::DoIt:
            return false;
        }
        return false;
      });
  if (!definitionsOnly) {
    assignError(error, "not a class definition");
    return false;
  }
  // SPEC §3.9: a definition of an existing class keeps it or reshapes it (acceptClassDef).
  std::vector<FileInError> found;
  const bool completed = applyChunkActions(ctx, actions, found, /*accept=*/true);
  for (auto& e : found) {
    errors.push_back(std::move(e.error));
  }
  if (completed && errors.empty()) {
    return true;
  }
  if (error != nullptr) {
    if (!errors.empty()) {
      *error = std::move(errors.front());
    } else {
      error->message = "class definition failed";
    }
  }
  return false;
}

}  // namespace ao

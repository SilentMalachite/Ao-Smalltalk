#include "ao/Compile.hpp"

#include "Session.hpp"
#include "ao/CompiledMethod.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/MethodImage.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/Vendor.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
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
  }
  return Oop{};
}

Oop boxUtf8(CallContext& ctx, std::string_view utf8) {
  return boxBytes(ctx, ctx.wk.stringClass, reinterpret_cast<const std::uint8_t*>(utf8.data()),
                  static_cast<std::uint32_t>(utf8.size()));
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
  const Oop sel = Symbol::intern(
      ctx.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  // Read the rooted slots after the last allocation. The send roots its own copies on entry.
  const Oop args[5] = {name.slot, ivars.slot, cvars.slot, pools.slot, cat.slot};
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

bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors) {
  const std::vector<compiler::ChunkAction> actions = compiler::parseChunks(src, errors);
  return applyChunks(ctx, actions, errors);
}

bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors) {
  std::ifstream in(path, std::ios::binary);
  if (!in) {
    errors.push_back(compiler::CompileError{{}, "cannot read: " + path.string()});
    return false;
  }
  std::ostringstream buf;
  buf << in.rdbuf();
  if (!in && !in.eof()) {
    errors.push_back(compiler::CompileError{{}, "cannot read: " + path.string()});
    return false;
  }
  const std::string src = buf.str();
  return fileInString(ctx, src, errors);
}

bool fileInLoadOrder(CallContext& ctx, const std::filesystem::path& loadOrder,
                     std::vector<compiler::CompileError>& errors) {
  std::ifstream in(loadOrder);
  if (!in) {
    errors.push_back(compiler::CompileError{{}, "cannot read: " + loadOrder.string()});
    return false;
  }
  const std::filesystem::path base = loadOrder.parent_path();
  std::string line;
  while (std::getline(in, line)) {
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
    if (!fileInFile(ctx, path, errors)) {
      return false;
    }
  }
  if (!in.eof()) {
    errors.push_back(compiler::CompileError{{}, "cannot read: " + loadOrder.string()});
    return false;
  }
  return true;
}

void assignError(compiler::CompileError* error, std::string message) {
  if (error == nullptr) {
    return;
  }
  error->span = {};
  error->message = std::move(message);
}

bool acceptMethodSource(CallContext& ctx, std::string_view className, bool meta,
                        std::string_view source, compiler::CompileError* error) {
  if (error != nullptr) {
    *error = {};
  }
  Root cls(ctx.roots, ctx.wk.named(className));
  if (!cls.slot.isHeap()) {
    assignError(error, "missing class: " + std::string(className));
    return false;
  }
  const Oop side = meta ? ctx.heap.klass(cls.slot) : cls.slot;
  if (!side.isHeap()) {
    assignError(error, "missing class: " + std::string(className));
    return false;
  }
  Root tgt(ctx.roots, side);
  compiler::CompileEnv env;
  fillInstVars(ctx, tgt.slot, env);
  compiler::CompileResult cr = compiler::compileMethod(source, env);
  if (!cr.ok) {
    if (error != nullptr) {
      *error = std::move(cr.error);
    }
    return false;
  }

  Root old(ctx.roots, Oop::nil());
  {
    const Oop dict = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
    if (dict.isHeap()) {
      const Oop sel = ctx.wk.intern(cr.image.selector);
      if (sel.isHeap()) {
        old.slot = MethodDictionary::at(ctx.heap, dict, sel);
      }
    }
  }
  if (old.slot.isHeap() && ctx.heap.klass(old.slot) == ctx.wk.nativeMethodClass) {
    assignError(error, "native selector overwrite refused: " + cr.image.selector);
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
  if (ctx.cache != nullptr) {
    ctx.cache->forget(ctx.heap, tgt.slot, selNow);
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
  const bool ok = fileInString(ctx, source, errors);
  if (ok && errors.empty()) {
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

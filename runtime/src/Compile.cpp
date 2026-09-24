#include "ao/Compile.hpp"

#include "Session.hpp"
#include "ao/CompiledMethod.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/MethodImage.hpp"
#include "ao/Parser.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/Vendor.hpp"

#include <algorithm>
#include <cctype>
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
    case compiler::LitKind::Binding:
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
    case compiler::LitKind::Binding:
      // SPEC §3.10: the session finds or makes the workspace's Association for the name.
      if (ctx.bindingHook == nullptr) {
        return Oop{};
      }
      return ctx.bindingHook(ctx, lit.text);
  }
  return Oop{};
}

Oop boxUtf8(CallContext& ctx, std::string_view utf8) {
  return boxBytes(ctx, ctx.wk.stringClass, reinterpret_cast<const std::uint8_t*>(utf8.data()),
                  static_cast<std::uint32_t>(utf8.size()));
}

void fillInstVars(CallContext& ctx, Oop cls, compiler::CompileEnv& env) {
  for (const Oop c : superclassChainFromRoot(ctx.heap, cls)) {
    const Oop names = ctx.heap.slotAt(c, kClassSlotInstVarNames);
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
  // SPEC §3.12: a method-level error names its method and does not stop the rest.
  for (const auto& m : action.methods) {
    compiler::CompileResult cr = compiler::compileMethod(m.source, env);
    if (!cr.ok) {
      compiler::CompileError e = std::move(cr.error);
      e.span.start += m.span.start;
      e.span.end += m.span.start;
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
    const Oop installed = installMethod(ctx, tgt.slot, cr.image);
    if (!installed.isHeap()) {
      addError(errors, {m.span, "install failed"}, methodKey(action, cr.image.selector));
    }
  }
  return true;
}

// False when a chunk stopped the file-in (a class definition failed, a methodsFor: was refused
// or named no class). Errors that do not stop it are only added to errors.
bool applyChunkActions(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                       std::vector<FileInError>& errors) {
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
  // expression, not one of its methods.
  const bool definitionsOnly =
      !actions.empty() &&
      std::all_of(actions.begin(), actions.end(), [](const compiler::ChunkAction& action) {
        switch (action.kind) {
          case compiler::ChunkKind::ClassDef:
            return action.soleDefinition;
          case compiler::ChunkKind::MethodsFor:
            return std::none_of(action.methods.begin(), action.methods.end(),
                                [](const compiler::ChunkMethod& m) { return m.afterSectionEnd; });
          case compiler::ChunkKind::DoIt:
            return false;
        }
        return false;
      });
  if (!definitionsOnly) {
    assignError(error, "not a class definition");
    return false;
  }
  const bool ok = applyChunks(ctx, actions, errors);
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

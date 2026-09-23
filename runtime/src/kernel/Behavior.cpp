#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Format.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/MethodDictionary.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace ao {
namespace {

Oop classFormat(CallContext& ctx, Oop cls) {
  if (!cls.isHeap()) {
    return Oop{};
  }
  return ctx.heap.slotAt(cls, kClassSlotFormat);
}

void splitNames(std::string_view s, std::vector<std::string_view>& out) {
  std::size_t i = 0;
  while (i < s.size()) {
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) {
      ++i;
    }
    const std::size_t start = i;
    while (i < s.size() && s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r') {
      ++i;
    }
    if (start < i) {
      out.emplace_back(s.data() + start, i - start);
    }
  }
}

Oop makeInstVarNames(CallContext& ctx, std::string_view spec) {
  std::vector<std::string_view> names;
  splitNames(spec, names);
  Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(names.size()), 0));
  if (!arr.slot.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < names.size(); ++i) {
    Oop sym = ctx.wk.intern(names[i]);
    if (!sym.isHeap()) {
      return Oop{};
    }
    ctx.heap.slotAtPut(arr.slot, i, sym);
  }
  return arr.slot;
}

}  // namespace

Oop ao_Behavior_basicNew_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  if (argc != 1) return Oop{};
  if (!receiver.isHeap() || !args[0].isSmallInteger()) {
    return Oop{};
  }
  const auto n = args[0].smallIntegerValue();
  if (n < 0) {
    return Oop{};
  }
  const Oop fmt = classFormat(ctx, receiver);
  const auto inst = Format::instSize(fmt);
  if (!Format::isIndexable(fmt)) {
    return allocateRetry(ctx, receiver, static_cast<std::uint32_t>(inst), 0);
  }
  if (Format::isBytes(fmt)) {
    return allocateRetry(ctx, receiver, static_cast<std::uint32_t>(n), kFlagBytes);
  }
  const auto total = inst + n;
  if (total < 0 || total > static_cast<std::int64_t>(UINT32_MAX)) {
    return Oop{};
  }
  return allocateRetry(ctx, receiver, static_cast<std::uint32_t>(total), 0);
}

Oop ao_Behavior_basicNew(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  const Oop fmt = classFormat(ctx, receiver);
  if (Format::isIndexable(fmt)) {
    Oop zero = Oop::fromSmallInteger(0);
    return NativeMethod::invoke(ctx, ao_Behavior_basicNew_, receiver, &zero, 1);
  }
  return allocateRetry(ctx, receiver, static_cast<std::uint32_t>(Format::instSize(fmt)), 0);
}

Oop ao_Behavior_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  const Oop fmt = classFormat(ctx, receiver);
  if (Format::isIndexable(fmt)) {
    Oop zero = Oop::fromSmallInteger(0);
    return NativeMethod::invoke(ctx, ao_Behavior_basicNew_, receiver, &zero, 1);
  }
  return ao_Behavior_basicNew(ctx, receiver, nullptr, 0);
}

Oop ao_Behavior_superclass(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  return ctx.heap.slotAt(receiver, kClassSlotSuperclass);
}

Oop ao_Behavior_methodDict(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  return ctx.heap.slotAt(receiver, kClassSlotMethodDict);
}

Oop ao_Behavior_selectors(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  const Oop dict = ctx.heap.slotAt(receiver, kClassSlotMethodDict);
  if (!dict.isHeap()) {
    return allocateRetry(ctx, ctx.wk.arrayClass, 0, 0);
  }
  Root inner(ctx.roots, ctx.heap.slotAt(dict, kDictSlotArray));
  if (!inner.slot.isHeap()) {
    return allocateRetry(ctx, ctx.wk.arrayClass, 0, 0);
  }
  const auto n = ctx.heap.size(inner.slot);
  std::uint32_t count = 0;
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (!ctx.heap.slotAt(inner.slot, i).isNil()) {
      ++count;
    }
  }
  Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, count, 0));
  if (!arr.slot.isHeap()) {
    return Oop{};
  }
  std::uint32_t j = 0;
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = ctx.heap.slotAt(inner.slot, i);
    if (!key.isNil()) {
      ctx.heap.slotAtPut(arr.slot, j++, key);
    }
  }
  return arr.slot;
}

Oop ao_Behavior_compiledMethodAt_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) return Oop{};
  return MethodDictionary::at(ctx.heap, ctx.heap.slotAt(receiver, kClassSlotMethodDict), args[0]);
}

Oop ao_Behavior_includesSelector_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) return Oop{};
  const Oop meth =
      MethodDictionary::at(ctx.heap, ctx.heap.slotAt(receiver, kClassSlotMethodDict), args[0]);
  return meth.isNil() ? Oop::false_() : Oop::true_();
}

Oop ao_Behavior_inheritsFrom_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) return Oop{};
  Oop super = ctx.heap.slotAt(receiver, kClassSlotSuperclass);
  while (super.isHeap()) {
    if (super == args[0]) {
      return Oop::true_();
    }
    super = ctx.heap.slotAt(super, kClassSlotSuperclass);
  }
  return Oop::false_();
}

Oop ao_Behavior_instSize(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::fromSmallInteger(Format::instSize(classFormat(ctx, receiver)));
}

Oop ao_Behavior_isVariable(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Format::isIndexable(classFormat(ctx, receiver)) ? Oop::true_() : Oop::false_();
}

Oop ao_Behavior_isBytes(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Format::isBytes(classFormat(ctx, receiver)) ? Oop::true_() : Oop::false_();
}

Oop ao_Behavior_isPointers(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Format::isPointers(classFormat(ctx, receiver)) ? Oop::true_() : Oop::false_();
}

Oop ao_Class_name(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  return ctx.heap.slotAt(receiver, kClassSlotName);
}

Oop ao_Class_category(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  return ctx.heap.slotAt(receiver, kClassSlotCategory);
}

Oop ao_Class_classPool(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  return ctx.heap.slotAt(receiver, kClassSlotClassPool);
}

Oop ao_Class_subclass_instanceVariableNames_classVariableNames_poolDictionaries_category_(
    CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 5 || !receiver.isHeap()) return Oop{};
  const Oop nameArg = args[0];
  const std::string nameBytes = Str::toUtf8(ctx.heap, nameArg);
  if (nameBytes.empty() && !(nameArg.isHeap() && ctx.heap.size(nameArg) == 0)) {
    return Oop{};
  }
  const Oop superFmt = classFormat(ctx, receiver);
  const auto superInst = Format::instSize(superFmt);
  const std::string ivarSpec = Str::toUtf8(ctx.heap, args[1]);
  std::vector<std::string_view> ivars;
  splitNames(ivarSpec, ivars);
  const auto instSize = superInst + static_cast<std::int64_t>(ivars.size());
  const Oop fmt =
      Format::make(instSize, Format::isIndexable(superFmt), Format::isBytes(superFmt));
  const Oop classFmt = Format::make(static_cast<std::int64_t>(kClassSlotCount), false, false);

  Root cls(ctx.roots, allocateRetry(ctx, Oop::nil(), kClassSlotCount, 0));
  if (!cls.slot.isHeap()) return Oop{};
  Root meta(ctx.roots, allocateRetry(ctx, ctx.wk.metaclassClass, kClassSlotCount, 0));
  if (!meta.slot.isHeap()) return Oop{};
  ctx.heap.header(cls.slot)->klass = meta.slot;

  Root dict(ctx.roots, MethodDictionary::create(ctx.heap, ctx.wk, 8));
  if (!dict.slot.isHeap()) {
    Gc gc(ctx.heap, ctx.roots);
    gc.collectNursery();
    dict.slot = MethodDictionary::create(ctx.heap, ctx.wk, 8);
  }
  Root metaDict(ctx.roots, MethodDictionary::create(ctx.heap, ctx.wk, 8));
  if (!metaDict.slot.isHeap()) {
    Gc gc(ctx.heap, ctx.roots);
    gc.collectNursery();
    metaDict.slot = MethodDictionary::create(ctx.heap, ctx.wk, 8);
  }
  if (!dict.slot.isHeap() || !metaDict.slot.isHeap()) {
    return Oop{};
  }

  Root ivarNames(ctx.roots, makeInstVarNames(ctx, ivarSpec));
  if (!ivarNames.slot.isHeap()) {
    return Oop{};
  }

  std::string metaName = nameBytes;
  metaName += " class";
  Root metaNameOop(ctx.roots, Str::fromUtf8(ctx.heap, ctx.wk, metaName));

  ctx.heap.slotAtPut(cls.slot, kClassSlotSuperclass, receiver);
  ctx.heap.slotAtPut(cls.slot, kClassSlotMethodDict, dict.slot);
  ctx.heap.slotAtPut(cls.slot, kClassSlotFormat, fmt);
  ctx.heap.slotAtPut(cls.slot, kClassSlotName, nameArg);
  ctx.heap.slotAtPut(cls.slot, kClassSlotThisClass, Oop::nil());
  ctx.heap.slotAtPut(cls.slot, kClassSlotCategory, args[4]);
  ctx.heap.slotAtPut(cls.slot, kClassSlotClassPool, Oop::nil());
  ctx.heap.slotAtPut(cls.slot, kClassSlotInstVarNames, ivarNames.slot);

  const Oop metaSuper = ctx.heap.klass(receiver);
  ctx.heap.slotAtPut(meta.slot, kClassSlotSuperclass, metaSuper);
  ctx.heap.slotAtPut(meta.slot, kClassSlotMethodDict, metaDict.slot);
  ctx.heap.slotAtPut(meta.slot, kClassSlotFormat, classFmt);
  ctx.heap.slotAtPut(meta.slot, kClassSlotName, metaNameOop.slot);
  ctx.heap.slotAtPut(meta.slot, kClassSlotThisClass, cls.slot);
  ctx.heap.slotAtPut(meta.slot, kClassSlotCategory, args[4]);
  ctx.heap.slotAtPut(meta.slot, kClassSlotClassPool, Oop::nil());
  ctx.heap.slotAtPut(meta.slot, kClassSlotInstVarNames, Oop::nil());

  ctx.wk.define(nameBytes, cls.slot);
  return cls.slot;
}

Oop ao_Metaclass_thisClass(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  return ctx.heap.slotAt(receiver, kClassSlotThisClass);
}

Oop ao_Metaclass_newForbidden(CallContext& ctx, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Str::fromUtf8(ctx.heap, ctx.wk, "shouldNotImplement");
}

namespace kernel {

void installBehavior(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.behaviorClass;
  putNative(heap, wk, cls, "new", 0, "ao_Behavior_new", ao_Behavior_new);
  putNative(heap, wk, cls, "basicNew", 0, "ao_Behavior_basicNew", ao_Behavior_basicNew);
  putNative(heap, wk, cls, "basicNew:", 1, "ao_Behavior_basicNew_", ao_Behavior_basicNew_);
  putNative(heap, wk, cls, "superclass", 0, "ao_Behavior_superclass", ao_Behavior_superclass);
  putNative(heap, wk, cls, "methodDict", 0, "ao_Behavior_methodDict", ao_Behavior_methodDict);
  putNative(heap, wk, cls, "selectors", 0, "ao_Behavior_selectors", ao_Behavior_selectors);
  putNative(heap, wk, cls, "compiledMethodAt:", 1, "ao_Behavior_compiledMethodAt_",
            ao_Behavior_compiledMethodAt_);
  putNative(heap, wk, cls, "includesSelector:", 1, "ao_Behavior_includesSelector_",
            ao_Behavior_includesSelector_);
  putNative(heap, wk, cls, "inheritsFrom:", 1, "ao_Behavior_inheritsFrom_",
            ao_Behavior_inheritsFrom_);
  putNative(heap, wk, cls, "instSize", 0, "ao_Behavior_instSize", ao_Behavior_instSize);
  putNative(heap, wk, cls, "isVariable", 0, "ao_Behavior_isVariable", ao_Behavior_isVariable);
  putNative(heap, wk, cls, "isBytes", 0, "ao_Behavior_isBytes", ao_Behavior_isBytes);
  putNative(heap, wk, cls, "isPointers", 0, "ao_Behavior_isPointers", ao_Behavior_isPointers);

  const Oop classCls = wk.classClass;
  putNative(heap, wk, classCls, "name", 0, "ao_Class_name", ao_Class_name);
  putNative(heap, wk, classCls, "category", 0, "ao_Class_category", ao_Class_category);
  putNative(heap, wk, classCls, "classPool", 0, "ao_Class_classPool", ao_Class_classPool);
  putNative(heap, wk, classCls,
            "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:", 5,
            "ao_Class_subclass_instanceVariableNames_classVariableNames_poolDictionaries_category_",
            ao_Class_subclass_instanceVariableNames_classVariableNames_poolDictionaries_category_);

  const Oop meta = wk.metaclassClass;
  putNative(heap, wk, meta, "thisClass", 0, "ao_Metaclass_thisClass", ao_Metaclass_thisClass);
  putNative(heap, wk, meta, "new", 0, "ao_Metaclass_newForbidden", ao_Metaclass_newForbidden);
  putNative(heap, wk, wk.metaclassMetaclass, "new", 0, "ao_Metaclass_newForbidden",
            ao_Metaclass_newForbidden);
}

}  // namespace kernel
}  // namespace ao

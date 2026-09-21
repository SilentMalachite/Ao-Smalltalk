#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Format.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

#include <cstring>
#include <string_view>
#include <vector>

namespace ao {
namespace {

Oop sendValue(CallContext& ctx, Oop block) {
  return send(ctx, block, ctx.wk.selValue, nullptr, 0, nullptr);
}

Oop fail(CallContext& ctx, Oop receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx.heap, ctx.wk, msg);
  return ao_Object_error_(ctx, receiver, &s, 1);
}

Oop classFormat(CallContext& ctx, Oop receiver) {
  const Oop cls = ctx.wk.classOf(receiver);
  if (!cls.isHeap()) {
    return Oop{};
  }
  return ctx.heap.slotAt(cls, kClassSlotFormat);
}

std::int64_t namedInstSize(CallContext& ctx, Oop receiver) {
  return Format::instSize(classFormat(ctx, receiver));
}

bool nameEquals(Heap& heap, Oop a, Oop b) {
  if (a == b) {
    return true;
  }
  if (!a.isHeap() || !b.isHeap()) {
    return false;
  }
  if ((heap.flags(a) & kFlagBytes) == 0 || (heap.flags(b) & kFlagBytes) == 0) {
    return false;
  }
  if (heap.size(a) != heap.size(b)) {
    return false;
  }
  return std::memcmp(heap.header(a) + 1, heap.header(b) + 1, heap.size(a)) == 0;
}

}  // namespace

Oop ao_Object_identityEquals(CallContext&, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return receiver == args[0] ? Oop::true_() : Oop::false_();
}

Oop ao_Object_identityNotEquals(CallContext&, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return receiver != args[0] ? Oop::true_() : Oop::false_();
}

Oop ao_Object_equals(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Object_identityEquals(ctx, receiver, args, argc);
}

Oop ao_Object_class(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return ctx.wk.classOf(receiver);
}

Oop ao_Object_identityHash(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  if (receiver.isSmallInteger()) {
    return receiver;
  }
  if (receiver.isCharacter()) {
    return Oop::fromSmallInteger(static_cast<std::int64_t>(receiver.characterValue()));
  }
  if (receiver.isNil()) {
    return Oop::fromSmallInteger(0);
  }
  if (receiver.isFalse()) {
    return Oop::fromSmallInteger(1);
  }
  if (receiver.isTrue()) {
    return Oop::fromSmallInteger(2);
  }
  if (receiver.isHeap()) {
    return Oop::fromSmallInteger(static_cast<std::int64_t>(ctx.heap.hash(receiver)));
  }
  return Oop::fromSmallInteger(0);
}

Oop ao_Object_hash(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Object_identityHash(ctx, receiver, args, argc);
}

Oop ao_Object_yourself(CallContext&, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return receiver;
}

Oop ao_Object_isNil(CallContext&, Oop, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::false_();
}

Oop ao_Object_notNil(CallContext&, Oop, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::true_();
}

Oop ao_Object_ifNil_(CallContext&, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return receiver;
}

Oop ao_Object_ifNotNil_(CallContext& ctx, Oop, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_Object_perform_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return send(ctx, receiver, args[0], nullptr, 0, nullptr);
}

Oop ao_Object_perform_with_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  return send(ctx, receiver, args[0], &args[1], 1, nullptr);
}

Oop ao_Object_perform_withArguments_(CallContext& ctx, Oop receiver, const Oop* args,
                                     std::uint32_t argc) {
  if (argc != 2) return Oop{};
  const Oop arr = args[1];
  if (!arr.isHeap() || (ctx.heap.flags(arr) & kFlagBytes) != 0) {
    return fail(ctx, receiver, "perform:withArguments: expects pointer slots");
  }
  const auto n = ctx.heap.size(arr);
  std::vector<Oop> unpacked(n);
  for (std::uint32_t i = 0; i < n; ++i) {
    unpacked[i] = ctx.heap.slotAt(arr, i);
  }
  return send(ctx, receiver, args[0], unpacked.data(), n, nullptr);
}

Oop ao_Object_doesNotUnderstand_(CallContext&, Oop, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return args[0];
}

Oop ao_Object_error_(CallContext&, Oop, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return args[0];
}

Oop ao_Object_subclassResponsibility(CallContext& ctx, Oop, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Str::fromUtf8(ctx.heap, ctx.wk, "subclassResponsibility");
}

Oop ao_Object_shouldNotImplement(CallContext& ctx, Oop, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Str::fromUtf8(ctx.heap, ctx.wk, "shouldNotImplement");
}

Oop ao_Object_isKindOf_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  Oop cls = ctx.wk.classOf(receiver);
  while (cls.isHeap()) {
    if (cls == args[0]) {
      return Oop::true_();
    }
    cls = ctx.heap.slotAt(cls, kClassSlotSuperclass);
  }
  return Oop::false_();
}

Oop ao_Object_isMemberOf_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return ctx.wk.classOf(receiver) == args[0] ? Oop::true_() : Oop::false_();
}

Oop ao_Object_respondsTo_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  const Oop meth = lookup(ctx.heap, ctx.wk.classOf(receiver), args[0]);
  return meth.isHeap() ? Oop::true_() : Oop::false_();
}

Oop ao_Object_shallowCopy(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  if (!receiver.isHeap()) {
    return receiver;
  }
  const auto n = ctx.heap.size(receiver);
  const auto flags = ctx.heap.flags(receiver);
  const Oop copy = ctx.heap.allocate(ctx.heap.klass(receiver), n, flags);
  if (!copy.isHeap()) {
    return Oop{};
  }
  if ((flags & kFlagBytes) != 0) {
    if (n != 0) {
      std::memcpy(ctx.heap.bytes(copy), ctx.heap.header(receiver) + 1, n);
    }
  } else {
    for (std::uint32_t i = 0; i < n; ++i) {
      ctx.heap.slotAtPut(copy, i, ctx.heap.slotAt(receiver, i));
    }
  }
  return copy;
}

Oop ao_Object_copy(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Object_shallowCopy(ctx, receiver, args, argc);
}

Oop ao_Object_instVarAt_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  if (!receiver.isHeap() || (ctx.heap.flags(receiver) & kFlagBytes) != 0 ||
      !args[0].isSmallInteger()) {
    return fail(ctx, receiver, "instVarAt: index out of range");
  }
  const auto idx = args[0].smallIntegerValue();
  const auto n = namedInstSize(ctx, receiver);
  if (idx < 1 || idx > n || static_cast<std::uint32_t>(idx) > ctx.heap.size(receiver)) {
    return fail(ctx, receiver, "instVarAt: index out of range");
  }
  return ctx.heap.slotAt(receiver, static_cast<std::uint32_t>(idx - 1));
}

Oop ao_Object_instVarAt_put_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  if (!receiver.isHeap() || (ctx.heap.flags(receiver) & kFlagBytes) != 0 ||
      !args[0].isSmallInteger()) {
    return fail(ctx, receiver, "instVarAt:put: index out of range");
  }
  const auto idx = args[0].smallIntegerValue();
  const auto n = namedInstSize(ctx, receiver);
  if (idx < 1 || idx > n || static_cast<std::uint32_t>(idx) > ctx.heap.size(receiver)) {
    return fail(ctx, receiver, "instVarAt:put: index out of range");
  }
  ctx.heap.slotAtPut(receiver, static_cast<std::uint32_t>(idx - 1), args[1]);
  return args[1];
}

Oop ao_Object_instVarNamed_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  std::vector<Oop> chain;
  Oop cls = ctx.wk.classOf(receiver);
  while (cls.isHeap()) {
    chain.push_back(cls);
    cls = ctx.heap.slotAt(cls, kClassSlotSuperclass);
  }
  std::int64_t index = 1;
  for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
    const Oop names = ctx.heap.slotAt(*it, kClassSlotInstVarNames);
    if (!names.isHeap() || (ctx.heap.flags(names) & kFlagBytes) != 0) {
      continue;
    }
    const auto n = ctx.heap.size(names);
    for (std::uint32_t i = 0; i < n; ++i) {
      if (nameEquals(ctx.heap, ctx.heap.slotAt(names, i), args[0])) {
        Oop idx = Oop::fromSmallInteger(index);
        return ao_Object_instVarAt_(ctx, receiver, &idx, 1);
      }
      ++index;
    }
  }
  return fail(ctx, receiver, "instVarNamed: not found");
}

Oop ao_Object_basicSize(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  if (!receiver.isHeap()) {
    return Oop::fromSmallInteger(0);
  }
  const Oop fmt = classFormat(ctx, receiver);
  if (!Format::isIndexable(fmt)) {
    return Oop::fromSmallInteger(0);
  }
  const auto n = static_cast<std::int64_t>(ctx.heap.size(receiver));
  if (Format::isBytes(fmt)) {
    return Oop::fromSmallInteger(n);
  }
  const auto named = Format::instSize(fmt);
  const auto var = n > named ? n - named : 0;
  return Oop::fromSmallInteger(var);
}

Oop ao_Object_basicAt_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  if (!receiver.isHeap() || !args[0].isSmallInteger()) {
    return fail(ctx, receiver, "basicAt: index out of range");
  }
  const Oop fmt = classFormat(ctx, receiver);
  if (!Format::isIndexable(fmt)) {
    return fail(ctx, receiver, "basicAt: index out of range");
  }
  const auto idx = args[0].smallIntegerValue();
  const auto n = static_cast<std::int64_t>(ctx.heap.size(receiver));
  if (Format::isBytes(fmt)) {
    if (idx < 1 || idx > n) {
      return fail(ctx, receiver, "basicAt: index out of range");
    }
    const auto* bytes = reinterpret_cast<const unsigned char*>(ctx.heap.header(receiver) + 1);
    return Oop::fromSmallInteger(bytes[static_cast<std::size_t>(idx - 1)]);
  }
  const auto named = Format::instSize(fmt);
  const auto slot = named + (idx - 1);
  if (idx < 1 || slot < 0 || slot >= n) {
    return fail(ctx, receiver, "basicAt: index out of range");
  }
  return ctx.heap.slotAt(receiver, static_cast<std::uint32_t>(slot));
}

Oop ao_Object_basicAt_put_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 2) return Oop{};
  if (!receiver.isHeap() || !args[0].isSmallInteger()) {
    return fail(ctx, receiver, "basicAt:put: index out of range");
  }
  const Oop fmt = classFormat(ctx, receiver);
  if (!Format::isIndexable(fmt)) {
    return fail(ctx, receiver, "basicAt:put: index out of range");
  }
  const auto idx = args[0].smallIntegerValue();
  const auto n = static_cast<std::int64_t>(ctx.heap.size(receiver));
  if (Format::isBytes(fmt)) {
    if (idx < 1 || idx > n || !args[1].isSmallInteger()) {
      return fail(ctx, receiver, "basicAt:put: index out of range");
    }
    const auto byte = args[1].smallIntegerValue();
    if (byte < 0 || byte > 255) {
      return fail(ctx, receiver, "basicAt:put: value out of range");
    }
    ctx.heap.bytes(receiver)[static_cast<std::size_t>(idx - 1)] =
        static_cast<std::byte>(static_cast<unsigned char>(byte));
    return args[1];
  }
  const auto named = Format::instSize(fmt);
  const auto slot = named + (idx - 1);
  if (idx < 1 || slot < 0 || slot >= n) {
    return fail(ctx, receiver, "basicAt:put: index out of range");
  }
  ctx.heap.slotAtPut(receiver, static_cast<std::uint32_t>(slot), args[1]);
  return args[1];
}

Oop ao_Object_printString(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  const Oop cls = ctx.wk.classOf(receiver);
  if (!cls.isHeap()) {
    return Str::fromUtf8(ctx.heap, ctx.wk, "");
  }
  const Oop name = ctx.heap.slotAt(cls, kClassSlotName);
  if (!name.isHeap()) {
    return Str::fromUtf8(ctx.heap, ctx.wk, "");
  }
  const auto n = ctx.heap.size(name);
  auto str = ctx.heap.allocate(ctx.wk.stringClass, n, kFlagBytes);
  if (!str.isHeap()) {
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(ctx.heap.bytes(str), ctx.heap.header(name) + 1, n);
  }
  return str;
}

Oop ao_Object_printOn_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  Oop str = ao_Object_printString(ctx, receiver, nullptr, 0);
  auto sel = Symbol::intern(ctx.wk, "nextPutAll:");
  return send(ctx, args[0], sel, &str, 1, nullptr);
}

Oop ao_Object_storeOn_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  return ao_Object_printOn_(ctx, receiver, args, argc);
}

Oop ao_Object_inspect(CallContext& ctx, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  if (ctx.inspectHook != nullptr) {
    ctx.inspectHook(ctx, receiver);
  }
  return receiver;
}

Oop ao_UndefinedObject_isNil(CallContext&, Oop, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::true_();
}

Oop ao_UndefinedObject_notNil(CallContext&, Oop, const Oop*, std::uint32_t argc) {
  if (argc != 0) return Oop{};
  return Oop::false_();
}

Oop ao_UndefinedObject_ifNil_(CallContext& ctx, Oop, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return sendValue(ctx, args[0]);
}

Oop ao_UndefinedObject_ifNotNil_(CallContext&, Oop receiver, const Oop*, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  return receiver;
}

namespace kernel {

void installObject(Heap& heap, WellKnown& wk) {
  const Oop cls = wk.objectClass;
  putNative(heap, wk, cls, "class", 0, "ao_Object_class", ao_Object_class);
  putNative(heap, wk, cls, "==", 1, "ao_Object_identityEquals", ao_Object_identityEquals);
  putNative(heap, wk, cls, "~~", 1, "ao_Object_identityNotEquals", ao_Object_identityNotEquals);
  putNative(heap, wk, cls, "=", 1, "ao_Object_equals", ao_Object_equals);
  putNative(heap, wk, cls, "hash", 0, "ao_Object_hash", ao_Object_hash);
  putNative(heap, wk, cls, "identityHash", 0, "ao_Object_identityHash", ao_Object_identityHash);
  putNative(heap, wk, cls, "yourself", 0, "ao_Object_yourself", ao_Object_yourself);
  putNative(heap, wk, cls, "isNil", 0, "ao_Object_isNil", ao_Object_isNil);
  putNative(heap, wk, cls, "notNil", 0, "ao_Object_notNil", ao_Object_notNil);
  putNative(heap, wk, cls, "ifNil:", 1, "ao_Object_ifNil_", ao_Object_ifNil_);
  putNative(heap, wk, cls, "ifNotNil:", 1, "ao_Object_ifNotNil_", ao_Object_ifNotNil_);
  putNative(heap, wk, cls, "perform:", 1, "ao_Object_perform_", ao_Object_perform_);
  putNative(heap, wk, cls, "perform:with:", 2, "ao_Object_perform_with_", ao_Object_perform_with_);
  putNative(heap, wk, cls, "perform:withArguments:", 2, "ao_Object_perform_withArguments_",
            ao_Object_perform_withArguments_);
  putNative(heap, wk, cls, "doesNotUnderstand:", 1, "ao_Object_doesNotUnderstand_",
            ao_Object_doesNotUnderstand_);
  putNative(heap, wk, cls, "error:", 1, "ao_Object_error_", ao_Object_error_);
  putNative(heap, wk, cls, "subclassResponsibility", 0, "ao_Object_subclassResponsibility",
            ao_Object_subclassResponsibility);
  putNative(heap, wk, cls, "shouldNotImplement", 0, "ao_Object_shouldNotImplement",
            ao_Object_shouldNotImplement);
  putNative(heap, wk, cls, "isKindOf:", 1, "ao_Object_isKindOf_", ao_Object_isKindOf_);
  putNative(heap, wk, cls, "isMemberOf:", 1, "ao_Object_isMemberOf_", ao_Object_isMemberOf_);
  putNative(heap, wk, cls, "respondsTo:", 1, "ao_Object_respondsTo_", ao_Object_respondsTo_);
  putNative(heap, wk, cls, "copy", 0, "ao_Object_copy", ao_Object_copy);
  putNative(heap, wk, cls, "shallowCopy", 0, "ao_Object_shallowCopy", ao_Object_shallowCopy);
  putNative(heap, wk, cls, "instVarAt:", 1, "ao_Object_instVarAt_", ao_Object_instVarAt_);
  putNative(heap, wk, cls, "instVarAt:put:", 2, "ao_Object_instVarAt_put_",
            ao_Object_instVarAt_put_);
  putNative(heap, wk, cls, "instVarNamed:", 1, "ao_Object_instVarNamed_", ao_Object_instVarNamed_);
  putNative(heap, wk, cls, "basicSize", 0, "ao_Object_basicSize", ao_Object_basicSize);
  putNative(heap, wk, cls, "basicAt:", 1, "ao_Object_basicAt_", ao_Object_basicAt_);
  putNative(heap, wk, cls, "basicAt:put:", 2, "ao_Object_basicAt_put_", ao_Object_basicAt_put_);
  putNative(heap, wk, cls, "printString", 0, "ao_Object_printString", ao_Object_printString);
  putNative(heap, wk, cls, "printOn:", 1, "ao_Object_printOn_", ao_Object_printOn_);
  putNative(heap, wk, cls, "storeOn:", 1, "ao_Object_storeOn_", ao_Object_storeOn_);
  putNative(heap, wk, cls, "inspect", 0, "ao_Object_inspect", ao_Object_inspect);

  const Oop undef = wk.undefinedObjectClass;
  putNative(heap, wk, undef, "isNil", 0, "ao_UndefinedObject_isNil", ao_UndefinedObject_isNil);
  putNative(heap, wk, undef, "notNil", 0, "ao_UndefinedObject_notNil", ao_UndefinedObject_notNil);
  putNative(heap, wk, undef, "ifNil:", 1, "ao_UndefinedObject_ifNil_", ao_UndefinedObject_ifNil_);
  putNative(heap, wk, undef, "ifNotNil:", 1, "ao_UndefinedObject_ifNotNil_",
            ao_UndefinedObject_ifNotNil_);
}

}  // namespace kernel
}  // namespace ao

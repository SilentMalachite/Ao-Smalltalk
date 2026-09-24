#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

namespace ao {
namespace {

constexpr std::uint32_t kHashedTally = 0;
constexpr std::uint32_t kHashedArray = 1;
constexpr std::uint32_t kOcArray = 0;
constexpr std::uint32_t kOcFirst = 1;
constexpr std::uint32_t kOcLast = 2;
constexpr std::uint32_t kIvStart = 0;
constexpr std::uint32_t kIvStop = 1;
constexpr std::uint32_t kIvStep = 2;
constexpr std::uint32_t kDefaultCap = 8;

Oop selEquals(WellKnown& wk) { return wk.intern("="); }
Oop selHash(WellKnown& wk) { return wk.intern("hash"); }

void consumeHash(CallContext& ctx, Root& key) {
  send(ctx, key.slot, selHash(ctx.wk), nullptr, 0, nullptr);
}

bool keysMatch(CallContext& ctx, Root& search, Oop candidate, bool identity) {
  if (search.slot == candidate) {
    return true;
  }
  if (identity || candidate.isNil()) {
    return false;
  }
  Root cand(ctx.roots, candidate);
  const Oop eq = send(ctx, search.slot, selEquals(ctx.wk), &cand.slot, 1, nullptr);
  return eq.isTrue();
}

std::int64_t tallyOf(Heap& heap, Oop hashed) {
  const Oop t = heap.slotAt(hashed, kHashedTally);
  return t.isSmallInteger() ? t.smallIntegerValue() : 0;
}

// error: の慣習どおりメッセージ文字列で失敗する。receiver はルート済みスロット（GC しても正しい）。
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

bool ensureInner(CallContext& ctx, Root& hashed) {
  Oop inner = ctx.heap.slotAt(hashed.slot, kHashedArray);
  if (inner.isHeap()) {
    return true;
  }
  Oop arr = allocateRetry(ctx, ctx.wk.arrayClass, kDefaultCap, 0);
  if (!arr.isHeap()) {
    return false;
  }
  ctx.heap.slotAtPut(hashed.slot, kHashedArray, arr);
  if (!ctx.heap.slotAt(hashed.slot, kHashedTally).isSmallInteger()) {
    ctx.heap.slotAtPut(hashed.slot, kHashedTally, Oop::fromSmallInteger(0));
  }
  return true;
}

bool growInner(CallContext& ctx, Root& hashed) {
  Root old(ctx.roots, ctx.heap.slotAt(hashed.slot, kHashedArray));
  const std::uint32_t n = old.slot.isHeap() ? ctx.heap.size(old.slot) : 0;
  const std::uint32_t next = n == 0 ? kDefaultCap : n * 2;
  Oop grown = allocateRetry(ctx, ctx.wk.arrayClass, next, 0);
  if (!grown.isHeap()) {
    return false;
  }
  if (old.slot.isHeap()) {
    for (std::uint32_t i = 0; i < n; ++i) {
      ctx.heap.slotAtPut(grown, i, ctx.heap.slotAt(old.slot, i));
    }
  }
  ctx.heap.slotAtPut(hashed.slot, kHashedArray, grown);
  return true;
}

// UINT32_MAX when the key is absent, and also when hash or = started an unwind (SPEC §3.4). The
// caller tells the two apart with unwinding(ctx) and then returns the empty OOP.
std::uint32_t findPair(CallContext& ctx, Root& dict, Root& key, bool identity) {
  consumeHash(ctx, key);
  if (unwinding(ctx) || !ensureInner(ctx, dict)) {
    return UINT32_MAX;
  }
  const Oop inner0 = ctx.heap.slotAt(dict.slot, kHashedArray);
  if (!inner0.isHeap()) {
    return UINT32_MAX;
  }
  const std::uint32_t n = ctx.heap.size(inner0);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
    const Oop cand = ctx.heap.slotAt(inner, i);
    if (cand.isNil()) {
      continue;
    }
    if (keysMatch(ctx, key, cand, identity)) {
      return i;
    }
    if (unwinding(ctx)) {
      return UINT32_MAX;
    }
  }
  return UINT32_MAX;
}

// Like findPair: UINT32_MAX also when hash or = started an unwind.
std::uint32_t findValue(CallContext& ctx, Root& set, Root& value, bool identity) {
  consumeHash(ctx, value);
  if (unwinding(ctx) || !ensureInner(ctx, set)) {
    return UINT32_MAX;
  }
  const Oop inner0 = ctx.heap.slotAt(set.slot, kHashedArray);
  if (!inner0.isHeap()) {
    return UINT32_MAX;
  }
  const std::uint32_t n = ctx.heap.size(inner0);
  for (std::uint32_t i = 0; i < n; ++i) {
    const Oop inner = ctx.heap.slotAt(set.slot, kHashedArray);
    const Oop cand = ctx.heap.slotAt(inner, i);
    if (cand.isNil()) {
      continue;
    }
    if (keysMatch(ctx, value, cand, identity)) {
      return i;
    }
    if (unwinding(ctx)) {
      return UINT32_MAX;
    }
  }
  return UINT32_MAX;
}

Oop hashedNew(CallContext& ctx, Oop receiver) {
  Root cls(ctx.roots, receiver);
  Oop obj = send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr);
  Root o(ctx.roots, obj);
  if (!o.slot.isHeap()) {
    return o.slot;
  }
  Oop arr = allocateRetry(ctx, ctx.wk.arrayClass, kDefaultCap, 0);
  if (!arr.isHeap()) {
    return o.slot;
  }
  ctx.heap.slotAtPut(o.slot, kHashedTally, Oop::fromSmallInteger(0));
  ctx.heap.slotAtPut(o.slot, kHashedArray, arr);
  return o.slot;
}

Oop hashedSize(CallContext& ctx, Oop receiver, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  return Oop::fromSmallInteger(tallyOf(ctx.heap, receiver));
}

Oop dictAt(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
           bool identity) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  const auto i = findPair(ctx, dict, key, identity);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (i == UINT32_MAX) {
    return Oop::nil();
  }
  const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
  return ctx.heap.slotAt(inner, i + 1);
}

Oop dictAtPut(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
              bool identity) {
  if (argc != 2 || !receiver.isHeap()) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  Root value(ctx.roots, args[1]);
  const auto found = findPair(ctx, dict, key, identity);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (found != UINT32_MAX) {
    const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
    ctx.heap.slotAtPut(inner, found + 1, value.slot);
    return value.slot;
  }
  if (!ensureInner(ctx, dict)) {
    return Oop{};
  }
  auto n = ctx.heap.size(ctx.heap.slotAt(dict.slot, kHashedArray));
  auto tally = tallyOf(ctx.heap, dict.slot);
  // tally は Smalltalk から書き換えられる。+1 が SmallInteger を超えるなら、伸ばす前に失敗する。
  if (tally >= kSmiMax) {
    return fail(ctx, dict.slot, "at:put: tally out of range");
  }
  if (tally * 2 >= static_cast<std::int64_t>(n)) {
    if (!growInner(ctx, dict)) {
      return Oop{};
    }
    n = ctx.heap.size(ctx.heap.slotAt(dict.slot, kHashedArray));
  }
  const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (ctx.heap.slotAt(inner, i).isNil()) {
      ctx.heap.slotAtPut(inner, i, key.slot);
      ctx.heap.slotAtPut(inner, i + 1, value.slot);
      ctx.heap.slotAtPut(dict.slot, kHashedTally, Oop::fromSmallInteger(tally + 1));
      return value.slot;
    }
  }
  return value.slot;
}

Oop dictIncludesKey(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                    bool identity) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  const auto i = findPair(ctx, dict, key, identity);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return i == UINT32_MAX ? Oop::false_() : Oop::true_();
}

Oop setAdd(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
           bool identity) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root set(ctx.roots, receiver);
  Root value(ctx.roots, args[0]);
  const auto found = findValue(ctx, set, value, identity);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (found != UINT32_MAX) {
    return value.slot;
  }
  if (!ensureInner(ctx, set)) {
    return Oop{};
  }
  auto n = ctx.heap.size(ctx.heap.slotAt(set.slot, kHashedArray));
  auto tally = tallyOf(ctx.heap, set.slot);
  // tally は Smalltalk から書き換えられる。+1 が SmallInteger を超えるなら、伸ばす前に失敗する。
  if (tally >= kSmiMax) {
    return fail(ctx, set.slot, "add: tally out of range");
  }
  if (tally >= static_cast<std::int64_t>(n)) {
    if (!growInner(ctx, set)) {
      return Oop{};
    }
    n = ctx.heap.size(ctx.heap.slotAt(set.slot, kHashedArray));
  }
  const Oop inner = ctx.heap.slotAt(set.slot, kHashedArray);
  for (std::uint32_t i = 0; i < n; ++i) {
    if (ctx.heap.slotAt(inner, i).isNil()) {
      ctx.heap.slotAtPut(inner, i, value.slot);
      ctx.heap.slotAtPut(set.slot, kHashedTally, Oop::fromSmallInteger(tally + 1));
      return value.slot;
    }
  }
  return value.slot;
}

Oop setIncludes(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                bool identity) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root set(ctx.roots, receiver);
  Root value(ctx.roots, args[0]);
  const auto i = findValue(ctx, set, value, identity);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return i == UINT32_MAX ? Oop::false_() : Oop::true_();
}

std::int64_t ocSize(Heap& heap, Oop oc) {
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!first.isSmallInteger() || !last.isSmallInteger()) {
    return 0;
  }
  const auto f = first.smallIntegerValue();
  const auto l = last.smallIntegerValue();
  if (l < f) {
    return 0;
  }
  // first と last は Smalltalk から書き換えられる。両端だと l - f + 1 は int64 も超えるので、
  // kSmiMax + 1 で頭打ちにする（size はこれを範囲外として失敗する）。
  const std::int64_t span = l - f;
  return span >= kSmiMax ? kSmiMax + 1 : span + 1;
}

bool ocEnsure(CallContext& ctx, Root& oc) {
  Oop arr = ctx.heap.slotAt(oc.slot, kOcArray);
  if (arr.isHeap()) {
    return true;
  }
  arr = allocateRetry(ctx, ctx.wk.arrayClass, kDefaultCap, 0);
  if (!arr.isHeap()) {
    return false;
  }
  ctx.heap.slotAtPut(oc.slot, kOcArray, arr);
  if (!ctx.heap.slotAt(oc.slot, kOcFirst).isSmallInteger()) {
    ctx.heap.slotAtPut(oc.slot, kOcFirst, Oop::fromSmallInteger(1));
  }
  if (!ctx.heap.slotAt(oc.slot, kOcLast).isSmallInteger()) {
    ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(0));
  }
  return true;
}

bool ocGrow(CallContext& ctx, Root& oc) {
  Root old(ctx.roots, ctx.heap.slotAt(oc.slot, kOcArray));
  const auto first = ctx.heap.slotAt(oc.slot, kOcFirst).smallIntegerValue();
  const auto last = ctx.heap.slotAt(oc.slot, kOcLast).smallIntegerValue();
  const auto used = last < first ? 0 : last - first + 1;
  const std::uint32_t n = old.slot.isHeap() ? ctx.heap.size(old.slot) : 0;
  std::uint32_t next = n == 0 ? kDefaultCap : n * 2;
  if (next < static_cast<std::uint32_t>(used) + 1) {
    next = static_cast<std::uint32_t>(used) + 1;
  }
  Oop grown = allocateRetry(ctx, ctx.wk.arrayClass, next, 0);
  if (!grown.isHeap()) {
    return false;
  }
  if (old.slot.isHeap() && used > 0) {
    for (std::int64_t i = 0; i < used; ++i) {
      ctx.heap.slotAtPut(grown, static_cast<std::uint32_t>(i),
                         ctx.heap.slotAt(old.slot, static_cast<std::uint32_t>(first - 1 + i)));
    }
  }
  ctx.heap.slotAtPut(oc.slot, kOcArray, grown);
  ctx.heap.slotAtPut(oc.slot, kOcFirst, Oop::fromSmallInteger(1));
  ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(used));
  return true;
}

}  // namespace

Oop ao_Dictionary_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return hashedNew(ctx, receiver);
}

Oop ao_Dictionary_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  return hashedSize(ctx, receiver, argc);
}

Oop ao_Dictionary_at_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return dictAt(ctx, receiver, args, argc, false);
}

Oop ao_Dictionary_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  return dictAtPut(ctx, receiver, args, argc, false);
}

Oop ao_Dictionary_includesKey_(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  return dictIncludesKey(ctx, receiver, args, argc, false);
}

Oop ao_IdentityDictionary_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  return dictAt(ctx, receiver, args, argc, true);
}

Oop ao_IdentityDictionary_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc) {
  return dictAtPut(ctx, receiver, args, argc, true);
}

Oop ao_IdentityDictionary_includesKey_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                       std::uint32_t argc) {
  return dictIncludesKey(ctx, receiver, args, argc, true);
}

Oop ao_Dictionary_includes_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root needle(ctx.roots, args[0]);
  consumeHash(ctx, needle);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (!ensureInner(ctx, dict)) {
    return Oop::false_();
  }
  const Oop inner0 = ctx.heap.slotAt(dict.slot, kHashedArray);
  const std::uint32_t n = inner0.isHeap() ? ctx.heap.size(inner0) : 0;
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
    if (ctx.heap.slotAt(inner, i).isNil()) {
      continue;
    }
    const Oop val = ctx.heap.slotAt(inner, i + 1);
    if (keysMatch(ctx, needle, val, false)) {
      return Oop::true_();
    }
    if (unwinding(ctx)) {
      return Oop{};
    }
  }
  return Oop::false_();
}

Oop ao_Dictionary_do_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  if (!ensureInner(ctx, dict)) {
    return dict.slot;
  }
  const Oop inner0 = ctx.heap.slotAt(dict.slot, kHashedArray);
  const std::uint32_t n = inner0.isHeap() ? ctx.heap.size(inner0) : 0;
  Root assoc(ctx.roots);
  Root key(ctx.roots);
  Root value(ctx.roots);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
    key.slot = ctx.heap.slotAt(inner, i);
    if (key.slot.isNil()) {
      continue;
    }
    value.slot = ctx.heap.slotAt(inner, i + 1);
    assoc.slot = allocateRetry(ctx, ctx.wk.associationClass, 2, 0);
    if (!assoc.slot.isHeap()) {
      return dict.slot;
    }
    ctx.heap.slotAtPut(assoc.slot, kAssocKey, key.slot);
    ctx.heap.slotAtPut(assoc.slot, kAssocValue, value.slot);
    Oop ignored;
    if (!callBlock(ctx, blk.slot, &assoc.slot, 1, &ignored)) {
      return Oop{};
    }
  }
  return dict.slot;
}

Oop ao_Dictionary_collect_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  Oop nOop = Oop::fromSmallInteger(tallyOf(ctx.heap, dict.slot));
  Root arr(ctx.roots, send(ctx, ctx.wk.arrayClass, ctx.wk.selBasicNew_, &nOop, 1, nullptr));
  if (!arr.slot.isHeap()) {
    return arr.slot;
  }
  if (!ensureInner(ctx, dict)) {
    return arr.slot;
  }
  const Oop inner0 = ctx.heap.slotAt(dict.slot, kHashedArray);
  const std::uint32_t n = inner0.isHeap() ? ctx.heap.size(inner0) : 0;
  std::int64_t idx = 1;
  Root val(ctx.roots);
  Root mapped(ctx.roots);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop inner = ctx.heap.slotAt(dict.slot, kHashedArray);
    if (ctx.heap.slotAt(inner, i).isNil()) {
      continue;
    }
    val.slot = ctx.heap.slotAt(inner, i + 1);
    if (!callBlock(ctx, blk.slot, &val.slot, 1, &mapped.slot)) {
      return Oop{};
    }
    Oop put[2] = {Oop::fromSmallInteger(idx), mapped.slot};
    send(ctx, arr.slot, ctx.wk.selAt_put_, put, 2, nullptr);
    if (unwinding(ctx)) {
      return Oop{};
    }
    ++idx;
  }
  return arr.slot;
}

Oop ao_Set_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return hashedNew(ctx, receiver);
}

Oop ao_Set_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  return hashedSize(ctx, receiver, argc);
}

Oop ao_Set_add_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return setAdd(ctx, receiver, args, argc, false);
}

Oop ao_Set_includes_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return setIncludes(ctx, receiver, args, argc, false);
}

Oop ao_IdentitySet_add_(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  return setAdd(ctx, receiver, args, argc, true);
}

Oop ao_IdentitySet_includes_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  return setIncludes(ctx, receiver, args, argc, true);
}

Oop ao_Set_do_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root set(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  if (!ensureInner(ctx, set)) {
    return set.slot;
  }
  const Oop inner0 = ctx.heap.slotAt(set.slot, kHashedArray);
  const std::uint32_t n = inner0.isHeap() ? ctx.heap.size(inner0) : 0;
  Root elt(ctx.roots);
  for (std::uint32_t i = 0; i < n; ++i) {
    const Oop inner = ctx.heap.slotAt(set.slot, kHashedArray);
    elt.slot = ctx.heap.slotAt(inner, i);
    if (elt.slot.isNil()) {
      continue;
    }
    Oop ignored;
    if (!callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
      return Oop{};
    }
  }
  return set.slot;
}

Oop ao_OrderedCollection_new(CallContext& ctx, const Oop& receiver, const Oop*,
                             std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Oop obj = send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr);
  Root oc(ctx.roots, obj);
  if (!oc.slot.isHeap()) {
    return oc.slot;
  }
  Oop arr = allocateRetry(ctx, ctx.wk.arrayClass, kDefaultCap, 0);
  if (!arr.isHeap()) {
    return oc.slot;
  }
  ctx.heap.slotAtPut(oc.slot, kOcArray, arr);
  ctx.heap.slotAtPut(oc.slot, kOcFirst, Oop::fromSmallInteger(1));
  ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(0));
  return oc.slot;
}

Oop ao_OrderedCollection_size(CallContext& ctx, const Oop& receiver, const Oop*,
                              std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  const auto n = ocSize(ctx.heap, receiver);
  if (n > kSmiMax) {
    return fail(ctx, receiver, "size out of range");
  }
  return Oop::fromSmallInteger(n);
}

Oop ao_OrderedCollection_add_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root oc(ctx.roots, receiver);
  Root value(ctx.roots, args[0]);
  if (!ocEnsure(ctx, oc)) {
    return Oop{};
  }
  const Oop arr = ctx.heap.slotAt(oc.slot, kOcArray);
  const auto n = static_cast<std::int64_t>(ctx.heap.size(arr));
  auto last = ctx.heap.slotAt(oc.slot, kOcLast).smallIntegerValue();
  if (last >= n) {
    if (!ocGrow(ctx, oc)) {
      return Oop{};
    }
    last = ctx.heap.slotAt(oc.slot, kOcLast).smallIntegerValue();
  }
  last += 1;
  ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(last));
  ctx.heap.slotAtPut(ctx.heap.slotAt(oc.slot, kOcArray), static_cast<std::uint32_t>(last - 1),
                     value.slot);
  return value.slot;
}

Oop ao_OrderedCollection_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap() || !args[0].isSmallInteger()) {
    return Oop{};
  }
  const auto index = args[0].smallIntegerValue();
  const auto n = ocSize(ctx.heap, receiver);
  if (index < 1 || index > n) {
    return Oop::nil();
  }
  const auto first = ctx.heap.slotAt(receiver, kOcFirst).smallIntegerValue();
  const Oop arr = ctx.heap.slotAt(receiver, kOcArray);
  return ctx.heap.slotAt(arr, static_cast<std::uint32_t>(first + index - 2));
}

Oop ao_OrderedCollection_do_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root oc(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  if (!ocEnsure(ctx, oc)) {
    return oc.slot;
  }
  const auto first = ctx.heap.slotAt(oc.slot, kOcFirst).smallIntegerValue();
  const auto last = ctx.heap.slotAt(oc.slot, kOcLast).smallIntegerValue();
  Root elt(ctx.roots);
  for (std::int64_t i = first; i <= last; ++i) {
    const Oop arr = ctx.heap.slotAt(oc.slot, kOcArray);
    elt.slot = ctx.heap.slotAt(arr, static_cast<std::uint32_t>(i - 1));
    Oop ignored;
    if (!callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
      return Oop{};
    }
  }
  return oc.slot;
}

Oop ao_Association_key_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  Root value(ctx.roots, args[1]);
  // SPEC §3.6: a subclass that adds variables gets them. May GC: key and value are read from roots.
  Oop a = allocateInstance(ctx, cls.slot, 2);
  if (!a.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(a, kAssocKey, key.slot);
  ctx.heap.slotAtPut(a, kAssocValue, value.slot);
  return a;
}

Oop ao_Association_key(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kAssocKey);
}

Oop ao_Association_value(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kAssocValue);
}

Oop ao_Association_key_(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(receiver, kAssocKey, args[0]);
  return receiver;
}

Oop ao_Association_value_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(receiver, kAssocValue, args[0]);
  return receiver;
}

Oop ao_Interval_from_to_by_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 3) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Root start(ctx.roots, args[0]);
  Root stop(ctx.roots, args[1]);
  Root step(ctx.roots, args[2]);
  // SPEC §3.6: a subclass that adds variables gets them. May GC: the bounds are read from roots.
  Oop iv = allocateInstance(ctx, cls.slot, 3);
  if (!iv.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(iv, kIvStart, start.slot);
  ctx.heap.slotAtPut(iv, kIvStop, stop.slot);
  ctx.heap.slotAtPut(iv, kIvStep, step.slot);
  return iv;
}

static Oop intervalSizeSmi(std::int64_t start, std::int64_t stop, std::int64_t step) {
  if (step == 0) {
    return Oop::fromSmallInteger(0);
  }
  __int128 diff;
  __int128 st = step;
  if (step > 0) {
    if (start > stop) {
      return Oop::fromSmallInteger(0);
    }
    diff = static_cast<__int128>(stop) - start;
  } else {
    if (start < stop) {
      return Oop::fromSmallInteger(0);
    }
    diff = static_cast<__int128>(start) - stop;
    st = -st;
  }
  const __int128 n = diff / st + 1;
  if (n >= kSmiMin && n <= kSmiMax) {
    return Oop::fromSmallInteger(static_cast<std::int64_t>(n));
  }
  return Oop{};
}

Oop ao_Interval_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  const Oop start = ctx.heap.slotAt(receiver, kIvStart);
  const Oop stop = ctx.heap.slotAt(receiver, kIvStop);
  const Oop step = ctx.heap.slotAt(receiver, kIvStep);
  if (start.isSmallInteger() && stop.isSmallInteger() && step.isSmallInteger()) {
    const Oop n = intervalSizeSmi(start.smallIntegerValue(), stop.smallIntegerValue(),
                                  step.smallIntegerValue());
    if (!n.isEmpty()) {
      return n;
    }
  }
  Root rcvr(ctx.roots, receiver);
  Root cur(ctx.roots, start);
  Root blkStop(ctx.roots, stop);
  Root blkStep(ctx.roots, step);
  if (step.isSmallInteger() && step.smallIntegerValue() == 0) {
    return Oop::fromSmallInteger(0);
  }
  std::int64_t count = 0;
  const bool forward = !step.isSmallInteger() || step.smallIntegerValue() > 0;
  // セレクタは send をまたいで使う。full GC の圧縮で Symbol も動くので、ルートに載せる。
  Root cmpSel(ctx.roots, ctx.wk.intern(forward ? ">" : "<"));
  Root add(ctx.roots, ctx.wk.intern("+"));
  for (;;) {
    const Oop past = send(ctx, cur.slot, cmpSel.slot, &blkStop.slot, 1, nullptr);
    if (unwinding(ctx)) {
      return Oop{};
    }
    if (past.isTrue()) {
      break;
    }
    ++count;
    cur.slot = send(ctx, cur.slot, add.slot, &blkStep.slot, 1, nullptr);
    if (unwinding(ctx)) {
      return Oop{};
    }
    if (count > (std::int64_t{1} << 20)) {
      break;
    }
  }
  return Oop::fromSmallInteger(count);
}

Oop ao_Interval_do_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root iv(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  const Oop start = ctx.heap.slotAt(iv.slot, kIvStart);
  const Oop stop = ctx.heap.slotAt(iv.slot, kIvStop);
  const Oop step = ctx.heap.slotAt(iv.slot, kIvStep);
  if (start.isSmallInteger() && stop.isSmallInteger() && step.isSmallInteger()) {
    const auto st = step.smallIntegerValue();
    if (st == 0) {
      return iv.slot;
    }
    Root elt(ctx.roots);
    if (st > 0) {
      for (__int128 i = start.smallIntegerValue(); i <= stop.smallIntegerValue(); i += st) {
        if (i < kSmiMin || i > kSmiMax) {
          break;
        }
        elt.slot = Oop::fromSmallInteger(static_cast<std::int64_t>(i));
        Oop ignored;
        if (!callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
          return Oop{};
        }
      }
    } else {
      for (__int128 i = start.smallIntegerValue(); i >= stop.smallIntegerValue(); i += st) {
        if (i < kSmiMin || i > kSmiMax) {
          break;
        }
        elt.slot = Oop::fromSmallInteger(static_cast<std::int64_t>(i));
        Oop ignored;
        if (!callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
          return Oop{};
        }
      }
    }
    return iv.slot;
  }
  if (step.isSmallInteger() && step.smallIntegerValue() == 0) {
    return iv.slot;
  }
  Root cur(ctx.roots, start);
  Root blkStop(ctx.roots, stop);
  Root blkStep(ctx.roots, step);
  const bool forward = !step.isSmallInteger() || step.smallIntegerValue() > 0;
  // セレクタは send をまたいで使う。full GC の圧縮で Symbol も動くので、ルートに載せる。
  Root cmpSel(ctx.roots, ctx.wk.intern(forward ? ">" : "<"));
  Root add(ctx.roots, ctx.wk.intern("+"));
  for (std::int64_t n = 0; n <= (std::int64_t{1} << 20); ++n) {
    const Oop past = send(ctx, cur.slot, cmpSel.slot, &blkStop.slot, 1, nullptr);
    if (unwinding(ctx)) {
      return Oop{};
    }
    if (past.isTrue()) {
      break;
    }
    Oop ignored;
    if (!callBlock(ctx, blk.slot, &cur.slot, 1, &ignored)) {
      return Oop{};
    }
    cur.slot = send(ctx, cur.slot, add.slot, &blkStep.slot, 1, nullptr);
    if (unwinding(ctx)) {
      return Oop{};
    }
  }
  return iv.slot;
}

Oop ao_Bag_do_(CallContext&, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return receiver;
}

Oop ao_Bag_size(CallContext&, const Oop&, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return Oop::fromSmallInteger(0);
}

Oop ao_Bag_add_(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return ao_Object_subclassResponsibility(ctx, receiver, nullptr, 0);
}

namespace kernel {

void installDictionary(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.dictionaryMetaclass, "new", 0, "ao_Dictionary_new", ao_Dictionary_new);
  putNative(heap, wk, wk.dictionaryClass, "size", 0, "ao_Dictionary_size", ao_Dictionary_size);
  putNative(heap, wk, wk.dictionaryClass, "at:", 1, "ao_Dictionary_at_", ao_Dictionary_at_);
  putNative(heap, wk, wk.dictionaryClass, "at:put:", 2, "ao_Dictionary_at_put_",
            ao_Dictionary_at_put_);
  putNative(heap, wk, wk.dictionaryClass, "includesKey:", 1, "ao_Dictionary_includesKey_",
            ao_Dictionary_includesKey_);
  putNative(heap, wk, wk.dictionaryClass, "includes:", 1, "ao_Dictionary_includes_",
            ao_Dictionary_includes_);
  putNative(heap, wk, wk.dictionaryClass, "do:", 1, "ao_Dictionary_do_", ao_Dictionary_do_);
  putNative(heap, wk, wk.dictionaryClass, "collect:", 1, "ao_Dictionary_collect_",
            ao_Dictionary_collect_);

  putNative(heap, wk, wk.identityDictionaryClass, "at:", 1, "ao_IdentityDictionary_at_",
            ao_IdentityDictionary_at_);
  putNative(heap, wk, wk.identityDictionaryClass, "at:put:", 2, "ao_IdentityDictionary_at_put_",
            ao_IdentityDictionary_at_put_);
  putNative(heap, wk, wk.identityDictionaryClass, "includesKey:", 1,
            "ao_IdentityDictionary_includesKey_", ao_IdentityDictionary_includesKey_);

  putNative(heap, wk, wk.setMetaclass, "new", 0, "ao_Set_new", ao_Set_new);
  putNative(heap, wk, wk.setClass, "size", 0, "ao_Set_size", ao_Set_size);
  putNative(heap, wk, wk.setClass, "add:", 1, "ao_Set_add_", ao_Set_add_);
  putNative(heap, wk, wk.setClass, "includes:", 1, "ao_Set_includes_", ao_Set_includes_);
  putNative(heap, wk, wk.setClass, "do:", 1, "ao_Set_do_", ao_Set_do_);

  putNative(heap, wk, wk.identitySetClass, "add:", 1, "ao_IdentitySet_add_", ao_IdentitySet_add_);
  putNative(heap, wk, wk.identitySetClass, "includes:", 1, "ao_IdentitySet_includes_",
            ao_IdentitySet_includes_);

  putNative(heap, wk, wk.orderedCollectionMetaclass, "new", 0, "ao_OrderedCollection_new",
            ao_OrderedCollection_new);
  putNative(heap, wk, wk.orderedCollectionClass, "size", 0, "ao_OrderedCollection_size",
            ao_OrderedCollection_size);
  putNative(heap, wk, wk.orderedCollectionClass, "add:", 1, "ao_OrderedCollection_add_",
            ao_OrderedCollection_add_);
  putNative(heap, wk, wk.orderedCollectionClass, "at:", 1, "ao_OrderedCollection_at_",
            ao_OrderedCollection_at_);
  putNative(heap, wk, wk.orderedCollectionClass, "do:", 1, "ao_OrderedCollection_do_",
            ao_OrderedCollection_do_);

  putNative(heap, wk, wk.associationMetaclass, "key:value:", 2, "ao_Association_key_value_",
            ao_Association_key_value_);
  putNative(heap, wk, wk.associationClass, "key", 0, "ao_Association_key", ao_Association_key);
  putNative(heap, wk, wk.associationClass, "value", 0, "ao_Association_value", ao_Association_value);
  putNative(heap, wk, wk.associationClass, "key:", 1, "ao_Association_key_", ao_Association_key_);
  putNative(heap, wk, wk.associationClass, "value:", 1, "ao_Association_value_",
            ao_Association_value_);

  putNative(heap, wk, wk.intervalMetaclass, "from:to:by:", 3, "ao_Interval_from_to_by_",
            ao_Interval_from_to_by_);
  putNative(heap, wk, wk.intervalClass, "size", 0, "ao_Interval_size", ao_Interval_size);
  putNative(heap, wk, wk.intervalClass, "do:", 1, "ao_Interval_do_", ao_Interval_do_);

  struct Stub {
    Oop cls;
    const char* doName;
    const char* sizeName;
    const char* addName;
  };
  const Stub stubs[] = {
      {wk.bagClass, "ao_Bag_do_", "ao_Bag_size", "ao_Bag_add_"},
      {wk.linkedListClass, "ao_LinkedList_do_", "ao_LinkedList_size", "ao_LinkedList_add_"},
      {wk.mappedCollectionClass, "ao_MappedCollection_do_", "ao_MappedCollection_size",
       "ao_MappedCollection_add_"},
  };
  for (const auto& s : stubs) {
    putNative(heap, wk, s.cls, "do:", 1, s.doName, ao_Bag_do_);
    putNative(heap, wk, s.cls, "size", 0, s.sizeName, ao_Bag_size);
    putNative(heap, wk, s.cls, "add:", 1, s.addName, ao_Bag_add_);
  }
}

}  // namespace kernel
}  // namespace ao

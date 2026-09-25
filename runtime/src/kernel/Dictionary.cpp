#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/HashedCollection.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <algorithm>

namespace ao {
namespace {

constexpr std::uint32_t kOcArray = 0;
constexpr std::uint32_t kOcFirst = 1;
constexpr std::uint32_t kOcLast = 2;
constexpr std::uint32_t kIvStart = 0;
constexpr std::uint32_t kIvStop = 1;
constexpr std::uint32_t kIvStep = 2;
constexpr std::uint32_t kDefaultCap = 8;
constexpr std::uint32_t kDictWidth = Hashed::kDictionaryWidth;
constexpr std::uint32_t kSetWidth = Hashed::kSetWidth;

// error: の慣習どおりメッセージ文字列で失敗する。receiver はルート済みスロット（GC しても正しい）。
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

bool isBoolean(Oop o) { return o.isTrue() || o.isFalse(); }

// SPEC §3.6 壊れた表: tally か array が配置に合わなければ、Dictionary と Set のネイティブは失敗する。
Oop failDamaged(CallContext& ctx, const Oop& coll) {
  return fail(ctx, coll, "damaged hashed collection");
}

// False after failing when coll's table is damaged. A keyed native checks before it sends hash.
bool checkTable(CallContext& ctx, const Oop& coll, std::uint32_t width) {
  Hashed::Table t;
  if (Hashed::read(ctx.heap, coll, width, &t) == Hashed::Shape::Damaged) {
    failDamaged(ctx, coll);
    return false;
  }
  return true;
}

// SPEC §3.6: the hash a Dictionary or Set saves for key. The identity versions take identityHash
// and send nothing; the others send hash once (sendHash). False when the frames unwind or hash
// answers no Integer: the native then answers the empty Oop.
bool keyHash(CallContext& ctx, Oop key, bool identity, std::int64_t* out) {
  if (identity) {
    *out = ao_Object_identityHash(ctx, key, nullptr, 0).smallIntegerValue();
    return true;
  }
  return sendHash(ctx, key, out);
}

enum class Lookup { Found, Absent, Failed };

struct Probe {
  Lookup result;
  std::uint32_t index;  // the entry, when Found
};

// SPEC §3.6 探索: from hash's home, compares the entries that saved hash, by == and then (unless
// identity) by `key = entryKey`, which must answer a Boolean. SPEC §3.6 再入: = may write the
// table, so after each send the table is read again from coll, and when the array, the generation
// or the compared key changed, the probe starts over with the same hash. Failed when the frames
// unwind, = answers no Boolean or the table is damaged (then it has aborted). coll and key are
// rooted; nothing runs between an answer and the caller's use of the entry.
Probe probe(CallContext& ctx, Root& coll, Root& key, std::int64_t hash, std::uint32_t width,
            bool identity) {
  Root array(ctx.roots);
  Root candidate(ctx.roots);
  Root equals(ctx.roots, identity ? Oop::nil() : ctx.wk.intern("="));
  for (;;) {
    Hashed::Table t;
    const Hashed::Shape shape = Hashed::read(ctx.heap, coll.slot, width, &t);
    if (shape == Hashed::Shape::Damaged) {
      failDamaged(ctx, coll.slot);
      return {Lookup::Failed, Hashed::kNoEntry};
    }
    if (shape == Hashed::Shape::Empty) {
      return {Lookup::Absent, Hashed::kNoEntry};
    }
    array.slot = t.array;
    const std::uint32_t mask = t.capacity - 1;
    std::uint32_t i = Hashed::home(hash, t.capacity);
    bool again = false;
    for (std::uint32_t n = 0; n < t.capacity; ++n, i = (i + 1) & mask) {
      const Oop k = ctx.heap.slotAt(array.slot, i * width + Hashed::kEntryKey);
      if (k.isNil()) {
        return {Lookup::Absent, i};
      }
      if (!Hashed::savedHashIs(ctx.heap, array.slot, width, i, hash)) {
        continue;
      }
      if (k == key.slot) {
        return {Lookup::Found, i};
      }
      if (identity) {
        continue;
      }
      candidate.slot = k;
      const Oop eq = send(ctx, key.slot, equals.slot, &candidate.slot, 1, nullptr);
      if (unwinding(ctx) || !isBoolean(eq)) {
        return {Lookup::Failed, Hashed::kNoEntry};
      }
      // SPEC §3.6 再入: another array or generation means entries were added or removed meanwhile,
      // even when the table looks as before (ABA); a write to another table changes neither. The
      // compared key is checked too, against writes that bypass the generation.
      Hashed::Table now;
      if (Hashed::read(ctx.heap, coll.slot, width, &now) != Hashed::Shape::Table ||
          now.array != array.slot || now.generation != t.generation ||
          ctx.heap.slotAt(array.slot, i * width + Hashed::kEntryKey) != candidate.slot) {
        again = true;
        break;
      }
      if (eq.isTrue()) {
        return {Lookup::Found, i};
      }
    }
    if (!again) {
      // Every entry is taken (a tally that lies, SPEC §3.6): the key is absent.
      return {Lookup::Absent, Hashed::kNoEntry};
    }
  }
}

// SPEC §3.6 挿入: puts key (and value, a Dictionary's) in a new entry after probe found key absent,
// with nothing run since. Grows first past 3/4 of the capacity, when the table is empty, or when
// no entry is free (a tally that lies). May GC: coll, key and value are rooted. False when the
// array cannot be allocated.
bool insertAbsent(CallContext& ctx, Root& coll, Root& key, Root& value, std::int64_t hash,
                  std::uint32_t width) {
  Hashed::Table t;
  const Hashed::Shape shape = Hashed::read(ctx.heap, coll.slot, width, &t);
  if (shape == Hashed::Shape::Damaged) {
    failDamaged(ctx, coll.slot);
    return false;
  }
  if (shape == Hashed::Shape::Empty || Hashed::mustGrow(t) ||
      Hashed::freeEntry(ctx.heap, t.array, t.capacity, width, hash) == Hashed::kNoEntry) {
    if (!Hashed::grow(ctx, coll, width) ||
        Hashed::read(ctx.heap, coll.slot, width, &t) != Hashed::Shape::Table) {
      return false;
    }
  }
  const std::uint32_t i = Hashed::freeEntry(ctx.heap, t.array, t.capacity, width, hash);
  Hashed::putEntry(ctx.heap, t.array, width, i, key.slot, value.slot, hash);
  ctx.heap.slotAtPut(coll.slot, Hashed::kSlotTally, Oop::fromSmallInteger(t.tally + 1));
  return true;
}

// The value of entry index of dict, which a probe just found.
Oop entryValue(const Heap& heap, Oop dict, std::uint32_t index) {
  return heap.slotAt(heap.slotAt(dict, Hashed::kSlotArray),
                     index * kDictWidth + Hashed::kEntryValue);
}

// SPEC §3.6 列挙: for each entry, from the first, puts its key in slots[0] and its value (nil in a
// Set) in slots[1] and calls visit. The table is read again from coll before each entry, so a
// block that writes it never makes this read outside the array. True at the end; false when visit
// answers false or the table is damaged (then it has aborted). Passes a safepoint every 64K entries
// looked at, free ones included.
template <typename Visit>
bool eachEntry(CallContext& ctx, Root& coll, std::uint32_t width, RootedArray& slots, Visit visit) {
  Gc gc(ctx.heap, ctx.roots);
  for (std::uint32_t i = 0;; ++i) {
    // SPEC §3.6: every 64K entries looked at, free ones included, so a sparse table passes too.
    // The table is read after the safepoint, which may move it.
    if (i != 0 && (i & 0xFFFF) == 0) {
      gc.safepoint();
    }
    Hashed::Table t;
    const Hashed::Shape shape = Hashed::read(ctx.heap, coll.slot, width, &t);
    if (shape == Hashed::Shape::Damaged) {
      failDamaged(ctx, coll.slot);
      return false;
    }
    if (shape == Hashed::Shape::Empty || i >= t.capacity) {
      return true;
    }
    const Oop k = ctx.heap.slotAt(t.array, i * width + Hashed::kEntryKey);
    if (k.isNil()) {
      continue;
    }
    slots[0] = k;
    slots[1] = width == kDictWidth ? ctx.heap.slotAt(t.array, i * width + Hashed::kEntryValue)
                                   : Oop::nil();
    if (!visit()) {
      return false;
    }
  }
}

enum class Pass { Values, Keys, KeysAndValues, Associations };

// do: (values, or a Set's elements), keysDo:, keysAndValuesDo: and associationsDo: (SPEC §3.6).
Oop enumerate(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
              std::uint32_t width, Pass pass) {
  if (argc != 1) {
    return Oop{};
  }
  Root coll(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  RootedArray slots(ctx.roots, 3);  // key, value, the Association
  const bool done = eachEntry(ctx, coll, width, slots, [&] {
    Oop ignored;
    switch (pass) {
      case Pass::Values:
        return callBlock(ctx, blk.slot, &slots[1], 1, &ignored);
      case Pass::Keys:
        return callBlock(ctx, blk.slot, &slots[0], 1, &ignored);
      case Pass::KeysAndValues:
        return callBlock(ctx, blk.slot, &slots[0], 2, &ignored);
      case Pass::Associations: {
        // A new Association per entry: writing it leaves the table alone. May GC: the key and the
        // value are read from their roots after it.
        const Oop assoc = allocateInstance(ctx, ctx.wk.associationClass, 2);
        if (!assoc.isHeap()) {
          return false;
        }
        ctx.heap.slotAtPut(assoc, kAssocKey, slots[0]);
        ctx.heap.slotAtPut(assoc, kAssocValue, slots[1]);
        slots[2] = assoc;
        return callBlock(ctx, blk.slot, &slots[2], 1, &ignored);
      }
    }
    return false;
  });
  return done ? coll.slot : Oop{};
}

Oop hashedNew(CallContext& ctx, const Oop& receiver, std::uint32_t width) {
  Root cls(ctx.roots, receiver);
  Root o(ctx.roots, send(ctx, cls.slot, ctx.wk.selBasicNew, nullptr, 0, nullptr));
  if (!o.slot.isHeap()) {
    return o.slot;
  }
  // basicNew leaves tally and array nil: an empty table. Give it the first array; when that cannot
  // be allocated, the empty table stays and the first insertion tries again.
  Hashed::Table t;
  if (Hashed::read(ctx.heap, o.slot, width, &t) == Hashed::Shape::Empty) {
    Hashed::grow(ctx, o, width);
  }
  return o.slot;
}

Oop hashedSize(CallContext& ctx, const Oop& receiver, std::uint32_t argc, std::uint32_t width) {
  if (argc != 0) {
    return Oop{};
  }
  Hashed::Table t;
  switch (Hashed::read(ctx.heap, receiver, width, &t)) {
    case Hashed::Shape::Damaged:
      return failDamaged(ctx, receiver);
    case Hashed::Shape::Empty:
      return Oop::fromSmallInteger(0);
    case Hashed::Shape::Table:
      return Oop::fromSmallInteger(t.tally);
  }
  return Oop{};
}

// SPEC §3.6 copy: shallowCopy of the receiver, then of its array when that is a heap object, so the
// copy and the receiver share no table. Never fails on a damaged table; empty Oop only when an
// allocation fails.
Oop hashedCopy(CallContext& ctx, const Oop& receiver, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root copy(ctx.roots, ao_Object_shallowCopy(ctx, receiver, nullptr, 0));
  if (!copy.slot.isHeap() || (ctx.heap.flags(copy.slot) & kFlagBytes) != 0 ||
      ctx.heap.size(copy.slot) <= Hashed::kSlotArray) {
    return copy.slot;
  }
  Root array(ctx.roots, ctx.heap.slotAt(copy.slot, Hashed::kSlotArray));
  if (!array.slot.isHeap()) {
    return copy.slot;
  }
  // shallowCopy may GC: array and copy are rooted, and the fresh array goes in right away.
  const Oop fresh = ao_Object_shallowCopy(ctx, array.slot, nullptr, 0);
  if (!fresh.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(copy.slot, Hashed::kSlotArray, fresh);
  return copy.slot;
}

Oop dictAt(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
           bool identity) {
  if (argc != 1) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  if (!checkTable(ctx, dict.slot, kDictWidth)) {
    return Oop{};
  }
  if (key.slot.isNil()) {
    return Oop::nil();
  }
  std::int64_t hash = 0;
  if (!keyHash(ctx, key.slot, identity, &hash)) {
    return Oop{};
  }
  const Probe p = probe(ctx, dict, key, hash, kDictWidth, identity);
  if (p.result == Lookup::Failed) {
    return Oop{};
  }
  return p.result == Lookup::Found ? entryValue(ctx.heap, dict.slot, p.index) : Oop::nil();
}

Oop dictAtIfAbsent(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                   bool identity) {
  if (argc != 2) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  Root blk(ctx.roots, args[1]);
  if (!checkTable(ctx, dict.slot, kDictWidth)) {
    return Oop{};
  }
  if (!key.slot.isNil()) {
    std::int64_t hash = 0;
    if (!keyHash(ctx, key.slot, identity, &hash)) {
      return Oop{};
    }
    const Probe p = probe(ctx, dict, key, hash, kDictWidth, identity);
    if (p.result == Lookup::Failed) {
      return Oop{};
    }
    if (p.result == Lookup::Found) {
      return entryValue(ctx.heap, dict.slot, p.index);
    }
  }
  Oop answer;
  if (!callBlock(ctx, blk.slot, nullptr, 0, &answer)) {
    return Oop{};
  }
  return answer;
}

Oop dictAtPut(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
              bool identity) {
  if (argc != 2) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  Root value(ctx.roots, args[1]);
  if (!checkTable(ctx, dict.slot, kDictWidth)) {
    return Oop{};
  }
  if (key.slot.isNil()) {
    return fail(ctx, dict.slot, "key must not be nil");
  }
  std::int64_t hash = 0;
  if (!keyHash(ctx, key.slot, identity, &hash)) {
    return Oop{};
  }
  const Probe p = probe(ctx, dict, key, hash, kDictWidth, identity);
  if (p.result == Lookup::Failed) {
    return Oop{};
  }
  if (p.result == Lookup::Found) {
    ctx.heap.slotAtPut(ctx.heap.slotAt(dict.slot, Hashed::kSlotArray),
                       p.index * kDictWidth + Hashed::kEntryValue, value.slot);
    return value.slot;
  }
  return insertAbsent(ctx, dict, key, value, hash, kDictWidth) ? value.slot : Oop{};
}

Oop dictIncludesKey(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                    bool identity) {
  if (argc != 1) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  if (!checkTable(ctx, dict.slot, kDictWidth)) {
    return Oop{};
  }
  if (key.slot.isNil()) {
    return Oop::false_();
  }
  std::int64_t hash = 0;
  if (!keyHash(ctx, key.slot, identity, &hash)) {
    return Oop{};
  }
  const Probe p = probe(ctx, dict, key, hash, kDictWidth, identity);
  if (p.result == Lookup::Failed) {
    return Oop{};
  }
  return p.result == Lookup::Found ? Oop::true_() : Oop::false_();
}

// removeKey: (withBlock false) and removeKey:ifAbsent: (SPEC §3.6). The entry goes by backward
// shift, which sends nothing.
Oop dictRemoveKey(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                  bool identity, bool withBlock) {
  if (argc != (withBlock ? 2u : 1u)) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root key(ctx.roots, args[0]);
  Root blk(ctx.roots, withBlock ? args[1] : Oop::nil());
  if (!checkTable(ctx, dict.slot, kDictWidth)) {
    return Oop{};
  }
  if (!key.slot.isNil()) {
    std::int64_t hash = 0;
    if (!keyHash(ctx, key.slot, identity, &hash)) {
      return Oop{};
    }
    const Probe p = probe(ctx, dict, key, hash, kDictWidth, identity);
    if (p.result == Lookup::Failed) {
      return Oop{};
    }
    Hashed::Table t;
    if (p.result == Lookup::Found &&
        Hashed::read(ctx.heap, dict.slot, kDictWidth, &t) == Hashed::Shape::Table) {
      const Oop value = ctx.heap.slotAt(t.array, p.index * kDictWidth + Hashed::kEntryValue);
      Hashed::removeEntry(ctx.heap, t.array, t.capacity, kDictWidth, p.index);
      ctx.heap.slotAtPut(dict.slot, Hashed::kSlotTally,
                         Oop::fromSmallInteger(t.tally > 0 ? t.tally - 1 : 0));
      return value;
    }
  }
  if (!withBlock) {
    return fail(ctx, dict.slot, "key not found");
  }
  Oop answer;
  if (!callBlock(ctx, blk.slot, nullptr, 0, &answer)) {
    return Oop{};
  }
  return answer;
}

Oop setAdd(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
           bool identity) {
  if (argc != 1) {
    return Oop{};
  }
  Root set(ctx.roots, receiver);
  Root element(ctx.roots, args[0]);
  if (!checkTable(ctx, set.slot, kSetWidth)) {
    return Oop{};
  }
  if (element.slot.isNil()) {
    return fail(ctx, set.slot, "element must not be nil");
  }
  std::int64_t hash = 0;
  if (!keyHash(ctx, element.slot, identity, &hash)) {
    return Oop{};
  }
  const Probe p = probe(ctx, set, element, hash, kSetWidth, identity);
  if (p.result == Lookup::Failed) {
    return Oop{};
  }
  if (p.result == Lookup::Found) {
    return element.slot;
  }
  return insertAbsent(ctx, set, element, element, hash, kSetWidth) ? element.slot : Oop{};
}

Oop setIncludes(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc,
                bool identity) {
  if (argc != 1) {
    return Oop{};
  }
  Root set(ctx.roots, receiver);
  Root element(ctx.roots, args[0]);
  if (!checkTable(ctx, set.slot, kSetWidth)) {
    return Oop{};
  }
  if (element.slot.isNil()) {
    return Oop::false_();
  }
  std::int64_t hash = 0;
  if (!keyHash(ctx, element.slot, identity, &hash)) {
    return Oop{};
  }
  const Probe p = probe(ctx, set, element, hash, kSetWidth, identity);
  if (p.result == Lookup::Failed) {
    return Oop{};
  }
  return p.result == Lookup::Found ? Oop::true_() : Oop::false_();
}

// SPEC §3.6 OrderedCollection: what the slots `array firstIndex lastIndex` say.
enum class OcShape {
  Empty,    // array is nil (basicNew): no elements
  Sound,    // the elements are array at firstIndex .. lastIndex
  Damaged,  // they do not fit (instVarAt:put:): the natives fail
};

struct OcSlots {
  Oop array{};
  std::int64_t first = 1;
  std::int64_t last = 0;
};

// Reads oc's slots. Sound when array is a Kernel Array and firstIndex and lastIndex are
// SmallIntegers with 1 <= firstIndex <= lastIndex + 1 <= array size + 1. Does not allocate.
OcShape ocRead(const Heap& heap, const WellKnown& wk, Oop oc, OcSlots* out) {
  if (!oc.isHeap() || (heap.flags(oc) & kFlagBytes) != 0 || heap.size(oc) <= kOcLast) {
    return OcShape::Damaged;
  }
  const Oop array = heap.slotAt(oc, kOcArray);
  if (array.isNil()) {
    return OcShape::Empty;
  }
  const Oop first = heap.slotAt(oc, kOcFirst);
  const Oop last = heap.slotAt(oc, kOcLast);
  if (!array.isHeap() || heap.klass(array) != wk.arrayClass ||
      (heap.flags(array) & kFlagBytes) != 0 || !first.isSmallInteger() || !last.isSmallInteger()) {
    return OcShape::Damaged;
  }
  const std::int64_t f = first.smallIntegerValue();
  const std::int64_t l = last.smallIntegerValue();
  if (f < 1 || l < f - 1 || l > static_cast<std::int64_t>(heap.size(array))) {
    return OcShape::Damaged;
  }
  out->array = array;
  out->first = f;
  out->last = l;
  return OcShape::Sound;
}

Oop failDamagedOc(CallContext& ctx, const Oop& oc) {
  return fail(ctx, oc, "damaged ordered collection");
}

// Makes room after the last element of a sound oc, its elements then starting at 1. When twice
// the elements fit in its Array, slides them to the front in place (B10: the ready queue's FIFO
// use must not grow it without bound); else gives it an Array of twice the size (kDefaultCap at
// least). May GC: oc is a Root. False when the Array cannot be allocated or oc is not sound.
bool ocGrow(CallContext& ctx, Root& oc) {
  OcSlots s;
  if (ocRead(ctx.heap, ctx.wk, oc.slot, &s) != OcShape::Sound) {
    return false;
  }
  const std::int64_t used = s.last - s.first + 1;
  const std::uint32_t size = ctx.heap.size(s.array);
  if (s.first > 1 && static_cast<std::uint64_t>(used) * 2 <= size) {
    // Room at the front: slide the elements there and nil the rest, so the Array holds nothing
    // it no longer contains. A FIFO use (add: at the end, removal at the front) stays this size.
    for (std::int64_t i = 0; i < used; ++i) {
      ctx.heap.slotAtPut(s.array, static_cast<std::uint32_t>(i),
                         ctx.heap.slotAt(s.array, static_cast<std::uint32_t>(s.first - 1 + i)));
    }
    for (auto i = static_cast<std::uint32_t>(used); i < size; ++i) {
      ctx.heap.slotAtPut(s.array, i, Oop::nil());
    }
    ctx.heap.slotAtPut(oc.slot, kOcFirst, Oop::fromSmallInteger(1));
    ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(used));
    return true;
  }
  const std::uint64_t next = std::max(
      {std::uint64_t{kDefaultCap}, std::uint64_t{size} * 2, static_cast<std::uint64_t>(used) + 1});
  if (next > UINT32_MAX) {
    ctx.heap.setOutOfMemory();
    return false;
  }
  Root old(ctx.roots, s.array);
  const Oop grown = allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(next), 0);
  if (!grown.isHeap()) {
    return false;
  }
  // No user code ran, so first and last are as read and old is still oc's array.
  for (std::int64_t i = 0; i < used; ++i) {
    ctx.heap.slotAtPut(grown, static_cast<std::uint32_t>(i),
                       ctx.heap.slotAt(old.slot, static_cast<std::uint32_t>(s.first - 1 + i)));
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
  return hashedNew(ctx, receiver, kDictWidth);
}

Oop ao_Dictionary_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  return hashedSize(ctx, receiver, argc, kDictWidth);
}

Oop ao_Dictionary_at_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return dictAt(ctx, receiver, args, argc, false);
}

Oop ao_Dictionary_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  return dictAtPut(ctx, receiver, args, argc, false);
}

Oop ao_Dictionary_at_ifAbsent_(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  return dictAtIfAbsent(ctx, receiver, args, argc, false);
}

Oop ao_Dictionary_includesKey_(CallContext& ctx, const Oop& receiver, const Oop* args,
                               std::uint32_t argc) {
  return dictIncludesKey(ctx, receiver, args, argc, false);
}

Oop ao_Dictionary_removeKey_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  return dictRemoveKey(ctx, receiver, args, argc, false, false);
}

Oop ao_Dictionary_removeKey_ifAbsent_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                      std::uint32_t argc) {
  return dictRemoveKey(ctx, receiver, args, argc, false, true);
}

Oop ao_IdentityDictionary_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  return dictAt(ctx, receiver, args, argc, true);
}

Oop ao_IdentityDictionary_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc) {
  return dictAtPut(ctx, receiver, args, argc, true);
}

Oop ao_IdentityDictionary_at_ifAbsent_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                       std::uint32_t argc) {
  return dictAtIfAbsent(ctx, receiver, args, argc, true);
}

Oop ao_IdentityDictionary_includesKey_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                       std::uint32_t argc) {
  return dictIncludesKey(ctx, receiver, args, argc, true);
}

Oop ao_IdentityDictionary_removeKey_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                     std::uint32_t argc) {
  return dictRemoveKey(ctx, receiver, args, argc, true, false);
}

Oop ao_IdentityDictionary_removeKey_ifAbsent_(CallContext& ctx, const Oop& receiver,
                                              const Oop* args, std::uint32_t argc) {
  return dictRemoveKey(ctx, receiver, args, argc, true, true);
}

// SPEC §3.6: whether a value is = to the argument (`anObject = value`, not sent to the identical
// value). No hash is sent.
Oop ao_Dictionary_includes_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root needle(ctx.roots, args[0]);
  Root equals(ctx.roots, ctx.wk.intern("="));
  RootedArray slots(ctx.roots, 2);
  bool found = false;
  const bool done = eachEntry(ctx, dict, kDictWidth, slots, [&] {
    if (slots[1] == needle.slot) {
      found = true;
      return false;
    }
    const Oop eq = send(ctx, needle.slot, equals.slot, &slots[1], 1, nullptr);
    if (unwinding(ctx) || !isBoolean(eq)) {
      return false;
    }
    found = eq.isTrue();
    return !found;
  });
  if (found) {
    return Oop::true_();
  }
  return done ? Oop::false_() : Oop{};
}

// SPEC §3.6: the values, not Associations (Blue Book).
Oop ao_Dictionary_do_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return enumerate(ctx, receiver, args, argc, kDictWidth, Pass::Values);
}

Oop ao_Dictionary_keysDo_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  return enumerate(ctx, receiver, args, argc, kDictWidth, Pass::Keys);
}

Oop ao_Dictionary_associationsDo_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                  std::uint32_t argc) {
  return enumerate(ctx, receiver, args, argc, kDictWidth, Pass::Associations);
}

Oop ao_Dictionary_keysAndValuesDo_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                   std::uint32_t argc) {
  return enumerate(ctx, receiver, args, argc, kDictWidth, Pass::KeysAndValues);
}

// SPEC §3.6: an Array of the block's answers for the values, as large as the tally was.
Oop ao_Dictionary_collect_(CallContext& ctx, const Oop& receiver, const Oop* args,
                           std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root dict(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  Hashed::Table t;
  const Hashed::Shape shape = Hashed::read(ctx.heap, dict.slot, kDictWidth, &t);
  if (shape == Hashed::Shape::Damaged) {
    return failDamaged(ctx, dict.slot);
  }
  Oop nOop = Oop::fromSmallInteger(shape == Hashed::Shape::Table ? t.tally : 0);
  Root arr(ctx.roots, send(ctx, ctx.wk.arrayClass, ctx.wk.selBasicNew_, &nOop, 1, nullptr));
  if (!arr.slot.isHeap()) {
    return arr.slot;
  }
  RootedArray slots(ctx.roots, 4);  // key, value, index, the block's answer
  std::int64_t idx = 1;
  const bool done = eachEntry(ctx, dict, kDictWidth, slots, [&] {
    if (!callBlock(ctx, blk.slot, &slots[1], 1, &slots[3])) {
      return false;
    }
    slots[2] = Oop::fromSmallInteger(idx);
    send(ctx, arr.slot, ctx.wk.selAt_put_, &slots[2], 2, nullptr);
    if (unwinding(ctx)) {
      return false;
    }
    ++idx;
    return true;
  });
  return done ? arr.slot : Oop{};
}

Oop ao_Set_new(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return hashedNew(ctx, receiver, kSetWidth);
}

Oop ao_Set_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  return hashedSize(ctx, receiver, argc, kSetWidth);
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
  return enumerate(ctx, receiver, args, argc, kSetWidth, Pass::Keys);
}

// SPEC §3.6: copy copies the array too, so the copy and the receiver share no table.
Oop ao_Dictionary_copy(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  return hashedCopy(ctx, receiver, argc);
}

Oop ao_Set_copy(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  return hashedCopy(ctx, receiver, argc);
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
  OcSlots s;
  switch (ocRead(ctx.heap, ctx.wk, receiver, &s)) {
    case OcShape::Damaged:
      return failDamagedOc(ctx, receiver);
    case OcShape::Empty:
      return Oop::fromSmallInteger(0);
    case OcShape::Sound:
      return Oop::fromSmallInteger(s.last - s.first + 1);
  }
  return Oop{};
}

Oop ao_OrderedCollection_add_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root oc(ctx.roots, receiver);
  Root value(ctx.roots, args[0]);
  OcSlots s;
  const OcShape shape = ocRead(ctx.heap, ctx.wk, oc.slot, &s);
  if (shape == OcShape::Damaged) {
    return failDamagedOc(ctx, oc.slot);
  }
  if (shape == OcShape::Empty) {
    // SPEC §3.6: the first add: makes an Array of kDefaultCap and starts at 1.
    const Oop arr = allocateRetry(ctx, ctx.wk.arrayClass, kDefaultCap, 0);
    if (!arr.isHeap()) {
      return Oop{};
    }
    ctx.heap.slotAtPut(oc.slot, kOcArray, arr);
    ctx.heap.slotAtPut(oc.slot, kOcFirst, Oop::fromSmallInteger(1));
    ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(0));
  } else if (s.last == static_cast<std::int64_t>(ctx.heap.size(s.array))) {
    if (!ocGrow(ctx, oc)) {
      return Oop{};
    }
  }
  // Read again: an allocation above may have moved the array.
  if (ocRead(ctx.heap, ctx.wk, oc.slot, &s) != OcShape::Sound) {
    return failDamagedOc(ctx, oc.slot);
  }
  s.last += 1;
  ctx.heap.slotAtPut(s.array, static_cast<std::uint32_t>(s.last - 1), value.slot);
  ctx.heap.slotAtPut(oc.slot, kOcLast, Oop::fromSmallInteger(s.last));
  return value.slot;
}

// SPEC §3.6: fails with "at: index out of range" unless index is a SmallInteger in [1, size].
// firstIndex, lastIndex and array can be written from Smalltalk, so the slot is checked against
// the array too.
Oop ao_OrderedCollection_at_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  OcSlots s;
  const OcShape shape = ocRead(ctx.heap, ctx.wk, receiver, &s);
  if (shape == OcShape::Damaged) {
    return failDamagedOc(ctx, receiver);
  }
  if (shape == OcShape::Sound && args[0].isSmallInteger()) {
    const auto index = args[0].smallIntegerValue();
    if (index >= 1 && index <= s.last - s.first + 1) {
      // A sound shape keeps first + index - 1 within the array (1-based).
      return ctx.heap.slotAt(s.array, static_cast<std::uint32_t>(s.first + index - 2));
    }
  }
  return fail(ctx, receiver, "at: index out of range");
}

Oop ao_OrderedCollection_do_(CallContext& ctx, const Oop& receiver, const Oop* args,
                             std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root oc(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  OcSlots s;
  const OcShape shape = ocRead(ctx.heap, ctx.wk, oc.slot, &s);
  if (shape == OcShape::Damaged) {
    return failDamagedOc(ctx, oc.slot);
  }
  if (shape == OcShape::Empty) {
    return oc.slot;
  }
  // SPEC §3.6: the indexes do: started with (Blue Book). The block may write the collection, so
  // its slots are read again before each element: damaged fails, an index outside them ends it.
  const std::int64_t first = s.first;
  const std::int64_t last = s.last;
  Root elt(ctx.roots);
  Gc gc(ctx.heap, ctx.roots);
  std::uint64_t visited = 0;
  for (std::int64_t i = first; i <= last; ++i) {
    const OcShape now = ocRead(ctx.heap, ctx.wk, oc.slot, &s);
    if (now == OcShape::Damaged) {
      return failDamagedOc(ctx, oc.slot);
    }
    if (now == OcShape::Empty || i < s.first || i > s.last) {
      break;
    }
    elt.slot = ctx.heap.slotAt(s.array, static_cast<std::uint32_t>(i - 1));
    Oop ignored;
    if (!callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
      return Oop{};
    }
    if ((++visited & 0xFFFF) == 0) {
      gc.safepoint();
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

namespace {

// SPEC §3.6 Interval: the number of elements when start, stop and step are SmallIntegers. It may
// pass SmallInteger (2^63 at most) and is then a LargeInteger, which may GC.
Oop intervalSizeSmi(CallContext& ctx, std::int64_t start, std::int64_t stop, std::int64_t step) {
  if (step == 0 || (step > 0 && start > stop) || (step < 0 && start < stop)) {
    return Oop::fromSmallInteger(0);
  }
  const __int128 diff = step > 0 ? static_cast<__int128>(stop) - start
                                 : static_cast<__int128>(start) - stop;
  const __int128 n = diff / (step > 0 ? static_cast<__int128>(step) : -static_cast<__int128>(step));
  // n is at most 2^63 - 1, so n + 1 elements fit once n is boxed.
  if (n < kSmiMax) {
    return Oop::fromSmallInteger(static_cast<std::int64_t>(n + 1));
  }
  const Oop boxed = LargeInteger::fromInt64(ctx, static_cast<std::int64_t>(n));
  return boxed.isEmpty() ? boxed : LargeInteger::add(ctx, boxed, Oop::fromSmallInteger(1));
}

// SPEC §3.6 Interval: the exact number of elements when start, stop and step are Integers, not
// all SmallIntegers: 0 for a zero step, or when stop lies before start in the step's direction;
// otherwise (stop - start) // step + 1. Sends nothing. iv is a rooted slot, read again after each
// LargeInteger operation (they read their operands before they allocate, and may GC). Empty Oop
// when an allocation fails.
Oop intervalSizeInteger(CallContext& ctx, const Oop& iv) {
  const Oop zero = Oop::fromSmallInteger(0);
  const int sign = LargeInteger::compare(ctx.heap, ctx.wk, ctx.heap.slotAt(iv, kIvStep), zero);
  const int order = LargeInteger::compare(ctx.heap, ctx.wk, ctx.heap.slotAt(iv, kIvStop),
                                          ctx.heap.slotAt(iv, kIvStart));
  if (sign == 0 || (sign > 0 && order < 0) || (sign < 0 && order > 0)) {
    return zero;
  }
  Root diff(ctx.roots, LargeInteger::sub(ctx, ctx.heap.slotAt(iv, kIvStop),
                                         ctx.heap.slotAt(iv, kIvStart)));
  if (diff.slot.isEmpty()) {
    return Oop{};
  }
  Root quotient(ctx.roots, LargeInteger::floorDiv(ctx, diff.slot, ctx.heap.slotAt(iv, kIvStep)));
  if (quotient.slot.isEmpty()) {
    return Oop{};
  }
  return LargeInteger::add(ctx, quotient.slot, Oop::fromSmallInteger(1));
}

// SPEC §3.6 Interval: the direction of a step, 1 forward, -1 backward, 0 none. A SmallInteger
// step goes by its sign; any other gets `step < 0`, then `step > 0`. False when the frames unwind
// or an answer is no Boolean.
bool intervalDirection(CallContext& ctx, Root& step, int* out) {
  if (step.slot.isSmallInteger()) {
    const auto s = step.slot.smallIntegerValue();
    *out = s > 0 ? 1 : (s < 0 ? -1 : 0);
    return true;
  }
  Oop zero = Oop::fromSmallInteger(0);
  const Oop negative = send(ctx, step.slot, ctx.wk.intern("<"), &zero, 1, nullptr);
  if (unwinding(ctx) || !isBoolean(negative)) {
    return false;
  }
  if (negative.isTrue()) {
    *out = -1;
    return true;
  }
  const Oop positive = send(ctx, step.slot, ctx.wk.intern(">"), &zero, 1, nullptr);
  if (unwinding(ctx) || !isBoolean(positive)) {
    return false;
  }
  *out = positive.isTrue() ? 1 : 0;
  return true;
}

// SPEC §3.6 Interval: calls visit with each element (a rooted slot) of iv, which is not all
// SmallIntegers, while `element <= stop` (forward) or `element >= stop` (backward) answers true;
// `element + step` is the next. No cap on the count: a safepoint every 64K elements, and the
// block's abort or unwind stops it. False when a comparison answers no Boolean, a step fails, the
// frames unwind, or visit answers false.
template <typename Visit>
bool intervalWalk(CallContext& ctx, Root& iv, Visit visit) {
  Root cur(ctx.roots, ctx.heap.slotAt(iv.slot, kIvStart));
  Root stop(ctx.roots, ctx.heap.slotAt(iv.slot, kIvStop));
  Root step(ctx.roots, ctx.heap.slotAt(iv.slot, kIvStep));
  int direction = 0;
  if (!intervalDirection(ctx, step, &direction)) {
    return false;
  }
  if (direction == 0) {
    return true;
  }
  // Selectors are used across sends; a full GC's compaction moves Symbols too, so they are rooted.
  // The walk goes on while `element <= stop` (forward) or `element >= stop` (backward) answers
  // true: a comparison with NaN is false, so a NaN bound ends it at once.
  Root within(ctx.roots, ctx.wk.intern(direction > 0 ? "<=" : ">="));
  Root add(ctx.roots, ctx.wk.intern("+"));
  Gc gc(ctx.heap, ctx.roots);
  for (std::uint64_t n = 1;; ++n) {
    const Oop inside = send(ctx, cur.slot, within.slot, &stop.slot, 1, nullptr);
    if (unwinding(ctx) || !isBoolean(inside)) {
      return false;
    }
    if (inside.isFalse()) {
      return true;
    }
    if (!visit(cur)) {
      return false;
    }
    cur.slot = send(ctx, cur.slot, add.slot, &step.slot, 1, nullptr);
    if (unwinding(ctx) || cur.slot.isEmpty()) {
      return false;
    }
    if ((n & 0xFFFF) == 0) {
      gc.safepoint();
    }
  }
}

}  // namespace

Oop ao_Interval_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  const Oop start = ctx.heap.slotAt(receiver, kIvStart);
  const Oop stop = ctx.heap.slotAt(receiver, kIvStop);
  const Oop step = ctx.heap.slotAt(receiver, kIvStep);
  if (start.isSmallInteger() && stop.isSmallInteger() && step.isSmallInteger()) {
    return intervalSizeSmi(ctx, start.smallIntegerValue(), stop.smallIntegerValue(),
                           step.smallIntegerValue());
  }
  if (LargeInteger::isInteger(ctx.wk, start) && LargeInteger::isInteger(ctx.wk, stop) &&
      LargeInteger::isInteger(ctx.wk, step)) {
    return intervalSizeInteger(ctx, receiver);
  }
  Root iv(ctx.roots, receiver);
  // SPEC §3.6: no cap on the count. Past SmallInteger it is boxed as a LargeInteger (which may GC;
  // nothing else is held across it). int64 would take 2^63 elements to overflow.
  std::int64_t count = 0;
  const bool done = intervalWalk(ctx, iv, [&](Root&) {
    ++count;
    return true;
  });
  return done ? LargeInteger::fromInt64(ctx, count) : Oop{};
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
    const auto last = stop.smallIntegerValue();
    if (st == 0) {
      return iv.slot;
    }
    Root elt(ctx.roots);
    Gc gc(ctx.heap, ctx.roots);
    std::uint64_t n = 0;
    // start and stop are SmallIntegers, so every element up to stop is one too.
    for (__int128 i = start.smallIntegerValue(); st > 0 ? i <= last : i >= last; i += st) {
      elt.slot = Oop::fromSmallInteger(static_cast<std::int64_t>(i));
      Oop ignored;
      if (!callBlock(ctx, blk.slot, &elt.slot, 1, &ignored)) {
        return Oop{};
      }
      if ((++n & 0xFFFF) == 0) {
        gc.safepoint();
      }
    }
    return iv.slot;
  }
  const bool done = intervalWalk(ctx, iv, [&](Root& element) {
    Oop ignored;
    return callBlock(ctx, blk.slot, &element.slot, 1, &ignored);
  });
  return done ? iv.slot : Oop{};
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
  putNative(heap, wk, wk.dictionaryClass, "at:ifAbsent:", 2, "ao_Dictionary_at_ifAbsent_",
            ao_Dictionary_at_ifAbsent_);
  putNative(heap, wk, wk.dictionaryClass, "includesKey:", 1, "ao_Dictionary_includesKey_",
            ao_Dictionary_includesKey_);
  putNative(heap, wk, wk.dictionaryClass, "removeKey:", 1, "ao_Dictionary_removeKey_",
            ao_Dictionary_removeKey_);
  putNative(heap, wk, wk.dictionaryClass, "removeKey:ifAbsent:", 2,
            "ao_Dictionary_removeKey_ifAbsent_", ao_Dictionary_removeKey_ifAbsent_);
  putNative(heap, wk, wk.dictionaryClass, "includes:", 1, "ao_Dictionary_includes_",
            ao_Dictionary_includes_);
  putNative(heap, wk, wk.dictionaryClass, "do:", 1, "ao_Dictionary_do_", ao_Dictionary_do_);
  putNative(heap, wk, wk.dictionaryClass, "keysDo:", 1, "ao_Dictionary_keysDo_",
            ao_Dictionary_keysDo_);
  putNative(heap, wk, wk.dictionaryClass, "associationsDo:", 1, "ao_Dictionary_associationsDo_",
            ao_Dictionary_associationsDo_);
  putNative(heap, wk, wk.dictionaryClass, "keysAndValuesDo:", 1, "ao_Dictionary_keysAndValuesDo_",
            ao_Dictionary_keysAndValuesDo_);
  putNative(heap, wk, wk.dictionaryClass, "collect:", 1, "ao_Dictionary_collect_",
            ao_Dictionary_collect_);
  putNative(heap, wk, wk.dictionaryClass, "copy", 0, "ao_Dictionary_copy", ao_Dictionary_copy);

  putNative(heap, wk, wk.identityDictionaryClass, "at:", 1, "ao_IdentityDictionary_at_",
            ao_IdentityDictionary_at_);
  putNative(heap, wk, wk.identityDictionaryClass, "at:put:", 2, "ao_IdentityDictionary_at_put_",
            ao_IdentityDictionary_at_put_);
  putNative(heap, wk, wk.identityDictionaryClass, "at:ifAbsent:", 2,
            "ao_IdentityDictionary_at_ifAbsent_", ao_IdentityDictionary_at_ifAbsent_);
  putNative(heap, wk, wk.identityDictionaryClass, "includesKey:", 1,
            "ao_IdentityDictionary_includesKey_", ao_IdentityDictionary_includesKey_);
  putNative(heap, wk, wk.identityDictionaryClass, "removeKey:", 1,
            "ao_IdentityDictionary_removeKey_", ao_IdentityDictionary_removeKey_);
  putNative(heap, wk, wk.identityDictionaryClass, "removeKey:ifAbsent:", 2,
            "ao_IdentityDictionary_removeKey_ifAbsent_", ao_IdentityDictionary_removeKey_ifAbsent_);

  putNative(heap, wk, wk.setMetaclass, "new", 0, "ao_Set_new", ao_Set_new);
  putNative(heap, wk, wk.setClass, "size", 0, "ao_Set_size", ao_Set_size);
  putNative(heap, wk, wk.setClass, "add:", 1, "ao_Set_add_", ao_Set_add_);
  putNative(heap, wk, wk.setClass, "includes:", 1, "ao_Set_includes_", ao_Set_includes_);
  putNative(heap, wk, wk.setClass, "do:", 1, "ao_Set_do_", ao_Set_do_);
  putNative(heap, wk, wk.setClass, "copy", 0, "ao_Set_copy", ao_Set_copy);

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

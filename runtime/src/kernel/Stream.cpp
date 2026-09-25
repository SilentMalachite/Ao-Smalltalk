#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Format.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Globals.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <algorithm>
#include <cstring>
#include <string>
#include <string_view>

namespace ao {
namespace {

constexpr std::uint32_t kStreamCollection = 0;
constexpr std::uint32_t kStreamPosition   = 1;
constexpr std::uint32_t kStreamReadLimit  = 2;
constexpr std::uint32_t kStreamWriteLimit = 3;

// SPEC §3.6 Kernel-Classes: OrderedCollection's `array firstIndex lastIndex`.
constexpr std::uint32_t kOcArray = 0;
constexpr std::uint32_t kOcFirst = 1;
constexpr std::uint32_t kOcLast = 2;

// receiver はルート済みスロット。メッセージの割り当てで GC が走っても正しい。
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

bool isBytes(const Heap& heap, Oop obj) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) != 0;
}

const unsigned char* payload(const Heap& heap, Oop bytes) {
  return reinterpret_cast<const unsigned char*>(heap.header(bytes) + 1);
}

bool isArray(CallContext& ctx, Oop obj) {
  return obj.isHeap() && ctx.wk.classOf(obj) == ctx.wk.arrayClass;
}

bool isStringy(CallContext& ctx, Oop obj) {
  if (!obj.isHeap()) {
    return false;
  }
  return chainIncludes(ctx.heap, ctx.wk.classOf(obj), ctx.wk.stringClass);
}

bool hasWriteLimit(const Heap& heap, Oop stream) {
  return stream.isHeap() && heap.size(stream) > kStreamWriteLimit;
}

std::int64_t smiOr(Oop v, std::int64_t fallback) {
  return v.isSmallInteger() ? v.smallIntegerValue() : fallback;
}

void noteWrite(Heap& heap, Oop stream, std::int64_t neu) {
  heap.slotAtPut(stream, kStreamPosition, Oop::fromSmallInteger(neu));
  if (smiOr(heap.slotAt(stream, kStreamReadLimit), 0) < neu) {
    heap.slotAtPut(stream, kStreamReadLimit, Oop::fromSmallInteger(neu));
  }
  if (hasWriteLimit(heap, stream) &&
      smiOr(heap.slotAt(stream, kStreamWriteLimit), 0) < neu) {
    heap.slotAtPut(stream, kStreamWriteLimit, Oop::fromSmallInteger(neu));
  }
}

std::int64_t collectionSize(CallContext& ctx, Root& coll) {
  const Oop n = send(ctx, coll.slot, ctx.wk.selSize, nullptr, 0, nullptr);
  return smiOr(n, 0);
}

// SPEC §3.6: an Array of twice arr's size (at least minSize, at most 2^32 - 1 slots) holding its
// elements. Past 2^32 - 1 slots nothing is allocated: out of memory. May GC.
Oop growArray(CallContext& ctx, Root& arr, std::int64_t minSize) {
  if (minSize > static_cast<std::int64_t>(UINT32_MAX)) {
    ctx.heap.setOutOfMemory();
    return Oop{};
  }
  const std::uint32_t old = arr.slot.isHeap() ? ctx.heap.size(arr.slot) : 0;
  const std::uint64_t doubled = old == 0 ? 1u : std::uint64_t{old} * 2u;
  const std::uint64_t neu = std::min<std::uint64_t>(
      std::max<std::uint64_t>(doubled, static_cast<std::uint64_t>(minSize)), UINT32_MAX);
  Oop n = allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(neu), 0);
  if (!n.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < old; ++i) {
    ctx.heap.slotAtPut(n, i, ctx.heap.slotAt(arr.slot, i));
  }
  return n;
}

// SPEC §3.6: str's bytes and cp's UTF-8 in a new String of str's class (String for a Symbol, which
// is interned and must not grow). A bytes class has no named slots. May GC: str is a Root.
Oop stringAppendChar(CallContext& ctx, Root& str, char32_t cp) {
  unsigned char enc[4];
  const auto n = Str::encodeUtf8(cp, enc);
  if (n == 0 || !isBytes(ctx.heap, str.slot)) {
    return Oop{};
  }
  const auto old = ctx.heap.size(str.slot);
  if (std::uint64_t{old} + n > UINT32_MAX) {
    ctx.heap.setOutOfMemory();
    return Oop{};
  }
  const Oop cls = ctx.wk.classOf(str.slot);
  Root answerClass(ctx.roots,
                   chainIncludes(ctx.heap, cls, ctx.wk.symbolClass) ? ctx.wk.stringClass : cls);
  Oop neu = allocateRetry(ctx, answerClass.slot, old + n, kFlagBytes);
  if (!neu.isHeap()) {
    return Oop{};
  }
  if (old != 0) {
    std::memcpy(ctx.heap.bytes(neu), ctx.heap.header(str.slot) + 1, old);
  }
  std::memcpy(ctx.heap.bytes(neu) + old, enc, n);
  return neu;
}

// SPEC §3.6 String への書き込み, case 2: the cheap check before trusting the byte offset the
// reserve gives. Inside the n bytes, the character at `at` is one byte, and one ends right
// before it.
bool reserveOffsetLooksSound(const unsigned char* p, std::uint32_t n, std::int64_t at) {
  if (at < 0 || at > static_cast<std::int64_t>(n)) {
    return false;
  }
  if (at < static_cast<std::int64_t>(n) && p[at] >= 0x80) {
    return false;
  }
  if (at == 0) {
    return true;
  }
  std::int64_t start = at - 1;
  while (start > 0 && at - start < 4 && (p[start] & 0xC0) == 0x80) {
    --start;
  }
  return start + Str::charBytes(p + start, n - static_cast<std::uint32_t>(start)) == at;
}

void setWriteLimit(Heap& heap, Oop stream, std::int64_t limit) {
  if (hasWriteLimit(heap, stream)) {
    heap.slotAtPut(stream, kStreamWriteLimit, Oop::fromSmallInteger(limit));
  }
}

// A new String of coll's class: its bytes before `at`, then enc (w bytes), then `tail` bytes
// taken from coll at `from`, then `pad` NUL bytes (reserve). Replaces the stream's collection.
bool replaceString(CallContext& ctx, Root& self, Root& coll, std::uint32_t at,
                   const unsigned char* enc, std::uint32_t w, std::uint32_t from,
                   std::uint32_t tail, std::uint64_t pad) {
  const std::uint64_t size = std::uint64_t{at} + w + tail + pad;
  if (size > UINT32_MAX) {
    ctx.heap.setOutOfMemory();
    return false;
  }
  Root cls(ctx.roots, ctx.wk.classOf(coll.slot));
  const Oop s = allocateRetry(ctx, cls.slot, static_cast<std::uint32_t>(size), kFlagBytes);
  if (!s.isHeap()) {
    return false;
  }
  auto* out = reinterpret_cast<unsigned char*>(ctx.heap.bytes(s));
  const unsigned char* in = payload(ctx.heap, coll.slot);
  std::memcpy(out, in, at);
  std::memcpy(out + at, enc, w);
  std::memcpy(out + at + w, in + from, tail);
  std::memset(out + at + w + tail, 0, static_cast<std::size_t>(pad));
  coll.slot = s;
  ctx.heap.slotAtPut(self.slot, kStreamCollection, s);
  return true;
}

// SPEC §3.6 String への書き込み: nextPut: of val at position pos (0 <= pos < SmallInteger max) into
// the stream's String collection, whose class finds the Kernel String>>at:put:. Writes the UTF-8
// in place when the widths agree or a wide character fits the one-byte reserve; otherwise
// replaces the collection with a new String (one character replaced, or the bytes up to the
// position, the character and a doubled reserve).
Oop stringNextPut(CallContext& ctx, Root& self, Root& coll, Root& val, std::int64_t pos) {
  unsigned char enc[4];
  const std::uint32_t w = val.slot.isCharacter() ? Str::encodeUtf8(val.slot.characterValue(), enc)
                                                 : 0;
  if (w == 0) {
    return fail(ctx, self.slot, "nextPut: value out of range");
  }
  const std::int64_t readLimit = smiOr(ctx.heap.slotAt(self.slot, kStreamReadLimit), 0);
  const Oop limitOop =
      hasWriteLimit(ctx.heap, self.slot) ? ctx.heap.slotAt(self.slot, kStreamWriteLimit) : Oop{};
  const bool limitKnown = limitOop.isSmallInteger() && limitOop.smallIntegerValue() >= 0;
  const std::int64_t limit = limitKnown ? limitOop.smallIntegerValue() : 0;
  // Past readLimit, while writeLimit is the character count, every character is reserve.
  const bool inReserve = limitKnown && pos >= readLimit;
  const std::uint32_t total = ctx.heap.size(coll.slot);
  const unsigned char* p = payload(ctx.heap, coll.slot);
  std::int64_t at = -1;
  if (limitKnown && pos <= limit) {
    if (total == limit) {
      at = pos;  // every character is one byte
    } else if (inReserve && limit - pos <= total &&
               reserveOffsetLooksSound(p, total, total - (limit - pos))) {
      at = total - (limit - pos);
    }
  }
  if (at < 0) {
    at = Str::byteOffsetOfChar(p, total, pos);
    if (at < 0) {
      return fail(ctx, self.slot, "nextPut: past end");
    }
  }
  const auto byteAt = static_cast<std::uint32_t>(at);
  const bool atEnd = byteAt == total;
  const std::uint32_t old = atEnd ? 0 : Str::charBytes(p + byteAt, total - byteAt);
  std::int64_t newLimit = limit;
  if (!atEnd && old == w) {
    std::memcpy(ctx.heap.bytes(coll.slot) + byteAt, enc, w);
  } else if (!atEnd && !inReserve) {
    // Overwrites a written character with one of another width: the count stays.
    if (!replaceString(ctx, self, coll, byteAt, enc, w, byteAt + old, total - byteAt - old, 0)) {
      return Oop{};
    }
  } else if (!atEnd && std::uint64_t{byteAt} + w <= total &&
             std::all_of(p + byteAt, p + byteAt + w, [](unsigned char b) { return b < 0x80; })) {
    // A wide character takes w one-byte reserve characters: w - 1 fewer characters.
    std::memcpy(ctx.heap.bytes(coll.slot) + byteAt, enc, w);
    newLimit = limit - (w - 1);
  } else {
    // At the end, or the reserve is short: the bytes up to here, the character, a new reserve.
    const std::uint64_t used = std::uint64_t{byteAt} + w;
    std::uint64_t capacity = used * 2 < 16 ? 16 : used * 2;
    capacity = (capacity + 7) & ~std::uint64_t{7};
    // SPEC §3.6: writeLimit = pos + 1 + reserve stays a SmallInteger (pos < kSmiMax here): a
    // position written by reflection cuts the reserve instead.
    const auto room = static_cast<std::uint64_t>(kSmiMax - (pos + 1));
    const std::uint64_t reserve = std::min(capacity - used, room);
    if (!replaceString(ctx, self, coll, byteAt, enc, w, 0, 0, reserve)) {
      return Oop{};
    }
    newLimit = pos + 1 + static_cast<std::int64_t>(reserve);
  }
  noteWrite(ctx.heap, self.slot, pos + 1);
  if (limitKnown || newLimit != limit) {
    setWriteLimit(ctx.heap, self.slot, newLimit < pos + 1 ? pos + 1 : newLimit);
  }
  return val.slot;
}

Oop classFormat(const Heap& heap, Oop cls) {
  return cls.isHeap() ? heap.slotAt(cls, kClassSlotFormat) : Oop{};
}

// Sends at: i to coll. Empty Oop when the frames unwind (the send failed or aborted).
Oop sendAt(CallContext& ctx, Root& coll, std::int64_t i) {
  Oop idx = Oop::fromSmallInteger(i);
  const Oop e = send(ctx, coll.slot, ctx.wk.selAt_, &idx, 1, nullptr);
  return unwinding(ctx) ? Oop{} : e;
}

// SPEC §3.6: String contents. With the Kernel String>>at:, the first k characters' bytes in one
// pass; otherwise at: 1..k, each a Character. The class is coll's, or String for a Symbol.
Oop stringPrefix(CallContext& ctx, Root& self, Root& coll, std::int64_t k) {
  const Oop cls = ctx.wk.classOf(coll.slot);
  Root answerClass(ctx.roots, chainIncludes(ctx.heap, cls, ctx.wk.symbolClass) ? ctx.wk.stringClass
                                                                                 : cls);
  if (isBytes(ctx.heap, coll.slot) && findsNative(ctx, cls, ctx.wk.selAt_, ao_String_at_)) {
    const std::int64_t n =
        Str::byteOffsetOfChar(payload(ctx.heap, coll.slot), ctx.heap.size(coll.slot), k);
    if (n < 0) {
      return fail(ctx, self.slot, "at: index out of range");
    }
    const Oop s = allocateRetry(ctx, answerClass.slot, static_cast<std::uint32_t>(n), kFlagBytes);
    if (!s.isHeap()) {
      return Oop{};
    }
    if (n != 0) {
      std::memcpy(ctx.heap.bytes(s), payload(ctx.heap, coll.slot), static_cast<std::size_t>(n));
    }
    return s;
  }
  std::string out;
  for (std::int64_t i = 1; i <= k; ++i) {
    const Oop e = sendAt(ctx, coll, i);
    if (e.isEmpty()) {
      return Oop{};
    }
    unsigned char enc[4];
    const std::uint32_t w = e.isCharacter() ? Str::encodeUtf8(e.characterValue(), enc) : 0;
    if (w == 0 || out.size() + w > UINT32_MAX) {
      return fail(ctx, self.slot, "contents: element out of range");
    }
    out.append(reinterpret_cast<const char*>(enc), w);
  }
  const auto n = static_cast<std::uint32_t>(out.size());
  const Oop s = allocateRetry(ctx, answerClass.slot, n, kFlagBytes);
  if (s.isHeap() && n != 0) {
    std::memcpy(ctx.heap.bytes(s), out.data(), n);
  }
  return s;
}

// at: 1..k of coll into the indexable part of answer (its first index after inst named slots, or
// its bytes). A byte must be a SmallInteger from 0 to 255.
Oop fillFromAt(CallContext& ctx, Root& self, Root& coll, Root& answer, std::int64_t k,
               std::int64_t inst, bool bytes) {
  for (std::int64_t i = 1; i <= k; ++i) {
    const Oop e = sendAt(ctx, coll, i);
    if (e.isEmpty()) {
      return Oop{};
    }
    const auto at = static_cast<std::uint32_t>(inst + i - 1);
    if (!bytes) {
      ctx.heap.slotAtPut(answer.slot, at, e);
      continue;
    }
    if (!e.isSmallInteger() || e.smallIntegerValue() < 0 || e.smallIntegerValue() > 255) {
      return fail(ctx, self.slot, "contents: element out of range");
    }
    ctx.heap.bytes(answer.slot)[at] =
        static_cast<std::byte>(static_cast<unsigned char>(e.smallIntegerValue()));
  }
  return answer.slot;
}

// SPEC §3.6: the first k elements of the stream's collection, taken with at:, in a new collection
// of its kind: a String (String for a Symbol); the same class for an indexable ArrayedCollection
// (instSize named slots left nil); an OrderedCollection; otherwise an Array. self is the stream.
Oop copyPrefix(CallContext& ctx, Root& self, Root& coll, std::int64_t k) {
  if (k < 0) {
    k = 0;
  }
  if (isStringy(ctx, coll.slot)) {
    return stringPrefix(ctx, self, coll, k);
  }
  Root cls(ctx.roots, ctx.wk.classOf(coll.slot));
  const Oop fmt = classFormat(ctx.heap, cls.slot);
  const bool arrayed = chainIncludes(ctx.heap, cls.slot, ctx.wk.arrayedCollectionClass) &&
                       Format::isIndexable(fmt);
  const std::int64_t inst = arrayed && !Format::isBytes(fmt) ? Format::instSize(fmt) : 0;
  if (k > static_cast<std::int64_t>(UINT32_MAX) - inst) {
    ctx.heap.setOutOfMemory();
    return Oop{};
  }
  if (arrayed) {
    const bool bytes = Format::isBytes(fmt);
    Root answer(ctx.roots, allocateRetry(ctx, cls.slot, static_cast<std::uint32_t>(inst + k),
                                         bytes ? kFlagBytes : 0));
    if (!answer.slot.isHeap()) {
      return Oop{};
    }
    return fillFromAt(ctx, self, coll, answer, k, inst, bytes);
  }
  Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, static_cast<std::uint32_t>(k), 0));
  if (!arr.slot.isHeap() || fillFromAt(ctx, self, coll, arr, k, 0, false).isEmpty()) {
    return Oop{};
  }
  if (!chainIncludes(ctx.heap, cls.slot, ctx.wk.orderedCollectionClass)) {
    return arr.slot;
  }
  const Oop oc = allocateInstance(ctx, ctx.wk.orderedCollectionClass, 3);
  if (!oc.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(oc, kOcArray, arr.slot);
  ctx.heap.slotAtPut(oc, kOcFirst, Oop::fromSmallInteger(1));
  ctx.heap.slotAtPut(oc, kOcLast, Oop::fromSmallInteger(k));
  return oc;
}

// SPEC §3.6: a global's name is a Symbol or a String. False when key is neither.
bool globalName(CallContext& ctx, Oop key, std::string* out) {
  if (!isStringy(ctx, key)) {
    return false;
  }
  *out = Str::toUtf8(ctx.heap, key);
  return true;
}

// The value Smalltalk binds to key, or the empty Oop when key names nothing it binds.
Oop globalAt(CallContext& ctx, Oop key) {
  std::string name;
  return globalName(ctx, key, &name) ? Globals::lookup(ctx.wk, ctx.wk.findSymbol(name)) : Oop{};
}

void callTranscriptHook(CallContext& ctx, Oop value) {
  if (ctx.transcriptHook != nullptr) {
    ctx.transcriptHook(ctx, value);
  }
}

Oop forwardTranscriptClass(CallContext& ctx, Oop receiver, const char* selector, const Oop* args,
                           std::uint32_t argc) {
  Root self(ctx.roots, receiver);
  if (argc == 0) {
    send(ctx, ctx.wk.transcript, ctx.wk.intern(selector), nullptr, 0, nullptr);
    return self.slot;
  }
  Root arg(ctx.roots, args[0]);
  send(ctx, ctx.wk.transcript, ctx.wk.intern(selector), &arg.slot, 1, nullptr);
  return self.slot;
}

Oop selNextPut(WellKnown& wk) { return wk.intern("nextPut:"); }

}  // namespace

Oop ao_Stream_next(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return ao_Object_subclassResponsibility(ctx, receiver, nullptr, 0);
}

Oop ao_Stream_nextPut_(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return ao_Object_subclassResponsibility(ctx, receiver, nullptr, 0);
}

Oop ao_Stream_nextPutAll_each(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root stream(ctx.roots, ctx.heap.slotAt(receiver, kBlockHome));
  Root elt(ctx.roots, args[0]);
  return send(ctx, stream.slot, selNextPut(ctx.wk), &elt.slot, 1, nullptr);
}

Oop ao_Stream_nextPutAll_(CallContext& ctx, const Oop& receiver, const Oop* args,
                          std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root coll(ctx.roots, args[0]);
  Root thunk(ctx.roots, makeNativeBlock(ctx, ao_Stream_nextPutAll_each, 1));
  if (!thunk.slot.isHeap()) {
    return coll.slot;
  }
  ctx.heap.slotAtPut(thunk.slot, kBlockHome, self.slot);
  send(ctx, coll.slot, ctx.wk.selDo_, &thunk.slot, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return coll.slot;
}

Oop ao_Stream_cr(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Oop nl = Oop::fromCharacter(static_cast<char32_t>('\n'));
  send(ctx, self.slot, selNextPut(ctx.wk), &nl, 1, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return self.slot;
}

Oop ao_PositionableStream_on_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Root coll(ctx.roots, args[0]);
  const auto inst = Format::instSize(ctx.heap.slotAt(cls.slot, kClassSlotFormat));
  if (inst < 3) {
    return Oop{};
  }
  Oop s = allocateRetry(ctx, cls.slot, static_cast<std::uint32_t>(inst), 0);
  if (!s.isHeap()) {
    return Oop{};
  }
  Root stream(ctx.roots, s);
  ctx.heap.slotAtPut(stream.slot, kStreamCollection, coll.slot);
  ctx.heap.slotAtPut(stream.slot, kStreamPosition, Oop::fromSmallInteger(0));
  const auto n = collectionSize(ctx, coll);
  if (unwinding(ctx)) {
    return Oop{};
  }
  const Oop limit = Oop::fromSmallInteger(n);
  ctx.heap.slotAtPut(stream.slot, kStreamReadLimit, limit);
  if (inst > static_cast<std::int64_t>(kStreamWriteLimit)) {
    ctx.heap.slotAtPut(stream.slot, kStreamWriteLimit, limit);
  }
  return stream.slot;
}

Oop ao_PositionableStream_next(CallContext& ctx, const Oop& receiver, const Oop*,
                               std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  const auto pos = smiOr(ctx.heap.slotAt(self.slot, kStreamPosition), 0);
  const auto limit = smiOr(ctx.heap.slotAt(self.slot, kStreamReadLimit), 0);
  if (pos >= limit) {
    return Oop::nil();
  }
  const auto neu = pos + 1;
  ctx.heap.slotAtPut(self.slot, kStreamPosition, Oop::fromSmallInteger(neu));
  Root coll(ctx.roots, ctx.heap.slotAt(self.slot, kStreamCollection));
  Oop idx = Oop::fromSmallInteger(neu);
  return send(ctx, coll.slot, ctx.wk.selAt_, &idx, 1, nullptr);
}

// SPEC §3.6: writes the position + 1st element, then advances position and raises readLimit. A
// String whose class finds the Kernel String>>at:put: is written as UTF-8 (stringNextPut); an Array
// grows by doubling; anything else gets at:put:.
Oop ao_WriteStream_nextPut_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root val(ctx.roots, args[0]);
  Root coll(ctx.roots, ctx.heap.slotAt(self.slot, kStreamCollection));
  if (!coll.slot.isHeap()) {
    return fail(ctx, self.slot, "nextPut: no collection");
  }
  const auto pos = smiOr(ctx.heap.slotAt(self.slot, kStreamPosition), 0);
  // position は Smalltalk から書き換えられる。+1 が SmallInteger を超えるなら失敗する。
  if (pos >= kSmiMax) {
    return fail(ctx, self.slot, "nextPut: position out of range");
  }
  // SPEC §3.6: the UTF-8 writes stand in for at:put: and size, so both must be the Kernel String's.
  if (isBytes(ctx.heap, coll.slot) &&
      findsNative(ctx, ctx.wk.classOf(coll.slot), ctx.wk.selAt_put_, ao_String_at_put_) &&
      findsNative(ctx, ctx.wk.classOf(coll.slot), ctx.wk.selSize, ao_String_size)) {
    if (pos < 0) {
      return fail(ctx, self.slot, "nextPut: position out of range");
    }
    return stringNextPut(ctx, self, coll, val, pos);
  }
  const auto neu = pos + 1;
  const auto n = collectionSize(ctx, coll);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (neu > n) {
    if (isArray(ctx, coll.slot)) {
      coll.slot = growArray(ctx, coll, neu);
      if (!coll.slot.isHeap()) {
        return Oop{};
      }
      ctx.heap.slotAtPut(self.slot, kStreamCollection, coll.slot);
      if (hasWriteLimit(ctx.heap, self.slot)) {
        ctx.heap.slotAtPut(self.slot, kStreamWriteLimit,
                           Oop::fromSmallInteger(static_cast<std::int64_t>(ctx.heap.size(coll.slot))));
      }
    } else if (isStringy(ctx, coll.slot) && val.slot.isCharacter() && neu == n + 1) {
      const Oop grown = stringAppendChar(ctx, coll, val.slot.characterValue());
      if (!grown.isHeap()) {
        // No room is out of memory (the empty Oop says so); otherwise no UTF-8 for the value.
        return ctx.heap.outOfMemory() ? Oop{} : fail(ctx, self.slot, "nextPut: value out of range");
      }
      coll.slot = grown;
      ctx.heap.slotAtPut(self.slot, kStreamCollection, coll.slot);
      noteWrite(ctx.heap, self.slot, neu);
      return val.slot;
    } else {
      return fail(ctx, self.slot, "nextPut: past end");
    }
  }
  Oop put[2] = {Oop::fromSmallInteger(neu), val.slot};
  send(ctx, coll.slot, ctx.wk.selAt_put_, put, 2, nullptr);
  if (unwinding(ctx)) {
    return Oop{};
  }
  noteWrite(ctx.heap, self.slot, neu);
  return val.slot;
}

Oop ao_PositionableStream_position(CallContext& ctx, const Oop& receiver, const Oop*,
                                   std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kStreamPosition);
}

// SPEC §3.6: clamps to [0, max(readLimit, position)], never into the reserve a stream-made String
// or a doubled Array carries past readLimit (Squeak's WriteStream>>position:).
Oop ao_PositionableStream_position_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                    std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  if (!args[0].isSmallInteger()) {
    return fail(ctx, receiver, "position: not an integer");
  }
  auto v = args[0].smallIntegerValue();
  if (v < 0) {
    v = 0;
  }
  const auto limit = std::max(smiOr(ctx.heap.slotAt(receiver, kStreamReadLimit), 0),
                              smiOr(ctx.heap.slotAt(receiver, kStreamPosition), 0));
  if (v > limit) {
    v = limit;
  }
  ctx.heap.slotAtPut(receiver, kStreamPosition, Oop::fromSmallInteger(v));
  return receiver;
}

Oop ao_PositionableStream_reset(CallContext& ctx, const Oop& receiver, const Oop*,
                                std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(receiver, kStreamPosition, Oop::fromSmallInteger(0));
  return receiver;
}

Oop ao_PositionableStream_contents(CallContext& ctx, const Oop& receiver, const Oop*,
                                   std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root coll(ctx.roots, ctx.heap.slotAt(self.slot, kStreamCollection));
  const auto n = smiOr(ctx.heap.slotAt(self.slot, kStreamReadLimit), 0);
  return copyPrefix(ctx, self, coll, n);
}

Oop ao_WriteStream_contents(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root coll(ctx.roots, ctx.heap.slotAt(self.slot, kStreamCollection));
  const auto n = smiOr(ctx.heap.slotAt(self.slot, kStreamPosition), 0);
  return copyPrefix(ctx, self, coll, n);
}

// SPEC §3.6: up to the larger of readLimit and position (Blue Book), so a reset stream still
// answers what was written.
Oop ao_ReadWriteStream_contents(CallContext& ctx, const Oop& receiver, const Oop*,
                                std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root coll(ctx.roots, ctx.heap.slotAt(self.slot, kStreamCollection));
  const auto read = smiOr(ctx.heap.slotAt(self.slot, kStreamReadLimit), 0);
  const auto pos = smiOr(ctx.heap.slotAt(self.slot, kStreamPosition), 0);
  return copyPrefix(ctx, self, coll, read > pos ? read : pos);
}

// SPEC §3.6: a ReadStream does not write its collection.
Oop ao_ReadStream_nextPut_(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return ao_Object_shouldNotImplement(ctx, receiver, nullptr, 0);
}

Oop ao_Transcript_nextPut_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  callTranscriptHook(ctx, args[0]);
  return args[0];
}

Oop ao_Transcript_nextPutAll_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  callTranscriptHook(ctx, args[0]);
  return args[0];
}

Oop ao_Transcript_show_(CallContext& ctx, const Oop& receiver, const Oop* args,
                        std::uint32_t argc) {
  ao_Transcript_nextPutAll_(ctx, receiver, args, argc);
  return receiver;
}

Oop ao_Transcript_cr(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  return ao_Stream_cr(ctx, receiver, args, argc);
}

Oop ao_Transcript_clear(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  callTranscriptHook(ctx, Oop::nil());
  return receiver;
}

Oop ao_Transcript_class_nextPut_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                 std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return forwardTranscriptClass(ctx, receiver, "nextPut:", args, argc);
}

Oop ao_Transcript_class_nextPutAll_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                    std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return forwardTranscriptClass(ctx, receiver, "nextPutAll:", args, argc);
}

Oop ao_Transcript_class_show_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return forwardTranscriptClass(ctx, receiver, "show:", args, argc);
}

Oop ao_Transcript_class_cr(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return forwardTranscriptClass(ctx, receiver, "cr", nullptr, 0);
}

Oop ao_Transcript_class_clear(CallContext& ctx, const Oop& receiver, const Oop*,
                              std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return forwardTranscriptClass(ctx, receiver, "clear", nullptr, 0);
}

// SPEC §3.6: the SmalltalkImage natives read and write Smalltalk, whatever the receiver.
Oop ao_SmalltalkImage_at_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  const Oop value = globalAt(ctx, args[0]);
  if (!value.isEmpty()) {
    return value;
  }
  std::string name;
  if (!globalName(ctx, args[0], &name)) {
    return abortEvaluation(ctx, "key not found");
  }
  return abortEvaluation(ctx, std::string_view("key not found: #" + name));
}

Oop ao_SmalltalkImage_at_put_(CallContext& ctx, const Oop&, const Oop* args, std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  std::string name;
  if (!globalName(ctx, args[0], &name)) {
    return abortEvaluation(ctx, "key must be a Symbol or String");
  }
  if (ctx.wk.isFixedGlobal(name)) {
    return abortEvaluation(ctx, std::string_view("cannot rebind Kernel global: " + name));
  }
  if (WellKnown::isPseudoVariableName(name)) {
    return abortEvaluation(ctx, std::string_view("cannot bind pseudo-variable: " + name));
  }
  // define does not collect. It fails only at old's max, and flags out of memory for the send.
  if (!ctx.wk.define(name, args[1])) {
    return Oop{};
  }
  return args[1];
}

Oop ao_SmalltalkImage_at_ifAbsent_(CallContext& ctx, const Oop&, const Oop* args,
                                   std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  const Oop value = globalAt(ctx, args[0]);
  if (!value.isEmpty()) {
    return value;
  }
  Oop answer;
  if (!callBlock(ctx, args[1], nullptr, 0, &answer)) {
    return Oop{};
  }
  return answer;
}

Oop ao_SmalltalkImage_includesKey_(CallContext& ctx, const Oop&, const Oop* args,
                                   std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return globalAt(ctx, args[0]).isEmpty() ? Oop::false_() : Oop::true_();
}

Oop ao_SmalltalkImage_globals(CallContext&, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return receiver;
}

namespace kernel {

void installStream(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.streamClass, "next", 0, "ao_Stream_next", ao_Stream_next);
  putNative(heap, wk, wk.streamClass, "nextPut:", 1, "ao_Stream_nextPut_", ao_Stream_nextPut_);
  putNative(heap, wk, wk.streamClass, "nextPutAll:", 1, "ao_Stream_nextPutAll_",
            ao_Stream_nextPutAll_);
  // SPEC §3.11: the thunk nextPutAll: passes to do: takes this name, so an image holding one that
  // a user's do: kept rebinds it at load.
  (void)NativeRegistry::addNamed("ao_Stream_nextPutAll_each", ao_Stream_nextPutAll_each, nullptr);
  putNative(heap, wk, wk.streamClass, "cr", 0, "ao_Stream_cr", ao_Stream_cr);

  putNative(heap, wk, wk.positionableStreamMetaclass, "on:", 1, "ao_PositionableStream_on_",
            ao_PositionableStream_on_);
  putNative(heap, wk, wk.positionableStreamClass, "next", 0, "ao_PositionableStream_next",
            ao_PositionableStream_next);
  putNative(heap, wk, wk.positionableStreamClass, "position", 0, "ao_PositionableStream_position",
            ao_PositionableStream_position);
  putNative(heap, wk, wk.positionableStreamClass, "position:", 1, "ao_PositionableStream_position_",
            ao_PositionableStream_position_);
  putNative(heap, wk, wk.positionableStreamClass, "reset", 0, "ao_PositionableStream_reset",
            ao_PositionableStream_reset);
  putNative(heap, wk, wk.positionableStreamClass, "contents", 0, "ao_PositionableStream_contents",
            ao_PositionableStream_contents);

  // SPEC §3.6: nextPut: is WriteStream's (ReadWriteStream inherits it); a ReadStream refuses it.
  putNative(heap, wk, wk.readStreamClass, "nextPut:", 1, "ao_ReadStream_nextPut_",
            ao_ReadStream_nextPut_);
  putNative(heap, wk, wk.writeStreamClass, "nextPut:", 1, "ao_WriteStream_nextPut_",
            ao_WriteStream_nextPut_);
  putNative(heap, wk, wk.writeStreamClass, "contents", 0, "ao_WriteStream_contents",
            ao_WriteStream_contents);
  putNative(heap, wk, wk.readWriteStreamClass, "contents", 0, "ao_ReadWriteStream_contents",
            ao_ReadWriteStream_contents);

  putNative(heap, wk, wk.transcriptClass, "nextPut:", 1, "ao_Transcript_nextPut_",
            ao_Transcript_nextPut_);
  putNative(heap, wk, wk.transcriptClass, "nextPutAll:", 1, "ao_Transcript_nextPutAll_",
            ao_Transcript_nextPutAll_);
  putNative(heap, wk, wk.transcriptClass, "show:", 1, "ao_Transcript_show_", ao_Transcript_show_);
  putNative(heap, wk, wk.transcriptClass, "cr", 0, "ao_Transcript_cr", ao_Transcript_cr);
  putNative(heap, wk, wk.transcriptClass, "clear", 0, "ao_Transcript_clear", ao_Transcript_clear);

  putNative(heap, wk, wk.transcriptMetaclass, "nextPut:", 1, "ao_Transcript_class_nextPut_",
            ao_Transcript_class_nextPut_);
  putNative(heap, wk, wk.transcriptMetaclass, "nextPutAll:", 1, "ao_Transcript_class_nextPutAll_",
            ao_Transcript_class_nextPutAll_);
  putNative(heap, wk, wk.transcriptMetaclass, "show:", 1, "ao_Transcript_class_show_",
            ao_Transcript_class_show_);
  putNative(heap, wk, wk.transcriptMetaclass, "cr", 0, "ao_Transcript_class_cr",
            ao_Transcript_class_cr);
  putNative(heap, wk, wk.transcriptMetaclass, "clear", 0, "ao_Transcript_class_clear",
            ao_Transcript_class_clear);

  putNative(heap, wk, wk.smalltalkImageClass, "at:", 1, "ao_SmalltalkImage_at_",
            ao_SmalltalkImage_at_);
  putNative(heap, wk, wk.smalltalkImageClass, "at:put:", 2, "ao_SmalltalkImage_at_put_",
            ao_SmalltalkImage_at_put_);
  putNative(heap, wk, wk.smalltalkImageClass, "at:ifAbsent:", 2, "ao_SmalltalkImage_at_ifAbsent_",
            ao_SmalltalkImage_at_ifAbsent_);
  putNative(heap, wk, wk.smalltalkImageClass, "includesKey:", 1, "ao_SmalltalkImage_includesKey_",
            ao_SmalltalkImage_includesKey_);
  putNative(heap, wk, wk.smalltalkImageClass, "globals", 0, "ao_SmalltalkImage_globals",
            ao_SmalltalkImage_globals);
}

}  // namespace kernel
}  // namespace ao

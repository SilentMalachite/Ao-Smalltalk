#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Format.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Globals.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"

#include <cstring>
#include <string>
#include <string_view>

namespace ao {
namespace {

constexpr std::uint32_t kStreamCollection = 0;
constexpr std::uint32_t kStreamPosition   = 1;
constexpr std::uint32_t kStreamReadLimit  = 2;
constexpr std::uint32_t kStreamWriteLimit = 3;

// receiver はルート済みスロット。メッセージの割り当てで GC が走っても正しい。
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

bool isBytes(const Heap& heap, Oop obj) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) != 0;
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

std::uint32_t encodeUtf8(char32_t cp, unsigned char out[4]) {
  if (cp <= 0x7F) {
    out[0] = static_cast<unsigned char>(cp);
    return 1;
  }
  if (cp <= 0x7FF) {
    out[0] = static_cast<unsigned char>(0xC0 | (cp >> 6));
    out[1] = static_cast<unsigned char>(0x80 | (cp & 0x3F));
    return 2;
  }
  if (cp <= 0xFFFF) {
    if (cp >= 0xD800 && cp <= 0xDFFF) {
      return 0;
    }
    out[0] = static_cast<unsigned char>(0xE0 | (cp >> 12));
    out[1] = static_cast<unsigned char>(0x80 | ((cp >> 6) & 0x3F));
    out[2] = static_cast<unsigned char>(0x80 | (cp & 0x3F));
    return 3;
  }
  if (cp <= 0x10FFFF) {
    out[0] = static_cast<unsigned char>(0xF0 | (cp >> 18));
    out[1] = static_cast<unsigned char>(0x80 | ((cp >> 12) & 0x3F));
    out[2] = static_cast<unsigned char>(0x80 | ((cp >> 6) & 0x3F));
    out[3] = static_cast<unsigned char>(0x80 | (cp & 0x3F));
    return 4;
  }
  return 0;
}

std::int64_t collectionSize(CallContext& ctx, Root& coll) {
  const Oop n = send(ctx, coll.slot, ctx.wk.selSize, nullptr, 0, nullptr);
  return smiOr(n, 0);
}

Oop growArray(CallContext& ctx, Root& arr, std::uint32_t minSize) {
  const std::uint32_t old = arr.slot.isHeap() ? ctx.heap.size(arr.slot) : 0;
  std::uint32_t neu = old == 0 ? 1u : old * 2u;
  if (neu < minSize) {
    neu = minSize;
  }
  Oop n = allocateRetry(ctx, ctx.wk.arrayClass, neu, 0);
  if (!n.isHeap()) {
    return Oop{};
  }
  for (std::uint32_t i = 0; i < old; ++i) {
    ctx.heap.slotAtPut(n, i, ctx.heap.slotAt(arr.slot, i));
  }
  return n;
}

Oop stringAppendChar(CallContext& ctx, Root& str, char32_t cp) {
  unsigned char enc[4];
  const auto n = encodeUtf8(cp, enc);
  if (n == 0 || !isBytes(ctx.heap, str.slot)) {
    return Oop{};
  }
  const auto old = ctx.heap.size(str.slot);
  Oop neu = allocateRetry(ctx, ctx.wk.stringClass, old + n, kFlagBytes);
  if (!neu.isHeap()) {
    return Oop{};
  }
  if (old != 0) {
    std::memcpy(ctx.heap.bytes(neu), ctx.heap.header(str.slot) + 1, old);
  }
  std::memcpy(ctx.heap.bytes(neu) + old, enc, n);
  return neu;
}

Oop copyPrefix(CallContext& ctx, Root& coll, std::int64_t n) {
  if (n < 0) {
    n = 0;
  }
  if (isStringy(ctx, coll.slot)) {
    std::string out;
    for (std::int64_t i = 1; i <= n; ++i) {
      const Oop ch = Str::at(ctx.heap, coll.slot, i);
      if (!ch.isCharacter()) {
        break;
      }
      unsigned char enc[4];
      const auto k = encodeUtf8(ch.characterValue(), enc);
      if (k == 0) {
        break;
      }
      out.append(reinterpret_cast<char*>(enc), k);
    }
    const auto bytes = static_cast<std::uint32_t>(out.size());
    Oop s = allocateRetry(ctx, ctx.wk.stringClass, bytes, kFlagBytes);
    if (!s.isHeap()) {
      return Oop{};
    }
    if (bytes != 0) {
      std::memcpy(ctx.heap.bytes(s), out.data(), bytes);
    }
    return s;
  }
  const auto un = static_cast<std::uint32_t>(n);
  Oop a = allocateRetry(ctx, ctx.wk.arrayClass, un, 0);
  if (!a.isHeap()) {
    return Oop{};
  }
  Root dst(ctx.roots, a);
  const std::uint32_t avail =
      coll.slot.isHeap() && !isBytes(ctx.heap, coll.slot) ? ctx.heap.size(coll.slot) : 0;
  const std::uint32_t copyN = un < avail ? un : avail;
  for (std::uint32_t i = 0; i < copyN; ++i) {
    ctx.heap.slotAtPut(dst.slot, i, ctx.heap.slotAt(coll.slot, i));
  }
  return dst.slot;
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

Oop ao_PositionableStream_nextPut_(CallContext& ctx, const Oop& receiver, const Oop* args,
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
  const auto neu = pos + 1;
  const auto n = collectionSize(ctx, coll);
  if (unwinding(ctx)) {
    return Oop{};
  }
  if (neu > n) {
    if (isArray(ctx, coll.slot)) {
      coll.slot = growArray(ctx, coll, static_cast<std::uint32_t>(neu));
      if (!coll.slot.isHeap()) {
        return Oop{};
      }
      ctx.heap.slotAtPut(self.slot, kStreamCollection, coll.slot);
      if (hasWriteLimit(ctx.heap, self.slot)) {
        ctx.heap.slotAtPut(self.slot, kStreamWriteLimit,
                           Oop::fromSmallInteger(static_cast<std::int64_t>(ctx.heap.size(coll.slot))));
      }
    } else if (isStringy(ctx, coll.slot) && val.slot.isCharacter() && neu == n + 1) {
      coll.slot = stringAppendChar(ctx, coll, val.slot.characterValue());
      if (!coll.slot.isHeap()) {
        return fail(ctx, self.slot, "nextPut: value out of range");
      }
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
  const auto limit = hasWriteLimit(ctx.heap, receiver)
                         ? smiOr(ctx.heap.slotAt(receiver, kStreamWriteLimit), 0)
                         : smiOr(ctx.heap.slotAt(receiver, kStreamReadLimit), 0);
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
  return copyPrefix(ctx, coll, n);
}

Oop ao_WriteStream_contents(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  Root self(ctx.roots, receiver);
  Root coll(ctx.roots, ctx.heap.slotAt(self.slot, kStreamCollection));
  const auto n = smiOr(ctx.heap.slotAt(self.slot, kStreamPosition), 0);
  return copyPrefix(ctx, coll, n);
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
  putNative(heap, wk, wk.positionableStreamClass, "nextPut:", 1, "ao_PositionableStream_nextPut_",
            ao_PositionableStream_nextPut_);
  putNative(heap, wk, wk.positionableStreamClass, "position", 0, "ao_PositionableStream_position",
            ao_PositionableStream_position);
  putNative(heap, wk, wk.positionableStreamClass, "position:", 1, "ao_PositionableStream_position_",
            ao_PositionableStream_position_);
  putNative(heap, wk, wk.positionableStreamClass, "reset", 0, "ao_PositionableStream_reset",
            ao_PositionableStream_reset);
  putNative(heap, wk, wk.positionableStreamClass, "contents", 0, "ao_PositionableStream_contents",
            ao_PositionableStream_contents);

  putNative(heap, wk, wk.writeStreamClass, "contents", 0, "ao_WriteStream_contents",
            ao_WriteStream_contents);

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

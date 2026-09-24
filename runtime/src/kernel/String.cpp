#include "ao/kernel/Install.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"
#include "ao/Symbol.hpp"

#include <cstring>
#include <string>

namespace ao {
namespace {

struct Utf8Step {
  char32_t cp;
  std::uint32_t nbytes;
};

Utf8Step decodeUtf8(const unsigned char* p, std::uint32_t remaining) {
  if (remaining == 0) {
    return {0, 0};
  }
  const unsigned char b0 = p[0];
  if (b0 <= 0x7F) {
    return {static_cast<char32_t>(b0), 1};
  }
  if ((b0 & 0xE0) == 0xC0 && remaining >= 2 && (p[1] & 0xC0) == 0x80) {
    const char32_t cp =
        (static_cast<char32_t>(b0 & 0x1F) << 6) | static_cast<char32_t>(p[1] & 0x3F);
    if (cp >= 0x80) {
      return {cp, 2};
    }
  } else if ((b0 & 0xF0) == 0xE0 && remaining >= 3 && (p[1] & 0xC0) == 0x80 &&
             (p[2] & 0xC0) == 0x80) {
    const char32_t cp = (static_cast<char32_t>(b0 & 0x0F) << 12) |
                        (static_cast<char32_t>(p[1] & 0x3F) << 6) |
                        static_cast<char32_t>(p[2] & 0x3F);
    if (cp >= 0x800 && (cp < 0xD800 || cp > 0xDFFF)) {
      return {cp, 3};
    }
  } else if ((b0 & 0xF8) == 0xF0 && remaining >= 4 && (p[1] & 0xC0) == 0x80 &&
             (p[2] & 0xC0) == 0x80 && (p[3] & 0xC0) == 0x80) {
    const char32_t cp = (static_cast<char32_t>(b0 & 0x07) << 18) |
                        (static_cast<char32_t>(p[1] & 0x3F) << 12) |
                        (static_cast<char32_t>(p[2] & 0x3F) << 6) |
                        static_cast<char32_t>(p[3] & 0x3F);
    if (cp >= 0x10000 && cp <= 0x10FFFF) {
      return {cp, 4};
    }
  }
  return {static_cast<char32_t>(b0), 1};
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

const unsigned char* bytePayload(const Heap& heap, Oop str) {
  return reinterpret_cast<const unsigned char*>(heap.header(str) + 1);
}

bool isBytes(const Heap& heap, Oop obj) {
  return obj.isHeap() && (heap.flags(obj) & kFlagBytes) != 0;
}

bool isStringy(CallContext& ctx, Oop obj) {
  if (!obj.isHeap()) {
    return false;
  }
  return chainIncludes(ctx.heap, ctx.wk.classOf(obj), ctx.wk.stringClass);
}

// receiver はルート済みスロット。メッセージの割り当てで GC が走っても正しい。
Oop fail(CallContext& ctx, const Oop& receiver, std::string_view msg) {
  Oop s = Str::fromUtf8(ctx, msg);
  return NativeMethod::invoke(ctx, ao_Object_error_, receiver, &s, 1);
}

}  // namespace

namespace Str {

std::uint32_t codePointCount(Heap& heap, Oop str) {
  if (!isBytes(heap, str)) {
    return 0;
  }
  const auto n = heap.size(str);
  const unsigned char* p = bytePayload(heap, str);
  std::uint32_t count = 0;
  std::uint32_t i = 0;
  while (i < n) {
    const Utf8Step step = decodeUtf8(p + i, n - i);
    if (step.nbytes == 0) {
      break;
    }
    i += step.nbytes;
    ++count;
  }
  return count;
}

Oop at(Heap& heap, Oop str, std::int64_t oneBased) {
  if (!isBytes(heap, str) || oneBased < 1) {
    return Oop{};
  }
  const auto n = heap.size(str);
  const unsigned char* p = bytePayload(heap, str);
  std::int64_t idx = 1;
  std::uint32_t i = 0;
  while (i < n) {
    const Utf8Step step = decodeUtf8(p + i, n - i);
    if (step.nbytes == 0) {
      break;
    }
    if (idx == oneBased) {
      return Oop::fromCharacter(step.cp);
    }
    i += step.nbytes;
    ++idx;
  }
  return Oop{};
}

}  // namespace Str

Oop ao_String_size(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  return Oop::fromSmallInteger(static_cast<std::int64_t>(Str::codePointCount(ctx.heap, receiver)));
}

Oop ao_String_at_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (!args[0].isSmallInteger()) {
    return fail(ctx, receiver, "at: index out of range");
  }
  const Oop ch = Str::at(ctx.heap, receiver, args[0].smallIntegerValue());
  if (!ch.isCharacter()) {
    return fail(ctx, receiver, "at: index out of range");
  }
  return ch;
}

Oop ao_String_at_put_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  if (!isBytes(ctx.heap, receiver) || !args[0].isSmallInteger() || !args[1].isCharacter()) {
    return fail(ctx, receiver, "at:put: index out of range");
  }
  const auto oneBased = args[0].smallIntegerValue();
  const auto n = ctx.heap.size(receiver);
  auto* p = reinterpret_cast<unsigned char*>(ctx.heap.bytes(receiver));
  std::int64_t idx = 1;
  std::uint32_t i = 0;
  while (i < n) {
    const Utf8Step step = decodeUtf8(p + i, n - i);
    if (step.nbytes == 0) {
      break;
    }
    if (idx == oneBased) {
      unsigned char enc[4];
      const auto encN = encodeUtf8(args[1].characterValue(), enc);
      if (encN == 0) {
        return fail(ctx, receiver, "at:put: value out of range");
      }
      const auto newSize = n - step.nbytes + encN;
      ObjectHeader* h = ctx.heap.header(receiver);
      const std::size_t oldBytes = ctx.heap.objectBytes(h);
      ObjectHeader probe = *h;
      probe.size = newSize;
      if (ctx.heap.objectBytes(&probe) != oldBytes) {
        return fail(ctx, receiver, "at:put: UTF-8 width mismatch");
      }
      if (encN != step.nbytes) {
        const auto tail = n - (i + step.nbytes);
        if (tail != 0) {
          std::memmove(p + i + encN, p + i + step.nbytes, tail);
        }
      }
      std::memcpy(p + i, enc, encN);
      h->size = newSize;
      return args[1];
    }
    i += step.nbytes;
    ++idx;
  }
  return fail(ctx, receiver, "at:put: index out of range");
}

Oop ao_String_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (receiver == args[0]) {
    return Oop::true_();
  }
  if (!isBytes(ctx.heap, receiver) || !isStringy(ctx, args[0])) {
    return Oop::false_();
  }
  const auto n = ctx.heap.size(receiver);
  if (n != ctx.heap.size(args[0])) {
    return Oop::false_();
  }
  if (n == 0) {
    return Oop::true_();
  }
  return std::memcmp(bytePayload(ctx.heap, receiver), bytePayload(ctx.heap, args[0]), n) == 0
             ? Oop::true_()
             : Oop::false_();
}


// SPEC §3.6: a value hash of the bytes. Symbol inherits it: `#abc = 'abc'`, so both hash alike.
Oop ao_String_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!isBytes(ctx.heap, receiver)) {
    return ao_Object_identityHash(ctx, receiver, args, argc);
  }
  const std::uint64_t h =
      valueHashBytes(kValueHashSeed, bytePayload(ctx.heap, receiver), ctx.heap.size(receiver));
  return Oop::fromSmallInteger(valueHashFold(h));
}

Oop ao_String_asSymbol(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isBytes(ctx.heap, receiver)) {
    return Oop{};
  }
  // Symbol は intern 表から外れないので、nursery を通さず old に置く（GC しない）。
  const Oop sym = ctx.wk.internTenured(Str::toUtf8(ctx.heap, receiver));
  if (!sym.isHeap()) {
    ctx.heap.setOutOfMemory();
  }
  return sym;
}

Oop ao_Symbol_asString(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isBytes(ctx.heap, receiver)) {
    return Oop{};
  }
  const auto n = ctx.heap.size(receiver);
  Oop s = allocateRetry(ctx, ctx.wk.stringClass, n, kFlagBytes);
  if (!s.isHeap()) {
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(ctx.heap.bytes(s), bytePayload(ctx.heap, receiver), n);
  }
  return s;
}

Oop ao_Symbol_at_put_(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  return ao_Object_shouldNotImplement(ctx, receiver, nullptr, 0);
}

Oop ao_Symbol_basicAt_put_(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  return ao_Object_shouldNotImplement(ctx, receiver, nullptr, 0);
}

Oop ao_String_printString(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  // Symbol subclasses String. Only a String quotes its bytes; every other class stays the class name.
  if (ctx.wk.classOf(receiver) != ctx.wk.stringClass || !isBytes(ctx.heap, receiver)) {
    return ao_Object_printString(ctx, receiver, nullptr, 0);
  }
  const auto n = ctx.heap.size(receiver);
  const unsigned char* bytes = bytePayload(ctx.heap, receiver);
  std::string out;
  out.push_back('\'');
  for (std::uint32_t i = 0; i < n; ++i) {
    out.push_back(static_cast<char>(bytes[i]));
    if (bytes[i] == static_cast<unsigned char>('\'')) {
      out.push_back('\'');
    }
  }
  out.push_back('\'');
  return Str::fromUtf8(ctx, out);
}

namespace kernel {

void installString(Heap& heap, WellKnown& wk) {
  const Oop str = wk.stringClass;
  putNative(heap, wk, str, "size", 0, "ao_String_size", ao_String_size);
  putNative(heap, wk, str, "at:", 1, "ao_String_at_", ao_String_at_);
  putNative(heap, wk, str, "at:put:", 2, "ao_String_at_put_", ao_String_at_put_);
  putNative(heap, wk, str, "=", 1, "ao_String_equals", ao_String_equals);
  putNative(heap, wk, str, "hash", 0, "ao_String_hash", ao_String_hash);
  putNative(heap, wk, str, "asSymbol", 0, "ao_String_asSymbol", ao_String_asSymbol);
  putNative(heap, wk, str, "printString", 0, "ao_String_printString", ao_String_printString);
  putNative(heap, wk, wk.symbolClass, "asString", 0, "ao_Symbol_asString", ao_Symbol_asString);
  putNative(heap, wk, wk.symbolClass, "at:put:", 2, "ao_Symbol_at_put_", ao_Symbol_at_put_);
  putNative(heap, wk, wk.symbolClass, "basicAt:put:", 2, "ao_Symbol_basicAt_put_",
            ao_Symbol_basicAt_put_);
}

}  // namespace kernel
}  // namespace ao

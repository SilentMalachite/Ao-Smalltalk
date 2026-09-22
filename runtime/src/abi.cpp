#include "ao_abi.h"
#include "ao/Runtime.hpp"
#include "Session.hpp"

extern "C" int ao_version(char* buf, int buf_len) {
  return ao::version_string(buf, buf_len) == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_runtime_boot(void) {
  return ao::boot() == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_runtime_shutdown(void) {
  return ao::shutdown() == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_image_save(const char* path) {
  return ao::sessionImageSave(path) == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_image_load(const char* path) {
  return ao::sessionImageLoad(path) == 0 ? AO_OK : AO_ERR;
}

extern "C" int ao_filein_load_order(const char* path) {
  return ao::sessionFileInLoadOrder(path) == 0 ? AO_OK : AO_ERR;
}

namespace {

AoTranscriptFn g_transcriptFn = nullptr;
void* g_transcriptUser = nullptr;

int encodeTranscriptUtf8(char32_t cp, char out[4]) {
  if (cp <= 0x7F) {
    out[0] = static_cast<char>(cp);
    return 1;
  }
  if (cp <= 0x7FF) {
    out[0] = static_cast<char>(0xC0 | (cp >> 6));
    out[1] = static_cast<char>(0x80 | (cp & 0x3F));
    return 2;
  }
  if (cp <= 0xFFFF) {
    if (cp >= 0xD800 && cp <= 0xDFFF) {
      return 0;
    }
    out[0] = static_cast<char>(0xE0 | (cp >> 12));
    out[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    out[2] = static_cast<char>(0x80 | (cp & 0x3F));
    return 3;
  }
  if (cp <= 0x10FFFF) {
    out[0] = static_cast<char>(0xF0 | (cp >> 18));
    out[1] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
    out[2] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
    out[3] = static_cast<char>(0x80 | (cp & 0x3F));
    return 4;
  }
  return 0;
}

void deliverTranscript(ao::CallContext& ctx, ao::Oop value) {
  if (g_transcriptFn == nullptr) {
    return;
  }
  if (value.isNil()) {
    g_transcriptFn("", 0, 1, g_transcriptUser);
    return;
  }
  if (value.isCharacter()) {
    char buf[4];
    const int n = encodeTranscriptUtf8(value.characterValue(), buf);
    if (n == 0) {
      return;
    }
    g_transcriptFn(buf, n, 0, g_transcriptUser);
    return;
  }
  if (value.isHeap() && (ctx.heap.flags(value) & ao::kFlagBytes) != 0) {
    const auto n = ctx.heap.size(value);
    const char* bytes = "";
    if (n != 0) {
      bytes = reinterpret_cast<const char*>(ctx.heap.bytes(value));
    }
    g_transcriptFn(bytes, static_cast<int>(n), 0, g_transcriptUser);
  }
}

}  // namespace

extern "C" void ao_set_transcript_hook(AoTranscriptFn fn, void* user) {
  g_transcriptFn = fn;
  g_transcriptUser = user;
  ao::Session* s = ao::session();
  if (s == nullptr || s->ctx == nullptr) {
    return;
  }
  if (fn == nullptr) {
    s->ctx->transcriptHook = nullptr;
    return;
  }
  s->ctx->transcriptHook = [](ao::CallContext& ctx, ao::Oop value) {
    deliverTranscript(ctx, value);
  };
}

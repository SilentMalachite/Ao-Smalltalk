#include "ao_abi.h"
#include "ao/Compile.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Runtime.hpp"
#include "Session.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <string>
#include <string_view>

namespace {

// SPEC §3.10: no C++ exception leaves the ABI. body runs; an exception (std::bad_alloc, a host
// hook that throws) makes the call answer failed. An evaluation it cut short may leave the session
// half way; shutting it down and booting again is always safe.
template <typename Body>
int guarded(int failed, Body&& body) noexcept {
  try {
    return body();
  } catch (...) {
    return failed;
  }
}

// 1 while an ABI entry runs, else 0. A refused call leaves it as it found it.
std::atomic<int> g_entered{0};

// SPEC §3.10: the runtime is busy while the interpreter runs on the session's base context (a
// native calls back). Read only with the entry taken, so no other entry replaces the session
// meanwhile. No session: not busy.
bool runtimeBusy() {
  const ao::Session* s = ao::session();
  return s != nullptr && s->ctx != nullptr && ao::interpreterRunning(*s->ctx);
}

// Marks the ABI entries that run Smalltalk, call a hook or replace the session: boot, shutdown,
// save, load, filein, workspace reset, eval, accept. SPEC §3.10: the runtime is busy while one of
// them runs (a host hook calls back) or runtimeBusy says so; then entered() is false and the call
// answers AO_ERR without touching the session. The taking is one compare_exchange (0 to 1), so of
// two threads that enter at once only one gets in; the one that got in gives the entry back when
// runtimeBusy. This constructor is the one place for the test.
class AbiEntry {
 public:
  AbiEntry() {
    int idle = 0;
    entered_ = g_entered.compare_exchange_strong(idle, 1);
    if (entered_ && runtimeBusy()) {
      g_entered.store(0);
      entered_ = false;
    }
  }
  ~AbiEntry() {
    if (entered_) {
      g_entered.store(0);
    }
  }
  AbiEntry(const AbiEntry&) = delete;
  AbiEntry& operator=(const AbiEntry&) = delete;
  bool entered() const { return entered_; }

 private:
  bool entered_;
};

}  // namespace

extern "C" int ao_version(char* buf, int buf_len) {
  return guarded(AO_ERR, [&] {
    if (buf == nullptr || buf_len < 1) {
      return AO_ERR;
    }
    // SPEC §3.10: snprintf still NUL-terminates what it cut.
    return ao::version_string(buf, buf_len) == 0 ? AO_OK : AO_ERR_RANGE;
  });
}

extern "C" int ao_runtime_boot(void) {
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [] { return ao::boot() == 0 ? AO_OK : AO_ERR; });
}

extern "C" int ao_runtime_shutdown(void) {
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [] { return ao::shutdown() == 0 ? AO_OK : AO_ERR; });
}

extern "C" int ao_image_save(const char* path) {
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [&] { return ao::sessionImageSave(path) == 0 ? AO_OK : AO_ERR; });
}

extern "C" int ao_filein_load_order(const char* path) {
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [&] { return ao::sessionFileInLoadOrder(path) == 0 ? AO_OK : AO_ERR; });
}

namespace {

AoTranscriptFn g_transcriptFn = nullptr;
void* g_transcriptUser = nullptr;
AoInspectFn g_inspectFn = nullptr;
void* g_inspectUser = nullptr;

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

void clearSpan(AoSpan* err) {
  if (err == nullptr) {
    return;
  }
  err->start = 0;
  err->end = 0;
  err->message[0] = '\0';
}

void fillSpan(AoSpan* err, const ao::compiler::CompileError& error) {
  if (err == nullptr) {
    return;
  }
  err->start = error.span.start;
  err->end = error.span.end;
  const std::size_t n = std::min(error.message.size(), sizeof(err->message) - 1);
  if (n != 0) {
    std::memcpy(err->message, error.message.data(), n);
  }
  err->message[n] = '\0';
}

// A span at 0-0 whose message is text, cut to what fits. Allocates nothing.
void setMessage(AoSpan* err, std::string_view text) {
  if (err == nullptr) {
    return;
  }
  err->start = 0;
  err->end = 0;
  const std::size_t n = std::min(text.size(), sizeof(err->message) - 1);
  if (n != 0) {
    std::memcpy(err->message, text.data(), n);
  }
  err->message[n] = '\0';
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

extern "C" void ao_set_inspect_hook(AoInspectFn fn, void* user) {
  g_inspectFn = fn;
  g_inspectUser = user;
}

extern "C" int ao_workspace_reset(void) {
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [] { return ao::sessionWorkspaceReset() == 0 ? AO_OK : AO_ERR; });
}

extern "C" int ao_eval(const char* source, int source_len, int mode, char* out, int out_len,
                       AoSpan* err) {
  const AbiEntry entry;
  const int rc = !entry.entered() ? -1 : guarded(-1, [&] {
    return ao::sessionEval(source, source_len, mode, out, out_len, err, g_inspectFn,
                           g_inspectUser);
  });
  if (rc != -1) {
    return rc;
  }
  // Refused while the runtime is busy, or an exception: nothing to show.
  clearSpan(err);
  if (out != nullptr && out_len > 0) {
    out[0] = '\0';
  }
  return AO_ERR;
}

extern "C" int ao_accept_method(const char* class_name, int meta, const char* source, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr || class_name == nullptr || source == nullptr ||
        (meta != 0 && meta != 1)) {
      return AO_ERR;
    }
    // SPEC §3.10: a name that does not resolve to a class is not a compile error.
    if (!ao::namesBehavior(*s->ctx, class_name)) {
      return AO_ERR;
    }
    ao::compiler::CompileError error;
    if (!ao::acceptMethodSource(*s->ctx, class_name, meta == 1, source, &error)) {
      fillSpan(err, error);
      return AO_ERR_COMPILE;
    }
    return AO_OK;
  });
}

extern "C" int ao_accept_class(const char* source, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr || source == nullptr) {
      return AO_ERR;
    }
    ao::compiler::CompileError error;
    if (!ao::acceptClassSource(*s->ctx, source, &error)) {
      fillSpan(err, error);
      return AO_ERR_COMPILE;
    }
    return AO_OK;
  });
}

extern "C" int ao_image_load(const char* path, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  std::string reason;
  const int rc = guarded(-1, [&] {
    return ao::sessionImageLoad(path, &reason) == 0 ? AO_OK : AO_ERR;
  });
  if (rc == AO_OK) {
    return AO_OK;
  }
  // SPEC §3.10: an AO_ERR says why, never with an empty message. An exception is
  // "image load failed". Copying the reason allocates nothing.
  setMessage(err, rc == -1 || reason.empty() ? std::string_view("image load failed") : reason);
  return AO_ERR;
}

extern "C" void ao_set_transcript_hook(AoTranscriptFn fn, void* user) {
  g_transcriptFn = fn;
  g_transcriptUser = user;
  // SPEC §3.10: the session side wires it into this session and every later one (boot, load).
  ao::setSessionTranscriptHook(fn == nullptr ? nullptr : deliverTranscript);
}

extern "C" int ao_browser_class_count(void) {
  return guarded(-1, [] { return ao::browserClassCount(); });
}

extern "C" int ao_browser_class_at(int index, char* name, int name_len, char* category,
                                    int category_len) {
  return guarded(AO_ERR, [&] {
    return ao::browserClassAt(index, name, name_len, category, category_len);
  });
}

extern "C" int ao_browser_protocol_count(const char* class_name, int meta) {
  return guarded(-1, [&] { return ao::browserProtocolCount(class_name, meta); });
}

extern "C" int ao_browser_protocol_at(const char* class_name, int meta, int index, char* buf,
                                       int len) {
  return guarded(AO_ERR, [&] { return ao::browserProtocolAt(class_name, meta, index, buf, len); });
}

extern "C" int ao_browser_selector_count(const char* class_name, int meta, const char* protocol) {
  return guarded(-1, [&] { return ao::browserSelectorCount(class_name, meta, protocol); });
}

extern "C" int ao_browser_selector_at(const char* class_name, int meta, const char* protocol,
                                      int index, char* buf, int len) {
  return guarded(AO_ERR, [&] {
    return ao::browserSelectorAt(class_name, meta, protocol, index, buf, len);
  });
}

extern "C" int ao_browser_source(const char* class_name, int meta, const char* selector, char* buf,
                                 int len) {
  return guarded(AO_ERR, [&] { return ao::browserSource(class_name, meta, selector, buf, len); });
}

extern "C" int ao_browser_class_definition(const char* class_name, char* buf, int len) {
  return guarded(AO_ERR, [&] { return ao::browserClassDefinition(class_name, buf, len); });
}

extern "C" int ao_browser_superclass(const char* class_name, int meta, char* buf, int len) {
  return guarded(AO_ERR, [&] { return ao::browserSuperclass(class_name, meta, buf, len); });
}

extern "C" int ao_browser_subclass_count(const char* class_name) {
  return guarded(-1, [&] { return ao::browserSubclassCount(class_name); });
}

extern "C" int ao_browser_subclass_at(const char* class_name, int index, char* buf, int len) {
  return guarded(AO_ERR, [&] { return ao::browserSubclassAt(class_name, index, buf, len); });
}

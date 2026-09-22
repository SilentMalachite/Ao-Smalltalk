#include "Session.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compile.hpp"
#include "ao/Image.hpp"
#include "ao/Send.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace ao {
namespace {

std::unique_ptr<Session> g_session;

void installEmptyCache(Session& session, HostOopHook transcript, HostOopHook inspect) {
  session.cache = std::make_unique<ClassMethodCache>();
  session.cache->addRoots(session.roots);
  session.ctx = std::unique_ptr<CallContext>(new CallContext{
      session.heap, session.roots, session.wk, session.cache.get()});
  session.ctx->transcriptHook = transcript;
  session.ctx->inspectHook = inspect;
}

bool loadedImageProbes(Session& session) {
  Oop arg = Oop::fromSmallInteger(2);
  const Oop three =
      send(*session.ctx, Oop::fromSmallInteger(1), session.wk.intern("+"), &arg, 1, nullptr);
  const Oop isNil = send(*session.ctx, Oop::nil(), session.wk.intern("isNil"), nullptr, 0, nullptr);
  return three.isSmallInteger() && three.smallIntegerValue() == 3 && isNil.isTrue();
}

}  // namespace

Session::Session(bool bootstrap) : wk(heap, roots) {
  if (!bootstrap) {
    return;
  }
  installEmptyCache(*this, nullptr, nullptr);
  Bootstrap::run(heap, roots, wk);
}

Session* session() { return g_session.get(); }

int sessionBoot() {
  if (g_session != nullptr) {
    return 1;
  }
  g_session = std::make_unique<Session>(true);
  return 0;
}

int sessionShutdown() {
  g_session.reset();
  return 0;
}

int sessionImageSave(const char* path) {
  if (g_session == nullptr || path == nullptr) {
    return 1;
  }
  return Image::save(g_session->heap, g_session->roots, g_session->wk, path) ? 0 : 1;
}

int sessionImageLoad(const char* path) {
  if (g_session == nullptr || path == nullptr) {
    return 1;
  }
  auto next = std::make_unique<Session>(false);
  if (!Image::load(next->heap, next->roots, next->wk, path)) {
    return 1;
  }
  HostOopHook transcript = nullptr;
  HostOopHook inspect = nullptr;
  if (g_session->ctx != nullptr) {
    transcript = g_session->ctx->transcriptHook;
    inspect = g_session->ctx->inspectHook;
  }
  installEmptyCache(*next, transcript, inspect);
  g_session = std::move(next);
  ensureTranscriptClassMethods();
  if (!loadedImageProbes(*g_session)) {
    return 1;
  }
  return 0;
}

int sessionFileInLoadOrder(const char* path) {
  if (g_session == nullptr || g_session->ctx == nullptr || path == nullptr) {
    return 1;
  }
  std::vector<compiler::CompileError> errors;
  return fileInLoadOrder(*g_session->ctx, path, errors) ? 0 : 1;
}

void ensureTranscriptClassMethods() {}

}  // namespace ao

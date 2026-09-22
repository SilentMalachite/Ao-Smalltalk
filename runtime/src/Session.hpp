#pragma once

#include "ao/NativeMethod.hpp"

#include <memory>

namespace ao {

// Not a public header. One process session, same objects as the test Boot fixture.
struct Session {
  Heap heap;
  Roots roots;
  WellKnown wk;
  std::unique_ptr<ClassMethodCache> cache;
  std::unique_ptr<CallContext> ctx;

  // true: Bootstrap::run. false: empty old space for Image::load.
  explicit Session(bool bootstrap);
};

Session* session();
int sessionBoot();
int sessionShutdown();
int sessionImageSave(const char* path);
int sessionImageLoad(const char* path);
int sessionFileInLoadOrder(const char* path);
void ensureTranscriptClassMethods();

}  // namespace ao

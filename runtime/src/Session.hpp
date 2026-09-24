#pragma once

#include "ao/NativeMethod.hpp"

#include "ao_abi.h"

#include <memory>
#include <string>
#include <vector>

namespace ao {

// Not a public header. One process session, same objects as the test Boot fixture.
struct Session {
  Heap heap;
  Roots roots;
  WellKnown wk;
  std::unique_ptr<ClassMethodCache> cache;
  std::unique_ptr<CallContext> ctx;
  // Session Dictionary of workspace name strings to bindings (Associations). Not part of the image.
  Oop workspace = Oop::nil();
  bool workspaceRooted = false;
  // knownGlobals for workspace compiles, rebuilt when wk.globalsVersion() moves (SPEC §3.10).
  std::vector<std::string> knownGlobals;
  std::uint64_t knownGlobalsVersion = 0;
  bool knownGlobalsCached = false;

  // Method source text. unique_ptr keeps the rooted slots stable when the table grows.
  // Not part of the image.
  struct MethodSource {
    Oop method = Oop::nil();
    Oop text = Oop::nil();
  };
  std::vector<std::unique_ptr<MethodSource>> methodSources;

  // true: Bootstrap::run. false: empty old space for Image::load.
  explicit Session(bool bootstrap);
  ~Session();
};

Session* session();
int sessionBoot();
int sessionShutdown();
int sessionImageSave(const char* path);
int sessionImageLoad(const char* path);
int sessionFileInLoadOrder(const char* path);
int sessionWorkspaceReset();
int sessionEval(const char* source, int sourceLen, int mode, char* out, int outLen, AoSpan* err,
                AoInspectFn inspect, void* inspectUser);
// `replaced`, when a heap object, is dropped from the rooted table before `method` is stored.
void rememberMethodSource(Oop method, Oop text, Oop replaced);
// SPEC §3.9: a class whose shape changed takes its methods' sources along. The pair of `from`
// names `to` from now on. False when `from` has no source.
bool moveMethodSource(Oop from, Oop to);
bool methodSource(Oop method, std::string& utf8);
// The root slots the method source table adds, each method's and its text's. The table is session
// state no Smalltalk object reaches, so a trace for what is alive (SPEC §3.9) does not start from
// them. Empty outside a session.
std::vector<const Oop*> methodSourceRootSlots();
void clearMethodSources();
void ensureKernelNatives(Session& s);

int browserClassCount();
int browserClassAt(int index, char* name, int nameLen, char* category, int categoryLen);
int browserProtocolCount(const char* className, int meta);
int browserProtocolAt(const char* className, int meta, int index, char* buf, int len);
int browserSelectorCount(const char* className, int meta, const char* protocol);
int browserSelectorAt(const char* className, int meta, const char* protocol, int index, char* buf,
                      int len);
int browserSource(const char* className, int meta, const char* selector, char* buf, int len);
int browserClassDefinition(const char* className, char* buf, int len);
int browserSuperclass(const char* className, int meta, char* buf, int len);
int browserSubclassCount(const char* className);
int browserSubclassAt(const char* className, int index, char* buf, int len);

}  // namespace ao

#include "ao_abi.h"

#include "../src/Session.hpp"
#include "ao/Bootstrap.hpp"
#include "ao/Compile.hpp"
#include "ao/Gc.hpp"
#include "ao/Globals.hpp"
#include "ao/Heap.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

// SPEC §3.6, §3.9 削除: undefine takes the pair out (nil key and value, tally one less), moves the
// globals version, refuses a fixed global and a name Smalltalk does not bind, and interns nothing.
TEST(RemoveUnits, UndefineUnbindsAndMovesTheGlobalsVersion) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ASSERT_TRUE(wk.define("RuGone", ao::Oop::fromSmallInteger(3)));
  const std::uint64_t version = wk.globalsVersion();
  const ao::Oop key = wk.findSymbol("RuGone");
  ASSERT_TRUE(key.isHeap());
  const ao::Oop pairs = heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotArray);
  std::uint32_t index = UINT32_MAX;
  for (std::uint32_t i = 0; i + 1 < heap.size(pairs); i += 2) {
    if (heap.slotAt(pairs, i) == key) index = i;
  }
  ASSERT_NE(UINT32_MAX, index);
  const std::int64_t tally =
      heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue();

  EXPECT_TRUE(wk.undefine("RuGone"));
  EXPECT_TRUE(ao::Globals::lookup(wk, key).isEmpty());
  EXPECT_TRUE(wk.named("RuGone").isNil());
  EXPECT_TRUE(heap.slotAt(pairs, index).isNil());
  EXPECT_TRUE(heap.slotAt(pairs, index + 1).isNil());
  EXPECT_EQ(tally - 1,
            heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue());
  EXPECT_EQ(version + 1, wk.globalsVersion());

  EXPECT_FALSE(wk.undefine("RuGone"));
  EXPECT_FALSE(wk.undefine("Object"));
  EXPECT_FALSE(wk.undefine("Smalltalk"));
  EXPECT_FALSE(wk.undefine("Processor"));
  EXPECT_FALSE(wk.undefine("RuNeverInterned"));
  EXPECT_TRUE(wk.findSymbol("RuNeverInterned").isEmpty());
  EXPECT_EQ(wk.objectClass, wk.named("Object"));
  EXPECT_EQ(version + 1, wk.globalsVersion());

  // The next define takes the emptied pair, in place.
  ASSERT_TRUE(wk.define("RuNext", ao::Oop::fromSmallInteger(4)));
  EXPECT_EQ(wk.findSymbol("RuNext"), heap.slotAt(pairs, index));
  EXPECT_EQ(tally, heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue());
}

namespace {

class RemoveAbi : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  // SPEC §3.10: the hook and the debug mode outlive the session; a test's must not reach the next.
  void TearDown() override {
    ao_runtime_shutdown();
    ao_set_transcript_hook(nullptr, nullptr);
    ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  }
};

// Print it through the ABI: the printString, or "<rc: message>" when it fails.
std::string printIt(const char* source) {
  char out[512];
  AoSpan err{};
  const int rc = ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_PRINTIT, out,
                         static_cast<int>(sizeof(out)), &err);
  if (rc != AO_OK) {
    return "<" + std::to_string(rc) + ": " + err.message + ">";
  }
  return out;
}

// The Print it of `source` aborts with doesNotUnderstand: #selector.
void expectDnu(const char* source, const char* selector) {
  const std::string got = printIt(source);
  EXPECT_EQ(0u, got.find("<" + std::to_string(AO_ERR_EVAL) + ": ")) << got;
  EXPECT_NE(std::string::npos, got.find(std::string("doesNotUnderstand: #") + selector)) << got;
}

int defineClass(const char* name, const char* super, const char* category) {
  const std::string def = std::string(super) + " subclass: #" + name +
                          "\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n  category: '" + category + "'\n";
  AoSpan err{};
  return ao_accept_class(def.c_str(), &err);
}

// The ID the Browser hands out for name (SPEC §3.10 クラス ID); 0 for NULL or a name that binds no
// listed class.
std::int64_t idOf(const char* name) { return name == nullptr ? 0 : ao_browser_class_id(name); }

// The call's message. err starts non-empty, so an AO_OK that leaves it alone shows.
std::string tryRemoveMethodId(std::int64_t id, int meta, const char* sel, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_method(id, meta, sel, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
}

std::string tryRemoveMethod(const char* cls, int meta, const char* sel, int* rc) {
  return tryRemoveMethodId(idOf(cls), meta, sel, rc);
}

bool browserLists(const char* className) {
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    char name[256];
    char category[256];
    if (ao_browser_class_at(i, nullptr, name, sizeof(name), category, sizeof(category)) == AO_OK &&
        std::strcmp(name, className) == 0) {
      return true;
    }
  }
  return false;
}

// The method bound to sel on cls's own side (instance side), or nil.
ao::Oop methodOf(const char* cls, const char* sel) {
  ao::Session* s = ao::session();
  const ao::Oop c = s->wk.named(cls);
  const ao::Oop dict = s->heap.slotAt(c, ao::kClassSlotMethodDict);
  return ao::MethodDictionary::at(s->heap, dict, s->wk.findSymbol(sel));
}

std::string tryRemoveClassId(std::int64_t id, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_class(id, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
}

std::string tryRemoveClass(const char* cls, int* rc) { return tryRemoveClassId(idOf(cls), rc); }

// The session's class ID table holds id. Reads the table only: lists no class, so issues and
// prunes nothing.
bool tableHolds(std::int64_t id) {
  for (const auto& entry : ao::session()->classIds) {
    if (entry->id == id) {
      return true;
    }
  }
  return false;
}

// The index in Smalltalk's pair array of name's key, or -1.
int pairIndexOf(const char* name) {
  ao::Session* s = ao::session();
  const ao::Oop key = s->wk.findSymbol(name);
  if (!key.isHeap()) {
    return -1;
  }
  const ao::Oop pairs = s->heap.slotAt(s->wk.smalltalk, ao::Globals::kSmalltalkSlotArray);
  for (std::uint32_t i = 0; i + 1 < s->heap.size(pairs); i += 2) {
    if (s->heap.slotAt(pairs, i) == key) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

std::int64_t globalsTally() {
  ao::Session* s = ao::session();
  return s->heap.slotAt(s->wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue();
}

// SPEC §6: every pair left in a native-required dictionary (both sides) holds a NativeMethod; an
// emptied pair (nil key) is skipped.
bool kernelDictsAreNative() {
  struct Scan {
    ao::Session* s;
    bool ok = true;
    void visit(ao::Oop cls) {
      if (!cls.isHeap()) return;
      const ao::Oop dict = s->heap.slotAt(cls, ao::kClassSlotMethodDict);
      if (!dict.isHeap()) return;
      const ao::Oop inner = s->heap.slotAt(dict, ao::kDictSlotArray);
      if (!inner.isHeap()) return;
      for (std::uint32_t i = 0; i + 1 < s->heap.size(inner); i += 2) {
        if (s->heap.slotAt(inner, i).isNil()) continue;
        const ao::Oop v = s->heap.slotAt(inner, i + 1);
        if (!v.isHeap() || s->heap.klass(v) != s->wk.nativeMethodClass) ok = false;
      }
    }
  } scan{ao::session()};
  scan.s->wk.eachNativeRequiredClass(
      [](void* p, ao::Oop cls) {
        auto* sc = static_cast<Scan*>(p);
        sc->visit(cls);
        sc->visit(sc->s->heap.klass(cls));
      },
      &scan);
  return scan.ok;
}

struct BusyRemove {
  std::int64_t objectId = 0;
  std::int64_t busyId = 0;
  std::int64_t goneId = 0;
  int methodRc = -9;
  int classRc = -9;
  int acceptRc = -9;
  std::int64_t readId = -9;
  std::int64_t newId = -9;
  bool goneHeldBeforeRead = false;
  bool goneHeldAfterRead = true;
  int goneProtocols = -9;
  std::string methodMsg;
  std::string classMsg;
};

// A transcript hook: the runtime is busy here (SPEC §3.10 再入).
void removeFromHook(const char*, int, int, void* user) {
  auto* b = static_cast<BusyRemove*>(user);
  AoSpan err{};
  b->methodRc = ao_remove_method(b->objectId, 0, "b12busy", &err);
  b->methodMsg = err.message;
  b->classRc = ao_remove_class(b->busyId, &err);
  b->classMsg = err.message;
  b->acceptRc = ao_accept_method_id(b->objectId, 0, "b12busy\n  ^2\n", &err);
  // The refused calls above resolved no ID, so the table is as the test left it; the reads below
  // are the first to list the classes since then: they issue B12BusyNew's ID and prune goneId.
  b->goneHeldBeforeRead = tableHolds(b->goneId);
  b->readId = ao_browser_class_id("B12Busy");
  b->newId = ao_browser_class_id("B12BusyNew");
  b->goneHeldAfterRead = tableHolds(b->goneId);
  b->goneProtocols = ao_browser_protocol_count(b->goneId, 0);
}

}  // namespace

// SPEC §3.9 削除, §6: a method accepted on a Kernel class goes, and the send falls to
// doesNotUnderstand:. The message is empty on AO_OK.
TEST_F(RemoveAbi, RemoveAcceptedMethodOnKernelClass) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12foo\n  ^1\n", &err)) << err.message;
  EXPECT_EQ("1", printIt("Object new b12foo"));
  EXPECT_EQ("1", printIt("3 b12foo"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("Object", 0, "b12foo", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("Object new b12foo", "b12foo");
  expectDnu("3 b12foo", "b12foo");
  char shown[64];
  EXPECT_EQ(AO_ERR, ao_browser_source(ao_browser_class_id("Object"), 0, "b12foo", shown, sizeof(shown)));
  EXPECT_EQ(0, ao_browser_selector_count(ao_browser_class_id("Object"), 0, "user"));
}

// SPEC §3.3, §3.9 削除: a send cached before the removal finds the superclass's method next, and
// doesNotUnderstand: once that goes too.
TEST_F(RemoveAbi, RemovedMethodFallsBackEvenWhenCached) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Parent", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12Kid", "B12Parent", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Parent", 0, "who\n  ^'parent'\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B12Kid", 0, "who\n  ^'kid'\n", &err)) << err.message;
  EXPECT_EQ("'kid'", printIt("B12Kid new who"));
  EXPECT_EQ("'kid'", printIt("B12Kid new who"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Kid", 0, "who", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("'parent'", printIt("B12Kid new who"));
  EXPECT_EQ("'parent'", printIt("B12Parent new who"));
  EXPECT_EQ("", tryRemoveMethod("B12Parent", 0, "who", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Kid new who", "who");
  expectDnu("B12Parent new who", "who");
}

// SPEC §3.9 削除: meta 1 takes the metaclass's dictionary; the instance side is untouched.
TEST_F(RemoveAbi, RemoveClassSideMethod) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Cls", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Cls", 1, "make\n  ^42\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B12Cls", 0, "make\n  ^1\n", &err)) << err.message;
  EXPECT_EQ("42", printIt("B12Cls make"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Cls", 1, "make", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Cls make", "make");
  EXPECT_EQ("1", printIt("B12Cls new make"));
  EXPECT_EQ("selector not found: B12Cls class>>make", tryRemoveMethod("B12Cls", 1, "make", &rc));
  EXPECT_EQ(AO_ERR, rc);
}

// SPEC §3.9 削除: a NativeMethod is refused, on either side; the native keeps working.
TEST_F(RemoveAbi, RemoveNativeMethodIsRefused) {
  int rc = -9;
  EXPECT_EQ("native method removal refused: Object>>printString",
            tryRemoveMethod("Object", 0, "printString", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("native method removal refused: Behavior>>new", tryRemoveMethod("Behavior", 0, "new", &rc));
  EXPECT_EQ(AO_ERR, rc);
  // SmallInteger>>printString answers the digits; Print it shows that String quoted, as below.
  EXPECT_EQ("'3'", printIt("3 printString"));
  // Object>>printString answers the class name; Print it shows the String quoted.
  EXPECT_EQ("'Object'", printIt("Object new printString"));
}

// SPEC §3.9 削除: only the side's own dictionary counts: an inherited selector, an unknown one and
// an empty one are refused with their reasons. A name that binds no listed class has no ID
// (unknown class id); a bad meta or a NULL selector is "remove failed".
TEST_F(RemoveAbi, RemoveMissingOrInheritedSelectorIsRefused) {
  ASSERT_EQ(AO_OK, defineClass("B12Child", "Object", "B12-Test"));
  int rc = -9;
  EXPECT_EQ("selector not found: B12Child>>printString",
            tryRemoveMethod("B12Child", 0, "printString", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("selector not found: B12Child>>nope", tryRemoveMethod("B12Child", 0, "nope", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("selector not found: B12Child class>>new", tryRemoveMethod("B12Child", 1, "new", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("selector not found: B12Child>>", tryRemoveMethod("B12Child", 0, "", &rc));
  EXPECT_EQ(AO_ERR, rc);
  for (const char* none : {"Processor", "Smalltalk", "B12Nope", ""}) {
    EXPECT_EQ("unknown class id", tryRemoveMethod(none, 0, "foo", &rc)) << none;
    EXPECT_EQ(AO_ERR, rc);
  }
  EXPECT_EQ("unknown class id", tryRemoveMethod(nullptr, 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 2, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 0, nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  // err may be NULL.
  EXPECT_EQ(AO_ERR, ao_remove_method(idOf("B12Child"), 0, "nope", nullptr));
  EXPECT_EQ("'3'", printIt("3 printString"));
}

// SPEC §3.9 削除, §3.10: the source entry goes with its blocks: the text, the debug info and the
// root slots (method, text, one block) are gone.
TEST_F(RemoveAbi, RemoveMethodDropsSourceEntry) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Src", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Src", 0, "blk\n  ^[:x | x + 1] value: 1\n", &err))
      << err.message;
  const ao::Oop method = methodOf("B12Src", "blk");
  ASSERT_TRUE(method.isHeap());
  std::string text;
  ASSERT_TRUE(ao::methodSource(method, text));
  ASSERT_TRUE(static_cast<bool>(ao::debugInfoFor(method)));
  const std::size_t slots = ao::methodSourceRootSlots(false).size();
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Src", 0, "blk", &rc));
  EXPECT_EQ(AO_OK, rc);
  // Nothing allocated, so the raw Oop still names the object.
  EXPECT_FALSE(ao::methodSource(method, text));
  EXPECT_FALSE(static_cast<bool>(ao::debugInfoFor(method)));
  EXPECT_EQ(slots - 3, ao::methodSourceRootSlots(false).size());
  char shown[64];
  EXPECT_EQ(AO_ERR, ao_browser_source(ao_browser_class_id("B12Src"), 0, "blk", shown, sizeof(shown)));
}

// SPEC §3.9 削除, §6: the name reads nil (PushGlobal, no recompile), the instances keep their
// class and methods, and the Browser no longer lists the class; its ID is unknown from then on.
TEST_F(RemoveAbi, RemovedClassNameReadsNilAndInstancesKeepWorking) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Inst", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Inst", 0, "answer\n  ^7\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12useInst\n  ^B12Inst\n", &err)) << err.message;
  // A class object's own printString goes through Object>>printString too: its class is the
  // metaclass, whose name is "<Name> class".
  EXPECT_EQ("B12Inst class", printIt("Object new b12useInst"));
  // An instance prints as its class name (Object>>printString).
  EXPECT_EQ("B12Inst", printIt("b12i := B12Inst new"));
  ASSERT_TRUE(browserLists("B12Inst"));
  const std::int64_t id = idOf("B12Inst");
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("Object new b12useInst"));
  EXPECT_EQ("7", printIt("b12i answer"));
  EXPECT_EQ("'B12Inst'", printIt("b12i printString"));
  EXPECT_FALSE(browserLists("B12Inst"));
  EXPECT_EQ("unknown class id", tryRemoveClass("B12Inst", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("unknown class id", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_ERR, rc);
}

// SPEC §3.9 削除, §3.10 ワークスペース変数: the globals version moved, so the name is an undeclared
// identifier again: it reads nil, and an assignment (refused while the class was there) binds it.
TEST_F(RemoveAbi, RemovedClassNameBecomesWorkspaceVariable) {
  ASSERT_EQ(AO_OK, defineClass("B12Var", "Object", "B12-Test"));
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Var class", printIt("B12Var"));
  const std::string refused = printIt("B12Var := 3");
  EXPECT_EQ(0u, refused.find("<" + std::to_string(AO_ERR_COMPILE) + ": ")) << refused;
  EXPECT_NE(std::string::npos, refused.find("cannot assign")) << refused;
  const std::uint64_t version = ao::session()->wk.globalsVersion();
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Var", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ(version + 1, ao::session()->wk.globalsVersion());
  EXPECT_EQ("nil", printIt("B12Var"));
  EXPECT_EQ("3", printIt("B12Var := 3"));
  EXPECT_EQ("3", printIt("B12Var"));
}

// SPEC §3.9 削除: a fixed global (a Kernel class, a vendor stub) is refused by its own name. A
// name that binds no listed class (Processor, Smalltalk, a metaclass, an unknown or empty name)
// and NULL have no ID.
TEST_F(RemoveAbi, RemoveFixedGlobalIsRefused) {
  int rc = -9;
  EXPECT_EQ("class removal refused: Object is a fixed global", tryRemoveClass("Object", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("class removal refused: Bag is a fixed global", tryRemoveClass("Bag", &rc));
  EXPECT_EQ(AO_ERR, rc);
  for (const char* none : {"Processor", "Smalltalk", "B12Nope", ""}) {
    EXPECT_EQ("unknown class id", tryRemoveClass(none, &rc)) << none;
    EXPECT_EQ(AO_ERR, rc);
  }
  EXPECT_EQ("unknown class id", tryRemoveClass(nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("Smalltalk at: #B12Meta put: Object class").find("<"));
  EXPECT_EQ("unknown class id", tryRemoveClass("B12Meta", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(AO_ERR, ao_remove_class(idOf("Object"), nullptr));
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("Object class", printIt("Object"));
  EXPECT_EQ("Bag class", printIt("Bag"));
}

// SPEC §3.9 削除: a Kernel class is refused by identity. Through an alias it is the same class
// with the same ID (refused by its own fixed name); with its name slot rewritten (instVarAt:put:,
// kClassSlotName is slot 3, index 4) the identity check still refuses it.
TEST_F(RemoveAbi, RemoveKernelAliasIsRefused) {
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("SmallInteger class", printIt("Smalltalk at: #B12IntAlias put: SmallInteger"));
  EXPECT_EQ(idOf("SmallInteger"), idOf("B12IntAlias"));
  int rc = -9;
  EXPECT_EQ("class removal refused: SmallInteger is a fixed global",
            tryRemoveClass("B12IntAlias", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("b12name := SmallInteger instVarAt: 4").find("<"));
  EXPECT_EQ("'B12Renamed'", printIt("SmallInteger instVarAt: 4 put: 'B12Renamed'"));
  EXPECT_EQ("class removal refused: B12Renamed is a kernel class",
            tryRemoveClass("SmallInteger", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("SmallInteger instVarAt: 4 put: b12name").find("<"));
  EXPECT_EQ("SmallInteger class", printIt("B12IntAlias"));
}

// SPEC §3.9 削除: a live subclass refuses the removal, named after the smallest name; a subclass
// whose name slot is no string (reflection) makes it "an unnamed subclass". The refused class is
// still bound.
TEST_F(RemoveAbi, RemoveClassWithSubclassIsRefused) {
  ASSERT_EQ(AO_OK, defineClass("B12Base", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12SubB", "B12Base", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12SubA", "B12Base", "B12-Test"));
  int rc = -9;
  EXPECT_EQ("class removal refused: B12Base has subclass B12SubA", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Base class", printIt("B12Base"));
  // The subclasses go (leaves first), then the base can go.
  EXPECT_EQ("", tryRemoveClass("B12SubA", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("class removal refused: B12Base has subclass B12SubB", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("", tryRemoveClass("B12SubB", &rc));
  EXPECT_EQ(AO_OK, rc);
  // An unbound subclass a workspace variable keeps alive through an instance still counts, and
  // with its name slot nilled (instVarAt:put:, kClassSlotName is slot 3, index 4) it is unnamed.
  // It goes while its name still binds it (a nameless class is not bound under its own name).
  ASSERT_EQ(AO_OK, defineClass("B12Anon", "B12Base", "B12-Test"));
  EXPECT_EQ("B12Anon", printIt("b12anon := B12Anon new"));
  EXPECT_EQ("", tryRemoveClass("B12Anon", &rc));
  EXPECT_EQ(AO_OK, rc);
  // instVarAt:put: answers the value (ao_Object_instVarAt_put_).
  EXPECT_EQ("nil", printIt("b12anon class instVarAt: 4 put: nil"));
  EXPECT_EQ("class removal refused: B12Base has an unnamed subclass", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("nil", printIt("b12anon := nil"));
  EXPECT_EQ("", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Base"));
}

// SPEC §3.9 削除: only the class's own binding goes; an alias keeps the class reachable, and the
// Browser keeps listing it under its own name slot with the same ID. That row's removal is refused:
// its name no longer binds it. A nameless class's row is refused the same way.
TEST_F(RemoveAbi, RemoveClassKeepsAliases) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Real", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Real", 0, "answer\n  ^9\n", &err)) << err.message;
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Real class", printIt("Smalltalk at: #B12Alias put: B12Real"));
  const std::int64_t id = idOf("B12Real");
  EXPECT_EQ(id, idOf("B12Alias"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Real"));
  EXPECT_EQ("9", printIt("B12Alias new answer"));
  // classRows reads the class's own name slot, not the binding key it was reached through.
  EXPECT_FALSE(browserLists("B12Alias"));
  EXPECT_TRUE(browserLists("B12Real"));
  EXPECT_EQ(id, idOf("B12Alias"));
  EXPECT_EQ("class removal refused: B12Real is not bound to this class", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("9", printIt("B12Alias new answer"));
  EXPECT_TRUE(browserLists("B12Real"));
  // Unbinding the alias from the Workspace drops the row.
  EXPECT_EQ("nil", printIt("Smalltalk at: #B12Alias put: nil"));
  EXPECT_FALSE(browserLists("B12Real"));
  // A class whose name slot is no String is not bound under its own name either.
  ASSERT_EQ(AO_OK, defineClass("B12Nameless", "Object", "B12-Test"));
  EXPECT_EQ("nil", printIt("B12Nameless instVarAt: 4 put: nil"));
  EXPECT_EQ("class removal refused: an unnamed class is not bound to this class",
            tryRemoveClass("B12Nameless", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_NE(0, idOf("B12Nameless"));
}

// SPEC §3.6: the emptied pair is the one the next registration takes; the tally follows.
TEST_F(RemoveAbi, RemovedGlobalSlotIsReused) {
  ASSERT_EQ(AO_OK, defineClass("B12Slot", "Object", "B12-Test"));
  const int index = pairIndexOf("B12Slot");
  ASSERT_NE(-1, index);
  const std::int64_t tally = globalsTally();
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Slot", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ(-1, pairIndexOf("B12Slot"));
  EXPECT_EQ(tally - 1, globalsTally());
  EXPECT_EQ("3", printIt("Smalltalk at: #B12Next put: 3"));
  EXPECT_EQ(index, pairIndexOf("B12Next"));
  EXPECT_EQ(tally, globalsTally());
  EXPECT_EQ("3", printIt("B12Next"));
}

// SPEC §3.10 再入と例外: from a hook the runtime is busy: both removals and ao_accept_method_id do
// nothing, while the Browser reads (which may issue and prune IDs) still answer. B12BusyNew is
// defined after the test's IDs were taken and first listed inside the hook, so a read there issues
// its ID; B12BusyGone had an ID and was unbound before the hook, so a read there prunes it.
TEST_F(RemoveAbi, RemoveWhileBusyIsRefused) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12busy\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, defineClass("B12Busy", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12BusyGone", "Object", "B12-Test"));
  BusyRemove seen;
  seen.objectId = idOf("Object");
  seen.busyId = idOf("B12Busy");
  seen.goneId = idOf("B12BusyGone");
  ASSERT_GT(seen.goneId, 0);
  int rc = -9;
  // The removal resolves the ID before it unbinds, so the table still holds goneId afterwards.
  EXPECT_EQ("", tryRemoveClassId(seen.goneId, &rc));
  EXPECT_EQ(AO_OK, rc);
  ASSERT_TRUE(tableHolds(seen.goneId));
  ASSERT_EQ(AO_OK, defineClass("B12BusyNew", "Object", "B12-Test"));
  ao_set_transcript_hook(removeFromHook, &seen);
  EXPECT_EQ("7", printIt("Transcript show: 'x'. 7"));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(AO_ERR, seen.methodRc);
  EXPECT_EQ("runtime is busy", seen.methodMsg);
  EXPECT_EQ(AO_ERR, seen.classRc);
  EXPECT_EQ("runtime is busy", seen.classMsg);
  EXPECT_EQ(AO_ERR, seen.acceptRc);
  EXPECT_EQ(seen.busyId, seen.readId);
  EXPECT_TRUE(seen.goneHeldBeforeRead);
  EXPECT_GT(seen.newId, 0);
  EXPECT_FALSE(seen.goneHeldAfterRead);
  EXPECT_EQ(-1, seen.goneProtocols);
  EXPECT_EQ(seen.newId, idOf("B12BusyNew"));
  EXPECT_EQ("1", printIt("3 b12busy"));
  EXPECT_TRUE(browserLists("B12Busy"));
}

// SPEC §3.9 削除: a refused call leaves the dictionaries, the cache, the globals version and the
// source table as they were.
TEST_F(RemoveAbi, RefusedRemoveChangesNothing) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Par", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12Kid2", "B12Par", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Par", 0, "who\n  ^'par'\n", &err)) << err.message;
  EXPECT_EQ("'par'", printIt("B12Kid2 new who"));
  const int classes = ao_browser_class_count();
  const std::int64_t tally = globalsTally();
  const std::uint64_t version = ao::session()->wk.globalsVersion();
  const std::size_t slots = ao::methodSourceRootSlots(false).size();
  const int protocols = ao_browser_protocol_count(ao_browser_class_id("Object"), 0);
  // Nothing between the snapshot and the check sends, so the cache moves only if a refusal drops it.
  const auto cacheBits = [] {
    std::vector<std::uint64_t> bits;
    for (const auto& e : ao::session()->cache->entries) {
      bits.push_back(e.klass.bits());
      bits.push_back(e.selector.bits());
      bits.push_back(e.method.bits());
    }
    return bits;
  };
  const std::vector<std::uint64_t> cached = cacheBits();
  ASSERT_NE(std::vector<std::uint64_t>(cached.size(), ao::Oop{}.bits()), cached);
  int rc = -9;
  EXPECT_EQ(AO_ERR, ao_remove_method(idOf("Object"), 0, "printString", &err));
  EXPECT_EQ(AO_ERR, ao_remove_method(idOf("B12Kid2"), 0, "who", &err));
  EXPECT_EQ(AO_ERR, ao_remove_class(idOf("Object"), &err));
  EXPECT_EQ(AO_ERR, ao_remove_class(idOf("B12Par"), &err));
  EXPECT_EQ("class removal refused: B12Par has subclass B12Kid2", tryRemoveClass("B12Par", &rc));
  EXPECT_EQ(cached, cacheBits());
  EXPECT_EQ(classes, ao_browser_class_count());
  EXPECT_EQ(tally, globalsTally());
  EXPECT_EQ(version, ao::session()->wk.globalsVersion());
  EXPECT_EQ(slots, ao::methodSourceRootSlots(false).size());
  EXPECT_EQ(protocols, ao_browser_protocol_count(ao_browser_class_id("Object"), 0));
  EXPECT_EQ("'par'", printIt("B12Kid2 new who"));
  EXPECT_EQ("'3'", printIt("3 printString"));
  char shown[64];
  EXPECT_EQ(AO_OK, ao_browser_source(ao_browser_class_id("B12Par"), 0, "who", shown, sizeof(shown)));
}

// SPEC §3.9 削除, §6: a removal is in the saved image: the method and the binding stay gone.
TEST_F(RemoveAbi, RemovalSurvivesSaveAndLoad) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Keep", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12Gone", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Keep", 0, "foo\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B12Keep", 0, "bar\n  ^2\n", &err)) << err.message;
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Keep", 0, "foo", &rc));
  EXPECT_EQ("", tryRemoveClass("B12Gone", &rc));
  const char* path = "remove-abi.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path, &err)) << err.message;
  std::remove(path);
  expectDnu("B12Keep new foo", "foo");
  EXPECT_EQ("2", printIt("B12Keep new bar"));
  EXPECT_EQ("nil", printIt("B12Gone"));
  EXPECT_FALSE(browserLists("B12Gone"));
  EXPECT_TRUE(browserLists("B12Keep"));
  // A loaded image's method has no source; removing it still works.
  EXPECT_EQ("", tryRemoveMethod("B12Keep", 0, "bar", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Keep new bar", "bar");
}

// SPEC §6: accepting a CompiledMethod on Kernel classes and removing it leaves every Kernel
// dictionary all-native, holes included.
TEST_F(RemoveAbi, KernelScanStaysGreenAfterRemovals) {
  AoSpan err{};
  ASSERT_TRUE(kernelDictsAreNative());
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12scan\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("SmallInteger", 0, "b12scanInt\n  ^2\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 1, "b12scanMeta\n  ^3\n", &err)) << err.message;
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("Object", 0, "b12scan", &rc));
  EXPECT_EQ("", tryRemoveMethod("SmallInteger", 0, "b12scanInt", &rc));
  EXPECT_EQ("", tryRemoveMethod("Object", 1, "b12scanMeta", &rc));
  EXPECT_TRUE(kernelDictsAreNative());
  EXPECT_EQ("'3'", printIt("3 printString"));
  EXPECT_EQ("7", printIt("3 + 4"));
}

// SPEC §3.9 削除: a halted process keeps running the method it was in; Proceed answers from it.
TEST_F(RemoveAbi, RemovedMethodKeepsRunningInHaltedProcess) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12halt\n  self halt.\n  ^5\n", &err))
      << err.message;
  ao_set_debug_mode(AO_DEBUG_LIVE);
  char out[64];
  ASSERT_EQ(AO_ERR_HALT, ao_eval("Object new b12halt", 18, AO_EVAL_PRINTIT, out, 64, &err));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("Object", 0, "b12halt", &rc));
  EXPECT_EQ(AO_OK, rc);
  ASSERT_EQ(AO_OK, ao_debug_proceed(pid, out, 64, &err)) << err.message;
  EXPECT_STREQ("5", out);
  // Back to a plain eval (SPEC §3.13: a DNU under AO_DEBUG_LIVE halts instead).
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  expectDnu("Object new b12halt", "b12halt");
}

namespace {

std::string frameLabel(int i) {
  char buf[256];
  return ao_debug_frame_label(i, buf, sizeof(buf)) == AO_OK ? std::string(buf) : "<none>";
}

// Frame i reads the placeholder of a method without source: AO_ERR_NOSOURCE, highlight 0-0.
void expectPlaceholderFrame(int i, const std::string& label) {
  EXPECT_EQ(label, frameLabel(i));
  char buf[256];
  AoSpan highlight{};
  highlight.start = 7;
  highlight.end = 9;
  EXPECT_EQ(AO_ERR_NOSOURCE, ao_debug_frame_source(i, buf, sizeof(buf), &highlight));
  EXPECT_EQ("\"" + label + " source not available\"", std::string(buf));
  EXPECT_EQ(0u, highlight.start);
  EXPECT_EQ(0u, highlight.end);
}

// The capture setting outlives the session (SPEC §3.10); a test's must not reach the next.
struct CaptureOn {
  CaptureOn() { ao_set_debug_capture(1); }
  ~CaptureOn() { ao_set_debug_capture(0); }
};

}  // namespace

// SPEC §3.13 Step: the statement starts live in the source table's entry, which the removal drops
// (§3.9 削除), so a removed method's frame has none. Step into goes past `x := self helper.` to
// helper's first instruction, and step over back in run goes past `^x + 1` to the sender.
TEST_F(RemoveAbi, SteppingARemovedMethodFindsNoStatementStart) {
  ASSERT_EQ(AO_OK, defineClass("B12Step", "Object", "B12"));
  const std::int64_t id = idOf("B12Step");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method_id(
                       id, 0, "run\n  | x |\n  self halt.\n  x := self helper.\n  ^x + 1\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method_id(id, 0, "helper\n  ^41\n", &err)) << err.message;
  ao_set_debug_mode(AO_DEBUG_LIVE);
  char out[64];

  // Kept, the method's next statement is where step into stops first.
  ASSERT_EQ(AO_ERR_HALT, ao_eval("B12Step new run", 15, AO_EVAL_PRINTIT, out, 64, &err));
  std::int64_t pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  ASSERT_EQ(AO_ERR_HALT, ao_debug_step_into(pid, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("B12Step>>run", frameLabel(0));
  ASSERT_EQ(AO_OK, ao_debug_abort(pid));

  ASSERT_EQ(AO_ERR_HALT, ao_eval("B12Step new run", 15, AO_EVAL_PRINTIT, out, 64, &err));
  pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Step", 0, "run", &rc));
  ASSERT_EQ(AO_OK, rc);
  ASSERT_EQ(AO_ERR_HALT, ao_debug_step_into(pid, out, 64, &err));
  EXPECT_STREQ("step", err.message);
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("B12Step>>helper", frameLabel(0));
  ASSERT_EQ(AO_ERR_HALT, ao_debug_step_out(pid, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("B12Step>>run", frameLabel(0));
  ASSERT_EQ(AO_ERR_HALT, ao_debug_step_over(pid, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("doIt", frameLabel(0));
  ASSERT_EQ(AO_OK, ao_debug_proceed(pid, out, 64, &err)) << err.message;
  EXPECT_STREQ("42", out);
}

// SPEC §3.10 ソース表, §3.13: removing the methods and the class a snapshot's frames ran keeps
// their labels and the generation; their source reads the placeholder.
TEST_F(RemoveAbi, SnapshotFramesOfRemovedMethodsReadPlaceholders) {
  const CaptureOn capture;
  ASSERT_EQ(AO_OK, defineClass("B12Snap", "Object", "B12"));
  const std::int64_t id = idOf("B12Snap");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method_id(id, 0, "boom\n  ^self error: 'b12'\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method_id(id, 0, "outer\n  ^self boom\n", &err)) << err.message;
  char out[64];
  ASSERT_EQ(AO_ERR_EVAL, ao_eval("B12Snap new outer", 17, AO_EVAL_PRINTIT, out, 64, &err));
  const int generation = ao_debug_generation();
  ASSERT_EQ(AO_OK, ao_debug_select(0));
  ASSERT_EQ("B12Snap>>boom", frameLabel(1));
  ASSERT_EQ("B12Snap>>outer", frameLabel(2));
  char buf[256];
  ASSERT_EQ(AO_OK, ao_debug_frame_source(1, buf, sizeof(buf), nullptr));

  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Snap", 0, "boom", &rc));
  ASSERT_EQ(AO_OK, rc);
  expectPlaceholderFrame(1, "B12Snap>>boom");
  ASSERT_EQ(AO_OK, ao_debug_frame_source(2, buf, sizeof(buf), nullptr));
  ASSERT_EQ(AO_OK, ao_remove_class(id, &err)) << err.message;
  expectPlaceholderFrame(2, "B12Snap>>outer");

  EXPECT_EQ(generation, ao_debug_generation());
  EXPECT_EQ(4, ao_debug_frame_count());
  char cls[64];
  ASSERT_EQ(AO_OK, ao_debug_frame_receiver_print(1, cls, sizeof(cls), buf, sizeof(buf)));
  EXPECT_STREQ("B12Snap", cls);
}

// SPEC §3.2, §3.9 削除, §3.13: a halted process roots the class its frame runs in. Removed and
// collected, the frame reads its temp, and step into and Proceed run the class's methods.
TEST_F(RemoveAbi, RemovedClassSurvivesGcInHaltedProcess) {
  ASSERT_EQ(AO_OK, defineClass("B12Held", "Object", "B12"));
  const std::int64_t id = idOf("B12Held");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method_id(
                       id, 0, "run\n  | x |\n  x := 3.\n  self halt.\n  ^self helper + x\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method_id(id, 0, "helper\n  ^39\n", &err)) << err.message;
  ao_set_debug_mode(AO_DEBUG_LIVE);
  char out[64];
  ASSERT_EQ(AO_ERR_HALT, ao_eval("B12Held new run", 15, AO_EVAL_PRINTIT, out, 64, &err));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  ASSERT_EQ(AO_OK, ao_remove_class(id, &err)) << err.message;
  EXPECT_EQ(0, idOf("B12Held"));

  ao::Session* s = ao::session();
  const std::uint64_t collections = s->heap.oldCollections();
  ao::Gc gc(s->heap, s->roots);
  gc.collectNursery();
  gc.collectOld();
  EXPECT_LT(collections, s->heap.oldCollections());

  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  expectPlaceholderFrame(1, "B12Held>>run");
  char cls[64];
  char buf[64];
  ASSERT_EQ(AO_OK, ao_debug_frame_receiver_print(1, cls, sizeof(cls), buf, sizeof(buf)));
  EXPECT_STREQ("B12Held", cls);
  ASSERT_EQ(AO_OK, ao_debug_frame_temp_print(1, 0, cls, sizeof(cls), buf, sizeof(buf)));
  EXPECT_STREQ("3", buf);
  ASSERT_EQ(AO_ERR_HALT, ao_debug_step_into(pid, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("B12Held>>helper", frameLabel(0));
  ASSERT_EQ(AO_OK, ao_debug_proceed(pid, out, 64, &err)) << err.message;
  EXPECT_STREQ("42", out);
}

// SPEC §3.10: AoSpan.message holds 255 bytes; a longer reason is cut, never emptied.
TEST_F(RemoveAbi, LongNameCutsTheMessageAt255Bytes) {
  const std::string name = "B" + std::string(299, 'N');
  ASSERT_EQ(AO_OK, defineClass(name.c_str(), "Object", "B12-Test"));
  int rc = -9;
  const std::string got = tryRemoveMethod(name.c_str(), 0, "foo", &rc);
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(255u, got.size());
  EXPECT_EQ(("selector not found: " + name + ">>foo").substr(0, 255), got);
}

// SPEC §3.9 削除: the method dictionary slot can hold anything after instVarAt:put: (kClassSlotMethodDict
// is slot 1, index 2). An empty Array, a one-slot Array, a String, and a two-slot Array whose array
// slot is a String are no dictionary: the removal is refused as "selector not found" without
// reading out of range, and nothing changes. Put back, the dictionary still answers the method.
TEST_F(RemoveAbi, RemoveMethodFromMalformedDictionaryIsRefused) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Bad", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Bad", 0, "foo\n  ^1\n", &err)) << err.message;
  EXPECT_EQ("1", printIt("B12Bad new foo"));
  const std::string saved = printIt("b12dict := B12Bad instVarAt: 2");
  ASSERT_NE(0u, saved.find('<')) << saved;
  const std::string arr = printIt("b12arr := Array new: 2. b12arr at: 2 put: 'xy'");
  ASSERT_NE(0u, arr.find('<')) << arr;
  int rc = -9;
  for (const char* bad : {"Array new: 0", "Array new: 1", "'abc'", "b12arr"}) {
    const std::string put = std::string("B12Bad instVarAt: 2 put: (") + bad + ")";
    ASSERT_NE(0u, printIt(put.c_str()).find('<')) << bad;
    EXPECT_EQ("selector not found: B12Bad>>foo", tryRemoveMethod("B12Bad", 0, "foo", &rc)) << bad;
    EXPECT_EQ(AO_ERR, rc) << bad;
  }
  ASSERT_NE(0u, printIt("B12Bad instVarAt: 2 put: b12dict").find('<'));
  EXPECT_EQ("1", printIt("B12Bad new foo"));
  EXPECT_EQ("", tryRemoveMethod("B12Bad", 0, "foo", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Bad new foo", "foo");
}

// SPEC §3.9 削除: a subclass is any descendant, not only a direct child. With B12Deep → B12DeepZ →
// B12DeepA the smallest name among all of them is B12DeepA.
TEST_F(RemoveAbi, RemoveClassNamesTheSmallestDescendant) {
  ASSERT_EQ(AO_OK, defineClass("B12Deep", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12DeepZ", "B12Deep", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12DeepA", "B12DeepZ", "B12-Test"));
  int rc = -9;
  EXPECT_EQ("class removal refused: B12Deep has subclass B12DeepA", tryRemoveClass("B12Deep", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("class removal refused: B12DeepZ has subclass B12DeepA",
            tryRemoveClass("B12DeepZ", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("", tryRemoveClass("B12DeepA", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("class removal refused: B12Deep has subclass B12DeepZ", tryRemoveClass("B12Deep", &rc));
  EXPECT_EQ(AO_ERR, rc);
}

// SPEC §3.3: a method dictionary slot rewritten with instVarAt:put: (kClassSlotMethodDict is slot
// 1, index 2) holds no method. A send skips it to the superclass or doesNotUnderstand:, the
// reflective reads answer nothing, and Accept fails with "install failed"; nothing reads out of
// range. Each probe sends a selector not sent before, so no cached lookup answers for it.
TEST_F(RemoveAbi, MalformedMethodDictionaryFallsBackOnSend) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Mal", "Object", "B12-Test"));
  for (int i = 0; i < 4; ++i) {
    const std::string own = "own" + std::to_string(i) + "\n  ^" + std::to_string(i) + "\n";
    ASSERT_EQ(AO_OK, ao_accept_method("B12Mal", 0, own.c_str(), &err)) << err.message;
    const std::string up = "b12up" + std::to_string(i) + "\n  ^5\n";
    ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, up.c_str(), &err)) << err.message;
  }
  ASSERT_EQ(AO_OK, ao_accept_method("B12Mal", 0, "kept\n  ^7\n", &err)) << err.message;
  const std::string saved = printIt("b12malDict := B12Mal instVarAt: 2");
  ASSERT_NE(0u, saved.find('<')) << saved;
  const std::string arr = printIt("b12malArr := Array new: 2. b12malArr at: 2 put: 'xy'");
  ASSERT_NE(0u, arr.find('<')) << arr;
  const char* bads[] = {"Array new: 0", "Array new: 1", "'abc'", "b12malArr"};
  for (int i = 0; i < 4; ++i) {
    SCOPED_TRACE(bads[i]);
    const std::string n = std::to_string(i);
    const std::string put = std::string("B12Mal instVarAt: 2 put: (") + bads[i] + ")";
    ASSERT_NE(0u, printIt(put.c_str()).find('<'));
    expectDnu(("B12Mal new own" + n).c_str(), ("own" + n).c_str());
    EXPECT_EQ("5", printIt(("B12Mal new b12up" + n).c_str()));
    EXPECT_EQ("false", printIt(("B12Mal includesSelector: #own" + n).c_str()));
    EXPECT_EQ("nil", printIt(("B12Mal compiledMethodAt: #own" + n).c_str()));
    EXPECT_EQ("0", printIt("B12Mal selectors size"));
    const std::string extra = "extra" + n + "\n  ^9\n";
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B12Mal", 0, extra.c_str(), &err));
    EXPECT_STREQ("install failed", err.message);
  }
  ASSERT_NE(0u, printIt("B12Mal instVarAt: 2 put: b12malDict").find('<'));
  EXPECT_EQ("7", printIt("B12Mal new kept"));
}

// SPEC §3.9 削除, §3.10 クラス ID: Foo goes while an alias keeps it, and a new Foo comes. The old
// class keeps its row (its own name, its ID) before the new one; each ID reads, accepts and
// removes on its own class, and the old row's removal is refused: Foo binds the new class.
TEST_F(RemoveAbi, AliasedOldClassRowActsOnItsOwnClass) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Foo", "Object", "B12-Old"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Foo", 0, "old\n  ^1\n", &err)) << err.message;
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Foo class", printIt("Smalltalk at: #B12FooAlias put: B12Foo"));
  const std::int64_t oldId = idOf("B12Foo");
  ASSERT_GT(oldId, 0);
  EXPECT_EQ(oldId, idOf("B12FooAlias"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(oldId, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Foo"));
  EXPECT_EQ(0, idOf("B12Foo"));
  EXPECT_EQ(oldId, idOf("B12FooAlias"));
  ASSERT_EQ(AO_OK, defineClass("B12Foo", "Object", "B12-New"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Foo", 0, "new\n  ^2\n", &err)) << err.message;
  const std::int64_t newId = idOf("B12Foo");
  ASSERT_GT(newId, oldId);

  // Two rows named B12Foo: the old one first (smaller ID), each in its own category.
  std::vector<std::pair<std::int64_t, std::string>> rows;
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    std::int64_t id = 0;
    char name[128];
    char category[128];
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, &id, name, sizeof(name), category, sizeof(category)));
    if (std::strcmp(name, "B12Foo") == 0) {
      rows.emplace_back(id, category);
    }
  }
  ASSERT_EQ(2u, rows.size());
  EXPECT_EQ(std::make_pair(oldId, std::string("B12-Old")), rows[0]);
  EXPECT_EQ(std::make_pair(newId, std::string("B12-New")), rows[1]);

  // Each ID reads its own class.
  char buf[256];
  ASSERT_EQ(AO_OK, ao_browser_selector_at(oldId, 0, "user", 0, buf, sizeof(buf)));
  EXPECT_STREQ("old", buf);
  ASSERT_EQ(AO_OK, ao_browser_selector_at(newId, 0, "user", 0, buf, sizeof(buf)));
  EXPECT_STREQ("new", buf);

  // The old row cannot go: B12Foo binds the new class, which stays.
  EXPECT_EQ("class removal refused: B12Foo is not bound to this class",
            tryRemoveClassId(oldId, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("2", printIt("B12Foo new new"));
  EXPECT_EQ(newId, idOf("B12Foo"));

  // Accept and method removal on the old row reach the old class only.
  ASSERT_EQ(AO_OK, ao_accept_method_id(oldId, 0, "extra\n  ^3\n", &err)) << err.message;
  EXPECT_EQ("3", printIt("B12FooAlias new extra"));
  expectDnu("B12Foo new extra", "extra");
  EXPECT_EQ("", tryRemoveMethodId(oldId, 0, "old", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12FooAlias new old", "old");
  EXPECT_EQ("selector not found: B12Foo>>old", tryRemoveMethodId(newId, 0, "old", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("2", printIt("B12Foo new new"));

  // The new row goes; the alias keeps the old class and its row.
  EXPECT_EQ("", tryRemoveClassId(newId, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Foo"));
  EXPECT_EQ("3", printIt("B12FooAlias new extra"));
  EXPECT_EQ(oldId, idOf("B12FooAlias"));
  EXPECT_EQ(-1, ao_browser_protocol_count(newId, 0));
}

// SPEC §3.9 削除, §3.10 クラス ID: an unknown ID (0 or less, never issued, dropped) is refused
// with "unknown class id" by both removals and ao_accept_method_id, and nothing changes. A bad
// meta or a NULL selector or source keeps its old answer.
TEST_F(RemoveAbi, UnknownClassIdIsRefusedByRemoveAndAccept) {
  ASSERT_EQ(AO_OK, defineClass("B12Stale", "Object", "B12-Test"));
  const std::int64_t stale = idOf("B12Stale");
  ASSERT_GT(stale, 0);
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(stale, &rc));
  EXPECT_EQ(AO_OK, rc);
  for (const std::int64_t id :
       {std::int64_t{0}, std::int64_t{-3}, stale, std::numeric_limits<std::int64_t>::max()}) {
    SCOPED_TRACE(id);
    EXPECT_EQ("unknown class id", tryRemoveClassId(id, &rc));
    EXPECT_EQ(AO_ERR, rc);
    EXPECT_EQ("unknown class id", tryRemoveMethodId(id, 0, "printString", &rc));
    EXPECT_EQ(AO_ERR, rc);
    AoSpan err{};
    EXPECT_EQ(AO_ERR, ao_accept_method_id(id, 0, "b12staleFoo\n  ^1\n", &err));
    EXPECT_STREQ("unknown class id", err.message);
  }
  EXPECT_EQ("remove failed", tryRemoveMethodId(idOf("Object"), 2, "foo", &rc));
  EXPECT_EQ("remove failed", tryRemoveMethodId(idOf("Object"), 0, nullptr, &rc));
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_accept_method_id(idOf("Object"), 2, "b12staleFoo\n  ^1\n", &err));
  EXPECT_EQ(AO_ERR, ao_accept_method_id(idOf("Object"), 0, nullptr, &err));
  expectDnu("Object new b12staleFoo", "b12staleFoo");
}

// SPEC §3.9 削除, §3.10 クラス ID: the ID table is no root for what is alive. The kid is unbound by
// a DoIt, and nothing lists the classes after that (a list would prune the kid's entry first), so
// the table still holds the kid, and nothing else, when removeClassOf probes the superclass's
// subclasses: the kid does not count as a live subclass, and the superclass goes.
TEST_F(RemoveAbi, ClassIdTableDoesNotBlockSuperclassRemoval) {
  ASSERT_EQ(AO_OK, defineClass("B12TabPar", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12TabKid", "B12TabPar", "B12-Test"));
  const std::int64_t kid = idOf("B12TabKid");
  ASSERT_GT(kid, 0);
  EXPECT_EQ("nil", printIt("Smalltalk at: #B12TabKid put: nil"));
  ASSERT_TRUE(tableHolds(kid));
  ao::Session* s = ao::session();
  std::string reason;
  EXPECT_TRUE(ao::removeClassOf(*s->ctx, s->wk.named("B12TabPar"), &reason)) << reason;
  EXPECT_TRUE(tableHolds(kid));
  EXPECT_EQ("nil", printIt("B12TabPar"));
}

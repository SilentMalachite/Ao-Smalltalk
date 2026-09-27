#include "ao_abi.h"

#include "../src/Session.hpp"
#include "ao/Bootstrap.hpp"
#include "ao/Globals.hpp"
#include "ao/Heap.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
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

// The call's message. err starts non-empty, so an AO_OK that leaves it alone shows.
std::string tryRemoveMethod(const char* cls, int meta, const char* sel, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_method(cls, meta, sel, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
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

std::string tryRemoveClass(const char* cls, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_class(cls, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
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
  int methodRc = -9;
  int classRc = -9;
  std::string methodMsg;
  std::string classMsg;
};

// A transcript hook: the runtime is busy here (SPEC §3.10 再入).
void removeFromHook(const char*, int, int, void* user) {
  auto* b = static_cast<BusyRemove*>(user);
  AoSpan err{};
  b->methodRc = ao_remove_method("Object", 0, "b12busy", &err);
  b->methodMsg = err.message;
  b->classRc = ao_remove_class("B12Busy", &err);
  b->classMsg = err.message;
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
  EXPECT_EQ(AO_ERR, ao_browser_source("Object", 0, "b12foo", shown, sizeof(shown)));
  EXPECT_EQ(0, ao_browser_selector_count("Object", 0, "user"));
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

// SPEC §3.9 削除: only the side's own dictionary counts: an inherited selector, an unknown one, a
// name that is no class, an empty name or selector, and a bad meta are refused with their reasons.
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
  EXPECT_EQ("not a class: Processor", tryRemoveMethod("Processor", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: Smalltalk", tryRemoveMethod("Smalltalk", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: B12Nope", tryRemoveMethod("B12Nope", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: ", tryRemoveMethod("", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 2, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod(nullptr, 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 0, nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  // err may be NULL.
  EXPECT_EQ(AO_ERR, ao_remove_method("B12Child", 0, "nope", nullptr));
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
  EXPECT_EQ(AO_ERR, ao_browser_source("B12Src", 0, "blk", shown, sizeof(shown)));
}

// SPEC §3.9 削除, §6: the name reads nil (PushGlobal, no recompile), the instances keep their
// class and methods, and the Browser no longer lists the class.
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
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Inst", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("Object new b12useInst"));
  EXPECT_EQ("7", printIt("b12i answer"));
  EXPECT_EQ("'B12Inst'", printIt("b12i printString"));
  EXPECT_FALSE(browserLists("B12Inst"));
  EXPECT_EQ("not a class: B12Inst", tryRemoveClass("B12Inst", &rc));
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

// SPEC §3.9 削除: a fixed global (a Kernel class, a vendor stub), a name that is no class
// (Processor, Smalltalk, a metaclass, an unknown or empty name) and a NULL name are refused.
TEST_F(RemoveAbi, RemoveFixedGlobalIsRefused) {
  int rc = -9;
  EXPECT_EQ("class removal refused: Object is a fixed global", tryRemoveClass("Object", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("class removal refused: Bag is a fixed global", tryRemoveClass("Bag", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: Processor", tryRemoveClass("Processor", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: Smalltalk", tryRemoveClass("Smalltalk", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: B12Nope", tryRemoveClass("B12Nope", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: ", tryRemoveClass("", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveClass(nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("Smalltalk at: #B12Meta put: Object class").find("<"));
  EXPECT_EQ("not a class: B12Meta", tryRemoveClass("B12Meta", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(AO_ERR, ao_remove_class("Object", nullptr));
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("Object class", printIt("Object"));
  EXPECT_EQ("Bag class", printIt("Bag"));
}

// SPEC §3.9 削除: a Kernel class through an alias is one by identity, as ao_accept_method sees it.
TEST_F(RemoveAbi, RemoveKernelAliasIsRefused) {
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("SmallInteger class", printIt("Smalltalk at: #B12IntAlias put: SmallInteger"));
  int rc = -9;
  EXPECT_EQ("class removal refused: B12IntAlias is a kernel class", tryRemoveClass("B12IntAlias", &rc));
  EXPECT_EQ(AO_ERR, rc);
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
  ASSERT_EQ(AO_OK, defineClass("B12Anon", "B12Base", "B12-Test"));
  EXPECT_EQ("B12Anon", printIt("b12anon := B12Anon new"));
  // instVarAt:put: answers the value (ao_Object_instVarAt_put_).
  EXPECT_EQ("nil", printIt("B12Anon instVarAt: 4 put: nil"));
  EXPECT_EQ("", tryRemoveClass("B12Anon", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("class removal refused: B12Base has an unnamed subclass", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("nil", printIt("b12anon := nil"));
  EXPECT_EQ("", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Base"));
}

// SPEC §3.9 削除: only the named binding goes; an alias keeps the class reachable, and the
// Browser keeps listing it under its own name slot (unaffected by which global reaches it), not
// under the alias's name.
TEST_F(RemoveAbi, RemoveClassKeepsAliases) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Real", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Real", 0, "answer\n  ^9\n", &err)) << err.message;
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Real class", printIt("Smalltalk at: #B12Alias put: B12Real"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Real", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Real"));
  EXPECT_EQ("9", printIt("B12Alias new answer"));
  // classRows reads the class's own name slot, not the binding key it was reached through.
  EXPECT_FALSE(browserLists("B12Alias"));
  EXPECT_TRUE(browserLists("B12Real"));
  EXPECT_EQ("", tryRemoveClass("B12Alias", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_FALSE(browserLists("B12Alias"));
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

// SPEC §3.10 再入と例外: from a hook the runtime is busy, and both calls do nothing.
TEST_F(RemoveAbi, RemoveWhileBusyIsRefused) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12busy\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, defineClass("B12Busy", "Object", "B12-Test"));
  BusyRemove seen;
  ao_set_transcript_hook(removeFromHook, &seen);
  EXPECT_EQ("7", printIt("Transcript show: 'x'. 7"));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(AO_ERR, seen.methodRc);
  EXPECT_EQ("runtime is busy", seen.methodMsg);
  EXPECT_EQ(AO_ERR, seen.classRc);
  EXPECT_EQ("runtime is busy", seen.classMsg);
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
  const int protocols = ao_browser_protocol_count("Object", 0);
  int rc = -9;
  EXPECT_EQ(AO_ERR, ao_remove_method("Object", 0, "printString", &err));
  EXPECT_EQ(AO_ERR, ao_remove_method("B12Kid2", 0, "who", &err));
  EXPECT_EQ(AO_ERR, ao_remove_class("Object", &err));
  EXPECT_EQ(AO_ERR, ao_remove_class("B12Par", &err));
  EXPECT_EQ("class removal refused: B12Par has subclass B12Kid2", tryRemoveClass("B12Par", &rc));
  EXPECT_EQ(classes, ao_browser_class_count());
  EXPECT_EQ(tally, globalsTally());
  EXPECT_EQ(version, ao::session()->wk.globalsVersion());
  EXPECT_EQ(slots, ao::methodSourceRootSlots(false).size());
  EXPECT_EQ(protocols, ao_browser_protocol_count("Object", 0));
  EXPECT_EQ("'par'", printIt("B12Kid2 new who"));
  EXPECT_EQ("'3'", printIt("3 printString"));
  char shown[64];
  EXPECT_EQ(AO_OK, ao_browser_source("B12Par", 0, "who", shown, sizeof(shown)));
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

// SPEC §3.10: AoSpan.message holds 255 bytes; a longer reason is cut, never emptied.
TEST_F(RemoveAbi, LongNameCutsTheMessageAt255Bytes) {
  const std::string name(300, 'N');
  int rc = -9;
  const std::string got = tryRemoveClass(name.c_str(), &rc);
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(255u, got.size());
  EXPECT_EQ(("not a class: " + name).substr(0, 255), got);
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

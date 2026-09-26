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
    if (ao_browser_class_at(i, name, sizeof(name), category, sizeof(category)) == AO_OK &&
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

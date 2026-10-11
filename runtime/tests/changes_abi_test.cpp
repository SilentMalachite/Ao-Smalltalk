#include "ao_abi.h"

#include "../src/ChangeSet.hpp"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

namespace {

class ChangesAbi : public ::testing::Test {
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

struct SetRow {
  std::int64_t id = 0;
  std::string name;
};

// The sets in order, as ao_changes_count and ao_changes_at answer them.
std::vector<SetRow> sets() {
  std::vector<SetRow> out;
  const int n = ao_changes_count();
  for (int i = 0; i < n; ++i) {
    SetRow row;
    char name[256];
    EXPECT_EQ(AO_OK, ao_changes_at(i, &row.id, name, static_cast<int>(sizeof name)));
    row.name = name;
    out.push_back(row);
  }
  return out;
}

// The kind's mark; ASCII "-" for Removed.
std::string mark(int kind) {
  switch (kind) {
    case AO_CHANGE_ADDED:
      return "+";
    case AO_CHANGE_CHANGED:
      return "~";
    case AO_CHANGE_REMOVED:
      return "-";
    default:
      return "";
  }
}

// "<name><mark>" for each class of the set, in order.
std::vector<std::string> classesOf(std::int64_t set) {
  std::vector<std::string> out;
  const int n = ao_changes_class_count(set);
  for (int i = 0; i < n; ++i) {
    char name[256];
    int kind = -9;
    EXPECT_EQ(AO_OK, ao_changes_class_at(set, i, name, static_cast<int>(sizeof name), &kind));
    out.push_back(std::string(name) + mark(kind));
  }
  return out;
}

// "<selector><mark>" for one side of cls in the set, in byte order.
std::vector<std::string> methodsOf(std::int64_t set, const char* cls, int meta) {
  std::vector<std::string> out;
  const int n = ao_changes_method_count(set, cls, meta);
  for (int i = 0; i < n; ++i) {
    char selector[256];
    int kind = -9;
    EXPECT_EQ(AO_OK, ao_changes_method_at(set, cls, meta, i, selector,
                                          static_cast<int>(sizeof selector), &kind));
    out.push_back(std::string(selector) + mark(kind));
  }
  return out;
}

std::int64_t newSet(const char* name) {
  std::int64_t id = 0;
  AoSpan err{};
  EXPECT_EQ(AO_OK, ao_changes_new(name, &id, &err)) << err.message;
  return id;
}

// Runs body inside a transcript hook, where the runtime is busy (SPEC §3.10 再入).
template <typename Body>
void whileBusy(Body body) {
  struct Holder {
    Body* body;
  } holder{&body};
  ao_set_transcript_hook(
      [](const char*, int, int, void* user) { (*static_cast<Holder*>(user)->body)(); }, &holder);
  EXPECT_EQ("7", printIt("Transcript show: 'x'. 7"));
  ao_set_transcript_hook(nullptr, nullptr);
}

// A path in the temporary directory, unique to this process and name.
std::string tempPath(const std::string& name) {
  return (std::filesystem::temp_directory_path() /
          ("ao-p16-" + std::to_string(getpid()) + "-" + name))
      .string();
}

int defineClass(const char* name, const char* super, const char* ivars = "",
                const char* category = "P16-Test") {
  const std::string def = std::string(super) + " subclass: #" + name +
                          "\n  instanceVariableNames: '" + ivars +
                          "'\n  classVariableNames: ''\n  poolDictionaries: ''\n  category: '" +
                          category + "'";
  AoSpan err{};
  const int rc = ao_accept_class(def.c_str(), &err);
  EXPECT_EQ(AO_OK, rc) << err.message;
  return rc;
}

int accept(const char* cls, int meta, const char* source) {
  AoSpan err{};
  const int rc = ao_accept_method(cls, meta, source, &err);
  EXPECT_EQ(AO_OK, rc) << err.message;
  return rc;
}

// The removal's message; empty on AO_OK.
std::string removeMethod(const char* cls, int meta, const char* sel) {
  AoSpan err{};
  return ao_remove_method(ao_browser_class_id(cls), meta, sel, &err) == AO_OK ? "" : err.message;
}

std::string removeClass(const char* cls) {
  AoSpan err{};
  return ao_remove_class(ao_browser_class_id(cls), &err) == AO_OK ? "" : err.message;
}

// The index of the selected live frame labeled label, or -1.
int frameLabeled(const char* label) {
  const int n = ao_debug_frame_count();
  for (int i = 0; i < n; ++i) {
    char buf[256];
    if (ao_debug_frame_label(i, buf, static_cast<int>(sizeof buf)) == AO_OK &&
        std::strcmp(buf, label) == 0) {
      return i;
    }
  }
  return -1;
}

}  // namespace

// SPEC §3.9 Change Sorter, §3.10: a new session holds one empty set, Unnamed1, which is current.
TEST_F(ChangesAbi, NewSessionHasOneEmptyCurrentSet) {
  const std::vector<SetRow> all = sets();
  ASSERT_EQ(1u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_GT(all[0].id, 0);
  EXPECT_EQ(all[0].id, ao_changes_current());
  EXPECT_EQ(0, ao_changes_class_count(all[0].id));
  EXPECT_EQ(0, ao_changes_method_count(all[0].id, "Object", 0));
}

// SPEC §3.10: with no session the counts answer -1, the current set 0, and the writes AO_ERR.
TEST_F(ChangesAbi, NoSessionAnswersFailure) {
  const std::int64_t id = ao_changes_current();
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(-1, ao_changes_count());
  EXPECT_EQ(0, ao_changes_current());
  EXPECT_EQ(-1, ao_changes_class_count(id));
  std::int64_t made = 7;
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_new("Later", &made, &err));
  EXPECT_EQ(0, made);
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_set_current(id));
}

// SPEC §3.9 Change Sorter: New makes an empty set that is current at once; the others stay.
TEST_F(ChangesAbi, NewSetBecomesCurrent) {
  const std::int64_t first = ao_changes_current();
  const std::int64_t second = newSet("Fixes");
  EXPECT_NE(first, second);
  EXPECT_EQ(second, ao_changes_current());
  const std::vector<SetRow> all = sets();
  ASSERT_EQ(2u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_EQ("Fixes", all[1].name);
  EXPECT_EQ(second, all[1].id);
  EXPECT_EQ(AO_OK, ao_changes_set_current(first));
  EXPECT_EQ(first, ao_changes_current());
}

// SPEC §3.10: an empty or taken name is refused with its reason, and nothing changes.
TEST_F(ChangesAbi, NewSetRefusesEmptyAndTakenNames) {
  std::int64_t id = 7;
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_new("", &id, &err));
  EXPECT_STREQ("change set name empty", err.message);
  EXPECT_EQ(0, id);
  EXPECT_EQ(AO_ERR, ao_changes_new(nullptr, &id, &err));
  EXPECT_STREQ("change set name empty", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_new("Unnamed1", &id, &err));
  EXPECT_STREQ("change set name taken: Unnamed1", err.message);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  EXPECT_EQ(1, ao_changes_count());
}

// SPEC §3.10: an unknown set ID is refused by Make Current and by every read; so are a NULL class
// name and a meta other than 0 or 1.
TEST_F(ChangesAbi, UnknownSetIdIsRefused) {
  const std::int64_t current = ao_changes_current();
  EXPECT_EQ(AO_ERR, ao_changes_set_current(current + 1000));
  EXPECT_EQ(AO_ERR, ao_changes_set_current(0));
  EXPECT_EQ(current, ao_changes_current());
  EXPECT_EQ(-1, ao_changes_class_count(current + 1000));
  EXPECT_EQ(-1, ao_changes_method_count(current + 1000, "Object", 0));
  EXPECT_EQ(-1, ao_changes_method_count(current, nullptr, 0));
  EXPECT_EQ(-1, ao_changes_method_count(current, "Object", 2));
  char name[32];
  int kind = 9;
  EXPECT_EQ(AO_ERR, ao_changes_class_at(current + 1000, 0, name, static_cast<int>(sizeof name),
                                        &kind));
  EXPECT_EQ(AO_CHANGE_NONE, kind);
}

// SPEC §3.10 の文字列バッファ: a cut name is AO_ERR_RANGE with the ID; a NULL buffer, a length under
// 1 and an index out of range are AO_ERR with ID 0.
TEST_F(ChangesAbi, ReadsFollowBufferRule) {
  const std::int64_t id = newSet("LongerName");
  std::int64_t got = 0;
  char small[4];
  EXPECT_EQ(AO_ERR_RANGE, ao_changes_at(1, &got, small, static_cast<int>(sizeof small)));
  EXPECT_STREQ("Lon", small);
  EXPECT_EQ(id, got);
  EXPECT_EQ(AO_ERR, ao_changes_at(1, &got, nullptr, 8));
  EXPECT_EQ(0, got);
  EXPECT_EQ(AO_ERR, ao_changes_at(1, &got, small, 0));
  EXPECT_EQ(AO_ERR, ao_changes_at(2, &got, small, static_cast<int>(sizeof small)));
  EXPECT_EQ(AO_ERR, ao_changes_at(-1, &got, small, static_cast<int>(sizeof small)));
  char name[64];
  EXPECT_EQ(AO_OK, ao_changes_at(0, nullptr, name, static_cast<int>(sizeof name)));
  EXPECT_STREQ("Unnamed1", name);
}

// SPEC §3.9 Change Sorter: boot and a successful image load start over with one empty Unnamed1
// under a new ID; a failed load keeps the sets.
TEST_F(ChangesAbi, BootAndImageLoadStartOver) {
  const std::int64_t first = ao_changes_current();
  newSet("Kept");
  const std::string image = tempPath("start-over.aoimage");
  ASSERT_EQ(AO_OK, ao_image_save(image.c_str()));
  AoSpan err{};
  ASSERT_EQ(AO_ERR, ao_image_load(tempPath("missing.aoimage").c_str(), &err));
  EXPECT_EQ(2, ao_changes_count());
  ASSERT_EQ(AO_OK, ao_image_load(image.c_str(), &err)) << err.message;
  std::vector<SetRow> all = sets();
  ASSERT_EQ(1u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_NE(first, all[0].id);
  EXPECT_EQ(all[0].id, ao_changes_current());
  const std::int64_t loaded = all[0].id;
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  all = sets();
  ASSERT_EQ(1u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_NE(loaded, all[0].id);
  std::filesystem::remove(image);
}

// SPEC §3.10 再入: from a hook the writes do nothing and answer AO_ERR ("runtime is busy" with an
// err); the reads still answer.
TEST_F(ChangesAbi, NewAndMakeCurrentAreRefusedWhileBusy) {
  const std::int64_t first = ao_changes_current();
  const std::int64_t second = newSet("Second");
  int newRc = -9;
  std::string newMsg;
  int currentRc = -9;
  int count = -9;
  whileBusy([&] {
    AoSpan err{};
    std::int64_t id = 0;
    newRc = ao_changes_new("Busy", &id, &err);
    newMsg = err.message;
    currentRc = ao_changes_set_current(first);
    count = ao_changes_count();
  });
  EXPECT_EQ(AO_ERR, newRc);
  EXPECT_EQ("runtime is busy", newMsg);
  EXPECT_EQ(AO_ERR, currentRc);
  EXPECT_EQ(2, count);
  EXPECT_EQ(second, ao_changes_current());
}

// SPEC §3.9 種類の規則.
TEST(ChangeKinds, AcceptAndRemovalRules) {
  using ao::ChangeKind;
  using ao::kindAfterAccept;
  using ao::kindAfterRemoval;
  EXPECT_EQ(ChangeKind::Added, kindAfterAccept(std::nullopt, false));
  EXPECT_EQ(ChangeKind::Changed, kindAfterAccept(std::nullopt, true));
  EXPECT_EQ(ChangeKind::Added, kindAfterAccept(ChangeKind::Added, true));
  EXPECT_EQ(ChangeKind::Changed, kindAfterAccept(ChangeKind::Changed, true));
  EXPECT_EQ(ChangeKind::Changed, kindAfterAccept(ChangeKind::Removed, false));
  EXPECT_EQ(ChangeKind::Removed, kindAfterRemoval(std::nullopt));
  EXPECT_EQ(std::nullopt, kindAfterRemoval(ChangeKind::Added));
  EXPECT_EQ(ChangeKind::Removed, kindAfterRemoval(ChangeKind::Changed));
  EXPECT_EQ(ChangeKind::Removed, kindAfterRemoval(ChangeKind::Removed));
}

// SPEC §3.9 記録: a new method is Added and stays Added when accepted again; in a new set the same
// Accept replaces a method, so it is Changed there. The definition Accept records the class.
TEST_F(ChangesAbi, AcceptRecordsAddedThenChangedInANewSet) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Counter", "Object", "count");
  accept("P16Counter", 0, "increment\n  count := (count ifNil: [0]) + 1.\n  ^count");
  accept("P16Counter", 0, "increment\n  ^0");
  EXPECT_EQ(std::vector<std::string>{"P16Counter+"}, classesOf(first));
  EXPECT_EQ(std::vector<std::string>{"increment+"}, methodsOf(first, "P16Counter", 0));
  const std::int64_t second = newSet("Second");
  accept("P16Counter", 0, "increment\n  ^1");
  EXPECT_EQ(std::vector<std::string>{"P16Counter"}, classesOf(second));
  EXPECT_EQ(std::vector<std::string>{"increment~"}, methodsOf(second, "P16Counter", 0));
  EXPECT_EQ(std::vector<std::string>{"increment+"}, methodsOf(first, "P16Counter", 0));
}

// SPEC §3.9 記録: a class-side Accept goes to the class side of the class's entry.
TEST_F(ChangesAbi, ClassSideAcceptIsRecordedOnTheClassSide) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Side", "Object");
  accept("P16Side", 1, "make\n  ^self new");
  EXPECT_EQ(std::vector<std::string>{}, methodsOf(set, "P16Side", 0));
  EXPECT_EQ(std::vector<std::string>{"make+"}, methodsOf(set, "P16Side", 1));
}

// SPEC §3.9 記録: a definition of a name that names a class is Changed; a reshape records only the
// definition, not the methods it carried.
TEST_F(ChangesAbi, ClassDefinitionAcceptIsChangedForAnExistingClass) {
  defineClass("P16Shape", "Object");
  accept("P16Shape", 0, "one\n  ^1");
  const std::int64_t set = newSet("Reshape");
  defineClass("P16Shape", "Object", "a b");
  EXPECT_EQ(std::vector<std::string>{"P16Shape~"}, classesOf(set));
  EXPECT_EQ(0, ao_changes_method_count(set, "P16Shape", 0));
  EXPECT_EQ("1", printIt("P16Shape new one"));
}

// SPEC §3.9 種類の規則: removing a method added in the set forgets it; removing an older one is
// Removed, and accepting it again is Changed.
TEST_F(ChangesAbi, RemoveMethodFollowsTheKindRules) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Gone", "Object");
  accept("P16Gone", 0, "temp\n  ^1");
  accept("P16Gone", 0, "kept\n  ^2");
  EXPECT_EQ("", removeMethod("P16Gone", 0, "temp"));
  EXPECT_EQ(std::vector<std::string>{"kept+"}, methodsOf(first, "P16Gone", 0));
  const std::int64_t second = newSet("Later");
  EXPECT_EQ("", removeMethod("P16Gone", 0, "kept"));
  EXPECT_EQ(std::vector<std::string>{"kept-"}, methodsOf(second, "P16Gone", 0));
  accept("P16Gone", 0, "kept\n  ^3");
  EXPECT_EQ(std::vector<std::string>{"kept~"}, methodsOf(second, "P16Gone", 0));
}

// SPEC §3.9 種類の規則: a class added in the set goes with its entry; an older class becomes
// Removed and its method entries go.
TEST_F(ChangesAbi, RemoveClassFollowsTheKindRules) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Old", "Object");
  defineClass("P16New", "Object");
  accept("P16New", 0, "x\n  ^1");
  EXPECT_EQ("", removeClass("P16New"));
  EXPECT_EQ(std::vector<std::string>{"P16Old+"}, classesOf(first));
  const std::int64_t second = newSet("Later");
  accept("P16Old", 0, "y\n  ^2");
  EXPECT_EQ("", removeClass("P16Old"));
  EXPECT_EQ(std::vector<std::string>{"P16Old-"}, classesOf(second));
  EXPECT_EQ(0, ao_changes_method_count(second, "P16Old", 0));
}

// SPEC §3.9 記録: methodsFor: chunks through ao_accept_class are recorded; they still keep no
// source (SPEC §3.10 ソースはイメージに書かない).
TEST_F(ChangesAbi, MethodsForChunkAcceptIsRecordedWithoutSource) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Chunk", "Object");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!P16Chunk methodsFor: 'x'!\nviaChunk\n  ^7! !", &err))
      << err.message;
  EXPECT_EQ(std::vector<std::string>{"viaChunk+"}, methodsOf(set, "P16Chunk", 0));
  char buf[256];
  EXPECT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("P16Chunk"), 0, "viaChunk",
                                               buf, static_cast<int>(sizeof buf)));
}

// SPEC §3.9 記録: the Debugger's Accept goes through the same Accept, so it is recorded.
TEST_F(ChangesAbi, DebuggerAcceptIsRecorded) {
  defineClass("P16Live", "Object");
  accept("P16Live", 0, "answer\n  self halt.\n  ^1");
  const std::int64_t set = newSet("Debugger");
  ao_set_debug_mode(AO_DEBUG_LIVE);
  const char* source = "P16Live new answer";
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_ERR_HALT, ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_PRINTIT,
                                 out, static_cast<int>(sizeof out), &err));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  const int frame = frameLabeled("P16Live>>answer");
  ASSERT_GE(frame, 0);
  EXPECT_EQ(AO_ERR_HALT, ao_debug_accept(pid, frame, "answer\n  ^42", out,
                                         static_cast<int>(sizeof out), &err))
      << err.message;
  EXPECT_EQ(std::vector<std::string>{"answer~"}, methodsOf(set, "P16Live", 0));
  EXPECT_EQ(AO_OK, ao_debug_abort(pid));
}

// SPEC §3.9 記録: what is refused records nothing: a compile error, a native overwrite, a native
// removal.
TEST_F(ChangesAbi, RefusedAcceptAndRemovalRecordNothing) {
  const std::int64_t set = ao_changes_current();
  AoSpan err{};
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, "broken\n  ^(", &err));
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, "printString\n  ^'x'", &err));
  EXPECT_EQ(AO_ERR, ao_remove_method(ao_browser_class_id("Object"), 0, "printString", &err));
  EXPECT_EQ(0, ao_changes_class_count(set));
}

// SPEC §3.9 記録: the vendor-style LOAD_ORDER file-in records nothing.
TEST_F(ChangesAbi, LoadOrderFileInRecordsNothing) {
  const std::filesystem::path dir = tempPath("load-order");
  std::filesystem::create_directories(dir);
  std::ofstream(dir / "LOAD_ORDER") << "a.st\n";
  std::ofstream(dir / "a.st") << "Object subclass: #P16Vendor\n"
                                 "  instanceVariableNames: ''\n"
                                 "  classVariableNames: ''\n"
                                 "  poolDictionaries: ''\n"
                                 "  category: 'P16-Vendor'!\n\n"
                                 "!P16Vendor methodsFor: 'x'!\n"
                                 "v\n  ^1! !\n";
  ASSERT_EQ(AO_OK, ao_filein_load_order((dir / "LOAD_ORDER").c_str()));
  EXPECT_EQ("1", printIt("P16Vendor new v"));
  EXPECT_EQ(0, ao_changes_class_count(ao_changes_current()));
  std::filesystem::remove_all(dir);
}

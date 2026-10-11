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

std::string readFile(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  std::ostringstream all;
  all << in.rdbuf();
  return all.str();
}

// The file out after its header chunk (whose date changes).
std::string afterHeader(const std::string& text) {
  const std::size_t end = text.find("\"!\n\n");
  return end == std::string::npos ? text : text.substr(end + 4);
}

// The set filed out to a temporary file, read back.
std::string fileOut(std::int64_t set, const std::string& name) {
  const std::string path = tempPath(name);
  AoSpan err{};
  EXPECT_EQ(AO_OK, ao_changes_file_out(set, path.c_str(), &err)) << err.message;
  const std::string text = readFile(path);
  std::filesystem::remove(path);
  return text;
}

// The Browser's source of the method, or "<rc>" when it has none.
std::string sourceOf(const char* cls, int meta, const char* sel) {
  char buf[1024];
  const int rc =
      ao_browser_source(ao_browser_class_id(cls), meta, sel, buf, static_cast<int>(sizeof buf));
  return rc == AO_OK ? std::string(buf) : "<" + std::to_string(rc) + ">";
}

std::string writeTemp(const std::string& name, const std::string& text) {
  const std::string path = tempPath(name);
  std::ofstream(path, std::ios::binary) << text;
  return path;
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

// SPEC §3.10 の文字列バッファ: a buffer with buf_len > 0 ends in NUL even when a read answers AO_ERR
// (a bad index, an unknown ID, a NULL class name, a bad meta, no session).
TEST_F(ChangesAbi, RefusedReadsLeaveTheBufferEndingInNul) {
  defineClass("P16Read", "Object");
  const std::int64_t set = ao_changes_current();
  accept("P16Read", 0, "m\n  ^1");
  std::int64_t got = 0;
  int kind = -9;
  const auto sentinel = [](char (&b)[4]) { std::memset(b, 'X', sizeof b); };
  char b[4];
  sentinel(b);
  EXPECT_EQ(AO_ERR, ao_changes_at(5, &got, b, static_cast<int>(sizeof b)));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR, ao_changes_class_at(set + 1000, 0, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR, ao_changes_class_at(set, 5, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR, ao_changes_method_at(set, nullptr, 0, 0, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR,
            ao_changes_method_at(set, "P16Read", 2, 0, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR,
            ao_changes_method_at(set + 1000, "P16Read", 0, 0, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR,
            ao_changes_method_at(set, "P16Read", 0, 5, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  sentinel(b);
  EXPECT_EQ(AO_ERR, ao_changes_at(0, &got, b, static_cast<int>(sizeof b)));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR, ao_changes_class_at(set, 0, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
  sentinel(b);
  EXPECT_EQ(AO_ERR,
            ao_changes_method_at(set, "P16Read", 0, 0, b, static_cast<int>(sizeof b), &kind));
  EXPECT_EQ('\0', b[0]);
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

// SPEC §3.9 Change Sorter: Remove from Change Set forgets the entry; the method stays.
TEST_F(ChangesAbi, DropForgetsTheEntryAndKeepsTheMethod) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Drop", "Object");
  accept("P16Drop", 0, "a\n  ^1");
  accept("P16Drop", 0, "b\n  ^2");
  AoSpan err{};
  std::strcpy(err.message, "stale");
  ASSERT_EQ(AO_OK, ao_changes_drop(set, "P16Drop", 0, "a", &err)) << err.message;
  EXPECT_STREQ("", err.message);
  EXPECT_EQ(std::vector<std::string>{"b+"}, methodsOf(set, "P16Drop", 0));
  EXPECT_EQ("1", printIt("P16Drop new a"));
  // selector NULL: the class entry with its methods; meta is not read.
  ASSERT_EQ(AO_OK, ao_changes_drop(set, "P16Drop", 7, nullptr, &err)) << err.message;
  EXPECT_EQ(0, ao_changes_class_count(set));
  EXPECT_EQ("2", printIt("P16Drop new b"));
}

// SPEC §3.10 チェンジセット: a drop of what the set does not hold, or in an unknown set, says why.
TEST_F(ChangesAbi, DropRefusesWhatTheSetDoesNotHold) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Miss", "Object");
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, "P16Miss", 1, "nope", &err));
  EXPECT_STREQ("not in change set: P16Miss class>>nope", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, "Nowhere", 0, nullptr, &err));
  EXPECT_STREQ("not in change set: Nowhere", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set + 1000, "P16Miss", 0, nullptr, &err));
  EXPECT_STREQ("unknown change set", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, nullptr, 0, nullptr, &err));
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, "P16Miss", 2, "x", &err));
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(std::vector<std::string>{"P16Miss+"}, classesOf(set));
}

// SPEC §3.9 Change Sorter, §3.10: a method entry moves with its kind and replaces the target's;
// the source's emptied class entry goes.
TEST_F(ChangesAbi, MoveMethodEntryKeepsItsKind) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Move", "Object");
  accept("P16Move", 0, "m\n  ^1");
  const std::int64_t second = newSet("Second");
  accept("P16Move", 0, "m\n  ^2");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_move(second, first, "P16Move", 0, "m", &err)) << err.message;
  EXPECT_EQ(0, ao_changes_class_count(second));
  EXPECT_EQ(std::vector<std::string>{"P16Move+"}, classesOf(first));
  EXPECT_EQ(std::vector<std::string>{"m~"}, methodsOf(first, "P16Move", 0));
}

// SPEC §3.10: a class entry moves with its methods; the target keeps its definition kind when the
// moved entry has none, and gets a class entry when it has none.
TEST_F(ChangesAbi, MoveClassEntryMergesIntoTheTarget) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Merge", "Object");
  accept("P16Merge", 0, "x\n  ^1");
  const std::int64_t second = newSet("Second");
  accept("P16Merge", 1, "y\n  ^2");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_move(second, first, "P16Merge", 0, nullptr, &err)) << err.message;
  EXPECT_EQ(0, ao_changes_class_count(second));
  EXPECT_EQ(std::vector<std::string>{"P16Merge+"}, classesOf(first));
  EXPECT_EQ(std::vector<std::string>{"x+"}, methodsOf(first, "P16Merge", 0));
  EXPECT_EQ(std::vector<std::string>{"y+"}, methodsOf(first, "P16Merge", 1));
  const std::int64_t third = newSet("Third");
  ASSERT_EQ(AO_OK, ao_changes_move(first, third, "P16Merge", 1, "y", &err)) << err.message;
  EXPECT_EQ(std::vector<std::string>{"P16Merge"}, classesOf(third));
  EXPECT_EQ(std::vector<std::string>{"y+"}, methodsOf(third, "P16Merge", 1));
}

// SPEC §3.10: a move to the same set or to an unknown one is refused, and nothing moves.
TEST_F(ChangesAbi, MoveRefusesSameOrUnknownSet) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Stay", "Object");
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_move(set, set, "P16Stay", 0, nullptr, &err));
  EXPECT_STREQ("same change set", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_move(set, set + 1000, "P16Stay", 0, nullptr, &err));
  EXPECT_STREQ("unknown change set", err.message);
  const std::int64_t other = newSet("Other");
  EXPECT_EQ(AO_ERR, ao_changes_move(set, other, "P16Stay", 0, "none", &err));
  EXPECT_STREQ("not in change set: P16Stay>>none", err.message);
  EXPECT_EQ(std::vector<std::string>{"P16Stay+"}, classesOf(set));
  EXPECT_EQ(0, ao_changes_class_count(other));
}

// SPEC §3.10 再入: from a hook drop and move do nothing and say "runtime is busy".
TEST_F(ChangesAbi, DropAndMoveAreRefusedWhileBusy) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Busy", "Object");
  const std::int64_t second = newSet("Second");
  int dropRc = -9;
  int moveRc = -9;
  std::string dropMsg;
  std::string moveMsg;
  whileBusy([&] {
    AoSpan err{};
    dropRc = ao_changes_drop(first, "P16Busy", 0, nullptr, &err);
    dropMsg = err.message;
    moveRc = ao_changes_move(first, second, "P16Busy", 0, nullptr, &err);
    moveMsg = err.message;
  });
  EXPECT_EQ(AO_ERR, dropRc);
  EXPECT_EQ("runtime is busy", dropMsg);
  EXPECT_EQ(AO_ERR, moveRc);
  EXPECT_EQ("runtime is busy", moveMsg);
  EXPECT_EQ(std::vector<std::string>{"P16Busy+"}, classesOf(first));
}

// SPEC §3.9 File Out: the header, the definitions, the methods by class and side, then the
// removals as comment chunks.
TEST_F(ChangesAbi, FileOutWritesHeaderDefinitionsMethodsAndRemovals) {
  defineClass("P16Counter", "Object", "count", "Examples");
  accept("P16Counter", 0, "reset\n  count := 0");
  const std::int64_t set = newSet("Unnamed2");
  defineClass("P16Counter", "Object", "count", "Examples");
  accept("P16Counter", 0, "increment\n  count := (count ifNil: [0]) + 1.\n  ^count");
  accept("P16Counter", 1, "new\n  ^super new reset");
  EXPECT_EQ("", removeMethod("P16Counter", 0, "reset"));
  const std::string text = fileOut(set, "counter.st");
  char version[32];
  ASSERT_EQ(AO_OK, ao_version(version, static_cast<int>(sizeof version)));
  EXPECT_TRUE(std::regex_search(
      text, std::regex("^\"Change Set: Unnamed2\nDate: [0-9]{4}-[0-9]{2}-[0-9]{2}\nAo ")))
      << text;
  EXPECT_NE(std::string::npos, text.find("\nAo " + std::string(version) + "\"!\n\n")) << text;
  EXPECT_EQ(
      "Object subclass: #P16Counter\n"
      "  instanceVariableNames: 'count'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Examples'!\n"
      "\n"
      "!P16Counter methodsFor: 'user'!\n"
      "increment\n"
      "  count := (count ifNil: [0]) + 1.\n"
      "  ^count! !\n"
      "\n"
      "!P16Counter class methodsFor: 'user'!\n"
      "new\n"
      "  ^super new reset! !\n"
      "\n"
      "\"Removed: P16Counter>>reset\"!\n",
      afterHeader(text));
}

// SPEC §3.9 File Out: a superclass in the set is defined before its subclasses, whatever order the
// set recorded them in.
TEST_F(ChangesAbi, FileOutDefinesSuperclassesFirst) {
  defineClass("P16Base", "Object");
  defineClass("P16Sub", "P16Base");
  const std::int64_t set = newSet("Order");
  defineClass("P16Sub", "P16Base", "", "Other");
  defineClass("P16Base", "Object", "", "Other");
  EXPECT_EQ((std::vector<std::string>{"P16Sub~", "P16Base~"}), classesOf(set));
  const std::string body = afterHeader(fileOut(set, "order.st"));
  const std::size_t base = body.find("Object subclass: #P16Base");
  const std::size_t sub = body.find("P16Base subclass: #P16Sub");
  ASSERT_NE(std::string::npos, base) << body;
  ASSERT_NE(std::string::npos, sub) << body;
  EXPECT_LT(base, sub);
}

// SPEC §3.8, §3.9 File Out: every `!` is doubled, in strings and comments too, and a `"` in the
// set's name is doubled in the header comment.
TEST_F(ChangesAbi, FileOutDoublesBangsAndQuotes) {
  const std::int64_t set = newSet("Fix \"1\"!");
  defineClass("P16Bang", "Object");
  accept("P16Bang", 0, "shout\n  \"Loud!\"\n  ^'Hi!'");
  const std::string text = fileOut(set, "bang.st");
  EXPECT_EQ(0u, text.find("\"Change Set: Fix \"\"1\"\"!!\nDate: ")) << text;
  EXPECT_NE(std::string::npos, text.find("shout\n  \"Loud!!\"\n  ^'Hi!!'! !\n")) << text;
}

// SPEC §3.9 File Out: a method without source is a comment chunk after the removals, not code.
TEST_F(ChangesAbi, FileOutWritesACommentForAMethodWithoutSource) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16NoSrc", "Object");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!P16NoSrc methodsFor: 'x'!\nlost\n  ^1! !", &err))
      << err.message;
  const std::string body = afterHeader(fileOut(set, "nosrc.st"));
  EXPECT_EQ(std::string::npos, body.find("methodsFor:")) << body;
  EXPECT_NE(std::string::npos, body.find("\"P16NoSrc>>lost source not available\"!\n")) << body;
}

// SPEC §3.9 File Out item 4: the comments for methods without source come before those for
// classes without a definition.
TEST_F(ChangesAbi, FileOutWritesMissingSourcesBeforeMissingDefinitions) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16NoSrc", "Object");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!P16NoSrc methodsFor: 'x'!\nlost\n  ^1! !", &err))
      << err.message;
  defineClass("P16Gone", "Object");
  EXPECT_EQ("3", printIt("Smalltalk at: #P16Gone put: 3"));
  const std::string body = afterHeader(fileOut(set, "missing.st"));
  const std::size_t source = body.find("\"P16NoSrc>>lost source not available\"!\n");
  const std::size_t definition = body.find("\"P16Gone definition not available\"!\n");
  ASSERT_NE(std::string::npos, source) << body;
  ASSERT_NE(std::string::npos, definition) << body;
  EXPECT_LT(source, definition) << body;
}

// SPEC §3.9 File Out items 2 and 4: a name resolves as ao_browser_class_id does (the class Smalltalk
// binds it to), so File Out writes the definition and the source the Change Sorter shows for it.
TEST_F(ChangesAbi, FileOutResolvesNamesLikeTheBrowser) {
  defineClass("P16Bar", "Object", "", "Other");
  accept("P16Bar", 0, "m\n  ^#bar");
  const std::int64_t set = newSet("Rebound");
  defineClass("P16Foo", "Object");
  accept("P16Foo", 0, "m\n  ^#foo");
  EXPECT_EQ("true", printIt("(Smalltalk at: #P16Foo put: P16Bar) == P16Bar"));
  const std::int64_t id = ao_browser_class_id("P16Foo");
  ASSERT_NE(0, id);
  char definition[1024];
  ASSERT_EQ(AO_OK,
            ao_browser_class_definition(id, definition, static_cast<int>(sizeof definition)));
  char source[1024];
  ASSERT_EQ(AO_OK, ao_browser_source(id, 0, "m", source, static_cast<int>(sizeof source)));
  const std::string body = afterHeader(fileOut(set, "rebound.st"));
  EXPECT_EQ(std::string(definition) + "!\n\n!P16Foo methodsFor: 'user'!\n" + source + "! !\n\n",
            body);
}

// SPEC §3.9 File Out: a removed class is a comment chunk, and its definition is not written.
TEST_F(ChangesAbi, FileOutWritesAClassRemovalAsAComment) {
  defineClass("P16Scratch", "Object");
  const std::int64_t set = newSet("Removals");
  EXPECT_EQ("", removeClass("P16Scratch"));
  EXPECT_EQ("\"Removed class: P16Scratch\"!\n", afterHeader(fileOut(set, "scratch.st")));
}

// SPEC §3.10 チェンジセット: an unknown set, a path that cannot be written and a NULL path say why.
TEST_F(ChangesAbi, FileOutRefusesUnknownSetAndUnwritablePath) {
  AoSpan err{};
  const std::int64_t set = ao_changes_current();
  EXPECT_EQ(AO_ERR, ao_changes_file_out(set + 1000, tempPath("x.st").c_str(), &err));
  EXPECT_STREQ("unknown change set", err.message);
  const std::string bad = tempPath("no-such-dir") + "/x.st";
  EXPECT_EQ(AO_ERR, ao_changes_file_out(set, bad.c_str(), &err));
  EXPECT_EQ("cannot write file: " + bad, std::string(err.message));
  EXPECT_EQ(AO_ERR, ao_changes_file_out(set, nullptr, &err));
  EXPECT_STREQ("file out failed", err.message);
}

// SPEC §3.10 再入: from a hook File Out writes nothing and says "runtime is busy".
TEST_F(ChangesAbi, FileOutIsRefusedWhileBusy) {
  const std::string path = tempPath("busy.st");
  int rc = -9;
  std::string msg;
  whileBusy([&] {
    AoSpan err{};
    rc = ao_changes_file_out(ao_changes_current(), path.c_str(), &err);
    msg = err.message;
  });
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("runtime is busy", msg);
  EXPECT_FALSE(std::filesystem::exists(path));
}

// SPEC §3.10 File In... (P16): the methods keep their sources, and the definitions and methods go
// into the current set.
TEST_F(ChangesAbi, FileInKeepsSourcesAndRecordsInTheCurrentSet) {
  const std::string path = writeTemp("filein.st",
                                     "Object subclass: #P16In\n"
                                     "  instanceVariableNames: 'n'\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n\n"
                                     "!P16In methodsFor: 'user'!\n"
                                     "n\n  ^n ifNil: [0]!\n\n"
                                     "bump\n  n := self n + 1! !\n\n"
                                     "!P16In class methodsFor: 'user'!\n"
                                     "fresh\n  ^self new! !\n");
  AoSpan err{};
  std::strcpy(err.message, "stale");
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_STREQ("", err.message);
  const std::int64_t set = ao_changes_current();
  EXPECT_EQ(std::vector<std::string>{"P16In+"}, classesOf(set));
  EXPECT_EQ((std::vector<std::string>{"bump+", "n+"}), methodsOf(set, "P16In", 0));
  EXPECT_EQ(std::vector<std::string>{"fresh+"}, methodsOf(set, "P16In", 1));
  EXPECT_EQ("n\n  ^n ifNil: [0]", sourceOf("P16In", 0, "n"));
  EXPECT_EQ("bump\n  n := self n + 1", sourceOf("P16In", 0, "bump"));
  EXPECT_EQ("fresh\n  ^self new", sourceOf("P16In", 1, "fresh"));
  EXPECT_EQ("0", printIt("P16In fresh n"));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: a definition of a class that exists keeps it (as ao_accept_class), so its
// other methods stay; the vendor file-in would rebind the name to a new class.
TEST_F(ChangesAbi, FileInKeepsTheMethodsOfAnExistingClass) {
  defineClass("P16Keep", "Object", "", "P16-In");
  accept("P16Keep", 0, "old\n  ^1");
  const std::int64_t set = newSet("FileIn");
  const std::string path = writeTemp("keep.st",
                                     "Object subclass: #P16Keep\n"
                                     "  instanceVariableNames: ''\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n\n"
                                     "!P16Keep methodsFor: 'user'!\n"
                                     "added\n  ^2! !\n");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_EQ("1", printIt("P16Keep new old"));
  EXPECT_EQ("2", printIt("P16Keep new added"));
  EXPECT_EQ(std::vector<std::string>{"P16Keep~"}, classesOf(set));
  EXPECT_EQ(std::vector<std::string>{"added+"}, methodsOf(set, "P16Keep", 0));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: an error answers the first one with its span in the file; the chunks
// before it stay applied and recorded.
TEST_F(ChangesAbi, FileInErrorAnswersTheFirstErrorAndKeepsEarlierChunks) {
  const std::string text =
      "Object subclass: #P16Early\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P16-In'!\n\n"
      "!P16Acount methodsFor: 'user'!\n"
      "x\n  ^1! !\n";
  const std::string path = writeTemp("broken.st", text);
  AoSpan err{};
  ASSERT_EQ(AO_ERR, ao_filein_file(path.c_str(), &err));
  EXPECT_STREQ("missing class: P16Acount", err.message);
  ASSERT_LT(err.start, err.end);
  EXPECT_EQ("P16Acount methodsFor: 'user'", text.substr(err.start, err.end - err.start));
  EXPECT_EQ(std::vector<std::string>{"P16Early+"}, classesOf(ao_changes_current()));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: an unreadable path and a NULL path say why.
TEST_F(ChangesAbi, FileInRefusesAnUnreadablePath) {
  const std::string path = tempPath("missing.st");
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_filein_file(path.c_str(), &err));
  EXPECT_EQ("cannot read: " + path, std::string(err.message));
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  EXPECT_EQ(AO_ERR, ao_filein_file(nullptr, &err));
  EXPECT_STREQ("file-in failed", err.message);
}

// SPEC §3.9 File Out, §3.10 File In...: a filed-out set files back in after a new boot with the same
// definitions and sources, recorded as added, so filing it out again writes the same body. The set's
// name has a quote and a bang: the header escapes them and File In reads the header as a comment.
TEST_F(ChangesAbi, FiledOutSetFilesBackInAfterBoot) {
  newSet("Fix \"1\"!");
  defineClass("P16Trip", "Object", "count", "Trips");
  accept("P16Trip", 0, "increment\n  count := (count ifNil: [0]) + 1.\n  ^count");
  accept("P16Trip", 1, "start\n  ^self new increment; yourself");
  accept("P16Trip", 0, "shout\n  ^'Hi!'");
  const std::string path = tempPath("trip.st");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_file_out(ao_changes_current(), path.c_str(), &err)) << err.message;
  const std::string text = readFile(path);
  EXPECT_EQ(0u, text.find("\"Change Set: Fix \"\"1\"\"!!\nDate: ")) << text;
  const std::string first = afterHeader(text);
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_EQ("shout\n  ^'Hi!'", sourceOf("P16Trip", 0, "shout"));
  EXPECT_EQ("2", printIt("P16Trip start increment"));
  EXPECT_EQ(first, afterHeader(fileOut(ao_changes_current(), "trip-again.st")));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: a Kernel class's methodsFor: chunk is not refused wholesale, so a File Out of
// a set that extends Object files back in after a new boot, and the class chunk after it is read.
TEST_F(ChangesAbi, FiledOutKernelExtensionFilesBackIn) {
  accept("Object", 0, "p16ext\n  ^41 + 1");
  defineClass("P16Z", "Object");
  accept("P16Z", 0, "z\n  ^7");
  const std::string path = tempPath("kernel-ext.st");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_file_out(ao_changes_current(), path.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_EQ("42", printIt("3 p16ext"));
  EXPECT_EQ("42", printIt("Object new p16ext"));
  EXPECT_EQ("7", printIt("P16Z new z"));
  const std::int64_t set = ao_changes_current();
  EXPECT_EQ((std::vector<std::string>{"P16Z+", "Object"}), classesOf(set));
  EXPECT_EQ(std::vector<std::string>{"p16ext+"}, methodsOf(set, "Object", 0));
  EXPECT_EQ(std::vector<std::string>{"z+"}, methodsOf(set, "P16Z", 0));
  EXPECT_EQ("p16ext\n  ^41 + 1", sourceOf("Object", 0, "p16ext"));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: on a Kernel class a selector that finds a native is refused as a method-level
// error (as Accept refuses it); the file goes on, the other methods and later chunks apply.
TEST_F(ChangesAbi, FileInRefusesNativeOverwriteOnKernelClassWithoutStopping) {
  const std::string path = writeTemp("kernel-native.st",
                                     "!SmallInteger methodsFor: 'x'!\n"
                                     "<= other\n  ^false!\n"
                                     "p16k\n  ^1! !\n\n"
                                     "Object subclass: #P16After\n"
                                     "  instanceVariableNames: ''\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n");
  AoSpan err{};
  ASSERT_EQ(AO_ERR, ao_filein_file(path.c_str(), &err));
  EXPECT_STREQ("native selector overwrite refused: <=", err.message);
  EXPECT_LT(err.start, err.end);
  EXPECT_EQ("1", printIt("3 p16k"));
  EXPECT_NE(0, ao_browser_class_id("P16After"));
  EXPECT_EQ("true", printIt("3 <= 4"));
  EXPECT_EQ(std::vector<std::string>{"p16k+"}, methodsOf(ao_changes_current(), "SmallInteger", 0));
  std::filesystem::remove(path);
}

// SPEC §3.10 再入: from a hook File In applies nothing and says "runtime is busy".
TEST_F(ChangesAbi, FileInIsRefusedWhileBusy) {
  const std::string path = writeTemp("busy-in.st",
                                     "Object subclass: #P16BusyIn\n"
                                     "  instanceVariableNames: ''\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n");
  int rc = -9;
  std::string msg;
  whileBusy([&] {
    AoSpan err{};
    rc = ao_filein_file(path.c_str(), &err);
    msg = err.message;
  });
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("runtime is busy", msg);
  EXPECT_EQ(0, ao_browser_class_id("P16BusyIn"));
  std::filesystem::remove(path);
}

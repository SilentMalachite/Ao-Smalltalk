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

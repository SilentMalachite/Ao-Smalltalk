#include "ao_abi.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

class SessionAbi : public ::testing::Test {
 protected:
  void TearDown() override { ao_runtime_shutdown(); }
};

TEST_F(SessionAbi, BootThenImageRoundTripKeepsOnePlusTwo) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const char* path = "session-abi-roundtrip.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  ao_runtime_shutdown();
  std::remove(path);
}

TEST_F(SessionAbi, SecondBootFailsUntilShutdown) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_runtime_boot());
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
}

TEST_F(SessionAbi, MissingLoadOrderIsError) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_filein_load_order("/no/such/LOAD_ORDER"));
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, PrintItInsertsThree) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, WorkspaceTempSurvivesNextEval) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("x := 4", 6, AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval("x", 1, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("4", out);
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, KnownGlobalStaysGlobal) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("Object new isNil", 16, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("false", out);
  EXPECT_EQ(AO_ERR_COMPILE, ao_eval("Object := 1", 10, AO_EVAL_DOIT, out, 64, &err));
  const char* assign = "Object := 1";
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_eval(assign, static_cast<int>(std::strlen(assign)), AO_EVAL_DOIT, out, 64, &err));
  EXPECT_NE(std::string::npos, std::string(err.message).find("cannot assign"));
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, BlockAssignmentDoesNotUpdateWorkspaceBinding) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  const char* src = "x := 1.\n[ x := 2 ] value.\n^x";
  ASSERT_EQ(AO_OK,
            ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("1", out);
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, WorkspaceResetDropsBindings) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("x := 4", 6, AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_workspace_reset());
  ASSERT_EQ(AO_OK, ao_eval("x", 1, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, InspectItPrintsAndNotifiesHook) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  struct Seen {
    std::string cls;
    std::string text;
    int calls = 0;
  } seen;
  ao_set_inspect_hook(
      [](const char* cls, const char* text, void* user) {
        auto* slot = static_cast<Seen*>(user);
        slot->calls += 1;
        slot->cls = cls == nullptr ? "" : cls;
        slot->text = text == nullptr ? "" : text;
      },
      &seen);
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_INSPECTIT, out, 64, &err));
  EXPECT_STREQ("3", out);
  EXPECT_EQ(1, seen.calls);
  EXPECT_EQ("SmallInteger", seen.cls);
  EXPECT_EQ("3", seen.text);
  ao_set_inspect_hook(nullptr, nullptr);
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_INSPECTIT, out, 64, &err));
  EXPECT_STREQ("3", out);
  EXPECT_EQ(1, seen.calls);
  ao_runtime_shutdown();
}

TEST_F(SessionAbi, EvalTranscriptShowThenCr) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  std::vector<std::string> seen;
  ao_set_transcript_hook(
      [](const char* utf8, int len, int is_clear, void* user) {
        auto* chunks = static_cast<std::vector<std::string>*>(user);
        if (is_clear != 0) {
          chunks->clear();
          return;
        }
        if (utf8 == nullptr || len < 0) {
          return;
        }
        chunks->emplace_back(utf8, static_cast<std::size_t>(len));
      },
      &seen);
  char out[64];
  AoSpan err{};
  const char* show = "Transcript show: 'hello'";
  const char* cr = "Transcript cr";
  ASSERT_EQ(AO_OK, ao_eval(show, static_cast<int>(std::strlen(show)), AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval(cr, static_cast<int>(std::strlen(cr)), AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(2u, seen.size());
  EXPECT_EQ("hello", seen[0]);
  EXPECT_EQ("\n", seen[1]);
  ao_runtime_shutdown();
}

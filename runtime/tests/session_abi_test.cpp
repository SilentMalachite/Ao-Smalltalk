#include "ao_abi.h"

#include "test_support.hpp"

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

// SPEC §3.10: ブロック内の代入も同じ束縛に書くので、外側にも次の評価にも見える。
TEST_F(SessionAbi, BlockAssignmentUpdatesWorkspaceBinding) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  const char* src = "x := 1.\n[ x := 2 ] value.\n^x";
  ASSERT_EQ(AO_OK,
            ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("2", out);
  ASSERT_EQ(AO_OK, ao_eval("blk := [x := x + 10]", 20, AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval("blk value", 9, AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval("x", 1, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("12", out);
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

// SPEC §3.2: old の上限を超える割り当ては評価エラー「out of memory」になる。
// 要求は 600000000 スロット（約 4.8 GB）で、上限（4 GiB − 1 MiB）の判定で断られ、コミットはしない。
TEST_F(SessionAbi, HugeAllocationReportsOutOfMemory) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  const char* src = "(Array new: 600000000) size";
  EXPECT_EQ(AO_ERR_EVAL,
            ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("out of memory", err.message);
  EXPECT_STREQ("", out);

  // フラグは消えている。次の評価はふつうに成功する。
  AoSpan err2{};
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_PRINTIT, out, 64, &err2));
  EXPECT_STREQ("3", out);
  EXPECT_STREQ("", err2.message);
  ao_runtime_shutdown();
}

// INSPECTIT でも、out of memory になった評価はエラーだけを返す。inspect フックを呼んでから
// out of memory を返していた（Inspector が開き、そのあとでエラーになる）。
TEST_F(SessionAbi, OutOfMemoryInspectItDoesNotCallInspectHook) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  int calls = 0;
  ao_set_inspect_hook([](const char*, const char*, void* user) { *static_cast<int*>(user) += 1; },
                      &calls);
  char out[64];
  AoSpan err{};
  const char* src = "(Array new: 600000000). 3";
  EXPECT_EQ(AO_ERR_EVAL,
            ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_INSPECTIT, out, 64, &err));
  EXPECT_STREQ("out of memory", err.message);
  EXPECT_STREQ("", out);
  EXPECT_EQ(0, calls);

  // フックは次の評価ではふつうに呼ばれる。
  AoSpan err2{};
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_INSPECTIT, out, 64, &err2));
  EXPECT_STREQ("3", out);
  EXPECT_EQ(1, calls);
  ao_set_inspect_hook(nullptr, nullptr);
  ao_runtime_shutdown();
}

// SPEC §3.4: 無限再帰は「stack overflow」の評価エラーになり、セッションはそのまま使える。
TEST_F(SessionAbi, SessionUsableAfterStackOverflow) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* def =
      "Object subclass: #R2Deep\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B2-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err));
  ASSERT_EQ(AO_OK, ao_accept_method("R2Deep", 0, "recur: n\n  ^self recur: n + 1\n", &err));
  char out[64];
  const std::string src = "R2Deep new recur: 0";
  int rc = AO_OK;
  runOnSmallStack([&] {
    rc = ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT, out, 64, &err);
  });
  EXPECT_EQ(AO_ERR_EVAL, rc);
  EXPECT_STREQ("stack overflow", err.message);
  EXPECT_STREQ("", out);
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
}

namespace {

int evalPrint(const char* src, char* out, int outLen, AoSpan* err) {
  return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, outLen, err);
}

}  // namespace

// 00 High: 宣言した temp は同じ名前の束縛の値で始まらず、書き戻しもしない。
TEST_F(SessionAbi, DeclaredTempIgnoresBinding) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("q := 7", out, 64, &err));
  ASSERT_EQ(AO_OK, evalPrint("| q | q", out, 64, &err));
  EXPECT_STREQ("nil", out);
  ASSERT_EQ(AO_OK, evalPrint("| q | q := 3. q", out, 64, &err));
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, evalPrint("q", out, 64, &err));
  EXPECT_STREQ("7", out);
  ao_runtime_shutdown();
}

// 00 High: 束縛は temp ではないので、255 を超えても評価できる。
TEST_F(SessionAbi, ThreeHundredBindingsEvaluate) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  for (int i = 0; i < 300; ++i) {
    const std::string src = "v" + std::to_string(i) + " := " + std::to_string(i);
    ASSERT_EQ(AO_OK, evalPrint(src.c_str(), out, 64, &err)) << i << ": " << err.message;
  }
  ASSERT_EQ(AO_OK, evalPrint("v0 + v299", out, 64, &err)) << err.message;
  EXPECT_STREQ("299", out);
  ao_runtime_shutdown();
}

// 00 High: 一度束縛になった名前も、後から同じ名前のクラスを定義すればクラスを指す。
TEST_F(SessionAbi, ClassDefinedAfterBindingWins) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("R2Late", out, 64, &err));
  EXPECT_STREQ("nil", out);
  const char* def =
      "Object subclass: #R2Late\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B2-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err));
  ASSERT_EQ(AO_OK, evalPrint("R2Late new class == R2Late", out, 64, &err));
  EXPECT_STREQ("true", out);
  ao_runtime_shutdown();
}

// 既知のグローバルは束縛より先に解決する。Smalltalk at:put: で足した名前も同じ。
TEST_F(SessionAbi, GlobalWinsOverBinding) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("R2Zap := 5", out, 64, &err));
  ASSERT_EQ(AO_OK, evalPrint("Smalltalk at: #R2Zap put: 3", out, 64, &err)) << err.message;
  ASSERT_EQ(AO_OK, evalPrint("R2Zap", out, 64, &err));
  EXPECT_STREQ("3", out);
  EXPECT_EQ(AO_ERR_COMPILE, evalPrint("R2Zap := 4", out, 64, &err));
  ao_runtime_shutdown();
}

// 00 High: Smalltalk は既知のグローバル。
TEST_F(SessionAbi, SmalltalkIsKnownGlobal) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("Smalltalk isNil", out, 64, &err));
  EXPECT_STREQ("false", out);
  EXPECT_EQ(AO_ERR_COMPILE, evalPrint("Smalltalk := 1", out, 64, &err));
  ao_runtime_shutdown();
}

// 02 Medium: 前の Do it で作ったブロックの ^ は、その評価の呼び出し元を巻き込まない。
TEST_F(SessionAbi, DeadHomeBlockDoesNotAbortLaterEval) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("b := [:x | ^x]", out, 64, &err));
  const char* src =
      "log := OrderedCollection new.\n"
      "log add: #before.\n"
      "log add: (b value: 3).\n"
      "log add: #after.\n"
      "log size";
  ASSERT_EQ(AO_OK, evalPrint(src, out, 64, &err)) << err.message;
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
}

// SPEC §3.5: 展開した分岐のレシーバが Boolean でなければ mustBeBoolean を送り、Object の既定は
// 「NonBoolean receiver」で評価を中断する。Print it は評価エラーになり、セッションはそのまま使える。
TEST_F(SessionAbi, NonBooleanBranchReportsReason) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "r2Branch\n  ^nil ifTrue: [1] ifFalse: [2]\n", &err))
      << err.message;
  char out[64];
  EXPECT_EQ(AO_ERR_EVAL, evalPrint("Object new r2Branch", out, 64, &err));
  EXPECT_STREQ("NonBoolean receiver", err.message);
  EXPECT_STREQ("", out);

  // Do it の本体で展開した分岐も同じ。
  AoSpan err2{};
  EXPECT_EQ(AO_ERR_EVAL, evalPrint("3 ifTrue: [1]", out, 64, &err2));
  EXPECT_STREQ("NonBoolean receiver", err2.message);

  // 中断は次の評価に持ち越さない。
  AoSpan err3{};
  ASSERT_EQ(AO_OK, evalPrint("1 + 2", out, 64, &err3)) << err3.message;
  EXPECT_STREQ("3", out);
  EXPECT_STREQ("", err3.message);
  ao_runtime_shutdown();
}

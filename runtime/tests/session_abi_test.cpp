#include "ao_abi.h"

#include "test_support.hpp"

#include "../src/Fiber.hpp"
#include "../src/Session.hpp"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

class SessionAbi : public ::testing::Test {
 protected:
  // SPEC §3.10: the hooks outlive the session, so a test's hook must not reach the next test.
  void TearDown() override {
    ao_runtime_shutdown();
    ao_set_transcript_hook(nullptr, nullptr);
    ao_set_inspect_hook(nullptr, nullptr);
  }
};

TEST_F(SessionAbi, BootThenImageRoundTripKeepsOnePlusTwo) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const char* path = "session-abi-roundtrip.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path, nullptr));
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

// SPEC §3.10 / §3.12: a file-in error that DEFERRED.md does not list makes ao_filein_load_order
// answer AO_ERR; once it is listed, AO_OK. The vendor LOAD_ORDER files in with AO_OK.
TEST_F(SessionAbi, FileInLoadOrderFailsOnUndeferredError) {
  namespace fs = std::filesystem;
  const fs::path dir = fs::temp_directory_path() / "ao-session-abi-filein";
  std::error_code ec;
  fs::remove_all(dir, ec);
  fs::create_directories(dir);
  {
    std::ofstream order(dir / "LOAD_ORDER");
    order << "a.st\n";
    std::ofstream a(dir / "a.st");
    a << "!Object subclass: #AbiFileIn\n"
         "  instanceVariableNames: ''\n"
         "  classVariableNames: ''\n"
         "  poolDictionaries: ''\n"
         "  category: 'B3-Test'!\n"
         "!AbiFileIn methodsFor: 't'!\n"
         "bad\n"
         "  ^1 +! !\n";
    ASSERT_TRUE(order && a);
  }
  const std::string order = (dir / "LOAD_ORDER").string();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_filein_load_order(order.c_str()));
  ao_runtime_shutdown();
  {
    std::ofstream deferred(dir / "DEFERRED.md");
    deferred << "AbiFileIn>>bad: kept out on purpose\n";
    ASSERT_TRUE(deferred);
  }
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_OK, ao_filein_load_order(order.c_str()));
  ao_runtime_shutdown();
  fs::remove_all(dir, ec);
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::string vendor = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  EXPECT_EQ(AO_OK, ao_filein_load_order(vendor.c_str()));
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

// 02 Medium: 前の Do it で作ったブロックの ^ は、ホームを探して評価を理由なしに打ち切らない。
// SPEC §3.4: cannotReturn: の既定で「cannot return」と理由を返して中断し、次の評価は使える。
TEST_F(SessionAbi, DeadHomeBlockAbortsWithReason) {
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
  EXPECT_EQ(AO_ERR_EVAL, evalPrint(src, out, 64, &err));
  EXPECT_STREQ("cannot return", err.message);
  AoSpan err2{};
  ASSERT_EQ(AO_OK, evalPrint("log size", out, 64, &err2)) << err2.message;
  EXPECT_STREQ("1", out);
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

// 00 Critical: Array の = は先頭で == を見る（自分を含む配列も即 true）。クラスは class == で比べ、
// Array のサブクラスのインスタンスどうしも要素で比べる。
TEST_F(SessionAbi, ArrayEqualsChecksIdentityFirstAndSameClass) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* def =
      "Array subclass: #B3Arr\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err)) << err.message;
  struct Case {
    const char* src;
    const char* want;
  };
  const Case cases[] = {
      {"#(1 2) = #(1 2)", "true"},
      {"#(1 2) = #(1 3)", "false"},
      {"#(1 2) = #(1 2 3)", "false"},
      {"#(1 2) = 3", "false"},
      {"a := #(1 2). a = a", "true"},
      {"(B3Arr new: 2) = (B3Arr new: 2)", "true"},
      {"(B3Arr new: 2) = (Array new: 2)", "false"},
      {"(Array new: 2) = (B3Arr new: 2)", "false"},
      {"m := B3Arr new: 1. m at: 1 put: m. m = m", "true"},
      {"s := Array new: 1. s at: 1 put: s. s = s", "true"},
      {"s = s copy", "true"},
  };
  char out[64];
  for (const Case& c : cases) {
    SCOPED_TRACE(c.src);
    AoSpan e{};
    ASSERT_EQ(AO_OK, evalPrint(c.src, out, 64, &e)) << e.message;
    EXPECT_STREQ(c.want, out);
  }
  ao_runtime_shutdown();
}

// 00 Critical / SPEC §3.4: 相互に参照し合う 2 つの配列の = は、スタックガードで「stack overflow」の
// abort になり、セッションはそのまま使える。
TEST_F(SessionAbi, MutuallyReferencingArraysAbortWithStackOverflow) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("p := Array new: 1. q := Array new: 1. p at: 1 put: q. "
                             "q at: 1 put: p. p size",
                             out, 64, &err))
      << err.message;
  int rc = AO_OK;
  runOnSmallStack([&] { rc = evalPrint("p = q", out, 64, &err); });
  EXPECT_EQ(AO_ERR_EVAL, rc);
  EXPECT_STREQ("stack overflow", err.message);
  EXPECT_STREQ("", out);
  AoSpan err2{};
  ASSERT_EQ(AO_OK, evalPrint("(p at: 1) == q", out, 64, &err2)) << err2.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, evalPrint("1 + 2", out, 64, &err2)) << err2.message;
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
}

// 00 High / SPEC §3.10: out が NULL か out_len が 1 未満なら、コンパイルも評価もせずに AO_ERR。
// 代入も Transcript への出力も起きないので、呼び出し側が再試行しても二重にならない。
TEST_F(SessionAbi, EvalWithoutOutBufferRefusesBeforeEvaluating) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  std::vector<std::string> seen;
  ao_set_transcript_hook(
      [](const char* utf8, int len, int, void* user) {
        if (utf8 != nullptr && len >= 0) {
          static_cast<std::vector<std::string>*>(user)->emplace_back(
              utf8, static_cast<std::size_t>(len));
        }
      },
      &seen);
  AoSpan err{};
  char out[64];
  const char* assign = "b3y := 3";
  const int assignLen = static_cast<int>(std::strlen(assign));
  EXPECT_EQ(AO_ERR, ao_eval(assign, assignLen, AO_EVAL_DOIT, nullptr, 0, &err));
  EXPECT_EQ(AO_ERR, ao_eval(assign, assignLen, AO_EVAL_DOIT, out, 0, &err));
  EXPECT_EQ(AO_ERR, ao_eval(assign, assignLen, AO_EVAL_DOIT, out, -1, &err));
  EXPECT_EQ(AO_ERR, ao_eval(assign, assignLen, AO_EVAL_PRINTIT, nullptr, 64, &err));
  const char* show = "Transcript show: 'b3'";
  const int showLen = static_cast<int>(std::strlen(show));
  EXPECT_EQ(AO_ERR, ao_eval(show, showLen, AO_EVAL_DOIT, nullptr, 0, &err));
  EXPECT_EQ(AO_ERR, ao_eval(show, showLen, AO_EVAL_DOIT, out, 0, &err));
  EXPECT_TRUE(seen.empty());
  // コンパイルもしないので、壊れたソースでもコンパイルエラーにならない。
  EXPECT_EQ(AO_ERR, ao_eval("1 +", 3, AO_EVAL_DOIT, nullptr, 0, &err));
  ASSERT_EQ(AO_OK, evalPrint("b3y", out, 64, &err)) << err.message;
  EXPECT_STREQ("nil", out);
  ao_set_transcript_hook(nullptr, nullptr);
  ao_runtime_shutdown();
}

// B6 review (06 Low) / SPEC §3.10: C++ の例外は ABI の境界を越えない。境界を越えると std::terminate で
// プロセスが落ちる。例外を投げるフック（ホストの不具合）で評価が途中で止まっても、ao_eval は AO_ERR を
// 返す。止まったセッションは捨て、新しいセッションはふつうに動く。
TEST_F(SessionAbi, ExceptionInsideEvalDoesNotCrossTheAbi) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ao_set_transcript_hook([](const char*, int, int, void*) { throw std::runtime_error("host bug"); },
                         nullptr);
  char out[64];
  AoSpan err{};
  const char* show = "Transcript show: 'x'. 3";
  EXPECT_EQ(AO_ERR, ao_eval(show, static_cast<int>(std::strlen(show)), AO_EVAL_PRINTIT, out, 64,
                            &err));
  EXPECT_STREQ("", out);
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("3", out);
}

namespace {

// A hook that calls back into the ABI while the runtime runs, once, and keeps each call's answer.
struct Reentry {
  std::string image;
  std::string loadOrder;
  std::vector<int> codes;
  std::string loadReason;
  std::string evalOut = "unchanged";
  bool entered = false;
};

void reenterEverything(Reentry& r) {
  if (r.entered) {
    return;
  }
  r.entered = true;
  AoSpan err{};
  char out[16] = "unchanged";
  r.codes.push_back(ao_image_save(r.image.c_str()));
  r.codes.push_back(ao_image_load(r.image.c_str(), &err));
  r.loadReason = err.message;
  r.codes.push_back(ao_runtime_shutdown());
  r.codes.push_back(ao_runtime_boot());
  r.codes.push_back(ao_eval("1", 1, AO_EVAL_PRINTIT, out, 16, &err));
  r.evalOut = out;
  r.codes.push_back(ao_accept_method("Object", 0, "b6reentry\n  ^1\n", &err));
  r.codes.push_back(ao_accept_class("Object subclass: #B6Reentry\n  instanceVariableNames: ''\n"
                                    "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                    "  category: 'B6-Test'\n",
                                    &err));
  r.codes.push_back(ao_workspace_reset());
  r.codes.push_back(ao_filein_load_order(r.loadOrder.c_str()));
}

// A saved image and a LOAD_ORDER that files in cleanly, both of which work outside a hook.
Reentry reentryFixture(const std::filesystem::path& dir) {
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
  std::filesystem::create_directories(dir);
  Reentry r;
  r.image = (dir / "reentry.aoimage").string();
  r.loadOrder = (dir / "LOAD_ORDER").string();
  std::ofstream(dir / "LOAD_ORDER") << "a.st\n";
  std::ofstream(dir / "a.st") << "!Object subclass: #B6FileIn\n  instanceVariableNames: ''\n"
                                 "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                 "  category: 'B6-Test'!\n";
  EXPECT_EQ(AO_OK, ao_image_save(r.image.c_str()));
  EXPECT_EQ(AO_OK, ao_filein_load_order(r.loadOrder.c_str()));
  return r;
}

void expectAllRefused(const Reentry& r) {
  ASSERT_TRUE(r.entered);
  EXPECT_EQ(std::vector<int>(9, AO_ERR), r.codes);
  EXPECT_EQ("runtime is busy", r.loadReason);
  EXPECT_EQ("", r.evalOut);
}

// After the refused calls the session is the one that ran: its workspace binding is there, the
// refused accepts added nothing, and outside a hook the same calls work again.
void expectSessionUntouched() {
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("b6x", 3, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("40", out);
  EXPECT_EQ(AO_ERR_EVAL, ao_eval("nil b6reentry", 13, AO_EVAL_PRINTIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval("Smalltalk includesKey: #B6Reentry", 33, AO_EVAL_PRINTIT, out, 64, &err))
      << err.message;
  EXPECT_STREQ("false", out);
  EXPECT_EQ(AO_OK, ao_accept_method("Object", 0, "b6reentry\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_eval("nil b6reentry", 13, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("1", out);
}

}  // namespace

// B6 review (06 Low) / SPEC §3.10: 失敗シナリオ。評価中の transcript フックから ao_image_load を呼ぶと、
// フックの中のロードは 0 を返してセッションを差し替え、実行中のインタプリタが古いヒープを読んで SIGSEGV に
// なった。ランタイムが動いている間の boot / shutdown / save / load / filein / workspace reset / eval /
// accept は AO_ERR で、評価はそのまま続く。
TEST_F(SessionAbi, ReentrantCallsFromTranscriptHookAreRefused) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  Reentry r = reentryFixture(std::filesystem::temp_directory_path() / "ao-b6-reentry-transcript");
  ao_set_transcript_hook(
      [](const char*, int, int, void* user) { reenterEverything(*static_cast<Reentry*>(user)); },
      &r);
  char out[64];
  AoSpan err{};
  const char* src = "b6x := 40. Transcript show: 'a'. Transcript show: 'b'. b6x + 2";
  ASSERT_EQ(AO_OK, ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err))
      << err.message;
  EXPECT_STREQ("42", out);
  ao_set_transcript_hook(nullptr, nullptr);
  expectAllRefused(r);
  expectSessionUntouched();
}

// SPEC §3.10: the inspect hook runs after the doIt has returned, outside the interpreter, but the
// ABI entry (ao_eval) still runs: the same calls are refused there too.
TEST_F(SessionAbi, ReentrantCallsFromInspectHookAreRefused) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  Reentry r = reentryFixture(std::filesystem::temp_directory_path() / "ao-b6-reentry-inspect");
  ao_set_inspect_hook(
      [](const char*, const char*, void* user) { reenterEverything(*static_cast<Reentry*>(user)); },
      &r);
  char out[64];
  AoSpan err{};
  const char* src = "b6x := 40. b6x + 2";
  ASSERT_EQ(AO_OK,
            ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_INSPECTIT, out, 64, &err))
      << err.message;
  EXPECT_STREQ("42", out);
  ao_set_inspect_hook(nullptr, nullptr);
  expectAllRefused(r);
  expectSessionUntouched();
}

namespace {

void collectTranscript(const char* utf8, int len, int is_clear, void* user) {
  auto* chunks = static_cast<std::vector<std::string>*>(user);
  if (is_clear == 0 && utf8 != nullptr && len >= 0) {
    chunks->emplace_back(utf8, static_cast<std::size_t>(len));
  }
}

int evalDoIt(const char* src) {
  char out[64];
  AoSpan err{};
  return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DOIT, out, 64, &err);
}

}  // namespace

// B6 review (06 Low) / SPEC §3.10: 失敗シナリオ。ao_set_transcript_hook を boot の前に呼ぶと、そのあとの
// Transcript show: は何も届けなかった。shutdown→boot のあとも同じだった（新しいセッションの ctx にフックを
// 戻していなかった）。フックは ABI 側が持ち、boot とロードが作るセッションに配線する。NULL で外れる。
TEST_F(SessionAbi, TranscriptHookReachesEverySession) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectTranscript, &seen);
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, evalDoIt("Transcript show: 'one'"));
  EXPECT_EQ(std::vector<std::string>{"one"}, seen);

  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, evalDoIt("Transcript show: 'two'"));
  EXPECT_EQ((std::vector<std::string>{"one", "two"}), seen);

  const auto path = std::filesystem::temp_directory_path() / "ao-b6-hook.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), nullptr));
  ASSERT_EQ(AO_OK, evalDoIt("Transcript show: 'three'"));
  EXPECT_EQ((std::vector<std::string>{"one", "two", "three"}), seen);

  ao_set_transcript_hook(nullptr, nullptr);
  ASSERT_EQ(AO_OK, evalDoIt("Transcript show: 'four'"));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, evalDoIt("Transcript show: 'five'"));
  EXPECT_EQ((std::vector<std::string>{"one", "two", "three"}), seen);
  std::filesystem::remove(path);
}

// B6 review (Claude Low) / SPEC §3.10: the busy test and the taking of the entry are one atomic
// step, so a call from another thread while an evaluation runs is refused like a hook's, and the
// evaluation goes on.
TEST_F(SessionAbi, CallFromAnotherThreadWhileEvaluatingIsRefused) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  struct Gate {
    std::promise<void> inHook;
    std::promise<void> release;
    bool first = true;
  } gate;
  std::future<void> released = gate.release.get_future();
  struct Hook {
    Gate* gate;
    std::future<void>* released;
  } hook{&gate, &released};
  ao_set_transcript_hook(
      [](const char*, int, int, void* user) {
        auto* h = static_cast<Hook*>(user);
        if (!h->gate->first) return;
        h->gate->first = false;
        h->gate->inHook.set_value();
        h->released->wait();
      },
      &hook);
  int rc = -1;
  char out[64];
  std::thread evaluator([&] {
    AoSpan err{};
    const char* src = "Transcript show: 'x'. 6 * 7";
    rc = ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  });
  gate.inHook.get_future().wait();
  char other[16];
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_eval("1", 1, AO_EVAL_PRINTIT, other, 16, &err));
  EXPECT_EQ(AO_ERR, ao_runtime_shutdown());
  gate.release.set_value();
  evaluator.join();
  EXPECT_EQ(AO_OK, rc);
  EXPECT_STREQ("42", out);
  ASSERT_EQ(AO_OK, ao_eval("1", 1, AO_EVAL_PRINTIT, other, 16, &err)) << err.message;
  EXPECT_STREQ("1", other);
}

// B6 review follow-up / SPEC §3.6: 失敗シナリオ。Association key:value:、Point x:y:、
// Rectangle origin:corner:、Interval from:to:by: はレシーバのクラスによらず 2 か 3 スロットで割り当てた。
// 変数を足したサブクラスのインスタンスは足した変数を読み書きできず、B6 の format の検査で保存もできな
// かった。生成ネイティブはクラスの instSize だけ割り当て、足した変数は nil から始まる。
TEST_F(SessionAbi, ClassSideConstructorsAllocateTheSubclassInstSize) {
  struct Case {
    const char* kernel;
    const char* make;
    int kernelSlots;
    const char* firstSlot;
  };
  const Case cases[] = {
      {"Association", "B6X key: 1 value: 2", 2, "1"},
      {"Point", "B6X x: 1 y: 2", 2, "1"},
      {"Rectangle", "B6X origin: 1 corner: 2", 2, "1"},
      {"Interval", "B6X from: 1 to: 3 by: 1", 3, "1"},
  };
  const auto path = std::filesystem::temp_directory_path() / "ao-b6-constructors.aoimage";
  for (const Case& c : cases) {
    SCOPED_TRACE(c.kernel);
    ASSERT_EQ(AO_OK, ao_runtime_boot());
    AoSpan err{};
    const std::string def = std::string(c.kernel) +
                            " subclass: #B6X\n  instanceVariableNames: 'extra'\n"
                            "  classVariableNames: ''\n  poolDictionaries: ''\n"
                            "  category: 'B6-Test'\n";
    ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method("B6X", 0, "extra\n  ^extra\n", &err)) << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method("B6X", 0, "extra: x\n  extra := x\n", &err))
        << err.message;
    char out[128];
    auto eval = [&](const std::string& src) {
      return ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT, out, 128, &err);
    };
    ASSERT_EQ(AO_OK, eval(std::string("Smalltalk at: #B6K put: (") + c.make + ")")) << err.message;
    const std::string extraIndex = std::to_string(c.kernelSlots + 1);
    ASSERT_EQ(AO_OK, eval("B6K instVarAt: " + extraIndex)) << err.message;
    EXPECT_STREQ("nil", out);
    EXPECT_EQ(AO_ERR_EVAL, eval("B6K instVarAt: " + std::to_string(c.kernelSlots + 2)));
    ASSERT_EQ(AO_OK, eval("B6K instVarAt: 1")) << err.message;
    EXPECT_STREQ(c.firstSlot, out);
    ASSERT_EQ(AO_OK, eval("B6K extra: 9. B6K extra")) << err.message;
    EXPECT_STREQ("9", out);

    ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
    ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
    ASSERT_EQ(AO_OK, eval("B6K extra")) << err.message;
    EXPECT_STREQ("9", out);
    ASSERT_EQ(AO_OK, eval("B6K instVarAt: 1")) << err.message;
    EXPECT_STREQ(c.firstSlot, out);
    ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  }
  std::filesystem::remove(path);
}

// ---- B10: the session's cooperative scheduler (SPEC §3.4, §3.10) ----

// SPEC §3.4 評価の終わり: a Do it drains the ready queue before it returns, so what a fork writes
// to the Transcript reaches the hook before ao_eval returns.
TEST_F(SessionAbi, DoItDrainsTranscriptFork) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectTranscript, &seen);
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, evalDoIt("[Transcript show: 'x'] fork"));
  EXPECT_EQ(std::vector<std::string>{"x"}, seen);
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
}

// SPEC §3.4 評価の終わり: the answer (the printString) is fixed before the drain; what the forks do
// shows in the next evaluation.
TEST_F(SessionAbi, PrintItBeforeDrain) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("n := 0. [n := 5] fork. n", out, 64, &err)) << err.message;
  EXPECT_STREQ("0", out);
  ASSERT_EQ(AO_OK, evalPrint("n", out, 64, &err)) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, evalPrint("a := OrderedCollection new. [a add: 1] fork. a size", out, 64, &err))
      << err.message;
  EXPECT_STREQ("0", out);
  ASSERT_EQ(AO_OK, evalPrint("a size", out, 64, &err)) << err.message;
  EXPECT_STREQ("1", out);
}

// The review's expression: the fork runs at the yield, and the Print it sees its increment.
TEST_F(SessionAbi, ForkIncrementVisibleAfterYield) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("| n | n := 0. [n := n + 1] fork. Processor yield. n", out, 64, &err))
      << err.message;
  EXPECT_STREQ("1", out);
}

// SPEC §3.4: a process waiting at the end of the drain is kept for the next evaluations; a signal
// there makes it run in that evaluation's drain. The base stays the same process throughout.
TEST_F(SessionAbi, WaiterSurvivesAcrossEvals) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalDoIt("base := Processor activeProcess. s := Semaphore new. "
                            "log := OrderedCollection new. [s wait. log add: #woke] fork"));
  EXPECT_EQ(1u, ao::session()->scheduler->liveFibers());
  ASSERT_EQ(AO_OK, evalPrint("log size", out, 64, &err)) << err.message;
  EXPECT_STREQ("0", out);
  ASSERT_EQ(AO_OK, evalDoIt("s signal"));
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
  ASSERT_EQ(AO_OK, evalPrint("log size", out, 64, &err)) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, evalPrint("Processor activeProcess == base", out, 64, &err)) << err.message;
  EXPECT_STREQ("true", out);
}

// SPEC §3.11 プロセス: an image saved with a waiting and a ready process loads with the base as
// Processor's activeProcess and an empty ready queue; the saved processes cannot run (resume fails,
// signal drops the waiter). The old session's fibers are abandoned with it.
TEST_F(SessionAbi, SaveLoadWithWaitersKeepsBaseActive) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalDoIt("Smalltalk at: #B10Base put: Processor activeProcess. "
                            "Smalltalk at: #B10Sem put: Semaphore new. "
                            "Smalltalk at: #B10Waiter put: [(Smalltalk at: #B10Sem) wait] fork. "
                            "Smalltalk at: #B10Looper put: [1 to: 1001 do: [:i | Processor yield]] fork"));
  // The waiter waits; the looper is still ready after the drain's 1000 rounds.
  EXPECT_EQ(2u, ao::session()->scheduler->liveFibers());
  const auto path = std::filesystem::temp_directory_path() / "ao-b10-waiters.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  EXPECT_EQ(0u, ao::session()->scheduler->liveFibers());
  ASSERT_EQ(AO_OK, evalPrint("Processor activeProcess == B10Base", out, 64, &err)) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, evalPrint("(Processor instVarAt: 1) size", out, 64, &err)) << err.message;
  EXPECT_STREQ("0", out);
  EXPECT_EQ(AO_ERR_EVAL, evalPrint("B10Waiter resume", out, 64, &err));
  EXPECT_STREQ("process cannot run", err.message);
  EXPECT_EQ(AO_ERR_EVAL, evalPrint("B10Looper resume", out, 64, &err));
  EXPECT_STREQ("process cannot run", err.message);
  ASSERT_EQ(AO_OK, evalPrint("B10Sem signal. (B10Sem instVarAt: 2) size", out, 64, &err))
      << err.message;
  EXPECT_STREQ("0", out);
  ASSERT_EQ(AO_OK, evalPrint("B10Sem instVarAt: 1", out, 64, &err)) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, evalPrint("Processor activeProcess == B10Base", out, 64, &err)) << err.message;
  EXPECT_STREQ("true", out);
  std::filesystem::remove(path);
}

// SPEC §3.4 abandon, §3.10: shutdown abandons the processes left, before the heap goes: no
// cleanup runs, no hook is called, each fiber's frames unwind so its roots go, and its stack goes
// back. Abandoning (what shutdown does first) brings the root counts back to what they were.
TEST_F(SessionAbi, ShutdownReclaimsFibers) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectTranscript, &seen);
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const char* waiters =
      "s := Semaphore new. "
      "1 to: 5 do: [:i | [[s wait] ensure: [Transcript show: 'cleanup']] fork]";
  // The first run interns the Symbols the source names; each stays rooted in the intern table
  // (WellKnown::internWith), so count from after it.
  ASSERT_EQ(AO_OK, evalDoIt(waiters));
  ao::Session* session = ao::session();
  ASSERT_EQ(5u, session->scheduler->liveFibers());
  session->scheduler->terminateAll(true);
  ASSERT_EQ(0u, session->scheduler->liveFibers());
  const ao::Roots::Counts before = session->roots.counts();
  ASSERT_EQ(AO_OK, evalDoIt(waiters));
  ASSERT_EQ(5u, session->scheduler->liveFibers());
  EXPECT_EQ(before.attachedStacks + 5, session->roots.counts().attachedStacks);
  session->scheduler->terminateAll(true);
  const ao::Roots::Counts after = session->roots.counts();
  EXPECT_EQ(before.slots, after.slots);
  EXPECT_EQ(before.ranges, after.ranges);
  EXPECT_EQ(before.frameSlots, after.frameSlots);
  EXPECT_EQ(before.handles, after.handles);
  EXPECT_EQ(before.attachedStacks, after.attachedStacks);
  EXPECT_EQ(0u, session->scheduler->processFailures());

  // Five forks take the pooled stacks and map the rest; shutdown gives all five back.
  ASSERT_EQ(AO_OK, evalDoIt(waiters));
  EXPECT_EQ(0u, ao::FiberStack::pooledCount());
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(ao::FiberStack::kPoolLimit, ao::FiberStack::pooledCount());
  EXPECT_TRUE(seen.empty());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("3 + 4", out, 64, &err)) << err.message;
  EXPECT_STREQ("7", out);
}

// SPEC §3.10 再入: a hook called from a forked process (here in the drain) is refused like one
// called from the base: every entry answers AO_ERR, and the evaluation's answer stands.
TEST_F(SessionAbi, ReentrantEvalFromHookRejected) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  Reentry r = reentryFixture(std::filesystem::temp_directory_path() / "ao-b10-reentry-fork");
  ao_set_transcript_hook(
      [](const char*, int, int, void* user) { reenterEverything(*static_cast<Reentry*>(user)); },
      &r);
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("b6x := 40. [Transcript show: 'a'] fork. b6x + 2", out, 64, &err))
      << err.message;
  EXPECT_STREQ("42", out);
  ao_set_transcript_hook(nullptr, nullptr);
  expectAllRefused(r);
  expectSessionUntouched();
}

// SPEC §3.4 評価の終わり: a process that fails in the drain is counted, but the evaluation's answer
// is the one it had: AO_OK and its printString, or its own AO_ERR_EVAL reason.
TEST_F(SessionAbi, ProcessFailureInDrainKeepsEvalAnswer) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalPrint("[nil foo] fork. 3 + 4", out, 64, &err)) << err.message;
  EXPECT_STREQ("7", out);
  EXPECT_STREQ("", err.message);
  EXPECT_EQ(1u, ao::session()->scheduler->processFailures());
  EXPECT_EQ("doesNotUnderstand: #foo", ao::session()->scheduler->lastFailureReason());
  EXPECT_EQ(AO_ERR_EVAL, evalPrint("[nil foo] fork. nil bar", out, 64, &err));
  EXPECT_STREQ("doesNotUnderstand: #bar", err.message);
  EXPECT_EQ(2u, ao::session()->scheduler->processFailures());
  ASSERT_EQ(AO_OK, evalPrint("3 + 4", out, 64, &err)) << err.message;
  EXPECT_STREQ("7", out);
}

// SPEC §3.4 デッドロック: the base's wait with nothing else to run fails the evaluation, and the
// next evaluation runs on the same base.
TEST_F(SessionAbi, BaseDeadlockFailsEvalBaseStays) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, evalDoIt("base := Processor activeProcess"));
  EXPECT_EQ(AO_ERR_EVAL, evalPrint("Semaphore new wait", out, 64, &err));
  EXPECT_STREQ("deadlock: no runnable process", err.message);
  ASSERT_EQ(AO_OK, evalPrint("Processor activeProcess == base", out, 64, &err)) << err.message;
  EXPECT_STREQ("true", out);
}

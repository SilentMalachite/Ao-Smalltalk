#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Interpreter.hpp"
#include "ao/TestRunner.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Send.hpp"
#include "ao/kernel/Install.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <system_error>
#include <vector>

TEST(BlockEval, ArgumentAndOuterTemp) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo: a\n  ^[:b | b + a] value: 3");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  ao::Oop arg = ao::Oop::fromSmallInteger(4);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), &arg, 1, ao::Oop::nil());
  EXPECT_EQ(7, got.smallIntegerValue());
}

// SPEC §3.4: ブロックは外側の temp を共有する（05 Critical）。
TEST(BlockEval, BlockAssignmentWritesOuterTemp) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo\n  | x |\n  x := 1.\n  [x := 2] value.\n  ^x");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(2, got.smallIntegerValue());
}

TEST(BlockEval, NonLocalReturnSkipsRest) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo\n  [ ^4 ] value.\n  ^5");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(4, got.smallIntegerValue());
}

TEST(BlockEval, ToDoNonLocalReturnStopsAtFirst) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo\n  1 to: 3 do: [:i | ^i].\n  ^0");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(1, got.smallIntegerValue());
}

TEST(BlockEval, ArrayDoNonLocalReturnStopsAtFirst) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo\n  #(9 8 7) do: [:e | ^e].\n  ^0");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(9, got.smallIntegerValue());
}

TEST(BlockEval, IfTrueIfFalseFromCompiledMethod) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo\n  ^true ifTrue: [4] ifFalse: [5]");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(4, got.smallIntegerValue());
}

TEST(AoTestRunner, ExampleFilePasses) {
  Boot b;
  const int code = ao::runSmalltalkTests(b.ctx, AO_SOURCE_DIR "/image/tests");
  EXPECT_EQ(0, code);
  EXPECT_EQ(0, b.ctx.testFailures);
}

TEST(AoTestRunner, SecondFileIsNotFirstDoIt) {
  namespace fs = std::filesystem;
  const fs::path dir = fs::temp_directory_path() / "ao-test-runner-second-doit";
  std::error_code ec;
  fs::remove_all(dir, ec);
  fs::create_directories(dir);
  struct Cleanup {
    fs::path path;
    ~Cleanup() {
      std::error_code ignore;
      fs::remove_all(path, ignore);
    }
  } cleanup{dir};
  {
    std::ofstream pass(dir / "a_pass.st");
    pass << "self assert: 1 equals: 1.\n";
    std::ofstream fail(dir / "b_fail.st");
    fail << "self assert: 1 equals: 2.\n";
    ASSERT_TRUE(pass && fail);
  }
  Boot b;
  const int code = ao::runSmalltalkTests(b.ctx, dir.string());
  EXPECT_EQ(1, code);
  EXPECT_GT(b.ctx.testFailures, 0);
}

namespace {

// dir を作り直し、files（名前と本文）を書く。テストの終わりに消す。
struct TestDir {
  std::filesystem::path path;
  explicit TestDir(const char* name) : path(std::filesystem::temp_directory_path() / name) {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    std::filesystem::create_directories(path);
  }
  ~TestDir() {
    std::error_code ignore;
    std::filesystem::remove_all(path, ignore);
  }
  bool write(const char* file, const char* body) const {
    std::ofstream out(path / file);
    out << body;
    return static_cast<bool>(out);
  }
};

}  // namespace

// SPEC §3.2: out of memory は評価エラー。途中の文で out of memory になり、捨てられた結果の後の
// assert が通っても、そのファイルは失敗として数える。old の上限を超える要求は GC せずに即座に
// 失敗するので、実際には何もコミットしない。
TEST(AoTestRunner, OutOfMemoryMidFileFails) {
  const TestDir dir("ao-test-runner-oom");
  ASSERT_TRUE(dir.write("oom.st", "Array new: 600000000.\nself assert: 1 equals: 1.\n"));
  Boot b;
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  EXPECT_EQ(1, code);
  EXPECT_EQ(1, b.ctx.testFailures);
  EXPECT_FALSE(b.heap.outOfMemory());
}

// 評価の前から立っていたフラグ（前の評価のもの）は、そのファイルのせいにしない。
TEST(AoTestRunner, EarlierOutOfMemoryIsNotBlamedOnFile) {
  const TestDir dir("ao-test-runner-stale-oom");
  ASSERT_TRUE(dir.write("pass.st", "self assert: 1 equals: 1.\n"));
  Boot b;
  b.heap.setOutOfMemory();
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  EXPECT_EQ(0, code);
  EXPECT_EQ(0, b.ctx.testFailures);
}

// SPEC §4.4: 実行中の abort（doesNotUnderstand:、0 除算）もコンパイルエラーもファイルの失敗で、
// 失敗したファイルの後も残りを実行する。失敗ごとに 1 行を stderr に出し、コンパイルエラーの位置は
// ファイル本文のバイト位置である。
TEST(AoTestRunner, FailuresAreCountedAndLaterFilesStillRun) {
  const TestDir dir("ao-test-runner-failures");
  ASSERT_TRUE(dir.write("a_dnu.st", "nil foo.\nself assert: 1 equals: 1.\n"));
  ASSERT_TRUE(dir.write("b_zero.st", "1/0.\nself assert: 1 equals: 1.\n"));
  ASSERT_TRUE(dir.write("c_compile.st", "self assert: 1 + equals: 4.\n"));
  ASSERT_TRUE(dir.write("d_pass.st", "self assert: 1 + 2 equals: 3.\n"));
  ASSERT_TRUE(dir.write("e_mismatch.st", "self assert: 1 + 2 equals: 4.\n"));
  Boot b;
  testing::internal::CaptureStderr();
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  const std::string reported = testing::internal::GetCapturedStderr();
  EXPECT_EQ(1, code);
  EXPECT_EQ(4, b.ctx.testFailures);
  const auto line = [&dir](const char* file, const char* rest) {
    return "ao --test: " + (dir.path / file).string() + rest + "\n";
  };
  const std::string expected = line("a_dnu.st", ": doesNotUnderstand: #foo") +
                               line("b_zero.st", ": division by zero") +
                               line("c_compile.st", ":17-24: expected expression") +
                               line("e_mismatch.st", ": 3 ~= 4");
  EXPECT_EQ(expected, reported);
  EXPECT_FALSE(b.ctx.aborting);
}

// SPEC §4.4: ファイルの評価、drain、terminate の間にプロセスの失敗があれば、そのファイルの失敗で、
// 件数によらず 1 行（最後の理由）。ファイルの abort とは別の行になる。drain のあとに残ったプロセスは
// terminate で終わり（後始末が走る。失敗に数えない）、後始末がブロックしたものは abandon して、
// 次のファイルに持ち越さない。
TEST(AoTestRunner, ProcessFailureFailsFileAndLeftoversEnd) {
  const TestDir dir("ao-test-runner-process");
  ASSERT_TRUE(dir.write("a_fork.st", "[nil foo] fork.\n[nil bar] fork.\nself assert: 1 equals: 1.\n"));
  ASSERT_TRUE(dir.write("b_both.st", "[nil baz] fork.\nnil qux.\n"));
  ASSERT_TRUE(dir.write("c_waiters.st",
                        "| s |\n"
                        "s := Semaphore new.\n"
                        "Smalltalk at: #B10Log put: 0.\n"
                        "[[s wait] ensure: [Smalltalk at: #B10Log put: 1]] fork.\n"
                        "[[s wait] ensure: [s wait]] fork.\n"
                        "self assert: 1 equals: 1.\n"));
  ASSERT_TRUE(dir.write("d_after.st",
                        "self assert: (Smalltalk at: #B10Log) equals: 1.\n"
                        "self assert: (Processor instVarAt: 1) size equals: 0.\n"));
  Boot b;
  testing::internal::CaptureStderr();
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  const std::string reported = testing::internal::GetCapturedStderr();
  EXPECT_EQ(1, code);
  const auto line = [&dir](const char* file, const char* rest) {
    return "ao --test: " + (dir.path / file).string() + rest + "\n";
  };
  const std::string expected = line("a_fork.st", ": process failed: doesNotUnderstand: #bar") +
                               line("b_both.st", ": doesNotUnderstand: #qux") +
                               line("b_both.st", ": process failed: doesNotUnderstand: #baz");
  EXPECT_EQ(expected, reported);
  EXPECT_EQ(3, b.ctx.testFailures);
  EXPECT_EQ(0u, b.scheduler.liveFibers());
  EXPECT_FALSE(b.ctx.aborting);
}

// SPEC §4.4: .st が 0 件か、ディレクトリが読めなければ exit 1。理由を stderr に出す。
TEST(AoTestRunner, EmptyOrMissingDirectoryFails) {
  const TestDir dir("ao-test-runner-empty");
  ASSERT_TRUE(dir.write("notes.txt", "not a test\n"));
  Boot b;
  testing::internal::CaptureStderr();
  EXPECT_EQ(1, ao::runSmalltalkTests(b.ctx, dir.path.string()));
  EXPECT_FALSE(testing::internal::GetCapturedStderr().empty());
  testing::internal::CaptureStderr();
  EXPECT_EQ(1, ao::runSmalltalkTests(b.ctx, (dir.path / "no-such-dir").string()));
  EXPECT_FALSE(testing::internal::GetCapturedStderr().empty());
}

namespace {

// ブロックの ^ が各コレクションの反復ネイティブを止めることを見るクラス。どのメソッドも、
// ブロックが最初に受けた要素を log に積んで ^ で返し、呼び出し側がその値を log に足して返す。
// 反復が止まれば log は 2 要素で、1 番目と 2 番目が同じオブジェクトになる。
constexpr const char* kNlrProbe =
    "!Object subclass: #R2Nlr\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Nlr methodsFor: 'probes'!\n"
    "firstOf: c log: log\n"
    "  c do: [:e | log add: e. ^e].\n"
    "  ^nil!\n"
    "collectFirst: c log: log\n"
    "  c collect: [:e | log add: e. ^e].\n"
    "  ^nil!\n"
    "detectFirst: c log: log\n"
    "  c detect: [:e | log add: e. ^e] ifNone: [nil].\n"
    "  ^nil!\n"
    "injectFirst: c log: log\n"
    "  c inject: 0 into: [:a :e | log add: e. ^e].\n"
    "  ^nil!\n"
    "run: sel on: c\n"
    "  | log |\n"
    "  log := OrderedCollection new.\n"
    "  log add: (self perform: sel withArguments: ((Array new: 2) at: 1 put: c; at: 2 put: log; yourself)).\n"
    "  ^log!\n"
    "oc\n"
    "  ^OrderedCollection new add: 7; add: 8; add: 9; yourself!\n"
    "set\n"
    "  ^Set new add: 7; add: 8; add: 9; yourself!\n"
    "dict\n"
    "  ^Dictionary new at: 1 put: 7; at: 2 put: 8; at: 3 put: 9; yourself!\n"
    "interval\n"
    "  ^7 to: 9! !\n";

// kNlrProbe を読み込み、`R2Nlr new run: #sel on: (R2Nlr new coll)` の log を返す。
ao::Oop runNlrProbe(Boot& b, const char* sel, const char* coll) {
  std::vector<ao::compiler::CompileError> errs;
  if (!ao::fileInString(b.ctx, kNlrProbe, errs) || !errs.empty()) {
    ADD_FAILURE() << (errs.empty() ? "file-in failed" : errs[0].message);
    return ao::Oop{};
  }
  ao::Root probe(b.roots, send0(b, b.wk.named("R2Nlr"), "new"));
  ao::Root c(b.roots, send0(b, probe.slot, coll));
  ao::Root selector(b.roots, b.wk.intern(sel));
  return send2(b, probe.slot, "run:on:", selector.slot, c.slot);
}

// log が 2 要素で、ブロックが受けた最初の要素（1 番目）が ^ の値（2 番目）と同じなら true。
void expectStoppedAtFirst(Boot& b, ao::Oop log) {
  ASSERT_TRUE(log.isHeap());
  ao::Root held(b.roots, log);
  const ao::Oop n = send0(b, held.slot, "size");
  ASSERT_TRUE(n.isSmallInteger());
  EXPECT_EQ(2, n.smallIntegerValue());
  const ao::Oop first = send1(b, held.slot, "at:", ao::Oop::fromSmallInteger(1));
  const ao::Oop second = send1(b, held.slot, "at:", ao::Oop::fromSmallInteger(2));
  EXPECT_EQ(first, second);
}

}  // namespace

TEST(BlockNonLocalReturn, NonLocalReturnStopsOrderedCollection) {
  Boot b;
  const ao::Oop log = runNlrProbe(b, "firstOf:log:", "oc");
  expectStoppedAtFirst(b, log);
  EXPECT_EQ(7, send1(b, log, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsSet) {
  Boot b;
  expectStoppedAtFirst(b, runNlrProbe(b, "firstOf:log:", "set"));
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsDictionary) {
  Boot b;
  expectStoppedAtFirst(b, runNlrProbe(b, "firstOf:log:", "dict"));
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsInterval) {
  Boot b;
  const ao::Oop log = runNlrProbe(b, "firstOf:log:", "interval");
  expectStoppedAtFirst(b, log);
  EXPECT_EQ(7, send1(b, log, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsDictionaryCollect) {
  Boot b;
  expectStoppedAtFirst(b, runNlrProbe(b, "collectFirst:log:", "dict"));
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsDetectIfNone) {
  Boot b;
  const ao::Oop log = runNlrProbe(b, "detectFirst:log:", "oc");
  expectStoppedAtFirst(b, log);
  EXPECT_EQ(7, send1(b, log, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsInjectInto) {
  Boot b;
  const ao::Oop log = runNlrProbe(b, "injectFirst:log:", "oc");
  expectStoppedAtFirst(b, log);
  EXPECT_EQ(7, send1(b, log, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(BlockNonLocalReturn, NonLocalReturnStopsArrayCollect) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, kNlrProbe, errs));
  ao::Root probe(b.roots, send0(b, b.wk.named("R2Nlr"), "new"));
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0));
  ao::Oop three[3] = {ao::Oop::fromSmallInteger(7), ao::Oop::fromSmallInteger(8),
                      ao::Oop::fromSmallInteger(9)};
  arr.slot = ao::Arr::fromSlots(b.heap, b.wk, three, 3);
  ao::Root selector(b.roots, b.wk.intern("collectFirst:log:"));
  const ao::Oop log = send2(b, probe.slot, "run:on:", selector.slot, arr.slot);
  expectStoppedAtFirst(b, log);
}

// SPEC §3.4: 無限再帰は C スタックを溢れさせず、「stack overflow」で abort する。
TEST(BlockAbort, UnboundedRecursionReportsStackOverflow) {
  Boot b;
  const char* src =
      "!Object subclass: #R2Deep\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B2-Test'!\n"
      "!R2Deep methodsFor: 'r'!\n"
      "recur: n\n"
      "  ^self recur: n + 1! !\n";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, src, errs));
  ao::Root deep(b.roots, send0(b, b.wk.named("R2Deep"), "new"));
  ao::Oop got = ao::Oop::fromSmallInteger(0);
  runOnSmallStack([&] { got = send1(b, deep.slot, "recur:", ao::Oop::fromSmallInteger(0)); });
  EXPECT_TRUE(got.isEmpty());
  EXPECT_TRUE(b.ctx.aborting);
  ASSERT_NE(nullptr, b.ctx.abortReason);
  EXPECT_EQ(std::string("stack overflow"), b.ctx.abortReason);
  ao::clearUnwinding(b.ctx);
  // 巻き戻しのあとも、同じコンテキストで評価を続けられる。
  EXPECT_EQ(3, send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2))
                   .smallIntegerValue());
}

namespace {

// ブロックのネイティブを見るクラス。共有 temp の前でも使えるよう、状態はインスタンス変数 n に置く。
constexpr const char* kBlockProbe =
    "!Object subclass: #R2Blocks\n"
    "  instanceVariableNames: 'n'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Blocks methodsFor: 'probes'!\n"
    "n\n"
    "  ^n!\n"
    "countTo: k\n"
    "  | cond body |\n"
    "  n := 0.\n"
    "  cond := [n < k].\n"
    "  body := [n := n + 1].\n"
    "  ^(cond whileTrue: body) isNil ifTrue: [n] ifFalse: [#notNil]!\n"
    "countDownFrom: k\n"
    "  | cond |\n"
    "  n := k.\n"
    "  cond := [n := n - 1. n = 0].\n"
    "  cond whileFalse.\n"
    "  ^n!\n"
    "repeatTo: k\n"
    "  | body |\n"
    "  n := 0.\n"
    "  body := [n := n + 1. n = k ifTrue: [^n]].\n"
    "  body repeat.\n"
    "  ^#never!\n"
    "times: k\n"
    "  n := 0.\n"
    "  ^k timesRepeat: [n := n + 1]!\n"
    "ensureNormal\n"
    "  n := 0.\n"
    "  ^[1] ensure: [n := n + 1]!\n"
    "ensureNlr\n"
    "  n := 0.\n"
    "  [^7] ensure: [n := 10].\n"
    "  ^0!\n"
    "curtailNormal\n"
    "  n := 0.\n"
    "  ^[1] ifCurtailed: [n := 5]!\n"
    "curtailNlr\n"
    "  n := 0.\n"
    "  [^7] ifCurtailed: [n := 5].\n"
    "  ^0! !\n";

struct BlockProbe {
  Boot b;
  ao::Root probe{b.roots};
  BlockProbe() {
    std::vector<ao::compiler::CompileError> errs;
    const bool ok = ao::fileInString(b.ctx, kBlockProbe, errs);
    EXPECT_TRUE(ok && errs.empty()) << (errs.empty() ? "file-in failed" : errs[0].message);
    probe.slot = send0(b, b.wk.named("R2Blocks"), "new");
  }
  ao::Oop call(const char* sel) { return send0(b, probe.slot, sel); }
  ao::Oop call(const char* sel, std::int64_t k) {
    return send1(b, probe.slot, sel, ao::Oop::fromSmallInteger(k));
  }
  std::int64_t n() { return send0(b, probe.slot, "n").smallIntegerValue(); }
};

// Smalltalk の式を AoTest の外で評価する（テンポラリは宣言して使う）。
ao::Oop evalExpr(Boot& b, const std::string& body) {
  auto img = ao::compiler::compileMethod("doIt\n" + body);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  return ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

}  // namespace

TEST(BlockNatives, WhileTrueNativeWithBlockVariables) {
  BlockProbe p;
  EXPECT_EQ(4, p.call("countTo:", 4).smallIntegerValue());
  EXPECT_EQ(0, p.call("countDownFrom:", 3).smallIntegerValue());
}

TEST(BlockNatives, RepeatStopsOnNonLocalReturn) {
  BlockProbe p;
  EXPECT_EQ(5, p.call("repeatTo:", 5).smallIntegerValue());
}

TEST(BlockNatives, TimesRepeatCounts) {
  BlockProbe p;
  EXPECT_EQ(3, p.call("times:", 3).smallIntegerValue());
  EXPECT_EQ(3, p.n());
  EXPECT_EQ(0, p.call("times:", 0).smallIntegerValue());
  EXPECT_EQ(0, p.n());
}

TEST(BlockNatives, ValueWithFourArgs) {
  Boot b;
  EXPECT_EQ(6, evalExpr(b, "^[:a :b :c | a + b + c] value: 1 value: 2 value: 3")
                   .smallIntegerValue());
  EXPECT_EQ(10, evalExpr(b, "^[:a :b :c :d | a + b + c + d] value: 1 value: 2 value: 3 value: 4")
                    .smallIntegerValue());
}

TEST(BlockNatives, NumArgsAnswersArity) {
  Boot b;
  EXPECT_EQ(0, evalExpr(b, "^[] numArgs").smallIntegerValue());
  EXPECT_EQ(2, evalExpr(b, "^[:a :b | a] numArgs").smallIntegerValue());
}

TEST(BlockNatives, EnsureRunsOnNormalAndNonLocalExit) {
  BlockProbe p;
  EXPECT_EQ(1, p.call("ensureNormal").smallIntegerValue());
  EXPECT_EQ(1, p.n());
  EXPECT_EQ(7, p.call("ensureNlr").smallIntegerValue());
  EXPECT_EQ(10, p.n());
  EXPECT_FALSE(p.b.ctx.nonlocalReturn);
}

TEST(BlockNatives, IfCurtailedRunsOnlyOnUnwind) {
  BlockProbe p;
  EXPECT_EQ(1, p.call("curtailNormal").smallIntegerValue());
  EXPECT_EQ(0, p.n());
  EXPECT_EQ(7, p.call("curtailNlr").smallIntegerValue());
  EXPECT_EQ(5, p.n());
}

// 02 Low: OrderedCollection の内部スロットを引数に展開しない。
TEST(BlockNatives, ValueWithArgumentsRejectsNonArray) {
  Boot b;
  // SPEC §3.3: the rejection aborts with error:'s message instead of answering it.
  const ao::Oop rejected =
      evalExpr(b, "^[:a :b :c | c] valueWithArguments: (OrderedCollection new add: 7; yourself)");
  EXPECT_TRUE(rejected.isEmpty());
  EXPECT_EQ("valueWithArguments: expects an Array", takeAbortReason(b));
  EXPECT_EQ(9, evalExpr(b, "^[:a :b | a + b] valueWithArguments: #(4 5)").smallIntegerValue());
}

namespace {

constexpr const char* kActivationProbe =
    "!Object subclass: #R2Activation\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Activation methodsFor: 'probes'!\n"
    "recursiveSenders\n"
    "  | blk log |\n"
    "  log := OrderedCollection new.\n"
    "  blk := [:b :d |\n"
    "    log add: thisContext sender.\n"
    "    d > 0 ifTrue: [b value: b value: d - 1].\n"
    "    log add: thisContext sender].\n"
    "  blk value: blk value: 1.\n"
    "  ^log!\n"
    "makeBlock\n"
    "  ^[:x | ^x]!\n"
    "useDeadHome\n"
    "  | b log |\n"
    "  log := OrderedCollection new.\n"
    "  b := self makeBlock.\n"
    "  log add: #before.\n"
    "  log add: (b value: 3).\n"
    "  log add: #after.\n"
    "  ^log! !\n";

ao::Oop runActivationProbe(Boot& b, const char* sel) {
  std::vector<ao::compiler::CompileError> errs;
  if (!ao::fileInString(b.ctx, kActivationProbe, errs) || !errs.empty()) {
    ADD_FAILURE() << (errs.empty() ? "file-in failed" : errs[0].message);
    return ao::Oop{};
  }
  ao::Root probe(b.roots, send0(b, b.wk.named("R2Activation"), "new"));
  return send0(b, probe.slot, sel);
}

ao::Oop ocAt(Boot& b, ao::Oop oc, std::int64_t i) {
  return send1(b, oc, "at:", ao::Oop::fromSmallInteger(i));
}

}  // namespace

// 02 Low: 同じブロックを再帰的に起動しても、外側の起動の sender は内側に書き換えられない。
TEST(BlockActivation, RecursiveBlockKeepsOwnSender) {
  Boot b;
  ao::Root log(b.roots, runActivationProbe(b, "recursiveSenders"));
  ASSERT_TRUE(log.slot.isHeap());
  ASSERT_EQ(4, send0(b, log.slot, "size").smallIntegerValue());
  const ao::Oop outerBefore = ocAt(b, log.slot, 1);
  const ao::Oop innerBefore = ocAt(b, log.slot, 2);
  EXPECT_EQ(outerBefore, ocAt(b, log.slot, 4));
  EXPECT_EQ(innerBefore, ocAt(b, log.slot, 3));
  EXPECT_NE(outerBefore, innerBefore);
  EXPECT_EQ(b.wk.methodContextClass, b.heap.klass(outerBefore));
  EXPECT_EQ(b.wk.blockContextClass, b.heap.klass(innerBefore));
}

// 02 Medium: ホームが返ったあとの ^ はホームを探して巻き戻さず、cannotReturn: を送る。SPEC §3.3 /
// §3.4: その既定は error: と同じで、理由のある「cannot return」で評価を中断する。
TEST(BlockActivation, DeadHomeReturnAbortsWithCannotReturn) {
  Boot b;
  EXPECT_TRUE(runActivationProbe(b, "useDeadHome").isEmpty());
  EXPECT_FALSE(b.ctx.nonlocalReturn);
  EXPECT_EQ("cannot return", takeAbortReason(b));
}

// SPEC §3.4: フレームを抜けたコンテキストは pc と sender が nil になる。
TEST(BlockActivation, ExitedContextIsMarkedDead) {
  Boot b;
  auto img = ao::compiler::compileMethod("ctx\n  ^thisContext");
  ASSERT_TRUE(img.ok) << img.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  const ao::Oop got = ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0,
                                           ao::Oop::nil());
  ASSERT_TRUE(got.isHeap());
  EXPECT_EQ(b.wk.methodContextClass, b.heap.klass(got));
  EXPECT_TRUE(b.heap.slotAt(got, ao::kCtxPc).isNil());
  EXPECT_TRUE(b.heap.slotAt(got, ao::kCtxSender).isNil());
}

namespace {

// source をコンパイルして引数なしで走らせる。
ao::Oop runSource(Boot& b, const char* source) {
  auto img = ao::compiler::compileMethod(source);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  return ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

}  // namespace

// 同じクロージャの次の起動にも、前の代入が見える。
TEST(BlockSharedTemps, CounterClosureKeepsState) {
  Boot b;
  ao::Root counter(b.roots, runSource(b, "counter\n  | n |\n  n := 0.\n  ^[n := n + 1]"));
  ASSERT_TRUE(counter.slot.isHeap());
  EXPECT_EQ(1, send0(b, counter.slot, "value").smallIntegerValue());
  EXPECT_EQ(2, send0(b, counter.slot, "value").smallIntegerValue());
  EXPECT_EQ(3, send0(b, counter.slot, "value").smallIntegerValue());
}

TEST(BlockSharedTemps, TwoBlocksShareTemp) {
  Boot b;
  const ao::Oop got = runSource(b,
                                "foo\n  | x inc get |\n  x := 0.\n  inc := [x := x + 1].\n"
                                "  get := [x].\n  inc value.\n  inc value.\n  ^get value");
  EXPECT_EQ(2, got.smallIntegerValue());
}

TEST(BlockSharedTemps, ThreeLevelNesting) {
  Boot b;
  EXPECT_EQ(11, runSource(b, "foo\n  | a |\n  a := 1.\n  [[[a := a + 10] value] value] value.\n  ^a")
                    .smallIntegerValue());
  EXPECT_EQ(6, runSource(b, "foo\n  | a |\n  a := 1.\n"
                            "  ^[:x | [:y | a := a + x + y. a] value: 3] value: 2")
                   .smallIntegerValue());
}

// 05 Critical の失敗シナリオ: do: のブロックで外側の temp に足し込む。
TEST(BlockSharedTemps, DoAccumulatesIntoOuterTemp) {
  Boot b;
  EXPECT_EQ(6, runSource(b, "foo\n  | sum |\n  sum := 0.\n  #(1 2 3) do: [:e | sum := sum + e].\n  ^sum")
                   .smallIntegerValue());
}

// 代入の無い捕捉はコピーのまま。外側を後から書き換えても、作った時点の値を持つ。
TEST(BlockSharedTemps, UnwrittenCaptureIsCopied) {
  Boot b;
  auto img = ao::compiler::compileMethod("foo: x\n  ^[x + 1]");
  ASSERT_TRUE(img.ok) << img.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  ao::Oop arg = ao::Oop::fromSmallInteger(4);
  ao::Root blk(b.roots, ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), &arg, 1,
                                             ao::Oop::nil()));
  ASSERT_TRUE(blk.slot.isHeap());
  EXPECT_EQ(5, send0(b, blk.slot, "value").smallIntegerValue());
}

// ---- SPEC §3.5 インライン展開: 展開した送信は、ネイティブと同じ Blue Book の意味で動く ----

namespace {

// 展開した to:do: が <= と + を本当に送ることを見るクラス。どちらも log に印を積む。
// v:log: はレシーバを返し、+ は新しいインスタンスを返す。
constexpr const char* kStepProbe =
    "!Object subclass: #R2Step\n"
    "  instanceVariableNames: 'v log'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Step methodsFor: 'arithmetic'!\n"
    "v\n"
    "  ^v!\n"
    "v: n log: aLog\n"
    "  v := n.\n"
    "  log := aLog!\n"
    "<= limit\n"
    "  log add: #le.\n"
    "  ^v <= limit!\n"
    "+ delta\n"
    "  log add: #plus.\n"
    "  ^R2Step new v: v + delta log: log! !\n";

// mustBeBoolean を上書きしたクラス。R2Truthy は true を、R2Murky は Boolean でない値を答える。
constexpr const char* kTruthProbe =
    "!Object subclass: #R2Truthy\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Truthy methodsFor: 'coercing'!\n"
    "mustBeBoolean\n"
    "  ^true! !\n"
    "!Object subclass: #R2Murky\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Murky methodsFor: 'coercing'!\n"
    "mustBeBoolean\n"
    "  ^3! !\n";

bool fileIn(Boot& b, const char* source) {
  std::vector<ao::compiler::CompileError> errs;
  if (!ao::fileInString(b.ctx, source, errs) || !errs.empty()) {
    ADD_FAILURE() << (errs.empty() ? "file-in failed" : errs[0].message);
    return false;
  }
  return true;
}

// source の評価が「NonBoolean receiver」で中断したことを確かめ、中断の状態を消す。
void expectNonBooleanAbort(Boot& b, const char* source) {
  SCOPED_TRACE(source);
  const ao::Oop got = runSource(b, source);
  EXPECT_TRUE(got.isEmpty());
  EXPECT_TRUE(b.ctx.aborting);
  if (b.ctx.abortReason == nullptr) {
    ADD_FAILURE() << "no abort reason";
  } else {
    EXPECT_EQ(std::string("NonBoolean receiver"), b.ctx.abortReason);
  }
  ao::clearUnwinding(b.ctx);
}

}  // namespace

namespace {

// runSource の GC で Symbol も動く。値を得たあとで intern し、今の番地と比べる。
void expectSymbol(Boot& b, const char* name, ao::Oop got) {
  EXPECT_EQ(b.wk.intern(name), got) << name;
}

}  // namespace

// 展開した分岐の中の代入は、外側の temp に書く。値は選んだブロックの値。
TEST(BlockInline, IfTrueAssignsOuterTemp) {
  Boot b;
  EXPECT_EQ(1, runSource(b, "foo\n  | y |\n  y := 0.\n  3 > 1 ifTrue: [y := 1].\n  ^y")
                   .smallIntegerValue());
  EXPECT_EQ(2, runSource(b, "foo\n  | y |\n  y := 0.\n  3 < 1 ifTrue: [y := 1] ifFalse: [y := 2].\n  ^y")
                   .smallIntegerValue());
  expectSymbol(b, "yes", runSource(b, "foo\n  ^3 > 1 ifFalse: [#no] ifTrue: [#yes]"));
  expectSymbol(b, "no", runSource(b, "foo\n  ^3 < 1 ifTrue: [#yes] ifFalse: [#no]"));
}

// 選ぶ分岐が無ければ nil。
TEST(BlockInline, FalseIfTrueIsNil) {
  Boot b;
  EXPECT_TRUE(runSource(b, "foo\n  ^false ifTrue: [1]").isNil());
  EXPECT_TRUE(runSource(b, "foo\n  ^true ifFalse: [1]").isNil());
  EXPECT_EQ(1, runSource(b, "foo\n  ^true ifTrue: [1]").smallIntegerValue());
  EXPECT_EQ(1, runSource(b, "foo\n  ^false ifFalse: [1]").smallIntegerValue());
}

// and: はレシーバが false なら false、or: は true なら true。引数ブロックは評価しない。
// それ以外は引数ブロックの値。
TEST(BlockInline, AndOrShortCircuit) {
  Boot b;
  EXPECT_TRUE(runSource(b, "foo\n  ^false and: [1 / 0]").isFalse());
  EXPECT_TRUE(runSource(b, "foo\n  ^true or: [nil foo]").isTrue());
  EXPECT_EQ(7, runSource(b, "foo\n  ^true and: [7]").smallIntegerValue());
  EXPECT_EQ(7, runSource(b, "foo\n  ^false or: [7]").smallIntegerValue());
  EXPECT_EQ(1100, runSource(b,
                            "foo\n  | n |\n  n := 0.\n"
                            "  false and: [n := n + 1].\n  true or: [n := n + 10].\n"
                            "  true and: [n := n + 100].\n  false or: [n := n + 1000].\n  ^n")
                      .smallIntegerValue());
  EXPECT_FALSE(b.ctx.aborting);
}

// while ループの値は nil。
TEST(BlockInline, WhileLoopsAnswerNil) {
  Boot b;
  EXPECT_TRUE(runSource(b, "foo\n  | i |\n  i := 0.\n  ^[i < 3] whileTrue: [i := i + 1]").isNil());
  EXPECT_TRUE(runSource(b, "foo\n  | i |\n  i := 0.\n  ^[i >= 3] whileFalse: [i := i + 1]").isNil());
  EXPECT_TRUE(runSource(b, "foo\n  | i |\n  i := 0.\n  ^[i := i + 1. i < 3] whileTrue").isNil());
  EXPECT_TRUE(runSource(b, "foo\n  | i |\n  i := 0.\n  ^[i := i + 1. i >= 3] whileFalse").isNil());
  EXPECT_EQ(3, runSource(b, "foo\n  | i |\n  i := 0.\n  [i := i + 1. i >= 3] whileFalse.\n  ^i")
                   .smallIntegerValue());
}

// 展開したブロック内の ^ は、含む実スコープの ^。メソッドなら、そのメソッドから返る。
TEST(BlockInline, ReturnInsideInlinedLoop) {
  Boot b;
  EXPECT_EQ(5, runSource(b,
                         "foo\n  | i |\n  i := 0.\n"
                         "  [true] whileTrue: [i := i + 1. i = 5 ifTrue: [^i]].\n  ^0")
                   .smallIntegerValue());
  EXPECT_EQ(4, runSource(b, "foo\n  1 to: 10 do: [:k | k = 4 ifTrue: [^k]].\n  ^0")
                   .smallIntegerValue());
  expectSymbol(b, "cond", runSource(b, "foo\n  | i |\n  i := 0.\n"
                         "  [i := i + 1. i = 2 ifTrue: [^#cond]. false] whileFalse.\n  ^0"));
  // 展開しないブロックの中の展開したループからの ^ は、ホームのメソッドから返る。
  EXPECT_EQ(30, runSource(b, "foo\n  [:n | 1 to: 10 do: [:k | k = n ifTrue: [^k * 10]]] value: 3.\n  ^0")
                    .smallIntegerValue());
  EXPECT_FALSE(b.ctx.nonlocalReturn);
}

// to:do: の値はレシーバ。ループは上限まで回る。
TEST(BlockInline, ToDoAnswersReceiverAndSums) {
  Boot b;
  EXPECT_EQ(11, runSource(b, "foo\n  | s |\n  s := 0.\n  ^(1 to: 4 do: [:k | s := s + k]) + s")
                    .smallIntegerValue());
  // 上限がレシーバより小さければ、一度も回らない。
  EXPECT_EQ(500, runSource(b, "foo\n  | s |\n  s := 0.\n  ^(5 to: 1 do: [:k | s := s + 1]) * 100 + s")
                     .smallIntegerValue());
}

// to:by:do: は刻みで進み、刻みが負なら >= で止まる。値はレシーバ。
TEST(BlockInline, ToByDoNegativeStep) {
  Boot b;
  EXPECT_EQ(10741, runSource(b,
                             "foo\n  | s |\n  s := 0.\n"
                             "  10 to: 1 by: -3 do: [:k | s := s * 10 + k].\n  ^s")
                       .smallIntegerValue());
  EXPECT_EQ(159, runSource(b,
                           "foo\n  | s |\n  s := 0.\n"
                           "  1 to: 10 by: 4 do: [:k | s := s * 10 + k].\n  ^s")
                     .smallIntegerValue());
  EXPECT_EQ(10, runSource(b, "foo\n  ^10 to: 1 by: -3 do: [:k | k]").smallIntegerValue());
}

// ループ変数を捕捉したクロージャは、作った反復の値を持つ。
TEST(BlockInline, LoopVarCapturedPerIteration) {
  Boot b;
  EXPECT_EQ(123, runSource(b,
                           "foo\n  | blocks |\n  blocks := OrderedCollection new.\n"
                           "  1 to: 3 do: [:k | blocks add: [k]].\n"
                           "  ^((blocks at: 1) value * 100) + ((blocks at: 2) value * 10) + "
                           "(blocks at: 3) value")
                     .smallIntegerValue());
}

// 展開したブロックの temp は、入るたびに（ループなら反復ごとに）nil から始まる。捕捉した temp は
// 反復ごとに別の temp ベクタに入るので、クロージャはその反復の temp を共有する。
TEST(BlockInline, InlinedLoopTempFreshEachIteration) {
  Boot b;
  {
    ao::Root seen(b.roots, runSource(b,
                                     "foo\n  | i seen |\n  i := 0.\n  seen := OrderedCollection new.\n"
                                     "  [i < 3] whileTrue: [| t | seen add: t. t := i. i := i + 1].\n"
                                     "  ^seen"));
    ASSERT_TRUE(seen.slot.isHeap());
    ASSERT_EQ(3, send0(b, seen.slot, "size").smallIntegerValue());
    for (std::int64_t k = 1; k <= 3; ++k) {
      EXPECT_TRUE(ocAt(b, seen.slot, k).isNil()) << k;
    }
  }
  {
    ao::Root seen(b.roots, runSource(b,
                                     "foo\n  | seen |\n  seen := OrderedCollection new.\n"
                                     "  1 to: 3 do: [:k | true ifTrue: [| t | seen add: t. t := k]].\n"
                                     "  ^seen"));
    ASSERT_TRUE(seen.slot.isHeap());
    ASSERT_EQ(3, send0(b, seen.slot, "size").smallIntegerValue());
    for (std::int64_t k = 1; k <= 3; ++k) {
      EXPECT_TRUE(ocAt(b, seen.slot, k).isNil()) << k;
    }
  }
  EXPECT_EQ(12, runSource(b,
                          "foo\n  | i blocks |\n  i := 0.\n  blocks := OrderedCollection new.\n"
                          "  [i < 3] whileTrue: [| t | t := i. blocks add: [t]. i := i + 1].\n"
                          "  ^((blocks at: 1) value * 100) + ((blocks at: 2) value * 10) + "
                          "(blocks at: 3) value")
                    .smallIntegerValue());
  // 捕捉のあとの代入も、同じ反復のクロージャには見える（共有）。次の反復には持ち越さない。
  EXPECT_EQ(100101102,
            runSource(b,
                      "foo\n  | i blocks |\n  i := 0.\n  blocks := OrderedCollection new.\n"
                      "  [i < 3] whileTrue: [| t | t := i. blocks add: [t]. t := t + 100. i := i + 1].\n"
                      "  ^((blocks at: 1) value * 1000000) + ((blocks at: 2) value * 1000) + "
                      "(blocks at: 3) value")
                .smallIntegerValue());
}

// to:do: と to:by:do: の上限は 1 回だけ評価する。OrderedCollection>>add: は引数を答える。
TEST(BlockInline, LimitEvaluatedOnce) {
  Boot b;
  EXPECT_EQ(106, runSource(b,
                           "foo\n  | c s |\n  c := OrderedCollection new.\n  s := 0.\n"
                           "  1 to: (c add: 3) do: [:k | s := s + k].\n  ^(c size * 100) + s")
                     .smallIntegerValue());
  EXPECT_EQ(122, runSource(b,
                           "foo\n  | c s |\n  c := OrderedCollection new.\n  s := 0.\n"
                           "  10 to: (c add: 1) by: -3 do: [:k | s := s + k].\n  ^(c size * 100) + s")
                     .smallIntegerValue());
}

// SPEC §3.5: 継続の判定は <=、増分は + を送る。レシーバは SmallInteger でなくてよい。
// 期待する順: 判定、本体、増分を繰り返し、判定が false で抜ける。値はレシーバ。
TEST(BlockInline, ToDoSendsComparisonAndIncrement) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kStepProbe));
  ao::Root log(b.roots, runSource(b,
                                  "foo\n  | log start |\n  log := OrderedCollection new.\n"
                                  "  start := R2Step new v: 1 log: log.\n"
                                  "  log add: (start to: 3 do: [:k | log add: k v]) == start.\n"
                                  "  ^log"));
  ASSERT_TRUE(log.slot.isHeap());
  ASSERT_EQ(11, send0(b, log.slot, "size").smallIntegerValue());
  const ao::Oop le = b.wk.intern("le");
  const ao::Oop plus = b.wk.intern("plus");
  EXPECT_EQ(le, ocAt(b, log.slot, 1));
  EXPECT_EQ(1, ocAt(b, log.slot, 2).smallIntegerValue());
  EXPECT_EQ(plus, ocAt(b, log.slot, 3));
  EXPECT_EQ(le, ocAt(b, log.slot, 4));
  EXPECT_EQ(2, ocAt(b, log.slot, 5).smallIntegerValue());
  EXPECT_EQ(plus, ocAt(b, log.slot, 6));
  EXPECT_EQ(le, ocAt(b, log.slot, 7));
  EXPECT_EQ(3, ocAt(b, log.slot, 8).smallIntegerValue());
  EXPECT_EQ(plus, ocAt(b, log.slot, 9));
  EXPECT_EQ(le, ocAt(b, log.slot, 10));
  EXPECT_TRUE(ocAt(b, log.slot, 11).isTrue());
}

// 展開したブロックは起動を作らないので、中の thisContext は含む実スコープのコンテキスト。
TEST(BlockInline, InlinedBlockRunsInEnclosingContext) {
  Boot b;
  EXPECT_TRUE(runSource(b, "foo\n  ^(true ifTrue: [thisContext]) == thisContext").isTrue());
  EXPECT_TRUE(runSource(b, "foo\n  | c |\n  1 to: 1 do: [:k | c := thisContext].\n  ^c == thisContext")
                  .isTrue());
}

// SPEC §3.5: 展開した分岐のレシーバが Boolean でなければ mustBeBoolean を送る。答えが Boolean なら
// それで分岐する。
TEST(BlockInline, MustBeBooleanAnswerChoosesBranch) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kTruthProbe));
  expectSymbol(b, "yes", runSource(b, "foo\n  ^R2Truthy new ifTrue: [#yes] ifFalse: [#no]"));
  EXPECT_EQ(7, runSource(b, "foo\n  ^R2Truthy new and: [7]").smallIntegerValue());
  EXPECT_FALSE(b.ctx.aborting);
}

// Object>>mustBeBoolean の既定、または Boolean でない答えは、「NonBoolean receiver」で中断する。
// 中断を消せば、同じコンテキストで評価を続けられる。
TEST(BlockInline, NonBooleanReceiverAborts) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kTruthProbe));
  expectNonBooleanAbort(b, "foo\n  ^nil ifTrue: [1] ifFalse: [2]");
  expectNonBooleanAbort(b, "foo\n  ^3 and: [true]");
  expectNonBooleanAbort(b, "foo\n  [nil] whileTrue: [nil].\n  ^0");
  expectNonBooleanAbort(b, "foo\n  ^R2Murky new ifTrue: [1]");
  EXPECT_EQ(3, runSource(b, "foo\n  ^1 + 2").smallIntegerValue());
}

// vendor の LinkedList>>do: は whileFalse: と外側の temp への代入で回る。
TEST(BlockInline, LinkedListDoCountsLinks) {
  Boot b;
  std::vector<ao::FileInError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER", errs));
  ASSERT_TRUE(b.wk.named("LinkedList").isHeap());
  ASSERT_TRUE(b.wk.named("Link").isHeap());
  EXPECT_EQ(3, runSource(b,
                         "foo\n  | list n |\n  list := LinkedList new.\n"
                         "  list add: Link new; add: Link new; add: Link new.\n"
                         "  n := 0.\n  list do: [:each | n := n + 1].\n  ^n")
                   .smallIntegerValue());
  EXPECT_FALSE(b.ctx.aborting);
}

namespace {

constexpr const char* kDeepEnsure =
    "!Object subclass: #R2DeepEnsure\n"
    "  instanceVariableNames: 'levels enter exit'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2DeepEnsure methodsFor: 'r'!\n"
    "start\n"
    "  levels := 0.\n"
    "  enter := 0.\n"
    "  exit := 0.\n"
    "  ^self recur!\n"
    "recur\n"
    "  levels := levels + 1.\n"
    "  ^[enter := enter + 1. self recur] ensure: [self b1]!\n"
    "b1\n"
    "  ^self b2!\n"
    "b2\n"
    "  ^self b3!\n"
    "b3\n"
    "  ^self b4!\n"
    "b4\n"
    "  exit := exit + 1!\n"
    "levels\n"
    "  ^levels!\n"
    "enter\n"
    "  ^enter!\n"
    "exit\n"
    "  ^exit! !\n";

}  // namespace

// レビュー指摘: 前のスレッドのスタックの範囲が残っていても、最外の入口で取り直す。取り直さないと、
// ガードが効かずに C スタックを溢れさせる。
TEST(BlockAbort, StaleStackRangeIsRefreshedAtOutermostEntry) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #R2Stale\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B2-Test'!\n"
                               "!R2Stale methodsFor: 'r'!\n"
                               "recur: n\n"
                               "  ^self recur: n + 1! !\n",
                               errs));
  ao::Root obj(b.roots, send0(b, b.wk.named("R2Stale"), "new"));
  ao::Oop got = ao::Oop::fromSmallInteger(0);
  runOnSmallStack([&] {
    // どの番地も「範囲内」に見える古い範囲。
    b.ctx.stackLimit = 1;
    b.ctx.stackHigh = ~std::uintptr_t{0};
    got = send1(b, obj.slot, "recur:", ao::Oop::fromSmallInteger(0));
  });
  EXPECT_TRUE(got.isEmpty());
  ASSERT_TRUE(b.ctx.aborting);
  EXPECT_EQ(std::string("stack overflow"), b.ctx.abortReason);
}

// レビュー指摘: stack overflow の abort 中も、限界近くの ensure: の後始末（数段の送信）を走らせる。
TEST(BlockAbort, EnsureCleanupRunsNearStackLimit) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, kDeepEnsure, errs));
  ASSERT_TRUE(errs.empty()) << errs[0].message;
  ao::Root obj(b.roots, send0(b, b.wk.named("R2DeepEnsure"), "new"));
  runOnSmallStack([&] { send0(b, obj.slot, "start"); });
  ASSERT_TRUE(b.ctx.aborting);
  EXPECT_EQ(std::string("stack overflow"), b.ctx.abortReason);
  ao::clearUnwinding(b.ctx);
  // levels は ensure: を送る前に数えた段、enter は本体が始まった段、exit は後始末が終わった回数。
  // 本体が始まった段の後始末はすべて走り（enter <= exit）、どの段でも後始末は 1 回まで
  // （exit <= levels）。ガードがどこで当たるかはスタックとフレームの大きさで決まる。最も深い段で
  // ensure: の適用か、そのレシーバのブロックの適用で当たると、本体はその段だけ始まらない
  // （levels - enter が 1）。後者では後始末は走る（SPEC §3.4）。
  const std::int64_t levels = send0(b, obj.slot, "levels").smallIntegerValue();
  const std::int64_t enter = send0(b, obj.slot, "enter").smallIntegerValue();
  const std::int64_t exit = send0(b, obj.slot, "exit").smallIntegerValue();
  SCOPED_TRACE(testing::Message() << "levels " << levels << " enter " << enter << " exit " << exit);
  EXPECT_GT(enter, 0);
  EXPECT_LE(enter, exit);
  EXPECT_LE(exit, levels);
  EXPECT_LE(levels - enter, 1);
}

// B10 / SPEC §3.4: ファイバは自分のスタックで動く。fiberStackHigh が 0 でなければ、ガードはその範囲を
// 取り、予約分はスレッドと同じ式（min(512 KiB, size / 4)、後始末はその半分）で決める。0 に戻せば
// スレッドのスタックに戻る。
TEST(BlockAbort, RefreshStackLimitUsesTheFiberStackRange) {
  Boot b;
  const std::uintptr_t low = std::uintptr_t{1} << 32;
  b.ctx.fiberStackLow = low;
  b.ctx.fiberStackHigh = low + (std::uintptr_t{8} << 20);
  ao::refreshStackLimit(b.ctx);
  EXPECT_EQ(low + (std::uintptr_t{8} << 20), b.ctx.stackHigh);
  EXPECT_EQ(low + 512 * 1024, b.ctx.stackLimit);
  EXPECT_EQ(low + 256 * 1024, b.ctx.stackCleanupLimit);

  b.ctx.fiberStackHigh = low + (std::uintptr_t{1} << 20);
  ao::refreshStackLimit(b.ctx);
  EXPECT_EQ(low + (std::uintptr_t{1} << 20), b.ctx.stackHigh);
  EXPECT_EQ(low + 256 * 1024, b.ctx.stackLimit);
  EXPECT_EQ(low + 128 * 1024, b.ctx.stackCleanupLimit);

  b.ctx.fiberStackLow = 0;
  b.ctx.fiberStackHigh = 0;
  ao::refreshStackLimit(b.ctx);
  EXPECT_EQ(reinterpret_cast<std::uintptr_t>(pthread_get_stackaddr_np(pthread_self())),
            b.ctx.stackHigh);
}

// B10 / SPEC §3.4: ファイバの範囲を持つコンテキストでは、スレッドのスタックがもっと大きくても、ガードは
// その範囲の中で当たる。最外の入口と applyMethod の取り直しが、どちらもファイバの範囲を使う。
TEST(BlockAbort, StackGuardUsesTheFiberStackRange) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #R10Deep\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B10-Test'!\n"
                               "!R10Deep methodsFor: 'r'!\n"
                               "recur: n\n"
                               "  ^self recur: n + 1! !\n",
                               errs));
  ao::Root obj(b.roots, send0(b, b.wk.named("R10Deep"), "new"));
  ao::Oop got = ao::Oop::fromSmallInteger(0);
  std::uintptr_t high = 0;
  std::uintptr_t low = 0;
  runOnSmallStack(
      [&] {
        // この関数のフレームから下の 1 MiB を、ファイバのスタックに見立てる。
        high = reinterpret_cast<std::uintptr_t>(__builtin_frame_address(0)) & ~std::uintptr_t{15};
        low = high - (std::uintptr_t{1} << 20);
        b.ctx.fiberStackLow = low;
        b.ctx.fiberStackHigh = high;
        got = send1(b, obj.slot, "recur:", ao::Oop::fromSmallInteger(0));
      },
      std::size_t{8} << 20);
  b.ctx.fiberStackLow = 0;
  b.ctx.fiberStackHigh = 0;
  EXPECT_TRUE(got.isEmpty());
  ASSERT_TRUE(b.ctx.aborting);
  EXPECT_EQ(std::string("stack overflow"), b.ctx.abortReason);
  EXPECT_EQ(high, b.ctx.stackHigh);
  EXPECT_EQ(low + 256 * 1024, b.ctx.stackLimit);
  ao::clearUnwinding(b.ctx);
}

namespace {

// 送った先で「NonBoolean receiver」の abort を始めるクラス（SPEC §3.4）。R2Sour はキーや座標や
// コレクションとして受けた hash = < <= > + size を中断する。R2Slots は hash（1。キー 1 と同じ値なので
// ハッシュ表が = を送る）と size を普通に答え、= と at:put: を中断する。R2Sink（Stream）は nextPut: を中断する。R2Seen は ensure: の後始末から
// 見た Dictionary の大きさを n に残す。
constexpr const char* kSourProbe =
    "!Object subclass: #R2Sour\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Sour methodsFor: 'aborting'!\n"
    "hash\n"
    "  ^nil ifTrue: [1]!\n"
    "= other\n"
    "  ^nil ifTrue: [false]!\n"
    "< other\n"
    "  ^nil ifTrue: [false]!\n"
    "<= other\n"
    "  ^nil ifTrue: [false]!\n"
    "> other\n"
    "  ^nil ifTrue: [false]!\n"
    "+ other\n"
    "  ^nil ifTrue: [self]!\n"
    "size\n"
    "  ^nil ifTrue: [0]! !\n"
    "!Object subclass: #R2Slots\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Slots methodsFor: 'aborting'!\n"
    "hash\n"
    "  ^1!\n"
    "size\n"
    "  ^3!\n"
    "= other\n"
    "  ^nil ifTrue: [false]!\n"
    "at: i put: v\n"
    "  ^nil ifTrue: [v]! !\n"
    "!Stream subclass: #R2Sink\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Sink methodsFor: 'aborting'!\n"
    "nextPut: c\n"
    "  ^nil ifTrue: [c]! !\n"
    "!Object subclass: #R2Seen\n"
    "  instanceVariableNames: 'n'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2Seen methodsFor: 'probes'!\n"
    "n\n"
    "  ^n!\n"
    "atPutSeen\n"
    "  | d |\n"
    "  d := Dictionary new.\n"
    "  [d at: R2Sour new put: 1] ensure: [n := d size].\n"
    "  ^d! !\n";

// C++ から送ったネイティブの答えが空 OOP で、「NonBoolean receiver」の中断が続いていることを
// 確かめ、中断の状態を消す。
void expectAbortedEmpty(Boot& b, ao::Oop got) {
  EXPECT_TRUE(got.isEmpty());
  EXPECT_TRUE(b.ctx.aborting);
  if (b.ctx.abortReason == nullptr) {
    ADD_FAILURE() << "no abort reason";
  } else {
    EXPECT_EQ(std::string("NonBoolean receiver"), b.ctx.abortReason);
  }
  ao::clearUnwinding(b.ctx);
}

std::int64_t sizeOf(Boot& b, ao::Oop coll) {
  const ao::Oop n = send0(b, coll, "size");
  EXPECT_TRUE(n.isSmallInteger());
  return n.isSmallInteger() ? n.smallIntegerValue() : -1;
}

}  // namespace

// レビュー指摘: hash が abort を始めても、Dictionary>>at:put: は対を入れずに戻る。ensure: の後始末
// から見た大きさは 0 のままである（SPEC §3.4）。
TEST(NativeSendUnwind, DictionaryAtPutSkipsInsertWhenHashAborts) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kSourProbe));
  ao::Root seen(b.roots, send0(b, b.wk.named("R2Seen"), "new"));
  expectAbortedEmpty(b, send0(b, seen.slot, "atPutSeen"));
  const ao::Oop n = send0(b, seen.slot, "n");
  ASSERT_TRUE(n.isSmallInteger());
  EXPECT_EQ(0, n.smallIntegerValue());
}

// SPEC §3.4: キーに送った hash か = が abort を始めたら、Dictionary / Set のネイティブは探索も挿入も
// やめて空 OOP を返す。大きさは変わらない。Dictionary>>includes: は値に = を送り、hash は送らない
// （SPEC §3.6）ので、空でない表で確かめる。IdentityDictionary と IdentitySet は hash も = も送らない。
TEST(NativeSendUnwind, HashedCollectionsStopWhenHashOrEqualsAborts) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kSourProbe));
  struct Case {
    const char* coll;  // 中断しない式
    const char* sel;
    const char* key;   // R2Sour は hash、R2Slots は = で中断する
    std::int64_t size;
  };
  const Case cases[] = {
      {"^Dictionary new", "at:put:", "R2Sour", 0},
      {"^Dictionary new", "at:", "R2Sour", 0},
      {"^Dictionary new", "includesKey:", "R2Sour", 0},
      {"^Dictionary new at: 1 put: 1; yourself", "includes:", "R2Sour", 1},
      {"^Set new", "add:", "R2Sour", 0},
      {"^Set new", "includes:", "R2Sour", 0},
      {"^Dictionary new at: 1 put: 1; yourself", "at:put:", "R2Slots", 1},
      {"^Dictionary new at: 1 put: 1; yourself", "includes:", "R2Slots", 1},
      {"^Set new add: 1; yourself", "add:", "R2Slots", 1},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(std::string(c.coll) + " " + c.sel + " " + c.key);
    ao::Root coll(b.roots, evalExpr(b, c.coll));
    ASSERT_TRUE(coll.slot.isHeap());
    ao::Root key(b.roots, send0(b, b.wk.named(c.key), "new"));
    const bool atPut = std::string(c.sel) == "at:put:";
    expectAbortedEmpty(b, atPut ? send2(b, coll.slot, c.sel, key.slot, ao::Oop::fromSmallInteger(2))
                                : send1(b, coll.slot, c.sel, key.slot));
    EXPECT_EQ(c.size, sizeOf(b, coll.slot));
  }
}

// SPEC §3.4: collection の at:put: が abort を始めたら、WriteStream>>nextPut: は位置を進めない。
TEST(NativeSendUnwind, WriteStreamKeepsPositionWhenAtPutAborts) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kSourProbe));
  ao::Root s(b.roots, evalExpr(b, "^WriteStream on: R2Slots new"));
  ASSERT_TRUE(s.slot.isHeap());
  expectAbortedEmpty(b, send1(b, s.slot, "nextPut:", ao::Oop::fromSmallInteger(7)));
  const ao::Oop pos = send0(b, s.slot, "position");
  ASSERT_TRUE(pos.isSmallInteger());
  EXPECT_EQ(0, pos.smallIntegerValue());
}

// SPEC §3.4: 送った先が abort を始めたら、ネイティブは残りの要素や座標に送らず、新しいオブジェクトも
// 作らず、直ちに空 OOP を返す。答えだけを見る（どれも、中断のあとに見える副作用を持たない）。
TEST(NativeSendUnwind, NativesAnswerEmptyWhenTheirSendAborts) {
  Boot b;
  ASSERT_TRUE(fileIn(b, kSourProbe));
  constexpr const char* kRect =
      "^Rectangle origin: (Point x: R2Sour new y: 0) corner: (Point x: 5 y: 5)";
  struct Case {
    const char* rcvr;  // 中断しない式
    const char* sel;
    const char* arg;   // nullptr なら単項
  };
  const Case cases[] = {
      {"^(Array new: 1) at: 1 put: R2Sour new; yourself", "=",
       "^(Array new: 1) at: 1 put: 1; yourself"},
      {"^Point x: R2Sour new y: 1", "+", "^Point x: 1 y: 1"},
      {"^Point x: R2Sour new y: 1", "=", "^Point x: 1 y: 1"},
      {kRect, "containsPoint:", "^Point x: 1 y: 1"},
      {kRect, "intersect:", "^Rectangle origin: (Point x: 1 y: 1) corner: (Point x: 4 y: 4)"},
      {"^Interval from: R2Sour new to: 3 by: 1", "size", nullptr},
      {"^ReadStream", "on:", "^R2Sour new"},
      {"| s | s := WriteStream on: (Array new: 0). s instVarAt: 1 put: R2Sour new. ^s",
       "nextPut:", "^1"},
      {"^R2Sink new", "cr", nullptr},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(std::string(c.rcvr) + " " + c.sel);
    ao::Root rcvr(b.roots, evalExpr(b, c.rcvr));
    ASSERT_TRUE(rcvr.slot.isHeap());
    if (c.arg == nullptr) {
      expectAbortedEmpty(b, send0(b, rcvr.slot, c.sel));
      continue;
    }
    ao::Root arg(b.roots, evalExpr(b, c.arg));
    ASSERT_FALSE(arg.slot.isEmpty());
    expectAbortedEmpty(b, send1(b, rcvr.slot, c.sel, arg.slot));
  }
}

namespace {

int gB3PrintStrings = 0;

ao::Oop countingPrintString(ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  ++gB3PrintStrings;
  return ao::Str::fromUtf8(ctx, "B3Eq");
}

}  // namespace

// 指摘 4 / SPEC §3.4: assert:equals: は、= が abort したら printString も error: も送らない。
// Smalltalk のメソッドは巻き戻しの最中に始まらないので、数えるネイティブの printString で見る。
TEST(AoTestRunner, AssertEqualsStopsAfterAbortingEquals) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #B3Eq\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B3-Test'!\n"
                               "!B3Eq methodsFor: 'b3'!\n"
                               "= other\n"
                               "  ^self kaboom! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  const ao::Oop cls = b.wk.named("B3Eq");
  ASSERT_TRUE(cls.isHeap());
  ASSERT_TRUE(ao::kernel::putNative(b.heap, b.wk, &b.cache, cls, "printString", 0,
                                    "b3_test_countingPrintString", countingPrintString));
  TestDir dir("ao-test-runner-aborting-equals");
  ASSERT_TRUE(dir.write("a.st", "self assert: B3Eq new equals: 1.\n"));
  gB3PrintStrings = 0;
  testing::internal::CaptureStderr();
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  const std::string err = testing::internal::GetCapturedStderr();
  EXPECT_EQ(1, code);
  EXPECT_NE(std::string::npos, err.find("a.st: doesNotUnderstand: #kaboom")) << err;
  EXPECT_EQ(0, gB3PrintStrings);
  // 一致しないだけなら、従来どおり両方の printString を理由にする。
  ASSERT_TRUE(dir.write("a.st", "self assert: 1 equals: B3Eq new.\n"));
  testing::internal::CaptureStderr();
  EXPECT_EQ(1, ao::runSmalltalkTests(b.ctx, dir.path.string()));
  const std::string mismatch = testing::internal::GetCapturedStderr();
  EXPECT_NE(std::string::npos, mismatch.find("a.st: 1 ~= B3Eq")) << mismatch;
  EXPECT_EQ(1, gB3PrintStrings);
}

namespace {

ao::Oop abortingSubclass(ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::abortEvaluation(ctx, std::string("no test class"));
}

}  // namespace

// 指摘 5 / SPEC §3.4: テストクラスの作成は最外である。前の abort を持ち越さずに作る。
// クラス側の subclass:… が Smalltalk のメソッドなら、持ち越した abort で途中から巻き戻ってしまう。
TEST(AoTestRunner, TestClassIsMadeAfterLeftOverAbort) {
  Boot b;
  TestDir dir("ao-test-runner-left-over-abort");
  ASSERT_TRUE(dir.write("a.st", "self assert: 1 + 2 equals: 3.\n"));
  auto img = ao::compiler::compileMethod(
      "subclass: a instanceVariableNames: b classVariableNames: c poolDictionaries: d "
      "category: e\n"
      "  ^super subclass: a instanceVariableNames: b classVariableNames: c poolDictionaries: d "
      "category: e");
  ASSERT_TRUE(img.ok) << img.error.message;
  ASSERT_TRUE(ao::installMethod(b.ctx, b.heap.klass(b.wk.objectClass), img.image).isHeap());
  (void)ao::abortEvaluation(b.ctx, std::string("left over"));
  testing::internal::CaptureStderr();
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  const std::string err = testing::internal::GetCapturedStderr();
  EXPECT_EQ(0, code) << err;
  EXPECT_FALSE(ao::unwinding(b.ctx));
}

// 指摘 5 / SPEC §3.4: テストクラスの作成の abort は ao --test の失敗で、理由を読んで消す。
TEST(AoTestRunner, AbortingTestClassFailsAndClears) {
  Boot b;
  TestDir dir("ao-test-runner-aborting-class");
  ASSERT_TRUE(dir.write("a.st", "self assert: 1 + 2 equals: 3.\n"));
  ASSERT_TRUE(ao::kernel::putNative(
      b.heap, b.wk, &b.cache, b.heap.klass(b.wk.objectClass),
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:", 5,
      "b3_test_abortingSubclass", abortingSubclass));
  testing::internal::CaptureStderr();
  const int code = ao::runSmalltalkTests(b.ctx, dir.path.string());
  const std::string err = testing::internal::GetCapturedStderr();
  EXPECT_EQ(1, code);
  EXPECT_NE(std::string::npos, err.find("no test class")) << err;
  EXPECT_FALSE(ao::unwinding(b.ctx));
  EXPECT_EQ("", ao::abortReasonText(b.ctx));
}

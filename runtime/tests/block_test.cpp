#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Interpreter.hpp"
#include "ao/TestRunner.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Send.hpp"

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

TEST(BlockEval, CopyDoesNotWriteOuter) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "foo\n  | x |\n  x := 1.\n  [x := 2] value.\n  ^x");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(1, got.smallIntegerValue());
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
  const ao::Oop rejected =
      evalExpr(b, "^[:a :b :c | c] valueWithArguments: (OrderedCollection new add: 7; yourself)");
  ASSERT_TRUE(rejected.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(rejected));
  EXPECT_EQ("valueWithArguments: expects an Array", ao::Str::toUtf8(b.heap, rejected));
  EXPECT_EQ(9, evalExpr(b, "^[:a :b | a + b] valueWithArguments: #(4 5)").smallIntegerValue());
}

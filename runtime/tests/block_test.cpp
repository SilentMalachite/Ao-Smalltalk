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

// 02 Medium: ホームが返ったあとの ^ は cannotReturn: の答えをブロックの値にし、呼び出し元は続く。
TEST(BlockActivation, DeadHomeReturnAnswersErrorAndContinues) {
  Boot b;
  ao::Root log(b.roots, runActivationProbe(b, "useDeadHome"));
  ASSERT_TRUE(log.slot.isHeap());
  ASSERT_EQ(3, send0(b, log.slot, "size").smallIntegerValue());
  EXPECT_EQ(b.wk.intern("before"), ocAt(b, log.slot, 1));
  const ao::Oop answer = ocAt(b, log.slot, 2);
  ASSERT_TRUE(answer.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(answer));
  EXPECT_EQ("cannot return", ao::Str::toUtf8(b.heap, answer));
  EXPECT_EQ(b.wk.intern("after"), ocAt(b, log.slot, 3));
  EXPECT_FALSE(b.ctx.nonlocalReturn);
  EXPECT_FALSE(b.ctx.aborting);
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
  std::vector<ao::compiler::CompileError> errs;
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
    "  instanceVariableNames: 'enter exit'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B2-Test'!\n"
    "!R2DeepEnsure methodsFor: 'r'!\n"
    "start\n"
    "  enter := 0.\n"
    "  exit := 0.\n"
    "  ^self recur!\n"
    "recur\n"
    "  ^[enter := enter + 1. self recur] ensure: [self b1]!\n"
    "b1\n"
    "  ^self b2!\n"
    "b2\n"
    "  ^self b3!\n"
    "b3\n"
    "  ^self b4!\n"
    "b4\n"
    "  exit := exit + 1!\n"
    "missing\n"
    "  ^enter - exit! !\n";

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
  EXPECT_EQ(0, send0(b, obj.slot, "missing").smallIntegerValue());
}

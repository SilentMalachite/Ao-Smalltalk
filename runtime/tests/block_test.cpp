#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Interpreter.hpp"
#include "ao/TestRunner.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <system_error>

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

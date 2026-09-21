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

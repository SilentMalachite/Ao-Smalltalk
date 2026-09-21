#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Interpreter.hpp"
#include "ao/TestRunner.hpp"

#include <gtest/gtest.h>

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

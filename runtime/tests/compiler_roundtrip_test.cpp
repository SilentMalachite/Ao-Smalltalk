#include "test_support.hpp"

#include "ao/Bytecode.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/Interpreter.hpp"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

TEST(CompilerRoundtrip, OnePlusTwoIsThree) {
  Boot b;
  auto img = ao::compiler::compileMethod("foo\n  ^1 + 2");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, b.wk.objectClass, nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(3, got.smallIntegerValue());
  EXPECT_GT(b.ctx.interpretedBytecodes, 0u);
}

TEST(CompilerRoundtrip, NativePlusDoesNotInterpret) {
  Boot b;
  auto got = send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  EXPECT_EQ(3, got.smallIntegerValue());
  EXPECT_EQ(0u, b.ctx.interpretedBytecodes);
}

TEST(CompilerRoundtrip, TempAssignAndReturn) {
  Boot b;
  auto img = ao::compiler::compileMethod("bar: x\n  | t |\n  t := x.\n  ^t");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  ao::Oop arg = ao::Oop::fromSmallInteger(11);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), &arg, 1, ao::Oop::nil());
  EXPECT_EQ(11, got.smallIntegerValue());
}

TEST(CompilerRoundtrip, CascadeReturnsReceiver) {
  Boot b;
  auto img = ao::compiler::compileMethod("baz\n  ^1 + 2; yourself");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::fromSmallInteger(1), nullptr, 0,
                                  ao::Oop::nil());
  EXPECT_EQ(1, got.smallIntegerValue());
}

TEST(CompilerRoundtrip, GlobalObject) {
  Boot b;
  auto img = ao::compiler::compileMethod("x\n  ^Object");
  ASSERT_TRUE(img.ok);
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(b.wk.objectClass, got);
}

TEST(CompilerRoundtrip, UnknownPrimitiveFallsThrough) {
  Boot b;
  auto img = ao::compiler::compileMethod("foo\n  <primitive: 1>\n  ^7");
  ASSERT_TRUE(img.ok) << img.error.message;
  auto cm = ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(7, got.smallIntegerValue());
}

TEST(CompilerRoundtrip, HandWrittenJumpFalseSkipsPush) {
  Boot b;
  // PushFalse at pc 0. JumpFalse at pc 1; relative base is opcode+3 = 4.
  // PushTwo is at pc 6, so the int16 offset is 2. Offset 3 assumed JumpFalse at pc 0.
  std::uint8_t code[] = {
      static_cast<std::uint8_t>(ao::compiler::Op::PushFalse),
      static_cast<std::uint8_t>(ao::compiler::Op::JumpFalse), 2, 0,
      static_cast<std::uint8_t>(ao::compiler::Op::PushOne),
      static_cast<std::uint8_t>(ao::compiler::Op::ReturnTop),
      static_cast<std::uint8_t>(ao::compiler::Op::PushTwo),
      static_cast<std::uint8_t>(ao::compiler::Op::ReturnTop),
  };
  auto bytes = b.heap.allocate(b.wk.byteArrayClass, sizeof(code), ao::kFlagBytes);
  ASSERT_TRUE(bytes.isHeap());
  std::memcpy(b.heap.bytes(bytes), code, sizeof(code));
  auto lits = ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0);
  auto cm = ao::CompiledMethod::create(b.ctx, 0, 0, 0, lits, bytes, b.wk.intern("j"),
                                        b.wk.objectClass);
  auto got = ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_EQ(2, got.smallIntegerValue());
}

namespace {

ao::Oop subclassOfObject(Boot& b, const char* name, const char* ivars) {
  auto n = b.wk.intern(name);
  auto iv = ao::Str::fromUtf8(b.heap, b.wk, ivars);
  auto empty = b.wk.intern("");
  ao::Oop args[5] = {n, iv, empty, empty, empty};
  auto sel = ao::Symbol::intern(
      b.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  return ao::send(b.ctx, b.wk.objectClass, sel, args, 5, nullptr);
}

}  // namespace

TEST(CompilerRoundtrip, SuperSendReturnsInstance) {
  Boot b;
  auto sub = subclassOfObject(b, "Sub", "");
  ASSERT_TRUE(sub.isHeap());
  auto img = ao::compiler::compileMethod("yourself\n  ^super yourself");
  ASSERT_TRUE(img.ok) << img.error.message;
  ASSERT_TRUE(ao::installMethod(b.ctx, sub, img.image).isHeap());
  auto obj = send0(b, sub, "new");
  ASSERT_TRUE(obj.isHeap());
  EXPECT_EQ(obj, send0(b, obj, "yourself"));
}

TEST(CompilerRoundtrip, HolderInstVarRoundTrip) {
  Boot b;
  auto holder = subclassOfObject(b, "Holder", "x");
  ASSERT_TRUE(holder.isHeap());
  ao::compiler::CompileEnv env;
  env.instVarNames.emplace_back("x");
  auto setImg = ao::compiler::compileMethod("set: v\n  x := v", env);
  ASSERT_TRUE(setImg.ok) << setImg.error.message;
  auto getImg = ao::compiler::compileMethod("get\n  ^x", env);
  ASSERT_TRUE(getImg.ok) << getImg.error.message;
  ASSERT_TRUE(ao::installMethod(b.ctx, holder, setImg.image).isHeap());
  ASSERT_TRUE(ao::installMethod(b.ctx, holder, getImg.image).isHeap());
  auto obj = send0(b, holder, "new");
  ASSERT_TRUE(obj.isHeap());
  send1(b, obj, "set:", ao::Oop::fromSmallInteger(41));
  EXPECT_EQ(41, send0(b, obj, "get").smallIntegerValue());
}

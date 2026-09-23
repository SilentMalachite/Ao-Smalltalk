#include "ao/Bytecode.hpp"
#include "ao/Compiler.hpp"
#include <gtest/gtest.h>
#include <string>

using ao::compiler::compileMethod;
using ao::compiler::disassemble;
using ao::compiler::LitKind;

TEST(Codegen, ReturnOnePlusTwo) {
  auto r = compileMethod("foo\n  ^1 + 2");
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ("foo", r.image.selector);
  EXPECT_EQ(0, r.image.numArgs);
  EXPECT_EQ(0, r.image.primitive);
  const std::string d = disassemble(r.image);
  EXPECT_EQ(
      "method foo args=0 temps=0 prim=0\n"
      "literals:\n"
      "  PushOne\n"
      "  PushTwo\n"
      "  SendSpecial 0 1\n"
      "  ReturnTop\n",
      d);
}

TEST(Codegen, TempAndAssign) {
  auto r = compileMethod("bar: x\n  | t |\n  t := x.\n  ^t");
  ASSERT_TRUE(r.ok);
  EXPECT_EQ(1, r.image.numArgs);
  EXPECT_EQ(2, r.image.numTemps);
  const std::string d = disassemble(r.image);
  EXPECT_TRUE(d.find("PushTemp 0") != std::string::npos);
  EXPECT_TRUE(d.find("PopStoreTemp 1") != std::string::npos);
  EXPECT_TRUE(d.find("PushTemp 1") != std::string::npos);
}

TEST(Codegen, CascadeAndBlock) {
  auto r = compileMethod("baz\n  ^self foo; bar: 1");
  ASSERT_TRUE(r.ok);
  EXPECT_TRUE(disassemble(r.image).find("Dup") != std::string::npos);
  auto b = compileMethod("qux\n  ^[:a | a + 1]");
  ASSERT_TRUE(b.ok);
  EXPECT_TRUE(disassemble(b.image).find("CreateBlock") != std::string::npos);
}

TEST(Codegen, SuperSendAndGlobal) {
  auto r = compileMethod("hash\n  ^super hash");
  ASSERT_TRUE(r.ok);
  EXPECT_TRUE(disassemble(r.image).find("SendSuper") != std::string::npos);
  auto g = compileMethod("x\n  ^Object");
  ASSERT_TRUE(g.ok);
  EXPECT_TRUE(disassemble(g.image).find("PushGlobal") != std::string::npos);
}

TEST(Codegen, ErrorSpanOnDanglingBinary) {
  auto r = compileMethod("foo\n  ^1 +");
  EXPECT_FALSE(r.ok);
  EXPECT_GT(r.error.span.end, r.error.span.start);
}

TEST(Codegen, LiteralArrayPseudoObjectsAreNotSymbols) {
  auto r = compileMethod("foo\n  ^#(nil true false)");
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(1u, r.image.literals.size());
  EXPECT_EQ(LitKind::Array, r.image.literals[0].kind);
  ASSERT_EQ(3u, r.image.literals[0].elements.size());
  EXPECT_EQ(LitKind::Nil, r.image.literals[0].elements[0].kind);
  EXPECT_EQ(LitKind::True, r.image.literals[0].elements[1].kind);
  EXPECT_EQ(LitKind::False, r.image.literals[0].elements[2].kind);
}

TEST(Codegen, EighteenDigitIntegerSurvivesAsInt) {
  auto r = compileMethod("foo\n  ^100000000000000001");
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(1u, r.image.literals.size());
  EXPECT_EQ(LitKind::Int, r.image.literals[0].kind);
  EXPECT_EQ(100000000000000001LL, r.image.literals[0].intValue);
}

TEST(Codegen, DisassemblesAppendedOps) {
  using ao::compiler::Op;
  ao::compiler::MethodImage image;
  image.selector = "hand";
  image.numTemps = 1;
  image.bytes = {
      static_cast<std::uint8_t>(Op::PushNewArray), 2,
      static_cast<std::uint8_t>(Op::PopStoreTemp), 0,
      static_cast<std::uint8_t>(Op::PushRemoteTemp), 1, 0,
      static_cast<std::uint8_t>(Op::StoreRemoteTemp), 1, 0,
      static_cast<std::uint8_t>(Op::PopStoreRemoteTemp), 0, 0,
      static_cast<std::uint8_t>(Op::PushLitVar), 0,
      static_cast<std::uint8_t>(Op::StoreLitVar), 0,
      static_cast<std::uint8_t>(Op::PopStoreLitVar), 0,
      static_cast<std::uint8_t>(Op::ReturnNil),
  };
  EXPECT_EQ(
      "method hand args=0 temps=1 prim=0\n"
      "literals:\n"
      "  PushNewArray 2\n"
      "  PopStoreTemp 0\n"
      "  PushRemoteTemp 1 0\n"
      "  StoreRemoteTemp 1 0\n"
      "  PopStoreRemoteTemp 0 0\n"
      "  PushLitVar 0\n"
      "  StoreLitVar 0\n"
      "  PopStoreLitVar 0\n"
      "  ReturnNil\n",
      disassemble(image));
}

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

namespace {

const ao::compiler::MethodImage* firstBlock(const ao::compiler::MethodImage& image) {
  for (const auto& lit : image.literals) {
    if (lit.kind == LitKind::Method) {
      return lit.method.get();
    }
  }
  return nullptr;
}

}  // namespace

// SPEC §3.4: 捕捉して代入する temp は temp ベクタに置く。ベクタはメソッドの入口で作る。
TEST(Codegen, BoxedTempUsesRemoteTemp) {
  auto r = compileMethod("foo\n  | x |\n  x := 1.\n  [x := 2] value.\n  ^x");
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(1, r.image.numTemps);
  const std::string d = disassemble(r.image);
  EXPECT_EQ(0u, d.find("method foo args=0 temps=1 prim=0\nliterals: [method]\n  PushNewArray 1\n"
                       "  PopStoreTemp 0\n  PushOne\n  PopStoreRemoteTemp 0 0\n  PushTemp 0\n"
                       "  CreateBlock 0 1\n"))
      << d;
  EXPECT_NE(std::string::npos, d.find("PushRemoteTemp 0 0"));
  const auto* inner = firstBlock(r.image);
  ASSERT_NE(nullptr, inner);
  EXPECT_EQ(1, inner->numTemps);
  EXPECT_NE(std::string::npos, disassemble(*inner).find("StoreRemoteTemp 0 0"));
}

// 代入の無い捕捉は値をコピーする（ベクタを作らない）。
TEST(Codegen, UnwrittenCaptureIsCopied) {
  auto r = compileMethod("foo: x\n  ^[x + 1]");
  ASSERT_TRUE(r.ok) << r.error.message;
  const std::string d = disassemble(r.image);
  EXPECT_EQ(std::string::npos, d.find("PushNewArray"));
  EXPECT_NE(std::string::npos, d.find("PushTemp 0\n  CreateBlock 0 1"));
  const auto* inner = firstBlock(r.image);
  ASSERT_NE(nullptr, inner);
  EXPECT_EQ(1, inner->numTemps);
  EXPECT_NE(std::string::npos, disassemble(*inner).find("PushTemp 0"));
  EXPECT_EQ(std::string::npos, disassemble(*inner).find("RemoteTemp"));
}

// 中間のブロックは、使わなくても内側のためにベクタを受け渡す。
TEST(Codegen, MiddleBlockForwardsVector) {
  auto r = compileMethod("foo\n  | a |\n  ^[[a := 1]]");
  ASSERT_TRUE(r.ok) << r.error.message;
  const auto* middle = firstBlock(r.image);
  ASSERT_NE(nullptr, middle);
  EXPECT_EQ(1, middle->numTemps);
  EXPECT_NE(std::string::npos, disassemble(*middle).find("PushTemp 0\n  CreateBlock 0 1"));
  const auto* inner = firstBlock(*middle);
  ASSERT_NE(nullptr, inner);
  EXPECT_NE(std::string::npos, disassemble(*inner).find("StoreRemoteTemp 0 0"));
}

// SPEC §3.8: 引数（メソッドとブロック）への代入はコンパイルエラー。
TEST(Codegen, ArgumentAssignIsError) {
  auto m = compileMethod("foo: x\n  x := 5");
  EXPECT_FALSE(m.ok);
  EXPECT_EQ("cannot assign to argument", m.error.message);
  auto b = compileMethod("foo\n  ^[:a | a := 1]");
  EXPECT_FALSE(b.ok);
  EXPECT_EQ("cannot assign to argument", b.error.message);
}

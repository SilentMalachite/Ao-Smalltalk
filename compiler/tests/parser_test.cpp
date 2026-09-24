#include "ao/Parser.hpp"
#include <gtest/gtest.h>

using ao::compiler::parseMethod;
using ao::compiler::Ast;

TEST(Parser, UnaryBinaryKeywordAndReturn) {
  auto r = parseMethod("foo: a plus: b\n  ^a + b negated");
  ASSERT_TRUE(r.ok);
  EXPECT_EQ("foo:plus:", r.method.name);
  ASSERT_EQ(2u, r.method.params.size());
  EXPECT_EQ("a", r.method.params[0]);
  auto& body = r.method.kids.at(0);
  EXPECT_EQ(Ast::Kind::Return, body.kind);
  auto& plus = body.kids.at(0);
  EXPECT_EQ(Ast::Kind::Send, plus.kind);
  EXPECT_EQ("+", plus.name);
}

TEST(Parser, TempsAndAssign) {
  auto r = parseMethod("bar: x\n  | t |\n  t := x.\n  ^t");
  ASSERT_TRUE(r.ok);
  ASSERT_EQ(1u, r.method.temps.size());
  EXPECT_EQ("t", r.method.temps[0]);
  auto& seq = r.method.kids.at(0);
  EXPECT_EQ(Ast::Kind::Sequence, seq.kind);
  EXPECT_EQ(Ast::Kind::Assign, seq.kids.at(0).kind);
  EXPECT_EQ("t", seq.kids.at(0).name);
}

TEST(Parser, UnderscoreAssignNormalizes) {
  auto r = parseMethod("x\n  | t |\n  t _ 1.\n  ^t");
  ASSERT_TRUE(r.ok);
  EXPECT_EQ(Ast::Kind::Assign, r.method.kids.at(0).kids.at(0).kind);
}

TEST(Parser, ErrorSpanDoesNotTouchOkFlag) {
  auto r = parseMethod("foo\n  ^1 +");
  EXPECT_FALSE(r.ok);
  EXPECT_GT(r.error.span.end, r.error.span.start);
  EXPECT_FALSE(r.error.message.empty());
}

TEST(Parser, Cascade) {
  auto r = parseMethod("foo\n  ^self bar: 1; baz");
  ASSERT_TRUE(r.ok);
  auto& casc = r.method.kids.at(0).kids.at(0);
  EXPECT_EQ(Ast::Kind::Cascade, casc.kind);
  // kids[0] = first full send (receiver + message); kids[1..] = extra Sends with empty receiver
  ASSERT_GE(casc.kids.size(), 2u);
  EXPECT_EQ("bar:", casc.kids[0].name);
  EXPECT_EQ("baz", casc.kids[1].name);
}

TEST(Parser, BlockWithArgs) {
  auto r = parseMethod("foo\n  ^[:a :b | a + b]");
  ASSERT_TRUE(r.ok);
  auto& blk = r.method.kids.at(0).kids.at(0);
  EXPECT_EQ(Ast::Kind::Block, blk.kind);
  ASSERT_EQ(2u, blk.params.size());
  EXPECT_EQ("a", blk.params[0]);
}

TEST(Parser, LiteralArrayAndByteArray) {
  auto r = parseMethod("foo\n  ^#(1 #b 'c' (2))");
  ASSERT_TRUE(r.ok);
  EXPECT_EQ(Ast::Kind::Literal, r.method.kids.at(0).kids.at(0).kind);
  auto r2 = parseMethod("foo\n  ^#[1 2 255]");
  ASSERT_TRUE(r2.ok);
}

TEST(Parser, PrimitivePragma) {
  auto r = parseMethod("at: i\n  <primitive: 60>\n  ^self basicAt: i");
  ASSERT_TRUE(r.ok);
  EXPECT_EQ(Ast::Kind::Primitive, r.method.kids.at(0).kind);
  EXPECT_EQ(60, r.method.kids.at(0).intValue);
}

TEST(Parser, LiteralArrayKeepsSeparateKeywords) {
  auto r = parseMethod("foo\n  ^#(at: put:)");
  ASSERT_TRUE(r.ok);
  auto& arr = r.method.kids.at(0).kids.at(0);
  ASSERT_EQ(2u, arr.kids.size());
  EXPECT_EQ("at:", arr.kids[0].text);
  EXPECT_EQ("put:", arr.kids[1].text);

  auto rPacked = parseMethod("foo\n  ^#(at:put:)");
  ASSERT_TRUE(rPacked.ok);
  auto& packed = rPacked.method.kids.at(0).kids.at(0);
  ASSERT_EQ(2u, packed.kids.size());
  EXPECT_EQ("at:", packed.kids[0].text);
  EXPECT_EQ("put:", packed.kids[1].text);

  auto r2 = parseMethod("foo\n  ^#(#at:put:)");
  ASSERT_TRUE(r2.ok);
  auto& arr2 = r2.method.kids.at(0).kids.at(0);
  ASSERT_EQ(1u, arr2.kids.size());
  EXPECT_EQ("at:put:", arr2.kids[0].text);
}

TEST(Parser, LiteralArrayPseudoObjectsAreNotSymbols) {
  auto r = parseMethod("foo\n  ^#(nil true false)");
  ASSERT_TRUE(r.ok);
  auto& arr = r.method.kids.at(0).kids.at(0);
  ASSERT_EQ(3u, arr.kids.size());
  EXPECT_EQ(Ast::Kind::Literal, arr.kids[0].kind);
  EXPECT_EQ(Ast::Kind::Literal, arr.kids[1].kind);
  EXPECT_EQ(Ast::Kind::Literal, arr.kids[2].kind);
  EXPECT_EQ("nil", arr.kids[0].name);
  EXPECT_EQ("true", arr.kids[1].name);
  EXPECT_EQ("false", arr.kids[2].name);
  EXPECT_NE("#", arr.kids[0].name);
  EXPECT_NE("#", arr.kids[1].name);
  EXPECT_NE("#", arr.kids[2].name);
}

TEST(Parser, TrueExpressionIsVariable) {
  auto r = parseMethod("foo\n  ^true");
  ASSERT_TRUE(r.ok);
  auto& v = r.method.kids.at(0).kids.at(0);
  EXPECT_EQ(Ast::Kind::Variable, v.kind);
  EXPECT_EQ("true", v.name);
}

// SPEC §3.8: `,` is a binary selector, as a send and as a method pattern.
TEST(Parser, CommaIsABinarySelector) {
  auto r = parseMethod(", other\n  ^'a' , other , #(1 2) , #,");
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(",", r.method.name);
  ASSERT_EQ(1u, r.method.params.size());
  EXPECT_EQ("other", r.method.params[0]);
  auto& last = r.method.kids.at(0).kids.at(0);
  EXPECT_EQ(Ast::Kind::Send, last.kind);
  EXPECT_EQ(",", last.name);
  ASSERT_EQ(2u, last.kids.size());
  EXPECT_EQ("#", last.kids[1].name);
  EXPECT_EQ(",", last.kids[1].text);
  EXPECT_EQ(",", last.kids[0].name);
}

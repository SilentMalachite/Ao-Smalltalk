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

// SPEC §3.8: a negative literal outside int64 keeps its digits; a byte array element or a
// primitive number outside int64 is refused instead of read as 0.
TEST(Parser, IntegerBeyondInt64) {
  auto r = parseMethod("foo\n  ^-100000000000000000000");
  ASSERT_TRUE(r.ok) << r.error.message;
  auto& lit = r.method.kids.at(0).kids.at(0);
  EXPECT_EQ(Ast::Kind::Literal, lit.kind);
  EXPECT_EQ("-100000000000000000000", lit.largeInt);
  auto arr = parseMethod("foo\n  ^#(-16r1FFFFFFFFFFFFFFFFFFFF)");
  ASSERT_TRUE(arr.ok) << arr.error.message;
  EXPECT_EQ("-16r1FFFFFFFFFFFFFFFFFFFF", arr.method.kids.at(0).kids.at(0).kids.at(0).largeInt);
  auto bytes = parseMethod("foo\n  ^#[1 99999999999999999999 3]");
  EXPECT_FALSE(bytes.ok);
  EXPECT_EQ("expected byte 0-255", bytes.error.message);
  EXPECT_EQ(11u, bytes.error.span.start);
  auto prim = parseMethod("foo\n  <primitive: 18446744073709551616>\n  ^1");
  EXPECT_FALSE(prim.ok);
  EXPECT_EQ("expected primitive number", prim.error.message);
}

// SPEC §3.8: an exponent that would make the Integer too large is a compile error at the number.
TEST(Parser, IntegerExponentTooLarge) {
  auto r = parseMethod("foo\n  ^1e65537");
  EXPECT_FALSE(r.ok);
  EXPECT_EQ("number too large", r.error.message);
  EXPECT_EQ(7u, r.error.span.start);
  EXPECT_EQ(14u, r.error.span.end);
  auto bad = parseMethod("foo\n  ^'abc");
  EXPECT_FALSE(bad.ok);
  EXPECT_EQ("invalid token", bad.error.message);
}

// SPEC §3.8: after block arguments the `|` is required. A temp declaration may follow it, and
// `||` is the separator and the opening of the temps. The body after the separator is not read
// as temps: `[:a :b | a | b]` has the body `a | b`.
TEST(Parser, BlockArgumentsThenTemps) {
  for (const char* src : {"foo\n  ^[:x | | t | t := x. t]", "foo\n  ^[:x || t | t := x. t]"}) {
    auto r = parseMethod(src);
    ASSERT_TRUE(r.ok) << src << ": " << r.error.message;
    auto& blk = r.method.kids.at(0).kids.at(0);
    ASSERT_EQ(Ast::Kind::Block, blk.kind) << src;
    ASSERT_EQ(1u, blk.params.size()) << src;
    ASSERT_EQ(1u, blk.temps.size()) << src;
    EXPECT_EQ("t", blk.temps[0]) << src;
    EXPECT_EQ(2u, blk.kids.at(0).kids.size()) << src;
  }
  for (const char* src : {"foo\n  ^[:a :b | a | b]", "foo\n  ^[:a | a | false]"}) {
    auto r = parseMethod(src);
    ASSERT_TRUE(r.ok) << src << ": " << r.error.message;
    auto& blk = r.method.kids.at(0).kids.at(0);
    EXPECT_TRUE(blk.temps.empty()) << src;
    auto& body = blk.kids.at(0);
    ASSERT_EQ(1u, body.kids.size()) << src;
    EXPECT_EQ(Ast::Kind::Send, body.kids[0].kind) << src;
    EXPECT_EQ("|", body.kids[0].name) << src;
  }
  auto empty = parseMethod("foo\n  ^[:a | ]");
  ASSERT_TRUE(empty.ok) << empty.error.message;
  EXPECT_TRUE(empty.method.kids.at(0).kids.at(0).kids.at(0).kids.empty());
  auto noArgs = parseMethod("foo\n  ^[ | t | t ]");
  ASSERT_TRUE(noArgs.ok) << noArgs.error.message;
  EXPECT_EQ(1u, noArgs.method.kids.at(0).kids.at(0).temps.size());
  auto missing = parseMethod("foo\n  ^[:a]");
  EXPECT_FALSE(missing.ok);
  EXPECT_EQ("expected '|'", missing.error.message);
  EXPECT_EQ(10u, missing.error.span.start);
  EXPECT_EQ(11u, missing.error.span.end);
}

// SPEC §3.8: a cascade part is a unary → binary → keyword chain. Its first message has no
// receiver child (it goes to the cascade receiver); the next ones take the previous message as
// their receiver. With the receiver super, the first message of every part is a super send.
TEST(Parser, CascadePartsAreMessageChains) {
  auto r = parseMethod("foo: x\n  ^x add: 3; yourself size; + 1 * 2; at: 1 put: 2 + 3");
  ASSERT_TRUE(r.ok) << r.error.message;
  auto& casc = r.method.kids.at(0).kids.at(0);
  ASSERT_EQ(Ast::Kind::Cascade, casc.kind);
  ASSERT_EQ(4u, casc.kids.size());
  EXPECT_EQ("add:", casc.kids[0].name);
  auto& unary = casc.kids[1];
  EXPECT_EQ("size", unary.name);
  ASSERT_EQ(1u, unary.kids.size());
  EXPECT_EQ("yourself", unary.kids[0].name);
  EXPECT_TRUE(unary.kids[0].kids.empty());
  auto& binary = casc.kids[2];
  EXPECT_EQ("*", binary.name);
  ASSERT_EQ(2u, binary.kids.size());
  EXPECT_EQ("+", binary.kids[0].name);
  EXPECT_EQ(1u, binary.kids[0].kids.size());
  auto& keyword = casc.kids[3];
  EXPECT_EQ("at:put:", keyword.name);
  ASSERT_EQ(2u, keyword.kids.size());
  EXPECT_EQ("+", keyword.kids[1].name);

  auto s = parseMethod("who\n  ^super who; who; yourself who");
  ASSERT_TRUE(s.ok) << s.error.message;
  auto& sc = s.method.kids.at(0).kids.at(0);
  ASSERT_EQ(3u, sc.kids.size());
  EXPECT_TRUE(sc.kids[0].isSuper);
  EXPECT_TRUE(sc.kids[1].isSuper);
  EXPECT_FALSE(sc.kids[2].isSuper);
  EXPECT_TRUE(sc.kids[2].kids.at(0).isSuper);
}

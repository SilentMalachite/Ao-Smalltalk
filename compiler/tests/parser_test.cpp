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

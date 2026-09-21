#include "ao/Scanner.hpp"
#include <gtest/gtest.h>

using ao::compiler::Scanner;
using ao::compiler::Tok;

TEST(Scanner, NumberStringSymbolKeywordBinary) {
  Scanner s("foo: 42 'a''b' #bar #+ 3.14");
  auto k = s.next();
  EXPECT_EQ(Tok::Keyword, k.kind);
  EXPECT_EQ("foo:", k.text);
  auto n = s.next();
  EXPECT_EQ(Tok::Number, n.kind);
  EXPECT_EQ(42.0, n.number);
  EXPECT_FALSE(n.isFloat);
  auto str = s.next();
  EXPECT_EQ(Tok::String, str.kind);
  EXPECT_EQ("a'b", str.text);
  auto sym = s.next();
  EXPECT_EQ(Tok::Symbol, sym.kind);
  EXPECT_EQ("bar", sym.text);
  auto bin = s.next();
  EXPECT_EQ(Tok::Symbol, bin.kind);
  EXPECT_EQ("+", bin.text);
  auto fl = s.next();
  EXPECT_EQ(Tok::Number, fl.kind);
  EXPECT_TRUE(fl.isFloat);
}

TEST(Scanner, AssignVariantsAndComment) {
  Scanner s("x := y _ z. \"cmt\" $あ");
  EXPECT_EQ(Tok::Ident, s.next().kind);
  EXPECT_EQ(Tok::Assign, s.next().kind);
  EXPECT_EQ(Tok::Ident, s.next().kind);
  EXPECT_EQ(Tok::Assign, s.next().kind);
  EXPECT_EQ(Tok::Ident, s.next().kind);
  EXPECT_EQ(Tok::Period, s.next().kind);
  auto ch = s.next();
  EXPECT_EQ(Tok::Character, ch.kind);
  EXPECT_EQ("あ", ch.text);
  EXPECT_EQ(Tok::Eof, s.next().kind);
}

TEST(Scanner, ArrayAndByteArrayHeaders) {
  Scanner s("#(1) #[2]");
  EXPECT_EQ(Tok::HashLParen, s.next().kind);
  EXPECT_EQ(Tok::Number, s.next().kind);
  EXPECT_EQ(Tok::RParen, s.next().kind);
  EXPECT_EQ(Tok::HashLBracket, s.next().kind);
}

TEST(Scanner, UnterminatedStringHasSpan) {
  Scanner s("'abc");
  auto t = s.next();
  EXPECT_EQ(Tok::Error, t.kind);
  EXPECT_EQ(0u, t.span.start);
  EXPECT_EQ(4u, t.span.end);
}

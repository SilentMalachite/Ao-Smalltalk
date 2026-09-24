#include "ao/Scanner.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <string>

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

TEST(Scanner, EighteenDigitIntegerIsExactInt64) {
  Scanner s("100000000000000001");
  auto n = s.next();
  EXPECT_EQ(Tok::Number, n.kind);
  EXPECT_FALSE(n.isFloat);
  EXPECT_EQ(100000000000000001LL, n.intValue);
  EXPECT_NE(100000000000000001LL, static_cast<std::int64_t>(n.number));
}

// SPEC §3.8: `,` is a binary selector character, in a send and in a symbol.
TEST(Scanner, CommaIsABinaryCharacter) {
  Scanner s("'a' , 'b' #, x,y");
  EXPECT_EQ(Tok::String, s.next().kind);
  auto comma = s.next();
  EXPECT_EQ(Tok::Binary, comma.kind);
  EXPECT_EQ(",", comma.text);
  EXPECT_EQ(Tok::String, s.next().kind);
  auto sym = s.next();
  EXPECT_EQ(Tok::Symbol, sym.kind);
  EXPECT_EQ(",", sym.text);
  EXPECT_EQ(Tok::Ident, s.next().kind);
  auto tight = s.next();
  EXPECT_EQ(Tok::Binary, tight.kind);
  EXPECT_EQ(",", tight.text);
  EXPECT_EQ(Tok::Ident, s.next().kind);
  EXPECT_EQ(Tok::Eof, s.next().kind);
}

// SPEC §3.8: a `-` after the first binary character is left out of the selector when a digit
// follows; it starts a negative literal.
TEST(Scanner, MinusBeforeDigitEndsBinarySelector) {
  auto binaries = [](const char* src) {
    Scanner s(src);
    std::string seen;
    for (auto t = s.next(); t.kind != Tok::Eof; t = s.next()) {
      if (t.kind == Tok::Error) {
        return std::string("error");
      }
      seen += t.kind == Tok::Binary ? "[" + t.text + "]" : t.text;
    }
    return seen;
  };
  EXPECT_EQ("2[*][-]1", binaries("2*-1"));
  EXPECT_EQ("3[@][-]2", binaries("3@-2"));
  EXPECT_EQ("3[>][-]1", binaries("3>-1"));
  EXPECT_EQ("3[-][-]1", binaries("3--1"));
  EXPECT_EQ("3[-]1", binaries("3-1"));
  EXPECT_EQ("x[-]y", binaries("x-y"));
  EXPECT_EQ("x[--]y", binaries("x--y"));
  EXPECT_EQ("2[*-]1", binaries("2*- 1"));
  EXPECT_EQ("a[->]b", binaries("a->b"));
}

// SPEC §3.8: an integer has no digit limit. One outside int64 keeps its digits (and radix) in
// largeInt instead of becoming 0; one inside keeps intValue.
TEST(Scanner, IntegerBeyondInt64KeepsItsDigits) {
  auto scan = [](const char* src) {
    Scanner s(src);
    auto t = s.next();
    EXPECT_EQ(Tok::Number, t.kind) << src;
    EXPECT_FALSE(t.isFloat) << src;
    EXPECT_EQ(Tok::Eof, s.next().kind) << src;
    return t;
  };
  auto big = scan("100000000000000000000");
  EXPECT_EQ("100000000000000000000", big.largeInt);
  EXPECT_EQ(0, big.intValue);
  EXPECT_EQ("9223372036854775808", scan("9223372036854775808").largeInt);
  EXPECT_EQ("16r1FFFFFFFFFFFFFFFFFFFF", scan("16r1FFFFFFFFFFFFFFFFFFFF").largeInt);
  EXPECT_EQ("16r1FFFFFFFFFFFFFFFFFFFF", scan("16r1ffffffffffffffffffff").largeInt);
  EXPECT_EQ("100000000000000000000", scan("000100000000000000000000").largeInt);
  auto max = scan("9223372036854775807");
  EXPECT_TRUE(max.largeInt.empty());
  EXPECT_EQ(INT64_MAX, max.intValue);
  auto padded = scan("0000000000000000000000000001");
  EXPECT_TRUE(padded.largeInt.empty());
  EXPECT_EQ(1, padded.intValue);
}

#include "ao/Scanner.hpp"
#include <cmath>
#include <cstdint>
#include <gtest/gtest.h>
#include <limits>
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

// SPEC §3.8: a Float is the correctly rounded value of mantissa × radix^exponent. It used to add
// the fraction digit by digit (0.7 was 0.7000000000000001) and to scale a radix by 10^exponent.
TEST(Scanner, FloatIsCorrectlyRounded) {
  auto value = [](const char* src) {
    Scanner s(src);
    auto t = s.next();
    EXPECT_EQ(Tok::Number, t.kind) << src;
    EXPECT_TRUE(t.isFloat) << src;
    EXPECT_EQ(Tok::Eof, s.next().kind) << src;
    return t.number;
  };
  EXPECT_EQ(0.7, value("0.7"));
  EXPECT_EQ(0.1, value("0.1"));
  EXPECT_EQ(0.3, value("0.3"));
  EXPECT_EQ(2.675, value("2.675"));
  EXPECT_EQ(123456789.123456789, value("123456789.123456789"));
  EXPECT_EQ(0.7, value("10r0.7"));
  EXPECT_EQ(150.0, value("1.5e2"));
  EXPECT_EQ(150.0, value("1.5d2"));
  EXPECT_EQ(0.0015, value("1.5e-3"));
  EXPECT_EQ(std::numeric_limits<double>::max(), value("1.7976931348623157e308"));
  EXPECT_EQ(std::numeric_limits<double>::min(), value("2.2250738585072014e-308"));
  EXPECT_EQ(std::numeric_limits<double>::denorm_min(), value("4.9406564584124654e-324"));
  EXPECT_EQ(std::numeric_limits<double>::infinity(), value("1.0e400"));
  EXPECT_EQ(std::numeric_limits<double>::infinity(), value("1.0e99999999999999999999"));
  EXPECT_EQ(0.0, value("1.0e-400"));
  EXPECT_EQ(0.0, value("0.0e500"));
  EXPECT_EQ(1.5, value("16r1.8"));
  EXPECT_EQ(0.5, value("2r0.1"));
  EXPECT_EQ(1.0 / 3.0, value("3r0.1"));
  EXPECT_EQ(0.5, value("36r0.I"));
  EXPECT_EQ(0.5, value("3r0.1111111111111111111111111111111111111111"));
  EXPECT_EQ(6.0, value("2r1.1e2"));
  EXPECT_EQ(0.25, value("2r1.0e-2"));
  EXPECT_EQ(1.0, value("2r1.00000000000000000000000000000000000000000000000000001"));
  EXPECT_EQ(1.0 + 0x1p-52,
            value("2r1.000000000000000000000000000000000000000000000000000011"));
  EXPECT_EQ(std::numeric_limits<double>::denorm_min(), value("2r1.0e-1074"));
  EXPECT_EQ(std::numeric_limits<double>::denorm_min(), value("2r1.1e-1075"));
  EXPECT_EQ(0.0, value("2r1.0e-1075"));
  EXPECT_EQ(std::numeric_limits<double>::infinity(), value("2r1.0e1024"));
  EXPECT_EQ(std::numeric_limits<double>::max(),
            value("2r1.1111111111111111111111111111111111111111111111111111e1023"));
  EXPECT_EQ(std::numeric_limits<double>::infinity(), value("2r1.0e99999999999999999999"));
  EXPECT_EQ(0.0, value("2r1.0e-99999999999999999999"));
}

// SPEC §3.8: an integer mantissa with an exponent of 0 or more is the Integer
// mantissa × radix^exponent (`1e3` was the Float 1000.0, `2r1e4` 10000.0, `0e500` infinity).
// Above 65536 the exponent is the compile error "number too large" unless the mantissa is 0.
TEST(Scanner, IntegerMantissaWithExponentIsInteger) {
  auto scan = [](const char* src) {
    Scanner s(src);
    auto t = s.next();
    EXPECT_EQ(Tok::Eof, s.next().kind) << src;
    return t;
  };
  auto intValue = [&](const char* src) {
    auto t = scan(src);
    EXPECT_EQ(Tok::Number, t.kind) << src;
    EXPECT_FALSE(t.isFloat) << src;
    EXPECT_TRUE(t.largeInt.empty()) << src;
    return t.intValue;
  };
  EXPECT_EQ(1000, intValue("1e3"));
  EXPECT_EQ(1000, intValue("1e+3"));
  EXPECT_EQ(16, intValue("2r1e4"));
  EXPECT_EQ(0, intValue("0e500"));
  EXPECT_EQ(0, intValue("0e99999999999999999999"));
  EXPECT_EQ(7, intValue("7e0"));
  EXPECT_EQ(INT64_MAX, intValue("9223372036854775807e0"));
  auto large = [&](const char* src) {
    auto t = scan(src);
    EXPECT_EQ(Tok::Number, t.kind) << src;
    EXPECT_FALSE(t.isFloat) << src;
    return t.largeInt;
  };
  EXPECT_EQ("1" + std::string(30, '0'), large("1e30"));
  EXPECT_EQ("9223372036854775810", large("922337203685477581e1"));
  EXPECT_EQ("2r1" + std::string(70, '0'), large("2r1e70"));
  EXPECT_EQ("1" + std::string(22, '0'), large("100000000000000000000e2"));
  EXPECT_EQ(65537u, large("1e65536").size());
  for (const char* src : {"1e65537", "1e99999999999999999999", "100000000000000000000e70000"}) {
    auto t = scan(src);
    EXPECT_EQ(Tok::Error, t.kind) << src;
    EXPECT_EQ("number too large", t.text) << src;
    EXPECT_EQ(std::string(src).size(), t.span.end) << src;
  }
  auto fl = scan("1e-3");
  EXPECT_TRUE(fl.isFloat);
  EXPECT_EQ(0.001, fl.number);
  auto radixFl = scan("2r1e-2");
  EXPECT_TRUE(radixFl.isFloat);
  EXPECT_EQ(0.25, radixFl.number);
}

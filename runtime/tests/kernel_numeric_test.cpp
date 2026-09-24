// SPEC §3.6 (B8): 数の混合演算と比較、bitShift: の境界、Boolean、Point、asCharacter、to:do:、
// = と hash の契約。

#include "ao_abi.h"

#include "test_support.hpp"

#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <limits>
#include <string>

#include <gtest/gtest.h>

namespace {

class KernelNumeric : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  void TearDown() override { ao_runtime_shutdown(); }

  // The Print it of src, or "<eval error: reason>" when the evaluation aborts (SPEC §3.3).
  std::string printIt(const std::string& src) {
    char out[512] = {};
    AoSpan err{};
    const int rc = ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT, out,
                           sizeof out, &err);
    if (rc == AO_ERR_EVAL) {
      return std::string("<eval error: ") + err.message + ">";
    }
    if (rc != AO_OK) {
      return "<rc " + std::to_string(rc) + ": " + err.message + ">";
    }
    return out;
  }
};

// The double LargeInteger::ratioToDouble gives for the Integers written num and den.
double ratio(Boot& b, const std::string& num, const std::string& den) {
  ao::Root n(b.roots, ao::LargeInteger::fromText(b.ctx, num));
  ao::Root d(b.roots, ao::LargeInteger::fromText(b.ctx, den));
  double out = -1234.5;
  EXPECT_TRUE(ao::LargeInteger::ratioToDouble(b.heap, b.wk, n.slot, d.slot, &out))
      << num << " / " << den;
  return out;
}

// 2^k in the radix-2 notation LargeInteger::fromText reads.
std::string pow2(int k, bool negative = false) {
  return std::string(negative ? "-" : "") + "2r1" + std::string(static_cast<std::size_t>(k), '0');
}

}  // namespace

// 03 High: Integer をレシーバとする Float との算術は Float を答える（向きによらない）。
TEST_F(KernelNumeric, IntegerAndFloatMixAnswersFloat) {
  EXPECT_EQ("2.5", printIt("1 + 1.5"));
  EXPECT_EQ("-0.5", printIt("1 - 1.5"));
  EXPECT_EQ("3", printIt("2 * 1.5"));
  EXPECT_EQ("true", printIt("(2 * 1.5) class == Float"));
  EXPECT_EQ("2.5", printIt("1.5 + 1"));
  EXPECT_EQ("0.5", printIt("1.5 - 1"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) + 1.0) = 1180591620717411303424.0"));
  EXPECT_EQ("true", printIt("(1.0 + (1 bitShift: 70)) = 1180591620717411303424.0"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) * 0.5) = 590295810358705651712.0"));
  EXPECT_EQ("true", printIt("(((1 bitShift: 70) + 1) - 1.0) class == Float"));
}

// 03 High: Fraction と Float の算術は Float を答える。
TEST_F(KernelNumeric, FractionAndFloatMixAnswersFloat) {
  EXPECT_EQ("1", printIt("(1/2) + 0.5"));
  EXPECT_EQ("true", printIt("((1/2) + 0.5) class == Float"));
  EXPECT_EQ("1", printIt("0.5 + (1/2)"));
  EXPECT_EQ("true", printIt("(0.5 + (1/2)) class == Float"));
  EXPECT_EQ("0.5", printIt("(1/4) * 2.0"));
  EXPECT_EQ("0.5", printIt("0.75 - (1/4)"));
}

// SPEC §3.6: Integer と Fraction は Fraction（正規化。分母が 1 なら Integer）。
TEST_F(KernelNumeric, IntegerAndFractionMixAnswersNormalizedFraction) {
  EXPECT_EQ("true", printIt("(1 + (1/2)) class == Fraction"));
  EXPECT_EQ("3", printIt("(1 + (1/2)) instVarAt: 1"));
  EXPECT_EQ("2", printIt("(1 + (1/2)) instVarAt: 2"));
  EXPECT_EQ("5", printIt("(3 - (1/2)) instVarAt: 1"));
  EXPECT_EQ("3", printIt("((1/2) + 1) instVarAt: 1"));
  EXPECT_EQ("-1", printIt("((1/2) - 1) instVarAt: 1"));
  EXPECT_EQ("1", printIt("2 * (1/2)"));
  EXPECT_EQ("true", printIt("(2 * (1/2)) class == SmallInteger"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) * (1/2)) = (1 bitShift: 69)"));
}

// SPEC §3.6: / も同じ型の規則。0 で割ると Integer と Fraction は中断、Float は IEEE754。
TEST_F(KernelNumeric, DivisionFollowsTheSameTypeRules) {
  EXPECT_EQ("0.5", printIt("1 / 2.0"));
  EXPECT_EQ("2", printIt("(1/2) / 0.25"));
  EXPECT_EQ("true", printIt("((1/2) / 0.25) class == Float"));
  EXPECT_EQ("6", printIt("3 / (1/2)"));
  EXPECT_EQ("true", printIt("(6 / 3) class == SmallInteger"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) / 2.0) = 590295810358705651712.0"));
  EXPECT_EQ("<eval error: division by zero>", printIt("1 / 0"));
  EXPECT_EQ("<eval error: division by zero>", printIt("(1/2) / 0"));
  EXPECT_EQ("inf", printIt("1 / 0.0"));
}

// SPEC §3.6: // \\ quo: rem: とビット演算は Integer どうしだけ。
TEST_F(KernelNumeric, IntegerOnlyOperatorsFailOnFloatsAndFractions) {
  EXPECT_EQ("<eval error: failed: #//>", printIt("7 // 2.0"));
  EXPECT_EQ("<eval error: failed: #\\\\>", printIt("7 \\\\ (1/2)"));
  EXPECT_EQ("<eval error: failed: #quo:>", printIt("7 quo: 2.0"));
  EXPECT_EQ("<eval error: failed: #rem:>", printIt("7 rem: (1/2)"));
  EXPECT_EQ("<eval error: failed: #bitAnd:>", printIt("7 bitAnd: 1.0"));
}

// 03 High: Rectangle の幅は成分の算術を送るので、Float の成分でも答える。
TEST_F(KernelNumeric, RectangleWidthWithFloatOriginIsFloat) {
  EXPECT_EQ("1.5",
            printIt("(Rectangle origin: (Point x: 0.5 y: 0) corner: (Point x: 2 y: 2)) width"));
  EXPECT_EQ("2", printIt("(Rectangle origin: (Point x: 0 y: 0.5) corner: (Point x: 2 y: 5/2)) "
                      "height"));
}

// SPEC §3.6: Integer を Float にするときは最近接偶数丸め。LargeInteger の全域、範囲外は ±inf。
TEST(KernelNumericConvert, IntegerToFloatRoundsHalfToEven) {
  Boot b;
  EXPECT_EQ(std::ldexp(1.0, 53), ratio(b, "9007199254740993", "1"));
  EXPECT_EQ(std::ldexp(1.0, 64), ratio(b, "18446744073709553664", "1"));
  EXPECT_EQ(0x1.0000000000001p+64, ratio(b, "18446744073709553665", "1"));
  EXPECT_EQ(0x1.0000000000002p+64, ratio(b, "18446744073709557760", "1"));
  EXPECT_EQ(-0x1.0000000000002p+64, ratio(b, "-18446744073709557760", "1"));
  EXPECT_EQ(std::ldexp(1.0, 62), ratio(b, "4611686018427387903", "1"));
  EXPECT_EQ(std::ldexp(1.0, 70), ratio(b, pow2(70), "1"));
  // 2^1024 - 2^970（2 進で 1 が 54 個、0 が 970 個）は最大の有限値と 2^1024 のちょうど中間で、
  // 偶数側（2^1024、つまり inf）へ丸める。1 小さければ最大の有限値。
  EXPECT_EQ(std::numeric_limits<double>::infinity(),
            ratio(b, "2r" + std::string(54, '1') + std::string(970, '0'), "1"));
  EXPECT_EQ(std::numeric_limits<double>::max(),
            ratio(b, "2r" + std::string(53, '1') + "0" + std::string(970, '1'), "1"));
  EXPECT_EQ(-std::numeric_limits<double>::infinity(), ratio(b, pow2(5000, true), "1"));
}

// SPEC §3.6: Fraction を Float にするときも 1 回だけ丸める（分子と分母を別々に丸めない）。
TEST(KernelNumericConvert, FractionToFloatRoundsOnceIncludingSubnormals) {
  Boot b;
  EXPECT_EQ(0x1.5555555555555p-2, ratio(b, "1", "3"));
  EXPECT_EQ(-0x1.5555555555555p-2, ratio(b, "-1", "3"));
  EXPECT_EQ(-0x1.5555555555555p-2, ratio(b, "1", "-3"));
  // 分子も分母も double の範囲を超えるが、比は 1 に近い。
  EXPECT_EQ(1.0, ratio(b, "2r1" + std::string(1099, '0') + "1", pow2(1100)));
  EXPECT_EQ(3.3333333333333335, ratio(b, "1" + std::string(399, '0') + "1",
                                      "3" + std::string(399, '0')));
  // 非正規化数: 2^-1074 の倍数への最近接偶数丸め。
  EXPECT_EQ(std::ldexp(1.0, -1074), ratio(b, "1", pow2(1074)));
  EXPECT_EQ(std::ldexp(1.0, -1074), ratio(b, "3", pow2(1076)));
  EXPECT_EQ(0.0, ratio(b, "1", pow2(1075)));
  EXPECT_EQ(std::ldexp(1.0, -1073), ratio(b, "3", pow2(1075)));
  EXPECT_EQ(0.0, ratio(b, "1", pow2(2000)));
  EXPECT_EQ(std::numeric_limits<double>::infinity(), ratio(b, pow2(2000), "3"));
}

namespace {

// Accepts `superclass subclass: #name instanceVariableNames: ivars ...` (SPEC §3.10).
void acceptClass(const char* superclass, const char* name, const char* ivars) {
  const std::string def = std::string(superclass) + " subclass: #" + name +
                          "\n  instanceVariableNames: '" + ivars +
                          "'\n  classVariableNames: ''\n  poolDictionaries: ''\n"
                          "  category: 'B8-Test'\n";
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
}

void acceptMethod(const char* className, const char* source) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method(className, 0, source, &err)) << err.message;
}

}  // namespace

// 03 High: Integer と Float の比較は向きによらず答える。
TEST_F(KernelNumeric, IntegerAndFloatCompareInBothDirections) {
  EXPECT_EQ("true", printIt("1 < 1.5"));
  EXPECT_EQ("true", printIt("1.5 > 1"));
  EXPECT_EQ("true", printIt("2 >= 1.5"));
  EXPECT_EQ("false", printIt("2 <= 1.5"));
  EXPECT_EQ("false", printIt("1 > 1.5"));
  EXPECT_EQ("true", printIt("0.5 < 1"));
  EXPECT_EQ("true", printIt("-0.5 > -1"));
  EXPECT_EQ("true", printIt("1.0 < (1 bitShift: 70)"));
  EXPECT_EQ("true", printIt("(1 bitShift: 70) > 1.0"));
  // 順序は数の値で決め、= は型をまたぐと false のまま。
  EXPECT_EQ("true", printIt("1 <= 1.0"));
  EXPECT_EQ("true", printIt("1 >= 1.0"));
  EXPECT_EQ("false", printIt("1 = 1.0"));
  EXPECT_EQ("false", printIt("1.0 = 1"));
  EXPECT_EQ("true", printIt("(Rectangle origin: (Point x: 0 y: 0) corner: (Point x: 10 y: 10)) "
                            "containsPoint: (Point x: 0.5 y: 0.5)"));
}

// 03 High: 整数と double は、どちらも丸めずに比べる。
TEST_F(KernelNumeric, IntegerAndFloatCompareWithoutRounding) {
  EXPECT_EQ("true", printIt("1152921504606846976.0 < 1152921504606846977"));
  EXPECT_EQ("true", printIt("1152921504606846977 > 1152921504606846976.0"));
  EXPECT_EQ("false", printIt("1152921504606846977 <= 1152921504606846976.0"));
  EXPECT_EQ("true", printIt("9007199254740993 > 9007199254740992.0"));
  // SmallInteger の最大値 2^62 - 1 は、double にすると 2^62 になる。
  EXPECT_EQ("true", printIt("4611686018427387903 < 4611686018427387904.0"));
  EXPECT_EQ("true", printIt("4611686018427387904.0 > 4611686018427387903"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) + 1) > 1180591620717411303424.0"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) - 1) < 1180591620717411303424.0"));
  EXPECT_EQ("true", printIt("(1 bitShift: 70) <= 1180591620717411303424.0"));
  EXPECT_EQ("true", printIt("(1 bitShift: 70) >= 1180591620717411303424.0"));
  EXPECT_EQ("true", printIt("(1 bitShift: 2000) > 1.0e308"));
  EXPECT_EQ("true", printIt("(0 - (1 bitShift: 2000)) < -1.0e308"));
}

// SPEC §3.6: NaN との比較はどれも false。+inf はどの有限の数より大きく、-inf は小さい。
TEST_F(KernelNumeric, NaNComparesFalseAndInfinityOrdersOutside) {
  const std::string nan = "(0.0 / 0.0)";
  for (const std::string sel : {"<", ">", "<=", ">="}) {
    for (const std::string other : {"1.0", "1", "(1 bitShift: 70)", "(1/2)"}) {
      EXPECT_EQ("false", printIt(nan + " " + sel + " " + other)) << sel << " " << other;
      EXPECT_EQ("false", printIt(other + " " + sel + " " + nan)) << other << " " << sel;
    }
  }
  EXPECT_EQ("false", printIt(nan + " = " + nan));
  EXPECT_EQ("false", printIt(nan + " between: 0 and: 1"));
  EXPECT_EQ("false", printIt("1 between: " + nan + " and: 2"));
  EXPECT_EQ("false", printIt("1.0 between: 0 and: " + nan));
  const std::string inf = "(1.0 / 0.0)";
  EXPECT_EQ("true", printIt("(1 bitShift: 5000) < " + inf));
  EXPECT_EQ("true", printIt(inf + " > (1 bitShift: 5000)"));
  EXPECT_EQ("true", printIt("(0 - (1 bitShift: 5000)) > (0.0 - " + inf + ")"));
  EXPECT_EQ("true", printIt(inf + " >= (1/3)"));
  EXPECT_EQ("false", printIt(inf + " <= 4611686018427387903"));
  EXPECT_EQ("true", printIt(inf + " = " + inf));
}

// 03 Medium: Fraction の = は正規化した分子と分母で、< は交差乗算で比べる。
TEST_F(KernelNumeric, FractionEqualsAndOrders) {
  EXPECT_EQ("true", printIt("(1/2) = (1/2)"));
  EXPECT_EQ("true", printIt("(1/2) = (2/4)"));
  EXPECT_EQ("false", printIt("(1/2) = (1/3)"));
  EXPECT_EQ("false", printIt("(1/2) = 0.5"));
  EXPECT_EQ("false", printIt("0.5 = (1/2)"));
  EXPECT_EQ("false", printIt("(1/2) = 1"));
  EXPECT_EQ("true", printIt("(1/2) < (2/3)"));
  EXPECT_EQ("false", printIt("(2/3) < (1/2)"));
  EXPECT_EQ("true", printIt("(2/3) > (1/2)"));
  EXPECT_EQ("true", printIt("(1/2) <= (2/4)"));
  EXPECT_EQ("false", printIt("(1/2) >= (2/3)"));
  EXPECT_EQ("true", printIt("(-1/2) < (1/3)"));
  EXPECT_EQ("true", printIt("(1/2) < 1"));
  EXPECT_EQ("true", printIt("1 > (1/2)"));
  EXPECT_EQ("true", printIt("(3/2) > 1"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) / 3) < (((1 bitShift: 70) + 1) / 3)"));
  EXPECT_EQ("true", printIt("(1 / (1 bitShift: 70)) < (1 / ((1 bitShift: 70) - 1))"));
  EXPECT_EQ("true", printIt("(1 bitShift: 70) > ((1 bitShift: 71) - 1 / 2)"));
  // Float とも丸めずに比べる。0.3333333333333333 は 1/3 より小さい。
  EXPECT_EQ("true", printIt("(1/3) > 0.3333333333333333"));
  EXPECT_EQ("true", printIt("0.3333333333333333 < (1/3)"));
  EXPECT_EQ("true", printIt("(1/2) <= 0.5"));
  EXPECT_EQ("true", printIt("(1/2) >= 0.5"));
}

// 03 Medium: Fraction を Magnitude として使う max: min: between:and: が動く。
TEST_F(KernelNumeric, MagnitudeProtocolWorksForFractionsAndMixedNumbers) {
  EXPECT_EQ("true", printIt("((1/2) max: (2/3)) = (2/3)"));
  EXPECT_EQ("true", printIt("((1/2) min: (2/3)) = (1/2)"));
  EXPECT_EQ("true", printIt("(1/2) between: 0 and: 1"));
  EXPECT_EQ("false", printIt("(3/2) between: 0 and: 1"));
  EXPECT_EQ("true", printIt("1.5 between: 1 and: (3/2)"));
  EXPECT_EQ("4", printIt("3 max: 4"));
  EXPECT_EQ("3", printIt("3 min: 4"));
  EXPECT_EQ("1.5", printIt("1 max: 1.5"));
  EXPECT_EQ("$b", printIt("$a max: $b"));
  EXPECT_EQ("$a", printIt("$a min: $b"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #<>", printIt("3 max: nil"));
}

// SPEC §3.6: 引数が数でなければ < は失敗し、> <= >= は Magnitude の既定で答える。
TEST_F(KernelNumeric, NonNumberArgumentFailsLessThanAndFallsBackForTheOthers) {
  EXPECT_EQ("<eval error: failed: #<>", printIt("1 < nil"));
  EXPECT_EQ("<eval error: failed: #<>", printIt("1.5 < 'a'"));
  EXPECT_EQ("<eval error: failed: #<>", printIt("(1/2) < nil"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #<>", printIt("1 > nil"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #<>", printIt("(1/2) > nil"));
  EXPECT_EQ("<eval error: failed: #<=>", printIt("1 <= nil"));
  EXPECT_EQ("<eval error: failed: #>=>", printIt("1.5 >= nil"));
  // 利用者の Magnitude は、既定の > が送る < で答える。
  acceptClass("Magnitude", "B8Money", "amount");
  acceptMethod("B8Money", "amount: aNumber\n  amount := aNumber\n");
  acceptMethod("B8Money", "< aNumber\n  ^amount < aNumber\n");
  EXPECT_EQ("true", printIt("3 > (B8Money new amount: 2)"));
  EXPECT_EQ("false", printIt("1.5 > (B8Money new amount: 2)"));
}

// 03 Medium: シフト量 -2^63 は符号反転の前に範囲を判定する。-2^24 より小さければ 0 か -1。
TEST_F(KernelNumeric, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne) {
  EXPECT_EQ("0", printIt("5 bitShift: (-1 bitShift: 63)"));
  EXPECT_EQ("-1", printIt("-5 bitShift: (-1 bitShift: 63)"));
  EXPECT_EQ("0", printIt("5 bitShift: -16777217"));
  EXPECT_EQ("-1", printIt("-5 bitShift: -16777217"));
  EXPECT_EQ("0", printIt("(1 bitShift: 100) bitShift: (0 - (1 bitShift: 80))"));
  EXPECT_EQ("-1", printIt("(0 - (1 bitShift: 100)) bitShift: (0 - (1 bitShift: 80))"));
  EXPECT_EQ("0", printIt("0 bitShift: (-1 bitShift: 63)"));
  // 範囲内の右シフトは床（負数は -∞ 側へ）。
  EXPECT_EQ("0", printIt("5 bitShift: -16777216"));
  EXPECT_EQ("-1", printIt("-5 bitShift: -16777216"));
  EXPECT_EQ("-3", printIt("-5 bitShift: -1"));
  EXPECT_EQ("-1", printIt("-1 bitShift: -64"));
  EXPECT_EQ("5", printIt("5 bitShift: 0"));
  EXPECT_EQ("true", printIt("(1 bitShift: 64) = ((1 bitShift: 63) * 2)"));
  EXPECT_EQ("1", printIt("(1 bitShift: 64) bitShift: -64"));
  // 2^24 を超える左シフトは失敗する。
  EXPECT_EQ("<eval error: failed: #bitShift:>", printIt("5 bitShift: 16777217"));
  EXPECT_EQ("<eval error: failed: #bitShift:>", printIt("5 bitShift: (1 bitShift: 63)"));
}

// 03 Low: = が値で比べる数は、hash も値から計算する（SmallInteger を答える）。
TEST_F(KernelNumeric, EqualNumbersHashEqually) {
  EXPECT_EQ("3", printIt("3 hash"));
  EXPECT_EQ("true", printIt("(1 bitShift: 70) hash = (1 bitShift: 70) hash"));
  EXPECT_EQ("true", printIt("(1 bitShift: 70) hash class == SmallInteger"));
  EXPECT_EQ("true", printIt("(0 - (1 bitShift: 70)) hash = (0 - (1 bitShift: 70)) hash"));
  EXPECT_EQ("false", printIt("(1 bitShift: 70) hash = (0 - (1 bitShift: 70)) hash"));
  EXPECT_EQ("true", printIt("1.5 hash = (1.0 + 0.5) hash"));
  EXPECT_EQ("true", printIt("1.5 hash class == SmallInteger"));
  EXPECT_EQ("true", printIt("0.0 = (0.0 * -1)"));
  EXPECT_EQ("true", printIt("0.0 hash = (0.0 * -1) hash"));
  EXPECT_EQ("true", printIt("(1/2) hash = (2/4) hash"));
  EXPECT_EQ("true", printIt("(1/2) hash class == SmallInteger"));
  EXPECT_EQ("true", printIt("((1 bitShift: 70) / 3) hash = ((1 bitShift: 70) / 3) hash"));
}

// 04 Medium: String と Symbol は同じバイト列なら = が true なので、hash も同じ関数で揃える。
TEST_F(KernelNumeric, EqualStringsAndSymbolsHashEqually) {
  EXPECT_EQ("true", printIt("'abc' hash = 'abc' copy hash"));
  EXPECT_EQ("true", printIt("#abc = 'abc'"));
  EXPECT_EQ("true", printIt("#abc hash = 'abc' hash"));
  EXPECT_EQ("true", printIt("'abc' hash = #abc hash"));
  EXPECT_EQ("true", printIt("'' hash = '' copy hash"));
  EXPECT_EQ("true", printIt("'日本語' hash = '日本語' copy hash"));
  EXPECT_EQ("true", printIt("'abc' hash class == SmallInteger"));
  EXPECT_EQ("false", printIt("'abc' hash = 'abd' hash"));
}

// 04 Medium / 03 Low: Array と Point は要素に hash を送って合成する。
TEST_F(KernelNumeric, EqualArraysAndPointsHashEqually) {
  const std::string lit = "#(1 'x' #y 2.5 #(4 'z'))";
  EXPECT_EQ("true", printIt(lit + " = " + lit));
  EXPECT_EQ("true", printIt(lit + " hash = " + lit + " hash"));
  EXPECT_EQ("true", printIt("#() hash = (Array new: 0) hash"));
  EXPECT_EQ("true", printIt("(Array new: 20) hash class == SmallInteger"));
  // 17 番目より後の要素は hash に入らないが、= なら hash も等しいことは変わらない。
  EXPECT_EQ("true", printIt("| a b | a := Array new: 40. b := Array new: 40. "
                            "1 to: 40 do: [:i | a at: i put: i printString. "
                            "b at: i put: i printString]. (a = b) & (a hash = b hash)"));
  EXPECT_EQ("true", printIt("(Point x: 1 y: 2) hash = (Point x: 1 y: 2) hash"));
  EXPECT_EQ("true", printIt("(Point x: 'a' y: (1/2)) = (Point x: 'a' copy y: (2/4))"));
  EXPECT_EQ("true", printIt("(Point x: 'a' y: (1/2)) hash = (Point x: 'a' copy y: (2/4)) hash"));
  EXPECT_EQ("true", printIt("(Point x: 1 y: 2) hash class == SmallInteger"));
}

// SPEC §3.6: 入れ子の上限があるので、自分を要素に持つ Array や Point の hash も止まる。
TEST_F(KernelNumeric, SelfHoldingArrayAndPointHashStop) {
  EXPECT_EQ("true", printIt("| a | a := Array new: 2. a at: 1 put: a; at: 2 put: a. "
                            "a hash class == SmallInteger"));
  EXPECT_EQ("true", printIt("| p | p := Point x: 1 y: 2. p x: p. p hash class == SmallInteger"));
  EXPECT_EQ("true", printIt("| a b | a := Array new: 1. b := Array new: 1. a at: 1 put: b. "
                            "b at: 1 put: a. a hash = a hash"));
  EXPECT_EQ("true", printIt("| a | a := Array new: 16. 1 to: 16 do: [:i | a at: i put: a]. "
                            "a hash class == SmallInteger"));
}

// 04 Medium: 要素の hash は Smalltalk のメソッドでもよい。答えが Integer でなければ失敗し、
// 巻き戻しが始まれば残りの要素には送らない（SPEC §3.4）。
TEST_F(KernelNumeric, ElementHashMayBeASmalltalkMethod) {
  acceptClass("Object", "B8Key", "k");
  acceptMethod("B8Key", "k: v\n  k := v\n");
  acceptMethod("B8Key", "k\n  ^k\n");
  acceptMethod("B8Key", "= other\n  ^(other isKindOf: B8Key) and: [k = other k]\n");
  acceptMethod("B8Key", "hash\n  ^k hash\n");
  EXPECT_EQ("true", printIt("| a b | a := Array new: 2. a at: 1 put: (B8Key new k: 'abc'); "
                            "at: 2 put: 7. b := Array new: 2. "
                            "b at: 1 put: (B8Key new k: 'abc' copy); at: 2 put: 7. "
                            "(a = b) & (a hash = b hash)"));
  EXPECT_EQ("true", printIt("| p q | p := Point x: (B8Key new k: #(1 2)) y: 1. "
                            "q := Point x: (B8Key new k: #(1 2)) y: 1. (p = q) & (p hash = q hash)"));

  acceptClass("Object", "B8BadKey", "");
  acceptMethod("B8BadKey", "hash\n  ^'not an Integer'\n");
  EXPECT_EQ("<eval error: failed: #hash>",
            printIt("| a | a := Array new: 1. a at: 1 put: B8BadKey new. a hash"));
  EXPECT_EQ("<eval error: failed: #hash>", printIt("(Point x: B8BadKey new y: 1) hash"));

  acceptClass("Object", "B8CountKey", "");
  acceptMethod("B8CountKey",
               "hash\n  Smalltalk at: #B8Count put: (Smalltalk at: #B8Count) + 1.\n  ^1\n");
  acceptClass("Object", "B8AbortKey", "");
  acceptMethod("B8AbortKey", "hash\n  ^nil foo\n");
  ASSERT_EQ("0", printIt("Smalltalk at: #B8Count put: 0"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #foo>",
            printIt("| a | a := Array new: 3. a at: 1 put: B8CountKey new; "
                    "at: 2 put: B8AbortKey new; at: 3 put: B8CountKey new. a hash"));
  EXPECT_EQ("1", printIt("Smalltalk at: #B8Count"));
}

// SPEC §3.6: 値から計算する hash はアドレスによらず、イメージを保存して読み直しても同じ。
TEST_F(KernelNumeric, ValueHashesSurviveImageSaveAndLoad) {
  ASSERT_EQ("true", printIt("| a | a := Array new: 6. a at: 1 put: 'abc'; "
                            "at: 2 put: (1 bitShift: 70); at: 3 put: 1.5; at: 4 put: (1/3); "
                            "at: 5 put: (Point x: 1 y: 'p'); at: 6 put: #(1 #(2)). "
                            "Smalltalk at: #B8Saved put: a. true"));
  const std::string before = printIt("(B8Saved collect: [:e | e hash]) printString");
  const std::string whole = printIt("B8Saved hash");
  ASSERT_EQ(std::string::npos, before.find("error")) << before;
  const auto path = std::filesystem::path(testing::TempDir()) / "b8-hashes.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  EXPECT_EQ(before, printIt("(B8Saved collect: [:e | e hash]) printString"));
  EXPECT_EQ(whole, printIt("B8Saved hash"));
  // 読み直したものと、新しく作った等しい値も同じ hash になる。
  EXPECT_EQ(before, printIt("| a | a := Array new: 6. a at: 1 put: 'abc'; "
                            "at: 2 put: (1 bitShift: 70); at: 3 put: 1.5; at: 4 put: (1/3); "
                            "at: 5 put: (Point x: 1 y: 'p'); at: 6 put: #(1 #(2)). "
                            "(a collect: [:e | e hash]) printString"));
  std::filesystem::remove(path);
}

// 03 Low: Boolean の全セレクタ。true & x と false | x は x をそのまま答える（Blue Book）。
TEST_F(KernelNumeric, BooleanOperatorsFollowTheBlueBook) {
  EXPECT_EQ("true", printIt("true & true"));
  EXPECT_EQ("false", printIt("true & false"));
  EXPECT_EQ("nil", printIt("true & nil"));
  EXPECT_EQ("3", printIt("true & 3"));
  EXPECT_EQ("false", printIt("false & true"));
  EXPECT_EQ("false", printIt("false & nil"));
  EXPECT_EQ("true", printIt("true | false"));
  EXPECT_EQ("true", printIt("true | nil"));
  EXPECT_EQ("false", printIt("false | false"));
  EXPECT_EQ("true", printIt("false | true"));
  EXPECT_EQ("3", printIt("false | 3"));
  EXPECT_EQ("nil", printIt("false | nil"));
  EXPECT_EQ("true", printIt("true eqv: true"));
  EXPECT_EQ("false", printIt("true eqv: false"));
  EXPECT_EQ("false", printIt("false eqv: true"));
  EXPECT_EQ("true", printIt("false eqv: false"));
  EXPECT_EQ("false", printIt("true xor: true"));
  EXPECT_EQ("true", printIt("true xor: false"));
  EXPECT_EQ("true", printIt("false xor: true"));
  EXPECT_EQ("false", printIt("false xor: false"));
  EXPECT_EQ("false", printIt("true not"));
  EXPECT_EQ("true", printIt("false not"));
  EXPECT_EQ("3", printIt("true and: [3]"));
  EXPECT_EQ("false", printIt("false and: [3]"));
  EXPECT_EQ("true", printIt("true or: [3]"));
  EXPECT_EQ("4", printIt("false or: [4]"));
  // 送信で呼んでも（インライン展開しなくても）同じ。
  EXPECT_EQ("nil", printIt("true perform: #& with: nil"));
  EXPECT_EQ("3", printIt("false perform: #| with: 3"));
}

// 03 Low: eqv: と xor: は、引数が Boolean でなければ失敗する。
TEST_F(KernelNumeric, EqvAndXorFailOnNonBooleanArguments) {
  EXPECT_EQ("<eval error: failed: #xor:>", printIt("true xor: 3"));
  EXPECT_EQ("<eval error: failed: #xor:>", printIt("false xor: nil"));
  EXPECT_EQ("<eval error: failed: #eqv:>", printIt("true eqv: 3"));
  EXPECT_EQ("<eval error: failed: #eqv:>", printIt("false eqv: nil"));
}

// 03 Low: 成分の計算が失敗したら、Point を作らずに失敗する（空 OOP を Point に入れない）。
TEST_F(KernelNumeric, PointArithmeticFailsOnAFailedComponent) {
  EXPECT_EQ("<eval error: failed: #+>", printIt("(Point x: 1 y: 2) + nil"));
  EXPECT_EQ("<eval error: failed: #*>", printIt("(Point x: 1 y: 2) * 'a'"));
  EXPECT_EQ("<eval error: failed: #->", printIt("(Point x: 1 y: 2) - 'b'"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #->", printIt("(Point x: nil y: 2) - 1"));
  EXPECT_EQ("<eval error: division by zero>", printIt("(Point x: 1 y: 2) // 0"));
  EXPECT_EQ("true", printIt("((Point x: 1 y: 2) + 0.5) = (Point x: 1.5 y: 2.5)"));
}

// 03 Low: Point と Rectangle のネイティブは、サブクラスのインスタンスも同じに扱う。
TEST_F(KernelNumeric, PointAndRectangleSubclassesUseTheNatives) {
  acceptClass("Point", "B8Point", "z");
  acceptClass("Rectangle", "B8Rect", "tag");
  EXPECT_EQ("1", printIt("(B8Point x: 1 y: 2) x"));
  EXPECT_EQ("2", printIt("(B8Point x: 1 y: 2) y"));
  EXPECT_EQ("5", printIt("((B8Point x: 1 y: 2) x: 5) x"));
  EXPECT_EQ("7", printIt("((B8Point x: 1 y: 2) y: 7) y"));
  EXPECT_EQ("4", printIt("((B8Point x: 1 y: 2) + (Point x: 3 y: 4)) x"));
  EXPECT_EQ("6", printIt("((Point x: 3 y: 4) + (B8Point x: 1 y: 2)) y"));
  EXPECT_EQ("true", printIt("((B8Point x: 1 y: 2) * 2) class == Point"));
  EXPECT_EQ("true", printIt("(B8Point x: 1 y: 2) = (Point x: 1 y: 2)"));
  EXPECT_EQ("true", printIt("(Point x: 1 y: 2) = (B8Point x: 1 y: 2)"));
  EXPECT_EQ("true", printIt("(B8Point x: 1 y: 2) hash = (Point x: 1 y: 2) hash"));
  EXPECT_EQ("10", printIt("(B8Rect origin: (Point x: 0 y: 0) corner: (B8Point x: 10 y: 20)) "
                          "width"));
  EXPECT_EQ("20", printIt("(B8Rect origin: (B8Point x: 0 y: 0) corner: (Point x: 10 y: 20)) "
                          "height"));
  EXPECT_EQ("true", printIt("(B8Rect origin: (Point x: 0 y: 0) corner: (Point x: 10 y: 10)) "
                            "origin = (Point x: 0 y: 0)"));
  EXPECT_EQ("true", printIt("(B8Rect origin: (Point x: 0 y: 0) corner: (Point x: 10 y: 10)) "
                            "containsPoint: (B8Point x: 1 y: 1)"));
  EXPECT_EQ("true", printIt("((B8Rect origin: (Point x: 0 y: 0) corner: (Point x: 10 y: 10)) "
                            "intersect: (Rectangle origin: (Point x: 5 y: 5) corner: "
                            "(Point x: 20 y: 20))) origin = (Point x: 5 y: 5)"));
}

// 03 Low: asCharacter は Unicode スカラー値だけを受け付ける（サロゲート、負数、0x10FFFF 超は失敗）。
TEST_F(KernelNumeric, AsCharacterAcceptsOnlyUnicodeScalarValues) {
  EXPECT_EQ("0", printIt("0 asCharacter asInteger"));
  EXPECT_EQ("$A", printIt("65 asCharacter"));
  EXPECT_EQ("55295", printIt("55295 asCharacter asInteger"));
  EXPECT_EQ("57344", printIt("57344 asCharacter asInteger"));
  EXPECT_EQ("1114111", printIt("1114111 asCharacter asInteger"));
  EXPECT_EQ("<eval error: failed: #asCharacter>", printIt("55296 asCharacter"));
  EXPECT_EQ("<eval error: failed: #asCharacter>", printIt("56320 asCharacter"));
  EXPECT_EQ("<eval error: failed: #asCharacter>", printIt("57343 asCharacter"));
  EXPECT_EQ("<eval error: failed: #asCharacter>", printIt("-1 asCharacter"));
  EXPECT_EQ("<eval error: failed: #asCharacter>", printIt("1114112 asCharacter"));
  EXPECT_EQ("<eval error: failed: #asCharacter>", printIt("(1 bitShift: 70) asCharacter"));
}

// 03 Low: to:do: の終端が SmallInteger でなければ、ループ変数に <= を送って終わりを決める。
// インライン展開したループ（SPEC §3.5）とネイティブの to:do:（ブロックが変数）は同じ意味。
TEST_F(KernelNumeric, ToDoWithNonSmallIntegerLimitSendsLessOrEqual) {
  for (const std::string blk : {"[:i | s := s + i]", "b"}) {
    const std::string pre = "| s b | s := 0. b := [:i | s := s + i]. ";
    EXPECT_EQ("3", printIt(pre + "1 to: 2.5 do: " + blk + ". s")) << blk;
    EXPECT_EQ("3", printIt(pre + "1 to: 5/2 do: " + blk + ". s")) << blk;
    EXPECT_EQ("1", printIt(pre + "1 to: 1.0 do: " + blk + ". s")) << blk;
    EXPECT_EQ("0", printIt(pre + "1 to: (0.0 / 0.0) do: " + blk + ". s")) << blk;
    EXPECT_EQ("0", printIt(pre + "(1 bitShift: 70) to: 5 do: " + blk + ". s")) << blk;
    EXPECT_EQ("true", printIt(pre + "(1 bitShift: 70) to: (1 bitShift: 70) + 2 do: " + blk +
                              ". s = (((1 bitShift: 70) * 3) + 3)"))
        << blk;
    EXPECT_EQ("1", printIt(pre + "1 to: 2.5 do: " + blk)) << blk;
    EXPECT_EQ("<eval error: failed: #<=>", printIt(pre + "1 to: nil do: " + blk)) << blk;
  }
  // 巨大な終端でも、途中の ^ で抜けられる。
  EXPECT_EQ("6", printIt("| s | s := 0. 1 to: (1 bitShift: 70) do: [:i | i > 3 ifTrue: [^s]. "
                         "s := s + i]. s"));
  EXPECT_EQ("4", printIt("| b | b := [:i | i > 3 ifTrue: [^i]]. 1 to: (1 bitShift: 70) do: b. nil"));
  // ループ変数は Integer のまま。
  EXPECT_EQ("true", printIt("| c b | b := [:i | c := i class]. 1 to: 1.5 do: b. c == SmallInteger"));
}

// SPEC §3.5: 刻みが Float の to:by:do:（インライン展開）も、Integer と Float の比較と算術で回る。
TEST_F(KernelNumeric, ToByDoWithFloatStepCountsInFloats) {
  EXPECT_EQ("4.5", printIt("| s | s := 0. 1 to: 2 by: 0.5 do: [:i | s := s + i]. s"));
  EXPECT_EQ("4.5", printIt("| s | s := 0. 2 to: 1 by: -0.5 do: [:i | s := s + i]. s"));
  EXPECT_EQ("3", printIt("| s | s := 0. 1 to: 2.5 by: 1 do: [:i | s := s + i]. s"));
}

// B8 レビュー（Codex Medium）: 8 バイトに満たない Float は = で 0.0 と読むので、hash も同じ値から。
TEST_F(KernelNumeric, ShortFloatHashesAsItsEqualsReadsIt) {
  EXPECT_EQ("true", printIt("| a b | a := Float new. b := Float new. a = b"));
  EXPECT_EQ("true", printIt("| a b | a := Float new. b := Float new. (a = b) & (a hash = b hash)"));
  EXPECT_EQ("true", printIt("Float new = 0.0"));
  EXPECT_EQ("true", printIt("Float new hash = 0.0 hash"));
  EXPECT_EQ("true", printIt("(Float basicNew: 4) hash = (0.0 * -1) hash"));
}

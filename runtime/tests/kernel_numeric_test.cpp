// SPEC §3.6 (B8): 数の混合演算と比較、bitShift: の境界、Boolean、Point、asCharacter、to:do:、
// = と hash の契約。

#include "ao_abi.h"

#include "test_support.hpp"

#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"

#include <cmath>
#include <cstring>
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

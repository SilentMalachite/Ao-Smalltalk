#include "test_support.hpp"

#include "ao/Context.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Oop.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

static ao::Oop makeFloat(Boot& b, double v) {
  auto o = b.heap.allocate(b.wk.floatClass, 8, ao::kFlagBytes);
  if (o.isHeap()) {
    std::memcpy(b.heap.bytes(o), &v, sizeof(v));
  }
  return o;
}

static double floatValue(Boot& b, ao::Oop o) {
  double v = 0;
  if (o.isHeap() && b.heap.size(o) >= 8) {
    std::memcpy(&v, b.heap.bytes(o), sizeof(v));
  }
  return v;
}

TEST(SmallIntegerArith, OnePlusTwoIsThree) {
  Boot b;
  auto r = send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(3, r.smallIntegerValue());
}

TEST(SmallIntegerArith, OverflowBecomesLargePositive) {
  Boot b;
  auto max = ao::Oop::fromSmallInteger((std::int64_t{1} << 62) - 1);
  auto r = send1(b, max, "+", ao::Oop::fromSmallInteger(1));
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.largePositiveIntegerClass, b.heap.klass(r));
}

TEST(SmallIntegerArith, ToDoSums) {
  Boot b;
  static std::int64_t sum;
  sum = 0;
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) sum += args[0].smallIntegerValue();
    return args[0];
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  ao::Oop args[2] = {ao::Oop::fromSmallInteger(3), blk};
  auto sel = ao::Symbol::intern(b.wk, "to:do:");
  ao::send(b.ctx, ao::Oop::fromSmallInteger(1), sel, args, 2, nullptr);
  EXPECT_EQ(6, sum);  // 1+2+3
}

TEST(SmallIntegerArith, OverflowBecomesLargeNegative) {
  Boot b;
  auto min = ao::Oop::fromSmallInteger(-(std::int64_t{1} << 62));
  auto r = send1(b, min, "-", ao::Oop::fromSmallInteger(1));
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.largeNegativeIntegerClass, b.heap.klass(r));
}

TEST(SmallIntegerArith, MultiplyOverflowIsLarge) {
  Boot b;
  auto a = ao::Oop::fromSmallInteger(std::int64_t{1} << 31);
  auto r = send1(b, a, "*", a);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.largePositiveIntegerClass, b.heap.klass(r));
  bool fits = false;
  EXPECT_EQ(std::int64_t{1} << 62, ao::LargeInteger::asInt64IfFits(b.heap, b.wk, r, &fits));
  EXPECT_TRUE(fits);
}

TEST(SmallIntegerArith, FloorDivAndModulo) {
  Boot b;
  auto seven = ao::Oop::fromSmallInteger(7);
  auto two = ao::Oop::fromSmallInteger(2);
  auto q = send1(b, seven, "//", two);
  auto m = send1(b, seven, "\\\\", two);
  ASSERT_TRUE(q.isSmallInteger());
  ASSERT_TRUE(m.isSmallInteger());
  EXPECT_EQ(3, q.smallIntegerValue());
  EXPECT_EQ(1, m.smallIntegerValue());

  auto nseven = ao::Oop::fromSmallInteger(-7);
  auto fq = send1(b, nseven, "//", two);
  auto fm = send1(b, nseven, "\\\\", two);
  ASSERT_TRUE(fq.isSmallInteger());
  ASSERT_TRUE(fm.isSmallInteger());
  EXPECT_EQ(-4, fq.smallIntegerValue());
  EXPECT_EQ(1, fm.smallIntegerValue());
}

TEST(SmallIntegerArith, QuoRemTowardZero) {
  Boot b;
  auto nseven = ao::Oop::fromSmallInteger(-7);
  auto two = ao::Oop::fromSmallInteger(2);
  auto q = send1(b, nseven, "quo:", two);
  auto r = send1(b, nseven, "rem:", two);
  ASSERT_TRUE(q.isSmallInteger());
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(-3, q.smallIntegerValue());
  EXPECT_EQ(-1, r.smallIntegerValue());
}

TEST(SmallIntegerArith, DivisionByZeroReturnsErrorString) {
  Boot b;
  auto r = send1(b, ao::Oop::fromSmallInteger(1), "//", ao::Oop::fromSmallInteger(0));
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ("division by zero", ao::Str::toUtf8(b.heap, r));
}

TEST(SmallIntegerArith, BitOpsAndShift) {
  Boot b;
  auto a = ao::Oop::fromSmallInteger(12);
  auto c = ao::Oop::fromSmallInteger(10);
  auto band = send1(b, a, "bitAnd:", c);
  auto bor = send1(b, a, "bitOr:", c);
  auto bxor = send1(b, a, "bitXor:", c);
  ASSERT_TRUE(band.isSmallInteger());
  EXPECT_EQ(8, band.smallIntegerValue());
  EXPECT_EQ(14, bor.smallIntegerValue());
  EXPECT_EQ(6, bxor.smallIntegerValue());
  auto sh = send1(b, ao::Oop::fromSmallInteger(1), "bitShift:", ao::Oop::fromSmallInteger(3));
  ASSERT_TRUE(sh.isSmallInteger());
  EXPECT_EQ(8, sh.smallIntegerValue());
  const auto p31 = ao::Oop::fromSmallInteger(std::int64_t{1} << 31);
  auto same = send1(b, p31, "bitAnd:", p31);
  ASSERT_TRUE(same.isSmallInteger());
  EXPECT_EQ(std::int64_t{1} << 31, same.smallIntegerValue());
  auto noneg = send1(b, ao::Oop::fromSmallInteger(-1), "bitAnd:", p31);
  ASSERT_TRUE(noneg.isSmallInteger());
  EXPECT_EQ(std::int64_t{1} << 31, noneg.smallIntegerValue());
}

TEST(SmallIntegerArith, CompareAndBetween) {
  Boot b;
  auto one = ao::Oop::fromSmallInteger(1);
  auto two = ao::Oop::fromSmallInteger(2);
  EXPECT_TRUE(send1(b, one, "<", two).isTrue());
  EXPECT_TRUE(send1(b, two, ">", one).isTrue());
  EXPECT_TRUE(send1(b, one, "<=", one).isTrue());
  EXPECT_TRUE(send1(b, two, ">=", one).isTrue());
  EXPECT_TRUE(send1(b, one, "=", one).isTrue());
  EXPECT_TRUE(send1(b, one, "=", two).isFalse());
  EXPECT_TRUE(send2(b, ao::Oop::fromSmallInteger(5), "between:and:", one,
                    ao::Oop::fromSmallInteger(10))
                  .isTrue());
  EXPECT_TRUE(send2(b, ao::Oop::fromSmallInteger(0), "between:and:", one,
                    ao::Oop::fromSmallInteger(10))
                  .isFalse());
}

TEST(SmallIntegerArith, ToMakesInterval) {
  Boot b;
  auto iv = send1(b, ao::Oop::fromSmallInteger(1), "to:", ao::Oop::fromSmallInteger(3));
  ASSERT_TRUE(iv.isHeap());
  EXPECT_EQ(b.wk.intervalClass, b.heap.klass(iv));
  EXPECT_EQ(1, b.heap.slotAt(iv, 0).smallIntegerValue());
  EXPECT_EQ(3, b.heap.slotAt(iv, 1).smallIntegerValue());
  EXPECT_EQ(1, b.heap.slotAt(iv, 2).smallIntegerValue());
}

TEST(SmallIntegerArith, CharacterProtocol) {
  Boot b;
  auto a = ao::Oop::fromCharacter(U'A');
  auto av = send0(b, a, "asciiValue");
  ASSERT_TRUE(av.isSmallInteger());
  EXPECT_EQ(65, av.smallIntegerValue());
  EXPECT_EQ(65, send0(b, a, "asInteger").smallIntegerValue());
  EXPECT_TRUE(send1(b, a, "=", ao::Oop::fromCharacter(U'A')).isTrue());
  EXPECT_TRUE(send1(b, a, "<", ao::Oop::fromCharacter(U'B')).isTrue());
  auto ch = send0(b, ao::Oop::fromSmallInteger(65), "asCharacter");
  ASSERT_TRUE(ch.isCharacter());
  EXPECT_EQ(U'A', ch.characterValue());
  EXPECT_EQ(a, send0(b, a, "asCharacter"));
}

TEST(SmallIntegerArith, FloatArithmetic) {
  Boot b;
  // Float の結果は allocateRetry で作るので、send をまたぐ値はルートしておく。
  ao::Root x(b.roots, makeFloat(b, 1.5));
  ao::Root y(b.roots, makeFloat(b, 2.5));
  auto sum = send1(b, x.slot, "+", y.slot);
  ASSERT_TRUE(sum.isHeap());
  EXPECT_EQ(b.wk.floatClass, b.heap.klass(sum));
  EXPECT_DOUBLE_EQ(4.0, floatValue(b, sum));
  EXPECT_TRUE(send1(b, x.slot, "<", y.slot).isTrue());
  EXPECT_TRUE(send1(b, x.slot, "=", makeFloat(b, 1.5)).isTrue());
}

TEST(SmallIntegerArith, FloatEqualsDoesNotCoerceInteger) {
  Boot b;
  auto one = makeFloat(b, 1.0);
  auto i = ao::Oop::fromSmallInteger(1);
  EXPECT_TRUE(send1(b, one, "=", i).isFalse());
  EXPECT_TRUE(send1(b, i, "=", one).isFalse());
  EXPECT_TRUE(send1(b, one, "=", makeFloat(b, 1.0)).isTrue());
}

static void expectNormalizedLarge(ao::Heap& heap, ao::Oop o) {
  ASSERT_TRUE(o.isHeap());
  ASSERT_EQ(0u, heap.size(o) % 4);
  ASSERT_GE(heap.size(o), 4u);
  std::uint32_t hi = 0;
  std::memcpy(&hi, heap.bytes(o) + heap.size(o) - 4, 4);
  EXPECT_NE(0u, hi);
}

TEST(SmallIntegerArith, SchoolbookAddMulBeyondInt64) {
  Boot b;
  // LargeInteger の箱詰めは allocateRetry を通るので、send をまたぐ値はルートしておく。
  ao::Root two63(b.roots, send1(b, ao::Oop::fromSmallInteger(1), "bitShift:",
                                ao::Oop::fromSmallInteger(63)));
  ASSERT_TRUE(ao::LargeInteger::isLarge(b.wk, two63.slot));
  bool fits = true;
  ao::LargeInteger::asInt64IfFits(b.heap, b.wk, two63.slot, &fits);
  EXPECT_FALSE(fits);
  expectNormalizedLarge(b.heap, two63.slot);

  ao::Root sum(b.roots, send1(b, two63.slot, "+", two63.slot));
  ASSERT_TRUE(ao::LargeInteger::isLarge(b.wk, sum.slot));
  expectNormalizedLarge(b.heap, sum.slot);
  bool sumFits = true;
  ao::LargeInteger::asInt64IfFits(b.heap, b.wk, sum.slot, &sumFits);
  EXPECT_FALSE(sumFits);
  auto two64 = send1(b, ao::Oop::fromSmallInteger(1), "bitShift:", ao::Oop::fromSmallInteger(64));
  EXPECT_TRUE(send1(b, sum.slot, "=", two64).isTrue());

  ao::Root prod(b.roots, send1(b, two63.slot, "*", two63.slot));
  ASSERT_TRUE(ao::LargeInteger::isLarge(b.wk, prod.slot));
  expectNormalizedLarge(b.heap, prod.slot);
  auto two126 = send1(b, ao::Oop::fromSmallInteger(1), "bitShift:", ao::Oop::fromSmallInteger(126));
  EXPECT_TRUE(send1(b, prod.slot, "=", two126).isTrue());

  auto neg = send1(b, ao::Oop::fromSmallInteger(0), "-", two63.slot);
  auto z = send1(b, two63.slot, "+", neg);
  ASSERT_TRUE(z.isSmallInteger());
  EXPECT_EQ(0, z.smallIntegerValue());
}

TEST(SmallIntegerArith, OverflowAfterNurseryCollectKeepsClass) {
  Boot b;
  while (true) {
    auto junk = b.heap.allocate(b.wk.arrayClass, 64, 0);
    if (!junk.isHeap()) {
      break;
    }
  }
  auto max = ao::Oop::fromSmallInteger((std::int64_t{1} << 62) - 1);
  auto r = send1(b, max, "+", ao::Oop::fromSmallInteger(1));
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.largePositiveIntegerClass, b.heap.klass(r));
}

TEST(SmallIntegerArith, LargeReceiverAddViaIntegerMethod) {
  Boot b;
  auto max = ao::Oop::fromSmallInteger((std::int64_t{1} << 62) - 1);
  ao::Root large(b.roots, send1(b, max, "+", ao::Oop::fromSmallInteger(1)));
  ASSERT_TRUE(large.slot.isHeap());
  auto r = send1(b, large.slot, "+", ao::Oop::fromSmallInteger(1));
  bool fits = false;
  EXPECT_EQ((std::int64_t{1} << 62) + 1, ao::LargeInteger::asInt64IfFits(b.heap, b.wk, r, &fits));
  EXPECT_TRUE(fits);
  EXPECT_TRUE(send1(b, large.slot, ">", max).isTrue());
}

TEST(SmallIntegerArith, FractionMulDiv) {
  Boot b;
  auto half = send1(b, ao::Oop::fromSmallInteger(1), "/", ao::Oop::fromSmallInteger(2));
  auto two = send1(b, half, "*", ao::Oop::fromSmallInteger(4));
  ASSERT_TRUE(two.isSmallInteger());
  EXPECT_EQ(2, two.smallIntegerValue());
  auto one = send1(b, half, "/", half);
  ASSERT_TRUE(one.isSmallInteger());
  EXPECT_EQ(1, one.smallIntegerValue());
}

TEST(SmallIntegerArith, FractionNormalizesToInteger) {
  Boot b;
  auto half = send1(b, ao::Oop::fromSmallInteger(1), "/", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(half.isHeap());
  EXPECT_EQ(b.wk.fractionClass, b.heap.klass(half));
  EXPECT_EQ(1, b.heap.slotAt(half, 0).smallIntegerValue());
  EXPECT_EQ(2, b.heap.slotAt(half, 1).smallIntegerValue());
  auto one = send1(b, half, "+", half);
  ASSERT_TRUE(one.isSmallInteger());
  EXPECT_EQ(1, one.smallIntegerValue());
  auto two = send1(b, ao::Oop::fromSmallInteger(4), "/", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(two.isSmallInteger());
  EXPECT_EQ(2, two.smallIntegerValue());
}

TEST(LargeIntegerApi, FromInt64AndAdd) {
  Boot b;
  auto smi = ao::LargeInteger::fromInt64(b.heap, b.wk, 3);
  ASSERT_TRUE(smi.isSmallInteger());
  EXPECT_FALSE(ao::LargeInteger::isLarge(b.wk, smi));
  const auto mag = std::int64_t{1} << 62;
  ao::Root large(b.roots, ao::LargeInteger::fromInt64(b.heap, b.wk, mag));
  ASSERT_TRUE(ao::LargeInteger::isLarge(b.wk, large.slot));
  EXPECT_EQ(b.wk.largePositiveIntegerClass, b.heap.klass(large.slot));
  bool fits = false;
  EXPECT_EQ(mag, ao::LargeInteger::asInt64IfFits(b.heap, b.wk, large.slot, &fits));
  EXPECT_TRUE(fits);
  ao::Root sum(b.roots, ao::LargeInteger::add(b.ctx, large.slot, ao::Oop::fromSmallInteger(1)));
  bool sumFits = true;
  ao::LargeInteger::asInt64IfFits(b.heap, b.wk, sum.slot, &sumFits);
  EXPECT_TRUE(sum.slot.isHeap());
  auto back = ao::LargeInteger::sub(b.ctx, sum.slot, large.slot);
  ASSERT_TRUE(back.isSmallInteger());
  EXPECT_EQ(1, back.smallIntegerValue());
  auto prod = ao::LargeInteger::mul(b.ctx, large.slot, ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(prod.isHeap());
}

TEST(GcNursery, RemainingAndSafepoint) {
  ao::Heap heap(512, 4096);
  ao::Roots roots;
  ao::Gc gc(heap, roots);
  const auto cap = heap.nurseryCapacity();
  EXPECT_EQ(cap, heap.nurseryRemaining());
  auto o = heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(o.isHeap());
  EXPECT_LT(heap.nurseryRemaining(), cap);
  while (heap.nurseryRemaining() >= cap / 8) {
    auto x = heap.allocate(ao::Oop::nil(), 1, 0);
    if (!x.isHeap()) {
      break;
    }
  }
  ASSERT_LT(heap.nurseryRemaining(), cap / 8);
  gc.safepoint();
  EXPECT_EQ(cap, heap.nurseryRemaining());
}

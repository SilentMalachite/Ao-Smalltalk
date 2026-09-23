#include "ao/Oop.hpp"

#include <cstdint>
#include <gtest/gtest.h>

TEST(OopTag, SmallIntegerRoundTrip) {
  auto z = ao::Oop::fromSmallInteger(0);
  auto a = ao::Oop::fromSmallInteger(1);
  auto n = ao::Oop::fromSmallInteger(-1);
  auto max = ao::Oop::fromSmallInteger((std::int64_t{1} << 62) - 1);
  auto min = ao::Oop::fromSmallInteger(-(std::int64_t{1} << 62));
  ASSERT_TRUE(z.isSmallInteger());
  EXPECT_EQ(0, z.smallIntegerValue());
  EXPECT_EQ(1, a.smallIntegerValue());
  EXPECT_EQ(-1, n.smallIntegerValue());
  EXPECT_EQ((std::int64_t{1} << 62) - 1, max.smallIntegerValue());
  EXPECT_EQ(-(std::int64_t{1} << 62), min.smallIntegerValue());
  EXPECT_FALSE(z.isHeap());
  EXPECT_FALSE(z.isCharacter());
  EXPECT_FALSE(z.isImmediate());
}

TEST(OopTag, CharacterRoundTrip) {
  auto a = ao::Oop::fromCharacter(U'A');
  auto z = ao::Oop::fromCharacter(0);
  auto max = ao::Oop::fromCharacter(0x10FFFF);
  ASSERT_TRUE(a.isCharacter());
  EXPECT_EQ(U'A', a.characterValue());
  EXPECT_EQ(0u, z.characterValue());
  EXPECT_EQ(0x10FFFFu, max.characterValue());
  EXPECT_FALSE(a.isSmallInteger());
  EXPECT_FALSE(a.isHeap());
}

TEST(OopTag, ImmediateThreePatterns) {
  auto n = ao::Oop::nil();
  auto t = ao::Oop::true_();
  auto f = ao::Oop::false_();
  EXPECT_TRUE(n.isNil());
  EXPECT_TRUE(n.isImmediate());
  EXPECT_TRUE(t.isTrue());
  EXPECT_TRUE(f.isFalse());
  EXPECT_NE(n, t);
  EXPECT_NE(t, f);
  EXPECT_NE(n, f);
  EXPECT_FALSE(n.isHeap());
  EXPECT_FALSE(n.isSmallInteger());
  EXPECT_EQ(ao::Oop::nil(), n);
}

TEST(OopTag, HeapAlignedPointerRoundTrip) {
  alignas(8) unsigned char buf[16];
  auto o = ao::Oop::fromHeap(buf);
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(static_cast<void*>(buf), o.heapPointer());
  EXPECT_FALSE(o.isSmallInteger());
  EXPECT_FALSE(o.isCharacter());
  EXPECT_FALSE(o.isImmediate());
  EXPECT_TRUE(ao::Oop::fromHeap(nullptr).isEmpty());
  EXPECT_FALSE(ao::Oop::fromHeap(nullptr).isHeap());
}

TEST(OopTag, IdentityEqualsIsBits) {
  EXPECT_EQ(ao::Oop::fromSmallInteger(3), ao::Oop::fromSmallInteger(3));
  EXPECT_NE(ao::Oop::fromSmallInteger(3), ao::Oop::fromSmallInteger(4));
}

// 01 テストの穴: 範囲外の値を黙って折り返さず、debug ビルドの assert で止まる。
TEST(OopTag, FromSmallIntegerOutOfRangeDies) {
#ifdef NDEBUG
  GTEST_SKIP() << "assert is compiled out under NDEBUG";
#else
  EXPECT_EQ(ao::kSmiMax, ao::Oop::fromSmallInteger(ao::kSmiMax).smallIntegerValue());
  EXPECT_EQ(ao::kSmiMin, ao::Oop::fromSmallInteger(ao::kSmiMin).smallIntegerValue());
  EXPECT_DEATH((void)ao::Oop::fromSmallInteger(ao::kSmiMax + 1), "out of range");
  EXPECT_DEATH((void)ao::Oop::fromSmallInteger(ao::kSmiMin - 1), "out of range");
  EXPECT_DEATH((void)ao::Oop::fromSmallInteger(INT64_MAX), "out of range");
#endif
}

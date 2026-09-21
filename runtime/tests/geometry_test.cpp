#include "test_support.hpp"

#include <gtest/gtest.h>

static ao::Oop pt(Boot& b, std::int64_t x, std::int64_t y) {
  ao::Oop args[2] = {ao::Oop::fromSmallInteger(x), ao::Oop::fromSmallInteger(y)};
  return ao::send(b.ctx, b.wk.pointClass, ao::Symbol::intern(b.wk, "x:y:"), args, 2, nullptr);
}

static ao::Oop rect(Boot& b, ao::Oop origin, ao::Oop corner) {
  return send2(b, b.wk.rectangleClass, "origin:corner:", origin, corner);
}

TEST(Geometry, PointAdd) {
  Boot b;
  ao::Oop a1[2] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2)};
  auto p1 = ao::send(b.ctx, b.wk.pointClass, ao::Symbol::intern(b.wk, "x:y:"), a1, 2, nullptr);
  ao::Oop a2[2] = {ao::Oop::fromSmallInteger(3), ao::Oop::fromSmallInteger(4)};
  auto p2 = ao::send(b.ctx, b.wk.pointClass, ao::Symbol::intern(b.wk, "x:y:"), a2, 2, nullptr);
  auto s = send1(b, p1, "+", p2);
  EXPECT_EQ(4, send0(b, s, "x").smallIntegerValue());
  EXPECT_EQ(6, send0(b, s, "y").smallIntegerValue());
}

TEST(Geometry, PointAccessorsEqualsAndSetters) {
  Boot b;
  auto p = pt(b, 1, 2);
  ASSERT_TRUE(p.isHeap());
  EXPECT_EQ(b.wk.pointClass, b.heap.klass(p));
  auto x = send0(b, p, "x");
  auto y = send0(b, p, "y");
  ASSERT_TRUE(x.isSmallInteger());
  ASSERT_TRUE(y.isSmallInteger());
  EXPECT_EQ(1, x.smallIntegerValue());
  EXPECT_EQ(2, y.smallIntegerValue());

  EXPECT_EQ(p, send1(b, p, "x:", ao::Oop::fromSmallInteger(8)));
  EXPECT_EQ(p, send1(b, p, "y:", ao::Oop::fromSmallInteger(9)));
  EXPECT_EQ(8, send0(b, p, "x").smallIntegerValue());
  EXPECT_EQ(9, send0(b, p, "y").smallIntegerValue());

  auto same = pt(b, 8, 9);
  auto other = pt(b, 8, 0);
  EXPECT_TRUE(send1(b, p, "=", same).isTrue());
  EXPECT_TRUE(send1(b, p, "=", other).isFalse());
  EXPECT_TRUE(send1(b, p, "=", ao::Oop::fromSmallInteger(8)).isFalse());
}

TEST(Geometry, PointSubtractScaleIntDivideAndPlusNumber) {
  Boot b;
  auto p = pt(b, 5, 7);
  auto d = send1(b, p, "-", pt(b, 1, 2));
  ASSERT_TRUE(d.isHeap());
  EXPECT_EQ(b.wk.pointClass, b.heap.klass(d));
  EXPECT_EQ(4, send0(b, d, "x").smallIntegerValue());
  EXPECT_EQ(5, send0(b, d, "y").smallIntegerValue());

  auto scaled = send1(b, pt(b, 2, 3), "*", ao::Oop::fromSmallInteger(2));
  EXPECT_EQ(4, send0(b, scaled, "x").smallIntegerValue());
  EXPECT_EQ(6, send0(b, scaled, "y").smallIntegerValue());

  auto hadamard = send1(b, pt(b, 2, 3), "*", pt(b, 3, 4));
  EXPECT_EQ(6, send0(b, hadamard, "x").smallIntegerValue());
  EXPECT_EQ(12, send0(b, hadamard, "y").smallIntegerValue());

  auto q = send1(b, pt(b, 5, 7), "//", ao::Oop::fromSmallInteger(2));
  EXPECT_EQ(2, send0(b, q, "x").smallIntegerValue());
  EXPECT_EQ(3, send0(b, q, "y").smallIntegerValue());

  auto plusN = send1(b, pt(b, 1, 2), "+", ao::Oop::fromSmallInteger(3));
  EXPECT_EQ(4, send0(b, plusN, "x").smallIntegerValue());
  EXPECT_EQ(5, send0(b, plusN, "y").smallIntegerValue());
}

TEST(Geometry, RectangleWidthHeightContainsAndIntersect) {
  Boot b;
  auto origin = pt(b, 0, 0);
  auto corner = pt(b, 10, 10);
  auto r = rect(b, origin, corner);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.rectangleClass, b.heap.klass(r));
  EXPECT_EQ(origin, send0(b, r, "origin"));
  EXPECT_EQ(corner, send0(b, r, "corner"));

  auto w = send0(b, r, "width");
  auto h = send0(b, r, "height");
  ASSERT_TRUE(w.isSmallInteger());
  ASSERT_TRUE(h.isSmallInteger());
  EXPECT_EQ(10, w.smallIntegerValue());
  EXPECT_EQ(10, h.smallIntegerValue());

  EXPECT_TRUE(send1(b, r, "containsPoint:", pt(b, 0, 0)).isTrue());
  EXPECT_TRUE(send1(b, r, "containsPoint:", pt(b, 9, 9)).isTrue());
  EXPECT_TRUE(send1(b, r, "containsPoint:", pt(b, 10, 10)).isFalse());
  EXPECT_TRUE(send1(b, r, "containsPoint:", pt(b, -1, 0)).isFalse());
  EXPECT_TRUE(send1(b, r, "containsPoint:", pt(b, 0, 10)).isFalse());

  auto other = rect(b, pt(b, 5, 5), pt(b, 15, 15));
  auto inter = send1(b, r, "intersect:", other);
  ASSERT_TRUE(inter.isHeap());
  EXPECT_EQ(b.wk.rectangleClass, b.heap.klass(inter));
  EXPECT_EQ(5, send0(b, send0(b, inter, "origin"), "x").smallIntegerValue());
  EXPECT_EQ(5, send0(b, send0(b, inter, "origin"), "y").smallIntegerValue());
  EXPECT_EQ(10, send0(b, send0(b, inter, "corner"), "x").smallIntegerValue());
  EXPECT_EQ(10, send0(b, send0(b, inter, "corner"), "y").smallIntegerValue());
}

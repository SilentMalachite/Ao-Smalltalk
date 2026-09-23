#include "test_support.hpp"

#include "ao/HandleScope.hpp"

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
  // Point の生成は GC しうるので、send をまたぐ値はルートしておく。
  ao::Root p1(b.roots,
              ao::send(b.ctx, b.wk.pointClass, ao::Symbol::intern(b.wk, "x:y:"), a1, 2, nullptr));
  ao::Oop a2[2] = {ao::Oop::fromSmallInteger(3), ao::Oop::fromSmallInteger(4)};
  ao::Root p2(b.roots,
              ao::send(b.ctx, b.wk.pointClass, ao::Symbol::intern(b.wk, "x:y:"), a2, 2, nullptr));
  ao::Root s(b.roots, send1(b, p1.slot, "+", p2.slot));
  EXPECT_EQ(4, send0(b, s.slot, "x").smallIntegerValue());
  EXPECT_EQ(6, send0(b, s.slot, "y").smallIntegerValue());
}

TEST(Geometry, PointAccessorsEqualsAndSetters) {
  Boot b;
  // Point の生成は GC しうるので、send をまたぐ値はルートしておく。
  ao::Root p(b.roots, pt(b, 1, 2));
  ASSERT_TRUE(p.slot.isHeap());
  EXPECT_EQ(b.wk.pointClass, b.heap.klass(p.slot));
  auto x = send0(b, p.slot, "x");
  auto y = send0(b, p.slot, "y");
  ASSERT_TRUE(x.isSmallInteger());
  ASSERT_TRUE(y.isSmallInteger());
  EXPECT_EQ(1, x.smallIntegerValue());
  EXPECT_EQ(2, y.smallIntegerValue());

  EXPECT_EQ(p.slot, send1(b, p.slot, "x:", ao::Oop::fromSmallInteger(8)));
  EXPECT_EQ(p.slot, send1(b, p.slot, "y:", ao::Oop::fromSmallInteger(9)));
  EXPECT_EQ(8, send0(b, p.slot, "x").smallIntegerValue());
  EXPECT_EQ(9, send0(b, p.slot, "y").smallIntegerValue());

  ao::Root same(b.roots, pt(b, 8, 9));
  ao::Root other(b.roots, pt(b, 8, 0));
  EXPECT_TRUE(send1(b, p.slot, "=", same.slot).isTrue());
  EXPECT_TRUE(send1(b, p.slot, "=", other.slot).isFalse());
  EXPECT_TRUE(send1(b, p.slot, "=", ao::Oop::fromSmallInteger(8)).isFalse());
}

TEST(Geometry, PointSubtractScaleIntDivideAndPlusNumber) {
  Boot b;
  // Point の生成は GC しうるので、受け手と引数は先に作ってルートしておく（引数の評価順は不定）。
  ao::Root p(b.roots, pt(b, 5, 7));
  ao::Root arg(b.roots, pt(b, 1, 2));
  ao::Root d(b.roots, send1(b, p.slot, "-", arg.slot));
  ASSERT_TRUE(d.slot.isHeap());
  EXPECT_EQ(b.wk.pointClass, b.heap.klass(d.slot));
  EXPECT_EQ(4, send0(b, d.slot, "x").smallIntegerValue());
  EXPECT_EQ(5, send0(b, d.slot, "y").smallIntegerValue());

  ao::Root scaled(b.roots, send1(b, pt(b, 2, 3), "*", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(4, send0(b, scaled.slot, "x").smallIntegerValue());
  EXPECT_EQ(6, send0(b, scaled.slot, "y").smallIntegerValue());

  p.slot = pt(b, 2, 3);
  arg.slot = pt(b, 3, 4);
  ao::Root hadamard(b.roots, send1(b, p.slot, "*", arg.slot));
  EXPECT_EQ(6, send0(b, hadamard.slot, "x").smallIntegerValue());
  EXPECT_EQ(12, send0(b, hadamard.slot, "y").smallIntegerValue());

  ao::Root q(b.roots, send1(b, pt(b, 5, 7), "//", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(2, send0(b, q.slot, "x").smallIntegerValue());
  EXPECT_EQ(3, send0(b, q.slot, "y").smallIntegerValue());

  ao::Root plusN(b.roots, send1(b, pt(b, 1, 2), "+", ao::Oop::fromSmallInteger(3)));
  EXPECT_EQ(4, send0(b, plusN.slot, "x").smallIntegerValue());
  EXPECT_EQ(5, send0(b, plusN.slot, "y").smallIntegerValue());
}

TEST(Geometry, RectangleWidthHeightContainsAndIntersect) {
  Boot b;
  // Point と Rectangle の生成は GC しうるので、send をまたぐ値と引数は先に作ってルートしておく。
  ao::Root origin(b.roots, pt(b, 0, 0));
  ao::Root corner(b.roots, pt(b, 10, 10));
  ao::Root r(b.roots, rect(b, origin.slot, corner.slot));
  ASSERT_TRUE(r.slot.isHeap());
  EXPECT_EQ(b.wk.rectangleClass, b.heap.klass(r.slot));
  EXPECT_EQ(origin.slot, send0(b, r.slot, "origin"));
  EXPECT_EQ(corner.slot, send0(b, r.slot, "corner"));

  auto w = send0(b, r.slot, "width");
  auto h = send0(b, r.slot, "height");
  ASSERT_TRUE(w.isSmallInteger());
  ASSERT_TRUE(h.isSmallInteger());
  EXPECT_EQ(10, w.smallIntegerValue());
  EXPECT_EQ(10, h.smallIntegerValue());

  ao::Root probe(b.roots, pt(b, 0, 0));
  EXPECT_TRUE(send1(b, r.slot, "containsPoint:", probe.slot).isTrue());
  probe.slot = pt(b, 9, 9);
  EXPECT_TRUE(send1(b, r.slot, "containsPoint:", probe.slot).isTrue());
  probe.slot = pt(b, 10, 10);
  EXPECT_TRUE(send1(b, r.slot, "containsPoint:", probe.slot).isFalse());
  probe.slot = pt(b, -1, 0);
  EXPECT_TRUE(send1(b, r.slot, "containsPoint:", probe.slot).isFalse());
  probe.slot = pt(b, 0, 10);
  EXPECT_TRUE(send1(b, r.slot, "containsPoint:", probe.slot).isFalse());

  origin.slot = pt(b, 5, 5);
  corner.slot = pt(b, 15, 15);
  ao::Root other(b.roots, rect(b, origin.slot, corner.slot));
  ao::Root inter(b.roots, send1(b, r.slot, "intersect:", other.slot));
  ASSERT_TRUE(inter.slot.isHeap());
  EXPECT_EQ(b.wk.rectangleClass, b.heap.klass(inter.slot));
  EXPECT_EQ(5, send0(b, send0(b, inter.slot, "origin"), "x").smallIntegerValue());
  EXPECT_EQ(5, send0(b, send0(b, inter.slot, "origin"), "y").smallIntegerValue());
  EXPECT_EQ(10, send0(b, send0(b, inter.slot, "corner"), "x").smallIntegerValue());
  EXPECT_EQ(10, send0(b, send0(b, inter.slot, "corner"), "y").smallIntegerValue());
}

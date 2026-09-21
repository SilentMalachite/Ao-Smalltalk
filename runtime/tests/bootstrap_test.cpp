#include "ao/Heap.hpp"
#include "ao/Roots.hpp"
#include "ao/WellKnown.hpp"

#include <gtest/gtest.h>

TEST(Bootstrap, WellKnownImmediatesMatchOopTags) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  EXPECT_EQ(ao::Oop::nil(), wk.nil());
  EXPECT_EQ(ao::Oop::true_(), wk.true_());
  EXPECT_EQ(ao::Oop::false_(), wk.false_());
  EXPECT_TRUE(wk.nil().isNil());
  EXPECT_TRUE(wk.true_().isTrue());
  EXPECT_TRUE(wk.false_().isFalse());
}

TEST(Bootstrap, WellKnownNamedImmediates) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  EXPECT_EQ(ao::Oop::nil(), wk.named("nil"));
  EXPECT_EQ(ao::Oop::true_(), wk.named("true"));
  EXPECT_EQ(ao::Oop::false_(), wk.named("false"));
  EXPECT_TRUE(wk.named("Object").isNil());
  EXPECT_TRUE(wk.named("nope").isNil());
}

TEST(Bootstrap, ImmediateClassSlotsReserved) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  EXPECT_TRUE(wk.undefinedObjectClass.isNil());
  EXPECT_TRUE(wk.trueClass.isNil());
  EXPECT_TRUE(wk.falseClass.isNil());
  EXPECT_TRUE(wk.smallIntegerClass.isNil());
  EXPECT_TRUE(wk.characterClass.isNil());
  EXPECT_TRUE(wk.classOf(ao::Oop::nil()).isNil());
}

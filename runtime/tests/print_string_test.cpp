#include "test_support.hpp"

#include "ao/LargeInteger.hpp"
#include "ao/Symbol.hpp"

#include <cstring>
#include <gtest/gtest.h>
#include <string>

static std::string printed(Boot& b, ao::Oop receiver) {
  return ao::Str::toUtf8(b.heap, send0(b, receiver, "printString"));
}

TEST(PrintString, SmallIntegerPrintsDecimalWithoutSeparators) {
  Boot b;
  EXPECT_EQ("3", printed(b, ao::Oop::fromSmallInteger(3)));
}

TEST(PrintString, NegativeSmallIntegerPrintsLeadingMinus) {
  Boot b;
  EXPECT_EQ("-2", printed(b, ao::Oop::fromSmallInteger(-2)));
}

TEST(PrintString, NilPrintsNil) {
  Boot b;
  EXPECT_EQ("nil", printed(b, ao::Oop::nil()));
}

TEST(PrintString, TruePrintsTrue) {
  Boot b;
  EXPECT_EQ("true", printed(b, ao::Oop::true_()));
}

TEST(PrintString, FalsePrintsFalse) {
  Boot b;
  EXPECT_EQ("false", printed(b, ao::Oop::false_()));
}

TEST(PrintString, PrintableCharacterPrintsDollarAndByte) {
  Boot b;
  EXPECT_EQ("$A", printed(b, ao::Oop::fromCharacter(U'A')));
}

TEST(PrintString, NonPrintableCharacterPrintsClassName) {
  Boot b;
  EXPECT_EQ("Character", printed(b, ao::Oop::fromCharacter(10)));
}

TEST(PrintString, StringPrintDoublesEmbeddedQuote) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "a'b");
  ASSERT_TRUE(s.isHeap());
  EXPECT_EQ("'a''b'", printed(b, s));
}

TEST(PrintString, ArrayPrintsElementPrintStrings) {
  Boot b;
  ao::Oop slots[2] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2)};
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, slots, 2);
  ASSERT_TRUE(arr.isHeap());
  EXPECT_EQ("#(1 2)", printed(b, arr));
}

TEST(PrintString, EmptyArrayPrintsEmptyLiteral) {
  Boot b;
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0);
  ASSERT_TRUE(arr.isHeap());
  EXPECT_EQ("#()", printed(b, arr));
}

TEST(PrintString, NestedArrayPastDepthFourPrintsEllipsis) {
  Boot b;
  // Outermost array is depth 1. The fifth nested array prints "..." and is not sent printString.
  ao::Oop inner = ao::Oop::fromSmallInteger(1);
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, &inner, 1);
  ASSERT_TRUE(arr.isHeap());
  for (int i = 0; i < 4; ++i) {
    arr = ao::Arr::fromSlots(b.heap, b.wk, &arr, 1);
    ASSERT_TRUE(arr.isHeap());
  }
  EXPECT_EQ("#(#(#(#(...))))", printed(b, arr));
}

TEST(PrintString, FloatOnePrintStringContainsOne) {
  Boot b;
  auto f = b.heap.allocate(b.wk.floatClass, 8, ao::kFlagBytes);
  ASSERT_TRUE(f.isHeap());
  const double one = 1.0;
  std::memcpy(b.heap.bytes(f), &one, sizeof(one));
  EXPECT_NE(std::string::npos, printed(b, f).find('1'));
}

TEST(PrintString, ObjectNewPrintStringStartsWithObject) {
  Boot b;
  auto obj = send0(b, b.wk.objectClass, "new");
  ASSERT_TRUE(obj.isHeap());
  EXPECT_TRUE(printed(b, obj).starts_with("Object"));
}

TEST(PrintString, LargeIntegerPrintsClassName) {
  Boot b;
  auto big = ao::LargeInteger::fromInt64(b.heap, b.wk, ao::kSmiMax + 1);
  ASSERT_TRUE(big.isHeap());
  EXPECT_EQ(b.wk.largePositiveIntegerClass, b.heap.klass(big));
  EXPECT_EQ("LargePositiveInteger", printed(b, big));
}

TEST(PrintString, SymbolPrintsClassName) {
  Boot b;
  auto sym = ao::Symbol::intern(b.wk, "a'b");
  ASSERT_TRUE(sym.isHeap());
  EXPECT_EQ("Symbol", printed(b, sym));
}

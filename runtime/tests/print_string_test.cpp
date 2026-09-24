#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Symbol.hpp"

#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

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

// What `receiver <sel> (WriteStream on: String new)` put on the stream, or "<abort: reason>".
// Every value that crosses a send is rooted: the sends may collect (GC stress too).
static std::string printedOn(Boot& b, ao::Oop receiver, const char* sel = "printOn:") {
  ao::Root rcvr(b.roots, receiver);
  ao::Root empty(b.roots, ao::Str::fromUtf8(b.heap, b.wk, ""));
  ao::Root stream(b.roots, send1(b, b.wk.named("WriteStream"), "on:", empty.slot));
  if (!stream.slot.isHeap()) {
    return "<no stream>";
  }
  send1(b, rcvr.slot, sel, stream.slot);
  if (b.ctx.aborting) {
    return "<abort: " + takeAbortReason(b) + ">";
  }
  return ao::Str::toUtf8(b.heap, send0(b, stream.slot, "contents"));
}

// 00 High / SPEC §3.10 printString: printOn: は #printString を送り、その答えを書く。
// ネイティブで上書きした printString（SmallInteger の十進など）がそのまま出る。クラス名ではない。
TEST(PrintString, PrintOnPutsTheReceiversPrintString) {
  Boot b;
  EXPECT_EQ("3", printedOn(b, ao::Oop::fromSmallInteger(3)));
  EXPECT_EQ("-2", printedOn(b, ao::Oop::fromSmallInteger(-2)));
  EXPECT_EQ("nil", printedOn(b, ao::Oop::nil()));
  EXPECT_EQ("true", printedOn(b, ao::Oop::true_()));
  EXPECT_EQ("$a", printedOn(b, ao::Oop::fromCharacter(U'a')));
  EXPECT_EQ("'a''b'", printedOn(b, ao::Str::fromUtf8(b.heap, b.wk, "a'b")));
  ao::Oop slots[2] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2)};
  EXPECT_EQ("#(1 2)", printedOn(b, ao::Arr::fromSlots(b.heap, b.wk, slots, 2)));
  EXPECT_EQ("Object", printedOn(b, send0(b, b.wk.objectClass, "new")));
  // storeOn: は printOn: と同じ。
  EXPECT_EQ("3", printedOn(b, ao::Oop::fromSmallInteger(3), "storeOn:"));
}

// 00 High: ユーザーが printString を上書きすると printOn: にも出る。printString の abort は
// その理由のまま伝わり、ストリームには何も書かない（SPEC §3.3、§3.4）。
TEST(PrintString, PrintOnSendsAnOverriddenPrintString) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  const auto acts = ao::compiler::parseChunks(
      "Object subclass: #B5Printed\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B5-Test'!\n"
      "Object subclass: #B5PrintFails\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B5-Test'!\n",
      errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs)) << (errs.empty() ? "" : errs[0].message);
  ao::compiler::CompileError err;
  ASSERT_TRUE(ao::acceptMethodSource(b.ctx, "B5Printed", false, "printString\n  ^'custom'\n", &err))
      << err.message;
  ASSERT_TRUE(ao::acceptMethodSource(b.ctx, "B5PrintFails", false,
                                     "printString\n  ^self error: 'boom'\n", &err))
      << err.message;
  EXPECT_EQ("custom", printedOn(b, send0(b, b.wk.named("B5Printed"), "new")));
  EXPECT_EQ("custom", printedOn(b, send0(b, b.wk.named("B5Printed"), "new"), "storeOn:"));

  ao::Root empty(b.roots, ao::Str::fromUtf8(b.heap, b.wk, ""));
  ao::Root stream(b.roots, send1(b, b.wk.named("WriteStream"), "on:", empty.slot));
  ASSERT_TRUE(stream.slot.isHeap());
  ao::Root failing(b.roots, send0(b, b.wk.named("B5PrintFails"), "new"));
  EXPECT_TRUE(send1(b, failing.slot, "printOn:", stream.slot).isEmpty());
  EXPECT_EQ("boom", takeAbortReason(b));
  EXPECT_EQ("", ao::Str::toUtf8(b.heap, send0(b, stream.slot, "contents")));
}

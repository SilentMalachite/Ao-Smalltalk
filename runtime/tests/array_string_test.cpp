#include "test_support.hpp"

#include "ao/HandleScope.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <string_view>

TEST(ArrayString, AtPutAndSize) {
  Boot b;
  auto a = send1(b, b.wk.arrayClass, "new:", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(a.isHeap());
  EXPECT_EQ(2, send0(b, a, "size").smallIntegerValue());
  ao::Oop put[2] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(9)};
  ao::send(b.ctx, a, ao::Symbol::intern(b.wk, "at:put:"), put, 2, nullptr);
  EXPECT_EQ(9, send1(b, a, "at:", ao::Oop::fromSmallInteger(1)).smallIntegerValue());
}

TEST(ArrayString, Utf8AtReturnsCharacter) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "A");
  auto ch = send1(b, s, "at:", ao::Oop::fromSmallInteger(1));
  ASSERT_TRUE(ch.isCharacter());
  EXPECT_EQ(U'A', ch.characterValue());
  EXPECT_EQ(1, send0(b, s, "size").smallIntegerValue());
}

TEST(ArrayString, ByteArrayAtPutSmallInteger) {
  Boot b;
  auto ba = send1(b, b.wk.byteArrayClass, "new:", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(ba.isHeap());
  EXPECT_EQ(b.wk.byteArrayClass, b.heap.klass(ba));
  EXPECT_EQ(2, send0(b, ba, "size").smallIntegerValue());
  auto r = send2(b, ba, "at:put:", ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(255));
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(255, r.smallIntegerValue());
  auto v = send1(b, ba, "at:", ao::Oop::fromSmallInteger(1));
  ASSERT_TRUE(v.isSmallInteger());
  EXPECT_EQ(255, v.smallIntegerValue());
  EXPECT_EQ(0, send1(b, ba, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(ArrayString, Utf8SizeIsScalarCount) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "AあB");
  ASSERT_TRUE(s.isHeap());
  EXPECT_EQ(3, send0(b, s, "size").smallIntegerValue());
  EXPECT_EQ(5, send0(b, s, "basicSize").smallIntegerValue());
  EXPECT_EQ(3u, ao::Str::codePointCount(b.heap, s));
  auto a = send1(b, s, "at:", ao::Oop::fromSmallInteger(1));
  auto jp = send1(b, s, "at:", ao::Oop::fromSmallInteger(2));
  auto c = send1(b, s, "at:", ao::Oop::fromSmallInteger(3));
  ASSERT_TRUE(a.isCharacter());
  ASSERT_TRUE(jp.isCharacter());
  ASSERT_TRUE(c.isCharacter());
  EXPECT_EQ(U'A', a.characterValue());
  EXPECT_EQ(U'あ', jp.characterValue());
  EXPECT_EQ(U'B', c.characterValue());
  EXPECT_EQ(U'あ', ao::Str::at(b.heap, s, 2).characterValue());
}

TEST(ArrayString, InvalidUtf8ByteIsCharacter) {
  Boot b;
  const char raw[] = {'A', static_cast<char>(0xFF), 'Z'};
  auto s = ao::Str::fromUtf8(b.heap, b.wk, std::string_view(raw, 3));
  EXPECT_EQ(3, send0(b, s, "size").smallIntegerValue());
  auto ch = send1(b, s, "at:", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(ch.isCharacter());
  EXPECT_EQ(static_cast<char32_t>(0xFF), ch.characterValue());
}

TEST(ArrayString, StringEqualsComparesBytes) {
  Boot b;
  auto a = ao::Str::fromUtf8(b.heap, b.wk, "hi");
  auto same = ao::Str::fromUtf8(b.heap, b.wk, "hi");
  auto other = ao::Str::fromUtf8(b.heap, b.wk, "h");
  EXPECT_TRUE(send1(b, a, "=", same).isTrue());
  EXPECT_TRUE(send1(b, a, "=", other).isFalse());
  EXPECT_TRUE(send1(b, a, "=", ao::Oop::fromSmallInteger(1)).isFalse());
}

TEST(ArrayString, StringAtPutSameWidth) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "Aあ");
  auto z = send2(b, s, "at:put:", ao::Oop::fromSmallInteger(1), ao::Oop::fromCharacter(U'Z'));
  ASSERT_TRUE(z.isCharacter());
  EXPECT_EQ(U'Z', z.characterValue());
  auto i = send2(b, s, "at:put:", ao::Oop::fromSmallInteger(2), ao::Oop::fromCharacter(U'い'));
  ASSERT_TRUE(i.isCharacter());
  EXPECT_EQ(U'い', i.characterValue());
  EXPECT_EQ("Zい", ao::Str::toUtf8(b.heap, s));
}

TEST(ArrayString, AsSymbolAndAsString) {
  Boot b;
  // send をまたぐ値はルートしておく（GC ストレスでは send ごとに動く）。
  ao::Root s(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "foo"));
  ao::Root sym(b.roots, send0(b, s.slot, "asSymbol"));
  ASSERT_TRUE(sym.slot.isHeap());
  EXPECT_EQ(b.wk.symbolClass, b.heap.klass(sym.slot));
  EXPECT_EQ(sym.slot, b.wk.intern("foo"));
  ao::Root copy(b.roots, send0(b, sym.slot, "asString"));
  ASSERT_TRUE(copy.slot.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(copy.slot));
  EXPECT_EQ("foo", ao::Str::toUtf8(b.heap, copy.slot));
  EXPECT_NE(sym.slot, copy.slot);
  EXPECT_TRUE(send1(b, s.slot, "=", copy.slot).isTrue());
  EXPECT_TRUE(send1(b, s.slot, "=", sym.slot).isTrue());
}

TEST(ArrayString, FromSlotsAndDo) {
  Boot b;
  ao::Oop slots[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2),
                      ao::Oop::fromSmallInteger(3)};
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, slots, 3);
  ASSERT_TRUE(arr.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(arr));
  EXPECT_EQ(3, send0(b, arr, "size").smallIntegerValue());
  EXPECT_EQ(2, send1(b, arr, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());

  static std::int64_t sum;
  sum = 0;
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) {
      sum += args[0].smallIntegerValue();
    }
    return args[0];
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  ASSERT_TRUE(blk.isHeap());
  EXPECT_EQ(arr, send1(b, arr, "do:", blk));
  EXPECT_EQ(6, sum);
}

TEST(ArrayString, StringDoYieldsCharacters) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "Aあ");
  static char32_t seen[2];
  static int nseen;
  nseen = 0;
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (nseen < 2 && args[0].isCharacter()) {
      seen[nseen++] = args[0].characterValue();
    }
    return args[0];
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  send1(b, s, "do:", blk);
  EXPECT_EQ(2, nseen);
  EXPECT_EQ(U'A', seen[0]);
  EXPECT_EQ(U'あ', seen[1]);
}

TEST(ArrayString, BasicAtOnArray) {
  Boot b;
  auto a = send1(b, b.wk.arrayClass, "new:", ao::Oop::fromSmallInteger(1));
  send2(b, a, "at:put:", ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(8));
  EXPECT_EQ(8, send1(b, a, "basicAt:", ao::Oop::fromSmallInteger(1)).smallIntegerValue());
}

TEST(ArrayString, AtPutGrowsUtf8WithinObjectBytes) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "A");
  auto r = send2(b, s, "at:put:", ao::Oop::fromSmallInteger(1), ao::Oop::fromCharacter(U'あ'));
  ASSERT_TRUE(r.isCharacter());
  EXPECT_EQ(U'あ', r.characterValue());
  EXPECT_EQ("あ", ao::Str::toUtf8(b.heap, s));
  EXPECT_EQ(1, send0(b, s, "size").smallIntegerValue());
  EXPECT_EQ(3, send0(b, s, "basicSize").smallIntegerValue());
  EXPECT_EQ(U'あ', send1(b, s, "at:", ao::Oop::fromSmallInteger(1)).characterValue());

  auto four = send2(b, s, "at:put:", ao::Oop::fromSmallInteger(1),
                    ao::Oop::fromCharacter(U'\U0001F600'));
  ASSERT_TRUE(four.isCharacter());
  EXPECT_EQ(U'\U0001F600', four.characterValue());
  EXPECT_EQ(1, send0(b, s, "size").smallIntegerValue());
  EXPECT_EQ(4, send0(b, s, "basicSize").smallIntegerValue());
}

TEST(ArrayString, AtPutShrinksUtf8WithinObjectBytes) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "あ");
  auto r = send2(b, s, "at:put:", ao::Oop::fromSmallInteger(1), ao::Oop::fromCharacter(U'A'));
  ASSERT_TRUE(r.isCharacter());
  EXPECT_EQ(U'A', r.characterValue());
  EXPECT_EQ("A", ao::Str::toUtf8(b.heap, s));
  EXPECT_EQ(1, send0(b, s, "size").smallIntegerValue());
  EXPECT_EQ(1, send0(b, s, "basicSize").smallIntegerValue());
}

TEST(ArrayString, SymbolAtPutDoesNotMutateInternedBytes) {
  Boot b;
  // shouldNotImplement はメッセージの割り当てで GC する。Symbol はルートに載せて読み直す。
  ao::Root sym(b.roots, b.wk.intern("foo"));
  auto r = send2(b, sym.slot, "at:put:", ao::Oop::fromSmallInteger(1),
                 ao::Oop::fromCharacter(U'Z'));
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ("shouldNotImplement", ao::Str::toUtf8(b.heap, r));
  EXPECT_EQ("foo", ao::Str::toUtf8(b.heap, sym.slot));
  EXPECT_EQ(sym.slot, b.wk.intern("foo"));

  auto br = send2(b, sym.slot, "basicAt:put:", ao::Oop::fromSmallInteger(1),
                  ao::Oop::fromSmallInteger(static_cast<std::int64_t>('Z')));
  ASSERT_TRUE(br.isHeap());
  EXPECT_EQ("shouldNotImplement", ao::Str::toUtf8(b.heap, br));
  EXPECT_EQ("foo", ao::Str::toUtf8(b.heap, sym.slot));
}

#include "test_support.hpp"

#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"

#include <gtest/gtest.h>

#include <string>

TEST(TranscriptModel, ShowInvokesHook) {
  Boot b;
  static std::string seen;
  seen.clear();
  b.ctx.transcriptHook = [](ao::CallContext& ctx, ao::Oop v) {
    if (v.isCharacter()) seen.push_back(static_cast<char>(v.characterValue()));
    else seen += ao::Str::toUtf8(ctx.heap, v);
  };
  auto t = b.wk.transcript; // インスタンス
  send1(b, t, "show:", ao::Str::fromUtf8(b.heap, b.wk, "hello"));
  send0(b, t, "cr");
  EXPECT_EQ("hello\n", seen);
}

TEST(TranscriptModel, ClassSideShowForwardsToInstanceHook) {
  Boot b;
  static std::string seen;
  seen.clear();
  b.ctx.transcriptHook = [](ao::CallContext& ctx, ao::Oop v) {
    if (v.isCharacter()) seen.push_back(static_cast<char>(v.characterValue()));
    else if (!v.isNil()) seen += ao::Str::toUtf8(ctx.heap, v);
  };
  auto text = ao::Str::fromUtf8(b.heap, b.wk, "hello");
  auto ret = send1(b, b.wk.transcriptClass, "show:", text);
  EXPECT_EQ(b.wk.transcriptClass, ret);
  send0(b, b.wk.transcriptClass, "cr");
  EXPECT_EQ("hello\n", seen);
}

TEST(TranscriptModel, NextPutAndClearInvokeHook) {
  Boot b;
  static std::string seen;
  static bool cleared;
  seen.clear();
  cleared = false;
  b.ctx.transcriptHook = [](ao::CallContext& ctx, ao::Oop v) {
    if (v.isNil()) {
      cleared = true;
      return;
    }
    if (v.isCharacter()) {
      seen.push_back(static_cast<char>(v.characterValue()));
    } else {
      seen += ao::Str::toUtf8(ctx.heap, v);
    }
  };
  auto t = b.wk.transcript;
  ASSERT_TRUE(t.isHeap());
  EXPECT_EQ(b.wk.transcriptClass, b.heap.klass(t));
  EXPECT_EQ(b.wk.transcriptClass, b.wk.named("Transcript"));
  EXPECT_NE(t, b.wk.named("Transcript"));
  send1(b, t, "nextPut:", ao::Oop::fromCharacter(U'A'));
  send1(b, t, "nextPutAll:", ao::Str::fromUtf8(b.heap, b.wk, "bc"));
  send0(b, t, "clear");
  EXPECT_EQ("Abc", seen);
  EXPECT_TRUE(cleared);
}

TEST(TranscriptModel, WriteStreamGrowsArray) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root arr(b.roots, send1(b, b.wk.arrayClass, "new:", ao::Oop::fromSmallInteger(1)));
  ao::Root ws(b.roots, send1(b, b.wk.writeStreamClass, "on:", arr.slot));
  ASSERT_TRUE(ws.slot.isHeap());
  EXPECT_EQ(b.wk.writeStreamClass, b.heap.klass(ws.slot));
  EXPECT_EQ(ao::Oop::fromSmallInteger(1),
            send1(b, ws.slot, "nextPut:", ao::Oop::fromSmallInteger(1)));
  EXPECT_EQ(ao::Oop::fromSmallInteger(2),
            send1(b, ws.slot, "nextPut:", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(2, send0(b, ws.slot, "position").smallIntegerValue());
  ao::Root contents(b.roots, send0(b, ws.slot, "contents"));
  ASSERT_TRUE(contents.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(contents.slot));
  EXPECT_EQ(2, send0(b, contents.slot, "size").smallIntegerValue());
  EXPECT_EQ(1, send1(b, contents.slot, "at:", ao::Oop::fromSmallInteger(1)).smallIntegerValue());
  EXPECT_EQ(2, send1(b, contents.slot, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(TranscriptModel, WriteStreamOnStringNextPut) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root empty(b.roots, send1(b, b.wk.stringClass, "new:", ao::Oop::fromSmallInteger(0)));
  ao::Root ws(b.roots, send1(b, b.wk.writeStreamClass, "on:", empty.slot));
  ASSERT_TRUE(ws.slot.isHeap());
  send1(b, ws.slot, "nextPut:", ao::Oop::fromCharacter(U'A'));
  send1(b, ws.slot, "nextPut:", ao::Oop::fromCharacter(U'あ'));
  ao::Root contents(b.roots, send0(b, ws.slot, "contents"));
  ASSERT_TRUE(contents.slot.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(contents.slot));
  EXPECT_EQ("Aあ", ao::Str::toUtf8(b.heap, contents.slot));
  EXPECT_EQ(2, send0(b, contents.slot, "size").smallIntegerValue());
  EXPECT_EQ(2, send0(b, ws.slot, "position").smallIntegerValue());
  EXPECT_EQ(ws.slot, send1(b, ws.slot, "position:", ao::Oop::fromSmallInteger(1)));
  EXPECT_EQ(1, send0(b, ws.slot, "position").smallIntegerValue());
}

TEST(TranscriptModel, WriteStreamStringAppendUpdatesWriteLimit) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root empty(b.roots, send1(b, b.wk.stringClass, "new:", ao::Oop::fromSmallInteger(0)));
  ao::Root ws(b.roots, send1(b, b.wk.writeStreamClass, "on:", empty.slot));
  send1(b, ws.slot, "nextPut:", ao::Oop::fromCharacter(U'x'));
  send1(b, ws.slot, "nextPut:", ao::Oop::fromCharacter(U'y'));
  send1(b, ws.slot, "nextPut:", ao::Oop::fromCharacter(U'z'));
  EXPECT_EQ(3, send0(b, ws.slot, "position").smallIntegerValue());
  EXPECT_EQ(ws.slot, send1(b, ws.slot, "position:", ao::Oop::fromSmallInteger(1)));
  EXPECT_EQ(1, send0(b, ws.slot, "position").smallIntegerValue());
  EXPECT_EQ(ws.slot, send1(b, ws.slot, "position:", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(2, send0(b, ws.slot, "position").smallIntegerValue());
}

TEST(TranscriptModel, ReadStreamNextPositionResetContents) {
  Boot b;
  ao::Oop slots[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2),
                      ao::Oop::fromSmallInteger(3)};
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, slots, 3);
  auto rs = send1(b, b.wk.readStreamClass, "on:", arr);
  ASSERT_TRUE(rs.isHeap());
  EXPECT_EQ(b.wk.readStreamClass, b.heap.klass(rs));
  EXPECT_EQ(1, send0(b, rs, "next").smallIntegerValue());
  EXPECT_EQ(2, send0(b, rs, "next").smallIntegerValue());
  EXPECT_EQ(rs, send0(b, rs, "reset"));
  EXPECT_EQ(1, send0(b, rs, "next").smallIntegerValue());
  EXPECT_EQ(rs, send1(b, rs, "position:", ao::Oop::fromSmallInteger(2)));
  EXPECT_EQ(3, send0(b, rs, "next").smallIntegerValue());
  EXPECT_TRUE(send0(b, rs, "next").isNil());
  auto contents = send0(b, rs, "contents");
  ASSERT_TRUE(contents.isHeap());
  EXPECT_EQ(3, send0(b, contents, "size").smallIntegerValue());
  EXPECT_EQ(2, send1(b, contents, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(TranscriptModel, NextPutAllCopiesCollection) {
  Boot b;
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Oop slots[2] = {ao::Oop::fromSmallInteger(9), ao::Oop::fromSmallInteger(8)};
  ao::Root src(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 2));
  ao::Root dest(b.roots, send1(b, b.wk.arrayClass, "new:", ao::Oop::fromSmallInteger(0)));
  ao::Root ws(b.roots, send1(b, b.wk.writeStreamClass, "on:", dest.slot));
  EXPECT_EQ(src.slot, send1(b, ws.slot, "nextPutAll:", src.slot));
  ao::Root contents(b.roots, send0(b, ws.slot, "contents"));
  EXPECT_EQ(9, send1(b, contents.slot, "at:", ao::Oop::fromSmallInteger(1)).smallIntegerValue());
  EXPECT_EQ(8, send1(b, contents.slot, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
}

TEST(TranscriptModel, SmalltalkImageAtAndAtPut) {
  Boot b;
  auto img = send0(b, b.wk.smalltalkImageClass, "new");
  ASSERT_TRUE(img.isHeap());
  EXPECT_EQ(b.wk.smalltalkImageClass, b.heap.klass(img));
  EXPECT_EQ(img, send0(b, img, "globals"));
  EXPECT_EQ(b.wk.objectClass, send1(b, img, "at:", b.wk.intern("Object")));
  auto extra = ao::Oop::fromSmallInteger(42);
  EXPECT_EQ(extra, send2(b, img, "at:put:", b.wk.intern("Foo"), extra));
  EXPECT_EQ(extra, send1(b, img, "at:", b.wk.intern("Foo")));
  EXPECT_EQ(extra, b.wk.named("Foo"));
  EXPECT_EQ(extra, ao::Globals::at(b.wk, "Foo"));
}

TEST(TranscriptModel, EachClassSkipsSmalltalkImageNonClassExtra) {
  Boot b;
  auto img = send0(b, b.wk.smalltalkImageClass, "new");
  auto extra = ao::Oop::fromSmallInteger(42);
  send2(b, img, "at:put:", b.wk.intern("Foo"), extra);
  struct Seen {
    ao::Oop want;
    bool found = false;
  } seen{extra, false};
  b.wk.eachClass(
      [](void* p, ao::Oop cls) {
        auto* s = static_cast<Seen*>(p);
        if (cls == s->want) {
          s->found = true;
        }
      },
      &seen);
  EXPECT_FALSE(seen.found);
  EXPECT_EQ(extra, b.wk.named("Foo"));
}

// position は Smalltalk から書き換えられる。SmallInteger の最大値にしてから nextPut: すると、+1 が
// SmallInteger の範囲を超える。abort せず、error: の慣習どおりメッセージ文字列で失敗する。
TEST(TranscriptModel, NextPutAtSmiMaxPositionFails) {
  Boot b;
  ao::Root arr(b.roots, send1(b, b.wk.arrayClass, "new:", ao::Oop::fromSmallInteger(0)));
  ao::Root ws(b.roots, send1(b, b.wk.writeStreamClass, "on:", arr.slot));
  ASSERT_TRUE(ws.slot.isHeap());
  const ao::Oop max = ao::Oop::fromSmallInteger(ao::kSmiMax);
  ASSERT_EQ(max, send2(b, ws.slot, "instVarAt:put:", ao::Oop::fromSmallInteger(2), max));
  const ao::Oop r = send1(b, ws.slot, "nextPut:", ao::Oop::fromSmallInteger(1));
  ASSERT_TRUE(r.isHeap());
  ASSERT_EQ(b.wk.stringClass, b.heap.klass(r));
  EXPECT_EQ("nextPut: position out of range", ao::Str::toUtf8(b.heap, r));
  EXPECT_EQ(max, send0(b, ws.slot, "position"));
}

#include "test_support.hpp"

#include <cstdint>
#include <gtest/gtest.h>
#include <vector>

TEST(CollectionDo, ArrayCollectDoublesViaNativeBlock) {
  Boot b;
  ao::Oop slots[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2),
                      ao::Oop::fromSmallInteger(3)};
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, slots, 3);
  auto body = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    ao::Oop two = ao::Oop::fromSmallInteger(2);
    return ao::send(ctx, args[0], ctx.wk.intern("*"), &two, 1, nullptr);
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  auto r = send1(b, arr, "collect:", blk);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(2, b.heap.slotAt(r, 0).smallIntegerValue());
  EXPECT_EQ(4, b.heap.slotAt(r, 1).smallIntegerValue());
  EXPECT_EQ(6, b.heap.slotAt(r, 2).smallIntegerValue());
}

TEST(CollectionDo, DictionaryAtPut) {
  Boot b;
  auto d = send0(b, b.wk.identityDictionaryClass, "new");
  auto k = b.wk.intern("a");
  ao::Oop args[2] = {k, ao::Oop::fromSmallInteger(9)};
  auto sel = ao::Symbol::intern(b.wk, "at:put:");
  ao::send(b.ctx, d, sel, args, 2, nullptr);
  EXPECT_EQ(9, send1(b, d, "at:", k).smallIntegerValue());
}

TEST(CollectionDo, ArraySelectRejectDetectInjectIncludesIsEmpty) {
  Boot b;
  ao::Oop slots[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2),
                      ao::Oop::fromSmallInteger(3)};
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, slots, 3);
  EXPECT_TRUE(send0(b, arr, "isEmpty").isFalse());
  EXPECT_TRUE(send1(b, arr, "includes:", ao::Oop::fromSmallInteger(2)).isTrue());
  EXPECT_TRUE(send1(b, arr, "includes:", ao::Oop::fromSmallInteger(9)).isFalse());

  auto odd = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (!args[0].isSmallInteger()) {
      return ao::Oop::false_();
    }
    return (args[0].smallIntegerValue() % 2) != 0 ? ao::Oop::true_() : ao::Oop::false_();
  };
  auto oddBlk = ao::makeNativeBlock(b.ctx, odd, 1);
  auto selected = send1(b, arr, "select:", oddBlk);
  ASSERT_TRUE(selected.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(selected));
  EXPECT_EQ(2, send0(b, selected, "size").smallIntegerValue());
  EXPECT_EQ(1, b.heap.slotAt(selected, 0).smallIntegerValue());
  EXPECT_EQ(3, b.heap.slotAt(selected, 1).smallIntegerValue());

  auto rejected = send1(b, arr, "reject:", oddBlk);
  ASSERT_TRUE(rejected.isHeap());
  EXPECT_EQ(1, send0(b, rejected, "size").smallIntegerValue());
  EXPECT_EQ(2, b.heap.slotAt(rejected, 0).smallIntegerValue());

  auto none = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(0);
  };
  auto noneBlk = ao::makeNativeBlock(b.ctx, none, 0);
  auto detected = send2(b, arr, "detect:ifNone:", oddBlk, noneBlk);
  EXPECT_EQ(1, detected.smallIntegerValue());

  auto noMatch = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::false_();
  };
  auto noBlk = ao::makeNativeBlock(b.ctx, noMatch, 1);
  auto fallback = send2(b, arr, "detect:ifNone:", noBlk, noneBlk);
  EXPECT_EQ(0, fallback.smallIntegerValue());

  auto add = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    return ao::send(ctx, args[0], ctx.wk.intern("+"), &args[1], 1, nullptr);
  };
  auto addBlk = ao::makeNativeBlock(b.ctx, add, 2);
  auto sum = send2(b, arr, "inject:into:", ao::Oop::fromSmallInteger(0), addBlk);
  EXPECT_EQ(6, sum.smallIntegerValue());
}

TEST(CollectionDo, DictionaryEqualsLookupAndCollectValues) {
  Boot b;
  auto d = send0(b, b.wk.dictionaryClass, "new");
  ASSERT_TRUE(d.isHeap());
  auto k1 = ao::Str::fromUtf8(b.heap, b.wk, "a");
  auto k2 = ao::Str::fromUtf8(b.heap, b.wk, "a");
  EXPECT_NE(k1, k2);
  send2(b, d, "at:put:", k1, ao::Oop::fromSmallInteger(9));
  EXPECT_EQ(9, send1(b, d, "at:", k2).smallIntegerValue());
  EXPECT_TRUE(send1(b, d, "includesKey:", k2).isTrue());
  send2(b, d, "at:put:", ao::Str::fromUtf8(b.heap, b.wk, "b"), ao::Oop::fromSmallInteger(4));
  EXPECT_EQ(2, send0(b, d, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, d, "includes:", ao::Oop::fromSmallInteger(9)).isTrue());

  auto body = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    ao::Oop two = ao::Oop::fromSmallInteger(2);
    return ao::send(ctx, args[0], ctx.wk.intern("*"), &two, 1, nullptr);
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  auto r = send1(b, d, "collect:", blk);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(r));
  EXPECT_EQ(2, send0(b, r, "size").smallIntegerValue());
  EXPECT_EQ(18, b.heap.slotAt(r, 0).smallIntegerValue());
  EXPECT_EQ(8, b.heap.slotAt(r, 1).smallIntegerValue());

  static std::vector<std::int64_t> keys;
  keys.clear();
  auto assocDo = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    auto k = ao::send(ctx, args[0], ctx.wk.intern("key"), nullptr, 0, nullptr);
    auto v = ao::send(ctx, args[0], ctx.wk.intern("value"), nullptr, 0, nullptr);
    if (v.isSmallInteger()) {
      keys.push_back(v.smallIntegerValue());
    }
    (void)k;
    return args[0];
  };
  auto doBlk = ao::makeNativeBlock(b.ctx, assocDo, 1);
  EXPECT_EQ(d, send1(b, d, "do:", doBlk));
  ASSERT_EQ(2u, keys.size());
  EXPECT_EQ(9, keys[0]);
  EXPECT_EQ(4, keys[1]);
}

TEST(CollectionDo, IdentityDictionaryDoesNotUseEquals) {
  Boot b;
  auto d = send0(b, b.wk.identityDictionaryClass, "new");
  auto k1 = ao::Str::fromUtf8(b.heap, b.wk, "a");
  auto k2 = ao::Str::fromUtf8(b.heap, b.wk, "a");
  send2(b, d, "at:put:", k1, ao::Oop::fromSmallInteger(9));
  EXPECT_EQ(9, send1(b, d, "at:", k1).smallIntegerValue());
  EXPECT_TRUE(send1(b, d, "includesKey:", k1).isTrue());
  EXPECT_TRUE(send1(b, d, "includesKey:", k2).isFalse());
  EXPECT_TRUE(send1(b, d, "at:", k2).isNil());
}

TEST(CollectionDo, SetAndIdentitySet) {
  Boot b;
  auto s = send0(b, b.wk.setClass, "new");
  auto a = ao::Str::fromUtf8(b.heap, b.wk, "x");
  auto a2 = ao::Str::fromUtf8(b.heap, b.wk, "x");
  send1(b, s, "add:", a);
  send1(b, s, "add:", a2);
  EXPECT_EQ(1, send0(b, s, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, s, "includes:", a2).isTrue());

  auto ids = send0(b, b.wk.identitySetClass, "new");
  send1(b, ids, "add:", a);
  EXPECT_EQ(1, send0(b, ids, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, ids, "includes:", a).isTrue());
  EXPECT_TRUE(send1(b, ids, "includes:", a2).isFalse());
  send1(b, ids, "add:", a2);
  EXPECT_EQ(2, send0(b, ids, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, ids, "includes:", a2).isTrue());
}

TEST(CollectionDo, OrderedCollectionAddAtDo) {
  Boot b;
  auto oc = send0(b, b.wk.orderedCollectionClass, "new");
  ASSERT_TRUE(oc.isHeap());
  EXPECT_EQ(0, send0(b, oc, "size").smallIntegerValue());
  EXPECT_EQ(ao::Oop::fromSmallInteger(1), send1(b, oc, "add:", ao::Oop::fromSmallInteger(1)));
  for (std::int64_t i = 2; i <= 10; ++i) {
    send1(b, oc, "add:", ao::Oop::fromSmallInteger(i));
  }
  EXPECT_EQ(10, send0(b, oc, "size").smallIntegerValue());
  EXPECT_EQ(2, send1(b, oc, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
  EXPECT_EQ(10, send1(b, oc, "at:", ao::Oop::fromSmallInteger(10)).smallIntegerValue());
  static std::int64_t sum;
  sum = 0;
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) {
      sum += args[0].smallIntegerValue();
    }
    return args[0];
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  EXPECT_EQ(oc, send1(b, oc, "do:", blk));
  EXPECT_EQ(55, sum);
}

TEST(CollectionDo, IntervalFromToByAndIntegerTo) {
  Boot b;
  ao::Oop args[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(5),
                     ao::Oop::fromSmallInteger(2)};
  auto iv = ao::send(b.ctx, b.wk.intervalClass, b.wk.intern("from:to:by:"), args, 3, nullptr);
  ASSERT_TRUE(iv.isHeap());
  EXPECT_EQ(b.wk.intervalClass, b.heap.klass(iv));
  EXPECT_EQ(3, send0(b, iv, "size").smallIntegerValue());
  static std::vector<std::int64_t> seen;
  seen.clear();
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) {
      seen.push_back(args[0].smallIntegerValue());
    }
    return args[0];
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  send1(b, iv, "do:", blk);
  ASSERT_EQ(3u, seen.size());
  EXPECT_EQ(1, seen[0]);
  EXPECT_EQ(3, seen[1]);
  EXPECT_EQ(5, seen[2]);

  auto to = send1(b, ao::Oop::fromSmallInteger(1), "to:", ao::Oop::fromSmallInteger(3));
  EXPECT_EQ(b.wk.intervalClass, b.heap.klass(to));
  EXPECT_EQ(3, send0(b, to, "size").smallIntegerValue());
}

TEST(CollectionDo, StringCollectYieldsCharacters) {
  Boot b;
  auto s = ao::Str::fromUtf8(b.heap, b.wk, "Aあ");
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args,
                 std::uint32_t) { return args[0]; };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  auto r = send1(b, s, "collect:", blk);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(r));
  EXPECT_EQ(2, send0(b, r, "size").smallIntegerValue());
  ASSERT_TRUE(b.heap.slotAt(r, 0).isCharacter());
  ASSERT_TRUE(b.heap.slotAt(r, 1).isCharacter());
  EXPECT_EQ(U'A', b.heap.slotAt(r, 0).characterValue());
  EXPECT_EQ(U'あ', b.heap.slotAt(r, 1).characterValue());
}

TEST(CollectionDo, CollectDoesNotGrowNativeRegistry) {
  Boot b;
  ao::Oop slots[1] = {ao::Oop::fromSmallInteger(1)};
  auto arr = ao::Arr::fromSlots(b.heap, b.wk, slots, 1);
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args,
                 std::uint32_t) { return args[0]; };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  send1(b, arr, "collect:", blk);
  const auto n = ao::NativeRegistry::size();
  send1(b, arr, "collect:", blk);
  EXPECT_EQ(n, ao::NativeRegistry::size());
}

TEST(CollectionDo, AssociationKeyValue) {
  Boot b;
  auto k = b.wk.intern("a");
  auto a = send2(b, b.wk.associationClass, "key:value:", k, ao::Oop::fromSmallInteger(9));
  ASSERT_TRUE(a.isHeap());
  EXPECT_EQ(b.wk.associationClass, b.heap.klass(a));
  EXPECT_EQ(k, send0(b, a, "key"));
  EXPECT_EQ(9, send0(b, a, "value").smallIntegerValue());
  send1(b, a, "key:", b.wk.intern("b"));
  send1(b, a, "value:", ao::Oop::fromSmallInteger(8));
  EXPECT_EQ(b.wk.intern("b"), send0(b, a, "key"));
  EXPECT_EQ(8, send0(b, a, "value").smallIntegerValue());
}

TEST(CollectionDo, BagLinkedListMappedCollectionStubs) {
  Boot b;
  const ao::Oop classes[3] = {b.wk.bagClass, b.wk.linkedListClass, b.wk.mappedCollectionClass};
  const char* doNames[3] = {"ao_Bag_do_", "ao_LinkedList_do_", "ao_MappedCollection_do_"};
  const char* sizeNames[3] = {"ao_Bag_size", "ao_LinkedList_size", "ao_MappedCollection_size"};
  for (int i = 0; i < 3; ++i) {
    auto cls = classes[i];
    auto o = send0(b, cls, "new");
    ASSERT_TRUE(o.isHeap());
    EXPECT_EQ(0, send0(b, o, "size").smallIntegerValue());
    EXPECT_EQ(o, send1(b, o, "do:", ao::Oop::nil()));
    auto r = send1(b, o, "add:", ao::Oop::fromSmallInteger(1));
    ASSERT_TRUE(r.isHeap());
    EXPECT_EQ("subclassResponsibility", ao::Str::toUtf8(b.heap, r));
    auto doMeth = send1(b, cls, "compiledMethodAt:", b.wk.intern("do:"));
    auto sizeMeth = send1(b, cls, "compiledMethodAt:", b.wk.intern("size"));
    EXPECT_EQ(doNames[i], ao::NativeMethod::nameBytes(b.heap, doMeth));
    EXPECT_EQ(sizeNames[i], ao::NativeMethod::nameBytes(b.heap, sizeMeth));
  }
}

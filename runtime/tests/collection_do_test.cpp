#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <random>
#include <string>
#include <gtest/gtest.h>
#include <vector>

TEST(CollectionDo, ArrayCollectDoublesViaNativeBlock) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  ao::Oop slots[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(2),
                      ao::Oop::fromSmallInteger(3)};
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 3));
  auto body = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    ao::Oop two = ao::Oop::fromSmallInteger(2);
    return ao::send(ctx, args[0], ctx.wk.intern("*"), &two, 1, nullptr);
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  ao::Root r(b.roots, send1(b, arr.slot, "collect:", blk.slot));
  ASSERT_TRUE(r.slot.isHeap());
  EXPECT_EQ(2, b.heap.slotAt(r.slot, 0).smallIntegerValue());
  EXPECT_EQ(4, b.heap.slotAt(r.slot, 1).smallIntegerValue());
  EXPECT_EQ(6, b.heap.slotAt(r.slot, 2).smallIntegerValue());
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
  // send をまたぐ値はルートしておく（GC ストレスでは send ごとに動く）。
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 3));
  EXPECT_TRUE(send0(b, arr.slot, "isEmpty").isFalse());
  EXPECT_TRUE(send1(b, arr.slot, "includes:", ao::Oop::fromSmallInteger(2)).isTrue());
  EXPECT_TRUE(send1(b, arr.slot, "includes:", ao::Oop::fromSmallInteger(9)).isFalse());

  auto odd = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (!args[0].isSmallInteger()) {
      return ao::Oop::false_();
    }
    return (args[0].smallIntegerValue() % 2) != 0 ? ao::Oop::true_() : ao::Oop::false_();
  };
  ao::Root oddBlk(b.roots, ao::makeNativeBlock(b.ctx, odd, 1));
  ao::Root selected(b.roots, send1(b, arr.slot, "select:", oddBlk.slot));
  ASSERT_TRUE(selected.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(selected.slot));
  EXPECT_EQ(2, send0(b, selected.slot, "size").smallIntegerValue());
  EXPECT_EQ(1, b.heap.slotAt(selected.slot, 0).smallIntegerValue());
  EXPECT_EQ(3, b.heap.slotAt(selected.slot, 1).smallIntegerValue());

  ao::Root rejected(b.roots, send1(b, arr.slot, "reject:", oddBlk.slot));
  ASSERT_TRUE(rejected.slot.isHeap());
  EXPECT_EQ(1, send0(b, rejected.slot, "size").smallIntegerValue());
  EXPECT_EQ(2, b.heap.slotAt(rejected.slot, 0).smallIntegerValue());

  auto none = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(0);
  };
  ao::Root noneBlk(b.roots, ao::makeNativeBlock(b.ctx, none, 0));
  auto detected = send2(b, arr.slot, "detect:ifNone:", oddBlk.slot, noneBlk.slot);
  EXPECT_EQ(1, detected.smallIntegerValue());

  auto noMatch = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::false_();
  };
  ao::Root noBlk(b.roots, ao::makeNativeBlock(b.ctx, noMatch, 1));
  auto fallback = send2(b, arr.slot, "detect:ifNone:", noBlk.slot, noneBlk.slot);
  EXPECT_EQ(0, fallback.smallIntegerValue());

  auto add = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    return ao::send(ctx, args[0], ctx.wk.intern("+"), &args[1], 1, nullptr);
  };
  ao::Root addBlk(b.roots, ao::makeNativeBlock(b.ctx, add, 2));
  auto sum = send2(b, arr.slot, "inject:into:", ao::Oop::fromSmallInteger(0), addBlk.slot);
  EXPECT_EQ(6, sum.smallIntegerValue());
}

TEST(CollectionDo, DictionaryEqualsLookupAndCollectValues) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root d(b.roots, send0(b, b.wk.dictionaryClass, "new"));
  ASSERT_TRUE(d.slot.isHeap());
  ao::Root k1(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "a"));
  ao::Root k2(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "a"));
  EXPECT_NE(k1.slot, k2.slot);
  send2(b, d.slot, "at:put:", k1.slot, ao::Oop::fromSmallInteger(9));
  EXPECT_EQ(9, send1(b, d.slot, "at:", k2.slot).smallIntegerValue());
  EXPECT_TRUE(send1(b, d.slot, "includesKey:", k2.slot).isTrue());
  send2(b, d.slot, "at:put:", ao::Str::fromUtf8(b.heap, b.wk, "b"), ao::Oop::fromSmallInteger(4));
  EXPECT_EQ(2, send0(b, d.slot, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, d.slot, "includes:", ao::Oop::fromSmallInteger(9)).isTrue());

  auto body = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    ao::Oop two = ao::Oop::fromSmallInteger(2);
    return ao::send(ctx, args[0], ctx.wk.intern("*"), &two, 1, nullptr);
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  ao::Root r(b.roots, send1(b, d.slot, "collect:", blk.slot));
  ASSERT_TRUE(r.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(r.slot));
  EXPECT_EQ(2, send0(b, r.slot, "size").smallIntegerValue());
  // SPEC §3.6: the enumeration order of a hashed collection is unspecified.
  std::vector<std::int64_t> mapped = {b.heap.slotAt(r.slot, 0).smallIntegerValue(),
                                      b.heap.slotAt(r.slot, 1).smallIntegerValue()};
  std::sort(mapped.begin(), mapped.end());
  EXPECT_EQ((std::vector<std::int64_t>{8, 18}), mapped);

  // 04 High / SPEC §3.6: Dictionary>>do: hands each value to the block, not an Association.
  static std::vector<std::int64_t> values;
  values.clear();
  auto valueDo = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) {
      values.push_back(args[0].smallIntegerValue());
    }
    return args[0];
  };
  ao::Root doBlk(b.roots, ao::makeNativeBlock(b.ctx, valueDo, 1));
  EXPECT_EQ(d.slot, send1(b, d.slot, "do:", doBlk.slot));
  std::sort(values.begin(), values.end());
  EXPECT_EQ((std::vector<std::int64_t>{4, 9}), values);
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
  // send をまたぐ値はルートしておく（GC ストレスでは send ごとに動く）。
  ao::Root s(b.roots, send0(b, b.wk.setClass, "new"));
  ao::Root a(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "x"));
  ao::Root a2(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "x"));
  send1(b, s.slot, "add:", a.slot);
  send1(b, s.slot, "add:", a2.slot);
  EXPECT_EQ(1, send0(b, s.slot, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, s.slot, "includes:", a2.slot).isTrue());

  ao::Root ids(b.roots, send0(b, b.wk.identitySetClass, "new"));
  send1(b, ids.slot, "add:", a.slot);
  EXPECT_EQ(1, send0(b, ids.slot, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, ids.slot, "includes:", a.slot).isTrue());
  EXPECT_TRUE(send1(b, ids.slot, "includes:", a2.slot).isFalse());
  send1(b, ids.slot, "add:", a2.slot);
  EXPECT_EQ(2, send0(b, ids.slot, "size").smallIntegerValue());
  EXPECT_TRUE(send1(b, ids.slot, "includes:", a2.slot).isTrue());
}

TEST(CollectionDo, OrderedCollectionAddAtDo) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  EXPECT_EQ(0, send0(b, oc.slot, "size").smallIntegerValue());
  EXPECT_EQ(ao::Oop::fromSmallInteger(1), send1(b, oc.slot, "add:", ao::Oop::fromSmallInteger(1)));
  for (std::int64_t i = 2; i <= 10; ++i) {
    send1(b, oc.slot, "add:", ao::Oop::fromSmallInteger(i));
  }
  EXPECT_EQ(10, send0(b, oc.slot, "size").smallIntegerValue());
  EXPECT_EQ(2, send1(b, oc.slot, "at:", ao::Oop::fromSmallInteger(2)).smallIntegerValue());
  EXPECT_EQ(10, send1(b, oc.slot, "at:", ao::Oop::fromSmallInteger(10)).smallIntegerValue());
  static std::int64_t sum;
  sum = 0;
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) {
      sum += args[0].smallIntegerValue();
    }
    return args[0];
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  EXPECT_EQ(oc.slot, send1(b, oc.slot, "do:", blk.slot));
  EXPECT_EQ(55, sum);
}

TEST(CollectionDo, IntervalFromToByAndIntegerTo) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  ao::Oop args[3] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(5),
                     ao::Oop::fromSmallInteger(2)};
  ao::Root iv(b.roots,
              ao::send(b.ctx, b.wk.intervalClass, b.wk.intern("from:to:by:"), args, 3, nullptr));
  ASSERT_TRUE(iv.slot.isHeap());
  EXPECT_EQ(b.wk.intervalClass, b.heap.klass(iv.slot));
  EXPECT_EQ(3, send0(b, iv.slot, "size").smallIntegerValue());
  static std::vector<std::int64_t> seen;
  seen.clear();
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isSmallInteger()) {
      seen.push_back(args[0].smallIntegerValue());
    }
    return args[0];
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  send1(b, iv.slot, "do:", blk.slot);
  ASSERT_EQ(3u, seen.size());
  EXPECT_EQ(1, seen[0]);
  EXPECT_EQ(3, seen[1]);
  EXPECT_EQ(5, seen[2]);

  ao::Root to(b.roots, send1(b, ao::Oop::fromSmallInteger(1), "to:", ao::Oop::fromSmallInteger(3)));
  EXPECT_EQ(b.wk.intervalClass, b.heap.klass(to.slot));
  EXPECT_EQ(3, send0(b, to.slot, "size").smallIntegerValue());
}

TEST(CollectionDo, StringCollectYieldsCharacters) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root s(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "Aあ"));
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args,
                 std::uint32_t) { return args[0]; };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  ao::Root r(b.roots, send1(b, s.slot, "collect:", blk.slot));
  ASSERT_TRUE(r.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(r.slot));
  EXPECT_EQ(2, send0(b, r.slot, "size").smallIntegerValue());
  ASSERT_TRUE(b.heap.slotAt(r.slot, 0).isCharacter());
  ASSERT_TRUE(b.heap.slotAt(r.slot, 1).isCharacter());
  EXPECT_EQ(U'A', b.heap.slotAt(r.slot, 0).characterValue());
  EXPECT_EQ(U'あ', b.heap.slotAt(r.slot, 1).characterValue());
}

TEST(CollectionDo, CollectDoesNotGrowNativeRegistry) {
  Boot b;
  ao::Oop slots[1] = {ao::Oop::fromSmallInteger(1)};
  // send をまたぐ値はルートしておく（GC ストレスでは send ごとに動く）。
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 1));
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args,
                 std::uint32_t) { return args[0]; };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  send1(b, arr.slot, "collect:", blk.slot);
  const auto n = ao::NativeRegistry::size();
  send1(b, arr.slot, "collect:", blk.slot);
  EXPECT_EQ(n, ao::NativeRegistry::size());
}

TEST(CollectionDo, AssociationKeyValue) {
  Boot b;
  // send をまたぐ値はルートしておく（GC ストレスでは send ごとに動く。old の Symbol も動く）。
  ao::Root k(b.roots, b.wk.intern("a"));
  ao::Root a(b.roots,
             send2(b, b.wk.associationClass, "key:value:", k.slot, ao::Oop::fromSmallInteger(9)));
  ASSERT_TRUE(a.slot.isHeap());
  EXPECT_EQ(b.wk.associationClass, b.heap.klass(a.slot));
  EXPECT_EQ(k.slot, send0(b, a.slot, "key"));
  EXPECT_EQ(9, send0(b, a.slot, "value").smallIntegerValue());
  send1(b, a.slot, "key:", b.wk.intern("b"));
  send1(b, a.slot, "value:", ao::Oop::fromSmallInteger(8));
  EXPECT_EQ(b.wk.intern("b"), send0(b, a.slot, "key"));
  EXPECT_EQ(8, send0(b, a.slot, "value").smallIntegerValue());
}

TEST(CollectionDo, BagLinkedListMappedCollectionStubs) {
  Boot b;
  // send は GC しうる。old の GC ではクラスも動くので、クラスもルートに置いて使うたびに読み直す。
  ao::RootedArray classes(b.roots, 3);
  classes[0] = b.wk.bagClass;
  classes[1] = b.wk.linkedListClass;
  classes[2] = b.wk.mappedCollectionClass;
  const char* doNames[3] = {"ao_Bag_do_", "ao_LinkedList_do_", "ao_MappedCollection_do_"};
  const char* sizeNames[3] = {"ao_Bag_size", "ao_LinkedList_size", "ao_MappedCollection_size"};
  for (std::uint32_t i = 0; i < 3; ++i) {
    ao::Root o(b.roots, send0(b, classes[i], "new"));
    ASSERT_TRUE(o.slot.isHeap());
    EXPECT_EQ(0, send0(b, o.slot, "size").smallIntegerValue());
    EXPECT_EQ(o.slot, send1(b, o.slot, "do:", ao::Oop::nil()));
    // SPEC §3.3: スタブの add: は値を返さず、subclassResponsibility で評価を中断する。
    EXPECT_TRUE(send1(b, o.slot, "add:", ao::Oop::fromSmallInteger(1)).isEmpty());
    EXPECT_EQ("subclassResponsibility", takeAbortReason(b));
    ao::Root doMeth(b.roots, send1(b, classes[i], "compiledMethodAt:", b.wk.intern("do:")));
    auto sizeMeth = send1(b, classes[i], "compiledMethodAt:", b.wk.intern("size"));
    EXPECT_EQ(doNames[i], ao::NativeMethod::nameBytes(b.heap, doMeth.slot));
    EXPECT_EQ(sizeNames[i], ao::NativeMethod::nameBytes(b.heap, sizeMeth));
  }
}

namespace {

// 内部のカウンタ（Smalltalk から書き換えられるスロット）を SmallInteger の最大値にしてから操作する。
// ±1 が SmallInteger の範囲を超えるので、error: の慣習どおり、その文言で評価を中断する（SPEC §3.3）。
void expectFailAbort(Boot& b, ao::Oop r, const char* message) {
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ(message, takeAbortReason(b));
}

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

}  // namespace

TEST(CollectionDo, DictionaryAtPutWithTallyAtSmiMaxFails) {
  Boot b;
  ao::Root dict(b.roots, send0(b, b.wk.dictionaryClass, "new"));
  ASSERT_TRUE(dict.slot.isHeap());
  ASSERT_EQ(smi(ao::kSmiMax), send2(b, dict.slot, "instVarAt:put:", smi(1), smi(ao::kSmiMax)));
  ao::Root key(b.roots, b.wk.intern("smiMaxKey"));
  // SPEC §3.6: a tally past the capacity is a damaged table.
  expectFailAbort(b, send2(b, dict.slot, "at:put:", key.slot, smi(1)),
                   "damaged hashed collection");
  EXPECT_EQ(smi(ao::kSmiMax), send1(b, dict.slot, "instVarAt:", smi(1)));
}

TEST(CollectionDo, SetAddWithTallyAtSmiMaxFails) {
  Boot b;
  ao::Root set(b.roots, send0(b, b.wk.setClass, "new"));
  ASSERT_TRUE(set.slot.isHeap());
  ASSERT_EQ(smi(ao::kSmiMax), send2(b, set.slot, "instVarAt:put:", smi(1), smi(ao::kSmiMax)));
  expectFailAbort(b, send1(b, set.slot, "add:", smi(7)), "damaged hashed collection");
  EXPECT_EQ(smi(ao::kSmiMax), send1(b, set.slot, "instVarAt:", smi(1)));
}

// size は last - first + 1。first と last を両端にすると SmallInteger の範囲を超えるが、そういう組は
// array に収まらないので、壊れた組として失敗する（SPEC §3.6 OrderedCollection）。
TEST(CollectionDo, OrderedCollectionSizeBeyondSmiMaxFails) {
  Boot b;
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  send2(b, oc.slot, "instVarAt:put:", smi(2), smi(0));
  send2(b, oc.slot, "instVarAt:put:", smi(3), smi(ao::kSmiMax));
  expectFailAbort(b, send0(b, oc.slot, "size"), "damaged ordered collection");
  send2(b, oc.slot, "instVarAt:put:", smi(2), smi(ao::kSmiMin));
  expectFailAbort(b, send0(b, oc.slot, "size"), "damaged ordered collection");
}

namespace {

// スケジューラの実行可能キューと同じ外し方（Process.cpp の ocRemoveFirst）: 先頭のスロットを nil にして
// firstIndex を進める。GC しない。
ao::Oop ocRemoveFirst(Boot& b, ao::Oop oc) {
  const ao::Oop arr = b.heap.slotAt(oc, 0);
  const std::int64_t first = b.heap.slotAt(oc, 1).smallIntegerValue();
  const auto idx = static_cast<std::uint32_t>(first - 1);
  const ao::Oop value = b.heap.slotAt(arr, idx);
  b.heap.slotAtPut(arr, idx, ao::Oop::nil());
  b.heap.slotAtPut(oc, 1, smi(first + 1));
  return value;
}

std::uint32_t ocArraySize(Boot& b, ao::Oop oc) { return b.heap.size(b.heap.slotAt(oc, 0)); }

}  // namespace

// B9 の積み残し / B10: add: は、使っている数の 2 倍が array に収まるなら、伸ばさずに先頭へ詰める。
// 先頭から外して末尾へ足す使い方（実行可能キューの FIFO）で、array が際限なく伸びない。
TEST(CollectionDo, OrderedCollectionFifoKeepsItsArraySmall) {
  Boot b;
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  constexpr std::int64_t kRounds = 100000;
  for (std::int64_t i = 1; i <= kRounds; ++i) {
    ASSERT_EQ(smi(i), send1(b, oc.slot, "add:", smi(i)));
    ASSERT_EQ(smi(i), ocRemoveFirst(b, oc.slot));
  }
  EXPECT_EQ(0, send0(b, oc.slot, "size").smallIntegerValue());
  EXPECT_LE(ocArraySize(b, oc.slot), 64u);
  // 5 つを入れたままの FIFO でも同じ。
  for (std::int64_t i = 1; i <= 5; ++i) {
    send1(b, oc.slot, "add:", smi(i));
  }
  for (std::int64_t i = 6; i <= kRounds; ++i) {
    ASSERT_EQ(smi(i), send1(b, oc.slot, "add:", smi(i)));
    ASSERT_EQ(smi(i - 5), ocRemoveFirst(b, oc.slot));
  }
  EXPECT_EQ(5, send0(b, oc.slot, "size").smallIntegerValue());
  EXPECT_LE(ocArraySize(b, oc.slot), 64u);
  EXPECT_EQ(smi(kRounds - 4), send1(b, oc.slot, "at:", smi(1)));
  EXPECT_EQ(smi(kRounds), send1(b, oc.slot, "at:", smi(5)));
}

// 詰めたあとも要素は元の順に firstIndex 1 から並び、at: と do: はその順に答える。空いたスロットは nil
// （外した要素を捕まえておかない）。2 倍が収まらなければ、今までどおり倍の array へ写す。
TEST(CollectionDo, OrderedCollectionCompactionKeepsOrder) {
  Boot b;
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  ASSERT_EQ(8u, ocArraySize(b, oc.slot));
  for (std::int64_t i = 1; i <= 8; ++i) {
    send1(b, oc.slot, "add:", smi(i));
  }
  for (std::int64_t i = 1; i <= 5; ++i) {
    ASSERT_EQ(smi(i), ocRemoveFirst(b, oc.slot));
  }
  // 残り 6 7 8 の 2 倍は 8 に収まる: 詰めて [6 7 8 9 nil nil nil nil]。
  send1(b, oc.slot, "add:", smi(9));
  EXPECT_EQ(8u, ocArraySize(b, oc.slot));
  EXPECT_EQ(smi(1), b.heap.slotAt(oc.slot, 1));
  EXPECT_EQ(smi(4), b.heap.slotAt(oc.slot, 2));
  for (std::uint32_t i = 4; i < 8; ++i) {
    EXPECT_TRUE(b.heap.slotAt(b.heap.slotAt(oc.slot, 0), i).isNil()) << i;
  }
  for (std::int64_t i = 1; i <= 4; ++i) {
    EXPECT_EQ(smi(i + 5), send1(b, oc.slot, "at:", smi(i)));
  }
  static std::vector<std::int64_t> seen;
  seen.clear();
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    seen.push_back(args[0].isSmallInteger() ? args[0].smallIntegerValue() : -1);
    return args[0];
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 1));
  send1(b, oc.slot, "do:", blk.slot);
  EXPECT_EQ((std::vector<std::int64_t>{6, 7, 8, 9}), seen);

  // 10..13 で埋めてから 6 7 を外す。残り 8..13 の 2 倍は 8 に収まらないので、16 の array へ写す。
  for (std::int64_t i = 10; i <= 13; ++i) {
    send1(b, oc.slot, "add:", smi(i));
  }
  ASSERT_EQ(smi(6), ocRemoveFirst(b, oc.slot));
  ASSERT_EQ(smi(7), ocRemoveFirst(b, oc.slot));
  send1(b, oc.slot, "add:", smi(14));
  EXPECT_EQ(16u, ocArraySize(b, oc.slot));
  EXPECT_EQ(7, send0(b, oc.slot, "size").smallIntegerValue());
  seen.clear();
  send1(b, oc.slot, "do:", blk.slot);
  EXPECT_EQ((std::vector<std::int64_t>{8, 9, 10, 11, 12, 13, 14}), seen);
}

namespace {

// collect: と select: は、ネイティブのブロック（thunk）の pc を添字や件数に使う。do: を書き換えた
// コレクションは、そのブロックを受け取って pc を書き換えられる。skip: の回数だけは書き換えずに通す。
const char* kSmiMaxPoker =
    "!Collection subclass: #SmiMaxPoker\n"
    "  instanceVariableNames: 'results skip'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'SmiRange'!\n"
    "!SmiMaxPoker methodsFor: 'enumerating'!\n"
    "reset\n"
    "  results := OrderedCollection new.\n"
    "  skip := 0!\n"
    "skip: n\n"
    "  skip := n!\n"
    "results\n"
    "  ^results!\n"
    "size\n"
    "  ^1!\n"
    "do: aBlock\n"
    "  | saved |\n"
    "  skip > 0 ifTrue: [skip := skip - 1. ^aBlock value: 1].\n"
    "  saved := aBlock instVarAt: 2.\n"
    "  aBlock instVarAt: 2 put: 4611686018427387903.\n"
    "  results add: (aBlock value: 1).\n"
    "  aBlock instVarAt: 2 put: saved! !\n";

ao::Oop newPoker(Boot& b) {
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_TRUE(ao::fileInString(b.ctx, kSmiMaxPoker, errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root poker(b.roots, send0(b, b.wk.named("SmiMaxPoker"), "new"));
  send0(b, poker.slot, "reset");
  return poker.slot;
}

}  // namespace

TEST(CollectionDo, CollectIndexAtSmiMaxFails) {
  Boot b;
  ao::Root poker(b.roots, newPoker(b));
  ASSERT_TRUE(poker.slot.isHeap());
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    return args[0];
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, +body, 1));
  expectFailAbort(b, send1(b, poker.slot, "collect:", blk.slot), "collect: index out of range");
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  EXPECT_EQ(smi(0), send0(b, results.slot, "size"));
}

// SPEC §3.6: select: は do: を 1 回だけ送る。件数が SmallInteger の最大値なら、述語を呼ぶ前に失敗する。
// 書き換えなければ、同じコレクションで選んだ要素の Array を答える。
TEST(CollectionDo, SelectCountersAtSmiMaxFail) {
  Boot b;
  ao::Root poker(b.roots, newPoker(b));
  ASSERT_TRUE(poker.slot.isHeap());
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::true_();
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, +body, 1));
  expectFailAbort(b, send1(b, poker.slot, "select:", blk.slot), "select: count out of range");
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  EXPECT_EQ(smi(0), send0(b, results.slot, "size"));
  send1(b, poker.slot, "skip:", smi(1));
  ao::Root selected(b.roots, send1(b, poker.slot, "select:", blk.slot));
  ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  ASSERT_TRUE(selected.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(selected.slot));
  ASSERT_EQ(1u, b.heap.size(selected.slot));
  EXPECT_EQ(smi(1), b.heap.slotAt(selected.slot, 0));
}

namespace {

// 入口の検査のあとで利用者のブロックが走り、thunk の pc を書き換える。do: はブロックを ivar に
// 保存し、collect: / select: / reject: のブロックがそれを SmallInteger の最大値にする。skip: の回数
// だけは書き換えない。
const char* kSmiMaxLatePoker =
    "!Collection subclass: #SmiMaxLatePoker\n"
    "  instanceVariableNames: 'thunk results skip'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'SmiRange'!\n"
    "!SmiMaxLatePoker methodsFor: 'enumerating'!\n"
    "reset\n"
    "  results := OrderedCollection new.\n"
    "  skip := 0!\n"
    "skip: n\n"
    "  skip := n!\n"
    "results\n"
    "  ^results!\n"
    "size\n"
    "  ^1!\n"
    "do: aBlock\n"
    "  | saved |\n"
    "  thunk := aBlock.\n"
    "  saved := aBlock instVarAt: 2.\n"
    "  results add: (aBlock value: 1).\n"
    "  aBlock instVarAt: 2 put: saved!\n"
    "poke\n"
    "  skip > 0 ifTrue: [skip := skip - 1. ^self].\n"
    "  thunk instVarAt: 2 put: 4611686018427387903!\n"
    "pokeCollect\n"
    "  ^self collect: [:x | self poke. x]!\n"
    "pokeSelect\n"
    "  ^self select: [:x | self poke. true]!\n"
    "pokeReject\n"
    "  ^self reject: [:x | self poke. false]! !\n";

ao::Oop newLatePoker(Boot& b) {
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_TRUE(ao::fileInString(b.ctx, kSmiMaxLatePoker, errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root poker(b.roots, send0(b, b.wk.named("SmiMaxLatePoker"), "new"));
  send0(b, poker.slot, "reset");
  return poker.slot;
}

}  // namespace

// 0649b20 の入口の検査は、ブロックのあとで読み直した pc には効かなかった（+1 で abort）。
TEST(CollectionDo, CollectIndexPokedByUserBlockFails) {
  Boot b;
  ao::Root poker(b.roots, newLatePoker(b));
  ASSERT_TRUE(poker.slot.isHeap());
  // 書き換えた添字への at:put: が先に失敗し、その文言で中断する。
  expectFailAbort(b, send0(b, poker.slot, "pokeCollect"), "basicAt:put: index out of range");
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  EXPECT_EQ(smi(0), send0(b, results.slot, "size"));
}

// SPEC §3.6: 述語のあとで読み直した件数が SmallInteger の最大値なら、要素を入れずに失敗する。
// 書き換えなければ（skip:）失敗しない。この do: はブロックのあとで件数（instVarAt: 2）を呼ぶ前の
// 値に戻すので、答えは空の Array である。
TEST(CollectionDo, SelectCountersPokedByUserBlockFail) {
  Boot b;
  ao::Root poker(b.roots, newLatePoker(b));
  ASSERT_TRUE(poker.slot.isHeap());
  expectFailAbort(b, send0(b, poker.slot, "pokeSelect"), "select: count out of range");
  send1(b, poker.slot, "skip:", smi(1));
  ao::Root selected(b.roots, send0(b, poker.slot, "pokeSelect"));
  ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  ASSERT_TRUE(selected.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(selected.slot));
  EXPECT_EQ(0u, b.heap.size(selected.slot));
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  EXPECT_EQ(smi(1), send0(b, results.slot, "size"));
}

TEST(CollectionDo, RejectCountersPokedByUserBlockFail) {
  Boot b;
  ao::Root poker(b.roots, newLatePoker(b));
  ASSERT_TRUE(poker.slot.isHeap());
  expectFailAbort(b, send0(b, poker.slot, "pokeReject"), "reject: count out of range");
  send1(b, poker.slot, "skip:", smi(1));
  ao::Root rejected(b.roots, send0(b, poker.slot, "pokeReject"));
  ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  ASSERT_TRUE(rejected.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(rejected.slot));
  EXPECT_EQ(0u, b.heap.size(rejected.slot));
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  EXPECT_EQ(smi(1), send0(b, results.slot, "size"));
}

namespace {

// Smalltalk の式を Object のメソッドとして nil に対して走らせる（テンポラリは宣言して使う）。
ao::Oop evalBody(Boot& b, const std::string& body) {
  auto img = ao::compiler::compileMethod("doIt\n" + body);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  return ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

// body の答えの printString。評価が中断したら "<abort: 理由>"（SPEC §3.3）。
std::string printOf(Boot& b, const std::string& body) {
  ao::Root v(b.roots, evalBody(b, body));
  if (b.ctx.aborting) {
    return "<abort: " + takeAbortReason(b) + ">";
  }
  ao::Root s(b.roots, send0(b, v.slot, "printString"));
  if (b.ctx.aborting) {
    return "<abort: " + takeAbortReason(b) + ">";
  }
  return s.slot.isHeap() ? ao::Str::toUtf8(b.heap, s.slot) : std::string("<no string>");
}

std::int64_t& predicateCalls() {
  static std::int64_t n = 0;
  return n;
}

std::string& transcriptSeen() {
  static std::string seen;
  return seen;
}

// GC を走らせずに nursery を使い切る（残りは 16 B 未満）。
void fillNursery(Boot& b) {
  while (b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
}

}  // namespace

// docs/claude-review/04 Medium: select: と reject: は、数える do: と詰める do: の 2 回、述語を呼んで
// いた。SPEC §3.6: do: は 1 回で、述語は要素ごとに 1 回だけ呼ぶ。
TEST(CollectionDo, SelectAndRejectCallThePredicateOncePerElement) {
  Boot b;
  ao::Oop slots[3] = {smi(1), smi(2), smi(3)};
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 3));
  auto notTwo = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    ++predicateCalls();
    return args[0] == ao::Oop::fromSmallInteger(2) ? ao::Oop::false_() : ao::Oop::true_();
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, notTwo, 1));
  predicateCalls() = 0;
  ao::Root selected(b.roots, send1(b, arr.slot, "select:", blk.slot));
  EXPECT_EQ(3, predicateCalls());
  ASSERT_TRUE(selected.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(selected.slot));
  ASSERT_EQ(2u, b.heap.size(selected.slot));
  EXPECT_EQ(smi(1), b.heap.slotAt(selected.slot, 0));
  EXPECT_EQ(smi(3), b.heap.slotAt(selected.slot, 1));

  predicateCalls() = 0;
  ao::Root rejected(b.roots, send1(b, arr.slot, "reject:", blk.slot));
  EXPECT_EQ(3, predicateCalls());
  ASSERT_TRUE(rejected.slot.isHeap());
  ASSERT_EQ(1u, b.heap.size(rejected.slot));
  EXPECT_EQ(smi(2), b.heap.slotAt(rejected.slot, 0));
}

// docs/claude-review/04 Medium の失敗シナリオ。Transcript には 1 2 3 が 1 回だけ出る。副作用で答えの
// 変わる述語でも、答えは要素ごとの 1 回の評価どおりで、nil で埋まらない。
TEST(CollectionDo, SelectWithSideEffectsSeesEachElementOnce) {
  Boot b;
  transcriptSeen().clear();
  b.ctx.transcriptHook = [](ao::CallContext& ctx, ao::Oop v) {
    if (v.isHeap()) {
      transcriptSeen() += ao::Str::toUtf8(ctx.heap, v);
    }
  };
  EXPECT_EQ("#(1 2 3)",
            printOf(b, "^#(1 2 3) select: [:x | Transcript show: x printString. true]"));
  EXPECT_EQ("123", transcriptSeen());
  EXPECT_EQ("#(1 2 3)",
            printOf(b, "| oc | oc := OrderedCollection new.\n"
                       "^#(1 2 3) select: [:x | oc add: x. oc size <= 3]"));
  EXPECT_EQ("#()", printOf(b, "| oc | oc := OrderedCollection new.\n"
                              "^#(1 2 3) reject: [:x | oc add: x. oc size <= 3]"));
  EXPECT_EQ("3", printOf(b, "| n | n := 0. #(1 2 3) reject: [:x | n := n + 1. false]. ^n"));
}

// SPEC §3.6: 集める Array は倍々に伸びる。順序は do: の順である。
TEST(CollectionDo, SelectGrowsItsBufferInOrder) {
  Boot b;
  const std::string setup =
      "| a | a := Array new: 100. 1 to: 100 do: [:i | a at: i put: i].\n";
  EXPECT_EQ("33", printOf(b, setup + "^(a select: [:x | x \\\\ 3 = 0]) size"));
  EXPECT_EQ("1683",
            printOf(b, setup + "^(a select: [:x | x \\\\ 3 = 0]) inject: 0 into: [:s :x | s + x]"));
  EXPECT_EQ("99", printOf(b, setup + "^(a select: [:x | x \\\\ 3 = 0]) at: 33"));
  EXPECT_EQ("67", printOf(b, setup + "^(a reject: [:x | x \\\\ 3 = 0]) size"));
}

// B9 review (Medium): select:・reject:・detect:ifNone: は Boolean でない答えの要素を黙って捨てた。
// SPEC §3.6: to:do: と同じく答えに mustBeBoolean を送る（既定は NonBoolean receiver で中断）。
TEST(CollectionDo, NonBooleanPredicateAnswersGetMustBeBoolean) {
  Boot b;
  EXPECT_EQ("<abort: NonBoolean receiver>", printOf(b, "^#(1 2 3) select: [:x | nil]"));
  EXPECT_EQ("<abort: NonBoolean receiver>", printOf(b, "^#(1 2 3) reject: [:x | 3]"));
  EXPECT_EQ("<abort: NonBoolean receiver>",
            printOf(b, "^#(1 2 3) detect: [:x | 'yes'] ifNone: [0]"));
  EXPECT_EQ("<abort: NonBoolean receiver>",
            printOf(b, "^(OrderedCollection new add: 1; yourself) select: [:x | x]"));
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #B9Truthy\n"
                               "  instanceVariableNames: 'truth'\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B9-Test'!\n"
                               "!B9Truthy methodsFor: 'testing'!\n"
                               "truth: x\n"
                               "  truth := x!\n"
                               "mustBeBoolean\n"
                               "  ^truth! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  // mustBeBoolean's Boolean answer decides.
  EXPECT_EQ("#(2)", printOf(b, "^#(1 2 3) select: [:x | B9Truthy new truth: x = 2]"));
  EXPECT_EQ("#(1 3)", printOf(b, "^#(1 2 3) reject: [:x | B9Truthy new truth: x = 2]"));
  EXPECT_EQ("3", printOf(b, "^#(1 2 3) detect: [:x | B9Truthy new truth: x > 2] ifNone: [0]"));
  EXPECT_EQ("<abort: NonBoolean receiver>",
            printOf(b, "^#(1 2 3) select: [:x | B9Truthy new truth: 7]"));
}

// B9 review (Low): 作業領域（ブロックのスロットの Array と件数）が壊れていても、述語を呼んでから
// 失敗していた。SPEC §3.6: 述語を呼ぶ前にも確かめ、壊れていれば述語を呼ばずに失敗する。
TEST(CollectionDo, SelectChecksItsBufferBeforeThePredicate) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Collection subclass: #B9Corrupt\n"
                               "  instanceVariableNames: 'slot value'\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B9-Test'!\n"
                               "!B9Corrupt methodsFor: 'enumerating'!\n"
                               "slot: i value: v\n"
                               "  slot := i. value := v!\n"
                               "size\n"
                               "  ^1!\n"
                               "do: aBlock\n"
                               "  aBlock instVarAt: slot put: value.\n"
                               "  aBlock value: 1! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  // Slot 7 holds the Array (kBlockHome), slot 2 the count (kCtxPc).
  const char* damages[] = {"7 value: 3", "7 value: (OrderedCollection new)", "7 value: nil",
                           "2 value: -1", "2 value: nil", "2 value: 100", "2 value: 1.5"};
  for (const char* damage : damages) {
    for (const char* sel : {"select:", "reject:"}) {
      SCOPED_TRACE(std::string(damage) + " " + sel);
      const std::string reason = std::string(sel) == "select:" ? "select: count out of range"
                                                               : "reject: count out of range";
      EXPECT_EQ("<abort: " + reason + ">",
                printOf(b, "Smalltalk at: #B9Calls put: 0.\n^(B9Corrupt new slot: " +
                               std::string(damage) + ") " + sel +
                               " [:x | Smalltalk at: #B9Calls put: (Smalltalk at: #B9Calls) + 1. "
                               "true]"));
      EXPECT_EQ("0", printOf(b, "^Smalltalk at: #B9Calls"));
    }
  }
  // A sound count and Array: the predicate runs.
  EXPECT_EQ("#(1)", printOf(b, "^(B9Corrupt new slot: 2 value: 0) select: [:x | true]"));
}

// select: と reject: の穴: 述語からの非局所リターンと述語の中のエラー、述語の中でのレシーバの書き換え
// （Array、OrderedCollection、Dictionary、Set）、10 万要素での作業領域の拡張、答えの種類。
TEST(CollectionDo, SelectAndRejectEdgeCases) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #B9Picker\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B9-Test'!\n"
                               "!B9Picker methodsFor: 'picking'!\n"
                               "firstOver: n in: c\n"
                               "  c select: [:x | x > n ifTrue: [^x]. false].\n"
                               "  ^nil!\n"
                               "logged: c into: log\n"
                               "  ^[c reject: [:x | x = 2 ifTrue: [^#left]. false]] ensure: [log add: #ensured]! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  // ^ out of the predicate leaves select:; ensure: runs.
  EXPECT_EQ("3", printOf(b, "^B9Picker new firstOver: 2 in: #(1 2 3 4)"));
  EXPECT_EQ("true", printOf(b, "| log r | log := OrderedCollection new.\n"
                               "r := B9Picker new logged: #(1 2 3) into: log.\n"
                               "^(r == #left) & (log size = 1)"));
  // An error in the predicate aborts with its reason.
  EXPECT_EQ("<abort: doesNotUnderstand: #foo>", printOf(b, "^#(1 2) select: [:x | x foo]"));
  EXPECT_EQ("<abort: division by zero>", printOf(b, "^#(1 0) reject: [:x | 1 / x > 0]"));
  // The receiver written from the predicate: the answer holds what each call saw.
  EXPECT_EQ("#(1 9)", printOf(b, "| a | a := Array new: 3. a at: 1 put: 1; at: 2 put: 2; at: 3 put: 3.\n"
                                 "^a select: [:x | a at: 3 put: 9. x \\\\ 2 = 1]"));
  EXPECT_EQ("#(1 2 3)", printOf(b, "| oc | oc := OrderedCollection new. oc add: 1; add: 2; add: 3.\n"
                                   "^oc select: [:x | oc add: 100. x < 100]"));
  EXPECT_EQ("true", printOf(b, "| d r | d := Dictionary new. 1 to: 5 do: [:i | d at: i put: i].\n"
                               "r := d select: [:v | d removeKey: v ifAbsent: [nil]. true].\n"
                               "^(r size <= 5) & (r class == Array)"));
  EXPECT_EQ("true", printOf(b, "| s r | s := Set new. 1 to: 5 do: [:i | s add: i].\n"
                               "r := s reject: [:e | s add: e + 10. false].\n"
                               "^(r size >= 1) & (r class == Array)"));
  // 100 000 elements: the buffer doubles 8 -> ... -> 131072 (fewer under GC stress).
  const std::string count = b.heap.gcStress() != 0 ? "600" : "100000";
  EXPECT_EQ(b.heap.gcStress() != 0 ? "300" : "50000",
            printOf(b, "| a | a := Array new: " + count + ". 1 to: " + count +
                           " do: [:i | a at: i put: i].\n^(a select: [:x | x \\\\ 2 = 0]) size"));
  EXPECT_EQ(count, printOf(b, "| a r | a := Array new: " + count + ". 1 to: " + count +
                                  " do: [:i | a at: i put: i].\nr := a reject: [:x | false]. ^r at: " +
                                  count));
  // The answer is an Array whatever the receiver.
  for (const char* rcvr : {"(OrderedCollection new add: 1; add: 2; yourself)", "'ab'",
                           "(Set new add: 1; add: 2; yourself)", "(Interval from: 1 to: 2 by: 1)",
                           "(Dictionary new at: #a put: 1; at: #b put: 2; yourself)"}) {
    SCOPED_TRACE(rcvr);
    EXPECT_EQ("true", printOf(b, std::string("| r | r := ") + rcvr +
                                     " select: [:x | true]. ^(r class == Array) & (r size = 2)"));
    EXPECT_EQ("true", printOf(b, std::string("| r | r := ") + rcvr +
                                     " reject: [:x | true]. ^(r class == Array) & (r size = 0)"));
  }
}

// GC 圧下: nursery を満杯にしてから、集める Array が 8 → 16 → 32 → 64 と伸びる select: を送る。
TEST(CollectionDo, SelectGrowsItsBufferWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);
  ao::Root arr(b.roots, send1(b, b.wk.arrayClass, "new:", smi(40)));
  for (std::int64_t i = 1; i <= 40; ++i) {
    ao::Root text(b.roots, ao::Str::fromUtf8(b.ctx, "e" + std::to_string(i)));
    send2(b, arr.slot, "at:put:", smi(i), text.slot);
  }
  auto keep = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::true_();
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, keep, 1));
  fillNursery(b);
  b.heap.setGcStress(1);
  ao::Root selected(b.roots, send1(b, arr.slot, "select:", blk.slot));
  b.heap.setGcStress(0);
  ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  ASSERT_TRUE(selected.slot.isHeap());
  ASSERT_EQ(40u, b.heap.size(selected.slot));
  for (std::uint32_t i = 0; i < 40; ++i) {
    const ao::Oop e = b.heap.slotAt(selected.slot, i);
    ASSERT_TRUE(e.isHeap()) << i;
    EXPECT_EQ("e" + std::to_string(i + 1), ao::Str::toUtf8(b.heap, e)) << i;
    EXPECT_EQ(b.heap.slotAt(arr.slot, i), e) << i;
  }
}

// B9 review (Medium) / B8 レビュー: Collection>>includes: は hash を送って答えを捨て、`要素 = anObject`
// を送っていた。SPEC §3.6: Dictionary>>includes: と同じく `anObject = 要素`（Blue Book）を送り、同一の
// 要素には送らずに true、`=` の答えが Boolean でなければ失敗し、hash は送らない。
TEST(CollectionDo, IncludesSendsEqualsToTheArgumentAndNoHash) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #B9Needle\n"
                               "  instanceVariableNames: 'answer'\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B9-Test'!\n"
                               "!B9Needle methodsFor: 'comparing'!\n"
                               "answer: x\n"
                               "  answer := x!\n"
                               "hash\n"
                               "  Smalltalk at: #B9Hashes put: (Smalltalk at: #B9Hashes) + 1.\n"
                               "  ^self error: 'hash sent'!\n"
                               "= other\n"
                               "  Smalltalk at: #B9Equals put: (Smalltalk at: #B9Equals) + 1.\n"
                               "  answer == #boom ifTrue: [^self error: 'boom'].\n"
                               "  ^answer! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  const std::string reset = "Smalltalk at: #B9Hashes put: 0. Smalltalk at: #B9Equals put: 0.\n";
  // = goes to the argument: an element never answers true for 1 = aNeedle, the needle does.
  EXPECT_EQ("true", printOf(b, reset + "^#(1 2) includes: (B9Needle new answer: true)"));
  EXPECT_EQ("1", printOf(b, "^Smalltalk at: #B9Equals"));
  EXPECT_EQ("false", printOf(b, reset + "^#(1 2) includes: (B9Needle new answer: false)"));
  EXPECT_EQ("2", printOf(b, "^Smalltalk at: #B9Equals"));
  EXPECT_EQ("0", printOf(b, "^Smalltalk at: #B9Hashes"));
  // A non-Boolean answer fails; an abort in = keeps its reason.
  EXPECT_EQ("<abort: failed: #includes:>",
            printOf(b, "^#(1 2) includes: (B9Needle new answer: nil)"));
  EXPECT_EQ("<abort: failed: #includes:>",
            printOf(b, "^#(1 2) includes: (B9Needle new answer: 3)"));
  EXPECT_EQ("<abort: boom>", printOf(b, "^#(1 2) includes: (B9Needle new answer: #boom)"));
  // An identical element is found without a send; an empty collection sends nothing.
  EXPECT_EQ("true", printOf(b, "| n a | " + reset + "n := B9Needle new answer: #boom. "
                                "a := Array new: 2. a at: 1 put: n. ^a includes: n"));
  EXPECT_EQ("0", printOf(b, "^Smalltalk at: #B9Equals"));
  EXPECT_EQ("false", printOf(b, reset + "^(Array new: 0) includes: (B9Needle new answer: #boom)"));
  EXPECT_EQ("0", printOf(b, "^(Smalltalk at: #B9Equals) + (Smalltalk at: #B9Hashes)"));
  // The same through do: of other collections.
  EXPECT_EQ("true", printOf(b, "^(OrderedCollection new add: 1; add: 2; yourself) includes: 2"));
  EXPECT_EQ("true", printOf(b, "^(Interval from: 1 to: 5 by: 1) includes: 3"));
  EXPECT_EQ("false", printOf(b, "^'abc' includes: $z"));
  EXPECT_EQ("true", printOf(b, "^'abc' includes: $b"));
  EXPECT_EQ("true", printOf(b, "^#(1 2) includes: (B9Needle new answer: true)"));
  EXPECT_EQ("true", printOf(b, "^#(1 2) includes: 2"));
}

namespace {

std::vector<char32_t>& seenChars() {
  static std::vector<char32_t> seen;
  return seen;
}

}  // namespace

// docs/claude-review/04 Medium: String の do: は at: を 1 から n まで送り、at: は毎回 UTF-8 を先頭から
// 数えていた（O(n²)）。SPEC §3.6: String の do: は UTF-8 を先頭から 1 回たどり、文字ごとに呼ぶ。
TEST(CollectionDo, StringDoWalksTheUtf8Once) {
  Boot b;
  ao::Root s(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "a\xC3\xA9\xE3\x81\x82\xF0\x9D\x84\x9Ez"));
  auto record = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    if (args[0].isCharacter()) {
      seenChars().push_back(args[0].characterValue());
    }
    return args[0];
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, record, 1));
  seenChars().clear();
  EXPECT_EQ(s.slot, send1(b, s.slot, "do:", blk.slot));
  EXPECT_EQ((std::vector<char32_t>{U'a', U'\u00E9', U'\u3042', U'\U0001D11E', U'z'}), seenChars());
  // Symbol は String の do: を継ぐ。
  ao::Root sym(b.roots, b.wk.intern("ab"));
  seenChars().clear();
  EXPECT_EQ(sym.slot, send1(b, sym.slot, "do:", blk.slot));
  EXPECT_EQ((std::vector<char32_t>{U'a', U'b'}), seenChars());
  ao::Root method(b.roots, send1(b, b.wk.stringClass, "compiledMethodAt:", b.wk.intern("do:")));
  EXPECT_EQ("ao_String_do_", ao::NativeMethod::nameBytes(b.heap, method.slot));
}

// SPEC §3.6: ブロックが文字列を書き換えても落ちない（大きさを読み直し、その範囲で続ける）。飛ばすか
// 2 度渡すかは規定しないので、回数は元の文字数の前後にあることだけを見る。
TEST(CollectionDo, StringDoSurvivesTheBlockWritingTheString) {
  Boot b;
  for (const char* body :
       {"| s n | s := 'abcdefg' copy. n := 0.\n"
        "s do: [:c | n := n + 1. n = 1 ifTrue: [s at: 1 put: $\xC3\xA9]]. ^n",
        "| s n | s := '\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9' copy. n := 0.\n"
        "s do: [:c | n := n + 1. s at: 1 put: $a]. ^n",
        "| s n | s := 'ab\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9x' copy. n := 0.\n"
        "s do: [:c | n := n + 1. n <= 4 ifTrue: [s at: n put: $z]]. ^n"}) {
    SCOPED_TRACE(body);
    const std::string printed = printOf(b, body);
    ASSERT_FALSE(printed.empty());
    ASSERT_NE('<', printed[0]) << printed;
    const int n = std::stoi(printed);
    EXPECT_GE(n, 1);
    EXPECT_LE(n, 12);
  }
}

// B9 review (Medium): String の do: は、ブロックが渡し済みの文字の幅を変えると、デコード済みの幅で
// 進むので多バイト文字の途中に落ち、継続バイトを 1 文字として渡した（'ab' の 'a' を 'あ' にすると
// #(97 129 130 98)）。SPEC §3.6: 渡した文字の数で位置を合わせ直すので、文字列に無い文字は渡さない。
TEST(CollectionDo, StringDoPassesOnlyCharactersOfTheString) {
  Boot b;
  EXPECT_EQ("#(97 98 nil nil)",
            printOf(b, "| s out n | s := 'ab' copy. out := Array new: 4. n := 0.\n"
                       "s do: [:c | n := n + 1. out at: n put: c asInteger.\n"
                       "  n = 1 ifTrue: [s at: 1 put: $\xE3\x81\x82]]. ^out"));
  EXPECT_EQ("'say \xE2\x80\x9Dhi\xE2\x80\x9D now'",
            printOf(b, "| s i | s := 'say \"hi\" now' copy. i := 0.\n"
                       "s do: [:c | i := i + 1. c = $\" ifTrue: [s at: i put: $\xE2\x80\x9D]]. ^s"));
  // Narrowing a passed character: the walk goes on at the next character.
  EXPECT_EQ("#(233 12354 98)",
            printOf(b, "| s out n | s := '\xC3\xA9\xE3\x81\x82" "b' copy. out := Array new: 3. n := 0.\n"
                       "s do: [:c | n := n + 1. out at: n put: c asInteger. s at: 1 put: $x]. ^out"));
  // Raw bytes that leave fewer characters than were passed end the walk; the continuation bytes
  // left after the second character are not passed.
  EXPECT_EQ("#(97 98 nil nil)",
            printOf(b, "| s out n | s := 'abcd' copy. out := Array new: 4. n := 0.\n"
                       "s do: [:c | n := n + 1. out at: n put: c asInteger. n = 2 ifTrue: [\n"
                       "  s basicAt: 1 put: 227; basicAt: 2 put: 129; basicAt: 3 put: 130]]. ^out"));
}

namespace {

struct StringDoProbe {
  std::mt19937 rng{20260926};
  std::int64_t calls = 0;
  std::int64_t strangers = 0;
  ao::Root* walked = nullptr;  // the String being walked (a rooted slot the GC keeps current)
};

StringDoProbe& stringDoProbe() {
  static StringDoProbe probe;
  return probe;
}

}  // namespace

// SPEC §3.6: ブロックが毎回ランダムな位置の文字を 1〜4 バイトの文字で書き換えても、渡される文字は
// どれも、その時点の文字列に at: で読める文字である。はぐれた継続バイトと単独の lead byte（正しく
// ない UTF-8）も混ぜる。
TEST(CollectionDo, StringDoRandomRewritesPassOnlyPresentCharacters) {
  Boot b;
  const bool stressed = b.heap.gcStress() != 0;
  static constexpr char32_t kChars[] = {U'a', U'\u00E9', U'\u3042', U'\U0001D11E', U'z'};
  auto body = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    StringDoProbe& p = stringDoProbe();
    ++p.calls;
    ao::Root ch(ctx.roots, args[0]);
    const std::uint32_t n = ao::Str::codePointCount(ctx.heap, p.walked->slot);
    bool present = false;
    for (std::uint32_t i = 1; i <= n && !present; ++i) {
      present = ao::Str::at(ctx.heap, p.walked->slot, i) == ch.slot;
    }
    p.strangers += present ? 0 : 1;
    const auto times = static_cast<std::uint32_t>(p.rng() % 3);
    for (std::uint32_t t = 0; t < times && n > 0; ++t) {
      ao::Oop put[2] = {ao::Oop::fromSmallInteger(1 + static_cast<std::int64_t>(p.rng() % n)),
                        ao::Oop::fromCharacter(kChars[p.rng() % 5])};
      ao::send(ctx, p.walked->slot, ctx.wk.intern("at:put:"), put, 2, nullptr);
      if (ctx.aborting) {
        // A wider character that does not fit the String's bytes fails; the String is unchanged.
        ao::clearUnwinding(ctx);
      }
    }
    return ch.slot;
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, +body, 1));
  ao::Root s(b.roots);
  stringDoProbe().walked = &s;
  for (int round = 0; round < (stressed ? 5 : 80); ++round) {
    SCOPED_TRACE(round);
    std::string bytes;
    const int len = 1 + static_cast<int>(stringDoProbe().rng() % 24);
    for (int i = 0; i < len; ++i) {
      // Mostly characters of 1 to 4 bytes; now and then a stray continuation byte or a lone lead
      // byte (invalid UTF-8, one character each to at:).
      const std::uint32_t kind = stringDoProbe().rng() % 8;
      if (kind == 0) {
        bytes.push_back('\xA0');
        continue;
      }
      if (kind == 1) {
        bytes.push_back('\xE3');
        continue;
      }
      unsigned char enc[4];
      const std::uint32_t w = ao::Str::encodeUtf8(kChars[stringDoProbe().rng() % 5], enc);
      bytes.append(reinterpret_cast<const char*>(enc), w);
    }
    s.slot = ao::Str::fromUtf8(b.ctx, bytes);
    stringDoProbe().calls = 0;
    stringDoProbe().strangers = 0;
    send1(b, s.slot, "do:", blk.slot);
    ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
    EXPECT_GE(stringDoProbe().calls, 1);
    EXPECT_EQ(0, stringDoProbe().strangers);
  }
  stringDoProbe().walked = nullptr;
}

// SPEC §3.6: at: か size を上書きしたサブクラスは、size と at: を送って回す。
TEST(CollectionDo, StringDoSendsAtWhenASubclassOverridesIt) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!String subclass: #B9Masked\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B9-Test'!\n"
                               "!B9Masked methodsFor: 'accessing'!\n"
                               "at: i\n"
                               "  ^$*! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  EXPECT_EQ("3", printOf(b, "| n | n := 0. (B9Masked new: 3) do: [:c | c == $* ifTrue: [n := n + 1]]. ^n"));
}

// docs/claude-review/04 Medium の計測: (String new: 40000) inject: 0 into: [...] は 6.7 秒かかった
// （Debug）。1 パスの do: で 1 秒未満。上限はゆるく取る（ASan を手で回すと 1.2 秒になるので 2 秒）。
TEST(KernelBench, StringInjectFortyThousandCharacters) {
  Boot b;
  const auto start = std::chrono::steady_clock::now();
  const std::string printed = printOf(b, "^(String new: 40000) inject: 0 into: [:a :c | a + 1]");
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::printf("B9 (String new: 40000) inject:into: %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ("40000", printed);
  EXPECT_LT(ms, 2000);
}

// B9 確認レビュー (Low): はぐれた継続バイト（0xA0）だけの String は、次の位置が継続バイトなので
// 毎回先頭から合わせ直し、do: が 2 乗時間になった（Release で 8 万バイト 12 秒）。はぐれた継続
// バイトはそれ自体 1 文字なので合わせ直さない（SPEC §3.6）。どのバイトも 1 文字として渡る。4 万バイトは
// 2 乗なら Debug で 7 秒かかる。上限は、手で回す ASan（1.5 秒ほど）でも赤にならないように取る。
TEST(KernelBench, StringDoOverStrayContinuationBytesIsLinear) {
  Boot b;
  const std::string bytes(40000, '\xA0');
  ao::Root s(b.roots, ao::Str::fromUtf8(b.ctx, bytes));
  ASSERT_TRUE(s.slot.isHeap());
  ASSERT_TRUE(b.wk.define("B9Stray", s.slot));
  const auto start = std::chrono::steady_clock::now();
  const std::string printed =
      printOf(b, "| n ok | n := 0. ok := true. B9Stray do: [:c | n := n + 1. "
                 "c asInteger = 160 ifFalse: [ok := false]]. ^ok & (n = 40000)");
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::printf("B9 40000 stray continuation bytes do: %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ("true", printed);
  EXPECT_LT(ms, 3000);
}

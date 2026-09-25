#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"

#include <algorithm>
#include <cstdint>
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

// size は last - first + 1。first と last を両端にすると SmallInteger の範囲を超える。
TEST(CollectionDo, OrderedCollectionSizeBeyondSmiMaxFails) {
  Boot b;
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  send2(b, oc.slot, "instVarAt:put:", smi(2), smi(0));
  send2(b, oc.slot, "instVarAt:put:", smi(3), smi(ao::kSmiMax));
  expectFailAbort(b, send0(b, oc.slot, "size"), "size out of range");
  send2(b, oc.slot, "instVarAt:put:", smi(2), smi(ao::kSmiMin));
  expectFailAbort(b, send0(b, oc.slot, "size"), "size out of range");
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

// SPEC §3.6: 集める Array は倍々に伸びる。順序は do: の順で、Boolean でない答えの要素はどちらにも
// 入らない。
TEST(CollectionDo, SelectGrowsItsBufferInOrderAndSkipsNonBooleans) {
  Boot b;
  const std::string setup =
      "| a | a := Array new: 100. 1 to: 100 do: [:i | a at: i put: i].\n";
  EXPECT_EQ("33", printOf(b, setup + "^(a select: [:x | x \\\\ 3 = 0]) size"));
  EXPECT_EQ("1683",
            printOf(b, setup + "^(a select: [:x | x \\\\ 3 = 0]) inject: 0 into: [:s :x | s + x]"));
  EXPECT_EQ("99", printOf(b, setup + "^(a select: [:x | x \\\\ 3 = 0]) at: 33"));
  EXPECT_EQ("67", printOf(b, setup + "^(a reject: [:x | x \\\\ 3 = 0]) size"));
  EXPECT_EQ("#()", printOf(b, "^#(1 2 3) select: [:x | nil]"));
  EXPECT_EQ("#()", printOf(b, "^#(1 2 3) reject: [:x | 3]"));
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

// B8 レビュー / SPEC §3.6: includes: は hash の答えを捨てていた。答えが Integer でなければ失敗し、
// hash が中断すればその理由のまま中断する。
TEST(CollectionDo, IncludesFailsWhenHashAnswersNoInteger) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #B9OddHash\n"
                               "  instanceVariableNames: 'answer'\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'B9-Test'!\n"
                               "!B9OddHash methodsFor: 'comparing'!\n"
                               "answer: x\n"
                               "  answer := x!\n"
                               "hash\n"
                               "  answer == #boom ifTrue: [^self error: 'boom'].\n"
                               "  ^answer! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  EXPECT_EQ("<abort: failed: #includes:>",
            printOf(b, "^#(1 2) includes: (B9OddHash new answer: nil)"));
  EXPECT_EQ("<abort: failed: #includes:>",
            printOf(b, "^#(1 2) includes: (B9OddHash new answer: 1.5)"));
  EXPECT_EQ("<abort: boom>", printOf(b, "^#(1 2) includes: (B9OddHash new answer: #boom)"));
  EXPECT_EQ("false", printOf(b, "^#(1 2) includes: (B9OddHash new answer: 7)"));
  EXPECT_EQ("false",
            printOf(b, "^#(1 2) includes: (B9OddHash new answer: 1000000000000000000000)"));
  EXPECT_EQ("true", printOf(b, "^#(1 2) includes: 2"));
}

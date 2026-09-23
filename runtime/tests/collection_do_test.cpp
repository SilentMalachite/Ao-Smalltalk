#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/HandleScope.hpp"

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
  EXPECT_EQ(18, b.heap.slotAt(r.slot, 0).smallIntegerValue());
  EXPECT_EQ(8, b.heap.slotAt(r.slot, 1).smallIntegerValue());

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
  ao::Root doBlk(b.roots, ao::makeNativeBlock(b.ctx, assocDo, 1));
  EXPECT_EQ(d.slot, send1(b, d.slot, "do:", doBlk.slot));
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

namespace {

// 内部のカウンタ（Smalltalk から書き換えられるスロット）を SmallInteger の最大値にしてから操作する。
// ±1 が SmallInteger の範囲を超えるので、abort せず、error: の慣習どおりメッセージ文字列で失敗する。
void expectFailString(Boot& b, ao::Oop r, const char* message) {
  ASSERT_TRUE(r.isHeap());
  ASSERT_EQ(b.wk.stringClass, b.heap.klass(r));
  EXPECT_EQ(message, ao::Str::toUtf8(b.heap, r));
}

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

}  // namespace

TEST(CollectionDo, DictionaryAtPutWithTallyAtSmiMaxFails) {
  Boot b;
  ao::Root dict(b.roots, send0(b, b.wk.dictionaryClass, "new"));
  ASSERT_TRUE(dict.slot.isHeap());
  ASSERT_EQ(smi(ao::kSmiMax), send2(b, dict.slot, "instVarAt:put:", smi(1), smi(ao::kSmiMax)));
  ao::Root key(b.roots, b.wk.intern("smiMaxKey"));
  expectFailString(b, send2(b, dict.slot, "at:put:", key.slot, smi(1)),
                   "at:put: tally out of range");
  EXPECT_EQ(smi(ao::kSmiMax), send1(b, dict.slot, "instVarAt:", smi(1)));
}

TEST(CollectionDo, SetAddWithTallyAtSmiMaxFails) {
  Boot b;
  ao::Root set(b.roots, send0(b, b.wk.setClass, "new"));
  ASSERT_TRUE(set.slot.isHeap());
  ASSERT_EQ(smi(ao::kSmiMax), send2(b, set.slot, "instVarAt:put:", smi(1), smi(ao::kSmiMax)));
  expectFailString(b, send1(b, set.slot, "add:", smi(7)), "add: tally out of range");
  EXPECT_EQ(smi(ao::kSmiMax), send1(b, set.slot, "instVarAt:", smi(1)));
}

// size は last - first + 1。first と last を両端にすると SmallInteger の範囲を超える。
TEST(CollectionDo, OrderedCollectionSizeBeyondSmiMaxFails) {
  Boot b;
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  send2(b, oc.slot, "instVarAt:put:", smi(2), smi(0));
  send2(b, oc.slot, "instVarAt:put:", smi(3), smi(ao::kSmiMax));
  expectFailString(b, send0(b, oc.slot, "size"), "size out of range");
  send2(b, oc.slot, "instVarAt:put:", smi(2), smi(ao::kSmiMin));
  expectFailString(b, send0(b, oc.slot, "size"), "size out of range");
}

namespace {

// collect: と select: は、ネイティブのブロック（thunk）の pc を添字や件数に使う。do: を書き換えた
// コレクションは、そのブロックを受け取って pc を書き換えられる。
const char* kSmiMaxPoker =
    "!Collection subclass: #SmiMaxPoker\n"
    "  instanceVariableNames: 'results'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'SmiRange'!\n"
    "!SmiMaxPoker methodsFor: 'enumerating'!\n"
    "reset\n"
    "  results := OrderedCollection new!\n"
    "results\n"
    "  ^results!\n"
    "size\n"
    "  ^1!\n"
    "do: aBlock\n"
    "  | saved |\n"
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
  send1(b, poker.slot, "collect:", blk.slot);
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  ASSERT_EQ(smi(1), send0(b, results.slot, "size"));
  expectFailString(b, send1(b, results.slot, "at:", smi(1)), "collect: index out of range");
}

// select: は件数を数える do: と、詰める do: の 2 回を回す。どちらのブロックも失敗する。
TEST(CollectionDo, SelectCountersAtSmiMaxFail) {
  Boot b;
  ao::Root poker(b.roots, newPoker(b));
  ASSERT_TRUE(poker.slot.isHeap());
  auto body = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::true_();
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, +body, 1));
  send1(b, poker.slot, "select:", blk.slot);
  ao::Root results(b.roots, send0(b, poker.slot, "results"));
  ASSERT_EQ(smi(2), send0(b, results.slot, "size"));
  expectFailString(b, send1(b, results.slot, "at:", smi(1)), "select: count out of range");
  expectFailString(b, send1(b, results.slot, "at:", smi(2)), "select: index out of range");
}

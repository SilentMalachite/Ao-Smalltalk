#include "test_support.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Heap.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <cstddef>
#include <cstdio>
#include <vector>

#include <gtest/gtest.h>

TEST(MethodDictionary, AtPutFindsInternedKey) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto dict = ao::MethodDictionary::create(heap, wk, 2);
  auto k1 = ao::Symbol::intern(wk, "==");
  auto k2 = ao::Symbol::intern(wk, "==");
  auto meth = ao::Oop::fromSmallInteger(7);
  ao::MethodDictionary::atPut(heap, dict, k1, meth);
  EXPECT_EQ(meth, ao::MethodDictionary::at(heap, dict, k2));
  EXPECT_TRUE(ao::MethodDictionary::at(heap, dict, ao::Symbol::intern(wk, "class")).isNil());
  EXPECT_EQ(dict, dict);  // outer oop identity
}

TEST(MethodDictionary, GrowKeepsOuterOopAndEntries) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto dict = ao::MethodDictionary::create(heap, wk, 2);
  const ao::Oop outer = dict;
  for (int i = 0; i < 10; ++i) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "s%d", i);
    ao::MethodDictionary::atPut(heap, dict, ao::Symbol::intern(wk, buf),
                                ao::Oop::fromSmallInteger(i));
  }
  EXPECT_EQ(outer, dict);
  EXPECT_EQ(ao::Oop::fromSmallInteger(9),
            ao::MethodDictionary::at(heap, dict, ao::Symbol::intern(wk, "s9")));
}

namespace {

// subclass: が作る辞書（8 エントリ）をちょうど満杯にするクラス。9 個目の登録で内部配列を伸ばす。
const char* kEightMethods =
    "!Object subclass: #MdEight\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'MethodDictionaryTest'!\n"
    "!MdEight methodsFor: 'eight'!\n"
    "m1\n  ^1!\nm2\n  ^2!\nm3\n  ^3!\nm4\n  ^4!\n"
    "m5\n  ^5!\nm6\n  ^6!\nm7\n  ^7!\nm8\n  ^8! !\n";

ao::Oop defineEightMethodClass(Boot& b) {
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_TRUE(ao::fileInString(b.ctx, kEightMethods, errs)) << (errs.empty() ? "" : errs[0].message);
  const ao::Oop cls = b.wk.named("MdEight");
  if (!cls.isHeap()) {
    return cls;
  }
  const ao::Oop dict = b.heap.slotAt(cls, ao::kClassSlotMethodDict);
  EXPECT_EQ(ao::Oop::fromSmallInteger(8), b.heap.slotAt(dict, ao::kDictSlotTally));
  EXPECT_EQ(16u, b.heap.size(b.heap.slotAt(dict, ao::kDictSlotArray)));
  return cls;
}

// 01 Medium の状態を作る: 9 個目の CompiledMethod 一式はちょうど入り、伸ばした辞書の内部配列
// （32 スロット、272 B）は入らないだけの nursery の残り。GC は走らせない。
void leaveNurseryForMethodOnly(Boot& b, const ao::compiler::MethodImage& image, ao::Oop cls) {
  const std::size_t before = b.heap.nurseryRemaining();
  ASSERT_TRUE(ao::boxMethodImage(b.ctx, image, cls).isHeap());
  const std::size_t methodBytes = before - b.heap.nurseryRemaining();
  while (b.heap.nurseryRemaining() >= methodBytes + 16) {
    ASSERT_TRUE(b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap());
  }
  ASSERT_GE(b.heap.nurseryRemaining(), methodBytes);
  ASSERT_LT(b.heap.nurseryRemaining(), methodBytes + 272);
}

}  // namespace

// 01 Medium の失敗シナリオそのもの: 満杯の辞書、nursery の残りは CompiledMethod の分だけ。
// 伸ばした内部配列は old に置かれ、9 個目も引ける。
TEST(MethodDictionary, NinthMethodWithFullNurseryIsInstalled) {
  Boot b;
  b.heap.setGcStress(0);  // 割り当ての途中で GC が走ると、nursery の残りを作れない
  ao::Root cls(b.roots, defineEightMethodClass(b));
  ASSERT_TRUE(cls.slot.isHeap());
  const auto cr = ao::compiler::compileMethod("ninth\n  ^9");
  ASSERT_TRUE(cr.ok);
  ao::Root sel(b.roots, b.wk.intern("ninth"));
  leaveNurseryForMethodOnly(b, cr.image, cls.slot);

  ao::Root installed(b.roots, ao::installMethod(b.ctx, cls.slot, cr.image));
  ASSERT_TRUE(installed.slot.isHeap());
  EXPECT_EQ(installed.slot, ao::lookup(b.heap, cls.slot, sel.slot));
  const ao::Oop dict = b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict);
  EXPECT_EQ(ao::Oop::fromSmallInteger(9), b.heap.slotAt(dict, ao::kDictSlotTally));
  EXPECT_TRUE(b.heap.inOld(b.heap.slotAt(dict, ao::kDictSlotArray)));
}

// old も上限なら内部配列を伸ばせない。atPut の失敗は installMethod の空 Oop として返り、
// 元の 8 エントリは残る。
TEST(MethodDictionary, AtPutFailureReachesInstallMethod) {
  Boot b(1 << 20, 1 << 20, 1 << 20);  // old は 1 MiB で頭打ち
  b.heap.setGcStress(0);
  ao::Root cls(b.roots, defineEightMethodClass(b));
  ASSERT_TRUE(cls.slot.isHeap());
  const auto cr = ao::compiler::compileMethod("ninth\n  ^9");
  ASSERT_TRUE(cr.ok);
  ao::Root sel(b.roots, b.wk.intern("ninth"));
  while (b.heap.allocateTenured(ao::Oop::nil(), 0, 0).isHeap()) {
  }
  leaveNurseryForMethodOnly(b, cr.image, cls.slot);

  const ao::Oop installed = ao::installMethod(b.ctx, cls.slot, cr.image);
  EXPECT_FALSE(installed.isHeap());
  EXPECT_FALSE(ao::lookup(b.heap, cls.slot, sel.slot).isHeap());
  const ao::Oop dict = b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict);
  EXPECT_EQ(ao::Oop::fromSmallInteger(8), b.heap.slotAt(dict, ao::kDictSlotTally));
  EXPECT_TRUE(ao::lookup(b.heap, cls.slot, b.wk.intern("m8")).isHeap());
}

// メソッド辞書の tally も Smalltalk から書き換えられる（クラスの instVarAt: 2 が辞書）。
// SmallInteger の最大値にしてからメソッドを足しても、atPut は tally を信じず数え直すので、
// +1 が範囲を超えることはない。登録は入り、tally は実際の数になる。
TEST(MethodDictionary, AtPutWithTallyAtSmiMaxRecountsAndInstalls) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #SmiMaxMethods\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'SmiRange'!\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named("SmiMaxMethods"));
  ASSERT_TRUE(cls.slot.isHeap());
  const ao::Oop max = ao::Oop::fromSmallInteger(ao::kSmiMax);
  ao::Root dict(b.roots, send1(b, cls.slot, "instVarAt:",
                               ao::Oop::fromSmallInteger(ao::kClassSlotMethodDict + 1)));
  ASSERT_EQ(b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict), dict.slot);
  ASSERT_EQ(max, send2(b, dict.slot, "instVarAt:put:",
                       ao::Oop::fromSmallInteger(ao::kDictSlotTally + 1), max));
  const auto cr = ao::compiler::compileMethod("one\n  ^1");
  ASSERT_TRUE(cr.ok);
  ao::Root sel(b.roots, b.wk.intern("one"));

  EXPECT_TRUE(ao::installMethod(b.ctx, cls.slot, cr.image).isHeap());
  EXPECT_TRUE(ao::lookup(b.heap, cls.slot, sel.slot).isHeap());
  EXPECT_EQ(ao::Oop::fromSmallInteger(1), b.heap.slotAt(dict.slot, ao::kDictSlotTally));
}

// 内側の配列は instVarAt:put: で奇数長に差し替えられ、tally は実際の数と食い違いうる。
// それでも atPut は空きの対を探し、空きが無ければ拡張して tally を数え直し、登録する（bindIn と同じ）。
TEST(MethodDictionary, AtPutGrowsWhenTallyLies) {
  Boot b;
  ao::Root dict(b.roots, ao::MethodDictionary::create(b.heap, b.wk, 1));
  ASSERT_TRUE(dict.slot.isHeap());
  ao::Root odd(b.roots, b.heap.allocateNoGc(ao::Oop::nil(), 3, 0));  // 対は 1 つ、余りが 1 スロット
  ASSERT_TRUE(odd.slot.isHeap());
  for (std::uint32_t i = 0; i < 3; ++i) {
    b.heap.slotAtPut(odd.slot, i, ao::Oop::nil());
  }
  b.heap.slotAtPut(dict.slot, ao::kDictSlotArray, odd.slot);
  ao::Root a(b.roots, b.wk.intern("lieA"));
  ao::Root c(b.roots, b.wk.intern("lieB"));
  ao::Root d(b.roots, b.wk.intern("lieC"));
  const ao::Oop one = ao::Oop::fromSmallInteger(1);
  const ao::Oop two = ao::Oop::fromSmallInteger(2);
  const ao::Oop three = ao::Oop::fromSmallInteger(3);

  EXPECT_TRUE(ao::MethodDictionary::atPut(b.heap, dict.slot, a.slot, one));
  // tally を 0 に偽る。空きの対は無いので、2 回目は拡張が要る。
  b.heap.slotAtPut(dict.slot, ao::kDictSlotTally, ao::Oop::fromSmallInteger(0));
  EXPECT_TRUE(ao::MethodDictionary::atPut(b.heap, dict.slot, c.slot, two));
  EXPECT_EQ(ao::Oop::fromSmallInteger(2), b.heap.slotAt(dict.slot, ao::kDictSlotTally));
  EXPECT_TRUE(ao::MethodDictionary::atPut(b.heap, dict.slot, d.slot, three));
  EXPECT_EQ(ao::Oop::fromSmallInteger(3), b.heap.slotAt(dict.slot, ao::kDictSlotTally));
  EXPECT_EQ(one, ao::MethodDictionary::at(b.heap, dict.slot, a.slot));
  EXPECT_EQ(two, ao::MethodDictionary::at(b.heap, dict.slot, c.slot));
  EXPECT_EQ(three, ao::MethodDictionary::at(b.heap, dict.slot, d.slot));
}

// セレクタはヒープの Symbol に限る。空 Oop（intern の失敗）や nil（空きスロットの印）、即値をキーに
// しても登録せず、false を返す。
TEST(MethodDictionary, AtPutRejectsNonHeapKey) {
  Boot b;
  ao::Root dict(b.roots, ao::MethodDictionary::create(b.heap, b.wk, 2));
  ASSERT_TRUE(dict.slot.isHeap());
  const ao::Oop meth = ao::Oop::fromSmallInteger(7);
  EXPECT_FALSE(ao::MethodDictionary::atPut(b.heap, dict.slot, ao::Oop{}, meth));
  EXPECT_FALSE(ao::MethodDictionary::atPut(b.heap, dict.slot, ao::Oop::nil(), meth));
  EXPECT_FALSE(ao::MethodDictionary::atPut(b.heap, dict.slot, ao::Oop::fromSmallInteger(3), meth));
  EXPECT_EQ(ao::Oop::fromSmallInteger(0), b.heap.slotAt(dict.slot, ao::kDictSlotTally));
  const ao::Oop inner = b.heap.slotAt(dict.slot, ao::kDictSlotArray);
  for (std::uint32_t i = 0; i < b.heap.size(inner); ++i) {
    EXPECT_TRUE(b.heap.slotAt(inner, i).isNil()) << "slot " << i;
  }
}

// SPEC §3.9 削除: the removed pair goes nil/nil and the tally drops by one; the other pairs stay
// where they are, and the next atPut takes the emptied pair.
TEST(MethodDictionary, RemoveKeyLeavesTheOthersFindableAndReusesThePair) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto dict = ao::MethodDictionary::create(heap, wk, 4);
  const ao::Oop a = ao::Symbol::intern(wk, "rkA");
  const ao::Oop b = ao::Symbol::intern(wk, "rkB");
  const ao::Oop c = ao::Symbol::intern(wk, "rkC");
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, a, ao::Oop::fromSmallInteger(1)));
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, b, ao::Oop::fromSmallInteger(2)));
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, c, ao::Oop::fromSmallInteger(3)));
  EXPECT_TRUE(ao::MethodDictionary::removeKey(heap, dict, b));
  EXPECT_TRUE(ao::MethodDictionary::at(heap, dict, b).isNil());
  EXPECT_EQ(1, ao::MethodDictionary::at(heap, dict, a).smallIntegerValue());
  EXPECT_EQ(3, ao::MethodDictionary::at(heap, dict, c).smallIntegerValue());
  EXPECT_EQ(2, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop inner = heap.slotAt(dict, ao::kDictSlotArray);
  EXPECT_TRUE(heap.slotAt(inner, 2).isNil());
  EXPECT_TRUE(heap.slotAt(inner, 3).isNil());
  // Not bound any more: false, and nothing changes.
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, dict, b));
  EXPECT_EQ(2, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop d = ao::Symbol::intern(wk, "rkD");
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, d, ao::Oop::fromSmallInteger(4)));
  EXPECT_EQ(d, heap.slotAt(inner, 2));
  EXPECT_EQ(3, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, ao::Oop::nil(), a));
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, dict, ao::Oop::fromSmallInteger(1)));
}

// SPEC §3.9 削除: a class's method dictionary can be replaced by anything (instVarAt:put:). A
// pointer object too small to hold the array slot, a bytes object, or a dictionary whose array is
// bytes answers false and changes nothing, without reading out of range.
TEST(MethodDictionary, RemoveKeyOnMalformedDictionaryAnswersFalse) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  const ao::Oop key = ao::Symbol::intern(wk, "rkMalformed");
  const ao::Oop empty = heap.allocateNoGc(ao::Oop::nil(), 0, 0);
  ASSERT_TRUE(empty.isHeap());
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, empty, key));
  const ao::Oop oneSlot = heap.allocateNoGc(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(oneSlot.isHeap());
  heap.slotAtPut(oneSlot, ao::kDictSlotTally, ao::Oop::fromSmallInteger(1));
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, oneSlot, key));
  EXPECT_EQ(1, heap.slotAt(oneSlot, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop bytes = heap.allocateNoGc(ao::Oop::nil(), 8, ao::kFlagBytes);
  ASSERT_TRUE(bytes.isHeap());
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, bytes, key));
  const ao::Oop bytesInner = heap.allocateNoGc(ao::Oop::nil(), 8, ao::kFlagBytes);
  ASSERT_TRUE(bytesInner.isHeap());
  const ao::Oop dict = heap.allocateNoGc(wk.methodDictionaryClass, 2, 0);
  ASSERT_TRUE(dict.isHeap());
  heap.slotAtPut(dict, ao::kDictSlotTally, ao::Oop::fromSmallInteger(1));
  heap.slotAtPut(dict, ao::kDictSlotArray, bytesInner);
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, dict, key));
  EXPECT_EQ(1, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
}

// SPEC §3.3: a slot rewritten with instVarAt:put: is no dictionary. pairArray answers nothing, at
// answers nil and atPut takes nothing, without reading out of range: an empty object, a one-slot
// object, a byte object, a two-slot object whose array is bytes, nil and an immediate.
TEST(MethodDictionary, MalformedDictionaryAnswersNilAndTakesNothing) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  const ao::Oop key = ao::Symbol::intern(wk, "mdMalformed");
  const ao::Oop value = ao::Oop::fromSmallInteger(5);
  const ao::Oop empty = heap.allocateNoGc(ao::Oop::nil(), 0, 0);
  const ao::Oop oneSlot = heap.allocateNoGc(ao::Oop::nil(), 1, 0);
  const ao::Oop bytes = heap.allocateNoGc(ao::Oop::nil(), 8, ao::kFlagBytes);
  const ao::Oop bytesInner = heap.allocateNoGc(ao::Oop::nil(), 8, ao::kFlagBytes);
  const ao::Oop bytesArrayDict = heap.allocateNoGc(wk.methodDictionaryClass, 2, 0);
  ASSERT_TRUE(empty.isHeap());
  ASSERT_TRUE(oneSlot.isHeap());
  ASSERT_TRUE(bytes.isHeap());
  ASSERT_TRUE(bytesInner.isHeap());
  ASSERT_TRUE(bytesArrayDict.isHeap());
  heap.slotAtPut(oneSlot, ao::kDictSlotTally, ao::Oop::fromSmallInteger(0));
  heap.slotAtPut(bytesArrayDict, ao::kDictSlotTally, ao::Oop::fromSmallInteger(0));
  heap.slotAtPut(bytesArrayDict, ao::kDictSlotArray, bytesInner);
  for (const ao::Oop bad : {empty, oneSlot, bytes, bytesArrayDict, ao::Oop::nil(),
                            ao::Oop::fromSmallInteger(3)}) {
    EXPECT_TRUE(ao::MethodDictionary::pairArray(heap, bad).isEmpty());
    EXPECT_TRUE(ao::MethodDictionary::at(heap, bad, key).isNil());
    EXPECT_FALSE(ao::MethodDictionary::atPut(heap, bad, key, value));
  }
  EXPECT_EQ(0, heap.slotAt(oneSlot, ao::kDictSlotTally).smallIntegerValue());
  EXPECT_EQ(0, heap.slotAt(bytesArrayDict, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop dict = ao::MethodDictionary::create(heap, wk, 2);
  ASSERT_TRUE(dict.isHeap());
  EXPECT_EQ(heap.slotAt(dict, ao::kDictSlotArray), ao::MethodDictionary::pairArray(heap, dict));
}

// review-agy C1 / SPEC §3.3: installMethod reads a method dictionary slot only on a class. On a
// String it answers the empty Oop and leaves the String's bytes as they were.
TEST(MethodDictionary, InstallMethodOnStringAnswersEmptyAndLeavesItIntact) {
  Boot b;
  const auto cr = ao::compiler::compileMethod("c1Probe\n  ^1");
  ASSERT_TRUE(cr.ok);
  ao::Root text(b.roots, ao::Str::fromUtf8(b.ctx, "abc"));
  EXPECT_TRUE(ao::installMethod(b.ctx, text.slot, cr.image).isEmpty());
  EXPECT_EQ("abc", ao::Str::toUtf8(b.heap, text.slot));
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(text.slot));
}

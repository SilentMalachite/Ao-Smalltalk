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
// SmallInteger の最大値にしてからメソッドを足すと、+1 が範囲を超える。abort せず、登録に失敗する。
TEST(MethodDictionary, AtPutWithTallyAtSmiMaxFails) {
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

  EXPECT_FALSE(ao::installMethod(b.ctx, cls.slot, cr.image).isHeap());
  EXPECT_FALSE(ao::lookup(b.heap, cls.slot, sel.slot).isHeap());
  EXPECT_EQ(max, b.heap.slotAt(dict.slot, ao::kDictSlotTally));
}

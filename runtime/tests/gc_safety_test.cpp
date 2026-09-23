// GC をまたぐネイティブの回帰テスト。docs/claude-review/03 と 04 の失敗シナリオをそのまま再現する。
// GC ストレス中は解放した領域を毒で埋めるので、古い番地を読めば即座に落ちる。
#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/HandleScope.hpp"

#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

// 03 の Money: `<` と `=` を Smalltalk で書いた Magnitude のサブクラス。
const char* kMoney =
    "!Magnitude subclass: #Money\n"
    "  instanceVariableNames: 'amount'\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'GcSafety'!\n"
    "!Money methodsFor: 'comparing'!\n"
    "amount: n\n"
    "  amount := n!\n"
    "amount\n"
    "  ^amount!\n"
    "< other\n"
    "  ^amount < other amount!\n"
    "= other\n"
    "  ^amount = other amount! !\n";

ao::Oop defineMoney(Boot& b) {
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_TRUE(ao::fileInString(b.ctx, kMoney, errs)) << (errs.empty() ? "" : errs[0].message);
  return b.wk.named("Money");
}

// `Money new amount: n`. GC ストレスはまだ入れないので、nursery に残る。
ao::Oop newMoney(Boot& b, ao::Oop money, std::int64_t n) {
  ao::Root m(b.roots, send0(b, money, "new"));
  send1(b, m.slot, "amount:", ao::Oop::fromSmallInteger(n));
  return m.slot;
}

}  // namespace

// レビューのループ本体 `a := Money new amount: 5. b := Money new amount: 5. a <= b` を回す。
// `<=` の間だけ GC ストレスを入れ、`<` の safepoint で必ず a と b を動かす。
TEST(GcSafety, MagnitudeLessOrEqualAcrossGc) {
  Boot b;
  b.heap.setGcStress(0);  // 定義と生成はストレスなしで行う（AO_GC_STRESS に左右されない）
  ao::Root money(b.roots, defineMoney(b));
  ASSERT_TRUE(money.slot.isHeap());
  int hits = 0;
  for (int i = 0; i < 50; ++i) {
    ao::Root x(b.roots, newMoney(b, money.slot, 5));
    ao::Root y(b.roots, newMoney(b, money.slot, 5));
    ASSERT_TRUE(b.heap.inNursery(x.slot));
    b.heap.setGcStress(1);
    const ao::Oop r = send1(b, x.slot, "<=", y.slot);
    b.heap.setGcStress(0);
    hits += r.isTrue() ? 1 : 0;
  }
  EXPECT_EQ(50, hits);
}

TEST(GcSafety, BetweenAndAcrossGc) {
  Boot b;
  b.heap.setGcStress(0);  // 定義と生成はストレスなしで行う（AO_GC_STRESS に左右されない）
  ao::Root money(b.roots, defineMoney(b));
  ASSERT_TRUE(money.slot.isHeap());
  int hits = 0;
  for (int i = 0; i < 50; ++i) {
    ao::Root x(b.roots, newMoney(b, money.slot, 5));
    ao::Root lo(b.roots, newMoney(b, money.slot, 3));
    ao::Root hi(b.roots, newMoney(b, money.slot, 7));
    ASSERT_TRUE(b.heap.inNursery(x.slot));
    b.heap.setGcStress(1);
    const ao::Oop r = send2(b, x.slot, "between:and:", lo.slot, hi.slot);
    b.heap.setGcStress(0);
    hits += r.isTrue() ? 1 : 0;
  }
  EXPECT_EQ(50, hits);
}

// 04: nursery にある Symbol に、nursery を満杯にしてから asString を送る。
TEST(GcSafety, SymbolAsStringAfterGc) {
  Boot b;
  b.heap.setGcStress(1);
  ao::Root sym(b.roots, ao::Symbol::intern(b.wk, "gcSafetyFreshSymbol"));
  ASSERT_TRUE(b.heap.inNursery(sym.slot));
  while (b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
  const ao::Oop s = send0(b, sym.slot, "asString");
  ASSERT_TRUE(s.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(s));
  EXPECT_EQ("gcSafetyFreshSymbol", ao::Str::toUtf8(b.heap, s));
}

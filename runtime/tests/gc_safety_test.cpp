// GC をまたぐネイティブの回帰テスト。docs/claude-review/03 と 04 の失敗シナリオをそのまま再現する。
// GC ストレス中は解放した領域を毒で埋めるので、古い番地を読めば即座に落ちる。
#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
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

namespace {

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

}  // namespace

// 04 Critical: 直前の GC 以降に作ったクラス A（nursery にある）を親にして、nursery の残りが 80 B
// 未満のときに `A subclass: #B …` を送る。最初の allocateRetry で A と名前のシンボルが動く。
TEST(GcSafety, SubclassKeepsSuperAndMetaSuper) {
  Boot b;
  b.heap.setGcStress(0);  // 定義と生成はストレスなしで行う（AO_GC_STRESS に左右されない）
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #GcSafetyA\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'GcSafety'!\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root a(b.roots, b.wk.named("GcSafetyA"));
  ASSERT_TRUE(b.heap.inNursery(a.slot));
  ao::RootedArray args(b.roots, 5);
  args[0] = b.wk.intern("GcSafetyB");
  args[1] = ao::Str::fromUtf8(b.heap, b.wk, "");
  args[2] = ao::Str::fromUtf8(b.heap, b.wk, "");
  args[3] = ao::Str::fromUtf8(b.heap, b.wk, "");
  args[4] = ao::Str::fromUtf8(b.heap, b.wk, "GcSafety");
  ASSERT_TRUE(b.heap.inNursery(args[0]));
  const ao::Oop sel = b.wk.intern(
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  const ao::Oop aBefore = a.slot;
  while (b.heap.nurseryRemaining() >= 80) {
    ASSERT_TRUE(b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap());
  }

  ao::Root created(b.roots, ao::send(b.ctx, a.slot, sel, args.ptr(), 5, nullptr));
  ASSERT_TRUE(created.slot.isHeap());
  ASSERT_NE(aBefore, a.slot);  // 前提: subclass: の途中で A が動いた

  const auto expectShape = [&](const char* when) {
    SCOPED_TRACE(when);
    const ao::Oop meta = b.heap.klass(created.slot);
    EXPECT_EQ(a.slot, b.heap.slotAt(created.slot, ao::kClassSlotSuperclass));
    EXPECT_EQ(b.wk.intern("GcSafetyB"), b.heap.slotAt(created.slot, ao::kClassSlotName));
    EXPECT_EQ(b.heap.klass(a.slot), b.heap.slotAt(meta, ao::kClassSlotSuperclass));
    EXPECT_EQ(created.slot, b.heap.slotAt(meta, ao::kClassSlotThisClass));
    EXPECT_EQ("GcSafety",
              ao::Str::toUtf8(b.heap, b.heap.slotAt(created.slot, ao::kClassSlotCategory)));
    EXPECT_EQ("GcSafety", ao::Str::toUtf8(b.heap, b.heap.slotAt(meta, ao::kClassSlotCategory)));
    EXPECT_EQ(created.slot, b.wk.named("GcSafetyB"));
  };
  expectShape("after subclass:");
  ao::Gc gc(b.heap, b.roots);
  gc.collectNursery();
  gc.collectNursery();
  expectShape("after two more scavenges");
}

// 01 High: nursery 半面（1 MiB）を超える Array を作り、コピーできる。
TEST(GcSafety, CopyArrayLargerThanNursery) {
  Boot b;
  constexpr std::int64_t kSize = 200000;  // 1.6 MB
  ao::Root arr(b.roots, send1(b, b.wk.arrayClass, "new:", smi(kSize)));
  ASSERT_TRUE(arr.slot.isHeap());
  EXPECT_EQ(smi(kSize), send0(b, arr.slot, "size"));
  send2(b, arr.slot, "at:put:", smi(1), smi(11));
  send2(b, arr.slot, "at:put:", smi(kSize), smi(22));

  ao::Root copy(b.roots, send0(b, arr.slot, "copy"));
  ASSERT_TRUE(copy.slot.isHeap());
  EXPECT_NE(arr.slot, copy.slot);
  EXPECT_TRUE(b.heap.inOld(copy.slot));
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(copy.slot));
  EXPECT_EQ(smi(kSize), send0(b, copy.slot, "size"));
  EXPECT_EQ(smi(11), send1(b, copy.slot, "at:", smi(1)));
  EXPECT_EQ(smi(22), send1(b, copy.slot, "at:", smi(kSize)));
  send2(b, copy.slot, "at:put:", smi(1), smi(33));
  EXPECT_EQ(smi(11), send1(b, arr.slot, "at:", smi(1)));
}

// 01 High の E2E を縮めたもの: 生存量が 4 MiB（old の初期容量）を超えたまま、40 KB の Array を
// 500 回差し替える。old は伸び、full GC は閾値（2×生存量）を超えるまで走らない。
TEST(GcSafety, LiveSetAbove4MiBDoesNotThrash) {
  Boot b;
  // 生存量に比例する GC を割り当てのたびに走らせると終わらないので、ストレスは切る。
  b.heap.setGcStress(0);
  constexpr std::int64_t kChunks = 100;
  constexpr std::int64_t kPer = 1500;  // 150000 個の 2 要素 Array（32 B）と 12 KB の chunk 100 個
  const auto start = std::chrono::steady_clock::now();
  ao::Root keep(b.roots, send1(b, b.wk.arrayClass, "new:", smi(kChunks)));
  ASSERT_TRUE(keep.slot.isHeap());
  for (std::int64_t c = 1; c <= kChunks; ++c) {
    ao::Root chunk(b.roots, send1(b, b.wk.arrayClass, "new:", smi(kPer)));
    ASSERT_TRUE(chunk.slot.isHeap()) << c;
    send2(b, keep.slot, "at:put:", smi(c), chunk.slot);
    for (std::int64_t k = 1; k <= kPer; ++k) {
      ao::Root pair(b.roots, send1(b, b.wk.arrayClass, "new:", smi(2)));
      ASSERT_TRUE(pair.slot.isHeap()) << c << " " << k;
      send2(b, pair.slot, "at:put:", smi(1), smi(k));
      send2(b, pair.slot, "at:put:", smi(2), smi(c));
      send2(b, chunk.slot, "at:put:", smi(k), pair.slot);
    }
  }
  ao::Gc(b.heap, b.roots).collectNursery();
  EXPECT_GT(b.heap.oldUsed(), std::size_t{4} << 20);

  const auto collectionsBefore = b.heap.oldCollections();
  ao::Root big(b.roots);
  for (int i = 0; i < 500; ++i) {
    big.slot = send1(b, b.wk.arrayClass, "new:", smi(5000));
    ASSERT_TRUE(big.slot.isHeap()) << i;
  }
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  EXPECT_LE(b.heap.oldCollections() - collectionsBefore, 1u);
  EXPECT_LT(ms, 30000);
  EXPECT_FALSE(b.heap.outOfMemory());

  const ao::Oop last = send1(b, send1(b, keep.slot, "at:", smi(kChunks)), "at:", smi(kPer));
  ASSERT_TRUE(last.isHeap());
  EXPECT_EQ(smi(kPer), send1(b, last, "at:", smi(1)));
  EXPECT_EQ(smi(kChunks), send1(b, last, "at:", smi(2)));
  std::printf("LiveSetAbove4MiB: %lld ms, old %zu bytes, %llu full collections\n",
              static_cast<long long>(ms), b.heap.oldUsed(),
              static_cast<unsigned long long>(b.heap.oldCollections()));
}

namespace {

// GC を走らせずに nursery を使い切る（残りは 16 B 未満）。
void fillNursery(Boot& b) {
  while (b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
}

// GC を走らせずに、nursery の残りをちょうど target バイトにする。16 B（空の pointers）と
// 24 B（8 バイトの bytes）で詰める。
void fillNurseryTo(Boot& b, std::size_t target) {
  while (b.heap.nurseryRemaining() > target) {
    const bool last24 = b.heap.nurseryRemaining() - target == 24;
    const ao::Oop filler = last24 ? b.heap.allocate(ao::Oop::nil(), 8, ao::kFlagBytes)
                                  : b.heap.allocate(ao::Oop::nil(), 0, 0);
    ASSERT_TRUE(filler.isHeap());
  }
  ASSERT_EQ(target, b.heap.nurseryRemaining());
}

// この send の間だけ GC ストレスを入れる。allocateRetry を通れば必ず nursery GC が走り、
// 解放した領域は毒で埋まる。
ao::Oop sendStressed0(Boot& b, ao::Oop rcvr, const char* sel) {
  b.heap.setGcStress(1);
  const ao::Oop r = send0(b, rcvr, sel);
  b.heap.setGcStress(0);
  return r;
}

}  // namespace

// 04 Medium: nursery が満杯のとき、shallowCopy と copy は GC してから複製する。
TEST(GcSafety, ShallowCopyWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // 定義と生成はストレスなしで行う（AO_GC_STRESS に左右されない）
  ao::Root arr(b.roots, send1(b, b.wk.arrayClass, "new:", smi(3)));
  send2(b, arr.slot, "at:put:", smi(1), smi(11));
  send2(b, arr.slot, "at:put:", smi(3), smi(33));
  ao::Root str(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "gcSafetyCopy"));
  ASSERT_TRUE(b.heap.inNursery(arr.slot));
  ASSERT_TRUE(b.heap.inNursery(str.slot));

  fillNursery(b);
  ao::Root arrCopy(b.roots, sendStressed0(b, arr.slot, "shallowCopy"));
  ASSERT_TRUE(arrCopy.slot.isHeap());
  EXPECT_NE(arr.slot, arrCopy.slot);
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(arrCopy.slot));
  EXPECT_EQ(smi(3), send0(b, arrCopy.slot, "size"));
  EXPECT_EQ(smi(11), send1(b, arrCopy.slot, "at:", smi(1)));
  EXPECT_EQ(smi(33), send1(b, arrCopy.slot, "at:", smi(3)));

  fillNursery(b);
  ao::Root strCopy(b.roots, sendStressed0(b, str.slot, "copy"));
  ASSERT_TRUE(strCopy.slot.isHeap());
  EXPECT_NE(str.slot, strCopy.slot);
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(strCopy.slot));
  EXPECT_EQ("gcSafetyCopy", ao::Str::toUtf8(b.heap, strCopy.slot));
}

// 04 Medium: nursery が満杯のとき、printString は GC してから文字列を作る。
TEST(GcSafety, PrintStringWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // 定義と生成はストレスなしで行う（AO_GC_STRESS に左右されない）
  ao::Root obj(b.roots, send0(b, b.wk.objectClass, "new"));
  ao::Root str(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "it's"));
  ASSERT_TRUE(b.heap.inNursery(obj.slot));
  const auto expectPrint = [&](ao::Oop rcvr, const char* expected) {
    SCOPED_TRACE(expected);
    fillNursery(b);
    const ao::Oop s = sendStressed0(b, rcvr, "printString");
    ASSERT_TRUE(s.isHeap());
    EXPECT_EQ(b.wk.stringClass, b.heap.klass(s));
    EXPECT_EQ(expected, ao::Str::toUtf8(b.heap, s));
  };
  expectPrint(obj.slot, "Object");
  expectPrint(str.slot, "'it''s'");
  expectPrint(smi(42), "42");
  expectPrint(ao::Oop::nil(), "nil");
  expectPrint(ao::Oop::true_(), "true");
  expectPrint(ao::Oop::false_(), "false");
  expectPrint(ao::Oop::fromCharacter(U'a'), "$a");
}

// 04 Medium: nursery が満杯のとき、asSymbol は新しい Symbol を old に置く（allocateTenured）。
TEST(GcSafety, AsSymbolWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // 定義と生成はストレスなしで行う（AO_GC_STRESS に左右されない）
  ao::Root str(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "gcSafetyFreshAsSymbol"));

  fillNursery(b);
  ao::Root sym(b.roots, sendStressed0(b, str.slot, "asSymbol"));
  ASSERT_TRUE(sym.slot.isHeap());
  EXPECT_EQ(b.wk.symbolClass, b.heap.klass(sym.slot));
  EXPECT_TRUE(b.heap.inOld(sym.slot));
  EXPECT_EQ("gcSafetyFreshAsSymbol", ao::Str::toUtf8(b.heap, sym.slot));
  EXPECT_EQ(sym.slot, b.wk.intern("gcSafetyFreshAsSymbol"));

  fillNursery(b);
  EXPECT_EQ(sym.slot, sendStressed0(b, str.slot, "asSymbol"));
}

// 03 Low: nursery が逼迫した状態で 2^62 のリテラルを含むメソッドをコンパイルしても、リテラルが
// 空にならない。残りをリテラル配列ちょうどにして、LargeInteger の箱詰めで GC を要求する。
TEST(GcSafety, LargeLiteralWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // 割り当ての途中で GC が走ると、nursery の残りを作れない
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #GcSafetyBig\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'GcSafety'!\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named("GcSafetyBig"));
  ASSERT_TRUE(cls.slot.isHeap());
  ao::Root inst(b.roots, send0(b, cls.slot, "new"));
  ao::Root sel(b.roots, b.wk.intern("big"));
  const auto cr = ao::compiler::compileMethod("big\n  ^4611686018427387904");
  ASSERT_TRUE(cr.ok) << cr.error.message;

  // installMethod で最初に割り当てるのはリテラル配列。その直後に nursery が空になる。
  fillNurseryTo(b, sizeof(ao::ObjectHeader) + 8 * cr.image.literals.size());
  ao::Root installed(b.roots, ao::installMethod(b.ctx, cls.slot, cr.image));
  ASSERT_TRUE(installed.slot.isHeap());

  const ao::Oop r = send0(b, inst.slot, "big");
  ASSERT_TRUE(ao::LargeInteger::isLarge(b.wk, r));
  bool fits = false;
  EXPECT_EQ(std::int64_t{1} << 62, ao::LargeInteger::asInt64IfFits(b.heap, b.wk, r, &fits));
  EXPECT_TRUE(fits);
}

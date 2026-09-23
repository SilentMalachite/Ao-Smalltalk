// GC をまたぐネイティブの回帰テスト。docs/claude-review/03 と 04 の失敗シナリオをそのまま再現する。
// GC ストレス中は解放した領域を毒で埋めるので、古い番地を読めば即座に落ちる。
#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/TestRunner.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
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

// 大きな object は nursery を通らず old に直置きされる。大きなゴミだけを作るループではスキャベンジが
// 起きず、full GC も走らないまま old が伸び続けていた（2000 回で 306 MiB）。SPEC §3.2 の第 2 契機
// （allocateRetry が直置きする前に oldUsed + サイズ > 閾値なら full GC）で、old は小さく収まる。
TEST(GcSafety, LargeGarbageLoopTriggersFullGc) {
  Boot b;
  const auto collectionsBefore = b.heap.oldCollections();
  for (int i = 0; i < 2000; ++i) {  // 160 KiB の Array を 2000 個（計 312 MiB）捨てる
    const ao::Oop garbage = ao::allocateRetry(b.ctx, b.wk.arrayClass, 20000, 0);
    ASSERT_TRUE(garbage.isHeap()) << i;
    ASSERT_TRUE(b.heap.inOld(garbage)) << i;
  }
  EXPECT_GT(b.heap.oldCollections(), collectionsBefore);
  EXPECT_LE(b.heap.oldCapacity(), std::size_t{16} << 20);

  // Smalltalk の `Array new: 20000` も同じ経路を通る。
  for (int i = 0; i < 300; ++i) {
    const ao::Oop garbage = send1(b, b.wk.arrayClass, "new:", smi(20000));
    ASSERT_TRUE(garbage.isHeap()) << i;
  }
  EXPECT_LE(b.heap.oldCapacity(), std::size_t{16} << 20);
  EXPECT_FALSE(b.heap.outOfMemory());
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

// 04 Low: `3 perform: #+ withArguments: (OrderedCollection new add: 4; yourself)` は、内部配列と
// 添字の 3 つを引数にして + を呼んでいた。引数は Array（とそのサブクラス）だけを受け付け、
// それ以外は error: の慣習どおりメッセージ文字列で失敗する。
TEST(GcSafety, PerformWithArgumentsRejectsOrderedCollection) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Array subclass: #GcSafetyArgs\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'GcSafety'!\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root plus(b.roots, b.wk.intern("+"));
  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ASSERT_TRUE(oc.slot.isHeap());
  send1(b, oc.slot, "add:", smi(4));
  ASSERT_EQ(smi(1), send0(b, oc.slot, "size"));

  const ao::Oop rejected = send2(b, smi(3), "perform:withArguments:", plus.slot, oc.slot);
  ASSERT_TRUE(rejected.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(rejected));
  EXPECT_EQ("perform:withArguments: expects an Array", ao::Str::toUtf8(b.heap, rejected));

  ao::Root arr(b.roots, send1(b, b.wk.arrayClass, "new:", smi(1)));
  send2(b, arr.slot, "at:put:", smi(1), smi(4));
  EXPECT_EQ(smi(7), send2(b, smi(3), "perform:withArguments:", plus.slot, arr.slot));

  ao::Root sub(b.roots, send1(b, b.wk.named("GcSafetyArgs"), "new:", smi(1)));
  ASSERT_TRUE(sub.slot.isHeap());
  send2(b, sub.slot, "at:put:", smi(1), smi(5));
  EXPECT_EQ(smi(8), send2(b, smi(3), "perform:withArguments:", plus.slot, sub.slot));
}

namespace {

// 1 MiB のゴミを old の先頭に置いてから起動する。ゴミの根を外したあとの最初の full GC で、起動時に
// 作った Symbol はすべて 1 MiB 手前へ動く。C++ のローカルに持った Symbol は古い番地を指したままになる。
struct GarbageFirstBoot {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk;
  ao::ClassMethodCache cache;
  ao::CallContext ctx;
  GarbageFirstBoot()
      : heap(1 << 20, 4 << 20, ao::kOldMaxBytes), wk(heap, roots), ctx{heap, roots, wk, &cache} {
    heap.setGcStress(0);
    cache.addRoots(roots);
    ao::Root garbage(roots, heap.allocateTenured(ao::Oop::nil(), 1 << 20, ao::kFlagBytes));
    ao::Bootstrap::run(heap, roots, wk);
  }
  ao::Oop send(ao::Oop rcvr, const char* sel, ao::Oop* args, std::uint32_t argc) {
    return ao::send(ctx, rcvr, wk.intern(sel), args, argc, nullptr);
  }
};

// 整数でない Interval の do: と size は、`>` と `+` を send して進む。
const char* kFloatInterval =
    "!Object subclass: #GcSafetyInterval\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'GcSafety'!\n"
    "!GcSafetyInterval methodsFor: 'running'!\n"
    "floats\n"
    "  ^Interval from: 1.0 to: 60.0 by: 1.0!\n"
    "blockInto: seen\n"
    "  ^[:x | seen add: x]! !\n";

// Interval と引数を GC ストレスなしで作り、selector の send の間だけストレスを入れる。ループの前に
// ストレスの GC は走らないので、4 回目のストレス GC（full GC）はループの途中で起き、Symbol を動かす。
ao::Oop sendToFloatInterval(GarbageFirstBoot& b, const char* sel, bool withBlock,
                            ao::Oop* seenOut) {
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_TRUE(ao::fileInString(b.ctx, kFloatInterval, errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root inst(b.roots, b.send(b.wk.named("GcSafetyInterval"), "new", nullptr, 0));
  ao::Root iv(b.roots, b.send(inst.slot, "floats", nullptr, 0));
  ao::Root seen(b.roots, b.send(b.wk.orderedCollectionClass, "new", nullptr, 0));
  ao::Root blk(b.roots, b.send(inst.slot, "blockInto:", &seen.slot, 1));
  EXPECT_TRUE(iv.slot.isHeap() && seen.slot.isHeap() && blk.slot.isHeap());
  const auto collectionsBefore = b.heap.oldCollections();
  b.heap.setGcStress(1);
  const ao::Oop r = b.send(iv.slot, sel, withBlock ? &blk.slot : nullptr, withBlock ? 1 : 0);
  b.heap.setGcStress(0);
  EXPECT_GT(b.heap.oldCollections(), collectionsBefore);
  *seenOut = seen.slot;
  return r;
}

}  // namespace

// Interval の do: は、`>` と `+` のセレクタを C++ のローカルに持ったまま send をまたいでいた。
// full GC の圧縮で Symbol が動くと、比較が true にならず 2^20 回まで回っていた。
TEST(GcSafety, FloatIntervalDoAcrossFullGc) {
  GarbageFirstBoot b;
  ao::Root seen(b.roots);
  sendToFloatInterval(b, "do:", true, &seen.slot);
  EXPECT_EQ(smi(60), b.send(seen.slot, "size", nullptr, 0));
}

TEST(GcSafety, FloatIntervalSizeAcrossFullGc) {
  GarbageFirstBoot b;
  ao::Root seen(b.roots);
  EXPECT_EQ(smi(60), sendToFloatInterval(b, "size", false, &seen.slot));
}

// SharedQueue の next は、取り出した値を C++ のローカルに持ったまま write の signal を送っていた。
// signal が待っているプロセスを起こし、scheduler の待ち行列を作り直す割り当てで GC が走ると、
// 値（キューから外したので他に参照がない）は古い番地のまま返っていた。
TEST(GcSafety, SharedQueueNextKeepsValueAcrossSignal) {
  Boot b;
  b.heap.setGcStress(0);  // 準備はストレスなしで行う（AO_GC_STRESS に左右されない）
  ao::Root p2(b.roots, send0(b, b.wk.processClass, "new"));
  ASSERT_TRUE(p2.slot.isHeap());
  send0(b, p2.slot, "resume");
  ao::Root q(b.roots, send0(b, b.wk.sharedQueueClass, "new"));
  ASSERT_TRUE(q.slot.isHeap());
  ao::Root value(b.roots, send1(b, b.wk.arrayClass, "new:", smi(1)));
  send2(b, value.slot, "at:put:", smi(1), smi(42));
  send1(b, q.slot, "nextPut:", value.slot);
  // write の excess は 0。今のプロセスを write で待たせ、p2 に切り替える。
  ao::Root write(b.roots, send1(b, q.slot, "instVarAt:", smi(3)));
  send0(b, write.slot, "wait");
  ASSERT_EQ(p2.slot, send0(b, b.wk.processor, "activeProcess"));
  // scheduler の待ち行列を捨てておき、signal からの resume に作り直させる（割り当て = GC）。
  send2(b, b.wk.processor, "instVarAt:put:", smi(1), ao::Oop::nil());

  b.heap.setGcStress(1);
  const ao::Oop got = send0(b, q.slot, "next");
  b.heap.setGcStress(0);
  EXPECT_EQ(value.slot, got);
}

// リテラル配列の箱詰めは、配列を割り当てた（GC した）あとで、値で受けた methodClass をルートに
// 載せていた。nursery にいたクラスが動くと、古い番地が根になり、次の full GC がそれをたどる。
// ストレスを 2 回に 1 回にして、リテラル配列の割り当てで最初の GC、要素の Float で 4 回目の
// GC（full GC）を起こす。
TEST(GcSafety, LiteralArrayKeepsMethodClassAcrossGc) {
  Boot b;
  b.heap.setGcStress(0);  // 準備はストレスなしで行う（AO_GC_STRESS に左右されない）
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #GcSafetyLiterals\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'GcSafety'!\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named("GcSafetyLiterals"));
  ASSERT_TRUE(cls.slot.isHeap());
  ASSERT_TRUE(b.heap.inNursery(cls.slot));
  const auto cr =
      ao::compiler::compileMethod("floats\n  ^#(1.5 2.5 3.5 4.5 5.5 6.5 7.5 8.5)");
  ASSERT_TRUE(cr.ok) << cr.error.message;

  b.heap.setGcStress(2);
  ao::Root installed(b.roots, ao::installMethod(b.ctx, cls.slot, cr.image));
  b.heap.setGcStress(0);
  ASSERT_TRUE(installed.slot.isHeap());
  ao::Root inst(b.roots, send0(b, cls.slot, "new"));
  ao::Root floats(b.roots, send0(b, inst.slot, "floats"));
  EXPECT_EQ(smi(8), send0(b, floats.slot, "size"));
}

// ao --test の assert:equals: は、失敗したとき printString のセレクタを C++ のローカルに持ったまま
// 2 回 send していた。1 回目の printString が full GC を起こすと、2 回目は動く前の番地で send する。
// 1 回目（実際の値の printString）で 4.8 MB の Array を作り、full GC を起こす。
TEST(GcSafety, TestRunnerFailureMessageAfterFullGc) {
  namespace fs = std::filesystem;
  const fs::path dir = fs::temp_directory_path() / "ao-gc-safety-test-runner";
  std::error_code ec;
  fs::remove_all(dir, ec);
  fs::create_directories(dir);
  struct Cleanup {
    fs::path path;
    ~Cleanup() {
      std::error_code ignore;
      fs::remove_all(path, ignore);
    }
  } cleanup{dir};
  {
    std::ofstream fail(dir / "noisy.st");
    fail << "SmalltalkImage new at: #GcSafetyMessage\n"
         << "  put: (self assert: GcSafetyNoisy new equals: 3).\n";
    ASSERT_TRUE(fail);
  }
  GarbageFirstBoot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx,
                               "!Object subclass: #GcSafetyNoisy\n"
                               "  instanceVariableNames: ''\n"
                               "  classVariableNames: ''\n"
                               "  poolDictionaries: ''\n"
                               "  category: 'GcSafety'!\n"
                               "!GcSafetyNoisy methodsFor: 'printing'!\n"
                               "printString\n"
                               "  Array new: 600000.\n"
                               "  ^'noisy'! !\n",
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  const auto collectionsBefore = b.heap.oldCollections();
  EXPECT_EQ(1, ao::runSmalltalkTests(b.ctx, dir.string()));
  EXPECT_EQ(1, b.ctx.testFailures);
  EXPECT_GT(b.heap.oldCollections(), collectionsBefore);
  const ao::Oop message = b.wk.named("GcSafetyMessage");
  ASSERT_TRUE(message.isHeap());
  ASSERT_EQ(b.wk.stringClass, b.heap.klass(message));
  EXPECT_EQ("noisy ~= 3", ao::Str::toUtf8(b.heap, message));
}

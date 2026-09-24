// GC をまたぐネイティブの回帰テスト。docs/claude-review/03 と 04 の失敗シナリオをそのまま再現する。
// GC ストレス中は解放した領域を毒で埋めるので、古い番地を読めば即座に落ちる。
#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Natives.hpp"
#include "ao/TestRunner.hpp"
#include "ao/kernel/Install.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
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
// それ以外は error: の慣習どおり、その文言で評価を中断する（SPEC §3.3）。
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

  EXPECT_TRUE(send2(b, smi(3), "perform:withArguments:", plus.slot, oc.slot).isEmpty());
  EXPECT_EQ("perform:withArguments: expects an Array", takeAbortReason(b));

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
    fail << "self assert: GcSafetyNoisy new equals: 3.\n";
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
  // SPEC §4.4: the mismatch aborts the file with the message as its reason, reported on stderr.
  testing::internal::CaptureStderr();
  EXPECT_EQ(1, ao::runSmalltalkTests(b.ctx, dir.string()));
  const std::string reported = testing::internal::GetCapturedStderr();
  EXPECT_EQ(1, b.ctx.testFailures);
  EXPECT_GT(b.heap.oldCollections(), collectionsBefore);
  EXPECT_NE(std::string::npos, reported.find("noisy.st: noisy ~= 3\n")) << reported;
}

namespace {

// nursery を使い切ってから send する。SPEC §3.3: 失敗は値を返さずに中断し、理由の文字列は GC して
// でも作る（空にも、out of memory にもしない）。
void expectErrorWithFullNursery(Boot& b, ao::Oop rcvr, const char* sel, const ao::Oop* args,
                                std::uint32_t argc, const char* message) {
  SCOPED_TRACE(sel);
  ao::Root r(b.roots, rcvr);
  fillNursery(b);
  const ao::Oop got = ao::send(b.ctx, r.slot, b.wk.intern(sel), args, argc, nullptr);
  EXPECT_TRUE(got.isEmpty());
  EXPECT_EQ(message, takeAbortReason(b));
}

}  // namespace

TEST(GcSafety, SubclassResponsibilityWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // nursery を満杯にしておくため、途中で GC を走らせない
  ao::Root obj(b.roots, send0(b, b.wk.objectClass, "new"));
  expectErrorWithFullNursery(b, obj.slot, "subclassResponsibility", nullptr, 0,
                             "subclassResponsibility");
  expectErrorWithFullNursery(b, obj.slot, "shouldNotImplement", nullptr, 0,
                             "shouldNotImplement");
}

TEST(GcSafety, DivisionByZeroWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // nursery を満杯にしておくため、途中で GC を走らせない
  const ao::Oop zero = smi(0);
  expectErrorWithFullNursery(b, smi(7), "//", &zero, 1, "division by zero");
}

TEST(GcSafety, StringIndexErrorWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // nursery を満杯にしておくため、途中で GC を走らせない
  ao::Root str(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "abc"));
  const ao::Oop index = smi(9);
  expectErrorWithFullNursery(b, str.slot, "at:", &index, 1, "at: index out of range");
}

namespace {

ao::Oop doubleIt(ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
  ao::Oop two = ao::Oop::fromSmallInteger(2);
  return ao::send(ctx, args[0], ctx.wk.intern("*"), &two, 1, nullptr);
}

ao::Oop isOdd(ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
  if (!args[0].isSmallInteger()) {
    return ao::Oop::false_();
  }
  return (args[0].smallIntegerValue() % 2) != 0 ? ao::Oop::true_() : ao::Oop::false_();
}

}  // namespace

// collect: が内部で作る thunk（makeNativeBlock）は GC しない割り当てだったので、nursery が満杯だと
// 結果の Array が nil のまま返った。結果の Array はちょうど入り、thunk は入らないようにする。
TEST(GcSafety, CollectWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // nursery を満杯にしておくため、途中で GC を走らせない
  ao::Oop slots[3] = {smi(1), smi(2), smi(3)};
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 3));
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, doubleIt, 1));
  ASSERT_TRUE(blk.slot.isHeap());
  fillNurseryTo(b, sizeof(ao::ObjectHeader) + 8 * 3);
  ao::Root r(b.roots, send1(b, arr.slot, "collect:", blk.slot));
  ASSERT_TRUE(r.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(r.slot));
  ASSERT_EQ(3u, b.heap.size(r.slot));
  EXPECT_EQ(smi(2), b.heap.slotAt(r.slot, 0));
  EXPECT_EQ(smi(4), b.heap.slotAt(r.slot, 1));
  EXPECT_EQ(smi(6), b.heap.slotAt(r.slot, 2));
}

// select: も最初に thunk を作る。nursery が満杯でも GC してから作り、正しい結果を返す。
TEST(GcSafety, SelectWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // nursery を満杯にしておくため、途中で GC を走らせない
  ao::Oop slots[3] = {smi(1), smi(2), smi(3)};
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 3));
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, isOdd, 1));
  ASSERT_TRUE(blk.slot.isHeap());
  fillNursery(b);
  ao::Root r(b.roots, send1(b, arr.slot, "select:", blk.slot));
  ASSERT_TRUE(r.slot.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(r.slot));
  ASSERT_EQ(2u, b.heap.size(r.slot));
  EXPECT_EQ(smi(1), b.heap.slotAt(r.slot, 0));
  EXPECT_EQ(smi(3), b.heap.slotAt(r.slot, 1));
}

namespace {

// 空のクラスを 1 つ定義する（file-in、GC ストレスは呼び出し側で止めておく）。
ao::Oop defineEmptyClass(Boot& b, const std::string& name) {
  std::vector<ao::compiler::CompileError> errs;
  const std::string src = "!Object subclass: #" + name +
                          "\n"
                          "  instanceVariableNames: ''\n"
                          "  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n"
                          "  category: 'GcSafety'!\n";
  EXPECT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
  return b.wk.named(name);
}

// installMethod が Symbol の intern より前に割り当てる分（リテラル配列とバイト列）。
std::size_t literalsAndBytes(Boot& b, const ao::compiler::MethodImage& image) {
  return b.heap.objectBytesFor(static_cast<std::uint32_t>(image.literals.size()), 0) +
         b.heap.objectBytesFor(static_cast<std::uint32_t>(image.bytes.size()), ao::kFlagBytes);
}

// GC を走らせずに old を上限まで使い切る（残りは 16 B 未満）。
void fillOld(Boot& b) {
  while (b.heap.allocateTenured(ao::Oop::nil(), 0, 0).isHeap()) {
  }
}

// 辞書のキーに空 Oop が混ざっていない。
void expectNoEmptyKey(Boot& b, ao::Oop dict) {
  const ao::Oop inner = b.heap.slotAt(dict, ao::kDictSlotArray);
  ASSERT_TRUE(inner.isHeap());
  for (std::uint32_t i = 0; i + 1 < b.heap.size(inner); i += 2) {
    EXPECT_FALSE(b.heap.slotAt(inner, i).isEmpty()) << "slot " << i;
  }
}

ao::Oop probeNative(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(1);
}

}  // namespace

// Symbol の intern は GC しない。nursery が満杯なら old に置く（SPEC §3.2）。以前は nursery だけを
// 見て空 Oop を返し、installMethod は空のセレクタで辞書に登録して成功を返していた（呼べば DNU）。
// nursery の残りを「リテラル配列＋バイト列＋16 B」にして、新しいセレクタの Symbol だけを入らなくする。
TEST(GcSafety, InstallMethodInternsFreshSelectorWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // 割り当ての途中で GC が走ると、nursery の残りを作れない
  ao::Root cls(b.roots, defineEmptyClass(b, "GcSafetyFreshSelector"));
  ASSERT_TRUE(cls.slot.isHeap());
  ao::Root inst(b.roots, send0(b, cls.slot, "new"));
  const auto cr = ao::compiler::compileMethod("zzProbeFreshSelector42\n  ^42");
  ASSERT_TRUE(cr.ok) << cr.error.message;

  fillNurseryTo(b, literalsAndBytes(b, cr.image) + 16);
  ao::Root installed(b.roots, ao::installMethod(b.ctx, cls.slot, cr.image));
  ASSERT_TRUE(installed.slot.isHeap());
  const ao::Oop sel = b.heap.slotAt(installed.slot, ao::kCmSlotSelector);
  ASSERT_TRUE(sel.isHeap());
  EXPECT_EQ(b.wk.symbolClass, b.heap.klass(sel));
  EXPECT_EQ(sel, b.wk.intern("zzProbeFreshSelector42"));
  expectNoEmptyKey(b, b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict));

  EXPECT_EQ(smi(42), send0(b, inst.slot, "zzProbeFreshSelector42"));
  ao::Root sels(b.roots, send0(b, cls.slot, "selectors"));
  ASSERT_TRUE(sels.slot.isHeap());
  ASSERT_EQ(1u, b.heap.size(sels.slot));
  EXPECT_EQ(b.wk.intern("zzProbeFreshSelector42"), b.heap.slotAt(sels.slot, 0));
}

// subclass: も instVarNames を intern する。nursery の残りがクラス一式と名前の配列の分だけでも、
// 名前の Symbol は old に置かれ、定義は成功する（以前は intern の空 Oop で subclass: が失敗していた）。
TEST(GcSafety, SubclassInternsFreshInstVarNamesWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);  // 割り当ての途中で GC が走ると、nursery の残りを作れない
  ao::Root name(b.roots, b.wk.intern("GcSafetyFreshIvars"));
  ao::Root ivars(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "zzProbeFreshIvar42"));
  ao::Root empty(b.roots, ao::Str::fromUtf8(b.heap, b.wk, ""));
  ao::Root cat(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "GcSafety"));
  ao::Root sel(b.roots, b.wk.intern(
                            "subclass:instanceVariableNames:classVariableNames:poolDictionaries:"
                            "category:"));
  ASSERT_TRUE(name.slot.isHeap() && ivars.slot.isHeap() && empty.slot.isHeap() &&
              cat.slot.isHeap() && sel.slot.isHeap());

  // クラスとメタクラス、2 つのメソッド辞書（外側と 16 スロットの内側）、名前 1 つの配列は入り、
  // 名前の Symbol は入らない。
  const std::size_t classBytes = b.heap.objectBytesFor(ao::kClassSlotCount, 0);
  const std::size_t dictBytes = b.heap.objectBytesFor(2, 0) + b.heap.objectBytesFor(16, 0);
  fillNurseryTo(b, 2 * classBytes + 2 * dictBytes + b.heap.objectBytesFor(1, 0) + 16);
  const ao::Oop args[5] = {name.slot, ivars.slot, empty.slot, empty.slot, cat.slot};
  ao::Root created(b.roots, ao::send(b.ctx, b.wk.objectClass, sel.slot, args, 5, nullptr));
  ASSERT_TRUE(created.slot.isHeap());
  EXPECT_EQ(created.slot, b.wk.named("GcSafetyFreshIvars"));
  const ao::Oop names = b.heap.slotAt(created.slot, ao::kClassSlotInstVarNames);
  ASSERT_TRUE(names.isHeap());
  ASSERT_EQ(1u, b.heap.size(names));
  EXPECT_EQ(b.wk.intern("zzProbeFreshIvar42"), b.heap.slotAt(names, 0));
}

// nursery が満杯で old も上限なら、新しいセレクタは intern できない。installMethod は空 Oop を返し、
// 辞書には何も登録しない（空のキーを入れない）。
TEST(GcSafety, InstallMethodFailsWhenSelectorCannotBeInterned) {
  Boot b(1 << 20, 1 << 20, 1 << 20);  // old は 1 MiB で頭打ち
  b.heap.setGcStress(0);
  ao::Root cls(b.roots, defineEmptyClass(b, "GcSafetyNoSelector"));
  ASSERT_TRUE(cls.slot.isHeap());
  const auto cr = ao::compiler::compileMethod("zzProbeUninternedSelector42\n  ^42");
  ASSERT_TRUE(cr.ok) << cr.error.message;
  ao::Root dict(b.roots, b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict));
  const ao::Oop tally = b.heap.slotAt(dict.slot, ao::kDictSlotTally);

  fillOld(b);
  fillNurseryTo(b, literalsAndBytes(b, cr.image) + 16);
  EXPECT_FALSE(ao::installMethod(b.ctx, cls.slot, cr.image).isHeap());
  EXPECT_TRUE(b.heap.outOfMemory());
  EXPECT_EQ(tally, b.heap.slotAt(dict.slot, ao::kDictSlotTally));
  expectNoEmptyKey(b, dict.slot);
}

// putNative も同じ。NativeMethod 一式は入るが、それより大きい新しいセレクタの Symbol は入らない。
// 以前は空のセレクタで登録して true を返していた。
TEST(GcSafety, PutNativeFailsWhenSelectorCannotBeInterned) {
  Boot b(1 << 20, 1 << 20, 1 << 20);  // old は 1 MiB で頭打ち
  b.heap.setGcStress(0);
  ao::Root cls(b.roots, defineEmptyClass(b, "GcSafetyNoNativeSelector"));
  ASSERT_TRUE(cls.slot.isHeap());
  const std::string selector(100, 'z');
  const std::string_view nativeName = "ao_GcSafetyProbe_native";
  const std::size_t nativeBytes =
      b.heap.objectBytesFor(ao::kNativeSlotCount, 0) +
      b.heap.objectBytesFor(static_cast<std::uint32_t>(nativeName.size()), ao::kFlagBytes);
  ASSERT_GT(b.heap.objectBytesFor(static_cast<std::uint32_t>(selector.size()), ao::kFlagBytes),
            nativeBytes);
  ao::Root dict(b.roots, b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict));
  const ao::Oop tally = b.heap.slotAt(dict.slot, ao::kDictSlotTally);

  fillOld(b);
  fillNurseryTo(b, nativeBytes);
  EXPECT_FALSE(
      ao::kernel::putNative(b.heap, b.wk, cls.slot, selector, 0, nativeName, probeNative));
  EXPECT_EQ(tally, b.heap.slotAt(dict.slot, ao::kDictSlotTally));
  expectNoEmptyKey(b, dict.slot);
}

namespace {

// `Name superclass: value`（instVarAt: 1 put:）で親の枠を書き換えたクラスを作る。
ao::Oop defineWithSuperclassSlot(Boot& b, const char* superName, const std::string& name,
                                 ao::Oop super) {
  ao::Root superRoot(b.roots, super);  // file-in は GC しうる。値で受けた super を先にルートする
  std::vector<ao::compiler::CompileError> errs;
  const std::string src = std::string("!") + superName + " subclass: #" + name +
                          "\n"
                          "  instanceVariableNames: ''\n"
                          "  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n"
                          "  category: 'GcSafety'!\n";
  EXPECT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named(name));
  EXPECT_TRUE(cls.slot.isHeap());
  if (!cls.slot.isHeap()) {
    return cls.slot;
  }
  ao::Root value(b.roots, superRoot.slot.isEmpty() ? cls.slot : superRoot.slot);
  EXPECT_EQ(value.slot, send2(b, cls.slot, "instVarAt:put:", smi(1), value.slot));
  return cls.slot;
}

void expectPerformRejects(Boot& b, ao::Oop argsObj) {
  ao::Root args(b.roots, argsObj);
  ao::Root plus(b.roots, b.wk.intern("+"));
  EXPECT_TRUE(send2(b, smi(3), "perform:withArguments:", plus.slot, args.slot).isEmpty());
  EXPECT_EQ("perform:withArguments: expects an Array", takeAbortReason(b));
}

}  // namespace

// B1 の退行: perform:withArguments: は Array の子孫かどうかだけを見て、バイト列の検査を落とした。
// String の子の親を Array に書き換えると、そのバイト列を slotAt で読んで落ちた（Release では任意の
// バイトを Oop として send に渡した）。引数はポインタオブジェクトに限る。
TEST(GcSafety, PerformWithArgumentsRejectsBytesUnderArray) {
  Boot b;
  ao::Root pbytes(b.roots, defineWithSuperclassSlot(b, "String", "GcSafetyPBytes", b.wk.arrayClass));
  ASSERT_TRUE(pbytes.slot.isHeap());
  ao::Root inst(b.roots, send1(b, pbytes.slot, "new:", smi(16)));
  ASSERT_TRUE(inst.slot.isHeap());
  ASSERT_NE(0, b.heap.flags(inst.slot) & ao::kFlagBytes);
  expectPerformRejects(b, inst.slot);
}

// 親の鎖が壊れていても（自分自身への循環、バイト列、スロットの無いオブジェクト）、Array の子孫かを
// 調べる走査は止まり、引数を拒む。
TEST(GcSafety, PerformWithArgumentsStopsOnBrokenSuperclassChain) {
  Boot b;
  ao::Root abc(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "abc"));
  ao::Root none(b.roots, send1(b, b.wk.arrayClass, "new:", smi(0)));
  ao::Root toBytes(b.roots, defineWithSuperclassSlot(b, "Object", "GcSafetySuperBytes", abc.slot));
  ao::Root toEmpty(b.roots, defineWithSuperclassSlot(b, "Object", "GcSafetySuperEmpty", none.slot));
  ao::Root toSelf(b.roots, defineWithSuperclassSlot(b, "Object", "GcSafetySuperSelf", ao::Oop{}));
  for (const ao::Oop cls : {toBytes.slot, toEmpty.slot, toSelf.slot}) {
    ASSERT_TRUE(cls.isHeap());
  }
  const std::pair<const char*, ao::Oop*> cases[] = {
      {"superclass is bytes", &toBytes.slot},
      {"superclass has no slots", &toEmpty.slot},
      {"superclass is itself", &toSelf.slot},
  };
  for (const auto& [label, cls] : cases) {
    SCOPED_TRACE(label);
    ao::Root inst(b.roots, send0(b, *cls, "new"));
    ASSERT_TRUE(inst.slot.isHeap());
    expectPerformRejects(b, inst.slot);
  }
}

namespace {

// SPEC §3.3: 壊れたスーパークラス鎖の 3 通り。親の枠は C++ から直接書き換え、テストごとに Boot を作る。
enum class BrokenParent { Bytes, FewSlots, Itself };

struct BrokenCase {
  const char* label;
  BrokenParent parent;
};

constexpr BrokenCase kBrokenCases[] = {
    {"superclass is bytes", BrokenParent::Bytes},
    {"superclass has too few slots", BrokenParent::FewSlots},
    {"superclass is itself", BrokenParent::Itself},
};

// `Object subclass: #ChainProbe instanceVariableNames: 'x'` と methods を file-in してから、親の枠を
// 壊す。クラス側の鎖（ChainProbe class から上）は壊さないので、new などのクラス側の送信は届く。
ao::Oop defineBrokenClass(Boot& b, BrokenParent parent, const std::string& methods = "") {
  std::vector<ao::compiler::CompileError> errs;
  const std::string src =
      "!Object subclass: #ChainProbe\n"
      "  instanceVariableNames: 'x'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'BrokenChain'!\n" +
      methods;
  EXPECT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named("ChainProbe"));
  if (!cls.slot.isHeap()) {
    return cls.slot;
  }
  ao::Root parentObj(b.roots, cls.slot);
  if (parent == BrokenParent::Bytes) {
    parentObj.slot = ao::Str::fromUtf8(b.heap, b.wk, "bad");
  } else if (parent == BrokenParent::FewSlots) {
    // 親の枠は Object を指すが、クラスの枠（kClassSlotCount 個）には足りない。
    parentObj.slot = send1(b, b.wk.arrayClass, "new:", smi(2));
    b.heap.slotAtPut(parentObj.slot, ao::kClassSlotSuperclass, b.wk.objectClass);
  }
  b.heap.slotAtPut(cls.slot, ao::kClassSlotSuperclass, parentObj.slot);
  return cls.slot;
}

// x に 7 を入れた ChainProbe のインスタンス。
ao::Oop newProbe(Boot& b, ao::Oop cls) {
  ao::Root inst(b.roots, send0(b, cls, "new"));
  EXPECT_TRUE(inst.slot.isHeap());
  if (inst.slot.isHeap()) {
    b.heap.slotAtPut(inst.slot, 0, smi(7));
  }
  return inst.slot;
}

// インスタンス側の鎖が壊れていると Object のメソッドへ送信が届かないので、ネイティブを直接呼ぶ。
ao::Oop callNative(Boot& b, ao::NativeFn fn, ao::Oop receiver, ao::Oop arg) {
  return ao::NativeMethod::invoke(b.ctx, fn, receiver, &arg, 1);
}

}  // namespace

// Codex の再現: `Object instVarAt: 1 put: Object. Object new isKindOf: UndefinedObject` が止まらなかった。
TEST(BrokenSuperclassChain, ObjectAsItsOwnSuperclassStops) {
  Boot b;
  ao::Root object(b.roots, b.wk.objectClass);
  ASSERT_EQ(object.slot, send2(b, object.slot, "instVarAt:put:", smi(1), object.slot));
  ao::Root inst(b.roots, send0(b, object.slot, "new"));
  ASSERT_TRUE(inst.slot.isHeap());
  EXPECT_TRUE(send1(b, inst.slot, "isKindOf:", b.wk.undefinedObjectClass).isFalse());
  EXPECT_TRUE(send1(b, inst.slot, "isKindOf:", object.slot).isTrue());
  EXPECT_TRUE(send1(b, object.slot, "inheritsFrom:", b.wk.undefinedObjectClass).isFalse());
  ao::Root missing(b.roots, b.wk.intern("chainProbeMissing"));
  EXPECT_TRUE(send1(b, inst.slot, "respondsTo:", missing.slot).isFalse());
  // 無いセレクタの探索は鎖の上限で止まり、Object の doesNotUnderstand: が評価を中断する（SPEC §3.3）。
  EXPECT_TRUE(send0(b, inst.slot, "chainProbeMissing").isEmpty());
  EXPECT_EQ("doesNotUnderstand: #chainProbeMissing", takeAbortReason(b));
}

// isKindOf: と inheritsFrom: は壊れた所で止まり、その先（Object）を見ない。
TEST(BrokenSuperclassChain, IsKindOfAndInheritsFromStop) {
  for (const auto& c : kBrokenCases) {
    SCOPED_TRACE(c.label);
    Boot b;
    ao::Root cls(b.roots, defineBrokenClass(b, c.parent));
    ASSERT_TRUE(cls.slot.isHeap());
    ao::Root inst(b.roots, newProbe(b, cls.slot));
    ASSERT_TRUE(inst.slot.isHeap());
    EXPECT_TRUE(callNative(b, ao::ao_Object_isKindOf_, inst.slot, cls.slot).isTrue());
    EXPECT_TRUE(callNative(b, ao::ao_Object_isKindOf_, inst.slot, b.wk.objectClass).isFalse());
    EXPECT_TRUE(
        callNative(b, ao::ao_Object_isKindOf_, inst.slot, b.wk.undefinedObjectClass).isFalse());
    EXPECT_TRUE(send1(b, cls.slot, "inheritsFrom:", b.wk.objectClass).isFalse());
    EXPECT_TRUE(send1(b, cls.slot, "inheritsFrom:", b.wk.undefinedObjectClass).isFalse());
  }
}

// instVarNamed: は壊れた所までのクラスの変数名だけを見る。ChainProbe 自身の x は見つかる。
TEST(BrokenSuperclassChain, InstVarNamedStops) {
  for (const auto& c : kBrokenCases) {
    SCOPED_TRACE(c.label);
    Boot b;
    ao::Root cls(b.roots, defineBrokenClass(b, c.parent));
    ASSERT_TRUE(cls.slot.isHeap());
    ao::Root inst(b.roots, newProbe(b, cls.slot));
    ASSERT_TRUE(inst.slot.isHeap());
    ao::Root x(b.roots, b.wk.intern("x"));
    EXPECT_EQ(smi(7), callNative(b, ao::ao_Object_instVarNamed_, inst.slot, x.slot));
    ao::Root missing(b.roots, b.wk.intern("missing"));
    // SPEC §3.3: error: の失敗は値を返さず、その文言で評価を中断する。
    EXPECT_TRUE(callNative(b, ao::ao_Object_instVarNamed_, inst.slot, missing.slot).isEmpty());
    EXPECT_EQ("instVarNamed: not found", takeAbortReason(b));
  }
}

// 送信の探索は壊れた所で打ち切る。ChainProbe 自身のメソッドは見つかる。無いセレクタは
// doesNotUnderstand: も見つからないので、既定と同じく評価を中断する（SPEC §3.3）。super 送信も、
// String の = が引数のクラスを調べる走査も止まる。
TEST(BrokenSuperclassChain, SendAndDoesNotUnderstandStop) {
  for (const auto& c : kBrokenCases) {
    SCOPED_TRACE(c.label);
    Boot b;
    ao::Root cls(b.roots, defineBrokenClass(b, c.parent,
                                            "!ChainProbe methodsFor: 'probe'!\n"
                                            "getX\n"
                                            "  ^x!\n"
                                            "superMissing\n"
                                            "  ^super chainProbeMissing! !\n"));
    ASSERT_TRUE(cls.slot.isHeap());
    ao::Root inst(b.roots, newProbe(b, cls.slot));
    ASSERT_TRUE(inst.slot.isHeap());
    EXPECT_EQ(smi(7), send0(b, inst.slot, "getX"));
    EXPECT_TRUE(send0(b, inst.slot, "chainProbeMissing").isEmpty());
    EXPECT_EQ("doesNotUnderstand: #chainProbeMissing", takeAbortReason(b));
    EXPECT_TRUE(send0(b, inst.slot, "superMissing").isEmpty());
    EXPECT_EQ("doesNotUnderstand: #chainProbeMissing", takeAbortReason(b));
    ao::Root missing(b.roots, b.wk.intern("chainProbeMissing"));
    EXPECT_TRUE(callNative(b, ao::ao_Object_respondsTo_, inst.slot, missing.slot).isFalse());
    ao::Root getX(b.roots, b.wk.intern("getX"));
    EXPECT_TRUE(callNative(b, ao::ao_Object_respondsTo_, inst.slot, getX.slot).isTrue());
    // 壊れた所より先（Object の ==）は無いものとして扱う。
    ao::Root identity(b.roots, b.wk.intern("=="));
    EXPECT_TRUE(callNative(b, ao::ao_Object_respondsTo_, inst.slot, identity.slot).isFalse());
    ao::Root str(b.roots, ao::Str::fromUtf8(b.heap, b.wk, "abc"));
    EXPECT_TRUE(send1(b, str.slot, "=", inst.slot).isFalse());
  }
}

// コンパイル: インスタンス変数を読むメソッドの accept と file-in は、壊れた所までの変数名で解決する。
// ChainProbe 自身の x は解決でき、実行すると 7 を返す。
TEST(BrokenSuperclassChain, CompileInstVarReferenceStops) {
  for (const auto& c : kBrokenCases) {
    SCOPED_TRACE(c.label);
    Boot b;
    ao::Root cls(b.roots, defineBrokenClass(b, c.parent));
    ASSERT_TRUE(cls.slot.isHeap());
    ao::compiler::CompileError err;
    EXPECT_TRUE(ao::acceptMethodSource(b.ctx, "ChainProbe", false, "readX\n  ^x", &err))
        << err.message;
    std::vector<ao::compiler::CompileError> errs;
    EXPECT_TRUE(ao::fileInString(b.ctx,
                                 "!ChainProbe methodsFor: 'probe'!\n"
                                 "writeX: v\n"
                                 "  x := v! !\n",
                                 errs))
        << (errs.empty() ? "" : errs[0].message);
    ao::Root inst(b.roots, newProbe(b, cls.slot));
    ASSERT_TRUE(inst.slot.isHeap());
    EXPECT_EQ(smi(7), send0(b, inst.slot, "readX"));
    send1(b, inst.slot, "writeX:", smi(9));
    EXPECT_EQ(smi(9), send0(b, inst.slot, "readX"));
  }
}

// SPEC §3.2: allocateRetry が old の上限で諦めるまでに走らせる full GC は 1 回まで。以前は大きな
// object の閾値の full GC（第 2 契機）と、上限での full GC（第 3 契機）が重なって 2 回走った。
// old の上限より大きな要求は、GC せずに out of memory にする。
TEST(GcSafety, OutOfMemoryRunsAtMostOneFullGc) {
  Boot b(1 << 20, 1 << 20, 2 << 20);  // old は 2 MiB で頭打ち
  b.heap.setGcStress(0);
  // 生きている object で old をほぼ埋める（full GC をしても空かない）。残りは 32 KiB 弱。
  const std::size_t room = b.heap.oldMaxBytes() - b.heap.oldUsed();
  ASSERT_GT(room, std::size_t{64} << 10);
  ao::Root live(b.roots, b.heap.allocateTenured(ao::Oop::nil(),
                                                static_cast<std::uint32_t>(room - (32 << 10)),
                                                ao::kFlagBytes));
  ASSERT_TRUE(live.slot.isHeap());

  const std::uint64_t before = b.heap.oldCollections();
  EXPECT_FALSE(ao::allocateRetry(b.ctx, ao::Oop::nil(), 64 << 10, ao::kFlagBytes).isHeap());
  EXPECT_TRUE(b.heap.outOfMemory());
  EXPECT_LE(b.heap.oldCollections() - before, 1u);
  EXPECT_TRUE(live.slot.isHeap());

  b.heap.clearOutOfMemory();
  const std::uint64_t beforeHuge = b.heap.oldCollections();
  EXPECT_FALSE(ao::allocateRetry(b.ctx, ao::Oop::nil(), 3u << 20, ao::kFlagBytes).isHeap());
  EXPECT_TRUE(b.heap.outOfMemory());
  EXPECT_EQ(beforeHuge, b.heap.oldCollections());
}

// GC しない割り当ても、old の上限で失敗したら out of memory のフラグを立てる（SPEC §3.2）。以前は
// allocateRetry だけが立てたので、collect: の thunk の NativeMethod が作れないと、nil で埋めた配列が
// 黙って返った。結果の配列と thunk のブロックは入り、NativeMethod は入らない nursery の残りにする。
TEST(GcSafety, CollectThunkMethodFailureSetsOutOfMemory) {
  Boot b(1 << 20, 1 << 20, 1 << 20);  // old は 1 MiB で頭打ち
  b.heap.setGcStress(0);  // nursery の残りを作るため、途中で GC を走らせない
  ao::Oop slots[3] = {smi(1), smi(2), smi(3)};
  ao::Root arr(b.roots, ao::Arr::fromSlots(b.heap, b.wk, slots, 3));
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, doubleIt, 1));
  ao::Root sel(b.roots, b.wk.intern("collect:"));
  ASSERT_TRUE(arr.slot.isHeap() && blk.slot.isHeap() && sel.slot.isHeap());

  fillOld(b);
  fillNurseryTo(b, b.heap.objectBytesFor(3, 0) + b.heap.objectBytesFor(ao::kBlockSlotCount, 0) + 16);
  ASSERT_FALSE(b.heap.outOfMemory());
  ao::send(b.ctx, arr.slot, sel.slot, &blk.slot, 1, nullptr);
  EXPECT_TRUE(b.heap.outOfMemory());
}

// Symbol の intern、メソッド辞書の作成と拡張も同じ。割り当て以外の理由の失敗（キーがヒープでない）
// ではフラグを立てない。
TEST(GcSafety, NoGcAllocationFailuresSetOutOfMemory) {
  Boot b(1 << 20, 1 << 20, 1 << 20);  // old は 1 MiB で頭打ち
  b.heap.setGcStress(0);
  ao::Root dict(b.roots, ao::MethodDictionary::create(b.heap, b.wk, 1));
  ao::Root key(b.roots, b.wk.intern("zzOomFirstKey"));
  ao::Root grownKey(b.roots, b.wk.intern("zzOomSecondKey"));
  ASSERT_TRUE(dict.slot.isHeap() && key.slot.isHeap() && grownKey.slot.isHeap());
  ASSERT_TRUE(ao::MethodDictionary::atPut(b.heap, dict.slot, key.slot, smi(1)));
  fillOld(b);
  fillNursery(b);

  EXPECT_FALSE(ao::MethodDictionary::atPut(b.heap, dict.slot, ao::Oop{}, smi(2)));
  EXPECT_FALSE(b.heap.outOfMemory());

  EXPECT_FALSE(b.wk.intern("zzOomFreshSymbolThatDoesNotFit").isHeap());
  EXPECT_TRUE(b.heap.outOfMemory());
  b.heap.clearOutOfMemory();

  EXPECT_FALSE(ao::MethodDictionary::create(b.heap, b.wk, 8).isHeap());
  EXPECT_TRUE(b.heap.outOfMemory());
  b.heap.clearOutOfMemory();

  EXPECT_FALSE(ao::MethodDictionary::atPut(b.heap, dict.slot, grownKey.slot, smi(2)));
  EXPECT_TRUE(b.heap.outOfMemory());
}

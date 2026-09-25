// SPEC §3.6 Collections (B9): Dictionary と Set のハッシュ表、classPool の配置と名前の並び、
// Interval の刻みと終端の比較、OrderedCollection>>at: の範囲。docs/claude-review/04 の失敗シナリオを
// そのまま回帰テストにする。

#include "ao_abi.h"

#include "test_support.hpp"

#include "ao/ClassPool.hpp"
#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/HashedCollection.hpp"
#include "ao/Natives.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <map>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace {

class HashedCollection : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  void TearDown() override { ao_runtime_shutdown(); }

  // The Print it of src, or "<eval error: reason>" when the evaluation aborts (SPEC §3.3).
  std::string printIt(const std::string& src) {
    char out[512] = {};
    AoSpan err{};
    const int rc = ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT, out,
                           sizeof out, &err);
    if (rc == AO_ERR_EVAL) {
      return std::string("<eval error: ") + err.message + ">";
    }
    if (rc != AO_OK) {
      return "<rc " + std::to_string(rc) + ": " + err.message + ">";
    }
    return out;
  }
};

void acceptClass(const char* superclass, const char* name, const char* ivars,
                 const char* classVars = "") {
  const std::string def = std::string(superclass) + " subclass: #" + name +
                          "\n  instanceVariableNames: '" + ivars + "'\n  classVariableNames: '" +
                          classVars + "'\n  poolDictionaries: ''\n  category: 'B9-Test'\n";
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
}

void acceptMethod(const char* className, const char* source, int meta = 0) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method(className, meta, source, &err)) << err.message;
}

std::string classDefinition(const char* className) {
  char defn[512];
  if (ao_browser_class_definition(className, defn, 512) != AO_OK) {
    return "<missing>";
  }
  return defn;
}

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

// 乱数で at:put: / removeKey:ifAbsent: / at:ifAbsent: / includesKey: を混ぜ、キーで引く Array
// （参照モデル）と突き合わせる file-in のソース。B9Coll は = を k の値で比べ、hash が k \\ 16 の
// 衝突の多いキーである。B9Random の dictionary は B9Coll と SmallInteger を混ぜ、identity は
// IdentityDictionary に SmallInteger を 64 倍して入れる（identityHash は値そのものなので、下位ビットが
// そろって長いクラスタになり、巡回する後方シフトを通る）。set は Set に B9Coll を入れる。
std::string randomProbeSource(int steps, int keys) {
  const std::string body = R"(| d ref seed ok key n v |
  d := %DICT% new. ref := Array new: %KEYS%. seed := 12345. ok := true.
  1 to: %STEPS% do: [:step |
    seed := (seed * 1103515245 + 12345) \\ 2147483648.
    n := seed \\ %KEYS% + 1.
    key := %KEY%.
    seed := (seed * 1103515245 + 12345) \\ 2147483648.
    (seed \\ 3) = 0 ifTrue: [d at: key put: step. ref at: n put: step].
    (seed \\ 3) = 1 ifTrue: [
      v := d removeKey: key ifAbsent: [nil].
      v = (ref at: n) ifFalse: [ok := false].
      ref at: n put: nil].
    (seed \\ 3) = 2 ifTrue: [
      (d at: key ifAbsent: [nil]) = (ref at: n) ifFalse: [ok := false].
      (d includesKey: key) = (ref at: n) notNil ifFalse: [ok := false]]].
  1 to: %KEYS% do: [:m |
    n := m.
    key := %KEY%.
    (d at: key ifAbsent: [nil]) = (ref at: n) ifFalse: [ok := false]].
  ^ok & (d size = (ref inject: 0 into: [:a :e | e isNil ifTrue: [a] ifFalse: [a + 1]]))!
)";
  const std::string half = std::to_string(keys / 2);
  auto fill = [&](std::string src, const std::string& dict, const std::string& key) {
    const std::pair<std::string, std::string> holes[] = {
        {"%DICT%", dict},  {"%KEY%", key},   {"%STEPS%", std::to_string(steps)},
        {"%KEYS%", std::to_string(keys)}, {"%HALF%", half}};
    for (const auto& [from, to] : holes) {
      for (std::size_t at = src.find(from); at != std::string::npos; at = src.find(from, at)) {
        src.replace(at, from.size(), to);
        at += to.size();
      }
    }
    return src;
  };
  return R"(!Object subclass: #B9Coll
  instanceVariableNames: 'k'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'B9-Test'!
!B9Coll methodsFor: 'probes'!
k: v
  k := v!
k
  ^k!
= other
  ^(other isKindOf: B9Coll) and: [k = other k]!
hash
  ^k \\ 16! !
!Object subclass: #B9Random
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'B9-Test'!
!B9Random methodsFor: 'probes'!
)" + fill("dictionary\n  " + body, "Dictionary", "n > %HALF% ifTrue: [B9Coll new k: n] ifFalse: [n]") +
         fill("identity\n  " + body, "IdentityDictionary", "n * 64") +
         fill(R"(set
  | s ok |
  s := Set new. ok := true.
  1 to: %KEYS% do: [:i | s add: (B9Coll new k: i \\ %HALF%)].
  0 to: %HALF% - 1 do: [:i | (s includes: (B9Coll new k: i)) ifFalse: [ok := false]].
  (s includes: (B9Coll new k: %HALF%)) ifTrue: [ok := false].
  ^ok & (s size = %HALF%)! !
)", "", "");
}

}  // namespace

// 04 High: Dictionary の do: は値を渡す。do: を通る inject:into:・select:・reject:・detect:ifNone:
// も値を受ける（以前は Association を受け、DNU や空の答えになった）。
TEST_F(HashedCollection, DictionaryEnumerationPassesValues) {
  ASSERT_EQ("2", printIt("d9 := Dictionary new. d9 at: #a put: 1; at: #b put: 2. d9 size"));
  EXPECT_EQ("3", printIt("d9 inject: 0 into: [:a :v | a + v]"));
  EXPECT_EQ("#(2)", printIt("d9 select: [:v | v > 1]"));
  EXPECT_EQ("#(1)", printIt("d9 reject: [:v | v > 1]"));
  EXPECT_EQ("2", printIt("d9 detect: [:v | v = 2] ifNone: ['none']"));
  EXPECT_EQ("3", printIt("| s | s := 0. d9 do: [:v | s := s + v]. s"));
  EXPECT_EQ("30", printIt("(d9 collect: [:v | v * 10]) inject: 0 into: [:a :v | a + v]"));
  EXPECT_EQ("true", printIt("d9 includes: 2"));
  EXPECT_EQ("false", printIt("d9 includes: #b"));
}

// 04 High: keysDo:・associationsDo:・keysAndValuesDo:・at:ifAbsent:・removeKey:・
// removeKey:ifAbsent: を Blue Book どおりに持つ。
TEST_F(HashedCollection, DictionaryBlueBookProtocol) {
  ASSERT_EQ("2", printIt("d9 := Dictionary new. d9 at: #a put: 1; at: #b put: 2. d9 size"));
  EXPECT_EQ("2", printIt("| n | n := 0. d9 keysDo: [:k | (k == #a) | (k == #b) ifTrue: "
                         "[n := n + 1]]. n"));
  EXPECT_EQ("3", printIt("| t | t := 0. d9 associationsDo: [:a | t := t + a value]. t"));
  EXPECT_EQ("true", printIt("| ok | ok := true. d9 associationsDo: [:a | "
                            "(a class == Association) & ((d9 at: a key) = a value) ifFalse: "
                            "[ok := false]]. ok"));
  // associationsDo: が渡す Association は写しで、書き換えても表は変わらない。
  EXPECT_EQ("1", printIt("d9 associationsDo: [:a | a value: 99]. d9 at: #a"));
  EXPECT_EQ("2", printIt("| t | t := 0. d9 keysAndValuesDo: [:k :v | k == #b ifTrue: [t := v]]. t"));
  EXPECT_EQ("1", printIt("d9 at: #a ifAbsent: [0]"));
  EXPECT_EQ("'none'", printIt("d9 at: #zz ifAbsent: ['none']"));
  EXPECT_EQ("1", printIt("d9 removeKey: #a"));
  EXPECT_EQ("1", printIt("d9 size"));
  EXPECT_EQ("false", printIt("d9 includesKey: #a"));
  EXPECT_EQ("nil", printIt("d9 at: #a"));
  EXPECT_EQ("<eval error: key not found>", printIt("d9 removeKey: #a"));
  EXPECT_EQ("7", printIt("d9 removeKey: #a ifAbsent: [7]"));
  EXPECT_EQ("2", printIt("d9 removeKey: #b ifAbsent: [7]"));
  EXPECT_EQ("0", printIt("d9 size"));
}

// 04 High: vendor の Bag は contents（Dictionary）の inject:into: で size を数える。
TEST(HashedCollectionVendor, BagSizeCountsWhatWasAdded) {
  Boot b;
  const std::string order = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  std::vector<ao::FileInError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, order, errs))
      << (errs.empty() ? "" : errs[0].error.message);
  ao::Root bag(b.roots, send0(b, b.wk.named("Bag"), "new"));
  ASSERT_TRUE(bag.slot.isHeap());
  send1(b, bag.slot, "add:", smi(3));
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ(smi(1), send0(b, bag.slot, "size"));
  send1(b, bag.slot, "add:", smi(3));
  send1(b, bag.slot, "add:", smi(4));
  EXPECT_EQ(smi(3), send0(b, bag.slot, "size"));
  EXPECT_EQ(smi(2), send1(b, bag.slot, "occurrencesOf:", smi(3)));
  EXPECT_FALSE(b.ctx.aborting);
}

// 04 Medium: nil はキーにも要素にもならない。以前は tally だけが増え、見えないエントリが残った。
TEST_F(HashedCollection, NilIsNeitherKeyNorElement) {
  ASSERT_EQ("0", printIt("d9 := Dictionary new. d9 size"));
  EXPECT_EQ("<eval error: key must not be nil>", printIt("d9 at: nil put: 1"));
  EXPECT_EQ("0", printIt("d9 size"));
  EXPECT_EQ("nil", printIt("d9 at: nil"));
  EXPECT_EQ("false", printIt("d9 includesKey: nil"));
  EXPECT_EQ("5", printIt("d9 at: nil ifAbsent: [5]"));
  EXPECT_EQ("<eval error: key not found>", printIt("d9 removeKey: nil"));
  EXPECT_EQ("6", printIt("d9 removeKey: nil ifAbsent: [6]"));
  EXPECT_EQ("0", printIt("(d9 collect: [:x | x]) size"));
  EXPECT_EQ("<eval error: key must not be nil>", printIt("IdentityDictionary new at: nil put: 1"));
  ASSERT_EQ("0", printIt("s9 := Set new. s9 size"));
  EXPECT_EQ("<eval error: element must not be nil>", printIt("s9 add: nil"));
  EXPECT_EQ("0", printIt("s9 size"));
  EXPECT_EQ("false", printIt("s9 includes: nil"));
  EXPECT_EQ("<eval error: element must not be nil>", printIt("IdentitySet new add: nil"));
}

// 04 High / Medium: 等しいキーは同じエントリを引く（B8 の = と hash の契約）。型をまたぐ = は
// false なので、1 と 1.0 は別のキーである。
TEST_F(HashedCollection, EqualKeysFindTheSameEntry) {
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. d at: 'abc' put: 1. d at: 'abc' copy put: 2. "
                            "(d size = 1) & ((d at: 'abc') = 2)"));
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. d at: #abc put: 1. d at: 'abc' put: 2. "
                            "(d size = 1) & ((d at: #abc) = 2) & (d includesKey: 'abc' copy)"));
  EXPECT_EQ("1", printIt("| d | d := Dictionary new. d at: (1 bitShift: 70) put: 1. "
                         "d at: (2 bitShift: 69)"));
  EXPECT_EQ("1", printIt("| d | d := Dictionary new. d at: 0.0 put: 1. d at: (0.0 * -1)"));
  EXPECT_EQ("1", printIt("| d | d := Dictionary new. d at: (1/2) put: 1. d at: (2/4)"));
  EXPECT_EQ("2", printIt("| d | d := Dictionary new. d at: 1 put: #int. d at: 1.0 put: #float. "
                         "d size"));
  EXPECT_EQ("1", printIt("| d a | d := Dictionary new. a := Array new: 3. a at: 1 put: 1; "
                         "at: 2 put: 'b'; at: 3 put: #c. d at: a put: 1. "
                         "d at: ((Array new: 3) at: 1 put: 1; at: 2 put: 'b' copy; at: 3 put: #c; "
                         "yourself)"));
  EXPECT_EQ("1", printIt("| d | d := Dictionary new. d at: (Point x: 1 y: 2) put: 1. "
                         "d at: (Point x: 1 y: 2)"));
  // 同一のキーは = を送らずに一致する（NaN は自分と = にならない）。
  EXPECT_EQ("1", printIt("| d n | n := 0.0 / 0.0. d := Dictionary new. d at: n put: 1. d at: n"));
  EXPECT_EQ("1", printIt("| s | s := Set new. s add: 'abc'; add: 'abc' copy; add: #abc. s size"));
  EXPECT_EQ("true", printIt("| s | s := Set new. s add: 'abc'. s includes: 'abc' copy"));
  EXPECT_EQ("false", printIt("| s | s := Set new. s add: 'abc'. s includes: 'abd'"));
}

// Identity 版は == で比べる。等しいが同一でない String は別のキーである。
TEST_F(HashedCollection, IdentityVersionsCompareIdentity) {
  EXPECT_EQ("true", printIt("| d k | d := IdentityDictionary new. k := 'abc'. d at: k put: 1. "
                            "d at: 'abc' copy put: 2. (d size = 2) & ((d at: k) = 1)"));
  EXPECT_EQ("nil", printIt("| d | d := IdentityDictionary new. d at: 'abc' copy put: 1. "
                           "d at: 'abc' copy"));
  EXPECT_EQ("true", printIt("| d k | d := IdentityDictionary new. k := 'abc' copy. d at: k put: 1. "
                            "(d removeKey: 'abc' copy ifAbsent: [#absent]) == #absent"));
  EXPECT_EQ("1", printIt("| d k | d := IdentityDictionary new. k := 'abc' copy. d at: k put: 1. "
                         "d removeKey: k"));
  EXPECT_EQ("2", printIt("| s | s := IdentitySet new. s add: 'abc' copy; add: 'abc' copy. s size"));
  EXPECT_EQ("1", printIt("| s | s := IdentitySet new. s add: #abc; add: #abc. s size"));
  EXPECT_EQ("true", printIt("(IdentityDictionary new at: #a put: 1; yourself) includesKey: #a"));
  EXPECT_EQ("9", printIt("(IdentityDictionary new at: #a put: 9; yourself) at: #a ifAbsent: [0]"));
}

// 04 High / SPEC §3.3: hash は Integer、= は Boolean を答えなければ失敗する。巻き戻しは空 OOP の
// まま伝わる。以前は hash の答えを捨て（consumeHash）、= は true 以外を不一致にしていた。
TEST_F(HashedCollection, HashAndEqualsAnswersAreChecked) {
  acceptClass("Object", "B9BadHash", "");
  acceptMethod("B9BadHash", "hash\n  ^'not an Integer'\n");
  acceptClass("Object", "B9FloatHash", "");
  acceptMethod("B9FloatHash", "hash\n  ^1.5\n");
  acceptClass("Object", "B9AbortHash", "");
  acceptMethod("B9AbortHash", "hash\n  ^nil foo\n");
  acceptClass("Object", "B9BadEq", "");
  acceptMethod("B9BadEq", "hash\n  ^1\n");
  acceptMethod("B9BadEq", "= other\n  ^3\n");
  acceptClass("Object", "B9LargeHash", "");
  acceptMethod("B9LargeHash", "hash\n  ^1 bitShift: 70\n");

  EXPECT_EQ("<eval error: failed: #at:put:>", printIt("Dictionary new at: B9BadHash new put: 1"));
  EXPECT_EQ("<eval error: failed: #at:>", printIt("Dictionary new at: B9BadHash new"));
  EXPECT_EQ("<eval error: failed: #includesKey:>",
            printIt("Dictionary new includesKey: B9BadHash new"));
  EXPECT_EQ("<eval error: failed: #removeKey:ifAbsent:>",
            printIt("Dictionary new removeKey: B9BadHash new ifAbsent: [0]"));
  EXPECT_EQ("<eval error: failed: #add:>", printIt("Set new add: B9BadHash new"));
  EXPECT_EQ("<eval error: failed: #includes:>", printIt("Set new includes: B9FloatHash new"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #foo>",
            printIt("Dictionary new at: B9AbortHash new put: 1"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #foo>", printIt("Set new add: B9AbortHash new"));
  // Identity 版は hash を送らない。
  EXPECT_EQ("1", printIt("(IdentityDictionary new at: B9BadHash new put: 1; yourself) size"));
  EXPECT_EQ("1", printIt("(IdentitySet new add: B9AbortHash new; yourself) size"));
  // LargeInteger の hash は Integer なので受け付ける。
  EXPECT_EQ("true", printIt("| d k | d := Dictionary new. k := B9LargeHash new. d at: k put: 1. "
                            "d at: B9LargeHash new put: 2. (d size = 2) & ((d at: k) = 1)"));

  ASSERT_EQ("1", printIt("d9 := Dictionary new. d9 at: B9BadEq new put: 1. d9 size"));
  EXPECT_EQ("<eval error: failed: #at:put:>", printIt("d9 at: B9BadEq new put: 2"));
  EXPECT_EQ("<eval error: failed: #at:>", printIt("d9 at: B9BadEq new"));
  EXPECT_EQ("1", printIt("d9 size"));
  EXPECT_EQ("<eval error: failed: #includes:>",
            printIt("(Dictionary new at: 1 put: 5; yourself) includes: B9BadEq new"));
  EXPECT_EQ("<eval error: failed: #add:>",
            printIt("Set new add: B9BadEq new; add: B9BadEq new; yourself"));
}

// SPEC §3.6: 1 回の操作でキーに hash を 1 回だけ送る。拡張と削除は hash を送らない。
TEST_F(HashedCollection, HashIsSentOncePerOperationAndNotByGrowth) {
  acceptClass("Object", "B9CountHash", "");
  acceptMethod("B9CountHash",
               "hash\n  Smalltalk at: #B9Hashes put: (Smalltalk at: #B9Hashes) + 1.\n  ^1\n");
  ASSERT_EQ("0", printIt("Smalltalk at: #B9Hashes put: 0"));
  // すべて hash 1 で衝突する。20 件で 2 回拡張する（7 件目と 13 件目）。
  EXPECT_EQ("20", printIt("keys9 := Array new: 20. d9 := Dictionary new. 1 to: 20 do: [:i | "
                          "keys9 at: i put: B9CountHash new. d9 at: (keys9 at: i) put: i]. "
                          "Smalltalk at: #B9Hashes"));
  EXPECT_EQ("20", printIt("d9 size"));
  EXPECT_EQ("7", printIt("d9 at: (keys9 at: 7)"));
  EXPECT_EQ("true", printIt("d9 includesKey: (keys9 at: 20)"));
  EXPECT_EQ("3", printIt("d9 removeKey: (keys9 at: 3)"));
  EXPECT_EQ("0", printIt("d9 removeKey: (keys9 at: 3) ifAbsent: [0]"));
  EXPECT_EQ("5", printIt("d9 at: (keys9 at: 5) ifAbsent: [0]"));
  EXPECT_EQ("25", printIt("Smalltalk at: #B9Hashes"));
  EXPECT_EQ("true", printIt("| ok | ok := true. 1 to: 20 do: [:i | i = 3 ifFalse: [(d9 at: (keys9 "
                            "at: i)) = i ifFalse: [ok := false]]]. ok"));
}

// SPEC §3.6 再入: hash・=・列挙のブロックが同じ表を書き換えても（拡張、削除、配列の差し替え）、
// 範囲外を読み書きせずに続くか、壊れた表として失敗する。
TEST_F(HashedCollection, ReentrantWritesDoNotBreakTheTable) {
  acceptClass("Object", "B9Grow", "dict");
  acceptMethod("B9Grow", "dict: d\n  dict := d\n");
  acceptMethod("B9Grow",
               "hash\n  | d |\n  dict isNil ifFalse: [d := dict. dict := nil. "
               "1 to: 40 do: [:i | d at: i put: i]].\n  ^7\n");
  acceptClass("Object", "B9EqGrow", "dict");
  acceptMethod("B9EqGrow", "dict: d\n  dict := d\n");
  acceptMethod("B9EqGrow", "hash\n  ^5\n");
  acceptMethod("B9EqGrow",
               "= other\n  | d |\n  dict isNil ifFalse: [d := dict. dict := nil. "
               "1 to: 40 do: [:i | d at: i put: i]].\n  ^false\n");
  acceptClass("Object", "B9EqRemove", "dict");
  acceptMethod("B9EqRemove", "dict: d\n  dict := d\n");
  acceptMethod("B9EqRemove", "hash\n  ^5\n");
  acceptMethod("B9EqRemove",
               "= other\n  | d |\n  dict isNil ifFalse: [d := dict. dict := nil. "
               "d removeKey: other].\n  ^false\n");
  acceptClass("Object", "B9EqShrink", "dict");
  acceptMethod("B9EqShrink", "dict: d\n  dict := d\n");
  acceptMethod("B9EqShrink", "hash\n  ^5\n");
  acceptMethod("B9EqShrink",
               "= other\n  | d |\n  dict isNil ifFalse: [d := dict. dict := nil. "
               "d instVarAt: 2 put: (Array new: 24); instVarAt: 1 put: 0].\n  ^false\n");
  acceptClass("Object", "B9EqDamage", "dict");
  acceptMethod("B9EqDamage", "dict: d\n  dict := d\n");
  acceptMethod("B9EqDamage", "hash\n  ^5\n");
  acceptMethod("B9EqDamage", "= other\n  dict instVarAt: 2 put: (Array new: 5).\n  ^false\n");

  // hash の中で拡張しても、まだ表を読んでいないので、そのまま入る。
  EXPECT_EQ("true", printIt("| d k | d := Dictionary new. k := B9Grow new dict: d. d at: k put: #k. "
                            "(d size = 41) & ((d at: k) == #k)"));
  // = の中で拡張すると配列が差し替わるので、探し直してから入れる。
  EXPECT_EQ("true", printIt("| d a b | d := Dictionary new. a := B9EqGrow new. d at: a put: 1. "
                            "b := B9EqGrow new dict: d. d at: b put: 2. "
                            "(d size = 42) & ((d at: a) = 1) & ((d at: b) = 2)"));
  // = の中で比べていたエントリを消すと、探し直す。
  EXPECT_EQ("true", printIt("| d a b | d := Dictionary new. a := B9EqRemove new. d at: a put: 1. "
                            "b := B9EqRemove new dict: d. d at: b put: 2. "
                            "(d size = 1) & ((d at: b) = 2) & (d includesKey: a) not"));
  // = の中で空の配列に差し替えても、範囲外を読まない。
  EXPECT_EQ("true", printIt("| d a b | d := Dictionary new. a := B9EqShrink new. d at: a put: 1. "
                            "b := B9EqShrink new dict: d. d at: b put: 2. "
                            "(d size = 1) & ((d at: b) = 2)"));
  EXPECT_EQ("<eval error: damaged hashed collection>",
            printIt("| d a | d := Dictionary new. a := B9EqDamage new. d at: a put: 1. "
                    "d at: (B9EqDamage new dict: d) put: 2"));
  // Set も同じ探索を使う。
  EXPECT_EQ("true", printIt("| s a b | s := Set new. a := B9EqGrow new. s add: a. "
                            "b := B9EqGrow new dict: Dictionary new. s add: b. s size = 2"));
  // 値の includes: の = が表を書き換えても落ちない。
  EXPECT_EQ("true", printIt("| d b | d := Dictionary new. d at: 1 put: 1; at: 2 put: 2. "
                            "b := B9EqGrow new dict: d. (d includes: b) not"));

  // 列挙のブロックが表を書き換えても落ちない（飛ばす・重ねるは規定しない）。
  EXPECT_EQ("true", printIt("| d n | d := Dictionary new. 1 to: 10 do: [:i | d at: i put: i]. "
                            "n := 0. d do: [:v | n := n + 1. n < 30 ifTrue: [d at: n + 100 put: n]]. "
                            "n > 0"));
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. 1 to: 10 do: [:i | d at: i put: i]. "
                            "d keysDo: [:k | d removeKey: k]. d size < 10"));
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. 1 to: 10 do: [:i | d at: i put: i]. "
                            "d associationsDo: [:a | d removeKey: a key. d at: a key + 50 put: 0]. "
                            "d size >= 0"));
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. 1 to: 10 do: [:i | d at: i put: i]. "
                            "d keysAndValuesDo: [:k :v | d instVarAt: 2 put: nil]. d size = 0"));
  EXPECT_EQ("true", printIt("| s n | s := Set new. 1 to: 5 do: [:i | s add: i]. n := 0. "
                            "s do: [:e | n := n + 1. n < 40 ifTrue: [s add: n + 100]]. n > 0"));
  EXPECT_EQ("<eval error: damaged hashed collection>",
            printIt("| d | d := Dictionary new. d at: 1 put: 1; at: 2 put: 2. "
                    "d do: [:v | d instVarAt: 2 put: (Array new: 3)]"));
}

// SPEC §3.6 壊れた表: 配置に合わない tally や array は失敗し、nil の array は空の表である。
// tally が実際の数と違うだけなら、探索は容量の回数で止まり、拡張が数え直す。
TEST_F(HashedCollection, DamagedTallyOrArray) {
  const char* broken[] = {
      "d instVarAt: 2 put: (Array new: 5)",   // 容量×幅でない
      "d instVarAt: 2 put: (Array new: 12)",  // 容量 4
      "d instVarAt: 2 put: (Array new: 72)",  // 容量 24（2 のべき乗でない）
      "d instVarAt: 2 put: 'abcdefghijklmnopqrstuvwx'",
      "d instVarAt: 2 put: 3",
      "d instVarAt: 1 put: 9",   // 容量 8 を超える
      "d instVarAt: 1 put: -1",
      "d instVarAt: 1 put: 'x'",
  };
  for (const char* b : broken) {
    SCOPED_TRACE(b);
    const std::string setup = std::string("| d | d := Dictionary new. d at: 1 put: 1. ") + b + ". ";
    EXPECT_EQ("<eval error: damaged hashed collection>", printIt(setup + "d at: 1"));
    EXPECT_EQ("<eval error: damaged hashed collection>", printIt(setup + "d size"));
    EXPECT_EQ("<eval error: damaged hashed collection>", printIt(setup + "d at: 2 put: 2"));
    EXPECT_EQ("<eval error: damaged hashed collection>", printIt(setup + "d do: [:v | v]"));
    EXPECT_EQ("<eval error: damaged hashed collection>", printIt(setup + "d removeKey: 1"));
  }
  // Dictionary の配列（幅 3）は Set（幅 2）の配置に合わない。
  EXPECT_EQ("<eval error: damaged hashed collection>",
            printIt("| s | s := Set new. s instVarAt: 2 put: (Array new: 24). s add: 1"));
  // nil の array は空の表。basicNew も同じ。
  EXPECT_EQ("0", printIt("| d | d := Dictionary new. d at: 1 put: 1. d instVarAt: 2 put: nil. d size"));
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. d at: 1 put: 1. d instVarAt: 2 put: nil. "
                            "d at: 1 put: 2; at: 3 put: 4. (d size = 2) & ((d at: 1) = 2)"));
  EXPECT_EQ("1", printIt("(Dictionary basicNew at: 1 put: 2; yourself) size"));
  EXPECT_EQ("1", printIt("(Set basicNew add: 1; yourself) size"));
  EXPECT_EQ("nil", printIt("Dictionary basicNew at: 1"));

  // 空きの無い表（tally は 0 のまま）: 探索は 8 回で止まり、挿入は拡張して数え直す。
  const std::string full =
      "| a d | a := Array new: 24. 0 to: 7 do: [:i | a at: 3 * i + 1 put: i; at: 3 * i + 2 put: i; "
      "at: 3 * i + 3 put: i]. d := Dictionary new. d instVarAt: 2 put: a; instVarAt: 1 put: 0. ";
  EXPECT_EQ("nil", printIt(full + "d at: 99"));
  EXPECT_EQ("5", printIt(full + "d at: 5"));
  EXPECT_EQ("false", printIt(full + "d includesKey: 99"));
  EXPECT_EQ("9", printIt(full + "d at: 99 put: 1. d size"));
  EXPECT_EQ("true", printIt(full + "d at: 99 put: 1. (d at: 99) = 1 & ((d at: 7) = 7)"));
  EXPECT_EQ("true", printIt(full + "(d removeKey: 3) = 3 & (d size = 0) & ((d at: 4) = 4) & "
                                   "(d includesKey: 3) not"));
  // tally が多すぎても、拡張が数え直す。
  EXPECT_EQ("3", printIt("| d | d := Dictionary new. d at: 1 put: 1; at: 2 put: 2. "
                         "d instVarAt: 1 put: 8. d at: 3 put: 3. d size"));
  // 保存した hash が SmallInteger でないエントリは、どのキーとも一致しない。
  EXPECT_EQ("true", printIt("| d | d := Dictionary new. d at: 0 put: 1. (d instVarAt: 2) at: 3 put: 'x'. "
                            "(d at: 0) isNil & (d size = 1)"));
}

// 削除（後方シフト）と拡張を、乱数の操作列で参照モデルと突き合わせる。GC ストレス下では、ほかの
// テストが GC 安全性を見るので、操作の数を減らして時間を抑える。
TEST(HashedCollectionRandom, OperationsMatchAReferenceModel) {
  Boot b;
  const bool stressed = b.heap.gcStress() != 0;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, randomProbeSource(stressed ? 100 : 3000, stressed ? 40 : 400),
                               errs))
      << (errs.empty() ? "" : errs[0].message);
  ASSERT_TRUE(errs.empty()) << errs[0].message;
  ao::Root probe(b.roots, send0(b, b.wk.named("B9Random"), "new"));
  ASSERT_TRUE(probe.slot.isHeap());
  for (const char* sel : {"dictionary", "identity", "set"}) {
    SCOPED_TRACE(sel);
    EXPECT_TRUE(send0(b, probe.slot, sel).isTrue());
    EXPECT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  }
}

namespace {

// A SmallInteger key from `from` on, not in used, whose home in a table of capacity entries is
// want. A SmallInteger's hash is its value, so a Dictionary saves the key itself as its hash.
std::int64_t keyWithHome(std::uint32_t want, std::uint32_t capacity, std::set<std::int64_t>& used,
                         std::int64_t from) {
  for (std::int64_t k = from;; ++k) {
    if (used.count(k) == 0 && ao::Hashed::home(k, capacity) == want) {
      used.insert(k);
      return k;
    }
  }
}

// Gives d a table with no free entry: entry p holds keys[p], the value keys[p] * 10 and the hash
// keys[p]. The tally is 0, a lie that only instVarAt:put: can tell (SPEC §3.6 壊れた表).
void fillFullTable(Boot& b, ao::Root& d, const std::vector<std::int64_t>& keys) {
  const auto capacity = static_cast<std::uint32_t>(keys.size());
  ao::Root array(b.roots, send1(b, b.wk.arrayClass, "new:", smi(capacity * 3)));
  ASSERT_TRUE(array.slot.isHeap());
  for (std::uint32_t p = 0; p < capacity; ++p) {
    b.heap.slotAtPut(array.slot, p * 3, smi(keys[p]));
    b.heap.slotAtPut(array.slot, p * 3 + 1, smi(keys[p] * 10));
    b.heap.slotAtPut(array.slot, p * 3 + 2, smi(keys[p]));
  }
  b.heap.slotAtPut(d.slot, ao::Hashed::kSlotTally, smi(0));
  b.heap.slotAtPut(d.slot, ao::Hashed::kSlotArray, array.slot);
}

}  // namespace

// B9 review (Low): 空きの無い表（tally が嘘）からの削除でエントリを見失わない。後方シフトは容量 - 1
// 歩で打ち切っていたので、回り込んで置かれたエントリがホームから届かなくなった（容量 8、キー
// #(0 10 11 3 4 5 6 7) を hash bitAnd: 7 で置いた表で removeKey: 0 のあと at: 10 が nil）。
// 最初の配置はその再現をホームで置き直したもの、残りは乱数で選んだ満杯の配置で、キーを乱数の順に
// 全部消しながら参照モデルと突き合わせる。
TEST(HashedCollectionRandom, RemovalFromAFullTableMatchesAReferenceModel) {
  Boot b;
  const bool stressed = b.heap.gcStress() != 0;
  std::mt19937 rng(20260925);
  std::vector<std::vector<std::uint32_t>> layouts = {{0, 2, 3, 3, 4, 5, 6, 7}};
  for (int t = 0; t < (stressed ? 20 : 300); ++t) {
    const std::uint32_t capacity = 8u << (rng() % 3);
    std::vector<std::uint32_t> homes(capacity);
    for (std::uint32_t& h : homes) {
      h = static_cast<std::uint32_t>(rng() % capacity);
    }
    layouts.push_back(std::move(homes));
  }
  for (std::size_t l = 0; l < layouts.size(); ++l) {
    SCOPED_TRACE(l);
    const auto capacity = static_cast<std::uint32_t>(layouts[l].size());
    std::set<std::int64_t> used;
    std::vector<std::int64_t> keys;
    for (const std::uint32_t h : layouts[l]) {
      keys.push_back(keyWithHome(h, capacity, used, static_cast<std::int64_t>(rng() % 1000)));
    }
    ao::Root d(b.roots, send0(b, b.wk.dictionaryClass, "new"));
    fillFullTable(b, d, keys);
    std::map<std::int64_t, std::int64_t> model;
    for (const std::int64_t k : keys) {
      model[k] = k * 10;
    }
    std::vector<std::int64_t> order = keys;
    if (l > 0) {
      std::shuffle(order.begin(), order.end(), rng);
    }
    for (const std::int64_t k : order) {
      ASSERT_EQ(smi(k * 10), send1(b, d.slot, "removeKey:", smi(k))) << "removing " << k;
      model.erase(k);
      for (const auto& [mk, mv] : model) {
        ASSERT_EQ(smi(mv), send1(b, d.slot, "at:", smi(mk))) << mk << " lost after removing " << k;
      }
      ASSERT_TRUE(send1(b, d.slot, "at:", smi(k)).isNil());
      ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
    }
  }
}

// 保存した hash は値ベースか identityHash（ヘッダに保存される）なので、イメージを保存して読み直した
// 表もそのまま引ける。
TEST_F(HashedCollection, TablesSurviveImageSaveAndLoad) {
  ASSERT_EQ("true", printIt("| d i s k | d := Dictionary new. d at: 'abc' put: 1; at: #sym put: 2; "
                            "at: (1 bitShift: 70) put: 3. 1 to: 20 do: [:n | d at: n put: n]. "
                            "k := Object new. i := IdentityDictionary new. i at: k put: 4. "
                            "s := Set new. s add: 'x'; add: 3.5. Smalltalk at: #B9D put: d; "
                            "at: #B9I put: i; at: #B9K put: k; at: #B9S put: s. true"));
  const auto path = std::filesystem::path(testing::TempDir()) / "b9-hashed.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  EXPECT_EQ("true", printIt("((B9D at: 'abc' copy) = 1) & ((B9D at: 'sym') = 2) & "
                            "((B9D at: (2 bitShift: 69)) = 3) & ((B9D at: 17) = 17) & "
                            "(B9D size = 23) & ((B9I at: B9K) = 4) & (B9S includes: 'x' copy) & "
                            "(B9S includes: 3.5)"));
  EXPECT_EQ("24", printIt("B9D at: 'new' put: 0. B9D removeKey: 5. B9D at: 'more' put: 1. B9D size"));
  std::filesystem::remove(path);
}

// SPEC §3.6 クラス変数: classPool はハッシュ表の配置で、キーの hash は #名前 hash と同じ。Symbol
// でも String でも同じ束縛を引き、名前はバイト順に並ぶ。
TEST(HashedClassPool, MakeHashesNamesAsSymbolsAndListsThemInByteOrder) {
  Boot b;
  const std::vector<std::string> written = {
      "Zeta", "alpha", "Mid", "Beta", "Count", "N", "Total", "Kept", "Q1", "Q2", "Q3",
      "Q4",   "Q5",    "Q6",  "Q7",   "Q8",    "Q9", "Q10", "Q11", "Q12", "Mid"};
  std::vector<std::string> expected(written.begin(), written.end());
  std::sort(expected.begin(), expected.end());
  expected.erase(std::unique(expected.begin(), expected.end()), expected.end());

  ao::Root pool(b.roots, ao::ClassPool::make(b.ctx, written));
  ASSERT_TRUE(pool.slot.isHeap());
  EXPECT_EQ(b.wk.dictionaryClass, b.heap.klass(pool.slot));
  EXPECT_EQ(expected, ao::ClassPool::names(b.heap, pool.slot));
  EXPECT_EQ(smi(static_cast<std::int64_t>(expected.size())), send0(b, pool.slot, "size"));
  for (const std::string& name : expected) {
    SCOPED_TRACE(name);
    ao::Root binding(b.roots, ao::ClassPool::bindingAt(b.heap, pool.slot, name));
    ASSERT_TRUE(binding.slot.isHeap());
    EXPECT_TRUE(ao::ClassPool::isBinding(b.heap, b.wk, binding.slot));
    EXPECT_EQ(b.wk.intern(name), b.heap.slotAt(binding.slot, ao::kAssocKey));
    // The Dictionary natives send hash to the key: the pool's hash must be the Symbol's.
    EXPECT_EQ(binding.slot, send1(b, pool.slot, "at:", b.wk.intern(name)));
    ao::Root text(b.roots, ao::Str::fromUtf8(b.heap, b.wk, name));
    EXPECT_EQ(binding.slot, send1(b, pool.slot, "at:", text.slot));
  }
  EXPECT_TRUE(ao::ClassPool::bindingAt(b.heap, pool.slot, "Absent").isEmpty());

  // The copy shares the bindings; at:put: on it leaves the pool alone.
  ao::Root copy(b.roots, ao::ClassPool::copy(b.ctx, pool.slot));
  ASSERT_TRUE(copy.slot.isHeap());
  EXPECT_NE(pool.slot, copy.slot);
  EXPECT_EQ(expected, ao::ClassPool::names(b.heap, copy.slot));
  for (const std::string& name : expected) {
    EXPECT_EQ(ao::ClassPool::bindingAt(b.heap, pool.slot, name),
              send1(b, copy.slot, "at:", b.wk.intern(name)))
        << name;
  }
  send2(b, copy.slot, "at:put:", b.wk.intern("Fresh"), smi(1));
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_TRUE(send1(b, copy.slot, "includesKey:", b.wk.intern("Fresh")).isTrue());
  EXPECT_TRUE(send1(b, pool.slot, "includesKey:", b.wk.intern("Fresh")).isFalse());

  // adopt takes the entries of the names both pools have.
  ao::Root other(b.roots, ao::ClassPool::make(b.ctx, {"Count", "Other"}));
  ASSERT_TRUE(other.slot.isHeap());
  ao::ClassPool::adopt(b.heap, other.slot, pool.slot);
  EXPECT_EQ(ao::ClassPool::bindingAt(b.heap, pool.slot, "Count"),
            ao::ClassPool::bindingAt(b.heap, other.slot, "Count"));
  EXPECT_NE(ao::ClassPool::bindingAt(b.heap, pool.slot, "Q1"),
            ao::ClassPool::bindingAt(b.heap, other.slot, "Other"));
}

// SPEC §3.6 / §3.10: クラス変数を読み書きするメソッド、classPool の写しへの at:（Symbol と
// String）、Browser の定義テキストはバイト順。書いた順を変えただけの再 Accept は classPool を
// 作り直さない。
TEST_F(HashedCollection, ClassVariablesThroughTheHashedPool) {
  acceptClass("Object", "B9Pool", "", "Zeta Alpha Mid");
  acceptMethod("B9Pool", "zeta\n  ^Zeta\n");
  acceptMethod("B9Pool", "zeta: v\n  Zeta := v\n");
  acceptMethod("B9Pool", "alpha: v\n  Alpha := v\n", 1);
  acceptMethod("B9Pool", "alpha\n  ^Alpha\n", 1);
  EXPECT_EQ("5", printIt("B9Pool new zeta: 5; zeta"));
  EXPECT_EQ("7", printIt("B9Pool alpha: 7; alpha"));
  EXPECT_EQ("5", printIt("(B9Pool classPool at: #Zeta) value"));
  EXPECT_EQ("true", printIt("(B9Pool classPool at: 'Zeta') == (B9Pool classPool at: #Zeta)"));
  EXPECT_EQ("3", printIt("B9Pool classPool size"));
  EXPECT_EQ("false", printIt("B9Pool classPool at: #New put: 1. B9Pool classPool includesKey: #New"));
  const std::string shown = classDefinition("B9Pool");
  EXPECT_NE(shown.find("classVariableNames: 'Alpha Mid Zeta'"), std::string::npos) << shown;

  ASSERT_EQ("true", printIt("pool9 := B9Pool instVarAt: 7. pool9 class == Dictionary"));
  acceptClass("Object", "B9Pool", "", "Mid Zeta Alpha");
  EXPECT_EQ("true", printIt("(B9Pool instVarAt: 7) == pool9"));
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(shown.c_str(), &err)) << err.message;
  EXPECT_EQ("true", printIt("(B9Pool instVarAt: 7) == pool9"));
  EXPECT_EQ("5", printIt("B9Pool new zeta"));
}

// 04 Medium: SmallInteger でない刻みの向きは step < 0 で決める。以前は負の Float の刻みを前向きと
// して扱い、size が 0 になった。
TEST_F(HashedCollection, IntervalStepDirectionIsSent) {
  EXPECT_EQ("3", printIt("(Interval from: 2.0 to: 1.0 by: -0.5) size"));
  EXPECT_EQ("#(2 1.5 1)", printIt("(Interval from: 2.0 to: 1.0 by: -0.5) collect: [:x | x]"));
  EXPECT_EQ("true", printIt("| s | s := OrderedCollection new. (Interval from: 1 to: 2 by: 0.5) "
                            "do: [:x | s add: x]. (s size = 3) & ((s at: 1) = 1) & "
                            "((s at: 2) = 1.5) & ((s at: 3) = 2.0)"));
  EXPECT_EQ("3", printIt("(Interval from: 1 to: 2 by: 0.5) size"));
  EXPECT_EQ("3", printIt("(Interval from: 0 to: 1 by: 1/2) size"));
  EXPECT_EQ("3", printIt("(Interval from: 1 to: 0 by: -1/2) size"));
  EXPECT_EQ("true", printIt("((Interval from: 1 to: 0 by: -1/2) collect: [:x | x]) = "
                            "((Array new: 3) at: 1 put: 1; at: 2 put: 1/2; at: 3 put: 0; yourself)"));
  EXPECT_EQ("0", printIt("(Interval from: 1 to: 5 by: 0.0) size"));
  EXPECT_EQ("0", printIt("| n | n := 0. (Interval from: 1 to: 5 by: 0.0) do: [:x | n := n + 1]. n"));
  EXPECT_EQ("0", printIt("(Interval from: 1 to: 5 by: -1.0) size"));
  EXPECT_EQ("2", printIt("(Interval from: 1 to: 2.5 by: 1) size"));
}

// 04 Medium: 刻みの向きと終端の比較の答えが Boolean でなければ失敗する（以前は 2^20 回まで回った）。
TEST_F(HashedCollection, IntervalComparisonsMustAnswerBooleans) {
  acceptClass("Object", "B9Step", "");
  acceptMethod("B9Step", "< x\n  ^3\n");
  acceptClass("Object", "B9Step2", "");
  acceptMethod("B9Step2", "< x\n  ^false\n");
  acceptMethod("B9Step2", "> x\n  ^nil\n");
  acceptClass("Object", "B9Mag", "");
  acceptMethod("B9Mag", "> x\n  ^3\n");
  acceptMethod("B9Mag", "+ x\n  ^self\n");
  EXPECT_EQ("<eval error: failed: #size>", printIt("(Interval from: 1 to: 5 by: B9Step new) size"));
  EXPECT_EQ("<eval error: failed: #do:>",
            printIt("(Interval from: 1 to: 5 by: B9Step new) do: [:x | x]"));
  EXPECT_EQ("<eval error: failed: #size>", printIt("(Interval from: 1 to: 5 by: B9Step2 new) size"));
  EXPECT_EQ("<eval error: failed: #size>", printIt("(Interval from: B9Mag new to: 5 by: 1) size"));
  EXPECT_EQ("<eval error: failed: #do:>",
            printIt("| n | n := 0. (Interval from: B9Mag new to: 5 by: 1) do: [:x | n := n + 1]"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #<>",
            printIt("(Interval from: 1 to: (1 bitShift: 70) by: nil) size"));
}

// 04 Medium: 2^20+1 要素で黙って打ち切る上限は無い。ブロックの abort でループは止まる。
TEST_F(HashedCollection, IntervalHasNoElementCap) {
  EXPECT_EQ("1048600", printIt("(Interval from: 1 to: 1048600.0 by: 1) size"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #foo>",
            printIt("| n | n := 0. (Interval from: 1 to: (1 bitShift: 200) by: 1) do: [:i | "
                    "n := n + 1. n = 3 ifTrue: [nil foo]]"));
  EXPECT_EQ("true", printIt("| lo hi | lo := 0 - (1 bitShift: 62). hi := (1 bitShift: 62) - 1. "
                            "(lo class == SmallInteger) & (hi class == SmallInteger) & "
                            "((Interval from: lo to: hi by: 1) size = (1 bitShift: 63)) & "
                            "((Interval from: lo to: hi by: 2) size = (1 bitShift: 62))"));
}

// 04 Low: OrderedCollection>>at: は範囲外と整数でない添字で失敗する（以前は nil と空 OOP）。
TEST_F(HashedCollection, OrderedCollectionAtOutOfRangeFails) {
  ASSERT_EQ("1", printIt("oc9 := OrderedCollection new. oc9 add: 5. oc9 size"));
  EXPECT_EQ("5", printIt("oc9 at: 1"));
  for (const char* index : {"0", "2", "-1", "1.0", "nil", "(1 bitShift: 70)"}) {
    SCOPED_TRACE(index);
    EXPECT_EQ("<eval error: at: index out of range>", printIt(std::string("oc9 at: ") + index));
  }
  // lastIndex を配列の外へ書き換えても、配列の外は読まない。
  EXPECT_EQ("<eval error: at: index out of range>",
            printIt("| oc | oc := OrderedCollection new. oc add: 1. oc instVarAt: 3 put: 100. "
                    "oc at: 50"));
  EXPECT_EQ("<eval error: at: index out of range>",
            printIt("| oc | oc := OrderedCollection new. oc add: 1. oc instVarAt: 1 put: nil. "
                    "oc at: 1"));
}

namespace {

// GC を走らせずに nursery を使い切る（残りは 16 B 未満）。
void fillNursery(Boot& b) {
  while (b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
}

std::vector<std::int64_t>& seenValues() {
  static std::vector<std::int64_t> seen;
  return seen;
}

}  // namespace

// GC 圧下: nursery を満杯にしてから、拡張する at:put:、removeKey:、associationsDo:（Association を
// 作る）を送る。GC ストレス中は解放した領域を毒で埋めるので、古い番地を読めば落ちる。
TEST(HashedCollectionGc, AtPutGrowRemoveAndEnumerateWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);
  ao::Root d(b.roots, send0(b, b.wk.dictionaryClass, "new"));
  ASSERT_TRUE(d.slot.isHeap());
  ao::RootedArray keys(b.roots, 8);
  for (std::uint32_t i = 0; i < 8; ++i) {
    keys[i] = ao::Str::fromUtf8(b.heap, b.wk, "gcKey" + std::to_string(i));
    ASSERT_TRUE(keys[i].isHeap());
  }
  for (std::uint32_t i = 0; i < 6; ++i) {
    send2(b, d.slot, "at:put:", keys[i], smi(i));
  }
  ASSERT_TRUE(b.heap.inNursery(d.slot));

  // 7 件目は容量 8 の 3/4 を超えるので、拡張する。
  fillNursery(b);
  b.heap.setGcStress(1);
  EXPECT_EQ(smi(6), send2(b, d.slot, "at:put:", keys[6], smi(6)));
  b.heap.setGcStress(0);
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ(smi(7), send0(b, d.slot, "size"));
  for (std::uint32_t i = 0; i < 7; ++i) {
    EXPECT_EQ(smi(i), send1(b, d.slot, "at:", keys[i])) << i;
  }

  fillNursery(b);
  b.heap.setGcStress(1);
  EXPECT_EQ(smi(3), send1(b, d.slot, "removeKey:", keys[3]));
  EXPECT_EQ(smi(70), send2(b, d.slot, "at:put:", keys[0], smi(70)));
  b.heap.setGcStress(0);
  EXPECT_EQ(smi(6), send0(b, d.slot, "size"));
  EXPECT_TRUE(send1(b, d.slot, "at:", keys[3]).isNil());
  EXPECT_EQ(smi(70), send1(b, d.slot, "at:", keys[0]));

  auto record = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    const ao::Oop v = ao::send(ctx, args[0], ctx.wk.intern("value"), nullptr, 0, nullptr);
    if (v.isSmallInteger()) {
      seenValues().push_back(v.smallIntegerValue());
    }
    return args[0];
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, record, 1));
  seenValues().clear();
  fillNursery(b);
  b.heap.setGcStress(1);
  EXPECT_EQ(d.slot, send1(b, d.slot, "associationsDo:", blk.slot));
  b.heap.setGcStress(0);
  std::vector<std::int64_t> seen = seenValues();
  std::sort(seen.begin(), seen.end());
  EXPECT_EQ((std::vector<std::int64_t>{1, 2, 4, 5, 6, 70}), seen);

  // Set も拡張する（7 件目）。
  ao::Root s(b.roots, send0(b, b.wk.setClass, "new"));
  for (std::uint32_t i = 0; i < 6; ++i) {
    send1(b, s.slot, "add:", keys[i]);
  }
  fillNursery(b);
  b.heap.setGcStress(1);
  EXPECT_EQ(keys[7], send1(b, s.slot, "add:", keys[7]));
  b.heap.setGcStress(0);
  EXPECT_EQ(smi(7), send0(b, s.slot, "size"));
  for (std::uint32_t i = 0; i < 8; ++i) {
    EXPECT_EQ(i != 6, send1(b, s.slot, "includes:", keys[i]).isTrue()) << i;
  }
}

// SPEC §3.6 の性能: 1 万件の at:put: と add: は、線形探索（以前は 4000 件で 9 秒）ではなく
// ハッシュ表で終わる。上限はゆるく取る（Debug）。
TEST(KernelBench, DictionaryTenThousandAtPut) {
  Boot b;
  ao::Root d(b.roots, send0(b, b.wk.dictionaryClass, "new"));
  const auto start = std::chrono::steady_clock::now();
  for (std::int64_t i = 1; i <= 10000; ++i) {
    send2(b, d.slot, "at:put:", smi(i), smi(i));
  }
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::printf("B9 Dictionary 10000 at:put: %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ(smi(10000), send0(b, d.slot, "size"));
  EXPECT_EQ(smi(4321), send1(b, d.slot, "at:", smi(4321)));
  EXPECT_LT(ms, 1000);
}

TEST(KernelBench, SetTenThousandAdd) {
  Boot b;
  ao::Root s(b.roots, send0(b, b.wk.setClass, "new"));
  ao::Root key(b.roots);
  const auto start = std::chrono::steady_clock::now();
  for (int i = 1; i <= 10000; ++i) {
    key.slot = ao::Str::fromUtf8(b.ctx, "element" + std::to_string(i));
    send1(b, s.slot, "add:", key.slot);
  }
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::printf("B9 Set 10000 add: %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ(smi(10000), send0(b, s.slot, "size"));
  key.slot = ao::Str::fromUtf8(b.ctx, "element4321");
  EXPECT_TRUE(send1(b, s.slot, "includes:", key.slot).isTrue());
  EXPECT_LT(ms, 1000);
}

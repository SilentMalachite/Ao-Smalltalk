// SPEC §3.6 Streams (B9): contents の種類と要素の数、String への書き込み（多バイト文字、予備、上書き、
// 差し替え、利用者の書き換え）、ReadStream の nextPut:、GC 圧下、性能。docs/claude-review/04 の失敗
// シナリオをそのまま回帰テストにする。

#include "ao_abi.h"

#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

class Streams : public ::testing::Test {
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

void acceptClass(const char* superclass, const char* name, const char* ivars) {
  const std::string def = std::string(superclass) + " subclass: #" + name +
                          "\n  instanceVariableNames: '" + ivars +
                          "'\n  classVariableNames: ''\n  poolDictionaries: ''\n  category: "
                          "'B9-Test'\n";
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
}

void acceptMethod(const char* className, const char* source) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method(className, 0, source, &err)) << err.message;
}

// A Print it of an Array holding the class name of what expr answers (as a String) and then its
// elements by at:, as `#('ByteArray' 1 2)`.
std::string describe(const std::string& expr) {
  return "| c a | c := " + expr +
         ".\n"
         "a := Array new: c size + 1. a at: 1 put: c class name asString.\n"
         "1 to: c size do: [:i | a at: i + 1 put: (c at: i)]. a";
}

}  // namespace

// docs/claude-review/04 Medium: contents は物理スロットを写し、OrderedCollection では内部の配列と
// firstIndex を返した。SPEC §3.6: at: で取り出し、OrderedCollection の系統は OrderedCollection。
TEST_F(Streams, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection) {
  EXPECT_EQ("#('OrderedCollection' 1 2)",
            printIt(describe("(ReadStream on: (OrderedCollection new add: 1; add: 2; yourself)) "
                             "contents")));
  acceptClass("OrderedCollection", "B9Ordered", "extra");
  EXPECT_EQ("#('OrderedCollection' 7)",
            printIt(describe("(ReadStream on: (B9Ordered new add: 7; yourself)) contents")));
  EXPECT_EQ("#('OrderedCollection')",
            printIt(describe("(ReadStream on: OrderedCollection new) contents")));
  EXPECT_EQ("3", printIt("| c | c := (ReadStream on: (OrderedCollection new add: 1; yourself)) "
                         "contents. c add: 3. c at: 2"));
}

// docs/claude-review/04 Medium: バイト列は 0 個として扱い、結果が全部 nil だった。
TEST_F(Streams, ContentsOnAByteArrayAnswersAByteArray) {
  EXPECT_EQ("#('ByteArray' 1 2)",
            printIt(describe("(WriteStream on: (ByteArray new: 4)) nextPut: 1; nextPut: 2; "
                             "contents")));
  EXPECT_EQ("#('ByteArray' 7 8 255)",
            printIt(describe("[| b | b := ByteArray new: 3. b at: 1 put: 7; at: 2 put: 8; at: 3 "
                             "put: 255. (ReadStream on: b) contents] value")));
  EXPECT_EQ("true", printIt("| b | b := ByteArray new: 2. (ReadStream on: b) contents ~~ b"));
}

// docs/claude-review/04 Medium: Array のサブクラスでは名前付き変数を飛ばさず、要素が 1 つずれた。
TEST_F(Streams, ContentsOnAnArraySubclassKeepsTheClassAndItsElements) {
  acceptClass("Array", "PG", "tag");
  acceptMethod("PG", "tag\n  ^tag\n");
  acceptMethod("PG", "tag: x\n  tag := x\n");
  const std::string pg =
      "[| p | p := PG new: 3. p tag: 9. p at: 1 put: 1; at: 2 put: 2; at: 3 put: 3. p] value";
  EXPECT_EQ("#('PG' 1 2 3)", printIt(describe("(ReadStream on: " + pg + ") contents")));
  EXPECT_EQ("nil", printIt("(ReadStream on: " + pg + ") contents tag"));
  EXPECT_EQ("4", printIt("(ReadStream on: " + pg + ") contents basicSize + 1"));
  EXPECT_EQ("#('PG' 5)",
            printIt(describe("(WriteStream on: (PG new: 4)) nextPut: 5; contents")));
}

// SPEC §3.6: String の系統は同じクラスの String（Symbol は String）で、1 パスで写す。
TEST_F(Streams, ContentsOnStringsAndSymbols) {
  EXPECT_EQ("'h\xC3\xA9llo'", printIt("(ReadStream on: 'h\xC3\xA9llo') contents"));
  EXPECT_EQ("'abc'", printIt("(ReadStream on: #abc) contents"));
  EXPECT_EQ("true", printIt("(ReadStream on: #abc) contents class == String"));
  EXPECT_EQ("'abc'", printIt("| r | r := ReadStream on: 'abc'. r next. r contents"));
  EXPECT_EQ("'a'", printIt("| w | w := WriteStream on: String new. w nextPutAll: 'abc'. "
                           "w position: 1. w contents"));
  EXPECT_EQ("true", printIt("| s | s := 'abc' copy. (ReadStream on: s) contents ~~ s"));
  acceptClass("String", "B9Str", "");
  EXPECT_EQ("'B9Str'",
            printIt("(ReadStream on: ((B9Str new: 2) at: 1 put: $x; at: 2 put: $y; yourself)) "
                    "contents class name asString"));
}

// SPEC §3.6: それ以外のコレクションは Array。要素は at: で取り出すので、at: の無いコレクション（Kernel
// の Interval）は doesNotUnderstand: で中断する（以前は物理スロット start stop step を写していた）。
TEST_F(Streams, ContentsOnOtherCollectionsAnswersAnArray) {
  acceptClass("SequenceableCollection", "B9Seq", "");
  acceptMethod("B9Seq", "size\n  ^3\n");
  acceptMethod("B9Seq", "at: i\n  ^i * 10\n");
  EXPECT_EQ("#(10 20 30)", printIt("(ReadStream on: B9Seq new) contents"));
  EXPECT_EQ("<eval error: doesNotUnderstand: #at:>",
            printIt("(ReadStream on: (Interval from: 1 to: 3 by: 1)) contents"));
  EXPECT_EQ("#(1 2)", printIt("(WriteStream on: (Array new: 0)) nextPut: 1; nextPut: 2; contents"));
}

// SPEC §3.6: k が大きさを超えれば at: が失敗する。答えに入らない要素は contents: element out of range。
// SmallInteger でない値と負の値は 0。
TEST_F(Streams, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit) {
  EXPECT_EQ("<eval error: at: index out of range>",
            printIt("| r | r := ReadStream on: 'ab'. r instVarAt: 3 put: 3. r contents"));
  EXPECT_EQ("<eval error: basicAt: index out of range>",
            printIt("| r | r := ReadStream on: #(1 2). r instVarAt: 3 put: 3. r contents"));
  EXPECT_EQ("#()", printIt("| r | r := ReadStream on: #(1 2). r instVarAt: 3 put: nil. r contents"));
  EXPECT_EQ("''", printIt("| r | r := ReadStream on: 'ab'. r instVarAt: 3 put: -4. r contents"));
  acceptClass("ByteArray", "B9Bytes", "");
  acceptMethod("B9Bytes", "at: i\n  ^300\n");
  EXPECT_EQ("<eval error: contents: element out of range>",
            printIt("(ReadStream on: (B9Bytes new: 2)) contents"));
  acceptClass("String", "B9Odd", "");
  acceptMethod("B9Odd", "at: i\n  ^3\n");
  EXPECT_EQ("<eval error: contents: element out of range>",
            printIt("(ReadStream on: (B9Odd new: 2)) contents"));
  acceptClass("String", "B9Masked", "");
  acceptMethod("B9Masked", "at: i\n  ^$*\n");
  EXPECT_EQ("true", printIt("(ReadStream on: (B9Masked new: 2)) contents = '**'"));
  EXPECT_EQ("true", printIt("(ReadStream on: (B9Masked new: 2)) contents class == B9Masked"));
}

// B9 review (Low): contents は k 個の答えを先に割り当ててから at: で取り出したので、readLimit を
// instVarAt:put: で大きくすると、巨大な領域を取ってから失敗した（2^31 なら取れずに out of memory）。
// at: が Kernel のネイティブなら、割り当てる前に at: と同じ理由で失敗する（SPEC §3.6）。
TEST_F(Streams, ContentsChecksTheRangeBeforeAllocating) {
  const std::string huge = "2147483648";  // 2^31 elements: 16 GB of slots
  EXPECT_EQ("<eval error: basicAt: index out of range>",
            printIt("| r | r := ReadStream on: #(1 2). r instVarAt: 3 put: " + huge + ". r contents"));
  EXPECT_EQ("<eval error: basicAt: index out of range>",
            printIt("| r | r := ReadStream on: (ByteArray new: 2). r instVarAt: 3 put: " + huge +
                    ". r contents"));
  EXPECT_EQ("<eval error: at: index out of range>",
            printIt("| r | r := ReadStream on: 'ab'. r instVarAt: 3 put: " + huge + ". r contents"));
  EXPECT_EQ("<eval error: at: index out of range>",
            printIt("| r | r := ReadStream on: (OrderedCollection new add: 1; yourself). "
                    "r instVarAt: 3 put: " + huge + ". r contents"));
  acceptClass("Array", "B9Tagged", "tag");
  EXPECT_EQ("<eval error: basicAt: index out of range>",
            printIt("| r | r := ReadStream on: (B9Tagged new: 2). r instVarAt: 3 put: 3. r contents"));
  EXPECT_EQ("true", printIt("| c | c := (ReadStream on: (B9Tagged new: 2)) contents. "
                            "(c class == B9Tagged) & (c size = 2)"));
  // A damaged OrderedCollection fails as its at: does; an empty contents reads nothing.
  EXPECT_EQ("<eval error: damaged ordered collection>",
            printIt("| o r | o := OrderedCollection new add: 1; yourself. r := ReadStream on: o. "
                    "o instVarAt: 3 put: 9. r contents"));
  EXPECT_EQ("0", printIt("| o r | o := OrderedCollection new add: 1; yourself. r := ReadStream on: o. "
                         "o instVarAt: 3 put: 9. r instVarAt: 3 put: 0. r contents size"));
  // Within range nothing changes.
  EXPECT_EQ("#(1 2)", printIt("(ReadStream on: #(1 2)) contents"));
  EXPECT_EQ("#(1)", printIt("| r | r := ReadStream on: #(1 2). r instVarAt: 3 put: 1. r contents"));
}

// docs/claude-review/04 Low: ReadWriteStream の contents が position までだった。Blue Book どおり
// readLimit と position の大きい方まで返す。WriteStream は position まで。
TEST_F(Streams, ReadWriteStreamContentsReachesTheReadLimit) {
  EXPECT_EQ("'abc'", printIt("| w | w := ReadWriteStream on: String new. w nextPutAll: 'abc'; "
                             "reset. w contents"));
  EXPECT_EQ("$a", printIt("| w | w := ReadWriteStream on: String new. w nextPutAll: 'abc'; reset. "
                          "w contents. w next"));
  EXPECT_EQ("#(1 2)", printIt("| w | w := ReadWriteStream on: (Array new: 0). w nextPut: 1; "
                              "nextPut: 2; reset. w contents"));
  EXPECT_EQ("''", printIt("| w | w := WriteStream on: String new. w nextPutAll: 'abc'; reset. "
                          "w contents"));
  EXPECT_EQ("true", printIt("ReadWriteStream includesSelector: #contents"));
}

// docs/claude-review/04 Low: ReadStream の nextPut: が元のコレクションを書き換えた。
TEST_F(Streams, ReadStreamNextPutFailsAndLeavesTheCollection) {
  EXPECT_EQ("<eval error: shouldNotImplement>",
            printIt("| s r | s := 'abc' copy. r := ReadStream on: s. r nextPut: $x. s"));
  EXPECT_EQ("<eval error: shouldNotImplement>",
            printIt("| s r | s := 'abc' copy. r := ReadStream on: s. "
                    "[r nextPut: $x] ensure: [Smalltalk at: #B9Kept put: s. "
                    "Smalltalk at: #B9Pos put: r position]"));
  EXPECT_EQ("'abc'", printIt("B9Kept"));
  EXPECT_EQ("0", printIt("B9Pos"));
  EXPECT_EQ("<eval error: shouldNotImplement>", printIt("(ReadStream on: #(1 2)) nextPut: 3"));
  EXPECT_EQ("false", printIt("PositionableStream includesSelector: #nextPut:"));
  EXPECT_EQ("true", printIt("WriteStream includesSelector: #nextPut:"));
  EXPECT_EQ("true", printIt("ReadStream includesSelector: #nextPut:"));
  EXPECT_EQ("$y", printIt("| w | w := ReadWriteStream on: String new. w nextPut: $y. w reset. "
                          "w next"));
}

// docs/claude-review/04 Medium: 固定長の String に書くと、多バイト文字を黙って捨てた。
TEST_F(Streams, FixedStringTakesWideCharacters) {
  EXPECT_EQ("'\xC3\xA9\xC3\xA9\xC3\xA9'",
            printIt("| w | w := WriteStream on: (String new: 8). w nextPutAll: "
                    "'\xC3\xA9\xC3\xA9\xC3\xA9'. w contents"));
  EXPECT_EQ("3", printIt("| w | w := WriteStream on: (String new: 8). w nextPutAll: "
                         "'\xC3\xA9\xC3\xA9\xC3\xA9'. w position"));
  EXPECT_EQ("'a\xF0\x9D\x84\x9E\xE3\x81\x82" "b'",
            printIt("| w | w := WriteStream on: (String new: 2). w nextPutAll: "
                    "'a\xF0\x9D\x84\x9E\xE3\x81\x82" "b'. w contents"));
  // 同じ幅はその場で書く。幅が変われば新しい String に差し替え、利用者の String は変えない。
  EXPECT_EQ("$b", printIt("| s w | s := String new: 3. w := WriteStream on: s. "
                          "w nextPut: $a; nextPut: $b. s at: 2"));
  EXPECT_EQ("true", printIt("| s w | s := String new: 3. w := WriteStream on: s. "
                            "w nextPut: $a. (w instVarAt: 1) == s"));
  EXPECT_EQ("0", printIt("| s w | s := String new: 3. w := WriteStream on: s. "
                         "w nextPut: $\xC3\xA9. (s at: 1) asInteger"));
  EXPECT_EQ("<eval error: nextPut: value out of range>",
            printIt("(WriteStream on: String new) nextPut: 3"));
  EXPECT_EQ("<eval error: nextPut: value out of range>",
            printIt("(WriteStream on: (String new: 2)) nextPut: 3"));
  EXPECT_EQ("<eval error: nextPut: value out of range>",
            printIt("| w | w := WriteStream on: (String new: 2). "
                    "[w nextPut: 3] ensure: [Smalltalk at: #B9Pos put: w position]. 1"));
  EXPECT_EQ("0", printIt("B9Pos"));
}

// SPEC §3.6: position で戻って書いた文字だけを置き換え、ほかの文字はそのまま残る。
TEST_F(Streams, OverwritingWrittenCharactersKeepsTheRest) {
  EXPECT_EQ("#('h\xC3\xA9llo' 'j\xC3\xA9llo' 'jello' 'jell\xE3\x81\x82' $l)",
            printIt("| w a | w := ReadWriteStream on: String new. w nextPutAll: 'hello'.\n"
                    "a := Array new: 5.\n"
                    "w position: 1. w nextPut: $\xC3\xA9. a at: 1 put: w contents.\n"
                    "w position: 0. w nextPut: $j. a at: 2 put: w contents.\n"
                    "w position: 1. w nextPut: $e. a at: 3 put: w contents.\n"
                    "w position: 4. w nextPut: $\xE3\x81\x82. a at: 4 put: w contents.\n"
                    "w position: 2. a at: 5 put: w next. a"));
  EXPECT_EQ("'he'", printIt("| w | w := WriteStream on: String new. w nextPutAll: 'hello'. "
                            "w position: 1. w nextPut: $e. w contents"));
}

// SPEC §3.6: 利用者が on: に渡した String を at:put: で書き換えても（幅が変わっても）、ストリームは
// 正しい位置に書く。
TEST_F(Streams, UserWritesToTheCollectionAreSeen) {
  EXPECT_EQ("'\xC3\xA9" "bXdef'",
            printIt("| s w | s := 'abcdef' copy. w := ReadWriteStream on: s. w position: 2. "
                    "s at: 1 put: $\xC3\xA9. w nextPut: $X. w contents"));
  EXPECT_EQ("'ab\xC3\xA9\xC3\xA9'",
            printIt("| s w | s := '\xC3\xA9\xC3\xA9\xC3\xA9\xC3\xA9' copy. "
                    "w := ReadWriteStream on: s. s at: 1 put: $a. w position: 1. w nextPut: $b. "
                    "w contents"));
  EXPECT_EQ("'\xC3\xA9" "bc'",
            printIt("| s w | s := 'ab' copy. w := WriteStream on: s. w position: 2. "
                    "s at: 1 put: $\xC3\xA9. w nextPut: $c. w contents"));
}

// SPEC §3.6: ストリームが作った String の予備に、広い文字をその場で書く。writeLimit は文字数のまま。
TEST_F(Streams, WideCharactersFillTheReserve) {
  EXPECT_EQ("100", printIt("| w | w := WriteStream on: String new. 100 timesRepeat: "
                           "[w nextPut: $\xE3\x81\x82]. w contents inject: 0 into: "
                           "[:n :c | c = $\xE3\x81\x82 ifTrue: [n + 1] ifFalse: [n]]"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: String new. 100 timesRepeat: "
                            "[w nextPut: $\xE3\x81\x82. w nextPut: $a]. "
                            "(w instVarAt: 1) size = (w instVarAt: 4)"));
  EXPECT_EQ("200", printIt("| w | w := WriteStream on: String new. 100 timesRepeat: "
                           "[w nextPut: $\xF0\x9D\x84\x9E. w nextPut: $a]. w contents size"));
  // position: は予備の中へ進まない（readLimit で頭打ち）ので、次の文字は書いた文字のすぐ後ろに入る。
  EXPECT_EQ("'az'", printIt("| w | w := WriteStream on: String new. w nextPut: $a. w position: 3. "
                            "w nextPut: $z. w contents"));
}

// B9 review (Low): position: の上限が writeLimit だったので、予備（ストリームが作った String の NUL や、
// 倍にした Array の空き）が見えた。`nextPutAll: 'abc'; position: 100; position` は 16、`position: 10`
// のあとの contents には NUL が 7 個入った。上限は readLimit と今の position の大きい方（SPEC §3.6）。
TEST_F(Streams, PositionDoesNotRevealTheReserve) {
  EXPECT_EQ("3", printIt("(WriteStream on: String new) nextPutAll: 'abc'; position: 100; position"));
  EXPECT_EQ("'abc'", printIt("| w | w := WriteStream on: String new. w nextPutAll: 'abc'. "
                             "w position: 10. w contents"));
  EXPECT_EQ("#(1 2 3)", printIt("| w | w := WriteStream on: (Array new: 0). w nextPut: 1; nextPut: 2; "
                                "nextPut: 3. w position: 10. w contents"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: (Array new: 0). w nextPut: 1; nextPut: 2; "
                            "nextPut: 3. w position: 10. (w position = 3) & ((w instVarAt: 4) = 4)"));
  // Backwards is fine, and back up to what was written.
  EXPECT_EQ("#(1 3)", printIt("| w a | w := ReadWriteStream on: String new. w nextPutAll: 'abc'. "
                              "a := Array new: 2. w position: 1. a at: 1 put: w position. "
                              "w position: 50. a at: 2 put: w position. a"));
  EXPECT_EQ("0", printIt("| w | w := WriteStream on: String new. w nextPutAll: 'abc'. w position: -4. "
                         "w position"));
  EXPECT_EQ("<eval error: position: not an integer>",
            printIt("(WriteStream on: String new) position: 1.5"));
  // A ReadStream reads up to readLimit; a user String passed to on: shows all of it.
  EXPECT_EQ("2", printIt("| r | r := ReadStream on: 'ab'. r position: 9. r position"));
  EXPECT_EQ("5", printIt("| w | w := WriteStream on: (String new: 5). w position: 9. w position"));
  // A position past readLimit (only reflection makes one) is kept as the bound.
  EXPECT_EQ("#(7 5)", printIt("| w a | w := WriteStream on: String new. w nextPutAll: 'ab'. "
                              "w instVarAt: 2 put: 7. a := Array new: 2. w position: 9. "
                              "a at: 1 put: w position. w position: 5. a at: 2 put: w position. a"));
}

// B9 review (Medium): 予備を足した writeLimit（position + 1 + 予備）が SmallInteger を超え、Debug と
// ASan は fromSmallInteger の assert、Release は負の writeLimit になった。予備を削って頭打ちにする。
// ほかのスロットも極端な値にして、失敗か正しい値で終わることを見る（SPEC §3.6 ストリーム）。
TEST_F(Streams, ExtremeSlotValuesStayInRange) {
  const std::string big = "4611686018427387902";  // SmallInteger max - 1
  const std::string max = "4611686018427387903";  // SmallInteger max
  EXPECT_EQ("true",
            printIt("| w | w := WriteStream on: String new. w instVarAt: 2 put: " + big +
                    "; instVarAt: 4 put: " + big + ". w nextPut: $a. "
                    "((w instVarAt: 4) = " + max + ") & ((w instVarAt: 2) = " + max + ") & "
                    "((w instVarAt: 4) class == SmallInteger) & ((w instVarAt: 3) = " + max + ")"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: String new. w instVarAt: 2 put: " + big +
                            "; instVarAt: 4 put: " + big + ". w nextPut: $\xE3\x81\x82. "
                            "(w instVarAt: 4) = " + max));
  // Without a matching writeLimit, the String has fewer characters than the position.
  EXPECT_EQ("<eval error: nextPut: past end>",
            printIt("| w | w := WriteStream on: String new. w instVarAt: 2 put: " + big +
                    ". w nextPut: $a"));
  EXPECT_EQ("<eval error: nextPut: position out of range>",
            printIt("| w | w := WriteStream on: String new. w instVarAt: 2 put: " + max +
                    ". w nextPut: $a"));
  EXPECT_EQ("<eval error: nextPut: position out of range>",
            printIt("| w | w := WriteStream on: (Array new: 2). w instVarAt: 2 put: " + max +
                    ". w nextPut: 1"));
  // An Array past 2^32 - 1 slots cannot be made: out of memory, and the stream is left alone.
  EXPECT_EQ("<eval error: out of memory>",
            printIt("| w | w := WriteStream on: (Array new: 2). w instVarAt: 2 put: 4294967296. "
                    "w nextPut: 1"));
  EXPECT_EQ("<eval error: out of memory>",
            printIt("| w | w := WriteStream on: (Array new: 2). w instVarAt: 2 put: 4294967296. "
                    "[w nextPut: 1] ensure: [Smalltalk at: #B9W put: w]"));
  EXPECT_EQ("true", printIt("((B9W instVarAt: 1) size = 2) & ((B9W instVarAt: 2) = 4294967296)"));
  // Huge or odd writeLimit and readLimit values do not break the writes that follow.
  for (const std::string& limit : {max, std::string("-5"), std::string("nil"), std::string("'x'")}) {
    SCOPED_TRACE(limit);
    EXPECT_EQ("'ab\xE3\x81\x82'",
              printIt("| w | w := WriteStream on: String new. w instVarAt: 4 put: " + limit +
                      ". w nextPut: $a; nextPut: $b; nextPut: $\xE3\x81\x82. w contents"));
    EXPECT_EQ("'ab'", printIt("| w | w := WriteStream on: String new. w instVarAt: 3 put: " + limit +
                              ". w nextPut: $a; nextPut: $b. w contents"));
  }
  EXPECT_EQ("#(1 2)", printIt("| w | w := WriteStream on: (Array new: 0). w instVarAt: 4 put: " + max +
                              ". w nextPut: 1; nextPut: 2. w contents"));
}

// B9 review (Low): size を上書きした String のサブクラスでも、at:put: が Kernel のものなら高速路を
// 通り、size を無視して別の位置に書いた。高速路は size もネイティブのときだけで、ほかは size と
// at:put: を送る（SPEC §3.6）。
TEST_F(Streams, StringSubclassOverridingSizeGetsSizeSent) {
  acceptClass("String", "B9Sized", "");
  acceptMethod("B9Sized", "size\n  Smalltalk at: #B9Sizes put: (Smalltalk at: #B9Sizes) + 1.\n"
                          "  ^super size\n");
  ASSERT_EQ("0", printIt("Smalltalk at: #B9Sizes put: 0"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: (B9Sized new: 2). Smalltalk at: #B9Sizes "
                            "put: 0. w nextPut: $a; nextPut: $b. w contents = 'ab'"));
  EXPECT_EQ("2", printIt("Smalltalk at: #B9Sizes"));
  // A size larger than the characters: at: 3 put: fails in the String, as the generic path says.
  acceptClass("String", "B9Tall", "");
  acceptMethod("B9Tall", "size\n  ^super size * 2\n");
  EXPECT_EQ("<eval error: at:put: index out of range>",
            printIt("| w | w := WriteStream on: ((B9Tall new: 2) at: 1 put: $a; at: 2 put: $b; "
                    "yourself). w position: 2. w nextPut: $x"));
}

// B9 review (Low): 汎用の経路で末尾に足すと、String のサブクラスが素の String に変わった。
// collection のクラスを保つ（Symbol の系統だけ String。SPEC §3.6）。
TEST_F(Streams, AppendingKeepsTheStringSubclass) {
  acceptClass("String", "B9Loud", "");
  acceptMethod("B9Loud", "at: i put: c\n  ^super at: i put: c\n");
  EXPECT_EQ("'B9Loud'",
            printIt("| w | w := WriteStream on: (B9Loud new: 0). w nextPut: $a; nextPut: $\xC3\xA9. "
                    "(w instVarAt: 1) class name asString"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: (B9Loud new: 0). w nextPut: $a; "
                            "nextPut: $\xC3\xA9. w contents = 'a\xC3\xA9'"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: (B9Loud new: 0). w nextPut: $a. "
                            "w contents class == B9Loud"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: #ab. w position: 2. w nextPut: $c. "
                            "(w instVarAt: 1) class == String"));
}

// Symbol の系統は at:put: が Kernel の String のものでないので、at:put: を送る（shouldNotImplement）。
// 末尾の次の文字は、新しい String に足す。
TEST_F(Streams, SymbolCollectionsSendAtPut) {
  EXPECT_EQ("<eval error: shouldNotImplement>", printIt("(WriteStream on: #abc) nextPut: $x"));
  EXPECT_EQ("'abcd'", printIt("| w | w := WriteStream on: #abc. w position: 3. w nextPut: $d. "
                              "w contents"));
  EXPECT_EQ("true", printIt("| w | w := WriteStream on: #abc. w position: 3. w nextPut: $d. "
                            "w contents class == String"));
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

ao::Oop smi(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

std::string utf8(const std::u32string& text) {
  std::string out;
  for (const char32_t cp : text) {
    if (cp < 0x80) {
      out.push_back(static_cast<char>(cp));
    } else if (cp < 0x800) {
      out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
      out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
      out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
      out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
      out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
      out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
      out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
      out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
      out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
  }
  return out;
}

// GC を走らせずに nursery を使い切る（残りは 16 B 未満）。
void fillNursery(Boot& b) {
  while (b.heap.allocate(ao::Oop::nil(), 0, 0).isHeap()) {
  }
}

}  // namespace

// SPEC §3.6: String への書き込みを、文字の列のモデルと突き合わせる。1 から 4 バイトの文字の追記と
// 上書き、position: での後戻り、reset からの next、利用者の String への at:put:（ストリームがまだそれを
// collection にしている間だけ、モデルにも反映する）を乱数で混ぜる。毎回 contents、position、readLimit、
// writeLimit と collection の文字数を確かめる。
TEST(StreamDifferential, StringWritesMatchAModel) {
  Boot b;
  const char32_t alphabet[] = {U'a', U'z', U'\0', U'é', U'あ', U'\U0001D11E'};
  std::mt19937 rng(20260925);
  auto pick = [&](std::uint32_t n) {
    return static_cast<std::uint32_t>(std::uniform_int_distribution<std::uint32_t>(0, n - 1)(rng));
  };
  std::u32string model = U"abéあcd";
  ao::Root user(b.roots, ao::Str::fromUtf8(b.ctx, utf8(model)));
  ao::Root w(b.roots, send1(b, b.wk.readWriteStreamClass, "on:", user.slot));
  ASSERT_TRUE(w.slot.isHeap());
  std::size_t pos = 0;
  ao::Root got(b.roots);
  for (int step = 0; step < 3000; ++step) {
    SCOPED_TRACE(step);
    const std::uint32_t op = pick(10);
    if (op < 6) {
      const char32_t c = alphabet[pick(6)];
      ASSERT_EQ(ao::Oop::fromCharacter(c), send1(b, w.slot, "nextPut:", ao::Oop::fromCharacter(c)))
          << takeAbortReason(b);
      if (pos < model.size()) {
        model[pos] = c;
      } else {
        model.push_back(c);
      }
      ++pos;
    } else if (op < 8) {
      pos = pick(static_cast<std::uint32_t>(model.size()) + 1);
      send1(b, w.slot, "position:", smi(static_cast<std::int64_t>(pos)));
    } else if (op == 8) {
      send0(b, w.slot, "reset");
      pos = 0;
      const std::size_t reads = pick(4);
      for (std::size_t i = 0; i < reads && pos < model.size(); ++i) {
        ASSERT_EQ(ao::Oop::fromCharacter(model[pos]), send0(b, w.slot, "next"));
        ++pos;
      }
    } else if (b.heap.slotAt(w.slot, 0) == user.slot && !model.empty()) {
      const std::size_t i = pick(static_cast<std::uint32_t>(model.size()));
      const char32_t c = alphabet[pick(6)];
      const ao::Oop r = send2(b, user.slot, "at:put:", smi(static_cast<std::int64_t>(i + 1)),
                              ao::Oop::fromCharacter(c));
      if (r.isEmpty()) {
        takeAbortReason(b);  // UTF-8 width mismatch: the String is unchanged.
      } else {
        model[i] = c;
      }
    }
    got.slot = send0(b, w.slot, "contents");
    ASSERT_TRUE(got.slot.isHeap()) << takeAbortReason(b);
    ASSERT_EQ(utf8(model), ao::Str::toUtf8(b.heap, got.slot));
    ASSERT_EQ(smi(static_cast<std::int64_t>(pos)), b.heap.slotAt(w.slot, 1));
    ASSERT_EQ(smi(static_cast<std::int64_t>(model.size())), b.heap.slotAt(w.slot, 2));
    const ao::Oop count = send0(b, b.heap.slotAt(w.slot, 0), "size");
    ASSERT_EQ(count, b.heap.slotAt(w.slot, 3));
  }
}

// GC 圧下: nursery を満杯にしてから、String と Array の WriteStream を伸ばし、ByteArray、
// OrderedCollection、String の contents を取る。GC ストレス中は解放した領域を毒で埋めるので、古い番地を
// 読めば落ちる。
TEST(StreamGc, GrowAndContentsWithFullNursery) {
  Boot b;
  b.heap.setGcStress(0);
  ao::Root ws(b.roots, send1(b, b.wk.writeStreamClass, "on:", ao::Str::fromUtf8(b.ctx, "")));
  ao::Root wa(b.roots, send1(b, b.wk.writeStreamClass, "on:",
                             send1(b, b.wk.arrayClass, "new:", smi(0))));
  ao::Root elt(b.roots);
  fillNursery(b);
  b.heap.setGcStress(1);
  std::u32string expect;
  for (int i = 0; i < 40; ++i) {
    const char32_t c = (i % 3 == 0) ? U'あ' : static_cast<char32_t>(U'a' + i % 26);
    send1(b, ws.slot, "nextPut:", ao::Oop::fromCharacter(c));
    expect.push_back(c);
    elt.slot = ao::Str::fromUtf8(b.ctx, "x" + std::to_string(i));
    send1(b, wa.slot, "nextPut:", elt.slot);
  }
  ao::Root sc(b.roots, send0(b, ws.slot, "contents"));
  ao::Root ac(b.roots, send0(b, wa.slot, "contents"));
  b.heap.setGcStress(0);
  ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  EXPECT_EQ(utf8(expect), ao::Str::toUtf8(b.heap, sc.slot));
  ASSERT_EQ(40u, b.heap.size(ac.slot));
  for (std::uint32_t i = 0; i < 40; ++i) {
    EXPECT_EQ("x" + std::to_string(i), ao::Str::toUtf8(b.heap, b.heap.slotAt(ac.slot, i))) << i;
  }

  ao::Root oc(b.roots, send0(b, b.wk.orderedCollectionClass, "new"));
  ao::Root bytes(b.roots, send1(b, b.wk.byteArrayClass, "new:", smi(3)));
  for (std::int64_t i = 1; i <= 3; ++i) {
    elt.slot = ao::Str::fromUtf8(b.ctx, "o" + std::to_string(i));
    send1(b, oc.slot, "add:", elt.slot);
    send2(b, bytes.slot, "at:put:", smi(i), smi(10 * i));
  }
  ao::Root ro(b.roots, send1(b, b.wk.readStreamClass, "on:", oc.slot));
  ao::Root rb(b.roots, send1(b, b.wk.readStreamClass, "on:", bytes.slot));
  fillNursery(b);
  b.heap.setGcStress(1);
  ao::Root oco(b.roots, send0(b, ro.slot, "contents"));
  ao::Root bco(b.roots, send0(b, rb.slot, "contents"));
  b.heap.setGcStress(0);
  ASSERT_FALSE(b.ctx.aborting) << takeAbortReason(b);
  EXPECT_EQ(b.wk.orderedCollectionClass, b.heap.klass(oco.slot));
  EXPECT_EQ(smi(3), send0(b, oco.slot, "size"));
  EXPECT_EQ("o2", ao::Str::toUtf8(b.heap, send1(b, oco.slot, "at:", smi(2))));
  EXPECT_EQ(b.wk.byteArrayClass, b.heap.klass(bco.slot));
  EXPECT_EQ(smi(30), send1(b, bco.slot, "at:", smi(3)));
}

namespace {

std::int64_t timedEval(Boot& b, const std::string& body, std::string* printed) {
  const auto start = std::chrono::steady_clock::now();
  ao::Root v(b.roots, evalBody(b, body));
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  *printed = b.ctx.aborting ? "<abort: " + takeAbortReason(b) + ">"
                            : ao::Str::toUtf8(b.heap, send0(b, v.slot, "printString"));
  return ms;
}

}  // namespace

// docs/claude-review/04 Medium の計測（Debug）: ReadStream の contents は 40000 文字で 6.2 秒、
// WriteStream への nextPut: $a 2 万回は 3.3 秒だった。1 パスの contents と倍々の予備で 1 秒未満。
TEST(KernelBench, ReadStreamContentsOfFortyThousandCharacters) {
  Boot b;
  std::string printed;
  const auto ms = timedEval(b, "^(ReadStream on: (String new: 40000)) contents size", &printed);
  std::printf("B9 ReadStream contents 40000 chars %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ("40000", printed);
  EXPECT_LT(ms, 1000);
}

TEST(KernelBench, WriteStreamTwentyThousandNextPut) {
  Boot b;
  std::string printed;
  auto ms = timedEval(b,
                      "| w | w := WriteStream on: String new. 20000 timesRepeat: [w nextPut: $a]. "
                      "^w contents size",
                      &printed);
  std::printf("B9 WriteStream 20000 nextPut: $a %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ("20000", printed);
  EXPECT_LT(ms, 1000);
  ms = timedEval(b,
                 "| w | w := WriteStream on: String new. 20000 timesRepeat: [w nextPut: "
                 "$\xE3\x81\x82]. ^w contents size",
                 &printed);
  std::printf("B9 WriteStream 20000 nextPut: wide %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ("20000", printed);
  EXPECT_LT(ms, 1000);
  ms = timedEval(b,
                 "| w | w := WriteStream on: (Array new: 0). 20000 timesRepeat: [w nextPut: 1]. "
                 "^w contents size",
                 &printed);
  std::printf("B9 WriteStream 20000 nextPut: into an Array %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ("20000", printed);
  EXPECT_LT(ms, 1000);
}

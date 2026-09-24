#include "test_support.hpp"
#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Lookup.hpp"
#include <cstring>
#include <gtest/gtest.h>
#include <string>

TEST(ChunkFileIn, InstallsCompiledMethodAndKeepsOldOnError) {
  Boot b;
  const char* def =
      "!Object subclass: #CmUser\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P5-Test'!\n"
      "!CmUser methodsFor: 't'!\n"
      "ok\n"
      "  ^1 + 2!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(def, errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs)) << (errs.empty() ? "" : errs[0].message);
  auto cls = b.wk.named("CmUser");
  ASSERT_TRUE(cls.isHeap());
  auto meth = ao::lookup(b.heap, cls, b.wk.intern("ok"));
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(meth));

  const char* bad =
      "!CmUser methodsFor: 't'!\n"
      "ok\n"
      "  ^1 +!\n";
  errs.clear();
  auto badActs = ao::compiler::parseChunks(bad, errs);
  ao::applyChunks(b.ctx, badActs, errs);
  EXPECT_FALSE(errs.empty());
  auto still = ao::lookup(b.heap, cls, b.wk.intern("ok"));
  EXPECT_EQ(meth, still);
}

TEST(ChunkFileIn, DoItIsNotEvaluated) {
  Boot b;
  const char* src = "!1 + 2!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_EQ(1u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::DoIt, acts[0].kind);
  EXPECT_TRUE(ao::applyChunks(b.ctx, acts, errs));
}

TEST(ChunkFileIn, InstVarReadCompilesPushInstVar) {
  Boot b;
  const char* src =
      "!Object subclass: #CmIvar\n"
      "  instanceVariableNames: 'a'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P5-Test'!\n"
      "!CmIvar methodsFor: 't'!\n"
      "x\n"
      "  ^a!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs)) << (errs.empty() ? "" : errs[0].message);
  auto cls = b.wk.named("CmIvar");
  ASSERT_TRUE(cls.isHeap());
  auto meth = ao::lookup(b.heap, cls, b.wk.intern("x"));
  ASSERT_TRUE(meth.isHeap());
  auto ba = send0(b, meth, "bytecodes");
  ASSERT_TRUE(ba.isHeap());
  ao::compiler::MethodImage image;
  image.bytes.resize(b.heap.size(ba));
  if (!image.bytes.empty()) {
    std::memcpy(image.bytes.data(), b.heap.bytes(ba), image.bytes.size());
  }
  const std::string d = ao::compiler::disassemble(image);
  EXPECT_NE(std::string::npos, d.find("PushInstVar 0")) << d;
}

// SPEC §3.12: メソッド単位のエラーは残りのチャンクを止めないが、1 件でもあれば file-in は失敗である。
TEST(ChunkFileIn, MethodErrorFailsFileInButKeepsGoing) {
  Boot b;
  const char* src =
      "!Object subclass: #CmHalf\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n"
      "!CmHalf methodsFor: 't'!\n"
      "bad\n"
      "  ^1 +!\n"
      "good\n"
      "  ^3! !\n";
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_FALSE(ao::fileInString(b.ctx, src, errs));
  ASSERT_EQ(1u, errs.size());
  const ao::Oop cls = b.wk.named("CmHalf");
  ASSERT_TRUE(cls.isHeap());
  EXPECT_TRUE(ao::lookup(b.heap, cls, b.wk.intern("good")).isHeap());
  EXPECT_TRUE(ao::lookup(b.heap, cls, b.wk.intern("bad")).isNil());
}

// SPEC §3.4 / §3.12: クラス定義チャンクの送信は最外の評価である。そこで起きた abort はクラス定義の
// 失敗として理由付きのエラーになり、状態は消えて、次の評価に持ち越さない。
TEST(ChunkFileIn, ClassDefinitionAbortIsAnErrorAndIsCleared) {
  Boot b;
  const char* src =
      "!Processor subclass: #CmNotAClass\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n";
  std::vector<ao::compiler::CompileError> errs;
  EXPECT_FALSE(ao::fileInString(b.ctx, src, errs));
  ASSERT_EQ(1u, errs.size());
  EXPECT_EQ(
      "subclass failed: CmNotAClass: doesNotUnderstand: "
      "#subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:",
      errs[0].message);
  EXPECT_FALSE(ao::unwinding(b.ctx));
  EXPECT_FALSE(b.wk.named("CmNotAClass").isHeap());
}

// 指摘 9 / SPEC §3.12: チャンク単位のエラーの位置は、そのチャンク（区切りの ! を除く）のバイト
// 範囲である。そのエラーでファイルの残りのチャンクは止まる。
TEST(ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan) {
  struct Case {
    const char* chunk;
    const char* message;
  };
  const Case cases[] = {
      {"Object subclass: #Object\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
       "  poolDictionaries: ''\n  category: 'K'",
       "refusing to redefine kernel class: Object"},
      {"B3NoSuper subclass: #B3Child\n  category: 'K'", "missing class: B3NoSuper"},
      {"B3NoSuch methodsFor: 'x'", "missing class: B3NoSuch"},
      {"Object methodsFor: 'x'", "refusing to redefine kernel class: Object"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.chunk);
    Boot b;
    const std::string src = std::string("Transcript show: 'lead'!\n\n!") + c.chunk +
                            "!\nfoo\n  ^1! !\n"
                            "!Object subclass: #B3After\n  category: 'K'!\n";
    std::vector<ao::compiler::CompileError> errs;
    EXPECT_FALSE(ao::fileInString(b.ctx, src, errs));
    ASSERT_EQ(1u, errs.size());
    EXPECT_EQ(c.message, errs[0].message);
    const std::string chunk(c.chunk);
    ASSERT_LE(errs[0].span.start, errs[0].span.end);
    ASSERT_LE(errs[0].span.end, src.size());
    EXPECT_EQ(chunk, src.substr(errs[0].span.start, errs[0].span.end - errs[0].span.start));
    EXPECT_FALSE(b.wk.named("B3After").isHeap());
  }
}

// 05 High / SPEC §3.8 チャンク形式: `$'` と `$"` は文字リテラルで、文字列もコメントも開かない。
// そのあとのメソッドとクラス側のセクションも読み込む。
TEST(ChunkFileIn, QuoteCharacterLiteralsKeepLaterChunks) {
  Boot b;
  const char* src =
      "!Object subclass: #B7Quote\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B7-Test'!\n"
      "!B7Quote methodsFor: 'a'!\n"
      "quote\n"
      "  ^$'!\n"
      "isDq: c\n"
      "  ^c = $\"!\n"
      "two\n"
      "  ^2! !\n"
      "!B7Quote class methodsFor: 'b'!\n"
      "three\n"
      "  ^3! !\n";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named("B7Quote"));
  ASSERT_TRUE(cls.slot.isHeap());
  ao::Root inst(b.roots, send0(b, cls.slot, "new"));
  EXPECT_EQ(ao::Oop::fromCharacter(U'\''), send0(b, inst.slot, "quote"));
  EXPECT_EQ(ao::Oop::true_(), send1(b, inst.slot, "isDq:", ao::Oop::fromCharacter(U'"')));
  EXPECT_EQ(ao::Oop::fromSmallInteger(2), send0(b, inst.slot, "two"));
  EXPECT_EQ(ao::Oop::fromSmallInteger(3), send0(b, cls.slot, "three"));
}

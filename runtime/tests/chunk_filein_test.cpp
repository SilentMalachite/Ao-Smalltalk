#include "test_support.hpp"
#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
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

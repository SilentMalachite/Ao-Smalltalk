#include "test_support.hpp"
#include "ao/Chunk.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Lookup.hpp"
#include <gtest/gtest.h>

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

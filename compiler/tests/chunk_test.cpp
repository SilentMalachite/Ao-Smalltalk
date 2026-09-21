#include "ao/Chunk.hpp"
#include <gtest/gtest.h>

TEST(Chunk, SplitsMethodsForAndProtocol) {
  const char* src =
      "!Foo methodsFor: 'accessing' stamp: 'ao 1/1/2026'!\n"
      "x\n"
      "  ^1!\n"
      "y: a\n"
      "  ^a!\n"
      "\n"
      "!Foo class methodsFor: 'instance creation'!\n"
      "new\n"
      "  ^super new!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(2u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[0].kind);
  EXPECT_EQ("Foo", acts[0].className);
  EXPECT_FALSE(acts[0].meta);
  EXPECT_EQ("accessing", acts[0].protocol);
  ASSERT_EQ(2u, acts[0].methods.size());
  EXPECT_TRUE(acts[1].meta);
}

TEST(Chunk, ClassDefinitionShape) {
  const char* src =
      "!Object subclass: #Foo\n"
      "  instanceVariableNames: 'a b'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Test-Foo'!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_EQ(1u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::ClassDef, acts[0].kind);
  EXPECT_EQ("Foo", acts[0].className);
  EXPECT_EQ("Object", acts[0].superName);
  EXPECT_EQ("a b", acts[0].instVars);
}

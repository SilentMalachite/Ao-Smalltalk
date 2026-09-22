#include "test_support.hpp"

#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/Globals.hpp"
#include "ao/Lookup.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(VendorOverwrite, RefusesNewSelectorOnObject) {
  Boot b;
  const char* src =
      "!Object methodsFor: 'vendor'!\n"
      "vendorProbe\n"
      "  ^1!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  EXPECT_FALSE(ao::applyChunks(b.ctx, acts, errs));
  auto meth = ao::lookup(b.heap, b.wk.objectClass, b.wk.intern("vendorProbe"));
  EXPECT_TRUE(meth.isNil());
}

TEST(VendorOverwrite, KeepsNativeIdentityEquals) {
  Boot b;
  const char* src =
      "!Object methodsFor: 'comparing'!\n"
      "== anObject\n"
      "  ^false!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ao::applyChunks(b.ctx, acts, errs);
  auto meth = ao::lookup(b.heap, b.wk.objectClass, b.wk.intern("=="));
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(meth));
}

TEST(VendorOverwrite, RebindsBagAndEvaluatesInstVar) {
  Boot b;
  const char* src =
      "!Collection subclass: #Bag\n"
      "  instanceVariableNames: 'contents'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P6b-Test'!\n"
      "!Bag methodsFor: 'accessing'!\n"
      "contents\n"
      "  ^contents!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs)) << (errs.empty() ? "" : errs[0].message);
  EXPECT_EQ(b.wk.bagClass, b.wk.named("Bag"));
  EXPECT_EQ(b.wk.bagClass, ao::Globals::at(b.wk, "Bag"));
  EXPECT_EQ(b.wk.bagMetaclass, b.heap.klass(b.wk.bagClass));
  auto meth = ao::lookup(b.heap, b.wk.bagClass, b.wk.intern("contents"));
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(meth));
  auto names = b.heap.slotAt(b.wk.bagClass, ao::kClassSlotInstVarNames);
  ASSERT_TRUE(names.isHeap());
  EXPECT_EQ(1u, b.heap.size(names));
  auto bag = send0(b, b.wk.bagClass, "new");
  ASSERT_TRUE(bag.isHeap());
  EXPECT_TRUE(send0(b, bag, "contents").isNil());
}

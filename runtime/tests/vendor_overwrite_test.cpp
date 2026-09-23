#include "test_support.hpp"

#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
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

namespace {

// Smalltalk at: #IntegerAlias put: SmallInteger と、メタクラスの別名 IntegerMetaAlias を作る。
void aliasSmallInteger(Boot& b) {
  ao::Root name(b.roots, b.wk.intern("IntegerAlias"));
  send2(b, b.wk.smalltalk, "at:put:", name.slot, b.wk.smallIntegerClass);
  name.slot = b.wk.intern("IntegerMetaAlias");
  send2(b, b.wk.smalltalk, "at:put:", name.slot, b.wk.smallIntegerMetaclass);
}

}  // namespace

// SPEC §3.12: Kernel クラスかどうかは、名前で引いた先のクラスそのもので決める。別名の methodsFor: も
// インスタンス側・クラス側とも丸ごと拒み、SmallInteger から引く <= はネイティブのままである。
// SendSpecial の高速路（3 <= 4）と送信（perform:with:）の答えも食い違わない。
TEST(VendorOverwrite, RefusesKernelClassThroughAlias) {
  Boot b;
  aliasSmallInteger(b);
  ASSERT_EQ(b.wk.smallIntegerClass, b.wk.named("IntegerAlias"));
  struct Side {
    const char* name;
    const char* src;
  };
  const Side sides[] = {
      {"IntegerAlias", "!IntegerAlias methodsFor: 'comparing'!\n<= x\n  ^false! !\n"},
      {"IntegerAlias", "!IntegerAlias class methodsFor: 'probes'!\nr2Probe\n  ^3! !\n"},
      {"IntegerMetaAlias", "!IntegerMetaAlias methodsFor: 'probes'!\nr2Probe\n  ^3! !\n"},
  };
  for (const Side& side : sides) {
    SCOPED_TRACE(side.src);
    std::vector<ao::compiler::CompileError> errs;
    EXPECT_FALSE(ao::fileInString(b.ctx, side.src, errs));
    EXPECT_EQ(std::string("refusing to redefine kernel class: ") + side.name,
              errs.empty() ? std::string() : errs[0].message);
  }
  const ao::Oop le = ao::lookup(b.heap, b.wk.smallIntegerClass, b.wk.intern("<="));
  ASSERT_TRUE(le.isHeap());
  EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(le));
  EXPECT_TRUE(ao::lookup(b.heap, b.wk.smallIntegerMetaclass, b.wk.intern("r2Probe")).isNil());

  auto img = ao::compiler::compileMethod("doIt\n  ^3 <= 4");
  ASSERT_TRUE(img.ok) << img.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  const ao::Oop fast =
      ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  ao::Root sel(b.roots, b.wk.intern("<="));
  const ao::Oop sent = send2(b, ao::Oop::fromSmallInteger(3), "perform:with:", sel.slot,
                             ao::Oop::fromSmallInteger(4));
  EXPECT_TRUE(fast.isTrue());
  EXPECT_TRUE(sent.isTrue());
}

// 別名を名乗るクラス定義は、新しいクラスを作って別名のグローバルを付け替えるだけである。
// SmallInteger そのものは変わらない（SPEC §3.12 の methodsFor: の拒否とは別）。
TEST(VendorOverwrite, ClassDefinitionThroughAliasOnlyRebindsGlobal) {
  Boot b;
  aliasSmallInteger(b);
  const char* src =
      "!Object subclass: #IntegerAlias\n"
      "  instanceVariableNames: 'x'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
  const ao::Oop alias = b.wk.named("IntegerAlias");
  ASSERT_TRUE(alias.isHeap());
  EXPECT_NE(b.wk.smallIntegerClass, alias);
  EXPECT_EQ(b.wk.objectClass, b.heap.slotAt(alias, ao::kClassSlotSuperclass));
  EXPECT_EQ(b.wk.integerClass, b.heap.slotAt(b.wk.smallIntegerClass, ao::kClassSlotSuperclass));
  EXPECT_EQ(b.wk.smallIntegerClass, b.wk.named("SmallInteger"));
}

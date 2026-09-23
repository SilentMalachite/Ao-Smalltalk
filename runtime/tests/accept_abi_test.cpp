#include "ao_abi.h"

#include <cstdio>
#include <cstring>
#include <string>

#include <gtest/gtest.h>

TEST(AcceptAbi, ObjectFooIsCallableAndKeepsSource) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* src = "foo\n  ^1\n";
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, src, &err));
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("Object new foo", 14, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("1", out);
  char shown[256];
  ASSERT_EQ(AO_OK, ao_browser_source("Object", 0, "foo", shown, 256));
  EXPECT_NE(std::string(shown).find("^1"), std::string::npos);
  const char* bad = "foo\n  ^\n";
  ASSERT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, bad, &err));
  ASSERT_EQ(AO_OK, ao_browser_source("Object", 0, "foo", shown, 256));
  EXPECT_NE(std::string(shown).find("^1"), std::string::npos);
  ASSERT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, "printString\n  ^1\n", &err));
  EXPECT_NE(std::string(err.message).find("native selector overwrite refused"), std::string::npos);
  ao_runtime_shutdown();
}

TEST(AcceptAbi, ClassDefinitionThenImageDropsSourceText) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* def =
      "Object subclass: #P9Foo\n"
      "  instanceVariableNames: 'x'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P9-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err));
  char category[64];
  char name[64];
  bool found = false;
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, name, 64, category, 64));
    if (std::strcmp(name, "P9Foo") == 0) {
      found = true;
      EXPECT_STREQ("P9-Test", category);
    }
  }
  EXPECT_TRUE(found);
  ASSERT_EQ(AO_OK, ao_accept_method("P9Foo", 0, "foo\n  ^x\n", &err));
  const char* path = "accept-abi.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("P9Foo new foo", 13, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
  char shown[256];
  ASSERT_EQ(AO_OK, ao_browser_source("P9Foo", 0, "foo", shown, 256));
  EXPECT_NE(std::string(shown).find("CompiledMethod"), std::string::npos);
  ao_runtime_shutdown();
  std::remove(path);
}

// SPEC §3.10: Kernel クラスでは、引くとネイティブに当たるセレクタ（上位クラスから継承したネイティブ）
// も拒む。インスタンス側とクラス側の両方。Kernel クラスへの新しいセレクタと、Kernel でないクラスで
// 継承したネイティブを上書きするのは受け付ける。
TEST(AcceptAbi, AcceptRefusesShadowingInheritedNativeInKernelClass) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  struct Refused {
    const char* cls;
    int meta;
    const char* source;
    const char* selector;
  };
  const Refused refused[] = {
      {"SmallInteger", 0, "<= x\n  ^false\n", "<="},  // Magnitude>><=
      {"Integer", 0, "<= x\n  ^false\n", "<="},       // Magnitude>><=
      {"SmallInteger", 0, "< x\n  ^false\n", "<"},    // Integer>><
      {"SmallInteger", 1, "new\n  ^3\n", "new"},      // Behavior>>new
  };
  for (const Refused& r : refused) {
    SCOPED_TRACE(r.source);
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method(r.cls, r.meta, r.source, &err));
    EXPECT_EQ(std::string("native selector overwrite refused: ") + r.selector,
              std::string(err.message));
  }
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("3 <= 4", 6, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("true", out);

  EXPECT_EQ(AO_OK, ao_accept_method("Object", 0, "r2New\n  ^1\n", &err)) << err.message;
  const char* def =
      "Object subclass: #B3Shadow\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err)) << err.message;
  EXPECT_EQ(AO_OK, ao_accept_method("B3Shadow", 0, "printString\n  ^'shadow'\n", &err))
      << err.message;
  EXPECT_EQ(AO_OK, ao_accept_method("B3Shadow", 0, "<= x\n  ^false\n", &err)) << err.message;
  EXPECT_EQ(AO_OK, ao_accept_method("B3Shadow", 1, "new\n  ^super new\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_eval("B3Shadow new <= 1", 17, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("false", out);
  ao_runtime_shutdown();
}

// SPEC §3.10: Kernel クラスかどうかは、名前で引いた先のクラスそのもので決める。別名で指しても、
// 継承したネイティブを隠すセレクタは拒む。SendSpecial の高速路と送信の答えは食い違わない。
// Kernel でないクラスの別名は、これまでどおり受け付ける。
TEST(AcceptAbi, AcceptRefusesKernelClassThroughAlias) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto run = [&](const char* src, int mode) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), mode, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, run("Smalltalk at: #IntegerAlias put: SmallInteger", AO_EVAL_DOIT))
      << err.message;
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("IntegerAlias", 0, "<= x\n  ^false\n", &err));
  EXPECT_EQ(std::string("native selector overwrite refused: <="), std::string(err.message));
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("IntegerAlias", 1, "new\n  ^3\n", &err));
  EXPECT_EQ(std::string("native selector overwrite refused: new"), std::string(err.message));
  ASSERT_EQ(AO_OK, run("Smalltalk at: #IntegerMetaAlias put: SmallInteger class", AO_EVAL_DOIT))
      << err.message;
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("IntegerMetaAlias", 0, "new\n  ^3\n", &err));
  EXPECT_EQ(std::string("native selector overwrite refused: new"), std::string(err.message));
  ASSERT_EQ(AO_OK, run("3 <= 4", AO_EVAL_PRINTIT)) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, run("3 perform: #<= with: 4", AO_EVAL_PRINTIT)) << err.message;
  EXPECT_STREQ("true", out);

  const char* def =
      "Object subclass: #B3Aliased\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err)) << err.message;
  ASSERT_EQ(AO_OK, run("Smalltalk at: #B3Alias put: B3Aliased", AO_EVAL_DOIT)) << err.message;
  EXPECT_EQ(AO_OK, ao_accept_method("B3Alias", 0, "<= x\n  ^false\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, run("B3Aliased new <= 1", AO_EVAL_PRINTIT)) << err.message;
  EXPECT_STREQ("false", out);
  ao_runtime_shutdown();
}

TEST(AcceptAbi, ObjectSubclassObjectIsCompileError) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const int before = ao_browser_class_count();
  ASSERT_EQ(AO_ERR_COMPILE, ao_accept_class("Object subclass: #Object", &err));
  EXPECT_EQ(before, ao_browser_class_count());
  ao_runtime_shutdown();
}

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

// SPEC §3.3 キャッシュの無効化: キャッシュは受信側のクラスで引く。Object>>zork を再 Accept したら、
// `3 zork` が SmallInteger の下に残したエントリも捨てる（定義クラスの分だけでは足りない）。
TEST(AcceptAbi, ReacceptReachesSendCachedForSubclassReceiver) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "zork\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("3 zork")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "zork\n  ^2\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("3 zork")) << err.message;
  EXPECT_STREQ("2", out);
  ASSERT_EQ(AO_OK, printIt("Object new zork")) << err.message;
  EXPECT_STREQ("2", out);
  ao_runtime_shutdown();
}

// SPEC §3.3: メソッドチャンク（file-in、ao_accept_class）による再定義も、キャッシュ済みの送信に届く。
// 定義クラスのインスタンスにも、継承するサブクラスのインスタンスにも。
TEST(AcceptAbi, ChunkRedefinitionReachesCachedSends) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B3R5M\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B3-Test'\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class("B3R5M subclass: #B3R5M2\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B3-Test'\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B3R5M", 0, "foo\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("B3R5M new foo")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, printIt("B3R5M2 new foo")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, ao_accept_class("!B3R5M methodsFor: 'x'!\nfoo\n  ^2! !", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("B3R5M new foo")) << err.message;
  EXPECT_STREQ("2", out);
  ASSERT_EQ(AO_OK, printIt("B3R5M2 new foo")) << err.message;
  EXPECT_STREQ("2", out);
  ao_runtime_shutdown();
}

// 00 Critical / SPEC §3.10: ao_accept_class はクラス定義メッセージと、チャンク形式のクラス定義・
// methodsFor: のチャンクだけを受け付ける。ほかのチャンクが 1 つでもあれば、全体を検査してから
// 何も適用せずに AO_ERR_COMPILE「not a class definition」を返す。
TEST(AcceptAbi, AcceptClassRefusesNonDefinitionWithoutApplying) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const int before = ao_browser_class_count();
  const char* refused[] = {
      "3 + 4",
      "zork\n  ^5\n",
      "   \n",
      // 式のあとにクラス定義。
      "3 + 4!\n"
      "Object subclass: #B3NotDefA\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n",
      // クラス定義のあとに式。
      "Object subclass: #B3NotDefB\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n"
      "3 + 4!\n",
      // 式のあとに methodsFor: のチャンク。
      "3 + 4!\n"
      "!Object methodsFor: 'b3'!\n"
      "b3Zap\n"
      "  ^1! !\n",
  };
  for (const char* src : refused) {
    SCOPED_TRACE(src);
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(src, &e));
    EXPECT_STREQ("not a class definition", e.message);
  }
  EXPECT_EQ(before, ao_browser_class_count());
  char buf[256];
  EXPECT_EQ(AO_ERR, ao_browser_class_definition("B3NotDefA", buf, 256));
  EXPECT_EQ(AO_ERR, ao_browser_class_definition("B3NotDefB", buf, 256));
  char out[64];
  EXPECT_EQ(AO_ERR_EVAL, ao_eval("Object new b3Zap", 16, AO_EVAL_PRINTIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval("3 + 4", 5, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("7", out);
  ao_runtime_shutdown();
}

// SPEC §3.10: チャンク形式のクラス定義と methodsFor: のチャンクを 1 つのソースで受け付ける。
TEST(AcceptAbi, AcceptClassTakesChunkDefinitionAndMethods) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* src =
      "Object subclass: #B3Chunked\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n"
      "!B3Chunked methodsFor: 'b3'!\n"
      "seven\n"
      "  ^7! !\n";
  ASSERT_EQ(AO_OK, ao_accept_class(src, &err)) << err.message;
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("B3Chunked new seven", 19, AO_EVAL_PRINTIT, out, 64, &err))
      << err.message;
  EXPECT_STREQ("7", out);
  ao_runtime_shutdown();
}

// 00 Critical / SPEC §3.10: 名前で引いた先がクラス（Behavior）でなければ、何もせずに AO_ERR。
TEST(AcceptAbi, AcceptMethodRefusesNonBehaviorName) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto run = [&](const char* src, int mode) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), mode, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, run("Smalltalk at: #B3NotClass put: 'text'", AO_EVAL_DOIT)) << err.message;
  const char* names[] = {"Processor", "Smalltalk", "nil", "B3NoSuchClass", "B3NotClass"};
  for (const char* name : names) {
    SCOPED_TRACE(name);
    for (int meta = 0; meta <= 1; ++meta) {
      EXPECT_EQ(AO_ERR, ao_accept_method(name, meta, "b3Foo\n  ^1\n", &err));
    }
  }
  ASSERT_EQ(AO_OK, run("3 + 4", AO_EVAL_PRINTIT)) << err.message;
  EXPECT_STREQ("7", out);
  ASSERT_EQ(AO_OK, run("Smalltalk at: #B3NotClass", AO_EVAL_PRINTIT)) << err.message;
  EXPECT_STREQ("'text'", out);
  ao_runtime_shutdown();
}

// 指摘 1 / SPEC §3.10: methodsFor: のグループは `! !` で終わる。そのあとのヘッダでないチャンクは
// 式として拒み、グループの前のメソッドも入れない（部分適用しない）。
TEST(AcceptAbi, AcceptClassRefusesChunksAfterSectionEnd) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B3Px\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B3-Test'\n",
                                   &err))
      << err.message;
  const char* refused[] = {
      "!B3Px methodsFor: 'x'!\nfoo\n  ^1! !\n3 + 4!\n",
      "!B3Px methodsFor: 'x'!\nbar\n  ^2! !\nSmalltalk halt!\n",
      "!B3Px methodsFor: 'x'! !\nbaz\n  ^3!\n",
      "!B3Px methodsFor: 'x'!\nfoo\n  ^1!\n! !\n3 + 4!\n",
  };
  for (const char* src : refused) {
    SCOPED_TRACE(src);
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(src, &e));
    EXPECT_STREQ("not a class definition", e.message);
  }
  const char* absent[] = {"B3Px new foo", "B3Px new bar", "B3Px new baz", "B3Px new Smalltalk",
                          "B3Px new three"};
  for (const char* src : absent) {
    SCOPED_TRACE(src);
    EXPECT_EQ(AO_ERR_EVAL, printIt(src));
  }
  // ヘッダごとに `! !` で閉じる形は受け付ける。
  ASSERT_EQ(AO_OK, ao_accept_class("!B3Px methodsFor: 'x'!\nfoo\n  ^1! !\n"
                                   "!B3Px methodsFor: 'y'!\nbar\n  ^2! !\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("B3Px new foo + B3Px new bar")) << err.message;
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
}

// 指摘 8 / SPEC §3.10: クラス定義メッセージのあとに文が続けば、何も適用せずに拒む。
TEST(AcceptAbi, AcceptClassRefusesStatementsAfterDefinition) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  const int before = ao_browser_class_count();
  const char* refused[] = {
      "Object subclass: #B3Pq instanceVariableNames: '' classVariableNames: '' "
      "poolDictionaries: '' category: 'P'.\nSmalltalk at: #B3Zz put: 1",
      "Object subclass: #B3Pq\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      "  poolDictionaries: ''\n  category: 'P'\nSmalltalk at: #B3Zz put: 1\n",
      "Object subclass: #B3Pq category: 'P' at: #B3Zz put: 1",
  };
  for (const char* src : refused) {
    SCOPED_TRACE(src);
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(src, &e));
    EXPECT_STREQ("not a class definition", e.message);
  }
  EXPECT_EQ(before, ao_browser_class_count());
  char buf[256];
  EXPECT_EQ(AO_ERR, ao_browser_class_definition("B3Pq", buf, 256));
  ASSERT_EQ(AO_OK, printIt("(Smalltalk at: #B3Zz) isNil")) << err.message;
  EXPECT_STREQ("true", out);
  // 末尾の `.` だけなら受け付ける。
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B3Pq instanceVariableNames: '' "
                                   "classVariableNames: '' poolDictionaries: '' category: 'P'.\n",
                                   &err))
      << err.message;
  EXPECT_EQ(AO_OK, ao_browser_class_definition("B3Pq", buf, 256));
  ao_runtime_shutdown();
}

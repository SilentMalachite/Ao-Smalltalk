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
  // SPEC §3.10: 読み込んだイメージのメソッドはソースが無い。AO_ERR_NOSOURCE とプレースホルダで、
  // プレースホルダを Accept しても本体は変わらない（self を返す空メソッドにならない）。
  char shown[256];
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("P9Foo", 0, "foo", shown, 256));
  EXPECT_STREQ("\"P9Foo>>foo source not available\"", shown);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("P9Foo", 0, shown, &err));
  EXPECT_STRNE("", err.message);
  ASSERT_EQ(AO_OK, ao_eval("P9Foo new foo", 13, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
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

namespace {

// A class definition message for ao_accept_class.
std::string b5Definition(const char* superName, const char* name, const char* instVars,
                         const char* category) {
  std::string def = superName;
  def += " subclass: #";
  def += name;
  def += "\n  instanceVariableNames: '";
  def += instVars;
  def += "'\n  classVariableNames: ''\n  poolDictionaries: ''\n  category: '";
  def += category;
  def += "'\n";
  return def;
}

// The class list's category heading of className, or "<missing>".
std::string b5Category(const char* className) {
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    char name[128];
    char category[128];
    if (ao_browser_class_at(i, name, 128, category, 128) == AO_OK &&
        std::string(name) == className) {
      return category;
    }
  }
  return "<missing>";
}

std::string b5ClassDefinition(const char* className) {
  char defn[512];
  if (ao_browser_class_definition(className, defn, 512) != AO_OK) {
    return "<missing>";
  }
  return defn;
}

}  // namespace

// 00 Critical / SPEC §3.9「クラス定義の再 Accept」: 失敗シナリオ。Foo2>>m を Accept したあと、同じ
// 定義文を ao_accept_class で受け付け直しても AO_OK で、Foo2 new m は 42 のまま。形が同じなので、
// クラスオブジェクト、既存インスタンス、ソースもそのまま。
TEST(AcceptAbi, ReacceptSameClassDefinitionKeepsClassAndMethods) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  const std::string def = b5Definition("Object", "Foo2", "", "B5-Test");
  ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
  const char* m = "m\n  ^42\n";
  ASSERT_EQ(AO_OK, ao_accept_method("Foo2", 0, m, &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("Foo2 new m")) << err.message;
  EXPECT_STREQ("42", out);
  ASSERT_EQ(AO_OK, printIt("oldFoo2 := Foo2. oldInst := Foo2 new. oldInst m")) << err.message;
  EXPECT_STREQ("42", out);

  ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("Foo2 new m")) << err.message;
  EXPECT_STREQ("42", out);
  ASSERT_EQ(AO_OK, printIt("oldFoo2 == Foo2")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("oldInst class == Foo2")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("oldInst m")) << err.message;
  EXPECT_STREQ("42", out);
  char source[256];
  ASSERT_EQ(AO_OK, ao_browser_source("Foo2", 0, "m", source, 256));
  EXPECT_STREQ(m, source);
  ao_runtime_shutdown();
}

// SPEC §3.9: 形が同じなら category だけが変わる（instanceVariableNames は空白の違いを問わない）。
// インスタンス側とクラス側のメソッド、ソースの無いメソッドも、そのまま残る。
TEST(AcceptAbi, ReacceptWithNewCategoryKeepsClassAndMethods) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  const std::string def = b5Definition("Object", "B5Cat", "a b", "B5-Old");
  const std::string chunks = def + "!\n!B5Cat methodsFor: 'b5'!\neight\n  ^8! !\n";
  ASSERT_EQ(AO_OK, ao_accept_class(chunks.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Cat", 0, "a: v\n  a := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Cat", 0, "a\n  ^a\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Cat", 1, "make\n  ^self new a: 5\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldCat := B5Cat. inst := B5Cat make. inst a")) << err.message;
  EXPECT_STREQ("5", out);
  EXPECT_EQ("B5-Old", b5Category("B5Cat"));

  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B5Cat instanceVariableNames: ' a  b ' "
                                   "classVariableNames: '' poolDictionaries: '' "
                                   "category: 'B5-New'",
                                   &err))
      << err.message;
  EXPECT_EQ("B5-New", b5Category("B5Cat"));
  const std::string defn = b5ClassDefinition("B5Cat");
  EXPECT_NE(defn.find("instanceVariableNames: 'a b'"), std::string::npos) << defn;
  EXPECT_NE(defn.find("category: 'B5-New'"), std::string::npos) << defn;
  ASSERT_EQ(AO_OK, printIt("oldCat == B5Cat")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("inst class == B5Cat")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("inst a")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("B5Cat make a")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("B5Cat new eight")) << err.message;
  EXPECT_STREQ("8", out);
  char source[256];
  EXPECT_EQ(AO_ERR_NOSOURCE, ao_browser_source("B5Cat", 0, "eight", source, 256));
  ASSERT_EQ(AO_OK, ao_browser_source("B5Cat", 1, "make", source, 256));
  EXPECT_STREQ("make\n  ^self new a: 5\n", source);
  ao_runtime_shutdown();
}

// SPEC §3.9: 形が変わるときは新しいクラスを作り、ソース表にあるメソッドを（インスタンス側も
// クラス側も）新しい形でコンパイルし直して移す。w を x の前に足すので、x を読み書きするメソッドは
// コンパイルし直したものでなければ 2 番目のスロットに届かない。旧クラスと既存インスタンスは、
// 旧クラスのメソッドで動く。
TEST(AcceptAbi, ReacceptWithNewInstanceVariableRecompilesMethods) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("Object", "B5Shape", "x", "B5-Test").c_str(), &err))
      << err.message;
  const char* getX = "x\n  ^x\n";
  const char* setX = "x: v\n  x := v\n";
  const char* withX = "withX: v\n  ^self new x: v\n";
  ASSERT_EQ(AO_OK, ao_accept_method("B5Shape", 0, getX, &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Shape", 0, setX, &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Shape", 1, withX, &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldShape := B5Shape. oldInst := B5Shape withX: 7. oldInst x"))
      << err.message;
  EXPECT_STREQ("7", out);

  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B5Shape", "w x", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Shape", 0, "w: v\n  w := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("(B5Shape withX: 5) x")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("(B5Shape withX: 5) w: 9; x")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("(B5Shape withX: 5) class == B5Shape")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("oldShape == B5Shape")) << err.message;
  EXPECT_STREQ("false", out);
  ASSERT_EQ(AO_OK, printIt("oldInst class == oldShape")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("oldInst x")) << err.message;
  EXPECT_STREQ("7", out);
  ASSERT_EQ(AO_OK, printIt("(oldInst x: 8) x")) << err.message;
  EXPECT_STREQ("8", out);

  // Browser ABI から見ても、メソッド、プロトコル、ソースが新しいクラスにある。
  EXPECT_NE(b5ClassDefinition("B5Shape").find("instanceVariableNames: 'w x'"), std::string::npos);
  EXPECT_EQ("B5-Test", b5Category("B5Shape"));
  ASSERT_EQ(1, ao_browser_protocol_count("B5Shape", 0));
  char buf[256];
  ASSERT_EQ(AO_OK, ao_browser_protocol_at("B5Shape", 0, 0, buf, 256));
  EXPECT_STREQ("user", buf);
  ASSERT_EQ(3, ao_browser_selector_count("B5Shape", 0, "user"));
  const char* selectors[] = {"w:", "x", "x:"};
  for (int i = 0; i < 3; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_selector_at("B5Shape", 0, "user", i, buf, 256));
    EXPECT_STREQ(selectors[i], buf);
  }
  ASSERT_EQ(AO_OK, ao_browser_source("B5Shape", 0, "x", buf, 256));
  EXPECT_STREQ(getX, buf);
  ASSERT_EQ(AO_OK, ao_browser_source("B5Shape", 0, "x:", buf, 256));
  EXPECT_STREQ(setX, buf);
  ASSERT_EQ(1, ao_browser_selector_count("B5Shape", 1, "user"));
  ASSERT_EQ(AO_OK, ao_browser_source("B5Shape", 1, "withX:", buf, 256));
  EXPECT_STREQ(withX, buf);
  ao_runtime_shutdown();
}

// SPEC §3.9: 形が変わるとき、ソース表にソースの無いメソッド（ao_accept_class の methodsFor: の
// チャンクで入れたもの）が 1 つでもあれば、何も変えずに AO_ERR_COMPILE。メッセージは当たった
// セレクタを含む。名前は旧クラスを指したままで、メソッドも動く。形が同じなら受け付ける。
TEST(AcceptAbi, ShapeChangeRefusesSourcelessMethodAndKeepsOldClass) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  const std::string instSide = b5Definition("Object", "B5NoSrc", "a", "B5-Test") +
                               "!\n!B5NoSrc methodsFor: 'b5'!\neight\n  ^8! !\n";
  ASSERT_EQ(AO_OK, ao_accept_class(instSide.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5NoSrc", 0, "a\n  ^a\n", &err)) << err.message;
  const std::string classSide = b5Definition("Object", "B5NoSrcMeta", "a", "B5-Test") +
                                "!\n!B5NoSrcMeta class methodsFor: 'b5'!\nmake\n  ^self new! !\n";
  ASSERT_EQ(AO_OK, ao_accept_class(classSide.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5NoSrcMeta", 0, "a\n  ^a\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldNoSrc := B5NoSrc. oldNoSrcMeta := B5NoSrcMeta")) << err.message;

  struct Refused {
    const char* name;
    const char* method;
  };
  const Refused refused[] = {{"B5NoSrc", "B5NoSrc>>eight"}, {"B5NoSrcMeta", "B5NoSrcMeta class>>make"}};
  for (const Refused& r : refused) {
    SCOPED_TRACE(r.name);
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE,
              ao_accept_class(b5Definition("Object", r.name, "a b", "B5-Test").c_str(), &e));
    EXPECT_STRNE("", e.message);
    EXPECT_NE(std::string(e.message).find(r.method), std::string::npos) << e.message;
    EXPECT_NE(b5ClassDefinition(r.name).find("instanceVariableNames: 'a'\n"), std::string::npos);
  }
  ASSERT_EQ(AO_OK, printIt("oldNoSrc == B5NoSrc")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("oldNoSrcMeta == B5NoSrcMeta")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("B5NoSrc new eight")) << err.message;
  EXPECT_STREQ("8", out);
  ASSERT_EQ(AO_OK, printIt("B5NoSrcMeta make class == B5NoSrcMeta")) << err.message;
  EXPECT_STREQ("true", out);
  char source[256];
  ASSERT_EQ(AO_OK, ao_browser_source("B5NoSrc", 0, "a", source, 256));
  EXPECT_STREQ("a\n  ^a\n", source);

  // 形が同じなら、ソースの無いメソッドがあっても受け付ける。
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B5NoSrc", "a", "B5-Kept").c_str(), &err))
      << err.message;
  EXPECT_EQ("B5-Kept", b5Category("B5NoSrc"));
  ASSERT_EQ(AO_OK, printIt("oldNoSrc == B5NoSrc")) << err.message;
  EXPECT_STREQ("true", out);
  ao_runtime_shutdown();
}

// SPEC §3.9: 形が変わるとき、コンパイルし直しが 1 つでも失敗すれば（消したインスタンス変数 y に
// 代入するメソッド）、何も変えずに AO_ERR_COMPILE。メッセージは当たったセレクタを含む。
TEST(AcceptAbi, ShapeChangeRefusesMethodThatDoesNotRecompile) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B5Drop", "x y", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Drop", 0, "x\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Drop", 0, "y\n  ^y\n", &err)) << err.message;
  const char* setY = "y: v\n  y := v\n";
  ASSERT_EQ(AO_OK, ao_accept_method("B5Drop", 0, setY, &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldDrop := B5Drop. (B5Drop new y: 3) y")) << err.message;
  EXPECT_STREQ("3", out);

  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("Object", "B5Drop", "x", "B5-Test").c_str(), &e));
  EXPECT_STRNE("", e.message);
  EXPECT_NE(std::string(e.message).find("B5Drop>>y:"), std::string::npos) << e.message;
  ASSERT_EQ(AO_OK, printIt("oldDrop == B5Drop")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("(B5Drop new y: 4) y")) << err.message;
  EXPECT_STREQ("4", out);
  EXPECT_NE(b5ClassDefinition("B5Drop").find("instanceVariableNames: 'x y'"), std::string::npos);
  char source[256];
  ASSERT_EQ(AO_OK, ao_browser_source("B5Drop", 0, "y:", source, 256));
  EXPECT_STREQ(setY, source);
  ao_runtime_shutdown();
}

// SPEC §3.9: サブクラスのあるクラスの形は変えない（サブクラスの付け替えは v1 でしない）。
// AO_ERR_COMPILE で、メッセージはサブクラスがあることを言う。形が同じなら受け付け、サブクラスも
// 同じクラスを指したまま。サブクラスの無いクラスの形は変えられる。
TEST(AcceptAbi, ShapeChangeRefusesClassWithSubclasses) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B5Base", "a", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("B5Base", "B5Sub", "", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5Base", 0, "a\n  ^a\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldBase := B5Base. B5Sub superclass == B5Base")) << err.message;
  EXPECT_STREQ("true", out);

  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("Object", "B5Base", "a b", "B5-Test").c_str(), &e));
  EXPECT_NE(std::string(e.message).find("B5Base has subclasses"), std::string::npos) << e.message;
  ASSERT_EQ(AO_OK, printIt("oldBase == B5Base")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("B5Sub superclass == B5Base")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("B5Sub new a")) << err.message;
  EXPECT_STREQ("nil", out);
  EXPECT_NE(b5ClassDefinition("B5Base").find("instanceVariableNames: 'a'\n"), std::string::npos);

  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B5Base", "a", "B5-Moved").c_str(), &err))
      << err.message;
  EXPECT_EQ("B5-Moved", b5Category("B5Base"));
  ASSERT_EQ(AO_OK, printIt("oldBase == B5Base and: [B5Sub superclass == B5Base]")) << err.message;
  EXPECT_STREQ("true", out);

  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("B5Base", "B5Sub", "c", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("B5Sub superclass == B5Base")) << err.message;
  EXPECT_STREQ("true", out);
  EXPECT_NE(b5ClassDefinition("B5Sub").find("instanceVariableNames: 'c'"), std::string::npos);
  ao_runtime_shutdown();
}

// B5 review M1 / SPEC §3.9: 形を変える subclass:… の送信が失敗したら（superclass のクラス側で
// 上書きしたメソッドが super を送って名前を新しいクラスに付け替えたあと、nil を答える、または
// 中断する）、AO_ERR_COMPILE で、名前は旧クラスに戻る。旧クラスのメソッドとソースもそのまま。
TEST(AcceptAbi, ShapeChangePutsNameBackWhenSubclassSendFails) {
  const char* header =
      "subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p "
      "category: k\n"
      "  super subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p "
      "category: k.\n";
  const char* endings[] = {"  ^nil\n", "  ^self error: 'after'\n"};
  for (const char* ending : endings) {
    SCOPED_TRACE(ending);
    ASSERT_EQ(AO_OK, ao_runtime_boot());
    AoSpan err{};
    char out[64];
    auto printIt = [&](const char* src) {
      return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
    };
    const std::string sup = b5Definition("Object", "B5FailSup", "", "B5-Test");
    ASSERT_EQ(AO_OK, ao_accept_class(sup.c_str(), &err)) << err.message;
    const std::string child = b5Definition("B5FailSup", "B5FailChild", "a", "B5-Test");
    ASSERT_EQ(AO_OK, ao_accept_class(child.c_str(), &err)) << err.message;
    const char* getA = "a\n  ^a\n";
    ASSERT_EQ(AO_OK, ao_accept_method("B5FailChild", 0, getA, &err)) << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method("B5FailChild", 0, "a: v\n  a := v\n", &err)) << err.message;
    ASSERT_EQ(AO_OK, printIt("oldChild := B5FailChild. (B5FailChild new a: 5) a")) << err.message;
    EXPECT_STREQ("5", out);
    const std::string override = std::string(header) + ending;
    ASSERT_EQ(AO_OK, ao_accept_method("B5FailSup", 1, override.c_str(), &err)) << err.message;

    AoSpan e{};
    const std::string reshaped = b5Definition("B5FailSup", "B5FailChild", "a b", "B5-Test");
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(reshaped.c_str(), &e));
    EXPECT_STRNE("", e.message);
    ASSERT_EQ(AO_OK, printIt("oldChild == B5FailChild")) << err.message;
    EXPECT_STREQ("true", out);
    ASSERT_EQ(AO_OK, printIt("(B5FailChild new a: 6) a")) << err.message;
    EXPECT_STREQ("6", out);
    char source[256];
    ASSERT_EQ(AO_OK, ao_browser_source("B5FailChild", 0, "a", source, 256));
    EXPECT_STREQ(getA, source);
    EXPECT_NE(b5ClassDefinition("B5FailChild").find("instanceVariableNames: 'a'\n"),
              std::string::npos);
    ao_runtime_shutdown();
  }
}

// B5 review M2 / SPEC §3.9: 形が変わるとき、インスタンス側のメソッドが、旧クラスにあって新しい形に
// 無いインスタンス変数（継承したものを含む）を名前で読めば、何も変えずに AO_ERR_COMPILE。新しい形では
// その読みが大域変数の読み出しになるからである。ブロックの中、入れ子のブロックの中、インライン化した
// ブロックの中の読みも数える。名前は旧クラスを指したままで、メソッドも旧クラスのまま動く。
TEST(AcceptAbi, ShapeChangeRefusesMethodReadingRemovedInstanceVariable) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const std::string& src) {
    return ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT, out, 64, &err);
  };
  for (const std::string& sup : {b5Definition("Object", "B5RdOldSup", "y", "B5-Test"),
                                 b5Definition("Object", "B5RdNewSup", "", "B5-Test")}) {
    ASSERT_EQ(AO_OK, ao_accept_class(sup.c_str(), &err)) << err.message;
  }

  struct Case {
    const char* name;
    const char* oldSuper;
    const char* oldVars;
    const char* newSuper;
    const char* newVars;
    const char* selector;
    const char* source;
    const char* expected;
  };
  const Case cases[] = {
      {"B5RdPlain", "Object", "x y", "Object", "x", "y", "y\n  ^y\n",
       "shape change refused: B5RdPlain>>y refers to removed instance variable y"},
      {"B5RdBlock", "Object", "x y", "Object", "x", "yIsNil",
       "yIsNil\n  ^[:k | y isNil] value: 1\n",
       "shape change refused: B5RdBlock>>yIsNil refers to removed instance variable y"},
      {"B5RdNested", "Object", "x y", "Object", "x", "nested", "nested\n  ^[[y]] value value\n",
       "shape change refused: B5RdNested>>nested refers to removed instance variable y"},
      {"B5RdInline", "Object", "x y", "Object", "x", "inl",
       "inl\n  ^x isNil ifTrue: [y] ifFalse: [x]\n",
       "shape change refused: B5RdInline>>inl refers to removed instance variable y"},
      {"B5RdRename", "Object", "x y", "Object", "x z", "y", "y\n  ^y\n",
       "shape change refused: B5RdRename>>y refers to removed instance variable y"},
      {"B5RdInherit", "B5RdOldSup", "x", "B5RdNewSup", "x", "y", "y\n  ^y\n",
       "shape change refused: B5RdInherit>>y refers to removed instance variable y"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.name);
    const std::string before = b5Definition(c.oldSuper, c.name, c.oldVars, "B5-Test");
    ASSERT_EQ(AO_OK, ao_accept_class(before.c_str(), &err)) << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method(c.name, 0, c.source, &err)) << err.message;
    ASSERT_EQ(AO_OK, printIt(std::string("oldRd := ") + c.name)) << err.message;

    AoSpan e{};
    const std::string after = b5Definition(c.newSuper, c.name, c.newVars, "B5-Test");
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(after.c_str(), &e));
    EXPECT_STREQ(c.expected, e.message);
    ASSERT_EQ(AO_OK, printIt(std::string("oldRd == ") + c.name)) << err.message;
    EXPECT_STREQ("true", out);
    ASSERT_EQ(AO_OK, printIt(std::string(c.name) + " superclass == " + c.oldSuper)) << err.message;
    EXPECT_STREQ("true", out);
    const std::string vars = std::string("instanceVariableNames: '") + c.oldVars + "'\n";
    EXPECT_NE(b5ClassDefinition(c.name).find(vars), std::string::npos);
    char source[256];
    ASSERT_EQ(AO_OK, ao_browser_source(c.name, 0, c.selector, source, 256));
    EXPECT_STREQ(c.source, source);
  }
  ASSERT_EQ(AO_OK, printIt("B5RdPlain new y")) << err.message;
  EXPECT_STREQ("nil", out);
  ASSERT_EQ(AO_OK, printIt("B5RdBlock new yIsNil")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("B5RdInherit new y")) << err.message;
  EXPECT_STREQ("nil", out);
  ao_runtime_shutdown();
}

// B5 review M2 / SPEC §3.9: 送信（self y）、シンボル（#y）、同じ名前の引数と temp、消えない変数
// （新しい superclass から継承するものを含む）は、消えるインスタンス変数の読みに数えない。
TEST(AcceptAbi, ShapeChangeKeepsMethodsThatDoNotReadRemovedVariables) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  for (const std::string& def : {b5Definition("Object", "B5KpSup", "y", "B5-Test"),
                                 b5Definition("Object", "B5KpOther", "y", "B5-Test"),
                                 b5Definition("Object", "B5Keep", "x y", "B5-Test")}) {
    ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
  }
  const char* sources[] = {
      "x\n  ^x\n",
      "x: v\n  x := v\n",
      "y\n  ^x\n",
      "sendsY\n  ^self y\n",
      "sym\n  ^#y == #y\n",
      "shadow\n  | y | y := 3. ^y\n",
      "arg: y\n  ^y\n",
      "blockArg\n  ^[:y | y] value: 4\n",
      "blockTemp\n  ^[| y | y := 5. y] value\n",
  };
  for (const char* s : sources) {
    ASSERT_EQ(AO_OK, ao_accept_method("B5Keep", 0, s, &err)) << err.message;
  }
  const std::string kept = b5Definition("Object", "B5Keep", "x", "B5-Test");
  ASSERT_EQ(AO_OK, ao_accept_class(kept.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("((B5Keep new x: 2; yourself) sendsY)")) << err.message;
  EXPECT_STREQ("2", out);
  ASSERT_EQ(AO_OK, printIt("B5Keep new sym")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("B5Keep new shadow")) << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, printIt("B5Keep new arg: 7")) << err.message;
  EXPECT_STREQ("7", out);
  ASSERT_EQ(AO_OK, printIt("B5Keep new blockArg")) << err.message;
  EXPECT_STREQ("4", out);
  ASSERT_EQ(AO_OK, printIt("B5Keep new blockTemp")) << err.message;
  EXPECT_STREQ("5", out);

  // superclass を変えても、新しい superclass が同じ名前の変数を持てば消えない。
  const std::string moved = b5Definition("B5KpSup", "B5KpMoved", "x", "B5-Test");
  ASSERT_EQ(AO_OK, ao_accept_class(moved.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5KpMoved", 0, "y\n  ^y\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5KpMoved", 0, "y: v\n  y := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("B5KpOther", "B5KpMoved", "x", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("B5KpMoved superclass == B5KpOther")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("(B5KpMoved new y: 9) y")) << err.message;
  EXPECT_STREQ("9", out);
  ao_runtime_shutdown();
}

#include "ao_abi.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

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
  ASSERT_EQ(AO_OK, printIt("Smalltalk includesKey: #B3Zz")) << err.message;
  EXPECT_STREQ("false", out);
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

// The class-side subclass:… override for B5LaySup: it answers a class with the given variables
// whatever the definition asks for.
std::string b5LayoutOverride(const char* instVars) {
  std::string src =
      "subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p "
      "category: k\n"
      "  ^super subclass: n instanceVariableNames: '";
  src += instVars;
  src += "' classVariableNames: c poolDictionaries: p category: k\n";
  return src;
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

// Codex review P2 / SPEC §3.9: superclass のクラス側で subclass:… を上書きして、要求 x y z を
// extra x y z で作らせても、移したメソッドは送信が答えたクラスの並びで動く。x は 2 番目、y は 3 番目の
// スロットを読み書きし、extra のスロットには触れない。
TEST(AcceptAbi, ShapeChangeMovesMethodsInLayoutTheSubclassSendAnswers) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("Object", "B5LaySup", "", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("B5LaySup", "B5LayChild", "x y", "B5-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5LayChild", 0, "x\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5LayChild", 0, "x: v\n  x := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5LayChild", 0, "y\n  ^y\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5LayChild", 0, "y: v\n  y := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B5LaySup", 1, b5LayoutOverride("extra x y z").c_str(), &err))
      << err.message;

  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("B5LaySup", "B5LayChild", "x y z", "B5-Test").c_str(),
                            &err))
      << err.message;
  EXPECT_NE(b5ClassDefinition("B5LayChild").find("instanceVariableNames: 'extra x y z'"),
            std::string::npos);
  struct Case {
    const char* expr;
    const char* expected;
  };
  const Case cases[] = {
      {"(B5LayChild new instVarAt: 2 put: 7; yourself) x", "7"},
      {"(B5LayChild new instVarAt: 3 put: 8; yourself) y", "8"},
      {"(B5LayChild new instVarAt: 1 put: 9; yourself) x", "nil"},
      {"B5LayChild new x: 5; y: 6; instVarAt: 1", "nil"},
      {"B5LayChild new x: 5; y: 6; instVarAt: 2", "5"},
      {"B5LayChild new x: 5; y: 6; instVarAt: 3", "6"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.expr);
    ASSERT_EQ(AO_OK, printIt(c.expr)) << err.message;
    EXPECT_STREQ(c.expected, out);
  }
  ao_runtime_shutdown();
}

// Codex review P2 / SPEC §3.9: 送信が答えたクラスの並び（上書きが y を落として x だけ）で、
// コンパイルし直しか消えるインスタンス変数の検査が失敗すれば、定義テキストの並び（x y z）で通っていても
// AO_ERR_COMPILE で、メッセージは事前の拒否と同じ形。名前は旧クラスに戻り、旧クラスのメソッドとソースも
// そのまま。
TEST(AcceptAbi, ShapeChangePutsNameBackWhenMethodsFailInLayoutTheSubclassSendAnswers) {
  struct Case {
    const char* method;
    const char* message;
  };
  const Case cases[] = {
      {"y: v\n  y := v\n", "shape change refused: B5LayDrop>>y: does not compile: "},
      {"y\n  ^y\n", "shape change refused: B5LayDrop>>y refers to removed instance variable y"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.method);
    ASSERT_EQ(AO_OK, ao_runtime_boot());
    AoSpan err{};
    char out[64];
    auto printIt = [&](const char* src) {
      return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
    };
    ASSERT_EQ(AO_OK,
              ao_accept_class(b5Definition("Object", "B5LaySup", "", "B5-Test").c_str(), &err))
        << err.message;
    ASSERT_EQ(AO_OK,
              ao_accept_class(b5Definition("B5LaySup", "B5LayDrop", "x y", "B5-Test").c_str(), &err))
        << err.message;
    const char* getX = "x\n  ^x\n";
    ASSERT_EQ(AO_OK, ao_accept_method("B5LayDrop", 0, getX, &err)) << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method("B5LayDrop", 0, "x: v\n  x := v\n", &err)) << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method("B5LayDrop", 0, c.method, &err)) << err.message;
    ASSERT_EQ(AO_OK, printIt("oldDrop := B5LayDrop. (B5LayDrop new x: 3) x")) << err.message;
    EXPECT_STREQ("3", out);
    ASSERT_EQ(AO_OK, ao_accept_method("B5LaySup", 1, b5LayoutOverride("x").c_str(), &err))
        << err.message;

    AoSpan e{};
    const std::string reshaped = b5Definition("B5LaySup", "B5LayDrop", "x y z", "B5-Test");
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(reshaped.c_str(), &e));
    EXPECT_EQ(0u, std::string(e.message).find(c.message)) << e.message;
    ASSERT_EQ(AO_OK, printIt("oldDrop == B5LayDrop")) << err.message;
    EXPECT_STREQ("true", out);
    ASSERT_EQ(AO_OK, printIt("(B5LayDrop new x: 4) x")) << err.message;
    EXPECT_STREQ("4", out);
    char source[256];
    ASSERT_EQ(AO_OK, ao_browser_source("B5LayDrop", 0, "x", source, 256));
    EXPECT_STREQ(getX, source);
    EXPECT_NE(b5ClassDefinition("B5LayDrop").find("instanceVariableNames: 'x y'\n"),
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

// B5 review L1 / SPEC §3.9: superclass が、名前が今指しているクラスそのもの（Foo subclass: #Foo）
// か、そのサブクラスなら、何も変えずに AO_ERR_COMPILE。別名で指していても同じ。受け付けると、
// Browser が表示した定義を Accept し直すたびに継承が 1 段深くなった。
TEST(AcceptAbi, AcceptClassRefusesClassAsItsOwnSuperclass) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  for (const std::string& def : {b5Definition("Object", "B5Self", "a", "B5-Test"),
                                 b5Definition("Object", "B5Mid", "a", "B5-Test"),
                                 b5Definition("B5Mid", "B5MidSub", "", "B5-Test")}) {
    ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
  }
  ASSERT_EQ(AO_OK, ao_accept_method("B5Self", 0, "a\n  ^a\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldSelf := B5Self. oldMid := B5Mid. "
                           "Smalltalk at: #B5SelfAlias put: B5Self. B5Self instSize"))
      << err.message;
  EXPECT_STREQ("1", out);

  struct Case {
    const char* superName;
    const char* name;
    const char* expected;
  };
  const Case cases[] = {
      {"B5Self", "B5Self", "superclass refused: B5Self is B5Self or its subclass"},
      {"B5SelfAlias", "B5SelfAlias",
       "superclass refused: B5SelfAlias is B5SelfAlias or its subclass"},
      {"B5MidSub", "B5Mid", "superclass refused: B5MidSub is B5Mid or its subclass"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.superName);
    AoSpan e{};
    const std::string def = b5Definition(c.superName, c.name, "b", "B5-Test");
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(def.c_str(), &e));
    EXPECT_STREQ(c.expected, e.message);
    ASSERT_EQ(AO_OK, printIt("oldSelf == B5Self and: [B5SelfAlias == B5Self]")) << err.message;
    EXPECT_STREQ("true", out);
    ASSERT_EQ(AO_OK, printIt("B5Self superclass == Object and: [B5Self instSize = 1]"))
        << err.message;
    EXPECT_STREQ("true", out);
    ASSERT_EQ(AO_OK, printIt("oldMid == B5Mid and: [B5MidSub superclass == B5Mid]")) << err.message;
    EXPECT_STREQ("true", out);
  }

  // Browser が表示した定義は Object の下のままで、そのまま Accept し直しても形は変わらない。
  const std::string shown = b5ClassDefinition("B5Self");
  EXPECT_EQ(0u, shown.find("Object subclass: #B5Self")) << shown;
  ASSERT_EQ(AO_OK, ao_accept_class(shown.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("oldSelf == B5Self and: [B5Self instSize = 1]")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("(B5Self new instVarAt: 1 put: 4; yourself) a")) << err.message;
  EXPECT_STREQ("4", out);
  ao_runtime_shutdown();
}

// B5 review L2 / SPEC §3.9: 1 回の ao_accept_class に複数のクラス定義があれば、先頭から順に適用し、
// 拒否された定義で止まって AO_ERR_COMPILE。それより前のチャンク（クラス定義と methodsFor:）は
// 適用済みのまま残り、それより後のチャンクは適用しない。
TEST(AcceptAbi, AcceptClassStopsAtRefusedDefinitionKeepingEarlierChunks) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  const std::string chunks = b5Definition("Object", "B5First", "", "B5-Test") + "!\n" +
                             "!B5First methodsFor: 'b5'!\none\n  ^1! !\n" +
                             b5Definition("B5First", "B5First", "a", "B5-Test") + "!\n" +
                             b5Definition("Object", "B5Third", "", "B5-Test") + "!\n" +
                             "!B5First methodsFor: 'b5'!\ntwo\n  ^2! !\n";
  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(chunks.c_str(), &e));
  EXPECT_STREQ("superclass refused: B5First is B5First or its subclass", e.message);

  EXPECT_EQ("B5-Test", b5Category("B5First"));
  ASSERT_EQ(AO_OK, printIt("B5First new one")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, printIt("B5First instSize")) << err.message;
  EXPECT_STREQ("0", out);
  EXPECT_EQ(1, ao_browser_selector_count("B5First", 0, "user"));
  EXPECT_EQ("<missing>", b5Category("B5Third"));
  EXPECT_EQ(AO_ERR_EVAL, printIt("B5First new two"));
  ao_runtime_shutdown();
}

// B4 (docs/claude-review/05 Critical) / SPEC §3.6: 失敗シナリオ。OrderedCollection のサブクラスが
// 足す x は、親の 3 つのスロット（array firstIndex lastIndex）の後ろに置かれる。getX は内部配列では
// なく x を返す。x y z に文字列と整数を入れても、add: は親のスロットを読んで動く。
TEST(AcceptAbi, KernelSubclassVariablesFollowParentSlots) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("OrderedCollection", "R5OC", "x", "B4-Test").c_str(),
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("R5OC", 0, "getX\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("R5OC new getX")) << err.message;
  EXPECT_STREQ("nil", out);
  ASSERT_EQ(AO_OK, printIt("R5OC new instVarNamed: #x")) << err.message;
  EXPECT_STREQ("nil", out);

  ASSERT_EQ(AO_OK, ao_accept_class(
                       b5Definition("OrderedCollection", "R5OCz", "x y z", "B4-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("R5OCz", 0,
                                    "fill\n  x := 'ab'. y := 1000000. z := 1000000\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("R5OCz", 0, "getX\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("r := R5OCz new. r fill. r add: 7. r add: 8. r size")) << err.message;
  EXPECT_STREQ("2", out);
  ASSERT_EQ(AO_OK, printIt("r at: 2")) << err.message;
  EXPECT_STREQ("8", out);
  ASSERT_EQ(AO_OK, printIt("r getX")) << err.message;
  EXPECT_STREQ("'ab'", out);
  ASSERT_EQ(AO_OK, printIt("r instVarAt: 6")) << err.message;
  EXPECT_STREQ("1000000", out);
  ao_runtime_shutdown();
}

// B4 (docs/claude-review/04「既報との関係」) / SPEC §3.6: Kernel クラスのスロットは名前を持つ。
// instVarNamed: は見つけ、Accept したメソッドはグローバルではなくスロットを読む。
TEST(AcceptAbi, KernelClassSlotsHaveNames) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  ASSERT_EQ(AO_OK, printIt("(Association key: 3 value: 4) instVarNamed: #key")) << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, printIt("(Association key: 3 value: 4) instVarNamed: #value")) << err.message;
  EXPECT_STREQ("4", out);
  ASSERT_EQ(AO_OK, ao_accept_method("Association", 0, "probeKey\n  ^key\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("(Association key: 3 value: 4) probeKey")) << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, ao_accept_method("Point", 0, "probeY\n  ^y\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("(Point x: 3 y: 4) probeY")) << err.message;
  EXPECT_STREQ("4", out);
  ao_runtime_shutdown();
}

// B4 / SPEC §3.6 / §3.9: 形を変える再 Accept も、Kernel の親のスロットの後ろに変数を置く。親を
// Object に替える定義は、親のスロットの名前（firstIndex）を読むメソッドがあれば拒む。
TEST(AcceptAbi, ShapeChangeUnderKernelSuperclassKeepsParentSlots) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("OrderedCollection", "B4Re", "x", "B4-Test").c_str(),
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4Re", 0, "getX\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4Re", 0, "setX: v\n  x := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4Re", 0, "readFirst\n  ^firstIndex\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(
                       b5Definition("OrderedCollection", "B4Re", "w x", "B4-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("b := B4Re new setX: 5. b add: 9. b getX")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("b instVarAt: 5")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("b size")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, printIt("b readFirst")) << err.message;
  EXPECT_STREQ("1", out);

  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("Object", "B4Re", "w x", "B4-Test").c_str(), &e));
  EXPECT_STREQ("shape change refused: B4Re>>readFirst refers to removed instance variable "
               "firstIndex",
               e.message);
  ao_runtime_shutdown();
}

// B4 / SPEC §3.6 / §3.11: 名前を持たない Kernel クラス（B4 より前のイメージ）でも、サブクラスの変数は
// 親のスロットと重ならない。名前の無いスロットはソースから読めない（firstIndex はグローバルになる）。
// チャンクの file-in、Accept、形を変える再 Accept の 3 経路とも同じ。
TEST(AcceptAbi, UnnamedKernelSlotsKeepSubclassVariablesApart) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  ASSERT_EQ(AO_OK, printIt("OrderedCollection instVarAt: 8 put: nil. 0")) << err.message;
  const std::string chunks = b5Definition("OrderedCollection", "B4Chunk", "x", "B4-Test") + "!\n" +
                             "!B4Chunk methodsFor: 'b4'!\nsetX: v\n  x := v!\ngetX\n  ^x! !\n";
  ASSERT_EQ(AO_OK, ao_accept_class(chunks.c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("c := B4Chunk new setX: 4. c add: 9. c getX")) << err.message;
  EXPECT_STREQ("4", out);
  ASSERT_EQ(AO_OK, printIt("c instVarAt: 4")) << err.message;
  EXPECT_STREQ("4", out);
  ASSERT_EQ(AO_OK, printIt("c size")) << err.message;
  EXPECT_STREQ("1", out);

  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("OrderedCollection", "B4Old", "x", "B4-Test").c_str(),
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4Old", 0, "getX\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4Old", 0, "setX: v\n  x := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4Old", 0, "readFirst\n  ^firstIndex\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("o := B4Old new setX: 5. o add: 9. o getX")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("o instVarAt: 4")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("o instVarNamed: #x")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("o size")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, printIt("o readFirst")) << err.message;
  EXPECT_STREQ("nil", out);

  ASSERT_EQ(AO_OK, ao_accept_class(
                       b5Definition("OrderedCollection", "B4Old", "w x", "B4-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("p := B4Old new setX: 6. p add: 9. p getX")) << err.message;
  EXPECT_STREQ("6", out);
  ASSERT_EQ(AO_OK, printIt("p instVarAt: 5")) << err.message;
  EXPECT_STREQ("6", out);
  ASSERT_EQ(AO_OK, printIt("p size")) << err.message;
  EXPECT_STREQ("1", out);
  ao_runtime_shutdown();
}

// B4 / SPEC §3.6: 保存して読み直したイメージでも、Kernel クラスの名前は残る。読み直す前に
// コンパイルしたメソッドも、あとでコンパイルするメソッドも、親のスロットの後ろの x を読む。
TEST(AcceptAbi, KernelSlotNamesSurviveImageSaveAndLoad) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("OrderedCollection", "R5OC", "x", "B4-Test").c_str(),
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("R5OC", 0, "getX\n  ^x\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Association", 0, "probeKey\n  ^key\n", &err)) << err.message;
  const char* path = "b4-kernel-slots.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  std::remove(path);

  ASSERT_EQ(AO_OK, ao_accept_method("R5OC", 0, "setX: v\n  x := v\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("r := R5OC new setX: 5. r add: 9. r getX")) << err.message;
  EXPECT_STREQ("5", out);
  ASSERT_EQ(AO_OK, printIt("r size")) << err.message;
  EXPECT_STREQ("1", out);
  ASSERT_EQ(AO_OK, printIt("(Association key: 3 value: 4) instVarNamed: #key")) << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, printIt("(Association key: 3 value: 4) probeKey")) << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, ao_accept_method("Association", 0, "probeValue\n  ^value\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("(Association key: 3 value: 4) probeValue")) << err.message;
  EXPECT_STREQ("4", out);
  ao_runtime_shutdown();
}

// B4 (docs/claude-review/04 Low) / SPEC §3.6: バイト列のクラスに名前付き変数を足す定義はエラーで、
// クラスを作らない。定義メッセージを評価しても同じ理由で中断する。変数の無いサブクラスは作れる。
TEST(AcceptAbi, BytesSuperclassRefusesInstanceVariables) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  char defn[512];
  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("String", "PJ", "tag", "B4-Test").c_str(), &e));
  EXPECT_STREQ("subclass failed: PJ: bytes class cannot have instance variables", e.message);
  EXPECT_EQ(AO_ERR, ao_browser_class_definition("PJ", defn, 512));

  EXPECT_EQ(AO_ERR_EVAL, printIt("ByteArray subclass: #PJ2 instanceVariableNames: 'a b' "
                                 "classVariableNames: '' poolDictionaries: '' category: 'B4-Test'"));
  EXPECT_STREQ("bytes class cannot have instance variables", err.message);
  EXPECT_EQ(AO_ERR, ao_browser_class_definition("PJ2", defn, 512));

  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("String", "PJ3", "", "B4-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, printIt("PJ3 isBytes")) << err.message;
  EXPECT_STREQ("true", out);
  ASSERT_EQ(AO_OK, printIt("(PJ3 new: 2) size")) << err.message;
  EXPECT_STREQ("2", out);
  ao_runtime_shutdown();
}

// B4 (docs/claude-review/01 High) / SPEC §3.6: 失敗シナリオ。Kernel クラスの名前は intern した Symbol
// で、メッセージが届く。メタクラスの name は thisClass の名前に ' class' を続けた String で、Kernel
// クラスもユーザークラスも同じ規則。保存して読み直したイメージでも同じ。
TEST(AcceptAbi, ClassNamesAreSymbolsAndMetaclassNamesFollowThem) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("Object", "B4Named", "", "B4-Test").c_str(), &err))
      << err.message;
  const struct {
    const char* source;
    const char* printed;
  } checks[] = {
      {"Object name == #Object", "true"},
      {"Object name size", "6"},
      {"3 class name == #SmallInteger", "true"},
      {"Metaclass name == #Metaclass", "true"},
      {"B4Named name == #B4Named", "true"},
      {"Object class name", "'Object class'"},
      {"Object class name class == String", "true"},
      {"B4Named class name", "'B4Named class'"},
      {"B4Named class name class == String", "true"},
      {"Metaclass class name", "'Metaclass class'"},
      {"Object class class name == #Metaclass", "true"},
  };
  auto expectChecks = [&](const char* when) {
    for (const auto& c : checks) {
      ASSERT_EQ(AO_OK, printIt(c.source)) << when << ": " << c.source << ": " << err.message;
      EXPECT_STREQ(c.printed, out) << when << ": " << c.source;
    }
  };
  expectChecks("booted");
  const char* path = "b4-class-names.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  std::remove(path);
  expectChecks("loaded");
  ao_runtime_shutdown();
}

// 01 Medium / 00 High / SPEC §3.6: Smalltalk は SmalltalkImage で、名前（Symbol）から値への辞書を
// 持つ。Workspace も Accept したメソッドも、グローバルの名前を実行時にこの辞書で引くので、Accept が
// 先でも後でも Smalltalk at:put: の値を読む。subclass: で作ったクラスと at:put: で足した名前は辞書に
// 入り、保存して読み直しても残る。
TEST(AcceptAbi, SmalltalkIsTheGlobalDictionary) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto run = [&](const char* src, int mode) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), mode, out, 128, &err);
  };
  auto printIt = [&](const char* src) { return run(src, AO_EVAL_PRINTIT); };
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "probeB4ZapEarly\n  ^Zap\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, printIt("3 probeB4ZapEarly")) << err.message;
  EXPECT_STREQ("nil", out);
  ASSERT_EQ(AO_OK, printIt("Smalltalk at: #Zap put: 3")) << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "probeB4ZapLate\n  ^Zap\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "probeStIsNil\n  ^Smalltalk isNil\n", &err))
      << err.message;
  ASSERT_EQ(AO_OK, run(b5Definition("Object", "B4Reg", "", "B4-Test").c_str(), AO_EVAL_DOIT))
      << err.message;
  const struct {
    const char* source;
    const char* printed;
  } checks[] = {
      {"Smalltalk isNil", "false"},
      {"Smalltalk class == SmalltalkImage", "true"},
      {"3 probeStIsNil", "false"},
      {"(Smalltalk at: #Object) == Object", "true"},
      {"(Smalltalk at: 'Object') == Object", "true"},
      {"(Smalltalk at: #Smalltalk) == Smalltalk", "true"},
      {"(Smalltalk at: #Processor) == Processor", "true"},
      {"Smalltalk includesKey: #Nope", "false"},
      {"Smalltalk includesKey: #Object", "true"},
      {"Smalltalk includesKey: 3", "false"},
      {"Smalltalk at: #Nope ifAbsent: [7]", "7"},
      {"(Smalltalk at: #Object ifAbsent: [7]) == Object", "true"},
      {"Zap", "3"},
      {"Smalltalk at: #Zap", "3"},
      {"3 probeB4ZapEarly", "3"},
      {"3 probeB4ZapLate", "3"},
      {"Smalltalk includesKey: #B4Reg", "true"},
      {"(Smalltalk at: #B4Reg) == B4Reg", "true"},
      {"B4Reg new class == B4Reg", "true"},
  };
  const struct {
    const char* source;
    const char* reason;
  } failures[] = {
      {"Smalltalk at: #Nope", "key not found: #Nope"},
      {"Smalltalk at: 3", "key not found"},
      {"Smalltalk at: #Object put: 3", "cannot rebind Kernel global: Object"},
      {"Smalltalk at: #Smalltalk put: 3", "cannot rebind Kernel global: Smalltalk"},
      {"Smalltalk at: #Processor put: 3", "cannot rebind Kernel global: Processor"},
      {"Smalltalk at: 3 put: 4", "key must be a Symbol or String"},
  };
  auto expectChecks = [&](const char* when) {
    for (const auto& c : checks) {
      ASSERT_EQ(AO_OK, printIt(c.source)) << when << ": " << c.source << ": " << err.message;
      EXPECT_STREQ(c.printed, out) << when << ": " << c.source;
    }
    for (const auto& f : failures) {
      EXPECT_EQ(AO_ERR_EVAL, printIt(f.source)) << when << ": " << f.source;
      EXPECT_STREQ(f.reason, err.message) << when << ": " << f.source;
    }
    // 拒んだ at:put: は何も変えない。
    ASSERT_EQ(AO_OK, printIt("(Smalltalk at: #Object) == Object")) << err.message;
    EXPECT_STREQ("true", out) << when;
    ASSERT_EQ(AO_OK, printIt("Smalltalk class == SmalltalkImage")) << err.message;
    EXPECT_STREQ("true", out) << when;
  };
  expectChecks("booted");
  const char* path = "b4-smalltalk-dictionary.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  std::remove(path);
  expectChecks("loaded");
  // 読み直したあとも、メソッドは実行時の値を読む。
  ASSERT_EQ(AO_OK, printIt("Smalltalk at: #Zap put: 4")) << err.message;
  ASSERT_EQ(AO_OK, printIt("3 probeB4ZapEarly")) << err.message;
  EXPECT_STREQ("4", out);
  ASSERT_EQ(AO_OK, printIt("Zap")) << err.message;
  EXPECT_STREQ("4", out);
  ao_runtime_shutdown();
}

namespace {

// A class definition message with class variables, for ao_accept_class.
std::string b4Definition(const char* superName, const char* name, const char* instVars,
                         const char* classVars, const char* category = "B4-Test") {
  std::string def = superName;
  def += " subclass: #";
  def += name;
  def += "\n  instanceVariableNames: '";
  def += instVars;
  def += "'\n  classVariableNames: '";
  def += classVars;
  def += "'\n  poolDictionaries: ''\n  category: '";
  def += category;
  def += "'\n";
  return def;
}

struct Check {
  const char* source;
  const char* printed;
};

// Prints each source in turn and expects its printString. The checks run in order, so a later
// one sees what an earlier one did.
void expectPrints(const std::vector<Check>& checks, const char* when = "") {
  for (const Check& c : checks) {
    char out[128];
    AoSpan err{};
    ASSERT_EQ(AO_OK, ao_eval(c.source, static_cast<int>(std::strlen(c.source)), AO_EVAL_PRINTIT,
                             out, 128, &err))
        << when << ": " << c.source << ": " << err.message;
    EXPECT_STREQ(c.printed, out) << when << ": " << c.source;
  }
}

// Accepts each method source on the given side of className.
void acceptMethods(const char* className, int meta, const std::vector<const char*>& sources) {
  for (const char* src : sources) {
    AoSpan err{};
    ASSERT_EQ(AO_OK, ao_accept_method(className, meta, src, &err))
        << className << (meta ? " class" : "") << ": " << src << ": " << err.message;
  }
}

}  // namespace

// B4 (docs/claude-review/05 High, 04「既報との関係」) / SPEC §3.6, §3.8: 失敗シナリオ。
// classVariableNames: 'Count' のクラスで bump Count := 1 を Accept でき（cannot assign にならない）、
// count ^Count がそのあと 1 を返す。ブロックの中、展開したブロックの中の読み書きも同じ変数である。
// classPool は名前から束縛（Association）への Dictionary で、束縛の value がクラス変数の値。
// 束縛はメソッドと共有する。名前は Smalltalk に登録しない。
TEST(AcceptAbi, ClassVariablesAreReadAndWrittenByMethods) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CV", "", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4CV", 0,
                {"bump\n  Count := 1\n", "count\n  ^Count\n", "incr\n  ^Count := Count + 1\n",
                 "blockSet: v\n  [:w | Count := w] value: v\n",
                 "inlined\n  Count isNil ifFalse: [Count := Count * 10].\n  ^Count\n"});
  expectPrints({
      {"B4CV new count", "nil"},
      {"B4CV new bump; count", "1"},
      {"B4CV new count", "1"},
      {"B4CV new incr", "2"},
      {"B4CV new blockSet: 7; count", "7"},
      {"B4CV new inlined", "70"},
      {"B4CV classPool class == Dictionary", "true"},
      {"B4CV classPool size", "1"},
      {"(B4CV classPool at: #Count) class == Association", "true"},
      {"(B4CV classPool at: #Count) key == #Count", "true"},
      {"(B4CV classPool at: #Count) value", "70"},
      {"(B4CV classPool at: #Count) value: 3. B4CV new count", "3"},
      {"Smalltalk includesKey: #Count", "false"},
      {"(B4CV class instVarNamed: #classPool) isNil", "true"},
      {"Object classPool isNil", "true"},
  });
  ao_runtime_shutdown();
}

// B4 / SPEC §3.6: サブクラスのメソッドは親のクラス変数を読み書きし、親と同じ値を共有する。
// クラス側のメソッドで書いた値をインスタンス側で読め、その逆もできる。サブクラスの classPool には
// 自分のクラス変数だけがある。
TEST(AcceptAbi, SubclassesAndClassSideShareClassVariables) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CvSup", "", "Shared").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4CvSup", "B4CvSub", "", "Own").c_str(), &err))
      << err.message;
  acceptMethods("B4CvSup", 0, {"shared\n  ^Shared\n", "shared: v\n  Shared := v\n"});
  acceptMethods("B4CvSup", 1, {"setShared: v\n  Shared := v\n"});
  acceptMethods("B4CvSub", 0,
                {"subShared\n  ^Shared\n", "subShared: v\n  Shared := v\n", "own\n  ^Own\n"});
  acceptMethods("B4CvSub", 1, {"classShared\n  ^Shared\n", "classOwn: v\n  Own := v\n"});
  expectPrints({
      {"B4CvSub new subShared: 3. B4CvSup new shared", "3"},
      {"B4CvSup new shared: 4. B4CvSub new subShared", "4"},
      {"B4CvSub classShared", "4"},
      {"B4CvSup setShared: 5. B4CvSub new subShared", "5"},
      {"B4CvSub classOwn: 6. B4CvSub new own", "6"},
      {"(B4CvSup classPool at: #Shared) value", "5"},
      {"B4CvSub classPool includesKey: #Shared", "false"},
      {"(B4CvSub classPool at: #Own) value", "6"},
  });
  ao_runtime_shutdown();
}

// B4 / SPEC §3.8: 解決順はローカル → インスタンス変数 → 擬変数 → クラス変数 → グローバル。同じ名前の
// グローバル（Smalltalk at: #Count put: 99）があっても、メソッドの Count はクラス変数を読む。temp と
// 引数はクラス変数を隠す。クラス側では Behavior の枠の名前（name）がクラス変数を隠す。
TEST(AcceptAbi, ClassVariablesResolveBetweenLocalsAndGlobals) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK,
            ao_accept_class(b4Definition("Object", "B4CvOrd", "", "Count name").c_str(), &err))
      << err.message;
  acceptMethods("B4CvOrd", 0,
                {"count\n  ^Count\n", "count: v\n  Count := v\n",
                 "temp\n  | Count |\n  Count := 5.\n  ^Count\n", "arg: Count\n  ^Count\n",
                 "blockArg\n  ^[:Count | Count] value: 8\n", "name\n  ^name\n",
                 "name: v\n  name := v\n"});
  acceptMethods("B4CvOrd", 1, {"probeName\n  ^name\n"});
  expectPrints({{"Smalltalk at: #Count put: 99", "99"}});
  acceptMethods("B4CvOrd", 0, {"countLate\n  ^Count\n"});
  expectPrints({
      {"B4CvOrd new count: 1; count", "1"},
      {"B4CvOrd new countLate", "1"},
      {"Count", "99"},
      {"B4CvOrd new temp", "5"},
      {"B4CvOrd new arg: 7", "7"},
      {"B4CvOrd new blockArg", "8"},
      {"B4CvOrd new count", "1"},
      {"B4CvOrd new name: 4; name", "4"},
      {"B4CvOrd probeName == #B4CvOrd", "true"},
      {"(B4CvOrd classPool at: #name) value", "4"},
  });
  ao_runtime_shutdown();
}

// B4 / SPEC §3.9: 形が同じ再 Accept でクラス変数を足しても、クラスは同じで、残る名前の束縛と値は
// そのまま。既存メソッドは同じ値を読む。定義テキストは自分のクラス変数を並べ、それを Accept し直しても
// 値は変わらない。
TEST(AcceptAbi, ReacceptSameShapeKeepsClassVariableBindings) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CvRe", "a", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4CvRe", 0, {"count\n  ^Count\n", "count: v\n  Count := v\n"});
  acceptMethods("B4CvRe", 1, {"classCount\n  ^Count\n"});
  expectPrints({{"oldRe := B4CvRe. oldBinding := B4CvRe classPool at: #Count. B4CvRe new count: 7; "
                 "count",
                 "7"}});

  ASSERT_EQ(AO_OK, ao_accept_class(
                       b4Definition("Object", "B4CvRe", "a", " Count  Total ", "B4-New").c_str(), &err))
      << err.message;
  EXPECT_EQ("B4-New", b5Category("B4CvRe"));
  acceptMethods("B4CvRe", 0, {"total\n  ^Total\n", "total: v\n  Total := v\n"});
  expectPrints({
      {"oldRe == B4CvRe", "true"},
      {"B4CvRe new count", "7"},
      {"B4CvRe classCount", "7"},
      {"(B4CvRe classPool at: #Count) == oldBinding", "true"},
      {"B4CvRe new total", "nil"},
      {"B4CvRe new total: 3; total", "3"},
      {"B4CvRe new count: 8. B4CvRe classCount", "8"},
      {"B4CvRe classPool size", "2"},
  });
  const std::string shown = b5ClassDefinition("B4CvRe");
  EXPECT_NE(shown.find("classVariableNames: 'Count Total'"), std::string::npos) << shown;
  ASSERT_EQ(AO_OK, ao_accept_class(shown.c_str(), &err)) << err.message;
  expectPrints({
      {"oldRe == B4CvRe", "true"},
      {"B4CvRe new count", "8"},
      {"B4CvRe new total", "3"},
      {"(B4CvRe classPool at: #Count) == oldBinding", "true"},
  });
  ao_runtime_shutdown();
}

// B4 / SPEC §3.9: 形が同じ再 Accept で、消えるクラス変数の束縛をメソッドが持っていれば（読みも代入も、
// ブロックの中も、クラス側も、ソースの無いメソッドも、サブクラスのメソッドも）、何も変えずに
// AO_ERR_COMPILE。どのメソッドも使っていなければ、その名前を classPool から除く。
TEST(AcceptAbi, ReacceptRefusesDroppingClassVariableAMethodHolds) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  struct Case {
    const char* name;
    const char* superName;
    const char* methodClass;
    int meta;
    const char* source;
    const char* expected;
  };
  ASSERT_EQ(AO_OK,
            ao_accept_class(b4Definition("Object", "B4CvRmSup", "", "Keep Gone").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4CvRmSup", "B4CvRmSub", "", "").c_str(), &err))
      << err.message;
  const Case cases[] = {
      {"B4CvRmRead", "Object", "B4CvRmRead", 0, "gone\n  ^Gone\n",
       "class variable change refused: B4CvRmRead>>gone refers to removed class variable Gone"},
      {"B4CvRmBlock", "Object", "B4CvRmBlock", 0, "gone: v\n  [Gone := v] value\n",
       "class variable change refused: B4CvRmBlock>>gone: refers to removed class variable Gone"},
      {"B4CvRmSide", "Object", "B4CvRmSide", 1, "gone\n  ^Gone\n",
       "class variable change refused: B4CvRmSide class>>gone refers to removed class variable "
       "Gone"},
      {"B4CvRmSup", "Object", "B4CvRmSub", 0, "gone\n  ^Gone\n",
       "class variable change refused: B4CvRmSub>>gone refers to removed class variable Gone"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.name);
    if (std::string(c.name) != "B4CvRmSup") {
      ASSERT_EQ(AO_OK,
                ao_accept_class(b4Definition(c.superName, c.name, "", "Keep Gone").c_str(), &err))
          << err.message;
    }
    ASSERT_EQ(AO_OK, ao_accept_method(c.methodClass, c.meta, c.source, &err)) << err.message;
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE,
              ao_accept_class(b4Definition(c.superName, c.name, "", "Keep", "B4-Other").c_str(), &e));
    EXPECT_STREQ(c.expected, e.message);
    EXPECT_EQ("B4-Test", b5Category(c.name));
    const std::string probe = std::string(c.name) + " classPool includesKey: #Gone";
    expectPrints({{probe.c_str(), "true"}});
  }
  expectPrints({{"B4CvRmRead new gone", "nil"}, {"B4CvRmSide gone", "nil"}});

  // A method file-in put in has no source; it holds the binding all the same.
  const std::string chunks = b4Definition("Object", "B4CvRmChunk", "", "Keep Gone") +
                             "!\n!B4CvRmChunk methodsFor: 'b4'!\ngone\n  ^Gone! !\n";
  ASSERT_EQ(AO_OK, ao_accept_class(chunks.c_str(), &err)) << err.message;
  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("Object", "B4CvRmChunk", "", "Keep").c_str(), &e));
  EXPECT_STREQ("class variable change refused: B4CvRmChunk>>gone refers to removed class variable Gone",
               e.message);

  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CvDrop", "", "A B").c_str(), &err))
      << err.message;
  acceptMethods("B4CvDrop", 0, {"a\n  ^A\n", "a: v\n  A := v\n", "sendsB\n  ^self b\n"});
  expectPrints({{"B4CvDrop new a: 2; a", "2"}});
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CvDrop", "", "A", "B4-New").c_str(),
                                   &err))
      << err.message;
  EXPECT_EQ("B4-New", b5Category("B4CvDrop"));
  expectPrints({
      {"B4CvDrop classPool includesKey: #B", "false"},
      {"B4CvDrop classPool size", "1"},
      {"B4CvDrop new a", "2"},
  });
  ao_runtime_shutdown();
}

// B4 / SPEC §3.9: 形が変わる再 Accept（インスタンス変数を足す）のあとも、クラス変数の値は残る。
// 新しいクラスは旧クラスの束縛をそのまま使うので、旧クラスのインスタンスとも値を共有する。
TEST(AcceptAbi, ShapeChangeKeepsClassVariableBindings) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK,
            ao_accept_class(b4Definition("Object", "B4CvShape", "x", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4CvShape", 0,
                {"count\n  ^Count\n", "count: v\n  Count := v\n", "x\n  ^x\n", "x: v\n  x := v\n"});
  acceptMethods("B4CvShape", 1, {"classCount\n  ^Count\n"});
  expectPrints({{"oldShape := B4CvShape. oldInst := B4CvShape new. oldBinding := B4CvShape "
                 "classPool at: #Count. oldInst count: 5; count",
                 "5"}});

  ASSERT_EQ(AO_OK,
            ao_accept_class(b4Definition("Object", "B4CvShape", "w x", "Count Extra").c_str(), &err))
      << err.message;
  expectPrints({
      {"oldShape == B4CvShape", "false"},
      {"B4CvShape new count", "5"},
      {"B4CvShape classCount", "5"},
      {"(B4CvShape classPool at: #Count) == oldBinding", "true"},
      {"(B4CvShape classPool at: #Extra) value", "nil"},
      {"B4CvShape new count: 6. oldInst count", "6"},
      {"oldInst count: 7. B4CvShape classCount", "7"},
      {"(B4CvShape new x: 3; yourself) x", "3"},
  });
  ao_runtime_shutdown();
}

// B4 / SPEC §3.9: 形が変わる再 Accept で、旧クラスから見えて新しい定義から見えないクラス変数を
// メソッドが読めば拒む。代入はコンパイルし直しの失敗。クラス側、ブロックの中、スーパークラスの
// クラス変数も数える。名前は旧クラスを指したまま。
TEST(AcceptAbi, ShapeChangeRefusesMethodReadingRemovedClassVariable) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  for (const std::string& sup : {b4Definition("Object", "B4CvShOldSup", "", "Inh"),
                                 b4Definition("Object", "B4CvShNewSup", "", "")}) {
    ASSERT_EQ(AO_OK, ao_accept_class(sup.c_str(), &err)) << err.message;
  }
  struct Case {
    const char* name;
    const char* oldSuper;
    const char* newSuper;
    int meta;
    const char* source;
    const char* expected;
  };
  const Case cases[] = {
      {"B4CvShRead", "Object", "Object", 0, "count\n  ^Count\n",
       "shape change refused: B4CvShRead>>count refers to removed class variable Count"},
      {"B4CvShWrite", "Object", "Object", 0, "count: v\n  Count := v\n",
       "shape change refused: B4CvShWrite>>count: does not compile: cannot assign"},
      {"B4CvShSide", "Object", "Object", 1, "count\n  ^Count\n",
       "shape change refused: B4CvShSide class>>count refers to removed class variable Count"},
      {"B4CvShBlock", "Object", "Object", 0, "count\n  ^[[Count]] value value\n",
       "shape change refused: B4CvShBlock>>count refers to removed class variable Count"},
      {"B4CvShInh", "B4CvShOldSup", "B4CvShNewSup", 0, "inh\n  ^Inh\n",
       "shape change refused: B4CvShInh>>inh refers to removed class variable Inh"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.name);
    ASSERT_EQ(AO_OK, ao_accept_class(b4Definition(c.oldSuper, c.name, "x", "Count").c_str(), &err))
        << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method(c.name, c.meta, c.source, &err)) << err.message;
    const std::string keep = std::string("oldSh := ") + c.name + ". oldSh == " + c.name;
    expectPrints({{keep.c_str(), "true"}});
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE,
              ao_accept_class(b4Definition(c.newSuper, c.name, "x y", "").c_str(), &e));
    EXPECT_STREQ(c.expected, e.message);
    const std::string same = std::string("oldSh == ") + c.name;
    expectPrints({{same.c_str(), "true"}});
  }
  expectPrints({{"B4CvShRead new count", "nil"}, {"B4CvShSide count", "nil"}});
  ao_runtime_shutdown();
}

// B4 / SPEC §3.11: 保存して読み直したイメージでも、クラス変数の値と、メソッドと classPool の共有は
// 保たれる。読み直したあとに Accept したメソッドも同じ束縛を使う。
TEST(AcceptAbi, ClassVariablesSurviveImageSaveAndLoad) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CvImg", "", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4CvImg", 0, {"count\n  ^Count\n", "count: v\n  Count := v\n"});
  acceptMethods("B4CvImg", 1, {"classCount\n  ^Count\n"});
  expectPrints({{"B4CvImg new count: 3; count", "3"}});
  const char* path = "b4-class-variables.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  std::remove(path);
  acceptMethods("B4CvImg", 0, {"twice\n  ^Count * 2\n"});
  expectPrints(
      {
          {"B4CvImg new count", "3"},
          {"B4CvImg classCount", "3"},
          {"B4CvImg new count: 4. (B4CvImg classPool at: #Count) value", "4"},
          {"B4CvImg classCount", "4"},
          {"B4CvImg new twice", "8"},
          {"(B4CvImg classPool at: #Count) value: 5. B4CvImg new twice", "10"},
      },
      "loaded");
  ao_runtime_shutdown();
}

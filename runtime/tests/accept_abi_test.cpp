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
  ASSERT_EQ(AO_OK, ao_image_load(path, nullptr));
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

// B4 / SPEC §3.6: 名前を持たない Kernel クラス（instVarAt: 8 put: nil で instVarNames を消した
// もの）でも、サブクラスの変数は親のスロットと重ならない。名前の無いスロットはソースから読めない（firstIndex はグローバルになる）。
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
  ASSERT_EQ(AO_OK, ao_image_load(path, nullptr));
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
  ASSERT_EQ(AO_OK, ao_image_load(path, nullptr));
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
  ASSERT_EQ(AO_OK, ao_image_load(path, nullptr));
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

// B4 review (Codex P2) / SPEC §3.9: 失敗シナリオ。superclass のクラス側の上書きが、名前を既存の別の
// クラス（B4AtTarget。クラス変数 Count は 99）に付け替えてそれを答えると、その並びでは read が消える
// インスタンス変数 gone を読むので拒否される。拒否されたら、答えたクラスの classPool も束縛も
// メソッド辞書も変わらない（旧クラスの Count の束縛は移らず、99 のまま）。名前は旧クラスに戻る。
TEST(AcceptAbi, RefusedShapeChangeLeavesTheAnsweredClassUntouched) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4AtSup", "", "").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4AtTarget", "", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4AtTarget", 1, {"count\n  ^Count\n", "count: v\n  Count := v\n"});
  ASSERT_EQ(AO_OK,
            ao_accept_class(b4Definition("B4AtSup", "B4AtVictim", "gone", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4AtVictim", 0, {"read\n  ^gone\n"});
  acceptMethods("B4AtVictim", 1, {"count\n  ^Count\n", "count: v\n  Count := v\n"});
  acceptMethods("B4AtSup", 1,
                {"subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p "
                 "category: k\n  Smalltalk at: n put: B4AtTarget.\n  ^B4AtTarget\n"});
  expectPrints({{"B4AtTarget count: 99. B4AtVictim count: 7. oldVictim := B4AtVictim. "
                 "oldTargetPool := B4AtTarget instVarNamed: #classPool. oldTargetBinding := oldTargetPool at: "
                 "#Count. oldVictimBinding := B4AtVictim classPool at: #Count. B4AtTarget count",
                 "99"}});

  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("B4AtSup", "B4AtVictim", "gone y", "Count").c_str(), &e));
  EXPECT_STREQ("shape change refused: B4AtVictim>>read refers to removed instance variable gone",
               e.message);
  expectPrints({
      {"oldVictim == B4AtVictim", "true"},
      {"B4AtTarget count", "99"},
      {"(B4AtTarget instVarNamed: #classPool) == oldTargetPool", "true"},
      {"(B4AtTarget classPool at: #Count) == oldTargetBinding", "true"},
      {"(B4AtTarget classPool at: #Count) value", "99"},
      {"oldTargetBinding value", "99"},
      {"B4AtTarget selectors size", "0"},
      {"B4AtTarget class selectors size", "2"},
      {"B4AtVictim count", "7"},
      {"(B4AtVictim classPool at: #Count) == oldVictimBinding", "true"},
      {"B4AtTarget count: 100. B4AtVictim count", "7"},
  });
  ao_runtime_shutdown();
}

// B4 review (Codex P2) / SPEC §3.9: 失敗シナリオ。インスタンス変数 x を同じ名前のクラス変数に
// 付け替える（ivar y、classvar x）と、x を読み書きしていたメソッドは、黙ってクラス変数を読み書き
// するようになっていた。逆向き（classvar x を ivar x に）も同じ。変更前にコンパイルされたメソッドが
// 何を読み書きしていたかで数えるので、読みも代入も拒否する。名前は旧クラスを指したまま。
TEST(AcceptAbi, ShapeChangeRefusesMovingAVariableToTheOtherKind) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  struct Case {
    const char* name;
    const char* oldIvars;
    const char* oldCvars;
    const char* newIvars;
    const char* newCvars;
    const char* source;
    const char* expected;
  };
  const Case cases[] = {
      {"B4KindIvRead", "x", "", "y", "x", "read\n  ^x\n",
       "shape change refused: B4KindIvRead>>read refers to removed instance variable x"},
      {"B4KindIvWrite", "x", "", "y", "x", "set: v\n  x := v\n",
       "shape change refused: B4KindIvWrite>>set: refers to removed instance variable x"},
      {"B4KindCvRead", "", "x", "x", "", "read\n  ^x\n",
       "shape change refused: B4KindCvRead>>read refers to removed class variable x"},
      {"B4KindCvWrite", "", "x", "x", "", "set: v\n  x := v\n",
       "shape change refused: B4KindCvWrite>>set: refers to removed class variable x"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.name);
    ASSERT_EQ(AO_OK,
              ao_accept_class(b4Definition("Object", c.name, c.oldIvars, c.oldCvars).c_str(), &err))
        << err.message;
    ASSERT_EQ(AO_OK, ao_accept_method(c.name, 0, c.source, &err)) << err.message;
    const std::string keep = std::string("oldKind := ") + c.name + ". oldKind == " + c.name;
    expectPrints({{keep.c_str(), "true"}});
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE,
              ao_accept_class(b4Definition("Object", c.name, c.newIvars, c.newCvars).c_str(), &e));
    EXPECT_STREQ(c.expected, e.message);
    const std::string same = std::string("oldKind == ") + c.name;
    expectPrints({{same.c_str(), "true"}});
  }
  // 旧クラスのメソッドは、今までどおり元の変数を読み書きする。
  acceptMethods("B4KindIvRead", 0, {"set: v\n  x := v\n"});
  acceptMethods("B4KindCvRead", 0, {"set: v\n  x := v\n"});
  expectPrints({
      {"(B4KindIvRead new set: 7; yourself) read", "7"},
      {"B4KindIvRead new read", "nil"},
      {"B4KindCvRead new set: 8. B4KindCvRead new read", "8"},
  });
  ao_runtime_shutdown();
}

// B4 review (Codex P2) / SPEC §3.9: 失敗シナリオ。Smalltalk に束縛されていなくても、生きている
// サブクラス（インスタンスが変数に残るクラス、形の変更で残った旧クラス）は「子孫すべて」に数える。
// どこからも届かないクラス（メソッドのソースとメソッドのキャッシュからしか届かないもの）は数えない。
TEST(AcceptAbi, ReacceptCountsLiveSubclassesThatNoNameBinds) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  // 名前を外したサブクラスのインスタンスが残っている。
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvPar", "", "Gone").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4LvPar", "B4LvChild", "", "").c_str(), &err))
      << err.message;
  acceptMethods("B4LvChild", 0, {"gone\n  ^Gone\n"});
  expectPrints({{"(B4LvPar classPool at: #Gone) value: 42. survivor := B4LvChild new. Smalltalk "
                 "at: #B4LvChild put: nil. survivor gone",
                 "42"}});
  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("Object", "B4LvPar", "", "", "B4-Other").c_str(), &e));
  EXPECT_STREQ("class variable change refused: B4LvChild>>gone refers to removed class variable Gone",
               e.message);
  EXPECT_EQ("B4-Test", b5Category("B4LvPar"));
  expectPrints({{"survivor gone", "42"}, {"B4LvPar classPool includesKey: #Gone", "true"}});
  e = AoSpan{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("Object", "B4LvPar", "a", "Gone").c_str(), &e));
  EXPECT_STREQ("shape change refused: B4LvPar has subclasses", e.message);
  expectPrints({{"survivor class superclass == B4LvPar", "true"}});

  // 形の変更で残った旧クラスのインスタンスが残っている。新しいクラスはもう Gone を使わない。
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvOld", "", "Gone").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4LvOld", "B4LvOldKid", "a", "").c_str(), &err))
      << err.message;
  acceptMethods("B4LvOldKid", 0, {"gone\n  ^Gone\n"});
  expectPrints({{"(B4LvOld classPool at: #Gone) value: 43. oldKid := B4LvOldKid new. oldKid gone",
                 "43"}});
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4LvOld", "B4LvOldKid", "a b", "").c_str(), &err))
      << err.message;
  acceptMethods("B4LvOldKid", 0, {"gone\n  ^nil\n"});
  expectPrints({{"oldKid class == B4LvOldKid", "false"}, {"B4LvOldKid new gone", "nil"}});
  e = AoSpan{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("Object", "B4LvOld", "", "").c_str(), &e));
  EXPECT_STREQ(
      "class variable change refused: B4LvOldKid>>gone refers to removed class variable Gone",
      e.message);
  expectPrints({{"oldKid gone", "43"}});

  // 形の変更で残った、そのクラス自身の旧クラス。束縛を今のクラスと共有する。
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvSelf", "", "Gone").c_str(), &err))
      << err.message;
  acceptMethods("B4LvSelf", 0, {"gone\n  ^Gone\n"});
  expectPrints({{"(B4LvSelf classPool at: #Gone) value: 44. oldSelf := B4LvSelf new. oldSelf gone",
                 "44"}});
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvSelf", "a", "Gone").c_str(), &err))
      << err.message;
  acceptMethods("B4LvSelf", 0, {"gone\n  ^nil\n"});
  e = AoSpan{};
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("Object", "B4LvSelf", "a", "").c_str(), &e));
  EXPECT_STREQ("class variable change refused: B4LvSelf>>gone refers to removed class variable Gone",
               e.message);
  expectPrints({{"oldSelf gone", "44"}, {"B4LvSelf classPool includesKey: #Gone", "true"}});

  // 何も届かないサブクラスは数えない。ソースの表とメソッドのキャッシュはメソッドを持つが、数えない。
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvGc", "", "Gone").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4LvGc", "B4LvGcKid", "", "").c_str(), &err))
      << err.message;
  acceptMethods("B4LvGcKid", 0, {"gone\n  ^Gone\n"});
  expectPrints({{"tmp := B4LvGcKid new. tmp gone. tmp := nil. Smalltalk at: #B4LvGcKid put: nil. "
                 "tmp",
                 "nil"}});
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvGc", "", "").c_str(), &err))
      << err.message;
  expectPrints({{"B4LvGc classPool includesKey: #Gone", "false"}});
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4LvGc", "a", "").c_str(), &err))
      << err.message;
  ao_runtime_shutdown();
}

// B4 review (Claude L1) / SPEC §3.9: 失敗シナリオ。65 段に入れ子にしたブロックの中で使うクラス変数を
// 消せていた（64 段で探索を打ち切り、未使用とみなしていた）。深さによらず数える。形が変わるときの
// 消える変数の検査も同じ。
TEST(AcceptAbi, ReacceptCountsVariablesInBlocksNestedAtAnyDepth) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  auto nested = [](const char* var, int depth) {
    std::string src = "deep\n  ^";
    for (int i = 0; i < depth; ++i) {
      src += "[";
    }
    src += var;
    for (int i = 0; i < depth; ++i) {
      src += "] value";
    }
    return src + "\n";
  };
  struct Case {
    const char* name;
    const char* oldIvars;
    const char* oldCvars;
    const char* newIvars;
    const char* newCvars;
    const char* var;
    const char* expected;
  };
  const Case cases[] = {
      {"B4DeepDrop", "", "Keep Gone", "", "Keep", "Gone",
       "class variable change refused: B4DeepDrop>>deep refers to removed class variable Gone"},
      {"B4DeepShCv", "x", "Gone", "x y", "", "Gone",
       "shape change refused: B4DeepShCv>>deep refers to removed class variable Gone"},
      {"B4DeepShIv", "x y", "", "x", "", "y",
       "shape change refused: B4DeepShIv>>deep refers to removed instance variable y"},
  };
  for (const Case& c : cases) {
    for (const int depth : {64, 65, 130}) {
      const std::string name = std::string(c.name) + std::to_string(depth);
      SCOPED_TRACE(name);
      const std::string before = b4Definition("Object", name.c_str(), c.oldIvars, c.oldCvars);
      ASSERT_EQ(AO_OK, ao_accept_class(before.c_str(), &err)) << err.message;
      ASSERT_EQ(AO_OK, ao_accept_method(name.c_str(), 0, nested(c.var, depth).c_str(), &err))
          << err.message;
      const std::string run = name + " new deep";
      expectPrints({{run.c_str(), "nil"}});
      AoSpan e{};
      EXPECT_EQ(AO_ERR_COMPILE,
                ao_accept_class(
                    b4Definition("Object", name.c_str(), c.newIvars, c.newCvars).c_str(), &e));
      std::string expected = c.expected;
      expected.replace(expected.find(c.name), std::strlen(c.name), name);
      EXPECT_EQ(expected, e.message);
    }
  }
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
  ASSERT_EQ(AO_OK, ao_image_load(path, nullptr));
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

// B4 review (Claude M1) / SPEC §3.6: 失敗シナリオ。subclass: に名前を String で渡しても、クラスの
// name は intern した Symbol で、Smalltalk もその Symbol で引ける。
TEST(AcceptAbi, SubclassInternsAStringName) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  expectPrints({
      {"(Object subclass: 'B4Zq' instanceVariableNames: '' classVariableNames: '' "
       "poolDictionaries: '' category: 'B4-Test') name == #B4Zq",
       "true"},
      {"B4Zq name class == Symbol", "true"},
      {"(Smalltalk at: #B4Zq) == B4Zq", "true"},
      {"B4Zq class name", "'B4Zq class'"},
      {"B4Zq new class == B4Zq", "true"},
  });
  ao_runtime_shutdown();
}

// B4 review (Claude M2) / SPEC §3.6, §3.8: 失敗シナリオ。Kernel クラスが足したスロットに、コンパイル
// したコードから代入できると、ネイティブが前提にする形が壊れる（OrderedCollection の array に
// String を入れると add: が slotAtPut のアサーションで止まる）。代入はコンパイルエラーで、読みは
// 今までどおり許す。継承したスロットも、クラス側の Behavior の枠も同じ。自分の変数には代入できる。
TEST(AcceptAbi, KernelInstanceVariablesAreReadOnlyInSource) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const struct {
    const char* cls;
    int meta;
    const char* source;
    const char* name;
  } refused[] = {
      {"OrderedCollection", 0, "b4zap\n  array := 'x'\n", "array"},
      {"OrderedCollection", 0, "b4zap\n  ^firstIndex := 0\n", "firstIndex"},
      {"Set", 0, "b4zap\n  tally := 'x'\n", "tally"},
      {"Dictionary", 0, "b4zap\n  array := nil\n", "array"},
      {"ReadStream", 0, "b4zap\n  position := 'x'\n", "position"},
      {"WriteStream", 0, "b4zap\n  writeLimit := 'x'\n", "writeLimit"},
      {"SmalltalkImage", 0, "b4zap\n  array := 3\n", "array"},
      {"Association", 0, "b4zap\n  [:v | key := v] value: 3\n", "key"},
      {"Object", 1, "b4zap\n  methodDict := nil\n", "methodDict"},
      {"Object", 1, "b4zap\n  superclass := nil. format := 3\n", "superclass"},
      {"OrderedCollection", 1, "b4zap\n  instVarNames := #()\n", "instVarNames"},
  };
  for (const auto& r : refused) {
    AoSpan err{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method(r.cls, r.meta, r.source, &err))
        << r.cls << (r.meta ? " class" : "") << ": " << r.source;
    EXPECT_EQ(std::string("cannot assign to Kernel instance variable ") + r.name, err.message)
        << r.cls << (r.meta ? " class" : "") << ": " << r.source;
  }

  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("OrderedCollection", "B4RoOC", "extra", "B4-Test")
                                       .c_str(),
                                   &err))
      << err.message;
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B4RoOC", 0, "b4zap\n  lastIndex := 9\n", &err));
  EXPECT_STREQ("cannot assign to Kernel instance variable lastIndex", err.message);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B4RoOC", 1, "b4zap\n  category := 'x'\n", &err));
  EXPECT_STREQ("cannot assign to Kernel instance variable category", err.message);
  const std::string chunks = "!B4RoOC methodsFor: 'b4'!\nb4zap\n  array := 3! !\n";
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(chunks.c_str(), &err));
  EXPECT_STREQ("cannot assign to Kernel instance variable array", err.message);
  acceptMethods("B4RoOC", 0,
                {"setExtra: v\n  extra := v\n", "extra\n  ^extra\n", "peekFirst\n  ^firstIndex\n"});
  acceptMethods("B4RoOC", 1, {"peekCategory\n  ^category\n"});
  acceptMethods("Association", 0, {"b4probeKey\n  ^key\n"});
  expectPrints({
      {"OrderedCollection new add: 1; add: 2; size", "2"},
      {"(OrderedCollection new respondsTo: #b4zap)", "false"},
      {"(Object class includesSelector: #b4zap)", "false"},
      {"c := B4RoOC new setExtra: 5; yourself. c add: 9. c size", "1"},
      {"c extra", "5"},
      {"c peekFirst", "1"},
      {"B4RoOC peekCategory", "'B4-Test'"},
      {"(Association key: 3 value: 4) b4probeKey", "3"},
  });
  ao_runtime_shutdown();
}

// B4 review (Claude M3) / SPEC §3.6: 失敗シナリオ。CV classPool at: #Count put: 5 が classPool の
// 束縛を素の 5 に差し替え、既存のメソッドは古い束縛を使い続け、新しい reread ^Count は install failed
// で拒まれた。classPool は写し（束縛は共有）を答えるので、at:put: は本体に届かず、value: は届く。
// classPool そのものの値が束縛でなくなったら（クラス側の classPool を直接書き換えた）、その名前を
// 使うメソッドは名前入りの理由で拒む。
TEST(AcceptAbi, ClassPoolAtPutLeavesTheBindingsAlone) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("Object", "B4CvPool", "", "Count").c_str(), &err))
      << err.message;
  acceptMethods("B4CvPool", 0, {"count\n  ^Count\n", "bump\n  Count := 2\n"});
  expectPrints({
      {"B4CvPool new bump; count", "2"},
      {"B4CvPool classPool at: #Count put: 5. B4CvPool new count", "2"},
      {"(B4CvPool classPool at: #Count) class == Association", "true"},
      {"(B4CvPool classPool at: #Count) value", "2"},
      {"B4CvPool classPool at: #Other put: 1. B4CvPool classPool size", "1"},
      {"(B4CvPool classPool at: #Count) value: 5. B4CvPool new count", "5"},
      {"(B4CvPool classPool at: #Count) == (B4CvPool classPool at: #Count)", "true"},
      {"B4CvPool classPool class == Dictionary", "true"},
      {"Object classPool isNil", "true"},
  });
  acceptMethods("B4CvPool", 0, {"reread\n  ^Count\n"});
  expectPrints({{"B4CvPool new reread", "5"}});
  ASSERT_EQ(AO_OK, ao_accept_class(b4Definition("B4CvPool", "B4CvPoolSub", "", "").c_str(), &err))
      << err.message;
  acceptMethods("B4CvPoolSub", 0, {"peekCount\n  ^Count\n"});

  // クラス側のメソッドは classPool（Behavior の枠）を読める。そこから本体の束縛を差し替えると、
  // その名前を使うメソッドは、Accept でも file-in でも形の変更でも入らない。
  acceptMethods("B4CvPool", 1, {"smash\n  classPool at: #Count put: 7\n"});
  expectPrints({{"B4CvPool smash. B4CvPool new count", "5"}});
  const std::string why = "class variable Count is not bound to an Association";
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B4CvPool", 0, "again\n  ^Count\n", &err));
  EXPECT_EQ(why, err.message);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B4CvPool", 1, "again\n  ^[Count := 1] value\n", &err));
  EXPECT_EQ(why, err.message);
  const std::string chunks = "!B4CvPool methodsFor: 'b4'!\nagain\n  ^Count! !\n";
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_class(chunks.c_str(), &err));
  EXPECT_EQ(why, err.message);
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b4Definition("B4CvPool", "B4CvPoolSub", "z", "").c_str(), &err));
  EXPECT_EQ("shape change refused: B4CvPoolSub>>peekCount: " + why, err.message);
  expectPrints({
      {"B4CvPoolSub instSize", "0"},
      {"B4CvPoolSub new peekCount", "5"},
      {"(B4CvPool new respondsTo: #again)", "false"},
  });
  ao_runtime_shutdown();
}

// B4 review (Claude M4) / SPEC §3.6: 失敗シナリオ。OrderedCollection subclass: #OC2
// instanceVariableNames: 'array extra' が通り、OC2>>mine ^array は親の内部 Array を返して、OC2 の
// array には届かなかった。継承した名前と、同じ定義の中の重複は、クラスを作らずに中断する。名前の
// 無いスロットは数えない。形を変える再 Accept でも同じで、名前は旧クラスのまま。
TEST(AcceptAbi, SubclassRefusesRedeclaredInstanceVariables) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[128];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 128, &err);
  };
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("OrderedCollection", "B4OC2", "array extra", "B4-Test")
                                .c_str(),
                            &err));
  EXPECT_STREQ("subclass failed: B4OC2: duplicate instance variable: array", err.message);
  EXPECT_EQ("<missing>", b5ClassDefinition("B4OC2"));

  EXPECT_EQ(AO_ERR_EVAL, printIt("Object subclass: #B4Dup instanceVariableNames: 'a b a' "
                                 "classVariableNames: '' poolDictionaries: '' category: 'B4-Test'"));
  EXPECT_STREQ("duplicate instance variable: a", err.message);
  EXPECT_EQ("<missing>", b5ClassDefinition("B4Dup"));
  // Behavior の枠はメタクラスのインスタンス（クラス）のもので、インスタンスの名前とは重ならない。
  EXPECT_EQ(AO_OK, printIt("Object subclass: #B4DupCat instanceVariableNames: 'category' "
                           "classVariableNames: '' poolDictionaries: '' category: 'B4-Test'"))
      << err.message;

  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("Object", "B4DupSup", "x", "B4-Test").c_str(), &err))
      << err.message;
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("B4DupSup", "B4DupSub", "y x", "B4-Test").c_str(), &err));
  EXPECT_STREQ("subclass failed: B4DupSub: duplicate instance variable: x", err.message);
  EXPECT_EQ("<missing>", b5ClassDefinition("B4DupSub"));

  ASSERT_EQ(AO_OK, ao_accept_class(b5Definition("B4DupSup", "B4DupRe", "y", "B4-Test").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B4DupRe", 0, "getY\n  ^y\n", &err)) << err.message;
  EXPECT_EQ(AO_ERR_COMPILE,
            ao_accept_class(b5Definition("B4DupSup", "B4DupRe", "y x", "B4-Test").c_str(), &err));
  EXPECT_STREQ("subclass failed: B4DupRe: duplicate instance variable: x", err.message);

  // 名前の無いスロットの名前は数えない（instVarNames を消したクラスの下）。
  ASSERT_EQ(AO_OK, printIt("Rectangle instVarAt: 8 put: nil. 0")) << err.message;
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Rectangle", "B4DupRect", "origin", "B4-Test").c_str(), &err))
      << err.message;
  expectPrints({
      {"B4DupRe instSize", "2"},
      {"B4DupRe new getY", "nil"},
      {"B4DupRect instSize", "3"},
      {"(Smalltalk includesKey: #B4Dup) | (Smalltalk includesKey: #B4DupSub)", "false"},
  });
  ao_runtime_shutdown();
}

// B4 review (Claude L) / SPEC §3.6: 失敗シナリオ。Smalltalk at: #nil put: 3 が通り、Smalltalk at: #nil
// が 3 を答えた。擬変数の名前は、固定のグローバルと同じく理由付きで拒み、辞書は変わらない。
// subclass: も擬変数の名前では登録しない。
TEST(AcceptAbi, SmalltalkRefusesPseudoVariableKeys) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  for (const char* name : {"nil", "true", "false", "self", "super", "thisContext"}) {
    for (const char* quote : {"#", "'"}) {
      std::string src = "Smalltalk at: ";
      src += quote;
      src += name;
      src += quote[0] == '\'' ? "'" : "";
      src += " put: 3";
      char out[128];
      AoSpan err{};
      EXPECT_EQ(AO_ERR_EVAL, ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT,
                                     out, 128, &err))
          << src;
      EXPECT_EQ(std::string("cannot bind pseudo-variable: ") + name, err.message) << src;
    }
  }
  expectPrints({
      {"Smalltalk includesKey: #nil", "false"},
      {"Smalltalk includesKey: #thisContext", "false"},
      {"Smalltalk at: #self ifAbsent: [7]", "7"},
      {"(Object subclass: #super instanceVariableNames: '' classVariableNames: '' "
       "poolDictionaries: '' category: 'B4-Test') name == #super",
       "true"},
      {"Smalltalk includesKey: #super", "false"},
      {"Smalltalk at: #B4NotPseudo put: 4", "4"},
  });
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 High) / SPEC §3.8: `,` is a binary selector character. `'a' , 'b'`
// was "invalid token"; now it compiles, and a method with the pattern `, other` runs.
TEST(AcceptAbi, CommaIsABinarySelector) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B7Comma\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B7-Test'\n",
                                   &err))
      << err.message;
  acceptMethods("B7Comma", 0, {", other\n  ^other + 1\n"});
  expectPrints({
      {"B7Comma new , 2", "3"},
      {"B7Comma new,4", "5"},
      {"(B7Comma new perform: #, with: 6)", "7"},
      {"#, size", "1"},
      {"(#(#, 1) at: 1) == #,", "true"},
  });
  char out[128];
  const char* concat = "'a' , 'b'";
  EXPECT_NE(AO_ERR_COMPILE, ao_eval(concat, static_cast<int>(std::strlen(concat)),
                                    AO_EVAL_PRINTIT, out, 128, &err))
      << err.message;
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 Medium) / SPEC §3.8: `2*-1` was the selector `*-` (doesNotUnderstand:).
// A `-` after the first binary character, followed by a digit, starts a negative literal.
TEST(AcceptAbi, MinusAfterBinaryCharacterStartsNegativeLiteral) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B7Minus\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B7-Test'\n",
                                   &err))
      << err.message;
  acceptMethods("B7Minus", 0, {"foo: n\n  ^n\n", "@ n\n  ^n\n", "sub: x from: y\n  ^y-x\n"});
  expectPrints({
      {"2*-1", "-2"},
      {"B7Minus new@-2", "-2"},
      {"3>-1", "true"},
      {"3--1", "4"},
      {"3-1", "2"},
      {"3 - 1", "2"},
      {"3 -1", "2"},
      {"B7Minus new foo: -1", "-1"},
      {"B7Minus new foo:-1", "-1"},
      {"B7Minus new sub: 1 from: 5", "4"},
  });
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 High, 03 High) / SPEC §3.8: an integer literal outside int64 became 0
// (`100000000000000000000 printString` was '0'). It is a LargeInteger now, and a value that fits
// SmallInteger stays one.
TEST(AcceptAbi, IntegerLiteralsBeyondInt64AreLargeIntegers) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B7Big\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B7-Test'\n",
                                   &err))
      << err.message;
  acceptMethods("B7Big", 0, {"big\n  ^100000000000000000000\n"});
  expectPrints({
      {"100000000000000000000 = (10000000000 * 10000000000)", "true"},
      {"100000000000000000000 class == LargePositiveInteger", "true"},
      {"100000000000000000000 = 200000000000000000000", "false"},
      {"-100000000000000000000 = (0 - (10000000000 * 10000000000))", "true"},
      {"-100000000000000000000 class == LargeNegativeInteger", "true"},
      {"-9223372036854775808 = (0 - (1 bitShift: 63))", "true"},
      {"9223372036854775808 = (1 bitShift: 63)", "true"},
      {"18446744073709551616 = (1 bitShift: 64)", "true"},
      {"16r1FFFFFFFFFFFFFFFFFFFF = ((1 bitShift: 81) - 1)", "true"},
      {"-2r10000000000000000000000000000000000000000000000000000000000000000000000 = "
       "(0 - (1 bitShift: 70))",
       "true"},
      {"(#(100000000000000000000 -100000000000000000000) at: 1) = (10000000000 * 10000000000)",
       "true"},
      {"(#(100000000000000000000 -100000000000000000000) at: 2) class == LargeNegativeInteger",
       "true"},
      {"B7Big new big = (10000000000 * 10000000000)", "true"},
      {"4611686018427387903 class == SmallInteger", "true"},
      {"-4611686018427387904 class == SmallInteger", "true"},
      {"4611686018427387904 class == LargePositiveInteger", "true"},
      {"0000000000000000000000000001 class == SmallInteger", "true"},
      {"| s | s := 0. 1 to: 3 by: 100000000000000000000 do: [:k | s := s + k]. s", "1"},
      {"| s | s := 0. 3 to: 1 by: -100000000000000000000 do: [:k | s := s + k]. s", "3"},
  });
  char out[128];
  const char* bytes = "#[1 99999999999999999999 3]";
  EXPECT_EQ(AO_ERR_COMPILE, ao_eval(bytes, static_cast<int>(std::strlen(bytes)), AO_EVAL_PRINTIT,
                                    out, 128, &err));
  EXPECT_STREQ("expected byte 0-255", err.message);
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/03 Medium) / SPEC §3.8: a Float literal is the correctly rounded value of
// the whole token. `0.3 = (0.1 + 0.2)` was true and `0.7 = (7 / 10.0)` false.
TEST(AcceptAbi, FloatLiteralsAreCorrectlyRounded) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  expectPrints({
      {"0.7 = (7 / 10.0)", "true"},
      {"0.3 = (0.1 + 0.2)", "false"},
      {"((0.3 - 0.1) - 0.2) < 0.0", "true"},
      {"-0.7 = (0.0 - (7 / 10.0))", "true"},
      {"1.5e2 = 150.0", "true"},
      {"1.0e400 printString", "'inf'"},
      {"0.0e500 = 0.0", "true"},
      {"16r1.8 = 1.5", "true"},
      {"2r1.1e2 = 6.0", "true"},
      {"(#(0.7 -0.7) at: 1) = (7 / 10.0)", "true"},
      {"(#(0.7 -0.7) at: 2) = (0.0 - (7 / 10.0))", "true"},
  });
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 Low) / SPEC §3.8: an integer mantissa with an exponent of 0 or more
// is an Integer (`1e3` was 1000.0, `2r1e4` 10000.0, `0e500` inf); a negative exponent or a
// fraction makes a correctly rounded Float.
TEST(AcceptAbi, IntegerMantissaWithExponentIsInteger) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  expectPrints({
      {"1e3 = 1000", "true"},
      {"1e3 class == SmallInteger", "true"},
      {"2r1e4 = 16", "true"},
      {"0e500 = 0", "true"},
      {"-1e3 = (0 - 1000)", "true"},
      {"1e30 = (1000000000000000 * 1000000000000000)", "true"},
      {"1e30 class == LargePositiveInteger", "true"},
      {"-1e30 = (0 - (1000000000000000 * 1000000000000000))", "true"},
      {"(#(1e3 2r1e4) at: 2) = 16", "true"},
      {"1e-3 = (1 / 1000.0)", "true"},
      {"1e-3 class == Float", "true"},
      {"2r1e-2 = 0.25", "true"},
      {"1.5e2 = 150.0", "true"},
  });
  char out[128];
  AoSpan err{};
  const char* huge = "1e65537";
  EXPECT_EQ(AO_ERR_COMPILE, ao_eval(huge, static_cast<int>(std::strlen(huge)), AO_EVAL_PRINTIT,
                                    out, 128, &err));
  EXPECT_STREQ("number too large", err.message);
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 High) / SPEC §3.8: a block with arguments and temps was "expected
// ']'", and `[:a :b | a | b]` read `a` as a temp and answered false. After the arguments the `|`
// is required; `||` is the separator and the opening of the temps.
TEST(AcceptAbi, BlockArgumentsThenTemps) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  expectPrints({
      {"[:x | | t | t := x. t] value: 3", "3"},
      {"[:x || t | t := x. t] value: 3", "3"},
      {"[:a :b | a | b] value: true value: false", "true"},
      {"[:a | a | false] value: true", "true"},
      {"[:a | ] value: 1", "nil"},
      {"[ | t | t ] value", "nil"},
      {"([:x || t | t := x. [t + 1]] value: 4) value", "5"},
  });
  char out[128];
  AoSpan err{};
  const char* missing = "[:a] value: 1";
  EXPECT_EQ(AO_ERR_COMPILE, ao_eval(missing, static_cast<int>(std::strlen(missing)),
                                    AO_EVAL_PRINTIT, out, 128, &err));
  EXPECT_STREQ("expected '|'", err.message);
  EXPECT_EQ(3u, err.start);
  EXPECT_EQ(4u, err.end);
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 Low) / SPEC §3.8: a cascade part took one message only (`x add: 3;
// yourself size` was "unexpected token"), and `^super who; who` sent the second who to self.
TEST(AcceptAbi, CascadePartsAreMessageChains) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  for (const char* def : {"Object subclass: #B7CascA\n  instanceVariableNames: ''\n"
                          "  classVariableNames: ''\n  poolDictionaries: ''\n"
                          "  category: 'B7-Test'\n",
                          "B7CascA subclass: #B7CascB\n  instanceVariableNames: ''\n"
                          "  classVariableNames: ''\n  poolDictionaries: ''\n"
                          "  category: 'B7-Test'\n"}) {
    ASSERT_EQ(AO_OK, ao_accept_class(def, &err)) << err.message;
  }
  acceptMethods("B7CascA", 0, {"who\n  ^1\n"});
  acceptMethods("B7CascB", 0,
                {"who\n  ^2\n", "superWho\n  ^super who; who\n",
                 "superChain\n  ^super who; yourself; who + 10\n"});
  expectPrints({
      {"OrderedCollection new add: 3; add: 4; yourself size", "2"},
      {"3 + 4; * 10 + 1", "31"},
      {"12 + 4; printString size * 2", "4"},
      {"B7CascB new superWho", "1"},
      {"B7CascB new superChain", "11"},
      {"B7CascB new who", "2"},
  });
  ao_runtime_shutdown();
}

// B7 (docs/claude-review/05 Medium) / SPEC §3.8: 10 000 nested parentheses crashed ao_eval with
// SIGSEGV (the parser and the code generator recurse per level), and so did a chain of 100 000
// messages or assignments. 256 levels still evaluate; past them is the compile error
// "nesting too deep" at the construct that goes past the limit.
TEST(AcceptAbi, NestingTooDeepIsACompileError) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const auto repeated = [](const std::string& s, int n) {
    std::string out;
    for (int i = 0; i < n; ++i) {
      out += s;
    }
    return out;
  };
  struct Deep {
    std::string source;
    const char* printed;
  };
  const std::vector<Deep> evaluated{
      {repeated("(", 256) + "7" + repeated(")", 256), "7"},
      {repeated("[", 256) + "7" + repeated("] value", 256), "7"},
      {"#" + repeated("(", 256) + "7" + repeated(")", 256) + " size", "1"},
      {"7" + repeated(" yourself", 257), "7"},
      {"0" + repeated(" + 1", 257), "257"},
      {"| a | " + repeated("a := ", 256) + "7", "7"},
  };
  for (const Deep& d : evaluated) {
    char out[128];
    AoSpan err{};
    ASSERT_EQ(AO_OK, ao_eval(d.source.c_str(), static_cast<int>(d.source.size()),
                             AO_EVAL_PRINTIT, out, 128, &err))
        << d.source.substr(0, 40) << ": " << err.message;
    EXPECT_STREQ(d.printed, out) << d.source.substr(0, 40);
  }
  struct TooDeep {
    std::string source;
    unsigned start;
    unsigned end;
  };
  const std::vector<TooDeep> refused{
      {repeated("(", 10000) + "7" + repeated(")", 10000), 256, 257},
      {repeated("[", 10000) + "7" + repeated("]", 10000), 256, 257},
      {"#" + repeated("(", 10000) + "7" + repeated(")", 10000), 257, 258},
      {"7" + repeated(" yourself", 100000), 2 + 9 * 257, 2 + 9 * 257 + 8},
      {"0" + repeated(" + 1", 100000), 2 + 4 * 257, 3 + 4 * 257},
      {"| a | " + repeated("a := ", 100000) + "7", 6 + 5 * 256, 7 + 5 * 256},
  };
  for (const TooDeep& d : refused) {
    char out[128];
    AoSpan err{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_eval(d.source.c_str(), static_cast<int>(d.source.size()),
                                      AO_EVAL_PRINTIT, out, 128, &err))
        << d.source.substr(0, 40);
    EXPECT_STREQ("nesting too deep", err.message) << d.source.substr(0, 40);
    EXPECT_EQ(d.start, err.start) << d.source.substr(0, 40);
    EXPECT_EQ(d.end, err.end) << d.source.substr(0, 40);
  }
  ao_runtime_shutdown();
}

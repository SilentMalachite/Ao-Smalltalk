#include "ao_abi.h"

#include <algorithm>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

class BrowserAbi : public ::testing::Test {
 protected:
  void TearDown() override { EXPECT_EQ(AO_OK, ao_runtime_shutdown()); }
};

TEST_F(BrowserAbi, ObjectIsKernelAndPrintStringIsNative) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const int n = ao_browser_class_count();
  ASSERT_GE(n, 56);
  bool found = false;
  char name[128];
  char category[128];
  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, name, 128, category, 128));
    if (std::strcmp(name, "Object") == 0) {
      found = true;
      EXPECT_STREQ("Kernel", category);
    }
  }
  EXPECT_TRUE(found);
  EXPECT_EQ(1, ao_browser_protocol_count("Object", 0));
  char protocol[32];
  ASSERT_EQ(AO_OK, ao_browser_protocol_at("Object", 0, 0, protocol, 32));
  EXPECT_STREQ("native", protocol);
  char source[256];
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("Object", 0, "printString", source, 256));
  EXPECT_STREQ("\"Object>>printString native ao_Object_printString\"", source);
  char defn[512];
  ASSERT_EQ(AO_OK, ao_browser_class_definition("Object", defn, 512));
  EXPECT_NE(std::string(defn).find("subclass: #Object"), std::string::npos);
  char superName[128];
  ASSERT_EQ(AO_OK, ao_browser_superclass("Object", 0, superName, 128));
  EXPECT_STREQ("", superName);
  ASSERT_EQ(AO_OK, ao_browser_superclass("SmallInteger", 0, superName, 128));
  EXPECT_STREQ("Integer", superName);

  const char* subclassSel =
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:";
  const int classNative = ao_browser_selector_count("Class", 0, "native");
  ASSERT_GT(classNative, 1);
  bool sawSubclass = false;
  char selector[128];
  for (int i = 0; i < classNative; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_selector_at("Class", 0, "native", i, selector, 128));
    if (std::strcmp(selector, subclassSel) == 0) {
      sawSubclass = true;
    }
  }
  EXPECT_TRUE(sawSubclass);
  EXPECT_EQ(0, ao_browser_selector_count("Object", 1, "native"));

  const int transcriptClassSide = ao_browser_selector_count("Transcript", 1, "native");
  ASSERT_GT(transcriptClassSide, 0);
  bool sawShow = false;
  for (int i = 0; i < transcriptClassSide; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_selector_at("Transcript", 1, "native", i, selector, 128));
    if (std::strcmp(selector, "show:") == 0) {
      sawShow = true;
    }
  }
  EXPECT_TRUE(sawShow);

  const int subs = ao_browser_subclass_count("Object");
  ASSERT_GT(subs, 0);
  std::vector<std::string> subclassNames;
  char sub[128];
  for (int i = 0; i < subs; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_subclass_at("Object", i, sub, 128));
    subclassNames.emplace_back(sub);
  }
  EXPECT_TRUE(std::is_sorted(subclassNames.begin(), subclassNames.end()));
  EXPECT_NE(std::find(subclassNames.begin(), subclassNames.end(), "Behavior"), subclassNames.end());
  EXPECT_EQ(std::find(subclassNames.begin(), subclassNames.end(), "SmallInteger"),
            subclassNames.end());

  EXPECT_EQ(AO_ERR, ao_browser_source("NoSuchClass", 0, "printString", source, 256));
  EXPECT_EQ(AO_ERR, ao_browser_superclass("NoSuchClass", 0, superName, 128));
  EXPECT_EQ(-1, ao_browser_subclass_count("NoSuchClass"));
  ao_runtime_shutdown();
}

// 00 Critical / SPEC §3.10: ソース表にソースの無いメソッドは AO_ERR_NOSOURCE と、コメント 1 つだけの
// プレースホルダ。プレースホルダを Accept してもコンパイルが失敗し、メソッドは変わらない。
// ao_accept_class の methodsFor: のチャンクは file-in と同じくソースを残さない。
TEST_F(BrowserAbi, SourcelessMethodAnswersNoSourceAndPlaceholderDoesNotCompile) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  auto printIt = [&](const char* src) {
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #B5Src\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B5-Test'!\n"
                                   "!B5Src methodsFor: 'b5'!\n"
                                   "eight\n"
                                   "  ^8!\n"
                                   "at: i put: v\n"
                                   "  ^v! !\n"
                                   "!B5Src class methodsFor: 'b5'!\n"
                                   "make\n"
                                   "  ^self new! !\n",
                                   &err))
      << err.message;
  const char* seven = "seven\n  ^7\n";
  ASSERT_EQ(AO_OK, ao_accept_method("B5Src", 0, seven, &err)) << err.message;

  char source[256];
  // ソースのあるユーザーメソッドは、これまでどおり AO_OK と本文。
  ASSERT_EQ(AO_OK, ao_browser_source("B5Src", 0, "seven", source, 256));
  EXPECT_STREQ(seven, source);

  struct Sourceless {
    int meta;
    const char* selector;
    const char* placeholder;
  };
  const Sourceless sourceless[] = {
      {0, "eight", "\"B5Src>>eight source not available\""},
      {0, "at:put:", "\"B5Src>>at:put: source not available\""},
      {1, "make", "\"B5Src class>>make source not available\""},
  };
  for (const Sourceless& m : sourceless) {
    SCOPED_TRACE(m.selector);
    ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("B5Src", m.meta, m.selector, source, 256));
    EXPECT_STREQ(m.placeholder, source);
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B5Src", m.meta, source, &e));
    EXPECT_STRNE("", e.message);
    // 本体は変わらず、ソースも無いまま。
    ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("B5Src", m.meta, m.selector, source, 256));
    EXPECT_STREQ(m.placeholder, source);
  }
  ASSERT_EQ(AO_OK, printIt("B5Src new eight")) << err.message;
  EXPECT_STREQ("8", out);
  ASSERT_EQ(AO_OK, printIt("B5Src new at: 1 put: 9")) << err.message;
  EXPECT_STREQ("9", out);
  ASSERT_EQ(AO_OK, printIt("B5Src make class == B5Src")) << err.message;
  EXPECT_STREQ("true", out);

  // ネイティブのプレースホルダも、Accept してもコンパイルが失敗する。
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("Object", 0, "printString", source, 256));
  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, source, &e));
  EXPECT_STRNE("", e.message);
  ASSERT_EQ(AO_OK, printIt("Object new printString")) << err.message;
  EXPECT_STREQ("'Object'", out);

  // 入り切らないときも AO_ERR_NOSOURCE が勝ち、buf は切り詰めて NUL で終わる。
  char small[8];
  std::memset(small, 'x', sizeof small);
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("B5Src", 0, "eight", small, 8));
  EXPECT_STREQ("\"B5Src>", small);
  char one[1] = {'x'};
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source("B5Src", 0, "eight", one, 1));
  EXPECT_EQ('\0', one[0]);
  // buf が NULL か len が 1 未満は AO_ERR。ソースのあるメソッドの切り詰めは AO_ERR_RANGE のまま。
  EXPECT_EQ(AO_ERR, ao_browser_source("B5Src", 0, "eight", nullptr, 8));
  EXPECT_EQ(AO_ERR, ao_browser_source("B5Src", 0, "eight", small, 0));
  ASSERT_EQ(AO_ERR_RANGE, ao_browser_source("B5Src", 0, "seven", small, 8));
  EXPECT_STREQ("seven\n ", small);
  EXPECT_EQ(AO_ERR, ao_browser_source("B5Src", 0, "nine", source, 256));
}

// 06 Low / SPEC §3.10: 件数を返す関数は、失敗なら -1 を返す（AO_ERR の 1 は 1 件と区別できない）。
// 失敗は、セッションが無い、名前がクラスに当たらない、meta が 0 でも 1 でない、引数が NULL。
TEST_F(BrowserAbi, CountsAnswerMinusOneOnFailure) {
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(-1, ao_browser_class_count());
  EXPECT_EQ(-1, ao_browser_protocol_count("Object", 0));
  EXPECT_EQ(-1, ao_browser_selector_count("Object", 0, "native"));
  EXPECT_EQ(-1, ao_browser_subclass_count("Object"));

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_GT(ao_browser_class_count(), 0);
  EXPECT_EQ(-1, ao_browser_protocol_count("NoSuchClass", 0));
  EXPECT_EQ(-1, ao_browser_selector_count("NoSuchClass", 0, "native"));
  EXPECT_EQ(-1, ao_browser_subclass_count("NoSuchClass"));
  EXPECT_EQ(-1, ao_browser_protocol_count("Object", 2));
  EXPECT_EQ(-1, ao_browser_protocol_count("Object", -1));
  EXPECT_EQ(-1, ao_browser_selector_count("Object", 2, "native"));
  EXPECT_EQ(-1, ao_browser_protocol_count(nullptr, 0));
  EXPECT_EQ(-1, ao_browser_selector_count(nullptr, 0, "native"));
  EXPECT_EQ(-1, ao_browser_selector_count("Object", 0, nullptr));
  EXPECT_EQ(-1, ao_browser_subclass_count(nullptr));
  // 成功なら 0 以上。
  EXPECT_EQ(1, ao_browser_protocol_count("Object", 0));
  EXPECT_GT(ao_browser_selector_count("Object", 0, "native"), 0);
  EXPECT_EQ(0, ao_browser_selector_count("Object", 0, "user"));
}

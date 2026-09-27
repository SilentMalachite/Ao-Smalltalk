#include "ao_abi.h"

#include "../src/Session.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace {

int doIt(const char* source) {
  char out[64];
  AoSpan err{};
  return ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_DOIT, out, 64, &err);
}

int defineRow(const char* name, const char* category = "B12-Id", const char* super = "Object") {
  const std::string def = std::string(super) + " subclass: #" + name +
                          "\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n  category: '" + category + "'\n";
  AoSpan err{};
  return ao_accept_class(def.c_str(), &err);
}

// Whether a class-list row carries id.
bool listsId(std::int64_t id) {
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    std::int64_t rowId = 0;
    char name[256];
    char category[256];
    if (ao_browser_class_at(i, &rowId, name, 256, category, 256) == AO_OK && rowId == id) {
      return true;
    }
  }
  return false;
}

// Whether the session's class ID table holds id.
bool sessionHoldsId(std::int64_t id) {
  const ao::Session* s = ao::session();
  return s != nullptr && std::any_of(s->classIds.begin(), s->classIds.end(),
                                     [&](const auto& entry) { return entry->id == id; });
}

}  // namespace

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
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, nullptr, name, 128, category, 128));
    if (std::strcmp(name, "Object") == 0) {
      found = true;
      EXPECT_STREQ("Kernel", category);
    }
  }
  EXPECT_TRUE(found);
  EXPECT_EQ(1, ao_browser_protocol_count(ao_browser_class_id("Object"), 0));
  char protocol[32];
  ASSERT_EQ(AO_OK, ao_browser_protocol_at(ao_browser_class_id("Object"), 0, 0, protocol, 32));
  EXPECT_STREQ("native", protocol);
  char source[256];
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("Object"), 0, "printString", source, 256));
  EXPECT_STREQ("\"Object>>printString native ao_Object_printString\"", source);
  char defn[512];
  ASSERT_EQ(AO_OK, ao_browser_class_definition(ao_browser_class_id("Object"), defn, 512));
  EXPECT_NE(std::string(defn).find("subclass: #Object"), std::string::npos);
  char superName[128];
  ASSERT_EQ(AO_OK, ao_browser_superclass(ao_browser_class_id("Object"), 0, nullptr, superName, 128));
  EXPECT_STREQ("", superName);
  ASSERT_EQ(AO_OK, ao_browser_superclass(ao_browser_class_id("SmallInteger"), 0, nullptr, superName, 128));
  EXPECT_STREQ("Integer", superName);

  const char* subclassSel =
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:";
  const int classNative = ao_browser_selector_count(ao_browser_class_id("Class"), 0, "native");
  ASSERT_GT(classNative, 1);
  bool sawSubclass = false;
  char selector[128];
  for (int i = 0; i < classNative; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_selector_at(ao_browser_class_id("Class"), 0, "native", i, selector, 128));
    if (std::strcmp(selector, subclassSel) == 0) {
      sawSubclass = true;
    }
  }
  EXPECT_TRUE(sawSubclass);
  EXPECT_EQ(0, ao_browser_selector_count(ao_browser_class_id("Object"), 1, "native"));

  const int transcriptClassSide = ao_browser_selector_count(ao_browser_class_id("Transcript"), 1, "native");
  ASSERT_GT(transcriptClassSide, 0);
  bool sawShow = false;
  for (int i = 0; i < transcriptClassSide; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_selector_at(ao_browser_class_id("Transcript"), 1, "native", i, selector, 128));
    if (std::strcmp(selector, "show:") == 0) {
      sawShow = true;
    }
  }
  EXPECT_TRUE(sawShow);

  const int subs = ao_browser_subclass_count(ao_browser_class_id("Object"));
  ASSERT_GT(subs, 0);
  std::vector<std::string> subclassNames;
  char sub[128];
  for (int i = 0; i < subs; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_subclass_at(ao_browser_class_id("Object"), i, nullptr, sub, 128));
    subclassNames.emplace_back(sub);
  }
  EXPECT_TRUE(std::is_sorted(subclassNames.begin(), subclassNames.end()));
  EXPECT_NE(std::find(subclassNames.begin(), subclassNames.end(), "Behavior"), subclassNames.end());
  EXPECT_EQ(std::find(subclassNames.begin(), subclassNames.end(), "SmallInteger"),
            subclassNames.end());

  EXPECT_EQ(AO_ERR, ao_browser_source(ao_browser_class_id("NoSuchClass"), 0, "printString", source, 256));
  EXPECT_EQ(AO_ERR, ao_browser_superclass(ao_browser_class_id("NoSuchClass"), 0, nullptr, superName, 128));
  EXPECT_EQ(-1, ao_browser_subclass_count(ao_browser_class_id("NoSuchClass")));
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
  ASSERT_EQ(AO_OK, ao_browser_source(ao_browser_class_id("B5Src"), 0, "seven", source, 256));
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
    ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("B5Src"), m.meta, m.selector, source, 256));
    EXPECT_STREQ(m.placeholder, source);
    AoSpan e{};
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B5Src", m.meta, source, &e));
    EXPECT_STRNE("", e.message);
    // 本体は変わらず、ソースも無いまま。
    ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("B5Src"), m.meta, m.selector, source, 256));
    EXPECT_STREQ(m.placeholder, source);
  }
  ASSERT_EQ(AO_OK, printIt("B5Src new eight")) << err.message;
  EXPECT_STREQ("8", out);
  ASSERT_EQ(AO_OK, printIt("B5Src new at: 1 put: 9")) << err.message;
  EXPECT_STREQ("9", out);
  ASSERT_EQ(AO_OK, printIt("B5Src make class == B5Src")) << err.message;
  EXPECT_STREQ("true", out);

  // ネイティブのプレースホルダも、Accept してもコンパイルが失敗する。
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("Object"), 0, "printString", source, 256));
  AoSpan e{};
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, source, &e));
  EXPECT_STRNE("", e.message);
  ASSERT_EQ(AO_OK, printIt("Object new printString")) << err.message;
  EXPECT_STREQ("'Object'", out);

  // 入り切らないときも AO_ERR_NOSOURCE が勝ち、buf は切り詰めて NUL で終わる。
  char small[8];
  std::memset(small, 'x', sizeof small);
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("B5Src"), 0, "eight", small, 8));
  EXPECT_STREQ("\"B5Src>", small);
  char one[1] = {'x'};
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("B5Src"), 0, "eight", one, 1));
  EXPECT_EQ('\0', one[0]);
  // buf が NULL か len が 1 未満は AO_ERR。ソースのあるメソッドの切り詰めは AO_ERR_RANGE のまま。
  EXPECT_EQ(AO_ERR, ao_browser_source(ao_browser_class_id("B5Src"), 0, "eight", nullptr, 8));
  EXPECT_EQ(AO_ERR, ao_browser_source(ao_browser_class_id("B5Src"), 0, "eight", small, 0));
  ASSERT_EQ(AO_ERR_RANGE, ao_browser_source(ao_browser_class_id("B5Src"), 0, "seven", small, 8));
  EXPECT_STREQ("seven\n ", small);
  EXPECT_EQ(AO_ERR, ao_browser_source(ao_browser_class_id("B5Src"), 0, "nine", source, 256));
}

// 06 Low / SPEC §3.10: 件数を返す関数は、失敗なら -1 を返す（AO_ERR の 1 は 1 件と区別できない）。
// 失敗は、セッションが無い、クラス ID が未知、meta が 0 でも 1 でない、引数が NULL。
TEST_F(BrowserAbi, CountsAnswerMinusOneOnFailure) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::int64_t earlier = ao_browser_class_id("Object");
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(-1, ao_browser_class_count());
  EXPECT_EQ(-1, ao_browser_protocol_count(earlier, 0));
  EXPECT_EQ(-1, ao_browser_selector_count(earlier, 0, "native"));
  EXPECT_EQ(-1, ao_browser_subclass_count(earlier));
  EXPECT_EQ(0, ao_browser_class_id("Object"));

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::int64_t object = ao_browser_class_id("Object");
  EXPECT_GT(ao_browser_class_count(), 0);
  EXPECT_EQ(-1, ao_browser_protocol_count(0, 0));
  EXPECT_EQ(-1, ao_browser_selector_count(0, 0, "native"));
  EXPECT_EQ(-1, ao_browser_subclass_count(0));
  EXPECT_EQ(-1, ao_browser_protocol_count(object, 2));
  EXPECT_EQ(-1, ao_browser_protocol_count(object, -1));
  EXPECT_EQ(-1, ao_browser_selector_count(object, 2, "native"));
  EXPECT_EQ(-1, ao_browser_selector_count(object, 0, nullptr));
  // 成功なら 0 以上。
  EXPECT_EQ(1, ao_browser_protocol_count(object, 0));
  EXPECT_GT(ao_browser_selector_count(object, 0, "native"), 0);
  EXPECT_EQ(0, ao_browser_selector_count(object, 0, "user"));
}

// SPEC §3.10 クラス ID: a listed class has one positive ID, the same from every read and for every
// name Smalltalk binds to it; an alias adds no row. A name that binds no listed class is 0.
TEST_F(BrowserAbi, ClassIdIsStableAndOneRowPerClass) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Row", "B12-Row"));
  const int before = ao_browser_class_count();
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12RowAlias put: B12Row"));
  EXPECT_EQ(before, ao_browser_class_count());
  const std::int64_t id = ao_browser_class_id("B12Row");
  EXPECT_GT(id, 0);
  EXPECT_EQ(id, ao_browser_class_id("B12Row"));
  EXPECT_EQ(id, ao_browser_class_id("B12RowAlias"));
  const int n = ao_browser_class_count();
  std::vector<std::int64_t> ids;
  int rows = 0;
  char name[128];
  char category[128];
  for (int i = 0; i < n; ++i) {
    std::int64_t rowId = -7;
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, &rowId, name, 128, category, 128));
    EXPECT_GT(rowId, 0);
    ids.push_back(rowId);
    if (rowId == id) {
      ++rows;
      EXPECT_STREQ("B12Row", name);
      EXPECT_STREQ("B12-Row", category);
    }
    if (std::strcmp(name, "Object") == 0) {
      EXPECT_EQ(ao_browser_class_id("Object"), rowId);
    }
  }
  EXPECT_EQ(1, rows);
  std::sort(ids.begin(), ids.end());
  EXPECT_EQ(ids.end(), std::adjacent_find(ids.begin(), ids.end()));
  // The ID out-parameter may be NULL; a failed row writes 0.
  EXPECT_EQ(AO_OK, ao_browser_class_at(0, nullptr, name, 128, category, 128));
  std::int64_t none = -7;
  EXPECT_EQ(AO_ERR, ao_browser_class_at(n, &none, name, 128, category, 128));
  EXPECT_EQ(0, none);
  for (const char* notListed : {"B12NoSuchClass", "", "Processor", "Smalltalk", "nil"}) {
    EXPECT_EQ(0, ao_browser_class_id(notListed)) << notListed;
  }
  EXPECT_EQ(0, ao_browser_class_id(nullptr));
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12RowMeta put: B12Row class"));
  EXPECT_EQ(0, ao_browser_class_id("B12RowMeta"));
}

// SPEC §3.10 クラス ID: a class no name binds any more leaves the list, and the table drops its
// ID. The same name defined again is a new class with a new, larger ID; IDs are never reused.
TEST_F(BrowserAbi, ClassIdsAreDroppedWithTheirClassAndNeverReused) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Drop"));
  const std::int64_t first = ao_browser_class_id("B12Drop");
  ASSERT_GT(first, 0);
  EXPECT_TRUE(sessionHoldsId(first));
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12Drop put: nil"));
  EXPECT_EQ(0, ao_browser_class_id("B12Drop"));
  EXPECT_FALSE(listsId(first));
  EXPECT_FALSE(sessionHoldsId(first));
  ASSERT_EQ(AO_OK, defineRow("B12Drop"));
  const std::int64_t second = ao_browser_class_id("B12Drop");
  EXPECT_GT(second, first);
  EXPECT_TRUE(listsId(second));
  EXPECT_FALSE(listsId(first));
}

// SPEC §3.10 クラス ID: an ID that is 0 or less, never issued, or dropped with its class is
// unknown: the counts answer -1 and every other read AO_ERR (ID out-parameters get 0).
TEST_F(BrowserAbi, UnknownClassIdIsRefusedByEveryRead) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Gone"));
  const std::int64_t gone = ao_browser_class_id("B12Gone");
  ASSERT_GT(gone, 0);
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12Gone put: nil"));
  char buf[256];
  for (const std::int64_t id :
       {std::int64_t{0}, std::int64_t{-1}, gone, std::numeric_limits<std::int64_t>::max()}) {
    SCOPED_TRACE(id);
    EXPECT_EQ(-1, ao_browser_protocol_count(id, 0));
    EXPECT_EQ(AO_ERR, ao_browser_protocol_at(id, 0, 0, buf, 256));
    EXPECT_EQ(-1, ao_browser_selector_count(id, 0, "user"));
    EXPECT_EQ(AO_ERR, ao_browser_selector_at(id, 0, "user", 0, buf, 256));
    EXPECT_EQ(AO_ERR, ao_browser_source(id, 0, "printString", buf, 256));
    EXPECT_EQ(AO_ERR, ao_browser_class_definition(id, buf, 256));
    std::int64_t out = -7;
    EXPECT_EQ(AO_ERR, ao_browser_superclass(id, 0, &out, buf, 256));
    EXPECT_EQ(0, out);
    EXPECT_EQ(-1, ao_browser_subclass_count(id));
    out = -7;
    EXPECT_EQ(AO_ERR, ao_browser_subclass_at(id, 0, &out, buf, 256));
    EXPECT_EQ(0, out);
  }
}

// SPEC §3.10 クラス ID: a new session (a load, a boot) starts an empty table. IDs from before are
// unknown there, and the same class gets a new, larger ID.
TEST_F(BrowserAbi, ClassIdsDoNotSurviveBootOrImageLoad) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::int64_t before = ao_browser_class_id("Object");
  ASSERT_GT(before, 0);
  EXPECT_EQ(1, ao_browser_protocol_count(before, 0));
  const char* path = "browser-abi-ids.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path, &err)) << err.message;
  std::remove(path);
  EXPECT_EQ(-1, ao_browser_protocol_count(before, 0));
  const std::int64_t loaded = ao_browser_class_id("Object");
  EXPECT_GT(loaded, before);
  EXPECT_EQ(1, ao_browser_protocol_count(loaded, 0));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(-1, ao_browser_protocol_count(loaded, 0));
  const std::int64_t booted = ao_browser_class_id("Object");
  EXPECT_GT(booted, loaded);
  EXPECT_EQ(1, ao_browser_protocol_count(booted, 0));
}

// SPEC §3.10 クラス ID: two listed classes with one name (the second's name slot rewritten with
// instVarAt:put:, kClassSlotName is slot 3, index 4) are two rows in ID order, and each ID reads
// its own class.
TEST_F(BrowserAbi, SameNamedClassesAreReadApartById) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineRow("B12Twin", "B12-TwinA"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Twin", 0, "first\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, defineRow("B12TwinB", "B12-TwinB"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12TwinB", 0, "second\n  ^2\n", &err)) << err.message;
  const std::int64_t a = ao_browser_class_id("B12Twin");
  const std::int64_t b = ao_browser_class_id("B12TwinB");
  ASSERT_LT(a, b);
  ASSERT_EQ(AO_OK, doIt("B12TwinB instVarAt: 4 put: #B12Twin"));
  std::vector<std::pair<std::int64_t, std::string>> twins;
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    std::int64_t id = 0;
    char name[128];
    char category[128];
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, &id, name, 128, category, 128));
    if (std::strcmp(name, "B12Twin") == 0) {
      twins.emplace_back(id, category);
    }
  }
  ASSERT_EQ(2u, twins.size());
  EXPECT_EQ(std::make_pair(a, std::string("B12-TwinA")), twins[0]);
  EXPECT_EQ(std::make_pair(b, std::string("B12-TwinB")), twins[1]);
  char buf[256];
  ASSERT_EQ(AO_OK, ao_browser_selector_at(a, 0, "user", 0, buf, 256));
  EXPECT_STREQ("first", buf);
  ASSERT_EQ(AO_OK, ao_browser_selector_at(b, 0, "user", 0, buf, 256));
  EXPECT_STREQ("second", buf);
  ASSERT_EQ(AO_OK, ao_browser_class_definition(b, buf, 256));
  EXPECT_NE(std::string::npos, std::string(buf).find("category: 'B12-TwinB'"));
  ASSERT_EQ(AO_OK, ao_browser_source(b, 0, "second", buf, 256));
  EXPECT_STREQ("second\n  ^2\n", buf);
}

// SPEC §3.10: the superclass read answers the name and its ID (the class side names the instance
// class); the subclass reads answer the listed classes whose superclass slot is the class itself,
// by identity, in list order. A superclass that is not listed keeps its name with ID 0.
TEST_F(BrowserAbi, SuperclassAndSubclassesAnswerIds) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Sup"));
  ASSERT_EQ(AO_OK, defineRow("B12SubB", "B12-Id", "B12Sup"));
  ASSERT_EQ(AO_OK, defineRow("B12SubA", "B12-Id", "B12Sup"));
  const std::int64_t sup = ao_browser_class_id("B12Sup");
  const std::int64_t subA = ao_browser_class_id("B12SubA");
  const std::int64_t subB = ao_browser_class_id("B12SubB");
  char buf[128];
  std::int64_t id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(subA, 0, &id, buf, 128));
  EXPECT_STREQ("B12Sup", buf);
  EXPECT_EQ(sup, id);
  id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(subA, 1, &id, buf, 128));
  EXPECT_STREQ("B12Sup", buf);
  EXPECT_EQ(sup, id);
  id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(ao_browser_class_id("Object"), 0, &id, buf, 128));
  EXPECT_STREQ("", buf);
  EXPECT_EQ(0, id);
  ASSERT_EQ(2, ao_browser_subclass_count(sup));
  ASSERT_EQ(AO_OK, ao_browser_subclass_at(sup, 0, &id, buf, 128));
  EXPECT_STREQ("B12SubA", buf);
  EXPECT_EQ(subA, id);
  ASSERT_EQ(AO_OK, ao_browser_subclass_at(sup, 1, &id, buf, 128));
  EXPECT_STREQ("B12SubB", buf);
  EXPECT_EQ(subB, id);
  EXPECT_EQ(AO_OK, ao_browser_subclass_at(sup, 1, nullptr, buf, 128));
  // A class that only shares B12Sup's name is not B12Sup: its subclass is no subclass of B12Sup.
  ASSERT_EQ(AO_OK, defineRow("B12Other"));
  ASSERT_EQ(AO_OK, defineRow("B12OtherKid", "B12-Id", "B12Other"));
  ASSERT_EQ(AO_OK, doIt("B12Other instVarAt: 4 put: #B12Sup"));
  EXPECT_EQ(2, ao_browser_subclass_count(sup));
  // Unbound, B12Other leaves the list: its name stays, its ID is 0.
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12Other put: nil"));
  id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(ao_browser_class_id("B12OtherKid"), 0, &id, buf, 128));
  EXPECT_STREQ("B12Sup", buf);
  EXPECT_EQ(0, id);
}

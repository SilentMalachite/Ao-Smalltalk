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
  ASSERT_EQ(AO_OK, ao_browser_source("Object", 0, "printString", source, 256));
  EXPECT_NE(std::string(source).find("ao_Object_printString"), std::string::npos);
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
  EXPECT_EQ(AO_ERR, ao_browser_subclass_count("NoSuchClass"));
  ao_runtime_shutdown();
}

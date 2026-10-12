#include "ao_abi.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <string>

namespace {

class SourceNatives : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  void TearDown() override { ao_runtime_shutdown(); }
};

// Print it through the ABI: the printString, or "<rc: message>" when it fails.
std::string printIt(const char* source) {
  char out[4096] = "";
  AoSpan err{};
  const int rc = ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_PRINTIT, out,
                         static_cast<int>(sizeof(out)), &err);
  if (rc != AO_OK) {
    return "<" + std::to_string(rc) + ": " + err.message + ">";
  }
  return out;
}

void acceptClass(const char* source) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(source, &err)) << err.message;
}

void acceptMethod(const char* cls, int meta, const char* source) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method(cls, meta, source, &err)) << err.message;
}

// text as a Smalltalk string literal (each `'` doubled).
std::string literal(const std::string& text) {
  std::string lit = "'";
  for (const char c : text) {
    if (c == '\'') {
      lit += '\'';
    }
    lit += c;
  }
  return lit + "'";
}

TEST_F(SourceNatives, SourceCodeAtAnswersTheAcceptedSource) {
  acceptClass("Object subclass: #P17Probe\n  instanceVariableNames: ''\n"
              "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'P17-Test'");
  acceptMethod("P17Probe", 0, "answer\n\t^42");
  acceptMethod("P17Probe", 1, "make\n\t^self new");
  EXPECT_EQ("'answer\n\t^42'", printIt("P17Probe sourceCodeAt: #answer"));
  EXPECT_EQ("'answer\n\t^42'", printIt("P17Probe sourceCodeAt: 'answer'"));
  EXPECT_EQ("'make\n\t^self new'", printIt("P17Probe class sourceCodeAt: #make"));
}

TEST_F(SourceNatives, SourceCodeAtAnswersNilWithoutASource) {
  acceptClass("Object subclass: #P17Probe\n  instanceVariableNames: ''\n"
              "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'P17-Test'");
  acceptMethod("P17Probe", 0, "answer\n\t^42");
  // Not on this side, inherited, unknown, a NativeMethod, a non-selector argument.
  EXPECT_EQ("nil", printIt("P17Probe class sourceCodeAt: #answer"));
  EXPECT_EQ("nil", printIt("P17Probe sourceCodeAt: #printString"));
  EXPECT_EQ("nil", printIt("P17Probe sourceCodeAt: #neverDefinedAnywhere"));
  EXPECT_EQ("nil", printIt("Object sourceCodeAt: #printString"));
  EXPECT_EQ("nil", printIt("P17Probe sourceCodeAt: 3"));
}

TEST_F(SourceNatives, ClassDefinitionAnswersTheBrowserText) {
  // ao_accept_class reads chunks, so the category has no `!`; its `'` is doubled in the text.
  acceptClass("Object subclass: #P17Probe\n  instanceVariableNames: 'a b'\n"
              "  classVariableNames: 'K'\n  poolDictionaries: ''\n  category: 'P17-Te''st'");
  const std::int64_t id = ao_browser_class_id("P17Probe");
  ASSERT_NE(0, id);
  char browser[1024] = "";
  ASSERT_EQ(AO_OK, ao_browser_class_definition(id, browser, static_cast<int>(sizeof browser)));
  EXPECT_EQ("true", printIt(("P17Probe definition = " + literal(browser)).c_str()));
}

TEST_F(SourceNatives, DefinitionIsNotUnderstoodByAMetaclass) {
  EXPECT_EQ("<3: doesNotUnderstand: #definition>", printIt("Object class definition"));
}

}  // namespace

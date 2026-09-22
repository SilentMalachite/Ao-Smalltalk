#include "ao/Vendor.hpp"
#include <gtest/gtest.h>

TEST(VendorExtract, LastDefinitionWinsAndDropsDoIt) {
  const char* src =
      "!Object subclass: #Link\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Old'!\n"
      "!Link methodsFor: 'accessing'!\n"
      "nextLink\n"
      "  ^nil!\n"
      "!Transcript show: 'ignore'! \n"
      "!Object subclass: #Link\n"
      "  instanceVariableNames: 'nextLink'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Collections-Support'!\n"
      "!Link methodsFor: 'accessing'!\n"
      "nextLink\n"
      "  ^nextLink!\n"
      "!Collection variableSubclass: #Heap\n"
      "  instanceVariableNames: 'array tally'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Collections-Unordered'!\n";
  auto r = ao::extractVendor(src, {"Link", "Heap", "Bag"});
  ASSERT_EQ(1u, r.files.size());
  EXPECT_EQ("Link", r.files[0].className);
  EXPECT_EQ("Object", r.files[0].superName);
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("instanceVariableNames: 'nextLink'"));
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("^nextLink"));
  EXPECT_EQ(std::string::npos, r.files[0].chunkText.find("^nil"));
  EXPECT_EQ(std::string::npos, r.files[0].chunkText.find("Transcript"));
  ASSERT_EQ(1u, r.notes.size());
  EXPECT_NE(std::string::npos, r.notes[0].find("Heap"));
  EXPECT_NE(std::string::npos, r.notes[0].find("variableSubclass:"));
}

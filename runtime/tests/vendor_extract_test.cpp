#include "ao/Chunk.hpp"
#include "ao/Vendor.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

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

TEST(VendorOrigin, RecordsUrlCommitAndLicense) {
  const std::string root = AO_SOURCE_DIR;
  std::ifstream in(root + "/image/vendor/ORIGIN.md");
  ASSERT_TRUE(in.good());
  std::stringstream buf;
  buf << in.rdbuf();
  const std::string text = buf.str();
  EXPECT_NE(std::string::npos, text.find("https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev"));
  EXPECT_NE(std::string::npos, text.find("2628e261acaf4577084f2e788e973d0269b44fc5"));
  EXPECT_NE(std::string::npos, text.find("image/vendor/cuis/LICENSE"));
  std::ifstream lic(root + "/image/vendor/cuis/LICENSE");
  ASSERT_TRUE(lic.good());
  std::string first;
  std::getline(lic, first);
  EXPECT_NE(std::string::npos, first.find("MIT"));
}

TEST(VendorChunks, EachExtractedFileHasOneClassDef) {
  namespace fs = std::filesystem;
  const fs::path dir = fs::path(AO_SOURCE_DIR) / "image" / "vendor" / "cuis";
  std::vector<fs::path> files;
  std::error_code ec;
  if (fs::is_directory(dir, ec) && !ec) {
    for (const fs::directory_entry& entry : fs::directory_iterator(dir, ec)) {
      if (!ec && entry.is_regular_file(ec) && entry.path().extension() == ".st") {
        files.push_back(entry.path());
      }
    }
  }
  ASSERT_FALSE(files.empty()) << "no vendor .st files";
  for (const fs::path& path : files) {
    std::ifstream in(path, std::ios::binary);
    ASSERT_TRUE(in.good()) << path;
    std::stringstream buf;
    buf << in.rdbuf();
    std::vector<ao::compiler::CompileError> errs;
    const std::vector<ao::compiler::ChunkAction> acts = ao::compiler::parseChunks(buf.str(), errs);
    EXPECT_TRUE(errs.empty()) << path.filename().string();
    int classDefs = 0;
    for (const ao::compiler::ChunkAction& act : acts) {
      if (act.kind == ao::compiler::ChunkKind::ClassDef) {
        ++classDefs;
      }
    }
    EXPECT_EQ(1, classDefs) << path.filename().string();
  }
}

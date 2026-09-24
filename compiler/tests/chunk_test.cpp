#include "ao/Chunk.hpp"
#include <gtest/gtest.h>

TEST(Chunk, SplitsMethodsForAndProtocol) {
  const char* src =
      "!Foo methodsFor: 'accessing' stamp: 'ao 1/1/2026'!\n"
      "x\n"
      "  ^1!\n"
      "y: a\n"
      "  ^a!\n"
      "\n"
      "!Foo class methodsFor: 'instance creation'!\n"
      "new\n"
      "  ^super new!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(2u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[0].kind);
  EXPECT_EQ("Foo", acts[0].className);
  EXPECT_FALSE(acts[0].meta);
  EXPECT_EQ("accessing", acts[0].protocol);
  ASSERT_EQ(2u, acts[0].methods.size());
  EXPECT_TRUE(acts[1].meta);
}

TEST(Chunk, ClassDefinitionShape) {
  const char* src =
      "!Object subclass: #Foo\n"
      "  instanceVariableNames: 'a b'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Test-Foo'!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_EQ(1u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::ClassDef, acts[0].kind);
  EXPECT_EQ("Foo", acts[0].className);
  EXPECT_EQ("Object", acts[0].superName);
  EXPECT_EQ("a b", acts[0].instVars);
}

TEST(Chunk, BangInCharacterDoesNotSplit) {
  const char* src =
      "!Foo methodsFor: 't'!\n"
      "bang\n"
      "  ^$!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(1u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[0].kind);
  ASSERT_EQ(1u, acts[0].methods.size());
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("$!"));
}

// SPEC §3.8 チャンク形式: `$'` and `$"` are character literals. They open no string or comment,
// so the bang after them still ends the chunk.
TEST(Chunk, QuoteCharacterLiteralsOpenNoStringOrComment) {
  const char* src =
      "!Foo methodsFor: 'a'!\n"
      "quote\n"
      "  ^$'!\n"
      "isDq: c\n"
      "  ^c = $\"!\n"
      "two\n"
      "  ^2! !\n"
      "!Foo class methodsFor: 'b'!\n"
      "three\n"
      "  ^3! !\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(2u, acts.size());
  ASSERT_EQ(3u, acts[0].methods.size());
  EXPECT_EQ("quote\n  ^$'", acts[0].methods[0].source);
  EXPECT_EQ("isDq: c\n  ^c = $\"", acts[0].methods[1].source);
  EXPECT_EQ("two\n  ^2", acts[0].methods[2].source);
  EXPECT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[1].kind);
  EXPECT_TRUE(acts[1].meta);
  ASSERT_EQ(1u, acts[1].methods.size());
  EXPECT_EQ("three\n  ^3", acts[1].methods[0].source);
}

TEST(Chunk, SubclassSendInMethodStaysMethodsFor) {
  const char* src =
      "!Foo methodsFor: 't'!\n"
      "uses\n"
      "  self subclass: #Bar!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(1u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[0].kind);
  ASSERT_EQ(1u, acts[0].methods.size());
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("subclass:"));
}

TEST(Chunk, LineEndBangAfterBinaryStillTerminates) {
  const char* src =
      "!Foo methodsFor: 't'!\n"
      "plus\n"
      "  ^$+!\n"
      "other\n"
      "  ^1!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(1u, acts.size());
  EXPECT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[0].kind);
  ASSERT_EQ(2u, acts[0].methods.size());
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("^$+"));
  EXPECT_EQ(std::string::npos, acts[0].methods[0].source.find("other"));
  EXPECT_NE(std::string::npos, acts[0].methods[1].source.find("other"));
}

TEST(Chunk, BangSpaceBangEndsMethodButDoubleBangStaysLiteral) {
  const char* spaced =
      "!Foo methodsFor: 'accessing'!\n"
      "nextLink\n"
      "  ^nextLink! !\n"
      "nextLink: aLink\n"
      "  ^nextLink := aLink! !\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(spaced, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(1u, acts.size());
  ASSERT_EQ(2u, acts[0].methods.size());
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("^nextLink"));
  EXPECT_EQ(std::string::npos, acts[0].methods[0].source.find("nextLink:"));
  EXPECT_EQ(std::string::npos, acts[0].methods[0].source.find("!"));
  EXPECT_NE(std::string::npos, acts[0].methods[1].source.find("nextLink:"));

  const char* glued =
      "!Foo methodsFor: 't'!\n"
      "bang\n"
      "  ^1!!\n"
      "  + 2!\n";
  errs.clear();
  acts = ao::compiler::parseChunks(glued, errs);
  ASSERT_TRUE(errs.empty());
  ASSERT_EQ(1u, acts.size());
  ASSERT_EQ(1u, acts[0].methods.size());
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("1!"));
  EXPECT_EQ(std::string::npos, acts[0].methods[0].source.find("1!!"));
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("+ 2"));
}

TEST(Chunk, CommentStampQuoteDoesNotSwallowClassDef) {
  const char* src =
      "!Foo commentStamp: 'hist' prior: 0!\n"
      "Prose with a \" quote.\n"
      "End!\n"
      "!Object subclass: #Foo\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Test'!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  int classDefs = 0;
  std::string superName;
  for (const auto& act : acts) {
    if (act.kind == ao::compiler::ChunkKind::ClassDef && act.className == "Foo") {
      ++classDefs;
      superName = act.superName;
    }
  }
  EXPECT_EQ(1, classDefs);
  EXPECT_EQ("Object", superName);
}

TEST(Chunk, CommentStampApostropheDoesNotSwallowClassDef) {
  const char* src =
      "!Foo commentStamp: 'hist' prior: 0!\n"
      "They don't stop.\n"
      "End!\n"
      "!Object subclass: #Foo\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Test'!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  int classDefs = 0;
  for (const auto& act : acts) {
    if (act.kind == ao::compiler::ChunkKind::ClassDef && act.className == "Foo") {
      ++classDefs;
    }
  }
  EXPECT_EQ(1, classDefs);
}

// SPEC §3.10: `! !` ends a methodsFor: section. What follows without a header is flagged, so
// ao_accept_class can refuse it. File-in still reads it as before.
TEST(Chunk, ChunksAfterSectionEndAreFlagged) {
  const char* src =
      "!Px methodsFor: 'x'!\n"
      "foo\n"
      "  ^1! !\n"
      "3 + 4!\n"
      "Smalltalk halt!\n"
      "!Px methodsFor: 'y'!\n"
      "bar\n"
      "  ^2!\n"
      "baz\n"
      "  ^3! !\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_EQ(2u, acts.size());
  ASSERT_EQ(ao::compiler::ChunkKind::MethodsFor, acts[0].kind);
  ASSERT_FALSE(acts[0].methods.empty());
  EXPECT_NE(std::string::npos, acts[0].methods[0].source.find("^1"));
  EXPECT_FALSE(acts[0].methods[0].afterSectionEnd);
  for (std::size_t i = 1; i < acts[0].methods.size(); ++i) {
    SCOPED_TRACE(acts[0].methods[i].source);
    EXPECT_TRUE(acts[0].methods[i].afterSectionEnd);
  }
  EXPECT_EQ(3u, acts[0].methods.size());
  ASSERT_EQ(2u, acts[1].methods.size());
  EXPECT_FALSE(acts[1].methods[0].afterSectionEnd);
  EXPECT_FALSE(acts[1].methods[1].afterSectionEnd);

  // A header that ends with `! !` has an empty section.
  const char* empty = "!Px methodsFor: 'x'! !\nfoo\n  ^1!\n";
  acts = ao::compiler::parseChunks(empty, errs);
  ASSERT_EQ(1u, acts.size());
  ASSERT_EQ(1u, acts[0].methods.size());
  EXPECT_TRUE(acts[0].methods[0].afterSectionEnd);

  // `! !` on a line of its own is an empty chunk, which ends the section too.
  const char* ownLine = "!Px methodsFor: 'x'!\nfoo\n  ^1!\n! !\n3 + 4!\n";
  acts = ao::compiler::parseChunks(ownLine, errs);
  ASSERT_EQ(1u, acts.size());
  ASSERT_EQ(2u, acts[0].methods.size());
  EXPECT_FALSE(acts[0].methods[0].afterSectionEnd);
  EXPECT_TRUE(acts[0].methods[1].afterSectionEnd);
}

// SPEC §3.10: a class definition chunk is the definition message alone.
TEST(Chunk, SoleDefinitionIsTheMessageAlone) {
  struct Case {
    const char* src;
    bool sole;
  };
  const Case cases[] = {
      {"Object subclass: #Pq instanceVariableNames: 'a b' classVariableNames: '' "
       "poolDictionaries: '' category: 'P'",
       true},
      {"Object subclass: #Pq\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
       "  poolDictionaries: ''\n  category: 'P'.\n",
       true},
      {"Object subclass: #Pq \"a comment\" category: 'P'", true},
      {"Object subclass: #Pq", true},
      {"Object subclass: #Pq instanceVariableNames: '' classVariableNames: '' "
       "poolDictionaries: '' category: 'P'.\nSmalltalk at: #Zz put: 1",
       false},
      {"Object subclass: #Pq category: 'P'\nSmalltalk at: #Zz put: 1", false},
      {"Object subclass: #Pq category: 'P' at: #Zz put: 1", false},
      {"Object subclass: #Pq category: 'P' instanceVariableNames: ''", false},
      {"Object subclass: #Pq category: 'P'. .", false},
      {"Object subclass: #Pq category: 'P' , 'Q'", false},
      {"Smalltalk at: #Pq put: (Object subclass: #Pq)", false},
      {"Object subclass: #Pq; yourself", false},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.src);
    std::vector<ao::compiler::CompileError> errs;
    auto acts = ao::compiler::parseChunks(c.src, errs);
    ASSERT_EQ(1u, acts.size());
    ASSERT_EQ(ao::compiler::ChunkKind::ClassDef, acts[0].kind);
    EXPECT_EQ(c.sole, acts[0].soleDefinition);
  }
}

// SPEC §3.12: each action carries its own chunk's bytes, without the `!` delimiters.
TEST(Chunk, ActionsCarryTheirChunkSpan) {
  const std::string src =
      "!Object subclass: #Foo\n  category: 'T'!\n"
      "\n"
      "!Foo methodsFor: 'x'!\n"
      "one\n"
      "  ^1! !\n"
      "Foo initialize!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_EQ(2u, acts.size());
  auto text = [&](const ao::compiler::SourceSpan& s) {
    return src.substr(s.start, s.end - s.start);
  };
  EXPECT_EQ("Object subclass: #Foo\n  category: 'T'", text(acts[0].span));
  EXPECT_EQ("Foo methodsFor: 'x'", text(acts[1].span));
}

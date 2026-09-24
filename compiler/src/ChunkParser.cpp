#include "ao/Chunk.hpp"

#include "ao/Scanner.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace ao::compiler {
namespace {

bool isWs(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool isBlank(std::string_view s) {
  for (char c : s) {
    if (!isWs(c)) {
      return false;
    }
  }
  return true;
}

struct RawChunk {
  std::string text;
  SourceSpan span;
  // Ended by "! !", which closes a methodsFor: section.
  bool endsSection = false;
};

bool atLineEnd(std::string_view src, std::uint32_t p) {
  while (p < src.size() && (src[p] == ' ' || src[p] == '\t')) {
    p++;
  }
  return p >= src.size() || src[p] == '\n' || src[p] == '\r';
}

// Cuis ends a chunk with "! !" (bang, spaces or tabs, bang) at line end.
// That first bang is the terminator. "!!" with no space stays a literal bang.
bool bangSpaceBangAt(std::string_view src, std::uint32_t i, std::uint32_t& second) {
  std::uint32_t p = i + 1;
  if (p >= src.size() || (src[p] != ' ' && src[p] != '\t')) {
    return false;
  }
  while (p < src.size() && (src[p] == ' ' || src[p] == '\t')) {
    ++p;
  }
  if (p >= src.size() || src[p] != '!') {
    return false;
  }
  if (!atLineEnd(src, p + 1)) {
    return false;
  }
  second = p;
  return true;
}

// The character at i is the value of a `$x` literal: an odd run of `$` comes right before it.
bool isCharacterLiteral(std::string_view src, std::uint32_t i) {
  std::uint32_t dollars = 0;
  while (i > 0 && src[i - 1] == '$') {
    dollars++;
    i--;
  }
  return dollars % 2 == 1;
}

bool isLetter(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

// Prose uses ASCII apostrophes in words ("don't"). Those are not strings.
bool isProseApostrophe(std::string_view src, std::uint32_t i) {
  if (i == 0 || i + 1 >= src.size()) {
    return false;
  }
  return isLetter(src[i - 1]) && isLetter(src[i + 1]);
}

bool firstLineHas(std::string_view text, std::string_view needle) {
  std::size_t n = 0;
  while (n < text.size() && text[n] != '\n' && text[n] != '\r') {
    ++n;
  }
  return text.substr(0, n).find(needle) != std::string_view::npos;
}

// The chunk after a `commentStamp:` header is class-comment prose. A `"` in
// that prose must not open a code comment, or the next class definition is
// swallowed.
std::vector<RawChunk> splitChunks(std::string_view src) {
  std::vector<RawChunk> out;
  const auto n = static_cast<std::uint32_t>(src.size());
  std::uint32_t i = 0;
  bool proseNext = false;
  while (i < n) {
    while (i < n && isWs(src[i])) {
      i++;
    }
    if (i >= n) {
      break;
    }
    if (src[i] == '!') {
      i++;
    }
    const bool prose = proseNext;
    const std::uint32_t start = i;
    std::string text;
    bool inStr = false;
    bool inCmt = false;
    bool endsSection = false;
    while (i < n) {
      const char c = src[i];
      if (!inStr && !inCmt && c == '!') {
        if (!prose && isCharacterLiteral(src, i)) {
          text.push_back(c);
          i++;
          continue;
        }
        if (!prose && i + 1 < n && src[i + 1] == '!') {
          text.push_back('!');
          i += 2;
          continue;
        }
        std::uint32_t secondBang = 0;
        if (bangSpaceBangAt(src, i, secondBang)) {
          i = secondBang;
          endsSection = true;
          break;
        }
        if (atLineEnd(src, i + 1)) {
          break;
        }
        text.push_back(c);
        i++;
        continue;
      }
      // `!!` is one `!` in strings and comments too. A single `!` there does not end the chunk.
      if (!prose && (inStr || inCmt) && c == '!' && i + 1 < n && src[i + 1] == '!') {
        text.push_back('!');
        i += 2;
        continue;
      }
      // `$'` and `$"` are characters. They open no string or comment.
      if (!inStr && !inCmt && (c == '\'' || c == '"') && isCharacterLiteral(src, i)) {
        text.push_back(c);
        i++;
        continue;
      }
      if (!prose && !inStr && c == '"') {
        inCmt = !inCmt;
        text.push_back(c);
        i++;
        continue;
      }
      if (!inCmt && c == '\'') {
        if (prose && !inStr && isProseApostrophe(src, i)) {
          text.push_back(c);
          i++;
          continue;
        }
        if (inStr && i + 1 < n && src[i + 1] == '\'') {
          text.push_back('\'');
          text.push_back('\'');
          i += 2;
          continue;
        }
        inStr = !inStr;
        text.push_back(c);
        i++;
        continue;
      }
      text.push_back(c);
      i++;
    }
    const std::uint32_t end = i;
    if (i < n && src[i] == '!') {
      i++;
    }
    if (isBlank(text)) {
      // An empty chunk ends a methodsFor: section too (`! !` on a line of its own).
      if (!out.empty()) {
        out.back().endsSection = true;
      }
      continue;
    }
    proseNext = firstLineHas(text, "commentStamp:");
    RawChunk raw;
    raw.text = std::move(text);
    raw.span.start = start;
    raw.span.end = end;
    raw.endsSection = endsSection;
    out.push_back(std::move(raw));
  }
  return out;
}

enum class HeadKind { MethodsFor, ClassDef, Other };

std::string_view firstLine(std::string_view text) {
  std::size_t n = 0;
  while (n < text.size() && text[n] != '\n' && text[n] != '\r') {
    n++;
  }
  return text.substr(0, n);
}

HeadKind classify(std::string_view text) {
  Scanner s(firstLine(text));
  for (;;) {
    const Token t = s.next();
    if (t.kind == Tok::Eof || t.kind == Tok::Error) {
      return HeadKind::Other;
    }
    if (t.kind == Tok::Keyword && t.text == "methodsFor:") {
      return HeadKind::MethodsFor;
    }
    if (t.kind == Tok::Keyword && t.text == "subclass:") {
      return HeadKind::ClassDef;
    }
  }
}

std::string tokenValue(const Token& t) {
  if (t.kind == Tok::String || t.kind == Tok::Symbol || t.kind == Tok::Ident) {
    return t.text;
  }
  return {};
}

ChunkAction parseMethodsFor(std::string_view text) {
  ChunkAction a;
  a.kind = ChunkKind::MethodsFor;
  Scanner s(text);
  Token t = s.next();
  if (t.kind == Tok::Ident) {
    a.className = t.text;
    t = s.next();
  }
  if (t.kind == Tok::Ident && t.text == "class") {
    a.meta = true;
    t = s.next();
  }
  while (t.kind != Tok::Eof && t.kind != Tok::Error) {
    if (t.kind == Tok::Keyword && t.text == "methodsFor:") {
      const Token v = s.next();
      a.protocol = tokenValue(v);
      t = s.next();
      continue;
    }
    if (t.kind == Tok::Keyword && t.text == "stamp:") {
      const Token v = s.next();
      a.stamp = tokenValue(v);
      t = s.next();
      continue;
    }
    t = s.next();
  }
  return a;
}

// SPEC §3.10: the chunk is `Super subclass: #Name` followed by the other definition keywords in
// this order (any may be left out), each argument one token, and nothing after it but a period.
bool isSoleDefinition(std::string_view text) {
  static constexpr std::string_view kKeywords[] = {"subclass:", "instanceVariableNames:",
                                                   "classVariableNames:", "poolDictionaries:",
                                                   "category:"};
  Scanner s(text);
  if (s.next().kind != Tok::Ident) {
    return false;
  }
  Token t = s.next();
  std::size_t next = 0;
  while (t.kind == Tok::Keyword) {
    std::size_t k = next;
    while (k < std::size(kKeywords) && kKeywords[k] != t.text) {
      ++k;
    }
    if (k == std::size(kKeywords) || (next == 0 && k != 0)) {
      return false;
    }
    next = k + 1;
    const Token v = s.next();
    if (v.kind != Tok::String && v.kind != Tok::Symbol && v.kind != Tok::Ident) {
      return false;
    }
    t = s.next();
  }
  if (next == 0) {
    return false;
  }
  if (t.kind == Tok::Period) {
    t = s.next();
  }
  return t.kind == Tok::Eof;
}

ChunkAction parseClassDef(std::string_view text) {
  ChunkAction a;
  a.kind = ChunkKind::ClassDef;
  a.source = std::string(text);
  a.soleDefinition = isSoleDefinition(text);
  Scanner s(text);
  Token t = s.next();
  if (t.kind == Tok::Ident) {
    a.superName = t.text;
    t = s.next();
  }
  while (t.kind != Tok::Eof && t.kind != Tok::Error) {
    if (t.kind != Tok::Keyword) {
      t = s.next();
      continue;
    }
    const std::string kw = t.text;
    const Token v = s.next();
    const std::string val = tokenValue(v);
    if (kw == "subclass:") {
      a.className = val;
    } else if (kw == "instanceVariableNames:") {
      a.instVars = val;
    } else if (kw == "classVariableNames:") {
      a.classVars = val;
    } else if (kw == "poolDictionaries:") {
      a.pools = val;
    } else if (kw == "category:") {
      a.category = val;
    }
    t = s.next();
  }
  return a;
}

}  // namespace

std::vector<ChunkAction> parseChunks(std::string_view src, std::vector<CompileError>&) {
  std::vector<ChunkAction> acts;
  ChunkAction pending;
  // True while a methodsFor: section is open: from its header to the `! !` (or empty chunk) that
  // ends it.
  bool collecting = false;
  auto flush = [&] {
    if (collecting) {
      acts.push_back(std::move(pending));
      pending = ChunkAction{};
      collecting = false;
    }
  };
  for (const RawChunk& raw : splitChunks(src)) {
    const HeadKind hk = classify(raw.text);
    if (hk == HeadKind::MethodsFor) {
      flush();
      pending = parseMethodsFor(raw.text);
      pending.span = raw.span;
      collecting = true;
    } else if (hk == HeadKind::ClassDef) {
      flush();
      acts.push_back(parseClassDef(raw.text));
      acts.back().span = raw.span;
    } else if (collecting) {
      ChunkMethod m;
      m.source = std::string(raw.text);
      m.span = raw.span;
      pending.methods.push_back(std::move(m));
    } else {
      // SPEC §3.8: outside a section, also right after the `! !` that ended one, a chunk that is
      // not a header or a class definition is an expression.
      ChunkAction doit;
      doit.kind = ChunkKind::DoIt;
      doit.source = std::string(raw.text);
      doit.span = raw.span;
      acts.push_back(std::move(doit));
    }
    if (raw.endsSection) {
      flush();
    }
  }
  flush();
  return acts;
}

}  // namespace ao::compiler

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
  std::string_view text;
  SourceSpan span;
};

std::vector<RawChunk> splitChunks(std::string_view src) {
  std::vector<RawChunk> out;
  const auto n = static_cast<std::uint32_t>(src.size());
  std::uint32_t i = 0;
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
    const std::uint32_t start = i;
    bool inStr = false;
    bool inCmt = false;
    while (i < n) {
      const char c = src[i];
      if (!inStr && !inCmt && c == '!') {
        break;
      }
      if (!inStr && c == '"') {
        inCmt = !inCmt;
        i++;
        continue;
      }
      if (!inCmt && c == '\'') {
        if (inStr && i + 1 < n && src[i + 1] == '\'') {
          i += 2;
          continue;
        }
        inStr = !inStr;
        i++;
        continue;
      }
      i++;
    }
    const std::uint32_t end = i;
    if (i < n && src[i] == '!') {
      i++;
    }
    RawChunk raw;
    raw.text = src.substr(start, end - start);
    raw.span.start = start;
    raw.span.end = end;
    if (isBlank(raw.text)) {
      continue;
    }
    out.push_back(raw);
  }
  return out;
}

enum class HeadKind { MethodsFor, ClassDef, Other };

HeadKind classify(std::string_view text) {
  Scanner s(text);
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

ChunkAction parseClassDef(std::string_view text) {
  ChunkAction a;
  a.kind = ChunkKind::ClassDef;
  a.source = std::string(text);
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
      collecting = true;
      continue;
    }
    if (hk == HeadKind::ClassDef) {
      flush();
      acts.push_back(parseClassDef(raw.text));
      continue;
    }
    if (collecting) {
      ChunkMethod m;
      m.source = std::string(raw.text);
      m.span = raw.span;
      pending.methods.push_back(std::move(m));
      continue;
    }
    ChunkAction doit;
    doit.kind = ChunkKind::DoIt;
    doit.source = std::string(raw.text);
    acts.push_back(std::move(doit));
  }
  flush();
  return acts;
}

}  // namespace ao::compiler

#include "ao/Vendor.hpp"

#include "ao/Chunk.hpp"
#include "ao/Scanner.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ao {
namespace {

bool isWs(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool isIdentChar(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

bool isIdentStart(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

int allowIndex(const std::vector<std::string>& allow, std::string_view name) {
  for (std::size_t i = 0; i < allow.size(); ++i) {
    if (std::string_view(allow[i]) == name) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

struct KeptMethod {
  bool meta = false;
  std::string protocol;
  std::string key;
  std::string source;
};

struct KeptClass {
  bool hasDef = false;
  bool deferred = false;
  std::string superName;
  std::string instVars;
  std::string classVars;
  std::string pools;
  std::string category;
  std::vector<KeptMethod> methods;
};

// parseChunks classifies only the keyword `subclass:`. These shapes must be
// found in the source itself or they are dropped as DoIt chunks.
constexpr std::string_view kShapeKeywords[] = {
    "variableByteSubclass:",
    "variableWordSubclass:",
    "variableSubclass:",
    "weakSubclass:",
};

void scanShapes(std::string_view changes, const std::vector<std::string>& allow,
                std::vector<KeptClass>& slots, std::vector<std::string>& notes) {
  enum class Lex { Code, Comment, String };
  Lex lex = Lex::Code;
  for (std::size_t i = 0; i < changes.size();) {
    const char c = changes[i];
    if (lex == Lex::Comment) {
      if (c == '"') {
        lex = Lex::Code;
      }
      ++i;
      continue;
    }
    if (lex == Lex::String) {
      if (c == '\'') {
        if (i + 1 < changes.size() && changes[i + 1] == '\'') {
          i += 2;
          continue;
        }
        lex = Lex::Code;
      }
      ++i;
      continue;
    }
    if (c == '"') {
      lex = Lex::Comment;
      ++i;
      continue;
    }
    if (c == '\'') {
      lex = Lex::String;
      ++i;
      continue;
    }
    if (c == '$' && i + 1 < changes.size()) {
      i += 2;
      continue;
    }
    const bool boundary = i == 0 || (!isIdentChar(changes[i - 1]) && changes[i - 1] != '#');
    if (!boundary) {
      ++i;
      continue;
    }
    bool matched = false;
    for (const std::string_view kw : kShapeKeywords) {
      if (!changes.substr(i).starts_with(kw)) {
        continue;
      }
      matched = true;
      std::size_t j = i + kw.size();
      while (j < changes.size() && isWs(changes[j])) {
        ++j;
      }
      if (j < changes.size() && changes[j] == '#') {
        ++j;
        if (j < changes.size() && isIdentStart(changes[j])) {
          const std::size_t begin = j;
          ++j;
          while (j < changes.size() && isIdentChar(changes[j])) {
            ++j;
          }
          const std::string_view name = changes.substr(begin, j - begin);
          const int idx = allowIndex(allow, name);
          if (idx >= 0) {
            slots[static_cast<std::size_t>(idx)].deferred = true;
            std::string note;
            note.reserve(name.size() + 2 + kw.size());
            note.append(name);
            note.append(": ");
            note.append(kw);
            notes.push_back(std::move(note));
          }
        }
      }
      i += kw.size();
      break;
    }
    if (!matched) {
      ++i;
    }
  }
}

std::string_view firstNonEmptyLine(std::string_view source) {
  std::size_t i = 0;
  while (i < source.size()) {
    std::size_t end = i;
    while (end < source.size() && source[end] != '\n' && source[end] != '\r') {
      ++end;
    }
    std::size_t begin = i;
    while (begin < end && (source[begin] == ' ' || source[begin] == '\t')) {
      ++begin;
    }
    while (end > begin && (source[end - 1] == ' ' || source[end - 1] == '\t')) {
      --end;
    }
    if (begin < end) {
      return source.substr(begin, end - begin);
    }
    i = end;
    if (i < source.size() && source[i] == '\r') {
      ++i;
    }
    if (i < source.size() && source[i] == '\n') {
      ++i;
    }
  }
  return {};
}

// A DoIt placed after a method bang is still attached to the open MethodsFor.
// Method chunks start with a selector pattern; statements do not.
bool isMethodChunk(std::string_view source) {
  const std::string_view line = firstNonEmptyLine(source);
  if (line.empty()) {
    return false;
  }
  compiler::Scanner scanner(line);
  compiler::Token t = scanner.next();
  if (t.kind == compiler::Tok::Ident) {
    return scanner.next().kind == compiler::Tok::Eof;
  }
  if (t.kind == compiler::Tok::Binary) {
    if (scanner.next().kind != compiler::Tok::Ident) {
      return false;
    }
    return scanner.next().kind == compiler::Tok::Eof;
  }
  if (t.kind != compiler::Tok::Keyword) {
    return false;
  }
  while (t.kind == compiler::Tok::Keyword) {
    if (scanner.next().kind != compiler::Tok::Ident) {
      return false;
    }
    t = scanner.next();
  }
  return t.kind == compiler::Tok::Eof;
}

std::string firstLineKey(std::string_view source) {
  return std::string(firstNonEmptyLine(source));
}

std::string quoteSmalltalk(std::string_view text) {
  std::string out;
  out.push_back('\'');
  for (const char c : text) {
    if (c == '\'') {
      out.push_back('\'');
    }
    out.push_back(c);
  }
  out.push_back('\'');
  return out;
}

std::string trimTrailingWs(std::string text) {
  while (!text.empty()) {
    const char c = text.back();
    if (c != '\n' && c != '\r' && c != ' ' && c != '\t') {
      break;
    }
    text.pop_back();
  }
  return text;
}

void keepMethod(KeptClass& cls, bool meta, const std::string& protocol, const std::string& source) {
  const std::string key = firstLineKey(source);
  for (KeptMethod& method : cls.methods) {
    if (method.meta == meta && method.key == key) {
      method.protocol = protocol;
      method.source = source;
      return;
    }
  }
  KeptMethod method;
  method.meta = meta;
  method.protocol = protocol;
  method.key = key;
  method.source = source;
  cls.methods.push_back(std::move(method));
}

std::string render(const std::string& name, const KeptClass& cls) {
  std::string text;
  text += '!';
  text += cls.superName;
  text += " subclass: #";
  text += name;
  text += "\n  instanceVariableNames: ";
  text += quoteSmalltalk(cls.instVars);
  text += "\n  classVariableNames: ";
  text += quoteSmalltalk(cls.classVars);
  text += "\n  poolDictionaries: ";
  text += quoteSmalltalk(cls.pools);
  text += "\n  category: ";
  text += quoteSmalltalk(cls.category);
  text += "!\n";

  std::size_t i = 0;
  while (i < cls.methods.size()) {
    const bool meta = cls.methods[i].meta;
    const std::string& protocol = cls.methods[i].protocol;
    text += '!';
    text += name;
    if (meta) {
      text += " class";
    }
    text += " methodsFor: ";
    text += quoteSmalltalk(protocol);
    text += "!\n";
    while (i < cls.methods.size() && cls.methods[i].meta == meta &&
           cls.methods[i].protocol == protocol) {
      text += trimTrailingWs(cls.methods[i].source);
      text += "!\n";
      ++i;
    }
  }
  return text;
}

}  // namespace

VendorExtractResult extractVendor(std::string_view changes, const std::vector<std::string>& allowlist) {
  VendorExtractResult result;
  std::vector<KeptClass> slots(allowlist.size());
  scanShapes(changes, allowlist, slots, result.notes);

  std::vector<compiler::CompileError> errors;
  const std::vector<compiler::ChunkAction> actions = compiler::parseChunks(changes, errors);
  for (const compiler::ChunkAction& action : actions) {
    if (action.kind == compiler::ChunkKind::ClassDef) {
      const int idx = allowIndex(allowlist, action.className);
      if (idx < 0 || slots[static_cast<std::size_t>(idx)].deferred) {
        continue;
      }
      KeptClass& cls = slots[static_cast<std::size_t>(idx)];
      cls.hasDef = true;
      cls.superName = action.superName;
      cls.instVars = action.instVars;
      cls.classVars = action.classVars;
      cls.pools = action.pools;
      cls.category = action.category;
      continue;
    }
    if (action.kind != compiler::ChunkKind::MethodsFor) {
      continue;
    }
    const int idx = allowIndex(allowlist, action.className);
    if (idx < 0 || slots[static_cast<std::size_t>(idx)].deferred) {
      continue;
    }
    KeptClass& cls = slots[static_cast<std::size_t>(idx)];
    for (const compiler::ChunkMethod& method : action.methods) {
      if (!isMethodChunk(method.source)) {
        continue;
      }
      keepMethod(cls, action.meta, action.protocol, method.source);
    }
  }

  for (std::size_t i = 0; i < allowlist.size(); ++i) {
    const KeptClass& cls = slots[i];
    if (cls.deferred || !cls.hasDef) {
      continue;
    }
    VendorClassFile file;
    file.className = allowlist[i];
    file.superName = cls.superName;
    file.chunkText = render(allowlist[i], cls);
    result.files.push_back(std::move(file));
  }
  return result;
}

}  // namespace ao

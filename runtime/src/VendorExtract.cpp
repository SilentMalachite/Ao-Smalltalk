#include "ao/Vendor.hpp"

#include "ao/Chunk.hpp"
#include "ao/Scanner.hpp"

#include <set>
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

// Bootstrap.cpp kDefs names. WellKnown is not referenced from this file.
bool isCatalogName(std::string_view name) {
  constexpr std::string_view kCatalog[] = {
      "Object",
      "Behavior",
      "ClassDescription",
      "Class",
      "Metaclass",
      "UndefinedObject",
      "Boolean",
      "True",
      "False",
      "Magnitude",
      "Number",
      "Integer",
      "SmallInteger",
      "LargePositiveInteger",
      "LargeNegativeInteger",
      "Float",
      "Fraction",
      "Character",
      "Collection",
      "SequenceableCollection",
      "ArrayedCollection",
      "Array",
      "ByteArray",
      "String",
      "Symbol",
      "Interval",
      "Dictionary",
      "IdentityDictionary",
      "Set",
      "IdentitySet",
      "OrderedCollection",
      "Association",
      "Bag",
      "LinkedList",
      "MappedCollection",
      "CompiledMethod",
      "NativeMethod",
      "Message",
      "MethodDictionary",
      "MethodContext",
      "BlockContext",
      "Process",
      "ProcessorScheduler",
      "Semaphore",
      "SharedQueue",
      "Point",
      "Rectangle",
      "Stream",
      "PositionableStream",
      "ReadStream",
      "WriteStream",
      "ReadWriteStream",
      "Transcript",
      "SmalltalkImage",
      "Date",
      "Time",
  };
  for (const std::string_view catalog : kCatalog) {
    if (catalog == name) {
      return true;
    }
  }
  return false;
}

// Word characters are alphanumeric, so Form does not match Format.
bool isHostWordChar(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

constexpr std::string_view kHostWords[] = {
    "StandardFileStream",
    "FilePlugin",
    "DirectoryEntry",
    "Morph",
    "DisplayScreen",
    "Form",
    "BitBlt",
    "Canvas",
    "World",
    "HandMorph",
};

bool containsHostWord(std::string_view text) {
  for (const std::string_view word : kHostWords) {
    std::size_t pos = 0;
    while (pos < text.size()) {
      const std::size_t found = text.find(word, pos);
      if (found == std::string_view::npos) {
        break;
      }
      const bool left = found == 0 || !isHostWordChar(text[found - 1]);
      const std::size_t after = found + word.size();
      const bool right = after >= text.size() || !isHostWordChar(text[after]);
      if (left && right) {
        return true;
      }
      pos = found + 1;
    }
  }
  return false;
}

struct MethodParts {
  std::string_view pattern;
  std::string_view body;
};

MethodParts splitMethod(std::string_view source) {
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
    std::size_t trim = end;
    while (trim > begin && (source[trim - 1] == ' ' || source[trim - 1] == '\t')) {
      --trim;
    }
    std::size_t next = end;
    if (next < source.size() && source[next] == '\r') {
      ++next;
    }
    if (next < source.size() && source[next] == '\n') {
      ++next;
    }
    if (begin < trim) {
      MethodParts parts;
      parts.pattern = source.substr(begin, trim - begin);
      parts.body = source.substr(next);
      return parts;
    }
    i = next;
  }
  return {};
}

std::string withoutComments(std::string_view body) {
  std::string out;
  bool comment = false;
  bool string = false;
  for (std::size_t i = 0; i < body.size(); ++i) {
    const char c = body[i];
    if (string) {
      out.push_back(c);
      if (c == '\'') {
        if (i + 1 < body.size() && body[i + 1] == '\'') {
          out.push_back(body[++i]);
        } else {
          string = false;
        }
      }
      continue;
    }
    if (comment) {
      if (c == '"') {
        comment = false;
      }
      continue;
    }
    if (c == '"') {
      comment = true;
      continue;
    }
    if (c == '\'') {
      string = true;
      out.push_back(c);
      continue;
    }
    out.push_back(c);
  }
  return out;
}

// <primitive: N> alone, or with the fallback ^self primitiveFailed, is not a host call.
bool isPrimitiveOnly(std::string_view body) {
  const std::string code = withoutComments(body);
  std::size_t i = 0;
  const auto skipWs = [&] {
    while (i < code.size() && isWs(code[i])) {
      ++i;
    }
  };
  skipWs();
  constexpr std::string_view kPrim = "<primitive:";
  if (i >= code.size() || code.compare(i, kPrim.size(), kPrim) != 0) {
    return false;
  }
  i += kPrim.size();
  while (i < code.size() && code[i] != '>') {
    ++i;
  }
  if (i >= code.size()) {
    return false;
  }
  ++i;
  if (i < code.size() && code[i] == '.') {
    ++i;
  }
  skipWs();
  if (i >= code.size()) {
    return true;
  }
  if (code[i] == '^') {
    ++i;
    skipWs();
  }
  constexpr std::string_view kSelf = "self";
  constexpr std::string_view kFailed = "primitiveFailed";
  if (i >= code.size() || code.compare(i, kSelf.size(), kSelf) != 0) {
    return false;
  }
  i += kSelf.size();
  if (i >= code.size() || !isWs(code[i])) {
    return false;
  }
  skipWs();
  if (i >= code.size() || code.compare(i, kFailed.size(), kFailed) != 0) {
    return false;
  }
  i += kFailed.size();
  if (i < code.size() && isHostWordChar(code[i])) {
    return false;
  }
  if (i < code.size() && code[i] == '.') {
    ++i;
  }
  skipWs();
  return i >= code.size();
}

struct HostMethod {
  std::string protocol;
  std::string source;
};

HostMethod hostPatch(const std::string& protocol, const std::string& source) {
  const MethodParts parts = splitMethod(source);
  if (isPrimitiveOnly(parts.body) || !containsHostWord(parts.body)) {
    return HostMethod{protocol, source};
  }
  HostMethod out;
  out.protocol = "ao-host-patch";
  out.source.assign(parts.pattern.begin(), parts.pattern.end());
  out.source += "\n  ^self error: 'host file stream is not connected'";
  return out;
}

void keepMethod(KeptClass& cls, bool meta, const std::string& protocol, const std::string& source) {
  const std::string key = firstLineKey(source);
  const HostMethod host = hostPatch(protocol, source);
  for (KeptMethod& method : cls.methods) {
    if (method.meta == meta && method.key == key) {
      method.protocol = host.protocol;
      method.source = host.source;
      return;
    }
  }
  KeptMethod method;
  method.meta = meta;
  method.protocol = host.protocol;
  method.key = key;
  method.source = host.source;
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

enum class SlotKind { Idle, Active, Dropped };

int findActive(const std::vector<std::string>& allowlist, const std::vector<SlotKind>& kind,
               std::string_view name) {
  for (std::size_t i = 0; i < allowlist.size(); ++i) {
    if (kind[i] == SlotKind::Active && allowlist[i] == name) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

void noteMissingSuper(const std::vector<std::string>& allowlist, const std::vector<KeptClass>& slots,
                      std::vector<SlotKind>& kind, std::vector<std::string>& notes, std::size_t i) {
  kind[i] = SlotKind::Dropped;
  std::string note;
  note.append(allowlist[i]);
  note.append(": missing superclass ");
  note.append(slots[i].superName);
  notes.push_back(std::move(note));
}

void dropMissingSupers(const std::vector<std::string>& allowlist, const std::vector<KeptClass>& slots,
                       std::vector<SlotKind>& kind, std::vector<std::string>& notes) {
  bool changed = true;
  while (changed) {
    changed = false;
    for (std::size_t i = 0; i < allowlist.size(); ++i) {
      if (kind[i] != SlotKind::Active) {
        continue;
      }
      const std::string& super = slots[i].superName;
      if (isCatalogName(super) || findActive(allowlist, kind, super) >= 0) {
        continue;
      }
      noteMissingSuper(allowlist, slots, kind, notes, i);
      changed = true;
    }
  }
}

bool reachesOwnSuper(const std::vector<std::string>& allowlist, const std::vector<KeptClass>& slots,
                     const std::vector<SlotKind>& kind, std::size_t start) {
  std::size_t cur = start;
  for (std::size_t step = 0; step < allowlist.size(); ++step) {
    const int next = findActive(allowlist, kind, slots[cur].superName);
    if (next < 0) {
      return false;
    }
    if (static_cast<std::size_t>(next) == start) {
      return true;
    }
    cur = static_cast<std::size_t>(next);
  }
  return false;
}

void dropCycles(const std::vector<std::string>& allowlist, const std::vector<KeptClass>& slots,
                std::vector<SlotKind>& kind, std::vector<std::string>& notes) {
  std::vector<std::size_t> cyclic;
  for (std::size_t i = 0; i < allowlist.size(); ++i) {
    if (kind[i] == SlotKind::Active && reachesOwnSuper(allowlist, slots, kind, i)) {
      cyclic.push_back(i);
    }
  }
  for (const std::size_t i : cyclic) {
    kind[i] = SlotKind::Dropped;
    std::string note;
    note.append(allowlist[i]);
    note.append(": superclass cycle");
    notes.push_back(std::move(note));
  }
}

std::vector<std::size_t> loadOrder(const std::vector<std::string>& allowlist,
                                   const std::vector<KeptClass>& slots,
                                   std::vector<std::string>& notes) {
  std::vector<SlotKind> kind(allowlist.size(), SlotKind::Idle);
  for (std::size_t i = 0; i < allowlist.size(); ++i) {
    if (!slots[i].deferred && slots[i].hasDef) {
      kind[i] = SlotKind::Active;
    }
  }
  dropMissingSupers(allowlist, slots, kind, notes);
  dropCycles(allowlist, slots, kind, notes);
  dropMissingSupers(allowlist, slots, kind, notes);

  const std::size_t n = allowlist.size();
  std::vector<int> indegree(n, 0);
  std::vector<std::vector<std::size_t>> children(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (kind[i] != SlotKind::Active) {
      continue;
    }
    const int super = findActive(allowlist, kind, slots[i].superName);
    if (super < 0) {
      continue;
    }
    const auto parent = static_cast<std::size_t>(super);
    if (parent == i) {
      continue;
    }
    indegree[i] += 1;
    children[parent].push_back(i);
  }

  std::set<std::size_t> ready;
  for (std::size_t i = 0; i < n; ++i) {
    if (kind[i] == SlotKind::Active && indegree[i] == 0) {
      ready.insert(i);
    }
  }
  std::vector<std::size_t> order;
  std::vector<bool> taken(n, false);
  while (!ready.empty()) {
    const std::size_t i = *ready.begin();
    ready.erase(ready.begin());
    taken[i] = true;
    order.push_back(i);
    for (const std::size_t child : children[i]) {
      indegree[child] -= 1;
      if (indegree[child] == 0) {
        ready.insert(child);
      }
    }
  }
  for (std::size_t i = 0; i < n; ++i) {
    if (kind[i] == SlotKind::Active && !taken[i]) {
      std::string note;
      note.append(allowlist[i]);
      note.append(": superclass cycle");
      notes.push_back(std::move(note));
    }
  }
  return order;
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

  for (const std::size_t i : loadOrder(allowlist, slots, result.notes)) {
    const KeptClass& cls = slots[i];
    VendorClassFile file;
    file.className = allowlist[i];
    file.superName = cls.superName;
    file.chunkText = render(allowlist[i], cls);
    result.files.push_back(std::move(file));
  }
  return result;
}

}  // namespace ao

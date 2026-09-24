#pragma once

#include "ao/Span.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace ao::compiler {

enum class ChunkKind { MethodsFor, ClassDef, DoIt };

struct ChunkMethod {
  std::string source;
  SourceSpan span;
  // The chunk came after the `! !` that ended its methodsFor: section. File-in still installs it;
  // ao_accept_class refuses it as an expression (SPEC §3.10).
  bool afterSectionEnd = false;
};

struct ChunkAction {
  ChunkKind kind = ChunkKind::DoIt;
  std::string className;
  std::string superName;
  bool meta = false;
  std::string protocol;
  std::string stamp;
  std::string instVars;
  std::string classVars;
  std::string pools;
  std::string category;
  std::string source;
  std::vector<ChunkMethod> methods;
  // The bytes of the chunk itself, without its `!` delimiters: the class definition, the
  // methodsFor: header, or the expression. A chunk-level file-in error points here (SPEC §3.12).
  SourceSpan span;
  // ClassDef only: the chunk is the class-definition message and nothing else (SPEC §3.10).
  bool soleDefinition = false;
};

std::vector<ChunkAction> parseChunks(std::string_view src, std::vector<CompileError>& errors);

}  // namespace ao::compiler

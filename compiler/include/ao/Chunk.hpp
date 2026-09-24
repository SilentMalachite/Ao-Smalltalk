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
  // Offsets in source just after each `!` that stands for `!!` in the file, in ascending order.
  // The file has one byte more there than source (see fileSpan).
  std::vector<std::uint32_t> undoubled;
};

// Where a span of method.source lies in the file the chunk came from. An error in the method
// points into the file body (SPEC §3.12), also after a `!!` read as `!`.
SourceSpan fileSpan(const ChunkMethod& method, SourceSpan inSource);

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

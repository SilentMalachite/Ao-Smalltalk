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
};

std::vector<ChunkAction> parseChunks(std::string_view src, std::vector<CompileError>& errors);

}  // namespace ao::compiler

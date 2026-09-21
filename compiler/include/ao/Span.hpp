#pragma once

#include <cstdint>
#include <string>

namespace ao::compiler {

struct SourceSpan {
  std::uint32_t start = 0;
  std::uint32_t end = 0;
};

struct CompileError {
  SourceSpan span;
  std::string message;
};

}  // namespace ao::compiler

#pragma once

#include "ao/Ast.hpp"
#include "ao/Span.hpp"

#include <string_view>

namespace ao::compiler {

struct ParseResult {
  bool ok = false;
  Ast method;
  CompileError error;
};

ParseResult parseMethod(std::string_view source);

}  // namespace ao::compiler

#pragma once

#include "ao/Ast.hpp"
#include "ao/MethodImage.hpp"

#include <string>

namespace ao::compiler {

void codegen(const Ast& method, const CompileEnv& env, CompileResult& out);
std::string disassemble(const MethodImage& image);

}  // namespace ao::compiler

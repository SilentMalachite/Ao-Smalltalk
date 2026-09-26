#include "ao/Compiler.hpp"

#include "ao/Codegen.hpp"
#include "ao/Parser.hpp"

namespace ao::compiler {

const char* version() { return "1.0.0"; }

CompileResult compileMethod(std::string_view source, const CompileEnv& env) {
  CompileResult r;
  ParseResult parsed = parseMethod(source);
  if (!parsed.ok) {
    r.error = std::move(parsed.error);
    return r;
  }
  codegen(parsed.method, env, r);
  return r;
}

}  // namespace ao::compiler

#pragma once

#include "ao/Span.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ao::compiler {

struct Ast {
  enum class Kind {
    Method,
    Sequence,
    Return,
    Assign,
    Send,
    Variable,
    Literal,
    Cascade,
    Block,
    Primitive,
  };

  Kind kind = Kind::Method;
  SourceSpan span;
  std::string name;
  std::uint8_t argc = 0;
  bool isSuper = false;
  std::int64_t intValue = 0;
  double floatValue = 0;
  std::string text;
  bool isFloat = false;
  // A number Literal outside int64: Token::largeInt with its sign, [-][<radix>r]<digits>.
  std::string largeInt;
  // SPEC §3.8: how many nesting levels this node holds (the parser refuses more than 256), so the
  // passes that recurse over the tree stay within a bounded depth.
  std::uint16_t nesting = 0;
  std::vector<Ast> kids;
  std::vector<std::string> params;
  std::vector<std::string> temps;
};

}  // namespace ao::compiler

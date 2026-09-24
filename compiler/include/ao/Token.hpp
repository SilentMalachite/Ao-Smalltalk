#pragma once

#include "ao/Span.hpp"

#include <cstdint>
#include <string>

namespace ao::compiler {

enum class Tok {
  Eof,
  Error,
  Ident,
  Keyword,
  Binary,
  Number,
  String,
  Symbol,
  Character,
  Assign,
  Caret,
  Semicolon,
  Period,
  Colon,
  LParen,
  RParen,
  LBracket,
  RBracket,
  HashLParen,
  HashLBracket,
  LBrace,
  RBrace,
};

struct Token {
  Tok kind = Tok::Eof;
  SourceSpan span;
  std::string text;
  double number = 0;
  std::int64_t intValue = 0;
  bool isFloat = false;
  // An Integer outside int64 (SPEC §3.8): its value as [<radix>r]<digits>, the digits upper case
  // without leading zeros. intValue is 0 then.
  std::string largeInt;
};

}  // namespace ao::compiler

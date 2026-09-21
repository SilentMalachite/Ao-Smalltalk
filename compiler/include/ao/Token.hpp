#pragma once

#include "ao/Span.hpp"

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
  bool isFloat = false;
};

}  // namespace ao::compiler

#pragma once
#include "ao/Token.hpp"
#include <string_view>

namespace ao::compiler {
class Scanner {
 public:
  explicit Scanner(std::string_view src);
  Token next();
 private:
  std::string_view src_;
  std::uint32_t i_ = 0;
  void skipTrivia();
  Token make(Tok k, std::uint32_t start, std::string text = {});
  Token lexNumber(std::uint32_t start);
  Token lexIdentOrKeyword(std::uint32_t start);
  Token lexString(std::uint32_t start);
  Token lexSymbol(std::uint32_t start);
  Token lexCharacter(std::uint32_t start);
  Token lexBinary(std::uint32_t start);
};
}

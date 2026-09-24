#include "ao/Scanner.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>

namespace ao::compiler {
namespace {

bool isSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool isAlpha(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool isAlnum(char c) {
  return isAlpha(c) || isDigit(c);
}

bool isBinaryChar(char c) {
  switch (c) {
    case '+':
    case '-':
    case '*':
    case '/':
    case '\\':
    case '~':
    case '<':
    case '>':
    case '=':
    case '@':
    case '%':
    case '|':
    case '&':
    case '?':
    case '!':
    case ',':
      return true;
    default:
      return false;
  }
}

int digitValue(char c, int radix) {
  int v = -1;
  if (c >= '0' && c <= '9') {
    v = c - '0';
  } else if (c >= 'A' && c <= 'Z') {
    v = 10 + (c - 'A');
  } else if (c >= 'a' && c <= 'z') {
    v = 10 + (c - 'a');
  }
  if (v < 0 || v >= radix) {
    return -1;
  }
  return v;
}

std::uint32_t utf8Len(unsigned char lead) {
  if (lead < 0x80) {
    return 1;
  }
  if ((lead & 0xE0) == 0xC0) {
    return 2;
  }
  if ((lead & 0xF0) == 0xE0) {
    return 3;
  }
  if ((lead & 0xF8) == 0xF0) {
    return 4;
  }
  return 1;
}

}  // namespace

Scanner::Scanner(std::string_view src) : src_(src) {}

Token Scanner::make(Tok k, std::uint32_t start, std::string text) {
  Token t;
  t.kind = k;
  t.span.start = start;
  t.span.end = i_;
  t.text = std::move(text);
  return t;
}

void Scanner::skipTrivia() {
  while (i_ < src_.size()) {
    const char c = src_[i_];
    if (isSpace(c)) {
      i_++;
      continue;
    }
    if (c != '"') {
      return;
    }
    const std::uint32_t start = i_;
    i_++;
    while (i_ < src_.size() && src_[i_] != '"') {
      i_++;
    }
    if (i_ >= src_.size()) {
      i_ = start;
      return;
    }
    i_++;
  }
}

Token Scanner::lexNumber(std::uint32_t start) {
  auto at = [&](std::uint32_t off) -> char {
    const std::uint32_t p = i_ + off;
    return p < src_.size() ? src_[p] : '\0';
  };

  auto addDigit = [](std::int64_t& acc, int radix, int digit) -> bool {
    const std::int64_t r = radix;
    const std::int64_t d = digit;
    if (acc > (std::numeric_limits<std::int64_t>::max() - d) / r) {
      return false;
    }
    acc = acc * r + d;
    return true;
  };

  std::int64_t intAcc = 0;
  double dblAcc = 0;
  bool overflow = false;
  while (isDigit(at(0))) {
    const int d = at(0) - '0';
    if (!overflow && !addDigit(intAcc, 10, d)) {
      overflow = true;
    }
    dblAcc = dblAcc * 10.0 + static_cast<double>(d);
    i_++;
  }

  int radix = 10;
  if ((at(0) == 'r' || at(0) == 'R') && !overflow && intAcc >= 2 && intAcc <= 36 &&
      digitValue(at(1), static_cast<int>(intAcc)) >= 0) {
    radix = static_cast<int>(intAcc);
    i_++;
    intAcc = 0;
    dblAcc = 0;
    overflow = false;
    int dv = 0;
    while ((dv = digitValue(at(0), radix)) >= 0) {
      if (!overflow && !addDigit(intAcc, radix, dv)) {
        overflow = true;
      }
      dblAcc = dblAcc * static_cast<double>(radix) + static_cast<double>(dv);
      i_++;
    }
  }

  bool isFloat = false;
  double value = overflow ? dblAcc : static_cast<double>(intAcc);
  if (at(0) == '.' && digitValue(at(1), radix) >= 0) {
    isFloat = true;
    i_++;
    double place = 1.0 / static_cast<double>(radix);
    int dv = 0;
    while ((dv = digitValue(at(0), radix)) >= 0) {
      value += static_cast<double>(dv) * place;
      place /= static_cast<double>(radix);
      i_++;
    }
  }

  const char expMark = at(0);
  if (expMark == 'e' || expMark == 'E' || expMark == 'd' || expMark == 'D') {
    std::uint32_t digitsOff = 1;
    if (at(1) == '+' || at(1) == '-') {
      digitsOff = 2;
    }
    if (isDigit(at(digitsOff))) {
      isFloat = true;
      i_++;
      int expSign = 1;
      if (at(0) == '+') {
        i_++;
      } else if (at(0) == '-') {
        expSign = -1;
        i_++;
      }
      std::int64_t exp = 0;
      bool expOverflow = false;
      while (isDigit(at(0))) {
        const int d = at(0) - '0';
        if (!expOverflow) {
          if (exp > (std::numeric_limits<std::int64_t>::max() - d) / 10) {
            expOverflow = true;
            exp = std::numeric_limits<std::int64_t>::max();
          } else {
            exp = exp * 10 + d;
          }
        }
        i_++;
      }
      if (expOverflow || exp > 400) {
        value = expSign < 0 ? 0.0 : std::numeric_limits<double>::infinity();
      } else {
        value *= std::pow(10.0, static_cast<double>(expSign) * static_cast<double>(exp));
      }
    }
  }

  Token t = make(Tok::Number, start, std::string(src_.substr(start, i_ - start)));
  t.number = value;
  t.isFloat = isFloat;
  if (!isFloat && !overflow) {
    t.intValue = intAcc;
  }
  return t;
}

Token Scanner::lexIdentOrKeyword(std::uint32_t start) {
  i_++;
  while (i_ < src_.size() && isAlnum(src_[i_])) {
    i_++;
  }
  if (i_ < src_.size() && src_[i_] == ':') {
    const bool assign = i_ + 1 < src_.size() && src_[i_ + 1] == '=';
    if (!assign) {
      i_++;
      return make(Tok::Keyword, start, std::string(src_.substr(start, i_ - start)));
    }
  }
  return make(Tok::Ident, start, std::string(src_.substr(start, i_ - start)));
}

Token Scanner::lexString(std::uint32_t start) {
  i_++;
  std::string text;
  while (i_ < src_.size()) {
    if (src_[i_] == '\'') {
      if (i_ + 1 < src_.size() && src_[i_ + 1] == '\'') {
        text.push_back('\'');
        i_ += 2;
        continue;
      }
      i_++;
      return make(Tok::String, start, std::move(text));
    }
    text.push_back(src_[i_]);
    i_++;
  }
  return make(Tok::Error, start);
}

Token Scanner::lexSymbol(std::uint32_t start) {
  i_++;
  if (i_ >= src_.size()) {
    return make(Tok::Error, start);
  }
  const char c = src_[i_];
  if (c == '(') {
    i_++;
    return make(Tok::HashLParen, start);
  }
  if (c == '[') {
    i_++;
    return make(Tok::HashLBracket, start);
  }
  if (c == '\'') {
    Token t = lexString(i_);
    t.kind = t.kind == Tok::Error ? Tok::Error : Tok::Symbol;
    t.span.start = start;
    return t;
  }
  if (isAlpha(c)) {
    std::string text;
    while (i_ < src_.size() && isAlpha(src_[i_])) {
      const std::uint32_t part = i_;
      i_++;
      while (i_ < src_.size() && isAlnum(src_[i_])) {
        i_++;
      }
      if (i_ < src_.size() && src_[i_] == ':') {
        i_++;
        text.append(src_.substr(part, i_ - part));
        continue;
      }
      text.append(src_.substr(part, i_ - part));
      break;
    }
    return make(Tok::Symbol, start, std::move(text));
  }
  if (isBinaryChar(c)) {
    Token bin = lexBinary(i_);
    return make(Tok::Symbol, start, std::move(bin.text));
  }
  return make(Tok::Error, start);
}

Token Scanner::lexCharacter(std::uint32_t start) {
  i_++;
  if (i_ >= src_.size()) {
    return make(Tok::Error, start);
  }
  const auto len = utf8Len(static_cast<unsigned char>(src_[i_]));
  if (i_ + len > src_.size()) {
    i_ = static_cast<std::uint32_t>(src_.size());
    return make(Tok::Error, start);
  }
  std::string text(src_.substr(i_, len));
  i_ += len;
  return make(Tok::Character, start, std::move(text));
}

Token Scanner::lexBinary(std::uint32_t start) {
  while (i_ < src_.size() && isBinaryChar(src_[i_])) {
    i_++;
  }
  return make(Tok::Binary, start, std::string(src_.substr(start, i_ - start)));
}

Token Scanner::next() {
  skipTrivia();
  if (i_ >= src_.size()) {
    return make(Tok::Eof, i_);
  }
  const std::uint32_t start = i_;
  const char c = src_[i_];
  if (c == '"') {
    i_ = static_cast<std::uint32_t>(src_.size());
    return make(Tok::Error, start);
  }
  if (isAlpha(c)) {
    return lexIdentOrKeyword(start);
  }
  if (isDigit(c)) {
    return lexNumber(start);
  }
  if (c == '\'') {
    return lexString(start);
  }
  if (c == '#') {
    return lexSymbol(start);
  }
  if (c == '$') {
    return lexCharacter(start);
  }
  if (c == '_') {
    i_++;
    return make(Tok::Assign, start, "_");
  }
  if (c == ':' && i_ + 1 < src_.size() && src_[i_ + 1] == '=') {
    i_ += 2;
    return make(Tok::Assign, start, ":=");
  }
  if (isBinaryChar(c)) {
    return lexBinary(start);
  }
  switch (c) {
    case '^':
      i_++;
      return make(Tok::Caret, start);
    case ';':
      i_++;
      return make(Tok::Semicolon, start);
    case '.':
      i_++;
      return make(Tok::Period, start);
    case ':':
      i_++;
      return make(Tok::Colon, start);
    case '(':
      i_++;
      return make(Tok::LParen, start);
    case ')':
      i_++;
      return make(Tok::RParen, start);
    case '[':
      i_++;
      return make(Tok::LBracket, start);
    case ']':
      i_++;
      return make(Tok::RBracket, start);
    case '{':
      i_++;
      return make(Tok::LBrace, start);
    case '}':
      i_++;
      return make(Tok::RBrace, start);
    default: {
      const auto len = utf8Len(static_cast<unsigned char>(c));
      const auto remain = static_cast<std::uint32_t>(src_.size()) - i_;
      i_ += len < remain ? len : remain;
      return make(Tok::Error, start);
    }
  }
}

}  // namespace ao::compiler

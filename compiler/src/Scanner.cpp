#include "ao/Scanner.hpp"

#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

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

// SPEC §3.8: the largest exponent an integer mantissa other than 0 may have.
constexpr std::int64_t kMaxIntegerExponent = 65536;

// Token::largeInt for integer digits outside int64: [<radix>r]<digits>, upper case, no leading
// zeros. The digits are not all zeros.
std::string largeIntText(std::string_view digits, int radix) {
  std::string text = radix == 10 ? std::string() : std::to_string(radix) + "r";
  for (std::size_t k = digits.find_first_not_of('0'); k < digits.size(); ++k) {
    const char c = digits[k];
    text.push_back(c >= 'a' && c <= 'z' ? static_cast<char>(c - 'a' + 'A') : c);
  }
  return text;
}

// A magnitude in base 2^32, least significant limb first, no zero limb on top.
using Mag = std::vector<std::uint32_t>;

void magMulAdd(Mag& m, std::uint32_t mul, std::uint32_t add) {
  std::uint64_t carry = add;
  for (std::uint32_t& limb : m) {
    const std::uint64_t t = static_cast<std::uint64_t>(limb) * mul + carry;
    limb = static_cast<std::uint32_t>(t);
    carry = t >> 32;
  }
  if (carry != 0) {
    m.push_back(static_cast<std::uint32_t>(carry));
  }
}

int magBits(const Mag& m) {
  return m.empty() ? 0 : static_cast<int>((m.size() - 1) * 32) + std::bit_width(m.back());
}

Mag magShl(const Mag& m, int k) {
  Mag r(static_cast<std::size_t>(k / 32), 0);
  const int s = k % 32;
  std::uint32_t carry = 0;
  for (const std::uint32_t limb : m) {
    r.push_back(s == 0 ? limb : (limb << s) | carry);
    carry = s == 0 ? 0 : limb >> (32 - s);
  }
  if (carry != 0) {
    r.push_back(carry);
  }
  return r;
}

int magCmp(const Mag& a, const Mag& b) {
  if (a.size() != b.size()) {
    return a.size() < b.size() ? -1 : 1;
  }
  for (std::size_t k = a.size(); k-- > 0;) {
    if (a[k] != b[k]) {
      return a[k] < b[k] ? -1 : 1;
    }
  }
  return 0;
}

// a -= b, where a >= b.
void magSub(Mag& a, const Mag& b) {
  std::int64_t borrow = 0;
  for (std::size_t k = 0; k < a.size(); ++k) {
    const std::int64_t d = static_cast<std::int64_t>(a[k]) - borrow -
                           static_cast<std::int64_t>(k < b.size() ? b[k] : 0);
    borrow = d < 0 ? 1 : 0;
    a[k] = static_cast<std::uint32_t>(d + (borrow << 32));
  }
  while (!a.empty() && a.back() == 0) {
    a.pop_back();
  }
}

// The double nearest num / den (ties to even). Neither is zero.
double nearestDouble(const Mag& num, const Mag& den) {
  // q = floor(num × 2^shift / den) has 54 or 55 bits; rem is what the division leaves.
  const int shift = 54 - (magBits(num) - magBits(den));
  Mag rem = shift > 0 ? magShl(num, shift) : num;
  const Mag d = shift < 0 ? magShl(den, -shift) : den;
  std::uint64_t q = 0;
  for (int bit = 55; bit >= 0; --bit) {
    const Mag part = magShl(d, bit);
    if (magCmp(rem, part) >= 0) {
      magSub(rem, part);
      q |= std::uint64_t{1} << bit;
    }
  }
  // Keep 53 bits, or fewer where the result is subnormal (its last bit is worth 2^-1074).
  int drop = std::bit_width(q) - 53;
  if (drop - shift < -1074) {
    drop = shift - 1074;
  }
  if (drop > 55) {
    return 0.0;  // below half the smallest subnormal
  }
  std::uint64_t kept = q >> drop;
  const std::uint64_t dropped = q & ((std::uint64_t{1} << drop) - 1);
  const std::uint64_t half = std::uint64_t{1} << (drop - 1);
  if (dropped > half || (dropped == half && (!rem.empty() || (kept & 1) != 0))) {
    kept++;
  }
  return std::ldexp(static_cast<double>(kept), drop - shift);
}

// SPEC §3.8: the Float nearest digits.fraction × radix^exp, computed exactly. (std::from_chars
// for double is not available before macOS 26, and it only reads radix 10 and 16.)
double nearestFloat(std::string_view digits, std::string_view fraction, int radix,
                    std::int64_t exp) {
  const auto r = static_cast<std::uint32_t>(radix);
  Mag num;
  for (const char c : digits) {
    magMulAdd(num, r, static_cast<std::uint32_t>(digitValue(c, radix)));
  }
  for (const char c : fraction) {
    magMulAdd(num, r, static_cast<std::uint32_t>(digitValue(c, radix)));
  }
  if (num.empty()) {
    return 0.0;
  }
  // num × radix^scale. Far outside the double range it is infinity or zero without computing.
  const std::int64_t scale = exp - static_cast<std::int64_t>(fraction.size());
  const double log2 = magBits(num) + static_cast<double>(scale) * std::log2(radix);
  if (log2 > 1100) {
    return std::numeric_limits<double>::infinity();
  }
  if (log2 < -1100) {
    return 0.0;
  }
  Mag den{1};
  Mag& scaled = scale >= 0 ? num : den;
  for (std::int64_t k = 0; k < (scale >= 0 ? scale : -scale); ++k) {
    magMulAdd(scaled, r, 0);
  }
  return nearestDouble(num, den);
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
  bool overflow = false;
  while (isDigit(at(0))) {
    if (!overflow && !addDigit(intAcc, 10, at(0) - '0')) {
      overflow = true;
    }
    i_++;
  }

  int radix = 10;
  std::uint32_t digitsStart = start;
  if ((at(0) == 'r' || at(0) == 'R') && !overflow && intAcc >= 2 && intAcc <= 36 &&
      digitValue(at(1), static_cast<int>(intAcc)) >= 0) {
    radix = static_cast<int>(intAcc);
    i_++;
    digitsStart = i_;
    intAcc = 0;
    overflow = false;
    int dv = 0;
    while ((dv = digitValue(at(0), radix)) >= 0) {
      if (!overflow && !addDigit(intAcc, radix, dv)) {
        overflow = true;
      }
      i_++;
    }
  }
  const std::string_view digits = src_.substr(digitsStart, i_ - digitsStart);

  std::string_view fraction;
  if (at(0) == '.' && digitValue(at(1), radix) >= 0) {
    i_++;
    const std::uint32_t from = i_;
    while (digitValue(at(0), radix) >= 0) {
      i_++;
    }
    fraction = src_.substr(from, i_ - from);
  }

  // The exponent stops growing far beyond any exponent a value can use.
  constexpr std::int64_t kExpSaturation = std::int64_t{1} << 56;
  bool expNeg = false;
  std::int64_t exp = 0;
  const char expMark = at(0);
  if (expMark == 'e' || expMark == 'E' || expMark == 'd' || expMark == 'D') {
    const std::uint32_t digitsOff = at(1) == '+' || at(1) == '-' ? 2 : 1;
    if (isDigit(at(digitsOff))) {
      expNeg = at(1) == '-';
      i_ += digitsOff;
      while (isDigit(at(0))) {
        if (exp < kExpSaturation) {
          exp = exp * 10 + (at(0) - '0');
        }
        i_++;
      }
    }
  }

  Token t = make(Tok::Number, start, std::string(src_.substr(start, i_ - start)));
  if (fraction.empty() && !(expNeg && exp != 0)) {
    // SPEC §3.8: an integer mantissa and an exponent of 0 or more make the Integer
    // mantissa × radix^exponent.
    const bool zero = digits.find_first_not_of('0') == std::string_view::npos;
    for (std::int64_t k = 0; !zero && !overflow && k < exp; ++k) {
      overflow = !addDigit(intAcc, radix, 0);
    }
    if (!overflow) {
      t.intValue = intAcc;
      t.number = static_cast<double>(intAcc);
      return t;
    }
    if (exp > kMaxIntegerExponent) {
      return make(Tok::Error, start, "number too large");
    }
    // No digit limit. The runtime makes the LargeInteger from the digits.
    t.largeInt = largeIntText(digits, radix);
    t.largeInt.append(static_cast<std::size_t>(exp), '0');
    return t;
  }
  // SPEC §3.8: the Float nearest mantissa × radix^exponent.
  t.isFloat = true;
  t.number = nearestFloat(digits, fraction, radix, expNeg ? -exp : exp);
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
    // SPEC §3.8: after the first character, a `-` followed by a digit starts a negative literal
    // (`2*-1` is `2 * -1`).
    if (i_ > start && src_[i_] == '-' && i_ + 1 < src_.size() && isDigit(src_[i_ + 1])) {
      break;
    }
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

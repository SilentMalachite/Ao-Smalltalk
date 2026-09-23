#pragma once

#include "ao/Span.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ao::compiler {

enum class LitKind : std::uint8_t {
  Nil,
  True,
  False,
  Int,
  Float,
  Char,
  String,
  Symbol,
  Array,
  ByteArray,
  Method,
};

struct MethodImage;

struct Literal {
  LitKind kind = LitKind::Nil;
  std::int64_t intValue = 0;
  double floatValue = 0;
  std::string text;
  std::vector<std::uint8_t> bytes;
  std::vector<Literal> elements;
  std::unique_ptr<MethodImage> method;

  Literal();
  Literal(const Literal& other);
  Literal(Literal&& other) noexcept;
  Literal& operator=(const Literal& other);
  Literal& operator=(Literal&& other) noexcept;
  ~Literal();
};

struct MethodImage {
  struct TempBinding {
    std::string name;
    bool workspace = false;
  };

  std::string selector;
  std::uint8_t numArgs = 0;
  std::uint8_t numTemps = 0;
  std::uint16_t primitive = 0;
  std::vector<std::uint8_t> bytes;
  std::vector<Literal> literals;
  std::vector<TempBinding> tempBindings;
};

inline Literal::Literal() = default;
inline Literal::Literal(Literal&& other) noexcept = default;
inline Literal& Literal::operator=(Literal&& other) noexcept = default;
inline Literal::~Literal() = default;

inline Literal::Literal(const Literal& other)
    : kind(other.kind),
      intValue(other.intValue),
      floatValue(other.floatValue),
      text(other.text),
      bytes(other.bytes),
      elements(other.elements),
      method(other.method ? std::make_unique<MethodImage>(*other.method) : nullptr) {}

inline Literal& Literal::operator=(const Literal& other) {
  if (this != &other) {
    kind = other.kind;
    intValue = other.intValue;
    floatValue = other.floatValue;
    text = other.text;
    bytes = other.bytes;
    elements = other.elements;
    method = other.method ? std::make_unique<MethodImage>(*other.method) : nullptr;
  }
  return *this;
}

struct CompileEnv {
  std::vector<std::string> instVarNames;
  std::vector<std::string> knownGlobals;
  std::vector<std::string> workspaceTemps;
  bool undeclaredAreTemps = false;
};

struct CompileResult {
  bool ok = false;
  MethodImage image;
  CompileError error;
};

}  // namespace ao::compiler

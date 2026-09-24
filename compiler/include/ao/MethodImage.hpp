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
  // A workspace binding named `text`. The runtime boxes it as the session's Association.
  Binding,
  // The class variable named `text` (SPEC §3.6). The runtime boxes it as its binding, the
  // Association in the classPool of the method's class or of a superclass.
  ClassVariable,
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
  std::string selector;
  std::uint8_t numArgs = 0;
  std::uint8_t numTemps = 0;
  std::uint16_t primitive = 0;
  std::vector<std::uint8_t> bytes;
  std::vector<Literal> literals;
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
  // SPEC §3.6 / §3.8: the class variables a method of the class sees, its own class's and its
  // superclasses'. A name that is no local, no instance variable and no pseudo-variable is a class
  // variable when it is here (LitVar on its binding), else a global.
  std::vector<std::string> classVarNames;
  std::vector<std::string> knownGlobals;
  // Workspace (SPEC §3.10): a name that is not local, an instance variable, a pseudo-variable or
  // a known global is a binding (LitVar). The method answers its last expression.
  bool undeclaredAreBindings = false;
};

struct CompileResult {
  bool ok = false;
  MethodImage image;
  CompileError error;
};

}  // namespace ao::compiler

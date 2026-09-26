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
  // Int: empty, or the value outside int64 as [-][<radix>r]<digits> (intValue is 0 then).
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

// One entry of the pc-to-source map (SPEC §3.8): the instruction at pc and the source it runs,
// as UTF-8 byte offsets [start, end) from where the compiled text begins (a doIt's `doIt\n`
// prefix included). A block's map uses the coordinates of its method's source.
struct PcSpan {
  std::uint32_t pc = 0;
  std::uint32_t start = 0;
  std::uint32_t end = 0;
};

enum class TempKind : std::uint8_t {
  Arg,
  Temp,     // a declared temp, also one of an inlined block
  LoopVar,  // the loop variable of an inlined to:do:
  Copied,   // an outer variable a block's closure copied
};

// A named temp of a frame (SPEC §3.8). The temp at `slot` holds the value, or, when vecIndex is
// not -1, the temp vector that holds it at vecIndex. For a captured and assigned local, slot is
// the vector's temp; for a Copied variable, slot is the copied value's temp (the copied vector's
// when vecIndex is not -1).
struct TempName {
  std::string name;
  TempKind kind = TempKind::Temp;
  std::uint8_t slot = 0;
  std::int16_t vecIndex = -1;
};

struct MethodImage {
  std::string selector;
  std::uint8_t numArgs = 0;
  std::uint8_t numTemps = 0;
  std::uint16_t primitive = 0;
  std::vector<std::uint8_t> bytes;
  std::vector<Literal> literals;
  // Debug information (SPEC §3.8). Not part of the CompiledMethod or the image.
  std::vector<PcSpan> pcMap;  // ascending pc
  std::vector<TempName> temps;
  // The pc of each statement's first instruction, the statements of inlined blocks too;
  // ascending, no repeats (SPEC §3.8 文の先頭表, the step of §3.13).
  std::vector<std::uint32_t> statementPcs;
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
  // SPEC §3.6 / §3.8: the first this many of instVarNames are the slots a Kernel class adds. The
  // natives rely on what they hold, so source reads them and an assignment to one is the compile
  // error "cannot assign to Kernel instance variable <name>".
  std::size_t kernelInstVarCount = 0;
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

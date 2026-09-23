#pragma once

#include <cstdint>
#include <string_view>

namespace ao::compiler {

enum class Op : std::uint8_t {
  PushReceiver = 0,
  PushTrue,
  PushFalse,
  PushNil,
  PushThisContext,
  PushMinusOne,
  PushZero,
  PushOne,
  PushTwo,
  PushTemp,
  PushInstVar,
  PushLiteral,
  PushGlobal,
  Pop,
  Dup,
  StoreTemp,
  StoreInstVar,
  PopStoreTemp,
  PopStoreInstVar,
  Send,
  SendSuper,
  SendSpecial,
  Jump,
  JumpTrue,
  JumpFalse,
  ReturnReceiver,
  ReturnTrue,
  ReturnFalse,
  ReturnNil,
  ReturnTop,
  ReturnBlock,
  CreateBlock,
  Primitive,
  // Appended in B2. Numbers only grow; existing ones never change (SPEC §3.5).
  PushNewArray,
  PushRemoteTemp,
  StoreRemoteTemp,
  PopStoreRemoteTemp,
  PushLitVar,
  StoreLitVar,
  PopStoreLitVar,
};

inline constexpr Op kLastOp = Op::PopStoreLitVar;

inline constexpr std::uint8_t operandBytes(Op op) {
  switch (op) {
    case Op::PushTemp:
    case Op::PushInstVar:
    case Op::PushLiteral:
    case Op::PushGlobal:
    case Op::StoreTemp:
    case Op::StoreInstVar:
    case Op::PopStoreTemp:
    case Op::PopStoreInstVar:
    case Op::PushNewArray:
    case Op::PushLitVar:
    case Op::StoreLitVar:
    case Op::PopStoreLitVar:
      return 1;
    case Op::Send:
    case Op::SendSuper:
    case Op::SendSpecial:
    case Op::Jump:
    case Op::JumpTrue:
    case Op::JumpFalse:
    case Op::CreateBlock:
    case Op::Primitive:
    case Op::PushRemoteTemp:
    case Op::StoreRemoteTemp:
    case Op::PopStoreRemoteTemp:
      return 2;
    case Op::PushReceiver:
    case Op::PushTrue:
    case Op::PushFalse:
    case Op::PushNil:
    case Op::PushThisContext:
    case Op::PushMinusOne:
    case Op::PushZero:
    case Op::PushOne:
    case Op::PushTwo:
    case Op::Pop:
    case Op::Dup:
    case Op::ReturnReceiver:
    case Op::ReturnTrue:
    case Op::ReturnFalse:
    case Op::ReturnNil:
    case Op::ReturnTop:
    case Op::ReturnBlock:
      return 0;
  }
  return 0;
}

inline constexpr std::uint8_t specialCount() { return 28; }

constexpr const char* specialSelector(std::uint8_t i) {
  constexpr const char* kSpecial[specialCount()] = {
      "+",          "-",        "*",     "/",      "//",     "\\\\", "@",      "bitShift:",
      "bitAnd:",    "bitOr:",   "<",     ">",      "<=",     ">=",   "=",      "~=",
      "==",         "class",    "size",  "at:",    "at:put:", "value", "value:", "do:",
      "new",        "new:",     "x",     "y",
  };
  if (i >= specialCount()) {
    return nullptr;
  }
  return kSpecial[i];
}

// Numbers in the special selector table that the interpreter answers for two SmallIntegers
// without a send (SPEC §3.5). The checks below tie each number to its place in the table.
inline constexpr std::uint8_t kSpecialAdd = 0;
inline constexpr std::uint8_t kSpecialSubtract = 1;
inline constexpr std::uint8_t kSpecialMultiply = 2;
inline constexpr std::uint8_t kSpecialLess = 10;
inline constexpr std::uint8_t kSpecialGreater = 11;
inline constexpr std::uint8_t kSpecialLessEqual = 12;
inline constexpr std::uint8_t kSpecialGreaterEqual = 13;
inline constexpr std::uint8_t kSpecialEqual = 14;

static_assert(std::string_view(specialSelector(kSpecialAdd)) == "+");
static_assert(std::string_view(specialSelector(kSpecialSubtract)) == "-");
static_assert(std::string_view(specialSelector(kSpecialMultiply)) == "*");
static_assert(std::string_view(specialSelector(kSpecialLess)) == "<");
static_assert(std::string_view(specialSelector(kSpecialGreater)) == ">");
static_assert(std::string_view(specialSelector(kSpecialLessEqual)) == "<=");
static_assert(std::string_view(specialSelector(kSpecialGreaterEqual)) == ">=");
static_assert(std::string_view(specialSelector(kSpecialEqual)) == "=");

}  // namespace ao::compiler

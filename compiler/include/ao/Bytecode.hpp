#pragma once

#include <cstdint>

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
};

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
      return 1;
    case Op::Send:
    case Op::SendSuper:
    case Op::SendSpecial:
    case Op::Jump:
    case Op::JumpTrue:
    case Op::JumpFalse:
    case Op::CreateBlock:
    case Op::Primitive:
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

inline const char* specialSelector(std::uint8_t i) {
  static constexpr const char* kSpecial[28] = {
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

}  // namespace ao::compiler

using ao::compiler::operandBytes;
using ao::compiler::specialSelector;

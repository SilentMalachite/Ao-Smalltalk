#include "ao/Bytecode.hpp"
#include <gtest/gtest.h>
#include <set>

TEST(BytecodeIsa, RequiredGroupsArePresent) {
  using ao::compiler::Op;
  std::set<Op> ops{
      Op::PushReceiver, Op::PushTemp, Op::PushLiteral, Op::PushNil, Op::PushTrue,
      Op::Pop, Op::Dup, Op::Send, Op::SendSuper, Op::SendSpecial,
      Op::Jump, Op::JumpTrue, Op::JumpFalse,
      Op::ReturnTop, Op::ReturnBlock, Op::CreateBlock, Op::Primitive};
  EXPECT_EQ(17u, ops.size());
  EXPECT_EQ(2u, operandBytes(Op::Jump));
  EXPECT_EQ(2u, operandBytes(Op::Send));
  EXPECT_EQ(0u, operandBytes(Op::ReturnTop));
  EXPECT_STREQ("+", specialSelector(0));
}

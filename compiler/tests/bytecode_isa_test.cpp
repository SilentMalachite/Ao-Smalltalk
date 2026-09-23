#include "ao/Bytecode.hpp"
#include <gtest/gtest.h>
#include <set>

TEST(BytecodeIsa, RequiredGroupsArePresent) {
  using ao::compiler::Op;
  using ao::compiler::operandBytes;
  using ao::compiler::specialSelector;
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

// SPEC §3.5: 番号は追記だけで増やす。既存の番号は変えない。
TEST(BytecodeIsa, NewOpsAppendedAfterPrimitive) {
  using ao::compiler::Op;
  using ao::compiler::operandBytes;
  EXPECT_EQ(32, static_cast<int>(Op::Primitive));
  EXPECT_EQ(33, static_cast<int>(Op::PushNewArray));
  EXPECT_EQ(34, static_cast<int>(Op::PushRemoteTemp));
  EXPECT_EQ(35, static_cast<int>(Op::StoreRemoteTemp));
  EXPECT_EQ(36, static_cast<int>(Op::PopStoreRemoteTemp));
  EXPECT_EQ(37, static_cast<int>(Op::PushLitVar));
  EXPECT_EQ(38, static_cast<int>(Op::StoreLitVar));
  EXPECT_EQ(39, static_cast<int>(Op::PopStoreLitVar));
  EXPECT_EQ(Op::PopStoreLitVar, ao::compiler::kLastOp);
  EXPECT_EQ(1u, operandBytes(Op::PushNewArray));
  EXPECT_EQ(2u, operandBytes(Op::PushRemoteTemp));
  EXPECT_EQ(2u, operandBytes(Op::StoreRemoteTemp));
  EXPECT_EQ(2u, operandBytes(Op::PopStoreRemoteTemp));
  EXPECT_EQ(1u, operandBytes(Op::PushLitVar));
  EXPECT_EQ(1u, operandBytes(Op::StoreLitVar));
  EXPECT_EQ(1u, operandBytes(Op::PopStoreLitVar));
}

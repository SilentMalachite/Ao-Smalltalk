#include "test_support.hpp"

#include "ao/Bytecode.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Natives.hpp"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <vector>

namespace {

using Op = ao::compiler::Op;

std::uint8_t op(Op o) { return static_cast<std::uint8_t>(o); }

// Hand-assembled method: code and literals, no arguments.
ao::Oop assemble(Boot& b, std::initializer_list<std::uint8_t> code, std::uint8_t numTemps,
                 ao::Oop literals) {
  const std::vector<std::uint8_t> bytes(code);
  ao::Root lits(b.roots, literals);
  ao::Root ba(b.roots, b.heap.allocate(b.wk.byteArrayClass,
                                       static_cast<std::uint32_t>(bytes.size()), ao::kFlagBytes));
  if (!ba.slot.isHeap()) {
    return ao::Oop{};
  }
  std::memcpy(b.heap.bytes(ba.slot), bytes.data(), bytes.size());
  return ao::CompiledMethod::create(b.ctx, 0, numTemps, 0, lits.slot, ba.slot,
                                    b.wk.intern("hand"), b.wk.objectClass);
}

ao::Oop noLiterals(Boot& b) { return ao::Arr::fromSlots(b.heap, b.wk, nullptr, 0); }

ao::Oop runMethod(Boot& b, ao::Oop cm) {
  return ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

}  // namespace

// temp 0 に大きさ 2 の temp ベクタを置き、添字 0 と 1 を読み書きする。
TEST(Interpreter, HandAssembledRemoteTempRoundTrip) {
  Boot b;
  ao::Root cm(b.roots,
              assemble(b,
                       {op(Op::PushNewArray), 2,
                        op(Op::PopStoreTemp), 0,
                        op(Op::PushTwo),
                        op(Op::StoreRemoteTemp), 1, 0,
                        op(Op::Pop),
                        op(Op::PushRemoteTemp), 1, 0,
                        op(Op::PushOne),
                        op(Op::SendSpecial), 0, 1,
                        op(Op::PopStoreRemoteTemp), 0, 0,
                        op(Op::PushRemoteTemp), 0, 0,
                        op(Op::PushRemoteTemp), 1, 0,
                        op(Op::SendSpecial), 2, 1,
                        op(Op::ReturnTop)},
                       1, noLiterals(b)));
  ASSERT_TRUE(cm.slot.isHeap());
  const ao::Oop got = runMethod(b, cm.slot);
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(6, got.smallIntegerValue());
}

TEST(Interpreter, PushNewArrayIsNilFilledArray) {
  Boot b;
  ao::Root cm(b.roots, assemble(b, {op(Op::PushNewArray), 3, op(Op::ReturnTop)}, 0, noLiterals(b)));
  ASSERT_TRUE(cm.slot.isHeap());
  const ao::Oop got = runMethod(b, cm.slot);
  ASSERT_TRUE(got.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(got));
  ASSERT_EQ(3u, b.heap.size(got));
  for (std::uint32_t i = 0; i < 3; ++i) {
    EXPECT_TRUE(b.heap.slotAt(got, i).isNil());
  }
}

// リテラル 0 の Association の値を読み、1 を足して書き戻す。
TEST(Interpreter, HandAssembledLitVarRoundTrip) {
  Boot b;
  ao::Root assoc(b.roots, b.heap.allocate(b.wk.associationClass, 2, 0));
  ASSERT_TRUE(assoc.slot.isHeap());
  b.heap.slotAtPut(assoc.slot, ao::kAssocKey, b.wk.intern("v"));
  b.heap.slotAtPut(assoc.slot, ao::kAssocValue, ao::Oop::fromSmallInteger(41));
  ao::Root lits(b.roots, ao::Arr::fromSlots(b.heap, b.wk, &assoc.slot, 1));
  ao::Root cm(b.roots, assemble(b,
                                {op(Op::PushLitVar), 0,
                                 op(Op::PushOne),
                                 op(Op::SendSpecial), 0, 1,
                                 op(Op::PopStoreLitVar), 0,
                                 op(Op::PushLitVar), 0,
                                 op(Op::StoreLitVar), 0,
                                 op(Op::ReturnTop)},
                                0, lits.slot));
  ASSERT_TRUE(cm.slot.isHeap());
  const ao::Oop got = runMethod(b, cm.slot);
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(42, got.smallIntegerValue());
  EXPECT_EQ(42, b.heap.slotAt(assoc.slot, ao::kAssocValue).smallIntegerValue());
}

// SPEC §3.5: 分岐の値が Boolean でなければ mustBeBoolean を送る。Object の既定は abort。
TEST(Interpreter, JumpOnNonBooleanAborts) {
  Boot b;
  ao::Root cm(b.roots, assemble(b,
                                {op(Op::PushNil),
                                 op(Op::JumpFalse), 2, 0,
                                 op(Op::PushOne),
                                 op(Op::ReturnTop),
                                 op(Op::PushTwo),
                                 op(Op::ReturnTop)},
                                0, noLiterals(b)));
  ASSERT_TRUE(cm.slot.isHeap());
  const ao::Oop got = runMethod(b, cm.slot);
  EXPECT_TRUE(got.isEmpty());
  EXPECT_TRUE(b.ctx.aborting);
  ASSERT_NE(nullptr, b.ctx.abortReason);
  EXPECT_EQ(std::string("NonBoolean receiver"), b.ctx.abortReason);
}

// mustBeBoolean を上書きしたクラスでは、その答えで分岐する。
TEST(Interpreter, JumpOnNonBooleanUsesMustBeBooleanAnswer) {
  Boot b;
  const char* src =
      "!Object subclass: #R2Truthy\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B2-Test'!\n"
      "!R2Truthy methodsFor: 't'!\n"
      "mustBeBoolean\n"
      "  ^false! !\n";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, src, errs));
  ASSERT_TRUE(errs.empty()) << errs[0].message;
  ao::Root truthy(b.roots, send0(b, b.wk.named("R2Truthy"), "new"));
  ASSERT_TRUE(truthy.slot.isHeap());
  ao::Root lits(b.roots, ao::Arr::fromSlots(b.heap, b.wk, &truthy.slot, 1));
  ao::Root cm(b.roots, assemble(b,
                                {op(Op::PushLiteral), 0,
                                 op(Op::JumpFalse), 2, 0,
                                 op(Op::PushOne),
                                 op(Op::ReturnTop),
                                 op(Op::PushTwo),
                                 op(Op::ReturnTop)},
                                0, lits.slot));
  ASSERT_TRUE(cm.slot.isHeap());
  const ao::Oop got = runMethod(b, cm.slot);
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(2, got.smallIntegerValue());
  EXPECT_FALSE(b.ctx.aborting);
}

// 範囲外の番号は、実行せずに失敗する。
TEST(Interpreter, OpcodePastLastOpFails) {
  Boot b;
  ao::Root cm(b.roots, assemble(b,
                                {static_cast<std::uint8_t>(op(ao::compiler::kLastOp) + 1),
                                 op(Op::PushOne), op(Op::ReturnTop)},
                                0, noLiterals(b)));
  ASSERT_TRUE(cm.slot.isHeap());
  EXPECT_TRUE(runMethod(b, cm.slot).isEmpty());
}

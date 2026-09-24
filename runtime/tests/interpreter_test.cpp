#include "test_support.hpp"

#include "ao/Bytecode.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/LargeInteger.hpp"
#include "ao/Natives.hpp"
#include "ao/Oop.hpp"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <initializer_list>
#include <limits>
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

namespace {

ao::Oop smallInt(std::int64_t v) { return ao::Oop::fromSmallInteger(v); }

// source を Object のメソッドとしてコンパイルする。
ao::Oop compileObjectMethod(Boot& b, const std::string& source) {
  auto img = ao::compiler::compileMethod(source);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  return ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass);
}

// `^a <sel> x` をコンパイルする。<sel> は SendSpecial になる。
ao::Oop compileBinary(Boot& b, const char* sel) {
  return compileObjectMethod(b, std::string("f: a with: x\n  ^a ") + sel + " x");
}

// 2 引数のメソッド cm を nil に送る。run は引数をルートしてから GC しうる。
ao::Oop runBinary(Boot& b, ao::Oop cm, ao::Oop a, ao::Oop x) {
  const ao::Oop args[2] = {a, x};
  return ao::Interpreter::run(b.ctx, cm, ao::Oop::nil(), args, 2, ao::Oop::nil());
}

ao::Oop makeFloat(Boot& b, double v) {
  const ao::Oop o = b.heap.allocate(b.wk.floatClass, sizeof(double), ao::kFlagBytes);
  if (o.isHeap()) {
    std::memcpy(b.heap.bytes(o), &v, sizeof(v));
  }
  return o;
}

}  // namespace

// SPEC §3.5: SmallInteger 同士の SendSpecial は送信しない。展開した to:do: の <= と +、本体の +
// のどれも送らない。
TEST(Interpreter, SendSpecialSmallIntegerFastPathSendsNothing) {
  Boot b;
  const char* src =
      "!Object subclass: #B3Loop\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n"
      "!B3Loop methodsFor: 't'!\n"
      "loop\n"
      "  ^1 to: 1000 do: [:i | i + 1]! !\n";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, src, errs));
  ASSERT_TRUE(errs.empty()) << errs[0].message;
  ao::Root inst(b.roots, send0(b, b.wk.named("B3Loop"), "new"));
  ASSERT_TRUE(inst.slot.isHeap());
  const std::uint64_t sends = b.ctx.interpretedSends;
  const ao::Oop got = send0(b, inst.slot, "loop");
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(1, got.smallIntegerValue());
  EXPECT_EQ(0u, b.ctx.interpretedSends - sends);
}

// SPEC §3.5: 答えが SmallInteger に収まらなければ送信に落ち、LargeInteger の正しい値になる。
TEST(Interpreter, SendSpecialOverflowFallsBackToLargeInteger) {
  Boot b;
  ao::Root add(b.roots, compileBinary(b, "+"));
  ao::Root sub(b.roots, compileBinary(b, "-"));
  ao::Root mul(b.roots, compileBinary(b, "*"));
  ASSERT_TRUE(add.slot.isHeap() && sub.slot.isHeap() && mul.slot.isHeap());
  constexpr std::int64_t kTwo31 = std::int64_t{1} << 31;
  struct Case {
    const ao::Oop* cm;
    std::int64_t a;
    std::int64_t x;
    std::int64_t want;
    bool positive;
  };
  const Case cases[] = {
      {&add.slot, ao::kSmiMax, 1, ao::kSmiMax + 1, true},
      {&add.slot, ao::kSmiMax, ao::kSmiMax, ao::kSmiMax * 2, true},
      {&sub.slot, ao::kSmiMin, 1, ao::kSmiMin - 1, false},
      {&sub.slot, ao::kSmiMin, ao::kSmiMax, ao::kSmiMin - ao::kSmiMax, false},
      {&mul.slot, kTwo31, kTwo31, kTwo31 * kTwo31, true},
      {&mul.slot, -kTwo31, kTwo31 * 2, std::numeric_limits<std::int64_t>::min(), false},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(testing::Message() << c.a << " op " << c.x);
    const std::uint64_t sends = b.ctx.interpretedSends;
    const ao::Oop got = runBinary(b, *c.cm, smallInt(c.a), smallInt(c.x));
    EXPECT_EQ(1u, b.ctx.interpretedSends - sends);
    ASSERT_TRUE(got.isHeap());
    EXPECT_EQ(c.positive ? b.wk.largePositiveIntegerClass : b.wk.largeNegativeIntegerClass,
              b.heap.klass(got));
    bool fits = false;
    EXPECT_EQ(c.want, ao::LargeInteger::asInt64IfFits(b.heap, b.wk, got, &fits));
    EXPECT_TRUE(fits);
  }
  // int64 にも収まらない積。割り戻して確かめる。
  constexpr std::int64_t kRoot = 3037000500;
  ao::Root big(b.roots, runBinary(b, mul.slot, smallInt(kRoot), smallInt(kRoot)));
  ASSERT_TRUE(big.slot.isHeap());
  EXPECT_EQ(b.wk.largePositiveIntegerClass, b.heap.klass(big.slot));
  const ao::Oop quotient = send1(b, big.slot, "//", smallInt(kRoot));
  ASSERT_TRUE(quotient.isSmallInteger());
  EXPECT_EQ(kRoot, quotient.smallIntegerValue());
  EXPECT_EQ(smallInt(0), send1(b, big.slot, "\\\\", smallInt(kRoot)));
  // 端で収まる答えは SmallInteger のまま。
  EXPECT_EQ(smallInt(ao::kSmiMax), runBinary(b, add.slot, smallInt(ao::kSmiMax), smallInt(0)));
  EXPECT_EQ(smallInt(ao::kSmiMin), runBinary(b, sub.slot, smallInt(ao::kSmiMin), smallInt(0)));
  EXPECT_EQ(smallInt(ao::kSmiMin), runBinary(b, mul.slot, smallInt(ao::kSmiMin), smallInt(1)));
}

// SPEC §3.5: SmallInteger でない値が混じれば通常の送信になり、答えは送信したときと同じ。
// Integer の算術は Float の引数に Float を答え（SPEC §3.6）、Integer>>= は Integer でない値に
// false を答える。
TEST(Interpreter, SendSpecialNonSmallIntegerFallsBack) {
  Boot b;
  // 引数の片方だけをヒープに置く。ルートしてからコンパイル（GC しうる）して送る。
  const auto run = [&b](const char* sel, ao::Oop a, ao::Oop x) {
    ao::Root ra(b.roots, a);
    ao::Root rx(b.roots, x);
    ao::Root cm(b.roots, compileBinary(b, sel));
    const std::uint64_t sends = b.ctx.interpretedSends;
    const ao::Oop got = runBinary(b, cm.slot, ra.slot, rx.slot);
    EXPECT_EQ(1u, b.ctx.interpretedSends - sends) << sel;
    // SPEC §3.3: a send whose native fails aborts with its selector instead of answering.
    if (got.isEmpty()) {
      EXPECT_EQ(std::string("failed: #") + sel, takeAbortReason(b));
    }
    EXPECT_FALSE(b.ctx.aborting) << sel;
    return got;
  };
  const ao::Oop three = smallInt(3);
  const auto floatValue = [&b](ao::Oop o) {
    double v = -1;
    if (o.isHeap() && b.heap.klass(o) == b.wk.floatClass) {
      std::memcpy(&v, b.heap.bytes(o), sizeof(v));
    }
    return v;
  };
  EXPECT_DOUBLE_EQ(7.5, floatValue(run("+", three, makeFloat(b, 4.5))));
  EXPECT_DOUBLE_EQ(-1.5, floatValue(run("-", three, makeFloat(b, 4.5))));
  EXPECT_DOUBLE_EQ(13.5, floatValue(run("*", three, makeFloat(b, 4.5))));
  for (const char* sel : {"<", "<=", ">="}) {
    EXPECT_TRUE(run(sel, three, makeFloat(b, 4.5)).isEmpty()) << sel;
  }
  EXPECT_TRUE(run(">", three, makeFloat(b, 4.5)).isFalse());
  EXPECT_TRUE(run("=", three, makeFloat(b, 4.5)).isFalse());
  EXPECT_TRUE(run("=", three, makeFloat(b, 3.0)).isFalse());
  EXPECT_TRUE(run("=", three, ao::Oop::nil()).isFalse());
  EXPECT_TRUE(run("=", ao::Oop::nil(), three).isFalse());
  EXPECT_TRUE(run("+", three, ao::Oop::nil()).isEmpty());
  EXPECT_TRUE(run("<", makeFloat(b, 4.5), three).isFalse());
  EXPECT_TRUE(run("<", ao::Oop::fromCharacter(U'a'), ao::Oop::fromCharacter(U'b')).isTrue());
  ao::Root sum(b.roots, run("+", makeFloat(b, 4.5), three));
  ASSERT_TRUE(sum.slot.isHeap());
  ASSERT_EQ(b.wk.floatClass, b.heap.klass(sum.slot));
  double v = 0;
  std::memcpy(&v, b.heap.bytes(sum.slot), sizeof(v));
  EXPECT_DOUBLE_EQ(7.5, v);
  // LargeInteger が混じれば送信し、収まる答えは SmallInteger に戻る。
  EXPECT_EQ(smallInt(ao::kSmiMax),
            run("-", ao::LargeInteger::fromInt64(b.ctx, ao::kSmiMax + 1), smallInt(1)));
  EXPECT_TRUE(run("<", three, ao::LargeInteger::fromInt64(b.ctx, ao::kSmiMax + 1)).isTrue());
  ao::Root large(b.roots, ao::LargeInteger::fromInt64(b.ctx, ao::kSmiMax + 1));
  ASSERT_TRUE(large.slot.isHeap());
  EXPECT_TRUE(run("=", large.slot, large.slot).isTrue());
}

// SPEC §3.5: 8 セレクタを SmallInteger の境界（等しい値、負数、0、両端）で送る。答えは C++ で
// 計算した値と、C++ から送ってネイティブが答えた値の両方に等しく、インタプリタは送信しない。
TEST(Interpreter, SendSpecialComparisonsAnswerBooleans) {
  Boot b;
  const std::int64_t edges[] = {ao::kSmiMin, -3, -1, 0, 1, 3, ao::kSmiMax};
  struct Compare {
    const char* sel;
    bool (*want)(std::int64_t, std::int64_t);
  };
  const Compare compares[] = {
      {"<", [](std::int64_t p, std::int64_t q) { return p < q; }},
      {">", [](std::int64_t p, std::int64_t q) { return p > q; }},
      {"<=", [](std::int64_t p, std::int64_t q) { return p <= q; }},
      {">=", [](std::int64_t p, std::int64_t q) { return p >= q; }},
      {"=", [](std::int64_t p, std::int64_t q) { return p == q; }},
  };
  for (const Compare& c : compares) {
    ao::Root cm(b.roots, compileBinary(b, c.sel));
    ASSERT_TRUE(cm.slot.isHeap()) << c.sel;
    for (const std::int64_t p : edges) {
      for (const std::int64_t q : edges) {
        SCOPED_TRACE(testing::Message() << p << ' ' << c.sel << ' ' << q);
        const std::uint64_t sends = b.ctx.interpretedSends;
        const ao::Oop got = runBinary(b, cm.slot, smallInt(p), smallInt(q));
        EXPECT_EQ(0u, b.ctx.interpretedSends - sends);
        EXPECT_EQ(c.want(p, q) ? ao::Oop::true_() : ao::Oop::false_(), got);
        EXPECT_EQ(send1(b, smallInt(p), c.sel, smallInt(q)), got);
      }
    }
  }
  struct Arith {
    const char* sel;
    std::int64_t (*want)(std::int64_t, std::int64_t);
  };
  const Arith ariths[] = {
      {"+", [](std::int64_t p, std::int64_t q) { return p + q; }},
      {"-", [](std::int64_t p, std::int64_t q) { return p - q; }},
      {"*", [](std::int64_t p, std::int64_t q) { return p * q; }},
  };
  const std::int64_t small[] = {-3, -1, 0, 1, 3};
  for (const Arith& c : ariths) {
    ao::Root cm(b.roots, compileBinary(b, c.sel));
    ASSERT_TRUE(cm.slot.isHeap()) << c.sel;
    for (const std::int64_t p : small) {
      for (const std::int64_t q : small) {
        SCOPED_TRACE(testing::Message() << p << ' ' << c.sel << ' ' << q);
        const std::uint64_t sends = b.ctx.interpretedSends;
        const ao::Oop got = runBinary(b, cm.slot, smallInt(p), smallInt(q));
        EXPECT_EQ(0u, b.ctx.interpretedSends - sends);
        EXPECT_EQ(smallInt(c.want(p, q)), got);
        EXPECT_EQ(send1(b, smallInt(p), c.sel, smallInt(q)), got);
      }
    }
  }
}

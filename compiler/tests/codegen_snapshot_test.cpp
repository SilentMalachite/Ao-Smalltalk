#include "ao/Bytecode.hpp"
#include "ao/Compiler.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using ao::compiler::compileMethod;
using ao::compiler::disassemble;
using ao::compiler::LitKind;

TEST(Codegen, ReturnOnePlusTwo) {
  auto r = compileMethod("foo\n  ^1 + 2");
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ("foo", r.image.selector);
  EXPECT_EQ(0, r.image.numArgs);
  EXPECT_EQ(0, r.image.primitive);
  const std::string d = disassemble(r.image);
  EXPECT_EQ(
      "method foo args=0 temps=0 prim=0\n"
      "literals:\n"
      "  PushOne\n"
      "  PushTwo\n"
      "  SendSpecial 0 1\n"
      "  ReturnTop\n",
      d);
}

TEST(Codegen, TempAndAssign) {
  auto r = compileMethod("bar: x\n  | t |\n  t := x.\n  ^t");
  ASSERT_TRUE(r.ok);
  EXPECT_EQ(1, r.image.numArgs);
  EXPECT_EQ(2, r.image.numTemps);
  const std::string d = disassemble(r.image);
  EXPECT_TRUE(d.find("PushTemp 0") != std::string::npos);
  EXPECT_TRUE(d.find("PopStoreTemp 1") != std::string::npos);
  EXPECT_TRUE(d.find("PushTemp 1") != std::string::npos);
}

TEST(Codegen, CascadeAndBlock) {
  auto r = compileMethod("baz\n  ^self foo; bar: 1");
  ASSERT_TRUE(r.ok);
  EXPECT_TRUE(disassemble(r.image).find("Dup") != std::string::npos);
  auto b = compileMethod("qux\n  ^[:a | a + 1]");
  ASSERT_TRUE(b.ok);
  EXPECT_TRUE(disassemble(b.image).find("CreateBlock") != std::string::npos);
}

TEST(Codegen, SuperSendAndGlobal) {
  auto r = compileMethod("hash\n  ^super hash");
  ASSERT_TRUE(r.ok);
  EXPECT_TRUE(disassemble(r.image).find("SendSuper") != std::string::npos);
  auto g = compileMethod("x\n  ^Object");
  ASSERT_TRUE(g.ok);
  EXPECT_TRUE(disassemble(g.image).find("PushGlobal") != std::string::npos);
}

TEST(Codegen, ErrorSpanOnDanglingBinary) {
  auto r = compileMethod("foo\n  ^1 +");
  EXPECT_FALSE(r.ok);
  EXPECT_GT(r.error.span.end, r.error.span.start);
}

TEST(Codegen, LiteralArrayPseudoObjectsAreNotSymbols) {
  auto r = compileMethod("foo\n  ^#(nil true false)");
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(1u, r.image.literals.size());
  EXPECT_EQ(LitKind::Array, r.image.literals[0].kind);
  ASSERT_EQ(3u, r.image.literals[0].elements.size());
  EXPECT_EQ(LitKind::Nil, r.image.literals[0].elements[0].kind);
  EXPECT_EQ(LitKind::True, r.image.literals[0].elements[1].kind);
  EXPECT_EQ(LitKind::False, r.image.literals[0].elements[2].kind);
}

TEST(Codegen, EighteenDigitIntegerSurvivesAsInt) {
  auto r = compileMethod("foo\n  ^100000000000000001");
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(1u, r.image.literals.size());
  EXPECT_EQ(LitKind::Int, r.image.literals[0].kind);
  EXPECT_EQ(100000000000000001LL, r.image.literals[0].intValue);
}

TEST(Codegen, DisassemblesAppendedOps) {
  using ao::compiler::Op;
  ao::compiler::MethodImage image;
  image.selector = "hand";
  image.numTemps = 1;
  image.bytes = {
      static_cast<std::uint8_t>(Op::PushNewArray), 2,
      static_cast<std::uint8_t>(Op::PopStoreTemp), 0,
      static_cast<std::uint8_t>(Op::PushRemoteTemp), 1, 0,
      static_cast<std::uint8_t>(Op::StoreRemoteTemp), 1, 0,
      static_cast<std::uint8_t>(Op::PopStoreRemoteTemp), 0, 0,
      static_cast<std::uint8_t>(Op::PushLitVar), 0,
      static_cast<std::uint8_t>(Op::StoreLitVar), 0,
      static_cast<std::uint8_t>(Op::PopStoreLitVar), 0,
      static_cast<std::uint8_t>(Op::ReturnNil),
  };
  EXPECT_EQ(
      "method hand args=0 temps=1 prim=0\n"
      "literals:\n"
      "  PushNewArray 2\n"
      "  PopStoreTemp 0\n"
      "  PushRemoteTemp 1 0\n"
      "  StoreRemoteTemp 1 0\n"
      "  PopStoreRemoteTemp 0 0\n"
      "  PushLitVar 0\n"
      "  StoreLitVar 0\n"
      "  PopStoreLitVar 0\n"
      "  ReturnNil\n",
      disassemble(image));
}

namespace {

const ao::compiler::MethodImage* firstBlock(const ao::compiler::MethodImage& image) {
  for (const auto& lit : image.literals) {
    if (lit.kind == LitKind::Method) {
      return lit.method.get();
    }
  }
  return nullptr;
}

}  // namespace

// SPEC §3.4: 捕捉して代入する temp は temp ベクタに置く。ベクタはメソッドの入口で作る。
TEST(Codegen, BoxedTempUsesRemoteTemp) {
  auto r = compileMethod("foo\n  | x |\n  x := 1.\n  [x := 2] value.\n  ^x");
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(1, r.image.numTemps);
  const std::string d = disassemble(r.image);
  EXPECT_EQ(0u, d.find("method foo args=0 temps=1 prim=0\nliterals: [method]\n  PushNewArray 1\n"
                       "  PopStoreTemp 0\n  PushOne\n  PopStoreRemoteTemp 0 0\n  PushTemp 0\n"
                       "  CreateBlock 0 1\n"))
      << d;
  EXPECT_NE(std::string::npos, d.find("PushRemoteTemp 0 0"));
  const auto* inner = firstBlock(r.image);
  ASSERT_NE(nullptr, inner);
  EXPECT_EQ(1, inner->numTemps);
  EXPECT_NE(std::string::npos, disassemble(*inner).find("StoreRemoteTemp 0 0"));
}

// 代入の無い捕捉は値をコピーする（ベクタを作らない）。
TEST(Codegen, UnwrittenCaptureIsCopied) {
  auto r = compileMethod("foo: x\n  ^[x + 1]");
  ASSERT_TRUE(r.ok) << r.error.message;
  const std::string d = disassemble(r.image);
  EXPECT_EQ(std::string::npos, d.find("PushNewArray"));
  EXPECT_NE(std::string::npos, d.find("PushTemp 0\n  CreateBlock 0 1"));
  const auto* inner = firstBlock(r.image);
  ASSERT_NE(nullptr, inner);
  EXPECT_EQ(1, inner->numTemps);
  EXPECT_NE(std::string::npos, disassemble(*inner).find("PushTemp 0"));
  EXPECT_EQ(std::string::npos, disassemble(*inner).find("RemoteTemp"));
}

// 中間のブロックは、使わなくても内側のためにベクタを受け渡す。
TEST(Codegen, MiddleBlockForwardsVector) {
  auto r = compileMethod("foo\n  | a |\n  ^[[a := 1]]");
  ASSERT_TRUE(r.ok) << r.error.message;
  const auto* middle = firstBlock(r.image);
  ASSERT_NE(nullptr, middle);
  EXPECT_EQ(1, middle->numTemps);
  EXPECT_NE(std::string::npos, disassemble(*middle).find("PushTemp 0\n  CreateBlock 0 1"));
  const auto* inner = firstBlock(*middle);
  ASSERT_NE(nullptr, inner);
  EXPECT_NE(std::string::npos, disassemble(*inner).find("StoreRemoteTemp 0 0"));
}

// SPEC §3.8: 引数（メソッドとブロック）への代入はコンパイルエラー。
TEST(Codegen, ArgumentAssignIsError) {
  auto m = compileMethod("foo: x\n  x := 5");
  EXPECT_FALSE(m.ok);
  EXPECT_EQ("cannot assign to argument", m.error.message);
  auto b = compileMethod("foo\n  ^[:a | a := 1]");
  EXPECT_FALSE(b.ok);
  EXPECT_EQ("cannot assign to argument", b.error.message);
}

namespace {

using ao::compiler::MethodImage;
using ao::compiler::Op;

// 命令 1 つ。a と b はオペランドのバイト、rel はジャンプのオフセット（符号付き）。
struct Insn {
  Op op = Op::PushNil;
  std::uint8_t a = 0;
  std::uint8_t b = 0;
  int rel = 0;
};

// image のバイト列を命令に分ける（disassemble と同じ読み方）。
std::vector<Insn> decode(const MethodImage& image) {
  std::vector<Insn> out;
  const std::vector<std::uint8_t>& bytes = image.bytes;
  std::size_t i = 0;
  while (i < bytes.size()) {
    Insn ins;
    ins.op = static_cast<Op>(bytes[i]);
    const std::uint8_t n = ao::compiler::operandBytes(ins.op);
    if (i + n >= bytes.size()) {
      out.push_back(ins);
      break;
    }
    if (n >= 1) {
      ins.a = bytes[i + 1];
    }
    if (n == 2) {
      ins.b = bytes[i + 2];
    }
    if (ins.op == Op::Jump || ins.op == Op::JumpTrue || ins.op == Op::JumpFalse) {
      const auto u = static_cast<std::uint16_t>(ins.a | (ins.b << 8));
      ins.rel = static_cast<int>(static_cast<std::int16_t>(u));
    }
    out.push_back(ins);
    i += 1u + n;
  }
  return out;
}

// image 自身（中のブロックは見ない）が selector を送る命令を持てば true。
bool sends(const MethodImage& image, const std::string& selector) {
  for (const Insn& ins : decode(image)) {
    if (ins.op == Op::Send || ins.op == Op::SendSuper) {
      if (ins.a < image.literals.size() && image.literals[ins.a].kind == LitKind::Symbol &&
          image.literals[ins.a].text == selector) {
        return true;
      }
    } else if (ins.op == Op::SendSpecial) {
      const char* special = ao::compiler::specialSelector(ins.a);
      if (special != nullptr && selector == special) {
        return true;
      }
    }
  }
  return false;
}

int countOp(const MethodImage& image, Op op) {
  int n = 0;
  for (const Insn& ins : decode(image)) {
    if (ins.op == op) {
      n += 1;
    }
  }
  return n;
}

bool hasConditionalJump(const MethodImage& image) {
  return countOp(image, Op::JumpTrue) + countOp(image, Op::JumpFalse) > 0;
}

// ループの印: 負のオフセットを持つジャンプ。whileTrue のように条件ジャンプで戻る形も認める。
bool hasBackwardJump(const MethodImage& image) {
  for (const Insn& ins : decode(image)) {
    const bool jump = ins.op == Op::Jump || ins.op == Op::JumpTrue || ins.op == Op::JumpFalse;
    if (jump && ins.rel < 0) {
      return true;
    }
  }
  return false;
}

}  // namespace

// SPEC §3.5: 0 引数のリテラルブロックを取る条件分岐と and: / or: は、送信せずにジャンプへ展開する。
// ブロックの CompiledMethod も CreateBlock も作らない。
TEST(Inline, IfTrueLiteralInlinesToJumps) {
  struct Case {
    const char* source;
    const char* selector;
  };
  const Case cases[] = {
      {"foo: x\n  ^x ifTrue: [1] ifFalse: [2]", "ifTrue:ifFalse:"},
      {"foo: x\n  ^x ifFalse: [1] ifTrue: [2]", "ifFalse:ifTrue:"},
      {"foo: x\n  ^x ifTrue: [1]", "ifTrue:"},
      {"foo: x\n  ^x ifFalse: [1]", "ifFalse:"},
      {"foo: x\n  ^x and: [1]", "and:"},
      {"foo: x\n  ^x or: [1]", "or:"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.source);
    auto r = compileMethod(c.source);
    ASSERT_TRUE(r.ok) << r.error.message;
    const std::string d = disassemble(r.image);
    EXPECT_FALSE(sends(r.image, c.selector)) << d;
    EXPECT_EQ(0, countOp(r.image, Op::CreateBlock)) << d;
    EXPECT_EQ(nullptr, firstBlock(r.image)) << d;
    EXPECT_TRUE(hasConditionalJump(r.image)) << d;
    EXPECT_FALSE(hasBackwardJump(r.image)) << d;
  }
}

// ブロックが外側の temp に代入しても、展開したブロックは実スコープではないので temp ベクタは要らない。
TEST(Inline, InlinedBranchAssignsOuterTempDirectly) {
  auto r = compileMethod("foo: x\n  | y |\n  x ifTrue: [y := 1] ifFalse: [y := 2].\n  ^y");
  ASSERT_TRUE(r.ok) << r.error.message;
  const std::string d = disassemble(r.image);
  EXPECT_EQ(nullptr, firstBlock(r.image)) << d;
  EXPECT_EQ(std::string::npos, d.find("PushNewArray")) << d;
  EXPECT_EQ(std::string::npos, d.find("RemoteTemp")) << d;
}

// リテラルでないブロック引数、カスケードのパート、super 送信、引数の数が合わないブロックは展開しない。
TEST(Inline, NonLiteralArgIsSend) {
  auto held = compileMethod("foo: x\n  | b |\n  b := [1].\n  ^x ifTrue: b");
  ASSERT_TRUE(held.ok) << held.error.message;
  EXPECT_TRUE(sends(held.image, "ifTrue:")) << disassemble(held.image);

  auto cascade = compileMethod("foo: x\n  ^x ifTrue: [1]; ifFalse: [2]");
  ASSERT_TRUE(cascade.ok) << cascade.error.message;
  const std::string dc = disassemble(cascade.image);
  EXPECT_TRUE(sends(cascade.image, "ifTrue:")) << dc;
  EXPECT_TRUE(sends(cascade.image, "ifFalse:")) << dc;
  EXPECT_EQ(2, countOp(cascade.image, Op::CreateBlock)) << dc;

  auto superSend = compileMethod("foo\n  ^super ifTrue: [1]");
  ASSERT_TRUE(superSend.ok) << superSend.error.message;
  EXPECT_TRUE(sends(superSend.image, "ifTrue:")) << disassemble(superSend.image);
  EXPECT_EQ(1, countOp(superSend.image, Op::SendSuper)) << disassemble(superSend.image);
  EXPECT_EQ(1, countOp(superSend.image, Op::CreateBlock)) << disassemble(superSend.image);

  auto oneArg = compileMethod("foo: x\n  ^x ifTrue: [:y | y]");
  ASSERT_TRUE(oneArg.ok) << oneArg.error.message;
  EXPECT_TRUE(sends(oneArg.image, "ifTrue:")) << disassemble(oneArg.image);

  auto heldCond = compileMethod("foo\n  | c |\n  c := [false].\n  ^c whileTrue: [1]");
  ASSERT_TRUE(heldCond.ok) << heldCond.error.message;
  EXPECT_TRUE(sends(heldCond.image, "whileTrue:")) << disassemble(heldCond.image);

  auto unaryHeld = compileMethod("foo: c\n  ^c whileFalse");
  ASSERT_TRUE(unaryHeld.ok) << unaryHeld.error.message;
  EXPECT_TRUE(sends(unaryHeld.image, "whileFalse")) << disassemble(unaryHeld.image);

  auto heldBody = compileMethod("foo: blk\n  ^1 to: 3 do: blk");
  ASSERT_TRUE(heldBody.ok) << heldBody.error.message;
  EXPECT_TRUE(sends(heldBody.image, "to:do:")) << disassemble(heldBody.image);

  auto noArgBody = compileMethod("foo\n  ^1 to: 3 do: [nil]");
  ASSERT_TRUE(noArgBody.ok) << noArgBody.error.message;
  EXPECT_TRUE(sends(noArgBody.image, "to:do:")) << disassemble(noArgBody.image);
}

// SPEC §3.5: while ループは後方ジャンプ（負のオフセット）で回る。ブロックは作らない。
TEST(Inline, WhileTrueEmitsBackwardJump) {
  struct Case {
    const char* source;
    const char* selector;
  };
  const Case cases[] = {
      {"foo\n  | i |\n  i := 0.\n  [i < 3] whileTrue: [i := i + 1].\n  ^i", "whileTrue:"},
      {"foo\n  | i |\n  i := 0.\n  [i >= 3] whileFalse: [i := i + 1].\n  ^i", "whileFalse:"},
      {"foo\n  | i |\n  i := 0.\n  [i := i + 1. i < 3] whileTrue.\n  ^i", "whileTrue"},
      {"foo\n  | i |\n  i := 0.\n  [i := i + 1. i >= 3] whileFalse.\n  ^i", "whileFalse"},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.source);
    auto r = compileMethod(c.source);
    ASSERT_TRUE(r.ok) << r.error.message;
    const std::string d = disassemble(r.image);
    EXPECT_FALSE(sends(r.image, c.selector)) << d;
    EXPECT_EQ(0, countOp(r.image, Op::CreateBlock)) << d;
    EXPECT_EQ(nullptr, firstBlock(r.image)) << d;
    EXPECT_TRUE(hasBackwardJump(r.image)) << d;
    EXPECT_TRUE(hasConditionalJump(r.image)) << d;
    EXPECT_EQ(std::string::npos, d.find("RemoteTemp")) << d;
  }
}

// SPEC §3.5: to:do: は、継続の判定に <= を、増分に + を本当に送るループになる。刻みが負なら >=。
TEST(Inline, ToDoEmitsCompareAndIncrementLoop) {
  auto up = compileMethod("foo\n  1 to: 10 do: [:i | i]");
  ASSERT_TRUE(up.ok) << up.error.message;
  const std::string du = disassemble(up.image);
  EXPECT_FALSE(sends(up.image, "to:do:")) << du;
  EXPECT_EQ(0, countOp(up.image, Op::CreateBlock)) << du;
  EXPECT_EQ(nullptr, firstBlock(up.image)) << du;
  EXPECT_TRUE(sends(up.image, "<=")) << du;
  EXPECT_TRUE(sends(up.image, "+")) << du;
  EXPECT_TRUE(hasBackwardJump(up.image)) << du;
  EXPECT_TRUE(hasConditionalJump(up.image)) << du;

  auto down = compileMethod("foo\n  10 to: 1 by: -2 do: [:i | i]");
  ASSERT_TRUE(down.ok) << down.error.message;
  const std::string dd = disassemble(down.image);
  EXPECT_FALSE(sends(down.image, "to:by:do:")) << dd;
  EXPECT_EQ(0, countOp(down.image, Op::CreateBlock)) << dd;
  EXPECT_TRUE(sends(down.image, ">=")) << dd;
  EXPECT_FALSE(sends(down.image, "<=")) << dd;
  EXPECT_TRUE(sends(down.image, "+")) << dd;
  EXPECT_TRUE(hasBackwardJump(down.image)) << dd;

  auto stepped = compileMethod("foo\n  1 to: 10 by: 3 do: [:i | i]");
  ASSERT_TRUE(stepped.ok) << stepped.error.message;
  const std::string ds = disassemble(stepped.image);
  EXPECT_FALSE(sends(stepped.image, "to:by:do:")) << ds;
  EXPECT_TRUE(sends(stepped.image, "<=")) << ds;
  EXPECT_FALSE(sends(stepped.image, ">=")) << ds;
}

// SPEC §3.5: to:by:do: を展開するのは、刻みが 0 でない数値リテラルのときだけ。
TEST(Inline, ToByDoRequiresNonzeroLiteralStep) {
  struct Case {
    const char* source;
    bool inlined;
  };
  const Case cases[] = {
      {"foo\n  1 to: 5 by: 0 do: [:i | i]", false},
      {"foo: s\n  1 to: 5 by: s do: [:i | i]", false},
      {"foo\n  1 to: 5 by: 1 + 1 do: [:i | i]", false},
      {"foo\n  1 to: 5 by: 2 do: [:i | i]", true},
      {"foo\n  5 to: 1 by: -1 do: [:i | i]", true},
      {"foo\n  1 to: 2 by: 0.5 do: [:i | i]", true},
  };
  for (const Case& c : cases) {
    SCOPED_TRACE(c.source);
    auto r = compileMethod(c.source);
    ASSERT_TRUE(r.ok) << r.error.message;
    const std::string d = disassemble(r.image);
    EXPECT_EQ(!c.inlined, sends(r.image, "to:by:do:")) << d;
    EXPECT_EQ(c.inlined ? 0 : 1, countOp(r.image, Op::CreateBlock)) << d;
    EXPECT_EQ(c.inlined, hasBackwardJump(r.image)) << d;
  }
}

// SPEC §3.8: 展開したブロックの temp、to:do: のループ変数と上限は、含む実スコープの temp に持ち上げる。
// 展開したブロックは CompiledMethod にならない。
TEST(Inline, InlinedBlockTempsDoNotCountAsBlocks) {
  // ループ変数と、1 回だけ評価した上限。
  auto loop = compileMethod("foo\n  1 to: self size do: [:i | i]");
  ASSERT_TRUE(loop.ok) << loop.error.message;
  EXPECT_EQ(nullptr, firstBlock(loop.image)) << disassemble(loop.image);
  EXPECT_EQ(0, loop.image.numArgs);
  EXPECT_GE(static_cast<int>(loop.image.numTemps), 2);

  // 展開した分岐の temp。
  auto branch = compileMethod("foo: x\n  ^x ifTrue: [| t | t := 1. t]");
  ASSERT_TRUE(branch.ok) << branch.error.message;
  EXPECT_EQ(nullptr, firstBlock(branch.image)) << disassemble(branch.image);
  EXPECT_EQ(1, branch.image.numArgs);
  EXPECT_GE(static_cast<int>(branch.image.numTemps), 2);

  // 展開した while の本体の temp。
  auto body = compileMethod(
      "foo\n  | i |\n  i := 0.\n  [i < 3] whileTrue: [| t | t := i. i := t + 1].\n  ^i");
  ASSERT_TRUE(body.ok) << body.error.message;
  EXPECT_EQ(nullptr, firstBlock(body.image)) << disassemble(body.image);
  EXPECT_GE(static_cast<int>(body.image.numTemps), 2);

  // 展開しないブロックの中で展開した to:do: の temp は、そのブロックの temp に数える。
  auto nested = compileMethod("foo\n  ^[:n | 1 to: n + 1 do: [:i | i]]");
  ASSERT_TRUE(nested.ok) << nested.error.message;
  EXPECT_EQ(0, nested.image.numTemps);
  const auto* blk = firstBlock(nested.image);
  ASSERT_NE(nullptr, blk);
  EXPECT_EQ(1, blk->numArgs);
  EXPECT_GE(static_cast<int>(blk->numTemps), 3);
  EXPECT_EQ(nullptr, firstBlock(*blk)) << disassemble(*blk);
  EXPECT_FALSE(sends(*blk, "to:do:")) << disassemble(*blk);
}

// SPEC §3.5: 展開したブロック内の ^ は、それを含む実スコープの ^ と同じ。メソッドならメソッドから
// 返り（ReturnBlock ではない）、展開しないブロックなら非局所リターン（ReturnBlock）になる。
TEST(Inline, ReturnInInlinedBlockFollowsRealScope) {
  auto method = compileMethod("foo: x\n  x ifTrue: [^1].\n  ^2");
  ASSERT_TRUE(method.ok) << method.error.message;
  const std::string dm = disassemble(method.image);
  EXPECT_EQ(0, countOp(method.image, Op::ReturnBlock)) << dm;
  EXPECT_EQ(0, countOp(method.image, Op::CreateBlock)) << dm;
  EXPECT_FALSE(sends(method.image, "ifTrue:")) << dm;

  auto loop = compileMethod("foo\n  1 to: 10 do: [:i | i = 4 ifTrue: [^i]].\n  ^0");
  ASSERT_TRUE(loop.ok) << loop.error.message;
  const std::string dl = disassemble(loop.image);
  EXPECT_EQ(0, countOp(loop.image, Op::ReturnBlock)) << dl;
  EXPECT_EQ(nullptr, firstBlock(loop.image)) << dl;

  auto inBlock = compileMethod("foo\n  ^[:x | x ifTrue: [^1]. 2]");
  ASSERT_TRUE(inBlock.ok) << inBlock.error.message;
  const auto* blk = firstBlock(inBlock.image);
  ASSERT_NE(nullptr, blk);
  const std::string db = disassemble(*blk);
  EXPECT_EQ(1, countOp(*blk, Op::ReturnBlock)) << db;
  EXPECT_EQ(nullptr, firstBlock(*blk)) << db;
  EXPECT_FALSE(sends(*blk, "ifTrue:")) << db;
}

// SPEC §3.8: 展開しても、to:do: のループ変数への代入はコンパイルエラーのまま。
TEST(Inline, LoopVariableAssignIsError) {
  auto r = compileMethod("foo\n  1 to: 3 do: [:i | i := 5]");
  EXPECT_FALSE(r.ok);
  EXPECT_EQ("cannot assign to argument", r.error.message);
}

namespace {

// A method of a class with the instance variable x and the class variables Count and x.
ao::compiler::CompileEnv classVarEnv() {
  ao::compiler::CompileEnv env;
  env.instVarNames = {"x"};
  env.classVarNames = {"Count", "x"};
  return env;
}

// The class variable literals named `name` in image and the blocks inside it.
int classVarLiterals(const MethodImage& image, const std::string& name) {
  int n = 0;
  for (const auto& lit : image.literals) {
    if (lit.kind == LitKind::ClassVariable && lit.text == name) {
      ++n;
    }
    if (lit.kind == LitKind::Method && lit.method) {
      n += classVarLiterals(*lit.method, name);
    }
  }
  return n;
}

}  // namespace

// B4 (docs/claude-review/05 High) / SPEC §3.8: 失敗シナリオ。クラス変数への代入は「cannot assign」に
// ならず PopStoreLitVar / StoreLitVar、読みは PushLitVar で、どれも 1 つのクラス変数リテラルを使う。
TEST(ClassVariable, ReadAndAssignUseOneLitVarLiteral) {
  auto r = compileMethod("bump\n  Count := 1.\n  ^Count := Count + 1", classVarEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  const std::string d = disassemble(r.image);
  EXPECT_EQ(1, classVarLiterals(r.image, "Count")) << d;
  EXPECT_EQ(1, countOp(r.image, Op::PopStoreLitVar)) << d;
  EXPECT_EQ(1, countOp(r.image, Op::StoreLitVar)) << d;
  EXPECT_EQ(1, countOp(r.image, Op::PushLitVar)) << d;
  EXPECT_EQ(0, countOp(r.image, Op::PushGlobal)) << d;
}

// SPEC §3.8: ローカル → インスタンス変数 → 擬変数 → クラス変数 → グローバル。ローカル（temp、引数、
// ブロック引数）とインスタンス変数は、同じ名前のクラス変数を隠す。
TEST(ClassVariable, LocalsAndInstanceVariablesHideClassVariables) {
  for (const char* src : {"temp\n  | Count | Count := 2. ^Count", "arg: Count\n  ^Count",
                          "blockArg\n  ^[:Count | Count] value: 3", "ivar\n  x := 4. ^x"}) {
    SCOPED_TRACE(src);
    auto r = compileMethod(src, classVarEnv());
    ASSERT_TRUE(r.ok) << r.error.message;
    EXPECT_EQ(0, classVarLiterals(r.image, "Count")) << disassemble(r.image);
    EXPECT_EQ(0, classVarLiterals(r.image, "x")) << disassemble(r.image);
    EXPECT_EQ(0, countOp(r.image, Op::PushLitVar)) << disassemble(r.image);
  }
  auto iv = compileMethod("ivar\n  ^x", classVarEnv());
  ASSERT_TRUE(iv.ok) << iv.error.message;
  EXPECT_EQ(1, countOp(iv.image, Op::PushInstVar)) << disassemble(iv.image);
}

// SPEC §3.8: クラス変数は同じ名前のグローバルを隠し、ブロックの中からも同じリテラルで読み書きする。
// クラス変数でない名前は、今までどおり PushGlobal で、代入は「cannot assign」。
TEST(ClassVariable, ClassVariableHidesGlobalInsideBlocks) {
  auto r = compileMethod("g\n  ^[Count := Count] value", classVarEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  const auto* blk = firstBlock(r.image);
  ASSERT_NE(nullptr, blk);
  const std::string db = disassemble(*blk);
  EXPECT_EQ(1, classVarLiterals(*blk, "Count")) << db;
  EXPECT_EQ(1, countOp(*blk, Op::PushLitVar)) << db;
  EXPECT_EQ(1, countOp(*blk, Op::StoreLitVar)) << db;
  EXPECT_EQ(0, countOp(*blk, Op::PushGlobal)) << db;

  auto global = compileMethod("g\n  ^Count");
  ASSERT_TRUE(global.ok) << global.error.message;
  EXPECT_EQ(1, countOp(global.image, Op::PushGlobal)) << disassemble(global.image);
  auto assign = compileMethod("s\n  Count := 1");
  EXPECT_FALSE(assign.ok);
  EXPECT_EQ("cannot assign", assign.error.message);
}

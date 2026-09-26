#include "ao/Bytecode.hpp"
#include "ao/Compiler.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

using ao::compiler::compileMethod;
using ao::compiler::LitKind;
using ao::compiler::MethodImage;
using ao::compiler::Op;
using ao::compiler::PcSpan;
using ao::compiler::TempKind;
using ao::compiler::TempName;

namespace {

// The pc of the first instruction `op` in image, or -1.
int pcOf(const MethodImage& image, Op op) {
  std::size_t pc = 0;
  while (pc < image.bytes.size()) {
    const auto at = static_cast<Op>(image.bytes[pc]);
    if (at == op) {
      return static_cast<int>(pc);
    }
    pc += 1 + ao::compiler::operandBytes(at);
  }
  return -1;
}

// The source text of the entry whose pc is exactly pc, or "<none>".
std::string spanAt(const MethodImage& image, std::string_view source, int pc) {
  for (const PcSpan& e : image.pcMap) {
    if (static_cast<int>(e.pc) == pc) {
      return std::string(source.substr(e.start, e.end - e.start));
    }
  }
  return "<none>";
}

const MethodImage* firstBlock(const MethodImage& image) {
  for (const auto& lit : image.literals) {
    if (lit.kind == LitKind::Method) {
      return lit.method.get();
    }
  }
  return nullptr;
}

const TempName* named(const MethodImage& image, const std::string& name) {
  for (const TempName& t : image.temps) {
    if (t.name == name) {
      return &t;
    }
  }
  return nullptr;
}

}  // namespace

TEST(PcMap, SendPcMapsToSendSpan) {
  const std::string src = "foo\n  ^self bar: 1";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const int pc = pcOf(r.image, Op::Send);
  ASSERT_GE(pc, 0);
  EXPECT_EQ("self bar: 1", spanAt(r.image, src, pc));
  for (std::size_t i = 1; i < r.image.pcMap.size(); ++i) {
    EXPECT_LT(r.image.pcMap[i - 1].pc, r.image.pcMap[i].pc);
  }
}

TEST(PcMap, InlinedConditionJumpMapsToReceiverSpan) {
  const std::string src = "foo: x\n  ^x ifTrue: [1]";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const int pc = pcOf(r.image, Op::JumpFalse);
  ASSERT_GE(pc, 0);
  EXPECT_EQ("x", spanAt(r.image, src, pc));
}

TEST(PcMap, ReturnBlockMapsToReturnStatement) {
  const std::string src = "foo\n  [^1] value";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const MethodImage* blk = firstBlock(r.image);
  ASSERT_NE(nullptr, blk);
  const int pc = pcOf(*blk, Op::ReturnBlock);
  ASSERT_GE(pc, 0);
  EXPECT_EQ("^1", spanAt(*blk, src, pc));
}

TEST(PcMap, BlockMethodHasItsOwnMapInMethodCoordinates) {
  const std::string src = "foo\n  ^[:a | [a bar] value] value: 3";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const MethodImage* outer = firstBlock(r.image);
  ASSERT_NE(nullptr, outer);
  const MethodImage* inner = firstBlock(*outer);
  ASSERT_NE(nullptr, inner);
  // value and value: are special selectors.
  EXPECT_EQ("[a bar] value", spanAt(*outer, src, pcOf(*outer, Op::SendSpecial)));
  EXPECT_EQ("a bar", spanAt(*inner, src, pcOf(*inner, Op::Send)));
  EXPECT_EQ("[:a | [a bar] value] value: 3", spanAt(r.image, src, pcOf(r.image, Op::SendSpecial)));
}

TEST(PcMap, TempNamesListArgsTempsAndLoopVars) {
  const std::string src = "foo: a with: b\n  | t u |\n  1 to: 3 do: [:i | t := i].\n  ^t";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const auto& temps = r.image.temps;
  ASSERT_EQ(5u, temps.size());
  EXPECT_EQ("a", temps[0].name);
  EXPECT_EQ(TempKind::Arg, temps[0].kind);
  EXPECT_EQ(0, temps[0].slot);
  EXPECT_EQ("b", temps[1].name);
  EXPECT_EQ(TempKind::Arg, temps[1].kind);
  EXPECT_EQ(1, temps[1].slot);
  EXPECT_EQ("t", temps[2].name);
  EXPECT_EQ(TempKind::Temp, temps[2].kind);
  EXPECT_EQ(2, temps[2].slot);
  EXPECT_EQ("u", temps[3].name);
  EXPECT_EQ(TempKind::Temp, temps[3].kind);
  EXPECT_EQ(3, temps[3].slot);
  EXPECT_EQ("i", temps[4].name);
  EXPECT_EQ(TempKind::LoopVar, temps[4].kind);
  // Slot 4 is the hidden limit of to:do:, which has no name.
  EXPECT_EQ(5, temps[4].slot);
  for (const TempName& t : temps) {
    EXPECT_EQ(-1, t.vecIndex) << t.name;
  }
}

TEST(PcMap, RemoteTempNamesVectorSlotAndIndex) {
  const std::string src = "foo: x\n  | s t |\n  [t := x. s := 2] value.\n  ^t";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const TempName* s = named(r.image, "s");
  const TempName* t = named(r.image, "t");
  ASSERT_NE(nullptr, s);
  ASSERT_NE(nullptr, t);
  EXPECT_EQ(TempKind::Temp, t->kind);
  // [x][vector]: the vector sits after the argument; s and t are in it in declaration order.
  EXPECT_EQ(1, s->slot);
  EXPECT_EQ(0, s->vecIndex);
  EXPECT_EQ(1, t->slot);
  EXPECT_EQ(1, t->vecIndex);
}

TEST(PcMap, CopiedOuterTempIsNamedInBlockScope) {
  const std::string src = "foo: x\n  | t |\n  ^[:a | t := a + x] value: 2";
  auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const MethodImage* blk = firstBlock(r.image);
  ASSERT_NE(nullptr, blk);
  const TempName* a = named(*blk, "a");
  const TempName* x = named(*blk, "x");
  const TempName* t = named(*blk, "t");
  ASSERT_NE(nullptr, a);
  ASSERT_NE(nullptr, x);
  ASSERT_NE(nullptr, t);
  EXPECT_EQ(TempKind::Arg, a->kind);
  EXPECT_EQ(0, a->slot);
  // The copied values follow the block's own temps, in the order the closure copies them.
  EXPECT_EQ(TempKind::Copied, t->kind);
  EXPECT_EQ(TempKind::Copied, x->kind);
  EXPECT_EQ(-1, x->vecIndex);
  EXPECT_EQ(0, t->vecIndex);
  EXPECT_NE(t->slot, x->slot);
  EXPECT_GE(t->slot, 1);
  EXPECT_GE(x->slot, 1);
  EXPECT_EQ(3u, blk->temps.size());
}

// SPEC §3.8: an entry before every Return*, the implicit ones too (review of P10-02). An implicit
// return's span is the statement whose value or end it returns after: the last statement, or the
// whole body when it has none.
TEST(PcMap, ImplicitReturnsMapToLastStatement) {
  {
    const std::string src = "foo\n  self bar";
    auto r = compileMethod(src);
    ASSERT_TRUE(r.ok) << r.error.message;
    EXPECT_EQ("self bar", spanAt(r.image, src, pcOf(r.image, Op::ReturnReceiver)));
  }
  {
    const std::string src = "foo\n  self bar.\n  self baz: 1";
    auto r = compileMethod(src);
    ASSERT_TRUE(r.ok) << r.error.message;
    EXPECT_EQ("self baz: 1", spanAt(r.image, src, pcOf(r.image, Op::ReturnReceiver)));
  }
  {
    const std::string src = "foo";
    auto r = compileMethod(src);
    ASSERT_TRUE(r.ok) << r.error.message;
    EXPECT_EQ("foo", spanAt(r.image, src, pcOf(r.image, Op::ReturnReceiver)));
  }
  {
    const std::string src = "foo\n  ^[3 + 4] value";
    auto r = compileMethod(src);
    ASSERT_TRUE(r.ok) << r.error.message;
    const MethodImage* blk = firstBlock(r.image);
    ASSERT_NE(nullptr, blk);
    EXPECT_EQ("3 + 4", spanAt(*blk, src, pcOf(*blk, Op::ReturnTop)));
  }
  {
    const std::string src = "foo\n  ^[:a | a bar. a baz] value: 1";
    auto r = compileMethod(src);
    ASSERT_TRUE(r.ok) << r.error.message;
    const MethodImage* blk = firstBlock(r.image);
    ASSERT_NE(nullptr, blk);
    EXPECT_EQ("a baz", spanAt(*blk, src, pcOf(*blk, Op::ReturnTop)));
  }
  {
    const std::string src = "foo\n  ^[:a | ] value: 1";
    auto r = compileMethod(src);
    ASSERT_TRUE(r.ok) << r.error.message;
    const MethodImage* blk = firstBlock(r.image);
    ASSERT_NE(nullptr, blk);
    EXPECT_EQ("[:a | ]", spanAt(*blk, src, pcOf(*blk, Op::ReturnTop)));
  }
  {
    ao::compiler::CompileEnv env;
    env.undeclaredAreBindings = true;
    const std::string src = "doIt\n  3 + 4.\n  5 + 6";
    auto r = compileMethod(src, env);
    ASSERT_TRUE(r.ok) << r.error.message;
    EXPECT_EQ("5 + 6", spanAt(r.image, src, pcOf(r.image, Op::ReturnTop)));
  }
  {
    ao::compiler::CompileEnv env;
    env.undeclaredAreBindings = true;
    const std::string src = "doIt\n  5 + 6";
    auto r = compileMethod(src, env);
    ASSERT_TRUE(r.ok) << r.error.message;
    EXPECT_EQ("5 + 6", spanAt(r.image, src, pcOf(r.image, Op::ReturnTop)));
  }
}

namespace {

bool strictlyAscending(const std::vector<std::uint32_t>& pcs) {
  for (std::size_t i = 1; i < pcs.size(); ++i) {
    if (pcs[i - 1] >= pcs[i]) {
      return false;
    }
  }
  return true;
}

}  // namespace

// SPEC §3.8 文の先頭表: every statement's first instruction, inlined blocks' statements included,
// ascending; a block has its own. The bytecode does not change.
TEST(PcMap, StatementPcsListEveryStatementStart) {
  const std::string src =
      "foo\n  | a |\n  a := 1.\n  a > 0 ifTrue: [a := 2. a := 3].\n  ^[:x | x. x + 1]";
  const auto r = compileMethod(src);
  ASSERT_TRUE(r.ok) << r.error.message;
  const std::vector<std::uint32_t>& pcs = r.image.statementPcs;
  ASSERT_EQ(5u, pcs.size());
  EXPECT_TRUE(strictlyAscending(pcs));
  EXPECT_EQ(0u, pcs[0]);
  // The inlined statements come after the conditional jump, before the ^.
  const int jump = pcOf(r.image, Op::JumpFalse);
  ASSERT_GE(jump, 0);
  EXPECT_GT(pcs[2], static_cast<std::uint32_t>(jump));
  EXPECT_LT(pcs[3], pcs[4]);
  EXPECT_EQ("a := 2", spanAt(r.image, src, static_cast<int>(pcs[2])));
  const MethodImage* block = firstBlock(r.image);
  ASSERT_NE(nullptr, block);
  ASSERT_EQ(2u, block->statementPcs.size());
  EXPECT_EQ(0u, block->statementPcs[0]);
  EXPECT_TRUE(strictlyAscending(block->statementPcs));
  // A workspace doIt: each statement, the last one too.
  ao::compiler::CompileEnv env;
  env.undeclaredAreBindings = true;
  const auto d = compileMethod("doIt\nx := 3. y := x + 1. y", env);
  ASSERT_TRUE(d.ok) << d.error.message;
  EXPECT_EQ(3u, d.image.statementPcs.size());
  EXPECT_TRUE(strictlyAscending(d.image.statementPcs));
}

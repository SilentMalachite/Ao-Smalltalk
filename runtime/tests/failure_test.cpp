// SPEC §3.3「失敗は評価を中断する」: 失敗は値として返らず、abort で最外まで巻き戻す。

#include "ao_abi.h"

#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Send.hpp"

#include <cstring>
#include <string>

#include <gtest/gtest.h>

namespace {

class FailureAbort : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  void TearDown() override { ao_runtime_shutdown(); }

  int print(const char* src) {
    err_ = AoSpan{};
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out_, sizeof out_,
                   &err_);
  }
  int doIt(const char* src) {
    err_ = AoSpan{};
    return ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_DOIT, out_, sizeof out_, &err_);
  }
  std::string printed(const char* src) {
    if (print(src) != AO_OK) {
      ADD_FAILURE() << src << ": " << err_.message;
      return {};
    }
    return out_;
  }

  char out_[256] = {};
  AoSpan err_{};
};

// Compiles body as a doIt and runs it outermost on b, like evalExpr in block_test.
ao::Oop runDoIt(Boot& b, const std::string& body) {
  auto img = ao::compiler::compileMethod("doIt\n" + body);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  return ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

}  // namespace

// 02 High: 失敗した送信の空 OOP が値として積まれ、Message に化けない。
TEST_F(FailureAbort, NilArgumentToPlusIsEvalError) {
  EXPECT_EQ(AO_ERR_EVAL, print("(1 + nil) class"));
  EXPECT_STRNE("", err_.message);
  EXPECT_STREQ("", out_);
}

// 02 High: 引数個数の合わないブロックの失敗も同じ。
TEST_F(FailureAbort, WrongArityBlockIsEvalError) {
  EXPECT_EQ(AO_ERR_EVAL, print("([:x | x] value) class"));
  EXPECT_STRNE("", err_.message);
  EXPECT_STREQ("", out_);
}

// SPEC §3.3 手順 4、§6: 未定義セレクタは doesNotUnderstand: #sel で中断する。
TEST_F(FailureAbort, DoesNotUnderstandNamesSelector) {
  EXPECT_EQ(AO_ERR_EVAL, print("nil foo"));
  EXPECT_STREQ("doesNotUnderstand: #foo", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("3 zork: 4 with: 5"));
  EXPECT_STREQ("doesNotUnderstand: #zork:with:", err_.message);
}

// SPEC §3.3: error: は引数の String を理由にする。
TEST_F(FailureAbort, ErrorReasonIsStringArgument) {
  EXPECT_EQ(AO_ERR_EVAL, print("self error: 'boom'"));
  EXPECT_STREQ("boom", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("nil error: #sym"));
  EXPECT_STREQ("sym", err_.message);
}

// SPEC §3.3: String 以外の引数は printString を理由にする。
TEST_F(FailureAbort, ErrorReasonPrintsOtherArgument) {
  EXPECT_EQ(AO_ERR_EVAL, print("nil error: 42"));
  EXPECT_STREQ("42", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("nil error: #(1 $a)"));
  EXPECT_STREQ("#(1 $a)", err_.message);
}

// SPEC §3.3: subclassResponsibility と shouldNotImplement も値を返さずに中断する。
TEST_F(FailureAbort, SubclassResponsibilityAborts) {
  EXPECT_EQ(AO_ERR_EVAL, print("Object new subclassResponsibility"));
  EXPECT_STREQ("subclassResponsibility", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("Object new shouldNotImplement"));
  EXPECT_STREQ("shouldNotImplement", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("Object class new"));
  EXPECT_STREQ("shouldNotImplement", err_.message);
}

// 06 Medium: ネイティブの範囲外エラーや 0 除算は、エラーの文字列を値として返さない。
TEST_F(FailureAbort, NativeErrorsAbortWithTheirMessage) {
  EXPECT_EQ(AO_ERR_EVAL, print("1/0"));
  EXPECT_STREQ("division by zero", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("(Object new instVarAt: 9) class"));
  EXPECT_STREQ("instVarAt: index out of range", err_.message);
  EXPECT_EQ(AO_ERR_EVAL, print("((Array new: 2) at: 5) class"));
  EXPECT_STRNE("", err_.message);
}

// 02 High: 失敗した引数は、コレクションに 0（空 OOP）として残らない。
TEST_F(FailureAbort, FailedArgumentLeavesNothingInSet) {
  ASSERT_EQ(AO_OK, doIt("s := Set new")) << err_.message;
  EXPECT_EQ(AO_ERR_EVAL, doIt("s add: ([:x | x] value)"));
  EXPECT_STRNE("", err_.message);
  EXPECT_EQ("0", printed("s size"));
  ASSERT_EQ(AO_OK, doIt("d := Dictionary new")) << err_.message;
  EXPECT_EQ(AO_ERR_EVAL, doIt("d at: ([:x | x] value) put: 1"));
  EXPECT_EQ("0", printed("d size"));
}

// ブロックを呼ぶネイティブも、ブロックの失敗を要素にしない。
TEST_F(FailureAbort, CollectStopsOnFailedBlock) {
  EXPECT_EQ(AO_ERR_EVAL, doIt("c := #(1 2) collect: [:a :b | a]"));
  EXPECT_STRNE("", err_.message);
  EXPECT_EQ("true", printed("c isNil"));
}

// SPEC §3.4: abort の途中の ensure: は走り、abort のあともセッションは次の評価に使える。
TEST_F(FailureAbort, EnsureRunsDuringAbortAndSessionContinues) {
  ASSERT_EQ(AO_OK, doIt("log := OrderedCollection new")) << err_.message;
  EXPECT_EQ(AO_ERR_EVAL, doIt("[log add: #body. nil foo. log add: #late] ensure: [log add: #cleanup]."
                              " log add: #after"));
  EXPECT_STREQ("doesNotUnderstand: #foo", err_.message);
  EXPECT_EQ("2", printed("log size"));
  EXPECT_EQ("true", printed("(log at: 1) == #body"));
  EXPECT_EQ("true", printed("(log at: 2) == #cleanup"));
  EXPECT_EQ("3", printed("1 + 2"));
  EXPECT_STREQ("", err_.message);
}

// SPEC §3.4: 実行時に組み立てた理由は、後始末の割り当てで GC が走っても壊れない。
TEST_F(FailureAbort, DynamicReasonSurvivesCleanupAllocations) {
  EXPECT_EQ(AO_ERR_EVAL,
            doIt("[nil error: 'boom-survives'] ensure: [1 to: 4000 do: [:i | Array new: 100]]"));
  EXPECT_STREQ("boom-survives", err_.message);
  EXPECT_EQ(AO_ERR_EVAL,
            doIt("[nil error: 123456789] ensure: [1 to: 4000 do: [:i | Array new: 100]]"));
  EXPECT_STREQ("123456789", err_.message);
}

// SPEC §3.4: 理由のヒープ文字列は、最外で読むまでルートに載っている。
TEST(FailureAbortBoot, DynamicReasonSurvivesCollections) {
  Boot b;
  EXPECT_TRUE(runDoIt(b, "^nil error: 'kept across collections'").isEmpty());
  ASSERT_TRUE(b.ctx.aborting);
  ao::Gc gc(b.heap, b.roots);
  for (int i = 0; i < 4; ++i) {
    for (int k = 0; k < 2000; ++k) {
      (void)ao::allocateRetry(b.ctx, b.wk.arrayClass, 64, 0);
    }
    gc.collectNursery();
    gc.collectOld();
  }
  EXPECT_EQ("kept across collections", ao::abortReasonText(b.ctx));
  ao::clearUnwinding(b.ctx);
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_EQ("", ao::abortReasonText(b.ctx));
}

// SPEC §3.4: 固定の理由は割り当てなしで入り、そのまま読める。
TEST(FailureAbortBoot, StaticReasonNeedsNoAllocation) {
  Boot b;
  EXPECT_TRUE(ao::abortEvaluation(b.ctx, "out of memory").isEmpty());
  EXPECT_EQ("out of memory", ao::abortReasonText(b.ctx));
  // すでに abort の最中なら、最初の理由を保つ。
  EXPECT_TRUE(ao::abortEvaluation(b.ctx, std::string("later")).isEmpty());
  EXPECT_EQ("out of memory", ao::abortReasonText(b.ctx));
  ao::clearUnwinding(b.ctx);
}

// SPEC §3.3 手順 4: Object>>doesNotUnderstand: の既定は Message を返さず、評価を中断する。
TEST(FailureAbortBoot, DefaultDoesNotUnderstandAborts) {
  Boot b;
  EXPECT_TRUE(send1(b, ao::Oop::fromSmallInteger(3), "zork:", ao::Oop::nil()).isEmpty());
  EXPECT_FALSE(b.ctx.nonlocalReturn);
  EXPECT_EQ("doesNotUnderstand: #zork:", takeAbortReason(b));
}

// 02 High: 空 OOP へ送っても classOf の nil で探索せず、評価を中断する。
TEST(FailureAbortBoot, SendToEmptyReceiverAborts) {
  Boot b;
  EXPECT_TRUE(send0(b, ao::Oop{}, "class").isEmpty());
  ASSERT_TRUE(b.ctx.aborting);
  EXPECT_EQ("failed: #class", ao::abortReasonText(b.ctx));
  ao::clearUnwinding(b.ctx);
}

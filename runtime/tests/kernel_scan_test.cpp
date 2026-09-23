#include "test_support.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Oop.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/kernel/Install.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

bool nativeRequiredDictsAreNative(Boot& b) {
  std::vector<std::string> bad;
  struct Scan {
    Boot* b;
    std::vector<std::string>* bad;
    void visit(ao::Oop cls) {
      if (!cls.isHeap()) return;
      auto dict = b->heap.slotAt(cls, ao::kClassSlotMethodDict);
      if (!dict.isHeap()) return;
      auto inner = b->heap.slotAt(dict, ao::kDictSlotArray);
      if (!inner.isHeap()) return;
      for (std::uint32_t i = 0; i + 1 < b->heap.size(inner); i += 2) {
        auto k = b->heap.slotAt(inner, i);
        auto v = b->heap.slotAt(inner, i + 1);
        if (k.isNil()) continue;
        if (!v.isHeap() || b->heap.klass(v) != b->wk.nativeMethodClass) {
          bad->push_back("non-native");
        }
      }
    }
  } scan{&b, &bad};
  b.wk.eachNativeRequiredClass(
      [](void* p, ao::Oop cls) {
        auto* s = static_cast<Scan*>(p);
        s->visit(cls);
        s->visit(s->b->heap.klass(cls));
      },
      &scan);
  return bad.empty();
}

}  // namespace

TEST(KernelScan, MethodDictionaryValuesAreNativeMethods) {
  Boot b;
  EXPECT_TRUE(nativeRequiredDictsAreNative(b));
}

TEST(KernelScan, UserCompiledMethodDoesNotFailScan) {
  Boot b;
  const char* src =
      "!Object subclass: #NotKernel\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P6b-Test'!\n"
      "!NotKernel methodsFor: 't'!\n"
      "ok\n"
      "  ^1!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs));
  auto cls = b.wk.named("NotKernel");
  auto meth = ao::lookup(b.heap, cls, b.wk.intern("ok"));
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(meth));
  EXPECT_TRUE(nativeRequiredDictsAreNative(b));
}

TEST(KernelScan, RequiredSelectorsAreNativeMethods) {
  Boot b;
  struct Req {
    ao::Oop cls;
    const char* sel;
  };
  const Req reqs[] = {
      {b.wk.objectClass, "isNil"},
      {b.wk.objectClass, "=="},
      {b.wk.objectClass, "class"},
      {b.wk.trueClass, "ifTrue:ifFalse:"},
      {b.wk.smallIntegerClass, "+"},
      {b.wk.arrayClass, "at:"},
      {b.wk.objectClass, "mustBeBoolean"},
      {b.wk.smallIntegerClass, "timesRepeat:"},
      {b.wk.blockContextClass, "value:value:value:"},
      {b.wk.blockContextClass, "value:value:value:value:"},
      {b.wk.blockContextClass, "numArgs"},
      {b.wk.blockContextClass, "whileTrue:"},
      {b.wk.blockContextClass, "whileFalse:"},
      {b.wk.blockContextClass, "whileTrue"},
      {b.wk.blockContextClass, "whileFalse"},
      {b.wk.blockContextClass, "repeat"},
      {b.wk.blockContextClass, "ensure:"},
      {b.wk.blockContextClass, "ifCurtailed:"},
      {b.wk.blockContextClass, "cannotReturn:"},
  };
  for (const auto& r : reqs) {
    auto sel = ao::Symbol::intern(b.wk, r.sel);
    auto meth = ao::lookup(b.heap, r.cls, sel);
    EXPECT_TRUE(meth.isHeap()) << r.sel;
    if (meth.isHeap()) {
      EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(meth)) << r.sel;
    }
  }
}

// SPEC §3.5: SendSpecial が SmallInteger 同士で送信を省く 8 セレクタは、SmallInteger から引くと
// どれもネイティブに当たる。省いても答えが送信と同じであることの前提。
TEST(KernelScan, SmallIntegerSpecialSelectorsResolveToNatives) {
  Boot b;
  for (const char* sel : {"+", "-", "*", "<", ">", "<=", ">=", "="}) {
    const ao::Oop meth = ao::lookup(b.heap, b.wk.smallIntegerClass, b.wk.intern(sel));
    ASSERT_TRUE(meth.isHeap()) << sel;
    EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(meth)) << sel;
  }
}

TEST(KernelBench, TenMillionToDo) {
  Boot b;
  auto body = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop* args, std::uint32_t) {
    ao::Oop one = ao::Oop::fromSmallInteger(1);
    return ao::send(ctx, args[0], ctx.wk.intern("+"), &one, 1, nullptr);
  };
  auto blk = ao::makeNativeBlock(b.ctx, body, 1);
  auto start = std::chrono::steady_clock::now();
  ao::Oop args[2] = {ao::Oop::fromSmallInteger(10000000), blk};
  ao::send(b.ctx, ao::Oop::fromSmallInteger(1), ao::Symbol::intern(b.wk, "to:do:"), args, 2, nullptr);
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
  EXPECT_LT(ms, 30000);
  std::printf("P4 to:do: 10000000 native %lld ms\n", static_cast<long long>(ms));
  EXPECT_EQ(0u, b.ctx.interpretedBytecodes);
}

// SPEC §3.5: ユーザーメソッドの展開した to:do:。1 反復は 13 命令（判定 4、本体 4、増分 5）で、
// SmallInteger の <= と + は送信しない。命令数は、前置き 5（1 を積んで複製、ループ変数と上限を
// 置く）、最後の判定 4、ReturnTop 1 を足した 13N + 10 である。
TEST(KernelBench, InlinedToDoMillion) {
  Boot b;
  constexpr std::int64_t kN = 1000000;
  const char* src =
      "!Object subclass: #B3Bench\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B3-Test'!\n"
      "!B3Bench methodsFor: 'bench'!\n"
      "loop\n"
      "  ^1 to: 1000000 do: [:i | i + 1]! !\n";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInString(b.ctx, src, errs));
  ASSERT_TRUE(errs.empty()) << errs[0].message;
  ao::Root inst(b.roots, send0(b, b.wk.named("B3Bench"), "new"));
  ASSERT_TRUE(inst.slot.isHeap());
  const std::uint64_t bytecodes = b.ctx.interpretedBytecodes;
  const std::uint64_t sends = b.ctx.interpretedSends;
  const auto start = std::chrono::steady_clock::now();
  const ao::Oop got = send0(b, inst.slot, "loop");
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
  std::printf("B3 inlined to:do: %lld compiled %lld ms\n", static_cast<long long>(kN),
              static_cast<long long>(ms));
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(1, got.smallIntegerValue());
  EXPECT_EQ(static_cast<std::uint64_t>(13 * kN + 10), b.ctx.interpretedBytecodes - bytecodes);
  EXPECT_EQ(0u, b.ctx.interpretedSends - sends);
  EXPECT_LT(ms, 30000);
}

// SPEC §3.10: ロードしたイメージに無い Kernel ネイティブだけを足し、あるものは上書きしない。
TEST(KernelInstall, InstallMissingAddsAbsentAndKeepsPresent) {
  Boot b;
  const ao::Oop dict = b.heap.slotAt(b.wk.blockContextClass, ao::kClassSlotMethodDict);
  const ao::Oop numArgs = b.wk.intern("numArgs");
  const ao::Oop whileTrue = b.wk.intern("whileTrue:");
  const ao::Oop marker = b.wk.intern("r2Marker");
  // 古いイメージを真似る: numArgs は無く、whileTrue: はネイティブ以外が入っている。
  ASSERT_TRUE(ao::MethodDictionary::atPut(b.heap, dict, numArgs, ao::Oop::nil()));
  ASSERT_TRUE(ao::MethodDictionary::atPut(b.heap, dict, whileTrue, marker));
  ao::kernel::installMissing(b.heap, b.roots, b.wk);
  const ao::Oop added = ao::MethodDictionary::at(b.heap, dict, numArgs);
  ASSERT_TRUE(added.isHeap());
  EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(added));
  EXPECT_EQ(marker, ao::MethodDictionary::at(b.heap, dict, whileTrue));
  // installMissing のあとも、ふつうの putNative は上書きする。
  ASSERT_TRUE(ao::kernel::putNative(b.heap, b.wk, b.wk.blockContextClass, "whileTrue:", 1,
                                    "ao_BlockContext_whileTrue_", ao::ao_BlockContext_whileTrue_));
  EXPECT_EQ(b.wk.nativeMethodClass,
            b.heap.klass(ao::MethodDictionary::at(b.heap, dict, whileTrue)));
}

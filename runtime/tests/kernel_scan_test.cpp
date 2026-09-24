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
      {b.wk.smalltalkImageClass, "at:"},
      {b.wk.smalltalkImageClass, "at:put:"},
      {b.wk.smalltalkImageClass, "at:ifAbsent:"},
      {b.wk.smalltalkImageClass, "includesKey:"},
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
  ao::kernel::installMissing(b.heap, b.roots, b.wk, &b.cache);
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

namespace {

ao::Oop answerOne(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(1);
}

ao::Oop answerTwo(ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
  return ao::Oop::fromSmallInteger(2);
}

// file-in のクラス定義（applyClassDef → subclass:...）で name を定義し、束縛されたクラスを返す。
ao::Oop fileInEmptyClass(Boot& b, const std::string& name) {
  std::vector<ao::compiler::CompileError> errs;
  const std::string src = "!Object subclass: #" + name +
                          "\n"
                          "  instanceVariableNames: ''\n"
                          "  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n"
                          "  category: 'B3-Test'!\n";
  EXPECT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
  return b.wk.named(name);
}

bool cacheHoldsClass(const Boot& b, ao::Oop cls) {
  for (const auto& e : b.cache.entries) {
    if (e.klass == cls) {
      return true;
    }
  }
  return false;
}

}  // namespace

// SPEC §3.3 キャッシュの無効化: flushSelector はそのセレクタのエントリを受信側のクラスによらず捨て、
// 他のセレクタのエントリは残す。flushAll はすべて捨てる。
TEST(MethodCacheInvalidation, FlushSelectorDropsEveryClassAndKeepsOtherSelectors) {
  Boot b;
  ao::Root printString(b.roots, b.wk.intern("printString"));
  ao::Root yourself(b.roots, b.wk.intern("yourself"));
  ao::Root printer(b.roots, ao::lookup(b.heap, b.wk.objectClass, printString.slot));
  ao::Root self(b.roots, ao::lookup(b.heap, b.wk.objectClass, yourself.slot));
  ASSERT_TRUE(printer.slot.isHeap());
  ASSERT_TRUE(self.slot.isHeap());
  b.cache.insert(b.heap, b.wk.objectClass, printString.slot, printer.slot);
  b.cache.insert(b.heap, b.wk.smallIntegerClass, printString.slot, printer.slot);
  b.cache.insert(b.heap, b.wk.trueClass, yourself.slot, self.slot);
  // 3 件が別の行に入っていること（衝突すると後の insert が前を消し、下の検査が意味を失う）。
  ASSERT_EQ(printer.slot, b.cache.probe(b.heap, b.wk.objectClass, printString.slot));
  ASSERT_EQ(printer.slot, b.cache.probe(b.heap, b.wk.smallIntegerClass, printString.slot));
  ASSERT_EQ(self.slot, b.cache.probe(b.heap, b.wk.trueClass, yourself.slot));
  b.cache.flushSelector(printString.slot);
  EXPECT_FALSE(b.cache.probe(b.heap, b.wk.objectClass, printString.slot).isHeap());
  EXPECT_FALSE(b.cache.probe(b.heap, b.wk.smallIntegerClass, printString.slot).isHeap());
  EXPECT_EQ(self.slot, b.cache.probe(b.heap, b.wk.trueClass, yourself.slot));
  b.cache.flushAll();
  EXPECT_FALSE(b.cache.probe(b.heap, b.wk.trueClass, yourself.slot).isHeap());
}

// SPEC §3.3: putNative による置換も、キャッシュ済みの送信に届く。定義クラス（Object）ではなく、
// サブクラスのレシーバ（3）で入ったエントリも捨てる。
TEST(MethodCacheInvalidation, PutNativeReplacementReachesCachedSend) {
  Boot b;
  const ao::Oop three = ao::Oop::fromSmallInteger(3);
  ASSERT_TRUE(ao::kernel::putNative(b.heap, b.wk, &b.cache, b.wk.objectClass, "b3CacheProbe", 0,
                                    "ao_MethodCacheProbe_one", answerOne));
  ao::Oop got = send0(b, three, "b3CacheProbe");
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(1, got.smallIntegerValue());
  ASSERT_TRUE(ao::kernel::putNative(b.heap, b.wk, &b.cache, b.wk.objectClass, "b3CacheProbe", 0,
                                    "ao_MethodCacheProbe_two", answerTwo));
  got = send0(b, three, "b3CacheProbe");
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(2, got.smallIntegerValue());
}

// SPEC §3.3 / §3.10: installMissing（イメージのロード後の ensureKernelNatives）が足したネイティブは、
// それまでスーパークラスのメソッドに解決してキャッシュしていた送信にも届く。
TEST(MethodCacheInvalidation, InstallMissingReachesCachedSend) {
  Boot b;
  const ao::Oop dict = b.heap.slotAt(b.wk.trueClass, ao::kClassSlotMethodDict);
  // 古いイメージを真似る: True>>printString が無く、Object>>printString（クラス名）が答える。
  ASSERT_TRUE(ao::MethodDictionary::atPut(b.heap, dict, b.wk.intern("printString"),
                                          ao::Oop::nil()));
  ao::Oop got = send0(b, ao::Oop::true_(), "printString");
  ASSERT_TRUE(got.isHeap());
  EXPECT_EQ("True", ao::Str::toUtf8(b.heap, got));
  ao::kernel::installMissing(b.heap, b.roots, b.wk, &b.cache);
  got = send0(b, ao::Oop::true_(), "printString");
  ASSERT_TRUE(got.isHeap());
  EXPECT_EQ("true", ao::Str::toUtf8(b.heap, got));
}

// SPEC §3.3: 既存の名前へのクラス定義（クラスの差し替え）はキャッシュ全体を捨てる。古いクラスと
// そのメタクラスのエントリは残らず、古いクラスで送信が当たらない。
TEST(MethodCacheInvalidation, ClassRedefinitionDropsOldClassEntries) {
  Boot b;
  ao::Root old(b.roots, fileInEmptyClass(b, "B3CacheReplaced"));
  ASSERT_TRUE(old.slot.isHeap());
  ao::Root inst(b.roots, send0(b, old.slot, "new"));
  ASSERT_TRUE(inst.slot.isHeap());
  ASSERT_TRUE(send0(b, inst.slot, "printString").isHeap());
  ASSERT_TRUE(cacheHoldsClass(b, old.slot));
  ASSERT_TRUE(cacheHoldsClass(b, b.heap.klass(old.slot)));
  ao::Root replacement(b.roots, fileInEmptyClass(b, "B3CacheReplaced"));
  ASSERT_TRUE(replacement.slot.isHeap());
  ASSERT_NE(old.slot, replacement.slot);
  EXPECT_FALSE(cacheHoldsClass(b, old.slot));
  EXPECT_FALSE(cacheHoldsClass(b, b.heap.klass(old.slot)));
  const ao::Oop fresh = send0(b, replacement.slot, "new");
  ASSERT_TRUE(fresh.isHeap());
  EXPECT_EQ(replacement.slot, b.heap.klass(fresh));
}

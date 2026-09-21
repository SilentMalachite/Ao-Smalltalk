#include "test_support.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Oop.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(KernelScan, MethodDictionaryValuesAreNativeMethods) {
  Boot b;
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
  b.wk.eachClass(
      [](void* p, ao::Oop cls) {
        auto* s = static_cast<Scan*>(p);
        s->visit(cls);
        s->visit(s->b->heap.klass(cls));
      },
      &scan);
  EXPECT_TRUE(bad.empty());
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

TEST(KernelBench, TenMillionToDo) {
  Boot b;
  auto body = [](ao::CallContext& ctx, ao::Oop, const ao::Oop* args, std::uint32_t) {
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

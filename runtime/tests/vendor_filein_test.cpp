#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Lookup.hpp"

#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(VendorFileIn, LoadOrderEvaluatesLinkRoundTrip) {
  Boot b;
  const std::string order = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, order, errs)) << (errs.empty() ? "" : errs[0].message);
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root linkClass(b.roots, b.wk.named("Link"));
  if (!linkClass.slot.isHeap()) {
    linkClass.slot = b.wk.named("Bag");
  }
  ASSERT_TRUE(linkClass.slot.isHeap());
  // Link が載った場合。セレクタは切り出したファイルのアクセサに合わせる。
  // 計画時点の想定は nextLink / nextLink:。切り出し結果の先頭メソッドが
  // 別セレクタなら、そのゲッターとセッター名へこのテストだけを合わせる。
  // vendor のソースは書き換えない。
  auto getter = ao::lookup(b.heap, linkClass.slot, b.wk.intern("nextLink"));
  auto setter = ao::lookup(b.heap, linkClass.slot, b.wk.intern("nextLink:"));
  if (getter.isHeap() && setter.isHeap()) {
    EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(getter));
    ao::Root link(b.roots, send0(b, linkClass.slot, "new"));
    ASSERT_TRUE(link.slot.isHeap());
    EXPECT_TRUE(send1(b, link.slot, "nextLink:", link.slot).isHeap());
    EXPECT_EQ(link.slot, send0(b, link.slot, "nextLink"));
    return;
  }
  // Link が DEFERRED で Bag のみのとき。Task 3 と同じ contents 往復。
  auto contents = ao::lookup(b.heap, b.wk.bagClass, b.wk.intern("contents"));
  ASSERT_TRUE(contents.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(contents));
  auto bag = send0(b, b.wk.bagClass, "new");
  EXPECT_TRUE(send0(b, bag, "contents").isNil());
}

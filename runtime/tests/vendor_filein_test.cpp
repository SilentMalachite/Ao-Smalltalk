#include "test_support.hpp"

#include "ao/ClassPool.hpp"
#include "ao/Compile.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Lookup.hpp"
#include "ao/Natives.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <set>
#include <string>
#include <system_error>
#include <vector>

TEST(VendorFileIn, LoadOrderEvaluatesLinkRoundTrip) {
  Boot b;
  const std::string order = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  std::vector<ao::FileInError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, order, errs)) << (errs.empty() ? "" : errs[0].error.message);
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

// SPEC §3.12: vendor の file-in で起きるメソッド単位のエラーの集合は、DEFERRED.md に列挙した集合と
// 一致する。列挙したのに載るメソッドも、列挙していないのに失敗するメソッドも、このテストの失敗である。
TEST(VendorFileIn, MethodErrorsAreExactlyTheDeferredOnes) {
  Boot b;
  const std::filesystem::path dir = std::filesystem::path(AO_SOURCE_DIR) / "image" / "vendor";
  std::vector<ao::FileInError> errs;
  std::vector<ao::FileInError> deferred;
  const bool ok = ao::fileInLoadOrder(b.ctx, dir / "LOAD_ORDER", errs, &deferred);
  for (const auto& e : errs) {
    ADD_FAILURE() << "not deferred: " << e.file << ":" << e.error.span.start << "-"
                  << e.error.span.end << ": " << e.method << ": " << e.error.message;
  }
  EXPECT_TRUE(ok);
  std::set<std::string> failed;
  for (const auto& e : deferred) {
    EXPECT_FALSE(e.file.empty()) << e.method;
    failed.insert(e.method);
  }
  const std::vector<std::string> listedInOrder = ao::deferredMethods(dir / "DEFERRED.md");
  const std::set<std::string> listed(listedInOrder.begin(), listedInOrder.end());
  EXPECT_FALSE(listed.empty());
  for (const auto& m : listed) {
    EXPECT_EQ(1u, failed.count(m)) << "listed in DEFERRED.md but filed in: " << m;
  }
  for (const auto& m : failed) {
    EXPECT_EQ(1u, listed.count(m)) << "failed but not listed: " << m;
  }
  EXPECT_FALSE(b.ctx.aborting);
}

// B4 (docs/claude-review/05 High) / SPEC §3.6: vendor の Time は classVariableNames:
// 'MillisecondClockOffset' を持つ。クラス側の releaseClassCachedState（MillisecondClockOffset := nil）
// はコンパイルされて載り、classPool の束縛に書く。
TEST(VendorFileIn, TimeClassVariableMethodIsFiledIn) {
  Boot b;
  const std::string order = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  std::vector<ao::FileInError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, order, errs)) << (errs.empty() ? "" : errs[0].error.message);
  ao::Root time(b.roots, b.wk.named("Time"));
  ASSERT_TRUE(ao::isClassShaped(b.heap, time.slot));
  const ao::Oop release =
      ao::lookup(b.heap, b.heap.klass(time.slot), b.wk.intern("releaseClassCachedState"));
  ASSERT_TRUE(release.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(release));
  ao::Root binding(b.roots, ao::ClassPool::bindingAt(
                                b.heap, b.heap.slotAt(time.slot, ao::kClassSlotClassPool),
                                "MillisecondClockOffset"));
  ASSERT_TRUE(binding.slot.isHeap());
  b.heap.slotAtPut(binding.slot, ao::kAssocValue, ao::Oop::fromSmallInteger(5));
  send0(b, time.slot, "releaseClassCachedState");
  EXPECT_FALSE(b.ctx.aborting);
  EXPECT_TRUE(b.heap.slotAt(binding.slot, ao::kAssocValue).isNil());
}

namespace {

// LOAD_ORDER を置くディレクトリ。作り直し、テストの終わりに消す。
struct LoadOrderDir {
  std::filesystem::path path;
  explicit LoadOrderDir(const char* name)
      : path(std::filesystem::temp_directory_path() / name) {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    std::filesystem::create_directories(path);
  }
  ~LoadOrderDir() {
    std::error_code ignore;
    std::filesystem::remove_all(path, ignore);
  }
  bool write(const char* file, const char* body) const {
    std::ofstream out(path / file, std::ios::binary);
    out << body;
    return static_cast<bool>(out);
  }
  std::string file(const char* name) const { return (path / name).string(); }
};

// インスタンス側に 2 つ、クラス側に 1 つ、コンパイルできないメソッドがある。good は載る。
constexpr const char* kWithErrors =
    "!Object subclass: #B3FileIn\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'B3-Test'!\n"
    "\n"
    "!B3FileIn methodsFor: 'probes'!\n"
    "bad\n"
    "  ^1 +!\n"
    "at: i put: v\n"
    "  ^1 +!\n"
    "good\n"
    "  ^3! !\n"
    "\n"
    "!B3FileIn class methodsFor: 'probes'!\n"
    "worse\n"
    "  ^1 +! !\n";

// 前のファイルにエラーがあっても、次のファイルは読まれる。
constexpr const char* kLater =
    "!B3FileIn methodsFor: 'probes'!\n"
    "later\n"
    "  ^4! !\n";

std::set<std::string> methodsOf(const std::vector<ao::FileInError>& errors) {
  std::set<std::string> out;
  for (const auto& e : errors) {
    out.insert(e.method);
  }
  return out;
}

bool installed(Boot& b, const char* selector) {
  const ao::Oop cls = b.wk.named("B3FileIn");
  return cls.isHeap() && ao::lookup(b.heap, cls, b.wk.intern(selector)).isHeap();
}

}  // namespace

// SPEC §3.12: DEFERRED に無いメソッドのエラーが 1 件でもあれば失敗。エラーにはファイル名と
// メソッドを付ける。メソッド単位のエラーは、残りのチャンクも次のファイルも止めない。
TEST(FileInLoadOrder, UndeferredMethodErrorsFailWithFileNames) {
  const LoadOrderDir dir("ao-filein-undeferred");
  ASSERT_TRUE(dir.write("LOAD_ORDER", "a.st\nb.st\n"));
  ASSERT_TRUE(dir.write("a.st", kWithErrors));
  ASSERT_TRUE(dir.write("b.st", kLater));
  Boot b;
  std::vector<ao::FileInError> errs;
  std::vector<ao::FileInError> deferred;
  EXPECT_FALSE(ao::fileInLoadOrder(b.ctx, dir.path / "LOAD_ORDER", errs, &deferred));
  EXPECT_TRUE(deferred.empty());
  ASSERT_EQ(3u, errs.size());
  for (const auto& e : errs) {
    EXPECT_EQ(dir.file("a.st"), e.file);
    EXPECT_FALSE(e.error.message.empty());
    EXPECT_LT(0u, e.error.span.start) << e.method;
  }
  const std::set<std::string> expected{"B3FileIn>>bad", "B3FileIn>>at:put:",
                                       "B3FileIn class>>worse"};
  EXPECT_EQ(expected, methodsOf(errs));
  EXPECT_TRUE(installed(b, "good"));
  EXPECT_TRUE(installed(b, "later"));
}

// SPEC §3.12: DEFERRED.md に列挙したメソッドのエラーは数えず、報告もしない。形式外の行は注記。
TEST(FileInLoadOrder, DeferredMethodErrorsAreNotCounted) {
  const LoadOrderDir dir("ao-filein-deferred");
  ASSERT_TRUE(dir.write("LOAD_ORDER", "a.st\nb.st\n"));
  ASSERT_TRUE(dir.write("a.st", kWithErrors));
  ASSERT_TRUE(dir.write("b.st", kLater));
  ASSERT_TRUE(dir.write("DEFERRED.md",
                        "# Deferred\n"
                        "\n"
                        "B3FileIn: not a method, a note\n"
                        "B3FileIn>>bad: 理由\n"
                        "B3FileIn>>at:put:: 理由\n"
                        "B3FileIn class>>worse: 理由\n"));
  Boot b;
  std::vector<ao::FileInError> errs;
  std::vector<ao::FileInError> deferred;
  EXPECT_TRUE(ao::fileInLoadOrder(b.ctx, dir.path / "LOAD_ORDER", errs, &deferred));
  EXPECT_TRUE(errs.empty()) << (errs.empty() ? "" : errs[0].method + ": " + errs[0].error.message);
  const std::set<std::string> expected{"B3FileIn>>bad", "B3FileIn>>at:put:",
                                       "B3FileIn class>>worse"};
  EXPECT_EQ(expected, methodsOf(deferred));
  EXPECT_TRUE(installed(b, "later"));
}

// 列挙が一部なら、残りのエラーだけが数えられて失敗になる。
TEST(FileInLoadOrder, PartlyDeferredStillFails) {
  const LoadOrderDir dir("ao-filein-partly-deferred");
  ASSERT_TRUE(dir.write("LOAD_ORDER", "a.st\n"));
  ASSERT_TRUE(dir.write("a.st", kWithErrors));
  ASSERT_TRUE(dir.write("DEFERRED.md", "B3FileIn>>bad: 理由\nB3FileIn class>>worse: 理由\n"));
  Boot b;
  std::vector<ao::FileInError> errs;
  EXPECT_FALSE(ao::fileInLoadOrder(b.ctx, dir.path / "LOAD_ORDER", errs));
  ASSERT_EQ(1u, errs.size());
  EXPECT_EQ("B3FileIn>>at:put:", errs[0].method);
  EXPECT_EQ(dir.file("a.st"), errs[0].file);
}

// SPEC §3.12: LOAD_ORDER に書いたファイルが読めなければ失敗。エラーはそのファイルの名前を持つ。
TEST(FileInLoadOrder, UnreadableListedFileFails) {
  const LoadOrderDir dir("ao-filein-unreadable");
  ASSERT_TRUE(dir.write("LOAD_ORDER", "b0.st\nmissing.st\n"));
  ASSERT_TRUE(dir.write("b0.st", "!Object subclass: #B3FileInFirst\n"
                                 "  instanceVariableNames: ''\n"
                                 "  classVariableNames: ''\n"
                                 "  poolDictionaries: ''\n"
                                 "  category: 'B3-Test'!\n"));
  Boot b;
  std::vector<ao::FileInError> errs;
  EXPECT_FALSE(ao::fileInLoadOrder(b.ctx, dir.path / "LOAD_ORDER", errs));
  ASSERT_EQ(1u, errs.size());
  EXPECT_EQ(dir.file("missing.st"), errs[0].file);
  EXPECT_TRUE(errs[0].method.empty());
  EXPECT_EQ(0u, errs[0].error.message.rfind("cannot read", 0)) << errs[0].error.message;
  EXPECT_TRUE(b.wk.named("B3FileInFirst").isHeap());
}

TEST(FileInLoadOrder, UnreadableLoadOrderFails) {
  Boot b;
  std::vector<ao::FileInError> errs;
  EXPECT_FALSE(ao::fileInLoadOrder(b.ctx, "/no/such/LOAD_ORDER", errs));
  ASSERT_EQ(1u, errs.size());
  EXPECT_EQ("/no/such/LOAD_ORDER", errs[0].file);
}

// SPEC §3.12 の DEFERRED.md の形式: `Class>>selector: 理由`（クラス側は `Class class>>selector: 理由`）。
// キーワードセレクタは `Class>>at:put:: 理由` と書く。理由の無い行や他の行は注記である。
TEST(FileInLoadOrder, DeferredMethodsReadsOnlyListingLines) {
  const LoadOrderDir dir("ao-filein-deferred-format");
  ASSERT_TRUE(dir.write("DEFERRED.md",
                        "# Deferred\n"
                        "MappedCollection: does not occur in the pinned sources\n"
                        "Bag size は P9 のゴールデンに入らなかった\n"
                        "Bag>>sum:ifEmpty:: `,` が二項演算子にならない\n"
                        "Time class>>releaseClassCachedState: クラス変数に代入できない\r\n"
                        "Exception>>description: `,`\n"
                        "  Heap>>indent: 行頭の空白は形式外\n"
                        "Heap>>noReason:\n"
                        "Heap>>noReason2: \n"
                        "Heap >>spaced: 形式外\n"
                        "Heap class >>spaced: 形式外\n"
                        ">>anonymous: 形式外\n"
                        "- Heap>>bullet: 形式外\n"));
  const std::vector<std::string> expected{"Bag>>sum:ifEmpty:", "Time class>>releaseClassCachedState",
                                          "Exception>>description"};
  EXPECT_EQ(expected, ao::deferredMethods(dir.path / "DEFERRED.md"));
  EXPECT_TRUE(ao::deferredMethods(dir.path / "NO_SUCH.md").empty());
}

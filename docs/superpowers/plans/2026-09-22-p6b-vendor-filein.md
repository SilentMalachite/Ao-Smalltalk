# P6b vendor file-in Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** `image/vendor` にピンした Cuis の非 Kernel クラスを LOAD_ORDER どおり CompiledMethod として載せ、そのうち少なくとも 1 メソッドを評価する。

**Architecture:** Cuis の巨大な `.changes` から許可リストのクラスだけを最後の定義が勝つ形で切り出す。既存の `parseChunks` / `applyChunks` で載せる。カタログに既にあるスタブ（Bag など）は `WellKnown` のスロットと `Smalltalk` グローバルを新しいクラスへ付け替える。ネイティブ必須クラスへの file-in と、既に NativeMethod があるセレクタの上書きは拒否する。DoIt は評価しない。

**Tech Stack:** C++20、GoogleTest、既存 `ao_compiler` のチャンクパーサ、Python は使わない。新規ライブラリは足さない。

**Spec:** `SPEC.md` §3.12、§6「image/vendor から file-in した非 Kernel メソッド」「vendor 由来メソッドがネイティブ必須セレクタを上書きしていない」。フェーズ正本は `docs/phases/P6b.md`。PR 骨格は `docs/prs/P6b-01.md` … `P6b-04.md`。

## 結論

P6 は完了している（`PHASE` は `P6b`、HEAD `2c13462`）。次に作るのは vendor file-in だけである。インタプリタ、イメージ、AppKit には進まない。

## 前提

- チャンクの分類と `applyChunks` は既にある（`compiler/src/ChunkParser.cpp`、`runtime/src/Compile.cpp`）。`Class>>subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:` はネイティブ。`ao filein` はまだ無い（`runtime/src/ao_main.cpp` は `--help` / `--version` / `--test` だけ）。
- `WellKnown::define` は `extra_` にしか書かない。`named` は `kNamedClasses` を先に見る。`Globals::at` は起動時にコピーした `smalltalk` 配列を先に見る。だから `Magnitude subclass: #Date` は、今のままではブートストラップの Date を置き換えない。
- スタブ Bag / LinkedList / MappedCollection / Date / Time の instSize は 0。Cuis のメソッドはインスタンス変数を読む。メソッドを古いスタブに足すだけでは `PushInstVar` が合わない。クラスオブジェクト自体を付け替える。
- `KernelScan.MethodDictionaryValuesAreNativeMethods` は `eachClass` で extra のユーザクラスまで見る。vendor を載せると、このテストが CompiledMethod を Kernel 違反として落とす。走査対象を「ネイティブ必須カタログ」に狭める。これは SPEC §3.6（Bag 等はスタブ可、ネイティブ必須ではない）と §6 の Kernel 走査を同時に満たすための変更である。
- `Op::Primitive` はインタプリタで何もせず本文へ落ちる（`runtime/src/Interpreter.cpp` の `case Op::Primitive: break`）。プリミティブ番号の実装は足さない。
- Cuis のコア（Bag、Exception、Date）は `Packages/*.pck.st` には無い。正本はリポジトリ内の `CuisImage/Cuis7.9-8206.changes`。2026-09-22 時点の `master` は `2628e261acaf4577084f2e788e973d0269b44fc5`。ライセンスは同リポジトリの `LICENSE`（MIT。条文はコピーして変えない）。
- グラフの stamp は `8580cc53` で HEAD より 1 コミット古い。実装時は `GRAPH_REPORT.md` を読んだうえで、この計画に書いたファイルを Serena で開く。経路の再確認は `graphify query "applyChunks installMethod WellKnown define"`。

## 範囲

**やる**

- Cuis を上記コミットでピンし、`ORIGIN.md` と `NOTICE` とライセンス全文を置く。
- 許可リストのクラスだけを切り出して `image/vendor/cuis/` にコミットする。`.image` と `.changes` 全体はコミットしない。
- `LOAD_ORDER`、Kernel 上書き禁止、スタブの rebind、ホスト／Morphic 参照メソッドのパッチ、`ao filein`。
- 切り出した `Link>>nextLink` 相当（インスタンス変数の読み書き）を評価する。

**やらない**

- Morphic / MVC / BitBlt / Form、Monticello / Metacello / Iceberg、ネットワーク一式。
- `variableSubclass:` / `variableByteSubclass:` / `variableWordSubclass:` / `weakSubclass:` の実装。その形のクラスは `DEFERRED.md` に理由を書いて LOAD_ORDER から外す。
- プール辞書、クラス側インスタンス変数、DoIt / `initialize` の評価。
- FileStream の実ファイル API。ホストを触るメソッドは `self error: 'host file stream is not connected'` に置き換える。ブリッジの新エントリは足さない。
- 波括弧 `{ }` など、許可リストの切り出しがコンパイルできなくてもコンパイラを広げない。コンパイルできないメソッドは今の `applyMethodsFor` どおりスキップし、クラス作成自体がスーパーを解決できないものは DEFERRED にする。
- Kernel メソッドの Smalltalk 化、他処理系の VM ソースのコピー。

## 仕様

許可リスト（この順が初期順。Task 2 でスーパーが許可リスト内ならスーパーを前に安定ソートする）:

```
Link
LinkedList
Bag
MappedCollection
Heap
Exception
Error
Notification
Timespan
Date
Time
FileStream
```

スタブ allowlist（カタログに既にあり、rebind してよい名前）: `Bag` `LinkedList` `MappedCollection` `Date` `Time`。

ネイティブ必須: `kNamedClasses` のうちスタブ allowlist 以外。ここへは ClassDef も MethodsFor も拒否する。

上書き禁止: インストール先のメソッド辞書に、そのセレクタの NativeMethod が既にあるときはそのメソッドだけスキップする。スタブにネイティブが無いセレクタは CompiledMethod にしてよい。

ホストパッチの置換条件: メソッドソースが次の語を単語として含むとき、本文をエラー送信に置き換える。`primitive:` だけなら置き換えない（本文へ落ちるため）。

```
StandardFileStream FilePlugin DirectoryEntry Morph DisplayScreen Form BitBlt Canvas World HandMorph
```

`Form` は部分文字列で当てない。区切りは英数字以外。

切り出しの勝ちルール: `.changes` は時系列ログなので、クラス定義はクラス名ごとに最後の `subclass:` 定義、メソッドは `(className, meta, 先頭行のパターン)` ごとに最後の本文。

## Global Constraints

- 製品は `SPEC.md` の v1 だけ。フェーズを飛ばさない。`PHASE` が `P6b` の間は P7 のイメージを作らない。
- Kernel メソッドは C++ の NativeMethod。`.st` を実行定義にしない。
- 新規 C++ は Apache License 2.0。vendor の MIT 条文は改変しない。`NOTICE` に origin を列挙する。
- C++20。API 境界で例外を投げない。公開ポインタは `ao::Oop`。
- 依存を足さない。Apple Silicon / macOS。
- 識別子とコミットメッセージは英語。コミットは短い命令形に Graphify / Serena のトレーラを付ける。
- 実装は Graphify（`GRAPH_REPORT.md` と query）と Serena（`find_symbol` / `find_referencing_symbols`、本体は `replace_symbol_body`）を通す。新規ファイルは作成後に overview。
- テストが赤のまま次タスクへ進まない。
- 作業ブランチは `p6b-vendor`、worktree は `.worktrees/p6b-vendor`。ビルドはその worktree の `build/`。
- 承認後、この計画を `docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md` にも置く。

---

### Task 1: changes 切り出し

**Files:**

- Create: `runtime/include/ao/Vendor.hpp`
- Create: `runtime/src/VendorExtract.cpp`
- Modify: `runtime/CMakeLists.txt`（`src/VendorExtract.cpp` を `ao_runtime` に足す）
- Create: `runtime/tests/vendor_extract_test.cpp`
- Modify: `runtime/CMakeLists.txt`（テストソースを `ao_runtime_tests` に足す）

**Interfaces:**

- Consumes: `ao::compiler::parseChunks`（`compiler/include/ao/Chunk.hpp`）
- Produces:

```cpp
namespace ao {
inline bool isVendorStub(std::string_view name) {
  return name == "Bag" || name == "LinkedList" || name == "MappedCollection" ||
         name == "Date" || name == "Time";
}

struct VendorClassFile {
  std::string className;
  std::string superName;
  std::string chunkText;
  bool unsupportedShape = false;  // variableSubclass: 等
};

struct VendorExtractResult {
  std::vector<VendorClassFile> files;  // allowlist 順、deferred は含まない
  std::vector<std::string> notes;      // "Heap: variableSubclass" など
};

VendorExtractResult extractVendor(std::string_view changes,
                                  const std::vector<std::string>& allowlist);
}
```

- [ ] **Step 1: 失敗するテストを書く**

`runtime/tests/vendor_extract_test.cpp`:

```cpp
#include "ao/Vendor.hpp"
#include <gtest/gtest.h>

TEST(VendorExtract, LastDefinitionWinsAndDropsDoIt) {
  const char* src =
      "!Object subclass: #Link\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Old'!\n"
      "!Link methodsFor: 'accessing'!\n"
      "nextLink\n"
      "  ^nil!\n"
      "!Transcript show: 'ignore'! \n"
      "!Object subclass: #Link\n"
      "  instanceVariableNames: 'nextLink'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Collections-Support'!\n"
      "!Link methodsFor: 'accessing'!\n"
      "nextLink\n"
      "  ^nextLink!\n"
      "!Collection variableSubclass: #Heap\n"
      "  instanceVariableNames: 'array tally'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Collections-Unordered'!\n";
  auto r = ao::extractVendor(src, {"Link", "Heap", "Bag"});
  ASSERT_EQ(1u, r.files.size());
  EXPECT_EQ("Link", r.files[0].className);
  EXPECT_EQ("Object", r.files[0].superName);
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("instanceVariableNames: 'nextLink'"));
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("^nextLink"));
  EXPECT_EQ(std::string::npos, r.files[0].chunkText.find("^nil"));
  EXPECT_EQ(std::string::npos, r.files[0].chunkText.find("Transcript"));
  ASSERT_EQ(1u, r.notes.size());
  EXPECT_NE(std::string::npos, r.notes[0].find("Heap"));
  EXPECT_NE(std::string::npos, r.notes[0].find("variableSubclass:"));
}
```

- [ ] **Step 2: 赤であることを確認する**

Run: `cmake -S . -B build && cmake --build build --target ao_runtime_tests -j && ./build/runtime/ao_runtime_tests --gtest_filter=VendorExtract.LastDefinitionWinsAndDropsDoIt`

Expected: FAIL（`extractVendor` が未定義）

- [ ] **Step 3: 切り出しを実装する**

`Vendor.hpp` に上のインターフェースを置く。`VendorExtract.cpp` の手順:

1. `parseChunks` の結果を順に見る。
2. `ClassDef` で `className` が allowlist にあれば、そのクラスの最後の定義として保持する（superName, instVars, classVars, pools, category）。
3. `MethodsFor` で `className` が allowlist にあれば、本文の先頭非空行をキーにして最後の本文を保持する。キーは meta フラグを含む。
4. ソース全体を 1 行ずつではなく、次の正規表現相当で走査する。実装は手書きスキャンでよい。`variableSubclass:` `variableByteSubclass:` `variableWordSubclass:` `weakSubclass:` の直後の `#Name` が allowlist にあれば `notes` に `"<Name>: <keyword>"` を足し、そのクラスは `files` に出さない。
5. 出力チャンクは次の形に再構成する。元の stamp や `classDefinition:` ヘッダは捨てる。

```
!Object subclass: #Link
  instanceVariableNames: 'nextLink'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Collections-Support'!
!Link methodsFor: 'accessing'!
nextLink
  ^nextLink!
!Link class methodsFor: 'instance creation'!
...!
```

6. allowlist に無いクラスは捨てる。DoIt は捨てる。
7. メソッド本文のホスト語置換は Task 4。このタスクでは置換しない。

- [ ] **Step 4: テストが緑であることを確認する**

同じ gtest フィルタ。Expected: PASS

- [ ] **Step 5: コミットする**

```bash
git add runtime/include/ao/Vendor.hpp runtime/src/VendorExtract.cpp \
  runtime/tests/vendor_extract_test.cpp runtime/CMakeLists.txt
git commit -m "Add Cuis changes extractor

Graphify: query applyChunks parseChunks
Serena: n/a (new symbols)"
```

### Task 2: ピンと vendor ソース

**Files:**

- Create: `image/vendor/ORIGIN.md`
- Create: `image/vendor/ALLOWLIST`（許可リストを 1 行 1 クラス）
- Create: `image/vendor/cuis/LICENSE`（Cuis の LICENSE をバイト列のまま）
- Create: `image/vendor/cuis/<Class>.st`（切り出し結果。中身は実行時に決まる）
- Create: `image/vendor/DEFERRED.md`（空でよい。Task 4 が理由を書く）
- Modify: `NOTICE`（Cuis origin の 3 行を足す）
- Create: `runtime/src/ao_extract_main.cpp` は作らない。切り出しはテストからではなく、worktree で一度だけ動かす小さなドライバを `ao` に足す。
- Modify: `runtime/src/ao_main.cpp`（`extract-vendor` サブコマンド）
- Modify: `runtime/tests/vendor_extract_test.cpp`（ORIGIN の 3 項目を読むテストを足す。ソースツリーは `AO_SOURCE_DIR`）

**Interfaces:**

- Consumes: `extractVendor`
- Produces: ディスク上の `image/vendor/cuis/*.st` と ORIGIN の 3 項目（URL、コミット、ライセンスパス）

- [ ] **Step 1: 失敗するテストを書く**

```cpp
TEST(VendorOrigin, RecordsUrlCommitAndLicense) {
  const std::string root = AO_SOURCE_DIR;
  std::ifstream in(root + "/image/vendor/ORIGIN.md");
  ASSERT_TRUE(in.good());
  std::stringstream buf;
  buf << in.rdbuf();
  const std::string text = buf.str();
  EXPECT_NE(std::string::npos, text.find("https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev"));
  EXPECT_NE(std::string::npos, text.find("2628e261acaf4577084f2e788e973d0269b44fc5"));
  EXPECT_NE(std::string::npos, text.find("image/vendor/cuis/LICENSE"));
  std::ifstream lic(root + "/image/vendor/cuis/LICENSE");
  ASSERT_TRUE(lic.good());
  std::string first;
  std::getline(lic, first);
  EXPECT_NE(std::string::npos, first.find("MIT"));
}
```

- [ ] **Step 2: 赤であることを確認する**

`./build/runtime/ao_runtime_tests --gtest_filter=VendorOrigin.RecordsUrlCommitAndLicense`

Expected: FAIL（ORIGIN.md が無い）

- [ ] **Step 3: Cuis をピンして切り出す**

リポジトリ外にクローンする。成果物の `.image` は ao-smalltalk にコピーしない。

```bash
git clone https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev.git /tmp/cuis-pin
git -C /tmp/cuis-pin checkout 2628e261acaf4577084f2e788e973d0269b44fc5
cp /tmp/cuis-pin/LICENSE image/vendor/cuis/LICENSE
```

`ao extract-vendor` を `ao_main.cpp` に足す。引数は `<changes> <allowlist> <out-dir>`。`extractVendor` の `files` を `<out-dir>/<Class>.st` に書き、`notes` を stdout に出す。

```bash
./build/ao extract-vendor \
  /tmp/cuis-pin/CuisImage/Cuis7.9-8206.changes \
  image/vendor/ALLOWLIST \
  image/vendor/cuis
```

`ORIGIN.md` の本文:

```markdown
# Cuis vendor pin

- URL: https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev
- Commit: 2628e261acaf4577084f2e788e973d0269b44fc5
- License: image/vendor/cuis/LICENSE（upstream LICENSE のコピー。MIT）
- Source file: CuisImage/Cuis7.9-8206.changes
- Extract: `ao extract-vendor` が許可リストの最後の定義だけを image/vendor/cuis/<Class>.st に書く。
```

`NOTICE` に次を足す。

```
Cuis-Smalltalk class library excerpts
https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev
commit 2628e261acaf4577084f2e788e973d0269b44fc5
MIT; see image/vendor/cuis/LICENSE
```

切り出した各 `.st` を `parseChunks` し、エラーなく ClassDef が 1 つ取れることを Task 2 のテストに足す（ファイルが 1 つも無ければ FAIL）。

- [ ] **Step 4: テストが緑であることを確認する**

`./build/runtime/ao_runtime_tests --gtest_filter='Vendor*'`

Expected: PASS。`image/vendor/cuis/` に `.image` も `.changes` も無い。

- [ ] **Step 5: コミットする**

```bash
git add image/vendor NOTICE runtime/src/ao_main.cpp runtime/tests/vendor_extract_test.cpp
git commit -m "Pin Cuis vendor origin

Graphify: query vendor ORIGIN
Serena: find_symbol main"
```

### Task 3: rebind と上書き禁止

**Files:**

- Modify: `runtime/include/ao/WellKnown.hpp`（`rebind`、`eachNativeRequiredClass`、`isCatalogName`）
- Modify: `runtime/src/WellKnown.cpp`
- Modify: `runtime/include/ao/Globals.hpp`（`atPut`）
- Modify: `runtime/src/Globals.cpp`
- Modify: `runtime/src/Compile.cpp`（`applyClassDef` / `applyMethodsFor`）
- Modify: `runtime/tests/kernel_scan_test.cpp`
- Create: `runtime/tests/vendor_overwrite_test.cpp`
- Modify: `runtime/CMakeLists.txt`

**Interfaces:**

- Consumes: `isVendorStub`、既存 `applyChunks`
- Produces:

```cpp
bool WellKnown::isCatalogName(std::string_view name) const;
bool WellKnown::rebind(std::string_view name, Oop cls);  // スタブ以外は false
void WellKnown::eachNativeRequiredClass(void (*fn)(void*, Oop), void*) const;
void Globals::atPut(WellKnown& wk, std::string_view name, Oop value);
```

`rebind` は `kNamedClasses` の `cls` と `meta`（`heap.klass(cls)`）を書き換え、`Globals::atPut` で同名スロットを更新する。スタブ以外のカタログ名では何も書かず false。

- [ ] **Step 1: 失敗するテストを書く**

`vendor_overwrite_test.cpp` に次の 3 本。`kernel_scan_test.cpp` の走査を `eachNativeRequiredClass` に替え、クラスとそのメタクラスの両方を見る。extra は見ない。

```cpp
TEST(VendorOverwrite, RefusesNewSelectorOnObject) {
  Boot b;
  const char* src =
      "!Object methodsFor: 'vendor'!\n"
      "vendorProbe\n"
      "  ^1!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  EXPECT_FALSE(ao::applyChunks(b.ctx, acts, errs));
  auto meth = ao::lookup(b.heap, b.wk.objectClass, b.wk.intern("vendorProbe"));
  EXPECT_TRUE(meth.isNil());
}

TEST(VendorOverwrite, KeepsNativeIdentityEquals) {
  Boot b;
  const char* src =
      "!Object methodsFor: 'comparing'!\n"
      "== anObject\n"
      "  ^false!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ao::applyChunks(b.ctx, acts, errs);
  auto meth = ao::lookup(b.heap, b.wk.objectClass, b.wk.intern("=="));
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(meth));
}

TEST(VendorOverwrite, RebindsBagAndEvaluatesInstVar) {
  Boot b;
  const char* src =
      "!Collection subclass: #Bag\n"
      "  instanceVariableNames: 'contents'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P6b-Test'!\n"
      "!Bag methodsFor: 'accessing'!\n"
      "contents\n"
      "  ^contents!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs)) << (errs.empty() ? "" : errs[0].message);
  EXPECT_EQ(b.wk.bagClass, b.wk.named("Bag"));
  EXPECT_EQ(b.wk.bagClass, ao::Globals::at(b.wk, "Bag"));
  EXPECT_EQ(b.wk.bagMetaclass, b.heap.klass(b.wk.bagClass));
  auto meth = ao::lookup(b.heap, b.wk.bagClass, b.wk.intern("contents"));
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(meth));
  auto bag = send0(b, b.wk.bagClass, "new");
  ASSERT_TRUE(bag.isHeap());
  EXPECT_TRUE(send0(b, bag, "contents").isNil());
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
  // KernelScan と同じ eachNativeRequiredClass 走査。bad は空。
}
```

`UserCompiledMethodDoesNotFailScan` の走査本体は `kernel_scan_test.cpp` の既存ループと同じ判定（値が NativeMethod でないものを bad に積む）を、テストファイル内の関数 `nativeRequiredDictsAreNative(Boot&)` にまとめて両方から呼ぶ。

- [ ] **Step 2: 赤であることを確認する**

`./build/runtime/ao_runtime_tests --gtest_filter='VendorOverwrite.*:KernelScan.*'`

Expected: `RebindsBagAndEvaluatesInstVar` と `RefusesNewSelectorOnObject` が FAIL。`KernelScan.MethodDictionaryValuesAreNativeMethods` は、まだ `eachClass` のままならこのタスクの編集前は PASS。編集後のヘルパに繋いだ時点でユーザークラスを含めても PASS になるのが目標。

- [ ] **Step 3: 実装する**

`applyClassDef` の先頭:

- `isCatalogName(className) && !isVendorStub(className)` ならエラー `"refusing to redefine kernel class: "` を積んで false。
- スーパーが無い既存エラーはそのまま。
- `subclass:` 送信が成功したあと、`isVendorStub(className)` なら `rebind`。false ならエラーにして false。

`applyMethodsFor` の先頭:

- 同じカタログ判定でネイティブ必須クラスならエラーにして false（メソッドは 1 つも入れない）。
- 各メソッドをコンパイルしたあと、`MethodDictionary::at` で既存エントリを見る。`heap.klass == nativeMethodClass` なら `"native selector overwrite refused: " + selector` を積んで `continue`。
- それ以外は今どおり `installMethod`。

`eachNativeRequiredClass` は `kNamedClasses` を歩き、`isVendorStub` の名前を飛ばす。`eachClass` の挙動は変えない。

- [ ] **Step 4: テストが緑であることを確認する**

同じフィルタに加え `./build/runtime/ao_runtime_tests --gtest_filter='ChunkFileIn.*:KernelScan.*'`

Expected: PASS。既存の `CmUser` file-in はユーザークラスなので壊れない。

- [ ] **Step 5: コミットする**

```bash
git add runtime/include/ao/WellKnown.hpp runtime/src/WellKnown.cpp \
  runtime/include/ao/Globals.hpp runtime/src/Globals.cpp \
  runtime/src/Compile.cpp runtime/tests/kernel_scan_test.cpp \
  runtime/tests/vendor_overwrite_test.cpp runtime/CMakeLists.txt
git commit -m "Refuse kernel overwrite and rebind vendor stubs

Graphify: path WellKnown define applyChunks
Serena: replace_symbol_body ao::applyClassDef"
```

### Task 4: LOAD_ORDER とホストパッチ

**Files:**

- Create: `image/vendor/LOAD_ORDER`
- Modify: `image/vendor/DEFERRED.md`
- Modify: `runtime/src/VendorExtract.cpp`（ホスト語の置換と、スーパー未解決の deferred）
- Modify: `runtime/tests/vendor_extract_test.cpp`
- Create: `image/patches/host-deny.st` は自動生成しない。置換は切り出し時にメソッド本文へ書く。パッチであることが分かるよう、置換メソッドのプロトコル名を `ao-host-patch` にする。

**Interfaces:**

- Consumes: Task 2 の `.st` と `VendorClassFile`
- Produces: `LOAD_ORDER` の行は `cuis/<Class>.st`。`image/vendor/` からの相対パス。deferred のクラスは行にしない。

- [ ] **Step 1: 失敗するテストを書く**

```cpp
TEST(VendorExtract, RewritesHostSelectorAndDefersMissingSuper) {
  const char* src =
      "!MissingSuper subclass: #FileStream\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Files'!\n"
      "!FileStream methodsFor: 'open/close'!\n"
      "open\n"
      "  ^StandardFileStream new!\n"
      "!Object subclass: #Link\n"
      "  instanceVariableNames: 'nextLink'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Collections-Support'!\n";
  auto r = ao::extractVendor(src, {"FileStream", "Link"});
  ASSERT_EQ(1u, r.files.size());
  EXPECT_EQ("Link", r.files[0].className);
  bool saw = false;
  for (const auto& n : r.notes) {
    if (n.find("FileStream") != std::string::npos && n.find("MissingSuper") != std::string::npos) {
      saw = true;
    }
  }
  EXPECT_TRUE(saw);
}

TEST(VendorExtract, PatchesHostWordInMethodBody) {
  const char* src =
      "!Object subclass: #FileStream\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Files'!\n"
      "!FileStream methodsFor: 'open/close'!\n"
      "open\n"
      "  ^StandardFileStream new!\n"
      "!FileStream methodsFor: 'primitives'!\n"
      "primSize\n"
      "  <primitive: 94>\n"
      "  ^self primitiveFailed!\n";
  auto r = ao::extractVendor(src, {"FileStream"});
  ASSERT_EQ(1u, r.files.size());
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("ao-host-patch"));
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("host file stream is not connected"));
  EXPECT_EQ(std::string::npos, r.files[0].chunkText.find("StandardFileStream"));
  EXPECT_NE(std::string::npos, r.files[0].chunkText.find("<primitive: 94>"));
}
```

スーパー未解決の判定に使う「既知のクラス」は、`kNamedClasses` の名前と、同じ allowlist 内で既に採用したクラス名。`WellKnown` をこの関数から触らない。名前の配列を `Vendor.hpp` に `catalogNames()` として出すか、抽出関数に `std::span<const std::string_view> knownSupers` を足す。テストは known super にカタログ名（Object、Collection、Magnitude、SequenceableCollection、ReadWriteStream など `Bootstrap.cpp` の `kDefs` の name）を含める。実装側のデフォルト引数でその一覧を持つ。

- [ ] **Step 2: 赤であることを確認する**

`./build/runtime/ao_runtime_tests --gtest_filter='VendorExtract.*'`

Expected: 新しい 2 本が FAIL。

- [ ] **Step 3: 依存順とパッチを実装し、実ソースを再切り出しする**

`extractVendor` の末尾で:

- スーパー名がカタログにも許可リストにも無いクラスは `files` から外し、`notes` に `"<Name>: missing superclass <Super>"`。
- 残ったクラスを、スーパーが許可リスト内ならスーパーが先に来るよう安定トポロジカルソートする。循環ならそのクラスを notes `"<Name>: superclass cycle"` で外す。
- メソッド本文にホスト語が単語としてあれば、本文を次に置き換える。プロトコルは `ao-host-patch`。

```smalltalk
open
  ^self error: 'host file stream is not connected'
```

セレクタの先頭行（パターン）は残す。上の `open` は例で、実際のパターン行を保持する。

再実行:

```bash
./build/ao extract-vendor \
  /tmp/cuis-pin/CuisImage/Cuis7.9-8206.changes \
  image/vendor/ALLOWLIST \
  image/vendor/cuis
```

`LOAD_ORDER` は採用されたクラスだけ、ソート後の順で `cuis/<Class>.st`。`DEFERRED.md` は notes のうち実ファイルに出なかったクラスを 1 行 1 理由で書く。採用が 0 ならこのタスクは完了にしない。`Link` か `Bag` の少なくとも一方が LOAD_ORDER に残っていること。

- [ ] **Step 4: テストが緑であることを確認する**

`./build/runtime/ao_runtime_tests --gtest_filter='Vendor*'`

Expected: PASS

- [ ] **Step 5: コミットする**

```bash
git add runtime/include/ao/Vendor.hpp runtime/src/VendorExtract.cpp \
  runtime/tests/vendor_extract_test.cpp image/vendor
git commit -m "Add vendor load order and host patches

Graphify: query image/vendor LOAD_ORDER
Serena: replace_symbol_body ao::extractVendor"
```

### Task 5: `ao filein` と評価

**Files:**

- Modify: `runtime/include/ao/Compile.hpp`
- Modify: `runtime/src/Compile.cpp`
- Modify: `runtime/src/ao_main.cpp`
- Create: `runtime/tests/vendor_filein_test.cpp`
- Modify: `runtime/CMakeLists.txt`
- Modify: `image/Kernel/Bag.st` `image/Kernel/LinkedList.st` `image/Kernel/MappedCollection.st`（ファイルが存在するスタブだけ。先頭コメントの「実行定義は C++ NativeMethod」を「実行定義は image/vendor の CompiledMethod。このファイルはプロトコルコメント。」に替える。LOAD_ORDER に無いスタブはコメントを変えない）

**Interfaces:**

- Consumes: `applyChunks`、`LOAD_ORDER`
- Produces:

```cpp
bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors);
bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors);
bool fileInLoadOrder(CallContext& ctx, const std::filesystem::path& loadOrder,
                     std::vector<compiler::CompileError>& errors);
```

`fileInLoadOrder` は `#` 始まりと空行を無視し、各行を LOAD_ORDER ファイルのディレクトリからの相対パスとして `fileInFile` する。1 ファイルでも `applyChunks` が false ならそこで止める。DoIt は既存どおり評価しない。

CLI:

```
ao filein <file.st>
ao filein --load-order <LOAD_ORDER>
```

失敗時は stderr に最初の `CompileError.message` を書き、終了コード 1。成功は 0。`--help` にこの 2 つを足す。

- [ ] **Step 1: 失敗するテストを書く**

```cpp
TEST(VendorFileIn, LoadOrderEvaluatesLinkRoundTrip) {
  Boot b;
  const std::string order = std::string(AO_SOURCE_DIR) + "/image/vendor/LOAD_ORDER";
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, order, errs)) << (errs.empty() ? "" : errs[0].message);
  auto linkClass = b.wk.named("Link");
  if (!linkClass.isHeap()) {
    linkClass = b.wk.named("Bag");
  }
  ASSERT_TRUE(linkClass.isHeap());
  // Link が載った場合。セレクタは切り出したファイルのアクセサに合わせる。
  // 計画時点の想定は nextLink / nextLink:。切り出し結果の先頭メソッドが
  // 別セレクタなら、そのゲッターとセッター名へこのテストだけを合わせる。
  // vendor のソースは書き換えない。
  auto getter = ao::lookup(b.heap, linkClass, b.wk.intern("nextLink"));
  auto setter = ao::lookup(b.heap, linkClass, b.wk.intern("nextLink:"));
  if (getter.isHeap() && setter.isHeap()) {
    EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(getter));
    auto link = send0(b, linkClass, "new");
    ASSERT_TRUE(link.isHeap());
    EXPECT_TRUE(send1(b, link, "nextLink:", link).isHeap());
    EXPECT_EQ(link, send0(b, link, "nextLink"));
    return;
  }
  // Link が DEFERRED で Bag のみのとき。Task 3 と同じ contents 往復。
  auto contents = ao::lookup(b.heap, b.wk.bagClass, b.wk.intern("contents"));
  ASSERT_TRUE(contents.isHeap());
  EXPECT_EQ(b.wk.compiledMethodClass, b.heap.klass(contents));
  auto bag = send0(b, b.wk.bagClass, "new");
  EXPECT_TRUE(send0(b, bag, "contents").isNil());
}
```

`nextLink:` の戻りはヒープオブジェクトであることだけを見る。Cuis がレシーバを返しても引数を返しても、どちらもヒープである。同一性の確認は次の `nextLink` 読み出しで行う。

- [ ] **Step 2: 赤であることを確認する**

`./build/runtime/ao_runtime_tests --gtest_filter=VendorFileIn.LoadOrderEvaluatesLinkRoundTrip`

Expected: FAIL（`fileInLoadOrder` が未定義）

- [ ] **Step 3: file-in を実装する**

`fileInString` は `parseChunks` して `applyChunks`。`fileInFile` はファイルを文字列にして `fileInString`。読めなければ `"cannot read: "` + path。

`ao_main.cpp` の `--help` 文字列を更新する。`--test` の前に `filein` を処理する。`--load-order` のときだけ `fileInLoadOrder`、それ以外の第 2 引数は 1 ファイル。

評価テストが `nextLink` を見つけられない場合は、`image/vendor/cuis/Link.st` を読み、インスタンス変数を 1 つ読むメソッドと、それを代入するメソッドのセレクタをテストに書く。ソース側は変えない。どちらも無ければ Bag の分岐だけを残し、Link 用の分岐を削除する。両方無い状態で緑にしない。

- [ ] **Step 4: テストが緑であることを確認する**

```bash
./build/runtime/ao_runtime_tests --gtest_filter='Vendor*:KernelScan.*:ChunkFileIn.*'
./build/ao filein --load-order image/vendor/LOAD_ORDER
echo $?
```

Expected: テスト PASS、`ao filein` の終了コード 0。

- [ ] **Step 5: コミットする**

```bash
git add runtime/include/ao/Compile.hpp runtime/src/Compile.cpp runtime/src/ao_main.cpp \
  runtime/tests/vendor_filein_test.cpp runtime/CMakeLists.txt image/Kernel
git commit -m "File-in vendor sources

Graphify: path filein CompiledMethod
Serena: find_symbol main"
```

### Task 6: フェーズ完了

**Files:**

- Modify: `PHASE`（`P7`）
- Modify: `docs/phases/P6b.md`（受け入れのチェックを入れる）
- Modify: `docs/prs/P6b-01.md` … `P6b-04.md`（状態を done）
- Modify: `graphify-out/`（更新結果）

- [ ] **Step 1: 全スイートを緑にする**

worktree で `./scripts/test.sh`。CTest と Swift smoke が成功すること。`KernelBench.TenMillionToDo` は時間がかかる。失敗したら次へ進まない。

- [ ] **Step 2: 受け入れを仕様と突き合わせる**

- `ORIGIN.md` に URL・コミット・ライセンスパスがある。
- `LOAD_ORDER` が固定され、DEFERRED のクラスは含まれない。
- 上書きテストと Kernel 走査が緑。
- LOAD_ORDER の非 Kernel メソッドが CompiledMethod で、Task 5 の評価が緑。
- `image/vendor` に Morphic パッケージや `.image` が無い。

- [ ] **Step 3: PHASE とフェーズ文書を更新する**

`PHASE` は `P7`。`docs/phases/P6b.md` の 4 つのチェックを `[x]`。PR 文書の「状態: planned」を `done`。

- [ ] **Step 4: グラフを更新する**

ディレクトリ追加なので `graphify . --mode deep`。失敗したら `--update` に落とさず、失敗を報告して止める。`GRAPH_REPORT.md` の stamp がこのコミットの直前の HEAD と一致するよう、文書更新のコミットのあとにグラフを更新して別コミットにするか、同じコミットに含める。含めてよい成果物は `graphify-out/graph.html` `GRAPH_REPORT.md` `graph.json`。

- [ ] **Step 5: コミットする**

```bash
git add PHASE docs/phases/P6b.md docs/prs/P6b-01.md docs/prs/P6b-02.md \
  docs/prs/P6b-03.md docs/prs/P6b-04.md graphify-out
git commit -m "Mark vendor file-in phase done

Graphify: deep update after image/vendor
Serena: n/a"
```

main へのマージと push はこの計画のタスクに含めない。

## Self-review

- SPEC §3.12 の載せる範囲は許可リストと 1:1。載せない範囲は抽出の allowlist 外。
- 上書き禁止は Task 3。ORIGIN / LOAD_ORDER / 評価は Task 2・4・5。
- FileStream の実 I/O は P6b のフェーズ文書が「スタブまたは削除」としているので、エラーパッチに固定した。ブリッジ追加はしない。
- `variableSubclass:` と欠落スーパーは DEFERRED として明示し、黙って捨てない。
- Kernel 走査を狭める理由は前提に書いた。ネイティブ必須クラスの辞書は CompiledMethod を持たない。
- プレースホルダの「後で実装」は置いていない。Link のセレクタ名だけ、切り出しファイルに合わせてテスト側を直すと書いた。vendor ソースは直さない。

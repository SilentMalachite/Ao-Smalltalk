# P18 メソッドソースの永続化 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Accept と File In… でソース表に入ったメソッドのソースを `.aoimage`（版 4）のソース節に保存し、ロードでソース表に戻す。

**Architecture:** `Image::save` が、ヒープとレコードの後ろにソース節（メソッドのヒープ内オフセットとソースのバイト列の組）を書く。節の大きさはヘッダの `reserved64`（`sourcesBytes`）に入れる。ロードと保存の前検査（`checkFile`）が節を検査する。`Image::load` は組を出力引数で返し、`sessionImageLoad` がソースだけの項目（ブロックとデバッグ情報は無い）としてソース表に入れ直す。ソースは Smalltalk のヒープに入れず、CompiledMethod の形もコンパイラも変えない。

**Tech Stack:** C++20 runtime（GoogleTest、ctest）、Swift 6 AppKit（XCTest）、POSIX sh（`cli_test.sh`）

**Spec:** `docs/superpowers/specs/2026-10-12-p18-method-source-persistence-design.md`（正本は Task 1 で更新する `SPEC.md`）

## Global Constraints

- CLAUDE.md の標準ワークフロー: Graphify（`graphify-out/GRAPH_REPORT.md`、`graphify query`）→ Serena（`activate_project`、`find_symbol`、`find_referencing_symbols`）→ SPEC → テスト（赤）→ Serena のシンボル編集 → 緑 → `graphify update .`。コミット本文に `Graphify: …` と `Serena: …` の 2 行を付ける。
- コミットメッセージは英語の短い命令形。末尾に `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`。
- 版: `kImageVersion = 4`。版 3 は「ソース節の無いイメージ」として読む。版 1・2 とそれ以外は `unsupported image version <版>`。
- ソース節: `u32 count`、`u32 0`、`count` 回 {`u64 method`（ヒープ内オフセット）、`u32 length`、`u32 0`、`length` バイト、0 で 8 バイト境界まで}。`sourcesBytes` は版 4 で 8 以上の 8 の倍数、版 3 で 0。
- 節の違反（CompiledMethod でない、ヒープの外、重複、はみ出し、詰め物が 0 でない、末尾の食い違い）はすべて `damaged image`。ヒープを載せる前に拒否する。
- 書かないもの: doIt の項目（`doItDebug`、`heldDoIts`）、ダンプに入らないメソッドの項目、ブロック、デバッグ情報。
- CLI の `ao image save` は `count = 0` の節を書く。`ao image load` は検査だけして捨てる。
- Kernel は触らない。依存ライブラリを足さない。リリース作業（版文字列、タグ）は範囲外。
- ビルド: `./scripts/build.sh`。C++ の 1 テスト: `ctest --test-dir build --output-on-failure -R '<regex>'`。全体: `./scripts/test.sh`（10 分を超えることがある。バックグラウンドで回す）。

## Review Focus

1. ロードしたクラスの形を変える（instVar を足す）: ロードで戻したソースでメソッドがコンパイルし直され、動き続け、ソースも残る（Task 4 `ReshapeAfterLoadRecompilesFromRestoredSource`）。
2. 保存 → ロード → 保存 → ロード: 戻した項目が 2 回目の保存にも書かれる（Task 4 `SourcesSurviveTwoRoundTrips`）。
3. ロードのあとに Accept し直したメソッド: 新しいソースが保存され、古いソースは残らない（Task 4 `ReacceptAfterLoadSavesTheNewSource`）。
4. ロードのあとに削除したメソッド: 次の保存の節に入らない（Task 4 `RemovedMethodAfterLoadIsNotSaved`）。
5. 日本語（多バイト UTF-8）のコメントと文字列を含むソース: バイト単位で同じに戻る（Task 3 `AcceptedSourceSurvivesSaveAndLoad`）。

---

### Task 1: SPEC と PHASE を P18 にする

**Files:**
- Modify: `SPEC.md`（§2.2 冒頭の段落、§2.3、§2.4、§3.9、§3.10、§3.11、§4.1、§4.3、§6）
- Modify: `PHASE`

**Interfaces:**
- Consumes: なし
- Produces: SPEC の節名「ソースはイメージのソース節に書く」（§3.10）。後のタスクのコメントはこの名前を引く。

- [ ] **Step 1: PHASE を書き換える**

```bash
printf 'P18\n' > PHASE
```

- [ ] **Step 2: §2.3 と冒頭の段落にフェーズを足す**

`SPEC.md` で次の置き換えをする（Edit ツール。各 `old` は 1 か所だけ一致する）。

- `| P17 | Smalltalk のチェンジセット | … |` の行の直後に 1 行足す:

```
| P18 | メソッドソースの永続化 | ソース表のソースを `.aoimage`（版 4）のソース節に保存し、ロードで戻す（§3.9、§3.10、§3.11） |
```

- `P9 完了が v1。P10、P11、P12、P13、P14、P15、P16、P17 は v1 のあとのフェーズである。` → `P9 完了が v1。P10、P11、P12、P13、P14、P15、P16、P17、P18 は v1 のあとのフェーズである。`
- 62 行目付近の段落の末尾 `P17 は、そのチェンジセットを Smalltalk のクラス `ChangeSet` にし、イメージに保存する。` の後ろに ` P18 は、Accept したメソッドのソースをイメージに保存する。` を足す。

- [ ] **Step 3: §2.4 に版を足す**

§2.4 の最初の箇条の `… その次のマイナー版 `1.6.0` に含める。` の直後（同じ段落、`C ABI（` の前）に足す:

```
P18（メソッドソースの永続化）は `.aoimage` の版を 4 に上げるが、版 3 のイメージも読み、振る舞いを足すだけなので、その次のマイナー版 `1.7.0` に含める。版 4 のイメージは 1.6.0 以前のランタイムでは読めない（`unsupported image version 4`）。
```

- [ ] **Step 4: §3.10 の節を書き換え、参照名をそろえる**

```bash
sed -i '' 's/ソースはイメージに書かない/ソースはイメージのソース節に書く/g' SPEC.md
grep -c 'ソースはイメージのソース節に書く' SPEC.md   # 8 以上
```

続けて `#### ソースはイメージのソース節に書く` の直後の段落を置き換える。

old:
```
メソッドソースはセッションのルート表（`(Oop method, Oop string)` を `Roots` に登録したベクタ）だけが持つ。`.aoimage` には書かない。上書きした古い対はルートから外す。`ao_runtime_boot` と `ao_image_load` は表を空にする。
```
new:
```
メソッドソースはセッションのルート表（`(Oop method, Oop string)` を `Roots` に登録したベクタ）が持つ。ソースを String としてヒープに保存することはしない。`.aoimage` には、版 4 のソース節（§3.11）に、メソッドとソースのバイト列の組として書く（P18）。上書きした古い対はルートから外す。`ao_runtime_boot` は表を空にする。`ao_image_load` は表を空にしてから、ソース節の組から項目を作り直す。
```

同じ節の規則の箇条を置き換える。

old:
```
vendor、`ao filein`、`ao_filein_load_order` の file-in、`ao_accept_class` の `methodsFor:` のチャンク、ロードしたイメージのメソッドは項目を持たない（プレースホルダ）。
```
new:
```
vendor、`ao filein`、`ao_filein_load_order` の file-in、`ao_accept_class` の `methodsFor:` のチャンクは項目を持たない（プレースホルダ）。ロードしたイメージのメソッドは、ソース節に組があればソースだけの項目（ブロックとデバッグ情報を持たない）を持ち、無ければ項目を持たない。
```

規則の箇条の最後（`- 形の変更（§3.9）で移したメソッドは、…ブロックとデバッグ情報を持たない）。` の行）の直後に足す:

```
- 保存（P18。`ao_image_save`）: 表の項目のうち、メソッドが保存するヒープに入り、ソースが String のものを、表の順にソース節に書く。doIt の項目、どこからも届かないメソッドの項目、ブロック、デバッグ情報は書かない。並びを作る C++ のメモリが無ければ保存は失敗する（`AO_ERR`）。
- ロード（P18。`ao_image_load`）: ソース節の組の順に、ソースの String を作ってソースだけの項目を入れる。作る途中でメモリが足りなくなったらそこで止め、残りのメソッドは項目を持たない（プレースホルダ）。ロードは成功を返す。戻した項目のメソッドは、Debugger ではソース位置と temp 名を出さず、Debugger の Accept もできない（Browser から Accept し直せば項目が作り直される）。
```

`ao_browser_source` の段落（§3.10）で置き換える。

old: `ソースが無いのは、NativeMethod、イメージを読み込んだあとのメソッド、vendor と file-in`
new: `ソースが無いのは、NativeMethod、読み込んだイメージのソース節に組の無かったメソッド、vendor と file-in`

- [ ] **Step 5: §3.9 の P17 の制限を書き換える**

old（`制限（P17）:` で始まる段落全体）:
```
制限（P17）: イメージから戻したセットのメソッドにはソースが無い。ソース表は `.aoimage` に書かないからである（§3.10「ソースはイメージのソース節に書く」）。Save Image… のあとに Open Image… すると、戻ったセットの File Out は、そのメソッドごとに `"<Class>>><selector> source not available"` のコメントのチャンクを書き（上の 4.）、ソース枠はプレースホルダを出す。種類と、クラスの定義（生きているクラスから作る）は戻る。ソースが戻るのは、Open Image… のあとに Accept か File In… をやり直したメソッドだけである。
```
new:
```
保存したイメージ（P18）: Save Image… のあとに Open Image… すると、戻ったセットのメソッドは、保存したときにソース表にあったソースを持つ（§3.10「ソースはイメージのソース節に書く」）。File Out はそのソースを書き、ソース枠もそれを出す。ソースの無かったメソッド（`ao_accept_class` の `methodsFor:` のチャンクで入れたものなど）は、今までどおり `"<Class>>><selector> source not available"` のコメントのチャンクになり（上の 4.）、ソース枠はプレースホルダを出す。種類と、クラスの定義（生きているクラスから作る）も戻る。
```

- [ ] **Step 6: §3.11 を版 4 にする**

- `- マジック `AOIM`、バージョン（3）、ポインタサイズ、エンディアン` → `- マジック `AOIM`、バージョン（4。版 3 も読む）、ポインタサイズ、エンディアン、ソース節のバイト数（`sourcesBytes`。P18）`
- `- 起動時に再配置し、NativeMethod の関数ポインタは …` の行の直前に足す:

```
- ソース節（P18。版 4）: レコードの直後に置く。`u32 count`、`u32 0`、続けて `count` 回、`u64 method`（ヒープ内オフセット。ポインタと同じ符号化）、`u32 length`、`u32 0`、ソースの UTF-8 の `length` バイト、0 で 8 バイト境界まで詰めたもの。ヘッダの最後の 8 バイト（版 3 の予約欄）が節のバイト数 `sourcesBytes` で、版 4 では 8 以上の 8 の倍数、版 3 では 0 である。書く項目は §3.10「ソースはイメージのソース節に書く」。CLI の `ao image save` はセッションを持たないので `count` 0 の節を書き、`ao image load` は節を検査して捨てる
```

- チェンジセットの箇条の `形式は変えないので、版は 3 のままである。ただしメソッドのソース表は書かないので（§3.10「ソースはイメージのソース節に書く」）、戻ったセットのメソッドにソースは無く、File Out はそのメソッドを `source not available` のコメントにする（§3.9 File Out の制限）` → `メソッドのソースはソース節（P18）から戻る（§3.9 の「保存したイメージ」）`
- `形式の版は 3 である。版 1 は、` → `形式の版は 4 である。版 4 は、版 3 のレコードの後ろにソース節を足した形式で、版 3 はソース節の無いイメージとして読む（ソース表は空になる）。保存は常に版 4 で書く。版 1 は、`
- `ほかの版も同じく `unsupported image version <版>` で拒否する。` は変えない。
- 拒否の表: `| 版が 3 でない | `unsupported image version <版>` |` → `| 版が 3 でも 4 でもない | `unsupported image version <版>` |`
- 表の最後の行 `| それ以外（予約欄、レコード、ヒープ、グローバルの照合などが合わない） | `damaged image` |` → `| それ以外（予約欄、`sourcesBytes`、レコード、ヒープ、グローバルの照合、ソース節などが合わない） | `damaged image` |`
- 「ロードの検査」の最初の段落: `ファイルは、ヘッダ、heapBytes のヒープ、レコード 184 件（well-known 127、グローバル 57。1 件は 16〜268 バイト）からなるので` → `ファイルは、ヘッダ、heapBytes のヒープ、レコード 184 件（well-known 127、グローバル 57。1 件は 16〜268 バイト）、sourcesBytes のソース節（版 4）からなるので`
- 「ロードの検査」の箇条の最後（`- CompiledMethod のインスタンスは …` の行）の直後に足す:

```
- ソース節（版 4）の各組の `method` は、ヒープ内のオブジェクトの先頭を指し、その klass は well-known の CompiledMethod である。同じ `method` は 2 回出ない。`length` とその詰め物は節からはみ出さず、詰め物（`count` の後ろと各 `length` の後ろの `u32` を含む）はすべて 0 である。`count` 件を読み終えた位置がファイルの末尾である。
```

- [ ] **Step 7: §4.1、§4.3、§6 にテストと受け入れを足す**

§4.1 の `- `image_save_load_test`: save 後に同一評価結果。…` の行の直後に足す:

```
- メソッドソースの永続化（§3.10、§3.11。P18）: `image_format_test` に版 4 と版 3 のヘッダ（`VersionThreeHasNoSourcesAndVersionFourNeedsASection`）。`image_save_load_test` に、版 3 のイメージがソース無しで読める（`VersionThreeImageLoadsWithoutSources`）、ソース節の壊れ方の拒否（`RefusesSourcesSectionThatDoesNotParse`、`RefusesSourceForAnObjectThatIsNoMethod`、`RefusesSourceOutsideTheHeap`、`RefusesRepeatedSourceMethod`、`RefusesSourceRunningPastTheFile`、`RefusesNonZeroSourcePadding`、`RefusesSourcesBytesThatDisagreeWithTheFile`）、Accept したソースと日本語のソースが戻り doIt のテキストは書かれない（`AcceptedSourceSurvivesSaveAndLoad`）。`accept_abi_test` の `ClassDefinitionThenImageKeepsSourceText`、`ReshapeAfterLoadRecompilesFromRestoredSource`、`SourcesSurviveTwoRoundTrips`、`ReacceptAfterLoadSavesTheNewSource`、`RemovedMethodAfterLoadIsNotSaved`。`changes_abi_test` の `SavedImageFilesOutTheRestoredSources`。`cli_test.sh` の `image_save` に版 4 のヘッダ
```

§4.3 の `- Change Sorter（§3.9、§3.10。P17）: …` の行の直後に足す:

```
- メソッドソースの永続化（P18）: `ChangeSorterTests` に、Open Image… のあと戻ったセットのメソッドのソースがソース枠に出る（`testOpenImageShowsTheRestoredMethodSource`）
```

§6 の `### Smalltalk のチェンジセット（P17）` の小節の後ろ（`---` と `## 7. 用語` の前）に足す:

```
### メソッドソースの永続化（P18）

P18（§3.9、§3.10、§3.11）は次をすべて満たす。上の項目は、下で変えたもの（イメージの版、戻ったセットのソース）を除いて変えない。

- [ ] `.aoimage` の版は 4。ソース節があり、版 3 はソース無しで読める。版 1・2・5 は理由付きで拒む（`VersionThreeHasNoSourcesAndVersionFourNeedsASection`、`VersionThreeImageLoadsWithoutSources`、`RefusesVersionOneWithReason`）
- [ ] Accept したメソッドのソースが保存とロードで戻り、`ao_browser_source` と `Behavior>>sourceCodeAt:` が答える。doIt のテキストは書かれない（`AcceptedSourceSurvivesSaveAndLoad`、`ClassDefinitionThenImageKeepsSourceText`）
- [ ] 壊れたソース節はヒープを載せる前に `damaged image` で拒まれる（`RefusesSources*`、`RefusesRepeatedSourceMethod`、`RefusesNonZeroSourcePadding`）
- [ ] ロードのあとの形の変更、再 Accept、削除、2 回の往復でソースが正しい（`ReshapeAfterLoadRecompilesFromRestoredSource`、`ReacceptAfterLoadSavesTheNewSource`、`RemovedMethodAfterLoadIsNotSaved`、`SourcesSurviveTwoRoundTrips`）
- [ ] 戻ったセットの File Out と Change Sorter のソース枠にソースが出る（`SavedImageFilesOutTheRestoredSources`、`testOpenImageShowsTheRestoredMethodSource`）
- [ ] Kernel を変えていない。Kernel 走査は緑
- [ ] `PHASE` は `P18`、SPEC §6 の本小節がすべて `[x]`、CHANGELOG に項目
```

P17 の小節の `保存して開き直すと、セット、current、ID が戻る。`.aoimage` の版は 3 のまま` は歴史として残す（P18 の小節が「上の項目は…除いて変えない」で上書きする）。

- [ ] **Step 8: 差分を確かめてコミットする**

```bash
git diff --stat   # SPEC.md と PHASE だけ
git add SPEC.md PHASE
git commit -F - <<'EOF'
Specify P18 method source persistence

Graphify: query image save load methodSources
Serena: none (SPEC and PHASE only)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
```

---

### Task 2: 版 4 のヘッダと空のソース節

保存は常に版 4 と空のソース節（`count` 0）を書き、ロードと前検査はソース節を検査して読む。この時点ではまだ組を書かない。

**Files:**
- Modify: `runtime/include/ao/ImageFormat.hpp`（`kImageVersion`、`kImageVersionNoSources`、`ImageHeader::sourcesBytes`）
- Modify: `runtime/src/ImageFormat.cpp`（`writeHeader`、`readHeader`）
- Modify: `runtime/include/ao/Image.hpp`（`ImageSource`、`save` と `load` の引数）
- Modify: `runtime/src/ImageSave.cpp`（`appendSources`、`Image::save`）
- Modify: `runtime/src/ImageLoad.cpp`（`ParsedSource`、`Parsed::sources`、`headerOk`、`parseSources`、`checkFile`、`Image::load`）
- Test: `runtime/tests/image_format_test.cpp`、`runtime/tests/image_save_load_test.cpp`

**Interfaces:**
- Consumes: なし
- Produces:
  - `ao::ImageFormat::kImageVersion == 4`、`ao::ImageFormat::kImageVersionNoSources == 3`、`ImageHeader::sourcesBytes`（`std::uint64_t`）
  - `struct ao::ImageSource { Oop method; std::string text; };`（`ao/Image.hpp`）
  - `Image::save(Heap&, Roots&, WellKnown&, std::string_view path, std::string* reason = nullptr, std::size_t haltedProcesses = 0, const std::vector<ImageSource>* sources = nullptr)`
  - `Image::load(Heap&, Roots&, WellKnown&, std::string_view path, std::string* reason = nullptr, std::vector<ImageSource>* sources = nullptr)`: 成功したとき `*sources` を再配置後の組で置き換える（版 3 なら空）。

- [ ] **Step 1: Graphify と Serena で影響を取る**

```bash
graphify query "Image save load checkFile headerOk readHeader" --budget 1500
```

Serena: `find_symbol` で `ao::ImageFormat::readHeader`、`checkFile`、`headerOk`、`ao::Image::save`、`ao::Image::load`。`find_referencing_symbols` で `Image::save` と `Image::load`（`ao_main.cpp`、`Session.cpp`、テスト）。どれも引数の追加は既定値付きなので呼び出し側は変えない。

- [ ] **Step 2: ヘッダのテストを書く（赤）**

`runtime/tests/image_format_test.cpp`:

`HeaderRoundTripAndRejects` で `h.nextHash = 1;` の直後に `h.sourcesBytes = 8;` を足し、`EXPECT_EQ(1u, out.nextHash);` の直後に `EXPECT_EQ(8u, out.sourcesBytes);` を足す。同じテストの版の拒否を 5 に変える:

```cpp
  ao::ImageFormat::writeHeader(buf.data(), h);
  buf[4] = std::byte{5};
  buf[5] = std::byte{0};
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("unsupported image version 5", reason);
```

`VersionOneIsRefusedWithReason` の `EXPECT_EQ(3u, ao::ImageFormat::kImageVersion);` → `EXPECT_EQ(4u, ao::ImageFormat::kImageVersion);`

ファイルの末尾に足す:

```cpp
// P18 / SPEC §3.11: version 4 keeps the sources section's size in the header's last 8 bytes (8 or
// more, a multiple of 8). Version 3 still reads, with no section; its last 8 bytes stay 0.
TEST(ImageFormat, VersionThreeHasNoSourcesAndVersionFourNeedsASection) {
  ao::ImageFormat::ImageHeader h;
  h.pointerBits = ao::ImageFormat::kImagePointerBits;
  h.endian = ao::ImageFormat::kImageEndianLittle;
  h.heapBytes = 16;
  h.wellKnownCount = 127;
  h.globalCount = 57;
  std::vector<std::byte> buf(ao::ImageFormat::kImageHeaderBytes);
  ao::ImageFormat::ImageHeader out;
  std::string reason;

  h.version = ao::ImageFormat::kImageVersionNoSources;
  h.sourcesBytes = 0;
  ao::ImageFormat::writeHeader(buf.data(), h);
  ASSERT_TRUE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason)) << reason;
  EXPECT_EQ(3u, out.version);
  EXPECT_EQ(0u, out.sourcesBytes);
  h.sourcesBytes = 8;
  ao::ImageFormat::writeHeader(buf.data(), h);
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("damaged image", reason);

  h.version = ao::ImageFormat::kImageVersion;
  for (std::uint64_t bad : {std::uint64_t{0}, std::uint64_t{4}, std::uint64_t{12}}) {
    h.sourcesBytes = bad;
    ao::ImageFormat::writeHeader(buf.data(), h);
    EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason)) << bad;
    EXPECT_EQ("damaged image", reason) << bad;
  }
  h.sourcesBytes = 24;
  ao::ImageFormat::writeHeader(buf.data(), h);
  ASSERT_TRUE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason)) << reason;
  EXPECT_EQ(4u, out.version);
  EXPECT_EQ(24u, out.sourcesBytes);
}
```

- [ ] **Step 3: 保存とロードのテストを書く（赤）**

`runtime/tests/image_save_load_test.cpp` の無名名前空間（`oopWords` の上）に足す:

```cpp
// P18 / SPEC §3.11: where the sources section starts (the file's end less its size).
std::size_t sourcesAt(const std::vector<char>& bytes) {
  ao::ImageFormat::ImageHeader header;
  if (!ao::ImageFormat::readHeader(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size(),
                                   &header) ||
      header.sourcesBytes > bytes.size()) {
    return 0;
  }
  return bytes.size() - static_cast<std::size_t>(header.sourcesBytes);
}

// A version 4 image's bytes as version 3: no sources section, the header's last 8 bytes 0.
std::vector<char> asVersionThree(std::vector<char> bytes) {
  ao::ImageFormat::ImageHeader header;
  EXPECT_TRUE(ao::ImageFormat::readHeader(reinterpret_cast<const std::byte*>(bytes.data()),
                                          bytes.size(), &header));
  bytes.resize(bytes.size() - static_cast<std::size_t>(header.sourcesBytes));
  header.version = ao::ImageFormat::kImageVersionNoSources;
  header.sourcesBytes = 0;
  ao::ImageFormat::writeHeader(reinterpret_cast<std::byte*>(bytes.data()), header);
  return bytes;
}

void putU32(std::vector<char>& bytes, std::size_t at, std::uint32_t v) {
  std::memcpy(bytes.data() + at, &v, sizeof v);
}
```

`oopWords` の最後の行 `return cursor == bytes.size();` を、ソース節を読んでメソッドの語も集める形にする:

```cpp
  if (cursor + header.sourcesBytes != bytes.size()) return false;
  if (header.sourcesBytes == 0) return true;
  std::uint32_t count = 0;
  std::memcpy(&count, p + cursor, sizeof count);
  cursor += 8;
  for (std::uint32_t i = 0; i < count; ++i) {
    if (cursor + 16 > bytes.size()) return false;
    out->push_back(word(cursor));
    std::uint32_t length = 0;
    std::memcpy(&length, p + cursor + 8, sizeof length);
    cursor += 16 + length + (8u - length % 8u) % 8u;
  }
  return cursor == bytes.size();
```

`RefusesVersionOneWithReason` を版 4 に合わせる: `EXPECT_EQ(3, bytes[4]);` → `EXPECT_EQ(4, bytes[4]);`。末尾の `bytes[4] = 3;` → `bytes[4] = 4;`（版 4 に戻せば読める、の意味は同じ）。

`RefusesVersionOneWithReason` の後ろに足す:

```cpp
// P18 / SPEC §3.11: a version 3 image (no sources section) still loads; the loaded image has no
// sources. Version 5 is refused at the header with its reason.
TEST(ImageSaveLoad, VersionThreeImageLoadsWithoutSources) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-version-3.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  std::vector<char> bytes = readAll(path);
  ASSERT_EQ(4, bytes[4]);
  ASSERT_TRUE(writeAll(path, asVersionThree(bytes)));
  {
    Loaded old;
    std::string reason;
    std::vector<ao::ImageSource> sources{ao::ImageSource{ao::Oop::nil(), "stale"}};
    ASSERT_TRUE(ao::Image::load(old.heap, old.roots, old.wk, path.string(), &reason, &sources))
        << reason;
    EXPECT_TRUE(sources.empty());
  }
  bytes[4] = 5;
  ASSERT_TRUE(writeAll(path, bytes));
  Loaded future;
  std::string reason;
  EXPECT_FALSE(ao::Image::load(future.heap, future.roots, future.wk, path.string(), &reason));
  EXPECT_EQ("unsupported image version 5", reason);
  EXPECT_EQ(0u, future.heap.oldUsed());
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: a save writes an empty sources section (count 0) when it has no sources; a
// count the section cannot hold, a non-zero pad after the count, and bytes after the last entry
// are a damaged image, refused before the heap is adopted.
TEST(ImageSaveLoad, RefusesSourcesSectionThatDoesNotParse) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-bad-sources.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> good = readAll(path);
  const std::size_t at = sourcesAt(good);
  ASSERT_EQ(good.size() - 8, at);
  std::uint32_t count = 1;
  std::memcpy(&count, good.data() + at, sizeof count);
  EXPECT_EQ(0u, count);

  auto refused = [&](const std::vector<char>& bytes, const char* what) {
    SCOPED_TRACE(what);
    ASSERT_TRUE(writeAll(path, bytes));
    Loaded image;
    std::string reason;
    EXPECT_FALSE(ao::Image::load(image.heap, image.roots, image.wk, path.string(), &reason));
    EXPECT_EQ("damaged image", reason);
    EXPECT_EQ(0u, image.heap.oldUsed());
  };
  std::vector<char> bytes = good;
  putU32(bytes, at, 1);
  refused(bytes, "a count with no entry");
  bytes = good;
  putU32(bytes, at + 4, 7);
  refused(bytes, "a non-zero pad after the count");
  bytes = good;
  bytes.resize(bytes.size() + 8, '\0');
  ao::ImageFormat::ImageHeader header;
  ASSERT_TRUE(ao::ImageFormat::readHeader(reinterpret_cast<const std::byte*>(bytes.data()),
                                          bytes.size(), &header));
  header.sourcesBytes += 8;
  ao::ImageFormat::writeHeader(reinterpret_cast<std::byte*>(bytes.data()), header);
  refused(bytes, "bytes after the last entry");
  std::filesystem::remove(path);
}
```

- [ ] **Step 4: 赤を確かめる**

Run: `./scripts/build.sh 2>&1 | tail -5`
Expected: コンパイルエラー（`kImageVersionNoSources`、`sourcesBytes`、`ao::ImageSource` が無い）。

- [ ] **Step 5: ヘッダを実装する**

`runtime/include/ao/ImageFormat.hpp`:

```cpp
  // SPEC §3.11: version 1 is the format before B4 (Symbol class names, Kernel instVarNames, the
  // global dictionary, classPools); version 2 the one before B9, whose Dictionaries and Sets pack
  // their pairs from the front instead of hashing them. Both are refused, not repaired. Version 3
  // is the one before P18's sources section and still loads, with no sources.
  static constexpr std::uint16_t kImageVersion = 4;
  static constexpr std::uint16_t kImageVersionNoSources = 3;
```

`ImageHeader` の `nextHash` の後ろに:

```cpp
    // SPEC §3.11 (P18): the sources section's size; 0 in version 3.
    std::uint64_t sourcesBytes = 0;
```

`runtime/src/ImageFormat.cpp` の `writeHeader` で `d.nextHash = h.nextHash;` の後ろに `d.reserved64 = h.sourcesBytes;`。

`readHeader` の版と予約欄の検査を置き換える（Serena `replace_symbol_body` で `ao::ImageFormat::readHeader` 全体を書き直すか、`replace_content` で該当行だけ）:

```cpp
  // SPEC §3.11: an image of another version is refused before anything else is read from it.
  if (d.version != kImageVersion && d.version != kImageVersionNoSources) {
    return refuse("unsupported image version " + std::to_string(d.version));
  }
  if (d.pointerBits != kImagePointerBits || d.endian != kImageEndianLittle ||
      d.headerBytes != kImageHeaderBytes) {
    return refuse("unsupported image format");
  }
  if (d.reserved16 != 0 || d.reserved32a != 0 || d.reserved32b != 0) {
    return refuse("damaged image");
  }
  // SPEC §3.11 (P18): version 4 keeps the sources section's size here; version 3 keeps 0.
  if (d.version == kImageVersionNoSources ? d.reserved64 != 0
                                          : d.reserved64 < 8 || (d.reserved64 % 8) != 0) {
    return refuse("damaged image");
  }
```

`out->nextHash = d.nextHash;` の後ろに `out->sourcesBytes = d.reserved64;`。

- [ ] **Step 6: `ImageSource` と引数を足す**

`runtime/include/ao/Image.hpp`（`struct Image {` の前）:

```cpp
// SPEC §3.10, §3.11 (P18): one pair of the sources section: a method in the heap and the UTF-8 of
// its source.
struct ImageSource {
  Oop method;
  std::string text;
};
```

`save` と `load` の宣言を置き換える:

```cpp
  // False when nothing was written; *reason (when given) then says why (SPEC §3.11).
  // haltedProcesses is how many processes a live debugger holds halted (SPEC §3.13): with any,
  // nothing is written ("halted processes"). sources (P18) go to the sources section, those whose
  // method is in the heap written, in order; none when null.
  static bool save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr, std::size_t haltedProcesses = 0,
                   const std::vector<ImageSource>* sources = nullptr);
  // False when the image is refused; *reason (when given) then says why (SPEC §3.11). On success
  // *sources (when given) is the sources section's pairs with the loaded methods (P18).
  static bool load(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                   std::string* reason = nullptr, std::vector<ImageSource>* sources = nullptr);
```

`ao/Oop.hpp` が `Image.hpp` から見えることを確かめる（`Heap.hpp` 経由で入る。入らなければ `#include "ao/Oop.hpp"` を足す）。

- [ ] **Step 7: 保存でソース節を書く**

`runtime/src/ImageSave.cpp` の `appendRecord` の後ろ（無名名前空間の中）に足す。`<unordered_set>` が無ければ include する。

```cpp
// SPEC §3.11 (P18): the sources section: u32 count, u32 0, then for each pair the method's heap
// offset, the u32 length, a u32 0 and the bytes padded with 0 to 8. A pair whose method is not in
// the dump has nothing to point at and is left out, as is a second pair for the same method.
// False when a source is longer than a u32 can say.
bool appendSources(std::vector<std::byte>& dst, const Trace& tr,
                   const std::vector<ImageSource>* sources) {
  const std::size_t start = dst.size();
  const std::uint32_t zero = 0;
  std::uint32_t count = 0;
  appendRaw(dst, &count, sizeof count);
  appendRaw(dst, &zero, sizeof zero);
  if (sources != nullptr) {
    std::unordered_set<std::uint64_t> written;
    for (const ImageSource& src : *sources) {
      std::uint64_t bits = 0;
      if (!src.method.isHeap() || !encodeOop(tr, src.method, false, &bits) ||
          !written.insert(bits).second) {
        continue;
      }
      if (src.text.size() > UINT32_MAX) {
        return false;
      }
      const auto length = static_cast<std::uint32_t>(src.text.size());
      appendRaw(dst, &bits, sizeof bits);
      appendRaw(dst, &length, sizeof length);
      appendRaw(dst, &zero, sizeof zero);
      appendRaw(dst, src.text.data(), src.text.size());
      dst.resize(dst.size() + (8u - length % 8u) % 8u, std::byte{0});
      ++count;
    }
  }
  std::memcpy(dst.data() + start, &count, sizeof count);
  return true;
}
```

`Image::save` の定義の引数に `const std::vector<ImageSource>* sources` を足す（既定値は宣言だけ）。グローバルのレコードを `tail` に足すループの後ろに:

```cpp
  std::vector<std::byte> sourcesSection;
  if (!appendSources(sourcesSection, tr, sources)) {
    return refuse("a method source is longer than 4 GiB");
  }
```

`header.nextHash = heap.hashCursor();` の後ろに `header.sourcesBytes = sourcesSection.size();`。`file.reserve(...)` に `+ sourcesSection.size()` を足し、`tail` を足した後ろに `appendRaw(file, sourcesSection.data(), sourcesSection.size());`。

- [ ] **Step 8: 検査とロードでソース節を読む**

`runtime/src/ImageLoad.cpp`:

`Parsed` の前に足し、`Parsed` にメンバを足す:

```cpp
// One pair of the sources section (P18): the method's heap offset, and where its bytes are in the
// file.
struct ParsedSource {
  std::uint64_t method = 0;
  std::size_t at = 0;
  std::uint32_t length = 0;
};
```

```cpp
  std::vector<ParsedSource> sources;
```

`headerOk` のファイルの大きさの検査を置き換える:

```cpp
  const std::uint64_t records = std::uint64_t{header.wellKnownCount} + header.globalCount;
  const std::uint64_t beforeRecords =
      ImageFormat::kImageHeaderBytes + std::uint64_t{header.heapBytes};
  // SPEC §3.11 (P18): the sources section follows the records; readHeader bounds its size.
  if (header.sourcesBytes > fileSize ||
      fileSize - header.sourcesBytes < beforeRecords + records * kMinRecordBytes ||
      fileSize - header.sourcesBytes > beforeRecords + records * kMaxRecordBytes) {
    return refuse("damaged image", "the file is " + std::to_string(fileSize) +
                                       " bytes, which its header, records and sources cannot make");
  }
```

`checkGlobals` の後ろ（`Parsed` より前。`findRecord`、`headerAt`、`fail`、`readPod` が見える位置）に足す:

```cpp
// SPEC §3.11 (P18): the sources section from cursor to the file's end. Each method is the offset
// of an object in the heap whose class is the well-known CompiledMethod, at most once; each
// length and its padding stay in the section, every pad is 0, and the last pair ends where the
// file does. Version 3 has no section (cursor is the file's end).
bool parseSources(const std::vector<std::byte>& file, std::size_t cursor, const std::byte* section,
                  const std::unordered_set<std::uint64_t>& starts,
                  const std::vector<ImageRecord>& wellKnown, std::vector<ParsedSource>* out,
                  std::string* detail) {
  if (cursor == file.size()) {
    return true;
  }
  const ImageRecord* methodRec = findRecord(wellKnown, "CompiledMethod");
  if (methodRec == nullptr) {
    return fail(detail, "the sources section has no well-known CompiledMethod to check against");
  }
  std::uint32_t count = 0;
  std::uint32_t pad = 0;
  if (!readPod(file, &cursor, &count) || !readPod(file, &cursor, &pad) || pad != 0) {
    return fail(detail, "the sources section does not start with its count");
  }
  std::unordered_set<std::uint64_t> seen;
  for (std::uint32_t i = 0; i < count; ++i) {
    const std::string which = "source " + std::to_string(i);
    std::uint64_t method = 0;
    std::uint32_t length = 0;
    if (!readPod(file, &cursor, &method) || !readPod(file, &cursor, &length) ||
        !readPod(file, &cursor, &pad) || pad != 0) {
      return fail(detail, which + " has no method and length");
    }
    if (starts.find(method) == starts.end() ||
        headerAt(section, method).klass.bits() != methodRec->bits) {
      return fail(detail, which + " is not for a CompiledMethod in the heap");
    }
    if (!seen.insert(method).second) {
      return fail(detail, which + " repeats a method");
    }
    const std::size_t padded = std::size_t{length} + (8u - length % 8u) % 8u;
    if (cursor > file.size() || padded > file.size() - cursor) {
      return fail(detail, which + " runs past the file");
    }
    for (std::size_t k = length; k < padded; ++k) {
      if (file[cursor + k] != std::byte{0}) {
        return fail(detail, which + " is padded with a byte that is not 0");
      }
    }
    out->push_back(ParsedSource{method, cursor, length});
    cursor += padded;
  }
  if (cursor != file.size()) {
    return fail(detail, "the sources section does not end where the file does");
  }
  return true;
}
```

`parseSources` が `Parsed` より前に来るので、`ParsedSource` は `parseSources` より前に置く。

`checkFile` の `cursor != file.size()` → `cursor != file.size() - p->header.sourcesBytes`（`headerOk` が `sourcesBytes <= file.size()` を保証する）。オブジェクトの検査の `if (...) { return damaged(); }` の後ろ、`return true;` の前に:

```cpp
  if (!parseSources(file, cursor, p->section, p->starts, p->wellKnown, &p->sources, detail)) {
    return damaged();
  }
```

`Image::load` の定義の引数に `std::vector<ImageSource>* sources` を足し、`wk.checkSmallIntegerFastPath();` の後ろ、`return true;` の前に:

```cpp
  // SPEC §3.11 (P18): the pairs with the methods as loaded; the session puts them in its table.
  if (sources != nullptr) {
    auto* base = const_cast<std::byte*>(heap.oldBase());
    sources->clear();
    for (const ParsedSource& s : parsed.sources) {
      sources->push_back(ImageSource{
          fileOop(base, s.method),
          std::string(reinterpret_cast<const char*>(file.data() + s.at), s.length)});
    }
  }
```

- [ ] **Step 9: 緑を確かめる**

Run: `./scripts/build.sh 2>&1 | tail -3 && ctest --test-dir build --output-on-failure -R 'ImageFormat\.|ImageSaveLoad\.|ImageSave\.'`
Expected: すべて PASS。落ちる既存テストがあれば、版 3 を前提にした手作りのバイト列だけを版 4 に直す（意味を変えない）。

- [ ] **Step 10: 全体を回し、コミットする**

Run: `ctest --test-dir build --output-on-failure 2>&1 | tail -3`
Expected: `100% tests passed`

```bash
git add runtime/include/ao/ImageFormat.hpp runtime/src/ImageFormat.cpp runtime/include/ao/Image.hpp \
  runtime/src/ImageSave.cpp runtime/src/ImageLoad.cpp runtime/tests/image_format_test.cpp \
  runtime/tests/image_save_load_test.cpp
git commit -F - <<'EOF'
Write version 4 images with a sources section

Graphify: query Image save load checkFile headerOk readHeader
Serena: replace_symbol_body ao::ImageFormat::readHeader, insert_after_symbol checkGlobals

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
```

---

### Task 3: セッションのソースを保存し、ロードで戻す

**Files:**
- Modify: `runtime/src/Session.cpp`（`sessionImageSave`、`sessionImageLoad`、新しい `restoreMethodSources`）
- Test: `runtime/tests/image_save_load_test.cpp`、`runtime/tests/accept_abi_test.cpp`

**Interfaces:**
- Consumes: `ao::ImageSource`、`Image::save(..., const std::vector<ImageSource>*)`、`Image::load(..., std::vector<ImageSource>*)`（Task 2）、`rememberMethodSource(Oop method, Oop text, Oop replaced, const compiler::MethodImage* image)`、`clearMethodSources()`、`Str::toUtf8(Heap&, Oop)`、`Str::fromUtf8(CallContext&, std::string_view)`（`ao/Context.hpp`）
- Produces: `void restoreMethodSources(Session& s, const std::vector<ImageSource>& sources)`（`Session.cpp` の無名名前空間）

- [ ] **Step 1: Graphify と Serena で影響を取る**

```bash
graphify query "sessionImageSave sessionImageLoad rememberMethodSource clearMethodSources" --budget 1500
```

Serena: `find_symbol` `sessionImageSave`、`sessionImageLoad`、`rememberMethodSource`（`include_body=True`）。`find_referencing_symbols` `clearMethodSources`。

- [ ] **Step 2: テストを書く（赤）**

`runtime/tests/image_save_load_test.cpp` の無名名前空間に足す:

```cpp
// P18: a session image with the accepted methods P18Src>>a ("a\n  ^1") and, when two,
// P18Src>>b ("b\n  ^2"), in that order in the source table. The runtime is shut down after.
std::filesystem::path saveWithSources(const char* file, bool two) {
  const auto path = std::filesystem::path(testing::TempDir()) / file;
  EXPECT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  EXPECT_EQ(AO_OK, ao_accept_class("Object subclass: #P18Src\n  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                   "  category: 'P18'\n",
                                   &err))
      << err.message;
  EXPECT_EQ(AO_OK, ao_accept_method("P18Src", 0, "a\n  ^1", &err)) << err.message;
  if (two) {
    EXPECT_EQ(AO_OK, ao_accept_method("P18Src", 0, "b\n  ^2", &err)) << err.message;
  }
  EXPECT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  ao_runtime_shutdown();
  return path;
}

// Loads bytes from path with Image::load and expects the damaged-image refusal, nothing adopted.
void expectDamaged(const std::filesystem::path& path, const std::vector<char>& bytes) {
  ASSERT_TRUE(writeAll(path, bytes));
  Loaded image;
  std::string reason;
  EXPECT_FALSE(ao::Image::load(image.heap, image.roots, image.wk, path.string(), &reason));
  EXPECT_EQ("damaged image", reason);
  EXPECT_EQ(0u, image.heap.oldUsed());
}
```

ファイルの末尾に足す（ソース節の 1 件目は `sourcesAt + 8`。method 8、length 4、詰め物 4、"a\n  ^1" 6 バイト、詰め物 2 で 24 バイト。2 件目は `sourcesAt + 32`）:

```cpp
// P18 / SPEC §3.10, §3.11: an accepted method's source comes back after a save and a load, byte
// for byte (multi-byte UTF-8 too), for ao_browser_source and sourceCodeAt:. The doIt's text is
// not written.
TEST(ImageSaveLoad, AcceptedSourceSurvivesSaveAndLoad) {
  const auto path = std::filesystem::path(testing::TempDir()) / "p18-sources.aoimage";
  const char* japanese = "greet\n  \"挨拶を返す\"\n  ^'こんにちは'";
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #P18Src\n  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                   "  category: 'P18'\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("P18Src", 0, "a\n  ^1", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("P18Src", 1, japanese, &err)) << err.message;
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("\"P18DoItMarker\" 3 + 4", 21, AO_EVAL_PRINTIT, out, 64, &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  ao_runtime_shutdown();
  const std::vector<char> bytes = readAll(path);
  const std::string text(bytes.begin(), bytes.end());
  EXPECT_EQ(std::string::npos, text.find("P18DoItMarker"));
  EXPECT_NE(std::string::npos, text.find("a\n  ^1"));

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  char shown[256];
  ASSERT_EQ(AO_OK, ao_browser_source(ao_browser_class_id("P18Src"), 0, "a", shown, 256));
  EXPECT_STREQ("a\n  ^1", shown);
  ASSERT_EQ(AO_OK, ao_browser_source(ao_browser_class_id("P18Src"), 1, "greet", shown, 256));
  EXPECT_STREQ(japanese, shown);
  ASSERT_EQ(AO_OK, ao_eval("(P18Src sourceCodeAt: #a) size", 30, AO_EVAL_PRINTIT, out, 64, &err))
      << err.message;
  EXPECT_STREQ("6", out);
  ao_runtime_shutdown();
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: a source for an object that is not a CompiledMethod.
TEST(ImageSaveLoad, RefusesSourceForAnObjectThatIsNoMethod) {
  const auto path = saveWithSources("p18-not-method.aoimage", false);
  ImageSurgery image(path);
  const std::size_t at = sourcesAt(image.bytes);
  image.setWord(at + 8, image.record("Smalltalk"));
  expectDamaged(path, image.bytes);
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: a source whose method offset is past the heap.
TEST(ImageSaveLoad, RefusesSourceOutsideTheHeap) {
  const auto path = saveWithSources("p18-outside.aoimage", false);
  ImageSurgery image(path);
  image.setWord(sourcesAt(image.bytes) + 8, image.header.heapBytes + 8);
  expectDamaged(path, image.bytes);
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: the same method twice.
TEST(ImageSaveLoad, RefusesRepeatedSourceMethod) {
  const auto path = saveWithSources("p18-repeat.aoimage", true);
  ImageSurgery image(path);
  const std::size_t at = sourcesAt(image.bytes);
  image.setWord(at + 32, image.word(at + 8));
  expectDamaged(path, image.bytes);
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: a length that runs past the file.
TEST(ImageSaveLoad, RefusesSourceRunningPastTheFile) {
  const auto path = saveWithSources("p18-past.aoimage", false);
  ImageSurgery image(path);
  putU32(image.bytes, sourcesAt(image.bytes) + 16, 0xFFFFu);
  expectDamaged(path, image.bytes);
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: padding that is not 0 (after the length, and after the bytes).
TEST(ImageSaveLoad, RefusesNonZeroSourcePadding) {
  const auto path = saveWithSources("p18-pad.aoimage", false);
  ImageSurgery image(path);
  const std::size_t at = sourcesAt(image.bytes);
  std::vector<char> bytes = image.bytes;
  putU32(bytes, at + 20, 1);
  expectDamaged(path, bytes);
  bytes = image.bytes;
  bytes[at + 24 + 6] = 'x';
  expectDamaged(path, bytes);
  std::filesystem::remove(path);
}

// P18 / SPEC §3.11: a sourcesBytes the file's size does not agree with.
TEST(ImageSaveLoad, RefusesSourcesBytesThatDisagreeWithTheFile) {
  const auto path = saveWithSources("p18-size.aoimage", false);
  ImageSurgery image(path);
  ao::ImageFormat::ImageHeader header = image.header;
  header.sourcesBytes += 8;
  ao::ImageFormat::writeHeader(reinterpret_cast<std::byte*>(image.bytes.data()), header);
  expectDamaged(path, image.bytes);
  std::filesystem::remove(path);
}
```

`ImageSurgery` は無名名前空間の後半（`RefusesVersionOneWithReason` より後）で定義されている。上のテストは `ImageSurgery` の定義より後ろ（ファイルの末尾）に置く。`saveWithSources` と `expectDamaged` は `ImageSurgery` を使わないので、先頭の無名名前空間でよい。

`runtime/tests/accept_abi_test.cpp` の `ClassDefinitionThenImageDropsSourceText` を `ClassDefinitionThenImageKeepsSourceText` に改名し（Serena `rename_symbol` は TEST マクロに効かないので、テスト名の文字列だけ手で変える）、ロードのあとの検査を置き換える:

old:
```cpp
  // SPEC §3.10: 読み込んだイメージのメソッドはソースが無い。AO_ERR_NOSOURCE とプレースホルダで、
  // プレースホルダを Accept しても本体は変わらない（self を返す空メソッドにならない）。
  char shown[256];
  ASSERT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("P9Foo"), 0, "foo", shown, 256));
  EXPECT_STREQ("\"P9Foo>>foo source not available\"", shown);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("P9Foo", 0, shown, &err));
  EXPECT_STRNE("", err.message);
  ASSERT_EQ(AO_OK, ao_eval("P9Foo new foo", 13, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
```
new:
```cpp
  // SPEC §3.10 (P18): 読み込んだイメージのメソッドは、保存したときのソースを持つ。
  char shown[256];
  ASSERT_EQ(AO_OK, ao_browser_source(ao_browser_class_id("P9Foo"), 0, "foo", shown, 256));
  EXPECT_STREQ("foo\n  ^x\n", shown);
  ASSERT_EQ(AO_OK, ao_accept_method("P9Foo", 0, shown, &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_eval("P9Foo new foo", 13, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
```

`grep -n "ClassDefinitionThenImageDropsSourceText" -r SPEC.md docs runtime` で残りの参照を探し、SPEC §4.1 のものを新しい名前に直す（Task 1 で足した行は新しい名前を使っている）。

- [ ] **Step 3: 赤を確かめる**

Run: `./scripts/build.sh 2>&1 | tail -3 && ctest --test-dir build --output-on-failure -R 'AcceptedSourceSurvives|RefusesSource|RefusesRepeated|RefusesNonZero|ClassDefinitionThenImageKeeps'`
Expected: FAIL。`AcceptedSourceSurvivesSaveAndLoad` と `ClassDefinitionThenImageKeepsSourceText` は `AO_ERR_NOSOURCE`（ソースが戻らない）。壊れ方のテストは `sourcesAt + 8` に組が無い（`count` 0）ので、書き換えた先が節の外になり、期待どおりに落ちないか、範囲外の書き込みで落ちる。

- [ ] **Step 4: 保存でソースを渡す**

`runtime/src/Session.cpp` の `sessionImageSave` の `const std::size_t halted = …` の前に足す（Serena `replace_symbol_body` で `sessionImageSave` を書き直す）:

```cpp
  // SPEC §3.10, §3.11 (P18): the table's sources go to the sources section; the doIt's do not.
  // Nothing here or in Image::save collects, so the methods stay where they are.
  std::vector<ImageSource> sources;
  try {
    sources.reserve(g_session->methodSources.size());
    for (const auto& entry : g_session->methodSources) {
      if (entry->text.isHeap() && (g_session->heap.flags(entry->text) & kFlagBytes) != 0) {
        sources.push_back(ImageSource{entry->method, Str::toUtf8(g_session->heap, entry->text)});
      }
    }
  } catch (const std::bad_alloc&) {
    return 1;
  }
```

`return Image::save(...)` の呼び出しに `&sources` を足す:

```cpp
  return Image::save(g_session->heap, g_session->roots, g_session->wk, path, nullptr, halted,
                     &sources)
             ? 0
             : 1;
```

- [ ] **Step 5: ロードでソース表を作り直す**

`sessionImageLoad` の前（無名名前空間の中、`rememberMethodSource` の宣言が見える位置）に足す:

```cpp
// SPEC §3.10 (P18): the loaded image's sources go back in the table, each without blocks or debug
// info, in the section's order. The methods stay pinned while their Strings are made, which may
// collect. Out of memory stops there: the methods left have no entry (a placeholder) and the load
// still succeeds.
void restoreMethodSources(Session& s, const std::vector<ImageSource>& sources) {
  if (sources.empty() || s.ctx == nullptr) {
    return;
  }
  CallContext& ctx = *s.ctx;
  std::vector<Oop> methods;
  try {
    methods.reserve(sources.size());
    for (const ImageSource& src : sources) {
      methods.push_back(src.method);
    }
    ctx.roots.pinRange(methods.data(), methods.size());
  } catch (const std::bad_alloc&) {
    return;
  }
  for (std::size_t i = 0; i < methods.size(); ++i) {
    const Oop text = Str::fromUtf8(ctx, sources[i].text);
    if (!text.isHeap()) {
      s.heap.clearOutOfMemory();
      break;
    }
    // Does not collect: text stays valid until it is in a rooted slot.
    rememberMethodSource(methods[i], text, Oop{}, nullptr);
  }
  ctx.roots.unpinRange(methods.data(), methods.size());
}
```

`sessionImageLoad` を書き換える:

- `Image::load(next->heap, next->roots, next->wk, path, reason)` → `std::vector<ImageSource> sources;` を前に置き、`Image::load(next->heap, next->roots, next->wk, path, reason, &sources)`
- `clearMethodSources();` の直後に `restoreMethodSources(*g_session, sources);`

`rememberMethodSource` は `session()`（`g_session`）に入れるので、差し替えの後に呼ぶ。`ctx.roots` が `s.roots` と同じ `Roots` であることを `CallContext` の定義で確かめる。違えば `s.roots` で pin する。

- [ ] **Step 6: 緑を確かめる**

Run: `./scripts/build.sh 2>&1 | tail -3 && ctest --test-dir build --output-on-failure -R 'ImageSaveLoad\.|ImageSave\.|AcceptAbi\.|SessionAbi\.'`
Expected: すべて PASS。`ImageSaveDoesNotTraceSnapshotOrBlockSlots` も緑（doIt の項目は書かない）。

- [ ] **Step 7: 全体を回し、コミットする**

Run: `ctest --test-dir build --output-on-failure 2>&1 | tail -3`
Expected: `100% tests passed`

```bash
git add runtime/src/Session.cpp runtime/tests/image_save_load_test.cpp runtime/tests/accept_abi_test.cpp SPEC.md
git commit -F - <<'EOF'
Save the session's method sources and restore them on load

Graphify: query sessionImageSave sessionImageLoad rememberMethodSource
Serena: replace_symbol_body sessionImageSave, sessionImageLoad

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
```

---

### Task 4: ロードのあとの操作とチェンジセット

Review Focus 1〜4 と、戻ったセットの File Out。実装はたぶん要らない（Task 3 の結果を固定するテスト）。落ちたら、原因を `Session.cpp` の項目の扱いで直す。

**Files:**
- Test: `runtime/tests/accept_abi_test.cpp`、`runtime/tests/changes_abi_test.cpp`
- Modify（落ちたときだけ）: `runtime/src/Session.cpp`

**Interfaces:**
- Consumes: Task 3 の保存とロード。`changes_abi_test` の `ChangesAbi` フィクスチャ（`bootWithModel()`、`defineClass(name, super)`、`accept(cls, meta, src)`、`newSet(name)`、`tempPath(name)`、`fileOut(set, name)`、`afterHeader(text)`）
- Produces: なし

- [ ] **Step 1: accept_abi_test にテストを書く**

`runtime/tests/accept_abi_test.cpp` の無名名前空間（無ければファイル先頭の include の後ろに作る）に足す:

```cpp
// P18: saves the running session to path, boots again and loads it.
void p18SaveAndReload(const char* path) {
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path, &err)) << err.message;
}

void p18Define(const char* ivars) {
  AoSpan err{};
  const std::string def = std::string("Object subclass: #P18Re\n  instanceVariableNames: '") +
                          ivars + "'\n  classVariableNames: ''\n  poolDictionaries: ''\n"
                          "  category: 'P18'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def.c_str(), &err)) << err.message;
}

std::string p18Source(const char* selector) {
  char shown[256] = "";
  const int rc = ao_browser_source(ao_browser_class_id("P18Re"), 0, selector, shown, 256);
  return rc == AO_OK ? std::string(shown) : "rc " + std::to_string(rc);
}

std::string p18PrintIt(const char* expr) {
  char out[64] = "";
  AoSpan err{};
  const int rc = ao_eval(expr, static_cast<int>(std::strlen(expr)), AO_EVAL_PRINTIT, out, 64, &err);
  return rc == AO_OK ? std::string(out) : std::string("error: ") + err.message;
}
```

ファイルの末尾に足す:

```cpp
// P18 Review Focus 1: a class loaded from an image changes shape; its methods are compiled again
// from the restored sources, keep working and keep their source.
TEST(AcceptAbi, ReshapeAfterLoadRecompilesFromRestoredSource) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  p18Define("x");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("P18Re", 0, "seven\n  ^7", &err)) << err.message;
  p18SaveAndReload("p18-reshape.aoimage");
  p18Define("x y");
  EXPECT_EQ("7", p18PrintIt("P18Re new seven"));
  EXPECT_EQ("seven\n  ^7", p18Source("seven"));
  ao_runtime_shutdown();
  std::remove("p18-reshape.aoimage");
}

// P18 Review Focus 2: the restored entries are saved again by the next save.
TEST(AcceptAbi, SourcesSurviveTwoRoundTrips) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  p18Define("");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("P18Re", 0, "two\n  ^2", &err)) << err.message;
  p18SaveAndReload("p18-twice.aoimage");
  p18SaveAndReload("p18-twice.aoimage");
  EXPECT_EQ("two\n  ^2", p18Source("two"));
  ao_runtime_shutdown();
  std::remove("p18-twice.aoimage");
}

// P18 Review Focus 3: a method accepted again after the load saves its new source only.
TEST(AcceptAbi, ReacceptAfterLoadSavesTheNewSource) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  p18Define("");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("P18Re", 0, "n\n  ^1", &err)) << err.message;
  p18SaveAndReload("p18-reaccept.aoimage");
  ASSERT_EQ(AO_OK, ao_accept_method("P18Re", 0, "n\n  \"P18Newer\" ^2", &err)) << err.message;
  p18SaveAndReload("p18-reaccept.aoimage");
  EXPECT_EQ("n\n  \"P18Newer\" ^2", p18Source("n"));
  EXPECT_EQ("2", p18PrintIt("P18Re new n"));
  ao_runtime_shutdown();
  std::ifstream in("p18-reaccept.aoimage", std::ios::binary);
  const std::string bytes((std::istreambuf_iterator<char>(in)), {});
  EXPECT_EQ(std::string::npos, bytes.find("n\n  ^1"));
  std::remove("p18-reaccept.aoimage");
}

// P18 Review Focus 4: a method removed after the load is not in the next save's sources.
TEST(AcceptAbi, RemovedMethodAfterLoadIsNotSaved) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  p18Define("");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("P18Re", 0, "gone\n  \"P18Gone\" ^0", &err)) << err.message;
  p18SaveAndReload("p18-remove.aoimage");
  ASSERT_EQ(AO_OK, ao_remove_method(ao_browser_class_id("P18Re"), 0, "gone", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_image_save("p18-remove.aoimage"));
  ao_runtime_shutdown();
  std::ifstream in("p18-remove.aoimage", std::ios::binary);
  const std::string bytes((std::istreambuf_iterator<char>(in)), {});
  EXPECT_EQ(std::string::npos, bytes.find("P18Gone"));
  std::remove("p18-remove.aoimage");
}
```

`<fstream>` と `<iterator>` を include に足す（`<cstdio>`、`<cstring>`、`<string>` はある）。補助関数は既存の名前とぶつからないよう `p18` で始める。`ao_remove_method` の引数の順は `bridge/ao_abi.h` で確かめる（クラス ID、meta、セレクタ、`AoSpan*`）。

- [ ] **Step 2: changes_abi_test にテストを書く**

`runtime/tests/changes_abi_test.cpp` の `SavedImageKeepsSetsCurrentAndIds` の後ろに足す:

```cpp
// P18 / SPEC §3.9: a set restored from a saved image files its methods out with their sources,
// not `source not available` comments.
TEST_F(ChangesAbi, SavedImageFilesOutTheRestoredSources) {
  const std::int64_t set = ao_changes_current();
  defineClass("P18Kept", "Object");
  accept("P18Kept", 0, "kept\n  ^#kept");
  const std::string path = tempPath("p18-fileout.aoimage");
  ASSERT_EQ(AO_OK, ao_image_save(path.c_str()));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path.c_str(), &err)) << err.message;
  const std::string body = afterHeader(fileOut(set, "p18-restored.st"));
  EXPECT_NE(std::string::npos, body.find("!P18Kept methodsFor: ")) << body;
  EXPECT_NE(std::string::npos, body.find("kept\n  ^#kept! !\n")) << body;
  EXPECT_EQ(std::string::npos, body.find("source not available")) << body;
  std::filesystem::remove(path);
}
```

- [ ] **Step 3: 回す**

Run: `./scripts/build.sh 2>&1 | tail -3 && ctest --test-dir build --output-on-failure -R 'ReshapeAfterLoad|SourcesSurviveTwo|ReacceptAfterLoad|RemovedMethodAfterLoad|SavedImageFilesOut'`
Expected: PASS。落ちたら `superpowers:systematic-debugging` で原因を取り、`Session.cpp` の項目の扱い（`moveMethodSource`、`forgetMethodSource`、`rememberMethodSource`）を Serena で直す。SPEC に無い振る舞いが要るなら、先に SPEC §3.10 を直す。

- [ ] **Step 4: コミットする**

```bash
git add runtime/tests/accept_abi_test.cpp runtime/tests/changes_abi_test.cpp
git commit -F - <<'EOF'
Pin restored sources across reshape, re-accept, removal and File Out

Graphify: path sessionImageLoad moveMethodSource
Serena: find_referencing_symbols rememberMethodSource

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
```

---

### Task 5: Change Sorter と CLI

**Files:**
- Test: `app/AoTests/ChangeSorterTests.swift`、`runtime/tests/cli_test.sh`

**Interfaces:**
- Consumes: Task 3。`ChangeSorterTests` の `boot()`、`acceptClass(_:)`、`acceptMethod(_:_:)`、`ChangeSorterWindow.selectRows(set:class:method:)`、`sourceText`
- Produces: なし

- [ ] **Step 1: Swift のテストを書く**

`app/AoTests/ChangeSorterTests.swift` の `testImageLoadBringsBackTheSavedSetsInTheWindow` の後ろに足す:

```swift
  // SPEC §3.9 Change Sorter (P18): after a load the restored set's method shows its source.
  func testOpenImageShowsTheRestoredMethodSource() {
    boot()
    acceptClass("P18Shown")
    acceptMethod("P18Shown", "shown\n  ^#shown")
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p18-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(ao_image_save(url.path), Int32(AO_OK))
    var err = AoSpan()
    XCTAssertEqual(ao_image_load(url.path, &err), Int32(AO_OK), spanMessage(err))
    let sorter = ChangeSorterWindow()
    sorter.noteImageLoaded()
    sorter.selectRows(set: 0, class: 0, method: 0)
    XCTAssertEqual(sorter.sourceText, "shown\n  ^#shown")
  }
```

- [ ] **Step 2: CLI のテストを書く**

`runtime/tests/cli_test.sh` の `image_save)` の `run 0 "$AO" image load two.aoimage` の直後に足す:

```sh
    # SPEC §3.11 (P18): ao image save writes version 4 with an empty sources section.
    [ "$(od -An -tu2 -j4 -N2 two.aoimage | tr -d ' ')" = 4 ] || fail "image version is not 4"
    [ "$(od -An -tu8 -j40 -N8 two.aoimage | tr -d ' ')" = 8 ] || fail "sources section is not 8 bytes"
```

- [ ] **Step 3: 回す**

Run: `./scripts/build.sh 2>&1 | tail -3 && ctest --test-dir build --output-on-failure -R 'cli' && B=$PWD/build && swift test --package-path app --filter 'ChangeSorterTests/testOpenImageShowsTheRestoredMethodSource' -Xlinker -force_load -Xlinker $B/runtime/libao_runtime.a -Xlinker -force_load -Xlinker $B/compiler/libao_compiler.a -Xlinker -lc++ 2>&1 | grep -E "error:|Executed"`
Expected: PASS（`Executed 1 test, with 0 failures`）

- [ ] **Step 4: コミットする**

```bash
git add app/AoTests/ChangeSorterTests.swift runtime/tests/cli_test.sh
git commit -F - <<'EOF'
Show restored sources in the Change Sorter and check the CLI image version

Graphify: query ChangeSorterTests cli_test image_save
Serena: insert_after_symbol ChangeSorterTests/testImageLoadBringsBackTheSavedSetsInTheWindow

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
```

---

### Task 6: CHANGELOG、受け入れ、グラフ

**Files:**
- Modify: `CHANGELOG.md`、`SPEC.md`（§6 P18 の小節の `[ ]` → `[x]`）、`graphify-out/`

- [ ] **Step 1: CHANGELOG に書く**

`CHANGELOG.md` の `## [Unreleased]` の小節の `### Changed` の最後の箇条の後ろに足す:

```markdown
- Method sources are saved in the image (phase P18, version 1.7.0). `.aoimage` is now version 4: a sources section after the records holds the source of every method accepted or filed in with File In…, and Open Image… brings it back to the Browser, `sourceCodeAt:`, the Change Sorter and File Out. Version 3 images still load, without sources; 1.6.0 and earlier refuse version 4 images (`unsupported image version 4`). The Debugger still shows no source positions or temp names for loaded methods until they are accepted again.
```

- [ ] **Step 2: 全体を回す**

Run（バックグラウンド、10 分を超えうる）: `./scripts/test.sh > /tmp/p18-test.log 2>&1; echo exit=$?`
Expected: `exit=0`、`100% tests passed`、XCTest `with 0 failures`。Kernel 走査（`AllKernelMethodsAreNative` 系）も緑。

- [ ] **Step 3: §6 を [x] にし、グラフを更新してコミットする**

SPEC §6 の P18 の小節の 7 項目を `[x]` にする（それぞれのテスト名が緑であることを Step 2 のログで確かめてから）。

```bash
graphify update .
git add CHANGELOG.md SPEC.md
git commit -F - <<'EOF'
Record P18 in the CHANGELOG and check off its acceptance

Graphify: update .
Serena: none (documents)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
git add graphify-out
git commit -F - <<'EOF'
Refresh the knowledge graph after P18

Graphify: update .
Serena: none (generated files)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>
EOF
```

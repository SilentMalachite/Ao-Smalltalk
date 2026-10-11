# P16 チェンジセットと Change Sorter — 実装計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Accept と削除をセッションのチェンジセットに記録し、AppKit の Change Sorter で見る・作る・現在のセットを選ぶ・項目を忘れる/移す・File Out する。File In… は .st を読み込み、メソッドにソースを持たせて現在のセットに記録する。

**Architecture:** 記録は C++ のセッションメモリ（`Session::changes`。クラス名・セレクタ・種類だけで OOP を持たない）に置き、GC もイメージも触らない。記録の入口は 4 つの `note*` 関数で、Accept の唯一の経路 `acceptInto`、チャンクの適用 `applyChunkActions` / `applyMethodsFor`、`removeMethodOf`、`removeClassOf` の成功の直後で呼ぶ。窓は `ao_changes_*` の読みと、既存の `ao_browser_source` / `ao_browser_class_definition` だけでソースと定義を出す。File In… は `FileInMode::Recorded` で、Accept と同じくソースを残し、クラス定義は Accept と同じ規則（既存クラスを保つか形を変える）で入れる。

**Tech Stack:** C++20（runtime）、GoogleTest、Swift 6 AppKit、XCTest。

**Spec:** `SPEC.md`（Task 1 で P16 を足す）。§2.3 P16、§3.9「Change Sorter（P16）」、§3.10「チェンジセット（P16）」、§4 のテスト名、§6「チェンジセット」。提案と決定はローカルの `docs/plans/p16-change-sets.html`（`docs/plans/` は gitignore されている）。決定は次の 4 つで、どれも提案どおり。

| # | 問い | 決定 |
|---|---|---|
| 1 | セッションが持てるチェンジセットの数 | 複数。現在のセットは 1 つ |
| 2 | File Out は削除をどう書くか | コメントのチャンク（`"Removed: Counter>>reset"!`）。File In は読み飛ばす |
| 3 | 項目の操作 | Remove from Change Set と Move to の両方 |
| 4 | File In… が読んだメソッド | ソースを残し、現在のセットに記録する |

## Global Constraints

- Kernel はネイティブのまま。Smalltalk 側に `ChangeSet` クラスもセレクタも足さない。`KernelScan.*` は緑のまま。
- `.aoimage` 形式を変えない。チェンジセットはイメージに書かない（Save Image… は書かない。boot と成功したロードは空の `Unnamed1` から）。
- `ao` CLI（`ao filein`）と vendor の LOAD_ORDER の file-in は変えない（ソースも記録も無い。`FileInMode::Plain` が既定値）。
- 記録は C++ のデータ（`std::string` と `std::map`）だけで、OOP もルートも持たない。`note*` は `noexcept`。記録のメモリが無くても Accept と削除の答えは変えない。
- ABI: 読み（`ao_changes_count`、`ao_changes_at`、`ao_changes_current`、`ao_changes_class_count`、`ao_changes_class_at`、`ao_changes_method_count`、`ao_changes_method_at`）は `AbiEntry` を取らず busy でも答える。書き込み（`ao_changes_new`、`ao_changes_set_current`、`ao_changes_drop`、`ao_changes_move`、`ao_changes_file_out`）と `ao_filein_file` は最外の入口。
- メッセージは SPEC の文字列そのまま: `change set name empty`、`change set name taken: <name>`、`change set failed`、`unknown change set`、`not in change set: <Class>`、`not in change set: <Class>>><selector>`、`same change set`、`cannot write file: <path>`、`file out failed`、`file-in failed`、`runtime is busy`。File In の失敗は最初のエラーのメッセージと区間（`cannot read: <path>` は 0-0）。
- 種類の印: Swift の一覧は `+`、`~`、`−`（U+2212）。C++ のテストの補助は ASCII の `-` を使う。
- セットの改名と削除、メソッドのスタンプと作者、Monticello 相当、Workspace の評価の記録はしない。
- Browser の振る舞いを変えない（Accept と削除が記録されるだけ）。ホットループに触れない（`docs/bench.md` は測り直さない）。
- Swift: AppKit が正、GUI は `@MainActor`、ランタイム呼び出しは `bridge/ao_abi.h` だけ、強制アンラップを増やさない。
- CLAUDE.md: 各タスクの前に Graphify（`graphify-out/GRAPH_REPORT.md`、必要なら `query` / `path`）と Serena（`find_symbol` / `find_referencing_symbols`）を取り、既存シンボルは `replace_symbol_body` / `insert_after_symbol` / `replace_content` で編集する。新規ファイルは作成後に `get_symbols_overview` を取る。C++ を Serena の外で大きく変えたら Serena の clangd を再起動する（Serena に restart ツールは無い。clangd の子プロセスを kill すると立ち上がり直す）。コミットは英語の短い命令形で、本文に `Graphify:` / `Serena:` の一行トレーラと `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`。最後に `/graphify . --update`。worktree で動かすサブエージェントは Serena を使わない（Serena の編集は main の作業木に当たる）。
- 頼まれていないリファクタをしない。`docs/superpowers/specs/` に設計書は足さない（理由はこの計画書と SPEC にある）。

## Review Focus

SPEC のテスト一覧が直接は触れないが、使う人がまず踏むもの（上から起きやすい順）。各行のテストは担当タスクに入れてある。

1. すでにあるクラスを含む .st を File In… する。クラスはそのまま（形が同じなら保たれ、違えば形を変える）で、ほかのメソッドは消えない。vendor の file-in と同じ「新しいクラスを作って名前を束縛し直す」にすると、ほかのメソッドが名前から消える。→ Task 6 `FileInKeepsTheMethodsOfAnExistingClass`
2. 文字列やコメントに `!` を含むメソッド、`"` や `!` を含むセット名を File Out して File In する。`!` はどこでも `!!`、見出しの `"` は `""` になり、読み戻すと元の文字列になる。→ Task 5 `FileOutDoublesBangsAndQuotes`、Task 6 `FiledOutSetFilesBackInAfterBoot`
3. Change Sorter を開いたまま Browser で Accept する。Change Sorter がキーウィンドウになったときに読み直し、新しい項目が出る。→ Task 8 `testBecomingKeyReadsTheSetsAgain`
4. Change Sorter が `Unnamed2` を選んでいる状態で Open Image… する。窓は空の `• Unnamed1` だけに戻り、古い ID を読まない。→ Task 8 `testImageLoadStartsTheWindowOverWithUnnamed1`、Task 9 `testOpenImageStartsAnOpenChangeSorterOver`
5. 長い評価の最中（RunLoop pump でボタンが押せる）に New… / Remove / File Out / File In… を押す。何も変えずに `runtime is busy` を返す。→ Task 2 `NewAndMakeCurrentAreRefusedWhileBusy`、Task 4 `DropAndMoveAreRefusedWhileBusy`、Task 5 `FileOutIsRefusedWhileBusy`、Task 6 `FileInIsRefusedWhileBusy`

---

## File Structure

| ファイル | 責務 | タスク |
|---|---|---|
| `SPEC.md` | P16 の仕様（§1.4、§2.3、§2.4、§3.9、§3.10、§4、§6） | 1, 10 |
| `docs/phases/P16.md` | フェーズの要約と受け入れ | 1, 10 |
| `bridge/ao_abi.h` | `AO_CHANGE_*`、`ao_changes_*`、`ao_filein_file` の宣言、busy の注記 | 2 |
| `runtime/src/ChangeSet.hpp`（新規） | 記録の型、種類の規則、`note*`、`sessionChanges*` の宣言（公開ヘッダでない） | 2, 3, 4, 5 |
| `runtime/src/ChangeSet.cpp`（新規） | 上の実装と File Out の書き出し | 2, 3, 4, 5 |
| `runtime/src/Session.hpp` / `Session.cpp` | `Session::changes`、`writeBuf` の公開、File Out の読み（`boundClassNamed` ほか）、`sessionFileInFile` | 2, 5, 6 |
| `runtime/include/ao/Compile.hpp` / `runtime/src/Compile.cpp` | 記録の呼び出し、`ChunkMode`、`FileInMode` | 3, 6 |
| `runtime/src/abi.cpp` | ABI の入口 | 2, 4, 5, 6 |
| `runtime/CMakeLists.txt` | `src/ChangeSet.cpp`、`tests/changes_abi_test.cpp` | 2 |
| `runtime/tests/changes_abi_test.cpp`（新規） | `ChangesAbi.*`、`ChangeKinds.*` | 2〜6 |
| `runtime/tests/chunk_filein_test.cpp` | `FiledOutCommentChunksAreSkipped` | 6 |
| `app/Ao/ChangeSorterModel.swift`（新規） | ABI の読み書きと選択の状態、`fileOutChangeSet` | 7 |
| `app/Ao/ChangeSorterWindow.swift`（新規） | 窓、一覧、ボタン、右クリックメニュー、シート | 8 |
| `app/Ao/MainMenu.swift` / `app/Ao/AoApp.swift` | File → File In… / File Out Change Set…、Tools → Change Sorter、`fileInChangeFile`、ロード時の初期化、文字の大きさ | 9 |
| `app/AoTests/ChangeSorterTests.swift`（新規） | P16 の XCTest | 7, 8, 9 |
| `app/AoTests/AcceptTests.swift` | File メニューの項目の並び | 9 |
| `PHASE`、`CHANGELOG.md`、`runtime/src/Runtime.cpp`、`compiler/src/Compiler.cpp`、両 `smoke_test.cpp`、`docs/README.md`、`graphify-out/` | 完了の記録と版 1.5.0 | 10 |

## 前提（コードで確かめた事実。`b35efe8`）

- メソッドの Accept はすべて `acceptInto`（`runtime/src/Compile.cpp:1437` の無名名前空間）を通る。`acceptMethodSource`（`ao_accept_method`）、`acceptMethodInto`（`ao_accept_method_id`）、`acceptMethodReplacing`（`ao_debug_accept`、`Session.cpp:1525`）がこれを呼ぶ。`acceptInto` の頭で `name`（クラス自身の名前。無名なら空）を計算済みで、成功の直前に `replaced`（置き換えた古いメソッド、無ければ空の `Oop{}`）を作る。
- Debugger の Accept はファイバの `CallContext` で走る。記録の入口が「ctx はセッションのものか」を見るなら、`&ctx.heap == &session()->heap` で比べる（`ctx` の同一性ではファイバを落とす）。
- `applyChunkActions(ctx, actions, errors, bool accept = false)`（同 `:1161`）がクラス定義を `acceptClassDef`（accept のとき）か `applyClassDef`（file-in）に、`methodsFor:` を `applyMethodsFor`（同 `:320`）に回す。呼び出し元は `acceptClassSource`（`accept=true`）、`applyChunks`、`fileInListedFile`（LOAD_ORDER）。`fileInString`（同 `:1322`）は `applyChunks` を呼び、`fileInFile`（同 `:1329`）は `fileInString` を呼ぶ。`fileInFile` の呼び出し元は CLI の `runFileIn`（`ao_main.cpp:182`）だけ。
- `applyMethodsFor` は `installMethod` だけでソースを残さない。`ownClassName`（同 `:422`）は `applyMethodsFor` より後ろで定義されている（同じ無名名前空間）。`isClassShaped` は `ao/Lookup.hpp` にある。
- `applyClassDef` は既存の名前でも新しいクラスを作って名前を束縛し直す（メソッドを引き継がない）。`acceptClassDef` は既存クラスを保つ（`recategorizeClass`）か形を変える（`reshapeClass`、メソッドを持ち越す）。`namesBehavior(ctx, name)` は公開で、割り当てない。
- `removeMethodOf`（同 `:1584`）と `removeClassOf`（同 `:1620`）は、検査のあとで変更し、最後に `forgetMethodSource` / `forgetMethodSourcesOf` を呼ぶ。どちらもヒープに割り当てない（記録は C++ のメモリなのでこの約束を破らない）。`own` は名前の枠の文字列（無名なら空）。
- `Session`（`runtime/src/Session.hpp`）は boot（`Session(true)`）とロード（`Session(false)`）のたびに作り直される。失敗したロードでは新しい `Session` を捨て、今のセッションが残る。だからメンバを既定構築するだけで「boot とロードで初期化、失敗したロードでは残る」が成り立つ。
- `writeBuf`（`Session.cpp:625`、無名名前空間）が §3.10 のバッファの規則を実装している。`definitionOf(Session&, const ClassRow&)`（同 `:923`）は `row.cls` と `row.name`（= `classNameOf(heap, cls)`）だけを読み、カテゴリの `!` と `'` を重ねる。呼び出し元は `browserClassDefinition`（同 `:1907`）だけ。
- `methodSource(Oop method, std::string&)` は公開。`MethodDictionary::at(heap, dict, key)` は辞書でない枠を「無い」と読む。`WellKnown::named` と `findSymbol` は割り当てない。
- チャンクの分割（`compiler/src/ChunkParser.cpp` の `splitChunks`）: `!!` はどこでも 1 つの `!`。文字列とコメントの外の単独の `!` がチャンクを終える。チャンクの先頭の空白は読み飛ばす。空のチャンクは直前の `methodsFor:` の節を閉じる。見出しでも定義でもない節の外のチャンクは `DoIt` で、`applyChunkActions` は何もしない。だからコメントだけのチャンクは読み飛ばされる。
- ABI の入口は `guarded`、`AbiEntry`、`clearSpan`、`setMessage`、`fillSpan`（`runtime/src/abi.cpp`）。`clearSpan` などは 2 つ目の無名名前空間（`:110` 以降）にあるので、それを使う入口はファイルの後ろに置く。
- Swift: `spanMessage(AoSpan)`（`WorkspaceWindow.swift:114`）、`BrowserModel.classID(named:)`、`ToolTextSize.font(fixedPitch:)`、`UniformFont`、`TranscriptWindow.append` / `.text`、`LaunchSet.make()`（boot する）。テストの差し替えクロージャは捕まえたローカル変数を書き換えてよい（`DebuggerWindowTests.swift:887`）。`AcceptTests.testFileMenuImageCommandsAndShowHierarchy`（`AcceptTests.swift:1386`）が File メニューの並びを `["Save Image…", "Open Image…", "", "Close"]` と固定している。
- テストの実行: `./scripts/build.sh` のあと `ctest --test-dir build -R <正規表現> --output-on-failure`。Swift は `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter <名前>`。全件は `./scripts/test.sh`、ASan は `./scripts/test.sh --asan`。

---

### Task 0: ブランチ

- [ ] **Step 1: main から作業ブランチを切る**

```bash
git switch -c feat/p16-change-sets
```

---

### Task 1: SPEC を先に直す（P16 の追記）

**Files:**
- Modify: `SPEC.md`（§1.4、§2.3、§2.4、§3.9、§3.10、§4、§6）
- Create: `docs/phases/P16.md`

**Interfaces:**
- Produces: 以降のタスクが従う文字列、ABI の名前と答え、テスト名。

- [ ] **Step 1: Graphify と Serena で影響を確かめる**

`graphify-out/GRAPH_REPORT.md` の Community 31「Compile.cpp」、Community 23「Session.cpp」、Community 104「BrowserWindow」、ハイパーエッジ「SPEC 3.9 removal flow」を読む。`graphify path removeMethodOf ao_remove_method` で削除の経路を取る。Serena は `activate_project` のあと `find_referencing_symbols` で `acceptInto`、`applyChunkActions`、`removeMethodOf`、`removeClassOf` の参照を取る（上の「前提」と一致することを確かめる）。

- [ ] **Step 2: §1.4 の非目標を直す**

`- チェンジセット UI / モンティチェロ相当` を次にする。

```markdown
- モンティチェロ相当（チェンジセットと Change Sorter は v1 のあとの P16 で足す。§3.9）
```

同じ節の最後の段落（「デバッガは v1 のあとのフェーズで足す…」）の末尾に次の 1 文を足す。

```markdown
P16 は、Accept と削除をセッションのチェンジセットに記録し、Change Sorter で見て、File Out / File In… する。
```

- [ ] **Step 3: §2.3 の表と注記**

表の P15 の行の後ろに足す。

```markdown
| P16 | チェンジセット | Accept と削除をチェンジセットに記録し、Change Sorter で見る・作る・移す・File Out する。File In… で .st を読み込み、記録する（§3.9、§3.10） |
```

`P9 完了が v1。P10、P11、P12、P13、P14、P15 は v1 のあとのフェーズである。` を次にする。

```markdown
P9 完了が v1。P10、P11、P12、P13、P14、P15、P16 は v1 のあとのフェーズである。
```

- [ ] **Step 4: §2.4 の版**

最初の箇条の `P15（Debugger の編集）も同じく振る舞いを足すだけなので、その次のマイナー版 `1.4.0` に含める。` の直後に足す。

```markdown
P16（チェンジセット）も同じく振る舞いを足すだけなので、その次のマイナー版 `1.5.0` に含める。
```

§3.9「起動と同梱」の `P15 までは `1.4.0`` を `P15 までは `1.4.0`、P16 までは `1.5.0`` にする。

- [ ] **Step 5: §3.9 のメニューバー**

「メニューバー:」の箇条を次にする（File の行を足し、Tools に Change Sorter を足す）。

```markdown
- Ao / File / Edit / Smalltalk / Tools / Window / Help
- File: Save Image…, Open Image…, File In…, File Out Change Set…, Close（`⌘W`）（File In… と File Out Change Set… は下の「Change Sorter（P16）」）
- Tools: Browser, Transcript, Workspace, Change Sorter, Use Fixed Pitch, Make Text Bigger（`⌘+`）, Make Text Smaller（`⌘-`）, Actual Size（`⌘0`）
- Smalltalk: Do it, Print it, Inspect it, Debug it（`⌘⇧D`）, Interrupt（`⌘.`）, Accept, Remove Method…, Remove Class…（キー無し。下の「削除」）
```

- [ ] **Step 6: §3.9 に「Change Sorter（P16）」を足す**

§3.9 の「#### 起動と同梱」の直前に、次の小節をそのまま入れる。

````markdown
#### Change Sorter（P16）

Accept と削除をチェンジセットに記録し、Change Sorter で見る。モデルの状態はランタイムのセッションのメモリにあり（§3.10「チェンジセット」）、窓は AppKit にある。Smalltalk 側に `ChangeSet` クラスを作らない。

チェンジセット:

- チェンジセットは、名前と、クラスごとの項目の並びである。クラスの項目は、クラスの名前（そのクラス自身の名前）、定義の種類（無いことがある）、インスタンス側とクラス側のセレクタごとの種類を持つ。種類は Added（印 `+`）、Changed（`~`）、Removed（`−`）である。
- セッションは 1 つ以上のチェンジセットを持ち、そのうち 1 つが現在のセットである。boot と Open Image…（`ao_image_load` の成功）は、空のセット `Unnamed1` 1 つだけを現在のセットにして始め直す。失敗したロードは何も変えない。Save Image… はチェンジセットを書かない。
- セットの改名と削除、メソッドのスタンプと作者は無い。

記録:

- 次の操作が成功したとき、現在のセットの項目を 1 つ更新する。
  - メソッドの Accept（Browser の Accept、`ao_accept_method`、`ao_accept_method_id`、Debugger の Accept、`ao_accept_class` の `methodsFor:` のチャンクのメソッド）。そのセレクタのメソッドを置き換えたなら Changed、無かったなら Added。
  - クラス定義の Accept（`ao_accept_class`）。受け付ける前にその名前がクラス（Behavior）を指していたなら Changed、そうでなければ Added。形の変更で移したメソッドは記録しない（定義 1 件として記録する）。
  - File In…（`ao_filein_file`）で読んだクラス定義とメソッド。Accept と同じ規則で記録する。
  - Remove Method…（`ao_remove_method`）はメソッドの項目を、Remove Class…（`ao_remove_class`）はクラスの定義を、Removed にする。クラスを消すと、そのクラスのメソッドの項目は消える。
- 種類の規則: Added のあとの Accept は Added のまま、Changed のあとは Changed のまま、Removed のあとは Changed。Added のあとの削除は項目を消す（定義が Added のクラスを消すと、クラスの項目ごと消える）。Changed のあとの削除は Removed。定義の種類もメソッドの項目も残らないクラスの項目は消える。
- 記録しないもの: vendor の file-in、`ao filein`、`ao_filein_load_order`、Workspace の評価（`subclass:` の送信で作ったクラスも）、ロードしたイメージ、名前の無いクラスへの Accept、拒まれた Accept と削除。
- 記録のための C++ のメモリが無ければ記録しない。Accept と削除の答えは変えない。

窓:

- Tools → Change Sorter で開く。窓は 1 つだけで、閉じても次に開くと同じ窓が出る。タイトルは `Change Sorter`、サブタイトルは `<現在のセットの名前> is current`。
- 上段に 3 つの一覧を並べる。Change sets（作った順。現在のセットは名前の前に `• `）、Classes（選んだセットのクラス、初めて記録した順。名前の後ろに空白と定義の種類の印。定義が変わっていなければ印は無い）、Methods（選んだクラスのインスタンス側、続いてクラス側。それぞれセレクタのバイト順。クラス側は `class » <セレクタ>`。後ろに空白と種類の印）。
- 下段に、読むだけのソース枠と、エラー欄と、ボタン New…、Make Current、File Out… を置く。
- ソース枠: メソッドを選ぶと、そのメソッドのソース（`ao_browser_source` の答え。プレースホルダも）を出す。Removed なら `removed`。名前がクラスを指さないか、セレクタが無ければ `"<Class>>><selector> source not available"`（クラス側は `<Class> class`）。クラスだけを選ぶとその定義（`ao_browser_class_definition`）を出す。定義が Removed なら `removed`、名前がクラスを指さなければ `"<Class> definition not available"`。セットだけなら空。
- 開いたとき、キーウィンドウになったとき、ボタンとメニューの操作のあとに読み直す。選んでいたセットが一覧から消えたら（boot、ロード）、現在のセットを選ぶ。選んでいたクラスやメソッドがセットから消えたら、その選択を外す。
- New…: シートを出す。メッセージは `New Change Set`、名前の欄、ボタンは Create と Cancel。名前の欄の初期値は、まだどのセットも使っていない最小の `Unnamed<n>`（n は 1 から）。Create で `ao_changes_new` を呼ぶ。新しいセットが現在のセットになり、選ばれる。拒まれたら（空の名前、使用中の名前）エラー欄に理由を出す。
- Make Current: 選んだセットを現在のセットにする。選んだセットが現在のセットなら無効。失敗したらエラー欄に `Make Current failed`。
- File Out…: 選んだセットを、NSSavePanel のシートで選んだファイルに書く（名前の初期値は `<セットの名前>.st`）。失敗したらエラー欄に理由を出す。
- Classes と Methods の一覧の右クリックメニュー: Remove from Change Set、区切り、Move to（サブメニューに、選んだセット以外のセットの名前）。右クリックした行を対象にする。行の外なら項目は無効。ほかのセットが無ければ Move to は無効。
  - Remove from Change Set は項目を忘れるだけで、メソッドもクラスも変えない。クラスの行なら、メソッドの項目ごと忘れる。確認は出さない。
  - Move to は、項目を種類ごと移し先のセットに移す（§3.10「チェンジセット」の `ao_changes_move`）。
- 成功した操作はエラー欄を空にする。
- VoiceOver ラベルは `Change sets`、`Classes`、`Methods`、`Source`、`Error`。
- 文字の大きさ（Tools → Make Text Bigger ほか）はソース枠にも効く。

File メニュー:

- File In…: NSOpenPanel で .st を 1 つ選び、`ao_filein_file` を呼ぶ。結果を Transcript に出す。成功なら `filed in: <path>`。失敗なら `file-in failed: <path>` と、次の行に最初のエラー `<path>:<start>-<end>: <message>`（`ao filein` と同じ形）。失敗しても、それより前のチャンクの結果と記録は残る。開いている Change Sorter は読み直す。
- File Out Change Set…: 現在のセットを、NSSavePanel で選んだファイルに書く。失敗したら警告のアラート（メッセージ `Could not file out the change set`、補足にパスと理由）。

評価中（busy。§3.10）は、書き込み（New…、Make Current、Remove from Change Set、Move to、File Out…、File Out Change Set…、File In…）は何もせずに拒まれる。理由 `runtime is busy` を、Change Sorter のエラー欄、アラート、Transcript の行に出す。

File Out の形式（チャンク形式。§3.8）:

1. 見出しのコメント `"Change Set: <name>` 改行 `Date: <YYYY-MM-DD>` 改行 `Ao <版>"`（ローカルの日付。版は `ao_version`。名前の中の `"` は `""`）
2. 定義の種類が Added か Changed のクラスの定義（`ao_browser_class_definition` と同じ本文）。セットの中にスーパークラスがあれば、それを先に書く。ほかはセットの順。
3. クラスごと（セットの順）に、インスタンス側、クラス側の順で、`!<Class> methodsFor: 'user'!`（クラス側は `!<Class> class methodsFor: 'user'!`）と、種類が Added か Changed のメソッドのソースをセレクタのバイト順に、`!` と空行で区切って並べ、最後のメソッドを ` !` で閉じる。ソースのあるメソッドが無い側は書かない。
4. コメントのチャンク。Removed のメソッド `"Removed: <Class>>><selector>"`、Removed のクラス `"Removed class: <Class>"`、ソースの無いメソッド `"<Class>>><selector> source not available"`、名前がクラスを指さない定義 `"<Class> definition not available"` の順（それぞれセットの順、側、セレクタの順）。

チャンクの間は空行 1 つ（コメントのチャンクの間は改行 1 つ）。文字列とコメントの中も含め、すべての `!` を `!!` にする。ただし定義の本文は、すでにカテゴリの `!` と `'` を重ねてあるので、そのまま書く。コメントのチャンクは式のチャンク（§3.8）なので、File In… と `ao filein` は評価せずに読み飛ばす。

```smalltalk
"Change Set: Unnamed2
Date: 2026-10-11
Ao 1.5.0"!

Object subclass: #Counter
  instanceVariableNames: 'count'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Examples'!

!Counter methodsFor: 'user'!
increment
  count := (count ifNil: [0]) + 1.
  ^count! !

!Counter class methodsFor: 'user'!
new
  ^super new reset! !

"Removed: Counter>>reset"!
```
````

- [ ] **Step 7: §3.10 の必須エントリと busy の一覧**

「必須エントリ:」の箇条の `- メソッドとクラスの削除（…）` の後ろに足す。

```markdown
- チェンジセットと File In…（`ao_changes_*`、`ao_filein_file`。下の「チェンジセット（P16）」、§3.9「Change Sorter（P16）」）
```

「#### 再入と例外」の箇条の `- `ao_remove_method`、`ao_remove_class`` の後ろに足す。

```markdown
- `ao_filein_file`、`ao_changes_new`、`ao_changes_set_current`、`ao_changes_drop`、`ao_changes_move`、`ao_changes_file_out`
```

その下の段落の `` `ao_image_load`、`ao_remove_method`、`ao_remove_class` の理由は `runtime is busy` `` を次にする。

```markdown
`ao_image_load`、`ao_remove_method`、`ao_remove_class`、`ao_filein_file`、`ao_changes_new`、`ao_changes_drop`、`ao_changes_move`、`ao_changes_file_out` の理由は `runtime is busy`
```

同じ段落の busy でも呼べるものの並びの `ブラウザの読み取り（…）、` の後ろに足す。

```markdown
チェンジセットの読み（`ao_changes_count`、`ao_changes_at`、`ao_changes_current`、`ao_changes_class_count`、`ao_changes_class_at`、`ao_changes_method_count`、`ao_changes_method_at`。下の「チェンジセット（P16）」）、
```

- [ ] **Step 8: §3.10 の C ABI のコードブロック**

「#### C ABI」のコードブロックの `int ao_remove_class(int64_t class_id, AoSpan* err);` の後ろに足す。

```c
enum { AO_CHANGE_NONE = 0, AO_CHANGE_ADDED = 1, AO_CHANGE_CHANGED = 2, AO_CHANGE_REMOVED = 3 };
int ao_changes_count(void);
int ao_changes_at(int index, int64_t* set_id, char* name, int name_len);
int64_t ao_changes_current(void);
int ao_changes_new(const char* name, int64_t* set_id, AoSpan* err);
int ao_changes_set_current(int64_t set_id);
int ao_changes_class_count(int64_t set_id);
int ao_changes_class_at(int64_t set_id, int index, char* name, int name_len, int* kind);
int ao_changes_method_count(int64_t set_id, const char* class_name, int meta);
int ao_changes_method_at(int64_t set_id, const char* class_name, int meta, int index,
                         char* selector, int len, int* kind);
int ao_changes_drop(int64_t set_id, const char* class_name, int meta, const char* selector,
                    AoSpan* err);
int ao_changes_move(int64_t from_id, int64_t to_id, const char* class_name, int meta,
                    const char* selector, AoSpan* err);
int ao_changes_file_out(int64_t set_id, const char* path, AoSpan* err);
int ao_filein_file(const char* path, AoSpan* err);
```

- [ ] **Step 9: §3.10 に「チェンジセット（P16）」を足す**

「#### ソースはイメージに書かない」の直前に、次の小節を入れる。

```markdown
#### チェンジセット（P16）

- チェンジセット（§3.9「Change Sorter（P16）」）はセッションのメモリにある。持つのはクラスの名前、セレクタ、種類だけで、OOP を持たない。GC はたどらず、`ao_image_save` は書かない。boot と成功した `ao_image_load` は、空のセット `Unnamed1` 1 つを現在のセットにして始める。失敗したロードは変えない。セットの ID は、ひとつのプロセスの中で使い回さない（クラス ID と同じ）。
- 読み（`ao_changes_count`、`ao_changes_at`、`ao_changes_current`、`ao_changes_class_count`、`ao_changes_class_at`、`ao_changes_method_count`、`ao_changes_method_at`）は busy でも呼べる。件数を返す関数は、セッションが無い、未知のセット ID、`class_name` が NULL、`meta` が 0 と 1 以外のとき -1 を返す。`*_at` は文字列バッファの規則に従い、範囲外の index も `AO_ERR` である。出力（`set_id`、`kind`）は NULL でもよく（書かない）、`AO_ERR` では 0 を書く。`ao_changes_current` は現在のセットの ID、セッションが無ければ 0。
- 並び: セットは作った順、クラスは初めて記録した順、メソッドはセレクタのバイト順。`ao_changes_class_at` の `kind` は定義の種類で、定義が変わっていなければ `AO_CHANGE_NONE`。セットにそのクラスの項目が無ければ `ao_changes_method_count` は 0。
- 書き込み（`ao_changes_new`、`ao_changes_set_current`、`ao_changes_drop`、`ao_changes_move`、`ao_changes_file_out`）と `ao_filein_file` は最外の入口で、busy なら何もせずに `AO_ERR`（`err` があれば `runtime is busy`）を返す。`err` のある関数は、`AO_OK` で空文字、`AO_ERR` で理由（空にしない。`ao_filein_file` 以外は `start` と `end` は 0）を入れる。
- `ao_changes_new`: 空のセットを作って現在のセットにし、`set_id`（NULL でもよい）にその ID を書く（失敗なら 0）。理由は `change set name empty`（NULL か空文字）、`change set name taken: <名前>`、セッションが無いときは `change set failed`。
- `ao_changes_set_current`: 未知の ID、セッションが無い、busy なら `AO_ERR`。
- `ao_changes_drop`: 項目を忘れる。`selector` が NULL ならクラスの項目をメソッドの項目ごと忘れる（`meta` は読まない）。メソッドの項目を忘れて、定義の種類もメソッドの項目も残らないクラスの項目は消える。メソッドとクラスそのものは変えない。理由は `unknown change set`、`not in change set: <Class>`、`not in change set: <Class>>><selector>`（クラス側は `<Class> class>><selector>`）。`class_name` が NULL、`selector` があるときの 0 と 1 以外の `meta`、セッションが無いときは `change set failed`。
- `ao_changes_move`: 項目を種類ごと `to_id` のセットに移す。移し先に同じ項目があれば、移した種類で置き換える。移し先にクラスの項目が無ければ作る（定義の種類は無し）。クラスの項目を移すとき、移した項目に定義の種類が無ければ、移し先の定義の種類はそのまま。移し元の、空になったクラスの項目は消える。理由は `ao_changes_drop` のものと、`from_id` と `to_id` が同じなら `same change set`。どちらかが未知なら `unknown change set`。
- `ao_changes_file_out`: §3.9 の形式で `path` に書く（あれば置き換える）。理由は `unknown change set`、`cannot write file: <path>`。`path` が NULL、セッションが無いときは `file out failed`。
- `ao_filein_file`: 1 つのファイルを file-in する（§3.12 のチャンクの規則。`ao filein` と同じく、メソッドのエラーでは止まらず、ほかのエラーで止まる）。読んだメソッドは Accept と同じくソース表に項目を持つ（下の「ソースはイメージに書かない」）。クラス定義は `ao_accept_class` と同じ規則で入れる（既にあるクラスは保つか形を変え、メソッドを黙って捨てない。§3.9「クラス定義の Accept」）。読んだクラス定義とメソッドを現在のセットに記録する。エラーが 1 件でもあれば `AO_ERR` で、`err` に最初のエラーの区間（ファイルのバイト位置）とメッセージを入れる。読めないパスは `cannot read: <path>`（区間 0-0）。`path` が NULL、セッションが無い、例外のときは `file-in failed`。エラーより前のチャンクの結果と記録は残る。
```

- [ ] **Step 10: §3.10 のソース表の規則と `ao_browser_source`**

「#### ソースはイメージに書かない」の規則の 1 つ目を次にする。

```markdown
- 項目を入れるのは、Accept（`ao_accept_method` と、`ao_accept_class` の形の変更で移したメソッド）と、File In…（`ao_filein_file`。P16）で読んだメソッドだけである。vendor、`ao filein`、`ao_filein_load_order` の file-in、`ao_accept_class` の `methodsFor:` のチャンク、ロードしたイメージのメソッドは項目を持たない（プレースホルダ）。
```

`ao_browser_source` の段落の `ソースが無いのは、NativeMethod、イメージを読み込んだあとのメソッド、vendor と file-in（`ao_accept_class` の `methodsFor:` のチャンクを含む）で入れたメソッドである。` の直後に足す。

```markdown
File In…（`ao_filein_file`。P16）で入れたメソッドはソースを持つ。
```

- [ ] **Step 11: §4 のテスト名**

§4.1 の箇条の最後に足す。

```markdown
- チェンジセット（§3.9「Change Sorter（P16）」、§3.10「チェンジセット（P16）」）: `changes_abi_test` に、種類の規則（`ChangeKinds.AcceptAndRemovalRules`）、新しいセッションの空の `Unnamed1`（`NewSessionHasOneEmptyCurrentSet`）、セッションが無いときの答え（`NoSessionAnswersFailure`）、New と Make Current（`NewSetBecomesCurrent`、`NewSetRefusesEmptyAndTakenNames`、`UnknownSetIdIsRefused`）、バッファの規則（`ReadsFollowBufferRule`）、boot とロードで始め直し、失敗したロードでは残る（`BootAndImageLoadStartOver`）、記録（`AcceptRecordsAddedThenChangedInANewSet`、`ClassSideAcceptIsRecordedOnTheClassSide`、`ClassDefinitionAcceptIsChangedForAnExistingClass`、`RemoveMethodFollowsTheKindRules`、`RemoveClassFollowsTheKindRules`、`MethodsForChunkAcceptIsRecordedWithoutSource`、`DebuggerAcceptIsRecorded`、`RefusedAcceptAndRemovalRecordNothing`、`LoadOrderFileInRecordsNothing`）、忘れると移す（`DropForgetsTheEntryAndKeepsTheMethod`、`DropRefusesWhatTheSetDoesNotHold`、`MoveMethodEntryKeepsItsKind`、`MoveClassEntryMergesIntoTheTarget`、`MoveRefusesSameOrUnknownSet`）、File Out（`FileOutWritesHeaderDefinitionsMethodsAndRemovals`、`FileOutDefinesSuperclassesFirst`、`FileOutDoublesBangsAndQuotes`、`FileOutWritesACommentForAMethodWithoutSource`、`FileOutWritesAClassRemovalAsAComment`、`FileOutRefusesUnknownSetAndUnwritablePath`）、File In（`FileInKeepsSourcesAndRecordsInTheCurrentSet`、`FileInKeepsTheMethodsOfAnExistingClass`、`FileInErrorAnswersTheFirstErrorAndKeepsEarlierChunks`、`FileInRefusesAnUnreadablePath`、`FiledOutSetFilesBackInAfterBoot`）、busy の拒否（`NewAndMakeCurrentAreRefusedWhileBusy`、`DropAndMoveAreRefusedWhileBusy`、`FileOutIsRefusedWhileBusy`、`FileInIsRefusedWhileBusy`）。`chunk_filein_test` に File Out のコメントのチャンクを読み飛ばす（`FiledOutCommentChunksAreSkipped`）
```

§4.3 の箇条の最後に足す。

```markdown
- Change Sorter（§3.9「Change Sorter（P16）」）: `ChangeSorterTests` に、モデル（`testNewSessionListsUnnamed1AsCurrentAndSelected`、`testAcceptedMethodShowsItsKindAndSource`、`testRemovedMethodAndRemovedClassShowRemoved`、`testNewSetIsCurrentAndNextFreeNameSkipsTakenNames`、`testMakeCurrentRemoveAndMoveUpdateTheLists`）、窓（`testWindowListsSetsWithTheCurrentMarkAndSubtitle`、`testSelectingAClassAndMethodShowsMarksAndSource`、`testNewButtonAsksForTheNextFreeNameAndSelectsTheNewCurrentSet`、`testMakeCurrentMovesTheMark`、`testEntryMenuOffersRemoveAndMoveToTheOtherSets`、`testRemoveFromMenuForgetsTheClassEntryButKeepsTheClass`、`testFileOutButtonWritesTheSelectedSet`、`testRefusedFileOutShowsTheReason`、`testBecomingKeyReadsTheSetsAgain`、`testImageLoadStartsTheWindowOverWithUnnamed1`、`testPaneAccessibilityLabels`）、メニュー（`testToolsChangeSorterOpensOneWindow`、`testFileInWritesTheFiledInLineAndRecords`、`testFailedFileInWritesTheFailedLineAndTheFirstError`、`testFileOutChangeSetFailureShowsAnAlert`、`testOpenImageStartsAnOpenChangeSorterOver`）。`AcceptTests.testFileMenuImageCommandsAndShowHierarchy` に File In… と File Out Change Set…
```

- [ ] **Step 12: §6 にチェックリストを足す**

「### Debugger の編集」の小節の後ろ（`---` の前）に足す。

```markdown
### チェンジセット

P16（§3.9、§3.10）は次をすべて満たす。上の項目は変えない。

- [ ] Browser で `Counter` を定義して `increment` を Accept すると、Tools → Change Sorter の `Unnamed1` に `Counter +` と `increment +` が出て、ソース枠に Accept したソースが出る
- [ ] New… で作ったセットが現在のセットになり、以降の記録はそこに入る。既にあるメソッドの Accept は `~`、削除は `−` で、そのソース枠は `removed`
- [ ] Remove from Change Set は項目を忘れ、メソッドもクラスも変えない。Move to は項目を種類ごと別のセットに移す
- [ ] File Out… で書いた .st を、shutdown して boot し直したセッションで File In… すると、同じ定義とソースが戻って現在のセットに記録され、削除のコメントは読み飛ばされる
- [ ] 既にあるクラスを含む .st を File In… しても、そのクラスのほかのメソッドは消えない
- [ ] File In… の結果が Transcript に出る（`filed in: <path>`、または `file-in failed: <path>` と最初のエラー）
- [ ] boot と Open Image… で空の `Unnamed1` だけに戻る。Save Image… はチェンジセットを書かない（`.aoimage` の形式は変えない）
- [ ] vendor の file-in と `ao filein` は変えない（ソースも記録も無い）
- [ ] 評価中の書き込みは `runtime is busy` で拒まれる
- [ ] Kernel 走査緑
- [ ] `PHASE` は `P16`、SPEC §6 の本小節がすべて `[x]`、CHANGELOG に項目
```

- [ ] **Step 13: `docs/phases/P16.md` を作る**

```markdown
# P16 — チェンジセットと Change Sorter

## 結論

Accept と削除をセッションのチェンジセットに記録し、Change Sorter で見る・作る・現在のセットを選ぶ・項目を忘れる/移す・File Out する。File In… は .st を読み込み、メソッドにソースを持たせて現在のセットに記録する。正本は SPEC §3.9「Change Sorter（P16）」、§3.10「チェンジセット（P16）」、§6「チェンジセット」。実装計画は [`superpowers/plans/2026-10-11-p16-change-sets.md`](../superpowers/plans/2026-10-11-p16-change-sets.md)。次のマイナー版（1.5.0）に含める（SPEC §2.4）。

## 前提

P15 緑（1.4.0 リリース済み）。SPEC の P16 追記は実装より先。

## 範囲

**やる:** セッションのチェンジセット（複数、現在のセットは 1 つ）、Accept・クラス定義の Accept・Debugger の Accept・削除の記録、`ao_changes_*`、`ao_filein_file`（ソースを残し、Accept の規則でクラスを入れ、記録する）、File Out の形式（削除はコメントのチャンク）、Change Sorter の窓、File → File In… / File Out Change Set…、Tools → Change Sorter。

**やらない:** セットの改名と削除、メソッドのスタンプと作者、Smalltalk 側の `ChangeSet` クラス、イメージへの保存、vendor と `ao filein` の変更、Workspace の評価の記録、Monticello 相当。

## TDD

`changes_abi_test` の `ChangesAbi.*` と `ChangeKinds.*`、`chunk_filein_test` の `FiledOutCommentChunksAreSkipped`、`ChangeSorterTests`、`AcceptTests.testFileMenuImageCommandsAndShowHierarchy`。

## 受け入れ

- [ ] Accept の記録と窓の表示（`AcceptRecordsAddedThenChangedInANewSet`、`testAcceptedMethodShowsItsKindAndSource`、`testSelectingAClassAndMethodShowsMarksAndSource`）
- [ ] New… と Make Current、種類の規則（`NewSetBecomesCurrent`、`RemoveMethodFollowsTheKindRules`、`RemoveClassFollowsTheKindRules`、`testNewButtonAsksForTheNextFreeNameAndSelectsTheNewCurrentSet`、`testRemovedMethodAndRemovedClassShowRemoved`）
- [ ] Remove from Change Set と Move to（`DropForgetsTheEntryAndKeepsTheMethod`、`MoveMethodEntryKeepsItsKind`、`MoveClassEntryMergesIntoTheTarget`、`testEntryMenuOffersRemoveAndMoveToTheOtherSets`）
- [ ] File Out と File In の往復（`FileOutWritesHeaderDefinitionsMethodsAndRemovals`、`FiledOutSetFilesBackInAfterBoot`、`FiledOutCommentChunksAreSkipped`）
- [ ] 既存クラスへの File In…（`FileInKeepsTheMethodsOfAnExistingClass`）
- [ ] File In… の Transcript の行（`testFileInWritesTheFiledInLineAndRecords`、`testFailedFileInWritesTheFailedLineAndTheFirstError`）
- [ ] boot とロードで始め直す（`BootAndImageLoadStartOver`、`testImageLoadStartsTheWindowOverWithUnnamed1`、`testOpenImageStartsAnOpenChangeSorterOver`）
- [ ] vendor と `ao filein` は変えない（`LoadOrderFileInRecordsNothing`、CLI の既存テスト）
- [ ] busy の拒否（`NewAndMakeCurrentAreRefusedWhileBusy`、`DropAndMoveAreRefusedWhileBusy`、`FileOutIsRefusedWhileBusy`、`FileInIsRefusedWhileBusy`）
- [ ] Kernel 走査緑（`KernelScan.*`）
- [ ] `PHASE` は `P16`、SPEC §6 の本小節がすべて `[x]`、CHANGELOG に項目

### 手動確認

- [ ] 1. Browser で `Counter` を定義し `increment` を Accept → Tools → Change Sorter に `Counter +` / `increment +`
- [ ] 2. New… → `Unnamed2` が `• Unnamed2` になり、サブタイトルが `Unnamed2 is current`
- [ ] 3. Browser で `increment` を直して Accept し、Remove Method… で別のメソッドを消す → Change Sorter をクリックすると `increment ~` と `−` の行が出る
- [ ] 4. Methods の行を右クリック → Move to ▸ `Unnamed1` → 行が移る
- [ ] 5. File Out… で書いた .st を、アプリを再起動して File → File In… → Transcript に `filed in:`、Change Sorter に記録
- [ ] 6. VoiceOver が 3 つの一覧を `Change sets`、`Classes`、`Methods` と読む
```

- [ ] **Step 14: コミット**

```bash
git add SPEC.md docs/phases/P16.md docs/superpowers/plans/2026-10-11-p16-change-sets.md
git commit -m "Specify change sets and the Change Sorter (P16)" -m "Add P16 to SPEC before the code: the session change sets, what is recorded and
the kind rules, the Change Sorter window, File In... and File Out Change Set...,
the File Out format, and the ao_changes_* / ao_filein_file ABI.

Graphify: path removeMethodOf ao_remove_method
Serena: find_referencing_symbols ao::(anonymous)::acceptInto

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: チェンジセットの入れ物、読み、New と Make Current

**Files:**
- Modify: `bridge/ao_abi.h`（busy の注記、P16 の宣言をまとめて足す）
- Create: `runtime/src/ChangeSet.hpp`、`runtime/src/ChangeSet.cpp`
- Modify: `runtime/src/Session.hpp`（`#include "ChangeSet.hpp"`、`Session::changes`、`writeBuf` の宣言）
- Modify: `runtime/src/Session.cpp`（`writeBuf` を無名名前空間の外へ）
- Modify: `runtime/src/abi.cpp`（`changesWrite`、読み 7 つ、`ao_changes_new`、`ao_changes_set_current`）
- Modify: `runtime/CMakeLists.txt`
- Create: `runtime/tests/changes_abi_test.cpp`

**Interfaces:**
- Produces（C ABI）: Task 1 Step 8 の宣言すべて。このタスクで実装するのは読み 7 つと `ao_changes_new`、`ao_changes_set_current`。`ao_changes_drop` / `ao_changes_move` は Task 4、`ao_changes_file_out` は Task 5、`ao_filein_file` は Task 6。
- Produces（C++）: `ao::ChangeKind`、`ao::ClassChange`、`ao::ChangeSet`、`ao::ChangeSets`、`Session::changes`、`int ao::writeBuf(const std::string&, char*, int)`、`sessionChanges{Count,At,Current,New,SetCurrent,ClassCount,ClassAt,MethodCount,MethodAt}`。
- Produces（テストの補助。`changes_abi_test.cpp` の無名名前空間）: `printIt`、`sets()`、`classesOf(set)`、`methodsOf(set, cls, meta)`、`newSet(name)`、`whileBusy(body)`、`tempPath(name)`。

- [ ] **Step 1: Serena で触る所を確かめる**

`find_symbol` で `ao/Session`（depth 1）、`writeBuf`（`runtime/src/Session.cpp`）、`find_referencing_symbols` で `writeBuf` を取る。参照は `Session.cpp` の中だけのはず。

- [ ] **Step 2: 失敗するテストを書く**

`runtime/tests/changes_abi_test.cpp` を作る。

```cpp
#include "ao_abi.h"

#include "../src/ChangeSet.hpp"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <gtest/gtest.h>

namespace {

class ChangesAbi : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  // SPEC §3.10: the hook and the debug mode outlive the session; a test's must not reach the next.
  void TearDown() override {
    ao_runtime_shutdown();
    ao_set_transcript_hook(nullptr, nullptr);
    ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  }
};

// Print it through the ABI: the printString, or "<rc: message>" when it fails.
std::string printIt(const char* source) {
  char out[512];
  AoSpan err{};
  const int rc = ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_PRINTIT, out,
                         static_cast<int>(sizeof(out)), &err);
  if (rc != AO_OK) {
    return "<" + std::to_string(rc) + ": " + err.message + ">";
  }
  return out;
}

struct SetRow {
  std::int64_t id = 0;
  std::string name;
};

// The sets in order, as ao_changes_count and ao_changes_at answer them.
std::vector<SetRow> sets() {
  std::vector<SetRow> out;
  const int n = ao_changes_count();
  for (int i = 0; i < n; ++i) {
    SetRow row;
    char name[256];
    EXPECT_EQ(AO_OK, ao_changes_at(i, &row.id, name, static_cast<int>(sizeof name)));
    row.name = name;
    out.push_back(row);
  }
  return out;
}

// The kind's mark; ASCII "-" for Removed.
std::string mark(int kind) {
  switch (kind) {
    case AO_CHANGE_ADDED:
      return "+";
    case AO_CHANGE_CHANGED:
      return "~";
    case AO_CHANGE_REMOVED:
      return "-";
    default:
      return "";
  }
}

// "<name><mark>" for each class of the set, in order.
std::vector<std::string> classesOf(std::int64_t set) {
  std::vector<std::string> out;
  const int n = ao_changes_class_count(set);
  for (int i = 0; i < n; ++i) {
    char name[256];
    int kind = -9;
    EXPECT_EQ(AO_OK, ao_changes_class_at(set, i, name, static_cast<int>(sizeof name), &kind));
    out.push_back(std::string(name) + mark(kind));
  }
  return out;
}

// "<selector><mark>" for one side of cls in the set, in byte order.
std::vector<std::string> methodsOf(std::int64_t set, const char* cls, int meta) {
  std::vector<std::string> out;
  const int n = ao_changes_method_count(set, cls, meta);
  for (int i = 0; i < n; ++i) {
    char selector[256];
    int kind = -9;
    EXPECT_EQ(AO_OK, ao_changes_method_at(set, cls, meta, i, selector,
                                          static_cast<int>(sizeof selector), &kind));
    out.push_back(std::string(selector) + mark(kind));
  }
  return out;
}

std::int64_t newSet(const char* name) {
  std::int64_t id = 0;
  AoSpan err{};
  EXPECT_EQ(AO_OK, ao_changes_new(name, &id, &err)) << err.message;
  return id;
}

// Runs body inside a transcript hook, where the runtime is busy (SPEC §3.10 再入).
template <typename Body>
void whileBusy(Body body) {
  struct Holder {
    Body* body;
  } holder{&body};
  ao_set_transcript_hook(
      [](const char*, int, int, void* user) { (*static_cast<Holder*>(user)->body)(); }, &holder);
  EXPECT_EQ("7", printIt("Transcript show: 'x'. 7"));
  ao_set_transcript_hook(nullptr, nullptr);
}

// A path in the temporary directory, unique to this process and name.
std::string tempPath(const std::string& name) {
  return (std::filesystem::temp_directory_path() /
          ("ao-p16-" + std::to_string(getpid()) + "-" + name))
      .string();
}

}  // namespace

// SPEC §3.9 Change Sorter, §3.10: a new session holds one empty set, Unnamed1, which is current.
TEST_F(ChangesAbi, NewSessionHasOneEmptyCurrentSet) {
  const std::vector<SetRow> all = sets();
  ASSERT_EQ(1u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_GT(all[0].id, 0);
  EXPECT_EQ(all[0].id, ao_changes_current());
  EXPECT_EQ(0, ao_changes_class_count(all[0].id));
  EXPECT_EQ(0, ao_changes_method_count(all[0].id, "Object", 0));
}

// SPEC §3.10: with no session the counts answer -1, the current set 0, and the writes AO_ERR.
TEST_F(ChangesAbi, NoSessionAnswersFailure) {
  const std::int64_t id = ao_changes_current();
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(-1, ao_changes_count());
  EXPECT_EQ(0, ao_changes_current());
  EXPECT_EQ(-1, ao_changes_class_count(id));
  std::int64_t made = 7;
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_new("Later", &made, &err));
  EXPECT_EQ(0, made);
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_set_current(id));
}

// SPEC §3.9 Change Sorter: New makes an empty set that is current at once; the others stay.
TEST_F(ChangesAbi, NewSetBecomesCurrent) {
  const std::int64_t first = ao_changes_current();
  const std::int64_t second = newSet("Fixes");
  EXPECT_NE(first, second);
  EXPECT_EQ(second, ao_changes_current());
  const std::vector<SetRow> all = sets();
  ASSERT_EQ(2u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_EQ("Fixes", all[1].name);
  EXPECT_EQ(second, all[1].id);
  EXPECT_EQ(AO_OK, ao_changes_set_current(first));
  EXPECT_EQ(first, ao_changes_current());
}

// SPEC §3.10: an empty or taken name is refused with its reason, and nothing changes.
TEST_F(ChangesAbi, NewSetRefusesEmptyAndTakenNames) {
  std::int64_t id = 7;
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_new("", &id, &err));
  EXPECT_STREQ("change set name empty", err.message);
  EXPECT_EQ(0, id);
  EXPECT_EQ(AO_ERR, ao_changes_new(nullptr, &id, &err));
  EXPECT_STREQ("change set name empty", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_new("Unnamed1", &id, &err));
  EXPECT_STREQ("change set name taken: Unnamed1", err.message);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  EXPECT_EQ(1, ao_changes_count());
}

// SPEC §3.10: an unknown set ID is refused by Make Current and by every read; so are a NULL class
// name and a meta other than 0 or 1.
TEST_F(ChangesAbi, UnknownSetIdIsRefused) {
  const std::int64_t current = ao_changes_current();
  EXPECT_EQ(AO_ERR, ao_changes_set_current(current + 1000));
  EXPECT_EQ(AO_ERR, ao_changes_set_current(0));
  EXPECT_EQ(current, ao_changes_current());
  EXPECT_EQ(-1, ao_changes_class_count(current + 1000));
  EXPECT_EQ(-1, ao_changes_method_count(current + 1000, "Object", 0));
  EXPECT_EQ(-1, ao_changes_method_count(current, nullptr, 0));
  EXPECT_EQ(-1, ao_changes_method_count(current, "Object", 2));
  char name[32];
  int kind = 9;
  EXPECT_EQ(AO_ERR, ao_changes_class_at(current + 1000, 0, name, static_cast<int>(sizeof name),
                                        &kind));
  EXPECT_EQ(AO_CHANGE_NONE, kind);
}

// SPEC §3.10 の文字列バッファ: a cut name is AO_ERR_RANGE with the ID; a NULL buffer, a length under
// 1 and an index out of range are AO_ERR with ID 0.
TEST_F(ChangesAbi, ReadsFollowBufferRule) {
  const std::int64_t id = newSet("LongerName");
  std::int64_t got = 0;
  char small[4];
  EXPECT_EQ(AO_ERR_RANGE, ao_changes_at(1, &got, small, static_cast<int>(sizeof small)));
  EXPECT_STREQ("Lon", small);
  EXPECT_EQ(id, got);
  EXPECT_EQ(AO_ERR, ao_changes_at(1, &got, nullptr, 8));
  EXPECT_EQ(0, got);
  EXPECT_EQ(AO_ERR, ao_changes_at(1, &got, small, 0));
  EXPECT_EQ(AO_ERR, ao_changes_at(2, &got, small, static_cast<int>(sizeof small)));
  EXPECT_EQ(AO_ERR, ao_changes_at(-1, &got, small, static_cast<int>(sizeof small)));
  char name[64];
  EXPECT_EQ(AO_OK, ao_changes_at(0, nullptr, name, static_cast<int>(sizeof name)));
  EXPECT_STREQ("Unnamed1", name);
}

// SPEC §3.9 Change Sorter: boot and a successful image load start over with one empty Unnamed1
// under a new ID; a failed load keeps the sets.
TEST_F(ChangesAbi, BootAndImageLoadStartOver) {
  const std::int64_t first = ao_changes_current();
  newSet("Kept");
  const std::string image = tempPath("start-over.aoimage");
  ASSERT_EQ(AO_OK, ao_image_save(image.c_str()));
  AoSpan err{};
  ASSERT_EQ(AO_ERR, ao_image_load(tempPath("missing.aoimage").c_str(), &err));
  EXPECT_EQ(2, ao_changes_count());
  ASSERT_EQ(AO_OK, ao_image_load(image.c_str(), &err)) << err.message;
  std::vector<SetRow> all = sets();
  ASSERT_EQ(1u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_NE(first, all[0].id);
  EXPECT_EQ(all[0].id, ao_changes_current());
  const std::int64_t loaded = all[0].id;
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  all = sets();
  ASSERT_EQ(1u, all.size());
  EXPECT_EQ("Unnamed1", all[0].name);
  EXPECT_NE(loaded, all[0].id);
  std::filesystem::remove(image);
}

// SPEC §3.10 再入: from a hook the writes do nothing and answer AO_ERR ("runtime is busy" with an
// err); the reads still answer.
TEST_F(ChangesAbi, NewAndMakeCurrentAreRefusedWhileBusy) {
  const std::int64_t first = ao_changes_current();
  const std::int64_t second = newSet("Second");
  int newRc = -9;
  std::string newMsg;
  int currentRc = -9;
  int count = -9;
  whileBusy([&] {
    AoSpan err{};
    std::int64_t id = 0;
    newRc = ao_changes_new("Busy", &id, &err);
    newMsg = err.message;
    currentRc = ao_changes_set_current(first);
    count = ao_changes_count();
  });
  EXPECT_EQ(AO_ERR, newRc);
  EXPECT_EQ("runtime is busy", newMsg);
  EXPECT_EQ(AO_ERR, currentRc);
  EXPECT_EQ(2, count);
  EXPECT_EQ(second, ao_changes_current());
}
```

`runtime/CMakeLists.txt` の `add_library(ao_runtime` の `src/Session.cpp` の次の行に `src/ChangeSet.cpp`、`add_executable(ao_runtime_tests` の `tests/remove_abi_test.cpp` の次の行に `tests/changes_abi_test.cpp` を足す。

- [ ] **Step 3: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（`../src/ChangeSet.hpp` が無い、`src/ChangeSet.cpp` が無い）。

- [ ] **Step 4: `bridge/ao_abi.h` を直す**

busy の注記（43〜62 行目のコメント）の頭から `ao_eval_result_length, ao_eval_result_copy,` の行までを次にする（`ao_set_debug_capture, ...` から後ろの行はそのまま）。

```c
/* SPEC §3.10. The runtime is busy while ao_runtime_boot, ao_runtime_shutdown, ao_image_save,
   ao_image_load, ao_filein_load_order, ao_filein_file, ao_workspace_reset, ao_eval,
   ao_accept_method, ao_accept_method_id, ao_accept_class, ao_remove_method, ao_remove_class,
   ao_changes_new, ao_changes_set_current, ao_changes_drop, ao_changes_move, ao_changes_file_out,
   ao_debug_frame_receiver_print, ao_debug_frame_temp_print, ao_debug_inspect,
   ao_debug_clear, ao_debug_proceed, ao_debug_step_into, ao_debug_step_over, ao_debug_step_out,
   ao_debug_restart, ao_debug_accept
   or ao_debug_abort runs, or the interpreter does. Called then (from a transcript or inspect
   hook, or a native), each of these twenty-nine does nothing and answers AO_ERR: ao_image_load,
   ao_filein_file, ao_remove_method, ao_remove_class, ao_changes_new, ao_changes_drop,
   ao_changes_move and ao_changes_file_out
   with the reason "runtime is busy", ao_eval with an empty out. The running evaluation goes on. The hook
   setters (ao_set_transcript_hook, ao_set_inspect_hook, ao_set_runloop_pump_hook),
   ao_request_interrupt, ao_version, the ao_browser_* reads (ao_browser_class_id too),
   the ao_changes_* reads (ao_changes_count, ao_changes_at, ao_changes_current,
   ao_changes_class_count, ao_changes_class_at, ao_changes_method_count, ao_changes_method_at),
   ao_eval_result_length, ao_eval_result_copy,
```

`int ao_remove_class(int64_t class_id, AoSpan* err);` の後ろ（`#ifdef __cplusplus` の前）に足す。

```c
/* SPEC §3.9 Change Sorter, §3.10 チェンジセット (P16). Change sets are session memory: boot and
   ao_image_load start over with one empty set, Unnamed1, which is current; ao_image_save does not
   write them. A set ID is never used again in the process. The reads may be called while busy;
   the writes (ao_changes_new, ao_changes_set_current, ao_changes_drop, ao_changes_move,
   ao_changes_file_out) and ao_filein_file are outermost entries. The *_count functions answer -1
   with no session, an unknown set ID, a NULL class_name or meta other than 0 or 1. The *_at
   functions follow the buffer rule; their outputs (set_id, kind) may be NULL and get 0 on
   AO_ERR. An err gets an empty message on AO_OK and the reason on AO_ERR. */
enum {
  AO_CHANGE_NONE = 0,
  AO_CHANGE_ADDED = 1,
  AO_CHANGE_CHANGED = 2,
  AO_CHANGE_REMOVED = 3
};
/* The sets in the order they were made. */
int ao_changes_count(void);
int ao_changes_at(int index, int64_t* set_id, char* name, int name_len);
/* The current set's ID; 0 with no session. */
int64_t ao_changes_current(void);
/* A new empty set, which becomes current; set_id gets its ID (0 on AO_ERR). AO_ERR: "change set
   name empty" (NULL or ""), "change set name taken: <name>", "runtime is busy", "change set
   failed" (no session). */
int ao_changes_new(const char* name, int64_t* set_id, AoSpan* err);
/* AO_ERR for an unknown ID, with no session, or while busy. */
int ao_changes_set_current(int64_t set_id);
/* A set's classes in the order first recorded. kind: the definition's kind, AO_CHANGE_NONE when
   only its methods changed. */
int ao_changes_class_count(int64_t set_id);
int ao_changes_class_at(int64_t set_id, int index, char* name, int name_len, int* kind);
/* One side's entries of a class in the set, by selector in byte order; the count is 0 when the
   set has no entry for the class. */
int ao_changes_method_count(int64_t set_id, const char* class_name, int meta);
int ao_changes_method_at(int64_t set_id, const char* class_name, int meta, int index,
                         char* selector, int len, int* kind);
/* Forgets an entry; selector NULL forgets the class entry with its methods (meta is not read).
   The methods and classes themselves do not change. AO_ERR: "unknown change set", "not in change
   set: <Class>" or "not in change set: <Class>>><selector>" (<Class> with " class" when meta),
   "runtime is busy", "change set failed" (no session, a NULL class_name, a bad meta). */
int ao_changes_drop(int64_t set_id, const char* class_name, int meta, const char* selector,
                    AoSpan* err);
/* Moves an entry with its kind to to_id; the target's kind for the same entry is replaced, and
   the target gets a class entry when it has none. AO_ERR as ao_changes_drop, and "same change
   set". */
int ao_changes_move(int64_t from_id, int64_t to_id, const char* class_name, int meta,
                    const char* selector, AoSpan* err);
/* Writes the set in the SPEC §3.9 File Out format, replacing path. AO_ERR: "unknown change set",
   "cannot write file: <path>", "runtime is busy", "file out failed" (no session, a NULL path). */
int ao_changes_file_out(int64_t set_id, const char* path, AoSpan* err);
/* SPEC §3.10 File In... (P16): files in one chunk-format file. Each method keeps its source, a
   class definition goes in as ao_accept_class's does, and both are recorded in the current set.
   AO_ERR with the first error in err: its span in the file and its message ("cannot read: <path>"
   with 0-0, "missing class: <name>", a compile error, ...), or "runtime is busy" / "file-in
   failed" with 0-0. The chunks before an error stay applied. */
int ao_filein_file(const char* path, AoSpan* err);
```

- [ ] **Step 5: `runtime/src/ChangeSet.hpp` を作る**

```cpp
#pragma once

#include "ao_abi.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace ao {

// SPEC §3.9 Change Sorter (P16): an entry's kind. The values are the ABI's (SPEC §3.10).
enum class ChangeKind : int {
  Added = AO_CHANGE_ADDED,
  Changed = AO_CHANGE_CHANGED,
  Removed = AO_CHANGE_REMOVED,
};

// SPEC §3.9 Change Sorter: one class's entry. Names, not Oops: a removed class has no class ID,
// and the GC does not trace this.
struct ClassChange {
  // The class's own name.
  std::string name;
  // nullopt: only its methods changed.
  std::optional<ChangeKind> definition;
  // By selector, in byte order.
  std::map<std::string, ChangeKind> instanceSide;
  std::map<std::string, ChangeKind> classSide;
};

struct ChangeSet {
  // Never used again in the process (as a class ID).
  std::int64_t id = 0;
  // Unique in the session.
  std::string name;
  // In the order first recorded.
  std::vector<ClassChange> classes;
};

// SPEC §3.9 Change Sorter, §3.10 チェンジセット: the session's change sets. A new session (boot,
// load) starts with one empty set, Unnamed1, which is current.
struct ChangeSets {
  // Never empty; in the order made.
  std::vector<ChangeSet> sets;
  std::int64_t current = 0;
  ChangeSets();
};

// SPEC §3.10 チェンジセット: the ao_changes_* bodies with the ABI's answers. A write answers AO_OK,
// or AO_ERR with the reason in *reason (left empty when the ABI's message is the generic one).
int sessionChangesCount();
int sessionChangesAt(int index, std::int64_t* setId, char* name, int nameLen);
std::int64_t sessionChangesCurrent();
int sessionChangesNew(const char* name, std::int64_t* setId, std::string* reason);
int sessionChangesSetCurrent(std::int64_t setId);
int sessionChangesClassCount(std::int64_t setId);
int sessionChangesClassAt(std::int64_t setId, int index, char* name, int nameLen, int* kind);
int sessionChangesMethodCount(std::int64_t setId, const char* className, int meta);
int sessionChangesMethodAt(std::int64_t setId, const char* className, int meta, int index,
                           char* selector, int len, int* kind);

}  // namespace ao
```

- [ ] **Step 6: `Session.hpp` と `Session.cpp`**

`Session.hpp` の `#include "ao/Scheduler.hpp"` の次に `#include "ChangeSet.hpp"` を足す。`Session` の `classIds` の宣言（`std::vector<std::unique_ptr<ClassId>> classIds;`）の後ろに足す（`insert_after_symbol` は構造体メンバに効かないことがあるので `replace_content` でよい）。

```cpp
  // SPEC §3.9 Change Sorter, §3.10 チェンジセット (P16): the session's change sets. Plain C++ data
  // (names and kinds, no Oops): the GC does not trace it and the image does not hold it.
  ChangeSets changes;
```

`Session.hpp` の `void ensureKernelNatives(Session& s);` の後ろに足す。

```cpp
// SPEC §3.10 の文字列バッファ: text into buf with a NUL after it; AO_ERR_RANGE when cut (buf still
// ends in NUL); AO_ERR for a NULL buf or len < 1.
int writeBuf(const std::string& text, char* buf, int len);
```

`Session.cpp` の無名名前空間（`struct ClassRow` の前の `namespace {`、587 行目付近）から `writeBuf` の定義を取り除き、同じ本文を `ensureKernelNatives` の定義の直後（その `namespace {` の前）に置く。

```cpp
int writeBuf(const std::string& text, char* buf, int len) {
  if (buf == nullptr || len <= 0) {
    return AO_ERR;
  }
  const auto room = static_cast<std::size_t>(len - 1);
  const bool fits = text.size() <= room;
  const std::size_t n = fits ? text.size() : room;
  if (n != 0) {
    std::memcpy(buf, text.data(), n);
  }
  buf[n] = '\0';
  return fits ? AO_OK : AO_ERR_RANGE;
}
```

- [ ] **Step 7: `runtime/src/ChangeSet.cpp` を作る**

```cpp
#include "ChangeSet.hpp"

#include "Session.hpp"

#include <iterator>
#include <string>
#include <string_view>

namespace ao {
namespace {

// SPEC §3.10 チェンジセット: never used again in this OS process, not even by a later session (as
// nextClassId in Session.cpp).
std::int64_t nextChangeSetId() {
  static std::int64_t next = 1;
  return next++;
}

ChangeSet* findSet(Session& s, std::int64_t id) {
  for (ChangeSet& set : s.changes.sets) {
    if (set.id == id) {
      return &set;
    }
  }
  return nullptr;
}

ClassChange* findClass(ChangeSet& set, std::string_view name) {
  for (ClassChange& entry : set.classes) {
    if (entry.name == name) {
      return &entry;
    }
  }
  return nullptr;
}

std::map<std::string, ChangeKind>& sideOf(ClassChange& entry, bool meta) {
  return meta ? entry.classSide : entry.instanceSide;
}

// False for what the method reads refuse (no session, an unknown set, a NULL name, meta other
// than 0 or 1). True with *side null when the set has no entry for the class.
bool methodSide(std::int64_t setId, const char* className, int meta,
                const std::map<std::string, ChangeKind>** side) {
  *side = nullptr;
  Session* s = session();
  if (s == nullptr || className == nullptr || (meta != 0 && meta != 1)) {
    return false;
  }
  ChangeSet* set = findSet(*s, setId);
  if (set == nullptr) {
    return false;
  }
  if (ClassChange* entry = findClass(*set, className)) {
    *side = &sideOf(*entry, meta == 1);
  }
  return true;
}

}  // namespace

ChangeSets::ChangeSets() {
  sets.push_back(ChangeSet{nextChangeSetId(), "Unnamed1", {}});
  current = sets.front().id;
}

int sessionChangesCount() {
  Session* s = session();
  return s == nullptr ? -1 : static_cast<int>(s->changes.sets.size());
}

int sessionChangesAt(int index, std::int64_t* setId, char* name, int nameLen) {
  Session* s = session();
  if (s == nullptr || index < 0 || static_cast<std::size_t>(index) >= s->changes.sets.size() ||
      name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  const ChangeSet& set = s->changes.sets[static_cast<std::size_t>(index)];
  if (setId != nullptr) {
    *setId = set.id;
  }
  return writeBuf(set.name, name, nameLen);
}

std::int64_t sessionChangesCurrent() {
  Session* s = session();
  return s == nullptr ? 0 : s->changes.current;
}

int sessionChangesNew(const char* name, std::int64_t* setId, std::string* reason) {
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  if (name == nullptr || *name == '\0') {
    *reason = "change set name empty";
    return AO_ERR;
  }
  for (const ChangeSet& set : s->changes.sets) {
    if (set.name == name) {
      *reason = "change set name taken: " + set.name;
      return AO_ERR;
    }
  }
  s->changes.sets.push_back(ChangeSet{nextChangeSetId(), name, {}});
  // SPEC §3.9 Change Sorter: a new set is current at once.
  s->changes.current = s->changes.sets.back().id;
  if (setId != nullptr) {
    *setId = s->changes.current;
  }
  return AO_OK;
}

int sessionChangesSetCurrent(std::int64_t setId) {
  Session* s = session();
  if (s == nullptr || findSet(*s, setId) == nullptr) {
    return AO_ERR;
  }
  s->changes.current = setId;
  return AO_OK;
}

int sessionChangesClassCount(std::int64_t setId) {
  Session* s = session();
  ChangeSet* set = s == nullptr ? nullptr : findSet(*s, setId);
  return set == nullptr ? -1 : static_cast<int>(set->classes.size());
}

int sessionChangesClassAt(std::int64_t setId, int index, char* name, int nameLen, int* kind) {
  Session* s = session();
  ChangeSet* set = s == nullptr ? nullptr : findSet(*s, setId);
  if (set == nullptr || index < 0 || static_cast<std::size_t>(index) >= set->classes.size() ||
      name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  const ClassChange& entry = set->classes[static_cast<std::size_t>(index)];
  if (kind != nullptr) {
    *kind = entry.definition ? static_cast<int>(*entry.definition) : AO_CHANGE_NONE;
  }
  return writeBuf(entry.name, name, nameLen);
}

int sessionChangesMethodCount(std::int64_t setId, const char* className, int meta) {
  const std::map<std::string, ChangeKind>* side = nullptr;
  if (!methodSide(setId, className, meta, &side)) {
    return -1;
  }
  return side == nullptr ? 0 : static_cast<int>(side->size());
}

int sessionChangesMethodAt(std::int64_t setId, const char* className, int meta, int index,
                           char* selector, int len, int* kind) {
  const std::map<std::string, ChangeKind>* side = nullptr;
  if (!methodSide(setId, className, meta, &side) || side == nullptr || index < 0 ||
      static_cast<std::size_t>(index) >= side->size() || selector == nullptr || len < 1) {
    return AO_ERR;
  }
  const auto it = std::next(side->begin(), index);
  if (kind != nullptr) {
    *kind = static_cast<int>(it->second);
  }
  return writeBuf(it->first, selector, len);
}

}  // namespace ao
```

- [ ] **Step 8: `runtime/src/abi.cpp` の入口**

先頭の include に `#include "ChangeSet.hpp"` を足す（`#include "Session.hpp"` の前）。ファイルの末尾に足す。

```cpp
namespace {

// SPEC §3.10 チェンジセット: a write. Outermost: while busy it answers AO_ERR with "runtime is
// busy". An AO_ERR's message is the reason, or `failed` when there is none (no session, a bad
// argument, an exception).
template <typename Body>
int changesWrite(AoSpan* err, std::string_view failed, Body&& body) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  std::string reason;
  const int rc = guarded(-1, [&] { return body(&reason); });
  if (rc == AO_OK) {
    return AO_OK;
  }
  setMessage(err, rc == -1 || reason.empty() ? failed : std::string_view(reason));
  return AO_ERR;
}

}  // namespace

// SPEC §3.10 チェンジセット: the reads take no AbiEntry and answer while busy, like the
// ao_browser_* reads.
extern "C" int ao_changes_count(void) {
  return guarded(-1, [] { return ao::sessionChangesCount(); });
}

extern "C" int ao_changes_at(int index, int64_t* set_id, char* name, int name_len) {
  if (set_id != nullptr) {
    *set_id = 0;
  }
  return guarded(AO_ERR, [&] { return ao::sessionChangesAt(index, set_id, name, name_len); });
}

// Reads one field; it cannot throw.
extern "C" int64_t ao_changes_current(void) { return ao::sessionChangesCurrent(); }

extern "C" int ao_changes_class_count(int64_t set_id) {
  return guarded(-1, [&] { return ao::sessionChangesClassCount(set_id); });
}

extern "C" int ao_changes_class_at(int64_t set_id, int index, char* name, int name_len,
                                   int* kind) {
  if (kind != nullptr) {
    *kind = AO_CHANGE_NONE;
  }
  return guarded(AO_ERR, [&] {
    return ao::sessionChangesClassAt(set_id, index, name, name_len, kind);
  });
}

extern "C" int ao_changes_method_count(int64_t set_id, const char* class_name, int meta) {
  return guarded(-1, [&] { return ao::sessionChangesMethodCount(set_id, class_name, meta); });
}

extern "C" int ao_changes_method_at(int64_t set_id, const char* class_name, int meta, int index,
                                    char* selector, int len, int* kind) {
  if (kind != nullptr) {
    *kind = AO_CHANGE_NONE;
  }
  return guarded(AO_ERR, [&] {
    return ao::sessionChangesMethodAt(set_id, class_name, meta, index, selector, len, kind);
  });
}

extern "C" int ao_changes_new(const char* name, int64_t* set_id, AoSpan* err) {
  if (set_id != nullptr) {
    *set_id = 0;
  }
  return changesWrite(err, "change set failed", [&](std::string* reason) {
    return ao::sessionChangesNew(name, set_id, reason);
  });
}

extern "C" int ao_changes_set_current(int64_t set_id) {
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [&] { return ao::sessionChangesSetCurrent(set_id); });
}
```

- [ ] **Step 9: テストが通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'ChangesAbi|SessionAbi|BrowserAbi|KernelScan' --output-on-failure`
Expected: PASS（`ChangesAbi` の 8 件と既存）。

- [ ] **Step 10: 新規ファイルの外形を取る**

Serena の clangd を再起動し（子プロセスを kill）、`get_symbols_overview` を `runtime/src/ChangeSet.cpp` と `runtime/tests/changes_abi_test.cpp` に取る。

- [ ] **Step 11: コミット**

```bash
git add bridge/ao_abi.h runtime/src/ChangeSet.hpp runtime/src/ChangeSet.cpp runtime/src/Session.hpp runtime/src/Session.cpp runtime/src/abi.cpp runtime/CMakeLists.txt runtime/tests/changes_abi_test.cpp
git commit -m "Add the change set store, its reads, New and Make Current (P16)" -m "Each session holds change sets as plain C++ data and starts with an empty
Unnamed1. The reads answer while busy; ao_changes_new and
ao_changes_set_current are outermost entries. The ABI header declares the
whole P16 surface; the other writes follow.

Graphify: query Session classIds writeBuf
Serena: find_referencing_symbols ao::writeBuf

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: Accept と削除を記録する

**Files:**
- Modify: `runtime/src/ChangeSet.hpp` / `ChangeSet.cpp`（種類の規則と `note*`）
- Modify: `runtime/src/Compile.cpp`（`ChunkMode`、`applyChunkActions`、`applyMethodsFor`、`acceptInto`、`acceptClassSource`、`removeMethodOf`、`removeClassOf`）
- Test: `runtime/tests/changes_abi_test.cpp`

**Interfaces:**
- Consumes: Task 2 の `Session::changes`、`findSet`、`findClass`、`sideOf`（`ChangeSet.cpp` の無名名前空間）。
- Produces: `std::optional<ao::ChangeKind> ao::kindAfterAccept(std::optional<ChangeKind> was, bool replaced)`、`ao::kindAfterRemoval(std::optional<ChangeKind> was)`、`void ao::noteMethodChange(CallContext&, const std::string& className, bool meta, std::string_view selector, bool replaced) noexcept`、`ao::noteMethodRemoval(CallContext&, const std::string&, bool, std::string_view) noexcept`、`ao::noteClassChange(CallContext&, const std::string&, bool existed) noexcept`、`ao::noteClassRemoval(CallContext&, const std::string&) noexcept`。`Compile.cpp` の無名名前空間に `enum class ChunkMode { FileIn, Accept, Recorded }`（`Recorded` は Task 6 で使う）と `applyChunkActions(ctx, actions, errors, ChunkMode mode = ChunkMode::FileIn)`、`applyMethodsFor(ctx, action, errors, ChunkMode mode)`。
- テストの補助を足す: `defineClass(name, super, ivars = "", category = "P16-Test")`、`accept(cls, meta, source)`、`removeMethod(cls, meta, sel)`、`removeClass(cls)`、`frameLabeled(label)`。

- [ ] **Step 1: Serena で参照を取る**

`find_symbol` で `acceptInto`、`applyChunkActions`、`applyMethodsFor`、`removeMethodOf`、`removeClassOf`（`include_body`）、`find_referencing_symbols` で `applyChunkActions` と `applyMethodsFor` を取る。`applyChunkActions` の呼び出し元は `acceptClassSource`、`applyChunks`、`fileInListedFile`、`applyMethodsFor` の呼び出し元は `applyChunkActions` だけのはず。

- [ ] **Step 2: 失敗するテストを書く**

`changes_abi_test.cpp` の無名名前空間の末尾（`tempPath` の後ろ）に補助を足す。

```cpp
int defineClass(const char* name, const char* super, const char* ivars = "",
                const char* category = "P16-Test") {
  const std::string def = std::string(super) + " subclass: #" + name +
                          "\n  instanceVariableNames: '" + ivars +
                          "'\n  classVariableNames: ''\n  poolDictionaries: ''\n  category: '" +
                          category + "'";
  AoSpan err{};
  const int rc = ao_accept_class(def.c_str(), &err);
  EXPECT_EQ(AO_OK, rc) << err.message;
  return rc;
}

int accept(const char* cls, int meta, const char* source) {
  AoSpan err{};
  const int rc = ao_accept_method(cls, meta, source, &err);
  EXPECT_EQ(AO_OK, rc) << err.message;
  return rc;
}

// The removal's message; empty on AO_OK.
std::string removeMethod(const char* cls, int meta, const char* sel) {
  AoSpan err{};
  return ao_remove_method(ao_browser_class_id(cls), meta, sel, &err) == AO_OK ? "" : err.message;
}

std::string removeClass(const char* cls) {
  AoSpan err{};
  return ao_remove_class(ao_browser_class_id(cls), &err) == AO_OK ? "" : err.message;
}

// The index of the selected live frame labeled label, or -1.
int frameLabeled(const char* label) {
  const int n = ao_debug_frame_count();
  for (int i = 0; i < n; ++i) {
    char buf[256];
    if (ao_debug_frame_label(i, buf, static_cast<int>(sizeof buf)) == AO_OK &&
        std::strcmp(buf, label) == 0) {
      return i;
    }
  }
  return -1;
}
```

ファイルの末尾にテストを足す。

```cpp
// SPEC §3.9 種類の規則.
TEST(ChangeKinds, AcceptAndRemovalRules) {
  using ao::ChangeKind;
  using ao::kindAfterAccept;
  using ao::kindAfterRemoval;
  EXPECT_EQ(ChangeKind::Added, kindAfterAccept(std::nullopt, false));
  EXPECT_EQ(ChangeKind::Changed, kindAfterAccept(std::nullopt, true));
  EXPECT_EQ(ChangeKind::Added, kindAfterAccept(ChangeKind::Added, true));
  EXPECT_EQ(ChangeKind::Changed, kindAfterAccept(ChangeKind::Changed, true));
  EXPECT_EQ(ChangeKind::Changed, kindAfterAccept(ChangeKind::Removed, false));
  EXPECT_EQ(ChangeKind::Removed, kindAfterRemoval(std::nullopt));
  EXPECT_EQ(std::nullopt, kindAfterRemoval(ChangeKind::Added));
  EXPECT_EQ(ChangeKind::Removed, kindAfterRemoval(ChangeKind::Changed));
  EXPECT_EQ(ChangeKind::Removed, kindAfterRemoval(ChangeKind::Removed));
}

// SPEC §3.9 記録: a new method is Added and stays Added when accepted again; in a new set the same
// Accept replaces a method, so it is Changed there. The definition Accept records the class.
TEST_F(ChangesAbi, AcceptRecordsAddedThenChangedInANewSet) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Counter", "Object", "count");
  accept("P16Counter", 0, "increment\n  count := (count ifNil: [0]) + 1.\n  ^count");
  accept("P16Counter", 0, "increment\n  ^0");
  EXPECT_EQ(std::vector<std::string>{"P16Counter+"}, classesOf(first));
  EXPECT_EQ(std::vector<std::string>{"increment+"}, methodsOf(first, "P16Counter", 0));
  const std::int64_t second = newSet("Second");
  accept("P16Counter", 0, "increment\n  ^1");
  EXPECT_EQ(std::vector<std::string>{"P16Counter"}, classesOf(second));
  EXPECT_EQ(std::vector<std::string>{"increment~"}, methodsOf(second, "P16Counter", 0));
  EXPECT_EQ(std::vector<std::string>{"increment+"}, methodsOf(first, "P16Counter", 0));
}

// SPEC §3.9 記録: a class-side Accept goes to the class side of the class's entry.
TEST_F(ChangesAbi, ClassSideAcceptIsRecordedOnTheClassSide) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Side", "Object");
  accept("P16Side", 1, "make\n  ^self new");
  EXPECT_EQ(std::vector<std::string>{}, methodsOf(set, "P16Side", 0));
  EXPECT_EQ(std::vector<std::string>{"make+"}, methodsOf(set, "P16Side", 1));
}

// SPEC §3.9 記録: a definition of a name that names a class is Changed; a reshape records only the
// definition, not the methods it carried.
TEST_F(ChangesAbi, ClassDefinitionAcceptIsChangedForAnExistingClass) {
  defineClass("P16Shape", "Object");
  accept("P16Shape", 0, "one\n  ^1");
  const std::int64_t set = newSet("Reshape");
  defineClass("P16Shape", "Object", "a b");
  EXPECT_EQ(std::vector<std::string>{"P16Shape~"}, classesOf(set));
  EXPECT_EQ(0, ao_changes_method_count(set, "P16Shape", 0));
  EXPECT_EQ("1", printIt("P16Shape new one"));
}

// SPEC §3.9 種類の規則: removing a method added in the set forgets it; removing an older one is
// Removed, and accepting it again is Changed.
TEST_F(ChangesAbi, RemoveMethodFollowsTheKindRules) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Gone", "Object");
  accept("P16Gone", 0, "temp\n  ^1");
  accept("P16Gone", 0, "kept\n  ^2");
  EXPECT_EQ("", removeMethod("P16Gone", 0, "temp"));
  EXPECT_EQ(std::vector<std::string>{"kept+"}, methodsOf(first, "P16Gone", 0));
  const std::int64_t second = newSet("Later");
  EXPECT_EQ("", removeMethod("P16Gone", 0, "kept"));
  EXPECT_EQ(std::vector<std::string>{"kept-"}, methodsOf(second, "P16Gone", 0));
  accept("P16Gone", 0, "kept\n  ^3");
  EXPECT_EQ(std::vector<std::string>{"kept~"}, methodsOf(second, "P16Gone", 0));
}

// SPEC §3.9 種類の規則: a class added in the set goes with its entry; an older class becomes
// Removed and its method entries go.
TEST_F(ChangesAbi, RemoveClassFollowsTheKindRules) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Old", "Object");
  defineClass("P16New", "Object");
  accept("P16New", 0, "x\n  ^1");
  EXPECT_EQ("", removeClass("P16New"));
  EXPECT_EQ(std::vector<std::string>{"P16Old+"}, classesOf(first));
  const std::int64_t second = newSet("Later");
  accept("P16Old", 0, "y\n  ^2");
  EXPECT_EQ("", removeClass("P16Old"));
  EXPECT_EQ(std::vector<std::string>{"P16Old-"}, classesOf(second));
  EXPECT_EQ(0, ao_changes_method_count(second, "P16Old", 0));
}

// SPEC §3.9 記録: methodsFor: chunks through ao_accept_class are recorded; they still keep no
// source (SPEC §3.10 ソースはイメージに書かない).
TEST_F(ChangesAbi, MethodsForChunkAcceptIsRecordedWithoutSource) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Chunk", "Object");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!P16Chunk methodsFor: 'x'!\nviaChunk\n  ^7! !", &err))
      << err.message;
  EXPECT_EQ(std::vector<std::string>{"viaChunk+"}, methodsOf(set, "P16Chunk", 0));
  char buf[256];
  EXPECT_EQ(AO_ERR_NOSOURCE, ao_browser_source(ao_browser_class_id("P16Chunk"), 0, "viaChunk",
                                               buf, static_cast<int>(sizeof buf)));
}

// SPEC §3.9 記録: the Debugger's Accept goes through the same Accept, so it is recorded.
TEST_F(ChangesAbi, DebuggerAcceptIsRecorded) {
  defineClass("P16Live", "Object");
  accept("P16Live", 0, "answer\n  self halt.\n  ^1");
  const std::int64_t set = newSet("Debugger");
  ao_set_debug_mode(AO_DEBUG_LIVE);
  const char* source = "P16Live new answer";
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_ERR_HALT, ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_PRINTIT,
                                 out, static_cast<int>(sizeof out), &err));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  const int frame = frameLabeled("P16Live>>answer");
  ASSERT_GE(frame, 0);
  EXPECT_EQ(AO_ERR_HALT, ao_debug_accept(pid, frame, "answer\n  ^42", out,
                                         static_cast<int>(sizeof out), &err))
      << err.message;
  EXPECT_EQ(std::vector<std::string>{"answer~"}, methodsOf(set, "P16Live", 0));
  EXPECT_EQ(AO_OK, ao_debug_abort(pid));
}

// SPEC §3.9 記録: what is refused records nothing: a compile error, a native overwrite, a native
// removal.
TEST_F(ChangesAbi, RefusedAcceptAndRemovalRecordNothing) {
  const std::int64_t set = ao_changes_current();
  AoSpan err{};
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, "broken\n  ^(", &err));
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, "printString\n  ^'x'", &err));
  EXPECT_EQ(AO_ERR, ao_remove_method(ao_browser_class_id("Object"), 0, "printString", &err));
  EXPECT_EQ(0, ao_changes_class_count(set));
}

// SPEC §3.9 記録: the vendor-style LOAD_ORDER file-in records nothing.
TEST_F(ChangesAbi, LoadOrderFileInRecordsNothing) {
  const std::filesystem::path dir = tempPath("load-order");
  std::filesystem::create_directories(dir);
  std::ofstream(dir / "LOAD_ORDER") << "a.st\n";
  std::ofstream(dir / "a.st") << "Object subclass: #P16Vendor\n"
                                 "  instanceVariableNames: ''\n"
                                 "  classVariableNames: ''\n"
                                 "  poolDictionaries: ''\n"
                                 "  category: 'P16-Vendor'!\n\n"
                                 "!P16Vendor methodsFor: 'x'!\n"
                                 "v\n  ^1! !\n";
  ASSERT_EQ(AO_OK, ao_filein_load_order((dir / "LOAD_ORDER").c_str()));
  EXPECT_EQ("1", printIt("P16Vendor new v"));
  EXPECT_EQ(0, ao_changes_class_count(ao_changes_current()));
  std::filesystem::remove_all(dir);
}
```

- [ ] **Step 3: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（`ao::kindAfterAccept` が無い）。

- [ ] **Step 4: 種類の規則と `note*` を書く**

`ChangeSet.hpp` の include に `#include <string_view>` を足し、`namespace ao {` の直後に `struct CallContext;` を、`sessionChangesCount` の宣言の前に次を足す。

```cpp
// SPEC §3.9 種類の規則. After an Accept: none → Added when nothing was replaced, else Changed;
// Added stays Added; Changed stays Changed; Removed → Changed. Never nullopt.
std::optional<ChangeKind> kindAfterAccept(std::optional<ChangeKind> was, bool replaced);
// After a removal: none → Removed; Added → nullopt (the entry goes); Changed and Removed →
// Removed.
std::optional<ChangeKind> kindAfterRemoval(std::optional<ChangeKind> was);

// SPEC §3.9 記録: each updates the current set of the session whose heap ctx runs on (a fiber of it
// too). Nothing without a session, for another heap (a test's own Boot, the CLI), or for an empty
// class name. className is the class's own name. Never throws: without the memory for it, the
// record is not made and the set is as it was.
void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept;
void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept;
// existed: the name named a class (a Behavior) before the definition.
void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept;
void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept;
```

`ChangeSet.cpp` の include に `#include <algorithm>` を足す。無名名前空間の `methodSide` の後ろに足す。

```cpp
// SPEC §3.9 記録: the current set of the session whose heap ctx runs on; null otherwise.
ChangeSet* recordingSet(CallContext& ctx, const std::string& className) {
  Session* s = session();
  if (s == nullptr || &ctx.heap != &s->heap || className.empty()) {
    return nullptr;
  }
  return findSet(*s, s->changes.current);
}

// A class entry with neither a definition kind nor a method entry goes.
void dropIfEmpty(ChangeSet& set, std::string_view className) {
  std::erase_if(set.classes, [&](const ClassChange& c) {
    return c.name == className && !c.definition && c.instanceSide.empty() && c.classSide.empty();
  });
}

std::optional<ChangeKind> kindOf(const std::map<std::string, ChangeKind>& side,
                                 const std::string& selector) {
  const auto found = side.find(selector);
  return found == side.end() ? std::nullopt : std::optional<ChangeKind>(found->second);
}
```

`ChangeSets::ChangeSets()` の前に足す（各関数は変更を組み立ててから 1 回で入れるので、途中の例外でセットが半端にならない）。

```cpp
std::optional<ChangeKind> kindAfterAccept(std::optional<ChangeKind> was, bool replaced) {
  if (!was) {
    return replaced ? ChangeKind::Changed : ChangeKind::Added;
  }
  return *was == ChangeKind::Added ? ChangeKind::Added : ChangeKind::Changed;
}

std::optional<ChangeKind> kindAfterRemoval(std::optional<ChangeKind> was) {
  if (was == ChangeKind::Added) {
    return std::nullopt;
  }
  return ChangeKind::Removed;
}

void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    std::string key(selector);
    if (ClassChange* entry = findClass(*set, className)) {
      auto& side = sideOf(*entry, meta);
      const ChangeKind kind = *kindAfterAccept(kindOf(side, key), replaced);
      side.insert_or_assign(std::move(key), kind);
      return;
    }
    ClassChange made{className, std::nullopt, {}, {}};
    sideOf(made, meta).emplace(std::move(key), *kindAfterAccept(std::nullopt, replaced));
    set->classes.push_back(std::move(made));
  } catch (...) {
    // SPEC §3.9 記録: without the memory the record is not made; the Accept stands.
  }
}

void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    std::string key(selector);
    ClassChange* entry = findClass(*set, className);
    if (entry == nullptr) {
      ClassChange made{className, std::nullopt, {}, {}};
      sideOf(made, meta).emplace(std::move(key), ChangeKind::Removed);
      set->classes.push_back(std::move(made));
      return;
    }
    auto& side = sideOf(*entry, meta);
    if (const std::optional<ChangeKind> now = kindAfterRemoval(kindOf(side, key))) {
      side.insert_or_assign(std::move(key), *now);
      return;
    }
    // Added in this set: the entry goes, and an emptied class entry with it.
    side.erase(key);
    dropIfEmpty(*set, className);
  } catch (...) {
  }
}

void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    if (ClassChange* entry = findClass(*set, className)) {
      entry->definition = kindAfterAccept(entry->definition, existed);
      return;
    }
    set->classes.push_back(ClassChange{className, kindAfterAccept(std::nullopt, existed), {}, {}});
  } catch (...) {
  }
}

void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    ClassChange* entry = findClass(*set, className);
    if (entry == nullptr) {
      set->classes.push_back(ClassChange{className, ChangeKind::Removed, {}, {}});
      return;
    }
    // SPEC §3.9 種類の規則: a class added in this set goes with its methods; another is Removed and
    // its method entries go.
    if (entry->definition == ChangeKind::Added) {
      std::erase_if(set->classes, [&](const ClassChange& c) { return c.name == className; });
      return;
    }
    entry->definition = ChangeKind::Removed;
    entry->instanceSide.clear();
    entry->classSide.clear();
  } catch (...) {
  }
}
```

- [ ] **Step 5: `Compile.cpp` から呼ぶ**

include に `#include "ChangeSet.hpp"` を足す（`#include "Session.hpp"` の次）。

`applyMethodsFor` の定義の直前（同じ無名名前空間）に足す。

```cpp
// How applyChunkActions applies what it reads. FileIn: the vendor file-in and `ao filein` (a
// definition makes a new class, no sources, no records). Accept: ao_accept_class (a definition keeps
// or reshapes an existing class; records, no sources). Recorded: File In... (P16; as Accept, and
// each method keeps its source).
enum class ChunkMode { FileIn, Accept, Recorded };

// Defined below; applyMethodsFor records under the class's own name.
std::string ownClassName(CallContext& ctx, Oop cls);
```

`applyMethodsFor` を `replace_symbol_body` で次にする。

```cpp
bool applyMethodsFor(CallContext& ctx, const compiler::ChunkAction& action,
                     std::vector<FileInError>& errors, ChunkMode mode) {
  Root cls(ctx.roots, ctx.wk.named(action.className));
  if (!cls.slot.isHeap()) {
    addError(errors, {action.span, "missing class: " + action.className});
    return false;
  }
  if (refusesKernelRedefinition(isKernelClass(ctx.wk, cls.slot), action, errors)) {
    return false;
  }
  const Oop target = action.meta ? ctx.heap.klass(cls.slot) : cls.slot;
  if (!target.isHeap()) {
    addError(errors, {action.span, "missing class: " + action.className});
    return false;
  }
  Root tgt(ctx.roots, target);
  compiler::CompileEnv env;
  fillInstVars(ctx, tgt.slot, env);
  fillClassVars(ctx, tgt.slot, env);
  // SPEC §3.9 記録: under the class's own name (a chunk naming an alias records the class).
  const std::string own = mode != ChunkMode::FileIn && isClassShaped(ctx.heap, cls.slot)
                              ? ownClassName(ctx, cls.slot)
                              : std::string();
  // SPEC §3.12: a method-level error names its method and does not stop the rest.
  for (const auto& m : action.methods) {
    compiler::CompileResult cr = compiler::compileMethod(m.source, env);
    if (!cr.ok) {
      compiler::CompileError e = std::move(cr.error);
      e.span = compiler::fileSpan(m, e.span);
      // The pattern parses even when the body does not, so the partial method has its selector.
      addError(errors, std::move(e),
               methodKey(action, compiler::parseMethod(m.source).method.name));
      continue;
    }
    // The method the selector names before the install (nil: none). Interned first, so the
    // dictionary is read after the last allocation.
    Root old(ctx.roots, Oop::nil());
    {
      const Oop sel = ctx.wk.intern(cr.image.selector);
      const Oop dict = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
      if (dict.isHeap() && sel.isHeap()) {
        old.slot = MethodDictionary::at(ctx.heap, dict, sel);
      }
    }
    if (old.slot.isHeap() && ctx.heap.klass(old.slot) == ctx.wk.nativeMethodClass) {
      addError(errors, {m.span, "native selector overwrite refused: " + cr.image.selector},
               methodKey(action, cr.image.selector));
      continue;
    }
    if (const std::string var = unboundClassVariable(ctx, cr.image, tgt.slot); !var.empty()) {
      addError(errors, {m.span, unboundClassVariableMessage(var)},
               methodKey(action, cr.image.selector));
      continue;
    }
    Root kept(ctx.roots, installMethod(ctx, tgt.slot, cr.image));
    if (!kept.slot.isHeap()) {
      addError(errors, {m.span, "install failed"}, methodKey(action, cr.image.selector));
      continue;
    }
    if (mode != ChunkMode::FileIn) {
      noteMethodChange(ctx, own, action.meta, cr.image.selector, old.slot.isHeap());
    }
  }
  return true;
}
```

`applyChunkActions` を次にする。

```cpp
bool applyChunkActions(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                       std::vector<FileInError>& errors, ChunkMode mode = ChunkMode::FileIn) {
  for (const auto& action : actions) {
    switch (action.kind) {
      case compiler::ChunkKind::ClassDef: {
        // SPEC §3.9 記録: a name that named a class before the definition is Changed.
        const bool existed = mode != ChunkMode::FileIn && namesBehavior(ctx, action.className);
        // SPEC §3.9: ao_accept_class and File In... keep or reshape an existing class; the vendor
        // file-in and `ao filein` do not.
        const bool applied = mode == ChunkMode::FileIn ? applyClassDef(ctx, action, errors)
                                                       : acceptClassDef(ctx, action, errors);
        if (!applied) {
          return false;
        }
        if (mode != ChunkMode::FileIn) {
          noteClassChange(ctx, action.className, existed);
        }
        break;
      }
      case compiler::ChunkKind::MethodsFor:
        if (!applyMethodsFor(ctx, action, errors, mode)) {
          return false;
        }
        break;
      case compiler::ChunkKind::DoIt:
        break;
    }
  }
  return true;
}
```

`acceptClassSource` の `applyChunkActions(ctx, actions, found, /*accept=*/true)` を `applyChunkActions(ctx, actions, found, ChunkMode::Accept)` にする（`replace_content`）。

`acceptInto` の `rememberMethodSource(kept.slot, text.slot, replaced, &cr.image);` の直後に足す（`replace_content`）。

```cpp
  // SPEC §3.9 記録: the method is in its dictionary and has its source.
  noteMethodChange(ctx, name, meta, cr.image.selector, replaced.isHeap());
```

`removeMethodOf` の `forgetMethodSource(method);` の直後に `noteMethodRemoval(ctx, own, meta, selector);` を、`removeClassOf` の `forgetMethodSourcesOf(ctx, ctx.heap.klass(cls));` の直後に `noteClassRemoval(ctx, own);` を足す。どちらも検査と変更のあとなので、拒否は記録しない。

- [ ] **Step 6: テストが通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'ChangesAbi|ChangeKinds|ChunkFileIn|AcceptAbi|RemoveAbi|LiveAccept|VendorFileIn|KernelScan' --output-on-failure`
Expected: PASS。`ChunkFileIn`、`VendorFileIn`、`AcceptAbi`、`RemoveAbi` が緑なら、file-in と Accept と削除の振る舞いは変わっていない。`DebuggerAcceptIsRecorded` が `frame < 0` で落ちたら、`ao_debug_frame_label` の実際のラベルを出して確かめてから補助を直す（`superpowers:systematic-debugging`）。

- [ ] **Step 7: コミット**

```bash
git add runtime/src/ChangeSet.hpp runtime/src/ChangeSet.cpp runtime/src/Compile.cpp runtime/tests/changes_abi_test.cpp
git commit -m "Record accepts and removals in the current change set (P16)" -m "acceptInto, the chunk accept path, removeMethodOf and removeClassOf note each
success in the current set with the kind rules. Records are plain C++ data and
never change the answer of the operation. The vendor file-in records nothing.

Graphify: path acceptInto removeMethodOf
Serena: replace_symbol_body ao::(anonymous)::applyMethodsFor

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: 項目を忘れる・移す（Remove from Change Set / Move to）

**Files:**
- Modify: `runtime/src/ChangeSet.hpp` / `ChangeSet.cpp`
- Modify: `runtime/src/abi.cpp`
- Test: `runtime/tests/changes_abi_test.cpp`

**Interfaces:**
- Consumes: Task 2 の `changesWrite`、Task 3 の `dropIfEmpty`。
- Produces: `int ao::sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector, std::string* reason)`、`int ao::sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta, const char* selector, std::string* reason)`、`ChangeSet.cpp` の無名名前空間に `std::string entryName(std::string_view className, bool meta, std::string_view selector)`（Task 5 も使う）。

- [ ] **Step 1: 失敗するテストを書く**

```cpp
// SPEC §3.9 Change Sorter: Remove from Change Set forgets the entry; the method stays.
TEST_F(ChangesAbi, DropForgetsTheEntryAndKeepsTheMethod) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Drop", "Object");
  accept("P16Drop", 0, "a\n  ^1");
  accept("P16Drop", 0, "b\n  ^2");
  AoSpan err{};
  std::strcpy(err.message, "stale");
  ASSERT_EQ(AO_OK, ao_changes_drop(set, "P16Drop", 0, "a", &err)) << err.message;
  EXPECT_STREQ("", err.message);
  EXPECT_EQ(std::vector<std::string>{"b+"}, methodsOf(set, "P16Drop", 0));
  EXPECT_EQ("1", printIt("P16Drop new a"));
  // selector NULL: the class entry with its methods; meta is not read.
  ASSERT_EQ(AO_OK, ao_changes_drop(set, "P16Drop", 7, nullptr, &err)) << err.message;
  EXPECT_EQ(0, ao_changes_class_count(set));
  EXPECT_EQ("2", printIt("P16Drop new b"));
}

// SPEC §3.10 チェンジセット: a drop of what the set does not hold, or in an unknown set, says why.
TEST_F(ChangesAbi, DropRefusesWhatTheSetDoesNotHold) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Miss", "Object");
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, "P16Miss", 1, "nope", &err));
  EXPECT_STREQ("not in change set: P16Miss class>>nope", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, "Nowhere", 0, nullptr, &err));
  EXPECT_STREQ("not in change set: Nowhere", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set + 1000, "P16Miss", 0, nullptr, &err));
  EXPECT_STREQ("unknown change set", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, nullptr, 0, nullptr, &err));
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(set, "P16Miss", 2, "x", &err));
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(std::vector<std::string>{"P16Miss+"}, classesOf(set));
}

// SPEC §3.9 Change Sorter, §3.10: a method entry moves with its kind and replaces the target's;
// the source's emptied class entry goes.
TEST_F(ChangesAbi, MoveMethodEntryKeepsItsKind) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Move", "Object");
  accept("P16Move", 0, "m\n  ^1");
  const std::int64_t second = newSet("Second");
  accept("P16Move", 0, "m\n  ^2");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_move(second, first, "P16Move", 0, "m", &err)) << err.message;
  EXPECT_EQ(0, ao_changes_class_count(second));
  EXPECT_EQ(std::vector<std::string>{"P16Move+"}, classesOf(first));
  EXPECT_EQ(std::vector<std::string>{"m~"}, methodsOf(first, "P16Move", 0));
}

// SPEC §3.10: a class entry moves with its methods; the target keeps its definition kind when the
// moved entry has none, and gets a class entry when it has none.
TEST_F(ChangesAbi, MoveClassEntryMergesIntoTheTarget) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Merge", "Object");
  accept("P16Merge", 0, "x\n  ^1");
  const std::int64_t second = newSet("Second");
  accept("P16Merge", 1, "y\n  ^2");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_move(second, first, "P16Merge", 0, nullptr, &err)) << err.message;
  EXPECT_EQ(0, ao_changes_class_count(second));
  EXPECT_EQ(std::vector<std::string>{"P16Merge+"}, classesOf(first));
  EXPECT_EQ(std::vector<std::string>{"x+"}, methodsOf(first, "P16Merge", 0));
  EXPECT_EQ(std::vector<std::string>{"y+"}, methodsOf(first, "P16Merge", 1));
  const std::int64_t third = newSet("Third");
  ASSERT_EQ(AO_OK, ao_changes_move(first, third, "P16Merge", 1, "y", &err)) << err.message;
  EXPECT_EQ(std::vector<std::string>{"P16Merge"}, classesOf(third));
  EXPECT_EQ(std::vector<std::string>{"y+"}, methodsOf(third, "P16Merge", 1));
}

// SPEC §3.10: a move to the same set or to an unknown one is refused, and nothing moves.
TEST_F(ChangesAbi, MoveRefusesSameOrUnknownSet) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16Stay", "Object");
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_changes_move(set, set, "P16Stay", 0, nullptr, &err));
  EXPECT_STREQ("same change set", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_move(set, set + 1000, "P16Stay", 0, nullptr, &err));
  EXPECT_STREQ("unknown change set", err.message);
  const std::int64_t other = newSet("Other");
  EXPECT_EQ(AO_ERR, ao_changes_move(set, other, "P16Stay", 0, "none", &err));
  EXPECT_STREQ("not in change set: P16Stay>>none", err.message);
  EXPECT_EQ(std::vector<std::string>{"P16Stay+"}, classesOf(set));
  EXPECT_EQ(0, ao_changes_class_count(other));
}

// SPEC §3.10 再入: from a hook drop and move do nothing and say "runtime is busy".
TEST_F(ChangesAbi, DropAndMoveAreRefusedWhileBusy) {
  const std::int64_t first = ao_changes_current();
  defineClass("P16Busy", "Object");
  const std::int64_t second = newSet("Second");
  int dropRc = -9;
  int moveRc = -9;
  std::string dropMsg;
  std::string moveMsg;
  whileBusy([&] {
    AoSpan err{};
    dropRc = ao_changes_drop(first, "P16Busy", 0, nullptr, &err);
    dropMsg = err.message;
    moveRc = ao_changes_move(first, second, "P16Busy", 0, nullptr, &err);
    moveMsg = err.message;
  });
  EXPECT_EQ(AO_ERR, dropRc);
  EXPECT_EQ("runtime is busy", dropMsg);
  EXPECT_EQ(AO_ERR, moveRc);
  EXPECT_EQ("runtime is busy", moveMsg);
  EXPECT_EQ(std::vector<std::string>{"P16Busy+"}, classesOf(first));
}
```

- [ ] **Step 2: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（リンクエラー `Undefined symbols: _ao_changes_drop`、`_ao_changes_move`）。

- [ ] **Step 3: 実装**

`ChangeSet.hpp` の `sessionChangesMethodAt` の宣言の後ろに足す。

```cpp
int sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector,
                       std::string* reason);
int sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta,
                       const char* selector, std::string* reason);
```

`ChangeSet.cpp` の無名名前空間の `kindOf` の後ろに足す。

```cpp
// "<Class>>><selector>", "<Class> class>><selector>" on the class side (SPEC §3.9 削除).
std::string entryName(std::string_view className, bool meta, std::string_view selector) {
  std::string out(className);
  if (meta) {
    out += " class";
  }
  out += ">>";
  out += selector;
  return out;
}

// Puts entry in set: over the entry of the same name, or at the end.
void putClass(ChangeSet& set, ClassChange&& entry) {
  if (ClassChange* found = findClass(set, entry.name)) {
    *found = std::move(entry);
    return;
  }
  set.classes.push_back(std::move(entry));
}

// The target's entry for className as a copy to merge into; a new one without a definition when
// the target has none.
ClassChange targetCopy(ChangeSet& to, const char* className) {
  if (ClassChange* found = findClass(to, className)) {
    return *found;
  }
  return ClassChange{className, std::nullopt, {}, {}};
}
```

ファイルの末尾（`}  // namespace ao` の前）に足す。どちらも、組み立て（コピー）が済んでから変えるので、途中の例外でセットは半端にならない。

```cpp
int sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector,
                       std::string* reason) {
  Session* s = session();
  if (s == nullptr || className == nullptr || (selector != nullptr && meta != 0 && meta != 1)) {
    return AO_ERR;
  }
  ChangeSet* set = findSet(*s, setId);
  if (set == nullptr) {
    *reason = "unknown change set";
    return AO_ERR;
  }
  ClassChange* entry = findClass(*set, className);
  if (selector == nullptr) {
    if (entry == nullptr) {
      *reason = std::string("not in change set: ") + className;
      return AO_ERR;
    }
    std::erase_if(set->classes, [&](const ClassChange& c) { return c.name == className; });
    return AO_OK;
  }
  if (entry == nullptr || sideOf(*entry, meta == 1).erase(selector) == 0) {
    *reason = "not in change set: " + entryName(className, meta == 1, selector);
    return AO_ERR;
  }
  dropIfEmpty(*set, className);
  return AO_OK;
}

int sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta,
                       const char* selector, std::string* reason) {
  Session* s = session();
  if (s == nullptr || className == nullptr || (selector != nullptr && meta != 0 && meta != 1)) {
    return AO_ERR;
  }
  ChangeSet* from = findSet(*s, fromId);
  ChangeSet* to = findSet(*s, toId);
  if (from == nullptr || to == nullptr) {
    *reason = "unknown change set";
    return AO_ERR;
  }
  if (from == to) {
    *reason = "same change set";
    return AO_ERR;
  }
  ClassChange* entry = findClass(*from, className);
  if (selector == nullptr) {
    if (entry == nullptr) {
      *reason = std::string("not in change set: ") + className;
      return AO_ERR;
    }
    // SPEC §3.10 チェンジセット: the moved kinds replace the target's; a definition kind the moved
    // entry lacks stays as the target has it.
    ClassChange merged = targetCopy(*to, className);
    if (entry->definition) {
      merged.definition = entry->definition;
    }
    for (const auto& [sel, kind] : entry->instanceSide) {
      merged.instanceSide.insert_or_assign(sel, kind);
    }
    for (const auto& [sel, kind] : entry->classSide) {
      merged.classSide.insert_or_assign(sel, kind);
    }
    putClass(*to, std::move(merged));
    std::erase_if(from->classes, [&](const ClassChange& c) { return c.name == className; });
    return AO_OK;
  }
  if (entry == nullptr) {
    *reason = "not in change set: " + entryName(className, meta == 1, selector);
    return AO_ERR;
  }
  auto& side = sideOf(*entry, meta == 1);
  const auto found = side.find(selector);
  if (found == side.end()) {
    *reason = "not in change set: " + entryName(className, meta == 1, selector);
    return AO_ERR;
  }
  ClassChange merged = targetCopy(*to, className);
  sideOf(merged, meta == 1).insert_or_assign(found->first, found->second);
  // to is another vector, so entry, side and found stay valid.
  putClass(*to, std::move(merged));
  side.erase(found);
  dropIfEmpty(*from, className);
  return AO_OK;
}
```

`abi.cpp` の末尾に足す。

```cpp
extern "C" int ao_changes_drop(int64_t set_id, const char* class_name, int meta,
                               const char* selector, AoSpan* err) {
  return changesWrite(err, "change set failed", [&](std::string* reason) {
    return ao::sessionChangesDrop(set_id, class_name, meta, selector, reason);
  });
}

extern "C" int ao_changes_move(int64_t from_id, int64_t to_id, const char* class_name, int meta,
                               const char* selector, AoSpan* err) {
  return changesWrite(err, "change set failed", [&](std::string* reason) {
    return ao::sessionChangesMove(from_id, to_id, class_name, meta, selector, reason);
  });
}
```

- [ ] **Step 4: テストが通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'ChangesAbi|ChangeKinds' --output-on-failure`
Expected: PASS（`ChangesAbi` は 23 件）。

- [ ] **Step 5: コミット**

```bash
git add runtime/src/ChangeSet.hpp runtime/src/ChangeSet.cpp runtime/src/abi.cpp runtime/tests/changes_abi_test.cpp
git commit -m "Add ao_changes_drop and ao_changes_move (P16)" -m "Remove from Change Set forgets an entry without touching the code; Move to
takes it to another set with its kind, replacing the target's.

Graphify: query ChangeSet sessionChangesMove
Serena: get_symbols_overview runtime/src/ChangeSet.cpp

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: File Out

**Files:**
- Modify: `runtime/src/Session.hpp` / `Session.cpp`（`definitionOf` を `classDefinitionText` に、`boundClassNamed`、`superclassNameOf`、`liveMethodSource`）
- Modify: `runtime/src/ChangeSet.hpp` / `ChangeSet.cpp`（`sessionChangesFileOut` と書き出し）
- Modify: `runtime/src/abi.cpp`
- Test: `runtime/tests/changes_abi_test.cpp`

**Interfaces:**
- Consumes: Task 4 の `entryName`。
- Produces（`Session.hpp`）: `Oop ao::boundClassNamed(Session&, const std::string& name)`、`std::string ao::classDefinitionText(Session&, Oop cls)`、`std::string ao::superclassNameOf(Session&, Oop cls)`、`bool ao::liveMethodSource(Session&, Oop cls, bool meta, const std::string& selector, std::string& text)`。
- Produces: `int ao::sessionChangesFileOut(std::int64_t setId, const char* path, std::string* reason)`。テストの補助 `readFile(path)`、`afterHeader(text)`、`fileOut(set, name)`。

- [ ] **Step 1: Serena で参照を取る**

`find_referencing_symbols` で `definitionOf`（`runtime/src/Session.cpp`）を取る。呼び出し元は `browserClassDefinition` だけのはず。

- [ ] **Step 2: 失敗するテストを書く**

無名名前空間の末尾に補助を足す。

```cpp
std::string readFile(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  std::ostringstream all;
  all << in.rdbuf();
  return all.str();
}

// The file out after its header chunk (whose date changes).
std::string afterHeader(const std::string& text) {
  const std::size_t end = text.find("\"!\n\n");
  return end == std::string::npos ? text : text.substr(end + 4);
}

// The set filed out to a temporary file, read back.
std::string fileOut(std::int64_t set, const std::string& name) {
  const std::string path = tempPath(name);
  AoSpan err{};
  EXPECT_EQ(AO_OK, ao_changes_file_out(set, path.c_str(), &err)) << err.message;
  const std::string text = readFile(path);
  std::filesystem::remove(path);
  return text;
}
```

テストを足す。

```cpp
// SPEC §3.9 File Out: the header, the definitions, the methods by class and side, then the
// removals as comment chunks.
TEST_F(ChangesAbi, FileOutWritesHeaderDefinitionsMethodsAndRemovals) {
  defineClass("P16Counter", "Object", "count", "Examples");
  accept("P16Counter", 0, "reset\n  count := 0");
  const std::int64_t set = newSet("Unnamed2");
  defineClass("P16Counter", "Object", "count", "Examples");
  accept("P16Counter", 0, "increment\n  count := (count ifNil: [0]) + 1.\n  ^count");
  accept("P16Counter", 1, "new\n  ^super new reset");
  EXPECT_EQ("", removeMethod("P16Counter", 0, "reset"));
  const std::string text = fileOut(set, "counter.st");
  char version[32];
  ASSERT_EQ(AO_OK, ao_version(version, static_cast<int>(sizeof version)));
  EXPECT_TRUE(std::regex_search(
      text, std::regex("^\"Change Set: Unnamed2\nDate: [0-9]{4}-[0-9]{2}-[0-9]{2}\nAo ")))
      << text;
  EXPECT_NE(std::string::npos, text.find("\nAo " + std::string(version) + "\"!\n\n")) << text;
  EXPECT_EQ(
      "Object subclass: #P16Counter\n"
      "  instanceVariableNames: 'count'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'Examples'!\n"
      "\n"
      "!P16Counter methodsFor: 'user'!\n"
      "increment\n"
      "  count := (count ifNil: [0]) + 1.\n"
      "  ^count! !\n"
      "\n"
      "!P16Counter class methodsFor: 'user'!\n"
      "new\n"
      "  ^super new reset! !\n"
      "\n"
      "\"Removed: P16Counter>>reset\"!\n",
      afterHeader(text));
}

// SPEC §3.9 File Out: a superclass in the set is defined before its subclasses, whatever order the
// set recorded them in.
TEST_F(ChangesAbi, FileOutDefinesSuperclassesFirst) {
  defineClass("P16Base", "Object");
  defineClass("P16Sub", "P16Base");
  const std::int64_t set = newSet("Order");
  defineClass("P16Sub", "P16Base", "", "Other");
  defineClass("P16Base", "Object", "", "Other");
  EXPECT_EQ((std::vector<std::string>{"P16Sub~", "P16Base~"}), classesOf(set));
  const std::string body = afterHeader(fileOut(set, "order.st"));
  const std::size_t base = body.find("Object subclass: #P16Base");
  const std::size_t sub = body.find("P16Base subclass: #P16Sub");
  ASSERT_NE(std::string::npos, base) << body;
  ASSERT_NE(std::string::npos, sub) << body;
  EXPECT_LT(base, sub);
}

// SPEC §3.8, §3.9 File Out: every `!` is doubled, in strings and comments too, and a `"` in the
// set's name is doubled in the header comment.
TEST_F(ChangesAbi, FileOutDoublesBangsAndQuotes) {
  const std::int64_t set = newSet("Fix \"1\"!");
  defineClass("P16Bang", "Object");
  accept("P16Bang", 0, "shout\n  \"Loud!\"\n  ^'Hi!'");
  const std::string text = fileOut(set, "bang.st");
  EXPECT_EQ(0u, text.find("\"Change Set: Fix \"\"1\"\"!!\nDate: ")) << text;
  EXPECT_NE(std::string::npos, text.find("shout\n  \"Loud!!\"\n  ^'Hi!!'! !\n")) << text;
}

// SPEC §3.9 File Out: a method without source is a comment chunk after the removals, not code.
TEST_F(ChangesAbi, FileOutWritesACommentForAMethodWithoutSource) {
  const std::int64_t set = ao_changes_current();
  defineClass("P16NoSrc", "Object");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!P16NoSrc methodsFor: 'x'!\nlost\n  ^1! !", &err))
      << err.message;
  const std::string body = afterHeader(fileOut(set, "nosrc.st"));
  EXPECT_EQ(std::string::npos, body.find("methodsFor:")) << body;
  EXPECT_NE(std::string::npos, body.find("\"P16NoSrc>>lost source not available\"!\n")) << body;
}

// SPEC §3.9 File Out: a removed class is a comment chunk, and its definition is not written.
TEST_F(ChangesAbi, FileOutWritesAClassRemovalAsAComment) {
  defineClass("P16Scratch", "Object");
  const std::int64_t set = newSet("Removals");
  EXPECT_EQ("", removeClass("P16Scratch"));
  EXPECT_EQ("\"Removed class: P16Scratch\"!\n", afterHeader(fileOut(set, "scratch.st")));
}

// SPEC §3.10 チェンジセット: an unknown set, a path that cannot be written and a NULL path say why.
TEST_F(ChangesAbi, FileOutRefusesUnknownSetAndUnwritablePath) {
  AoSpan err{};
  const std::int64_t set = ao_changes_current();
  EXPECT_EQ(AO_ERR, ao_changes_file_out(set + 1000, tempPath("x.st").c_str(), &err));
  EXPECT_STREQ("unknown change set", err.message);
  const std::string bad = tempPath("no-such-dir") + "/x.st";
  EXPECT_EQ(AO_ERR, ao_changes_file_out(set, bad.c_str(), &err));
  EXPECT_EQ("cannot write file: " + bad, std::string(err.message));
  EXPECT_EQ(AO_ERR, ao_changes_file_out(set, nullptr, &err));
  EXPECT_STREQ("file out failed", err.message);
}

// SPEC §3.10 再入: from a hook File Out writes nothing and says "runtime is busy".
TEST_F(ChangesAbi, FileOutIsRefusedWhileBusy) {
  const std::string path = tempPath("busy.st");
  int rc = -9;
  std::string msg;
  whileBusy([&] {
    AoSpan err{};
    rc = ao_changes_file_out(ao_changes_current(), path.c_str(), &err);
    msg = err.message;
  });
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("runtime is busy", msg);
  EXPECT_FALSE(std::filesystem::exists(path));
}
```

- [ ] **Step 3: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（リンクエラー `Undefined symbols: _ao_changes_file_out`）。

- [ ] **Step 4: Session の読みを足す**

`Session.hpp` の `writeBuf` の宣言の後ろに足す。

```cpp
// SPEC §3.9 File Out (P16): what the change set writer reads, by the class's own name.
// The class Smalltalk binds name to, when it is a class (not a metaclass, not another global) whose
// own name is name; the empty Oop otherwise. Allocates nothing.
Oop boundClassNamed(Session& s, const std::string& name);
// ao_browser_class_definition's text for cls: chunk-safe already (the category's `!` and `'`
// doubled).
std::string classDefinitionText(Session& s, Oop cls);
// The own name of cls's superclass; empty for nil.
std::string superclassNameOf(Session& s, Oop cls);
// The source-table text of the method selector names in cls's side (the metaclass's when meta);
// false when there is no such method, it is a NativeMethod, or it has no source.
bool liveMethodSource(Session& s, Oop cls, bool meta, const std::string& selector,
                      std::string& text);
```

`Session.cpp`: 無名名前空間の `definitionOf` を削除する。`browserClassDefinition` の前に（`insert_before_symbol`）、元の本文を `cls` と `classNameOf(s.heap, cls)` で書いた `classDefinitionText` と、残りの 3 つを置く。

```cpp
std::string classDefinitionText(Session& s, Oop cls) {
  std::string text;
  if (!pointerSlots(s.heap, cls, kClassSlotSuperclass + 1)) {
    text = "nil";
  } else {
    const Oop sup = s.heap.slotAt(cls, kClassSlotSuperclass);
    text = sup.isHeap() ? classNameOf(s.heap, sup) : std::string("nil");
  }
  text += " subclass: #";
  text += classNameOf(s.heap, cls);
  text += "\n  instanceVariableNames: '";
  text += instVarList(s.heap, cls);
  text += "'\n  classVariableNames: '";
  text += classVarList(s.heap, s.wk, cls);
  text += "'\n  poolDictionaries: ''\n  category: '";
  // SPEC §3.10: a string literal in a chunk, so accepting the text again keeps the category.
  for (const char c : definitionCategory(s.heap, cls)) {
    if (c == '\'' || c == '!') {
      text.push_back(c);
    }
    text.push_back(c);
  }
  text += "'";
  return text;
}

Oop boundClassNamed(Session& s, const std::string& name) {
  const Oop cls = s.wk.named(name);
  if (!pointerSlots(s.heap, cls, kClassSlotCount) ||
      s.heap.klass(s.heap.klass(cls)) != s.wk.metaclassClass || classNameOf(s.heap, cls) != name) {
    return Oop{};
  }
  return cls;
}

std::string superclassNameOf(Session& s, Oop cls) {
  if (!pointerSlots(s.heap, cls, kClassSlotSuperclass + 1)) {
    return {};
  }
  const Oop sup = s.heap.slotAt(cls, kClassSlotSuperclass);
  return sup.isHeap() ? classNameOf(s.heap, sup) : std::string();
}

bool liveMethodSource(Session& s, Oop cls, bool meta, const std::string& selector,
                      std::string& text) {
  const Oop side = sideOf(s, cls, meta ? 1 : 0);
  const Oop sel = s.wk.findSymbol(selector);
  if (!pointerSlots(s.heap, side, kClassSlotMethodDict + 1) || !sel.isHeap()) {
    return false;
  }
  const Oop method = MethodDictionary::at(s.heap, s.heap.slotAt(side, kClassSlotMethodDict), sel);
  if (!method.isHeap() || s.heap.klass(method) == s.wk.nativeMethodClass) {
    return false;
  }
  return methodSource(method, text);
}
```

`browserClassDefinition` の `return writeBuf(definitionOf(*s, *row), buf, len);` を `return writeBuf(classDefinitionText(*s, row->cls), buf, len);` にする。

- [ ] **Step 5: 書き出しを書く**

`ChangeSet.hpp` の `sessionChangesMove` の宣言の後ろに足す。

```cpp
// SPEC §3.9 File Out: writes the set to path, replacing it.
int sessionChangesFileOut(std::int64_t setId, const char* path, std::string* reason);
```

`ChangeSet.cpp` の include に `#include "ao/Runtime.hpp"`、`#include <ctime>`、`#include <fstream>`、`#include <unordered_set>` を足す。無名名前空間の末尾に足す。

```cpp
// SPEC §3.8 チャンク形式: every `!` doubled, in strings and comments too, then the closing `!`.
std::string chunk(std::string_view text) {
  std::string out;
  out.reserve(text.size() + 1);
  for (const char c : text) {
    if (c == '!') {
      out.push_back('!');
    }
    out.push_back(c);
  }
  out.push_back('!');
  return out;
}

// A comment chunk: text in double quotes (a `"` in it doubled), as a chunk.
std::string commentChunk(std::string_view text) {
  std::string body = "\"";
  for (const char c : text) {
    if (c == '"') {
      body.push_back('"');
    }
    body.push_back(c);
  }
  body.push_back('"');
  return chunk(body);
}

// The local date, YYYY-MM-DD.
std::string today() {
  const std::time_t now = std::time(nullptr);
  std::tm local{};
  localtime_r(&now, &local);
  char buf[16];
  return std::strftime(buf, sizeof buf, "%Y-%m-%d", &local) == 0 ? std::string()
                                                                  : std::string(buf);
}

// SPEC §3.9 File Out: names, each after the superclasses it has among them, otherwise in order.
std::vector<std::string> superclassOrder(Session& s, const std::vector<std::string>& names) {
  const std::unordered_set<std::string> wanted(names.begin(), names.end());
  std::unordered_set<std::string> placed;
  std::vector<std::string> out;
  for (const std::string& name : names) {
    // The chain from name up to the first class out of the set or already placed.
    std::vector<std::string> chain;
    std::string at = name;
    while (wanted.count(at) != 0 && placed.count(at) == 0 &&
           std::find(chain.begin(), chain.end(), at) == chain.end()) {
      chain.push_back(at);
      const Oop cls = boundClassNamed(s, at);
      at = cls.isEmpty() ? std::string() : superclassNameOf(s, cls);
    }
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
      placed.insert(*it);
      out.push_back(*it);
    }
  }
  return out;
}

// SPEC §3.9 File Out の形式.
std::string fileOutText(Session& s, const ChangeSet& set) {
  char version[32] = "";
  version_string(version, static_cast<int>(sizeof version));
  std::string out = commentChunk("Change Set: " + set.name + "\nDate: " + today() + "\nAo " +
                                 version);
  out += "\n\n";
  // What has no source, as comment chunks after the removals.
  std::vector<std::string> lost;
  std::vector<std::string> defined;
  for (const ClassChange& c : set.classes) {
    if (c.definition && *c.definition != ChangeKind::Removed) {
      defined.push_back(c.name);
    }
  }
  for (const std::string& name : superclassOrder(s, defined)) {
    const Oop cls = boundClassNamed(s, name);
    if (cls.isEmpty()) {
      lost.push_back(name + " definition not available");
      continue;
    }
    // Chunk-safe already: written as it is.
    out += classDefinitionText(s, cls) + "!\n\n";
  }
  for (const ClassChange& c : set.classes) {
    const Oop cls = boundClassNamed(s, c.name);
    for (const bool meta : {false, true}) {
      std::vector<std::string> sources;
      for (const auto& [selector, kind] : meta ? c.classSide : c.instanceSide) {
        if (kind == ChangeKind::Removed) {
          continue;
        }
        std::string text;
        if (!cls.isEmpty() && liveMethodSource(s, cls, meta, selector, text)) {
          sources.push_back(std::move(text));
        } else {
          lost.push_back(entryName(c.name, meta, selector) + " source not available");
        }
      }
      if (sources.empty()) {
        continue;
      }
      out += "!" + c.name + (meta ? " class" : "") + " methodsFor: 'user'!\n";
      for (std::size_t i = 0; i < sources.size(); ++i) {
        out += chunk(sources[i]);
        out += i + 1 < sources.size() ? "\n\n" : " !\n\n";
      }
    }
  }
  for (const ClassChange& c : set.classes) {
    for (const bool meta : {false, true}) {
      for (const auto& [selector, kind] : meta ? c.classSide : c.instanceSide) {
        if (kind == ChangeKind::Removed) {
          out += commentChunk("Removed: " + entryName(c.name, meta, selector)) + "\n";
        }
      }
    }
  }
  for (const ClassChange& c : set.classes) {
    if (c.definition == ChangeKind::Removed) {
      out += commentChunk("Removed class: " + c.name) + "\n";
    }
  }
  for (const std::string& text : lost) {
    out += commentChunk(text) + "\n";
  }
  return out;
}
```

ファイルの末尾に足す。

```cpp
int sessionChangesFileOut(std::int64_t setId, const char* path, std::string* reason) {
  Session* s = session();
  if (s == nullptr || path == nullptr) {
    return AO_ERR;
  }
  ChangeSet* set = findSet(*s, setId);
  if (set == nullptr) {
    *reason = "unknown change set";
    return AO_ERR;
  }
  const std::string text = fileOutText(*s, *set);
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(text.data(), static_cast<std::streamsize>(text.size()));
  out.close();
  if (!out) {
    *reason = std::string("cannot write file: ") + path;
    return AO_ERR;
  }
  return AO_OK;
}
```

`abi.cpp` の末尾に足す。

```cpp
extern "C" int ao_changes_file_out(int64_t set_id, const char* path, AoSpan* err) {
  return changesWrite(err, "file out failed", [&](std::string* reason) {
    return ao::sessionChangesFileOut(set_id, path, reason);
  });
}
```

- [ ] **Step 6: テストが通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'ChangesAbi|ChangeKinds|BrowserAbi|AcceptAbi' --output-on-failure`
Expected: PASS。`BrowserAbi` と `AcceptAbi` が緑なら `classDefinitionText` への置き換えで定義の本文は変わっていない。

- [ ] **Step 7: コミット**

```bash
git add runtime/src/Session.hpp runtime/src/Session.cpp runtime/src/ChangeSet.hpp runtime/src/ChangeSet.cpp runtime/src/abi.cpp runtime/tests/changes_abi_test.cpp
git commit -m "Add ao_changes_file_out (P16)" -m "File Out writes a header comment, the definitions superclasses first, the
methods by class and side, then removals and lost sources as comment chunks.
Every bang is doubled.

Graphify: path browserClassDefinition classDefinitionText
Serena: find_referencing_symbols ao::(anonymous)::definitionOf

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 6: File In（`ao_filein_file`）

**Files:**
- Modify: `runtime/include/ao/Compile.hpp`（`FileInMode`、`fileInString` / `fileInFile` の引数）
- Modify: `runtime/src/Compile.cpp`（`applyMethodsFor` の `Recorded`、`fileInString`、`fileInFile`）
- Modify: `runtime/src/Session.hpp` / `Session.cpp`（`sessionFileInFile`）
- Modify: `runtime/src/abi.cpp`
- Test: `runtime/tests/changes_abi_test.cpp`、`runtime/tests/chunk_filein_test.cpp`

**Interfaces:**
- Consumes: Task 3 の `ChunkMode::Recorded`（宣言済み）と記録、Task 5 の `ao_changes_file_out`。
- Produces: `enum class ao::FileInMode { Plain, Recorded }`、`bool ao::fileInString(CallContext&, std::string_view, std::vector<compiler::CompileError>&, FileInMode mode = FileInMode::Plain)`、`bool ao::fileInFile(CallContext&, const std::filesystem::path&, std::vector<compiler::CompileError>&, FileInMode mode = FileInMode::Plain)`、`int ao::sessionFileInFile(const char* path, compiler::CompileError* first)`、C ABI `ao_filein_file`。補助 `sourceOf(cls, meta, sel)`、`writeTemp(name, text)`。

- [ ] **Step 1: Serena で参照を取る**

`find_referencing_symbols` で `fileInString` と `fileInFile` を取る。既定値を足すので呼び出し元（`fileInFile`、CLI の `runFileIn`、テスト）は変えない。

- [ ] **Step 2: 失敗するテストを書く**

`chunk_filein_test.cpp` の末尾に足す。

```cpp
// SPEC §3.9 File Out: the header, removal and lost-source comment chunks of a filed-out change set
// are expressions, which a file-in reads past without an error.
TEST(ChunkFileIn, FiledOutCommentChunksAreSkipped) {
  Boot b;
  const char* src =
      "\"Change Set: Fix \"\"1\"\"!!\nDate: 2026-10-11\nAo 1.5.0\"!\n\n"
      "\"Removed: Counter>>reset\"!\n"
      "\"Removed class: Scratch\"!\n"
      "\"Counter>>total source not available\"!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(src, errs);
  ASSERT_TRUE(errs.empty()) << errs[0].message;
  ASSERT_EQ(4u, acts.size());
  for (const auto& a : acts) {
    EXPECT_EQ(ao::compiler::ChunkKind::DoIt, a.kind);
  }
  EXPECT_TRUE(ao::fileInString(b.ctx, src, errs)) << (errs.empty() ? "" : errs[0].message);
}
```

`changes_abi_test.cpp` の無名名前空間の末尾に補助を足す。

```cpp
// The Browser's source of the method, or "<rc>" when it has none.
std::string sourceOf(const char* cls, int meta, const char* sel) {
  char buf[1024];
  const int rc =
      ao_browser_source(ao_browser_class_id(cls), meta, sel, buf, static_cast<int>(sizeof buf));
  return rc == AO_OK ? std::string(buf) : "<" + std::to_string(rc) + ">";
}

std::string writeTemp(const std::string& name, const std::string& text) {
  const std::string path = tempPath(name);
  std::ofstream(path, std::ios::binary) << text;
  return path;
}
```

テストを足す。

```cpp
// SPEC §3.10 File In... (P16): the methods keep their sources, and the definitions and methods go
// into the current set.
TEST_F(ChangesAbi, FileInKeepsSourcesAndRecordsInTheCurrentSet) {
  const std::string path = writeTemp("filein.st",
                                     "Object subclass: #P16In\n"
                                     "  instanceVariableNames: 'n'\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n\n"
                                     "!P16In methodsFor: 'user'!\n"
                                     "n\n  ^n ifNil: [0]!\n\n"
                                     "bump\n  n := self n + 1! !\n\n"
                                     "!P16In class methodsFor: 'user'!\n"
                                     "fresh\n  ^self new! !\n");
  AoSpan err{};
  std::strcpy(err.message, "stale");
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_STREQ("", err.message);
  const std::int64_t set = ao_changes_current();
  EXPECT_EQ(std::vector<std::string>{"P16In+"}, classesOf(set));
  EXPECT_EQ((std::vector<std::string>{"bump+", "n+"}), methodsOf(set, "P16In", 0));
  EXPECT_EQ(std::vector<std::string>{"fresh+"}, methodsOf(set, "P16In", 1));
  EXPECT_EQ("n\n  ^n ifNil: [0]", sourceOf("P16In", 0, "n"));
  EXPECT_EQ("bump\n  n := self n + 1", sourceOf("P16In", 0, "bump"));
  EXPECT_EQ("fresh\n  ^self new", sourceOf("P16In", 1, "fresh"));
  EXPECT_EQ("0", printIt("P16In fresh n"));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: a definition of a class that exists keeps it (as ao_accept_class), so its
// other methods stay; the vendor file-in would rebind the name to a new class.
TEST_F(ChangesAbi, FileInKeepsTheMethodsOfAnExistingClass) {
  defineClass("P16Keep", "Object", "", "P16-In");
  accept("P16Keep", 0, "old\n  ^1");
  const std::int64_t set = newSet("FileIn");
  const std::string path = writeTemp("keep.st",
                                     "Object subclass: #P16Keep\n"
                                     "  instanceVariableNames: ''\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n\n"
                                     "!P16Keep methodsFor: 'user'!\n"
                                     "added\n  ^2! !\n");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_EQ("1", printIt("P16Keep new old"));
  EXPECT_EQ("2", printIt("P16Keep new added"));
  EXPECT_EQ(std::vector<std::string>{"P16Keep~"}, classesOf(set));
  EXPECT_EQ(std::vector<std::string>{"added+"}, methodsOf(set, "P16Keep", 0));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: an error answers the first one with its span in the file; the chunks
// before it stay applied and recorded.
TEST_F(ChangesAbi, FileInErrorAnswersTheFirstErrorAndKeepsEarlierChunks) {
  const std::string text =
      "Object subclass: #P16Early\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P16-In'!\n\n"
      "!P16Acount methodsFor: 'user'!\n"
      "x\n  ^1! !\n";
  const std::string path = writeTemp("broken.st", text);
  AoSpan err{};
  ASSERT_EQ(AO_ERR, ao_filein_file(path.c_str(), &err));
  EXPECT_STREQ("missing class: P16Acount", err.message);
  ASSERT_LT(err.start, err.end);
  EXPECT_EQ("P16Acount methodsFor: 'user'", text.substr(err.start, err.end - err.start));
  EXPECT_EQ(std::vector<std::string>{"P16Early+"}, classesOf(ao_changes_current()));
  std::filesystem::remove(path);
}

// SPEC §3.10 File In...: an unreadable path and a NULL path say why.
TEST_F(ChangesAbi, FileInRefusesAnUnreadablePath) {
  const std::string path = tempPath("missing.st");
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_filein_file(path.c_str(), &err));
  EXPECT_EQ("cannot read: " + path, std::string(err.message));
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  EXPECT_EQ(AO_ERR, ao_filein_file(nullptr, &err));
  EXPECT_STREQ("file-in failed", err.message);
}

// SPEC §3.9 File Out, §3.10 File In...: a filed-out set files back in after a new boot with the same
// definitions and sources, recorded as added, so filing it out again writes the same body.
TEST_F(ChangesAbi, FiledOutSetFilesBackInAfterBoot) {
  defineClass("P16Trip", "Object", "count", "Trips");
  accept("P16Trip", 0, "increment\n  count := (count ifNil: [0]) + 1.\n  ^count");
  accept("P16Trip", 1, "start\n  ^self new increment; yourself");
  accept("P16Trip", 0, "shout\n  ^'Hi!'");
  const std::string path = tempPath("trip.st");
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_changes_file_out(ao_changes_current(), path.c_str(), &err)) << err.message;
  const std::string first = afterHeader(readFile(path));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_filein_file(path.c_str(), &err)) << err.message;
  EXPECT_EQ("shout\n  ^'Hi!'", sourceOf("P16Trip", 0, "shout"));
  EXPECT_EQ("2", printIt("P16Trip start increment"));
  EXPECT_EQ(first, afterHeader(fileOut(ao_changes_current(), "trip-again.st")));
  std::filesystem::remove(path);
}

// SPEC §3.10 再入: from a hook File In applies nothing and says "runtime is busy".
TEST_F(ChangesAbi, FileInIsRefusedWhileBusy) {
  const std::string path = writeTemp("busy-in.st",
                                     "Object subclass: #P16BusyIn\n"
                                     "  instanceVariableNames: ''\n"
                                     "  classVariableNames: ''\n"
                                     "  poolDictionaries: ''\n"
                                     "  category: 'P16-In'!\n");
  int rc = -9;
  std::string msg;
  whileBusy([&] {
    AoSpan err{};
    rc = ao_filein_file(path.c_str(), &err);
    msg = err.message;
  });
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("runtime is busy", msg);
  EXPECT_EQ(0, ao_browser_class_id("P16BusyIn"));
  std::filesystem::remove(path);
}
```

- [ ] **Step 3: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（リンクエラー `Undefined symbols: _ao_filein_file`）。`FiledOutCommentChunksAreSkipped` だけはこの時点で通ってよい（チャンクの読みは変えない。そのことを固定するテスト）。

- [ ] **Step 4: `Compile.hpp` と `Compile.cpp`**

`Compile.hpp` の `applyChunks` の宣言の前に足し、`fileInString` と `fileInFile` の宣言に引数を足す。

```cpp
// SPEC §3.10 File In... (P16). Plain: the vendor file-in and `ao filein` (a definition makes a new
// class; no sources, no change records). Recorded: File In...: a definition keeps or reshapes an
// existing class (as ao_accept_class), each method keeps its source (as an Accept), and both are
// recorded in the current change set (SPEC §3.9).
enum class FileInMode { Plain, Recorded };
```

```cpp
bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors,
                  FileInMode mode = FileInMode::Plain);
bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors,
                FileInMode mode = FileInMode::Plain);
```

`Compile.cpp` の `applyMethodsFor` の `Root kept(...)` の行から、その後の `if (mode != ChunkMode::FileIn) { noteMethodChange(...); }` までを次にする（`replace_content`）。

```cpp
    // SPEC §3.10 File In... (P16): the text is boxed before the install, as Accept boxes it.
    Root text(ctx.roots, Oop::nil());
    if (mode == ChunkMode::Recorded) {
      text.slot = boxUtf8(ctx, m.source);
      if (!text.slot.isHeap()) {
        addError(errors, {m.span, "method source allocation failed"},
                 methodKey(action, cr.image.selector));
        continue;
      }
    }
    Root kept(ctx.roots, installMethod(ctx, tgt.slot, cr.image));
    if (!kept.slot.isHeap()) {
      addError(errors, {m.span, "install failed"}, methodKey(action, cr.image.selector));
      continue;
    }
    if (mode == ChunkMode::Recorded) {
      // kept was boxed from cr.image, so its blocks sit at the image's literal indices.
      rememberMethodSource(kept.slot, text.slot, old.slot.isHeap() ? old.slot : Oop{}, &cr.image);
    }
    if (mode != ChunkMode::FileIn) {
      noteMethodChange(ctx, own, action.meta, cr.image.selector, old.slot.isHeap());
    }
```

`fileInString` と `fileInFile` を次にする。

```cpp
bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors, FileInMode mode) {
  const std::size_t before = errors.size();
  const std::vector<compiler::ChunkAction> actions = compiler::parseChunks(src, errors);
  std::vector<FileInError> found;
  const bool completed = applyChunkActions(
      ctx, actions, found, mode == FileInMode::Recorded ? ChunkMode::Recorded : ChunkMode::FileIn);
  for (auto& e : found) {
    errors.push_back(std::move(e.error));
  }
  return completed && errors.size() == before;
}

bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors, FileInMode mode) {
  std::string src;
  if (!readSource(path, &src)) {
    errors.push_back(compiler::CompileError{{}, "cannot read: " + path.string()});
    return false;
  }
  return fileInString(ctx, src, errors, mode);
}
```

- [ ] **Step 5: `sessionFileInFile` と ABI**

`Session.hpp` の `int sessionFileInLoadOrder(const char* path);` の後ろに足す（`compiler::CompileError` は `ao/Span.hpp` にあり、`Session.hpp` がすでに含む `ao/MethodImage.hpp` から見える）。

```cpp
// SPEC §3.10 ao_filein_file (P16): fileInFile in FileInMode::Recorded on the session. Nonzero on
// any error; *first (when given) is the first one, its span in the file.
int sessionFileInFile(const char* path, compiler::CompileError* first);
```

`Session.cpp` の `sessionFileInLoadOrder` の後ろに足す。

```cpp
int sessionFileInFile(const char* path, compiler::CompileError* first) {
  if (g_session == nullptr || g_session->ctx == nullptr || path == nullptr) {
    return 1;
  }
  // SPEC §3.10 File In...: any error fails it; the first one says where and why.
  std::vector<compiler::CompileError> errors;
  const bool ok = fileInFile(*g_session->ctx, path, errors, FileInMode::Recorded);
  if (ok && errors.empty()) {
    return 0;
  }
  if (first != nullptr && !errors.empty()) {
    *first = std::move(errors.front());
  }
  return 1;
}
```

`abi.cpp` の末尾に足す（`clearSpan` / `fillSpan` より後ろである必要がある）。

```cpp
// SPEC §3.10 File In... (P16): outermost. AO_ERR with the first error's span and message, or
// "runtime is busy" / "file-in failed" with 0-0.
extern "C" int ao_filein_file(const char* path, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  ao::compiler::CompileError first;
  const int rc = guarded(-1, [&] {
    return ao::sessionFileInFile(path, &first) == 0 ? AO_OK : AO_ERR;
  });
  if (rc == AO_OK) {
    return AO_OK;
  }
  if (rc == -1 || first.message.empty()) {
    setMessage(err, "file-in failed");
  } else {
    fillSpan(err, first);
  }
  return AO_ERR;
}
```

- [ ] **Step 6: テストが通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure`
Expected: PASS（全件。`ChangesAbi` は 36 件。CLI のテストも CTest に入っている）。CLI の `ao filein` は `FileInMode::Plain` のままなので変わらない。

- [ ] **Step 7: コミット**

```bash
git add runtime/include/ao/Compile.hpp runtime/src/Compile.cpp runtime/src/Session.hpp runtime/src/Session.cpp runtime/src/abi.cpp runtime/tests/changes_abi_test.cpp runtime/tests/chunk_filein_test.cpp
git commit -m "Add ao_filein_file for File In... (P16)" -m "File In... keeps each method's source, takes class definitions by the Accept
rules so an existing class keeps its methods, and records both in the current
set. The vendor file-in and ao filein stay as they were.

Graphify: path fileInFile applyMethodsFor
Serena: replace_symbol_body ao::fileInString

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 7: Swift のモデル（`ChangeSorterModel`）

**Files:**
- Create: `app/Ao/ChangeSorterModel.swift`
- Create: `app/AoTests/ChangeSorterTests.swift`

**Interfaces:**
- Consumes: Task 2〜6 の C ABI。`spanMessage(_:)`、`BrowserModel.classID(named:)`。
- Produces: `enum ChangeKind: Int32 { none, added, changed, removed; var mark: String }`、`struct ChangeSetRow { id: Int64; name: String }`、`struct ChangeClassRow { name: String; kind: ChangeKind }`、`struct ChangeMethodRow { selector: String; meta: Bool; kind: ChangeKind; var label: String }`、`@MainActor final class ChangeSorterModel`（`sets`、`currentID`、`classes`、`methods`、`source`、`selectedSetID`、`selectedClass`、`selectedMethod`、`currentName`、`selectedSetName`、`refresh()`、`select(setID:className:method:)`、`nextFreeName()`、`newSet(named:) -> (status: Int32, message: String)`、`makeCurrent(_:) -> Int32`、`remove(className:method:) -> (status: Int32, message: String)`、`move(className:method:to:) -> (status: Int32, message: String)`）、`func fileOutChangeSet(_ setID: Int64, to url: URL) -> (status: Int32, message: String)`。
- テストの補助（`ChangeSorterTests` の中）: `boot()`、`acceptClass(_:ivars:)`、`acceptMethod(_:meta:_:)`、`closeVisibleWindows()`。

- [ ] **Step 1: 失敗するテストを書く**

`app/AoTests/ChangeSorterTests.swift` を作る。

```swift
import AppKit
import XCTest
@testable import Ao

@MainActor
final class ChangeSorterTests: XCTestCase {
  override func tearDown() {
    // tearDown is nonisolated; XCTest calls it on the main thread.
    MainActor.assumeIsolated { Self.closeVisibleWindows() }
    ao_set_transcript_hook(nil, nil)
    ao_runtime_shutdown()
    super.tearDown()
  }

  // Nothing the test opened outlives it: sheets end first, then every visible window closes.
  private static func closeVisibleWindows() {
    let windows = NSApplication.shared.windows
    for window in windows {
      for sheet in window.sheets {
        window.endSheet(sheet)
      }
    }
    for window in windows where window.isVisible && !window.isSheet {
      window.isReleasedWhenClosed = false
      window.close()
    }
  }

  private func boot() {
    _ = NSApplication.shared
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
  }

  private func acceptClass(_ name: String, ivars: String = "") {
    var err = AoSpan()
    let source = "Object subclass: #\(name)\n  instanceVariableNames: '\(ivars)'\n"
      + "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'P16-Test'"
    XCTAssertEqual(ao_accept_class(source, &err), Int32(AO_OK), spanMessage(err))
  }

  private func acceptMethod(_ className: String, meta: Bool = false, _ source: String) {
    var err = AoSpan()
    XCTAssertEqual(ao_accept_method(className, meta ? 1 : 0, source, &err), Int32(AO_OK), spanMessage(err))
  }

  // SPEC §3.9 Change Sorter: a new session lists Unnamed1, current and selected.
  func testNewSessionListsUnnamed1AsCurrentAndSelected() {
    boot()
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.sets.map(\.name), ["Unnamed1"])
    XCTAssertEqual(model.selectedSetID, model.currentID)
    XCTAssertEqual(model.currentName, "Unnamed1")
    XCTAssertEqual(model.classes, [])
    XCTAssertEqual(model.source, "")
  }

  // SPEC §3.9 Change Sorter: the class shows its definition kind, the methods theirs (the class side
  // as `class » <selector>`), and the source pane the accepted text.
  func testAcceptedMethodShowsItsKindAndSource() {
    boot()
    acceptClass("P16Counter", ivars: "count")
    let source = "increment\n  count := (count ifNil: [0]) + 1.\n  ^count"
    acceptMethod("P16Counter", source)
    acceptMethod("P16Counter", meta: true, "make\n  ^self new")
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16Counter", kind: .added)])
    model.select(setID: model.currentID, className: "P16Counter")
    XCTAssertEqual(model.methods.map(\.label), ["increment", "class » make"])
    XCTAssertEqual(model.methods.map(\.kind.mark), ["+", "+"])
    XCTAssertTrue(model.source.hasPrefix("Object subclass: #P16Counter"), model.source)
    model.select(setID: model.currentID, className: "P16Counter", method: model.methods[0])
    XCTAssertEqual(model.source, source)
  }

  // SPEC §3.9 Change Sorter: a removed method or class shows `removed`.
  func testRemovedMethodAndRemovedClassShowRemoved() {
    boot()
    acceptClass("P16Gone")
    acceptMethod("P16Gone", "old\n  ^1")
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Later", nil, &err), Int32(AO_OK))
    XCTAssertEqual(ao_remove_method(BrowserModel.classID(named: "P16Gone"), 0, "old", &err), Int32(AO_OK))
    let model = ChangeSorterModel()
    model.refresh()
    model.select(
      setID: model.currentID,
      className: "P16Gone",
      method: ChangeMethodRow(selector: "old", meta: false, kind: .removed)
    )
    XCTAssertEqual(model.methods.map(\.kind.mark), ["−"])
    XCTAssertEqual(model.source, "removed")
    XCTAssertEqual(ao_remove_class(BrowserModel.classID(named: "P16Gone"), &err), Int32(AO_OK))
    model.refresh()
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16Gone", kind: .removed)])
    XCTAssertNil(model.selectedMethod)
    XCTAssertEqual(model.source, "removed")
  }

  // SPEC §3.9 Change Sorter: New's default is the smallest free Unnamed<n>; the new set is current
  // and selected; a taken or empty name is refused with the runtime's reason.
  func testNewSetIsCurrentAndNextFreeNameSkipsTakenNames() {
    boot()
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.nextFreeName(), "Unnamed2")
    XCTAssertEqual(model.newSet(named: "Unnamed3").status, Int32(AO_OK))
    XCTAssertEqual(model.currentName, "Unnamed3")
    XCTAssertEqual(model.selectedSetID, model.currentID)
    XCTAssertEqual(model.nextFreeName(), "Unnamed2")
    let taken = model.newSet(named: "Unnamed1")
    XCTAssertEqual(taken.status, Int32(AO_ERR))
    XCTAssertEqual(taken.message, "change set name taken: Unnamed1")
    XCTAssertEqual(model.newSet(named: "").message, "change set name empty")
    XCTAssertEqual(model.sets.map(\.name), ["Unnamed1", "Unnamed3"])
  }

  // SPEC §3.9 Change Sorter: Make Current, Move to and Remove from Change Set change the lists.
  func testMakeCurrentRemoveAndMoveUpdateTheLists() {
    boot()
    acceptClass("P16Move")
    acceptMethod("P16Move", "a\n  ^1")
    acceptMethod("P16Move", "b\n  ^2")
    let model = ChangeSorterModel()
    model.refresh()
    let first = model.currentID
    XCTAssertEqual(model.newSet(named: "Target").status, Int32(AO_OK))
    let target = model.currentID
    XCTAssertEqual(model.makeCurrent(first), Int32(AO_OK))
    XCTAssertEqual(model.currentID, first)
    model.select(setID: first, className: "P16Move")
    let a = model.methods[0]
    XCTAssertEqual(model.move(className: "P16Move", method: a, to: target).status, Int32(AO_OK))
    XCTAssertEqual(model.methods.map(\.label), ["b"])
    XCTAssertEqual(model.remove(className: "P16Move", method: nil).status, Int32(AO_OK))
    XCTAssertEqual(model.classes, [])
    XCTAssertNil(model.selectedClass)
    model.select(setID: target, className: "P16Move")
    XCTAssertEqual(model.methods.map(\.label), ["a"])
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16Move", kind: .none)])
  }
}
```

- [ ] **Step 2: 失敗を確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter ChangeSorterTests`
Expected: FAIL（`cannot find 'ChangeSorterModel' in scope`）。

- [ ] **Step 3: `app/Ao/ChangeSorterModel.swift` を作る**

```swift
import CAo
import Foundation

// SPEC §3.9 Change Sorter (P16): an entry's kind, as ao_changes_* answer it (SPEC §3.10).
enum ChangeKind: Int32 {
  case none = 0
  case added = 1
  case changed = 2
  case removed = 3

  // The mark after a name in the lists; none shows nothing.
  var mark: String {
    switch self {
    case .none:
      return ""
    case .added:
      return "+"
    case .changed:
      return "~"
    case .removed:
      return "−"
    }
  }
}

struct ChangeSetRow: Equatable {
  let id: Int64
  let name: String
}

struct ChangeClassRow: Equatable {
  let name: String
  let kind: ChangeKind
}

struct ChangeMethodRow: Equatable {
  let selector: String
  let meta: Bool
  let kind: ChangeKind

  // SPEC §3.9 Change Sorter: a class-side entry reads `class » <selector>`.
  var label: String {
    meta ? "class » \(selector)" : selector
  }
}

// SPEC §3.9 File Out: writes the set to url; on AO_ERR the message is the reason.
func fileOutChangeSet(_ setID: Int64, to url: URL) -> (status: Int32, message: String) {
  var err = AoSpan()
  let status = url.path.withCString { ao_changes_file_out(setID, $0, &err) }
  return (status, spanMessage(err))
}

// SPEC §3.9 Change Sorter: the window's model. The runtime holds the sets (SPEC §3.10 チェンジセット);
// this holds the selection and what the lists and the source pane show.
@MainActor
final class ChangeSorterModel {
  private(set) var sets: [ChangeSetRow] = []
  private(set) var currentID: Int64 = 0
  private(set) var classes: [ChangeClassRow] = []
  private(set) var methods: [ChangeMethodRow] = []
  private(set) var source = ""
  private(set) var selectedSetID: Int64?
  private(set) var selectedClass: String?
  private(set) var selectedMethod: ChangeMethodRow?

  var currentName: String? {
    sets.first { $0.id == currentID }?.name
  }

  var selectedSetName: String? {
    sets.first { $0.id == selectedSetID }?.name
  }

  // Reads everything again. A selected set the runtime no longer lists (after a boot or a load)
  // gives way to the current one; a class or method no longer in the set is deselected.
  func refresh() {
    sets = loadSets()
    currentID = ao_changes_current()
    if !sets.contains(where: { $0.id == selectedSetID }) {
      selectedSetID = sets.contains(where: { $0.id == currentID }) ? currentID : nil
      selectedClass = nil
      selectedMethod = nil
    }
    classes = selectedSetID.map { loadClasses($0) } ?? []
    if let name = selectedClass, !classes.contains(where: { $0.name == name }) {
      selectedClass = nil
    }
    if let setID = selectedSetID, let className = selectedClass {
      methods = loadMethods(setID: setID, className: className)
    } else {
      methods = []
    }
    selectedMethod = selectedMethod.flatMap { picked in
      methods.first { $0.selector == picked.selector && $0.meta == picked.meta }
    }
    source = loadSource()
  }

  func select(setID: Int64?, className: String? = nil, method: ChangeMethodRow? = nil) {
    selectedSetID = setID
    selectedClass = className
    selectedMethod = className == nil ? nil : method
    refresh()
  }

  // SPEC §3.9 Change Sorter: the smallest Unnamed<n>, n from 1, that no set is named.
  func nextFreeName() -> String {
    let taken = Set(sets.map(\.name))
    var n = 1
    while taken.contains("Unnamed\(n)") {
      n += 1
    }
    return "Unnamed\(n)"
  }

  // AO_OK selects the new set, which the runtime made current.
  func newSet(named name: String) -> (status: Int32, message: String) {
    var err = AoSpan()
    var id: Int64 = 0
    let status = name.withCString { ao_changes_new($0, &id, &err) }
    if status == Int32(AO_OK) {
      select(setID: id)
    } else {
      refresh()
    }
    return (status, spanMessage(err))
  }

  func makeCurrent(_ id: Int64) -> Int32 {
    let status = ao_changes_set_current(id)
    refresh()
    return status
  }

  // SPEC §3.9 Change Sorter: Remove from Change Set in the selected set. method nil forgets the
  // class entry with its methods.
  func remove(className: String, method: ChangeMethodRow?) -> (status: Int32, message: String) {
    guard let setID = selectedSetID else {
      return (Int32(AO_ERR), "")
    }
    var err = AoSpan()
    let status = className.withCString { cls in
      withSelector(method) { ao_changes_drop(setID, cls, method?.meta == true ? 1 : 0, $0, &err) }
    }
    refresh()
    return (status, spanMessage(err))
  }

  // SPEC §3.9 Change Sorter: Move to, from the selected set.
  func move(
    className: String,
    method: ChangeMethodRow?,
    to targetID: Int64
  ) -> (status: Int32, message: String) {
    guard let setID = selectedSetID else {
      return (Int32(AO_ERR), "")
    }
    var err = AoSpan()
    let status = className.withCString { cls in
      withSelector(method) {
        ao_changes_move(setID, targetID, cls, method?.meta == true ? 1 : 0, $0, &err)
      }
    }
    refresh()
    return (status, spanMessage(err))
  }

  private func loadSets() -> [ChangeSetRow] {
    let count = ao_changes_count()
    guard count > 0 else {
      return []
    }
    var rows: [ChangeSetRow] = []
    for index in 0..<count {
      var id: Int64 = 0
      guard let name = copyChangeText({ ao_changes_at(index, &id, $0, $1) }) else {
        return []
      }
      rows.append(ChangeSetRow(id: id, name: name))
    }
    return rows
  }

  private func loadClasses(_ setID: Int64) -> [ChangeClassRow] {
    let count = ao_changes_class_count(setID)
    guard count > 0 else {
      return []
    }
    var rows: [ChangeClassRow] = []
    for index in 0..<count {
      var kind: Int32 = 0
      guard let name = copyChangeText({ ao_changes_class_at(setID, index, $0, $1, &kind) }) else {
        return []
      }
      rows.append(ChangeClassRow(name: name, kind: ChangeKind(rawValue: kind) ?? ChangeKind.none))
    }
    return rows
  }

  // The instance side, then the class side, each by selector (SPEC §3.9 Change Sorter).
  private func loadMethods(setID: Int64, className: String) -> [ChangeMethodRow] {
    var rows: [ChangeMethodRow] = []
    for meta in [false, true] {
      let side: Int32 = meta ? 1 : 0
      let count = className.withCString { ao_changes_method_count(setID, $0, side) }
      guard count > 0 else {
        continue
      }
      for index in 0..<count {
        var kind: Int32 = 0
        let selector = className.withCString { cls in
          copyChangeText { ao_changes_method_at(setID, cls, side, index, $0, $1, &kind) }
        }
        guard let selector else {
          return []
        }
        rows.append(ChangeMethodRow(
          selector: selector, meta: meta, kind: ChangeKind(rawValue: kind) ?? ChangeKind.none))
      }
    }
    return rows
  }

  // SPEC §3.9 Change Sorter のソース枠.
  private func loadSource() -> String {
    guard let className = selectedClass else {
      return ""
    }
    let classID = BrowserModel.classID(named: className)
    if let method = selectedMethod {
      if method.kind == .removed {
        return "removed"
      }
      let placeholder =
        "\"\(className)\(method.meta ? " class" : "")>>\(method.selector) source not available\""
      guard classID != 0 else {
        return placeholder
      }
      return copyChangeText(reportingNoSource: true) {
        ao_browser_source(classID, method.meta ? 1 : 0, method.selector, $0, $1)
      } ?? placeholder
    }
    if classes.first(where: { $0.name == className })?.kind == .removed {
      return "removed"
    }
    guard classID != 0 else {
      return "\"\(className) definition not available\""
    }
    return copyChangeText { ao_browser_class_definition(classID, $0, $1) } ?? ""
  }
}

// Calls body with method's selector as a C string, or with nil for no method.
private func withSelector<R>(_ method: ChangeMethodRow?, _ body: (UnsafePointer<CChar>?) -> R) -> R {
  guard let method else {
    return body(nil)
  }
  return method.selector.withCString { body($0) }
}

// SPEC §3.10 の文字列バッファ: reads text, doubling the buffer while the read answers AO_ERR_RANGE.
// With reportingNoSource an AO_ERR_NOSOURCE placeholder is text too (a cut one asks for more room).
// nil on any other failure.
private func copyChangeText(
  reportingNoSource: Bool = false,
  _ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32
) -> String? {
  var capacity = 256
  while capacity <= 1_048_576 {
    var buffer = [CChar](repeating: 0, count: capacity)
    let rc = buffer.withUnsafeMutableBufferPointer { pointer -> Int32 in
      guard let base = pointer.baseAddress else {
        return Int32(AO_ERR)
      }
      return read(base, Int32(capacity))
    }
    let text = buffer.withUnsafeBufferPointer { pointer in
      pointer.baseAddress.map { String(cString: $0) } ?? ""
    }
    if rc == Int32(AO_OK) {
      return text
    }
    let noSource = reportingNoSource && rc == Int32(AO_ERR_NOSOURCE)
    if noSource && text.utf8.count < capacity - 1 {
      return text
    }
    if !noSource && rc != Int32(AO_ERR_RANGE) {
      return nil
    }
    capacity *= 2
  }
  return nil
}
```

- [ ] **Step 4: テストが通ることを確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter ChangeSorterTests`
Expected: PASS（5 件）。Swift 6 の並行性エラー（クロージャの中の `&id` / `&err`）が出たら、`BrowserModel.copyClass` と同じく `withUnsafeMutablePointer(to:)` で包む。

- [ ] **Step 5: 外形を取ってコミット**

Serena の `get_symbols_overview` を `app/Ao/ChangeSorterModel.swift` に取る。

```bash
git add app/Ao/ChangeSorterModel.swift app/AoTests/ChangeSorterTests.swift
git commit -m "Add the Change Sorter model (P16)" -m "ChangeSorterModel reads the sets, classes and methods through ao_changes_*,
keeps the selection, and shows sources and definitions through the Browser
reads.

Graphify: query BrowserModel ao_changes_count
Serena: get_symbols_overview app/Ao/ChangeSorterModel.swift

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 8: Change Sorter の窓

**Files:**
- Create: `app/Ao/ChangeSorterWindow.swift`
- Test: `app/AoTests/ChangeSorterTests.swift`

**Interfaces:**
- Consumes: Task 7 の `ChangeSorterModel`、`fileOutChangeSet`。`ToolTextSize.font(fixedPitch:)`、`UniformFont`。
- Produces: `@MainActor final class ChangeSorterWindow`（`model`、`window`、`askName`、`chooseFileOut`、`sourceText`、`errorText`、`setTitles`、`classTitles`、`methodTitles`、`canMakeCurrent`、`paneAccessibilityLabels`、`reload()`、`noteImageLoaded()`、`applyFont()`、`selectRows(set:class:method:)`、`newSet()`、`makeCurrent()`、`fileOut()`、`removeEntry(className:method:)`、`moveEntry(className:method:to:)`、`entryMenu(className:method:) -> NSMenu`、`windowDidBecomeKey(_:)`）。

- [ ] **Step 1: 失敗するテストを書く**

`ChangeSorterTests` の末尾（クラスの閉じ括弧の前）に足す。

```swift
  // SPEC §3.9 Change Sorter: the current set's row starts with `• `, and the subtitle names it.
  func testWindowListsSetsWithTheCurrentMarkAndSubtitle() {
    boot()
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Second", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.window.title, "Change Sorter")
    XCTAssertEqual(sorter.window.subtitle, "Second is current")
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Second"])
    XCTAssertFalse(sorter.canMakeCurrent)
  }

  // SPEC §3.9 Change Sorter: the lists carry the marks; the source pane follows the selection.
  func testSelectingAClassAndMethodShowsMarksAndSource() {
    boot()
    acceptClass("P16Win")
    acceptMethod("P16Win", "a\n  ^1")
    acceptMethod("P16Win", meta: true, "make\n  ^self new")
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    XCTAssertEqual(sorter.classTitles, ["P16Win +"])
    XCTAssertEqual(sorter.methodTitles, ["a +", "class » make +"])
    XCTAssertTrue(sorter.sourceText.hasPrefix("Object subclass: #P16Win"), sorter.sourceText)
    sorter.selectRows(set: 0, class: 0, method: 1)
    XCTAssertEqual(sorter.sourceText, "make\n  ^self new")
  }

  // SPEC §3.9 Change Sorter: New… offers the next free name; the new set is current and selected; a
  // refusal goes to the error field; Cancel changes nothing.
  func testNewButtonAsksForTheNextFreeNameAndSelectsTheNewCurrentSet() {
    boot()
    let sorter = ChangeSorterWindow()
    var asked: String?
    sorter.askName = { _, suggested, answer in
      asked = suggested
      answer("Fixes")
    }
    sorter.newSet()
    XCTAssertEqual(asked, "Unnamed2")
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Fixes"])
    XCTAssertEqual(sorter.window.subtitle, "Fixes is current")
    XCTAssertEqual(sorter.model.selectedSetName, "Fixes")
    XCTAssertEqual(sorter.errorText, "")
    sorter.askName = { _, _, answer in answer("Fixes") }
    sorter.newSet()
    XCTAssertEqual(sorter.errorText, "change set name taken: Fixes")
    sorter.askName = { _, _, answer in answer(nil) }
    sorter.newSet()
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Fixes"])
  }

  // SPEC §3.9 Change Sorter: Make Current moves the mark and then is disabled.
  func testMakeCurrentMovesTheMark() {
    boot()
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Second", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0)
    XCTAssertTrue(sorter.canMakeCurrent)
    sorter.makeCurrent()
    XCTAssertEqual(sorter.setTitles, ["• Unnamed1", "Second"])
    XCTAssertFalse(sorter.canMakeCurrent)
  }

  // SPEC §3.9 Change Sorter: the entry menu is Remove from Change Set, a separator and Move to with
  // the other sets; without an entry its items are disabled. Move to moves the entry.
  func testEntryMenuOffersRemoveAndMoveToTheOtherSets() {
    boot()
    acceptClass("P16Menu")
    acceptMethod("P16Menu", "a\n  ^1")
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Other", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    let menu = sorter.entryMenu(className: "P16Menu", method: sorter.model.methods.first)
    XCTAssertEqual(menu.items.map(\.title), ["Remove from Change Set", "", "Move to"])
    XCTAssertEqual(menu.items[2].submenu?.items.map(\.title), ["Other"])
    XCTAssertTrue(menu.items[0].isEnabled)
    XCTAssertTrue(menu.items[2].isEnabled)
    let empty = sorter.entryMenu(className: nil, method: nil)
    XCTAssertFalse(empty.items[0].isEnabled)
    XCTAssertFalse(empty.items[2].isEnabled)
    guard let moveItem = menu.items[2].submenu?.items.first, let action = moveItem.action else {
      XCTFail("missing Move to item")
      return
    }
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: moveItem.target, from: moveItem))
    XCTAssertEqual(sorter.methodTitles, [])
    sorter.selectRows(set: 1, class: 0)
    XCTAssertEqual(sorter.classTitles, ["P16Menu"])
    XCTAssertEqual(sorter.methodTitles, ["a +"])
  }

  // SPEC §3.9 Change Sorter: Remove from Change Set on a class row forgets the entry; the class stays.
  func testRemoveFromMenuForgetsTheClassEntryButKeepsTheClass() {
    boot()
    acceptClass("P16Forget")
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    let menu = sorter.entryMenu(className: "P16Forget", method: nil)
    guard let action = menu.items[0].action else {
      XCTFail("missing Remove from Change Set")
      return
    }
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: menu.items[0].target, from: menu.items[0]))
    XCTAssertEqual(sorter.classTitles, [])
    XCTAssertNotEqual(BrowserModel.classID(named: "P16Forget"), 0)
  }

  // SPEC §3.9 Change Sorter: File Out… offers `<set>.st` and writes the selected set.
  func testFileOutButtonWritesTheSelectedSet() {
    boot()
    acceptClass("P16Out")
    acceptMethod("P16Out", "a\n  ^1")
    let sorter = ChangeSorterWindow()
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p16-\(UUID().uuidString).st")
    defer { try? FileManager.default.removeItem(at: url) }
    var suggested: String?
    sorter.chooseFileOut = { _, name, answer in
      suggested = name
      answer(url)
    }
    sorter.fileOut()
    XCTAssertEqual(suggested, "Unnamed1.st")
    let text = (try? String(contentsOf: url, encoding: .utf8)) ?? ""
    XCTAssertTrue(text.hasPrefix("\"Change Set: Unnamed1\n"), text)
    XCTAssertTrue(text.contains("!P16Out methodsFor: 'user'!\na\n  ^1! !\n"), text)
    XCTAssertEqual(sorter.errorText, "")
  }

  // SPEC §3.9 Change Sorter: a failed File Out shows the runtime's reason.
  func testRefusedFileOutShowsTheReason() {
    boot()
    let sorter = ChangeSorterWindow()
    let url = URL(fileURLWithPath: "/nonexistent-ao-p16-dir/x.st")
    sorter.chooseFileOut = { _, _, answer in answer(url) }
    sorter.fileOut()
    XCTAssertEqual(sorter.errorText, "cannot write file: \(url.path)")
  }

  // Review Focus 3: an Accept made while the window is open shows when the window becomes key.
  func testBecomingKeyReadsTheSetsAgain() {
    boot()
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.classTitles, [])
    acceptClass("P16Later")
    sorter.windowDidBecomeKey(Notification(name: NSWindow.didBecomeKeyNotification, object: sorter.window))
    XCTAssertEqual(sorter.classTitles, ["P16Later +"])
  }

  // Review Focus 4: after a load the window shows the new session's one set.
  func testImageLoadStartsTheWindowOverWithUnnamed1() {
    boot()
    let sorter = ChangeSorterWindow()
    sorter.askName = { _, _, answer in answer("Before") }
    sorter.newSet()
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p16-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(ao_image_save(url.path), Int32(AO_OK))
    var err = AoSpan()
    XCTAssertEqual(ao_image_load(url.path, &err), Int32(AO_OK), spanMessage(err))
    sorter.noteImageLoaded()
    XCTAssertEqual(sorter.setTitles, ["• Unnamed1"])
    XCTAssertEqual(sorter.window.subtitle, "Unnamed1 is current")
  }

  // SPEC §3.9 Change Sorter: the VoiceOver labels.
  func testPaneAccessibilityLabels() {
    boot()
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.paneAccessibilityLabels, ["Change sets", "Classes", "Methods", "Source", "Error"])
  }
```

- [ ] **Step 2: 失敗を確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter ChangeSorterTests`
Expected: FAIL（`cannot find 'ChangeSorterWindow' in scope`）。

- [ ] **Step 3: `app/Ao/ChangeSorterWindow.swift` を作る**

```swift
import AppKit
import CAo

// SPEC §3.9 Change Sorter (P16): the change sets, the selected set's classes, the selected class's
// methods, and a read-only source pane. isVertical is a vertical divider, so side-by-side lists.
@MainActor
final class ChangeSorterWindow: NSObject, NSTableViewDataSource, NSTableViewDelegate, NSMenuDelegate,
  NSWindowDelegate {
  let model = ChangeSorterModel()
  let window: NSWindow

  private let setTable = NSTableView()
  private let classTable = NSTableView()
  private let methodTable = NSTableView()
  private let sourceView: NSTextView
  private let uniformFont = UniformFont()
  private let errorField: NSTextField
  private let newButton: NSButton
  private let currentButton: NSButton
  private let fileOutButton: NSButton
  private var applying = false

  // Asked for the new set's name, with the suggested one; the callback gets nil for Cancel. Tests
  // replace it.
  typealias NameQuestion = @MainActor (NSWindow, String, @escaping @MainActor (String?) -> Void) -> Void
  var askName: NameQuestion = ChangeSorterWindow.askForName

  // Asked where File Out… writes, with the suggested file name; the callback gets nil for Cancel.
  // Tests replace it.
  typealias FileOutQuestion = @MainActor (NSWindow, String, @escaping @MainActor (URL?) -> Void) -> Void
  var chooseFileOut: FileOutQuestion = ChangeSorterWindow.askForFileOutURL

  var sourceText: String {
    sourceView.string
  }

  var errorText: String {
    errorField.stringValue
  }

  var setTitles: [String] {
    rowTitles(for: setTable)
  }

  var classTitles: [String] {
    rowTitles(for: classTable)
  }

  var methodTitles: [String] {
    rowTitles(for: methodTable)
  }

  var canMakeCurrent: Bool {
    currentButton.isEnabled
  }

  var paneAccessibilityLabels: [String?] {
    [
      setTable.accessibilityLabel(),
      classTable.accessibilityLabel(),
      methodTable.accessibilityLabel(),
      sourceView.accessibilityLabel(),
      errorField.accessibilityLabel(),
    ]
  }

  override init() {
    _ = NSApplication.shared
    let frame = NSRect(x: 260, y: 140, width: 760, height: 520)
    let footer: CGFloat = 36
    // NSSplitView keeps the current size ratio, so a zero frame stays collapsed.
    let band = NSRect(x: 0, y: 0, width: frame.width, height: (frame.height - footer) / 2)
    let column = NSRect(x: 0, y: 0, width: frame.width / 3, height: band.height)
    let window = NSWindow(
      contentRect: frame,
      styleMask: [.titled, .closable, .miniaturizable, .resizable],
      backing: .buffered,
      defer: false
    )
    window.title = "Change Sorter"
    window.isReleasedWhenClosed = false
    window.isRestorable = false
    let source = ChangeSorterWindow.makeSource(frame: band)
    let errorField = NSTextField(labelWithString: "")
    errorField.font = NSFont.systemFont(ofSize: NSFont.smallSystemFontSize)
    errorField.textColor = .secondaryLabelColor
    errorField.lineBreakMode = .byTruncatingTail

    self.window = window
    self.sourceView = source.text
    self.errorField = errorField
    self.newButton = NSButton(title: "New…", target: nil, action: nil)
    self.currentButton = NSButton(title: "Make Current", target: nil, action: nil)
    self.fileOutButton = NSButton(title: "File Out…", target: nil, action: nil)
    super.init()

    configure(setTable)
    configure(classTable)
    configure(methodTable)
    classTable.menu = entryContextMenu()
    methodTable.menu = entryContextMenu()
    setTable.setAccessibilityLabel("Change sets")
    classTable.setAccessibilityLabel("Classes")
    methodTable.setAccessibilityLabel("Methods")
    sourceView.setAccessibilityLabel("Source")
    errorField.setAccessibilityLabel("Error")
    newButton.target = self
    newButton.action = #selector(newSetPressed(_:))
    currentButton.target = self
    currentButton.action = #selector(makeCurrentPressed(_:))
    fileOutButton.target = self
    fileOutButton.action = #selector(fileOutPressed(_:))

    let lists = NSSplitView(frame: band)
    lists.isVertical = true
    lists.dividerStyle = .thin
    for table in [setTable, classTable, methodTable] {
      let pane = scroll(for: table, frame: column)
      lists.addArrangedSubview(pane)
      pane.frame = column
    }
    let outer = NSSplitView(
      frame: NSRect(x: 0, y: footer, width: frame.width, height: frame.height - footer))
    outer.isVertical = false
    outer.dividerStyle = .thin
    outer.autoresizingMask = [.width, .height]
    outer.addArrangedSubview(lists)
    outer.addArrangedSubview(source.scroll)
    lists.frame = band
    source.scroll.frame = band

    let content = NSView(frame: NSRect(origin: .zero, size: frame.size))
    content.addSubview(outer)
    var right = frame.width - 8
    for button in [fileOutButton, currentButton, newButton] {
      button.sizeToFit()
      right -= button.frame.width
      button.frame.origin = NSPoint(x: right, y: 6)
      button.autoresizingMask = [.minXMargin, .maxYMargin]
      content.addSubview(button)
      right -= 8
    }
    errorField.frame = NSRect(x: 8, y: 10, width: max(right - 16, 1), height: 16)
    errorField.autoresizingMask = [.width, .maxYMargin]
    content.addSubview(errorField)
    window.contentView = content
    window.delegate = self

    applyFont()
    reload()
    window.makeKeyAndOrderFront(nil)
  }

  // SPEC §3.9 Change Sorter: reads the runtime again and shows it.
  func reload() {
    model.refresh()
    show()
  }

  // SPEC §3.9 Change Sorter: a load started the sets over; the current set is selected.
  func noteImageLoaded() {
    model.select(setID: nil)
    errorField.stringValue = ""
    show()
  }

  func applyFont() {
    guard let font = ToolTextSize.font(fixedPitch: false) else {
      return
    }
    uniformFont.apply(font, to: sourceView)
  }

  func windowDidBecomeKey(_ notification: Notification) {
    reload()
  }

  // Selects rows as clicks would.
  func selectRows(set: Int, class classRow: Int? = nil, method: Int? = nil) {
    let setID = value(at: set, in: model.sets)?.id
    model.select(setID: setID)
    let className = classRow.flatMap { value(at: $0, in: model.classes)?.name }
    model.select(setID: setID, className: className)
    model.select(
      setID: setID,
      className: className,
      method: method.flatMap { value(at: $0, in: model.methods) }
    )
    show()
  }

  func newSet() {
    askName(window, model.nextFreeName()) { [weak self] name in
      guard let self, let name else {
        return
      }
      self.report(self.model.newSet(named: name))
      self.show()
    }
  }

  func makeCurrent() {
    guard let id = model.selectedSetID else {
      return
    }
    let status = model.makeCurrent(id)
    report((status, "Make Current failed"))
    show()
  }

  func fileOut() {
    guard let id = model.selectedSetID, let name = model.selectedSetName else {
      return
    }
    chooseFileOut(window, name + ".st") { [weak self] url in
      guard let self, let url else {
        return
      }
      self.report(fileOutChangeSet(id, to: url))
    }
  }

  func removeEntry(className: String, method: ChangeMethodRow?) {
    report(model.remove(className: className, method: method))
    show()
  }

  func moveEntry(className: String, method: ChangeMethodRow?, to setID: Int64) {
    report(model.move(className: className, method: method, to: setID))
    show()
  }

  // SPEC §3.9 Change Sorter: Remove from Change Set, a separator, and Move to ▸ the other sets.
  // Without an entry, the items are disabled; without another set, so is Move to.
  func entryMenu(className: String?, method: ChangeMethodRow?) -> NSMenu {
    let menu = NSMenu(title: "Entry")
    fill(menu, className: className, method: method)
    return menu
  }

  // MARK: NSTableViewDataSource, NSTableViewDelegate

  func numberOfRows(in tableView: NSTableView) -> Int {
    rowTitles(for: tableView).count
  }

  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
    guard let text = value(at: row, in: rowTitles(for: tableView)) else {
      return nil
    }
    let identifier = NSUserInterfaceItemIdentifier("name")
    let cell = tableView.makeView(withIdentifier: identifier, owner: self) as? NSTextField
      ?? NSTextField(labelWithString: "")
    cell.identifier = identifier
    cell.stringValue = text
    cell.isEditable = false
    cell.isBordered = false
    cell.drawsBackground = false
    cell.lineBreakMode = .byTruncatingTail
    return cell
  }

  func tableViewSelectionDidChange(_ notification: Notification) {
    guard !applying, let table = notification.object as? NSTableView else {
      return
    }
    let setID = value(at: setTable.selectedRow, in: model.sets)?.id
    if table === setTable {
      model.select(setID: setID)
    } else if table === classTable {
      model.select(setID: setID, className: value(at: classTable.selectedRow, in: model.classes)?.name)
    } else {
      model.select(
        setID: setID,
        className: model.selectedClass,
        method: value(at: methodTable.selectedRow, in: model.methods)
      )
    }
    show()
  }

  // MARK: NSMenuDelegate

  // SPEC §3.9 Change Sorter: the right-clicked row's entry; nothing outside a row.
  func menuNeedsUpdate(_ menu: NSMenu) {
    if menu === classTable.menu {
      fill(menu, className: value(at: classTable.clickedRow, in: model.classes)?.name, method: nil)
      return
    }
    let method = value(at: methodTable.clickedRow, in: model.methods)
    fill(menu, className: method == nil ? nil : model.selectedClass, method: method)
  }

  // MARK: private

  private func show() {
    applying = true
    defer { applying = false }
    setTable.reloadData()
    classTable.reloadData()
    methodTable.reloadData()
    selectRow(model.sets.firstIndex { $0.id == model.selectedSetID }, in: setTable)
    selectRow(model.classes.firstIndex { $0.name == model.selectedClass }, in: classTable)
    selectRow(model.methods.firstIndex { $0 == model.selectedMethod }, in: methodTable)
    sourceView.string = model.source
    window.subtitle = model.currentName.map { "\($0) is current" } ?? ""
    currentButton.isEnabled = model.selectedSetID != nil && model.selectedSetID != model.currentID
    fileOutButton.isEnabled = model.selectedSetID != nil
  }

  private func report(_ result: (status: Int32, message: String)) {
    errorField.stringValue = result.status == Int32(AO_OK) ? "" : result.message
  }

  private func rowTitles(for table: NSTableView) -> [String] {
    if table === setTable {
      return model.sets.map { ($0.id == model.currentID ? "• " : "") + $0.name }
    }
    if table === classTable {
      return model.classes.map { titled($0.name, $0.kind) }
    }
    return model.methods.map { titled($0.label, $0.kind) }
  }

  private func titled(_ name: String, _ kind: ChangeKind) -> String {
    kind.mark.isEmpty ? name : "\(name) \(kind.mark)"
  }

  private func selectRow(_ row: Int?, in table: NSTableView) {
    if let row {
      table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    } else {
      table.deselectAll(nil)
    }
  }

  private func entryContextMenu() -> NSMenu {
    let menu = NSMenu(title: "Entry")
    menu.delegate = self
    return menu
  }

  private func fill(_ menu: NSMenu, className: String?, method: ChangeMethodRow?) {
    menu.removeAllItems()
    menu.autoenablesItems = false
    let remove = NSMenuItem(
      title: "Remove from Change Set", action: #selector(removeFromMenu(_:)), keyEquivalent: "")
    remove.target = self
    remove.isEnabled = className != nil
    remove.representedObject = className.map { EntryTarget(className: $0, method: method, setID: 0) }
    menu.addItem(remove)
    menu.addItem(.separator())
    let targets = NSMenu(title: "Move to")
    targets.autoenablesItems = false
    for set in model.sets where set.id != model.selectedSetID {
      let item = NSMenuItem(title: set.name, action: #selector(moveFromMenu(_:)), keyEquivalent: "")
      item.target = self
      item.isEnabled = className != nil
      item.representedObject = className.map { EntryTarget(className: $0, method: method, setID: set.id) }
      targets.addItem(item)
    }
    let move = NSMenuItem(title: "Move to", action: nil, keyEquivalent: "")
    move.submenu = targets
    move.isEnabled = className != nil && !targets.items.isEmpty
    menu.addItem(move)
  }

  @objc private func removeFromMenu(_ sender: NSMenuItem) {
    guard let target = sender.representedObject as? EntryTarget else {
      return
    }
    removeEntry(className: target.className, method: target.method)
  }

  @objc private func moveFromMenu(_ sender: NSMenuItem) {
    guard let target = sender.representedObject as? EntryTarget else {
      return
    }
    moveEntry(className: target.className, method: target.method, to: target.setID)
  }

  @objc private func newSetPressed(_ sender: Any?) {
    newSet()
  }

  @objc private func makeCurrentPressed(_ sender: Any?) {
    makeCurrent()
  }

  @objc private func fileOutPressed(_ sender: Any?) {
    fileOut()
  }

  private func value<Element>(at row: Int, in values: [Element]) -> Element? {
    guard row >= 0, row < values.count else {
      return nil
    }
    return values[row]
  }

  private func configure(_ table: NSTableView) {
    let column = NSTableColumn(identifier: NSUserInterfaceItemIdentifier("name"))
    column.resizingMask = .autoresizingMask
    column.width = 240
    table.addTableColumn(column)
    table.headerView = nil
    table.allowsEmptySelection = true
    table.allowsMultipleSelection = false
    table.columnAutoresizingStyle = .lastColumnOnlyAutoresizingStyle
    table.dataSource = self
    table.delegate = self
  }

  private func scroll(for table: NSTableView, frame: NSRect) -> NSScrollView {
    let scroll = NSScrollView(frame: frame)
    scroll.hasVerticalScroller = true
    scroll.hasHorizontalScroller = false
    scroll.borderType = .noBorder
    scroll.autoresizingMask = [.width, .height]
    let contentSize = scroll.contentSize
    let width = contentSize.width > 0 ? contentSize.width : frame.width
    let height = contentSize.height > 0 ? contentSize.height : frame.height
    table.frame = NSRect(x: 0, y: 0, width: width, height: height)
    table.autoresizingMask = [.width]
    scroll.documentView = table
    return scroll
  }

  private static func makeSource(frame: NSRect) -> (scroll: NSScrollView, text: NSTextView) {
    let scroll = NSScrollView(frame: frame)
    scroll.hasVerticalScroller = true
    scroll.hasHorizontalScroller = false
    scroll.borderType = .noBorder
    scroll.autoresizingMask = [.width, .height]
    let contentSize = scroll.contentSize
    let width = contentSize.width > 0 ? contentSize.width : frame.width
    let height = contentSize.height > 0 ? contentSize.height : frame.height
    let text = NSTextView(frame: NSRect(x: 0, y: 0, width: width, height: height))
    text.minSize = NSSize(width: 0, height: height)
    text.maxSize = NSSize(width: CGFloat.greatestFiniteMagnitude, height: CGFloat.greatestFiniteMagnitude)
    text.isVerticallyResizable = true
    text.isHorizontallyResizable = false
    text.autoresizingMask = [.width]
    text.isEditable = false
    text.isSelectable = true
    text.isRichText = false
    if let container = text.textContainer {
      container.containerSize = NSSize(width: width, height: CGFloat.greatestFiniteMagnitude)
      container.widthTracksTextView = true
    }
    scroll.documentView = text
    return (scroll, text)
  }

  // SPEC §3.9 Change Sorter: `New Change Set`, a name field, Create and Cancel.
  static func askForName(
    _ window: NSWindow,
    _ suggested: String,
    _ answer: @escaping @MainActor (String?) -> Void
  ) {
    let alert = NSAlert()
    alert.messageText = "New Change Set"
    let field = NSTextField(string: suggested)
    field.frame = NSRect(x: 0, y: 0, width: 240, height: 24)
    field.setAccessibilityLabel("Name")
    alert.accessoryView = field
    alert.addButton(withTitle: "Create")
    alert.addButton(withTitle: "Cancel")
    alert.window.initialFirstResponder = field
    alert.beginSheetModal(for: window) { response in
      MainActor.assumeIsolated {
        answer(response == .alertFirstButtonReturn ? field.stringValue : nil)
      }
    }
  }

  static func askForFileOutURL(
    _ window: NSWindow,
    _ name: String,
    _ answer: @escaping @MainActor (URL?) -> Void
  ) {
    let panel = NSSavePanel()
    panel.canCreateDirectories = true
    panel.nameFieldStringValue = name
    panel.beginSheetModal(for: window) { response in
      MainActor.assumeIsolated {
        answer(response == .OK ? panel.url : nil)
      }
    }
  }
}

// A context menu item's entry and, for Move to, its target set.
private final class EntryTarget: NSObject {
  let className: String
  let method: ChangeMethodRow?
  let setID: Int64

  init(className: String, method: ChangeMethodRow?, setID: Int64) {
    self.className = className
    self.method = method
    self.setID = setID
  }
}
```

`makeCurrent` は失敗のときだけ `report` がメッセージを出す（成功ならエラー欄は空になる）。

- [ ] **Step 4: テストが通ることを確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter ChangeSorterTests`
Expected: PASS（16 件）。`NSMenuItem.separator()` の `title` が空でない環境なら、`entryMenu` のテストは `menu.items[1].isSeparatorItem` で確かめる形に直す（期待値を実装に合わせて変えない。区切りであることを確かめる）。

- [ ] **Step 5: 外形を取ってコミット**

Serena の `get_symbols_overview` を `app/Ao/ChangeSorterWindow.swift` に取る。

```bash
git add app/Ao/ChangeSorterWindow.swift app/AoTests/ChangeSorterTests.swift
git commit -m "Add the Change Sorter window (P16)" -m "Three lists, a read-only source pane, New..., Make Current, File Out..., and
an entry menu with Remove from Change Set and Move to. The window reads the
runtime again whenever it becomes key.

Graphify: query BrowserWindow ChangeSorterWindow
Serena: get_symbols_overview app/Ao/ChangeSorterWindow.swift

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 9: メニューと AoApp

**Files:**
- Modify: `app/Ao/MainMenu.swift`（`Actions` の 3 項目、File と Tools の項目）
- Modify: `app/Ao/AoApp.swift`（`fileInChangeFile`、`changeSorter`、File In… / File Out Change Set… / Change Sorter、ロードと文字の大きさ）
- Modify: `app/AoTests/AcceptTests.swift`（File メニューの並び）
- Test: `app/AoTests/ChangeSorterTests.swift`

**Interfaces:**
- Consumes: Task 7 の `fileOutChangeSet`、`ChangeSorterModel`。Task 8 の `ChangeSorterWindow`（`reload`、`noteImageLoaded`、`applyFont`、`setTitles`）。
- Produces: `MainMenu.Actions.fileIn`、`.fileOutChangeSet`、`.showChangeSorter`。`@MainActor func fileInChangeFile(at url: URL, transcript: TranscriptWindow?) -> Int32`。`AoApp.changeSorter`（`private(set)`）、`AoApp.fileIn(from:)`、`AoApp.fileOutCurrentChangeSet(to:) -> Bool`。

- [ ] **Step 1: 失敗するテストを書く**

`AcceptTests.testFileMenuImageCommandsAndShowHierarchy` の期待値を次にする。

```swift
      ["Save Image…", "Open Image…", "", "File In…", "File Out Change Set…", "", "Close"]
```

`ChangeSorterTests` の末尾に足す。

```swift
  private func launchApp() -> AoApp {
    let app = AoApp()
    app.applicationWillFinishLaunching(
      Notification(name: NSApplication.willFinishLaunchingNotification, object: NSApplication.shared)
    )
    return app
  }

  private func sendToolsItem(_ title: String) -> Bool {
    guard let item = NSApplication.shared.mainMenu?.item(withTitle: "Tools")?.submenu?.item(withTitle: title),
          let action = item.action else {
      return false
    }
    return NSApplication.shared.sendAction(action, to: item.target, from: item)
  }

  private func temporaryFile(_ text: String, ext: String = "st") -> URL {
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("ao-p16-\(UUID().uuidString).\(ext)")
    XCTAssertNoThrow(try text.write(to: url, atomically: true, encoding: .utf8))
    return url
  }

  // SPEC §3.9 Change Sorter: Tools → Change Sorter opens one window, however often it is chosen.
  func testToolsChangeSorterOpensOneWindow() {
    _ = launchApp()
    let sorters = { NSApplication.shared.windows.filter { $0.title == "Change Sorter" && $0.isVisible } }
    XCTAssertEqual(sorters().count, 0)
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    XCTAssertEqual(sorters().count, 1)
  }

  // SPEC §3.9 File In…: `filed in: <path>`, and the definitions and methods are recorded.
  func testFileInWritesTheFiledInLineAndRecords() {
    let launch = LaunchSet.make()
    let url = temporaryFile(
      "Object subclass: #P16FileIn\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'P16-In'!\n\n!P16FileIn methodsFor: 'user'!\nx\n  ^1! !\n")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(fileInChangeFile(at: url, transcript: launch.transcript), Int32(AO_OK))
    XCTAssertEqual(launch.transcript.text, "filed in: \(url.path)\n")
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16FileIn", kind: .added)])
  }

  // SPEC §3.9 File In…: `file-in failed: <path>` and the first error in the `ao filein` form.
  func testFailedFileInWritesTheFailedLineAndTheFirstError() {
    let launch = LaunchSet.make()
    let header = "P16Acount methodsFor: 'user'"
    let url = temporaryFile("!\(header)!\nx\n  ^1! !\n")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(fileInChangeFile(at: url, transcript: launch.transcript), Int32(AO_ERR))
    let end = 1 + header.utf8.count
    XCTAssertEqual(
      launch.transcript.text,
      "file-in failed: \(url.path)\n\(url.path):1-\(end): missing class: P16Acount\n"
    )
  }

  // SPEC §3.9 File Out Change Set…: a failure is an alert naming the file and the reason.
  func testFileOutChangeSetFailureShowsAnAlert() {
    let app = launchApp()
    var alerts: [NSAlert] = []
    app.presentAlert = { alerts.append($0) }
    let url = URL(fileURLWithPath: "/nonexistent-ao-p16-dir/x.st")
    XCTAssertFalse(app.fileOutCurrentChangeSet(to: url))
    XCTAssertEqual(alerts.first?.messageText, "Could not file out the change set")
    XCTAssertEqual(alerts.first?.informativeText, url.path + "\ncannot write file: " + url.path)
  }

  // Review Focus 4: Open Image… starts an open Change Sorter over.
  func testOpenImageStartsAnOpenChangeSorterOver() {
    let app = launchApp()
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Before", nil, &err), Int32(AO_OK))
    app.changeSorter?.reload()
    XCTAssertEqual(app.changeSorter?.setTitles, ["Unnamed1", "• Before"])
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("ao-p16-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertTrue(app.saveImage(to: url))
    XCTAssertTrue(app.openImage(from: url))
    XCTAssertEqual(app.changeSorter?.setTitles, ["• Unnamed1"])
  }
```

`MainMenu` の File の並びは `AcceptTests` が見るので、`ChangeSorterTests` では Tools の項目だけを見る。

- [ ] **Step 2: 失敗を確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'ChangeSorterTests|AcceptTests/testFileMenuImageCommandsAndShowHierarchy'`
Expected: FAIL（`cannot find 'fileInChangeFile' in scope`、`changeSorter` が無い）。

- [ ] **Step 3: `MainMenu.swift`**

`Actions` の `var openImage: () -> Void = {}` の後ろに足す。

```swift
    // SPEC §3.9 Change Sorter (P16): File In…, File Out Change Set… and Tools → Change Sorter.
    var fileIn: () -> Void = {}
    var fileOutChangeSet: () -> Void = {}
    var showChangeSorter: () -> Void = {}
```

File の項目を次にする。

```swift
    bar.addItem(top("File", items: [
      actionItem("Save Image…", key: "", run: { _ in actions.saveImage() }),
      actionItem("Open Image…", key: "", run: { _ in actions.openImage() }),
      .separator(),
      actionItem("File In…", key: "", run: { _ in actions.fileIn() }),
      actionItem("File Out Change Set…", key: "", run: { _ in actions.fileOutChangeSet() }),
      .separator(),
      responderItem("Close", key: "w", action: #selector(NSWindow.performClose(_:)))
    ]))
```

Tools の `actionItem("Workspace", …)` の後ろに足す。

```swift
      actionItem("Change Sorter", key: "", run: { _ in actions.showChangeSorter() }),
```

- [ ] **Step 4: `AoApp.swift`**

`fileInVendor` の後ろに足す。

```swift
/// SPEC §3.9 File In… (P16): files in one .st file (ao_filein_file) and writes the result to the
/// Transcript: `filed in: <path>`, or `file-in failed: <path>` and the first error as
/// `<path>:<start>-<end>: <message>`, the line form of `ao filein`.
@MainActor
func fileInChangeFile(at url: URL, transcript: TranscriptWindow?) -> Int32 {
  var err = AoSpan()
  let status = url.path.withCString { ao_filein_file($0, &err) }
  if status == Int32(AO_OK) {
    transcript?.append("filed in: \(url.path)\n")
  } else {
    transcript?.append(
      "file-in failed: \(url.path)\n\(url.path):\(err.start)-\(err.end): \(spanMessage(err))\n")
  }
  return status
}
```

`AoApp` の `private var browser: BrowserWindow?` の後ろに足す。

```swift
  private(set) var changeSorter: ChangeSorterWindow?
```

`MainMenu.Actions(...)` の `openImage: { self.presentOpenImage() },` の後ろに足す（メンバごとの初期化子は宣言の順に引数を取るので、3 つともここに置く）。

```swift
      fileIn: { self.presentFileIn() },
      fileOutChangeSet: { self.presentFileOutChangeSet() },
      showChangeSorter: { self.showChangeSorter() },
```

`openImage(from:)` の `browser?.noteImageLoaded()` の後ろに `changeSorter?.noteImageLoaded()` を、`resizeText` の `browser?.applyFont()` の後ろに `changeSorter?.applyFont()` を足す。

`presentOpenImage()` の後ろに足す。

```swift
  // SPEC §3.9 File In…: the Transcript gets the result; an open Change Sorter reads the records.
  func fileIn(from url: URL) {
    _ = fileInChangeFile(at: url, transcript: launch?.transcript)
    changeSorter?.reload()
  }

  // SPEC §3.9 File Out Change Set…: the current set. A failure is an alert naming the file and the
  // reason.
  func fileOutCurrentChangeSet(to url: URL) -> Bool {
    let result = fileOutChangeSet(ao_changes_current(), to: url)
    guard result.status == Int32(AO_OK) else {
      reportImageFailure("Could not file out the change set", url: url, reason: result.message)
      return false
    }
    return true
  }

  private func presentFileIn() {
    let panel = NSOpenPanel()
    panel.canChooseFiles = true
    panel.canChooseDirectories = false
    panel.allowsMultipleSelection = false
    panel.begin { response in
      MainActor.assumeIsolated {
        guard response == .OK, let url = panel.url else {
          return
        }
        self.fileIn(from: url)
      }
    }
  }

  private func presentFileOutChangeSet() {
    let model = ChangeSorterModel()
    model.refresh()
    let panel = NSSavePanel()
    panel.canCreateDirectories = true
    panel.nameFieldStringValue = (model.currentName ?? "Unnamed1") + ".st"
    panel.begin { response in
      MainActor.assumeIsolated {
        guard response == .OK, let url = panel.url else {
          return
        }
        _ = self.fileOutCurrentChangeSet(to: url)
      }
    }
  }

  private func showChangeSorter() {
    if changeSorter == nil {
      changeSorter = ChangeSorterWindow()
    }
    changeSorter?.reload()
    changeSorter?.window.makeKeyAndOrderFront(nil)
  }
```

`reportImageFailure` は名前が画像寄りだが、アラートの形が同じなので使い回す（改名はしない）。

- [ ] **Step 5: テストが通ることを確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'ChangeSorterTests|AcceptTests|ToolWindowTests'`
Expected: PASS（`ChangeSorterTests` は 21 件。`ToolWindowTests` の Tools の並びのテストは、末尾の 3 項目だけを見るので変わらない）。

- [ ] **Step 6: コミット**

```bash
git add app/Ao/MainMenu.swift app/Ao/AoApp.swift app/AoTests/AcceptTests.swift app/AoTests/ChangeSorterTests.swift
git commit -m "Add File In..., File Out Change Set... and Tools > Change Sorter (P16)" -m "File In... writes its result to the Transcript in the ao filein line form.
Open Image... starts an open Change Sorter over, and the text size reaches
its source pane.

Graphify: path AoApp MainMenu
Serena: find_symbol AoApp/applicationWillFinishLaunching

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 10: 完了の記録（版 1.5.0、PHASE、SPEC §6、グラフ）

**Files:**
- Modify: `runtime/src/Runtime.cpp`、`compiler/src/Compiler.cpp`、`runtime/tests/smoke_test.cpp`、`compiler/tests/smoke_test.cpp`（`1.4.0` → `1.5.0`）
- Modify: `PHASE`、`SPEC.md` §6、`CHANGELOG.md`、`docs/phases/P16.md`、`docs/README.md`
- Modify: `graphify-out/`

- [ ] **Step 1: 版のテストを先に直す**

`runtime/tests/smoke_test.cpp` のコメントを `// SPEC §2.4: P16 までを含む版は 1.5.0。リリースの版に `-` 以降を付けない。` に、期待値を `"1.5.0"` に、`compiler/tests/smoke_test.cpp` の期待値を `"1.5.0"` にする。

Run: `cmake --build build && ctest --test-dir build -R 'Smoke' --output-on-failure`
Expected: FAIL（`1.4.0` が返る）。

- [ ] **Step 2: 版を上げる**

`runtime/src/Runtime.cpp` の `"1.4.0"` と `compiler/src/Compiler.cpp` の `"1.4.0"` を `"1.5.0"` にする。

Run: `cmake --build build && ctest --test-dir build -R 'Smoke' --output-on-failure`
Expected: PASS。

- [ ] **Step 3: 全件と ASan**

Run: `./scripts/test.sh && ./scripts/test.sh --asan`
Expected: CTest と `swift test` の全件 PASS（`KernelScan.*`、`ao --test image/tests`、CLI を含む）。ASan で `ChangeSet.cpp` の `std::erase_if` のあとのダングリング参照（`entry` ポインタ）を見る。落ちたら `superpowers:systematic-debugging`。

- [ ] **Step 4: 記録を更新する**

- `PHASE` を `P16` にする。
- SPEC §6「チェンジセット」の項目を、テストと手動確認で確かめたものから `[x]` にする（手動確認が済むまで残りは `[ ]`）。
- `docs/phases/P16.md` の受け入れを同じく `[x]` にする。
- `docs/README.md` の表の P15 の行の後ろに `| P16 チェンジセット | [phases/P16.md](phases/P16.md) | done |` を足し、依存の行の末尾に ` → P16` を、計画書の並びに `P16: [2026-10-11-p16-change-sets.md](superpowers/plans/2026-10-11-p16-change-sets.md)` を足す。
- `CHANGELOG.md` の `## [Unreleased]` の下に足す。

```markdown
### Added

- Change sets (phase P16 of SPEC §2.3): Accepts and removals are recorded in the session's current change set. Tools → Change Sorter lists the sets, their classes and methods with their kinds (`+`, `~`, `−`), makes new sets, makes one current, forgets or moves entries, and files a set out as a chunk-format `.st` file (removals become comment chunks).
- File → File In… loads a `.st` file into the session: the methods keep their sources, an existing class keeps its methods, and everything read is recorded in the current change set. The result goes to the Transcript.
- C ABI: `ao_changes_*` and `ao_filein_file`.

The `.aoimage` format and the `ao` CLI do not change: change sets are not saved in images, and `ao filein` and the vendor file-in keep no sources and record nothing.
```

- [ ] **Step 5: グラフを更新する**

```
/graphify . --update
```

`graphify-out/GRAPH_REPORT.md` で `ChangeSet.cpp`、`ChangeSorterWindow`、`ChangeSorterModel` がコミュニティに入り、`acceptInto` → `noteMethodChange` の辺があることを確かめる。新しい import の循環が無いこと（「Import Cycles」）を確かめる。

- [ ] **Step 6: 手動確認**

`./scripts/package-app.sh && open build/Ao.app` でアプリを作って起動し、`docs/phases/P16.md` の手動確認 1〜6 を行う。済んだものを `[x]` にし、SPEC §6 の残りもそれに合わせる。

- [ ] **Step 7: コミット**

```bash
git add PHASE SPEC.md CHANGELOG.md docs runtime/src/Runtime.cpp compiler/src/Compiler.cpp runtime/tests/smoke_test.cpp compiler/tests/smoke_test.cpp graphify-out
git commit -m "Record P16 completion, bump to 1.5.0, refresh the knowledge graph" -m "Graphify: update . (ChangeSet, ChangeSorterWindow, acceptInto paths)
Serena: get_symbols_overview runtime/src/ChangeSet.cpp

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

- [ ] **Step 8: ブランチの仕上げ**

`superpowers:finishing-a-development-branch` に従う（PR を作るなら `git diff main...HEAD` で全体を見て、本文にテスト計画と `🤖 Generated with [Claude Code](https://claude.com/claude-code)` を入れる）。

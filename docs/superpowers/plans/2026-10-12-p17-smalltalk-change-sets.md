# P17 Smalltalk のチェンジセット — 実装計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** P16 のチェンジセットを C++ のセッションメモリ（`Session::changes`）から Smalltalk のクラス `ChangeSet`（`image/Tools-model/ChangeSet.st`）に移し、イメージに保存されるようにする。ABI と Change Sorter の操作は P16 のまま。

**Architecture:** モデル（種類の合成、拒否の理由、Remove / Move、File Out の本文）はすべて Smalltalk に書く。C++ の `ChangeSet.cpp` は「最外の送信の手当て」と「ABI の値への変換」だけの薄い層になる。記録は P16 と同じ呼び出し位置（`acceptInto`、`applyMethodsFor`、`applyChunkActions`、`removeMethodOf`、`removeClassOf`）から、成功のあとに `ChangeSet class>>note…` を送る。Kernel に足すのはネイティブ `Behavior>>sourceCodeAt:` と `Class>>definition` の 2 つだけ。Tools-model の file-in は、アプリでは vendor のあと、`ao --test`（`runSmalltalkTests`）ではテストの前に行う。

**Tech Stack:** C++20（runtime）、Smalltalk（チャンク形式、Kernel のネイティブだけを使う）、GoogleTest、`ao --test`、Swift 6 AppKit、XCTest。

**Spec:** `docs/superpowers/specs/2026-10-12-p17-smalltalk-change-sets-design.md`（設計判断の理由）。正本は Task 1 で更新する `SPEC.md`（§2.3、§2.4、§3.7、§3.9、§3.10、§3.11、§3.12、§4、§6）。

## 計画で決めたこと（設計書の「計画で詰めること」への答え）

| 問い | 決定 | 理由 |
|---|---|---|
| CLI で Tools-model を読む形 | `runSmalltalkTests(ctx, dir)` が `<dir>/../Tools-model/LOAD_ORDER` を、あればテストの前に file-in する（失敗は 1 行出して終了コード 1）。`ao filein --load-order` と `ao image save --load-order` は `--load-order` を繰り返せるようにする | `ao --test image/tests` と GoogleTest の `block_test`（`runSmalltalkTests(b.ctx, AO_SOURCE_DIR "/image/tests")`）の両方が同じ 1 か所で Tools-model を得る。vendor の LOAD_ORDER から `../Tools-model` を指すと、アプリのバンドルと `vendor_filein_test` / `gcstress_vendor` の見るものが変わる |
| セレクタのバイト順 | Smalltalk で文字ごとに `Character>><` で比べ、挿入ソートで `Array` に並べる（`ClassChanges class>>is:before:`、`selectorsMeta:`） | String の `<`、`asSortedCollection:`、`sort:` は Kernel に無い（調査済み）。`String>>at:` はバイト値の Character を答えるので、バイト順になる |
| 最外の送信の手当てを内側から呼べるか | 呼べる。記録の呼び出し元はすべて最外の ABI 入口（`ao_accept_*`、`ao_remove_*`、`ao_filein_file`、`ao_debug_accept`）か CLI で、Smalltalk から届く経路は無い（`find_referencing_symbols` で確認済み）。送信は常にセッションの base の `ctx` で行い、`haltSuppressed` を上げ、`ctx.debug` を外す | `Scheduler::canHalt` は `haltSuppressed == 0` と評価中のプロセスを要する。`ctx.debug` を外せばスナップショットも置き換わらない（`DebugEntry` が前例） |
| File Out の日付と版 | `aChangeSet fileOutStringDate: aString version: aString`。日付（`YYYY-MM-DD`）と版は C++ が渡す | Smalltalk から日付を得る手段が無い（`Date today` は `DateAndTime` が無く DNU）。設計書の `fileOutString` をこの名前にする（Task 1 で SPEC に書く） |
| `fileOutString…` が読むソース | `Behavior>>sourceCodeAt:`。セッションの外（`ao --test`、テストの `Boot`）では常に nil | ソース表はセッションのもの。Smalltalk のテストは「source not available」の行を確かめ、ソースのある本文は C++ の `ChangesAbi.FileOut*` が確かめる |
| ABI からの送信の失敗 | 記録は捨てる。読み出しは busy と同じ答え（`*_count` -1、`*_at` `AO_ERR`、`current` 0）。書き込みは `ao_changes_file_out` も含めて `change set failed` | 設計書のとおり |
| `ChangeSet` が無いとき | 「Smalltalk が `ChangeSet` に束縛しているのがクラスでない」も含む（`boundClassNamed(s, "ChangeSet")` が空）。記録は何もしない。`count` 0、`current` 0、`*_at` `AO_ERR`、`class_count` / `method_count` -1、書き込みは `change sets unavailable`、`set_current` は `AO_ERR` | 設計書のとおり |

## Global Constraints

- Kernel はネイティブのまま。足すのは `Behavior>>sourceCodeAt:`（`ao_Behavior_sourceCodeAt_`）と `Class>>definition`（`ao_Class_definition`）だけ。`KernelScan.*` は緑のまま。
- `image/Tools-model/ChangeSet.st` は Kernel のネイティブだけを使う（vendor に頼らない。`ao --test` と GoogleTest は vendor を読まない）。使えないもの: `,`、String の `<`、`copyReplaceAll:with:`、`asSortedCollection:`、`sort:`、Dictionary の `keys` / `values`、OrderedCollection の `remove:` / `removeFirst` / `addAll:`、`to:by:do:`、`streamContents:`、`Date`、`{ }`。`select:` / `reject:` は `Array` を答える。
- チャンク形式なので、`.st` の中のメソッド本文の `!` はすべて `!!` と書く（`$!` は `$!!`、`'!'` は `'!!'`）。式のチャンクは file-in で実行されない（初期化は遅延）。
- Smalltalk の名前の規約: クラス名とセレクタは Symbol で持ち、`==` で比べる。セットの名前は String。種類は Symbol `#added`、`#changed`、`#removed`、無いときは nil。
- メッセージは SPEC の文字列そのまま: `change set name empty`、`change set name taken: <name>`、`not in change set: <Class>`、`not in change set: <Class>>><selector>`（クラス側は `<Class> class>><selector>`）、`same change set`、`unknown change set`、`change set failed`、`change sets unavailable`、`cannot write file: <path>`、`file out failed`、`runtime is busy`。
- File Out の本文は P16 の SPEC §3.9 の書式とバイト単位で同じ（P16 の `ChangesAbi.FileOut*` がそのまま緑）。
- ABI の 12 関数のシグネチャは変えない。読み出しは `AbiEntry` を取り、busy なら拒む（`*_count` -1、`*_at` `AO_ERR`、`current` 0）。
- `.aoimage` の版は 3 のまま。チェンジセットはヒープのオブジェクトとして保存される。
- `ao filein` と vendor の LOAD_ORDER の file-in は記録しない（`FileInMode::Plain`）。Tools-model の file-in も Plain。
- Swift: AppKit が正、GUI は `@MainActor`、ランタイム呼び出しは `bridge/ao_abi.h` だけ、強制アンラップを増やさない。
- CLAUDE.md: 各タスクの前に Graphify（`graphify-out/GRAPH_REPORT.md`、必要なら `graphify query` / `graphify path`）と Serena（`find_symbol` / `find_referencing_symbols`）。既存シンボルは `replace_symbol_body` / `insert_after_symbol` / `replace_content` で編集し、新規ファイルは作成後に `get_symbols_overview`。C++ を Serena の外で大きく変えたら Serena の clangd を kill して立ち上げ直させる（restart ツールは無い）。サブエージェントは worktree を使わない（Serena の編集は main の作業木に当たる）。
- コミットは英語の短い命令形。本文に `Graphify: …` / `Serena: …` の一行トレーラと `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`。
- 頼まれていないリファクタをしない。Browser・Debugger・Workspace の振る舞いを変えない。ホットループに触れない。

## Review Focus

SPEC のテスト一覧が直接は触れないが、使う人がまず踏むもの（上から起きやすい順）。各行のテストは担当タスクに入れてある。

1. ワークスペースや Browser で `ChangeSet` のメソッドを壊した（`self error:` を入れた、消した）まま、ライブデバッガを有効にして別のクラスに Accept する。Accept は `AO_OK` のまま、プロセスは止まらず、デバッガのスナップショット（`ao_debug_generation`）も動かない。→ Task 5 `ModelSendsNeitherHaltNorCapture`、`BrokenModelLeavesAcceptAndRemovalAnswers`
2. 1.5.0 で保存したイメージ（`ChangeSet` が無い）をアプリで開く。アプリが Tools-model を file-in し、Change Sorter は `• Unnamed1` を出す。→ Task 6 `testOpenImageWithoutChangeSetFilesInTheToolsModel`
3. セットを作って保存し、開き直してから New… する。保存したセット・current・ID が戻り、新しいセットの ID は保存したどの ID とも重ならない。→ Task 5 `SavedImageKeepsSetsCurrentAndIds`
4. 長い評価の最中（RunLoop pump）に Change Sorter をクリックする。読み出しは拒まれ、一覧は前の内容のまま、エラー欄に `runtime is busy`。評価が終わって窓がキーになると読み直し、エラー欄は消える。→ Task 6 `testBusyReadKeepsTheListsAndSaysBusy`
5. 大文字と小文字、非 ASCII を含むセレクタの並び。P16 の `std::map` と同じバイト順（`Alpha` < `alpha` < `zeta` < `é…`）。→ Task 3 の `changeset.st`（`selectorsMeta:`）、Task 5 `MethodReadsAreInByteOrder`

---

## File Structure

| ファイル | 責務 | タスク |
|---|---|---|
| `SPEC.md`、`docs/phases/P17.md`（新規） | P17 の仕様と要約 | 1, 7 |
| `runtime/src/kernel/Behavior.cpp`、`runtime/src/Session.hpp` / `Session.cpp` | 2 つのネイティブ、`classDefinitionText(Heap&, const WellKnown&, Oop)` | 2 |
| `image/Kernel/Behavior.st`、`image/Kernel/Class.st`、`docs/native-selectors.md` | ネイティブのプロトコルと名前 | 2 |
| `runtime/tests/kernel_scan_test.cpp`、`runtime/tests/source_natives_test.cpp`（新規） | ネイティブのテスト | 2 |
| `image/Tools-model/ChangeSet.st`（新規）、`image/Tools-model/LOAD_ORDER`（新規） | Smalltalk のモデル | 3, 4 |
| `image/tests/changeset.st`（新規） | モデルの Smalltalk テスト | 3, 4 |
| `runtime/src/TestRunner.cpp`、`runtime/src/ao_main.cpp`、`runtime/CMakeLists.txt`、`runtime/tests/cli_test.sh` | Tools-model の file-in、`--load-order` の繰り返し | 3 |
| `runtime/src/ChangeSet.hpp` / `ChangeSet.cpp` | 送信の手当てと ABI の変換（薄い層） | 5 |
| `runtime/src/Session.hpp` / `Session.cpp` | `Session::changes` を消す、`fileOutText` の補助を消す | 5 |
| `runtime/src/Compile.cpp`、`runtime/include/ao/Compile.hpp` | 記録の呼び出し位置の GC の注記 | 5 |
| `runtime/src/abi.cpp`、`bridge/ao_abi.h` | 読み出しの busy の拒否 | 5 |
| `runtime/tests/changes_abi_test.cpp` | `ChangesAbi.*` の更新と追加 | 5 |
| `app/Ao/AoApp.swift`、`app/Ao/ChangeSorterModel.swift`、`app/Ao/ChangeSorterWindow.swift` | Tools-model の file-in、busy の表示 | 6 |
| `app/AoTests/ChangeSorterTests.swift`、`app/AoTests/ToolWindowTests.swift` | Swift のテスト | 6 |
| `scripts/package-app.sh`、`scripts/test.sh` | バンドルへの同梱と起動の確認 | 6 |
| `PHASE`、`CHANGELOG.md`、版の文字列、`docs/README.md`、`graphify-out/` | 完了の記録と版 1.6.0 | 7 |

## 前提（コードで確かめた事実。`f311548`）

- 記録の呼び出し位置: `Compile.cpp:418`（`applyMethodsFor`、`mode != ChunkMode::FileIn`）、`:1227`（`applyChunkActions` のクラス定義）、`:1577`（`acceptInto`）、`:1676`（`removeMethodOf`）、`:1719`（`removeClassOf`）。これらの関数の呼び出し元は `abi.cpp` の `ao_accept_method` / `ao_accept_method_id` / `ao_accept_class` / `ao_remove_method` / `ao_remove_class`、`Session.cpp` の `sessionFileInFile`（`ao_filein_file`）と `sessionDebugAccept`（`ao_debug_accept`）、CLI の `runFileIn`（Plain なので記録しない）。どれも最外の入口で `AbiEntry` を持つ。
- `Compile.hpp` は `removeMethodOf` / `removeClassOf` を「ヒープに割り当てない」と約束している。P17 では記録の送信が最後に割り当てる（GC しうる）。呼び出し元（`abi.cpp:562`、`:590`）は戻ったあと Oop を読まない。
- 最外の送信の前例: `installEmptyWorkspace`（`Session.cpp:108`）、`DebugEntry`（`Session.cpp:2236`。`clearOutOfMemory`、`clearUnwinding`、`refreshStackLimit`、`ctx.debug = nullptr`、終わりに読み直して消す）。`refreshStackLimit` は `ao/Interpreter.hpp`。`ctx.haltSuppressed` を上げると `Scheduler::canHalt` が偽になる（`Scheduler.cpp:515`、`NoHalt` が前例）。
- `ao::send(ctx, receiver, sel, args, argc, ic)` は入口で受け手と引数を自分でルートする。ただし `wk.intern` は割り当てる（GC しうる）ので、Oop を値で持ったまま intern してはいけない。`Root`（`ao/HandleScope.hpp`）は `Root r(roots, value)`。
- `Str::fromUtf8(CallContext&, std::string_view)`（GC しうる、失敗で空の Oop）、`Str::toUtf8(Heap&, Oop)`（`ao/Context.hpp`）。`Oop::fromSmallInteger` は `kSmiMin..kSmiMax`（`ao/Oop.hpp`）の外で assert する。
- `boundClassNamed(Session&, const std::string&)`、`classDefinitionText(Session&, Oop)`、`writeBuf` は `Session.hpp` で公開。`classDefinitionText` は `s.heap` と `s.wk` しか読まない。`methodSource(Oop, std::string&)` は `session()` のソース表を読む（セッションの外では見つからない）。
- `superclassNameOf` と `liveMethodSource` の呼び出し元は `ChangeSet.cpp` の `fileOutText` だけ（Task 5 で消す。消す前に `find_referencing_symbols` で確かめる）。
- `Object>>error:` は文言そのままで abort する（ライブなら止まる）。`ao_eval` の失敗は `AO_ERR` とその文言。
- `runSmalltalkTests(ctx, dir)`（`TestRunner.cpp`）は `AoTest` を作り、`dir` の `*.st` を名前順に 1 つのメソッド本文としてコンパイルして送る。`self assert: x equals: y` だけがある。呼び出し元は `ao_main.cpp` の `bootAndRunTests` と `runtime/tests/block_test.cpp:82`。
- `fileInLoadOrder(ctx, path, errors)` は LOAD_ORDER のディレクトリからの相対パスで読み、Plain で入れる。
- Swift: `ChangeSorterModel.refresh()` は `ao_changes_count() <= 0` を空の一覧と読む。`ChangeSorterWindow.noteImageLoaded()` は `select(setID: nil)`（= `refresh`）してから表示するので、保存済みのセットは読み直しで出る。`ChangeSorterTests.boot()` は `ao_runtime_boot` だけで vendor も Tools-model も読まない。`fileInVendor(at:transcript:)`（`AoApp.swift:38`）が vendor の行を Transcript と stderr に書く。`scripts/package-app.sh:19` は `image/vendor` だけをバンドルに写す。

---

### Task 0: ブランチ

済み。`feat/p17-smalltalk-change-sets` は `main`（`f311548`、PR #28 のマージ）の上にあり、設計書のコミット 1 つを持つ。`PHASE` は `P16`。

---

### Task 1: SPEC を先に直す（P17 の追記）

**Files:**
- Modify: `SPEC.md`（§2.2 の `Tools-model/` の行、§2.3、§2.4、§3.7 手順 9、§3.9「Change Sorter（P16）」と「起動と同梱」、§3.10 の busy の規則と「チェンジセット（P16）」、§3.11、§3.12、§4、§6）
- Create: `docs/phases/P17.md`（`docs/phases/P16.md` と同じ見出し: 結論、前提、範囲、TDD、受け入れ、手動確認）

**Interfaces:**
- Produces: 以降のタスクが引く SPEC の文言（下の「書くこと」）。

- [ ] **Step 1: Graphify と Serena**

`graphify-out/GRAPH_REPORT.md` の `ChangeSet.cpp` のコミュニティを読み、SPEC の該当行を `grep -n "チェンジセット\|Change Sorter\|Tools-model\|P16" SPEC.md` で拾う。

- [ ] **Step 2: SPEC に書くこと**

既存の P16 の文を消さず、P17 で変わる所を書き換える。書く中身（文言は Global Constraints のまま）:

1. §2.2 ディレクトリ契約の `Tools-model/` の行: 「Ao のツールが使うモデル（P17: チェンジセット）。自前。Kernel のネイティブだけを使う」。
2. §2.3 に P17 の行: 「P17 Smalltalk のチェンジセット: チェンジセットを `image/Tools-model/` の Smalltalk のクラス `ChangeSet` にし、イメージに保存する」。§2.4: P17 は 1.6.0。
3. §3.7 手順 9: vendor の LOAD_ORDER に続けて `image/Tools-model/LOAD_ORDER` を file-in する（Plain。ソースも記録も無い）。
4. §3.9「Change Sorter（P16）」: 「Smalltalk 側に `ChangeSet` クラスを作らない」を撤回し、「モデルは Smalltalk の `ChangeSet`（P17）、窓は AppKit」にする。busy で読み出しを拒まれたら、一覧を前のまま残し、エラー欄に `runtime is busy`。評価が終わり窓がキーになったら読み直し、エラー欄の `runtime is busy` を消す。Open Image… のあとは、そのイメージに保存されていたセットを current を選んで出す（`ChangeSet` の無いイメージなら、アプリが Tools-model を file-in してから出すので `• Unnamed1`）。
5. §3.9「起動と同梱」: Tools-model のディレクトリは `AO_TOOLS_MODEL_DIR`（空でないとき）か、バンドルの `Contents/Resources/Tools-model`。vendor のあとに file-in し、Transcript と stderr（`ao: ` の後）に `tools model loaded: <dir>` / `tools model not found: <dir>` / `tools model file-in failed: <dir>` の 1 行。Open Image… で開いたイメージに `ChangeSet` クラスが無いとき（`ao_browser_class_id("ChangeSet")` が 0）も同じ file-in をする。`scripts/package-app.sh` は `image/Tools-model` を `Contents/Resources/Tools-model` に写す。
6. §3.10 の busy の規則: `ao_changes_*` の読み出しを「busy でも読める」の列から外し、「最外の入口。busy なら `*_count` -1、`*_at` `AO_ERR`、`ao_changes_current` 0」と書く。
7. §3.10「チェンジセット」: 記録の置き場をセッションのメモリから Smalltalk の `ChangeSet` に。モデルの入口（クラス側: `allChangeSets`、`current`、`named:`、`withId:`、`newNamed:`、`reasonToCreate:`、`noteMethodChange:selector:meta:existed:`、`noteClassChange:existed:`、`noteMethodRemoval:selector:meta:`、`noteClassRemoval:`。インスタンス側: `name`、`id`、`classChanges`、`changesFor:`、`makeCurrent`、`reasonToRemoveClass:`、`reasonToRemoveSelector:class:meta:`、`removeClass:`、`removeSelector:class:meta:`、`reasonToMoveClass:to:`、`reasonToMoveSelector:class:meta:to:`、`moveClass:to:`、`moveSelector:class:meta:to:`、`fileOutStringDate:version:`。`ClassChanges`: `className`、`definitionKind`、`kindOf:meta:`、`selectorsMeta:`）。初期化は遅延（`AllChangeSets` が nil なら最初の参照で空の `Unnamed1` を作り current にする）。ID は `NextId` から振り、同じイメージの中で使い回さない。拒否の理由は `reasonTo…` が String で答え（成功は nil）、`newNamed:` などワークスペース向けの操作は理由があれば `self error:`。C++ からの送信は最外の送信（abort を読んで消す、ライブデバッガで止まらない、スナップショットを置き換えない）。送信の失敗: 記録は捨てる、読み出しは busy と同じ答え、書き込みは `change set failed`。`ChangeSet` が無いとき（Smalltalk の `ChangeSet` がクラスでないときも）: 記録は何もしない、`ao_changes_count` 0、`ao_changes_current` 0、`*_at` `AO_ERR`、`ao_changes_class_count` / `ao_changes_method_count` -1、`ao_changes_set_current` `AO_ERR`、書き込みは `change sets unavailable`。File Out の日付と版は C++ が渡す。足したネイティブ: `Behavior>>sourceCodeAt: aSelector`（受け手自身のメソッド辞書のそのセレクタの、ソース表のソースを String で。無い、ネイティブ、ソースが無い、セッションの外では nil）、`Class>>definition`（`ao_browser_class_definition` と同じ本文の String）。
8. §3.11: チェンジセットはイメージに保存される（クラス変数として）。版は 3 のまま。保存したイメージを開くとセット・current・ID が戻る。
9. §3.12「自作してよいもの」に「`image/Tools-model/` の、Ao のツールが使うモデル（チェンジセット）」。§2.2 と揃える。
10. §4: テスト名（Task 2〜6 のテスト名をそのまま列挙。`image/tests/changeset.st` も）。
11. §6 に `### Smalltalk のチェンジセット（P17）` の小節とチェックリスト（すべて `[ ]`）。

- [ ] **Step 3: `docs/phases/P17.md` を書く**

`docs/phases/P16.md` の形で。結論（上の Goal と、計画書へのリンク `../superpowers/plans/2026-10-12-p17-smalltalk-change-sets.md`）、前提（P16 緑、PR #28 マージ済み）、範囲（やる / やらない。やらないは設計書の「やらないこと」）、TDD（Task 2〜6 のテストファイル名）、受け入れ（`[ ]`）、手動確認（`[ ]` で 5 項目: Accept が Change Sorter に出る、Save Image… → Open Image… でセットが戻る、1.5.0 のイメージを開くと `• Unnamed1`、ワークスペースで `ChangeSet current name` が `'Unnamed1'`、長い評価中に Change Sorter をクリックすると `runtime is busy`）。

- [ ] **Step 4: コミット**

```bash
git add SPEC.md docs/phases/P17.md
git commit -m "Specify Smalltalk change sets (P17)" -m "Graphify: query noteMethodChange ChangeSet recording send Smalltalk
Serena: none (spec only)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: ネイティブ `Behavior>>sourceCodeAt:` と `Class>>definition`

**Files:**
- Modify: `runtime/src/Session.hpp`、`runtime/src/Session.cpp`（`classDefinitionText` の Heap 版）
- Modify: `runtime/src/kernel/Behavior.cpp`（2 つのネイティブと `installBehavior` の登録）
- Modify: `image/Kernel/Behavior.st`、`image/Kernel/Class.st`、`docs/native-selectors.md`
- Modify: `runtime/tests/kernel_scan_test.cpp`（`RequiredSelectorsAreNativeMethods` の一覧）
- Create: `runtime/tests/source_natives_test.cpp`（`runtime/CMakeLists.txt` のテストのソース一覧に足す）

**Interfaces:**
- Produces: `std::string classDefinitionText(const Heap& heap, const WellKnown& wk, Oop cls)`（`Session.hpp`。既存の `classDefinitionText(Session&, Oop)` はこれを呼ぶだけになる）。Smalltalk の `aBehavior sourceCodeAt: aSymbolOrString` → String か nil、`aClass definition` → String。

- [ ] **Step 1: Graphify と Serena**

`graphify query "installBehavior ao_Class_category classDefinitionText"`。Serena: `find_symbol installBehavior`（`runtime/src/kernel/Behavior.cpp`、body あり）、`find_symbol classDefinitionText`、`find_referencing_symbols classDefinitionText`、`find_symbol methodSource`（`Session.cpp`）。`Heap` と `WellKnown` が const で読めるか（`classNameOf`、`instVarList`、`classVarList`、`definitionCategory` の引数）を確かめ、const で読めなければ `Heap&` / `WellKnown&` にする。

- [ ] **Step 2: 失敗するテストを書く**

`runtime/tests/source_natives_test.cpp`:

```cpp
#include "ao_abi.h"

#include <gtest/gtest.h>

#include <string>

namespace {

class SourceNatives : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  void TearDown() override { ao_runtime_shutdown(); }
};

// Print it through the ABI: the printString, or "<rc: message>" when it fails.
std::string printIt(const char* source) {
  char out[4096] = "";
  AoSpan err{};
  const int rc = ao_eval(source, static_cast<int>(std::char_traits<char>::length(source)),
                         AO_EVAL_PRINT, out, static_cast<int>(sizeof out), &err);
  if (rc != AO_OK) {
    return "<" + std::to_string(rc) + ": " + err.message + ">";
  }
  return out;
}

void acceptClass(const char* source) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class(source, &err)) << err.message;
}

void acceptMethod(const char* cls, int meta, const char* source) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method(cls, meta, source, &err)) << err.message;
}

TEST_F(SourceNatives, SourceCodeAtAnswersTheAcceptedSource) {
  acceptClass("Object subclass: #P17Probe\n  instanceVariableNames: ''\n"
              "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'P17-Test'");
  acceptMethod("P17Probe", 0, "answer\n\t^42");
  acceptMethod("P17Probe", 1, "make\n\t^self new");
  EXPECT_EQ("'answer\n\t^42'", printIt("P17Probe sourceCodeAt: #answer"));
  EXPECT_EQ("'answer\n\t^42'", printIt("P17Probe sourceCodeAt: 'answer'"));
  EXPECT_EQ("'make\n\t^self new'", printIt("P17Probe class sourceCodeAt: #make"));
}

TEST_F(SourceNatives, SourceCodeAtAnswersNilWithoutASource) {
  acceptClass("Object subclass: #P17Probe\n  instanceVariableNames: ''\n"
              "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'P17-Test'");
  acceptMethod("P17Probe", 0, "answer\n\t^42");
  // Not on this side, inherited, unknown, a NativeMethod, a non-selector argument.
  EXPECT_EQ("nil", printIt("P17Probe class sourceCodeAt: #answer"));
  EXPECT_EQ("nil", printIt("P17Probe sourceCodeAt: #printString"));
  EXPECT_EQ("nil", printIt("P17Probe sourceCodeAt: #neverDefinedAnywhere"));
  EXPECT_EQ("nil", printIt("Object sourceCodeAt: #printString"));
  EXPECT_EQ("nil", printIt("P17Probe sourceCodeAt: 3"));
}

// text as a Smalltalk string literal (each `'` doubled).
std::string literal(const std::string& text) {
  std::string lit = "'";
  for (const char c : text) {
    if (c == '\'') {
      lit += '\'';
    }
    lit += c;
  }
  return lit + "'";
}

TEST_F(SourceNatives, ClassDefinitionAnswersTheBrowserText) {
  // ao_accept_class reads chunks, so the category has no `!`; its `'` is doubled in the text.
  acceptClass("Object subclass: #P17Probe\n  instanceVariableNames: 'a b'\n"
              "  classVariableNames: 'K'\n  poolDictionaries: ''\n  category: 'P17-Te''st'");
  const int64_t id = ao_browser_class_id("P17Probe");
  ASSERT_NE(0, id);
  char browser[1024] = "";
  ASSERT_EQ(AO_OK, ao_browser_class_definition(id, browser, static_cast<int>(sizeof browser)));
  EXPECT_EQ("true", printIt(("P17Probe definition = " + literal(browser)).c_str()));
}

TEST_F(SourceNatives, DefinitionIsNotUnderstoodByAMetaclass) {
  EXPECT_EQ("<1: doesNotUnderstand: #definition>", printIt("Object class definition"));
}

}  // namespace
```

`ao_eval` の引数と `AO_EVAL_PRINT` の名前は `bridge/ao_abi.h` と `runtime/tests/changes_abi_test.cpp` の `printIt` に合わせる（違っていたらそちらに従う。DNU の答えの形 `<rc: message>` も既存のテストの書き方に合わせる）。`kernel_scan_test.cpp` の `RequiredSelectorsAreNativeMethods` の一覧に `{"Behavior", "sourceCodeAt:"}` と `{"Class", "definition"}` を、既存の行の形で足す。

- [ ] **Step 3: 赤を確かめる**

Run: `cmake --build build -j && ctest --test-dir build -R 'SourceNatives|KernelScan' --output-on-failure`
Expected: FAIL（`doesNotUnderstand: #sourceCodeAt:` / `#definition`、KernelScan の一覧の 2 行）

- [ ] **Step 4: 実装**

`Session.hpp` の `classDefinitionText(Session& s, Oop cls)` の宣言の前に:

```cpp
// ao_browser_class_definition's text for cls, from the heap alone (SPEC §3.10 Class>>definition,
// P17): chunk-safe already (the category's `!` and `'` doubled).
std::string classDefinitionText(const Heap& heap, const WellKnown& wk, Oop cls);
```

`Session.cpp` で既存の本体をこの版に移し（`s.heap` → `heap`、`s.wk` → `wk`）、`classDefinitionText(Session& s, Oop cls)` は `return classDefinitionText(s.heap, s.wk, cls);` にする（`replace_symbol_body`）。

`runtime/src/kernel/Behavior.cpp` に `#include "../Session.hpp"` を足し（`methodSource`、`session()`、`classDefinitionText` のため）、`ao_Class_category` の後に:

```cpp
// SPEC §3.10 Behavior>>sourceCodeAt: (P17): the source-table text of the method aSelector names in
// the receiver's own dictionary (a metaclass's is the class side). nil when there is none, it is a
// NativeMethod, it has no source, or the receiver's heap is not the session's (the source table is
// the session's). The selector is looked up, never interned.
Oop ao_Behavior_sourceCodeAt_(CallContext& ctx, const Oop& receiver, const Oop* args,
                              std::uint32_t argc) {
  if (argc != 1 || !receiver.isHeap()) {
    return Oop{};
  }
  Session* s = session();
  if (s == nullptr || &s->heap != &ctx.heap || !args[0].isHeap() ||
      (ctx.heap.flags(args[0]) & kFlagBytes) == 0) {
    return Oop::nil();
  }
  const Oop sel = ctx.wk.findSymbol(Str::toUtf8(ctx.heap, args[0]));
  if (!sel.isHeap() || ctx.heap.size(receiver) <= kClassSlotMethodDict) {
    return Oop::nil();
  }
  const Oop method =
      MethodDictionary::at(ctx.heap, ctx.heap.slotAt(receiver, kClassSlotMethodDict), sel);
  if (!method.isHeap() || ctx.heap.klass(method) == ctx.wk.nativeMethodClass) {
    return Oop::nil();
  }
  std::string text;
  if (!methodSource(method, text)) {
    return Oop::nil();
  }
  return Str::fromUtf8(ctx, text);
}

// SPEC §3.10 Class>>definition (P17): ao_browser_class_definition's text as a String.
Oop ao_Class_definition(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) {
    return Oop{};
  }
  return Str::fromUtf8(ctx, classDefinitionText(ctx.heap, ctx.wk, receiver));
}
```

（`ctx.heap.size`、`kFlagBytes`、`MethodDictionary::at` のヘッダは `Behavior.cpp` の既存の include と `Session.cpp` の `liveMethodSource` に合わせる。受け手がクラスの形かを見る既存の補助（`pointerSlots` など）が kernel 側にあればそれを使う。）

`installBehavior` に、`compiledMethodAt:` と `category` の登録の隣で:

```cpp
  putNative(heap, wk, cls, "sourceCodeAt:", 1, "ao_Behavior_sourceCodeAt_",
            ao_Behavior_sourceCodeAt_);
  putNative(heap, wk, classCls, "definition", 0, "ao_Class_definition", ao_Class_definition);
```

`image/Kernel/Behavior.st` の `'accessing'` の見出しの下のコメントに `sourceCodeAt:` を、`image/Kernel/Class.st` に `definition` を、既存のファイルの書き方（プロトコルの見出しとコメントだけ。実行定義を書かない）で足す。`docs/native-selectors.md` の表に:

```
| `sourceCodeAt:` | `sourceCodeAt_` | `ao_Behavior_sourceCodeAt_` |
| `definition` | `definition` | `ao_Class_definition` |
```

- [ ] **Step 5: 緑を確かめる**

Run: `cmake --build build -j && ctest --test-dir build -R 'SourceNatives|KernelScan|KernelInstall|BrowserAbi|ImageSaveLoad' --output-on-failure`
Expected: PASS。続けて `ctest --test-dir build --output-on-failure -E '^KernelBench\.'` で全体が緑（Behavior / Class のセレクタ数を数えるテストがあれば、そのテストの期待値に 1 を足すのが正しいかを読んでから直す）。

- [ ] **Step 6: Serena の overview とコミット**

`get_symbols_overview runtime/tests/source_natives_test.cpp`。

```bash
git add runtime/src/Session.hpp runtime/src/Session.cpp runtime/src/kernel/Behavior.cpp \
  image/Kernel/Behavior.st image/Kernel/Class.st docs/native-selectors.md \
  runtime/tests/kernel_scan_test.cpp runtime/tests/source_natives_test.cpp runtime/CMakeLists.txt
git commit -m "Add Behavior>>sourceCodeAt: and Class>>definition natives (P17)" -m "Graphify: query installBehavior ao_Class_category classDefinitionText
Serena: replace_symbol_body ao::classDefinitionText; insert_after_symbol ao_Class_category

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: Smalltalk のモデル（記録・種類・理由・Remove / Move）と Tools-model の file-in

**Files:**
- Create: `image/Tools-model/ChangeSet.st`、`image/Tools-model/LOAD_ORDER`（`.gitkeep` は消す）
- Create: `image/tests/changeset.st`
- Modify: `runtime/src/TestRunner.cpp`（`runSmalltalkTests` が Tools-model を先に file-in）
- Modify: `runtime/src/ao_main.cpp`（`--load-order` の繰り返し、usage の文言）
- Modify: `runtime/CMakeLists.txt`（`gcstress_vendor` に Tools-model を足す）、`runtime/tests/cli_test.sh`（`--load-order` 2 つの場合）

**Interfaces:**
- Consumes: Kernel のネイティブだけ。
- Produces（Task 4〜6 が使う。名前を変えない）:
  - `ChangeSet class`: `allChangeSets`（OrderedCollection、作った順）、`current`、`named: aString`（nil 可）、`withId: anInteger`（nil 可）、`reasonToCreate: aString`（nil か String）、`newNamed: aString`（current にして答える。理由があれば `self error:`）、`noteMethodChange: className selector: aSymbol meta: aBoolean existed: aBoolean`、`noteClassChange: className existed: aBoolean`、`noteMethodRemoval: className selector: aSymbol meta: aBoolean`、`noteClassRemoval: className`、`kindAfterAccept: was existed: aBoolean`、`kindAfterRemoval: was`、`text: aString with: anotherString`、`entryName: className selector: aSelector meta: aBoolean`、`classNamed: aSymbol`（Task 4）。
  - `ChangeSet`: `name`、`id`、`classChanges`（OrderedCollection of ClassChanges、最初に記録した順）、`changesFor: className`（nil 可）、`makeCurrent`、`reasonToRemoveClass:`、`reasonToRemoveSelector:class:meta:`、`removeClass:`、`removeSelector:class:meta:`、`reasonToMoveClass:to:`、`reasonToMoveSelector:class:meta:to:`、`moveClass:to:`、`moveSelector:class:meta:to:`。
  - `ClassChanges`: `className`（Symbol）、`definitionKind`（nil / Symbol）、`kindOf: aSelector meta: aBoolean`（nil / Symbol）、`selectorsMeta: aBoolean`（バイト順の Array of Symbol）、`isEmpty`。

- [ ] **Step 1: Graphify と Serena**

`graphify query "runSmalltalkTests bootAndRunTests fileInLoadOrder"`。Serena: `find_symbol runSmalltalkTests`（body）、`find_referencing_symbols runSmalltalkTests`、`find_symbol runFileIn`、`find_symbol runImage`、`find_symbol fileInLoadOrder`。`image/vendor/cuis/Time.st` の頭でクラス定義と `methodsFor:` のチャンクの書き方を見る。

- [ ] **Step 2: 失敗するテストを書く**

`image/tests/changeset.st`（ファイル全体が 1 つのメソッド本文。チャンク形式ではないので `!` はそのまま書く）:

```smalltalk
"changeset — SPEC §3.9 種類の規則、§3.10 チェンジセット（P17）。ChangeSet を参照する唯一のテストファイルなので、始まりは遅延の初期化の直後。"
| first work other entry sels |
"遅延の初期化: 最初の参照で空の Unnamed1 が current になる"
first := ChangeSet current.
self assert: first name equals: 'Unnamed1'.
self assert: ChangeSet allChangeSets size equals: 1.
self assert: first classChanges size equals: 0.
self assert: (ChangeSet withId: first id) == first equals: true.
self assert: (ChangeSet named: 'Unnamed1') == first equals: true.
self assert: (ChangeSet named: 'Nope') equals: nil.
self assert: (ChangeSet withId: 0) equals: nil.

"作成の理由と newNamed:"
self assert: (ChangeSet reasonToCreate: '') equals: 'change set name empty'.
self assert: (ChangeSet reasonToCreate: 'Unnamed1') equals: 'change set name taken: Unnamed1'.
self assert: (ChangeSet reasonToCreate: 'Work') equals: nil.
work := ChangeSet newNamed: 'Work'.
self assert: ChangeSet current == work equals: true.
self assert: work id > first id equals: true.
self assert: ChangeSet allChangeSets size equals: 2.
self assert: (ChangeSet allChangeSets at: 2) == work equals: true.

"メソッドの種類: 無し→追加/変更、追加→追加、変更→変更、削除→変更、追加の削除で項目ごと消える"
ChangeSet noteMethodChange: #Foo selector: #bar meta: false existed: false.
entry := work changesFor: #Foo.
self assert: entry className equals: #Foo.
self assert: entry definitionKind equals: nil.
self assert: (entry kindOf: #bar meta: false) equals: #added.
ChangeSet noteMethodChange: #Foo selector: #bar meta: false existed: true.
self assert: (entry kindOf: #bar meta: false) equals: #added.
ChangeSet noteMethodChange: #Foo selector: #baz meta: false existed: true.
self assert: (entry kindOf: #baz meta: false) equals: #changed.
ChangeSet noteMethodRemoval: #Foo selector: #baz meta: false.
self assert: (entry kindOf: #baz meta: false) equals: #removed.
ChangeSet noteMethodChange: #Foo selector: #baz meta: false existed: false.
self assert: (entry kindOf: #baz meta: false) equals: #changed.
ChangeSet noteMethodRemoval: #Foo selector: #bar meta: false.
self assert: (entry kindOf: #bar meta: false) equals: nil.
ChangeSet noteMethodRemoval: #Foo selector: #baz meta: false.
self assert: (entry kindOf: #baz meta: false) equals: #removed.
"クラス側は別の辞書"
ChangeSet noteMethodChange: #Foo selector: #bar meta: true existed: false.
self assert: (entry kindOf: #bar meta: true) equals: #added.
self assert: (entry kindOf: #bar meta: false) equals: nil.
"追加したメソッドだけの項目は、その削除で項目ごと消える"
ChangeSet noteMethodChange: #Solo selector: #only meta: false existed: false.
ChangeSet noteMethodRemoval: #Solo selector: #only meta: false.
self assert: (work changesFor: #Solo) equals: nil.
"記録は current だけに入る"
self assert: (first changesFor: #Foo) equals: nil.

"クラスの種類: 追加したクラスの削除は項目ごと、既存クラスの削除はメソッドの項目を消して削除"
ChangeSet noteClassChange: #Made existed: false.
self assert: (work changesFor: #Made) definitionKind equals: #added.
ChangeSet noteClassChange: #Made existed: true.
self assert: (work changesFor: #Made) definitionKind equals: #added.
ChangeSet noteMethodChange: #Made selector: #m meta: false existed: false.
ChangeSet noteClassRemoval: #Made.
self assert: (work changesFor: #Made) equals: nil.
ChangeSet noteClassChange: #Old existed: true.
ChangeSet noteMethodChange: #Old selector: #m meta: false existed: true.
ChangeSet noteClassRemoval: #Old.
self assert: (work changesFor: #Old) definitionKind equals: #removed.
self assert: ((work changesFor: #Old) selectorsMeta: false) size equals: 0.
ChangeSet noteClassRemoval: #Gone.
self assert: (work changesFor: #Gone) definitionKind equals: #removed.
"記録の順"
self assert: (work classChanges at: 1) className equals: #Foo.
self assert: (work classChanges at: 2) className equals: #Old.
self assert: (work classChanges at: 3) className equals: #Gone.

"セレクタのバイト順（大文字が先、長さは後）"
ChangeSet noteMethodChange: #Order selector: #zeta meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #alpha meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #Alpha meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #al meta: false existed: false.
ChangeSet noteMethodChange: #Order selector: #at:put: meta: false existed: false.
sels := (work changesFor: #Order) selectorsMeta: false.
self assert: sels size equals: 5.
self assert: (sels at: 1) equals: #Alpha.
self assert: (sels at: 2) equals: #al.
self assert: (sels at: 3) equals: #alpha.
self assert: (sels at: 4) equals: #at:put:.
self assert: (sels at: 5) equals: #zeta.

"Remove from Change Set の理由と操作"
self assert: (work reasonToRemoveClass: #Nope) equals: 'not in change set: Nope'.
self assert: (work reasonToRemoveSelector: #nope class: #Foo meta: false) equals: 'not in change set: Foo>>nope'.
self assert: (work reasonToRemoveSelector: #nope class: #Foo meta: true) equals: 'not in change set: Foo class>>nope'.
self assert: (work reasonToRemoveSelector: #x class: #Nope meta: false) equals: 'not in change set: Nope>>x'.
self assert: (work reasonToRemoveSelector: #baz class: #Foo meta: false) equals: nil.
work removeSelector: #baz class: #Foo meta: false.
self assert: ((work changesFor: #Foo) kindOf: #baz meta: false) equals: nil.
work removeSelector: #bar class: 'Foo' meta: true.
self assert: (work changesFor: #Foo) equals: nil.
self assert: (work reasonToRemoveClass: #Gone) equals: nil.
work removeClass: #Gone.
self assert: (work changesFor: #Gone) equals: nil.

"Move to の理由と操作"
other := ChangeSet newNamed: 'Other'.
self assert: (work reasonToMoveClass: #Order to: work) equals: 'same change set'.
self assert: (work reasonToMoveSelector: #zeta class: #Order meta: false to: work) equals: 'same change set'.
self assert: (work reasonToMoveClass: #Nope to: other) equals: 'not in change set: Nope'.
self assert: (work reasonToMoveSelector: #nope class: #Order meta: false to: other) equals: 'not in change set: Order>>nope'.
work moveSelector: #zeta class: #Order meta: false to: other.
self assert: ((other changesFor: #Order) kindOf: #zeta meta: false) equals: #added.
self assert: ((work changesFor: #Order) kindOf: #zeta meta: false) equals: nil.
"移したクラスの種類が移し先を上書きし、移す側に無い定義の種類は移し先のまま"
ChangeSet noteClassChange: #Order existed: false.
self assert: ChangeSet current == other equals: true.
work moveClass: #Order to: other.
self assert: (work changesFor: #Order) equals: nil.
self assert: (other changesFor: #Order) definitionKind equals: #added.
self assert: ((other changesFor: #Order) kindOf: #alpha meta: false) equals: #added.
self assert: ((other changesFor: #Order) kindOf: #zeta meta: false) equals: #added.

"Make Current"
work makeCurrent.
self assert: ChangeSet current == work equals: true.
self assert: ChangeSet allChangeSets size equals: 3.
```

`runtime/tests/cli_test.sh` の `filein` の場合に、`--load-order` を 2 つ渡して両方が読まれる（2 つ目のクラスが定義される）ことを、既存の `filein` の場合と同じ書き方で足す（一時ディレクトリに LOAD_ORDER を 2 つ作る）。

- [ ] **Step 3: 赤を確かめる**

Run: `cmake --build build -j && ./build/runtime/ao --test image/tests; ctest --test-dir build -R 'cli_|Block' --output-on-failure`
Expected: `ao --test: changeset.st: doesNotUnderstand: #current` 相当で FAIL（`ChangeSet` が nil）、`block_test` の Smalltalk テストも FAIL、cli の `--load-order` 2 つで usage エラー。`ao` の実行ファイルの場所は `build/` の中を `find build -name ao -type f` で確かめる。

- [ ] **Step 4: `image/Tools-model/LOAD_ORDER` と `ChangeSet.st` を書く**

`image/Tools-model/LOAD_ORDER`:

```
# Ao のツールが使うモデル（SPEC §2.2、§3.12）。vendor のあとに file-in する。Kernel だけを使う。
ChangeSet.st
```

`image/Tools-model/ChangeSet.st`（チャンク形式。メソッド本文の `!` は `!!`）:

```smalltalk
"ChangeSet — Ao のチェンジセット（SPEC §3.9 Change Sorter、§3.10 チェンジセット、P17）。モデルだけを持ち、窓は AppKit。Kernel のネイティブだけを使う。file-in は式のチャンクを実行しないので、初期化は遅延。"!

Object subclass: #ClassChanges
  instanceVariableNames: 'className definitionKind instanceSide classSide'
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Tools-Changes'!

Object subclass: #ChangeSet
  instanceVariableNames: 'name id classChanges'
  classVariableNames: 'AllChangeSets Current NextId'
  poolDictionaries: ''
  category: 'Tools-Changes'!

!ClassChanges class methodsFor: 'instance creation'!
named: aSymbol
	"aSymbol のクラスの、空の項目。"
	^self new setClassName: aSymbol! !

!ClassChanges class methodsFor: 'ordering'!
is: aString before: anotherString
	"バイト順で aString が先なら true（SPEC §3.9 Change Sorter の並び）。"
	| n |
	n := aString size min: anotherString size.
	1 to: n do: [:i |
		(aString at: i) = (anotherString at: i)
			ifFalse: [^(aString at: i) < (anotherString at: i)]].
	^aString size < anotherString size! !

!ClassChanges methodsFor: 'private'!
setClassName: aSymbol
	className := aSymbol.
	instanceSide := Dictionary new.
	classSide := Dictionary new! !

!ClassChanges methodsFor: 'accessing'!
className
	^className!

definitionKind
	"nil、#added、#changed、#removed。"
	^definitionKind!

definitionKind: aSymbolOrNil
	definitionKind := aSymbolOrNil!

sideFor: meta
	^meta ifTrue: [classSide] ifFalse: [instanceSide]!

kindOf: aSelector meta: meta
	^(self sideFor: meta) at: aSelector asSymbol ifAbsent: [nil]!

at: aSelector meta: meta put: aKind
	(self sideFor: meta) at: aSelector asSymbol put: aKind!

forget: aSelector meta: meta
	(self sideFor: meta) removeKey: aSelector asSymbol ifAbsent: [nil]!

clearMethods
	instanceSide := Dictionary new.
	classSide := Dictionary new!

isEmpty
	"定義の種類もメソッドの項目も無い。"
	^definitionKind isNil and: [instanceSide size = 0 and: [classSide size = 0]]!

selectorsMeta: meta
	"その側のセレクタを、バイト順に並べた Array。"
	| side out n j |
	side := self sideFor: meta.
	out := Array new: side size.
	n := 0.
	side keysDo: [:sel |
		j := n.
		[j > 0 and: [ClassChanges is: sel before: (out at: j)]] whileTrue: [
			out at: j + 1 put: (out at: j).
			j := j - 1].
		out at: j + 1 put: sel.
		n := n + 1].
	^out!

mergeInto: aClassChanges
	"Move to（SPEC §3.10）: 移す種類が移し先を上書きし、移す側に無い定義の種類は移し先のまま。"
	definitionKind isNil ifFalse: [aClassChanges definitionKind: definitionKind].
	instanceSide keysAndValuesDo: [:sel :kind | aClassChanges at: sel meta: false put: kind].
	classSide keysAndValuesDo: [:sel :kind | aClassChanges at: sel meta: true put: kind]! !

!ChangeSet class methodsFor: 'accessing'!
allChangeSets
	"作った順。最初の参照で空の Unnamed1 を作り、current にする。"
	AllChangeSets isNil ifTrue: [
		AllChangeSets := OrderedCollection new.
		NextId isNil ifTrue: [NextId := 1].
		Current := self addNamed: 'Unnamed1'].
	^AllChangeSets!

current
	self allChangeSets.
	^Current!

named: aString
	^self allChangeSets detect: [:each | each name = aString] ifNone: [nil]!

withId: anInteger
	^self allChangeSets detect: [:each | each id = anInteger] ifNone: [nil]! !

!ChangeSet class methodsFor: 'instance creation'!
reasonToCreate: aString
	"作れないなら理由の String、作れるなら nil。"
	aString size = 0 ifTrue: [^'change set name empty'].
	(self named: aString) isNil ifFalse: [^self text: 'change set name taken: ' with: aString].
	^nil!

newNamed: aString
	"新しいセットを current にして答える。"
	| reason set |
	reason := self reasonToCreate: aString.
	reason isNil ifFalse: [^self error: reason].
	set := self addNamed: aString.
	set makeCurrent.
	^set! !

!ChangeSet class methodsFor: 'recording'!
noteMethodChange: className selector: aSymbol meta: meta existed: existed
	"Accept のあと（C++ から）。existed: そのセレクタのメソッドを置き換えた。"
	self current recordMethodChange: className asSymbol selector: aSymbol asSymbol meta: meta existed: existed!

noteClassChange: className existed: existed
	"クラス定義の Accept のあと。existed: その名前は前からクラスだった。"
	self current recordClassChange: className asSymbol existed: existed!

noteMethodRemoval: className selector: aSymbol meta: meta
	self current recordMethodRemoval: className asSymbol selector: aSymbol asSymbol meta: meta!

noteClassRemoval: className
	self current recordClassRemoval: className asSymbol! !

!ChangeSet class methodsFor: 'kinds'!
kindAfterAccept: was existed: existed
	"SPEC §3.9 種類の規則。無し→追加（置き換えたなら変更）、追加→追加、ほか→変更。"
	was isNil ifTrue: [^existed ifTrue: [#changed] ifFalse: [#added]].
	^was == #added ifTrue: [#added] ifFalse: [#changed]!

kindAfterRemoval: was
	"追加→nil（項目が消える）、ほか→削除。"
	^was == #added ifTrue: [nil] ifFalse: [#removed]! !

!ChangeSet class methodsFor: 'private'!
addNamed: aString
	| set |
	set := self new setName: aString id: NextId.
	NextId := NextId + 1.
	AllChangeSets add: set.
	^set!

text: aString with: anotherString
	| s |
	s := WriteStream on: String new.
	s nextPutAll: aString; nextPutAll: anotherString.
	^s contents!

entryName: className selector: aSelector meta: meta
	"<Class>>><selector>、クラス側は <Class> class>><selector>（SPEC §3.9 削除）。"
	| s |
	s := WriteStream on: String new.
	s nextPutAll: className.
	meta ifTrue: [s nextPutAll: ' class'].
	s nextPutAll: '>>'; nextPutAll: aSelector.
	^s contents! !

!ChangeSet methodsFor: 'private'!
setName: aString id: anInteger
	name := aString.
	id := anInteger.
	classChanges := OrderedCollection new!

entryFor: className
	"className の項目。無ければ作って最後に足す。"
	| entry |
	entry := self changesFor: className.
	entry isNil ifFalse: [^entry].
	entry := ClassChanges named: className asSymbol.
	classChanges add: entry.
	^entry!

forgetEntry: className
	| key kept |
	key := className asSymbol.
	kept := OrderedCollection new.
	classChanges do: [:each | each className == key ifFalse: [kept add: each]].
	classChanges := kept!

dropIfEmpty: className
	| entry |
	entry := self changesFor: className.
	(entry notNil and: [entry isEmpty]) ifTrue: [self forgetEntry: className]! !

!ChangeSet methodsFor: 'accessing'!
name
	^name!

id
	^id!

classChanges
	"ClassChanges の OrderedCollection。最初に記録した順。"
	^classChanges!

changesFor: className
	| key |
	key := className asSymbol.
	^classChanges detect: [:each | each className == key] ifNone: [nil]!

makeCurrent
	Current := self! !

!ChangeSet methodsFor: 'recording'!
recordMethodChange: className selector: aSymbol meta: meta existed: existed
	| entry |
	entry := self entryFor: className.
	entry at: aSymbol meta: meta put: (ChangeSet kindAfterAccept: (entry kindOf: aSymbol meta: meta) existed: existed)!

recordMethodRemoval: className selector: aSymbol meta: meta
	| entry kind |
	entry := self entryFor: className.
	kind := ChangeSet kindAfterRemoval: (entry kindOf: aSymbol meta: meta).
	kind isNil
		ifTrue: [
			entry forget: aSymbol meta: meta.
			self dropIfEmpty: className]
		ifFalse: [entry at: aSymbol meta: meta put: kind]!

recordClassChange: className existed: existed
	| entry |
	entry := self entryFor: className.
	entry definitionKind: (ChangeSet kindAfterAccept: entry definitionKind existed: existed)!

recordClassRemoval: className
	"追加したクラスはメソッドごと消え、ほかは削除になってメソッドの項目が消える。"
	| entry |
	entry := self entryFor: className.
	entry definitionKind == #added ifTrue: [^self forgetEntry: className].
	entry definitionKind: #removed.
	entry clearMethods! !

!ChangeSet methodsFor: 'removing'!
reasonToRemoveClass: className
	(self changesFor: className) isNil
		ifTrue: [^ChangeSet text: 'not in change set: ' with: className].
	^nil!

reasonToRemoveSelector: aSelector class: className meta: meta
	| entry |
	entry := self changesFor: className.
	(entry isNil or: [(entry kindOf: aSelector meta: meta) isNil])
		ifTrue: [^ChangeSet text: 'not in change set: ' with: (ChangeSet entryName: className selector: aSelector meta: meta)].
	^nil!

removeClass: className
	"Remove from Change Set: 項目を忘れる。メソッドとクラスはそのまま。"
	| reason |
	reason := self reasonToRemoveClass: className.
	reason isNil ifFalse: [^self error: reason].
	self forgetEntry: className!

removeSelector: aSelector class: className meta: meta
	| reason |
	reason := self reasonToRemoveSelector: aSelector class: className meta: meta.
	reason isNil ifFalse: [^self error: reason].
	(self changesFor: className) forget: aSelector meta: meta.
	self dropIfEmpty: className! !

!ChangeSet methodsFor: 'moving'!
reasonToMoveClass: className to: aChangeSet
	aChangeSet == self ifTrue: [^'same change set'].
	^self reasonToRemoveClass: className!

reasonToMoveSelector: aSelector class: className meta: meta to: aChangeSet
	aChangeSet == self ifTrue: [^'same change set'].
	^self reasonToRemoveSelector: aSelector class: className meta: meta!

moveClass: className to: aChangeSet
	| reason |
	reason := self reasonToMoveClass: className to: aChangeSet.
	reason isNil ifFalse: [^self error: reason].
	(self changesFor: className) mergeInto: (aChangeSet entryFor: className).
	self forgetEntry: className!

moveSelector: aSelector class: className meta: meta to: aChangeSet
	| reason entry |
	reason := self reasonToMoveSelector: aSelector class: className meta: meta to: aChangeSet.
	reason isNil ifFalse: [^self error: reason].
	entry := self changesFor: className.
	(aChangeSet entryFor: className) at: aSelector meta: meta put: (entry kindOf: aSelector meta: meta).
	entry forget: aSelector meta: meta.
	self dropIfEmpty: className! !
```

注意: このファイルの本文には `!` が無い（Task 4 で足す File Out には `!` が出るので `!!` にする）。`entryFor:` は移し先に項目が無いとき、定義の種類の無い新しい項目を最後に足す（P16 の `targetCopy` と `putClass`）。

- [ ] **Step 5: `runSmalltalkTests` が Tools-model を先に読む**

`TestRunner.cpp` の `runSmalltalkTests` の、`AoTest` を作る前に（`replace_content` で）:

```cpp
  // SPEC §4.4 (P17): the tests may use the tool models (image/Tools-model, beside the tests'
  // directory). Filed in as the app files them in (Plain); a failure fails the run.
  const std::filesystem::path models =
      std::filesystem::path(dir).parent_path() / "Tools-model" / "LOAD_ORDER";
  if (std::filesystem::exists(models)) {
    std::vector<FileInError> errors;
    if (!fileInLoadOrder(ctx, models, errors) || !errors.empty()) {
      for (const FileInError& e : errors) {
        std::fprintf(stderr, "ao --test: %s:%u-%u: %s\n", e.file.c_str(), e.error.span.start,
                     e.error.span.end, e.error.message.c_str());
      }
      std::fprintf(stderr, "ao --test: tools model file-in failed: %s\n", models.c_str());
      return 1;
    }
  }
```

`dir` が `image/tests/`（末尾のスラッシュ付き）でも働くよう、`parent_path` の前に `std::filesystem::path(dir).lexically_normal()` で末尾の区切りを落とすこと（`"image/tests/"` の `parent_path()` は `"image/tests"`）。`FileInError` の欄の名前と `fileInLoadOrder` の include は `ao/Compile.hpp` に合わせる。`runSmalltalkTests` の戻り値の意味（0 成功、1 失敗）は既存に合わせる。

- [ ] **Step 6: `--load-order` を繰り返せるようにする**

`ao_main.cpp`:
- `runFileIn`: `argv[2] == "--load-order"` のとき、`argv[2..]` が `--load-order <path>` の組の並び（1 組以上）であることを確かめ、それぞれ順に `fileInLoadOrder` する。どれかが false なら残りを読まない。エラーはすべて `printFileInErrors`。usage の行は `ao filein --load-order <LOAD_ORDER> [--load-order <LOAD_ORDER>]...`。
- `runImage` の save: `ao image save [--load-order <LOAD_ORDER>]... <path>`。最後の引数が path、その前が組の並び。
- `--help` の行も同じ形に。

`runtime/CMakeLists.txt` の `gcstress_vendor` の 2 つの `filein --load-order "$1"` を `filein --load-order "$1" --load-order "$4"` にし、`COMMAND` の引数の最後に `${CMAKE_SOURCE_DIR}/image/Tools-model/LOAD_ORDER` を足す（`$0`〜`$3` の並びは既存のまま、`$4` を足す。既存の引数の渡し方に合わせる）。コメントの「ao --test は vendor を file-in しない」の行に「Tools-model はテストの隣から読む」を足す。

- [ ] **Step 7: 緑を確かめる**

Run: `cmake --build build -j && ./build/runtime/ao --test image/tests && ctest --test-dir build --output-on-failure -E '^KernelBench\.'`
Expected: PASS（`ChangesAbi.*` は P16 の C++ 実装のままなので緑のまま。`gcstress_vendor`、`cli_*`、`Block*` も緑）。`ao filein --load-order image/vendor/LOAD_ORDER --load-order image/Tools-model/LOAD_ORDER` が終了コード 0。

- [ ] **Step 8: コミット**

```bash
git rm image/Tools-model/.gitkeep
git add image/Tools-model image/tests/changeset.st runtime/src/TestRunner.cpp runtime/src/ao_main.cpp \
  runtime/CMakeLists.txt runtime/tests/cli_test.sh
git commit -m "Add the Smalltalk ChangeSet model and file it in before ao --test (P17)" -m "Graphify: query runSmalltalkTests bootAndRunTests fileInLoadOrder
Serena: replace_content ao::runSmalltalkTests; replace_symbol_body runFileIn, runImage

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: File Out の本文を Smalltalk で書く

**Files:**
- Modify: `image/Tools-model/ChangeSet.st`（`ChangeSet class` の `classNamed:`、`superclassOrder:`、`chunk:on:`、`commentChunk:on:`、`ChangeSet` の `fileOutStringDate:version:` とその補助）
- Modify: `image/tests/changeset.st`（末尾に File Out の確認を足す）

**Interfaces:**
- Consumes: Task 2 の `Class>>definition`、`Behavior>>sourceCodeAt:`。Task 3 の `ClassChanges>>selectorsMeta:`、`ChangeSet class>>text:with:`、`entryName:selector:meta:`。
- Produces: `aChangeSet fileOutStringDate: aString version: aString` → String（P16 の `fileOutText` とバイト単位で同じ）。`ChangeSet class>>classNamed: aSymbol` → Smalltalk がその名前に束縛しているクラス（メタクラスやほかの値なら nil）。

- [ ] **Step 1: Graphify と Serena**

`graphify query "fileOutText superclassOrder commentChunk"`。Serena: `find_symbol fileOutText`（`runtime/src/ChangeSet.cpp`、body）、`find_symbol superclassOrder`、`chunk`、`commentChunk`。本文の書式はこの C++ を正とする（SPEC §3.9 File Out の形式）。

- [ ] **Step 2: 失敗するテストを足す**

`image/tests/changeset.st` の末尾に（同じメソッド本文の続き。一時変数 `out fo expect base sub` を先頭の `| … |` に足す。テストの本文は 1 つのメソッドとしてコンパイルされるので、本文の中で作るクラスは名前で書かず、`subclass:…` の答えを一時変数に入れて使う）:

```smalltalk
"File Out（SPEC §3.9 File Out の形式）。ao --test はセッションの外なのでソースは無く、メソッドは source not available の行になる"
base := Object subclass: #P17Base instanceVariableNames: 'a' classVariableNames: '' poolDictionaries: '' category: 'P17-Test'.
sub := base subclass: #P17Sub instanceVariableNames: '' classVariableNames: '' poolDictionaries: '' category: 'P17-Test'.
Smalltalk at: #P17NotAClass put: 3.
out := ChangeSet newNamed: 'Out "q" !'.
ChangeSet noteClassChange: #P17Sub existed: false.
ChangeSet noteClassChange: #P17Base existed: false.
ChangeSet noteClassChange: #Missing existed: true.
ChangeSet noteMethodChange: #P17Base selector: #kept meta: false existed: false.
ChangeSet noteMethodChange: #P17Base selector: #made meta: true existed: false.
ChangeSet noteMethodRemoval: #P17Base selector: #gone meta: false.
ChangeSet noteClassRemoval: #Dropped.
fo := out fileOutStringDate: '2026-10-12' version: '9.9.9'.
expect := WriteStream on: String new.
expect nextPutAll: '"Change Set: Out ""q"" !!'; cr;
	nextPutAll: 'Date: 2026-10-12'; cr;
	nextPutAll: 'Ao 9.9.9"!'; cr; cr;
	nextPutAll: base definition; nextPutAll: '!'; cr; cr;
	nextPutAll: sub definition; nextPutAll: '!'; cr; cr;
	nextPutAll: '"Removed: P17Base>>gone"!'; cr;
	nextPutAll: '"Removed class: Dropped"!'; cr;
	nextPutAll: '"P17Base>>kept source not available"!'; cr;
	nextPutAll: '"P17Base class>>made source not available"!'; cr;
	nextPutAll: '"Missing definition not available"!'; cr.
self assert: fo equals: expect contents.
self assert: (ChangeSet classNamed: #P17Base) == base equals: true.
self assert: (ChangeSet classNamed: 'P17Sub') == sub equals: true.
self assert: (ChangeSet classNamed: #P17NotAClass) equals: nil.
self assert: (ChangeSet classNamed: #NoSuchGlobal) equals: nil.
"見出しの `!` は 2 つになり、`\"` は 2 つになる。スーパークラスは記録の順に関わらず先"
self assert: (ChangeSet superclassOrder: (OrderedCollection new add: #P17Sub; add: #Missing; add: #P17Base; yourself)) size equals: 3.
self assert: ((ChangeSet superclassOrder: (OrderedCollection new add: #P17Sub; add: #Missing; add: #P17Base; yourself)) at: 1) equals: #P17Base.
```

（`OrderedCollection>>add:` は受け手でなく引数を答えるので、カスケードの最後に `yourself` を置く。メタクラスを答えないことは `Smalltalk at: #P17Meta put: base class` を足して確かめてもよい。）

- [ ] **Step 3: 赤を確かめる**

Run: `./build/runtime/ao --test image/tests`
Expected: FAIL（`doesNotUnderstand: #fileOutStringDate:version:`）

- [ ] **Step 4: 実装（`ChangeSet.st` に足す。メソッド本文の `!` は `!!`）**

`ChangeSet class` の 'private' のあとに:

```smalltalk
!ChangeSet class methodsFor: 'fileIn/Out'!
classNamed: aSymbol
	"Smalltalk が aSymbol に束縛しているクラス（メタクラスでもほかの値でもない）。無ければ nil。"
	| value |
	value := Smalltalk at: aSymbol asSymbol ifAbsent: [nil].
	value class class == Metaclass ifFalse: [^nil].
	^value!

superclassOrder: classNames
	"名前の並び。それぞれを、並びの中にあるスーパークラスのあとに置く。ほかは元の順。"
	| placed out chain at cls i |
	placed := OrderedCollection new.
	out := OrderedCollection new.
	classNames do: [:each |
		chain := OrderedCollection new.
		at := each.
		[at notNil and: [(classNames includes: at) and: [(placed includes: at) not and: [(chain includes: at) not]]]] whileTrue: [
			chain add: at.
			cls := self classNamed: at.
			at := (cls isNil or: [cls superclass isNil]) ifTrue: [nil] ifFalse: [cls superclass name asSymbol]].
		i := chain size.
		[i > 0] whileTrue: [
			placed add: (chain at: i).
			out add: (chain at: i).
			i := i - 1]].
	^out!

chunk: aString on: aStream
	"SPEC §3.8 チャンク形式: `!!` をすべて重ね、閉じる `!!` を足す。"
	aString do: [:c |
		c = $!! ifTrue: [aStream nextPut: $!!].
		aStream nextPut: c].
	aStream nextPut: $!!!

commentChunk: aString on: aStream
	"aString を二重引用符で囲み（中の `""` は重ねる）、チャンクにする。"
	| body |
	body := WriteStream on: String new.
	body nextPut: $".
	aString do: [:c |
		c = $" ifTrue: [body nextPut: $"].
		body nextPut: c].
	body nextPut: $".
	self chunk: body contents on: aStream! !
```

`ChangeSet` に:

```smalltalk
!ChangeSet methodsFor: 'fileIn/Out'!
fileOutStringDate: aDate version: aVersion
	"SPEC §3.9 File Out の形式。aDate は YYYY-MM-DD、aVersion は Ao の版（C++ が渡す）。"
	| out header lostSources lostDefinitions |
	out := WriteStream on: String new.
	header := WriteStream on: String new.
	header nextPutAll: 'Change Set: '; nextPutAll: name; cr;
		nextPutAll: 'Date: '; nextPutAll: aDate; cr;
		nextPutAll: 'Ao '; nextPutAll: aVersion.
	ChangeSet commentChunk: header contents on: out.
	out cr; cr.
	lostSources := OrderedCollection new.
	lostDefinitions := OrderedCollection new.
	self fileOutDefinitionsOn: out lost: lostDefinitions.
	classChanges do: [:each |
		self fileOut: each meta: false on: out lost: lostSources.
		self fileOut: each meta: true on: out lost: lostSources].
	classChanges do: [:each |
		self fileOutRemovalsOf: each meta: false on: out.
		self fileOutRemovalsOf: each meta: true on: out].
	classChanges do: [:each |
		each definitionKind == #removed ifTrue: [
			ChangeSet commentChunk: (ChangeSet text: 'Removed class: ' with: each className) on: out.
			out cr]].
	lostSources do: [:text | ChangeSet commentChunk: text on: out. out cr].
	lostDefinitions do: [:text | ChangeSet commentChunk: text on: out. out cr].
	^out contents!

fileOutDefinitionsOn: out lost: lost
	| defined cls |
	defined := OrderedCollection new.
	classChanges do: [:each |
		(each definitionKind notNil and: [each definitionKind ~~ #removed])
			ifTrue: [defined add: each className]].
	(ChangeSet superclassOrder: defined) do: [:each |
		cls := ChangeSet classNamed: each.
		cls isNil
			ifTrue: [lost add: (ChangeSet text: each with: ' definition not available')]
			ifFalse: [out nextPutAll: cls definition; nextPutAll: '!!'; cr; cr]]!

fileOut: entry meta: meta on: out lost: lost
	"その側の、削除でないメソッドのソースを 1 つの methodsFor: の節に。ソースの無いものは lost へ。"
	| cls side sources source |
	cls := ChangeSet classNamed: entry className.
	side := cls isNil ifTrue: [nil] ifFalse: [meta ifTrue: [cls class] ifFalse: [cls]].
	sources := OrderedCollection new.
	(entry selectorsMeta: meta) do: [:sel |
		(entry kindOf: sel meta: meta) == #removed ifFalse: [
			source := side isNil ifTrue: [nil] ifFalse: [side sourceCodeAt: sel].
			source isNil
				ifTrue: [lost add: (ChangeSet text: (ChangeSet entryName: entry className selector: sel meta: meta) with: ' source not available')]
				ifFalse: [sources add: source]]].
	sources size = 0 ifTrue: [^self].
	out nextPutAll: '!!'; nextPutAll: entry className.
	meta ifTrue: [out nextPutAll: ' class'].
	out nextPutAll: ' methodsFor: ''user''!!'; cr.
	1 to: sources size do: [:i |
		ChangeSet chunk: (sources at: i) on: out.
		i < sources size
			ifTrue: [out cr; cr]
			ifFalse: [out nextPutAll: ' !!'; cr; cr]]!

fileOutRemovalsOf: entry meta: meta on: out
	(entry selectorsMeta: meta) do: [:sel |
		(entry kindOf: sel meta: meta) == #removed ifTrue: [
			ChangeSet commentChunk: (ChangeSet text: 'Removed: ' with: (ChangeSet entryName: entry className selector: sel meta: meta)) on: out.
			out cr]]! !
```

P16 の `fileOutText` と並べて読み、順（ヘッダ、定義、メソッドの節、メソッドの削除、クラスの削除、ソースの無いもの、定義の無いもの）と改行が同じであることを確かめる。

- [ ] **Step 5: 緑を確かめる**

Run: `./build/runtime/ao --test image/tests && ctest --test-dir build -R 'Block|gcstress_vendor' --output-on-failure`
Expected: PASS

- [ ] **Step 6: コミット**

```bash
git add image/Tools-model/ChangeSet.st image/tests/changeset.st
git commit -m "Write the change set File Out in Smalltalk (P17)" -m "Graphify: query fileOutText superclassOrder commentChunk
Serena: find_symbol ao::fileOutText (reference for the format)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）

**Files:**
- Modify（中身を置き換える）: `runtime/src/ChangeSet.hpp`、`runtime/src/ChangeSet.cpp`
- Modify: `runtime/src/Session.hpp`（`ChangeSets changes;` と `#include "ChangeSet.hpp"` を消す、`superclassNameOf` / `liveMethodSource` を消す）、`runtime/src/Session.cpp`（それらの本体）
- Modify: `runtime/src/Compile.cpp`（`#include "ChangeSet.hpp"` を直接）、`runtime/include/ao/Compile.hpp`（`removeMethodOf` / `removeClassOf` の「割り当てない」の注記）
- Modify: `runtime/src/abi.cpp`（読み出しに `AbiEntry`）、`bridge/ao_abi.h`（読み出しの注記）
- Modify: `runtime/tests/changes_abi_test.cpp`

**Interfaces:**
- Consumes: Task 3・4 の Smalltalk の入口（名前は Task 3 の Produces のとおり）、`boundClassNamed`、`writeBuf`、`version_string`。
- Produces: `ChangeSet.hpp` の関数（`note*` 4 つ、`sessionChanges*` 11 個）はシグネチャを変えない。`ChangeKind`、`ClassChange`、`ChangeSet`（C++ の構造体）、`ChangeSets`、`kindAfterAccept`、`kindAfterRemoval` は消える。

- [ ] **Step 1: Graphify と Serena**

`graphify path "noteMethodChange()" "send()"`、`graphify query "ChangeSets Session changes sessionChangesCount"`。Serena: `find_referencing_symbols` を `noteMethodChange`、`noteMethodRemoval`、`noteClassChange`、`noteClassRemoval`、`ChangeSets`、`ChangeKind`、`kindAfterAccept`、`superclassNameOf`、`liveMethodSource`、`removeMethodOf`、`removeClassOf`、`acceptMethodInto`、`acceptMethodReplacing`、`acceptClassSource` に取る。記録の呼び出しのあと、各呼び出し元が Root でない Oop を読み直していないか（`applyMethodsFor` のループの中、`ao_accept_method_id`、`sessionDebugAccept`）を確かめ、読んでいれば Root から読み直すように直す。

- [ ] **Step 2: テストを書き換え、足す（赤）**

`changes_abi_test.cpp`:

1. フィクスチャの `SetUp` で Tools-model を読む:

```cpp
  void SetUp() override {
    ASSERT_EQ(AO_OK, ao_runtime_boot());
    ASSERT_EQ(AO_OK, ao_filein_load_order(AO_SOURCE_DIR "/image/Tools-model/LOAD_ORDER"));
  }
```

2. `TEST(ChangeKinds, AcceptAndRemovalRules)` を消す（規則は `image/tests/changeset.st` が確かめる）。
3. `NewAndMakeCurrentAreRefusedWhileBusy` の中の「the reads still answer」の `EXPECT_EQ(2, ao_changes_count())` を、読み出しも拒まれる形に変え、名前を `WritesAndReadsAreRefusedWhileBusy` にする:

```cpp
    EXPECT_EQ(-1, ao_changes_count());
    EXPECT_EQ(0, ao_changes_current());
    int64_t id = -1;
    char name[32] = "x";
    EXPECT_EQ(AO_ERR, ao_changes_at(0, &id, name, static_cast<int>(sizeof name)));
    EXPECT_EQ(0, id);
    EXPECT_STREQ("", name);
    EXPECT_EQ(-1, ao_changes_class_count(current));
    EXPECT_EQ(-1, ao_changes_method_count(current, "Object", 0));
```

（`current` は hook の外で先に読んだ `ao_changes_current()`。`class_at` と `method_at` も `AO_ERR`、kind は `AO_CHANGE_NONE`、バッファは空、を同じ所で確かめる。）

4. `BootAndImageLoadStartOver` を「boot は始め直す」だけにし（`ao_runtime_shutdown` → `ao_runtime_boot` → Tools-model を file-in → `Unnamed1` 1 つ）、ロードの確かめは次の新しいテストに移す。

5. 新しいテスト:

```cpp
// SPEC §3.11 (P17): the sets are heap objects, so a saved image brings them back with the current
// set and the IDs; a set made after the load takes an ID none of them has.
TEST_F(ChangesAbi, SavedImageKeepsSetsCurrentAndIds) {
  const int64_t first = ao_changes_current();
  const int64_t work = newSet("Work");
  defineClass("Counter");
  const int64_t other = newSet("Other");
  ASSERT_EQ(AO_OK, ao_changes_set_current(work));
  const std::string path = tempPath("p17-sets.aoimage");
  ASSERT_EQ(AO_OK, ao_image_save(path.c_str()));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path.c_str(), &err)) << err.message;
  ASSERT_EQ(3, ao_changes_count());
  EXPECT_EQ(work, ao_changes_current());
  int64_t ids[3] = {};
  char name[32] = "";
  for (int i = 0; i < 3; ++i) {
    ASSERT_EQ(AO_OK, ao_changes_at(i, &ids[i], name, static_cast<int>(sizeof name)));
  }
  EXPECT_EQ(first, ids[0]);
  EXPECT_EQ(work, ids[1]);
  EXPECT_EQ(other, ids[2]);
  EXPECT_EQ(1, ao_changes_class_count(work));
  const int64_t later = newSet("Later");
  EXPECT_GT(later, other);
}

// SPEC §3.10 チェンジセット (P17): with no ChangeSet class, Accept and removal answer as before and
// record nothing, the reads answer no sets, and the writes say so.
TEST_F(ChangesAbi, WithoutTheModelRecordsNothingAndWritesAreUnavailable) {
  AoSpan err{};
  const int64_t model = ao_browser_class_id("ChangeSet");
  ASSERT_NE(0, model);
  ASSERT_EQ(AO_OK, ao_remove_class(model, &err)) << err.message;
  defineClass("Counter");
  EXPECT_EQ(0, ao_changes_count());
  EXPECT_EQ(0, ao_changes_current());
  int64_t id = -1;
  char name[32] = "x";
  EXPECT_EQ(AO_ERR, ao_changes_at(0, &id, name, static_cast<int>(sizeof name)));
  EXPECT_EQ(-1, ao_changes_class_count(1));
  EXPECT_EQ(-1, ao_changes_method_count(1, "Counter", 0));
  EXPECT_EQ(AO_ERR, ao_changes_set_current(1));
  int64_t made = 0;
  EXPECT_EQ(AO_ERR, ao_changes_new("Work", &made, &err));
  EXPECT_STREQ("change sets unavailable", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_drop(1, "Counter", 0, nullptr, &err));
  EXPECT_STREQ("change sets unavailable", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_move(1, 2, "Counter", 0, nullptr, &err));
  EXPECT_STREQ("change sets unavailable", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_file_out(1, tempPath("p17-none.st").c_str(), &err));
  EXPECT_STREQ("change sets unavailable", err.message);
}

// SPEC §3.10 チェンジセット (P17): a model whose methods fail (DNU, error:) drops the record and
// leaves the Accept's and the removal's answers; the reads answer as while busy and the writes
// say change set failed.
TEST_F(ChangesAbi, BrokenModelLeavesAcceptAndRemovalAnswers) {
  AoSpan err{};
  const int64_t model = ao_browser_class_id("ChangeSet");
  ASSERT_EQ(AO_OK, ao_remove_method(model, 1, "allChangeSets", &err)) << err.message;
  defineClass("Counter");
  ASSERT_EQ(AO_OK, ao_accept_method("Counter", 0, "bump\n\t^1", &err)) << err.message;
  const int64_t counter = ao_browser_class_id("Counter");
  ASSERT_EQ(AO_OK, ao_remove_method(counter, 0, "bump", &err)) << err.message;
  EXPECT_EQ(-1, ao_changes_count());
  EXPECT_EQ(0, ao_changes_current());
  int64_t made = 0;
  EXPECT_EQ(AO_ERR, ao_changes_new("Work", &made, &err));
  EXPECT_STREQ("change set failed", err.message);
  EXPECT_EQ(AO_ERR, ao_changes_file_out(1, tempPath("p17-broken.st").c_str(), &err));
  EXPECT_STREQ("change set failed", err.message);
}

// SPEC §3.10 チェンジセット (P17): the model's failure in a record neither halts a live debugger
// nor replaces the snapshot.
TEST_F(ChangesAbi, ModelSendsNeitherHaltNorCapture) {
  ao_set_debug_capture(1);
  ao_set_debug_mode(AO_DEBUG_LIVE);
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("ChangeSet", 1,
                                    "noteMethodChange: c selector: s meta: m existed: e\n"
                                    "\t^self error: 'broken'",
                                    &err))
      << err.message;
  const int generation = ao_debug_generation();
  defineClass("Counter");
  EXPECT_EQ(AO_OK, ao_accept_method("Counter", 0, "bump\n\t^1", &err)) << err.message;
  EXPECT_EQ(0, ao_debug_halted_count());
  EXPECT_EQ(generation, ao_debug_generation());
  EXPECT_EQ("1", printIt("Counter new bump"));
}

// SPEC §3.9 Change Sorter: the method entries are read in byte order.
TEST_F(ChangesAbi, MethodReadsAreInByteOrder) {
  defineClass("Counter");
  AoSpan err{};
  for (const char* source : {"zeta\n\t^1", "alpha\n\t^1", "Alpha\n\t^1", "al\n\t^1"}) {
    ASSERT_EQ(AO_OK, ao_accept_method("Counter", 0, source, &err)) << err.message;
  }
  const int64_t set = ao_changes_current();
  ASSERT_EQ(4, ao_changes_method_count(set, "Counter", 0));
  const char* want[] = {"Alpha", "al", "alpha", "zeta"};
  for (int i = 0; i < 4; ++i) {
    char sel[32] = "";
    int kind = AO_CHANGE_NONE;
    ASSERT_EQ(AO_OK, ao_changes_method_at(set, "Counter", 0, i, sel, static_cast<int>(sizeof sel),
                                          &kind));
    EXPECT_STREQ(want[i], sel);
    EXPECT_EQ(AO_CHANGE_ADDED, kind);
  }
}

// SPEC §3.10 (P17): the workspace entries refuse with error: and the reason.
TEST_F(ChangesAbi, WorkspaceNewNamedRefusesWithTheReason) {
  EXPECT_EQ("<" + std::to_string(AO_ERR) + ": change set name taken: Unnamed1>",
            printIt("ChangeSet newNamed: 'Unnamed1'"));
  EXPECT_EQ("'Work'", printIt("(ChangeSet newNamed: 'Work') name"));
  EXPECT_EQ(2, ao_changes_count());
}
```

（`defineClass`、`newSet`、`tempPath`、`printIt` は既存の補助。`printIt` の失敗の形は既存の定義に合わせて期待値を書く。`ao_set_debug_capture` / `ao_debug_generation` / `ao_debug_halted_count` の名前は `bridge/ao_abi.h` で確かめる。`defineClass` が `ao_accept_class` を使い、記録が必要なら記録されていることも既存のテストで確かめ済み。）

Run: `cmake --build build -j && ctest --test-dir build -R 'ChangesAbi' --output-on-failure`
Expected: FAIL（新しいテストと busy の読み出し。P16 の C++ 実装は Smalltalk のモデルを見ない）

- [ ] **Step 3: `ChangeSet.hpp` を置き換える**

```cpp
#pragma once

#include "ao_abi.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace ao {

struct CallContext;

// SPEC §3.9 記録, §3.10 チェンジセット (P17): each sends the matching `ChangeSet class>>note…` to
// the Smalltalk model as an outermost send on the session's base context. Nothing without a
// session, for another heap (a test's own Boot, the CLI), for an empty class name, or without a
// ChangeSet class. A send that fails (doesNotUnderstand:, error:, out of memory) is dropped: the
// Accept or removal stands. Never throws. May collect: the caller reads its Oops again from
// rooted slots afterwards.
void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept;
void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept;
// existed: the name named a class (a Behavior) before the definition.
void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept;
void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept;

// SPEC §3.10 チェンジセット (P17): the ao_changes_* bodies, each an outermost send to the model
// (the ABI takes AbiEntry first). Without a ChangeSet class the reads answer no sets; when a send
// fails they answer as while busy. A write answers AO_OK, or AO_ERR with the reason in *reason
// (left empty when the ABI's message is the generic one: no session, a bad argument).
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
int sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector,
                       std::string* reason);
int sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta,
                       const char* selector, std::string* reason);
// SPEC §3.9 File Out: the model's text for the set, written to path (replacing it).
int sessionChangesFileOut(std::int64_t setId, const char* path, std::string* reason);

}  // namespace ao
```

- [ ] **Step 4: `ChangeSet.cpp` を置き換える**

骨格（下のとおりに書き、読み出しと書き込みの残りも同じ形で埋める。どの関数も `Session* s = session(); if (s == nullptr || s->ctx == nullptr) …` で始める）:

```cpp
#include "ChangeSet.hpp"

#include "Session.hpp"
#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Runtime.hpp"
#include "ao/Send.hpp"

#include <ctime>
#include <fstream>
#include <initializer_list>
#include <string>
#include <string_view>

namespace ao {
namespace {

constexpr const char* kFailed = "change set failed";
constexpr const char* kUnavailable = "change sets unavailable";
constexpr const char* kUnknown = "unknown change set";

// SPEC §3.10 チェンジセット (P17): one outermost stretch of sends to the model on the session's
// base context. It clears what an earlier entry left and takes the stack range again; while it
// runs the base has no debug sink and may not halt, so a failure in the model neither replaces
// the snapshot nor stops a process (as DebugEntry). The end reads and clears the abort. After the
// first failed send the others answer the empty Oop without sending.
class ModelSend {
 public:
  explicit ModelSend(Session& s) : s_(s), ctx_(*s.ctx), debug_(ctx_.debug) {
    s_.heap.clearOutOfMemory();
    clearUnwinding(ctx_);
    refreshStackLimit(ctx_);
    ctx_.debug = nullptr;
    ++ctx_.haltSuppressed;
  }
  ~ModelSend() {
    clearUnwinding(ctx_);
    s_.heap.clearOutOfMemory();
    --ctx_.haltSuppressed;
    ctx_.debug = debug_;
  }
  ModelSend(const ModelSend&) = delete;
  ModelSend& operator=(const ModelSend&) = delete;

  // receiver and each arg are rooted slots, read after the selector is interned (which may
  // collect). The answer, or the empty Oop when the send aborted or ran out of memory.
  Oop send(const Oop& receiver, std::string_view selector,
           std::initializer_list<const Oop*> args = {}) {
    if (failed_) {
      return Oop{};
    }
    const Oop sel = s_.wk.intern(selector);
    if (!sel.isHeap() || args.size() > 4) {
      failed_ = true;
      return Oop{};
    }
    Oop argv[4];
    std::uint32_t n = 0;
    for (const Oop* a : args) {
      argv[n++] = *a;
    }
    const Oop answer = ao::send(ctx_, receiver, sel, argv, n, nullptr);
    if (answer.isEmpty() || unwinding(ctx_) || s_.heap.outOfMemory()) {
      failed_ = true;
      clearUnwinding(ctx_);
      return Oop{};
    }
    return answer;
  }
  // A String of text (may collect); the empty Oop and failed() when it cannot be made.
  Oop string(std::string_view text) {
    const Oop str = failed_ ? Oop{} : Str::fromUtf8(ctx_, text);
    failed_ = failed_ || !str.isHeap();
    return str;
  }
  bool failed() const { return failed_; }

 private:
  Session& s_;
  CallContext& ctx_;
  DebugSink* debug_;
  bool failed_ = false;
};

// SPEC §3.10 (P17): the ChangeSet class Smalltalk binds; the empty Oop when there is none.
Oop modelClass(Session& s) { return boundClassNamed(s, "ChangeSet"); }

bool bytesText(Session& s, Oop v, std::string* out) {
  if (!v.isHeap() || (s.heap.flags(v) & kFlagBytes) == 0) {
    return false;
  }
  *out = Str::toUtf8(s.heap, v);
  return true;
}

int kindCode(Session& s, Oop kind) {
  if (kind.isHeap()) {
    if (kind == s.wk.findSymbol("added")) return AO_CHANGE_ADDED;
    if (kind == s.wk.findSymbol("changed")) return AO_CHANGE_CHANGED;
    if (kind == s.wk.findSymbol("removed")) return AO_CHANGE_REMOVED;
  }
  return AO_CHANGE_NONE;
}

bool smallIntegerId(std::int64_t id) { return id > 0 && id <= kSmiMax; }

// The set with setId into *set (nil when the model has none). False when the send failed.
bool setWithId(ModelSend& m, const Oop& cls, std::int64_t setId, Root& set) {
  if (!smallIntegerId(setId)) {
    set.slot = Oop::nil();
    return true;
  }
  const Oop id = Oop::fromSmallInteger(setId);
  set.slot = m.send(cls, "withId:", {&id});
  return !m.failed();
}

// SPEC §3.9 記録: the session whose heap ctx runs on, when it has a model; null otherwise.
Session* recordingSession(CallContext& ctx, const std::string& className) {
  Session* s = session();
  if (s == nullptr || s->ctx == nullptr || &ctx.heap != &s->heap || className.empty()) {
    return nullptr;
  }
  return s;
}

// The local date, YYYY-MM-DD (P16's today(), moved as it is).
std::string today();

}  // namespace

void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept {
  try {
    Session* s = recordingSession(ctx, className);
    if (s == nullptr) {
      return;
    }
    Root cls(s->roots, modelClass(*s));
    if (!cls.slot.isHeap()) {
      return;
    }
    ModelSend m(*s);
    Root name(s->roots, m.string(className));
    Root sel(s->roots, m.string(selector));
    const Oop side = meta ? Oop::true_() : Oop::false_();
    const Oop existed = replaced ? Oop::true_() : Oop::false_();
    m.send(cls.slot, "noteMethodChange:selector:meta:existed:",
           {&name.slot, &sel.slot, &side, &existed});
  } catch (...) {
    // SPEC §3.9 記録: without the memory for it the record is not made; the Accept stands.
  }
}
```

ほかの 3 つも同じ形で書く（`noteMethodRemoval`: `noteMethodRemoval:selector:meta:` に `{&name.slot, &sel.slot, &side}`、`noteClassChange`: `noteClassChange:existed:` に `{&name.slot, &existed}`、`noteClassRemoval`: `noteClassRemoval:` に `{&name.slot}`）。`true` / `false` / nil / SmallInteger は即値か固定のオブジェクトで GC で動かないことを `ao/Oop.hpp` と `WellKnown` で確かめ、動くなら `Root` に入れる。

読み出し（例。ほかも同じ形）:

```cpp
int sessionChangesCount() {
  Session* s = session();
  if (s == nullptr || s->ctx == nullptr) {
    return -1;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return 0;
  }
  ModelSend m(*s);
  Root all(s->roots, m.send(cls.slot, "allChangeSets"));
  const Oop n = m.send(all.slot, "size");
  return n.isSmallInteger() ? static_cast<int>(n.smallIntegerValue()) : -1;
}

int sessionChangesAt(int index, std::int64_t* setId, char* name, int nameLen) {
  Session* s = session();
  if (s == nullptr || s->ctx == nullptr || index < 0 || name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  Root cls(s->roots, modelClass(*s));
  if (!cls.slot.isHeap()) {
    return AO_ERR;
  }
  ModelSend m(*s);
  Root all(s->roots, m.send(cls.slot, "allChangeSets"));
  const Oop n = m.send(all.slot, "size");
  if (!n.isSmallInteger() || index >= n.smallIntegerValue()) {
    return AO_ERR;
  }
  const Oop at = Oop::fromSmallInteger(index + 1);
  Root set(s->roots, m.send(all.slot, "at:", {&at}));
  Root text(s->roots, m.send(set.slot, "name"));
  const Oop id = m.send(set.slot, "id");
  std::string utf8;
  if (m.failed() || !id.isSmallInteger() || !bytesText(*s, text.slot, &utf8)) {
    return AO_ERR;
  }
  if (setId != nullptr) {
    *setId = id.smallIntegerValue();
  }
  return writeBuf(utf8, name, nameLen);
}
```

- `sessionChangesCurrent`: `ChangeSet current` → `id`。モデルが無い、失敗、SmallInteger でない → 0。
- `sessionChangesClassCount(setId)`: `setWithId`、nil → -1、`classChanges` → `size`。モデルが無い → -1。
- `sessionChangesClassAt`: 範囲を `size` で確かめ、`classChanges at: index + 1` → `className`（Symbol のバイト）と `definitionKind`（`kindCode`）。
- `sessionChangesMethodCount(setId, className, meta)`: `className == nullptr` か meta が 0/1 でない → -1。`setWithId`、nil → -1。`changesFor:`（className の String）→ nil なら 0、でなければ `selectorsMeta:`（true/false）→ `size`。
- `sessionChangesMethodAt`: 同じ道で `selectorsMeta:` の `at: index + 1` → セレクタ、`kindOf:meta:`（セレクタの Oop と true/false）→ `kindCode`。
- `sessionChangesNew(name, setId, reason)`: モデルが無い → `kUnavailable`。`name` が NULL なら `""`。`reasonToCreate:` → nil でなければその String を `*reason`（bytes でなければ `kFailed`）。nil なら `newNamed:` → `id` を `*setId`。どこかで失敗 → `kFailed`。
- `sessionChangesSetCurrent(setId)`: モデルが無い、nil、失敗 → `AO_ERR`。あれば `makeCurrent`。
- `sessionChangesDrop`: `className == nullptr` か（`selector != nullptr` で meta が 0/1 でない）→ `AO_ERR`（理由なし）。モデルが無い → `kUnavailable`。セットが nil → `kUnknown`。`selector == nullptr` なら `reasonToRemoveClass:` → nil なら `removeClass:`、でなければ理由。あれば `reasonToRemoveSelector:class:meta:` → `removeSelector:class:meta:`。失敗 → `kFailed`。
- `sessionChangesMove`: 同じ形で、2 つのセットのどちらかが nil → `kUnknown`。`reasonToMoveClass:to:` / `reasonToMoveSelector:class:meta:to:` → `moveClass:to:` / `moveSelector:class:meta:to:`。
- `sessionChangesFileOut(setId, path, reason)`: `path == nullptr` → `AO_ERR`（理由なし）。モデルが無い → `kUnavailable`。nil → `kUnknown`。`version_string` と `today()` を String にして `fileOutStringDate:version:` → bytes でなければ `kFailed`。`Str::toUtf8` した本文を P16 と同じく `std::ofstream`（binary、trunc）で書き、失敗は `cannot write file: <path>`。

- [ ] **Step 5: Session、Compile、ABI を直す**

- `Session.hpp`: `#include "ChangeSet.hpp"` と `ChangeSets changes;` とその注記を消す。`superclassNameOf` と `liveMethodSource` の宣言を消し、`Session.cpp` の本体も消す（参照がもう無いことを `find_referencing_symbols` で確かめてから。Serena の `safe_delete_symbol` を使う）。`boundClassNamed` の注記の「SPEC §3.9 File Out (P16): what the change set writer reads」を「the ChangeSet model's class and the File Out names (P16, P17)」に。
- `Compile.cpp`: `#include "ChangeSet.hpp"` を足す（`Session.hpp` 経由で来なくなる）。
- `Compile.hpp`: `removeMethodOf` / `removeClassOf` の「nothing here allocates on the heap」を「nothing before the change allocates on the heap; the record in the current change set (SPEC §3.9 記録) comes last and may collect」に。`applyChunks` などの FileInMode の注記の「recorded in the current change set」はそのまま。
- `abi.cpp`: 読み出し 7 つに `AbiEntry` を取らせる（拒まれたら `-1` / `AO_ERR` / `0`。`*_at` は既存どおり先に出力を空にしてから）。`ao_changes_current` は `guarded(std::int64_t{0}, …)`。「the reads take no AbiEntry and answer while busy」の注記を「SPEC §3.10 チェンジセット (P17): the reads send to the model, so they are outermost entries too and refuse while busy」に。`AbiEntry` の注記の列挙に change sets を足す。
- `bridge/ao_abi.h`: `ao_changes_*` の読み出しの注記を「busy なら拒む（`*_count` -1、`*_at` AO_ERR、`ao_changes_current` 0）」に。

- [ ] **Step 6: 緑を確かめる**

Run: `cmake --build build -j && ctest --test-dir build --output-on-failure -E '^KernelBench\.'`
Expected: PASS。P16 の `ChangesAbi.*`（記録、Drop / Move、File Out、File In）は Smalltalk のモデルで緑のまま。GC の下でも: `AO_GC_STRESS=1 ./build/runtime/ao_runtime_tests --gtest_filter='ChangesAbi.*'`（テストの実行ファイルの場所は `find build -name ao_runtime_tests`）。

- [ ] **Step 7: Serena の clangd を立ち上げ直し、コミット**

`pkill -f 'clangd.*ao-smalltalk'` などで Serena の clangd を落とし（メモリ `serena-clangd-restart`）、`get_symbols_overview runtime/src/ChangeSet.cpp` が新しい中身を返すことを確かめる。

```bash
git add runtime/src/ChangeSet.hpp runtime/src/ChangeSet.cpp runtime/src/Session.hpp runtime/src/Session.cpp \
  runtime/src/Compile.cpp runtime/include/ao/Compile.hpp runtime/src/abi.cpp bridge/ao_abi.h \
  runtime/tests/changes_abi_test.cpp
git commit -m "Keep change sets in the Smalltalk model and send to it from C++ (P17)" -m "Graphify: path noteMethodChange() send()
Serena: replace_symbol_body ao::sessionChangesCount; safe_delete_symbol ao::liveMethodSource, ao::superclassNameOf

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 6: アプリ（Tools-model の file-in、busy の表示、イメージを開いたあと）

**Files:**
- Modify: `app/Ao/AoApp.swift`（`toolsModelDirectory`、`fileInLoadOrder(at:label:transcript:)`、起動と Open Image… での file-in）
- Modify: `app/Ao/ChangeSorterModel.swift`（`readRefused`）、`app/Ao/ChangeSorterWindow.swift`（エラー欄）
- Modify: `app/AoTests/ChangeSorterTests.swift`、`app/AoTests/ToolWindowTests.swift`
- Modify: `scripts/package-app.sh`、`scripts/test.sh`

**Interfaces:**
- Consumes: Task 5 の ABI（読み出しが busy で -1 / `AO_ERR` / 0）。`ao_browser_class_id`、`ao_filein_load_order`。
- Produces: `func toolsModelDirectory(environment: [String: String], resources: URL) -> URL`、`@MainActor func fileInToolsModel(at dir: URL, transcript: TranscriptWindow)`、`ChangeSorterModel.readRefused: Bool`。

- [ ] **Step 1: Graphify と Serena**

`graphify query "fileInVendor openImage ChangeSorterModel refresh noteImageLoaded"`。Serena（Swift）: `find_symbol fileInVendor`、`vendorDirectory`、`AoApp/openImage`、`AoApp/applicationWillFinishLaunching`、`ChangeSorterModel/refresh`、`ChangeSorterWindow/reload`、`ChangeSorterWindow/noteImageLoaded`、それぞれの `find_referencing_symbols`。`scripts/test.sh` の `--app` の節（`WANT=` と `open --env`）を読む。

- [ ] **Step 2: 失敗するテストを書く**

`ChangeSorterTests.swift`:
- `boot()` を「`ao_runtime_boot` のあと `ao_filein_load_order(<repo>/image/Tools-model/LOAD_ORDER)` が `AO_OK`」にする。リポジトリのパスは `ToolWindowTests.swift:148` と同じく `#filePath` から（`app/AoTests/…` から 3 つ上）。補助 `toolsModelURL` を private static に。
- `setUp` で `setenv("AO_TOOLS_MODEL_DIR", toolsModelURL.path, 1)`、`tearDown` で `unsetenv("AO_TOOLS_MODEL_DIR")`（`launchApp()` が Tools-model を読むため）。
- 足すテスト:

```swift
  // SPEC §3.9 Change Sorter (P17): a refused read leaves the lists and says busy; the next read
  // after the evaluation shows the sets again and clears the line.
  func testBusyReadKeepsTheListsAndSaysBusy() {
    boot()
    let sorter = ChangeSorterWindow()
    sorter.reload()
    XCTAssertEqual(sorter.model.sets.map(\.name), ["Unnamed1"])
    whileBusy {
      sorter.reload()
      XCTAssertTrue(sorter.model.readRefused)
      XCTAssertEqual(sorter.model.sets.map(\.name), ["Unnamed1"])
      XCTAssertEqual(sorter.errorText, "runtime is busy")
    }
    sorter.reload()
    XCTAssertFalse(sorter.model.readRefused)
    XCTAssertEqual(sorter.errorText, "")
  }

  // SPEC §3.9 (P17): Open Image... shows the sets the image was saved with.
  func testOpenImageShowsTheSavedSets() {
    let app = launchApp()
    var err = AoSpan()
    var work: Int64 = 0
    XCTAssertEqual(ao_changes_new("Work", &work, &err), Int32(AO_OK))
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("p17-open.aoimage")
    XCTAssertTrue(app.saveImage(to: url))
    // Unnamed1 current and a third set, so the window shows what the load brings back.
    var first: Int64 = 0
    var name = [CChar](repeating: 0, count: 32)
    XCTAssertEqual(ao_changes_at(0, &first, &name, Int32(name.count)), Int32(AO_OK))
    XCTAssertEqual(ao_changes_set_current(first), Int32(AO_OK))
    var later: Int64 = 0
    XCTAssertEqual(ao_changes_new("Later", &later, &err), Int32(AO_OK))
    app.showChangeSorter()
    XCTAssertTrue(app.openImage(from: url))
    XCTAssertEqual(app.changeSorter?.model.sets.map(\.name), ["Unnamed1", "Work"])
    XCTAssertEqual(app.changeSorter?.model.currentName, "Work")
  }

  // SPEC §3.9 起動と同梱 (P17): an image without ChangeSet gets the tools model on Open Image...
  func testOpenImageWithoutChangeSetFilesInTheToolsModel() {
    // A bare boot (no tools model) writes an image without ChangeSet.
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("p17-bare.aoimage")
    XCTAssertEqual(url.path.withCString { ao_image_save($0) }, Int32(AO_OK))
    let app = launchApp()
    XCTAssertTrue(app.openImage(from: url))
    XCTAssertNotEqual("ChangeSet".withCString { ao_browser_class_id($0) }, 0)
    app.showChangeSorter()
    XCTAssertEqual(app.changeSorter?.model.sets.map(\.name), ["Unnamed1"])
  }

  // SPEC §3.9 起動と同梱 (P17): launch files the tools model in after the vendor.
  func testLaunchFilesInTheToolsModel() {
    _ = launchApp()
    XCTAssertNotEqual("ChangeSet".withCString { ao_browser_class_id($0) }, 0)
    XCTAssertEqual(ao_changes_count(), 1)
  }
```

（`errorText`（エラー欄の文字列を読む）、`app.changeSorter`、`showChangeSorter()`、`saveImage(to:)` の名前と可視性は既存のテストが使っているものに合わせ、無ければテストのための読み取り口を既存の流儀で足す。`launchApp()` の前に別のセッションが要るテストは、既存の `testOpenImageStartsAnOpenChangeSorterOver` の組み立てを手本にする。）

`ToolWindowTests.swift` の vendor のテストの隣に:

```swift
  func testToolsModelDirectoryHonoursTheEnvironment() {
    let resources = URL(fileURLWithPath: "/tmp/ao-resources", isDirectory: true)
    let bundled = resources.appendingPathComponent("Tools-model", isDirectory: true).path
    XCTAssertEqual(
      toolsModelDirectory(environment: ["AO_TOOLS_MODEL_DIR": "/tmp/ao-models"], resources: resources).path,
      "/tmp/ao-models")
    XCTAssertEqual(toolsModelDirectory(environment: [:], resources: resources).path, bundled)
    XCTAssertEqual(toolsModelDirectory(environment: ["AO_TOOLS_MODEL_DIR": ""], resources: resources).path, bundled)
  }
```

Run: `./scripts/build.sh && (cd app && swift test --filter 'ChangeSorterTests|ToolWindowTests')`（`scripts/test.sh` の `swift test` の呼び方（リンクの指定）をそのまま使う）
Expected: FAIL（`toolsModelDirectory`、`readRefused` が無い。`boot()` 以外の既存の Change Sorter のテストは Tools-model を読めば緑）

- [ ] **Step 3: 実装**

`AoApp.swift`:

```swift
/// SPEC §3.9 起動と同梱 (P17): the tools-model directory is AO_TOOLS_MODEL_DIR when it is not
/// empty, else `Tools-model` in the bundle's resources.
func toolsModelDirectory(environment: [String: String], resources: URL) -> URL {
  if let dir = environment["AO_TOOLS_MODEL_DIR"], !dir.isEmpty {
    return URL(fileURLWithPath: dir, isDirectory: true)
  }
  return resources.appendingPathComponent("Tools-model", isDirectory: true)
}
```

`fileInVendor(at:transcript:)` の本体を `fileInLoadOrder(at dir: URL, label: String, transcript: TranscriptWindow)` に移し（行は `"\(label) not found: …"`、`"\(label) loaded: …"`、`"\(label) file-in failed: …"`）、`fileInVendor` は `fileInLoadOrder(at: dir, label: "vendor", transcript:)`、`fileInToolsModel` は `label: "tools model"` を呼ぶだけにする（既存の行の文言は変わらない）。

`applicationWillFinishLaunching` で vendor のあとに `fileInToolsModel(at: toolsModel, transcript: started.transcript)`（`toolsModel` は `toolsModelDirectory(environment:resources:)`。`AoApp` のプロパティに持って Open Image… でも使う）。`openImage(from:)` で成功のあと、`noteImageLoaded` の前に:

```swift
    // SPEC §3.9 起動と同梱 (P17): an image saved without the change set model gets it now.
    if "ChangeSet".withCString({ ao_browser_class_id($0) }) == 0, let transcript = launch?.transcript {
      fileInToolsModel(at: toolsModel, transcript: transcript)
    }
```

`ChangeSorterModel.swift`:

```swift
  // SPEC §3.9 Change Sorter (P17): the last refresh was refused (busy, or the model failed); the
  // lists are as before it.
  private(set) var readRefused = false

  func refresh() {
    guard ao_changes_count() >= 0 else {
      readRefused = true
      return
    }
    readRefused = false
    … 既存の本体 …
  }
```

（`loadSets()` が `ao_changes_count()` をもう一度読むのはそのままでよい。）

`ChangeSorterWindow.swift` の `reload()`:

```swift
  func reload() {
    model.refresh()
    if model.readRefused {
      errorField.stringValue = "runtime is busy"
    } else if errorField.stringValue == "runtime is busy" {
      errorField.stringValue = ""
    }
    show()
  }
```

（`noteImageLoaded` は既存のまま。`errorText` が要れば `var errorText: String { errorField.stringValue }` をテストの読み取り口として足す。）

`scripts/package-app.sh` の vendor の行の後に `cp -R "$ROOT/image/Tools-model" "$APP/Contents/Resources/Tools-model"`。`scripts/test.sh` の `--app`: `open` に `--env AO_TOOLS_MODEL_DIR=` を足し、vendor の行を待つ `WANT` のあとで、同じ待ち方で `ao: tools model loaded: <bundle>/Contents/Resources/Tools-model` も待つ（既存の待ちの書き方をそのまま 2 行目に広げる。コメントも 1 行足す）。

- [ ] **Step 4: 緑を確かめる**

Run: `./scripts/test.sh`（C++ と Swift のすべて）。`--app` があれば `./scripts/test.sh --app` も。
Expected: PASS

- [ ] **Step 5: コミット**

```bash
git add app/Ao/AoApp.swift app/Ao/ChangeSorterModel.swift app/Ao/ChangeSorterWindow.swift \
  app/AoTests/ChangeSorterTests.swift app/AoTests/ToolWindowTests.swift scripts/package-app.sh scripts/test.sh
git commit -m "File in the tools model at launch and on Open Image; show busy reads (P17)" -m "Graphify: query fileInVendor openImage ChangeSorterModel refresh noteImageLoaded
Serena: replace_symbol_body fileInVendor, ChangeSorterModel/refresh, ChangeSorterWindow/reload

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 7: 完了の記録（版 1.6.0、PHASE、SPEC §6、グラフ）

**Files:**
- Modify: `PHASE`（`P17`）、`CHANGELOG.md`（1.6.0 の項目）、版の文字列（`grep -rn '1\.5\.0' runtime compiler app docs/README.md README.md README.ja.md` で出る所。P16 の Task 10 が触った所と同じ: `runtime/src/Runtime.cpp`、`compiler/src/Compiler.cpp`、両方の `smoke_test.cpp` など）、`SPEC.md` §6 の P17 の小節を `[x]`、`docs/phases/P17.md` の受け入れを `[x]`（手動確認は `[ ]` のまま）、`docs/README.md`（フェーズの一覧に P17）
- Modify: `graphify-out/`（`/graphify . --update`）

- [ ] **Step 1: 版と記録を更新する**

P16 の Task 10（`docs/superpowers/plans/2026-10-11-p16-change-sets.md` の「Task 10」）と同じ所を同じ書き方で 1.6.0 / P17 にする。CHANGELOG の項目: 「チェンジセットを Smalltalk のクラス `ChangeSet`（`image/Tools-model/`）にし、イメージに保存する」「`Behavior>>sourceCodeAt:`、`Class>>definition`」「`ao_changes_*` の読み出しは busy なら拒む」「`ao filein` / `ao image save` の `--load-order` を繰り返せる」「`ao --test` はテストの隣の Tools-model を先に読む」。

- [ ] **Step 2: 全体を緑にする**

Run: `./scripts/test.sh`
Expected: PASS（`KernelScan.*` を含む）

- [ ] **Step 3: グラフを更新してコミット**

`/graphify . --update`（`graphify update .`）。

```bash
git add PHASE CHANGELOG.md SPEC.md docs/phases/P17.md docs/README.md graphify-out runtime compiler app README.md README.ja.md
git commit -m "Record P17 completion, bump to 1.6.0, refresh the knowledge graph" -m "Graphify: update .
Serena: none (records only)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

（`git add` は実際に変えたファイルだけにする。`docs/Qiita*.md` は追跡外のまま触らない。）

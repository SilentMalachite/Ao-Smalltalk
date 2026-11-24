# review-agy 指摘の修正計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** `review-agy.md`（HEAD 346f382、v1.7.0、PHASE P18）の指摘のうち、検証で実在を確かめたものを、SPEC を先に直してから TDD で直す。

**Architecture:** 指摘はファイルとシンボルの持ち主ごとに 8 バッチに分ける。1 バッチを 1 ブランチ・1 PR にし、main へ順に入れる。挙動が変わるバッチは、最初のコミットで SPEC.md を直す。

**Tech Stack:** C++20 runtime/compiler（GoogleTest）、Swift 6 AppKit（XCTest）、Smalltalk（`ao --test image/tests`）。

**Spec:** `SPEC.md`（製品）、`CLAUDE.md`（手順）、`review-agy.md`（指摘の原文）。

---

## 結論

- 指摘 40 件（Critical 2 / High 12 / Medium 26）と主な Low を、6 領域に分けて並行で検証した。ソースは変えていない。多くは、既存の `build/` のライブラリにリンクしたプローブか `build/ao` で再現した。読んで確かめただけのものは、表の下に挙げる。
  - 全面的に却下した指摘は無い。
  - 一部を却下したものは 13 件（下の表の「一部」）。
  - 重要度を付け直した結果は Critical 1、High 3、Medium 15、Low〜Medium 3、Low 18。
- Critical として残るのは C1 だけである。`!Smalltalk methodsFor:` のチャンク 1 つで `Object` のスーパークラスとメソッド辞書が壊れ、しかも `AO_OK` が返る。
- 新たに High に上げたのは M1 と M15 の 2 件。どちらも反射（`instVarAt:put:`）を使わずに起きる。
  - M1: Kernel のネイティブを CompiledMethod で置き換えられる。
  - M15: OrderedCollection の生成で範囲外に書き込む。
- C2 は Medium に下げた。SPEC §3.6（574 行）が、この壊れ方を v1 の既知の制約にしているからである。
- PHASE は `P18` のまま変えない。どのバッチも既存機能の欠陥修正である。新しい依存ライブラリは足さない。
- 決めてもらう点が 6 つある（下の D1〜D6）。各バッチは推奨案で書いた。推奨と違う案を選ぶと、そのバッチの SPEC 文と赤テストだけが変わる。

## 前提（検証で確かめたこと）

- **反射**: `Object>>instVarAt:put:`（`runtime/src/kernel/Object.cpp:312`）が拒むのはバイト列だけである。クラスの `format`、`superclass`、`methodDict`、`thisClass` は、Kernel クラスのものでも Smalltalk から書き換えられる。
  - SPEC.md:574 は、この壊れ方を既知の制約にしている（「ネイティブは壊れうる」）。
  - 一方、669・715・805 行の同じ制約には「メモリの安全だけを保つ」という約束が付いている。
- **空 OOP**: ネイティブが失敗すると、`send()` は巻き戻し中でなくても空 OOP を返す。
  - 確立した書き方は `if (unwinding(ctx) || v.isEmpty()) return Oop{};`。こう書くと、外側の `performSend` → `stopFailedSend` → `abortFailedSend` が、`out of memory` か `failed: #sel` で中断する。
  - 外側の評価は、評価の終わりに OOM の印を見る（`Session.cpp:1376`、`Scheduler.cpp:955/995`）。だから、OOM を飲み込むだけのバグも最後には報告される。
  - 実害は 2 つ。失敗のあとの文が実行され続けることと、空 OOP がスロットに残ることである。
- **RunLoopPump**: インタプリタは後方ジャンプと活性化の検査点で `maybePumpRunLoop()` を呼ぶ（`Interpreter.cpp:268, 626`）。
  - Smalltalk を走らせる ABI 呼び出しはどれも、AppKit のイベントを配送しうる。`ao_debug_*_print`、`ao_debug_inspect`、`ao_debug_abort`、`ao_accept_*`、`ao_filein_file`、`ao_changes_*` がそうである。
  - アプリ側の busy 表示（`EvaluationActivity`）が立つのは、`ao_eval` と Proceed / Step の間だけである。
- **グラフ**: `graphify-out/` は 929374c から作ってある。HEAD との差は版の文字列とテストだけである（`Compiler.cpp`、`Runtime.cpp`、`changes_abi_test.cpp`、`smoke_test.cpp`）。
- **テストの道具**:
  - OOM: `Boot(nursery, old, oldMax)` で上限を決め、`fillOld` / `fillNurseryTo` / `setGcStress(0)` で埋める（`gc_safety_test.cpp:280, 744`）。
  - C++ の確保失敗: `failSlotGrowthForTesting(k)`。
  - アプリの pump 再入: `TranscriptBox` / `transcriptBoxHook`（`DebuggerWindowTests.swift:1192-1215`）と `interruptOncePump`（:1184）。
  - 破損イメージ: `ImageSurgery` / `expectRefused`（`image_save_load_test.cpp`）。

## ユーザー決定待ち

| # | 決めること | 推奨 | 推奨と違う案を選んだときに変わるもの |
|---|---|---|---|
| D1 | SPEC:574 の既知の制約に「メモリの安全だけを保つ」を足すか（C2、NativeMethod の Low、R1 の `classFormat`） | 足す。669・715・805 行と同じ扱いにする。結果は規定しない | 足さないなら R5 は無くなる（C2 は既知の制約のまま残す） |
| D2 | M24 `InlineCache` を削除するか | 削除しない。契約をヘッダのコメントに書くだけにする（`native_send_test.cpp` が `&ic` を渡すので assert は入れない）。削除は SPEC §1.3（48 行「インラインキャッシュ 1 段」）の変更と約 120 か所の書き換えになるので、別の作業にする | 削除するなら、R8 に SPEC §1.3 の改訂と全呼び出し元の書き換えが入る |
| D3 | M6: 壊れた Semaphore / Process のリストをどう扱うか | 送信から入るネイティブ（`signal`、`wait` など）は `damaged ordered collection` で失敗する（SPEC 715 と同じ理由）。送信の無い内部の走査（`adoptImage`）は空として扱う | すべて空として扱うなら、R4 の SPEC 文と `Process.SignalOnDamagedLinkedListEnds` の期待値が変わる |
| D4 | M12: `size` が Integer でないときの `isEmpty` | Integer なら `isZero` で Boolean を答える。Integer でなければ失敗する（`failed: #isEmpty`）。`ArrayedCollection>>do:` も同じ規則にする | Blue Book どおり `size = 0` を送るなら、R4 の SPEC 文と期待値が変わる |
| D5 | H5: 空 OOP のスロットを持つイメージ | ロードは `damaged image` で拒む。保存は、既存の「ロードの検査を保存の前に通す」仕組み（`Image::check`）によって、オブジェクト名と規則を示して拒む。1.7.0 で保存したイメージにも、OOM のあとだけ空 OOP が入りうる | 読むときに nil へ直すなら、R7 の SPEC 文、テスト、`acceptWord` の分岐が変わる |
| D6 | M2: 擬変数と同じ名前のインスタンス変数 | コンパイラで隠す。`isSelfishReturn` と `super` の判定で、インスタンス変数を先に見る。SPEC は変えない（869 行の解決順のとおり） | クラス定義で拒むなら、SPEC:869 の改訂と `subclass:` の検査が R1 に入る |

## 検証結果と重要度

「一部」は、指摘の一部の経路を却下したもの。

| ID | レビュー | 検証後 | 判定と理由（要点） |
|---|---|---|---|
| C1 | Critical | **Critical** | 実在。指摘より広い。反射は不要。`!Smalltalk methodsFor:` が `AO_OK` を返して Object を壊す。`!Processor class methodsFor:` と、メタクラスを指す別名の `A class` も Kernel に入る |
| C2 | Critical | Medium | 実在。反射が必要で、SPEC:574 の範囲（約束が抜けている）。ポインタの偽造まで再現した |
| H1 | High | **High** | 実在。印字中に閉じると、停止プロセスが Open Image… か終了まで残り、Save が拒まれ続ける |
| H2 | High | Medium | 実在。同じ Workspace の Inspect it から開いた Debugger に限られる。Inspector が壊れるだけ |
| H3 | High | Low | 実在。戻り値の契約違反だけで、アプリは倍化で吸収している（`DebuggerWindow.swift:61`） |
| H4 | High | Medium | 実在。停止中にヘッダを書き換えると、Accept で SIGSEGV / ASan の範囲外読みになる |
| H5 | High | Medium | 実在。読めてしまうが、クラッシュは無い |
| H6 | High | Low | 実在。中断のあとの最初の Step が空振りする |
| H7 | High | Medium（一部） | `bad_alloc` の経路は実在の UAF（注入で ASan が確認）。後始末中の Accept の経路は却下 |
| H8 | High | Low（一部） | 呼び出し元が濾していて落ちない。ただし取りこぼしが別にある（`Behavior.cpp:24` の `classFormat`） |
| H9 | High | Low（一部） | 不正コードになるのは、カスケードの 2 番目以降の部分だけ。メモリは安全 |
| H10 | High | Medium | 実在。OOM のときだけ |
| H11 | High | Medium | 実在。OOM のときだけ |
| H12 | High | Medium（一部） | OOM なしで起きる実バグがある: `Interval do:` の失敗を `select:` などが飲み込む。挙げられた 3 か所は理論上だけ、1 か所は却下 |
| M1 | Medium | **High** | 実在。反射は不要。`subclass:` の上書きが Object を答えると、`printString` が乗っ取られる |
| M2 | Medium | Low | 実在。ほかに `super` の判定（`Parser.cpp:366, 426`）にも同じ穴がある |
| M3 | Medium | Low〜Medium | 実在。vendor は影響を受けない |
| M4 | Medium | Low（一部） | 狭い OOM の理由違い |
| M5 | Medium | Low（一部） | 飲み込みではなく、報告が遅れるだけ |
| M6 | Medium | Medium（一部） | 実在のハング（`s signal` が戻らない）。オーバーフローと切り詰めは無害 |
| M7 | Medium | Medium | 実在。DNU のセレクタを定義するとフレーム番号がずれ、Restart が別のフレームに当たりうる |
| M8 | Medium | Medium | 実在。キャッシュ経由で、Workspace だけが持つクラスが保存される。保存が `AO_ERR` になることもある |
| M9 | Medium | Low〜Medium | 実在。Browser の読み出し 7 関数 |
| M10 | Medium | Low | 実在（読んで確認）。レビューの分割案は UTF-8 を切ってしまう |
| M11 | Medium | Low（一部） | 順序の件は却下（SPEC:1153 と P16 の設計どおり）。nil 名は反射のときだけ |
| M12 | Medium | Medium | 実在。`ArrayedCollection>>do:` にも同じ穴がある |
| M13 | Medium | Low（一部） | 範囲外にはならない。`subclass:` でゴミの名前ができる |
| M14 | Medium | Medium | 実在。65533 個の変数で instSize が 0 になり、範囲外に書き込む |
| M15 | Medium | **High**（一部） | 形の半分は実在（反射なしで範囲外に書き込む）。OOM の半分は却下（nil の array は空。SPEC 713、16ae68e の設計） |
| M16 | Medium | Low（一部） | 768 MiB 規模でないと起きない。`box` の切り詰めは到達しない |
| M17 | Medium | Low | 機能だけの問題で、メモリは安全 |
| M18 | Medium | Medium | 実在 |
| M19 | Medium | Low〜Medium | 実在。編集は消えない |
| M20 | Medium | Medium | 実在 |
| M21 | Medium | Low（一部） | テストでしか起きない |
| M22〜M25 | Medium | Low | 潜在（いまは発火しない）。レビューの見立てどおり |
| M26 | Medium | Low（一部） | (a) と (b) は実在の潜在欠陥。(c) は `scanShapes` が却下、`containsHostWord` が実在、`firstLineKey` は確認できない |

**読んで確かめただけの項目**（プローブで再現していない）:
- H1、H2、M18〜M21 と app の Low（Swift はビルドも実行もしていない）
- H10、H11、M4、M5、M10、M16、M22〜M25、M26a/b、`pushHandle`

それ以外はプローブで再現した。C1 は、この計画の作成時にも別に再現した。Tools-model に `!Smalltalk methodsFor:` のチャンクを足すと、`ao --test` で `Object superclass` が `doesNotUnderstand: #superclass` になる。

読んで確かめただけの項目は、各バッチの赤テストが赤になることを、修正の前に必ず確かめる。赤にならなければ、その項目は直さずに報告する。

## 範囲外（却下・見送り）

| 項目 | 扱い | 根拠 |
|---|---|---|
| H7 の後始末中の Accept | 却下 | `runAside` は restarting 中は走らない。`abortSetAside>0` の間は `canHalt` が false。ASan のプローブで 3 フレームとも正常 |
| M11 の祖先の順序 | 却下 | SPEC:1153 の文言どおりで、P16 計画 2342-2361 の設計 |
| M15 の OOM の半分 | 却下 | SPEC 713「`array` が nil なら空である」。直すと意図した設計を覆す |
| vendor スタブの `rebind` とキャッシュ（Low） | 却下 | `Behavior.cpp:367-370` がキャッシュ全体を捨てている |
| `sourcesFitFile`、`capacityFor`、`position:` の負値、SmallInteger / Magnitude の safepoint、`forwardTranscriptClass` | 却下 | 既存の検査や SPEC（774、606）で満たされている |
| 他プロセスへの非局所リターンの理由（Low） | 却下 | SPEC:1425「理由が無いときは `evaluation failed`」 |
| `package-app.sh:49`、`ci.yml:35-36` | 却下 | SPEC:1930（検証は `test.sh --app`）、SPEC:1962（`--asan` と `--app` は CI で回さない） |
| `#a:b` を受理する（Low） | 見送り | SPEC にシンボルの文法が無い。欠陥ではない |
| Magnitude の非 Boolean の素通し、`timesRepeat:` で LargeInteger（Low） | 見送り | SPEC が規定していない（742、743、728）。入れるなら、SPEC で意味を決めることから始める |
| Bytes と Weak の同時設定（Low） | 見送り | 無害。本番コードは Weak を作らない。禁じるには SPEC 1653 の改訂が先に要る |
| `firstLineKey` の衝突（M26c） | 見送り | 上流の入力がリポジトリに無く、実データで確かめられない |
| Browser の Hierarchy 表示での選択（Low） | 見送り | 見た目だけで、実行して確かめていない |

---

## Global Constraints

- PHASE は `P18` のまま。`SPEC.md` §2.3 のフェーズを足さない。
- 依存ライブラリを足さない（SPEC §5）。
- Kernel のメソッドは `NativeMethod` のまま（`KernelScan.*` が緑）。Kernel の `.st` に実行定義を書かない。
- ネイティブのシンボル名は `ao_<Class>_<selectorMangled>`。新しいネイティブは作らない。
- エラー文字列は、各バッチの「SPEC を先に直す」に書いた文字列をそのまま使う。
- コミットは、リポジトリの慣習どおり英語の短い命令形（例 `Refuse methodsFor: on a non-class global`）。本文に `Graphify:` と `Serena:` のトレーラを付ける（CLAUDE.md「コミット」）。
- CHANGELOG はリリース準備でまとめて書く（1.7.0 の 485a05b と同じ）。各バッチでは触らない。

## 全バッチ共通の手順（CLAUDE.md 標準ワークフロー）

1. 前のバッチを main にマージしてから、main から `fix/agy-rN-<slug>` を切る。worktree は使わない（Serena の編集は main の作業ツリーに当たる）。この計画書は、R1 の最初のコミットに含める（2026-09-23 の計画と同じ）。
2. Graphify: `GRAPH_REPORT.md` を読み、対象シンボルについて `graphify path` / `explain` を取る。R1 の最初に `graphify update .` を実行する。
3. Serena: 対象シンボルごとに `find_symbol` → `find_referencing_symbols` を取ってから、`replace_symbol_body` / `replace_content` で編集する。
4. SPEC: バッチの最初のコミットで、本文の該当節と、§4 のテスト一覧（テスト名）を直す。
5. 赤:
   - 下に書いたテストを先に書き、`--gtest_filter` で 1 件ずつ流して赤を確かめる。
   - Debug ビルドでは、範囲外アクセスが `slotAt` の assert になり、gtest のバイナリごと落ちる。これも赤として扱う。
   - 修正前に止まらない赤（M6 のハング）は、`timeout 10` を付けて流す。
   - アサーションは拒否の値（理由の文字列、`AO_ERR*`、答えの値）で書く。Release では assert が無く、黙って範囲外を読むからである。
6. 緑（すべてを満たして完了）:
   - `scripts/test.sh`（ctest と swift test）
   - `build/ao --test image/tests`
   - `AO_GC_STRESS=1 build/runtime/ao_runtime_tests --gtest_filter='-KernelBench.*:*Interrupt*'`
     - 割込ループの試験は、stress 下で 1 件に数分かかるので外す。外れるのは SPEC:1879 の P13 の試験、`LiveRestart.RestartFromInterrupt`、R3 で足す `LiveStep.InterruptThenStepOverAdvances` など。これらは通常の ctest と、R8 の最終確認で流れる。
     - 所要時間はまだ測っていない。R1 の最初に 1 度流して時間を記録し、PR 本文に書く。
   - R1、R3、R4、R5、R7、R8 は `scripts/test.sh --asan` も流す（`build-asan` は 10/12 のままなので作り直す）
7. `graphify update .` を実行し、PR 本文に対応した ID（C1、M15 など）を並べる。

## Review Focus

どのタスクのテストもまだ当たっておらず、利用者が踏みやすいものを並べる。各行のテストは、担当するバッチに入れてある。

1. **1.7.0 で保存したイメージ**: H5 のあと、空 OOP を持つイメージは読めなくなる（D5）。保存した直後のイメージ（新規、セッション、vendor 入り）がロードの検査に通ることを確かめる。R7 `ImageSaveLoad.FreshSessionAndVendorImagesHaveNoEmptyWords`。
2. **別名経由の削除**: C1 と同じく、別名（`Smalltalk at: #A put: Foo class`）経由の `ao_remove_method("A", 1, sel)` で、Kernel の側のメソッドを消せないこと。R1 `RemoveThroughMetaclassAliasLeavesKernelIntact`（いま緑なら固定のためのテストとして残す）。
3. **ウィンドウの閉じ方**: ⌘W と閉じるボタンは、どちらも `windowShouldClose` を通る。`performClose(nil)` のテストで両方を代表させ、`close()`（プログラムから閉じる経路）は別のテストで固定する。R2。
4. **アプリが持つフレーム番号**: M7 の修正のあとで、アプリがキャッシュしたフレーム番号のまま Restart / Accept しても、正しいフレームに当たること。R3 のランタイムのテストで、件数が変わらないことを固定する。
5. **新しいテストを GC ストレス下で流す**: 共通手順 6 の gcstress を、新しいテストにも適用する。空 OOP とルートの修正は、GC がまたぐときだけ壊れる。

---

## バッチの順序と依存

```
R1 Kernel 保護 ─ R2 App ─ R3 デバッガのランタイム ─ R4 失敗の伝搬 ─ R5 反射のメモリ安全(D1)
                                                         │
                                       R6 コンパイラ ────┴─ R7 イメージ(H5) ─ R8 ABI・GC の潜在欠陥
```

- R1 が最初なのは、唯一の Critical と、反射なしで起きる High 2 件を含むからである。
- R2（アプリ）はランタイムに依存しない。High の H1 を含むので早めに入れる。
- R3 と R4 は、どちらも `Scheduler.cpp` を触る（`runFiber` と `oc*`）。
- R7 の H5 は、R4（H10〜H12、M25）と R6（不正な文字リテラル）のあとに入れる。そうしないと、空 OOP や不正な Character がスロットに残ったまま保存が失敗する。

---

## R1 クラス定義と `methodsFor:` の Kernel 保護（C1、M1、M15、M14、M13、H8、Low のサブクラス判定）

**Files:**
- `runtime/src/Compile.cpp`（`applyMethodsFor`、`hidesNative`、`installMethod`、`reshapeClass`、`hasSubclass`）
- `runtime/src/kernel/Behavior.cpp`（`ao_Class_subclass_…`、`classFormat`）
- `runtime/src/kernel/Dictionary.cpp`（`ao_OrderedCollection_new`）
- `runtime/src/WellKnown.cpp`（`isClassObject`）
- `runtime/src/kernel/BlockContext.cpp`（`Str::toUtf8`）、`runtime/src/Symbol.cpp`（`Symbol::bytes`）
- `runtime/include/ao/Lookup.hpp`（`hasSlots` を `Process.cpp` の無名名前空間からここへ移す）
- Test: `chunk_filein_test.cpp`、`accept_abi_test.cpp`、`hashed_collection_test.cpp`、`behavior_test.cpp`

### SPEC を先に直す

- §3.10（1348 付近）と §3.12（1691、1699）に次を足す。
  - 名前で引いた先が Behavior でなければ（`Smalltalk`、`Processor`、String を入れたグローバル）、`methodsFor:` はチャンク単位のエラー `missing class: <name>` になる。
  - `class` を付けたチャンクと `meta = 1` は、メタクラスの側を対象にする。対象の側が Kernel クラスか Kernel クラスのメタクラスなら、名前で引いた先が Kernel でなくても Kernel として拒む。例: `A` が `Foo class` を指すとき、`A class` は Metaclass を指す。
- §3.9（1045）: 上書きが答えたクラスが Kernel クラスなら、名前を元のクラスに戻し、何も変えずに `AO_ERR_COMPILE` を返す。理由は、ほかの形の変更の拒否（`reshapeClass`、SPEC:587）と同じ接頭辞の `shape change refused: <クラス名> answered kernel class <答えたクラスの名前>`。
- §3.6（568 付近の `subclass:` の段落）に次を足す。
  - instSize（継承分を含む）が 65535 を超えるなら、クラスを作らずに評価を中断する。理由は `too many instance variables`。この検査は、重複の検査より前にする。
  - instanceVariableNames がバイト列（String か Symbol）でなければ失敗する（`failed: #subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:`）。
- §4.1 に下のテスト名を足す。

### 項目

- [ ] **C1** `applyMethodsFor`、`hidesNative`、`installMethod`
  - 赤:
    - `ChunkFileIn.ChunkLevelErrorsCarryTheChunkSpan` に 2 行足す。`Smalltalk methodsFor: 'x'` → `missing class: Smalltalk`、`Processor class methodsFor: 'x'` → `missing class: Processor`。
    - 新しく `ChunkFileIn.MethodsForNonClassGlobalLeavesObjectIntact` を作る。`Smalltalk at: #S put: 'abc'` のあとの `!S methodsFor:` は `missing class: S`。適用後も `Object superclass isNil` が true。
    - 新しく `AcceptAbi.MetaclassAliasCannotHideKernelNative` を作る。`Smalltalk at: #A put: Foo class` のあと、`ao_accept_method("A", 1, "printString ^'x'")` は `AO_ERR_COMPILE` で、理由は `native selector overwrite refused: printString`。チャンク `!A class methodsFor: 'x'!` は `refusing to redefine kernel class: A`。
    - Review Focus 2: `AcceptAbi.RemoveThroughMetaclassAliasLeavesKernelIntact`。
  - 修正:
    - `applyMethodsFor` の冒頭で `isBehavior(ctx, cls.slot)` を検査する（`Compile.cpp:575`。前方宣言を足す）。偽なら `missing class:`。
    - `target` の計算を `refusesKernelRedefinition` の前に移し、判定を `isKernelClass(wk, cls) || isKernelClass(wk, tgt)` にする。
    - `hidesNative` も同じ 2 項で判定する。これで `acceptInto`（`ao_accept_method`）の経路も塞がる。
    - `installMethod` の冒頭で、`isClassShaped(heap, tgt)` でなければ false を返す。
- [ ] **M1** `reshapeClass`
  - 赤: `AcceptAbi.ShapeChangeRefusesAnsweredKernelClass`。手本は `RefusedShapeChangeLeavesTheAnsweredClassUntouched`（:1906）。
    - クラス側の `subclass:…` が `Smalltalk at: n put: Object. ^Object` のとき、`printString` を持つ `AoM1Foo` を変数を変えて再 Accept する。
    - 期待: `AO_ERR_COMPILE`、理由は `shape change refused: AoM1Foo answered kernel class Object`。`(Object compiledMethodAt: #printString) class == NativeMethod`。`Smalltalk at: #AoM1Foo` は旧クラスを指す。
  - 修正: `fresh` の形の検査の直後で、`isKernelClass(ctx.wk, fresh)` なら `rebindClassName` で名前を戻して拒む。辞書には何も書かない。
- [ ] **M15** `ao_OrderedCollection_new`
  - 赤: `HashedCollection.NewAnswersForeignBasicNewUntouched`。
    - クラス側の `basicNew` が `^Array new: 1` のとき、`X new` はその Array を答え、`size = 1` で落ちない。
    - `^'ab'` のときも同じ。
  - 修正: `basicNew` の答えが `hasSlots(heap, obj, kOcLast)` でなければ、スロットに触らずにそのまま答える（`ao_Semaphore_new` の前例と同じ）。array を確保できなかったとき（OOM）の扱いは今のまま（nil の array は空）。
- [ ] **M14** `ao_Class_subclass_…`
  - 赤: `Behavior.SubclassBeyondInstSizeLimitAborts`。`OrderedCollection subclass:` に 65533 個の名前を渡すと、理由は `too many instance variables` で、クラスはできない。
    - 上限の内側（65535 ちょうど）は、重複の検査が O(n²) で遅い（Debug で約 29 秒）ので試験しない。
  - 修正: `superInst + ivars.size() > Format::kInstSizeMask` なら、重複の検査の前に `abortEvaluation`。
- [ ] **M13** `Str::toUtf8`、`Symbol::bytes`、`subclass:`
  - 赤: `Behavior.SubclassWithNonStringInstanceVariableNamesFails`。`instanceVariableNames: (Array with: Object)` は上の理由で失敗し、クラスはできない。
  - 修正:
    - `Str::toUtf8` と `Symbol::bytes` は、`kFlagBytes` の無いヒープオブジェクトに空を答える。
    - `subclass:` は、`args[0]` と同じく、`args[1]` がバイト列でなければ `Oop{}` を返す。
    - Serena で 2 関数の全参照を取り、空を答えたあとの挙動が変わる呼び出し元が無いことを確かめる。
- [ ] **H8** `WellKnown::isClassObject`、`Behavior.cpp` の `classFormat`
  - 赤: `Behavior.BasicNewOnByteObjectWithMetaclassClassFails`。
    - `AoP8 class instVarAt: 3 put: 196608` のあと、`(AoP8 class basicNew: 16) basicNew` は `failed: #basicNew`。
  - 修正:
    - `isClassObject` を、`Compile.cpp:564` の `isClassObject` と同じ判定にする。判定は `isClassShaped(obj)`、`isClassShaped(meta)`、`klass(meta) == metaclassClass`、`thisClass == obj` の 4 つ。
    - `classFormat` は、`isClassShaped(heap, cls)` でなければ `Oop{}` を答える。
- [ ] **Low サブクラス判定** `reshapeClass`（:1003）
  - 赤: `AcceptAbi.ShapeChangeRefusedWhenOnlyAGrandchildRemains`。
    - AoLF←AoLC←AoLG で、`AoLC class instVarAt: 5 put: nil` のあとに AoLF を再 Accept すると、`shape change refused: AoLF has subclasses`。
  - 修正: `probeSubclasses(ctx, old.slot).any` で判定する。呼び出し元が 1 つだけの `hasSubclass` は消す。

## R2 App: Debugger の窓、ツールの窓、スクリプト、CI（H1、H2、M18〜M21、app の Low）

**Files:**
- `app/Ao/DebuggerWindow.swift`、`app/Ao/WorkspaceWindow.swift`、`app/Ao/BrowserWindow.swift`
- `app/Ao/ChangeSorterWindow.swift`、`app/Ao/ChangeSorterModel.swift`
- `app/Ao/TranscriptWindow.swift`、`app/Ao/LaunchSet.swift`
- `app/AoTests/*.swift`
- `scripts/test.sh`、`.github/workflows/ci.yml`

### SPEC を先に直す

- §3.9（1000）: 「評価の実行中（pump が…）」を「ランタイムが busy の間（ABI 呼び出しの中で pump が窓の閉じる操作を配送したとき）」に改める。
- §3.9（1000）に次を足す。窓は閉じる前に `ao_debug_abort` を呼ぶ。断られて、まだ止まっているなら窓を閉じない。
- §4.3 に下のテスト名を足す。

### 項目

- [ ] **H1** `DebuggerWindow.windowShouldClose(_:)`、`windowWillClose(_:)`
  - 赤（`DebuggerWindowTests`。`TranscriptBox` を手本にする）:
    - `testClosingLiveDebuggerDuringItsPrintStringKeepsIt`: `printString` が `Transcript show: 'p'. ^'slow'` で、Transcript のフックが `debugger.window.performClose(nil)` を呼ぶ。`variable(at: 0)` のあとも `window.isVisible` で、`ao_debug_halted_count() == 1`。
    - `testClosingLiveDebuggerDuringAnotherAbortKeepsIt`: Debugger A の Abort の `ensure:` の中で B を閉じる。B は残る。
  - 修正:
    - `windowShouldClose`（編集の確認のあと）が `ao_debug_abort(pid)` を自分で呼ぶ。
      - `AO_OK` なら `finished = true` にして閉じる。
      - 断られて、`ao_debug_select(pid) == AO_OK`（まだ止まっている）なら閉じずに `updateButtons()`。
      - それ以外は閉じる。
    - `aborting` フラグを足し、Abort の `ensure:` の中から pump されて 2 度目に閉じようとしたときは false を返す。
    - `windowWillClose` の abort は、プログラムから `close()` したときのためだけに残す。
    - ライブでない Debugger（`isLive == false`）の閉じ方は変えない。
    - 却下するレビュー案: アプリ側の busy カウンタ（SPEC 1244「busy の判定は 1 か所にまとめる」に反する）と、メニューの灰色化（SPEC 943 の範囲外）。
- [ ] **H2** `DebuggerWindow.resume(_:refused:)`、`inspectVariable(at:)`
  - 赤: `testProceedDuringInspectItDrainKeepsInspectorResult`。
    - フックが `debugger.proceed()` を呼び、ソースは `[Transcript show: 'x'] fork. 3`。
    - Inspector は 1 枚で、`SmallInteger` を示し、停止数は 1 のまま。
  - 修正:
    - `EvaluationActivity.isActive` なら、`around`（`withInspectHook`）を通さずに `call` を呼ぶ。ランタイムは busy で断るので、そのあとの `refused:` と `updateButtons()` の経路は今のまま通る（Accept の拒否の表示を失わない）。
    - `inspectVariable` の、拒否の前に状態を消している箇所（433-449）も同じ順にする。
- [ ] **M18** `showFrame`、`reload`、`confirmIfEdited`
  - 赤: `testShowingAnotherFrameDropsUndoOfDiscardedEdit`。`selectFrame(2)` のあと `XCTAssertFalse(undo.canUndo)`。
  - 修正: 文字列と `shownSource` を設定し、`undoManager?.removeAllActions()` を呼ぶ補助関数を 1 つ作り、3 か所（407、572、744）で使う。
- [ ] **M19** `configureTable`
  - 赤: `testClickingEmptyFrameAreaKeepsEditedFrameSelected`。`deselectAll(nil)` のあとも `selectedFrame == 1`。
  - 修正: `frameTable.allowsEmptySelection = false`。
- [ ] **M20** `variable(at:)`、`printValue(frame:temp:)`
  - 赤: `testPrintRetryReadsItsOwnProcessAfterPumpedSelect`。フックが `ao_debug_select(b.pid)` を呼んでも、`a.variable(at: 1).value.hasPrefix("aaa")`。
  - 修正: 倍化の各回の頭で `ao_debug_select(pid)` を呼び直し、失敗なら nil を返す（SPEC 997）。
- [ ] **M21** `TranscriptWindow.installHook`、`LaunchSet.deinit`
  - 修正: 自分が入れたフックのときだけ外すトークンにする。`nonisolated deinit` から C のグローバルを上書きしない。既存の `ChangeSorterTests` と `ToolWindowTests` が緑のまま。
- [ ] **Low**:
  - Browser の削除確認シートが、Open Image… をまたいで別のイメージのクラスを消せてしまう。`ChangeSorterWindow` の `loads` の検査（232-234）を写す。赤は `testDeleteSheetAnsweredAfterImageLoadDoesNothing`。
  - `ChangeSorterWindow.noteImageLoaded`（174-179）は、エラー欄を消す代わりに `noteRead()` を呼ぶ。
  - `ChangeSorterModel.fileOutChangeSet` に `@MainActor` を付ける。
  - Swift 6 の警告 5 件:
    - `ChangeSorterTests.swift:17` は `nonisolated private static let` にする。
    - ほかの 4 か所は、クロージャに捕捉されてから書き換えられる `var` を、`let` で捕まえる小さなクラスの箱にする。
    - 完了条件は、`swift test` のビルドで `AoTests` の警告が 0 件であること。
  - `scripts/test.sh`:
    - 99-116: 2 行を合わせて 30 秒の締め切りにする（SPEC 1932）。
    - 68-79: 5 秒待っても終わらなければ `kill -9` する。
    - 確認は `scripts/test.sh --app`。
  - `ci.yml` 15-17: `cancel-in-progress: ${{ github.event_name == 'pull_request' }}`。

## R3 ライブデバッガのランタイム（H4、H7、M7、H6、H3、M4、M5）

**Files:**
- `runtime/src/Session.cpp`（`sessionDebugAccept`、`stillHalted`、`debugFrameSource`）
- `runtime/src/Interpreter.cpp`（`reactivateAccepted`、`performSend`、`stepCheck` の分岐）
- `runtime/src/Scheduler.cpp`（`halt`、`step`、`runFiber`）
- `runtime/src/DebugSnapshot.cpp`（`synthesizesNative`、`LiveFrames`）
- `runtime/src/Send.cpp`（`doesNotUnderstand`）
- Test: `debug_abi_test.cpp`、`image_save_load_test.cpp`、`failure_test.cpp`

### SPEC を先に直す

- §3.13（1826）: `interrupted` の停止からの Step は、止めた命令そのものを実行してから次の境界で止まる。
- §3.13（1817、1818）: 最内のフレームに進行中の送信があって合成するかどうかは、停止したときに決まる。停止中にそのセレクタを定義しても、フレームの数と番号は変わらない。
- §3.10（1516、1517）: ブロックのフレームで、ブロックがソース表に無くても、ホームのメソッドに項目があるなら、ホームのソースを書く。答えは `AO_OK` か `AO_ERR_RANGE` で、強調は 0-0（ロードしたイメージの項目にはブロックが無いため）。
- §4.1 に下のテスト名を足す。

### 項目

- [ ] **H4** `sessionDebugAccept`、`stillHalted`
  - 赤: `LiveAccept.AcceptRefusesHeaderRewrittenWhileHalted`。
    - `bar ^self halt` で止め、Do it で `m instVarAt: 1 put: (m instVarAt: 1) + 3` を実行し、`ao_debug_accept(pid, 1, "bar ^3")` を呼ぶ。
    - 期待: `AO_ERR`、理由は既存の `debugger accept refused: not an accepted method`（`Session.cpp:1598`）。
    - temps が 0 のときは、修正前は ASan なしでも落ちる。temps があるときは ASan で範囲外読みとして見える。
  - 修正: `numArgs > target->temps->n` なら拒む。`stillHalted` も同じ上限で読む。`temps != nullptr` の検査は要らない（`FrameLink` が必ず設定する）。
- [ ] **H7** `reactivateAccepted`、`runFiber`
  - 赤: `LiveAccept.AcceptWithoutRootSlotsLeavesNoPin`。
    - k を 0 から 64 まで回し、各回で止める → `failSlotGrowthForTesting(k)` → Accept。
    - `ao_debug_halted_count() == 0` になった回で、`roots.counts().pinnedSlots == 0`。
    - 落ちる k（いまは 21）を決め打ちしない。
  - 修正:
    - `reactivateAccepted` の前置き（`Root method(…)` から `disarmAccept` まで）を例外安全にする。catch して `disarmAccept`、`ctx.reactivating = false` のあと再送出する（`proceed` の catch と同じ形）。
    - 保険として、`runFiber` の出口でも `disarmAccept(ctx); ctx.reactivating = false;`。
    - レビュー案の `clearUnwinding` に入れる方法は却下する（基底の ctx で約 20 か所から呼ばれ、`runAside` は accept のスロットを退避しない）。
- [ ] **M7** `Scheduler::halt`、`LiveFrames`
  - 赤: `LiveDebug.DefiningDnuSelectorWhileHaltedKeepsFrames`。
    - `callZork` の DNU で止め、`ao_accept_method` で `zork` を定義する。
    - フレームの数は変わらず、`label(1) == "PrbM7>>callZork"`。
  - 修正: 合成するかどうかの bool を、停止したときにプロセスの記録へ残す。ラベルは今までどおり引き、無ければ DNU の形にする。新しい GC ルートは作らない。
- [ ] **H6** `Scheduler::step`、`Interpreter::run` の `stepMode` 分岐
  - 赤: `LiveStep.InterruptThenStepOverAdvances`（`interruptOncePump` を使う）。`[n < 100000] whileTrue: […]` を中断し、Step over のあとの `ao_debug_frame_pc(0)` が中断したときの pc と違う。Step into も同じ。
  - 修正:
    - `interrupted` の停止のあとの Step で、`ctx.stepSkipOnce = true` を立てる。既存の `stepMode != None` の分岐の中で 1 度だけ消費する。
    - 1 回きりでないと、`[true] whileTrue` の Step over が止まらなくなる。
    - 検査点をホットループへ移す案は却下する（P13 計画:7、P15 計画:23「ホットループに分岐を足さない」）。
    - R3 の緑の条件に足す: `docs/bench.md` のホットループ比が、揺らぎを超えて悪くなっていない。
- [ ] **H3** `debugFrameSource`
  - 赤: `ImageSaveLoad.LoadedBlockFrameReadsHomeSource`（`AcceptedSourceSurvivesSaveAndLoad` の隣）。
    - 期待: `AO_OK`。16 バイトのバッファなら `AO_ERR_RANGE`。強調は 0-0。
  - 修正: ブロックのフレームで、ブロックが表に無く、ホームに項目があるなら、`return writeBuf(text, buf, len)` を返す。
- [ ] **M5** `performSend`（Interpreter.cpp:373）
  - 赤: `LiveProceed.ProceedThenOutOfMemoryAbortsAtTheSend`。手本は `ProceedFromNativeFailureDoesNotHaltAgain`。Proceed のあとの OOM は、その送信で `out of memory` の中断になる。
  - 修正: `ctx.heap.outOfMemory()` を `proceededHere` より先に見る。
- [ ] **M4** `doesNotUnderstand`（Send.cpp:103）
  - 赤: `FailureAbort.DnuWithoutRoomForMessageIsOutOfMemory`。old を上限近くまで埋め、200 引数の DNU を送ると、理由は `out of memory`。
  - 修正: `outOfMemory()` なら `abortEvaluation(ctx, "out of memory")`。

## R4 Kernel ネイティブの失敗の伝搬（H12、H10、H11、M6、M12、M25、M16、safepoint の Low）

**Files:**
- `runtime/src/kernel/Collection.cpp`、`runtime/src/kernel/Stream.cpp`、`runtime/src/kernel/Process.cpp`
- `runtime/src/kernel/Float.cpp`（`makeFraction`）
- `runtime/src/Scheduler.cpp`（`ocSize`、`ocFirst`、`ocRemoveFirst`、`ocRemoveLast`、`ocRemoveIdentity`）
- `runtime/src/Bootstrap.cpp`、`runtime/src/kernel/LargeInteger.cpp`
- `runtime/include/ao/Send.hpp`（`sendDo` を足す）
- Test: `collection_do_test.cpp`、`gc_safety_test.cpp`、`process_test.cpp`、`bootstrap_test.cpp`

### SPEC を先に直す

- §3.4（347 付近）【D3】: `signal`、`wait` などのネイティブは、リスト（`linkedList`）が SPEC 715 の形でなければ、`damaged ordered collection` で失敗する。ランタイムが送信なしにたどる走査（イメージの採用）は、そのリストを空として扱う。どちらも、配列の範囲の外は読まない。
- §3.6（640 付近）【D4】: `isEmpty` は、`size` の答えが Integer なら、それが 0 かどうかを答える。Integer でなければ失敗する。`ArrayedCollection>>do:` は、`size` が SmallInteger でなければ失敗する。
- §4.1 に下のテスト名を足す。

### 項目

- [ ] **H12** `Collection.cpp` の 243、352、359、377、400 行、`Stream.cpp` の 483 行、thunk の確保（302、345、372、394 行、`Stream.cpp` の 479 行）、`ao_Stream_cr`（496 行）
  - 赤: `CollectionDo.IntervalDoFailureIsNotSwallowed`。
    - `<` と `>` が 3 を答える `OddStep` を `fileInString` で定義する（手本は `gc_safety_test.cpp:725`）。
    - `Interval from: 1 to: 5 by: OddStep new` に対して、`select:`、`reject:`、`detect:ifNone:`、`inject:into:`、`includes:`、`nextPutAll:` のどれもが空を答え、理由は `failed: #do:`。
  - 修正:
    - `Send.hpp` に `bool sendDo(CallContext& ctx, Oop receiver, Oop thunk)` を足す。`ctx.wk.selDo_` を送る。巻き戻し中なら false。黙った空なら `abortFailedSend(ctx, ctx.wk.selDo_)` のあと false。
    - 上の 6 か所で使う。thunk を確保できなかったときは `return Oop{}`。
    - `ao_Stream_cr` は、`nextPut:` の空を受けて `Oop{}` を返す。
    - 理論上だけの 4 か所（64、281-284、`Stream.cpp` 74-77、610）は触らない。
- [ ] **H10** `ao_Semaphore_new`、`ao_SharedQueue_new`
  - 赤: `GcSafety.SemaphoreNewOutOfMemoryLeavesNoEmptySlot`。2 スロットは入るが 3 スロットは入らないところまで埋める。答えは空で、`outOfMemory()`。
  - 修正: 各 send のあと、`slotAtPut` の前に `if (v.isEmpty()) return Oop{};`。
- [ ] **H11** `makeFraction`
  - 赤: `GcSafety.FractionNormalizeOutOfMemoryAnswersNoFraction`。`Boot b(1 << 20, 1 << 20, 2 << 20)` と、ルートした tenured の詰め物を使い、`(1 bitShift: 600000) / 3` を評価する。答えは空で、`outOfMemory()`。
  - 修正: `truncDiv` と `neg` のあとで、空なら `return Oop{}`。
- [ ] **M6** `Scheduler.cpp` の `oc*`
  - 赤: `Process.SignalOnDamagedLinkedListEnds`（`SignalWithExcessAtSmiMaxFails` の隣）。
    - `(s instVarAt: 2) instVarAt: 3 put: 4611686018427387903` のあとの `s signal` は、`damaged ordered collection` で終わる。
    - いまは ctest のタイムアウトで赤になる。
  - 修正: SPEC 715 の不変条件を確かめる関数を 1 つ作り（`1 <= firstIndex <= lastIndex + 1 <= size + 1`、array は Kernel の Array）、5 つの `oc*` がそれを通す。すべてのループが array の大きさで止まる。
- [ ] **M12** `ao_Collection_isEmpty`、`ao_ArrayedCollection_do_`（274-276 行）
  - 赤（`ArraySelectRejectDetectInjectIncludesIsEmpty` の隣）:
    - `CollectionDo.IsEmptyOnLargeIntervalAnswersBoolean`: `(1 to: (1 bitShift: 100)) isEmpty == false`。
    - `CollectionDo.ArrayedDoWithNonSmallIntegerSizeFails`: `size` が 2^100 を答えるサブクラスの `do:` は失敗する。
  - 修正: `LargeInteger::isInteger` なら `isZero` で Boolean を答え、それ以外は `Oop{}`。`do:` は SmallInteger でなければ `Oop{}`。
- [ ] **M25** `Bootstrap.cpp` の 183、309、320 行と、312-315 行
  - 赤: `Bootstrap.TinyNurseryStillBoots`。`Heap heap(4096, 1 << 20)` で `Bootstrap::run` のあと、Processor と Transcript がヒープのオブジェクトで、`1 + 2` が 3。
  - 修正: 3 か所の確保を `allocateNoGc` にする。send の答えを確かめ、失敗なら nil のままにする。
- [ ] **M16** `magBitLength`、`magBit`、`magSetBit`、`box`
  - 修正: ビット長と添字を `int64_t` にする。`box` に `size() > UINT32_MAX / 4` なら OOM の検査を足す。
  - 実用的な赤テストは作れない（768 MiB 規模）。`LargeIntegerApi` の既存のテストが緑のままであることを確かめる。
- [ ] **Low safepoint**（SPEC 606）: `ao_ArrayedCollection_do_`（Collection.cpp:279）と Stream の `contents`（285、309 行）で、64K 回ごとに `safepoint` を通す。
  - 赤は不要。`GcStress` 下で既存のテストが緑のまま。

## R5 反射のあとのメモリ安全（C2、NativeMethod の Low、M17）【D1】

**Files:**
- `runtime/src/kernel/Object.cpp`（`ao_Object_basicAt_`、`ao_Object_basicAt_put_`、`ao_Object_basicSize`）
- `runtime/src/NativeMethod.cpp`（`apply`、`functionOf`、`nameBytes`）
- `runtime/src/MethodDictionary.cpp`（`atPut`、`growInner`）
- Test: `failure_test.cpp`、`native_method_test.cpp`、`method_dictionary_test.cpp`

### SPEC を先に直す

- §3.6（574）の既知の制約に次を足す（669、805 と同じ書き方）。結果は規定しない。メモリの安全だけを保つ。
- 同じ箇所に次を足す。オブジェクトのバイト列の印と、クラスの format の `isBytes` が合わなければ、`basicAt:`、`basicAt:put:`、`basicSize` は失敗する。理由は `basicAt: object does not match its class format`、`basicAt:put: object does not match its class format`。`basicSize` は 0 を答える。
- §4.1 に下のテスト名を足す。

### 項目

- [ ] **C2**
  - 赤: `FailureAbort.BasicAtPutRefusesFormatThatDisagreesWithObject`。
    - Array のサブクラス `P` で `p at: 1 put: Object` のあと、`P instVarAt: 3 put: 196608` にする。
    - `p basicAt: 1 put: 24` は `AO_ERR_EVAL` で、上の理由。
    - format を戻したあとも `(p at: 1) == Object`（値のアサーションなので、ASan も gcstress も要らない）。
    - バイト列の側（`ByteArray` のサブクラス、`basicAt: 8`）も同じテストに入れる。
  - 修正: 3 つのネイティブの頭で `const bool objBytes = (ctx.heap.flags(receiver) & kFlagBytes) != 0;` と `Format::isBytes(fmt)` を比べ、違えば失敗する。
- [ ] **Low NativeMethod**
  - 赤: `NativeMethod.ApplyRefusesTooFewSlots`。スロット 0 の NativeMethod を作り、偽の辞書から送っても落ちず、失敗する。
  - 修正: 「ポインタのオブジェクトで、`size >= kNativeSlotCount`」の述語を 1 つ作り、`apply`、`functionOf`、`nameBytes` が共有する。`Session.cpp:995` の呼び出しもそれを通す。
- [ ] **M17**
  - 赤: `MethodDictionary.AtPutGrowsWhenTallyLies`。内側の配列が奇数長で `tally` が 0 でも、2 回目の Accept が入る。
  - 修正: `Globals.cpp:86-123` の `bindIn` と同じ形にする（空きのペアを探す、拡張するときに実際の数を数え直す、オーバーフローを防ぐ）。bool の戻り値と `allocateNoGc` は保つ（2026-09-23 計画:127 の決定）。

## R6 コンパイラとチャンク（H9、M2、M3、文字リテラルの UTF-8、M11）

**Files:**
- `compiler/src/Parser.cpp`（`parseKeywordMessage`、`parseCascadeMessage`、`isSuper` / `toSuper` の判定）
- `compiler/src/Codegen.cpp`（`isSelfishReturn`）
- `compiler/src/ChunkParser.cpp`（`splitChunks`）
- `compiler/src/Scanner.cpp`（`lexCharacter`）
- `image/Tools-model/ChangeSet.st`（`superclassOrder:`）
- Test: `compiler/tests/parser_test.cpp`、`codegen_snapshot_test.cpp`、`chunk_test.cpp`、`scanner_test.cpp`、`image/tests/changeset.st`

### SPEC を先に直す

- §3.8 に次を足す。
  - 1 つの送信の引数は 255 個まで。超えたらコンパイルエラーで、理由は `too many arguments`。位置は 256 番目のキーワード。
  - 文字リテラル `$x` の後ろは、整った UTF-8 の 1 文字（Unicode スカラー値。過長な符号、サロゲート、0x10FFFF を超える値は不可）。そうでなければコンパイルエラーで、理由は `invalid character literal`。文字列リテラルは今までどおり（651）。
- §3.8 のチャンク形式（905-909 付近）に、クラスコメントの規則を書く。
  - prose になるのは、セクションの外で、トークンが `Ident [class] commentStamp:` で始まるヘッダのあとの 1 チャンクだけである。
  - セクションの中のメソッドは、`commentStamp:` を含んでいてもメソッドである。
- §4.2 に下のテスト名を足す。

### 項目

- [ ] **H9**
  - 赤: `Parser.SendWithMoreThan255ArgumentsIsRejected`。通常の送信、カスケードの部分、`super` のカスケードの部分の 3 通り。どれも `!ok` で、`too many arguments`、位置は `k256:`。
  - 修正: `parseKeywordMessage` と `parseCascadeMessage` のキーワードのループで、256 個目を数えた時点でエラーにする。`makeSend` だけに置かない。
- [ ] **M2**【D6】
  - 赤: `Codegen.InstVarNamedLikePseudoVariableShadowsReturn`。
    - `env.instVarNames = {"nil", "true", "false", "self"}` のとき、`foo ^nil` は `PushInstVar 0 / ReturnTop`（`ReturnNil` ではない）。
    - インスタンス変数 `super` があるとき、`super foo` は `SendSuper` にならない。
  - 修正:
    - `isSelfishReturn` は、`instVarIndex(expr.name) >= 0` なら false を返す。
    - `Parser.cpp:366, 426` の名前による `isSuper` / `toSuper` も、インスタンス変数を先に見る。
    - `local(expr)` の半分は足さない（擬変数と同じ名前のローカルは `Parser.cpp:151-153` が拒んでいる）。
- [ ] **M3**
  - 赤: `Chunk.CommentStampMethodInSectionDoesNotMakeNextChunkProse`。
    - 入力は `!Foo methodsFor: 'x'!\ncommentStamp: s\n\t^s!\nanswer\n\t"the users' number"\n\t^42!\nother\n\t^7! !\n`。
    - 期待: アクションは 1 つで、メソッドは 3 つ。`methods[1].source == "answer\n\t\"the users' number\"\n\t^42"`。
  - 修正: `splitChunks` が、セクションの状態（ヘッダを見た、空のチャンクで終わる）を自分で持つ。トークンで `Ident [class] Keyword("commentStamp:")` を照合する。
- [ ] **文字リテラルの UTF-8**
  - 赤: `Scanner.CharacterLiteralNeedsWellFormedUtf8`。
    - `Scanner("$\xC3x")` は `Tok::Error` で、`x` を食べない。
    - `compileMethod("foo ^$\xF4\x90\x80\x80")` は `!ok`、`invalid character literal`、位置は `[5-10]`。
    - `$\xC0\x80`（過長）と `$\xED\xA0\x80`（サロゲート）も拒む。
  - 修正: `lexCharacter` で、継続バイト、過長な符号、サロゲート、0x10FFFF 以下を検査する。
- [ ] **M11**（nil の名前の半分だけ）
  - 赤: `image/tests/changeset.st` に足す。`p instVarAt: 4 put: nil` のあとの `(ChangeSet superclassOrder: …) size` が 1。
  - 修正: `ChangeSet.st:206` で `cls superclass name isNil` を検査する。祖先をたどる案は却下する。

## R7 イメージの保存と読み込み（H5、M8、CLI の Low、`installMissing`）

R4 と R6 のマージ後に入れる。

**Files:**
- `runtime/src/ImageLoad.cpp`（`acceptWord`）
- `runtime/src/Session.cpp`（`sessionImageSave`）
- `runtime/src/ao_main.cpp`（`runImage`、引数の検査）
- Test: `image_save_load_test.cpp`、`session_abi_test.cpp`、`runtime/tests/cli_test.sh`

### SPEC を先に直す

- §3.11 のロードの検査（1653-1662）【D5】に次を足す。
  - ヒープのスロットとレコードの語が空 OOP（bits 0）なら `damaged image`。
  - Unicode スカラー値でない Character も `damaged image`。
  - 保存は、保存の前に同じ検査を通すので、オブジェクトと規則を示して拒む。
- §3.11 の 1601 行: 「ロードのあと足りないネイティブを補う」を、`ao image load` でも行う（`adoptImage` のあと `installMissing`）。
- §4.5（1946）: `ao --test <dir>` の余分な引数と、`ao image load <path>` の余分な引数は、使い方を出して終了コード 2。
- §4.1 に下のテスト名を足す。

### 項目

- [ ] **H5** `acceptWord`
  - 赤:
    - `ImageLoadChecks.RefusesEmptyOopInSlotOrRecord`。`ImageSurgery` で `record("Smalltalk")` のスロット 1 つと、あるレコードの bits を 0 にすると、`expectRefused(bad, "damaged image")`。
    - 不正な Character（0xD800、0x110000）も同じ。
    - `ImageSaveChecks.SaveRefusesEmptySlot`。
    - Review Focus 1: `ImageSaveLoad.FreshSessionAndVendorImagesHaveNoEmptyWords`（新規、セッション、`--load-order image/vendor/LOAD_ORDER` の 3 通り）。
  - 修正:
    - `acceptWord` で bits 0 と、スカラー値でない Character を拒む。
    - `decodeNonHeap` は変えない（`ImageFormat.ImmediateBitsRoundTrip` と、p7 計画:271 の決定）。
- [ ] **M8** `sessionImageSave`、`runImage`
  - 赤: `SessionAbi.ImageSaveDoesNotTraceMethodCache`（`ImageSaveDoesNotTraceClassIdSlots` の隣）。
    - Workspace の束縛だけが持つ Behavior に 1 度送信してから束縛を捨て、メソッド辞書を壊す。
    - 期待: `EXPECT_EQ(AO_OK, ao_image_save(path))`。保存したイメージに目印の文字列が無い。
  - 修正: `Image::save` の前に `cache->flushAll()`。`runImage`（ao_main.cpp:286 付近）でも同じ。
- **CLI**
  - 赤: `cli_test.sh` に 2 件足す。`ao --test dir extra` と `ao image load p extra` は、使い方を出して終了コード 2。
  - 修正: `ao_main.cpp` の引数の検査で、余分な引数を拒む。`runImage` の `image load` で、`adoptImage` のあとに `installMissing` を 1 回呼ぶ。

## R8 ABI と GC の潜在欠陥（M9、M10、M22、M23、M24、M26、`pushHandle`）

**Files:**
- `runtime/src/abi.cpp`（browser の読み出し 7 関数、`deliverTranscript`）
- `runtime/src/Gc.cpp`（`clearWeakAfterOldMark`）
- `runtime/src/WellKnown.cpp`（`~WellKnown`）、`runtime/include/ao/Send.hpp`（`send` の契約のコメント）
- `runtime/src/Heap.cpp`（コンストラクタ）
- `runtime/src/Roots.cpp`（`enterNextFrameBlock`、`pushHandle`）
- `runtime/src/VendorExtract.cpp`（`containsHostWord`）
- Test: `browser_abi_test.cpp`、`transcript_model_test.cpp`、`gc_test.cpp`、`vendor_extract_test.cpp`

### SPEC を先に直す

- §3.10（Transcript のフック）: Transcript に渡す文字列が `INT_MAX` バイトを超えるなら、UTF-8 の文字の境界で `INT_MAX` 以下の塊に分けて、順にフックへ渡す。
- §4.1 に下のテスト名を足す。

### 項目

- [ ] **M9** `ao_browser_class_at`、`_protocol_at`、`_selector_at`、`_source`、`_class_definition`、`_superclass`、`_subclass_at`
  - 赤: `BrowserAbi.RefusedReadsLeaveTheBufferEndingInNul`。手本は `changes_abi_test.cpp:312-314`。
    - バッファを `'X'` で埋め、セッション無し、不明な ID、`meta` 2、NULL の引数、範囲外の添字で呼ぶ。
    - どれも `buf[0] == '\0'`。
  - 修正: 7 つのラッパーの入口（`guarded` の外）で `blankBuf(buf, len)`。
- [ ] **M10** `deliverTranscript`
  - 赤: `TranscriptModel.LongTextIsSplitOnUtf8Boundaries`。
    - 塊の大きさを引数に取る内部関数を作り、テストでは小さな上限で呼ぶ（2 GiB を確保しない）。
    - 「あ」をまたぐ位置で分けても、各塊が整った UTF-8 で、連結すると元に戻る。
  - 修正: `deliverTranscript` を、境界で塊に分ける内部関数の上に作り直す。
- [ ] **M22** `clearWeakAfterOldMark`
  - 赤: `GcWeak.NurseryWeakSlotToUnseenNurseryObjectIsCleared`（`GcWeak_NurseryWeakSlotNilsUnmarkedOldReferent` の隣）。
    - old の O、nursery の X{O}、ルートした nursery の弱い W{X} を作り、`collectOld()` を呼ぶ。
    - W のスロットが nil。
  - 修正: `nurserySeen` を渡し、見ていない nursery のオブジェクトを指す弱いスロットも nil にする。
- [ ] **M23** `~WellKnown`
  - 赤: `GcRoots.WellKnownDestructorRemovesItsRoots`。内側のスコープで WellKnown を作って壊したあと、`roots.counts()` が元に戻る。
  - 修正: デストラクタで、登録したスロットと intern 表を `roots_->remove(...)` する。
- [ ] **M24**【D2】 `send`
  - 修正: `Send.hpp` の `send` の宣言に、次の契約をコメントで書く。本番の呼び出し元はすべて `nullptr` を渡す。非 null の `ic` は、メソッドの差し替えで無効化されず、GC をまたいで持てない。
  - assert は入れない。`native_send_test.cpp:58-126` が 1 段のキャッシュの単体テストとして `&ic` を渡しているからである。テストは変えない。
- [ ] **M26a** `Heap` のコンストラクタ
  - 赤: `GcNursery.UnalignedNurserySizeStillAllocatesHeapObjects`。`Heap(1001, …)` で `collectNursery` のあと、`allocate(...)` の答えが `isHeap()`。
  - 修正: `nurseryBytes` を 8 の倍数に切り下げる。
- [ ] **M26b** `enterNextFrameBlock`
  - 修正: 先にブロックを確保してから状態を変える。
  - 赤: `GcRoots.FrameBlockGrowthFailureLeavesRootsConsistent`。`failSlotGrowthForTesting` と同じ形の注入点を `Roots` のフレームブロックの確保に足す。
- [ ] **M26c** `containsHostWord`
  - 赤: `VendorExtract.HostWordInCommentOrStringIsNotPatched`（合成した入力を使う）。
  - 修正: `scanShapes` と同じ字句の扱いで、コメントと文字列を飛ばす。
- [ ] **Low `pushHandle`**
  - 修正: 2 つのベクタを先に `reserve` してから `push_back` する。
  - 赤は作らない（C++ の OOM でしか起きない）。`GcRoots` の既存のテストが緑のまま。
- [ ] **最終確認**: `ctest --test-dir build -R '^gcstress$'` を 1 度、最後まで流す。`CMakeLists.txt` は TIMEOUT を設定していないので、ctest に打ち切られることはない。ただし、全体の所要時間はまだ測っていない（レビューでも途中で止めている）。バックグラウンドで流してよい。

---

## 受け入れ（計画全体）

- [ ] D1〜D6 が決まり、各バッチの SPEC 文が決定に合っている
- [ ] R1〜R8 の PR がそれぞれ main にマージされ、各 PR の本文に対応 ID と `Graphify:` / `Serena:` のトレーラがある
- [ ] 各バッチで共通手順 6 がすべて緑（`--asan` は指定したバッチ）
- [ ] `KernelScan.*` が緑（全 Kernel メソッドが `NativeMethod`）
- [ ] `ctest -R '^gcstress$'` が最後まで緑（R8 の最終確認）
- [ ] `swift test` のビルドで `AoTests` の警告が 0 件（R2）
- [ ] `graphify update .` 済みで、`PHASE` は `P18` のまま

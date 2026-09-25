# Claude レビュー指摘の修正計画（docs/claude-review）

## Context

`docs/claude-review/`（HEAD 1ccf79a、PHASE P9）の指摘は、重複を除いて Critical 11 / High 29 / Medium 28 / Low 39。ほぼすべてプローブで再現されている。内容は次の 5 テーマに分かれる。

- メモリ破壊（GC 安全性）
- 失敗が黙って成功扱いになる経路
- コンパイラの意味論の欠落
- Browser でのデータ消失
- 資源の上限

SPEC §6 の未チェック項目「未定義セレクタは DNU に入り、デフォルトはエラーオブジェクトを返す」は、テーマ 2 で閉じる。

ユーザー決定（2026-09-23）:
- 範囲: 全件。バッチに分け、Critical と High を先に処理する。
- クロージャ: ジャンプ展開に加えて共有 temp まで実装する（Blue Book の意味論）。SPEC §3.4 と §3.5 を先に改訂する。
- Process: 協調スケジューラを実装する（fork / yield / wait で実際に実行が切り替わる）。
- old space: 拡張できるようにする。大きなオブジェクトは old に直接置く。ロード時はヘッダの heapBytes に合わせて確保する。

PHASE は `P9` のまま。どのバッチも v1（P9 統合）の欠陥修正として扱い、フェーズは追加しない。

計画が承認されたら、この文書を既存の慣習に合わせて `docs/superpowers/plans/2026-09-23-review-fixes.md` に置き、B0 の最初のコミットに含める。新しい依存ライブラリは追加しない。mmap / mprotect / pthread_get_stack* は OS の API、ファイバの切り替えは自前のアセンブリで書く。

## 全バッチ共通の手順（CLAUDE.md 標準ワークフロー）

1. ブランチ `fix/review-bNN-<slug>` を切る。1 バッチ 1 PR。
2. Graphify: `GRAPH_REPORT.md` を読み、`graphify path` / `explain` で対象の経路を取る。グラフは 54d2fa5 時点で古いので、B0 で更新する。
3. Serena: `find_symbol` → `find_referencing_symbols` の順に調べてから `replace_symbol_body` などで編集する。ヘッダや型の置換は `replace_in_files` の dry_run で差分を確かめてから適用する。
4. 挙動を変えるバッチは、SPEC.md の該当節を最初のコミットで直す。
5. レビューの「失敗シナリオ」をそのまま回帰テストにして、赤を確認する。
6. 実装して緑にする。`scripts/test.sh`（ctest と swift test）、`build/ao --test image/tests`、B0 以降は GC ストレス実行も回す。
7. 最後に `graphify update .` を実行し、コミットに `Graphify:` と `Serena:` のトレーラを付ける。
8. 対応したレビュー項目は、PR 本文に `docs/claude-review/NN` の見出しで列挙する。

---

## バッチの順序と依存

```
B0 準備 ─ B1 GC ─ B2 ブロック/インタプリタ ─ B3 失敗の伝搬 ─┬─ B4 クラスのメタデータ ─ B7 コンパイラ構文
                                                          ├─ B5 Browser/Workspace
                                                          ├─ B6 イメージ/セッション ─ B10 スケジューラ
                                                          └─ B8 Kernel 数値 ─ B9 コレクション ─ B11 App/ビルド
```

- B1 を最初に行うのは、ほかのすべてのバッチが GC ストレス下で緑であることを前提にするため。
- B2 の `abortEvaluation` と LitVar 系のオペコードは、B3、B4、B10 で再利用する。
- B9 のハッシュ表化は、B8 の `=` と `hash` の契約を前提にする。
- B10 は B2、B3、B6 に依存する。

## B0 準備（テスト基盤）

- `.gitignore` に `/.cache/` と `.serena/logs/` を足す（07 Low）。
- `graphify update .` を実行し、グラフを HEAD に合わせる（今のグラフは 54d2fa5 から作ったもの）。
- GC ストレスモード（01 テストの穴）:
  - `Heap::setGcStress(n)` を足す。環境変数 `AO_GC_STRESS` でも設定できるようにする。
  - このモードでは、`allocateRetry` と safepoint で n 回に 1 回ナーサリ GC を走らせ、そのうち 4 回に 1 回は `collectOld` も走らせる。
  - GC のあとは、解放した領域を `0xA5` で埋め、古い番地を読んだら即座に落ちるようにする。
  - ctest に `gcstress` 項目を登録する。有効にするのは B1 の最後。
- CMake に `option(AO_SANITIZE)` を足す（`-fsanitize=address,undefined`、出力先は `build-asan`）。`scripts/test.sh --asan` で回せるようにする（07 テストの穴）。
- `CMAKE_OSX_DEPLOYMENT_TARGET 14.0` を `project()` の前に置く（07 Low）。
- テスト: `GcStress.EnvEnablesStress`。

## B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）

### SPEC を先に直す

- §3.2:
  - old は、上限 4 GiB（ヘッダの `uint32 heapBytes` の上限）の仮想領域を 1 つ予約し、必要な分だけコミットする。アドレスは動かない。
  - 大きなオブジェクトは old に直接置く。
  - スキャベンジは失敗しない。old に入り切らない生存物は to-space に残す。
  - full GC はスキャベンジの後、`oldUsed > threshold` のときだけ走らせる。`threshold = max(初期容量, 2×生存量)`。
  - 上限に達したら、評価エラー「out of memory」にする。
  - ネイティブが受け取る receiver と引数は、ルート済みとする。
- §3.10: 「ヒープの既定容量は変えない」を「既定の初期容量（nursery 1 MiB×2、old 4 MiB）は変えない。old は上限まで伸びる。`ao_image_load` はヘッダの heapBytes に合わせてコミットする」に改める。
- §3.11: heapBytes は old の上限以下とする。
- §4.1: GC ストレス実行を足す。
- §5.5: mmap / mprotect を、使ってよい OS API として明記する。

### 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）

- `Root`: 今と同じ公開 `slot` を持つ。
- `RootedArray`:
  - 6 要素までは内部に持つ。
  - 登録は `Roots::pushRange/popRange` の 1 回で済ませる（LIFO を assert する）。
  - `ptr()`、`operator[]`、`size()` を持つ。
- `allocateRetry(CallContext&, cls, size, flags)` を公開し、GC を走らせる割り当てはこれだけにする。
- 移行:
  - 16 ファイルの `Root` と 11 ファイルの `allocateRetry` は、中身がまったく同じである（ハッシュで確認済み）。Serena の `replace_in_files` で削除し、include を足す。
  - Session.cpp の `HostRoot` と `HostSlots` は `rename_symbol` で改名する。
  - この段は純粋なリファクタとして 1 コミットにする。

### ネイティブのルートを構造で保証する

- `NativeFn = Oop(*)(CallContext&, const Oop& receiver, const Oop* args, uint32_t)` に変える。
- 新しい `NativeMethod::invoke` が receiver と args を 1 つの `RootedArray` に載せ、ルート済みスロットへの参照を渡す。`apply` はこれを呼ぶ。
- 263 個のネイティブのシグネチャは、`replace_in_files` の正規表現で一括置換する。次のヘルパーも同じ形に変える: `applyBlock`、`dictAt`、`dictAtPut`、`dictIncludesKey`、`setAdd`、`setIncludes`。
- ローカル変数を渡しているネイティブ直呼び（Behavior.cpp:108/118、Object.cpp:24/270、Process.cpp:320–412、Boolean.cpp:73）は `invoke` 経由にする。
- これで直る既報: Magnitude の `<=` と `between:and:`（03 Critical）、`Symbol>>asString`（04 Low）、`sendBin` / `Compile.cpp:180` / `perform:` の未ルート引数。
- DNU の経路（Send.cpp:63–98）: receiver、セレクタ、引数を `RootedArray` に載せる。Message の割り当ては `allocateRetry` を通す（02 Low）。
- 性能ゲート: ネイティブ中心のループで、変更前と比べて 5% 以内。
- CLAUDE.md の C++ 規約に「receiver をローカル変数にコピーして GC をまたがない」を足すことを、ユーザーに提案する（CLAUDE.md の変更なので承認を得てから）。

### GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）

- old は `mmap(PROT_NONE)` で予約し、1 MiB 以上の単位で `mprotect` によってコミットする。
  - 検討して退けた案: 大きなバッファへの付け替え（スキャベンジの途中で伸ばせず、ポインタも無効になる）と、チャンク分割（`inOld` の判定が重くなり、圧縮も複雑になる）。
- `Heap(nursery=1MiB, oldInitial=4MiB, oldMax=4GiB)` にする。
  - `allocate` は、`objectBytes ≥ min(64KiB, nursery 半面)` なら old に置く。
  - 追加: `allocateTenured` と `allocateNoGc`（Heap 層のコードが使う、GC しない割り当て）。
- `copy()` の変更（01 Critical と High を解消する）:
  - `reserveOld` で置けなければ、to-space に残す。
  - `collectOld()` の呼び出し、`failed_`、`oldCompacted_`、do/while の再試行を削除する。
  - スキャベンジ中に old が動かなくなるので、:104 と :119 の古い番地への書き込みも起きなくなる。
- `collectNursery` の順序: スキャベンジ → 弱参照スロットの消去（to-space を含む）→ フリップ → 閾値を超えていれば `collectOld`。
  - 閾値は `collectOld` の後に `clamp(2×live, initial, max)` に更新する。これで、何も回収できない full GC を繰り返さなくなる（01 High）。
- `collectOld` のルート更新は、`seen` 集合を使ってスロットごとに 1 回にする（01 Low）。
- OOM: `allocateRetry` は、ナーサリ → `collectNursery` → ナーサリ → `allocateTenured` の順に試す。すべて失敗したら `heap.setOutOfMemory()` を呼ぶ。この状態は B3 の failure として、評価を巻き戻す。
- イメージ:
  - `adoptOldBytes` は n バイトをコミットし、`n > oldMax` のときだけ拒否する。
  - ImageLoad.cpp:438 の判定を `heapBytes > oldMaxBytes()` に変える。
  - save では `tr.end > kOldMaxBytes` を検査する（06 High「保存できるがロードできない」）。

### その他（同じバッチで処理する）

- `MethodDictionary::atPut` を bool にする。`create` と `growInner` は `allocateNoGc` を使う。失敗は `installMethod` と `putNative` を通して呼び出し元まで伝える（01 Medium）。
- `shallowCopy` と `printString` は `allocateRetry` を使う。`asSymbol` は `allocateTenured`（04 Medium）。`LargeInteger::box(ctx)` は `allocateRetry`（03 Low）。
- `subclass:`: ローカルの `nameArg` をやめ、`args[0]`、`args[4]`、`klass(receiver)` を使う箇所ごとに読み直す（04 Critical）。
- `perform:withArguments:` は Array（とそのサブクラス）だけを受け付け、`RootedArray` に展開する（04 Low）。
- `Oop::fromSmallInteger` に範囲の assert を入れる。`kSmiMin` と `kSmiMax` は Oop.hpp へ移す（01 テストの穴）。

### テスト（書く順。各段を緑にしてから次へ進む）

1. SPEC を直し、HandleScope へのリファクタを行う。テストは `GcRoots.RangeRootIsVisitedAndPopped`。
2. NativeFn のシグネチャを一括で置換する。既存のスイートは全部緑のまま。
3. heap_alloc_test: `LargeObjectAllocatedInOld`、`ObjectLargerThanNurseryAllocates`、`OldGrowsPastInitialCapacity`、`OldStopsAtMax`、`AllocateNoGcSpillsToOld`
4. gc_test:
   - `CompactionDuringScavengeDoesNotCorruptSlots`（プローブ p1 の配置）
   - `NoIdentitySplitWhenOldAtMax`（プローブ p6）
   - `WeakSlotInToSpaceSurvivorCleared`
   - `CollectOldOnlyAfterThreshold`
   - `NoRepeatedZeroYieldCollects`
   - `DuplicateRootForwardedOnce`
   - 既存の `OldOomDoesNotClobberRoot` を `OldAtMaxKeepsSurvivorInToSpace` に書き換える。
5. image_save_load_test: `LoadSizesOldFromHeader`（4 MiB 超）、`RejectsHeapBytesAboveOldMax`
6. native_method_test: `ReceiverRefTracksMove`、`UnrootedCallerArgSurvivesGc`、`InvokeRootsDirectCall`。native_send_test: `DnuWithFullNurseryReturnsMessage`、`SuperSendUnderStress`
7. 新規 `runtime/tests/gc_safety_test.cpp`:
   - `MagnitudeLessOrEqualAcrossGc`、`BetweenAndAcrossGc`
   - `SubclassKeepsSuperAndMetaSuper`
   - `SymbolAsStringAfterGc`
   - `PerformWithArgumentsRejectsOrderedCollection`
   - `ShallowCopy/PrintString/AsSymbol/LargeLiteralWithFullNursery`
   - `CopyArrayLargerThanNursery`
   - `LiveSetAbove4MiBDoesNotThrash`
   - method_dictionary_test: `AtPutFailureReachesInstallMethod`
   - session_abi_test: `HugeAllocationReportsOutOfMemory`
8. oop_tag_test: `FromSmallIntegerOutOfRangeDies`。そのあと、ctest の `gcstress` / `gcstress_vendor` 項目を有効にし、ASan で全体を 1 回回す。

### リスク

- `collectOld` がスキャベンジの後にも走るようになる。そのため、`allocateRetry` をまたいで未ルートの old OOP を持つコードは、今より壊れやすくなる。ストレスモードとポイズン埋めで検出する。
- remembered set が無いので、スキャベンジのたびに到達可能な old を全部たどる。この費用はヒープの大きさに比例する（カードテーブルは後続の課題）。
- テスト用のヒープも、それぞれ 4 GiB の仮想領域を予約する（コミットはしない）。`Heap` は move-only にし、デストラクタで munmap する。

## B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）

設計の骨子:
- 共有 temp は temp ベクタ（ヒープの Array）で実現する。どの temp をベクタに入れるかは、コンパイラの事前解析で決める。
- ワークスペース変数は、temp ではなく Association 束縛にする。
- 新しいオペコードは `Primitive` の後ろに追記する。`.aoimage` の版は 1 のまま。well-known 表（127 個）には触らない。

### SPEC を先に直す

- §3.4: クロージャ（共有 temp）、ブロック起動ごとに新しいアクティベーションを作ること、`cannotReturn:`、ネイティブを貫く NLR、`ensure:`、abort とスタックガード。
- §3.5: オペコード表の全体（ジャンプの基点は次の命令）、番号は追記だけで変えないこと、インライン展開の一覧と値の規則、`mustBeBoolean`。
- §3.6 Kernel-Methods: 新しいセレクタ。
- §3.8: 捕捉解析、インライン展開、引数への代入をエラーにすること。
- §3.10 ワークスペース変数: Association 束縛と名前の解決順（ローカル → インスタンス変数 → 擬変数 → knownGlobals → 束縛）。255 の規則は削除する。
- §3.11: 版は 1 のまま。ロード後に `ensureKernelNatives` を呼ぶ（`ensureTranscriptClassMethods` を一般化する）。
- §6: 「ブロックが外側 temp を共有する」のチェック項目を足す。

### 共有 temp（`compiler/src/Codegen.cpp`）

- 事前解析 `analyzeScopes(const Ast&, const CompileEnv&)` を追加する。これで `bindTemp`、`localIndex`、`addWorkspaceTemp`、`Scope::copied` を置き換える。
  - 「実スコープ」はメソッドと、展開されないブロックのこと。展開されたブロックは外側の実スコープに併合する。
  - `inlinePlan(send)` を展開可否の唯一の判定にし、事前解析と生成の両方で使う。
- 変数の種類: Arg、LoopVar（`to:do:` の変数）、Temp、Hidden（`to:do:` の上限）。
  - Arg と LoopVar への代入はコンパイルエラーにする（05 Low の一部）。
  - 捕捉される Temp のうち、どこかで代入されるものをベクタに入れる。これは保守的な規則で、最適化は見送る。
- スロットの並び: `[args][ベクタに入れない temp（展開で持ち上げた temp と Hidden を含む）][ベクタ所有スロット][copied]`。copied は末尾のままなので、`Interpreter.cpp:474-490` は変えない。`numTemps` は並びから計算する（`Codegen.cpp:113` で値が固定される問題も直る）。
- ベクタは、それを宣言したノードの入口で `PushNewArray k; PopStoreTemp v` によって作る。展開したループ本体では反復ごとに新しいベクタができる。展開したブロックの temp は、入口で nil に戻す。
- 新しいオペコード（`Bytecode.hpp`。`kLastOp` を足し、`Interpreter.cpp:507` の `opb > Primitive` を置き換える）:

| # | Op | オペランド |
|---|---|---|
| 33 | PushNewArray | n |
| 34 / 35 / 36 | PushRemoteTemp / StoreRemoteTemp / PopStoreRemoteTemp | i t |
| 37 / 38 / 39 | PushLitVar / StoreLitVar / PopStoreLitVar | lit（Association） |

- LitVar 系は、ワークスペース束縛と B4 のクラス変数の両方で使う。

### ワークスペース変数（00 High ×3 と Medium を同時に解消する）

- `CompileEnv.undeclaredAreTemps` を `undeclaredAreBindings` にし、`LitKind::Binding` を足す。
- Session の resolver は Association を探すか作り、ワークスペース辞書に値として入れる。
- 次を削除する: `MethodImage::tempBindings`、`CallContext::hostTemps` / `hostTempCount`、`HostTempGuard`、Session.cpp:720-764 の読み込みと書き戻し。
- knownGlobals の名前集合はセッションにキャッシュし、クラス定義と `Smalltalk at:put:` のときだけ無効化する。

### インライン展開

- 展開するのは、`compileSend` がレシーバ自身をコンパイルする場合だけ。カスケードのパートは展開しない。
  - ifTrue:、ifFalse:、ifTrue:ifFalse:、ifFalse:ifTrue:、and:、or: は、全引数が 0 引数のリテラルブロックのとき。
  - whileTrue:、whileFalse: はレシーバと引数の両方、whileTrue、whileFalse はレシーバが、0 引数のリテラルブロックのとき。
  - to:do: は本体が 1 引数ブロックのとき。to:by:do: はそれに加えて、刻みが 0 でない数値リテラルのとき。
  - それ以外は通常の送信にする。
- 値の規則:
  - 分岐が無ければ nil。
  - `and:` は false、`or:` は true で短絡する。
  - while 系は nil。
  - to:do: はレシーバを返す。
- ループのコード:
  - to:do: の上限は 1 回だけ評価して Hidden に置き、`<=`（刻みが負なら `>=`）と `+` は SendSpecial で送る。
  - 後方ジャンプ用の `emitJumpBack` を足す。
- 展開したブロック内の `^` は、外側の実スコープの規則で返す。
- JumpTrue / JumpFalse の値が Boolean でなければ `#mustBeBoolean` を送る。その答えも Boolean でなければ abort する。

### ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）

- 新しいヘルパー: `unwinding(ctx)`（NLR か abort の最中か）、`callBlock(ctx, blk, args, n, &out)`、`abortEvaluation(ctx, reason)`。
- ブロックを呼ぶネイティブをすべて `callBlock` に寄せ、巻き戻し中なら空 OOP を返す（02 High）。対象:
  - Collection.cpp の各サンク、`do:`、`collect:`、`select:`、`reject:`、`detect:ifNone:`、`inject:into:`
  - SmallInteger.cpp:180
  - Dictionary.cpp:420/450/503/589/745/753/774
  - Stream.cpp:263
  - Boolean.cpp と Object.cpp の `sendValue`
- 新しいネイティブ（02 Medium）:
  - BlockContext: `value:value:value:`、`value:value:value:value:`、`numArgs`、`whileTrue:`、`whileFalse:`、`whileTrue`、`whileFalse`、`repeat`、`ensure:`、`ifCurtailed:`、`cannotReturn:`
  - Integer: `timesRepeat:`
  - Object: `mustBeBoolean`
  - ネイティブのループは 64K 回ごとに safepoint を通る。
- `ensure:` の手順: 本体を実行する → 巻き戻しの状態を Roots に退避してクリアする → 後始末ブロックを実行する → 状態を戻す（後始末が自分で巻き戻しを始めた場合は、そちらを優先する）。`ifCurtailed:` は本体が巻き戻ったときだけ実行する。
- `valueWithArguments:` は Array だけを受け付ける（02 Low）。

### スタックガード、abort、死んだホーム

- `CallContext` に `stackLimit`、`aborting`、`abortReason` を足す。
- `refreshStackLimit(ctx)` は、`pthread_get_stackaddr_np` と `pthread_get_stacksize_np` から、予約分 min(512 KiB, size/4) を残した下限を求める。呼ぶのはセッションの boot、テストの `Boot`、最外の `Interpreter::run`。B10 ではファイバ切り替えのたびにも呼ぶ。
- `applyMethod` は `__builtin_frame_address(0) < stackLimit` なら「stack overflow」で abort する（02 Critical）。
- abort は「ホームの無い NLR」として扱い、どのフレームでも捕まらずに最外まで戻る。Session はその理由を `AoSpan.message` に入れて `AO_ERR_EVAL` を返す。次の最外の入口で、残った abort をクリアする。
- ブロックは起動ごとに `Context::createBlock` で新しいアクティベーションを作り、クロージャ自体は書き換えない（02 Low）。メソッドのフレームが抜けるときは、`ContextExitGuard` が `kCtxPc` と `kCtxSender` を nil にして、死んだ印にする。
- `ReturnBlock` のホームが死んでいれば、`cannotReturn:` の答えをブロックの呼び出し元に返す。無関係な呼び出し元は巻き込まない（02 Medium）。

### 手順（各段を緑にしてから次へ進む）

| 段 | 作業 | 主なテスト |
|---|---|---|
| S0 | SPEC だけを直す | — |
| S1 | オペコード、インタプリタ、逆アセンブラ、`mustBeBoolean` | `NewOpsAppendedAfterPrimitive`、`HandAssembledRemoteTempRoundTrip`、`JumpOnNonBooleanAborts` |
| S2 | 巻き戻し、`callBlock`、スタックガード、ネイティブの移行 | `NonLocalReturnStops{OrderedCollection,Set,Dictionary,Interval,DictionaryCollect,DetectIfNone,InjectInto}`、`UnboundedRecursionReportsStackOverflow`、`SessionUsableAfterStackOverflow` |
| S3 | ブロックのネイティブ | `WhileTrueNativeWithBlockVariables`、`TimesRepeatCounts`、`ValueWithFourArgs`、`NumArgsAnswersArity`、`EnsureRunsOnNormalAndNonLocalExit`、`IfCurtailedRunsOnlyOnUnwind`、`ValueWithArgumentsRejectsNonArray`、KernelScan の必須セレクタ |
| S4 | 起動ごとのアクティベーション、死んだホーム | `RecursiveBlockKeepsOwnSender`、`DeadHomeReturnAnswersErrorAndContinues` |
| S5 | ワークスペース束縛（S6 より先に行う） | `UndeclaredAssignIsBindingStore`、`GlobalWinsOverBinding`、`BlockAssignmentUpdatesWorkspaceBinding`、`DeclaredTempIgnoresBinding`、`ThreeHundredBindingsEvaluate`、`ClassDefinedAfterBindingWins` |
| S6 | 事前解析と temp ベクタ | `CopyDoesNotWriteOuter` を反転した `BlockAssignmentWritesOuterTemp`（期待値 2）、`CounterClosureKeepsState`、`TwoBlocksShareTemp`、`ThreeLevelNesting`、`UnwrittenCaptureIsCopied`、`BoxedTempUsesRemoteTemp`、`ArgumentAssignIsError` |
| S7 | インライン展開 | `IfTrueLiteralInlinesToJumps`、`NonLiteralArgIsSend`、`WhileTrueEmitsBackwardJump`、`IfTrueAssignsOuterTemp`、`FalseIfTrueIsNil`、`ReturnInsideInlinedLoop`、`ToByDoNegativeStep`、`LoopVarCapturedPerIteration`、`InlinedLoopTempFreshEachIteration`、`LinkedListDoCountsLinks`、`image/tests/blocks.st` |
| S8 | `docs/bench.md` を再計測する（to:do: の展開で比 5.76 が変わる） | — |

### リスク

- vendor のコンパイルエラーは、このバッチでは 17 件から減らない（`,`、ブロック temp の構文、クラス変数が原因で、これらは B4 と B7 で直す）。件数が 17 以下であることを検査して、新しいエラーの混入を検出する。
- パーサを直すまで、テストでは `[:x | | t | ...]` の形を使わない。
- 既存イメージのコンパイル済みブロックは、再 Accept するまでコピーの意味論のまま動く。
- ジャンプは 16 bit。255 temp の上限に、持ち上げた temp とベクタのスロットも数える。
- abort に入る経路は `abortEvaluation` だけにし、B3 と B10 はこれを使う。

## B3 失敗の伝搬とキャッシュの無効化（テーマ 2）

方針:
- 失敗は値として返さない。B2 の `abortEvaluation` で最外フレームまで巻き戻し、途中の `ensure:` は実行する。`ao_eval` は `AO_ERR_EVAL` とメッセージを返す。
- SPEC の文言も合わせて改める。対象は §3.3 の手順 4、§3.10 の「空 OOP は AO_ERR_EVAL」、§6 の DNU 項目。新しい文言は「評価を中断し、`doesNotUnderstand: #sel` を返す」。
- 例外オブジェクトと `on:do:` による捕捉は v1 では扱わない（vendor の Exception は file-in するが、ハンドラ探索は行わない）。これも SPEC に明記する。
- 巻き戻しの理由は、B2 の `abortReason` を文字列の Oop（ルート済み）に広げたものに持たせる。

- 次の既定をネイティブにし、どれも `abortEvaluation` を呼ぶ（02 High、06 Medium）。
  - `Object>>doesNotUnderstand:`: 理由は `doesNotUnderstand: #sel`
  - `error:`: 引数の printString を理由にする
  - ネイティブの範囲外エラー（今は `error:` の文字列を返しているもの）
- `performSend` で、`result.isEmpty() && !unwinding(ctx)` のときは、原因となったセレクタ名を理由にして abort する。`classOf(空)` がクラスでない値を返す経路も塞ぐ（02 High）。
- `ClassMethodCache` に `flushSelector(sel)` と `flushAll()` を足す。`installMethod`、`putNative`、クラスの差し替え（`Behavior.cpp:274-285`、`Session.cpp:220`）を、すべてこの 1 つの関数から呼ぶ（00 High、05 High）。
- `ao_accept_class`: クラス定義メッセージかチャンクのヘッダだけを受け付け、それ以外は `AO_ERR_COMPILE`「not a class definition」にする（00 Critical）。
- `ao_accept_method`: 名前の解決結果が Behavior でなければ `AO_ERR`（00 Critical）。
- `ao_Array_equals` は、先頭で `==` なら true を返す。クラスの比較は `class ==` にそろえる（00 Critical）。相互に参照し合う配列の深い再帰は、B2 のスタックガードで「stack overflow」の abort になる。
- file-in: `fileInLoadOrder` はエラーが 1 件でもあれば非 0 を返す。`ao filein` と `ao image save --load-order` はファイル名付きでエラーを stderr に出し、exit 1 にする。`ao_filein_load_order` もエラーを返す。意図して除外するメソッドは `image/vendor/DEFERRED.md` に `Class>>selector: 理由` の形式で列挙する。`vendor_filein_test` では「エラー集合 = DEFERRED の集合」を固定する（05 Medium、06 High）。
- `ao --test`: 実行時の failure を失敗として数える。失敗の理由、コンパイルエラーの位置、ファイル名を stderr に出す。テストが 0 件なら exit 1（06 Medium / Low）。
- ABI の返り値:
  - count 系はエラー時に -1 を返す（06 Low）。
  - `ao_version` は切り詰めたとき `AO_ERR_RANGE` を返す（06 Low）。
  - `out == NULL` の評価は実行前に拒否する（00 High）。
  - `ao_image_load` は、探針に失敗したら `g_session` を差し替えない（探索で見つけた追加の欠陥）。
- テスト:
  - `(1 + nil) class` と `([:x | x] value) class` は、どちらも `AO_ERR_EVAL` になり、メッセージが空でない。
  - `Set new add: ([:x|x] value)` のあと、コレクションに 0 が残らない。
  - `ao --test` は `nil foo.` を失敗として数え、`1/0` も失敗として数える。
  - `ao_accept_class("3 + 4")` は `AO_ERR_COMPILE` になる。
  - `ao_accept_method("Processor", …)` は `AO_ERR` になる。
  - `Object>>zork` を再 Accept すると、`3 zork` に反映される。
  - チャンク経由で再定義すると、キャッシュ済みの送信にも反映される。
  - count 系の ABI は -1 を返す。
  - `ao_version(buf,4)` は `AO_ERR_RANGE` を返す。
- SPEC §6 の DNU 項目を `[x]` にする。

## B4 クラスのメタデータ（Kernel の形と名前）

- Bootstrap で Kernel クラスにも `instVarNames` を入れる。`fillInstVars` は親の `instSize` と名前の数が合わなければダミー名で詰める（05 Critical。04 の `instVarNamed:` もこれで直る）。
- `wireCycle` の後にクラス名を `wk.intern` の Symbol に入れ直す。メタクラスの `name` はクラス名から計算する（01 High）。
- `Smalltalk` を `SmalltalkImage` のインスタンスにし、名前から値への辞書を持たせる。`at:` / `at:put:` / `includesKey:` / `at:ifAbsent:` はネイティブ。`subclass:` と `WellKnown::define` の登録もここへ反映する。イメージの `checkGlobals` と `Globals::at` も合わせて直す（01 Medium、00 High「knownGlobals に Smalltalk が無い」）。
- クラス変数（05 High）:
  - `subclass:` の `classVariableNames:` を、classPool（名前から Association への辞書）として保持する。
  - `CompileEnv` にスーパークラス連鎖のクラス変数を渡し、B2 の `PushLitVar` / `StoreLitVar` を生成する。
  - 名前の解決順（ローカル → インスタンス変数 → クラス変数 → グローバル）を SPEC §3.8 に書く。
- テスト:
  - `R5OC new getX` が x を返す（親の内部スロットを返さない）。
  - `Object name == #Object` が true。
  - `Smalltalk at: #Zap put: 3` のあと、`Zap` が 3。
  - `Count := 1` を Accept でき、そのあと `^Count` が 1。
  - `String subclass: #PJ instanceVariableNames: 'tag'` がエラーになる。
- 親が bytes で、インスタンス変数を足そうとするクラス定義はエラーにする（04 Low）。

## B5 Browser と Workspace のデータ消失（テーマ 4）

runtime:
- クラス定義の再 Accept（00 Critical）:
  - superclass と instVarNames が同じなら、既存のクラスオブジェクトを保ったまま category と classVars だけを更新する。
  - 形が変わる場合は新しいクラスを作り、ソース表にあるメソッドをすべて再コンパイルして移す。ソースの無いメソッドが 1 つでもあれば、旧クラスのまま `AO_ERR_COMPILE` にする。
  - 既存インスタンスは移行しない。これを SPEC §3.9 に明記する。
- ソースの無いメソッド: `ao_browser_source` は新しいコード `AO_ERR_NOSOURCE = 5` とプレースホルダを返す。SPEC §3.10 に追記する（00 Critical）。
- `printOn:` は `#printString` を送る（00 High）。

app:
- ソース欄のテキストビュー 3 か所で、引用符・ダッシュ・テキスト置換、綴りの自動修正、`smartInsertDeleteEnabled` を無効にする（07 High）。
- `allowsUndo = true` にし、Edit メニューに Undo / Redo を足す。Print it は `insertText(_:replacementRange:)` 経由で挿入する（07 Medium）。
- Browser で未 Accept の変更があるときは、選択を変える前に確認シートを出す（07 Medium）。
- `NOSOURCE` のメソッドを表示するときは、ペインを読み取り専用にする。
- 送信先を明示する。クラスとプロトコルを選び、セレクタが未選択なら「新しいメソッド」として `ao_accept_method` に送る。クラスだけを選んでいる場合は `ao_accept_class` に送る（00 Critical）。
- Accept が成功したら、受け入れたセレクタと `user` プロトコルを選んでから `publish()` する（07 Medium）。
- Save Image / Open Image が失敗したら NSAlert を出す（00 High）。
- テスト: `insertText` による打鍵経路（`'` と `--`）、Undo、Accept 後の選択状態、NOSOURCE の読み取り専用。

## B6 イメージとセッションの堅牢化

- 保存を原子的にする。同じディレクトリの一時ファイルに書き、`fsync` してから `rename(2)` する。失敗したら一時ファイルを消す（06 High）。
- save 時に、NativeMethod の名前が `findName` で解決できるかを検査し、解決できなければ失敗させる。native block thunk には登録名を付けて再結合できるようにする（06 High）。
- ロード時の検証（06 Medium ×2、Low）:
  - flags は `Bytes|Weak` 以外が立っていたら拒否する。
  - klass はポインタ形で `kClassSlotCount` 以上のスロットを持つこと。
  - well-known のクラスと methodDict、CompiledMethod の最低限の形を確かめる。
  - 先にヘッダの 48 バイトだけを読み、サイズを検査してから本体を読む。
  - ABI 関数では例外を捕捉する。
- 再入ガード: `gInterpreterDepth > 0`（B10 以降は「実行中のプロセスがある」）のとき、save / load / shutdown / boot / eval / accept は `AO_ERR` を返す（06 Low）。
- transcript フックは abi 側の保持値を `sessionBoot` で ctx に反映する（06 Low）。
- 容量: B1 で load が heapBytes に合わせて old を確保するので、save 側では上限（SPEC に書く最大値）だけを検査する（06 High）。

## B7 コンパイラの構文とチャンク形式

- スキャナ:
  - `isBinaryChar` に `,` を足す（05 High）。
  - int64 を超える整数リテラルは、数字列を Literal に持たせ、`boxLiteral` で LargeInteger を作る（05 High / 03 High）。
  - Float は字句全体を `std::from_chars` に渡す（03 Medium）。
  - 指数: 仮数が整数なら整数のまま扱い、基数付きなら `radix^exp` を掛ける。inf は結果の値で判定する（05 Low）。
  - `lexBinary` で、先頭以外の `-` の後に数字が続くなら、その手前で切る（05 Medium）。
- パーサ:
  - ブロックでは、引数の後の `|` を必須にし、`||` を正しく扱う。replay は削除する（05 High）。
  - カスケードの各パートで、単項 → 二項 → キーワードの連鎖を解析する。先頭の送信が super なら、全パートを `SendSuper` にする（05 Low）。
  - 入れ子の深さを 256 段までにし、超えたら「nesting too deep」を返す（05 Medium）。
  - 宣言の検証: 引数への代入、擬変数名の宣言、同じスコープでの重複名をコンパイルエラーにする（05 Low）。
- チャンク（`chunk_test.cpp:89` の非標準の期待値は直す）:
  - `$'` と `$"` を文字リテラルとして扱う（05 High）。
  - 文字列やコメントの中でも `!!` を `!` に戻す（05 Medium）。
  - `! !` でセクションを閉じる（05 Medium）。
  - ヘッダとして判定するのは、`!` で始まるチャンクか、セクションが閉じた後の `X subclass: #Y` の形だけにする（05 Low）。
- vendor: `{}` と `<primitive: 'name'>` は Blue Book の構文ではない。該当するメソッドは `image/patches/` で最小限に書き換えるか、DEFERRED に載せる。B3 のテストで vendor のエラーが 0 件（DEFERRED を除く）になるようにする。

## B8 Kernel の意味論（数値とスカラー）

- 混合演算: Integer の算術・比較ネイティブで、引数が Float か Fraction なら委譲する。`asFloat` は LargeInteger の全域を扱えるようにする。整数と double の比較は厳密に行う（03 High）。
- Fraction の `=` と `<` をネイティブで足す（03 Medium）。
- `bitShift:` の -2^63 は先に判定する（03 Medium）。
- Float に `>=` と `<=` のネイティブを置く（NaN の扱い。03 Low）。
- 値ベースの `hash` を LargeInteger、Float、Point、String、Symbol、Array に足す（03 Low、04 Medium）。
- Boolean: `True>>&` と `False>>|` は引数をそのまま返す。`eqv:` と `xor:` は、引数が Boolean でなければ失敗させる（03 Low）。
- Point: 成分の計算が空 OOP なら失敗にする。サブクラスは `inheritsFrom` で判定し、割り当てるサイズは instSize から取る（03 Low ×2）。割り当てるサイズは B6 で対応済み（`x:y:`、`origin:corner:` と、同じ型の `Association key:value:`、`Interval from:to:by:` は `allocateInstance` でクラスの instSize を割り当てる）。`inheritsFrom` での判定は残っている。
- `asCharacter` はサロゲートを拒否する（03 Low）。
- `to:do:` の終端が SmallInteger でないときは、汎用ループで回す（03 Low）。

B8 の実施結果（計画からの逸脱と、実装時に決めたこと）:

- 済みを確かめた項目: Magnitude の `<=` / `between:and:` の GC 安全性（B1）、int64 を超える整数リテラルと Float リテラルの丸め（B7）、`LargeInteger::box(ctx)` の allocateRetry（B1）、クラス側の生成の instSize（B6）。
- Fraction にも値ベースの `hash` を足した。Fraction の `=` を足したので、`=` と `hash` の契約を保つために要る。
- Magnitude に `max:` と `min:` を足した（Blue Book）。Kernel に無かったが、03 は Fraction の `max:` が動くことを求めている。
- Integer、Float、Fraction に `> <= >=` のネイティブを置いた（計画は Float の `>=` と `<=` だけ）。Magnitude の既定は `<` の否定なので、Integer をレシーバとする NaN の比較も true になるからである。引数が数でなければ Magnitude の既定に任せ、利用者の Magnitude との比較は従来どおり動く。
- 型をまたぐ `=` は false のまま（`1 = 1.0`、`(1/2) = 0.5`）。順序は値で決めるので、`1 <= 1.0` は true、`1 = 1.0` は false になる。Symbol の `=` も変えず（`#abc = 'abc'` は true）、`hash` を String と同じ関数にそろえた。
- `//` `\\` `quo:` `rem:` とビット演算は Integer どうしのまま（Float と Fraction には無い）。SPEC に明記した。
- Array と Point の `hash` の入れ子の上限は、`CallContext::hashNesting` で数える（4 段、Array は先頭 16 要素）。B10 でプロセスごとの状態に移すなら、この数も含める。
- 03 の Rectangle の例は、Kernel に `@` と `Point>>corner:` が無いので、`Point x:y:` と `Rectangle origin:corner:` で書いた（`@` は 03 が SPEC の必須外として外した Number プロトコル）。
- インライン展開しない `to:by:do:` は Kernel のネイティブに無く、`doesNotUnderstand:` のまま（SPEC §3.6 の必須外）。インライン展開した `to:do:` と `to:by:do:` は、`<=` と `+` が Float と Fraction を受けるようになったので、変更なしで同じ意味になった。
- 残る懸念（B8 レビューで判明、後続で扱う）: `magDivMod` と `gcd` は 1 ビットずつ割るので、巨大な Fraction の生成が遅い（20 万ビットで 5.6 秒）。B8 では右シフトだけを桁とビットの移動に直した。

## B9 Kernel の意味論（コレクションとストリーム）

- Dictionary と Set:
  - `hash` を使うハッシュ表（開番地法と線形プローブ）にし、拡張時は再ハッシュする（04 High。B8 の hash 契約を前提にする）。
  - `do:` は値を渡す。`associationsDo:`、`keysDo:`、`keysAndValuesDo:`、`at:ifAbsent:`、`removeKey:`、`removeKey:ifAbsent:` を足す（04 High）。
  - nil キーは拒否して失敗させる（04 Medium）。
  - `collection_do_test.cpp:111-126` の期待値を直す。
  - `consumeHash`（`Dictionary.cpp` の `consumeHash`、`Collection.cpp` の `includes:`）は `hash` の答えを捨てる。このため、B8 で入った `hash` の失敗（Integer でない答え、空 OOP）が黙って成功になる。ハッシュ表化の際に、答えが Integer でなければ失敗として扱う（B8 レビューで判明）。
- `select:` と `reject:` は 1 パスにする（04 Medium）。
- Stream:
  - `contents` は `at:` で要素を取り出し、コレクションと同じ種類で作る（04 Medium）。
  - 多バイト文字の `nextPut:` は String を差し替える（04 Medium）。
  - String 専用の `do:` と `contents` は 1 パスにし、WriteStream は倍々で拡張する（04 Medium、性能）。
  - ReadWriteStream の `contents` は readLimit まで返す。ReadStream の `nextPut:` は失敗させる（04 Low ×2）。
- Interval: 刻みの向きは `step < 0` の送信で決める。比較の結果が Boolean でなければ失敗させる（04 Medium）。
- Symbol の `copy` は self を返す。`basicNew:` は 2^32 以上を拒否する。OrderedCollection の `at:` は範囲外で失敗させる（04 Low ×3）。
- `Dictionary` の内部スロット位置を共有ヘッダの定数にする（00 Low）。
- identity hash のサイドテーブルは実装せず、SPEC §3.1 を「16 bit、衝突は許容」に改める（01 Low）。

B9 の実施結果（計画からの逸脱と、実装時に決めたこと）:

- ハッシュ表の配置: エントリごとにキーの hash を保存する（Dictionary は `key value hash` の 3 スロット、Set は `element hash` の 2 スロット）。拡張と削除は保存した hash を使い、`hash` も `=` も送らない。途中で利用者のコードが走らないので、巻き戻しで表が半端な状態に残らない。削除は tombstone を使わない後方シフトにした。容量は 8 以上の 2 のべき乗で、挿入後の数が容量の 3/4 を超える前に 2 倍にする。配置と送信しないヘルパーは `runtime/include/ao/HashedCollection.hpp`（`ao::Hashed`）に置き、`Dictionary.cpp` と `ClassPool.cpp` が共有する（00 Low。`Session.cpp` の直書きは B9 の前に無くなっていた）。
- 探索の開始位置は hash を混ぜて求める（`hash × 0x9E3779B97F4A7C15` の上位ビット）。B9 レビューで、混ぜないと 2 のべき乗にそろったキーで約 30 倍、9 万個の IdentitySet で 8 秒（Release）かかることが分かったため。
- 比べ方: 保存した hash が等しいエントリだけ比べる。同一なら `=` を送らない（NaN のキーも引ける）。`hash` が Integer 以外、`=` が Boolean 以外なら失敗する。Identity 版は送信せず identityHash と `==` で比べる。`consumeHash` は廃止し、`hash` を送る処理は `sendHash`（B8 の入れ子の規則つき）にまとめた。
- `Collection>>includes:` は、計画の「hash の答えを検査する」をやめ、hash を送らないことにした。線形探索では hash の値を使わず、`Dictionary>>includes:` も送らないからである（B9 レビューで判明）。送るのは Blue Book どおり `anObject = 要素` で、同一の要素には送らず true、答えが Boolean でなければ失敗する。
- nil のキーと要素は失敗する（`key must not be nil`、`element must not be nil`）。`Dictionary>>at:` は、キーが無ければ今までどおり nil を答える（Blue Book の失敗には変えていない）。
- 壊れた表（array の形が合わない、tally が範囲外）は `damaged hashed collection` で失敗する。tally が実際の数と違うだけなら、探索・挿入・削除・列挙は失敗しない。`size` と `collect:` は tally を信じる。
- Dictionary と Set の `copy` は array も写す（以前から array を共有していた。B9 レビューで判明）。`shallowCopy` は浅いまま。
- classPool: ハッシュ表では書いた順を保てないので、名前の並びはバイト順にした（Squeak のクラス定義もソート済み）。再 Accept は名前を集合として比べる。名前として扱うのは Symbol のキーだけである。
- イメージの形式を版 3 に上げ、版 2 は `unsupported image version 2` で拒否する（v2 の辞書は先頭から詰めた配置で、新しいネイティブでは引けない）。
- Interval: 刻みの向きは `step < 0` と `step > 0` を送って決める（どちらも false なら 0 要素）。続ける条件は肯定形（前向きは `要素 <= stop`）にし、端点が NaN なら 0 要素にした（B9 レビューで判明）。2^20 での打ち切りは外した。端点と刻みがすべて Integer なら、size を送信なしに LargeInteger 演算で求める。
- OrderedCollection: 範囲外の `at:` は `at: index out of range`。firstIndex・lastIndex・array の組が壊れていれば、`size`・`do:`・`add:`・`at:` は `damaged ordered collection` で失敗する（B9 レビューで判明。`do:` が Heap の assert で落ちていた）。
- `select:`・`reject:` は 1 パスにした。`select:`・`reject:`・`detect:ifNone:` の述語の答えは、`to:do:` と同じく `mustBeBoolean` を通す（今までは Boolean 以外を黙って捨てていた）。thunk 名 `ao_Collection_filter_count` と `ao_Collection_filter_fill` は `ao_Collection_filter_scan` にまとめた。版 3 は未リリースなので旧名を残していない。
- Stream:
  - `contents` は `at:` で要素を取り出す。答えの種類は、String の系統は同じクラス（Symbol は String）、可変長の ArrayedCollection は同じクラス、OrderedCollection の系統は OrderedCollection、それ以外は Array にした。Kernel の Interval は `at:` を持たないので、`contents` は DNU になる。
  - String への書き込みでは、writeLimit を collection の文字数（容量）にし、readLimit の後ろに NUL の予備を置いて倍々で伸ばす。スロットは増やしていない。`position:` は readLimit と今の position の大きい方で頭打ちにし、予備を見せない。
  - O(n) を約束するのは末尾への追記だけにした。readLimit より前への書き込みは、前に多バイト文字があれば 1 回 O(position) かかる。幅の違う上書きは String を作り直すので O(大きさ) である。
- `String>>do:` は UTF-8 を 1 回たどる。ブロックが文字の幅を変えたら、渡した文字の数から位置を合わせ直す。文字列に無い文字を渡すことはない（B9 レビューで判明）。
- 残る懸念（後続で扱う）:
  - String 上の ReadStream の `next` は、`at:` を送るので 1 回 O(i) かかる（5 万文字で約 10 秒）。
  - readLimit より前への多バイト文字の上書きを繰り返すと 2 乗になる。
  - `ocGrow` は、使っている数によらず配列を倍にする（B9 より前からの動作）。そのため、`add:` と先頭からの取り出しを繰り返す FIFO 的な使い方では、配列が際限なく大きくなる（10 万回で size 0、配列 65536）。`Processor yield` の ready キューもこの使い方なので、B10 で「使っている数の 2 倍が配列に収まるなら、その場で詰める」ように直す。

## B10 協調スケジューラ（02 High）

前提: B2（共有 temp、`ensure:`、起動ごとのアクティベーション、スタックガード）、B3（failure による巻き戻し）、B6（再入ガード）が入っていること。

### ファイバ

AArch64 のコンテキスト切り替えを自前で書く。

- `runtime/src/FiberSwitch_arm64.S` の `ao_fiber_switch(save, load)` は、x19–x28、fp、lr、sp、d8–d15 の 168 バイトを退避・復元する。x18 には触らない。
- トランポリン `_ao_fiber_trampoline` を置く。
- オフセットは `FiberRegs.h` に `#define` で書き、C++ 側では `static_assert` で確かめる。
- 検討して退けた案:
  - ucontext: macOS では非推奨で、`-Werror` との相性が悪い。切り替えのたびに sigprocmask も呼ぶ。
  - C++20 コルーチン: スタックを持たないので、再帰するネイティブの下では中断できない。
  - pthread: フックはメインスレッドで呼ぶ必要がある。
  - Boost.Context: SPEC §5.5 の依存制限に反する。

スタック（`runtime/src/Fiber.{hpp,cpp}`）:
- `mmap` で確保し、先頭にガードページ（`sysconf(_SC_PAGESIZE)`、16 KiB）を置く。
- 大きさは 1 本 8 MiB（遅延コミット）。
- プールには 4 本まで残し、戻すときに `madvise(MADV_FREE)` する。
- 生きているファイバは 256 本までとし、それを超える `fork` は失敗させる。

### SPEC §3.4 に書く意味論

- ベースプロセス:
  - boot またはロード時の `Processor activeProcess` をベースプロセスとし、ホストのスレッドのスタック上で動かす。`ao_eval` はすべてベースで実行する。
  - 他のプロセスは、同じスレッド上のファイバ 1 本ずつで動かす。
- 実行可能キュー:
  - 単一の FIFO（Processor の slot 0）。`priority:` は保存するが使わない。プリエンプションは無い。
- 各操作:
  - `fork`: 実行可能キューの末尾に入れる。実行されるのは、アクティブなプロセスが yield / wait / suspend するか終了したとき。
  - `yield`: キューが空ならすぐに返る。
  - `wait` / `signal`: 待ち手はキューの末尾に移すだけで、その場では切り替えない。
  - `SharedQueue`: セマフォを通してブロックする。
  - `suspend` / `resume`: キューとの出し入れだけを行う。
  - `terminate`（新しいネイティブ）: 対象のプロセスを巻き戻して終わらせる。その間、`ensure:` は実行される。
- デッドロック: ベースがブロックし、ほかに実行できるプロセスが無ければ、`wait` は「deadlock: no runnable process」で失敗して `AO_ERR_EVAL` になる。`activeProcess` はベースのまま（nil になる既報の不具合も直る）。
- doIt の終わり:
  - 結果と printString を先に確定する。
  - そのあと `drain(1000 ラウンド)` を実行し、実行可能キューが空になるまで yield する。
  - 待ち状態のプロセスは、次の eval まで生き残る。
- fork したプロセス内のエラー、DNU、別プロセスのホームへの `^`:
  - そのプロセスだけを終了させる。
  - `processFailures` の件数に数え、`ao --test` はこれを失敗として扱う。
- save / load:
  - save は eval の合間にだけ許す。C のスタックはイメージに書かない。
  - load のあとは実行可能キューを空にする。開始済みだったプロセスは死んだものとして扱う。
  - 旧セッションのファイバは、`ensure:` を実行せずに（abandon で）破棄する。
- shutdown とセッションの破棄: ヒープが残っているうちに `terminateAll(abandon)` を呼ぶ。

### プロセスごとの状態

- `FiberRecord` の中身:
  - ルートした Process
  - 専用の `CallContext`
  - `FiberStack`
  - 退避したレジスタ
  - 状態（NotStarted / Ready / Waiting / Suspended / Dead）
  - `terminateRequested` と `resumeTo`
- ファイバごとに `CallContext` を持つ理由: `thread_local gInterpreterDepth` と、最外フレームによる `activeContext` / `nonlocal*` のルートは、1 つの ctx を共有すると壊れる。そこで `gInterpreterDepth` を廃止し、`ctx.depth` に移す。
- `CallContext` のフィールドの扱い:
  - ベースから写すもの: heap、roots、wk、cache、scheduler、フック（フックは切り替えのたびに更新する）。
  - ファイバごとに持つもの: activeContext、nonlocal 系、failure、depth、stackLimit。
  - ファイバ側では常に null: hostTemps。
  - ベースへ畳み込むもの: `testFailures` と `interpretedBytecodes`。切り替えで離れるたびに加算する。
- ワークスペース束縛は B2 で Association にするので、fork 先で代入しても辞書に反映される。

### ファイルごとの変更

- 新規:
  - `runtime/include/ao/Scheduler.hpp` と `runtime/src/Scheduler.cpp`（fork、resume、suspend、terminate、yield、wait、signal、drain、terminateAll、adoptImage、switchTo、fiberMain、reapDead）
  - `Fiber.{hpp,cpp}`、`FiberRegs.h`、`FiberSwitch_arm64.S`（`__aarch64__` でなければ `#error`）
- 既存:
  - `NativeMethod.hpp`: `CallContext` に `Scheduler*`、`depth`、`stackLimit` を足す。
  - `Interpreter.cpp`: `DepthGuard` は `ctx.depth` を使う。スタックガードは `ctx.stackLimit` を読む。
  - `kernel/Process.cpp`: ネイティブを Scheduler の薄い包みにし、`ao_Process_terminate` を足す。SharedQueue と `signal` で、切り替えをまたいで生のまま持っている Oop をルートする。
  - `Session.{hpp,cpp}`: Scheduler を持つ。`adoptImage`、eval 後の `drain`、load / shutdown / デストラクタでの `terminateAll` を呼ぶ。
  - `abi.cpp`: busy ガード（B6 と共通）。
  - `TestRunner.cpp`、`ao_main.cpp`: ファイルごとに drain と terminateAll を呼ぶ。
  - `test_support.hpp`: `Boot` に Scheduler を持たせる。
- CMake:
  - `project(ao LANGUAGES CXX ASM)` にする。
  - 警告のフラグは `$<$<COMPILE_LANGUAGE:CXX>:...>` で C++ に限る。
  - `fiber_test.cpp` を足す。
  - ASan ビルドでは `__sanitizer_start_switch_fiber` と `__sanitizer_finish_switch_fiber` を呼ぶ。

### 手順（各段を緑にしてから次へ進む）

1. SPEC の §3.2、§3.4、§3.6（`terminate`）、§3.10、§4.1 を直す。
2. `fiber_test`:
   - 1 万回のピンポンで、int と double のローカルが保たれる。
   - ガードページが PROT_NONE になっている。
   - プールが再利用される。
3. depth と stackLimit を ctx へ移す。既存のスイートは全部緑のまま。
4. fork と yield:
   - 状態遷移だけを見ていた既存テスト 4 本を書き直す。
   - `ForkRunsOnlyAfterYield`、`ActiveProcessInsideForkIsForked`、`ForkFifoOrder`、`YieldEmptyReturns`
5. ブロックする操作: `WaitBlocksUntilSignal`、`BaseDeadlockIsFailureActiveStaysBase`、`SharedQueueProducerConsumer`、`SharedQueueEmptyNextDeadlock`
6. 終了とエラー:
   - `ForkDnuTerminatesOnlyFork`、`ForkNlrToBaseHomeTerminates`、`TerminateWaiterRunsEnsure`、`RecursionInForkFailsNoCrash`
   - 50 本のファイバを待たせた状態で、GC ストレスと `collectOld` を回す。
7. session_abi_test:
   - `DoItDrainsTranscriptFork`、`PrintItBeforeDrain`、`WaiterSurvivesAcrossEvals`
   - `SaveLoadWithWaitersKeepsBaseActive`
   - `ShutdownReclaimsFibers`（Roots の件数が元に戻る）
   - `ReentrantEvalFromHookRejected`
   - レビューの式 `[n := n + 1] fork. Processor yield. n` が `1` になる。
8. ゴールデン `image/tests/process.st`: fork の順序、セマフォのピンポン、SharedQueue、`activeProcess` の同一性。

### リスク

- スケジューラの呼び出しはすべて GC 点になる。B1 のストレスモードで検出する。
- ルートが残ったままスタックを解放すると、Roots にぶら下がりが残る。Debug では、`fiberMain` の入口と出口で Roots の件数が一致することを assert する。
- フックの Swift コードも、ファイバのスタック上で動く。
- `yield` しない無限ループはアプリを止める（B11 で SPEC に明記する）。
- 対象は arm64 だけ。arm64e のポインタ認証を有効にすると、lr の差し替えが壊れる。

B10 の実施結果（計画からの逸脱と、実装時に決めたこと）:

- 着手時に見つけた計画の穴:
  - Roots の LIFO 部分（ネイティブ呼び出しのフレームと `RootedArray` の範囲）は、ファイバをまたぐと積み順が交錯する。そこでこの部分を `Roots::Stack` にまとめ、プロセスごとに持たせて切り替えのたびに差し替える。GC は、走っている分と駐車中の分をちょうど 1 回ずつたどる。`add` / `remove` のスロットとハンドル表は共有のままにした。
  - スタックガードは、スレッドのスタックしか知らなかった。ファイバの上では即座に stack overflow になるので、`CallContext` にファイバのスタックの範囲を持たせた。
  - abandon でスタックを解放するだけだと、Roots にファイバのスタックを指すエントリが残る。そこで abandon は、後始末ブロックを走らせない terminate とした。ファイバに切り替え、C++ のフレームを正常に巻き戻す（`ctx.abandoning`）。
- 実行可能キューの正本は C++ の deque にした。`quiescentProcesses`、myList、activeProcess は、切り替えのたびにそれへ合わせる。状態から NotStarted は外し、`started` の印で表す（まだ始まっていないプロセスは、実行可能か止まっているかのどちらかである）。
- `SharedQueue>>nextPut:` は待たない（Blue Book どおり上限なし）。writeSynch は形だけ残して使わない。今までの実装は writeSynch を wait していたので、wait が本当にブロックするようになると、ベースで 2 回続けて送った時点でデッドロックしていた。
- 走れないプロセスは、ベースでもこのセッションで fork したものでもない Process（`Process new` やイメージにあったもの）と、終わったプロセスである。`resume` は `process cannot run` で失敗し、`signal` は捨てる。
- terminate の決定:
  - ベースへの terminate は `process terminated` で失敗する。
  - 後始末が yield / wait / suspend したら、送ったプロセスに戻る。
  - signal を受けてまだ `wait` から戻っていないプロセスを terminate すると、その signal をセマフォに返す（取りこぼし防止）。
- busy は `interpreterRunning(base)` の 1 か所で判定する。ベースの `ctx.depth > 0` か、ベース以外のプロセスが走っているときである。`AbiEntry` は先に `g_entered` を取り、そのあとでセッションを読む（差し替え中のセッションを読まないため）。
- drain は `ao_eval` と `ao --test` だけで行う。ワークスペースの作成、探針、accept、file-in で fork したプロセスは、次の eval に持ち越す。
- `ao --test` は、ファイルごとに drain → terminate → drain を進展が無くなるまで（上限 1000 回）繰り返す。そのあとも後始末がブロックしたまま残ったものだけを abandon する。
- B9 の積み残し（`ocGrow` が FIFO の使い方で際限なく伸びる）は、使っている数の 2 倍が配列に収まるなら先頭へ詰める形で直した。
- Codex レビューで直したもの（すべて Medium と仮説 1 件）:
  - `NativeMethod::invoke` のフレームの pop を RAII にした。ファイバ内の C++ 例外で Roots にフレームが残り、Debug の assert でホストが落ちていた。
  - `SharedQueue>>next` は、`wait` から戻って中身が空なら待ち直す。suspend と resume を挟むと、要素数と excessSignals がずれていた。
  - out of memory の印をプロセスごとの状態にした。ファイバの割り当て失敗が、ベースの正常な `ao_eval` を失敗にしていた。
  - `terminateAll(false)` は、後始末が yield しただけのプロセスまで abandon していた。
  - `signal` と `nextPut:` は、実行可能キューに入れられなければリストを変えない。`add:` の失敗は、空の答えのまま見過ごさず abort する。
  - 修正コミットの再レビューで見つかった 1 件も直した。`enqueue` は C++ の deque に先に積む（拡張が例外を投げても何も変わらない）。`nextPut:` は、起こす途中の例外でも足した要素を外す。
- 残る懸念（後続で扱う）:
  - 保存するとき、駐車中のファイバがルートした値も、どこからも参照されないオブジェクトとしてイメージに入る（ロードの検査は通る）。
  - `ctx.testFailures` は、失敗の行 1 つにつき 1 を数える。abort とプロセスの失敗が同じファイルで起きれば 2 である。
  - `yield` しない無限ループはアプリを止める（B11 で SPEC に明記する）。

## B11 App とビルドの残り

- Transcript は `textStorage.append` で差分だけを追加し、スクロールは評価の最後にまとめる（07 Medium）。
- vendor を `Contents/Resources/vendor` に同梱し、`Bundle.main` から探す。開発時は環境変数 `AO_VENDOR_DIR` で場所を渡す（07 Medium）。
- 評価結果の長さ:
  - ABI に `ao_eval_result_length` と `ao_eval_result_copy` を足し、直前の結果をセッションに持たせる。
  - `ao_eval` は `AO_ERR_RANGE` のときに必要な長さを返す。
  - Swift 側は長さを指定して読む（07 Medium、Low「NUL」）。
  - SPEC §3.10 に追記する。
- Inspector は `willCloseNotification` で配列から外す（07 Low）。
- `AoSpan` の UTF-8 オフセットを UTF-16 に換算して、エラー位置を選択表示する（07 Low）。
- 評価の中断は v1 の範囲外として SPEC §3.9 に明記する（07 Low）。B10 の後は、`[true] whileTrue` のような無限ループは既知の制約として扱う。
- `package-app.sh`: 先に `build.sh` を実行し、Release でビルドする。`codesign --force --sign -` を実行し、CFBundleVersion を入れる（07 Low）。
- テストの後始末: ウィンドウを閉じ、フックを外す（07 テストの穴）。
- テストを足す:
  - 日本語を含む選択範囲と、その Print it の挿入位置
  - 64 KiB を超える結果と、NUL を含む結果
  - Transcript に 2 万行を出したときの所要時間の上限
  - `open build/Ao.app` で起動したときに vendor が読み込まれること（`test.sh` から実行する）

---

## 検証（全体）

- 各バッチ: `scripts/test.sh` が緑、`AO_GC_STRESS=1 ctest` が緑、`build/ao --test image/tests` が exit 0。
- ASan / UBSan ビルドで ctest を 1 回回す（B1 と B10 の後は必須）。
- レビューの各失敗シナリオを回帰テストとして持つ（runtime は GoogleTest、Smalltalk は `image/tests/*.st`、app は XCTest）。
- vendor の file-in: エラー集合が DEFERRED と一致する。
- 手動: `open build/Ao.app` で起動して、次を確認する。
  - Transcript に vendor の読み込み結果が出る。
  - Workspace で `'a', 'b'` を打鍵して Print it できる。
  - Browser で編集中に選択を変えると確認が出る。
  - 読み取り専用の場所に保存すると、アラートが出る。
- 最後に SPEC §6 のチェックリストを更新し、`graphify update .` を実行する。

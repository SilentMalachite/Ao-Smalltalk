# 02 インタプリタ・送信・コンテキスト・プロセス

対象: `runtime/src/{Interpreter,Send,Context}.cpp`, `runtime/include/ao/{Interpreter,Send,Context}.hpp`, `runtime/src/kernel/{BlockContext,Process,CompiledMethodNatives}.cpp`, `compiler/include/ao/{Bytecode,MethodImage}.hpp`, `runtime/tests/{block_test,context_test,process_test,native_send_test,collection_do_test}.cpp`（NLR の相手側として `runtime/src/kernel/Dictionary.cpp` の反復ネイティブも確認）

検証: コードを読んだうえで、既存の `build/runtime/libao_runtime.a` と `build/compiler/libao_compiler.a`（CMake 既定の Debug ビルド）に C ABI（`ao_accept_class` / `ao_accept_method` / `ao_eval`）でリンクしたプローブを scratchpad で実行した。DNU の件だけは内部 API（`test_support.hpp` の `Boot`）を使った。リポジトリのビルドディレクトリには触れていない。

## 概要

| 重大度 | 件数 |
|---|---|
| Critical | 1 |
| High | 3 |
| Medium | 2 |
| Low | 3 |

## 指摘

### [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる
- 場所: `runtime/src/Interpreter.cpp:242-251`（`DepthGuard` / `gInterpreterDepth` は数えるだけで上限を見ない）、`runtime/src/Interpreter.cpp:440`、`runtime/src/Send.cpp:132`
- 問題: Smalltalk の送信は 1 回ごとに `send → applyMethod → Interpreter::run` と C++ の再帰を 1 段積む。深さの上限もスタック残量の検査もない。
- 失敗シナリオ: `R2A>>recur: n ^self recur: n + 1` を accept して `R2A new recur: 0` を Print it すると SIGSEGV（exit 139）。有限の再帰でも `depth: n  n = 0 ifTrue: [^0]. ^(self depth: n - 1) + 1` は 2,406 段なら通り、2,437 段で SIGSEGV になる（8 MB スタック、Debug）。`ulimit -s 65520` にすると 5000 段が通るので、原因はスタック溢れである。アプリは `ao_eval` をメインスレッドで呼ぶので、ユーザーの `foo ^self foo` の一行でアプリが終了し、保存していない作業が失われる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `Interpreter::run` の入口で `gInterpreterDepth` の上限、または `pthread_get_stackaddr_np` から求めたスタック残量を検査する。超えたら空 OOP を返す、またはエラーオブジェクトを返すかたちで巻き戻す。

### [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）
- 場所: `runtime/src/Interpreter.cpp:278-291`（NLR は `ctx.nonlocalReturn` フラグと空 OOP で伝える方式）。フラグを見ない反復: `runtime/src/kernel/Dictionary.cpp:420`（Dictionary do:）、`:450`（Dictionary collect:）、`:503`（Set do:）、`:589`（OrderedCollection do:）、`:745/:753/:774`（Interval do:）。フラグを見る実装は `Collection.cpp:175`（Array do:）と `SmallInteger.cpp:181`（to:do:）だけ。
- 問題: ブロックの `^` はフラグを立て、空 OOP を返して C++ スタックを巻き戻す。そのため途中のネイティブは、ブロックを呼ぶたびにフラグを確認して即座に戻らなければならない。上に挙げたネイティブは戻り値もフラグも見ずに反復を続ける。
- 失敗シナリオ: `firstOf: c  c do: [:e | ^e]. ^nil` は、Array なら 1 を返すが、OrderedCollection(1 2 3) では **3** を返す。`c do: [:e | log add: e. ^e]` のあとの `log size` は、Array では 1、OrderedCollection / Interval / Set / Dictionary / Dictionary collect: では **3** になる。`^` のあとも各要素でブロック先頭の送信が実行される。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: ブロックを呼ぶ共通ヘルパーを作り、戻り値が空 OOP またはフラグ成立なら即座に空 OOP を返すようにして、全反復ネイティブをそこに寄せる。あわせて、Kernel ネイティブの NLR 回帰テストを型ごとに足す。

### [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける
- 場所: `runtime/src/Interpreter.cpp:408`（`stack.push(result)` が空 OOP を検査しない）、`runtime/src/Send.cpp:88-93`（DNU メソッドが見つからないと Message をそのまま返す）、`runtime/src/WellKnown.cpp:285`（`classOf(空)` が nil を返す）
- 問題: ネイティブの失敗やブロック引数個数の不一致（`Interpreter.cpp:424`）は空 OOP を返す。NLR でなければ、呼び出し側フレームはそれをスタックに積んで実行を続ける。空 OOP への送信は、クラス nil での探索失敗から DNU に進み、DNU も見つからないので `Message` が結果になる。SPEC §479 は「空 OOP は `AO_ERR_EVAL`」としているが、失敗が式の途中で起きると AO_OK と無意味な値になる。
- 失敗シナリオ: `([:x | x] value) class` は `Message`（AO_OK）、`(1 + nil) class` は `Message` になる。`r setv: ([:x | x] value). r v == nil` は Boolean でなく `Message` を返す。`Set new add: ([:x|x] value)` や `Dictionary new at: (…) put: 1`、`instVarAt:put:` では、生の 0 がヒープオブジェクトのスロットやコレクションに残る。トップレベルの `[:x | x] value` は AO_ERR_EVAL になるが、メッセージは空である。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `performSend` で `result.isEmpty() && !ctx.nonlocalReturn` のとき、フレームを失敗として巻き戻すか、`error:` 相当のエラーオブジェクトに置き換える。どちらにするかを SPEC に明記する。`classOf` が空 OOP でクラスでない値を返す経路も塞ぐ。

### [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）
- 場所: `runtime/src/kernel/Process.cpp:399-414`（fork）、`:264-286`（yield）、`:324-348`（wait）、`:385-397`（SharedQueue next）
- 問題: スケジューラは `activeProcess` とキューを書き換えるだけで、プロセスの文脈を実行する経路がどこにもない。SPEC §3.4 の「協調的グリーン・スレッド」と、P4-07 の「スケジューラが 1 プロセスを回せる」を満たしていない。
- 失敗シナリオ: `| n | n := 0. [n := n + 1] fork. Processor yield. n` は 0 を返す（本体は一度も走らない）。`Semaphore new wait. #x` は即座に続行する。`SharedQueue new next` は空なのに待たず nil を返す。`Semaphore new wait. Processor activeProcess` は **nil** になり、以後のセッションは実行中のコードに対応するアクティブプロセスを失ったままになる。
- 根拠: プローブで再現（確度: 高）。`process_test.cpp` は状態遷移しか見ておらず、本体が実行されることを検証していない。
- 修正の方向: 最低限、`yield` / `wait` で待ち状態のプロセスのブロックを実際に評価する協調スケジューラを入れる。v1 で見送るなら、SPEC と P4-07 の受け入れ条件を「ブックキーピングのみ」に改め、`fork` / `wait` を明示的なエラーにする。

### [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する
- 場所: `runtime/src/Interpreter.cpp:287-290`（最外フレームでフラグを消して miss を返す）、`:683-695`
- 問題: ホームのメソッドコンテキストがすでに返っている場合、スタック上の一致するフレームを探して最外フレームまで全フレームを巻き戻し、空 OOP で終わる。Blue Book の `cannotReturn:` に当たる処理がなく、無関係な呼び出し元も一緒に打ち切られる。
- 失敗シナリオ: 1 回目の Do it で `b := [:x | ^x]` を評価し、2 回目で `log add: #before. b value: 3. log add: #after. #done` を評価すると、`AO_ERR_EVAL` になり、out も err.message も空になる。`#after` は実行されず、ワークスペース変数も書き戻されない。メソッド内で `makeBlock ^[:x | ^x]` を後から評価した場合も同じ。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `ReturnBlock` の時点で、ホームが生きているかどうか（アクティブなフレームの集合に含まれるか。例えば返ったコンテキストに印を付ける）を判定する。死んでいれば `cannotReturn:` を送るか、エラーオブジェクトを返してブロック内で処理を完結させる。

### [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る
- 場所: `runtime/src/kernel/Install.cpp:53-60`（BlockContext は `value` / `value:` / `value:value:` / `valueWithArguments:` のみ）。`whileTrue:` / `whileFalse:` / `timesRepeat:` / `repeat` / `ensure:` / `ifCurtailed:` はランタイム全体で未定義で、コンパイラもインライン化しない。
- 問題: SPEC §0 は Blue Book の言語意味論に準拠するとしているが、条件ループと 3 引数以上のブロック呼び出しがない。前項の「DNU の既定は Message を返す」と組み合わさり、ループ本体が一度も実行されないまま成功扱いになる。
- 失敗シナリオ: `[false] whileTrue: [1]`、`3 timesRepeat: [...]`、`[1] ensure: [2]`、`[:a :b :c | a] value: 1 value: 2 value: 3`、`[:x|x] numArgs` は、いずれも `Message` を返して AO_OK になる。vendor の Cuis ソース（`Exception.st`、`Heap.st` など）はこれらに依存している。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `whileTrue:` 系・`timesRepeat:`・`value:value:value:(value:)`・`numArgs` を BlockContext / Integer のネイティブとして足す（NLR のフラグ検査も入れる）。あるいはコンパイラでインライン化する。ensure: を v1 で見送るなら SPEC に書く。

### [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる
- 場所: `runtime/src/Send.cpp:63-78`
- 問題: `allocateMessage` は `heap.allocate` を直接呼んでいる。`allocateRetry` を通さないので、ナーサリが満杯だと失敗する。
- 失敗シナリオ: ナーサリを埋めた状態で `3 zork` を送ると、`Message` ではなく空 OOP が返る。同じ状態の `Object new` は GC 後に成功する。safepoint 間で大量に割り当てたあとに DNU が起きると、上記 High と同じく黙った失敗になる。
- 根拠: プローブで再現（確度: 高。実運用で起きる頻度は低い）
- 修正の方向: `allocateRetry` を使い、`msg` を Root に載せてから引数配列を割り当てる。

### [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する
- 場所: `runtime/src/kernel/BlockContext.cpp:57-72`
- 問題: クラスを検査せず、ポインタオブジェクトであれば全スロットを引数にする。
- 失敗シナリオ: `[:a :b :c | c] valueWithArguments: (OrderedCollection new add: 7; yourself)` は 7 ではなく、OC 内部の `lastIndex` である 1 を返す。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `classOf(arg) == arrayClass` などで Array に限定し、それ以外は失敗にする。

### [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない
- 場所: `runtime/src/Interpreter.cpp:450-452`
- 問題: ブロックを評価するたびにクロージャオブジェクト自身の `kCtxSender` / `kCtxPc` / `kCtxStackp` を上書きし、復元しない。
- 失敗シナリオ: 同じブロックを再帰的に評価すると、内側が返ったあとに外側の `thisContext sender` が内側の呼び出し元を指す。グローバルやインスタンス変数に保存したブロックは、最後に呼ばれたときの送信元コンテキスト連鎖（receiver・method を含む）を保持し続け、GC で回収されない。
- 根拠: コード読解（確度: 中）
- 修正の方向: ブロックの起動ごとに、専用のアクティベーションコンテキストを割り当てる。コスト面で難しければ、戻るときに `sender` を nil に戻す。

## テストの穴
- NLR の回帰テストが Array do: と to:do: にしかない（`block_test.cpp:44-62`）。OrderedCollection / Set / Dictionary / Interval / Dictionary collect: と、ブロック本体に送信を含まない `[:e | ^e]` のケースがない。
- 死んだホームへの `^`（別の Do it やメソッドの外で保存したブロック）の期待動作を決めたテストがない。
- 再帰の深さの上限（無限再帰でクラッシュしないこと）のテストがない。
- ブロックの引数個数の不一致（`value` / `value:` / `valueWithArguments:`）の結果を検証するテストがない。空 OOP がネストした式を通って Message に化けることも検証されていない。
- `process_test.cpp` は状態遷移しか検証しておらず、fork したブロックが実行されること、`wait` がブロックすること、SharedQueue の空読みを検証していない。`wait` 後に `activeProcess` が nil になることは見逃されている。
- DNU を割り当て圧力下で検証するテスト、`valueWithArguments:` に非 Array を渡すテスト、super をブロック内で使うテスト（現状は正しく動く）がない。
- `Interpreter` の `Jump` / `JumpTrue` / `JumpFalse` はコンパイラが発行しないので、バイトコード単体テストがなければ未検証のままになる。非 Boolean で分岐すると黙って片側に落ちる（`Interpreter.cpp:661`）。

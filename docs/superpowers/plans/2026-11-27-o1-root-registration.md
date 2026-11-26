# O1: ルートの登録を全部か無しかにする Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 評価の中で C++ の確保失敗（`std::bad_alloc`）が起きても、解放されるメモリを指す登録や、戻らない `activeContext`、実行可能キューに残る古いポインタを残さないようにする。

**Architecture:**
- まとめて登録する箇所では、登録の前に `Roots::reserveSlots(n)` を呼ぶ。既存の `rootEntry`（Session.cpp）と同じ形である。`reserveSlots` は、何も登録しないうちに投げる（Roots.hpp の契約）。こうすると、コンストラクタが途中で投げてデストラクタが走らず、登録が残る、という穴がふさがる。
- スケジューラは、例外で抜けるときに、実行可能キューと作りかけのプロセスの記録を元に戻す。
- テストは、注入した失敗を k 個目の登録に置き、k を動かす掃引である。

**Tech Stack:** C++20、GoogleTest、ASan、`AO_GC_STRESS`

**Spec:** `SPEC.md`（§3.2、§3.4、§3.10「再入と例外」、§4.1）。

**診断と試作:** 背景は、手元のメモ `docs/superpowers/plans/2026-11-27-o1-diagnosis.md`（未追跡）にある。計画の前に、スクラッチの worktree で修正とテストを試した。その結果は次のとおりで、この計画の値はそこから取った。

| 掃引 | k の範囲（自然に終わるまで） | 通常 | ASan | GC ストレス |
|---|---|---|---|---|
| 評価 | 0〜107 | 通る | エラー 0 件 | 6.8 秒 |
| ライブモードの評価 | 0〜114 | 通る | エラー 0 件 | 7.2 秒 |
| fork | 0〜93 | 通る | エラー 0 件 | 3.1 秒 |
| Accept と記録 | 0〜358 | 通る | エラー 0 件 | 45.9 秒 |

## Global Constraints

- CLAUDE.md の標準ワークフローに従う。Graphify（`graphify explain` / `path`）と Serena（`find_symbol`、`find_referencing_symbols`、シンボル編集）を使う。
- 編集の道具を分ける。
  - コードとテストは Serena で編集する。
  - SPEC.md は Edit で編集する。
  - sed、python、heredoc、`git stash` は使わない。
- コミットの順と形を守る。
  - SPEC の変更が先で、専用のコミットにする。そのあとに、コードとテストのコミットを続ける。
  - 英語の命令形にし、本文に `Graphify:` と `Serena:` の行を入れる。
  - 末尾は `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>` とする。
- SPEC の各文は、コードがすることをそのまま書き、§4.1 に名前を書いたテストで押さえる。
- テストの行は、赤を見たものだけを数える。修正の前に流して赤を見て、報告に書く。
- PHASE は `P18` のまま。CHANGELOG、ネイティブ、依存は変えない。
- ABI の外へ C++ の例外を出さない（§3.10）。

## Review Focus

1. **掃引していない `enqueue` の呼び出し元:** `signal` と `resume` は、例外のときに `enqueue` が `ready_` を戻すことに頼る。掃引は `fork` と `yield` しか通らない。レビューはコードを読んで、2 つの呼び出し元がそのとき何も変えないことを確かめる（Task 2）。
2. **登録したあとに投げる手順:** `addFiber` で、`rootRecord` のあとの `attachStack` が投げる場合。注入の仕組みが無いので、テストでは通らない（Task 2）。
3. **評価をまたいで生きるプロセス:** fork が成功したあとで評価が失敗すると、そのプロセスは次の評価まで生きる（試作の k=63〜73）。これは今までの動作である。テストは、生きているプロセスの記録（スロット 4 つとスタック 1 つ）を数から引き、次の評価のあとで数が完全に戻ることを確かめる（Task 1、2）。
4. **送信のたびの費用:** `reserveSlots` は `Interpreter::run` の起動ごとに呼ばれる。リリースビルドの KernelBench で前後を比べる。中央値で 5% を超えて遅くなったら、まず `Roots.hpp` のインラインの容量の確認を試す。箇所は減らさない（Task 1）。
5. **掃引の終わり:** k が、呼び出しの登録する数を超えると、注入した失敗は起きない。掃引はそこで止まる。新しいテスト用の口 `slotGrowthFailurePending()` でそれを見る（Task 1）。

---

### Task 0: SPEC

**Files:**
- Modify: `SPEC.md` §3.2（「1 回の GC は、どのルートも 1 度だけたどる。」の箇条の直後）
- Modify: `SPEC.md` §4.1（「イメージの保存と読み込み（§3.11。review-agy R7）」の群の直後）

- [ ] **Step 1: §3.2 に次の箇条を足す（文言はこのまま）**

```
- C++ のメモリが足りず、ルートのスロットを登録する表を伸ばせないときは、その登録が `std::bad_alloc` を投げる。評価の中でスロットを登録する箇所（メソッドとブロックの起動、インラインキャッシュ、プロセスの作成）は、まとめて登録するスロットを全部か無しかで入れる。例外で抜けるときは、それまでに登録したスロットをすべて外し、走っているプロセスのコンテキストを入る前のものに戻す。プロセスを実行可能キューに入れる途中で例外が出たときは、キューを入れる前に戻し、fork が作りかけたプロセスは捨てる。そのため、例外を捕まえて `AO_ERR` を返す ABI の入口（§3.10「再入と例外」）や、例外を飲むチェンジセットの記録（§3.9）のあとも、ルートとスケジューラは解放されたメモリを指さず、そのあとの GC と評価は普通に動く。
```

- [ ] **Step 2: §4.1 に次の群を足す**

```
- ルートの登録の失敗（§3.2。O1）:
  - `session_abi_test` に、評価の中の k 番目のルートの登録を失敗させても、ソースの表とクラス ID の表の外のルートの数（スロット、ピン、範囲、ネイティブのフレーム、ハンドル、つないだスタック）が、生きているプロセスの分を除いて元に戻り、`activeContext` が空に戻り、そのあとの GC と評価が通り、次の評価のあとは数が完全に戻る（`SessionAbi.FailedRootAddInsideAnEvalLeavesNoRootBehind`）。ライブモードの評価でも同じ（`SessionAbi.FailedRootAddInsideALiveEvalLeavesNoRootBehind`）。fork と `Processor yield` の中でも同じ（`SessionAbi.FailedRootAddInsideAForkLeavesNoRootBehind`）
  - `accept_abi_test` に、Accept とチェンジセットの記録の送信の中で同じ（`AcceptAbi.FailedRootAddInsideTheRecordLeavesNoRootBehind`）
  - `native_send_test` に、インラインキャッシュの 2 つのスロットの登録が全部か無しか（`NativeSend.InlineCacheRootsAreAllOrNothing`）
```

- [ ] **Step 3: Commit** — `Specify that root registration inside an evaluation is all or nothing`

### Task 1: インタプリタの 4 つのガード

**Files:**
- Modify: `runtime/include/ao/Roots.hpp`（テスト用の口）
- Modify: `runtime/src/Interpreter.cpp`（`FieldRoots`、`ActiveGuard`、`NonlocalGuard` のコンストラクタ）
- Modify: `runtime/src/InterpFrame.hpp`（`Temps` のコンストラクタ）
- Test: `runtime/tests/session_abi_test.cpp`、`runtime/tests/accept_abi_test.cpp`

**Interfaces:**
- Produces: `bool Roots::slotGrowthFailurePending() const`。
  - `failSlotGrowthForTesting` が仕掛けた失敗がまだ起きていなければ true を返す（`failAfter_ != kNoFailure`）。
  - `failSlotGrowthForTesting` の隣に置く。
- Produces（各テストファイルの無名名前空間）: 次の 2 つ。
  - `struct RootShape { std::size_t slots, pinned, ranges, frames, handles, stacks; bool operator==(const RootShape&) const = default; };`
  - `RootShape rootShape(ao::Session& s)`。`counts()` の 6 つを返す。`slots` だけは、`methodSourceRootSlots(false).size()` と `classIdRootSlots().size()` を引く。
- Produces（session_abi_test.cpp の無名名前空間）: `void sweepFailedRootAdds(const char* src, const char* answer)`。下の Step 2 の掃引の本体で、評価、ライブモード、fork の 3 本が使う。

- [ ] **Step 1: 口を足す（`Roots.hpp`）。テストより先に、コンパイルを通すため**

- [ ] **Step 2: `sweepFailedRootAdds` と、それを使う赤のテストを書く**

テストは `SessionAbi.FailedRootAddInsideAnEvalLeavesNoRootBehind` である。式は `| a | a := 0. #(1 2 3) do: [:x | a := a + x]. a`、答えは `"6"` とする。

ライブモードのテスト（`SessionAbi.FailedRootAddInsideALiveEvalLeavesNoRootBehind`）は Task 2 で足す。評価のプロセスを作る `forkEval` が `rootRecord` を通るので、Task 1 の修正だけでは緑にならない（実装のときに、修正の前と同じ k=30 で落ちるのを見た）。

`sweepFailedRootAdds` の中身は次のとおり。

1. boot する。
2. 注入なしで 1 回評価して、答えを確かめる。ワークスペースのルートと doIt の項目は、ここで一度だけ作られる。
3. k = 0 から、注入した失敗が起きなくなるまで、次を繰り返す。各 k に `SCOPED_TRACE(k)` を付ける。
   1. `before = rootShape(s)` とする。
   2. `failSlotGrowthForTesting(k)` で失敗を仕掛け、評価する。
   3. `fired = !slotGrowthFailurePending()` を見てから、`failSlotGrowthForTesting(-1)` で仕掛けを外す。
   4. `after = rootShape(s)` とし、生きているプロセスの分を引く。`live = s.scheduler->liveFibers()` として、`after.slots -= 4 * live`、`after.stacks -= live` とする。
   5. GC より前に、`ASSERT` で確かめる。
      - `ASSERT_TRUE(before == after)`
      - `ASSERT_TRUE(s.ctx->activeContext.isEmpty())`
   6. `Gc::collectNursery` と `collectOld` を走らせる。
   7. 注入なしで評価し、答えを確かめる。そのあと `ASSERT_TRUE(before == rootShape(s))` を確かめる（生きていたプロセスは、この評価で片付く）。
   8. `fired` でなければ抜ける。
4. 最後に `EXPECT_GT(k, 0u)` とする。

- [ ] **Step 3: 赤のテストを書く: `AcceptAbi.FailedRootAddInsideTheRecordLeavesNoRootBehind`**

準備は次のとおり。

1. boot して `Tools-model` を読み込む。
2. `O1Accept` を定義する。
3. `ao_changes_new` でチェンジセットを 1 つ作る。ループの外で 1 回だけ作る。毎回作ると、GC ストレス下で遅い。

k ごとに次を行う。

1. 注入なしで `bar ^1` を Accept する。
2. 失敗を仕掛けて `bar ^2` を Accept する。
3. 確かめる。
   - `rc` は `AO_OK` か `AO_ERR` である。
   - Step 2 の 5.（GC より前の `ASSERT`）と 6.（GC）。生きているプロセスは無いので、引かない。
   - `O1Accept new bar` の評価が、`rc` が `AO_ERR` なら `"1"`、`AO_OK` なら `"2"` を返す。

終わりの条件は Step 2 と同じである。

- [ ] **Step 4: 赤を見る**

Run: `ninja -C build ao_runtime_tests && build/runtime/ao_runtime_tests --gtest_filter='SessionAbi.FailedRootAddInsideAnEval*:AcceptAbi.FailedRootAddInsideTheRecord*'`

Expected: 2 本とも FAIL する。ルートの数が合わない k がある。

- [ ] **Step 5: 4 つのコンストラクタで、登録の前に `reserveSlots` を呼ぶ**

| 場所 | 呼ぶもの |
|---|---|
| `FieldRoots` | `reserveSlots(3)` |
| `ActiveGuard` | `reserveSlots(rootShared ? 2 : 1)`。`saved` の登録より前に呼ぶ |
| `NonlocalGuard` | `rootShared` のときだけ `reserveSlots(2)` |
| `Temps` | 配列を確保した直後に `reserveSlots(n)` |

各所に 1 行のコメントを付ける。趣旨は `rootEntry` と同じで、「全か無か。足りなければ、何も登録しないうちに投げる」とする。

- [ ] **Step 6: 緑を見る**

1. Step 4 と同じコマンドで PASS する。
2. `AO_GC_STRESS=1` で同じ 2 本を流し、時間を測る。どれかが 60 秒を超えたら、`changes_abi_test.cpp:480` と同じく、ストレスのときだけ k の刻みを広げる。

- [ ] **Step 7: KernelBench の前後を比べる**

1. `scripts/build.sh --release` で、修正前（Task 0 の HEAD）と修正後のリリースビルドを作る。修正前のバイナリはコピーしておく。
2. それぞれで `--gtest_filter='KernelBench.*'` を 3 回ずつ流す。
3. テストごとの中央値の ms を表にする。

- [ ] **Step 8: ASan で 2 本を流す**

Run: `scripts/build.sh --asan && build-asan/runtime/ao_runtime_tests --gtest_filter='SessionAbi.FailedRootAdd*:AcceptAbi.FailedRootAdd*'`

Expected: PASS し、ASan のエラーは 0 件。

- [ ] **Step 9: Commit** — `Register the interpreter's root slots all or nothing`

### Task 2: プロセスの作成と実行可能キュー

**Files:**
- Modify: `runtime/src/Scheduler.cpp`（`rootRecord`、`Scheduler::addFiber`、`Scheduler::enqueue`、`Scheduler::fork`）
- Test: `runtime/tests/session_abi_test.cpp`

**Interfaces:**
- Consumes: Task 1 の `slotGrowthFailurePending()`、`rootShape`、`sweepFailedRootAdds`。

- [ ] **Step 1: 赤のテストを 2 本書く**

| テスト | 中身 |
|---|---|
| `SessionAbi.FailedRootAddInsideAForkLeavesNoRootBehind` | `sweepFailedRootAdds("\| p \| p := [3 + 4] fork. Processor yield. 5", "5")` |
| `SessionAbi.FailedRootAddInsideALiveEvalLeavesNoRootBehind` | `ao_set_debug_mode(AO_DEBUG_LIVE)` のあと、Task 1 と同じ式と答えで `sweepFailedRootAdds` |

- [ ] **Step 2: 赤を見る**（Task 1 の修正の上で流す）

Expected: FAIL する。試作では、次の順に壊れた。

1. まず k=53 で、`rootRecord` のスロットが残る。
2. それを直すと、k=56 で Debug の assert `switchTo takes another live process` に当たる。`enqueue` が例外で抜け、`ready_` に記録が残るためである。

- [ ] **Step 3: 直す**

| 場所 | 変更 |
|---|---|
| `rootRecord` | 4 つの登録の前に `roots.reserveSlots(4)` を呼ぶ |
| `addFiber`（`records_`） | `rootRecord` の前に `records_.reserve(records_.size() + 1)` を呼ぶ。こうすれば `records_.push_back` は投げない |
| `addFiber`（`attachStack`） | try と catch(...) で囲む。投げたら `unrootRecord` で 4 つを外して投げ直す |
| `enqueue` | `ready_.push_back(&r)` のあとを try と catch(...) で囲む。投げたら `ready_.pop_back()` して投げ直す。`unwinding` のときの `pop_back` と同じ扱いである |
| `fork` | `enqueue` が投げたら、`enqueue` が false を返したときと同じく、記録を `State::Dead` にして `reapDead()` し、投げ直す |

Scheduler のコンストラクタの `rootRecord` も同じ関数を使うので、全か無かになる。それ以上の変更はしない。

- [ ] **Step 4: 緑を見る**

ストレス下の時間と ASan も、Task 1 と同じに見る。

`attachStack` と `records_.reserve` の失敗には、注入の仕組みが無い。そのため、この 2 つの経路には赤の行が無い。報告にそう書く。

- [ ] **Step 5: Commit** — `Keep a forked process's record and the ready queue consistent on a C++ throw`

### Task 3: インラインキャッシュ（`IcGuard`）

**Files:**
- Modify: `runtime/src/Send.cpp`（`IcGuard` のコンストラクタ）
- Test: `runtime/tests/native_send_test.cpp`

- [ ] **Step 1: 赤のテストを書く: `NativeSend.InlineCacheRootsAreAllOrNothing`**

`NativeSend.IdentityEqualsAndClass` と同じ準備をする。

1. `before = roots.counts().slots` とする。
2. `roots.failSlotGrowthForTesting(1)` で失敗を仕掛ける。
3. `EXPECT_THROW(ao::send(ctx, one, eq, &one, 1, &ic), std::bad_alloc)` を確かめる。
4. 仕掛けを外し、`EXPECT_EQ(before, roots.counts().slots)` を確かめる。
5. そのあと同じ送信が true を返す。

- [ ] **Step 2: 赤を見る**

Expected: FAIL する。ルートの数が 1 多い。

- [ ] **Step 3: 直す**

`ic != nullptr` のとき、2 つの登録の前に `roots.reserveSlots(2)` を呼ぶ。

- [ ] **Step 4: 緑を見る**

- [ ] **Step 5: Commit** — `Register the inline cache's root slots all or nothing`

### Task 4: 仕上げ

- [ ] **Step 1: 全体を検証する**

| コマンド | 通る条件 |
|---|---|
| `ctest --test-dir build --output-on-failure -E '^gcstress$'` | 全件通る |
| `build/ao --test image/tests` | 終了コード 0 |
| `swift test --package-path app` | 失敗 0、AoTests の警告 0 |
| `scripts/build.sh --asan && ctest --test-dir build-asan --output-on-failure -E '^(KernelBench\..*\|gcstress\|gcstress_vendor)$'` | 全件通る |
| 全 GC ストレス（`AO_GC_STRESS=1 ao_runtime_tests --gtest_filter=-KernelBench.*`） | 全件通る |

全 GC ストレスは、コピーしたバイナリで流す。

- [ ] **Step 2: レビューする**

最も強いモデルの新しいレビュアー 1 人に、ブランチ全体（`origin/main..HEAD`）を見てもらう。修正のウェーブは 1 回までとする。

- [ ] **Step 3: グラフを更新する**

`graphify update .` を流してコミットする。

- [ ] **Step 4: PR を作る**

本文に次のことを書く。

- 直したこと
- テスト
- KernelBench の表
- 赤の行が無い経路

CI を待つ。マージはしない。

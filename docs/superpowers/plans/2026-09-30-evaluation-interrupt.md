# P13 評価の中断 — 実装計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ao.app のライブ評価を ⌘. / Interrupt で止め、理由 `interrupted` のライブデバッガに渡す（命令境界停止。Proceed は止めた命令から続ける）。

**Architecture:** セッションの中断要求フラグとホストの RunLoop pump フックを ABI に足す。評価はメインスレッド同期のまま。インタプリタの後方ジャンプ safepoint で (1) 間引き後に pump、(2) 毎回到達でフラグ確認し、ベースが待っている評価プロセスだけを `Scheduler::halt(..., "interrupted", true)` で止める（step / Debug it と同形）。

**Tech Stack:** C++20（runtime）、GoogleTest、Swift 6 AppKit、XCTest、CoreFoundation RunLoop（ホスト pump）。

**Spec / Design:** 実装前に `SPEC.md` を更新する（下記 Task 1）。設計の正本: `docs/superpowers/specs/2026-09-30-evaluation-interrupt-design.md`。

## Global Constraints

- Kernel はネイティブのまま。Smalltalk 側に中断セレクタを足さない。
- ワーカースレッド化・ブリッジキュー・CLI `SIGINT` はしない。
- 検査点は **インタプリタの safepoint（後方ジャンプ）のみ**。ネイティブの `gc.safepoint()` ではフラグを見ない。
- 止めてよいのは `Scheduler::canHalt` が真のとき（ベースが待っている評価プロセス）。それ以外はフラグを消費しない（8 件上限だけは step と同じく理由 `interrupted` で abort）。
- 停止形は step と同じ命令境界。`Object>>halt` の「送信の値 nil」経路に乗せない。
- ライブモード off では止めない（受け入れの不変条件）。
- `ao_request_interrupt` / `ao_set_runloop_pump_hook` は **AbiEntry を取らない**（busy 中も可。`ao_set_debug_capture` と同型）。
- CLAUDE.md: 各タスク前に Serena で定義・参照を取り、シンボル編集を優先。コミット本文に `Graphify:` / `Serena:` トレーラ。最後に `/graphify . --update`。
- 頼まれていないリファクタをしない。

## Review Focus

1. `[true] whileTrue`（後方ジャンプ）がライブで `AO_ERR_HALT` / `interrupted` になる。
2. Proceed 後に再要求なしでは再停止せず、ループが続く。
3. ライブ off ではフラグがあっても完走する。
4. busy 中の `ao_request_interrupt` が `AO_OK`。
5. 最内フレームに合成ネイティブが無い（命令境界）。
6. bench のホットループ比が揺らぎを超えて悪化しない。

---

## File Structure

| ファイル | 責務 |
|---|---|
| `SPEC.md` | P13、§1.4 / §3.9 / §3.10 / §3.13 / §4 / §6 |
| `bridge/ao_abi.h` | `AoRunLoopPumpFn`、`ao_set_runloop_pump_hook`、`ao_request_interrupt`、busy 説明の更新 |
| `runtime/src/abi.cpp` | 上記 ABI、`g_interruptRequested`、`g_runLoopPumpFn` / `user` |
| `runtime/src/Session.hpp` / `Session.cpp` | フラグのクリア（`ao_eval` 入場）、公開が必要なら accessor |
| `runtime/src/Interpreter.cpp` | 後方ジャンプ safepoint で pump（間引き）と `checkInterrupt` |
| `runtime/include/ao/Scheduler.hpp`（宣言確認） / `Scheduler.cpp` | 既存 `halt` / `canHalt` を再利用（変更最小） |
| `runtime/tests/debug_abi_test.cpp` | `LiveDebug` に中断テストを追加 |
| `app/Ao/LaunchSet.swift` | pump 登録と解除 |
| `app/Ao/RunLoopPump.swift`（新規・短く） | `CFRunLoopRunInMode` の C コールバック |
| `app/Ao/MainMenu.swift` / `AoApp.swift` | Interrupt（⌘.）と評価中の有効化 |
| `app/AoTests/DebuggerWindowTests.swift` または `ToolWindowTests.swift` | メニューと中断 UI |
| `PHASE`, `CHANGELOG.md`, `docs/phases/P13.md`, `docs/README.md` | 記録 |
| `docs/bench.md` | 必要なら P13 計測メモ |

## ビルドとテストのコマンド

```sh
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON   # 未作成時
cmake --build build
ctest --test-dir build --output-on-failure -R 'DebugAbi|LiveDebug'
ctest --test-dir build --output-on-failure
./build/ao --test image/tests
swift test --package-path app \
  -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a \
  -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a \
  -Xlinker -lc++ --filter 'DebuggerWindowTests|ToolWindowTests|MainMenu'
```

---

### Task 1: SPEC を P13 に更新する

**Files:**
- Modify: `SPEC.md`（設計書「SPEC への落とし込み」1–8 をすべて）

- [ ] **Step 1: 設計書のチェックリストどおり SPEC を更新する**

必須箇所（省略禁止）:

1. §2.3 に P13「評価の中断」。§2.4 に次マイナー（例: 1.2.0）に含める旨。
2. §1.4 のデバッガ一文にホスト中断（`interrupted`）を足す。
3. §3.2 の「§3.9 の評価の中断」参照を整合。
4. §3.9 Workspace の「外から止められない」を書き換え（評価プロセス＋インタプリタ safepoint。fork/drain・長いネイティブは強制終了のまま）。
5. §3.9 メニューに Interrupt（⌘.）。
6. §3.10 に `ao_request_interrupt` と `ao_set_runloop_pump_hook`。
7. §3.13 停止表に `interrupted` 行。Proceed 本文に `interrupted` を step / Debug it と並べる。検査点はインタプリタ safepoint のみ。「やらない」から外からの中断を外し、ネイティブ safepoint 中断などを残制限として書く。
8. §4 / §6 にテストと受け入れチェックリスト（未チェック）。

- [ ] **Step 2: Commit**

```bash
git add SPEC.md
git commit -m "$(cat <<'EOF'
Specify P13 evaluation interrupt in SPEC.

Host Cmd-. halts the live evaluating process at interpreter safepoints with reason interrupted.

Graphify: (none yet)
Serena: (SPEC only)
EOF
)"
```

---

### Task 2: ABI の宣言とフック／フラグの土台（テスト赤）

**Files:**
- Modify: `bridge/ao_abi.h`
- Modify: `runtime/src/abi.cpp`
- Modify: `runtime/tests/debug_abi_test.cpp`（`LiveDebug` 末尾付近）

- [ ] **Step 1: 失敗するテストを先に書く**

`LiveDebug` に追加（既存 `SetUp` が `AO_DEBUG_LIVE` を立てる前提）:

```cpp
TEST_F(LiveDebug, InterruptRequestIsOkWhileBusyAndHaltsTightLoop) {
  // Flag while a nested eval is busy: set from the transcript hook path is awkward;
  // instead start a loop and request from a pump hook that fires once.
  static int pumps = 0;
  ao_set_runloop_pump_hook(
      [](void*) {
        if (pumps++ == 0) {
          EXPECT_EQ(AO_OK, ao_request_interrupt());
        }
      },
      nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  EXPECT_STREQ("interrupted", err_.message);
  EXPECT_EQ(1, ao_debug_halted_count());
  const std::int64_t pid = ao_debug_halted_pid();
  EXPECT_EQ(1, ao_debug_can_proceed(pid));
  // Bytecode-boundary: innermost is the doIt / whileTrue frame, not a synthesized native.
  EXPECT_NE(std::string::npos, label(0).find("doIt") == std::string::npos
                                   ? label(0).find("[]")
                                   : 0);
  EXPECT_EQ(std::string::npos, label(0).find(" native "));
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, InterruptIgnoredWhenNotLive) {
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ao_set_runloop_pump_hook([](void*) { (void)ao_request_interrupt(); }, nullptr);
  // A bounded loop so the test finishes even if interrupt wrongly halts.
  ASSERT_EQ(AO_OK, doIt("1 to: 10000 do: [:i | i]. 3 + 4"));
  EXPECT_STREQ("7", out_);
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, InterruptFlagClearedOnEvalEntry) {
  ASSERT_EQ(AO_OK, ao_request_interrupt());
  ASSERT_EQ(AO_OK, doIt("3 + 4"));  // live, but flag cleared at entry before any safepoint work
  EXPECT_STREQ("", err_.message);
}
```

（`label(0)` の断言は実装後に実ラベルへ合わせてよい。意図: 合成 `native` フレームが最内に無い。）

- [ ] **Step 2: テストを回して赤を確認**

```sh
cmake --build build && ctest --test-dir build --output-on-failure -R 'InterruptRequest|InterruptIgnored|InterruptFlag'
```

Expected: リンクエラーまたは未定義シンボル / FAIL。

- [ ] **Step 3: `ao_abi.h` に宣言を足す**

`ao_set_inspect_hook` の直後:

```c
typedef void (*AoRunLoopPumpFn)(void* user);

/* NULL removes it. Survives shutdown, boot, and image load. May be set before the first boot
   and while busy. */
void ao_set_runloop_pump_hook(AoRunLoopPumpFn fn, void* user);

/* Sets the session interrupt request. AO_ERR with no session; otherwise AO_OK even while busy.
   Consumed when the live evaluating process stops with reason "interrupted". Cleared on abort,
   normal eval end, and the next ao_eval entry. Live mode off ignores it (does not stop). */
int ao_request_interrupt(void);
```

busy 説明コメントの「呼べる」一覧に `ao_request_interrupt` と `ao_set_runloop_pump_hook` を足す（拒む一覧には入れない）。

- [ ] **Step 4: `abi.cpp` に最小実装（まだ止めない）**

```cpp
namespace {
AoRunLoopPumpFn g_runLoopPumpFn = nullptr;
void* g_runLoopPumpUser = nullptr;
bool g_interruptRequested = false;
}  // namespace

extern "C" void ao_set_runloop_pump_hook(AoRunLoopPumpFn fn, void* user) {
  g_runLoopPumpFn = fn;
  g_runLoopPumpUser = user;
}

extern "C" int ao_request_interrupt(void) {
  if (g_session == nullptr) {
    return AO_ERR;
  }
  g_interruptRequested = true;
  return AO_OK;
}
```

`ao_eval` 入場（既存の結果クリア付近）で `g_interruptRequested = false`。  
Interpreter から見えるよう、`Session.hpp` か内部ヘッダに `ao::interruptRequested()` / `ao::clearInterruptRequest()` / `ao::pumpRunLoopIfDue()` を薄く公開する（abi.cpp の匿名 namespace を Interpreter が触れないため）。推奨: フラグと pump を `Session.cpp` のファイルスコープ＋`Session.hpp` の自由関数にし、abi.cpp はそれを呼ぶ。

- [ ] **Step 5: Commit（まだ中断停止は未実装でテストは赤のまま残してよいが、 ideally Task 3 まで一気に緑にする）。方針: Task 2 では ABI とクリアのみコミットし、停止テストは Task 3 で緑にする。**

Task 2 のコミット対象から停止テストを外し、先に ABI 単体テストだけにする場合:

```cpp
TEST_F(DebugAbi, RequestInterruptNeedsSession) {
  ao_runtime_shutdown();
  EXPECT_EQ(AO_ERR, ao_request_interrupt());
}

TEST_F(LiveDebug, RequestInterruptOkWithSession) {
  EXPECT_EQ(AO_OK, ao_request_interrupt());
}
```

停止系は Task 3 で追加。

```bash
git add bridge/ao_abi.h runtime/src/abi.cpp runtime/src/Session.hpp runtime/src/Session.cpp runtime/tests/debug_abi_test.cpp
git commit -m "$(cat <<'EOF'
Add ao_request_interrupt and the runloop pump hook.

Clear the interrupt flag on each ao_eval entry; stopping at safepoints comes next.

Graphify: path Session abi
Serena: insert ao_request_interrupt ao_set_runloop_pump_hook
EOF
)"
```

---

### Task 3: インタプリタ safepoint で中断停止（テスト緑）

**Files:**
- Modify: `runtime/src/Interpreter.cpp`（`jumpTo`、必要なら `stepCheck` 近くに `checkInterrupt`）
- Modify: `runtime/src/Session.cpp` / `Session.hpp`（pump 間引きカウンタ）
- Modify: `runtime/tests/debug_abi_test.cpp`
- Test: 上記

- [ ] **Step 1: 失敗する停止テストを書く**

```cpp
TEST_F(LiveDebug, InterruptHaltsTightLoopAtBytecodeBoundary) {
  static int pumps = 0;
  pumps = 0;
  ao_set_runloop_pump_hook(
      [](void*) {
        if (pumps++ == 0) {
          EXPECT_EQ(AO_OK, ao_request_interrupt());
        }
      },
      nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  EXPECT_STREQ("interrupted", err_.message);
  EXPECT_EQ(1, ao_debug_can_proceed(ao_debug_halted_pid()));
  EXPECT_EQ(std::string::npos, label(0).find(" native "));
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, InterruptProceedResumesWithoutRearm) {
  static int pumps = 0;
  pumps = 0;
  ao_set_runloop_pump_hook(
      [](void*) {
        if (pumps == 0) {
          ++pumps;
          (void)ao_request_interrupt();
        }
      },
      nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("| n | n := 0. [n < 100000] whileTrue: [n := n + 1]. n"));
  const std::int64_t pid = ao_debug_halted_pid();
  pumps = 100;  // do not re-request
  ASSERT_EQ(AO_OK, ao_debug_proceed(pid, out_, sizeof out_, &err_));
  // finishes the loop
  EXPECT_TRUE(std::string(out_).size() > 0 || err_.message[0] == '\0');
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, InterruptIgnoredWhenNotLive) {
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ao_set_runloop_pump_hook([](void*) { (void)ao_request_interrupt(); }, nullptr);
  ASSERT_EQ(AO_OK, doIt("1 to: 10000 do: [:i | i]. 7"));
  EXPECT_STREQ("7", out_);
  ao_set_runloop_pump_hook(nullptr, nullptr);
}
```

`ao_debug_proceed` の実際のシグネチャに合わせる（既存 `LiveDebug` の Proceed テストをコピー）。

- [ ] **Step 2: 赤を確認**

```sh
ctest --test-dir build --output-on-failure -R 'InterruptHalts|InterruptProceed|InterruptIgnored'
```

- [ ] **Step 3: `checkInterrupt` を実装し、`jumpTo` の後方ジャンプから呼ぶ**

`Interpreter.cpp`（`stepCheck` の近く）:

```cpp
[[gnu::noinline]] bool checkInterrupt(CallContext& ctx) {
  if (!ao::interruptRequested()) {
    return true;
  }
  if (g_debugMode != AO_DEBUG_LIVE || ctx.scheduler == nullptr) {
    return true;  // do not consume
  }
  // Only the evaluating process may stop; others leave the flag for it.
  if (!ctx.scheduler->runningEval()) {
    return true;
  }
  if (ctx.aborting || ctx.abandoning || ctx.abortSetAside > 0 || ctx.haltSuppressed > 0) {
    return true;
  }
  if (!ctx.scheduler->canHalt(ctx)) {
    if (ctx.scheduler->haltedCount() >= /* kMaxHalted via canHalt path */) {
      ao::clearInterruptRequest();
      abortEvaluation(ctx, "interrupted");
      return false;
    }
    return true;
  }
  ao::clearInterruptRequest();
  return ctx.scheduler->halt(ctx, "interrupted", true);
}
```

`haltedCount` が private なら、`canHalt` が偽のとき「評価プロセスなのに止められない」＝ 8 件上限とみなし abort する、と stepCheck に合わせて単純化する:

```cpp
  if (!ctx.scheduler->canHalt(ctx)) {
    ao::clearInterruptRequest();
    ctx.stepMode = StepMode::None;
    abortEvaluation(ctx, "interrupted");
    return false;
  }
```

注意: `runningEval()` だが `canHalt` 偽の他要因（abort 中など）ではフラグを消費して abort しないこと。上の aborting ガードを先に置く。

`jumpTo`:

```cpp
  if (rel < 0) {
    gc.safepoint();
    ao::maybePumpRunLoop();  // throttled
    if (!checkInterrupt(ctx)) {
      return false;  // caller must treat like abort/halt (empty)
    }
  }
```

`jumpTo` が `false` を返したときの呼び出し側が、halt 後の空 OOP と同じく評価を終えることを既存の jump 失敗経路で確認する。足りなければ `checkInterrupt` 失敗時に `ctx` の abort/halt 状態を呼び出し側が既に見ているか追う。

**より安全な形:** `jumpTo` は pc 更新と safepoint / pump まで行い、`checkInterrupt` はインタプリタループで後方ジャンプ命令の処理直後に呼ぶ。既存の `Jump` / `JumpTrue` / `JumpFalse` ケースを Serena で探し、`rel < 0` のあと `if (!checkInterrupt(ctx)) return Oop{};` を足す。

- [ ] **Step 4: pump 間引き**

`Session.cpp`:

```cpp
void maybePumpRunLoop() {
  if (g_runLoopPumpFn == nullptr) {
    return;
  }
  static std::uint32_t count = 0;
  static std::uint64_t lastNs = 0;
  ++count;
  const auto now = /* steady_clock nanos */;
  constexpr std::uint32_t kEvery = 1024;
  constexpr std::uint64_t kNs = 16'000'000;  // 16ms
  if (count < kEvery && (now - lastNs) < kNs) {
    return;
  }
  count = 0;
  lastNs = now;
  g_runLoopPumpFn(g_runLoopPumpUser);
}
```

定数は後で bench で調整。テストでは pump が毎 safepoint でなくても、最初の数回でフラグが立つよう `kEvery` をテスト専用に下げるか、テストは **pump 無しでループ前に `ao_request_interrupt()`** する別ケースも持つ:

```cpp
TEST_F(LiveDebug, InterruptWithoutPumpUsesPendingFlag) {
  // Start eval; we cannot set the flag mid-loop without a pump, so use a cooperative yield:
  // request is set inside a native-free loop by pre-setting? Actually pre-set is cleared on entry.
  // So pump hook is required for tight loops. Keep the pump-based test as primary.
}
```

設計どおり、タイトループには pump が必須。テストは pump フックで 1 回だけ request。

- [ ] **Step 5: 緑を確認**

```sh
cmake --build build
ctest --test-dir build --output-on-failure -R 'Interrupt'
ctest --test-dir build --output-on-failure -R 'LiveDebug'
```

- [ ] **Step 6: Commit**

```bash
git add runtime/src/Interpreter.cpp runtime/src/Session.cpp runtime/src/Session.hpp \
  runtime/tests/debug_abi_test.cpp
git commit -m "$(cat <<'EOF'
Halt live tight loops on interrupt at interpreter safepoints.

Pump the host runloop on a throttle; consume the flag only for the evaluating process.

Graphify: path Interpreter Scheduler Session
Serena: insert_before_symbol checkInterrupt; replace_symbol_body jumpTo
EOF
)"
```

---

### Task 4: 追加の runtime エッジケース

**Files:**
- Modify: `runtime/tests/debug_abi_test.cpp`
- Modify: 実装が足りなければ `Interpreter.cpp` / `abi.cpp`

- [ ] **Step 1: テストを足す**

```cpp
TEST_F(LiveDebug, InterruptRequestOkWhileBusy) {
  static bool seen = false;
  ao_set_runloop_pump_hook(
      [](void*) {
        if (!seen) {
          seen = true;
          EXPECT_EQ(AO_OK, ao_request_interrupt());
        }
      },
      nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, SaveImageRefusedWhileInterruptedHalt) {
  ao_set_runloop_pump_hook([](void*) { (void)ao_request_interrupt(); }, nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  AoSpan err{};
  EXPECT_NE(AO_OK, ao_image_save("/tmp/ao-p13-should-not-save.aoimage", &err));
  EXPECT_TRUE(std::strstr(err.message, "halted") != nullptr);
  (void)ao_debug_abort(ao_debug_halted_pid());
  ao_set_runloop_pump_hook(nullptr, nullptr);
}
```

fork したプロセスがフラグを消費しないことは、短くコメント＋可能なら簡単なケースで。難しければ SPEC の文と canHalt ガードで足りる旨をテストコメントに残す。

- [ ] **Step 2: 緑にして Commit**

```bash
git add runtime/tests/debug_abi_test.cpp
git commit -m "$(cat <<'EOF'
Cover busy interrupt requests and save refusal while interrupted.

Graphify: path debug_abi_test
Serena: (tests only)
EOF
)"
```

---

### Task 5: Ao.app — pump と Interrupt メニュー

**Files:**
- Create: `app/Ao/RunLoopPump.swift`
- Modify: `app/Ao/LaunchSet.swift`
- Modify: `app/Ao/MainMenu.swift`
- Modify: `app/Ao/AoApp.swift`（actions 配線）
- Modify: `app/Package.swift` または Xcode 源リスト（新規ファイルをターゲットに含める）

- [ ] **Step 1: RunLoop pump**

```swift
import CoreFoundation
import CAo

enum RunLoopPump {
  static func install() {
    ao_set_runloop_pump_hook({ _ in
      // Non-blocking: process one ready event if any.
      _ = CFRunLoopRunInMode(CFRunLoopMode.defaultMode, 0, true)
    }, nil)
  }

  static func remove() {
    ao_set_runloop_pump_hook(nil, nil)
  }
}
```

（クロージャを C 関数ポインタに渡せない場合は `@_cdecl` または静的関数 + `UnsafeMutableRawPointer` を使う。既存の transcript hook の Swift 側をコピーする。）

- [ ] **Step 2: `LaunchSet.make` で `RunLoopPump.install()`、`deinit` で `remove()`**

- [ ] **Step 3: メニュー**

`MainMenu.Actions` に `interrupt: () -> Void` と `canInterrupt: () -> Bool`。

Smalltalk メニュー、Debug it の下:

```swift
let interrupt = actionItem("Interrupt", key: ".", run: { _ in actions.interrupt() },
                           enabled: actions.canInterrupt)
// key "." with default .command → ⌘.
```

- [ ] **Step 4: `AoApp` / Workspace から `ao_request_interrupt()`**

`canInterrupt`: ランタイム busy かつライブ（アプリは常時ライブ）のとき true。busy 判定が ABI に無ければ、「評価中フラグ」を Workspace が持つ、またはメニューは常時有効で効くのは評価中のみ（設計許容）。

- [ ] **Step 5: XCTest**

`ToolWindowTests` または `DebuggerWindowTests` に:

- メニューに Interrupt と ⌘. がある
- `ao_request_interrupt` を評価中に呼ぶと理由 `interrupted` で Debugger が開く（既存 halt UI テストを流用）

- [ ] **Step 6: Commit**

```bash
git add app/Ao/RunLoopPump.swift app/Ao/LaunchSet.swift app/Ao/MainMenu.swift app/Ao/AoApp.swift \
  app/AoTests/
git commit -m "$(cat <<'EOF'
Wire Cmd-. Interrupt and a runloop pump in Ao.app.

Graphify: path MainMenu LaunchSet
Serena: (Swift; restart_language_server after)
EOF
)"
```

---

### Task 6: PHASE・CHANGELOG・phase 文書・bench・Graphify

**Files:**
- Modify: `PHASE` → `P13`
- Modify: `CHANGELOG.md`（`[Unreleased]` と Known limitations の書き換え）
- Create: `docs/phases/P13.md`
- Modify: `docs/README.md`
- Modify: `SPEC.md` §6 の P13 チェックを実装完了に合わせて `[x]`（全部緑のあと）
- Modify: `docs/bench.md`（ホットループを前後で測り、悪化が無ければ一行）

- [ ] **Step 1: 全テスト**

```sh
ctest --test-dir build --output-on-failure
./build/ao --test image/tests
swift test --package-path app ...  # 既存の force_load 付き
```

- [ ] **Step 2: 手動確認（docs/phases/P13.md にチェックリスト）**

1. `[true] whileTrue` → ⌘. → `interrupted`
2. メニュー Interrupt
3. Proceed → 再 ⌘.
4. Abort
5. 評価外 ⌘.

- [ ] **Step 3: Graphify**

```
/graphify . --update
```

- [ ] **Step 4: Commit**

```bash
git add PHASE CHANGELOG.md SPEC.md docs/phases/P13.md docs/README.md docs/bench.md graphify-out/
git commit -m "$(cat <<'EOF'
Record P13 completion and refresh the knowledge graph.

Graphify: /graphify . --update
Serena: (docs)
EOF
)"
```

---

## Execution notes

- TDD: 各 runtime タスクは赤 → 最小実装 → 緑 → コミット。
- `jumpTo` が `bool` を返す既存契約と halt の空 OOP を混同しない。Serena で Jump 系の全呼び出しを確認する。
- pump の Swift → C 関数ポインタは transcript hook の実装を必ず真似る（ここが一番のホスト側ハマりどころ）。
- bench が明らかに悪化したら `kEvery` / 16ms を緩め、フラグ確認は毎後方ジャンプのままにする。

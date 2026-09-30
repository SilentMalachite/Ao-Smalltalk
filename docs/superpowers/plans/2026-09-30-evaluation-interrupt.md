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
| `runtime/src/Session.hpp` / `Session.cpp` | フラグのクリア（`ao_eval` 入場）、`debugMode()`、`setInterruptRequestedForTest`、pump 間引き |
| `runtime/src/Interpreter.cpp` | 後方ジャンプで pump。Jump 系直後に `checkInterrupt` |
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

- [ ] **Step 1: 失敗するテストを先に書く（ABI のみ。停止は Task 3）**

```cpp
TEST_F(DebugAbi, RequestInterruptNeedsSession) {
  ao_runtime_shutdown();
  EXPECT_EQ(AO_ERR, ao_request_interrupt());
}

TEST_F(LiveDebug, RequestInterruptOkWithSession) {
  EXPECT_EQ(AO_OK, ao_request_interrupt());
}
```

Task 2 冒頭にあった停止テスト案（lambda pump）は使わない。フックは Task 3 のとおり静的関数にする。

- [ ] **Step 2: テストを回して赤を確認**

```sh
cmake --build build && ctest --test-dir build --output-on-failure -R 'RequestInterrupt'
```

Expected: 未定義シンボルまたは FAIL。

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

- [ ] **Step 5: Commit（ABI とクリアのみ。停止は Task 3）**

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
- Modify: `runtime/src/Interpreter.cpp`（`jumpTo` は safepoint+pump のみ。Jump 系の直後で `checkInterrupt`）
- Modify: `runtime/src/Session.cpp` / `Session.hpp`（フラグ、pump、`debugMode()`、テスト用 `setInterruptRequestedForTest`）
- Modify: `runtime/include/ao/Scheduler.hpp`（`kMaxHalted` は既に公開。`haltedCount()` も公開済み）
- Modify: `runtime/tests/debug_abi_test.cpp`
- Test: 上記

**唯一の手順（混同しない）:** `jumpTo` の `false` は範囲外ジャンプ専用のままにする。後方ジャンプでは `gc.safepoint()` のあと `ao::maybePumpRunLoop()` だけ。`checkInterrupt` は `Jump` / `JumpTrue` / `JumpFalse` を処理した直後、`rel < 0` のときに `if (!checkInterrupt(ctx)) return Oop{};`。

- [ ] **Step 1: 失敗する停止テストを書く**

C++ のフックはキャプチャ無しの関数ポインタ（Swift と同様）。テストでは静的関数を使う:

```cpp
namespace {
int g_interruptPumps = 0;
void interruptOncePump(void*) {
  if (g_interruptPumps++ == 0) {
    EXPECT_EQ(AO_OK, ao_request_interrupt());
  }
}
}  // namespace

TEST_F(LiveDebug, InterruptHaltsTightLoopAtBytecodeBoundary) {
  g_interruptPumps = 0;
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  EXPECT_STREQ("interrupted", err_.message);
  EXPECT_EQ(1, ao_debug_can_proceed(ao_debug_halted_pid()));
  EXPECT_EQ(std::string::npos, label(0).find(" native "));
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, InterruptProceedResumesWithoutRearm) {
  g_interruptPumps = 0;
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("| n | n := 0. [n < 100000] whileTrue: [n := n + 1]. n"));
  const std::int64_t pid = ao_debug_halted_pid();
  g_interruptPumps = 100;  // do not re-request
  ASSERT_EQ(AO_OK, ao_debug_proceed(pid, out_, sizeof out_, &err_));
  EXPECT_STREQ("100000", out_);
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, InterruptIgnoredWhenNotLive) {
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  g_interruptPumps = 0;
  ASSERT_EQ(AO_OK, doIt("1 to: 10000 do: [:i | i]. 7"));
  EXPECT_STREQ("7", out_);
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

// Design: halt does not need the pump — only flag check. Entry clears ABI requests, so tests
// arm the flag via Session test helper after the eval has started (called from the first pump,
// then the hook is cleared so later safepoints have pump == nullptr).
TEST_F(LiveDebug, InterruptWithoutPumpUsesPendingFlag) {
  static bool armed = false;
  ao_set_runloop_pump_hook(
      [](void*) {
        if (!armed) {
          armed = true;
          ao::setInterruptRequestedForTest();  // Session.hpp, tests may call; not a public ABI
          ao_set_runloop_pump_hook(nullptr, nullptr);
        }
      },
      nullptr);
  armed = false;
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  EXPECT_STREQ("interrupted", err_.message);
}
```

- [ ] **Step 2: 赤を確認**

```sh
ctest --test-dir build --output-on-failure -R 'InterruptHalts|InterruptProceed|InterruptIgnored|InterruptWithoutPump'
```

- [ ] **Step 3: `checkInterrupt`（この形だけを正とする）**

`Session.hpp` に `ao::debugMode()`（`g_debugMode` の読み）を公開する。Interpreter はファイルスコープの `g_debugMode` を直接見ない。

```cpp
[[gnu::noinline]] bool checkInterrupt(CallContext& ctx) {
  if (!ao::interruptRequested()) {
    return true;
  }
  if (ao::debugMode() != AO_DEBUG_LIVE || ctx.scheduler == nullptr) {
    return true;  // do not consume
  }
  if (!ctx.scheduler->runningEval()) {
    return true;  // do not consume — forked / drain processes leave the flag
  }
  if (ctx.aborting || ctx.abandoning || ctx.abortSetAside > 0 || ctx.haltSuppressed > 0) {
    return true;  // do not consume
  }
  if (!ctx.scheduler->canHalt(ctx)) {
    // Only the 8-halt cap aborts (same idea as step). Other canHalt failures already filtered.
    if (ctx.scheduler->haltedCount() >= Scheduler::kMaxHalted) {
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

`jumpTo`（後方のみ）:

```cpp
  if (rel < 0) {
    gc.safepoint();
    ao::maybePumpRunLoop();
  }
```

Jump 系ケース（Serena で全箇所）の直後:

```cpp
    if (rel < 0 && !checkInterrupt(ctx)) {
      return Oop{};
    }
```

- [ ] **Step 4: pump 間引き**（`Session.cpp`）

```cpp
void maybePumpRunLoop() {
  if (g_runLoopPumpFn == nullptr) {
    return;
  }
  static std::uint32_t count = 0;
  static auto last = std::chrono::steady_clock::now();
  ++count;
  const auto now = std::chrono::steady_clock::now();
  constexpr std::uint32_t kEvery = 1024;
  if (count < kEvery && now - last < std::chrono::milliseconds(16)) {
    return;
  }
  count = 0;
  last = now;
  g_runLoopPumpFn(g_runLoopPumpUser);
}
```

- [ ] **Step 5: 緑を確認 → Commit**

```bash
git add runtime/src/Interpreter.cpp runtime/src/Session.cpp runtime/src/Session.hpp \
  runtime/tests/debug_abi_test.cpp
git commit -m "$(cat <<'EOF'
Halt live tight loops on interrupt at interpreter safepoints.

Pump the host runloop on a throttle; consume the flag only for the evaluating process.

Graphify: path Interpreter Scheduler Session
Serena: insert_before_symbol checkInterrupt; Jump cases after backward jump
EOF
)"
```

---

### Task 4: 追加の runtime エッジケース

**Files:**
- Modify: `runtime/tests/debug_abi_test.cpp`
- Modify: 実装が足りなければ `Interpreter.cpp` / `Session.cpp`

- [ ] **Step 1: テストを足す**

```cpp
TEST_F(LiveDebug, InterruptRequestOkWhileBusy) {
  g_interruptPumps = 0;
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

TEST_F(LiveDebug, SaveImageRefusedWhileInterruptedHalt) {
  // Signature is ao_image_save(const char* path) only — see LiveDebug.SaveWithHaltedProcessIsRefused.
  const std::string path =
      (std::filesystem::temp_directory_path() / "ao-p13-interrupt-save.aoimage").string();
  std::remove(path.c_str());
  g_interruptPumps = 0;
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  ASSERT_EQ(AO_ERR_HALT, doIt("[true] whileTrue"));
  EXPECT_EQ(AO_ERR, ao_image_save(path.c_str()));
  EXPECT_FALSE(std::filesystem::exists(path));
  (void)ao_debug_abort(ao_debug_halted_pid());
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

// Like NinthHaltAborts: eight halted processes, then interrupt aborts with "interrupted".
TEST_F(LiveDebug, NinthInterruptAborts) {
  for (int k = 0; k < 8; ++k) {
    ASSERT_EQ(AO_ERR_HALT, doIt("self halt")) << k;
  }
  EXPECT_EQ(8, ao_debug_halted_count());
  g_interruptPumps = 0;
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  ASSERT_EQ(AO_ERR_EVAL, doIt("[true] whileTrue"));
  EXPECT_STREQ("interrupted", err_.message);
  EXPECT_EQ(8, ao_debug_halted_count());
  ao_set_runloop_pump_hook(nullptr, nullptr);
}

// Forked process must not consume the flag: parent finishes; flag still set for a later eval
// that clears on entry (so the next eval starts clean). Concrete check: after a fork loop is
// running under drain, requesting interrupt does not stop the forked process as a live halt
// of that pid — only the evaluating process may halt.
TEST_F(LiveDebug, InterruptDoesNotHaltForkedProcess) {
  g_interruptPumps = 0;
  ao_set_runloop_pump_hook(interruptOncePump, nullptr);
  // Eval process forks a tight loop then answers 7; drain may run the fork. Interrupt during
  // the parent's short run must not leave a halted non-eval pid.
  ASSERT_EQ(AO_OK, doIt("[ [true] whileTrue ] fork. 7"));
  EXPECT_STREQ("7", out_);
  EXPECT_EQ(0, ao_debug_halted_count());
  ao_set_runloop_pump_hook(nullptr, nullptr);
}
```

`InterruptDoesNotHaltForkedProcess` がスケジューラ都合でフレークするなら、短い `1 to: 10000` の fork に変え、halted_count == 0 と `runningEval` ガードのコメントを残す。止められないことは `checkInterrupt` の `!runningEval()` early-return が本体。

- [ ] **Step 2: 緑にして Commit**

```bash
git add runtime/tests/debug_abi_test.cpp runtime/src/Interpreter.cpp runtime/src/Session.cpp
git commit -m "$(cat <<'EOF'
Cover interrupt edges: busy request, save refusal, ninth abort, fork.

Graphify: path debug_abi_test Scheduler
Serena: (tests; checkInterrupt guards)
EOF
)"
```

---

### Task 5: Ao.app — pump と Interrupt メニュー

**Files:**
- Create: `app/Ao/RunLoopPump.swift`
- Modify: `app/Ao/LaunchSet.swift`
- Modify: `app/Ao/MainMenu.swift`
- Modify: `app/Ao/AoApp.swift`
- Modify: `app/AoTests/AcceptTests.swift`（Smalltalk メニュータイトル列に `Interrupt` を挿入）
- Modify: `app/AoTests/ToolWindowTests.swift`（⌘. とタイトル）
- Modify: `app/AoTests/DebuggerWindowTests.swift`（中断で Debugger が開く）
- Modify: `app/Package.swift` または源リスト（新規ファイル）

- [ ] **Step 1: RunLoop pump（TranscriptWindow と同型。クロージャを渡さない）**

`TranscriptWindow.swift` の `aoTranscriptHook` + `Unmanaged` を真似る:

```swift
import CoreFoundation
import CAo

enum RunLoopPump {
  static func install() {
    ao_set_runloop_pump_hook(aoRunLoopPumpHook, nil)
  }

  static func remove() {
    ao_set_runloop_pump_hook(nil, nil)
  }
}

private func aoRunLoopPumpHook(_ user: UnsafeMutableRawPointer?) {
  _ = user
  _ = CFRunLoopRunInMode(CFRunLoopMode.defaultMode, 0, true)
}
```

- [ ] **Step 2: `LaunchSet.make` で `RunLoopPump.install()`、`deinit` で `remove()`**

- [ ] **Step 3: メニュー**

`MainMenu.Actions` に `interrupt` / `canInterrupt`。Debug it の次に Interrupt（⌘.）。

設計どおり、評価中は Interrupt **以外**の Smalltalk 評価系（Do it / Print it / Inspect it / Debug it / Accept）をグレーアウトする。`canDoIt` 等を Actions に足すか、既存の validation を評価中フラグでまとめる。

- [ ] **Step 4: `interrupt` → `ao_request_interrupt()`。評価中フラグは Workspace の同期 `ao_eval` 前後で立て下ろす**

- [ ] **Step 5: XCTest**

- `AcceptTests.testRemoveMenuItemsFollowSelection` 付近のタイトル配列を  
  `["Do it", "Print it", "Inspect it", "Debug it", "Interrupt", "Accept", ...]` に更新。
- `ToolWindowTests.testMainMenuListsToolsAndSmalltalkKeys` に Interrupt と `keyEquivalent == "."`。
- `DebuggerWindowTests`: pump または直接 `ao_request_interrupt` で `[true] whileTrue` 相当を止め、理由 `interrupted` で Debugger が開く。

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

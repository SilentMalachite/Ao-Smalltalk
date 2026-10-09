# P15 Debugger の編集 — 実装計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** ライブ Debugger のソース枠でメソッドを直して Accept すると、定義クラスに入れ、そのフレームの活性化を起動した送信の中で、同じ receiver と引数で新しいメソッドを起動し直し、最初の命令の前で理由 `accepted` で止める。

**Architecture:** P14 Restart の巻き戻し（`ctx.restartFrame` / `ctx.restarting` / `applyRestart`）をそのまま使う。違いは対象フレームに着いたあとだけである。Restart はその場で pc を 0 に戻すが、Accept は対象の `Interpreter::run` を抜け（コンテキストは死んだ印になる）、その呼び出し元の `applyMethod` が、ファイバの `CallContext` に固定ルートで預けた「新メソッド・receiver・引数」で `Interpreter::run` をもう一度呼ぶ。探索はし直さない。新しい活性化の最初の命令は `StepMode::Accepted`（Debug it と同じ形）で止める。メソッドの導入は `acceptMethodInto` と同じ 1 つの経路を通し、セレクタの一致だけを足す。

**Tech Stack:** C++20（runtime）、GoogleTest、Swift 6 AppKit、XCTest。

**Spec:** `SPEC.md`（コミット `ac349e6` で P15 を追記済み）。§2.3 P15、§3.9「Debugger の編集（P15）」、§3.10 `ao_debug_can_accept` / `ao_debug_accept`、§3.13「Accept（P15）」、§4 のテスト名、§6「Debugger の編集」。設計の前例は `docs/superpowers/specs/2026-10-03-debugger-restart-design.md`（P14）。

## Global Constraints

- Kernel はネイティブのまま。Smalltalk 側にセレクタを足さない。`KernelScan.*` は緑のまま。
- `ao_debug_accept` は最外の入口（busy なら空の `AO_ERR`）。`ao_debug_can_accept` は busy でも呼べる（`AbiEntry` を取らない）。
- 拒否のメッセージは SPEC の文字列そのまま: `debugger accept refused: home frame is not on the stack`、`debugger accept refused: not an accepted method`、`debugger accept refused: selector changed to <新しいセレクタ>`（いずれも `start` と `end` は 0）。
- 失敗（拒否、コンパイルエラー、セレクタ変更）ではプロセスは止まったままで、メソッド辞書、ソース表、評価結果、スナップショット、中断のフラグに触れない。クリアはメソッドを入れたあと。
- 捨てる側の `ensure:` / `ifCurtailed:` は走らせない（Restart と同じ）。探索し直さない。
- 答えの形は Proceed と同じ（`AO_OK` / `AO_ERR_HALT` / `AO_ERR_EVAL`）。Proceed 不能の停止でも Accept できる。
- 停止理由は `accepted`。Proceed はその命令から続ける（halt 型の「送信の値 nil」ではない）。
- ライブモード off（CLI、`ao --test`）は変えない。`.aoimage` 形式と CLI を変えない（1.4.0 はマイナー版）。
- ホットループに分岐を足さない。足すのは `applyMethod` の戻り直後の 1 分岐（`ctx.reactivating`）だけ。`docs/bench.md` の比を悪化させない。
- CLAUDE.md: 各タスクの前に Serena で `find_symbol` / `find_referencing_symbols` を取り、既存シンボルは `replace_symbol_body` などで編集する。コミットは英語の短い命令形で、本文に `Graphify:` / `Serena:` の一行トレーラと `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`。最後に `/graphify . --update`。
- 頼まれていないリファクタをしない。`docs/superpowers/specs/` に設計書は足さない（理由はこの計画書と SPEC にある）。

## Review Focus

SPEC のテスト一覧が触れないが、使う人がまず踏むもの（上から起きやすい順）。各行のテストは担当タスクに入れてある。

1. 同じフレームを続けて 2 回 Accept する（直した → まだ違う → もう一度直す）。2 回目も通り、`accepted` で止まる。→ Task 2 `AcceptTwiceOnSameFrame`
2. クラス側のメソッド（methodClass がメタクラス）を Accept する。クラス側の辞書に入り、インスタンス側は変わらない。→ Task 2 `AcceptClassSideMethod`
3. Accept して止まったあと Abort すると、外側の `ensure:` は走り、捨てた内側の `ensure:` は走らない。→ Task 2 `AbortAfterAcceptRunsOnlyOuterEnsure`
4. コンパイルエラーを直してもう一度 Accept すると通り、error 行が空になる。→ Task 4 `testAcceptAfterCompileErrorClearsErrorLine`
5. 日本語のコメントの後ろにあるコンパイルエラーの区間が、UTF-16 で正しい位置を選ぶ。→ Task 4 `testAcceptCompileErrorSpanAfterJapaneseComment`

---

## File Structure

| ファイル | 責務 | タスク |
|---|---|---|
| `bridge/ao_abi.h` | `ao_debug_can_accept`、`ao_debug_accept` の宣言と busy の注記 | 1, 2 |
| `runtime/src/abi.cpp` | 2 つの ABI 入口（`guarded`、`debugResumeEntry`） | 1, 2 |
| `runtime/src/Session.hpp` / `Session.cpp` | `acceptTarget`、`debugCanAccept`、`sessionDebugAccept` | 1, 2, 3 |
| `runtime/include/ao/NativeMethod.hpp` | `StepMode::Accepted`、`CallContext::acceptSlots`、`CallContext::reactivating` | 2 |
| `runtime/include/ao/Interpreter.hpp` / `runtime/src/Interpreter.cpp` | `disarmAccept`、`applyRestart` の Accept 分岐、`stepCheck` の `accepted`、`applyMethod` の起動し直し | 2 |
| `runtime/include/ao/Scheduler.hpp` / `runtime/src/Scheduler.cpp` | `proceed` が Accept の預け物を受ける、`halt` の失敗で外す | 2 |
| `runtime/include/ao/Compile.hpp` / `runtime/src/Compile.cpp` | `acceptMethodReplacing`（セレクタ一致を足した `acceptMethodInto`） | 2, 3 |
| `runtime/tests/debug_abi_test.cpp` | `LiveAccept.*` | 1, 2, 3 |
| `app/Ao/DebuggerWindow.swift` | 編集可否、Accept、error 行、破棄の確認 | 4, 5 |
| `app/Ao/BrowserWindow.swift` | `askToDiscard` を `private` から外す（Debugger が使う） | 5 |
| `app/Ao/MainMenu.swift` / `app/Ao/AoApp.swift` | Accept の宛先と `canAccept` | 6 |
| `app/AoTests/DebuggerWindowTests.swift` | P15 の XCTest | 4, 5, 6 |
| `PHASE`、`SPEC.md` §6、`CHANGELOG.md`、`docs/phases/P15.md`、`docs/README.md`、`docs/bench.md`、`graphify-out/` | 完了の記録 | 7 |

## 前提（コードで確かめた事実。`ac349e6`）

- すべての起動は `applyMethod`（`runtime/src/Interpreter.cpp:990`）を通り、CompiledMethod は `Interpreter::run`（同 `:539`）に入る。`Interpreter::run` の呼び出し元は `applyMethod` だけである。
- `Interpreter::run` は `FieldRoots` で `frame->receiver` を、`argHold` で引数をルートする。`ContextExitGuard` が抜けるときにコンテキストの pc と sender を nil にする（死んだ印。ここで作ったブロックの `^` は `cannot return`）。
- P14 の巻き戻し: `Session::resumeHalted` → `Scheduler::proceed(id, restartFrame)` が `ctx.restartFrame` を立て、`Scheduler::halt` が `ctx.restarting = true` で false を返す。内側は `unwinding()` で抜け、対象フレームは `consumeNonlocal` → `Leave::restart` → `applyRestart`。`applyRestart` が false なら呼び出し元は `return Oop{}` する。対象が再起動せずに抜けると `FrameLink` のデストラクタが `restartFrame` を消す。
- `Scheduler::halt`（`Scheduler.cpp:526`）は止める前に `ctx.stepMode = StepMode::None` にする。`stepCheck` の `StepMode::DebugIt` は最初の命令の前で無条件に止める。
- ソース表: `Session::methodSources`（`std::vector<std::unique_ptr<MethodSource>>`、`entry->method`）。doIt は `doItDebug` / `heldDoIts` にあり、`methodSources` には無い。再 Accept は `rememberMethodSource(kept, text, replaced, &image)` で古い項目を消す。vendor、チャンクの file-in、ロードしたイメージのメソッドは項目を持たない。
- `acceptMethodInto(ctx, cls, meta, source, error)`（`Compile.cpp:1433`）はコンパイル → ネイティブ上書き拒否 → 未束縛クラス変数 → `installMethod`（`invalidateMethodCache` を含む）→ `rememberMethodSource` の順。`meta` が真なら `klass(cls)` に入れる。
- メタクラスは `heap.klass(m) == wk.metaclassClass`、そのクラスは `slotAt(m, kClassSlotThisClass)`。
- 固定ルート: `Roots::pinRange(Oop*, n)` / `unpinRange` は順不同で外せる。ファイバの `CallContext::roots` はベースと同じ `Roots`。
- Swift: `BrowserWindow.DiscardConfirmation` と `askToDiscard`（`private static`）、`utf16Range(of:in:)`、`selectErrorSpan(status:span:source:base:in:)` が既にある。Accept メニューは `MainMenu.Actions.accept` / `canEvaluate`。

---

### Task 1: Accept の対象判定と `ao_debug_can_accept`

**Files:**
- Modify: `bridge/ao_abi.h`（`ao_debug_can_restart` の宣言の下、busy で呼べる読みの注記）
- Modify: `runtime/src/abi.cpp`（`ao_debug_can_restart` の下）
- Modify: `runtime/src/Session.hpp`（`debugCanRestart` の下）
- Modify: `runtime/src/Session.cpp`（`resumeHalted` の前の無名名前空間に `acceptTarget`、`debugCanRestart` の下に `debugCanAccept`）
- Test: `runtime/tests/debug_abi_test.cpp`（末尾の `}  // namespace` の前）

**Interfaces:**
- Consumes: `LiveFrames::frameAt`、`Scheduler::haltedContext(pid, &reason)`、`Session::methodSources`、`MethodDictionary::at`。
- Produces:
  - `int ao_debug_can_accept(int64_t pid, int frame_index);`
  - `int ao::debugCanAccept(std::int64_t pid, int frameIndex);`
  - `const Frame* acceptTarget(Session& s, const CallContext& fiber, const LiveFrames& frames, int frameIndex, const char** refusal);`（`Session.cpp` の無名名前空間。Task 2 が使う。成功で `*refusal == nullptr`）

- [ ] **Step 1: Serena で現状を取る**

`find_symbol` で `debugCanRestart`（`runtime/src/Session.cpp`、本体つき）、`ao_debug_can_restart`（`runtime/src/abi.cpp`）、`LiveFrames/frameAt`。`find_referencing_symbols` で `debugCanRestart` の参照（`abi.cpp` だけのはず）。

- [ ] **Step 2: 失敗するテストを書く**

`debug_abi_test.cpp` の末尾（`}  // namespace` の前）に足す。

```cpp
// ---- P15: Debugger の編集 (SPEC §3.13 Accept, §3.10) ----

class LiveAccept : public LiveDebug {
 protected:
  int proceed(std::int64_t pid) {
    err_ = AoSpan{};
    return ao_debug_proceed(pid, out_, sizeof out_, &err_);
  }
  // The index of the first frame labelled `want` in the selected process; -1 when none.
  int frameLabeled(const std::string& want) {
    for (int i = 0; i < ao_debug_frame_count(); ++i) {
      if (label(i) == want) {
        return i;
      }
    }
    return -1;
  }
};

// SPEC §3.13: a doIt frame is no accepted method; neither is the synthesized halt native.
TEST_F(LiveAccept, AcceptDoItFrameIsRefused) {
  ASSERT_EQ(AO_ERR_HALT, doIt("self halt"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("doIt", label(1));
  EXPECT_EQ(0, ao_debug_can_accept(pid, 1));
}

TEST_F(LiveAccept, AcceptNativeFrameIsRefused) {
  defineDbgLive();
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgLive new haltIn: 3"));
  const std::int64_t pid = selectHalted();
  EXPECT_EQ(0, ao_debug_can_accept(pid, 0));
  EXPECT_EQ(1, ao_debug_can_accept(pid, 1));  // DbgLive>>haltIn:, accepted through ao_accept_method
  EXPECT_EQ(0, ao_debug_can_accept(pid, 99));
  EXPECT_EQ(0, ao_debug_can_accept(pid, -1));
}

// SPEC §3.13: a method filed in from a chunk has no source entry (the placeholder frame).
TEST_F(LiveAccept, AcceptPlaceholderFrameIsRefused) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("!Object subclass: #DbgAccChunk\n  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                   "  category: 'P15-Test'!\n"
                                   "!DbgAccChunk methodsFor: 'x'!\n"
                                   "stop\n  self halt.\n  ^1! !\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccChunk new stop"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("DbgAccChunk>>stop", label(1));
  EXPECT_EQ(0, ao_debug_can_accept(pid, 1));
}

// SPEC §3.13: a method re-accepted from the Browser while halted is no longer the dictionary's.
TEST_F(LiveAccept, AcceptReplacedMethodFrameIsRefused) {
  defineClass("DbgAccRep");
  accept("DbgAccRep", 0, "bar\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccRep new bar"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(1, ao_debug_can_accept(pid, 1));
  accept("DbgAccRep", 0, "bar\n  ^2");
  EXPECT_EQ(0, ao_debug_can_accept(pid, 1));
}

// SPEC §3.13: a block whose home has returned has no home activation on the chain.
TEST_F(LiveAccept, AcceptOnBlockWithDeadHomeIsRefused) {
  defineClass("DbgAccDead");
  accept("DbgAccDead", 0, "makeBlock\n  ^[self halt. 3]");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccDead new makeBlock value"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("[] in DbgAccDead>>makeBlock", label(1));
  EXPECT_EQ(0, ao_debug_can_accept(pid, 1));
}

TEST_F(LiveAccept, AcceptPostmortemIsRefused) {
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ASSERT_EQ(AO_ERR_EVAL, doIt("self halt"));
  EXPECT_EQ(0, ao_debug_can_accept(0, 0));
  EXPECT_EQ(0, ao_debug_can_accept(424242, 1));
}
```

- [ ] **Step 3: 失敗を確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'LiveAccept' --output-on-failure`
Expected: ビルドエラー `use of undeclared identifier 'ao_debug_can_accept'`。

- [ ] **Step 4: ABI を宣言する**

`bridge/ao_abi.h` の `int ao_debug_can_restart(int64_t pid, int frame_index);` の下に:

```c
/* SPEC §3.10 P15: 1 when pid is halted and frame_index has an Accept target (§3.13 Accept); 0
   otherwise. Callable while busy. */
int ao_debug_can_accept(int64_t pid, int frame_index);
```

`ao_debug_restart` の宣言の下に（実装は Task 2）:

```c
/* SPEC §3.13 Accept (P15): compile source into the defining class of the target method of
   frame_index (the frame, or a block's home), then start that activation over with the new
   method in the send that started it, halting before its first instruction ("accepted").
   AO_ERR "debugger accept refused: ..." when it has no target; AO_ERR_COMPILE for a compile
   error, a refused overwrite or "debugger accept refused: selector changed to <sel>": nothing
   changes then and the process stays halted. Otherwise the answers of Proceed. */
int ao_debug_accept(int64_t pid, int frame_index, const char* source, char* out, int out_len,
                    AoSpan* err);
```

同じファイル冒頭の busy の注記で、`(ao_debug_halted_pid to ao_debug_select,\n   ao_debug_can_restart)` を `(ao_debug_halted_pid to ao_debug_select,\n   ao_debug_can_restart, ao_debug_can_accept)` に、拒む関数の列挙の `ao_debug_restart` を `ao_debug_restart, ao_debug_accept` に、`each of these twenty-two` を `each of these twenty-three` に直す。

- [ ] **Step 5: 判定を実装する**

`runtime/src/Session.cpp`: 先頭の include に `#include "InterpFrame.hpp"` を足す（`Frame` の中身を読むため。`DebugSnapshot.cpp` と同じ）。`resumeHalted` のある無名名前空間の先頭に:

```cpp
// SPEC §3.13 Accept (P15): the activation an Accept of frame i of the halted process starts over —
// frame i when it is a method, or its block's home method activation, still on the same chain —
// when its method is an accepted one: a CompiledMethod with its own source entry (not the doIt's,
// not a block's) that its methodClass's dictionary still holds under its selector. Null otherwise,
// with the refusal in *refusal. Allocates nothing.
const Frame* acceptTarget(Session& s, const CallContext& fiber, const LiveFrames& frames,
                          int frameIndex, const char** refusal) {
  *refusal = "debugger accept refused: not an accepted method";
  const Frame* f =
      frameIndex >= 0 ? frames.frameAt(static_cast<std::uint32_t>(frameIndex)) : nullptr;
  if (f == nullptr) {
    return nullptr;
  }
  if (f->isBlock) {
    const Oop home = s.heap.slotAt(f->context, kBlockHome);
    const Frame* h = nullptr;
    for (const Frame* g = fiber.topFrame; g != nullptr; g = g->prev) {
      if (!g->isBlock && g->context == home) {
        h = g;
        break;
      }
    }
    if (h == nullptr) {
      *refusal = "debugger accept refused: home frame is not on the stack";
      return nullptr;
    }
    f = h;
  }
  const Oop m = f->method;
  if (!m.isHeap() || s.heap.klass(m) != s.wk.compiledMethodClass) {
    return nullptr;
  }
  const bool hasEntry =
      std::any_of(s.methodSources.begin(), s.methodSources.end(),
                  [m](const std::unique_ptr<Session::MethodSource>& e) { return e->method == m; });
  if (!hasEntry) {
    return nullptr;
  }
  const Oop cls = s.heap.slotAt(m, kCmSlotMethodClass);
  const Oop sel = s.heap.slotAt(m, kCmSlotSelector);
  if (!cls.isHeap() || !sel.isHeap()) {
    return nullptr;
  }
  const Oop dict = s.heap.slotAt(cls, kClassSlotMethodDict);
  if (!dict.isHeap() || MethodDictionary::at(s.heap, dict, sel) != m) {
    return nullptr;
  }
  *refusal = nullptr;
  return f;
}
```

`debugCanRestart` の下に（`Session.hpp` にも `int debugCanAccept(std::int64_t pid, int frameIndex);` を `debugCanRestart` の下に宣言）:

```cpp
int debugCanAccept(std::int64_t pid, int frameIndex) {
  Session* s = session();
  if (s == nullptr || s->scheduler == nullptr || pid <= 0) {
    return 0;
  }
  const std::string* reason = nullptr;
  const CallContext* ctx = s->scheduler->haltedContext(static_cast<std::uint64_t>(pid), &reason);
  if (ctx == nullptr) {
    return 0;
  }
  const LiveFrames frames(*ctx, *reason);
  const char* refusal = nullptr;
  return acceptTarget(*s, *ctx, frames, frameIndex, &refusal) != nullptr ? 1 : 0;
}
```

`runtime/src/abi.cpp` の `ao_debug_can_restart` の下に:

```cpp
extern "C" int ao_debug_can_accept(int64_t pid, int frame_index) {
  return guarded(0, [&] { return ao::debugCanAccept(pid, frame_index); });
}
```

`ao_debug_accept` は宣言だけで、定義は Task 2 で足す（このタスクのテストは呼ばないのでリンクは通る）。

- [ ] **Step 6: 緑を確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'LiveAccept|LiveRestart|KernelScan' --output-on-failure`
Expected: PASS（6 件の `LiveAccept` と既存）。

- [ ] **Step 7: コミット**

```bash
git add bridge/ao_abi.h runtime/src/abi.cpp runtime/src/Session.hpp runtime/src/Session.cpp runtime/tests/debug_abi_test.cpp
git commit -m "Add ao_debug_can_accept for P15" -m "Decide which live frames have an Accept target: an accepted method that its
class's dictionary still holds, or a block's home activation on the chain.

Graphify: path sessionDebugRestart applyMethod
Serena: find_symbol debugCanRestart, LiveFrames/frameAt

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: `ao_debug_accept` — 入れて、起動し直して、`accepted` で止める

**Files:**
- Modify: `runtime/include/ao/NativeMethod.hpp`（`StepMode`、`CallContext`）
- Modify: `runtime/include/ao/Interpreter.hpp`（`disarmAccept` の宣言）
- Modify: `runtime/src/Interpreter.cpp`（`FrameLink::~FrameLink`、`stepCheck`、`applyRestart`、`applyMethod`、新規 `disarmAccept` / `reactivateAccepted`）
- Modify: `runtime/include/ao/Scheduler.hpp` / `runtime/src/Scheduler.cpp`（`proceed`、`halt`）
- Modify: `runtime/include/ao/Compile.hpp` / `runtime/src/Compile.cpp`（`acceptMethodReplacing`。このタスクではセレクタを比べない形で入れ、Task 3 で比べる）
- Modify: `runtime/src/Session.hpp` / `runtime/src/Session.cpp`（`resumeHalted` の引数、`sessionDebugAccept`）
- Modify: `runtime/src/abi.cpp`（`ao_debug_accept`）
- Test: `runtime/tests/debug_abi_test.cpp`

**Interfaces:**
- Consumes: Task 1 の `acceptTarget`。P14 の `resumeHalted`、`Scheduler::proceed(pid, restartFrame)`、`applyRestart`。
- Produces:
  - `enum class StepMode : std::uint8_t { None, Into, Over, Out, DebugIt, Accepted };`
  - `CallContext::acceptSlots`（`std::vector<Oop>`。[0] 新メソッド、[1] receiver、[2..] 引数。預けている間は固定ルート）と `CallContext::reactivating`（`bool`）
  - `void disarmAccept(CallContext& ctx);`、`bool decodeMethodHeader(CallContext& ctx, Oop method, std::uint8_t* numArgs, std::uint8_t* numTemps);`（`Interpreter.hpp`）
  - `EvalEnd Scheduler::proceed(std::uint64_t pid, const Frame* restartFrame = nullptr, const Oop* acceptSlots = nullptr, std::size_t acceptCount = 0);`
  - `bool acceptMethodReplacing(CallContext& ctx, Oop cls, bool meta, std::string_view source, std::string_view selector, Oop* installed, compiler::CompileError* error);`
  - `int ao::sessionDebugAccept(std::int64_t pid, int frameIndex, const char* source, char* out, int outLen, AoSpan* err, AoInspectFn inspect, void* inspectUser);`
  - `int ao_debug_accept(int64_t pid, int frame_index, const char* source, char* out, int out_len, AoSpan* err);`

- [ ] **Step 1: Serena で定義と参照を取る**

`find_symbol` 本体つき: `applyRestart`、`FrameLink`、`stepCheck`、`applyMethod`（`runtime/src/Interpreter.cpp`）、`Scheduler/proceed`、`Scheduler/halt`（`runtime/src/Scheduler.cpp`）、`resumeHalted`、`sessionDebugRestart`（`runtime/src/Session.cpp`）、`acceptMethodInto`（`runtime/src/Compile.cpp`）。`find_referencing_symbols`: `StepMode`（switch の網羅を確かめる）、`Scheduler/proceed`、`resumeHalted`、`CallContext/restartFrame`。

- [ ] **Step 2: 失敗するテストを書く**

`LiveAccept` の `protected:` の先頭に `acceptIn` を足す:

```cpp
  int acceptIn(std::int64_t pid, int frame, const char* src) {
    err_ = AoSpan{};
    return ao_debug_accept(pid, frame, src, out_, sizeof out_, &err_);
  }
```

続けて末尾に:

```cpp
// SPEC §6 P15 受け入れ: the sender of a DNU accepted from the Debugger halts at the first instruction
// of the new method (accepted); Proceed answers its value; the Browser and later sends see it.
TEST_F(LiveAccept, AcceptStopsAtFirstInstructionOfNewMethod) {
  defineClass("DbgAcc");
  accept("DbgAcc", 0, "bar\n  ^self zork");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAcc new bar"));
  EXPECT_STREQ("doesNotUnderstand: #zork", err_.message);
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("DbgAcc>>bar", label(1));
  EXPECT_EQ(1, ao_debug_can_accept(pid, 1));
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "bar\n  ^42")) << err_.message;
  EXPECT_STREQ("accepted", err_.message);
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("accepted", reason());
  ASSERT_EQ(2, ao_debug_frame_count());
  EXPECT_EQ("DbgAcc>>bar", label(0));
  EXPECT_EQ(0, ao_debug_frame_kind(0));
  EXPECT_EQ("bar\n  ^42", source(0).text);
  EXPECT_EQ("doIt", label(1));
  EXPECT_EQ(1, ao_debug_can_proceed(pid));
}

TEST_F(LiveAccept, ProceedAfterAcceptAnswersNewMethodValue) {
  defineClass("DbgAccP");
  accept("DbgAccP", 0, "bar\n  ^self zork");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccP new bar"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "bar\n  ^42")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("42", out_);
  EXPECT_EQ(0, ao_debug_halted_count());
  const std::int64_t id = ao_browser_class_id("DbgAccP");
  char buf[256] = "unset";
  ASSERT_EQ(AO_OK, ao_browser_source(id, 0, "bar", buf, sizeof buf));
  EXPECT_STREQ("bar\n  ^42", buf);
  ASSERT_EQ(AO_OK, printIt("DbgAccP new bar"));
  EXPECT_STREQ("42", out_);
}

TEST_F(LiveAccept, AcceptKeepsReceiverAndArguments) {
  defineClass("DbgAccArgs", "k");
  accept("DbgAccArgs", 0, "setK: v\n  k := v");
  accept("DbgAccArgs", 0, "with: x and: y\n  self halt.\n  ^0");
  ASSERT_EQ(AO_ERR_HALT, printIt("(DbgAccArgs new setK: 100) with: 20 and: 3"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("DbgAccArgs>>with:and:", label(1));
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "with: x and: y\n  ^k + x + y")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("123", out_);
}

TEST_F(LiveAccept, AcceptWithMoreTempsAndDeeperStack) {
  defineClass("DbgAccTemps");
  accept("DbgAccTemps", 0, "calc\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccTemps new calc"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(AO_ERR_HALT,
            acceptIn(pid, 1,
                     "calc\n  | a b c d |\n  a := 1. b := 2. c := 3. d := 4.\n"
                     "  ^a + (b * (c + (d * (a + (b * c)))))"))
      << err_.message;
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  ASSERT_EQ(4, ao_debug_frame_temp_count(0));
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("63", out_);  // 1 + 2 * (3 + 4 * 7)
}

TEST_F(LiveAccept, AcceptOnBlockFrameRestartsHome) {
  defineClass("DbgAccBlk");
  accept("DbgAccBlk", 0, "run\n  ^[self halt. 1] value");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccBlk new run"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("[] in DbgAccBlk>>run", label(1));
  EXPECT_EQ(1, ao_debug_can_accept(pid, 1));
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "run\n  ^5")) << err_.message;
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("DbgAccBlk>>run", label(0));
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("5", out_);
}

TEST_F(LiveAccept, AcceptOuterFrameDropsInnerWithoutEnsure) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  defineClass("DbgAccNest");
  accept("DbgAccNest", 0, "outer\n  ^self inner");
  accept("DbgAccNest", 0, "inner\n  ^[self halt. 7] ensure: [Transcript show: 'done']");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccNest new outer"));
  const std::int64_t pid = selectHalted();
  const int outer = frameLabeled("DbgAccNest>>outer");
  ASSERT_GE(outer, 0);
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, outer, "outer\n  ^8")) << err_.message;
  EXPECT_TRUE(seen.empty());
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("8", out_);
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_TRUE(seen.empty());
}

// SPEC §3.13: an activation a native started (Collection>>includes: sends = to the needle) starts
// over inside that native's send.
TEST_F(LiveAccept, AcceptFrameActivatedFromNativeSend) {
  defineClass("DbgAccEq");
  accept("DbgAccEq", 0, "= other\n  self halt.\n  ^false");
  ASSERT_EQ(AO_ERR_HALT, printIt("#(1) includes: DbgAccEq new"));
  const std::int64_t pid = selectHalted();
  const int eq = frameLabeled("DbgAccEq>>=");
  ASSERT_GE(eq, 0);
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, eq, "= other\n  ^true")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("true", out_);
}

TEST_F(LiveAccept, AcceptFrameActivatedByPerform) {
  defineClass("DbgAccPerf");
  accept("DbgAccPerf", 0, "bar\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccPerf new perform: #bar"));
  const std::int64_t pid = selectHalted();
  const int bar = frameLabeled("DbgAccPerf>>bar");
  ASSERT_GE(bar, 0);
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, bar, "bar\n  ^2")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("2", out_);
}

// SPEC §3.3: the inline caches of a send that ran before see the accepted method.
TEST_F(LiveAccept, AcceptInDebuggerUpdatesCachedSends) {
  defineClass("DbgAccCache");
  accept("DbgAccCache", 0, "bar\n  self halt.\n  ^1");
  accept("DbgAccCache", 0, "callBar\n  ^self bar");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccCache new callBar"));
  ASSERT_EQ(AO_OK, proceed(ao_debug_halted_pid())) << err_.message;
  EXPECT_STREQ("1", out_);
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccCache new callBar"));
  const std::int64_t pid = selectHalted();
  const int bar = frameLabeled("DbgAccCache>>bar");
  ASSERT_GE(bar, 0);
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, bar, "bar\n  ^42")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("42", out_);
  ASSERT_EQ(AO_OK, printIt("DbgAccCache new callBar"));
  EXPECT_STREQ("42", out_);
}

// SPEC §3.13: outer activations of the old method (recursion) go on running it.
TEST_F(LiveAccept, AcceptKeepsOuterRecursionOnOldMethod) {
  defineClass("DbgAccRec");
  accept("DbgAccRec", 0, "count: n\n  n = 0 ifTrue: [self halt. ^0].\n  ^(self count: n - 1) + 1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccRec new count: 2"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("DbgAccRec>>count:", label(1));
  ASSERT_EQ(AO_ERR_HALT,
            acceptIn(pid, 1, "count: n\n  n = 0 ifTrue: [^100].\n  ^(self count: n - 1) + 10"))
      << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("102", out_);  // the two outer activations add 1, not 10
}

// SPEC §3.13: the discarded activation's context is dead, so a block made there cannot return.
TEST_F(LiveAccept, DiscardedActivationBlockCannotReturn) {
  defineClass("DbgAccRet", "saved");
  accept("DbgAccRet", 0, "saved\n  ^saved");
  accept("DbgAccRet", 0, "stash\n  saved := [^1].\n  self halt.\n  ^2");
  ASSERT_EQ(AO_OK, doIt("dbgAccRet := DbgAccRet new"));
  ASSERT_EQ(AO_ERR_HALT, printIt("dbgAccRet stash"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "stash\n  ^3")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("3", out_);
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ASSERT_EQ(AO_ERR_EVAL, doIt("dbgAccRet saved value"));
  EXPECT_NE(std::string::npos, std::string(err_.message).find("cannot return")) << err_.message;
}

TEST_F(LiveAccept, AcceptOnNonProceedable) {
  defineClass("DbgAccBad");
  accept("DbgAccBad", 0, "bad\n  ^3 ifTrue: [4]");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccBad new bad"));
  const std::int64_t pid = selectHalted();
  EXPECT_EQ(0, ao_debug_can_proceed(pid));
  const int bad = frameLabeled("DbgAccBad>>bad");
  ASSERT_GE(bad, 0);
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, bad, "bad\n  ^4")) << err_.message;
  EXPECT_STREQ("accepted", err_.message);
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("4", out_);
}

// Review Focus 1: the accepted activation can be accepted again.
TEST_F(LiveAccept, AcceptTwiceOnSameFrame) {
  defineClass("DbgAcc2");
  accept("DbgAcc2", 0, "bar\n  ^self zork");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAcc2 new bar"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "bar\n  ^1")) << err_.message;
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ(1, ao_debug_can_accept(pid, 0));
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 0, "bar\n  ^2")) << err_.message;
  EXPECT_STREQ("accepted", err_.message);
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("2", out_);
}

// Review Focus 2: a class-side method goes back into the metaclass's dictionary.
TEST_F(LiveAccept, AcceptClassSideMethod) {
  defineClass("DbgAccMeta");
  accept("DbgAccMeta", 1, "make\n  self halt.\n  ^1");
  accept("DbgAccMeta", 0, "make\n  ^-1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccMeta make"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ("DbgAccMeta class>>make", label(1));
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "make\n  ^7")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("7", out_);
  ASSERT_EQ(AO_OK, printIt("DbgAccMeta new make"));
  EXPECT_STREQ("-1", out_);
}

// Review Focus 3: Abort after Accept runs the outer ensure:, never the discarded inner one.
TEST_F(LiveAccept, AbortAfterAcceptRunsOnlyOuterEnsure) {
  std::vector<std::string> seen;
  ao_set_transcript_hook(collectChunks, &seen);
  defineClass("DbgAccAbort");
  accept("DbgAccAbort", 0, "outer\n  ^[self inner] ensure: [Transcript show: 'outer']");
  accept("DbgAccAbort", 0, "inner\n  ^self deep");
  accept("DbgAccAbort", 0, "deep\n  ^[self halt. 1] ensure: [Transcript show: 'deep']");
  ASSERT_EQ(AO_ERR_HALT, doIt("DbgAccAbort new outer"));
  const std::int64_t pid = selectHalted();
  const int inner = frameLabeled("DbgAccAbort>>inner");
  ASSERT_GE(inner, 0);
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, inner, "inner\n  ^2")) << err_.message;
  EXPECT_TRUE(seen.empty());
  EXPECT_EQ(AO_OK, ao_debug_abort(pid));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(std::vector<std::string>{"outer"}, seen);
}
```

ファイル先頭の include に変更は要らない（`ao_browser_*` は `ao_abi.h`）。

- [ ] **Step 3: 失敗を確かめる**

Run: `cmake --build build`
Expected: リンクエラー `Undefined symbols: _ao_debug_accept`。

- [ ] **Step 4: `CallContext` と `StepMode` を足す**

`runtime/include/ao/NativeMethod.hpp`:

```cpp
enum class StepMode : std::uint8_t { None, Into, Over, Out, DebugIt, Accepted };
```

`CallContext` の `bool restarting = false;` の下に（`#include <vector>` が無ければ足す）:

```cpp
  // SPEC §3.13 Accept (P15): armed with restartFrame. [0] the accepted method, [1] the receiver,
  // [2..] the arguments of the target activation; one pinned range of roots while armed (empty:
  // a plain Restart). reactivating: the target has left, and the applyMethod it returns to starts
  // the accepted method in its place.
  std::vector<Oop> acceptSlots;
  bool reactivating = false;
```

- [ ] **Step 5: インタプリタに Accept の分岐を足す**

`runtime/include/ao/Interpreter.hpp`（`applyMethod` の宣言の近く）:

```cpp
// SPEC §3.13 Accept (P15): unpins and empties ctx.acceptSlots. Nothing when they are empty.
void disarmAccept(CallContext& ctx);
```

`runtime/src/Interpreter.cpp`:

1. `FrameLink::~FrameLink` の Restart の後始末に `disarmAccept(ctx);` を足す:

```cpp
  ~FrameLink() {
    ctx.topFrame = frame.prev;
    // A Restart target that leaves without restarting: no later frame may match its address.
    if (ctx.restartFrame == &frame) [[unlikely]] {
      ctx.restartFrame = nullptr;
      disarmAccept(ctx);
    }
  }
```

2. `stepCheck` の switch に（`DebugIt` の下）:

```cpp
    case StepMode::Accepted:
      reached = true;
      reason = "accepted";
      break;
```

3. `applyRestart` の先頭の判定の直後に Accept の分岐を入れる（以降の Restart の本体は変えない）:

```cpp
  if (!ctx.restarting || ctx.restartFrame != &frame) {
    return false;
  }
  // SPEC §3.13 Accept: the target leaves (its context dies on the way out), and the applyMethod it
  // returns to starts the accepted method in its place.
  if (!ctx.acceptSlots.empty()) {
    ctx.restarting = false;
    ctx.restartFrame = nullptr;
    ctx.proceededAt = nullptr;
    ctx.stepMode = StepMode::None;
    ctx.reactivating = true;
    return false;
  }
```

4. 無名名前空間を閉じたあと（`Interpreter::run` の前ではなく、`applyMethod` の前）に:

```cpp
void disarmAccept(CallContext& ctx) {
  if (ctx.acceptSlots.empty()) {
    return;
  }
  ctx.roots.unpinRange(ctx.acceptSlots.data(), ctx.acceptSlots.size());
  ctx.acceptSlots.clear();
}

namespace {

// SPEC §3.13 Accept (P15): the target activation has left. The send that started it now starts the
// accepted method with the same receiver and arguments, without a lookup, and halts before its
// first instruction (accepted). An Accept of the new activation comes back here.
[[gnu::noinline]] Oop reactivateAccepted(CallContext& ctx) {
  for (;;) {
    ctx.reactivating = false;
    const auto argc = static_cast<std::uint32_t>(ctx.acceptSlots.size() - 2);
    Root method(ctx.roots, ctx.acceptSlots[0]);
    Root receiver(ctx.roots, ctx.acceptSlots[1]);
    RootedArray args(ctx.roots, argc);
    for (std::uint32_t i = 0; i < argc; ++i) {
      args.ptr()[i] = ctx.acceptSlots[2 + i];
    }
    disarmAccept(ctx);
    ctx.stepMode = StepMode::Accepted;
    const Oop result =
        Interpreter::run(ctx, method.slot, receiver.slot, args.ptr(), argc, Oop::nil());
    // The new activation did not reach its first instruction: the mark goes, not to a caller.
    if (ctx.stepMode == StepMode::Accepted) {
      ctx.stepMode = StepMode::None;
    }
    if (!ctx.reactivating) {
      return result;
    }
  }
}

}  // namespace
```

5. `applyMethod` の CompiledMethod の枝:

```cpp
  if (k == ctx.wk.compiledMethodClass) {
    const Oop result = Interpreter::run(ctx, method, receiver, args, argc, block);
    if (ctx.reactivating) [[unlikely]] {
      return reactivateAccepted(ctx);
    }
    return result;
  }
```

`Root` / `RootedArray` は `Interpreter.cpp` で既に使っている（`blockHold`、`argHold`）。

- [ ] **Step 6: Scheduler に預け物を渡す**

`runtime/include/ao/Scheduler.hpp` の `proceed` を:

```cpp
  // ... Restart proceeds with restartFrame (an interpreted frame of pid): that process need not be
  // canProceed, and its halt reruns the frame. Accept (P15) also hands acceptCount slots (the
  // accepted method, the receiver, the arguments), copied into pid's acceptSlots and pinned.
  EvalEnd proceed(std::uint64_t pid, const Frame* restartFrame = nullptr,
                  const Oop* acceptSlots = nullptr, std::size_t acceptCount = 0);
```

`runtime/src/Scheduler.cpp` の `Scheduler::proceed`:

```cpp
Scheduler::EvalEnd Scheduler::proceed(std::uint64_t pid, const Frame* restartFrame,
                                      const Oop* acceptSlots, std::size_t acceptCount) {
  assert(current_ == &base() && "proceed runs on the base process");
  Record* r = findId(pid);
  assert(r != nullptr && r->state == State::Halted &&
         (r->proceedable || restartFrame != nullptr) && "proceed needs canProceed or a restart");
  assert((acceptCount == 0 || restartFrame != nullptr) && "an Accept is a restart");
  if (acceptCount > 0) {
    // assign may throw before anything below changes; no GC runs between it and the pin.
    r->ctx->acceptSlots.assign(acceptSlots, acceptSlots + acceptCount);
    r->ctx->roots.pinRange(r->ctx->acceptSlots.data(), acceptCount);
  }
  // Set only now, so a refused or failed entry before this never leaves a restart armed.
  r->ctx->restartFrame = restartFrame;
  // In no list: awaitEval switches to it, and its halt returns (true, or false to restart).
  r->state = State::Suspended;
  return awaitEval(pid);
}
```

`Scheduler::halt` の `afterResume` 失敗の枝に `disarmAccept(ctx);` を足す（`#include "ao/Interpreter.hpp"` が無ければ足す）:

```cpp
  if (!afterResume(ctx, me)) {
    ctx.restartFrame = nullptr;
    ctx.restarting = false;
    disarmAccept(ctx);
    return false;
  }
```

- [ ] **Step 7: 導入の関数を足す（セレクタはまだ比べない）**

`runtime/include/ao/Compile.hpp` の `acceptMethodInto` の下に:

```cpp
// SPEC §3.13 Accept (P15): acceptMethodInto with the same rules and messages, for a source whose
// selector must be `selector`. On success *installed (a root slot of the caller) holds the new
// method.
bool acceptMethodReplacing(CallContext& ctx, Oop cls, bool meta, std::string_view source,
                           std::string_view selector, Oop* installed,
                           compiler::CompileError* error);
```

`runtime/src/Compile.cpp`: 今の `acceptMethodInto` の本体を無名名前空間の `acceptInto` に移し、2 つの公開関数をその薄い包みにする。`acceptInto` は `acceptMethodInto` の本体そのままに、引数 `const std::string_view* selector` と `Oop* installed` を足し、最後の `rememberMethodSource(...)` のあとに `if (installed != nullptr) { *installed = kept.slot; }` を入れる。`selector` はこのタスクでは読まない（Task 3）。

```cpp
bool acceptMethodInto(CallContext& ctx, Oop target, bool meta, std::string_view source,
                      compiler::CompileError* error) {
  return acceptInto(ctx, target, meta, source, nullptr, nullptr, error);
}

bool acceptMethodReplacing(CallContext& ctx, Oop cls, bool meta, std::string_view source,
                           std::string_view selector, Oop* installed,
                           compiler::CompileError* error) {
  return acceptInto(ctx, cls, meta, source, &selector, installed, error);
}
```

（`acceptInto` は `acceptMethodInto` の定義より前、ファイル内の `ao` 名前空間の無名名前空間に置く。`assignError` などの依存が上にあることを確かめる。）

- [ ] **Step 8: Session と ABI の入口**

`runtime/src/Session.cpp` の `resumeHalted` に預け物を通す:

```cpp
int resumeHalted(Session& s, std::uint64_t id, StepMode step, const Frame* restartFrame,
                 const Oop* acceptSlots, std::size_t acceptCount, char* out, int outLen,
                 AoSpan* err, AoInspectFn inspect, void* inspectUser) {
  ...
  const Scheduler::EvalEnd end =
      step == StepMode::None ? s.scheduler->proceed(id, restartFrame, acceptSlots, acceptCount)
                             : s.scheduler->step(id, step);
  ...
}
```

`sessionDebugResume` と `sessionDebugRestart` の呼び出しには `nullptr, 0` を足す。`sessionDebugRestart` の下に:

```cpp
int sessionDebugAccept(std::int64_t pid, int frameIndex, const char* source, char* out,
                       int outLen, AoSpan* err, AoInspectFn inspect, void* inspectUser) {
  spanMessage(err, std::string());
  Session* s = g_session.get();
  if (s == nullptr || s->ctx == nullptr || s->scheduler == nullptr || source == nullptr ||
      out == nullptr || outLen < 1 || pid <= 0) {
    blankOut(out, outLen);
    return AO_ERR;
  }
  const auto id = static_cast<std::uint64_t>(pid);
  const std::string* reason = nullptr;
  const CallContext* fiber = s->scheduler->haltedContext(id, &reason);
  if (fiber == nullptr) {
    blankOut(out, outLen);
    return AO_ERR;
  }
  const LiveFrames frames(*fiber, *reason);
  const char* refusal = nullptr;
  const Frame* target = acceptTarget(*s, *fiber, frames, frameIndex, &refusal);
  if (target == nullptr) {
    spanMessage(err, refusal);
    blankOut(out, outLen);
    return AO_ERR;
  }
  // [0] the accepted method, [1] the receiver, [2..] the arguments: pinned before the compile,
  // which may collect, and handed to the process by proceed.
  CallContext& ctx = *s->ctx;
  const Oop old = target->method;
  std::uint8_t numArgs = 0;
  std::uint8_t numTemps = 0;
  if (!decodeMethodHeader(ctx, old, &numArgs, &numTemps)) {
    spanMessage(err, "debugger accept refused: not an accepted method");
    blankOut(out, outLen);
    return AO_ERR;
  }
  std::vector<Oop> slots(2u + numArgs, Oop::nil());
  slots[1] = target->receiver;
  for (std::uint32_t i = 0; i < numArgs; ++i) {
    slots[2 + i] = target->temps->slots[i];
  }
  struct Pin {
    Roots& roots;
    std::vector<Oop>& v;
    Pin(Roots& r, std::vector<Oop>& slotsRef) : roots(r), v(slotsRef) {
      roots.pinRange(v.data(), v.size());
    }
    ~Pin() { roots.unpinRange(v.data(), v.size()); }
  } pin(ctx.roots, slots);
  Oop cls = s->heap.slotAt(old, kCmSlotMethodClass);
  bool meta = false;
  if (s->heap.klass(cls) == s->wk.metaclassClass) {
    cls = s->heap.slotAt(cls, kClassSlotThisClass);
    meta = true;
  }
  const std::string selector = byteText(s->heap, s->heap.slotAt(old, kCmSlotSelector));
  compiler::CompileError error;
  if (!acceptMethodReplacing(ctx, cls, meta, source, selector, &slots[0], &error)) {
    // The span counts from the start of source (no doIt prefix), as ao_accept_method_id's.
    spanMessage(err, error.message);
    if (err != nullptr) {
      err->start = error.span.start;
      err->end = error.span.end;
    }
    blankOut(out, outLen);
    return AO_ERR_COMPILE;
  }
  return resumeHalted(*s, id, StepMode::None, target, slots.data(), slots.size(), out, outLen, err,
                      inspect, inspectUser);
}
```

引数の数はメソッドのヘッダから読む（`frames.tempCount` は temp 全体の数なので使えない）。`Interpreter.cpp:198` の `decodeHeader` は無名名前空間にあるので、`runtime/include/ao/Interpreter.hpp` に

```cpp
// A CompiledMethod header's argument and temp counts (SPEC §3.5). False when it is malformed.
bool decodeMethodHeader(CallContext& ctx, Oop method, std::uint8_t* numArgs,
                        std::uint8_t* numTemps);
```

を宣言し、`Interpreter.cpp` の `disarmAccept` の隣に `bool decodeMethodHeader(...) { return decodeHeader(ctx, method, numArgs, numTemps); }` を定義する。`byteText` は `Session.cpp:612`、`spanMessage` / `blankOut` は `Session.cpp:1030` / `:1043`（いずれも同じファイルの中で見える）。

`Session.hpp` の `sessionDebugRestart` の下に宣言:

```cpp
int sessionDebugAccept(std::int64_t pid, int frameIndex, const char* source, char* out,
                       int outLen, AoSpan* err, AoInspectFn inspect, void* inspectUser);
```

`runtime/src/abi.cpp` の `ao_debug_restart` の下に:

```cpp
extern "C" int ao_debug_accept(int64_t pid, int frame_index, const char* source, char* out,
                               int out_len, AoSpan* err) {
  return debugResumeEntry(out, out_len, err, [&] {
    return ao::sessionDebugAccept(pid, frame_index, source, out, out_len, err, g_inspectFn,
                                  g_inspectUser);
  });
}
```

- [ ] **Step 9: 緑を確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'LiveAccept|LiveRestart|LiveStep|LiveProceed|KernelScan|RemoveAbi|Accept' --output-on-failure`
Expected: PASS。落ちたら `superpowers:systematic-debugging`。特に `AcceptFrameActivatedFromNativeSend` のフレームのラベルが違うときは、`frameLabeled` の対象を実際のラベルに合わせる前に、`includes:` が本当にネイティブから `=` を送っているか（`Collection.cpp:385`）を確かめる。

- [ ] **Step 10: ASan で回す**

Run: `./scripts/test.sh --asan`
Expected: PASS。固定ルートの外し忘れ（`Roots` の pinned 数）とダングリング `Frame*` を見る。

- [ ] **Step 11: コミット**

```bash
git add runtime bridge
git commit -m "Add ao_debug_accept: install and start the frame over (P15)" -m "Accept reuses the Restart unwind. At the target, the run leaves instead of
resetting its pc, and applyMethod starts the accepted method with the
receiver and arguments pinned in the process's CallContext, halting before
its first instruction with reason accepted.

Graphify: path sessionDebugRestart applyMethod
Serena: replace_symbol_body ao::applyRestart, ao::applyMethod, Scheduler/proceed

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: 失敗の経路 — セレクタの変更、コンパイルエラー、拒否、busy、Kernel 走査

**Files:**
- Modify: `runtime/src/Compile.cpp`（`acceptInto` のセレクタ比較）
- Test: `runtime/tests/debug_abi_test.cpp`

**Interfaces:**
- Consumes: Task 2 の `acceptInto(..., const std::string_view* selector, ...)`、`sessionDebugAccept`。
- Produces: なし（振る舞いだけ）。

- [ ] **Step 1: 失敗するテストを書く**

`debug_abi_test.cpp` の先頭の include に `#include "ao/Bootstrap.hpp"`、`#include "ao/MethodDictionary.hpp"`、`#include "ao/WellKnown.hpp"` を足し、無名名前空間の中（`DebugAbi` の前）に `remove_abi_test.cpp` と同じ走査を置く:

```cpp
// SPEC §6: every pair left in a native-required dictionary (both sides) holds a NativeMethod.
// The same scan as remove_abi_test.cpp's.
bool kernelDictsAreNative() {
  struct Scan {
    ao::Session* s;
    bool ok = true;
    void visit(ao::Oop cls) {
      if (!cls.isHeap()) return;
      const ao::Oop dict = s->heap.slotAt(cls, ao::kClassSlotMethodDict);
      if (!dict.isHeap()) return;
      const ao::Oop inner = s->heap.slotAt(dict, ao::kDictSlotArray);
      if (!inner.isHeap()) return;
      for (std::uint32_t i = 0; i + 1 < s->heap.size(inner); i += 2) {
        if (s->heap.slotAt(inner, i).isNil()) continue;
        const ao::Oop v = s->heap.slotAt(inner, i + 1);
        if (!v.isHeap() || s->heap.klass(v) != s->wk.nativeMethodClass) ok = false;
      }
    }
  } scan{ao::session()};
  scan.s->wk.eachNativeRequiredClass(
      [](void* p, ao::Oop cls) {
        auto* sc = static_cast<Scan*>(p);
        sc->visit(cls);
        sc->visit(sc->s->heap.klass(cls));
      },
      &scan);
  return scan.ok;
}
```

末尾に:

```cpp
TEST_F(LiveAccept, AcceptCompileErrorChangesNothing) {
  defineClass("DbgAccErr");
  accept("DbgAccErr", 0, "bar\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccErr new bar"));
  const std::int64_t pid = selectHalted();
  const int resultBefore = ao_eval_result_length();
  ASSERT_EQ(AO_ERR_COMPILE, acceptIn(pid, 1, "bar\n  ^1 +"));
  EXPECT_STRNE("", err_.message);
  EXPECT_LE(err_.start, err_.end);
  EXPECT_EQ(1, ao_debug_halted_count());
  EXPECT_EQ(resultBefore, ao_eval_result_length());
  ASSERT_EQ(AO_OK, ao_debug_select(pid));
  EXPECT_EQ("halt", reason());
  EXPECT_EQ("bar\n  self halt.\n  ^1", source(1).text);
  EXPECT_EQ(1, ao_debug_can_accept(pid, 1));
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("1", out_);
}

TEST_F(LiveAccept, AcceptSelectorChangeIsRefused) {
  defineClass("DbgAccSel");
  accept("DbgAccSel", 0, "bar\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccSel new bar"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(AO_ERR_COMPILE, acceptIn(pid, 1, "baz\n  ^2"));
  EXPECT_STREQ("debugger accept refused: selector changed to baz", err_.message);
  EXPECT_EQ(0u, err_.start);
  EXPECT_EQ(0u, err_.end);
  EXPECT_EQ(1, ao_debug_halted_count());
  EXPECT_EQ(AO_OK, ao_debug_abort(pid));
  ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  ASSERT_EQ(AO_ERR_EVAL, doIt("DbgAccSel new baz"));
  EXPECT_STREQ("doesNotUnderstand: #baz", err_.message);
}

TEST_F(LiveAccept, AcceptRefusalMessages) {
  defineClass("DbgAccMsg");
  accept("DbgAccMsg", 0, "makeBlock\n  ^[self halt. 3]");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccMsg new makeBlock value"));
  const std::int64_t pid = selectHalted();
  EXPECT_EQ(AO_ERR, acceptIn(pid, 1, "makeBlock\n  ^4"));
  EXPECT_STREQ("debugger accept refused: home frame is not on the stack", err_.message);
  EXPECT_EQ(AO_ERR, acceptIn(pid, 0, "x\n  ^1"));
  EXPECT_STREQ("debugger accept refused: not an accepted method", err_.message);
  EXPECT_EQ(AO_ERR, acceptIn(pid, 2, "doIt\n  ^1"));  // the doIt
  EXPECT_STREQ("debugger accept refused: not an accepted method", err_.message);
  EXPECT_EQ(AO_ERR, acceptIn(pid, 99, "x\n  ^1"));
  EXPECT_STREQ("debugger accept refused: not an accepted method", err_.message);
  EXPECT_EQ(0u, err_.start);
  EXPECT_EQ(0u, err_.end);
  EXPECT_EQ(1, ao_debug_halted_count());
}

TEST_F(LiveAccept, AcceptUnknownPidFails) {
  EXPECT_EQ(AO_ERR, acceptIn(424242, 1, "bar\n  ^1"));
  EXPECT_STREQ("", err_.message);
  ASSERT_EQ(AO_ERR_HALT, doIt("self halt"));
  const std::int64_t pid = ao_debug_halted_pid();
  EXPECT_EQ(AO_ERR, acceptIn(pid, 1, nullptr));
  EXPECT_STREQ("", err_.message);
  EXPECT_EQ(AO_ERR, ao_debug_accept(pid, 1, "bar\n  ^1", nullptr, 16, &err_));
  EXPECT_EQ(AO_ERR, ao_debug_accept(pid, 1, "bar\n  ^1", out_, 0, &err_));
  EXPECT_EQ(1, ao_debug_halted_count());
}

TEST_F(LiveAccept, AcceptWhileBusyIsRefused) {
  defineClass("DbgAccBusy");
  accept("DbgAccBusy", 0, "bar\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("DbgAccBusy new bar"));
  struct Hook {
    std::int64_t pid = 0;
    int answer = 0;
    int can = -1;
  } hook;
  hook.pid = ao_debug_halted_pid();
  ao_set_transcript_hook(
      [](const char*, int, int is_clear, void* user) {
        if (is_clear != 0) {
          return;
        }
        auto* p = static_cast<Hook*>(user);
        char out[16];
        AoSpan err{};
        p->can = ao_debug_can_accept(p->pid, 1);
        p->answer = ao_debug_accept(p->pid, 1, "bar\n  ^2", out, sizeof out, &err);
      },
      &hook);
  ASSERT_EQ(AO_OK, doIt("Transcript show: 'x'"));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(1, hook.can);
  EXPECT_EQ(AO_ERR, hook.answer);
  EXPECT_EQ(1, ao_debug_halted_count());
  ASSERT_EQ(AO_OK, proceed(hook.pid)) << err_.message;
  EXPECT_STREQ("1", out_);
}

TEST_F(LiveAccept, KernelScanStaysGreenAfterDebuggerAccept) {
  ASSERT_TRUE(kernelDictsAreNative());
  accept("Object", 0, "p15scan\n  self halt.\n  ^1");
  ASSERT_EQ(AO_ERR_HALT, printIt("Object new p15scan"));
  const std::int64_t pid = selectHalted();
  ASSERT_EQ(AO_ERR_HALT, acceptIn(pid, 1, "p15scan\n  ^2")) << err_.message;
  ASSERT_EQ(AO_OK, proceed(pid)) << err_.message;
  EXPECT_STREQ("2", out_);
  EXPECT_TRUE(kernelDictsAreNative());
  ASSERT_EQ(AO_OK, printIt("3 + 4"));
  EXPECT_STREQ("7", out_);
}
```

- [ ] **Step 2: 失敗を確かめる**

Run: `cmake --build build && ctest --test-dir build -R 'LiveAccept' --output-on-failure`
Expected: `AcceptSelectorChangeIsRefused` だけ FAIL（`AO_ERR_HALT` が返り、`baz` が入る）。ほかは Task 1–2 の実装で PASS。

- [ ] **Step 3: セレクタを比べる**

`runtime/src/Compile.cpp` の `acceptInto` で、`compileMethod` が通った直後（`Root old(...)` の前）に:

```cpp
  // SPEC §3.13 Accept (P15): a source for another selector installs nothing.
  if (selector != nullptr && cr.image.selector != *selector) {
    assignError(error, "debugger accept refused: selector changed to " + cr.image.selector);
    return false;
  }
```

- [ ] **Step 4: 緑を確かめる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure`
Expected: 全件 PASS。

- [ ] **Step 5: コミット**

```bash
git add runtime/src/Compile.cpp runtime/tests/debug_abi_test.cpp
git commit -m "Refuse a Debugger Accept that changes the selector" -m "Cover the failure paths of ao_debug_accept: compile errors and refusals
change nothing, busy is refused, and the Kernel scan stays green.

Graphify: query acceptMethodInto rememberMethodSource invalidateMethodCache
Serena: find_referencing_symbols acceptMethodInto

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: Debugger のソース枠の編集、Accept、error 行

**Files:**
- Modify: `app/Ao/DebuggerWindow.swift`
- Test: `app/AoTests/DebuggerWindowTests.swift`

**Interfaces:**
- Consumes: `ao_debug_can_accept`、`ao_debug_accept`、`selectErrorSpan(status:span:source:base:in:)`、`spanMessage(_:)`。
- Produces（`DebuggerWindow` の内部 API。Task 5、6 が使う）:
  - `var canAcceptEdit: Bool`
  - `var hasUnacceptedChanges: Bool`
  - `var errorText: String`
  - `func replaceSource(_ value: String)`
  - `func accept()`
  - `private var shownSource: String`、`private let errorLine: NSTextField`

- [ ] **Step 1: Serena で取る**

`get_symbols_overview app/Ao/DebuggerWindow.swift`。`find_symbol` 本体つき: `DebuggerWindow/showFrame`、`DebuggerWindow/resume`、`DebuggerWindow/reload`、`DebuggerWindow/updateButtons`、`DebuggerWindow/installButtons`。`find_referencing_symbols DebuggerWindow/resume`。

- [ ] **Step 2: 失敗するテストを書く**

`DebuggerWindowTests.swift` の `testProcessTerminatedElsewhereDisablesDebugger` の下に:

```swift
  // SPEC §3.9 Debugger の編集: the source pane is editable only on a frame Accept can target.
  func testDebuggerSourceEditableOnlyForAcceptableFrames() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinEdit")
    acceptMethod("DbgWinEdit", "bar\n  self halt.\n  ^1")
    let workspace = workspace("DbgWinEdit new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("halt opened no Debugger")
      return
    }
    XCTAssertEqual(debugger.frameLabels, ["Object>>halt native ao_Object_halt", "DbgWinEdit>>bar", "doIt"])
    XCTAssertFalse(debugger.sourceIsEditable)
    debugger.selectFrame(1)
    XCTAssertTrue(debugger.sourceIsEditable)
    XCTAssertTrue(debugger.canAcceptEdit)
    debugger.selectFrame(2)
    XCTAssertFalse(debugger.sourceIsEditable)
    button("Abort", in: debugger)?.performClick(nil)
  }

  // SPEC §3.9: a post-mortem Debugger stays read-only.
  func testPostmortemDebuggerSourceStaysReadOnly() {
    defineAccClass("DbgWinPm")
    acceptMethod("DbgWinPm", "bar\n  ^nil foo")
    guard let debugger = debugAfterDoIt("DbgWinPm new bar") else {
      return
    }
    debugger.selectFrame(1)
    XCTAssertFalse(debugger.sourceIsEditable)
    XCTAssertFalse(debugger.canAcceptEdit)
  }

  // SPEC §3.9: Accept reads the frames anew and selects the new activation, innermost.
  func testAcceptInDebuggerSelectsNewActivation() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinAcc")
    acceptMethod("DbgWinAcc", "bar\n  ^self zork")
    let workspace = workspace("DbgWinAcc new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("DNU opened no Debugger")
      return
    }
    debugger.selectFrame(1)
    debugger.replaceSource("bar\n  ^42")
    XCTAssertTrue(debugger.hasUnacceptedChanges)
    debugger.accept()
    XCTAssertTrue(debugger.window.isVisible)
    XCTAssertEqual(debugger.title, "Debugger: accepted")
    XCTAssertEqual(debugger.selectedFrame, 0)
    XCTAssertEqual(debugger.frameLabels.first, "DbgWinAcc>>bar")
    XCTAssertEqual(debugger.sourceText, "bar\n  ^42")
    XCTAssertFalse(debugger.hasUnacceptedChanges)
    XCTAssertEqual(debugger.errorText, "")
    button("Abort", in: debugger)?.performClick(nil)
  }

  func testAcceptThenProceedInsertsNewResult() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinAccP")
    acceptMethod("DbgWinAccP", "bar\n  ^self zork")
    let workspace = workspace("DbgWinAccP new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("DNU opened no Debugger")
      return
    }
    debugger.selectFrame(1)
    debugger.replaceSource("bar\n  ^42")
    debugger.accept()
    button("Proceed", in: debugger)?.performClick(nil)
    XCTAssertEqual(workspace.text, "DbgWinAccP new bar42")
    XCTAssertFalse(debugger.window.isVisible)
  }

  // SPEC §3.9: a compile error goes to the error line, its span is selected, and nothing else moves.
  func testAcceptCompileErrorShowsReasonAndKeepsText() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinAccErr")
    acceptMethod("DbgWinAccErr", "bar\n  self halt.\n  ^1")
    let workspace = workspace("DbgWinAccErr new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("halt opened no Debugger")
      return
    }
    debugger.selectFrame(1)
    let broken = "bar\n  ^1 +"
    debugger.replaceSource(broken)
    debugger.accept()
    XCTAssertNotEqual(debugger.errorText, "")
    XCTAssertEqual(debugger.sourceText, broken)
    XCTAssertEqual(debugger.selectedFrame, 1)
    XCTAssertEqual(debugger.title, "Debugger: halt")
    XCTAssertTrue(debugger.hasUnacceptedChanges)
    XCTAssertEqual(ao_debug_halted_count(), 1)
    let errorLine = views(in: debugger.window.contentView, of: NSTextField.self)
      .first { $0.accessibilityLabel() == "Accept error" }
    XCTAssertEqual(errorLine?.stringValue, debugger.errorText)
    XCTAssertEqual(errorLine?.font, NSFont.systemFont(ofSize: NSFont.systemFontSize))
  }

  // Review Focus 4: after a compile error, a fixed Accept goes through and clears the error line.
  func testAcceptAfterCompileErrorClearsErrorLine() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinAccFix")
    acceptMethod("DbgWinAccFix", "bar\n  self halt.\n  ^1")
    let workspace = workspace("DbgWinAccFix new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("halt opened no Debugger")
      return
    }
    debugger.selectFrame(1)
    debugger.replaceSource("bar\n  ^1 +")
    debugger.accept()
    XCTAssertNotEqual(debugger.errorText, "")
    debugger.replaceSource("bar\n  ^5")
    debugger.accept()
    XCTAssertEqual(debugger.errorText, "")
    XCTAssertEqual(debugger.title, "Debugger: accepted")
    button("Proceed", in: debugger)?.performClick(nil)
    XCTAssertEqual(workspace.text, "DbgWinAccFix new bar5")
  }

  // Review Focus 5: the error span counts UTF-8 bytes; the pane selects it in UTF-16.
  func testAcceptCompileErrorSpanAfterJapaneseComment() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinAccJa")
    acceptMethod("DbgWinAccJa", "bar\n  self halt.\n  ^1")
    let workspace = workspace("DbgWinAccJa new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("halt opened no Debugger")
      return
    }
    debugger.selectFrame(1)
    let broken = "bar\n  \"日本語のコメント\"\n  ^1 + )"
    debugger.replaceSource(broken)
    debugger.accept()
    XCTAssertNotEqual(debugger.errorText, "")
    let selection = debugger.sourceSelection
    let comment = (broken as NSString).range(of: "\"日本語のコメント\"")
    XCTAssertGreaterThan(selection.length, 0)
    XCTAssertGreaterThanOrEqual(selection.location, NSMaxRange(comment))
    XCTAssertLessThanOrEqual(NSMaxRange(selection), (broken as NSString).length)
  }
```

`private func acceptDbgWin` の下にヘルパを足す:

```swift
  private func defineAccClass(_ name: String) {
    var err = AoSpan()
    let definition =
      "Object subclass: #\(name)\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'P15-Test'!\n"
    let defined = definition.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
    XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
  }
```

編集を残したまま終わるテストも、tearDown の `window.close()` は `windowShouldClose` を通らないので、確認のシートで止まらない。

- [ ] **Step 3: 失敗を確かめる**

Run: `./scripts/build.sh && swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter DebuggerWindowTests`
Expected: コンパイルエラー `value of type 'DebuggerWindow' has no member 'canAcceptEdit'`。

- [ ] **Step 4: 実装する**

`DebuggerWindow` のプロパティ（`private let sourceView: NSTextView` の下）に:

```swift
  // SPEC §3.9 Debugger の編集 (P15): the source as last shown, so an edit can be told apart, and
  // the line where a refused Accept says why (live only).
  private var shownSource = ""
  private let errorLine = NSTextField(labelWithString: "")
```

公開の読み（`sourceIsEditable` の下）:

```swift
  var errorText: String {
    errorLine.stringValue
  }

  var hasUnacceptedChanges: Bool {
    sourceView.isEditable && sourceView.string != shownSource
  }

  // SPEC §3.9: live, halted, and the selected frame has an Accept target (ao_debug_can_accept).
  var canAcceptEdit: Bool {
    guard isLive, !finished, let selectedFrame, ao_debug_select(pid) == Int32(AO_OK) else {
      return false
    }
    return ao_debug_can_accept(pid, Int32(selectedFrame)) == 1
  }

  func replaceSource(_ value: String) {
    sourceView.string = value
  }
```

`restart()` の下に:

```swift
  // SPEC §3.9 Debugger の編集: Smalltalk → Accept. A refusal or a compile error stays in the
  // window (error line, span selected); a halt reads everything anew, as the buttons do.
  func accept() {
    guard let selectedFrame, canAcceptEdit else {
      return
    }
    let text = sourceView.string
    resume({ pid, out, outLen, err in
      text.withCString { ao_debug_accept(pid, Int32(selectedFrame), $0, out, outLen, err) }
    }, refused: { status, err in
      self.errorLine.stringValue = spanMessage(err)
      selectErrorSpan(status: status, span: err, source: text, base: 0, in: self.sourceView)
    })
  }
```

`resume` に `refused` を足す:

```swift
  private func resume(
    _ call: (Int64, UnsafeMutablePointer<CChar>, Int32, UnsafeMutablePointer<AoSpan>) -> Int32,
    refused: ((Int32, AoSpan) -> Void)? = nil
  ) {
    ...（around の呼び出しまでは今のまま）
    if status == Int32(AO_ERR_HALT) {
      reload()
      return
    }
    // SPEC §3.9: Accept's refusals (a message) and compile errors leave the process halted.
    if let refused,
       status == Int32(AO_ERR_COMPILE) || (status == Int32(AO_ERR) && !spanMessage(err).isEmpty) {
      refused(status, err)
      updateButtons()
      return
    }
    guard status != Int32(AO_ERR) else {
    ...（以降は今のまま）
```

`reload()` の先頭に `errorLine.stringValue = ""` を足す。

`showFrame(_:)` の `sourceView.string = frame.source` の直後に:

```swift
    shownSource = frame.source
    sourceView.isEditable = canAcceptEdit
```

`updateButtons()` の末尾に（ほかで終わった process の枠を読み取り専用に戻す）:

```swift
    if !halted {
      sourceView.isEditable = false
    }
```

`installButtons(above:size:)` で error 行を下に置く:

```swift
    let barHeight: CGFloat = 32
    let errorHeight: CGFloat = 20
    let container = NSView(frame: NSRect(origin: .zero, size: size))
    container.autoresizingMask = [.width, .height]
    split.frame = NSRect(
      x: 0, y: errorHeight, width: size.width, height: max(size.height - barHeight - errorHeight, 0)
    )
    // SPEC §3.9: the error line keeps the system font size (文字の大きさ).
    errorLine.frame = NSRect(x: 8, y: 2, width: max(size.width - 16, 0), height: 16)
    errorLine.autoresizingMask = [.width, .maxYMargin]
    errorLine.font = NSFont.systemFont(ofSize: NSFont.systemFontSize)
    errorLine.lineBreakMode = .byTruncatingTail
    errorLine.setAccessibilityLabel("Accept error")
    container.addSubview(errorLine)
```

（以降の `runs` とボタンの作成は今のまま。）

- [ ] **Step 5: 緑を確かめる**

Run: Step 3 と同じ。
Expected: `DebuggerWindowTests` 全件 PASS（既存の `testDebuggerControlsHaveAccessibilityLabels` を含む）。

- [ ] **Step 6: コミット**

```bash
git add app/Ao/DebuggerWindow.swift app/AoTests/DebuggerWindowTests.swift
git commit -m "Edit and Accept in the live Debugger's source pane" -m "The pane is editable on frames ao_debug_can_accept allows. Accept halts in
the new activation and reads the frames anew; a refusal or compile error
goes to an error line with its span selected.

Graphify: path DebuggerWindow ao_debug_accept
Serena: replace_symbol_body DebuggerWindow/resume, DebuggerWindow/showFrame

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: 破棄の確認（フレームの選択、ボタン、窓を閉じる）

**Files:**
- Modify: `app/Ao/BrowserWindow.swift`（`askToDiscard` の `private` を外す）
- Modify: `app/Ao/DebuggerWindow.swift`
- Test: `app/AoTests/DebuggerWindowTests.swift`

**Interfaces:**
- Consumes: Task 4 の `hasUnacceptedChanges`、`shownSource`。`BrowserWindow.DiscardConfirmation`、`BrowserWindow.askToDiscard`。
- Produces: `var confirmDiscard: BrowserWindow.DiscardConfirmation`（テストが差し替える）、`private func confirmIfEdited(_:)`、`private func moveSelection(to:)`。

- [ ] **Step 1: Serena で取る**

`find_symbol` 本体つき: `DebuggerWindow/selectFrame`、`DebuggerWindow/windowShouldClose`、`DebuggerWindow/installButtons`、`BrowserWindow/askToDiscard`。`find_referencing_symbols DebuggerWindow/selectFrame`（`init`、`reload`、テスト）。

- [ ] **Step 2: 失敗するテストを書く**

```swift
  // SPEC §3.9: with an unaccepted edit, choosing a frame, a button or closing asks first; Cancel
  // changes nothing, Discard goes ahead.
  func testUnacceptedEditAsksToDiscardFirst() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinAsk")
    acceptMethod("DbgWinAsk", "bar\n  self halt.\n  ^1")
    let workspace = workspace("DbgWinAsk new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("halt opened no Debugger")
      return
    }
    var asked = 0
    var answer = false
    debugger.confirmDiscard = { _, decide in
      asked += 1
      decide(answer)
    }
    debugger.selectFrame(1)
    XCTAssertEqual(asked, 0)
    debugger.replaceSource("bar\n  ^2")
    // Frame: Cancel keeps the selection and the edit.
    debugger.selectFrame(2)
    XCTAssertEqual(asked, 1)
    XCTAssertEqual(debugger.selectedFrame, 1)
    XCTAssertEqual(debugger.sourceText, "bar\n  ^2")
    // Buttons: Cancel does nothing.
    button("Proceed", in: debugger)?.performClick(nil)
    XCTAssertEqual(asked, 2)
    XCTAssertTrue(debugger.window.isVisible)
    XCTAssertEqual(ao_debug_halted_count(), 1)
    // Close: Cancel keeps the window and the process.
    debugger.window.performClose(nil)
    XCTAssertEqual(asked, 3)
    XCTAssertTrue(debugger.window.isVisible)
    XCTAssertEqual(ao_debug_halted_count(), 1)
    // Discard: the frame changes and the edit is gone.
    answer = true
    debugger.selectFrame(2)
    XCTAssertEqual(asked, 4)
    XCTAssertEqual(debugger.selectedFrame, 2)
    XCTAssertFalse(debugger.hasUnacceptedChanges)
    // No edit: no question.
    button("Abort", in: debugger)?.performClick(nil)
    XCTAssertEqual(asked, 4)
    XCTAssertFalse(debugger.window.isVisible)
  }
```

- [ ] **Step 3: 失敗を確かめる**

Run: Task 4 Step 3 と同じ（`--filter DebuggerWindowTests/testUnacceptedEditAsksToDiscardFirst`）。
Expected: コンパイルエラー `value of type 'DebuggerWindow' has no member 'confirmDiscard'`。

- [ ] **Step 4: 実装する**

`BrowserWindow.swift`: `private static func askToDiscard(` を `static func askToDiscard(` にする（中身は変えない）。

`DebuggerWindow.swift` のプロパティに:

```swift
  // SPEC §3.9 Debugger の編集: asks before an unaccepted edit goes (Browser's question). Tests
  // replace it.
  var confirmDiscard: BrowserWindow.DiscardConfirmation = BrowserWindow.askToDiscard
  private var confirming = false
```

`selectFrame(_:)` を確認つきにし、元の動きを `moveSelection(to:)` に移す:

```swift
  // Through the table, so a click and a test take the same path (tableViewSelectionDidChange).
  func selectFrame(_ index: Int) {
    guard index >= 0, index < frames.count else {
      return
    }
    confirmIfEdited { self.moveSelection(to: index) }
  }
```

private に:

```swift
  private func moveSelection(to index: Int) {
    frameTable.selectRowIndexes(IndexSet(integer: index), byExtendingSelection: false)
    if frameTable.selectedRow == index {
      showFrame(index)
    }
  }

  // SPEC §3.9 Debugger の編集: an unaccepted edit goes only after the question; Cancel leaves the
  // selection and the operation alone. One question at a time.
  private func confirmIfEdited(_ run: @escaping @MainActor () -> Void) {
    guard !confirming else {
      return
    }
    guard hasUnacceptedChanges else {
      run()
      return
    }
    confirming = true
    confirmDiscard(window) { discard in
      self.confirming = false
      guard discard || !self.hasUnacceptedChanges else {
        return
      }
      self.sourceView.string = self.shownSource
      run()
    }
  }
```

`init` と `reload()` の `selectFrame(0)` を `moveSelection(to: 0)` にする（読み直しは編集を捨てる。SPEC §3.9 の `AO_ERR_HALT`）。

マウスのクリック（AppKit はプログラムの `selectRowIndexes` ではこれを呼ばない）:

```swift
  func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
    guard tableView === frameTable, hasUnacceptedChanges else {
      return true
    }
    confirmIfEdited { self.moveSelection(to: row) }
    return false
  }
```

`installButtons` のボタンの実行を包む:

```swift
      buttonActions.runs[ObjectIdentifier(button)] = { [weak self] in
        guard let self else {
          return
        }
        self.confirmIfEdited { run(self) }
      }
```

`windowShouldClose(_:)`:

```swift
  func windowShouldClose(_ sender: NSWindow) -> Bool {
    if isLive, !finished, EvaluationActivity.isActive {
      return false
    }
    guard hasUnacceptedChanges else {
      return true
    }
    confirmIfEdited { self.window.close() }
    return false
  }
```

- [ ] **Step 5: 緑を確かめる**

Run: Task 4 Step 3 と同じ（フィルタなしで `DebuggerWindowTests` と `AcceptTests`）。
Expected: PASS。

- [ ] **Step 6: コミット**

```bash
git add app/Ao/DebuggerWindow.swift app/Ao/BrowserWindow.swift app/AoTests/DebuggerWindowTests.swift
git commit -m "Ask before the Debugger drops an unaccepted edit" -m "Choosing a frame, pressing a button or closing the window asks first, with
the Browser's question; Cancel changes nothing.

Graphify: path DebuggerWindow BrowserWindow
Serena: replace_symbol_body DebuggerWindow/selectFrame, DebuggerWindow/windowShouldClose

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 6: Smalltalk → Accept の宛先

**Files:**
- Modify: `app/Ao/DebuggerWindow.swift`（`owning`、`allowsAccept`）
- Modify: `app/Ao/MainMenu.swift`（`canAccept`）
- Modify: `app/Ao/AoApp.swift`（`accept` と `canAccept`）
- Test: `app/AoTests/DebuggerWindowTests.swift`

**Interfaces:**
- Consumes: Task 4 の `canAcceptEdit`、`accept()`。`EvaluationActivity.isActive`、`sendToKeyBrowser`。
- Produces: `static func owning(_ window: NSWindow?) -> DebuggerWindow?`、`static func allowsAccept(keyWindow: NSWindow?) -> Bool`、`MainMenu.Actions.canAccept: () -> Bool`。

- [ ] **Step 1: Serena で取る**

`find_symbol MainMenu/Actions`（`depth=1`）、`find_symbol AoApp/applicationWillFinishLaunching` 本体つき、`find_referencing_symbols MainMenu/Actions/canEvaluate`。

- [ ] **Step 2: 失敗するテストを書く**

```swift
  // SPEC §3.9: Smalltalk → Accept follows the key live Debugger's pane; it stays off while
  // evaluating, as before.
  func testAcceptMenuFollowsDebuggerEditability() {
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    defineAccClass("DbgWinMenu")
    acceptMethod("DbgWinMenu", "bar\n  self halt.\n  ^1")
    let workspace = workspace("DbgWinMenu new bar")
    workspace.selectAll()
    workspace.printIt()
    guard let debugger = workspace.debuggers.last else {
      XCTFail("halt opened no Debugger")
      return
    }
    XCTAssertTrue(DebuggerWindow.owning(debugger.window) === debugger)
    XCTAssertNil(DebuggerWindow.owning(workspace.window))
    XCTAssertFalse(DebuggerWindow.allowsAccept(keyWindow: debugger.window))  // native row
    debugger.selectFrame(1)
    XCTAssertTrue(DebuggerWindow.allowsAccept(keyWindow: debugger.window))
    XCTAssertTrue(DebuggerWindow.allowsAccept(keyWindow: workspace.window))
    XCTAssertTrue(DebuggerWindow.allowsAccept(keyWindow: nil))
    // The menu item asks canAccept.
    var allowed = false
    var actions = MainMenu.Actions()
    actions.canAccept = { allowed }
    let menu = MainMenu.build(actions: actions)
    guard let accept = menu.item(withTitle: "Smalltalk")?.submenu?.item(withTitle: "Accept") else {
      XCTFail("missing Accept")
      return
    }
    func enabled() -> Bool {
      (accept.target as? NSMenuItemValidation)?.validateMenuItem(accept) ?? true
    }
    XCTAssertFalse(enabled())
    allowed = true
    XCTAssertTrue(enabled())
    button("Abort", in: debugger)?.performClick(nil)
  }
```

- [ ] **Step 3: 失敗を確かめる**

Run: Task 4 Step 3 と同じ（`--filter testAcceptMenuFollowsDebuggerEditability`）。
Expected: コンパイルエラー `type 'DebuggerWindow' has no member 'owning'`。

- [ ] **Step 4: 実装する**

`DebuggerWindow.swift` の `snapshotFrameCount()` の下に:

```swift
  // SPEC §3.9 Debugger の編集: the open Debugger whose window is `window`.
  static func owning(_ window: NSWindow?) -> DebuggerWindow? {
    guard let window else {
      return nil
    }
    return open.first { $0.window === window }
  }

  // Smalltalk → Accept with `keyWindow` key: a Debugger there allows it only on an editable pane;
  // any other window keeps the item as it was.
  static func allowsAccept(keyWindow: NSWindow?) -> Bool {
    owning(keyWindow)?.canAcceptEdit ?? true
  }
```

`MainMenu.swift` の `Actions` に（`accept` の下）:

```swift
    // SPEC §3.9 Debugger の編集: off while evaluating, and on a key Debugger without an editable pane.
    var canAccept: () -> Bool = { true }
```

Accept の項目を:

```swift
      actionItem("Accept", key: "", run: { _ in actions.accept() }, enabled: actions.canAccept),
```

`AoApp.swift` の `accept:` と、その下に `canAccept:`:

```swift
      accept: {
        let key = NSApplication.shared.keyWindow
        if let debugger = DebuggerWindow.owning(key) {
          debugger.accept()
          return
        }
        sendToKeyBrowser(self.browser, keyWindow: key) { $0.accept() }
      },
      canAccept: {
        !EvaluationActivity.isActive
          && DebuggerWindow.allowsAccept(keyWindow: NSApplication.shared.keyWindow)
      },
```

（`Actions` のメンバの並びは宣言順に合わせる: `accept` の直後に `canAccept`。）

- [ ] **Step 5: 緑を確かめる**

Run: `./scripts/test.sh`
Expected: CTest と `swift test` 全件 PASS（`AcceptTests` の「Items without a test stay enabled」は既定の `canAccept` が真なので変わらない）。

- [ ] **Step 6: コミット**

```bash
git add app
git commit -m "Route Smalltalk > Accept to the key live Debugger" -m "Graphify: path AoApp DebuggerWindow
Serena: replace_symbol_body AoApp/applicationWillFinishLaunching, find_referencing_symbols MainMenu/Actions/canEvaluate

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 7: ベンチ、完了の記録、グラフ

**Files:**
- Modify: `docs/bench.md`、`PHASE`、`SPEC.md`（§6「Debugger の編集」）、`CHANGELOG.md`、`docs/README.md`
- Create: `docs/phases/P15.md`
- Modify: `graphify-out/*`

- [ ] **Step 1: ベンチを測る**

`docs/bench.md` の「P11 step branch」と同じ手順で、`ac349e6`（P15 の前）とこのブランチの Release を交互に 5 回ずつ:

Run: `./scripts/build.sh --release && ctest --test-dir build-release -R 'KernelBench.InlinedToDoMillion|KernelBench.TenMillionToDo' --output-on-failure`（比べる側は `git worktree add ../ao-p14 ac349e6` で同じ手順。どちらも `runtime/tests/kernel_scan_test.cpp:149` / `:168`）
Expected: 中央値の比（inlined / native）が P11 節の揺らぎ（Release 256–264 ms）を超えて悪化しない。`applyMethod` の分岐はホットループの外（送信ごと）なので、`interpretedSends` が 0 のループでは差が出ないはずである。

`docs/bench.md` の末尾に節を足す:

```markdown
## P15 accept branch

- Machine: Apple Silicon (`Apple M1 Max`). Date: <測った日>.
- Commits: `ac349e6` (before P15) and the P15 branch head (one `ctx.reactivating` test after `Interpreter::run` returns in `applyMethod`).
- Method: as the P11 section; medians of 5 alternate runs.

| Build | Commit | inlined ms | native ms | ratio |
|---|---|---|---|---|
| Release | `ac349e6` | <値> | <値> | <値> |
| Release | P15 | <値> | <値> | <値> |
```

（`<値>` は測った数字で埋める。埋めずにコミットしない。）

- [ ] **Step 2: フェーズの記録**

`PHASE` を `P15` にする。

`SPEC.md` §6「Debugger の編集」の各項目を、テストが確かめたものから `[x]` にする（P14 と同じく、Ao.app の手での確認は `docs/phases/P15.md` の「手動確認」に残す）。

`docs/phases/P15.md` を作る:

```markdown
# P15 — ライブ Debugger の編集

## 結論

ライブ Debugger のソース枠でメソッドを直して Accept し、そのフレームを新しいメソッドで起動し直す。正本は SPEC §3.9「Debugger の編集（P15）」、§3.10 `ao_debug_can_accept` / `ao_debug_accept`、§3.13「Accept（P15）」、§6「Debugger の編集」。実装計画は [`superpowers/plans/2026-10-09-p15-debugger-accept.md`](../superpowers/plans/2026-10-09-p15-debugger-accept.md)。次のマイナー版（1.4.0）に含める（SPEC §2.4）。

## 前提

P14 緑（1.3.0 リリース済み）。SPEC の P15 追記は実装より先（`ac349e6`）。

## 範囲

**やる:** `ao_debug_can_accept` / `ao_debug_accept`、Restart の巻き戻しに乗せた起動し直し（`applyMethod` が同じ receiver と引数で新しいメソッドを起動）、理由 `accepted` の停止、Debugger のソース枠の編集、error 行、破棄の確認、Smalltalk → Accept の宛先。

**やらない:** 事後 Debugger での編集、doIt の編集、セレクタを変える Accept、Debugger からのクラス定義の Accept、起動し直さずに入れるだけの Accept、temp の書き換え、CLI の変更。

## TDD

`debug_abi_test` の `LiveAccept.*` と `DebuggerWindowTests` の P15。

## 受け入れ

- [x] Debugger で直した `Foo>>bar` の最初の命令で `accepted` で止まり、Proceed で新しい値（`LiveAccept.AcceptStopsAtFirstInstructionOfNewMethod`、`ProceedAfterAcceptAnswersNewMethodValue`、`DebuggerWindowTests.testAcceptInDebuggerSelectsNewActivation`、`testAcceptThenProceedInsertsNewResult`）
- [x] Browser と以降の送信が直したメソッドを見る（`ProceedAfterAcceptAnswersNewMethodValue`、`AcceptInDebuggerUpdatesCachedSends`）
- [x] コンパイルエラーは error 行に出て、テキストもメソッドも変えず、プロセスは止まったまま（`AcceptCompileErrorChangesNothing`、`testAcceptCompileErrorShowsReasonAndKeepsText`）
- [x] セレクタの変更は拒む（`AcceptSelectorChangeIsRefused`）
- [x] doIt、合成ネイティブ、ソースの無いメソッドのフレームは編集できない（`AcceptDoItFrameIsRefused`、`AcceptNativeFrameIsRefused`、`AcceptPlaceholderFrameIsRefused`、`testDebuggerSourceEditableOnlyForAcceptableFrames`）
- [x] 外側の Accept は内側を捨て、内側の `ensure:` は走らない（`AcceptOuterFrameDropsInnerWithoutEnsure`）
- [x] 編集を残したままの操作は破棄の確認が先（`testUnacceptedEditAsksToDiscardFirst`）
- [x] ライブモード off は変えない（`AcceptPostmortemIsRefused`。CLI / `ao --test` はライブ入口を使わない）
- [x] Kernel 走査緑（`KernelScanStaysGreenAfterDebuggerAccept`、`KernelScan.*`）、`docs/bench.md` の比が悪化しない（「P15 accept branch」）
- [x] `PHASE` は `P15`、SPEC §6 の本小節がすべて `[x]`、CHANGELOG に項目

### 手動確認

- [ ] 1. Browser で `Foo>>bar` を `^self zork` で Accept → Workspace の `Foo new bar` を Print it → Debugger で `Foo>>bar` を選んで `^42` に直して Smalltalk → Accept → `accepted` で止まる → Proceed で `42`
- [ ] 2. 合成ネイティブ行と doIt 行ではソース枠が編集できない
- [ ] 3. 編集を残して別のフレームを選ぶとシートが出て、Cancel で何も変わらない
- [ ] 4. error 行を VoiceOver が「Accept error」と読む
```

`CHANGELOG.md` の `## [Unreleased]` の下に:

```markdown
Phase P15 of SPEC §2.3. The `.aoimage` format and the `ao` CLI do not change.

The live Debugger's source pane can be edited on frames whose method was accepted in this session (SPEC §3.9, §3.10, §3.13). Smalltalk → Accept installs the method into its defining class and starts the frame's activation over with the new method, in the send that started it, with the same receiver and arguments; it halts before the first instruction with reason `accepted`. Inner frames are discarded without running their `ensure:` / `ifCurtailed:` blocks.

### Runtime

- C ABI: `ao_debug_can_accept(pid, frame_index)` and `ao_debug_accept(pid, frame_index, source, out, out_len, err)`. Accept is an outermost entry (busy → `AO_ERR`). Answers match Proceed. Refusals: `debugger accept refused: not an accepted method`, `debugger accept refused: home frame is not on the stack` (`AO_ERR`), and `debugger accept refused: selector changed to <selector>` (`AO_ERR_COMPILE`). A refused or failed Accept changes nothing and leaves the process halted.

### Ao.app

- The live Debugger's source pane is editable on frames `ao_debug_can_accept` allows, with an error line for refused Accepts. Choosing another frame, pressing a button or closing the window with an unaccepted edit asks first.
- Smalltalk → Accept goes to the key live Debugger when its pane is editable.

### Known limitations

- No editing in the post-mortem Debugger or of the doIt, no selector change from the Debugger, and no class definitions from the Debugger.
```

`docs/README.md` の表の P14 の行の下に `| P15 Debugger の編集 | [phases/P15.md](phases/P15.md) | done |`、依存の行の末尾に ` → P15`、計画書の列挙の末尾（`P14 の設計: ...` のあと）に `。P15: [2026-10-09-p15-debugger-accept.md](superpowers/plans/2026-10-09-p15-debugger-accept.md)` を足す。

- [ ] **Step 3: 全体を回す**

Run: `./scripts/test.sh && ./scripts/test.sh --asan`
Expected: 両方 PASS。出力の末尾をそのまま記録する。

- [ ] **Step 4: グラフを更新する**

Run: `/graphify . --update`
Expected: `graphify-out/GRAPH_REPORT.md` に `ao_debug_accept`、`sessionDebugAccept`、`reactivateAccepted` が現れる。新しい循環 include が無いことを GRAPH_REPORT のサイクルで確かめる。

- [ ] **Step 5: コードレビュー**

`superpowers:requesting-code-review` でブランチ全体（`git diff main...HEAD`）を見てもらう。重点: 固定ルートの外し忘れ（`FrameLink`、`Scheduler::halt` の失敗、`reactivateAccepted`）、`Frame*` のダングリング、失敗時に何も変えないこと、`ctx.reactivating` が対象の `applyMethod` 以外で読まれないこと。

- [ ] **Step 6: コミット**

```bash
git add PHASE SPEC.md CHANGELOG.md docs graphify-out
git commit -m "Record P15 completion and refresh the knowledge graph" -m "Graphify: update . (Interpreter, Session, Scheduler, DebuggerWindow paths)
Serena: none (docs only)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

## 自己点検（SPEC との突き合わせ）

| SPEC | タスク |
|---|---|
| §3.10 `ao_debug_can_accept`（busy 可） | 1、3（`AcceptWhileBusyIsRefused` で busy 中に 1） |
| §3.10 `ao_debug_accept` の拒否メッセージ、`start` / `end` 0 | 1、3（`AcceptRefusalMessages`） |
| §3.10 コンパイル規則は `ao_accept_method_id` と同じ | 2（`acceptInto` を共有） |
| §3.10 セレクタの変更 | 3 |
| §3.10 失敗で何も変えない | 2（クリアは `resumeHalted` の中、導入のあと）、3（`AcceptCompileErrorChangesNothing`） |
| §3.10 NULL / 未知 pid / out の拒否 | 3（`AcceptUnknownPidFails`） |
| §3.10 入場時の消去、答えの形 | 2（`resumeHalted` を共有） |
| §3.10 評価の中断のフラグを Accept の入場でクリア | 2（`resumeHalted` の `clearInterruptRequest`。導入のあと） |
| §3.13 対象（種類 0 / 1、ホーム） | 1 |
| §3.13 入れ方、古い項目が消える | 2（`acceptMethodInto` と同じ `rememberMethodSource`） |
| §3.13 起動し直し（捨てる、`ensure:` なし、死んだ印、探索し直さない、temp の大きさ） | 2 |
| §3.13 `accepted` で止まる、Proceed はそこから | 2 |
| §3.13 戻さないもの（外側の再帰は古いまま） | 2（`AcceptKeepsOuterRecursionOnOldMethod`） |
| §3.13 Proceed 不能でも可 | 2（`AcceptOnNonProceedable`） |
| §3.9 編集できる条件、読み取り専用 | 4 |
| §3.9 Accept の答えごとの Debugger の動き、error 行、UTF-16 区間 | 4 |
| §3.9 破棄の確認 | 5 |
| §3.9 Smalltalk → Accept | 6 |
| §3.9 error 行の VoiceOver と文字の大きさ | 4 |
| §4 テスト名 | 1–6 |
| §6 受け入れ、`PHASE`、CHANGELOG、bench | 7 |

8 件上限（§3.13「止まっているプロセスがすでに 8 あって止められないときは… `accepted` で abort」）: Accept するプロセス自身は再開した時点で止まった数から外れるので、Accept の経路では上限に当たらない。`stepCheck` の既存の分岐（`abortEvaluation(ctx, reason)`）が理由 `accepted` を渡すので、文言は SPEC のとおりになる。専用のテストは足さない。

# Changelog

All notable changes to Ao are recorded here. Versions follow [Semantic Versioning](https://semver.org/) (SPEC §2.4).

## [Unreleased]

## [1.3.0] - 2026-10-04

Phase P14 of SPEC §2.3. The `.aoimage` format and the `ao` CLI do not change: images saved by 1.2.0 load in 1.3.0.

The live Debugger can Restart a selected interpreted frame from the start of its method or block (SPEC §3.9, §3.10, §3.13). Inner interpreted frames and native C stacks between them are discarded. Inner `ensure:` / `ifCurtailed:` blocks do not run (use Abort when cleanup is required). Synthesized native / DNU rows cannot Restart. Proceed-incapable stops (`NonBoolean receiver`, `cannot return`) may Restart an interpreted frame. Halt's "nil as the send result" path is not used.

### Runtime

- C ABI: `ao_debug_can_restart(pid, frame_index)` and `ao_debug_restart(pid, frame_index, out, out_len, err)`. Restart is an outermost entry (busy → `AO_ERR`). Answers match Proceed (`AO_OK` / `AO_ERR_HALT` / `AO_ERR_EVAL`). A synthesized or out-of-range index answers `AO_ERR` with `restart refused: not an interpreted frame`.
- The selected interpreted frame's pc is 0, its operand stack is empty, extra temps are nil, and arguments (and a block's copied values) are kept. The process resumes until it ends or halts again.

### Ao.app

- Live Debugger button row: Proceed, Abort, Restart, Step over, Step into, Step out. Restart follows the selected row (`ao_debug_can_restart`). Post-mortem Debugger has no Restart.

### Project

- Release assets: `Ao-1.3.0-macos-arm64.zip`, `ao-cli-1.3.0-macos-arm64.tar.gz`, and `SHA256SUMS`.

### Known limitations

- The live debugger has no editing in the Debugger, and does not step into natives.
- A frame whose method has no source (re-accepted or removed while halted, loaded from an image, or filed in) has no statement starts, so Step over and Step into do not stop at its statements: they stop only in a deeper frame (Step into) or back in the sender (SPEC §3.13).
- No JIT, FFI, networking, or catching of Smalltalk exception objects (SPEC §1.4, §5).
- The bytecode interpreter is not optimized ([docs/bench.md](docs/bench.md)).
- The builds are ad-hoc signed and not notarized, so macOS asks before the first launch.

### Install

Requires an Apple Silicon Mac with macOS 14 or later. Download `Ao-1.3.0-macos-arm64.zip` (the app) and/or `ao-cli-1.3.0-macos-arm64.tar.gz` (the CLI), then check them with `shasum -a 256 -c SHA256SUMS`. The builds are not notarized: after the first launch attempt, choose **Open Anyway** in System Settings → Privacy & Security, or run `xattr -dr com.apple.quarantine Ao.app`. See the [README](https://github.com/SilentMalachite/Ao-Smalltalk/blob/v1.3.0/README.md) for usage.

## [1.2.0] - 2026-09-30

Phase P13 of SPEC §2.3. The `.aoimage` format and the `ao` CLI do not change: images saved by 1.1.0 load in 1.2.0.

Ao.app can now interrupt an evaluation (SPEC §3.9, §3.10, §3.13). During a live evaluation, ⌘. or Smalltalk → Interrupt requests a halt with reason `interrupted` at an interpreter safepoint. The live Debugger can Proceed, Abort, or Step as in P11. The stop is a bytecode boundary (same shape as step / Debug it), not `Object>>halt`'s nil-as-send-result.

### Runtime

- C ABI: `ao_request_interrupt` sets a session interrupt flag (ok while busy; needs a session). `ao_set_runloop_pump_hook` registers a host RunLoop pump (same shape as the transcript hook; NULL clears it).
- At interpreter safepoints (a taken backward jump, and a method or block activation before its first instruction), the runtime may pump the host RunLoop (throttled) and, in live mode, halt the evaluating process the base is waiting on with reason `interrupted` (`AO_ERR_HALT`). The flag is consumed once. A halt on a backward jump shows the jump target as the frame pc, where Proceed goes on. Live mode off ignores the flag so CLI and `ao --test` stay unchanged.
- Forked or drain-only processes are not halted by the flag. Long natives that never reach an interpreter safepoint are not interrupted either.

### Ao.app

- Smalltalk → Interrupt (⌘.). While an evaluation (or Debugger resume) is active, Do it / Print it / Inspect it / Debug it / Accept are disabled and Interrupt is enabled.
- A RunLoop pump is registered at launch. It takes queued window-server events and hands them to `NSApplication`, so ⌘. and the Interrupt menu item are delivered during a synchronous `ao_eval`. Nested Proceed / Step keep the evaluation state until the outermost evaluation ends.
- The interrupt request is cleared when any evaluation ends (including a live halt for another reason) and on entry to Proceed / Step, so a stale request never stops the next Step with `interrupted`.
- While an evaluation runs, closing a live Debugger (for another halted evaluation) is refused, as its Abort would be; close it again after the evaluation ends.
- If the Workspace text got shorter during a Print it (an edit the pump delivered), the result goes at the end of the text instead of being dropped.

### Project

- Release assets: `Ao-1.2.0-macos-arm64.zip`, `ao-cli-1.2.0-macos-arm64.tar.gz`, and `SHA256SUMS`.

### Known limitations

- A tight loop on the evaluating process the base is waiting for, once it reaches an interpreter safepoint, can be interrupted with ⌘. / Interrupt. Loops on other processes (fork / drain) and long natives that never hit an interpreter safepoint still require a force quit.
- The live debugger has no Restart, no editing in the Debugger, and does not step into natives.
- A frame whose method has no source (re-accepted or removed while halted, loaded from an image, or filed in) has no statement starts, so Step over and Step into do not stop at its statements: they stop only in a deeper frame (Step into) or back in the sender (SPEC §3.13).
- No JIT, FFI, networking, or catching of Smalltalk exception objects (SPEC §1.4, §5).
- The bytecode interpreter is not optimized ([docs/bench.md](docs/bench.md)).
- The builds are ad-hoc signed and not notarized, so macOS asks before the first launch.

### Install

Requires an Apple Silicon Mac with macOS 14 or later. Download `Ao-1.2.0-macos-arm64.zip` (the app) and/or `ao-cli-1.2.0-macos-arm64.tar.gz` (the CLI), then check them with `shasum -a 256 -c SHA256SUMS`. The builds are not notarized: after the first launch attempt, choose **Open Anyway** in System Settings → Privacy & Security, or run `xattr -dr com.apple.quarantine Ao.app`. See the [README](https://github.com/SilentMalachite/Ao-Smalltalk/blob/v1.2.0/README.md) for usage.

## [1.1.0] - 2026-09-27

Phases P10–P12 of SPEC §2.3, the first phases after v1. The `.aoimage` format and the `ao` CLI do not change: images saved by 1.0.0 load in 1.1.0.

Phases P10 and P11: the post-mortem debugger and the live debugger (SPEC §3.13). By default an evaluation still aborts as before and the debugger only shows the stack as it was when the abort started. In live mode (Ao.app) an evaluation runs on its own process, and `halt` and failures stop it so the Debugger can proceed, step, or abort it.

Phase P12: the System Browser removes methods and classes (SPEC §3.9 削除). A CompiledMethod on any class, and a class that is not a Kernel class, can be removed after a confirmation. There is no undo.

### Runtime

- When an evaluation aborts, the interpreted frames are copied before the stack unwinds, innermost first (at most 256, with the total). A failed native or `doesNotUnderstand:` send gets a synthesized innermost frame with its receiver and arguments. The copy allocates nothing on the Smalltalk heap and is not written to the image.
- Capture is off by default. `ao --test`, the CLI, the result of `ao_eval`, and the image format do not change.
- `Object>>halt` aborts the evaluation with the reason `halt`.
- The session's source table also keeps the doIt's source, block methods, and debug info.
- C ABI: `ao_set_debug_capture` and `ao_debug_*` read the snapshot (frame labels, the source with the failing send, temp names and values) and inspect a value. The reads work while the runtime is busy; printString, inspect, and clear are refused then.
- Live mode (`ao_set_debug_mode(AO_DEBUG_LIVE)`, off by default): each `ao_eval` runs its doIt on an evaluating process. `halt`, `error:` (the Kernel's failures with a message too), `doesNotUnderstand:`, a failed send, `NonBoolean receiver` and `cannot return` halt that process, and `ao_eval` answers `AO_ERR_HALT` (6) with the reason. Stack overflow, out of memory, deadlock and the process operations still abort. At most 8 processes are halted at once.
- A halted process's frames are read live through the same `ao_debug_*` reads (`ao_debug_select`). `ao_debug_proceed`, `ao_debug_step_into` / `_over` / `_out` and `ao_debug_abort` go on or end it; Proceed answers nil from the halted send. `NonBoolean receiver` and `cannot return` can only be aborted. `AO_EVAL_DEBUGIT` halts before the first instruction.
- A halted process is not running, so the runtime is not busy. `ao_image_save` refuses while a process is halted (`halted processes`); loading an image or shutting down abandons halted processes.
- C ABI: `ao_remove_method` takes a CompiledMethod out of a class's (or its metaclass's) method dictionary, invalidates the method cache for the selector and forgets its source; `ao_remove_class` takes a class's binding out of `Smalltalk` (an alias, the class object and its instances stay). A NativeMethod, an inherited selector, a fixed global, a Kernel class (also through an alias) and a class with a live subclass are refused with a reason. A removed global name is an undeclared identifier in the Workspace again. Both are refused while the runtime is busy. A halted process keeps running the method it was in.
- C ABI: the Browser names a class by a session class ID instead of its name. `ao_browser_class_id` answers the ID of the class bound to a name; `ao_browser_class_at` answers each row's ID; the reads, `ao_accept_method_id` (new), `ao_remove_method` and `ao_remove_class` take an ID. The class list has one row per class. An old class that only an alias keeps keeps its own row, and its removal is refused (`is not bound to this class`) instead of unbinding the new class of the same name. An unknown or stale ID is refused (`unknown class id`); IDs do not survive a boot or an image load.
- A method dictionary slot rewritten with `instVarAt:put:` no longer crashes a send: the class has no methods there, so the send goes to the superclass or `doesNotUnderstand:`.

### Compiler and interpreter

- The compiler emits a pc-to-source table and temp names for every method and block. The bytecode and the disassembly do not change.
- Interpreted frames are linked for the capture. Natives and the SmallInteger fast path are unchanged.
- The compiler also emits each method's and block's statement starts, for the step. The interpreter loop tests one step flag per instruction ([docs/bench.md](docs/bench.md): no measurable change).

### Ao.app

- A Debugger window with three panes: frames, the frame's source with the failing send selected, and its variables (`self`, arguments, temps) with their values. A double click opens an Inspector. A frame without source shows a placeholder.
- The Workspace shows a Debug button after a failed evaluation, and `process failed: <reason>` when a process fails although the evaluation answered.
- After the next evaluation, an open Debugger no longer prints or inspects values.
- The app runs in live mode. A halted evaluation opens a live Debugger with Proceed, Abort, Step over, Step into and Step out; the Workspace shows `halted: <reason>`. When Proceed or a step ends the evaluation, a Print it's result goes into the Workspace. Closing the live Debugger aborts the process.
- Smalltalk → Debug it (⌘⇧D) stops before the first statement.
- Save Image with a halted process fails with an alert that says `halted processes`.
- Right-click a class or a selector for Remove Class… / Remove Method…, also in the Smalltalk menu. A sheet asks first (`Remove Foo>>bar?`, `Remove class Foo?`; Remove / Cancel). A refusal shows its reason in the Browser's error field.
- The Browser keeps its rows and selection by class ID, so two classes with the same name are browsed, edited and removed apart. After Open Image it selects the class of the same name again.

### Project

- Release assets: `Ao-1.1.0-macos-arm64.zip`, `ao-cli-1.1.0-macos-arm64.tar.gz`, and `SHA256SUMS`.

### Known limitations

- An evaluation cannot be interrupted. A loop that never yields hangs the app, and only a force quit ends it.
- The live debugger has no Restart, no editing in the Debugger, and does not step into natives.
- A frame whose method has no source (re-accepted or removed while halted, loaded from an image, or filed in) has no statement starts, so Step over and Step into do not stop at its statements: they stop only in a deeper frame (Step into) or back in the sender (SPEC §3.13).
- No JIT, FFI, networking, or catching of Smalltalk exception objects (SPEC §1.4, §5).
- The bytecode interpreter is not optimized ([docs/bench.md](docs/bench.md)).
- The builds are ad-hoc signed and not notarized, so macOS asks before the first launch.

### Install

Requires an Apple Silicon Mac with macOS 14 or later. Download `Ao-1.1.0-macos-arm64.zip` (the app) and/or `ao-cli-1.1.0-macos-arm64.tar.gz` (the CLI), then check them with `shasum -a 256 -c SHA256SUMS`. The builds are not notarized: after the first launch attempt, choose **Open Anyway** in System Settings → Privacy & Security, or run `xattr -dr com.apple.quarantine Ao.app`. See the [README](https://github.com/SilentMalachite/Ao-Smalltalk/blob/v1.1.0/README.md) for usage.

## [1.0.0] - 2026-09-26

The first release. It completes phases P0–P9 of SPEC §2.3, the v1 definition: the Blue Book class hierarchy and message semantics run, every Kernel method is native code, and the macOS System Browser, Transcript, and Workspace accept, evaluate, and browse source.

### Runtime

- 64-bit tagged object pointers (`ao::Oop`) as direct pointers, with no object table.
- Exact generational GC: bump-allocated nursery, mark-compact old generation, weak slots, and immovable old objects. `AO_GC_STRESS=n` runs the collector every n-th allocation for testing.
- Blue Book metaclass bootstrap, the `Smalltalk` global dictionary, and interned symbols.
- Message send with an inline cache and a per-class method cache, `super`, `doesNotUnderstand:`, and `perform:`. A failure aborts the evaluation with a reason string.
- The Kernel classes of SPEC §3.6 as C++ `NativeMethod`s: objects and classes, `SmallInteger`, `LargePositiveInteger` / `LargeNegativeInteger`, `Float`, `Fraction`, the collections, streams, processes, `Point` / `Rectangle`, and the Transcript model.
- Cooperative processes on arm64 stacks with guard pages, with `Semaphore`, `SharedQueue`, `ensure:`, and `terminate`.

### Compiler and interpreter

- Smalltalk-80 scanner, parser, and AST, and Ao bytecode generation into `CompiledMethod`.
- Chunk-format file-in with method categories.
- Bytecode interpreter: method and block contexts, blocks that share outer temps, and non-local return. `to:do:` on a literal block compiles into jumps, and SmallInteger arithmetic and comparisons skip the send.

### Class library and images

- A pinned Cuis-Smalltalk excerpt (MIT) in `image/vendor`, filed in by `LOAD_ORDER`. Vendor methods may not override native-only selectors.
- `.aoimage` save and load. Loading checks the image and re-binds native methods by symbol name. A failed save leaves the old image in place.

### Ao.app

- Transcript, Workspace (Do it ⌘D, Print it ⌘I, Inspect it, workspace variables), and Inspector windows in AppKit.
- A five-pane System Browser: accept methods and class definitions, instance and class side, a hierarchy view, read-only native methods, and a question before an unaccepted edit is dropped.
- Compile errors select their span and leave the text unchanged. Spans stay right after Japanese text and emoji.
- File → Save Image… / Open Image…, a shared text size (⌘+, ⌘-, ⌘0), fixed pitch for the Transcript, and VoiceOver labels.
- The bundle carries the class library and files it in at launch.

### `ao` CLI

- `--version`, `filein`, `--test` (golden tests), `image save`, `image load`, and `extract-vendor`.

### Project

- GitHub Actions CI runs the whole suite, including the Kernel scan, on Apple Silicon macOS (SPEC §4.5).
- Release assets: `Ao-1.0.0-macos-arm64.zip`, `ao-cli-1.0.0-macos-arm64.tar.gz`, and `SHA256SUMS`.

### Known limitations

- An evaluation cannot be interrupted. A loop that never yields hangs the app, and only a force quit ends it.
- No debugger, JIT, FFI, networking, or catching of Smalltalk exception objects (SPEC §1.4, §5).
- The bytecode interpreter is not optimized ([docs/bench.md](docs/bench.md)).
- The builds are ad-hoc signed and not notarized, so macOS asks before the first launch.

[Unreleased]: https://github.com/SilentMalachite/Ao-Smalltalk/compare/v1.3.0...HEAD
[1.3.0]: https://github.com/SilentMalachite/Ao-Smalltalk/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/SilentMalachite/Ao-Smalltalk/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/SilentMalachite/Ao-Smalltalk/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/SilentMalachite/Ao-Smalltalk/releases/tag/v1.0.0

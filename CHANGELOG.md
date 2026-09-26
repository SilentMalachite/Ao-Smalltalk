# Changelog

All notable changes to Ao are recorded here. Versions follow [Semantic Versioning](https://semver.org/) (SPEC §2.4).

## [Unreleased]

Phases P10 and P11 of SPEC §2.3: the post-mortem debugger and the live debugger (SPEC §3.13). By default an evaluation still aborts as before and the debugger only shows the stack as it was when the abort started. In live mode (Ao.app) an evaluation runs on its own process, and `halt` and failures stop it so the Debugger can proceed, step, or abort it.

### Runtime

- When an evaluation aborts, the interpreted frames are copied before the stack unwinds, innermost first (at most 256, with the total). A failed native or `doesNotUnderstand:` send gets a synthesized innermost frame with its receiver and arguments. The copy allocates nothing on the Smalltalk heap and is not written to the image.
- Capture is off by default. `ao --test`, the CLI, the result of `ao_eval`, and the image format do not change.
- `Object>>halt` aborts the evaluation with the reason `halt`.
- The session's source table also keeps the doIt's source, block methods, and debug info.
- C ABI: `ao_set_debug_capture` and `ao_debug_*` read the snapshot (frame labels, the source with the failing send, temp names and values) and inspect a value. The reads work while the runtime is busy; printString, inspect, and clear are refused then.
- Live mode (`ao_set_debug_mode(AO_DEBUG_LIVE)`, off by default): each `ao_eval` runs its doIt on an evaluating process. `halt`, `error:` (the Kernel's failures with a message too), `doesNotUnderstand:`, a failed send, `NonBoolean receiver` and `cannot return` halt that process, and `ao_eval` answers `AO_ERR_HALT` (6) with the reason. Stack overflow, out of memory, deadlock and the process operations still abort. At most 8 processes are halted at once.
- A halted process's frames are read live through the same `ao_debug_*` reads (`ao_debug_select`). `ao_debug_proceed`, `ao_debug_step_into` / `_over` / `_out` and `ao_debug_abort` go on or end it; Proceed answers nil from the halted send. `NonBoolean receiver` and `cannot return` can only be aborted. `AO_EVAL_DEBUGIT` halts before the first instruction.
- A halted process is not running, so the runtime is not busy. `ao_image_save` refuses while a process is halted (`halted processes`); loading an image or shutting down abandons halted processes.

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

### Known limitations

- An evaluation cannot be interrupted. A loop that never yields hangs the app, and only a force quit ends it.
- The live debugger has no Restart, no editing in the Debugger, and does not step into natives.
- No JIT, FFI, networking, or catching of Smalltalk exception objects (SPEC §1.4, §5).
- The bytecode interpreter is not optimized ([docs/bench.md](docs/bench.md)).
- The builds are ad-hoc signed and not notarized, so macOS asks before the first launch.

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

[Unreleased]: https://github.com/SilentMalachite/Ao-Smalltalk/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/SilentMalachite/Ao-Smalltalk/releases/tag/v1.0.0

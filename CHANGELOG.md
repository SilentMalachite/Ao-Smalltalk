# Changelog

All notable changes to Ao are recorded here. Versions follow [Semantic Versioning](https://semver.org/) (SPEC §2.4).

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

[1.0.0]: https://github.com/SilentMalachite/Ao-Smalltalk/releases/tag/v1.0.0

# Ao

Ao is a **new** Smalltalk system for Apple Silicon Macs. The language, object model, and tool contracts follow Smalltalk-80 (the Blue Book). The VM is not a port of Squeak, Pharo, OpenSmalltalk, or any other existing Smalltalk VM.

日本語: [README.ja.md](README.ja.md)

Product spec: [SPEC.md](SPEC.md) (Japanese; product authority). Agent process: [CLAUDE.md](CLAUDE.md). Contributing: [CONTRIBUTING.md](CONTRIBUTING.md). Changes: [CHANGELOG.md](CHANGELOG.md).

## Status

**Ao 1.1.0** is the current release. It completes phases P0–P12 of [SPEC.md](SPEC.md) §2.3. Phases P0–P9 are the v1 definition, released as 1.0.0; P10–P12 add a debugger and removal in the Browser:

| Area | What Ao does |
|---|---|
| Object memory | 64-bit tagged `Oop`, nursery + old-generation exact GC |
| Classes | Blue Book metaclass cycle (`Object class class == Metaclass`, `Metaclass class class == Metaclass`) |
| Kernel | Every Kernel method (SPEC §3.6) is a C++ `NativeMethod`. A test walks the Kernel method dictionaries and fails on anything else |
| Compiler | Smalltalk-80 source → AST → `CompiledMethod` bytecode. Chunk-format file-in |
| Interpreter | User methods, blocks with shared temps, non-local return, `ensure:`, cooperative processes, `Semaphore`, `SharedQueue` |
| Class library | Non-Kernel classes filed in from a pinned Cuis-Smalltalk excerpt (`image/vendor`) |
| Images | `.aoimage` save and load. Native methods are re-bound by symbol name on load |
| Tools | AppKit Transcript, Workspace (Do it / Print it / Inspect it / Debug it), and a five-pane System Browser with Accept and Remove |
| Debugger | `halt` and failures stop the evaluation in a Debugger window with Proceed, Abort, Step over, Step into, and Step out (P10, P11) |

## Install

Requirements: an Apple Silicon Mac with macOS 14 or later. Intel Macs are not supported.

Download from the [Releases](https://github.com/SilentMalachite/Ao-Smalltalk/releases) page:

| File | Contents |
|---|---|
| `Ao-1.1.0-macos-arm64.zip` | `Ao.app`, `LICENSE`, `NOTICE` |
| `ao-cli-1.1.0-macos-arm64.tar.gz` | The `ao` command-line tool, the class library it files in (`vendor/`), `LICENSE`, `NOTICE` |
| `SHA256SUMS` | SHA-256 checksums of the two archives |

Check the downloads:

```sh
shasum -a 256 -c SHA256SUMS
```

The builds are ad-hoc signed, not notarized. macOS blocks them the first time. Either open **System Settings → Privacy & Security** and choose **Open Anyway** after the first launch attempt, or clear the quarantine flag yourself:

```sh
xattr -dr com.apple.quarantine Ao.app
xattr -d com.apple.quarantine ao-cli-1.1.0-macos-arm64/ao
```

## Using Ao.app

On launch Ao opens a **Transcript** and a **Workspace**, files in the bundled class library, and writes `vendor loaded: …` to the Transcript.

In the Workspace, select source and use the **Smalltalk** menu:

| Command | Key | Effect |
|---|---|---|
| Do it | ⌘D | Evaluate and discard the result |
| Print it | ⌘I | Evaluate and insert the result's `printString` after the selection |
| Inspect it | — | Evaluate and show the result in an inspector window |
| Debug it | ⌘⇧D | Evaluate in the Debugger, stopped before the first statement |

```smalltalk
3 + 4.
#(1 2 3) collect: [:x | x * 2].
Transcript show: 'hello'; cr.
Object class class == Metaclass.
```

A compile error selects the span it points at and leaves your text alone. Undefined names in the Workspace become workspace variables.

**Tools → Browser** opens the System Browser: class categories, classes (instance or class side), protocols, and selectors, above a source pane. Edit a method or a class definition and choose **Smalltalk → Accept**. The next Do it sees the change. **Smalltalk → Show Hierarchy** replaces the class list with the selected class's hierarchy; choose it again to go back. Kernel methods are native and have no Smalltalk source.

To remove a method or a class, right-click it in the Browser, or choose **Smalltalk → Remove Method…** or **Remove Class…**, and confirm. There is no undo. Native methods, Kernel classes, and classes that still have subclasses are refused, and the Browser shows why. Removing a class takes its name out of `Smalltalk`; its instances keep working.

### Debugger

When an evaluation calls `halt` or fails (an error, `doesNotUnderstand:`, a failed send), it stops and a **Debugger** window opens, and the Workspace shows `halted: <reason>`. The Debugger lists the frames, shows the selected frame's source with the current send selected, and lists `self`, the arguments, and the temps with their values; double-click one to inspect it.

| Button | Effect |
|---|---|
| Proceed | Go on; the stopped send answers `nil` |
| Step over | Stop at the next statement of this method, or back in its sender |
| Step into | Also stop at the first instruction of a method or block it sends to |
| Step out | Stop back in the sender |
| Abort | End the evaluation (its `ensure:` blocks run) |

When Proceed or a step finishes a Print it, the result goes into the Workspace. Closing the Debugger aborts the evaluation. Stack overflow, out of memory, and a deadlock still abort without stopping; the Workspace then shows a **Debug** button that opens the stack as it was when the abort started. Save Image is refused while an evaluation is stopped.

**File → Save Image…** writes a `.aoimage` with your classes and methods. **File → Open Image…** loads one back.

**Tools** also holds **Use Fixed Pitch** and the text size commands (**Make Text Bigger** ⌘+, **Make Text Smaller** ⌘-, **Actual Size** ⌘0), shared by the Transcript, the Workspace, and the Browser source pane.

Evaluation runs on the main thread and cannot be interrupted. A loop that never yields, such as `[true] whileTrue`, hangs the app. Force quit is the only way out, and unsaved work is lost.

## Using the `ao` CLI

```sh
ao --version                               # 1.1.0
ao filein hello.st                         # file in one chunk-format file
ao filein --load-order vendor/LOAD_ORDER   # file in the class library in order
ao --test tests/                           # run every *.st in tests/ as a golden test
ao image save --load-order vendor/LOAD_ORDER my.aoimage
ao image load my.aoimage                   # load an image and check it
```

The commands are quiet on success and exit 0. Errors go to stderr and the exit code is non-zero. A golden test file is Smalltalk source evaluated once, for example:

```smalltalk
self assert: 3 + 4 equals: 7.
self assert: (#(1 2 3) collect: [:x | x * 2]) equals: #(2 4 6).
```

See SPEC §4.4 for the full `ao --test` contract.

## Not in Ao

No JIT (user methods run as bytecode), no FFI, no networking, and no catching of Smalltalk exception objects. The Debugger has no Restart, no editing, and does not step into native methods. Ao does not load Squeak, Pharo, or Xerox `.image` files. It runs only on Apple Silicon macOS. SPEC §1.4 lists the non-goals.

## Build from source

Requirements: Apple Silicon macOS 14+, CMake 3.28+, Ninja, Apple Clang with C++20, Swift 6. The first build downloads GoogleTest.

```sh
./scripts/test.sh             # build/, CTest, swift test
./scripts/test.sh --asan      # the same under AddressSanitizer (build-asan/)
./scripts/package-app.sh      # Release build and build/Ao.app (ad-hoc signed)
./scripts/test.sh --app       # also package Ao.app, launch it, and check it loads the class library
```

`--app` opens a window and takes focus, so it is not part of the default run.

Equivalent manual steps:

```sh
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
ctest --test-dir build --output-on-failure
swift test --package-path app \
  -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a \
  -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a \
  -Xlinker -lc++
```

CI runs `./scripts/test.sh` on every push and pull request to `main` (SPEC §4.5).

## Layout

```
runtime/       C++20 object memory, GC, dispatch, Kernel natives, interpreter, images, ao CLI
compiler/      C++20 scanner, parser, bytecode code generation (no heap)
bridge/        C ABI between runtime and app (ao_abi.h)
app/           Swift 6 + AppKit: Transcript, Workspace, System Browser, Inspector, Debugger
image/Kernel/  Kernel protocol comments (the definitions are C++)
image/vendor/  Pinned Cuis-Smalltalk class library excerpt and its load order
image/tests/   Golden Smalltalk tests for ao --test
scripts/       Build, test, and packaging scripts
docs/          Phase, PR, review, and benchmark notes
graphify-out/  Knowledge graph (committed)
```

## Documentation

| File | Role |
|---|---|
| [SPEC.md](SPEC.md) | Product specification |
| [CHANGELOG.md](CHANGELOG.md) | Release notes |
| [CLAUDE.md](CLAUDE.md) | How agents implement (Graphify + Serena) |
| [docs/README.md](docs/README.md) | Phase / PR index |
| [docs/native-selectors.md](docs/native-selectors.md) | Native selector mangling |
| [docs/bench.md](docs/bench.md) | Native and interpreter microbenchmarks |
| [image/vendor/ORIGIN.md](image/vendor/ORIGIN.md) | Class library origin and pin |

English is the canonical language for GitHub-facing docs in this repository (`README.md`, `CONTRIBUTING.md`, `CHANGELOG.md`, `LICENSE`). Japanese counterparts are translations.

## License

Copyright 2026 Ao contributors.

Licensed under the **Apache License, Version 2.0**. The legal text is English: [LICENSE](LICENSE). See also [NOTICE](NOTICE).

The class library in `image/vendor/cuis` comes from [Cuis-Smalltalk](https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev) and keeps its MIT license ([image/vendor/cuis/LICENSE](image/vendor/cuis/LICENSE)).

# Ao

Ao is a **new** Smalltalk system for Apple Silicon Macs. The language, object model, and tool contracts follow Smalltalk-80 (the Blue Book). The VM is not a port of Squeak, Pharo, OpenSmalltalk, or any other existing Smalltalk VM.

日本語: [README.ja.md](README.ja.md)

Product spec: [SPEC.md](SPEC.md) (Japanese; product authority). Agent process: [CLAUDE.md](CLAUDE.md). Contributing: [CONTRIBUTING.md](CONTRIBUTING.md).

## Status

The current phase is in the root [`PHASE`](PHASE) file (`P6`). Phases P0–P5 are done:

| Done | What you can run today |
|---|---|
| Object memory | 64-bit tagged `Oop`, nursery + old GC |
| Bootstrap | Blue Book 6–10 metaclass cycle, Kernel class catalog |
| Dispatch | `NativeMethod`, lookup, `send` |
| Kernel | SPEC §3.6 classes as C++ natives (not bytecode) |
| Compiler | Smalltalk-80 source → AST → `CompiledMethod` (bytecode). Chunk file-in |

**Not in this tree yet:** bytecode interpreter (P6), vendor class-library file-in (P6b), `.aoimage` save/load (P7), AppKit Browser / Transcript / Workspace (P8–P9).

User methods compile to heap `CompiledMethod` objects. They are not executed until P6. Kernel methods stay C++ `NativeMethod`.

## Requirements

- Apple Silicon macOS
- CMake 3.28+, Ninja, a C++20 compiler (Apple Clang)
- Swift 6 (AppKit smoke tests)

Intel Macs are out of scope.

## Build and test

```sh
./scripts/test.sh
```

That configures `build/`, builds the runtime and compiler, runs CTest, and runs `swift test --package-path app`.

Equivalent steps:

```sh
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
ctest --test-dir build --output-on-failure
swift test --package-path app
```

## Layout

```
runtime/     C++20 object memory, Kernel natives, dispatch
compiler/    C++20 scanner, parser, bytecode codegen (no heap)
image/       Kernel protocol comments; vendor .st later
app/         Swift 6 + AppKit (skeleton)
bridge/      C ABI only (`ao_abi.h`)
docs/        Phase and PR notes
graphify-out/  Knowledge graph (committed)
```

## Documentation

| File | Role |
|---|---|
| [SPEC.md](SPEC.md) | Product specification |
| [CLAUDE.md](CLAUDE.md) | How agents implement (Graphify + Serena) |
| [docs/README.md](docs/README.md) | Phase / PR index |
| [docs/native-selectors.md](docs/native-selectors.md) | Native selector mangling |
| [docs/bench.md](docs/bench.md) | P4 native microbench |

English is the canonical language for GitHub-facing docs in this repository (`README.md`, `CONTRIBUTING.md`, `LICENSE`). Japanese counterparts are translations.

## License

Copyright 2026 Ao contributors.

Licensed under the **Apache License, Version 2.0**. The legal text is English: [LICENSE](LICENSE). See also [NOTICE](NOTICE).

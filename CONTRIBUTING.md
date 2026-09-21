# Contributing

Japanese: [CONTRIBUTING.ja.md](CONTRIBUTING.ja.md)

The product spec is [SPEC.md](SPEC.md). The agent process is [CLAUDE.md](CLAUDE.md). Chat is not the spec.

## Current phase

Read the one-line root file [`PHASE`](PHASE). Implement only that phase. Do not start later phases (interpreter UI, vendor file-in, JIT) early.

## Rules that do not bend

- Kernel classes and methods are C++ `NativeMethod`. `image/Kernel/*.st` is protocol comments, not executable definitions.
- Do not copy VM source from Squeak, Pharo, OpenSmalltalk, or other Smalltalk VMs.
- Do not add features that are not in SPEC.md for the current phase.
- Public pointers are `ao::Oop`. Do not throw C++ exceptions at API boundaries.
- The Swift app talks to the runtime only through `bridge/ao_abi.h`.

## Build

Apple Silicon macOS, CMake 3.28+, Ninja, C++20, Swift 6.

```sh
./scripts/test.sh
```

Do not leave a red suite.

## Commits

English imperative subjects, for example `Add tagged OOP tests`. When an agent session used Graphify or Serena, add a one-line trailer as in CLAUDE.md.

## License

Contributions are under the Apache License, Version 2.0. See [LICENSE](LICENSE) and [NOTICE](NOTICE).

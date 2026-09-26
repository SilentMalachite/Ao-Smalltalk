# Contributing

Japanese: [CONTRIBUTING.ja.md](CONTRIBUTING.ja.md)

The product spec is [SPEC.md](SPEC.md). The agent process is [CLAUDE.md](CLAUDE.md). Chat is not the spec.

## Where the project stands

v1 (phase P9 in SPEC §2.3) is released as 1.0.0. The root [`PHASE`](PHASE) file stays `P9`. A change after v1 starts in SPEC.md: update the spec first (SPEC §8), then write the tests, then the code. Items in SPEC §1.4 (JIT, FFI, debugger, and the rest) stay out unless the spec takes them in.

## Rules that do not bend

- Kernel classes and methods are C++ `NativeMethod`. `image/Kernel/*.st` is protocol comments, not executable definitions.
- Do not copy VM source from Squeak, Pharo, OpenSmalltalk, or other Smalltalk VMs.
- Do not add features that are not in SPEC.md.
- Public pointers are `ao::Oop`. Do not throw C++ exceptions at API boundaries.
- The Swift app talks to the runtime only through `bridge/ao_abi.h`.

## Build

Apple Silicon macOS 14+, CMake 3.28+, Ninja, C++20, Swift 6.

```sh
./scripts/test.sh
```

Do not leave a red suite. CI runs the same script on every push and pull request to `main`, and `main` accepts only changes whose CI is green (SPEC §4.5). That run includes the Kernel scan test, which fails if any Kernel method is not a `NativeMethod`.

## Commits

English imperative subjects, for example `Add tagged OOP tests`. When an agent session used Graphify or Serena, add a one-line trailer as in CLAUDE.md.

## Releasing

SPEC §2.4 defines the version, the assets, and when a tag may be made. With `V` set to the new version:

1. On a branch, set `ao::version_string` (`runtime/src/Runtime.cpp`) and `ao::compiler::version` (`compiler/src/Compiler.cpp`) to `V`, with their tests, and add a `CHANGELOG.md` entry. Merge after CI is green.
2. On `main`, check that every SPEC §6 item is `[x]`, then build and check the app:

   ```sh
   ./scripts/test.sh --app
   ```

3. Stage the assets in `build/dist`:

   ```sh
   V=1.0.0
   D=build/dist
   rm -rf "$D" && mkdir -p "$D/Ao-$V-macos-arm64" "$D/ao-cli-$V-macos-arm64"
   ditto build/Ao.app "$D/Ao-$V-macos-arm64/Ao.app"
   cp LICENSE NOTICE "$D/Ao-$V-macos-arm64/"
   cp build-release/ao LICENSE NOTICE "$D/ao-cli-$V-macos-arm64/"
   cp -R image/vendor "$D/ao-cli-$V-macos-arm64/vendor"
   find "$D" -name .DS_Store -exec rm -f {} +
   (cd "$D" &&
     ditto -c -k --keepParent "Ao-$V-macos-arm64" "Ao-$V-macos-arm64.zip" &&
     tar -czf "ao-cli-$V-macos-arm64.tar.gz" "ao-cli-$V-macos-arm64" &&
     shasum -a 256 "Ao-$V-macos-arm64.zip" "ao-cli-$V-macos-arm64.tar.gz" > SHA256SUMS)
   ```

4. Tag and publish, with the `CHANGELOG.md` entry as the notes:

   ```sh
   git tag -a "v$V" -m "Ao $V"
   git push origin "v$V"
   gh release create "v$V" --title "Ao $V" --notes-file <notes> \
     "$D/Ao-$V-macos-arm64.zip" "$D/ao-cli-$V-macos-arm64.tar.gz" "$D/SHA256SUMS"
   ```

## License

Contributions are under the Apache License, Version 2.0. See [LICENSE](LICENSE) and [NOTICE](NOTICE).

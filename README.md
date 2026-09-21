# Ao

macOS native Smalltalk. Product spec: [SPEC.md](SPEC.md). Agent process: [CLAUDE.md](CLAUDE.md).

## Build

    cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build
    ctest --test-dir build --output-on-failure
    swift test --package-path app

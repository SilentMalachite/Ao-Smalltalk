#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
"$ROOT/scripts/build.sh"
ctest --test-dir "$ROOT/build" --output-on-failure
swift test --package-path "$ROOT/app" \
  -Xlinker -force_load -Xlinker "$ROOT/build/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$ROOT/build/compiler/libao_compiler.a" \
  -Xlinker -lc++

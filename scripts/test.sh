#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
usage() {
  echo "usage: $0 [--asan]" >&2
  exit 2
}
ASAN=0
for arg in "$@"; do
  case "$arg" in
    --asan) ASAN=1 ;;
    *) usage ;;
  esac
done
if [ "$ASAN" -eq 1 ]; then
  BUILD="$ROOT/build-asan"
  "$ROOT/scripts/build.sh" --asan
  echo "note: KernelBench timing tests are skipped under ASan" >&2
  ctest --test-dir "$BUILD" --output-on-failure -E '^KernelBench\.'
  set -- --scratch-path "$BUILD/swift" --sanitize=address --sanitize=undefined
else
  BUILD="$ROOT/build"
  "$ROOT/scripts/build.sh"
  ctest --test-dir "$BUILD" --output-on-failure
  set --
fi
swift test --package-path "$ROOT/app" "$@" \
  -Xlinker -force_load -Xlinker "$BUILD/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$BUILD/compiler/libao_compiler.a" \
  -Xlinker -lc++

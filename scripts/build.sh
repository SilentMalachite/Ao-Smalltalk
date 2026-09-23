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
  set -- -DAO_SANITIZE=ON
else
  BUILD="$ROOT/build"
  set --
fi
cmake -S "$ROOT" -B "$BUILD" -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "$@"
cmake --build "$BUILD"
if [ "$ASAN" -eq 0 ]; then
  cp "$BUILD/compile_commands.json" "$ROOT/compile_commands.json"
fi

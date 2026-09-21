#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cmake -S "$ROOT" -B "$ROOT/build" -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build "$ROOT/build"
cp "$ROOT/build/compile_commands.json" "$ROOT/compile_commands.json"

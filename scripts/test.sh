#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
"$ROOT/scripts/build.sh"
ctest --test-dir "$ROOT/build" --output-on-failure

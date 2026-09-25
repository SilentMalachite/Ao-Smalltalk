#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
usage() {
  echo "usage: $0 [--asan | --app]" >&2
  exit 2
}
ASAN=0
APP=0
for arg in "$@"; do
  case "$arg" in
    --asan) ASAN=1 ;;
    --app) APP=1 ;;
    *) usage ;;
  esac
done
if [ "$ASAN" -eq 1 ] && [ "$APP" -eq 1 ]; then
  usage
fi
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
if [ "$APP" -eq 0 ]; then
  exit 0
fi

# --app: package build/Ao.app, launch it through LaunchServices (so the
# shell environment, AO_VENDOR_DIR included, is not passed) and wait for
# the vendor line on its stderr.
"$ROOT/scripts/package-app.sh"
BUNDLE="$ROOT/build/Ao.app"
EXE="$BUNDLE/Contents/MacOS/Ao"
codesign --verify --strict "$BUNDLE"
WANT="ao: vendor loaded: $(cd "$BUNDLE" && pwd -P)/Contents/Resources/vendor"
LOG="$(mktemp "${TMPDIR:-/tmp}/ao-app-stderr.XXXXXX")"
LAUNCHED=0
cleanup() {
  if [ "$LAUNCHED" -eq 1 ]; then
    pkill -n -f "$EXE" || true
  fi
  rm -f "$LOG"
}
trap cleanup EXIT
trap 'exit 1' HUP INT TERM
LAUNCHED=1
open -n -g --stderr "$LOG" "$BUNDLE"
i=0
until grep -qxF "$WANT" "$LOG"; do
  if [ "$i" -ge 30 ]; then
    echo "app: FAILED: no line '$WANT' on stderr within 30 s" >&2
    echo "app: stderr of $EXE was:" >&2
    if [ -s "$LOG" ]; then
      sed 's/^/  | /' "$LOG" >&2
    else
      echo "  (empty)" >&2
    fi
    exit 1
  fi
  sleep 1
  i=$((i + 1))
done
echo "app: ok: $WANT"

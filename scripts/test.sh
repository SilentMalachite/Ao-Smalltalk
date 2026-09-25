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

# --app: package build/Ao.app, launch it with open and wait for the vendor
# line on its stderr. An app opened with open inherits the shell's
# environment, so --env AO_VENDOR_DIR= empties the variable; the app ignores
# an empty one and loads the bundle's Contents/Resources/vendor.
BUNDLE="$ROOT/build/Ao.app"
EXE="$(cd "$ROOT" && pwd -P)/build/Ao.app/Contents/MacOS/Ao"
# PIDs of the processes whose command line starts with the path $EXE
# (compared as a string, not a pattern).
app_pids() {
  ps -axo pid=,args= | while read -r pid args; do
    case "$args" in
      "$EXE" | "$EXE "*) echo "$pid" ;;
    esac
  done
}
# package-app.sh deletes and rebuilds the bundle; never touch a running one.
if [ -n "$(app_pids)" ]; then
  echo "app: build/Ao.app is already running; quit it first" >&2
  exit 1
fi
"$ROOT/scripts/package-app.sh"
codesign --verify --strict "$BUNDLE"
WANT="ao: vendor loaded: $(cd "$BUNDLE" && pwd -P)/Contents/Resources/vendor"
LOG="$(mktemp "${TMPDIR:-/tmp}/ao-app-stderr.XXXXXX")"
PID=
cleanup() {
  # Stop only the process open started, and only while it still runs $EXE.
  if [ -n "$PID" ] && app_pids | grep -qxF "$PID"; then
    kill "$PID" 2>/dev/null || true
    j=0
    while [ "$j" -lt 5 ] && kill -0 "$PID" 2>/dev/null; do
      sleep 1
      j=$((j + 1))
    done
  fi
  rm -f "$LOG"
}
trap cleanup EXIT
trap 'exit 1' HUP INT TERM
open -n -g --env AO_VENDOR_DIR= --stderr "$LOG" "$BUNDLE"
# Nothing ran from build/Ao.app before the packaging, so the one process that
# runs it now is the one open started. With none, or more than one, stop none.
i=0
while [ "$i" -lt 10 ]; do
  pids="$(app_pids)"
  if [ -n "$pids" ]; then
    if [ "$(printf '%s\n' "$pids" | grep -c .)" -eq 1 ]; then
      PID="$pids"
    else
      echo "app: warning: several processes run $EXE; stopping none" >&2
    fi
    break
  fi
  sleep 1
  i=$((i + 1))
done
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

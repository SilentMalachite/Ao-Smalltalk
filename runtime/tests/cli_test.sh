#!/bin/sh
# ao の CLI を実プロセスで起動し、終了コードと stderr の行を確かめる。
#   SPEC §3.12: ao filein <file.st>、ao filein --load-order、ao image save --load-order
#   SPEC §4.4:  ao --test <dir>
# usage: cli_test.sh <ao> <work-dir> filein|image_save|test
set -u
AO="$1"
WORK="$2"
CASE="$3"

rm -rf "$WORK"
mkdir -p "$WORK"
cd "$WORK" || exit 1

fail() {
  echo "FAIL: $*" >&2
  exit 1
}

# run <expected exit> <command...>: stderr goes to err.txt.
run() {
  expected="$1"
  shift
  "$@" 2>err.txt
  status=$?
  if [ "$status" -ne "$expected" ]; then
    cat err.txt >&2
    fail "exit $status (expected $expected): $*"
  fi
}

# stderr_is <expected text>: err.txt is exactly that (lines joined with newlines).
stderr_is() {
  printf '%s' "$1" >expected.txt
  if [ -n "$1" ]; then
    printf '\n' >>expected.txt
  fi
  if ! cmp -s expected.txt err.txt; then
    echo "--- expected stderr" >&2
    cat expected.txt >&2
    echo "--- actual stderr" >&2
    cat err.txt >&2
    fail "stderr differs"
  fi
}

write_fixtures() {
  cat >good.st <<'EOF'
!Object subclass: #CliGood
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!

!CliGood methodsFor: 'probes'!
three
  ^3! !
EOF
  cat >bad.st <<'EOF'
!Object subclass: #CliBad
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!

!CliBad methodsFor: 'probes'!
bad
  ^1 +! !
EOF
  cat >kern.st <<'EOF'
!Object subclass: #Object
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Kernel'!
EOF
  mkdir -p plain deferred missing kernel
  cp good.st bad.st plain/
  cp good.st bad.st deferred/
  printf 'good.st\nbad.st\n' >plain/LOAD_ORDER
  printf 'good.st\nbad.st\n' >deferred/LOAD_ORDER
  printf '# Deferred\nCliBad>>bad: kept out on purpose\n' >deferred/DEFERRED.md
  cp good.st missing/
  printf 'good.st\nnone.st\n' >missing/LOAD_ORDER
  cp kern.st good.st kernel/
  printf 'kern.st\ngood.st\n' >kernel/LOAD_ORDER
  # SPEC §3.12 (P17): second/ subclasses CliGood, which only first/ defines.
  mkdir -p first second
  cp good.st first/
  printf 'good.st\n' >first/LOAD_ORDER
  cat >second/sub.st <<'EOF'
!CliGood subclass: #CliSub
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!

!CliSub methodsFor: 'probes'!
four
  ^self three + 1! !
EOF
  printf 'sub.st\n' >second/LOAD_ORDER
}

# bad.st の `^1 +` は `+`（ファイル本文の 166-167 バイト目）の後に式が無い。
BAD_LINE='bad.st:166-167: expected expression'
# kern.st のクラス定義のチャンクは、先頭の ! の後（1 バイト目）から閉じる ! の手前（122）まで。
KERN_LINE='kern.st:1-122: refusing to redefine kernel class: Object'

case "$CASE" in
  filein)
    write_fixtures
    run 0 "$AO" filein good.st
    stderr_is ''
    run 1 "$AO" filein bad.st
    stderr_is "$BAD_LINE"
    run 1 "$AO" filein none.st
    stderr_is 'none.st:0-0: cannot read: none.st'
    run 1 "$AO" filein --load-order plain/LOAD_ORDER
    stderr_is "plain/$BAD_LINE"
    run 0 "$AO" filein --load-order deferred/LOAD_ORDER
    stderr_is ''
    run 1 "$AO" filein --load-order missing/LOAD_ORDER
    stderr_is 'missing/none.st:0-0: cannot read: missing/none.st'
    run 1 "$AO" filein --load-order no-such/LOAD_ORDER
    stderr_is 'no-such/LOAD_ORDER:0-0: cannot read: no-such/LOAD_ORDER'
    run 1 "$AO" filein kern.st
    stderr_is "$KERN_LINE"
    run 1 "$AO" filein --load-order kernel/LOAD_ORDER
    stderr_is "kernel/$KERN_LINE"
    # SPEC §3.12 (P17): --load-order repeats; the files are read in order, so the second can use
    # what the first defined (CliSub's superclass CliGood), and the second alone cannot.
    run 0 "$AO" filein --load-order first/LOAD_ORDER --load-order second/LOAD_ORDER
    stderr_is ''
    run 1 "$AO" filein --load-order second/LOAD_ORDER
    run 1 "$AO" filein --load-order second/LOAD_ORDER --load-order first/LOAD_ORDER
    # The second is read too (its error shows), and a failed one stops the rest.
    run 1 "$AO" filein --load-order first/LOAD_ORDER --load-order plain/LOAD_ORDER
    stderr_is "plain/$BAD_LINE"
    run 1 "$AO" filein --load-order missing/LOAD_ORDER --load-order plain/LOAD_ORDER
    stderr_is 'missing/none.st:0-0: cannot read: missing/none.st'
    run 2 "$AO" filein --load-order first/LOAD_ORDER --load-order
    run 2 "$AO" filein --load-order first/LOAD_ORDER second/LOAD_ORDER
    ;;
  image_save)
    write_fixtures
    run 1 "$AO" image save --load-order plain/LOAD_ORDER plain.aoimage
    stderr_is "plain/$BAD_LINE"
    [ ! -e plain.aoimage ] || fail "image written after a failed file-in"
    run 1 "$AO" image save --load-order missing/LOAD_ORDER missing.aoimage
    stderr_is 'missing/none.st:0-0: cannot read: missing/none.st'
    [ ! -e missing.aoimage ] || fail "image written after a failed file-in"
    run 0 "$AO" image save --load-order deferred/LOAD_ORDER deferred.aoimage
    stderr_is ''
    [ -s deferred.aoimage ] || fail "image not written"
    # SPEC §3.12 (P17): --load-order repeats, in order; a failure in any writes no image.
    run 0 "$AO" image save --load-order first/LOAD_ORDER --load-order second/LOAD_ORDER two.aoimage
    stderr_is ''
    [ -s two.aoimage ] || fail "image not written for two load orders"
    run 0 "$AO" image load two.aoimage
    run 1 "$AO" image save --load-order first/LOAD_ORDER --load-order plain/LOAD_ORDER three.aoimage
    stderr_is "plain/$BAD_LINE"
    [ ! -e three.aoimage ] || fail "image written after a failed second file-in"
    run 0 "$AO" image load deferred.aoimage
    stderr_is ''
    # SPEC §3.11: a version 1 image (before B4) is refused at the header, with its reason.
    cp deferred.aoimage v1.aoimage
    printf '\001' | dd of=v1.aoimage bs=1 seek=4 conv=notrunc 2>/dev/null
    run 1 "$AO" image load v1.aoimage
    stderr_is 'ao: image load failed: unsupported image version 1'
    # SPEC §3.11: version 2 (before the hashed Dictionary and Set) is refused the same way.
    cp deferred.aoimage v2.aoimage
    printf '\002' | dd of=v2.aoimage bs=1 seek=4 conv=notrunc 2>/dev/null
    run 1 "$AO" image load v2.aoimage
    stderr_is 'ao: image load failed: unsupported image version 2'
    printf 'not an image' >garbage.aoimage
    run 1 "$AO" image load garbage.aoimage
    stderr_is 'ao: image load failed: not an Ao image'
    run 1 "$AO" image load none.aoimage
    stderr_is 'ao: image load failed: cannot read image file'
    ;;
  test)
    mkdir -p tests empty pass
    printf 'nil foo.\nself assert: 1 equals: 1.\n' >tests/a_dnu.st
    printf '1/0.\nself assert: 1 equals: 1.\n' >tests/b_zero.st
    printf 'self assert: 1 + equals: 4.\n' >tests/c_compile.st
    printf 'self assert: 1 + 2 equals: 3.\n' >tests/d_pass.st
    # SPEC §4.4: a process that fails fails its file, one line with the last reason.
    printf '[nil foo] fork.\n[nil bar] fork.\nself assert: 1 equals: 1.\n' >tests/e_fork.st
    printf 'self assert: 1 + 2 equals: 3.\n' >pass/pass.st
    # SPEC §4.4: processes left after the drain are terminated, not failures; one whose cleanup
    # blocks is abandoned.
    printf '| s |\ns := Semaphore new.\n[[s wait] ensure: [s wait]] fork.\n[s wait] fork.\n' >pass/waiters.st
    printf 'not a test\n' >empty/notes.txt
    run 1 "$AO" --test tests
    stderr_is 'ao --test: tests/a_dnu.st: doesNotUnderstand: #foo
ao --test: tests/b_zero.st: division by zero
ao --test: tests/c_compile.st:17-24: expected expression
ao --test: tests/e_fork.st: process failed: doesNotUnderstand: #bar'
    run 0 "$AO" --test pass
    stderr_is ''
    run 1 "$AO" --test empty
    stderr_is 'ao --test: empty: no .st files'
    run 1 "$AO" --test no-such-dir
    stderr_is 'ao --test: no-such-dir: cannot read directory'
    ;;
  *)
    fail "unknown case: $CASE"
    ;;
esac
echo "ok: $CASE"

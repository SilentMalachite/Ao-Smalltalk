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
  # SPEC §3.11 保存: defining CliM8Sub runs CliM8's class-side subclass:..., which then makes a
  # `Behavior new` class no one holds, breaks its methodDict and sends once to its instance. When
  # the file-in ends, only the method cache reaches that class.
  mkdir -p m8
  cat >m8/m8.st <<'EOF'
!Object subclass: #CliM8
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!

!CliM8 class methodsFor: 'definition'!
subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p category: k
  | made b |
  made := super subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p category: k.
  b := Behavior new.
  b instVarAt: 1 put: Object; instVarAt: 2 put: 7.
  b basicNew yourself.
  ^made! !

!CliM8 subclass: #CliM8Sub
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!
EOF
  printf 'm8.st\n' >m8/LOAD_ORDER
  # SPEC §3.11 ロード: defining CliInmSub runs CliInm's class-side subclass:..., which gives
  # UndefinedObject (slot 2 of a class = its method dictionary) an empty MethodDictionary. The saved
  # image then lacks `nil isNil`, which only the natives ao image load supplies after loading.
  mkdir -p inm
  cat >inm/inm.st <<'EOF'
!Object subclass: #CliInm
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!

!CliInm class methodsFor: 'definition'!
subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p category: k
  | made d |
  made := super subclass: n instanceVariableNames: i classVariableNames: c poolDictionaries: p category: k.
  d := MethodDictionary new.
  d instVarAt: 1 put: 0; instVarAt: 2 put: (Array new: 16).
  UndefinedObject instVarAt: 2 put: d.
  ^made! !

!CliInm subclass: #CliInmSub
  instanceVariableNames: ''
  classVariableNames: ''
  poolDictionaries: ''
  category: 'Cli-Test'!
EOF
  printf 'inm.st\n' >inm/LOAD_ORDER
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
    # SPEC §3.11 (P18): ao image save writes version 4 with an empty sources section.
    [ "$(od -An -tu2 -j4 -N2 two.aoimage | tr -d ' ')" = 4 ] || fail "image version is not 4"
    [ "$(od -An -tu8 -j40 -N8 two.aoimage | tr -d ' ')" = 8 ] || fail "sources section is not 8 bytes"
    run 1 "$AO" image save --load-order first/LOAD_ORDER --load-order plain/LOAD_ORDER three.aoimage
    stderr_is "plain/$BAD_LINE"
    [ ! -e three.aoimage ] || fail "image written after a failed second file-in"
    # SPEC §3.11 保存: ao image save drops the method cache first, so the class only the cache
    # reaches is neither written nor checked.
    run 0 "$AO" image save --load-order m8/LOAD_ORDER m8.aoimage
    stderr_is ''
    run 0 "$AO" image load m8.aoimage
    stderr_is ''
    # SPEC §3.11: ao image load supplies the natives the image lacks; its `nil isNil` probe passes
    # on an image whose UndefinedObject has an empty method dictionary.
    run 0 "$AO" image save --load-order inm/LOAD_ORDER inm.aoimage
    stderr_is ''
    run 0 "$AO" image load inm.aoimage
    stderr_is ''
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
    # SPEC §3.12: an extra argument is refused with the usage lines and exit 2; the image is not read.
    run 2 "$AO" image load deferred.aoimage extra
    [ "$(head -n 1 err.txt)" = 'ao: usage: ao image save <path>' ] || fail "no usage line for image load extra"
    # SPEC §3.12: so is a missing path.
    run 2 "$AO" image load
    [ "$(head -n 1 err.txt)" = 'ao: usage: ao image save <path>' ] || fail "no usage line for image load without a path"
    # SPEC §3.12: a trailing --load-order is no path; nothing is written under that name.
    run 2 "$AO" image save --load-order
    [ "$(head -n 1 err.txt)" = 'ao: usage: ao image save <path>' ] || fail "no usage line for image save --load-order"
    [ ! -e ./--load-order ] || fail "image written to --load-order"
    run 2 "$AO" image save --load-order first/LOAD_ORDER --load-order
    [ "$(head -n 1 err.txt)" = 'ao: usage: ao image save <path>' ] || fail "no usage line for a trailing --load-order"
    [ ! -e ./--load-order ] || fail "image written to --load-order"
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
    # SPEC §3.12: an extra argument is refused with the usage line and exit 2; nothing runs.
    run 2 "$AO" --test pass extra
    stderr_is 'ao: usage: ao --test [<dir>]'
    # SPEC §3.12 / §4.4: with no argument, it runs the image/tests found above the executable.
    run 0 "$AO" --test
    ;;
  *)
    fail "unknown case: $CASE"
    ;;
esac
echo "ok: $CASE"

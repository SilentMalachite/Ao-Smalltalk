# 05 コンパイラ（スキャナ・パーサ・コード生成・チャンク）

対象: `compiler/include/ao/*.hpp`、`compiler/src/{Scanner,Parser,Codegen,ChunkParser,Compiler}.cpp`、`runtime/src/Compile.cpp`、`runtime/include/ao/Compile.hpp`、テスト `compiler/tests/*.cpp`、`runtime/tests/{compiler_roundtrip_test,chunk_filein_test,accept_abi_test,browser_abi_test}.cpp`

検証: 全ファイルを読んだうえで、既存の `build/runtime/libao_runtime.a` と `build/compiler/libao_compiler.a` にリンクしたプローブをスクラッチで実行した（`ao_eval` / `ao_accept_class` / `ao_accept_method` 経由の評価、`parseChunks` の直接呼び出し、`fileInString` で vendor を 1 ファイルずつ読み込んでエラーを収集、AST 走査による vendor の外側 temp 代入箇所の数え上げ）。リポジトリ内のファイルは変更していない。Codegen.cpp のワークスペース temp 処理（~90–120 行）と、依頼文に既出として挙がっている項目は除外した。

## 概要

| 重大度 | 件数 |
|---|---|
| Critical | 2 |
| High | 6 |
| Medium | 5 |
| Low | 4 |

## 指摘

### [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる
- 場所: `runtime/src/Compile.cpp:127-148`（`fillInstVars`）、利用箇所 `:212` と `:381`
- 問題: `fillInstVars` はスーパークラス連鎖の `kClassSlotInstVarNames` を並べるだけである。Kernel クラスはこの名前配列を持たないが、`instSize` は 0 ではない（OrderedCollection は 3、Dictionary・Set・Point は 2）。一方 `subclass:…`（`kernel/Behavior.cpp:233`）は新しい変数を `superInst` の後ろに置く。このため、コンパイラが振る添字は親の内部スロットを指してしまう。
- 失敗シナリオ: `OrderedCollection subclass: #R5OC instanceVariableNames: 'x'` を作り、`getX ^x` を定義する。`R5OC new getX` は `x` ではなく、内部配列 `#(nil nil …)` を返す。変数を `'x y z'` にして `x := 'ab'. y := 1000000. z := 1000000` を実行したあと `add:` を送ると、`Heap.cpp:74` の `slotAt` アサーション（bytes オブジェクトをポインタとして読んだ）でプロセスが abort する。NDEBUG ビルドでは、バイト列を Oop として読む型混同になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 親の `instSize` から名前を持たない先頭スロットの数を求め、ダミー名で詰めてから名前を並べる。あるいは Bootstrap で Kernel クラスにも instVarNames を入れる。

### [Critical] ブロック内の外側 temp への代入が黙って捨てられる（ifTrue: も展開しない）
- 場所: `compiler/src/Codegen.cpp:268-286`（`bindTemp` が外側変数をブロックの `copied` に複製）、`:543-545`（`compileAssign` が複製側へ `StoreTemp`）、`:569-597`（`ifTrue:`・`whileTrue:` などを展開しない。`emitJump` / `patchJump`（`:191-210`）は一度も呼ばれない）
- 問題: ブロックは外側 temp を値でコピーして起動する。ブロック内の代入はコピーにしか書かれず、外側にも同じブロックの次回起動にも残らない。しかもコンパイラは制御構造を展開しないので、`ifTrue: [x := …]` のような基本形まで壊れる。SPEC §3.5 は jump を「コンパイラが生成し」と定めているが、コンパイラは生成していない。
- 失敗シナリオ: `| y | y := 0. 3 > 1 ifTrue: [y := 1]. y` は `0` を返す。メソッド `sign: n | s | s := 0. n > 0 ifTrue: [s := 1]. ^s` も `R5W new sign: 5` で `0` を返す。`#(1 2 3) do: [:e | sum := sum + e]` の結果も `0` である。vendor の Cuis ソースでは、この形が 57 箇所あった（`Bag>>max:`、`Heap>>downHeap:`、`LinkedList>>do:`、`Time class>>readFrom:` など）。いずれもエラーなしで誤った結果を返す。
- 根拠: プローブで再現（確度: 高）。SPEC §3.4 は v1 を「コピーされた値」と書き、`block_test.cpp` の `CopyDoesNotWriteOuter` はこの挙動を固定している。それでも、Blue Book の BlockContext はホームの temp を共有する。コピーだけなら、代入を受け付けて捨てるのは誤りである。
- 修正の方向: 先に SPEC を直す。対応は 2 つである。(1) リテラルブロックを引数に取る `ifTrue:`・`ifFalse:`・`and:`・`or:`・`whileTrue:`・`to:do:` をジャンプに展開する（02 が指摘した `whileTrue:` 未実装もここで解消する）。(2) 共有 temp（間接セル）を入れる。v1 のうちは、捕捉した変数への代入をコンパイルエラーにする。

### [High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない
- 場所: `compiler/src/Scanner.cpp:27-48`（`isBinaryChar` に `','` が無い）。`,` は `:393-397` の既定分岐に落ちて `Tok::Error` になる。
- 問題: `,` は Smalltalk-80 の二項セレクタ文字である。これが無いため `'a' , 'b'`、`#(1 2) , #(3)`、`#,`、および `, other` というメソッドパターンがすべて「invalid token」になる（app 担当からの報告も同じ根本原因）。
- 失敗シナリオ: `ao_eval("'a', 'b'")` は AO_ERR_COMPILE（区間 3–4、`,`）を返す。vendor を読み込むと、`Heap.st`・`Exception.st`・`Timespan.st`・`Time.st` で `,` を使う 7 メソッドが落ちる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `isBinaryChar` に `','` を足す。スキャナのテストに `,` と `#,` を加える。

### [High] 引数と temp を両方持つブロックが通らず、`[:a :b | a | b]` は誤コンパイルされる
- 場所: `compiler/src/Parser.cpp:467-488`（`parseBlock` の temp 解析）
- 問題: 引数の後の `|` を消費したあと、次の `|` を「空の temp 宣言」として読んでしまう。一方で、「識別子の並び + `|`」は開きの `|` が無くても temp 宣言として採用する。
- 失敗シナリオ: `[:x | | t | t := x. t] value: 3` と `[:x || t | …]` は `expected ']'` になる。vendor の `Bag>>product:` / `Bag>>sum:` もこの形で落ちる。`[:a :b | a | b] value: true value: false` は `a` を temp 宣言と誤読し、`true` ではなく `false` を返す。`[:a | a | false] value: true` も同じく `false` になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 引数がある場合は区切りの `|` を必須にし（`||` は「区切り + temp の開き」と読む）、そのあとに `parseTemps` と同じ `| ids |` 解析を行う。識別子を読み戻す（replay）経路は削除する。

### [High] 2^63 以上の整数リテラルが黙って 0 になる
- 場所: `compiler/src/Scanner.cpp:136-143, 218-220`（あふれたら `intValue` を設定しない）、`compiler/src/Parser.cpp:426`、`compiler/src/Codegen.cpp:730-735`（`intValue == 0` を `PushZero` にする）
- 問題: int64 をあふれたとき、スキャナは `isFloat=false` のまま `intValue=0` を残す。パーサとコード生成はその 0 をそのまま使う。
- 失敗シナリオ: `100000000000000000000 printString` は `'0'` を返し、`-100000000000000000000` も `0` になる。`-9223372036854775808` も `0` である。`#[1 99999999999999999999 3]` も 0 のバイトとして受理される。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 桁の文字列を Literal に持たせて、`boxLiteral` で LargeInteger を生成する。最低限、あふれた時点でコンパイルエラーにする。

### [High] クラス変数を読み書きできない
- 場所: `runtime/src/Compile.cpp:127-148, 211-212, 380-381`（`CompileEnv` にクラス変数・プールが無い）。関連: `runtime/src/kernel/Behavior.cpp:280`（`classVariableNames:` を無視して classPool を nil にする）、`Interpreter.cpp:575-581`（`PushGlobal` は `wk.named` だけを引く）
- 問題: `classVariableNames:` で宣言した名前がコンパイル時に解決されない。代入は「cannot assign」になり、読み出しは同名のグローバルを引く（無ければ nil）。
- 失敗シナリオ: `classVariableNames: 'Count'` のクラスで `bump Count := 1` を accept すると `cannot assign` で拒否される。`count ^Count` は nil を返す。vendor の `Time class>>…`（`MillisecondClockOffset := nil`）もこれで落ちる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: クラス定義でクラス変数を Association のプールとして保持する。`CompileEnv` に（スーパークラス連鎖の）クラス変数名を渡し、Association 経由の push / store を生成する。

### [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない
- 場所: `runtime/src/Compile.cpp:232`（`applyMethodsFor` の `installMethod` の直後に `ctx.cache->forget` が無い）
- 問題: `acceptMethodSource`（`:422-424`）は無効化するが、file-in と `ao_accept_class` が通る `applyMethodsFor` は何もしない。既出の「定義クラス分しか無効化しない」（~423）とは別の問題で、こちらは無効化がまったく行われない。
- 失敗シナリオ: `R5M>>foo ^1` を accept して `R5M new foo` を 1 回呼ぶ。次に `ao_accept_class("!R5M methodsFor: 'x'!\nfoo\n  ^2! !")` を実行する。戻り値は AO_OK だが、`R5M new foo` は `1` のままになる（未キャッシュのサブクラス `R5M2 new foo` は `2`）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `applyMethodsFor` でもインストール後に `forget` する（サブクラス分の扱いは既出の指摘と同じ直し方でそろえる）。

### [High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む
- 場所: `compiler/src/ChunkParser.cpp:134-156`（`isCharacterBang` を考慮するのは `$!` だけ）
- 問題: 文字リテラル `$'` で `inStr` が、`$"` で `inCmt` が切り替わり、次の `'` / `"` まで行末の `!` が終端として扱われない。
- 失敗シナリオ: `quote ^$'!` の後ろにある `two ^2! !`、DoIt、`!Foo class methodsFor: 'b'!` 以降の全メソッドが 1 チャンクに連結され、1 件のコンパイルエラーでまとめて失われる。`isDq: c ^c = $"!` でも同じことが起きる。
- 根拠: プローブ（`parseChunks` 直呼び）で再現（確度: 高）
- 修正の方向: `'` / `"` を見る前に、直前が奇数個の `$` なら文字リテラルとして扱う（`isCharacterBang` と同じ判定を流用する）。

（参考）小数リテラルが正しく丸められない件（`Scanner.cpp:163-211`、`0.7 = (7 / 10.0)` が false）は `03-kernel-numeric.md` で扱う。プローブでも再現した。

### [Medium] 二項演算子の直後の負数リテラルが、別のセレクタとして字句化される
- 場所: `compiler/src/Scanner.cpp:318-323`（`lexBinary` が `-` まで貪欲に取る）
- 問題: 空白を挟まない `2*-1`・`3@-2`・`3>-1` が、セレクタ `*-`・`@-`・`>-` になる。Squeak / Pharo は、後ろに数字が続く `-` を二項セレクタに含めない。
- 失敗シナリオ: `2*-1` と `3@-2` は DNU（`Message`）になる。`3 > -1` なら `true` になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `lexBinary` で、先頭以外の `-` の次が数字なら、その手前で切る。

### [Medium] 文字列の中の `!!` が `!` に戻らない
- 場所: `compiler/src/ChunkParser.cpp:111-121`（`!inStr && !inCmt` のときしか `!!` を解釈しない）
- 問題: チャンク形式ではチャンク全体で `!` が `!!` に二重化されるが、文字列内の二重化を元に戻していない。
- 失敗シナリオ: 標準の file-out にある `^'Hello!!'` を読むと、メソッドは `'Hello!!'` を返す（プローブではチャンクのソースに `!!` が残った）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 文字列・コメントの中でも `!!` → `!` を適用する。単独の `!` は、どの状態でも終端とする。

### [Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる
- 場所: `compiler/src/ChunkParser.cpp:122-126, 160-163`（2 つ目の `!` を読み捨てて空チャンクを作らない）、`:301-306`（`collecting` が解除されない）
- 問題: `methodsFor:` セクションの終わりを検出しないため、次のヘッダまでの全チャンクがメソッドのソースとして扱われる。既出の「DoIt を黙って捨てる」とは別の問題で、こちらは不正なメソッドとして登録してしまう。
- 失敗シナリオ: `…^2! !\n\nFoo initialize!` の `Foo initialize` が、`Foo>>Foo`（本体 `initialize`）として登録される。`Smalltalk at: #Bar put: 3!` は `Foo` 宛てのコンパイルエラーになる。
- 根拠: プローブ（`parseChunks` 直呼び）で再現（確度: 高）。`chunk_test.cpp:89` の `BangSpaceBang…` はこの非標準の挙動を固定している。
- 修正の方向: `! !` の 2 つ目で空チャンクを出し、`collecting` を解除する。Cuis の「メソッドごとにヘッダ」形式は、それでも正しく読める。

### [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している
- 場所: `runtime/src/Compile.cpp:213-237`（エラーを積んでも `return true`）、`runtime/src/Session.cpp:191-192`（`errors` を捨てる）
- 問題: `ao_filein_load_order` はメソッドのコンパイル失敗を呼び出し側へ伝えない。
- 失敗シナリオ: 現在の `image/vendor/LOAD_ORDER` では、`Bag`（2）・`Heap`（2）・`Exception`（2）・`Notification`（1）・`Timespan`（2）・`Date`（1）・`Time`（7）の計 17 メソッドがコンパイルに失敗する。それでも AO_OK が返り、app 側は気づかない。原因は上記の `,`・ブロックの temp・クラス変数に加え、`{…}`・`` `…` ``・`<primitive: 'name'>`。
- 根拠: プローブ（`fileInString` をファイル単位で実行）で再現（確度: 高）
- 修正の方向: エラーがあれば非 0 を返すか、件数と先頭のメッセージを ABI で取れるようにする。`vendor_filein_test` で「エラー 0 件」を固定する。

### [Medium] 構文の入れ子に深さの上限がなく、コンパイル時にスタックが溢れる
- 場所: `compiler/src/Parser.cpp:273-656`（再帰下降）、`compiler/src/Codegen.cpp:443-484`（再帰生成）
- 問題: 括弧やブロックの入れ子の深さを検査していない。02 のインタプリタの再帰とは別に、コンパイル段階でも落ちる。
- 失敗シナリオ: 括弧 1 万段の `((…(1)…))` を ao_eval に渡すと SIGSEGV（exit 139）になる。app はメインスレッドで評価するので、app ごと落ちて未保存のイメージが失われる。
- 根拠: プローブで再現（確度: 高。5000 段は成功、1 万段で落ちる）
- 修正の方向: パーサに深さカウンタを入れ、たとえば 256 段で `nesting too deep` のエラーにする。

### [Low] 指数表記の意味が Smalltalk-80 と異なる
- 場所: `compiler/src/Scanner.cpp:177-213`
- 問題: `1e3` を Float にする（ST-80 / Pharo は Integer 1000）。基数付きでも 10 を底に使う（`2r1e4` が 16 ではなく `10000.0`）。`exp > 400` なら仮数が 0 でも inf にする（`0e500` → `inf`）。
- 失敗シナリオ: 上の入力をプローブで確認した。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 仮数が整数なら整数のまま `radix^exp` を掛ける。inf の判定は結果の値で行う。

### [Low] 宣言の検証がない（引数への代入・擬変数名・重複名）
- 場所: `compiler/src/Codegen.cpp:543-545`（引数も `StoreTemp` の対象になる）、`:89-113`・`:752-765`（重複と擬変数名を検査しない）、`:486-507`（`isSelfishReturn` が temp の `self` を無視する）
- 問題: `foo: x x := 5`、`dup: x dup: x`、`| t t |`、`| self |`、`nilArg: nil` がすべて通る。
- 失敗シナリオ: `| self | self := 3. ^self` は 3 ではなく receiver を返す。本文の `self` は temp を読むのに、`^self` は `ReturnReceiver` に短縮されるためである。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 引数とブロック引数への代入、擬変数名の宣言、同一スコープでの重複をコンパイルエラーにする。

### [Low] カスケードの扱いが不完全
- 場所: `compiler/src/Codegen.cpp:604-618`、`compiler/src/Parser.cpp:355-384`
- 問題: `super foo; bar` の 2 つ目以降が通常送信になる。また各パートは 1 メッセージしか受け付けない。
- 失敗シナリオ: `^super who; who` はスーパークラスの 1 ではなく 2 を返す。`x add: 3; yourself size` は `unexpected token` になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 先頭の送信が super なら全パートを `SendSuper` にする。カスケードの各パートで、単項 → 二項 → キーワードの連鎖を解析する。

### [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する
- 場所: `compiler/src/ChunkParser.cpp:187-201`、`:127`（行の途中の単独 `!` は終端扱いにならない）
- 問題: メソッドパターンが `subclass: x` だと ClassDef に分類される。`applyClassDef` は `missing class` で false を返し、`applyChunks`（`Compile.cpp:281-283`）がファイルの残りを全部中断する。また `foo ^1! bar ^2! !` は 1 つのメソッドとして連結される。
- 失敗シナリオ: 上の入力をプローブで確認した（`ClassDef class=x`）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: ヘッダと認めるのは、`!` で始まる行のチャンク、または直前のセクションが閉じた後の `X subclass: #Y …` の形に限る。

## テストの穴
- `block_test` の `CopyDoesNotWriteOuter` は外側に書き戻されない挙動を固定しているが、`ifTrue: [x := …]` や `do: [:e | sum := …]` の結果を確かめるテストがない。
- スキャナのテストに、`,`・`#,`・`2*-1`・`3@-2`・20 桁の整数・`1e3` / `2r1e4` の型と値が無い（`EighteenDigitInteger…` は int64 の範囲内だけ）。
- パーサのテストに、`[:x | | t | …]`・`[:x || t | …]`・`[:a :b | a | b]`・入れ子の深さの上限が無い。
- Kernel クラス（OrderedCollection、Point など）のサブクラスでインスタンス変数を読み書きするテストが無い（`HolderInstVarRoundTrip` は Object 直下のみ）。
- クラス変数の読み書きのテストが無い。
- チャンクのテストに、`$'`・`$"`・文字列内の `!!`・`! !` の後の DoIt・パターン `subclass:` のメソッドが無い。
- チャンク経由の再定義で、キャッシュ済みの送信が新しいメソッドを使うことを確かめるテストが無い。
- 上限のテスト（リテラル 256 個、temp 255 個、コピー値 255 個）が無い。
- vendor の file-in がコンパイルエラー 0 件で終わることを確かめるテストが無い（現状 17 件）。

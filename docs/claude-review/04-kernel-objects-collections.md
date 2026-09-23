# 04 Object・Behavior・コレクション系 Kernel ネイティブ

対象: `runtime/src/kernel/{Object,Behavior,Array,String,Collection,Dictionary,Stream}.cpp`、`runtime/tests/{object_boolean_test,behavior_test,array_string_test,collection_do_test,print_string_test,transcript_model_test}.cpp`

検証: コードを読んだうえで、`build/` の `libao_runtime.a` / `libao_compiler.a`（Debug, -O0）をスクラッチへ複製してプローブを動かした。

1. `ao_eval` の Print it ドライバで約 150 式を評価した。
2. ランタイム内部 API を直接呼ぶ GC プローブを使った。nursery を満杯にしてからネイティブを呼ぶ。
3. `ao_accept_class` / `ao_accept_method` を使うプローブを動かした。

対象スイート 80 件（KernelScan を含む）は、複製したテストバイナリですべて緑だった。

## 概要

| 重大度 | 件数 |
|---|---|
| Critical | 1 |
| High | 2 |
| Medium | 8 |
| Low | 8 |

## 指摘

### [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む
- 場所: `runtime/src/kernel/Behavior.cpp:228, 243-272, 274, 277, 283`。呼び出し側の `runtime/src/Compile.cpp:180` の `Oop args[5]` もルートされていない
- 問題: `receiver`（親クラス）と `nameArg`（`args[0]` のコピー）は、どちらもルートされない値のコピーである。その後の `allocateRetry` や、`MethodDictionary::create` が失敗したときの `collectNursery` でオブジェクトが移動する。GC のあとも古いアドレスのまま `superclass` と `name` のスロットへ書く。さらに `ctx.heap.klass(receiver)` は転送済みヘッダから転送先、つまり親クラス自身を返す。このため、メタクラスの superclass が親のメタクラスではなく親クラスになる。`args[4]`（category）も、`applyClassDef` や `perform:withArguments:` のようにルートされない配列から渡されると、同じく古いアドレスになる。
- 失敗シナリオ: 直前の GC 以降に作ったクラス A を親にして、nursery の残りが 80 バイト未満のときに `A subclass: #B …` を送る。プローブでは次のようになった。
  - B.superclass と B.name が、どちらも移動前（nursery の退避元）の旧アドレスを指した。
  - `B class superclass` が `A class` ではなく A になった。
  - その後 nursery を 2 回反転させると、B.superclass は無関係なゴミオブジェクト（klass=nil、size=0）に変わった。

  名前のシンボルは直前に intern されるので、ふつうは nursery にある。Browser のクラス Accept、vendor file-in、Workspace のどれからでも起こりうる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 冒頭で `receiver`・`args[0]`・`args[4]` を `Root` に載せ、以降は必ず Root を通して読む。`metaSuper` もルート経由で求める。GC 圧下の回帰テストを足す。

### [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる
- 場所: `runtime/src/kernel/Dictionary.cpp:47-49, 101-145, 184-259`
- 問題: `consumeHash` は `hash` を送るが、結果を捨てる。`findPair` / `findValue` は内部配列を先頭から走査し、`=` を送って比べる。このため `at:put:`・`at:`・`add:`・`includes:` がどれも O(n) 回の送信になり、n 件を入れると O(n²) になる。
- 失敗シナリオ: `| d | d := Dictionary new. 1 to: 4000 do: [:i | d at: i put: i]. d size` に 9.3 秒かかる（2000 件なら 2.3 秒）。`Set` に 4000 件 `add:` すると 9.2 秒。Workspace はメインスレッドで評価するので、その間 UI が固まる。
- 根拠: プローブで計測（Debug ビルド。確度: 高）
- 修正の方向: `hash` の値で開番地法の初期位置を決め、線形プローブで探す。拡張するときは再ハッシュする。その前に、後述の `=` と `hash` の不一致を直しておく必要がある。

### [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない
- 場所: `runtime/src/kernel/Dictionary.cpp:393-423`（`do:`）、`:368-391`（`includes:` は値を見る）、`:425-456`（`collect:` は値を見る）
- 問題: Blue Book と Cuis の `Dictionary>>do:` は値を列挙する。ここでは要素ごとに Association を作って渡す。`select:`・`reject:`・`detect:ifNone:`・`inject:into:` は `do:` を経由する Collection のネイティブなので Association を受け取り、`collect:` と `includes:` だけが値を見る。さらに `at:ifAbsent:`・`removeKey:`・`keysDo:`・`associationsDo:` が無い。
- 失敗シナリオ: `d at: #a put: 1; at: #b put: 2` のあとで、次のようになる。
  - `d inject: 0 into: [:a :v | a + v]` は Message（DNU）を返す。
  - `d select: [:v | v > 1]` は `#()` を返す。
  - `d detect: [:v | v = 2] ifNone: ['none']` は `'none'` を返す。

  vendor の file-in 後に `| b | b := Bag new. b add: 3. b size` を評価すると AO_ERR_EVAL になる。`Bag>>size` が `contents inject:into:` で値を足しているためである。SPEC §3.6 は、P9 までに Bag などの中核プロトコルを動かすことを求めている。
- 根拠: プローブで再現（確度: 高）。`collection_do_test.cpp:111-126` は、Association を渡す挙動を期待値として固定している
- 修正の方向: `do:` を値の列挙に直す。`associationsDo:`・`keysDo:`・`keysAndValuesDo:`・`at:ifAbsent:`・`removeKey:` / `removeKey:ifAbsent:` をネイティブで足し、テストの期待値も直す。

### [Medium] `select:` / `reject:` が述語ブロックを要素ごとに 2 回評価する
- 場所: `runtime/src/kernel/Collection.cpp:134-155`
- 問題: 1 回目の `do:` で数え、2 回目の `do:` で詰める。述語に副作用があったり結果が毎回変わったりすると、数えた個数と詰めた中身が合わなくなる。
- 失敗シナリオ: `#(1 2 3) select: [:x | Transcript show: x printString. true]` は、Transcript に 1 2 3 を 2 回出す。`| oc | oc := OrderedCollection new. #(1 2 3) select: [:x | oc add: x. oc size <= 3]` は `#(nil nil nil)` を返す（正しくは `#(1 2 3)`）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 1 パスで、Root 付きの一時バッファ（または OrderedCollection）に集め、最後に Array へ詰める。

### [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す
- 場所: `runtime/src/kernel/Stream.cpp:185-197`（`copyPrefix`）
- 問題: 文字列でなければ、コレクションの物理スロットを先頭から `n` 個コピーする。名前付きインスタンス変数を飛ばさない。バイト列は 0 個として扱うので、結果が全部 nil になる。
- 失敗シナリオ:
  - 要素 1, 2 の OrderedCollection で `(ReadStream on: oc) contents` を評価すると `#(#(1 2 nil nil nil nil nil nil) 1)`（内部配列と firstIndex）が返る。
  - `(WriteStream on: (ByteArray new: 4)) nextPut: 1; nextPut: 2; contents` は `#(nil nil)` を返す。
  - `Array subclass: #PG instanceVariableNames: 'tag'` のインスタンスでは要素が 1 つずれる（`#(nil 9 nil)`）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 要素は `at:` を送って取り出す。結果はコレクションと同じ種類で作る。少なくとも ByteArray には ByteArray を返す。

### [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる
- 場所: `runtime/src/kernel/Stream.cpp:357-359`、`runtime/src/kernel/String.cpp:225-229`
- 問題: `String>>at:put:` は、UTF-8 の幅が変わってオブジェクトの 8 バイト境界を越えると失敗し、エラー文字列を返す。`nextPut:` はこの戻り値を確かめず、`noteWrite` で位置を進める。
- 失敗シナリオ: `| w | w := WriteStream on: (String new: 8). w nextPutAll: 'ééé'. w contents` は NUL を 3 文字返し、`position` は 3 になる。書いた文字はどこにも残らない。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 置き換えで幅が変わるときは、`stringAppendChar` と同じく新しい String を作って差し替える。最低でも `at:put:` の失敗を検出し、位置を進めない。

### [Medium] nil をキーや要素にすると tally だけが増え、見えないエントリが残る
- 場所: `runtime/src/kernel/Dictionary.cpp:114, 137, 209-214, 251-255`
- 問題: 空きスロットの印が nil なので、nil キーを書き込んでも空きのままになり、tally だけが増える。
- 失敗シナリオ:
  - `d at: nil put: 1` のあと、`d at: nil` は nil、`d includesKey: nil` は false を返すのに、`size` は 1 になる。
  - もう一度 `at: nil put: 2` を送ると、`size` は 2 になる。
  - `Set new add: nil` も `size` が 1 なのに、`includes: nil` は false を返す。
  - `collect:` は tally の長さの配列を作るので、末尾に nil が残る。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: nil キーを拒否してエラーオブジェクトを返すか、空きの印を専用の番兵オブジェクトに替える。

### [Medium] `shallowCopy`・`printString`・`asSymbol` が、nursery が足りないときに GC を再試行せず失敗する
- 場所: `runtime/src/kernel/Object.cpp:203, 365`、`runtime/src/kernel/String.cpp:272`（経由する `WellKnown::intern`）、`Object.cpp:23`（`fail` の `Str::fromUtf8`）
- 問題: `heap.allocate` は GC しない。これらは `allocateRetry` を使わないので、残りが足りないと空 OOP を返す。
- 失敗シナリオ: `| a | a := Array new: 100000. a copy size` は Message を返す。`copy` が空 OOP を返し、それに送った `size` が DNU になるためである。直後の `(Array new: 100000) size` は GC を再試行するので成功する。`| s | s := String new: 600000.` のあとの `s printString size`・`s copy size`・`s asSymbol size` も同じく失敗する。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: これらも `allocateRetry` と同様に、receiver を Root に載せてから GC して確保し直す。

### [Medium] Interval が、SmallInteger でない負の刻みを正の向きとして扱う
- 場所: `runtime/src/kernel/Dictionary.cpp:708, 767`（`forward` の判定）、`:717, :769`（2^20 回での打ち切り）
- 問題: `forward = !step.isSmallInteger() || step > 0` なので、Float などの負の刻みでも `>` で比べ、最初の比較でループが終わる。比較が Boolean を返さないときは 2^20 回まで回り続ける。本当に長い Interval は、2^20+1 要素で黙って打ち切られる。
- 失敗シナリオ: `(Interval from: 2.0 to: 1.0 by: -0.5) size` は 0 を返す（正しくは 3）。`collect:` は `#()` を返す。`(Interval from: 1 to: 2 by: 0.5) do: [...]` は整数と Float の比較が失敗するため、ブロックを約 100 万回呼び、Transcript に Message が延々と出る。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 刻みの向きは `step < 0` を送って決める。比較の結果が Boolean でなければ打ち切り、エラーオブジェクトを返す。

### [Medium] String・Symbol・Array の `=` と `hash` が一致しない
- 場所: `runtime/src/kernel/String.cpp:246-266, 333`、`Array.cpp:170`、`Object.cpp:99-101`
- 問題: `=` は内容比較に上書きしてあるのに、`hash` は `Object>>hash`（identityHash）のままである。`#abc = 'abc'` も true になるが、hash は別の値になる。
- 失敗シナリオ: `'abc' = 'abc' copy` は true なのに、`'abc' hash = 'abc' copy hash` は false になる。`#(1 2 3)` も同じ。いまの Dictionary は hash を使わないので表面化しない。しかし High の線形探索をハッシュ表に直した時点で、等しい文字列をキーにしても引けなくなる。ユーザーが自作するハッシュ構造も壊れる。数値版は 03-kernel-numeric で既報。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: String（バイト列の FNV など）と Array（要素の hash を合成し、深さに上限を付ける）に `hash` のネイティブを足す。Symbol の `=` を同一性比較にするかどうかもあわせて決める。

### [Medium] String の添字アクセスが O(i) なので、`do:`・`contents`・`nextPutAll:` が O(n²) になる
- 場所: `runtime/src/kernel/String.cpp:154-174`（`Str::at`）、`Collection.cpp:171-178`（`size` と `at:` で回す `do:`）、`Stream.cpp:139-155, 163-174`
- 問題: `at:` は毎回 UTF-8 を先頭から数え直す。`do:` は `at: 1..n` を送るので、全体で O(n²) になる。WriteStream への追記は、1 文字ごとに文字列全体をコピーし直す。`contents` も `Str::at` を 1 から n まで呼ぶ。
- 失敗シナリオ（計測値）:
  - `(String new: 40000) inject: 0 into: [:a :c | a + 1]` は 6.7 秒（同じ長さの Array なら 0.33 秒）。
  - `(ReadStream on: (String new: 40000)) contents size` は 6.2 秒。
  - `nextPut: $a` を 2 万回送ると 3.3 秒。
- 根拠: プローブで計測（Debug ビルド。確度: 高）
- 修正の方向: String 専用の `do:` と `contents` を、1 パスで処理するように書く。WriteStream は容量を倍々で確保し、書いたバイト数を別に持つ。

### [Low] ReadWriteStream の `contents` が position までしか返さない
- 場所: `runtime/src/kernel/Stream.cpp:410-418, 545-546`
- 問題: WriteStream の `contents` は position で切る。ReadWriteStream はこれを継承しているが、Blue Book や Squeak の ReadWriteStream は readLimit まで返す。
- 失敗シナリオ: `| w | w := ReadWriteStream on: String new. w nextPutAll: 'abc'; reset. w contents` は `''` を返す（期待値は `'abc'`）。直後の `w next` は `$a` を返すので、中身自体は残っている。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: ReadWriteStream に `contents` を足し、readLimit と position の大きい方まで返す。

### [Low] ReadStream の `nextPut:` が元のコレクションを書き換える
- 場所: `runtime/src/kernel/Stream.cpp:534-535`（`nextPut:` を PositionableStream に登録している）
- 問題: ReadStream でも書き込みが通ってしまう。
- 失敗シナリオ: `| s r | s := 'abc' copy. r := ReadStream on: s. r nextPut: $x. s` は `'xbc'` を返す。リテラルに対して同じことをすると、メソッドのリテラル自体が変わる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `nextPut:` を WriteStream 側へ移し、ReadStream では `shouldNotImplement` にする。

### [Low] Symbol の `copy` が、intern されていない別の Symbol を作る
- 場所: `runtime/src/kernel/Object.cpp:196-217`
- 問題: バイト列をそのまま複製するので、同じ綴りの Symbol が 2 つできる。セレクタの探索は同一性で比べるので、複製したほうはセレクタとして使えない。
- 失敗シナリオ: `#printString copy == #printString` は false を返す。`3 perform: #printString copy` は DNU（Message）になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: Symbol の `shallowCopy` / `copy` は self を返すようにする。

### [Low] バイト列クラスの `basicNew:` が、2^32 以上のサイズを切り詰める
- 場所: `runtime/src/kernel/Behavior.cpp:93-94`
- 問題: ポインタ側には `UINT32_MAX` を超えたかどうかの確認がある。バイト列側は `static_cast<std::uint32_t>(n)` で下位 32 ビットだけを使う。
- 失敗シナリオ: `(String new: 4294967299) size` は 3 を返す。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: バイト列側にも同じ上限の確認を入れ、失敗させる。

### [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す
- 場所: `runtime/src/kernel/Dictionary.cpp:560-568`
- 問題: 範囲外の添字には nil を、整数でない添字には空 OOP（AO_ERR_EVAL）を返す。Array と String はエラーオブジェクトを返すので、扱いが揃っていない。
- 失敗シナリオ: 要素が 1 個の OrderedCollection に `at: 0` や `at: 2` を送ると nil が返るので、誤りに気づけない。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `basicAt:` と同じく、`error:` を通してエラーオブジェクトを返す。

### [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す
- 場所: `runtime/src/kernel/Object.cpp:138-151`
- 問題: バイト列でなければどんなオブジェクトも受け付け、名前付き変数を含む物理スロットを引数として展開する。展開先の `std::vector` はルートされない。受け手のネイティブが GC のあとで `args[i]` を読むと、古いアドレスを読むことになる（Critical の `subclass:…` がその例）。
- 失敗シナリオ: `3 perform: #+ withArguments: (OrderedCollection new add: 4; yourself)` は、内部配列・firstIndex・lastIndex の 3 つを引数にして `+` を呼び、空 OOP になる。
- 根拠: コード読解とプローブ（確度: 中）
- 修正の方向: 引数は Array に限る。展開した値は RootedArray に載せる。

### [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む
- 場所: `runtime/src/kernel/String.cpp:279-286`
- 問題: `allocateRetry` のあとで、ルートしていない `receiver` のペイロードを memcpy する。
- 失敗シナリオ: nursery にある Symbol に対し、nursery を満杯にしてから `asString` を送ると、移動前の領域から読む。その領域はまだ上書きされていないので、今のところ偶然正しい文字列になる（プローブで確認）。GC の実装が変われば壊れる。
- 根拠: プローブとコード読解（確度: 中）
- 修正の方向: 確保の前に `receiver` を Root に載せる。

### [Low] バイト列クラスに、名前付き変数を持つサブクラスを作れてしまう
- 場所: `runtime/src/kernel/Behavior.cpp:236-240`
- 問題: 親が bytes のとき、インスタンス変数を足しても形式は bytes のままで、実体には変数用のスロットが無い。
- 失敗シナリオ: `String subclass: #PJ instanceVariableNames: 'tag' …` は成功する。しかし `(PJ new: 2) instVarAt: 1` は範囲外エラーになり、`tag` を参照するメソッドも値を保持できない。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: bytes の親に変数を足す定義はエラーにする。

## 既報との関係（件数に含めない）
- クラス変数: `Behavior.cpp:280` が `classVariableNames:` を捨て、classPool を常に nil にする。これは 05-compiler「クラス変数を読み書きできない」と原因が同じである。
- Kernel クラスに instVarNames が無い件（05-compiler の Critical と原因が同じ）は、Object 側にも影響が出ている。
  - `(Association key: 3 value: 4) instVarNamed: #key` は `'instVarNamed: not found'` を返す。
  - Association に `probeKey ^key` を Accept すると、エラーにならずに `key` をグローバルとして読み、nil を返す。
- 反復ネイティブが `^` を無視して回り続ける件は、02-interpreter で既報。
- Kernel 走査（`KernelScan.*`）は緑。起動直後の Kernel メソッド辞書は、すべて NativeMethod だった。

## テストの穴
- GC 圧下での Kernel ネイティブのテストが無い。nursery を満杯にしてから `subclass:…`・`asString`・`copy`・`printString` を呼び、戻り値とスロットが正しいかを確かめるテストが要る。
- 添字の境界（0、負、size+1、整数以外、2^32 以上）を、Array・String・ByteArray・OrderedCollection でまとめて確かめるテストが無い。
- Dictionary / Set について、nil キーのテスト、要素が多いときの性能のテスト（例: 1 万件で 1 秒未満）、`=` と `hash` が一致するかのテストが無い。
- `collection_do_test.cpp:111-126` は、Dictionary の `do:` が Association を渡すことを期待値にしている。これは Blue Book と逆の挙動を固定している。
- `select:` / `reject:` の述語が要素ごとに 1 回だけ評価されることのテストが無い。
- ストリームについて、次のテストが無い。
  - ByteArray・OrderedCollection・Array サブクラス上での `contents`
  - 固定長 String への多バイト文字の書き込み
  - ReadWriteStream の `contents`
  - ReadStream への `nextPut:`
- Interval の刻みが負の Float や Fraction のときのテストが無い（`IntervalFromToByAndIntegerTo` は SmallInteger の刻みだけ）。
- `instVarNamed:` と `perform:withArguments:` のテストが無い。Symbol の `copy` が同一性を保つかのテストも無い。

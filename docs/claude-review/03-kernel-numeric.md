# 03 数値・スカラー系 Kernel ネイティブ

対象: `runtime/src/kernel/{SmallInteger,LargeInteger,Float,Magnitude,Character,Geometry,Boolean}.cpp`, `runtime/include/ao/LargeInteger.hpp`, `runtime/tests/{smallinteger_arith_test,geometry_test,object_boolean_test,print_string_test}.cpp`

検証: 対象ファイルは全行を読んだ。`graphify query` で Magnitude の近傍を確認した。既存の `build/` の静的ライブラリにリンクしたプローブ（`ao_eval` PRINTIT、`ao_accept_class` / `ao_accept_method`）をスクラッチで実行した。Python を正解とする差分テストも回した。対象は `+ - * // \\ quo: rem: bitAnd: bitOr: bitXor: bitShift: < =` の 1500 件で、SmallInteger/LargeInteger 境界、±2^62、±2^63、±2^64、±2^128 付近を重点的に選んだ。1500 件すべて一致した。`kSmiMin // -1`、`0 - kSmiMin`、`kSmiMin * -1` の昇格、SmallInteger への正規化、floor と trunc の符号規則はいずれも正しい。Kernel セレクタはすべて `putNative` 経由で登録されており、NativeMethod 不変条件は守られている。

補足: LargeInteger の printString がクラス名になること、Float の printString が `%g` であること、`1 = 1.0` が false になること、0 除算が `error:` の文字列を返すことは、SPEC またはテストで意図された挙動である。いずれも指摘から外した。Number プロトコルの未実装（`negated`, `abs`, `@`, `asFloat`, `truncated` など）は SPEC の必須セレクタに含まれないため、これも外した。

## 概要

| 重大度 | 件数 |
|---|---|
| Critical | 1 |
| High | 2 |
| Medium | 3 |
| Low | 8 |

## 指摘

### [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす
- 場所: `runtime/src/kernel/Magnitude.cpp:28-35`, `runtime/src/kernel/Magnitude.cpp:57-61`
- 問題: `ao_Magnitude_lessOrEqual` は `<` を送ったあと、値渡しで受けた `receiver` のまま `=` を送る（`between:and:` は `>=` のあとに `<=` を送る）。最初の送信が CompiledMethod ならば、`Interpreter::run` の safepoint で nursery GC が走り、オブジェクトが移動する。2 回目の送信は移動前の古いアドレスに対して行われる。`sendBin` から呼ばれた場合は `args` もローカル変数への未ルートのポインタなので、同じ理由で古くなる。
- 失敗シナリオ: `Magnitude subclass: #Money` を定義し、`<` と `=` を Smalltalk で書く。そのうえで `1 to: 20000 do: [:i | a := Money new amount: 5. b := Money new amount: 5. a <= b]` を実行すると、`lookup` → `Heap::slotAt` の `assert(i < h->size)` で abort する。NDEBUG ビルドでは、転送済みヘッダをクラスとして読む不正メモリアクセスになる。同じループを `(a < b) or: [a = b]` に書き換えると 0 件で完走する。vendor の `Time`（Magnitude のサブクラスで、`<` と `=` を Smalltalk で定義している）でも同じ経路を通る。
- 根拠: プローブで再現した。バックトレースは `ao_Magnitude_lessOrEqual → sendBin → send → lookup → Heap::slotAt`（確度: 高）。
- 修正の方向: `Geometry.cpp` と同じく、`receiver` と引数を `Root` で保持し、各送信のあとは `Root::slot` から読み直す。

### [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す
- 場所: `runtime/src/kernel/SmallInteger.cpp:41-66`, `runtime/src/kernel/SmallInteger.cpp:147-152`, `runtime/src/kernel/Float.cpp:65-77`, `runtime/src/kernel/Float.cpp:116-128`
- 問題: Integer の `+ - * <` は `bothInts` で Float を拒否し、`Oop{}` を返す。Float 側の `asFloat` は int64 に収まる整数しか受け付けず、Fraction も扱わない。`asNumDen` も Float を扱わない。結果として同じ演算が向きによって成功したり失敗したりする。
- 失敗シナリオ: `1.5 + 1` は `2.5` になるが、`1 + 1.5`、`1 - 1.5`、`2 * 1.5`、`1 < 1.5`、`1.5 > 1`、`2 >= 1.5` は AO_ERR_EVAL になる。`(1 bitShift: 70) + 1.0`、`1.0 < (1 bitShift: 70)`、`(1/2) + 0.5`、`0.5 + (1/2)` も失敗する。影響は Rectangle にも及び、`(0@0 corner: 10@10) containsPoint: (0.5@0.5)` は false、`(Rectangle origin: 0.5@0 corner: 2@2) width` は失敗する。さらに Float と int64 の比較は double への丸めで精度を失う。`1152921504606846976.0 < 1152921504606846977` は false になる。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: Integer の算術・比較ネイティブで、引数が Float または Fraction のときに Float/Fraction の経路へ委譲する。`asFloat` は LargeInteger 全域を扱えるようにする。比較は整数と double を厳密に比べる。

### [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）
- 場所: `compiler/src/Scanner.cpp:218-219`, `compiler/src/Parser.cpp:426`
- 問題: 10 進数の累積がオーバーフローすると `overflow` が立つ。このとき `t.intValue` は代入されず、既定値の 0 のまま Parser に渡る。
- 失敗シナリオ: `100000000000000000000` → `0`、`9223372036854775808 = 0` → `true`、`18446744073709551616` → `0`。LargeInteger 算術が正しく動いても、ソース上の大きな定数はすべて 0 になる。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: オーバーフローしたら数字列を LargeInteger リテラル（バイト列）として保持し、`boxLiteral` で箱詰めする。v1 で対応しないなら、コンパイルエラーにする。

### [Medium] Float リテラルの解析が正しく丸められない（compiler 側）
- 場所: `compiler/src/Scanner.cpp:171-172`, `compiler/src/Scanner.cpp:207`
- 問題: 小数部を `value += dv * place` で逐次足しているため、IEEE754 の最近接値にならない。指数が 400 を超えると、仮数に関係なく ∞ になる。
- 失敗シナリオ: `0.3 = (0.1 + 0.2)` → `true`（IEEE では false）。`0.7 = (0.1 * 7)` → `true`。`(0.3 - 0.1) - 0.2` の符号が IEEE と逆になる。`0e500` → `inf`（正しくは 0）。`0.001e310` → `inf`（正しくは 1e307）。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: 字句全体を `std::from_chars(double)` または `strtod` に渡す。

### [Medium] `bitShift:` にシフト量 -2^63 を渡すと符号反転オーバーフロー（UB）になり、誤った結果を返す
- 場所: `runtime/src/kernel/LargeInteger.cpp:714`
- 問題: `asInt64IfFits` は -2^63 を「int64 に収まる」と判定する。その直後の `-sh` は signed overflow で UB になる。実機ではラップして負値になり、`1<<24` のガードを通過する。そのうえ `static_cast<unsigned>` が 0 になるため、元の値がそのまま返る。
- 失敗シナリオ: `5 bitShift: (-1 bitShift: 63)` → `5`（正しくは 0）。`-5 bitShift: (-1 bitShift: 63)` → `-5`（正しくは -1）。
- 根拠: プローブで再現した。コード読解でも確認した（確度: 高）。
- 修正の方向: `sh < -(1<<24)` を先に判定して 0 または -1 を返し、そのあとで符号を反転する。

### [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない
- 場所: `runtime/src/kernel/Float.cpp:321-324`
- 問題: Fraction には四則演算しか登録されていない。そのため `=` は Object の同一性比較になり、`<` は DNU になる。
- 失敗シナリオ: `(1/2) = (1/2)` → `false`。`(1/2) < (2/3)` → DNU（Message が返る）。Fraction を Magnitude として使う `max:` や `between:and:`、ソートはすべて壊れる。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: 正規化済みの分子・分母を比べる `ao_Fraction_equals` と、交差乗算で比べる `ao_Fraction_lessThan` を追加する。

### [Low] `>=` を `(a < b) not` で計算するため、NaN に対して true を返す
- 場所: `runtime/src/kernel/Magnitude.cpp:43-49`
- 問題・失敗シナリオ: `(0.0/0.0) >= 1.0` → `true`、`1.0 >= (0.0/0.0)` → `true`。一方で `<=` は false を返すので、両者の結果が食い違う。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: Float に `>=` と `<=` のネイティブを置くか、`(b < a) or: [a = b]` で計算する。

### [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る
- 場所: `runtime/src/kernel/Object.cpp:99-101`（`=` の定義は `runtime/src/kernel/SmallInteger.cpp:137`, `runtime/src/kernel/Float.cpp:239`, `runtime/src/kernel/Geometry.cpp:177`）
- 問題・失敗シナリオ: `(1 bitShift: 70) hash = (1 bitShift: 70) hash`、`1.5 hash = 1.5 hash`、`(Point x: 1 y: 2) hash = (Point x: 1 y: 2) hash` はいずれも false になる。現在の Dictionary と Set は線形走査なので表に出ていない。しかし、ハッシュを使う利用者コードや将来のハッシュ表化で、キーが見つからなくなる。
- 根拠: プローブで確認した（確度: 高）。
- 修正の方向: LargeInteger、Float、Point に値ベースの `hash` ネイティブを追加する。

### [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める
- 場所: `runtime/src/kernel/Boolean.cpp:57-60`, `runtime/src/kernel/Boolean.cpp:67-75`, `runtime/src/kernel/Boolean.cpp:117-130`
- 問題・失敗シナリオ: Blue Book の `true & x` と `false | x` は `x` を返すが、この実装では `true & nil` → `false`、`false | 3` → `false`、`true xor: 3` → `true` になる。誤った引数を渡しても黙って Boolean が返るため、誤りが隠れる。
- 根拠: プローブで確認した（確度: 高）。
- 修正の方向: `True>>&` と `False>>|` は引数をそのまま返す。`eqv:` と `xor:` は、引数が Boolean でなければ失敗させる。

### [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する
- 場所: `runtime/src/kernel/Geometry.cpp:84-86`
- 問題・失敗シナリオ: `(Point x: 1 y: 2) + nil` は正常な Point を返したように見えるが、その `x` は空 OOP で、後から AO_ERR_EVAL になる。失敗の原因と表面化する場所が離れる。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: `nx` と `ny` が `isEmpty()` なら、Point を作らずに `Oop{}` を返す。

### [Low] Point と Rectangle のネイティブがサブクラスを扱えない
- 場所: `runtime/src/kernel/Geometry.cpp:36-44`, `runtime/src/kernel/Geometry.cpp:110`, `runtime/src/kernel/Geometry.cpp:204`
- 問題・失敗シナリオ: `isPoint` と `isRect` はクラスの完全一致を要求するため、サブクラスのインスタンスでは `x` や `+` が失敗する。`x:y:` と `origin:corner:` はレシーバクラスの instSize を無視して常に 2 スロットで割り当てる。そのため、ivar を足したサブクラスでは、追加した ivar へのアクセスが範囲外として失敗する（境界チェックがあるのでメモリ破壊にはならない）。
- 根拠: コード読解（確度: 高）。
- 修正の方向: `inheritsFrom` で判定し、割り当てサイズはクラスの instSize から取る。

### [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる
- 場所: `runtime/src/kernel/SmallInteger.cpp:196`
- 問題・失敗シナリオ: SPEC は Character を「Unicode スカラー」と定めているが、サロゲートも通ってしまう。`'a', 55296 asCharacter, 'b'` 相当の文字列を Print it すると、出力は `'a` で切れる（printString 自体は 5 文字ある）。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: `0xD800 <= v <= 0xDFFF` を拒否する。

### [Low] `to:do:` の終端が SmallInteger でないと失敗する
- 場所: `runtime/src/kernel/SmallInteger.cpp:171`
- 問題・失敗シナリオ: `1 to: 2.5 do: [...]` と `1 to: (1 bitShift: 70) do: [...]` は AO_ERR_EVAL になる。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: 終端が Float や LargeInteger のときは、`<=` の送信で終了を判定する汎用ループに切り替える。

### [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない
- 場所: `runtime/src/kernel/LargeInteger.cpp:368-381`, `runtime/src/kernel/LargeInteger.cpp:508-518`
- 問題・失敗シナリオ: `box(Heap&, ...)` は `allocate` が失敗すると、そのまま `Oop{}` を返す。唯一の呼び出し元は `Compile.cpp:70` のリテラル箱詰めで、nursery が逼迫した状態で 2^62 以上のリテラルを含むメソッドをコンパイルすると、そのリテラルが空になる。Float リテラルは `allocateRetry` を使うので、扱いが不揃いでもある。
- 根拠: コード読解（確度: 中）。
- 修正の方向: `CallContext` を受け取る `box(ctx, ...)` の経路にそろえる。

## テストの穴
- Magnitude ネイティブの GC ストレステストが無い。Smalltalk で `<` を定義した利用者サブクラスで、`<=` と `between:and:` を nursery GC をまたいで回すものが必要である。Critical の指摘はこれで検出できた。
- Integer をレシーバとする Float / Fraction / 巨大 LargeInteger との混合演算・比較のテストが無い。Float 側のテストは `FloatArithmetic` だけで、どちらも Float 同士である。
- `bitShift:` の境界テストが無い。シフト量が 0、±31〜65、±2^24 超、-2^63 のケースと、負数の右シフトで floor になるケースが必要である。
- LargeInteger の負数どうしの `bitAnd:` / `bitOr:` / `bitXor:`（2 の補数の意味論）と、`quo:` / `rem:` / `//` / `\\` の符号 4 通りを LargeInteger で確かめるテストが無い。
- `kSmiMin // -1`、`kSmiMin quo: -1`、`0 - kSmiMin` のように、境界で昇格・正規化されるケースが明示的なテストに無い（現状の実装では正しい）。
- NaN、±inf を含む比較と `between:and:` のテストが無い。
- Boolean の `and:` `or:` `not` `&` `|` `eqv:` `xor:` のテストが無い（`ifTrue:ifFalse:` しかない）。Boolean 以外の引数を渡すケースも無い。
- Fraction の `=`、比較、Float との混合演算のテストが無い。
- Character の範囲（負数、サロゲート、0x10FFFF、0x110000）のテストが無い。
- int64 を超える整数リテラルと、Float リテラルの丸め（`0.3`、`1e23`、`0e500`）のテストが無い（compiler 側）。
- Point / Rectangle のサブクラスと、成分計算が失敗するケース（`+ nil`）のテストが無い。

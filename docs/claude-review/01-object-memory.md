# 01 オブジェクトメモリ・GC・ブートストラップ

対象: `runtime/include/ao/{Oop,Heap,Gc,Roots,Format,Bootstrap,WellKnown,Globals,Symbol,Runtime,MethodDictionary,Lookup,CompiledMethod,NativeMethod,Natives}.hpp`, `runtime/include/ao/kernel/Install.hpp`, `runtime/src/{Oop,Heap,Gc,Roots,Bootstrap,WellKnown,Globals,Symbol,Runtime,MethodDictionary,Lookup,CompiledMethod,NativeMethod}.cpp`, `runtime/src/kernel/Install.cpp`、および関連テスト（gc / heap_alloc / oop_tag / bootstrap / lookup / method_dictionary / native_method / symbol / compiled_method）

検証: コード読解に加え、`build/` の `libao_runtime.a` / `libao_compiler.a` にリンクしたプローブをスクラッチで実行した（リポジトリとビルドは変更していない）。

- p1: `Heap`/`Gc`/`Roots` を直接使い、スキャベンジ中に old 満杯 → `collectOld` を起こす
- p6: old 枯渇でスキャベンジを失敗させる
- p5 / p8 / p9: `test_support.hpp` の `Boot` を使い、ネイティブと `installMethod` をナーサリ逼迫下で呼ぶ
- `ao_eval` / `ao_accept_method` を使った E2E（churn・大容量割り当て・名前・`Smalltalk`）
- `Gc.cpp` と `MethodDictionary.cpp` に計測だけを足したコピーを差し替えてリンクし、`collectOld` の回数・回収量・親オブジェクトの移動を観測

## 概要
| 重大度 | 件数 |
|---|---|
| Critical | 1 |
| High | 4 |
| Medium | 2 |
| Low | 2 |

## 指摘

### [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む
- 場所: `runtime/src/Gc.cpp:103-126`（とくに `:104` と `:119`）、起点は `:35-36`
- 問題: `scavengeFromRoots` は old オブジェクトのヘッダを `ObjectHeader* h` に取り出してから `h->klass = copy(...)` と `slots[i] = copy(slots[i])` を実行する。`copy()` は old が足りないと `collectOld()` を呼ぶ。`collectOld()` は old を圧縮して `h` の実体を別の番地へ動かす。その後、代入は古い番地へ書き込まれる。直後の `oldCompacted_` チェックで return するが、書き込みはもう済んでいる。
- 失敗シナリオ: old に [ゴミ G(24B)][A(2 slot)][B(1 slot, slot0=42)] があり、A.slot1 だけがナーサリの N を指す状態で `collectNursery()` を呼ぶ。N のコピーで old が溢れ、`collectOld` が G を回収して A と B を前へ詰める。A の旧 slot1 の番地には B の slot0 が来ている。そこへ N のコピー先ポインタが書かれ、B.slot0 は 42 からヒープポインタに変わる。
- 根拠: p1 で再現（`B.slot0 raw=0x10179d888 isSmi=0 (expected SmallInteger 42)`）。E2E でも、計測版 GC で「ゴミの後ろに置かれた長寿命 Array の slot コピー中に `collectOld`」を起こした。親は offset 163712 から前へ動き、旧番地は圧縮後の生存領域の内側にあった（`stale=1`）。Session の old は 4MB 固定で promote-all のため、長いセッションでは普通に起きる（確度: 高）。
- 修正の方向: `copy()` から戻ったら、書き込みの前に `oldCompacted_ || failed_` を確認して即 return する。書き込みを捨てても、再スキャン時に転送済みの N は正しく解決される。あるいはスキャベンジ中は `collectOld` を呼ばず、失敗として外側でやり直す。

### [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる
- 場所: `runtime/src/Gc.cpp:35-46`, `:56-66`
- 問題: `copy()` は元オブジェクトの `klass` を転送先で上書きし、`kFlagForwarded` を立てる。途中で `failed_` になると `collectNursery` はフリップも巻き戻しもせずに終わる。まだ走査していない別経路からは、転送済みの元オブジェクトがそのまま見える。その `klass` は転送先ポインタであり、書き込みはコピーと元に分かれる。割り当ては空 OOP を返し、Smalltalk 側にはエラーとして出ない。
- 失敗シナリオ: root→N1（小）、root→N2（old に入らない大きさ）、root→N3、N3.slot0→N1 の状態で `collectNursery()` を呼ぶ。N1 は old へコピーされ、N2 で失敗する。以後 `klass(N3.slot0)` は N1 のコピー（クラスではない）を返し、N3 経由で書いた値は root 側の N1 から見えない。E2E では 20 万要素の `keep` を作る式が `Message` を返し、エラーにならない。
- 根拠: p6 で再現（`klass(n3.slot0)==cls? 0 (klass is now the forwarding pointer)`, `identity split: n1.slot0=1 n3.slot0.slot0=99`）（確度: 高）。
- 修正の方向: 失敗時は転送を巻き戻す（転送ログを持つ）か、old を拡張してスキャベンジを必ず完了させる。枯渇は OOM として Smalltalk のエラーに上げる。

### [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない
- 場所: `runtime/src/Heap.cpp:32-39`, `runtime/include/ao/Heap.hpp:28`（Session は `Heap heap;` の既定値 `runtime/src/Session.hpp:15`）
- 問題: `Heap::allocate` はナーサリにしか置かない。大きなオブジェクトを old へ直接置く経路がない。`allocateRetry` は GC 後にもう一度ナーサリへ置こうとするだけなので、1MB を超える要求は必ず空 OOP になる。
- 失敗シナリオ: `(Array new: 131071) size` と `(String new: 2000000) size` は `Message` を返す。`OrderedCollection` に 70000 回 `add:` すると、`size` は 65536 で止まる。4464 件が黙って消え、失敗した `add:` のたびにフル GC が走るので 5 秒かかる（60000 件なら 0.4 秒）。
- 根拠: E2E で再現（確度: 高）。
- 修正の方向: 閾値を超える割り当てを old（またはラージオブジェクト領域）に直接置く。置けないときは Smalltalk 側に OOM エラーを返す。

### [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する
- 場所: `runtime/src/Gc.cpp:14-18`, `:35-41`, `:58-61`、`runtime/include/ao/Heap.hpp:28`
- 問題: スキャベンジはナーサリの生存物をすべて old へ昇格させる。そのため old には「生存データ＋ナーサリの生存分」の空きが要る。old は 4MB 固定で拡張しない。生存が約 2.5MB を超えると、`copy()` のたびに何も回収できない `collectOld`（全 mark-compact）が走り、スキャベンジは失敗する。以後はセーフポイントと `allocateRetry` のたびに同じことを繰り返し、割り当ては空 OOP で黙って失敗する。
- 失敗シナリオ: `keep` に N 個の `Array with: k with: k` を保持したまま、40KB の Array を 500 回差し替える。N=20000 は 0.48 秒、N=60000 は 98 秒、N=85000 は 200 秒で打ち切った。計測では `collectOld used 4194296 -> 4194296`（回収 0）が 1 評価内で 88 回続いた。
- 根拠: E2E ＋計測版 GC で再現（確度: 高）。関連する 06 の「old 容量超過のイメージがロード不能」とは別の実行時症状である。
- 修正の方向: old を拡張可能にする（`std::unique_ptr<std::byte[]>` の固定領域をやめる）か、ナーサリ生存物を to-space に残す通常のセミスペース方式にする。回収 0 の `collectOld` の後は同じスキャベンジ内で再試行せず、OOM として一度だけ失敗させる。

### [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない
- 場所: `runtime/src/Bootstrap.cpp:149-156`, `:164`
- 問題: `makeName` は `heap.allocate(Oop::nil(), n, kFlagBytes)` で名前を作る。57 個の Kernel クラスとそのメタクラスの `name` は、クラスを持たないオブジェクトになる。一方、`subclass:` で作ったユーザークラスは Symbol を名前に持つので、扱いが一致しない。
- 失敗シナリオ: `Object name size`、`Object name == #Object`、`Object class name` は、どれも DNU 経由で `Message` を返す。`3 class name` は `AO_ERR_EVAL` になる。`ProbeFoo name size` は 8 を正しく返す。`aStream nextPutAll: self class name` の定番の書き方が Kernel クラスで壊れる。
- 根拠: E2E で再現（確度: 高）。
- 修正の方向: `wireCycle` の後（`symbolClass` が結線された後）に、クラス名を `wk.intern(name)` の Symbol で入れ直す。メタクラスはユーザークラスと同じく String にそろえるか、`name` をメタクラス側で計算する。Browser 側は `byteText` で読んでいるので影響しない。

### [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない
- 場所: `runtime/src/Globals.cpp:75-85`
- 問題: `Globals::install` は `heap.allocate(Oop::nil(), kSmalltalkCount, 0)` で `Smalltalk` を作る。SPEC §3.6 は `SmalltalkImage`（グローバル辞書のホスト）を求めているが、実体はクラスを持たない配列である。ユーザー定義クラスはここに登録されない。
- 失敗シナリオ: メソッド `probeStIsNil ^ Smalltalk isNil` を `Object` に Accept して `3 probeStIsNil` を評価すると、`Message` が返る。`^ Smalltalk at: #Object` も同じである。00 の「knownGlobals に `Smalltalk` が無い」を直しても、この症状は残る。
- 根拠: `ao_accept_method` ＋ `ao_eval` で再現（確度: 高）。
- 修正の方向: `smalltalkImageClass` のインスタンスとし、名前→値の辞書（Dictionary または専用スロット）を持たせる。`at:` / `at:put:` / `includesKey:` をネイティブで用意し、`WellKnown::define` の extra もそこへ反映する。イメージ形式の `checkGlobals` も合わせて直す。

### [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す
- 場所: `runtime/src/MethodDictionary.cpp:37-50`, `:69-72`、呼び出し側 `runtime/src/Compile.cpp:260-273`
- 問題: `growInner` は GC しない `heap.allocate` を使う。ナーサリが足りないと false を返し、`atPut` は `void` のまま何もせず戻る。`installMethod` は `cm.slot` を返し、`acceptMethodSource` も成功として扱う。file-in の経路はセーフポイントを通らず、割り当て失敗時にしか GC しない。そのためナーサリは満杯近くまで使われ、この状態になりやすい。
- 失敗シナリオ: 8 メソッドで満杯の辞書を持つクラスで、ナーサリの残りを 112B にしてから 9 個目を `installMethod` する。`CompiledMethod` が返るが、`lookup(#ninth)` は nil になる。
- 根拠: p8 で再現（確度: 高）。現行の vendor file-in では、計測で脱落 0 件だった。
- 修正の方向: `atPut` を bool にするか `CallContext` を受け取り、`allocateRetry` で拡張する。失敗は `installMethod` から呼び出し元へ伝える。

### [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない
- 場所: `runtime/src/Heap.cpp:45-46`、`runtime/include/ao/Heap.hpp:22`
- 問題: `nextHash_` は 65535 で一周する。SPEC §3.1 は「不足時はサイドテーブル」としているが、実装されていない。
- 失敗シナリオ: 65535 個を超える `IdentitySet` / `IdentityDictionary`、および `identityHash` に頼るハッシュ表で衝突が線形に増える（正しさは保たれ、性能だけが落ちる）。
- 根拠: コード読解（確度: 高）。
- 修正の方向: SPEC どおり、衝突時に拡張ハッシュを持つサイドテーブルを入れる。v1 で見送るなら SPEC 側を直す。

### [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない
- 場所: `runtime/src/Roots.cpp:7-12`, `runtime/src/Gc.cpp:319-339`
- 問題: `collectOld` はルートを旧アドレス→新アドレスの表で書き換える。同じ `Oop*` が 2 回登録されていると、新アドレスをさらに表で引く。それが別オブジェクトの旧アドレスと一致すれば、別物を指すようになる。現状は `ActiveGuard` のコメント（`Interpreter.cpp:199-201`）どおり、慣習だけで避けている。
- 失敗シナリオ: 同じ `InlineCache` を持つ `send` が再入すると、`IcGuard` が同じスロットを二重登録する（現状 `ic` は常に nullptr で未発火）。
- 根拠: コード読解（確度: 中）。
- 修正の方向: `collectOld` のルート更新を「スロット単位で 1 回」に限る（visited 集合）か、`Roots::add` の重複を debug assert で弾く。

## GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答

- ネイティブ向けのハンドルスコープは無い。`NativeFn` は `Oop receiver` を値で受け取る（`NativeMethod.hpp:57`）。`send` → `applyMethod` → `NativeMethod::apply`（`NativeMethod.cpp:120`）のどこでも、receiver はルートされない。呼び出し元 `performSend` の `Root rcvr` は更新されるが、ネイティブが持つのはそのコピーなので古いままになる。
- `args` は、インタプリタから来たときだけ `RootedArray`（`Interpreter.cpp:26-45`）を指し、GC で更新される。C++ から `send(ctx, x, sel, &local, 1, …)` で呼ぶと、ルートされていないローカルを指す（例: `Magnitude.cpp` の `sendBin`、`Session.cpp` の `dictAtPutKey` の `Oop args[2]`）。受け側のネイティブは入口で `Root` に取っているので、現状は発火しない。
- 保護手段は `Roots::add/remove(Oop*)` だけである。RAII の `Root` は 16 ファイルに匿名名前空間で重複定義され、公開ヘッダに無い。`Roots::pushHandle` はネイティブから使われていない。
- 同じ型の既報サイト: `ao_Magnitude_lessOrEqual` / `between:and:`（03）、`ao_Class_subclass_…`（04。p5 で親クラス slot がナーサリの旧番地を指すことを再確認）、`ao_Symbol_asString`（04）。`runtime/src/kernel/*.cpp` と `runtime/src/*.cpp` の全関数をヒューリスティックに走査した（GC し得る呼び出しの後で、ルートしていない receiver・引数・ローカル Oop を使う箇所）。この 3 件以外に新しい該当は見つからなかった（残りは戻り値の使用か SmallInteger で、誤検知）。
- 修正の方向: `ao/HandleScope.hpp` のような共通ヘッダに `Root` / `RootedArray` をまとめる。`NativeMethod::apply` で receiver と args を 1 回ルートし、ネイティブにはルート済みスロットの参照を渡す形にすれば、個別のルート漏れを構造的に防げる。

## テストの穴
- `collectNursery` の途中で old が溢れ、ゴミの後ろにある old 親が移動するケースのテストが無い（Critical がすり抜けた）。`GcOld` はすべて `collectOld` の単独呼び出しで試している。
- old 枯渇でスキャベンジが失敗した後の不変条件（転送済みオブジェクトが残らない、同一性が保たれる）のテストが無い。`OldOomDoesNotClobberRoot` は root の値しか見ていない。
- ナーサリ半面を超える割り当て、および old が 60〜90% 生存の状態での churn（性能退行）のテストが無い。
- 「GC を毎回割り当てで起こす」ストレスモードが無い。ネイティブはナーサリに余裕がある状態でしかテストされず、未ルートの receiver・ローカルを検出できない。
- ブートストラップ後に、Smalltalk から `Object name` / `Smalltalk at:` を送るテストが無い。`bootstrap_test` は C++ からスロットを直接読むだけである。
- `MethodDictionary::atPut` の拡張失敗（ナーサリ逼迫）時の挙動のテストが無い。
- `Oop::fromSmallInteger` は範囲外の値を黙って折り返す。debug assert も、範囲外入力のテストも無い（現状の呼び出し側は範囲を確認している）。

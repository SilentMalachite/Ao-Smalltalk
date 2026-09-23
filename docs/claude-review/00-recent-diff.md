# 00 直近差分（origin/main..HEAD の 6 コミット）
対象: `origin/main..HEAD`（b6d0cd4 〜 1ccf79a）。`graphify-out/` は除外
検証: 各指摘の「根拠」欄のとおり。プローブは、ビルド済みの `libao_runtime.a` に対して実行した

## 概要
| 重大度 | 件数 |
|---|---|
| Critical | 5 |
| High | 8 |
| Medium | 1 |
| Low | 1 |

## 指摘

### [Critical] 既存クラスの定義を Accept すると全メソッドが消える
- 場所: `runtime/src/Compile.cpp:435`
- 問題: `acceptClassSource` は `subclass:instanceVariableNames:...` を送り、新しいクラスを作って名前を付け替える。定義文が同じでも既存メソッドは引き継がれない。
- 失敗シナリオ: Foo2 を定義し `Foo2>>m ^42` を Accept → `Foo2 new m` は 42。同じ定義文を `ao_accept_class` で再 Accept → `Foo2 new m` は doesNotUnderstand。Browser でセレクタ未選択のまま Accept するだけで起き、戻り値は `AO_OK`。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 既存クラスがあれば形状変更だけを適用し、メソッド辞書を引き継ぐ。

### [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる
- 場所: `runtime/src/Compile.cpp:436`
- 問題: `fileInString` / `applyChunks` は DoIt チャンクを無言でスキップする。そのためメソッド本体や式を渡しても何も登録されず、`AO_OK` が返る。
- 失敗シナリオ: `ao_accept_class("zork\n  ^5\n")` も `ao_accept_class("3 + 4")` も `AO_OK` を返す。新しいクラスにはセレクタがないので、`BrowserWindow.submit` は必ず `ao_accept_class` 側に回る。最初のメソッドを書いて Accept しても登録されず、エラーも出ない。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: クラス定義メッセージ以外はエラーにする。App 側はクラス定義とメソッドを区別して送る。

### [Critical] 自分を含む Array の `=` でスタックオーバーフロー
- 場所: `runtime/src/kernel/Array.cpp:100`
- 問題: `ao_Array_equals` に同一性の短絡がなく、クラスも Array ちょうどでないと等しくならない。
- 失敗シナリオ: `| a | a := Array new: 1. a at: 1 put: a. a = a` → segfault（exit 139）。Print it でアプリが落ちる。`Array subclass: #MyArr` のインスタンスでは `a = a` が false になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 先頭で `==` を判定して true を返す。クラスの比較は `class ==` に揃える。

### [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み
- 場所: `runtime/src/Compile.cpp:369`
- 問題: `wk.named(className)` が返したものを、検査せずにクラスとして扱う。
- 失敗シナリオ: `ao_accept_method("Processor", 0, "foo ^1")` → `Assertion failed: (i < h->size), function slotAt`。リリースビルドではヒープの範囲外に書き込む。`"Smalltalk"` を渡すと、グローバル配列のスロットをメソッド辞書とみなして `atPut` を呼ぶ。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 名前の解決結果が Behavior であることを確かめ、違えばエラーを返す。

### [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える
- 場所: `runtime/src/Session.cpp:469`
- 問題: ソースを保存していないメソッドでは、コンパイルできるプレースホルダが表示される。対象はイメージ読み込み後のすべてのメソッドと、vendor・file-in のメソッド。
- 失敗シナリオ: Open Image のあとや `Date>>printOn:` では、`printOn:\n  "CompiledMethod"\n` と表示される。これを Accept すると self を返すだけの空メソッドに置き換わる。
- 根拠: コード読解（確度: 高）
- 修正の方向: ソースがなければペインを読み取り専用にするか、コンパイルできない表示にする。

### [High] ClassMethodCache の無効化が定義クラスの分だけ
- 場所: `runtime/src/Compile.cpp:423`
- 問題: キャッシュのキーは受信側のクラスだが、無効化するのは (定義クラス, セレクタ) だけ。
- 失敗シナリオ: `Object>>zork ^1` を Accept → `3 zork` は 1。`^2` で再 Accept しても、`3 zork` は 1 のまま（`Object new zork` は 2）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: セレクタ単位、またはキャッシュ全体を無効化する。

### [High] 一度未宣言だった名前が、後から定義したクラスを隠す
- 場所: `compiler/src/Codegen.cpp:97`
- 問題: Workspace 辞書のキーを knownGlobals と照合せずに temp として追加している。
- 失敗シナリオ: Print it `Foo` → nil（束縛が作られる）。そのあと `Object subclass: #Foo ...` を Accept しても、`Foo` は nil のまま。
- 根拠: コード読解（確度: 高）
- 修正の方向: knownGlobals に含まれる名前は temp にしない。または束縛を削除する。

### [High] `Smalltalk` が knownGlobals に含まれない
- 場所: `runtime/src/Session.cpp:606`
- 問題: `WellKnown::named` は `Smalltalk` を解決できるのに、`collectKnownGlobals` がこれを含めていない。SPEC の一覧にも同じ漏れがある。
- 失敗シナリオ: Print it `Smalltalk` → `nil`。`Smalltalk at: #Zap put: 3` → doesNotUnderstand。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `Smalltalk` を追加し、SPEC も直す。

### [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅
- 場所: `compiler/src/Codegen.cpp:115`
- 問題: 過去の束縛をすべて、毎回 temp として追加している。
- 失敗シナリオ: `vN := N` を 300 回評価すると、251 回目以降は `too many temporaries` で失敗する。App は `ao_workspace_reset` を呼ばない。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 束縛は temp ではなく辞書経由で読み書きする。App に Reset を用意する。

### [High] printOn: が新しい printString を使わない
- 場所: `runtime/src/kernel/Object.cpp:377`
- 問題: `ao_Object_printOn_` が `ao_Object_printString` を直接呼んでいる。
- 失敗シナリオ: `3 printOn: s` の結果が `'SmallInteger'` になる。`Date>>printOn:` の出力は `SmallInteger/SmallInteger/...` になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `printOn:` から `#printString` を送る。

### [High] out == NULL の Do it が副作用ありで AO_ERR を返す
- 場所: `runtime/src/Session.cpp:768`
- 問題: 入力検査は通るが、実行後の `writeBuf("")` が失敗する。
- 失敗シナリオ: `ao_eval("y := 3", 6, AO_EVAL_DOIT, nullptr, 0, &err)` の戻り値は 1 なのに、`y` は 3 になっている。呼び出し側が再試行すると副作用が二重に起きる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: out が NULL のときは書き込みを省く。または実行前に拒否する。

### [High] `| q |` の宣言 temp が Workspace 束縛と混ざる
- 場所: `compiler/src/Codegen.cpp:103`
- 問題: 宣言した temp が、同じ名前の束縛の値で初期化され、代入も書き戻される。
- 失敗シナリオ: `q := 7` → Print it `| q | q` の結果が 7 になる（期待値は nil）。
- 根拠: コード読解（確度: 高）
- 修正の方向: 宣言した temp は束縛の対象から外す。

### [High] Save / Open Image の失敗がユーザーに見えない
- 場所: `app/Ao/AoApp.swift:81`
- 問題: `ao_image_save` / `ao_image_load` の戻り値を捨てている。
- 失敗シナリオ: 読み取り専用の場所に保存したり、イメージでないファイルを開いたりしても、何も表示されない。
- 根拠: プローブで確認（確度: 高）
- 修正の方向: 失敗したら NSAlert を出す。

### [Medium] eval ごとに knownGlobals と束縛全体を作り直す
- 場所: `runtime/src/Session.cpp:735`
- 問題: 毎回、グローバル名の再構築、線形探索、束縛ごとの `at:` / `at:put:` 送信が走る。
- 失敗シナリオ: 束縛が 250 個あると、`1 + 2` の評価だけで `=` の送信が約 6 万回起き、それがメインスレッドで実行される。
- 根拠: コード読解（確度: 中）
- 修正の方向: 名前集合をキャッシュし、辞書をネイティブで直接読む。

### [Low] Dictionary の内部スロット位置を直書き
- 場所: `runtime/src/Session.cpp:624`
- 問題: `slotAt(session.workspace, 1)` と `pointerSlots(..., 2)` を直書きしている。CLAUDE.md の「マジックナンバーを well-known 表か定数ヘッダへ」に反する。
- 失敗シナリオ: `kernel/Dictionary.cpp` のレイアウトを変えると、キーの列挙が黙って壊れる。
- 根拠: コード読解（確度: 高）
- 修正の方向: レイアウト定数を共有ヘッダに移す。

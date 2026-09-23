# Ao — macOS ネイティブ Smalltalk 処理系 SPEC

## 0. 結論

Ao は、Apple Silicon 上の **新規** Smalltalk 処理系である。Xerox Smalltalk-80 Blue Book（Goldberg & Robson, 1983）の **言語意味論・オブジェクトモデル・開発ツール契約** に準拠する。Blue Book 第 27–30 章の 16-bit OOP / セグメントメモリ / 参照カウント実装は再現しない。

v1 の完成定義は次の三点が同時に成り立つことである。

1. Blue Book の核となるクラス階層とメッセージ意味論が動く。
2. Kernel カテゴリのクラスとメソッドは **すべてネイティブコード**（インタプリタを通らない）。
3. macOS ネイティブの System Browser / Transcript / Workspace から、ソースの受け入れ・評価・閲覧ができる。

新規実装なのは **VM（オブジェクトメモリ・ディスパッチ・ネイティブ Kernel・コンパイラ・AppKit）** だけである。Squeak / Pharo / OpenSmalltalk-VM / VisualWorks の **VM ソースの移植・埋め込みは禁止**する。クラスライブラリは自作しない（§3.12）。

---

## 1. 前提

### 1.1 動機

既存の Smalltalk は次のいずれかである。

- バイトコード VM + 巨大なイメージ（Squeak / Pharo）
- クラシック BitBlt 画面をホストにエミュレートする Blue Book VM（dbanay ほか）

Ao はどちらでもない。ホストは macOS / AppKit。実行のホットパスは ARM64 ネイティブ関数。ユーザー定義メソッドだけがコンパイラ成果物（v1 はバイトコード、後続でネイティブ JIT）になる。

### 1.2 準拠の定義

「ブルーブック準拠」は次を指す。次を指さないものも明示する。

| 準拠する | 準拠しない |
|---|---|
| すべてがオブジェクト。すべてはメッセージ送信 | 16-bit OOP、64K オブジェクト上限 |
| 単一継承。メソッド探索はスーパークラス鎖 | セグメントメモリ、オブジェクトテーブル必須 |
| クラスはオブジェクト。各クラスに対応するメタクラス | 1983 Xerox 仮想イメージのロード |
| `Behavior` / `ClassDescription` / `Class` / `Metaclass` の役割分担 | Green Book の特定ハードウェア前提 |
| ブロックは `BlockContext` 相当のクロージャ | MVC / BitBlt / Form を唯一の GUI とする |
| `doesNotUnderstand:`、`perform:`、`#==` / `#~~` | クラシック 808×606 ピクセル画面 |
| Transcript / Workspace / System Browser の契約 | 既存 .image バイナリ互換 |
| Smalltalk-80 ソース構文（カスケード、キーワードセレクタ、ブロック） | ANSI Smalltalk の完全サブセット宣言（後続） |

言語リファレンスの一次資料は Blue Book Part 1（言語）と Part 4 のうち意味論に関わる記述。実装リファレンスの一次資料は Part 4 のバイトコード集合とプリミティブ集合の **意味** であり、アルゴリズムの複製ではない。

### 1.3 性能方針

- Kernel メソッドは C++ で書き、コンパイル時に ARM64 ネイティブコードになる。
- メッセージ送信の通常パスは、レシーバクラスのメソッドキャッシュ（インラインキャッシュ 1 段 + クラス側メガモーフィックキャッシュ）を通る。
- SmallInteger 算術、`#==`、`#class`、配列アクセス、ブロック評価のホットパスにインタプリタを介入させない。
- オブジェクトポインタは 64-bit tagged pointer。オブジェクトテーブルを介さない直接ポインタ。

### 1.4 非目標（v1）

- 1983 Xerox 仮想イメージ（`.im`）のバイナリロード。ソースの file-in とは別物
- クラスライブラリの自作（既存の許諾付き `.st` を取り込む。§3.12）
- ネットワーク、FFI の一般化、Objective-C ブリッジの全面公開
- デバッガ / チェンジセット UI / モンティチェロ相当
- JIT（ユーザーメソッドのネイティブ化）。設計上の差し込み口だけ用意する
- Windows / Linux
- SwiftUI を主 UI にすること（AppKit が正。SwiftUI は補助のみ）

---

## 2. 範囲

### 2.1 成果物

| 成果物 | 内容 |
|---|---|
| `Ao.app` | macOS アプリケーション。System Browser / Transcript / Workspace |
| `libao_runtime` | C++20 ランタイム。オブジェクトメモリ、GC、ディスパッチ、Kernel ネイティブメソッド、バイトコードインタプリタ |
| `ao` CLI | ヘッドレス評価。ファイル読み込み、テストランナー |
| スナップショット | `.aoimage`。ヒープとグローバル辞書の保存 / 復元 |
| 本 SPEC と `CLAUDE.md` | 実装の正本。チャットは正本にしない |

### 2.2 リポジトリ構成

```
ao-smalltalk/
  SPEC.md
  CLAUDE.md
  AGENTS.md
  README.md              # GitHub 案内（英語が正本）
  README.ja.md
  LICENSE                # Apache License 2.0（英語）
  NOTICE
  graphify-out/          # Graphify 成果。コミットする
  runtime/               # C++20 — オブジェクトメモリと Kernel
    include/ao/
    src/
    tests/
  compiler/              # C++20 — 字句 / 構文 / バイトコード生成
  image/
    Kernel/              # コメントとプロトコルのみ。実行定義は C++
    vendor/              # 取り込み元 .st（ライセンス原文を同梱）
    patches/             # vendor 差分（Ao 向け最小パッチ）
    Tools-model/         # ホスト GUI が使うモデルだけ、必要なら自前
    tests/
  app/                   # Swift 6 + AppKit
    Ao/
    AoTests/
  bridge/                # C ABI。runtime ↔ app
  docs/
  scripts/
```

### 2.3 フェーズ

実装はフェーズを飛ばさない。各フェーズの受け入れを満たしてから次へ進む。

| フェーズ | 名前 | 出すもの |
|---|---|---|
| P0 | 骨格 | リポジトリ、ビルド、Graphify 初期グラフ、Serena プロジェクト、空のテスト |
| P1 | オブジェクトメモリ | tagged OOP、ヒープ、割り当て、正確な GC |
| P2 | ブートストラップ | Object〜Metaclass の循環、よく知られたオブジェクト |
| P3 | ネイティブディスパッチ | メッセージ送信、NativeMethod、メソッドキャッシュ |
| P4 | Kernel ネイティブ実装 | 下記 3.6 のクラス群をネイティブメソッドで充足 |
| P5 | コンパイラ | .st → AST → CompiledMethod（バイトコード） |
| P6 | インタプリタ | ユーザーメソッド実行、コンテキスト、ブロック |
| P6b | vendor file-in | `image/vendor` を LOAD_ORDER どおり載せる |
| P7 | イメージ | `.aoimage` の save / load、起動 |
| P8 | AppKit ツール | System Browser / Transcript / Workspace |
| P9 | 統合 | Do it / Print it / accept、階層閲覧、エラー表示 |

P9 完了が v1。

---

## 3. 仕様

### 3.1 オブジェクトポインタ（OOP）

64-bit 値。下位ビットでタグを分ける。

```
bit 0 = 1          → SmallInteger。値は符号付き 63-bit（算術右シフト 1）
bit 1:0 = 10       → Character。Unicode スカラー値を上位に格納
bit 2:0 = 100      → 即値シングルトン（nil / true / false を 3 パターンで符号化）
bit 2:0 = 000      → ヒープオブジェクト。8 バイト整列ポインタ
```

即値はヒープを持たない。`#class` はタグからクラスオブジェクトを返す。`#==` はビット列の比較（即値もヒープも同じ）。

ヒープオブジェクトのヘッダ（最小 16 バイト、必要なら 24）:

| フィールド | 幅 | 内容 |
|---|---|---|
| class OOP | 64 | クラスへの tagged/heap OOP |
| size | 32 | スロット数、またはバイト数 |
| flags | 16 | ポインタオブジェクトか、弱参照か、旧世代か、不動か |
| hash | 16 | identity hash の短縮形。不足時はサイドテーブル |

可変長オブジェクト（Array, String, ByteArray, CompiledMethod）はヘッダ直後にペイロードを置く。

### 3.2 オブジェクトメモリと GC

- アロケータは世代別（nursery bump + 旧世代 free-list または immix 系）。v1 は nursery + mark-compact old でよい。
- ルート: グローバル辞書 `Smalltalk`、現在のプロセス／コンテキスト連鎖、AppKit が保持するハンドル表、イメージ起動時の well-known 表。
- 正確式。コンパイラとインタプリタはスタック上の OOP を GC に報告する。
- ファイナライザと弱配列は v1 では `ephemeron` なしの弱スロットフラグまで。
- スレッド: ミューテータは基本 1 本（Smalltalk プロセスはグリーンユーザースレッド）。GC は safepoint。AppKit メインスレッドとはブリッジキューで切る。
- old は、上限 4 GiB − 1 MiB（イメージヘッダの `uint32 heapBytes` で表せる最大値を、コミット単位の 1 MiB に揃えた値）の仮想領域を 1 つ予約し、必要な分だけコミットする。old のアドレスは動かない。
- 大きなオブジェクトは nursery を通さず old に直接置く。
- スキャベンジは失敗しない。old に入り切らない生存物は to-space に残す（昇格失敗）。
- スキャベンジはルートから届く old をすべてたどる。たどった量が `oldUsed` より少なければ、old に死んだオブジェクトがある。直前の full GC が動かせないオブジェクトの前に残した穴は、死んだオブジェクトに数えない。
- full GC を走らせる契機は次の 4 つだけとする。`threshold = max(初期容量, 2×生存量)` を old の上限で頭打ちにした値。
  - スキャベンジの後で `oldUsed > threshold` のとき。
  - GC を走らせてよい割り当て（`allocateRetry`）が大きなオブジェクトを old に直接置く前で、`oldUsed + そのサイズ > threshold` のとき。スキャベンジを先に行う。
  - `allocateRetry` が old の上限で失敗したとき。その呼び出しの中でまだ full GC が走っていなければ、full GC を 1 回走らせてから、割り当てを 1 回やり直す。`allocateRetry` 1 回が諦めるまでに走らせる full GC は 1 回までとする。old の上限より大きな要求は、GC せずに失敗にする。
  - スキャベンジが昇格に失敗し、かつ old に死んだオブジェクトを見つけたとき。old が上限に近いと閾値も上限に張り付き、第 1 の契機が成り立たないまま、同じ生存物を to-space に残すスキャベンジが続く。死んだオブジェクトが無ければ走らせないので、回収 0 の full GC を繰り返さない。
- safepoint は、nursery の空きが半面の 1/8 を下回ったときにスキャベンジする。ただし、直前のスキャベンジが昇格に失敗し、その後 nursery への割り当ても full GC も無いときは、スキャベンジしない。同じ生存物を to-space に残すだけで、進捗が無い。
- GC を走らせない割り当て（メソッド辞書の作成・拡張、Symbol の intern、NativeMethod の作成）は、nursery に置き、nursery が満杯なら old に置く（`asSymbol` の Symbol は old に直接置く）。失敗するのは old の上限のときだけである。
- それでも割り当てられないとき（GC を走らせない割り当てが old の上限で失敗したときを含む）は、評価エラー「out of memory」にする。
- ネイティブが受け取る receiver と引数は、ルート済みとする。ネイティブの途中で GC が走っても、転送先を指す。

### 3.3 メッセージ送信

送信 `receiver selector: args` の意味は Blue Book に従う。

1. `receiver` のクラス `C` を取る。
2. `C` からスーパークラス鎖を `Object` まで辿り、メソッド辞書でセレクタを探す。
3. 見つかればそのメソッドを適用する。
4. 見つからなければ `receiver doesNotUnderstand: aMessage` を送る。
5. `super` 送信は、メソッドが定義されたクラスのスーパークラスから探索を始める。

スーパークラスの枠は `instVarAt:put:` で何にでも書き換えられるので、鎖が壊れていることがある。鎖をたどる処理（送信の探索、ネイティブ、コンパイラ）は、どれも次の規則で止まる。

- 鎖は、`nil`、クラスの形をしていないもの（即値、バイト列、クラスの枠をすべて持たないポインタオブジェクト）、すでに通ったクラス（循環）、1024 段目、のどれかに当たったところで終わる。その先は無いものとして扱う。
- 送信: 鎖が終わるまでにセレクタが見つからなければ `doesNotUnderstand:` を送る。`doesNotUnderstand:` も見つからなければ、送信の値は作った `Message` とする。
- `isKindOf:` と `inheritsFrom:` は、鎖が終わるまでに引数に当たらなければ `false`。`respondsTo:` は、見つからなければ `false`。
- `instVarNamed:` とコンパイラのインスタンス変数名は、鎖が終わるまでのクラスの変数名だけを集める。`instVarNamed:` は、名前が無ければ `error:` で失敗する（名前が無いときと同じ）。

実装上の通常パス:

```
lookup(receiver, selector)
  → inline cache (予想クラス一致なら即ジャンプ)
  → class method cache
  → 辞書探索（Symbol は identity）
  → doesNotUnderstand:
```

メソッドは 2 種類だけ持つ。

| 種類 | 表現 | 適用 |
|---|---|---|
| `NativeMethod` | C++ 関数ポインタ + 引数個数 + プリミティブ番号（任意） | Kernel の全メソッド |
| `CompiledMethod` | バイトコード + リテラル枠 + 一時変数数 | ユーザー定義、および Kernel 以外 |

`NativeMethod` は Smalltalk オブジェクトとしても存在する（Inspect できる）が、実行は関数ポインタ直呼び。バイトコードインタプリタに入らない。

セレクタは `Symbol`。同一文字列は intern され `#==` が成り立つ。

### 3.4 コンテキストとプロセス

v1 の実行モデル:

- ユーザーメソッドは `MethodContext`（Blue Book `Context` / `MethodContext` 相当）。
- プロセスは協調的。`ProcessorScheduler` / `Process` / `Semaphore` を Kernel に含める。
- ホストのプリエンプションは使わない。長時間ネイティブメソッドは safepoint を自ら発行する。

#### クロージャと共有 temp

- ブロック式を評価すると、クロージャ（`BlockContext` の形のオブジェクト）ができる。クロージャは、メソッド、レシーバ、ホーム（そのブロックを字句上含むメソッドのコンテキスト）、コピーした値の Array を持つ。sender は nil。
- ブロックは外側の temp を共有する（Blue Book の意味論）。ブロックが捕捉した temp のうち、どこかで代入されるものは、コンパイラが temp ベクタ（ヒープの Array）に置く。ベクタは、その temp を宣言したスコープ（メソッド、ブロック、展開したブロック）に入るたびに新しく作る。捕捉したブロックはベクタそのものをコピーするので、外側とブロックが同じ値を読み書きする。ブロック内の代入は、外側にも、同じブロックの次の起動にも見える。
- 代入の無い捕捉（引数、`to:do:` のループ変数、代入の無い temp）は、クロージャを作った時点の値をコピーする。
- ブロックを起動するたびに、新しいアクティベーション（`BlockContext`）を作る。アクティベーションの sender は呼び出し元である。クロージャ自身は書き換えない。
- コンテキストは、そのフレームを抜けるときに pc と sender を nil にする。これが死んだコンテキストの印である。

#### 非局所リターン、ensure:、abort

- ブロック内の `^` は、ホームのメソッドから返る（非局所リターン）。途中のフレームはネイティブも含めてすべて巻き戻す。ブロックを呼んだネイティブは、巻き戻しの最中なら直ちに空 OOP を返し、残りの反復も副作用も行わない。
- ホームが死んでいれば、ブロックのアクティベーションに `cannotReturn: 値` を送り、その答えをブロックの値として呼び出し元に返す。無関係な呼び出し元は巻き込まない。`BlockContext>>cannotReturn:` の既定は `self error: 'cannot return'` と同じ答えである。
- `ensure: aBlock` は、レシーバのブロックを評価したあと、正常に終わっても巻き戻しの途中でも `aBlock` を評価する。後始末の間は巻き戻しを止め、終わったら続ける。後始末が自分で巻き戻しを始めたら、そちらを優先する。`ifCurtailed: aBlock` は、レシーバのブロックが巻き戻ったときだけ `aBlock` を評価する。
- abort は評価の中断である。ホームの無い非局所リターンとして扱い、どのフレームでも止まらずに最外（`ao_eval` の 1 回、`ao --test` の 1 ファイル）まで戻る。途中の `ensure:` は実行する。abort は理由の文字列を持つ。abort を始めるのは `abortEvaluation` だけである。
- abort の状態は最外で読んで消す。前の評価の abort を次の評価に持ち越さない。
- スタックガード: メソッド（ネイティブを含む）を適用する前に、C スタックの残りが予約分（`min(512 KiB, スタックの大きさの 1/4)`）を下回っていれば、「stack overflow」で abort する。無限再帰でプロセスは落ちない。
  - スタックの範囲は、最外の入口（`ao_eval`、`ao --test` の 1 ファイル、最外の `Interpreter::run`）で必ず取り直す。前のスレッドの範囲を使い続けない。
  - `ensure:` / `ifCurtailed:` の後始末の間は、予約分の半分まで使ってよい。stack overflow の abort の途中でも、限界近くの後始末が走る。

### 3.5 バイトコード（ユーザーメソッド）

Blue Book 第 28 章の集合を現代化した **Ao バイトコード** を定義する。意味は対応づけ、エンコーディングは 8-bit opcode + 必要なら 1–2 オペランド。

オペコード表（番号は追記だけで増やし、既存の番号を変えない）:

| # | Op | オペランド | 意味 |
|---|---|---|---|
| 0–8 | PushReceiver, PushTrue, PushFalse, PushNil, PushThisContext, PushMinusOne, PushZero, PushOne, PushTwo | — | 定数を積む |
| 9 | PushTemp | i | temp i を積む |
| 10 | PushInstVar | i | インスタンス変数 i を積む |
| 11 | PushLiteral | lit | リテラルを積む |
| 12 | PushGlobal | lit（Symbol） | その名前のグローバルを積む（無ければ nil） |
| 13 / 14 | Pop / Dup | — | |
| 15 / 16 | StoreTemp / StoreInstVar | i | 積んだまま書く |
| 17 / 18 | PopStoreTemp / PopStoreInstVar | i | 降ろして書く |
| 19 | Send | lit argc | 通常送信 |
| 20 | SendSuper | lit argc | super 送信 |
| 21 | SendSpecial | k argc | 特殊セレクタ表の k 番を送る |
| 22 | Jump | rel16 | 無条件ジャンプ |
| 23 / 24 | JumpTrue / JumpFalse | rel16 | 降ろした値が true / false ならジャンプ |
| 25–29 | ReturnReceiver, ReturnTrue, ReturnFalse, ReturnNil, ReturnTop | — | メソッドから返る |
| 30 | ReturnBlock | — | ブロックの `^`（非局所リターン） |
| 31 | CreateBlock | lit n | 積んだ n 個をコピーしてクロージャを作る |
| 32 | Primitive | u16 | 先頭のプリミティブ番号 |
| 33 | PushNewArray | n | nil で埋めた大きさ n の Array を積む |
| 34 / 35 / 36 | PushRemoteTemp / StoreRemoteTemp / PopStoreRemoteTemp | i t | temp t の Array（temp ベクタ）の i 番を読み書きする |
| 37 / 38 / 39 | PushLitVar / StoreLitVar / PopStoreLitVar | lit（Association） | 束縛の値を読み書きする |

- ジャンプのオフセットは符号付き 16 bit（リトルエンディアン）で、基点は次の命令の先頭である。後方ジャンプは、条件ジャンプも含めて safepoint を通る。
- JumpTrue / JumpFalse で降ろした値が Boolean でなければ、その値に `mustBeBoolean` を送る。答えが Boolean ならそれで分岐し、Boolean でなければ「NonBoolean receiver」で abort する。`Object>>mustBeBoolean` の既定は、同じ理由の abort である。
- LitVar 系のリテラルは Association（値のスロットを持つポインタオブジェクト）でなければならない。ワークスペースの束縛（§3.10）とクラス変数が使う。

コンパイラが生成し、インタプリタが実行する。Kernel はこれを使わない。

インライン展開: 次の送信は、送信せずにジャンプへ展開する。展開するのは、コンパイラがレシーバ自身をコンパイルする送信（カスケードのパートでも super 送信でもない）で、ブロック引数がリテラルのときだけである。それ以外は通常の送信にする。Boolean などのネイティブは残り、展開しない送信で使う。

| 送信 | 条件 | 値 |
|---|---|---|
| `ifTrue:` `ifFalse:` `ifTrue:ifFalse:` `ifFalse:ifTrue:` | 全引数が 0 引数のリテラルブロック | 選んだブロックの値。選ぶ分岐が無ければ nil |
| `and:` `or:` | 引数が 0 引数のリテラルブロック | `and:` はレシーバが false なら false、`or:` は true なら true。それ以外は引数ブロックの値 |
| `whileTrue:` `whileFalse:` | レシーバと引数が 0 引数のリテラルブロック | nil |
| `whileTrue` `whileFalse` | レシーバが 0 引数のリテラルブロック | nil |
| `to:do:` | 最後の引数が 1 引数のリテラルブロック | レシーバ |
| `to:by:do:` | 上に加えて、刻みが 0 でない数値リテラル | レシーバ |

- `to:do:` の上限は 1 回だけ評価する。反復は、判定 → 本体 → 増分 → 判定の順である。判定は `<=`（刻みが負なら `>=`）、増分は `+` を送る。レシーバは SmallInteger でなくてよい。刻みの `0.0` も 0 とみなし、展開しない。
- 展開したブロックの temp は、そのブロックに入るたびに（ループなら反復ごとに）nil から始まる。ループ変数を捕捉したクロージャは、作った反復の値を持つ。
- 展開したブロック内の `^` と `thisContext` は、そのブロックを含む実スコープ（メソッドか、展開しないブロック）のものと同じ意味である。

JIT 差し込み口: `CompiledMethod` に `nativeCode` スロットを予約し、v1 では常に `nil`。

### 3.6 Kernel クラス（ネイティブ実装必須）

次のクラスの **インスタンスメソッドとクラスメソッドはすべて NativeMethod** とする。Smalltalk ソースで書いてインタプリタ実行してはならない。対応する `.st` ファイルは **コメントとプロトコル分類のためだけ** に置いてよい。実行定義は C++。

#### Kernel-Objects

- `Object`
- `UndefinedObject`（`nil`）
- `Boolean`, `True`, `False`

必須セレクタ（最小。実装時に Blue Book プロトコルを充足して増やす）:

`Object`: `class`, `==`, `~~`, `=`, `hash`, `identityHash`, `yourself`, `isNil`, `notNil`, `ifNil:`, `ifNotNil:`, `perform:`, `perform:with:`, `perform:withArguments:`, `doesNotUnderstand:`, `error:`, `subclassResponsibility`, `shouldNotImplement`, `isKindOf:`, `isMemberOf:`, `respondsTo:`, `copy`, `shallowCopy`, `instVarAt:`, `instVarAt:put:`, `instVarNamed:`, `basicSize`, `basicAt:`, `basicAt:put:`, `printString`, `printOn:`, `storeOn:`, `inspect`（ホスト Inspector を開くブリッジ）, `mustBeBoolean`

`Boolean`: `ifTrue:`, `ifFalse:`, `ifTrue:ifFalse:`, `ifFalse:ifTrue:`, `and:`, `or:`, `not`, `&`, `|`, `eqv:`, `xor:`

#### Kernel-Classes

- `Behavior`
- `ClassDescription`
- `Class`
- `Metaclass`

必須セレクタ:

`Behavior`: `new`, `basicNew`, `basicNew:`, `superclass`, `methodDict`, `selectors`, `compiledMethodAt:`, `includesSelector:`, `inheritsFrom:`, `instSize`, `isVariable`, `isBytes`, `isPointers`

`Class`: `subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:`, `name`, `category`, `classPool`

`Metaclass`: `thisClass`, クラス側 `new` の禁止または制御

メタクラス循環は Blue Book ルール 6–10 を満たすこと。

- すべてのクラスはメタクラスのインスタンス
- メタクラス階層はクラス階層に平行
- すべてのメタクラスは `Class` を継承（`Object class` のスーパークラスが `Class`）
- すべてのメタクラスは `Metaclass` のインスタンス
- `Metaclass class` も `Metaclass` のインスタンス

#### Kernel-Methods

- `CompiledMethod`
- `NativeMethod`（Ao 拡張。Blue Book に無いが Kernel の表現に必要）
- `Message`
- `MethodDictionary`
- `BlockContext` / `MethodContext`（または統一 `Context` + フラグ）

必須セレクタ:

`BlockContext`: `value`, `value:`, `value:value:`, `value:value:value:`, `value:value:value:value:`, `valueWithArguments:`（引数は Array とそのサブクラスだけ）, `numArgs`, `whileTrue:`, `whileFalse:`, `whileTrue`, `whileFalse`, `repeat`, `ensure:`, `ifCurtailed:`, `cannotReturn:`

ブロックを呼ぶネイティブは、ブロックを呼ぶたびに巻き戻しを確かめる（§3.4）。ネイティブのループは 64K 回ごとに safepoint を通る。

#### Kernel-Processes

- `Process`
- `ProcessorScheduler`（グローバル `Processor`）
- `Semaphore`
- `SharedQueue`（最小）

#### Collections

ネイティブ必須:

- `Collection`, `SequenceableCollection`, `ArrayedCollection`
- `Array`, `ByteArray`, `String`, `Symbol`
- `Interval`
- `Dictionary`, `IdentityDictionary`, `Set`, `IdentitySet`
- `OrderedCollection`
- `Association`

`Bag`, `LinkedList`, `MappedCollection` は P4 ではスタブ可。P9 までに `Collection` プロトコルの中核（`do:`, `collect:`, `select:`, `reject:`, `detect:ifNone:`, `inject:into:`, `includes:`, `size`, `isEmpty`）をネイティブまたは確実な転送で実装する。

#### Magnitude

- `Magnitude`
- `Number`, `Integer`, `SmallInteger`, `LargePositiveInteger`, `LargeNegativeInteger`
- `Float`（IEEE754 binary64）
- `Fraction`（P4 は四則の最小）
- `Character`
- `Date` / `Time` は P9 までスタブ可

`SmallInteger` の `+ - * // \\ quo: rem: bitAnd: bitOr: bitXor: bitShift: = < > <= >=` はネイティブ。オーバーフローは `LargeInteger` へ透過。`Integer>>timesRepeat:` もネイティブ。

#### Graphics-min

- `Point`, `Rectangle`

描画プリミティブ（BitBlt, Form, Display）は v1 の対象外。座標計算だけホストツールが使う。

#### Streams / System

- `Stream`, `PositionableStream`, `ReadStream`, `WriteStream`, `ReadWriteStream`
- `Transcript` モデル（実際の出力先はホストウィンドウ）
- `SmalltalkImage`（グローバル辞書のホスト。クラシック `Smalltalk`）

### 3.7 ブートストラップ手順

循環（`Object` のクラスは `Object class`、`Object class` のクラスは `Metaclass`、…）を Smalltalk ソースから作れない。C++ で手書きする。

順序:

1. ヒープを作る。
2. `nil` / `true` / `false` の即値または専用オブジェクトを登録する。
3. クラスオブジェクトの骨格を未初期化で割り当て、well-known 表に載せる。
4. 各クラスの `superclass` / `methodDict` / `format` / メタクラスリンクを結ぶ。
5. `NativeMethod` を関数ポインタから生成し、各メソッド辞書へ `Symbol` キーで入れる。
6. グローバル `Smalltalk` にクラス名 → クラスを登録する。
7. スナップショット可能にする。
8. 以降の非 Kernel クラスは `image/vendor/` の file-in で追加する。自作しない。

well-known 表は `include/ao/WellKnown.hpp` に列挙し、テストから名前で参照する。

### 3.8 コンパイラ

入力: Smalltalk-80 メソッドソースおよびチャンク形式（`!Class methodsFor: 'protocol' stamp: '...'!`）。

出力: `CompiledMethod` またはクラス定義の副作用。

要求:

- カスケード `;`
- キーワードセレクタ、バイナリセレクタ、ユニタリ
- ブロック `[:a :b | ...]`
- 一時変数 `| x y |`
- リテラル: 数、文字列、シンボル、文字、配列 `#( )`、バイト配列
- 擬変数 `self` `super` `thisContext` `nil` `true` `false`
- 代入 `:=` および `_`（入力は受け付けるが、ソース保存は `:=`）
- 捕捉解析: コードを生成する前に、変数ごとに、宣言したスコープ、展開しないブロックからの捕捉、代入の有無を調べ、temp ベクタに置く temp を決める（§3.4）。
- インライン展開（§3.5）
- 引数（メソッドとブロック）と `to:do:` のループ変数への代入は、コンパイルエラー `cannot assign to argument` にする。
- temp の上限は 255 である。引数、temp、持ち上げた temp（展開したブロックの temp、`to:do:` のループ変数と上限）、temp ベクタを入れるスロット、コピーした値を合わせて数える。

エラーはソース区間付き。Browser の accept は失敗時にテキストを壊さずエラーを表示する。

### 3.9 ホスト GUI（AppKit）

アプリケーション名: **Ao**。バンドル ID: `jp.ao.smalltalk`（実装時に変更可。SPEC を更新すること）。

ウィンドウはすべて独立した `NSWindow`。MDI もフルスクリーンキャンバスも使わない。

#### Transcript

- グローバル `Transcript` への `show:` / `cr` / `nextPutAll:` / `clear` がこのウィンドウに出る。
- フォントは可変幅可。等幅設定を用意する。
- 起動時に 1 枚開く。閉じてもオブジェクトは生き、再表示できる。

#### Workspace

- 自由テキスト。
- メニュー / キー:
  - Do it（選択範囲を評価、結果は捨てる）
  - Print it（評価して結果の `printString` を挿入）
  - Inspect it（結果を Inspector 相当のウィンドウへ。v1 は簡易オブジェクトビューで可）
- 評価コンテキストの `self` は `nil`。ワークスペース変数（未定義識別子を束縛にする。§3.10）は v1 で実装する。

#### System Browser

クラシック 5 ペインの契約を AppKit で実装する。

```
[ class category list ] [ class list ]
[ message category   ] [ message list ]
[          source + comment editor         ]
```

- 左上: `SystemOrganization` 相当のカテゴリ
- 右上: カテゴリ内クラス。インスタンス / クラス サイド切替
- 左下: プロトコル
- 右下: セレクタ
- 下: ソース。accept でコンパイルしメソッド辞書を更新
- クラス定義テキストの accept でクラス作成 / 再定義
- 階層表示（hierarchy）をクラスリストの代替または別コマンドで提供

外観は macOS 標準（NSSplitView, NSTableView / NSOutlineView, NSTextView）。Smalltalk-80 の白黒ビットマップ見た目を再現しない。キーバインドは macOS 標準 + Smalltalk 慣習（Do it は `⌘D`、Print it は `⌘P` が衝突するため `⌘P` は Print it にせず、メニューで明示。Print it は `⌘I` を既定候補とし、設定可能にする）。

メニューバー:

- Ao / File / Edit / Smalltalk / Tools / Window / Help
- Tools: Browser, Transcript, Workspace
- Smalltalk: Do it, Print it, Inspect it, Accept

アクセシビリティ: VoiceOver ラベルを主要コントロールに付ける。動的な過度なアニメーションを使わない。

### 3.10 ブリッジ

C ABI（`bridge/ao_abi.h`）のみが runtime と app の境界。

必須エントリ:

- 起動 / 終了 / イメージ load-save
- 文字列ソースの評価（Workspace / Do it）
- クラス一覧、セレクタ一覧、ソース取得、accept
- Transcript コールバック（ランタイム → アプリ）
- エラーオブジェクトの文字列化

AppKit オブジェクトを OOP としてヒープに直接置かない。ホストハンドル表で結ぶ。

#### セッション

プロセスにセッションは 1 つ。`ao::boot()` はそれを 1 つ作る。既にあるときに再度呼ぶと 0 以外を返す。`ao::shutdown()` はセッションを捨て、セッションが無くても 0 を返す。

中身はテストの `Boot` と同じである。`Heap`、`Roots`、`WellKnown`、`Bootstrap::run`、`ClassMethodCache`、そのキャッシュを指す `CallContext`。既定の初期容量（nursery 1 MiB×2、old 4 MiB）は変えない。old は上限まで伸びる。`ao_image_load` はヘッダの heapBytes に合わせてコミットする。

`ao_image_load` はヒープと well-known とキャッシュを載せ替える。transcript フック関数ポインタはセッション側に残し、ロードで消さない。ロードのあと `ensureKernelNatives` を呼ぶ。これは、Kernel のネイティブ（Transcript のクラス側の転送を含む）のうち、ロードしたイメージのメソッド辞書に無いセレクタだけを `putNative` する。既にあるセレクタは上書きしない。後から足したネイティブが、古いイメージにも入る。`ao_image_save` は実行中のインタプリタの外からだけ呼び、呼び出し規約は `Image::save` と同じ。`ao_image_load` は `Image::load` が成功したあと、`1 + 2` が SmallInteger の 3 で、`nil isNil` が true でなければ `AO_ERR`。探針に失敗したセッションはシャットダウンしない。`ao_filein_load_order` は `fileInLoadOrder` をセッションに対して呼ぶ。パスが読めなければ `AO_ERR`。

#### C ABI

既存の `AO_OK = 0`、`AO_ERR = 1` は維持する。追加する。

```
AO_ERR_COMPILE = 2
AO_ERR_EVAL = 3
AO_ERR_RANGE = 4
```

文字列バッファは、`buf_len > 0` なら必ず NUL で終わる。入り切らないときは `AO_ERR_RANGE`。

```c
typedef struct AoSpan {
  unsigned int start;
  unsigned int end;
  char message[256];
} AoSpan;

typedef void (*AoTranscriptFn)(const char* utf8, int len, int is_clear, void* user);
typedef void (*AoInspectFn)(const char* class_name, const char* print_utf8, void* user);

int ao_image_save(const char* path);
int ao_image_load(const char* path);
int ao_filein_load_order(const char* path);
void ao_set_transcript_hook(AoTranscriptFn fn, void* user);
void ao_set_inspect_hook(AoInspectFn fn, void* user);

int ao_browser_class_count(void);
int ao_browser_class_at(int index, char* name, int name_len, char* category, int category_len);
int ao_browser_protocol_count(const char* class_name, int meta);
int ao_browser_protocol_at(const char* class_name, int meta, int index, char* buf, int len);
int ao_browser_selector_count(const char* class_name, int meta, const char* protocol);
int ao_browser_selector_at(const char* class_name, int meta, const char* protocol,
                           int index, char* buf, int len);
int ao_browser_source(const char* class_name, int meta, const char* selector, char* buf, int len);
int ao_browser_class_definition(const char* class_name, char* buf, int len);
int ao_browser_superclass(const char* class_name, int meta, char* buf, int len);
int ao_browser_subclass_count(const char* class_name);
int ao_browser_subclass_at(const char* class_name, int index, char* buf, int len);

int ao_workspace_reset(void);
int ao_eval(const char* source, int source_len, int mode, char* out, int out_len, AoSpan* err);
int ao_accept_method(const char* class_name, int meta, const char* source, AoSpan* err);
int ao_accept_class(const char* source, AoSpan* err);
```

`meta` は 0 がインスタンス側、1 がクラス側（そのクラスの `klass`）。クラス一覧にメタクラスは出さない。`mode` は `AO_EVAL_DOIT = 1`、`AO_EVAL_PRINTIT = 2`、`AO_EVAL_INSPECTIT = 3`。フックの `user` は Swift が保持するオブジェクトのポインタである。ランタイムはそれを OOP として辿らない。フックは評価を呼び直さない。

#### ソースはイメージに書かない

メソッドソースはセッションのルート表（`(Oop method, Oop string)` を `Roots` に登録したベクタ）だけが持つ。`.aoimage` には書かない。上書きした古い対はルートから外す。`ao_runtime_boot` と `ao_image_load` は表を空にする。

#### プロトコルとカテゴリ

プロトコルはメソッド辞書の各値を見て、クラスが `NativeMethod` なら `native`、それ以外なら `user`。空の側は返さない。順序は `native` の次に `user`。セレクタはプロトコルで絞り、UTF-8 でソートする。継承したメソッドは含めない。カテゴリ（`kClassSlotCategory`）が nil または空なら、一覧上の見出しは `Kernel`。定義テキストの category は、nil なら空文字 `''`、それ以外はそのバイト列。

#### Transcript のクラス側転送

クラス側の `show:` / `nextPutAll:` / `nextPut:` / `cr` / `clear` は、`WellKnown::transcript` に同じメッセージを送り、戻り値はクラス（レシーバ）である。フックへ渡す UTF-8 は、文字ならそのスカラー 1 個、バイト列の文字列ならそのバイト、nil なら `is_clear = 1`、`len = 0`。インスタンス側の既存ネイティブは変えない。

#### ワークスペース変数

ワークスペースはセッションに 1 つ。`IdentityDictionary` ではなく、名前文字列をキーにした `Dictionary` をルートする。値は束縛（`Association`。キーは名前の文字列、値は変数の値）である。`ao_workspace_reset` は空の辞書に戻す。

- 名前の解決順は、ローカル（引数と temp）→ インスタンス変数 → 擬変数 → `knownGlobals` → 束縛。宣言した temp（`| q |`）は同じ名前の束縛と関係しない。
- `knownGlobals` は `Globals::nameAt` の 57 名、`Smalltalk`、`eachExtra` の名、`eachClass` のクラス名バイト。`Smalltalk` はグローバル表そのもので、クラスは `SmalltalkImage` である（`at:` と `at:put:` を受ける）。セッションはこれをキャッシュし、クラスの定義と `Smalltalk at:put:`（グローバルの登録）のあとで作り直す。既知のグローバル名の読みは `PushGlobal`、その名前への代入はコンパイルエラー `cannot assign`。後から同じ名前のクラスを定義すると、束縛よりクラスが勝つ。
- どれにも当たらない名前は束縛である。読みは `PushLitVar`、代入は `StoreLitVar` / `PopStoreLitVar`。束縛が辞書に無ければ、メソッドを作るとき（リテラルを箱に入れるとき）に値 nil で作って辞書に入れる。同じ名前の束縛は評価をまたいで同じ Association なので、ブロックに捕捉した束縛への代入も辞書に残る。束縛の数に上限は無い（temp の 255 に数えない）。

Do it は結果を捨て `out` は空文字。Print it は `printString` の UTF-8 を `out` に書く。Inspect it は `inspect` のあと Print it と同じ文字列を `out` に書く。空 OOP は `AO_ERR_EVAL`。abort（§3.4）も `AO_ERR_EVAL` で、理由を `AoSpan.message` に入れる。コンパイル失敗は `AO_ERR_COMPILE` と `AoSpan`。

#### printString

Object の `printString` はクラス名のまま。次だけネイティブで上書きする。

| レシーバ | 文字列 |
|---|---|
| nil | `nil` |
| true | `true` |
| false | `false` |
| SmallInteger | 十進。負号付き。桁区切りなし |
| Character | スカラーが 32…126 なら `$` と その 1 バイト。それ以外はクラス名 |
| String | 単一引用符で囲み、中の `'` は `''` |
| Float | `snprintf` の `%g` |
| Array | `#(` の直後に要素の `printString` を空白区切りで並べ、`)` で閉じる。空なら `#()`。深さ 4 を超えた要素は `...` |

LargeInteger とそれ以外はクラス名のまま。

### 3.11 イメージ形式 `.aoimage`

- マジック `AOIM`、バージョン、ポインタサイズ、エンディアン
- well-known 表
- ヒープダンプ（直接ポインタはファイル内オフセットに再配置）
- グローバル辞書
- 起動時に再配置し、NativeMethod の関数ポインタは **ロード時にシンボル名で結び直す**（ポインタをファイルに書かない）

`NativeMethod` は安定したシンボル名（例: `ao_Object_identityEquals`）を持つ。版番号は 1 のままとする。オペコードやネイティブを追記しても版は変えない。ロードのあと `ensureKernelNatives`（§3.10）で足りないネイティブを補う。古いイメージのコンパイル済みブロックは、再 Accept するまでコピーの意味論のまま動く。

ヘッダの `heapBytes` は old の上限以下とする。上限を超えるヒープは保存せず、そのようなイメージのロードは拒否する。

### 3.12 クラスライブラリは取り込む。自作しない

巨大な Smalltalk ライブラリ（Collection の周辺、数値、ストリーム、ファイル、例外、日付、ツールモデル）を Ao 用に書き下ろすことは **禁止** する。人手もエージェントも、既存実装と同等のライブラリを再発明しない。

自作してよいのは次だけである。

- C++ の VM / Kernel ネイティブメソッド
- AppKit の Browser / Transcript / Workspace
- vendor ソースを Ao のオブジェクトモデルに載せるための **最小パッチ**
- `image/tests/` の受け入れテスト

#### 取り込み元

一次候補は **Cuis Smalltalk** のクラスライブラリ（規模が小さく、MIT、Blue Book に近い）。Cuis で足りないパッケージだけ Squeak（MIT）から足す。

使わないもの:

- Xerox 1983 仮想イメージのバイナリ
- Pharo / Squeak の VM・プラグイン・BitBlt 実装
- VisualWorks などプロプライエタリのソース

確定した upstream のリビジョンは `image/vendor/ORIGIN.md` に URL・コミット・ライセンス全文の場所を書く。未確定のまま file-in しない。

#### 載せ方

1. upstream の `.st`（またはチェンク書き出し）を `image/vendor/<origin>/` に置く。
2. Kernel と衝突するメソッド（`Object>>#==` などネイティブ必須）は file-in しない。すでに `NativeMethod` があるセレクタは上書き禁止。
3. ホストに移した機能（描画、ファイルダイアログ、Browser ビュー）を参照するメソッドは `image/patches/` でスタブか削除する。
4. P5 以降、`ao filein image/vendor/...` で `CompiledMethod` として載せる。
5. ロード順は `image/vendor/LOAD_ORDER` に固定する。

v1 で載せる範囲（file-in）:

- Collection 周辺でネイティブにしていない葉（`Bag`, `LinkedList`, `Heap` など）
- `Exception` / `Error` / `Notification` とその流儀
- `Date` / `Time` / `Timespan` 相当
- `FileStream` のうちホスト ABI に接続できる最小
- ツールの **モデル**（カテゴリ・クラス一覧）が vendor にあるならそれを使う

v1 で載せない:

- Morphic / MVC / BitBlt / Form 世界
- Monticello / Metacello / Iceberg
- ネットワークスタック一式

#### ライセンス

vendor のライセンスを落とさない。新規の C++ / Swift は **Apache License 2.0**。混在する場合は `NOTICE` に origin を列挙する。ライセンス条文の正本は英語の `LICENSE`。

---

## 4. TDD

テストは実装より先に書く。赤のテストを残したまま次フェーズへ進まない。

### 4.1 runtime（GoogleTest）

最低限のスイート:

- `oop_tag_test`: SmallInteger / Character / 即値 / ヒープのタグ往復
- `heap_alloc_test`: 割り当て、サイズ、クラススロット
- `gc_test`: nursery 回収、旧世代昇格、ルート保護、内部ポインタ更新
- `bootstrap_test`: メタクラス循環の不変条件 10 項目
- `lookup_test`: 継承、super、doesNotUnderstand
- `native_send_test`: `1 + 2`、`true ifTrue: []`、`#==`
- `smallinteger_arith_test`: オーバーフローで LargeInteger へ
- `collection_do_test`: Array/String/Dictionary の中核プロトコル
- `compiler_roundtrip_test`: ソース → バイトコード → 評価
- `block_test`: 引数、返り値、外側 temps の共有、非局所リターン、`ensure:`
- `image_save_load_test`: save 後に同一評価結果
- `transcript_model_test`: コールバックが呼ばれる

GC ストレス実行: 環境変数 `AO_GC_STRESS=n` を付けると、`allocateRetry` と safepoint で n 回に 1 回 nursery GC を走らせ、そのうち 4 回に 1 回は old の GC も走らせる。GC で解放した領域は `0xA5` で埋め、古い番地を読んだら落ちるようにする。ctest の `gcstress` 項目は、runtime のスイート全体（時間計測の `KernelBench.*` を除く）を 1 プロセスでこのモードで回す。`gcstress_vendor` 項目は、vendor の file-in（`ao filein --load-order image/vendor/LOAD_ORDER`）と `ao --test image/tests` をこのモードで回す。`ao filein` は、file-in が成功してもメソッド単位のエラーを 1 行ずつ stderr に出す。vendor の file-in には既知のエラーがあるので、`gcstress_vendor` はストレスなしの 1 回の出力を基準にし、ストレス下の出力がそれと一致することを確かめる。

### 4.2 compiler

- パース成功 / 失敗区間
- カスケード、キーワード、ブロックの AST
- 生成バイトコードのスナップショットテスト（安定したら固定）

### 4.3 app（XCTest）

- ABI 経由の評価
- Browser モデルのカテゴリ / クラス / セレクタ列挙
- accept 成功と失敗
- Workspace Do it / Print it

### 4.4 ゴールデン評価（P6 以降常時）

`image/tests/*.st` に Smalltalk で書いた例題を置き、`ao --test` で実行する。

```smalltalk
"example"
self assert: 1 + 2 equals: 3.
self assert: (true ifTrue: [4] ifFalse: [5]) equals: 4.
self assert: (Object new class) equals: Object.
```

---

## 5. 制約

1. **VM は新規実装。** 他処理系の C/C++/Swift VM をコピーしない。クラスライブラリの `.st` は §3.12 に従い取り込む。
2. **Kernel はネイティブ。** Kernel メソッドを `.st` の実行定義にしない。
3. **チャットは正本ではない。** 仕様変更は `SPEC.md` を先に直す。
4. **余計なものを作らない。** 依頼されていないデバッガ、パッケージマネージャ、シンタックステーマ、ウェブサイトを追加しない。
5. **依存は最小。** runtime は C++20 標準ライブラリ + 必要なら mimalloc 程度。OS の API として mmap / mprotect を使ってよい（old の予約とコミット）。GUI は AppKit のみ。Boost、Qt、SDL、SwiftUI 主系統は使わない。
6. **Apple Silicon を第一対象。** Intel Mac は考慮しない。
7. **C ABI 以外で Swift が C++ テンプレートに依存しない。**
8. **例外方針:** C++ は例外を境界で使わない。エラーは Smalltalk 例外オブジェクトか `AoError` コード。
9. **エンコーディング:** ソースと文字列は UTF-8。`Character` は Unicode スカラー。
10. **ライセンス:** 新規コードは Apache License 2.0 を既定とする（変更するなら SPEC を更新）。条文の正本は英語の `LICENSE`。
11. **Graphify と Serena を使う。** 手順は `CLAUDE.md`。この制約は開発プロセスにも適用する。
12. **日本語コメントはユーザー向け文書と `.st` コメントに限る。** C++ / Swift の識別子とコミットメッセージは英語。

---

## 6. 受け入れ基準

v1 は次をすべて満たす。

### 言語 / ランタイム

- [x] `1 + 2` が `3`（SmallInteger、ネイティブパス）
- [x] `true ifTrue: ['ok'] ifFalse: ['ng']` が `'ok'`
- [x] `nil isNil` が `true`、`Object new isNil` が `false`
- [x] `Object new class == Object`
- [x] `Object class class == Metaclass`
- [x] `Metaclass class class == Metaclass`
- [x] `Object superclass` は `nil`（または明示したルート方針に一致。採用したら SPEC を更新）
- [ ] 未定義セレクタは `doesNotUnderstand:` に入り、デフォルトはエラーオブジェクトを返す
- [x] `#(1 2 3) collect: [:x | x * 2]` が `#(2 4 6)`
- [x] ブロックが外側の temp を共有する（`| y | y := 0. 3 > 1 ifTrue: [y := 1]. y` が `1`、`#(1 2 3) do: [:e | sum := sum + e]` のあと `sum` が `6`）
- [x] ユーザーが Browser から `Object>>foo` を追加し、Workspace から `Object new foo` を評価できる
- [x] `.aoimage` を保存して再起動し、追加したメソッドが残る
- [x] `image/vendor` から file-in した非 Kernel メソッドが `CompiledMethod` として評価できる
- [x] vendor 由来メソッドがネイティブ必須セレクタを上書きしていない
- [x] Kernel のメソッド辞書を走査したとき、値はすべて `NativeMethod`
- [ ] 上記 Kernel 走査テストが CI で失敗する変更はマージしない

### ツール

- [x] 起動すると Transcript と Workspace が出る
- [ ] Tools メニューから System Browser を開ける
- [x] Browser で `Object` のメソッド一覧が見える
- [x] ソースペインでメソッドを編集し Accept すると、直後の Do it に反映される
- [x] Transcript に `Transcript show: 'hello'; cr` が出る
- [ ] ウィンドウ操作が macOS 標準（閉じる、最小化、Spaces）に従う

### 性能（緩いゲート。数値は実機で更新）

- [x] `docs/bench.md` の比は 5.76 である。v1 はこの比をリリース条件にしない。
- [x] 起動（イメージロード後に Workspace 操作可能）が 2 秒以内を目標とする。未達なら理由を SPEC に書く

### プロセス

- [x] `graphify-out/GRAPH_REPORT.md` がリポジトリにあり、主要モジュールをコミュニティとして記述している
- [x] Serena で `Object` 相当のネイティブ実装シンボルが解決できる
- [x] `CLAUDE.md` の Graphify / Serena 手順を実装エージェントが破っていない

---

## 7. 用語

| 用語 | 意味 |
|---|---|
| Blue Book | *Smalltalk-80: The Language and its Implementation* |
| Kernel | 3.6 節のクラス群。ネイティブ必須 |
| NativeMethod | 関数ポインタを持つメソッドオブジェクト |
| CompiledMethod | バイトコードメソッドオブジェクト |
| OOP | 64-bit tagged object pointer |
| accept | Browser でソースをコンパイルし辞書に入れる操作 |
| Do it | 選択ソースを評価する操作 |

---

## 8. 変更規則

- 本ファイルが仕様の正本である。
- フェーズ追加、クラス追加、バイトコード変更、キーバインド確定は、実装の前にこのファイルを更新する。
- 更新したら Graphify を掛け直す（`CLAUDE.md` 参照）。

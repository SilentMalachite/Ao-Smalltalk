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
| hash | 16 | identity hash。作るたびに 1 から順に振り、65535 の次は 1 に戻る。衝突は許容し、サイドテーブルは持たない |

identity hash は 16 bit なので、65535 個を超えるオブジェクトでは必ず衝突する。`IdentityDictionary` や `IdentitySet`（§3.6）は衝突しても正しく引けるが、衝突が多いと探索は遅くなる。

可変長オブジェクト（Array, String, ByteArray, CompiledMethod）はヘッダ直後にペイロードを置く。

### 3.2 オブジェクトメモリと GC

- アロケータは世代別（nursery bump + 旧世代 free-list または immix 系）。v1 は nursery + mark-compact old でよい。
- ルート: グローバル辞書 `Smalltalk`、生きているすべてのプロセス（走っているもの、実行可能なもの、待っているもの、止まっているもの。§3.4）とそのコンテキスト連鎖、AppKit が保持するハンドル表、イメージ起動時の well-known 表。生きているプロセスは、どこからも参照されていなくても回収しない。
- 正確式。コンパイラとインタプリタはスタック上の OOP を GC に報告する。
- ファイナライザと弱配列は v1 では `ephemeron` なしの弱スロットフラグまで。
- スレッド: ミューテータは 1 本（Smalltalk のプロセスは、同じスレッドの上で切り替えるファイバ。§3.4）。GC は safepoint。AppKit メインスレッドとはブリッジキューで切る。
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
- それでも割り当てられないとき（GC を走らせない割り当てが old の上限で失敗したときを含む）は、評価を「out of memory」で中断する（§3.3 の失敗の規則）。
- ネイティブが受け取る receiver と引数は、ルート済みとする。ネイティブの途中で GC が走っても、転送先を指す。
- スケジューラの呼び出し（`fork`、`yield`、`wait`、`signal`、`suspend`、`resume`、`terminate`。これらを使う `SharedQueue` の `next` と `nextPut:` も）は、どれも GC 点である。切り替えれば、戻るまでにほかのプロセスが走り、GC も走る。ネイティブはそれをまたいで生の OOP を持たない（ルートするか、ルート済みのスロットから読み直す）。
- GC は、走っているプロセスのルートに加えて、止まっているすべてのプロセスのルート（そのプロセスのネイティブがルートした値）をたどる。1 回の GC は、どのルートも 1 度だけたどる。

### 3.3 メッセージ送信

送信 `receiver selector: args` の意味は Blue Book に従う。

1. `receiver` のクラス `C` を取る。
2. `C` からスーパークラス鎖を `Object` まで辿り、メソッド辞書でセレクタを探す。
3. 見つかればそのメソッドを適用する。
4. 見つからなければ `receiver doesNotUnderstand: aMessage` を送る。`Object>>doesNotUnderstand:` の既定は、評価を中断し（§3.4 の abort）、`doesNotUnderstand: #sel` を理由として返す（`ao_eval` は `AO_ERR_EVAL` とこの文字列を返す）。
5. `super` 送信は、メソッドが定義されたクラスのスーパークラスから探索を始める。

スーパークラスの枠は `instVarAt:put:` で何にでも書き換えられるので、鎖が壊れていることがある。鎖をたどる処理（送信の探索、ネイティブ、コンパイラ）は、どれも次の規則で止まる。

- 鎖は、`nil`、クラスの形をしていないもの（即値、バイト列、クラスの枠をすべて持たないポインタオブジェクト）、すでに通ったクラス（循環）、1024 段目、のどれかに当たったところで終わる。その先は無いものとして扱う。
- 送信: 鎖が終わるまでにセレクタが見つからなければ `doesNotUnderstand:` を送る。`doesNotUnderstand:` も見つからなければ、既定と同じく `doesNotUnderstand: #sel` で評価を中断する。
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

キャッシュの無効化: メソッドの追加・置換（Browser の accept、file-in のメソッドチャンク、`putNative`）とクラスの差し替え（既存の名前へのクラス定義）は、どれも同じ 1 つの関数を通り、その関数がキャッシュを無効化する。無効化は、そのセレクタのエントリをレシーバのクラスによらずすべて捨てるか、キャッシュ全体を捨てる（クラスの差し替え）。定義クラスのエントリだけを捨てるのでは足りない（サブクラスをレシーバとするエントリが残る）。差し替えの直後の送信は、キャッシュ済みの送信でも新しいメソッドを適用する。例えば `Object>>zork` を再 Accept すると、それまで `3 zork` でキャッシュしていた送信にも反映される。反射（`instVarAt:put:` でメソッド辞書を書き換えること）はこの限りでない。

メソッドは 2 種類だけ持つ。

| 種類 | 表現 | 適用 |
|---|---|---|
| `NativeMethod` | C++ 関数ポインタ + 引数個数 + プリミティブ番号（任意） | Kernel の全メソッド |
| `CompiledMethod` | バイトコード + リテラル枠 + 一時変数数 | ユーザー定義、および Kernel 以外 |

`NativeMethod` は Smalltalk オブジェクトとしても存在する（Inspect できる）が、実行は関数ポインタ直呼び。バイトコードインタプリタに入らない。

セレクタは `Symbol`。同一文字列は intern され `#==` が成り立つ。

#### 失敗は評価を中断する

失敗を値として返さない。失敗はすべて abort（§3.4）で評価を中断する。abort は最外まで巻き戻し、途中の `ensure:` は実行する。`ao_eval` は `AO_ERR_EVAL` を返し、理由を `AoSpan.message` に入れる（§3.10）。理由は空にしない。

| 失敗 | 理由 |
|---|---|
| `Object>>doesNotUnderstand:` の既定。`doesNotUnderstand:` も見つからない送信 | `doesNotUnderstand: #` とセレクタ（例: `doesNotUnderstand: #foo`） |
| `Object>>error: anObject` | 下の細則 |
| `Object>>subclassResponsibility`、`Object>>shouldNotImplement`、メタクラスへの `new` | `subclassResponsibility`、`shouldNotImplement` |
| Kernel ネイティブの、文言のある失敗（範囲外の添字、0 除算、引数の型など） | その文言（例: `at: index out of range`、`division by zero`） |
| 送信の結果が空 OOP で、巻き戻しの最中でない（文言の無いネイティブの失敗、ブロックの引数個数の不一致など） | `failed: #` とセレクタ（例: `failed: #value`）。ただし old の上限で割り当てられなかった後なら `out of memory` |
| 空 OOP をレシーバにした送信 | 同上 |
| old の上限（§3.2） | `out of memory` |
| スタックガード（§3.4） | `stack overflow` |
| 分岐の値が Boolean でない（§3.5） | `NonBoolean receiver` |
| ホームが死んだ `^` の既定（§3.4） | `cannot return` |
| プロセスの操作（§3.4 の「プロセスと協調スケジューラ」） | `deadlock: no runnable process`、`too many processes`、`process terminated`、`process cannot run` |

- `error:` の理由: 引数が String（Symbol など String のサブクラスを含む）なら、その内容（UTF-8 のバイト列）。それ以外は、引数に `printString` を送った答えの内容。`printString` が評価を中断したら、その理由のままにする。`printString` が String を答えなければ、理由は `error:` とする。
- 空 OOP はネイティブの中だけで使う失敗の印であり、値ではない。インタプリタはそれをオペランドスタックに積まない。ヒープのスロットにもコレクションにも残らない。`classOf` が空 OOP にクラスでない値（nil）を返す経路は、送信の入口で止める。
- 例外オブジェクトと `on:do:` による捕捉は、v1 では扱わない。vendor の `Exception` / `Error` / `Notification` は file-in するが、ランタイムはハンドラを探さない。`on:do:` は Kernel に無いので、送ると `doesNotUnderstand: #on:do:` で中断する。
- 理由は、それを最外で読むまで GC をまたいで保つ。すでに abort の最中なら、最初の理由を保つ。abort の途中で走る `ensure:` の後始末が abort しても同じである（§3.4）。
- 最外で読むとき、理由の NUL バイトは `\0` の 2 文字に置き換える（C 文字列で途切れないように）。

### 3.4 コンテキストとプロセス

v1 の実行モデル:

- ユーザーメソッドは `MethodContext`（Blue Book `Context` / `MethodContext` 相当）。
- プロセスは協調的。`ProcessorScheduler` / `Process` / `Semaphore` を Kernel に含める（下の「プロセスと協調スケジューラ」）。
- ホストのプリエンプションは使わない。長時間ネイティブメソッドは safepoint を自ら発行する。

#### クロージャと共有 temp

- ブロック式を評価すると、クロージャ（`BlockContext` の形のオブジェクト）ができる。クロージャは、メソッド、レシーバ、ホーム（そのブロックを字句上含むメソッドのコンテキスト）、コピーした値の Array を持つ。sender は nil。
- ブロックは外側の temp を共有する（Blue Book の意味論）。ブロックが捕捉した temp のうち、どこかで代入されるものは、コンパイラが temp ベクタ（ヒープの Array）に置く。ベクタは、その temp を宣言したスコープ（メソッド、ブロック、展開したブロック）に入るたびに新しく作る。捕捉したブロックはベクタそのものをコピーするので、外側とブロックが同じ値を読み書きする。ブロック内の代入は、外側にも、同じブロックの次の起動にも見える。
- 代入の無い捕捉（引数、`to:do:` のループ変数、代入の無い temp）は、クロージャを作った時点の値をコピーする。
- ブロックを起動するたびに、新しいアクティベーション（`BlockContext`）を作る。アクティベーションの sender は呼び出し元である。クロージャ自身は書き換えない。
- コンテキストは、そのフレームを抜けるときに pc と sender を nil にする。これが死んだコンテキストの印である。

#### 非局所リターン、ensure:、abort

- ブロック内の `^` は、ホームのメソッドから返る（非局所リターン）。途中のフレームはネイティブも含めてすべて巻き戻す。ブロックを呼んだネイティブも、メッセージを送ったネイティブ（キーに `hash` や `=` を送る Dictionary など）も、呼んだ先から戻ったときに巻き戻しの最中なら、直ちに空 OOP を返す。残りの反復も、続く送信も、副作用も行わない。
- ホームが死んでいれば、ブロックのアクティベーションに `cannotReturn: 値` を送る。ホームを探してスタックを巻き戻すことはしない。`cannotReturn:` が答えを返せば、それをブロックの値として呼び出し元に返す。`BlockContext>>cannotReturn:` の既定は `self error: 'cannot return'` と同じで、評価を `cannot return` で中断する（§3.3）。
- `ensure: aBlock` は、レシーバのブロックを評価したあと、正常に終わっても巻き戻しの途中でも `aBlock` を評価する。後始末の間は巻き戻しを止め、終わったら続ける。`ifCurtailed: aBlock` は、レシーバのブロックが巻き戻ったときだけ `aBlock` を評価する。後始末が自分で巻き戻し（非局所リターンか abort）を始めたときは、止めていた巻き戻しの種類で決める。`ensure:` と `ifCurtailed:` で同じである。
  - 止めていたのが非局所リターンなら（`ensure:` のレシーバが正常に終わったときも）、後始末が始めた巻き戻しを優先する。
  - 止めていたのが abort なら、abort を優先する。後始末が始めた非局所リターン（後始末の `^`）は捨てる。後始末が abort したときは、その理由を捨てて最初の理由を保つ（§3.3）。
- abort は評価の中断である。ホームの無い非局所リターンとして扱い、どのフレームでも止まらずに最外（下記）まで戻る。途中の `ensure:` は実行する。abort を始めるのは `abortEvaluation` だけである。
- abort は理由の文字列を持つ。理由には 2 種類ある。割り当てなしで入れる固定の文言（`stack overflow`、`NonBoolean receiver`、`out of memory` など）と、実行時に組み立てた文字列（ヒープの String）である。後者は、最外で読んで消すまで GC のルートに置く。後者を割り当てられなければ、理由は `out of memory` にする。`ensure:` の後始末の間も、退避した理由を保つ。
- abort の状態は最外で読んで消す。前の評価の abort を次の評価に持ち越さない。
- 最外は、C++ から Smalltalk へ送る入口の 1 回である。次のものが最外である。
  - `ao_eval` の 1 回
  - `ao --test` の 1 ファイルと、テストクラス（`AoTest`）の作成
  - file-in と `ao_accept_class` の、クラス定義チャンク 1 つ（C++ から `subclass:…category:` を送るところ）
  - ワークスペースの作成（起動、`ao_workspace_reset`、`ao_image_load`）
  - `ao_image_load` のロード後の探針（§3.10）
  - fork したプロセスの本体（そのプロセスのスタックで、fork したブロックに `value` を送るところ。下の「プロセスと協調スケジューラ」）
- 最外は、入る前に前の abort と非局所リターンを消して、スタックの範囲を取り直す。出るときに abort の理由を読んで消す。最外での abort は、その入口の失敗である。`ao_eval` は `AO_ERR_EVAL` と理由を返す（§3.3）。クラス定義チャンクの abort は、そのチャンクの file-in エラー `subclass failed: <クラス名>: <理由>` にする（§3.12）。ワークスペースの作成と探針の abort は、その ABI の失敗（`AO_ERR`）にする。テストクラスの作成の abort は `ao --test` の失敗（exit 1）にする。プロセスの本体の abort は、そのプロセスの失敗（下）にする。ただし `terminate` による巻き戻しは失敗にしない。
- スタックガード: メソッド（ネイティブを含む）を適用する前に、C スタックの残りが予約分（`min(512 KiB, スタックの大きさの 1/4)`）を下回っていれば、「stack overflow」で abort する。無限再帰でプロセスは落ちない。
  - スタックの範囲は、最外（上記）の入口と、最外の `Interpreter::run` で必ず取り直す。前のスレッドの範囲を使い続けない。
  - ベースプロセス（下）はホストのスレッドのスタックの範囲を使う。ほかのプロセスは、そのプロセスのスタック（8 MiB。下）の範囲を使い、予約分もその大きさから求める。どちらも、ほかのスタックの範囲を使わない。
  - `ensure:` / `ifCurtailed:` の後始末の間は、予約分の半分まで使ってよい。stack overflow の abort の途中でも、限界近くの後始末が走る。

#### プロセスと協調スケジューラ

プロセスと状態:

- プロセスは `Process` のインスタンスである。どのプロセスも、ランタイムを呼んだホストのスレッド 1 本の上で動く。同時に走るプロセスはいつも 1 つで、`Processor activeProcess` はそのプロセスを答える（nil にならない）。フック（§3.10）も、送ったプロセスの上で同じスレッドから呼ぶ。
- ベースプロセス: boot とロード（§3.11）の直後の `Processor activeProcess` をベースプロセス（ベース）とする。そこが Process でなければ、新しい Process を作って置き、それをベースとする。ベースはホストのスレッドのスタックで動く。`ao_eval` をはじめ、最外（上）のうちプロセスの本体でないものは、すべてベースで実行する。ベースは終わらない。
- ベース以外のプロセスは `fork` で作る。それぞれ専用の C スタックを持ち、同じスレッドの上で切り替えて動く（ファイバ）。スタックは 8 MiB（触れた分だけコミットする）で、下端に読み書きできないガードページを置く。
- ベース以外の生きているプロセス（終わっていないもの。まだ始まっていないものを含む）は 256 までである。
- プロセスの状態は、走っている、実行可能（実行可能キューにいる）、待っている（セマフォの linkedList にいる）、止まっている（`suspend` された。どのリストにもいない）、終わった、のどれかである。fork したプロセスは、ブロックを始めるまで、実行可能か止まっているかである（まだ始まっていないプロセス）。
- 走れないプロセスは、ベースでもこのセッションで fork したものでもない Process（`Process new` で作ったもの、ロードしたイメージにあったもの）と、終わったプロセスである。
- 実行可能キューは `Processor` の quiescentProcesses（OrderedCollection）で、単一の FIFO である。走っているプロセスは入らない。`priority:` は値を保存するだけで、順序に使わない。
- fork したプロセスの suspendedContext は、fork したブロックである。myList は、そのプロセスが入っているリスト（実行可能キューか、セマフォの linkedList）で、どこにも入っていなければ nil である。
- プリエンプションは無い。切り替わるのは、走っているプロセスが `yield` したとき、`wait` で待ちに入ったとき、自分を `suspend` したとき、ほかのプロセスに `terminate` を送ったとき（下）、終わったときだけである。切り替える先は、実行可能キューの先頭のプロセス（以下、次のプロセス）である。
- abort と非局所リターンの状態、割り当てられなかった印（§3.2 の `out of memory`）、スタックの範囲、インタプリタの入れ子の深さ、`hash` の入れ子の数（§3.6）は、プロセスごとに持つ。あるプロセスの abort と非局所リターンは、ほかのプロセスのフレームを巻き戻さない。あるプロセスで割り当てられなかったことは、ほかのプロセスとベースの評価の失敗にならない。
- グローバル、クラス変数、ワークスペースの束縛（§3.10）は、すべてのプロセスで共有する。fork したブロックでの代入は、ワークスペースの束縛にも残る。

操作（どれもネイティブ。§3.6 の Kernel-Processes）:

- `aBlock fork`
  - 引数が 0 個のブロックでなければ失敗する（§3.3。理由は `failed: #fork`）。
  - ベース以外の生きているプロセスがすでに 256 あれば、Process を作らずに `too many processes` で失敗する。
  - プロセスのスタックを確保できない（`mmap` が失敗する）ときも、`too many processes` で失敗する。作りかけの Process は実行可能キューに入れず、走れないプロセスとして残る。
  - それ以外は、新しい Process を作って実行可能キューの末尾に入れ、その Process を答える。切り替えない。ブロックは、そのプロセスに切り替わったときに始まる。
- `Processor yield`: 実行可能キューが空なら、何もせずに返る。空でなければ、走っているプロセスをキューの末尾に入れ、次のプロセスに切り替える。自分の番が来たら返る。答えは `Processor` である。
- `aSemaphore wait`: excessSignals が 1 以上なら、1 減らしてすぐに返る。0 なら、走っているプロセスを linkedList の末尾に入れて待たせ、次のプロセスに切り替える。`signal` で実行可能になり、自分の番が来たら返る。答えはレシーバである。
- `aSemaphore signal`: linkedList の先頭の待つプロセスを外し、実行可能キューの末尾に入れる。その場では切り替えない（`signal` を送ったプロセスが走り続ける）。実行可能キューに入れられない（割り当てられない）ときは、linkedList から外さずに失敗する。外したのが走れないプロセスなら、それを捨てて次の待つプロセスを見る。待つプロセスが無ければ、excessSignals を 1 増やす。SmallInteger の上限を超えるなら失敗する（`signal: excess signals out of range`）。答えはレシーバである。
- `aSharedQueue nextPut: x`: x を末尾に足し、`next` で待つプロセスがあれば 1 つを実行可能にする（`signal` と同じく切り替えない）。実行可能にできずに失敗したときは、x を足さない。自分は待たない（要素の数に上限は無い）。答えは x である。
- `aSharedQueue next`: 先頭の要素を外して答える。空なら、`nextPut:` されるまで待つ（`wait` と同じ）。`signal` を受けずに `wait` から戻っても（`suspend` と `resume`）、空なら待ち直す。
- `aProcess suspend`
  - 走っているプロセスなら、止まっている状態にして次のプロセスに切り替える。`resume` されて自分の番が来たら返る。
  - 実行可能か待っているプロセスなら、そのリストから外して止まっている状態にする。待っていたプロセスは、`resume` されたあと、`signal` を受けずに `wait` から戻る。
  - 止まっているプロセスと走れないプロセスには、何もしない。
  - 答えはレシーバである。
- `aProcess resume`
  - 止まっているプロセス（まだ始まっていないものを含む）を、実行可能キューの末尾に入れる。切り替えない。
  - 走っている、実行可能、待っているプロセスには、何もしない。キューに 2 度入れず、待っているプロセスは待ったままである。
  - 走れないプロセスなら失敗する（`process cannot run`）。
  - 答えはレシーバである。
- `aProcess terminate`: プロセスを巻き戻して終わらせる。巻き戻しは abort と同じで、途中の `ensure:` と `ifCurtailed:` の後始末を、終わらせるプロセスの上で実行する。
  - 走っているプロセス自身（ベースでない）なら、プロセスの本体まで巻き戻して終わり、次のプロセスに切り替える。送った式には戻らない。
  - まだ始まっていないプロセスなら、（いれば）実行可能キューから外し、ブロックを始めずに終わらせる。切り替えない。
  - ほかの始まったプロセス（実行可能、待っている、止まっている）なら、そのリストから外し、そのプロセスに切り替えて巻き戻させる。そのプロセスが終われば、`terminate` を送ったプロセスに戻る。後始末の途中でそのプロセスが切り替えれば（`yield`、`wait`、`suspend`）、その時点で送ったプロセスに戻る。残りの巻き戻しは、そのプロセスが次に走ったときに続ける。
  - `signal` を受けて待ちから外れたが、まだ `wait` から戻っていないプロセス（実行可能なもの、そのあと `suspend` されて止まっているもの）を終わらせるときは、切り替える前にその signal をセマフォに返す。返すことは、そのセマフォにもう一度 `signal` を送るのと同じである（待つプロセスがいればそれを実行可能にし、いなければ excessSignals を 1 増やす）。SharedQueue の `next` で待っていたプロセスなら、その要素は次に `next` を送ったプロセスが受け取る。返すのが失敗すれば（`signal: excess signals out of range`）、`terminate` はその理由で失敗し、プロセスはそのまま残る。abandon（下）では返さない。`suspend` でも返さない（`resume` されれば、signal を受けた `wait` から普通に戻る）。
  - `terminate` を送って相手の終わりを待っているプロセスは、待っているプロセスとして扱う（`resume` しても何もしない）。それを（相手の後始末などが）`suspend` すると、止まっている状態になり、戻る先の印が外れる。相手はそのあと終わっても切り替えても送ったプロセスには戻らず、次のプロセスに切り替える（無ければ下のデッドロック）。送ったプロセスは、`resume` されて自分の番が来たら `terminate` から戻る。
  - ベースへの `terminate` は失敗する（`process terminated`）。ベースは終わらない。ベースで送れば、その `ao_eval` は `AO_ERR_EVAL` を返す。ベース以外のプロセスで送れば、送ったプロセスの失敗（下）になる。
  - 走れないプロセスには、何もしない。
  - 送ったプロセスが走り続けるとき、答えはレシーバである。
  - `terminate` で終わったプロセスは、失敗に数えない。

デッドロック:

- 切り替えようとして実行可能キューが空なら、次のとおりにする。どちらでも、`activeProcess` はベースのままである（nil にしない）。
  - 走っているのがベースなら（ベースの `wait`、ベース自身への `suspend`）、その操作を取り消し（linkedList から外す。excessSignals は変えない）、評価を `deadlock: no runnable process` で中断する（§3.3）。
  - 走っているのがベース以外なら（`wait`、自分への `suspend`、プロセスの終わり）、ベースに切り替える。このときベースは待っているか止まっている。ベースのその操作を上と同じく取り消し、ベースの評価を `deadlock: no runnable process` で中断する。切り替えたプロセスは、待っている、止まっている、終わった状態のまま残る。

プロセスの失敗:

- ベース以外のプロセスの本体が abort で終わったとき（`doesNotUnderstand:`、`error:`、ネイティブの失敗、`stack overflow` など §3.3 の失敗）、そのプロセスだけが終わる。これをプロセスの失敗と呼ぶ。途中の `ensure:` は実行する。ほかのプロセスとベースの評価は続く。
- 本体が abort にならずに終わっても、そのプロセスで割り当てられなかった（§3.2）なら、理由 `out of memory` のプロセスの失敗とする。
- 本体の中で C++ の例外が起きたとき（例外を投げるフックなど）も、そのプロセスの失敗とする。理由は `internal error` である。例外はプロセスの本体の外へ出さず、ホストのプロセスは落ちない。
- ブロックの `^` のホームが生きていて、そのプロセスのフレームに無い（ほかのプロセスのコンテキストである）なら、非局所リターンはそのプロセスの本体まで巻き戻り、プロセスの失敗になる。理由は `non-local return to another process` である。ホームのあるプロセスは巻き戻さない。ホームが死んでいれば、上の `cannotReturn:` の規則による。
- ランタイムは、プロセスの失敗の数と、最後の失敗の理由を持つ。`ao --test` はこれを失敗に数える（§4.4）。`ao_eval` の返り値と `AoSpan` には現れない。

評価の終わり（drain）:

- `ao_eval` は、評価を実行したら（abort したときも）、返り値を確定してから drain する。返り値とは、`out` の文字列（Print it と Inspect it の printString。Inspect it では `inspect` も済ませる）と、`AO_ERR_EVAL` の理由である。drain の前に、ベースの abort の状態を読んで消す。
- drain は、ベースで `Processor yield` を最大 1000 回繰り返すことである。実行可能キューが空になれば、そこで止める。
- drain は返り値を変えない。drain の間に起きた失敗は、プロセスのものもベースのものも、その `ao_eval` の失敗にならない。drain の間の Transcript の出力は、その `ao_eval` が返る前にフックに届く。
- drain のあとに残ったプロセス（待っているもの、止まっているもの、1000 回で走り切らなかった実行可能なもの）は、次の評価に持ち越す。次の評価でベースが切り替えたときと、次の drain で続きを走る。
- 例: `[n := n + 1] fork. Processor yield. n` の Print it は `1` である。`[Transcript show: 'x'] fork` の Do it は、返る前に Transcript へ `x` を出す。

abandon:

- abandon は、ベース以外のプロセスを、後始末（`ensure:` と `ifCurtailed:`）を走らせずに終わらせることである。abandon の間は Smalltalk のコードを走らせず、フックも呼ばない。失敗に数えない。
- abandon するのは、`ao_image_load` が古いセッションを新しいセッションと差し替えるとき、`ao::shutdown()`、セッションの破棄のときである（§3.10）。どれもヒープを捨てる前に行う。
- `ao --test` は、ファイルごとに、drain のあとで残ったベース以外のプロセスを `terminate` で終わらせる（後始末を走らせる。§4.4）。abandon するのは、その後始末が（`wait` などで）ブロックしたまま残ったプロセス（と、§4.4 の繰り返しの上限までに走り切らなかったもの）だけである。

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
- LitVar 系のリテラルは Association（値のスロットを持つポインタオブジェクト）でなければならない。ワークスペースの束縛（§3.10）とクラス変数（§3.6）が使う。
- SendSpecial の `+` `-` `*` `<` `>` `<=` `>=` `=` は、レシーバと引数がともに SmallInteger なら、送信せずに計算して積む（Blue Book の特殊セレクタと同じ）。比較は常にそうする。`+` `-` `*` は、答えが SmallInteger に収まるときだけそうする。それ以外（桁あふれ、SmallInteger でない値、ほかの特殊セレクタ）は通常の送信にする。
  - SmallInteger から引いたこれらのセレクタはネイティブに当たる。accept（§3.10）でも file-in（§3.12）でも、これを隠せない。したがって答えは、送信したときと同じである。
  - ロードしたイメージで、SmallInteger から引いたどれか 1 つがネイティブに当たらないことがある。隠すメソッドを入れられた古いイメージである。そのセッションでは高速路を使わない。
  - 反射（`instVarAt:put:` でメソッド辞書やスーパークラスを書き換えること）による変更は、Blue Book と同じく高速路に反映しない。

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

- `to:do:` の上限は 1 回だけ評価する。反復は、判定 → 本体 → 増分 → 判定の順である。判定の `<=`（刻みが負なら `>=`）と増分の `+` は、SendSpecial で送る。SmallInteger 同士なら、上の規則で送信せずに計算する。レシーバは SmallInteger でなくてよい。刻みの `0.0` も 0 とみなし、展開しない。
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

Boolean の演算（Blue Book）:

- `true & x` と `false | x` は `x` をそのまま答える。`x` は Boolean でなくてよい（`true & nil` は nil、`false | 3` は 3）。`false & x` は false、`true | x` は true。
- `eqv:` と `xor:` は、引数が Boolean でなければ失敗する（§3.3。理由は `failed: #eqv:`、`failed: #xor:`）。`true xor: 3` は評価エラーである。

`=` と `hash`:

- `a = b` が true なら、`a hash = b hash` も true である。Kernel で `=` を値の比較に上書きするクラスは、`hash` も値から計算する。
- `hash` は SmallInteger である。値から計算する `hash` は内容だけで決まり、アドレス、GC、イメージの保存と読み込みで変わらない。
- 型をまたぐ `=` は false である（`1 = 1.0`、`(1/2) = 0.5`、`1.0 = 1` は false）。Integer と Fraction は正規化してあるので、等しい値は同じクラスになる。

| レシーバ | `=` が true になる相手 | `hash` |
|---|---|---|
| SmallInteger, LargePositiveInteger, LargeNegativeInteger | 値の等しい Integer | SmallInteger に収まる値はその値。それ以外は符号と絶対値から |
| Float | IEEE754 の `==` が成り立つ Float（`0.0 = -0.0` は true。NaN はどれとも等しくない。8 バイトに満たない Float（`Float new`）は 0.0 として読む） | `=` と同じに読んだ値のビット列から。`-0.0` と 8 バイトに満たない Float は `0.0` と同じ |
| Fraction | 分子どうしと分母どうしが等しい Fraction | 分子と分母の `hash` から |
| String, Symbol | 同じバイト列の String か Symbol（`#abc = 'abc'` は true） | バイト列から。String と Symbol は同じ関数 |
| Array | 同じクラスで要素数が同じで、要素どうしが `=` | 要素数と、先頭 16 要素に `hash` を送った答えから |
| Point | x どうしと y どうしが `=` の Point | x と y に `hash` を送った答えから |

- Array と Point の `hash` は、要素に `hash` を送る（要素の `hash` は Smalltalk のメソッドでもよい）。答えが Integer でなければ失敗する（§3.3）。送った先で巻き戻しが始まったら、残りの要素に送らず直ちに空 OOP を返す（§3.4）。
- 入れ子の上限: Array と Point の `hash` のネイティブは、Kernel の Array と Point の `hash` がじかに入れ子になった数が 4 以上なら、要素に送らない。Array は要素数だけから、Point は定数を答える。自分を要素に持つ Array や Point でも止まる。
- `hash` は値だけで決まり、呼ばれた文脈によらない。要素で見つかる `hash` のメソッドが Kernel の Array か Point の `hash` のネイティブでなければ（利用者のメソッド、ほかのクラスのネイティブ）、入れ子の数を 0 にしてから送り、戻ったら（失敗や巻き戻しで戻ったときも）元の数に戻す。したがって、利用者の `hash` が中で送る `hash` は、どの深さから呼ばれても同じ答えになる。
- 利用者のオブジェクトを経由する循環（利用者の `hash` が、自分を要素に持つ Array の `hash` を送る）は打ち切らない。再帰はスタックガード（§3.4）に当たり、評価は `stack overflow` で中断する。
- Character は即値なので、同一性の `=` と `identityHash`（スカラー値）で足りる。`=` を上書きしない Kernel クラス（Interval、Association、Rectangle、OrderedCollection、Dictionary、Set、ByteArray など）は、同一性の `=` と `hash`（`identityHash`）のままである。

`copy` と `shallowCopy`:

- `Object>>shallowCopy` と `Object>>copy` は、レシーバと同じクラス・同じ大きさの新しいオブジェクトに、スロット（バイト列ならバイト）をそのまま写して答える。即値はレシーバをそのまま答える。
- Symbol の `copy` と `shallowCopy` は、レシーバそのものを答える。intern した Symbol は綴りごとに 1 つで、セレクタの探索は同一性で比べるからである。`#printString copy == #printString` は true、`3 perform: #printString copy` は `'3'` である。
- Dictionary と Set（サブクラスを含む）の `copy` は、`array` も写す（Collections の「Dictionary と Set」）。写しとレシーバは表を共有しない。`shallowCopy` は Blue Book どおり浅く、`array` をレシーバと共有する。

#### Kernel-Classes

- `Behavior`
- `ClassDescription`
- `Class`
- `Metaclass`

必須セレクタ:

`Behavior`: `new`, `basicNew`, `basicNew:`, `superclass`, `methodDict`, `selectors`, `compiledMethodAt:`, `includesSelector:`, `inheritsFrom:`, `instSize`, `isVariable`, `isBytes`, `isPointers`

`Class`: `subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:`, `name`, `category`, `classPool`

`Metaclass`: `thisClass`, `name`, クラス側 `new` の禁止または制御

`basicNew:`:

- `basicNew: n` は、n が 0 以上の SmallInteger でなければ失敗する（§3.3）。可変長でないクラスは、n によらず instSize だけのスロットを割り当てる。可変長のポインタのクラスは instSize + n のスロットを、バイト列のクラスは n バイトを割り当てる。
- スロットの数とバイト数は 2^32 − 1 以下である。超えれば割り当てずに失敗する（理由は `failed: #basicNew:`。`new:` から送ったときは `failed: #new:`）。下位 32 ビットに切り詰めない。`(String new: 4294967299) size` は評価エラーである。
- 上限以内でも、GC をしても old に入らなければ（old の上限。§3.2）、評価を `out of memory` で中断する。`Array new: 4294967295` はこれに当たる。

メタクラス循環は Blue Book ルール 6–10 を満たすこと。

- すべてのクラスはメタクラスのインスタンス
- メタクラス階層はクラス階層に平行
- すべてのメタクラスは `Class` を継承（`Object class` のスーパークラスが `Class`）
- すべてのメタクラスは `Metaclass` のインスタンス
- `Metaclass class` も `Metaclass` のインスタンス

クラスの名前:

- クラスの `name` は intern した Symbol である。Kernel クラスも `subclass:` で作ったクラスも同じで、`Object name == #Object` と `3 class name == #SmallInteger` は true である。`subclass:` に名前を String で渡しても（`Object subclass: 'Foo' …`）、名前の枠と `Smalltalk` の登録には、同じバイト列を intern した Symbol を使う。
- メタクラスの `name` は、`thisClass` の名前のあとに ` class` を続けた String である（`Object class name` は `'Object class'`）。Kernel クラスのメタクラスもユーザークラスのメタクラスも、`thisClass` から計算する。メタクラスの名前の枠にも同じ文字列の String を置く（Browser と `printString` が読む）。

インスタンス変数の名前:

- 名前付きのスロットを足す Kernel クラスは、足したスロットの名前を instVarNames（Symbol の Array）に持つ。名前はネイティブが使うスロットの意味に合わせる。足すスロットと名前は次の表のとおりで、表に無い Kernel クラスはスロットを足さない（`ClassDescription`、`Class`、`Metaclass` は `Behavior` と同じ枠を持つ）。

| クラス | 足すスロットの名前（先頭から順） |
|---|---|
| `Behavior` | `superclass methodDict format name thisClass category classPool instVarNames` |
| `Fraction` | `numerator denominator` |
| `Interval` | `start stop step` |
| `Dictionary`, `Set` | `tally array` |
| `OrderedCollection` | `array firstIndex lastIndex` |
| `Association` | `key value` |
| `CompiledMethod` | `header literals bytecodes nativeCode selector methodClass` |
| `NativeMethod` | `selector argc primitive name methodClass registryIndex` |
| `Message` | `selector args` |
| `MethodDictionary` | `tally array` |
| `MethodContext` | `sender pc stackp method receiver argc` |
| `BlockContext` | `home copied` |
| `Process` | `nextLink suspendedContext priority myList` |
| `ProcessorScheduler` | `quiescentProcesses activeProcess` |
| `Semaphore` | `excessSignals linkedList` |
| `SharedQueue` | `contents readSynch writeSynch` |
| `Point` | `x y` |
| `Rectangle` | `origin corner` |
| `PositionableStream` | `collection position readLimit` |
| `WriteStream` | `writeLimit` |
| `SmalltalkImage` | `tally array` |

- インスタンス変数の名前と添字は、スーパークラス鎖を根から順にたどって決める。鎖の各クラスは、自分の instSize が 1 つ前のクラスの instSize より増えた分のスロットに、自分の instVarNames を先頭から順に当てる。名前が足りなければ、残りのスロットは名前を持たず、ソースから参照できない（コンパイラは識別子にならない仮の名前で埋める）。名前が足りなくなるのは、ユーザーがクラスの instVarNames を変えたとき（`OrderedCollection instVarAt: 8 put: nil` など）だけである。増えた分より多い名前は使わない。コンパイラ（§3.8）と `instVarNamed:` はこの規則に従う。これで、Kernel クラスのサブクラスが足す変数は、親のスロットと重ならない。
- `subclass:instanceVariableNames:…` は、スーパークラスがバイト列のクラス（`isBytes`）で、instanceVariableNames が 1 つ以上あれば、クラスを作らずに評価を中断する（§3.3）。理由は `bytes class cannot have instance variables` である。バイト列のオブジェクトには名前付きのスロットが無いからである。インスタンス変数の無いサブクラスは今までどおり作れる。
- Kernel クラスのクラス側の生成ネイティブ `Association key:value:`、`Point x:y:`、`Rectangle origin:corner:`、`Interval from:to:by:` は、`basicNew` と同じく、レシーバのクラスの instSize だけのスロットを割り当てる（format はランタイムと同じに読む。Kernel のスロットの数を下回らない）。Kernel のスロットに値を入れ、サブクラスが足したスロットは nil である。`Association subclass: #X instanceVariableNames: 'extra'` のあとの `X key: 1 value: 2` は 3 スロットで、`extra` を読み書きでき、そのインスタンスはクラスの format に合う（§3.11 のロードの検査に通り、保存できる）。
- `subclass:instanceVariableNames:…` は、instanceVariableNames の名前が、スーパークラス鎖の名前付きスロットの名前（上の規則で決まる名前。名前の無いスロットは数えない）にあるか、同じ instanceVariableNames に 2 度現れれば、クラスを作らずに評価を中断する（§3.3）。理由は `duplicate instance variable: <名前>` で、最初に当たった名前を示す。コンパイラは名前を先頭に近いスロットに解決するので、あとのスロットにはソースから届かないからである。検査はバイト列の検査のあと、何も割り当てる前にする。
- Kernel クラスが足したスロットは、コンパイルしたコードからは読み取り専用である。読みは今までどおりインスタンス変数として読む（`Association>>probeKey ^key` は key を読む）。代入はコンパイルエラーで、理由は `cannot assign to Kernel instance variable <名前>` である（§3.8）。ネイティブは、これらのスロットに決まった種類の値（`OrderedCollection` の `array` なら Array）があるものとして読み書きするので、ソースから別の値を入れるとネイティブが壊れるからである。
  - 読み取り専用のスロットは、メソッドのクラスのスーパークラス鎖（そのクラス自身を含む）でいちばん近い Kernel クラスの instSize までの、先頭のスロットである。サブクラスが継承したスロットも含む。クラス側のメソッドでは、鎖でいちばん近いのは Kernel クラスのメタクラス（`Object class` など）なので、Behavior の 8 つの枠（`superclass methodDict format name thisClass category classPool instVarNames`）が読み取り専用になる。vendor のスタブ（§3.12）は Kernel クラスに数えない。
  - 既知の制約（v1）: `instVarAt:put:` はこの制限を受けない。Kernel クラスのスロットに `instVarAt:put:` で合わない値を入れると、ネイティブは壊れうる。
  - classPool（Behavior の `classPool`）を壊れた表にしたとき（`tally` を容量より大きくする、`array` を配置に合わない値にするなど。Collections の「壊れた表」）は、ランタイムはそのクラスに classPool が無いものとして扱う。その名前はクラス変数として解決されず（§3.8）、Browser の定義テキスト（§3.10）にも並ばない。すでにコンパイルしたメソッドは束縛を持ち続け、読み書きできる。`classPool` の答えは下の「クラス変数」に書く。

クラス変数:

- `subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:` は、classVariableNames を空白で区切った名前ごとにクラス変数を作り、作ったクラスの classPool（`kClassSlotClassPool`）に置く。classPool は Kernel の `Dictionary` で、名前（intern した Symbol）から束縛への辞書である。束縛は `Association` で、キーは同じ Symbol、値はクラス変数の値（最初は nil）である。同じ名前を 2 度書いても束縛は 1 つである。クラス変数が無くても、classPool は空の `Dictionary` である。
- classPool は、ほかの `Dictionary` と同じハッシュ表の配置（Collections の「Dictionary と Set」）を持つ。ランタイムが classPool を作るときは、キーに `hash` を送らず、同じ値（`#名前 hash`。String と Symbol の値ベースの hash）を C++ で求めて入れる。したがって `Foo classPool at: #Count` も `Foo classPool at: 'Count'` も同じ束縛を引く。
- classPool の名前は、キーのうち Symbol のものだけである。Symbol でないキー（String、LargeInteger、ByteArray、Float など。`instVarAt:` で取り出した classPool に `at:put:` したとき）は名前にならない。クラス変数として解決せず（§3.8）、Browser の定義テキスト（§3.10）にも並べない。
- classPool の名前を並べるとき（Browser の定義テキスト、§3.10）は、名前のバイト列の昇順に並べる。ハッシュ表は書いた順を保たないからである。
- `Class>>classPool` はその辞書の写しを答える。写しは新しい `Dictionary` で、classPool の `tally` と配列の中身をそのまま写すので、同じ名前を同じ束縛（Association そのもの）に結ぶ。`Foo classPool at: #Count` は束縛で、その `value` がクラス変数の値である。`(Foo classPool at: #Count) value: 5` はクラス変数の値を変える。写しへの `at:put:` はクラスの classPool も束縛も変えない。メソッドのリテラルは classPool の束縛そのもので、再 Accept の検査（§3.9）もそれを前提にするので、束縛を別の値に差し替えさせないためである。送るたびに新しい写しを答える。
  - classPool の `array` が nil（空の表。`Foo instVarAt: 7 put: Dictionary basicNew` のあとなど）なら、新しい空の `Dictionary`（`Dictionary new` と同じ）を答える。枠の値そのものは答えない。
  - classPool が壊れた表なら（枠が `tally array` のスロットを持つポインタのオブジェクトで、Collections の「壊れた表」に当たるとき）、`damaged hashed collection` で失敗する。
  - classPool の枠がそれ以外（Kernel クラスの nil、SmallInteger、スロットが 2 つに満たないオブジェクトなど）なら、枠の値をそのまま答える。
- classPool のある名前の値が束縛（`Association`）でないとき（クラス側のメソッドがインスタンス変数 `classPool` を読んで `at:put:` した、`instVarAt:` で classPool そのものを取り出して書き換えた、など）、その名前をクラス変数として読むか代入するメソッドは入れない。Accept と file-in では、コンパイルエラーで、理由は `class variable <名前> is not bound to an Association` である。形の変更での移し替え（§3.9）では、送信のあとの検査と同じく、どのクラスも変えずに名前を旧クラスに戻して `AO_ERR_COMPILE` を返す。理由は同じ文の前に `shape change refused: <Class>>><selector>: ` を付けたものである。
- Kernel クラスはクラス変数を持たない（classPool は nil）。メタクラスの classPool の枠も nil である。
- メソッドから見えるクラス変数は、そのクラス（クラス側のメソッドなら、メタクラスの thisClass）とそのスーパークラス鎖のクラスの classPool にある名前である。近いクラスの名前が、遠いクラスの同じ名前を隠す。サブクラスのメソッドも、クラス側のメソッドも、同じ束縛を読み書きする。コンパイラの名前の解決順は §3.8 に書く。
- メソッドは束縛そのものをリテラルに持つ（§3.5 の LitVar 系）。classPool と束縛はヒープにあり、イメージを保存して読み直しても、値もメソッドとの共有も保つ（§3.11）。
- クラス変数の名前は `Smalltalk` の辞書（下の「グローバル辞書」）に登録しない。
- poolDictionaries は v1 では受け取って捨てる。プール辞書は無い。

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

必須セレクタ:

`Process`: `resume`, `suspend`, `terminate`, `priority:`

`ProcessorScheduler`: `activeProcess`, `yield`

`Semaphore`: `signal`, `wait`, クラス側 `new`

`SharedQueue`: `nextPut:`, `next`, クラス側 `new`

`BlockContext`（Kernel-Methods）: `fork`

意味は §3.4 の「プロセスと協調スケジューラ」に書く。

#### Collections

ネイティブ必須:

- `Collection`, `SequenceableCollection`, `ArrayedCollection`
- `Array`, `ByteArray`, `String`, `Symbol`
- `Interval`
- `Dictionary`, `IdentityDictionary`, `Set`, `IdentitySet`
- `OrderedCollection`
- `Association`

`Bag`, `LinkedList`, `MappedCollection` は P4 ではスタブ可。P9 までに `Collection` プロトコルの中核（`do:`, `collect:`, `select:`, `reject:`, `detect:ifNone:`, `inject:into:`, `includes:`, `size`, `isEmpty`）をネイティブまたは確実な転送で実装する。

Collection の列挙:

- `select:` と `reject:` は、レシーバに `do:` を 1 回だけ送り、述語のブロックを要素ごとに 1 回だけ呼ぶ。`select:` は答えが true の要素を、`reject:` は答えが false の要素を、渡された順に並べた Array を答える。レシーバの種類によらず（OrderedCollection、String、Set、Interval でも）答えは Array である。述語の答えが Boolean でなければ、§3.5 の分岐と同じく答えに `mustBeBoolean` を送り、その答えで決める。それも Boolean でなければ `NonBoolean receiver` で中断する（`Object>>mustBeBoolean` の既定も同じ理由で中断する）。`#(1 2 3) select: [:x | nil]` は評価エラーである。集めた要素は、`do:` に渡すネイティブのブロック（§3.11）のスロットにある Array に入れ、足りなくなれば大きさを倍にした Array に写す。述語に副作用があっても、答えは要素ごとに 1 回の評価どおりである。`#(1 2 3) select: [:x | Transcript show: x printString. true]` は 1 2 3 を 1 回だけ出す。`| oc | oc := OrderedCollection new. #(1 2 3) select: [:x | oc add: x. oc size <= 3]` は `#(1 2 3)` である。
  - そのブロックのスロット（集めた数と Array）は、ブロックを受け取った `do:` から書き換えられる。数が 0 以上その Array の大きさ以下の SmallInteger でないか、Array が Kernel の Array でなければ、`select: count out of range`（`reject:` では `reject: count out of range`）で失敗する。スロットは、ブロックが呼ばれるたびに、述語を呼ぶ前と、述語から戻って要素を入れる前に確かめる。述語を呼ぶ前に壊れていれば、述語は呼ばない。
- `detect: aBlock ifNone: exceptionBlock` も、述語の答えが Boolean でなければ `select:` と同じく `mustBeBoolean` を送る。
- `includes: anObject` は、`do:` で要素を順に見て、`anObject = 要素` が true になったところで true を答える（Blue Book。`=` は anObject に送る）。要素が anObject と同一なら、`=` を送らずに true とする。`=` の答えが Boolean でなければ失敗する（§3.3。理由は `failed: #includes:`）。`hash` は送らない。Dictionary の `includes:`（値を比べる）と同じ規則である。空のコレクションは何も送らずに false を答える。
- String の `do:` は、UTF-8 のバイト列を先頭から 1 回だけたどり、文字ごとにブロックを呼ぶ（`at:` を送らない。長さ n の文字列で O(n)）。渡した文字の数 k を数える。ブロックから戻るたびにレシーバのバイト数を読み直し、バイト数が変わったか、次に読む位置が多バイト文字の途中なら、そのときの文字列の k + 1 文字目の位置に合わせ直す（`at:` と同じ数え方で先頭から数える。k 文字に満たなければ終わる）。したがって、渡す文字はどれも、渡した時点の文字列に `at:` で読める文字である。文字列に無い文字（多バイト文字の途中の継続バイトなど）は渡さない。
  - 「多バイト文字の途中」は、次に読む位置の直前 3 バイト以内で、いちばん近い継続バイト（0x80〜0xBF）でないバイトから始まる正しい UTF-8 の列が、その位置を覆うことである（見るのは高々 3 バイト）。どの列にも覆われない継続バイト（はぐれた継続バイト）は、`at:` と同じくそれ自体が 1 文字なので、合わせ直さない。`| s i | s := 'say "hi" now' copy. i := 0. s do: [:c | i := i + 1. c = $" ifTrue: [s at: i put: $”]]. s` は `'say ”hi” now'` である。
  - ブロックが文字列を書き換えたとき、文字を飛ばすか 2 度渡すかは規定しない。
  - ブロックが文字列を書き換えなければ、UTF-8 として正しくないバイト列（`basicAt:put:` で書いたもの、UTF-8 でないソースの文字列リテラルから作ったものなど）でも O(n) である。ブロックが書き換えたときは、合わせ直すたびに O(n) かかることがある。幅の変わる `at:put:` は 1 回が O(n) なので、それで書き換えるなら全体の時間は書き換えにかかる時間の定数倍を超えない。`basicAt:put:` の書き換えは 1 回が O(1) なので、毎回合わせ直させると全体は O(n²) になりうる（`String new: 40000` の do: の中で毎回 2 バイトを多バイト文字に書き換えるなど）。
  - ループは 64K 回ごとに safepoint を通る。レシーバのクラスで見つかる `at:` か `size` が Kernel の String のネイティブでなければ（利用者が上書きしたサブクラス）、ArrayedCollection の `do:` と同じく `size` と `at:` を送って回す。

Dictionary と Set:

- `Dictionary`、`IdentityDictionary`、`Set`、`IdentitySet` は、開番地法（線形プローブ）のハッシュ表である。スロットは `tally array`（Kernel-Classes の表）で、`tally` は要素の数、`array` はエントリを並べた Array である。
- エントリの幅は、Dictionary と IdentityDictionary が 3 スロット（`key value hash`）、Set と IdentitySet が 2 スロット（`element hash`）である。`hash` は、キー（Set では要素）のハッシュ値を保存した SmallInteger である。キー（要素）が nil のエントリは空きである。
- 容量（エントリの数）は 8 以上の 2 のべき乗で、`array` の大きさは容量×幅 + 1 である。エントリは先頭から並び、最後の 1 スロットは表の世代番号（SmallInteger）である。`new` は容量 8、世代番号 0 の空の表を作る。
- 世代番号: ネイティブがエントリを足すか消すたびに（挿入、削除）1 増やし、SmallInteger の最大値の次は 0 に戻す。値の置き換え（`at:put:` で既にあるキーに結ぶ）はエントリを動かさないので変えない。拡張は新しい `array` に替えるので、世代番号は 0 から始める。世代番号は再入の検出（下の「再入」）だけに使う。
- ハッシュ値:
  - Dictionary と Set は、1 回の操作でキーに `hash` を 1 回だけ送る。送り方（入れ子の数の扱い）は、Array の `hash` が要素に送るときと同じである（Kernel-Objects の「`=` と `hash`」）。答えが Integer でなければ失敗する（§3.3）。答えが LargeInteger なら、その `hash` の値を保存する。送った先で巻き戻しが始まったら、表に触れずに直ちに空 OOP を返す（§3.4）。
  - IdentityDictionary と IdentitySet は送信しない。`identityHash` と同じ値を使う。
- ホーム: 容量を c = 2^k とすると、hash のホーム（探索を始めるエントリ）は、hash を 64 ビットの符号なし整数として `0x9E3779B97F4A7C15` を掛け（2^64 を法とする）、上位 k ビットを取った値 `(hash × 0x9E3779B97F4A7C15 mod 2^64) >> (64 − k)` である。下位ビットのそろった hash（`i * 4096` の SmallInteger や、65535 以下の identityHash を容量 131072 の表に入れたとき）でも、ホームが表全体に散らばるようにするためである。混ぜるのはホームを求めるときだけで、エントリに保存する hash は混ぜる前の値である。拡張と削除は、保存した hash からホームを求め直す。classPool（Kernel-Classes のクラス変数）も同じホームを使う。
- 探索: キーの hash のホームのエントリから始めて 1 つずつ後ろへ見る（最後の次は先頭）。空きに当たれば、キーは無い。保存した hash がキーの hash と等しいエントリだけを比べる。
  - Dictionary と Set は、エントリのキーがキーと同一（`==`）なら一致とし、そうでなければキーに `=` を送る（`キー = エントリのキー`）。答えが Boolean でなければ失敗する（§3.3）。
  - IdentityDictionary と IdentitySet は `==` だけで比べる。
- 挿入: キーが無ければ、入れたあとの要素の数が容量の 3/4 を超えるとき、先に容量を 2 倍にする（拡張）。そのあと、キーの hash の位置から探した最初の空きに入れる。
- 拡張は新しい配列を作り、各エントリを保存した hash で入れ直し、`tally` を入れ直した数にしてから `array` を差し替える。削除は後方シフトで行い、空きの印を残さない。空いた場所を i として、その後ろのエントリを、空きに当たるか、見る場所が i に戻るまで順に見る（i に戻るのは、`tally` が実際の数と違って空きの無い表から消すときだけである）。エントリのホームが、巡回した順で i より後ろ、そのエントリ以前にあれば動かさない。そうでなければ、そのエントリを i へ移し、移したもとの場所を新しい i とする。終わったら i を空きにする。どちらで終わっても、残ったエントリはどれも、ホームから空きを通らずに届く。拡張も削除も `hash` と `=` を送らない。利用者のコードが走らないので、途中で失敗も巻き戻しも起きない。
- 再入: `hash` と `=` は利用者のメソッドでもよく、その中で同じ表を書き換えうる（`at:put:`、`removeKey:`、拡張による `array` の差し替え、`instVarAt:put:`）。ネイティブは送信から戻るたびに、レシーバから表を読み直す。`array` が差し替わったか、世代番号が変わっていれば、求めた hash のまま初めから探し直す（`hash` は送り直さない）。削除と再挿入で元の見かけ（同じ `array`、同じ `tally`、同じ位置の同じキー）に戻しても、世代番号で見つける。世代番号は表ごとなので、ほかの表を書き換える `=`（ログやキャッシュ）では探し直さない。同じ表でも、値の置き換えだけなら探し直さない。`instVarAt:put:` や `array` への直接の `at:put:` で、世代番号を通さずにエントリや `tally` を書き換えたときの結果は規定しない（Kernel-Classes の既知の制約。メモリの安全だけを保つ）。どの書き換えのあとも、配列の範囲外は読み書きしない。探し直しの回数に上限は無い。`=` が送られるたびに表を書き換えれば、探索は終わらない（利用者のループと同じく、停止は保証しない）。探し直しても、`hash` は 1 回の操作で 1 回しか送らない。
- 壊れた表:
  - `array` が nil なら空の表として扱う（`Dictionary basicNew` など）。`size` は 0 で、最初の挿入で容量 8 の配列を作り、`tally` を 0 から数える。
  - `array` がポインタのオブジェクトでないか、大きさが「8 以上の 2 のべき乗×幅 + 1」でないか、最後のスロット（世代番号）が SmallInteger でないか、`tally` が 0 以上容量以下の SmallInteger でなければ（`instVarAt:put:` で壊したときなど）、Dictionary と Set のネイティブはどれも失敗する。理由は `damaged hashed collection` である。
  - `tally` が実際の数と違うだけなら、探索、挿入、削除、列挙は失敗しない。挿入で空きが見つからなければ拡張し、拡張は `tally` を数え直す。削除は `tally` を 1 減らす（0 より小さくしない）。ただし `size` は `tally` を答え、`tally` を使うネイティブ（Dictionary の `collect:` と、`size` を送る Collection の `collect:`）は `tally` を信じる。`tally` が実際の数より小さければ `collect:` は失敗しうる。大きければ、答えの Array の末尾に nil が残る。`tally` を実際と違う値にできるのは `instVarAt:put:` だけである（Kernel-Classes の既知の制約）。保存した hash が SmallInteger でないエントリは、どのキーとも一致せず、拡張と削除では hash を 0 として扱う。
- 列挙（`do:`、`keysDo:`、`associationsDo:`、`keysAndValuesDo:`、`collect:`、値の `includes:`）は、`array` を先頭から順に見る。順序は規定しない。ブロック（`includes:` では `=`）が表を書き換えても、ネイティブはエントリごとに表を読み直し、そのときの `array` の大きさの範囲で続ける。そのとき要素を飛ばしたり 2 度渡したりすることがあるが、どうなるかは規定しない。列挙のループも、空きを含めてエントリを 64K 個見るごとに safepoint を通る（疎な大きい表でも通る）。

Dictionary のプロトコル（Blue Book）。IdentityDictionary は、キーを探すセレクタ（`at:`、`at:put:`、`at:ifAbsent:`、`includesKey:`、`removeKey:`、`removeKey:ifAbsent:`）を同一性版で持ち、ほかは Dictionary のものを使う。

| セレクタ | 動作 |
|---|---|
| `at: key` | key の値。無ければ nil |
| `at: key put: value` | key を value に結び、value を答える。key が nil なら失敗し、理由は `key must not be nil` |
| `at: key ifAbsent: aBlock` | key の値。無ければ `aBlock value` の答え |
| `includesKey: key` | key があれば true |
| `removeKey: key` | key のエントリを消し、その値を答える。無ければ失敗し、理由は `key not found` |
| `removeKey: key ifAbsent: aBlock` | key のエントリを消し、その値を答える。無ければ `aBlock value` の答え |
| `includes: anObject` | 値のどれかについて `anObject = 値` が true なら true（同一なら送らない）。`hash` は送らない。`=` の答えが Boolean でなければ失敗する |
| `do: aBlock` | 値ごとに `aBlock value: 値`。Association は渡さない |
| `keysDo: aBlock` | キーごとに `aBlock value: キー` |
| `associationsDo: aBlock` | エントリごとに、キーと値を持つ新しい Association を作って渡す。それを書き換えても表は変わらない |
| `keysAndValuesDo: aBlock` | エントリごとに `aBlock value: キー value: 値` |
| `collect: aBlock` | 値ごとの `aBlock value: 値` の答えを並べた Array。大きさは送ったときの `tally` である（`tally` が実際の数と違うときは「壊れた表」）。ブロックが表を書き換えたときの答えは規定しない（失敗することもある） |
| `size` | `tally` |
| `copy` | レシーバと同じクラスの新しいオブジェクトに、スロットをそのまま写したもの。ただし `array` がヒープのオブジェクトなら、それも同じクラス・同じ大きさの新しいオブジェクトに写して入れる（壊れた表でも失敗しない）。写しとレシーバは表を共有しないので、`e := d copy. e at: 2 put: 2` のあとも `d at: 2` は nil のままである。`shallowCopy` は浅く、`array` を共有する |

Set と IdentitySet のプロトコルは `add:`、`includes:`、`do:`（要素ごと）、`size`、`copy`（Dictionary と同じく `array` も写す）である。`add: anObject` は、anObject が無ければ入れ、anObject を答える。anObject が nil なら失敗し、理由は `element must not be nil` である。

- nil はキーにも要素にもならない。nil には `hash` を送らない。`at: nil` は nil、`includesKey: nil` と Set の `includes: nil` は false、`at: nil ifAbsent:` と `removeKey: nil ifAbsent:` はブロックの答えである。`removeKey: nil` は `key not found` で失敗する。
- Dictionary の `do:` は値を渡すので、`do:` を通る Collection のネイティブ（`select:`、`reject:`、`detect:ifNone:`、`inject:into:`）も値を受ける。

Interval:

- `Interval from: start to: stop by: step` の要素は、start、start + step、start + step + step と続き、終端を越えたところで終わる。`do:` はこの順に要素を渡し、`size` は要素の数を答える。
- start、stop、step がどれも Integer（SmallInteger、LargePositiveInteger、LargeNegativeInteger）なら、`size` は送信せずに厳密に求める。step が 0 なら 0 である。step が正で stop < start のときと、負で stop > start のときも 0 である。それ以外は `(stop - start) // step + 1` である。答えは SmallInteger に収まれば SmallInteger、収まらなければ LargeInteger である（`(1 to: (1 bitShift: 70)) size` は `1 bitShift: 70`）。どれも SmallInteger なら、`do:` も送信せずに数える。
- そうでなければ（`do:` では、どれかが SmallInteger でなければ）、まず刻みの向きを決める。step が SmallInteger ならその符号で決める。そうでなければ `step < 0` を送り、true なら後ろ向きである。false なら `step > 0` を送り、true なら前向き、false なら要素は無い（刻み 0 と同じ）。どちらも答えが Boolean でなければ失敗する（§3.3）。
- 前向きは `要素 <= stop`、後ろ向きは `要素 >= stop` を送り、答えが true の間だけ続け、false になったところで終わる。答えが Boolean でなければ失敗する。次の要素は `要素 + step` を送って求める。それが失敗すれば失敗する。比べられない値（NaN）との比較は false なので、端点が NaN なら要素は無い。`nan := 0.0 / 0.0` として、`(Interval from: 1 to: nan by: 1)` と `(Interval from: nan to: 5 by: 1)` の `size` は 0 で、`do:` はブロックを呼ばない。
- 要素の数に上限は無い。`size` は数えた数が SmallInteger を超えれば LargeInteger を答える。ループはネイティブのループの規則（Kernel-Methods。64K 回ごとに safepoint）に従い、ブロックの abort や巻き戻しで止まる。
- `(Interval from: 2.0 to: 1.0 by: -0.5) size` は 3 で、`collect:` は 3 要素の Array を答える。`(Interval from: 1 to: 2 by: 0.5) do: aBlock` は 1、1.5、2 で aBlock を呼ぶ。Fraction の刻みも同じである（`(Interval from: 0 to: 1 by: 1/2) size` は 3）。
- `collect:`（Collection の `collect:`）は、`size` を送って答えの Array を作ってから、`do:` で要素を集める。どれかが SmallInteger でなければ、`do:` は比較と `+` を送り、Integer でなければ `size` も送る。それらに副作用があって、`size` と `do:` の要素の数が食い違うときの `collect:` の答えは規定しない（失敗することもある）。`size` が大きすぎれば、Array を作る `basicNew:` で失敗する。

OrderedCollection:

- スロット `array firstIndex lastIndex`（Kernel-Classes の表）は、要素が `array` の firstIndex 番目から lastIndex 番目にあることを表す。`size` は lastIndex − firstIndex + 1 である。
- `array` が nil なら空である（`basicNew` のあとなど）。`size` は 0 で、最初の `add:` が大きさ 8 の Array を作り、firstIndex を 1、lastIndex を 0 にしてから入れる。
- `add:` で `array` の末尾に空きが無いとき、`size` の 2 倍が `array` の大きさ以下なら、要素を `array` の先頭へ詰めてから入れる（`array` を伸ばさない）。そうでなければ、2 倍（8 未満なら 8）の大きさの Array に写してから入れる。先頭から取り出して末尾に足す使い方（実行可能キュー。§3.4）でも、`array` は際限なく伸びない。
- そうでなければ、`array` が Kernel の Array で、firstIndex と lastIndex が SmallInteger で、`1 <= firstIndex <= lastIndex + 1 <= (array の大きさ) + 1` が成り立たなければ（`instVarAt:put:` で壊したとき）、`size`、`do:`、`add:`、`at:` はどれも失敗する。理由は `damaged ordered collection` である。Array の範囲の外は読み書きしない。
- `at: index` は、index が 1 以上 `size` 以下の SmallInteger でなければ失敗する（§3.3）。理由は String の `at:` と同じ `at: index out of range` である。組が壊れていれば、index によらず `damaged ordered collection` で失敗する。
- `do:` は、送ったときの firstIndex から lastIndex までの添字の要素を順に渡す（Blue Book）。ブロックを呼ぶ前に毎回組を読み直し、壊れていれば失敗する。次の添字がそのときの firstIndex から lastIndex の外なら、そこで終わる。ループは 64K 回ごとに safepoint を通る。

#### Magnitude

- `Magnitude`
- `Number`, `Integer`, `SmallInteger`, `LargePositiveInteger`, `LargeNegativeInteger`
- `Float`（IEEE754 binary64）
- `Fraction`（P4 は四則の最小）
- `Character`
- `Date` / `Time` は P9 までスタブ可

`SmallInteger` の `+ - * // \\ quo: rem: bitAnd: bitOr: bitXor: bitShift: = < > <= >=` はネイティブ。オーバーフローは `LargeInteger` へ透過。`Integer>>timesRepeat:` もネイティブ。

数の演算:

- Integer（SmallInteger と LargeInteger）、Fraction、Float の `+ - * /` は、どの組み合わせでも答える。答えは一般性の高いほう（Integer < Fraction < Float）の型である。Integer と Float、Fraction と Float は Float。Integer と Fraction は Fraction で、正規化し、分母が 1 なら Integer。Integer どうしの `/` は、割り切れれば Integer、でなければ Fraction。`1 + 1.5` は 2.5、`(1/2) + 0.5` は 1.0、`1 + (1/2)` は `3/2`。
- Integer と Fraction を Float にするときは、IEEE754 binary64 の最近接偶数丸めで正しく丸める。LargeInteger の全域を扱い、範囲を超えれば ±inf である。
- 0 で割ると: Integer と Fraction を 0（Integer）で割ると `division by zero` で中断する。Float が関わる割り算は IEEE754 に従う（`1 / 0.0` は inf、`0.0 / 0.0` は NaN）。
- `//` `\\` `quo:` `rem:` `bitAnd:` `bitOr:` `bitXor:` `bitShift:` は Integer どうしの演算である。引数が Integer でなければ失敗する（§3.3）。Float と Fraction はこれらを持たない。
- `bitShift:` のシフト量が -2^24 より小さい（-2^63 や負の LargeInteger を含む）なら、答えはレシーバが負なら -1、でなければ 0。シフト量が 2^24 より大きければ失敗する。`5 bitShift: (-1 bitShift: 63)` は 0、`-5 bitShift: (-1 bitShift: 63)` は -1。

数の比較:

- Integer、Fraction、Float の `< > <= >=` は、値を厳密に比べる。Float を Integer に、Integer を Float に丸めてから比べることはしない（`1152921504606846976.0 < 1152921504606846977` は true）。NaN との比較はどれも false（`(0.0/0.0) >= 1.0` も `1.0 >= (0.0/0.0)` も false）。+inf はどの有限の数より大きく、-inf は小さい。
- `=` は型をまたぐと false のままである（Kernel-Objects の「`=` と `hash`」）。したがって `1 <= 1.0` は true だが、`1 = 1.0` は false である。
- 引数が Integer、Fraction、Float のどれでもなければ、`<` は失敗する。`> <= >=` は Magnitude の既定（`>` は `引数 < レシーバ`、`<=` は `<` と `=`、`>=` は `<` の否定）で答える。
- `Magnitude>>max:` は `self > 引数` が true ならレシーバ、false なら引数を答える。`min:` は `self < 引数` で同じように選ぶ（Blue Book）。比較の答えが Boolean でなければ失敗する。`between:and:` は `>=` と `<=` を送る。

`to:do:`:

- `Integer>>to:do:` のネイティブ（インライン展開しない送信。§3.5）は、レシーバと終端がともに SmallInteger なら数えて回す。そうでなければ（終端が Float、Fraction、LargeInteger など）、ループ変数に `<= 終端` を送って続けるかを決め、`+ 1` で進める。§3.5 のインライン展開と同じ意味である。`<=` の答えが Boolean でなければ、§3.5 の分岐と同じく `mustBeBoolean` を送る。`1 to: 2.5 do: aBlock` は 1 と 2 で aBlock を呼ぶ。

Character:

- `Integer>>asCharacter` は Unicode スカラー値（0 以上 0x10FFFF 以下で、サロゲート U+D800–U+DFFF を除く）だけを受け付ける。それ以外は失敗する（§3.3）。

#### Graphics-min

- `Point`, `Rectangle`

描画プリミティブ（BitBlt, Form, Display）は v1 の対象外。座標計算だけホストツールが使う。

- Point と Rectangle のネイティブは、それぞれのサブクラスのインスタンスも同じに扱う（クラスの一致ではなく `inheritsFrom:` と同じ判定）。
- Point の算術（`+ - * //`）は成分ごとに送り、答えは Point である。成分の計算が失敗したら（`(Point x: 1 y: 2) + nil`）、Point を作らずに失敗する（§3.3）。
- Rectangle の `containsPoint:` と `intersect:` は成分を `<=` と `<` で比べる。比べた答えが Boolean でなければ（失敗の空 OOP を含む）、false や成分の代わりにせず失敗する（§3.3）。比べた先で巻き戻しが始まったら、直ちに空 OOP を返す（§3.4）。

#### Streams / System

- `Stream`, `PositionableStream`, `ReadStream`, `WriteStream`, `ReadWriteStream`
- `Transcript` モデル（実際の出力先はホストウィンドウ）
- `SmalltalkImage`（グローバル辞書のホスト。クラシック `Smalltalk`）

ストリーム:

- スロットは `collection position readLimit`（PositionableStream）と `writeLimit`（WriteStream）である（Kernel-Classes の表。これ以上足さない）。position、readLimit、writeLimit は要素（String では文字）の数である。position は読んだか書いた要素の数、readLimit は読める要素の数、writeLimit は collection の大きさ（書ける要素の数）である。
- `on: aCollection` は、position を 0 に、readLimit と writeLimit を `aCollection size` にする。
- `next` は、position が readLimit より小さければ position を 1 進め、`collection at: position` の答えを答える。そうでなければ nil を答える。
- `position: anInteger` は、anInteger が SmallInteger でなければ `position: not an integer` で失敗する。負なら 0 に、readLimit と今の position の大きい方を超えれば、その値にする（頭打ちにし、失敗しない）。writeLimit までは進めない。ストリームが作った String の予備や、倍にした Array の空きを見せないためである（Squeak の `WriteStream>>position:` と同じ）。`(WriteStream on: String new) nextPutAll: 'abc'; position: 100; position` は 3 である。readLimit か position が SmallInteger でなければ 0 として扱う。
- `nextPut:` は WriteStream（と ReadWriteStream）のメソッドである。PositionableStream は持たない。ReadStream の `nextPut:` は `shouldNotImplement` で失敗し、collection も position も変えない。`| s r | s := 'abc' copy. r := ReadStream on: s. r nextPut: $x` は評価エラーで、s は `'abc'` のままである。
- WriteStream の `nextPut: anObject` は、collection の position + 1 番目を anObject にし、position を 1 進め、readLimit を position 以上にして、anObject を答える。書けなければ失敗し（§3.3）、collection の中身、position、readLimit を変えない。
  - collection が Array で、position + 1 が大きさを超えるとき: 大きさを倍（それでも足りなければ position + 1）にした Array に写して collection を差し替え、writeLimit を新しい大きさにする。倍が 2^32 − 1 を超えるなら 2^32 − 1 にする。position + 1 が 2^32 − 1 を超えるなら（`instVarAt:put:` で position を書き換えたとき）、割り当てずに `out of memory` で中断する。
  - collection のクラスで見つかる `at:put:` と `size` がどちらも Kernel の String のネイティブ（`String>>at:put:` と `String>>size`）なら: 下の「String への書き込み」。どちらかを上書きしたサブクラスは、次の場合に当たる。
  - それ以外は `collection at: position + 1 put: anObject` を送る。position + 1 が `collection size` を超えれば `nextPut: past end` で失敗する。ただし、collection が String の系統で、anObject が Character で、position が `collection size` に等しいときは、末尾に文字を足した新しい String に collection を差し替える。新しい String のクラスは collection のクラスである（Symbol の系統なら String）。String のサブクラスを String に変えない。
- `contents` の要素の数 k は、ReadStream（PositionableStream）では readLimit、WriteStream では position、ReadWriteStream では readLimit と position の大きい方である（Blue Book）。SmallInteger でない値と負の値は 0 とする。`| w | w := ReadWriteStream on: String new. w nextPutAll: 'abc'; reset. w contents` は `'abc'` である。
- `contents` は、`collection at: 1` から `collection at: k` までを送って要素を取り出し、新しいコレクションに並べて答える。collection の物理スロットは写さない。k が collection の大きさを超えれば `at:` が失敗するので、`contents` も失敗する。答えの種類は collection で決まる。
  - collection のクラスで見つかる `at:` が Kernel のネイティブで、k 番目の `at:` が失敗すると先にわかるときは、答えを割り当てる前に、`at:` と同じ理由で失敗する（`instVarAt:put:` で readLimit を 2^28 にしても、2 GB を割り当ててから失敗しない）。Array と ByteArray の系統（`ArrayedCollection>>at:`）は k が要素の数（basicSize − instSize）を超えれば `basicAt: index out of range`、OrderedCollection は k が `size` を超えれば `at: index out of range`、String（`String>>at:`）は k が文字数を超えれば `at: index out of range` である。
  - String の系統: collection と同じクラスの String。ただし Symbol（とそのサブクラス）なら String。collection のクラスで見つかる `at:` が Kernel の `String>>at:` のネイティブなら、`at:` を送らずに UTF-8 を先頭から 1 回だけたどり、k 文字目までのバイト列を写す（同じ文字列になる。長さ n で O(n)）。k 文字に満たなければ `at: index out of range` で失敗する。
  - ArrayedCollection の系統で可変長のクラス（Array、ByteArray とそのサブクラス）: collection と同じクラス。`basicNew:` と同じく、instSize のスロット（名前付き変数。nil のまま）と k 個の要素を割り当てる。`Array subclass: #PG instanceVariableNames: 'tag'` のインスタンスの `contents` は PG で、要素はずれない。
  - OrderedCollection の系統: OrderedCollection（Kernel のクラス）。
  - それ以外（利用者のコレクションなど）: Array。`at:` を持たないコレクション（Kernel の Interval など）は `doesNotUnderstand: #at:` で失敗する。
  - 取り出した要素が答えに入らないとき（String に Character でないもの、ByteArray に 0 以上 255 以下の SmallInteger でないもの）は、`contents: element out of range` で失敗する。

String への書き込み（WriteStream の `nextPut:`）:

- `at:put:` を送らずに、collection の UTF-8 のバイト列を直接書き換える。anObject が Character でなければ `nextPut: value out of range` で失敗する。
- writeLimit は collection の文字数である。ストリームは、collection の後ろに予備（容量）を持つことがある。予備は readLimit + 1 文字目から writeLimit 文字目までで、どれも 1 バイトの文字（NUL）である。`on:` のあと、readLimit と writeLimit は等しいので、利用者が `on:` に渡した String は予備を持たない。予備は、ストリームが作った String にだけある。
- 書く文字の位置（position + 1 文字目のバイト位置）は、次の順に求める。1 と 2 は、collection の文字数が writeLimit であることを前提にする。
  1. collection のバイト数が writeLimit に等しいなら、どの文字も 1 バイトなので、position がそのままバイト位置である。
  2. position が readLimit 以上なら、position より後ろは予備で、どの文字も 1 バイトなので、バイト数 − (writeLimit − position) がバイト位置である。位置が 0 以上バイト数以下で、末尾でなければそのバイトが 1 バイトの文字で、その直前で文字が切れていることを確かめる。
  3. 1 と 2 が当てはまらないか、2 の確かめが合わないなら、先頭から文字を数える（遅いが正しい）。position より文字が少なければ `nextPut: past end` で失敗する。
- 位置に文字があり、書く文字と UTF-8 の幅が同じなら、その場で書き換える（`at:put:` と同じ。collection は差し替えない）。
- 幅が違うか、位置が末尾なら:
  - position が readLimit 以上で、位置から書く文字の幅だけのバイトがどれも 1 バイトの文字（予備）なら、その場で書き、文字数を（幅 − 1）減らす。writeLimit も（幅 − 1）減る。
  - position が readLimit より小さければ（書いた文字の上書き。writeLimit が 0 以上の SmallInteger でないときも）、その 1 文字だけを置き換えた新しい String を作り、collection を差し替える。文字数は変わらない。
  - それ以外（末尾、または予備が足りない）は、位置までのバイト列、書く文字、予備を並べた新しい String を作り、collection を差し替える。新しい String のバイト数は、位置までのバイト数と書く文字の幅の和の 2 倍（16 以上。8 の倍数に切り上げる）で、残りが予備である。writeLimit は新しい文字数（position + 1 + 予備の文字数）になる。それが SmallInteger の最大値を超えるなら（`instVarAt:put:` で position を書き換えたとき）、超えない数まで予備を削る。writeLimit は SmallInteger のままで、文字数と等しい。
  - 新しい String のクラスは collection のクラスである。利用者が `on:` に渡した String は、差し替えたあとは書き換えない。
- `| w | w := WriteStream on: (String new: 8). w nextPutAll: 'ééé'. w contents` は `'ééé'`、`w position` は 3 である。`WriteStream on: String new` に `nextPut:` を n 回送る時間は、書く文字によらず O(n) である。O(n) を約束するのは、この末尾への追記だけである。
  - readLimit より前（書いた文字や、利用者が `on:` に渡した String の中）に書くときは、position より前に多バイト文字があれば、位置を先頭から数えるので 1 回が O(position) である。幅の違う文字で上書きするときは String を作り直すので 1 回が O(大きさ)である。`reset` してから多バイト文字を書き直すことを繰り返すと、全体は 2 乗の時間になる（`reset` して 'あ' を 4 万回書き直すと Release で 4 秒）。
- 利用者が `on:` に渡した String を `at:put:` で書き換えても（幅が変わっても）、文字数は変わらないので、ストリームは正しい位置に書く。次のときに書く位置は規定しない（メモリの外は読み書きしない）: `basicAt:put:` で UTF-8 のバイト列を変えて文字数を変えたとき、ストリームが作った String を `instVarAt:` などで取り出して予備の文字の幅を変えたとき、`instVarAt:put:` で collection、position、readLimit、writeLimit を書き換えたとき（Kernel-Classes の既知の制約）。

グローバル辞書:

- グローバル `Smalltalk` は `SmalltalkImage` のインスタンスで、グローバル名（intern した Symbol）から値への辞書を持つ。グローバル名の解決は、すべてこの辞書を引く。インタプリタの `PushGlobal`（実行時に引く。無い名前は nil）、クラス定義・Browser・file-in の名前の解決（`WellKnown::named`）、ワークスペースの `knownGlobals`（§3.10）である。
- 辞書は `Smalltalk` の 2 つのスロット `tally array` に置く。`tally` は対の数、`array` は Array で、キーと値を交互に並べる。キーが nil の対は空きである。対は登録した順に並ぶ。v1 ではグローバルを消さない。
- ブートストラップ（§3.7）は、56 個の Kernel クラスをクラス名で、`Processor` をスケジューラで、`Smalltalk` を辞書自身で登録する。この 58 個の名前を固定のグローバルと呼ぶ。固定のグローバルの値は、vendor のスタブを file-in が結び直すとき（§3.12）のほかは変わらない。
- `subclass:instanceVariableNames:…` は、作ったクラスをその名前（intern した Symbol）で登録する。同じ名前があれば値を置き換える。固定のグローバルの名前なら登録しない。擬変数の名前（`nil true false self super thisContext`。§3.8）も登録しない。
- 擬変数の名前は、`Smalltalk` の辞書のキーにならない。コンパイラは擬変数をグローバルより先に解決するので（§3.8）、辞書に入れてもソースから読めず、`Smalltalk at: #nil` だけが別の値を答えることになるからである。
- 次のネイティブは、レシーバによらず `Smalltalk` の辞書を読み書きする（`SmalltalkImage new` も同じ辞書を指す）。キーは Symbol か String で、同じバイト列の Symbol として扱う。

| セレクタ | 動作 |
|---|---|
| `at: key` | key の値。key が無ければ評価を中断する（§3.3）。理由は `key not found: #<名前>`（key が Symbol でも String でもなければ `key not found`） |
| `at: key put: value` | key を value に結び、value を答える。key が固定のグローバルなら中断し、理由は `cannot rebind Kernel global: <名前>`。key が擬変数の名前なら中断し、理由は `cannot bind pseudo-variable: <名前>`。key が Symbol でも String でもなければ `key must be a Symbol or String` |
| `at: key ifAbsent: aBlock` | key の値。key が無ければ `aBlock value` の答え |
| `includesKey: key` | key があれば true。key が Symbol でも String でもなければ false |
| `globals` | レシーバ |

### 3.7 ブートストラップ手順

循環（`Object` のクラスは `Object class`、`Object class` のクラスは `Metaclass`、…）を Smalltalk ソースから作れない。C++ で手書きする。

順序:

1. ヒープを作る。
2. `nil` / `true` / `false` の即値または専用オブジェクトを登録する。
3. クラスオブジェクトの骨格を未初期化で割り当て、well-known 表に載せる。
4. 各クラスの `superclass` / `methodDict` / `format` / メタクラスリンクを結ぶ。
5. 循環を結んだあと（`Symbol`、`String`、`Array` のクラスが結ばれてから）、クラスの名前の枠に intern した Symbol を、メタクラスの名前の枠に `<クラス名> class` の String を、名前付きのスロットを足す Kernel クラスの instVarNames に §3.6 の表の名前（Symbol の Array）を入れる。
6. `NativeMethod` を関数ポインタから生成し、各メソッド辞書へ `Symbol` キーで入れる。
7. グローバル `Smalltalk`（`SmalltalkImage` の辞書。§3.6）を作り、Kernel クラスをクラス名で、`Processor` と `Smalltalk` 自身を登録する。
8. スナップショット可能にする。
9. 以降の非 Kernel クラスは `image/vendor/` の file-in で追加する。自作しない。

well-known 表は `include/ao/WellKnown.hpp` に列挙し、テストから名前で参照する。

### 3.8 コンパイラ

入力: Smalltalk-80 メソッドソースおよびチャンク形式（`!Class methodsFor: 'protocol' stamp: '...'!`）。

出力: `CompiledMethod` またはクラス定義の副作用。

要求:

- カスケード `;`。各パートは、単項 → 二項 → キーワードの連鎖である。各パートの最初のメッセージはカスケードのレシーバへ送り、2 つ目以降のメッセージはその結果へ送る（`x add: 3; yourself size` の 2 つ目のパートは `(x yourself) size`）。レシーバが `super` なら、どのパートの最初のメッセージも super 送信である。
- キーワードセレクタ、バイナリセレクタ、ユニタリ
  - バイナリセレクタの文字に `,` を含む（`'a' , 'b'`、`#,`、メソッドパターン `, other`）。
  - バイナリセレクタの 2 文字目以降の `-` は、直後が数字ならセレクタに含めない。そこから負の数のリテラルである（`2*-1` は `2 * -1`、`3@-2` は `3 @ -2`）。
- ブロック `[:a :b | ...]`
  - 引数があれば、引数の並びのあとの `|` は必須である。そのあとに temp 宣言 `| t |` を続けてよい。`||` は「区切り + temp 宣言の開き」と読む（`[:x | | t | ...]` と `[:x || t | ...]` は同じ）。区切りのあとの本体を temp 宣言と読み違えない（`[:a :b | a | b]` の本体は `a | b`）。
- 一時変数 `| x y |`
- 宣言: 同じスコープ（メソッドの引数と temp、1 つのブロックの引数と temp）で同じ名前を 2 回宣言したら、コンパイルエラー `duplicate name: <名前>` にする。擬変数の名前を引数や temp に宣言したら、コンパイルエラー `cannot declare pseudo-variable: <名前>` にする。
- 入れ子（括弧、ブロック、リテラル配列）の深さは 256 段までである。これとは別に、式の木の深さは 1024 段までである。木の深さは、括弧、ブロック、リテラル配列のほか、代入の値（`a := b := 3` の `b := 3`）と、メッセージの連鎖の受け手（`a b c` の `a b`、`1 + 2 + 3` の `1 + 2`）でも 1 段深くなる。どちらを超えても、コンパイルエラー `nesting too deep` にする。
- リテラル: 数、文字列、シンボル、文字、配列 `#( )`、バイト配列
  - 整数リテラルに桁の上限は無い。SmallInteger に入らない値は LargeInteger である（`100000000000000000000`、`-9223372036854775808`）。バイト配列 `#[ ]` の要素が 0–255 の外なら、コンパイルエラーである。
  - 小数点のある数は Float で、値は字句全体を正しく丸めたものである（`0.7 = (7 / 10.0)` は true）。
  - 指数 `e`: 仮数が整数で指数が 0 以上なら、値は整数 `仮数 × 基数^指数` である（`1e3` は Integer の 1000、`2r1e4` は 16、`0e500` は 0）。この場合、仮数が 0 でないのに指数が 65536 を超えたら、コンパイルエラー `number too large` にする（指数の分だけ桁が増えるので、`1e999999999` のような字句が巨大な整数を作らないようにする）。仮数が小数か指数が負なら、値は `仮数 × 基数^指数` を丸めた Float である（v1 には Fraction のリテラルが無い）。Float が無限大になるかどうかは、仮数や指数ではなく結果の値で決める。
- 擬変数 `self` `super` `thisContext` `nil` `true` `false`
- 代入 `:=` および `_`（入力は受け付けるが、ソース保存は `:=`）
- 捕捉解析: コードを生成する前に、変数ごとに、宣言したスコープ、展開しないブロックからの捕捉、代入の有無を調べ、temp ベクタに置く temp を決める（§3.4）。
- インライン展開（§3.5）
- 引数（メソッドとブロック）と `to:do:` のループ変数への代入は、コンパイルエラー `cannot assign to argument` にする。
- temp の上限は 255 である。引数、temp、持ち上げた temp（展開したブロックの temp、`to:do:` のループ変数と上限）、temp ベクタを入れるスロット、コピーした値を合わせて数える。
- 名前の解決順は、ローカル（メソッドとブロックの引数と temp）→ インスタンス変数 → 擬変数 → クラス変数（§3.6）→ グローバル（`Smalltalk` の辞書。§3.6）である。前のものが、後ろの同じ名前を隠す。ワークスペース（§3.10）の解決順は §3.10 に書く。ワークスペースのレシーバは nil なので、クラス変数は無い。
  - クラス変数は、メソッドのクラスから見えるもの（§3.6）である。読みは `PushLitVar`、代入は `StoreLitVar` / `PopStoreLitVar` で、リテラルは classPool の束縛（Association）である。束縛は、メソッドを作るとき（リテラルを箱に入れるとき）に、メソッドのクラス（クラス側ならその thisClass）からスーパークラス鎖をたどって引く。
  - どれにも当たらない名前はグローバルである。読みは `PushGlobal`（実行時に引く。§3.5）で、代入はコンパイルエラー `cannot assign` である。
  - インスタンス変数のうち、Kernel クラスが足したスロット（§3.6）は読み取り専用である。読みは `PushInstVar` で、代入はコンパイルエラー `cannot assign to Kernel instance variable <名前>` である（区間は代入の式）。
  - クラス側のメソッドでは、Behavior の 8 つの枠の名前（`superclass methodDict format name thisClass category classPool instVarNames`。§3.6）がインスタンス変数なので、同じ名前のクラス変数を隠す。この 8 つは読み取り専用のインスタンス変数である。

エラーはソース区間付き。Browser の accept は失敗時にテキストを壊さずエラーを表示する。

#### チャンク形式

- チャンクは、文字列とコメントの外にある単独の `!` で終わる。`!!` は `!` 1 文字に戻す。文字列とコメントの中でも `!!` は `!` に戻す（`^'Hello!!'` のメソッドは `'Hello!'` を返す）。
- 文字リテラル `$'` と `$"` は、文字列やコメントを開かない。
- `! !`（`!` と空白と `!`）と、空白だけのチャンクは、`methodsFor:` のセクションを閉じる。前に `!` が付くチャンクも、その手前でセクションを閉じる（メソッド `foo ^1!` のすぐあとの `!Object subclass: #Y …!` はクラス定義である）。
- `methodsFor:` のヘッダ（`!Foo methodsFor: 'x'!` のように、前に `!` が付くチャンク）は、どこにあってもヘッダである。
- セクションの中（ヘッダのあと、閉じる前）のチャンクは、ヘッダでなければメソッドである。パターンが `subclass: x` や `methodsFor: x` のメソッドもメソッドである。
- セクションの外のチャンクは、クラス定義メッセージ（`X subclass: #Y …`）ならクラス定義、`X methodsFor: 'y'` なら前に `!` が無くてもヘッダ（Browser の accept に渡すソースの先頭など）、それ以外は式である。セクションを閉じたあとの式を、直前のクラスのメソッドにしない。

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
- クラス定義テキストの accept でクラス作成 / 再定義（下の「クラス定義の再 Accept」）
- 階層表示（hierarchy）をクラスリストの代替または別コマンドで提供

外観は macOS 標準（NSSplitView, NSTableView / NSOutlineView, NSTextView）。Smalltalk-80 の白黒ビットマップ見た目を再現しない。キーバインドは macOS 標準 + Smalltalk 慣習（Do it は `⌘D`、Print it は `⌘P` が衝突するため `⌘P` は Print it にせず、メニューで明示。Print it は `⌘I` を既定候補とし、設定可能にする）。

メニューバー:

- Ao / File / Edit / Smalltalk / Tools / Window / Help
- Tools: Browser, Transcript, Workspace
- Smalltalk: Do it, Print it, Inspect it, Accept

アクセシビリティ: VoiceOver ラベルを主要コントロールに付ける。動的な過度なアニメーションを使わない。

#### クラス定義の再 Accept

既にあるクラスの定義を `ao_accept_class` で受け付けたときの規則である。メソッドを黙って捨てない。Kernel クラスの再定義は今までどおり拒む（§3.12）。file-in（§3.12）のクラス定義の扱いは変えない。

この節でサブクラス（子孫すべて）と言うときは、生きているクラスのうち、スーパークラスの連鎖にそのクラスを含むもの（そのクラス自身は除く）すべてを指す。Smalltalk に名前で束縛されているかどうかは問わない。名前を外したクラスでも、インスタンスや変数から届くものは数え、形の変更で残った旧クラスも数える。生きているとは、GC のルート（§3.2）から、各オブジェクトのクラスとポインタのスロットをたどって届くことである（弱い参照はたどらない）。ただし、メソッドのキャッシュ（§3.3）とソース表（§3.10）はルートからたどらない。どちらも Smalltalk のオブジェクトからは届かないセッションの表で、そこからしか届かないクラスのメソッドは、もう動かないからである。届かないクラスは、GC がまだ回収していなくても数えない。

- superclass が、定義するクラスの名前が今指しているクラスそのものか、そのクラスを上位に持つクラスなら（`Foo subclass: #Foo …`）、何も変えずに `AO_ERR_COMPILE` を返す。メッセージは `superclass refused: <Super> is <Name> or its subclass` である。受け付けると、新しいクラスが旧クラスのサブクラスになり、Browser が表示した定義を Accept し直すたびに継承が 1 段深くなるからである。
- 形が同じとき（superclass が、名前で引いた同じクラスで、instVarNames が同じ名前の同じ順のとき）は、既存のクラスオブジェクトを保つ。メソッド辞書（インスタンス側とクラス側）、メタクラス、既存インスタンスはそのままである。更新するのは category と classVariableNames（classPool。§3.6）だけである。classPool は次のように更新する。
  - 残る名前は、今の束縛（Association）と値をそのまま保つ。コンパイル済みのメソッドがその束縛を共有しているからである。
  - 新しい名前は、値 nil の新しい束縛で足す。コンパイル済みのメソッドはコンパイルし直さない。新しい名前をグローバルとして読んでいたメソッドは、Accept し直すまでグローバルを読む。
  - 消える名前の束縛を、生きているクラス（この節の冒頭の意味）のどれかのメソッドが持っていれば、何も変えずに `AO_ERR_COMPILE` を返す（category も変えない）。束縛を持ちうるのは、そのクラスとそのサブクラス（子孫すべて）、形の変更で残ったそれらの旧クラス（束縛を今のクラスと共有する。下）である。インスタンス側とクラス側のメソッド、ソースの無いメソッド、ブロックの中（入れ子の深さによらない）を数え、読みも代入も数える。束縛を持つメソッドは、classPool から外れた束縛を黙って読み書きし続けるからである。メッセージは `class variable change refused: <Class>>><selector> refers to removed class variable <var>` で、`<Class>` はメソッドのあるクラス（クラス側なら `<Class> class`）である。どのメソッドも持たなければ、消える名前を classPool から除く。
- 形が変わるとき（superclass か instVarNames が違うとき）は、新しいクラスを作り、名前をそれに付け替える。旧クラスのメソッド（インスタンス側とクラス側のすべて）を、ソース表（§3.10「ソースはイメージに書かない」）のソースで新しいクラスに対してコンパイルし直して移す。移したメソッドのソースはソース表に入れる。
- 形が変わるとき、新しいクラスの classPool にある名前のうち、旧クラスの classPool にもある名前は、旧クラスの束縛（Association）をそのまま使う。値も、旧クラスのメソッド（既存インスタンスが使う）との共有も保つ。移したメソッドは、この束縛を指す。
- 形が変わるとき、次のどれかに当たれば、何も変えずに `AO_ERR_COMPILE` を返す。名前は旧クラスを指したまま、旧クラスとそのメソッドもそのままである。`AoSpan.message` は空にせず、理由（当たったセレクタ、サブクラスがあること、または消えるインスタンス変数かクラス変数）を入れる。
  - ソース表にソースの無いメソッド（NativeMethod を含む）が 1 つでもある。
  - コンパイルし直しが 1 つでも失敗する。
  - インスタンス側のメソッドが、旧クラスにあって新しい形に無いインスタンス変数（継承したものを含む）を読むか代入する。数えるのは、旧クラスのメソッドが変更前にコンパイルされたとおりにインスタンス変数として読み書きするものである。新しい形ではその名前が大域変数か、同じ名前のクラス変数に解決され、黙って意味が変わるからである。ブロックの中（入れ子の深さによらない）の読み書きも数える。送信（`self y`）、シンボル（`#y`）、同じ名前の引数と temp は数えない。メッセージは `shape change refused: <Name>>><selector> refers to removed instance variable <var>` である。コンパイルし直しの失敗（新しい形で代入できない名前への代入など）があれば、そちらを先に報告する。
  - インスタンス側かクラス側のメソッドが、旧クラスから見えて新しい定義から見えないクラス変数（§3.6。スーパークラスのものを含む）を読むか代入する。数えるのは、旧クラスのメソッドが変更前にコンパイルされたとおりにクラス変数として（束縛を通して）読み書きするものである。インスタンス変数と同じ理由で、数え方も同じである（新しい形では同じ名前のインスタンス変数に解決されることもある）。メッセージは `shape change refused: <Name>>><selector> refers to removed class variable <var>`（クラス側は `<Name> class>><selector>`）である。消えるインスタンス変数の読み書きがあれば、そちらを先に報告する。
  - そのクラスにサブクラス（この節の冒頭の意味で、名前の無いものを含む）がある。サブクラスの付け替えは v1 ではしない。
- 形が変わるとき、新しいクラスを作る `subclass:…` の送信が失敗すれば（評価を中断した、またはクラスを答えなかった。superclass のクラス側でこのメッセージを上書きすると起こりうる）、`AO_ERR_COMPILE` を返し、名前を旧クラスに戻す。送信の途中で名前が新しいクラスに付け替わっていても戻す。旧クラスとそのメソッドはそのままである。
- メソッドは、送信が答えたクラスのインスタンス変数の並び（インスタンス側とクラス側）とクラス変数で移す。superclass のクラス側の上書きで、並びやクラス変数が定義テキストと違うことがあるからである。それでコンパイルし直しか消える変数の検査が失敗すれば、上の拒否と同じメッセージで `AO_ERR_COMPILE` を返し、名前を旧クラスに戻す。旧クラスとそのメソッドはそのままである。
- 送信が答えたクラス（上書きが答えた既存の別のクラスでもよい）の classPool とメソッド辞書は、上の検査がすべて通るまで変えない。束縛の引き継ぎ（上）とメソッドの移し替えはそのあとにする。拒否したときは、どのクラスの classPool も束縛もメソッド辞書も変わらない。移す途中で割り当てに失敗すれば、送信が答えたクラスの classPool とメソッド辞書（インスタンス側とクラス側）を元のオブジェクトに戻し、名前を旧クラスに戻して `AO_ERR_COMPILE` を返す。
- 既存インスタンスは移行しない。形が変わったあとも旧クラスのインスタンスのまま残り、旧クラスのメソッドで動く。
- 1 回の `ao_accept_class` に複数のクラス定義があれば、先頭から順に適用し、拒否された定義で止まって `AO_ERR_COMPILE` を返す。それより前のチャンク（クラス定義と `methodsFor:`）は適用済みのまま残り、それより後のチャンクは適用しない。

### 3.10 ブリッジ

C ABI（`bridge/ao_abi.h`）のみが runtime と app の境界。

必須エントリ:

- 起動 / 終了 / イメージ load-save
- 文字列ソースの評価（Workspace / Do it）
- クラス一覧、セレクタ一覧、ソース取得、accept
- Transcript コールバック（ランタイム → アプリ）
- 評価エラーの理由の文字列化（`AoSpan.message`。§3.3 の失敗の規則）

AppKit オブジェクトを OOP としてヒープに直接置かない。ホストハンドル表で結ぶ。

#### セッション

OS のプロセスにセッションは 1 つ。`ao::boot()` はそれを 1 つ作る。既にあるときに再度呼ぶと 0 以外を返す。`ao::shutdown()` はセッションを捨て、セッションが無くても 0 を返す。

セッションは協調スケジューラ（§3.4）を 1 つ持つ。boot とロードのあと、ベースプロセスを決める（§3.4、§3.11）。セッションを捨てるとき（`ao::shutdown()`、`ao_image_load` が新しいセッションと差し替えるとき、セッションの破棄）は、ヒープを捨てる前に、ベース以外のプロセスを abandon する（§3.4。後始末を走らせない）。`ao_image_load` が失敗したときは、古いセッションのプロセスはそのまま残る。

中身はテストの `Boot` と同じである。`Heap`、`Roots`、`WellKnown`、`Bootstrap::run`、`ClassMethodCache`、そのキャッシュを指す `CallContext`。既定の初期容量（nursery 1 MiB×2、old 4 MiB）は変えない。old は上限まで伸びる。`ao_image_load` はヘッダの heapBytes に合わせてコミットする。

`ao_image_load` はヒープと well-known とキャッシュを載せ替える。transcript フック（`ao_set_transcript_hook`）は ABI 側が保持し、`ao::boot()` と `ao_image_load` は、作った新しいセッションにそれを配線する。boot の前に設定したフックも、shutdown→boot のあとも、ロードのあとも届く。NULL を設定すると外れる。ロードのあと `ensureKernelNatives` を呼ぶ。これは、Kernel のネイティブ（Transcript のクラス側の転送を含む）のうち、ロードしたイメージのメソッド辞書に無いセレクタだけを `putNative` する。既にあるセレクタは上書きしない。後から足したネイティブが、古いイメージにも入る。`ao_image_save` の呼び出し規約は `Image::save` と同じ（§3.11。原子的に書き、失敗しても旧イメージは残る）。`ao_image_load` は `Image::load` が成功したあと、`1 + 2` が SmallInteger の 3 で、`nil isNil` が true でなければ `AO_ERR`。ロードと探針は新しいセッションに対して行い、どちらも成功したときだけ現在のセッションと差し替える。どちらかに失敗したら `AO_ERR` を返し、ロード前のセッションをそのまま使い続ける（差し替えも、シャットダウンもしない）。`AO_ERR` のとき、`err` が NULL でなければ `AoSpan.message` に理由を入れる（空にしない。`start` と `end` は 0）。理由は、`Image::load` が拒否したときはその理由（§3.11。`unsupported image version 1` など）、探針が失敗したときは `image probes failed`、それ以外（セッションが無い、`path` が NULL、ワークスペースを作れない）は `image load failed` である。`AO_OK` のときは空文字にする。CLI の `ao image load <path>` は、失敗したとき標準エラーに `ao: image load failed: <理由>` を 1 行出す。`ao_filein_load_order` は `fileInLoadOrder` をセッションに対して呼ぶ。パスが読めないとき、または file-in のエラー（§3.12。`DEFERRED.md` で除外したものを除く）が 1 件でもあるときは `AO_ERR`。

#### 再入と例外

ランタイムが動いている間、つまり次の ABI の入口のどれかが実行中か、インタプリタが実行中の間は、ランタイムは busy である。busy のときに、フック（transcript、inspect）やネイティブから次の関数を呼ぶと、何もせずに `AO_ERR` を返す。

- `ao_runtime_boot`、`ao_runtime_shutdown`
- `ao_image_save`、`ao_image_load`、`ao_filein_load_order`
- `ao_workspace_reset`、`ao_eval`、`ao_accept_method`、`ao_accept_class`

拒んだ呼び出しはセッションに触れない。呼び出し元の評価はそのまま続き、その結果を返す。`ao_image_load` の理由は `runtime is busy`、`ao_eval` の `out` は空文字（`out` が NULL でなく `out_len` が 1 以上のとき）である。フックの設定（`ao_set_transcript_hook`、`ao_set_inspect_hook`）とブラウザの読み取り（`ao_browser_*`、`ao_version`）は busy でも呼べる。busy の判定は 1 か所にまとめる。インタプリタが実行中とは、ベースプロセスで評価が走っているか、ベース以外のプロセス（§3.4）が走っていることである。ベース以外のプロセスから呼ばれたフックの中の呼び出しも、drain（§3.4）の間の呼び出しも拒む。

ABI の関数は C++ の例外を境界の外へ出さない。関数の中で捕捉し、int を返す関数は `AO_ERR`（件数を返す関数は -1）を返す。`ao_image_load` の理由は `image load failed` である。

#### C ABI

既存の `AO_OK = 0`、`AO_ERR = 1` は維持する。追加する。

```
AO_ERR_COMPILE = 2
AO_ERR_EVAL = 3
AO_ERR_RANGE = 4
AO_ERR_NOSOURCE = 5
```

文字列バッファは、`buf_len > 0` なら必ず NUL で終わる。入り切らないときは `AO_ERR_RANGE`（`ao_browser_source` の `AO_ERR_NOSOURCE` だけは例外で、下に書く）。`ao_version` も同じで、切り詰めたら（`buf` は NUL で終わる）`AO_ERR_RANGE`、`buf` が NULL か `buf_len` が 1 未満なら `AO_ERR` を返す。

件数を返す関数（`ao_browser_class_count`、`ao_browser_protocol_count`、`ao_browser_selector_count`、`ao_browser_subclass_count`）は、成功なら 0 以上の件数を返し、失敗なら -1 を返す。失敗は、セッションが無い、名前がクラスに当たらない、`meta` が 0 でも 1 でない、引数が NULL、のどれかである。`AO_ERR`（1）を返さない。1 件と区別できないからである。

```c
typedef struct AoSpan {
  unsigned int start;
  unsigned int end;
  char message[256];
} AoSpan;

typedef void (*AoTranscriptFn)(const char* utf8, int len, int is_clear, void* user);
typedef void (*AoInspectFn)(const char* class_name, const char* print_utf8, void* user);

int ao_image_save(const char* path);
int ao_image_load(const char* path, AoSpan* err);
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

`ao_browser_source` は、ソース表（下の「ソースはイメージに書かない」）にソースがあるメソッドなら、そのソースを `buf` に書いて `AO_OK` を返す（入り切らなければ `AO_ERR_RANGE`）。ソース表にソースが無いメソッドなら `AO_ERR_NOSOURCE` を返し、`buf` にプレースホルダを書く。ソースが無いのは、NativeMethod、イメージを読み込んだあとのメソッド、vendor と file-in（`ao_accept_class` の `methodsFor:` のチャンクを含む）で入れたメソッドである。プレースホルダは、コメント 1 つだけの 1 行である（改行を含まない）。

```
"<クラス>>><セレクタ> source not available"      CompiledMethod
"<クラス>>><セレクタ> native <シンボル名>"        NativeMethod
```

`<クラス>` はクラス名で、クラス側（`meta` が 1）なら `<クラス名> class` である。`<シンボル名>` は NativeMethod のシンボル名である（§3.11。例: `"Object>>printString native ao_Object_printString"`）。プレースホルダはセレクタのパターンを含まないので、そのまま `ao_accept_method` に渡してもコンパイルが失敗し（`AO_ERR_COMPILE`）、メソッドは変わらない。プレースホルダが `buf` に入り切らないときも `AO_ERR_NOSOURCE` を返し（`AO_ERR_RANGE` より優先する）、`buf` は切り詰めて NUL で終える。`buf` が NULL か `len` が 1 未満なら、今までどおり `AO_ERR` である。クラスやセレクタが見つからないときの `AO_ERR` も変えない。

`ao_accept_method` は `NativeMethod` を CompiledMethod で置き換えない。対象の側のメソッド辞書にネイティブがあるセレクタに加えて、Kernel クラス（§3.6）では、そのクラスから引くとネイティブに当たるセレクタ（上位クラスから継承したネイティブ）も拒む。例えば `SmallInteger>><=` は `Magnitude>><=` のネイティブを隠すので拒む。どちらも `AO_ERR_COMPILE` で、メッセージは `native selector overwrite refused: <selector>` である。Kernel でないクラスは、継承したネイティブを上書きできる。Kernel クラスかどうかは、名前で引いた先のクラスそのもので決める（クラス側でも、名前で引いたクラスで決める。引いた先が Kernel クラスのメタクラスなら、Kernel クラスとみなす）。`Smalltalk at: #IntegerAlias put: SmallInteger` のような別名で指しても、Kernel クラスの名前で指したときと同じに拒む。`ao_accept_method` は、名前で引いた先がクラス（Behavior）でなければ（`Processor`、`Smalltalk`、未定義の名前など）、何もせずに `AO_ERR` を返す。

`ao_accept_class` が受け付けるのは、クラス定義メッセージ（`Super subclass: #Name instanceVariableNames: … category: …`）と、チャンク形式のクラス定義・`methodsFor:` のチャンクだけである。それ以外のチャンク（式、メソッドの本体だけのテキストなど）が 1 つでもあれば、何も適用せずに `AO_ERR_COMPILE` を返す。メッセージは `not a class definition` である。

- `methodsFor:` のグループは `! !` で終わる。そのあとのヘッダでないチャンクは、グループのメソッドではなく式として拒む。例えば、ヘッダ `!Foo methodsFor: 'x'!`、メソッド `foo ^1! !`、式 `3 + 4!` の 3 チャンクは、`Foo>>foo` も入れずに拒む。
- クラス定義メッセージのチャンクは、そのメッセージ 1 つだけからなる。メッセージは、クラス名に送るキーワードメッセージである。キーワードは `subclass:` で始まり、`instanceVariableNames:`、`classVariableNames:`、`poolDictionaries:`、`category:` をこの順に続ける（途中を省いてよい）。引数はどれも 1 つの字句（Symbol、String、名前）である。メッセージのあとに置いてよいのは `.` だけである。文が続けば（`… category: 'X'. Smalltalk at: #Y put: 1`）、それも拒む。

#### ソースはイメージに書かない

メソッドソースはセッションのルート表（`(Oop method, Oop string)` を `Roots` に登録したベクタ）だけが持つ。`.aoimage` には書かない。上書きした古い対はルートから外す。`ao_runtime_boot` と `ao_image_load` は表を空にする。

#### プロトコルとカテゴリ

プロトコルはメソッド辞書の各値を見て、クラスが `NativeMethod` なら `native`、それ以外なら `user`。空の側は返さない。順序は `native` の次に `user`。セレクタはプロトコルで絞り、UTF-8 でソートする。継承したメソッドは含めない。カテゴリ（`kClassSlotCategory`）が nil または空なら、一覧上の見出しは `Kernel`。定義テキストの category は、nil なら空文字 `''`、それ以外はそのバイト列を文字列リテラルにしたもの。定義テキストはチャンクとして Accept し直すので、`'` を `''` に、`!` を `!!` に二重にする（§3.8 チャンク形式）。表示した定義を Accept し直しても、カテゴリは変わらない。定義テキストの classVariableNames は、そのクラスの classPool の名前（§3.6。スーパークラスのものは含めない）を、名前のバイト列の昇順に空白 1 つで区切ったもの。poolDictionaries は常に空文字 `''`。表示した定義を Accept し直しても、クラス変数は変わらない（§3.9）。

#### Transcript のクラス側転送

クラス側の `show:` / `nextPutAll:` / `nextPut:` / `cr` / `clear` は、`WellKnown::transcript` に同じメッセージを送り、戻り値はクラス（レシーバ）である。フックへ渡す UTF-8 は、文字ならそのスカラー 1 個、バイト列の文字列ならそのバイト、nil なら `is_clear = 1`、`len = 0`。インスタンス側の既存ネイティブは変えない。

#### ワークスペース変数

ワークスペースはセッションに 1 つ。`IdentityDictionary` ではなく、名前文字列をキーにした `Dictionary` をルートする。値は束縛（`Association`。キーは名前の文字列、値は変数の値）である。`ao_workspace_reset` は空の辞書に戻す。

- 名前の解決順は、ローカル（引数と temp）→ インスタンス変数 → 擬変数 → `knownGlobals` → 束縛。宣言した temp（`| q |`）は同じ名前の束縛と関係しない。
- `knownGlobals` は `Smalltalk` の辞書（§3.6）のキー全部である。固定のグローバル（Kernel クラス名、`Processor`、`Smalltalk`）と、`subclass:` と `Smalltalk at:put:` で足した名前を含む。セッションはこれをキャッシュし、クラスの定義と `Smalltalk at:put:`（グローバルの登録）のあとで作り直す。既知のグローバル名の読みは `PushGlobal`、その名前への代入はコンパイルエラー `cannot assign`。後から同じ名前のクラスを定義すると、束縛よりクラスが勝つ。
- どれにも当たらない名前は束縛である。読みは `PushLitVar`、代入は `StoreLitVar` / `PopStoreLitVar`。束縛が辞書に無ければ、メソッドを作るとき（リテラルを箱に入れるとき）に値 nil で作って辞書に入れる。同じ名前の束縛は評価をまたいで同じ Association なので、ブロックに捕捉した束縛への代入も辞書に残る。束縛の数に上限は無い（temp の 255 に数えない）。

`ao_eval` は、`out` が NULL か `out_len` が 1 未満なら、何も評価せずに `AO_ERR` を返す（副作用を起こさない。呼び出し側が再試行しても二重にならない）。Do it は結果を捨て `out` は空文字。Print it は `printString` の UTF-8 を `out` に書く。Inspect it は `inspect` のあと Print it と同じ文字列を `out` に書く。評価の失敗（§3.3 の失敗の規則。どれも abort）は `AO_ERR_EVAL` で、理由を `AoSpan.message` に入れる。理由が 255 バイトを超えれば切る。`AO_ERR_EVAL` のときの `AoSpan.message` は空にしない。abort 以外で値が得られなかったとき（理由が無いとき）は `evaluation failed` を入れる。コンパイル失敗は `AO_ERR_COMPILE` と `AoSpan`。評価したあと、返す前に drain する（§3.4。返り値は drain の前に確定し、drain で変わらない）。

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

`Object>>printOn: aStream` は、レシーバに `printString` を送り、その答えを `aStream` に `nextPutAll:` で書き、レシーバを答える（`nextPutAll:` の答えではない）。上の表のネイティブも、ユーザーが上書きした `printString` も、そのまま出る。`storeOn:` も同じである。`printString` が評価を中断したら、`nextPutAll:` を送らずにその理由のまま中断する（§3.3）。

### 3.11 イメージ形式 `.aoimage`

- マジック `AOIM`、バージョン（3）、ポインタサイズ、エンディアン
- well-known 表
- ヒープダンプ（直接ポインタはファイル内オフセットに再配置）
- グローバル辞書（`Smalltalk` の中身としてヒープダンプに入る）
- クラス変数（classPool の辞書と束縛。§3.6）もヒープダンプに入る。メソッドのリテラルと classPool は、ロードのあとも同じ束縛を指す
- 起動時に再配置し、NativeMethod の関数ポインタは **ロード時にシンボル名で結び直す**（ポインタをファイルに書かない）

`NativeMethod` は安定したシンボル名（例: `ao_Object_identityEquals`）を持つ。オペコードやネイティブを追記しても版は変えない。ロードのあと `ensureKernelNatives`（§3.10）で足りないネイティブを補う。

ネイティブのブロック（`makeNativeBlock` が作る thunk。`nextPutAll:` や `collect:` などのネイティブが、内部で `do:` に渡す BlockContext）も、NativeMethod を 1 つ持つ。その名前は、thunk の関数を Kernel のインストールで登録した名前（例: `ao_Stream_nextPutAll_each`、`ao_Collection_collect_fill`）である。thunk が捕捉する状態（ストリーム、数え上げ、結果の配列など）はすべてブロックのスロットにあり、関数はランタイムの静的な関数なので、ヒープに逃げた thunk（利用者の `do:` がブロックを保持した場合）は、ロードで名前から結び直せば保存したときと同じに動く。名前を登録していない関数の thunk（テストの関数など）の名前は `ao_NativeBlock_thunk` で、これは結び直せない。

形式の版は 3 である。版 1 は、クラスの名前の Symbol、Kernel クラスの instVarNames、グローバル辞書、classPool（§3.6）より前の形式である。版 2 は、Dictionary と Set がハッシュ表（§3.6 Collections）になる前の形式で、`array` にキーと値（Set では要素）を先頭から詰めていた。版 3 の Dictionary と Set の `array` は、末尾に世代番号のスロットを持つ（§3.6。版 3 は世代番号を足す前に公開していないので、版は上げない）。新しいネイティブはその配置を引けない。ロードは版 1 と版 2 のイメージをヘッダを読んだ段階で拒否し、修復しない。理由は `unsupported image version 1`、`unsupported image version 2` である。ほかの版も同じく `unsupported image version <版>` で拒否する。ロードは、クラスの名前とインスタンス変数名を直さない。版 3 のイメージの Kernel クラスは、保存したときの名前と instVarNames を持つ。名前の無いスロットがあるのは、ユーザーがクラスの instVarNames を変えたとき（`OrderedCollection instVarAt: 8 put: nil` など。§3.6）だけである。

ロードが拒否するときの理由は次のとおりである。`ao_image_load`（§3.10）と CLI の `ao image load` はこれを出す。

| 場合 | 理由 |
|---|---|
| ファイルが読めない | `cannot read image file` |
| ヘッダより短い、マジックが `AOIM` でない | `not an Ao image` |
| 版が 3 でない | `unsupported image version <版>` |
| ポインタサイズ、エンディアン、ヘッダ長が違う | `unsupported image format` |
| heapBytes が old の上限を超える | `image heap exceeds the old space limit` |
| それ以外（予約欄、レコード、ヒープ、グローバルの照合などが合わない） | `damaged image` |

グローバル辞書（§3.6）は `Smalltalk` の中身で、ヒープダンプに入る。well-known 表の `Smalltalk` がそれを指す。ファイル末尾のグローバルのレコードには、照合のために 57 の名前（Kernel クラス名と `Processor`）の値を書く。ロードは、`Smalltalk` がグローバル辞書であり、57 の名前の値がレコードと一致し、`Smalltalk` の値が `Smalltalk` 自身であることを確かめる。`subclass:` と `Smalltalk at:put:` で足したグローバルは辞書にだけあり、レコードに書かない（extra のレコードは 0 件）。辞書より前に保存した旧イメージ（`Smalltalk` が 57 要素の表で、足したグローバルを extra のレコードに持つもの）は版 1 なので、ヘッダの段階で拒否する。版 3 で `Smalltalk` がグローバル辞書でないイメージと、extra のレコードを持つイメージは、壊れたイメージとして拒否する。

ヘッダの `heapBytes` は old の上限以下とする。上限を超えるヒープは保存せず、そのようなイメージのロードは拒否する。

#### プロセス

- 保存は評価の合間にだけ行う（評価の最中の `ao_image_save` は busy で `AO_ERR`。§3.10）。プロセスの C スタックと切り替えの状態はイメージに書かない。`Processor`、Process、Semaphore、実行可能キューは、ヒープのオブジェクトとしてそのまま書く。待っているプロセスがあっても保存できる。
- ロードのあと（boot のあとも同じ）、`Processor activeProcess` をベースプロセスにし、実行可能キューを空にする（§3.4）。イメージにあったほかの Process は走れないプロセス（§3.4。死んだものとして扱う）で、保存の前に始まっていたかどうかによらない。`resume` は `process cannot run` で失敗し、セマフォの linkedList に残っていれば `signal` が捨てる。

#### 保存

保存（`Image::save`、`ao_image_save`、`ao image save`）が成功したイメージは、同じ版のランタイムの `Image::load` で必ず読める。保存は、書く前に、書こうとするファイルのバイト列に下の「ロードの検査」をそのまま当てる（ロードと同じ関数を使う）。どれかに反すれば、何も書かずに失敗する。失敗の理由（`Image::save` の reason。`ao image save` は標準エラーに `ao: image save failed: <理由>` を 1 行出す）は、どのオブジェクト（クラスならその名前、インスタンスならそのクラスの名前）がどの規則に反したかを言う。`ao_image_save` は理由を返さず `AO_ERR` である。この約束は `Image::load` までで、`ao_image_load` の探針（§3.10。`1 + 2` と `nil isNil`）は含まない。例えば `SmallInteger>>+` を壊したイメージは保存できるが、`ao_image_load` は探針で拒む。

- ロードの検査の old の上限には、保存するヒープの old の上限を使う（セッションでは 4 GiB − 1 MiB。§3.2）。生存データは nursery と old の両方にあり、ロードはそれを 1 つの old に並べるので、old の上限より小さい old に収まっていたセッションでも超えることがある。
- 名前の無い thunk（上）がヒープに逃げていれば、NativeMethod の名前の規則に反する。
- `Smalltalk` がグローバル辞書でないときも失敗する（今までどおり）。

書き方は次のとおりである。

- 保存先がシンボリックリンクなら、リンクをたどった先（32 段まで）を置き換え、リンクは残す。リンク先がまだ無くてもよい。保存先がまだ無ければ、そのパスに作る。
- 保存先が既にあって書き込めない（`access(W_OK)` が失敗する）なら、何も書かずに失敗する。読み取り専用のイメージは置き換えない（保存先を開いて書いていた以前の実装と同じ）。
- 保存先と同じディレクトリに一時ファイル `.aoimage-XXXXXX`（X は英数字 6 文字。`O_EXCL` で作る）を作る。名前の長さは保存先の名前によらないので、長い名前（255 バイトまで）の保存先にも保存できる。パーミッションは、保存先が既にあればそれを写し、無ければ 0666 から umask を引いたもの（作るときにカーネルが引く）とする。
- 一時ファイルに書く。書いている間だけ SIGXFSZ を無視する。ファイルサイズの上限を超える write は、プロセスを終わらせずに EFBIG で失敗する。書き終えたら SIGXFSZ の扱いを元に戻す。
- 一時ファイルを fsync（macOS では `F_FULLFSYNC`。効かないファイルシステムでは `fsync`）し、`rename(2)` で保存先に置き換える。rename より前のどこかで失敗したら、一時ファイルを消して失敗する。保存先の旧イメージには触れないので、失敗した保存のあとも旧イメージはそのままロードできる。
- rename が成功した時点で保存は成功である。そのあとディレクトリを fsync するが、ベストエフォートであり、失敗しても保存を失敗にしない（置き換えは済んでいて、失敗を返すと実態と食い違う）。rename のあとは、メモリの割り当てを含めて、失敗しうる処理をしない（ディレクトリのパスは rename の前に用意する）。

#### ロードの検査

ロードは、まずヘッダの 48 バイトだけを読んで確かめる。heapBytes が old の上限を超えるなら、本体を読まずに `image heap exceeds the old space limit` で拒否する。続いてファイルの大きさを確かめる。ファイルは、ヘッダ、heapBytes のヒープ、レコード 184 件（well-known 127、グローバル 57。1 件は 16〜268 バイト）からなるので、その範囲に無ければ本体を読まずに `damaged image` で拒否する。

本体を読んだら、ヒープを old に載せる前に次を確かめる。保存も、書く前に同じ関数で確かめる。どれかが成り立たなければ、ロードは `damaged image` で拒否する。

以下で「Behavior のオブジェクト」とは、klass から親（superclass）をたどると Behavior に着くオブジェクトをいう。クラス、メタクラス、`Behavior new` で作った無名のクラスがこれに当たる。親は lookup（§3.3）と同じく 1024 段までたどる。循環もそこで止まるので、循環そのものは拒否しない。

- オブジェクトヘッダの flags は、`Bytes` と `Weak` のほかを立てない（保存はこの 2 つだけを書く。`Marked` などが残ると GC が壊れる）。先頭の詰め物は、klass が nil、大きさが 0、flags が `Bytes` だけである。
- klass は、nil か Behavior のオブジェクトである。klass が nil なのは、メソッド辞書の中の配列や NativeMethod の名前などの内部のオブジェクトである。
- klass が nil でないオブジェクトは、klass の format（§3.6。instSize、可変長か、バイト列か）に合う形をしている。format はランタイムと同じに読む。SmallInteger でない format は 0（名前付きのスロットが 0 個、固定長、ポインタ）とみなす。
  - `Bytes` は、format がバイト列のクラスのインスタンスにだけ立つ。`Weak` は klass が nil のオブジェクトにだけ立つ（弱参照を持つクラスは無い）。
  - ポインタのオブジェクトのスロット数は instSize 以上で、可変長でないクラスなら instSize と等しい。
- Behavior のオブジェクトは、インスタンスの有無によらず、クラスの形（ポインタ形で `kClassSlotCount` 以上のスロット）をしている。methodDict は nil か MethodDictionary のインスタンス、superclass は nil か Behavior のオブジェクトである。
- well-known 表のクラスとメタクラスは Behavior のオブジェクトである。well-known 表のセレクタ（`value` など）は Symbol である。
- MethodDictionary のインスタンスは 2 つ以上のスロットを持つポインタ形で、配列のスロットは nil かポインタ形のオブジェクトである。
- CompiledMethod のインスタンスは `kCmSlotCount`（6）以上のスロットを持つポインタ形である。NativeMethod のインスタンスは `kNativeSlotCount`（6）以上のスロットを持つポインタ形で、名前がこのランタイムの登録名で解決できる。Symbol は同じバイト列のものが 2 つ無い。

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
2. Kernel と衝突するメソッド（`Object>>#==` などネイティブ必須）は file-in しない。すでに `NativeMethod` があるセレクタは上書き禁止。Kernel クラス（§3.6）への `methodsFor:` チャンクは丸ごと拒む。Kernel クラスかどうかは accept（§3.10）と同じく、名前で引いた先のクラスそのもので決める。別名で指しても拒む。
3. ホストに移した機能（描画、ファイルダイアログ、Browser ビュー）を参照するメソッドは `image/patches/` でスタブか削除する。
4. P5 以降、`ao filein image/vendor/...` で `CompiledMethod` として載せる。
5. ロード順は `image/vendor/LOAD_ORDER` に固定する。

file-in のエラー:

- file-in のエラーは、チャンク単位のエラーとメソッド単位のエラーである。エラーが 1 件でもあれば file-in は失敗である。成功扱いにしない。
- チャンク単位のエラーは、クラス定義の失敗（Kernel クラスの再定義の拒否、スーパークラスが無い、`subclass:` の失敗など）、存在しないクラスへの `methodsFor:`、Kernel クラスへの `methodsFor:` の拒否である。チャンク単位のエラーは、そのファイルの残りのチャンクを止める。`fileInLoadOrder` では、LOAD_ORDER の残りのファイルも読まない。それまでに適用したチャンクは戻さない。チャンク単位のエラーは `DEFERRED.md` で除外できない。位置は、そのチャンク（クラス定義のチャンク、`methodsFor:` のヘッダのチャンク）のバイト範囲である（区切りの `!` を含まない）。
- メソッド単位のエラーは、メソッドのコンパイルエラーとネイティブ上書きの拒否（とメソッドを登録できないこと）である。残りのチャンクの file-in を止めない。位置は、コンパイルエラーならファイル本文での箇所、それ以外はそのメソッドのチャンクのバイト範囲である。
- 意図して載せないメソッドは、LOAD_ORDER と同じディレクトリの `DEFERRED.md` に、1 行に 1 つ `Class>>selector: 理由` の形式で、行頭から列挙する（クラス側は `Class class>>selector: 理由`）。セレクタは、`>>` のあと最初の `: `（コロンと空白）の手前までである。キーワードセレクタは、末尾の `:` のあとに `: 理由` を続ける（`Bag>>sum:ifEmpty:: 理由`）。理由の無い行（`: ` とそのあとの文字が無い行）と、この形式でない行は、注記として読み飛ばす。LOAD_ORDER による file-in（`fileInLoadOrder`）では、列挙したメソッドのエラーを失敗に数えず、報告もしない（そのメソッドは入らない）。単一ファイルの file-in には除外が無い。
- `fileInLoadOrder` は、LOAD_ORDER かそこに書いたファイルが読めないとき、または数えるエラーが 1 件でもあるときに失敗を返す。エラーには、それが起きたファイルの名前を付ける。
- `ao filein <file.st>` と `ao filein --load-order <LOAD_ORDER>` は、数えるエラーを 1 行に 1 件、`<ファイル名>:<start>-<end>: <メッセージ>` の形式で stderr に出す。位置の無いエラー（ファイルが読めない）は `0-0` である。失敗なら exit 1、成功なら exit 0。
- `ao image save --load-order <LOAD_ORDER> <path>` は、file-in が失敗したら、同じ形式でエラーを stderr に出し、イメージを書かずに exit 1 で終わる。
- `ao_filein_load_order` は、file-in が失敗したら `AO_ERR` を返す（§3.10）。
- `vendor_filein_test` は、`image/vendor/LOAD_ORDER` の file-in で起きるメソッド単位のエラーの集合（`Class>>selector`）が、`DEFERRED.md` に列挙した集合と一致することを確かめる。列挙したのに成功するメソッドも、列挙していないのに失敗するメソッドも、テストの失敗である。

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
- `kernel_numeric_test`: 数の混合演算と厳密な比較（NaN、±inf、LargeInteger）、Float への丸め、`bitShift:` の境界、Boolean の演算、Point と Rectangle のサブクラス、`asCharacter` の範囲、`to:do:` の終端、`=` と `hash` の契約
- `collection_do_test`: Array/String/Dictionary の中核プロトコル。`select:` と `reject:` の述語は要素ごとに 1 回（Boolean でない答えは `mustBeBoolean`、作業領域の検査は述語の前）、`includes:` は `anObject = 要素` で `hash` を送らない、String の `do:` の 1 パスと書き換え（渡す文字は文字列にある文字）
- `stream_test`: ストリームの `contents` の種類と要素の数（ByteArray、OrderedCollection、Array のサブクラス、Symbol、ReadWriteStream。割り当て前の範囲の検査）、String への書き込み（多バイト文字、予備、上書き、差し替え、利用者の書き換え、`size` を上書きしたサブクラス、クラスを保つ追記）、`position:` の上限、スロットを極端な値にした反射、ReadStream の `nextPut:`、GC 圧下、性能
- `hashed_collection_test`: Dictionary と Set のハッシュ表（`=` と `hash` の送り方と失敗、nil、削除と拡張、再入、壊れた表、GC 圧下、性能、ホームの混ぜ方、満杯の表からの削除、`copy`、世代番号による再入の検出）、classPool の配置と名前の並び（Symbol のキーだけ）と写し、Interval の刻みと終端の比較（NaN）と Integer の `size`、OrderedCollection の組の検査と `at:` の範囲
- `compiler_roundtrip_test`: ソース → バイトコード → 評価
- `block_test`: 引数、返り値、外側 temps の共有、非局所リターン、`ensure:`
- `image_save_load_test`: save 後に同一評価結果。保存の失敗（書き込み、容量、ロードの検査に反するヒープ）で旧イメージが残る。壊れたイメージ（flags、klass、クラスの形、format、巨大な heapBytes）を拒否する。保存先がリンク、読み取り専用、長い名前のとき
- `session_abi_test`: 評価中のフックからの再入が `AO_ERR` になる（ベース以外のプロセスから呼ばれたフックでも。`ReentrantEvalFromHookRejected`）。transcript フックが boot の前後とロードをまたいで届く。評価の終わりの drain（`DoItDrainsTranscriptFork`、`PrintItBeforeDrain`、`[n := n + 1] fork. Processor yield. n` が `1`）、評価をまたいで残る待つプロセス（`WaiterSurvivesAcrossEvals`）、待つプロセスのある save と load でベースが `activeProcess` のまま（`SaveLoadWithWaitersKeepsBaseActive`）、shutdown でプロセスを回収し、ルートの数が元に戻る（`ShutdownReclaimsFibers`）
- `fiber_test`: 1 万回の往復の切り替えで整数と浮動小数点のローカルが保たれる、スタックの下端のガードページが読み書きできない、返したスタックを再利用する
- `process_test`: 協調スケジューラ（§3.4）。fork は切り替えるまで走らない、fork の中の `activeProcess`、FIFO の順、空のキューの `yield`（`ForkRunsOnlyAfterYield`、`ActiveProcessInsideForkIsForked`、`ForkFifoOrder`、`YieldEmptyReturns`）。resume・suspend・wait・signal の状態遷移と myList。ブロックする `wait` と SharedQueue、ベースのデッドロック（`WaitBlocksUntilSignal`、`BaseDeadlockIsFailureActiveStaysBase`、`SharedQueueProducerConsumer`、`SharedQueueEmptyNextDeadlock`）。プロセスの失敗と `terminate`（`ForkDnuTerminatesOnlyFork`、`ForkNlrToBaseHomeTerminates`、`TerminateWaiterRunsEnsure`、`RecursionInForkFailsNoCrash`）。signal を受けてまだ `wait` から戻っていないプロセスを `terminate` すると signal を返す（`TerminateSignaledWaiterGivesSignalBack`）。50 本のプロセスを待たせたままの GC ストレスと old の GC。プロセスごとのルートとスタックの範囲、FIFO に使う OrderedCollection の `array` が伸び続けないこと。同じ意味論の Smalltalk 側のゴールデンは `image/tests/process.st`（§4.4。fork の順序、セマフォのピンポン、SharedQueue、`activeProcess` の同一性）
- `transcript_model_test`: コールバックが呼ばれる

GC ストレス実行: 環境変数 `AO_GC_STRESS=n` を付けると、`allocateRetry` と safepoint で n 回に 1 回 nursery GC を走らせ、そのうち 4 回に 1 回は old の GC も走らせる。GC で解放した領域は `0xA5` で埋め、古い番地を読んだら落ちるようにする。ctest の `gcstress` 項目は、runtime のスイート全体（時間計測の `KernelBench.*` を除く）を 1 プロセスでこのモードで回す。`gcstress_vendor` 項目は、vendor の file-in（`ao filein --load-order image/vendor/LOAD_ORDER`）と `ao --test image/tests` をこのモードで回す。`ao filein` の stderr と終了コードは §3.12 の「file-in のエラー」に従う。`gcstress_vendor` はストレスなしの 1 回の出力を基準にし、ストレス下の出力がそれと一致すること、どちらも exit 0 であることを確かめる。

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

`ao --test <dir>` の契約:

- `<dir>` の `*.st` を名前順に、1 ファイルずつ実行する。1 ファイルは最外の評価 1 回である（§3.4）。失敗したファイルがあっても、残りのファイルを実行する。
- ファイルの失敗は、読めない、コンパイルエラー、実行中の abort（`assert:equals:` の不一致、`doesNotUnderstand:`、`error:`、ネイティブの失敗など §3.3 の失敗すべて）である。`assert:equals:` の不一致は、理由 `<実際の printString> ~= <期待の printString>` で abort する。
- 1 ファイルの評価のあと、drain（§3.4）し、残ったベース以外のプロセスを `terminate` で終わらせる（後始末を走らせる）。`terminate` のあとにもう一度 drain して、後始末が切り替えて実行可能のまま残ったプロセスに、後始末を続けさせる。後始末が fork したプロセスなど、まだ `terminate` を送っていないプロセスが残れば、`terminate` と drain を繰り返す。新しく `terminate` したプロセスが無く、生きているプロセスも減らなかった回か、1000 回目で繰り返しをやめる。そのとき残ったプロセス（後始末がブロックしたまま終わらなかったものなど）は abandon する（§3.4。残りの後始末は走らせない）。プロセスを次のファイルに持ち越さない。
- そのファイルの評価、drain、`terminate` の間にプロセスの失敗（§3.4）が 1 件でもあれば、そのファイルの失敗である。`terminate` で終わったプロセスは数えない。
- 失敗ごとに 1 行を stderr に出す。形式は `ao --test: <ファイル名>: <理由>`。コンパイルエラーは `ao --test: <ファイル名>:<start>-<end>: <メッセージ>`（位置はファイル本文のバイト位置）。プロセスの失敗は、件数によらずファイルごとに 1 行で、`ao --test: <ファイル名>: process failed: <最後の失敗の理由>` である。
- 終了コードは、1 つでも失敗があれば 1、`.st` が 0 件か `<dir>` が読めなければ 1、それ以外は 0。

---

## 5. 制約

1. **VM は新規実装。** 他処理系の C/C++/Swift VM をコピーしない。クラスライブラリの `.st` は §3.12 に従い取り込む。
2. **Kernel はネイティブ。** Kernel メソッドを `.st` の実行定義にしない。
3. **チャットは正本ではない。** 仕様変更は `SPEC.md` を先に直す。
4. **余計なものを作らない。** 依頼されていないデバッガ、パッケージマネージャ、シンタックステーマ、ウェブサイトを追加しない。
5. **依存は最小。** runtime は C++20 標準ライブラリ + 必要なら mimalloc 程度。OS の API として mmap / mprotect / madvise を使ってよい（old の予約とコミット、プロセスのスタックとガードページ）。プロセスの切り替え（§3.4）は arm64 のアセンブリで自前で書く。ucontext、Boost.Context、OS のスレッドを使わない。GUI は AppKit のみ。Boost、Qt、SDL、SwiftUI 主系統は使わない。
6. **Apple Silicon を第一対象。** Intel Mac は考慮しない。
7. **C ABI 以外で Swift が C++ テンプレートに依存しない。**
8. **例外方針:** C++ は例外を境界で使わない。エラーは評価の中断（§3.3。理由の文字列）か ABI のエラーコード。Smalltalk の例外オブジェクトによる捕捉は v1 では扱わない。
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
- [x] 未定義セレクタは `doesNotUnderstand:` に入り、デフォルトは評価を中断して `doesNotUnderstand: #sel` を返す（`nil foo` の Print it は `AO_ERR_EVAL` と `doesNotUnderstand: #foo`）
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

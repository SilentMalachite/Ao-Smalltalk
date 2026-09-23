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

### 3.3 メッセージ送信

送信 `receiver selector: args` の意味は Blue Book に従う。

1. `receiver` のクラス `C` を取る。
2. `C` からスーパークラス鎖を `Object` まで辿り、メソッド辞書でセレクタを探す。
3. 見つかればそのメソッドを適用する。
4. 見つからなければ `receiver doesNotUnderstand: aMessage` を送る。
5. `super` 送信は、メソッドが定義されたクラスのスーパークラスから探索を始める。

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
- ブロックは `BlockContext` 相当。外側コンテキストとコピーされた値、および後続フェーズではフルクロージャ（コピー + 共有テンプ）へ拡張できる構造にする。
- プロセスは協調的。`ProcessorScheduler` / `Process` / `Semaphore` を Kernel に含める。
- ホストのプリエンプションは使わない。長時間ネイティブメソッドは safepoint を自ら発行する。

### 3.5 バイトコード（ユーザーメソッド）

Blue Book 第 28 章の集合を現代化した **Ao バイトコード** を定義する。意味は対応づけ、エンコーディングは 8-bit opcode + 必要なら 1–2 オペランド。

必須グループ:

- stack: push receiver / temp / lit / special const、pop、dup
- send: 普通送信、super 送信、算術特殊送信
- jump: 無条件、true/false
- return: メソッド、ブロック
- block: ブロック作成（クロージャ作成）
- primitive: 先頭プリミティブ番号（失敗時はバイトコード本文へ）

コンパイラが生成し、インタプリタが実行する。Kernel はこれを使わない。

JIT 差し込み口: `CompiledMethod` に `nativeCode` スロットを予約し、v1 では常に `nil`。

### 3.6 Kernel クラス（ネイティブ実装必須）

次のクラスの **インスタンスメソッドとクラスメソッドはすべて NativeMethod** とする。Smalltalk ソースで書いてインタプリタ実行してはならない。対応する `.st` ファイルは **コメントとプロトコル分類のためだけ** に置いてよい。実行定義は C++。

#### Kernel-Objects

- `Object`
- `UndefinedObject`（`nil`）
- `Boolean`, `True`, `False`

必須セレクタ（最小。実装時に Blue Book プロトコルを充足して増やす）:

`Object`: `class`, `==`, `~~`, `=`, `hash`, `identityHash`, `yourself`, `isNil`, `notNil`, `ifNil:`, `ifNotNil:`, `perform:`, `perform:with:`, `perform:withArguments:`, `doesNotUnderstand:`, `error:`, `subclassResponsibility`, `shouldNotImplement`, `isKindOf:`, `isMemberOf:`, `respondsTo:`, `copy`, `shallowCopy`, `instVarAt:`, `instVarAt:put:`, `instVarNamed:`, `basicSize`, `basicAt:`, `basicAt:put:`, `printString`, `printOn:`, `storeOn:`, `inspect`（ホスト Inspector を開くブリッジ）

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

`SmallInteger` の `+ - * // \\ quo: rem: bitAnd: bitOr: bitXor: bitShift: = < > <= >=` はネイティブ。オーバーフローは `LargeInteger` へ透過。

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
- 評価コンテキストの `self` は `nil`。ワークスペース変数（未定義識別子を temps にする）は v1 で実装する。

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

中身はテストの `Boot` と同じである。`Heap`、`Roots`、`WellKnown`、`Bootstrap::run`、`ClassMethodCache`、そのキャッシュを指す `CallContext`。ヒープの既定容量は変えない。

`ao_image_load` はヒープと well-known とキャッシュを載せ替える。transcript フック関数ポインタはセッション側に残し、ロードで消さない。ロードのあと `ensureTranscriptClassMethods` を呼び、メタクラスに `show:` が無ければクラス側ネイティブを `putNative` する。`ao_image_save` は実行中のインタプリタの外からだけ呼び、呼び出し規約は `Image::save` と同じ。`ao_image_load` は `Image::load` が成功したあと、`1 + 2` が SmallInteger の 3 で、`nil isNil` が true でなければ `AO_ERR`。探針に失敗したセッションはシャットダウンしない。`ao_filein_load_order` は `fileInLoadOrder` をセッションに対して呼ぶ。パスが読めなければ `AO_ERR`。

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

ワークスペースはセッションに 1 つ。`IdentityDictionary` ではなく、名前文字列をキーにした `Dictionary` をルートする。`ao_workspace_reset` は空の辞書に戻す。`knownGlobals` は `Globals::nameAt` の 57 名、`eachExtra` の名、`eachClass` のクラス名バイト。`workspaceTemps` は辞書のキーを UTF-8 でソートしたもの。未定義名はテンプ、既知のグローバル名の読みは `PushGlobal`、その名前への代入はコンパイルエラー `cannot assign`。テンプが 255 を超えたら `too many temporaries`。Do it は結果を捨て `out` は空文字。Print it は `printString` の UTF-8 を `out` に書く。Inspect it は `inspect` のあと Print it と同じ文字列を `out` に書く。空 OOP は `AO_ERR_EVAL`。コンパイル失敗は `AO_ERR_COMPILE` と `AoSpan`。

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

`NativeMethod` は安定したシンボル名（例: `ao_Object_identityEquals`）を持つ。版番号は 1 のままとする。

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
- `block_test`: 引数、返り値、外側 temps
- `image_save_load_test`: save 後に同一評価結果
- `transcript_model_test`: コールバックが呼ばれる

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
5. **依存は最小。** runtime は C++20 標準ライブラリ + 必要なら mimalloc 程度。GUI は AppKit のみ。Boost、Qt、SDL、SwiftUI 主系統は使わない。
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

# P8–P9 Remaining Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** macOS アプリ Ao が独立した NSWindow で Transcript / Workspace / System Browser を出し、Do it / Print it / Inspect it / accept と階層表示とエラー表示まで動いて v1 の受け入れを閉じる。

**Architecture:** `ao_runtime_boot` がプロセスに 1 つだけ `Bootstrap::run` 済みのヒープを持つ。Swift は `bridge/ao_abi.h` だけを呼ぶ。AppKit オブジェクトは OOP にしない。Transcript の表示は既存の `CallContext::transcriptHook` を C 関数ポインタで Swift に渡す。グローバル `Transcript` はクラスのままとし、クラスメソッドがインスタンスへ転送してフックを呼ぶ。ワークスペースの未定義名はテンプ、既知のグローバル名は今までの `PushGlobal`。メソッドソースはセッションのルート表だけが持ち、`.aoimage` の版は 1 のまま。

**Tech Stack:** C++20、GoogleTest、Swift 6、AppKit、既存の `Heap` / `Bootstrap` / `compileMethod` / `installMethod` / `Interpreter::run`。新規ライブラリは足さない。

**Spec:** `SPEC.md` §2.3 の P8 と P9、§3.5、§3.8、§3.9、§3.10、§3.11、§4.3、§6。フェーズ正本は `docs/phases/P8.md` と `docs/phases/P9.md`。PR 骨格は `docs/prs/P8-01.md` … `P8-05.md` と `docs/prs/P9-01.md` … `P9-04.md`。

## 結論

P0 から P7 は終わっている。`PHASE` は `P8`。`main` は `34386b2`（`Mark image snapshot phase done`）。`origin/main` より 8 コミット進んでいる。AppKit は `app/Ao/Placeholder.swift`（`phase == "P0"`）と `bridge/ao_abi.h` の `ao_version` / `ao_runtime_boot` / `ao_runtime_shutdown` だけである。`ao::boot()` は `0` を返すだけでヒープを持たない。

この計画は P8 を実行し、受け入れが緑になってから P9 に進む。P9 のタスクを P8 のブランチに混ぜない。

承認後、この文書を `docs/superpowers/plans/2026-09-23-p8-p9-remaining.md` に置き、実装ブランチの最初のコミットにする。

## 前提

- グラフは `8f67ec7` から作られていて、HEAD のフェーズ印より 1 コミット古い。GUI のコミュニティは PR 文書ノードだけである。実装の入口は `bridge/ao_abi.h`、`CallContext::transcriptHook`、`Bootstrap::run`、`installMethod`、`Interpreter::run`。
- `graphify . --update` は API キーが要る。更新コマンドは `graphify update .`（AST のみ）。
- Serena の `activate_project` は grok の single_project では使えない。定義と参照は `find_symbol` / `find_referencing_symbols`、本体は `replace_symbol_body`。worktree は絶対パスで開く。
- テスト用ヒープは `Boot` フィクスチャがスタック上に作る。ABI のセッションとは別である。既存の GoogleTest をセッションに載せ替えない。
- `ao_runtime` は静的ライブラリで、`ao_compiler` は PRIVATE リンクである。Swift から使う最終リンクは `libao_runtime.a` と `libao_compiler.a` の両方を `-force_load` する。
- クラスのカテゴリスロットはブートストラップでは nil のままである。ここを埋めない（イメージの既存バイト列を変えない）。
- `CompiledMethod` は 6 スロット。`kCmSlotNativeCode` は nil のまま。スロットを足さない。
- `printString` は今、クラス名だけを返す。`1 + 2` の Print it が `3` になるには、下の仕様にあるサブクラスの上書きが要る。
- グローバル名 `Transcript` はクラスである。インスタンスは `WellKnown::transcript`。`show:` は今、クラスのインスタンス側にある。クラスへ送ってもメタクラスを探すので、クラス側の転送が無いと Workspace の `Transcript show:` はフックに届かない。
- ブロックの外側テンプは作成時にコピーされる。ワークスペース変数をブロックの中で代入しても、外側の束縛は変わらない。この計画ではクロージャを共有テンプに変えない。
- テストバイナリは `build/runtime/ao_runtime_tests`。CLI は `build/ao`。
- 未追跡の `.cache/` と `.serena/logs/` はコミットしない。

## 範囲

**P8 でやる**

- プロセスに 1 つのセッション。`ao_runtime_boot` / `ao_runtime_shutdown` / `ao_image_save` / `ao_image_load` / `ao_filein_load_order`。
- Transcript のクラス側転送、C の transcript フック、等幅切替、閉じてもテキストが残る窓。
- Workspace の窓。評価コマンドの実体は P9。
- Browser の 5 ペイン。一覧・プロトコル・ソース取得・クラス定義テキストの ABI と表示。accept の実体は P9。
- メニューバーとキー。Do it は `⌘D`、Print it は `⌘I`。`⌘P` は Print it にしない。
- `Ao.app`（バンドル ID `jp.ao.smalltalk`）。
- フェーズ完了後に `PHASE` を `P9` にする。

**P9 でやる**

- `printString` の上書き（仕様に書いた型だけ）。
- 文字列評価（Do it / Print it / Inspect it）、ワークスペース変数、Inspector 窓。
- メソッド accept とクラス定義 accept。失敗してもソースペインの文字列は残る。
- 階層表示、VoiceOver ラベル、File メニューのイメージ保存と読み込み。
- `image/tests` のゴールデン、SPEC §6 の性能の記録、v1 のチェックを閉じる。

**やらない**

- JIT、FFI、デバッガ、パッケージマネージャ、SwiftUI への主系統移行、白黒ビットマップ、シンタックステーマ。
- `.aoimage` の版を 2 にすること、well-known を 127 から増やすこと、`globalCount` の 57 を変えること。
- メソッドソースをイメージに書くこと。
- `StoreGlobal` バイトコード。グローバルへの代入。
- Kernel `.st` のプロトコル名を Browser に出すこと。プロトコルは `native` と `user` だけ。
- 継承チェーン上のセレクタ一覧。一覧は、そのクラス自身のメソッド辞書だけ。
- MappedCollection の自作。`image/vendor/DEFERRED.md` のまま。
- Bag / LinkedList の再実装。vendor のメソッドか既存ネイティブをテストするだけ。
- ブロックをフルクロージャにすること。
- README の装飾。

## 仕様

実装の前に Task 1 で、この節の決定を `SPEC.md` に書き足す。ここに書いた契約が ABI の正本である。

### セッション

`ao::boot()` はセッションを 1 つ作る。既にあるときに再度呼ぶと 0 以外を返す。`ao::shutdown()` はセッションを捨て、セッションが無くても 0 を返す。

セッションの中身は、テストの `Boot` と同じである。`Heap`、`Roots`、`WellKnown`、`Bootstrap::run`、`ClassMethodCache`、そのキャッシュを指す `CallContext`。ヒープの既定容量は変えない。

`ao_image_load` はヒープと well-known とキャッシュを載せ替える。transcript フック関数ポインタはセッション側に残し、ロードで消さない。ロードのあと `ensureTranscriptClassMethods` を呼び、メタクラスに `show:` が無ければ下のクラス側ネイティブを `putNative` する。P7 で保存したイメージにはクラス側転送が無いため。

`ao_image_save` は実行中のインタプリタの外からだけ呼ぶ。呼び出し規約は今の `Image::save` と同じ。`ao_image_load` は `Image::load` が成功したあと、CLI と同じ探針を行う。`1 + 2` が SmallInteger の 3 で、`nil isNil` が true でなければ `AO_ERR`。探針に失敗したセッションはシャットダウンしない。呼び出し側が捨てる。

`ao_filein_load_order` は既存の `fileInLoadOrder` をセッションに対して呼ぶ。パスが読めなければ `AO_ERR`。

### C ABI

既存の `AO_OK = 0`、`AO_ERR = 1` は維持する。追加する。

```c
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

`meta` は 0 がインスタンス側、1 がクラス側（そのクラスの `klass`）。クラス一覧にメタクラスは出さない。

`mode` は `AO_EVAL_DOIT = 1`、`AO_EVAL_PRINTIT = 2`、`AO_EVAL_INSPECTIT = 3`。

フックの `user` は Swift が保持するオブジェクトのポインタである。ランタイムはそれを OOP として辿らない。フックは評価を呼び直さない。

### Transcript

クラス側の `show:` / `nextPutAll:` / `nextPut:` / `cr` / `clear` は、`WellKnown::transcript` に同じメッセージを送り、戻り値はクラス（レシーバ）である。これで `Transcript show: 'hello'; cr` のカスケードがクラスに残る。

フックへ渡す UTF-8:

- 文字ならそのスカラー 1 個
- バイト列の文字列ならそのバイト
- nil なら `is_clear = 1`、`len = 0`

インスタンス側の既存ネイティブは変えない。`transcript_model_test` は緑のまま。

### Browser の読み取り

クラス一覧は `WellKnown::eachClass` のクラス名を UTF-8 でソートしたもの。カテゴリは `kClassSlotCategory` のバイト列。nil または空なら一覧上の見出しは `Kernel`。定義テキストの category は、nil なら空文字 `''`、それ以外はそのバイト列。

プロトコルはメソッド辞書の各値を見て、クラスが `NativeMethod` なら `native`、それ以外なら `user`。空の側は返さない。順序は `native` の次に `user`。セレクタはプロトコルで絞り、UTF-8 でソートする。継承したメソッドは含めない。

ソース:

- NativeMethod: `"<selector>\n  \"NativeMethod <symbolName>\"\n"`。`symbolName` は `NativeMethod::nameBytes`。
- CompiledMethod でセッション表にソースがある: その UTF-8 をそのまま。
- CompiledMethod で表に無い: `"<selector>\n  \"CompiledMethod\"\n"`。

クラス定義テキスト（セレクタ未選択のペイン）:

```
<superclass> subclass: #<name>
  instanceVariableNames: '<names separated by spaces>'
  classVariableNames: ''
  poolDictionaries: ''
  category: '<category>'
```

スーパークラスが nil なら `<superclass>` は `nil`。インスタンス変数名スロットがポインタ配列でなければ空。クラス変数とプールは常に空（スロットを再構文解析しない）。

スーパークラス名は `ao_browser_superclass`。Object のインスタンス側は空文字で `AO_OK`。サブクラスは、一覧のうちスーパークラス名が一致するものをソートして返す。

### 評価とワークスペース変数

`ao_eval` はソースの前に `doIt\n` を付け、`compileMethod` し、`installMethod` せず `Interpreter::run` する。レシーバは nil。

`CompileEnv` に次を足す。既定は今の挙動のまま（スナップショットテストは変えない）。

```cpp
std::vector<std::string> knownGlobals;
std::vector<std::string> workspaceTemps;
bool undeclaredAreTemps = false;
```

`MethodImage` に次を足す。

```cpp
struct TempBinding {
  std::string name;
  bool workspace = false;
};
std::vector<TempBinding> tempBindings;
```

`undeclaredAreTemps` のとき:

- 既にテンプ、インスタンス変数、擬変数なら今まで通り。
- `knownGlobals` にある名前の読みは `PushGlobal`。
- その名前への代入はコンパイルエラー `cannot assign`。
- それ以外の読みと代入はワークスペーステンプ。既存の `workspaceTemps` をインデックス 0 から順に置き、新しく捕捉した名前は末尾に足し、`workspace = true`。
- ソースの `| t |` で宣言され、ワークスペースに無い名前は `workspace = false`。
- メソッド本体のコンパイルが終わってから `numTemps` を最終のテンプ数に合わせる。255 を超えたら `too many temporaries`。

ランタイムが渡す `knownGlobals` は、`Globals::nameAt` の 57 名、`eachExtra` の名、`eachClass` のクラス名バイト。`workspaceTemps` はワークスペース辞書のキーを UTF-8 でソートしたもの。

`CallContext` に次を足す。null のときは今のインタプリタのまま。

```cpp
Oop* hostTemps = nullptr;
std::uint32_t hostTempCount = 0;
```

`hostTempCount == numTemps` のときだけ、`Temps` を作った直後（引数を書き込む前）に `hostTemps` をコピーし、`Temps` の生存期間が終わるときに同じ配列へ書き戻す。呼び出し側がその配列をルートする。

ワークスペースはセッションに 1 つ。`IdentityDictionary` ではなく、名前文字列をキーにした既存の `Dictionary` をルートする。`ao_workspace_reset` は空の辞書に戻す。

Do it は結果を捨て、`out` は空文字。Print it は結果へ `printString` を送り、その UTF-8 を `out` に書く。Inspect it は結果へ `inspect` を送り、フックへクラス名と `printString` を渡したあと、Print it と同じ文字列を `out` に書く。評価が空 OOP を返したら `AO_ERR_EVAL`。コンパイル失敗は `AO_ERR_COMPILE` と `AoSpan`。

### accept

`ao_accept_method` は `compileMethod` と `installMethod`。インスタンス変数名は既存の `fillInstVars`。既存の値が `NativeMethod` ならインストールせず `AO_ERR_COMPILE`、メッセージは `native selector overwrite refused`。成功したらメソッドキャッシュのそのクラスとセレクタを `forget` し、セッションのソース表にソース文字列を置く。失敗したら辞書もソース表も変えない。

`ao_accept_class` はペインの文字列を `fileInString` に渡す。カーネル再定義の拒否は今の `refusesKernelRedefinition` のまま。失敗したらイメージは変えない。

ソース表は `(Oop method, Oop string)` を `Roots` に登録したベクタ。上書きした古い対はルートから外す。`ao_image_load` と `ao_runtime_boot` は表を空にする。

### printString

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

### アプリ

- バンドル ID `jp.ao.smalltalk`。表示名 `Ao`。macOS 14。`NSWindow` は独立。`styleMask` は `.titled, .closable, .miniaturizable, .resizable`。
- 起動時に Transcript と Workspace を 1 枚ずつ出す。Tools メニューが Browser / Transcript / Workspace を開くか前面にする。閉じても Swift 側のテキストと、Transcript ならモデルを保持する。
- メニューバーは Ao / File / Edit / Smalltalk / Tools / Window / Help。
- Edit は cut / copy / paste / select all。File の P8 は Close（`⌘W`）だけ。P9 で Save Image… と Open Image… を足す。Help はバージョン文字列を NSAlert で出す。
- Transcript のフォントは `NSFont.userFont`。メニュー項目 Use Fixed Pitch が `NSFont.userFixedPitchFont` に切り替え、`UserDefaults` の `AoTranscriptFixedPitch` に残す。
- Browser は `NSSplitView` と `NSTableView` と `NSTextView`。上段がカテゴリとクラス、下段がプロトコルとセレクタ、一番下がソース。クラス側切替は segmented control。
- Smalltalk メニューの Do it / Print it / Inspect it / Accept は、P8 では項目とキーだけを作る。P9 でキーウィンドウが Workspace なら評価、Browser なら Accept だけがソースを受け付ける。
- 主要コントロールの `accessibilityLabel` は P9 で付ける。文字列は `Class categories`、`Classes`、`Protocols`、`Selectors`、`Source`、`Transcript`、`Workspace`、`Error`。
- 階層は Browser のコマンド Show Hierarchy。クラス一覧の代わりに、選択クラスのスーパークラス鎖と直下のサブクラスを出す。もう一度で通常の一覧に戻る。
- accept 失敗時はソースの文字列をそのままにし、ペインの下のラベルに `AoSpan.message` を出す。成功したらラベルを空にする。
- Inspector はクラス名と printString の 2 行を出す窓。インスタンス変数は出さない。
- 起動時、カレントディレクトリに `image/vendor/LOAD_ORDER` があれば `ao_filein_load_order` する。失敗しても窓は開く。最初のエラー文字列を Transcript に足す。
- Swift の GUI 型は `@MainActor`。新規の強制アンラップは足さない。

### 性能

インタプリタの 10M ループは再計測しない。`docs/bench.md` の比 5.76 を維持する。P9 の最後で SPEC §6 のその項目を、計測値を記録し v1 は 10 倍をリリース条件にしない、と書き換えてからチェックする。起動時間は `ao_runtime_boot` と、リポジトリの `LOAD_ORDER` を続けて呼んだ時間を `docs/bench.md` に書く。2 秒を超えたら SPEC §6 にそのミリ秒と理由を書き、目標の文は残す。

## Global Constraints

- 製品は `SPEC.md` の v1 だけ。フェーズを飛ばさない。P8 のブランチで P9 の評価と accept を実装しない。
- Kernel メソッドは C++ の NativeMethod。`.st` を実行定義にしない。
- 新規 C++ / Swift は Apache License 2.0。既存ファイルにライセンスヘッダは足さない。
- C++20。API 境界で例外を投げない。公開ポインタは `ao::Oop`。ABI のエラーは `int`。
- Swift は C ABI だけを呼ぶ。C++ テンプレートに依存しない。
- 依存を足さない。GUI は AppKit。Apple Silicon / macOS。
- 識別子とコミットメッセージは英語。コミットは短い命令形。トレーラに Graphify と Serena を 1 行ずつ付ける。
- 実装は `GRAPH_REPORT.md` と `graphify query`、Serena のシンボル編集を通す。新規ファイルは作成後に overview。
- テストが赤のまま次タスクへ進まない。全テストは `./scripts/test.sh`。単体は下の各タスクのコマンド。
- `@MainActor` を GUI に付ける。強制アンラップを新規に増やさない。
- バンドル ID は `jp.ao.smalltalk`。Do it は `⌘D`。Print it は `⌘I`。

---

### Task 1: セッションと SPEC の ABI 契約

**Files:**

- Modify: `SPEC.md`（§3.9 の末尾、§3.10、§3.11 の直後）
- Create: `runtime/src/Session.hpp`
- Create: `runtime/src/Session.cpp`
- Modify: `runtime/src/Runtime.cpp`
- Modify: `runtime/src/abi.cpp`
- Modify: `runtime/include/ao/Runtime.hpp`（宣言は増やさない。セッションは `Session.hpp`）
- Modify: `bridge/ao_abi.h`
- Modify: `runtime/CMakeLists.txt`（`src/Session.cpp` を `ao_runtime` に足す）
- Test: `runtime/tests/session_abi_test.cpp`

**Interfaces:**

- Consumes: `Bootstrap::run`、`Image::save`、`Image::load`、`fileInLoadOrder`、`Globals::nameAt`
- Produces: 仕様節の `ao_runtime_boot` / `shutdown` / `ao_image_save` / `ao_image_load` / `ao_filein_load_order`。プロセスにセッションは 1 つ。

- [ ] **Step 1: SPEC に契約を書く**

§3.10 の箇条書きの直後に、この計画の「仕様」のうちセッション、C ABI の関数一覧、ソースはイメージに書かない、プロトコルは `native` / `user`、カテゴリ nil は一覧では `Kernel`、Transcript のクラス側転送、ワークスペース変数、printString の表、を要約して足す。数値と関数名は上の節からそのまま写す。版番号は 1 のまま、と §3.11 に 1 文足す。

- [ ] **Step 2: 失敗するテストを書く**

`runtime/tests/session_abi_test.cpp`:

```cpp
#include "ao_abi.h"

#include <cstdio>
#include <gtest/gtest.h>

TEST(SessionAbi, BootThenImageRoundTripKeepsOnePlusTwo) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const char* path = "session-abi-roundtrip.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  ao_runtime_shutdown();
  std::remove(path);
}

TEST(SessionAbi, SecondBootFailsUntilShutdown) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_runtime_boot());
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
}

TEST(SessionAbi, MissingLoadOrderIsError) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_filein_load_order("/no/such/LOAD_ORDER"));
  ao_runtime_shutdown();
}
```

`std::remove` と `<cstdio>` を include する。

- [ ] **Step 3: 赤を確認する**

Run: `cmake --build build --target ao_runtime_tests && ./build/runtime/ao_runtime_tests --gtest_filter='SessionAbi.*'`

Expected: コンパイル失敗（`ao_image_save` が無い）。

- [ ] **Step 4: セッションを実装する**

`Session.hpp` は runtime の外に公開しない。`sessionBoot()` / `sessionShutdown()` / `session()` を置き、`session()` は無ければ null。

`ao::boot()` はセッションが無ければ `new` して `Bootstrap::run` し 0 を返す。あれば 1。`ao::shutdown()` は delete して 0。

`ao_image_save` / `ao_image_load` はセッションの `Image::save` / `Image::load`。ロードが成功したあとに CLI と同じ探針を行い、`1 + 2` が 3 で `nil isNil` が true でなければ `AO_ERR`。ロード後にキャッシュを空の `ClassMethodCache` で作り直し、フックポインタは残す。このタスクの `ensureTranscriptClassMethods` は空の関数でよい。Task 2 で中身を入れる。

`ao_filein_load_order` はセッションの `CallContext` に `fileInLoadOrder`。

`ao_eval` はこのタスクでは作らない。

- [ ] **Step 5: 緑を確認する**

同じフィルタ。Expected: PASS。続けて `./build/runtime/ao_runtime_tests --gtest_filter='Smoke.*:AbiSmoke.*'` も PASS（既存の boot / shutdown が 0 のまま）。

- [ ] **Step 6: コミット**

```bash
git add SPEC.md bridge/ao_abi.h runtime/src/Session.hpp runtime/src/Session.cpp \
  runtime/src/Runtime.cpp runtime/src/abi.cpp runtime/CMakeLists.txt \
  runtime/tests/session_abi_test.cpp \
  docs/superpowers/plans/2026-09-23-p8-p9-remaining.md
git commit -m "Add process session behind the C ABI

Graphify: query ao_abi Bootstrap Image::load
Serena: replace_symbol_body ao::boot"
```

計画文書はこのコミットに含める。

---

### Task 2: Transcript のクラス側転送とフック

**Files:**

- Modify: `runtime/src/kernel/Stream.cpp`（`installStream` とクラス側の 5 関数）
- Modify: `runtime/src/Session.cpp`
- Modify: `runtime/src/abi.cpp`
- Modify: `bridge/ao_abi.h`（`AoTranscriptFn` と `ao_set_transcript_hook`。Task 1 で宣言済みなら触らない）
- Test: `runtime/tests/session_abi_test.cpp`

**Interfaces:**

- Consumes: `callTranscriptHook`、`WellKnown::transcript`、`WellKnown::transcriptMetaclass`
- Produces: クラス `Transcript` への `show:` / `cr` がフックを呼ぶ。`ao_set_transcript_hook`。

- [ ] **Step 1: 失敗するテスト**

`ao_eval` は Task 9 まで無い。転送は `transcript_model_test.cpp` に足す。レシーバはクラスオブジェクト `transcriptClass` なので、探索はメタクラスから始まる。フックの文字列が `hello`、`cr` のあと `hello\n`、戻り値は `transcriptClass`。

```cpp
TEST(TranscriptModel, ClassSideShowForwardsToInstanceHook) {
  Boot b;
  static std::string seen;
  seen.clear();
  b.ctx.transcriptHook = [](ao::CallContext& ctx, ao::Oop v) {
    if (v.isCharacter()) seen.push_back(static_cast<char>(v.characterValue()));
    else if (!v.isNil()) seen += ao::Str::toUtf8(ctx.heap, v);
  };
  auto text = ao::Str::fromUtf8(b.heap, b.wk, "hello");
  auto ret = send1(b, b.wk.transcriptClass, "show:", text);
  EXPECT_EQ(b.wk.transcriptClass, ret);
  send0(b, b.wk.transcriptClass, "cr");
  EXPECT_EQ("hello\n", seen);
}
```

- [ ] **Step 2: 赤を確認する**

Run: `./build/runtime/ao_runtime_tests --gtest_filter='TranscriptModel.ClassSideShowForwardsToInstanceHook'`

Expected: FAIL（メタクラスに `show:` が無い、または戻りがクラスでない）。

- [ ] **Step 3: 実装する**

`Stream.cpp` に `ao_Transcript_class_show_`、`ao_Transcript_class_nextPutAll_`、`ao_Transcript_class_nextPut_`、`ao_Transcript_class_cr`、`ao_Transcript_class_clear`。それぞれインスタンス `ctx.wk.transcript` へ同じセレクタを `send` し、レシーバ（クラス）を返す。`installStream` で `wk.transcriptMetaclass` に `putNative`。シンボル名は `ao_Transcript_class_show_` のように `class` を挟む。

`ao_set_transcript_hook` はセッションの `CallContext::transcriptHook` を、UTF-8 変換してから C 関数を呼ぶラムダに差し替える。変換規則は仕様のとおり。`fn == nullptr` なら `transcriptHook = nullptr`。

`ensureTranscriptClassMethods` は、メタクラスのメソッド辞書に `show:` が無いとき同じ `putNative` を呼ぶ。`ao_image_load` の成功パスで呼ぶ。

- [ ] **Step 4: 緑を確認する**

同じフィルタと `TranscriptModel.*`。Expected: PASS。C 関数ポインタの変換は、Task 9 の `Transcript show:` 評価で確認する。

- [ ] **Step 5: コミット**

```bash
git commit -m "Forward Transcript class messages to the instance hook

Graphify: query transcriptHook Transcript
Serena: replace_symbol_body ao::kernel::installStream"
```

---

### Task 3: Browser 読み取り ABI

**Files:**

- Modify: `runtime/src/Session.cpp`
- Modify: `runtime/src/abi.cpp`
- Modify: `bridge/ao_abi.h`
- Test: `runtime/tests/browser_abi_test.cpp`

**Interfaces:**

- Consumes: `eachClass`、`kClassSlotCategory`、`kClassSlotMethodDict`、`kClassSlotName`、`kClassSlotSuperclass`、`kClassSlotInstVarNames`、`NativeMethod::nameBytes`
- Produces: 仕様の `ao_browser_*` 読み取り一式。`ao_browser_subclass_*` と `ao_browser_superclass` もここに含める（P9 の階層が同じ関数を使う）。

- [ ] **Step 1: 失敗するテスト**

```cpp
TEST(BrowserAbi, ObjectIsKernelAndPrintStringIsNative) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const int n = ao_browser_class_count();
  ASSERT_GE(n, 56);
  bool found = false;
  char name[128];
  char category[128];
  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, name, 128, category, 128));
    if (std::strcmp(name, "Object") == 0) {
      found = true;
      EXPECT_STREQ("Kernel", category);
    }
  }
  EXPECT_TRUE(found);
  EXPECT_EQ(1, ao_browser_protocol_count("Object", 0));
  char protocol[32];
  ASSERT_EQ(AO_OK, ao_browser_protocol_at("Object", 0, 0, protocol, 32));
  EXPECT_STREQ("native", protocol);
  char source[256];
  ASSERT_EQ(AO_OK, ao_browser_source("Object", 0, "printString", source, 256));
  EXPECT_NE(std::string(source).find("ao_Object_printString"), std::string::npos);
  char defn[512];
  ASSERT_EQ(AO_OK, ao_browser_class_definition("Object", defn, 512));
  EXPECT_NE(std::string(defn).find("subclass: #Object"), std::string::npos);
  char superName[128];
  ASSERT_EQ(AO_OK, ao_browser_superclass("Object", 0, superName, 128));
  EXPECT_STREQ("", superName);
  ASSERT_EQ(AO_OK, ao_browser_superclass("SmallInteger", 0, superName, 128));
  EXPECT_STREQ("Integer", superName);
  ao_runtime_shutdown();
}
```

`subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:` は `Class` のインスタンス側（`meta = 0`）にある。Object のメタクラス自身の辞書には無い。同じテストで `ao_browser_selector_count("Class", 0, "native")` の一覧にそのセレクタが含まれることを見る。クラス側切替の確認は `Transcript` の `meta = 1` に `show:` があること（Task 2 でメタクラスへ入る）。

- [ ] **Step 2: 赤を確認する**

Expected: コンパイル失敗（`ao_browser_class_count` が無い）。

- [ ] **Step 3: 実装する**

仕様のソート、カテゴリ、プロトコル、ソーステンプレート、定義テキスト、スーパークラス、サブクラス。名前が見つからなければ `AO_ERR`。このタスクではソース表が常に空なので、CompiledMethod はテンプレート分岐だけ通る。

- [ ] **Step 4: 緑を確認する**

Run: `./build/runtime/ao_runtime_tests --gtest_filter='BrowserAbi.*'`

Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Add browser listing ABI

Graphify: query eachClass MethodDictionary NativeMethod
Serena: find_symbol ao::WellKnown::eachClass"
```

---

### Task 4: Swift から ABI をリンクする

**Files:**

- Create: `app/CAo/module.modulemap`
- Create: `app/CAo/include/ao_abi.h` は作らない。module map が `../../bridge/ao_abi.h` を指す
- Modify: `app/Package.swift`
- Modify: `scripts/test.sh`（`swift test` の行に両方の `-force_load` と `-lc++`）
- Create: `app/Ao/BrowserModel.swift`
- Modify: `app/Ao/Placeholder.swift`（削除）
- Modify: `app/AoTests/SmokeTests.swift`
- Create: `app/AoTests/BrowserModelTests.swift`

**Interfaces:**

- Consumes: Task 3 の `ao_browser_*` と `ao_runtime_boot`
- Produces: `BrowserModel.refresh()` がカテゴリ、クラス、プロトコル、セレクタ、ソースを Swift の値型で持つ。`import CAo`。

module map:

```
module CAo {
  header "../../bridge/ao_abi.h"
  export *
}
```

`scripts/test.sh` の swift 行:

```sh
swift test --package-path "$ROOT/app" \
  -Xlinker -force_load -Xlinker "$ROOT/build/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$ROOT/build/compiler/libao_compiler.a" \
  -Xlinker -lc++
```

`BrowserModel` は `@MainActor`。`boot()` が `ao_runtime_boot` を 1 回だけ呼ぶ。テストの `tearDown` で `ao_runtime_shutdown`。

- [ ] **Step 1: 失敗するテスト**

```swift
import XCTest
@testable import Ao

@MainActor
final class BrowserModelTests: XCTestCase {
  func testObjectListsPrintStringAsNative() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    model.refresh()
    let cats = model.categories
    XCTAssertTrue(cats.contains("Kernel"))
    model.select(category: "Kernel", className: "Object", meta: false, protocol: "native")
    XCTAssertTrue(model.selectors.contains("printString"))
    XCTAssertTrue(model.source.contains("ao_Object_printString"))
    ao_runtime_shutdown()
  }
}
```

`SmokeTests` の `AoPlaceholder.phase` は削除し、`BrowserModel` が存在することだけを見るテストに置き換える。

- [ ] **Step 2: 赤を確認する**

Run: `./scripts/build.sh` のあと、上の `swift test --filter BrowserModelTests`

Expected: FAIL（`BrowserModel` が無い、またはリンクエラー）。リンクエラーなら `-force_load` のパスを直してからモデルを書く。

- [ ] **Step 3: 実装する**

`BrowserModel` が ABI を呼び、配列に貯める。選択状態がソース文字列を決める。セレクタ未選択なら `ao_browser_class_definition`。強制アンラップは使わない。戻り値の `AO_OK` 以外は空配列。

- [ ] **Step 4: 緑を確認する**

同じ `swift test --filter BrowserModelTests`。Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Link the Swift package to the C ABI

Graphify: query ao_abi BrowserModel
Serena: get_symbols_overview app/Ao/BrowserModel.swift"
```

---

### Task 5: Transcript と Workspace の窓

**Files:**

- Create: `app/Ao/TranscriptWindow.swift`
- Create: `app/Ao/WorkspaceWindow.swift`
- Create: `app/Ao/LaunchSet.swift`
- Create: `app/AoTests/ToolWindowTests.swift`

**Interfaces:**

- Consumes: `ao_set_transcript_hook`、`NSTextView`
- Produces: `LaunchSet.make()` が Transcript と Workspace のコントローラを返す。Transcript はフックで文字列を足し、`clear` で空にする。`useFixedPitch` がフォントを切り替える。窓を `close()` してもコントローラの文字列は残る。

- [ ] **Step 1: 失敗するテスト**

```swift
@MainActor
func testTranscriptAppendsAndSurvivesClose() {
  let launch = LaunchSet.make()
  XCTAssertEqual(launch.transcript.title, "Transcript")
  XCTAssertEqual(launch.workspace.title, "Workspace")
  launch.transcript.append("hello")
  launch.transcript.append("\n")
  launch.transcript.closeWindow()
  XCTAssertEqual(launch.transcript.text, "hello\n")
  launch.transcript.useFixedPitch = true
  XCTAssertEqual(launch.transcript.useFixedPitch, true)
}
```

- [ ] **Step 2: 赤を確認する**

Expected: コンパイル失敗（`LaunchSet` が無い）。

- [ ] **Step 3: 実装する**

`TranscriptWindow` は `NSWindow` と `NSTextView`。編集不可。フックの C 関数はキャプチャしないトップレベル関数にし、`user` からモデルへ戻す。`append` は表示と保持文字列の両方に足す。`is_clear != 0` で両方を空にする。固定ピッチは `UserDefaults` キー `AoTranscriptFixedPitch`。

`WorkspaceWindow` は編集可能な `NSTextView`。タイトル `Workspace`。評価はまだ繋がない。

`LaunchSet.make()` は両方を作り、Transcript のフックを登録する。`NSApplication.shared` を起こしてもよい。`run()` は呼ばない。

- [ ] **Step 4: 緑を確認する**

Run: `swift test --package-path app --filter ToolWindowTests` に Task 4 と同じリンカフラグ。

Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Add Transcript and Workspace windows

Graphify: query TranscriptWindow LaunchSet
Serena: get_symbols_overview app/Ao/TranscriptWindow.swift"
```

---

### Task 6: System Browser の 5 ペイン

**Files:**

- Create: `app/Ao/BrowserWindow.swift`
- Modify: `app/AoTests/BrowserModelTests.swift`

**Interfaces:**

- Consumes: `BrowserModel`
- Produces: `BrowserWindow`。5 つの一覧とソースペイン。インスタンス / クラスの segmented control。選択に合わせて `BrowserModel.select`。

- [ ] **Step 1: 失敗するテスト**

モデルをブートした `BrowserWindow` で、カテゴリ表に `Kernel`、クラス表に `Object`、プロトコル表に `native`、セレクタ表に `printString` があり、ソースに `ao_Object_printString` が含まれる。クラス `Class` のインスタンス側に `subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:` が見える。`Transcript` のクラス側に `show:` が見える。一覧は継承を含めない。

表の中身はウィンドウの `NSTableView` の `numberOfRows` と、モデルの配列で確かめる。ヒットテスト用に、コントローラが `model` を公開する。

- [ ] **Step 2: 赤を確認する**

Expected: コンパイル失敗。

- [ ] **Step 3: 実装する**

縦の `NSSplitView` の上に横分割（カテゴリ | クラス）、中に横分割（プロトコル | セレクタ）、下に `NSTextView`。タイトルは `System Browser`。accept ボタンは置かない。ソースは選択の表示だけ。白黒ビットマップや独自の枠は足さない。

- [ ] **Step 4: 緑を確認する**

`--filter BrowserModelTests`。Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Add System Browser panes

Graphify: query BrowserWindow BrowserModel
Serena: get_symbols_overview app/Ao/BrowserWindow.swift"
```

---

### Task 7: メニュー、アプリ束、P8 を閉じる

**Files:**

- Create: `app/Ao/MainMenu.swift`
- Create: `app/AoApp/main.swift`
- Create: `scripts/package-app.sh`
- Modify: `app/Package.swift`（executable target `AoApp`）
- Modify: `app/AoTests/ToolWindowTests.swift`
- Modify: `docs/phases/P8.md`（受け入れを `[x]`、手動確認の手順を 5 行）
- Modify: `docs/prs/P8-01.md` … `P8-05.md` の状態を `done`
- Modify: `PHASE` を `P9`
- Modify: `runtime/src/Runtime.cpp` のバージョン文字列を `0.0.0-p8`

**Interfaces:**

- Consumes: `LaunchSet`、`BrowserWindow`、`MainMenu.build()`
- Produces: メニュー項目とキー。`build/Ao.app`。`PHASE` が `P9`。

`MainMenu.build(actions:)` が `NSMenu` を返す。項目名は仕様のメニューバーと、Smalltalk の 4 項目、Tools の 3 項目、Use Fixed Pitch、Close。Do it の `keyEquivalent` は `d` で `.command`。Print it は `i` で `.command`。`p` を Print it に割り当てない。P8 のアクションは窓を開くもの以外、空のクロージャ。

`package-app.sh` は `swift build --package-path app` に同じ `-force_load` を渡し、`build/Ao.app/Contents/MacOS/Ao` と `Info.plist` を書く。`CFBundleIdentifier` は `jp.ao.smalltalk`。`CFBundleName` は `Ao`。

- [ ] **Step 1: 失敗するテスト**

`MainMenu.build` の項目タイトルに `Transcript`、`Workspace`、`Browser`、`Do it`、`Print it`、`Inspect it`、`Accept` がある。Print it の keyEquivalent は `i`。Do it は `d`。`p` の項目が Print it でない。

- [ ] **Step 2: 赤を確認する**

Expected: コンパイル失敗。

- [ ] **Step 3: 実装し、束を作る**

`AoApp` の `applicationWillFinishLaunching` でメニューをセットし、`LaunchSet` を出し、CWD に `image/vendor/LOAD_ORDER` があれば file-in する。失敗時は Transcript にメッセージを足して窓は開いたまま。

手動確認を `docs/phases/P8.md` に書く。起動すると Transcript と Workspace が出る。Tools から Browser が開く。閉じる、最小化、Spaces は標準の `NSWindow` に任せる。自動テストでは `run()` しない。

- [ ] **Step 4: 検証**

`./scripts/test.sh` が緑。`./scripts/package-app.sh` のあと `plutil -extract CFBundleIdentifier raw build/Ao.app/Contents/Info.plist` が `jp.ao.smalltalk`。

`graphify update .`。失敗したらコミットせず、失敗事実だけを残して止まる。

- [ ] **Step 5: コミット**

```bash
git commit -m "Add Ao menus and mark AppKit tools phase done

Graphify: update .
Serena: get_symbols_overview app/Ao/MainMenu.swift"
```

P8 はここで止める。マージの判断はユーザーに返す。P9 は `PHASE` が `P9` で、P8 が main に入ってから、別ブランチ `p9-integrate` で始める。

---

### Task 8: printString

**Files:**

- Modify: `runtime/src/kernel/Object.cpp`（nil / true / false は各クラスの install 側）
- Modify: `runtime/src/kernel/Boolean.cpp`、`SmallInteger.cpp`、`Character.cpp`、`String.cpp`、`Array.cpp`、`Float.cpp` の install 関数
- Test: `runtime/tests/print_string_test.cpp`

**Interfaces:**

- Consumes: 既存の `ao_Object_printString`（クラス名のまま残す）
- Produces: 仕様の表。シンボル名は `ao_SmallInteger_printString`、`ao_UndefinedObject_printString`、`ao_True_printString`、`ao_False_printString`、`ao_Character_printString`、`ao_String_printString`、`ao_Array_printString`、`ao_Float_printString`。

- [ ] **Step 1: 失敗するテスト**

`Boot` で `send0(..., "printString")` し、`Str::toUtf8` が `3`、`-2`、`nil`、`true`、`false`、`$A`、`'a''b'`、`#(1 2)`、Float `1` が `1` を含む、Object new が `Object` で始まる、を期待する。

- [ ] **Step 2: 赤を確認する**

Expected: FAIL（`3` ではなく `SmallInteger`）。

- [ ] **Step 3: 実装する**

各クラスのメソッド辞書に `putNative`。Array は要素へ `printString` を `send` する。深さは C++ の引数で 4 まで。Object の実装はクラス名のまま。

- [ ] **Step 4: 緑を確認する**

`PrintString.*` と `KernelScan.*`。Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Print literals from printString

Graphify: query printString SmallInteger
Serena: find_symbol ao_Object_printString"
```

---

### Task 9: 評価とワークスペース変数

**Files:**

- Modify: `compiler/include/ao/MethodImage.hpp`
- Modify: `compiler/src/Codegen.cpp`
- Modify: `runtime/include/ao/NativeMethod.hpp`（`hostTemps`）
- Modify: `runtime/src/Interpreter.cpp`
- Modify: `runtime/src/Session.cpp`
- Modify: `runtime/src/abi.cpp`
- Test: `compiler/tests/workspace_temp_test.cpp`
- Test: `runtime/tests/session_abi_test.cpp`（Task 2 で保留した eval テストをここに書く）

**Interfaces:**

- Consumes: `compileMethod`、`Interpreter::run`、`knownGlobals` の集め方（仕様）
- Produces: `ao_eval`、`ao_workspace_reset`、`AO_EVAL_*`。

- [ ] **Step 1: 失敗するコンパイラテスト**

`undeclaredAreTemps = true`、`knownGlobals = {"Object"}`、`workspaceTemps = {}` で次をコンパイルする。

```
doIt
  x := 4.
  ^x
```

`ok` であり、`tempBindings` に `x` が `workspace == true` で 1 つ。`Object` への代入ソース `Object := 1` は `ok == false` でメッセージに `cannot assign`。`^Object` の逆アセンブルは従来どおり `PushGlobal` を含む。フラグが false の既存スナップショットは変えない。別テストで `compileMethod("foo\n  ^1 + 2")` の逆アセンブルが今の期待と一致することを、既存の `codegen_snapshot_test` が既に見ているので、それを壊さない。

- [ ] **Step 2: 赤を確認する**

Expected: FAIL（`tempBindings` が空、またはフィールドが無い）。

- [ ] **Step 3: コンパイラを実装する**

仕様の解決順。`numTemps` は本体コンパイルの後に更新する。ブロックの中で捕捉した未定義名は、メソッドのテンプに足してから既存の `bindTemp` がコピーする。共有テンプにはしない。

- [ ] **Step 4: インタプリタの hostTemps と ao_eval**

仕様のとおりコピーインと書き戻し。`ao_eval` は辞書から初期値を並べ、`applyMethod` / `Interpreter::run` のレシーバを nil にする。Do it / Print it / Inspect it は仕様のとおり。Inspect のフックが null でも `out` には printString を書く。

- [ ] **Step 5: ランタイムテスト**

```cpp
TEST(SessionAbi, PrintItInsertsThree) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("1 + 2", 5, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
}

TEST(SessionAbi, WorkspaceTempSurvivesNextEval) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("x := 4", 6, AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_eval("x", 1, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("4", out);
  ao_runtime_shutdown();
}

TEST(SessionAbi, KnownGlobalStaysGlobal) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("Object new isNil", 16, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("false", out);
  EXPECT_EQ(AO_ERR_COMPILE, ao_eval("Object := 1", 10, AO_EVAL_DOIT, out, 64, &err));
  ao_runtime_shutdown();
}

TEST(SessionAbi, BlockAssignmentDoesNotUpdateWorkspaceBinding) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  const char* src = "x := 1.\n[ x := 2 ] value.\n^x";
  ASSERT_EQ(AO_OK, ao_eval(src, static_cast<int>(std::strlen(src)), AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("1", out);
  ao_runtime_shutdown();
}

TEST(SessionAbi, WorkspaceResetDropsBindings) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("x := 4", 6, AO_EVAL_DOIT, out, 64, &err));
  ASSERT_EQ(AO_OK, ao_workspace_reset());
  ASSERT_EQ(AO_OK, ao_eval("x", 1, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
  ao_runtime_shutdown();
}
```

Task 2 の `Transcript show: 'hello'` と `Transcript cr` のテストもここに置いて緑にする。

- [ ] **Step 6: 緑を確認する**

`WorkspaceTemp.*`、`SessionAbi.*`、`codegen_snapshot_test`。Expected: PASS。

- [ ] **Step 7: コミット**

```bash
git commit -m "Evaluate workspace text with undeclared temps

Graphify: query compileMethod Interpreter::run PushGlobal
Serena: replace_symbol_body ao::compiler::compileMethod"
```

`compileMethod` の本体が `Compiler.cpp` にあり、変更が `Codegen.cpp` だけなら、トレーラのシンボルは実際に `replace_symbol_body` した関数名にする。

---

### Task 10: accept

**Files:**

- Modify: `runtime/include/ao/Compile.hpp`
- Modify: `runtime/src/Compile.cpp`（`fillInstVars` はここの無名名前空間にある。accept は同じファイルに置く）
- Modify: `runtime/src/abi.cpp`
- Modify: `runtime/src/Session.cpp`（ソース表のルートだけ）
- Test: `runtime/tests/accept_abi_test.cpp`

**Interfaces:**

- Consumes: `installMethod`、`fileInString`、`fillInstVars`、`ClassMethodCache::forget`、Task 3 の `ao_browser_source`
- Produces: `Compile.hpp` の `acceptMethodSource(CallContext&, std::string_view className, bool meta, std::string_view source, compiler::CompileError*)` と `acceptClassSource(...)`。ABI はそれを呼ぶ。ソース表の出し入れは `Session.hpp` の `rememberMethodSource` / `methodSource` / `clearMethodSources`。

- [ ] **Step 1: 失敗するテスト**

```cpp
TEST(AcceptAbi, ObjectFooIsCallableAndKeepsSource) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* src = "foo\n  ^1\n";
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, src, &err));
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("Object new foo", 14, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("1", out);
  char shown[256];
  ASSERT_EQ(AO_OK, ao_browser_source("Object", 0, "foo", shown, 256));
  EXPECT_NE(std::string(shown).find("^1"), std::string::npos);
  const char* bad = "foo\n  ^\n";
  ASSERT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, bad, &err));
  ASSERT_EQ(AO_OK, ao_browser_source("Object", 0, "foo", shown, 256));
  EXPECT_NE(std::string(shown).find("^1"), std::string::npos);
  ASSERT_EQ(AO_ERR_COMPILE, ao_accept_method("Object", 0, "printString\n  ^1\n", &err));
  EXPECT_NE(std::string(err.message).find("native selector overwrite refused"), std::string::npos);
  ao_runtime_shutdown();
}

TEST(AcceptAbi, ClassDefinitionThenImageDropsSourceText) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* def =
      "Object subclass: #P9Foo\n"
      "  instanceVariableNames: 'x'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P9-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err));
  char category[64];
  char name[64];
  bool found = false;
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, name, 64, category, 64));
    if (std::strcmp(name, "P9Foo") == 0) {
      found = true;
      EXPECT_STREQ("P9-Test", category);
    }
  }
  EXPECT_TRUE(found);
  ASSERT_EQ(AO_OK, ao_accept_method("P9Foo", 0, "foo\n  ^x\n", &err));
  const char* path = "accept-abi.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  char out[64];
  ASSERT_EQ(AO_OK, ao_eval("P9Foo new foo", 13, AO_EVAL_PRINTIT, out, 64, &err));
  EXPECT_STREQ("nil", out);
  char shown[256];
  ASSERT_EQ(AO_OK, ao_browser_source("P9Foo", 0, "foo", shown, 256));
  EXPECT_NE(std::string(shown).find("CompiledMethod"), std::string::npos);
  ao_runtime_shutdown();
  std::remove(path);
}
```

カーネルの `Object subclass: #Object` は `AO_ERR_COMPILE` であることも同じファイルに書く。

- [ ] **Step 2: 赤を確認する**

Expected: コンパイル失敗（`ao_accept_method` が無い）。

- [ ] **Step 3: 実装する**

仕様の accept。キャッシュの `forget` はインストールの後。ソース表の更新は成功時だけ。ロードとブートで表を空にする。

- [ ] **Step 4: 緑を確認する**

`AcceptAbi.*` と `KernelScan.*`。Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Accept methods and class definitions through the ABI

Graphify: query installMethod fileInString
Serena: find_symbol ao::installMethod"
```

---

### Task 11: Workspace の Do it / Print it / Inspect it

**Files:**

- Modify: `app/Ao/WorkspaceWindow.swift`
- Modify: `app/Ao/MainMenu.swift`
- Create: `app/Ao/InspectorWindow.swift`
- Create: `app/AoTests/WorkspaceEvalTests.swift`

**Interfaces:**

- Consumes: `ao_eval`、`ao_set_inspect_hook`、`AO_EVAL_*`
- Produces: 選択範囲の Do it は結果を捨てる。Print it は選択の直後に printString を挿入する。Inspect it は Inspector 窓を開く。`self` はランタイム側が nil。

- [ ] **Step 1: 失敗するテスト**

テキスト `1 + 2` を全選択して Print it すると、文字列が `1 + 2` のあとに `3` を含む。`x := 5` を Do it したあと、別の評価で Print it した `x` が `5`。Inspect it の窓の文字列にクラス名と printString がある。コンパイルできない `1 +` はテキストを変えず、エラー文字列が空でない。

テストは `ao_runtime_boot` を `setUp` で、`shutdown` を `tearDown` で呼ぶ。他テストとセッションが重ならないよう、このクラスは単独フィルタで走らせる。

- [ ] **Step 2: 赤を確認する**

Expected: FAIL（コマンドが未接続）。

- [ ] **Step 3: 実装する**

選択が空ならキャレット行を範囲にする。`NSTextView` の `string` を範囲で切り、`ao_eval` に渡す。Print it の挿入は選択の終端。Do it はテキストを変えない。エラーは窓の下の `NSTextField`。Inspector はフックと、コマンド側の `out` のどちらから開いても同じ 2 行にする。メニューの Smalltalk 項目は、キーウィンドウが Workspace のときそのコントローラへ委譲する。

- [ ] **Step 4: 緑を確認する**

`--filter WorkspaceEvalTests`。Expected: PASS。

- [ ] **Step 5: コミット**

```bash
git commit -m "Wire Do it Print it and Inspect it

Graphify: query ao_eval WorkspaceWindow
Serena: find_symbol WorkspaceWindow"
```

---

### Task 12: Browser の accept、階層、VoiceOver、v1 を閉じる

**Files:**

- Modify: `app/Ao/BrowserWindow.swift`
- Modify: `app/Ao/MainMenu.swift`（File の Save Image… / Open Image…、Show Hierarchy）
- Create: `app/AoTests/AcceptTests.swift`
- Create: `image/tests/p9-collect.st`
- Modify: `docs/bench.md`
- Modify: `SPEC.md` §6 の性能の 2 項目
- Modify: `docs/phases/P9.md` と `docs/prs/P9-01.md` … `P9-04.md`
- Modify: `PHASE` は `P9` のまま
- Modify: `runtime/src/Runtime.cpp` のバージョン文字列を `0.0.0-p9`

**Interfaces:**

- Consumes: `ao_accept_method`、`ao_accept_class`、`ao_browser_superclass`、`ao_browser_subclass_*`、`ao_image_save`、`ao_image_load`
- Produces: 受け入れの残り。ソースを壊さない失敗。階層コマンド。ラベル。ゴールデン。性能の記録。

- [ ] **Step 1: 失敗する Swift テスト**

Browser のソースに `foo\n  ^1\n` を入れて accept する。その後 Workspace で `Object new foo` を Print it すると `1`。ソースを `foo\n  ^\n` に変えて accept すると、テキストビューの文字列は `foo\n  ^\n` のまま、エラーラベルが空でない。`printString` の上書き accept もテキストを残し、評価した `Object new printString` はクラス名のまま（ネイティブが残る）。

Show Hierarchy のあと、モデルの表示名に `Integer`（SmallInteger のスーパ）が含まれる。`accessibilityLabel` が仕様の 8 文字列と一致する。

- [ ] **Step 2: 赤を確認する**

Expected: FAIL。

- [ ] **Step 3: UI を実装する**

accept は ABI が `AO_OK` のときだけモデルを `refresh` し、ラベルを空にする。失敗時は `textView.string` に代入しない。Show Hierarchy は同じ表のデータソースを鎖とサブクラスに差し替える。もう一度で `refresh` の通常一覧に戻る。

File の Save / Open は `NSSavePanel` / `NSOpenPanel` の 1 ファイル。成功したらセッションが入れ替わる。Open のあと Transcript フックをセットし直す（ABI がポインタを残すことの確認を、Open 後に `Transcript show: 'z'` する Swift テストで 1 本足す）。

ラベルは仕様の英語を各ビューの `setAccessibilityLabel` に付ける。

- [ ] **Step 4: ゴールデン**

`image/tests/p9-collect.st`:

```smalltalk
self assert: (#(1 2 3) collect: [:x | x * 2]) equals: #(2 4 6).
self assert: (Bag new add: 1; size) equals: 1.
```

`ao --test` が 0。Bag の行が赤なら、その 1 行を消して `image/vendor/DEFERRED.md` に「Bag size は P9 のゴールデンに入らなかった」と事実だけ足す。Bag を書き直さない。

- [ ] **Step 5: 性能の記録**

10M ループは再実行しない。`docs/bench.md` に P9 の日付と「比 5.76 を再計測していない」を 2 行足す。

起動時間は次を 1 回測り、ミリ秒を `docs/bench.md` に書く。

```cpp
// 一時テストにはしない。コマンドで測る。
// ao_runtime_boot(); ao_filein_load_order("<repo>/image/vendor/LOAD_ORDER");
```

2 秒を超えたら SPEC §6 の起動の項目にミリ秒と「ブートストラップと vendor file-in がこの時間」を書く。10 倍の項目は、チェックを外したままにせず、文を「`docs/bench.md` の比は 5.76 である。v1 はこの比をリリース条件にしない。」に変えてから `[x]` にする。言語とツールの `[ ]` は、対応するテストが緑のものだけ `[x]` にする。手動の窓操作（閉じる、最小化、Spaces）は `[ ]` のまま、`docs/phases/P9.md` に「起動して閉じる・最小化する」を 3 行書く。

- [ ] **Step 6: 検証**

`./scripts/test.sh` が緑。`./scripts/package-app.sh` が `jp.ao.smalltalk`。`graphify update .`。

- [ ] **Step 7: コミット**

```bash
git commit -m "Accept browser methods and record v1 acceptance

Graphify: update .
Serena: find_symbol ao_Object_printString"
```

バージョン文字列の変更が別の振る舞いなら、同じコミットに含めてよい。`PHASE` は `P9` のまま。

## 仕様との対応

| SPEC | タスク |
|---|---|
| §3.9 Transcript の show/cr/clear、等幅、閉じても生き残る | 2, 5 |
| §3.9 Workspace の Do it / Print it / Inspect it、self は nil、ワークスペース変数 | 9, 11 |
| §3.9 5 ペイン、インスタンス/クラス、accept、階層 | 3, 6, 10, 12 |
| §3.9 メニューと `⌘D` / `⌘I` | 7, 11 |
| §3.9 VoiceOver、過度なアニメーションを足さない | 12 |
| §3.10 起動、評価、一覧、accept、フック、エラー文字列 | 1, 2, 3, 9, 10 |
| §3.10 AppKit をヒープに置かない | 4, 5 |
| §3.11 版 1、関数ポインタを書かない | 1, 10 |
| §4.3 ABI 評価、一覧、accept、Do it / Print it | 4, 9, 10, 11, 12 |
| §6 `#(1 2 3) collect:` | 12 |
| §6 Browser から `Object>>foo` を足して Workspace で評価 | 10, 11, 12 |
| §6 イメージ再起動でメソッドが残る | 10, 12 |
| §6 Transcript に hello | 2, 9 |
| §6 性能 | 12 |
| §3.6 printString が Print it で読めること | 8 |

## リスク

- SwiftPM の `-force_load` は `swift test` をスクリプト経由で呼んだときだけ付く。素の `swift test` は未定義シンボルで落ちる。`scripts/test.sh` を正とする。
- セッションはプロセスに 1 つなので、ABI テストは shutdown を忘れると次が赤になる。各テストの末尾で `ao_runtime_shutdown` する。
- vendor file-in は起動を遅くする。2 秒を超えてもアルゴリズムは変えない。SPEC にミリ秒を書く。
- ブロック内代入がワークスペースを更新しないことは Task 9 のテストで固定する。直したくなったら SPEC を先に変える別計画にする。
- グラフ更新が API キー待ちで止まったら、`graphify update .` の失敗を報告してコミットしない。`graphify . --update` には切り替えない。

## 実行の選択

P8（Task 1 から Task 7）だけを先に実行する。P9（Task 8 から Task 12）は、P8 が main に入り `PHASE` が `P9` になってから別ブランチ `p9-integrate` で実行する。

実装ブランチは `.worktrees/p8-appkit`、ブランチ名 `p8-appkit`。main の作業ツリーでは編集しない。

**1. Subagent-Driven（推奨）** — タスクごとに新しいサブエージェント。タスクの間でレビュー。

**2. Inline Execution** — このセッションで executing-plans。チェックポイントで止める。

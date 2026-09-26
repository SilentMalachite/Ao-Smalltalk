# Ao

Ao は Apple Silicon Mac 向けの **新規** Smalltalk 処理系です。言語・オブジェクトモデル・開発ツールの契約は Smalltalk-80（Blue Book）に従います。Squeak / Pharo / OpenSmalltalk など既存処理系の VM ソースの移植ではありません。

English (canonical GitHub text): [README.md](README.md)

製品仕様: [SPEC.md](SPEC.md)（正本）。エージェント手順: [CLAUDE.md](CLAUDE.md)。貢献: [CONTRIBUTING.ja.md](CONTRIBUTING.ja.md)。変更履歴: [CHANGELOG.md](CHANGELOG.md)。

## 現状

**Ao 1.0.0** が最初のリリースです。[SPEC.md](SPEC.md) §2.3 のフェーズ P0–P9 をすべて終えています。これが v1 の定義です。

| 領域 | v1 でできること |
|---|---|
| オブジェクトメモリ | 64-bit tagged `Oop`、nursery と old 世代の正確な GC |
| クラス | Blue Book のメタクラス循環（`Object class class == Metaclass`、`Metaclass class class == Metaclass`） |
| Kernel | Kernel のメソッド（SPEC §3.6）はすべて C++ の `NativeMethod`。Kernel のメソッド辞書を走査し、それ以外があれば落ちるテストがある |
| コンパイラ | Smalltalk-80 ソース → AST → `CompiledMethod`（バイトコード）。チャンク形式の file-in |
| インタプリタ | ユーザーメソッド、外側の temp を共有するブロック、非局所リターン、`ensure:`、協調プロセス、`Semaphore`、`SharedQueue` |
| クラスライブラリ | 非 Kernel クラスを、版を固定した Cuis-Smalltalk の抜粋（`image/vendor`）から file-in する |
| イメージ | `.aoimage` の保存と読み込み。ネイティブメソッドはロード時にシンボル名で結び直す |
| ツール | AppKit の Transcript、Workspace（Do it / Print it / Inspect it）、Accept できる 5 ペインの System Browser |

## インストール

必要環境: Apple Silicon Mac、macOS 14 以降。Intel Mac は対象外です。

[Releases](https://github.com/SilentMalachite/Ao-Smalltalk/releases) から取得します。

| ファイル | 中身 |
|---|---|
| `Ao-1.0.0-macos-arm64.zip` | `Ao.app`、`LICENSE`、`NOTICE` |
| `ao-cli-1.0.0-macos-arm64.tar.gz` | コマンドラインツール `ao`、それが file-in するクラスライブラリ（`vendor/`）、`LICENSE`、`NOTICE` |
| `SHA256SUMS` | 上の 2 つのアーカイブの SHA-256 |

ダウンロードを確かめます。

```sh
shasum -a 256 -c SHA256SUMS
```

ビルドはアドホック署名だけで、公証していません。初回は macOS が起動を止めます。一度起動を試したあと **システム設定 → プライバシーとセキュリティ** で **このまま開く** を選ぶか、隔離属性を自分で外してください。

```sh
xattr -dr com.apple.quarantine Ao.app
xattr -d com.apple.quarantine ao-cli-1.0.0-macos-arm64/ao
```

## Ao.app の使い方

起動すると **Transcript** と **Workspace** が開き、同梱のクラスライブラリを file-in して、Transcript に `vendor loaded: …` を出します。

Workspace でソースを選択し、**Smalltalk** メニューを使います。

| コマンド | キー | 動作 |
|---|---|---|
| Do it | ⌘D | 評価し、結果を捨てる |
| Print it | ⌘I | 評価し、結果の `printString` を選択範囲の後ろに挿入する |
| Inspect it | — | 評価し、結果をインスペクタのウィンドウに出す |

```smalltalk
3 + 4.
#(1 2 3) collect: [:x | x * 2].
Transcript show: 'hello'; cr.
Object class class == Metaclass.
```

コンパイルエラーはテキストを変えずに、その区間を選択して示します。Workspace で未定義の名前はワークスペース変数になります。

**Tools → Browser** で System Browser が開きます。上にクラスカテゴリ、クラス（インスタンス側 / クラス側）、プロトコル、セレクタ、下にソースペインがあります。メソッドかクラス定義を編集して **Smalltalk → Accept** を選ぶと、直後の Do it に反映されます。**Smalltalk → Show Hierarchy** はクラス一覧を選択中のクラスの階層に置き換えます。もう一度選ぶと元に戻ります。Kernel のメソッドはネイティブなので Smalltalk のソースを持ちません。

**File → Save Image…** は自分のクラスとメソッドを含む `.aoimage` を書きます。**File → Open Image…** で読み戻します。

**Tools** には **Use Fixed Pitch** と文字の大きさのコマンド（**Make Text Bigger** ⌘+、**Make Text Smaller** ⌘-、**Actual Size** ⌘0）もあります。大きさは Transcript、Workspace、Browser のソースペインで共通です。

v1 の評価はメインスレッドで走り、中断できません。`[true] whileTrue` のように `yield` しないループはアプリを止めます。強制終了するしかなく、保存していない作業は失われます。

## `ao` CLI の使い方

```sh
ao --version                               # 1.0.0
ao filein hello.st                         # チャンク形式のファイルを 1 つ file-in する
ao filein --load-order vendor/LOAD_ORDER   # クラスライブラリを順に file-in する
ao --test tests/                           # tests/ の *.st をすべてゴールデンテストとして実行する
ao image save --load-order vendor/LOAD_ORDER my.aoimage
ao image load my.aoimage                   # イメージを読み込んで検査する
```

成功したときは何も出さず、終了コード 0 で終わります。エラーは標準エラーに出し、終了コードは 0 以外です。ゴールデンテストのファイルは、1 回評価する Smalltalk のソースです。

```smalltalk
self assert: 3 + 4 equals: 7.
self assert: (#(1 2 3) collect: [:x | x * 2]) equals: #(2 4 6).
```

`ao --test` の契約の全体は SPEC §4.4 にあります。

## v1 に無いもの

デバッガ、JIT（ユーザーメソッドはバイトコードで動く）、FFI、ネットワーク、Smalltalk の例外オブジェクトによる捕捉はありません。Squeak / Pharo / Xerox の `.image` は読めません。動くのは Apple Silicon の macOS だけです。非目標は SPEC §1.4 にあります。

## ソースからのビルド

必要環境: Apple Silicon の macOS 14 以降、CMake 3.28 以上、Ninja、C++20 の Apple Clang、Swift 6。初回のビルドで GoogleTest を取得します。

```sh
./scripts/test.sh             # build/、CTest、swift test
./scripts/test.sh --asan      # 同じものを AddressSanitizer で（build-asan/）
./scripts/package-app.sh      # Release ビルドと build/Ao.app（アドホック署名）
./scripts/test.sh --app       # Ao.app を作って起動し、クラスライブラリを読み込むことも確かめる
```

`--app` はウィンドウを開いてフォーカスを奪うので、既定では回しません。

同等の手順:

```sh
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
ctest --test-dir build --output-on-failure
swift test --package-path app \
  -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a \
  -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a \
  -Xlinker -lc++
```

CI は `main` への push と pull request ごとに `./scripts/test.sh` を回します（SPEC §4.5）。

## 構成

```
runtime/       C++20 オブジェクトメモリ、GC、ディスパッチ、Kernel ネイティブ、インタプリタ、イメージ、ao CLI
compiler/      C++20 字句・構文・バイトコード生成（ヒープを持たない）
bridge/        runtime と app の間の C ABI（ao_abi.h）
app/           Swift 6 + AppKit: Transcript、Workspace、System Browser、Inspector
image/Kernel/  Kernel のプロトコルコメント（定義は C++）
image/vendor/  版を固定した Cuis-Smalltalk のクラスライブラリ抜粋と読み込み順
image/tests/   ao --test 用の Smalltalk ゴールデンテスト
scripts/       ビルド、テスト、パッケージのスクリプト
docs/          フェーズ、PR、レビュー、ベンチマークの文書
graphify-out/  知識グラフ（コミットする）
```

## 文書

| ファイル | 役割 |
|---|---|
| [SPEC.md](SPEC.md) | 製品仕様（正本） |
| [CHANGELOG.md](CHANGELOG.md) | リリースノート |
| [CLAUDE.md](CLAUDE.md) | エージェント実装手順（Graphify + Serena） |
| [docs/README.md](docs/README.md) | フェーズ / PR 索引 |
| [docs/native-selectors.md](docs/native-selectors.md) | ネイティブセレクタのマングル |
| [docs/bench.md](docs/bench.md) | ネイティブとインタプリタのマイクロベンチ |
| [image/vendor/ORIGIN.md](image/vendor/ORIGIN.md) | クラスライブラリの取り込み元と版 |

GitHub 向け文書（`README.md`、`CONTRIBUTING.md`、`CHANGELOG.md`、`LICENSE`）の正本は英語です。このファイルは翻訳です。

## ライセンス

Copyright 2026 Ao contributors.

**Apache License, Version 2.0**。条文の正本は英語の [LICENSE](LICENSE)。[NOTICE](NOTICE) も参照してください。

`image/vendor/cuis` のクラスライブラリは [Cuis-Smalltalk](https://github.com/Cuis-Smalltalk/Cuis-Smalltalk-Dev) に由来し、元の MIT ライセンス（[image/vendor/cuis/LICENSE](image/vendor/cuis/LICENSE)）のままです。

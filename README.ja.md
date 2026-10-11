# Ao

[![CI](https://github.com/SilentMalachite/Ao-Smalltalk/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/SilentMalachite/Ao-Smalltalk/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/SilentMalachite/Ao-Smalltalk)](https://github.com/SilentMalachite/Ao-Smalltalk/releases)
[![License](https://img.shields.io/github/license/SilentMalachite/Ao-Smalltalk)](LICENSE)
![Platform](https://img.shields.io/badge/platform-macOS%2014%2B%20%28Apple%20Silicon%29-lightgrey?logo=apple)
![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus)
![Swift 6](https://img.shields.io/badge/Swift-6-F05138?logo=swift&logoColor=white)

Ao は Apple Silicon Mac 向けの **新規** Smalltalk 処理系です。言語・オブジェクトモデル・開発ツールの契約は Smalltalk-80（Blue Book）に従います。Squeak / Pharo / OpenSmalltalk など既存処理系の VM ソースの移植ではありません。

English (canonical GitHub text): [README.md](README.md)

製品仕様: [SPEC.md](SPEC.md)（正本）。エージェント手順: [CLAUDE.md](CLAUDE.md)。貢献: [CONTRIBUTING.ja.md](CONTRIBUTING.ja.md)。変更履歴: [CHANGELOG.md](CHANGELOG.md)。

## 現状

**Ao 1.4.0** が現在のリリースです。[SPEC.md](SPEC.md) §2.3 のフェーズ P0–P15 をすべて終えています。P0–P9 が v1 の定義で、1.0.0 としてリリースしました。P10–P12 でデバッガと Browser の削除を足し（1.1.0）、P13 で評価の中断を（1.2.0）、P14 で Debugger の Restart を（1.3.0）、P15 で Debugger の編集と Accept を足しました（1.4.0）。

| 領域 | できること |
|---|---|
| オブジェクトメモリ | 64-bit tagged `Oop`、nursery と old 世代の正確な GC |
| クラス | Blue Book のメタクラス循環（`Object class class == Metaclass`、`Metaclass class class == Metaclass`） |
| Kernel | Kernel のメソッド（SPEC §3.6）はすべて C++ の `NativeMethod`。Kernel のメソッド辞書を走査し、それ以外があれば落ちるテストがある |
| コンパイラ | Smalltalk-80 ソース → AST → `CompiledMethod`（バイトコード）。チャンク形式の file-in |
| インタプリタ | ユーザーメソッド、外側の temp を共有するブロック、非局所リターン、`ensure:`、協調プロセス、`Semaphore`、`SharedQueue` |
| クラスライブラリ | 非 Kernel クラスを、版を固定した Cuis-Smalltalk の抜粋（`image/vendor`）から file-in する |
| イメージ | `.aoimage` の保存と読み込み。ネイティブメソッドはロード時にシンボル名で結び直す |
| ツール | AppKit の Transcript、Workspace（Do it / Print it / Inspect it / Debug it）、Accept と削除ができる 5 ペインの System Browser |
| デバッガ | `halt` と失敗で評価を止め、Debugger 窓で Proceed、Abort、Step over、Step into、Step out を行う（P10、P11）。走っている評価も ⌘. でここに止める（P13）。選んだフレームを Restart でやり直す（P14）。ライブ Debugger のソース欄は編集して Accept でき、新しいメソッドでそのフレームを最初からやり直す（P15） |

## インストール

必要環境: Apple Silicon Mac、macOS 14 以降。Intel Mac は対象外です。

[Releases](https://github.com/SilentMalachite/Ao-Smalltalk/releases) から取得します。

| ファイル | 中身 |
|---|---|
| `Ao-1.4.0-macos-arm64.zip` | `Ao.app`、`LICENSE`、`NOTICE` |
| `ao-cli-1.4.0-macos-arm64.tar.gz` | コマンドラインツール `ao`、それが file-in するクラスライブラリ（`vendor/`）、`LICENSE`、`NOTICE` |
| `SHA256SUMS` | 上の 2 つのアーカイブの SHA-256 |

ダウンロードを確かめます。

```sh
shasum -a 256 -c SHA256SUMS
```

ビルドはアドホック署名だけで、公証していません。初回は macOS が起動を止めます。一度起動を試したあと **システム設定 → プライバシーとセキュリティ** で **このまま開く** を選ぶか、隔離属性を自分で外してください。

```sh
xattr -dr com.apple.quarantine Ao.app
xattr -d com.apple.quarantine ao-cli-1.4.0-macos-arm64/ao
```

## Ao.app の使い方

起動すると **Transcript** と **Workspace** が開き、同梱のクラスライブラリを file-in して、Transcript に `vendor loaded: …` を出します。

Workspace でソースを選択し、**Smalltalk** メニューを使います。

| コマンド | キー | 動作 |
|---|---|---|
| Do it | ⌘D | 評価し、結果を捨てる |
| Print it | ⌘I | 評価し、結果の `printString` を選択範囲の後ろに挿入する |
| Inspect it | — | 評価し、結果をインスペクタのウィンドウに出す |
| Debug it | ⌘⇧D | Debugger の中で評価する。最初の文の前で止まる |
| Interrupt | ⌘. | 走っている評価を Debugger に止める（理由 `interrupted`） |
| Accept | — | ライブ Debugger で編集したメソッドを入れ、そのフレームを新しいメソッドでやり直す（Smalltalk → Accept。編集できるフレームのとき） |

```smalltalk
3 + 4.
#(1 2 3) collect: [:x | x * 2].
Transcript show: 'hello'; cr.
Object class class == Metaclass.
```

コンパイルエラーはテキストを変えずに、その区間を選択して示します。Workspace で未定義の名前はワークスペース変数になります。

**Tools → Browser** で System Browser が開きます。上にクラスカテゴリ、クラス（インスタンス側 / クラス側）、プロトコル、セレクタ、下にソースペインがあります。メソッドかクラス定義を編集して **Smalltalk → Accept** を選ぶと、直後の Do it に反映されます。**Smalltalk → Show Hierarchy** はクラス一覧を選択中のクラスの階層に置き換えます。もう一度選ぶと元に戻ります。Kernel のメソッドはネイティブなので Smalltalk のソースを持ちません。

メソッドやクラスを削除するには、Browser で右クリックするか **Smalltalk → Remove Method…** / **Remove Class…** を選び、確認します。元に戻す手段はありません。ネイティブメソッド、Kernel クラス、サブクラスが残っているクラスは削除を拒み、Browser に理由を出します。クラスを削除すると、その名前が `Smalltalk` から外れます。既存のインスタンスはそのまま動きます。

### デバッガ

評価が `halt` を呼ぶか失敗する（エラー、`doesNotUnderstand:`、失敗した送信）と、評価が止まって **Debugger** 窓が開き、Workspace に `halted: <理由>` が出ます。Debugger にはフレームの一覧、選んだフレームのソース（いまの送信を選択して示す）、`self`・引数・temp とその値が並びます。値をダブルクリックするとインスペクタが開きます。

| ボタン | 動作 |
|---|---|
| Proceed | 続ける。止めた送信は `nil` を返す |
| Step over | このメソッドの次の文か、呼び出し元に戻ったところで止まる |
| Step into | 加えて、送信した先のメソッドやブロックの最初の命令でも止まる |
| Step out | 呼び出し元に戻ったところで止まる |
| Restart | 選んだメソッドかブロックのフレームを先頭からやり直す（内側のフレームは捨て、その `ensure:` は走らない）。ネイティブの行ではできない |
| Abort | 評価を終わらせる（`ensure:` のブロックは走る） |

Proceed や step で Print it が終わると、結果が Workspace に入ります。Debugger を閉じると評価を中止します。スタックあふれ、メモリ不足、デッドロックは止めずに中止します。そのときは Workspace に **Debug** ボタンが出て、中止が始まったときのスタックを開けます。評価が止まっている間は、イメージを保存できません。

**File → Save Image…** は自分のクラスとメソッドを含む `.aoimage` を書きます。**File → Open Image…** で読み戻します。

**Tools** には **Use Fixed Pitch** と文字の大きさのコマンド（**Make Text Bigger** ⌘+、**Make Text Smaller** ⌘-、**Actual Size** ⌘0）もあります。大きさは Transcript、Workspace、Browser のソースペインで共通です。

評価はメインスレッドで走ります。`[true] whileTrue` のように `yield` しないループは、**Smalltalk → Interrupt**（⌘.）で Debugger に止められます（理由 `interrupted`）。Proceed は止めたところから続けます。評価が fork したプロセスのループや、インタプリタに戻らない長いネイティブは、強制終了するしかありません。

## `ao` CLI の使い方

```sh
ao --version                               # 1.4.0
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

## Ao に無いもの

JIT（ユーザーメソッドはバイトコードで動く）、FFI、ネットワーク、Smalltalk の例外オブジェクトによる捕捉はありません。Debugger はネイティブメソッドの中に step into しません。事後 Debugger と doIt のフレームは編集できません。Squeak / Pharo / Xerox の `.image` は読めません。動くのは Apple Silicon の macOS だけです。非目標は SPEC §1.4 にあります。

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
app/           Swift 6 + AppKit: Transcript、Workspace、System Browser、Inspector、Debugger
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

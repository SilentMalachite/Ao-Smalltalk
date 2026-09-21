# Ao

Ao は Apple Silicon Mac 向けの **新規** Smalltalk 処理系です。言語・オブジェクトモデル・開発ツールの契約は Smalltalk-80（Blue Book）に従います。Squeak / Pharo / OpenSmalltalk など既存処理系の VM ソースの移植ではありません。

English (canonical GitHub text): [README.md](README.md)

製品仕様: [SPEC.md](SPEC.md)。エージェント手順: [CLAUDE.md](CLAUDE.md)。貢献: [CONTRIBUTING.ja.md](CONTRIBUTING.ja.md)。

## 現状

現行フェーズはルートの [`PHASE`](PHASE)（`P6`）。P0–P5 は完了しています。

| 完了 | いま動くもの |
|---|---|
| オブジェクトメモリ | 64-bit tagged `Oop`、nursery + old GC |
| ブートストラップ | Blue Book 6–10 のメタクラス循環、Kernel カタログ |
| ディスパッチ | `NativeMethod`、lookup、`send` |
| Kernel | SPEC §3.6 のクラスを C++ ネイティブ（バイトコードではない） |
| コンパイラ | Smalltalk-80 ソース → AST → `CompiledMethod`（バイトコード）。チャンク file-in |

**まだ無いもの:** バイトコードインタプリタ（P6）、vendor クラスライブラリの file-in（P6b）、`.aoimage` の save/load（P7）、AppKit の Browser / Transcript / Workspace（P8–P9）。

ユーザーメソッドはヒープ上の `CompiledMethod` になる。実行は P6 までしない。Kernel メソッドは C++ の `NativeMethod` のまま。

## 必要環境

- Apple Silicon の macOS
- CMake 3.28 以上、Ninja、C++20（Apple Clang）
- Swift 6（AppKit のスモークテスト）

Intel Mac は対象外。

## ビルドとテスト

```sh
./scripts/test.sh
```

`build/` を構成し、runtime と compiler をビルドし、CTest と `swift test --package-path app` を実行する。

同等の手順:

```sh
cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
ctest --test-dir build --output-on-failure
swift test --package-path app
```

## 構成

```
runtime/      C++20 オブジェクトメモリ、Kernel ネイティブ、ディスパッチ
compiler/     C++20 字句・構文・バイトコード生成（ヒープを持たない）
image/        Kernel のプロトコルコメント。vendor .st は後続
app/          Swift 6 + AppKit（骨格）
bridge/       C ABI のみ（`ao_abi.h`）
docs/         フェーズと PR の実行文書
graphify-out/ 知識グラフ（コミットする）
```

## 文書

| ファイル | 役割 |
|---|---|
| [SPEC.md](SPEC.md) | 製品仕様（正本） |
| [CLAUDE.md](CLAUDE.md) | エージェント実装手順（Graphify + Serena） |
| [docs/README.md](docs/README.md) | フェーズ / PR 索引 |
| [docs/native-selectors.md](docs/native-selectors.md) | ネイティブセレクタのマングル |
| [docs/bench.md](docs/bench.md) | P4 ネイティブマイクロベンチ |

GitHub 向け文書（`README.md`、`CONTRIBUTING.md`、`LICENSE`）の正本は英語。このファイルは翻訳。

## ライセンス

Copyright 2026 Ao contributors.

**Apache License, Version 2.0**。条文の正本は英語の [LICENSE](LICENSE)。[NOTICE](NOTICE) も参照。

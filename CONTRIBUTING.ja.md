# 貢献

English (canonical): [CONTRIBUTING.md](CONTRIBUTING.md)

製品仕様は [SPEC.md](SPEC.md)。エージェント手順は [CLAUDE.md](CLAUDE.md)。チャットは正本ではない。

## 現行フェーズ

ルートの [`PHASE`](PHASE) を一行で読む。そのフェーズだけ実装する。後続（インタプリタ UI、vendor file-in、JIT）を先取りしない。

## 曲げない規則

- Kernel のクラスとメソッドは C++ の `NativeMethod`。`image/Kernel/*.st` はプロトコルコメントであり、実行定義ではない。
- Squeak / Pharo / OpenSmalltalk など既存処理系の VM ソースをコピーしない。
- 現行フェーズの SPEC.md に無い機能を足さない。
- 公開ポインタは `ao::Oop`。API 境界で C++ 例外を投げない。
- Swift アプリは `bridge/ao_abi.h` だけを通してランタイムを呼ぶ。

## ビルド

Apple Silicon macOS、CMake 3.28 以上、Ninja、C++20、Swift 6。

```sh
./scripts/test.sh
```

テストが赤のまま進めない。

## コミット

英語の命令形（例: `Add tagged OOP tests`）。エージェントが Graphify / Serena を使ったセッションでは、CLAUDE.md のとおり一行トレーラを付ける。

## ライセンス

貢献は Apache License, Version 2.0。条文は英語の [LICENSE](LICENSE)。[NOTICE](NOTICE) も参照。

# 貢献

English (canonical): [CONTRIBUTING.md](CONTRIBUTING.md)

製品仕様は [SPEC.md](SPEC.md)。エージェント手順は [CLAUDE.md](CLAUDE.md)。チャットは正本ではない。

## いまの位置

v1（SPEC §2.3 のフェーズ P9）は 1.0.0 としてリリース済み。ルートの [`PHASE`](PHASE) は `P9` のまま。v1 のあとの変更は SPEC.md から始める。先に仕様を直し（SPEC §8）、次にテスト、最後にコードを書く。SPEC §1.4 の項目（JIT、FFI、デバッガなど）は、仕様が取り込むまで入れない。

## 曲げない規則

- Kernel のクラスとメソッドは C++ の `NativeMethod`。`image/Kernel/*.st` はプロトコルコメントであり、実行定義ではない。
- Squeak / Pharo / OpenSmalltalk など既存処理系の VM ソースをコピーしない。
- SPEC.md に無い機能を足さない。
- 公開ポインタは `ao::Oop`。API 境界で C++ 例外を投げない。
- Swift アプリは `bridge/ao_abi.h` だけを通してランタイムを呼ぶ。

## ビルド

Apple Silicon の macOS 14 以降、CMake 3.28 以上、Ninja、C++20、Swift 6。

```sh
./scripts/test.sh
```

テストが赤のまま進めない。CI は `main` への push と pull request ごとに同じスクリプトを回し、`main` は CI が緑の変更だけを受け入れる（SPEC §4.5）。この実行には Kernel 走査テストが含まれ、`NativeMethod` でない Kernel メソッドがあれば落ちる。

## コミット

英語の命令形（例: `Add tagged OOP tests`）。エージェントが Graphify / Serena を使ったセッションでは、CLAUDE.md のとおり一行トレーラを付ける。

## リリース

版、添付物、タグを打てる条件は SPEC §2.4 にある。`V` を新しい版として:

1. ブランチで `ao::version_string`（`runtime/src/Runtime.cpp`）と `ao::compiler::version`（`compiler/src/Compiler.cpp`）を `V` にし、テストも合わせる。`CHANGELOG.md` に項目を足す。CI が緑になってからマージする。
2. `main` で SPEC §6 がすべて `[x]` であることを確かめ、アプリをビルドして確かめる。

   ```sh
   ./scripts/test.sh --app
   ```

3. 添付物を `build/dist` に用意する。

   ```sh
   V=1.0.0
   D=build/dist
   rm -rf "$D" && mkdir -p "$D/Ao-$V-macos-arm64" "$D/ao-cli-$V-macos-arm64"
   ditto build/Ao.app "$D/Ao-$V-macos-arm64/Ao.app"
   cp LICENSE NOTICE "$D/Ao-$V-macos-arm64/"
   cp build-release/ao LICENSE NOTICE "$D/ao-cli-$V-macos-arm64/"
   cp -R image/vendor "$D/ao-cli-$V-macos-arm64/vendor"
   find "$D" -name .DS_Store -exec rm -f {} +
   (cd "$D" &&
     ditto -c -k --norsrc --noextattr --noacl --keepParent "Ao-$V-macos-arm64" "Ao-$V-macos-arm64.zip" &&
     tar --no-xattrs --no-acls -czf "ao-cli-$V-macos-arm64.tar.gz" "ao-cli-$V-macos-arm64" &&
     shasum -a 256 "Ao-$V-macos-arm64.zip" "ao-cli-$V-macos-arm64.tar.gz" > SHA256SUMS)
   ```

4. タグを打って公開する。ノートは `CHANGELOG.md` の項目を使う。

   ```sh
   git tag -a "v$V" -m "Ao $V"
   git push origin "v$V"
   gh release create "v$V" --title "Ao $V" --notes-file <notes> \
     "$D/Ao-$V-macos-arm64.zip" "$D/ao-cli-$V-macos-arm64.tar.gz" "$D/SHA256SUMS"
   ```

## ライセンス

貢献は Apache License, Version 2.0。条文は英語の [LICENSE](LICENSE)。[NOTICE](NOTICE) も参照。

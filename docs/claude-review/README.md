# Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）

リポジトリ全体を 7 領域に分けてレビューし、別途 `origin/main..HEAD` の直近差分もレビューした。各領域の指摘は、ほぼすべてビルド済みの `build/*.a` にプローブを当てて再現を確かめている。例外は各ファイルの「根拠」欄に書いた。プローブはリポジトリの外に置き、リポジトリのコードは変更していない。

## 索引

| ファイル | 領域 | Critical | High | Medium | Low |
|---|---|---|---|---|---|
| [00-recent-diff.md](00-recent-diff.md) | 直近 6 コミット（Accept / Workspace / Browser） | 5 | 8 | 1 | 1 |
| [01-object-memory.md](01-object-memory.md) | オブジェクトメモリ・GC・ブートストラップ | 1 | 4 | 2 | 2 |
| [02-interpreter.md](02-interpreter.md) | インタプリタ・送信・コンテキスト・プロセス | 1 | 3 | 2 | 3 |
| [03-kernel-numeric.md](03-kernel-numeric.md) | 数値・スカラー系 Kernel | 1 | 2 | 3 | 8 |
| [04-kernel-objects-collections.md](04-kernel-objects-collections.md) | Object・Behavior・コレクション系 Kernel | 1 | 2 | 8 | 8 |
| [05-compiler.md](05-compiler.md) | スキャナ・パーサ・コード生成・チャンク | 2 | 6 | 5 | 4 |
| [06-image-session-abi.md](06-image-session-abi.md) | イメージ・セッション・C ABI・CLI | 0 | 4 | 3 | 6 |
| [07-app-build.md](07-app-build.md) | Swift アプリ・ビルド・スクリプト | 0 | 1 | 5 | 7 |
| **合計（重複を除く）** | | **11** | **29** | **28** | **39** |

重複の扱い:
- 03 の High「int64 を超える整数リテラルが 0 になる」は、05 の High と同じ問題。
- 05 の Medium「file-in のコンパイルエラーが成功扱い」は、06 の High と同じ問題。
- 00 の「ClassMethodCache の無効化が定義クラスの分だけ」（Accept 経路）と、05 の「チャンク経由の再定義でキャッシュを無効化しない」（file-in 経路）は根が同じ。どちらも、キャッシュを無効化する処理が経路ごとにばらばらなことから起きている。

問題がなかった点:
- Kernel メソッドは、すべて NativeMethod として入っている（KernelScan テストで確認）。
- Kernel の `.st` に実行定義はない。
- 整数の基本演算は、Python と 1500 件突き合わせて一致した。
- Swift 6 の strict concurrency ビルドで警告は 0 件。GUI 型はすべて `@MainActor` が付いている。

## 横断テーマ

### 1. GC 安全性（メモリ破壊）
ネイティブにはハンドルスコープがない。`NativeFn` は receiver を値で受け取り、ディスパッチ経路のどこでもルートされない。RAII の `Root` は 16 ファイルに重複して定義されている。この構造が、次の 3 つの Critical を生んでいる。
- 01: スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む（`Gc.cpp:104,119`）
- 03: `Magnitude>>#<=` / `between:and:` が、GC 後の古い receiver にメッセージを送る（`Magnitude.cpp:28-35`）
- 04: `subclass:` が、ルートしていない superclass と名前をクラスに書き込む（`Behavior.cpp:228`）

これに加え、01 の High「old が枯渇してスキャベンジが失敗すると、オブジェクトの同一性が崩れる」もこの系統に入る。

### 2. 失敗が黙って成功になる
戻り値やコードから、エラーと成功の区別がつかなくなっている。
- 02: 失敗を表す空の OOP が値としてスタックに積まれ、`Message` に化ける。
- 01: 1MB を超える割り当ての失敗。メソッド辞書を拡張できないと、メソッドが捨てられる。
- 00: `ao_accept_class` が、クラス定義でない入力にも `AO_OK` を返す。
- 06: vendor file-in で 17 メソッドが欠落しても `AO_OK` を返す。`ao --test` が実行時エラーを失敗として数えない。

### 3. 言語意味論の欠落（コンパイラ）
- 05: ブロックの中で外側の temp に代入しても反映されない。`ifTrue:` / `whileTrue:` はインライン展開されない。
- 05: `,` をスキャンできない。`[:x | | t | ...]` を構文解析できない。クラス変数を読み書きできない。`$'` のあとを読み飛ばす。
- 02: `whileTrue:`、`timesRepeat:`、`ensure:` が実装されていない。コレクションの `do:` が非局所リターンを無視する。
- 03: `1 + 1.5` のように、Integer を左辺にした混合演算が失敗する。

### 4. Browser / Accept でのデータ消失
- 00: クラス定義を再 Accept すると全メソッドが消える。プレースホルダを Accept すると本体が消える。
- 07: 選択を変えると未保存の編集が黙って捨てられる。Undo がない。スマート引用符の置換で、文字列リテラルがコンパイルできなくなる。

### 5. 資源の上限と、その先の振る舞い
- 02: 再帰の深さに上限がなく、約 2,430 段で SIGSEGV になる。
- 01 / 06: old space が 4MB で固定されている。そのため GC が止まったように遅くなり、保存できても読み込めないイメージもできる。
- 06: イメージ保存が原子的でなく、失敗すると既存のイメージが壊れる。
- 00: Workspace の束縛が 255 個を超えると、eval がすべて失敗する。

## 推奨する着手順
1. **GC 安全性（テーマ 1）**: `ao/HandleScope.hpp` に `Root` / `RootedArray` をまとめ、`NativeMethod::apply` で receiver と args をルートする。スキャベンジの途中では `collectOld` を走らせない。
2. **ブロックの変数捕捉と制御構造のインライン展開（05 Critical）**: vendor のコードが 57 か所で依存しており、正しく動くコードの前提になる。
3. **失敗の伝搬（テーマ 2）**: 空の OOP を値として扱わず、エラーとして返す。file-in / accept は、エラーの件数を返り値に反映する。
4. **Browser でのデータ消失（00 Critical ×3、07 High）**: ユーザーのコードを失う問題を止める。
5. **再帰深度のガード**と、**イメージ保存の原子化**（一時ファイルに書いてから rename する）。

# P6b — vendor file-in

## 結論

`image/vendor` を LOAD_ORDER どおり載せ、非 Kernel を CompiledMethod にする。ライブラリを自作しない。

## 前提

P6 緑。SPEC §3.12。

## 範囲

**やる:** Cuis をピン、ORIGIN.md、LOAD_ORDER、Kernel 上書き禁止、host 移設パッチ、file-in。

**やらない:** Morphic / MVC / BitBlt / Form、Monticello / Metacello / Iceberg、ネットワーク一式、Xerox 1983 仮想イメージ。

## 仕様

一次候補 Cuis Smalltalk（MIT、Blue Book に近い）。足りないパッケージだけ Squeak MIT。ライセンス全文を同梱。新規 C++ / Swift は Apache License 2.0。混在は `NOTICE`。

NativeMethod があるセレクタは上書き禁止。

## TDD

vendor 由来メソッドがネイティブ必須セレクタを上書きしていないこと。file-in した非 Kernel が CompiledMethod として評価できること（SPEC §6）。

## 制約

`ORIGIN.md` 未確定のまま file-in しない。Pharo / Squeak の VM・プラグイン・BitBlt を使わない。

## 受け入れ

- [x] ORIGIN.md に URL・コミット・ライセンス場所がある
- [x] LOAD_ORDER が固定されている
- [x] 上書き禁止テスト緑
- [x] 非 Kernel が CompiledMethod

## PR 一覧

[P6b-01](../prs/P6b-01.md) … [P6b-04](../prs/P6b-04.md)

## Graphify / Serena

vendor 追加はディレクトリ追加なので deep 更新。

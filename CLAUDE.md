# CLAUDE.md — Ao を Claude Code / Grok Build で実装するときの必読

Grok Build は本ファイルと `AGENTS.md` の両方を読む。内容が衝突したら **本ファイルがプロセス、`SPEC.md` が製品** を優先する。ユーザーのその場の指示が最優先。

チャットは正本ではない。仕様は `SPEC.md`、手順は本ファイル。

---

## 結論（先に読む）

1. 作るものは `SPEC.md` の v1 だけである。頼まれていない機能を足さない。
2. このリポジトリはモジュール横断が多く、メタクラス循環・ネイティブディスパッチ・コンパイラ・AppKit が互いに参照する。**Graphify と Serena を使わずに実装・改修してはならない。**
3. Kernel メソッドは C++ のネイティブ関数にする。Smalltalk ソースを実行定義にしない。
4. テストが赤のまま次フェーズへ進まない。
5. 既存 Smalltalk 処理系のソースをコピーしない。

Graphify または Serena が使えないセッションでは、コードを書かず、使えなくなっている事実と復旧手順だけを報告する。

---

## 前提

- 製品仕様: `SPEC.md`（必読。推測で埋めない）
- 対象: Apple Silicon、macOS、C++20 runtime + Swift 6 AppKit
- エージェント: Claude Code と Grok Build。両方とも本ファイルに従う
- 複雑さの理由: オブジェクトメモリ、クラス/メタクラスの循環、メソッド辞書、バイトコード、ホスト GUI が同一概念を別言語で共有する

---

## 必須ツール — Graphify と Serena

このプロジェクトでは次を **必須** とする。推奨ではない。省略は仕様違反である。

| ツール | 役割 | 使わない場合に起きること |
|---|---|---|
| Graphify | リポジトリ全体の知識グラフ。コミュニティ、神ノード、モジュール間パス | エージェントがファイルを横断 grep し、循環と境界を壊す |
| Serena | LSP シンボル単位の検索・参照・置換 | 行番号編集でメソッド辞書やブートストラップを壊す |

両者は相補である。Graphify が地図、Serena が現地の手術である。どちらか一方では足りない。

### Graphify — いつ・何を

初期化（P0、およびグラフが無いとき）:

```
/graphify .
```

以降、**コードまたは SPEC を変更したセッションの終わり** に:

```
/graphify . --update
```

構造が大きく変わったら（クラス追加、ディレクトリ追加、ディスパッチ変更）:

```
/graphify . --mode deep
```

成果物（コミット対象）:

```
graphify-out/graph.html
graphify-out/GRAPH_REPORT.md
graphify-out/graph.json
```

セッション開始時の義務:

1. `graphify-out/GRAPH_REPORT.md` を読む。神ノードとコミュニティを把握する。
2. 改修対象が他モジュールに触れるなら `graphify path` / `graphify query` / `graphify explain`（または MCP の `query_graph`, `shortest_path`, `get_neighbors`）で経路を取る。
3. 経路に出たファイルだけを作業対象の候補にする。グラフに無い新規ファイルを足すときは SPEC のディレクトリ契約に従う。

禁止:

- グラフを無視してリポジトリを広く grep して設計判断すること
- `graphify-out/` を gitignore して捨てること
- セッション終了時にグラフを更新せず「後で」にすること

### Serena — いつ・何を

セッション開始時の義務:

1. `activate_project` で本リポジトリをアクティブにする。
2. `check_onboarding_performed` が未了ならオンボーディングを行い、プロジェクト記憶に「ブートストラップ順序」「ABI 境界」「Kernel はネイティブ」を残す。
3. 既存シンボルを触る作業では、ファイル全体を読む前に:
   - `get_symbols_overview`（ファイルの外形）
   - `find_symbol`（定義）
   - `find_referencing_symbols`（全参照）
4. 本体の書き換えは `replace_symbol_body` / `insert_after_symbol` / `insert_before_symbol` を優先する。
5. リネームは `rename_symbol`。検索置換で名前を変えない。
6. C++ または Swift を Serena 外で大きく変えたら `restart_language_server`。

禁止:

- 行番号と文字列マッチだけでメソッドを移植すること
- `Object` / `Behavior` / `lookup` / `gc` 系の参照を取らずにシグネチャを変えること
- Serena が解決できるシンボルを「たぶんこのファイル」で開くこと

### 標準ワークフロー（すべての実装タスク）

```
[1] Graphify: GRAPH_REPORT を読む。必要なら query / path / explain
[2] Serena:  activate_project → find_symbol → find_referencing_symbols
[3] SPEC.md: 変更が仕様に含まれるか確認。含まれなければ SPEC を先に直す
[4] テストを先に書く（赤）
[5] Serena のシンボル編集で実装する
[6] テストを緑にする。関連スイートも回す
[7] Graphify --update
[8] 差分を最小にして止める。隣のリファクタをしない
```

ステップ [1] と [2] を飛ばした実装は未完了である。PR / コミットメッセージに「Graphify: … / Serena: …」と、使ったクエリまたはシンボル名を一行で残す。

### ツールが死んでいるとき

- Serena の言語サーバが Swift / C++ を解決しない → コードを増やさず、`restart_language_server`、コンパイルデータベース、Xcode / clangd の状態を報告する。
- Graphify が空または古い → `/graphify .` を先に実行する。失敗したらコードを書かない。
- 両方失敗 → ユーザーに環境復旧を求める。推測実装をしない。

---

## スコープ

やってよいこと:

- `SPEC.md` の現行フェーズとその受け入れ基準を満たす実装
- テスト、ビルド、グラフ更新、最小のドキュメント修正
- SPEC の欠陥を見つけたときの SPEC 更新（実装より先）

やってはいけないこと:

- 次フェーズの先取り（例: P3 で Browser を作り始める）
- JIT、FFI、BitBlt、デバッガ、パッケージマネージャ
- Squeak / Pharo / OpenSmalltalk / dbanay VM からのコピー
- 「あると便利」なユーティリティクラスの量産
- README の長文装飾、バッジ、スクリーンショット自動化
- SwiftUI への主系統移行
- 依存ライブラリの追加（SPEC 5 節に無いもの）。必要なら SPEC を先に更新してユーザー確認

---

## コーディング規約

### 共通

- 識別子は英語。ユーザー向け文書と `.st` コメントは日本語可
- 一行の責務を混ぜない
- 公開 API を足すならテストを同時に足す
- マジックナンバーを well-known 表か定数ヘッダへ

### C++20（`runtime/`, `compiler/`）

- C++20。例外を境界で投げない
- OOP は独自型 `ao::Oop`（生の `uint64_t` を散らさない）
- 所有は明確。GC 管理オブジェクトを `std::shared_ptr` に載せない
- receiver をローカル変数にコピーして GC をまたがない。ネイティブの `receiver` と `args` はルート済みのスロットなので、GC しうる呼び出し（send、`allocateRetry` など）の後は、そこから読み直す
- ヘッダは最小。循環 include を作らない（Graphify のサイクルをセッションごとに見る）
- ネイティブメソッドのシンボル名は安定させる: `ao_<Class>_<selectorMangled>`
  - 例: `ao_Object_identityEquals`（`==`）、`ao_SmallInteger_add`（`+`）
- セレクタのマングル規則は `docs/native-selectors.md` に一度だけ書く（P3 で作成）

### Swift 6（`app/`）

- AppKit が正。`@MainActor` を GUI に付ける
- ランタイム呼び出しは `bridge/ao_abi.h` のみ
- 強制アンラップを新規に増やさない

### Smalltalk（`image/`）

- Kernel の `.st` はプロトコルとコメント用。実行定義を書かない
- 非 Kernel はチャンク形式。セレクタとプロトコルを Browser と同じ分類にする

### テスト

- C++: GoogleTest
- Swift: XCTest
- Smalltalk: `image/tests/*.st` を `ao --test` で実行
- テスト名は行為と期待を含む

---

## フェーズ進行

`SPEC.md` §2.3 の順を守る。現在フェーズはリポジトリルートの `PHASE` ファイルに一行で書く（例: `P3`）。無いときは P0 として扱う。

フェーズを進める条件:

1. そのフェーズの受け入れテストが緑
2. Kernel 走査テストがあるフェーズ以降、Kernel メソッドがすべて `NativeMethod`
3. Graphify 更新済み
4. `PHASE` を書き換え、SPEC のチェックリストを更新

---

## 設計判断の固定（変更するなら SPEC 先）

- 64-bit tagged pointer。オブジェクトテーブルなし
- Kernel = NativeMethod。ユーザーメソッド = CompiledMethod（v1 バイトコード）
- メタクラス規則は Blue Book 6–10
- GUI は独立 NSWindow。クラシック BitBlt 画面ではない
- イメージに関数ポインタを書かない。ロード時にシンボル名で再結合

迷ったら「ネイティブ Kernel を薄く保ち、ツールはホストに置く」。Smalltalk 側に IDE フレームワークを再実装しない（モデル状態だけ Smalltalk、ビューは AppKit）。

---

## コミット

- 英語の短い命令形: `Add tagged OOP tests`
- 本文に Graphify / Serena の一行トレーラ:
  ```
  Graphify: path ObjectMemory NativeMethod
  Serena: replace_symbol_body ao::Interpreter::send
  ```
- 生成物のうち `graphify-out/` は含めてよい。ビルド成果は含めない

---

## 受け入れ（エージェント作業の完了条件）

作業を「完了」と報告してよいのは次をすべて満たすときだけである。

- [ ] 対象変更が `SPEC.md` の現行フェーズに含まれる
- [ ] Graphify で影響経路を確認した
- [ ] Serena で定義と参照を確認し、シンボル編集した（新規ファイル作成はその限りでないが、作成後に overview を取る）
- [ ] 新規または更新したテストが緑
- [ ] Kernel を触ったなら「全 Kernel メソッドが NativeMethod」テストが緑
- [ ] グラフを `--update` した
- [ ] 頼まれていないファイルを増やしていない

---

## ユーザーの作業指示フォーマットへの応答

ユーザーは「結論 → 前提 → 範囲 → 仕様 → TDD → 制約 → 受け入れ」で仕様を書く。エージェントの計画もこの順で短く返す。長文の再説明をしない。`SPEC.md` にあることを繰り返さない。

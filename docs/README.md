# Ao 実装文書

GitHub 向けの案内（英語が正本）: [`README.md`](../README.md) / [`README.ja.md`](../README.ja.md)。

製品の正本は [`SPEC.md`](../SPEC.md)。エージェント手順の正本は [`CLAUDE.md`](../CLAUDE.md)。このディレクトリは **フェーズと PR の実行文書** である。SPEC の本文は複製しない。

## 使い方

1. ルートの `PHASE` を読む。無いときは P0。
2. 対応する `docs/phases/P*.md` を読む。
3. そのフェーズの PR を `docs/prs/` から一つずつ消化する。
4. 受け入れが緑、Kernel 走査があるフェーズではそれが緑、Graphify 更新済み、それから `PHASE` を進める。
5. 赤のテストを残したまま次フェーズへ行かない。
6. コード・SPEC・docs を変えたセッション末に `graphify update .`。

## フェーズ

| フェーズ | 文書 | 状態 |
|---|---|---|
| P0 骨格 | [phases/P0.md](phases/P0.md) | done |
| P1 オブジェクトメモリ | [phases/P1.md](phases/P1.md) | done |
| P2 ブートストラップ | [phases/P2.md](phases/P2.md) | done |
| P3 ネイティブディスパッチ | [phases/P3.md](phases/P3.md) | done |
| P4 Kernel ネイティブ | [phases/P4.md](phases/P4.md) | done |
| P5 コンパイラ | [phases/P5.md](phases/P5.md) | done |
| P6 インタプリタ | [phases/P6.md](phases/P6.md) | planned |
| P6b vendor file-in | [phases/P6b.md](phases/P6b.md) | planned |
| P7 イメージ | [phases/P7.md](phases/P7.md) | done |
| P8 AppKit ツール | [phases/P8.md](phases/P8.md) | planned |
| P9 統合 | [phases/P9.md](phases/P9.md) | planned |

依存は直線: P0 → P1 → P2 → P3 → P4 → P5 → P6 → P6b → P7 → P8 → P9。

## PR

| ID | 題 | 親 |
|---|---|---|
| [P0-01](prs/P0-01.md) | git / LICENSE / gitignore / PHASE / README | P0 |
| [P0-02](prs/P0-02.md) | ディレクトリ骨格 | P0 |
| [P0-03](prs/P0-03.md) | CMake runtime/compiler/CLI + GoogleTest | P0 |
| [P0-04](prs/P0-04.md) | C ABI + Swift 空テスト | P0 |
| [P0-05](prs/P0-05.md) | docs/phases と docs/prs | P0 |
| [P0-06](prs/P0-06.md) | Serena プロジェクト | P0 |
| [P0-07](prs/P0-07.md) | Graphify 初期グラフ | P0 |
| [P1-01](prs/P1-01.md) | `ao::Oop` タグ | P1 |
| [P1-02](prs/P1-02.md) | オブジェクトヘッダと bump 割り当て | P1 |
| [P1-03](prs/P1-03.md) | nursery GC | P1 |
| [P1-04](prs/P1-04.md) | old 世代 mark-compact と昇格 | P1 |
| [P1-05](prs/P1-05.md) | GC ルート API | P1 |
| [P1-06](prs/P1-06.md) | weak slot GC | P1 |
| [P1-07](prs/P1-07.md) | immovable old objects | P1 |
| [P2-01](prs/P2-01.md) | WellKnown と即値クラス | P2 |
| [P2-02](prs/P2-02.md) | クラス骨格の割り当て | P2 |
| [P2-03](prs/P2-03.md) | メタクラス循環（Blue Book 6–10） | P2 |
| [P2-04](prs/P2-04.md) | Smalltalk グローバル辞書 | P2 |
| [P3-01](prs/P3-01.md) | Symbol intern | P3 |
| [P3-02](prs/P3-02.md) | MethodDictionary | P3 |
| [P3-03](prs/P3-03.md) | NativeMethod とセレクタマングル | P3 |
| [P3-04](prs/P3-04.md) | lookup / super / doesNotUnderstand: | P3 |
| [P3-05](prs/P3-05.md) | インラインキャッシュとクラスキャッシュ | P3 |
| [P4-01](prs/P4-01.md) | Object / UndefinedObject / Boolean | P4 |
| [P4-02](prs/P4-02.md) | Behavior / ClassDescription / Class / Metaclass | P4 |
| [P4-03](prs/P4-03.md) | Magnitude / SmallInteger / Character | P4 |
| [P4-04](prs/P4-04.md) | Array / ByteArray / String / Symbol | P4 |
| [P4-05](prs/P4-05.md) | Dictionary / Set / OrderedCollection | P4 |
| [P4-06](prs/P4-06.md) | Stream / Transcript モデル | P4 |
| [P4-07](prs/P4-07.md) | Process / ProcessorScheduler / Semaphore | P4 |
| [P4-08](prs/P4-08.md) | Point / Rectangle | P4 |
| [P4-09](prs/P4-09.md) | Kernel NativeMethod 走査と bench | P4 |
| [P5-01](prs/P5-01.md) | 字句解析 | P5 |
| [P5-02](prs/P5-02.md) | 構文解析と AST | P5 |
| [P5-03](prs/P5-03.md) | Ao バイトコード ISA | P5 |
| [P5-04](prs/P5-04.md) | コード生成 CompiledMethod | P5 |
| [P5-05](prs/P5-05.md) | チャンク file-in | P5 |
| [P6-01](prs/P6-01.md) | MethodContext / BlockContext | P6 |
| [P6-02](prs/P6-02.md) | インタプリタループ | P6 |
| [P6-03](prs/P6-03.md) | ブロックと `ao --test` | P6 |
| [P6b-01](prs/P6b-01.md) | Cuis pin と ORIGIN.md | P6b |
| [P6b-02](prs/P6b-02.md) | LOAD_ORDER と Kernel 上書き禁止 | P6b |
| [P6b-03](prs/P6b-03.md) | host 移設パッチ | P6b |
| [P6b-04](prs/P6b-04.md) | vendor file-in | P6b |
| [P7-01](prs/P7-01.md) | `.aoimage` 形式 | P7 |
| [P7-02](prs/P7-02.md) | save | P7 |
| [P7-03](prs/P7-03.md) | load と NativeMethod 再結合 | P7 |
| [P8-01](prs/P8-01.md) | Ao.app 骨格 | P8 |
| [P8-02](prs/P8-02.md) | Transcript ウィンドウ | P8 |
| [P8-03](prs/P8-03.md) | Workspace ウィンドウ | P8 |
| [P8-04](prs/P8-04.md) | System Browser 5 ペイン | P8 |
| [P8-05](prs/P8-05.md) | メニューとキー | P8 |
| [P9-01](prs/P9-01.md) | Do it / Print it / Inspect it | P9 |
| [P9-02](prs/P9-02.md) | Browser accept | P9 |
| [P9-03](prs/P9-03.md) | エラー表示と VoiceOver | P9 |
| [P9-04](prs/P9-04.md) | v1 ゴールデン受け入れ | P9 |

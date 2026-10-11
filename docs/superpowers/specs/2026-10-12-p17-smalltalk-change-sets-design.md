# P17 — Smalltalk のチェンジセット（設計）

## 結論

P16 のチェンジセットを、C++ のセッションメモリから Smalltalk のクラス `ChangeSet` に移す。`image/Tools-model/ChangeSet.st` に Smalltalk のソースで書き、boot で vendor のあとに file-in する。Accept・削除・File In… の成功のあと、C++ が `ChangeSet` に記録のメッセージを送る。チェンジセットはヒープのオブジェクトなので、イメージに保存される。Kernel は C++ のネイティブのままで、足すのは `Behavior>>sourceCodeAt:` と `Class>>definition` の 2 つのネイティブだけである。正本は実装前に更新する SPEC（§2.3 に P17、§2.4、§3.7、§3.9、§3.10、§3.11、§3.12、§4、§6）である。本書は設計判断の理由を残す。

## 前提

- P16（PR #28、1.5.0 未リリース）の上に積む。PHASE は `P16`。
- P16 の SPEC §3.9 は「Smalltalk 側に `ChangeSet` クラスを作らない」と決め、チェンジセットを C++ のデータ（`Session::changes`）だけで持つ。`ao_image_save` は書かず、boot とロードは空の `Unnamed1` から始める。
- SPEC §3.12 はツールモデルの自作を禁止する。一方、ディレクトリ契約（SPEC の `image/Tools-model/`）は「ホスト GUI が使うモデルだけ、必要なら自前」で、食い違っている。`image/Tools-model/` は空（`.gitkeep` のみ）。
- CLAUDE.md の設計指針は「モデル状態だけ Smalltalk、ビューは AppKit」。P16 はこれから外れていた。
- 現状の事実（調査 2026-10-12）:
  - boot 時の非 Kernel の file-in は vendor の LOAD_ORDER だけ（アプリ `fileInVendor`、CLI `--load-order`）。`ao --test` は vendor を読まない。file-in は式のチャンクを実行しない。
  - C++ から Smalltalk への送信は `ao::send`（`Send.hpp`）。最外の送信は `clearUnwinding` / `refreshStackLimit` のあとに送り、`unwinding` と abort を読んで消す（`printStringOf`、`installEmptyWorkspace` が前例）。`acceptInto` はこの手当てをしていない。
  - Smalltalk からメソッドのソースを読むネイティブは無い。ソース表は C++ からだけ読める。ファイルを書くネイティブも無い。
  - `ao_image_save` はヒープ全体を書く。新しいクラスとクラス変数は保存され、イメージの版（3）は上げなくてよい。

## 利用者との合意（2026-10-12）

| 論点 | 決定 |
|---|---|
| 動機 | Smalltalk 側で定義できなければ Smalltalk ではない。チェンジセットを Smalltalk のクラスにする |
| 範囲 | チェンジセットだけ（A 案）。Kernel は C++ のまま |
| 保存 | チェンジセットをイメージに含める |
| 方針 | SPEC §3.12 に `image/Tools-model/` の狭い例外を足して自作する（vendor の Cuis ChangeSet は取り込まない） |
| フェーズ | P17 として P16 の上に積む。版 1.6.0 |
| 記録の流れ | 成功のあと C++ から Smalltalk に通知する（A 案） |
| 評価中の読み出し | 拒む |

## 方式

**採用: 成功のあと C++ から `ChangeSet` に通知する。**

1. Accept、削除、File In…、`ao_accept_class` が成功する（P16 の `note*` の呼び出し位置と同じ）。
2. C++ は最外の送信の手当て（`clearUnwinding`、`refreshStackLimit`、ライブデバッガの停止を無効にする）をして、`ChangeSet` のクラス側の記録メソッドを送る。
3. 送信が失敗しても（`ChangeSet` が無い、DNU、abort）無視し、Accept と削除の答えは変えない。

**不採用: C++ に出来事を溜め、Smalltalk が次に読むときに取り込む。** 記録が遅れて見え、C++ と Smalltalk に二重の状態ができる。

**不採用: Accept そのものを Smalltalk に移す（`Behavior>>compile:` など）。** Browser・Debugger・File In… の Accept の経路を作り直すことになり、1 フェーズに収まらない。

**不採用: 中身を C++ に残し、Smalltalk に薄い窓口だけ置く。** 実体が C++ のままで、動機に応えない。

**不採用: vendor（Cuis）の `ChangeSet` を取り込む。** ChangeRecord、SystemChangeNotifier、スタンプ、FileStream などへの依存が広く、パッチと除外が大きくなる見込み（未精査）。

## 設計判断

### 置き場と読み込み

- `image/Tools-model/ChangeSet.st`（チャンク形式）と `image/Tools-model/LOAD_ORDER` を置く。カテゴリは `Tools-Changes`。
- boot のあと、vendor の LOAD_ORDER に続けて Tools-model の LOAD_ORDER を file-in する（`FileInMode::Plain`。ソースも記録も無い）。経路はアプリ（`fileInVendor` の後）、CLI（`--load-order` に Tools-model を足すか、別の指定）、`ao --test`（Tools-model を先に file-in してからテストを走らせる）。CLI の形は計画で決める。
- 保存したイメージにはクラスごと入っているので、Open Image… では読み直さない。ただし `ChangeSet` の無いイメージ（1.5.0 以前の保存、ユーザーが消したもの）を開いたら、アプリが Tools-model を file-in する。

### Smalltalk 側のクラス

- `ChangeSet`（1 つのチェンジセット）
  - インスタンス変数: `name`（String）、`id`（SmallInteger）、`classChanges`（`ClassChanges` の OrderedCollection。最初に記録した順）。
  - クラス変数: `AllChangeSets`（作った順の OrderedCollection）、`Current`、`NextId`。
- `ClassChanges`（1 クラス分の記録）
  - インスタンス変数: `className`（Symbol）、`definitionKind`（nil、`#added`、`#changed`、`#removed`）、`instanceSide`、`classSide`（セレクタ → 種類 の Dictionary）。
- 種類は Symbol `#added`、`#changed`、`#removed`。ABI の `AO_CHANGE_*` との対応は C++ の薄い層で行う。
- 初期化は遅延である。file-in は式のチャンクを実行しないので、`AllChangeSets` が nil なら最初の参照で空の `Unnamed1` を作って current にする。boot 直後は空の `Unnamed1`、保存したイメージを開くと保存時のセットが戻る。
- ID は `NextId` から振り、同じイメージの中で使い回さない。

### クラス側・インスタンス側の入口

- 参照: `ChangeSet current`、`ChangeSet allChangeSets`、`ChangeSet named: aString`（無ければ nil）、`ChangeSet withId: anInteger`（無ければ nil）。
- 作成と切り替え: `ChangeSet newNamed: aString`（新しいセットを current にして答える）、`aChangeSet makeCurrent`。
- 記録（C++ から送る。P16 の `note*` に対応）:
  - `ChangeSet noteMethodChange: className selector: aSymbol meta: aBoolean existed: aBoolean`
  - `ChangeSet noteClassChange: className existed: aBoolean`
  - `ChangeSet noteMethodRemoval: className selector: aSymbol meta: aBoolean`
  - `ChangeSet noteClassRemoval: className`
  - 種類の合成は P16 の SPEC §3.10 の規則のまま（追加のあとの変更は追加、追加のあとの削除は項目ごと消える、など）。
- 操作: `removeClass:`、`removeSelector:class:meta:`、`moveClass:to:`、`moveSelector:class:meta:to:`。
- File Out の本文: `aChangeSet fileOutString`。P16 の SPEC §3.9 の書式（スーパークラス順、削除のコメント、ソースや定義の無いもののコメント、`!` の `!!`、コメントのチャンクの区切り）をそのまま Smalltalk で書く。

### 拒否の理由は 1 か所

- 拒否の理由を答えるメソッドを Smalltalk 側に置く。成功なら nil、拒否なら文言の String を答える（例 `ChangeSet reasonToCreate: aString`、`aChangeSet reasonToRemoveClass: className`、`aChangeSet reasonToMove…`）。
- 文言は P16 のまま: `change set name empty`、`change set name taken: <name>`、`not in change set: <Class>`、`not in change set: <Class>>><selector>`、`same change set`。
- ワークスペース向けの `newNamed:` などは、理由があれば `self error:` で知らせる。
- C++ は先に理由を聞き、nil なら実行する。ABI からの呼び出しでエラーやデバッガの停止を起こさない。

### ネイティブ（Kernel、C++）

- `Behavior>>sourceCodeAt: aSelector`: ソース表にそのクラス（メタクラスはクラス側）のそのセレクタのソースがあれば String、無ければ nil。
- `Class>>definition`: クラス定義のチャンクの本文（P16 の `classDefinitionText`）を String で答える。
- どちらも NativeMethod。`docs/native-selectors.md` と `image/Kernel/*.st` のプロトコルに書く。`KernelScan.*` は緑のまま。
- ファイルを書くネイティブは足さない（FileStream はクラスライブラリで、自作の禁止の範囲）。書き出しは ABI が行う。

### C ABI

- `ao_changes_*` の 12 関数はシグネチャを変えない。中身を `ChangeSet` への送信に置き換える。Swift はほぼそのまま。
- 読み出し（`ao_changes_count`、`ao_changes_at`、`ao_changes_current`、`ao_changes_class_count`、`ao_changes_class_at`、`ao_changes_method_count`、`ao_changes_method_at`）は busy なら拒む: `*_count` は -1、`*_at` は `AO_ERR`、`current` は 0。P16 の「busy でも読める」から変わる。
- 書き込み（`ao_changes_new`、`ao_changes_set_current`、`ao_changes_drop`、`ao_changes_move`、`ao_changes_file_out`）と `ao_filein_file` は P16 と同じく最外の入口で、busy なら `runtime is busy`。
- `ao_changes_file_out` は `fileOutString` を送り、答えの String を C++ がファイルに書く（`cannot write file: <path>` は P16 のまま）。
- ABI からの送信が失敗したとき（DNU、abort）: 記録は捨てる、読み出しは busy と同じ答え、書き込みは `change set failed`。
- `ChangeSet` が無いとき: 記録は何もしない、読み出しは「セット 0 個、current 0」、書き込みは `change sets unavailable`。

### C++ から消すもの

- `Session::changes`、種類の規則、`fileOutText` とその補助。`runtime/src/ChangeSet.cpp` は、送信の手当てと ABI の変換をする薄い層にする。
- `note*` の呼び出し位置（`acceptInto`、`applyMethodsFor`、`applyChunkActions`、`removeMethodOf`、`removeClassOf`）は残し、中身を送信に変える。

### Change Sorter（Swift）

- 窓の作りと操作は P16 のまま。
- busy で読み出しを拒まれたら、前の内容を残してエラー欄に `runtime is busy` を出す。評価が終わり、窓がキーになったときに読み直す。
- Open Image… のあとは、そのイメージに保存されていたセットを表示する（P16 の「`Unnamed1` からやり直す」は、イメージにセットが無いときだけ）。

### SPEC の変更点

- §2.3: P17 の行。§2.4: P17 は 1.6.0。
- §3.7 手順 9: vendor のあとに `image/Tools-model/` を file-in する。
- §3.9: 「Smalltalk 側に `ChangeSet` クラスを作らない」を撤回。モデルは Smalltalk の `ChangeSet`、窓は AppKit。busy の表示とイメージを開いたあとの表示。
- §3.10「チェンジセット」: セッションのメモリから Smalltalk のオブジェクトへ。busy の読み出しの拒否、送信の失敗、`change sets unavailable`。足したネイティブ 2 つ。
- §3.11: チェンジセットはイメージに保存される（版は 3 のまま）。
- §3.12: 「自作してよいもの」に「`image/Tools-model/` の、Ao のツールが使うモデル（チェンジセット）」を足す。ディレクトリ契約の記述と揃える。
- §4: テスト名。§6: P17 のチェックリスト。`docs/phases/P17.md`、`docs/native-selectors.md`。

## テスト

- Smalltalk（`image/tests/changeset.st`、`ao --test`）: 種類の合成、`newNamed:` の拒否、`reasonTo…` の文言、Remove と Move、`fileOutString` の書式、遅延の初期化。
- C++（GoogleTest）:
  - P16 の `ChangesAbi.*` を新しい実装の仕様として残す。busy の読み出しのテストだけ「拒む」に変える。
  - `sourceCodeAt:` と `definition` のネイティブ。
  - 保存して開き直すとセットと current と ID が戻る。
  - `ChangeSet` が無いとき・壊れているとき（メソッドを消した、DNU）に、Accept と削除の答えが変わらず、ABI が決まった答えを返す。
  - ABI からの送信でライブデバッガが止まらない。
  - `KernelScan.*` が緑のまま。
- Swift: `ChangeSorterTests` を残し、busy の表示と、イメージを開いたあとの保存済みセットの表示を足す。

## やらないこと

- Kernel の Smalltalk 化。Accept の Smalltalk 化。
- FileStream などのファイル I/O のクラス。Smalltalk からのファイル書き込み。
- Browser など、ほかのツールのモデルの移行。
- セットの改名と削除、メソッドのスタンプと作者、Workspace の評価の記録（P16 と同じく対象外）。
- イメージの版の変更。

## 計画で詰めること

- CLI で Tools-model を読む形（`--load-order` を複数取るか、vendor の LOAD_ORDER から参照するか）。
- セレクタをバイト順に並べる方法（既存の String の比較のネイティブで足りるか）。
- 最外の送信の手当てを、`acceptInto` などの内側から安全に呼べるか（`sessionDebugAccept` の経路、止まったプロセスがあるとき）。

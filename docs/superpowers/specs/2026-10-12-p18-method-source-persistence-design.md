# P18 — メソッドソースの永続化（設計）

## 結論

ソース表（`Session::methodSources`）のソースを `.aoimage` に保存し、ロードで戻す。ヒープとレコードの後ろに「ソース節」を足し、形式の版を 3 から 4 に上げる。ソースは今までどおりセッションの C++ の表だけが持ち、Smalltalk のヒープには入れない。戻すのはソースだけで、ブロックとデバッグ情報は戻さない。これで P17 の制限（イメージから戻したセットのメソッドにソースが無い）が消える。正本は実装前に更新する SPEC（§2.3 に P18、§2.4、§3.9、§3.10、§3.11、§6）である。本書は設計判断の理由を残す。

## 前提

- P17（PR #29、1.6.0 未リリース）の上に積む。PHASE は `P17`。
- SPEC §3.10「ソースはイメージに書かない」: ソースはセッションのルート表だけが持ち、`ao_runtime_boot` と `ao_image_load` は表を空にする。§3.9 の P17 の制限はこの帰結である。
- 現状の事実（調査 2026-10-12）:
  - ヘッダは 48 バイト（`DiskHeader`、`runtime/src/ImageFormat.cpp`）。`reserved16`、`reserved32a`、`reserved32b`、`reserved64` は 0 でなければ `damaged image`。版は `kImageVersion = 3`（`runtime/include/ao/ImageFormat.hpp`）。
  - ファイルはヘッダ、ヒープ、レコード 184 件で終わる。`checkFile` は読み終えた位置がファイルの末尾であることを求める。`headerOk` はヘッダだけでファイルの大きさの範囲を確かめる。
  - 保存のオフセット表（`Trace`）は `Image::save` の中だけにある。`encodeOop` は、ダンプに入らないオブジェクトに false を返す。`sessionImageSave` はソース表と doIt の項目のルートを外してから保存するので、テキストの String はダンプに入らない。
  - 保存は書く前に、ロードと同じ `Image::check` をバイト列に当てる。
  - `sessionImageLoad` は新しいセッションに `Image::load` し、探針のあと `g_session` を差し替え、`clearMethodSources()` を呼ぶ。失敗したロードは差し替えの前に返るので、セッションに触れない。
  - `rememberMethodSource(method, text, Oop{}, nullptr)` は、ブロックもデバッグ情報も持たない項目を入れる。引くときは `findEntry` が method の同一性で探す。
  - CLI の `ao image save` と `ao image load` はセッションを持たない。

## 利用者との合意（2026-10-12）

| 論点 | 決定 |
|---|---|
| P18 の中身 | メソッドソースの永続化 |
| 戻す範囲 | ソースだけ。ブロックとデバッグ情報は戻さない |
| 方式 | A 案: `.aoimage` にソース節を足し、版 4 にする |
| 版 3 | ソース節の無いイメージとして読む |
| リリース | マイナー版 1.7.0 |

## 方式

**採用（A）: `.aoimage` の末尾にソース節を足す。** 変更は保存とロードとその検査に閉じる。ソースが Smalltalk から見えない C++ の表にあるという構造を変えない。CompiledMethod の形とコンパイラは変えない。

**不採用（B）: CompiledMethod にソースのスロットを足す。** 保存とロードは何もしなくて済むが、`kCmSlotCount`、コンパイラ、エミッタ、ネイティブ、ロードの検査、形の変更での移し替えに手が入る。「ソースはイメージに書かない」を前提にした箇所をすべて洗い直すことになる。

**不採用（C）: `.sources` / `.changes` 相当の別ファイル。** イメージと別ファイルを一緒に、原子的に保存する必要があり、片方だけ壊れたり移されたりした場合も扱わなければならない。1 ファイルを rename で置き換える今の保存と相性が悪い。

## 形式（版 4）

- ヘッダの `reserved64` を `sourcesBytes`（ソース節のバイト数）にする。版 4 では 8 以上で 8 の倍数、版 3 では 0 である。`headerOk` は、ファイルの大きさが `48 + heapBytes + レコード + sourcesBytes` の範囲にあることを、ヘッダだけで確かめる。
- ソース節はレコードの直後に置く。

```
u32 count
u32 0                     （詰め物）
count 回:
  u64 method              ヒープ内オフセット（ポインタの符号化と同じ）
  u32 length              ソースのバイト数
  u32 0                   （詰め物）
  length バイト            ソース（UTF-8）
  0 で 8 バイト境界まで詰める
```

- 検査（ロードと、書く前の保存が同じ関数で行う）。どれかに反すれば `damaged image`。
  - `method` はヒープ内のオブジェクトの先頭を指し、その klass は CompiledMethod である。
  - 同じ `method` が 2 回出ない。
  - 長さが節からはみ出さない。詰め物はすべて 0 である。
  - `count` 件を読み終えた位置が、節の末尾（ファイルの末尾）と一致する。
- 版 3 はソース節の無いイメージとして読む。版 1 と版 2 は今までどおり拒否する。ほかの版は `unsupported image version <版>`。保存は常に版 4 で書く。1.6.0 以前のランタイムは版 4 を `unsupported image version 4` で拒否する（CHANGELOG に書く）。

## 保存

- 書く項目は、`Session::methodSources` の項目のうち、`method` がダンプに入り（`encodeOop` が成功する）、`text` が String のものである。表の順に書く。
- 書かないもの:
  - doIt の項目（`doItDebug`、`heldDoIts`）。doIt のテキストはファイルに入らない（`ImageSaveDoesNotTraceSnapshotOrBlockSlots`）。
  - どこからも届かないメソッドの項目。メソッドがダンプに入らないので、指す先が無い。
  - ブロックとデバッグ情報。
- `Image::save` に「書くソースの並び（method の Oop と UTF-8 のテキスト）」の引数を足す。`sessionImageSave` が、ルートを外す前にソース表から作って渡す。保存は GC を起こさないので、並びの Oop は動かない（計画で確かめる）。
- ソースは String としてダンプに入れず、ソース節にバイト列として書く。
- CLI の `ao image save` はセッションを持たないので、`count = 0` のソース節を書く。
- 並びを作るメモリが無ければ保存を失敗させる（`ao_image_save` は `AO_ERR`）。ソースを黙って落としたイメージを作らない。

## ロード

- `Image::load` はソース節を検査して解釈し、再配置したあとの「(CompiledMethod の Oop, UTF-8 のテキスト)」の並びを出力引数で返す。版 3 では空である。
- `sessionImageLoad` は、`g_session` を差し替えて `clearMethodSources()` を呼んだあと、並びの順にテキストから String を作り、`rememberMethodSource(method, text, Oop{}, nullptr)` で項目を入れる。String を作る間、並びの Oop をルートに置く（作るときに GC が起こりうる）。
- 作り直しの途中でメモリが無くなったら、そこで止める。残りのメソッドはプレースホルダになり、ロードは成功を返す。ヒープはもう新しいセッションに移っていて、失敗にすると実態と食い違う。
- 壊れたソース節は、ヒープを載せる前の検査で `damaged image` として拒否する。拒否したロードはセッションに触れない。
- CLI の `ao image load` はセッションを持たないので、検査だけして捨てる。

## 振る舞いの変化

- `ao_browser_source` と `Behavior>>sourceCodeAt:` は、ロードしたイメージのメソッドでも、保存したときにソースがあればそれを答える。
- Change Sorter のソース枠と File Out にソースが戻る（P17 の制限が消える）。
- 形の変更（§3.9）は、ロードしたメソッドもソース表のソースでコンパイルし直して移せる。
- 変わらないもの: ロードしたメソッドの Debugger はソース位置と temp 名を出さない。Debugger の Accept はできる（自分の項目を持つ、インストール済みのメソッドと同じ。P15）。Accept し直したメソッドは完全な項目を持つ。vendor と `ao filein` で入れたメソッドは、もともとソースを持たない。

## テスト

先に書いて、赤を確かめる。

- `image_format_test`: 版 4 を受け付け、版 3 も読め、版 5 を `unsupported image version 5` で拒否する。`kImageVersion == 4`。版 3 の `reserved64` が 0 でなければ `damaged image`。
- `image_save_load_test`:
  - Accept したメソッドのソースが保存とロードで戻る（ASCII と日本語の UTF-8）。
  - doIt のテキストはファイルに入らない。
  - ソース節の壊れ方（CompiledMethod でない、ヒープの外、重複、長さのはみ出し、詰め物が 0 でない、末尾の食い違い、`sourcesBytes` とファイルの大きさの食い違い）をそれぞれ `damaged image` で拒否する。
  - 既存の手作りヘッダのテスト（`oopWords`、`RefusesExtraRecords`、`RefusesVersionOneWithReason`、`RefusesHugeHeapBytesFromTheHeader`、`ImageSurgery`）を版 4 とソース節に合わせる。
- `accept_abi_test`: `ClassDefinitionThenImageDropsSourceText` を「ロードのあともソースが戻る」に反転する。
- `changes_abi_test`: 戻ったセットの File Out にメソッドのソースが入り、`source not available` が出ない。
- `browser_abi_test`: ロードしたメソッドの `ao_browser_source` が `AO_OK`。
- `ChangeSorterTests`（XCTest）: Open Image… のあと、ソース枠にソースが出る。
- `cli_test.sh`: `ao image save` から `ao image load` が版 4 で往復する。

## 範囲外

- ブロックとデバッグ情報を戻すこと（ロードしたメソッドの Debugger）。
- `ao filein` と vendor のメソッドのソース。
- 別ファイルのソース（C 案）。

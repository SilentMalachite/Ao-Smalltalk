# 07 Swift アプリ・ビルド・スクリプト
対象: app/Package.swift, app/CAo/module.modulemap, app/AoApp/main.swift, app/Ao/{AoApp,BrowserModel,BrowserWindow,InspectorWindow,LaunchSet,MainMenu,TranscriptWindow,WorkspaceWindow}.swift, app/AoTests/*.swift, bridge/ao_abi.h（Swift 側の使い方）, CMakeLists.txt, runtime/CMakeLists.txt, compiler/CMakeLists.txt, scripts/{build,test,package-app}.sh, .gitignore
検証: コード読解に加え、scratch（`/private/tmp/.../scratchpad/r07/`）で次のプローブを実行した。リポジトリの build/ と app/.build には触れていない（静的ライブラリはコピーして使用）。
- probe1/2: `NSTextView` の既定値を確認し、`keyDown` で `x 'a' --` を打鍵。さらに Transcript と同じ「全文再設定＋フォント＋スクロール」を 2000 回と 8000 回繰り返して時間を計測
- probe3: SwiftPM が `-Xlinker -force_load` の静的ライブラリ更新で再リンクするかを確認（再リンクした。指摘なし）
- repo コピー: `swift build --build-tests -strict-concurrency=complete` を実行（Swift の警告 0。ld の警告は 94 件）。`package-app.sh` を実行し `codesign --verify --strict` と `otool` で確認
- probe4: C から `ao_eval` を呼び、出力内の NUL、64 KiB 超過、`AoSpan` のオフセット単位を確認

## 概要
| 重大度 | 件数 |
|---|---|
| Critical | 0 |
| High | 1 |
| Medium | 5 |
| Low | 7 |

Swift 6 の厳格な並行性チェックでは警告が出なかった。GUI 型はすべて `@MainActor` で、C への呼び出しもすべてメインスレッドで行われる。フックは `MainActor.assumeIsolated` で受けており、ランタイムにスレッドは無い。新しい強制アンラップも無く、AppKit のみを使い、ランタイムは `bridge/ao_abi.h` 経由でだけ呼んでいる。ソース長 `source.utf8.count` と `withCString` の組み合わせ、NSRange と String.Index の変換（`Range(_:in:)`、挿入位置の UTF-16 長）も正しい。

## 指摘

### [High] Workspace と Browser のソース欄でスマート引用符・ダッシュ置換が有効なまま
- 場所: `app/Ao/TranscriptWindow.swift:54-66`（`makeToolTextWindow`。Workspace と Inspector も共用）、`app/Ao/BrowserWindow.swift:385-400`（`makeSource`）
- 問題: プログラムから生成した `NSTextView` の既定値は `isAutomaticQuoteSubstitutionEnabled` / `isAutomaticDashSubstitutionEnabled` / `isAutomaticTextReplacementEnabled` / `isAutomaticSpellingCorrectionEnabled` がすべて true である。コードはどれも無効にしていない。
- 失敗シナリオ: Workspace で `Transcript show: 'hello'` と打つと `‘hello’`（U+2018/U+2019）になり、Do it が compile error になる。Browser でクラス定義の `instanceVariableNames: ''` を打っても同じで、`--` は em dash（U+2014）に、識別子は綴り自動修正で別の語に化ける。受け入れ基準の「Browser で foo を追加し Workspace で評価」を手で打つと失敗する。テストは `replaceText` で文字列を直接流し込むため、この経路を通らない。
- 根拠: プローブで再現した。`keyDown` で `x 'a' --` を打鍵すると結果は `x ‘a’ — ` だった（確度: 高）。
- 修正の方向: 3 か所のテキストビューで引用符・ダッシュ・テキスト置換・綴り自動修正・`smartInsertDeleteEnabled` を false にする。打鍵経路のテスト（`keyDown` か `insertText` に `'` を送る）を加える。

### [Medium] Undo が無く、Browser で編集中のソースが無警告で捨てられる
- 場所: `app/Ao/TranscriptWindow.swift:54-66`、`app/Ao/BrowserWindow.swift:385-400`（`allowsUndo` 未設定）、`app/Ao/MainMenu.swift:33-38`（Edit に Undo/Redo が無い）、`app/Ao/BrowserWindow.swift:290`、`app/Ao/WorkspaceWindow.swift:324`
- 問題: `allowsUndo` の既定は false で（プローブで確認）、Edit メニューにも ⌘Z が無い。Browser では選択を変えるたびに `reloadLists` が `sourceView.string = model.source` で本文を置き換える。Print it も `textView.string` を丸ごと差し替える。
- 失敗シナリオ: Browser でメソッドを書き換え、Accept 前にセレクタ一覧を誤クリックすると、編集内容が確認なしに消え、元に戻す手段も無い。Workspace で誤って削除した場合も復旧できない。
- 根拠: プローブ（`allowsUndo false`）とコード読解（確度: 高）。
- 修正の方向: `allowsUndo = true` にし、Edit メニューに `undo:` / `redo:` を追加する。Print it は `insertText(_:replacementRange:)` か `shouldChangeText` と `didChangeText` を経由させる。Browser は未 Accept の変更があるとき選択変更前に確認を出す。

### [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る
- 場所: `app/Ao/BrowserWindow.swift:203-211`、`256-274`
- 問題: Accept が成功すると `publish()` を呼ぶが、`protocolName` と `selectorName` は Accept 前のままである。新しいメソッドは SPEC どおり `user` プロトコルに入るので、表示中の `native` 一覧には出てこない。
- 失敗シナリオ: 初期表示（Object / native / printString）で本文を `foo ^1` に書き換えて Accept する → `foo` は登録されるが、ソース欄は `printString` のネイティブ本文に戻り、`foo` はどの一覧でも選択されない。書いたコードが消えたように見え、SPEC §6 の受け入れ手順そのものが分かりにくい。
- 根拠: コード読解（確度: 高）。`testAcceptFooThenPrintItInsertsOneAndFailedAcceptKeepsText` は Accept 後のソース欄を検査していない。
- 修正の方向: Accept が成功したら、受け入れたセレクタ（ソース先頭から取るか ABI が返す）と `user` プロトコルを選択してから `publish()` する。

### [Medium] Transcript は出力のたびに全文を置き換え、大量出力で UI が長時間止まる
- 場所: `app/Ao/TranscriptWindow.swift:105-108`、`134-139`
- 問題: フックを 1 回呼ぶたびに `textView.string = text`（全文）、`applyFont()`（全文に属性を付け直す）、`scrollRangeToVisible` を実行している。評価はメインスレッドで同期実行されるので、評価中は画面が固まる。
- 失敗シナリオ: `1 to: 10000 do: [:i | Transcript show: i printString; cr]` はフックを 2 万回呼ぶ。プローブの実測では 1 回あたり約 1.5 ms（非表示ウィンドウで 2000 回 2.95 s、8000 回 12.0 s）で、見積もりでは約 30 秒以上のビーチボールになる。
- 根拠: プローブで計測した（確度: 中。表示中のウィンドウではさらに遅くなるはず）。
- 修正の方向: `textStorage.append` で差分だけを追加する。フォントは typingAttributes と追加分の属性で与え、スクロールは評価の終わりにまとめる。

### [Medium] vendor の file-in がカレントディレクトリ頼みで、.app から起動すると読み込まれない
- 場所: `app/Ao/AoApp.swift:132-139`、`scripts/package-app.sh:12-16`
- 問題: `"image/vendor/LOAD_ORDER"` を相対パスのまま `fileExists` と `ao_filein_load_order` に渡している。`package-app.sh` は `image/` をバンドルに入れない。
- 失敗シナリオ: `open build/Ao.app` や Finder から起動すると cwd が `/` になり、vendor クラスが一切読まれない。エラー表示も無い（ファイルが無ければ黙って return する）。リポジトリのルートから `swift run` したときだけ動くので、起動方法によって中身の違うシステムになる。
- 根拠: コード読解と、生成したバンドルの構成確認（`Contents/` にあるのは `Info.plist` と `MacOS/` だけ）（確度: 高）。
- 修正の方向: `image/vendor` を `Contents/Resources` に同梱し、`Bundle.main` からの絶対パスで探す。開発時は環境変数か引数でルートを渡す。

### [Medium] 64 KiB を超える Print it / Inspect it は副作用だけ残して失敗する
- 場所: `app/Ao/WorkspaceWindow.swift:4`、`224-226`、`253-273`
- 問題: 出力バッファは 65 536 バイト固定である。ランタイムは評価を終えたあとで `AO_ERR_RANGE` を返し、必要な長さは返さない。評価には副作用があるため、アプリ側でバッファを広げて再試行することもできない。
- 失敗シナリオ: `y := 7. Array new: 20000` を Print it する → `rc=4`（AO_ERR_RANGE）で、エラー欄には「result does not fit」とだけ出る。一方で `y` は 7 に更新済み（プローブで確認）。中程度の大きさのコレクションは表示できない。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: ABI で必要長を返すか、ランタイム側の結果文字列を保持して 2 回目の呼び出しで取り出せるようにする。少なくとも切り詰めた先頭部分と「…」を挿入する。

### [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される
- 場所: `app/Ao/WorkspaceWindow.swift:266-271`（Inspector のフック経路も同じ: `20-21`）
- 問題: `ao_eval` は出力長を返さないので、Swift 側は `String(cString:)` で最初の NUL までしか読まない。
- 失敗シナリオ: `(String new: 3)` を Print it すると、ランタイムは `27 00 00 00 27` の 5 バイトを書くが、Workspace には `'` 1 文字だけが挿入される。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: ABI から書き込み長を返してもらい、`String(decoding:as:)` で長さ指定で読む。

### [Low] Inspector ウィンドウが閉じても解放されず、増え続ける
- 場所: `app/Ao/WorkspaceWindow.swift:113`、`244-251`
- 問題: 内容の違う Inspect it のたびに `InspectorWindow` を `inspectors` に追加するが、取り除く処理が無い（`isReleasedWhenClosed = false`）。
- 失敗シナリオ: 異なる式を何度も Inspect it すると、閉じたウィンドウとテキストビューがプロセス終了まで残る。再利用の対象も `inspectors.last` だけなので、A→B→A と Inspect すると A の窓が 2 枚になる。
- 根拠: コード読解（確度: 高）。
- 修正の方向: `NSWindow.willCloseNotification` で配列から外す。

### [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない
- 場所: `app/Ao/WorkspaceWindow.swift:225`、`app/Ao/BrowserWindow.swift:206`
- 問題: SPEC §3.8 は「エラーはソース区間付き」と定めているが、アプリは `message` しか使っていない。区間は UTF-8 のバイト単位で、評価したときの選択範囲の先頭からの相対位置である。たとえば `'あ' + + 1` の誤りは 8..9 だが、UTF-16 では 6 になる（プローブで確認）。
- 失敗シナリオ: 複数行を評価して「expected expression」だけが出ても、どこが悪いのか分からない。将来ハイライトを実装するとき、NSRange にそのまま渡すと日本語を含む行で位置がずれる。
- 根拠: プローブとコード読解（確度: 高）。
- 修正の方向: 選択部分の UTF-8 バイトオフセットを `String.Index` 経由で UTF-16 に換算し、選択範囲の先頭を足してから `setSelectedRange` と `showFindIndicator` で示す。換算のテストに日本語を含める。

### [Low] Workspace の評価を中断できない（無限ループでアプリが固まる）
- 場所: `app/Ao/WorkspaceWindow.swift:223`（メインスレッドで `ao_eval` を同期実行）
- 問題: 中断の仕組み（⌘. や割り込みフラグ）が無い。
- 失敗シナリオ: `[true] whileTrue` を Do it すると、強制終了するしかなく、保存していないイメージや編集中のテキストも失われる。
- 根拠: コード読解（確度: 高）。
- 修正の方向: v1 の範囲外なら SPEC に明記する。範囲内にするなら、ランタイムに割り込みフラグを持たせ、ABI にそれを立てる関数を用意する。

### [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる
- 場所: `scripts/package-app.sh:4-38`
- 問題: `codesign` を実行していない。実行ファイルはリンカによるアドホック署名のままで、Identifier は `AoApp-<hash>`、`Info.plist=not bound` になる。`swift build` は `-c release` なしの Debug で、C++ 側も既定の Debug（`CMakeLists.txt:10-12`）である。このスクリプトは `build.sh` も呼ばないので、古い `.a` を同梱することがある。
- 失敗シナリオ: `codesign --verify --strict build/Ao.app` が「code has no resources but signature indicates they must be present」で失敗する（プローブで確認）。別の Mac に渡すと Gatekeeper に拒否され、-O0 の成果物がそのまま配布物になる。
- 根拠: プローブで再現した（確度: 高）。
- 修正の方向: 最後に `codesign --force --sign - "$APP"` を実行する。Release 構成でビルドし、先に `build.sh` を呼ぶ。`CFBundleVersion` と `CFBundleShortVersionString` も追加する。

### [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない
- 場所: `CMakeLists.txt:8`（`CMAKE_OSX_DEPLOYMENT_TARGET` を指定していない）、`app/Package.swift:6`、`scripts/package-app.sh:31`
- 問題: `.a` 内のオブジェクトは macOS 27.0 向け（`minos 27.0`）でビルドされているが、Swift 側と `LSMinimumSystemVersion` は 14.0 を名乗っている。リンク時に ld が「built for newer 'macOS' version (27.0) than being linked (14.0)」を 94 件出す。
- 失敗シナリオ: macOS 14〜26 で動かしたとき、C++ 側が可用性チェックなしに新しい OS の API に依存していれば、起動時か実行時に失敗しうる。現時点で取り込んでいる libc++ のシンボルは古いものだけだった。
- 根拠: プローブで確認した（確度: 中）。
- 修正の方向: `set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)` を `project()` より前に置く。

### [Low] .gitignore に `.cache/` と `.serena/logs/` が無い
- 場所: `.gitignore:1-15`
- 問題: clangd のインデックス（`.cache/`）と Serena のログが未追跡のまま出ている（git status でも確認できる）。
- 失敗シナリオ: `git add -A` でインデックスやログがコミットされ、履歴が膨らむ。ログにはローカルのパスも含まれる。
- 根拠: git status（確度: 高）。
- 修正の方向: `/.cache/` と `.serena/logs/` を追加する。

## テストの穴
- すべての UI テストが `replaceText` で文字列を直接流し込んでおり、打鍵経路（スマート置換・Undo）を通るテストが 1 本も無い。
- 日本語・絵文字を含むテストが無い。対象にすべきは選択範囲の抽出、Print it の挿入位置、Transcript の多バイト文字、`AoSpan` のバイト→UTF-16 換算。
- `AO_ERR_RANGE`（64 KiB 超過）と、NUL を含む出力に対する Workspace の振る舞いが未検証。
- Accept 成功後のソース欄とセレクタ選択の状態を確認していない（`testAcceptFoo…` はエラー欄しか見ていない）。
- Transcript の大量出力のテストが無い（性能の退行を検出できない）。
- cwd がリポジトリのルート以外のときの起動（vendor file-in）と、`package-app.sh` の成果物（バンドル構成・`codesign --verify`）を検証するテストが無く、`test.sh` もこれらを実行しない。
- CMake に ASan/UBSan のオプションが無い。C ABI の境界（バッファ長、フックのポインタ）をサニタイザ付きで一度も走らせていない。
- テストで作ったウィンドウを閉じていない。`TranscriptWindow` のフック（`g_transcriptUser`）は、テストの終了後も解放済みのオブジェクトを指したまま残る。今は次の boot で `ctx->transcriptHook` が null になるので参照されないが、テストの順序に頼った状態である。
- 範囲外の観察（コンパイラ担当向け）: probe4 で `'a' , 'b'` が `invalid token`（span 4..5）になった。二項セレクタ `,` をスキャナが受け付けていない可能性がある。

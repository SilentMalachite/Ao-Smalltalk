# P13 — 評価の中断（設計）

## 結論

Ao.app で走っているライブ評価を、⌘. または Smalltalk → Interrupt で止め、既存のライブデバッガに渡す。止め方は P11 の `halt` と同じ経路で、理由だけ `interrupted` にする。評価はメインスレッドの同期 `ao_eval` のままにし、セーフポイントで短い RunLoop 吸い込みと中断フラグ確認を挟む。正本は実装前に更新する SPEC（§2.3 に P13、§3.10 / §3.13 / §6）である。本書は設計判断の理由を残す。

## 前提

- PHASE は `P12`。1.1.0（P10–P12）はリリース済み。SPEC §2.3 のフェーズ表は P12 で終わっている。
- P11 ライブモードでは、`halt` / 失敗で評価プロセスが止まり、`ao_eval` は `AO_ERR_HALT` を返す。Proceed / Abort / Step がある。
- SPEC §3.13 の「やらない」に「走っている評価の外からの中断」がある。P13 でこれを「評価中にホストが要求し、セーフポイントで効く中断」に置き換える。
- ABI はメインスレッドから同期で呼ぶ。ブリッジキューは置かない（§3.10）。ワーカースレッドへ評価を移す案は採らない。
- CHANGELOG / README の Known limitations に「`yield` しないループはアプリを止め、強制終了しかない」とある。これが P13 の動機である。
- インタプリタは後方ジャンプで safepoint を通る（§3.5）。P11 の step 印は命令ループ先頭の 1 分岐で見る（`docs/bench.md` で費用は揺らぎの中）。

## 利用者との合意（2026-09-30）

| 論点 | 決定 |
|---|---|
| 優先 | 使い勝手・安全性（A）と言語・ランタイム（B）のうち、中断（A）を先にする |
| 中断後 | ライブデバッガで止める（`halt` と同じ）。理由は `interrupted` |
| 入口 | ⌘. と Smalltalk → Interrupt。Escape は付けない |
| 範囲 | Ao.app だけ。CLI の `SIGINT` は後続 |
| 実装方式 | セーフポイントで RunLoop を短く回し、中断フラグを見る（評価のワーカースレッド化はしない） |
| 止める対象 | その `ao_eval` の評価プロセスだけ（P11 と同じ） |

## 方式

**採用: セッションの中断要求フラグ + セーフポイントでのホスト pump + ライブ halt。**

1. ホストが ⌘. / Interrupt で `ao_request_interrupt()` を呼ぶ。
2. 評価中、インタプリタのセーフポイントが (a) 必要ならホストの RunLoop pump を呼び、(b) フラグを見て、立っていれば評価プロセスを `halt` と同じ経路で止める。
3. `ao_eval` は `AO_ERR_HALT`、Workspace は `halted: interrupted`、Debugger は Proceed / Abort / Step。

**不採用: 評価をワーカースレッドへ移す。** UI は常に生きるが、「ABI はメインから同期」「ブリッジキュー無し」を捨て、GC・ルート・AppKit 境界の改修が大きい。

**不採用: 中断を abort のみにする。** 実装は軽いが、合意した「ライブデバッガで止める」と矛盾する。

## 設計判断

### ABI

- `ao_request_interrupt(void)` を足す。セッションが無ければ `AO_ERR`。あればフラグを立てて `AO_OK`。
- **busy 中でも呼べる。** 中断は評価の内側から効かせる入口なので、既存の「busy なら拒む」一覧には入れない。
- フラグはセッションに 1 つ。多重押しは 1 回分（溜めない）。
- 消費は「中断で halt したとき」。abort・評価の正常終了・次の `ao_eval` 入場時にもクリアし、前の評価の要求を持ち越さない。
- ライブモードでない評価（CLI 既定など）では、フラグが立っていても **止めない**。立ててよいが、今回の受け入れ対象外。

### セーフポイントと RunLoop

- フラグ確認は、既存のインタプリタ safepoint（後方ジャンプを含む）で行う。`[true] whileTrue` は反復ごとに届く。
- RunLoop を回さないとメインスレッド同期の評価中に ⌘. が届かない。runtime が AppKit に直接依存しないよう、**ホストが登録する pump フック**（関数ポインタ）を呼ぶ。未登録なら pump は何もしない（GoogleTest はフラグを直接立てて止められる）。
- pump は毎回の safepoint では呼ばない（ホットループの費用）。目安は「一定回数の safepoint ごと」または「短い壁時計間隔ごと」の、どちらか先に来た方。非ブロッキング（待ち時間 0）で、溜まったイベントを数個まで処理する。正確な定数は実装時に `docs/bench.md` のホットループと手感で決める。
- 長いネイティブに新たな safepoint は強制挿入しない。既存の safepoint があるものだけが中断に応じる。タイトなユーザーバイトコードが主対象である。

### 再入

- pump 中に届いてよいのは Interrupt（と再描画などランタイムを動かさないイベント）に限る、という **ホスト側の規律** を SPEC / 設計の前提にする。
- ランタイム側は、評価中（busy）の禁止 ABI（新たな `ao_eval`、Accept、image save/load、削除など）を今までどおり拒む。ネストした RunLoop からそれらを呼んでも `runtime is busy`。
- 中断で止まったあとは、既存どおり runtime は busy ではない（P11）。Debugger 操作と別の Do it は P11 の規則のまま。

### ホスト（Ao.app）

- Smalltalk メニューに Interrupt。キー相当は ⌘.。
- 有効化は「ライブ評価中（busy かつライブモード）」が理想。常時メニューにあっても、効くのは評価中の `ao_request_interrupt` だけでもよい。
- 中断後の UI は P11 の halt と同じ経路（ライブ Debugger、Workspace の `halted: …`）。理由文字列 `interrupted` をそのまま出す。

### Proceed のあと

- フラグは halt 時に消費済み。Proceed でループが再開しても、再要求するまで止まらない。
- 再度 ⌘. すれば、また止まる。

## 範囲

**やる:**

- SPEC に P13 を追加し、§3.13 の「やらない」から「走っている評価の外からの中断」を外して中断の規則を書く
- `ao_request_interrupt`、セッションフラグ、safepoint での確認とライブ halt
- ホストの pump フックと Ao.app の登録、⌘. / Interrupt
- GoogleTest / XCTest / 手動確認、CHANGELOG、PHASE、bench の確認

**やらない:**

- CLI `SIGINT`
- Restart、Debugger 内編集 / Accept、temp の書き換え
- ネイティブへの Step into、ネイティブ全体への safepoint 強制挿入
- 評価のワーカースレッド化、ブリッジキュー
- フォークした別プロセスや、すでに halt 中のプロセスをこの操作で新たに止めること
- Escape キー

## テスト

### runtime（GoogleTest）

- ライブモードで、タイトな後方ジャンプのループ中にフラグを立てると `AO_ERR_HALT` と理由 `interrupted`
- フラグは 1 回で消費され、Proceed 後に再要求なしでは再停止しない
- ライブモード off ではフラグがあっても評価が完走する
- 中断停止中の Save Image 拒否は P11 と同じ理由
- busy 中の `ao_request_interrupt` は成功する（拒まない）
- pump 未登録でもフラグだけで止まれる
- Kernel 走査、`ao --test image/tests`、既存デバッガ系は緑のまま
- `docs/bench.md` のホットループ比が揺らぎを超えて悪化しない

### app（XCTest）

- Interrupt メニュー項目と ⌘. のバインドがある
- 評価中に中断要求するとライブ Debugger が開き、理由に `interrupted` が含まれる（シートを出さないテスト用の差し替え、または ABI 直叩き＋UI の表示経路）
- 評価していないときの Interrupt がクラッシュや不正な halt を残さない

### 手動確認（Ao.app）

1. `[true] whileTrue` → ⌘. → Debugger、理由 `interrupted`
2. メニューの Interrupt でも同じ
3. Proceed で再開 → 再度 ⌘. で再停止
4. Abort で評価終了、Workspace が使える
5. 評価外の ⌘. で落ちない

## リスク

- **pump の再入:** ホストが評価中に Do it などを許すと、busy 拒否に頼ることになる。メニュー項目の無効化を怠るとユーザーが混乱するので、Interrupt 以外の Smalltalk 評価系は評価中グレーアウトを維持する。
- **応答遅延:** pump 間隔が長いと ⌘. から停止まで体感が遅れる。短すぎると bench が劣化する。定数は計測で決める。
- **ネイティブの長いループ:** ユーザーバイトコード以外（例: 巨大なネイティブ `do:`）は、既存 safepoint が無ければ止まらない。今回は既知の制限として SPEC に書く。
- **SPEC との衝突:** 「やらない」リストと §3.10 の再入節を同じコミットで直さないと、実装と文書が食い違う。

## SPEC への落とし込み（実装前）

1. §2.3 に P13「評価の中断」を足す。版は 1.1.0 の次のマイナー（例: 1.2.0）に含める旨を §2.4 に追記する。
2. §3.10 に `ao_request_interrupt`、フラグの寿命、busy でも呼べること、pump フックを書く。
3. §3.13 のライブデバッガに「ホストからの中断」を書き、「やらない」から外からの中断を削除する。
4. §4 / §6 に上記テストと受け入れチェックリストを足す。
5. CHANGELOG の Known limitations から「評価を中断できない」を、P13 完了時に外す。

## 受け入れ（P13 完了の定義）

- [ ] Workspace の `[true] whileTrue` を ⌘. でライブ Debugger に止められる（理由 `interrupted`）
- [ ] Interrupt メニューでも同じ
- [ ] Proceed のあと再要求なしでは再停止せず、再 ⌘. でまた止まれる
- [ ] Abort で評価が終わり、Workspace が使える
- [ ] CLI 既定と `ao --test` は変わらない（ライブ off では中断しない）
- [ ] Kernel 走査緑、`docs/bench.md` の比が悪化しない
- [ ] `PHASE` は `P13`、SPEC §6 の P13 小節がすべて `[x]`、CHANGELOG に項目

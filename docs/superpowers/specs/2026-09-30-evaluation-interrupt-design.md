# P13 — 評価の中断（設計）

## 結論

Ao.app で走っているライブ評価を、⌘. または Smalltalk → Interrupt で止め、既存のライブデバッガに渡す。止めたあとの契約は P11 と同じ（`AO_ERR_HALT`、Proceed / Abort / Step）だが、**停止の形は `Object>>halt` ではなく step / Debug it と同じ命令境界停止**である。理由文字列だけ `interrupted` にする。評価はメインスレッドの同期 `ao_eval` のままにし、インタプリタのセーフポイントで短い RunLoop 吸い込みと中断フラグ確認を挟む。正本は実装前に更新する SPEC（§2.3 に P13、§3.9 / §3.10 / §3.13 / §6 ほか）である。本書は設計判断の理由を残す。

## 前提

- PHASE は `P12`。1.1.0（P10–P12）はリリース済み。SPEC §2.3 のフェーズ表は P12 で終わっている。
- P11 ライブモードでは、評価プロセスが止まり、`ao_eval` は `AO_ERR_HALT` を返す。Proceed / Abort / Step がある。
- SPEC §3.13 の停止表では、`halt` は「止めた送信の値として nil」、step / Debug it は「止めた命令から続ける」。進行中の送信が無い停止では合成フレームを作らない。
- SPEC §3.13 の「やらない」と §3.9 Workspace に「走っている評価を外から止める手段は無い」とある。P13 でこれを改める。
- ABI はメインスレッドから同期で呼ぶ。ブリッジキューは置かない（§3.10）。ワーカースレッドへ評価を移す案は採らない。
- CHANGELOG / README の Known limitations に「`yield` しないループはアプリを止め、強制終了しかない」とある。これが P13 の動機である。
- インタプリタは後方ジャンプで safepoint を通る（§3.5）。P11 の step 印は命令ループ先頭の 1 分岐で見る（`docs/bench.md` で費用は揺らぎの中）。
- 既存フックは `ao_set_transcript_hook` / `ao_set_inspect_hook` と同型（関数ポインタ + `user`、NULL で外す、boot/load をまたいで残る）。

## 利用者との合意（2026-09-30）

| 論点 | 決定 |
|---|---|
| 優先 | 使い勝手・安全性（A）と言語・ランタイム（B）のうち、中断（A）を先にする |
| 中断後 | ライブデバッガで止める。理由は `interrupted`。UI は P11 の halt 停止と同じ見え方 |
| 停止の形 | 命令境界（step / Debug it と同形）。Proceed は止めた命令から続ける |
| 入口 | ⌘. と Smalltalk → Interrupt。Escape は付けない |
| 範囲 | Ao.app だけ。CLI の `SIGINT` は後続 |
| 実装方式 | インタプリタのセーフポイントで RunLoop を短く回し、中断フラグを見る（評価のワーカースレッド化はしない） |
| 止める対象 | ベースが待っているライブ評価プロセスだけ（P11 の「評価プロセスでないプロセスは止めない」を守る） |

## 方式

**採用: セッションの中断要求フラグ + インタプリタ safepoint でのホスト pump + ライブ停止（命令境界）。**

1. ホストが ⌘. / Interrupt で `ao_request_interrupt()` を呼ぶ。
2. ライブ評価中、**評価プロセスが**インタプリタのセーフポイントに来たら (a) 必要ならホストの RunLoop pump を呼び、(b) フラグを見て、立っていればその評価プロセスを理由 `interrupted` で止める。
3. `ao_eval` は `AO_ERR_HALT`、Workspace は `halted: interrupted`、Debugger は Proceed / Abort / Step。
4. Proceed は step と同じく、止めた命令から続ける（後方ジャンプの手前で止まっていれば、そのジャンプを再実行する）。

**不採用: 評価をワーカースレッドへ移す。** UI は常に生きるが、「ABI はメインから同期」「ブリッジキュー無し」を捨て、GC・ルート・AppKit 境界の改修が大きい。

**不採用: 中断を abort のみにする。** 実装は軽いが、合意した「ライブデバッガで止める」と矛盾する。

**不採用: `Object>>halt` と同一の送信停止にする。** safepoint は送信の途中ではなく命令境界なので、halt の Proceed（送信の値 nil）を当てはめると意味が壊れる。

## 設計判断

### §3.13 停止表への行

次の 1 行を停止表に足す。

| 起きたこと | 理由 | Proceed / Step |
|---|---|---|
| ホストの中断要求を、ベースが待っている評価プロセスがインタプリタのセーフポイントで見た | `interrupted` | できる |

- 停止の形は step / Debug it と同じ: 進行中の送信は無い（合成フレーム無し）。pc は次に実行する命令の先頭。Proceed はその命令から続ける。
- 既存の「止められないとき」規則をそのまま適用する: abort / 後始末 / abandon の最中、止まっているプロセスがすでに 8 あるとき、評価プロセスでないプロセス、スケジューラが自分のリストを更新する送信の中、では止めずに（8 件上限なら理由 `interrupted` で）abort する。step の 8 件規則と同じ扱いである。
- 「`halt` と同じ」と言ってよいのは、**ライブ停止して `AO_ERR_HALT` を返し、Workspace / Debugger の UI 経路が同じ**ことまでに限る。

### ABI: 中断要求

- `ao_request_interrupt(void)` を足す。セッションが無ければ `AO_ERR`。あればフラグを立てて `AO_OK`。
- **busy 中でも呼べる。** 中断は評価の内側から効かせる入口なので、既存の「busy なら拒む」一覧には入れない。
- フラグはセッションに 1 つ。多重押しは 1 回分（溜めない）。
- 消費は「理由 `interrupted` で止めたとき」。abort・評価の正常終了・次の `ao_eval` 入場時にもクリアし、前の評価の要求を持ち越さない。
- **ライブモード off では止めない**（フラグを立てても評価は完走する）。これは受け入れの不変条件であり、「ライブ off でも中断して止める」ことだけを後続／やらないにする。

### ABI: RunLoop pump フック

既存フックと同型にする。

```c
typedef void (*AoRunLoopPumpFn)(void* user);
/* NULL removes it. Survives shutdown, boot, and image load. May be set before the first boot.
   May be set while busy (so the app can register at launch and leave it). */
void ao_set_runloop_pump_hook(AoRunLoopPumpFn fn, void* user);
```

- 未登録（NULL）なら pump は何もしない。GoogleTest はフラグを直接立てて止められる。
- Ao.app は起動時に登録し、非ブロッキングで溜まったイベントを数個まで処理する（待ち時間 0）。

### 誰がフラグを見て止めるか

- フラグを消費して止めてよいのは、**ベースが待っているライブ評価プロセス**が、自分のインタプリタ実行中に検査点へ来たときだけである。
- fork したプロセス、drain 中の他プロセス、ベース、すでに halt 中のプロセスは、フラグを見ても止めない（消費もしない）。P11 の「評価プロセスでないプロセスの停止」をやらない、を守る。

### 検査点（セーフポイント）

一本化する。

- **フラグ確認と（間引き後の）pump 呼び出しの検査点は、インタプリタの safepoint（後方ジャンプを含む）だけである。**
- ネイティブが発行する既存 safepoint（64K ループなど）では、中断フラグを見ない。ネイティブへの safepoint 強制挿入もしない。
- 既知の制限（SPEC / CHANGELOG に残す）:
  - インタプリタ safepoint に来ない長いネイティブ（巨大なネイティブ `do:` など）
  - fork や drain で回っている、評価プロセスでないプロセスのループ
  これらは ⌘. でも止まらず、強制終了しかない。

### フラグ確認と pump の頻度

- **フラグ確認は、対象プロセスの各インタプリタ safepoint で行う**（間引かない）。GoogleTest が pump 無しで止められる契約と一致させる。
- **pump だけ間引く。** 目安は「一定回数のインタプリタ safepoint ごと」または「短い壁時計間隔ごと」の、どちらか先。正確な定数は実装時に `docs/bench.md` と手感で決める。

### 再入

- pump 中に届いてよいのは Interrupt（と再描画などランタイムを動かさないイベント）に限る、という **ホスト側の規律** を前提にする。
- ランタイム側は、評価中（busy）の禁止 ABI（新たな `ao_eval`、Accept、image save/load、削除など）を今までどおり拒む。ネストした RunLoop からそれらを呼んでも `runtime is busy`。
- 中断で止まったあとは、既存どおり runtime は busy ではない（P11）。Debugger 操作と別の Do it は P11 の規則のまま。

### ホスト（Ao.app）

- Smalltalk メニューに Interrupt。キー相当は ⌘.。
- 有効化は「ライブ評価中（busy かつライブモード）」が理想。常時メニューにあっても、効くのは評価中の `ao_request_interrupt` だけでもよい。
- 中断後の UI は P11 のライブ停止と同じ経路（ライブ Debugger、Workspace の `halted: …`）。理由文字列 `interrupted` をそのまま出す。
- 評価中は Interrupt 以外の Smalltalk 評価系メニューをグレーアウトし、二重評価をホスト側でも避ける。

### Proceed のあと

- フラグは停止時に消費済み。Proceed でループが再開しても、再要求するまで止まらない。
- 再度 ⌘. すれば、また止まる。

## 範囲

**やる:**

- SPEC に P13 を追加し、中断の規則を書く（下記「SPEC への落とし込み」）
- `ao_request_interrupt`、`ao_set_runloop_pump_hook`、セッションフラグ、インタプリタ safepoint での確認とライブ停止
- Ao.app の pump 登録、⌘. / Interrupt
- GoogleTest / XCTest / 手動確認、CHANGELOG、PHASE、bench の確認

**やらない:**

- CLI `SIGINT`
- ライブモード off での中断停止
- Restart、Debugger 内編集 / Accept、temp の書き換え
- ネイティブへの Step into、ネイティブ safepoint での中断、ネイティブへの safepoint 強制挿入
- 評価のワーカースレッド化、ブリッジキュー
- フォークした別プロセスや、すでに halt 中のプロセスをこの操作で新たに止めること
- Escape キー

## テスト

### runtime（GoogleTest）

- ライブモードで、タイトな後方ジャンプのループ中にフラグを立てると `AO_ERR_HALT` と理由 `interrupted`
- 停止は命令境界（合成の最内ネイティブフレームが無い）。Proceed は止めた命令から続き、ループが再開する
- フラグは 1 回で消費され、Proceed 後に再要求なしでは再停止しない
- ライブモード off ではフラグがあっても評価が完走する（不変条件）
- 中断停止中の Save Image 拒否は P11 と同じ理由
- busy 中の `ao_request_interrupt` は成功する（拒まない）
- pump 未登録でもフラグだけで止まれる
- 評価プロセスでないプロセスの実行中にフラグを立てても、そのプロセスは止まらない（評価プロセスが次に safepoint に来たときだけ止まる、または評価が終わればフラグはクリア）
- 止まっているプロセスが 8 あるときの中断は、step と同じく理由 `interrupted` で abort する
- Kernel 走査、`ao --test image/tests`、既存デバッガ系は緑のまま
- `docs/bench.md` のホットループ比が揺らぎを超えて悪化しない

### app（XCTest）

- Interrupt メニュー項目と ⌘. のバインドがある
- `ao_set_runloop_pump_hook` を登録できる（NULL で外せる）
- 評価中に中断要求するとライブ Debugger が開き、理由に `interrupted` が含まれる
- 評価していないときの Interrupt がクラッシュや不正な halt を残さない

### 手動確認（Ao.app）

1. `[true] whileTrue` → ⌘. → Debugger、理由 `interrupted`
2. メニューの Interrupt でも同じ
3. Proceed で再開 → 再度 ⌘. で再停止
4. Abort で評価終了、Workspace が使える
5. 評価外の ⌘. で落ちない

## リスク

- **pump の再入:** ホストが評価中に Do it などを許すと、busy 拒否に頼ることになる。Interrupt 以外の評価系は評価中グレーアウトを維持する。
- **応答遅延:** pump 間隔が長いと ⌘. から停止まで体感が遅れる。短すぎると bench が劣化する。定数は計測で決める。フラグ確認自体は毎 safepoint なので、届いたあとの停止遅延は 1 反復程度に抑える。
- **止まらないループ:** インタプリタ safepoint に来ないネイティブと、評価プロセスでないプロセスのループは止まらない。既知の制限として残す。
- **SPEC との衝突:** §1.4 / §3.9 / §3.10 / §3.13 と「やらない」を同じ変更で直さないと、文書内矛盾が残る。

## SPEC への落とし込み（実装前）

必須（どれも省略しない）:

1. **§2.3** に P13「評価の中断」を足す。**§2.4** に、1.1.0 の次のマイナー（例: 1.2.0）に含める旨を追記する。
2. **§1.4** のデバッガ一文を改める。現行の「`halt` と失敗で止め」だけでは足りないので、ホストからの中断（理由 `interrupted`）も止める、と書く。
3. **§3.2** の「§3.9 の評価の中断」参照を、P13 の中断（セーフポイントで効く）に整合させる。
4. **§3.9 Workspace**（現行「外から止める手段は無い」）を、P13 の中断に書き換える: ライブ評価中の ⌘. / Interrupt → 評価プロセスがインタプリタ safepoint で理由 `interrupted` のライブ停止。fork / drain 中の他プロセスのループと、インタプリタ safepoint に来ない長いネイティブは、まだ外から止められず強制終了しかない、と明記する。
5. **§3.9 メニューバー** の Smalltalk メニューに Interrupt（⌘.）を足す。
6. **§3.10** に `ao_request_interrupt`（フラグの寿命、busy でも呼べる、ライブ off では止めない）、`ao_set_runloop_pump_hook`（シグネチャ・寿命・busy 中も設定可・NULL で外す）を書く。
7. **§3.13** の停止表に `interrupted` 行を足す。停止形は命令境界（合成フレーム無し）。**Proceed 本文**の「step と Debug it の停止からは、止めた命令から続ける」に `interrupted` を並べ、halt 型（送信の値 nil）と誤読されないようにする。「止められないとき」規則の適用と、検査点がインタプリタ safepoint のみであること（ネイティブ safepoint では見ない）を書く。「やらない」から「走っている評価の外からの中断」を外し、残制限として「評価プロセスでないプロセスの停止」「ネイティブ safepoint での中断」「ネイティブへの safepoint 強制挿入」を残す／追記する。
8. **§4 / §6** に上記テストと受け入れチェックリストを足す。
9. **CHANGELOG** の Known limitations（P13 完了時）は「評価を中断できない」を丸ごと消さず、次のように書き換える: **「ベースが待っている評価プロセスがインタプリタのセーフポイントに来るタイトループは、⌘. / Interrupt で中断できる。fork や drain で回っている他プロセスのループと、インタプリタ safepoint に来ない長いネイティブは、まだ強制終了しかない。」**

## 受け入れ（P13 完了の定義）

- [ ] Workspace の `[true] whileTrue` を ⌘. でライブ Debugger に止められる（理由 `interrupted`）
- [ ] Interrupt メニューでも同じ
- [ ] Proceed は止めた命令から続き、再要求なしでは再停止せず、再 ⌘. でまた止まれる
- [ ] Abort で評価が終わり、Workspace が使える
- [ ] ライブモード off（CLI 既定と `ao --test`）ではフラグがあっても止まらず、振る舞いが変わらない
- [ ] Kernel 走査緑、`docs/bench.md` の比が悪化しない
- [ ] `PHASE` は `P13`、SPEC §6 の P13 小節がすべて `[x]`、CHANGELOG に項目

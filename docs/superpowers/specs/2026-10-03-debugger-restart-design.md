# P14 — ライブ Debugger の Restart（設計）

## 結論

ライブ Debugger で選んだ解釈フレームを、メソッドまたはブロックの先頭からやり直す。より内側のフレームは捨て、その `ensure:` は走らせない。合成ネイティブと DNU フレームは拒む。Debugger 内の編集、Accept、temp の書き換えはしない。チェンジセット相当は後続である。正本は実装前に更新する SPEC（§2.3 に P14、§3.9 / §3.10 / §3.13 / §6）である。本書は設計判断の理由を残す。

## 前提

- PHASE は `P13`。1.2.0 はリリース済み。SPEC §3.13 の「やらない」に Restart がある。
- 解釈フレームだけが `CallContext::topFrame` の連鎖に載る。ネイティブは載らない。Debugger の最内ネイティブ / DNU は進行中の送信から合成する。
- 停止中のファイバは `Scheduler::halt` の `switchTo(base)` の内側で凍っている。Proceed はそこから戻り、halt 型なら送信の値 nil で続く。
- Abort は `terminate` で後始末を走らせる。Restart はそれと違う。

## 利用者との合意（2026-10-03）

| 論点 | 決定 |
|---|---|
| 次フェーズ | Restart を P14 にする。チェンジセットはその後 |
| Restart の形 | 選んだ解釈フレームを本体先頭からやり直す（クラシック） |
| 後始末 | 捨てる内側の `ensure:` は走らせない |
| 入れない | Debugger 内編集と Accept、temp 書き換え |

## 方式

**採用: ファイバ再開時に Restart 印を見て、対象 `Frame*` まで C スタックを巻き戻し、その活性化を初期化してインタプリタループを続ける。**

1. ホストが `ao_debug_restart(pid, frame_index, …)` を呼ぶ。
2. `frame_index` が種類 0 または 1 でなければ拒む。
3. 止まったプロセスのコンテキストに対象フレームを記録し、Proceed と同じくファイバへ戻る（Proceed 不能の停止でもよい）。
4. `halt()` から通常の true（送信の値 nil）では戻らない。Restart 印を立てて false を返す。
5. 内側の解釈とネイティブは `unwinding` として抜け、対象フレームで pc・オペランドスタック・引数以外の temp を初期化し、印を消してループを続ける。

**不採用: Proceed と同じく halt ネイティブから nil を返す。** 本体先頭からのやり直しにならない。

**不採用: 内側の `ensure:` を走らせる。** Abort と区別が付かず、クラシック Restart でもない。

## 設計判断

- `ao_debug_can_restart(pid, frame_index)` は busy でも読める。pid が止まっていて、その index が解釈フレームなら 1。
- `ao_debug_restart` は最外の入口（busy なら `AO_ERR`）。入場時に中断フラグを消す。答えの形は Proceed と同じ。
- 拒否のメッセージは `restart refused: not an interpreted frame`（種類 2、範囲外）。止まっていない pid は Proceed と同じく空の `AO_ERR`。
- ブロックを Restart したときはその活性化だけ先頭へ。ホームメソッドを Restart したときはブロック側を捨てる。
- 事後 Debugger からは Restart しない。

## やらない

Debugger 内編集と Accept、temp 書き換え、ネイティブフレームの Restart、ネイティブへの step into、事後からの Restart、`ensure:` 付き Restart、チェンジセット、イメージ形式の変更、CLI / `ao --test` の変更。

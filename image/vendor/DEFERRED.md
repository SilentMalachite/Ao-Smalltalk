# Deferred

MappedCollection: does not occur in the pinned sources
FileStream: subclass: #FileStream does not exist
Bag size は P9 のゴールデンに入らなかった

## 載せないメソッド

LOAD_ORDER による file-in で意図して載せないメソッドを、1 行に 1 つ `Class>>selector: 理由` の形式で書く（SPEC §3.12）。
クラス側は `Class class>>selector: 理由` と書く。キーワードセレクタは、末尾の `:` のあとに `: 理由` を続ける（例: `Bag>>sum:ifEmpty:: 理由`）。
この形式でない行は注記として読み飛ばす。

列挙したメソッドのエラーは数えず、報告もしない。そのメソッドはイメージに入らない。
`vendor_filein_test` は、失敗するメソッドの集合がこの列挙と一致することを確かめる。直して載るようになったら、その行を消す。

いまの 10 件は、どれもコンパイルエラーである。括弧内は実際のエラーメッセージ。

```text
Bag>>product:: ブロックが引数と temp を両方持つ構文 [ :each :times | | v | ... ] を解析できない（expected ']'）。B7 のパーサで直す
Bag>>sum:ifEmpty:: ブロックが引数と temp を両方持つ構文 [ :each :times | | v | ... ] を解析できない（expected ']'）。B7 のパーサで直す
Notification class>>signalToUpdateDebuggerOn:dueTo:: 波括弧の配列 {...} は Blue Book の構文でない（expected expression）。B7 で image/patches/ に書き換えるか、ここに残す
Date>>printOn:format:: 波括弧の配列 {d. m. y} は Blue Book の構文でない（expected expression）。B7 で image/patches/ に書き換えるか、ここに残す
Time class>>dateAndTimeFromSeconds:: 波括弧の配列 {...} は Blue Book の構文でない（expected expression）。B7 で image/patches/ に書き換えるか、ここに残す
Time class>>dateAndTimeNow: 波括弧の配列 {...} は Blue Book の構文でない（expected expression）。B7 で image/patches/ に書き換えるか、ここに残す
Timespan class>>includingDateAndTime:: コンパイル時リテラル `Duration zero`（バッククォート）は Blue Book の構文でない（invalid token）。B7 で image/patches/ に書き換えるか、ここに残す
Time class>>primHighResClock: 名前付きプリミティブ <primitive: 'primitiveHighResClock'> は Blue Book の構文でない（expected primitive number）。B7 で image/patches/ に書き換えるか、ここに残す
Time class>>primUtcWithOffset: 名前付きプリミティブ <primitive: 'primitiveUtcWithOffset'> は Blue Book の構文でない（expected primitive number）。B7 で image/patches/ に書き換えるか、ここに残す
Time class>>primUtcWithOffset:: 名前付きプリミティブ <primitive: 'primitiveUtcWithOffset'> は Blue Book の構文でない（expected primitive number）。B7 で image/patches/ に書き換えるか、ここに残す
```

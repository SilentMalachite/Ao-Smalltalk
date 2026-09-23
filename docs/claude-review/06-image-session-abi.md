# 06 イメージ・セッション・C ABI・CLI

対象: `runtime/src/{ImageFormat,ImageSave,ImageLoad,VendorExtract,Session,abi,ao_main,TestRunner}.cpp`、`runtime/src/Session.hpp`、`runtime/include/ao/{Image,ImageFormat,Vendor,TestRunner}.hpp`、`bridge/ao_abi.h`、`image/`（Kernel/*.st、vendor/cuis/*.st、ALLOWLIST、LOAD_ORDER、DEFERRED.md、tests/*.st）、関連テスト（image_format / image_save_load / image_registry / vendor_extract / vendor_filein / vendor_overwrite / session_abi / kernel_scan / smoke）。ファイル・イン経路の確認のため `runtime/src/Compile.cpp`（fileIn*）、`runtime/src/kernel/BlockContext.cpp`（makeNativeBlock）、`runtime/src/Gc.cpp` も参照した。

検証: コード読解に加えて、スクラッチ領域で `build/` の既存静的ライブラリにリンクしたプローブと `build/ao` CLI を実行した。
- `ao image save/load`、`ulimit -f` による書き込み失敗の再現
- 細工したイメージ（flags、klass、methodDict の書き換え）をロードしてから GC と評価を実行
- 切り詰め 150 件、ビット反転 300 件、ワード上書き 300 件のファジング
- native block thunk をヒープへ逃がしてから save/load
- 生存データを old 容量の近くまで増やしてから save/load
- vendor LOAD_ORDER の file-in でエラー件数を数え、`ao filein` と `ao --test` の終了コードを確認
- 評価中のフックから ABI を再入

ビルドは Debug（assert 有効）である。Kernel `.st` はプロトコル見出しとコメントだけで実行定義は無く、file-in も拒否されることを確認した（違反なし）。

## 概要
| 重大度 | 件数 |
|---|---|
| Critical | 0 |
| High | 4 |
| Medium | 3 |
| Low | 6 |

## 指摘

### [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる
- 場所: `runtime/src/ImageSave.cpp:218-227`（`writeFile`）、呼び出しは `:318`
- 問題: 保存先を `std::ios::trunc` で直接開いて書く。一時ファイルへ書いてから `rename` する手順も、`fsync` も無い。書き込み途中で失敗すると、元のイメージは切り詰められたまま残る。
- 失敗シナリオ: 正常な 71,632 バイトのイメージに、`ulimit -f 20` の状態で `ao image save … p.aoimage` を実行する。save は exit 1 を返し、ファイルは 10,240 バイトに縮む。その後の `ao image load` は失敗する。ディスクフルや電源断のほか、下記の High #2 と #3（保存は成功するがロードできない）でも同じく旧イメージを失う。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 同じディレクトリの一時ファイルへ書き、`fsync` してから `rename(2)` で置き換える。失敗したら一時ファイルを消す。

### [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる
- 場所: `runtime/src/kernel/BlockContext.cpp:29-33`（`makeNativeBlock`）、`runtime/src/ImageSave.cpp:73-95,175-216`（検証なし）、`runtime/src/ImageLoad.cpp:281-283,357-360`（名前で拒否）
- 問題: `makeNativeBlock` は `NativeRegistry::add`（名前なし）で登録し、名前 `ao_NativeBlock_thunk` の NativeMethod を作る。この名前は `gNativeByName` に無い。Image::save は NativeMethod の名前が再結合できるかを確かめずに書き出すので、ロード時の `findName` で必ず失敗する。
- 失敗シナリオ: ユーザークラスで `do: aBlock  self holder at: 1 put: aBlock`（`holder ^ #(1)`）を定義し、`(WriteStream on: String new) nextPutAll: Keeper new` を評価する。thunk がリテラル配列に残る。この状態で `ao_image_save` は 0、同じファイルの `ao_image_load` は 1 を返し、CLI の `ao image load` も失敗する。対照として thunk を逃がさない場合は load=0 になる。High #1 と重なると、保存先の旧イメージも失う。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: 保存時に、NativeMethod の名前が `findName` で解決できない場合は save を失敗させる（または thunk を nil にする）。thunk をブロックとして外へ渡さない設計に変える案もある。

### [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない
- 場所: `runtime/src/ImageSave.cpp:231-318`（容量の検査なし）、`runtime/src/ImageLoad.cpp:438`（`heapBytes > heap.oldCapacity()` で拒否）
- 問題: save は nursery と old の両方にある生存オブジェクトを 1 つのヒープ区画に並べる。load はその区画を既定容量（4MB）の old 空間へまとめてコピーする。old と nursery の合計が 4MB を超えても実行中のセッションは正常に動くが、それを表したイメージはロードできない。save は検査せず成功を返す。
- 失敗シナリオ: クラスから到達できるリテラル配列に `Array new: 1000` を 400 個と 120 個格納すると、oldUsed 3.69MB、nursery 0.55MB になる。この状態で `ao_image_save` は 0 を返し、4,244,988 バイトのファイル（heapBytes 4,240,264）ができる。`ao_image_load` と `ao image load` はどちらも失敗する。High #1 と重なると旧イメージも失う。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: save 時に `tr.end` がロード先の old 容量を超えるなら失敗を返す。あるいは load 側でヘッダの `heapBytes` に合わせて old 空間を確保する（既定容量の扱いは SPEC を先に直す）。

### [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する
- 場所: `runtime/src/Compile.cpp:213-237`（`applyMethodsFor` は `continue` して true）、`:320-354`（`fileInLoadOrder`）、`runtime/src/ao_main.cpp:163-172,229-236`、`runtime/src/Session.cpp:187-193`
- 問題: メソッド単位のコンパイルエラーとネイティブ上書きの拒否は `errors` に積まれるだけで、戻り値は true のままである。CLI の `ao filein` は exit 0 で何も出力しない。`ao image save --load-order` はそのまま保存し、`ao_filein_load_order` は AO_OK を返す。そのため、アプリの「file-in failed」表示も出ない。
- 失敗シナリオ: 現行の `image/vendor/LOAD_ORDER` で errors が 17 件出る。欠落する例は `Exception>>description`、`Bag>>sum:ifEmpty:`、`Date>>printOn:format:`、`Heap>>heapExample`、`Time>>dateAndTimeNow`、`Time class>>releaseClassCachedState` である。原因は、`,` 二項演算子が invalid token になること、`{}` 配列、文字列形式の `<primitive:>`、クラス変数への代入である。file-in 後に `Exception new description` を評価すると DNU（Message）が返る。`vendor_filein_test` は戻り値しか見ておらず、DEFERRED.md にも記録が無い。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `errors` が空でなければ `fileInLoadOrder` と CLI を失敗扱いにし、ファイル名付きでエラーを報告する。意図して除外するメソッドは DEFERRED.md に列挙する。

### [Medium] ロード時にオブジェクトヘッダの flags を検証せず、Marked などが GC を壊す
- 場所: `runtime/src/ImageLoad.cpp:186-203,313`、影響先は `runtime/src/Gc.cpp:217`
- 問題: save が書くのは `Bytes|Weak` だけだが、load はファイル上の `kFlagMarked`、`kFlagForwarded`、`kFlagImmovable` をそのまま採用する。old GC は Marked 済みのオブジェクトを走査しないので、その子が回収・移動され、参照が宙に浮く。
- 失敗シナリオ: pointer オブジェクトの一部に flags `0x10` を立てたイメージを `ao_image_load` すると成功し、評価も通る。その後の `Gc::collectOld()` で SIGSEGV（exit 139）になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `walkObjects` で `flags & ~(kFlagBytes|kFlagWeak)` が 0 でなければ拒否する（または 0 に落とす）。

### [Medium] クラス参照とクラスの形を検証せず、壊れたイメージでロード中または直後にプロセスが落ちる
- 場所: `runtime/src/ImageLoad.cpp:233`（klass は任意の即値またはオブジェクトで通る）、`:324-333`（well-known に任意オブジェクトを bind）、`runtime/src/Session.cpp:168-175`
- 問題: precheck が確かめるのは参照先がオブジェクトの先頭かどうかだけである。klass が SmallInteger やバイト列でも、well-known のクラスや methodDict がバイト列でも受理する。後段の `Dictionary new`、`1 + 2` の探針、lookup が `slotAt` でバイト列を読む。Debug では assert で abort し、Release では範囲外のポインタを読む。
- 失敗シナリオ: SmallInteger クラスの methodDict スロットを Symbol に書き換えると、`ao_image_load` の探針中に abort する。Processor インスタンスの klass を Symbol にすると、ロードは 0 を返したあと `Processor printString` で abort する。ワード上書きのファジングでは 300 件中 192 件が受理された（切り詰めは 150 件すべて拒否され、パーサの境界検査は健全だった）。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: ロード時に、klass がポインタ形で `kClassSlotCount` 以上のスロットを持つオブジェクトであることを検証する。well-known のクラスと methodDict の形、CompiledMethod の最低限の形も検証する。

### [Medium] `ao --test` が実行時エラーを失敗として数えない
- 場所: `runtime/src/TestRunner.cpp:154-159`
- 問題: 失敗の判定は「doIt の結果が空」か「`assert:equals:` の不一致」の 2 つだけである。DNU は Message を値として返し、`error:` は引数を返すだけなので、途中の文で起きたエラーは無視されて先へ進む。
- 失敗シナリオ: 本文が `nil foo. self assert: 1 equals: 1.`、`#(1 2) at: 9. …`、`3 zork: 4. …`、`… 1/0.`、`self assert: (self error: 1) equals: 1.` のテストは、いずれも exit 0 で合格になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `CallContext` にエラー件数を持たせ、DNU と `error:` の経路で加算する。ランナーは各ファイルのあとでその件数を見る。

### [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする
- 場所: `runtime/src/TestRunner.cpp:47-59,118-124,137-141`
- 問題: 組み立てた `"x ~= y"` を `error:` に渡すが、`error:` は何も出力しない。コンパイルエラー、ファイル読み込みの失敗、ディレクトリ不在も、何も言わずに exit 1 で終わる。`.st` が 0 件なら exit 0 になる。
- 失敗シナリオ: `self assert: 1 + 2 equals: 4.` → exit 1、出力なし。`self assert: 1 + equals: 4.` → exit 1、出力なし。空ディレクトリ → exit 0。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: ファイル名、メッセージ、コンパイルエラーの位置を stderr に出す。テスト 0 件はエラーにする。

### [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free
- 場所: `runtime/src/Session.cpp:153-178`（`g_session = std::move(next)`）。`ao_runtime_shutdown` と `ao_image_save` も同様にガードが無い
- 問題: 実行中のインタプリタは旧セッションの Heap と ctx を参照している。それでも `gInterpreterDepth` を見ずにセッションを破棄する。SPEC は save だけを「インタプリタの外から」と定めており、load には再入の禁止もガードも無い。
- 失敗シナリオ: transcript フックの中で `ao_image_load` を呼び、`Transcript show: 'a'. Transcript show: 'b'. 3 + 4` を評価すると、フック内の load は 0 を返し、その後 SIGSEGV になる。
- 根拠: プローブで再現（確度: 高）
- 修正の方向: `gInterpreterDepth > 0` のときは save、load、shutdown、boot、eval を AO_ERR で拒否する。

### [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない
- 場所: `runtime/src/Session.cpp:889-900,919-934,999-1009`
- 問題: `ao_browser_protocol_count`、`ao_browser_selector_count`、`ao_browser_subclass_count` は、未知のクラスや不正な meta のとき `AO_ERR` を返す。この値は「1 件」と同じである。
- 失敗シナリオ: `ao_browser_subclass_count("NoSuchClass")` が 1 を返す。アプリは `at(0)` の失敗で空リストにするので影響を免れているが、ABI の契約としては曖昧である。
- 根拠: コード読解（確度: 高）
- 修正の方向: エラーは -1 を返す。ヘッダにも明記する。

### [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す
- 場所: `runtime/src/Runtime.cpp:12-18`、`runtime/src/abi.cpp:9-11`
- 問題: SPEC は「入り切らないときは `AO_ERR_RANGE`」と定める。実装は 0 か 1 だけを返し、abi 側で AO_ERR にまとめる。
- 失敗シナリオ: `ao_version(buf, 4)` が AO_ERR を返すので、呼び出し側はバッファを広げて再試行する判断ができない。
- 根拠: コード読解（確度: 高）
- 修正の方向: 切り詰めを区別して AO_ERR_RANGE を返す。

### [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない
- 場所: `runtime/src/abi.cpp:163-177`、`runtime/src/Session.cpp:28-35,86-92`
- 問題: `g_transcriptFn` は保持されるが、`ctx->transcriptHook` を設定するのは、呼び出しの時点でセッションがある場合だけである。新しいセッションの `installEmptyCache(nullptr, nullptr)` はフックを戻さない。
- 失敗シナリオ: `ao_set_transcript_hook(fn)` → `ao_runtime_boot()` → `Transcript show:` の順に呼ぶと何も届かない。shutdown→boot の再起動でも同様に消える。アプリは boot を先にしているので現状は回避できている。
- 根拠: コード読解（確度: 高）
- 修正の方向: `sessionBoot` で abi 側の現在のフックを ctx に反映する（abi から Session へ登録関数を渡す）。

### [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる
- 場所: `runtime/src/ImageLoad.cpp:31-48,400-411`
- 問題: ファイルサイズ分の `vector::resize` をしてから、ヘッダの `heapBytes` と old 容量を照合する。
- 失敗シナリオ: 巨大なファイルを誤って Open Image すると、全体をゼロ埋めで確保する。`bad_alloc` が起きれば `extern "C"` 境界を越えて `std::terminate` になり、CLAUDE.md の「例外を境界で投げない」に反する。
- 根拠: コード読解（確度: 中）
- 修正の方向: まず 48 バイトのヘッダを読み、`heapBytes <= oldCapacity` とファイルサイズの上限を確かめてから本体を読む。ABI 関数では例外を捕捉する。

## テストの穴
- `image_save_load_test` のロード拒否テストは不正なマジックだけである。切り詰め、flags、klass、methodDict の破損、ファジングが無い。
- 保存失敗時に旧イメージが残ることを確かめるテストが無い。一時ファイル＋rename は未実装である。
- NativeMethod の名前が解決できないオブジェクト（native thunk）を含むヒープを save→load するテストが無い。
- `vendor_filein_test` は `fileInLoadOrder` の戻り値だけを検証し、`errs.empty()` を見ていない。17 件のエラーが見逃されている。
- CLI の終了コード（`ao filein` と `ao image save --load-order` がエラー時に非 0 を返すこと、`ao --test` が実行時エラーと 0 件で失敗すること）のテストが無い。
- `session_abi_test` に次のテストが無い: フックを boot 前に設定するケース、shutdown→boot、評価中の再入、未知クラスでの count、`ao_version` の RANGE。
- 生存データが old 容量に近いセッションを save→load する容量境界のテストが無い。

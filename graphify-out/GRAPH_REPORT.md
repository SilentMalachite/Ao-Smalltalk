# Graph Report - ao-smalltalk  (2026-09-27)

## Corpus Check
- 13 files · ~346,123 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 6224 nodes · 16351 edges · 365 communities (215 shown, 150 thin omitted)
- Extraction: 87% EXTRACTED · 13% INFERRED · 0% AMBIGUOUS · INFERRED: 2101 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- WellKnown
- Compile.cpp
- TEST_F
- DebuggerWindowTests
- string
- TEST
- .fromSmallInteger
- Interpreter::run
- TEST (2)
- TranscriptWindow
- abi.cpp
- DebuggerWindow
- send
- Parser
- LargeInteger.cpp
- AcceptTests
- BrowserWindow
- TEST (5)
- Stream.cpp
- cstdint
- Oop
- TEST_F (2)
- Emitter
- Scanner.cpp
- TEST (4)
- TEST (3)
- Heap.cpp
- Scheduler
- P12 — Browser の削除 (phase doc)
- TEST (6)
- Boot
- cstdint (2)
- Heap
- DebugFrames
- Session
- TEST (8)
- TEST (11)
- TEST (9)
- ImageSave.cpp
- .false_
- TEST_F (4)
- Geometry.cpp
- TEST_F (3)
- ao_eval
- Emitter (2)
- Float.cpp
- BrowserModel
- CallContext
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- Scheduler.cpp
- Ast
- .nil
- TEST_F (6)
- Emitter (3)
- Session.cpp
- classRows
- .isHeap
- WellKnown.cpp
- Heap (2)
- ToolWindowTests
- P12 追補: クラス ID と壊れたメソッド辞書 実装計画
- P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- String.cpp
- TEST (16)
- TEST (7)
- FiberStack
- HashedCollection.cpp
- Array.cpp
- AoApp
- DebugSnapshot
- string (2)
- P12 Browser 削除 実装計画
- P6b — vendor file-in
- DebugSnapshot.cpp
- ao_runtime_boot
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- evalBody
- Roots
- Float.cpp (2)
- Scheduler::Record
- WorkspaceWindow
- ImageFormat
- TEST (13)
- Claude Review Fixes Plan (2026-09-23)
- Boolean.cpp
- VendorExtract.cpp
- Cooperative Process Scheduler
- LiveFrames
- Bootstrap.cpp
- BlockContext.cpp
- TEST_F (5)
- TEST (14)
- Class Removal Rules
- TEST (15)
- BrowserModelTests
- AppKit
- TEST (12)
- P5 — コンパイラ
- ClassPool.cpp
- Process.cpp
- Gc
- DefinitionScanner
- .build
- TEST (17)
- Cooperative Process Scheduler (2)
- TEST_F (5) (2)
- ImageLoad.cpp
- Roots.cpp
- uint64_t
- TEST (18)
- ao_main.cpp
- Literal
- clearUnwinding
- VendorExtract.cpp (2)
- PingPong
- ImageSurgery
- TEST (19)
- PingPong (2)
- pc_map_test.cpp
- abortReasonText
- P4 — Kernelネイティブ実装
- .nil (2)
- ImageLoad.cpp (2)
- VendorExtract.cpp (3)
- GarbageFirstBoot
- native_method_test.cpp
- TEST (20)
- TEST (21)
- TEST (7) (2)
- DiskHeader
- CountingSink
- Loaded
- ChunkAction
- ChunkParser.cpp
- P3 — ネイティブディスパッチ
- PingPong (3)
- VirtualRegion.cpp
- TEST (22)
- TEST (23)
- TEST (24)
- MethodDebugInfo
- Stack
- TEST (5) (2)
- Claude Review Fixes Plan (2026-09-23) (2)
- .isTrue
- TEST_F (7)
- TEST_F (8)
- TEST (27)
- Method Removal Rules (3)
- TEST (28)
- TEST (26)
- Ao Smalltalk 概要 (JA)
- Ao 実装文書 index (docs/README.md)
- Ao 実装文書 index (docs/README.md) (2)
- P8 — AppKit ツール
- FrameBlock
- path
- Kernel Classes (NativeMethod required)
- Method Removal Rules
- Token
- P2 — ブートストラップ
- ImageFormat (2)
- TEST (10)
- TEST (29)
- native_send_test.cpp
- ao_eval (2)
- codegen
- ChunkParser.cpp (2)
- cstring
- P8–P9 Remaining Implementation Plan
- send2
- Vendor.hpp
- synthesizesNative
- VendorExtract.cpp (4)
- ao_runtime_boot (2)
- WorkspaceEvalTests
- TEST (30)
- Class Removal Rules (2)
- CallContext (2)
- Counts
- ChunkAction (2)
- ImageLoad.cpp (3)
- WellKnown.cpp (2)
- Method Removal Rules (3) (2)
- FileSizeLimit
- Rec
- Heap (3)
- WellKnown.cpp (3)
- cli_test.sh
- popFrame
- VendorExtract.cpp (5)
- TestDir
- test.sh
- Ao Smalltalk 概要 (JA) (2)
- TEST (32)
- codegen (2)
- TEST (12) (2)
- compiler/tests/smoke_test.cpp
- CallContext (3)
- CallContext (4)
- Roots::visitAll
- abortingSubclass
- Root
- imageRegistryStubA
- acceptClass
- answerOne
- AcceptTests (2)
- .turnRunLoop
- Post-mortem debugger
- Live debugger
- Rules That Do Not Bend
- string (2)
- cstdint (3)
- .specialSelector
- Stream.cpp (2)
- cstring (2)
- Scheduler::Record (2)
- WellKnown.cpp (4)
- abortingNew
- build.sh
- BrowserModel.subclasses(of:)
- BrowserModel.superclass(of:meta:)
- BrowserWindow.showHierarchy() / toggleHierarchy
- Package.swift
- Native Method Symbol Naming Convention
- ao_compiler library target
- ClassMethodCache の無効化が定義クラスの分だけ
- out == NULL の Do it が副作用ありで AO_ERR を返す
- =は値で比較するのにhashは同一性ハッシュのままで=/hashの契約を破る
- int64を超える整数リテラルが黙って0になる
- Dictionary/Setがhashを捨てて線形探索し要素数の2乗で遅くなる
- Kernelクラスにinstance変数名が無くinstVarNamed:等が失敗する(既報関連)
- P0→P11 linear dependency
- uint64_t (2)
- BrowserModel.applyHierarchyList(_:selecting:)
- BrowserModel.hierarchy(of:meta:)
- BrowserModel (2)
- BrowserModel (3)
- BrowserWindow (2)
- BrowserWindow (3)
- BrowserWindow (4)
- BrowserWindow (5)
- BrowserWindow (6)
- BrowserWindow (7)
- BrowserWindow (8)
- BrowserWindow (9)
- BrowserWindow (10)
- BrowserWindow (11)
- BrowserWindow (12)
- BrowserWindow (13)
- AcceptTests (2)
- AcceptTests (3)
- ao_browser_class_count
- ao_browser_subclass_count
- ao_eval (2)
- 1.0.0 release
- codegen (3)
- compiler/src/Scanner.cpp chunk scanner (referenced, not read)
- 非クラス名で ao_accept_method を呼ぶと範囲外書き込み
- 宣言した temp が Workspace 束縛と混ざる
- Dictionary の内部スロット位置を直書き
- eval ごとに knownGlobals と束縛全体を作り直す
- Save / Open Image の失敗がユーザーに見えない
- Smalltalk が knownGlobals に含まれない
- 一度未宣言だった名前が、後から定義したクラスを隠す
- ブートストラップしたクラスの名前が Symbol でなくクラス nil のバイト列
- identity hash が 16 ビットで SPEC のサイドテーブルがない
- Roots が同じスロットの二重登録を許し collectOld の更新が冪等でない
- グローバル Smalltalk がクラス nil の 57 要素固定配列でどのメッセージも通らない
- BlockContextをクロージャとアクティベーションに兼用しsenderを上書きしたまま戻さない
- 死んだホームへの ^（cannotReturn:）が評価全体を理由なしに中断する
- DNUのMessage割り当てにGCリトライがなくナーサリ逼迫時にDNUが空OOPになる
- Process/Semaphoreが実行を切り替えない（fork本体は実行されずwaitもブロックしない）
- valueWithArguments:がArray以外のポインタオブジェクトを受け入れ内部スロットを引数に展開する
- asCharacterがサロゲート(U+D800-DFFF)を受け付けPrint itの出力が途中で切れる
- bitShift:にシフト量-2^63を渡すと符号反転オーバーフロー(UB)になり誤った結果を返す
- & | eqv: xor: がBoolean以外の引数をfalseに丸める
- Floatリテラルの解析が正しく丸められない
- Fractionの=と<が無く等しい分数どうしが等しくならない
- >= を (a < b) not で計算するためNaNに対してtrueを返す
- LargeInteger::fromInt64はnurseryが満杯でもGCを再試行しない
- Pointの算術が成分計算の失敗を空OOPのまま新しいPointに格納する
- PointとRectangleのネイティブがサブクラスを扱えない
- to:do:の終端がSmallIntegerでないと失敗する
- バイト列クラスのbasicNew:が2^32以上のサイズを切り詰める
- バイト列クラスに名前付き変数を持つサブクラスを作れてしまう
- 固定長Stringに書くWriteStreamが多バイト文字を黙って捨てる
- IntervalがSmallIntegerでない負の刻みを正の向きとして扱う
- nilをキーや要素にするとtallyだけが増え見えないエントリが残る
- OrderedCollectionのat:が範囲外の添字にnilを返す
- perform:withArguments:がArray以外も受け付けルートされない引数配列を渡す
- ReadStreamのnextPut:が元のコレクションを書き換える
- ReadWriteStreamのcontentsがpositionまでしか返さない
- select:/reject:が述語ブロックを要素ごとに2回評価する
- shallowCopy・printString・asSymbolがnursery不足時にGCを再試行せず失敗する
- ストリームのcontentsがStringとArray以外のコレクションで壊れた値を返す
- Symbolのcopyがinternされていない別のSymbolを作る
- 文字列の中の!!が!に戻らない
- 引数とtempを両方持つブロックが通らず[:a :b | a | b]は誤コンパイルされる
- カスケードの扱いが不完全
- チャンクの種別を1行目のキーワードで判定するのでメソッドをクラス定義と誤認する
- ! ! のあとのDoItチャンクが直前クラスのメソッドとしてインストールされる
- 指数表記の意味がSmalltalk-80と異なる
- 二項演算子の直後の負数リテラルが別のセレクタとして字句化される
- 宣言の検証がない(引数への代入・擬変数名・重複名)
- 構文の入れ子に深さの上限がなくコンパイル時にスタックが溢れる
- ao_versionが切り詰め時にAO_ERR_RANGEではなくAO_ERRを返す
- count系ABIがエラーでAO_ERR(=1)を返し件数1と区別できない
- 評価中のフックからao_image_loadを呼ぶとuse-after-free
- クラス参照とクラスの形を検証せず壊れたイメージでロード中または直後にプロセスが落ちる
- ロード時にオブジェクトヘッダのflagsを検証せずMarkedなどがGCを壊す
- ロード前にファイル全体を読みヘッダ検証が後になる
- native block thunkがヒープへ逃げると保存は成功するがロードできないイメージになる
- 生存データがold容量を超えるセッションを保存でき、そのイメージはロードできない
- ao --testが失敗理由を出さず空ディレクトリを合格にする
- transcriptフックがboot前、またはshutdown→boot後に配線されない
- 新しいセレクタをAcceptするとソース欄が別メソッドの本文に戻る
- AoSpanの区間を捨てているのでエラー位置が分からない
- C++のデプロイメントターゲットがアプリの最小OSと一致していない
- .gitignoreに.cache/と.serena/logs/が無い
- Inspectorウィンドウが閉じても解放されず増え続ける
- package-app.shが作るバンドルは署名検証に通らないDebugビルドになる
- Print itの結果がNULを含むとそこで途切れて壊れた文字列が挿入される
- Transcriptは出力のたびに全文を置き換え大量出力でUIが長時間止まる
- vendorのfile-inがカレントディレクトリ頼みで.appから起動すると読み込まれない
- Workspaceの評価を中断できない(無限ループでアプリが固まる)
- CLAUDE.md (process source of truth)
- P10 debugger plan reference
- P11 plan reference (p11-ancient-matsumoto.md)
- PR table
- SPEC.md (product source of truth)
- runtime/CMakeLists.txt build config
- fileInLoadOrder
- installMethod
- ao_runtime_boot (4)
- abi.cpp (3)
- Compile.cpp (2)
- .isHeap (2)
- vector
- CompileEnv
- Compile.cpp (3)
- Compile.cpp (4)
- .isHeap (3)
- isKernelClass
- runtime_src_fiber
- TEST (34)
- TEST (35)
- TEST (36)
- TEST (37)
- TEST (38)
- vector (2)
- TEST_F (9)
- ao_runtime_boot (5)
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- compiler/tests/smoke_test.cpp (2)
- ao_runtime_boot (2) (2)

## God Nodes (most connected - your core abstractions)
1. `Oop` - 914 edges
2. `Heap` - 254 edges
3. `WellKnown` - 168 edges
4. `vector` - 149 edges
5. `BrowserWindow` - 124 edges
6. `Session` - 112 edges
7. `TEST_F()` - 111 edges
8. `TEST()` - 106 edges
9. `Ast` - 103 edges
10. `Boot` - 101 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `TDD` --references--> `DebuggerWindowTests`  [INFERRED]
  docs/phases/P11.md → app/AoTests/DebuggerWindowTests.swift
- `前提` --references--> `DebugFrames`  [INFERRED]
  docs/phases/P11.md → runtime/include/ao/DebugSnapshot.hpp
- ``Object>>halt`（P10-03）` --references--> `ao_Object_halt()`  [INFERRED]
  docs/superpowers/plans/2026-09-26-p10-debugger.md → runtime/src/kernel/Object.cpp
- `仕様` --references--> `abortEvaluation()`  [INFERRED]
  docs/phases/P10.md → runtime/src/Send.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **SPEC 3.10 class ID scheme: stable IDs across Browser rows, ABI, and Session table** — app_ao_browsermodel_browserclass, app_ao_browsermodel_classid, bridge_ao_abi_ao_browser_class_id, runtime_src_session_browserclassid, runtime_src_session_sessionclassforid, runtime_src_session_assignclassids, runtime_src_session_classid [EXTRACTED 0.90]
- **SPEC 3.9 removal flow: Browser UI to method dictionary and source table** — app_ao_browserwindow_removemethod, bridge_ao_abi_ao_remove_method, runtime_include_ao_compile_removemethodof, runtime_src_compile_removemethodof_impl, runtime_include_ao_methoddictionary_removekey, runtime_src_session_forgetmethodsource [EXTRACTED 0.95]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — docs_claude_review_00_recent_diff_classdef_string_silently_dropped, docs_claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, docs_claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, docs_claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **Spec-first design refinement of the debugger across P10 and P11** — docs_prs_p10_01_spec313, docs_prs_p11_01_spec313live, docs_prs_p11_01_p11plan [INFERRED 0.75]
- **Vendor Loading Kernel-Integrity Safeguards** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_allowlist, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_native_overwrite_refusal, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_kernel_scan_narrowing, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_host_patch [INFERRED 0.80]
- **Vendor File-in to Image Snapshot Pipeline** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_load_order, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_link_class, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.80]
- **Browser removal confirmation flow: context menu → confirmRemove → model refresh** — docs_superpowers_plans_2026_09_26_p12_browser_remove_confirmremove, docs_superpowers_plans_2026_09_26_p12_browser_remove_mainmenu_actions, docs_superpowers_plans_2026_09_26_p12_browser_remove_browsermodel_select [INFERRED 0.80]
- **AppKit ツールウィンドウ群とメニュー** — docs_prs_p8_01_ao_app_skeleton, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [INFERRED 0.80]
- **Vendor extraction, deferral list, and file-in test form one pipeline** — image_vendor_origin_extract_process, image_vendor_deferred_doc, runtime_tests_vendor_filein_test [INFERRED 0.80]
- **SPEC 3.9 class re-accept shape change: reshapeClass carries methods and sources to a fresh class** — runtime_src_compile_acceptclassdef, runtime_src_compile_reshapeclass, runtime_src_compile_compilecarried, runtime_src_compile_installmethod_impl, runtime_src_session_movemethodsource, runtime_tests_accept_abi_test_reacceptwithnewinstancevariablerecompilesmethods [INFERRED 0.85]
- **aoimage Method Persistence Across Restart** — docs_prs_p9_04_aoimage_restart, docs_superpowers_plans_2026_09_22_p7_aoimage_save_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_load_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_native_rebind_by_name, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.85]
- **P10 postmortem debugger data pipeline: pc-map → snapshot → ABI → window** — docs_prs_p10_02_methodimage, docs_prs_p10_03_debugsnapshot, docs_prs_p10_05_aoabih, app_ao_debuggerwindow [INFERRED 0.85]
- **P11 live debugger control flow: eval process → halt → resume/step** — docs_prs_p11_02_evalprocess, docs_prs_p11_03_stoporabort, docs_prs_p11_04_resumehalted, docs_prs_p11_05_stepabi [INFERRED 0.85]
- **Method removal pipeline: ao_remove_method → removeMethodNamed → removeKey / forgetMethodSource / cache invalidation** — spec_ao_remove_method, docs_superpowers_plans_2026_09_26_p12_browser_remove_removemethodnamed, docs_superpowers_plans_2026_09_26_p12_browser_remove_removekey, docs_superpowers_plans_2026_09_26_p12_browser_remove_forgetmethodsource [INFERRED 0.85]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — docs_prs_p1_02_pr, docs_prs_p1_03_pr, docs_prs_p1_04_pr, docs_prs_p1_05_pr, docs_prs_p1_06_pr, docs_prs_p1_07_pr [INFERRED 0.85]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — docs_prs_p4_01_doc, docs_prs_p4_02_doc, docs_prs_p4_03_doc, docs_prs_p4_04_doc, docs_prs_p4_05_doc, docs_prs_p4_06_doc, docs_prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunkparser [INFERRED 0.85]
- **Session Class ID Identity Mechanism** — runtime_src_session_classids, runtime_src_session_classrows, runtime_src_session_browserclassid, bridge_ao_abi_ao_browser_class_id, runtime_tests_browser_abi_test_browserabi, docs_superpowers_plans_2026_09_27_p12_class_identity [INFERRED 0.85]
- **MethodDictionary Malformed-Shape Defense** — runtime_src_methoddictionary_pairarray, runtime_src_methoddictionary_at, runtime_src_methoddictionary_atput, runtime_src_methoddictionary_removekey, runtime_tests_method_dictionary_test_malformeddictionaryanswersnilandtakesnothing, runtime_tests_remove_abi_test_malformedmethoddictionaryfallsbackonsend [INFERRED 0.90]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]
- **Method dictionary mutation paths sharing single cache-invalidation function** — spec_ao_accept_method, spec_ao_accept_method_id, spec_ao_remove_method, spec_ao_remove_class, spec_method_cache [EXTRACTED 1.00]
- **P12 removal flow: Browser UI -> class ID -> C ABI -> dictionary/cache/source-table updates** — spec_browser_remove_ui, spec_section_3_10_classid, spec_ao_remove_method, spec_ao_remove_class, spec_method_cache, spec_source_table [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **Session tables excluded from liveness tracing and image save** — spec_method_cache, spec_source_table, spec_section_3_10_classid, spec_debugger, spec_live_subclass [EXTRACTED 1.00]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **1.1.0 release versioning contract (semver minor bump, version_string, assets, Info.plist)** — spec_minor_version_bump_post_v1, spec_release_semver, spec_version_string_source_of_truth, spec_release_assets, spec_info_plist_version, changelog_1_1_0_release [INFERRED 0.85]

## Communities (365 total, 150 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 1 - "Compile.cpp"
Cohesion: 0.05
Nodes (120): BrowserModel.copyClass(at:), ao_accept_class, ao_browser_class_at, cctype, ChunkAction, Codegen, CompileEnv, compileMethod() (+112 more)

### Community 2 - "TEST_F"
Cohesion: 0.03
Nodes (82): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+74 more)

### Community 3 - "DebuggerWindowTests"
Cohesion: 0.07
Nodes (28): Int, WorkspaceWindow, .errorAccessibilityLabel, .errorText, .inspectorCount, .inspectorText, .inspectorWindow, .textAccessibilityLabel (+20 more)

### Community 4 - "string"
Cohesion: 0.09
Nodes (47): algorithm, Bootstrap, chrono, Chunk, cmath, compile, CompiledMethod, compiler (+39 more)

### Community 5 - "TEST"
Cohesion: 0.02
Nodes (93): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+85 more)

### Community 6 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 7 - "Interpreter::run"
Cohesion: 0.05
Nodes (70): Frame, context, depth, isBlock, method, pc, prev, receiver (+62 more)

### Community 8 - "TEST (2)"
Cohesion: 0.02
Nodes (80): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+72 more)

### Community 9 - "TranscriptWindow"
Cohesion: 0.04
Nodes (54): AnyObject, InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable (+46 more)

### Community 10 - "abi.cpp"
Cohesion: 0.05
Nodes (75): atomic, Body, ClassIdIsStableAndOneRowPerClass, ClassIdsAreDroppedWithTheirClassAndNeverReused, ClassIdsDoNotSurviveBootOrImageLoad, CountsAnswerMinusOneOnFailure, ABI（P10-05）, interpreter (+67 more)

### Community 11 - "DebuggerWindow"
Cohesion: 0.05
Nodes (47): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+39 more)

### Community 12 - "send"
Cohesion: 0.09
Nodes (68): Graphify / Serena, フレーム連鎖（P10-03）, run, InlineCache, cachedClass, cachedMethod, applyMethod(), ao_ArrayedCollection_do_() (+60 more)

### Community 13 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 14 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 15 - "AcceptTests"
Cohesion: 0.12
Nodes (9): AcceptTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+1 more)

### Community 16 - "BrowserWindow"
Cohesion: 0.06
Nodes (36): Any, BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels (+28 more)

### Community 17 - "TEST (5)"
Cohesion: 0.03
Nodes (67): AllocateNoGcSpillsToOld, ByteObjectPayload, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses (+59 more)

### Community 18 - "Stream.cpp"
Cohesion: 0.11
Nodes (65): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+57 more)

### Community 19 - "cstdint"
Cohesion: 0.07
Nodes (19): cassert, string, string_view, vector, uint32_t, strictlyAscending(), cstddef, cstdint (+11 more)

### Community 20 - "Oop"
Cohesion: 0.10
Nodes (60): Graphify / Serena, ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext (+52 more)

### Community 21 - "TEST_F (2)"
Cohesion: 0.03
Nodes (62): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+54 more)

### Community 22 - "Emitter"
Cohesion: 0.14
Nodes (13): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+5 more)

### Community 23 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 24 - "TEST (4)"
Cohesion: 0.04
Nodes (58): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+50 more)

### Community 25 - "TEST (3)"
Cohesion: 0.04
Nodes (56): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+48 more)

### Community 26 - "Heap.cpp"
Cohesion: 0.07
Nodes (50): charconv, allocateTenured, checkNotPoisoned, fitsOld, growOld, initObject, largeObjectBytes, objectBytes (+42 more)

### Community 27 - "Scheduler"
Cohesion: 0.04
Nodes (52): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+44 more)

### Community 28 - "P12 — Browser の削除 (phase doc)"
Cohesion: 0.06
Nodes (54): BrowserModel.classID(named:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:) (+46 more)

### Community 29 - "TEST (6)"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 30 - "Boot"
Cohesion: 0.05
Nodes (51): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+43 more)

### Community 31 - "cstdint (2)"
Cohesion: 0.06
Nodes (44): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, clearWeakAfterOldMark (+36 more)

### Community 32 - "Heap"
Cohesion: 0.05
Nodes (43): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+35 more)

### Community 33 - "DebugFrames"
Cohesion: 0.08
Nodes (36): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context, count, empty, kind, method, pc (+28 more)

### Community 34 - "Session"
Cohesion: 0.05
Nodes (49): 手順（PR の順序と依存）, attachBlocks(), blockLiteral(), ClassId, cls, id, classIdRootSlots(), MethodImage (+41 more)

### Community 35 - "TEST (8)"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 36 - "TEST (11)"
Cohesion: 0.05
Nodes (45): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+37 more)

### Community 37 - "TEST (9)"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 38 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (46): cerrno, fcntl, writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot(), byte (+38 more)

### Community 39 - ".false_"
Cohesion: 0.10
Nodes (45): MethodDictionary::at, MethodDictionary::pairArray, CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t (+37 more)

### Community 40 - "TEST_F (4)"
Cohesion: 0.05
Nodes (44): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+36 more)

### Community 41 - "Geometry.cpp"
Cohesion: 0.15
Nodes (41): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+33 more)

### Community 42 - "TEST_F (3)"
Cohesion: 0.06
Nodes (45): AliasedOldClassRowActsOnItsOwnClass, ClassIdTableDoesNotBlockSuperclassRemoval, int64_t, KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, MalformedMethodDictionaryFallsBackOnSend, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad (+37 more)

### Community 43 - "ao_eval"
Cohesion: 0.05
Nodes (39): v1 known limitations, CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, B2 `to:do:` bench, P11 step branch, P4 microbench (+31 more)

### Community 44 - "Emitter (2)"
Cohesion: 0.05
Nodes (41): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+33 more)

### Community 45 - "Float.cpp"
Cohesion: 0.15
Nodes (43): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+35 more)

### Community 46 - "BrowserModel"
Cohesion: 0.17
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 47 - "CallContext"
Cohesion: 0.05
Nodes (42): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+34 more)

### Community 48 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 49 - "Scheduler.cpp"
Cohesion: 0.10
Nodes (36): afterResume, endEval, enqueue, find, leaveLists, reapDead, recordFailure, switchTo (+28 more)

### Community 50 - "Ast"
Cohesion: 0.10
Nodes (26): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+18 more)

### Community 51 - ".nil"
Cohesion: 0.18
Nodes (34): flags, slotAt, slotAtPut, addFiber, liveFibers, Gc::clearWeakAfterOldMark(), int64_t, Root (+26 more)

### Community 52 - "TEST_F (6)"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 53 - "Emitter (3)"
Cohesion: 0.07
Nodes (35): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), int64_t, LitKind (+27 more)

### Community 54 - "Session.cpp"
Cohesion: 0.10
Nodes (31): removeMethodOf, Image, check, load, save, MethodDictionary::removeKey, removeMethodOf (definition), bumpDebugGeneration() (+23 more)

### Community 55 - "classRows"
Cohesion: 0.13
Nodes (36): assignClassIds(), browserClassAt(), browserClassCount(), browserClassDefinition(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt() (+28 more)

### Community 56 - ".isHeap"
Cohesion: 0.07
Nodes (35): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span (+27 more)

### Community 57 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (34): string_view, isVendorStub(), findSymbol, global, internWith, isFixedGlobal, isPseudoVariableName, Roots (+26 more)

### Community 58 - "Heap (2)"
Cohesion: 0.09
Nodes (35): WellKnown, installArray(), WellKnown, installBoolean(), WellKnown, installCharacter(), WellKnown, installCollection() (+27 more)

### Community 59 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+3 more)

### Community 60 - "P12 追補: クラス ID と壊れたメソッド辞書 実装計画"
Cohesion: 0.13
Nodes (30): P12 追補: クラス ID と壊れたメソッド辞書 実装計画, Design rationale: session class ID table, Design rationale: MethodDictionary shape check (pairArray), int64_t, contextPc(), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode() (+22 more)

### Community 61 - "P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt"
Cohesion: 0.11
Nodes (31): docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, SPEC.md, Codegen.cpp: Emitter::mark と compile* 群, MethodImage.hpp: PcSpan / TempName / pcMap / temps (+23 more)

### Community 62 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 63 - "String.cpp"
Cohesion: 0.14
Nodes (31): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+23 more)

### Community 64 - "TEST (16)"
Cohesion: 0.07
Nodes (28): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+20 more)

### Community 65 - "TEST (7)"
Cohesion: 0.06
Nodes (32): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, CascadePartsAreMessageChains, Inline, IntegerBeyondInt64KeepsItsDigits (+24 more)

### Community 66 - "FiberStack"
Cohesion: 0.10
Nodes (26): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+18 more)

### Community 67 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 68 - "Array.cpp"
Cohesion: 0.14
Nodes (30): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), valueHashWord(), ao_Array_equals() (+22 more)

### Community 69 - "AoApp"
Cohesion: 0.12
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 70 - "DebugSnapshot"
Cohesion: 0.07
Nodes (27): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+19 more)

### Community 71 - "string (2)"
Cohesion: 0.15
Nodes (29): byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), string, WellKnown, debugFrameSource() (+21 more)

### Community 72 - "P12 Browser 削除 実装計画"
Cohesion: 0.13
Nodes (29): P12 — Browser の削除 (phase doc), BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove, Session::forgetMethodSource, Globals::unbind, hasSubclass（生きているサブクラスの判定）, MainMenu.Actions（削除メニュー項目）, P12 Browser 削除 実装計画 (+21 more)

### Community 73 - "P6b — vendor file-in"
Cohesion: 0.15
Nodes (29): vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, P8 — AppKit ツール, Kernel NativeMethod 走査, P6b-01: Cuis pin と ORIGIN.md, Cuis vendor pin (ORIGIN.md), P6b-02: LOAD_ORDER と Kernel 上書き禁止 (+21 more)

### Community 74 - "DebugSnapshot.cpp"
Cohesion: 0.16
Nodes (26): frameAt, uint32_t, DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method(), DebugSnapshot::pc(), DebugSnapshot::process(), DebugSnapshot::receiver() (+18 more)

### Community 75 - "ao_runtime_boot"
Cohesion: 0.08
Nodes (22): AoInspectFn, AoTranscriptFn, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提 (+14 more)

### Community 76 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.14
Nodes (27): Scheduler.hpp/.cpp (switchTo, quiet/normal abort分岐), Send.hpp/.cpp: abortEvaluation / abortEvaluationQuiet, bridge/ao_abi.h: ao_debug_* 宣言, LaunchSet.swift: ao_set_debug_capture(1), WorkspaceWindow.swift: installErrorField/openDebugger, AO_ERR_HALT / AO_EVAL_DEBUGIT / AO_DEBUG_POSTMORTEM / AO_DEBUG_LIVE, p11-ancient-matsumoto.md (計画書), P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数 (+19 more)

### Community 77 - "evalBody"
Cohesion: 0.12
Nodes (25): boxMethodImage, answerAwaited(), answerEval(), blankOut(), AoInspectFn, EvalEnd, optional, StepMode (+17 more)

### Community 78 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 79 - "Float.cpp (2)"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 80 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, string, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+20 more)

### Community 81 - "WorkspaceWindow"
Cohesion: 0.16
Nodes (16): aoWorkspaceInspectHook(), failureText(), keptEvalResult(), selectErrorSpan(), spanMessage(), CChar, Int32, Int64 (+8 more)

### Community 82 - "ImageFormat"
Cohesion: 0.09
Nodes (26): bit, uint16_t, uint32_t, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+18 more)

### Community 83 - "TEST (13)"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 84 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.11
Nodes (27): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+19 more)

### Community 85 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 86 - "VendorExtract.cpp"
Cohesion: 0.11
Nodes (26): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+18 more)

### Community 87 - "Cooperative Process Scheduler"
Cohesion: 0.11
Nodes (24): P11 known limitations, Known limitation: sourceless frames have no statement starts (Step over/into skip statements), Debug it (AO_EVAL_DEBUGIT), MainMenu.swift: Debug it ⌘⇧D, README Debugger usage (Proceed, Step over/into/out, Abort), README.ja デバッガの使い方, v1 に無いもの, Not in v1 (Non-goals) (+16 more)

### Community 88 - "LiveFrames"
Cohesion: 0.09
Nodes (19): CallContext, Frame, size_t, string, uint32_t, LiveFrames, context, frames_ (+11 more)

### Community 89 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (24): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+16 more)

### Community 90 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 91 - "TEST_F (5)"
Cohesion: 0.09
Nodes (21): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+13 more)

### Community 92 - "TEST (14)"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 93 - "Class Removal Rules"
Cohesion: 0.10
Nodes (23): 1.1.0 install notes (macOS 14+, shasum check, Open Anyway / xattr quarantine), Context menu flow (discard confirm -> select row -> remove confirm -> ABI), Class removal unbinds name only, Ao Smalltalk Overview (EN), README Browser Remove Method…/Remove Class… usage, ao_accept_class, Browser Remove UI (context menu, confirmation sheet), Class Aliases (other global names for a class) (+15 more)

### Community 94 - "TEST (15)"
Cohesion: 0.11
Nodes (20): classpool, DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path (+12 more)

### Community 95 - "BrowserModelTests"
Cohesion: 0.18
Nodes (10): BrowserModelTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+2 more)

### Community 96 - "AppKit"
Cohesion: 0.13
Nodes (12): Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, Void, TranscriptBox, transcriptBoxHook() (+4 more)

### Community 97 - "TEST (12)"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 98 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 99 - "ClassPool.cpp"
Cohesion: 0.17
Nodes (22): adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view, uint32_t (+14 more)

### Community 100 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 101 - "Gc"
Cohesion: 0.20
Nodes (20): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+12 more)

### Community 102 - "DefinitionScanner"
Cohesion: 0.23
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 103 - ".build"
Cohesion: 0.22
Nodes (10): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+2 more)

### Community 104 - "TEST (17)"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 105 - "Cooperative Process Scheduler (2)"
Cohesion: 0.13
Nodes (21): Addendum tests (browser_abi_test, accept_abi_test, session_abi_test, method_dictionary_test, BrowserModelTests), P12 Addendum: Class ID and Broken Method Dictionary (2026-09-27), MethodDictionary::pairArray (shape check), Failure Aborts Evaluation, allocateRetry, ao_accept_method_id, ao_image_load, AoSpan.message (error reason) (+13 more)

### Community 106 - "TEST_F (5) (2)"
Cohesion: 0.11
Nodes (21): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, expectFailAbort(), int64_t, KernelBench, string, describe() (+13 more)

### Community 107 - "ImageLoad.cpp"
Cohesion: 0.19
Nodes (19): ifstream, bindAll(), checkGlobals(), byte, T, uint32_t, fileOop(), globalNamesOk() (+11 more)

### Community 108 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 109 - "uint64_t"
Cohesion: 0.25
Nodes (15): acceptWord(), uint64_t, unordered_map, unordered_set, headerAt(), heapShaped(), ObjectRules, behavior_ (+7 more)

### Community 110 - "TEST (18)"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 111 - "ao_main.cpp"
Cohesion: 0.17
Nodes (16): climits, dyld, runtime, addRoots, bootAndRunTests(), string, imageUsage(), main() (+8 more)

### Community 112 - "Literal"
Cohesion: 0.11
Nodes (20): int16_t, string, uint16_t, uint8_t, MethodImage, bytes, literals, numArgs (+12 more)

### Community 113 - "clearUnwinding"
Cohesion: 0.15
Nodes (20): awaitEval, findId, isHalted, proceed, runAwaited, yield, EvalEnd, StepMode (+12 more)

### Community 114 - "VendorExtract.cpp (2)"
Cohesion: 0.25
Nodes (19): allowIndex(), containsHostWord(), string_view, extractVendor(), firstLineKey(), firstNonEmptyLine(), hostPatch(), isHostWordChar() (+11 more)

### Community 115 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 116 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 117 - "TEST (19)"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 118 - "PingPong (2)"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 119 - "pc_map_test.cpp"
Cohesion: 0.13
Nodes (19): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, firstBlock(), named(), pcOf() (+11 more)

### Community 120 - "abortReasonText"
Cohesion: 0.20
Nodes (18): 捕捉（P10-03、配線は P10-04）, refreshStackLimit(), abortReasonText(), clearUnwinding(), string, finishEval(), sessionDebugAbort(), sessionEval() (+10 more)

### Community 121 - "P4 — Kernelネイティブ実装"
Cohesion: 0.16
Nodes (18): P4 — Kernelネイティブ実装, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード (+10 more)

### Community 122 - ".nil (2)"
Cohesion: 0.24
Nodes (16): allocateNoGc, size, at(), bind(), bindIn(), Roots, string_view, uint32_t (+8 more)

### Community 123 - "ImageLoad.cpp (2)"
Cohesion: 0.29
Nodes (17): readHeader, atOffset(), checkFile(), Roots, size_t, string, string_view, WellKnown (+9 more)

### Community 124 - "VendorExtract.cpp (3)"
Cohesion: 0.19
Nodes (18): size_t, dropCycles(), dropMissingSupers(), findActive(), isCatalogName(), KeptClass, category, classVars (+10 more)

### Community 125 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 126 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 127 - "TEST (20)"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 128 - "TEST (21)"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 129 - "TEST (7) (2)"
Cohesion: 0.21
Nodes (16): classVarLiterals(), countOp(), MethodImage, Op, string, uint8_t, decode(), firstBlock() (+8 more)

### Community 130 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 131 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 132 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 133 - "ChunkAction"
Cohesion: 0.12
Nodes (16): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+8 more)

### Community 134 - "ChunkParser.cpp"
Cohesion: 0.30
Nodes (15): classify(), string_view, firstLineHas(), isBlank(), isCharacterLiteral(), isLetter(), isProseApostrophe(), isSoleDefinition() (+7 more)

### Community 135 - "P3 — ネイティブディスパッチ"
Cohesion: 0.16
Nodes (15): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary (+7 more)

### Community 136 - "PingPong (3)"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

### Community 137 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 138 - "TEST (22)"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 139 - "TEST (23)"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 140 - "TEST (24)"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 141 - "MethodDebugInfo"
Cohesion: 0.15
Nodes (15): uint32_t, PcSpan, end, pc, start, DebugInfoRef, body, index (+7 more)

### Community 142 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 143 - "TEST (5) (2)"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 144 - "Claude Review Fixes Plan (2026-09-23) (2)"
Cohesion: 0.16
Nodes (14): B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12), MappedCollection absent from pinned sources (+6 more)

### Community 145 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 146 - "TEST_F (7)"
Cohesion: 0.14
Nodes (14): BusyRemove, acceptRc, busyId, classMsg, classRc, goneHeldAfterRead, goneHeldBeforeRead, goneId (+6 more)

### Community 147 - "TEST_F (8)"
Cohesion: 0.14
Nodes (14): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), reenterEverything() helper, Reentry (+6 more)

### Community 148 - "TEST (27)"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 149 - "Method Removal Rules (3)"
Cohesion: 0.21
Nodes (12): version_string(), package-app.sh script, Acceptance Criteria (§6), AcceptTests Remove/ContextMenu XCTests, CI (GitHub Actions), Golden Evaluation (ao --test), Info.plist CFBundleShortVersionString/CFBundleVersion from ao --version (1.0.0 for v1, 1.1.0 through P12), GitHub Release assets (Ao-<ver>-macos-arm64.zip, ao-cli-<ver>-macos-arm64.tar.gz, SHA256SUMS) (+4 more)

### Community 150 - "TEST (28)"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 151 - "TEST (26)"
Cohesion: 0.17
Nodes (12): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), KnownGlobalAssignIsError, KnownGlobalReadIsPushGlobal (+4 more)

### Community 152 - "Ao Smalltalk 概要 (JA)"
Cohesion: 0.23
Nodes (12): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target), リリース手順 (JA), Releasing Process (EN), CLAUDE.md, Kernel Scan Test (via ctest) (+4 more)

### Community 153 - "Ao 実装文書 index (docs/README.md)"
Cohesion: 0.26
Nodes (12): P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+4 more)

### Community 154 - "Ao 実装文書 index (docs/README.md) (2)"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 155 - "P8 — AppKit ツール"
Cohesion: 0.20
Nodes (12): P9 — 統合, P9-01: Do it / Print it / Inspect it, Do it / Print it / Inspect it, P9-02: Browser accept, Browser accept, P9-03: エラー表示と VoiceOver, エラー表示と VoiceOver, P9-04 v1 Golden Acceptance (+4 more)

### Community 156 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 157 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 158 - "Kernel Classes (NativeMethod required)"
Cohesion: 0.17
Nodes (12): Bootstrap Procedure, Constraints (§5), Cuis Smalltalk, Graphify and Serena Requirement, 16-bit Identity Hash, Kernel Classes (native-only), NativeMethod, Performance Policy (native hot paths) (+4 more)

### Community 159 - "Method Removal Rules"
Cohesion: 0.22
Nodes (11): CHANGELOG: [1.0.0] first release, CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27), CHANGELOG: P10/P11 post-mortem and live debugger entry, 1.1.0 release assets (Ao-1.1.0-macos-arm64.zip, ao-cli-1.1.0-macos-arm64.tar.gz, SHA256SUMS), P10-P12 released as 1.1.0, v1 = P9 completion, released as 1.0.0, v1 Status Table, .aoimage Image Format (+3 more)

### Community 160 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 161 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 162 - "ImageFormat (2)"
Cohesion: 0.18
Nodes (9): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), TEST() (+1 more)

### Community 163 - "TEST (10)"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 164 - "TEST (29)"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 165 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 166 - "ao_eval (2)"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 167 - "codegen"
Cohesion: 0.22
Nodes (9): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+1 more)

### Community 168 - "ChunkParser.cpp (2)"
Cohesion: 0.22
Nodes (9): string, Token, uint32_t, RawChunk, endsSection, span, text, undoubled (+1 more)

### Community 169 - "cstring"
Cohesion: 0.25
Nodes (7): csignal, mach, mach_vm, pingPongFiber(), step(), set, unistd

### Community 170 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.25
Nodes (8): いまの位置 (1.1.0、PHASE は P12), Where the project stands (1.1.0, PHASE reads P12), PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md, Change Rules (§8)

### Community 171 - "send2"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 172 - "Vendor.hpp"
Cohesion: 0.25
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 173 - "synthesizesNative"
Cohesion: 0.44
Nodes (9): CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget(), sentToSuper() (+1 more)

### Community 174 - "VendorExtract.cpp (4)"
Cohesion: 0.39
Nodes (9): string, doubleBangs(), HostMethod, protocol, source, quoteSmalltalk(), render(), trimTrailingWs() (+1 more)

### Community 175 - "ao_runtime_boot (2)"
Cohesion: 0.25
Nodes (8): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionStringIsReleaseOneOneZero, VersionTruncationIsRangeError

### Community 176 - "WorkspaceEvalTests"
Cohesion: 0.29
Nodes (7): sendToKeyWorkspace(), Bool, MainActor, NSWindow, Void, WorkspaceButtonAction, NSObject

### Community 177 - "TEST (30)"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 178 - "Class Removal Rules (2)"
Cohesion: 0.25
Nodes (8): Build only what SPEC.md has (v1 and later §2.3 phases), Ao Bytecode Set, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation), Blue Book Conformance Definition, Broken Superclass Chain Rules, Message Send Semantics, PushGlobal bytecode

### Community 179 - "CallContext (2)"
Cohesion: 0.25
Nodes (8): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, uint32_t, invalidateMethodCache()

### Community 180 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 181 - "ChunkAction (2)"
Cohesion: 0.29
Nodes (7): ChunkMethod, source, span, undoubled, string, uint32_t, fileSpan()

### Community 182 - "ImageLoad.cpp (3)"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 183 - "WellKnown.cpp (2)"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 184 - "Method Removal Rules (3) (2)"
Cohesion: 0.40
Nodes (6): Method dictionary removal (nil pair, decrement tally), Kernel Scan Test (KernelScan.MethodDictionaryValuesAreNativeMethods), ao_remove_method Rules, invalidateMethodCache(cache, selector), Linear MethodDictionary (interleaved keys/values), Method Removal Design

### Community 185 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 186 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 187 - "Heap (3)"
Cohesion: 0.33
Nodes (6): uint16_t, ObjectHeader, flags, hash, klass, size

### Community 188 - "WellKnown.cpp (3)"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 189 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 190 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 191 - "VendorExtract.cpp (5)"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 192 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 193 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 194 - "Ao Smalltalk 概要 (JA) (2)"
Cohesion: 0.67
Nodes (3): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps)

### Community 195 - "TEST (32)"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 196 - "codegen (2)"
Cohesion: 0.50
Nodes (4): CompileResult, error, image, ok

### Community 197 - "TEST (12) (2)"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 198 - "compiler/tests/smoke_test.cpp"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneOneZero

### Community 199 - "CallContext (3)"
Cohesion: 0.50
Nodes (3): DebugSink, Frame, Scheduler

### Community 200 - "CallContext (4)"
Cohesion: 0.50
Nodes (4): Entry, klass, method, selector

### Community 201 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 202 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 203 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 204 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 205 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 206 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 209 - "Post-mortem debugger"
Cohesion: 0.67
Nodes (3): ao_set_debug_capture / ao_debug_*, Object>>halt, Post-mortem debugger

### Community 210 - "Live debugger"
Cohesion: 0.67
Nodes (3): Ao.app Debugger window, Live debugger, ao_set_debug_mode(AO_DEBUG_LIVE) / AO_ERR_HALT

### Community 211 - "Rules That Do Not Bend"
Cohesion: 0.67
Nodes (3): Fixed Design Decisions, 曲げない規則, Rules That Do Not Bend

### Community 212 - "string (2)"
Cohesion: 0.67
Nodes (3): RemoveUnits, TEST(), UndefineUnbindsAndMovesTheGlobalsVersion

### Community 215 - "Stream.cpp (2)"
Cohesion: 0.67
Nodes (3): Roots, uint32_t, RootedArray::RootedArray()

### Community 217 - "Scheduler::Record (2)"
Cohesion: 0.67
Nodes (3): size_t, Scheduler::haltedCount(), Scheduler::liveFibers()

### Community 218 - "WellKnown.cpp (4)"
Cohesion: 0.67
Nodes (3): NamedClass, name, WellKnown

### Community 219 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Ambiguous Edges - Review These
- `Smalltalk-80 removeFromSystem` → `Blue Book (Smalltalk-80: The Language and its Implementation)`  [AMBIGUOUS]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md · relation: conceptually_related_to
- `P5-01: 字句解析` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references
- `P7-03: load と NativeMethod 再結合` → `P8-01: Ao.app 骨格`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to
- `SPEC §3.13 デバッガ（捕捉の意味論）` → `Debug it (AO_EVAL_DEBUGIT)`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `P8-05: メニューとキー` → `P9-01: Do it / Print it / Inspect it`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references

## Knowledge Gaps
- **990 isolated node(s):** `CallContext`, `CallContext`, `CallContext`, `CallContext`, `Root` (+985 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2641 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **150 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Smalltalk-80 removeFromSystem` and `Blue Book (Smalltalk-80: The Language and its Implementation)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P5-01: 字句解析` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P7-03: load と NativeMethod 再結合` and `P8-01: Ao.app 骨格`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `SPEC §3.13 デバッガ（捕捉の意味論）` and `Debug it (AO_EVAL_DEBUGIT)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
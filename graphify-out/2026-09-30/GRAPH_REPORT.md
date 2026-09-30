# Graph Report - ao-smalltalk  (2026-09-30)

## Corpus Check
- 301 files · ~352,183 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 6263 nodes · 16653 edges · 325 communities (208 shown, 117 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 2301 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `a2540b2a`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Compile.cpp
- TEST_F
- TEST
- abi.cpp
- Dictionary.cpp
- TEST
- string
- DebuggerWindow
- TEST_F
- Object.cpp
- .fromSmallInteger
- Parser
- LargeInteger.cpp
- AcceptTests
- BrowserWindow
- DebuggerWindowTests
- Emitter
- Stream.cpp
- TEST
- Scanner.cpp
- ImageSave.cpp
- アプリ
- TEST
- Boot
- Heap
- TEST_F
- TEST
- vector
- TEST
- Scheduler.cpp
- Heap.cpp
- TEST
- TEST
- ChunkParser.cpp
- Session
- Analysis
- TranscriptWindow
- Gc.cpp
- Float.cpp
- Behavior.cpp
- Ast
- BrowserModel
- CallContext
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- clearUnwinding
- DebugFrames
- TEST_F
- ImageLoad.cpp
- ao_request_interrupt
- TEST
- ToolWindowTests
- TEST
- classRows
- TEST_F
- TEST_F
- String.cpp
- TEST
- string_view
- MethodSource
- Send.cpp
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- ao_accept_method
- TEST
- Ao 実装文書 index (docs/README.md)
- DebugSnapshot
- Record
- .isHeap
- TEST
- DebugSnapshot.cpp
- Geometry.cpp
- VendorExtract.cpp
- TEST
- Scheduler
- AoApp
- DiskHeader
- collectNursery
- P12 Browser Removal Design
- Roots
- TEST
- Claude Review Fixes Plan (2026-09-23)
- Boolean.cpp
- ClassPool.cpp
- FiberStack
- P6b Vendor File-in Implementation Plan
- Oop
- Scheduler::Record
- Fiber.cpp
- TEST
- Literal
- ChunkAction
- Session.cpp
- Gc
- LiveFrames
- .base
- BlockContext.cpp
- TEST_F
- WorkspaceWindow
- BrowserModelTests
- Debug it (AO_EVAL_DEBUGIT)
- PingPong
- TEST
- TEST
- TEST
- NativeMethod.cpp
- TEST
- .build
- string
- TEST
- Roots.cpp
- uint64_t
- HashedCollection.cpp
- AppKit
- DefinitionScanner
- TEST
- P12 — Browser の削除 (phase doc)
- CallContext
- TEST
- Process.cpp
- ao_main.cpp
- ImageSurgery
- .nil
- ImageFormat
- KeptClass
- TEST
- native_method_test.cpp
- TEST
- TEST
- GarbageFirstBoot
- CountingSink
- NativeFrame
- P3 — ネイティブディスパッチ
- P12 追補: クラス ID と壊れたメソッド辞書 実装計画
- FiberRegs
- P3 — ネイティブディスパッチ
- KeptMethod
- takeAbortReason
- TEST
- CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27)
- IgnoreFileSizeSignal
- MethodDebugInfo
- TEST
- Frame
- string
- Loaded
- ClassId
- Deferred vendor methods list
- OperandStack
- BusyRemove
- Interpreter::run
- Scheduler::addFiber
- TEST
- Object Memory and Generational GC
- Ao Smalltalk 概要 (JA)
- .isTrue
- 設計判断
- Interpreter.cpp
- path
- Character.cpp
- TEST
- P0 (親フェーズ, stub)
- P1 — オブジェクトメモリ
- TEST
- FrameBlock
- string
- ao_eval (2)
- HashedCollection
- heap
- isKernelClass
- interpreter
- parser
- roots
- runtime_src_session
- string_view
- TEST
- vector
- VendorClassFile
- contextPc
- TEST
- ObjectHeader
- P5 — コンパイラ
- fileInLoadOrder
- TEST
- BrowserWindow.removeAfterConfirming(...)
- TEST
- SelectedFrames
- SuperclassWalk
- ClassMethodCache
- Counts
- ScopedGcStressEnv
- P11 — ライブデバッガ
- create
- ClassDef
- WellKnown.cpp
- abortingSubclass
- FileSizeLimit
- Rec
- HashNesting
- SlotNames
- cli_test.sh
- popFrame
- abortingNew
- TestDir
- test.sh
- TEST
- NativeMethod.hpp
- Roots::visitAll
- Root
- imageRegistryStubA
- answerOne
- Post-mortem debugger
- Live debugger
- Rules That Do Not Bend
- .specialSelector
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
- uint64_t
- BrowserModel.applyHierarchyList(_:selecting:)
- BrowserModel.hierarchy(of:meta:)
- ao_browser_class_count
- ao_browser_subclass_count
- ao_eval
- 1.0.0 release
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
- reshapeClass
- CarriedMethod
- runtime_src_fiber
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml

## God Nodes (most connected - your core abstractions)
1. `Oop` - 954 edges
2. `Heap` - 255 edges
3. `vector` - 179 edges
4. `WellKnown` - 168 edges
5. `BrowserWindow` - 125 edges
6. `TEST_F()` - 123 edges
7. `Session` - 112 edges
8. `TEST()` - 112 edges
9. `Ast` - 103 edges
10. `TEST()` - 101 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `TDD` --references--> `DebuggerWindowTests`  [INFERRED]
  docs/phases/P11.md → app/AoTests/DebuggerWindowTests.swift
- `前提` --references--> `DebugFrames`  [INFERRED]
  docs/phases/P11.md → runtime/include/ao/DebugSnapshot.hpp
- `Execution notes` --references--> `jumpTo()`  [INFERRED]
  docs/superpowers/plans/2026-09-30-evaluation-interrupt.md → runtime/src/Interpreter.cpp
- `仕様` --references--> `abortEvaluation()`  [INFERRED]
  docs/phases/P10.md → runtime/src/Send.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **SPEC 3.10 class ID scheme: stable IDs across Browser rows, ABI, and Session table** — app_ao_browsermodel_browserclass, app_ao_browsermodel_classid, bridge_ao_abi_ao_browser_class_id, runtime_src_session_browserclassid, runtime_src_session_sessionclassforid, runtime_src_session_assignclassids, runtime_src_session_classid [EXTRACTED 0.90]
- **SPEC 3.9 removal flow: Browser UI to method dictionary and source table** — app_ao_browserwindow_removemethod, bridge_ao_abi_ao_remove_method, runtime_include_ao_compile_removemethodof, runtime_src_compile_removemethodof_impl, runtime_include_ao_methoddictionary_removekey, runtime_src_session_forgetmethodsource [EXTRACTED 0.95]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **Method dictionary mutation paths sharing single cache-invalidation function** — spec_ao_accept_method, spec_ao_accept_method_id, spec_ao_remove_method, spec_ao_remove_class, spec_method_cache [EXTRACTED 1.00]
- **P12 removal flow: Browser UI -> class ID -> C ABI -> dictionary/cache/source-table updates** — spec_browser_remove_ui, spec_section_3_10_classid, spec_ao_remove_method, spec_ao_remove_class, spec_method_cache, spec_source_table [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **Session tables excluded from liveness tracing and image save** — spec_method_cache, spec_source_table, spec_section_3_10_classid, spec_debugger, spec_live_subclass [EXTRACTED 1.00]
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
- **1.1.0 release versioning contract (semver minor bump, version_string, assets, Info.plist)** — spec_minor_version_bump_post_v1, spec_release_semver, spec_version_string_source_of_truth, spec_release_assets, spec_info_plist_version, changelog_1_1_0_release [INFERRED 0.85]
- **Session Class ID Identity Mechanism** — runtime_src_session_classids, runtime_src_session_classrows, runtime_src_session_browserclassid, bridge_ao_abi_ao_browser_class_id, runtime_tests_browser_abi_test_browserabi, docs_superpowers_plans_2026_09_27_p12_class_identity [INFERRED 0.85]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **MethodDictionary Malformed-Shape Defense** — runtime_src_methoddictionary_pairarray, runtime_src_methoddictionary_at, runtime_src_methoddictionary_atput, runtime_src_methoddictionary_removekey, runtime_tests_method_dictionary_test_malformeddictionaryanswersnilandtakesnothing, runtime_tests_remove_abi_test_malformedmethoddictionaryfallsbackonsend [INFERRED 0.90]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]

## Communities (325 total, 117 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 1 - "Compile.cpp"
Cohesion: 0.14
Nodes (37): cctype, CompileError, message, span, string, acceptClassSource, acceptClassSource(), acceptClassSource (definition) (+29 more)

### Community 2 - "TEST_F"
Cohesion: 0.03
Nodes (89): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+81 more)

### Community 3 - "TEST"
Cohesion: 0.02
Nodes (100): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+92 more)

### Community 4 - "abi.cpp"
Cohesion: 0.03
Nodes (131): AoTranscriptFn, atomic, Body, ao_accept_method_id, ao_browser_class_id, ao_remove_class, ao_remove_method, AoSpan (+123 more)

### Community 5 - "Dictionary.cpp"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 6 - "TEST"
Cohesion: 0.02
Nodes (80): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+72 more)

### Community 7 - "string"
Cohesion: 0.09
Nodes (44): algorithm, Bootstrap, BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, chrono, Chunk, classpool, cmath (+36 more)

### Community 8 - "DebuggerWindow"
Cohesion: 0.05
Nodes (48): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+40 more)

### Community 9 - "TEST_F"
Cohesion: 0.02
Nodes (88): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+80 more)

### Community 10 - "Object.cpp"
Cohesion: 0.12
Nodes (52): Graphify / Serena, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 11 - ".fromSmallInteger"
Cohesion: 0.10
Nodes (55): int64_t, CallContext, uint8_t, createBlock(), createMethod(), bumpGeneration(), ao_ArrayedCollection_do_(), ao_Collection_collect_() (+47 more)

### Community 12 - "Parser"
Cohesion: 0.11
Nodes (22): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+14 more)

### Community 13 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 14 - "AcceptTests"
Cohesion: 0.12
Nodes (13): spanMessage(), AcceptTests, Bool, Int32, NSMenu, NSSegmentedControl, NSTableView, NSTextView (+5 more)

### Community 15 - "BrowserWindow"
Cohesion: 0.05
Nodes (38): BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText (+30 more)

### Community 16 - "DebuggerWindowTests"
Cohesion: 0.10
Nodes (17): DebuggerWindowTests, Int, NSButton, NSFont, NSTableView, NSTextView, NSView, NSWindow (+9 more)

### Community 17 - "Emitter"
Cohesion: 0.13
Nodes (16): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+8 more)

### Community 18 - "Stream.cpp"
Cohesion: 0.09
Nodes (74): allocateInstance(), allocateRetry(), CallContext, Roots, uint16_t, uint32_t, RootedArray::RootedArray(), ao_PositionableStream_contents() (+66 more)

### Community 19 - "TEST"
Cohesion: 0.04
Nodes (53): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+45 more)

### Community 20 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 21 - "ImageSave.cpp"
Cohesion: 0.10
Nodes (42): cerrno, climits, fcntl, appendRaw(), appendRecord(), collectImageSlot(), byte, Roots (+34 more)

### Community 22 - "アプリ"
Cohesion: 0.07
Nodes (35): AnyObject, InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable (+27 more)

### Community 23 - "TEST"
Cohesion: 0.04
Nodes (59): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+51 more)

### Community 24 - "Boot"
Cohesion: 0.04
Nodes (57): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+49 more)

### Community 25 - "Heap"
Cohesion: 0.04
Nodes (49): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, Heap (+41 more)

### Community 26 - "TEST_F"
Cohesion: 0.06
Nodes (48): AliasedOldClassRowActsOnItsOwnClass, ClassIdTableDoesNotBlockSuperclassRemoval, KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, MalformedMethodDictionaryFallsBackOnSend, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad, RemoveAcceptedMethodOnKernelClass (+40 more)

### Community 27 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 28 - "vector"
Cohesion: 0.06
Nodes (30): string_view, vector, uint32_t, strictlyAscending(), csignal, cstddef, cstdint, mach (+22 more)

### Community 29 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 30 - "Scheduler.cpp"
Cohesion: 0.17
Nodes (36): flags, slotAt, slotAtPut, addFiber, liveFibers, CallContext, int64_t, Root (+28 more)

### Community 31 - "Heap.cpp"
Cohesion: 0.09
Nodes (43): charconv, allocateTenured, fitsOld, growOld, initObject, objectBytes, oldUsed, align8() (+35 more)

### Community 32 - "TEST"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 33 - "TEST"
Cohesion: 0.05
Nodes (45): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+37 more)

### Community 34 - "ChunkParser.cpp"
Cohesion: 0.18
Nodes (23): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+15 more)

### Community 35 - "Session"
Cohesion: 0.05
Nodes (44): removeMethodOf, MethodDictionary::removeKey, removeMethodOf (definition), classIdRootSlots(), HostOopHook, Roots, debugCanProceed(), debugFrameCount() (+36 more)

### Community 36 - "Analysis"
Cohesion: 0.06
Nodes (32): Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, CompileResult (+24 more)

### Community 37 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+20 more)

### Community 38 - "Gc.cpp"
Cohesion: 0.21
Nodes (10): cstdlib, Roots, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::Gc() (+2 more)

### Community 39 - "Float.cpp"
Cohesion: 0.05
Nodes (104): NumberOp, NumberRelation, NumKind, WellKnown, installArray(), WellKnown, installBoolean(), WellKnown (+96 more)

### Community 40 - "Behavior.cpp"
Cohesion: 0.18
Nodes (30): MethodDictionary::at, MethodDictionary::pairArray, ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes() (+22 more)

### Community 41 - "Ast"
Cohesion: 0.05
Nodes (57): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+49 more)

### Community 42 - "BrowserModel"
Cohesion: 0.17
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 43 - "CallContext"
Cohesion: 0.05
Nodes (42): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+34 more)

### Community 44 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 45 - "clearUnwinding"
Cohesion: 0.13
Nodes (24): Image, check, load, save, refreshStackLimit(), boot(), clearUnwinding(), clearMethodSources() (+16 more)

### Community 46 - "DebugFrames"
Cohesion: 0.08
Nodes (42): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context, count, empty, kind, method, pc (+34 more)

### Community 47 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 48 - "ImageLoad.cpp"
Cohesion: 0.12
Nodes (45): ifstream, readHeader, acceptWord(), atOffset(), bindAll(), checkFile(), checkGlobals(), byte (+37 more)

### Community 49 - "ao_request_interrupt"
Cohesion: 0.09
Nodes (30): Execution notes, File Structure, Global Constraints, P13 評価の中断 — 実装計画, Review Focus, Task 1: SPEC を P13 に更新する, Task 3: インタプリタ safepoint で中断停止（テスト緑）, Task 4: 追加の runtime エッジケース (+22 more)

### Community 50 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 51 - "ToolWindowTests"
Cohesion: 0.11
Nodes (13): fileInVendor(), LaunchSet, RunLoopPump, NSFont, NSMenu, NSMenuItem, NSTextView, NSView (+5 more)

### Community 52 - "TEST"
Cohesion: 0.05
Nodes (45): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+37 more)

### Community 53 - "classRows"
Cohesion: 0.14
Nodes (36): assignClassIds(), browserClassAt(), browserClassDefinition(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+28 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (32): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+24 more)

### Community 55 - "TEST_F"
Cohesion: 0.06
Nodes (40): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+32 more)

### Community 56 - "String.cpp"
Cohesion: 0.15
Nodes (37): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), ao_String_asSymbol(), ao_String_at_() (+29 more)

### Community 57 - "TEST"
Cohesion: 0.15
Nodes (13): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, MalformedDictionaryAnswersNilAndTakesNothing, NinthMethodWithFullNurseryIsInstalled, RemoveKeyLeavesTheOthersFindableAndReusesThePair (+5 more)

### Community 58 - "string_view"
Cohesion: 0.11
Nodes (22): findSymbol, global, internWith, isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown (+14 more)

### Community 59 - "MethodSource"
Cohesion: 0.08
Nodes (32): P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）, リスク, 手順（PR の順序と依存）, 検証, 範囲, 結論, attachBlocks(), blockLiteral() (+24 more)

### Community 60 - "Send.cpp"
Cohesion: 0.10
Nodes (40): Graphify / Serena, `Object>>halt`（P10-03）, SPEC を先に直す（P10-01）, フレーム連鎖（P10-03）, 仕様, 捕捉（P10-03、配線は P10-04）, run, InlineCache (+32 more)

### Community 61 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.06
Nodes (66): P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 範囲, 結論 (+58 more)

### Community 62 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 63 - "ao_accept_method"
Cohesion: 0.08
Nodes (24): BrowserModel.classID(named:), BrowserModel.copyClass(at:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.removeClass / removeClass(atRow:) (+16 more)

### Community 64 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 65 - "Ao 実装文書 index (docs/README.md)"
Cohesion: 0.07
Nodes (60): P4 — Kernelネイティブ実装, vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, P8 — AppKit ツール, P9 — 統合, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装 (+52 more)

### Community 66 - "DebugSnapshot"
Cohesion: 0.06
Nodes (28): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+20 more)

### Community 67 - "Record"
Cohesion: 0.12
Nodes (29): afterResume, canHalt, endEval, enqueue, find, leaveLists, reapDead, recordFailure (+21 more)

### Community 68 - ".isHeap"
Cohesion: 0.13
Nodes (27): RemoveUnits, allocateNoGc, klass, size, at(), bind(), bindIn(), Roots (+19 more)

### Community 69 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 70 - "DebugSnapshot.cpp"
Cohesion: 0.16
Nodes (26): frameAt, uint32_t, DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method(), DebugSnapshot::pc(), DebugSnapshot::process(), DebugSnapshot::receiver() (+18 more)

### Community 71 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 72 - "VendorExtract.cpp"
Cohesion: 0.20
Nodes (22): allowIndex(), containsHostWord(), string_view, extractVendor(), firstLineKey(), firstNonEmptyLine(), hostPatch(), isCatalogName() (+14 more)

### Community 73 - "TEST"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 74 - "Scheduler"
Cohesion: 0.04
Nodes (44): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+36 more)

### Community 75 - "AoApp"
Cohesion: 0.12
Nodes (14): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+6 more)

### Community 76 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 77 - "collectNursery"
Cohesion: 0.36
Nodes (10): collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld() (+2 more)

### Community 78 - "P12 Browser Removal Design"
Cohesion: 0.07
Nodes (29): Context menu flow (discard confirm -> select row -> remove confirm -> ABI), ao_accept_class, ao_accept_method, Ao Bytecode Set, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation), Blue Book Conformance Definition, Browser Remove UI (context menu, confirmation sheet) (+21 more)

### Community 79 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 80 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 81 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.11
Nodes (27): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+19 more)

### Community 82 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 83 - "ClassPool.cpp"
Cohesion: 0.20
Nodes (24): hashedcollection, adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view (+16 more)

### Community 84 - "FiberStack"
Cohesion: 0.10
Nodes (26): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+18 more)

### Community 85 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.11
Nodes (26): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+18 more)

### Community 86 - "Oop"
Cohesion: 0.09
Nodes (42): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), valueHashWord() (+34 more)

### Community 87 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, string, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+20 more)

### Community 88 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 89 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 90 - "Literal"
Cohesion: 0.06
Nodes (38): int16_t, int64_t, LitKind, string, uint16_t, uint32_t, uint8_t, unique_ptr (+30 more)

### Community 91 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 92 - "Session.cpp"
Cohesion: 0.09
Nodes (35): boxMethodImage, shutdown(), answerAwaited(), answerEval(), blankOut(), browserClassCount(), bumpDebugGeneration(), clearInterruptRequest() (+27 more)

### Community 93 - "Gc"
Cohesion: 0.20
Nodes (12): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress (+4 more)

### Community 94 - "LiveFrames"
Cohesion: 0.09
Nodes (20): CallContext, DebugSink, onAbort, Frame, size_t, string, LiveFrames, context (+12 more)

### Community 95 - ".base"
Cohesion: 0.13
Nodes (24): abandonAll, awaitEval, drain, findId, isHalted, proceed, runAwaited, terminate (+16 more)

### Community 96 - "BlockContext.cpp"
Cohesion: 0.17
Nodes (26): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+18 more)

### Community 97 - "TEST_F"
Cohesion: 0.05
Nodes (39): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+31 more)

### Community 98 - "WorkspaceWindow"
Cohesion: 0.10
Nodes (15): sendToKeyWorkspace(), Bool, NSWindow, WorkspaceWindow, .errorAccessibilityLabel, .errorText, .inspectorCount, .inspectorText (+7 more)

### Community 99 - "BrowserModelTests"
Cohesion: 0.18
Nodes (10): BrowserModelTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+2 more)

### Community 100 - "Debug it (AO_EVAL_DEBUGIT)"
Cohesion: 0.12
Nodes (23): P11 known limitations, Known limitation: sourceless frames have no statement starts (Step over/into skip statements), Debug it (AO_EVAL_DEBUGIT), MainMenu.swift: Debug it ⌘⇧D, README Debugger usage (Proceed, Step over/into/out, Abort), v1 に無いもの, Not in v1 (Non-goals), Host GUI (AppKit) (+15 more)

### Community 101 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 102 - "TEST"
Cohesion: 0.13
Nodes (19): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, firstBlock(), named(), pcOf() (+11 more)

### Community 103 - "TEST"
Cohesion: 0.08
Nodes (26): BlockWithArgs, Cascade, ParseResult, error, method, ok, parseMethod(), BlockArgumentsThenTemps (+18 more)

### Community 104 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 105 - "NativeMethod.cpp"
Cohesion: 0.32
Nodes (15): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+7 more)

### Community 106 - "TEST"
Cohesion: 0.09
Nodes (23): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+15 more)

### Community 107 - ".build"
Cohesion: 0.20
Nodes (10): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+2 more)

### Community 108 - "string"
Cohesion: 0.39
Nodes (9): string, doubleBangs(), HostMethod, protocol, source, quoteSmalltalk(), render(), trimTrailingWs() (+1 more)

### Community 109 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 110 - "Roots.cpp"
Cohesion: 0.11
Nodes (20): cassert, new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack() (+12 more)

### Community 111 - "uint64_t"
Cohesion: 0.28
Nodes (13): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+5 more)

### Community 112 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (29): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+21 more)

### Community 113 - "AppKit"
Cohesion: 0.10
Nodes (17): Ao, aoRunLoopPumpHook(), EvaluationActivity, T, UnsafeMutableRawPointer, interruptOncePump(), CChar, Int32 (+9 more)

### Community 114 - "DefinitionScanner"
Cohesion: 0.34
Nodes (7): DefinitionScanner, Bool, Token, keyword, other, Equatable, Unicode

### Community 115 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 116 - "P12 — Browser の削除 (phase doc)"
Cohesion: 0.08
Nodes (46): P12 — Browser の削除 (phase doc), Addendum tests (browser_abi_test, accept_abi_test, session_abi_test, method_dictionary_test, BrowserModelTests), P12 Addendum: Class ID and Broken Method Dictionary (2026-09-27), Method dictionary removal (nil pair, decrement tally), MethodDictionary::pairArray (shape check), Class removal unbinds name only, BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove (+38 more)

### Community 117 - "CallContext"
Cohesion: 0.15
Nodes (15): ActiveGuard, rootShared, saved, contextAlive(), ContextExitGuard, CallContext, Frame, Roots (+7 more)

### Community 118 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 119 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 120 - "ao_main.cpp"
Cohesion: 0.23
Nodes (14): dyld, runtime, addRoots, bootAndRunTests(), string, imageUsage(), main(), printFileInErrors() (+6 more)

### Community 121 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 122 - ".nil"
Cohesion: 0.19
Nodes (17): allocateSkeletons(), allocClass(), Roots, string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors() (+9 more)

### Community 123 - "ImageFormat"
Cohesion: 0.10
Nodes (21): uint16_t, uint32_t, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes (+13 more)

### Community 124 - "KeptClass"
Cohesion: 0.21
Nodes (17): size_t, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars, deferred (+9 more)

### Community 125 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 126 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 127 - "TEST"
Cohesion: 0.05
Nodes (40): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, ClassDefinitionThroughAliasOnlyRebindsGlobal (+32 more)

### Community 128 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 129 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 130 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 131 - "NativeFrame"
Cohesion: 0.50
Nodes (4): Roots, NativeFrame, argc, slots

### Community 132 - "P3 — ネイティブディスパッチ"
Cohesion: 0.09
Nodes (26): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス (+18 more)

### Community 133 - "P12 追補: クラス ID と壊れたメソッド辞書 実装計画"
Cohesion: 0.23
Nodes (15): P12 追補: クラス ID と壊れたメソッド辞書 実装計画, Design rationale: session class ID table, Design rationale: MethodDictionary shape check (pairArray), at(), atPut(), uint32_t, WellKnown, create() (+7 more)

### Community 134 - "FiberRegs"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

### Community 135 - "P3 — ネイティブディスパッチ"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 136 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 137 - "takeAbortReason"
Cohesion: 0.11
Nodes (22): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+14 more)

### Community 138 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 139 - "CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27)"
Cohesion: 0.06
Nodes (39): CHANGELOG: [1.0.0] first release, CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27), 1.1.0 install notes (macOS 14+, shasum check, Open Anyway / xattr quarantine), CHANGELOG: P10/P11 post-mortem and live debugger entry, 1.1.0 release assets (Ao-1.1.0-macos-arm64.zip, ao-cli-1.1.0-macos-arm64.tar.gz, SHA256SUMS), Build only what SPEC.md has (v1 and later §2.3 phases), いまの位置 (1.1.0、PHASE は P12), Where the project stands (1.1.0, PHASE reads P12) (+31 more)

### Community 140 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 141 - "MethodDebugInfo"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 142 - "TEST"
Cohesion: 0.15
Nodes (14): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, CallContext, Lookup, NativeFn, uint32_t, WellKnown (+6 more)

### Community 143 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 144 - "string"
Cohesion: 0.28
Nodes (15): byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), string, WellKnown, definitionCategory() (+7 more)

### Community 145 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 146 - "ClassId"
Cohesion: 0.50
Nodes (4): ClassId, cls, id, int64_t

### Community 147 - "Deferred vendor methods list"
Cohesion: 0.16
Nodes (14): B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12), MappedCollection absent from pinned sources (+6 more)

### Community 148 - "OperandStack"
Cohesion: 0.12
Nodes (14): deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots, Temps (+6 more)

### Community 149 - "BusyRemove"
Cohesion: 0.14
Nodes (14): BusyRemove, acceptRc, busyId, classMsg, classRc, goneHeldAfterRead, goneHeldBeforeRead, goneId (+6 more)

### Community 150 - "Interpreter::run"
Cohesion: 0.29
Nodes (13): byteCount(), int16_t, uint32_t, uint8_t, decodeHeader(), instSlot(), Interpreter::run(), jumpTo() (+5 more)

### Community 151 - "Scheduler::addFiber"
Cohesion: 0.20
Nodes (11): terminateAll, Roots, size_t, nextProcessId(), rootRecord(), Scheduler::addFiber(), Scheduler::haltedCount(), Scheduler::liveFibers() (+3 more)

### Community 152 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 153 - "Object Memory and Generational GC"
Cohesion: 0.20
Nodes (10): Failure Aborts Evaluation, allocateRetry, ao_image_load, AoSpan.message (error reason), Base Process, Cooperative Process Scheduler (fibers), Full GC Triggers, Non-local Return, ensure:, abort (+2 more)

### Community 154 - "Ao Smalltalk 概要 (JA)"
Cohesion: 0.08
Nodes (26): v1 known limitations, Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps), AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target) (+18 more)

### Community 155 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 156 - "設計判断"
Cohesion: 0.22
Nodes (9): §3.13 停止表への行, ABI: RunLoop pump フック, ABI: 中断要求, Proceed のあと, フラグ確認と pump の頻度, 再入, 検査点（セーフポイント）, 設計判断 (+1 more)

### Community 157 - "Interpreter.cpp"
Cohesion: 0.23
Nodes (14): boolean(), branchTruth(), clearNonlocal(), consumeNonlocal(), int64_t, DepthGuard, outermost, hit() (+6 more)

### Community 158 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 159 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 160 - "TEST"
Cohesion: 0.09
Nodes (27): BlockAssignmentIsBindingStore, BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), string (+19 more)

### Community 161 - "P0 (親フェーズ, stub)"
Cohesion: 0.26
Nodes (12): P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+4 more)

### Community 162 - "P1 — オブジェクトメモリ"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 163 - "TEST"
Cohesion: 0.25
Nodes (8): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionStringIsReleaseOneOneZero, VersionTruncationIsRangeError

### Community 164 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 165 - "string"
Cohesion: 0.14
Nodes (26): ソース表の拡張（P10-04）, removeClassOf, isClassShaped(), superclassOf(), pair, string, string_view, deferredListing() (+18 more)

### Community 166 - "ao_eval (2)"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 167 - "HashedCollection"
Cohesion: 0.29
Nodes (4): string, testing::Test, HashedCollection, randomProbeSource()

### Community 169 - "isKernelClass"
Cohesion: 0.33
Nodes (6): acceptMethodSource, MethodDictionary::atPut, acceptMethodInto (definition), WellKnown, installMethod (definition), isKernelClass()

### Community 175 - "TEST"
Cohesion: 0.15
Nodes (11): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, testing::Test, KernelNumeric (+3 more)

### Community 177 - "VendorClassFile"
Cohesion: 0.25
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 178 - "contextPc"
Cohesion: 0.44
Nodes (10): contextPc(), CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget() (+2 more)

### Community 179 - "TEST"
Cohesion: 0.11
Nodes (23): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, answerMessage(), CallContext (+15 more)

### Community 180 - "ObjectHeader"
Cohesion: 0.33
Nodes (6): uint16_t, ObjectHeader, flags, hash, klass, size

### Community 181 - "P5 — コンパイラ"
Cohesion: 0.15
Nodes (21): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+13 more)

### Community 182 - "fileInLoadOrder"
Cohesion: 0.53
Nodes (6): path, deferredMethods(), fileInListedFile(), fileInLoadOrder(), readSource(), sessionFileInLoadOrder()

### Community 183 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 184 - "BrowserWindow.removeAfterConfirming(...)"
Cohesion: 0.40
Nodes (5): BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:), BrowserWindow.removeAfterConfirming(...), BrowserWindow.removeMethod / removeMethod(atRow:), testRemoveMethodAfterConfirmUpdatesLists

### Community 185 - "TEST"
Cohesion: 0.25
Nodes (6): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, TEST(), VersionOneIsRefusedWithReason

### Community 186 - "SelectedFrames"
Cohesion: 0.29
Nodes (5): debugReason(), SelectedFrames, frames_, live_, none_

### Community 187 - "SuperclassWalk"
Cohesion: 0.40
Nodes (3): uint32_t, SuperclassWalk, depth_

### Community 188 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 189 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 190 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 191 - "P11 — ライブデバッガ"
Cohesion: 0.29
Nodes (7): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 範囲, 結論

### Community 192 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

### Community 193 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 194 - "WellKnown.cpp"
Cohesion: 0.12
Nodes (16): deque, Roots, size_t, string, unordered_map, ImageSelector, name, WellKnown (+8 more)

### Community 195 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 196 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 197 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 199 - "SlotNames"
Cohesion: 0.67
Nodes (3): SlotNames, names, WellKnown

### Community 200 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 201 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 202 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 203 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 205 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 208 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneOneZero

### Community 211 - "NativeMethod.hpp"
Cohesion: 0.50
Nodes (3): DebugSink, Frame, Scheduler

### Community 213 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 215 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 216 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 218 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 221 - "Post-mortem debugger"
Cohesion: 0.67
Nodes (3): ao_set_debug_capture / ao_debug_*, Object>>halt, Post-mortem debugger

### Community 222 - "Live debugger"
Cohesion: 0.67
Nodes (3): Ao.app Debugger window, Live debugger, ao_set_debug_mode(AO_DEBUG_LIVE) / AO_ERR_HALT

### Community 223 - "Rules That Do Not Bend"
Cohesion: 0.67
Nodes (3): Fixed Design Decisions, 曲げない規則, Rules That Do Not Bend

### Community 352 - "reshapeClass"
Cohesion: 0.17
Nodes (23): FileInError, error, file, method, string, MethodDictionary::create, string_view, isVendorStub() (+15 more)

### Community 353 - "CarriedMethod"
Cohesion: 0.40
Nodes (5): CarriedMethod, image, meta, selector, source

## Ambiguous Edges - Review These
- `Smalltalk-80 removeFromSystem` → `Blue Book (Smalltalk-80: The Language and its Implementation)`  [AMBIGUOUS]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md · relation: conceptually_related_to
- `Debug it (AO_EVAL_DEBUGIT)` → `SPEC §3.13 デバッガ（捕捉の意味論）`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `P5-01: 字句解析` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references
- `P7-03: load と NativeMethod 再結合` → `P8-01: Ao.app 骨格`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `P8-05: メニューとキー` → `P9-01: Do it / Print it / Inspect it`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references

## Knowledge Gaps
- **1015 isolated node(s):** `.selectedClass`, `.metaFlag`, `.canRemoveMethod`, `.canRemoveClass`, `.hasUnacceptedChanges` (+1010 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2648 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **117 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Smalltalk-80 removeFromSystem` and `Blue Book (Smalltalk-80: The Language and its Implementation)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `Debug it (AO_EVAL_DEBUGIT)` and `SPEC §3.13 デバッガ（捕捉の意味論）`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P5-01: 字句解析` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P7-03: load と NativeMethod 再結合` and `P8-01: Ao.app 骨格`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
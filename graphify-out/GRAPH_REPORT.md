# Graph Report - ao-smalltalk  (2026-09-30)

## Corpus Check
- 300 files · ~351,471 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 6244 nodes · 16613 edges · 326 communities (206 shown, 120 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 2293 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `a47a79a1`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Compile.cpp
- TEST_F
- TEST
- abi.cpp
- .fromSmallInteger
- TEST
- image_save_load_test.cpp
- DebuggerWindow
- TEST_F
- Object.cpp
- send
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
- cstdint
- TEST
- Scheduler.cpp
- Heap.cpp
- TEST
- TEST
- ChunkParser.cpp
- Session
- Analysis
- TranscriptWindow
- Gc
- Float.cpp
- .true_
- Ast
- BrowserModel
- CallContext
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- Session.cpp
- DebugFrames
- TEST_F
- ImageLoad.cpp
- ao_eval
- ao_set_debug_capture
- ToolWindowTests
- .isHeap
- classRows
- TEST_F
- TEST_F
- String.cpp
- TEST
- WellKnown.cpp
- MethodDebugInfo
- Send.cpp
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- AoSpan
- TEST
- Ao 実装文書 index (docs/README.md)
- DebugSnapshot
- SmallInteger.cpp
- TEST
- TEST
- DebugSnapshot.cpp
- Geometry.cpp
- VendorExtract.cpp
- TEST
- string
- AoApp
- DiskHeader
- collectNursery
- ao_remove_class Rules
- Roots
- TEST
- Claude Review Fixes Plan (2026-09-23)
- Oop
- fiber_test.cpp
- FiberStack
- P6b Vendor File-in Implementation Plan
- CallContext
- Scheduler::Record
- Fiber.cpp
- TEST
- Literal
- ChunkAction
- evalBody
- Character.cpp
- LiveFrames
- InlineCache
- BlockContext.cpp
- TEST_F
- WorkspaceWindow
- BrowserModelTests
- CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27)
- liveClasses
- pc_map_test.cpp
- TEST
- TEST (17)
- NativeMethod.cpp
- PingPong
- .build
- TEST
- TEST
- Roots.cpp
- uint64_t
- HashedCollection.cpp
- AppKit
- String
- TEST
- P12 — Browser の削除 (phase doc)
- Frame
- TEST
- Lookup.hpp
- ao_main.cpp
- ImageSurgery
- TEST
- ImageFormat
- KeptClass
- GarbageFirstBoot
- native_method_test.cpp
- TEST
- TEST
- ScopedGcStressEnv
- CountingSink
- TEST
- P3 — ネイティブディスパッチ
- .nil
- FiberRegs
- P3 — ネイティブディスパッチ
- create
- VirtualRegion.cpp
- TEST (22)
- Release / Semantic Versioning
- IgnoreFileSizeSignal
- MethodDebugInfo
- createBlock
- DepthGuard
- string
- Loaded
- ClassId
- Deferred vendor methods list
- OperandStack
- Frame
- BusyRemove
- MethodParts
- removeMethodOf (definition)
- send2
- Ao Smalltalk 概要 (JA)
- .isTrue
- ImageSelector
- Interpreter.cpp
- path
- TEST
- TEST
- P0 (親フェーズ, stub)
- P1 — オブジェクトメモリ
- abortingNew
- FrameBlock
- removeClassOf
- ao_eval (2)
- P2 — ブートストラップ
- heap
- P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）
- interpreter
- parser
- roots
- runtime_src_session
- string_view
- TEST
- vector
- Vendor.hpp
- contextPc
- Bootstrap
- P5 — コンパイラ
- TEST
- TEST
- P10 — 事後デバッガ
- TEST
- SelectedFrames
- ClassMethodCache
- Counts
- P11 — ライブデバッガ
- clearUnwinding
- ClassDef
- WellKnown::InternTable
- FileSizeLimit
- Rec
- intern
- cli_test.sh
- popFrame
- TestDir
- test.sh
- CompileResult
- TEST
- assemble
- NativeMethod.hpp
- Roots::visitAll
- abortingSubclass
- Root
- imageRegistryStubA
- acceptClass
- answerOne
- .menuTitles
- .turnRunLoop
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
- boxLiteral
- vector
- reshapeClass
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
6. `TEST_F()` - 119 edges
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

## Communities (326 total, 120 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 1 - "Compile.cpp"
Cohesion: 0.12
Nodes (50): cctype, isClassShaped(), superclassOf(), acceptClassDef(), acceptClassSource(), acceptMethodInto(), acceptMethodSource(), anyMethodIn() (+42 more)

### Community 2 - "TEST_F"
Cohesion: 0.03
Nodes (87): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+79 more)

### Community 3 - "TEST"
Cohesion: 0.03
Nodes (80): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+72 more)

### Community 4 - "abi.cpp"
Cohesion: 0.05
Nodes (79): atomic, Body, ClassIdIsStableAndOneRowPerClass, ClassIdsAreDroppedWithTheirClassAndNeverReused, ClassIdsDoNotSurviveBootOrImageLoad, CountsAnswerMinusOneOnFailure, ABI（P10-05）, Design rationale: session class ID table (+71 more)

### Community 5 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 6 - "TEST"
Cohesion: 0.02
Nodes (82): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+74 more)

### Community 7 - "image_save_load_test.cpp"
Cohesion: 0.14
Nodes (28): algorithm, chrono, Chunk, classpool, compile, CompiledMethod, compiler, context (+20 more)

### Community 8 - "DebuggerWindow"
Cohesion: 0.05
Nodes (46): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+38 more)

### Community 9 - "TEST_F"
Cohesion: 0.03
Nodes (81): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+73 more)

### Community 10 - "Object.cpp"
Cohesion: 0.12
Nodes (52): Graphify / Serena, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 11 - "send"
Cohesion: 0.17
Nodes (36): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+28 more)

### Community 12 - "Parser"
Cohesion: 0.11
Nodes (22): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+14 more)

### Community 13 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 14 - "AcceptTests"
Cohesion: 0.13
Nodes (10): spanMessage(), AcceptTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String (+2 more)

### Community 15 - "BrowserWindow"
Cohesion: 0.06
Nodes (34): BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText (+26 more)

### Community 16 - "DebuggerWindowTests"
Cohesion: 0.10
Nodes (18): Int, DebuggerWindowTests, Int, NSButton, NSFont, NSTableView, NSTextView, NSView (+10 more)

### Community 17 - "Emitter"
Cohesion: 0.13
Nodes (16): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+8 more)

### Community 18 - "Stream.cpp"
Cohesion: 0.10
Nodes (70): allocateInstance(), allocateRetry(), CallContext, Roots, uint16_t, uint32_t, RootedArray::RootedArray(), ao_PositionableStream_contents() (+62 more)

### Community 19 - "TEST"
Cohesion: 0.04
Nodes (53): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+45 more)

### Community 20 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 21 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (44): cerrno, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot() (+36 more)

### Community 22 - "アプリ"
Cohesion: 0.10
Nodes (27): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String (+19 more)

### Community 23 - "TEST"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (58): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, expectAbortedEmpty() (+50 more)

### Community 25 - "Heap"
Cohesion: 0.04
Nodes (96): hashedcollection, Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_ (+88 more)

### Community 26 - "TEST_F"
Cohesion: 0.06
Nodes (44): AliasedOldClassRowActsOnItsOwnClass, ClassIdTableDoesNotBlockSuperclassRemoval, KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, MalformedMethodDictionaryFallsBackOnSend, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad, RemoveAcceptedMethodOnKernelClass (+36 more)

### Community 27 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 28 - "cstdint"
Cohesion: 0.07
Nodes (20): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, Roots (+12 more)

### Community 29 - "TEST"
Cohesion: 0.03
Nodes (84): AllocateNoGcSpillsToOld, ByteObjectPayload, ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap (+76 more)

### Community 30 - "Scheduler.cpp"
Cohesion: 0.03
Nodes (113): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+105 more)

### Community 31 - "Heap.cpp"
Cohesion: 0.07
Nodes (50): charconv, allocateTenured, checkNotPoisoned, growOld, initObject, objectBytes, oldUsed, uint16_t (+42 more)

### Community 32 - "TEST"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 33 - "TEST"
Cohesion: 0.05
Nodes (45): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+37 more)

### Community 34 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 35 - "Session"
Cohesion: 0.05
Nodes (51): 手順（PR の順序と依存）, attachBlocks(), blockLiteral(), browserClassCount(), MethodImage, size_t, debugCanProceed(), debugFrameCount() (+43 more)

### Community 36 - "Analysis"
Cohesion: 0.08
Nodes (24): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, Analysis (+16 more)

### Community 37 - "TranscriptWindow"
Cohesion: 0.07
Nodes (30): fileInVendor(), aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int (+22 more)

### Community 38 - "Gc"
Cohesion: 0.11
Nodes (22): cstdlib, Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_ (+14 more)

### Community 39 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 40 - ".true_"
Cohesion: 0.17
Nodes (31): MethodDictionary::at, MethodDictionary::pairArray, isPseudoVariableName, ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize() (+23 more)

### Community 41 - "Ast"
Cohesion: 0.05
Nodes (63): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+55 more)

### Community 42 - "BrowserModel"
Cohesion: 0.17
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 43 - "CallContext"
Cohesion: 0.05
Nodes (42): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+34 more)

### Community 44 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 45 - "Session.cpp"
Cohesion: 0.09
Nodes (31): Image, check, load, save, bumpDebugGeneration(), clearMethodSources(), AoRunLoopPumpFn, CallContext (+23 more)

### Community 46 - "DebugFrames"
Cohesion: 0.08
Nodes (41): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context, count, empty, kind, method, pc (+33 more)

### Community 47 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 48 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (30): ifstream, readHeader, bindAll(), checkFile(), checkGlobals(), byte, Roots, size_t (+22 more)

### Community 49 - "ao_eval"
Cohesion: 0.05
Nodes (55): B2 `to:do:` bench, 仕様（設計判断）, P10 から 1.1.0 まで約 24 時間。実装は Claude Code、レビューは別のセッションで, デバッガにまだ無いもの：評価の中断、Restart、ネイティブへの Step into, ライブデバッガでは、止めたプロセスを「走っていない」ことにした, 事後デバッガ：巻き戻す前のスタックを写すだけにした, 前編のまとめ：失敗の扱いを変えなかったので、既存のテストと CLI が守りになった, Execution notes (+47 more)

### Community 50 - "ao_set_debug_capture"
Cohesion: 0.10
Nodes (18): AoTranscriptFn, 範囲, Qiita 後編: 協調スケジューラとファイバ切り替え, SPEC を先に直す（P10-01）, 範囲, 前提, ao_set_debug_capture(), ao_set_debug_mode() (+10 more)

### Community 51 - "ToolWindowTests"
Cohesion: 0.13
Nodes (10): LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String, T (+2 more)

### Community 52 - ".isHeap"
Cohesion: 0.07
Nodes (74): RemoveUnits, allocateNoGc, flags, size, slotAt, slotAtPut, int64_t, signal (+66 more)

### Community 53 - "classRows"
Cohesion: 0.14
Nodes (36): assignClassIds(), browserClassAt(), browserClassDefinition(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+28 more)

### Community 54 - "TEST_F"
Cohesion: 0.05
Nodes (36): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+28 more)

### Community 55 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 56 - "String.cpp"
Cohesion: 0.13
Nodes (39): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), ao_String_asSymbol(), ao_String_at_() (+31 more)

### Community 57 - "TEST"
Cohesion: 0.15
Nodes (13): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, MalformedDictionaryAnswersNilAndTakesNothing, NinthMethodWithFullNurseryIsInstalled, RemoveKeyLeavesTheOthersFindableAndReusesThePair (+5 more)

### Community 58 - "WellKnown.cpp"
Cohesion: 0.10
Nodes (29): findSymbol, global, internWith, isFixedGlobal, Roots, string_view, NamedClass, name (+21 more)

### Community 59 - "MethodDebugInfo"
Cohesion: 0.12
Nodes (17): uint32_t, PcSpan, end, pc, start, DebugInfoRef, body, index (+9 more)

### Community 60 - "Send.cpp"
Cohesion: 0.17
Nodes (27): Graphify / Serena, フレーム連鎖（P10-03）, run, applyMethod(), abortDoesNotUnderstand(), abortEvaluation(), abortEvaluationQuiet(), abortFailedSend() (+19 more)

### Community 61 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.07
Nodes (58): docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, SPEC.md, Codegen.cpp: Emitter::mark と compile* 群, MethodImage.hpp: PcSpan / TempName / pcMap / temps (+50 more)

### Community 62 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 63 - "AoSpan"
Cohesion: 0.05
Nodes (59): BrowserModel.classID(named:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:) (+51 more)

### Community 64 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 65 - "Ao 実装文書 index (docs/README.md)"
Cohesion: 0.07
Nodes (59): P4 — Kernelネイティブ実装, vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, P8 — AppKit ツール, P9 — 統合, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装 (+51 more)

### Community 66 - "DebugSnapshot"
Cohesion: 0.06
Nodes (28): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+20 more)

### Community 67 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 68 - "TEST"
Cohesion: 0.05
Nodes (48): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+40 more)

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
Cohesion: 0.18
Nodes (33): allowIndex(), containsHostWord(), size_t, string, string_view, doubleBangs(), dropCycles(), dropMissingSupers() (+25 more)

### Community 73 - "TEST"
Cohesion: 0.07
Nodes (28): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+20 more)

### Community 74 - "string"
Cohesion: 0.10
Nodes (17): cmath, string, cstdio, filesystem, future, iterator, limits, runtime (+9 more)

### Community 75 - "AoApp"
Cohesion: 0.12
Nodes (14): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+6 more)

### Community 76 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 77 - "collectNursery"
Cohesion: 0.11
Nodes (24): BlockContextKeepsHomeAndCopied, CleanupFailureReleasesItsReasonHandle, ContextGc, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, MethodContextSurvivesNurseryCollection (+16 more)

### Community 78 - "ao_remove_class Rules"
Cohesion: 0.08
Nodes (29): Context menu flow (discard confirm -> select row -> remove confirm -> ABI), Class removal unbinds name only, ao_accept_class, ao_accept_method, Ao Bytecode Set, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation), Blue Book Conformance Definition (+21 more)

### Community 79 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 80 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 81 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.11
Nodes (27): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+19 more)

### Community 82 - "Oop"
Cohesion: 0.14
Nodes (39): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+31 more)

### Community 83 - "fiber_test.cpp"
Cohesion: 0.14
Nodes (16): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), TEST(), csignal (+8 more)

### Community 84 - "FiberStack"
Cohesion: 0.10
Nodes (26): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+18 more)

### Community 85 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.11
Nodes (26): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+18 more)

### Community 86 - "CallContext"
Cohesion: 0.15
Nodes (23): valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+15 more)

### Community 87 - "Scheduler::Record"
Cohesion: 0.07
Nodes (27): Scheduler, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, block, ctx, deadlockPending (+19 more)

### Community 88 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 89 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 90 - "Literal"
Cohesion: 0.07
Nodes (33): int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal (+25 more)

### Community 91 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 92 - "evalBody"
Cohesion: 0.14
Nodes (25): boxMethodImage, answerAwaited(), answerEval(), blankOut(), clearInterruptRequest(), AoInspectFn, EvalEnd, optional (+17 more)

### Community 93 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 94 - "LiveFrames"
Cohesion: 0.09
Nodes (20): CallContext, DebugSink, onAbort, Frame, size_t, string, LiveFrames, context (+12 more)

### Community 95 - "InlineCache"
Cohesion: 0.32
Nodes (7): InlineCache, cachedClass, cachedMethod, ClassMethodCache::addRoots(), Roots, IcGuard, ic

### Community 96 - "BlockContext.cpp"
Cohesion: 0.24
Nodes (22): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+14 more)

### Community 97 - "TEST_F"
Cohesion: 0.07
Nodes (27): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+19 more)

### Community 98 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (24): AnyObject, installDebugButton(), sendToKeyWorkspace(), Bool, MainActor, NSButton, NSTextField, NSWindow (+16 more)

### Community 99 - "BrowserModelTests"
Cohesion: 0.18
Nodes (10): BrowserModelTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+2 more)

### Community 100 - "CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27)"
Cohesion: 0.05
Nodes (54): CHANGELOG: [1.0.0] first release, CHANGELOG: [1.1.0] release (P10-P12, 2026-09-27), 1.1.0 install notes (macOS 14+, shasum check, Open Anyway / xattr quarantine), P11 known limitations, Known limitation: sourceless frames have no statement starts (Step over/into skip statements), CHANGELOG: P10/P11 post-mortem and live debugger entry, 1.1.0 release assets (Ao-1.1.0-macos-arm64.zip, ao-cli-1.1.0-macos-arm64.tar.gz, SHA256SUMS), Build only what SPEC.md has (v1 and later §2.3 phases) (+46 more)

### Community 101 - "liveClasses"
Cohesion: 0.38
Nodes (7): ソース表の拡張（P10-04）, リスク, liveClasses(), classIdRootSlots(), DebugInfo, bodies, methodSourceRootSlots()

### Community 102 - "pc_map_test.cpp"
Cohesion: 0.13
Nodes (21): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, uint32_t, firstBlock(), named() (+13 more)

### Community 103 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 104 - "TEST (17)"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 105 - "NativeMethod.cpp"
Cohesion: 0.22
Nodes (19): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+11 more)

### Community 106 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 107 - ".build"
Cohesion: 0.22
Nodes (10): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+2 more)

### Community 108 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 109 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 110 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 111 - "uint64_t"
Cohesion: 0.23
Nodes (21): acceptWord(), atOffset(), string, uint64_t, unordered_map, unordered_set, fail(), headerAt() (+13 more)

### Community 112 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 113 - "AppKit"
Cohesion: 0.15
Nodes (11): Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, transcriptBoxHook(), SmokeTests, AppKit (+3 more)

### Community 114 - "String"
Cohesion: 0.17
Nodes (11): DefinitionScanner, Bool, Int32, NSMenu, Selector, String, Token, keyword (+3 more)

### Community 115 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 116 - "P12 — Browser の削除 (phase doc)"
Cohesion: 0.08
Nodes (46): P12 — Browser の削除 (phase doc), Addendum tests (browser_abi_test, accept_abi_test, session_abi_test, method_dictionary_test, BrowserModelTests), P12 Addendum: Class ID and Broken Method Dictionary (2026-09-27), Method dictionary removal (nil pair, decrement tally), MethodDictionary::pairArray (shape check), BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove, Session::forgetMethodSource (+38 more)

### Community 117 - "Frame"
Cohesion: 0.33
Nodes (6): ContextExitGuard, Frame, Roots, FieldRoots, frame, FrameLink

### Community 118 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 119 - "Lookup.hpp"
Cohesion: 0.40
Nodes (3): uint32_t, SuperclassWalk, depth_

### Community 120 - "ao_main.cpp"
Cohesion: 0.29
Nodes (13): climits, dyld, addRoots, bootAndRunTests(), string, imageUsage(), main(), printFileInErrors() (+5 more)

### Community 121 - "ImageSurgery"
Cohesion: 0.25
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 122 - "TEST"
Cohesion: 0.17
Nodes (12): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, fillNursery(), smi(), TEST() (+4 more)

### Community 123 - "ImageFormat"
Cohesion: 0.08
Nodes (25): uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes, kImagePointerBits (+17 more)

### Community 124 - "KeptClass"
Cohesion: 0.14
Nodes (14): KeptClass, category, classVars, deferred, hasDef, instVars, methods, pools (+6 more)

### Community 125 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 126 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 127 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 128 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 129 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 130 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 131 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 132 - "P3 — ネイティブディスパッチ"
Cohesion: 0.16
Nodes (15): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary (+7 more)

### Community 133 - ".nil"
Cohesion: 0.13
Nodes (31): P12 追補: クラス ID と壊れたメソッド辞書 実装計画, Design rationale: MethodDictionary shape check (pairArray), allocateSkeletons(), allocClass(), Roots, string_view, WellKnown, ensureMethodDict() (+23 more)

### Community 134 - "FiberRegs"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

### Community 135 - "P3 — ネイティブディスパッチ"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 136 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

### Community 137 - "VirtualRegion.cpp"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 138 - "TEST (22)"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 139 - "Release / Semantic Versioning"
Cohesion: 0.12
Nodes (19): CLAUDE.md, P10-P12 released as 1.1.0, v1 = P9 completion, released as 1.0.0, version_string(), package-app.sh script, Acceptance Criteria (§6), Bootstrap Procedure, Change Rules (§8) (+11 more)

### Community 140 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 141 - "MethodDebugInfo"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 142 - "createBlock"
Cohesion: 0.60
Nodes (4): CallContext, uint8_t, createBlock(), createMethod()

### Community 144 - "string"
Cohesion: 0.28
Nodes (15): byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), string, WellKnown, definitionCategory() (+7 more)

### Community 145 - "Loaded"
Cohesion: 0.13
Nodes (17): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+9 more)

### Community 146 - "ClassId"
Cohesion: 0.50
Nodes (4): ClassId, cls, id, int64_t

### Community 147 - "Deferred vendor methods list"
Cohesion: 0.16
Nodes (14): B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12), MappedCollection absent from pinned sources (+6 more)

### Community 148 - "OperandStack"
Cohesion: 0.13
Nodes (13): deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots, Temps (+5 more)

### Community 149 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 150 - "BusyRemove"
Cohesion: 0.14
Nodes (14): BusyRemove, acceptRc, busyId, classMsg, classRc, goneHeldAfterRead, goneHeldBeforeRead, goneId (+6 more)

### Community 151 - "MethodParts"
Cohesion: 0.50
Nodes (4): MethodParts, body, pattern, splitMethod()

### Community 152 - "removeMethodOf (definition)"
Cohesion: 0.67
Nodes (3): removeMethodOf, MethodDictionary::removeKey, removeMethodOf (definition)

### Community 153 - "send2"
Cohesion: 0.06
Nodes (32): ClassDefinitionThroughAliasOnlyRebindsGlobal, ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, Geometry, KeepsNativeIdentityEquals, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails (+24 more)

### Community 154 - "Ao Smalltalk 概要 (JA)"
Cohesion: 0.08
Nodes (25): v1 known limitations, Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps), AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target) (+17 more)

### Community 155 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 156 - "ImageSelector"
Cohesion: 0.67
Nodes (3): ImageSelector, name, WellKnown

### Community 157 - "Interpreter.cpp"
Cohesion: 0.13
Nodes (36): ActiveGuard, rootShared, saved, boolean(), branchTruth(), byteCount(), checkInterrupt(), clearNonlocal() (+28 more)

### Community 158 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 159 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 160 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 161 - "P0 (親フェーズ, stub)"
Cohesion: 0.26
Nodes (12): P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+4 more)

### Community 162 - "P1 — オブジェクトメモリ"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 163 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 164 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 166 - "ao_eval (2)"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 167 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 169 - "P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）"
Cohesion: 0.33
Nodes (6): `Object>>halt`（P10-03）, P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）, 仕様, 捕捉（P10-03、配線は P10-04）, 検証, 結論

### Community 175 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 177 - "Vendor.hpp"
Cohesion: 0.20
Nodes (11): string, string_view, isVendorStub(), VendorClassFile, chunkText, className, superName, unsupportedShape (+3 more)

### Community 178 - "contextPc"
Cohesion: 0.44
Nodes (10): contextPc(), CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget() (+2 more)

### Community 179 - "Bootstrap"
Cohesion: 0.17
Nodes (15): Bootstrap, MethodDictionary, CallContext, uint32_t, stubA(), stubB(), answerMessage(), CallContext (+7 more)

### Community 181 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 182 - "TEST"
Cohesion: 0.25
Nodes (8): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionStringIsReleaseOneOneZero, VersionTruncationIsRangeError

### Community 183 - "TEST"
Cohesion: 0.22
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 184 - "P10 — 事後デバッガ"
Cohesion: 0.29
Nodes (7): P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 結論

### Community 185 - "TEST"
Cohesion: 0.25
Nodes (6): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, TEST(), VersionOneIsRefusedWithReason

### Community 186 - "SelectedFrames"
Cohesion: 0.29
Nodes (5): debugReason(), SelectedFrames, frames_, live_, none_

### Community 188 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 189 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 191 - "P11 — ライブデバッガ"
Cohesion: 0.29
Nodes (7): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 範囲, 結論

### Community 192 - "clearUnwinding"
Cohesion: 0.19
Nodes (18): endEval, recordFailure, refreshStackLimit(), Scheduler::runFiber(), abortReasonText(), clearUnwinding(), string, sessionDebugAbort() (+10 more)

### Community 193 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 194 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 196 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 197 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 199 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 200 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 201 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 203 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 205 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 206 - "CompileResult"
Cohesion: 0.14
Nodes (12): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+4 more)

### Community 208 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneOneZero

### Community 209 - "assemble"
Cohesion: 0.50
Nodes (4): initializer_list, assemble(), uint8_t, op()

### Community 211 - "NativeMethod.hpp"
Cohesion: 0.50
Nodes (3): DebugSink, Frame, Scheduler

### Community 213 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 214 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 215 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 216 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 217 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

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

### Community 350 - "boxLiteral"
Cohesion: 0.36
Nodes (8): boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), MethodImage, uint32_t, uint8_t, installMethod()

### Community 352 - "vector"
Cohesion: 0.14
Nodes (30): CompileError, message, span, string, vector, acceptClassSource, FileInError, error (+22 more)

### Community 353 - "reshapeClass"
Cohesion: 0.09
Nodes (26): BrowserModel.copyClass(at:), ao_accept_class, ao_browser_class_at, acceptMethodSource, MethodDictionary::atPut, MethodDictionary::create, acceptMethodInto (definition), CarriedMethod (+18 more)

## Ambiguous Edges - Review These
- `Smalltalk-80 removeFromSystem` → `Blue Book (Smalltalk-80: The Language and its Implementation)`  [AMBIGUOUS]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md · relation: conceptually_related_to
- `Debug it (AO_EVAL_DEBUGIT)` → `SPEC §3.13 デバッガ（捕捉の意味論）`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references
- `P7-03: load と NativeMethod 再結合` → `P8-01: Ao.app 骨格`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `P9-01: Do it / Print it / Inspect it` → `P8-05: メニューとキー`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P4-09: Kernel NativeMethod 走査と bench` → `P5-01: 字句解析`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references

## Knowledge Gaps
- **1014 isolated node(s):** `.selectedClass`, `.metaFlag`, `.canRemoveMethod`, `.canRemoveClass`, `.hasUnacceptedChanges` (+1009 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2643 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **120 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Smalltalk-80 removeFromSystem` and `Blue Book (Smalltalk-80: The Language and its Implementation)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `Debug it (AO_EVAL_DEBUGIT)` and `SPEC §3.13 デバッガ（捕捉の意味論）`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P7-03: load と NativeMethod 再結合` and `P8-01: Ao.app 骨格`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P9-01: Do it / Print it / Inspect it` and `P8-05: メニューとキー`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
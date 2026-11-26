# Graph Report - ao-smalltalk  (2026-11-26)

## Corpus Check
- 325 files · ~460,652 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 83 file(s) not represented in the graph (top: .st 67, (none) 13, .toml 1)

## Summary
- 7094 nodes · 19664 edges · 357 communities (238 shown, 119 thin omitted)
- Extraction: 83% EXTRACTED · 17% INFERRED · 0% AMBIGUOUS · INFERRED: 3430 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `67b345de`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- TEST_F
- WellKnown
- TEST_F
- BlockContext.cpp
- .fromSmallInteger
- TEST
- String.cpp
- TEST
- CallContext
- MethodSource
- LargeInteger.cpp
- Oop
- Stream.cpp
- DebuggerWindow
- BrowserModel
- Parser
- TEST
- Emitter
- AcceptTests
- image_save_load_test.cpp
- ao_eval
- Boot
- P6b Vendor File-in Implementation Plan
- CompiledMethodNatives.cpp
- Heap
- Scanner.cpp
- .isHeap
- TEST
- TEST
- RealScope
- abi.cpp
- resumeHalted
- Scheduler
- TEST
- .nil
- ChangeSorterWindow
- WorkspaceWindow
- Heap.cpp
- TEST
- TEST
- ImageSave.cpp
- TEST_F
- GarbageFirstBoot
- Send.cpp
- Float.cpp
- ToolWindowTests
- isClassShaped
- TEST_F
- DebugFrames
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- .isSmallInteger
- P12 Browser 削除 実装計画
- DebuggerWindowTests
- ImageFormat
- DebugSnapshot
- Session.cpp
- TEST_F
- String
- WellKnown.cpp
- TEST_F
- AoApp
- ImageHeader
- DebugSnapshot.cpp
- Geometry.cpp
- アプリ
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- .false_
- TEST
- CallContext
- TEST
- NativeMethod.cpp
- TEST
- TEST
- Boolean.cpp
- size
- .testDebuggerControlsHaveAccessibilityLabels
- VendorExtract.cpp
- §3.9 Removal (Browser deletion rules)
- Scheduler::Record
- TEST
- BrowserModelTests
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- High（12 件）
- Roots
- SmallInteger.cpp
- TEST
- TEST
- Table
- TEST_F
- Claude Review Fixes Plan (2026-09-23)
- TranscriptWindow
- MainMenu
- ChangeSet.cpp
- FiberStackBounds
- ChangeSorterModel
- TEST
- Fiber.cpp
- 前提
- FiberStack
- TEST
- TEST
- CompileEnv
- Compiler (.st to CompiledMethod)
- LiveFrames
- BrowserWindow
- hasSlots
- P5 — コンパイラ
- UniformFont
- .base
- P4 — Kernelネイティブ実装
- stream_test.cpp
- TEST
- pc_map_test.cpp
- Roots.cpp
- fiber_test.cpp
- OperandStack
- string
- TEST
- .isEmpty
- TEST
- review-agy 指摘の修正計画
- P1 — オブジェクトメモリ
- FiberRegs
- VirtualRegion.cpp
- native_method_test.cpp
- P11 — ライブデバッガ
- ClassPool.cpp
- TEST
- ao_runtime_boot
- Ao implementation docs index (phases and PRs)
- DiskHeader
- 設計判断
- CountingSink
- review-agy 修正の引継ぎ（2026-11-26、R6 の PR 作成後）
- AppKit
- ChunkAction
- ChunkParser.cpp
- Session
- Release 1.0.0 (v1, P0-P9)
- applyMethod
- TEST
- native_send_test.cpp
- TEST
- Rec
- ImageSurgery
- Stack
- MethodDebugInfo
- String
- Root
- .isTrue
- Gc.cpp
- string
- TEST_F
- Loaded
- Release / Semantic Versioning
- ClassMethodCache
- TEST
- ao_accept_method
- Interpreter.cpp
- TEST
- TEST
- uint64_t
- FrameBlock
- path
- TEST
- Ast
- clearUnwinding
- Token
- PingPong
- Debugger Accept (reactivate frame with new method)
- P2 — ブートストラップ
- .aoimage Image Format
- abortingSubclass
- TEST
- Gc
- TEST
- TEST
- send2
- 前提（コードで確かめた事実。`b35efe8`）
- test.sh
- ImageLoad.cpp
- CompileError
- Analysis
- Frame
- Ao README
- string
- Image::load
- Counts
- P0 (親フェーズ, stub)
- vector
- Bootstrap.cpp
- Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）
- parser
- P3 — ネイティブディスパッチ
- .specialSelector
- Scheduler::halt
- P18 — メソッドソースの永続化
- Literal
- ao_main.cpp
- .isBase
- NativeMethod.hpp
- HashNesting
- P8–P9 Remaining Implementation Plan
- popFrame
- Vendor Class Library (import, do not write)
- Frame
- WellKnown::InternTable
- DebuggerWindowTests.swift
- TestDir
- Graphify Required Tool
- intern
- ao CMake Project
- interpreter
- ObjectHeader
- runtime_src_fiber
- string_view
- Roots::visitAll
- compiler/tests/smoke_test.cpp
- VendorClassFile
- b5Definition
- 項目
- HostMethod
- .testAcceptMenuFollowsDebuggerEditability
- test (macOS arm64) Job
- P17 — Smalltalk のチェンジセット
- imageRegistryStubA
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
- uint64_t
- OcSlots
- BrowserModel.applyHierarchyList(_:selecting:)
- BrowserModel.hierarchy(of:meta:)
- boxLiteral
- RegisterSwap
- FileSizeLimit
- createBlock
- KeptMethod
- ParseResult
- 3. 確定指摘一覧
- acceptMethodInto (definition)
- ImageSource
- TEST
- create
- cstdint
- IgnoreFileSizeSignal
- ActiveGuard
- ao_browser_class_count
- ao_browser_subclass_count
- ao_eval
- Fixed Design Decisions
- Build only what SPEC.md has (v1 and later §2.3 phases)
- acceptClass
- compiler/src/Scanner.cpp chunk scanner (referenced, not read)
- いまの位置 (1.1.0、PHASE は P12)
- answerOne
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
- Context menu flow (discard confirm -> select row -> remove confirm -> ABI)
- heap
- TEST
- DepthGuard
- README.ja デバッガの使い方
- ImageSelector
- roots
- runtime_src_session
- MethodImage
- Roots
- runtime_src_session
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- ao_accept_method
- P13 Evaluation Interrupt Phase
- vector

## God Nodes (most connected - your core abstractions)
1. `Oop` - 989 edges
2. `Heap` - 258 edges
3. `vector` - 199 edges
4. `TEST_F()` - 199 edges
5. `WellKnown` - 168 edges
6. `Session` - 135 edges
7. `TEST()` - 129 edges
8. `BrowserWindow` - 128 edges
9. `TEST()` - 127 edges
10. `TEST_F()` - 109 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `置き場と読み込み` --references--> `fileInVendor()`  [INFERRED]
  docs/superpowers/specs/2026-10-12-p17-smalltalk-change-sets-design.md → app/Ao/AoApp.swift
- `テスト` --references--> `ChangeSorterTests`  [INFERRED]
  docs/superpowers/specs/2026-10-12-p17-smalltalk-change-sets-design.md → app/AoTests/ChangeSorterTests.swift
- `TDD` --references--> `DebuggerWindowTests`  [INFERRED]
  docs/phases/P11.md → app/AoTests/DebuggerWindowTests.swift
- `TDD` --references--> `DebuggerWindowTests`  [INFERRED]
  docs/phases/P15.md → app/AoTests/DebuggerWindowTests.swift

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **SPEC 3.10 class ID scheme: stable IDs across Browser rows, ABI, and Session table** — app_ao_browsermodel_browserclass, app_ao_browsermodel_classid, bridge_ao_abi_ao_browser_class_id, runtime_src_session_browserclassid, runtime_src_session_sessionclassforid, runtime_src_session_assignclassids, runtime_src_session_classid [EXTRACTED 0.90]
- **SPEC 3.9 removal flow: Browser UI to method dictionary and source table** — app_ao_browserwindow_removemethod, bridge_ao_abi_ao_remove_method, runtime_include_ao_compile_removemethodof, runtime_src_compile_removemethodof_impl, runtime_include_ao_methoddictionary_removekey, runtime_src_session_forgetmethodsource [EXTRACTED 0.95]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **Method dictionary mutation paths sharing single cache-invalidation function** — spec_ao_accept_method, spec_ao_accept_method_id, spec_ao_remove_method, spec_ao_remove_class, spec_method_cache [EXTRACTED 1.00]
- **P12 removal flow: Browser UI -> class ID -> C ABI -> dictionary/cache/source-table updates** — spec_browser_remove_ui, spec_section_3_10_classid, spec_ao_remove_method, spec_ao_remove_class, spec_method_cache, spec_source_table [EXTRACTED 1.00]
- **P13 interrupt flow: host pump, request flag, safepoint check, live halt** — changelog_ao_request_interrupt, changelog_ao_set_runloop_pump_hook, plans_2026_09_30_evaluation_interrupt_runloop_pump_swift, plans_2026_09_30_evaluation_interrupt_check_interrupt, specs_2026_09_30_evaluation_interrupt_design_scheduler_can_halt, specs_2026_09_30_evaluation_interrupt_design_interpreter_safepoint [EXTRACTED 1.00]
- **P14 Restart: ABI, fiber unwind, frame chain, no inner ensure** — changelog_ao_debug_restart, specs_2026_10_03_debugger_restart_design_fiber_unwind_restart, specs_2026_10_03_debugger_restart_design_call_context_top_frame, specs_2026_10_03_debugger_restart_design_no_inner_ensure, phases_p14_live_restart_tests [EXTRACTED 1.00]
- **Phase doc + design spec + plan trio for P13** — docs_phases_p13_evaluation_interrupt, docs_superpowers_specs_2026_09_30_evaluation_interrupt_design_design, docs_superpowers_plans_2026_09_30_evaluation_interrupt_plan, changelog_release_1_2_0 [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **Session tables excluded from liveness tracing and image save** — spec_method_cache, spec_source_table, spec_section_3_10_classid, spec_debugger, spec_live_subclass [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — docs_claude_review_00_recent_diff_classdef_string_silently_dropped, docs_claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, docs_claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, docs_claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **Live debugger resume operations (Proceed/Step/Restart/Accept)** — spec_debugger_restart, spec_debugger_accept [EXTRACTED 1.00]
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
- **Method removal pipeline: ao_remove_method → removeMethodNamed → removeKey / forgetMethodSource / cache invalidation** — docs_superpowers_plans_2026_09_26_p12_browser_remove_removemethodnamed, docs_superpowers_plans_2026_09_26_p12_browser_remove_removekey, docs_superpowers_plans_2026_09_26_p12_browser_remove_forgetmethodsource [INFERRED 0.85]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — docs_prs_p1_02_pr, docs_prs_p1_03_pr, docs_prs_p1_04_pr, docs_prs_p1_05_pr, docs_prs_p1_06_pr, docs_prs_p1_07_pr [INFERRED 0.85]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — docs_prs_p4_01_doc, docs_prs_p4_02_doc, docs_prs_p4_03_doc, docs_prs_p4_04_doc, docs_prs_p4_05_doc, docs_prs_p4_06_doc, docs_prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunkparser [INFERRED 0.85]
- **1.1.0 release versioning contract (semver minor bump, version_string, assets, Info.plist)** — spec_minor_version_bump_post_v1, spec_release_semver, spec_version_string_source_of_truth, spec_release_assets, spec_info_plist_version [INFERRED 0.85]
- **Session Class ID Identity Mechanism** — runtime_src_session_classids, runtime_src_session_classrows, runtime_src_session_browserclassid, bridge_ao_abi_ao_browser_class_id, runtime_tests_browser_abi_test_browserabi, docs_superpowers_plans_2026_09_27_p12_class_identity [INFERRED 0.85]
- **MethodDictionary Malformed-Shape Defense** — runtime_src_methoddictionary_pairarray, runtime_src_methoddictionary_at, runtime_src_methoddictionary_atput, runtime_src_methoddictionary_removekey, runtime_tests_method_dictionary_test_malformeddictionaryanswersnilandtakesnothing, runtime_tests_remove_abi_test_malformedmethoddictionaryfallsbackonsend [INFERRED 0.90]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]

## Communities (357 total, 119 thin omitted)

### Community 0 - "TEST_F"
Cohesion: 0.01
Nodes (159): AbortAfterAcceptRunsOnlyOuterEnsure, AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, AcceptAfterTerminateElsewhereIsRefused, AcceptClassSideMethod, AcceptCompileErrorChangesNothing, AcceptDoItFrameIsRefused, AcceptFailureKeepsResultSnapshotAndInterrupt (+151 more)

### Community 1 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 2 - "TEST_F"
Cohesion: 0.03
Nodes (73): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+65 more)

### Community 3 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 4 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (84): Pass, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+76 more)

### Community 5 - "TEST"
Cohesion: 0.02
Nodes (80): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+72 more)

### Community 6 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 7 - "TEST"
Cohesion: 0.02
Nodes (99): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+91 more)

### Community 8 - "CallContext"
Cohesion: 0.04
Nodes (50): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, acceptSlots (+42 more)

### Community 9 - "MethodSource"
Cohesion: 0.07
Nodes (37): `Object>>halt`（P10-03）, P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）, SPEC を先に直す（P10-01）, リスク, 仕様, 手順（PR の順序と依存）, 検証, 範囲 (+29 more)

### Community 10 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (72): Digits, 項目, __int128, H11. `Fraction` 正規化時の `truncDiv` / `neg` OOM（空 OOP）未検査, add(), addBig(), asInt64IfFits(), Big (+64 more)

### Community 11 - "Oop"
Cohesion: 0.09
Nodes (68): Graphify / Serena, C2. `Object>>basicAt:` / `basicAt:put:` の `kFlagBytes` 未検証によるヒープ OOB 読み書き・OOP 偽造, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers() (+60 more)

### Community 12 - "Stream.cpp"
Cohesion: 0.10
Nodes (69): allocateInstance(), allocateRetry(), CallContext, Roots, uint16_t, uint32_t, RootedArray::RootedArray(), ao_PositionableStream_contents() (+61 more)

### Community 13 - "DebuggerWindow"
Cohesion: 0.05
Nodes (52): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .canAcceptEdit, .errorText, .frameLabels, .hasUnacceptedChanges (+44 more)

### Community 14 - "BrowserModel"
Cohesion: 0.16
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 15 - "Parser"
Cohesion: 0.09
Nodes (29): uint32_t, SourceSpan, end, start, Kind, size_t, string, string_view (+21 more)

### Community 16 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 17 - "Emitter"
Cohesion: 0.14
Nodes (17): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+9 more)

### Community 18 - "AcceptTests"
Cohesion: 0.11
Nodes (14): spanMessage(), AcceptTests, AnswerBox, Bool, Int32, NSMenu, NSSegmentedControl, NSTableView (+6 more)

### Community 19 - "image_save_load_test.cpp"
Cohesion: 0.08
Nodes (32): algorithm, Bootstrap, cmath, cstring, future, gc, install, iterator (+24 more)

### Community 20 - "ao_eval"
Cohesion: 0.05
Nodes (60): BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:), BrowserWindow.removeAfterConfirming(...), BrowserWindow.removeMethod / removeMethod(atRow:), testRemoveMethodAfterConfirmUpdatesLists, ao_accept_method_id, ao_remove_class, ao_remove_method (+52 more)

### Community 21 - "Boot"
Cohesion: 0.04
Nodes (56): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, string (+48 more)

### Community 22 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.11
Nodes (26): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+18 more)

### Community 23 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext, uint32_t (+2 more)

### Community 24 - "Heap"
Cohesion: 0.04
Nodes (75): R7 イメージの保存と読み込み（H5、M8、CLI の Low、`installMissing`）, SPEC を先に直す, 項目, Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_ (+67 more)

### Community 25 - "Scanner.cpp"
Cohesion: 0.07
Nodes (59): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CharacterLiteralNeedsWellFormedUtf8, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary (+51 more)

### Community 26 - ".isHeap"
Cohesion: 0.13
Nodes (18): ソース表の拡張（P10-04）, clearWeakAfterOldMark, string_view, isVendorStub(), copyMethodDictionary(), liveClasses(), Gc::collectOld(), Gc::copy() (+10 more)

### Community 27 - "TEST"
Cohesion: 0.04
Nodes (67): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, AdoptionResetsDamagedReadyQueue, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, BlockingWaitSwitchesPastDamagedReadyQueue, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase (+59 more)

### Community 28 - "TEST"
Cohesion: 0.04
Nodes (60): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+52 more)

### Community 29 - "RealScope"
Cohesion: 0.08
Nodes (27): Capture, owner, var, uint32_t, RealScope, copied, copiedBase, isBlock (+19 more)

### Community 30 - "abi.cpp"
Cohesion: 0.07
Nodes (73): EvaluationActivity, .isActive, Bool, atomic, AoSpan, end, message, start (+65 more)

### Community 31 - "resumeHalted"
Cohesion: 0.20
Nodes (23): H4. 停止中のスロット改変に対する `sessionDebugAccept` / `acceptTarget` / `stillHalted` の境界検証欠落, boxMethodImage, answerAwaited(), answerEval(), blankOut(), clearInterruptRequest(), AoInspectFn, EvalEnd (+15 more)

### Community 32 - "Scheduler"
Cohesion: 0.04
Nodes (50): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+42 more)

### Community 33 - "TEST"
Cohesion: 0.04
Nodes (60): ArrayCollectDoublesViaNativeBlock, ArrayedDoWithNonSmallIntegerSizeFails, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails (+52 more)

### Community 34 - ".nil"
Cohesion: 0.11
Nodes (55): R4 Kernel ネイティブの失敗の伝搬（H12、H10、H11、M6、M12、M25、M16、safepoint の Low）, slotAt, slotAtPut, enqueue, find, listsDamaged, reapDead, signal (+47 more)

### Community 35 - "ChangeSorterWindow"
Cohesion: 0.09
Nodes (21): ChangeSorterWindow, .canMakeCurrent, .errorText, .paneAccessibilityLabels, .sourceText, Bool, Notification, NSButton (+13 more)

### Community 36 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (25): AnyObject, installDebugButton(), sendToKeyWorkspace(), Bool, Int, MainActor, NSButton, NSTextField (+17 more)

### Community 37 - "Heap.cpp"
Cohesion: 0.09
Nodes (45): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+37 more)

### Community 38 - "TEST"
Cohesion: 0.03
Nodes (64): AcceptedSourceSurvivesSaveAndLoad, AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal (+56 more)

### Community 39 - "TEST"
Cohesion: 0.07
Nodes (30): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+22 more)

### Community 40 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (46): cerrno, climits, csignal, fcntl, appendRaw(), appendRecord(), appendSources(), collectImageSlot() (+38 more)

### Community 41 - "TEST_F"
Cohesion: 0.04
Nodes (61): AliasedOldClassRowActsOnItsOwnClass, ClassIdTableDoesNotBlockSuperclassRemoval, KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, MalformedMethodDictionaryFallsBackOnSend, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad, RemoveAcceptedMethodOnKernelClass (+53 more)

### Community 42 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 43 - "Send.cpp"
Cohesion: 0.09
Nodes (47): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 範囲 (+39 more)

### Community 44 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 45 - "ToolWindowTests"
Cohesion: 0.09
Nodes (12): LaunchSet, Bool, Int32, NSFont, NSMenu, NSMenuItem, NSTextView, NSView (+4 more)

### Community 46 - "isClassShaped"
Cohesion: 0.13
Nodes (39): Global Constraints, hasSlots(), uint32_t, isClassShaped(), superclassOf(), SuperclassWalk, depth_, MethodDictionary::at (+31 more)

### Community 47 - "TEST_F"
Cohesion: 0.05
Nodes (39): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+31 more)

### Community 48 - "DebugFrames"
Cohesion: 0.07
Nodes (45): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context, count, empty, kind, method, pc (+37 more)

### Community 49 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 50 - ".isSmallInteger"
Cohesion: 0.17
Nodes (25): int64_t, bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t (+17 more)

### Community 51 - "P12 Browser 削除 実装計画"
Cohesion: 0.07
Nodes (39): P12 — Browser の削除 (phase doc), Addendum tests (browser_abi_test, accept_abi_test, session_abi_test, method_dictionary_test, BrowserModelTests), P12 Addendum: Class ID and Broken Method Dictionary (2026-09-27), Method dictionary removal (nil pair, decrement tally), MethodDictionary::pairArray (shape check), Class removal unbinds name only, BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove (+31 more)

### Community 52 - "DebuggerWindowTests"
Cohesion: 0.10
Nodes (16): AnswerBox, DebuggerWindowTests, Int, NSButton, NSTableView, NSWindow, String, Void (+8 more)

### Community 53 - "ImageFormat"
Cohesion: 0.11
Nodes (18): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+10 more)

### Community 54 - "DebugSnapshot"
Cohesion: 0.06
Nodes (28): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+20 more)

### Community 55 - "Session.cpp"
Cohesion: 0.07
Nodes (40): Task 3: セッションのソースを保存し、ロードで戻す, ロード, bumpDebugGeneration(), clearMethodSources(), AoRunLoopPumpFn, CallContext, HostOopHook, ImageSource (+32 more)

### Community 56 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 57 - "String"
Cohesion: 0.07
Nodes (20): .classTitles, .methodTitles, .setTitles, EntryTarget, Any, Element, Int, Int32 (+12 more)

### Community 58 - "WellKnown.cpp"
Cohesion: 0.10
Nodes (30): findSymbol, global, internWith, isFixedGlobal, isPseudoVariableName, Roots, string_view, WellKnown (+22 more)

### Community 59 - "TEST_F"
Cohesion: 0.04
Nodes (78): AcceptRecordsAddedThenChangedInANewSet, BootAndImageLoadStartOver, BrokenModelLeavesAcceptAndRemovalAnswers, ClassDefinitionAcceptIsChangedForAnExistingClass, ClassSideAcceptIsRecordedOnTheClassSide, DebuggerAcceptIsRecorded, DebuggerAcceptRefusesWhenTheRecordTerminatesTheProcess, DropAndMoveAreRefusedWhileBusy (+70 more)

### Community 60 - "AoApp"
Cohesion: 0.10
Nodes (22): AoApp, fileInChangeFile(), fileInLoadOrder(), fileInToolsModel(), fileInVendor(), openImageFile(), saveImageFile(), Bool (+14 more)

### Community 61 - "ImageHeader"
Cohesion: 0.10
Nodes (23): bit, uint16_t, uint32_t, uint64_t, ImageHeader, endian, extraCount, globalCount (+15 more)

### Community 62 - "DebugSnapshot.cpp"
Cohesion: 0.13
Nodes (40): frameAt, flags, contextPc(), CallContext, Frame, string, uint32_t, DebugSnapshot::capture() (+32 more)

### Community 63 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 64 - "アプリ"
Cohesion: 0.09
Nodes (28): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String (+20 more)

### Community 65 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 66 - ".false_"
Cohesion: 0.15
Nodes (36): H12. `Collection` / `Stream` ネイティブ内での OOM 握りつぶし・`unwinding` 検査漏れ・空 OOP 未検知, boolean(), ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString() (+28 more)

### Community 67 - "TEST"
Cohesion: 0.06
Nodes (49): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+41 more)

### Community 68 - "CallContext"
Cohesion: 0.12
Nodes (29): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), valueHashWord(), ao_Array_equals() (+21 more)

### Community 69 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 70 - "NativeMethod.cpp"
Cohesion: 0.19
Nodes (22): R5 反射のあとのメモリ安全（C2、NativeMethod の Low、M17）【D1】, SPEC を先に直す, 項目, add(), addNamed(), apply(), CallContext, NativeFn (+14 more)

### Community 71 - "TEST"
Cohesion: 0.09
Nodes (22): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+14 more)

### Community 72 - "TEST"
Cohesion: 0.06
Nodes (38): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+30 more)

### Community 73 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 74 - "size"
Cohesion: 0.26
Nodes (15): allocateNoGc, size, at(), bind(), bindIn(), Roots, string_view, uint32_t (+7 more)

### Community 75 - ".testDebuggerControlsHaveAccessibilityLabels"
Cohesion: 0.28
Nodes (5): NSFont, NSTextView, NSView, T, UInt

### Community 76 - "VendorExtract.cpp"
Cohesion: 0.21
Nodes (23): 検証結果と重要度, 潜在 / 公開 API 契約欠陥（5 件）, allowIndex(), containsHostWord(), string_view, extractVendor(), firstLineKey(), firstNonEmptyLine() (+15 more)

### Community 77 - "§3.9 Removal (Browser deletion rules)"
Cohesion: 0.08
Nodes (34): ao_accept_class, ao_accept_method_id, Ao Bytecode Set, ao_eval, ao_remove_class, ao_remove_method, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation) (+26 more)

### Community 78 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending, evalMode (+20 more)

### Community 79 - "TEST"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 80 - "BrowserModelTests"
Cohesion: 0.18
Nodes (10): BrowserModelTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+2 more)

### Community 81 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.06
Nodes (63): docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, SPEC.md, Codegen.cpp: Emitter::mark と compile* 群, MethodImage.hpp: PcSpan / TempName / pcMap / temps (+55 more)

### Community 82 - "High（12 件）"
Cohesion: 0.12
Nodes (15): R1（PR #33、マージ済み）, R2（PR #34、マージ済み）, R4（PR #36、マージ待ち）, review-agy 修正の中間報告（2026-11-26 R4 完了時点）, 再開するとき, 判断したこと（作業記録の Ruling）, 後のバッチに回したもの, 手順からのずれ (+7 more)

### Community 83 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 84 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (29): safepoint, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual() (+21 more)

### Community 85 - "TEST"
Cohesion: 0.08
Nodes (27): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+19 more)

### Community 86 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 87 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 88 - "TEST_F"
Cohesion: 0.06
Nodes (31): BasicAtPutRefusesFormatThatDisagreesWithObject, CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DnuWithoutRoomForMessageIsOutOfMemory (+23 more)

### Community 89 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.08
Nodes (37): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+29 more)

### Community 90 - "TranscriptWindow"
Cohesion: 0.10
Nodes (20): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int32, NSRect (+12 more)

### Community 91 - "MainMenu"
Cohesion: 0.15
Nodes (15): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+7 more)

### Community 92 - "ChangeSet.cpp"
Cohesion: 0.29
Nodes (27): ctime, bytesText(), changesSession(), countOf(), int64_t, Root, size_t, elementAt() (+19 more)

### Community 93 - "FiberStackBounds"
Cohesion: 0.38
Nodes (7): AO_FIBER_REAL_FRAME, fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitchFinal(), threadStackBounds()

### Community 94 - "ChangeSorterModel"
Cohesion: 0.12
Nodes (30): ChangeClassRow, ChangeKind, added, changed, .mark, none, removed, ChangeMethodRow (+22 more)

### Community 95 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 96 - "Fiber.cpp"
Cohesion: 0.13
Nodes (18): asan_interface, common_interface_defs, mman, pthread, array, array, size_t, fiberEntered() (+10 more)

### Community 97 - "前提"
Cohesion: 0.15
Nodes (15): P18 — メソッドソースの永続化（設計）, テスト, 保存, 利用者との合意（2026-10-12）, 前提, 形式（版 4）, 振る舞いの変化, 方式 (+7 more)

### Community 98 - "FiberStack"
Cohesion: 0.14
Nodes (18): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+10 more)

### Community 99 - "TEST"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 100 - "TEST"
Cohesion: 0.05
Nodes (45): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+37 more)

### Community 101 - "CompileEnv"
Cohesion: 0.11
Nodes (16): Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, CompileResult (+8 more)

### Community 102 - "Compiler (.st to CompiledMethod)"
Cohesion: 0.22
Nodes (11): Host GUI (AppKit), Chunk Format, Closures and Shared Temps, CompiledMethod, Compiler (.st to CompiledMethod), Debugger (§3.13), Debugger Window, v1 Non-goals (+3 more)

### Community 103 - "LiveFrames"
Cohesion: 0.09
Nodes (22): CallContext, DebugSink, onAbort, Frame, size_t, string, LiveFrames, context (+14 more)

### Community 104 - "BrowserWindow"
Cohesion: 0.06
Nodes (36): BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText (+28 more)

### Community 105 - "hasSlots"
Cohesion: 0.32
Nodes (22): H10. `Semaphore class>>new` / `SharedQueue class>>new` の `send()` 後 `unwinding` / 空 OOP 検査欠落, ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend() (+14 more)

### Community 106 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 107 - "UniformFont"
Cohesion: 0.14
Nodes (11): Void, Int, NSFont, NSRange, ToolTextSize, .offset, UniformFont, NSObject (+3 more)

### Community 108 - ".base"
Cohesion: 0.11
Nodes (28): abandonAll, addFiber, awaitEval, drain, findId, isHalted, liveFibers, proceed (+20 more)

### Community 109 - "P4 — Kernelネイティブ実装"
Cohesion: 0.16
Nodes (18): P4 — Kernelネイティブ実装, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード (+10 more)

### Community 110 - "stream_test.cpp"
Cohesion: 0.06
Nodes (41): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+33 more)

### Community 111 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 112 - "pc_map_test.cpp"
Cohesion: 0.13
Nodes (21): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, uint32_t, firstBlock(), named() (+13 more)

### Community 113 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 114 - "fiber_test.cpp"
Cohesion: 0.16
Nodes (14): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), TEST(), mach (+6 more)

### Community 115 - "OperandStack"
Cohesion: 0.12
Nodes (18): フレーム連鎖（P10-03）, deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots (+10 more)

### Community 116 - "string"
Cohesion: 0.16
Nodes (27): Task 5: File Out, Task 2: ネイティブ `Behavior>>sourceCodeAt:` と `Class>>definition`, boundClassNamed(), browserClassDefinition(), byteText(), categoryHeading(), classDefinitionText(), classNameOf() (+19 more)

### Community 117 - "TEST"
Cohesion: 0.09
Nodes (23): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, ClassSideCommentStampHeaderMakesNextChunkProse, CommentStampApostropheDoesNotSwallowClassDef, CommentStampMethodInSectionDoesNotMakeNextChunkProse (+15 more)

### Community 118 - ".isEmpty"
Cohesion: 0.15
Nodes (19): BrowserModel.copyClass(at:), ao_browser_class_at, P12 追補: クラス ID と壊れたメソッド辞書 実装計画, SPEC を先に直す, Design rationale: session class ID table, Design rationale: MethodDictionary shape check (pairArray), at(), atPut() (+11 more)

### Community 119 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 120 - "review-agy 指摘の修正計画"
Cohesion: 0.12
Nodes (16): Global Constraints, R2 App: Debugger の窓、ツールの窓、スクリプト、CI（H1、H2、M18〜M21、app の Low）, R8 ABI と GC の潜在欠陥（M9、M10、M22、M23、M24、M26、`pushHandle`）, review-agy 指摘の修正計画, Review Focus, SPEC を先に直す, SPEC を先に直す, バッチの順序と依存 (+8 more)

### Community 121 - "P1 — オブジェクトメモリ"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 122 - "FiberRegs"
Cohesion: 0.20
Nodes (10): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+2 more)

### Community 123 - "VirtualRegion.cpp"
Cohesion: 0.21
Nodes (12): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+4 more)

### Community 124 - "native_method_test.cpp"
Cohesion: 0.19
Nodes (17): AddInternsByFunctionPointer, ApplyRefusesTooFewSlots, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn (+9 more)

### Community 125 - "P11 — ライブデバッガ"
Cohesion: 0.29
Nodes (7): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 範囲, 結論

### Community 126 - "ClassPool.cpp"
Cohesion: 0.20
Nodes (24): hashedcollection, adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view (+16 more)

### Community 127 - "TEST"
Cohesion: 0.11
Nodes (19): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, FiledOutCommentChunksAreSkipped (+11 more)

### Community 128 - "ao_runtime_boot"
Cohesion: 0.04
Nodes (56): AbiSmoke, AoTranscriptFn, BootAndShutdownReturnZero, BootVersionShutdown, 仕様（設計判断）, Qiita 後編: 協調スケジューラとファイバ切り替え, Task 4: 項目を忘れる・移す（Remove from Change Set / Move to）, ao_filein_load_order() (+48 more)

### Community 129 - "Ao implementation docs index (phases and PRs)"
Cohesion: 0.07
Nodes (44): vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, P8 — AppKit ツール, P9 — 統合, Kernel NativeMethod 走査, P6b-01: Cuis pin と ORIGIN.md, Cuis vendor pin (ORIGIN.md) (+36 more)

### Community 130 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 131 - "設計判断"
Cohesion: 0.25
Nodes (8): Change Sorter（Swift）, Smalltalk 側のクラス, SPEC の変更点, クラス側・インスタンス側の入口, ネイティブ（Kernel、C++）, 拒否の理由は 1 か所, 置き場と読み込み, 設計判断

### Community 132 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 133 - "review-agy 修正の引継ぎ（2026-11-26、R6 の PR 作成後）"
Cohesion: 0.25
Nodes (7): R6 で決めたこと（確認してほしいもの）, R7 以降に持ち越したもの, review-agy 修正の引継ぎ（2026-11-26、R6 の PR 作成後）, いまの状態, 次にやること, 環境の注意, 結論

### Community 134 - "AppKit"
Cohesion: 0.10
Nodes (15): Ao, aoRunLoopPumpHook(), RunLoopPump, UnsafeMutableRawPointer, BusyBody, busyBodyHook(), CChar, Int32 (+7 more)

### Community 135 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 136 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): beginsWithHeaderKeyword(), classify(), string, string_view, Token, uint32_t, isBlank(), isCharacterLiteral() (+16 more)

### Community 137 - "Session"
Cohesion: 0.06
Nodes (70): assignClassIds(), browserClassAt(), browserClassCount(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+62 more)

### Community 138 - "Release 1.0.0 (v1, P0-P9)"
Cohesion: 0.29
Nodes (7): .aoimage save/load with native rebinding, Pinned Cuis-Smalltalk excerpt (image/vendor), Exact generational GC (nursery + mark-compact old), Kernel classes as C++ NativeMethods, Release 1.0.0 (v1, P0-P9), 64-bit tagged ao::Oop, Rules that do not bend

### Community 139 - "applyMethod"
Cohesion: 0.19
Nodes (23): P15 — ライブ Debugger の編集, TDD, 前提, 範囲, 結論, File Structure, Global Constraints, P15 Debugger の編集 — 実装計画 (+15 more)

### Community 140 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, BasicNewOnByteObjectWithMetaclassClassFails, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain (+13 more)

### Community 141 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 142 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 143 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 144 - "ImageSurgery"
Cohesion: 0.23
Nodes (12): size_t, string_view, uint64_t, emptyOrNoScalar(), emptyOrNoScalarWords(), ImageSurgery, bytes, header (+4 more)

### Community 145 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 146 - "MethodDebugInfo"
Cohesion: 0.12
Nodes (17): uint32_t, PcSpan, end, pc, start, DebugInfoRef, body, index (+9 more)

### Community 147 - "String"
Cohesion: 0.25
Nodes (8): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Unicode

### Community 148 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 149 - ".isTrue"
Cohesion: 0.35
Nodes (12): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+4 more)

### Community 150 - "Gc.cpp"
Cohesion: 0.14
Nodes (13): cstdlib, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::scavengeFromRoots(), optional (+5 more)

### Community 151 - "string"
Cohesion: 0.23
Nodes (18): size_t, string, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars (+10 more)

### Community 152 - "TEST_F"
Cohesion: 0.19
Nodes (12): ClassDefinitionAnswersTheBrowserText, DefinitionIsNotUnderstoodByAMetaclass, acceptClass(), acceptMethod(), string, testing::Test, literal(), printIt() (+4 more)

### Community 153 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 154 - "Release / Semantic Versioning"
Cohesion: 0.19
Nodes (13): version_string(), package-app.sh script, Acceptance Criteria (§6), AcceptTests Remove/ContextMenu XCTests, CI (GitHub Actions), Golden Evaluation (ao --test), Info.plist CFBundleShortVersionString/CFBundleVersion from ao --version (1.0.0 for v1, 1.1.0 through P12), GitHub Release assets (Ao-<ver>-macos-arm64.zip, ao-cli-<ver>-macos-arm64.tar.gz, SHA256SUMS) (+5 more)

### Community 155 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 156 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 157 - "ao_accept_method"
Cohesion: 0.10
Nodes (20): BrowserModel.classID(named:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.removeClass / removeClass(atRow:), BrowserWindow.showDefinedClass(from:) (+12 more)

### Community 158 - "Interpreter.cpp"
Cohesion: 0.17
Nodes (30): branchTruth(), byteCount(), clearNonlocal(), consumeNonlocal(), contextAlive(), CallContext, int16_t, uint32_t (+22 more)

### Community 159 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 160 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 161 - "uint64_t"
Cohesion: 0.24
Nodes (17): byte, uint64_t, unordered_map, headerAt(), heapShaped(), bits, objectName(), ObjectRules (+9 more)

### Community 162 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 163 - "path"
Cohesion: 0.14
Nodes (15): path, string, uint32_t, expectDamaged(), expectLoads(), expectRefused(), fileNames(), findBytesOfSize() (+7 more)

### Community 164 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 165 - "Ast"
Cohesion: 0.08
Nodes (36): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+28 more)

### Community 166 - "clearUnwinding"
Cohesion: 0.09
Nodes (31): P17 — Smalltalk のチェンジセット（設計）, やらないこと, テスト, 利用者との合意（2026-10-12）, 前提, 方式, 結論, 計画で詰めること (+23 more)

### Community 167 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 168 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 169 - "Debugger Accept (reactivate frame with new method)"
Cohesion: 0.25
Nodes (8): ao_debug_accept, ao_debug_can_accept, ao_debug_restart, Debugger Accept (reactivate frame with new method), Debugger Restart (frame from start), Halt reason `accepted`, P14 Restart Phase, P15 Debugger Edit Phase

### Community 170 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 171 - ".aoimage Image Format"
Cohesion: 0.12
Nodes (16): Failure Aborts Evaluation, allocateRetry, ao_image_load, AoSpan.message (error reason), .aoimage Image Format, Base Process, Cooperative Process Scheduler (fibers), ensureKernelNatives (+8 more)

### Community 172 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 173 - "TEST"
Cohesion: 0.22
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 174 - "Gc"
Cohesion: 0.11
Nodes (27): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, collectBeforeTenured, collectNursery (+19 more)

### Community 175 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 176 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 177 - "send2"
Cohesion: 0.12
Nodes (19): ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RebindsBagAndEvaluatesInstVar, RectangleWidthHeightContainsAndIntersect (+11 more)

### Community 178 - "前提（コードで確かめた事実。`b35efe8`）"
Cohesion: 0.11
Nodes (58): ChunkMode, P16 チェンジセットと Change Sorter — 実装計画, Task 0: ブランチ, Task 1: SPEC を先に直す（P16 の追記）, Task 3: Accept と削除を記録する, 前提（コードで確かめた事実。`b35efe8`）, File Structure, P17 Smalltalk のチェンジセット — 実装計画 (+50 more)

### Community 179 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 180 - "ImageLoad.cpp"
Cohesion: 0.15
Nodes (38): Task 2: 版 4 のヘッダと空のソース節, acceptWord(), atOffset(), checkFile(), size_t, string, T, uint32_t (+30 more)

### Community 181 - "CompileError"
Cohesion: 0.11
Nodes (25): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutGrowsWhenTallyLies, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxRecountsAndInstalls, CompileError, message, span (+17 more)

### Community 182 - "Analysis"
Cohesion: 0.13
Nodes (15): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+7 more)

### Community 183 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 184 - "Ao README"
Cohesion: 0.07
Nodes (54): ao_debug_restart / ao_debug_can_restart, AO_ERR_HALT, ao_remove_method / ao_remove_class, ao_request_interrupt, ao_set_runloop_pump_hook, Browser session class ID (ao_browser_class_id), CHANGELOG, Known limitations (no JIT/FFI, unoptimized interpreter, uninterruptible natives) (+46 more)

### Community 185 - "string"
Cohesion: 0.14
Nodes (27): chrono, Chunk, classpool, compile, CompiledMethod, compiler, string, context (+19 more)

### Community 186 - "Image::load"
Cohesion: 0.26
Nodes (12): ifstream, bindAll(), checkGlobals(), ImageSource, Roots, string_view, WellKnown, fileOop() (+4 more)

### Community 187 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 188 - "P0 (親フェーズ, stub)"
Cohesion: 0.26
Nodes (12): P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+4 more)

### Community 189 - "vector"
Cohesion: 0.10
Nodes (42): cctype, vector, FileInError, error, file, method, string, addError() (+34 more)

### Community 190 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (24): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+16 more)

### Community 191 - "Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）"
Cohesion: 0.39
Nodes (11): Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）, boolean(), CallContext, string, string_view, noteClassChange(), noteClassRemoval(), noteMethodChange() (+3 more)

### Community 193 - "P3 — ネイティブディスパッチ"
Cohesion: 0.16
Nodes (15): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary (+7 more)

### Community 195 - "Scheduler::halt"
Cohesion: 0.26
Nodes (12): afterResume, canHalt, leaveLists, switchTo, takeNext, string, Scheduler::abandonAll(), Scheduler::block() (+4 more)

### Community 196 - "P18 — メソッドソースの永続化"
Cohesion: 0.29
Nodes (7): P18 — メソッドソースの永続化, TDD, 前提, 受け入れ, 手動確認, 範囲, 結論

### Community 197 - "Literal"
Cohesion: 0.07
Nodes (33): int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal (+25 more)

### Community 198 - "ao_main.cpp"
Cohesion: 0.16
Nodes (21): Task 0: ブランチ, Task 3: Smalltalk のモデル（記録・種類・理由・Remove / Move）と Tools-model の file-in, dyld, runtime, addRoots, bootAndRunTests(), collectLoadOrders(), CallContext (+13 more)

### Community 199 - ".isBase"
Cohesion: 0.20
Nodes (10): terminateAll, Roots, size_t, nextProcessId(), rootRecord(), Scheduler::haltedCount(), Scheduler::liveFibers(), Scheduler::reapDead() (+2 more)

### Community 200 - "NativeMethod.hpp"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 202 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.40
Nodes (5): PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 203 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 204 - "Vendor Class Library (import, do not write)"
Cohesion: 0.20
Nodes (10): CLAUDE.md, Bootstrap Procedure, Change Rules (§8), Constraints (§5), Cuis Smalltalk, Graphify and Serena Requirement, Kernel Classes (native-only), v1 Completion Definition (+2 more)

### Community 205 - "Frame"
Cohesion: 0.33
Nodes (6): ContextExitGuard, Frame, Roots, FieldRoots, frame, FrameLink

### Community 206 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 207 - "DebuggerWindowTests.swift"
Cohesion: 0.32
Nodes (7): interruptOncePump(), selectOtherPump(), CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, transcriptBoxHook()

### Community 208 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 209 - "Graphify Required Tool"
Cohesion: 0.67
Nodes (3): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps)

### Community 210 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 211 - "ao CMake Project"
Cohesion: 0.67
Nodes (4): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target)

### Community 213 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 216 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 217 - "compiler/tests/smoke_test.cpp"
Cohesion: 0.40
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneSevenZero

### Community 218 - "VendorClassFile"
Cohesion: 0.25
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 219 - "b5Definition"
Cohesion: 0.29
Nodes (7): ao_accept_class, b5Definition(), ClassDefinitionThenImageDropsSourceText, ReacceptWithNewInstanceVariableRecompilesMethods, ShapeChangePutsNameBackWhenSubclassSendFails, ShapeChangeRefusesClassWithSubclasses, ShapeChangeRefusesSourcelessMethodAndKeepsOldClass

### Community 220 - "項目"
Cohesion: 0.43
Nodes (7): R3 ライブデバッガのランタイム（H4、H7、M7、H6、H3、M4、M5）, SPEC を先に直す, 項目, H7. `clearUnwinding` の `disarmAccept` 呼び出し欠落による `acceptSlots` ピン残留・GC UAF, run, disarmAccept(), reactivateAccepted()

### Community 221 - "HostMethod"
Cohesion: 0.33
Nodes (7): doubleBangs(), HostMethod, protocol, source, quoteSmalltalk(), render(), trimTrailingWs()

### Community 223 - "test (macOS arm64) Job"
Cohesion: 0.67
Nodes (3): Kernel Scan Test (via ctest), test (macOS arm64) Job, CI Workflow

### Community 224 - "P17 — Smalltalk のチェンジセット"
Cohesion: 0.33
Nodes (5): P17 — Smalltalk のチェンジセット, TDD, 前提, 範囲, 結論

### Community 225 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 240 - "OcSlots"
Cohesion: 0.33
Nodes (6): int64_t, OcSlots, array, first, last, Scheduler::fiberEntry()

### Community 247 - "boxLiteral"
Cohesion: 0.47
Nodes (6): boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), uint32_t, uint8_t

### Community 248 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 249 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 250 - "createBlock"
Cohesion: 0.60
Nodes (4): CallContext, uint8_t, createBlock(), createMethod()

### Community 251 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 252 - "ParseResult"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 253 - "3. 確定指摘一覧"
Cohesion: 0.50
Nodes (4): 3. 確定指摘一覧, Critical（2 件）, Medium（26 件）, 指摘件数サマリー

### Community 254 - "acceptMethodInto (definition)"
Cohesion: 0.50
Nodes (4): acceptMethodSource, MethodDictionary::atPut, acceptMethodInto (definition), installMethod (definition)

### Community 255 - "ImageSource"
Cohesion: 0.50
Nodes (4): string, ImageSource, method, text

### Community 256 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 257 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

### Community 258 - "cstdint"
Cohesion: 0.07
Nodes (17): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, Roots (+9 more)

### Community 259 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 260 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

### Community 266 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 269 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 340 - "TEST"
Cohesion: 0.67
Nodes (3): RemoveUnits, TEST(), UndefineUnbindsAndMovesTheGlobalsVersion

### Community 343 - "ImageSelector"
Cohesion: 0.67
Nodes (3): ImageSelector, name, WellKnown

## Ambiguous Edges - Review These
- `Debug it (AO_EVAL_DEBUGIT)` → `SPEC §3.13 デバッガ（捕捉の意味論）`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `P5-01: 字句解析` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P8-01: Ao.app 骨格` → `P7-03: load と NativeMethod 再結合`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `P8-05: メニューとキー` → `P9-01: Do it / Print it / Inspect it`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to

## Knowledge Gaps
- **1103 isolated node(s):** `.selectedClass`, `.metaFlag`, `.canRemoveMethod`, `.canRemoveClass`, `.hasUnacceptedChanges` (+1098 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2960 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **119 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Debug it (AO_EVAL_DEBUGIT)` and `SPEC §3.13 デバッガ（捕捉の意味論）`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P5-01: 字句解析` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P8-01: Ao.app 骨格` and `P7-03: load と NativeMethod 再結合`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P8-05: メニューとキー` and `P9-01: Do it / Print it / Inspect it`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
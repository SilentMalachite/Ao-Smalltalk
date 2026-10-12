# Graph Report - ao-smalltalk  (2026-10-12)

## Corpus Check
- 318 files · ~422,465 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 83 file(s) not represented in the graph (top: .st 67, (none) 13, .toml 1)

## Summary
- 6806 nodes · 18658 edges · 321 communities (204 shown, 117 thin omitted)
- Extraction: 84% EXTRACTED · 16% INFERRED · 0% AMBIGUOUS · INFERRED: 2936 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `7abcb13e`
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
- Analysis
- LargeInteger.cpp
- Oop
- Stream.cpp
- DebuggerWindow
- BrowserModel
- Parser
- TEST
- Emitter
- AcceptTests
- abi.cpp
- CHANGELOG
- Boot
- image_save_load_test.cpp
- applyMethod
- Heap
- Scanner.cpp
- Frame
- TEST
- TEST
- MethodSource
- ChangeSorterTests
- Session.cpp
- ao_runtime_shutdown
- TEST
- Scheduler
- cstdint
- .replaceText
- Heap.cpp
- TEST
- checkFile
- ImageSave.cpp
- TEST_F
- reshapeClass
- unwinding
- Float.cpp
- ToolWindowTests
- allocateRetry
- TEST_F
- DebugFrames
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- .nil
- P12 Browser 削除 実装計画
- DebuggerWindowTests
- ImageFormat
- DebugSnapshot
- Session
- TEST_F
- ChangeSorterWindow
- WellKnown.cpp
- TEST_F
- AoApp
- putNative
- ChangeSet.cpp
- Geometry.cpp
- WorkspaceWindow
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- .isSmallInteger
- TEST
- Array.cpp
- Scheduler.cpp
- NativeMethod.cpp
- TEST
- TEST
- Boolean.cpp
- vector
- .debugAfterDoIt
- VendorExtract.cpp
- ao_remove_class Rules
- Scheduler::Record
- TEST
- BrowserModelTests
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- DebugSnapshot.cpp
- Roots
- SmallInteger.cpp
- TEST
- TEST
- Ast
- TEST_F
- Claude Review Fixes Plan (2026-09-23)
- VendorClassFile
- MainMenu
- fiber_test.cpp
- P6b — vendor file-in
- ChangeSorterModel
- Natives.hpp
- Literal
- CallContext
- FiberStack
- Gc.cpp
- TEST
- Image::load
- Compiler (.st to CompiledMethod)
- LiveFrames
- BrowserWindow
- Process.cpp
- P5 — コンパイラ
- Table
- TEST
- Fiber.cpp
- TEST_F
- TEST
- string
- Roots.cpp
- ao_accept_method
- Interpreter.cpp
- string
- TEST
- TEST
- TEST
- ClassDef
- P4 — Kernelネイティブ実装
- TEST
- TEST
- native_method_test.cpp
- P11 — ライブデバッガ
- ClassPool.cpp
- TEST
- Ao implementation docs index (phases and PRs)
- P8 — AppKit ツール
- DiskHeader
- VirtualRegion.cpp
- CountingSink
- clearUnwinding
- AppKit
- ChunkAction
- ChunkParser.cpp
- ClassId
- .whileActive
- FiberRegs
- TEST
- TEST
- TEST
- InspectorWindow
- CompileEnv
- Stack
- Release / Semantic Versioning
- String
- P2 — ブートストラップ
- .isTrue
- .aoimage Image Format
- makeToolTextWindow
- Ao README
- GarbageFirstBoot
- ClassMethodCache
- TEST
- §3.9 Removal (Browser deletion rules)
- .isHeap
- synthesizesNative
- ImageLoad.cpp
- FrameBlock
- PingPong
- ImageSurgery
- Token
- TEST
- Vendor Class Library (import, do not write)
- C ABI (bridge/ao_abi.h)
- FileSizeLimit
- Gc
- TEST
- send2
- Compile.cpp
- test.sh
- Class ID (session class-ID table)
- Loaded
- abortingSubclass
- Debugger Accept (reactivate frame with new method)
- TEST_F
- Counts
- b5Definition
- native_send_test.cpp
- parser
- P3 — ネイティブディスパッチ
- .specialSelector
- path
- Codegen.cpp
- ao_main.cpp
- Rec
- NativeMethod.hpp
- P8–P9 Remaining Implementation Plan
- popFrame
- TestDir
- Release 1.0.0 (v1, P0-P9)
- TEST
- Graphify Required Tool
- ao CMake Project
- interpreter
- TEST
- runtime_src_fiber
- string_view
- Roots::visitAll
- Debug it (AO_EVAL_DEBUGIT)
- Root
- imageRegistryStubA
- answerOne
- test (macOS arm64) Job
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
- uint64_t
- BrowserModel.applyHierarchyList(_:selecting:)
- BrowserModel.hierarchy(of:meta:)
- IgnoreFileSizeSignal
- TDD Test Suites
- ao_browser_class_count
- ao_browser_subclass_count
- ao_eval
- Fixed Design Decisions
- Build only what SPEC.md has (v1 and later §2.3 phases)
- compiler/src/Scanner.cpp chunk scanner (referenced, not read)
- いまの位置 (1.1.0、PHASE は P12)
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
- README.ja デバッガの使い方
- roots
- runtime_src_session
- MethodImage
- Roots
- CompileError
- runtime_src_session
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- ao_accept_method
- P13 Evaluation Interrupt Phase
- vector

## God Nodes (most connected - your core abstractions)
1. `Oop` - 977 edges
2. `Heap` - 256 edges
3. `vector` - 188 edges
4. `TEST_F()` - 187 edges
5. `WellKnown` - 168 edges
6. `Session` - 132 edges
7. `BrowserWindow` - 126 edges
8. `TEST()` - 112 edges
9. `TEST_F()` - 106 edges
10. `Ast` - 103 edges

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

## Communities (321 total, 117 thin omitted)

### Community 0 - "TEST_F"
Cohesion: 0.02
Nodes (145): AbortAfterAcceptRunsOnlyOuterEnsure, AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, AcceptAfterTerminateElsewhereIsRefused, AcceptClassSideMethod, AcceptCompileErrorChangesNothing, AcceptDoItFrameIsRefused, AcceptFailureKeepsResultSnapshotAndInterrupt (+137 more)

### Community 1 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 2 - "TEST_F"
Cohesion: 0.03
Nodes (89): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+81 more)

### Community 3 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 4 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (87): OcShape, Pass, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_() (+79 more)

### Community 5 - "TEST"
Cohesion: 0.02
Nodes (80): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+72 more)

### Community 6 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 7 - "TEST"
Cohesion: 0.03
Nodes (80): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+72 more)

### Community 8 - "CallContext"
Cohesion: 0.04
Nodes (49): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, acceptSlots (+41 more)

### Community 9 - "Analysis"
Cohesion: 0.05
Nodes (49): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+41 more)

### Community 10 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 11 - "Oop"
Cohesion: 0.08
Nodes (75): Graphify / Serena, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make() (+67 more)

### Community 12 - "Stream.cpp"
Cohesion: 0.11
Nodes (63): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+55 more)

### Community 13 - "DebuggerWindow"
Cohesion: 0.05
Nodes (53): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .canAcceptEdit, .errorText, .frameLabels, .hasUnacceptedChanges (+45 more)

### Community 14 - "BrowserModel"
Cohesion: 0.16
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 15 - "Parser"
Cohesion: 0.11
Nodes (22): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+14 more)

### Community 16 - "TEST"
Cohesion: 0.03
Nodes (72): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, EnvEnablesStress (+64 more)

### Community 17 - "Emitter"
Cohesion: 0.16
Nodes (10): int16_t, Op, size_t, uint16_t, uint8_t, Emitter, real_, size (+2 more)

### Community 18 - "AcceptTests"
Cohesion: 0.11
Nodes (14): value, spanMessage(), AcceptTests, Bool, Int32, NSMenu, NSSegmentedControl, NSTableView (+6 more)

### Community 19 - "abi.cpp"
Cohesion: 0.05
Nodes (101): atomic, AoSpan, end, message, start, ClassIdIsStableAndOneRowPerClass, ClassIdsAreDroppedWithTheirClassAndNeverReused, ClassIdsDoNotSurviveBootOrImageLoad (+93 more)

### Community 20 - "CHANGELOG"
Cohesion: 0.11
Nodes (33): ao_debug_restart / ao_debug_can_restart, AO_ERR_HALT, ao_request_interrupt, ao_set_runloop_pump_hook, Browser session class ID (ao_browser_class_id), CHANGELOG, Live debug mode (AO_DEBUG_LIVE), Post-mortem frame capture (ao_set_debug_capture / ao_debug_*) (+25 more)

### Community 21 - "Boot"
Cohesion: 0.04
Nodes (59): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, string (+51 more)

### Community 22 - "image_save_load_test.cpp"
Cohesion: 0.11
Nodes (35): chrono, Chunk, cmath, compile, CompiledMethod, compiler, context, cstring (+27 more)

### Community 23 - "applyMethod"
Cohesion: 0.09
Nodes (49): P15 — ライブ Debugger の編集, TDD, 前提, 範囲, 結論, File Structure, Global Constraints, P15 Debugger の編集 — 実装計画 (+41 more)

### Community 24 - "Heap"
Cohesion: 0.03
Nodes (70): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, AllocateNoGcSpillsToOld, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, ByteObjectPayload, EachImageSlotLists127Names, ExhaustionReturnsEmpty (+62 more)

### Community 25 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 26 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 27 - "TEST"
Cohesion: 0.04
Nodes (58): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+50 more)

### Community 28 - "TEST"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 29 - "MethodSource"
Cohesion: 0.15
Nodes (19): 手順（PR の順序と依存）, attachBlocks(), blockLiteral(), MethodImage, size_t, string, uint64_t, MethodSource (+11 more)

### Community 30 - "ChangeSorterTests"
Cohesion: 0.09
Nodes (15): ChangeSorterTests, Bool, String, URL, 受け入れ, 手動確認, P17 — Smalltalk のチェンジセット, TDD (+7 more)

### Community 31 - "Session.cpp"
Cohesion: 0.07
Nodes (40): climits, Image, check, load, save, boot(), shutdown(), assignClassIds() (+32 more)

### Community 32 - "ao_runtime_shutdown"
Cohesion: 0.05
Nodes (36): AbiSmoke, AoTranscriptFn, BootAndShutdownReturnZero, BootVersionShutdown, Qiita 後編: 協調スケジューラとファイバ切り替え, `Object>>halt`（P10-03）, P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）, SPEC を先に直す（P10-01） (+28 more)

### Community 33 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 34 - "Scheduler"
Cohesion: 0.04
Nodes (55): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+47 more)

### Community 35 - "cstdint"
Cohesion: 0.07
Nodes (25): algorithm, Bootstrap, cassert, cstddef, cstdint, gc, memory, MethodDictionary (+17 more)

### Community 36 - ".replaceText"
Cohesion: 0.13
Nodes (6): SmokeTests, NSTextView, NSView, String, WorkspaceEvalTests, XCTestCase

### Community 37 - "Heap.cpp"
Cohesion: 0.07
Nodes (50): charconv, allocateTenured, checkNotPoisoned, growOld, initObject, objectBytes, oldUsed, uint16_t (+42 more)

### Community 38 - "TEST"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 39 - "checkFile"
Cohesion: 0.17
Nodes (19): ifstream, checkFile(), byte, size_t, T, uint32_t, Image::check(), Parsed (+11 more)

### Community 40 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (44): cerrno, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot() (+36 more)

### Community 41 - "TEST_F"
Cohesion: 0.04
Nodes (75): AliasedOldClassRowActsOnItsOwnClass, BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:), BrowserWindow.removeAfterConfirming(...), BrowserWindow.removeMethod / removeMethod(atRow:), testRemoveMethodAfterConfirmUpdatesLists, ao_remove_class, ao_remove_method (+67 more)

### Community 42 - "reshapeClass"
Cohesion: 0.11
Nodes (35): ChunkMode, acceptMethodSource, FileInError, error, file, method, string, MethodDictionary::atPut (+27 more)

### Community 43 - "unwinding"
Cohesion: 0.07
Nodes (73): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 範囲 (+65 more)

### Community 44 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 45 - "ToolWindowTests"
Cohesion: 0.10
Nodes (12): LaunchSet, Bool, Int32, NSFont, NSMenu, NSMenuItem, NSTextView, NSView (+4 more)

### Community 46 - "allocateRetry"
Cohesion: 0.09
Nodes (47): Global Constraints, removeMethodOf, MethodDictionary::at, MethodDictionary::pairArray, MethodDictionary::removeKey, isPseudoVariableName, removeMethodOf (definition), CallContext (+39 more)

### Community 47 - "TEST_F"
Cohesion: 0.05
Nodes (44): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+36 more)

### Community 48 - "DebugFrames"
Cohesion: 0.06
Nodes (47): uint32_t, PcSpan, end, pc, start, P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context (+39 more)

### Community 49 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 50 - ".nil"
Cohesion: 0.09
Nodes (38): P12 追補: クラス ID と壊れたメソッド辞書 実装計画, Design rationale: session class ID table, Design rationale: MethodDictionary shape check (pairArray), allocateSkeletons(), allocClass(), Roots, string_view, WellKnown (+30 more)

### Community 51 - "P12 Browser 削除 実装計画"
Cohesion: 0.07
Nodes (39): P12 — Browser の削除 (phase doc), Addendum tests (browser_abi_test, accept_abi_test, session_abi_test, method_dictionary_test, BrowserModelTests), P12 Addendum: Class ID and Broken Method Dictionary (2026-09-27), Method dictionary removal (nil pair, decrement tally), MethodDictionary::pairArray (shape check), Class removal unbinds name only, BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove (+31 more)

### Community 52 - "DebuggerWindowTests"
Cohesion: 0.14
Nodes (10): DebuggerWindowTests, NSButton, String, Void, TranscriptBox, 受け入れ, 手動確認, 受け入れ (+2 more)

### Community 53 - "ImageFormat"
Cohesion: 0.09
Nodes (23): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle (+15 more)

### Community 54 - "DebugSnapshot"
Cohesion: 0.07
Nodes (27): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+19 more)

### Community 55 - "Session"
Cohesion: 0.07
Nodes (63): browserClassAt(), browserClassCount(), browserClassDefinition(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+55 more)

### Community 56 - "TEST_F"
Cohesion: 0.06
Nodes (40): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+32 more)

### Community 57 - "ChangeSorterWindow"
Cohesion: 0.07
Nodes (33): ChangeSorterWindow, .canMakeCurrent, .classTitles, .errorText, .methodTitles, .paneAccessibilityLabels, .setTitles, .sourceText (+25 more)

### Community 58 - "WellKnown.cpp"
Cohesion: 0.06
Nodes (43): findSymbol, global, internWith, isFixedGlobal, bytes(), string_view, WellKnown, intern() (+35 more)

### Community 59 - "TEST_F"
Cohesion: 0.04
Nodes (77): AcceptRecordsAddedThenChangedInANewSet, BootAndImageLoadStartOver, BrokenModelLeavesAcceptAndRemovalAnswers, ClassDefinitionAcceptIsChangedForAnExistingClass, ClassSideAcceptIsRecordedOnTheClassSide, DebuggerAcceptIsRecorded, DebuggerAcceptRefusesWhenTheRecordTerminatesTheProcess, Task 4: 項目を忘れる・移す（Remove from Change Set / Move to） (+69 more)

### Community 60 - "AoApp"
Cohesion: 0.08
Nodes (27): AoApp, fileInChangeFile(), fileInLoadOrder(), fileInToolsModel(), fileInVendor(), openImageFile(), saveImageFile(), Bool (+19 more)

### Community 61 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installArray(), WellKnown, installBoolean(), WellKnown, installCharacter(), WellKnown, installCollection() (+26 more)

### Community 62 - "ChangeSet.cpp"
Cohesion: 0.21
Nodes (39): ctime, Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）, boolean(), bytesText(), changesSession(), countOf(), CallContext, int64_t (+31 more)

### Community 63 - "Geometry.cpp"
Cohesion: 0.19
Nodes (33): allocateInstance(), CallContext, Roots, uint32_t, RootedArray::RootedArray(), ao_Point_add(), ao_Point_equals(), ao_Point_hash() (+25 more)

### Community 64 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (39): AnyObject, aoWorkspaceInspectHook(), failureText(), installDebugButton(), installErrorField(), keptEvalResult(), selectErrorSpan(), sendToKeyWorkspace() (+31 more)

### Community 65 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 66 - ".isSmallInteger"
Cohesion: 0.14
Nodes (35): int64_t, contextPc(), bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root (+27 more)

### Community 67 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 68 - "Array.cpp"
Cohesion: 0.14
Nodes (30): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), valueHashWord(), ao_Array_equals() (+22 more)

### Community 69 - "Scheduler.cpp"
Cohesion: 0.06
Nodes (93): flags, slotAt, slotAtPut, addFiber, afterResume, enqueue, find, findId (+85 more)

### Community 70 - "NativeMethod.cpp"
Cohesion: 0.22
Nodes (19): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+11 more)

### Community 71 - "TEST"
Cohesion: 0.08
Nodes (26): BlockWithArgs, Cascade, ParseResult, error, method, ok, parseMethod(), BlockArgumentsThenTemps (+18 more)

### Community 72 - "TEST"
Cohesion: 0.04
Nodes (51): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+43 more)

### Community 73 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 74 - "vector"
Cohesion: 0.05
Nodes (47): vector, cstdio, future, runtime, namedSlotNames(), superclassChainFromRoot(), DebugInfo, bodies (+39 more)

### Community 75 - ".debugAfterDoIt"
Cohesion: 0.13
Nodes (10): Int, NSFont, NSTableView, NSTextView, NSView, NSWindow, T, UInt (+2 more)

### Community 76 - "VendorExtract.cpp"
Cohesion: 0.06
Nodes (77): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+69 more)

### Community 77 - "ao_remove_class Rules"
Cohesion: 0.18
Nodes (12): ao_accept_class, Ao Bytecode Set, ao_remove_class, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation), Blue Book Conformance Definition, Class Definition Re-Accept, Fixed Globals (+4 more)

### Community 78 - "Scheduler::Record"
Cohesion: 0.07
Nodes (27): Scheduler, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, block, ctx, deadlockPending (+19 more)

### Community 79 - "TEST"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 80 - "BrowserModelTests"
Cohesion: 0.18
Nodes (10): BrowserModelTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+2 more)

### Community 81 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.07
Nodes (58): docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, SPEC.md, Codegen.cpp: Emitter::mark と compile* 群, MethodImage.hpp: PcSpan / TempName / pcMap / temps (+50 more)

### Community 82 - "DebugSnapshot.cpp"
Cohesion: 0.16
Nodes (26): frameAt, uint32_t, DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method(), DebugSnapshot::pc(), DebugSnapshot::process(), DebugSnapshot::receiver() (+18 more)

### Community 83 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 84 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (29): safepoint, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual() (+21 more)

### Community 85 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 86 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 87 - "Ast"
Cohesion: 0.11
Nodes (19): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+11 more)

### Community 88 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 89 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.10
Nodes (32): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+24 more)

### Community 90 - "VendorClassFile"
Cohesion: 0.25
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 91 - "MainMenu"
Cohesion: 0.15
Nodes (15): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+7 more)

### Community 92 - "fiber_test.cpp"
Cohesion: 0.15
Nodes (18): csignal, mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest (+10 more)

### Community 93 - "P6b — vendor file-in"
Cohesion: 0.19
Nodes (17): vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, Kernel NativeMethod 走査, P6b-01: Cuis pin と ORIGIN.md, Cuis vendor pin (ORIGIN.md), P6b-02: LOAD_ORDER と Kernel 上書き禁止, LOAD_ORDER と上書き禁止 (+9 more)

### Community 94 - "ChangeSorterModel"
Cohesion: 0.13
Nodes (29): ChangeClassRow, ChangeKind, added, changed, .mark, none, removed, ChangeMethodRow (+21 more)

### Community 95 - "Natives.hpp"
Cohesion: 0.09
Nodes (23): classpool, DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, fstream, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails (+15 more)

### Community 96 - "Literal"
Cohesion: 0.07
Nodes (33): int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal (+25 more)

### Community 97 - "CallContext"
Cohesion: 0.16
Nodes (23): ソース表の拡張（P10-04）, リスク, removeClassOf, isClassShaped(), superclassOf(), anyMethodIn(), CallContext, pair (+15 more)

### Community 98 - "FiberStack"
Cohesion: 0.11
Nodes (25): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+17 more)

### Community 99 - "Gc.cpp"
Cohesion: 0.15
Nodes (12): cstdlib, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::scavengeFromRoots(), optional, optional (+4 more)

### Community 100 - "TEST"
Cohesion: 0.05
Nodes (45): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+37 more)

### Community 101 - "Image::load"
Cohesion: 0.24
Nodes (11): readHeader, bindAll(), checkGlobals(), Roots, string_view, WellKnown, fileOop(), Image::load() (+3 more)

### Community 102 - "Compiler (.st to CompiledMethod)"
Cohesion: 0.20
Nodes (12): Host GUI (AppKit), Chunk Format, Closures and Shared Temps, CompiledMethod, Compiler (.st to CompiledMethod), Debugger (§3.13), Debugger Window, v1 Non-goals (+4 more)

### Community 103 - "LiveFrames"
Cohesion: 0.09
Nodes (19): CallContext, Frame, size_t, string, uint32_t, LiveFrames, context, frames_ (+11 more)

### Community 104 - "BrowserWindow"
Cohesion: 0.06
Nodes (34): BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText (+26 more)

### Community 105 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 106 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 107 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 108 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 109 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch() (+10 more)

### Community 110 - "TEST_F"
Cohesion: 0.09
Nodes (21): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+13 more)

### Community 111 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 112 - "string"
Cohesion: 0.07
Nodes (26): BlockMethodHasItsOwnMapInMethodCoordinates, string, string_view, MethodImage, Op, string, string_view, uint32_t (+18 more)

### Community 113 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 114 - "ao_accept_method"
Cohesion: 0.10
Nodes (21): BrowserModel.classID(named:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.removeClass / removeClass(atRow:), BrowserWindow.showDefinedClass(from:) (+13 more)

### Community 115 - "Interpreter.cpp"
Cohesion: 0.07
Nodes (62): フレーム連鎖（P10-03）, deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots (+54 more)

### Community 116 - "string"
Cohesion: 0.20
Nodes (26): Task 5: File Out, Task 2: ネイティブ `Behavior>>sourceCodeAt:` と `Class>>definition`, boundClassNamed(), byteText(), categoryHeading(), classDefinitionText(), classNameOf(), classVarList() (+18 more)

### Community 117 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 118 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 119 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 120 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 121 - "P4 — Kernelネイティブ実装"
Cohesion: 0.16
Nodes (18): P4 — Kernelネイティブ実装, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード (+10 more)

### Community 122 - "TEST"
Cohesion: 0.13
Nodes (18): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, describe(), evalBody() (+10 more)

### Community 123 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 124 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 125 - "P11 — ライブデバッガ"
Cohesion: 0.29
Nodes (7): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 範囲, 結論

### Community 126 - "ClassPool.cpp"
Cohesion: 0.20
Nodes (24): hashedcollection, adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view (+16 more)

### Community 127 - "TEST"
Cohesion: 0.11
Nodes (18): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, FiledOutCommentChunksAreSkipped (+10 more)

### Community 128 - "Ao implementation docs index (phases and PRs)"
Cohesion: 0.14
Nodes (26): P0 (親フェーズ, stub), ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P0-01: git / LICENSE / PHASE / README, PHASE file marker (+18 more)

### Community 129 - "P8 — AppKit ツール"
Cohesion: 0.12
Nodes (23): P8 — AppKit ツール, P9 — 統合, P8-01: Ao.app 骨格, Ao.app 骨格, P8-02: Transcript ウィンドウ, Transcript ウィンドウ, P8-03: Workspace ウィンドウ, Workspace ウィンドウ (+15 more)

### Community 130 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.19
Nodes (13): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+5 more)

### Community 132 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 133 - "clearUnwinding"
Cohesion: 0.07
Nodes (40): Change Sorter（Swift）, P17 — Smalltalk のチェンジセット（設計）, Smalltalk 側のクラス, SPEC の変更点, やらないこと, クラス側・インスタンス側の入口, テスト, ネイティブ（Kernel、C++） (+32 more)

### Community 134 - "AppKit"
Cohesion: 0.08
Nodes (23): Ao, aoRunLoopPumpHook(), EvaluationActivity, .isActive, RunLoopPump, Bool, UnsafeMutableRawPointer, BusyBody (+15 more)

### Community 135 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 136 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 137 - "ClassId"
Cohesion: 0.50
Nodes (4): ClassId, cls, id, int64_t

### Community 139 - "FiberRegs"
Cohesion: 0.14
Nodes (14): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+6 more)

### Community 140 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 141 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 142 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 143 - "InspectorWindow"
Cohesion: 0.22
Nodes (8): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String

### Community 144 - "CompileEnv"
Cohesion: 0.07
Nodes (29): BlockAssignmentIsBindingStore, Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings (+21 more)

### Community 145 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 146 - "Release / Semantic Versioning"
Cohesion: 0.31
Nodes (9): package-app.sh script, Acceptance Criteria (§6), CI (GitHub Actions), Info.plist CFBundleShortVersionString/CFBundleVersion from ao --version (1.0.0 for v1, 1.1.0 through P12), Kernel Scan Test (KernelScan.MethodDictionaryValuesAreNativeMethods), P12 Browser Removal Acceptance, GitHub Release assets (Ao-<ver>-macos-arm64.zip, ao-cli-<ver>-macos-arm64.tar.gz, SHA256SUMS), Release / Semantic Versioning (+1 more)

### Community 147 - "String"
Cohesion: 0.25
Nodes (8): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Unicode

### Community 148 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 149 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 150 - ".aoimage Image Format"
Cohesion: 0.33
Nodes (6): .aoimage Image Format, ensureKernelNatives, 16-bit Identity Hash, NativeMethod, Performance Policy (native hot paths), 64-bit Tagged OOP

### Community 151 - "makeToolTextWindow"
Cohesion: 0.09
Nodes (21): Void, aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int, Int32 (+13 more)

### Community 152 - "Ao README"
Cohesion: 0.10
Nodes (26): ao_remove_method / ao_remove_class, Known limitations (no JIT/FFI, unoptimized interpreter, uninterruptible natives), CONTRIBUTING, CONTRIBUTING (Japanese), Release procedure (SPEC §2.4), B2 to:do: inlining bench, Benchmark notes (docs/bench.md), KernelBench.InlinedToDoMillion / TenMillionToDo (+18 more)

### Community 153 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 155 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 156 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 158 - "§3.9 Removal (Browser deletion rules)"
Cohesion: 0.24
Nodes (10): ao_accept_method_id, ao_remove_method, Broken Superclass Chain Rules, Browser Remove UI (context menu, confirmation sheet), Busy / Reentrancy Rules, Message Send Semantics, Method Cache and Invalidation, ao_remove_method Rules (+2 more)

### Community 159 - ".isHeap"
Cohesion: 0.09
Nodes (35): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, MalformedDictionaryAnswersNilAndTakesNothing, NinthMethodWithFullNurseryIsInstalled, RemoveKeyLeavesTheOthersFindableAndReusesThePair (+27 more)

### Community 160 - "synthesizesNative"
Cohesion: 0.46
Nodes (8): CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget(), synthesizesNative()

### Community 161 - "ImageLoad.cpp"
Cohesion: 0.20
Nodes (28): acceptWord(), atOffset(), string, uint64_t, unordered_map, unordered_set, fail(), findRecord() (+20 more)

### Community 162 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 164 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 165 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 167 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 168 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 169 - "Vendor Class Library (import, do not write)"
Cohesion: 0.18
Nodes (11): CLAUDE.md, Bootstrap Procedure, Change Rules (§8), Constraints (§5), Cuis Smalltalk, Graphify and Serena Requirement, Kernel Classes (native-only), v1 Completion Definition (+3 more)

### Community 173 - "C ABI (bridge/ao_abi.h)"
Cohesion: 0.29
Nodes (7): ao_eval, Bridge (runtime <-> app), C ABI (bridge/ao_abi.h), Debugger Readout ABI (ao_debug_*), Post-v1 phases bump the minor version (1.1.0), P12 Browser Removal Phase, Implementation Phases P0-P12

### Community 174 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 175 - "Gc"
Cohesion: 0.14
Nodes (22): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, collectNursery, collectOld, copy, heap_ (+14 more)

### Community 176 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 177 - "send2"
Cohesion: 0.11
Nodes (19): ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RebindsBagAndEvaluatesInstVar, RectangleWidthHeightContainsAndIntersect (+11 more)

### Community 178 - "Compile.cpp"
Cohesion: 0.11
Nodes (34): cctype, boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), CarriedMethod, image, meta (+26 more)

### Community 179 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 181 - "Class ID (session class-ID table)"
Cohesion: 0.18
Nodes (12): Failure Aborts Evaluation, allocateRetry, ao_image_load, AoSpan.message (error reason), Base Process, Class Aliases (other global names for a class), Cooperative Process Scheduler (fibers), Full GC Triggers (+4 more)

### Community 182 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 183 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 184 - "Debugger Accept (reactivate frame with new method)"
Cohesion: 0.25
Nodes (8): ao_debug_accept, ao_debug_can_accept, ao_debug_restart, Debugger Accept (reactivate frame with new method), Debugger Restart (frame from start), Halt reason `accepted`, P14 Restart Phase, P15 Debugger Edit Phase

### Community 185 - "TEST_F"
Cohesion: 0.15
Nodes (12): ClassDefinitionAnswersTheBrowserText, DefinitionIsNotUnderstoodByAMetaclass, acceptClass(), acceptMethod(), string, testing::Test, literal(), printIt() (+4 more)

### Community 187 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 188 - "b5Definition"
Cohesion: 0.20
Nodes (9): BrowserModel.copyClass(at:), ao_accept_class, ao_browser_class_at, b5Definition(), ClassDefinitionThenImageDropsSourceText, ReacceptWithNewInstanceVariableRecompilesMethods, ShapeChangePutsNameBackWhenSubclassSendFails, ShapeChangeRefusesClassWithSubclasses (+1 more)

### Community 190 - "native_send_test.cpp"
Cohesion: 0.54
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 193 - "P3 — ネイティブディスパッチ"
Cohesion: 0.16
Nodes (15): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary (+7 more)

### Community 195 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 197 - "Codegen.cpp"
Cohesion: 0.11
Nodes (25): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Inline, LitKind (+17 more)

### Community 198 - "ao_main.cpp"
Cohesion: 0.22
Nodes (18): Task 0: ブランチ, Task 3: Smalltalk のモデル（記録・種類・理由・Remove / Move）と Tools-model の file-in, dyld, addRoots, bootAndRunTests(), collectLoadOrders(), CallContext, string (+10 more)

### Community 199 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 200 - "NativeMethod.hpp"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 202 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.40
Nodes (5): PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 203 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 206 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 207 - "Release 1.0.0 (v1, P0-P9)"
Cohesion: 0.29
Nodes (7): .aoimage save/load with native rebinding, Pinned Cuis-Smalltalk excerpt (image/vendor), Exact generational GC (nursery + mark-compact old), Kernel classes as C++ NativeMethods, Release 1.0.0 (v1, P0-P9), 64-bit tagged ao::Oop, Rules that do not bend

### Community 208 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 209 - "Graphify Required Tool"
Cohesion: 0.67
Nodes (3): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps)

### Community 211 - "ao CMake Project"
Cohesion: 0.67
Nodes (4): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target)

### Community 213 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneSixZero

### Community 216 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 217 - "Debug it (AO_EVAL_DEBUGIT)"
Cohesion: 0.47
Nodes (5): Debug it (AO_EVAL_DEBUGIT), MainMenu.swift: Debug it ⌘⇧D, Evaluation Process (live mode), Live Debugger (P11), Live debugger Step into / over / out (depth-based step flag)

### Community 219 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 220 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 222 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 223 - "test (macOS arm64) Job"
Cohesion: 0.67
Nodes (3): Kernel Scan Test (via ctest), test (macOS arm64) Job, CI Workflow

### Community 225 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 247 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 248 - "TDD Test Suites"
Cohesion: 0.50
Nodes (4): AcceptTests Remove/ContextMenu XCTests, Golden Evaluation (ao --test), remove_abi_test, TDD Test Suites

### Community 355 - "CompileError"
Cohesion: 0.15
Nodes (32): CompileError, message, span, string, File Structure, P16 チェンジセットと Change Sorter — 実装計画, Task 0: ブランチ, Task 1: SPEC を先に直す（P16 の追記） (+24 more)

## Ambiguous Edges - Review These
- `P5-01: 字句解析` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P8-01: Ao.app 骨格` → `P7-03: load と NativeMethod 再結合`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `P8-05: メニューとキー` → `P9-01: Do it / Print it / Inspect it`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references
- `Debug it (AO_EVAL_DEBUGIT)` → `SPEC §3.13 デバッガ（捕捉の意味論）`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references

## Knowledge Gaps
- **1057 isolated node(s):** `.selectedClass`, `.metaFlag`, `.canRemoveMethod`, `.canRemoveClass`, `.hasUnacceptedChanges` (+1052 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2818 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **117 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `P5-01: 字句解析` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P8-01: Ao.app 骨格` and `P7-03: load と NativeMethod 再結合`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P8-05: メニューとキー` and `P9-01: Do it / Print it / Inspect it`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Debug it (AO_EVAL_DEBUGIT)` and `SPEC §3.13 デバッガ（捕捉の意味論）`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
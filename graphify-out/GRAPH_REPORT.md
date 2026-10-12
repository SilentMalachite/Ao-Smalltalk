# Graph Report - ao-smalltalk  (2026-10-12)

## Corpus Check
- 318 files · ~422,784 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 83 file(s) not represented in the graph (top: .st 67, (none) 13, .toml 1)

## Summary
- 6808 nodes · 18670 edges · 346 communities (228 shown, 118 thin omitted)
- Extraction: 84% EXTRACTED · 16% INFERRED · 0% AMBIGUOUS · INFERRED: 2942 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `23f08b6f`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- TEST_F
- WellKnown
- TEST_F
- BlockContext.cpp
- Dictionary.cpp
- TEST
- String.cpp
- TEST
- CallContext
- RealScope
- LargeInteger.cpp
- Object.cpp
- Stream.cpp
- DebuggerWindow
- BrowserModel
- Parser
- TEST
- Emitter
- AcceptTests
- abi.cpp
- P13 Evaluation Interrupt design
- Boot
- string
- resumeHalted
- Heap
- Scanner.cpp
- Frame
- TEST
- TEST
- MethodSource
- AoSpan
- Session.cpp
- ao_runtime_boot
- TEST
- Scheduler
- cstdint
- .replaceText
- Heap.cpp
- TEST
- ImageLoad.cpp
- ImageSave.cpp
- TEST_F
- .isHeap
- Send.cpp
- Float.cpp
- ToolWindowTests
- Behavior.cpp
- TEST_F
- DebugFrames
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- .fromSmallInteger
- P12 Browser 削除 実装計画
- DebuggerWindowTests
- ImageFormat
- DebugSnapshot
- writeBuf
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
- HashedCollection.cpp
- TEST
- Array.cpp
- .nil
- NativeMethod.cpp
- TEST
- slotAt
- Oop
- vector
- .assertWholeText
- VendorExtract.cpp
- §3.9 Removal (Browser deletion rules)
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
- Chunk
- MainMenu
- PingPong
- TEST
- ChangeSorterModel
- FileInError
- Literal
- liveClasses
- FiberStack
- Gc.cpp
- TEST
- checkFile
- Compiler (.st to CompiledMethod)
- LiveFrames
- BrowserWindow
- Process.cpp
- P5 — コンパイラ
- TranscriptWindow
- TEST
- Fiber.cpp
- TEST_F
- TEST
- pc_map_test.cpp
- Roots.cpp
- ao_accept_method
- OperandStack
- string
- TEST
- TEST
- TEST
- ClassDef
- P4 — Kernelネイティブ実装
- unwinding
- TEST
- native_method_test.cpp
- P11 — ライブデバッガ
- ClassPool.cpp
- TEST
- P0 (親フェーズ, stub)
- Ao implementation docs index (phases and PRs)
- DiskHeader
- VirtualRegion.cpp
- CountingSink
- refreshStackLimit
- AppKit
- ChunkAction
- ChunkParser.cpp
- Session
- CAo
- FiberRegs
- TEST
- TEST
- TEST
- InspectorWindow
- Analysis
- Stack
- Release / Semantic Versioning
- DefinitionScanner
- P2 — ブートストラップ
- .isTrue
- ChangeMethodRow
- UniformFont
- Ao README
- GarbageFirstBoot
- session_abi_test.cpp
- ClassMethodCache
- TEST
- Task 2: `ao_debug_accept` — 入れて、起動し直して、`accepted` で止める
- Interpreter::run
- TEST
- synthesizesNative
- uint64_t
- FrameBlock
- Interpreter.cpp
- CallContext
- ImageSurgery
- TEST
- Token
- TEST
- Vendor Class Library (import, do not write)
- ImageFormat.cpp
- P1 — オブジェクトメモリ
- Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）
- clearUnwinding
- FileSizeLimit
- Gc
- TEST
- send2
- CompileError
- test.sh
- Parsed
- .aoimage Image Format
- Loaded
- abortingSubclass
- Debugger Accept (reactivate frame with new method)
- TEST_F
- collectNursery
- Counts
- Character.cpp
- P10 — 事後デバッガ
- native_send_test.cpp
- StackPool
- parser
- P3 — ネイティブディスパッチ
- .specialSelector
- path
- Frame
- TEST
- ao_main.cpp
- Rec
- NativeMethod.hpp
- WellKnown::InternTable
- P8–P9 Remaining Implementation Plan
- popFrame
- ObjectHeader
- SelectedFrames
- TestDir
- intern
- TEST
- Graphify Required Tool
- P9-04 v1 Golden Acceptance
- ao CMake Project
- interpreter
- createBlock
- runtime_src_fiber
- string_view
- Roots::visitAll
- Debug it (AO_EVAL_DEBUGIT)
- NativeFrame
- Root
- imageRegistryStubA
- ScopedGcStressEnv
- answerOne
- test (macOS arm64) Job
- lookup_test.cpp
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
- acceptMethodInto (definition)
- BrowserModel.applyHierarchyList(_:selecting:)
- BrowserModel.hierarchy(of:meta:)
- acceptClass
- .turnRunLoop
- HashNesting
- SlotNames
- RootedArray::RootedArray
- ImageSelector
- NamedClass
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
- 前提（コードで確かめた事実。`b35efe8`）
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

## Communities (346 total, 118 thin omitted)

### Community 0 - "TEST_F"
Cohesion: 0.02
Nodes (148): AbortAfterAcceptRunsOnlyOuterEnsure, AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, AcceptAfterTerminateElsewhereIsRefused, AcceptClassSideMethod, AcceptCompileErrorChangesNothing, AcceptDoItFrameIsRefused, AcceptFailureKeepsResultSnapshotAndInterrupt (+140 more)

### Community 1 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 2 - "TEST_F"
Cohesion: 0.03
Nodes (82): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+74 more)

### Community 3 - "BlockContext.cpp"
Cohesion: 0.24
Nodes (22): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+14 more)

### Community 4 - "Dictionary.cpp"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 5 - "TEST"
Cohesion: 0.02
Nodes (82): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+74 more)

### Community 6 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 7 - "TEST"
Cohesion: 0.03
Nodes (81): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+73 more)

### Community 8 - "CallContext"
Cohesion: 0.04
Nodes (49): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, acceptSlots (+41 more)

### Community 9 - "RealScope"
Cohesion: 0.08
Nodes (27): Capture, owner, var, uint32_t, RealScope, copied, copiedBase, isBlock (+19 more)

### Community 10 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 11 - "Object.cpp"
Cohesion: 0.11
Nodes (53): Graphify / Serena, boolean(), ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_() (+45 more)

### Community 12 - "Stream.cpp"
Cohesion: 0.10
Nodes (67): allocateInstance(), allocateRetry(), CallContext, uint16_t, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position() (+59 more)

### Community 13 - "DebuggerWindow"
Cohesion: 0.05
Nodes (52): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .canAcceptEdit, .errorText, .frameLabels, .hasUnacceptedChanges (+44 more)

### Community 14 - "BrowserModel"
Cohesion: 0.16
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 15 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 16 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 17 - "Emitter"
Cohesion: 0.14
Nodes (16): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+8 more)

### Community 18 - "AcceptTests"
Cohesion: 0.12
Nodes (11): spanMessage(), AcceptTests, Int32, NSMenu, NSSegmentedControl, NSTableView, NSTextView, NSView (+3 more)

### Community 19 - "abi.cpp"
Cohesion: 0.06
Nodes (70): atomic, ClassIdIsStableAndOneRowPerClass, ClassIdsAreDroppedWithTheirClassAndNeverReused, ClassIdsDoNotSurviveBootOrImageLoad, CountsAnswerMinusOneOnFailure, ABI（P10-05）, ObjectIsKernelAndPrintStringIsNative, AbiEntry (+62 more)

### Community 20 - "P13 Evaluation Interrupt design"
Cohesion: 0.15
Nodes (25): ao_debug_restart / ao_debug_can_restart, AO_ERR_HALT, ao_request_interrupt, ao_set_runloop_pump_hook, Live debug mode (AO_DEBUG_LIVE), P13 Evaluation Interrupt (phase doc), P14 Live Debugger Restart (phase doc), P13 Evaluation Interrupt implementation plan (+17 more)

### Community 21 - "Boot"
Cohesion: 0.05
Nodes (63): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+55 more)

### Community 22 - "string"
Cohesion: 0.10
Nodes (40): algorithm, Bootstrap, chrono, classpool, cmath, compile, CompiledMethod, compiler (+32 more)

### Community 23 - "resumeHalted"
Cohesion: 0.10
Nodes (39): Task 1: Accept の対象判定と `ao_debug_can_accept`, boxMethodImage, acceptTarget(), answerAwaited(), answerEval(), blankOut(), clearInterruptRequest(), AoInspectFn (+31 more)

### Community 24 - "Heap"
Cohesion: 0.04
Nodes (57): Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_, fromStart_ (+49 more)

### Community 25 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 26 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 27 - "TEST"
Cohesion: 0.04
Nodes (53): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+45 more)

### Community 28 - "TEST"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 29 - "MethodSource"
Cohesion: 0.10
Nodes (26): 手順（PR の順序と依存）, attachBlocks(), blockLiteral(), MethodImage, size_t, DebugInfoRef, body, index (+18 more)

### Community 30 - "AoSpan"
Cohesion: 0.09
Nodes (53): BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:), BrowserWindow.removeAfterConfirming(...), BrowserWindow.removeMethod / removeMethod(atRow:), BrowserWindow.submit(_:method:), testRemoveMethodAfterConfirmUpdatesLists, ao_accept_method_id, ao_remove_class (+45 more)

### Community 31 - "Session.cpp"
Cohesion: 0.08
Nodes (33): Image, check, load, save, boot(), shutdown(), bumpDebugGeneration(), clearMethodSources() (+25 more)

### Community 32 - "ao_runtime_boot"
Cohesion: 0.08
Nodes (25): AbiSmoke, AoTranscriptFn, BootAndShutdownReturnZero, BootVersionShutdown, ao_runtime_boot(), ao_runtime_shutdown(), ao_set_debug_capture(), ao_set_debug_mode() (+17 more)

### Community 33 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 34 - "Scheduler"
Cohesion: 0.03
Nodes (74): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+66 more)

### Community 35 - "cstdint"
Cohesion: 0.07
Nodes (15): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, DebugSink (+7 more)

### Community 36 - ".replaceText"
Cohesion: 0.16
Nodes (4): NSTextView, NSView, String, WorkspaceEvalTests

### Community 37 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 38 - "TEST"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 39 - "ImageLoad.cpp"
Cohesion: 0.21
Nodes (17): ifstream, bindAll(), checkGlobals(), byte, T, fileOop(), globalNamesOk(), ImageRecord (+9 more)

### Community 40 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (45): cerrno, cstdlib, fcntl, appendRaw(), appendRecord(), collectImageSlot(), byte, Roots (+37 more)

### Community 41 - "TEST_F"
Cohesion: 0.04
Nodes (59): AliasedOldClassRowActsOnItsOwnClass, ClassIdTableDoesNotBlockSuperclassRemoval, KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, MalformedMethodDictionaryFallsBackOnSend, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad, RemoveAcceptedMethodOnKernelClass (+51 more)

### Community 42 - ".isHeap"
Cohesion: 0.10
Nodes (48): cctype, MethodDictionary::create, string_view, isVendorStub(), acceptClassDef(), addError(), anyMethodIn(), applyClassDef() (+40 more)

### Community 43 - "Send.cpp"
Cohesion: 0.12
Nodes (39): Graphify / Serena, フレーム連鎖（P10-03）, 捕捉（P10-03、配線は P10-04）, run, InlineCache, cachedClass, cachedMethod, applyMethod() (+31 more)

### Community 44 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 45 - "ToolWindowTests"
Cohesion: 0.10
Nodes (12): LaunchSet, Bool, Int32, NSFont, NSMenu, NSMenuItem, NSTextView, NSView (+4 more)

### Community 46 - "Behavior.cpp"
Cohesion: 0.16
Nodes (33): Global Constraints, MethodDictionary::at, MethodDictionary::pairArray, ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize() (+25 more)

### Community 47 - "TEST_F"
Cohesion: 0.05
Nodes (43): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+35 more)

### Community 48 - "DebugFrames"
Cohesion: 0.08
Nodes (42): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context, count, empty, kind, method, pc (+34 more)

### Community 49 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 50 - ".fromSmallInteger"
Cohesion: 0.09
Nodes (43): BrowserModel.copyClass(at:), ao_browser_class_at, Qiita 後編: 協調スケジューラとファイバ切り替え, P12 追補: クラス ID と壊れたメソッド辞書 実装計画, Design rationale: session class ID table, Design rationale: MethodDictionary shape check (pairArray), int64_t, CallContext (+35 more)

### Community 51 - "P12 Browser 削除 実装計画"
Cohesion: 0.07
Nodes (39): P12 — Browser の削除 (phase doc), Addendum tests (browser_abi_test, accept_abi_test, session_abi_test, method_dictionary_test, BrowserModelTests), P12 Addendum: Class ID and Broken Method Dictionary (2026-09-27), Method dictionary removal (nil pair, decrement tally), MethodDictionary::pairArray (shape check), Class removal unbinds name only, BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove (+31 more)

### Community 52 - "DebuggerWindowTests"
Cohesion: 0.10
Nodes (18): DebuggerWindowTests, Int, NSButton, NSTableView, NSView, NSWindow, String, T (+10 more)

### Community 53 - "ImageFormat"
Cohesion: 0.08
Nodes (25): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, encodeNonHeap, kImageEndianLittle (+17 more)

### Community 54 - "DebugSnapshot"
Cohesion: 0.07
Nodes (27): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+19 more)

### Community 55 - "writeBuf"
Cohesion: 0.14
Nodes (36): assignClassIds(), browserClassAt(), browserClassDefinition(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+28 more)

### Community 56 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 57 - "ChangeSorterWindow"
Cohesion: 0.07
Nodes (29): ChangeSorterWindow, .canMakeCurrent, .classTitles, .errorText, .methodTitles, .paneAccessibilityLabels, .setTitles, .sourceText (+21 more)

### Community 58 - "WellKnown.cpp"
Cohesion: 0.12
Nodes (26): findSymbol, global, internWith, isFixedGlobal, isPseudoVariableName, Roots, string_view, WellKnown::addRoots() (+18 more)

### Community 59 - "TEST_F"
Cohesion: 0.04
Nodes (77): AcceptRecordsAddedThenChangedInANewSet, BootAndImageLoadStartOver, BrokenModelLeavesAcceptAndRemovalAnswers, ClassDefinitionAcceptIsChangedForAnExistingClass, ClassSideAcceptIsRecordedOnTheClassSide, DebuggerAcceptIsRecorded, DebuggerAcceptRefusesWhenTheRecordTerminatesTheProcess, Task 4: 項目を忘れる・移す（Remove from Change Set / Move to） (+69 more)

### Community 60 - "AoApp"
Cohesion: 0.08
Nodes (28): AoApp, fileInChangeFile(), fileInLoadOrder(), fileInToolsModel(), fileInVendor(), openImageFile(), saveImageFile(), Bool (+20 more)

### Community 61 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installArray(), WellKnown, installBoolean(), WellKnown, installCharacter(), WellKnown, installCollection() (+26 more)

### Community 62 - "ChangeSet.cpp"
Cohesion: 0.29
Nodes (27): ctime, bytesText(), changesSession(), countOf(), int64_t, Root, size_t, elementAt() (+19 more)

### Community 63 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 64 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (39): AnyObject, aoWorkspaceInspectHook(), failureText(), installDebugButton(), installErrorField(), keptEvalResult(), selectErrorSpan(), sendToKeyWorkspace() (+31 more)

### Community 65 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 66 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 67 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 68 - "Array.cpp"
Cohesion: 0.14
Nodes (30): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), valueHashWord(), ao_Array_equals() (+22 more)

### Community 69 - ".nil"
Cohesion: 0.08
Nodes (69): flags, slotAtPut, addFiber, afterResume, enqueue, find, leaveLists, liveFibers (+61 more)

### Community 70 - "NativeMethod.cpp"
Cohesion: 0.32
Nodes (15): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+7 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 72 - "slotAt"
Cohesion: 0.04
Nodes (74): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+66 more)

### Community 73 - "Oop"
Cohesion: 0.13
Nodes (40): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+32 more)

### Community 74 - "vector"
Cohesion: 0.07
Nodes (33): vector, csignal, mach, mach_vm, resource, namedSlotNames(), superclassChainFromRoot(), DebugInfo (+25 more)

### Community 75 - ".assertWholeText"
Cohesion: 0.50
Nodes (3): NSFont, NSTextView, UInt

### Community 76 - "VendorExtract.cpp"
Cohesion: 0.06
Nodes (77): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+69 more)

### Community 77 - "§3.9 Removal (Browser deletion rules)"
Cohesion: 0.08
Nodes (34): ao_accept_class, ao_accept_method_id, Ao Bytecode Set, ao_eval, ao_remove_class, ao_remove_method, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation) (+26 more)

### Community 78 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, string, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+20 more)

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
Cohesion: 0.24
Nodes (28): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+20 more)

### Community 85 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 86 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 87 - "Ast"
Cohesion: 0.08
Nodes (34): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+26 more)

### Community 88 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 89 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.10
Nodes (32): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+24 more)

### Community 90 - "Chunk"
Cohesion: 0.14
Nodes (12): Chunk, MethodImage, string, VendorClassFile, chunkText, className, superName, unsupportedShape (+4 more)

### Community 91 - "MainMenu"
Cohesion: 0.15
Nodes (16): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+8 more)

### Community 92 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 93 - "TEST"
Cohesion: 0.05
Nodes (45): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+37 more)

### Community 94 - "ChangeSorterModel"
Cohesion: 0.11
Nodes (29): ChangeClassRow, ChangeKind, added, changed, .mark, none, removed, ChangeSetRow (+21 more)

### Community 95 - "FileInError"
Cohesion: 0.08
Nodes (24): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, FileInError, error (+16 more)

### Community 96 - "Literal"
Cohesion: 0.06
Nodes (38): int16_t, int64_t, LitKind, string, uint16_t, uint32_t, uint8_t, unique_ptr (+30 more)

### Community 97 - "liveClasses"
Cohesion: 0.12
Nodes (19): ao_accept_class, ソース表の拡張（P10-04）, リスク, removeClassOf, superclassOf(), hasSubclass(), liveClasses(), probeSubclasses() (+11 more)

### Community 98 - "FiberStack"
Cohesion: 0.13
Nodes (20): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+12 more)

### Community 99 - "Gc.cpp"
Cohesion: 0.24
Nodes (9): Roots, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::Gc(), Gc::scavengeFromRoots() (+1 more)

### Community 100 - "TEST"
Cohesion: 0.05
Nodes (45): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+37 more)

### Community 101 - "checkFile"
Cohesion: 0.29
Nodes (17): readHeader, atOffset(), checkFile(), Roots, size_t, string, string_view, WellKnown (+9 more)

### Community 102 - "Compiler (.st to CompiledMethod)"
Cohesion: 0.22
Nodes (11): Host GUI (AppKit), Chunk Format, Closures and Shared Temps, CompiledMethod, Compiler (.st to CompiledMethod), Debugger (§3.13), Debugger Window, v1 Non-goals (+3 more)

### Community 103 - "LiveFrames"
Cohesion: 0.09
Nodes (19): CallContext, Frame, size_t, string, uint32_t, LiveFrames, context, frames_ (+11 more)

### Community 104 - "BrowserWindow"
Cohesion: 0.06
Nodes (37): BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText (+29 more)

### Community 105 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 106 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 107 - "TranscriptWindow"
Cohesion: 0.11
Nodes (19): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int32, NSRect (+11 more)

### Community 108 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 109 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 110 - "TEST_F"
Cohesion: 0.05
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

### Community 114 - "ao_accept_method"
Cohesion: 0.11
Nodes (19): BrowserModel.classID(named:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.removeClass / removeClass(atRow:), BrowserWindow.showDefinedClass(from:) (+11 more)

### Community 115 - "OperandStack"
Cohesion: 0.13
Nodes (16): deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots, Temps (+8 more)

### Community 116 - "string"
Cohesion: 0.26
Nodes (18): Task 5: File Out, Task 2: ネイティブ `Behavior>>sourceCodeAt:` と `Class>>definition`, boundClassNamed(), byteText(), categoryHeading(), classDefinitionText(), classNameOf(), classVarList() (+10 more)

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

### Community 122 - "unwinding"
Cohesion: 0.22
Nodes (27): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+19 more)

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

### Community 128 - "P0 (親フェーズ, stub)"
Cohesion: 0.26
Nodes (12): P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+4 more)

### Community 129 - "Ao implementation docs index (phases and PRs)"
Cohesion: 0.10
Nodes (37): vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, P8 — AppKit ツール, P9 — 統合, Kernel NativeMethod 走査, P6b-01: Cuis pin と ORIGIN.md, Cuis vendor pin (ORIGIN.md) (+29 more)

### Community 130 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 132 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 133 - "refreshStackLimit"
Cohesion: 0.11
Nodes (20): Change Sorter（Swift）, P17 — Smalltalk のチェンジセット（設計）, Smalltalk 側のクラス, SPEC の変更点, やらないこと, クラス側・インスタンス側の入口, テスト, ネイティブ（Kernel、C++） (+12 more)

### Community 134 - "AppKit"
Cohesion: 0.12
Nodes (16): Ao, busyBodyHook(), CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, interruptOncePump(), CChar (+8 more)

### Community 135 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 136 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 137 - "Session"
Cohesion: 0.06
Nodes (37): beginDebugFreeEntry(), browserClassCount(), ClassId, cls, id, classIdRootSlots(), debugCanProceed(), DebugEntry (+29 more)

### Community 138 - "CAo"
Cohesion: 0.15
Nodes (8): aoRunLoopPumpHook(), EvaluationActivity, .isActive, RunLoopPump, Bool, T, UnsafeMutableRawPointer, CAo

### Community 139 - "FiberRegs"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

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
Cohesion: 0.11
Nodes (17): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String (+9 more)

### Community 144 - "Analysis"
Cohesion: 0.06
Nodes (30): Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, CompileResult (+22 more)

### Community 145 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 146 - "Release / Semantic Versioning"
Cohesion: 0.19
Nodes (13): version_string(), package-app.sh script, Acceptance Criteria (§6), AcceptTests Remove/ContextMenu XCTests, CI (GitHub Actions), Golden Evaluation (ao --test), Info.plist CFBundleShortVersionString/CFBundleVersion from ao --version (1.0.0 for v1, 1.1.0 through P12), GitHub Release assets (Ao-<ver>-macos-arm64.zip, ao-cli-<ver>-macos-arm64.tar.gz, SHA256SUMS) (+5 more)

### Community 147 - "DefinitionScanner"
Cohesion: 0.29
Nodes (7): DefinitionScanner, Bool, Int32, Token, keyword, other, Unicode

### Community 148 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 149 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 150 - "ChangeMethodRow"
Cohesion: 0.17
Nodes (12): ChangeMethodRow, .label, Bool, EntryTarget, Int32, Int64, MainActor, NSMenu (+4 more)

### Community 151 - "UniformFont"
Cohesion: 0.15
Nodes (10): Void, Int, NSFont, NSRange, ToolTextSize, .offset, UniformFont, NSTextStorage (+2 more)

### Community 152 - "Ao README"
Cohesion: 0.07
Nodes (41): ao_remove_method / ao_remove_class, .aoimage save/load with native rebinding, Browser session class ID (ao_browser_class_id), CHANGELOG, Pinned Cuis-Smalltalk excerpt (image/vendor), Exact generational GC (nursery + mark-compact old), Known limitations (no JIT/FFI, unoptimized interpreter, uninterruptible natives), Kernel classes as C++ NativeMethods (+33 more)

### Community 153 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 154 - "session_abi_test.cpp"
Cohesion: 0.10
Nodes (19): future, iterator, BusyRead, copies, evalCodes, lengths, string, expectAllRefused() (+11 more)

### Community 155 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 156 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 157 - "Task 2: `ao_debug_accept` — 入れて、起動し直して、`accepted` で止める"
Cohesion: 0.20
Nodes (16): P15 — ライブ Debugger の編集, TDD, 前提, 範囲, 結論, File Structure, Global Constraints, P15 Debugger の編集 — 実装計画 (+8 more)

### Community 158 - "Interpreter::run"
Cohesion: 0.23
Nodes (16): byteCount(), int16_t, int64_t, uint32_t, uint8_t, decodeHeader(), decodeMethodHeader(), instSlot() (+8 more)

### Community 159 - "TEST"
Cohesion: 0.18
Nodes (11): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, MalformedDictionaryAnswersNilAndTakesNothing, NinthMethodWithFullNurseryIsInstalled, RemoveKeyLeavesTheOthersFindableAndReusesThePair (+3 more)

### Community 160 - "synthesizesNative"
Cohesion: 0.44
Nodes (9): CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget(), sentToSuper() (+1 more)

### Community 161 - "uint64_t"
Cohesion: 0.25
Nodes (14): uint32_t, uint64_t, unordered_map, headerAt(), heapShaped(), ObjectRules, behavior_, dict_ (+6 more)

### Community 162 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 163 - "Interpreter.cpp"
Cohesion: 0.27
Nodes (12): branchTruth(), clearNonlocal(), consumeNonlocal(), DepthGuard, outermost, hit(), Leave, leave (+4 more)

### Community 164 - "CallContext"
Cohesion: 0.19
Nodes (11): ActiveGuard, rootShared, saved, checkInterrupt(), contextAlive(), CallContext, disarmAccept(), FrameLink (+3 more)

### Community 165 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 166 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 167 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 168 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 169 - "Vendor Class Library (import, do not write)"
Cohesion: 0.20
Nodes (10): CLAUDE.md, Bootstrap Procedure, Change Rules (§8), Constraints (§5), Cuis Smalltalk, Graphify and Serena Requirement, Kernel Classes (native-only), v1 Completion Definition (+2 more)

### Community 170 - "ImageFormat.cpp"
Cohesion: 0.21
Nodes (10): bit, byte, size_t, string, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader() (+2 more)

### Community 171 - "P1 — オブジェクトメモリ"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 172 - "Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）"
Cohesion: 0.39
Nodes (11): Task 5: C++ を薄い層にする（記録の送信、ABI の変換、busy の読み出しの拒否）, boolean(), CallContext, string, string_view, noteClassChange(), noteClassRemoval(), noteMethodChange() (+3 more)

### Community 173 - "clearUnwinding"
Cohesion: 0.27
Nodes (12): clearUnwinding(), ao_AoTest_assert_equals_(), CallContext, path, Root, string, string_view, uint32_t (+4 more)

### Community 174 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 175 - "Gc"
Cohesion: 0.18
Nodes (13): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress (+5 more)

### Community 176 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 177 - "send2"
Cohesion: 0.12
Nodes (19): ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RebindsBagAndEvaluatesInstVar, RectangleWidthHeightContainsAndIntersect (+11 more)

### Community 178 - "CompileError"
Cohesion: 0.14
Nodes (29): CompileError, message, span, string, File Structure, Task 6: File In（`ao_filein_file`）, FileInMode, acceptClassSource() (+21 more)

### Community 179 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 180 - "Parsed"
Cohesion: 0.20
Nodes (11): decodeNonHeap, acceptWord(), unordered_set, Parsed, globals, heapBytes, offsets, section (+3 more)

### Community 181 - ".aoimage Image Format"
Cohesion: 0.12
Nodes (16): Failure Aborts Evaluation, allocateRetry, ao_image_load, AoSpan.message (error reason), .aoimage Image Format, Base Process, Cooperative Process Scheduler (fibers), ensureKernelNatives (+8 more)

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
Cohesion: 0.19
Nodes (12): ClassDefinitionAnswersTheBrowserText, DefinitionIsNotUnderstoodByAMetaclass, acceptClass(), acceptMethod(), string, testing::Test, literal(), printIt() (+4 more)

### Community 186 - "collectNursery"
Cohesion: 0.36
Nodes (10): collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld() (+2 more)

### Community 187 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 188 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 189 - "P10 — 事後デバッガ"
Cohesion: 0.25
Nodes (8): P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 範囲, 結論

### Community 190 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 191 - "StackPool"
Cohesion: 0.29
Nodes (7): array, size_t, kPoolLimit, pooledCount, StackPool, count, stacks

### Community 193 - "P3 — ネイティブディスパッチ"
Cohesion: 0.16
Nodes (15): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary (+7 more)

### Community 195 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 196 - "Frame"
Cohesion: 0.43
Nodes (5): ContextExitGuard, Frame, Roots, FieldRoots, frame

### Community 197 - "TEST"
Cohesion: 0.09
Nodes (27): BlockAssignmentIsBindingStore, BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), string (+19 more)

### Community 198 - "ao_main.cpp"
Cohesion: 0.19
Nodes (19): climits, Task 0: ブランチ, Task 3: Smalltalk のモデル（記録・種類・理由・Remove / Move）と Tools-model の file-in, dyld, runtime, addRoots, bootAndRunTests(), collectLoadOrders() (+11 more)

### Community 199 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 200 - "NativeMethod.hpp"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 201 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 202 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.40
Nodes (5): PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 203 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 204 - "ObjectHeader"
Cohesion: 0.33
Nodes (6): uint16_t, ObjectHeader, flags, hash, klass, size

### Community 205 - "SelectedFrames"
Cohesion: 0.33
Nodes (4): SelectedFrames, frames_, live_, none_

### Community 206 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 207 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 208 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 209 - "Graphify Required Tool"
Cohesion: 0.67
Nodes (3): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps)

### Community 210 - "P9-04 v1 Golden Acceptance"
Cohesion: 0.40
Nodes (5): P9-04 v1 Golden Acceptance, GRAPH_REPORT.md Process Check, ao_Object_identityEquals Native Method, Serena Symbol Resolution, SPEC.md §6 Acceptance Checklist

### Community 211 - "ao CMake Project"
Cohesion: 0.67
Nodes (4): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target)

### Community 213 - "createBlock"
Cohesion: 0.60
Nodes (4): CallContext, uint8_t, createBlock(), createMethod()

### Community 216 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 217 - "Debug it (AO_EVAL_DEBUGIT)"
Cohesion: 0.47
Nodes (5): Debug it (AO_EVAL_DEBUGIT), MainMenu.swift: Debug it ⌘⇧D, Evaluation Process (live mode), Live Debugger (P11), Live debugger Step into / over / out (depth-based step flag)

### Community 218 - "NativeFrame"
Cohesion: 0.50
Nodes (4): Roots, NativeFrame, argc, slots

### Community 219 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 220 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 221 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 222 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 223 - "test (macOS arm64) Job"
Cohesion: 0.67
Nodes (3): Kernel Scan Test (via ctest), test (macOS arm64) Job, CI Workflow

### Community 224 - "lookup_test.cpp"
Cohesion: 0.60
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 225 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 240 - "acceptMethodInto (definition)"
Cohesion: 0.50
Nodes (4): acceptMethodSource, MethodDictionary::atPut, acceptMethodInto (definition), installMethod (definition)

### Community 247 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 250 - "SlotNames"
Cohesion: 0.67
Nodes (3): SlotNames, names, WellKnown

### Community 251 - "RootedArray::RootedArray"
Cohesion: 0.67
Nodes (3): Roots, uint32_t, RootedArray::RootedArray()

### Community 252 - "ImageSelector"
Cohesion: 0.67
Nodes (3): ImageSelector, name, WellKnown

### Community 253 - "NamedClass"
Cohesion: 0.67
Nodes (3): NamedClass, name, WellKnown

### Community 355 - "前提（コードで確かめた事実。`b35efe8`）"
Cohesion: 0.17
Nodes (30): ChunkMode, P16 チェンジセットと Change Sorter — 実装計画, Task 0: ブランチ, Task 1: SPEC を先に直す（P16 の追記）, Task 3: Accept と削除を記録する, 前提（コードで確かめた事実。`b35efe8`）, File Structure, P17 Smalltalk のチェンジセット — 実装計画 (+22 more)

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
- **118 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

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
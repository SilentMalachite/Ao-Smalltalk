# Graph Report - ao-smalltalk  (2026-09-27)

## Corpus Check
- 26 files · ~321,088 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 6114 nodes · 15525 edges · 336 communities (213 shown, 123 thin omitted)
- Extraction: 89% EXTRACTED · 11% INFERRED · 0% AMBIGUOUS · INFERRED: 1752 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- WellKnown
- Compile.cpp
- TEST_F()
- Dictionary.cpp
- TEST()
- Interpreter.cpp
- TEST_F()
- Ast
- string
- DebuggerWindow
- TEST()
- Oop
- LargeInteger.cpp
- Parser
- Scheduler
- BrowserWindow
- Stream.cpp
- debug_abi_test.cpp
- Heap
- AcceptTests
- Emitter
- Scanner.cpp
- Boot
- TEST()
- abi.cpp
- WorkspaceWindow
- Scheduler.cpp
- DebuggerWindowTests
- Session
- cstdint
- TEST()
- TEST()
- ImageSave.cpp
- Bytecode interpreter
- TEST()
- TEST()
- TEST()
- TranscriptWindow
- CallContext
- Heap.cpp
- Float.cpp
- .false_()
- Session.cpp
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- TEST()
- TEST_F()
- AoApp
- InspectorWindow
- .isHeap()
- Literal
- WellKnown.cpp
- TEST_F()
- VendorExtract.cpp
- send()
- TEST_F()
- String.cpp
- putNative()
- Analysis
- ToolWindowTests
- uint32_t
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- HashedCollection.cpp
- BrowserModel
- AoSpan
- FiberStack
- P1 — オブジェクトメモリ
- Array.cpp
- ao_runtime_shutdown()
- Gc
- DebugSnapshot
- TEST()
- MethodSource
- DebugSnapshot.cpp
- Geometry.cpp
- TEST()
- TEST()
- vector
- P12 Browser 削除 実装計画
- Roots
- SmallInteger.cpp
- Scheduler::Record
- Oop
- TEST()
- TEST_F()
- Claude Review Fixes Plan (2026-09-23)
- ImageLoad.cpp
- .base()
- Boolean.cpp
- P6b Vendor File-in Implementation Plan
- TEST_F()
- LiveFrames
- ClassPool.cpp
- Bootstrap.cpp
- BlockContext.cpp
- TEST()
- Bootstrap
- ao_remove_class
- Method Removal Rules
- Global Dictionary (Smalltalk)
- collectNursery
- blankOut()
- uint64_t
- AppKit
- ImageHeader
- pc_map_test.cpp
- TEST()
- CompileError
- Process.cpp
- MainMenu
- TEST()
- ao_main.cpp
- Roots.cpp
- NativeMethod.cpp
- BrowserModelTests
- TEST()
- TEST()
- PingPong
- ImageSurgery
- Cooperative Process Scheduler
- P12 Browser Removal Design
- TEST()
- Fiber.cpp
- GarbageFirstBoot
- native_method_test.cpp
- TEST()
- TEST()
- TEST()
- DiskHeader
- CountingSink
- Loaded
- Failure Aborts Evaluation (abortEvaluation)
- ChunkAction
- ChunkParser.cpp
- FiberRegs
- Globals.cpp
- VirtualRegion.cpp
- DefinitionScanner
- TEST()
- TEST()
- TEST()
- DebugFrames
- Stack
- TEST()
- P4-02: Behavior / ClassDescription / Class / Metaclass
- Deferred vendor methods list
- ImageFormat
- RootedArray
- allocateRetry()
- KeptClass
- TEST()
- Plan architecture: Frame chain + snapshot + pcMap
- TEST()
- abortEvaluation / abortEvaluationQuiet
- .isTrue()
- clearUnwinding()
- TEST()
- ClassMethodCache
- FrameBlock
- Vendor.hpp
- path
- Session Source Table (not in image)
- P11 step branch bench
- P11 conclusion: live debugger
- Token
- CompiledMethodNatives.cpp
- TEST()
- specialIndex()
- Proceed/Abort/Step into,over,out operations
- ao CMake Project
- P3-04: lookup / super / doesNotUnderstand:
- TEST()
- TEST()
- Image::load()
- contextPc()
- MethodDictionary.cpp
- native_send_test.cpp
- RawChunk
- P10-01..P10-07 PR list
- P0-03: CMake + GoogleTest + CLI
- P2-03: メタクラス循環（Blue Book 6–10）
- TEST()
- SelectedFrames
- Counts
- WellKnown::named()
- sessionDebugClear()
- ChunkMethod
- P10 scope (do/don't)
- Parsed
- Imported Class Library (no self-authored library)
- Graphify Required Tool
- P8–P9 Remaining Implementation Plan
- FileSizeLimit
- Rec
- cli_test.sh
- P9-04 v1 Golden Acceptance
- popFrame
- WellKnown::WellKnown()
- TestDir
- ScopedGcStressEnv
- test.sh
- TEST()
- TEST()
- Roots::visitAll()
- abortingSubclass()
- Root
- imageRegistryStubA()
- acceptClass()
- answerOne()
- .turnRunLoop()
- Rules That Do Not Bend
- globalsVersion（グローバル名の版）
- Session::MethodSource table
- HashNesting
- .specialSelector()
- abortingNew()
- build.sh
- WriteStream String Writes (reserve capacity)
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
- v1 に無いもの
- uint64_t
- package-app.sh
- Closures and Shared Temps (temp vector)
- Smalltalk グローバル辞書（tally/array の対）
- CChar
- UnsafeMutablePointer
- Any
- Int
- NSRect
- NSScrollView
- NSTableColumn
- NSTextField
- NSWindow
- T
- UInt
- PHASE File
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
- v1 Status Table
- runtime/CMakeLists.txt build config
- MethodImage
- pair
- path
- Root
- Visit
- EvalEnd
- HostOopHook
- MethodImage
- DebugSink
- deque
- unordered_map
- MethodImage
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- Interval Semantics
- v1 Non-Goals
- OrderedCollection Layout
- Repository Layout (runtime/compiler/image/app/bridge)

## God Nodes (most connected - your core abstractions)
1. `Oop` - 834 edges
2. `Heap` - 232 edges
3. `WellKnown` - 168 edges
4. `vector` - 136 edges
5. `TEST_F()` - 111 edges
6. `BrowserWindow` - 111 edges
7. `TEST()` - 107 edges
8. `Ast` - 103 edges
9. `TEST()` - 101 edges
10. `Session` - 101 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `Linear MethodDictionary (interleaved keys/values)` --semantically_similar_to--> `Global Dictionary (Smalltalk)`  [INFERRED] [semantically similar]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md → SPEC.md
- `Native Method Rebind by Name (no function pointers serialized)` --implements--> `addNamed()`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/NativeMethod.cpp
- `Host Word Patch (ao-host-patch)` --implements--> `extractVendor()`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp
- `LOAD_ORDER Dependency Sequencing` --implements--> `extractVendor()`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — docs_claude_review_00_recent_diff_classdef_string_silently_dropped, docs_claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, docs_claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, docs_claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **Spec-first design refinement of the debugger across P10 and P11** — docs_prs_p10_01_spec313, docs_prs_p11_01_spec313live, docs_prs_p11_01_p11plan [INFERRED 0.75]
- **Performance regression guardrail via docs/bench.md** — docs_bench_p11_step_branch, phases_p10_constraints, phases_p11_constraints [INFERRED 0.75]
- **Vendor Loading Kernel-Integrity Safeguards** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_allowlist, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_native_overwrite_refusal, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_kernel_scan_narrowing, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_host_patch [INFERRED 0.80]
- **Vendor File-in to Image Snapshot Pipeline** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_load_order, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_link_class, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.80]
- **AppKit ツールウィンドウ群とメニュー** — docs_prs_p8_01_ao_app_skeleton, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [INFERRED 0.80]
- **Vendor extraction, deferral list, and file-in test form one pipeline** — image_vendor_origin_extract_process, image_vendor_deferred_doc, runtime_tests_vendor_filein_test [INFERRED 0.80]
- **aoimage Method Persistence Across Restart** — docs_prs_p9_04_aoimage_restart, docs_superpowers_plans_2026_09_22_p7_aoimage_save_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_load_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_native_rebind_by_name, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.85]
- **P10 postmortem debugger data pipeline: pc-map → snapshot → ABI → window** — docs_prs_p10_02_methodimage, docs_prs_p10_03_debugsnapshot, docs_prs_p10_05_aoabih, app_ao_debuggerwindow [INFERRED 0.85]
- **P11 live debugger control flow: eval process → halt → resume/step** — docs_prs_p11_02_evalprocess, docs_prs_p11_03_stoporabort, docs_prs_p11_04_resumehalted, docs_prs_p11_05_stepabi [INFERRED 0.85]
- **Live debugging operation flow (evaluating process + halt + operations)** — phases_p11_evaluating_process, phases_p11_halt_mechanism, phases_p11_operations [INFERRED 0.85]
- **Post-mortem debugging pipeline (frame chain + snapshot + window)** — phases_p10_callcontext_topframe, plans_2026_09_26_p10_debugger_capture_design, phases_p10_debugger_window_spec [INFERRED 0.85]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — docs_prs_p1_02_pr, docs_prs_p1_03_pr, docs_prs_p1_04_pr, docs_prs_p1_05_pr, docs_prs_p1_06_pr, docs_prs_p1_07_pr [INFERRED 0.85]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — docs_prs_p4_01_doc, docs_prs_p4_02_doc, docs_prs_p4_03_doc, docs_prs_p4_04_doc, docs_prs_p4_05_doc, docs_prs_p4_06_doc, docs_prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunkparser [INFERRED 0.85]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]
- **Method removal pipeline: ao_remove_method → removeMethodNamed → removeKey / forgetMethodSource / cache invalidation** — spec_ao_remove_method, docs_superpowers_plans_2026_09_26_p12_browser_remove_removemethodnamed, docs_superpowers_plans_2026_09_26_p12_browser_remove_removekey, docs_superpowers_plans_2026_09_26_p12_browser_remove_forgetmethodsource, spec_cache_invalidation [INFERRED 0.85]
- **Browser removal confirmation flow: context menu → confirmRemove → model refresh** — docs_superpowers_plans_2026_09_26_p12_browser_remove_confirmremove, docs_superpowers_plans_2026_09_26_p12_browser_remove_mainmenu_actions, docs_superpowers_plans_2026_09_26_p12_browser_remove_browsermodel_select, spec_browser_removal_ui [INFERRED 0.80]
- **Class removal refusal checks: fixed global, Kernel class, live subclass** — spec_ao_remove_class, docs_superpowers_plans_2026_09_26_p12_browser_remove_removeclassnamed, spec_fixed_global, spec_kernel_class, docs_superpowers_plans_2026_09_26_p12_browser_remove_hassubclass [INFERRED 0.85]

## Communities (336 total, 123 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (146): array, InternTable, unique_ptr, WellKnown, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+138 more)

### Community 1 - "Compile.cpp"
Cohesion: 0.07
Nodes (104): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, Boot, cctype, ChunkAction, CompileEnv (+96 more)

### Community 2 - "TEST_F()"
Cohesion: 0.03
Nodes (77): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+69 more)

### Community 3 - "Dictionary.cpp"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 4 - "TEST()"
Cohesion: 0.02
Nodes (85): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+77 more)

### Community 5 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (70): Frame, context, depth, isBlock, method, pc, prev, receiver (+62 more)

### Community 6 - "TEST_F()"
Cohesion: 0.03
Nodes (83): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+75 more)

### Community 7 - "Ast"
Cohesion: 0.04
Nodes (68): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+60 more)

### Community 8 - "string"
Cohesion: 0.10
Nodes (39): algorithm, chrono, Chunk, classpool, cmath, compile, CompiledMethod, compiler (+31 more)

### Community 9 - "DebuggerWindow"
Cohesion: 0.05
Nodes (47): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+39 more)

### Community 10 - "TEST()"
Cohesion: 0.03
Nodes (78): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+70 more)

### Community 11 - "Oop"
Cohesion: 0.09
Nodes (67): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+59 more)

### Community 12 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 13 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 14 - "Scheduler"
Cohesion: 0.04
Nodes (68): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+60 more)

### Community 15 - "BrowserWindow"
Cohesion: 0.06
Nodes (34): Any, BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels (+26 more)

### Community 16 - "Stream.cpp"
Cohesion: 0.11
Nodes (63): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+55 more)

### Community 17 - "debug_abi_test.cpp"
Cohesion: 0.06
Nodes (62): CLAUDE.md, docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, Codegen.cpp: Emitter::mark と compile* 群, MethodImage.hpp: PcSpan / TempName / pcMap / temps (+54 more)

### Community 18 - "Heap"
Cohesion: 0.04
Nodes (53): Heap, checkNotPoisoned, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_ (+45 more)

### Community 19 - "AcceptTests"
Cohesion: 0.14
Nodes (9): AcceptTests, NSMenu, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+1 more)

### Community 20 - "Emitter"
Cohesion: 0.14
Nodes (11): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+3 more)

### Community 21 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 22 - "Boot"
Cohesion: 0.05
Nodes (58): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+50 more)

### Community 23 - "TEST()"
Cohesion: 0.04
Nodes (58): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+50 more)

### Community 24 - "abi.cpp"
Cohesion: 0.07
Nodes (54): atomic, Body, CountsAnswerMinusOneOnFailure, interpreter, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at(), ao_browser_class_count() (+46 more)

### Community 25 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (23): AnyObject, installDebugButton(), installErrorField(), sendToKeyWorkspace(), Bool, Int, NSButton, NSTextField (+15 more)

### Community 26 - "Scheduler.cpp"
Cohesion: 0.11
Nodes (53): flags, size, slotAt, slotAtPut, addFiber, enqueue, find, liveFibers (+45 more)

### Community 27 - "DebuggerWindowTests"
Cohesion: 0.11
Nodes (13): DebuggerWindowTests, Int, NSButton, NSFont, NSTableView, NSTextView, NSView, NSWindow (+5 more)

### Community 28 - "Session"
Cohesion: 0.05
Nodes (55): ClassMethodCache, DebugSink, HostOopHook, attachBlocks(), blockLiteral(), clearMethodSources(), int64_t, Roots (+47 more)

### Community 29 - "cstdint"
Cohesion: 0.07
Nodes (15): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, DebugSink (+7 more)

### Community 30 - "TEST()"
Cohesion: 0.04
Nodes (53): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+45 more)

### Community 31 - "TEST()"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 32 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (50): cerrno, fcntl, Image, check, load, save, encodeNonHeap, writeFiller (+42 more)

### Community 33 - "Bytecode interpreter"
Cohesion: 0.05
Nodes (51): P4-08: Point / Rectangle, Point/Rectangle 座標計算, P4-09: Kernel NativeMethod 走査と bench, Kernel NativeMethod 走査, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+43 more)

### Community 34 - "TEST()"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 35 - "TEST()"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 36 - "TEST()"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 37 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+20 more)

### Community 38 - "CallContext"
Cohesion: 0.05
Nodes (45): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+37 more)

### Community 39 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 40 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 41 - ".false_()"
Cohesion: 0.13
Nodes (37): superclassOf(), isPseudoVariableName, boolean(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize() (+29 more)

### Community 42 - "Session.cpp"
Cohesion: 0.13
Nodes (36): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+28 more)

### Community 43 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 44 - "TEST()"
Cohesion: 0.06
Nodes (38): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+30 more)

### Community 45 - "TEST_F()"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 46 - "AoApp"
Cohesion: 0.09
Nodes (21): AoApp, fileInVendor(), openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification (+13 more)

### Community 47 - "InspectorWindow"
Cohesion: 0.10
Nodes (23): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String (+15 more)

### Community 48 - ".isHeap()"
Cohesion: 0.13
Nodes (35): run, InlineCache, cachedClass, cachedMethod, applyMethod(), abortDoesNotUnderstand(), abortEvaluation(), abortEvaluationQuiet() (+27 more)

### Community 49 - "Literal"
Cohesion: 0.06
Nodes (38): int16_t, int64_t, LitKind, string, uint16_t, uint32_t, uint8_t, unique_ptr (+30 more)

### Community 50 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (35): deque, intern, internWith, isCatalogName, Oop, size_t, string, string_view (+27 more)

### Community 51 - "TEST_F()"
Cohesion: 0.06
Nodes (38): KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad, RemoveAcceptedMethodOnKernelClass, RemoveClassKeepsAliases, RemoveClassNamesTheSmallestDescendant, RemoveClassSideMethod (+30 more)

### Community 52 - "VendorExtract.cpp"
Cohesion: 0.16
Nodes (37): allowIndex(), containsHostWord(), size_t, string, string_view, doubleBangs(), dropCycles(), dropMissingSupers() (+29 more)

### Community 53 - "send()"
Cohesion: 0.17
Nodes (34): int64_t, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_() (+26 more)

### Community 54 - "TEST_F()"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 55 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 56 - "putNative()"
Cohesion: 0.08
Nodes (36): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+28 more)

### Community 57 - "Analysis"
Cohesion: 0.06
Nodes (30): Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, CompileResult (+22 more)

### Community 58 - "ToolWindowTests"
Cohesion: 0.13
Nodes (10): LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String, T (+2 more)

### Community 59 - "uint32_t"
Cohesion: 0.14
Nodes (21): DebugFrames, uint32_t, debugFrameCount(), debugFrameKind(), debugFrameLabel(), debugFramePc(), debugFrameTotal(), debugInfoFor() (+13 more)

### Community 60 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 61 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (31): hashedcollection, CallContext, int64_t, uint32_t, Root, Table, array, capacity (+23 more)

### Community 62 - "BrowserModel"
Cohesion: 0.20
Nodes (8): BrowserModel, .metaFlag, ListedClass, Bool, Int32, String, CChar, UnsafeMutablePointer

### Community 63 - "AoSpan"
Cohesion: 0.11
Nodes (32): AoSpan, end, message, start, ao_accept_class(), ao_accept_method(), ao_debug_proceed(), ao_debug_step_into() (+24 more)

### Community 64 - "FiberStack"
Cohesion: 0.10
Nodes (26): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+18 more)

### Community 65 - "P1 — オブジェクトメモリ"
Cohesion: 0.11
Nodes (31): P0 (親フェーズ, stub), ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P2 — ブートストラップ, WellKnown.hpp — well-known クラス表 (+23 more)

### Community 66 - "Array.cpp"
Cohesion: 0.14
Nodes (30): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), valueHashWord(), ao_Array_equals() (+22 more)

### Community 67 - "ao_runtime_shutdown()"
Cohesion: 0.08
Nodes (22): AbiSmoke, AoTranscriptFn, BootAndShutdownReturnZero, BootVersionShutdown, ao_runtime_shutdown(), ao_set_debug_mode(), ao_set_inspect_hook(), ao_set_transcript_hook() (+14 more)

### Community 68 - "Gc"
Cohesion: 0.09
Nodes (27): cstdlib, Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_ (+19 more)

### Community 69 - "DebugSnapshot"
Cohesion: 0.07
Nodes (26): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+18 more)

### Community 70 - "TEST()"
Cohesion: 0.08
Nodes (29): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+21 more)

### Community 71 - "MethodSource"
Cohesion: 0.08
Nodes (29): PcSpan, DebugInfo, bodies, DebugInfoRef, body, index, source, Oop (+21 more)

### Community 72 - "DebugSnapshot.cpp"
Cohesion: 0.16
Nodes (26): frameAt, uint32_t, DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method(), DebugSnapshot::pc(), DebugSnapshot::process(), DebugSnapshot::receiver() (+18 more)

### Community 73 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 74 - "TEST()"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 75 - "TEST()"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 76 - "vector"
Cohesion: 0.11
Nodes (23): vector, csignal, mach, mach_vm, resource, acceptMethods(), Check, printed (+15 more)

### Community 77 - "P12 Browser 削除 実装計画"
Cohesion: 0.12
Nodes (28): P12 — Browser の削除（phase doc）, SPEC.md, docs/README.md（実装文書ガイド）, BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove, Session::forgetMethodSource, Globals::unbind, hasSubclass（生きているサブクラスの判定） (+20 more)

### Community 78 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 79 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 80 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, string, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+20 more)

### Community 81 - "Oop"
Cohesion: 0.21
Nodes (27): argCountOf(), byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), Heap, Oop (+19 more)

### Community 82 - "TEST()"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 83 - "TEST_F()"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 84 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.11
Nodes (27): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+19 more)

### Community 85 - "ImageLoad.cpp"
Cohesion: 0.22
Nodes (26): ifstream, atOffset(), checkFile(), byte, size_t, string, T, uint32_t (+18 more)

### Community 86 - ".base()"
Cohesion: 0.14
Nodes (25): afterResume, endEval, leaveLists, reapDead, recordFailure, switchTo, takeNext, Record (+17 more)

### Community 87 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 88 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.11
Nodes (26): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+18 more)

### Community 89 - "TEST_F()"
Cohesion: 0.08
Nodes (22): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+14 more)

### Community 90 - "LiveFrames"
Cohesion: 0.09
Nodes (19): CallContext, Frame, size_t, string, uint32_t, LiveFrames, context, frames_ (+11 more)

### Community 91 - "ClassPool.cpp"
Cohesion: 0.20
Nodes (24): isClassShaped(), adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view (+16 more)

### Community 92 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (24): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+16 more)

### Community 93 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 94 - "TEST()"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 95 - "Bootstrap"
Cohesion: 0.14
Nodes (15): Bootstrap, MethodDictionary, RemoveUnits, string, Heap, Oop, Roots, WellKnown (+7 more)

### Community 96 - "ao_remove_class"
Cohesion: 0.14
Nodes (23): ao_debug_* ABI, Debugger window (P10/P11), Kernel NativeMethod scan test, Live debug mode (AO_DEBUG_LIVE), 設計理由: Smalltalk 側削除セレクタではなく C ABI を採用, 設計理由: クラス削除は束縛だけを外す最小操作, 設計理由: 固定グローバルと Kernel 別名は拒む, ao_accept_class (+15 more)

### Community 97 - "Method Removal Rules"
Cohesion: 0.11
Nodes (24): MethodDictionary, Broken Superclass Chain Rule (1024 limit), invalidateMethodCache（単一の無効化経路）, Chunk Format, Class Names as Interned Symbols, CompiledMethod, Compiler (.st to CompiledMethod), = and hash Contract (+16 more)

### Community 98 - "Global Dictionary (Smalltalk)"
Cohesion: 0.13
Nodes (24): Ao Bytecode Set, Ao SPEC (macOS native Smalltalk), .aoimage Image Format (version 3), Blue Book (Smalltalk-80: The Language and its Implementation), Blue Book Conformance Definition, Bootstrap Procedure, Class Definition Re-Accept, Fixed Globals (56 Kernel classes + Processor + Smalltalk) (+16 more)

### Community 99 - "collectNursery"
Cohesion: 0.11
Nodes (23): BlockContextKeepsHomeAndCopied, CleanupFailureReleasesItsReasonHandle, ContextGc, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, MethodContextSurvivesNurseryCollection (+15 more)

### Community 100 - "blankOut()"
Cohesion: 0.17
Nodes (21): EvalEnd, answerAwaited(), answerEval(), blankOut(), AoInspectFn, optional, StepMode, DebugEntry (+13 more)

### Community 101 - "uint64_t"
Cohesion: 0.24
Nodes (15): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+7 more)

### Community 102 - "AppKit"
Cohesion: 0.13
Nodes (11): Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, transcriptBoxHook(), SmokeTests, AppKit (+3 more)

### Community 103 - "ImageHeader"
Cohesion: 0.11
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 104 - "pc_map_test.cpp"
Cohesion: 0.13
Nodes (21): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, uint32_t, firstBlock(), named() (+13 more)

### Community 105 - "TEST()"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 106 - "CompileError"
Cohesion: 0.11
Nodes (22): ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span, string, KeepsNativeIdentityEquals, RebindsBagAndEvaluatesInstVar, RefusesKernelClassThroughAlias (+14 more)

### Community 107 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 108 - "MainMenu"
Cohesion: 0.22
Nodes (10): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+2 more)

### Community 109 - "TEST()"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 110 - "ao_main.cpp"
Cohesion: 0.16
Nodes (17): climits, dyld, runtime, addRoots, bootAndRunTests(), string, imageUsage(), main() (+9 more)

### Community 111 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 112 - "NativeMethod.cpp"
Cohesion: 0.22
Nodes (19): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+11 more)

### Community 113 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 114 - "TEST()"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 115 - "TEST()"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 116 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 117 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 118 - "Cooperative Process Scheduler"
Cohesion: 0.11
Nodes (20): Abandon (terminate without cleanup), allocateRetry (GC-capable allocation), Base Process, Cooperative Process Scheduler, Deadlock: no runnable process, Debugger (P10/P11), Evaluation Process, Fiber Process Stacks (8 MiB + guard page) (+12 more)

### Community 119 - "P12 Browser Removal Design"
Cohesion: 0.15
Nodes (20): Acceptance Criteria, Browser Removal (P12 remove rules), Browser Remove UI (context menu + confirmation sheet), Browser Removal XCTests, CI (GitHub Actions), GC Stress Mode (AO_GC_STRESS, gcstress/gcstress_vendor), Golden Evaluation (ao --test), P12 Browser Removal Acceptance (+12 more)

### Community 120 - "TEST()"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 121 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 122 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 123 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 124 - "TEST()"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 125 - "TEST()"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 126 - "TEST()"
Cohesion: 0.14
Nodes (17): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, evalBody(), fillNursery() (+9 more)

### Community 127 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 128 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 129 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 130 - "Failure Aborts Evaluation (abortEvaluation)"
Cohesion: 0.12
Nodes (17): Failure Aborts Evaluation (abortEvaluation), ao_eval, Host GUI (AppKit), Debug Snapshot Capture, Debugger Readout ABI (ao_debug_*), Debugger Window, Evaluation Drain (1000 yields), Empty OOP (native failure marker) (+9 more)

### Community 131 - "ChunkAction"
Cohesion: 0.12
Nodes (16): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+8 more)

### Community 132 - "ChunkParser.cpp"
Cohesion: 0.30
Nodes (15): classify(), string_view, firstLineHas(), isBlank(), isCharacterLiteral(), isLetter(), isProseApostrophe(), isSoleDefinition() (+7 more)

### Community 133 - "FiberRegs"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

### Community 134 - "Globals.cpp"
Cohesion: 0.28
Nodes (15): at(), bind(), bindIn(), Heap, Oop, Roots, string_view, uint32_t (+7 more)

### Community 135 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 136 - "DefinitionScanner"
Cohesion: 0.34
Nodes (7): DefinitionScanner, Bool, Token, keyword, other, Equatable, Unicode

### Community 137 - "TEST()"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 138 - "TEST()"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 139 - "TEST()"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 140 - "DebugFrames"
Cohesion: 0.13
Nodes (14): DebugFrames, context, count, empty, kind, method, pc, receiver (+6 more)

### Community 141 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 142 - "TEST()"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 143 - "P4-02: Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.14
Nodes (14): P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード, P4-04: Array / ByteArray / String / Symbol (+6 more)

### Community 144 - "Deferred vendor methods list"
Cohesion: 0.16
Nodes (14): B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12), MappedCollection absent from pinned sources (+6 more)

### Community 145 - "ImageFormat"
Cohesion: 0.15
Nodes (14): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes (+6 more)

### Community 146 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 147 - "allocateRetry()"
Cohesion: 0.20
Nodes (13): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+5 more)

### Community 148 - "KeptClass"
Cohesion: 0.14
Nodes (14): KeptClass, category, classVars, deferred, hasDef, instVars, methods, pools (+6 more)

### Community 149 - "TEST()"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 150 - "Plan architecture: Frame chain + snapshot + pcMap"
Cohesion: 0.15
Nodes (13): 1.0.0 release, P10 debugger plan reference, v1 = P9 completion, released as 1.0.0, .aoimage format v3 (debug info excluded), Plan architecture: Frame chain + snapshot + pcMap, Emitter::mark / compileSend / compileReturn / compileInlined, Frame chain design (P10-03), Plan goal: capture stack before unwind (+5 more)

### Community 151 - "TEST()"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 152 - "abortEvaluation / abortEvaluationQuiet"
Cohesion: 0.15
Nodes (13): ao_debug_* ABI design (P10-05), abortEvaluation / abortEvaluationQuiet, App window patterns (NSWindow, WorkspaceWindow, InspectorWindow), DebugSnapshot::capture design, Frame/Temps/OperandStack live on C++ stack, not heap, HostOopHook (transcript/inspect hooks), ao_Object_halt design (P10-03), performSend (+5 more)

### Community 153 - ".isTrue()"
Cohesion: 0.49
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 154 - "clearUnwinding()"
Cohesion: 0.26
Nodes (13): yield, Scheduler::drain(), clearUnwinding(), CallContext, path, Root, string, string_view (+5 more)

### Community 155 - "TEST()"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 156 - "ClassMethodCache"
Cohesion: 0.17
Nodes (12): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+4 more)

### Community 157 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 158 - "Vendor.hpp"
Cohesion: 0.20
Nodes (11): string, string_view, isVendorStub(), VendorClassFile, chunkText, className, superName, unsupportedShape (+3 more)

### Community 159 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 160 - "Session Source Table (not in image)"
Cohesion: 0.18
Nodes (12): ao_browser_source (placeholder), Class Variables (classPool of Associations), Damaged Table Rule, Hash Home Mixing (Fibonacci multiplier), Dictionary and Set Hash Tables (open addressing), pc-to-Source Map and Temp Names, Shape Change (recompile and move methods), Session Source Table (not in image) (+4 more)

### Community 161 - "P11 step branch bench"
Cohesion: 0.18
Nodes (11): P11 known limitations, v1 known limitations, B2 to:do: bench, SmallInteger fast path (no-send arithmetic), P11 step branch bench, P6 interpreter bench, P11 acceptance criteria, P11 constraints (one step flag, bench ratio, ABI signature frozen) (+3 more)

### Community 162 - "P11 conclusion: live debugger"
Cohesion: 0.22
Nodes (11): Unreleased: P10/P11 Debugger, P11 plan reference (p11-ancient-matsumoto.md), SPEC.md (product source of truth), P10 conclusion: post-mortem debugger, Busy & image-save rules for halted processes, P11 conclusion: live debugger, Evaluation process (forked, per SPEC §3.13), Halt mechanism (halt/error:/DNU/NonBoolean/cannot return) (+3 more)

### Community 163 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 164 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext, uint32_t (+2 more)

### Community 165 - "TEST()"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 166 - "specialIndex()"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 167 - "Proceed/Abort/Step into,over,out operations"
Cohesion: 0.20
Nodes (10): ao_set_debug_capture / ao_debug_*, Ao.app Debugger window, Live debugger, ao_set_debug_mode(AO_DEBUG_LIVE) / AO_ERR_HALT, Object>>halt, Post-mortem debugger, ao_debug_* ABI busy rules, Debug it (⌘⇧D) (+2 more)

### Community 168 - "ao CMake Project"
Cohesion: 0.24
Nodes (10): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target), リリース手順 (JA), Releasing Process (EN), Kernel Scan Test (via ctest), test (macOS arm64) Job (+2 more)

### Community 169 - "P3-04: lookup / super / doesNotUnderstand:"
Cohesion: 0.20
Nodes (10): P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, P3-03: NativeMethod とセレクタマングル, NativeMethod とセレクタマングル規則, Smalltalk-80 Blue Book（探索意味論）, P3-04: lookup / super / doesNotUnderstand:, lookup / super / doesNotUnderstand: の探索意味論 (+2 more)

### Community 170 - "TEST()"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 171 - "TEST()"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 172 - "Image::load()"
Cohesion: 0.31
Nodes (9): readHeader, bindAll(), checkGlobals(), Roots, string_view, WellKnown, fileOop(), Image::load() (+1 more)

### Community 173 - "contextPc()"
Cohesion: 0.44
Nodes (10): contextPc(), CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget() (+2 more)

### Community 174 - "MethodDictionary.cpp"
Cohesion: 0.40
Nodes (9): at(), atPut(), Heap, Oop, uint32_t, WellKnown, create(), growInner() (+1 more)

### Community 175 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 176 - "RawChunk"
Cohesion: 0.22
Nodes (9): string, Token, uint32_t, RawChunk, endsSection, span, text, undoubled (+1 more)

### Community 177 - "P10-01..P10-07 PR list"
Cohesion: 0.25
Nodes (9): P4 microbench, CLAUDE.md (process source of truth), PR table, P10 acceptance criteria, P10 constraints (no interpreter/native coupling, bench ratio), P10-01..P10-07 PR list, P10 TDD test files, P11-01..P11-07 PR list (+1 more)

### Community 178 - "P0-03: CMake + GoogleTest + CLI"
Cohesion: 0.25
Nodes (9): P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub, P0-06: Serena project (+1 more)

### Community 179 - "P2-03: メタクラス循環（Blue Book 6–10）"
Cohesion: 0.22
Nodes (9): P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10）, メタクラス循環, P2-04: Smalltalk グローバル辞書 (+1 more)

### Community 180 - "TEST()"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 181 - "SelectedFrames"
Cohesion: 0.25
Nodes (6): LiveFrames, debugReason(), SelectedFrames, frames_, live_, none_

### Community 182 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 183 - "WellKnown::named()"
Cohesion: 0.25
Nodes (5): findSymbol, global, isFixedGlobal, WellKnown::named(), WellKnown::undefine()

### Community 184 - "sessionDebugClear()"
Cohesion: 0.25
Nodes (8): bumpDebugGeneration(), CallContext, string_view, debugClear(), onAbort, sessionDebugClear(), sessionShutdown(), workspaceBinding()

### Community 185 - "ChunkMethod"
Cohesion: 0.29
Nodes (7): ChunkMethod, source, span, undoubled, string, uint32_t, fileSpan()

### Community 186 - "P10 scope (do/don't)"
Cohesion: 0.38
Nodes (7): Abort-time snapshot capture, CallContext::topFrame frame chaining, Debugger window spec (frames/source/variables), Object>>halt spec (reason halt), P10 scope (do/don't), Workspace Debug button / process failed:, DebugFrames abstraction (snapshot + live frames)

### Community 187 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 188 - "Imported Class Library (no self-authored library)"
Cohesion: 0.29
Nodes (7): SPEC Change Rules, Constraints (new VM, minimal deps, no copying), Cuis Smalltalk, File-in Errors and DEFERRED.md, Graphify and Serena Required, Startup and Vendor Bundling, Imported Class Library (no self-authored library)

### Community 189 - "Graphify Required Tool"
Cohesion: 0.40
Nodes (5): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps), Ao Smalltalk Overview (EN), Ao Smalltalk 概要 (JA)

### Community 190 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.33
Nodes (6): docs/bench.md, PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 191 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 192 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 193 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 194 - "P9-04 v1 Golden Acceptance"
Cohesion: 0.40
Nodes (5): P9-04 v1 Golden Acceptance, GRAPH_REPORT.md Process Check, ao_Object_identityEquals Native Method, Serena Symbol Resolution, SPEC.md §6 Acceptance Checklist

### Community 195 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 196 - "WellKnown::WellKnown()"
Cohesion: 0.40
Nodes (5): addRoots, Heap, Roots, WellKnown::addRoots(), WellKnown::WellKnown()

### Community 197 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 198 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 199 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 200 - "TEST()"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 201 - "TEST()"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 202 - "Roots::visitAll()"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 203 - "abortingSubclass()"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 204 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 205 - "imageRegistryStubA()"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 206 - "acceptClass()"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 207 - "answerOne()"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 209 - "Rules That Do Not Bend"
Cohesion: 0.67
Nodes (3): Fixed Design Decisions, 曲げない規則, Rules That Do Not Bend

### Community 210 - "globalsVersion（グローバル名の版）"
Cohesion: 0.67
Nodes (3): 設計理由: 削除も globalsVersion を進める, globalsVersion（グローバル名の版）, knownGlobals cache

### Community 211 - "Session::MethodSource table"
Cohesion: 0.67
Nodes (3): evalBody / sessionEval, Session::MethodSource table, Source table extension design (P10-04)

### Community 214 - "abortingNew()"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 216 - "WriteStream String Writes (reserve capacity)"
Cohesion: 0.67
Nodes (3): Streams (PositionableStream/WriteStream), String do: UTF-8 Single Pass, WriteStream String Writes (reserve capacity)

## Ambiguous Edges - Review These
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to
- `Debug it (AO_EVAL_DEBUGIT)` → `SPEC §3.13 デバッガ（捕捉の意味論）`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P4-09: Kernel NativeMethod 走査と bench` → `P5-01: 字句解析`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references
- `P7-03: load と NativeMethod 再結合` → `P8-01: Ao.app 骨格`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `P8-05: メニューとキー` → `P9-01: Do it / Print it / Inspect it`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references
- `Smalltalk-80 removeFromSystem` → `Blue Book (Smalltalk-80: The Language and its Implementation)`  [AMBIGUOUS]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md · relation: conceptually_related_to

## Knowledge Gaps
- **974 isolated node(s):** `CallContext`, `CallContext`, `CallContext`, `CallContext`, `Root` (+969 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2605 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **123 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `Debug it (AO_EVAL_DEBUGIT)` and `SPEC §3.13 デバッガ（捕捉の意味論）`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P4-09: Kernel NativeMethod 走査と bench` and `P5-01: 字句解析`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P7-03: load と NativeMethod 再結合` and `P8-01: Ao.app 骨格`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P8-05: メニューとキー` and `P9-01: Do it / Print it / Inspect it`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 296 files · ~301,552 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5915 nodes · 15346 edges · 299 communities (203 shown, 96 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 1836 edges (avg confidence: 0.82)
- Token cost: 411,767 input · 0 output

## Community Hubs (Navigation)
- WellKnown
- Float.cpp
- TEST_F()
- .fromSmallInteger()
- Heap
- Interpreter.cpp
- TEST()
- DebuggerWindowTests
- Compile.cpp
- BrowserWindow
- TEST()
- DebuggerWindow
- LargeInteger.cpp
- Oop
- Parser
- Stream.cpp
- Boot
- Codegen.cpp
- handlescope
- Emitter
- P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt
- send()
- TEST_F()
- Scanner.cpp
- TEST()
- abi.cpp
- string
- WorkspaceWindow
- Scheduler.cpp
- TEST_F()
- TEST()
- BrowserModel
- Bytecode interpreter
- TEST()
- TEST()
- TEST()
- vector
- AoSpan
- TEST()
- ImageSave.cpp
- TranscriptWindow
- Scheduler
- CallContext
- Session.cpp
- TEST()
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- uint32_t
- DefinitionScanner
- Heap.cpp
- TEST_F()
- allocateRetry()
- DebugSnapshot.cpp
- TEST()
- takeAbortReason()
- Ast
- .isHeap()
- Send.cpp
- String.cpp
- TEST_F()
- ToolWindowTests
- .nil()
- AoApp
- CompileEnv
- FiberStack
- HashedCollection.cpp
- Task 12: Browser accept, Hierarchy, VoiceOver, C
- P1 — オブジェクトメモリ
- ao_main.cpp
- P6b Vendor File-in Implementation Plan
- DebugSnapshot
- VendorExtract.cpp
- WellKnown.cpp
- Geometry.cpp
- Session
- TEST()
- TEST()
- TEST()
- Roots
- Scheduler::Record
- ImageFormat
- TEST()
- TEST()
- Claude Review Fixes Plan (2026-09-23)
- Boolean.cpp
- .base()
- Bootstrap.cpp
- classRows()
- TEST_F()
- TEST()
- ClassPool.cpp
- Ao (macOS Native Smalltalk System)
- LiveFrames
- Gc
- string
- AppKit
- TEST()
- DebugFrames
- Process.cpp
- NativeMethod.cpp
- ao_runtime_shutdown()
- TEST()
- ImageLoad.cpp
- Roots.cpp
- uint64_t
- evalBody()
- TEST()
- PingPong
- Dictionary/Set/IdentityDictionary/IdentitySet op
- TEST()
- Fiber.cpp
- MainMenu
- TEST()
- ImageSurgery
- TEST()
- checkFile()
- GarbageFirstBoot
- Loaded
- native_method_test.cpp
- TEST()
- TEST()
- DiskHeader
- KeptClass
- CountingSink
- ChunkAction
- ChunkParser.cpp
- FiberRegs
- MethodSource
- VirtualRegion.cpp
- Bootstrap procedure (9 ordered steps)
- TEST()
- TEST()
- TEST()
- Deferred vendor methods list
- Stack
- clearUnwinding()
- NativeMethod
- P10 scope (do/don't)
- P4-02: Behavior / ClassDescription / Class / Met
- RootedArray
- .isTrue()
- Debugger snapshot read ABI (ao_debug_frame_*, ao
- InspectorWindow
- abortEvaluation / abortEvaluationQuiet
- C ABI bridge (bridge/ao_abi.h)
- TEST()
- Literal
- uint32_t
- FrameBlock
- path
- P11 step branch bench
- vendor_filein_test.cpp
- MethodImage
- Token
- Plan architecture: Frame chain + snapshot + pcMa
- TEST()
- P3-04: lookup / super / doesNotUnderstand:
- ClassMethodCache
- CompiledMethodNatives.cpp
- Cooperative ProcessorScheduler / Process / Semap
- TEST()
- Proceed/Abort/Step into,over,out operations
- docs/README.md
- ao CMake Project
- Workspace window (Do it/Print it/Inspect it/Debu
- TempName
- RawChunk
- TEST()
- P0-03: CMake + GoogleTest + CLI
- P2-03: メタクラス循環（Blue Book 6–10）
- intern()
- Character.cpp
- Live debugger runtime semantics (§3.13 P11)
- TEST()
- Gc.cpp
- P10-01..P10-07 PR list
- ObjectHeader
- Counts
- ChunkMethod
- Heap::allocate()
- Parsed
- WellKnown::InternTable
- Graphify Required Tool
- P8–P9 Remaining Implementation Plan
- FileSizeLimit
- cli_test.sh
- TEST()
- PcSpan
- popFrame
- KeptMethod
- TestDir
- ScopedGcStressEnv
- test.sh
- TEST()
- TEST()
- NativeMethod.hpp
- Roots::visitAll()
- abortingSubclass()
- Root
- imageRegistryStubA()
- answerOne()
- Rules That Do Not Bend
- Session::MethodSource table
- HashNesting
- .specialSelector()
- abortingNew()
- build.sh
- Package.swift
- Native Method Symbol Naming Convention
- ClassMethodCache の無効化が定義クラスの分だけ
- out == NULL の Do it が副作用ありで AO_ERR を返す
- =は値で比較するのにhashは同一性ハッシュのままで=/hashの契約を破る
- int64を超える整数リテラルが黙って0になる
- Dictionary/Setがhashを捨てて線形探索し要素数の2乗で遅くなる
- Kernelクラスにinstance変数名が無くinstVarNamed:等が失敗する(既報関連
- ao_compiler library target
- v1 に無いもの
- uint64_t
- package-app.sh
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
- グローバル Smalltalk がクラス nil の 57 要素固定配列でどのメッセージも通らな
- BlockContextをクロージャとアクティベーションに兼用しsenderを上書きしたまま戻さ
- 死んだホームへの ^（cannotReturn:）が評価全体を理由なしに中断する
- DNUのMessage割り当てにGCリトライがなくナーサリ逼迫時にDNUが空OOPになる
- Process/Semaphoreが実行を切り替えない（fork本体は実行されずwaitもブロッ
- valueWithArguments:がArray以外のポインタオブジェクトを受け入れ内部スロッ
- asCharacterがサロゲート(U+D800-DFFF)を受け付けPrint itの出力が途
- bitShift:にシフト量-2^63を渡すと符号反転オーバーフロー(UB)になり誤った結果を返
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
- perform:withArguments:がArray以外も受け付けルートされない引数配列を渡
- ReadStreamのnextPut:が元のコレクションを書き換える
- ReadWriteStreamのcontentsがpositionまでしか返さない
- select:/reject:が述語ブロックを要素ごとに2回評価する
- shallowCopy・printString・asSymbolがnursery不足時にGCを再
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
- native block thunkがヒープへ逃げると保存は成功するがロードできないイメージにな
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
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- v1 Status Table
- Serena project.yml
- Primitive opcode (32)
- PushNewArray opcode (33)
- PushRemoteTemp/StoreRemoteTemp/PopStoreRemoteTem
- ReturnBlock opcode (30, non-local return)
- Return* opcodes (25-29)

## God Nodes (most connected - your core abstractions)
1. `Oop` - 937 edges
2. `Heap` - 252 edges
3. `vector` - 176 edges
4. `WellKnown` - 167 edges
5. `TEST_F()` - 111 edges
6. `TEST()` - 107 edges
7. `Ast` - 103 edges
8. `TEST()` - 101 edges
9. `Boot` - 101 edges
10. `Session` - 100 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `debugSpanAt()` --calls--> `end`  [INFERRED]
  runtime/src/Session.cpp → compiler/include/ao/MethodImage.hpp
- `Native Selector Overwrite Refusal` --implements--> `applyClassDef()`  [EXTRACTED]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `Native Method Rebind by Name (no function pointers serialized)` --implements--> `addNamed()`  [EXTRACTED]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/NativeMethod.cpp
- `Host Word Patch (ao-host-patch)` --implements--> `extractVendor()`  [EXTRACTED]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — claude_review_01_object_memory_scavenge_collectold_stale_write, claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — claude_review_00_recent_diff_classdef_string_silently_dropped, claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — prs_p1_02_pr, prs_p1_03_pr, prs_p1_04_pr, prs_p1_05_pr, prs_p1_06_pr, prs_p1_07_pr [INFERRED 0.85]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — prs_p3_01_doc, prs_p3_02_doc, prs_p3_03_doc, prs_p3_04_doc, prs_p3_05_doc [INFERRED 0.95]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — prs_p4_01_doc, prs_p4_02_doc, prs_p4_03_doc, prs_p4_04_doc, prs_p4_05_doc, prs_p4_06_doc, prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — prs_p5_01_scanner, prs_p5_02_parser, prs_p5_03_bytecode_isa, prs_p5_04_codegen, prs_p5_05_chunkparser [INFERRED 0.85]
- **.aoimage 保存/読み込みラウンドトリップ** — prs_p7_01_aoimage_format, prs_p7_02_imagesave, prs_p7_03_imageload_rebind [INFERRED 0.90]
- **AppKit ツールウィンドウ群とメニュー** — prs_p8_01_ao_app_skeleton, prs_p8_02_transcript_window, prs_p8_03_workspace_window, prs_p8_04_browser_window, prs_p8_05_main_menu [INFERRED 0.80]
- **Vendor Loading Kernel-Integrity Safeguards** — plans_2026_09_22_p6b_vendor_filein_allowlist, plans_2026_09_22_p6b_vendor_filein_native_overwrite_refusal, plans_2026_09_22_p6b_vendor_filein_kernel_scan_narrowing, plans_2026_09_22_p6b_vendor_filein_host_patch [INFERRED 0.80]
- **aoimage Method Persistence Across Restart** — prs_p9_04_aoimage_restart, plans_2026_09_22_p7_aoimage_save_procedure, plans_2026_09_22_p7_aoimage_load_procedure, plans_2026_09_22_p7_aoimage_native_rebind_by_name, plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.85]
- **Vendor File-in to Image Snapshot Pipeline** — plans_2026_09_22_p6b_vendor_filein_load_order, plans_2026_09_22_p6b_vendor_filein_link_class, plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.80]
- **P8 Phase Tasks (must land before P9 begins)** — plans_2026_09_23_p8_p9_remaining_task_1_session_abi, plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, plans_2026_09_23_p8_p9_remaining_task_4_swift_link, plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — plans_2026_09_23_p8_p9_remaining_task_8_printstring, plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, plans_2026_09_23_p8_p9_remaining_task_10_accept, plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — plans_2026_09_23_review_fixes_stack_guard, plans_2026_09_23_review_fixes_b3_failure_propagation, plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **Vendor extraction, deferral list, and file-in test form one pipeline** — vendor_origin_extract_process, vendor_deferred_doc, runtime_tests_vendor_filein_test [INFERRED 0.80]
- **Message Dispatch and Class Hierarchy** — spec_message_send, spec_method_cache, spec_doesnotunderstand, spec_behavior_classdescription_class_metaclass, spec_kernel [EXTRACTED 0.90]
- **Cooperative Scheduling Flow** — spec_process_scheduler, spec_sharedqueue, spec_deadlock, spec_drain, spec_abandon [EXTRACTED 0.90]
- **Evaluation Interruption Model** — spec_nonlocal_return_ensure_abort, spec_halt, spec_exception_handling, spec_live_debugger [EXTRACTED 0.85]
- **Post-mortem debugging pipeline (frame chain + snapshot + window)** — phases_p10_callcontext_topframe, plans_2026_09_26_p10_debugger_capture_design, phases_p10_debugger_window_spec [INFERRED 0.85]
- **Live debugging operation flow (evaluating process + halt + operations)** — phases_p11_evaluating_process, phases_p11_halt_mechanism, phases_p11_operations [INFERRED 0.85]
- **Performance regression guardrail via docs/bench.md** — docs_bench_p11_step_branch, phases_p10_constraints, phases_p11_constraints [INFERRED 0.75]
- **P10 postmortem debugger data pipeline: pc-map → snapshot → ABI → window** — docs_prs_p10_02_methodimage, docs_prs_p10_03_debugsnapshot, docs_prs_p10_05_aoabih, app_ao_debuggerwindow [INFERRED 0.85]
- **P11 live debugger control flow: eval process → halt → resume/step** — docs_prs_p11_02_evalprocess, docs_prs_p11_03_stoporabort, docs_prs_p11_04_resumehalted, docs_prs_p11_05_stepabi [INFERRED 0.85]
- **Spec-first design refinement of the debugger across P10 and P11** — docs_prs_p10_01_spec313, docs_prs_p11_01_spec313live, docs_prs_p11_01_p11plan [INFERRED 0.75]
- **Live debugger stop-and-resume cycle** — spec_halt, spec_eval_process, spec_step_operations, spec_proceed_operation, spec_live_abort_operation [INFERRED 0.85]
- **Kernel hashed collection mechanics** — spec_dictionary_set_hashtable, spec_equality_hash_contract, spec_class_variables_classpool [INFERRED 0.75]
- **Debug info pipeline: compiler to ABI** — spec_debug_info_compiler, spec_pc_source_table, spec_statement_start_table, spec_source_not_in_image, spec_debug_read_abi [INFERRED 0.85]

## Communities (299 total, 96 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Float.cpp"
Cohesion: 0.06
Nodes (96): NumberOp, NumberRelation, NumKind, bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes() (+88 more)

### Community 2 - "TEST_F()"
Cohesion: 0.03
Nodes (79): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+71 more)

### Community 3 - ".fromSmallInteger()"
Cohesion: 0.08
Nodes (89): OcShape, Pass, safepoint, probe, visit, allocateInstance(), ao_Association_key(), ao_Association_key_value_() (+81 more)

### Community 4 - "Heap"
Cohesion: 0.03
Nodes (83): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+75 more)

### Community 5 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (70): Frame, context, depth, isBlock, method, pc, prev, receiver (+62 more)

### Community 6 - "TEST()"
Cohesion: 0.02
Nodes (84): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+76 more)

### Community 7 - "DebuggerWindowTests"
Cohesion: 0.07
Nodes (17): DebuggerWindowTests, Int, NSButton, NSFont, NSTableView, NSTextView, NSView, NSWindow (+9 more)

### Community 8 - "Compile.cpp"
Cohesion: 0.07
Nodes (81): cctype, FileInError, error, file, method, string, uint32_t, isClassShaped() (+73 more)

### Community 9 - "BrowserWindow"
Cohesion: 0.08
Nodes (26): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+18 more)

### Community 10 - "TEST()"
Cohesion: 0.03
Nodes (78): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+70 more)

### Community 11 - "DebuggerWindow"
Cohesion: 0.05
Nodes (47): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+39 more)

### Community 12 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 13 - "Oop"
Cohesion: 0.10
Nodes (65): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+57 more)

### Community 14 - "Parser"
Cohesion: 0.11
Nodes (22): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+14 more)

### Community 15 - "Stream.cpp"
Cohesion: 0.11
Nodes (63): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+55 more)

### Community 16 - "Boot"
Cohesion: 0.04
Nodes (64): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, BrokenParent, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message (+56 more)

### Community 17 - "Codegen.cpp"
Cohesion: 0.04
Nodes (60): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Analysis, declared (+52 more)

### Community 18 - "handlescope"
Cohesion: 0.12
Nodes (30): chrono, Chunk, cmath, compile, CompiledMethod, compiler, context, cstdio (+22 more)

### Community 19 - "Emitter"
Cohesion: 0.13
Nodes (14): Inline, int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+6 more)

### Community 20 - "P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt"
Cohesion: 0.06
Nodes (63): CLAUDE.md, docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, SPEC.md, Codegen.cpp: Emitter::mark と compile* 群 (+55 more)

### Community 21 - "send()"
Cohesion: 0.10
Nodes (60): run, InlineCache, cachedClass, cachedMethod, applyMethod(), ao_Array_equals(), ao_ArrayedCollection_do_(), ao_Collection_collect_() (+52 more)

### Community 22 - "TEST_F()"
Cohesion: 0.03
Nodes (61): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+53 more)

### Community 23 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 24 - "TEST()"
Cohesion: 0.04
Nodes (58): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+50 more)

### Community 25 - "abi.cpp"
Cohesion: 0.08
Nodes (52): atomic, Body, CountsAnswerMinusOneOnFailure, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at(), ao_browser_class_count(), ao_browser_class_definition() (+44 more)

### Community 26 - "string"
Cohesion: 0.08
Nodes (15): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+7 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (38): AnyObject, aoWorkspaceInspectHook(), failureText(), installDebugButton(), installErrorField(), keptEvalResult(), selectErrorSpan(), sendToKeyWorkspace() (+30 more)

### Community 28 - "Scheduler.cpp"
Cohesion: 0.08
Nodes (52): afterResume, canHalt, endEval, enqueue, find, haltedCount, leaveLists, reapDead (+44 more)

### Community 29 - "TEST_F()"
Cohesion: 0.04
Nodes (51): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+43 more)

### Community 30 - "TEST()"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 31 - "BrowserModel"
Cohesion: 0.11
Nodes (17): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, UnsafeMutablePointer (+9 more)

### Community 32 - "Bytecode interpreter"
Cohesion: 0.05
Nodes (51): P4-08: Point / Rectangle, Point/Rectangle 座標計算, P4-09: Kernel NativeMethod 走査と bench, Kernel NativeMethod 走査, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+43 more)

### Community 33 - "TEST()"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 34 - "TEST()"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 35 - "TEST()"
Cohesion: 0.04
Nodes (48): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery, DivisionByZeroWithFullNursery (+40 more)

### Community 36 - "vector"
Cohesion: 0.06
Nodes (39): vector, uint32_t, strictlyAscending(), csignal, future, mach, mach_vm, ABI boundary memory (+31 more)

### Community 37 - "AoSpan"
Cohesion: 0.06
Nodes (47): AoSpan, end, message, start, ao_accept_class(), ao_accept_method(), ao_debug_proceed(), ao_debug_step_into() (+39 more)

### Community 38 - "TEST()"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 39 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (46): cerrno, fcntl, encodeNonHeap, writeFiller, appendRaw(), appendRecord(), collectImageSlot(), byte (+38 more)

### Community 40 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+20 more)

### Community 41 - "Scheduler"
Cohesion: 0.04
Nodes (43): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+35 more)

### Community 42 - "CallContext"
Cohesion: 0.05
Nodes (42): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+34 more)

### Community 43 - "Session.cpp"
Cohesion: 0.09
Nodes (37): Image, check, load, save, boot(), shutdown(), attachBlocks(), blockLiteral() (+29 more)

### Community 44 - "TEST()"
Cohesion: 0.05
Nodes (41): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+33 more)

### Community 45 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 46 - "uint32_t"
Cohesion: 0.11
Nodes (29): argCountOf(), uint32_t, debugFrameCount(), debugFrameKind(), debugFrameLabel(), debugFramePc(), debugFrameSource(), debugFrameTotal() (+21 more)

### Community 47 - "DefinitionScanner"
Cohesion: 0.10
Nodes (18): DefinitionScanner, Any, Bool, Int, Int32, NSRect, NSScrollView, NSTableColumn (+10 more)

### Community 48 - "Heap.cpp"
Cohesion: 0.10
Nodes (36): charconv, fitsOld, growOld, oldCapacity, align8(), byte, size_t, uint16_t (+28 more)

### Community 49 - "TEST_F()"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 50 - "allocateRetry()"
Cohesion: 0.13
Nodes (37): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+29 more)

### Community 51 - "DebugSnapshot.cpp"
Cohesion: 0.13
Nodes (36): frameAt, contextPc(), CallContext, Frame, string, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context() (+28 more)

### Community 52 - "TEST()"
Cohesion: 0.09
Nodes (32): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+24 more)

### Community 53 - "takeAbortReason()"
Cohesion: 0.06
Nodes (37): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, GrowAndContentsWithFullNursery, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters (+29 more)

### Community 54 - "Ast"
Cohesion: 0.10
Nodes (26): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+18 more)

### Community 55 - ".isHeap()"
Cohesion: 0.12
Nodes (34): Gc::copy(), Heap::checkNotPoisoned(), Heap::inNursery(), Heap::inOld(), int64_t, findsKernelNestingHash(), sendHash(), ao_BlockContext_cannotReturn_() (+26 more)

### Community 56 - "Send.cpp"
Cohesion: 0.10
Nodes (25): algorithm, Bootstrap, iterator, lookup, MethodDictionary, namedSlotNames(), superclassChainFromRoot(), ClassMethodCache::addRoots() (+17 more)

### Community 57 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 58 - "TEST_F()"
Cohesion: 0.06
Nodes (31): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+23 more)

### Community 59 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+3 more)

### Community 60 - ".nil()"
Cohesion: 0.17
Nodes (31): flags, slotAt, slotAtPut, int64_t, addFiber, liveFibers, Gc::clearWeakAfterOldMark(), Roots (+23 more)

### Community 61 - "AoApp"
Cohesion: 0.12
Nodes (14): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+6 more)

### Community 62 - "CompileEnv"
Cohesion: 0.06
Nodes (29): BlockAssignmentIsBindingStore, Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings (+21 more)

### Community 63 - "FiberStack"
Cohesion: 0.10
Nodes (27): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+19 more)

### Community 64 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (31): hashedcollection, CallContext, int64_t, uint32_t, Root, Table, array, capacity (+23 more)

### Community 65 - "Task 12: Browser accept, Hierarchy, VoiceOver, C"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 66 - "P1 — オブジェクトメモリ"
Cohesion: 0.11
Nodes (31): P0 (親フェーズ, stub), ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P2 — ブートストラップ, WellKnown.hpp — well-known クラス表 (+23 more)

### Community 67 - "ao_main.cpp"
Cohesion: 0.10
Nodes (25): climits, dyld, fstream, runtime, addRoots, string, VendorClassFile, chunkText (+17 more)

### Community 68 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.09
Nodes (31): Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes, Link (vendor class) (+23 more)

### Community 69 - "DebugSnapshot"
Cohesion: 0.07
Nodes (27): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+19 more)

### Community 70 - "VendorExtract.cpp"
Cohesion: 0.19
Nodes (29): allowIndex(), containsHostWord(), string, string_view, doubleBangs(), extractVendor(), firstLineKey(), firstNonEmptyLine() (+21 more)

### Community 71 - "WellKnown.cpp"
Cohesion: 0.10
Nodes (28): findSymbol, global, internWith, Roots, string_view, ImageSelector, name, WellKnown (+20 more)

### Community 72 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 73 - "Session"
Cohesion: 0.07
Nodes (29): browserClassCount(), int64_t, debugCanProceed(), debugHaltedCount(), debugHaltedPid(), debugSelect(), CallContext, DebugSink (+21 more)

### Community 74 - "TEST()"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 75 - "TEST()"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 76 - "TEST()"
Cohesion: 0.08
Nodes (26): BlockWithArgs, Cascade, ParseResult, error, method, ok, parseMethod(), BlockArgumentsThenTemps (+18 more)

### Community 77 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 78 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, string, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+20 more)

### Community 79 - "ImageFormat"
Cohesion: 0.09
Nodes (26): bit, uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes (+18 more)

### Community 80 - "TEST()"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 81 - "TEST()"
Cohesion: 0.09
Nodes (27): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+19 more)

### Community 82 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.11
Nodes (27): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B5: Browser/Workspace data-loss fixes, B6: Image/session robustness (atomic save, load validation) (+19 more)

### Community 83 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 84 - ".base()"
Cohesion: 0.13
Nodes (24): abandonAll, awaitEval, drain, findId, isHalted, proceed, runAwaited, terminate (+16 more)

### Community 85 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (24): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+16 more)

### Community 86 - "classRows()"
Cohesion: 0.19
Nodes (25): browserClassAt(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource(), browserSubclassAt() (+17 more)

### Community 87 - "TEST_F()"
Cohesion: 0.09
Nodes (21): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+13 more)

### Community 88 - "TEST()"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 89 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (23): adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view, uint32_t (+15 more)

### Community 90 - "Ao (macOS Native Smalltalk System)"
Cohesion: 0.09
Nodes (24): Ao (macOS Native Smalltalk System), Ao.app (macOS Application Deliverable), Ao Bytecode (Modernized Blue Book Ch.28 Set), ao CLI, .aoimage Snapshot, AppKit (macOS host GUI), Smalltalk-80 Blue Book (Goldberg & Robson, 1983), SPEC change rules (§8) (+16 more)

### Community 91 - "LiveFrames"
Cohesion: 0.09
Nodes (20): CallContext, DebugSink, onAbort, Frame, size_t, string, LiveFrames, context (+12 more)

### Community 92 - "Gc"
Cohesion: 0.13
Nodes (23): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, collectNursery, collectOld, copy, heap_ (+15 more)

### Community 93 - "string"
Cohesion: 0.19
Nodes (22): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+14 more)

### Community 94 - "AppKit"
Cohesion: 0.13
Nodes (11): Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, transcriptBoxHook(), SmokeTests, AppKit (+3 more)

### Community 95 - "TEST()"
Cohesion: 0.10
Nodes (22): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, compileBinary() (+14 more)

### Community 96 - "DebugFrames"
Cohesion: 0.10
Nodes (19): DebugFrames, context, count, empty, kind, method, pc, receiver (+11 more)

### Community 97 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 98 - "NativeMethod.cpp"
Cohesion: 0.20
Nodes (20): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+12 more)

### Community 99 - "ao_runtime_shutdown()"
Cohesion: 0.10
Nodes (17): AbiSmoke, AoTranscriptFn, BootAndShutdownReturnZero, BootVersionShutdown, ao_runtime_shutdown(), ao_set_inspect_hook(), ao_set_transcript_hook(), AoInspectFn (+9 more)

### Community 100 - "TEST()"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 101 - "ImageLoad.cpp"
Cohesion: 0.19
Nodes (19): ifstream, bindAll(), checkGlobals(), byte, T, uint32_t, fileOop(), globalNamesOk() (+11 more)

### Community 102 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 103 - "uint64_t"
Cohesion: 0.25
Nodes (15): acceptWord(), uint64_t, unordered_map, unordered_set, headerAt(), heapShaped(), ObjectRules, behavior_ (+7 more)

### Community 104 - "evalBody()"
Cohesion: 0.18
Nodes (20): answerAwaited(), answerEval(), blankOut(), AoInspectFn, EvalEnd, optional, StepMode, DebugEntry (+12 more)

### Community 105 - "TEST()"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 106 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 107 - "Dictionary/Set/IdentityDictionary/IdentitySet op"
Cohesion: 0.12
Nodes (20): Boolean/True/False operation semantics, Character asCharacter validation, Collection enumeration semantics (select:/reject:/includes:/detect:ifNone:), Collections native-required category, copy / shallowCopy semantics, Dictionary/Set Blue Book protocol table, Dictionary/Set/IdentityDictionary/IdentitySet open-addressing hash table, = and hash contract (+12 more)

### Community 108 - "TEST()"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 109 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 110 - "MainMenu"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, Selector, String, Void

### Community 111 - "TEST()"
Cohesion: 0.13
Nodes (19): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, firstBlock(), named(), pcOf() (+11 more)

### Community 112 - "ImageSurgery"
Cohesion: 0.27
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 113 - "TEST()"
Cohesion: 0.11
Nodes (18): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+10 more)

### Community 114 - "checkFile()"
Cohesion: 0.29
Nodes (17): readHeader, atOffset(), checkFile(), Roots, size_t, string, string_view, WellKnown (+9 more)

### Community 115 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 116 - "Loaded"
Cohesion: 0.12
Nodes (17): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+9 more)

### Community 117 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 118 - "TEST()"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 119 - "TEST()"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 120 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 121 - "KeptClass"
Cohesion: 0.21
Nodes (17): size_t, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars, deferred (+9 more)

### Community 122 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 123 - "ChunkAction"
Cohesion: 0.12
Nodes (16): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+8 more)

### Community 124 - "ChunkParser.cpp"
Cohesion: 0.30
Nodes (15): classify(), string_view, firstLineHas(), isBlank(), isCharacterLiteral(), isLetter(), isProseApostrophe(), isSoleDefinition() (+7 more)

### Community 125 - "FiberRegs"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

### Community 126 - "MethodSource"
Cohesion: 0.13
Nodes (16): DebugInfoRef, body, index, source, string, uint32_t, uint64_t, MethodSource (+8 more)

### Community 127 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 128 - "Bootstrap procedure (9 ordered steps)"
Cohesion: 0.17
Nodes (16): basicNew: allocation rules, Bootstrap procedure (9 ordered steps), PushLitVar/StoreLitVar/PopStoreLitVar (37-39), Class and metaclass naming rules, Class variables / classPool mechanism, Global dictionary (Smalltalk/SmalltalkImage), Kernel class added-slot instVarNames table, Kernel-Classes category (Behavior/ClassDescription/Class/Metaclass) (+8 more)

### Community 129 - "TEST()"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 130 - "TEST()"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 131 - "TEST()"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 132 - "Deferred vendor methods list"
Cohesion: 0.15
Nodes (15): B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12) (+7 more)

### Community 133 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 134 - "clearUnwinding()"
Cohesion: 0.24
Nodes (14): refreshStackLimit(), abortReasonText(), clearUnwinding(), string, sessionDebugAbort(), CallContext, path, Root (+6 more)

### Community 135 - "NativeMethod"
Cohesion: 0.18
Nodes (15): P10 postmortem debugger acceptance criteria, Behavior / ClassDescription / Class / Metaclass Roles, BlockContext required protocol, CreateBlock opcode (31), CompiledMethod, Interpreted frame chain (CallContext::topFrame), Frame kind/label taxonomy (method/block/synthesized native), Terminology glossary (§7) (+7 more)

### Community 136 - "P10 scope (do/don't)"
Cohesion: 0.21
Nodes (14): Unreleased: P10/P11 Debugger, Abort-time snapshot capture, CallContext::topFrame frame chaining, P10 conclusion: post-mortem debugger, Debugger window spec (frames/source/variables), Object>>halt spec (reason halt), P10 scope (do/don't), Workspace Debug button / process failed: (+6 more)

### Community 137 - "P4-02: Behavior / ClassDescription / Class / Met"
Cohesion: 0.14
Nodes (14): P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード, P4-04: Array / ByteArray / String / Symbol (+6 more)

### Community 138 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 139 - ".isTrue()"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 140 - "Debugger snapshot read ABI (ao_debug_frame_*, ao"
Cohesion: 0.20
Nodes (14): v1 acceptance criteria (§6, language/tools/process), Failure capture mechanism (pre-unwind snapshot), CI pipeline (§4.5, GitHub Actions), Class definition re-Accept rules (shape change), Debugger snapshot read ABI (ao_debug_frame_*, ao_debug_reason), Golden evaluation tests (image/tests/*.st, ao --test, §4.4), .aoimage format (version 3), Image load validation checks (+6 more)

### Community 141 - "InspectorWindow"
Cohesion: 0.22
Nodes (8): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String

### Community 142 - "abortEvaluation / abortEvaluationQuiet"
Cohesion: 0.15
Nodes (13): ao_debug_* ABI design (P10-05), abortEvaluation / abortEvaluationQuiet, App window patterns (NSWindow, WorkspaceWindow, InspectorWindow), DebugSnapshot::capture design, Frame/Temps/OperandStack live on C++ stack, not heap, HostOopHook (transcript/inspect hooks), ao_Object_halt design (P10-03), performSend (+5 more)

### Community 143 - "C ABI bridge (bridge/ao_abi.h)"
Cohesion: 0.22
Nodes (13): ao_eval evaluation entry point, C ABI bridge (bridge/ao_abi.h), Chunk format (! terminated, methodsFor:), Class library incorporation policy (no self-authored library), Compiler §3.8 requirements (grammar, nesting limits, temp resolution), v1 constraints (§5, 12 rules), DEFERRED.md exclusion list format, Evaluation result overflow ABI (ao_eval_result_length/copy) (+5 more)

### Community 144 - "TEST()"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 145 - "Literal"
Cohesion: 0.17
Nodes (12): int64_t, LitKind, unique_ptr, Literal, bytes, elements, floatValue, intValue (+4 more)

### Community 146 - "uint32_t"
Cohesion: 0.17
Nodes (7): uint32_t, Rec, argCount, base, kind, pc, tempCount

### Community 147 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 148 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 149 - "P11 step branch bench"
Cohesion: 0.18
Nodes (11): P11 known limitations, v1 known limitations, B2 to:do: bench, SmallInteger fast path (no-send arithmetic), P11 step branch bench, P6 interpreter bench, P11 acceptance criteria, P11 constraints (one step flag, bench ratio, ABI signature frozen) (+3 more)

### Community 150 - "vendor_filein_test.cpp"
Cohesion: 0.22
Nodes (10): classpool, ao_runtime library target, ao_runtime_tests executable target, ao executable target, cli_filein / cli_image_save / cli_test ctest targets, gcstress ctest target (AO_GC_STRESS=1, excludes KernelBench.*), gcstress_vendor ctest target, googletest v1.15.2 FetchContent dependency (+2 more)

### Community 151 - "MethodImage"
Cohesion: 0.18
Nodes (11): uint16_t, MethodImage, bytes, literals, numArgs, numTemps, pcMap, primitive (+3 more)

### Community 152 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 153 - "Plan architecture: Frame chain + snapshot + pcMa"
Cohesion: 0.18
Nodes (11): P10 debugger plan reference, .aoimage format v3 (debug info excluded), Plan architecture: Frame chain + snapshot + pcMap, Emitter::mark / compileSend / compileReturn / compileInlined, Frame chain design (P10-03), Plan goal: capture stack before unwind, Graphify god nodes & communities for this work, compiler::MethodImage (+3 more)

### Community 154 - "TEST()"
Cohesion: 0.18
Nodes (9): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), TEST() (+1 more)

### Community 155 - "P3-04: lookup / super / doesNotUnderstand:"
Cohesion: 0.18
Nodes (11): P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary, P3-03: NativeMethod とセレクタマングル, NativeMethod とセレクタマングル規則, Smalltalk-80 Blue Book（探索意味論）, P3-04: lookup / super / doesNotUnderstand: (+3 more)

### Community 156 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 157 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext, uint32_t (+2 more)

### Community 158 - "Cooperative ProcessorScheduler / Process / Semap"
Cohesion: 0.20
Nodes (11): Abandon (Process Teardown Without Cleanup), Closures and Shared Temp Variables, MethodContext / Process Execution Model, Deadlock Handling, Evaluation Drain Mechanism, IdentityDictionary / IdentitySet (16-bit identity hash collisions), Non-local Return, ensure:, and abort, Object Memory and Generational GC (+3 more)

### Community 159 - "TEST()"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 160 - "Proceed/Abort/Step into,over,out operations"
Cohesion: 0.20
Nodes (10): ao_set_debug_capture / ao_debug_*, Ao.app Debugger window, Live debugger, ao_set_debug_mode(AO_DEBUG_LIVE) / AO_ERR_HALT, Object>>halt, Post-mortem debugger, ao_debug_* ABI busy rules, Debug it (⌘⇧D) (+2 more)

### Community 161 - "docs/README.md"
Cohesion: 0.22
Nodes (9): 1.0.0 release, CLAUDE.md (process source of truth), P0→P11 linear dependency, P11 plan reference (p11-ancient-matsumoto.md), Phase progression table, SPEC.md (product source of truth), v1 = P9 completion, released as 1.0.0, Plan scope (do/don't) for P10 (+1 more)

### Community 162 - "ao CMake Project"
Cohesion: 0.24
Nodes (10): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target), リリース手順 (JA), Releasing Process (EN), Repository Layout, Kernel Scan Test (via ctest) (+2 more)

### Community 163 - "Workspace window (Do it/Print it/Inspect it/Debu"
Cohesion: 0.33
Nodes (10): Compiler debug info emission, Debug it (AO_EVAL_DEBUGIT) semantics, Debug it menu command (⌘⇧D), Debugger window (postmortem, 3-pane), pc->source table, Statement-start table (for Step), app TDD suite (XCTest, §4.3), compiler TDD suite (§4.2, pc_map_test) (+2 more)

### Community 164 - "TempName"
Cohesion: 0.22
Nodes (9): int16_t, string, uint8_t, TempName, kind, name, slot, vecIndex (+1 more)

### Community 165 - "RawChunk"
Cohesion: 0.22
Nodes (9): string, Token, uint32_t, RawChunk, endsSection, span, text, undoubled (+1 more)

### Community 166 - "TEST()"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 167 - "P0-03: CMake + GoogleTest + CLI"
Cohesion: 0.25
Nodes (9): P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub, P0-06: Serena project (+1 more)

### Community 168 - "P2-03: メタクラス循環（Blue Book 6–10）"
Cohesion: 0.22
Nodes (9): P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10）, メタクラス循環, P2-04: Smalltalk グローバル辞書 (+1 more)

### Community 169 - "intern()"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 170 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 171 - "Live debugger runtime semantics (§3.13 P11)"
Cohesion: 0.44
Nodes (9): P11 live debugger acceptance criteria, Evaluation process (doIt runs on its own Process), Object>>halt, Live-debugger Abort operation (terminate + ensure:), Live debugger operation ABI (ao_debug_proceed/step_*/abort, ao_set_debug_mode), Live debugger runtime semantics (§3.13 P11), Live Debugger window (Proceed/Abort/Step buttons), Proceed operation (resume with nil result) (+1 more)

### Community 172 - "TEST()"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 173 - "Gc.cpp"
Cohesion: 0.36
Nodes (6): cstdlib, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::scavengeFromRoots()

### Community 174 - "P10-01..P10-07 PR list"
Cohesion: 0.29
Nodes (8): P4 microbench, PR table, P10 acceptance criteria, P10 constraints (no interpreter/native coupling, bench ratio), P10-01..P10-07 PR list, P10 TDD test files, P11-01..P11-07 PR list, PR sequence & dependencies (01→02→04→05→06→07)

### Community 175 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 176 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 177 - "ChunkMethod"
Cohesion: 0.29
Nodes (7): ChunkMethod, source, span, undoubled, string, uint32_t, fileSpan()

### Community 178 - "Heap::allocate()"
Cohesion: 0.33
Nodes (7): allocateTenured, initObject, largeObjectBytes, objectBytes, reserveOld, Heap::allocate(), Heap::allocateTenured()

### Community 179 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 180 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 181 - "Graphify Required Tool"
Cohesion: 0.40
Nodes (5): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps), Ao Smalltalk Overview (EN), Ao Smalltalk 概要 (JA)

### Community 182 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.33
Nodes (6): docs/bench.md, PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 183 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 184 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 185 - "TEST()"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 186 - "PcSpan"
Cohesion: 0.40
Nodes (5): uint32_t, PcSpan, end, pc, start

### Community 187 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 188 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 189 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 190 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 191 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 192 - "TEST()"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 193 - "TEST()"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 194 - "NativeMethod.hpp"
Cohesion: 0.50
Nodes (3): DebugSink, Frame, Scheduler

### Community 195 - "Roots::visitAll()"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 196 - "abortingSubclass()"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 197 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 198 - "imageRegistryStubA()"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 199 - "answerOne()"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 200 - "Rules That Do Not Bend"
Cohesion: 0.67
Nodes (3): Fixed Design Decisions, 曲げない規則, Rules That Do Not Bend

### Community 201 - "Session::MethodSource table"
Cohesion: 0.67
Nodes (3): evalBody / sessionEval, Session::MethodSource table, Source table extension design (P10-04)

### Community 204 - "abortingNew()"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Ambiguous Edges - Review These
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/prs/P4-09.md · relation: references
- `P4-09: Kernel NativeMethod 走査と bench` → `P5-01: 字句解析`  [AMBIGUOUS]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/prs/P5-01.md · relation: references
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/prs/P7-01.md · relation: references
- `P7-03: load と NativeMethod 再結合` → `P8-01: Ao.app 骨格`  [AMBIGUOUS]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/prs/P8-01.md · relation: references
- `P8-05: メニューとキー` → `P9-01: Do it / Print it / Inspect it`  [AMBIGUOUS]
  /Users/hiro/Projetct/GitHub/ao-smalltalk/docs/prs/P9-01.md · relation: references
- `SPEC §3.13 デバッガ（捕捉の意味論）` → `Debug it (AO_EVAL_DEBUGIT)`  [AMBIGUOUS]
  docs/prs/P11-05.md · relation: conceptually_related_to
- `Compile.cpp: acceptMethodSource` → `Image::save(..., haltedProcesses) 保存の拒否`  [AMBIGUOUS]
  docs/prs/P11-03.md · relation: conceptually_related_to
- `Failure capture mechanism (pre-unwind snapshot)` → `Live-debugger Abort operation (terminate + ensure:)`  [AMBIGUOUS]
  SPEC.md · relation: semantically_similar_to

## Knowledge Gaps
- **949 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+944 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2506 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **96 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

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
- **What is the exact relationship between `SPEC §3.13 デバッガ（捕捉の意味論）` and `Debug it (AO_EVAL_DEBUGIT)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `Compile.cpp: acceptMethodSource` and `Image::save(..., haltedProcesses) 保存の拒否`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
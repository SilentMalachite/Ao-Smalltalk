# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 295 files · ~302,229 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 5924 nodes · 15368 edges · 311 communities (215 shown, 96 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 1834 edges (avg confidence: 0.82)
- Token cost: 174,620 tokens (subagent total; input/output split not reported)

## Community Hubs (Navigation)
- WellKnown
- Boot
- Codegen.cpp
- NativeMethod.cpp
- MainMenu
- BrowserModelTests
- CompileEnv
- ImageSurgery
- TEST()
- FileInError
- MethodDebugInfo
- LargeInteger.cpp
- FiberRegs
- VirtualRegion.cpp
- KeptClass
- CountingSink
- Loaded
- CompileResult
- Oop
- sessionImageLoad()
- Stack
- image_save_load_test.cpp
- RootedArray
- Scanner
- InspectorWindow
- Literal
- uint32_t
- FrameBlock
- MethodImage
- Token
- Heap
- ClassMethodCache
- DebuggerWindowTests
- TempName
- VendorClassFile
- string
- StringDoProbe
- Counts
- BlockProbe
- Parsed
- SelectedFrames
- WellKnown::InternTable
- BrowserWindow
- FileSizeLimit
- CarriedMethod
- TEST_F()
- Emitter
- KeptMethod
- TestDir
- ScopedGcStressEnv
- IgnoreFileSizeSignal
- Root
- HashNesting
- WorkspaceWindow
- Scheduler
- abi.cpp
- vector
- Session
- TEST()
- .fromSmallInteger()
- DebugFrames
- TranscriptWindow
- CallContext
- .nil()
- Ast
- ImageSave.cpp
- TEST_F()
- AoSpan
- TEST()
- ImageLoad.cpp
- Interpreter.cpp
- Send.cpp
- String.cpp
- Session.cpp
- Gc
- ToolWindowTests
- AoApp
- HashedCollection.cpp
- TEST_F()
- Array.cpp
- BrowserModel
- DebugSnapshot
- string
- VendorExtract.cpp
- DebuggerWindow
- TEST()
- DiskHeader
- Roots
- WellKnown.cpp
- Fiber.cpp
- TEST()
- TEST_F()
- fiber_test.cpp
- ImageFormat
- Scheduler::Record
- ChunkParser.cpp
- Bootstrap.cpp
- evalBody()
- AppKit
- TEST_F()
- FiberStack
- uint64_t
- ChunkAction
- LiveFrames
- DefinitionScanner
- WorkspaceEvalTests
- Process.cpp
- Roots.cpp
- TEST()
- TEST()
- TEST()
- TEST()
- .publish()
- .init()
- stream_test.cpp
- .isHeap()
- allocateRetry()
- string
- native_method_test.cpp
- TEST()
- TEST()
- TEST()
- TEST()
- kernel_numeric_test.cpp
- TEST()
- .isTrue()
- TEST()
- TEST()
- TEST()
- Stream.cpp
- path
- TEST()
- exactFloat()
- CompiledMethodNatives.cpp
- TEST()
- specialIndex()
- contextPc()
- clearUnwinding()
- intern()
- TEST()
- named()
- fillFullTable()
- Scheduler::runFiber()
- Runtime.cpp
- popFrame
- Scheduler::resume()
- TEST()
- TEST()
- Roots::visitAll()
- imageRegistryStubA()
- answerOne()
- .specialSelector()
- abortingNew()
- TEST()
- uint64_t
- Scheduler.cpp
- TEST()
- TEST()
- TEST()
- Heap.cpp
- Float.cpp
- .false_()
- TEST()
- TEST()
- TEST_F()
- putNative()
- Compile.cpp
- SmallInteger.cpp
- send()
- DebugSnapshot.cpp
- Geometry.cpp
- TEST()
- Boolean.cpp
- TEST()
- BlockContext.cpp
- TEST()
- ClassPool.cpp
- TEST()
- TEST()
- Scanner.cpp
- P10 scope (do/don't)
- abortEvaluation / abortEvaluationQuiet
- P11 step branch bench
- Plan architecture: Frame chain + snapshot + pcMap
- Proceed/Abort/Step into,over,out operations
- docs/README.md
- P10-01..P10-07 PR list
- Session::MethodSource table
- cli_test.sh
- test.sh
- build.sh
- Package.swift
- package-app.sh
- ao_runtime library target
- P4-02: Behavior / ClassDescription / Class / Metaclass
- ao CMake Project
- P3-04: lookup / super / doesNotUnderstand:
- Method Removal Rules
- P0-03: CMake + GoogleTest + CLI
- P2-03: メタクラス循環（Blue Book 6–10）
- Graphify Required Tool
- P8–P9 Remaining Implementation Plan
- P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt
- Rules That Do Not Bend
- Native Method Symbol Naming Convention
- ao_compiler library target
- ClassMethodCache の無効化が定義クラスの分だけ
- out == NULL の Do it が副作用ありで AO_ERR を返す
- =は値で比較するのにhashは同一性ハッシュのままで=/hashの契約を破る
- int64を超える整数リテラルが黙って0になる
- Dictionary/Setがhashを捨てて線形探索し要素数の2乗で遅くなる
- Kernelクラスにinstance変数名が無くinstVarNamed:等が失敗する(既報関連)
- v1 に無いもの
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
- Bytecode interpreter
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
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- P1 — オブジェクトメモリ
- P6b Vendor File-in Implementation Plan
- Claude Review Fixes Plan (2026-09-23)
- P12 Browser Removal Design
- Global Dictionary (Smalltalk)
- Compiler (.st to CompiledMethod)
- C ABI (bridge/ao_abi.h)
- Failure Aborts Evaluation (abortEvaluation)
- Cooperative Process Scheduler
- Class Definition Re-Accept
- Imported Class Library (no self-authored library)
- Live Debugger (evaluation process, halting)
- Kernel Classes (NativeMethod required)
- WriteStream String Writes (reserve capacity)
- Closures and Shared Temps (temp vector)
- Interval Semantics
- v1 Non-Goals
- OrderedCollection Layout
- Repository Layout (runtime/compiler/image/app/bridge)

## God Nodes (most connected - your core abstractions)
1. `Oop` - 937 edges
2. `Heap` - 252 edges
3. `vector` - 176 edges
4. `WellKnown` - 167 edges
5. `TEST_F()` - 111 edges
6. `TEST()` - 107 edges
7. `Ast` - 103 edges
8. `Boot` - 101 edges
9. `TEST()` - 101 edges
10. `Session` - 100 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `Linear MethodDictionary (interleaved keys/values)` --semantically_similar_to--> `Global Dictionary (Smalltalk)`  [INFERRED] [semantically similar]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md → SPEC.md
- `Adopted: two C ABI removal functions` --semantically_similar_to--> `ao_accept_method`  [INFERRED] [semantically similar]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md → SPEC.md
- `Host Word Patch (ao-host-patch)` --implements--> `extractVendor()`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp
- `LOAD_ORDER Dependency Sequencing` --implements--> `extractVendor()`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Evaluation Interruption Model** — spec_live_debugger [EXTRACTED 0.85]
- **Cooperative Scheduling Flow** — spec_deadlock, spec_drain, spec_abandon [EXTRACTED 0.90]
- **Message Dispatch and Class Hierarchy** — spec_message_send, spec_method_cache [EXTRACTED 0.90]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — docs_claude_review_00_recent_diff_classdef_string_silently_dropped, docs_claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, docs_claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, docs_claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **Kernel hashed collection mechanics** — spec_equality_hash_contract [INFERRED 0.75]
- **Spec-first design refinement of the debugger across P10 and P11** — docs_prs_p10_01_spec313, docs_prs_p11_01_spec313live, docs_prs_p11_01_p11plan [INFERRED 0.75]
- **Performance regression guardrail via docs/bench.md** — docs_bench_p11_step_branch, phases_p10_constraints, phases_p11_constraints [INFERRED 0.75]
- **Vendor Loading Kernel-Integrity Safeguards** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_allowlist, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_native_overwrite_refusal, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_kernel_scan_narrowing, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_host_patch [INFERRED 0.80]
- **Vendor File-in to Image Snapshot Pipeline** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_load_order, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_link_class, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.80]
- **AppKit ツールウィンドウ群とメニュー** — docs_prs_p8_01_ao_app_skeleton, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [INFERRED 0.80]
- **Vendor extraction, deferral list, and file-in test form one pipeline** — image_vendor_origin_extract_process, image_vendor_deferred_doc, runtime_tests_vendor_filein_test [INFERRED 0.80]
- **aoimage Method Persistence Across Restart** — docs_prs_p9_04_aoimage_restart, docs_superpowers_plans_2026_09_22_p7_aoimage_save_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_load_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_native_rebind_by_name, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.85]
- **Debug info pipeline: compiler to ABI** — spec_statement_start_table [INFERRED 0.85]
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
- **Method removal pipeline: dictionary entry removal, cache invalidation, source table drop** — spec_ao_remove_method, docs_prs_p3_02_methoddictionary, spec_cache_invalidation, spec_source_table, specs_2026_09_26_browser_remove_design_invalidate_method_cache [EXTRACTED 1.00]
- **Class removal refusal checks: fixed globals, kernel identity, live subclasses** — spec_ao_remove_class, spec_fixed_globals, spec_ao_accept_method, spec_live_subclass, specs_2026_09_26_browser_remove_design_has_subclass [EXTRACTED 1.00]
- **Debugger stack: capture, frame labels, readout ABI, window, live stepping** — spec_debug_capture, spec_frame_kinds_labels, spec_debugger_readout_abi, spec_debugger_window, spec_live_debugger, spec_step_semantics [INFERRED 0.85]

## Communities (311 total, 96 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): WellKnown, InternTable, Roots, unique_ptr, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Boot"
Cohesion: 0.03
Nodes (109): CompileError, Boot, abortingSubclass(), countingPrintString(), evalExpr(), expectAbortedEmpty(), expectNonBooleanAbort(), expectSymbol() (+101 more)

### Community 10 - "Codegen.cpp"
Cohesion: 0.05
Nodes (61): Analysis, Analyzer, Capture, LexScope, RealScope, TempVector, Var, codegen() (+53 more)

### Community 101 - "NativeMethod.cpp"
Cohesion: 0.20
Nodes (20): NativeFrame, add(), addNamed(), apply(), create(), findName(), functionOf(), invoke() (+12 more)

### Community 104 - "MainMenu"
Cohesion: 0.22
Nodes (9): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, Selector, String, Void (+1 more)

### Community 105 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 107 - "CompileEnv"
Cohesion: 0.10
Nodes (20): CompileEnv, bindingLiterals(), TEST(), workspaceEnv(), BlockAssignmentIsBindingStore, classVarNames, instVarNames, kernelInstVarCount (+12 more)

### Community 109 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): ImageSurgery, methodDictKey(), oopWords(), wellKnownBitsAt(), size_t, string_view, uint64_t, bytes (+2 more)

### Community 114 - "TEST()"
Cohesion: 0.11
Nodes (16): LoadOrderDir, TEST(), DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails (+8 more)

### Community 116 - "FileInError"
Cohesion: 0.20
Nodes (19): FileInError, isVendorStub(), acceptClassDef(), addError(), applyChunkActions(), applyClassDef(), applyMethodsFor(), fileInListedFile() (+11 more)

### Community 119 - "MethodDebugInfo"
Cohesion: 0.12
Nodes (18): PcSpan, DebugInfoRef, MethodDebugInfo, debugSpanAt(), p10ExpectSpansWithin(), p10HasTemp(), uint32_t, end (+10 more)

### Community 12 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Big, add(), addBig(), asInt64IfFits(), bigOf(), bitAnd(), bitOp(), bitOr() (+62 more)

### Community 121 - "FiberRegs"
Cohesion: 0.11
Nodes (18): FiberRegs, PingPong, fiberInit(), d, fp, lr, sp, x (+10 more)

### Community 125 - "VirtualRegion.cpp"
Cohesion: 0.21
Nodes (12): VirtualRegion, pageBytes(), roundUp(), reserve, commit, release, size_t, byte (+4 more)

### Community 126 - "KeptClass"
Cohesion: 0.21
Nodes (17): KeptClass, dropCycles(), dropMissingSupers(), findActive(), loadOrder(), noteMissingSuper(), reachesOwnSuper(), size_t (+9 more)

### Community 127 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, TestSink, throwingTranscript(), pinnedAfter, slotsAfter, slotsBefore, snap, CallContext (+5 more)

### Community 128 - "Loaded"
Cohesion: 0.13
Nodes (16): Loaded, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), nativeRequiredDictsAreNative(), runSource(), send0(), send1() (+8 more)

### Community 129 - "CompileResult"
Cohesion: 0.13
Nodes (12): CompileResult, ParseResult, compileMethod(), parseMethod(), Codegen, error, image, ok (+4 more)

### Community 13 - "Oop"
Cohesion: 0.09
Nodes (65): Oop, bits(), instSize(), isBytes(), isIndexable(), isPointers(), make(), ao_Object_basicAt_() (+57 more)

### Community 135 - "sessionImageLoad()"
Cohesion: 0.16
Nodes (15): Image, clearMethodSources(), ensureKernelNatives(), installEmptyCache(), installEmptyWorkspace(), Session::Session(), sessionBoot(), sessionImageLoad() (+7 more)

### Community 136 - "Stack"
Cohesion: 0.14
Nodes (13): Stack, Roots::attachStack(), Roots::switchStack(), attached, frameBase_, frameBlock_, frameBlocks_, frameCap_ (+5 more)

### Community 14 - "image_save_load_test.cpp"
Cohesion: 0.10
Nodes (33): Interpreter, p10SlotListed(), Bootstrap, Chunk, CompiledMethod, MethodDictionary, Symbol, ABI boundary memory (+25 more)

### Community 140 - "RootedArray"
Cohesion: 0.16
Nodes (11): Root, RootedArray, Roots, uint32_t, unique_ptr, slot, data_, inline_ (+3 more)

### Community 145 - "Scanner"
Cohesion: 0.23
Nodes (13): Scanner, Scanner::next(), uint32_t, i_, lexBinary, lexCharacter, lexIdentOrKeyword, lexNumber (+5 more)

### Community 149 - "InspectorWindow"
Cohesion: 0.24
Nodes (8): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String

### Community 150 - "Literal"
Cohesion: 0.17
Nodes (12): Literal, Literal::Literal(), int64_t, LitKind, unique_ptr, bytes, elements, floatValue (+4 more)

### Community 151 - "uint32_t"
Cohesion: 0.17
Nodes (7): Rec, uint32_t, argCount, base, kind, pc, tempCount

### Community 152 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, Range, Roots::popRange(), capacity, slots, used, size_t, unique_ptr (+2 more)

### Community 157 - "MethodImage"
Cohesion: 0.18
Nodes (11): MethodImage, uint16_t, bytes, literals, numArgs, numTemps, pcMap, primitive (+3 more)

### Community 158 - "Token"
Cohesion: 0.18
Nodes (11): Token, int64_t, string, Tok, intValue, isFloat, kind, largeInt (+3 more)

### Community 16 - "Heap"
Cohesion: 0.04
Nodes (57): Heap, ObjectHeader, VirtualRegion, SuperclassWalk, Gc::Gc(), Heap::header(), lookup(), at() (+49 more)

### Community 161 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, Entry, entries, flushAll, flushSelector, insert, kSize, klass (+3 more)

### Community 17 - "DebuggerWindowTests"
Cohesion: 0.10
Nodes (13): DebuggerWindowTests, TranscriptBox, Int, NSButton, NSFont, NSTableView, NSTextView, NSView (+5 more)

### Community 172 - "TempName"
Cohesion: 0.22
Nodes (9): TempName, int16_t, string, uint8_t, kind, name, slot, vecIndex (+1 more)

### Community 175 - "VendorClassFile"
Cohesion: 0.25
Nodes (9): VendorClassFile, VendorExtractResult, string, chunkText, className, superName, unsupportedShape, files (+1 more)

### Community 18 - "string"
Cohesion: 0.08
Nodes (18): CallContext, CallContext, CallContext, stubA(), stubB(), string, string_view, NativeMethod (+10 more)

### Community 181 - "StringDoProbe"
Cohesion: 0.25
Nodes (8): StringDoProbe, mt19937, int64_t, Root, calls, rng, strangers, walked

### Community 182 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, Roots::counts(), attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots

### Community 183 - "BlockProbe"
Cohesion: 0.29
Nodes (5): BlockProbe, b, probe, int64_t, Root

### Community 186 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 187 - "SelectedFrames"
Cohesion: 0.29
Nodes (5): SelectedFrames, debugReason(), frames_, live_, none_

### Community 188 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): WellKnown::InternTable, deque, size_t, string, unordered_map, byBytes, table

### Community 19 - "BrowserWindow"
Cohesion: 0.12
Nodes (18): BrowserWindow, AcceptTests, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title (+10 more)

### Community 193 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): FileSizeLimit, rlim_t, oldAction_, oldLimit_

### Community 199 - "CarriedMethod"
Cohesion: 0.40
Nodes (5): CarriedMethod, image, meta, selector, source

### Community 2 - "TEST_F()"
Cohesion: 0.03
Nodes (81): DebugAbi, LiveDebug, LiveProceed, LiveStep, Printed, ProceedFromHook, Reentry, Source (+73 more)

### Community 20 - "Emitter"
Cohesion: 0.14
Nodes (11): Emitter, hasReceiverChild(), nameIn(), int16_t, Op, size_t, string_view, uint16_t (+3 more)

### Community 200 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 201 - "TestDir"
Cohesion: 0.40
Nodes (3): TestDir, path, path

### Community 202 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): ScopedGcStressEnv, optional, string, saved_

### Community 207 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 208 - "Root"
Cohesion: 0.67
Nodes (3): Root, Roots, slot

### Community 23 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (37): WorkspaceButtonAction, WorkspaceWindow, aoWorkspaceInspectHook(), failureText(), installDebugButton(), installErrorField(), keptEvalResult(), selectErrorSpan() (+29 more)

### Community 24 - "Scheduler"
Cohesion: 0.04
Nodes (53): Scheduler, Scheduler::canHalt(), Scheduler::terminateAll(), CallContext, EvalEnd, Record, size_t, string (+45 more)

### Community 25 - "abi.cpp"
Cohesion: 0.08
Nodes (49): AbiEntry, ao_browser_class_at(), ao_browser_class_count(), ao_browser_class_definition(), ao_browser_protocol_at(), ao_browser_protocol_count(), ao_browser_selector_at(), ao_browser_selector_count() (+41 more)

### Community 26 - "vector"
Cohesion: 0.06
Nodes (44): MethodImage, DebugInfo, Check, strictlyAscending(), bootAndRunTests(), imageUsage(), main(), printFileInErrors() (+36 more)

### Community 27 - "Session"
Cohesion: 0.06
Nodes (52): MethodSource, Session, attachBlocks(), blockLiteral(), debugCanProceed(), debugHaltedCount(), debugHaltedPid(), debugSelect() (+44 more)

### Community 29 - "TEST()"
Cohesion: 0.06
Nodes (49): Insn, classVarEnv(), classVarLiterals(), countOp(), decode(), firstBlock(), hasBackwardJump(), hasConditionalJump() (+41 more)

### Community 3 - ".fromSmallInteger()"
Cohesion: 0.08
Nodes (88): OcSlots, Probe, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size() (+80 more)

### Community 34 - "DebugFrames"
Cohesion: 0.08
Nodes (35): DebugFrames, NoFrames, argCountOf(), debugFrameCount(), debugFrameKind(), debugFrameLabel(), debugFramePc(), debugFrameTotal() (+27 more)

### Community 35 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): ToolTextSize, TranscriptWindow, UniformFont, aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool (+20 more)

### Community 36 - "CallContext"
Cohesion: 0.05
Nodes (45): CallContext, DebugSink, Frame, Scheduler, BindingHook, abandoning, aborting, abortReason (+37 more)

### Community 38 - ".nil()"
Cohesion: 0.13
Nodes (42): NoHalt, Gc::clearWeakAfterOldMark(), RootedArray::RootedArray(), ensureOc(), fail(), hasSlots(), isNoArgBlock(), isProcess() (+34 more)

### Community 4 - "Ast"
Cohesion: 0.08
Nodes (41): Ast, SourceSpan, Parser, join(), argc, floatValue, intValue, isFloat (+33 more)

### Community 40 - "ImageSave.cpp"
Cohesion: 0.10
Nodes (42): NameCollect, NamedOop, Trace, appendRaw(), appendRecord(), collectImageSlot(), directoryOf(), encodeOop() (+34 more)

### Community 46 - "TEST_F()"
Cohesion: 0.05
Nodes (37): HashedCollection, classDefinition(), randomProbeSource(), TEST_F(), ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool (+29 more)

### Community 47 - "AoSpan"
Cohesion: 0.08
Nodes (38): AoSpan, Reentry, ao_accept_class(), ao_accept_method(), ao_debug_step_into(), ao_debug_step_out(), ao_debug_step_over(), ao_eval() (+30 more)

### Community 48 - "TEST()"
Cohesion: 0.05
Nodes (34): BrowserAbi, KernelNumeric, ao_runtime_shutdown(), TEST(), TEST(), AbiSmoke, ArrayPrintsElementPrintStrings, BootAndShutdownReturnZero (+26 more)

### Community 49 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (35): ImageRecord, acceptWord(), atOffset(), checkFile(), checkGlobals(), fail(), findRecord(), globalNamesOk() (+27 more)

### Community 5 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (71): Frame, OperandStack, Temps, ActiveGuard, ContextExitGuard, DepthGuard, FieldRoots, FrameLink (+63 more)

### Community 50 - "Send.cpp"
Cohesion: 0.13
Nodes (34): InlineCache, IcGuard, Scheduler::afterResume(), abortDoesNotUnderstand(), abortEvaluation(), abortEvaluationQuiet(), abortFailedSend(), abortReasonText() (+26 more)

### Community 51 - "String.cpp"
Cohesion: 0.16
Nodes (33): Utf8Step, ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString() (+25 more)

### Community 53 - "Session.cpp"
Cohesion: 0.15
Nodes (31): ListedMethod, browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+23 more)

### Community 54 - "Gc"
Cohesion: 0.08
Nodes (35): Gc, Gc::collectBeforeTenured(), Gc::collectNursery(), Gc::safepoint(), Gc::stressPoint(), forceNursery(), forceOld(), forceSlide() (+27 more)

### Community 56 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): LaunchSet, ToolWindowTests, fileInVendor(), NSFont, NSMenu, NSMenuItem, NSTextView, NSView (+3 more)

### Community 57 - "AoApp"
Cohesion: 0.12
Nodes (14): AoApp, openImageFile(), saveImageFile(), vendorDirectory(), Bool, Int32, MainActor, Notification (+6 more)

### Community 59 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (31): CallContext, Root, Table, bumpGeneration(), capacityFor(), copyEntry(), freeEntry(), grow() (+23 more)

### Community 6 - "TEST_F()"
Cohesion: 0.03
Nodes (77): BusyRead, SessionAbi, ao_eval_result_copy(), ao_eval_result_length(), ao_set_debug_mode(), ao_set_inspect_hook(), ao_set_transcript_hook(), readResultAndReenter() (+69 more)

### Community 61 - "Array.cpp"
Cohesion: 0.14
Nodes (30): SavedHashNesting, bytesValueHash(), valueHashBytes(), valueHashFold(), valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString() (+22 more)

### Community 62 - "BrowserModel"
Cohesion: 0.21
Nodes (8): BrowserModel, ListedClass, .metaFlag, Bool, CChar, Int32, String, UnsafeMutablePointer

### Community 65 - "DebugSnapshot"
Cohesion: 0.07
Nodes (27): DebugSnapshot, Roots, sessionDebugSnapshot(), capture, clear, context, frames_, held_ (+19 more)

### Community 68 - "string"
Cohesion: 0.16
Nodes (29): ClassRow, MethodName, NameBag, byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals() (+21 more)

### Community 69 - "VendorExtract.cpp"
Cohesion: 0.19
Nodes (29): HostMethod, MethodParts, allowIndex(), containsHostWord(), doubleBangs(), extractVendor(), firstLineKey(), firstNonEmptyLine() (+21 more)

### Community 7 - "DebuggerWindow"
Cohesion: 0.05
Nodes (47): DebugFrame, DebuggerButtonActions, DebuggerWindow, DebugOutcome, aoDebuggerInspectHook(), decodeDebugText(), readDebugFrames(), readDebugText() (+39 more)

### Community 72 - "TEST()"
Cohesion: 0.08
Nodes (24): Dbg, smi(), TEST(), runOnSmallStack(), AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot (+16 more)

### Community 73 - "DiskHeader"
Cohesion: 0.08
Nodes (27): DiskHeader, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader(), ImageFormat::writeFiller(), ImageFormat::writeHeader(), byte, size_t (+19 more)

### Community 74 - "Roots"
Cohesion: 0.07
Nodes (28): Roots, StackWalker, uint8_t, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 75 - "WellKnown.cpp"
Cohesion: 0.10
Nodes (27): ImageSelector, NamedClass, WellKnown::addRoots(), WellKnown::bindImageSlot(), WellKnown::classOf(), WellKnown::eachClass(), WellKnown::eachImageSlot(), WellKnown::findSymbol() (+19 more)

### Community 78 - "Fiber.cpp"
Cohesion: 0.11
Nodes (25): FiberStackBounds, StackPool, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch(), fiberSwitch(), fiberSwitchFinal(), pageBytes() (+17 more)

### Community 8 - "TEST()"
Cohesion: 0.04
Nodes (76): BrokenCase, GarbageFirstBoot, callNative(), defineBrokenClass(), defineEmptyClass(), defineWithSuperclassSlot(), doubleIt(), expectErrorWithFullNursery() (+68 more)

### Community 80 - "TEST_F()"
Cohesion: 0.08
Nodes (21): FailureAbort, TEST_F(), CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+13 more)

### Community 81 - "fiber_test.cpp"
Cohesion: 0.10
Nodes (25): Deep, RegisterSwap, deepFiber(), pingPongFiber(), recurse(), registerFiber(), step(), switchWithRegisters() (+17 more)

### Community 82 - "ImageFormat"
Cohesion: 0.08
Nodes (26): ImageFormat, ImageHeader, TEST(), HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t (+18 more)

### Community 83 - "Scheduler::Record"
Cohesion: 0.08
Nodes (26): Scheduler::Record, Scheduler, unique_ptr, abandon, awaitingTerminate, ctx, deadlockPending, evalMode (+18 more)

### Community 84 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): RawChunk, classify(), firstLineHas(), isBlank(), isCharacterLiteral(), isLetter(), isProseApostrophe(), isSoleDefinition() (+16 more)

### Community 85 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (24): ClassDef, SlotNames, allocateSkeletons(), allocClass(), ensureMethodDict(), installNatives(), internHotSelectors(), makeSlotNames() (+16 more)

### Community 86 - "evalBody()"
Cohesion: 0.16
Nodes (23): DebugEntry, refreshStackLimit(), answerAwaited(), answerEval(), blankOut(), debugInspect(), debugPrint(), debugReceiverPrint() (+15 more)

### Community 88 - "AppKit"
Cohesion: 0.12
Nodes (11): SmokeTests, transcriptBoxHook(), Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, AppKit (+3 more)

### Community 89 - "TEST_F()"
Cohesion: 0.09
Nodes (21): Streams, TEST_F(), AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection (+13 more)

### Community 90 - "FiberStack"
Cohesion: 0.14
Nodes (18): FiberStack, clearShadow(), protectionAt(), TEST(), unmap, release, DeepRecursionOnFiberStack, Fiber (+10 more)

### Community 91 - "uint64_t"
Cohesion: 0.23
Nodes (16): ObjectRules, bindAll(), fileOop(), headerAt(), heapShaped(), pointerObject(), relocate(), relocateWord() (+8 more)

### Community 93 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkAction, ChunkMethod, fileSpan(), ChunkKind, category, className, classVars, instVars (+15 more)

### Community 95 - "LiveFrames"
Cohesion: 0.09
Nodes (20): CallContext, DebugSink, Frame, LiveFrames, onAbort, size_t, string, context (+12 more)

### Community 96 - "DefinitionScanner"
Cohesion: 0.22
Nodes (10): DefinitionScanner, Token, spanMessage(), Bool, Int32, String, keyword, other (+2 more)

### Community 97 - "WorkspaceEvalTests"
Cohesion: 0.17
Nodes (4): WorkspaceEvalTests, NSTextView, NSView, String

### Community 100 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 103 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt(), Roots::pinRange(), Roots::pushHandle(), Roots::pushRange() (+11 more)

### Community 106 - "TEST()"
Cohesion: 0.10
Nodes (20): TEST(), AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc (+12 more)

### Community 108 - "TEST()"
Cohesion: 0.10
Nodes (20): TEST(), BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf (+12 more)

### Community 11 - "TEST()"
Cohesion: 0.03
Nodes (74): TEST(), AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort (+66 more)

### Community 111 - "TEST()"
Cohesion: 0.11
Nodes (19): TEST(), ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef (+11 more)

### Community 112 - ".publish()"
Cohesion: 0.14
Nodes (7): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value

### Community 113 - ".init()"
Cohesion: 0.16
Nodes (9): Any, Int, NSRect, NSScrollView, NSTableColumn, NSTableView, NSTextField, NSTextView (+1 more)

### Community 115 - "stream_test.cpp"
Cohesion: 0.16
Nodes (18): describe(), evalBody(), fillNursery(), smi(), TEST(), timedEval(), utf8(), GrowAndContentsWithFullNursery (+10 more)

### Community 117 - ".isHeap()"
Cohesion: 0.16
Nodes (17): holdsBinding(), liveClasses(), methodHoldingDropped(), ownClassName(), Gc::clearWeakAfterNursery(), Gc::collectOld(), Gc::copy(), Gc::scavengeFromRoots() (+9 more)

### Community 118 - "allocateRetry()"
Cohesion: 0.13
Nodes (18): create(), createBlock(), createMethod(), allocateInstance(), allocateRetry(), workspaceBinding(), ao_AoTest_assert_equals_(), CallContext (+10 more)

### Community 120 - "string"
Cohesion: 0.18
Nodes (18): acceptClassSource(), applyChunks(), assignError(), deferredListing(), deferredMethods(), definedClassVarNames(), definedInstVarNames(), fileInFile() (+10 more)

### Community 122 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): callsWithLocal(), expectArray42(), gcThenArg(), gcThenReceiver(), probeMethod(), stubEq(), TEST(), unrootedArray42() (+8 more)

### Community 123 - "TEST()"
Cohesion: 0.12
Nodes (17): TEST(), ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger (+9 more)

### Community 124 - "TEST()"
Cohesion: 0.12
Nodes (17): TEST(), ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs (+9 more)

### Community 130 - "TEST()"
Cohesion: 0.13
Nodes (15): makeSubclass(), TEST(), ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed (+7 more)

### Community 131 - "TEST()"
Cohesion: 0.13
Nodes (15): installNative(), subclassOfObject(), TEST(), CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip (+7 more)

### Community 132 - "kernel_numeric_test.cpp"
Cohesion: 0.18
Nodes (12): pow2(), ratio(), TEST(), FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench (+4 more)

### Community 133 - "TEST()"
Cohesion: 0.13
Nodes (15): firstNonEmptyLine(), TEST(), CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense (+7 more)

### Community 141 - ".isTrue()"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), pickBy(), sendBin() (+2 more)

### Community 143 - "TEST()"
Cohesion: 0.15
Nodes (13): TEST(), ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, next, IntegerBeyondInt64KeepsItsDigits, IntegerMantissaWithExponentIsInteger, EighteenDigitIntegerIsExactInt64 (+5 more)

### Community 144 - "TEST()"
Cohesion: 0.15
Nodes (13): TEST(), ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, ShowInvokesHook (+5 more)

### Community 148 - "TEST()"
Cohesion: 0.17
Nodes (12): TEST(), AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating (+4 more)

### Community 15 - "Stream.cpp"
Cohesion: 0.11
Nodes (63): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+55 more)

### Community 153 - "path"
Cohesion: 0.18
Nodes (10): expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage(), utf8Bytes(), writeAll(), path (+2 more)

### Community 155 - "TEST()"
Cohesion: 0.18
Nodes (11): TEST(), BlockMethodHasItsOwnMapInMethodCoordinates, CopiedOuterTempIsNamedInBlockScope, ImplicitReturnsMapToLastStatement, InlinedConditionJumpMapsToReceiverSpan, PcMap, RemoteTempNamesVectorSlotAndIndex, ReturnBlockMapsToReturnStatement (+3 more)

### Community 159 - "exactFloat()"
Cohesion: 0.31
Nodes (11): exactFloat(), magBits(), magCmp(), magShl(), magSub(), nearestDouble(), nearestFloat(), Scanner::Scanner() (+3 more)

### Community 162 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), headerWord(), isCompiledMethod() (+2 more)

### Community 163 - "TEST()"
Cohesion: 0.20
Nodes (10): install(), TEST(), BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown (+2 more)

### Community 164 - "specialIndex()"
Cohesion: 0.27
Nodes (10): operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST(), BytecodeIsa, Op, uint8_t (+2 more)

### Community 170 - "contextPc()"
Cohesion: 0.44
Nodes (10): contextPc(), DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget(), sentToSuper(), synthesizesNative(), CallContext (+2 more)

### Community 171 - "clearUnwinding()"
Cohesion: 0.36
Nodes (10): clearUnwinding(), makeAoTest(), readFile(), runFile(), runSmalltalkTests(), CallContext, path, Root (+2 more)

### Community 176 - "intern()"
Cohesion: 0.25
Nodes (8): bytes(), intern(), WellKnown::define(), WellKnown::internSpecialSelectors(), isFixedGlobal, isPseudoVariableName, string_view, WellKnown

### Community 178 - "TEST()"
Cohesion: 0.25
Nodes (8): TEST(), CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, SmallIntegerRoundTrip

### Community 179 - "named()"
Cohesion: 0.29
Nodes (8): firstBlock(), named(), pcOf(), spanAt(), MethodImage, Op, string, string_view

### Community 189 - "fillFullTable()"
Cohesion: 0.33
Nodes (7): fillFullTable(), keyWithHome(), smi(), int64_t, Root, set, uint32_t

### Community 194 - "Scheduler::runFiber()"
Cohesion: 0.33
Nodes (6): Scheduler::endEval(), Scheduler::runFiber(), endEval, reapDead, recordFailure, outOfMemory

### Community 196 - "Runtime.cpp"
Cohesion: 0.40
Nodes (3): boot(), shutdown(), runtime

### Community 197 - "popFrame"
Cohesion: 0.40
Nodes (5): popFrame, pushFrame, uint32_t, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 198 - "Scheduler::resume()"
Cohesion: 0.40
Nodes (5): Scheduler::resume(), Scheduler::suspend(), enqueue, find, block

### Community 204 - "TEST()"
Cohesion: 0.50
Nodes (4): TEST(), BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil

### Community 205 - "TEST()"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 206 - "Roots::visitAll()"
Cohesion: 0.50
Nodes (4): Roots::Stack::visit(), Roots::visitAll(), walker_, VisitFn

### Community 209 - "imageRegistryStubA()"
Cohesion: 0.67
Nodes (4): imageRegistryStubA(), imageRegistryStubB(), CallContext, uint32_t

### Community 210 - "answerOne()"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 215 - "abortingNew()"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 22 - "TEST()"
Cohesion: 0.05
Nodes (59): active(), at(), itemsOf(), myList(), newEnv(), readyList(), run(), runAbort() (+51 more)

### Community 30 - "Scheduler.cpp"
Cohesion: 0.09
Nodes (46): nextProcessId(), rootRecord(), Scheduler::abandonAll(), Scheduler::abortHalted(), Scheduler::activeProcess(), Scheduler::addFiber(), Scheduler::awaitEval(), Scheduler::block() (+38 more)

### Community 31 - "TEST()"
Cohesion: 0.04
Nodes (48): soleWeak(), TEST(), AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage (+40 more)

### Community 32 - "TEST()"
Cohesion: 0.04
Nodes (48): countVisitedRoots(), TEST(), ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap (+40 more)

### Community 33 - "TEST()"
Cohesion: 0.06
Nodes (45): at(), bind(), bindIn(), each(), install(), isDictionary(), lookup(), nameAt() (+37 more)

### Community 37 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): align8(), gcStressFromEnv(), Heap::adoptOldBytes(), Heap::allocate(), Heap::allocateNoGc(), Heap::allocateTenured(), Heap::bytes(), Heap::fitsOld() (+34 more)

### Community 39 - "Float.cpp"
Cohesion: 0.15
Nodes (44): ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan(), ao_Float_hash(), ao_Float_lessOrEqual(), ao_Float_lessThan() (+36 more)

### Community 42 - ".false_()"
Cohesion: 0.14
Nodes (35): boolean(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+27 more)

### Community 43 - "TEST()"
Cohesion: 0.06
Nodes (38): placeGarbageBeforePin(), rawWordAt(), TEST(), fillOldAndRetainYoung(), slotsAreNil(), TEST(), TEST(), AllocateNoGcSpillsToOld (+30 more)

### Community 44 - "TEST()"
Cohesion: 0.05
Nodes (40): TEST(), ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails (+32 more)

### Community 45 - "TEST_F()"
Cohesion: 0.06
Nodes (40): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod(), TEST_F(), AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook (+32 more)

### Community 52 - "putNative()"
Cohesion: 0.08
Nodes (36): installArray(), installBehavior(), installBoolean(), installCharacter(), installCollection(), installCompiledMethod(), installDictionary(), installGeometry() (+28 more)

### Community 55 - "Compile.cpp"
Cohesion: 0.16
Nodes (33): isClassShaped(), superclassOf(), acceptMethodSource(), anyMethodIn(), boxBytes(), boxedOk(), boxLiteral(), boxMethodImage() (+25 more)

### Community 66 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (29): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+21 more)

### Community 67 - "send()"
Cohesion: 0.20
Nodes (29): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+21 more)

### Community 70 - "DebugSnapshot.cpp"
Cohesion: 0.16
Nodes (26): DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method(), DebugSnapshot::pc(), DebugSnapshot::process(), DebugSnapshot::receiver(), DebugSnapshot::rootSlots(), DebugSnapshot::selector() (+18 more)

### Community 71 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 76 - "TEST()"
Cohesion: 0.11
Nodes (26): assemble(), compileBinary(), compileObjectMethod(), makeFloat(), noLiterals(), op(), runBinary(), runMethod() (+18 more)

### Community 77 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 79 - "TEST()"
Cohesion: 0.08
Nodes (26): TEST(), BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo (+18 more)

### Community 87 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 9 - "TEST()"
Cohesion: 0.03
Nodes (78): b4Definition(), b5ClassDefinition(), b5Definition(), b5LayoutOverride(), p10Method(), TEST(), AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs (+70 more)

### Community 92 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (23): adopt(), bindingAt(), copy(), instSizeOf(), isBinding(), isBytes(), isName(), isPointers() (+15 more)

### Community 94 - "TEST()"
Cohesion: 0.11
Nodes (23): answerMessage(), expectBox(), install(), pairAfterAlloc(), stubA(), stubB(), TEST(), trueDnuSentinel() (+15 more)

### Community 98 - "TEST()"
Cohesion: 0.10
Nodes (21): repeated(), TEST(), BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope (+13 more)

### Community 99 - "Scanner.cpp"
Cohesion: 0.21
Nodes (21): digitValue(), isAlnum(), isAlpha(), isBinaryChar(), isDigit(), isSpace(), largeIntText(), magMulAdd() (+13 more)

### Community 138 - "P10 scope (do/don't)"
Cohesion: 0.21
Nodes (14): Abort-time snapshot capture, CallContext::topFrame frame chaining, Debugger window spec (frames/source/variables), Object>>halt spec (reason halt), P10 scope (do/don't), Workspace Debug button / process failed:, Busy & image-save rules for halted processes, DebugFrames abstraction (snapshot + live frames) (+6 more)

### Community 146 - "abortEvaluation / abortEvaluationQuiet"
Cohesion: 0.15
Nodes (13): ao_debug_* ABI design (P10-05), abortEvaluation / abortEvaluationQuiet, App window patterns (NSWindow, WorkspaceWindow, InspectorWindow), DebugSnapshot::capture design, Frame/Temps/OperandStack live on C++ stack, not heap, HostOopHook (transcript/inspect hooks), ao_Object_halt design (P10-03), performSend (+5 more)

### Community 156 - "P11 step branch bench"
Cohesion: 0.18
Nodes (11): P11 known limitations, v1 known limitations, SmallInteger fast path (no-send arithmetic), P11 acceptance criteria, P11 manual verification (unchecked, GUI unavailable in session), P11 TDD test files, Verification plan, B2 to:do: bench (+3 more)

### Community 160 - "Plan architecture: Frame chain + snapshot + pcMap"
Cohesion: 0.18
Nodes (11): Plan architecture: Frame chain + snapshot + pcMap, Emitter::mark / compileSend / compileReturn / compileInlined, Frame chain design (P10-03), Graphify god nodes & communities for this work, compiler::MethodImage, NativeMethod::invoke (no context pushed), pc→source & temp-name design (P10-02), P10 debugger plan reference (+3 more)

### Community 165 - "Proceed/Abort/Step into,over,out operations"
Cohesion: 0.20
Nodes (10): ao_set_debug_capture / ao_debug_*, Ao.app Debugger window, ao_set_debug_mode(AO_DEBUG_LIVE) / AO_ERR_HALT, Object>>halt, ao_debug_* ABI busy rules, Debug it (⌘⇧D), Debugger window operation buttons, Proceed/Abort/Step into,over,out operations (+2 more)

### Community 166 - "docs/README.md"
Cohesion: 0.22
Nodes (9): P0→P11 linear dependency, Phase progression table, Plan scope (do/don't) for P10, SPEC.md change table (P10-01), 1.0.0 release, CLAUDE.md (process source of truth), P11 plan reference (p11-ancient-matsumoto.md), SPEC.md (product source of truth) (+1 more)

### Community 180 - "P10-01..P10-07 PR list"
Cohesion: 0.29
Nodes (8): PR table, P10 acceptance criteria, P10-01..P10-07 PR list, P10 TDD test files, P11-01..P11-07 PR list, PR sequence & dependencies (01→02→04→05→06→07), P4 microbench, P10 constraints (no interpreter/native coupling, bench ratio)

### Community 212 - "Session::MethodSource table"
Cohesion: 0.67
Nodes (3): evalBody / sessionEval, Session::MethodSource table, Source table extension design (P10-04)

### Community 195 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 203 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 134 - "ao_runtime library target"
Cohesion: 0.13
Nodes (15): ao_runtime library target, ao executable target, cli_filein / cli_image_save / cli_test ctest targets, gcstress_vendor ctest target, src/FiberSwitch_arm64.S, Bag size not in P9 golden, FileStream subclass does not exist, MappedCollection absent from pinned sources (+7 more)

### Community 139 - "P4-02: Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.14
Nodes (14): P4-01: Object / UndefinedObject / Boolean, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, P4-04: Array / ByteArray / String / Symbol, P4-05: Dictionary / Set / OrderedCollection, P4-06: Stream / Transcript モデル, P4-07: Process / ProcessorScheduler / Semaphore, Object / UndefinedObject / Boolean のネイティブ実装 (+6 more)

### Community 167 - "ao CMake Project"
Cohesion: 0.24
Nodes (10): AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target), test (macOS arm64) Job, CI Workflow, Kernel Scan Test (via ctest), リリース手順 (JA) (+2 more)

### Community 168 - "P3-04: lookup / super / doesNotUnderstand:"
Cohesion: 0.20
Nodes (10): P3-01: Symbol intern, P3-02: MethodDictionary, P3-03: NativeMethod とセレクタマングル, P3-04: lookup / super / doesNotUnderstand:, P3-05: インラインキャッシュとクラスキャッシュ, Smalltalk-80 Blue Book（探索意味論）, Symbol intern, NativeMethod とセレクタマングル規則 (+2 more)

### Community 169 - "Method Removal Rules"
Cohesion: 0.31
Nodes (10): MethodDictionary, ao_browser_source (placeholder), ao_remove_method, invalidateMethodCache(cache, selector), Method Removal Rules, Shape Change (recompile and move methods), Session Source Table (not in image), Linear MethodDictionary (interleaved keys/values) (+2 more)

### Community 173 - "P0-03: CMake + GoogleTest + CLI"
Cohesion: 0.25
Nodes (9): PHASE file marker, ao::boot / shutdown / version_string, ao_abi.h C ABI stub, Serena project.yml, P0-01: git / LICENSE / PHASE / README, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, P0-04: C ABI + Swift smoke (+1 more)

### Community 174 - "P2-03: メタクラス循環（Blue Book 6–10）"
Cohesion: 0.22
Nodes (9): P2-01: WellKnown と即値クラス, P2-02: クラス骨格の割り当て, P2-03: メタクラス循環（Blue Book 6–10）, P2-04: Smalltalk グローバル辞書, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, WellKnown 表と即値クラス, クラス骨格の割り当て, メタクラス循環 (+1 more)

### Community 191 - "Graphify Required Tool"
Cohesion: 0.40
Nodes (5): Standard Implementation Workflow (8 steps), Ao Smalltalk Overview (EN), Ao Smalltalk 概要 (JA), Graphify Required Tool, Serena Required Tool

### Community 192 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.33
Nodes (6): PHASE file, docs/bench.md, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 21 - "P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt"
Cohesion: 0.06
Nodes (61): Codegen.cpp: Emitter::mark と compile* 群, MethodImage.hpp: PcSpan / TempName / pcMap / temps, BlockContext.cpp: runAside / abortSetAside, DebugSnapshot.hpp/.cpp (DebugSink, DebugFrames, capture/clear), InterpFrame.hpp (Frame/Temps/OperandStack), Interpreter.cpp: 連結ガード、performSend, NativeMethod.hpp: CallContext::topFrame/debug/abortSetAside, Object.cpp/Natives.hpp: ao_Object_halt (+53 more)

### Community 211 - "Rules That Do Not Bend"
Cohesion: 0.67
Nodes (3): Fixed Design Decisions, 曲げない規則, Rules That Do Not Bend

### Community 28 - "Bytecode interpreter"
Cohesion: 0.05
Nodes (51): Smalltalk Scanner（字句解析）, Smalltalk Parser と AST, Ao バイトコード ISA, Codegen（CompiledMethod 生成）, Chunk file-in パーサ, Context GC roots, MethodContext / BlockContext, compiler_roundtrip_test (+43 more)

### Community 41 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 横断テーマ4: Browser/Accept でのデータ消失, 横断テーマ1: GC安全性(メモリ破壊), 横断テーマ3: 言語意味論の欠落(コンパイラ), 推奨する着手順, 横断テーマ5: 資源の上限とその先の振る舞い, 横断テーマ2: 失敗が黙って成功になる, Native selector mangling スキーム (ao_<Class>_<selectorMangled>), CompiledMethod accessor NativeMethods (P5) (+33 more)

### Community 58 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it, SPEC §3.9 Five-pane Browser, instance/class, accept, hierarchy, SPEC §3.9 Menu and Cmd-D / Cmd-I, SPEC §3.9 Transcript show/cr/clear, fixed pitch, survives close, SPEC §3.9 VoiceOver, no excess animation (+25 more)

### Community 60 - "P1 — オブジェクトメモリ"
Cohesion: 0.11
Nodes (31): ao::Heap — ヘッダ付き bump 割り当て, ao::Roots — GC ルート API, WellKnown.hpp — well-known クラス表, MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, CompiledMethod — バイトコード生成物, Interpreter / bytecode ループ (MethodContext・BlockContext), P0 (親フェーズ, stub) (+23 more)

### Community 63 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.09
Nodes (31): .aoimage Restart Method Persistence, GRAPH_REPORT.md Process Check, ao_Object_identityEquals Native Method, Serena Symbol Resolution, SPEC.md §6 Acceptance Checklist, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin (+23 more)

### Community 64 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.10
Nodes (31): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+23 more)

### Community 102 - "P12 Browser Removal Design"
Cohesion: 0.15
Nodes (22): Browser Removal XCTests, Kernel Scan Test (KernelScan.MethodDictionaryValuesAreNativeMethods), remove_abi_test, confirmDiscard (swappable confirmation), makeErrorField, model.refresh() and publish(), Acceptance Criteria, Browser Removal (P12 remove rules) (+14 more)

### Community 110 - "Global Dictionary (Smalltalk)"
Cohesion: 0.16
Nodes (20): ao_remove_class, Ao Bytecode Set, .aoimage Image Format (version 3), Bootstrap Procedure, Fixed Globals (56 Kernel classes + Processor + Smalltalk), Global Dictionary (Smalltalk), Image Load Checks, Atomic Image Save (+12 more)

### Community 137 - "Compiler (.st to CompiledMethod)"
Cohesion: 0.13
Nodes (15): Chunk Format, CompiledMethod, Compiler (.st to CompiledMethod), Heap Object Header (class/size/flags/hash), 16-bit Identity Hash, Inline Expansion (ifTrue:/whileTrue:/to:do:), Message Send and Lookup, Method Cache (inline + class megamorphic) (+7 more)

### Community 142 - "C ABI (bridge/ao_abi.h)"
Cohesion: 0.20
Nodes (14): ao_accept_class, ao_eval, ao_image_load, AoSpan, C ABI (bridge/ao_abi.h), ensureKernelNatives, Bridge (runtime-app boundary), Busy / Reentrancy Rule (+6 more)

### Community 147 - "Failure Aborts Evaluation (abortEvaluation)"
Cohesion: 0.15
Nodes (13): Debugger Readout ABI (ao_debug_*), Failure Aborts Evaluation (abortEvaluation), Host GUI (AppKit), Debug Snapshot Capture, Debugger Window, Empty OOP (native failure marker), ensure: / ifCurtailed: Cleanup, Failure Reason Table (+5 more)

### Community 154 - "Cooperative Process Scheduler"
Cohesion: 0.17
Nodes (12): Abandon (terminate without cleanup), allocateRetry (GC-capable allocation), Base Process, Cooperative Process Scheduler, Deadlock: no runnable process, Fiber Process Stacks (8 MiB + guard page), Full GC Triggers, GC Roots (+4 more)

### Community 177 - "Class Definition Re-Accept"
Cohesion: 0.28
Nodes (9): hasSubclass (Compile.cpp), Class Definition Re-Accept, Class Variables (classPool of Associations), Damaged Table Rule, Dictionary and Set Hash Tables (open addressing), Table Generation Number (reentrancy detection), Removal Risks, Hash Home Mixing (Fibonacci multiplier) (+1 more)

### Community 184 - "Imported Class Library (no self-authored library)"
Cohesion: 0.25
Nodes (8): ao_accept_method, Constraints (new VM, minimal deps, no copying), Cuis Smalltalk, File-in Errors and DEFERRED.md, Graphify and Serena Required, Startup and Vendor Bundling, SPEC Change Rules, Imported Class Library (no self-authored library)

### Community 185 - "Live Debugger (evaluation process, halting)"
Cohesion: 0.29
Nodes (8): Live Debugger ABI (proceed/step/abort), Debugger (P10/P11), Evaluation Process, Frame Kinds and Labels, Live Debugger (evaluation process, halting), P10 Post-mortem Debugger Phase, P11 Live Debugger Phase, Implementation Phases P0-P12

### Community 190 - "Kernel Classes (NativeMethod required)"
Cohesion: 0.29
Nodes (7): Class Names as Interned Symbols, = and hash Contract, Instance Variable Names Table, Kernel Classes (NativeMethod required), v1 Completion Definition, Ao SPEC (macOS native Smalltalk), Kernel-added Slots Read-only

### Community 217 - "WriteStream String Writes (reserve capacity)"
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
- `Blue Book (Smalltalk-80: The Language and its Implementation)` → `Smalltalk-80 removeFromSystem`  [AMBIGUOUS]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md · relation: conceptually_related_to

## Knowledge Gaps
- **956 isolated node(s):** `CallContext`, `MethodImage`, `CallContext`, `CallContext`, `CallContext` (+951 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2513 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **96 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

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
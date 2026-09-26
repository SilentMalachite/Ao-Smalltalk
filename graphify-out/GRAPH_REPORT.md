# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 287 files · ~286,979 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5513 nodes · 14358 edges · 288 communities (197 shown, 91 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 1663 edges (avg confidence: 0.82)
- Token cost: 1,180,610 input · 0 output

## Community Hubs (Navigation)
- WellKnown
- .fromSmallInteger()
- TEST()
- Interpreter.cpp
- Float.cpp
- TEST()
- .isHeap()
- Codegen.cpp
- LargeInteger.cpp
- Parser
- Stream.cpp
- Boot
- TEST()
- TEST_F()
- Emitter
- Scanner.cpp
- DebuggerWindow
- handlescope
- TEST()
- Claude Review Fixes Plan (2026-09-23)
- Object.cpp
- Heap
- TEST()
- Oop
- AcceptTests
- abi.cpp
- Bytecode interpreter
- WorkspaceWindow
- TEST()
- Literal
- TEST()
- Compile.cpp
- Heap.cpp
- DebugSnapshot
- TEST()
- BrowserWindow
- TEST_F()
- ImageSave.cpp
- 横断テーマ2: 失敗が黙って成功になる
- TEST()
- P1 — オブジェクトメモリ
- Scheduler.cpp
- TEST()
- TEST_F()
- TEST_F()
- CallContext
- cstdint
- ImageLoad.cpp
- Scheduler
- vector
- gc_safety_test.cpp
- putNative()
- Session
- SPEC.md (Product Specification)
- Session.cpp
- ToolWindowTests
- ao_eval()
- Task 12: Browser accept, Hierarchy, VoiceOver, C
- String.cpp
- AoApp
- WellKnown.cpp
- .run()
- BrowserModel
- DebuggerWindowTests
- .nil()
- VendorExtract.cpp
- TEST()
- .true_()
- Geometry.cpp
- ao_runtime_shutdown()
- TEST()
- DiskHeader
- Gc
- CompileEnv
- BlockContext.cpp
- Roots
- string
- string_view
- TEST()
- TEST_F()
- Array.cpp
- Scheduler::Record
- AppKit
- ChunkParser.cpp
- FiberStack
- ClassPool.cpp
- TEST_F()
- vendor_filein_test.cpp
- uint64_t
- TranscriptWindow
- ChunkAction
- CompileError
- ImageFormat
- DebugSnapshot.cpp
- MethodSource
- UniformFont
- TEST()
- Process.cpp
- NativeMethod.cpp
- TEST()
- TEST()
- string
- Roots.cpp
- HashedCollection.cpp
- Fiber.cpp
- DefinitionScanner
- BrowserModelTests
- send2()
- TEST()
- P7 .aoimage Implementation Plan
- PingPong
- ImageSurgery
- TEST()
- .build()
- Ast
- TEST()
- sessionImageLoad()
- allocateRetry()
- GarbageFirstBoot
- native_method_test.cpp
- TEST()
- TEST()
- P6b Vendor File-in Implementation Plan
- size
- KeptClass
- VirtualRegion.cpp
- Loaded
- Phase Progression P0-P11
- evalBody()
- String
- TEST()
- TEST()
- TEST()
- Stack
- fiber_test.cpp
- TEST()
- P4-06: Stream / Transcript モデル
- FileInError
- RootedArray
- .isTrue()
- installNatives()
- Abort / Evaluation Failure Propagation
- Smalltalk-80 Blue Book
- InspectorWindow
- TEST()
- clearUnwinding()
- TEST()
- TEST()
- P10-04: セッションのソース表の拡張と doIt
- Range
- path
- Token
- P3-04: lookup / super / doesNotUnderstand:
- Table
- ClassMethodCache
- CompiledMethodNatives.cpp
- C ABI Bridge (ao_abi.h)
- Debugger (P10 Post-mortem)
- TEST()
- TEST()
- TEST()
- TEST()
- DebugFrames
- Vendor.hpp
- FiberRegs
- P2-03: メタクラス循環（Blue Book 6–10）
- P0-03: CMake + GoogleTest + CLI
- WellKnown::define()
- StackPool
- Character.cpp
- TEST()
- ObjectHeader
- Counts
- ClassDef
- Parsed
- WellKnown::InternTable
- uint32_t
- PcSpan
- P8–P9 Remaining Implementation Plan
- FileSizeLimit
- cli_test.sh script
- RegisterSwap
- pushFrame
- KeptMethod
- TestDir
- BusyRead
- Metaclass Circularity (Blue Book Rules 6-10)
- TEST()
- Roots::visitAll()
- IgnoreFileSizeSignal
- Root
- imageRegistryStubB()
- answerTwo()
- TDD Test Suites (SPEC section 4)
- ao-smalltalk/AGENTS.md
- .turnRunLoop()
- フェーズの直線依存 (P0→...→P11)
- WellKnown::checkSmallIntegerFastPath()
- Utf8Step
- abortingNew()
- usage()
- PackageDescription
- docs/native-selectors.md
- ao_compiler_tests executable target
- チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない
- 64KiBを超えるPrint it/Inspect itは副作用だけ残して失敗する
- String・Symbol・Arrayの=とhashが一致しない
- 2^63以上の整数リテラルが黙って0になる(compiler側)
- Stringの添字アクセスがO(i)なのでdo:・contents・nextPutAll:がO(
- Kernelクラスのサブクラスでインスタンス変数の添字が親の物理スロットと重なる
- .globalsVersion()
- package-app.sh script
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
- Serena project.yml
- Kernel Instance Variable Read-Only Rule
- Message
- MethodDictionary

## God Nodes (most connected - your core abstractions)
1. `Oop` - 915 edges
2. `Heap` - 252 edges
3. `vector` - 172 edges
4. `WellKnown` - 167 edges
5. `TEST()` - 107 edges
6. `Ast` - 103 edges
7. `Boot` - 101 edges
8. `TEST()` - 100 edges
9. `TEST()` - 96 edges
10. `Session` - 93 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `NativeRegistry::addNamed / findName`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `Native Selector Overwrite Refusal` --implements--> `applyClassDef / applyMethodsFor`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `Native Method Rebind by Name (no function pointers serialized)` --implements--> `NativeRegistry::addNamed / findName`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/NativeMethod.cpp
- `Host Word Patch (ao-host-patch)` --implements--> `ao::extractVendor`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp
- `LOAD_ORDER Dependency Sequencing` --implements--> `ao::extractVendor`  [EXTRACTED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/VendorExtract.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **P0 skeleton deliverables** — bridge_ao_abi, serena_project [EXTRACTED 1.00]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **v1 Non-goals Stated Consistently Across Docs** — changelog_known_limitations_p10, readme_not_in_v1, contributing_rules_that_do_not_bend [INFERRED 0.85]
- **Metaclass Circularity Cycle** — spec_object, spec_behavior, spec_class, spec_metaclass [EXTRACTED 1.00]
- **Debugger Read Pipeline** — spec_debugger, spec_debug_capture, spec_frame_chain, spec_debug_abi [EXTRACTED 1.00]
- **Cooperative Process Scheduling Group** — spec_process, spec_processor_scheduler, spec_semaphore, spec_shared_queue [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — docs_claude_review_00_recent_diff_classdef_string_silently_dropped, docs_claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, docs_claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, docs_claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — docs_prs_p1_02_pr, docs_prs_p1_03_pr, docs_prs_p1_04_pr, docs_prs_p1_05_pr, docs_prs_p1_06_pr, docs_prs_p1_07_pr [INFERRED 0.85]
- **P10 事後デバッガの構成要素 (フレーム連鎖→スナップショット→Debugger窓)** — docs_phases_p10_callcontext, docs_phases_p10_debugsnapshot, docs_phases_p10_debuggerwindow [EXTRACTED 1.00]
- **P11 ライブデバッガ操作 (評価プロセス→Scheduler停止復帰→Debugger窓操作ボタン)** — docs_phases_p11_evalprocess, docs_phases_p11_scheduler, docs_phases_p10_debuggerwindow [INFERRED 0.85]
- **P10 デバッガフェーズの実装フロー（コンパイラ pc 表 → 捕捉 → セッション → ABI → GUI → 受け入れ）** — docs_prs_p10_02_doc, docs_prs_p10_03_doc, docs_prs_p10_04_doc, docs_prs_p10_05_doc, docs_prs_p10_06_doc, docs_prs_p10_07_doc [INFERRED 0.95]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — docs_prs_p4_01_doc, docs_prs_p4_02_doc, docs_prs_p4_03_doc, docs_prs_p4_04_doc, docs_prs_p4_05_doc, docs_prs_p4_06_doc, docs_prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunkparser [INFERRED 0.85]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **AppKit ツールウィンドウ群とメニュー** — docs_prs_p8_01_ao_app_skeleton, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [INFERRED 0.80]
- **Vendor Loading Kernel-Integrity Safeguards** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_allowlist, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_native_overwrite_refusal, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_kernel_scan_narrowing, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_host_patch [INFERRED 0.80]
- **aoimage Method Persistence Across Restart** — docs_prs_p9_04_aoimage_restart, docs_superpowers_plans_2026_09_22_p7_aoimage_save_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_load_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_native_rebind_by_name, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.85]
- **Vendor File-in to Image Snapshot Pipeline** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_load_order, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_link_class, runtime_src_ao_main_image_cli, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.80]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **Vendor extraction, deferral list, and file-in test form one pipeline** — image_vendor_origin_extract_process, image_vendor_deferred_doc, runtime_tests_vendor_filein_test [INFERRED 0.80]
- **Interpreted frame chain to abort snapshot to C ABI to Debugger window** — docs_superpowers_plans_2026_09_26_p10_debugger_frame_chain, docs_superpowers_plans_2026_09_26_p10_debugger_debugsnapshot, docs_superpowers_plans_2026_09_26_p10_debugger_debug_abi, docs_superpowers_plans_2026_09_26_p10_debugger_debugger_window [EXTRACTED 1.00]

## Communities (288 total, 91 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - ".fromSmallInteger()"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 2 - "TEST()"
Cohesion: 0.02
Nodes (84): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+76 more)

### Community 3 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (69): Frame, context, isBlock, method, pc, prev, receiver, sendArgc (+61 more)

### Community 4 - "Float.cpp"
Cohesion: 0.08
Nodes (77): NumberOp, NumberRelation, NumKind, bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes() (+69 more)

### Community 5 - "TEST()"
Cohesion: 0.03
Nodes (78): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+70 more)

### Community 6 - ".isHeap()"
Cohesion: 0.07
Nodes (70): clearWeakAfterOldMark, run, InlineCache, cachedClass, cachedMethod, size_t, uintptr_t, unordered_set (+62 more)

### Community 7 - "Codegen.cpp"
Cohesion: 0.05
Nodes (60): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+52 more)

### Community 8 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 9 - "Parser"
Cohesion: 0.11
Nodes (22): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+14 more)

### Community 10 - "Stream.cpp"
Cohesion: 0.11
Nodes (63): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+55 more)

### Community 11 - "Boot"
Cohesion: 0.05
Nodes (62): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+54 more)

### Community 12 - "TEST()"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 13 - "TEST_F()"
Cohesion: 0.03
Nodes (59): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+51 more)

### Community 14 - "Emitter"
Cohesion: 0.14
Nodes (11): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+3 more)

### Community 15 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 16 - "DebuggerWindow"
Cohesion: 0.07
Nodes (38): aoDebuggerInspectHook(), DebugFrame, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .open, .selectedFrame (+30 more)

### Community 17 - "handlescope"
Cohesion: 0.13
Nodes (25): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, chrono, Chunk, cmath, compile, CompiledMethod, compiler (+17 more)

### Community 18 - "TEST()"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 19 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.05
Nodes (55): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+47 more)

### Community 20 - "Object.cpp"
Cohesion: 0.11
Nodes (53): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+45 more)

### Community 21 - "Heap"
Cohesion: 0.04
Nodes (46): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+38 more)

### Community 22 - "TEST()"
Cohesion: 0.04
Nodes (52): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+44 more)

### Community 23 - "Oop"
Cohesion: 0.11
Nodes (44): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+36 more)

### Community 24 - "AcceptTests"
Cohesion: 0.14
Nodes (9): AcceptTests, NSMenu, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+1 more)

### Community 25 - "abi.cpp"
Cohesion: 0.09
Nodes (48): atomic, Body, CountsAnswerMinusOneOnFailure, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at(), ao_browser_class_count(), ao_browser_class_definition() (+40 more)

### Community 26 - "Bytecode interpreter"
Cohesion: 0.05
Nodes (51): P4-08: Point / Rectangle, Point/Rectangle 座標計算, P4-09: Kernel NativeMethod 走査と bench, Kernel NativeMethod 走査, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+43 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.11
Nodes (15): sendToKeyWorkspace(), Bool, NSWindow, WorkspaceWindow, .errorAccessibilityLabel, .errorText, .inspectorCount, .inspectorText (+7 more)

### Community 28 - "TEST()"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 29 - "Literal"
Cohesion: 0.05
Nodes (49): BlockMethodHasItsOwnMapInMethodCoordinates, int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr (+41 more)

### Community 30 - "TEST()"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 31 - "Compile.cpp"
Cohesion: 0.11
Nodes (46): cctype, isClassShaped(), superclassOf(), acceptMethodSource(), anyMethodIn(), applyMethodsFor(), boxBytes(), boxedOk() (+38 more)

### Community 32 - "Heap.cpp"
Cohesion: 0.09
Nodes (44): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+36 more)

### Community 33 - "DebugSnapshot"
Cohesion: 0.05
Nodes (41): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+33 more)

### Community 34 - "TEST()"
Cohesion: 0.04
Nodes (47): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+39 more)

### Community 35 - "BrowserWindow"
Cohesion: 0.08
Nodes (24): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+16 more)

### Community 36 - "TEST_F()"
Cohesion: 0.06
Nodes (36): BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots, DoItFrameSourceDropsPrefix, EvalErrorFillsFramesInnermostFirst (+28 more)

### Community 37 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (45): cerrno, climits, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord() (+37 more)

### Community 38 - "横断テーマ2: 失敗が黙って成功になる"
Cohesion: 0.05
Nodes (46): B2 to:do: bench（jump 展開ループ）, bytecode/native 比率メトリクス, SmallInteger fast path（send を経ない演算）, P4 microbench（native to:do: 基準）, P6 interpreter bench（bytecode block send）, 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる (+38 more)

### Community 39 - "TEST()"
Cohesion: 0.05
Nodes (37): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+29 more)

### Community 40 - "P1 — オブジェクトメモリ"
Cohesion: 0.08
Nodes (40): P0 (親フェーズ, stub), CallContext::topFrame — 解釈フレーム連鎖, Debugger 窓 (frames / source / variables), abort スナップショット / ao_debug_* ABI, P10 — 事後デバッガ, 評価プロセス (doIt を fork したプロセスで実行), P11 — ライブデバッガ, Scheduler (terminate / resumeTo / switchTo) (+32 more)

### Community 41 - "Scheduler.cpp"
Cohesion: 0.11
Nodes (42): afterResume, enqueue, find, leaveLists, reapDead, signal, switchTo, takeNext (+34 more)

### Community 42 - "TEST()"
Cohesion: 0.06
Nodes (38): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+30 more)

### Community 43 - "TEST_F()"
Cohesion: 0.06
Nodes (40): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+32 more)

### Community 44 - "TEST_F()"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 45 - "CallContext"
Cohesion: 0.06
Nodes (37): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+29 more)

### Community 46 - "cstdint"
Cohesion: 0.08
Nodes (18): cassert, cstddef, cstdint, cstdlib, memory, CallContext, DebugSink, onAbort (+10 more)

### Community 47 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (35): ifstream, readHeader, acceptWord(), atOffset(), checkFile(), checkGlobals(), byte, Roots (+27 more)

### Community 48 - "Scheduler"
Cohesion: 0.06
Nodes (35): CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler, abandonAll (+27 more)

### Community 49 - "vector"
Cohesion: 0.09
Nodes (27): Bootstrap, vector, MethodDictionary, resource, namedSlotNames(), superclassChainFromRoot(), DebugInfo, bodies (+19 more)

### Community 50 - "gc_safety_test.cpp"
Cohesion: 0.11
Nodes (29): cstdio, dyld, filesystem, fstream, future, install, iterator, addRoots (+21 more)

### Community 51 - "putNative()"
Cohesion: 0.08
Nodes (36): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+28 more)

### Community 52 - "Session"
Cohesion: 0.09
Nodes (36): argCountOf(), uint32_t, debugInfoFor(), debugTempCount(), debugTempName(), findEntry(), frameDebugInfo(), frameInRange() (+28 more)

### Community 53 - "SPEC.md (Product Specification)"
Cohesion: 0.07
Nodes (33): ao_debug_* / ao_set_debug_capture C ABI, Post-mortem Debugger Capture, P10 Known Limitations, Unreleased: Phase P10 Debugger, 1.0.0 Release Entry, Fixed Design Decisions, Graphify Required Tool, PHASE File (+25 more)

### Community 54 - "Session.cpp"
Cohesion: 0.16
Nodes (31): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+23 more)

### Community 55 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+3 more)

### Community 56 - "ao_eval()"
Cohesion: 0.09
Nodes (33): AoSpan, end, message, start, ao_accept_class(), ao_accept_method(), ao_eval(), ao_image_load() (+25 more)

### Community 57 - "Task 12: Browser accept, Hierarchy, VoiceOver, C"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 58 - "String.cpp"
Cohesion: 0.18
Nodes (30): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+22 more)

### Community 59 - "AoApp"
Cohesion: 0.11
Nodes (14): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+6 more)

### Community 60 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (31): findSymbol, global, internWith, Roots, string_view, WellKnown, ImageSelector, name (+23 more)

### Community 61 - ".run()"
Cohesion: 0.12
Nodes (21): AnyObject, aoWorkspaceInspectHook(), failureText(), installDebugButton(), installErrorField(), keptEvalResult(), selectErrorSpan(), spanMessage() (+13 more)

### Community 62 - "BrowserModel"
Cohesion: 0.21
Nodes (8): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, UnsafeMutablePointer

### Community 63 - "DebuggerWindowTests"
Cohesion: 0.14
Nodes (10): DebuggerWindowTests, Int, NSFont, NSTableView, NSTextView, NSView, NSWindow, String (+2 more)

### Community 64 - ".nil()"
Cohesion: 0.19
Nodes (27): flags, slotAt, slotAtPut, int64_t, Gc::clearWeakAfterOldMark(), int64_t, uint32_t, WellKnown (+19 more)

### Community 65 - "VendorExtract.cpp"
Cohesion: 0.19
Nodes (29): allowIndex(), containsHostWord(), string, string_view, doubleBangs(), ao::extractVendor, firstLineKey(), firstNonEmptyLine() (+21 more)

### Community 66 - "TEST()"
Cohesion: 0.08
Nodes (29): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+21 more)

### Community 67 - ".true_()"
Cohesion: 0.21
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 68 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 69 - "ao_runtime_shutdown()"
Cohesion: 0.08
Nodes (23): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost (+15 more)

### Community 70 - "TEST()"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 71 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 72 - "Gc"
Cohesion: 0.11
Nodes (26): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, collectBeforeTenured, collectNursery (+18 more)

### Community 73 - "CompileEnv"
Cohesion: 0.08
Nodes (22): Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, CompileResult (+14 more)

### Community 74 - "BlockContext.cpp"
Cohesion: 0.17
Nodes (27): ao_runtime library target, ao executable target, cli_filein / cli_image_save / cli_test ctest targets, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat() (+19 more)

### Community 75 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 76 - "string"
Cohesion: 0.17
Nodes (28): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+20 more)

### Community 77 - "string_view"
Cohesion: 0.11
Nodes (14): algorithm, string_view, NativeMethod, CallContext, MethodImage, CallContext, WellKnown, SlotNames (+6 more)

### Community 78 - "TEST()"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 79 - "TEST_F()"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 80 - "Array.cpp"
Cohesion: 0.18
Nodes (24): valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+16 more)

### Community 81 - "Scheduler::Record"
Cohesion: 0.08
Nodes (25): Scheduler, size_t, uint64_t, unique_ptr, Scheduler::liveFibers(), Scheduler::Record, abandon, awaitingTerminate (+17 more)

### Community 82 - "AppKit"
Cohesion: 0.12
Nodes (12): Ao, aoTranscriptHook(), CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, SmokeTests, AppKit (+4 more)

### Community 83 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 84 - "FiberStack"
Cohesion: 0.13
Nodes (19): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+11 more)

### Community 85 - "ClassPool.cpp"
Cohesion: 0.20
Nodes (24): hashedcollection, adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view (+16 more)

### Community 86 - "TEST_F()"
Cohesion: 0.09
Nodes (21): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+13 more)

### Community 87 - "vendor_filein_test.cpp"
Cohesion: 0.11
Nodes (20): classpool, DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path (+12 more)

### Community 88 - "uint64_t"
Cohesion: 0.23
Nodes (16): bindAll(), uint64_t, unordered_map, fileOop(), headerAt(), heapShaped(), bits, ObjectRules (+8 more)

### Community 89 - "TranscriptWindow"
Cohesion: 0.14
Nodes (14): configureSourceEditing(), makeToolTextWindow(), Any, Bool, NSRect, NSTextView, NSWindow, String (+6 more)

### Community 90 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 91 - "CompileError"
Cohesion: 0.16
Nodes (23): CompileError, message, span, string, acceptClassSource(), applyChunks(), assignError(), path (+15 more)

### Community 92 - "ImageFormat"
Cohesion: 0.09
Nodes (23): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle (+15 more)

### Community 93 - "DebugSnapshot.cpp"
Cohesion: 0.18
Nodes (21): contextPc(), CallContext, Frame, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method() (+13 more)

### Community 94 - "MethodSource"
Cohesion: 0.12
Nodes (23): attachBlocks(), blockLiteral(), MethodImage, Roots, size_t, DebugInfoRef, body, index (+15 more)

### Community 95 - "UniformFont"
Cohesion: 0.13
Nodes (13): Int, NSFont, NSRange, ToolTextSize, .offset, UniformFont, MainActor, Void (+5 more)

### Community 96 - "TEST()"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 97 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 98 - "NativeMethod.cpp"
Cohesion: 0.20
Nodes (20): add(), NativeRegistry::addNamed / findName, apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+12 more)

### Community 99 - "TEST()"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 100 - "TEST()"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 101 - "string"
Cohesion: 0.15
Nodes (5): string, ABI boundary memory, runtime, boot(), shutdown()

### Community 102 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 103 - "HashedCollection.cpp"
Cohesion: 0.23
Nodes (20): bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t, uint64_t (+12 more)

### Community 104 - "Fiber.cpp"
Cohesion: 0.15
Nodes (17): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+9 more)

### Community 105 - "DefinitionScanner"
Cohesion: 0.25
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 106 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 107 - "send2()"
Cohesion: 0.11
Nodes (20): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+12 more)

### Community 108 - "TEST()"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 109 - "P7 .aoimage Implementation Plan"
Cohesion: 0.13
Nodes (20): P9-04 v1 Golden Acceptance, .aoimage Restart Method Persistence, GRAPH_REPORT.md Process Check, ao_Object_identityEquals Native Method, Serena Symbol Resolution, SPEC.md §6 Acceptance Checklist, CmUser (user file-in test class), Extra Section (non-catalog classes) (+12 more)

### Community 110 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 111 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 112 - "TEST()"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 113 - ".build()"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, Selector, String, Void

### Community 114 - "Ast"
Cohesion: 0.11
Nodes (19): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+11 more)

### Community 115 - "TEST()"
Cohesion: 0.13
Nodes (18): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, describe(), evalBody() (+10 more)

### Community 116 - "sessionImageLoad()"
Cohesion: 0.14
Nodes (18): Image, check, load, save, clearMethodSources(), HostOopHook, dropDoItDebug(), ensureKernelNatives() (+10 more)

### Community 117 - "allocateRetry()"
Cohesion: 0.14
Nodes (17): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+9 more)

### Community 118 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 119 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 120 - "TEST()"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 121 - "TEST()"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 122 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.15
Nodes (17): Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes, Link (vendor class) (+9 more)

### Community 123 - "size"
Cohesion: 0.24
Nodes (15): allocateNoGc, size, at(), bind(), bindIn(), Roots, string_view, uint32_t (+7 more)

### Community 124 - "KeptClass"
Cohesion: 0.21
Nodes (17): size_t, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars, deferred (+9 more)

### Community 125 - "VirtualRegion.cpp"
Cohesion: 0.21
Nodes (12): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+4 more)

### Community 126 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 127 - "Phase Progression P0-P11"
Cohesion: 0.16
Nodes (17): Ao Smalltalk System, ao_image_save, Bootstrap Procedure, Cuis Smalltalk, DEFERRED.md Exclusion List, Graphify / Serena Mandatory Tooling, Image Format .aoimage, Image Load Validation Rules (+9 more)

### Community 128 - "evalBody()"
Cohesion: 0.19
Nodes (14): blankOut(), AoInspectFn, optional, DebugEntry, debugFrameLabel(), debugInspect(), debugPrint(), debugReceiverPrint() (+6 more)

### Community 129 - "String"
Cohesion: 0.14
Nodes (16): Array, ByteArray, Character, = and hash Contract, Float (IEEE754 binary64), Fraction, Magnitude, Number / Integer Hierarchy (+8 more)

### Community 130 - "TEST()"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 131 - "TEST()"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 132 - "TEST()"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 133 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 134 - "fiber_test.cpp"
Cohesion: 0.15
Nodes (10): csignal, mach, mach_vm, ao_runtime_tests executable target, gcstress ctest target (AO_GC_STRESS=1, excludes KernelBench.*), googletest v1.15.2 FetchContent dependency, pingPongFiber(), step() (+2 more)

### Community 135 - "TEST()"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 136 - "P4-06: Stream / Transcript モデル"
Cohesion: 0.14
Nodes (14): P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード, P4-04: Array / ByteArray / String / Symbol (+6 more)

### Community 137 - "FileInError"
Cohesion: 0.26
Nodes (14): FileInError, error, file, method, string, string_view, isVendorStub(), acceptClassDef() (+6 more)

### Community 138 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 139 - ".isTrue()"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 140 - "installNatives()"
Cohesion: 0.25
Nodes (14): allocateSkeletons(), allocClass(), Roots, string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors() (+6 more)

### Community 141 - "Abort / Evaluation Failure Propagation"
Cohesion: 0.16
Nodes (14): Abandon (Process Teardown Without Cleanup), Abort / Evaluation Failure Propagation, BlockContext, Block Closures and Shared Temps, Cooperative Fiber Scheduler, Deadlock Handling, Drain (Evaluation End), ensure: / ifCurtailed: (+6 more)

### Community 142 - "Smalltalk-80 Blue Book"
Cohesion: 0.21
Nodes (14): Association, Smalltalk-80 Blue Book, Boolean / True / False, Class Variables / classPool, Collection Protocol, Dictionary / IdentityDictionary, Smalltalk Global Dictionary, Open-Addressing Hash Table Design (+6 more)

### Community 143 - "InspectorWindow"
Cohesion: 0.22
Nodes (8): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String

### Community 144 - "TEST()"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 145 - "clearUnwinding()"
Cohesion: 0.24
Nodes (13): recordFailure, outOfMemory, Scheduler::runFiber(), clearUnwinding(), CallContext, path, Root, string (+5 more)

### Community 146 - "TEST()"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 147 - "TEST()"
Cohesion: 0.17
Nodes (12): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), KnownGlobalAssignIsError, KnownGlobalReadIsPushGlobal (+4 more)

### Community 148 - "P10-04: セッションのソース表の拡張と doIt"
Cohesion: 0.18
Nodes (12): P10-02: コンパイラの pc→ソース表と temp 名, pc→ソース表と temp 名, フレーム連鎖と abort 時の捕捉（DebugSnapshot）, P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt, P10-04: セッションのソース表の拡張と doIt, セッションのソース表の拡張と doIt, ao_debug_* C ABI, P10-05: ao_debug_* ABI (+4 more)

### Community 149 - "Range"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 150 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 151 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 152 - "P3-04: lookup / super / doesNotUnderstand:"
Cohesion: 0.18
Nodes (11): P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary, P3-03: NativeMethod とセレクタマングル, NativeMethod とセレクタマングル規則, Smalltalk-80 Blue Book（探索意味論）, P3-04: lookup / super / doesNotUnderstand: (+3 more)

### Community 153 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 154 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 155 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext, uint32_t (+2 more)

### Community 156 - "C ABI Bridge (ao_abi.h)"
Cohesion: 0.33
Nodes (11): ao_accept_class, ao_accept_method, ao_eval, ao_image_load, AoSpan, AppKit Host GUI, C ABI Bridge (ao_abi.h), ao_debug_* ABI (+3 more)

### Community 157 - "Debugger (P10 Post-mortem)"
Cohesion: 0.22
Nodes (11): Ao Bytecode Instruction Set, CompiledMethod, Ao Compiler, Method/Block Activation Contexts, Debugger Capture / Snapshot, Debugger (P10 Post-mortem), Frame Chain (CallContext::topFrame), Inline Expansion of Control-Flow Sends (+3 more)

### Community 158 - "TEST()"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 159 - "TEST()"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 160 - "TEST()"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 161 - "TEST()"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 162 - "DebugFrames"
Cohesion: 0.20
Nodes (9): DebugFrames, count, kind, method, pc, receiver, selector, temp (+1 more)

### Community 163 - "Vendor.hpp"
Cohesion: 0.24
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 164 - "FiberRegs"
Cohesion: 0.20
Nodes (10): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+2 more)

### Community 165 - "P2-03: メタクラス循環（Blue Book 6–10）"
Cohesion: 0.22
Nodes (9): P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10）, メタクラス循環, P2-04: Smalltalk グローバル辞書 (+1 more)

### Community 166 - "P0-03: CMake + GoogleTest + CLI"
Cohesion: 0.25
Nodes (9): P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub, P0-06: Serena project (+1 more)

### Community 167 - "WellKnown::define()"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 168 - "StackPool"
Cohesion: 0.22
Nodes (9): array, size_t, kPoolLimit, map, pooledCount, pageBytes(), StackPool, count (+1 more)

### Community 169 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 170 - "TEST()"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 171 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 172 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 173 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 174 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 175 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 176 - "uint32_t"
Cohesion: 0.48
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 177 - "PcSpan"
Cohesion: 0.40
Nodes (6): uint32_t, PcSpan, end, pc, start, debugSpanAt()

### Community 178 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.33
Nodes (6): docs/bench.md, PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 179 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 180 - "cli_test.sh script"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 181 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 182 - "pushFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 183 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 184 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 185 - "BusyRead"
Cohesion: 0.40
Nodes (5): BusyRead, copies, evalCodes, lengths, string

### Community 186 - "Metaclass Circularity (Blue Book Rules 6-10)"
Cohesion: 0.60
Nodes (5): Behavior, Class, ClassDescription, Metaclass, Metaclass Circularity (Blue Book Rules 6-10)

### Community 187 - "TEST()"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 188 - "Roots::visitAll()"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 189 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 190 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 191 - "imageRegistryStubB()"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "answerTwo()"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 193 - "TDD Test Suites (SPEC section 4)"
Cohesion: 0.67
Nodes (4): GC Stress Mode (AO_GC_STRESS), Kernel Scan Test (NativeMethod invariant), Generational Object Memory / GC, TDD Test Suites (SPEC section 4)

### Community 196 - "フェーズの直線依存 (P0→...→P11)"
Cohesion: 0.67
Nodes (3): Ao 実装文書 (docs/ index), フェーズの直線依存 (P0→...→P11), v1 リリース (P9 完了 = 1.0.0)

### Community 198 - "Utf8Step"
Cohesion: 0.67
Nodes (3): Utf8Step, cp, nbytes

### Community 199 - "abortingNew()"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Ambiguous Edges - Review These
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
- `B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool)` → `Compiler MethodImage pcMap + temps (Emitter::mark)`  [AMBIGUOUS]
  docs/superpowers/plans/2026-09-26-p10-debugger.md · relation: conceptually_related_to

## Knowledge Gaps
- **853 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+848 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2350 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **91 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

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
- **What is the exact relationship between `B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool)` and `Compiler MethodImage pcMap + temps (Emitter::mark)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger()`, `TEST()`, `Interpreter.cpp`, `Float.cpp`, `TEST()`, `.isHeap()`, `LargeInteger.cpp`, `Stream.cpp`, `Boot`, `TEST()`, `handlescope`, `TEST()`, `Object.cpp`, `Heap`, `TEST()`, `abi.cpp`, `TEST()`, `Compile.cpp`, `Heap.cpp`, `DebugSnapshot`, `TEST()`, `ImageSave.cpp`, `TEST()`, `Scheduler.cpp`, `TEST()`, `CallContext`, `cstdint`, `ImageLoad.cpp`, `vector`, `gc_safety_test.cpp`, `putNative()`, `Session`, `Session.cpp`, `String.cpp`, `WellKnown.cpp`, `.nil()`, `TEST()`, `.true_()`, `Geometry.cpp`, `TEST()`, `DiskHeader`, `Gc`, `BlockContext.cpp`, `Roots`, `string`, `string_view`, `TEST()`, `TEST_F()`, `Array.cpp`, `Scheduler::Record`, `ClassPool.cpp`, `uint64_t`, `CompileError`, `DebugSnapshot.cpp`, `MethodSource`, `Process.cpp`, `NativeMethod.cpp`, `TEST()`, `Roots.cpp`, `HashedCollection.cpp`, `Fiber.cpp`, `send2()`, `TEST()`, `ImageSurgery`, `TEST()`, `allocateRetry()`, `GarbageFirstBoot`, `native_method_test.cpp`, `size`, `Loaded`, `evalBody()`, `TEST()`, `TEST()`, `Stack`, `TEST()`, `FileInError`, `RootedArray`, `.isTrue()`, `installNatives()`, `clearUnwinding()`, `Range`, `path`, `Table`, `ClassMethodCache`, `CompiledMethodNatives.cpp`, `TEST()`, `TEST()`, `WellKnown::define()`, `Character.cpp`, `ObjectHeader`, `ClassDef`, `WellKnown::InternTable`, `uint32_t`, `PcSpan`, `pushFrame`, `Root`, `imageRegistryStubB()`, `answerTwo()`, `WellKnown::checkSmallIntegerFastPath()`, `abortingNew()`?**
  _High betweenness centrality (0.304) - this node is a cross-community bridge._
# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 291 files · ~301,136 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5868 nodes · 15414 edges · 303 communities (213 shown, 90 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 1872 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `702661c0`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- .fromSmallInteger
- TEST
- CallContext
- Float.cpp
- TEST
- send
- Codegen.cpp
- LargeInteger.cpp
- Parser
- Stream.cpp
- Boot
- TEST
- TEST_F
- Emitter
- Scanner.cpp
- DebuggerWindow
- handlescope
- TEST
- Claude Review Fixes Plan (2026-09-23)
- Oop
- Heap
- TEST
- Record
- BrowserWindow
- abi.cpp
- P6b — vendor file-in
- WorkspaceWindow
- TEST
- Literal
- TEST
- Compile.cpp
- Heap.cpp
- DebugSnapshot
- TEST
- DefinitionScanner
- TEST_F
- ImageSave.cpp
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- TEST
- SmallInteger.cpp
- .nil
- TEST
- TEST_F
- TEST_F
- CallContext
- cstdint
- ImageLoad.cpp
- Scheduler
- Scheduler.cpp
- main
- putNative
- Session
- SPEC.md (Product Specification)
- Session.cpp
- ToolWindowTests
- Interpreter::run
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- VendorExtract.cpp
- AoApp
- WellKnown.cpp
- .run
- BrowserModel
- DebuggerWindowTests
- .isHeap
- P6b Vendor File-in Implementation Plan
- TEST
- .false_
- Geometry.cpp
- AoSpan
- TEST
- DiskHeader
- Gc.cpp
- TEST
- BlockContext.cpp
- Roots
- string
- size
- TEST
- TEST_F
- Array.cpp
- Scheduler::Record
- ClassPool.cpp
- ChunkParser.cpp
- FiberStack
- LiveFrames
- stream_test.cpp
- TEST
- uint64_t
- TranscriptWindow
- ChunkAction
- Boolean.cpp
- ImageFormat
- DebugSnapshot.cpp
- MethodSource
- image_save_load_test.cpp
- TEST
- TEST
- NativeMethod.cpp
- TEST
- TEST
- Process.cpp
- Roots.cpp
- HashedCollection.cpp
- Fiber.cpp
- P5 — コンパイラ
- CountingSink
- send2
- TEST
- .publish
- FiberRegs
- ImageSurgery
- TEST
- MainMenu
- Ast
- sendToKeyWorkspace
- BrowserModelTests
- ao_runtime library target
- CLAUDE.md
- native_method_test.cpp
- TEST
- TEST
- Frame
- Qiita-part2.md
- TestRunner.cpp
- VirtualRegion.cpp
- Loaded
- Phase Progression P0-P11
- blankOut
- = and hash Contract
- TEST
- TEST
- TEST
- Stack
- DebugSnapshot::capture (root-safe, no heap allocation)
- TEST
- P4 — Kernelネイティブ実装
- vector
- GarbageFirstBoot
- .isTrue
- Bootstrap.cpp
- Abort / Evaluation Failure Propagation
- Smalltalk-80 Blue Book
- AppKit
- TEST
- sessionImageLoad
- TEST
- reshapeClass
- P10 — 事後デバッガ
- FrameBlock
- path
- Token
- string
- P3 — ネイティブディスパッチ
- Temps
- TEST
- C ABI Bridge (ao_abi.h)
- Debugger (P10 Post-mortem)
- TEST
- specialIndex
- Rec
- ClassMethodCache
- P11-04: Proceed / Abort
- P9 — 統合
- synthesizesNative
- P2 — ブートストラップ
- Ao 実装文書 (docs/ index)
- InspectorWindow
- P11-02: 評価プロセス
- P1 — オブジェクトメモリ
- TEST
- ObjectHeader
- Counts
- P11-07: 受け入れ、CHANGELOG、Graphify deep
- Parsed
- TEST
- ao_runtime_shutdown
- Table
- P8–P9 Remaining Implementation Plan
- FileSizeLimit
- cli_test.sh
- P11-05: Step into / over / out と Debug it
- popFrame
- OperandStack
- TestDir
- Frame
- Metaclass Circularity (Blue Book Rules 6-10)
- compiler/tests/smoke_test.cpp
- Roots::visitAll
- P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数
- Root
- Interpreter.cpp
- VendorClassFile
- TDD Test Suites (SPEC section 4)
- ao-smalltalk/AGENTS.md
- P8 — AppKit ツール
- CompiledMethodNatives.cpp
- Qiita-part1.md
- DebugFrames
- string
- build.sh
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
- package-app.sh
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
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- Kernel Instance Variable Read-Only Rule
- Message
- MethodDictionary
- KeptMethod
- Bootstrap
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- IgnoreFileSizeSignal
- WellKnown::named
- uint32_t
- ScopedGcStressEnv
- TEST
- codegen
- abortingSubclass
- imageRegistryStubA
- answerOne
- .specialSelector
- DepthGuard
- abortingNew

## God Nodes (most connected - your core abstractions)
1. `Oop` - 937 edges
2. `Heap` - 252 edges
3. `vector` - 176 edges
4. `WellKnown` - 167 edges
5. `TEST_F()` - 109 edges
6. `TEST()` - 107 edges
7. `Ast` - 103 edges
8. `TEST()` - 101 edges
9. `Boot` - 101 edges
10. `Session` - 100 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `Interfaces` --references--> `DebugFrames`  [INFERRED]
  docs/prs/P11-01.md → runtime/include/ao/DebugSnapshot.hpp
- `手順` --references--> `sessionEval()`  [INFERRED]
  docs/prs/P11-04.md → runtime/src/Session.cpp
- `目的` --references--> `ao_eval()`  [INFERRED]
  docs/prs/P11-02.md → runtime/src/abi.cpp
- `Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系` --references--> `ao_SmallInteger_add()`  [INFERRED]
  docs/Qiita-part1.md → runtime/src/kernel/SmallInteger.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **Interpreted frame chain to abort snapshot to C ABI to Debugger window** — docs_superpowers_plans_2026_09_26_p10_debugger_frame_chain, docs_superpowers_plans_2026_09_26_p10_debugger_debugsnapshot, docs_superpowers_plans_2026_09_26_p10_debugger_debug_abi, docs_superpowers_plans_2026_09_26_p10_debugger_debugger_window [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **P0 skeleton deliverables** — bridge_ao_abi, serena_project [EXTRACTED 1.00]
- **P10 事後デバッガの構成要素 (フレーム連鎖→スナップショット→Debugger窓)** — docs_phases_p10_callcontext, docs_phases_p10_debugsnapshot, docs_phases_p10_debuggerwindow [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
- **失敗が黙って成功(AO_OK)になるパターン** — docs_claude_review_00_recent_diff_classdef_string_silently_dropped, docs_claude_review_01_object_memory_method_dict_grow_fail_silently_drops_method, docs_claude_review_06_image_session_abi_vendor_filein_errors_reported_as_success, docs_claude_review_02_interpreter_empty_oop_pushed_as_value_becomes_message [EXTRACTED 1.00]
- **Debugger Read Pipeline** — spec_debugger, spec_debug_capture, spec_frame_chain, spec_debug_abi [EXTRACTED 1.00]
- **Metaclass Circularity Cycle** — spec_object, spec_behavior, spec_class, spec_metaclass [EXTRACTED 1.00]
- **Cooperative Process Scheduling Group** — spec_process, spec_processor_scheduler, spec_semaphore, spec_shared_queue [EXTRACTED 1.00]
- **Vendor Loading Kernel-Integrity Safeguards** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_allowlist, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_native_overwrite_refusal, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_kernel_scan_narrowing, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_host_patch [INFERRED 0.80]
- **Vendor File-in to Image Snapshot Pipeline** — docs_superpowers_plans_2026_09_22_p6b_vendor_filein_load_order, docs_superpowers_plans_2026_09_22_p6b_vendor_filein_link_class, runtime_src_ao_main_image_cli, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.80]
- **AppKit ツールウィンドウ群とメニュー** — docs_prs_p8_01_ao_app_skeleton, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [INFERRED 0.80]
- **Vendor extraction, deferral list, and file-in test form one pipeline** — image_vendor_origin_extract_process, image_vendor_deferred_doc, runtime_tests_vendor_filein_test [INFERRED 0.80]
- **aoimage Method Persistence Across Restart** — docs_prs_p9_04_aoimage_restart, docs_superpowers_plans_2026_09_22_p7_aoimage_save_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_load_procedure, docs_superpowers_plans_2026_09_22_p7_aoimage_native_rebind_by_name, docs_superpowers_plans_2026_09_22_p7_aoimage_link_survives [INFERRED 0.85]
- **P11 ライブデバッガ操作 (評価プロセス→Scheduler停止復帰→Debugger窓操作ボタン)** — docs_phases_p11_evalprocess, docs_phases_p11_scheduler, docs_phases_p10_debuggerwindow [INFERRED 0.85]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — docs_prs_p1_02_pr, docs_prs_p1_03_pr, docs_prs_p1_04_pr, docs_prs_p1_05_pr, docs_prs_p1_06_pr, docs_prs_p1_07_pr [INFERRED 0.85]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — docs_prs_p4_01_doc, docs_prs_p4_02_doc, docs_prs_p4_03_doc, docs_prs_p4_04_doc, docs_prs_p4_05_doc, docs_prs_p4_06_doc, docs_prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunkparser [INFERRED 0.85]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **v1 Non-goals Stated Consistently Across Docs** — changelog_known_limitations_p10, readme_not_in_v1, contributing_rules_that_do_not_bend [INFERRED 0.85]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **P10 デバッガフェーズの実装フロー（コンパイラ pc 表 → 捕捉 → セッション → ABI → GUI → 受け入れ）** — docs_prs_p10_02_doc, docs_prs_p10_03_doc, docs_prs_p10_04_doc, docs_prs_p10_05_doc, docs_prs_p10_06_doc, docs_prs_p10_07_doc [INFERRED 0.95]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]

## Communities (303 total, 90 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (88): OcShape, Pass, probe, visit, allocateInstance(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 2 - "TEST"
Cohesion: 0.02
Nodes (84): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+76 more)

### Community 3 - "CallContext"
Cohesion: 0.19
Nodes (11): run, ActiveGuard, rootShared, saved, applyMethod(), contextAlive(), CallContext, interpreterRunning() (+3 more)

### Community 4 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 5 - "TEST"
Cohesion: 0.03
Nodes (78): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+70 more)

### Community 6 - "send"
Cohesion: 0.09
Nodes (65): InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan() (+57 more)

### Community 7 - "Codegen.cpp"
Cohesion: 0.05
Nodes (57): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+49 more)

### Community 8 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 9 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 10 - "Stream.cpp"
Cohesion: 0.05
Nodes (116): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), CallContext, uint16_t (+108 more)

### Community 11 - "Boot"
Cohesion: 0.05
Nodes (62): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+54 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 13 - "TEST_F"
Cohesion: 0.03
Nodes (69): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+61 more)

### Community 14 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+7 more)

### Community 15 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 16 - "DebuggerWindow"
Cohesion: 0.06
Nodes (47): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+39 more)

### Community 17 - "handlescope"
Cohesion: 0.15
Nodes (24): chrono, Chunk, classpool, cmath, compile, CompiledMethod, compiler, context (+16 more)

### Community 18 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 19 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.13
Nodes (24): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+16 more)

### Community 20 - "Oop"
Cohesion: 0.09
Nodes (65): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+57 more)

### Community 21 - "Heap"
Cohesion: 0.04
Nodes (45): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+37 more)

### Community 22 - "TEST"
Cohesion: 0.04
Nodes (53): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+45 more)

### Community 23 - "Record"
Cohesion: 0.12
Nodes (29): afterResume, canHalt, endEval, enqueue, find, leaveLists, reapDead, recordFailure (+21 more)

### Community 24 - "BrowserWindow"
Cohesion: 0.12
Nodes (19): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, AcceptTests (+11 more)

### Community 25 - "abi.cpp"
Cohesion: 0.08
Nodes (50): atomic, Body, CountsAnswerMinusOneOnFailure, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at(), ao_browser_class_count(), ao_browser_class_definition() (+42 more)

### Community 26 - "P6b — vendor file-in"
Cohesion: 0.15
Nodes (20): NativeMethod — ネイティブメソッドディスパッチ, vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, NativeMethod のシンボル名再結合 (イメージロード時), P4-09: Kernel NativeMethod 走査と bench, Kernel NativeMethod 走査, P6b-01: Cuis pin と ORIGIN.md (+12 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.13
Nodes (12): WorkspaceWindow, .errorAccessibilityLabel, .errorText, .inspectorCount, .inspectorText, .inspectorWindow, .textAccessibilityLabel, .title (+4 more)

### Community 28 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 29 - "Literal"
Cohesion: 0.07
Nodes (32): int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal (+24 more)

### Community 30 - "TEST"
Cohesion: 0.04
Nodes (51): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+43 more)

### Community 31 - "Compile.cpp"
Cohesion: 0.13
Nodes (32): cctype, CompileError, message, span, string, acceptClassSource(), applyChunks(), assignError() (+24 more)

### Community 32 - "Heap.cpp"
Cohesion: 0.09
Nodes (44): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+36 more)

### Community 33 - "DebugSnapshot"
Cohesion: 0.06
Nodes (28): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+20 more)

### Community 34 - "TEST"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 35 - "DefinitionScanner"
Cohesion: 0.10
Nodes (19): DefinitionScanner, Any, Bool, Int, Int32, NSRect, NSScrollView, NSTableColumn (+11 more)

### Community 36 - "TEST_F"
Cohesion: 0.03
Nodes (81): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+73 more)

### Community 37 - "ImageSave.cpp"
Cohesion: 0.10
Nodes (42): cerrno, climits, fcntl, appendRaw(), appendRecord(), collectImageSlot(), byte, Roots (+34 more)

### Community 38 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (46): B2 to:do: bench（jump 展開ループ）, bytecode/native 比率メトリクス, SmallInteger fast path（send を経ない演算）, P4 microbench（native to:do: 基準）, P6 interpreter bench（bytecode block send）, 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる (+38 more)

### Community 39 - "TEST"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 40 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (29): safepoint, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual() (+21 more)

### Community 41 - ".nil"
Cohesion: 0.14
Nodes (39): flags, slotAt, slotAtPut, int64_t, addFiber, liveFibers, contextPc(), CallContext (+31 more)

### Community 42 - "TEST"
Cohesion: 0.08
Nodes (26): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+18 more)

### Community 43 - "TEST_F"
Cohesion: 0.04
Nodes (51): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+43 more)

### Community 44 - "TEST_F"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 45 - "CallContext"
Cohesion: 0.05
Nodes (44): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+36 more)

### Community 46 - "cstdint"
Cohesion: 0.07
Nodes (20): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, Gc (+12 more)

### Community 47 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (35): ifstream, readHeader, acceptWord(), atOffset(), checkFile(), checkGlobals(), byte, Roots (+27 more)

### Community 48 - "Scheduler"
Cohesion: 0.04
Nodes (45): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+37 more)

### Community 49 - "Scheduler.cpp"
Cohesion: 0.09
Nodes (39): abandonAll, awaitEval, drain, findId, isHalted, proceed, runAwaited, terminate (+31 more)

### Community 50 - "main"
Cohesion: 0.31
Nodes (9): addRoots, bootAndRunTests(), string, imageUsage(), main(), runExtractVendor(), runImage(), testsBesideExecutable() (+1 more)

### Community 51 - "putNative"
Cohesion: 0.08
Nodes (36): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+28 more)

### Community 52 - "Session"
Cohesion: 0.06
Nodes (45): 手順, attachBlocks(), blockLiteral(), int64_t, MethodImage, Roots, size_t, debugCanProceed() (+37 more)

### Community 53 - "SPEC.md (Product Specification)"
Cohesion: 0.11
Nodes (21): ao_debug_* / ao_set_debug_capture C ABI, Post-mortem Debugger Capture, P10 Known Limitations, Unreleased: Phase P10 Debugger, 1.0.0 Release Entry, Fixed Design Decisions, PHASE File, 曲げない規則 (+13 more)

### Community 54 - "Session.cpp"
Cohesion: 0.17
Nodes (30): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+22 more)

### Community 55 - "ToolWindowTests"
Cohesion: 0.14
Nodes (10): LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String, T (+2 more)

### Community 56 - "Interpreter::run"
Cohesion: 0.20
Nodes (16): boolean(), byteCount(), int16_t, int64_t, uint32_t, uint8_t, decodeHeader(), instSlot() (+8 more)

### Community 57 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 58 - "VendorExtract.cpp"
Cohesion: 0.17
Nodes (28): allowIndex(), containsHostWord(), string_view, doubleBangs(), extractVendor(), firstLineKey(), firstNonEmptyLine(), HostMethod (+20 more)

### Community 59 - "AoApp"
Cohesion: 0.11
Nodes (15): AoApp, fileInVendor(), openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification (+7 more)

### Community 60 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (38): internWith, isFixedGlobal, isPseudoVariableName, WellKnown, intern(), deque, Roots, size_t (+30 more)

### Community 61 - ".run"
Cohesion: 0.15
Nodes (16): aoWorkspaceInspectHook(), failureText(), keptEvalResult(), selectErrorSpan(), spanMessage(), CChar, Int, Int32 (+8 more)

### Community 62 - "BrowserModel"
Cohesion: 0.21
Nodes (8): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, UnsafeMutablePointer

### Community 63 - "DebuggerWindowTests"
Cohesion: 0.11
Nodes (14): DebuggerWindowTests, Int, NSButton, NSFont, NSTableView, NSTextView, NSView, NSWindow (+6 more)

### Community 64 - ".isHeap"
Cohesion: 0.13
Nodes (22): uint32_t, isClassShaped(), superclassOf(), SuperclassWalk, depth_, anyMethodIn(), copyMethodDictionary(), CallContext (+14 more)

### Community 65 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.09
Nodes (32): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+24 more)

### Community 66 - "TEST"
Cohesion: 0.08
Nodes (29): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+21 more)

### Community 67 - ".false_"
Cohesion: 0.15
Nodes (34): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+26 more)

### Community 68 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+20 more)

### Community 69 - "AoSpan"
Cohesion: 0.10
Nodes (30): AoSpan, end, message, start, Files, ao_accept_class(), ao_accept_method(), ao_debug_proceed() (+22 more)

### Community 70 - "TEST"
Cohesion: 0.05
Nodes (39): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+31 more)

### Community 71 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 72 - "Gc.cpp"
Cohesion: 0.09
Nodes (31): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, clearWeakAfterNursery, clearWeakAfterOldMark, collectNursery (+23 more)

### Community 73 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 74 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 75 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 76 - "string"
Cohesion: 0.18
Nodes (17): byteText(), categoryHeading(), ClassRow, category, cls, name, classVarList(), collectKnownGlobals() (+9 more)

### Community 77 - "size"
Cohesion: 0.19
Nodes (18): allocateNoGc, size, at(), bind(), bindIn(), Roots, string_view, uint32_t (+10 more)

### Community 78 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 79 - "TEST_F"
Cohesion: 0.06
Nodes (31): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+23 more)

### Community 80 - "Array.cpp"
Cohesion: 0.18
Nodes (24): valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+16 more)

### Community 81 - "Scheduler::Record"
Cohesion: 0.08
Nodes (26): Scheduler, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending, evalMode (+18 more)

### Community 82 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (23): adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view, uint32_t (+15 more)

### Community 83 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 84 - "FiberStack"
Cohesion: 0.10
Nodes (26): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), array, byte (+18 more)

### Community 85 - "LiveFrames"
Cohesion: 0.09
Nodes (20): CallContext, DebugSink, onAbort, Frame, size_t, string, LiveFrames, context (+12 more)

### Community 86 - "stream_test.cpp"
Cohesion: 0.06
Nodes (41): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+33 more)

### Community 87 - "TEST"
Cohesion: 0.11
Nodes (16): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+8 more)

### Community 88 - "uint64_t"
Cohesion: 0.23
Nodes (16): bindAll(), uint64_t, unordered_map, fileOop(), headerAt(), heapShaped(), bits, ObjectRules (+8 more)

### Community 89 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+20 more)

### Community 90 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 91 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 92 - "ImageFormat"
Cohesion: 0.08
Nodes (26): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, encodeNonHeap (+18 more)

### Community 93 - "DebugSnapshot.cpp"
Cohesion: 0.16
Nodes (26): frameAt, uint32_t, DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method(), DebugSnapshot::pc(), DebugSnapshot::process(), DebugSnapshot::receiver() (+18 more)

### Community 94 - "MethodSource"
Cohesion: 0.07
Nodes (34): uint32_t, PcSpan, end, pc, start, Interfaces, DebugInfo, bodies (+26 more)

### Community 95 - "image_save_load_test.cpp"
Cohesion: 0.15
Nodes (13): csignal, iterator, mach, mach_vm, resource, ao_runtime_tests executable target, gcstress ctest target (AO_GC_STRESS=1, excludes KernelBench.*), googletest v1.15.2 FetchContent dependency (+5 more)

### Community 96 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 97 - "TEST"
Cohesion: 0.14
Nodes (18): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, allocate, setGcStress, Heap::allocateNoGc() (+10 more)

### Community 98 - "NativeMethod.cpp"
Cohesion: 0.22
Nodes (19): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+11 more)

### Community 99 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 100 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 101 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 102 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 103 - "HashedCollection.cpp"
Cohesion: 0.22
Nodes (21): hashedcollection, bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t (+13 more)

### Community 104 - "Fiber.cpp"
Cohesion: 0.16
Nodes (18): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 105 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 106 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 107 - "send2"
Cohesion: 0.11
Nodes (19): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+11 more)

### Community 108 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 109 - ".publish"
Cohesion: 0.12
Nodes (7): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, CAo

### Community 110 - "FiberRegs"
Cohesion: 0.06
Nodes (36): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+28 more)

### Community 111 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 112 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 113 - "MainMenu"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, Selector, String, Void

### Community 114 - "Ast"
Cohesion: 0.09
Nodes (23): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+15 more)

### Community 115 - "sendToKeyWorkspace"
Cohesion: 0.13
Nodes (14): AnyObject, installDebugButton(), installErrorField(), sendToKeyWorkspace(), Bool, MainActor, NSButton, NSTextField (+6 more)

### Community 116 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 117 - "ao_runtime library target"
Cohesion: 0.11
Nodes (20): B7: Compiler syntax and chunk format fixes, ao_fiber_switch AArch64 asm context switch, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12) (+12 more)

### Community 118 - "CLAUDE.md"
Cohesion: 0.20
Nodes (12): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps), AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target), リリース手順 (JA) (+4 more)

### Community 119 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 120 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 121 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 122 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 123 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 124 - "TestRunner.cpp"
Cohesion: 0.29
Nodes (12): ao_AoTest_assert_equals_(), CallContext, path, Root, string, string_view, uint32_t, listTests() (+4 more)

### Community 125 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 126 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 127 - "Phase Progression P0-P11"
Cohesion: 0.16
Nodes (17): Ao Smalltalk System, ao_image_save, Bootstrap Procedure, Cuis Smalltalk, DEFERRED.md Exclusion List, Graphify / Serena Mandatory Tooling, Image Format .aoimage, Image Load Validation Rules (+9 more)

### Community 128 - "blankOut"
Cohesion: 0.19
Nodes (16): Interfaces, answerAwaited(), answerEval(), blankOut(), AoInspectFn, EvalEnd, StepMode, DebugEntry (+8 more)

### Community 129 - "= and hash Contract"
Cohesion: 0.14
Nodes (16): Array, ByteArray, Character, = and hash Contract, Float (IEEE754 binary64), Fraction, Magnitude, Number / Integer Hierarchy (+8 more)

### Community 130 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 131 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 132 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 133 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 134 - "DebugSnapshot::capture (root-safe, no heap allocation)"
Cohesion: 0.18
Nodes (14): ClassMethodCache::flushSelector/flushAll, Stack guard + abort as homeless NLR, ao_debug_* C ABI, Ao.app DebuggerWindow + Workspace Debug button, DebugSnapshot::capture (root-safe, no heap allocation), Frame linked list (CallContext::topFrame), GRAPH_REPORT.md god nodes/communities (2026-09-26), Object>>halt native (ao_Object_halt) (+6 more)

### Community 135 - "TEST"
Cohesion: 0.13
Nodes (21): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, uint32_t, firstBlock(), named() (+13 more)

### Community 136 - "P4 — Kernelネイティブ実装"
Cohesion: 0.15
Nodes (17): P4 — Kernelネイティブ実装, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装, P4-02: Behavior / ClassDescription / Class / Metaclass, P4-03: Magnitude / SmallInteger / Character, SmallInteger 算術のネイティブ実装, Array / ByteArray / String の可変長ペイロード (+9 more)

### Community 137 - "vector"
Cohesion: 0.17
Nodes (25): vector, FileInError, error, file, method, string, string_view, isVendorStub() (+17 more)

### Community 138 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 139 - ".isTrue"
Cohesion: 0.49
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 140 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (24): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+16 more)

### Community 141 - "Abort / Evaluation Failure Propagation"
Cohesion: 0.16
Nodes (14): Abandon (Process Teardown Without Cleanup), Abort / Evaluation Failure Propagation, BlockContext, Block Closures and Shared Temps, Cooperative Fiber Scheduler, Deadlock Handling, Drain (Evaluation End), ensure: / ifCurtailed: (+6 more)

### Community 142 - "Smalltalk-80 Blue Book"
Cohesion: 0.21
Nodes (14): Association, Smalltalk-80 Blue Book, Boolean / True / False, Class Variables / classPool, Collection Protocol, Dictionary / IdentityDictionary, Smalltalk Global Dictionary, Open-Addressing Hash Table Design (+6 more)

### Community 143 - "AppKit"
Cohesion: 0.17
Nodes (10): Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, transcriptBoxHook(), SmokeTests, AppKit (+2 more)

### Community 144 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 145 - "sessionImageLoad"
Cohesion: 0.10
Nodes (21): Image, check, load, save, bumpDebugGeneration(), clearMethodSources(), CallContext, HostOopHook (+13 more)

### Community 146 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 147 - "reshapeClass"
Cohesion: 0.11
Nodes (28): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+20 more)

### Community 148 - "P10 — 事後デバッガ"
Cohesion: 0.10
Nodes (26): CallContext::topFrame — 解釈フレーム連鎖, Debugger 窓 (frames / source / variables), abort スナップショット / ao_debug_* ABI, P10 — 事後デバッガ, 評価プロセス (doIt を fork したプロセスで実行), P11 — ライブデバッガ, Scheduler (terminate / resumeTo / switchTo), P10-01: SPEC と CLAUDE.md の改訂、PHASE (+18 more)

### Community 149 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 150 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 151 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 152 - "string"
Cohesion: 0.23
Nodes (18): size_t, string, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars (+10 more)

### Community 153 - "P3 — ネイティブディスパッチ"
Cohesion: 0.19
Nodes (13): MethodDictionary / lookup (IC→class cache→辞書→DNU), P3 — ネイティブディスパッチ, P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary, P3-03: NativeMethod とセレクタマングル, NativeMethod とセレクタマングル規則 (+5 more)

### Community 154 - "Temps"
Cohesion: 0.22
Nodes (8): Roots, uint32_t, unique_ptr, Temps, n, roots, slots, remoteSlot()

### Community 155 - "TEST"
Cohesion: 0.13
Nodes (13): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+5 more)

### Community 156 - "C ABI Bridge (ao_abi.h)"
Cohesion: 0.33
Nodes (11): ao_accept_class, ao_accept_method, ao_eval, ao_image_load, AoSpan, AppKit Host GUI, C ABI Bridge (ao_abi.h), ao_debug_* ABI (+3 more)

### Community 157 - "Debugger (P10 Post-mortem)"
Cohesion: 0.22
Nodes (11): Ao Bytecode Instruction Set, CompiledMethod, Ao Compiler, Method/Block Activation Contexts, Debugger Capture / Snapshot, Debugger (P10 Post-mortem), Frame Chain (CallContext::topFrame), Inline Expansion of Control-Flow Sends (+3 more)

### Community 158 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 159 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 160 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

### Community 161 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 162 - "P11-04: Proceed / Abort"
Cohesion: 0.25
Nodes (7): Interfaces, P11-04: Proceed / Abort, コミット文, テスト, 完了条件, 手順, 目的

### Community 163 - "P9 — 統合"
Cohesion: 0.20
Nodes (12): P9 — 統合, P9-01: Do it / Print it / Inspect it, Do it / Print it / Inspect it, P9-02: Browser accept, Browser accept, P9-03: エラー表示と VoiceOver, エラー表示と VoiceOver, P9-04 v1 Golden Acceptance (+4 more)

### Community 164 - "synthesizesNative"
Cohesion: 0.44
Nodes (9): CallContext, Frame, string, DebugSnapshot::capture(), hasSendInFlight(), LiveFrames::LiveFrames(), sendTarget(), sentToSuper() (+1 more)

### Community 165 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 166 - "Ao 実装文書 (docs/ index)"
Cohesion: 0.24
Nodes (16): P0 phase doc, P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke (+8 more)

### Community 167 - "InspectorWindow"
Cohesion: 0.22
Nodes (8): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String

### Community 168 - "P11-02: 評価プロセス"
Cohesion: 0.25
Nodes (7): Files, P11-02: 評価プロセス, コミット文, テスト, 完了条件, 目的, setSessionDebugMode()

### Community 169 - "P1 — オブジェクトメモリ"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 170 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 171 - "ObjectHeader"
Cohesion: 0.33
Nodes (6): uint16_t, ObjectHeader, flags, hash, klass, size

### Community 172 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 173 - "P11-07: 受け入れ、CHANGELOG、Graphify deep"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P11-07: 受け入れ、CHANGELOG、Graphify deep, コミット文, テスト, 完了条件, 手順, 目的

### Community 174 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 175 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 176 - "ao_runtime_shutdown"
Cohesion: 0.07
Nodes (30): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, ao_filein_load_order(), ao_image_save(), ao_runtime_shutdown(), BrowserAbi, testing::Test (+22 more)

### Community 177 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 178 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.33
Nodes (6): docs/bench.md, PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 179 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 180 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 181 - "P11-05: Step into / over / out と Debug it"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P11-05: Step into / over / out と Debug it, コミット文, テスト, 完了条件, 目的

### Community 182 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 183 - "OperandStack"
Cohesion: 0.24
Nodes (6): deque, OperandStack, roots, slots, answerWithoutSend(), WellKnown

### Community 184 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 185 - "Frame"
Cohesion: 0.29
Nodes (7): ContextExitGuard, Frame, Roots, FieldRoots, frame, FrameLink, stepCheck()

### Community 186 - "Metaclass Circularity (Blue Book Rules 6-10)"
Cohesion: 0.60
Nodes (5): Behavior, Class, ClassDescription, Metaclass, Metaclass Circularity (Blue Book Rules 6-10)

### Community 187 - "compiler/tests/smoke_test.cpp"
Cohesion: 0.40
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 188 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 189 - "P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数, コミット文, テスト, 完了条件, 手順, 目的

### Community 190 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 191 - "Interpreter.cpp"
Cohesion: 0.47
Nodes (9): branchTruth(), clearNonlocal(), consumeNonlocal(), hit(), Leave, leave, value, miss() (+1 more)

### Community 192 - "VendorClassFile"
Cohesion: 0.25
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 193 - "TDD Test Suites (SPEC section 4)"
Cohesion: 0.67
Nodes (4): GC Stress Mode (AO_GC_STRESS), Kernel Scan Test (NativeMethod invariant), Generational Object Memory / GC, TDD Test Suites (SPEC section 4)

### Community 194 - "ao-smalltalk/AGENTS.md"
Cohesion: 0.67
Nodes (3): ao-smalltalk/AGENTS.md, CLAUDE.md process authority, SPEC.md product authority

### Community 195 - "P8 — AppKit ツール"
Cohesion: 0.29
Nodes (11): P8 — AppKit ツール, P8-01: Ao.app 骨格, Ao.app 骨格, P8-02: Transcript ウィンドウ, Transcript ウィンドウ, P8-03: Workspace ウィンドウ, Workspace ウィンドウ, P8-04: System Browser 5 ペイン (+3 more)

### Community 196 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext, uint32_t (+2 more)

### Community 197 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 198 - "DebugFrames"
Cohesion: 0.09
Nodes (20): DebugFrames, context, count, empty, kind, method, pc, receiver (+12 more)

### Community 199 - "string"
Cohesion: 0.10
Nodes (17): string, cstdio, cstring, dyld, filesystem, fstream, future, runtime (+9 more)

### Community 288 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 289 - "Bootstrap"
Cohesion: 0.16
Nodes (15): algorithm, Bootstrap, MethodDictionary, CallContext, uint32_t, stubA(), stubB(), answerMessage() (+7 more)

### Community 290 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.20
Nodes (7): Files, Interfaces, P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告, コミット文, 完了条件, 手順, 目的

### Community 291 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 292 - "WellKnown::named"
Cohesion: 0.67
Nodes (3): findSymbol, global, WellKnown::named()

### Community 294 - "uint32_t"
Cohesion: 0.11
Nodes (31): 手順, argCountOf(), classNameOf(), uint32_t, debugFrameCount(), debugFrameKind(), debugFrameLabel(), debugFramePc() (+23 more)

### Community 297 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 298 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 299 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 302 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 303 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 305 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 308 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Ambiguous Edges - Review These
- `P5-01: 字句解析` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P5-01.md · relation: references
- `Compiler MethodImage pcMap + temps (Emitter::mark)` → `B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool)`  [AMBIGUOUS]
  docs/superpowers/plans/2026-09-26-p10-debugger.md · relation: conceptually_related_to
- `P4-08: Point / Rectangle` → `P4-09: Kernel NativeMethod 走査と bench`  [AMBIGUOUS]
  docs/prs/P4-09.md · relation: references
- `P9-01: Do it / Print it / Inspect it` → `P8-05: メニューとキー`  [AMBIGUOUS]
  docs/prs/P9-01.md · relation: references
- `P8-01: Ao.app 骨格` → `P7-03: load と NativeMethod 再結合`  [AMBIGUOUS]
  docs/prs/P8-01.md · relation: references
- `P6b-04: vendor file-in` → `P7-01: .aoimage 形式`  [AMBIGUOUS]
  docs/prs/P7-01.md · relation: references

## Knowledge Gaps
- **955 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+950 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2512 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **90 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `P5-01: 字句解析` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Compiler MethodImage pcMap + temps (Emitter::mark)` and `B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `P4-08: Point / Rectangle` and `P4-09: Kernel NativeMethod 走査と bench`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P9-01: Do it / Print it / Inspect it` and `P8-05: メニューとキー`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P8-01: Ao.app 骨格` and `P7-03: load と NativeMethod 再結合`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `P6b-04: vendor file-in` and `P7-01: .aoimage 形式`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `TEST`, `CallContext`, `Float.cpp`, `TEST`, `send`, `LargeInteger.cpp`, `Stream.cpp`, `Boot`, `TEST`, `TEST_F`, `handlescope`, `TEST`, `Heap`, `TEST`, `Record`, `abi.cpp`, `TEST`, `Compile.cpp`, `Heap.cpp`, `DebugSnapshot`, `TEST`, `ImageSave.cpp`, `TEST`, `SmallInteger.cpp`, `.nil`, `TEST`, `CallContext`, `cstdint`, `ImageLoad.cpp`, `Scheduler`, `Scheduler.cpp`, `putNative`, `Session`, `Session.cpp`, `Interpreter::run`, `WellKnown.cpp`, `.isHeap`, `TEST`, `.false_`, `Geometry.cpp`, `TEST`, `DiskHeader`, `Gc.cpp`, `BlockContext.cpp`, `Roots`, `string`, `size`, `TEST`, `TEST_F`, `Array.cpp`, `Scheduler::Record`, `ClassPool.cpp`, `LiveFrames`, `stream_test.cpp`, `uint64_t`, `Boolean.cpp`, `DebugSnapshot.cpp`, `MethodSource`, `TEST`, `NativeMethod.cpp`, `TEST`, `Process.cpp`, `Roots.cpp`, `HashedCollection.cpp`, `Fiber.cpp`, `CountingSink`, `send2`, `TEST`, `ImageSurgery`, `native_method_test.cpp`, `Frame`, `TestRunner.cpp`, `Loaded`, `blankOut`, `TEST`, `TEST`, `Stack`, `vector`, `GarbageFirstBoot`, `.isTrue`, `Bootstrap.cpp`, `sessionImageLoad`, `reshapeClass`, `FrameBlock`, `path`, `Temps`, `TEST`, `ClassMethodCache`, `synthesizesNative`, `ObjectHeader`, `TEST`, `Table`, `popFrame`, `OperandStack`, `Root`, `Interpreter.cpp`, `CompiledMethodNatives.cpp`, `string`, `Bootstrap`, `WellKnown::named`, `uint32_t`, `abortingSubclass`, `imageRegistryStubA`, `answerOne`, `.specialSelector`, `abortingNew`?**
  _High betweenness centrality (0.362) - this node is a cross-community bridge._
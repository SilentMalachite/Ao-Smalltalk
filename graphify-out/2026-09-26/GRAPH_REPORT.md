# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 291 files · ~288,918 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5596 nodes · 14608 edges · 304 communities (214 shown, 90 thin omitted)
- Extraction: 88% EXTRACTED · 12% INFERRED · 0% AMBIGUOUS · INFERRED: 1700 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `b89dde7e`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- .fromSmallInteger
- TEST
- Interpreter.cpp
- Float.cpp
- TEST
- send
- RealScope
- LargeInteger.cpp
- Parser
- Stream.cpp
- Boot
- TEST
- TEST_F
- Emitter
- Scanner.cpp
- DebuggerWindow
- vector
- TEST
- Claude Review Fixes Plan (2026-09-23)
- Oop
- Heap
- TEST
- Boolean.cpp
- BrowserWindow
- abi.cpp
- Ao 実装文書 (docs/ index)
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
- Image::save
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- TEST
- P1 — オブジェクトメモリ
- Scheduler.cpp
- Frame
- TEST_F
- TEST_F
- CallContext
- cstdint
- ImageLoad.cpp
- Scheduler
- MethodDebugInfo
- ao_main.cpp
- putNative
- Session
- SPEC.md (Product Specification)
- classRows
- ToolWindowTests
- .isHeap
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- String.cpp
- AoApp
- WellKnown.cpp
- .run
- BrowserModel
- DebuggerWindowTests
- Gc.cpp
- VendorExtract.cpp
- TEST
- allocateRetry
- Geometry.cpp
- TEST
- TEST
- DiskHeader
- collectNursery
- CompileEnv
- BlockContext.cpp
- Roots
- string
- CallContext
- TEST
- ao_eval
- Array.cpp
- Scheduler::Record
- AppKit
- ChunkParser.cpp
- FiberStack
- SmallInteger.cpp
- TEST_F
- TEST
- uint64_t
- TranscriptWindow
- ChunkAction
- string
- ImageFormat
- DebugSnapshot.cpp
- MethodSource
- TEST
- TEST
- Process.cpp
- NativeMethod.cpp
- TEST
- TEST
- ImageHeader
- Roots.cpp
- .isSmallInteger
- FiberStackBounds
- P5 — コンパイラ
- CountingSink
- Bootstrap
- TEST
- P9 — 統合
- Deep
- ImageSurgery
- TEST
- MainMenu
- Ast
- TEST
- Session.cpp
- Deferred vendor methods list
- CLAUDE.md
- native_method_test.cpp
- TEST
- TEST
- P6b Vendor File-in Implementation Plan
- Qiita-part2.md
- string
- VirtualRegion.cpp
- Loaded
- Phase Progression P0-P11
- evalBody
- = and hash Contract
- TEST
- TEST
- TEST
- Stack
- DebugSnapshot::capture (root-safe, no heap allocation)
- TEST
- P4 — Kernelネイティブ実装
- reshapeClass
- RootedArray
- .isTrue
- .nil
- Abort / Evaluation Failure Propagation
- Smalltalk-80 Blue Book
- Fiber.cpp
- send2
- runFile
- TEST
- writeFile
- P10 — 事後デバッガ
- FrameBlock
- path
- Token
- Gc
- Table
- ClassMethodCache
- B2: Block semantics and interpreter (shared temps, inlining)
- C ABI Bridge (ao_abi.h)
- Debugger (P10 Post-mortem)
- TEST
- Codegen.cpp
- Image::load
- PingPong
- P11-04: Proceed / Abort
- VendorClassFile
- FiberRegs
- P2 — ブートストラップ
- P0-03: CMake + GoogleTest + CLI
- intern
- StackPool
- Character.cpp
- TEST
- ObjectHeader
- Counts
- P11-07: 受け入れ、CHANGELOG、Graphify deep
- Parsed
- WellKnown::InternTable
- TEST
- PcSpan
- P8–P9 Remaining Implementation Plan
- FileSizeLimit
- cli_test.sh
- RegisterSwap
- popFrame
- KeptMethod
- TestDir
- ao_set_inspect_hook
- Metaclass Circularity (Blue Book Rules 6-10)
- TEST
- Roots::visitAll
- P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数
- Root
- imageRegistryStubB()
- answerOne
- TDD Test Suites (SPEC section 4)
- ao-smalltalk/AGENTS.md
- P11-02: 評価プロセス
- P11-05: Step into / over / out と Debug it
- .specialSelector
- StringDoProbe
- abortingNew
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
- Format.hpp
- isClassShaped
- InlineCache
- BlockProbe
- fileInLoadOrder
- fillFullTable
- P11-03: 止める、止まったプロセスのフレーム、保存の拒否
- Qiita-part1.md
- valueHashFold
- ScopedGcStressEnv
- TEST
- ParseResult
- .ptr
- expectPrints
- DepthGuard
- NameBag

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
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `Interfaces` --references--> `DebugFrames`  [INFERRED]
  docs/prs/P11-01.md → runtime/include/ao/DebugSnapshot.hpp
- `手順` --references--> `sessionEval()`  [INFERRED]
  docs/prs/P11-02.md → runtime/src/Session.cpp
- `手順` --references--> `sessionEval()`  [INFERRED]
  docs/prs/P11-04.md → runtime/src/Session.cpp
- `目的` --references--> `ao_eval()`  [INFERRED]
  docs/prs/P11-02.md → runtime/src/abi.cpp

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

## Communities (304 total, 90 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 2 - "TEST"
Cohesion: 0.03
Nodes (74): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+66 more)

### Community 3 - "Interpreter.cpp"
Cohesion: 0.10
Nodes (43): run, ActiveGuard, rootShared, saved, applyMethod(), boolean(), branchTruth(), byteCount() (+35 more)

### Community 4 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 5 - "TEST"
Cohesion: 0.03
Nodes (78): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+70 more)

### Community 6 - "send"
Cohesion: 0.20
Nodes (28): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+20 more)

### Community 7 - "RealScope"
Cohesion: 0.08
Nodes (27): Capture, owner, var, uint32_t, RealScope, copied, copiedBase, isBlock (+19 more)

### Community 8 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 9 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 10 - "Stream.cpp"
Cohesion: 0.15
Nodes (35): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_ReadWriteStream_contents(), ao_WriteStream_contents(), ao_WriteStream_nextPut_(), classFormat() (+27 more)

### Community 11 - "Boot"
Cohesion: 0.05
Nodes (66): makeSubclass(), abortingSubclass(), countingPrintString(), CallContext, string, uint32_t, evalExpr(), expectAbortedEmpty() (+58 more)

### Community 12 - "TEST"
Cohesion: 0.03
Nodes (85): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CompileError (+77 more)

### Community 13 - "TEST_F"
Cohesion: 0.03
Nodes (70): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+62 more)

### Community 14 - "Emitter"
Cohesion: 0.16
Nodes (10): int16_t, Op, size_t, uint16_t, uint8_t, Emitter, real_, hasReceiverChild() (+2 more)

### Community 15 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 16 - "DebuggerWindow"
Cohesion: 0.07
Nodes (38): aoDebuggerInspectHook(), DebugFrame, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .open, .selectedFrame (+30 more)

### Community 17 - "vector"
Cohesion: 0.07
Nodes (53): algorithm, chrono, Chunk, classpool, cmath, compile, CompiledMethod, compiler (+45 more)

### Community 18 - "TEST"
Cohesion: 0.05
Nodes (40): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+32 more)

### Community 19 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.19
Nodes (16): B10: Cooperative process scheduler (fibers), B11: App and build remainder, B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes, B6: Image/session robustness (atomic save, load validation), B8: Kernel numeric semantics, B9: Kernel collections and streams (+8 more)

### Community 20 - "Oop"
Cohesion: 0.11
Nodes (58): uint64_t, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, ao_Object_basicAt_() (+50 more)

### Community 21 - "Heap"
Cohesion: 0.05
Nodes (64): hashedcollection, Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_ (+56 more)

### Community 22 - "TEST"
Cohesion: 0.05
Nodes (44): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+36 more)

### Community 23 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+18 more)

### Community 24 - "BrowserWindow"
Cohesion: 0.10
Nodes (22): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+14 more)

### Community 25 - "abi.cpp"
Cohesion: 0.08
Nodes (53): atomic, Body, CountsAnswerMinusOneOnFailure, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at(), ao_browser_class_count(), ao_browser_class_definition() (+45 more)

### Community 26 - "Ao 実装文書 (docs/ index)"
Cohesion: 0.13
Nodes (32): vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, P8 — AppKit ツール, P4-09: Kernel NativeMethod 走査と bench, Kernel NativeMethod 走査, P6b-01: Cuis pin と ORIGIN.md, Cuis vendor pin (ORIGIN.md) (+24 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.12
Nodes (13): Int, WorkspaceWindow, .errorAccessibilityLabel, .errorText, .inspectorCount, .inspectorText, .inspectorWindow, .textAccessibilityLabel (+5 more)

### Community 28 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 29 - "Literal"
Cohesion: 0.05
Nodes (49): BlockMethodHasItsOwnMapInMethodCoordinates, int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr (+41 more)

### Community 30 - "TEST"
Cohesion: 0.03
Nodes (88): AllocateNoGcSpillsToOld, ByteObjectPayload, ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap (+80 more)

### Community 31 - "Compile.cpp"
Cohesion: 0.16
Nodes (31): cctype, acceptClassSource(), acceptMethodSource(), anyMethodIn(), applyChunks(), applyMethodsFor(), assignError(), boxBytes() (+23 more)

### Community 32 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 33 - "DebugSnapshot"
Cohesion: 0.04
Nodes (47): Files, DebugFrames, count, kind, method, pc, receiver, selector (+39 more)

### Community 34 - "TEST"
Cohesion: 0.04
Nodes (47): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+39 more)

### Community 35 - "DefinitionScanner"
Cohesion: 0.08
Nodes (22): DefinitionScanner, Any, Bool, Int, Int32, Notification, NSRect, NSScrollView (+14 more)

### Community 36 - "TEST_F"
Cohesion: 0.06
Nodes (36): BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots, DoItFrameSourceDropsPrefix, EvalErrorFillsFramesInnermostFirst (+28 more)

### Community 37 - "Image::save"
Cohesion: 0.13
Nodes (25): writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot(), byte, Roots, string_view (+17 more)

### Community 38 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (46): B2 to:do: bench（jump 展開ループ）, bytecode/native 比率メトリクス, SmallInteger fast path（send を経ない演算）, P4 microbench（native to:do: 基準）, P6 interpreter bench（bytecode block send）, 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる (+38 more)

### Community 39 - "TEST"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 40 - "P1 — オブジェクトメモリ"
Cohesion: 0.30
Nodes (15): P0 (親フェーズ, stub), ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P0-05: docs/phases と docs/prs, P0-07: Graphify 初期グラフ (+7 more)

### Community 41 - "Scheduler.cpp"
Cohesion: 0.08
Nodes (68): flags, slotAt, slotAtPut, afterResume, enqueue, find, leaveLists, liveFibers (+60 more)

### Community 42 - "Frame"
Cohesion: 0.08
Nodes (26): Frame, context, isBlock, method, pc, prev, receiver, sendArgc (+18 more)

### Community 43 - "TEST_F"
Cohesion: 0.04
Nodes (50): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+42 more)

### Community 44 - "TEST_F"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 45 - "CallContext"
Cohesion: 0.06
Nodes (37): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+29 more)

### Community 46 - "cstdint"
Cohesion: 0.04
Nodes (38): cassert, cerrno, climits, string_view, csignal, cstddef, cstdint, cstdlib (+30 more)

### Community 47 - "ImageLoad.cpp"
Cohesion: 0.22
Nodes (26): ifstream, atOffset(), checkFile(), byte, size_t, string, T, uint32_t (+18 more)

### Community 48 - "Scheduler"
Cohesion: 0.06
Nodes (35): CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler, abandonAll (+27 more)

### Community 49 - "MethodDebugInfo"
Cohesion: 0.33
Nodes (6): MethodDebugInfo, pcMap, temps, size_t, p10ExpectSpansWithin(), p10HasTemp()

### Community 50 - "ao_main.cpp"
Cohesion: 0.21
Nodes (14): dyld, runtime, addRoots, bootAndRunTests(), string, imageUsage(), main(), printFileInErrors() (+6 more)

### Community 51 - "putNative"
Cohesion: 0.08
Nodes (36): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+28 more)

### Community 52 - "Session"
Cohesion: 0.07
Nodes (33): kind, pc, browserClassCount(), DebugEntry, debugFrameKind(), debugFramePc(), debugTempCount(), frameInRange() (+25 more)

### Community 53 - "SPEC.md (Product Specification)"
Cohesion: 0.11
Nodes (21): ao_debug_* / ao_set_debug_capture C ABI, Post-mortem Debugger Capture, P10 Known Limitations, Unreleased: Phase P10 Debugger, 1.0.0 Release Entry, Fixed Design Decisions, PHASE File, 曲げない規則 (+13 more)

### Community 54 - "classRows"
Cohesion: 0.20
Nodes (25): browserClassAt(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource(), browserSubclassAt() (+17 more)

### Community 55 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+3 more)

### Community 56 - ".isHeap"
Cohesion: 0.17
Nodes (28): 手順, toDoSendingLessOrEqual(), Scheduler::runFiber(), abortDoesNotUnderstand(), abortEvaluation(), abortEvaluationQuiet(), abortFailedSend(), abortReasonText() (+20 more)

### Community 57 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 58 - "String.cpp"
Cohesion: 0.18
Nodes (31): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+23 more)

### Community 59 - "AoApp"
Cohesion: 0.11
Nodes (14): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+6 more)

### Community 60 - "WellKnown.cpp"
Cohesion: 0.10
Nodes (28): findSymbol, global, internWith, Roots, string_view, ImageSelector, name, WellKnown (+20 more)

### Community 61 - ".run"
Cohesion: 0.07
Nodes (36): AnyObject, InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable (+28 more)

### Community 62 - "BrowserModel"
Cohesion: 0.11
Nodes (17): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, UnsafeMutablePointer (+9 more)

### Community 63 - "DebuggerWindowTests"
Cohesion: 0.14
Nodes (10): DebuggerWindowTests, Int, NSFont, NSTableView, NSTextView, NSView, NSWindow, String (+2 more)

### Community 64 - "Gc.cpp"
Cohesion: 0.13
Nodes (17): clearWeakAfterOldMark, checkNotPoisoned, Roots, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark() (+9 more)

### Community 65 - "VendorExtract.cpp"
Cohesion: 0.17
Nodes (28): allowIndex(), containsHostWord(), string_view, doubleBangs(), extractVendor(), firstLineKey(), firstNonEmptyLine(), HostMethod (+20 more)

### Community 66 - "TEST"
Cohesion: 0.06
Nodes (44): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+36 more)

### Community 67 - "allocateRetry"
Cohesion: 0.13
Nodes (36): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+28 more)

### Community 68 - "Geometry.cpp"
Cohesion: 0.19
Nodes (33): allocateInstance(), CallContext, Roots, uint32_t, RootedArray::RootedArray(), ao_Point_add(), ao_Point_equals(), ao_Point_hash() (+25 more)

### Community 69 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 70 - "TEST"
Cohesion: 0.10
Nodes (20): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+12 more)

### Community 71 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 72 - "collectNursery"
Cohesion: 0.20
Nodes (15): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint() (+7 more)

### Community 73 - "CompileEnv"
Cohesion: 0.06
Nodes (29): BlockAssignmentIsBindingStore, Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings (+21 more)

### Community 74 - "BlockContext.cpp"
Cohesion: 0.17
Nodes (27): ao_runtime library target, ao executable target, cli_filein / cli_image_save / cli_test ctest targets, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat() (+19 more)

### Community 75 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 76 - "string"
Cohesion: 0.14
Nodes (36): Interfaces, 手順, argCountOf(), byteText(), categoryHeading(), classNameOf(), ClassRow, category (+28 more)

### Community 77 - "CallContext"
Cohesion: 0.16
Nodes (30): ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_SmalltalkImage_at_(), ao_SmalltalkImage_at_ifAbsent_(), ao_SmalltalkImage_at_put_(), ao_SmalltalkImage_globals(), ao_SmalltalkImage_includesKey_(), ao_Stream_cr() (+22 more)

### Community 78 - "TEST"
Cohesion: 0.08
Nodes (26): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+18 more)

### Community 79 - "ao_eval"
Cohesion: 0.06
Nodes (41): AoSpan, end, message, start, CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock (+33 more)

### Community 80 - "Array.cpp"
Cohesion: 0.18
Nodes (24): valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+16 more)

### Community 81 - "Scheduler::Record"
Cohesion: 0.08
Nodes (24): Scheduler, uint64_t, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+16 more)

### Community 82 - "AppKit"
Cohesion: 0.18
Nodes (7): Ao, SmokeTests, AppKit, CAo, P0 phase doc, XCTest, XCTestCase

### Community 83 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 84 - "FiberStack"
Cohesion: 0.15
Nodes (16): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, FiberStack, acquire, base_ (+8 more)

### Community 85 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 86 - "TEST_F"
Cohesion: 0.08
Nodes (23): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+15 more)

### Community 87 - "TEST"
Cohesion: 0.11
Nodes (16): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+8 more)

### Community 88 - "uint64_t"
Cohesion: 0.24
Nodes (15): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+7 more)

### Community 89 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+20 more)

### Community 90 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 91 - "string"
Cohesion: 0.22
Nodes (13): CarriedMethod, image, meta, selector, source, string, string_view, deferredListing() (+5 more)

### Community 92 - "ImageFormat"
Cohesion: 0.15
Nodes (14): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+6 more)

### Community 93 - "DebugSnapshot.cpp"
Cohesion: 0.18
Nodes (21): contextPc(), CallContext, Frame, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method() (+13 more)

### Community 94 - "MethodSource"
Cohesion: 0.12
Nodes (23): attachBlocks(), blockLiteral(), MethodImage, Roots, size_t, DebugInfoRef, body, index (+15 more)

### Community 95 - "TEST"
Cohesion: 0.11
Nodes (25): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+17 more)

### Community 96 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 97 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 98 - "NativeMethod.cpp"
Cohesion: 0.22
Nodes (19): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+11 more)

### Community 99 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 100 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 101 - "ImageHeader"
Cohesion: 0.11
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 102 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 103 - ".isSmallInteger"
Cohesion: 0.14
Nodes (34): int64_t, bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t (+26 more)

### Community 104 - "FiberStackBounds"
Cohesion: 0.24
Nodes (11): AO_FIBER_REAL_FRAME, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitch() (+3 more)

### Community 105 - "P5 — コンパイラ"
Cohesion: 0.14
Nodes (22): CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P5-01: 字句解析, Smalltalk Scanner（字句解析）, P5-02: 構文解析と AST, Smalltalk Parser と AST (+14 more)

### Community 106 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 107 - "Bootstrap"
Cohesion: 0.10
Nodes (26): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, Bootstrap, GrowKeepsOuterOopAndEntries, MethodDictionary, NinthMethodWithFullNurseryIsInstalled (+18 more)

### Community 108 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 109 - "P9 — 統合"
Cohesion: 0.20
Nodes (12): P9 — 統合, P9-01: Do it / Print it / Inspect it, Do it / Print it / Inspect it, P9-02: Browser accept, Browser accept, P9-03: エラー表示と VoiceOver, エラー表示と VoiceOver, P9-04 v1 Golden Acceptance (+4 more)

### Community 110 - "Deep"
Cohesion: 0.22
Nodes (10): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+2 more)

### Community 111 - "ImageSurgery"
Cohesion: 0.25
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 112 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 113 - "MainMenu"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, Selector, String, Void

### Community 114 - "Ast"
Cohesion: 0.09
Nodes (29): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+21 more)

### Community 115 - "TEST"
Cohesion: 0.14
Nodes (14): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, fillNursery(), smi(), TEST() (+6 more)

### Community 116 - "Session.cpp"
Cohesion: 0.10
Nodes (32): Image, check, load, save, boot(), shutdown(), blankOut(), bumpDebugGeneration() (+24 more)

### Community 117 - "Deferred vendor methods list"
Cohesion: 0.15
Nodes (15): B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12), MappedCollection absent from pinned sources (+7 more)

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

### Community 122 - "P6b Vendor File-in Implementation Plan"
Cohesion: 0.09
Nodes (32): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+24 more)

### Community 123 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 124 - "string"
Cohesion: 0.23
Nodes (18): size_t, string, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars (+10 more)

### Community 125 - "VirtualRegion.cpp"
Cohesion: 0.21
Nodes (12): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+4 more)

### Community 126 - "Loaded"
Cohesion: 0.13
Nodes (17): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+9 more)

### Community 127 - "Phase Progression P0-P11"
Cohesion: 0.16
Nodes (17): Ao Smalltalk System, ao_image_save, Bootstrap Procedure, Cuis Smalltalk, DEFERRED.md Exclusion List, Graphify / Serena Mandatory Tooling, Image Format .aoimage, Image Load Validation Rules (+9 more)

### Community 128 - "evalBody"
Cohesion: 0.39
Nodes (8): Interfaces, AoInspectFn, optional, debugInspect(), evalBody(), inspectValue(), printStringOf(), knownGlobals

### Community 129 - "= and hash Contract"
Cohesion: 0.14
Nodes (16): Array, ByteArray, Character, = and hash Contract, Float (IEEE754 binary64), Fraction, Magnitude, Number / Integer Hierarchy (+8 more)

### Community 130 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+6 more)

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
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 136 - "P4 — Kernelネイティブ実装"
Cohesion: 0.08
Nodes (32): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, P4 — Kernelネイティブ実装, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary (+24 more)

### Community 137 - "reshapeClass"
Cohesion: 0.20
Nodes (21): FileInError, error, file, method, string, string_view, isVendorStub(), acceptClassDef() (+13 more)

### Community 138 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 139 - ".isTrue"
Cohesion: 0.49
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 140 - ".nil"
Cohesion: 0.11
Nodes (31): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+23 more)

### Community 141 - "Abort / Evaluation Failure Propagation"
Cohesion: 0.16
Nodes (14): Abandon (Process Teardown Without Cleanup), Abort / Evaluation Failure Propagation, BlockContext, Block Closures and Shared Temps, Cooperative Fiber Scheduler, Deadlock Handling, Drain (Evaluation End), ensure: / ifCurtailed: (+6 more)

### Community 142 - "Smalltalk-80 Blue Book"
Cohesion: 0.21
Nodes (14): Association, Smalltalk-80 Blue Book, Boolean / True / False, Class Variables / classPool, Collection Protocol, Dictionary / IdentityDictionary, Smalltalk Global Dictionary, Open-Addressing Hash Table Design (+6 more)

### Community 143 - "Fiber.cpp"
Cohesion: 0.22
Nodes (11): asan_interface, common_interface_defs, pthread, array, clearShadow(), byte, size_t, map (+3 more)

### Community 144 - "send2"
Cohesion: 0.07
Nodes (32): ClassDefinitionThroughAliasOnlyRebindsGlobal, ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, Geometry, KeepsNativeIdentityEquals, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails (+24 more)

### Community 145 - "runFile"
Cohesion: 0.26
Nodes (12): ao_AoTest_assert_equals_(), CallContext, path, Root, string, string_view, uint32_t, listTests() (+4 more)

### Community 146 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 147 - "writeFile"
Cohesion: 0.27
Nodes (12): size_t, string, directoryOf(), makeTemporary(), NamedOop, name, value, saveTarget() (+4 more)

### Community 148 - "P10 — 事後デバッガ"
Cohesion: 0.09
Nodes (28): CallContext::topFrame — 解釈フレーム連鎖, Debugger 窓 (frames / source / variables), abort スナップショット / ao_debug_* ABI, P10 — 事後デバッガ, 評価プロセス (doIt を fork したプロセスで実行), P11 — ライブデバッガ, Scheduler (terminate / resumeTo / switchTo), P10-01: SPEC と CLAUDE.md の改訂、PHASE (+20 more)

### Community 149 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 150 - "path"
Cohesion: 0.18
Nodes (11): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readHeapBytes() (+3 more)

### Community 151 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 152 - "Gc"
Cohesion: 0.22
Nodes (11): Gc, clearWeakAfterNursery, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress, scavengeFromRoots (+3 more)

### Community 153 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 154 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 155 - "B2: Block semantics and interpreter (shared temps, inlining)"
Cohesion: 0.22
Nodes (10): B0: Test infrastructure (GC stress mode, ASan), B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), Fiber stack via mmap with guard page, pooled, HandleScope/RootedArray/allocateRetry consolidation, Inlining of ifTrue:/whileTrue:/to:do: etc., NativeFn receiver/args rooted via RootedArray, Old space: mmap-reserved, mprotect-committed virtual region up to 4GiB (+2 more)

### Community 156 - "C ABI Bridge (ao_abi.h)"
Cohesion: 0.33
Nodes (11): ao_accept_class, ao_accept_method, ao_eval, ao_image_load, AoSpan, AppKit Host GUI, C ABI Bridge (ao_abi.h), ao_debug_* ABI (+3 more)

### Community 157 - "Debugger (P10 Post-mortem)"
Cohesion: 0.22
Nodes (11): Ao Bytecode Instruction Set, CompiledMethod, Ao Compiler, Method/Block Activation Contexts, Debugger Capture / Snapshot, Debugger (P10 Post-mortem), Frame Chain (CallContext::topFrame), Inline Expansion of Control-Flow Sends (+3 more)

### Community 158 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 159 - "Codegen.cpp"
Cohesion: 0.07
Nodes (35): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Analysis, declared (+27 more)

### Community 160 - "Image::load"
Cohesion: 0.31
Nodes (9): readHeader, bindAll(), checkGlobals(), Roots, string_view, WellKnown, fileOop(), Image::load() (+1 more)

### Community 161 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 162 - "P11-04: Proceed / Abort"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P11-04: Proceed / Abort, コミット文, テスト, 完了条件, 手順, 目的

### Community 163 - "VendorClassFile"
Cohesion: 0.25
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 164 - "FiberRegs"
Cohesion: 0.20
Nodes (10): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+2 more)

### Community 165 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 166 - "P0-03: CMake + GoogleTest + CLI"
Cohesion: 0.25
Nodes (9): P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub, P0-06: Serena project (+1 more)

### Community 167 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 168 - "StackPool"
Cohesion: 0.40
Nodes (5): array, kPoolLimit, StackPool, count, stacks

### Community 169 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

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

### Community 175 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 176 - "TEST"
Cohesion: 0.25
Nodes (8): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionStringIsReleaseOneZeroZero, VersionTruncationIsRangeError

### Community 177 - "PcSpan"
Cohesion: 0.40
Nodes (5): uint32_t, PcSpan, end, pc, start

### Community 178 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.33
Nodes (6): docs/bench.md, PHASE file, docs/phases/P8.md, docs/phases/P9.md, P8–P9 Remaining Implementation Plan, SPEC.md

### Community 179 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 180 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 181 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 182 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 183 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 184 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 185 - "ao_set_inspect_hook"
Cohesion: 0.29
Nodes (6): AoTranscriptFn, ao_set_inspect_hook(), ao_set_transcript_hook(), AoInspectFn, testing::Test, SessionAbi

### Community 186 - "Metaclass Circularity (Blue Book Rules 6-10)"
Cohesion: 0.60
Nodes (5): Behavior, Class, ClassDescription, Metaclass, Metaclass Circularity (Blue Book Rules 6-10)

### Community 187 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 188 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 189 - "P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数, コミット文, テスト, 完了条件, 目的

### Community 190 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 191 - "imageRegistryStubB()"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 193 - "TDD Test Suites (SPEC section 4)"
Cohesion: 0.67
Nodes (4): GC Stress Mode (AO_GC_STRESS), Kernel Scan Test (NativeMethod invariant), Generational Object Memory / GC, TDD Test Suites (SPEC section 4)

### Community 194 - "ao-smalltalk/AGENTS.md"
Cohesion: 0.67
Nodes (3): ao-smalltalk/AGENTS.md, CLAUDE.md process authority, SPEC.md product authority

### Community 195 - "P11-02: 評価プロセス"
Cohesion: 0.25
Nodes (7): Files, P11-02: 評価プロセス, コミット文, テスト, 完了条件, 手順, 目的

### Community 196 - "P11-05: Step into / over / out と Debug it"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P11-05: Step into / over / out と Debug it, コミット文, テスト, 完了条件, 目的

### Community 198 - "StringDoProbe"
Cohesion: 0.25
Nodes (8): mt19937, int64_t, Root, StringDoProbe, calls, rng, strangers, walked

### Community 199 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 288 - "Format.hpp"
Cohesion: 0.46
Nodes (7): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make()

### Community 289 - "isClassShaped"
Cohesion: 0.32
Nodes (7): isClassShaped(), superclassOf(), pair, hasSubclass(), liveClasses(), methodHoldingDropped(), ownClassName()

### Community 290 - "InlineCache"
Cohesion: 0.32
Nodes (7): InlineCache, cachedClass, cachedMethod, ClassMethodCache::addRoots(), Roots, IcGuard, ic

### Community 291 - "BlockProbe"
Cohesion: 0.29
Nodes (5): BlockProbe, b, probe, int64_t, Root

### Community 292 - "fileInLoadOrder"
Cohesion: 0.48
Nodes (7): path, deferredMethods(), fileInFile(), fileInListedFile(), fileInLoadOrder(), readSource(), sessionFileInLoadOrder()

### Community 293 - "fillFullTable"
Cohesion: 0.33
Nodes (7): int64_t, Root, set, uint32_t, fillFullTable(), keyWithHome(), smi()

### Community 294 - "P11-03: 止める、止まったプロセスのフレーム、保存の拒否"
Cohesion: 0.33
Nodes (5): P11-03: 止める、止まったプロセスのフレーム、保存の拒否, コミット文, テスト, 完了条件, 目的

### Community 295 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 296 - "valueHashFold"
Cohesion: 0.53
Nodes (6): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold()

### Community 297 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 298 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 299 - "ParseResult"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 300 - ".ptr"
Cohesion: 0.50
Nodes (3): CallContext, string_view, workspaceBinding()

### Community 301 - "expectPrints"
Cohesion: 0.50
Nodes (4): Check, printed, source, expectPrints()

### Community 303 - "NameBag"
Cohesion: 0.67
Nodes (3): NameBag, heap, names

## Ambiguous Edges - Review These
- `B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool)` → `Compiler MethodImage pcMap + temps (Emitter::mark)`  [AMBIGUOUS]
  docs/superpowers/plans/2026-09-26-p10-debugger.md · relation: conceptually_related_to
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

## Knowledge Gaps
- **900 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+895 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2397 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **90 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool)` and `Compiler MethodImage pcMap + temps (Emitter::mark)`?**
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
- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `Interpreter.cpp`, `Float.cpp`, `TEST`, `send`, `LargeInteger.cpp`, `Stream.cpp`, `Boot`, `TEST`, `vector`, `Heap`, `Boolean.cpp`, `abi.cpp`, `TEST`, `Compile.cpp`, `Heap.cpp`, `DebugSnapshot`, `TEST`, `Image::save`, `TEST`, `Scheduler.cpp`, `Frame`, `CallContext`, `cstdint`, `putNative`, `Session`, `classRows`, `.isHeap`, `String.cpp`, `WellKnown.cpp`, `Gc.cpp`, `TEST`, `allocateRetry`, `Geometry.cpp`, `collectNursery`, `BlockContext.cpp`, `Roots`, `string`, `CallContext`, `Array.cpp`, `Scheduler::Record`, `SmallInteger.cpp`, `uint64_t`, `string`, `DebugSnapshot.cpp`, `MethodSource`, `TEST`, `Process.cpp`, `NativeMethod.cpp`, `ImageHeader`, `Roots.cpp`, `.isSmallInteger`, `FiberStackBounds`, `CountingSink`, `Bootstrap`, `TEST`, `TEST`, `native_method_test.cpp`, `Loaded`, `evalBody`, `TEST`, `Stack`, `TEST`, `reshapeClass`, `RootedArray`, `.isTrue`, `.nil`, `send2`, `runFile`, `writeFile`, `FrameBlock`, `path`, `Table`, `ClassMethodCache`, `TEST`, `Image::load`, `intern`, `Character.cpp`, `ObjectHeader`, `WellKnown::InternTable`, `popFrame`, `Root`, `imageRegistryStubB()`, `answerOne`, `.specialSelector`, `abortingNew`, `Format.hpp`, `isClassShaped`, `InlineCache`, `BlockProbe`, `fileInLoadOrder`, `fillFullTable`, `.ptr`?**
  _High betweenness centrality (0.335) - this node is a cross-community bridge._
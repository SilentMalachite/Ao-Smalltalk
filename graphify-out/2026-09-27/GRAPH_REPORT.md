# Graph Report - ao-smalltalk  (2026-09-27)

## Corpus Check
- 296 files · ~344,435 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 6193 nodes · 16450 edges · 345 communities (218 shown, 127 thin omitted)
- Extraction: 87% EXTRACTED · 13% INFERRED · 0% AMBIGUOUS · INFERRED: 2192 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ee97f827`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- TEST_F
- .fromSmallInteger
- TEST
- TEST_F
- TEST
- .isSmallInteger
- abi.cpp
- RealScope
- DebuggerWindow
- AcceptTests
- LargeInteger.cpp
- send
- Parser
- Scheduler
- BrowserWindow
- Boot
- P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt
- Stream.cpp
- Heap
- string
- MethodSource
- Emitter
- Scanner.cpp
- Interpreter::run
- DebuggerWindowTests
- TEST
- WorkspaceWindow
- TEST
- TEST_F
- cstdint
- TEST
- Object.cpp
- P6b — vendor file-in
- .isHeap
- TEST
- TEST
- TranscriptWindow
- TEST
- ImageLoad.cpp
- ImageSave.cpp
- Heap.cpp
- Oop
- CallContext
- classRows
- Float.cpp
- Gc
- TEST
- ao_runtime_boot
- WellKnown.cpp
- BrowserModel
- 横断テーマ3: 言語意味論の欠落(コンパイラ)
- TEST
- TEST_F
- vector
- ImageFormat.cpp
- Literal
- VendorExtract.cpp
- allocateRetry
- TEST_F
- ao_eval
- String.cpp
- Analysis
- ToolWindowTests
- DebugFrames
- putNative
- AoApp
- Task 12: Browser accept, Hierarchy, VoiceOver, Close v1
- HashedCollection.cpp
- TEST
- FiberStack
- P1 — オブジェクトメモリ
- Session.cpp
- Array.cpp
- Frame
- AppKit
- P12 追補: クラス ID と壊れたメソッド辞書 実装計画
- TEST
- DebugSnapshot
- ao_accept_method
- image_save_load_test.cpp
- CallContext
- DebugSnapshot.cpp
- Geometry.cpp
- WorkspaceEvalTests
- FileInError
- Roots
- SmallInteger.cpp
- Scheduler::Record
- TEST
- TEST
- Claude Review Fixes Plan (2026-09-23)
- Scheduler.cpp
- .nil
- string
- LiveFrames
- ClassPool.cpp
- ClassDef
- BlockContext.cpp
- TEST_F
- BrowserModelTests
- pc_map_test.cpp
- TEST
- TEST
- Process.cpp
- .build
- TEST
- reshapeClass
- Roots.cpp
- NativeMethod.cpp
- DefinitionScanner
- TEST
- PingPong
- ImageSurgery
- Class Removal Rules
- TEST
- Fiber.cpp
- Method Removal Rules
- TEST
- P12 Browser 削除 実装計画
- GarbageFirstBoot
- native_method_test.cpp
- TEST
- TEST
- AoSpan
- DiskHeader
- CountingSink
- Loaded
- Failure Aborts Evaluation (abortEvaluation)
- P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告
- ChunkAction
- ChunkParser.cpp
- unwinding
- FiberRegs
- Ast
- VirtualRegion.cpp
- .specialSelector
- TEST
- TEST
- TEST
- Bootstrap
- Stack
- C ABI (bridge/ao_abi.h)
- Kernel Classes (NativeMethod required)
- Ao 実装文書 index (docs/README.md)
- Deferred vendor methods list
- ao_main.cpp
- RootedArray
- TEST
- Interpreter.cpp
- TEST
- 1.0.0 release
- TEST
- Reentry
- Compile.cpp
- TEST
- P3 — ネイティブディスパッチ
- TEST
- ClassMethodCache
- FrameBlock
- Scanner
- path
- Cooperative Process Scheduler
- Class Definition Re-Accept
- P11 known limitations
- P11 plan reference (p11-ancient-matsumoto.md)
- Token
- CompiledMethodNatives.cpp
- TEST
- performSend
- Post-mortem debugger
- Ao Smalltalk 概要 (JA)
- TEST
- send2
- Temps
- CLAUDE.md (process source of truth)
- P0 (親フェーズ, stub)
- P2 — ブートストラップ
- Character.cpp
- TEST
- Counts
- exactFloat
- P8 — AppKit ツール
- Table
- Frame
- Bytecode interpreter
- Scheduler::fork
- Imported Class Library (no self-authored library)
- Session Source Table (not in image)
- P10 — 事後デバッガ
- ObjectHeader
- FileSizeLimit
- Rec
- cli_test.sh
- P9-04 v1 Golden Acceptance
- popFrame
- native_send_test.cpp
- TestDir
- test.sh
- TEST
- compiler/tests/smoke_test.cpp
- Roots::visitAll
- Live Debugger (evaluation process, halting)
- abortingSubclass
- Root
- imageRegistryStubA
- acceptClass
- answerOne
- .turnRunLoop
- Rules That Do Not Bend
- P11 — ライブデバッガ
- abortingNew
- build.sh
- WriteStream String Writes (reserve capacity)
- BrowserModel.subclasses(of:)
- BrowserModel.superclass(of:meta:)
- BrowserWindow.showHierarchy() / toggleHierarchy
- Package.swift
- CHANGELOG: P10/P11 post-mortem and live debugger entry
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
- BrowserModel.applyHierarchyList(_:selecting:)
- BrowserModel.hierarchy(of:meta:)
- ActiveGuard
- SelectedFrames
- PcSpan
- TEST
- StackPool
- ScopedGcStressEnv
- assemble
- Live debugger
- SlotNames
- P10 debugger plan reference
- PR table
- SPEC.md (product source of truth)
- v1 = P9 completion, released as 1.0.0
- interpreter
- parser
- ao_browser_class_count
- ao_browser_subclass_count
- ao_eval
- CHANGELOG: [1.0.0] first release
- runtime_src_fiber
- PHASE File
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
- string_view
- v1 Status Table
- runtime/CMakeLists.txt build config
- fileInLoadOrder
- installMethod
- Session
- keyWithHome
- Bootstrap order memory
- Kernel is native memory
- Memory maintenance guide
- Serena project.yml
- Interval Semantics
- v1 Non-Goals
- OrderedCollection Layout
- Repository Layout (runtime/compiler/image/app/bridge)
- SPEC §3.3 メッセージ送信 (method dictionary shape)

## God Nodes (most connected - your core abstractions)
1. `Oop` - 953 edges
2. `Heap` - 255 edges
3. `vector` - 179 edges
4. `WellKnown` - 168 edges
5. `BrowserWindow` - 123 edges
6. `Session` - 112 edges
7. `TEST()` - 111 edges
8. `TEST_F()` - 111 edges
9. `Ast` - 103 edges
10. `TEST()` - 101 edges

## Surprising Connections (you probably didn't know these)
- `ao_Object_identityEquals Native Method` --semantically_similar_to--> `addNamed()`  [INFERRED] [semantically similar]
  docs/prs/P9-04.md → runtime/src/NativeMethod.cpp
- `TDD` --references--> `DebuggerWindowTests`  [INFERRED]
  docs/phases/P11.md → app/AoTests/DebuggerWindowTests.swift
- `前提` --references--> `DebugFrames`  [INFERRED]
  docs/phases/P11.md → runtime/include/ao/DebugSnapshot.hpp
- `仕様` --references--> `abortEvaluation()`  [INFERRED]
  docs/phases/P10.md → runtime/src/Send.cpp
- `範囲` --references--> `ao_image_save()`  [INFERRED]
  docs/phases/P11.md → runtime/src/abi.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **SPEC 3.10 class ID scheme: stable IDs across Browser rows, ABI, and Session table** — app_ao_browsermodel_browserclass, app_ao_browsermodel_classid, bridge_ao_abi_ao_browser_class_id, runtime_src_session_browserclassid, runtime_src_session_sessionclassforid, runtime_src_session_assignclassids, runtime_src_session_classid [EXTRACTED 0.90]
- **SPEC 3.9 removal flow: Browser UI to method dictionary and source table** — app_ao_browserwindow_removemethod, bridge_ao_abi_ao_remove_method, runtime_include_ao_compile_removemethodof, runtime_src_compile_removemethodof_impl, runtime_include_ao_methoddictionary_removekey, runtime_src_session_forgetmethodsource [EXTRACTED 0.95]
- **コンパイラの言語意味論欠落(制御構造・二項演算子)** — docs_claude_review_05_compiler_block_outer_temp_assignment_dropped_no_inline, docs_claude_review_02_interpreter_control_flow_protocol_unimplemented_dnu_ok, docs_claude_review_05_compiler_comma_not_binary_char_string_concat_fails [EXTRACTED 1.00]
- **未ルート receiver/name によるヒープ破壊パターン (GC安全性 Critical 3件)** — docs_claude_review_01_object_memory_scavenge_collectold_stale_write, docs_claude_review_03_kernel_numeric_magnitude_lessequal_unrooted_receiver_heap_corruption, docs_claude_review_04_kernel_objects_collections_subclass_unrooted_receiver_name_dangling_pointer [EXTRACTED 1.00]
- **P8 Phase Tasks (must land before P9 begins)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_1_session_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_2_transcript_forwarding, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_3_browser_read_abi, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_4_swift_link, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_5_transcript_workspace_windows, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_6_browser_panes, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_7_menu_app_bundle [EXTRACTED 1.00]
- **P9 Phase Tasks (start only after P8 merges and PHASE=P9)** — docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_8_printstring, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_9_eval_workspace_vars, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_10_accept, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_11_workspace_eval_ui, docs_superpowers_plans_2026_09_23_p8_p9_remaining_task_12_browser_accept_hierarchy [EXTRACTED 1.00]
- **Mandatory Graphify + Serena Tooling Gate** — claude_graphify_requirement, claude_serena_requirement, claude_standard_workflow [EXTRACTED 1.00]
- **abortEvaluation as sole abort entry point across batches** — docs_superpowers_plans_2026_09_23_review_fixes_stack_guard, docs_superpowers_plans_2026_09_23_review_fixes_b3_failure_propagation, docs_superpowers_plans_2026_09_23_review_fixes_b10_scheduler [EXTRACTED 1.00]
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
- **Browser Method/Class Removal Feature (P12)** — bridge_ao_abi_ao_remove_method, bridge_ao_abi_ao_remove_class, runtime_tests_remove_abi_test_removeabi, docs_phases_p12, docs_superpowers_specs_2026_09_26_browser_remove_design_removal, changelog_p12_browser_removal [INFERRED 0.85]
- **Method removal pipeline: ao_remove_method → removeMethodNamed → removeKey / forgetMethodSource / cache invalidation** — spec_ao_remove_method, docs_superpowers_plans_2026_09_26_p12_browser_remove_removemethodnamed, docs_superpowers_plans_2026_09_26_p12_browser_remove_removekey, docs_superpowers_plans_2026_09_26_p12_browser_remove_forgetmethodsource [INFERRED 0.85]
- **P1 GC実装フロー (Heap→nursery→old→roots→weak→immovable)** — docs_prs_p1_02_pr, docs_prs_p1_03_pr, docs_prs_p1_04_pr, docs_prs_p1_05_pr, docs_prs_p1_06_pr, docs_prs_p1_07_pr [INFERRED 0.85]
- **P4 Kernel ネイティブメソッド充足パターン（各クラス群への send/NativeMethod/Bootstrap 共通適用）** — docs_prs_p4_01_doc, docs_prs_p4_02_doc, docs_prs_p4_03_doc, docs_prs_p4_04_doc, docs_prs_p4_05_doc, docs_prs_p4_06_doc, docs_prs_p4_07_doc [INFERRED 0.85]
- **P5 コンパイラパイプライン（Scanner→Parser→ISA→Codegen→ChunkParser）** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunkparser [INFERRED 0.85]
- **Session Class ID Identity Mechanism** — runtime_src_session_classids, runtime_src_session_classrows, runtime_src_session_browserclassid, bridge_ao_abi_ao_browser_class_id, runtime_tests_browser_abi_test_browserabi, docs_superpowers_plans_2026_09_27_p12_class_identity [INFERRED 0.85]
- **Canonical-English / Japanese Translation Pairs** — readme_ao_overview, readme_ja_ao_overview, contributing_release_process, contributing_ja_release_process [INFERRED 0.85]
- **MethodDictionary Malformed-Shape Defense** — runtime_src_methoddictionary_pairarray, runtime_src_methoddictionary_at, runtime_src_methoddictionary_atput, runtime_src_methoddictionary_removekey, runtime_tests_method_dictionary_test_malformeddictionaryanswersnilandtakesnothing, runtime_tests_remove_abi_test_malformedmethoddictionaryfallsbackonsend [INFERRED 0.90]
- **.aoimage 保存/読み込みラウンドトリップ** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_imagesave, docs_prs_p7_03_imageload_rebind [INFERRED 0.90]
- **P3 メッセージ送信基盤パイプライン（Symbol intern → MethodDictionary → NativeMethod → lookup → send キャッシュ）** — docs_prs_p3_01_doc, docs_prs_p3_02_doc, docs_prs_p3_03_doc, docs_prs_p3_04_doc, docs_prs_p3_05_doc [INFERRED 0.95]

## Communities (345 total, 127 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (149): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+141 more)

### Community 1 - "TEST_F"
Cohesion: 0.03
Nodes (81): AbortNonProceedableRunsEnsure, AbortRunsEnsureBlocks, BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots (+73 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (88): OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

### Community 3 - "TEST"
Cohesion: 0.03
Nodes (83): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+75 more)

### Community 4 - "TEST_F"
Cohesion: 0.03
Nodes (62): ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+54 more)

### Community 5 - "TEST"
Cohesion: 0.02
Nodes (84): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+76 more)

### Community 6 - ".isSmallInteger"
Cohesion: 0.10
Nodes (52): RemoveUnits, allocateNoGc, flags, size, slotAt, slotAtPut, int64_t, at() (+44 more)

### Community 7 - "abi.cpp"
Cohesion: 0.07
Nodes (59): atomic, Body, ClassIdIsStableAndOneRowPerClass, ClassIdsAreDroppedWithTheirClassAndNeverReused, ClassIdsDoNotSurviveBootOrImageLoad, CountsAnswerMinusOneOnFailure, ABI（P10-05）, ObjectIsKernelAndPrintStringIsNative (+51 more)

### Community 8 - "RealScope"
Cohesion: 0.08
Nodes (33): Analyzer, Capture, owner, var, uint32_t, LexScope, parent, real (+25 more)

### Community 9 - "DebuggerWindow"
Cohesion: 0.05
Nodes (46): aoDebuggerInspectHook(), DebugFrame, DebuggerButtonActions, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .isLive (+38 more)

### Community 10 - "AcceptTests"
Cohesion: 0.13
Nodes (11): spanMessage(), AcceptTests, Int32, NSMenu, NSSegmentedControl, NSTableView, NSTextView, NSView (+3 more)

### Community 11 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 12 - "send"
Cohesion: 0.09
Nodes (52): Graphify / Serena, 捕捉（P10-03、配線は P10-04）, InlineCache, cachedClass, cachedMethod, toDoSendingLessOrEqual(), Scheduler::runFiber(), abortDoesNotUnderstand() (+44 more)

### Community 13 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 14 - "Scheduler"
Cohesion: 0.04
Nodes (52): CallContext, EvalEnd, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+44 more)

### Community 15 - "BrowserWindow"
Cohesion: 0.06
Nodes (39): BrowserWindow, .acceptsMethod, .canRemoveClass, .canRemoveMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText (+31 more)

### Community 16 - "Boot"
Cohesion: 0.05
Nodes (60): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, expectAbortedEmpty() (+52 more)

### Community 17 - "P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt"
Cohesion: 0.10
Nodes (32): CLAUDE.md, docs/README.md, 2026-09-26-p10-debugger.md (計画書), PHASE file (P10), P10-01: SPEC と CLAUDE.md の改訂、PHASE, SPEC §3.13 デバッガ（捕捉の意味論）, SPEC.md, Codegen.cpp: Emitter::mark と compile* 群 (+24 more)

### Community 18 - "Stream.cpp"
Cohesion: 0.10
Nodes (65): ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+57 more)

### Community 19 - "Heap"
Cohesion: 0.05
Nodes (54): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+46 more)

### Community 20 - "string"
Cohesion: 0.13
Nodes (29): chrono, Chunk, classpool, cmath, compile, CompiledMethod, compiler, string (+21 more)

### Community 21 - "MethodSource"
Cohesion: 0.08
Nodes (34): 手順（PR の順序と依存）, removeMethodOf, MethodDictionary::removeKey, removeMethodOf (definition), attachBlocks(), blockLiteral(), MethodImage, Roots (+26 more)

### Community 22 - "Emitter"
Cohesion: 0.13
Nodes (17): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+9 more)

### Community 23 - "Scanner.cpp"
Cohesion: 0.21
Nodes (21): string, Tok, Token, uint32_t, digitValue(), isAlnum(), isAlpha(), isBinaryChar() (+13 more)

### Community 24 - "Interpreter::run"
Cohesion: 0.21
Nodes (19): run, applyMethod(), byteCount(), contextAlive(), CallContext, int16_t, uint32_t, uint8_t (+11 more)

### Community 25 - "DebuggerWindowTests"
Cohesion: 0.10
Nodes (18): Int, DebuggerWindowTests, Int, NSButton, NSFont, NSTableView, NSTextView, NSView (+10 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (53): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, EvalProcessRunsUntilItEndsAndLeavesNothing, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork (+45 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.06
Nodes (47): AnyObject, InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable (+39 more)

### Community 28 - "TEST"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 29 - "TEST_F"
Cohesion: 0.05
Nodes (54): AliasedOldClassRowActsOnItsOwnClass, ClassIdTableDoesNotBlockSuperclassRemoval, KernelScanStaysGreenAfterRemovals, LongNameCutsTheMessageAt255Bytes, MalformedMethodDictionaryFallsBackOnSend, RefusedRemoveChangesNothing, RemovalSurvivesSaveAndLoad, RemoveAcceptedMethodOnKernelClass (+46 more)

### Community 30 - "cstdint"
Cohesion: 0.07
Nodes (16): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, DebugSink (+8 more)

### Community 31 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 32 - "Object.cpp"
Cohesion: 0.12
Nodes (52): Graphify / Serena, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 33 - "P6b — vendor file-in"
Cohesion: 0.16
Nodes (19): vendor ライセンス方針 (Cuis MIT / 新規 Apache-2.0), P6b — vendor file-in, P7 — イメージ, Kernel NativeMethod 走査, Chunk file-in パーサ, P6b-01: Cuis pin と ORIGIN.md, Cuis vendor pin (ORIGIN.md), P6b-02: LOAD_ORDER と Kernel 上書き禁止 (+11 more)

### Community 34 - ".isHeap"
Cohesion: 0.10
Nodes (25): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, MalformedDictionaryAnswersNilAndTakesNothing, NinthMethodWithFullNurseryIsInstalled, RemoveKeyLeavesTheOthersFindableAndReusesThePair (+17 more)

### Community 35 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 36 - "TEST"
Cohesion: 0.04
Nodes (48): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+40 more)

### Community 37 - "TranscriptWindow"
Cohesion: 0.07
Nodes (28): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+20 more)

### Community 38 - "TEST"
Cohesion: 0.04
Nodes (51): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+43 more)

### Community 39 - "ImageLoad.cpp"
Cohesion: 0.06
Nodes (85): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ifstream, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap (+77 more)

### Community 40 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (44): cerrno, fcntl, appendRaw(), appendRecord(), collectImageSlot(), byte, Roots, size_t (+36 more)

### Community 41 - "Heap.cpp"
Cohesion: 0.09
Nodes (44): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+36 more)

### Community 42 - "Oop"
Cohesion: 0.17
Nodes (33): make(), Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, ao_Boolean_subclassResponsibility() (+25 more)

### Community 43 - "CallContext"
Cohesion: 0.05
Nodes (45): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+37 more)

### Community 44 - "classRows"
Cohesion: 0.14
Nodes (36): assignClassIds(), browserClassAt(), browserClassDefinition(), browserClassId(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount() (+28 more)

### Community 45 - "Float.cpp"
Cohesion: 0.15
Nodes (44): NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_greaterOrEqual(), ao_Float_greaterThan() (+36 more)

### Community 46 - "Gc"
Cohesion: 0.14
Nodes (22): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, collectNursery, collectOld, copy, heap_ (+14 more)

### Community 47 - "TEST"
Cohesion: 0.08
Nodes (29): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+21 more)

### Community 48 - "ao_runtime_boot"
Cohesion: 0.05
Nodes (40): AbiSmoke, AoTranscriptFn, BootAndShutdownReturnZero, BootVersionShutdown, CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, B2 `to:do:` bench, `Object>>halt`（P10-03） (+32 more)

### Community 49 - "WellKnown.cpp"
Cohesion: 0.06
Nodes (43): findSymbol, global, internWith, isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown (+35 more)

### Community 50 - "BrowserModel"
Cohesion: 0.17
Nodes (13): BrowserClass, BrowserModel, .classes, .metaFlag, .selectedClass, .selectedClassRow, ListedClass, Bool (+5 more)

### Community 51 - "横断テーマ3: 言語意味論の欠落(コンパイラ)"
Cohesion: 0.05
Nodes (41): 自分を含む Array の = でスタックオーバーフロー, 既存クラスの再 Accept で全メソッドが消える, クラス定義でない文字列が AO_OK で黙って捨てられる, printOn: が新しい printString を使わない, ソース未保存メソッドのプレースホルダを Accept すると本体が消える, Workspace 束縛が 255 temp の上限に達すると eval が全滅, メソッド辞書の拡張に失敗するとメソッドを黙って捨て installMethod は成功を返す, ネイティブ向けハンドルスコープが無く receiver・args が GC をまたいでルートされない (+33 more)

### Community 52 - "TEST"
Cohesion: 0.06
Nodes (38): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+30 more)

### Community 53 - "TEST_F"
Cohesion: 0.05
Nodes (38): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+30 more)

### Community 54 - "vector"
Cohesion: 0.07
Nodes (35): algorithm, vector, cstdio, cstdlib, cstring, future, iterator, limits (+27 more)

### Community 55 - "ImageFormat.cpp"
Cohesion: 0.24
Nodes (10): bit, byte, size_t, string, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader() (+2 more)

### Community 56 - "Literal"
Cohesion: 0.07
Nodes (33): int16_t, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal (+25 more)

### Community 57 - "VendorExtract.cpp"
Cohesion: 0.06
Nodes (77): .aoimage Restart Method Persistence, Vendor Class Allowlist, Bag (vendor stub rebind target), Cuis Smalltalk Vendor Pin, DEFERRED Unsupported Class Shapes, FileStream (host-patched vendor class), Host Word Patch (ao-host-patch), Kernel Scan Narrowed to Native-Required Classes (+69 more)

### Community 58 - "allocateRetry"
Cohesion: 0.11
Nodes (41): MethodDictionary::at, MethodDictionary::pairArray, CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t (+33 more)

### Community 59 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 60 - "ao_eval"
Cohesion: 0.06
Nodes (27): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, 仕様（設計判断）, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues (+19 more)

### Community 61 - "String.cpp"
Cohesion: 0.14
Nodes (37): bytesValueHash(), int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold(), ao_String_asSymbol(), ao_String_at_() (+29 more)

### Community 62 - "Analysis"
Cohesion: 0.06
Nodes (32): Codegen, CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, CompileResult (+24 more)

### Community 63 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+3 more)

### Community 64 - "DebugFrames"
Cohesion: 0.09
Nodes (34): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, DebugFrames, context, count, empty, kind, method, pc (+26 more)

### Community 65 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installArray(), WellKnown, installBoolean(), WellKnown, installCharacter(), WellKnown, installCollection() (+26 more)

### Community 66 - "AoApp"
Cohesion: 0.10
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 67 - "Task 12: Browser accept, Hierarchy, VoiceOver, Close v1"
Cohesion: 0.11
Nodes (33): Session-Only Method Source Table, Browser Protocol Split: native vs user, Selective printString Native Overrides, Process Session Model, SPEC §3.10 AppKit objects not on the heap, SPEC §3.10 Boot, eval, listing, accept, hooks, error strings, SPEC §3.11 Image version stays 1, no function pointers written, SPEC §3.6 printString readable via Print it (+25 more)

### Community 68 - "HashedCollection.cpp"
Cohesion: 0.22
Nodes (21): hashedcollection, bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t (+13 more)

### Community 69 - "TEST"
Cohesion: 0.08
Nodes (24): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+16 more)

### Community 70 - "FiberStack"
Cohesion: 0.13
Nodes (19): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+11 more)

### Community 71 - "P1 — オブジェクトメモリ"
Cohesion: 0.39
Nodes (12): ao::Gc — 正確 GC (nursery + old mark-compact), ao::Heap — ヘッダ付き bump 割り当て, ao::Oop — 64-bit tagged pointer, P1 — オブジェクトメモリ, ao::Roots — GC ルート API, P1-01: ao::Oop タグ, P1-02: オブジェクトヘッダと bump 割り当て, P1-03: nursery GC (+4 more)

### Community 72 - "Session.cpp"
Cohesion: 0.08
Nodes (48): boxMethodImage, Image, check, load, save, refreshStackLimit(), boot(), shutdown() (+40 more)

### Community 73 - "Array.cpp"
Cohesion: 0.13
Nodes (34): valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+26 more)

### Community 74 - "Frame"
Cohesion: 0.14
Nodes (14): Frame, context, depth, isBlock, method, pc, prev, receiver (+6 more)

### Community 75 - "AppKit"
Cohesion: 0.16
Nodes (10): Ao, CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, transcriptBoxHook(), SmokeTests, AppKit (+2 more)

### Community 76 - "P12 追補: クラス ID と壊れたメソッド辞書 実装計画"
Cohesion: 0.12
Nodes (25): BrowserModel.copyClass(at:), ao_browser_class_at, P12 — Browser の削除 (phase doc), Qiita 後編: 協調スケジューラとファイバ切り替え, P12 追補: クラス ID と壊れたメソッド辞書 実装計画, Design rationale: session class ID table, Design rationale: MethodDictionary shape check (pairArray), Scheduler / Fiber cooperative process scheduler (+17 more)

### Community 77 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, Qiita 前編: GC とブロックの意味論, Concept: tests green yet Critical bugs found only by external review, Qiita 後編: 修正バッチ B0–B11 (+20 more)

### Community 78 - "DebugSnapshot"
Cohesion: 0.07
Nodes (27): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+19 more)

### Community 79 - "ao_accept_method"
Cohesion: 0.08
Nodes (24): BrowserModel.classID(named:), BrowserModel.loadSource(), BrowserModel.select(...), BrowserModel.selector(withSource:), BrowserWindow.accept(), BrowserWindow.definedClassName(in:), BrowserWindow.removeClass / removeClass(atRow:), BrowserWindow.showDefinedClass(from:) (+16 more)

### Community 80 - "image_save_load_test.cpp"
Cohesion: 0.16
Nodes (12): csignal, mach, mach_vm, resource, pingPongFiber(), step(), fileInSource(), fileNames() (+4 more)

### Community 81 - "CallContext"
Cohesion: 0.16
Nodes (26): ソース表の拡張（P10-04）, リスク, removeClassOf, isClassShaped(), superclassOf(), acceptMethodInto(), acceptMethodSource(), CallContext (+18 more)

### Community 82 - "DebugSnapshot.cpp"
Cohesion: 0.13
Nodes (36): frameAt, contextPc(), CallContext, Frame, string, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context() (+28 more)

### Community 83 - "Geometry.cpp"
Cohesion: 0.20
Nodes (32): allocateInstance(), Roots, uint32_t, RootedArray::RootedArray(), ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide() (+24 more)

### Community 84 - "WorkspaceEvalTests"
Cohesion: 0.16
Nodes (4): NSTextView, NSView, String, WorkspaceEvalTests

### Community 85 - "FileInError"
Cohesion: 0.18
Nodes (21): acceptClassSource, FileInError, error, file, method, string, string_view, isVendorStub() (+13 more)

### Community 86 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 87 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 88 - "Scheduler::Record"
Cohesion: 0.07
Nodes (28): Scheduler, unique_ptr, Scheduler::endEval(), Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+20 more)

### Community 89 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 90 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 91 - "Claude Review Fixes Plan (2026-09-23)"
Cohesion: 0.11
Nodes (27): B0: Test infrastructure (GC stress mode, ASan), B10: Cooperative process scheduler (fibers), B11: App and build remainder, B1: GC safety and old-space growth, B2: Block semantics and interpreter (shared temps, inlining), B3: Failure propagation and cache invalidation, B4: Kernel class metadata (instVarNames, SmalltalkImage, classPool), B5: Browser/Workspace data-loss fixes (+19 more)

### Community 92 - "Scheduler.cpp"
Cohesion: 0.08
Nodes (51): abandonAll, afterResume, canHalt, drain, findId, leaveLists, reapDead, signal (+43 more)

### Community 93 - ".nil"
Cohesion: 0.14
Nodes (16): boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), uint32_t, uint8_t, Gc::clearWeakAfterOldMark(), uint32_t (+8 more)

### Community 94 - "string"
Cohesion: 0.19
Nodes (24): byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), string, WellKnown, debugFrameSource() (+16 more)

### Community 95 - "LiveFrames"
Cohesion: 0.09
Nodes (19): CallContext, Frame, size_t, string, uint32_t, LiveFrames, context, frames_ (+11 more)

### Community 96 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (23): adopt(), bindingAt(), copy(), CallContext, int64_t, string, string_view, uint32_t (+15 more)

### Community 97 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 98 - "BlockContext.cpp"
Cohesion: 0.12
Nodes (34): v1 known limitations, Unreleased: P10/P11 Debugger, P11 step branch, P4 microbench, P6 interpreter bench, PHASE file, docs/phases/P8.md, docs/phases/P9.md (+26 more)

### Community 99 - "TEST_F"
Cohesion: 0.09
Nodes (21): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+13 more)

### Community 100 - "BrowserModelTests"
Cohesion: 0.18
Nodes (10): BrowserModelTests, Int32, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T (+2 more)

### Community 101 - "pc_map_test.cpp"
Cohesion: 0.13
Nodes (21): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, uint32_t, firstBlock(), named() (+13 more)

### Community 102 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 103 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 104 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 105 - ".build"
Cohesion: 0.22
Nodes (10): Actions, MainMenu, MenuAction, Bool, NSMenu, NSMenuItem, Selector, String (+2 more)

### Community 106 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 107 - "reshapeClass"
Cohesion: 0.09
Nodes (29): ao_accept_class, acceptMethodSource, MethodDictionary::atPut, MethodDictionary::create, acceptMethodInto (definition), CarriedMethod, image, meta (+21 more)

### Community 108 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 109 - "NativeMethod.cpp"
Cohesion: 0.20
Nodes (20): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+12 more)

### Community 110 - "DefinitionScanner"
Cohesion: 0.36
Nodes (6): DefinitionScanner, Token, keyword, other, Equatable, Unicode

### Community 111 - "TEST"
Cohesion: 0.13
Nodes (18): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, describe(), evalBody() (+10 more)

### Community 112 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 113 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 114 - "Class Removal Rules"
Cohesion: 0.19
Nodes (15): .aoimage Image Format (version 3), ensureKernelNatives, Fixed Globals (56 Kernel classes + Processor + Smalltalk), Global Dictionary (Smalltalk), Image Load Checks, Atomic Image Save, Live Subclass Definition, Name Resolution Order (+7 more)

### Community 115 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 116 - "Fiber.cpp"
Cohesion: 0.13
Nodes (21): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, mman, pthread, array, size_t, fiberEntered() (+13 more)

### Community 117 - "Method Removal Rules"
Cohesion: 0.12
Nodes (24): Acceptance Criteria, Browser Removal (P12 remove rules), Browser Remove UI (context menu + confirmation sheet), Browser Removal XCTests, CI (GitHub Actions), GC Stress Mode (AO_GC_STRESS, gcstress/gcstress_vendor), Golden Evaluation (ao --test), Kernel Scan Test (KernelScan.MethodDictionaryValuesAreNativeMethods) (+16 more)

### Community 118 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 119 - "P12 Browser 削除 実装計画"
Cohesion: 0.16
Nodes (23): BrowserModel.select（クラス未選択を保てる）, BrowserWindow.confirmRemove, Session::forgetMethodSource, Globals::unbind, hasSubclass（生きているサブクラスの判定）, MainMenu.Actions（削除メニュー項目）, P12 Browser 削除 実装計画, Compile::removeClassNamed (+15 more)

### Community 120 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 121 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 122 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 123 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 124 - "AoSpan"
Cohesion: 0.10
Nodes (36): BrowserWindow.performRemoveClass(_:), BrowserWindow.performRemoveMethod(_:ofClassID:meta:), BrowserWindow.removeAfterConfirming(...), BrowserWindow.removeMethod / removeMethod(atRow:), testRemoveMethodAfterConfirmUpdatesLists, ao_accept_method_id, ao_browser_class_id, ao_remove_class (+28 more)

### Community 125 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 126 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 127 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 128 - "Failure Aborts Evaluation (abortEvaluation)"
Cohesion: 0.12
Nodes (17): Failure Aborts Evaluation (abortEvaluation), ao_eval, Host GUI (AppKit), Debug Snapshot Capture, Debugger Readout ABI (ao_debug_*), Debugger Window, Evaluation Drain (1000 yields), Empty OOP (native failure marker) (+9 more)

### Community 129 - "P11-06: Debugger 窓の操作、Debug it、保存の拒否の警告"
Cohesion: 0.13
Nodes (29): Scheduler.hpp/.cpp (switchTo, quiet/normal abort分岐), Send.hpp/.cpp: abortEvaluation / abortEvaluationQuiet, bridge/ao_abi.h: ao_debug_* 宣言, LaunchSet.swift: ao_set_debug_capture(1), WorkspaceWindow.swift: installErrorField/openDebugger, AO_ERR_HALT / AO_EVAL_DEBUGIT / AO_DEBUG_POSTMORTEM / AO_DEBUG_LIVE, p11-ancient-matsumoto.md (計画書), P11-01: SPEC の詳細化、PR 指示書、PHASE、ABI の定数 (+21 more)

### Community 130 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 131 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 132 - "unwinding"
Cohesion: 0.20
Nodes (28): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+20 more)

### Community 133 - "FiberRegs"
Cohesion: 0.12
Nodes (16): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+8 more)

### Community 134 - "Ast"
Cohesion: 0.08
Nodes (27): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+19 more)

### Community 135 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 137 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 138 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 139 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 140 - "Bootstrap"
Cohesion: 0.17
Nodes (13): Bootstrap, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), namedSlotNames() (+5 more)

### Community 141 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 142 - "C ABI (bridge/ao_abi.h)"
Cohesion: 0.47
Nodes (6): ao_image_load, AoSpan, Bridge (runtime-app boundary), C ABI (bridge/ao_abi.h), Runtime Session, Common Removal Rules (AO_ERR not AO_ERR_COMPILE)

### Community 143 - "Kernel Classes (NativeMethod required)"
Cohesion: 0.10
Nodes (23): Ao Bytecode Set, Ao SPEC (macOS native Smalltalk), Blue Book (Smalltalk-80: The Language and its Implementation), Blue Book Conformance Definition, Bootstrap Procedure, Broken Superclass Chain Rule (1024 limit), Class Names as Interned Symbols, = and hash Contract (+15 more)

### Community 144 - "Ao 実装文書 index (docs/README.md)"
Cohesion: 0.14
Nodes (30): P4 — Kernelネイティブ実装, CompiledMethod — バイトコード生成物, P5 — コンパイラ, Interpreter / bytecode ループ (MethodContext・BlockContext), P6 — インタプリタ, P4-01: Object / UndefinedObject / Boolean, Object / UndefinedObject / Boolean のネイティブ実装, Behavior / ClassDescription / Class / Metaclass のネイティブ実装 (+22 more)

### Community 145 - "Deferred vendor methods list"
Cohesion: 0.16
Nodes (14): B7: Compiler syntax and chunk format fixes, Backquote compile-time literal unsupported, Bag size not in P9 golden, Brace array {...} syntax unsupported by v1 compiler, Deferred vendor methods list, FileStream subclass does not exist, Class>>selector: reason line format (SPEC §3.12), MappedCollection absent from pinned sources (+6 more)

### Community 146 - "ao_main.cpp"
Cohesion: 0.13
Nodes (23): climits, dyld, addRoots, string, VendorClassFile, chunkText, className, superName (+15 more)

### Community 147 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 148 - "TEST"
Cohesion: 0.12
Nodes (14): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+6 more)

### Community 149 - "Interpreter.cpp"
Cohesion: 0.23
Nodes (13): boolean(), branchTruth(), clearNonlocal(), consumeNonlocal(), int64_t, DepthGuard, outermost, hit() (+5 more)

### Community 150 - "TEST"
Cohesion: 0.09
Nodes (27): BlockAssignmentIsBindingStore, BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), string (+19 more)

### Community 152 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 153 - "Reentry"
Cohesion: 0.14
Nodes (14): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), reenterEverything() helper, Reentry (+6 more)

### Community 154 - "Compile.cpp"
Cohesion: 0.15
Nodes (30): cctype, CompileError, message, span, string, acceptClassSource(), applyChunks(), assignError() (+22 more)

### Community 155 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 156 - "P3 — ネイティブディスパッチ"
Cohesion: 0.16
Nodes (15): MethodDictionary / lookup (IC→class cache→辞書→DNU), NativeMethod — ネイティブメソッドディスパッチ, P3 — ネイティブディスパッチ, NativeMethod のシンボル名再結合 (イメージロード時), P3-01: Symbol intern, Symbol intern, P3-02: MethodDictionary, MethodDictionary (+7 more)

### Community 157 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, next, IntegerBeyondInt64KeepsItsDigits, IntegerMantissaWithExponentIsInteger, TEST(), EighteenDigitIntegerIsExactInt64 (+5 more)

### Community 158 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 159 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 160 - "Scanner"
Cohesion: 0.23
Nodes (13): uint32_t, Scanner, i_, lexBinary, lexCharacter, lexIdentOrKeyword, lexNumber, lexString (+5 more)

### Community 161 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 162 - "Cooperative Process Scheduler"
Cohesion: 0.17
Nodes (12): Abandon (terminate without cleanup), allocateRetry (GC-capable allocation), Base Process, Cooperative Process Scheduler, Deadlock: no runnable process, Fiber Process Stacks (8 MiB + guard page), Full GC Triggers, GC Roots (+4 more)

### Community 163 - "Class Definition Re-Accept"
Cohesion: 0.29
Nodes (8): ao_accept_class, Class Definition Re-Accept, Class Variables (classPool of Associations), Damaged Table Rule, Hash Home Mixing (Fibonacci multiplier), Dictionary and Set Hash Tables (open addressing), Shape Change (recompile and move methods), Table Generation Number (reentrancy detection)

### Community 166 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 167 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (10): ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext, uint32_t (+2 more)

### Community 168 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 169 - "performSend"
Cohesion: 0.24
Nodes (8): フレーム連鎖（P10-03）, deque, OperandStack, roots, slots, answerWithoutSend(), WellKnown, performSend()

### Community 170 - "Post-mortem debugger"
Cohesion: 0.67
Nodes (3): ao_set_debug_capture / ao_debug_*, Object>>halt, Post-mortem debugger

### Community 171 - "Ao Smalltalk 概要 (JA)"
Cohesion: 0.15
Nodes (15): Graphify Required Tool, Serena Required Tool, Standard Implementation Workflow (8 steps), AO_SANITIZE Option, compiler/ Subdirectory (ao_compiler target), ao CMake Project, runtime/ Subdirectory (ao_runtime target), リリース手順 (JA) (+7 more)

### Community 172 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 173 - "send2"
Cohesion: 0.11
Nodes (19): ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RebindsBagAndEvaluatesInstVar, RectangleWidthHeightContainsAndIntersect (+11 more)

### Community 174 - "Temps"
Cohesion: 0.21
Nodes (7): Roots, uint32_t, unique_ptr, Temps, n, roots, slots

### Community 176 - "P0 (親フェーズ, stub)"
Cohesion: 0.26
Nodes (12): P0 (親フェーズ, stub), P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+4 more)

### Community 177 - "P2 — ブートストラップ"
Cohesion: 0.24
Nodes (11): P2 — ブートストラップ, WellKnown.hpp — well-known クラス表, P2-01: WellKnown と即値クラス, WellKnown 表と即値クラス, クラス骨格の割り当て, P2-02: クラス骨格の割り当て, Smalltalk-80 Blue Book（メタクラス規則, 章 6–10）, P2-03: メタクラス循環（Blue Book 6–10） (+3 more)

### Community 178 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 179 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 180 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 181 - "exactFloat"
Cohesion: 0.31
Nodes (11): int64_t, string_view, exactFloat(), magBits(), magCmp(), magShl(), magSub(), nearestDouble() (+3 more)

### Community 182 - "P8 — AppKit ツール"
Cohesion: 0.29
Nodes (11): P8 — AppKit ツール, P8-01: Ao.app 骨格, Ao.app 骨格, P8-02: Transcript ウィンドウ, Transcript ウィンドウ, P8-03: Workspace ウィンドウ, Workspace ウィンドウ, P8-04: System Browser 5 ペイン (+3 more)

### Community 183 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 184 - "Frame"
Cohesion: 0.29
Nodes (7): ContextExitGuard, Frame, Roots, FieldRoots, frame, FrameLink, stepCheck()

### Community 185 - "Bytecode interpreter"
Cohesion: 0.28
Nodes (9): Ao バイトコード ISA, Codegen（CompiledMethod 生成）, Context GC roots, MethodContext / BlockContext, compiler_roundtrip_test, Bytecode interpreter, Interpreter::run, ao --test (+1 more)

### Community 186 - "Scheduler::fork"
Cohesion: 0.25
Nodes (9): addFiber, enqueue, find, liveFibers, Scheduler::fork(), Scheduler::forkEval(), block, Scheduler::resume() (+1 more)

### Community 187 - "Imported Class Library (no self-authored library)"
Cohesion: 0.25
Nodes (8): ao_accept_method, SPEC Change Rules, Constraints (new VM, minimal deps, no copying), Cuis Smalltalk, File-in Errors and DEFERRED.md, Graphify and Serena Required, Startup and Vendor Bundling, Imported Class Library (no self-authored library)

### Community 188 - "Session Source Table (not in image)"
Cohesion: 0.22
Nodes (9): ao_browser_source (placeholder), Chunk Format, CompiledMethod, Compiler (.st to CompiledMethod), Inline Expansion (ifTrue:/whileTrue:/to:do:), pc-to-Source Map and Temp Names, Session Source Table (not in image), Statement Start Table (+1 more)

### Community 189 - "P10 — 事後デバッガ"
Cohesion: 0.25
Nodes (8): P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 範囲, 結論

### Community 190 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size, Heap::header()

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
Cohesion: 0.20
Nodes (12): P9 — 統合, P9-01: Do it / Print it / Inspect it, Do it / Print it / Inspect it, P9-02: Browser accept, Browser accept, P9-03: エラー表示と VoiceOver, エラー表示と VoiceOver, P9-04 v1 Golden Acceptance (+4 more)

### Community 195 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 196 - "native_send_test.cpp"
Cohesion: 0.54
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 197 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 198 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 199 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 200 - "compiler/tests/smoke_test.cpp"
Cohesion: 0.40
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 201 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 202 - "Live Debugger (evaluation process, halting)"
Cohesion: 0.29
Nodes (8): Debugger (P10/P11), Evaluation Process, Frame Kinds and Labels, Live Debugger (evaluation process, halting), Live Debugger ABI (proceed/step/abort), P10 Post-mortem Debugger Phase, P11 Live Debugger Phase, Implementation Phases P0-P12

### Community 203 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 204 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 205 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 206 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 207 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 209 - "Rules That Do Not Bend"
Cohesion: 0.67
Nodes (3): Fixed Design Decisions, 曲げない規則, Rules That Do Not Bend

### Community 210 - "P11 — ライブデバッガ"
Cohesion: 0.29
Nodes (7): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 範囲, 結論

### Community 211 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 213 - "WriteStream String Writes (reserve capacity)"
Cohesion: 0.67
Nodes (3): Streams (PositionableStream/WriteStream), String do: UTF-8 Single Pass, WriteStream String Writes (reserve capacity)

### Community 238 - "ActiveGuard"
Cohesion: 0.24
Nodes (5): ActiveGuard, rootShared, saved, NonlocalGuard, rootShared

### Community 239 - "SelectedFrames"
Cohesion: 0.29
Nodes (5): debugReason(), SelectedFrames, frames_, live_, none_

### Community 240 - "PcSpan"
Cohesion: 0.40
Nodes (6): uint32_t, PcSpan, end, pc, start, debugSpanAt()

### Community 241 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 242 - "StackPool"
Cohesion: 0.40
Nodes (5): array, kPoolLimit, StackPool, count, stacks

### Community 243 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 244 - "assemble"
Cohesion: 0.50
Nodes (4): initializer_list, assemble(), uint8_t, op()

### Community 245 - "Live debugger"
Cohesion: 0.67
Nodes (3): Ao.app Debugger window, Live debugger, ao_set_debug_mode(AO_DEBUG_LIVE) / AO_ERR_HALT

### Community 246 - "SlotNames"
Cohesion: 0.67
Nodes (3): SlotNames, names, WellKnown

### Community 342 - "Session"
Cohesion: 0.06
Nodes (35): browserClassCount(), ClassId, cls, id, classIdRootSlots(), debugCanProceed(), debugFrameCount(), debugFrameTotal() (+27 more)

### Community 354 - "keyWithHome"
Cohesion: 0.33
Nodes (6): Hashed::home() hash-mixing home function, set, uint32_t, HashedHome.MixesTheHashBeforeTakingTheTopBits test, KernelBench Dictionary/Set/IdentitySet benchmarks, keyWithHome()

## Ambiguous Edges - Review These
- `Smalltalk-80 removeFromSystem` → `Blue Book (Smalltalk-80: The Language and its Implementation)`  [AMBIGUOUS]
  docs/superpowers/specs/2026-09-26-browser-remove-design.md · relation: conceptually_related_to
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

## Knowledge Gaps
- **1020 isolated node(s):** `.selectedClass`, `.metaFlag`, `.canRemoveMethod`, `.canRemoveClass`, `.hasUnacceptedChanges` (+1015 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2631 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **127 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Smalltalk-80 removeFromSystem` and `Blue Book (Smalltalk-80: The Language and its Implementation)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
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
# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 282 files · ~281,471 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5616 nodes · 15090 edges · 234 communities (213 shown, 21 thin omitted)
- Extraction: 85% EXTRACTED · 15% INFERRED · 0% AMBIGUOUS · INFERRED: 2320 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `47c6ede6`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Codegen.cpp
- .fromSmallInteger
- LargeInteger.cpp
- TEST
- Scanner.cpp
- VendorExtract.cpp
- Oop
- send
- ao_eval
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- String.cpp
- CompileError
- 指摘
- ChunkAction
- Heap
- TEST_F
- Geometry.cpp
- TEST
- docs/README.md
- Boot
- Scheduler
- TEST
- WorkspaceWindow
- ClassPool.cpp
- makeToolTextWindow
- P3 — Native Dispatch
- Bytecode interpreter
- gc_safety_test.cpp
- TEST
- TEST
- image_save_load_test.cpp
- ChunkParser.cpp
- string
- TEST_F
- TEST
- BrowserWindow
- P2-02 class object skeletons
- P6b — vendor file-in
- ImageSave.cpp
- P9 — Integration
- TEST
- Ast
- Loaded
- CallContext
- TEST
- MethodSource
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- アプリ
- Parser
- TEST_F
- ao_image_load
- TEST
- TEST
- SmallInteger.cpp
- roots
- putNative
- Session.cpp
- ToolWindowTests
- TEST
- wellknown
- .publish
- DiskHeader
- TEST
- CompileEnv
- WellKnown.cpp
- Ao.app skeleton
- TEST
- DebugSnapshot
- TEST
- TEST
- Token
- string
- TEST
- ao image save
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- BrowserModel
- TEST_F
- .true_
- TEST
- native_send_test.cpp
- abi.cpp
- TEST
- Bootstrap.cpp
- Stream.cpp
- build.sh
- test.sh
- SPEC.md
- Session
- .isTrue
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- MethodImage
- .build
- HashedCollection.cpp
- P11 — ライブデバッガ
- Memory maintenance guide
- string
- string_view
- Deferred
- TranscriptWindow
- native_method_test.cpp
- Qiita-part2.md
- ImageLoad.cpp
- OperandStack
- TEST
- AppKit
- ImageFormat
- TEST
- Gc
- Boolean.cpp
- TEST
- size
- ImageSurgery
- TEST
- Interpreter.cpp
- Roots
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- P1-06: weak slot GC
- DefinitionScanner
- vector
- main
- uint64_t
- package-app.sh
- NativeMethod.cpp
- path
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- Reentry
- refreshStackLimit
- Stack
- reshapeClass
- FiberStack
- 指摘
- applyChunks
- TEST
- Scheduler::Record
- uint64_t
- Process.cpp
- TEST
- P4 — Kernel Native Implementation
- interpreter
- DebugSnapshot.cpp
- TEST
- Roots.cpp
- GarbageFirstBoot
- FiberRegs
- Fiber.cpp
- fiber_test.cpp
- TEST
- sessionImageLoad
- FileSizeLimit
- allocateRetry
- send2
- cli_test.sh
- .nil
- RootedArray
- BrowserModelTests
- .fromBits
- P7 .aoimage Implementation Plan
- abortingNew
- [1.0.0] - 2026-09-26
- intern
- 横断テーマ
- performSend
- Frame
- abortEvaluation
- Send.cpp
- CompiledMethodNatives.cpp
- .isHeap
- 手順（PR の順序と依存）
- .base
- Counts
- Dbg
- TestDir
- sessionEval
- ClassMethodCache
- imageRegistryStubA
- TEST
- Range
- DebugFrames
- specialIndex
- CountingSink
- Roots::visitAll
- .specialSelector
- boxLiteral
- popFrame
- NativeFrame
- BlockProbe
- P10-01: SPEC と CLAUDE.md の改訂、PHASE
- TEST
- PingPong
- P9-01: Do it / Print it / Inspect it
- loadedImageProbes
- codegen
- ObjectHeader
- TEST
- Qiita-part1.md
- README.md
- .isBase
- DepthGuard
- StackPool
- TEST
- Root
- Frame
- Character.cpp
- HashedCollection
- WellKnown::InternTable
- P1-07: immovable old objects
- 仕様
- NativeMethod.hpp
- IgnoreFileSizeSignal
- acceptClass
- runOnSmallStack
- RootedArray::RootedArray
- smallIntegerAnswer
- NameBag
- Scheduler::recordFailure
- evalExpr
- expectNonBooleanAbort

## God Nodes (most connected - your core abstractions)
1. `Oop` - 918 edges
2. `Heap` - 252 edges
3. `vector` - 172 edges
4. `WellKnown` - 167 edges
5. `Ast` - 104 edges
6. `Boot` - 101 edges
7. `TEST()` - 100 edges
8. `Session` - 98 edges
9. `TEST()` - 96 edges
10. `TEST()` - 96 edges

## Surprising Connections (you probably didn't know these)
- `テスト` --references--> `WorkspaceEvalTests`  [INFERRED]
  docs/prs/P10-06.md → app/AoTests/WorkspaceEvalTests.swift
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない` --references--> `isBinaryChar()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/Scanner.cpp
- `前提` --references--> `DebugFrames`  [INFERRED]
  docs/phases/P11.md → runtime/include/ao/DebugSnapshot.hpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **P3 native dispatch path** — docs_phases_p3_lookup, docs_phases_p3_methoddictionary, docs_phases_p3_nativemethod, docs_phases_p3_dnu [EXTRACTED 1.00]
- **P1 object memory stack** — docs_phases_p1_ao_oop, docs_phases_p1_heap, docs_phases_p1_gc, docs_phases_p1_nursery [EXTRACTED 1.00]
- **P1 PR implementation chain** — docs_prs_p1_01, docs_prs_p1_02, docs_prs_p1_03, docs_phases_p1 [EXTRACTED 1.00]
- **Mandatory agent tooling trio** — concept_graphify, concept_serena, claude [EXTRACTED 1.00]
- **Runtime app ABI boundary** — concept_libao_runtime, concept_ao_app, concept_c_abi_boundary, bridge_ao_abi [EXTRACTED 1.00]
- **P2 bootstrap pipeline** — docs_prs_p2_01_pr, docs_prs_p2_02_pr, docs_prs_p2_03_pr, docs_prs_p2_04_pr [EXTRACTED 1.00]
- **P3 method dispatch pipeline** — docs_prs_p3_01_pr, docs_prs_p3_02_pr, docs_prs_p3_03_pr, docs_prs_p3_04_pr, docs_prs_p3_05_pr [EXTRACTED 1.00]
- **P4 Kernel NativeMethod suite** — docs_prs_p4_01_pr, docs_prs_p4_02_pr, docs_prs_p4_03_pr, docs_prs_p4_04_pr, docs_prs_p4_05_pr, docs_prs_p4_06_pr, docs_prs_p4_07_pr, docs_prs_p4_08_pr, docs_prs_p4_09_pr [EXTRACTED 1.00]
- **P5 compiler pipeline** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunk_parser [EXTRACTED 1.00]
- **AOIM save/load and native rebind** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_image_save, docs_prs_p7_03_image_load, docs_prs_p7_02_nativemethod_symbol_names [EXTRACTED 1.00]
- **AppKit tool windows and menu** — docs_prs_p8_01_ao_app, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [EXTRACTED 1.00]
- **C ABI wrappers over ao::Runtime** — runtime_src_abi_ao_version, runtime_src_abi_ao_runtime_boot, runtime_src_abi_ao_runtime_shutdown [EXTRACTED 1.00]

## Communities (234 total, 21 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (61): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+53 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (89): ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_() (+81 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (46): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks (+38 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.08
Nodes (61): Task 4: LOAD_ORDER とホストパッチ, string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult (+53 more)

### Community 7 - "Oop"
Cohesion: 0.09
Nodes (65): [High] printOn: が新しい printString を使わない, Graphify / Serena, bits(), isBytes(), isIndexable(), isPointers(), make(), Oop (+57 more)

### Community 8 - "send"
Cohesion: 0.17
Nodes (36): int64_t, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_() (+28 more)

### Community 9 - "ao_eval"
Cohesion: 0.04
Nodes (75): AoSpan, end, message, start, 00 直近差分（origin/main..HEAD の 6 コミット）, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, 概要, 01 オブジェクトメモリ・GC・ブートストラップ (+67 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (17): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+9 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 13 - "Literal"
Cohesion: 0.11
Nodes (19): int64_t, LitKind, unique_ptr, Literal, bytes, elements, floatValue, intValue (+11 more)

### Community 14 - "Float.cpp"
Cohesion: 0.08
Nodes (67): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+59 more)

### Community 15 - "String.cpp"
Cohesion: 0.12
Nodes (39): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, ao_String_asSymbol(), ao_String_at_() (+31 more)

### Community 16 - "CompileError"
Cohesion: 0.14
Nodes (17): CompileError, message, span, string, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, Task 10: accept, ソース表の拡張（P10-04）, acceptClassSource() (+9 more)

### Community 17 - "指摘"
Cohesion: 0.18
Nodes (11): [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない, [High] 一度未宣言だった名前が、後から定義したクラスを隠す (+3 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (54): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_ (+46 more)

### Community 20 - "TEST_F"
Cohesion: 0.06
Nodes (40): BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, ClearDropsRoots, アプリ, DoItFrameSourceDropsPrefix, EvalErrorFillsFramesInnermostFirst (+32 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "docs/README.md"
Cohesion: 0.10
Nodes (31): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator (+23 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (57): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, expectAbortedEmpty() (+49 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (35): SPEC §3.4 に書く意味論, CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+27 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (60): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+52 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.12
Nodes (17): sendToKeyWorkspace(), Bool, Int, NSWindow, Void, WorkspaceWindow, .errorAccessibilityLabel, .errorText (+9 more)

### Community 28 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (22): hashedcollection, adopt(), bindingAt(), int64_t, string, string_view, uint32_t, WellKnown (+14 more)

### Community 29 - "makeToolTextWindow"
Cohesion: 0.09
Nodes (21): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int, Int32, NSFont (+13 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "gc_safety_test.cpp"
Cohesion: 0.10
Nodes (25): classpool, cstdio, dyld, filesystem, fstream, future, install, iterator (+17 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.13
Nodes (26): ao_abi, chrono, Chunk, cmath, compile, CompiledMethod, compiler, context (+18 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.08
Nodes (16): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+8 more)

### Community 38 - "TEST_F"
Cohesion: 0.03
Nodes (60): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+52 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.10
Nodes (24): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+16 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.10
Nodes (21): Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, Behavior, Bootstrap, Class (+13 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.14
Nodes (18): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+10 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (45): cerrno, climits, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord() (+37 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.06
Nodes (30): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+22 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.11
Nodes (19): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+11 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (34): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+26 more)

### Community 49 - "TEST"
Cohesion: 0.08
Nodes (25): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+17 more)

### Community 50 - "MethodSource"
Cohesion: 0.08
Nodes (25): uint32_t, PcSpan, end, pc, start, DebugInfoRef, body, index (+17 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.18
Nodes (26): P4 microbench, P6 interpreter bench, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value() (+18 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (32): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+24 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "アプリ"
Cohesion: 0.08
Nodes (38): Void, InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, aoWorkspaceInspectHook() (+30 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 58 - "TEST_F"
Cohesion: 0.06
Nodes (34): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+26 more)

### Community 59 - "ao_image_load"
Cohesion: 0.06
Nodes (57): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, B2 `to:do:` bench, [High] Save / Open Image の失敗がユーザーに見えない, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない (+49 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "TEST"
Cohesion: 0.12
Nodes (20): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, firstBlock(), named(), pcOf() (+12 more)

### Community 62 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 64 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+26 more)

### Community 65 - "Session.cpp"
Cohesion: 0.17
Nodes (32): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+24 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.11
Nodes (12): LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String, T (+4 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".publish"
Cohesion: 0.08
Nodes (17): Any, Int, Notification, NSRect, NSSegmentedControl, NSTableView, NSTextField, NSTextView (+9 more)

### Community 70 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "CompileEnv"
Cohesion: 0.08
Nodes (29): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+21 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (31): findSymbol, global, internWith, Roots, string_view, WellKnown, ImageSelector, name (+23 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 76 - "DebugSnapshot"
Cohesion: 0.06
Nodes (37): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kMaxFrames (+29 more)

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 79 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 80 - "string"
Cohesion: 0.18
Nodes (29): argCountOf(), byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), string, uint32_t (+21 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 84 - "BrowserModel"
Cohesion: 0.20
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 86 - ".true_"
Cohesion: 0.21
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (80): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+72 more)

### Community 88 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 89 - "abi.cpp"
Cohesion: 0.07
Nodes (48): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, 範囲, Browser の読み取り, Task 3: Browser 読み取り ABI, B5 Browser と Workspace のデータ消失（テーマ 4） (+40 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (74): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+66 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.24
Nodes (17): allocateSkeletons(), allocClass(), Roots, string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors() (+9 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.10
Nodes (67): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_() (+59 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 96 - "Session"
Cohesion: 0.07
Nodes (42): kind, pc, attachBlocks(), blockLiteral(), blockMethods(), MethodImage, Roots, size_t (+34 more)

### Community 97 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 103 - "MethodImage"
Cohesion: 0.12
Nodes (19): int16_t, string, uint16_t, uint8_t, MethodImage, bytes, literals, numArgs (+11 more)

### Community 104 - ".build"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 106 - "P11 — ライブデバッガ"
Cohesion: 0.22
Nodes (8): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 受け入れ, 範囲, 結論

### Community 111 - "TranscriptWindow"
Cohesion: 0.09
Nodes (23): AoApp, fileInVendor(), openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification (+15 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.12
Nodes (45): ifstream, readHeader, acceptWord(), atOffset(), bindAll(), checkFile(), checkGlobals(), byte (+37 more)

### Community 115 - "OperandStack"
Cohesion: 0.12
Nodes (14): ファイルごとの変更, deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots (+6 more)

### Community 116 - "TEST"
Cohesion: 0.13
Nodes (18): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, describe(), evalBody() (+10 more)

### Community 117 - "AppKit"
Cohesion: 0.15
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "ImageFormat"
Cohesion: 0.12
Nodes (18): uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes, kImagePointerBits (+10 more)

### Community 119 - "TEST"
Cohesion: 0.05
Nodes (55): AllocateNoGcSpillsToOld, ByteObjectPayload, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty (+47 more)

### Community 120 - "Gc"
Cohesion: 0.06
Nodes (47): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API (+39 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.22
Nodes (28): マングル表, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_() (+20 more)

### Community 122 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 123 - "size"
Cohesion: 0.24
Nodes (15): allocateNoGc, size, at(), bind(), bindIn(), Roots, string_view, uint32_t (+7 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter.cpp"
Cohesion: 0.16
Nodes (30): ActiveGuard, rootShared, saved, branchTruth(), byteCount(), clearNonlocal(), consumeNonlocal(), contextAlive() (+22 more)

### Community 127 - "Roots"
Cohesion: 0.08
Nodes (25): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+17 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (22): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+14 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.19
Nodes (23): ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+15 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.19
Nodes (13): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+5 more)

### Community 132 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 133 - "DefinitionScanner"
Cohesion: 0.23
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 134 - "vector"
Cohesion: 0.16
Nodes (32): cctype, vector, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Task 3: rebind と上書き禁止, FileInError, error, file, method (+24 more)

### Community 135 - "main"
Cohesion: 0.09
Nodes (27): Build, Commits, Contributing, いまの位置, コミット, ビルド, ライセンス, リリース (+19 more)

### Community 136 - "uint64_t"
Cohesion: 0.28
Nodes (13): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+5 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.33
Nodes (14): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+6 more)

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.13
Nodes (14): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B7 コンパイラの構文とチャンク形式, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, バッチの順序と依存 (+6 more)

### Community 141 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 142 - "Reentry"
Cohesion: 0.15
Nodes (13): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), Reentry, codes (+5 more)

### Community 143 - "refreshStackLimit"
Cohesion: 0.24
Nodes (13): refreshStackLimit(), ao_AoTest_assert_equals_(), CallContext, path, Root, string, string_view, uint32_t (+5 more)

### Community 144 - "Stack"
Cohesion: 0.10
Nodes (19): FrameBlock, capacity, slots, used, size_t, unique_ptr, attached, Stack (+11 more)

### Community 145 - "reshapeClass"
Cohesion: 0.17
Nodes (21): CarriedMethod, image, meta, selector, source, carriedMethodName(), compileCarried(), copyMethodDictionary() (+13 more)

### Community 146 - "FiberStack"
Cohesion: 0.14
Nodes (17): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, FiberStack, acquire, base_ (+9 more)

### Community 147 - "指摘"
Cohesion: 0.12
Nodes (17): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む (+9 more)

### Community 148 - "applyChunks"
Cohesion: 0.11
Nodes (20): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 5: `ao filein` と評価, Task 6: フェーズ完了 (+12 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.09
Nodes (23): Scheduler, uint64_t, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+15 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.04
Nodes (49): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+41 more)

### Community 156 - "DebugSnapshot.cpp"
Cohesion: 0.18
Nodes (21): contextPc(), CallContext, Frame, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method() (+13 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (52): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+44 more)

### Community 158 - "Roots.cpp"
Cohesion: 0.14
Nodes (17): size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt() (+9 more)

### Community 159 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 160 - "FiberRegs"
Cohesion: 0.14
Nodes (14): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+6 more)

### Community 161 - "Fiber.cpp"
Cohesion: 0.17
Nodes (17): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch() (+9 more)

### Community 162 - "fiber_test.cpp"
Cohesion: 0.16
Nodes (17): csignal, mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest (+9 more)

### Community 163 - "TEST"
Cohesion: 0.11
Nodes (19): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+11 more)

### Community 164 - "sessionImageLoad"
Cohesion: 0.24
Nodes (10): Image, check, load, save, boot(), clearMethodSources(), ensureKernelNatives(), sessionBoot() (+2 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "allocateRetry"
Cohesion: 0.10
Nodes (29): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, 指摘, [Medium] `shallowCopy`・`printString`・`asSymbol` が、nursery が足りないときに GC を再試行せず失敗する (+21 more)

### Community 167 - "send2"
Cohesion: 0.11
Nodes (19): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+11 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".nil"
Cohesion: 0.16
Nodes (40): flags, slotAt, slotAtPut, enqueue, find, CallContext, int64_t, Record (+32 more)

### Community 170 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 171 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 172 - ".fromBits"
Cohesion: 0.25
Nodes (6): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, TEST(), VersionOneIsRefusedWithReason

### Community 173 - "P7 .aoimage Implementation Plan"
Cohesion: 0.17
Nodes (12): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 5: CLI, Task 6: フェーズを閉じる, リスク (+4 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "[1.0.0] - 2026-09-26"
Cohesion: 0.22
Nodes (9): [1.0.0] - 2026-09-26, Ao.app, `ao` CLI, Changelog, Class library and images, Compiler and interpreter, Known limitations, Project (+1 more)

### Community 176 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 177 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 178 - "performSend"
Cohesion: 0.16
Nodes (16): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, Files, Interfaces, P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt, コミット文, 完了条件, 手順, 目的 (+8 more)

### Community 179 - "Frame"
Cohesion: 0.15
Nodes (13): Frame, context, isBlock, method, pc, prev, receiver, sendArgc (+5 more)

### Community 180 - "abortEvaluation"
Cohesion: 0.10
Nodes (31): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 受け入れ (+23 more)

### Community 181 - "Send.cpp"
Cohesion: 0.10
Nodes (24): algorithm, Bootstrap, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, format, lookup, MethodDictionary, InlineCache, cachedClass (+16 more)

### Community 182 - "CompiledMethodNatives.cpp"
Cohesion: 0.41
Nodes (13): CompiledMethod accessors (P5), Native selector mangling, 規則, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps() (+5 more)

### Community 183 - ".isHeap"
Cohesion: 0.15
Nodes (21): clearWeakAfterOldMark, isClassShaped(), superclassOf(), anyMethodIn(), CallContext, pair, size_t, Visit (+13 more)

### Community 184 - "手順（PR の順序と依存）"
Cohesion: 0.16
Nodes (17): Files, P10-04: セッションのソース表の拡張と doIt, コミット文, テスト, 完了条件, 手順, 目的, P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む） (+9 more)

### Community 185 - ".base"
Cohesion: 0.27
Nodes (11): afterResume, leaveLists, switchTo, takeNext, yield, Scheduler::abandonAll(), Scheduler::block(), Scheduler::drain() (+3 more)

### Community 186 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 187 - "Dbg"
Cohesion: 0.29
Nodes (3): Dbg, b, sink

### Community 188 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 189 - "sessionEval"
Cohesion: 0.13
Nodes (22): Files, clearUnwinding(), blankOut(), bumpDebugGeneration(), AoInspectFn, optional, debugClear(), DebugEntry (+14 more)

### Community 190 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 193 - "Range"
Cohesion: 0.50
Nodes (4): Range, first, n, Roots::popRange()

### Community 194 - "DebugFrames"
Cohesion: 0.20
Nodes (9): DebugFrames, count, kind, method, pc, receiver, selector, temp (+1 more)

### Community 195 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 196 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 197 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 199 - "boxLiteral"
Cohesion: 0.32
Nodes (8): [High] 2^63 以上の整数リテラルが黙って 0 になる, Interfaces, boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), uint32_t, uint8_t

### Community 200 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 201 - "NativeFrame"
Cohesion: 0.50
Nodes (4): Roots, NativeFrame, argc, slots

### Community 202 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 203 - "P10-01: SPEC と CLAUDE.md の改訂、PHASE"
Cohesion: 0.12
Nodes (16): Files, Interfaces, P10-01: SPEC と CLAUDE.md の改訂、PHASE, コミット文, テスト, 完了条件, 手順, 目的 (+8 more)

### Community 204 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 205 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 206 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 207 - "loadedImageProbes"
Cohesion: 0.22
Nodes (11): Interfaces, P10-05: ao_debug_* ABI, コミット文, テスト, 完了条件, 手順, 目的, HostOopHook (+3 more)

### Community 208 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 209 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 210 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 211 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 212 - "README.md"
Cohesion: 0.10
Nodes (19): Cuis vendor pin, Ao, Documentation, Install, Ao, Ao.app の使い方, `ao` CLI の使い方, v1 に無いもの (+11 more)

### Community 213 - ".isBase"
Cohesion: 0.25
Nodes (8): terminateAll, Roots, size_t, rootRecord(), Scheduler::liveFibers(), Scheduler::reapDead(), Scheduler::Scheduler(), unrootRecord()

### Community 215 - "StackPool"
Cohesion: 0.22
Nodes (9): clearShadow(), array, byte, size_t, kPoolLimit, pooledCount, StackPool, count (+1 more)

### Community 216 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 217 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 218 - "Frame"
Cohesion: 0.33
Nodes (6): ContextExitGuard, Frame, Roots, FieldRoots, frame, FrameLink

### Community 219 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 220 - "HashedCollection"
Cohesion: 0.25
Nodes (5): classDefinition(), string, testing::Test, HashedCollection, randomProbeSource()

### Community 221 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 222 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 223 - "仕様"
Cohesion: 0.18
Nodes (11): CLI, extra 節, globals 節, load, OOP エンコード, save, Task 4: load と受け入れ評価, well-known 節 (+3 more)

### Community 224 - "NativeMethod.hpp"
Cohesion: 0.50
Nodes (3): DebugSink, Frame, Scheduler

### Community 225 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 226 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 227 - "runOnSmallStack"
Cohesion: 0.67
Nodes (3): テスト, function, runOnSmallStack()

### Community 228 - "RootedArray::RootedArray"
Cohesion: 0.67
Nodes (3): Roots, uint32_t, RootedArray::RootedArray()

### Community 229 - "smallIntegerAnswer"
Cohesion: 0.67
Nodes (3): boolean(), int64_t, smallIntegerAnswer()

### Community 230 - "NameBag"
Cohesion: 0.67
Nodes (3): NameBag, heap, names

## Knowledge Gaps
- **1091 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+1086 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2416 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **21 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Codegen.cpp`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `send`, `Heap.cpp`, `TEST`, `Float.cpp`, `String.cpp`, `CompileError`, `Heap`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `ClassPool.cpp`, `P3 — Native Dispatch`, `gc_safety_test.cpp`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `TEST_F`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `MethodSource`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `putNative`, `Session.cpp`, `DiskHeader`, `CompileEnv`, `WellKnown.cpp`, `DebugSnapshot`, `TEST`, `string`, `.true_`, `TEST`, `native_send_test.cpp`, `abi.cpp`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `OperandStack`, `TEST`, `TEST`, `Gc`, `Boolean.cpp`, `TEST`, `size`, `ImageSurgery`, `Interpreter.cpp`, `Roots`, `Array.cpp`, `vector`, `NativeMethod.cpp`, `path`, `TEST`, `refreshStackLimit`, `Stack`, `reshapeClass`, `applyChunks`, `Scheduler::Record`, `Process.cpp`, `TEST`, `DebugSnapshot.cpp`, `TEST`, `Roots.cpp`, `GarbageFirstBoot`, `Fiber.cpp`, `TEST`, `allocateRetry`, `send2`, `.nil`, `RootedArray`, `.fromBits`, `abortingNew`, `intern`, `performSend`, `Frame`, `abortEvaluation`, `Send.cpp`, `CompiledMethodNatives.cpp`, `.isHeap`, `手順（PR の順序と依存）`, `Dbg`, `sessionEval`, `ClassMethodCache`, `imageRegistryStubA`, `Range`, `CountingSink`, `.specialSelector`, `boxLiteral`, `popFrame`, `NativeFrame`, `BlockProbe`, `TEST`, `ObjectHeader`, `README.md`, `.isBase`, `Root`, `Character.cpp`, `WellKnown::InternTable`, `NativeMethod.hpp`, `smallIntegerAnswer`, `evalExpr`, `expectNonBooleanAbort`?**
  _High betweenness centrality (0.395) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `main`, `uint64_t`, `Oop`, `Emitter`, `NativeMethod.cpp`, `ao_eval`, `Literal`, `path`, `refreshStackLimit`, `Stack`, `reshapeClass`, `ChunkAction`, `CompileError`, `applyChunks`, `TEST_F`, `Reentry`, `Scheduler`, `ClassPool.cpp`, `DebugSnapshot.cpp`, `gc_safety_test.cpp`, `fiber_test.cpp`, `image_save_load_test.cpp`, `ChunkParser.cpp`, `string`, `TEST_F`, `ImageSave.cpp`, `Ast`, `TEST`, `MethodSource`, `Send.cpp`, `.isHeap`, `手順（PR の順序と依存）`, `Parser`, `Session.cpp`, `CompileEnv`, `DebugSnapshot`, `TEST`, `codegen`, `string`, `.true_`, `TEST`, `Bootstrap.cpp`, `Session`, `NameBag`, `MethodImage`, `ImageLoad.cpp`, `Gc`, `TEST`, `ImageSurgery`, `Roots`?**
  _High betweenness centrality (0.114) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Array.cpp`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Oop`, `send`, `NativeMethod.cpp`, `Heap.cpp`, `path`, `Float.cpp`, `String.cpp`, `TEST`, `Process.cpp`, `Boot`, `ClassPool.cpp`, `P3 — Native Dispatch`, `GarbageFirstBoot`, `gc_safety_test.cpp`, `image_save_load_test.cpp`, `string`, `.nil`, `ImageSave.cpp`, `Loaded`, `CallContext`, `intern`, `TEST`, `Send.cpp`, `BlockContext.cpp`, `.isHeap`, `putNative`, `DiskHeader`, `CompileEnv`, `WellKnown.cpp`, `TEST`, `string`, `ObjectHeader`, `native_send_test.cpp`, `Bootstrap.cpp`, `Stream.cpp`, `NativeMethod.hpp`, `Session`, `NameBag`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `TEST`, `Gc`, `size`?**
  _High betweenness centrality (0.050) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `Oop` (e.g. with `Graphify / Serena` and `現状`) actually correct?**
  _`Oop` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _1091 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013071895424836602 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.04701962236208811 - nodes in this community are weakly interconnected._
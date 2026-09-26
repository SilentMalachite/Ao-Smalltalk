# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 281 files · ~276,571 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5516 nodes · 14717 edges · 223 communities (204 shown, 19 thin omitted)
- Extraction: 85% EXTRACTED · 15% INFERRED · 0% AMBIGUOUS · INFERRED: 2234 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `69c5adcd`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Analysis
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
- Codegen.cpp
- Float.cpp
- String.cpp
- CompileError
- 指摘
- ChunkAction
- Heap
- vector
- Geometry.cpp
- TEST
- docs/README.md
- Boot
- Scheduler
- TEST
- WorkspaceWindow
- ClassPool.cpp
- TranscriptWindow
- P3 — Native Dispatch
- Bytecode interpreter
- ao_main.cpp
- TEST
- TEST
- handlescope
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
- DefinitionScanner
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
- TEST
- abi.cpp
- TEST
- .nil
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
- AoApp
- native_method_test.cpp
- Qiita-part2.md
- ImageLoad.cpp
- performSend
- takeAbortReason
- AppKit
- ImageFormat
- TEST
- collectNursery
- Boolean.cpp
- TEST
- slotAt
- ImageSurgery
- TEST
- Interpreter.cpp
- Roots
- TEST_F
- TEST
- CallContext
- VirtualRegion.cpp
- Gc
- checkFile
- Compile.cpp
- 貢献
- uint64_t
- package-app.sh
- NativeMethod.cpp
- path
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- Reentry
- runFile
- Stack
- string
- FiberStack
- 指摘
- P6b vendor file-in Implementation Plan
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
- PingPong
- TEST
- installEmptyWorkspace
- FileSizeLimit
- allocateRetry
- send2
- cli_test.sh
- Scheduler.cpp
- RootedArray
- fiber_test.cpp
- ImageFormat.cpp
- Global Constraints
- abortingNew
- [1.0.0] - 2026-09-26
- intern
- 横断テーマ
- Table
- 指摘
- abortEvaluation
- InlineCache
- .isSmallInteger
- .isHeap
- 手順（PR の順序と依存）
- P9-03: エラー表示と VoiceOver
- Counts
- Dbg
- TestDir
- sessionEval
- abortingSubclass
- imageRegistryStubA
- TEST
- Range
- DebugFrames
- Rec
- CountingSink
- Roots::visitAll
- .specialSelector
- boxLiteral
- popFrame
- NativeFrame
- DebugSink
- P10-01: SPEC と CLAUDE.md の改訂、PHASE
- P10-07: 受け入れ、CHANGELOG、Graphify deep
- SlotNames
- NamedClass
- loadedImageProbes
- codegen
- lookup_test.cpp
- TEST
- Qiita-part1.md
- README.md
- DepthGuard
- ScopedGcStressEnv
- TEST
- Root
- valueHashFold
- P7 .aoimage Implementation Plan
- FiberStackBounds
- Parsed
- RegisterSwap
- answerOne

## God Nodes (most connected - your core abstractions)
1. `Oop` - 911 edges
2. `Heap` - 252 edges
3. `vector` - 170 edges
4. `WellKnown` - 167 edges
5. `Ast` - 104 edges
6. `Boot` - 101 edges
7. `TEST()` - 100 edges
8. `TEST()` - 96 edges
9. `TEST()` - 95 edges
10. `BrowserWindow` - 91 edges

## Surprising Connections (you probably didn't know these)
- `範囲` --references--> `BrowserWindow`  [INFERRED]
  docs/superpowers/plans/2026-09-26-p10-debugger.md → app/Ao/BrowserWindow.swift
- `テスト` --references--> `WorkspaceEvalTests`  [INFERRED]
  docs/prs/P10-06.md → app/AoTests/WorkspaceEvalTests.swift
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない` --references--> `isBinaryChar()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/Scanner.cpp

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

## Communities (223 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Analysis"
Cohesion: 0.06
Nodes (46): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+38 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (89): ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_() (+81 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (71): Digits, B8 Kernel の意味論（数値とスカラー）, __int128, add(), addBig(), asInt64IfFits(), Big, d (+63 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (46): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks (+38 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.06
Nodes (74): Global Constraints, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 4: LOAD_ORDER とホストパッチ, Task 6: フェーズ完了, extra 節, string, VendorClassFile (+66 more)

### Community 7 - "Oop"
Cohesion: 0.10
Nodes (64): [High] printOn: が新しい printString を使わない, Graphify / Serena, bits(), isBytes(), isIndexable(), isPointers(), make(), Oop (+56 more)

### Community 8 - "send"
Cohesion: 0.20
Nodes (32): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+24 more)

### Community 9 - "ao_eval"
Cohesion: 0.05
Nodes (56): AoSpan, end, message, start, B2 `to:do:` bench, 01 オブジェクトメモリ・GC・ブートストラップ, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない (+48 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (17): Capture, owner, var, int16_t, Op, size_t, uint16_t, uint8_t (+9 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 13 - "Codegen.cpp"
Cohesion: 0.06
Nodes (44): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), int64_t, LitKind (+36 more)

### Community 14 - "Float.cpp"
Cohesion: 0.14
Nodes (46): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals() (+38 more)

### Community 15 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 16 - "CompileError"
Cohesion: 0.20
Nodes (16): CompileError, message, span, string, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Task 5: `ao filein` と評価, acceptClassSource() (+8 more)

### Community 17 - "指摘"
Cohesion: 0.07
Nodes (27): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す (+19 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (53): Heap, checkNotPoisoned, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_ (+45 more)

### Community 20 - "vector"
Cohesion: 0.11
Nodes (22): algorithm, ao_abi, vector, cstdlib, future, iterator, resource, namedSlotNames() (+14 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "docs/README.md"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (49): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+41 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (37): SPEC §3.4 に書く意味論, CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+29 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (62): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+54 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.12
Nodes (17): sendToKeyWorkspace(), Bool, Int, NSWindow, Void, WorkspaceWindow, .errorAccessibilityLabel, .errorText (+9 more)

### Community 28 - "ClassPool.cpp"
Cohesion: 0.20
Nodes (23): hashedcollection, adopt(), bindingAt(), CallContext, int64_t, string, string_view, uint32_t (+15 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.07
Nodes (29): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+21 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.14
Nodes (19): climits, dyld, fstream, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots (+11 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "handlescope"
Cohesion: 0.13
Nodes (30): chrono, Chunk, classpool, cmath, compile, CompiledMethod, compiler, context (+22 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.07
Nodes (19): Bootstrap, cassert, string, string_view, cstddef, cstdint, format, memory (+11 more)

### Community 38 - "TEST_F"
Cohesion: 0.03
Nodes (60): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+52 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.08
Nodes (27): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+19 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.14
Nodes (18): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+10 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (47): cerrno, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot() (+39 more)

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
Nodes (17): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+9 more)

### Community 48 - "CallContext"
Cohesion: 0.05
Nodes (48): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+40 more)

### Community 49 - "TEST"
Cohesion: 0.09
Nodes (22): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+14 more)

### Community 50 - "MethodSource"
Cohesion: 0.08
Nodes (32): uint32_t, PcSpan, end, pc, start, ソース表の拡張（P10-04）, uint32_t, DebugInfo (+24 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.24
Nodes (22): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+14 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (23): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+15 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "アプリ"
Cohesion: 0.08
Nodes (37): InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText() (+29 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 58 - "TEST_F"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 59 - "ao_image_load"
Cohesion: 0.06
Nodes (59): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, 06 イメージ・セッション・C ABI・CLI, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする (+51 more)

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
Cohesion: 0.08
Nodes (38): Task 2: 再結合に必要なヒープとレジストリの口, リスク, WellKnown, fromSlots(), installArray(), WellKnown, installBehavior(), WellKnown (+30 more)

### Community 65 - "Session.cpp"
Cohesion: 0.17
Nodes (31): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+23 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.11
Nodes (13): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+5 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - "DefinitionScanner"
Cohesion: 0.08
Nodes (23): DefinitionScanner, Any, Bool, Int, Int32, Notification, NSRect, NSTableView (+15 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 72 - "CompileEnv"
Cohesion: 0.09
Nodes (26): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+18 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (30): findSymbol, global, internWith, deque, Roots, size_t, string, string_view (+22 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 76 - "DebugSnapshot"
Cohesion: 0.06
Nodes (29): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+21 more)

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
Cohesion: 0.28
Nodes (16): byteText(), categoryHeading(), classNameOf(), classVarList(), collectKnownGlobals(), string, WellKnown, definitionCategory() (+8 more)

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
Cohesion: 0.11
Nodes (18): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, BrowserModelTests (+10 more)

### Community 85 - "TEST_F"
Cohesion: 0.05
Nodes (43): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, テスト, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually (+35 more)

### Community 86 - ".true_"
Cohesion: 0.16
Nodes (34): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+26 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (75): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+67 more)

### Community 88 - "TEST"
Cohesion: 0.11
Nodes (23): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, answerMessage(), CallContext (+15 more)

### Community 89 - "abi.cpp"
Cohesion: 0.11
Nodes (30): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at() (+22 more)

### Community 90 - "TEST"
Cohesion: 0.02
Nodes (83): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+75 more)

### Community 91 - ".nil"
Cohesion: 0.13
Nodes (24): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, allocateSkeletons(), allocClass(), Roots, string_view, WellKnown, ensureMethodDict() (+16 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.10
Nodes (67): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_() (+59 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.05
Nodes (58): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+50 more)

### Community 96 - "Session"
Cohesion: 0.09
Nodes (24): blockLiteral(), blockMethods(), size_t, CallContext, DebugSink, Roots, uint64_t, unique_ptr (+16 more)

### Community 97 - ".isTrue"
Cohesion: 0.35
Nodes (11): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext (+3 more)

### Community 103 - "MethodImage"
Cohesion: 0.12
Nodes (19): int16_t, string, uint16_t, uint8_t, MethodImage, bytes, literals, numArgs (+11 more)

### Community 104 - ".build"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.23
Nodes (20): bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t, uint64_t (+12 more)

### Community 106 - "P11 — ライブデバッガ"
Cohesion: 0.22
Nodes (8): P11 — ライブデバッガ, PR 一覧, TDD, 制約, 前提, 受け入れ, 範囲, 結論

### Community 111 - "AoApp"
Cohesion: 0.13
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.20
Nodes (18): ifstream, bindAll(), checkGlobals(), Roots, string_view, WellKnown, fileOop(), findRecord() (+10 more)

### Community 115 - "performSend"
Cohesion: 0.07
Nodes (28): フレーム連鎖（P10-03）, Frame, context, isBlock, method, pc, prev, receiver (+20 more)

### Community 116 - "takeAbortReason"
Cohesion: 0.12
Nodes (20): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, expectFailAbort(), int64_t, KernelBench, string, evalBody() (+12 more)

### Community 117 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "ImageFormat"
Cohesion: 0.09
Nodes (23): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, kImageEndianLittle, kImageFillerBytes (+15 more)

### Community 119 - "TEST"
Cohesion: 0.06
Nodes (40): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+32 more)

### Community 120 - "collectNursery"
Cohesion: 0.20
Nodes (15): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint() (+7 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 123 - "slotAt"
Cohesion: 0.16
Nodes (22): allocateNoGc, size, slotAt, at(), bind(), bindIn(), Roots, string_view (+14 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter.cpp"
Cohesion: 0.10
Nodes (42): run, ActiveGuard, rootShared, saved, applyMethod(), boolean(), branchTruth(), byteCount() (+34 more)

### Community 127 - "Roots"
Cohesion: 0.08
Nodes (25): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+17 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (23): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+15 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "CallContext"
Cohesion: 0.14
Nodes (24): B9 Kernel の意味論（コレクションとストリーム）, valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_() (+16 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 132 - "Gc"
Cohesion: 0.12
Nodes (18): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+10 more)

### Community 133 - "checkFile"
Cohesion: 0.31
Nodes (16): atOffset(), checkFile(), byte, size_t, string, T, uint32_t, fail() (+8 more)

### Community 134 - "Compile.cpp"
Cohesion: 0.12
Nodes (48): cctype, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, Task 3: rebind と上書き禁止, FileInError, error, file, method, string (+40 more)

### Community 135 - "貢献"
Cohesion: 0.29
Nodes (7): いまの位置, コミット, ビルド, ライセンス, リリース, 曲げない規則, 貢献

### Community 136 - "uint64_t"
Cohesion: 0.30
Nodes (13): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+5 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.30
Nodes (15): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+7 more)

### Community 139 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 140 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.13
Nodes (14): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B5 Browser と Workspace のデータ消失（テーマ 4）, B7 コンパイラの構文とチャンク形式, Claude レビュー指摘の修正計画（docs/claude-review）, Context, バッチの順序と依存, ファイバ (+6 more)

### Community 141 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 142 - "Reentry"
Cohesion: 0.13
Nodes (15): BusyRead, copies, evalCodes, lengths, path, string, expectAllRefused(), Reentry (+7 more)

### Community 143 - "runFile"
Cohesion: 0.25
Nodes (11): ao_AoTest_assert_equals_(), CallContext, path, Root, string, string_view, uint32_t, listTests() (+3 more)

### Community 144 - "Stack"
Cohesion: 0.10
Nodes (19): FrameBlock, capacity, slots, used, size_t, unique_ptr, attached, Stack (+11 more)

### Community 145 - "string"
Cohesion: 0.20
Nodes (15): CarriedMethod, image, meta, selector, source, carriedMethodName(), compileCarried(), Root (+7 more)

### Community 146 - "FiberStack"
Cohesion: 0.13
Nodes (19): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+11 more)

### Community 147 - "指摘"
Cohesion: 0.07
Nodes (31): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, 指摘, [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む (+23 more)

### Community 148 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.33
Nodes (5): P6b vendor file-in Implementation Plan, Self-review, 仕様, 範囲, 結論

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.08
Nodes (24): Scheduler, uint64_t, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+16 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.04
Nodes (55): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+47 more)

### Community 156 - "DebugSnapshot.cpp"
Cohesion: 0.18
Nodes (21): contextPc(), CallContext, Frame, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method() (+13 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (57): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+49 more)

### Community 158 - "Roots.cpp"
Cohesion: 0.14
Nodes (17): size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt() (+9 more)

### Community 159 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 160 - "FiberRegs"
Cohesion: 0.20
Nodes (10): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+2 more)

### Community 161 - "Fiber.cpp"
Cohesion: 0.13
Nodes (19): asan_interface, common_interface_defs, mman, pthread, array, array, size_t, fiberEntered() (+11 more)

### Community 162 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 163 - "TEST"
Cohesion: 0.10
Nodes (23): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+15 more)

### Community 164 - "installEmptyWorkspace"
Cohesion: 0.13
Nodes (18): Image, check, load, save, boot(), bumpDebugGeneration(), clearMethodSources(), CallContext (+10 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "allocateRetry"
Cohesion: 0.07
Nodes (34): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+26 more)

### Community 167 - "send2"
Cohesion: 0.07
Nodes (28): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals (+20 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - "Scheduler.cpp"
Cohesion: 0.09
Nodes (62): flags, slotAtPut, afterResume, enqueue, find, leaveLists, reapDead, signal (+54 more)

### Community 170 - "RootedArray"
Cohesion: 0.16
Nodes (11): Roots, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+3 more)

### Community 171 - "fiber_test.cpp"
Cohesion: 0.29
Nodes (7): csignal, mach, mach_vm, pingPongFiber(), step(), unistd, utility

### Community 172 - "ImageFormat.cpp"
Cohesion: 0.21
Nodes (10): bit, byte, size_t, string, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader() (+2 more)

### Community 173 - "Global Constraints"
Cohesion: 0.22
Nodes (9): Global Constraints, load, Task 1: 形式ヘッダと即値ビット, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI, Task 6: フェーズを閉じる, B6 イメージとセッションの堅牢化 (+1 more)

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

### Community 178 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 179 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 180 - "abortEvaluation"
Cohesion: 0.09
Nodes (43): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 受け入れ (+35 more)

### Community 181 - "InlineCache"
Cohesion: 0.28
Nodes (8): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, ClassMethodCache::addRoots(), Roots, IcGuard, ic

### Community 182 - ".isSmallInteger"
Cohesion: 0.34
Nodes (14): CompiledMethod accessors (P5), int64_t, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive() (+6 more)

### Community 183 - ".isHeap"
Cohesion: 0.27
Nodes (9): clearWeakAfterOldMark, anyMethodIn(), Visit, usedRemovedVariable(), Gc::collectOld(), Gc::copy(), Heap::checkNotPoisoned(), Heap::inNursery() (+1 more)

### Community 184 - "手順（PR の順序と依存）"
Cohesion: 0.13
Nodes (20): Files, P10-04: セッションのソース表の拡張と doIt, コミット文, テスト, 完了条件, 手順, 目的, Files (+12 more)

### Community 185 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

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
Cohesion: 0.20
Nodes (15): attachBlocks(), blankOut(), AoInspectFn, MethodImage, optional, Roots, dropDoItDebug(), evalBody() (+7 more)

### Community 190 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "TEST"
Cohesion: 0.15
Nodes (11): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, testing::Test, KernelNumeric (+3 more)

### Community 193 - "Range"
Cohesion: 0.50
Nodes (4): Range, first, n, Roots::popRange()

### Community 194 - "DebugFrames"
Cohesion: 0.20
Nodes (9): DebugFrames, count, kind, method, pc, receiver, selector, temp (+1 more)

### Community 195 - "Rec"
Cohesion: 0.33
Nodes (6): Rec, argCount, base, kind, pc, tempCount

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

### Community 203 - "P10-01: SPEC と CLAUDE.md の改訂、PHASE"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-01: SPEC と CLAUDE.md の改訂、PHASE, コミット文, テスト, 完了条件, 手順, 目的

### Community 204 - "P10-07: 受け入れ、CHANGELOG、Graphify deep"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-07: 受け入れ、CHANGELOG、Graphify deep, コミット文, テスト, 完了条件, 手順, 目的

### Community 205 - "SlotNames"
Cohesion: 0.67
Nodes (3): SlotNames, names, WellKnown

### Community 206 - "NamedClass"
Cohesion: 0.67
Nodes (3): NamedClass, name, WellKnown

### Community 207 - "loadedImageProbes"
Cohesion: 0.19
Nodes (14): Interfaces, P10-05: ao_debug_* ABI, コミット文, テスト, 完了条件, 手順, 目的, ABI（P10-05） (+6 more)

### Community 208 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 209 - "lookup_test.cpp"
Cohesion: 0.60
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 210 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 211 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 212 - "README.md"
Cohesion: 0.08
Nodes (23): P4 microbench, P6 interpreter bench, Cuis vendor pin, Ao, Build from source, Documentation, Install, Ao (+15 more)

### Community 215 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 216 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 217 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 220 - "valueHashFold"
Cohesion: 0.33
Nodes (8): bytesValueHash(), HashNesting, CallContext, int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold()

### Community 223 - "P7 .aoimage Implementation Plan"
Cohesion: 0.17
Nodes (11): CLI, globals 節, OOP エンコード, P7 .aoimage Implementation Plan, save, well-known 節, ファイル配置（すべて little-endian）, 仕様 (+3 more)

### Community 225 - "FiberStackBounds"
Cohesion: 0.38
Nodes (7): AO_FIBER_REAL_FRAME, fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitchFinal(), threadStackBounds()

### Community 227 - "Parsed"
Cohesion: 0.20
Nodes (11): decodeNonHeap, acceptWord(), unordered_set, Parsed, globals, heapBytes, offsets, section (+3 more)

### Community 228 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 231 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

## Knowledge Gaps
- **1085 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+1080 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2395 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Analysis`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `VendorExtract.cpp`, `send`, `Heap.cpp`, `TEST`, `Float.cpp`, `String.cpp`, `CompileError`, `Heap`, `vector`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `ClassPool.cpp`, `TEST`, `TEST`, `handlescope`, `string`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `MethodSource`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `putNative`, `Session.cpp`, `CompileEnv`, `WellKnown.cpp`, `DebugSnapshot`, `TEST`, `string`, `.true_`, `TEST`, `TEST`, `abi.cpp`, `TEST`, `.nil`, `Stream.cpp`, `Session`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `performSend`, `takeAbortReason`, `TEST`, `collectNursery`, `Boolean.cpp`, `TEST`, `slotAt`, `Interpreter.cpp`, `Roots`, `CallContext`, `Compile.cpp`, `NativeMethod.cpp`, `path`, `TEST`, `runFile`, `Stack`, `指摘`, `Scheduler::Record`, `Process.cpp`, `TEST`, `DebugSnapshot.cpp`, `TEST`, `Roots.cpp`, `GarbageFirstBoot`, `Fiber.cpp`, `TEST`, `installEmptyWorkspace`, `allocateRetry`, `send2`, `Scheduler.cpp`, `RootedArray`, `ImageFormat.cpp`, `abortingNew`, `intern`, `Table`, `abortEvaluation`, `InlineCache`, `.isSmallInteger`, `.isHeap`, `手順（PR の順序と依存）`, `Dbg`, `sessionEval`, `abortingSubclass`, `imageRegistryStubA`, `Range`, `CountingSink`, `.specialSelector`, `boxLiteral`, `popFrame`, `NativeFrame`, `SlotNames`, `NamedClass`, `lookup_test.cpp`, `README.md`, `Root`, `answerOne`?**
  _High betweenness centrality (0.418) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Analysis`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Compile.cpp`, `checkFile`, `uint64_t`, `Emitter`, `Oop`, `NativeMethod.cpp`, `Codegen.cpp`, `path`, `runFile`, `Stack`, `CompileError`, `ChunkAction`, `string`, `Reentry`, `Scheduler`, `ClassPool.cpp`, `DebugSnapshot.cpp`, `ao_main.cpp`, `handlescope`, `ChunkParser.cpp`, `string`, `ImageSave.cpp`, `fiber_test.cpp`, `Ast`, `MethodSource`, `.isHeap`, `手順（PR の順序と依存）`, `Parser`, `Session.cpp`, `TEST`, `CompileEnv`, `DebugSnapshot`, `TEST`, `string`, `.true_`, `Session`, `Parsed`, `MethodImage`, `ImageLoad.cpp`, `TEST`, `slotAt`, `ImageSurgery`, `Roots`?**
  _High betweenness centrality (0.121) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `Gc`, `checkFile`, `Compile.cpp`, `Oop`, `send`, `TEST`, `NativeMethod.cpp`, `Heap.cpp`, `path`, `Float.cpp`, `String.cpp`, `vector`, `TEST`, `Process.cpp`, `Boot`, `ClassPool.cpp`, `GarbageFirstBoot`, `ao_main.cpp`, `handlescope`, `string`, `Scheduler.cpp`, `ImageSave.cpp`, `ImageFormat.cpp`, `Loaded`, `CallContext`, `intern`, `Table`, `TEST`, `abortEvaluation`, `BlockContext.cpp`, `putNative`, `WellKnown.cpp`, `TEST`, `string`, `lookup_test.cpp`, `TEST`, `.nil`, `Stream.cpp`, `Session`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `TEST`, `collectNursery`, `slotAt`?**
  _High betweenness centrality (0.065) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `Oop` (e.g. with `Graphify / Serena` and `現状`) actually correct?**
  _`Oop` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _1085 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013157894736842105 - nodes in this community are weakly interconnected._
- **Should `Analysis` be split into smaller, more focused modules?**
  _Cohesion score 0.055051421657592255 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 253 files · ~196,280 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 4388 nodes · 11772 edges · 177 communities (155 shown, 22 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1613 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `63d109db`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- RealScope
- .isHeap
- LargeInteger.cpp
- TEST
- Scanner.cpp
- VendorExtract.cpp
- Object.cpp
- Stream.cpp
- putNative
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- send
- VirtualRegion.cpp
- ao_eval
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- interpreter_test.cpp
- uint64_t
- Boot
- WorkspaceWindow
- Session.cpp
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
- P9-01: Do it / Print it / Inspect it
- TEST
- Ast
- Loaded
- CallContext
- TEST
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- SmallInteger.cpp
- Parser
- Send.cpp
- ao_abi.h
- TEST
- BrowserModelTests
- String.cpp
- roots
- vector
- allocateRetry
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- natives
- CompileError
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- Token
- Boolean.cpp
- TEST
- ao image save
- Oop
- BrowserModel
- Analysis
- .false_
- TEST
- TEST
- abi.cpp
- TEST
- Bootstrap.cpp
- 指摘
- build.sh
- test.sh
- SPEC.md
- .publish
- ao_Magnitude_lessOrEqual
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- docs/README.md
- Claude レビュー指摘の修正計画（docs/claude-review）
- 指摘
- ao_image_load
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- ClassDef
- ImageLoad.cpp
- .build
- TEST
- DefinitionScanner
- Claude レビュー指摘の修正計画（docs/claude-review）
- atPut
- native_send_test.cpp
- AppKit
- Character.cpp
- Reentry
- ImageSurgery
- Interpreter.cpp
- 横断テーマ
- ImageFormat
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- TEST
- Array.cpp
- Parsed
- P9-02: Browser accept
- send2
- P9-04: v1 ゴールデン受け入れ
- gc_stress_test.cpp
- 指摘
- package-app.sh
- ObjectHeader
- image_save_load_test.cpp
- Bytecode.hpp
- lookup_test.cpp
- Vendor.hpp
- TEST
- Roots.cpp
- Compiler.cpp
- CompiledMethodNatives.cpp
- intern
- CompileEnv
- TEST
- isClassShaped
- uint64_t
- ClassMethodCache
- BlockProbe
- P4 — Kernel Native Implementation
- interpreter
- block_test.cpp
- TEST
- 指摘
- WellKnown::checkSmallIntegerFastPath
- Gc
- TEST
- README.md
- P4-06 Stream / Transcript model
- Image.hpp
- FileSizeLimit
- imageRegistryStubA
- forceSlide
- cli_test.sh
- create
- IgnoreFileSizeSignal
- TestDir
- answerOne
- Root
- abortingNew
- WellKnown.hpp
- runOnSmallStack

## God Nodes (most connected - your core abstractions)
1. `Oop` - 754 edges
2. `Heap` - 227 edges
3. `WellKnown` - 167 edges
4. `vector` - 151 edges
5. `Ast` - 103 edges
6. `Roots` - 97 edges
7. `TEST()` - 94 edges
8. `TEST()` - 93 edges
9. `TEST()` - 89 edges
10. `BrowserWindow` - 86 edges

## Surprising Connections (you probably didn't know these)
- `[Low] Inspector ウィンドウが閉じても解放されず、増え続ける` --references--> `InspectorWindow`  [INFERRED]
  docs/claude-review/07-app-build.md → app/Ao/InspectorWindow.swift
- `[Low] `AoSpan` の区間を捨てているので、エラー位置が分からない` --references--> `AoSpan`  [INFERRED]
  docs/claude-review/07-app-build.md → bridge/ao_abi.h
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp

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

## Communities (177 total, 22 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "RealScope"
Cohesion: 0.07
Nodes (34): Analyzer, Capture, owner, var, Inline, uint32_t, LexScope, parent (+26 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (62): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+54 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (60): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+52 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (45): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks (+37 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.06
Nodes (64): ArrayAndByteArrayHeaders, AssignVariantsAndComment, chrono, cmath, CommaIsABinaryCharacter, uint32_t, Scanner, i_ (+56 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.10
Nodes (53): extra 節, allowIndex(), containsHostWord(), size_t, string, string_view, doubleBangs(), dropCycles() (+45 more)

### Community 7 - "Object.cpp"
Cohesion: 0.12
Nodes (52): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.12
Nodes (54): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset() (+46 more)

### Community 9 - "putNative"
Cohesion: 0.09
Nodes (33): Bootstrap, Task 2: 再結合に必要なヒープとレジストリの口, リスク, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown (+25 more)

### Community 10 - "Emitter"
Cohesion: 0.11
Nodes (22): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+14 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (43): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+35 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (66): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail (+58 more)

### Community 13 - "Literal"
Cohesion: 0.07
Nodes (34): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+26 more)

### Community 14 - "Float.cpp"
Cohesion: 0.20
Nodes (28): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_printString() (+20 more)

### Community 15 - "send"
Cohesion: 0.13
Nodes (42): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む） (+34 more)

### Community 16 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 17 - "ao_eval"
Cohesion: 0.07
Nodes (41): AoSpan, end, message, start, 01 オブジェクトメモリ・GC・ブートストラップ, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, テストの穴, 概要 (+33 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.06
Nodes (51): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+43 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (48): CallContext, uint32_t, unique_ptr, Root, slot, RootedArray, data_, inline_ (+40 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (26): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+18 more)

### Community 22 - "TEST"
Cohesion: 0.08
Nodes (26): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+18 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "interpreter_test.cpp"
Cohesion: 0.12
Nodes (24): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, assemble() (+16 more)

### Community 25 - "uint64_t"
Cohesion: 0.20
Nodes (22): acceptWord(), uint32_t, uint64_t, unordered_map, unordered_set, findRecord(), headerAt(), heapShaped() (+14 more)

### Community 26 - "Boot"
Cohesion: 0.03
Nodes (91): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+83 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (34): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), installErrorField() (+26 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (88): ao_abi, memory, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount() (+80 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (17): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView (+9 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.08
Nodes (41): climits, cstdio, Global Constraints, P7 .aoimage Implementation Plan, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI, Task 6: フェーズを閉じる (+33 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "handlescope"
Cohesion: 0.12
Nodes (28): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, Chunk, compile, CompiledMethod, compiler, context, cstring (+20 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.13
Nodes (10): cassert, string, Literal::Literal(), string_view, cstddef, cstdint, NativeMethod, CallContext (+2 more)

### Community 38 - "TEST_F"
Cohesion: 0.06
Nodes (37): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, DeadHomeBlockAbortsWithReason, Task 11: Workspace の Do it / Print it / Inspect it (+29 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.11
Nodes (21): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, AcceptTests (+13 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.10
Nodes (24): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+16 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (44): cerrno, csignal, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord() (+36 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.08
Nodes (24): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+16 more)

### Community 47 - "Loaded"
Cohesion: 0.14
Nodes (15): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+7 more)

### Community 48 - "CallContext"
Cohesion: 0.08
Nodes (24): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+16 more)

### Community 49 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 50 - ".nil"
Cohesion: 0.20
Nodes (32): int64_t, ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend() (+24 more)

### Community 52 - "TEST"
Cohesion: 0.08
Nodes (29): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+21 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.20
Nodes (25): safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_() (+17 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (23): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+15 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewAbortsWithShouldNotImplement, MetaclassNewIsForbidden (+5 more)

### Community 56 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (26): Native selector mangling, マングル表, 規則, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+18 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (24): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+16 more)

### Community 58 - "Send.cpp"
Cohesion: 0.10
Nodes (38): InlineCache, cachedClass, cachedMethod, abortDoesNotUnderstand(), abortEvaluation(), abortFailedSend(), abortReasonText(), abortWithSelector() (+30 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.10
Nodes (19): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, filesystem, fstream, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense (+11 more)

### Community 61 - "BrowserModelTests"
Cohesion: 0.22
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 62 - "String.cpp"
Cohesion: 0.18
Nodes (25): Task 1: 形式ヘッダと即値ビット, ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_printString(), ao_String_size(), ao_Symbol_asString() (+17 more)

### Community 64 - "vector"
Cohesion: 0.12
Nodes (58): cctype, vector, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, FileInError, error, file (+50 more)

### Community 65 - "allocateRetry"
Cohesion: 0.05
Nodes (53): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, 指摘 (+45 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".init"
Cohesion: 0.13
Nodes (11): Any, Int, NSRect, NSTableView, NSTextField, NSTextView, NSView, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+3 more)

### Community 70 - "DiskHeader"
Cohesion: 0.08
Nodes (28): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+20 more)

### Community 71 - "natives"
Cohesion: 0.10
Nodes (20): classpool, DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, natives, PartlyDeferredStillFails (+12 more)

### Community 72 - "CompileError"
Cohesion: 0.11
Nodes (24): CompileError, message, span, string, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Global Constraints, P6b vendor file-in Implementation Plan, Self-review (+16 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.08
Nodes (31): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+23 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 79 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 80 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_() (+18 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "Oop"
Cohesion: 0.13
Nodes (16): bits(), int64_t, isBytes(), isIndexable(), isPointers(), make(), uint64_t, Oop (+8 more)

### Community 84 - "BrowserModel"
Cohesion: 0.20
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "Analysis"
Cohesion: 0.08
Nodes (25): CompileResult, error, image, ok, Analysis, declared, error, failed (+17 more)

### Community 86 - ".false_"
Cohesion: 0.20
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "TEST"
Cohesion: 0.05
Nodes (63): AllocateNoGcSpillsToOld, ByteObjectPayload, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses (+55 more)

### Community 89 - "abi.cpp"
Cohesion: 0.09
Nodes (36): algorithm, atomic, Body, CountsAnswerMinusOneOnFailure, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Global Constraints, Task 10: accept (+28 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (71): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+63 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.25
Nodes (16): allocateSkeletons(), allocClass(), string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeSlotNames() (+8 more)

### Community 92 - "指摘"
Cohesion: 0.17
Nodes (12): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る, [Low] `>=` を `(a < b) not` で計算するため、NaN に対して true を返す (+4 more)

### Community 95 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 96 - ".publish"
Cohesion: 0.14
Nodes (8): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value, [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る

### Community 97 - "ao_Magnitude_lessOrEqual"
Cohesion: 0.58
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 103 - "docs/README.md"
Cohesion: 0.20
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.13
Nodes (14): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論, バッチの順序と依存 (+6 more)

### Community 105 - "指摘"
Cohesion: 0.18
Nodes (11): [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い, [Low] Inspector ウィンドウが閉じても解放されず、増え続ける, [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる, [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される, [Low] Workspace の評価を中断できない（無限ループでアプリが固まる）, [Medium] 64 KiB を超える Print it / Inspect it は副作用だけ残して失敗する (+3 more)

### Community 106 - "ao_image_load"
Cohesion: 0.08
Nodes (45): B2 `to:do:` bench, [High] Save / Open Image の失敗がユーザーに見えない, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる (+37 more)

### Community 111 - "AoApp"
Cohesion: 0.10
Nodes (17): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+9 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.20
Nodes (26): ifstream, readHeader, atOffset(), bindAll(), checkFile(), checkGlobals(), byte, size_t (+18 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall, KernelScan (+12 more)

### Community 117 - "DefinitionScanner"
Cohesion: 0.23
Nodes (10): DefinitionScanner, Bool, Int32, String, Token, keyword, other, spanMessage() (+2 more)

### Community 118 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.12
Nodes (15): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論 (+7 more)

### Community 119 - "atPut"
Cohesion: 0.25
Nodes (8): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, 前提, at(), atPut(), uint32_t, WellKnown, create(), growInner()

### Community 120 - "native_send_test.cpp"
Cohesion: 0.14
Nodes (23): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, answerMessage(), CallContext (+15 more)

### Community 121 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 122 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 123 - "Reentry"
Cohesion: 0.22
Nodes (9): string, expectAllRefused(), Reentry, codes, entered, evalOut, image, loadOrder (+1 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (60): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, ファイルごとの変更, ファイルごとの変更, Interpreter, run, ActiveGuard, rootShared, saved (+52 more)

### Community 126 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 127 - "ImageFormat"
Cohesion: 0.09
Nodes (23): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle (+15 more)

### Community 128 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.31
Nodes (15): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+7 more)

### Community 131 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 132 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 133 - "send2"
Cohesion: 0.05
Nodes (45): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, BlockContextForkCreatesAndResumesProcess, ClassDefinitionThroughAliasOnlyRebindsGlobal, ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra (+37 more)

### Community 134 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 135 - "gc_stress_test.cpp"
Cohesion: 0.25
Nodes (7): cstdlib, optional, string, uint64_t, rawWordAt(), ScopedGcStressEnv, saved_

### Community 136 - "指摘"
Cohesion: 0.14
Nodes (13): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+5 more)

### Community 138 - "ObjectHeader"
Cohesion: 0.12
Nodes (16): CLI, globals 節, load, OOP エンコード, save, well-known 節, ファイル配置（すべて little-endian）, 仕様 (+8 more)

### Community 139 - "image_save_load_test.cpp"
Cohesion: 0.18
Nodes (15): resource, path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir() (+7 more)

### Community 140 - "Bytecode.hpp"
Cohesion: 0.24
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), TEST(), NewOpsAppendedAfterPrimitive (+2 more)

### Community 141 - "lookup_test.cpp"
Cohesion: 0.19
Nodes (13): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, CallContext, NativeFn, uint32_t, WellKnown, install() (+5 more)

### Community 142 - "Vendor.hpp"
Cohesion: 0.24
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.14
Nodes (16): iterator, walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock() (+8 more)

### Community 146 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 147 - "intern"
Cohesion: 0.22
Nodes (9): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors() (+1 more)

### Community 148 - "CompileEnv"
Cohesion: 0.10
Nodes (25): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+17 more)

### Community 149 - "TEST"
Cohesion: 0.22
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "isClassShaped"
Cohesion: 0.22
Nodes (8): uint32_t, isClassShaped(), superclassOf(), SuperclassWalk, depth_, size_t, kernelSlotCount(), namesBehavior()

### Community 152 - "ClassMethodCache"
Cohesion: 0.17
Nodes (12): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, probe, Entry (+4 more)

### Community 153 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.04
Nodes (49): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+41 more)

### Community 156 - "block_test.cpp"
Cohesion: 0.14
Nodes (16): [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, WellKnown::global(), abortingSubclass(), countingPrintString(), CallContext, string, uint32_t, evalExpr() (+8 more)

### Community 157 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 158 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 160 - "Gc"
Cohesion: 0.06
Nodes (43): BlockContextKeepsHomeAndCopied, ContextGc, Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順 (+35 more)

### Community 161 - "TEST"
Cohesion: 0.14
Nodes (13): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, 06 イメージ・セッション・C ABI・CLI, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, テストの穴, 概要, 結論 (+5 more)

### Community 162 - "README.md"
Cohesion: 0.06
Nodes (28): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+20 more)

### Community 163 - "P4-06 Stream / Transcript model"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, Transcript

### Community 164 - "Image.hpp"
Cohesion: 0.40
Nodes (5): Image, check, load, save, sessionImageSave()

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 167 - "forceSlide"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, forceNursery(), forceOld(), forceSlide()

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

### Community 170 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 172 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Knowledge Gaps
- **831 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+826 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1835 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **22 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `TEST`, `Object.cpp`, `Stream.cpp`, `putNative`, `Heap.cpp`, `TEST`, `Literal`, `Float.cpp`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `interpreter_test.cpp`, `Boot`, `Session.cpp`, `P3 — Native Dispatch`, `ao_main.cpp`, `TEST`, `handlescope`, `string`, `P2-02 class object skeletons`, `ImageSave.cpp`, `Loaded`, `CallContext`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `Send.cpp`, `String.cpp`, `vector`, `allocateRetry`, `DiskHeader`, `CompileError`, `WellKnown.cpp`, `TEST`, `Boolean.cpp`, `.false_`, `TEST`, `abi.cpp`, `Bootstrap.cpp`, `ao_Magnitude_lessOrEqual`, `native_method_test.cpp`, `ClassDef`, `ImageLoad.cpp`, `TEST`, `atPut`, `native_send_test.cpp`, `Character.cpp`, `ImageSurgery`, `Interpreter.cpp`, `ImageFormat`, `Array.cpp`, `send2`, `gc_stress_test.cpp`, `ObjectHeader`, `image_save_load_test.cpp`, `lookup_test.cpp`, `Roots.cpp`, `CompiledMethodNatives.cpp`, `intern`, `CompileEnv`, `TEST`, `isClassShaped`, `ClassMethodCache`, `BlockProbe`, `block_test.cpp`, `TEST`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `README.md`, `imageRegistryStubA`, `forceSlide`, `create`, `answerOne`, `Root`, `abortingNew`, `WellKnown.hpp`?**
  _High betweenness centrality (0.392) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `RealScope`, `Parsed`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Object.cpp`, `Emitter`, `image_save_load_test.cpp`, `Literal`, `Vendor.hpp`, `ao_eval`, `ChunkAction`, `Heap`, `CompileEnv`, `Roots`, `isClassShaped`, `interpreter_test.cpp`, `uint64_t`, `Boot`, `Session.cpp`, `block_test.cpp`, `ao_main.cpp`, `Gc`, `handlescope`, `ChunkParser.cpp`, `string`, `Image.hpp`, `ImageSave.cpp`, `Ast`, `TEST`, `Parser`, `Send.cpp`, `TEST`, `allocateRetry`, `natives`, `CompileError`, `TEST`, `Oop`, `Analysis`, `.false_`, `TEST`, `TEST`, `abi.cpp`, `Bootstrap.cpp`, `ImageLoad.cpp`, `Reentry`, `ImageSurgery`, `ImageFormat`?**
  _High betweenness centrality (0.127) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Stream.cpp`, `WellKnown.cpp`, `WellKnown.hpp`, `Oop`, `Heap`, `Roots`, `intern`, `uint64_t`, `.false_`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.092) - this node is a cross-community bridge._
- **Are the 13 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 13 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _831 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013333333333333334 - nodes in this community are weakly interconnected._
- **Should `RealScope` be split into smaller, more focused modules?**
  _Cohesion score 0.07439613526570048 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 253 files · ~187,225 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 4254 nodes · 11349 edges · 172 communities (154 shown, 18 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1579 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `9f32eae2`
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
- Object.cpp
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- ao_main.cpp
- send
- VirtualRegion.cpp
- ao_eval
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- docs/README.md
- Boot
- TEST
- TEST
- WorkspaceWindow
- Session.cpp
- TranscriptWindow
- P3 — Native Dispatch
- Bytecode interpreter
- NativeMethod.cpp
- TEST
- TEST
- handlescope
- ChunkParser.cpp
- string
- TEST
- TEST
- BrowserWindow
- P2-02 class object skeletons
- P6b — vendor file-in
- Image::save
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
- .isHeap
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- vector
- allocateRetry
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- TEST
- TEST
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
- TEST
- BrowserModel
- ImageFormat.cpp
- .false_
- TEST
- slotAt
- Image::load
- TEST
- ClassPool.cpp
- GarbageFirstBoot
- build.sh
- test.sh
- SPEC.md
- .publish
- ao_Magnitude_lessOrEqual
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- TEST_F
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- ClassDef
- ImageLoad.cpp
- .build
- collectNursery
- DefinitionScanner
- .isEmpty
- RootedArray
- native_send_test.cpp
- AppKit
- Character.cpp
- Gc
- P3-01 Symbol intern
- Interpreter.cpp
- 横断テーマ
- ImageFormat
- InspectorWindow
- TEST
- Oop
- P4 — Kernel Native Implementation
- P9-02: Browser accept
- send2
- P9-04: v1 ゴールデン受け入れ
- TEST
- 指摘
- package-app.sh
- ObjectHeader
- TEST
- specialIndex
- lookup_test.cpp
- ImageSelector
- TEST
- Roots.cpp
- codegen
- InlineCache
- intern
- CompileEnv
- TEST
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- uint64_t
- ClassMethodCache
- P1-06: weak slot GC
- P4-07 Process / ProcessorScheduler / Semaphore
- interpreter
- abortingSubclass
- NamedOop
- P1-07: immovable old objects
- WellKnown::checkSmallIntegerFastPath
- Gc.cpp
- TEST
- P4-05 Dictionary / Set / OrderedCollection
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- TEST
- expectSpecialSends
- imageRegistryStubA
- answerOne
- cli_test.sh
- TestDir
- abortingNew
- TEST

## God Nodes (most connected - your core abstractions)
1. `Oop` - 753 edges
2. `Heap` - 225 edges
3. `WellKnown` - 167 edges
4. `vector` - 143 edges
5. `Ast` - 103 edges
6. `Roots` - 97 edges
7. `TEST()` - 93 edges
8. `TEST()` - 89 edges
9. `BrowserWindow` - 86 edges
10. `Boot` - 74 edges

## Surprising Connections (you probably didn't know these)
- `[Low] Inspector ウィンドウが閉じても解放されず、増え続ける` --references--> `InspectorWindow`  [INFERRED]
  docs/claude-review/07-app-build.md → app/Ao/InspectorWindow.swift
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
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

## Communities (172 total, 18 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): array, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (58): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+50 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.11
Nodes (62): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+54 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (60): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+52 more)

### Community 4 - "TEST"
Cohesion: 0.07
Nodes (32): FailedProbeKeepsCurrentSession, ImageSave, ImageSaveLoad, KernelMethodsStayNative, LoadSizesOldFromHeader, NurseryGcAfterLoad, OnePlusTwoMatches, RefusesExtraRecords (+24 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary (+50 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.08
Nodes (61): Task 4: LOAD_ORDER とホストパッチ, string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult (+53 more)

### Community 7 - "Object.cpp"
Cohesion: 0.12
Nodes (52): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.06
Nodes (83): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean() (+75 more)

### Community 9 - "Float.cpp"
Cohesion: 0.20
Nodes (28): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_printString() (+20 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (43): charconv, allocateTenured, fitsOld, growOld, header, initObject, objectBytes, oldUsed (+35 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (47): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+39 more)

### Community 13 - "Literal"
Cohesion: 0.09
Nodes (23): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+15 more)

### Community 14 - "ao_main.cpp"
Cohesion: 0.14
Nodes (18): climits, dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests() (+10 more)

### Community 15 - "send"
Cohesion: 0.18
Nodes (35): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill(), ao_Collection_includes_() (+27 more)

### Community 16 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 17 - "ao_eval"
Cohesion: 0.05
Nodes (57): AoSpan, end, message, start, 01 オブジェクトメモリ・GC・ブートストラップ, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, テストの穴, 概要 (+49 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (47): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_ (+39 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (43): Root, slot, FrameBlock, capacity, slots, used, size_t, StackWalker (+35 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (26): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+18 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (28): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+20 more)

### Community 23 - "docs/README.md"
Cohesion: 0.08
Nodes (39): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator (+31 more)

### Community 24 - "Boot"
Cohesion: 0.06
Nodes (34): string, evalExpr(), expectNonBooleanAbort(), expectSymbol(), fileIn(), runActivationProbe(), runSource(), callNative() (+26 more)

### Community 25 - "TEST"
Cohesion: 0.08
Nodes (29): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+21 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (52): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+44 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (30): aoWorkspaceInspectHook(), failureText(), installErrorField(), sendToKeyWorkspace(), spanMessage(), Bool, CChar, Int (+22 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (88): SPEC を先に直す, SPEC を先に直す, Image, load, save, blankOut(), browserClassAt(), browserClassCount() (+80 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (17): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView (+9 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.12
Nodes (18): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling (+10 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "NativeMethod.cpp"
Cohesion: 0.06
Nodes (41): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, CLI, extra 節, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan (+33 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 35 - "handlescope"
Cohesion: 0.14
Nodes (27): ao_abi, Chunk, compile, CompiledMethod, compiler, context, cstdio, cstring (+19 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.10
Nodes (34): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+26 more)

### Community 37 - "string"
Cohesion: 0.08
Nodes (14): cassert, chrono, string, string_view, cstddef, cstdint, limits, memory (+6 more)

### Community 38 - "TEST"
Cohesion: 0.12
Nodes (15): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+7 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.12
Nodes (21): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, AcceptTests (+13 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.10
Nodes (21): Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, Behavior, Bootstrap, Class (+13 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.10
Nodes (24): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+16 more)

### Community 43 - "Image::save"
Cohesion: 0.13
Nodes (27): [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, encodeNonHeap, writeHeader, appendRaw(), appendRecord(), collectImageSlot(), byte, size_t (+19 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.09
Nodes (23): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+15 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): size_t, string_view, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.08
Nodes (24): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+16 more)

### Community 49 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 50 - ".nil"
Cohesion: 0.08
Nodes (62): ClassRedefinitionDropsOldClassEntries, CompiledMethod accessors (P5), FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall (+54 more)

### Community 52 - "TEST"
Cohesion: 0.07
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

### Community 58 - ".isHeap"
Cohesion: 0.11
Nodes (38): スタックガード、abort、死んだホーム, スタックガード、abort、死んだホーム, run, applyMethod(), refreshStackLimit(), abortDoesNotUnderstand(), abortEvaluation(), abortFailedSend() (+30 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 62 - "String.cpp"
Cohesion: 0.18
Nodes (25): Task 1: 形式ヘッダと即値ビット, ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_printString(), ao_String_size(), ao_Symbol_asString() (+17 more)

### Community 64 - "vector"
Cohesion: 0.07
Nodes (91): cctype, CompileError, message, span, string, vector, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す (+83 more)

### Community 65 - "allocateRetry"
Cohesion: 0.05
Nodes (58): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 指摘 (+50 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".init"
Cohesion: 0.12
Nodes (12): Any, Int, NSRect, NSTableView, NSTextField, NSTextView, NSView, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+4 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "TEST"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (30): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+22 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.18
Nodes (11): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, ImageRegistry, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+3 more)

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
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "TEST"
Cohesion: 0.40
Nodes (5): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, TEST(), VersionOneIsRefusedWithReason

### Community 84 - "BrowserModel"
Cohesion: 0.10
Nodes (22): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, BrowserModelTests (+14 more)

### Community 85 - "ImageFormat.cpp"
Cohesion: 0.24
Nodes (10): bit, byte, size_t, string, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader() (+2 more)

### Community 86 - ".false_"
Cohesion: 0.20
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (98): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+90 more)

### Community 88 - "slotAt"
Cohesion: 0.12
Nodes (28): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, allocateNoGc, size, slotAt, slotAtPut, at() (+20 more)

### Community 89 - "Image::load"
Cohesion: 0.31
Nodes (11): bindAll(), checkGlobals(), string, string_view, WellKnown, globalNamesOk(), Image::load(), ImageRecord (+3 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (73): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+65 more)

### Community 91 - "ClassPool.cpp"
Cohesion: 0.23
Nodes (18): classpool, adopt(), bindingAt(), string, string_view, uint32_t, WellKnown, instSizeOf() (+10 more)

### Community 92 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 95 - "SPEC.md"
Cohesion: 0.05
Nodes (54): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+46 more)

### Community 96 - ".publish"
Cohesion: 0.13
Nodes (8): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value, [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る

### Community 97 - "ao_Magnitude_lessOrEqual"
Cohesion: 0.58
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.18
Nodes (19): algorithm, Bootstrap, format, allocateSkeletons(), allocClass(), string_view, WellKnown, ensureMethodDict() (+11 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.12
Nodes (15): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B4 クラスのメタデータ（Kernel の形と名前）, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論 (+7 more)

### Community 105 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 106 - "TEST_F"
Cohesion: 0.03
Nodes (89): AbiSmoke, ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootAndShutdownReturnZero, BootThenImageRoundTripKeepsOnePlusTwo, BootVersionShutdown, ClassDefinedAfterBindingWins, DeadHomeBlockAbortsWithReason (+81 more)

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
Cohesion: 0.22
Nodes (22): acceptWord(), byte, size_t, T, uint32_t, uint64_t, unordered_set, fileOop() (+14 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "collectNursery"
Cohesion: 0.36
Nodes (10): collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld() (+2 more)

### Community 117 - "DefinitionScanner"
Cohesion: 0.25
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 118 - ".isEmpty"
Cohesion: 0.12
Nodes (15): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+7 more)

### Community 119 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 120 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 121 - "AppKit"
Cohesion: 0.21
Nodes (5): Ao, AppKit, CAo, PackageDescription, XCTest

### Community 122 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 123 - "Gc"
Cohesion: 0.18
Nodes (13): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress (+5 more)

### Community 124 - "P3-01 Symbol intern"
Cohesion: 0.22
Nodes (8): Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary, P3-02 MethodDictionary, Symbol

### Community 125 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (58): ファイルごとの変更, ファイルごとの変更, pthread, ActiveGuard, rootShared, saved, answerWithoutSend(), boolean() (+50 more)

### Community 126 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 127 - "ImageFormat"
Cohesion: 0.11
Nodes (20): uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes, kImagePointerBits (+12 more)

### Community 128 - "InspectorWindow"
Cohesion: 0.36
Nodes (5): InspectorWindow, .text, NSTextView, NSWindow, String

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Oop"
Cohesion: 0.12
Nodes (29): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+21 more)

### Community 131 - "P4 — Kernel Native Implementation"
Cohesion: 0.05
Nodes (43): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+35 more)

### Community 132 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 133 - "send2"
Cohesion: 0.11
Nodes (19): ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RebindsBagAndEvaluatesInstVar, RectangleWidthHeightContainsAndIntersect (+11 more)

### Community 134 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 135 - "TEST"
Cohesion: 0.10
Nodes (24): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+16 more)

### Community 136 - "指摘"
Cohesion: 0.13
Nodes (14): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+6 more)

### Community 138 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 139 - "TEST"
Cohesion: 0.10
Nodes (21): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, assemble() (+13 more)

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "lookup_test.cpp"
Cohesion: 0.60
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 142 - "ImageSelector"
Cohesion: 0.67
Nodes (3): ImageSelector, name, WellKnown

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 146 - "InlineCache"
Cohesion: 0.33
Nodes (6): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, IcGuard, ic

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "CompileEnv"
Cohesion: 0.05
Nodes (41): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, isBinaryChar() (+33 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.33
Nodes (6): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 152 - "ClassMethodCache"
Cohesion: 0.17
Nodes (12): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, probe, Entry (+4 more)

### Community 153 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 154 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.29
Nodes (7): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, ProcessorScheduler, Semaphore, send

### Community 156 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 157 - "NamedOop"
Cohesion: 0.29
Nodes (7): string, NameCollect, failed, out, NamedOop, name, value

### Community 158 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 160 - "Gc.cpp"
Cohesion: 0.15
Nodes (13): cstdlib, optional, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::Gc() (+5 more)

### Community 161 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 162 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.40
Nodes (5): Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 163 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.33
Nodes (6): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 164 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 165 - "expectSpecialSends"
Cohesion: 1.00
Nodes (3): CallContext, expectSpecialSends(), runSource()

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 167 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "TEST"
Cohesion: 0.20
Nodes (10): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, NinthMethodWithFullNurseryIsInstalled, MethodImage, defineEightMethodClass() (+2 more)

## Knowledge Gaps
- **809 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+804 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1770 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `ao_eval`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `TEST`, `Session.cpp`, `NativeMethod.cpp`, `TEST`, `handlescope`, `string`, `Image::save`, `Loaded`, `CallContext`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `.isHeap`, `String.cpp`, `vector`, `allocateRetry`, `TEST`, `WellKnown.cpp`, `TEST`, `Boolean.cpp`, `ImageFormat.cpp`, `.false_`, `TEST`, `slotAt`, `ClassPool.cpp`, `GarbageFirstBoot`, `SPEC.md`, `ao_Magnitude_lessOrEqual`, `Bootstrap.cpp`, `native_method_test.cpp`, `ClassDef`, `ImageLoad.cpp`, `collectNursery`, `.isEmpty`, `RootedArray`, `native_send_test.cpp`, `Character.cpp`, `P3-01 Symbol intern`, `Interpreter.cpp`, `send2`, `TEST`, `ObjectHeader`, `TEST`, `lookup_test.cpp`, `ImageSelector`, `Roots.cpp`, `InlineCache`, `intern`, `CompileEnv`, `ClassMethodCache`, `abortingSubclass`, `NamedOop`, `WellKnown::checkSmallIntegerFastPath`, `Gc.cpp`, `expectSpecialSends`, `imageRegistryStubA`, `answerOne`, `abortingNew`, `TEST`?**
  _High betweenness centrality (0.387) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `LargeInteger.cpp`, `TEST`, `Scanner.cpp`, `VendorExtract.cpp`, `Object.cpp`, `Emitter`, `Literal`, `ao_main.cpp`, `ChunkAction`, `CompileEnv`, `Roots`, `TEST`, `Session.cpp`, `NamedOop`, `Gc.cpp`, `NativeMethod.cpp`, `handlescope`, `ChunkParser.cpp`, `string`, `Image::save`, `Ast`, `Loaded`, `Parser`, `.isHeap`, `allocateRetry`, `TEST`, `.false_`, `TEST`, `Image::load`, `ClassPool.cpp`, `Bootstrap.cpp`, `ImageLoad.cpp`?**
  _High betweenness centrality (0.130) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `string`, `Stream.cpp`, `WellKnown.cpp`, `Heap`, `Roots`, `intern`, `.false_`, `uint64_t`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.093) - this node is a cross-community bridge._
- **Are the 12 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 12 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _809 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013245033112582781 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.04890453834115806 - nodes in this community are weakly interconnected._
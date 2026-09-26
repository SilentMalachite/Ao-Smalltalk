# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 284 files · ~286,092 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5725 nodes · 15359 edges · 234 communities (216 shown, 18 thin omitted)
- Extraction: 85% EXTRACTED · 15% INFERRED · 0% AMBIGUOUS · INFERRED: 2363 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `7aad6c67`
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
- RealScope
- Float.cpp
- String.cpp
- DebuggerWindow
- 指摘
- ChunkAction
- Heap
- TEST_F
- Geometry.cpp
- TEST
- docs/README.md
- TEST
- Scheduler
- TEST
- WorkspaceWindow
- ClassPool.cpp
- ToolTextSize
- P3 — Native Dispatch
- Bytecode interpreter
- putNative
- TEST
- TEST
- string
- ChunkParser.cpp
- cstdint
- TEST_F
- TEST
- AcceptTests
- P2-02 class object skeletons
- P6b — vendor file-in
- ImageSave.cpp
- P9 — Integration
- TEST
- Ast
- Loaded
- CallContext
- TEST
- PcSpan
- heap
- TEST
- BlockContext.cpp
- TEST_F
- takeAbortReason
- InspectorWindow
- Parser
- TEST_F
- ao_image_load
- TEST
- TEST
- SmallInteger.cpp
- roots
- DebuggerWindowTests
- Session.cpp
- ToolWindowTests
- TEST
- wellknown
- BrowserWindow
- DiskHeader
- TEST
- 指摘
- WellKnown.cpp
- Ao.app skeleton
- .isEmpty
- DebugSnapshot
- TEST
- TEST
- Token
- string
- TEST
- ao image save
- TranscriptWindow
- BrowserModel
- TEST_F
- Behavior.cpp
- TEST
- KeptClass
- abi.cpp
- TEST
- ClassDef
- Stream.cpp
- build.sh
- test.sh
- SPEC.md
- Session
- Files
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- MethodImage
- .build
- .isSmallInteger
- P11 — ライブデバッガ
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- Qiita-part2.md
- checkFile
- OperandStack
- TEST
- AppKit
- ImageFormat
- TEST
- Gc
- Boolean.cpp
- TEST
- .isHeap
- ImageSurgery
- TEST
- Interpreter::run
- Roots
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- Literal
- DefinitionScanner
- Compile.cpp
- ao_main.cpp
- uint64_t
- package-app.sh
- NativeMethod.cpp
- path
- 仕様
- TEST
- Reentry
- vector
- Stack
- extractVendor
- FiberStack
- allocateRetry
- P6b vendor file-in Implementation Plan
- TEST
- Scheduler::Record
- uint64_t
- Process.cpp
- ImageLoad.cpp
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
- Ao
- FileSizeLimit
- codegen_snapshot_test.cpp
- Boot
- cli_test.sh
- .nil
- 指摘
- BrowserModelTests
- TEST
- P7 .aoimage Implementation Plan
- abortingNew
- README.md
- intern
- .init
- performSend
- Frame
- abortEvaluation
- native_send_test.cpp
- CompiledMethodNatives.cpp
- P10-07: 受け入れ、CHANGELOG、Graphify deep
- MethodSource
- ImageFormat.cpp
- Counts
- Dbg
- TestDir
- clearUnwinding
- ClassMethodCache
- imageRegistryStubA
- TEST
- FrameBlock
- DebugFrames
- Codegen.cpp
- CountingSink
- Roots::visitAll
- .specialSelector
- 指摘
- Roots.hpp
- NativeFrame
- Format.hpp
- P10-01: SPEC と CLAUDE.md の改訂、PHASE
- TEST
- PingPong
- Parsed
- Gc.cpp
- ParseResult
- CompileEnv
- TEST
- Qiita-part1.md
- Ao
- create
- DepthGuard
- P9-04: v1 ゴールデン受け入れ
- TEST
- abortingSubclass
- collectNursery
- answerOne
- Character.cpp
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- P1 — Object Memory
- 仕様
- FiberStackBounds
- WellKnown::InternTable
- acceptClass
- B10 協調スケジューラ（02 High）
- IcGuard
- ScopedGcStressEnv
- TEST
- 06 イメージ・セッション・C ABI・CLI
- Root
- SlotNames

## God Nodes (most connected - your core abstractions)
1. `Oop` - 918 edges
2. `Heap` - 252 edges
3. `vector` - 172 edges
4. `WellKnown` - 167 edges
5. `TEST()` - 107 edges
6. `Ast` - 104 edges
7. `Boot` - 101 edges
8. `TEST()` - 100 edges
9. `Session` - 97 edges
10. `TEST()` - 96 edges

## Surprising Connections (you probably didn't know these)
- `TDD` --references--> `DebuggerWindowTests`  [INFERRED]
  docs/phases/P11.md → app/AoTests/DebuggerWindowTests.swift
- `[High] クラス変数を読み書きできない` --references--> `CompileEnv`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/include/ao/MethodImage.hpp
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

## Communities (234 total, 18 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Analysis"
Cohesion: 0.07
Nodes (24): Codegen, CompileResult, error, image, ok, Analysis, declared, error (+16 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (82): ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, int64_t, visit, allocateInstance() (+74 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (47): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedLoadKeepsDebugGeneration, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage (+39 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.17
Nodes (29): allowIndex(), containsHostWord(), string, string_view, doubleBangs(), firstLineKey(), firstNonEmptyLine(), HostMethod (+21 more)

### Community 7 - "Oop"
Cohesion: 0.10
Nodes (62): [High] printOn: が新しい printString を使わない, Graphify / Serena, uint64_t, Oop, kCharTag, kImmTag, kLow3, kSmiTag (+54 more)

### Community 8 - "send"
Cohesion: 0.09
Nodes (62): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, boolean(), int64_t, smallIntegerAnswer(), ao_Array_equals() (+54 more)

### Community 9 - "ao_eval"
Cohesion: 0.05
Nodes (56): AoSpan, end, message, start, 01 オブジェクトメモリ・GC・ブートストラップ, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, テストの穴, 概要 (+48 more)

### Community 10 - "Emitter"
Cohesion: 0.16
Nodes (15): int16_t, Op, size_t, uint16_t, uint8_t, Emitter, real_, hasReceiverChild() (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.07
Nodes (50): charconv, allocateTenured, checkNotPoisoned, growOld, initObject, objectBytes, oldUsed, uint16_t (+42 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (49): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+41 more)

### Community 13 - "RealScope"
Cohesion: 0.08
Nodes (27): Capture, owner, var, uint32_t, RealScope, copied, copiedBase, isBlock (+19 more)

### Community 14 - "Float.cpp"
Cohesion: 0.14
Nodes (46): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals() (+38 more)

### Community 15 - "String.cpp"
Cohesion: 0.11
Nodes (42): bytesValueHash(), HashNesting, CallContext, int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold() (+34 more)

### Community 16 - "DebuggerWindow"
Cohesion: 0.07
Nodes (38): aoDebuggerInspectHook(), DebugFrame, DebuggerWindow, .frameLabels, .inspectorCount, .inspectorText, .open, .selectedFrame (+30 more)

### Community 17 - "指摘"
Cohesion: 0.08
Nodes (22): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+14 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.03
Nodes (81): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames (+73 more)

### Community 20 - "TEST_F"
Cohesion: 0.06
Nodes (35): BlockFrameLabelIsBracketsIn, BuffersFollowRangeRule, CaptureOffLeavesNoFrames, CaptureSettingSurvivesBootAndLoad, CaptureTurnedOnDuringTempPrintWaitsForTheEnd, ClearDropsRoots, DoItFrameSourceDropsPrefix, EvalErrorFillsFramesInnermostFirst (+27 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.08
Nodes (26): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+18 more)

### Community 23 - "docs/README.md"
Cohesion: 0.14
Nodes (20): CLAUDE.md process authority, Graphify knowledge graph tool, Phase P0 skeleton, Phase pipeline P0-P9, Serena LSP symbol tool, SPEC.md product authority, P0 phase doc, P0-01: git / LICENSE / PHASE / README (+12 more)

### Community 24 - "TEST"
Cohesion: 0.08
Nodes (26): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+18 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (36): SPEC §3.4 に書く意味論, CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+28 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (58): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+50 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.11
Nodes (19): sendToKeyWorkspace(), Bool, Int, NSRange, NSWindow, WorkspaceWindow, .errorAccessibilityLabel, .errorText (+11 more)

### Community 28 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (22): hashedcollection, adopt(), bindingAt(), int64_t, string, string_view, uint32_t, WellKnown (+14 more)

### Community 29 - "ToolTextSize"
Cohesion: 0.17
Nodes (9): Int, NSFont, NSRange, ToolTextSize, .offset, UniformFont, NSTextStorage, NSTextStorageDelegate (+1 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+26 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "string"
Cohesion: 0.09
Nodes (46): algorithm, ao_abi, Bootstrap, chrono, Chunk, classpool, cmath, compile (+38 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.18
Nodes (23): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+15 more)

### Community 37 - "cstdint"
Cohesion: 0.06
Nodes (21): string_view, cstddef, cstdint, cstdlib, memory, NativeMethod, CallContext, CallContext (+13 more)

### Community 38 - "TEST_F"
Cohesion: 0.03
Nodes (60): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+52 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "AcceptTests"
Cohesion: 0.13
Nodes (11): AcceptTests, Bool, NSMenu, NSSegmentedControl, NSTableView, NSTextView, NSView, String (+3 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.14
Nodes (18): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+10 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (48): cerrno, climits, csignal, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, fcntl, appendRaw(), appendRecord(), collectImageSlot() (+40 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.06
Nodes (30): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+22 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.11
Nodes (25): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+17 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (37): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+29 more)

### Community 49 - "TEST"
Cohesion: 0.05
Nodes (39): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+31 more)

### Community 50 - "PcSpan"
Cohesion: 0.18
Nodes (12): uint32_t, PcSpan, end, pc, start, debugInfoFor(), DebugInfoRef, body (+4 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.24
Nodes (22): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+14 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (32): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+24 more)

### Community 55 - "takeAbortReason"
Cohesion: 0.10
Nodes (21): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+13 more)

### Community 56 - "InspectorWindow"
Cohesion: 0.09
Nodes (34): InspectorWindow, .text, MainActor, NSObjectProtocol, NSTextView, NSWindow, Sendable, String (+26 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (24): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+16 more)

### Community 58 - "TEST_F"
Cohesion: 0.05
Nodes (39): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+31 more)

### Community 59 - "ao_image_load"
Cohesion: 0.06
Nodes (53): B2 `to:do:` bench, P4 microbench, P6 interpreter bench, [High] Save / Open Image の失敗がユーザーに見えない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない (+45 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "TEST"
Cohesion: 0.12
Nodes (20): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, firstBlock(), named(), pcOf() (+12 more)

### Community 62 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 64 - "DebuggerWindowTests"
Cohesion: 0.14
Nodes (11): DebuggerWindowTests, Int, NSFont, NSTableView, NSTextView, NSView, NSWindow, String (+3 more)

### Community 65 - "Session.cpp"
Cohesion: 0.19
Nodes (30): Interfaces, 手順, browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt() (+22 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.12
Nodes (12): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+4 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.07
Nodes (27): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+19 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "指摘"
Cohesion: 0.15
Nodes (13): [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる, [High] 2^63 以上の整数リテラルが黙って 0 になる, [High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない, [High] クラス変数を読み書きできない, [High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む, [Low] カスケードの扱いが不完全, [Low] 宣言の検証がない（引数への代入・擬変数名・重複名）, [Low] 指数表記の意味が Smalltalk-80 と異なる (+5 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (28): findSymbol, global, internWith, Roots, string_view, ImageSelector, name, WellKnown (+20 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - ".isEmpty"
Cohesion: 0.18
Nodes (26): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, ao_WriteStream_nextPut_(), classFormat(), collectionSize(), copyPrefix(), int64_t, Root, string_view (+18 more)

### Community 76 - "DebugSnapshot"
Cohesion: 0.05
Nodes (41): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+33 more)

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.06
Nodes (32): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, CascadePartsAreMessageChains, Inline, IntegerBeyondInt64KeepsItsDigits (+24 more)

### Community 79 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 80 - "string"
Cohesion: 0.15
Nodes (33): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+25 more)

### Community 81 - "TEST"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "TranscriptWindow"
Cohesion: 0.11
Nodes (19): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int32, NSRect (+11 more)

### Community 84 - "BrowserModel"
Cohesion: 0.18
Nodes (11): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, UnsafeMutablePointer (+3 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 86 - "Behavior.cpp"
Cohesion: 0.22
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (78): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+70 more)

### Community 88 - "KeptClass"
Cohesion: 0.14
Nodes (22): size_t, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars, deferred (+14 more)

### Community 89 - "abi.cpp"
Cohesion: 0.09
Nodes (44): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ABI（P10-05）, ObjectIsKernelAndPrintStringIsNative, AbiEntry (+36 more)

### Community 90 - "TEST"
Cohesion: 0.02
Nodes (83): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+75 more)

### Community 91 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 92 - "Stream.cpp"
Cohesion: 0.15
Nodes (41): Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents() (+33 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.09
Nodes (31): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+23 more)

### Community 96 - "Session"
Cohesion: 0.09
Nodes (29): argCountOf(), debugTempCount(), debugTempName(), frameDebugInfo(), frameInRange(), frameTempCount(), frameValue(), CallContext (+21 more)

### Community 97 - "Files"
Cohesion: 0.15
Nodes (19): Files, P10-05: ao_debug_* ABI, コミット文, テスト, 完了条件, 目的, blankOut(), AoInspectFn (+11 more)

### Community 103 - "MethodImage"
Cohesion: 0.12
Nodes (19): int16_t, string, uint16_t, uint8_t, MethodImage, bytes, literals, numArgs (+11 more)

### Community 104 - ".build"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, Selector, String, Void

### Community 105 - ".isSmallInteger"
Cohesion: 0.11
Nodes (35): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+27 more)

### Community 106 - "P11 — ライブデバッガ"
Cohesion: 0.20
Nodes (9): P11 — ライブデバッガ, PR 一覧, TDD, 仕様（設計判断）, 制約, 前提, 受け入れ, 範囲 (+1 more)

### Community 111 - "AoApp"
Cohesion: 0.12
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 114 - "checkFile"
Cohesion: 0.29
Nodes (17): readHeader, atOffset(), checkFile(), Roots, size_t, string, string_view, WellKnown (+9 more)

### Community 115 - "OperandStack"
Cohesion: 0.12
Nodes (14): deque, Roots, uint32_t, unique_ptr, OperandStack, roots, slots, Temps (+6 more)

### Community 116 - "TEST"
Cohesion: 0.13
Nodes (18): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, describe(), evalBody() (+10 more)

### Community 117 - "AppKit"
Cohesion: 0.22
Nodes (6): Ao, SmokeTests, AppKit, PackageDescription, XCTest, XCTestCase

### Community 118 - "ImageFormat"
Cohesion: 0.08
Nodes (25): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, encodeNonHeap, kImageEndianLittle (+17 more)

### Community 119 - "TEST"
Cohesion: 0.05
Nodes (51): AllocateNoGcSpillsToOld, ByteObjectPayload, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty (+43 more)

### Community 120 - "Gc"
Cohesion: 0.20
Nodes (12): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress (+4 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.05
Nodes (39): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+31 more)

### Community 123 - ".isHeap"
Cohesion: 0.14
Nodes (23): allocateNoGc, Gc::collectOld(), Gc::copy(), at(), bind(), bindIn(), Roots, string_view (+15 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter::run"
Cohesion: 0.13
Nodes (26): ActiveGuard, rootShared, saved, byteCount(), contextAlive(), ContextExitGuard, CallContext, Frame (+18 more)

### Community 127 - "Roots"
Cohesion: 0.07
Nodes (28): StackWalker, uint8_t, Roots, add, attachStack, counts, detachStack, dropHandle (+20 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (23): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+15 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.20
Nodes (22): ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+14 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 132 - "Literal"
Cohesion: 0.11
Nodes (19): int64_t, LitKind, unique_ptr, Literal, bytes, elements, floatValue, intValue (+11 more)

### Community 133 - "DefinitionScanner"
Cohesion: 0.25
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 134 - "Compile.cpp"
Cohesion: 0.07
Nodes (86): cctype, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Interfaces, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価 (+78 more)

### Community 135 - "ao_main.cpp"
Cohesion: 0.16
Nodes (18): Task 5: CLI, dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests() (+10 more)

### Community 136 - "uint64_t"
Cohesion: 0.25
Nodes (14): uint32_t, uint64_t, unordered_map, headerAt(), heapShaped(), ObjectRules, behavior_, dict_ (+6 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.24
Nodes (18): load, B6 イメージとセッションの堅牢化, add(), addNamed(), apply(), CallContext, NativeFn, string_view (+10 more)

### Community 139 - "path"
Cohesion: 0.18
Nodes (10): path, string, uint32_t, expectRefused(), findBytesOfSize(), freshDir(), readHeapBytes(), saveFreshImage() (+2 more)

### Community 140 - "仕様"
Cohesion: 0.25
Nodes (7): 範囲, `Object>>halt`（P10-03）, P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, SPEC を先に直す（P10-01）, 仕様, 範囲, ao_set_debug_capture()

### Community 141 - "TEST"
Cohesion: 0.11
Nodes (18): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+10 more)

### Community 142 - "Reentry"
Cohesion: 0.15
Nodes (13): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), Reentry, codes (+5 more)

### Community 143 - "vector"
Cohesion: 0.12
Nodes (23): vector, DebugInfo, bodies, MethodDebugInfo, pcMap, temps, listTests(), acceptMethods() (+15 more)

### Community 144 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 145 - "extractVendor"
Cohesion: 0.17
Nodes (15): Global Constraints, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 4: LOAD_ORDER とホストパッチ, Task 6: フェーズ完了, string, VendorClassFile, chunkText (+7 more)

### Community 146 - "FiberStack"
Cohesion: 0.15
Nodes (17): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+9 more)

### Community 147 - "allocateRetry"
Cohesion: 0.05
Nodes (50): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 指摘 (+42 more)

### Community 148 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.33
Nodes (5): P6b vendor file-in Implementation Plan, Self-review, 仕様, 範囲, 結論

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.08
Nodes (24): Scheduler, uint64_t, unique_ptr, Scheduler::findId(), Scheduler::Record, abandon, awaitingTerminate, ctx (+16 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "ImageLoad.cpp"
Cohesion: 0.21
Nodes (17): ifstream, bindAll(), checkGlobals(), byte, T, fileOop(), globalNamesOk(), ImageRecord (+9 more)

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.04
Nodes (55): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+47 more)

### Community 156 - "DebugSnapshot.cpp"
Cohesion: 0.18
Nodes (21): contextPc(), CallContext, Frame, uint32_t, DebugSnapshot::capture(), DebugSnapshot::context(), DebugSnapshot::kind(), DebugSnapshot::method() (+13 more)

### Community 157 - "TEST"
Cohesion: 0.05
Nodes (57): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+49 more)

### Community 158 - "Roots.cpp"
Cohesion: 0.13
Nodes (19): new, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle() (+11 more)

### Community 159 - "GarbageFirstBoot"
Cohesion: 0.15
Nodes (16): CallContext, Roots, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot (+8 more)

### Community 160 - "FiberRegs"
Cohesion: 0.14
Nodes (14): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+6 more)

### Community 161 - "Fiber.cpp"
Cohesion: 0.13
Nodes (18): asan_interface, common_interface_defs, mman, array, array, size_t, fiberEntered(), fiberSanitizerFinishSwitch() (+10 more)

### Community 162 - "fiber_test.cpp"
Cohesion: 0.15
Nodes (18): mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds (+10 more)

### Community 163 - "TEST"
Cohesion: 0.09
Nodes (22): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CaptureAfterDeepRecursionAddsNoLifoSlots, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, テスト, DoesNotUnderstandSynthesizesFrameWithoutMethod (+14 more)

### Community 164 - "Ao"
Cohesion: 0.20
Nodes (10): Ao, Ao.app の使い方, `ao` CLI の使い方, v1 に無いもの, インストール, ソースからのビルド, ライセンス, 文書 (+2 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "codegen_snapshot_test.cpp"
Cohesion: 0.21
Nodes (16): classVarLiterals(), countOp(), MethodImage, Op, string, uint8_t, decode(), firstBlock() (+8 more)

### Community 167 - "Boot"
Cohesion: 0.05
Nodes (56): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span (+48 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".nil"
Cohesion: 0.08
Nodes (71): flags, size, slotAt, slotAtPut, afterResume, enqueue, find, leaveLists (+63 more)

### Community 170 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 171 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 172 - "TEST"
Cohesion: 0.25
Nodes (8): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionStringIsReleaseOneZeroZero, VersionTruncationIsRangeError

### Community 173 - "P7 .aoimage Implementation Plan"
Cohesion: 0.17
Nodes (12): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 4: load と受け入れ評価, Task 6: フェーズを閉じる, リスク (+4 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "README.md"
Cohesion: 0.07
Nodes (24): [1.0.0] - 2026-09-26, Ao.app, `ao` CLI, Changelog, Class library and images, Compiler and interpreter, Known limitations, Project (+16 more)

### Community 176 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 177 - ".init"
Cohesion: 0.18
Nodes (9): AnyObject, installDebugButton(), MainActor, NSTextField, Selector, Void, WorkspaceButtonAction, NSButton (+1 more)

### Community 178 - "performSend"
Cohesion: 0.33
Nodes (9): branchTruth(), clearNonlocal(), consumeNonlocal(), hit(), Leave, leave, value, miss() (+1 more)

### Community 179 - "Frame"
Cohesion: 0.15
Nodes (13): Frame, context, isBlock, method, pc, prev, receiver, sendArgc (+5 more)

### Community 180 - "abortEvaluation"
Cohesion: 0.09
Nodes (36): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 受け入れ (+28 more)

### Community 181 - "native_send_test.cpp"
Cohesion: 0.54
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 182 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 183 - "P10-07: 受け入れ、CHANGELOG、Graphify deep"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-07: 受け入れ、CHANGELOG、Graphify deep, コミット文, テスト, 完了条件, 手順, 目的

### Community 184 - "MethodSource"
Cohesion: 0.08
Nodes (40): Files, P10-04: セッションのソース表の拡張と doIt, コミット文, テスト, 完了条件, 手順, 目的, P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む） (+32 more)

### Community 185 - "ImageFormat.cpp"
Cohesion: 0.21
Nodes (10): bit, byte, size_t, string, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader() (+2 more)

### Community 186 - "Counts"
Cohesion: 0.25
Nodes (8): Counts, attachedStacks, frameSlots, handles, pinnedSlots, ranges, slots, Roots::counts()

### Community 187 - "Dbg"
Cohesion: 0.29
Nodes (3): Dbg, b, sink

### Community 188 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 189 - "clearUnwinding"
Cohesion: 0.10
Nodes (29): Image, check, load, save, refreshStackLimit(), boot(), clearUnwinding(), bumpDebugGeneration() (+21 more)

### Community 190 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 193 - "FrameBlock"
Cohesion: 0.17
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 194 - "DebugFrames"
Cohesion: 0.20
Nodes (9): DebugFrames, count, kind, method, pc, receiver, selector, temp (+1 more)

### Community 195 - "Codegen.cpp"
Cohesion: 0.11
Nodes (24): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Inline, LitKind (+16 more)

### Community 196 - "CountingSink"
Cohesion: 0.14
Nodes (13): CountingSink, pinnedAfter, slotsAfter, slotsBefore, snap, CallContext, DebugSink, Roots (+5 more)

### Community 197 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 199 - "指摘"
Cohesion: 0.17
Nodes (12): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+4 more)

### Community 200 - "Roots.hpp"
Cohesion: 0.29
Nodes (6): cassert, uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 201 - "NativeFrame"
Cohesion: 0.50
Nodes (4): Roots, NativeFrame, argc, slots

### Community 202 - "Format.hpp"
Cohesion: 0.46
Nodes (7): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make()

### Community 203 - "P10-01: SPEC と CLAUDE.md の改訂、PHASE"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-01: SPEC と CLAUDE.md の改訂、PHASE, コミット文, テスト, 完了条件, 手順, 目的

### Community 204 - "TEST"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 205 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 206 - "Parsed"
Cohesion: 0.20
Nodes (11): decodeNonHeap, acceptWord(), unordered_set, Parsed, globals, heapBytes, offsets, section (+3 more)

### Community 207 - "Gc.cpp"
Cohesion: 0.25
Nodes (9): Roots, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectBeforeTenured(), Gc::Gc() (+1 more)

### Community 208 - "ParseResult"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 209 - "CompileEnv"
Cohesion: 0.22
Nodes (9): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+1 more)

### Community 210 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 211 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 212 - "Ao"
Cohesion: 0.20
Nodes (10): Ao, Build from source, Documentation, Install, Layout, License, Not in v1, Status (+2 more)

### Community 213 - "create"
Cohesion: 0.25
Nodes (8): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod()

### Community 215 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 216 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 217 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 218 - "collectNursery"
Cohesion: 0.39
Nodes (9): collectNursery, collectOld, Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld(), forceSlide() (+1 more)

### Community 219 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 220 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 221 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 222 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 223 - "仕様"
Cohesion: 0.25
Nodes (8): CLI, extra 節, globals 節, OOP エンコード, save, well-known 節, ファイル配置（すべて little-endian）, 仕様

### Community 224 - "FiberStackBounds"
Cohesion: 0.38
Nodes (7): AO_FIBER_REAL_FRAME, fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitchFinal(), threadStackBounds()

### Community 225 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 226 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 227 - "B10 協調スケジューラ（02 High）"
Cohesion: 0.33
Nodes (6): B10 協調スケジューラ（02 High）, ファイバ, ファイルごとの変更, プロセスごとの状態, リスク, 手順（各段を緑にしてから次へ進む）

### Community 228 - "IcGuard"
Cohesion: 0.50
Nodes (4): ClassMethodCache::addRoots(), Roots, IcGuard, ic

### Community 229 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 230 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 231 - "06 イメージ・セッション・C ABI・CLI"
Cohesion: 0.50
Nodes (3): 06 イメージ・セッション・C ABI・CLI, テストの穴, 概要

### Community 232 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 233 - "SlotNames"
Cohesion: 0.67
Nodes (3): SlotNames, names, WellKnown

## Knowledge Gaps
- **1102 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+1097 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2453 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Analysis`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `send`, `Heap.cpp`, `TEST`, `Float.cpp`, `String.cpp`, `Heap`, `Geometry.cpp`, `TEST`, `TEST`, `TEST`, `ClassPool.cpp`, `putNative`, `TEST`, `TEST`, `string`, `cstdint`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `PcSpan`, `TEST`, `BlockContext.cpp`, `TEST_F`, `takeAbortReason`, `SmallInteger.cpp`, `Session.cpp`, `WellKnown.cpp`, `.isEmpty`, `DebugSnapshot`, `TEST`, `string`, `Behavior.cpp`, `TEST`, `abi.cpp`, `TEST`, `ClassDef`, `Stream.cpp`, `Session`, `Files`, `.isSmallInteger`, `native_method_test.cpp`, `checkFile`, `OperandStack`, `TEST`, `TEST`, `Boolean.cpp`, `TEST`, `.isHeap`, `ImageSurgery`, `Interpreter::run`, `Roots`, `Array.cpp`, `Compile.cpp`, `NativeMethod.cpp`, `path`, `TEST`, `vector`, `Stack`, `allocateRetry`, `Scheduler::Record`, `Process.cpp`, `ImageLoad.cpp`, `DebugSnapshot.cpp`, `TEST`, `Roots.cpp`, `GarbageFirstBoot`, `Fiber.cpp`, `TEST`, `Ao`, `Boot`, `.nil`, `abortingNew`, `intern`, `performSend`, `Frame`, `abortEvaluation`, `native_send_test.cpp`, `CompiledMethodNatives.cpp`, `MethodSource`, `ImageFormat.cpp`, `Dbg`, `clearUnwinding`, `ClassMethodCache`, `imageRegistryStubA`, `FrameBlock`, `CountingSink`, `.specialSelector`, `Roots.hpp`, `NativeFrame`, `Format.hpp`, `TEST`, `Ao`, `create`, `abortingSubclass`, `collectNursery`, `answerOne`, `Character.cpp`, `WellKnown::InternTable`, `Root`, `SlotNames`?**
  _High betweenness centrality (0.386) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Analysis`, `LargeInteger.cpp`, `Literal`, `Scanner.cpp`, `Compile.cpp`, `ao_main.cpp`, `Oop`, `VendorExtract.cpp`, `NativeMethod.cpp`, `path`, `RealScope`, `Reentry`, `Stack`, `extractVendor`, `ChunkAction`, `Heap`, `TEST_F`, `Scheduler`, `ImageLoad.cpp`, `ClassPool.cpp`, `DebugSnapshot.cpp`, `fiber_test.cpp`, `string`, `ChunkParser.cpp`, `cstdint`, `codegen_snapshot_test.cpp`, `ImageSave.cpp`, `Ast`, `TEST`, `MethodSource`, `Parser`, `Session.cpp`, `Codegen.cpp`, `Roots.hpp`, `DebugSnapshot`, `Parsed`, `Gc.cpp`, `string`, `CompileEnv`, `Behavior.cpp`, `KeptClass`, `Session`, `MethodImage`, `checkFile`, `TEST`, `ImageSurgery`, `Roots`?**
  _High betweenness centrality (0.106) - this node is a cross-community bridge._
- **Why does `TEST()` connect `TEST` to `Oop`, `TEST`, `ao_eval`, `vector`, `PcSpan`, `abi.cpp`, `collectNursery`, `ao_image_load`, `TEST`?**
  _High betweenness centrality (0.057) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `Oop` (e.g. with `Graphify / Serena` and `現状`) actually correct?**
  _`Oop` has 18 INFERRED edges - model-reasoned connections that need verification._
- **Are the 23 inferred relationships involving `TEST()` (e.g. with `PcSpan` and `repeated()`) actually correct?**
  _`TEST()` has 23 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _1102 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013071895424836602 - nodes in this community are weakly interconnected._
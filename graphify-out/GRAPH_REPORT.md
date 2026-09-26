# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 281 files · ~275,384 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5495 nodes · 14677 edges · 232 communities (213 shown, 19 thin omitted)
- Extraction: 85% EXTRACTED · 15% INFERRED · 0% AMBIGUOUS · INFERRED: 2234 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `b8a13515`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- RealScope
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
- vector
- 指摘
- ChunkAction
- Heap
- Boot
- Geometry.cpp
- TEST
- P1 — Object Memory
- TEST
- Scheduler
- TEST
- WorkspaceWindow
- BrowserModelTests
- TranscriptWindow
- P3 — Native Dispatch
- Bytecode interpreter
- ao_main.cpp
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
- MethodImage
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
- applyMethod
- BrowserModel
- TEST_F
- .false_
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
- docs/README.md
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
- Frame
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
- BlockProbe
- .isEmpty
- Compile.cpp
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
- Codegen.cpp
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
- sessionImageLoad
- FileSizeLimit
- allocateRetry
- send2
- cli_test.sh
- .nil
- DefinitionScanner
- session_abi_test.cpp
- 指摘
- P7 .aoimage Implementation Plan
- abortingNew
- [1.0.0] - 2026-09-26
- intern
- claude-review/README.md
- TEST
- 指摘
- P10 — 事後デバッガ
- asNumDen
- CompiledMethodNatives.cpp
- .isHeap
- 手順（PR の順序と依存）
- OperandStack
- Counts
- ObjectHeader
- TestDir
- sessionEval
- abortingSubclass
- imageRegistryStubA
- TEST
- FrameBlock
- DebugFrames
- P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）
- TestSink
- Roots::visitAll
- .specialSelector
- boxLiteral
- ao_abi.h
- TEST
- WellKnown::InternTable
- P10-01: SPEC と CLAUDE.md の改訂、PHASE
- P10-07: 受け入れ、CHANGELOG、Graphify deep
- P9-04: v1 ゴールデン受け入れ
- P10-02: コンパイラの pc→ソース表と temp 名
- 仕様
- codegen
- Bootstrap
- TEST
- Qiita-part1.md
- Ao
- Ao
- DepthGuard
- ScopedGcStressEnv
- TEST
- Root
- acceptClass
- Frame
- valueHashFold
- evalExpr
- P1-04 old-generation mark-compact
- 仕様
- isClassShaped
- FiberStackBounds
- B10 協調スケジューラ（02 High）
- Parsed
- RegisterSwap
- IgnoreFileSizeSignal
- ActiveGuard
- answerOne

## God Nodes (most connected - your core abstractions)
1. `Oop` - 907 edges
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
- `[Low] Inspector ウィンドウが閉じても解放されず、増え続ける` --references--> `InspectorWindow`  [INFERRED]
  docs/claude-review/07-app-build.md → app/Ao/InspectorWindow.swift
- `テスト` --references--> `WorkspaceEvalTests`  [INFERRED]
  docs/prs/P10-06.md → app/AoTests/WorkspaceEvalTests.swift
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp

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

## Communities (232 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): InternTable, Roots, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "RealScope"
Cohesion: 0.08
Nodes (28): Capture, owner, var, uint32_t, uint8_t, RealScope, copied, copiedBase (+20 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (89): ネイティブのルートを構造で保証する, OcShape, Pass, probe, visit, allocateInstance(), ao_Association_key(), ao_Association_key_value_() (+81 more)

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
Cohesion: 0.10
Nodes (64): [High] printOn: が新しい printString を使わない, Graphify / Serena, bits(), isBytes(), isIndexable(), isPointers(), make(), Oop (+56 more)

### Community 8 - "send"
Cohesion: 0.08
Nodes (69): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, Graphify / Serena, Files, int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_() (+61 more)

### Community 9 - "ao_eval"
Cohesion: 0.05
Nodes (59): AoSpan, end, message, start, B2 `to:do:` bench, P4 microbench, P6 interpreter bench, 01 オブジェクトメモリ・GC・ブートストラップ (+51 more)

### Community 10 - "Emitter"
Cohesion: 0.19
Nodes (14): int16_t, Op, size_t, uint16_t, Emitter, real_, hasReceiverChild(), size (+6 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (47): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+39 more)

### Community 13 - "Literal"
Cohesion: 0.11
Nodes (18): int64_t, LitKind, unique_ptr, Literal, bytes, elements, floatValue, intValue (+10 more)

### Community 14 - "Float.cpp"
Cohesion: 0.21
Nodes (30): NumberOp, NumberRelation, ao_Float_add(), ao_Float_divide(), ao_Float_greaterOrEqual(), ao_Float_greaterThan(), ao_Float_lessOrEqual(), ao_Float_lessThan() (+22 more)

### Community 15 - "String.cpp"
Cohesion: 0.12
Nodes (39): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, ao_String_asSymbol(), ao_String_at_() (+31 more)

### Community 16 - "vector"
Cohesion: 0.12
Nodes (33): CompileError, message, span, string, vector, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Task 5: `ao filein` と評価 (+25 more)

### Community 17 - "指摘"
Cohesion: 0.15
Nodes (13): [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] Save / Open Image の失敗がユーザーに見えない, [High] `Smalltalk` が knownGlobals に含まれない (+5 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (70): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_ (+62 more)

### Community 20 - "Boot"
Cohesion: 0.08
Nodes (37): expectAbortedEmpty(), expectSymbol(), fileIn(), runActivationProbe(), string, evalBody(), expectFailAbort(), printOf() (+29 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "TEST"
Cohesion: 0.08
Nodes (26): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+18 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (35): SPEC §3.4 に書く意味論, CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+27 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (60): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+52 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.12
Nodes (18): sendToKeyWorkspace(), Bool, Int, NSWindow, Void, WorkspaceWindow, .errorAccessibilityLabel, .errorText (+10 more)

### Community 28 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.07
Nodes (29): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+21 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.15
Nodes (14): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling (+6 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.14
Nodes (14): dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests(), string (+6 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.13
Nodes (32): chrono, Chunk, classpool, cmath, compile, CompiledMethod, compiler, context (+24 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.06
Nodes (26): ao_abi, BytecodeIsa, cassert, string, Op, uint8_t, operandBytes(), specialCount() (+18 more)

### Community 38 - "TEST_F"
Cohesion: 0.03
Nodes (60): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+52 more)

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
Cohesion: 0.14
Nodes (18): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+10 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.10
Nodes (43): cerrno, climits, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, fcntl, appendRaw(), appendRecord(), collectImageSlot(), byte (+35 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.06
Nodes (30): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+22 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.10
Nodes (25): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+17 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, Roots, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.05
Nodes (48): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, abortSetAside, activeContext (+40 more)

### Community 49 - "TEST"
Cohesion: 0.08
Nodes (27): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+19 more)

### Community 50 - "MethodSource"
Cohesion: 0.08
Nodes (29): uint32_t, PcSpan, end, pc, start, uint32_t, debugInfoFor(), DebugInfoRef (+21 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (31): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+23 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "アプリ"
Cohesion: 0.08
Nodes (35): InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText() (+27 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 58 - "TEST_F"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 59 - "ao_image_load"
Cohesion: 0.07
Nodes (50): [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる, [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free, [Medium] `ao --test` が実行時エラーを失敗として数えない, [Medium] クラス参照とクラスの形を検証せず、壊れたイメージでロード中または直後にプロセスが落ちる (+42 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "MethodImage"
Cohesion: 0.06
Nodes (38): BlockMethodHasItsOwnMapInMethodCoordinates, int16_t, string, uint16_t, uint8_t, MethodImage, bytes, literals (+30 more)

### Community 62 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (29): safepoint, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual() (+21 more)

### Community 64 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+26 more)

### Community 65 - "Session.cpp"
Cohesion: 0.22
Nodes (25): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+17 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.12
Nodes (12): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+4 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".publish"
Cohesion: 0.08
Nodes (16): Any, Int, Notification, NSRect, NSSegmentedControl, NSTableView, NSTextField, NSTextView (+8 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 72 - "CompileEnv"
Cohesion: 0.08
Nodes (27): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+19 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (30): findSymbol, global, internWith, isFixedGlobal, Roots, string_view, ImageSelector, name (+22 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 76 - "DebugSnapshot"
Cohesion: 0.07
Nodes (28): DebugSnapshot, capture, clear, context, frames_, held_, kFixedSlots, kind (+20 more)

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
Cohesion: 0.21
Nodes (20): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+12 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "applyMethod"
Cohesion: 0.12
Nodes (16): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt, コミット文, 完了条件, 手順, 目的, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, スタックガード、abort、死んだホーム (+8 more)

### Community 84 - "BrowserModel"
Cohesion: 0.20
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 86 - ".false_"
Cohesion: 0.14
Nodes (35): isPseudoVariableName, ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+27 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (79): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+71 more)

### Community 88 - "native_send_test.cpp"
Cohesion: 0.26
Nodes (11): inline cache, lookup, P3-05 inline cache and send, send, answerMessage(), CallContext, uint32_t, pairAfterAlloc() (+3 more)

### Community 89 - "abi.cpp"
Cohesion: 0.11
Nodes (30): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at() (+22 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (76): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+68 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.24
Nodes (17): allocateSkeletons(), allocClass(), Roots, string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors() (+9 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.16
Nodes (40): Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_() (+32 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.10
Nodes (26): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+18 more)

### Community 96 - "Session"
Cohesion: 0.09
Nodes (24): clearMethodSources(), CallContext, DebugSink, Roots, uint64_t, unique_ptr, WellKnown, releaseMethodSources() (+16 more)

### Community 97 - ".isTrue"
Cohesion: 0.49
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 103 - "docs/README.md"
Cohesion: 0.19
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 104 - ".build"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 106 - "P11 — ライブデバッガ"
Cohesion: 0.20
Nodes (9): P11 — ライブデバッガ, PR 一覧, TDD, 仕様（設計判断）, 制約, 前提, 受け入れ, 範囲 (+1 more)

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
Cohesion: 0.16
Nodes (36): ifstream, decodeNonHeap, readHeader, acceptWord(), atOffset(), bindAll(), checkFile(), checkGlobals() (+28 more)

### Community 115 - "Frame"
Cohesion: 0.10
Nodes (21): Frame, context, isBlock, method, pc, prev, receiver, sendArgc (+13 more)

### Community 116 - "TEST"
Cohesion: 0.17
Nodes (12): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, fillNursery(), smi(), TEST() (+4 more)

### Community 117 - "AppKit"
Cohesion: 0.12
Nodes (11): Ao, sendToKeyBrowser(), MainActor, NSWindow, Void, SmokeTests, AppKit, CAo (+3 more)

### Community 118 - "ImageFormat"
Cohesion: 0.08
Nodes (30): bit, uint16_t, uint32_t, ImageFormat, encodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes (+22 more)

### Community 119 - "TEST"
Cohesion: 0.04
Nodes (57): AllocateNoGcSpillsToOld, ByteObjectPayload, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty (+49 more)

### Community 120 - "Gc"
Cohesion: 0.09
Nodes (33): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, collectBeforeTenured, collectNursery (+25 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.22
Nodes (28): マングル表, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_() (+20 more)

### Community 122 - "TEST"
Cohesion: 0.05
Nodes (39): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+31 more)

### Community 123 - "size"
Cohesion: 0.27
Nodes (14): allocateNoGc, size, at(), bind(), bindIn(), Roots, string_view, uint32_t (+6 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter.cpp"
Cohesion: 0.19
Nodes (27): branchTruth(), byteCount(), clearNonlocal(), consumeNonlocal(), contextAlive(), CallContext, int16_t, uint32_t (+19 more)

### Community 127 - "Roots"
Cohesion: 0.08
Nodes (27): StackWalker, uint32_t, uint8_t, Roots, add, attachStack, counts, detachStack (+19 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (25): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+17 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.19
Nodes (23): ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+15 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 132 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 133 - ".isEmpty"
Cohesion: 0.16
Nodes (27): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, ao_PositionableStream_position(), ao_WriteStream_nextPut_(), classFormat(), copyPrefix(), int64_t, Root, string_view (+19 more)

### Community 134 - "Compile.cpp"
Cohesion: 0.12
Nodes (53): cctype, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, Task 3: rebind と上書き禁止, string_view, isVendorStub(), acceptClassDef(), acceptMethodSource(), addError() (+45 more)

### Community 135 - "main"
Cohesion: 0.13
Nodes (17): Build, Commits, Contributing, いまの位置, コミット, ビルド, ライセンス, リリース (+9 more)

### Community 136 - "uint64_t"
Cohesion: 0.22
Nodes (16): uint64_t, unordered_map, fileOop(), headerAt(), heapShaped(), bits, ObjectRules, behavior_ (+8 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.22
Nodes (19): add(), addNamed(), apply(), CallContext, NativeFn, Roots, string_view, uint32_t (+11 more)

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.08
Nodes (24): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, 06 イメージ・セッション・C ABI・CLI, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, テストの穴, 概要, B0 準備（テスト基盤） (+16 more)

### Community 141 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 142 - "Reentry"
Cohesion: 0.15
Nodes (13): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), Reentry, codes (+5 more)

### Community 143 - "refreshStackLimit"
Cohesion: 0.23
Nodes (14): Interfaces, refreshStackLimit(), abortReasonText(), string, CallContext, path, Root, string (+6 more)

### Community 144 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 145 - "Codegen.cpp"
Cohesion: 0.09
Nodes (26): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+18 more)

### Community 146 - "FiberStack"
Cohesion: 0.14
Nodes (18): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+10 more)

### Community 147 - "指摘"
Cohesion: 0.12
Nodes (17): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む (+9 more)

### Community 148 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.12
Nodes (16): Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 6: フェーズ完了, 仕様, 範囲 (+8 more)

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
Cohesion: 0.15
Nodes (15): size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt() (+7 more)

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
Cohesion: 0.08
Nodes (23): AbandonDoesNotCapture, BlockFrameKeepsTempsAndHome, CleanupAbortKeepsFirstSnapshot, CleanupFailureAfterNormalEndIsCaptured, DeadlockOnBaseCaptures, テスト, DoesNotUnderstandSynthesizesFrameWithoutMethod, ErrorInNestedMethodCapturesInnermostFirst (+15 more)

### Community 164 - "sessionImageLoad"
Cohesion: 0.16
Nodes (14): SPEC を先に直す, Image, check, load, save, bumpDebugGeneration(), CallContext, string_view (+6 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "allocateRetry"
Cohesion: 0.09
Nodes (30): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, 指摘, [Medium] `shallowCopy`・`printString`・`asSymbol` が、nursery が足りないときに GC を再試行せず失敗する (+22 more)

### Community 167 - "send2"
Cohesion: 0.11
Nodes (19): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+11 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".nil"
Cohesion: 0.08
Nodes (70): flags, slotAt, slotAtPut, afterResume, enqueue, find, leaveLists, liveFibers (+62 more)

### Community 170 - "DefinitionScanner"
Cohesion: 0.23
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 171 - "session_abi_test.cpp"
Cohesion: 0.13
Nodes (11): csignal, future, iterator, mach, mach_vm, pingPongFiber(), step(), stdexcept (+3 more)

### Community 172 - "指摘"
Cohesion: 0.12
Nodes (15): 03 数値・スカラー系 Kernel ネイティブ, [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+7 more)

### Community 173 - "P7 .aoimage Implementation Plan"
Cohesion: 0.15
Nodes (13): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI, Task 6: フェーズを閉じる (+5 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "[1.0.0] - 2026-09-26"
Cohesion: 0.22
Nodes (9): [1.0.0] - 2026-09-26, Ao.app, `ao` CLI, Changelog, Class library and images, Compiler and interpreter, Known limitations, Project (+1 more)

### Community 176 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 177 - "claude-review/README.md"
Cohesion: 0.14
Nodes (11): 00 直近差分（origin/main..HEAD の 6 コミット）, 概要, 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9） (+3 more)

### Community 178 - "TEST"
Cohesion: 0.29
Nodes (6): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, TEST(), VersionOneIsRefusedWithReason

### Community 179 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 180 - "P10 — 事後デバッガ"
Cohesion: 0.22
Nodes (9): P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 受け入れ, 範囲 (+1 more)

### Community 181 - "asNumDen"
Cohesion: 0.22
Nodes (14): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumKind, ao_Float_equals(), ao_Fraction_equals(), asDouble(), asFloat(), asNumDen() (+6 more)

### Community 182 - "CompiledMethodNatives.cpp"
Cohesion: 0.41
Nodes (13): CompiledMethod accessors (P5), Native selector mangling, 規則, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps() (+5 more)

### Community 183 - ".isHeap"
Cohesion: 0.19
Nodes (13): ソース表の拡張（P10-04）, clearWeakAfterOldMark, liveClasses(), Gc::collectOld(), Gc::copy(), Heap::checkNotPoisoned(), Heap::inNursery(), Heap::inOld() (+5 more)

### Community 184 - "手順（PR の順序と依存）"
Cohesion: 0.13
Nodes (20): Files, P10-04: セッションのソース表の拡張と doIt, コミット文, テスト, 完了条件, 手順, 目的, Files (+12 more)

### Community 185 - "OperandStack"
Cohesion: 0.18
Nodes (9): deque, OperandStack, roots, slots, answerWithoutSend(), boolean(), int64_t, WellKnown (+1 more)

### Community 186 - "Counts"
Cohesion: 0.29
Nodes (7): Counts, attachedStacks, frameSlots, handles, ranges, slots, Roots::counts()

### Community 187 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 188 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 189 - "sessionEval"
Cohesion: 0.23
Nodes (13): attachBlocks(), blankOut(), AoInspectFn, MethodImage, optional, Roots, dropDoItDebug(), evalBody() (+5 more)

### Community 190 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 193 - "FrameBlock"
Cohesion: 0.18
Nodes (10): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+2 more)

### Community 194 - "DebugFrames"
Cohesion: 0.20
Nodes (9): DebugFrames, count, kind, method, pc, receiver, selector, temp (+1 more)

### Community 195 - "P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）"
Cohesion: 0.20
Nodes (8): P11 ライブデバッガの設計判断（P10-01 で SPEC §3.13 に書く）, uint32_t, Rec, argCount, base, kind, pc, tempCount

### Community 196 - "TestSink"
Cohesion: 0.25
Nodes (6): CallContext, DebugSink, Roots, TestSink, calls, snap

### Community 197 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 199 - "boxLiteral"
Cohesion: 0.32
Nodes (8): [High] 2^63 以上の整数リテラルが黙って 0 になる, Interfaces, boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), uint32_t, uint8_t

### Community 200 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 201 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 202 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 203 - "P10-01: SPEC と CLAUDE.md の改訂、PHASE"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-01: SPEC と CLAUDE.md の改訂、PHASE, コミット文, テスト, 完了条件, 手順, 目的

### Community 204 - "P10-07: 受け入れ、CHANGELOG、Graphify deep"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-07: 受け入れ、CHANGELOG、Graphify deep, コミット文, テスト, 完了条件, 手順, 目的

### Community 205 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 206 - "P10-02: コンパイラの pc→ソース表と temp 名"
Cohesion: 0.33
Nodes (6): Interfaces, P10-02: コンパイラの pc→ソース表と temp 名, コミット文, テスト, 完了条件, 目的

### Community 207 - "仕様"
Cohesion: 0.12
Nodes (18): Interfaces, P10-05: ao_debug_* ABI, コミット文, テスト, 完了条件, 手順, 目的, ABI（P10-05） (+10 more)

### Community 208 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 209 - "Bootstrap"
Cohesion: 0.10
Nodes (21): algorithm, Bootstrap, cstddef, Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern (+13 more)

### Community 210 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 211 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 212 - "Ao"
Cohesion: 0.22
Nodes (9): Ao, Documentation, Install, Layout, License, Not in v1, Status, Using Ao.app (+1 more)

### Community 213 - "Ao"
Cohesion: 0.22
Nodes (9): Ao, Ao.app の使い方, `ao` CLI の使い方, v1 に無いもの, インストール, ライセンス, 文書, 構成 (+1 more)

### Community 215 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 216 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 217 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 218 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 219 - "Frame"
Cohesion: 0.33
Nodes (6): ContextExitGuard, Frame, Roots, FieldRoots, frame, FrameLink

### Community 220 - "valueHashFold"
Cohesion: 0.27
Nodes (11): bytesValueHash(), HashNesting, CallContext, int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold() (+3 more)

### Community 222 - "P1-04 old-generation mark-compact"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 223 - "仕様"
Cohesion: 0.25
Nodes (8): CLI, extra 節, globals 節, load, OOP エンコード, save, well-known 節, 仕様

### Community 224 - "isClassShaped"
Cohesion: 0.32
Nodes (5): uint32_t, isClassShaped(), superclassOf(), SuperclassWalk, depth_

### Community 225 - "FiberStackBounds"
Cohesion: 0.38
Nodes (7): AO_FIBER_REAL_FRAME, fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitchFinal(), threadStackBounds()

### Community 226 - "B10 協調スケジューラ（02 High）"
Cohesion: 0.29
Nodes (6): B10 協調スケジューラ（02 High）, ファイバ, ファイルごとの変更, プロセスごとの状態, リスク, 手順（各段を緑にしてから次へ進む）

### Community 227 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 228 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 229 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 230 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

### Community 231 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

## Knowledge Gaps
- **1077 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+1072 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2386 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `send`, `Heap.cpp`, `TEST`, `Float.cpp`, `String.cpp`, `vector`, `Heap`, `Boot`, `Geometry.cpp`, `TEST`, `TEST`, `TEST`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `TEST_F`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `MethodSource`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `putNative`, `Session.cpp`, `CompileEnv`, `WellKnown.cpp`, `DebugSnapshot`, `TEST`, `string`, `applyMethod`, `.false_`, `TEST`, `native_send_test.cpp`, `abi.cpp`, `TEST`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `Frame`, `TEST`, `ImageFormat`, `TEST`, `Gc`, `Boolean.cpp`, `TEST`, `size`, `ImageSurgery`, `Interpreter.cpp`, `Roots`, `Array.cpp`, `BlockProbe`, `.isEmpty`, `Compile.cpp`, `uint64_t`, `NativeMethod.cpp`, `path`, `TEST`, `refreshStackLimit`, `Stack`, `Codegen.cpp`, `P6b vendor file-in Implementation Plan`, `Scheduler::Record`, `Process.cpp`, `TEST`, `DebugSnapshot.cpp`, `TEST`, `Roots.cpp`, `GarbageFirstBoot`, `Fiber.cpp`, `TEST`, `sessionImageLoad`, `allocateRetry`, `send2`, `.nil`, `abortingNew`, `intern`, `TEST`, `asNumDen`, `CompiledMethodNatives.cpp`, `.isHeap`, `手順（PR の順序と依存）`, `OperandStack`, `ObjectHeader`, `sessionEval`, `abortingSubclass`, `imageRegistryStubA`, `FrameBlock`, `.specialSelector`, `boxLiteral`, `TEST`, `WellKnown::InternTable`, `仕様`, `Bootstrap`, `Ao`, `Ao`, `Root`, `valueHashFold`, `evalExpr`, `isClassShaped`, `ActiveGuard`, `answerOne`?**
  _High betweenness centrality (0.371) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `RealScope`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Compile.cpp`, `uint64_t`, `Oop`, `NativeMethod.cpp`, `ao_eval`, `path`, `Literal`, `Reentry`, `refreshStackLimit`, `Stack`, `Codegen.cpp`, `ChunkAction`, `Heap`, `Scheduler`, `DebugSnapshot.cpp`, `ao_main.cpp`, `image_save_load_test.cpp`, `ChunkParser.cpp`, `string`, `TEST_F`, `ImageSave.cpp`, `session_abi_test.cpp`, `Ast`, `TEST`, `MethodSource`, `.isHeap`, `手順（PR の順序と依存）`, `Parser`, `MethodImage`, `Session.cpp`, `TEST`, `CompileEnv`, `DebugSnapshot`, `TEST`, `codegen`, `Bootstrap`, `string`, `.false_`, `TEST`, `Bootstrap.cpp`, `isClassShaped`, `Session`, `Parsed`, `ImageLoad.cpp`, `Gc`, `TEST`, `ImageSurgery`, `Roots`?**
  _High betweenness centrality (0.128) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Array.cpp`, `.fromSmallInteger`, `LargeInteger.cpp`, `.isEmpty`, `TEST`, `Oop`, `uint64_t`, `send`, `NativeMethod.cpp`, `Heap.cpp`, `path`, `String.cpp`, `vector`, `Boot`, `TEST`, `Process.cpp`, `GarbageFirstBoot`, `ao_main.cpp`, `image_save_load_test.cpp`, `string`, `.nil`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `intern`, `BlockContext.cpp`, `asNumDen`, `ObjectHeader`, `putNative`, `WellKnown.cpp`, `TEST`, `string`, `Bootstrap`, `native_send_test.cpp`, `Bootstrap.cpp`, `Stream.cpp`, `isClassShaped`, `Session`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `ImageFormat`, `TEST`, `Gc`, `size`?**
  _High betweenness centrality (0.061) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `Oop` (e.g. with `Graphify / Serena` and `現状`) actually correct?**
  _`Oop` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _1077 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013071895424836602 - nodes in this community are weakly interconnected._
- **Should `RealScope` be split into smaller, more focused modules?**
  _Cohesion score 0.07563025210084033 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 276 files · ~267,436 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5256 nodes · 14212 edges · 213 communities (195 shown, 18 thin omitted)
- Extraction: 85% EXTRACTED · 15% INFERRED · 0% AMBIGUOUS · INFERRED: 2201 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `acccccc0`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Codegen.cpp
- Dictionary.cpp
- LargeInteger.cpp
- TEST
- Scanner.cpp
- VendorExtract.cpp
- Oop
- unwinding
- ao_accept_class
- Emitter
- Heap.cpp
- Roots
- Literal
- Float.cpp
- String.cpp
- applyChunks
- 指摘
- ChunkAction
- Heap
- .isEmpty
- Geometry.cpp
- TEST
- P1 — Object Memory
- Boot
- Scheduler
- TEST
- WorkspaceWindow
- TEST
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
- ClassPool.cpp
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- アプリ
- Parser
- TEST_F
- ao_abi.h
- TEST
- DefinitionScanner
- SmallInteger.cpp
- roots
- bootstrap_test.cpp
- classRows
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- TEST
- CompileEnv
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- Token
- string
- TEST
- ao image save
- .isHeap
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
- ao_image_load
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- Qiita-part2.md
- vector
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- AppKit
- ImageFormat
- TEST
- TEST
- Boolean.cpp
- TEST
- bindIn
- ImageSurgery
- TEST
- Interpreter.cpp
- ImageHeader
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- BlockProbe
- allocateRetry
- Compile.cpp
- main
- Ao
- package-app.sh
- NativeMethod.cpp
- path
- Bytecode.hpp
- StackPool
- ao_eval
- clearUnwinding
- Stack
- codegen
- FiberStack
- Image::save
- boxLiteral
- TEST
- Scheduler::Record
- uint64_t
- Process.cpp
- ClassMethodCache
- P4 — Kernel Native Implementation
- interpreter
- TEST
- TEST
- Roots.cpp
- GarbageFirstBoot
- FiberRegs
- Fiber.cpp
- fiber_test.cpp
- Gc.cpp
- Session.cpp
- FileSizeLimit
- 指摘
- CompileError
- cli_test.sh
- .fromSmallInteger
- ObjectHeader
- CompiledMethodNatives.cpp
- PingPong
- P7 .aoimage Implementation Plan
- abortingNew
- [1.0.0] - 2026-09-26
- Gc
- Ao
- Character.cpp
- 指摘
- P10 — 事後デバッガ
- string
- TEST
- P11 — ライブデバッガ
- sessionEval
- acceptClass
- Counts
- 仕様
- TestDir
- 貢献
- abortingSubclass
- imageRegistryStubA
- Qiita-part1.md
- Range
- answerOne
- evalBody
- FrameBlock
- Roots::visitAll
- .specialSelector
- Roots::setStackWalker
- RootedArray
- TEST
- TEST
- P10-01: SPEC と CLAUDE.md の改訂、PHASE
- P10-07: 受け入れ、CHANGELOG、Graphify deep
- Format.hpp
- WellKnown::InternTable
- P10-05: ao_debug_* ABI
- ScopedGcStressEnv
- lookup_test.cpp
- TEST
- create
- NameBag

## God Nodes (most connected - your core abstractions)
1. `Oop` - 877 edges
2. `Heap` - 252 edges
3. `WellKnown` - 167 edges
4. `vector` - 160 edges
5. `Ast` - 104 edges
6. `Roots` - 100 edges
7. `Boot` - 97 edges
8. `TEST()` - 96 edges
9. `TEST()` - 95 edges
10. `BrowserWindow` - 91 edges

## Surprising Connections (you probably didn't know these)
- `範囲` --references--> `BrowserWindow`  [INFERRED]
  docs/superpowers/plans/2026-09-26-p10-debugger.md → app/Ao/BrowserWindow.swift
- `テスト` --references--> `WorkspaceEvalTests`  [INFERRED]
  docs/prs/P10-06.md → app/AoTests/WorkspaceEvalTests.swift
- `[Low] `AoSpan` の区間を捨てているので、エラー位置が分からない` --references--> `AoSpan`  [INFERRED]
  docs/claude-review/07-app-build.md → bridge/ao_abi.h
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

## Communities (213 total, 18 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (62): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+54 more)

### Community 2 - "Dictionary.cpp"
Cohesion: 0.08
Nodes (90): ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, visit, allocateInstance(), ao_Association_key() (+82 more)

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
Nodes (61): [High] printOn: が新しい printString を使わない, Graphify / Serena, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_ (+53 more)

### Community 8 - "unwinding"
Cohesion: 0.16
Nodes (33): int64_t, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_() (+25 more)

### Community 9 - "ao_accept_class"
Cohesion: 0.05
Nodes (42): AoSpan, end, message, start, 01 オブジェクトメモリ・GC・ブートストラップ, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, テストの穴, 概要 (+34 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (17): int16_t, LitKind, Op, size_t, uint16_t, uint8_t, Emitter, real_ (+9 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (44): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+36 more)

### Community 12 - "Roots"
Cohesion: 0.03
Nodes (78): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+70 more)

### Community 13 - "Literal"
Cohesion: 0.07
Nodes (29): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+21 more)

### Community 14 - "Float.cpp"
Cohesion: 0.14
Nodes (46): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals() (+38 more)

### Community 15 - "String.cpp"
Cohesion: 0.17
Nodes (31): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+23 more)

### Community 16 - "applyChunks"
Cohesion: 0.17
Nodes (12): [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, P6b vendor file-in Implementation Plan, Self-review, Task 3: rebind と上書き禁止, 仕様, 前提, 範囲, 結論 (+4 more)

### Community 17 - "指摘"
Cohesion: 0.14
Nodes (13): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+5 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (69): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+61 more)

### Community 20 - ".isEmpty"
Cohesion: 0.17
Nodes (25): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, ao_WriteStream_nextPut_(), classFormat(), copyPrefix(), int64_t, Root, string_view, uint64_t (+17 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (29): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+21 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (58): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+50 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (36): CallContext, Record, size_t, string, unique_ptr, Scheduler, abandonAll, activeProcess (+28 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (57): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+49 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.12
Nodes (18): sendToKeyWorkspace(), Bool, Int, NSWindow, Void, WorkspaceWindow, .errorAccessibilityLabel, .errorText (+10 more)

### Community 28 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

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
Cohesion: 0.21
Nodes (7): dyld, fstream, runtime, ao executable, ao_runtime library, ao_runtime_tests, sstream

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.11
Nodes (39): algorithm, ao_abi, Bootstrap, chrono, Chunk, classpool, cmath, compile (+31 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.08
Nodes (13): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+5 more)

### Community 38 - "TEST_F"
Cohesion: 0.04
Nodes (56): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+48 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.09
Nodes (26): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+18 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.14
Nodes (18): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+10 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.11
Nodes (27): cerrno, climits, csignal, fcntl, collectImageSlot(), size_t, string, string_view (+19 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.06
Nodes (30): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+22 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.09
Nodes (23): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+15 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (31): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, activeContext, bindingHook (+23 more)

### Community 49 - "TEST"
Cohesion: 0.04
Nodes (51): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+43 more)

### Community 50 - "ClassPool.cpp"
Cohesion: 0.22
Nodes (22): isClassShaped(), adopt(), bindingAt(), int64_t, string, string_view, uint32_t, WellKnown (+14 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.18
Nodes (26): P4 microbench, P6 interpreter bench, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value() (+18 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "アプリ"
Cohesion: 0.08
Nodes (37): InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText() (+29 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (24): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+16 more)

### Community 58 - "TEST_F"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "DefinitionScanner"
Cohesion: 0.25
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 62 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 64 - "bootstrap_test.cpp"
Cohesion: 0.40
Nodes (4): format, namedSlotNames(), superclassChainFromRoot(), type_traits

### Community 65 - "classRows"
Cohesion: 0.17
Nodes (28): Interfaces, 手順, browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt() (+20 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.12
Nodes (12): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+4 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".init"
Cohesion: 0.10
Nodes (14): Any, Int, Notification, NSRect, NSSegmentedControl, NSTableView, NSTextField, NSTextView (+6 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "CompileEnv"
Cohesion: 0.09
Nodes (24): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+16 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (35): deque, findSymbol, global, internWith, isFixedGlobal, isPseudoVariableName, bytes(), string_view (+27 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.14
Nodes (13): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+5 more)

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

### Community 80 - "string"
Cohesion: 0.24
Nodes (18): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+10 more)

### Community 81 - "TEST"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - ".isHeap"
Cohesion: 0.10
Nodes (43): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, 手順, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する） (+35 more)

### Community 84 - "BrowserModel"
Cohesion: 0.11
Nodes (18): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, BrowserModelTests (+10 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 86 - ".false_"
Cohesion: 0.18
Nodes (28): superclassOf(), boolean(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes() (+20 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 89 - "abi.cpp"
Cohesion: 0.11
Nodes (31): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry (+23 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (80): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+72 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.25
Nodes (16): allocateSkeletons(), allocClass(), string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeSlotNames() (+8 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.15
Nodes (43): Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents() (+35 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.10
Nodes (26): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+18 more)

### Community 96 - "Session"
Cohesion: 0.11
Nodes (19): CallContext, string, uint64_t, unique_ptr, WellKnown, rerootMethodSources(), Session, cache (+11 more)

### Community 97 - ".isTrue"
Cohesion: 0.35
Nodes (11): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext (+3 more)

### Community 103 - "docs/README.md"
Cohesion: 0.19
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 104 - ".build"
Cohesion: 0.24
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (31): hashedcollection, CallContext, int64_t, uint32_t, Root, Table, array, capacity (+23 more)

### Community 106 - "ao_image_load"
Cohesion: 0.06
Nodes (61): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [High] Save / Open Image の失敗がユーザーに見えない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる (+53 more)

### Community 111 - "AoApp"
Cohesion: 0.12
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 114 - "vector"
Cohesion: 0.11
Nodes (59): vector, ifstream, readHeader, acceptWord(), atOffset(), bindAll(), checkFile(), checkGlobals() (+51 more)

### Community 115 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.10
Nodes (19): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B5 Browser と Workspace のデータ消失（テーマ 4）, B6 イメージとセッションの堅牢化, B7 コンパイラの構文とチャンク形式, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論 (+11 more)

### Community 116 - "TEST"
Cohesion: 0.14
Nodes (17): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, evalBody(), fillNursery() (+9 more)

### Community 117 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "ImageFormat"
Cohesion: 0.15
Nodes (13): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+5 more)

### Community 119 - "TEST"
Cohesion: 0.06
Nodes (39): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+31 more)

### Community 120 - "TEST"
Cohesion: 0.11
Nodes (24): BlockContextKeepsHomeAndCopied, CleanupFailureReleasesItsReasonHandle, ContextGc, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, MethodContextSurvivesNurseryCollection (+16 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 123 - "bindIn"
Cohesion: 0.30
Nodes (12): allocateNoGc, at(), bind(), bindIn(), string_view, uint32_t, WellKnown, each() (+4 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.27
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (62): Files, Interfaces, P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt, コミット文, 完了条件, 目的, フレーム連鎖（P10-03）, ActiveGuard (+54 more)

### Community 127 - "ImageHeader"
Cohesion: 0.11
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (23): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+15 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.12
Nodes (32): bytesValueHash(), HashNesting, CallContext, int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold() (+24 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.19
Nodes (13): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+5 more)

### Community 132 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 133 - "allocateRetry"
Cohesion: 0.09
Nodes (32): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない (+24 more)

### Community 134 - "Compile.cpp"
Cohesion: 0.14
Nodes (39): cctype, FileInError, error, file, method, string, acceptClassDef(), acceptClassSource() (+31 more)

### Community 135 - "main"
Cohesion: 0.15
Nodes (18): Build, Commits, Contributing, License, Releasing, Rules that do not bend, Where the project stands, addRoots (+10 more)

### Community 136 - "Ao"
Cohesion: 0.20
Nodes (10): Ao, Build from source, Documentation, Install, Layout, License, Not in v1, Status (+2 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.24
Nodes (17): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+9 more)

### Community 139 - "path"
Cohesion: 0.18
Nodes (11): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readHeapBytes() (+3 more)

### Community 140 - "Bytecode.hpp"
Cohesion: 0.24
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), TEST(), NewOpsAppendedAfterPrimitive (+2 more)

### Community 141 - "StackPool"
Cohesion: 0.22
Nodes (9): clearShadow(), array, byte, size_t, kPoolLimit, pooledCount, StackPool, count (+1 more)

### Community 142 - "ao_eval"
Cohesion: 0.06
Nodes (42): B2 `to:do:` bench, [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い, [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる, [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される, [Low] Workspace の評価を中断できない（無限ループでアプリが固まる）, [Medium] 64 KiB を超える Print it / Inspect it は副作用だけ残して失敗する (+34 more)

### Community 143 - "clearUnwinding"
Cohesion: 0.16
Nodes (22): ABI（P10-05）, `Object>>halt`（P10-03）, SPEC を先に直す（P10-01）, 仕様, 捕捉（P10-03、配線は P10-04）, refreshStackLimit(), Scheduler::runFiber(), abortReasonText() (+14 more)

### Community 144 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 145 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 146 - "FiberStack"
Cohesion: 0.15
Nodes (16): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, FiberStack, acquire, base_ (+8 more)

### Community 147 - "Image::save"
Cohesion: 0.17
Nodes (20): writeFiller, writeHeader, appendRaw(), appendRecord(), byte, uint64_t, uintptr_t, WellKnown (+12 more)

### Community 148 - "boxLiteral"
Cohesion: 0.12
Nodes (18): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+10 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.08
Nodes (25): Scheduler, uint64_t, unique_ptr, Scheduler::findId(), Scheduler::Record, abandon, awaitingTerminate, ctx (+17 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, Entry, klass (+3 more)

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.04
Nodes (55): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+47 more)

### Community 156 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (52): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+44 more)

### Community 158 - "Roots.cpp"
Cohesion: 0.17
Nodes (13): size_t, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+5 more)

### Community 159 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 160 - "FiberRegs"
Cohesion: 0.14
Nodes (14): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+6 more)

### Community 161 - "Fiber.cpp"
Cohesion: 0.17
Nodes (17): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch() (+9 more)

### Community 162 - "fiber_test.cpp"
Cohesion: 0.16
Nodes (17): mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds (+9 more)

### Community 163 - "Gc.cpp"
Cohesion: 0.16
Nodes (13): cstdlib, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::copy(), Gc::Gc() (+5 more)

### Community 164 - "Session.cpp"
Cohesion: 0.20
Nodes (16): Image, check, load, save, clearMethodSources(), HostOopHook, ensureKernelNatives(), installEmptyCache() (+8 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "指摘"
Cohesion: 0.12
Nodes (16): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む (+8 more)

### Community 167 - "CompileError"
Cohesion: 0.07
Nodes (32): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span (+24 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".fromSmallInteger"
Cohesion: 0.09
Nodes (67): flags, size, slotAt, slotAtPut, afterResume, enqueue, find, leaveLists (+59 more)

### Community 170 - "ObjectHeader"
Cohesion: 0.20
Nodes (9): checkNotPoisoned, uint16_t, uint32_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 171 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 172 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 173 - "P7 .aoimage Implementation Plan"
Cohesion: 0.15
Nodes (13): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 5: CLI, Task 6: フェーズを閉じる, リスク (+5 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "[1.0.0] - 2026-09-26"
Cohesion: 0.22
Nodes (9): [1.0.0] - 2026-09-26, Ao.app, `ao` CLI, Changelog, Class library and images, Compiler and interpreter, Known limitations, Project (+1 more)

### Community 176 - "Gc"
Cohesion: 0.16
Nodes (13): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress (+5 more)

### Community 177 - "Ao"
Cohesion: 0.20
Nodes (10): Ao, Ao.app の使い方, `ao` CLI の使い方, v1 に無いもの, インストール, ソースからのビルド, ライセンス, 文書 (+2 more)

### Community 178 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 179 - "指摘"
Cohesion: 0.17
Nodes (11): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+3 more)

### Community 180 - "P10 — 事後デバッガ"
Cohesion: 0.20
Nodes (10): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 受け入れ (+2 more)

### Community 181 - "string"
Cohesion: 0.14
Nodes (22): anyMethodIn(), CarriedMethod, image, meta, selector, source, carriedMethodName(), compileCarried() (+14 more)

### Community 182 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 183 - "P11 — ライブデバッガ"
Cohesion: 0.20
Nodes (9): P11 — ライブデバッガ, PR 一覧, TDD, 仕様（設計判断）, 制約, 前提, 受け入れ, 範囲 (+1 more)

### Community 184 - "sessionEval"
Cohesion: 0.15
Nodes (23): Files, Interfaces, P10-04: セッションのソース表の拡張と doIt, コミット文, テスト, 完了条件, 手順, 目的 (+15 more)

### Community 185 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 186 - "Counts"
Cohesion: 0.29
Nodes (7): Counts, attachedStacks, frameSlots, handles, ranges, slots, Roots::counts()

### Community 187 - "仕様"
Cohesion: 0.20
Nodes (10): CLI, globals 節, load, OOP エンコード, save, Task 4: load と受け入れ評価, well-known 節, ファイル配置（すべて little-endian） (+2 more)

### Community 188 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 189 - "貢献"
Cohesion: 0.29
Nodes (7): いまの位置, コミット, ビルド, ライセンス, リリース, 曲げない規則, 貢献

### Community 190 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "Qiita-part1.md"
Cohesion: 0.33
Nodes (5): 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった

### Community 193 - "Range"
Cohesion: 0.33
Nodes (5): size_t, Range, first, n, Roots::popRange()

### Community 194 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 195 - "evalBody"
Cohesion: 0.33
Nodes (6): blankOut(), AoInspectFn, optional, evalBody(), knownGlobals, sessionEvalResultCopy()

### Community 196 - "FrameBlock"
Cohesion: 0.40
Nodes (5): FrameBlock, capacity, slots, used, unique_ptr

### Community 197 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

### Community 200 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 201 - "TEST"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 202 - "TEST"
Cohesion: 0.25
Nodes (8): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionStringIsReleaseOneZeroZero, VersionTruncationIsRangeError

### Community 203 - "P10-01: SPEC と CLAUDE.md の改訂、PHASE"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-01: SPEC と CLAUDE.md の改訂、PHASE, コミット文, テスト, 完了条件, 手順, 目的

### Community 204 - "P10-07: 受け入れ、CHANGELOG、Graphify deep"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-07: 受け入れ、CHANGELOG、Graphify deep, コミット文, テスト, 完了条件, 手順, 目的

### Community 205 - "Format.hpp"
Cohesion: 0.48
Nodes (6): bits(), int64_t, isBytes(), isIndexable(), isPointers(), make()

### Community 206 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 207 - "P10-05: ao_debug_* ABI"
Cohesion: 0.33
Nodes (6): Files, P10-05: ao_debug_* ABI, コミット文, テスト, 完了条件, 目的

### Community 208 - "ScopedGcStressEnv"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 209 - "lookup_test.cpp"
Cohesion: 0.60
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 210 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 211 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

### Community 212 - "NameBag"
Cohesion: 0.67
Nodes (3): NameBag, heap, names

## Knowledge Gaps
- **994 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+989 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2246 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Codegen.cpp`, `Dictionary.cpp`, `LargeInteger.cpp`, `TEST`, `VendorExtract.cpp`, `unwinding`, `Heap.cpp`, `Roots`, `Float.cpp`, `String.cpp`, `applyChunks`, `Heap`, `.isEmpty`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `ClassPool.cpp`, `TEST`, `BlockContext.cpp`, `TEST`, `SmallInteger.cpp`, `bootstrap_test.cpp`, `classRows`, `CompileEnv`, `WellKnown.cpp`, `TEST`, `string`, `.isHeap`, `.false_`, `native_send_test.cpp`, `abi.cpp`, `TEST`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `.isTrue`, `HashedCollection.cpp`, `ao_image_load`, `native_method_test.cpp`, `vector`, `TEST`, `TEST`, `TEST`, `Boolean.cpp`, `TEST`, `bindIn`, `Interpreter.cpp`, `ImageHeader`, `Array.cpp`, `BlockProbe`, `allocateRetry`, `Compile.cpp`, `Ao`, `NativeMethod.cpp`, `path`, `clearUnwinding`, `Stack`, `Image::save`, `boxLiteral`, `Scheduler::Record`, `Process.cpp`, `ClassMethodCache`, `TEST`, `TEST`, `Roots.cpp`, `GarbageFirstBoot`, `Fiber.cpp`, `Gc.cpp`, `CompileError`, `.fromSmallInteger`, `ObjectHeader`, `CompiledMethodNatives.cpp`, `abortingNew`, `Gc`, `Ao`, `Character.cpp`, `string`, `sessionEval`, `abortingSubclass`, `imageRegistryStubA`, `Range`, `answerOne`, `FrameBlock`, `.specialSelector`, `RootedArray`, `TEST`, `Format.hpp`, `WellKnown::InternTable`, `lookup_test.cpp`, `create`?**
  _High betweenness centrality (0.337) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `main`, `Compile.cpp`, `Oop`, `NativeMethod.cpp`, `path`, `Roots`, `Literal`, `ao_eval`, `clearUnwinding`, `Stack`, `applyChunks`, `ChunkAction`, `Image::save`, `Scheduler`, `ao_main.cpp`, `fiber_test.cpp`, `Gc.cpp`, `ChunkParser.cpp`, `string`, `Session.cpp`, `image_save_load_test.cpp`, `ImageSave.cpp`, `Ast`, `TEST`, `ClassPool.cpp`, `string`, `sessionEval`, `Parser`, `bootstrap_test.cpp`, `classRows`, `CompileEnv`, `TEST`, `string`, `NameBag`, `.false_`, `TEST`, `Bootstrap.cpp`, `Session`, `ao_image_load`, `TEST`, `ImageSurgery`?**
  _High betweenness centrality (0.170) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Array.cpp`, `Dictionary.cpp`, `LargeInteger.cpp`, `allocateRetry`, `TEST`, `Oop`, `unwinding`, `NativeMethod.cpp`, `Heap.cpp`, `path`, `Float.cpp`, `String.cpp`, `Image::save`, `.isEmpty`, `TEST`, `Process.cpp`, `Boot`, `GarbageFirstBoot`, `ao_main.cpp`, `Gc.cpp`, `image_save_load_test.cpp`, `string`, `.fromSmallInteger`, `ObjectHeader`, `Loaded`, `Gc`, `TEST`, `ClassPool.cpp`, `CallContext`, `BlockContext.cpp`, `bootstrap_test.cpp`, `WellKnown.cpp`, `TEST`, `string`, `lookup_test.cpp`, `.isHeap`, `NameBag`, `.false_`, `native_send_test.cpp`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `HashedCollection.cpp`, `native_method_test.cpp`, `vector`, `TEST`, `TEST`, `bindIn`, `ImageHeader`?**
  _High betweenness centrality (0.060) - this node is a cross-community bridge._
- **Are the 16 inferred relationships involving `Oop` (e.g. with `Graphify / Serena` and `現状`) actually correct?**
  _`Oop` has 16 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _994 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013333333333333334 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.04579630895420369 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 263 files · ~256,480 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5084 nodes · 13742 edges · 196 communities (178 shown, 18 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1958 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `63aee639`
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
- Scheduler.cpp
- ao_eval
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- send
- CompileError
- 指摘
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- Boot
- Scheduler
- TEST
- WorkspaceWindow
- TEST
- TranscriptWindow
- lookup_test.cpp
- Bytecode interpreter
- ao_main.cpp
- TEST
- TEST
- string
- ChunkParser.cpp
- cstdint
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
- ClassPool.cpp
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- TEST
- Parser
- TEST_F
- ao_abi.h
- TEST
- BrowserModelTests
- String.cpp
- roots
- Compile.cpp
- allocateRetry
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
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- Token
- GarbageFirstBoot
- TEST
- ao image save
- Send.cpp
- BrowserModel
- TEST_F
- .false_
- TEST
- TEST
- abi.cpp
- TEST
- .nil
- Stream.cpp
- build.sh
- test.sh
- SPEC.md
- Session.cpp
- .isTrue
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- docs/README.md
- P4 — Kernel Native Implementation
- HashedCollection.cpp
- ao_image_load
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- P7 .aoimage Implementation Plan
- ImageLoad.cpp
- .build
- TEST
- AppKit
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- NativeMethod.cpp
- putNative
- Boolean.cpp
- TEST
- InspectorWindow
- ImageSurgery
- TEST
- Interpreter.cpp
- ImageFormat
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- BlockProbe
- 指摘
- vector
- TEST
- README.md
- package-app.sh
- ObjectHeader
- path
- Codegen.cpp
- SmallInteger.cpp
- Reentry
- TEST
- Roots.cpp
- codegen
- FiberStack
- intern
- uint64_t
- TEST
- Scheduler::Record
- uint64_t
- Process.cpp
- Fiber.cpp
- P4-03 Magnitude / SmallInteger / Character
- interpreter
- TEST
- TEST
- Gc
- WellKnown::checkSmallIntegerFastPath
- PingPong
- StackPool
- fiber_test.cpp
- Parsed
- P1-06: weak slot GC
- FileSizeLimit
- P9-02: Browser accept
- send2
- cli_test.sh
- .isHeap
- Stack
- CompiledMethodNatives.cpp
- 指摘
- P9-04: v1 ゴールデン受け入れ
- abortingNew
- Format.hpp
- 横断テーマ
- Claude レビュー指摘の修正計画（docs/claude-review）
- Table
- TEST
- RootedArray
- FiberRegs
- CompiledMethod
- ClassMethodCache
- native_send_test.cpp
- Ao
- Counts
- P4-06 Stream / Transcript model
- TestDir
- IgnoreFileSizeSignal
- abortingSubclass
- imageRegistryStubA
- image_save_load_test.cpp
- acceptClass
- answerOne
- runOnSmallStack

## God Nodes (most connected - your core abstractions)
1. `Oop` - 876 edges
2. `Heap` - 252 edges
3. `WellKnown` - 167 edges
4. `vector` - 160 edges
5. `Ast` - 103 edges
6. `Roots` - 98 edges
7. `Boot` - 97 edges
8. `TEST()` - 96 edges
9. `TEST()` - 95 edges
10. `TEST()` - 91 edges

## Surprising Connections (you probably didn't know these)
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない` --references--> `isBinaryChar()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/Scanner.cpp
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

## Communities (196 total, 18 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "Analysis"
Cohesion: 0.05
Nodes (41): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+33 more)

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
Cohesion: 0.06
Nodes (74): Global Constraints, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 4: LOAD_ORDER とホストパッチ, Task 6: フェーズ完了, extra 節, string, VendorClassFile (+66 more)

### Community 7 - "Oop"
Cohesion: 0.11
Nodes (58): [High] printOn: が新しい printString を使わない, uint64_t, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_ (+50 more)

### Community 8 - "Scheduler.cpp"
Cohesion: 0.11
Nodes (51): flags, slotAt, slotAtPut, enqueue, find, liveFibers, terminateAll, CallContext (+43 more)

### Community 9 - "ao_eval"
Cohesion: 0.05
Nodes (61): AoSpan, end, message, start, B2 `to:do:` bench, 01 オブジェクトメモリ・GC・ブートストラップ, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, テストの穴 (+53 more)

### Community 10 - "Emitter"
Cohesion: 0.15
Nodes (11): int16_t, Op, size_t, uint16_t, uint8_t, Emitter, real_, hasReceiverChild() (+3 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (45): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldUsed (+37 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (45): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+37 more)

### Community 13 - "Literal"
Cohesion: 0.10
Nodes (22): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+14 more)

### Community 14 - "Float.cpp"
Cohesion: 0.14
Nodes (46): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals() (+38 more)

### Community 15 - "send"
Cohesion: 0.10
Nodes (55): int64_t, refreshStackLimit(), ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan() (+47 more)

### Community 16 - "CompileError"
Cohesion: 0.12
Nodes (24): CompileError, message, span, string, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する (+16 more)

### Community 17 - "指摘"
Cohesion: 0.13
Nodes (14): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+6 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.05
Nodes (37): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+29 more)

### Community 20 - "Roots"
Cohesion: 0.07
Nodes (31): Root, slot, StackWalker, uint32_t, uint8_t, Roots, attachStack, counts (+23 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.10
Nodes (50): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point と Rectangle のネイティブがサブクラスを扱えない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+42 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (29): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+21 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (58): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+50 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (48): CallContext, Record, size_t, string, unique_ptr, Scheduler, abandonAll, activeProcess (+40 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (61): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+53 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (35): aoWorkspaceInspectHook(), failureText(), installErrorField(), keptEvalResult(), selectErrorSpan(), sendToKeyWorkspace(), spanMessage(), Bool (+27 more)

### Community 28 - "TEST"
Cohesion: 0.22
Nodes (9): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, pow2(), ratio() (+1 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.11
Nodes (20): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int32, NSRect (+12 more)

### Community 30 - "lookup_test.cpp"
Cohesion: 0.07
Nodes (30): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+22 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.16
Nodes (17): dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests(), string (+9 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 35 - "string"
Cohesion: 0.12
Nodes (35): ao_abi, chrono, Chunk, classpool, cmath, compile, compiler, string (+27 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "cstdint"
Cohesion: 0.10
Nodes (10): cassert, string_view, cstddef, cstdint, memory, NativeMethod, CallContext, CallContext (+2 more)

### Community 38 - "TEST_F"
Cohesion: 0.04
Nodes (56): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+48 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.10
Nodes (24): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser() (+16 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (48): cerrno, climits, fcntl, encodeNonHeap, writeFiller, writeHeader, Gc::copy(), Heap::checkNotPoisoned() (+40 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.10
Nodes (25): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+17 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (31): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, activeContext, bindingHook (+23 more)

### Community 49 - "TEST"
Cohesion: 0.05
Nodes (41): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+33 more)

### Community 50 - "ClassPool.cpp"
Cohesion: 0.23
Nodes (21): adopt(), bindingAt(), int64_t, string, string_view, uint32_t, WellKnown, instSizeOf() (+13 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (55): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+47 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): P4 microbench, P6 interpreter bench, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value() (+16 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (31): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+23 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 58 - "TEST_F"
Cohesion: 0.05
Nodes (37): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+29 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 62 - "String.cpp"
Cohesion: 0.17
Nodes (31): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+23 more)

### Community 64 - "Compile.cpp"
Cohesion: 0.13
Nodes (45): cctype, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, Task 3: rebind と上書き禁止, string_view, isVendorStub(), acceptClassSource(), acceptMethodSource(), anyMethodIn() (+37 more)

### Community 65 - "allocateRetry"
Cohesion: 0.13
Nodes (20): B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）, SPEC を先に直す, テスト（書く順。各段を緑にしてから次へ進む）, リスク, 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）, CallContext, uint16_t (+12 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.12
Nodes (11): fileInVendor(), LaunchSet, NSMenu, NSMenuItem, NSTextView, NSView, String, T (+3 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - "DefinitionScanner"
Cohesion: 0.09
Nodes (22): DefinitionScanner, Any, Bool, Int, Int32, NSRect, NSSegmentedControl, NSTableView (+14 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 72 - "CompileEnv"
Cohesion: 0.09
Nodes (22): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+14 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (33): deque, findSymbol, global, internWith, deque, size_t, string, string_view (+25 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.22
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

### Community 80 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "Send.cpp"
Cohesion: 0.15
Nodes (24): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, abortDoesNotUnderstand(), abortEvaluation(), abortFailedSend(), abortWithSelector() (+16 more)

### Community 84 - "BrowserModel"
Cohesion: 0.13
Nodes (13): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Notification (+5 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 86 - ".false_"
Cohesion: 0.15
Nodes (34): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+26 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "TEST"
Cohesion: 0.13
Nodes (13): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+5 more)

### Community 89 - "abi.cpp"
Cohesion: 0.13
Nodes (27): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at() (+19 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (78): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+70 more)

### Community 91 - ".nil"
Cohesion: 0.17
Nodes (21): allocateSkeletons(), allocClass(), string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeSlotNames() (+13 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.10
Nodes (69): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_() (+61 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 96 - "Session.cpp"
Cohesion: 0.06
Nodes (95): Image, check, load, save, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition() (+87 more)

### Community 97 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 103 - "docs/README.md"
Cohesion: 0.16
Nodes (21): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 104 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (27): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+19 more)

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.22
Nodes (21): hashedcollection, bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t (+13 more)

### Community 106 - "ao_image_load"
Cohesion: 0.07
Nodes (51): [High] Save / Open Image の失敗がユーザーに見えない, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる, [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free (+43 more)

### Community 111 - "AoApp"
Cohesion: 0.12
Nodes (16): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+8 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "P7 .aoimage Implementation Plan"
Cohesion: 0.12
Nodes (16): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI (+8 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.15
Nodes (37): ifstream, readHeader, acceptWord(), atOffset(), bindAll(), checkFile(), checkGlobals(), byte (+29 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "TEST"
Cohesion: 0.13
Nodes (18): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, string, describe(), evalBody() (+10 more)

### Community 117 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 119 - "NativeMethod.cpp"
Cohesion: 0.23
Nodes (18): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+10 more)

### Community 120 - "putNative"
Cohesion: 0.09
Nodes (33): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+25 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 123 - "InspectorWindow"
Cohesion: 0.20
Nodes (9): InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, [Low] Inspector ウィンドウが閉じても解放されず、増え続ける, NSObjectProtocol (+1 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.27
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (59): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, run, ActiveGuard, rootShared, saved, answerWithoutSend(), applyMethod(), boolean() (+51 more)

### Community 127 - "ImageFormat"
Cohesion: 0.07
Nodes (33): bit, HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap (+25 more)

### Community 128 - "TEST_F"
Cohesion: 0.09
Nodes (21): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+13 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.19
Nodes (23): ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+15 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.19
Nodes (13): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+5 more)

### Community 132 - "BlockProbe"
Cohesion: 0.17
Nodes (10): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, Task 2: 再結合に必要なヒープとレジストリの口, リスク, ensureNativeNames(), BlockProbe, b, probe, int64_t (+2 more)

### Community 133 - "指摘"
Cohesion: 0.08
Nodes (28): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, 指摘, [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む (+20 more)

### Community 134 - "vector"
Cohesion: 0.12
Nodes (29): vector, FileInError, error, file, method, string, isClassShaped(), superclassOf() (+21 more)

### Community 135 - "TEST"
Cohesion: 0.10
Nodes (25): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+17 more)

### Community 136 - "README.md"
Cohesion: 0.09
Nodes (19): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+11 more)

### Community 138 - "ObjectHeader"
Cohesion: 0.12
Nodes (16): CLI, globals 節, load, OOP エンコード, save, well-known 節, ファイル配置（すべて little-endian）, 仕様 (+8 more)

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "Codegen.cpp"
Cohesion: 0.11
Nodes (25): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Inline, LitKind (+17 more)

### Community 141 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 142 - "Reentry"
Cohesion: 0.15
Nodes (13): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), Reentry, codes (+5 more)

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.11
Nodes (22): attached, walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::attached(), Roots::attachStack() (+14 more)

### Community 145 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 146 - "FiberStack"
Cohesion: 0.14
Nodes (17): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, FiberStack, acquire, base_ (+9 more)

### Community 147 - "intern"
Cohesion: 0.22
Nodes (9): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors() (+1 more)

### Community 148 - "uint64_t"
Cohesion: 0.28
Nodes (13): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+5 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.08
Nodes (24): Scheduler, uint64_t, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+16 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "Fiber.cpp"
Cohesion: 0.17
Nodes (17): AO_FIBER_REAL_FRAME, asan_interface, common_interface_defs, pthread, array, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch() (+9 more)

### Community 154 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 156 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (52): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+44 more)

### Community 158 - "Gc"
Cohesion: 0.06
Nodes (43): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, clearWeakAfterOldMark (+35 more)

### Community 160 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 161 - "StackPool"
Cohesion: 0.22
Nodes (9): clearShadow(), array, byte, size_t, kPoolLimit, pooledCount, StackPool, count (+1 more)

### Community 162 - "fiber_test.cpp"
Cohesion: 0.16
Nodes (17): csignal, mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest (+9 more)

### Community 163 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 164 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 167 - "send2"
Cohesion: 0.11
Nodes (20): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+12 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".isHeap"
Cohesion: 0.15
Nodes (22): allocateNoGc, size, at(), bind(), bindIn(), string_view, uint32_t, WellKnown (+14 more)

### Community 170 - "Stack"
Cohesion: 0.10
Nodes (20): FrameBlock, capacity, slots, used, size_t, unique_ptr, Range, first (+12 more)

### Community 171 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 172 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 173 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "Format.hpp"
Cohesion: 0.46
Nodes (7): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make()

### Community 176 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 177 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.11
Nodes (17): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B5 Browser と Workspace のデータ消失（テーマ 4）, B7 コンパイラの構文とチャンク形式, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論 (+9 more)

### Community 178 - "Table"
Cohesion: 0.18
Nodes (10): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+2 more)

### Community 179 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 180 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 181 - "FiberRegs"
Cohesion: 0.14
Nodes (14): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+6 more)

### Community 182 - "CompiledMethod"
Cohesion: 0.33
Nodes (5): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, CompiledMethod, LayoutNativeCodeNil, TEST()

### Community 183 - "ClassMethodCache"
Cohesion: 0.08
Nodes (25): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, 06 イメージ・セッション・C ABI・CLI, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, テストの穴, 概要, 範囲 (+17 more)

### Community 184 - "native_send_test.cpp"
Cohesion: 0.54
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 185 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 186 - "Counts"
Cohesion: 0.29
Nodes (7): Counts, attachedStacks, frameSlots, handles, ranges, slots, Roots::counts()

### Community 187 - "P4-06 Stream / Transcript model"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, Transcript

### Community 188 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 189 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 190 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "image_save_load_test.cpp"
Cohesion: 0.21
Nodes (9): algorithm, Bootstrap, format, gc, iterator, lookup, resource, set (+1 more)

### Community 193 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 194 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

## Knowledge Gaps
- **923 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+918 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2169 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `VendorExtract.cpp`, `Scheduler.cpp`, `Heap.cpp`, `TEST`, `Float.cpp`, `send`, `CompileError`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `Boot`, `Scheduler`, `TEST`, `lookup_test.cpp`, `TEST`, `string`, `cstdint`, `P2-02 class object skeletons`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `ClassPool.cpp`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `TEST`, `String.cpp`, `Compile.cpp`, `allocateRetry`, `WellKnown.cpp`, `TEST`, `GarbageFirstBoot`, `Send.cpp`, `.false_`, `abi.cpp`, `TEST`, `.nil`, `Stream.cpp`, `Session.cpp`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `P7 .aoimage Implementation Plan`, `ImageLoad.cpp`, `TEST`, `NativeMethod.cpp`, `putNative`, `Boolean.cpp`, `TEST`, `Interpreter.cpp`, `ImageFormat`, `Array.cpp`, `BlockProbe`, `指摘`, `vector`, `TEST`, `README.md`, `ObjectHeader`, `path`, `SmallInteger.cpp`, `Roots.cpp`, `intern`, `Scheduler::Record`, `Process.cpp`, `Fiber.cpp`, `TEST`, `TEST`, `Gc`, `WellKnown::checkSmallIntegerFastPath`, `send2`, `.isHeap`, `Stack`, `CompiledMethodNatives.cpp`, `abortingNew`, `Format.hpp`, `Table`, `TEST`, `RootedArray`, `ClassMethodCache`, `native_send_test.cpp`, `Ao`, `abortingSubclass`, `imageRegistryStubA`, `answerOne`?**
  _High betweenness centrality (0.330) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Analysis`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Oop`, `path`, `Codegen.cpp`, `Literal`, `Reentry`, `send`, `CompileError`, `ChunkAction`, `Roots`, `uint64_t`, `Scheduler`, `Gc`, `ao_main.cpp`, `fiber_test.cpp`, `string`, `ChunkParser.cpp`, `cstdint`, `Parsed`, `Stack`, `ImageSave.cpp`, `Ast`, `TEST`, `ClassPool.cpp`, `Parser`, `Compile.cpp`, `image_save_load_test.cpp`, `TEST`, `CompileEnv`, `TEST`, `.false_`, `TEST`, `.nil`, `Session.cpp`, `ImageLoad.cpp`, `NativeMethod.cpp`, `TEST`, `ImageSurgery`?**
  _High betweenness centrality (0.179) - this node is a cross-community bridge._
- **Why does `TEST()` connect `TEST` to `image_save_load_test.cpp`, `.isTrue`, `FileSizeLimit`, `Oop`, `.isHeap`, `ao_eval`, `ImageSave.cpp`, `ao_image_load`, `path`, `send`, `CompileError`, `Loaded`, `Scheduler`, `ImageSurgery`, `Gc`?**
  _High betweenness centrality (0.066) - this node is a cross-community bridge._
- **Are the 15 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 15 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _923 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013245033112582781 - nodes in this community are weakly interconnected._
- **Should `Analysis` be split into smaller, more focused modules?**
  _Cohesion score 0.05217391304347826 - nodes in this community are weakly interconnected._
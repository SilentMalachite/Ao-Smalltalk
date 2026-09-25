# Graph Report - ao-smalltalk  (2026-09-25)

## Corpus Check
- 258 files · ~234,318 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 79 file(s) not represented in the graph (top: .st 64, (none) 13, .toml 1)

## Summary
- 4733 nodes · 12833 edges · 186 communities (167 shown, 19 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1758 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ade43f46`
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
- putNative
- ao_eval
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- send
- applyChunks
- 指摘
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- Boot
- uint64_t
- TEST
- WorkspaceWindow
- kernel_numeric_test.cpp
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
- P9-01: Do it / Print it / Inspect it
- TEST
- Ast
- Loaded
- CallContext
- TEST
- Process.cpp
- heap
- TEST
- BlockContext.cpp
- TEST_F
- .isEmpty
- SmallInteger.cpp
- Parser
- TEST_F
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
- Oop
- TEST
- ao image save
- Character.cpp
- BrowserModel
- TEST_F
- .true_
- TEST
- TEST
- abi.cpp
- TEST
- Bootstrap.cpp
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
- Claude レビュー指摘の修正計画（docs/claude-review）
- HashedCollection.cpp
- ao_image_load
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- .publish
- ImageLoad.cpp
- .build
- TEST
- AppKit
- applyMethod
- NativeMethod.cpp
- 指摘
- Send.cpp
- hashed_collection_test.cpp
- Reentry
- ImageSurgery
- Interpreter.cpp
- ao_WriteStream_nextPut_
- ImageFormat
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- P9-02: Browser accept
- 指摘
- P9-04: v1 ゴールデン受け入れ
- TEST
- Ao
- package-app.sh
- ObjectHeader
- path
- specialIndex
- TEST
- FileInError
- TEST
- Roots.cpp
- codegen
- .isSmallInteger
- intern
- .isHeap
- TEST
- copy
- uint64_t
- RootedArray
- block_test.cpp
- P4 — Kernel Native Implementation
- interpreter
- runFile
- CompileError
- collectNursery
- WellKnown::checkSmallIntegerFastPath
- Gc
- Gc.cpp
- Ao
- P4-07 Process / ProcessorScheduler / Semaphore
- P4-05 Dictionary / Set / OrderedCollection
- FileSizeLimit
- imageRegistryStubA
- TEST
- cli_test.sh
- .nil
- TEST
- TestDir
- answerOne
- bindAll
- abortingNew
- native_send_test.cpp
- 横断テーマ
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- Parsed
- TEST
- createBlock
- TEST
- Entry
- create
- runOnSmallStack

## God Nodes (most connected - your core abstractions)
1. `Oop` - 838 edges
2. `Heap` - 245 edges
3. `WellKnown` - 167 edges
4. `vector` - 154 edges
5. `Ast` - 103 edges
6. `Roots` - 97 edges
7. `TEST()` - 94 edges
8. `TEST()` - 93 edges
9. `TEST()` - 89 edges
10. `BrowserWindow` - 86 edges

## Surprising Connections (you probably didn't know these)
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `ファイル配置（すべて little-endian）` --references--> `ObjectHeader`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/include/ao/Heap.hpp
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

## Communities (186 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): array, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (49): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+41 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (90): ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, allocateInstance(), ao_Association_key() (+82 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (70): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+62 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (46): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks (+38 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.06
Nodes (61): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+53 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.07
Nodes (63): Task 4: LOAD_ORDER とホストパッチ, extra 節, string, VendorClassFile, chunkText, className, superName, unsupportedShape (+55 more)

### Community 7 - "Object.cpp"
Cohesion: 0.12
Nodes (51): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+43 more)

### Community 8 - "putNative"
Cohesion: 0.08
Nodes (35): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+27 more)

### Community 9 - "ao_eval"
Cohesion: 0.05
Nodes (51): AoSpan, end, message, start, 01 オブジェクトメモリ・GC・ブートストラップ, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, テストの穴 (+43 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.08
Nodes (46): charconv, allocateTenured, checkNotPoisoned, fitsOld, growOld, initObject, largeObjectBytes, objectBytes (+38 more)

### Community 12 - "TEST"
Cohesion: 0.05
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+40 more)

### Community 13 - "Literal"
Cohesion: 0.10
Nodes (22): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+14 more)

### Community 14 - "Float.cpp"
Cohesion: 0.08
Nodes (67): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+59 more)

### Community 15 - "send"
Cohesion: 0.19
Nodes (34): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+26 more)

### Community 16 - "applyChunks"
Cohesion: 0.16
Nodes (16): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価 (+8 more)

### Community 17 - "指摘"
Cohesion: 0.12
Nodes (15): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる (+7 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.05
Nodes (58): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+50 more)

### Community 20 - "Roots"
Cohesion: 0.06
Nodes (40): Root, slot, FrameBlock, capacity, slots, used, size_t, StackWalker (+32 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.08
Nodes (26): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+18 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "Boot"
Cohesion: 0.04
Nodes (66): Geometry, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PointAccessorsEqualsAndSetters (+58 more)

### Community 25 - "uint64_t"
Cohesion: 0.30
Nodes (12): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+4 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (76): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+68 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (33): Int32, InspectorWindow, .text, NSTextView, NSWindow, String, failureText(), installErrorField() (+25 more)

### Community 28 - "kernel_numeric_test.cpp"
Cohesion: 0.17
Nodes (13): chrono, cmath, FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, largeinteger, limits, RightShiftOfAMillionBitsIsLinear (+5 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (17): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView (+9 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.11
Nodes (15): classpool, dyld, filesystem, fstream, future, natives, runtime, ao executable (+7 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.13
Nodes (26): algorithm, ao_abi, Bootstrap, Chunk, compile, CompiledMethod, compiler, context (+18 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.08
Nodes (16): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+8 more)

### Community 38 - "TEST_F"
Cohesion: 0.05
Nodes (39): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+31 more)

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
Cohesion: 0.08
Nodes (48): cerrno, climits, csignal, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw() (+40 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.09
Nodes (29): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+21 more)

### Community 47 - "Loaded"
Cohesion: 0.14
Nodes (15): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+7 more)

### Community 48 - "CallContext"
Cohesion: 0.08
Nodes (25): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+17 more)

### Community 49 - "TEST"
Cohesion: 0.08
Nodes (29): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+21 more)

### Community 50 - "Process.cpp"
Cohesion: 0.24
Nodes (28): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+20 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (47): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+39 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.24
Nodes (22): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+14 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 55 - ".isEmpty"
Cohesion: 0.10
Nodes (21): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+13 more)

### Community 56 - "SmallInteger.cpp"
Cohesion: 0.22
Nodes (30): Native selector mangling, マングル表, 規則, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+22 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 58 - "TEST_F"
Cohesion: 0.06
Nodes (34): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+26 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "BrowserModelTests"
Cohesion: 0.22
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 62 - "String.cpp"
Cohesion: 0.16
Nodes (33): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size() (+25 more)

### Community 64 - "vector"
Cohesion: 0.13
Nodes (49): cctype, vector, isClassShaped(), superclassOf(), string_view, isVendorStub(), acceptClassDef(), acceptClassSource() (+41 more)

### Community 65 - "allocateRetry"
Cohesion: 0.14
Nodes (18): B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）, SPEC を先に直す, その他（同じバッチで処理する）, テスト（書く順。各段を緑にしてから次へ進む）, リスク, 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical） (+10 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - "DefinitionScanner"
Cohesion: 0.10
Nodes (19): Any, DefinitionScanner, Bool, Int, NSRect, NSTableView, NSTextField, NSTextView (+11 more)

### Community 70 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (18): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+10 more)

### Community 72 - "CompileEnv"
Cohesion: 0.08
Nodes (28): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+20 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.08
Nodes (32): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+24 more)

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

### Community 80 - "Oop"
Cohesion: 0.14
Nodes (38): Task 8: printString, bits(), isBytes(), isIndexable(), isPointers(), make(), Oop, kCharTag (+30 more)

### Community 81 - "TEST"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 84 - "BrowserModel"
Cohesion: 0.20
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (40): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+32 more)

### Community 86 - ".true_"
Cohesion: 0.21
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "TEST"
Cohesion: 0.05
Nodes (59): AllocateNoGcSpillsToOld, BootstrapInstallsObjectIdentityEquals, ByteObjectPayload, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, ExhaustionReturnsEmpty, HeapAlloc (+51 more)

### Community 89 - "abi.cpp"
Cohesion: 0.08
Nodes (38): atomic, Body, CountsAnswerMinusOneOnFailure, 06 イメージ・セッション・C ABI・CLI, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, テストの穴, 概要 (+30 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (71): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+63 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (23): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+15 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.19
Nodes (32): Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_(), ao_ReadWriteStream_contents(), ao_SmalltalkImage_at_put_() (+24 more)

### Community 95 - "SPEC.md"
Cohesion: 0.07
Nodes (39): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+31 more)

### Community 96 - "Session.cpp"
Cohesion: 0.06
Nodes (91): Image, check, load, save, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition() (+83 more)

### Community 97 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 103 - "docs/README.md"
Cohesion: 0.20
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.10
Nodes (20): [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B5 Browser と Workspace のデータ消失（テーマ 4）, B6 イメージとセッションの堅牢化, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review） (+12 more)

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (29): hashedcollection, CallContext, int64_t, uint32_t, Root, Table, array, capacity (+21 more)

### Community 106 - "ao_image_load"
Cohesion: 0.06
Nodes (53): B2 `to:do:` bench, [High] Save / Open Image の失敗がユーザーに見えない, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる, [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free (+45 more)

### Community 111 - "AoApp"
Cohesion: 0.10
Nodes (16): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+8 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - ".publish"
Cohesion: 0.14
Nodes (8): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value, [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.17
Nodes (34): ifstream, readHeader, acceptWord(), atOffset(), checkFile(), checkGlobals(), byte, size_t (+26 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "TEST"
Cohesion: 0.12
Nodes (16): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+8 more)

### Community 117 - "AppKit"
Cohesion: 0.12
Nodes (11): Ao, aoWorkspaceInspectHook(), CChar, UnsafeMutableRawPointer, UnsafePointer, SmokeTests, AppKit, CAo (+3 more)

### Community 118 - "applyMethod"
Cohesion: 0.11
Nodes (18): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む） (+10 more)

### Community 119 - "NativeMethod.cpp"
Cohesion: 0.07
Nodes (47): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, CLI, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save (+39 more)

### Community 120 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 121 - "Send.cpp"
Cohesion: 0.14
Nodes (26): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, abortDoesNotUnderstand(), abortEvaluation(), abortReasonText(), abortWithSelector() (+18 more)

### Community 122 - "hashed_collection_test.cpp"
Cohesion: 0.09
Nodes (27): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionRandom, HashedCollectionVendor (+19 more)

### Community 123 - "Reentry"
Cohesion: 0.22
Nodes (9): string, expectAllRefused(), Reentry, codes, entered, evalOut, image, loadOrder (+1 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (58): ファイルごとの変更, ファイルごとの変更, pthread, ActiveGuard, rootShared, saved, answerWithoutSend(), boolean() (+50 more)

### Community 126 - "ao_WriteStream_nextPut_"
Cohesion: 0.18
Nodes (26): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, ao_PositionableStream_position(), ao_WriteStream_nextPut_(), collectionSize(), copyPrefix(), int64_t, Root, string_view (+18 more)

### Community 127 - "ImageFormat"
Cohesion: 0.12
Nodes (18): uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes, kImagePointerBits (+10 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (23): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+15 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.19
Nodes (23): ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+15 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.19
Nodes (13): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+5 more)

### Community 132 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 133 - "指摘"
Cohesion: 0.12
Nodes (16): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む (+8 more)

### Community 134 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 135 - "TEST"
Cohesion: 0.09
Nodes (27): cstdlib, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical (+19 more)

### Community 136 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, Build and test, Documentation, Layout, License, Requirements, Status

### Community 138 - "ObjectHeader"
Cohesion: 0.33
Nodes (6): uint16_t, ObjectHeader, flags, hash, klass, size

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "TEST"
Cohesion: 0.14
Nodes (14): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, int64_t, KernelBench, fillNursery(), smi(), TEST() (+6 more)

### Community 142 - "FileInError"
Cohesion: 0.22
Nodes (13): [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, FileInError, error, file, method, string, printFileInErrors(), runFileIn() (+5 more)

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 146 - ".isSmallInteger"
Cohesion: 0.29
Nodes (16): CompiledMethod accessors (P5), int64_t, bumpCounter(), counterAtMax(), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs() (+8 more)

### Community 147 - "intern"
Cohesion: 0.22
Nodes (9): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors() (+1 more)

### Community 148 - ".isHeap"
Cohesion: 0.21
Nodes (11): clearWeakAfterOldMark, Gc::collectOld(), Gc::copy(), Heap::checkNotPoisoned(), Heap::inNursery(), Heap::inOld(), string, toUtf8() (+3 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "copy"
Cohesion: 0.20
Nodes (12): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, 指摘, [Low] Symbol の `copy` が、intern されていない別の Symbol を作る (+4 more)

### Community 152 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 153 - "block_test.cpp"
Cohesion: 0.11
Nodes (19): abortingSubclass(), BlockProbe, b, probe, countingPrintString(), CallContext, int64_t, Root (+11 more)

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.05
Nodes (43): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+35 more)

### Community 156 - "runFile"
Cohesion: 0.31
Nodes (11): refreshStackLimit(), CallContext, path, Root, string, string_view, listTests(), makeAoTest() (+3 more)

### Community 157 - "CompileError"
Cohesion: 0.05
Nodes (42): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, BlockContextForkCreatesAndResumesProcess, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message (+34 more)

### Community 158 - "collectNursery"
Cohesion: 0.36
Nodes (10): collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld() (+2 more)

### Community 160 - "Gc"
Cohesion: 0.18
Nodes (12): Gc, clearWeakAfterNursery, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress, scavengeFromRoots (+4 more)

### Community 161 - "Gc.cpp"
Cohesion: 0.27
Nodes (8): size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::Gc(), Gc::scavengeFromRoots(), unordered_map

### Community 162 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 163 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.29
Nodes (7): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, ProcessorScheduler, Semaphore, send

### Community 164 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.40
Nodes (5): Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 167 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".nil"
Cohesion: 0.11
Nodes (25): [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] 2^63 以上の整数リテラルが黙って 0 になる, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, その他（同じバッチで処理する）, acceptMethodSource(), boxBytes(), boxedOk(), boxLiteral() (+17 more)

### Community 170 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 172 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 173 - "bindAll"
Cohesion: 0.18
Nodes (10): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, bindAll(), fileOop(), relocate(), relocateWord() (+2 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "native_send_test.cpp"
Cohesion: 0.54
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 176 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 177 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.14
Nodes (13): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論, バッチの順序と依存, ファイバ (+5 more)

### Community 178 - "TEST"
Cohesion: 0.29
Nodes (7): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionTruncationIsRangeError

### Community 179 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 180 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 181 - "createBlock"
Cohesion: 0.60
Nodes (4): CallContext, uint8_t, createBlock(), createMethod()

### Community 182 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 183 - "Entry"
Cohesion: 0.50
Nodes (4): Entry, klass, method, selector

### Community 184 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

## Knowledge Gaps
- **845 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+840 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1998 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Object.cpp`, `putNative`, `Heap.cpp`, `TEST`, `Float.cpp`, `send`, `applyChunks`, `Heap`, `Roots`, `Geometry.cpp`, `Boot`, `TEST`, `image_save_load_test.cpp`, `string`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `Process.cpp`, `TEST`, `BlockContext.cpp`, `.isEmpty`, `SmallInteger.cpp`, `String.cpp`, `vector`, `allocateRetry`, `DiskHeader`, `CompileEnv`, `WellKnown.cpp`, `TEST`, `Character.cpp`, `.true_`, `TEST`, `abi.cpp`, `Bootstrap.cpp`, `Stream.cpp`, `Session.cpp`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `applyMethod`, `NativeMethod.cpp`, `Send.cpp`, `hashed_collection_test.cpp`, `ImageSurgery`, `Interpreter.cpp`, `ao_WriteStream_nextPut_`, `Array.cpp`, `TEST`, `Ao`, `ObjectHeader`, `path`, `TEST`, `FileInError`, `Roots.cpp`, `.isSmallInteger`, `intern`, `.isHeap`, `copy`, `RootedArray`, `block_test.cpp`, `runFile`, `CompileError`, `collectNursery`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `Ao`, `imageRegistryStubA`, `.nil`, `answerOne`, `bindAll`, `abortingNew`, `native_send_test.cpp`, `createBlock`, `Entry`, `create`?**
  _High betweenness centrality (0.391) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Object.cpp`, `Emitter`, `path`, `Literal`, `FileInError`, `applyChunks`, `ChunkAction`, `Heap`, `Roots`, `Boot`, `uint64_t`, `block_test.cpp`, `TEST`, `runFile`, `CompileError`, `ao_main.cpp`, `Gc.cpp`, `image_save_load_test.cpp`, `ChunkParser.cpp`, `string`, `ImageSave.cpp`, `bindAll`, `Ast`, `TEST`, `Parsed`, `Parser`, `TEST`, `CompileEnv`, `TEST`, `.true_`, `TEST`, `Bootstrap.cpp`, `Session.cpp`, `ImageLoad.cpp`, `NativeMethod.cpp`, `hashed_collection_test.cpp`, `Reentry`, `ImageSurgery`?**
  _High betweenness centrality (0.127) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `string`, `WellKnown.cpp`, `Oop`, `Heap`, `Roots`, `intern`, `.true_`, `uint64_t`, `Stream.cpp`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.071) - this node is a cross-community bridge._
- **Are the 13 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 13 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _845 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013071895424836602 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.04983164983164983 - nodes in this community are weakly interconnected._
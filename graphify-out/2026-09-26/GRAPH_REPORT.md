# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 264 files · ~252,504 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5062 nodes · 13644 edges · 194 communities (175 shown, 19 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1932 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `e6aaedbb`
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
- vector
- 指摘
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1-07: immovable old objects
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
- bootstrap_test.cpp
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
- copy
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- FileInError
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
- Character.cpp
- BrowserModel
- TEST_F
- allocateRetry
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
- Interpreter::run
- AppKit
- applyMethod
- NativeMethod.cpp
- putNative
- Boolean.cpp
- TEST
- uint32_t
- ImageSurgery
- Interpreter.cpp
- OperandStack
- ImageFormat.cpp
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- P9-02: Browser accept
- 指摘
- P9-04: v1 ゴールデン受け入れ
- TEST
- README.md
- package-app.sh
- ObjectHeader
- path
- Codegen.cpp
- SmallInteger.cpp
- Frame
- TEST
- Stack
- codegen
- FiberStack
- intern
- P1-06: weak slot GC
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
- .isHeap
- Gc.cpp
- PingPong
- P4 — Kernel Native Implementation
- .nil
- FileSizeLimit
- imageRegistryStubA
- send2
- cli_test.sh
- bindIn
- Counts
- CompiledMethodNatives.cpp
- answerOne
- fiber_test.cpp
- abortingNew
- native_send_test.cpp
- claude-review/README.md
- Claude レビュー指摘の修正計画（docs/claude-review）
- Table
- TEST
- RootedArray
- FiberRegs
- TEST
- ClassMethodCache
- FiberStackBounds
- ClassDef
- WellKnown::InternTable
- 貢献
- P4-08 Point / Rectangle
- DepthGuard
- RegisterSwap
- popFrame
- lookup_test.cpp
- ActiveGuard

## God Nodes (most connected - your core abstractions)
1. `Oop` - 876 edges
2. `Heap` - 252 edges
3. `WellKnown` - 167 edges
4. `vector` - 159 edges
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
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp
- `Status` --references--> `Oop`  [INFERRED]
  README.md → runtime/include/ao/Oop.hpp

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

## Communities (194 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "Analysis"
Cohesion: 0.05
Nodes (47): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+39 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (90): ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, visit, ao_Association_key() (+82 more)

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
Cohesion: 0.08
Nodes (60): Task 4: LOAD_ORDER とホストパッチ, string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult (+52 more)

### Community 7 - "Oop"
Cohesion: 0.10
Nodes (63): [High] printOn: が新しい printString を使わない, bits(), int64_t, isBytes(), isIndexable(), isPointers(), make(), Oop (+55 more)

### Community 8 - "Scheduler.cpp"
Cohesion: 0.09
Nodes (51): afterResume, enqueue, find, leaveLists, reapDead, recordFailure, switchTo, takeNext (+43 more)

### Community 9 - "ao_eval"
Cohesion: 0.04
Nodes (58): AoSpan, end, message, start, B2 `to:do:` bench, P4 microbench, P6 interpreter bench, 01 オブジェクトメモリ・GC・ブートストラップ (+50 more)

### Community 10 - "Emitter"
Cohesion: 0.14
Nodes (12): int16_t, Op, size_t, uint16_t, uint8_t, Emitter, real_, hasReceiverChild() (+4 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.08
Nodes (46): charconv, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes, oldCapacity (+38 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+40 more)

### Community 13 - "Literal"
Cohesion: 0.09
Nodes (23): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+15 more)

### Community 14 - "Float.cpp"
Cohesion: 0.10
Nodes (57): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+49 more)

### Community 15 - "send"
Cohesion: 0.10
Nodes (57): boolean(), ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_() (+49 more)

### Community 16 - "vector"
Cohesion: 0.10
Nodes (34): CompileError, message, span, string, vector, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Global Constraints (+26 more)

### Community 17 - "指摘"
Cohesion: 0.18
Nodes (11): [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない, [High] 一度未宣言だった名前が、後から定義したクラスを隠す (+3 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.05
Nodes (38): Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+30 more)

### Community 20 - "Roots"
Cohesion: 0.06
Nodes (34): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, Root, slot, StackWalker, uint8_t, Roots, attachStack, counts (+26 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (49): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+41 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (37): CallContext, Record, size_t, string, unique_ptr, Scheduler, abandonAll, activeProcess (+29 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (58): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+50 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.06
Nodes (36): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), installErrorField() (+28 more)

### Community 28 - "TEST"
Cohesion: 0.15
Nodes (11): FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, RightShiftOfAMillionBitsIsLinear, KernelBench, string, testing::Test, KernelNumeric (+3 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.11
Nodes (19): LaunchSet, aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect (+11 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.11
Nodes (21): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, MethodDictionary (+13 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.10
Nodes (21): climits, dyld, fstream, future, runtime, ao executable, ao_runtime library, ao_runtime_tests (+13 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.08
Nodes (27): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+19 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.12
Nodes (36): algorithm, ao_abi, Bootstrap, chrono, Chunk, classpool, cmath, compile (+28 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.07
Nodes (22): cassert, string, string_view, cstddef, cstdint, Heap, Oop, P3-01 Symbol intern (+14 more)

### Community 38 - "TEST_F"
Cohesion: 0.04
Nodes (59): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+51 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.11
Nodes (21): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, AcceptTests (+13 more)

### Community 41 - "bootstrap_test.cpp"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.16
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (46): cerrno, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, fcntl, writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot() (+38 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+9 more)

### Community 46 - "Ast"
Cohesion: 0.11
Nodes (19): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+11 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (31): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, activeContext, bindingHook (+23 more)

### Community 49 - "TEST"
Cohesion: 0.09
Nodes (22): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+14 more)

### Community 50 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (22): hashedcollection, adopt(), bindingAt(), int64_t, string, string_view, uint32_t, WellKnown (+14 more)

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.21
Nodes (24): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+16 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (32): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+24 more)

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
Cohesion: 0.10
Nodes (23): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+15 more)

### Community 61 - "BrowserModelTests"
Cohesion: 0.22
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 62 - "String.cpp"
Cohesion: 0.14
Nodes (37): bytesValueHash(), int64_t, size_t, valueHashBytes(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_do_() (+29 more)

### Community 64 - "Compile.cpp"
Cohesion: 0.11
Nodes (47): cctype, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, Task 3: rebind と上書き禁止, accept, Task 10: accept, string_view, isVendorStub() (+39 more)

### Community 65 - "copy"
Cohesion: 0.07
Nodes (31): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す (+23 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".init"
Cohesion: 0.11
Nodes (13): Any, Int, NSRect, NSSegmentedControl, NSTableView, NSTextField, NSTextView, NSView (+5 more)

### Community 70 - "DiskHeader"
Cohesion: 0.11
Nodes (19): uint16_t, uint32_t, uint64_t, DiskHeader, endian, extraCount, globalCount, headerBytes (+11 more)

### Community 71 - "FileInError"
Cohesion: 0.08
Nodes (26): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, FileInError, error (+18 more)

### Community 72 - "CompileEnv"
Cohesion: 0.10
Nodes (25): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+17 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.10
Nodes (27): findSymbol, global, internWith, string_view, ImageSelector, name, WellKnown, NamedClass (+19 more)

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

### Community 80 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 81 - "TEST"
Cohesion: 0.16
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
Cohesion: 0.05
Nodes (42): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+34 more)

### Community 86 - "allocateRetry"
Cohesion: 0.09
Nodes (45): 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）, 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）, boxLiteral(), CallContext, uint16_t, uint8_t, create(), CallContext (+37 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "TEST"
Cohesion: 0.06
Nodes (39): AllocateNoGcSpillsToOld, ByteObjectPayload, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, ExhaustionReturnsEmpty, HeapAlloc, IdentityEqualsAndClass (+31 more)

### Community 89 - "abi.cpp"
Cohesion: 0.11
Nodes (30): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at() (+22 more)

### Community 90 - "TEST"
Cohesion: 0.02
Nodes (99): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+91 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.25
Nodes (16): allocateSkeletons(), allocClass(), string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeSlotNames() (+8 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.11
Nodes (65): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_() (+57 more)

### Community 95 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 96 - "Session.cpp"
Cohesion: 0.06
Nodes (93): SPEC を先に直す, SPEC を先に直す, Image, check, load, save, blankOut(), browserClassAt() (+85 more)

### Community 97 - ".isTrue"
Cohesion: 0.24
Nodes (13): Task 1: 形式ヘッダと即値ビット, uint64_t, ImageFormat::decodeNonHeap(), ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_() (+5 more)

### Community 103 - "docs/README.md"
Cohesion: 0.10
Nodes (35): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator (+27 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.12
Nodes (15): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B5 Browser と Workspace のデータ消失（テーマ 4）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+7 more)

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.23
Nodes (20): bumpGeneration(), capacityFor(), copyEntry(), CallContext, int64_t, Root, uint32_t, uint64_t (+12 more)

### Community 106 - "ao_image_load"
Cohesion: 0.08
Nodes (49): [High] Save / Open Image の失敗がユーザーに見えない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる, [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free (+41 more)

### Community 111 - "AoApp"
Cohesion: 0.12
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - ".publish"
Cohesion: 0.11
Nodes (16): DefinitionScanner, sendToKeyBrowser(), Bool, Int32, MainActor, Notification, NSWindow, String (+8 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.07
Nodes (81): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ifstream, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap (+73 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "Interpreter::run"
Cohesion: 0.24
Nodes (16): byteCount(), contextAlive(), CallContext, int16_t, uint8_t, decodeHeader(), instSlot(), Interpreter::run() (+8 more)

### Community 117 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "applyMethod"
Cohesion: 0.11
Nodes (18): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む） (+10 more)

### Community 119 - "NativeMethod.cpp"
Cohesion: 0.08
Nodes (40): CLI, extra 節, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save (+32 more)

### Community 120 - "putNative"
Cohesion: 0.09
Nodes (34): WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter(), WellKnown, installCollection() (+26 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 123 - "uint32_t"
Cohesion: 0.21
Nodes (9): ファイルごとの変更, ファイルごとの変更, uint32_t, unique_ptr, remoteSlot(), Temps, n, roots (+1 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.27
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.47
Nodes (9): branchTruth(), clearNonlocal(), consumeNonlocal(), hit(), Leave, leave, value, miss() (+1 more)

### Community 126 - "OperandStack"
Cohesion: 0.18
Nodes (8): answerWithoutSend(), deque, int64_t, WellKnown, OperandStack, roots, slots, smallIntegerAnswer()

### Community 127 - "ImageFormat.cpp"
Cohesion: 0.32
Nodes (7): bit, byte, size_t, string, ImageFormat::readHeader(), ImageFormat::writeFiller(), ImageFormat::writeHeader()

### Community 128 - "TEST_F"
Cohesion: 0.05
Nodes (41): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+33 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.14
Nodes (29): B9 Kernel の意味論（コレクションとストリーム）, uint64_t, valueHashFold(), valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_() (+21 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

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
Cohesion: 0.11
Nodes (21): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+13 more)

### Community 136 - "README.md"
Cohesion: 0.09
Nodes (20): Build, Commits, Contributing, Current phase, License, Rules that do not bend, Ao, Build and test (+12 more)

### Community 138 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "Codegen.cpp"
Cohesion: 0.11
Nodes (25): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Inline, LitKind (+17 more)

### Community 141 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 142 - "Frame"
Cohesion: 0.20
Nodes (10): ContextExitGuard, FieldRoots, frame, Frame, context, isBlock, method, pc (+2 more)

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Stack"
Cohesion: 0.07
Nodes (36): Range, first, n, attached, walker_, Stack, frameBase_, frameBlock_ (+28 more)

### Community 145 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 146 - "FiberStack"
Cohesion: 0.14
Nodes (18): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, clearShadow(), byte, FiberStack (+10 more)

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.09
Nodes (23): Scheduler, uint64_t, unique_ptr, Scheduler::Record, abandon, awaitingTerminate, ctx, deadlockPending (+15 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "Fiber.cpp"
Cohesion: 0.13
Nodes (19): asan_interface, common_interface_defs, mman, pthread, array, array, size_t, fiberEntered() (+11 more)

### Community 154 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 156 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (56): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+48 more)

### Community 158 - "Gc"
Cohesion: 0.10
Nodes (27): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, collectBeforeTenured, collectNursery (+19 more)

### Community 160 - ".isHeap"
Cohesion: 0.12
Nodes (26): clearWeakAfterOldMark, isClassShaped(), superclassOf(), anyMethodIn(), boxBytes(), boxUtf8(), CallContext, pair (+18 more)

### Community 161 - "Gc.cpp"
Cohesion: 0.11
Nodes (17): cstdlib, nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*) (+9 more)

### Community 162 - "PingPong"
Cohesion: 0.11
Nodes (20): uint64_t, uintptr_t, Deep, fiberRegs, lowest, mainBounds, mainRegs, sum (+12 more)

### Community 163 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (27): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+19 more)

### Community 164 - ".nil"
Cohesion: 0.28
Nodes (16): flags, slotAt, slotAtPut, int64_t, Gc::clearWeakAfterOldMark(), int64_t, ocFirst(), ocRemoveFirst() (+8 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 167 - "send2"
Cohesion: 0.07
Nodes (31): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, GrowKeepsOuterOopAndEntries (+23 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - "bindIn"
Cohesion: 0.30
Nodes (12): allocateNoGc, at(), bind(), bindIn(), string_view, uint32_t, WellKnown, each() (+4 more)

### Community 170 - "Counts"
Cohesion: 0.14
Nodes (13): Counts, attachedStacks, frameSlots, handles, ranges, slots, FrameBlock, capacity (+5 more)

### Community 171 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 172 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 173 - "fiber_test.cpp"
Cohesion: 0.20
Nodes (8): csignal, mach, mach_vm, pingPongFiber(), step(), set, unistd, utility

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "native_send_test.cpp"
Cohesion: 0.54
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 176 - "claude-review/README.md"
Cohesion: 0.14
Nodes (11): 00 直近差分（origin/main..HEAD の 6 コミット）, 概要, 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9） (+3 more)

### Community 177 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.13
Nodes (14): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B5 Browser と Workspace のデータ消失（テーマ 4）, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, バッチの順序と依存 (+6 more)

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
Cohesion: 0.20
Nodes (10): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+2 more)

### Community 182 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 183 - "ClassMethodCache"
Cohesion: 0.08
Nodes (25): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, 06 イメージ・セッション・C ABI・CLI, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, テストの穴, 概要, B3 失敗の伝搬とキャッシュの無効化（テーマ 2） (+17 more)

### Community 184 - "FiberStackBounds"
Cohesion: 0.38
Nodes (7): AO_FIBER_REAL_FRAME, fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitchFinal(), threadStackBounds()

### Community 185 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 186 - "WellKnown::InternTable"
Cohesion: 0.29
Nodes (7): deque, size_t, string, unordered_map, WellKnown::InternTable, byBytes, table

### Community 187 - "貢献"
Cohesion: 0.33
Nodes (6): コミット, ビルド, ライセンス, 曲げない規則, 現行フェーズ, 貢献

### Community 188 - "P4-08 Point / Rectangle"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 190 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 191 - "popFrame"
Cohesion: 0.40
Nodes (5): uint32_t, popFrame, pushFrame, enterNextFrameBlock, returnToPreviousFrameBlock

### Community 192 - "lookup_test.cpp"
Cohesion: 0.60
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 193 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

## Knowledge Gaps
- **927 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+922 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2164 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Scheduler.cpp`, `Heap.cpp`, `TEST`, `Float.cpp`, `send`, `vector`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `bootstrap_test.cpp`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `ClassPool.cpp`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `TEST`, `String.cpp`, `Compile.cpp`, `copy`, `DiskHeader`, `CompileEnv`, `WellKnown.cpp`, `TEST`, `GarbageFirstBoot`, `Character.cpp`, `allocateRetry`, `TEST`, `abi.cpp`, `TEST`, `Bootstrap.cpp`, `Stream.cpp`, `Session.cpp`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `Interpreter::run`, `applyMethod`, `NativeMethod.cpp`, `putNative`, `Boolean.cpp`, `TEST`, `uint32_t`, `Interpreter.cpp`, `OperandStack`, `TEST_F`, `Array.cpp`, `TEST`, `README.md`, `ObjectHeader`, `path`, `SmallInteger.cpp`, `Frame`, `Stack`, `intern`, `Scheduler::Record`, `Process.cpp`, `Fiber.cpp`, `TEST`, `TEST`, `Gc`, `WellKnown::checkSmallIntegerFastPath`, `.isHeap`, `Gc.cpp`, `.nil`, `imageRegistryStubA`, `send2`, `bindIn`, `Counts`, `CompiledMethodNatives.cpp`, `answerOne`, `abortingNew`, `native_send_test.cpp`, `Table`, `TEST`, `RootedArray`, `ClassMethodCache`, `ClassDef`, `WellKnown::InternTable`, `popFrame`, `lookup_test.cpp`, `ActiveGuard`?**
  _High betweenness centrality (0.397) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Analysis`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Oop`, `path`, `Codegen.cpp`, `Literal`, `Stack`, `ChunkAction`, `Roots`, `Scheduler`, `.isHeap`, `ao_main.cpp`, `Gc.cpp`, `image_save_load_test.cpp`, `ChunkParser.cpp`, `string`, `TEST_F`, `bootstrap_test.cpp`, `ImageSave.cpp`, `fiber_test.cpp`, `Ast`, `ClassPool.cpp`, `Parser`, `TEST`, `Compile.cpp`, `FileInError`, `CompileEnv`, `TEST`, `allocateRetry`, `TEST`, `TEST`, `TEST`, `Bootstrap.cpp`, `Session.cpp`, `ImageLoad.cpp`, `NativeMethod.cpp`, `TEST`, `ImageSurgery`?**
  _High betweenness centrality (0.150) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `bootstrap_test.cpp`, `WellKnown.cpp`, `send`, `Heap`, `Roots`, `intern`, `allocateRetry`, `uint64_t`, `Fiber.cpp`, `Stream.cpp`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.069) - this node is a cross-community bridge._
- **Are the 15 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 15 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _927 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013333333333333334 - nodes in this community are weakly interconnected._
- **Should `Analysis` be split into smaller, more focused modules?**
  _Cohesion score 0.05367231638418079 - nodes in this community are weakly interconnected._
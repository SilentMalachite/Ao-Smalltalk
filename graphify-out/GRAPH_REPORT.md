# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 266 files · ~261,701 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5150 nodes · 13870 edges · 201 communities (181 shown, 20 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1980 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `37ffd41e`
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
- Scheduler::terminate
- ao_eval
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- putNative
- applyChunks
- 指摘
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- TEST
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
- P9-01: Do it / Print it / Inspect it
- TEST
- Ast
- Loaded
- CallContext
- TEST
- Interpreter.cpp
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- InspectorWindow
- Parser
- TEST_F
- ao_abi.h
- TEST
- BrowserModelTests
- SmallInteger.cpp
- roots
- vector
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
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- Token
- string
- TEST
- ao image save
- send
- BrowserModel
- TEST_F
- allocateRetry
- TEST
- Bootstrap
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
- ImageLoad.cpp
- Claude レビュー指摘の修正計画（docs/claude-review）
- takeAbortReason
- AppKit
- ImageFormat
- TEST
- Gc
- Boolean.cpp
- TEST
- size
- ImageSurgery
- TEST
- Interpreter::run
- ImageHeader
- TEST_F
- TEST
- Array.cpp
- utility
- BlockProbe
- 指摘
- .isHeap
- main
- Ao
- package-app.sh
- NativeMethod.cpp
- path
- Bytecode.hpp
- Fiber.cpp
- Reentry
- clearUnwinding
- Stack
- codegen
- FiberStack
- intern
- uint64_t
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
- TEST
- FiberRegs
- FiberStackBounds
- fiber_test.cpp
- Parsed
- sessionImageLoad
- FileSizeLimit
- P9-02: Browser accept
- Boot
- cli_test.sh
- .nil
- ObjectHeader
- .isSmallInteger
- PingPong
- P9-04: v1 ゴールデン受け入れ
- abortingNew
- [1.0.0] - 2026-09-26
- 横断テーマ
- Ao
- Character.cpp
- TEST
- Capture
- compileCarried
- TEST
- IgnoreFileSizeSignal
- ListedMethod
- acceptClass
- Counts
- runOnSmallStack
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
- Scheduler::recordFailure

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

## Communities (201 total, 20 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (57): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+49 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (88): ネイティブのルートを構造で保証する, OcShape, Pass, probe, visit, ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+80 more)

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
Nodes (63): [High] printOn: が新しい printString を使わない, bits(), isBytes(), isIndexable(), isPointers(), make(), uint64_t, Oop (+55 more)

### Community 8 - "Scheduler::terminate"
Cohesion: 0.15
Nodes (19): afterResume, enqueue, find, leaveLists, signal, switchTo, takeNext, size_t (+11 more)

### Community 9 - "ao_eval"
Cohesion: 0.04
Nodes (64): AoSpan, end, message, start, 00 直近差分（origin/main..HEAD の 6 コミット）, 概要, 01 オブジェクトメモリ・GC・ブートストラップ, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答 (+56 more)

### Community 10 - "Emitter"
Cohesion: 0.14
Nodes (13): int16_t, Op, size_t, string_view, uint16_t, Emitter, real_, hasReceiverChild() (+5 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 12 - "TEST"
Cohesion: 0.03
Nodes (78): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, EnvEnablesStress (+70 more)

### Community 13 - "Literal"
Cohesion: 0.10
Nodes (22): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+14 more)

### Community 14 - "Float.cpp"
Cohesion: 0.08
Nodes (67): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+59 more)

### Community 15 - "putNative"
Cohesion: 0.09
Nodes (33): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+25 more)

### Community 16 - "applyChunks"
Cohesion: 0.09
Nodes (26): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価 (+18 more)

### Community 17 - "指摘"
Cohesion: 0.18
Nodes (11): [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない, [High] 一度未宣言だった名前が、後から定義したクラスを隠す (+3 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (65): Heap, bytes, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_ (+57 more)

### Community 20 - "Roots"
Cohesion: 0.07
Nodes (31): Root, slot, StackWalker, uint32_t, uint8_t, Roots, attachStack, counts (+23 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.22
Nodes (31): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, allocateInstance(), CallContext, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply() (+23 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "TEST"
Cohesion: 0.08
Nodes (26): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+18 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (37): SPEC §3.4 に書く意味論, CallContext, Record, size_t, string, uint64_t, unique_ptr, Scheduler (+29 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (76): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+68 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.10
Nodes (20): installErrorField(), sendToKeyWorkspace(), Bool, NSTextField, NSTextView, NSWindow, Void, WorkspaceWindow (+12 more)

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
Cohesion: 0.14
Nodes (12): dyld, filesystem, fstream, future, runtime, ao executable, ao_runtime library, ao_runtime_tests (+4 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.12
Nodes (32): ao_abi, chrono, Chunk, cmath, compile, CompiledMethod, compiler, TEST() (+24 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.09
Nodes (13): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+5 more)

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
Cohesion: 0.14
Nodes (18): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+10 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.10
Nodes (41): cerrno, climits, fcntl, appendRaw(), appendRecord(), collectImageSlot(), byte, size_t (+33 more)

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
Cohesion: 0.14
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (31): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, activeContext, bindingHook (+23 more)

### Community 49 - "TEST"
Cohesion: 0.07
Nodes (30): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+22 more)

### Community 50 - "Interpreter.cpp"
Cohesion: 0.23
Nodes (8): algorithm, classpool, hashedcollection, map, natives, pthread, DepthGuard, outermost

### Community 52 - "TEST"
Cohesion: 0.04
Nodes (53): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+45 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.17
Nodes (27): P4 microbench, P6 interpreter bench, safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat() (+19 more)

### Community 54 - "TEST_F"
Cohesion: 0.06
Nodes (32): CleanupFailureKeepsFirstReason, CleanupFailureReleasesItsReasonHandle, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DefaultDoesNotUnderstandAborts, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations (+24 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "InspectorWindow"
Cohesion: 0.10
Nodes (25): InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText() (+17 more)

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

### Community 62 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 64 - "vector"
Cohesion: 0.15
Nodes (27): vector, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, FileInError, error, file, method, string, acceptClassDef() (+19 more)

### Community 65 - "Session.cpp"
Cohesion: 0.27
Nodes (23): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+15 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.12
Nodes (12): fileInVendor(), LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String (+4 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - "DefinitionScanner"
Cohesion: 0.08
Nodes (23): DefinitionScanner, Any, Bool, Int, Int32, Notification, NSRect, NSSegmentedControl (+15 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "CompileEnv"
Cohesion: 0.08
Nodes (26): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+18 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.08
Nodes (33): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+25 more)

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

### Community 80 - "string"
Cohesion: 0.19
Nodes (21): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+13 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "send"
Cohesion: 0.08
Nodes (68): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`） (+60 more)

### Community 84 - "BrowserModel"
Cohesion: 0.17
Nodes (10): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る (+2 more)

### Community 85 - "TEST_F"
Cohesion: 0.06
Nodes (36): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+28 more)

### Community 86 - "allocateRetry"
Cohesion: 0.10
Nodes (42): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+34 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "Bootstrap"
Cohesion: 0.14
Nodes (18): Bootstrap, format, MethodDictionary, uint32_t, RootedArray::RootedArray(), CallContext, uint32_t, stubA() (+10 more)

### Community 89 - "abi.cpp"
Cohesion: 0.08
Nodes (37): AbiSmoke, atomic, Body, BootAndShutdownReturnZero, BootVersionShutdown, CountsAnswerMinusOneOnFailure, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない (+29 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (78): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+70 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.25
Nodes (16): allocateSkeletons(), allocClass(), string_view, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeSlotNames() (+8 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.06
Nodes (100): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_ReadStream_nextPut_() (+92 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.10
Nodes (26): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+18 more)

### Community 96 - "Session"
Cohesion: 0.09
Nodes (25): clearMethodSources(), CallContext, string, uint64_t, unique_ptr, WellKnown, methodSourceRootSlots(), moveMethodSource() (+17 more)

### Community 97 - ".isTrue"
Cohesion: 0.49
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 103 - "docs/README.md"
Cohesion: 0.16
Nodes (21): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 104 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 106 - "ao_image_load"
Cohesion: 0.06
Nodes (57): B2 `to:do:` bench, [High] Save / Open Image の失敗がユーザーに見えない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない (+49 more)

### Community 111 - "AoApp"
Cohesion: 0.12
Nodes (15): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+7 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (36): ifstream, readHeader, atOffset(), bindAll(), checkFile(), checkGlobals(), byte, size_t (+28 more)

### Community 115 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.12
Nodes (16): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B5 Browser と Workspace のデータ消失（テーマ 4）, B7 コンパイラの構文とチャンク形式, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+8 more)

### Community 116 - "takeAbortReason"
Cohesion: 0.10
Nodes (24): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, string, evalBody(), expectFailAbort(), printOf(), int64_t (+16 more)

### Community 117 - "AppKit"
Cohesion: 0.14
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "ImageFormat"
Cohesion: 0.13
Nodes (16): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+8 more)

### Community 119 - "TEST"
Cohesion: 0.13
Nodes (13): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+5 more)

### Community 120 - "Gc"
Cohesion: 0.06
Nodes (43): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, clearWeakAfterOldMark (+35 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.06
Nodes (36): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+28 more)

### Community 123 - "size"
Cohesion: 0.27
Nodes (14): allocateNoGc, size, at(), bind(), bindIn(), string_view, uint32_t, WellKnown (+6 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter::run"
Cohesion: 0.05
Nodes (58): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+50 more)

### Community 127 - "ImageHeader"
Cohesion: 0.10
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (22): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+14 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.19
Nodes (23): ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+15 more)

### Community 131 - "utility"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 132 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 133 - "指摘"
Cohesion: 0.05
Nodes (41): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す (+33 more)

### Community 134 - ".isHeap"
Cohesion: 0.13
Nodes (46): cctype, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, isClassShaped(), superclassOf(), acceptClassSource(), acceptMethodSource(), anyMethodIn(), applyMethodsFor() (+38 more)

### Community 135 - "main"
Cohesion: 0.15
Nodes (18): Build, Commits, Contributing, License, Releasing, Rules that do not bend, Where the project stands, addRoots (+10 more)

### Community 136 - "Ao"
Cohesion: 0.20
Nodes (10): Ao, Build from source, Documentation, Install, Layout, License, Not in v1, Status (+2 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.08
Nodes (40): CLI, extra 節, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save (+32 more)

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "Bytecode.hpp"
Cohesion: 0.24
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), TEST(), NewOpsAppendedAfterPrimitive (+2 more)

### Community 141 - "Fiber.cpp"
Cohesion: 0.16
Nodes (15): asan_interface, common_interface_defs, array, clearShadow(), array, byte, size_t, kPoolLimit (+7 more)

### Community 142 - "Reentry"
Cohesion: 0.15
Nodes (13): BusyRead, copies, evalCodes, lengths, string, expectAllRefused(), Reentry, codes (+5 more)

### Community 143 - "clearUnwinding"
Cohesion: 0.24
Nodes (15): refreshStackLimit(), clearUnwinding(), installEmptyWorkspace(), loadedImageProbes(), sessionWorkspaceReset(), CallContext, path, Root (+7 more)

### Community 144 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 145 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 146 - "FiberStack"
Cohesion: 0.16
Nodes (15): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, FiberStack, acquire, base_ (+7 more)

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "uint64_t"
Cohesion: 0.30
Nodes (13): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+5 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.07
Nodes (31): reapDead, recordFailure, Record, Scheduler, uint64_t, unique_ptr, Scheduler::afterResume(), Scheduler::find() (+23 more)

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
Cohesion: 0.11
Nodes (18): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+10 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (56): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+48 more)

### Community 158 - "Roots.cpp"
Cohesion: 0.17
Nodes (13): size_t, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+5 more)

### Community 159 - "TEST"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 160 - "FiberRegs"
Cohesion: 0.14
Nodes (14): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t (+6 more)

### Community 161 - "FiberStackBounds"
Cohesion: 0.24
Nodes (11): AO_FIBER_REAL_FRAME, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitch() (+3 more)

### Community 162 - "fiber_test.cpp"
Cohesion: 0.15
Nodes (18): csignal, mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest (+10 more)

### Community 163 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 164 - "sessionImageLoad"
Cohesion: 0.22
Nodes (11): Image, check, load, save, HostOopHook, ensureKernelNatives(), installEmptyCache(), Session::Session() (+3 more)

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 167 - "Boot"
Cohesion: 0.04
Nodes (57): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span (+49 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".nil"
Cohesion: 0.18
Nodes (36): flags, slotAt, slotAtPut, terminateAll, CallContext, int64_t, Root, string_view (+28 more)

### Community 170 - "ObjectHeader"
Cohesion: 0.29
Nodes (7): ファイル配置（すべて little-endian）, uint16_t, ObjectHeader, flags, hash, klass, size

### Community 171 - ".isSmallInteger"
Cohesion: 0.31
Nodes (15): CompiledMethod accessors (P5), int64_t, decodeHeader(), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps() (+7 more)

### Community 172 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

### Community 173 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "[1.0.0] - 2026-09-26"
Cohesion: 0.22
Nodes (9): [1.0.0] - 2026-09-26, Ao.app, `ao` CLI, Changelog, Class library and images, Compiler and interpreter, Known limitations, Project (+1 more)

### Community 176 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 177 - "Ao"
Cohesion: 0.20
Nodes (10): Ao, Ao.app の使い方, `ao` CLI の使い方, v1 に無いもの, インストール, ソースからのビルド, ライセンス, 文書 (+2 more)

### Community 178 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 179 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 180 - "Capture"
Cohesion: 0.39
Nodes (4): Capture, owner, var, uint8_t

### Community 181 - "compileCarried"
Cohesion: 0.29
Nodes (8): CarriedMethod, image, meta, selector, source, carriedMethodName(), compileCarried(), Root

### Community 182 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 183 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 184 - "ListedMethod"
Cohesion: 0.25
Nodes (8): ListedMethod, method, native, selector, MethodSource, method, text, sourceOf()

### Community 185 - "acceptClass"
Cohesion: 0.67
Nodes (4): acceptAllocatingKey(), acceptCachingKey(), acceptClass(), acceptMethod()

### Community 186 - "Counts"
Cohesion: 0.29
Nodes (7): Counts, attachedStacks, frameSlots, handles, ranges, slots, Roots::counts()

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
Cohesion: 0.47
Nodes (6): blankOut(), AoInspectFn, optional, evalBody(), knownGlobals, sessionEval()

### Community 196 - "FrameBlock"
Cohesion: 0.40
Nodes (5): FrameBlock, capacity, slots, used, unique_ptr

### Community 197 - "Roots::visitAll"
Cohesion: 0.50
Nodes (4): walker_, Roots::Stack::visit(), Roots::visitAll(), VisitFn

## Knowledge Gaps
- **948 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+943 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2201 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **20 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Scheduler::terminate`, `Heap.cpp`, `TEST`, `Float.cpp`, `putNative`, `applyChunks`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `TEST`, `TEST`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `vector`, `Session.cpp`, `CompileEnv`, `WellKnown.cpp`, `TEST`, `string`, `send`, `allocateRetry`, `Bootstrap`, `abi.cpp`, `TEST`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `takeAbortReason`, `Gc`, `Boolean.cpp`, `TEST`, `size`, `Interpreter::run`, `ImageHeader`, `Array.cpp`, `BlockProbe`, `指摘`, `.isHeap`, `Ao`, `NativeMethod.cpp`, `path`, `clearUnwinding`, `Stack`, `intern`, `Scheduler::Record`, `Process.cpp`, `ClassMethodCache`, `TEST`, `TEST`, `Roots.cpp`, `TEST`, `FiberStackBounds`, `Boot`, `.nil`, `ObjectHeader`, `.isSmallInteger`, `abortingNew`, `Ao`, `Character.cpp`, `TEST`, `ListedMethod`, `abortingSubclass`, `imageRegistryStubA`, `Range`, `answerOne`, `FrameBlock`, `.specialSelector`?**
  _High betweenness centrality (0.352) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `LargeInteger.cpp`, `Scanner.cpp`, `.isHeap`, `VendorExtract.cpp`, `main`, `Oop`, `Emitter`, `NativeMethod.cpp`, `TEST`, `Literal`, `path`, `clearUnwinding`, `Stack`, `codegen`, `ChunkAction`, `Heap`, `Roots`, `applyChunks`, `uint64_t`, `Reentry`, `Scheduler`, `TEST`, `ao_main.cpp`, `fiber_test.cpp`, `Parsed`, `ChunkParser.cpp`, `string`, `image_save_load_test.cpp`, `Boot`, `ImageSave.cpp`, `Ast`, `TEST`, `Interpreter.cpp`, `Capture`, `compileCarried`, `Parser`, `Session.cpp`, `CompileEnv`, `TEST`, `string`, `allocateRetry`, `TEST`, `Bootstrap`, `Bootstrap.cpp`, `Session`, `ImageLoad.cpp`, `Gc`, `TEST`, `ImageSurgery`?**
  _High betweenness centrality (0.172) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Array.cpp`, `.fromSmallInteger`, `LargeInteger.cpp`, `指摘`, `.isHeap`, `Oop`, `TEST`, `NativeMethod.cpp`, `Heap.cpp`, `TEST`, `path`, `Float.cpp`, `putNative`, `intern`, `TEST`, `Process.cpp`, `TEST`, `TEST`, `ao_main.cpp`, `image_save_load_test.cpp`, `string`, `Boot`, `.nil`, `ObjectHeader`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `BlockContext.cpp`, `vector`, `WellKnown.cpp`, `TEST`, `string`, `send`, `Bootstrap`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `TEST`, `Gc`, `size`, `ImageHeader`?**
  _High betweenness centrality (0.059) - this node is a cross-community bridge._
- **Are the 15 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 15 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _948 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013157894736842105 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.049689440993788817 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-26)

## Corpus Check
- 277 files · ~268,574 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 81 file(s) not represented in the graph (top: .st 65, (none) 13, .toml 1)

## Summary
- 5290 nodes · 14283 edges · 216 communities (196 shown, 20 thin omitted)
- Extraction: 85% EXTRACTED · 15% INFERRED · 0% AMBIGUOUS · INFERRED: 2204 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `0bf75e96`
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
- unwinding
- ao_eval
- Emitter
- Heap.cpp
- Roots
- Codegen.cpp
- Float.cpp
- String.cpp
- applyChunks
- 指摘
- ChunkAction
- Heap
- Scheduler::terminate
- Geometry.cpp
- TEST
- P1-04 old-generation mark-compact
- TEST
- Scheduler
- TEST
- WorkspaceWindow
- BrowserModelTests
- TranscriptWindow
- P3 — Native Dispatch
- Bytecode interpreter
- Runtime.cpp
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
- TEST
- SmallInteger.cpp
- roots
- MethodImage
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
- Interpreter.cpp
- ImageHeader
- TEST_F
- TEST
- Array.cpp
- VirtualRegion.cpp
- BlockProbe
- allocateRetry
- vector
- main
- uint64_t
- package-app.sh
- NativeMethod.cpp
- path
- specialIndex
- Fiber.cpp
- B11 App とビルドの残り（07 Medium ×3、Low ×6、テストの穴）
- refreshStackLimit
- Stack
- Analysis
- FiberStack
- applyMethod
- TEST
- TEST
- Scheduler::Record
- uint64_t
- Process.cpp
- TEST
- P4 — Kernel Native Implementation
- interpreter
- TEST
- TEST
- Roots.cpp
- GarbageFirstBoot
- FiberRegs
- FiberStackBounds
- fiber_test.cpp
- VendorClassFile
- sessionImageLoad
- FileSizeLimit
- 指摘
- Boot
- cli_test.sh
- .nil
- ObjectHeader
- .isSmallInteger
- PingPong
- P7 .aoimage Implementation Plan
- abortingNew
- [1.0.0] - 2026-09-26
- intern
- 横断テーマ
- Character.cpp
- 指摘
- P10 — 事後デバッガ
- P9-02: Browser accept
- P9-04: v1 ゴールデン受け入れ
- P11 — ライブデバッガ
- P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）
- InlineCache
- Counts
- 仕様
- TestDir
- Parsed
- abortingSubclass
- imageRegistryStubA
- P1-07: immovable old objects
- Range
- answerOne
- sessionEval
- FrameBlock
- Roots::visitAll
- .specialSelector
- Roots::setStackWalker
- RootedArray
- TEST
- P4-08 Point / Rectangle
- P10-01: SPEC と CLAUDE.md の改訂、PHASE
- P10-07: 受け入れ、CHANGELOG、Graphify deep
- RegisterSwap
- PcSpan
- loadedImageProbes
- ParseResult
- image_save_load_test.cpp
- TEST
- IgnoreFileSizeSignal
- NameBag
- NativeFrame
- DepthGuard
- Scheduler::recordFailure

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

## Communities (216 total, 20 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "RealScope"
Cohesion: 0.07
Nodes (27): Capture, owner, var, uint32_t, RealScope, copied, copiedBase, isBlock (+19 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.08
Nodes (89): ネイティブのルートを構造で保証する, OcShape, Pass, safepoint, probe, visit, ao_Association_key(), ao_Association_key_value_() (+81 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.08
Nodes (82): Digits, [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+74 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (46): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks (+38 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.11
Nodes (50): allowIndex(), containsHostWord(), size_t, string, string_view, doubleBangs(), dropCycles(), dropMissingSupers() (+42 more)

### Community 7 - "Oop"
Cohesion: 0.10
Nodes (64): [High] printOn: が新しい printString を使わない, Graphify / Serena, bits(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+56 more)

### Community 8 - "unwinding"
Cohesion: 0.22
Nodes (26): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_scan(), ao_Collection_includes_(), ao_Collection_includes_scan() (+18 more)

### Community 9 - "ao_eval"
Cohesion: 0.05
Nodes (53): AoSpan, end, message, start, B2 `to:do:` bench, P4 microbench, P6 interpreter bench, 00 直近差分（origin/main..HEAD の 6 コミット） (+45 more)

### Community 10 - "Emitter"
Cohesion: 0.14
Nodes (17): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+9 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.08
Nodes (47): charconv, allocate, allocateTenured, fitsOld, growOld, initObject, largeObjectBytes, objectBytes (+39 more)

### Community 12 - "Roots"
Cohesion: 0.03
Nodes (76): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, CountsFollowEveryKindOfRoot, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, FrameBlocksStayWithTheirStack (+68 more)

### Community 13 - "Codegen.cpp"
Cohesion: 0.08
Nodes (31): int64_t, LitKind, unique_ptr, Literal, bytes, elements, floatValue, intValue (+23 more)

### Community 14 - "Float.cpp"
Cohesion: 0.14
Nodes (46): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumberOp, NumberRelation, NumKind, ao_Float_add(), ao_Float_divide(), ao_Float_equals() (+38 more)

### Community 15 - "String.cpp"
Cohesion: 0.12
Nodes (39): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, ao_String_asSymbol(), ao_String_at_() (+31 more)

### Community 16 - "applyChunks"
Cohesion: 0.17
Nodes (15): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価 (+7 more)

### Community 17 - "指摘"
Cohesion: 0.07
Nodes (27): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる (+19 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (73): Heap, bytes, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_ (+65 more)

### Community 20 - "Scheduler::terminate"
Cohesion: 0.13
Nodes (23): afterResume, find, leaveLists, reapDead, signal, switchTo, takeNext, yield (+15 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1-04 old-generation mark-compact"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 24 - "TEST"
Cohesion: 0.08
Nodes (26): DepthCountsActivationsOnTheContext, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+18 more)

### Community 25 - "Scheduler"
Cohesion: 0.06
Nodes (37): SPEC §3.4 に書く意味論, CallContext, deque, Record, size_t, string, uint64_t, unique_ptr (+29 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (57): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+49 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.12
Nodes (17): sendToKeyWorkspace(), Bool, NSWindow, Void, WorkspaceWindow, .errorAccessibilityLabel, .errorText, .inspectorCount (+9 more)

### Community 28 - "BrowserModelTests"
Cohesion: 0.21
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.07
Nodes (29): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Any, Bool, CChar, Int, Int32 (+21 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "Runtime.cpp"
Cohesion: 0.33
Nodes (4): runtime, ao executable, ao_runtime library, ao_runtime_tests

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.09
Nodes (24): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+16 more)

### Community 35 - "handlescope"
Cohesion: 0.13
Nodes (31): ao_abi, chrono, classpool, cmath, compile, compiler, context, cstdio (+23 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.08
Nodes (14): cassert, Chunk, string, string_view, cstddef, cstdint, memory, NativeMethod (+6 more)

### Community 38 - "TEST_F"
Cohesion: 0.04
Nodes (56): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BaseDeadlockFailsEvalBaseStays, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize (+48 more)

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
Cohesion: 0.11
Nodes (23): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+15 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (48): cerrno, climits, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, fcntl, encodeNonHeap, writeFiller, Gc::copy(), Heap::checkNotPoisoned() (+40 more)

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
Cohesion: 0.14
Nodes (15): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+7 more)

### Community 48 - "CallContext"
Cohesion: 0.05
Nodes (42): BindingHook, CallContext, abandoning, aborting, abortReason, abortReasonHandle, activeContext, bindingHook (+34 more)

### Community 49 - "TEST"
Cohesion: 0.07
Nodes (30): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+22 more)

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
Cohesion: 0.08
Nodes (21): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+13 more)

### Community 55 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, BasicNewColonAtTheBoundAndOddSizes, BasicNewColonRefusesSizesPastUint32, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject (+7 more)

### Community 56 - "アプリ"
Cohesion: 0.09
Nodes (37): InspectorWindow, .text, MainActor, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText() (+29 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (24): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+16 more)

### Community 58 - "TEST_F"
Cohesion: 0.06
Nodes (34): ClassPoolAfterGrowthAndRemoval, ClassPoolNamesAreItsSymbolKeys, ClassPoolOfAnEmptyOrDamagedTable, ClassVariablesThroughTheHashedPool, CopyDoesNotShareTheTable, DamagedOrderedCollectionFails, DamagedTablesFailInEveryNative, DamagedTallyOrArray (+26 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "TEST"
Cohesion: 0.13
Nodes (19): BlockMethodHasItsOwnMapInMethodCoordinates, MethodImage, Op, string, string_view, firstBlock(), named(), pcOf() (+11 more)

### Community 62 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 64 - "MethodImage"
Cohesion: 0.12
Nodes (19): int16_t, string, uint16_t, uint8_t, MethodImage, bytes, literals, numArgs (+11 more)

### Community 65 - "Session.cpp"
Cohesion: 0.27
Nodes (23): browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource() (+15 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.13
Nodes (11): LaunchSet, NSFont, NSMenu, NSMenuItem, NSTextView, NSView, String, T (+3 more)

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - "DefinitionScanner"
Cohesion: 0.07
Nodes (26): DefinitionScanner, Any, Bool, Int, Int32, Notification, NSRect, NSSegmentedControl (+18 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (19): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, set (+11 more)

### Community 72 - "CompileEnv"
Cohesion: 0.07
Nodes (31): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+23 more)

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
Cohesion: 0.24
Nodes (18): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+10 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - ".isHeap"
Cohesion: 0.15
Nodes (34): Interfaces, 捕捉（P10-03、配線は P10-04）, toDoSendingLessOrEqual(), Scheduler::runFiber(), abortDoesNotUnderstand(), abortEvaluation(), abortFailedSend(), abortReasonText() (+26 more)

### Community 84 - "BrowserModel"
Cohesion: 0.20
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "TEST_F"
Cohesion: 0.04
Nodes (51): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+43 more)

### Community 86 - ".false_"
Cohesion: 0.21
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 89 - "abi.cpp"
Cohesion: 0.09
Nodes (35): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Browser の読み取り, Task 3: Browser 読み取り ABI, B5 Browser と Workspace のデータ消失（テーマ 4）, ObjectIsKernelAndPrintStringIsNative (+27 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (78): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+70 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (23): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+15 more)

### Community 92 - "Stream.cpp"
Cohesion: 0.09
Nodes (72): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, 50 時間で v1 に届き、直後のレビューで Critical が 11 件見つかった, Ao は Kernel を C++ で書き、画面を AppKit に任せる処理系, GC 安全性：ネイティブ関数がローカル変数に持ったレシーバが、GC のあとで古い番地を指す, old 領域が 4 MB で頭打ちになり、full GC を 88 回回していた, ブロックの意味論：`ifTrue:` の中の代入と `^` が正しく動かなかった, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents() (+64 more)

### Community 94 - "test.sh"
Cohesion: 0.70
Nodes (4): app_pids(), cleanup(), test.sh script, usage()

### Community 95 - "SPEC.md"
Cohesion: 0.10
Nodes (26): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+18 more)

### Community 96 - "Session"
Cohesion: 0.09
Nodes (23): clearMethodSources(), CallContext, string, uint64_t, unique_ptr, WellKnown, releaseMethodSources(), rerootMethodSources() (+15 more)

### Community 97 - ".isTrue"
Cohesion: 0.44
Nodes (10): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext, uint32_t (+2 more)

### Community 103 - "docs/README.md"
Cohesion: 0.10
Nodes (28): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator (+20 more)

### Community 104 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 105 - "HashedCollection.cpp"
Cohesion: 0.12
Nodes (30): CallContext, int64_t, uint32_t, Root, Table, array, capacity, generation (+22 more)

### Community 106 - "ao_image_load"
Cohesion: 0.06
Nodes (55): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, [High] Save / Open Image の失敗がユーザーに見えない, 06 イメージ・セッション・C ABI・CLI, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする (+47 more)

### Community 111 - "AoApp"
Cohesion: 0.12
Nodes (17): AoApp, fileInVendor(), openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification (+9 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "Qiita-part2.md"
Cohesion: 0.14
Nodes (13): Codex は、修正が持ち込んだ退行と、見落としていた不具合を捕まえた, Cuis のコアはパッケージに無く、変更ログにあった, double 版の `std::from_chars` は 2〜36 進の小数を読めなかった, macOS の `open` は環境変数をアプリに渡していた, ベンチマークは Debug ビルドの数字だった, 仕様と手順は先に固めたが、道具の強制は守り切れなかった, 例外の仕組みや remembered set は、まだ後回しにしている, 効いたやり方：再現してから直し、直したものを別のエージェントに疑わせる (+5 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (35): ifstream, readHeader, atOffset(), bindAll(), checkFile(), checkGlobals(), byte, size_t (+27 more)

### Community 115 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.11
Nodes (17): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B6 イメージとセッションの堅牢化, B7 コンパイラの構文とチャンク形式, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, バッチの順序と依存 (+9 more)

### Community 116 - "takeAbortReason"
Cohesion: 0.12
Nodes (20): GrowAndContentsWithFullNursery, OverwriteAndReserveWithFullNursery, ReadStreamContentsOfFortyThousandCharacters, string, evalBody(), expectFailAbort(), printOf(), int64_t (+12 more)

### Community 117 - "AppKit"
Cohesion: 0.10
Nodes (11): Ao, sendToKeyBrowser(), MainActor, NSWindow, Void, SmokeTests, AppKit, CAo (+3 more)

### Community 118 - "ImageFormat"
Cohesion: 0.14
Nodes (16): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes (+8 more)

### Community 119 - "TEST"
Cohesion: 0.06
Nodes (36): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+28 more)

### Community 120 - "Gc"
Cohesion: 0.04
Nodes (54): BlockContextKeepsHomeAndCopied, BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, CompiledMethod, ContextGc, cstdlib, Files, Interfaces (+46 more)

### Community 121 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 122 - "TEST"
Cohesion: 0.08
Nodes (28): AtPutGrowRemoveAndEnumerateWithFullNursery, BagSizeCountsWhatWasAdded, CountPastSmallIntegerIsALargeInteger, DictionaryAlignedKeysAtPut, DictionaryTenThousandAtPut, HashedClassPool, HashedCollectionGc, HashedCollectionInterval (+20 more)

### Community 123 - "size"
Cohesion: 0.27
Nodes (14): allocateNoGc, size, at(), bind(), bindIn(), string_view, uint32_t, WellKnown (+6 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.26
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 126 - "Interpreter.cpp"
Cohesion: 0.07
Nodes (55): フレーム連鎖（P10-03）, ActiveGuard, rootShared, saved, answerWithoutSend(), boolean(), branchTruth(), byteCount() (+47 more)

### Community 127 - "ImageHeader"
Cohesion: 0.10
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 128 - "TEST_F"
Cohesion: 0.08
Nodes (25): AppendingKeepsTheStringSubclass, ContentsChecksTheRangeBeforeAllocating, ContentsFailsPastTheCollectionAndOnElementsThatDoNotFit, ContentsOnAByteArrayAnswersAByteArray, ContentsOnAnArraySubclassKeepsTheClassAndItsElements, ContentsOnAnOrderedCollectionAnswersAnOrderedCollection, ContentsOnOtherCollectionsAnswersAnArray, ContentsOnStringsAndSymbols (+17 more)

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.12
Nodes (32): bytesValueHash(), HashNesting, CallContext, int64_t, size_t, uint64_t, valueHashBytes(), valueHashFold() (+24 more)

### Community 131 - "VirtualRegion.cpp"
Cohesion: 0.23
Nodes (11): size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit, release (+3 more)

### Community 132 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 133 - "allocateRetry"
Cohesion: 0.09
Nodes (29): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）, SPEC を先に直す, その他（同じバッチで処理する）, テスト（書く順。各段を緑にしてから次へ進む）, リスク (+21 more)

### Community 134 - "vector"
Cohesion: 0.08
Nodes (77): cctype, vector, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Interfaces, FileInError, error, file (+69 more)

### Community 135 - "main"
Cohesion: 0.05
Nodes (45): Build, Commits, Contributing, いまの位置, コミット, ビルド, ライセンス, リリース (+37 more)

### Community 136 - "uint64_t"
Cohesion: 0.30
Nodes (12): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+4 more)

### Community 138 - "NativeMethod.cpp"
Cohesion: 0.32
Nodes (15): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+7 more)

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "Fiber.cpp"
Cohesion: 0.14
Nodes (17): asan_interface, common_interface_defs, mman, pthread, array, clearShadow(), array, byte (+9 more)

### Community 142 - "B11 App とビルドの残り（07 Medium ×3、Low ×6、テストの穴）"
Cohesion: 0.11
Nodes (23): B11 App とビルドの残り（07 Medium ×3、Low ×6、テストの穴）, B11 では扱わない, SPEC を先に直す（最初のコミット。`.gitignore` も含める）, リスク, 検証, 評価結果の保持（07 Medium「64 KiB」、Low「NUL」）, ao_eval_result_copy(), ao_eval_result_length() (+15 more)

### Community 143 - "refreshStackLimit"
Cohesion: 0.31
Nodes (11): refreshStackLimit(), CallContext, path, Root, string, string_view, listTests(), makeAoTest() (+3 more)

### Community 144 - "Stack"
Cohesion: 0.14
Nodes (13): attached, Stack, frameBase_, frameBlock_, frameBlocks_, frameCap_, frameSlotCount, frameUsed_ (+5 more)

### Community 145 - "Analysis"
Cohesion: 0.07
Nodes (24): Codegen, CompileResult, error, image, ok, Analysis, declared, error (+16 more)

### Community 146 - "FiberStack"
Cohesion: 0.15
Nodes (16): DeepRecursionOnFiberStack, Fiber, GuardPageIsProtNone, PingPongKeepsIntAndDoubleLocals, PoolReusesStacks, FiberStack, acquire, base_ (+8 more)

### Community 147 - "applyMethod"
Cohesion: 0.12
Nodes (17): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, Files, P10-03: フレーム連鎖、abort 時の捕捉、Object>>halt, コミット文, 完了条件, 手順, 目的, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4） (+9 more)

### Community 148 - "TEST"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "Scheduler::Record"
Cohesion: 0.08
Nodes (25): Scheduler, uint64_t, unique_ptr, Scheduler::findId(), Scheduler::Record, abandon, awaitingTerminate, ctx (+17 more)

### Community 152 - "Process.cpp"
Cohesion: 0.35
Nodes (21): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_Process_terminate() (+13 more)

### Community 153 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, Lookup, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.05
Nodes (43): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+35 more)

### Community 156 - "TEST"
Cohesion: 0.11
Nodes (18): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+10 more)

### Community 157 - "TEST"
Cohesion: 0.04
Nodes (57): AbandonSkipsCleanupsAndRestoresRoots, ActiveProcessInsideForkIsForked, BaseDeadlockIsFailureActiveStaysBase, BlockContextForkCreatesAndResumesProcess, FiberCountersFoldIntoBase, FiftyWaitersSurviveGcStressAndOldGc, ForkDnuTerminatesOnlyFork, ForkFifoOrder (+49 more)

### Community 158 - "Roots.cpp"
Cohesion: 0.17
Nodes (13): size_t, uint32_t, Roots::add(), Roots::attached(), Roots::detachStack(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+5 more)

### Community 159 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 160 - "FiberRegs"
Cohesion: 0.20
Nodes (8): fiberInit(), FiberRegs, d, fp, lr, sp, x, uint64_t

### Community 161 - "FiberStackBounds"
Cohesion: 0.24
Nodes (11): AO_FIBER_REAL_FRAME, fiberEntered(), fiberSanitizerFinishSwitch(), fiberSanitizerStartSwitch(), FiberStackBounds, bottom, size, fiberSwitch() (+3 more)

### Community 162 - "fiber_test.cpp"
Cohesion: 0.15
Nodes (18): csignal, mach, mach_vm, uint64_t, uintptr_t, Deep, fiberRegs, lowest (+10 more)

### Community 163 - "VendorClassFile"
Cohesion: 0.22
Nodes (10): Task 4: LOAD_ORDER とホストパッチ, string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult (+2 more)

### Community 164 - "sessionImageLoad"
Cohesion: 0.38
Nodes (7): Image, check, load, save, ensureKernelNatives(), sessionImageLoad(), sessionImageSave()

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "指摘"
Cohesion: 0.08
Nodes (28): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, 指摘, [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む (+20 more)

### Community 167 - "Boot"
Cohesion: 0.05
Nodes (51): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span (+43 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - ".nil"
Cohesion: 0.17
Nodes (38): flags, slotAt, slotAtPut, enqueue, terminateAll, CallContext, int64_t, Root (+30 more)

### Community 170 - "ObjectHeader"
Cohesion: 0.22
Nodes (8): checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 171 - ".isSmallInteger"
Cohesion: 0.29
Nodes (16): CompiledMethod accessors (P5), int64_t, bumpCounter(), counterAtMax(), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs() (+8 more)

### Community 172 - "PingPong"
Cohesion: 0.20
Nodes (10): PingPong, alternated, fiberBounds, fiberFrame, fiberN, fiberRegs, fiberX, mainBounds (+2 more)

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
Cohesion: 0.22
Nodes (9): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors() (+1 more)

### Community 177 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 178 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 179 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 180 - "P10 — 事後デバッガ"
Cohesion: 0.20
Nodes (10): Graphify / Serena, P10 — 事後デバッガ, PR 一覧, TDD, 仕様, 制約, 前提, 受け入れ (+2 more)

### Community 181 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 182 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 183 - "P11 — ライブデバッガ"
Cohesion: 0.20
Nodes (9): P11 — ライブデバッガ, PR 一覧, TDD, 仕様（設計判断）, 制約, 前提, 受け入れ, 範囲 (+1 more)

### Community 184 - "P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）"
Cohesion: 0.10
Nodes (26): Files, P10-04: セッションのソース表の拡張と doIt, コミット文, テスト, 完了条件, 手順, 目的, `Object>>halt`（P10-03） (+18 more)

### Community 185 - "InlineCache"
Cohesion: 0.33
Nodes (6): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, IcGuard, ic

### Community 186 - "Counts"
Cohesion: 0.29
Nodes (7): Counts, attachedStacks, frameSlots, handles, ranges, slots, Roots::counts()

### Community 187 - "仕様"
Cohesion: 0.22
Nodes (9): CLI, extra 節, globals 節, load, OOP エンコード, save, well-known 節, ファイル配置（すべて little-endian） (+1 more)

### Community 188 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 189 - "Parsed"
Cohesion: 0.29
Nodes (7): Parsed, globals, heapBytes, offsets, section, starts, wellKnown

### Community 190 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 191 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 192 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 193 - "Range"
Cohesion: 0.33
Nodes (5): size_t, Range, first, n, Roots::popRange()

### Community 194 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 195 - "sessionEval"
Cohesion: 0.38
Nodes (7): blankOut(), AoInspectFn, optional, evalBody(), knownGlobals, sessionEval(), sessionEvalResultCopy()

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
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 202 - "P4-08 Point / Rectangle"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 203 - "P10-01: SPEC と CLAUDE.md の改訂、PHASE"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-01: SPEC と CLAUDE.md の改訂、PHASE, コミット文, テスト, 完了条件, 手順, 目的

### Community 204 - "P10-07: 受け入れ、CHANGELOG、Graphify deep"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P10-07: 受け入れ、CHANGELOG、Graphify deep, コミット文, テスト, 完了条件, 手順, 目的

### Community 205 - "RegisterSwap"
Cohesion: 0.33
Nodes (6): RegisterSwap, fiberIn, fiberOut, fiberRegs, mainBounds, mainRegs

### Community 206 - "PcSpan"
Cohesion: 0.40
Nodes (5): uint32_t, PcSpan, end, pc, start

### Community 207 - "loadedImageProbes"
Cohesion: 0.16
Nodes (16): Files, Interfaces, P10-05: ao_debug_* ABI, コミット文, テスト, 完了条件, 手順, 目的 (+8 more)

### Community 208 - "ParseResult"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 209 - "image_save_load_test.cpp"
Cohesion: 0.15
Nodes (16): algorithm, Bootstrap, format, gc, iterator, lookup, MethodDictionary, resource (+8 more)

### Community 210 - "TEST"
Cohesion: 0.50
Nodes (4): TEST(), CompilerSmoke, VersionIsNonEmpty, VersionIsReleaseOneZeroZero

### Community 211 - "IgnoreFileSizeSignal"
Cohesion: 0.50
Nodes (3): IgnoreFileSizeSignal, old_, saved_

### Community 212 - "NameBag"
Cohesion: 0.67
Nodes (3): NameBag, heap, names

### Community 213 - "NativeFrame"
Cohesion: 0.50
Nodes (3): NativeFrame, argc, slots

## Knowledge Gaps
- **1003 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+998 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2268 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **20 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `unwinding`, `Heap.cpp`, `Roots`, `Float.cpp`, `String.cpp`, `applyChunks`, `指摘`, `Heap`, `Scheduler::terminate`, `Geometry.cpp`, `TEST`, `TEST`, `TEST`, `P3 — Native Dispatch`, `TEST`, `TEST`, `handlescope`, `string`, `P2-02 class object skeletons`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `ClassPool.cpp`, `TEST`, `BlockContext.cpp`, `TEST`, `SmallInteger.cpp`, `Session.cpp`, `CompileEnv`, `WellKnown.cpp`, `TEST`, `string`, `.isHeap`, `.false_`, `native_send_test.cpp`, `abi.cpp`, `TEST`, `Bootstrap.cpp`, `Stream.cpp`, `Session`, `.isTrue`, `HashedCollection.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `takeAbortReason`, `TEST`, `Gc`, `Boolean.cpp`, `TEST`, `size`, `ImageSurgery`, `Interpreter.cpp`, `ImageHeader`, `Array.cpp`, `BlockProbe`, `allocateRetry`, `vector`, `main`, `NativeMethod.cpp`, `path`, `refreshStackLimit`, `Stack`, `Analysis`, `applyMethod`, `TEST`, `Scheduler::Record`, `Process.cpp`, `TEST`, `TEST`, `TEST`, `Roots.cpp`, `GarbageFirstBoot`, `FiberStackBounds`, `指摘`, `Boot`, `.nil`, `ObjectHeader`, `.isSmallInteger`, `abortingNew`, `intern`, `Character.cpp`, `P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）`, `InlineCache`, `abortingSubclass`, `imageRegistryStubA`, `Range`, `answerOne`, `FrameBlock`, `.specialSelector`, `RootedArray`, `TEST`, `loadedImageProbes`, `image_save_load_test.cpp`, `NativeFrame`?**
  _High betweenness centrality (0.330) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `RealScope`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `main`, `uint64_t`, `Oop`, `Emitter`, `NativeMethod.cpp`, `Roots`, `Codegen.cpp`, `path`, `refreshStackLimit`, `Stack`, `Analysis`, `ChunkAction`, `applyChunks`, `B11 App とビルドの残り（07 Medium ×3、Low ×6、テストの穴）`, `Scheduler`, `fiber_test.cpp`, `VendorClassFile`, `ChunkParser.cpp`, `string`, `handlescope`, `Boot`, `ImageSave.cpp`, `Ast`, `TEST`, `ClassPool.cpp`, `P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）`, `Parser`, `Parsed`, `MethodImage`, `Session.cpp`, `TEST`, `CompileEnv`, `TEST`, `string`, `image_save_load_test.cpp`, `NameBag`, `.false_`, `TEST`, `Bootstrap.cpp`, `Session`, `ImageLoad.cpp`, `ImageFormat`, `Gc`, `TEST`, `ImageSurgery`?**
  _High betweenness centrality (0.150) - this node is a cross-community bridge._
- **Why does `P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）` connect `P10 事後デバッガ Implementation Plan（P11 ライブデバッガの設計を含む）` to `Session`, `handlescope`, `docs/README.md`, `Roots`, `Analysis`, `.isHeap`, `アプリ`, `WorkspaceWindow`?**
  _High betweenness centrality (0.061) - this node is a cross-community bridge._
- **Are the 16 inferred relationships involving `Oop` (e.g. with `Graphify / Serena` and `現状`) actually correct?**
  _`Oop` has 16 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _1003 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013157894736842105 - nodes in this community are weakly interconnected._
- **Should `RealScope` be split into smaller, more focused modules?**
  _Cohesion score 0.07308377896613191 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-25)

## Corpus Check
- 254 files · ~204,758 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 79 file(s) not represented in the graph (top: .st 64, (none) 13, .toml 1)

## Summary
- 4483 nodes · 12147 edges · 186 communities (167 shown, 19 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1683 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `b0d46f6f`
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
- Stream.cpp
- putNative
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- Collection.cpp
- VirtualRegion.cpp
- ao_accept_method
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
- classRows
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
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- SmallInteger.cpp
- Parser
- send
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
- ao_accept_class
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
- Character.cpp
- BrowserModel
- TEST_F
- .false_
- TEST
- TEST
- abi.cpp
- TEST
- Bootstrap.cpp
- evalBody
- build.sh
- test.sh
- SPEC.md
- Session
- .isTrue
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- docs/README.md
- Claude レビュー指摘の修正計画（docs/claude-review）
- ao_eval
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
- Claude レビュー指摘の修正計画（docs/claude-review）
- NativeMethod.cpp
- .fromSmallInteger
- TEST
- Bootstrap
- Reentry
- ImageSurgery
- Interpreter.cpp
- 横断テーマ
- ImageHeader
- performSend
- TEST
- Array.cpp
- string
- P9-02: Browser accept
- TEST
- P9-04: v1 ゴールデン受け入れ
- .isHeap
- 指摘
- package-app.sh
- ObjectHeader
- expectSaveRefusedAfter
- specialIndex
- GarbageFirstBoot
- applyMethod
- TEST
- Roots.cpp
- codegen
- .isSmallInteger
- intern
- CompileEnv
- TEST
- TEST
- uint64_t
- RootedArray
- BlockProbe
- P4-03 Magnitude / SmallInteger / Character
- interpreter
- abortingSubclass
- send2
- uint32_t
- WellKnown::checkSmallIntegerFastPath
- Gc
- TEST
- README.md
- P4 — Kernel Native Implementation
- Session.cpp
- FileSizeLimit
- imageRegistryStubA
- TEST
- cli_test.sh
- vector
- Frame
- TestDir
- answerOne
- Image::load
- abortingNew
- uint32_t
- ListedMethod
- B10 協調スケジューラ（02 High）
- boxLiteral
- consumeNonlocal
- TEST
- Format.hpp
- TEST
- ActiveGuard
- NameBag
- DepthGuard

## God Nodes (most connected - your core abstractions)
1. `Oop` - 787 edges
2. `Heap` - 231 edges
3. `WellKnown` - 167 edges
4. `vector` - 151 edges
5. `Ast` - 103 edges
6. `Roots` - 97 edges
7. `TEST()` - 94 edges
8. `TEST()` - 93 edges
9. `TEST()` - 90 edges
10. `BrowserWindow` - 86 edges

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

## Communities (186 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): array, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (58): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+50 more)

### Community 2 - "Dictionary.cpp"
Cohesion: 0.13
Nodes (54): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+46 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (69): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+61 more)

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
Cohesion: 0.11
Nodes (58): [High] printOn: が新しい printString を使わない, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, ao_Object_basicAt_() (+50 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.12
Nodes (52): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset() (+44 more)

### Community 9 - "putNative"
Cohesion: 0.09
Nodes (32): WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter(), installCollection(), WellKnown (+24 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (42): charconv, allocateTenured, growOld, initObject, objectBytes, oldUsed, align8(), byte (+34 more)

### Community 12 - "TEST"
Cohesion: 0.03
Nodes (69): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail (+61 more)

### Community 13 - "Literal"
Cohesion: 0.09
Nodes (23): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+15 more)

### Community 14 - "Float.cpp"
Cohesion: 0.08
Nodes (65): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+57 more)

### Community 15 - "Collection.cpp"
Cohesion: 0.20
Nodes (27): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill(), ao_Collection_includes_() (+19 more)

### Community 16 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 17 - "ao_accept_method"
Cohesion: 0.06
Nodes (30): 01 オブジェクトメモリ・GC・ブートストラップ, テストの穴, 概要, 02 インタプリタ・送信・コンテキスト・プロセス, [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る） (+22 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (65): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, load, Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_ (+57 more)

### Community 20 - "Roots"
Cohesion: 0.04
Nodes (52): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*) (+44 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.25
Nodes (28): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+20 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (48): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, string (+40 more)

### Community 25 - "uint64_t"
Cohesion: 0.30
Nodes (12): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+4 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (58): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+50 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (35): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), installErrorField() (+27 more)

### Community 28 - "classRows"
Cohesion: 0.28
Nodes (19): browserClassAt(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource(), browserSubclassAt() (+11 more)

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
Cohesion: 0.19
Nodes (14): dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests(), string (+6 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.08
Nodes (27): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+19 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.11
Nodes (34): ao_abi, chrono, Chunk, cmath, compile, CompiledMethod, compiler, context (+26 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.14
Nodes (28): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+20 more)

### Community 37 - "string"
Cohesion: 0.08
Nodes (13): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+5 more)

### Community 38 - "TEST_F"
Cohesion: 0.06
Nodes (37): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+29 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.11
Nodes (22): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, Int32 (+14 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.10
Nodes (24): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+16 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.07
Nodes (56): cerrno, climits, csignal, fcntl, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle (+48 more)

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
Cohesion: 0.14
Nodes (15): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+7 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (37): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+29 more)

### Community 49 - "TEST"
Cohesion: 0.08
Nodes (25): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+17 more)

### Community 50 - ".nil"
Cohesion: 0.24
Nodes (28): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+20 more)

### Community 52 - "TEST"
Cohesion: 0.08
Nodes (26): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+18 more)

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
Cohesion: 0.25
Nodes (27): ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual(), ao_Integer_greaterThan() (+19 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (24): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+16 more)

### Community 58 - "send"
Cohesion: 0.23
Nodes (22): toDoSendingLessOrEqual(), abortDoesNotUnderstand(), abortEvaluation(), abortFailedSend(), abortWithSelector(), allocateMessage(), cacheIndex(), callBlock() (+14 more)

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
Cohesion: 0.21
Nodes (25): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_hash(), ao_String_printString(), ao_String_size(), ao_Symbol_asString() (+17 more)

### Community 64 - "Compile.cpp"
Cohesion: 0.14
Nodes (40): cctype, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, isClassShaped(), superclassOf(), acceptClassDef(), acceptClassSource(), acceptMethodSource(), anyMethodIn() (+32 more)

### Community 65 - "allocateRetry"
Cohesion: 0.06
Nodes (50): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 指摘 (+42 more)

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
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.10
Nodes (18): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+10 more)

### Community 72 - "ao_accept_class"
Cohesion: 0.08
Nodes (31): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, 05 コンパイラ（スキャナ・パーサ・コード生成・チャンク）, [High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Low] カスケードの扱いが不完全, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, [Low] 宣言の検証がない（引数への代入・擬変数名・重複名） (+23 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (35): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+27 more)

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

### Community 80 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

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
Cohesion: 0.19
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "TEST_F"
Cohesion: 0.05
Nodes (37): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+29 more)

### Community 86 - ".false_"
Cohesion: 0.19
Nodes (27): boolean(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (76): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+68 more)

### Community 88 - "TEST"
Cohesion: 0.07
Nodes (47): BootstrapInstallsObjectIdentityEquals, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend (+39 more)

### Community 89 - "abi.cpp"
Cohesion: 0.12
Nodes (28): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at() (+20 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (73): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+65 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (23): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+15 more)

### Community 92 - "evalBody"
Cohesion: 0.16
Nodes (20): abortReasonText(), clearUnwinding(), string, blankOut(), AoInspectFn, evalBody(), knownGlobals, sessionEval() (+12 more)

### Community 95 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 96 - "Session"
Cohesion: 0.10
Nodes (21): browserClassCount(), CallContext, string, uint64_t, unique_ptr, WellKnown, methodSourceRootSlots(), moveMethodSource() (+13 more)

### Community 97 - ".isTrue"
Cohesion: 0.35
Nodes (11): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext (+3 more)

### Community 103 - "docs/README.md"
Cohesion: 0.20
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.10
Nodes (19): [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B6 イメージとセッションの堅牢化, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+11 more)

### Community 105 - "ao_eval"
Cohesion: 0.09
Nodes (30): AoSpan, end, message, start, B2 `to:do:` bench, P4 microbench, P6 interpreter bench, 07 Swift アプリ・ビルド・スクリプト (+22 more)

### Community 106 - "ao_image_load"
Cohesion: 0.08
Nodes (43): [High] Save / Open Image の失敗がユーザーに見えない, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる, [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free (+35 more)

### Community 111 - "AoApp"
Cohesion: 0.10
Nodes (17): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+9 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - ".publish"
Cohesion: 0.13
Nodes (9): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value, B5 Browser と Workspace のデータ消失（テーマ 4） (+1 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.15
Nodes (35): ifstream, acceptWord(), atOffset(), checkFile(), byte, size_t, string, T (+27 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall, KernelScan (+12 more)

### Community 117 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.11
Nodes (17): B0 準備（テスト基盤）, B11 App とビルドの残り, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC を先に直す (+9 more)

### Community 119 - "NativeMethod.cpp"
Cohesion: 0.11
Nodes (28): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI, Task 6: フェーズを閉じる (+20 more)

### Community 120 - ".fromSmallInteger"
Cohesion: 0.19
Nodes (13): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+5 more)

### Community 121 - "TEST"
Cohesion: 0.12
Nodes (15): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+7 more)

### Community 122 - "Bootstrap"
Cohesion: 0.22
Nodes (9): algorithm, Bootstrap, classpool, format, RootedArray::RootedArray(), CallContext, uint32_t, stubA() (+1 more)

### Community 123 - "Reentry"
Cohesion: 0.18
Nodes (11): path, string, expectAllRefused(), Reentry, codes, entered, evalOut, image (+3 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.25
Nodes (10): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, methodDictKey() (+2 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.20
Nodes (18): pthread, byteCount(), contextAlive(), ContextExitGuard, CallContext, deque, int16_t, uint8_t (+10 more)

### Community 126 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 127 - "ImageHeader"
Cohesion: 0.10
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 128 - "performSend"
Cohesion: 0.22
Nodes (8): answerWithoutSend(), int64_t, WellKnown, OperandStack, roots, slots, performSend(), smallIntegerAnswer()

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.22
Nodes (20): ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+12 more)

### Community 131 - "string"
Cohesion: 0.26
Nodes (17): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, classVarList() (+9 more)

### Community 132 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 133 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 134 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 135 - ".isHeap"
Cohesion: 0.21
Nodes (12): clearWeakAfterOldMark, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::collectOld(), Gc::copy(), Gc::scavengeFromRoots() (+4 more)

### Community 136 - "指摘"
Cohesion: 0.14
Nodes (13): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅 (+5 more)

### Community 138 - "ObjectHeader"
Cohesion: 0.12
Nodes (16): CLI, extra 節, globals 節, OOP エンコード, save, well-known 節, ファイル配置（すべて little-endian）, 仕様 (+8 more)

### Community 139 - "expectSaveRefusedAfter"
Cohesion: 0.17
Nodes (13): path, string, uint32_t, expectRefused(), expectSaveRefusedAfter(), fileNames(), findBytesOfSize(), freshDir() (+5 more)

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 142 - "applyMethod"
Cohesion: 0.17
Nodes (12): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`） (+4 more)

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.14
Nodes (16): iterator, walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock() (+8 more)

### Community 145 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 146 - ".isSmallInteger"
Cohesion: 0.26
Nodes (18): CompiledMethod accessors (P5), int64_t, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive() (+10 more)

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "CompileEnv"
Cohesion: 0.12
Nodes (21): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+13 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 152 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 153 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 154 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 156 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 157 - "send2"
Cohesion: 0.11
Nodes (20): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+12 more)

### Community 158 - "uint32_t"
Cohesion: 0.29
Nodes (7): uint32_t, unique_ptr, remoteSlot(), Temps, n, roots, slots

### Community 160 - "Gc"
Cohesion: 0.06
Nodes (44): BlockContextKeepsHomeAndCopied, CleanupFailureReleasesItsReasonHandle, ContextGc, cstdlib, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost (+36 more)

### Community 161 - "TEST"
Cohesion: 0.29
Nodes (7): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionTruncationIsRangeError

### Community 162 - "README.md"
Cohesion: 0.07
Nodes (26): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+18 more)

### Community 163 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 164 - "Session.cpp"
Cohesion: 0.20
Nodes (17): Image, check, load, save, clearMethodSources(), HostOopHook, ensureKernelNatives(), installEmptyCache() (+9 more)

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

### Community 169 - "vector"
Cohesion: 0.12
Nodes (33): CompileError, message, span, string, vector, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価 (+25 more)

### Community 170 - "Frame"
Cohesion: 0.22
Nodes (9): FieldRoots, frame, Frame, context, isBlock, method, pc, receiver (+1 more)

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 172 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 173 - "Image::load"
Cohesion: 0.33
Nodes (9): bindAll(), checkGlobals(), string_view, WellKnown, fileOop(), Image::load(), recordKind(), relocate() (+1 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "uint32_t"
Cohesion: 0.48
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 176 - "ListedMethod"
Cohesion: 0.22
Nodes (9): ListedMethod, method, native, selector, MethodSource, method, text, protocolsOf() (+1 more)

### Community 177 - "B10 協調スケジューラ（02 High）"
Cohesion: 0.22
Nodes (8): ファイルごとの変更, B10 協調スケジューラ（02 High）, SPEC §3.4 に書く意味論, ファイバ, ファイルごとの変更, プロセスごとの状態, リスク, 手順（各段を緑にしてから次へ進む）

### Community 178 - "boxLiteral"
Cohesion: 0.32
Nodes (8): [High] 2^63 以上の整数リテラルが黙って 0 になる, boxBytes(), boxedOk(), boxLiteral(), boxMethodImage(), MethodImage, uint32_t, uint8_t

### Community 179 - "consumeNonlocal"
Cohesion: 0.39
Nodes (8): branchTruth(), clearNonlocal(), consumeNonlocal(), hit(), Leave, leave, value, miss()

### Community 180 - "TEST"
Cohesion: 0.40
Nodes (5): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, TEST(), VersionOneIsRefusedWithReason

### Community 181 - "Format.hpp"
Cohesion: 0.48
Nodes (6): bits(), int64_t, isBytes(), isIndexable(), isPointers(), make()

### Community 182 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 183 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

### Community 184 - "NameBag"
Cohesion: 0.67
Nodes (3): NameBag, heap, names

## Knowledge Gaps
- **831 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+826 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1869 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Dictionary.cpp`, `LargeInteger.cpp`, `TEST`, `Stream.cpp`, `putNative`, `Heap.cpp`, `TEST`, `Float.cpp`, `Collection.cpp`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `classRows`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `send`, `String.cpp`, `Compile.cpp`, `allocateRetry`, `ao_accept_class`, `WellKnown.cpp`, `TEST`, `Boolean.cpp`, `Character.cpp`, `.false_`, `TEST`, `abi.cpp`, `Bootstrap.cpp`, `evalBody`, `Session`, `.isTrue`, `native_method_test.cpp`, `TEST`, `NativeMethod.cpp`, `.fromSmallInteger`, `Bootstrap`, `ImageSurgery`, `Interpreter.cpp`, `ImageHeader`, `performSend`, `Array.cpp`, `string`, `.isHeap`, `ObjectHeader`, `expectSaveRefusedAfter`, `GarbageFirstBoot`, `applyMethod`, `Roots.cpp`, `.isSmallInteger`, `intern`, `CompileEnv`, `TEST`, `RootedArray`, `BlockProbe`, `abortingSubclass`, `send2`, `uint32_t`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `README.md`, `imageRegistryStubA`, `vector`, `Frame`, `answerOne`, `Image::load`, `abortingNew`, `uint32_t`, `ListedMethod`, `boxLiteral`, `consumeNonlocal`, `Format.hpp`, `ActiveGuard`?**
  _High betweenness centrality (0.361) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `LargeInteger.cpp`, `string`, `Scanner.cpp`, `VendorExtract.cpp`, `Oop`, `Emitter`, `expectSaveRefusedAfter`, `Literal`, `ao_accept_method`, `ChunkAction`, `Heap`, `CompileEnv`, `Roots`, `uint64_t`, `classRows`, `ao_main.cpp`, `Gc`, `image_save_load_test.cpp`, `ChunkParser.cpp`, `string`, `Session.cpp`, `ImageSave.cpp`, `Image::load`, `Ast`, `ListedMethod`, `NameBag`, `Parser`, `Compile.cpp`, `allocateRetry`, `TEST`, `ao_accept_class`, `TEST`, `.false_`, `TEST`, `TEST`, `Bootstrap.cpp`, `evalBody`, `Session`, `ImageLoad.cpp`, `NativeMethod.cpp`, `Bootstrap`, `Reentry`, `ImageSurgery`?**
  _High betweenness centrality (0.118) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `string`, `Oop`, `Stream.cpp`, `WellKnown.cpp`, `Heap`, `Roots`, `intern`, `.false_`, `uint64_t`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.073) - this node is a cross-community bridge._
- **Are the 13 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 13 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _831 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013245033112582781 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.04890453834115806 - nodes in this community are weakly interconnected._
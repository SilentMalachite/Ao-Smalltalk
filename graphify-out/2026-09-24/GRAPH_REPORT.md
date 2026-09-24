# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 253 files · ~192,400 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 4339 nodes · 11611 edges · 170 communities (152 shown, 18 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1604 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ced5d579`
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
- Object.cpp
- Oop
- putNative
- Emitter
- Heap.cpp
- TEST
- Literal
- Session.hpp
- send
- VirtualRegion.cpp
- ao_eval
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
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
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- SmallInteger.cpp
- Parser
- clearUnwinding
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- Compile.cpp
- allocateRetry
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- TEST
- vector
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
- Codegen.cpp
- .true_
- TEST
- TEST
- abi.cpp
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
- docs/README.md
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- ao_image_load
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- ClassDef
- ImageLoad.cpp
- .build
- TEST
- DefinitionScanner
- Claude レビュー指摘の修正計画（docs/claude-review）
- RootedArray
- uint32_t
- AppKit
- .false_
- .isHeap
- ImageSurgery
- Interpreter.cpp
- 横断テーマ
- ImageFormat
- InspectorWindow
- TEST
- Array.cpp
- P4-03 Magnitude / SmallInteger / Character
- P9-02: Browser accept
- send2
- P9-04: v1 ゴールデン受け入れ
- TEST
- 指摘
- package-app.sh
- ObjectHeader
- path
- Bytecode.hpp
- Bootstrap
- Vendor.hpp
- TEST
- Roots.cpp
- codegen
- CompiledMethodNatives.cpp
- intern
- CompileEnv
- TEST
- isClassShaped
- uint64_t
- ClassMethodCache
- BlockProbe
- P4 — Kernel Native Implementation
- interpreter
- abortingSubclass
- TEST
- 指摘
- WellKnown::checkSmallIntegerFastPath
- Gc
- TEST
- Ao
- 貢献
- TEST
- FileSizeLimit
- imageRegistryStubA
- cli_test.sh
- TestDir
- abortingNew

## God Nodes (most connected - your core abstractions)
1. `Oop` - 755 edges
2. `Heap` - 226 edges
3. `WellKnown` - 167 edges
4. `vector` - 147 edges
5. `Ast` - 103 edges
6. `Roots` - 97 edges
7. `TEST()` - 93 edges
8. `TEST()` - 89 edges
9. `BrowserWindow` - 86 edges
10. `TEST()` - 76 edges

## Surprising Connections (you probably didn't know these)
- `[Low] Inspector ウィンドウが閉じても解放されず、増え続ける` --references--> `InspectorWindow`  [INFERRED]
  docs/claude-review/07-app-build.md → app/Ao/InspectorWindow.swift
- `[Low] `AoSpan` の区間を捨てているので、エラー位置が分からない` --references--> `AoSpan`  [INFERRED]
  docs/claude-review/07-app-build.md → bridge/ao_abi.h
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp

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

## Communities (170 total, 18 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): array, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "RealScope"
Cohesion: 0.07
Nodes (34): Analyzer, Capture, owner, var, Inline, uint32_t, LexScope, parent (+26 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.11
Nodes (62): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+54 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (60): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+52 more)

### Community 4 - "TEST"
Cohesion: 0.06
Nodes (33): EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks, ImageSave, ImageSaveLoad, KernelMethodsStayNative (+25 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.06
Nodes (62): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary (+54 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.10
Nodes (51): allowIndex(), containsHostWord(), size_t, string, string_view, doubleBangs(), dropCycles(), dropMissingSupers() (+43 more)

### Community 7 - "Object.cpp"
Cohesion: 0.12
Nodes (52): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 8 - "Oop"
Cohesion: 0.09
Nodes (69): [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 1: 形式ヘッダと即値ビット, Task 2: Transcript のクラス側転送とフック, bits(), isBytes(), isIndexable(), isPointers() (+61 more)

### Community 9 - "putNative"
Cohesion: 0.08
Nodes (57): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean() (+49 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (12): int16_t, LitKind, Op, size_t, string_view, uint16_t, uint8_t, Emitter (+4 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (44): charconv, allocateTenured, fitsOld, growOld, header, initObject, largeObjectBytes, objectBytes (+36 more)

### Community 12 - "TEST"
Cohesion: 0.05
Nodes (44): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+36 more)

### Community 13 - "Literal"
Cohesion: 0.08
Nodes (29): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+21 more)

### Community 14 - "Session.hpp"
Cohesion: 0.25
Nodes (5): runtime, ao executable, ao_runtime library, ao_runtime_tests, boot()

### Community 15 - "send"
Cohesion: 0.10
Nodes (59): InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan() (+51 more)

### Community 16 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 17 - "ao_eval"
Cohesion: 0.06
Nodes (43): AoSpan, end, message, start, B2 `to:do:` bench, 00 直近差分（origin/main..HEAD の 6 コミット）, 概要, 01 オブジェクトメモリ・GC・ブートストラップ (+35 more)

### Community 18 - "ChunkAction"
Cohesion: 0.09
Nodes (23): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+15 more)

### Community 19 - "Heap"
Cohesion: 0.05
Nodes (51): Heap, checkNotPoisoned, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_ (+43 more)

### Community 20 - "Roots"
Cohesion: 0.06
Nodes (40): Root, slot, FrameBlock, capacity, slots, used, size_t, StackWalker (+32 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.14
Nodes (38): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point と Rectangle のネイティブがサブクラスを扱えない, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+30 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (28): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+20 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (46): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, string (+38 more)

### Community 25 - "TEST"
Cohesion: 0.09
Nodes (22): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+14 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (56): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+48 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (29): aoWorkspaceInspectHook(), failureText(), installErrorField(), sendToKeyWorkspace(), Bool, CChar, Int, Int32 (+21 more)

### Community 28 - "Session.cpp"
Cohesion: 0.05
Nodes (95): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む） (+87 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (17): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView (+9 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "NativeMethod.cpp"
Cohesion: 0.13
Nodes (27): Global Constraints, P7 .aoimage Implementation Plan, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI, Task 6: フェーズを閉じる, リスク (+19 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.08
Nodes (27): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+19 more)

### Community 35 - "string"
Cohesion: 0.12
Nodes (29): ao_abi, chrono, Chunk, compile, CompiledMethod, compiler, string, context (+21 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "cstdint"
Cohesion: 0.06
Nodes (26): algorithm, cassert, climits, string_view, cstddef, cstdint, dyld, memory (+18 more)

### Community 38 - "TEST_F"
Cohesion: 0.05
Nodes (46): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, ClassDefinedAfterBindingWins, DeadHomeBlockAbortsWithReason, EvalTranscriptShowThenCr, EvalWithoutOutBufferRefusesBeforeEvaluating (+38 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.12
Nodes (20): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, AcceptTests (+12 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.09
Nodes (42): atomic, cerrno, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord() (+34 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.08
Nodes (24): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+16 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.08
Nodes (24): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+16 more)

### Community 49 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 50 - ".nil"
Cohesion: 0.14
Nodes (39): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, int64_t, ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume() (+31 more)

### Community 52 - "TEST"
Cohesion: 0.08
Nodes (26): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+18 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.22
Nodes (23): safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_() (+15 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (23): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+15 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewAbortsWithShouldNotImplement, MetaclassNewIsForbidden (+5 more)

### Community 56 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (25): Native selector mangling, マングル表, 規則, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+17 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (24): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+16 more)

### Community 58 - "clearUnwinding"
Cohesion: 0.27
Nodes (13): abortReasonText(), clearUnwinding(), string, CallContext, path, Root, string, string_view (+5 more)

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
Cohesion: 0.21
Nodes (24): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_printString(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+16 more)

### Community 64 - "Compile.cpp"
Cohesion: 0.12
Nodes (49): cctype, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, acceptClassDef(), acceptClassSource(), acceptMethodSource(), anyMethodIn(), assignError() (+41 more)

### Community 65 - "allocateRetry"
Cohesion: 0.04
Nodes (61): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, 指摘 (+53 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".init"
Cohesion: 0.13
Nodes (11): Any, Int, NSRect, NSTableView, NSTextField, NSTextView, NSView, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+3 more)

### Community 70 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "vector"
Cohesion: 0.10
Nodes (40): CompileError, message, span, string, vector, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Global Constraints (+32 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (30): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+22 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, ImageRegistry, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating (+4 more)

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
Cohesion: 0.29
Nodes (6): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint64_t, TEST(), VersionOneIsRefusedWithReason

### Community 84 - "BrowserModel"
Cohesion: 0.11
Nodes (18): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, BrowserModelTests (+10 more)

### Community 85 - "Codegen.cpp"
Cohesion: 0.10
Nodes (28): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+20 more)

### Community 86 - ".true_"
Cohesion: 0.20
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (76): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+68 more)

### Community 88 - "TEST"
Cohesion: 0.07
Nodes (49): BootstrapInstallsObjectIdentityEquals, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend (+41 more)

### Community 89 - "abi.cpp"
Cohesion: 0.12
Nodes (27): Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at(), ao_browser_class_count() (+19 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (73): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+65 more)

### Community 91 - "ClassPool.cpp"
Cohesion: 0.21
Nodes (19): classpool, load, format, adopt(), bindingAt(), string, string_view, uint32_t (+11 more)

### Community 92 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 95 - "SPEC.md"
Cohesion: 0.07
Nodes (40): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+32 more)

### Community 96 - ".publish"
Cohesion: 0.13
Nodes (8): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value, [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る

### Community 97 - "ao_Magnitude_lessOrEqual"
Cohesion: 0.58
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 103 - "docs/README.md"
Cohesion: 0.16
Nodes (21): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.07
Nodes (26): [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い, [Low] Inspector ウィンドウが閉じても解放されず、増え続ける, [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる, [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される, [Low] Workspace の評価を中断できない（無限ループでアプリが固まる）, [Medium] 64 KiB を超える Print it / Inspect it は副作用だけ残して失敗する (+18 more)

### Community 105 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 106 - "ao_image_load"
Cohesion: 0.08
Nodes (48): [High] Save / Open Image の失敗がユーザーに見えない, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる, [Low] 評価中のフックから `ao_image_load` を呼ぶと use-after-free (+40 more)

### Community 111 - "AoApp"
Cohesion: 0.11
Nodes (16): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+8 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.15
Nodes (41): ifstream, readHeader, acceptWord(), bindAll(), checkGlobals(), classShaped(), byte, size_t (+33 more)

### Community 115 - ".build"
Cohesion: 0.20
Nodes (10): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, NSMenu (+2 more)

### Community 116 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall, KernelScan (+12 more)

### Community 117 - "DefinitionScanner"
Cohesion: 0.23
Nodes (10): DefinitionScanner, Bool, Int32, String, Token, keyword, other, spanMessage() (+2 more)

### Community 118 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.12
Nodes (16): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B5 Browser と Workspace のデータ消失（テーマ 4）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+8 more)

### Community 119 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 120 - "uint32_t"
Cohesion: 0.48
Nodes (7): answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 121 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 122 - ".false_"
Cohesion: 0.40
Nodes (9): boolean(), ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext (+1 more)

### Community 123 - ".isHeap"
Cohesion: 0.15
Nodes (15): Gc::collectOld(), Gc::copy(), Heap::allocateNoGc(), Heap::checkNotPoisoned(), Heap::header(), Heap::inNursery(), Heap::inOld(), string (+7 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.28
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (69): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, ファイルごとの変更, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, ファイルごとの変更, リスク (+61 more)

### Community 126 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 127 - "ImageFormat"
Cohesion: 0.12
Nodes (18): uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes, kImagePointerBits (+10 more)

### Community 128 - "InspectorWindow"
Cohesion: 0.33
Nodes (5): InspectorWindow, .text, NSTextView, NSWindow, String

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.31
Nodes (15): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+7 more)

### Community 131 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 132 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 133 - "send2"
Cohesion: 0.11
Nodes (19): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+11 more)

### Community 134 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 135 - "TEST"
Cohesion: 0.06
Nodes (40): AllocateNoGcSpillsToOld, ByteObjectPayload, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, ExhaustionReturnsEmpty, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest (+32 more)

### Community 136 - "指摘"
Cohesion: 0.17
Nodes (12): [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅 (+4 more)

### Community 138 - "ObjectHeader"
Cohesion: 0.13
Nodes (14): CLI, extra 節, globals 節, OOP エンコード, save, well-known 節, ファイル配置（すべて little-endian）, 仕様 (+6 more)

### Community 139 - "path"
Cohesion: 0.15
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "Bytecode.hpp"
Cohesion: 0.24
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), TEST(), NewOpsAppendedAfterPrimitive (+2 more)

### Community 141 - "Bootstrap"
Cohesion: 0.19
Nodes (13): Bootstrap, cstdio, Lookup, MethodDictionary, answerOne(), answerTwo(), CallContext, uint32_t (+5 more)

### Community 142 - "Vendor.hpp"
Cohesion: 0.18
Nodes (12): Task 4: LOAD_ORDER とホストパッチ, string, string_view, isVendorStub(), VendorClassFile, chunkText, className, superName (+4 more)

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 146 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "CompileEnv"
Cohesion: 0.08
Nodes (28): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+20 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "isClassShaped"
Cohesion: 0.24
Nodes (8): uint32_t, isClassShaped(), superclassOf(), SuperclassWalk, depth_, hasSubclass(), liveClasses(), methodSourceRootSlots()

### Community 152 - "ClassMethodCache"
Cohesion: 0.17
Nodes (12): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, probe, Entry (+4 more)

### Community 153 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 156 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 157 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 158 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 160 - "Gc"
Cohesion: 0.06
Nodes (41): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, optional, Gc, clearWeakAfterNursery (+33 more)

### Community 161 - "TEST"
Cohesion: 0.29
Nodes (7): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, TEST(), Smoke, VersionStringIsNonEmpty, VersionTruncationIsRangeError

### Community 162 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 163 - "貢献"
Cohesion: 0.33
Nodes (6): コミット, ビルド, ライセンス, 曲げない規則, 現行フェーズ, 貢献

### Community 164 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Knowledge Gaps
- **819 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+814 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1806 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Object.cpp`, `putNative`, `Heap.cpp`, `Literal`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `TEST`, `Session.cpp`, `NativeMethod.cpp`, `TEST`, `TEST`, `string`, `cstdint`, `ImageSave.cpp`, `Loaded`, `CallContext`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `clearUnwinding`, `String.cpp`, `Compile.cpp`, `allocateRetry`, `DiskHeader`, `vector`, `WellKnown.cpp`, `TEST`, `Boolean.cpp`, `TEST`, `.true_`, `TEST`, `abi.cpp`, `ClassPool.cpp`, `GarbageFirstBoot`, `SPEC.md`, `ao_Magnitude_lessOrEqual`, `native_method_test.cpp`, `ClassDef`, `ImageLoad.cpp`, `TEST`, `RootedArray`, `uint32_t`, `.false_`, `.isHeap`, `Interpreter.cpp`, `Array.cpp`, `send2`, `TEST`, `ObjectHeader`, `path`, `Bootstrap`, `Roots.cpp`, `CompiledMethodNatives.cpp`, `intern`, `CompileEnv`, `isClassShaped`, `ClassMethodCache`, `BlockProbe`, `abortingSubclass`, `TEST`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `Ao`, `imageRegistryStubA`, `abortingNew`?**
  _High betweenness centrality (0.365) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `RealScope`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Object.cpp`, `Emitter`, `path`, `Literal`, `Vendor.hpp`, `Session.hpp`, `Bootstrap`, `ao_eval`, `ChunkAction`, `Heap`, `CompileEnv`, `Roots`, `isClassShaped`, `Session.cpp`, `Gc`, `NativeMethod.cpp`, `string`, `ChunkParser.cpp`, `cstdint`, `TEST_F`, `ImageSave.cpp`, `Ast`, `Parser`, `clearUnwinding`, `Compile.cpp`, `allocateRetry`, `TEST`, `Codegen.cpp`, `.true_`, `TEST`, `TEST`, `ClassPool.cpp`, `ImageLoad.cpp`, `ImageSurgery`?**
  _High betweenness centrality (0.143) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `cstdint`, `Oop`, `WellKnown.cpp`, `Heap`, `Roots`, `intern`, `.true_`, `uint64_t`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.093) - this node is a cross-community bridge._
- **Are the 14 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 14 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _819 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013071895424836602 - nodes in this community are weakly interconnected._
- **Should `RealScope` be split into smaller, more focused modules?**
  _Cohesion score 0.07439613526570048 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 253 files · ~177,905 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 4172 nodes · 11175 edges · 178 communities (155 shown, 23 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1577 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `9970ac3b`
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
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- ao_main.cpp
- TEST
- SPEC.md
- ao_eval
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- docs/README.md
- TEST
- TEST
- Boot
- WorkspaceWindow
- Session.cpp
- TranscriptWindow
- native_send_test.cpp
- Bytecode interpreter
- NativeMethod.cpp
- TEST
- TEST
- string
- ChunkParser.cpp
- P3 — Native Dispatch
- vector
- TEST
- BrowserWindow
- P2-02 class object skeletons
- P6b — vendor file-in
- ImageSave.cpp
- P9 — Integration
- TEST
- Ast
- P0 phase doc
- CallContext
- ao_runtime_boot
- Process.cpp
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- putNative
- Parser
- send
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- reshapeClass
- 指摘
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- image_save_load_test.cpp
- Compile.cpp
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- Token
- SmallInteger.cpp
- TEST
- ao image save
- ImageFormat
- BrowserModel
- abi.cpp
- .false_
- TEST
- TEST
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST
- forceSlide
- ao_accept_class
- build.sh
- test.sh
- README.md
- .publish
- ao_Magnitude_lessOrEqual
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- .nil
- Claude レビュー指摘の修正計画（docs/claude-review）
- CompiledMethodNatives.cpp
- TEST_F
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- P6b vendor file-in Implementation Plan
- ImageLoad.cpp
- .build
- CallContext
- DefinitionScanner
- P7 .aoimage Implementation Plan
- Interpreter.cpp
- performSend
- AppKit
- Character.cpp
- .isHeap
- 指摘
- Interpreter::run
- claude-review/README.md
- ImageHeader
- 仕様
- TEST
- Format.hpp
- P4 — Kernel Native Implementation
- NamedOop
- send2
- Ao
- TEST
- 指摘
- package-app.sh
- Array.cpp
- uint32_t
- Bytecode.hpp
- Frame
- P1-06: weak slot GC
- TEST
- Roots.cpp
- codegen
- BlockProbe
- intern
- 指摘
- TEST
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- uint64_t
- ao_image_load
- P1-07: immovable old objects
- P4-08 Point / Rectangle
- interpreter
- block_test.cpp
- P4-06 Stream / Transcript model
- findBytesOfSize
- WellKnown::checkSmallIntegerFastPath
- Gc
- P4-05 Dictionary / Set / OrderedCollection
- create
- Interpreter
- ActiveGuard
- runOnSmallStack
- imageRegistryStubA
- answerOne
- cli_test.sh
- Compiler.cpp
- NonlocalGuard
- TestDir
- TEST
- Root
- abortingNew
- method_dictionary_test.cpp
- expectSpecialSends
- oopWords

## God Nodes (most connected - your core abstractions)
1. `Oop` - 752 edges
2. `Heap` - 225 edges
3. `WellKnown` - 167 edges
4. `vector` - 138 edges
5. `Roots` - 97 edges
6. `Ast` - 93 edges
7. `TEST()` - 93 edges
8. `BrowserWindow` - 86 edges
9. `TEST()` - 74 edges
10. `Boot` - 74 edges

## Surprising Connections (you probably didn't know these)
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp
- `Status` --references--> `Oop`  [INFERRED]
  README.md → runtime/include/ao/Oop.hpp
- `[High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）` --references--> `boxLiteral()`  [INFERRED]
  docs/claude-review/03-kernel-numeric.md → runtime/src/Compile.cpp

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

## Communities (178 total, 23 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (147): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+139 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (49): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+41 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.09
Nodes (73): [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, make(), CallContext, uint8_t, createBlock() (+65 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "TEST"
Cohesion: 0.09
Nodes (23): FailedProbeKeepsCurrentSession, ImageSave, ImageSaveLoad, KernelMethodsStayNative, LoadSizesOldFromHeader, NurseryGcAfterLoad, OnePlusTwoMatches, RefusesExtraRecords (+15 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (47): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+39 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.07
Nodes (64): mman, allowIndex(), containsHostWord(), size_t, string, string_view, dropCycles(), dropMissingSupers() (+56 more)

### Community 7 - "Oop"
Cohesion: 0.11
Nodes (59): [High] printOn: が新しい printString を使わない, uint64_t, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_ (+51 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.12
Nodes (53): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+45 more)

### Community 9 - "Float.cpp"
Cohesion: 0.22
Nodes (26): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_printString() (+18 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (44): charconv, allocateTenured, fitsOld, growOld, header, initObject, largeObjectBytes, objectBytes (+36 more)

### Community 12 - "TEST"
Cohesion: 0.05
Nodes (47): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+39 more)

### Community 13 - "Literal"
Cohesion: 0.10
Nodes (22): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+14 more)

### Community 14 - "ao_main.cpp"
Cohesion: 0.10
Nodes (33): climits, cstdio, dyld, filesystem, fstream, runtime, ao executable, ao_runtime library (+25 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall, KernelScan (+12 more)

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "ao_eval"
Cohesion: 0.10
Nodes (25): AoSpan, end, message, start, B2 `to:do:` bench, P4 microbench, P6 interpreter bench, 07 Swift アプリ・ビルド・スクリプト (+17 more)

### Community 18 - "ChunkAction"
Cohesion: 0.10
Nodes (21): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+13 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (69): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, ファイル配置（すべて little-endian）, Heap, checkNotPoisoned, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_ (+61 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (49): cassert, memory, CallContext, uint32_t, unique_ptr, Root, slot, RootedArray (+41 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.14
Nodes (38): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point と Rectangle のネイティブがサブクラスを扱えない, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+30 more)

### Community 22 - "TEST"
Cohesion: 0.08
Nodes (26): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+18 more)

### Community 23 - "docs/README.md"
Cohesion: 0.12
Nodes (23): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P2 — Bootstrap, Blue Book 6–10 (+15 more)

### Community 24 - "TEST"
Cohesion: 0.09
Nodes (23): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, assemble() (+15 more)

### Community 25 - "TEST"
Cohesion: 0.06
Nodes (35): Bootstrap, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+27 more)

### Community 26 - "Boot"
Cohesion: 0.03
Nodes (90): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, chrono, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops (+82 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (36): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), installErrorField() (+28 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (89): Image, load, save, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt() (+81 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (17): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView (+9 more)

### Community 30 - "native_send_test.cpp"
Cohesion: 0.29
Nodes (9): CallContext, WellKnown, answerMessage(), CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "NativeMethod.cpp"
Cohesion: 0.33
Nodes (13): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+5 more)

### Community 33 - "TEST"
Cohesion: 0.09
Nodes (22): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, format, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf (+14 more)

### Community 34 - "TEST"
Cohesion: 0.08
Nodes (27): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+19 more)

### Community 35 - "string"
Cohesion: 0.10
Nodes (25): ao_abi, Chunk, compile, CompiledMethod, compiler, string, string_view, context (+17 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (26): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, Token, uint32_t, firstLine() (+18 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 38 - "vector"
Cohesion: 0.19
Nodes (22): vector, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, Task 3: rebind と上書き禁止, FileInError, error, file, method, string (+14 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (12): BlockWithArgs, Cascade, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma (+4 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.12
Nodes (20): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, AcceptTests (+12 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.12
Nodes (15): array, Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, Behavior, Bootstrap (+7 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.15
Nodes (27): encodeNonHeap, writeFiller, writeHeader, appendRaw(), appendRecord(), collectImageSlot(), byte, size_t (+19 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.06
Nodes (30): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+22 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.09
Nodes (30): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+22 more)

### Community 47 - "P0 phase doc"
Cohesion: 0.23
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 48 - "CallContext"
Cohesion: 0.08
Nodes (24): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+16 more)

### Community 49 - "ao_runtime_boot"
Cohesion: 0.07
Nodes (31): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, accept, C ABI, P8–P9 Remaining Implementation Plan (+23 more)

### Community 50 - "Process.cpp"
Cohesion: 0.20
Nodes (31): int64_t, ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend() (+23 more)

### Community 52 - "TEST"
Cohesion: 0.08
Nodes (26): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+18 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.22
Nodes (23): safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_() (+15 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewAbortsWithShouldNotImplement, MetaclassNewIsForbidden (+5 more)

### Community 56 - "putNative"
Cohesion: 0.09
Nodes (31): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+23 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (21): uint32_t, SourceSpan, end, start, deque, Kind, string, string_view (+13 more)

### Community 58 - "send"
Cohesion: 0.09
Nodes (62): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_() (+54 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.15
Nodes (13): EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RewritesHostSelectorAndDefersMissingSuper, string, string_view (+5 more)

### Community 61 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 62 - "String.cpp"
Cohesion: 0.08
Nodes (45): classpool, DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, natives, PartlyDeferredStillFails (+37 more)

### Community 64 - "reshapeClass"
Cohesion: 0.10
Nodes (31): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+23 more)

### Community 65 - "指摘"
Cohesion: 0.06
Nodes (36): 01 オブジェクトメモリ・GC・ブートストラップ, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない (+28 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.17
Nodes (12): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ChunksAfterSectionEndAreFlagged, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST() (+4 more)

### Community 69 - ".init"
Cohesion: 0.13
Nodes (11): Any, Int, NSRect, NSTableView, NSTextField, NSTextView, NSView, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+3 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "image_save_load_test.cpp"
Cohesion: 0.16
Nodes (17): size_t, string_view, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), Loaded, cache, ctx (+9 more)

### Community 72 - "Compile.cpp"
Cohesion: 0.18
Nodes (25): cctype, CompileError, message, span, string, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Task 5: `ao filein` と評価, acceptClassSource() (+17 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.08
Nodes (33): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+25 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.18
Nodes (11): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, ImageRegistry, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+3 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.06
Nodes (45): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), Inline (+37 more)

### Community 79 - "Token"
Cohesion: 0.20
Nodes (10): int64_t, string, Tok, Token, intValue, isFloat, kind, number (+2 more)

### Community 80 - "SmallInteger.cpp"
Cohesion: 0.12
Nodes (51): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+43 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "ImageFormat"
Cohesion: 0.14
Nodes (13): cstddef, HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes (+5 more)

### Community 84 - "BrowserModel"
Cohesion: 0.10
Nodes (21): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, BrowserModelTests (+13 more)

### Community 85 - "abi.cpp"
Cohesion: 0.13
Nodes (22): AoTranscriptFn, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Browser の読み取り, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, ao_browser_class_count(), ao_browser_protocol_at() (+14 more)

### Community 86 - ".false_"
Cohesion: 0.20
Nodes (26): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+18 more)

### Community 87 - "TEST"
Cohesion: 0.04
Nodes (64): AcceptAbi, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition, AcceptClassStopsAtRefusedDefinitionKeepingEarlierChunks, AcceptClassTakesChunkDefinitionAndMethods, AcceptMethodRefusesNonBehaviorName (+56 more)

### Community 88 - "TEST"
Cohesion: 0.06
Nodes (56): AllocateNoGcSpillsToOld, ByteObjectPayload, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, ExhaustionReturnsEmpty, HeapAlloc, IdentityEqualsAndClass (+48 more)

### Community 89 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.09
Nodes (22): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, B4 クラスのメタデータ（Kernel の形と名前）, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+14 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (71): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+63 more)

### Community 91 - "forceSlide"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, forceNursery(), forceOld(), forceSlide()

### Community 92 - "ao_accept_class"
Cohesion: 0.13
Nodes (17): 02 インタプリタ・送信・コンテキスト・プロセス, テストの穴, 概要, 03 数値・スカラー系 Kernel ネイティブ, テストの穴, 概要, 05 コンパイラ（スキャナ・パーサ・コード生成・チャンク）, テストの穴 (+9 more)

### Community 95 - "README.md"
Cohesion: 0.09
Nodes (19): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+11 more)

### Community 96 - ".publish"
Cohesion: 0.14
Nodes (8): sendToKeyBrowser(), MainActor, Notification, NSSegmentedControl, NSWindow, Void, value, [Medium] 新しいセレクタを Accept すると、ソース欄が別メソッドの本文に戻る

### Community 97 - "ao_Magnitude_lessOrEqual"
Cohesion: 0.58
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 103 - ".nil"
Cohesion: 0.15
Nodes (23): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+15 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.09
Nodes (22): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, B4 クラスのメタデータ（Kernel の形と名前）, B8 Kernel の意味論（数値とスカラー）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+14 more)

### Community 105 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 106 - "TEST_F"
Cohesion: 0.06
Nodes (30): ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, ClassDefinedAfterBindingWins, DeadHomeBlockAbortsWithReason, Task 11: Workspace の Do it / Print it / Inspect it, EvalTranscriptShowThenCr, EvalWithoutOutBufferRefusesBeforeEvaluating (+22 more)

### Community 111 - "AoApp"
Cohesion: 0.10
Nodes (18): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+10 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.10
Nodes (20): Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 4: LOAD_ORDER とホストパッチ, Task 6: フェーズ完了, 仕様 (+12 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.17
Nodes (33): readHeader, acceptWord(), bindAll(), checkGlobals(), byte, size_t, string, string_view (+25 more)

### Community 115 - ".build"
Cohesion: 0.23
Nodes (9): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, NSMenu (+1 more)

### Community 116 - "CallContext"
Cohesion: 0.21
Nodes (15): isClassShaped(), superclassOf(), anyMethodIn(), CallContext, pair, size_t, hasSubclass(), holdsBinding() (+7 more)

### Community 117 - "DefinitionScanner"
Cohesion: 0.25
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 118 - "P7 .aoimage Implementation Plan"
Cohesion: 0.13
Nodes (14): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 4: load と受け入れ評価, Task 5: CLI, Task 6: フェーズを閉じる (+6 more)

### Community 119 - "Interpreter.cpp"
Cohesion: 0.23
Nodes (13): boolean(), branchTruth(), clearNonlocal(), consumeNonlocal(), int64_t, DepthGuard, outermost, hit() (+5 more)

### Community 120 - "performSend"
Cohesion: 0.20
Nodes (7): answerWithoutSend(), deque, WellKnown, OperandStack, roots, slots, performSend()

### Community 121 - "AppKit"
Cohesion: 0.18
Nodes (5): Ao, AppKit, CAo, PackageDescription, XCTest

### Community 122 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 123 - ".isHeap"
Cohesion: 0.13
Nodes (17): Gc::collectOld(), Gc::copy(), Heap::allocateNoGc(), Heap::checkNotPoisoned(), Heap::header(), Heap::inNursery(), Heap::inOld(), string (+9 more)

### Community 124 - "指摘"
Cohesion: 0.25
Nodes (8): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する, 指摘

### Community 125 - "Interpreter::run"
Cohesion: 0.29
Nodes (15): byteCount(), contextAlive(), CallContext, int16_t, uint8_t, decodeHeader(), instSlot(), Interpreter::run() (+7 more)

### Community 126 - "claude-review/README.md"
Cohesion: 0.15
Nodes (11): 00 直近差分（origin/main..HEAD の 6 コミット）, 概要, 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9） (+3 more)

### Community 127 - "ImageHeader"
Cohesion: 0.11
Nodes (21): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+13 more)

### Community 128 - "仕様"
Cohesion: 0.25
Nodes (8): CLI, extra 節, globals 節, load, OOP エンコード, save, well-known 節, 仕様

### Community 129 - "TEST"
Cohesion: 0.25
Nodes (8): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, MethodErrorFailsFileInButKeepsGoing, TEST()

### Community 130 - "Format.hpp"
Cohesion: 0.46
Nodes (7): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make()

### Community 131 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (38): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+30 more)

### Community 132 - "NamedOop"
Cohesion: 0.29
Nodes (7): string, NameCollect, failed, out, NamedOop, name, value

### Community 133 - "send2"
Cohesion: 0.07
Nodes (31): BlockContextForkCreatesAndResumesProcess, ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PointAccessorsEqualsAndSetters, PointAdd (+23 more)

### Community 134 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 135 - "TEST"
Cohesion: 0.11
Nodes (22): cstdlib, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, optional (+14 more)

### Community 136 - "指摘"
Cohesion: 0.17
Nodes (12): [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅 (+4 more)

### Community 138 - "Array.cpp"
Cohesion: 0.31
Nodes (15): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+7 more)

### Community 139 - "uint32_t"
Cohesion: 0.20
Nodes (9): ファイルごとの変更, ファイルごとの変更, uint32_t, unique_ptr, mirror(), Temps, n, roots (+1 more)

### Community 140 - "Bytecode.hpp"
Cohesion: 0.26
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "Frame"
Cohesion: 0.22
Nodes (9): ContextExitGuard, FieldRoots, frame, Frame, context, isBlock, method, pc (+1 more)

### Community 142 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.14
Nodes (16): iterator, walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock() (+8 more)

### Community 145 - "codegen"
Cohesion: 0.25
Nodes (7): CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 146 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "指摘"
Cohesion: 0.12
Nodes (18): [High] 2^63 以上の整数リテラルが黙って 0 になる, [Low] カスケードの扱いが不完全, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, [Low] 宣言の検証がない（引数への代入・擬変数名・重複名）, [Low] 指数表記の意味が Smalltalk-80 と異なる, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる, [Medium] 二項演算子の直後の負数リテラルが、別のセレクタとして字句化される (+10 more)

### Community 149 - "TEST"
Cohesion: 0.22
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 152 - "ao_image_load"
Cohesion: 0.06
Nodes (46): [High] Save / Open Image の失敗がユーザーに見えない, 06 イメージ・セッション・C ABI・CLI, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない (+38 more)

### Community 153 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 154 - "P4-08 Point / Rectangle"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 156 - "block_test.cpp"
Cohesion: 0.14
Nodes (16): [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, WellKnown::global(), abortingSubclass(), countingPrintString(), CallContext, string, uint32_t, evalExpr() (+8 more)

### Community 157 - "P4-06 Stream / Transcript model"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, Transcript

### Community 158 - "findBytesOfSize"
Cohesion: 0.33
Nodes (6): path, uint32_t, findBytesOfSize(), readAll(), readHeapBytes(), writeAll()

### Community 160 - "Gc"
Cohesion: 0.08
Nodes (32): algorithm, BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, clearWeakAfterOldMark (+24 more)

### Community 161 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.40
Nodes (5): Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 162 - "create"
Cohesion: 0.50
Nodes (4): CallContext, uint16_t, uint8_t, create()

### Community 163 - "Interpreter"
Cohesion: 0.17
Nodes (12): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`） (+4 more)

### Community 164 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 167 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 172 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "method_dictionary_test.cpp"
Cohesion: 0.09
Nodes (25): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, BootstrapInstallsObjectIdentityEquals, GrowKeepsOuterOopAndEntries, InheritsFromSuperclass, InternSameBytesIsIdentical (+17 more)

### Community 176 - "expectSpecialSends"
Cohesion: 1.00
Nodes (3): CallContext, expectSpecialSends(), runSource()

## Knowledge Gaps
- **802 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+797 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1713 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **23 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `TEST`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `ao_main.cpp`, `TEST`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `TEST`, `Boot`, `Session.cpp`, `native_send_test.cpp`, `NativeMethod.cpp`, `TEST`, `TEST`, `string`, `P3 — Native Dispatch`, `vector`, `P2-02 class object skeletons`, `ImageSave.cpp`, `CallContext`, `Process.cpp`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `putNative`, `send`, `String.cpp`, `reshapeClass`, `指摘`, `image_save_load_test.cpp`, `Compile.cpp`, `WellKnown.cpp`, `TEST`, `SmallInteger.cpp`, `ImageFormat`, `abi.cpp`, `.false_`, `TEST`, `forceSlide`, `README.md`, `ao_Magnitude_lessOrEqual`, `.nil`, `CompiledMethodNatives.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `CallContext`, `P7 .aoimage Implementation Plan`, `Interpreter.cpp`, `performSend`, `Character.cpp`, `.isHeap`, `Interpreter::run`, `ImageHeader`, `Format.hpp`, `NamedOop`, `send2`, `Ao`, `TEST`, `Array.cpp`, `uint32_t`, `Frame`, `Roots.cpp`, `BlockProbe`, `intern`, `指摘`, `TEST`, `ao_image_load`, `block_test.cpp`, `findBytesOfSize`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `create`, `Interpreter`, `ActiveGuard`, `imageRegistryStubA`, `answerOne`, `Root`, `abortingNew`, `method_dictionary_test.cpp`, `expectSpecialSends`?**
  _High betweenness centrality (0.373) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `.fromSmallInteger`, `LargeInteger.cpp`, `NamedOop`, `VendorExtract.cpp`, `Oop`, `Emitter`, `Literal`, `ao_main.cpp`, `ChunkAction`, `Heap`, `Roots`, `TEST`, `Boot`, `Session.cpp`, `block_test.cpp`, `findBytesOfSize`, `Gc`, `NativeMethod.cpp`, `string`, `ChunkParser.cpp`, `ImageSave.cpp`, `Ast`, `method_dictionary_test.cpp`, `oopWords`, `Parser`, `String.cpp`, `reshapeClass`, `image_save_load_test.cpp`, `Compile.cpp`, `TEST`, `ImageFormat`, `abi.cpp`, `.false_`, `TEST`, `TEST`, `ao_accept_class`, `.nil`, `P6b vendor file-in Implementation Plan`, `ImageLoad.cpp`, `CallContext`?**
  _High betweenness centrality (0.127) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `.nil`, `P2-02 class object skeletons`, `WellKnown.cpp`, `Stream.cpp`, `Heap`, `Roots`, `intern`, `.false_`, `uint64_t`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.084) - this node is a cross-community bridge._
- **Are the 12 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 12 INFERRED edges - model-reasoned connections that need verification._
- **Are the 5 inferred relationships involving `Roots` (e.g. with `B2 `to:do:` bench` and `01 オブジェクトメモリ・GC・ブートストラップ`) actually correct?**
  _`Roots` has 5 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _802 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013333333333333334 - nodes in this community are weakly interconnected._
# Graph Report - p7-image  (2026-09-23)

## Corpus Check
- 210 files · ~79,299 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 76 file(s) not represented in the graph (top: .st 61, (none) 14, .toml 1)

## Summary
- 2521 nodes · 6525 edges · 121 communities (106 shown, 15 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 557 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `8f67ec7a`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Interpreter.cpp
- .isHeap
- LargeInteger.cpp
- image_save_load_test.cpp
- Token
- VendorExtract.cpp
- Object.cpp
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- putNative
- Behavior.cpp
- SPEC.md
- runSmalltalkTests
- .fromSmallInteger
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- P4-07 Process / ProcessorScheduler / Semaphore
- String.cpp
- Oop
- P4 — Kernel Native Implementation
- native_send_test.cpp
- ChunkAction
- lookup_test.cpp
- Bytecode interpreter
- TEST
- TEST
- ClassMethodCache
- gtest
- ChunkParser.cpp
- P3 — Native Dispatch
- Compile.cpp
- TEST
- cstdint
- P2-02 class object skeletons
- P6b — vendor file-in
- ImageSave.cpp
- P9 — Integration
- TEST
- Ast
- docs/README.md
- CallContext
- P6b vendor file-in Implementation Plan
- TEST
- heap
- TEST
- BlockContext.cpp
- Roots.cpp
- TEST
- TEST
- Parser
- .nil
- ao_abi.h
- TEST
- globals
- TEST
- roots
- test_support.hpp
- NativeMethod.cpp
- SmokeTests.swift
- TEST
- wellknown
- Send.cpp
- DiskHeader
- TEST
- P1-06: weak slot GC
- WellKnown.cpp
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-03: エラー表示と VoiceOver
- TEST
- Codegen.cpp
- ImageHeader
- TEST
- send2
- ao image save
- ImageFormat
- P3-04 lookup / super / doesNotUnderstand:
- intern
- Gc
- Vendor.hpp
- README.ja.md
- Globals.cpp
- TEST
- allocateRetry
- compiler_roundtrip_test.cpp
- build.sh
- test.sh
- TEST
- Symbol
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- allocateRetry
- WellKnown
- CompiledMethodNatives.cpp
- Memory maintenance guide
- string
- string_view
- DEFERRED.md
- gc_test.cpp
- main
- Image::load
- ImageLoad.cpp
- vector
- gc
- P7 .aoimage Implementation Plan
- P4-08 Point / Rectangle
- TEST
- imageRegistryStubA

## God Nodes (most connected - your core abstractions)
1. `Oop` - 563 edges
2. `Heap` - 159 edges
3. `WellKnown` - 152 edges
4. `Roots` - 95 edges
5. `send()` - 66 edges
6. `Ast` - 65 edges
7. `Parser` - 55 edges
8. `TEST()` - 49 edges
9. `Emitter` - 37 edges
10. `Gc` - 37 edges

## Surprising Connections (you probably didn't know these)
- `範囲` --references--> `applyMethodsFor()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `前提` --references--> `applyChunks()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `CLI` --references--> `fileInLoadOrder()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/Compile.cpp
- `仕様との対応` --references--> `addNamed()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/NativeMethod.cpp
- `extra 節` --references--> `isCatalogName()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/VendorExtract.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **P3 native dispatch path** — docs_phases_p3_lookup, docs_phases_p3_methoddictionary, docs_phases_p3_nativemethod, docs_phases_p3_dnu [EXTRACTED 1.00]
- **P1 object memory stack** — docs_phases_p1_ao_oop, docs_phases_p1_heap, docs_phases_p1_gc, docs_phases_p1_nursery [EXTRACTED 1.00]
- **P1 PR implementation chain** — docs_prs_p1_01, docs_prs_p1_02, docs_prs_p1_03, docs_phases_p1 [EXTRACTED 1.00]
- **Mandatory agent tooling trio** — concept_graphify, concept_serena, claude [EXTRACTED 1.00]
- **P0 skeleton deliverables** — concept_phase_p0, bridge_ao_abi, app_ao_placeholder, serena_project, concept_graphify [EXTRACTED 1.00]
- **Runtime app ABI boundary** — concept_libao_runtime, concept_ao_app, concept_c_abi_boundary, bridge_ao_abi [EXTRACTED 1.00]
- **P2 bootstrap pipeline** — docs_prs_p2_01_pr, docs_prs_p2_02_pr, docs_prs_p2_03_pr, docs_prs_p2_04_pr [EXTRACTED 1.00]
- **P3 method dispatch pipeline** — docs_prs_p3_01_pr, docs_prs_p3_02_pr, docs_prs_p3_03_pr, docs_prs_p3_04_pr, docs_prs_p3_05_pr [EXTRACTED 1.00]
- **P4 Kernel NativeMethod suite** — docs_prs_p4_01_pr, docs_prs_p4_02_pr, docs_prs_p4_03_pr, docs_prs_p4_04_pr, docs_prs_p4_05_pr, docs_prs_p4_06_pr, docs_prs_p4_07_pr, docs_prs_p4_08_pr, docs_prs_p4_09_pr [EXTRACTED 1.00]
- **P5 compiler pipeline** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunk_parser [EXTRACTED 1.00]
- **AOIM save/load and native rebind** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_image_save, docs_prs_p7_03_image_load, docs_prs_p7_02_nativemethod_symbol_names [EXTRACTED 1.00]
- **AppKit tool windows and menu** — docs_prs_p8_01_ao_app, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [EXTRACTED 1.00]
- **C ABI wrappers over ao::Runtime** — runtime_src_abi_ao_version, runtime_src_abi_ao_runtime_boot, runtime_src_abi_ao_runtime_shutdown [EXTRACTED 1.00]

## Communities (121 total, 15 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (145): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+137 more)

### Community 1 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (51): safepoint, ActiveGuard, rootShared, saved, allocateRetry(), byteCount(), clearNonlocal(), consumeNonlocal() (+43 more)

### Community 2 - ".isHeap"
Cohesion: 0.13
Nodes (58): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+50 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (58): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+50 more)

### Community 4 - "image_save_load_test.cpp"
Cohesion: 0.06
Nodes (40): ImageSave, ImageSaveLoad, iterator, KernelMethodsStayNative, NurseryGcAfterLoad, OnePlusTwoMatches, RejectsBadMagicAndSecondLoad, Image (+32 more)

### Community 5 - "Token"
Cohesion: 0.06
Nodes (53): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+45 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.11
Nodes (51): allowIndex(), containsHostWord(), size_t, string, string_view, vector, dropCycles(), dropMissingSupers() (+43 more)

### Community 7 - "Object.cpp"
Cohesion: 0.05
Nodes (119): Native selector mangling, マングル表, 規則, natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+111 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 10 - "Emitter"
Cohesion: 0.19
Nodes (9): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, cur_ (+1 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.06
Nodes (51): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, cassert, ファイル配置（すべて little-endian）, EachImageSlotLists127Names, ExtraListsCmUser (+43 more)

### Community 12 - "TEST"
Cohesion: 0.06
Nodes (32): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+24 more)

### Community 13 - "Literal"
Cohesion: 0.08
Nodes (26): CompileEnv, instVarNames, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr (+18 more)

### Community 14 - "putNative"
Cohesion: 0.09
Nodes (30): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+22 more)

### Community 15 - "Behavior.cpp"
Cohesion: 0.15
Nodes (32): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, MethodDictionary, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_() (+24 more)

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "runSmalltalkTests"
Cohesion: 0.20
Nodes (11): ao_AoTest_assert_equals_(), CallContext, path, string, string_view, uint32_t, vector, listTests() (+3 more)

### Community 18 - ".fromSmallInteger"
Cohesion: 0.18
Nodes (31): int64_t, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill() (+23 more)

### Community 19 - "Heap"
Cohesion: 0.07
Nodes (33): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+25 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (33): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handleAt (+25 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.15
Nodes (18): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+10 more)

### Community 24 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.29
Nodes (7): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, ProcessorScheduler, Semaphore, send

### Community 25 - "String.cpp"
Cohesion: 0.17
Nodes (26): Task 1: 形式ヘッダと即値ビット, allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString() (+18 more)

### Community 26 - "Oop"
Cohesion: 0.09
Nodes (25): 現状, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make() (+17 more)

### Community 27 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (31): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+23 more)

### Community 28 - "native_send_test.cpp"
Cohesion: 0.17
Nodes (15): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, CallContext, NativeFn, uint32_t, WellKnown (+7 more)

### Community 29 - "ChunkAction"
Cohesion: 0.11
Nodes (19): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+11 more)

### Community 30 - "lookup_test.cpp"
Cohesion: 0.15
Nodes (16): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, slotAt, slotAtPut, superOf(), CallContext, NativeFn (+8 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "TEST"
Cohesion: 0.09
Nodes (24): AddInternsByFunctionPointer, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+16 more)

### Community 33 - "TEST"
Cohesion: 0.09
Nodes (22): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, format, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf (+14 more)

### Community 34 - "ClassMethodCache"
Cohesion: 0.13
Nodes (14): 範囲, Interpreter, run, ClassMethodCache, entries, forget, insert, kSize (+6 more)

### Community 35 - "gtest"
Cohesion: 0.10
Nodes (26): chrono, Chunk, climits, compile, compiler, vector, TEST(), CompilerSmoke (+18 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.20
Nodes (23): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, uint32_t, vector, firstLine() (+15 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.12
Nodes (16): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+8 more)

### Community 38 - "Compile.cpp"
Cohesion: 0.18
Nodes (30): CompileError, message, span, string, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価, collectNursery, allocateRetry() (+22 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (12): BlockWithArgs, Cascade, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma (+4 more)

### Community 40 - "cstdint"
Cohesion: 0.16
Nodes (6): string, string_view, cstddef, cstdint, memory, NativeMethod

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.12
Nodes (18): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+10 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.12
Nodes (35): appendRaw(), appendRecord(), collectExtra(), collectImageSlot(), byte, size_t, string, string_view (+27 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.09
Nodes (22): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+14 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.07
Nodes (28): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+20 more)

### Community 47 - "docs/README.md"
Cohesion: 0.28
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 48 - "CallContext"
Cohesion: 0.17
Nodes (12): HostOopHook, CallContext, activeContext, cache, inspectHook, interpretedBytecodes, nonlocalHome, nonlocalReturn (+4 more)

### Community 49 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.11
Nodes (17): Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 6: フェーズ完了, 仕様, 前提 (+9 more)

### Community 50 - "TEST"
Cohesion: 0.18
Nodes (9): CascadeAndBlock, Codegen, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), EighteenDigitIntegerSurvivesAsInt, ErrorSpanOnDanglingBinary, ReturnOnePlusTwo, SuperSendAndGlobal (+1 more)

### Community 52 - "TEST"
Cohesion: 0.10
Nodes (19): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+11 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.22
Nodes (15): P4 microbench, P6 interpreter bench, ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn (+7 more)

### Community 54 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 56 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 57 - "Parser"
Cohesion: 0.13
Nodes (18): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+10 more)

### Community 58 - ".nil"
Cohesion: 0.28
Nodes (27): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+19 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.15
Nodes (13): EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RewritesHostSelectorAndDefersMissingSuper, string, string_view (+5 more)

### Community 61 - "globals"
Cohesion: 0.10
Nodes (18): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, Transcript, EachClassSkipsSmalltalkImageNonClassExtra, globals (+10 more)

### Community 62 - "TEST"
Cohesion: 0.12
Nodes (15): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, cstdio, runtime, ao executable, ao_runtime library (+7 more)

### Community 64 - "test_support.hpp"
Cohesion: 0.15
Nodes (14): largeinteger, floatValue(), makeFloat(), Boot, cache, ctx, heap, roots (+6 more)

### Community 65 - "NativeMethod.cpp"
Cohesion: 0.16
Nodes (20): CLI, extra 節, globals 節, load, OOP エンコード, save, well-known 節, 仕様 (+12 more)

### Community 66 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (9): BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST(), LineEndBangAfterBinaryStillTerminates, SplitsMethodsForAndProtocol (+1 more)

### Community 69 - "Send.cpp"
Cohesion: 0.16
Nodes (20): Task 4: load と受け入れ評価, interpreter, InlineCache, cachedClass, cachedMethod, applyMethod(), lookup(), allocateMessage() (+12 more)

### Community 70 - "DiskHeader"
Cohesion: 0.10
Nodes (20): uint16_t, uint32_t, uint64_t, DiskHeader, endian, extraCount, globalCount, headerBytes (+12 more)

### Community 71 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 73 - "WellKnown.cpp"
Cohesion: 0.08
Nodes (32): deque, size_t, string, string_view, unordered_map, WellKnown, Entry, cls (+24 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "Codegen.cpp"
Cohesion: 0.10
Nodes (26): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), LitKind, string (+18 more)

### Community 79 - "ImageHeader"
Cohesion: 0.13
Nodes (17): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+9 more)

### Community 80 - "TEST"
Cohesion: 0.29
Nodes (7): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, TEST(), TenMillionToDo, UserCompiledMethodDoesNotFailScan

### Community 81 - "send2"
Cohesion: 0.22
Nodes (10): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+2 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "ImageFormat"
Cohesion: 0.13
Nodes (15): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+7 more)

### Community 84 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.17
Nodes (10): class hierarchy, doesNotUnderstand:, lookup, MethodDictionary, NativeMethod, P3-04 lookup / super / doesNotUnderstand:, inline cache, lookup (+2 more)

### Community 85 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 86 - "Gc"
Cohesion: 0.10
Nodes (20): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, copy, failed_, heap_, oldCompacted_, roots_ (+12 more)

### Community 87 - "Vendor.hpp"
Cohesion: 0.15
Nodes (14): Task 4: LOAD_ORDER とホストパッチ, string, string_view, vector, isVendorStub(), VendorClassFile, chunkText, className (+6 more)

### Community 88 - "README.ja.md"
Cohesion: 0.09
Nodes (18): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+10 more)

### Community 89 - "Globals.cpp"
Cohesion: 0.36
Nodes (7): at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 90 - "TEST"
Cohesion: 0.18
Nodes (11): AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, BlockEval, CopyDoesNotWriteOuter, ExampleFilePasses, IfTrueIfFalseFromCompiledMethod, NonLocalReturnSkipsRest (+3 more)

### Community 91 - "allocateRetry"
Cohesion: 0.27
Nodes (9): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, createBlock(), createMethod(), Root (+1 more)

### Community 92 - "compiler_roundtrip_test.cpp"
Cohesion: 0.24
Nodes (11): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, CompiledMethod, LayoutNativeCodeNil, collectOld, TEST(), CallContext, uint32_t (+3 more)

### Community 95 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 96 - "Symbol"
Cohesion: 0.20
Nodes (9): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, CallContext, uint32_t, stubEq(), TEST() (+1 more)

### Community 97 - "TEST"
Cohesion: 0.40
Nodes (5): ChunkFileIn, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, TEST()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.49
Nodes (9): allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), run(), superOf() (+1 more)

### Community 104 - "allocateRetry"
Cohesion: 0.27
Nodes (8): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, create(), Root, slot

### Community 105 - "WellKnown"
Cohesion: 0.16
Nodes (10): class skeleton, metaclass cycle, P2-03 metaclass cycle, Bootstrap, Globals, P2-04 Smalltalk global dictionary, Smalltalk global dictionary, CallContext (+2 more)

### Community 106 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 111 - "gc_test.cpp"
Cohesion: 0.18
Nodes (9): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), objectBytes (+1 more)

### Community 112 - "main"
Cohesion: 0.23
Nodes (12): Task 5: CLI, 結論, addRoots, bootAndRunTests(), string, imageUsage(), main(), runExtractVendor() (+4 more)

### Community 113 - "Image::load"
Cohesion: 0.30
Nodes (11): bindAll(), checkGlobals(), string, WellKnown, extraNamesOk(), fileOop(), globalNamesOk(), Image::load() (+3 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.45
Nodes (11): acceptWord(), uint64_t, heapShaped(), bits, payloadFits(), precheck(), readU64(), recordsOk() (+3 more)

### Community 115 - "vector"
Cohesion: 0.32
Nodes (12): byte, size_t, string_view, uint32_t, vector, findRecord(), parseRecord(), parseRecords() (+4 more)

### Community 117 - "P7 .aoimage Implementation Plan"
Cohesion: 0.20
Nodes (10): Global Constraints, P7 .aoimage Implementation Plan, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 6: フェーズを閉じる, リスク, 仕様との対応, 前提 (+2 more)

### Community 118 - "P4-08 Point / Rectangle"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 119 - "TEST"
Cohesion: 0.40
Nodes (5): KeepsNativeIdentityEquals, RebindsBagAndEvaluatesInstVar, RefusesNewSelectorOnObject, TEST(), VendorOverwrite

### Community 120 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

## Knowledge Gaps
- **585 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `kind` (+580 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1052 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **15 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Interpreter.cpp`, `.isHeap`, `LargeInteger.cpp`, `image_save_load_test.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `putNative`, `Behavior.cpp`, `runSmalltalkTests`, `.fromSmallInteger`, `Heap`, `Roots`, `Geometry.cpp`, `String.cpp`, `native_send_test.cpp`, `lookup_test.cpp`, `TEST`, `TEST`, `ClassMethodCache`, `gtest`, `P3 — Native Dispatch`, `Compile.cpp`, `cstdint`, `ImageSave.cpp`, `CallContext`, `P6b vendor file-in Implementation Plan`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `.nil`, `test_support.hpp`, `NativeMethod.cpp`, `Send.cpp`, `DiskHeader`, `TEST`, `WellKnown.cpp`, `TEST`, `send2`, `ImageFormat`, `P3-04 lookup / super / doesNotUnderstand:`, `intern`, `Gc`, `Globals.cpp`, `allocateRetry`, `compiler_roundtrip_test.cpp`, `Symbol`, `Bootstrap.cpp`, `allocateRetry`, `WellKnown`, `CompiledMethodNatives.cpp`, `gc_test.cpp`, `Image::load`, `ImageLoad.cpp`, `imageRegistryStubA`?**
  _High betweenness centrality (0.407) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `image_save_load_test.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `Behavior.cpp`, `.fromSmallInteger`, `TEST`, `String.cpp`, `Oop`, `native_send_test.cpp`, `lookup_test.cpp`, `TEST`, `ClassMethodCache`, `gtest`, `P3 — Native Dispatch`, `cstdint`, `ImageSave.cpp`, `CallContext`, `BlockContext.cpp`, `.nil`, `test_support.hpp`, `NativeMethod.cpp`, `Send.cpp`, `P1-06: weak slot GC`, `WellKnown.cpp`, `ImageHeader`, `P3-04 lookup / super / doesNotUnderstand:`, `intern`, `Gc`, `Globals.cpp`, `Symbol`, `Bootstrap.cpp`, `WellKnown`, `gc_test.cpp`, `main`, `Image::load`, `ImageLoad.cpp`, `vector`, `P7 .aoimage Implementation Plan`?**
  _High betweenness centrality (0.119) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `cstdint`, `Oop`, `Heap`, `Roots`?**
  _High betweenness centrality (0.112) - this node is a cross-community bridge._
- **Are the 2 inferred relationships involving `Oop` (e.g. with `現状` and `WellKnown::eachImageSlot()`) actually correct?**
  _`Oop` has 2 INFERRED edges - model-reasoned connections that need verification._
- **Are the 8 inferred relationships involving `Heap` (e.g. with `手順` and `load`) actually correct?**
  _`Heap` has 8 INFERRED edges - model-reasoned connections that need verification._
- **Are the 58 inferred relationships involving `send()` (e.g. with `Task 4: load と受け入れ評価` and `Task 5: CLI`) actually correct?**
  _`send()` has 58 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _585 weakly-connected nodes found - possible documentation gaps or missing edges._
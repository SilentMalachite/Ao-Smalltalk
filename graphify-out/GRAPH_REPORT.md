# Graph Report - p6b-vendor  (2026-09-22)

## Corpus Check
- 201 files · ~70,704 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 76 file(s) not represented in the graph (top: .st 61, (none) 14, .toml 1)

## Summary
- 2287 nodes · 5971 edges · 112 communities (96 shown, 16 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 522 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `3a738d25`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Interpreter.cpp
- .isHeap
- LargeInteger.cpp
- SmallInteger.cpp
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
- ao_main.cpp
- send
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- P4 — Kernel Native Implementation
- String.cpp
- Oop
- P4-03 Magnitude / SmallInteger / Character
- native_send_test.cpp
- ChunkAction
- TEST
- Bytecode interpreter
- TEST
- TEST
- uint32_t
- gtest
- ChunkParser.cpp
- WellKnown
- Compile.cpp
- TEST
- cstdint
- P2-02 class object skeletons
- P6b — vendor file-in
- performSend
- P9-04: v1 ゴールデン受け入れ
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
- Gc
- ao_abi.h
- TEST
- TEST
- TEST
- roots
- Boot
- create
- SmokeTests.swift
- TEST
- wellknown
- Boolean.cpp
- P9-02: Browser accept
- TEST
- P1-06: weak slot GC
- WellKnown.cpp
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-03: エラー表示と VoiceOver
- TEST
- Codegen.cpp
- install
- TEST
- send0
- ao image save
- .false_
- lookup_test.cpp
- intern
- Gc.cpp
- Frame
- README.ja.md
- install
- TEST
- allocateRetry
- collectNursery
- build.sh
- test.sh
- TEST
- native_method_test.cpp
- CompileError
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- allocateRetry
- TEST
- DepthGuard
- Memory maintenance guide
- string
- string_view
- DEFERRED.md
- gc

## God Nodes (most connected - your core abstractions)
1. `Oop` - 536 edges
2. `WellKnown` - 148 edges
3. `Heap` - 134 edges
4. `Roots` - 90 edges
5. `Ast` - 65 edges
6. `send()` - 64 edges
7. `Parser` - 55 edges
8. `TEST()` - 49 edges
9. `Emitter` - 37 edges
10. `Gc` - 36 edges

## Surprising Connections (you probably didn't know these)
- `範囲` --references--> `applyMethodsFor()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `前提` --references--> `applyChunks()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `P6 interpreter bench` --references--> `makeNativeBlock()`  [INFERRED]
  docs/bench.md → runtime/src/kernel/BlockContext.cpp
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `P6b vendor file-in Implementation Plan` --references--> `parseChunks()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → compiler/src/ChunkParser.cpp

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

## Communities (112 total, 16 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (141): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+133 more)

### Community 1 - "Interpreter.cpp"
Cohesion: 0.19
Nodes (21): safepoint, ActiveGuard, rootShared, saved, allocateRetry(), byteCount(), clearNonlocal(), CallContext (+13 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (61): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+53 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (26): Native selector mangling, マングル表, 規則, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_() (+18 more)

### Community 5 - "Token"
Cohesion: 0.06
Nodes (53): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+45 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.08
Nodes (62): Task 4: LOAD_ORDER とホストパッチ, string, vector, VendorClassFile, chunkText, className, superName, unsupportedShape (+54 more)

### Community 7 - "Object.cpp"
Cohesion: 0.13
Nodes (46): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+38 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "Float.cpp"
Cohesion: 0.19
Nodes (29): FlOp, largeinteger, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply() (+21 more)

### Community 10 - "Emitter"
Cohesion: 0.17
Nodes (10): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, cur_ (+2 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (35): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+27 more)

### Community 12 - "TEST"
Cohesion: 0.05
Nodes (43): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+35 more)

### Community 13 - "Literal"
Cohesion: 0.07
Nodes (33): CompileEnv, instVarNames, CompileResult, error, image, ok, int64_t, LitKind (+25 more)

### Community 14 - "putNative"
Cohesion: 0.10
Nodes (30): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+22 more)

### Community 15 - "Behavior.cpp"
Cohesion: 0.20
Nodes (27): allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "ao_main.cpp"
Cohesion: 0.10
Nodes (29): climits, vector, dyld, filesystem, fstream, iterator, MethodImage, addRoots (+21 more)

### Community 18 - "send"
Cohesion: 0.08
Nodes (82): CompiledMethod accessors (P5), interpreter, int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_() (+74 more)

### Community 19 - "Heap"
Cohesion: 0.06
Nodes (38): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+30 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (39): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handles_ (+31 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.09
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 25 - "String.cpp"
Cohesion: 0.19
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 26 - "Oop"
Cohesion: 0.21
Nodes (13): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+5 more)

### Community 27 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 28 - "native_send_test.cpp"
Cohesion: 0.25
Nodes (9): inline cache, lookup, P3-05 inline cache and send, send, CallContext, uint32_t, stubA(), stubB() (+1 more)

### Community 29 - "ChunkAction"
Cohesion: 0.11
Nodes (19): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+11 more)

### Community 30 - "TEST"
Cohesion: 0.10
Nodes (22): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, slotAt (+14 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "uint32_t"
Cohesion: 0.15
Nodes (12): Interpreter, run, applyMethod(), uint32_t, unique_ptr, RootedArray, data, n (+4 more)

### Community 35 - "gtest"
Cohesion: 0.13
Nodes (19): AtPutFindsInternedKey, Bootstrap, chrono, Chunk, compile, CompiledMethod, compiler, context (+11 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.20
Nodes (23): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, uint32_t, vector, firstLine() (+15 more)

### Community 37 - "WellKnown"
Cohesion: 0.09
Nodes (18): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+10 more)

### Community 38 - "Compile.cpp"
Cohesion: 0.19
Nodes (27): Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価, string_view, isVendorStub(), allocateRetry(), applyChunks(), applyClassDef(), applyMethodsFor() (+19 more)

### Community 39 - "TEST"
Cohesion: 0.09
Nodes (19): BlockWithArgs, Cascade, ParseResult, error, method, ok, deque, parseMethod() (+11 more)

### Community 40 - "cstdint"
Cohesion: 0.14
Nodes (7): string, Literal::Literal(), string_view, cstddef, cstdint, memory, NativeMethod

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "performSend"
Cohesion: 0.17
Nodes (11): consumeNonlocal(), deque, hit(), Leave, leave, value, miss(), OperandStack (+3 more)

### Community 44 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.12
Nodes (16): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+8 more)

### Community 47 - "docs/README.md"
Cohesion: 0.16
Nodes (21): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 48 - "CallContext"
Cohesion: 0.09
Nodes (23): HostOopHook, CallContext, activeContext, cache, inspectHook, interpretedBytecodes, nonlocalHome, nonlocalReturn (+15 more)

### Community 49 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.11
Nodes (17): Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 6: フェーズ完了, 仕様, 前提 (+9 more)

### Community 50 - "TEST"
Cohesion: 0.18
Nodes (9): CascadeAndBlock, Codegen, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), EighteenDigitIntegerSurvivesAsInt, ErrorSpanOnDanglingBinary, ReturnOnePlusTwo, SuperSendAndGlobal (+1 more)

### Community 52 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.26
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

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

### Community 58 - "Gc"
Cohesion: 0.25
Nodes (8): Gc, clearWeakAfterNursery, failed_, heap_, oldCompacted_, roots_, scavengeFromRoots, Gc::collectNursery()

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.15
Nodes (13): EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RewritesHostSelectorAndDefersMissingSuper, string, string_view (+5 more)

### Community 61 - "TEST"
Cohesion: 0.18
Nodes (11): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut, TranscriptModel (+3 more)

### Community 62 - "TEST"
Cohesion: 0.13
Nodes (14): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, runtime, ao executable, ao_runtime library, ao_runtime_tests (+6 more)

### Community 64 - "Boot"
Cohesion: 0.13
Nodes (15): int64_t, pt(), rect(), formatOf(), floatValue(), makeFloat(), Boot, cache (+7 more)

### Community 65 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 66 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (9): BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST(), LineEndBangAfterBinaryStillTerminates, SplitsMethodsForAndProtocol (+1 more)

### Community 69 - "Boolean.cpp"
Cohesion: 0.27
Nodes (23): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+15 more)

### Community 70 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 71 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (28): deque, size_t, string, string_view, WellKnown, Entry, cls, name (+20 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "Codegen.cpp"
Cohesion: 0.11
Nodes (23): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), LitKind, string (+15 more)

### Community 79 - "install"
Cohesion: 0.33
Nodes (11): install, natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+3 more)

### Community 80 - "TEST"
Cohesion: 0.25
Nodes (8): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, nativeRequiredDictsAreNative(), TEST(), TenMillionToDo, UserCompiledMethodDoesNotFailScan

### Community 81 - "send0"
Cohesion: 0.14
Nodes (15): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, Geometry, LayoutNativeCodeNil, LoadOrderEvaluatesLinkRoundTrip, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber (+7 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - ".false_"
Cohesion: 0.20
Nodes (15): uint64_t, ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), CallContext, uint32_t (+7 more)

### Community 84 - "lookup_test.cpp"
Cohesion: 0.20
Nodes (10): class hierarchy, doesNotUnderstand:, lookup, MethodDictionary, NativeMethod, P3-04 lookup / super / doesNotUnderstand:, CallContext, uint32_t (+2 more)

### Community 85 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 86 - "Gc.cpp"
Cohesion: 0.23
Nodes (9): clearWeakAfterOldMark, copy, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc(), Gc::scavengeFromRoots() (+1 more)

### Community 87 - "Frame"
Cohesion: 0.25
Nodes (8): FieldRoots, frame, Frame, context, isBlock, method, pc, receiver

### Community 88 - "README.ja.md"
Cohesion: 0.08
Nodes (21): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+13 more)

### Community 89 - "install"
Cohesion: 0.53
Nodes (5): at(), atPut(), string_view, WellKnown, install()

### Community 90 - "TEST"
Cohesion: 0.18
Nodes (11): AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, BlockEval, CopyDoesNotWriteOuter, ExampleFilePasses, IfTrueIfFalseFromCompiledMethod, NonLocalReturnSkipsRest (+3 more)

### Community 91 - "allocateRetry"
Cohesion: 0.43
Nodes (7): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, createBlock(), createMethod()

### Community 92 - "collectNursery"
Cohesion: 0.39
Nodes (8): collectNursery, collectOld, Gc::safepoint(), CallContext, uint32_t, forceNursery(), forceOld(), forceSlide()

### Community 95 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 96 - "native_method_test.cpp"
Cohesion: 0.29
Nodes (6): AddInternsByFunctionPointer, NameAndApply, CallContext, uint32_t, stubEq(), TEST()

### Community 97 - "CompileError"
Cohesion: 0.14
Nodes (14): ChunkFileIn, CompileError, message, span, string, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar (+6 more)

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.35
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 104 - "allocateRetry"
Cohesion: 0.43
Nodes (6): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, create()

### Community 105 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

## Knowledge Gaps
- **527 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `kind` (+522 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 957 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **16 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Interpreter.cpp`, `.isHeap`, `LargeInteger.cpp`, `SmallInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `Behavior.cpp`, `ao_main.cpp`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `P1 — Object Memory`, `String.cpp`, `native_send_test.cpp`, `TEST`, `TEST`, `uint32_t`, `gtest`, `WellKnown`, `Compile.cpp`, `cstdint`, `P2-02 class object skeletons`, `performSend`, `CallContext`, `P6b vendor file-in Implementation Plan`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `Gc`, `Boot`, `create`, `Boolean.cpp`, `TEST`, `WellKnown.cpp`, `TEST`, `install`, `send0`, `.false_`, `lookup_test.cpp`, `intern`, `Gc.cpp`, `Frame`, `README.ja.md`, `install`, `allocateRetry`, `collectNursery`, `native_method_test.cpp`, `Bootstrap.cpp`, `allocateRetry`?**
  _High betweenness centrality (0.457) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `P2-02 class object skeletons`, `.false_`, `Heap`, `Roots`, `Oop`?**
  _High betweenness centrality (0.106) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `ao_main.cpp`, `send`, `TEST`, `String.cpp`, `native_send_test.cpp`, `TEST`, `TEST`, `gtest`, `WellKnown`, `cstdint`, `P2-02 class object skeletons`, `CallContext`, `BlockContext.cpp`, `Gc`, `Boot`, `create`, `P1-06: weak slot GC`, `WellKnown.cpp`, `install`, `lookup_test.cpp`, `intern`, `Gc.cpp`, `install`, `native_method_test.cpp`, `Bootstrap.cpp`?**
  _High betweenness centrality (0.080) - this node is a cross-community bridge._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _527 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013986013986013986 - nodes in this community are weakly interconnected._
- **Should `.isHeap` be split into smaller, more focused modules?**
  _Cohesion score 0.12202380952380952 - nodes in this community are weakly interconnected._
- **Should `LargeInteger.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.1191772534785239 - nodes in this community are weakly interconnected._
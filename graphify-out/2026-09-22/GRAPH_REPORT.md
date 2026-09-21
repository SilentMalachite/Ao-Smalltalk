# Graph Report - p5-compiler  (2026-09-22)

## Corpus Check
- 181 files · ~57,710 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 61 file(s) not represented in the graph (top: .st 50, (none) 10, .toml 1)

## Summary
- 1959 nodes · 5153 edges · 111 communities (92 shown, 19 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 445 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `4ff2f374`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Object.cpp
- .isHeap
- .nil
- Oop
- LargeInteger.cpp
- SmallInteger.cpp
- TEST
- Stream.cpp
- String.cpp
- Heap.cpp
- Token
- TEST
- Behavior.cpp
- TEST
- Float.cpp
- gtest
- Heap
- P4 — Kernel Native Implementation
- Geometry.cpp
- Gc
- Roots
- putNative
- SPEC.md
- Gc.cpp
- TEST
- WellKnown.cpp
- P3 — Native Dispatch
- P2-02 class object skeletons
- P6b — vendor file-in
- P4-03 Magnitude / SmallInteger / Character
- Bytecode interpreter
- Bootstrap.cpp
- TEST
- docs/README.md
- TEST
- TEST
- Boot
- TEST
- TEST
- TEST
- TEST
- ao_abi.h
- send
- ClassMethodCache
- SmokeTests.swift
- P9-03: エラー表示と VoiceOver
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-02: Browser accept
- stubA
- TEST
- TEST
- ao image save
- install
- intern
- build.sh
- test.sh
- cstdint
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- Array.cpp
- BlockContext.cpp
- Roots.cpp
- Memory maintenance guide
- wellknown
- Literal
- ClassDef
- P1 — Object Memory
- cstring
- Parser
- Emitter
- Root
- ChunkParser.cpp
- .advance
- Compile.cpp
- Send.cpp
- ChunkAction
- .make
- CompileResult
- Ast
- Scanner
- Bytecode.hpp
- TEST
- Codegen.cpp
- .isTrue
- TEST
- .bindTemp
- send1
- TEST
- CompileError
- Op
- P1-06: weak slot GC
- P9-04: v1 ゴールデン受け入れ
- compiler
- P4-04 Array / ByteArray / String / Symbol
- allocateRetry
- native_send_test.cpp
- send0
- Root
- Root
- Root
- Root
- Root
- stubEq
- string
- string_view

## God Nodes (most connected - your core abstractions)
1. `Oop` - 493 edges
2. `WellKnown` - 145 edges
3. `Heap` - 132 edges
4. `Roots` - 73 edges
5. `Ast` - 65 edges
6. `send()` - 58 edges
7. `Parser` - 55 edges
8. `TEST()` - 49 edges
9. `Emitter` - 37 edges
10. `TEST()` - 34 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `ao_ArrayedCollection_at_()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Array.cpp
- `マングル表` --references--> `ao_ArrayedCollection_new_()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Array.cpp

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

## Communities (111 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - "Object.cpp"
Cohesion: 0.13
Nodes (46): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+38 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (61): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+53 more)

### Community 3 - ".nil"
Cohesion: 0.15
Nodes (41): CompiledMethod accessors (P5), int64_t, at(), string_view, WellKnown, install(), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals() (+33 more)

### Community 4 - "Oop"
Cohesion: 0.13
Nodes (19): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+11 more)

### Community 5 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 6 - "SmallInteger.cpp"
Cohesion: 0.12
Nodes (49): Native selector mangling, マングル表, 規則, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_() (+41 more)

### Community 7 - "TEST"
Cohesion: 0.05
Nodes (43): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+35 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "String.cpp"
Cohesion: 0.19
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 10 - "Heap.cpp"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, uint16_t, uint32_t, ObjectHeader, flags, hash (+26 more)

### Community 11 - "Token"
Cohesion: 0.10
Nodes (36): cmath, lexBinary, lexCharacter, lexIdentOrKeyword, lexNumber, lexString, skipTrivia, string (+28 more)

### Community 12 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 13 - "Behavior.cpp"
Cohesion: 0.20
Nodes (27): allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 14 - "TEST"
Cohesion: 0.67
Nodes (3): AddInternsByFunctionPointer, NameAndApply, TEST()

### Community 15 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 16 - "gtest"
Cohesion: 0.15
Nodes (15): AtPutFindsInternedKey, Bootstrap, chrono, CompiledMethod, context, globals, GrowKeepsOuterOopAndEntries, gtest (+7 more)

### Community 17 - "Heap"
Cohesion: 0.08
Nodes (27): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+19 more)

### Community 18 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (27): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send (+19 more)

### Community 19 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 20 - "Gc"
Cohesion: 0.17
Nodes (12): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectOld, copy, failed_, heap_, oldCompacted_ (+4 more)

### Community 21 - "Roots"
Cohesion: 0.09
Nodes (20): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handleAt (+12 more)

### Community 22 - "putNative"
Cohesion: 0.10
Nodes (30): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+22 more)

### Community 23 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 24 - "Gc.cpp"
Cohesion: 0.14
Nodes (12): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), gc (+4 more)

### Community 25 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 26 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (34): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+26 more)

### Community 27 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 28 - "P2-02 class object skeletons"
Cohesion: 0.08
Nodes (27): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+19 more)

### Community 29 - "P6b — vendor file-in"
Cohesion: 0.11
Nodes (24): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+16 more)

### Community 30 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (23): P4 microbench, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior (+15 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "Bootstrap.cpp"
Cohesion: 0.35
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "docs/README.md"
Cohesion: 0.20
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 35 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 36 - "TEST"
Cohesion: 0.10
Nodes (22): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, slotAt (+14 more)

### Community 37 - "Boot"
Cohesion: 0.15
Nodes (13): rect(), formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap (+5 more)

### Community 38 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 40 - "TEST"
Cohesion: 0.18
Nodes (11): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut, TranscriptModel (+3 more)

### Community 41 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 42 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 43 - "send"
Cohesion: 0.24
Nodes (25): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill(), ao_Collection_includes_() (+17 more)

### Community 44 - "ClassMethodCache"
Cohesion: 0.12
Nodes (16): HostOopHook, CallContext, cache, inspectHook, transcriptHook, ClassMethodCache, addRoots, entries (+8 more)

### Community 45 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 46 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 47 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 48 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 49 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 50 - "stubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 51 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 52 - "TEST"
Cohesion: 0.33
Nodes (6): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, TEST(), TenMillionToDo

### Community 53 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 54 - "install"
Cohesion: 0.47
Nodes (8): install, ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), CallContext, uint32_t

### Community 55 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 58 - "cstdint"
Cohesion: 0.13
Nodes (7): string, vector, string_view, cstdint, NativeMethod, MethodImage, CallContext

### Community 65 - "Array.cpp"
Cohesion: 0.38
Nodes (10): natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size(), CallContext (+2 more)

### Community 66 - "BlockContext.cpp"
Cohesion: 0.26
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 67 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 70 - "Literal"
Cohesion: 0.09
Nodes (26): CompileEnv, instVarNames, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr (+18 more)

### Community 71 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 72 - "P1 — Object Memory"
Cohesion: 0.15
Nodes (18): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+10 more)

### Community 73 - "cstring"
Cohesion: 0.12
Nodes (16): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, cstdio, cstring, runtime, ao executable (+8 more)

### Community 74 - "Parser"
Cohesion: 0.14
Nodes (15): ParseResult, error, method, ok, deque, string_view, parseMethod(), Parser (+7 more)

### Community 75 - "Emitter"
Cohesion: 0.28
Nodes (4): size_t, Emitter, cur_, scope_

### Community 77 - "ChunkParser.cpp"
Cohesion: 0.20
Nodes (20): atLineEnd(), classify(), string, string_view, uint32_t, vector, firstLine(), isBlank() (+12 more)

### Community 79 - "Compile.cpp"
Cohesion: 0.23
Nodes (19): largeinteger, collectNursery, allocateRetry(), applyChunks(), applyClassDef(), applyMethodsFor(), boxBytes(), boxLiteral() (+11 more)

### Community 80 - "Send.cpp"
Cohesion: 0.17
Nodes (18): InlineCache, cachedClass, cachedMethod, lookup(), allocateMessage(), cacheIndex(), ClassMethodCache::addRoots(), ClassMethodCache::insert() (+10 more)

### Community 81 - "ChunkAction"
Cohesion: 0.11
Nodes (19): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+11 more)

### Community 82 - ".make"
Cohesion: 0.19
Nodes (8): uint32_t, SourceSpan, end, start, Kind, string, vector, join()

### Community 83 - "CompileResult"
Cohesion: 0.12
Nodes (14): CascadeAndBlock, Codegen, CompileResult, error, image, ok, codegen(), compileMethod() (+6 more)

### Community 84 - "Ast"
Cohesion: 0.12
Nodes (16): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+8 more)

### Community 85 - "Scanner"
Cohesion: 0.18
Nodes (12): ArrayAndByteArrayHeaders, AssignVariantsAndComment, uint32_t, Scanner, i_, lexSymbol, make, next (+4 more)

### Community 86 - "Bytecode.hpp"
Cohesion: 0.26
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 87 - "TEST"
Cohesion: 0.18
Nodes (11): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+3 more)

### Community 88 - "Codegen.cpp"
Cohesion: 0.21
Nodes (9): LitKind, MethodImage, string, disassemble(), formatLit(), internable(), opName(), utf8Scalar() (+1 more)

### Community 89 - ".isTrue"
Cohesion: 0.41
Nodes (8): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 90 - "TEST"
Cohesion: 0.18
Nodes (10): BlockWithArgs, Cascade, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma, TempsAndAssign (+2 more)

### Community 91 - ".bindTemp"
Cohesion: 0.24
Nodes (9): string_view, uint8_t, vector, Scope, args, copied, isBlock, parent (+1 more)

### Community 92 - "send1"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), TEST() (+1 more)

### Community 93 - "TEST"
Cohesion: 0.29
Nodes (7): BangInCharacterDoesNotSplit, Chunk, ClassDefinitionShape, TEST(), LineEndBangAfterBinaryStillTerminates, SplitsMethodsForAndProtocol, SubclassSendInMethodStaysMethodsFor

### Community 94 - "CompileError"
Cohesion: 0.25
Nodes (8): ChunkFileIn, CompileError, message, span, string, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, TEST()

### Community 95 - "Op"
Cohesion: 0.32
Nodes (3): Op, uint16_t, int16_t

### Community 96 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 97 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 98 - "compiler"
Cohesion: 0.33
Nodes (4): compiler, TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 99 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 100 - "allocateRetry"
Cohesion: 0.47
Nodes (6): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, create()

### Community 101 - "native_send_test.cpp"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, stubA(), stubB(), trueDnuSentinel()

### Community 102 - "send0"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, LayoutNativeCodeNil, TEST(), send0()

### Community 108 - "stubEq"
Cohesion: 0.67
Nodes (3): CallContext, uint32_t, stubEq()

## Knowledge Gaps
- **452 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `kind` (+447 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 812 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Object.cpp`, `.isHeap`, `.nil`, `LargeInteger.cpp`, `SmallInteger.cpp`, `Stream.cpp`, `String.cpp`, `Heap.cpp`, `TEST`, `Behavior.cpp`, `Float.cpp`, `gtest`, `Heap`, `Geometry.cpp`, `Gc`, `Roots`, `putNative`, `Gc.cpp`, `WellKnown.cpp`, `P3 — Native Dispatch`, `P2-02 class object skeletons`, `Bootstrap.cpp`, `TEST`, `Boot`, `TEST`, `send`, `ClassMethodCache`, `stubA`, `TEST`, `install`, `intern`, `cstdint`, `Array.cpp`, `BlockContext.cpp`, `Roots.cpp`, `ClassDef`, `Root`, `Compile.cpp`, `Send.cpp`, `TEST`, `.isTrue`, `send1`, `allocateRetry`, `native_send_test.cpp`, `send0`, `Root`, `Root`, `Root`, `Root`, `Root`, `stubEq`?**
  _High betweenness centrality (0.474) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Heap`, `Roots`, `P2-02 class object skeletons`, `Oop`?**
  _High betweenness centrality (0.143) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Object.cpp`, `.isHeap`, `.nil`, `LargeInteger.cpp`, `TEST`, `Stream.cpp`, `String.cpp`, `Heap.cpp`, `TEST`, `Float.cpp`, `gtest`, `Gc`, `putNative`, `Gc.cpp`, `TEST`, `WellKnown.cpp`, `P3 — Native Dispatch`, `P2-02 class object skeletons`, `Bootstrap.cpp`, `TEST`, `Boot`, `send`, `ClassMethodCache`, `intern`, `cstdint`, `Array.cpp`, `BlockContext.cpp`, `Send.cpp`, `TEST`, `P1-06: weak slot GC`, `native_send_test.cpp`?**
  _High betweenness centrality (0.089) - this node is a cross-community bridge._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _452 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014084507042253521 - nodes in this community are weakly interconnected._
- **Should `Object.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.13435374149659865 - nodes in this community are weakly interconnected._
- **Should `.isHeap` be split into smaller, more focused modules?**
  _Cohesion score 0.12202380952380952 - nodes in this community are weakly interconnected._
# Graph Report - p5-compiler  (2026-09-22)

## Corpus Check
- cluster-only mode — file stats not available

## Summary
- 1976 nodes · 5153 edges · 110 communities (89 shown, 21 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 445 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `4ff2f374`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- .nil
- .isHeap
- LargeInteger.cpp
- SmallInteger.cpp
- Token
- Parser
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
- cstdint
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
- Bootstrap.cpp
- TEST
- TEST
- Bytecode interpreter
- TEST
- TEST
- Codegen.cpp
- gtest
- ChunkParser.cpp
- P3 — Native Dispatch
- Send.cpp
- TEST
- ChunkAction
- P2-02 class object skeletons
- P6b — vendor file-in
- boxLiteral
- P9 — Integration
- TEST
- Ast
- docs/README.md
- ClassMethodCache
- TEST
- TEST
- NativeMethod
- TEST
- BlockContext.cpp
- Roots.cpp
- TEST
- TEST
- CompiledMethodNatives.cpp
- Gc
- ao_abi.h
- SourceSpan
- TEST
- .isTrue
- allocateRetry
- Boot
- create
- SmokeTests.swift
- TEST
- TEST
- P3-01 Symbol intern
- P9-02: Browser accept
- TEST
- P1-06: weak slot GC
- P2-03 metaclass cycle
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-03: エラー表示と VoiceOver
- Character.cpp
- Scope
- P4-06 Stream / Transcript model
- TEST
- native_send_test.cpp
- ao image save
- TEST
- install
- intern
- send0
- Root
- stubA
- Root
- Root
- Root
- Root
- build.sh
- test.sh
- unique_ptr
- size_t
- deque
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- WellKnown
- WellKnown
- NativeFn
- WellKnown
- Memory maintenance guide
- string
- string_view

## God Nodes (most connected - your core abstractions)
1. `Oop` - 467 edges
2. `WellKnown` - 145 edges
3. `Heap` - 120 edges
4. `Ast` - 65 edges
5. `Roots` - 64 edges
6. `send()` - 58 edges
7. `Parser` - 55 edges
8. `TEST()` - 49 edges
9. `Emitter` - 37 edges
10. `TEST()` - 34 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `マングル表` --references--> `modulo()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/LargeInteger.cpp
- `マングル表` --references--> `ao_Magnitude_greaterOrEqual()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Magnitude.cpp

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

## Communities (110 total, 21 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - ".nil"
Cohesion: 0.07
Nodes (68): int64_t, allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name (+60 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (61): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+53 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "SmallInteger.cpp"
Cohesion: 0.12
Nodes (49): Native selector mangling, マングル表, 規則, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_() (+41 more)

### Community 5 - "Token"
Cohesion: 0.07
Nodes (49): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+41 more)

### Community 6 - "Parser"
Cohesion: 0.15
Nodes (14): Kind, string, string_view, Tok, vector, join(), Parser, cur_ (+6 more)

### Community 7 - "Object.cpp"
Cohesion: 0.13
Nodes (46): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+38 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "Float.cpp"
Cohesion: 0.12
Nodes (41): FlOp, install, natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+33 more)

### Community 10 - "Emitter"
Cohesion: 0.17
Nodes (10): Op, string_view, uint16_t, uint8_t, Emitter, cur_, scope_, utf8Scalar() (+2 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, uint16_t, uint32_t, ObjectHeader, flags, hash (+26 more)

### Community 12 - "TEST"
Cohesion: 0.06
Nodes (34): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+26 more)

### Community 13 - "Literal"
Cohesion: 0.07
Nodes (33): CompileEnv, instVarNames, CompileResult, error, image, ok, int64_t, LitKind (+25 more)

### Community 14 - "putNative"
Cohesion: 0.09
Nodes (30): NativeFn, WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter(), Heap (+22 more)

### Community 15 - "Behavior.cpp"
Cohesion: 0.20
Nodes (28): collectNursery, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes() (+20 more)

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "cstdint"
Cohesion: 0.12
Nodes (12): string, vector, string_view, cstddef, cstdint, memory, CallContext, WellKnown (+4 more)

### Community 18 - "send"
Cohesion: 0.21
Nodes (27): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill(), ao_Collection_includes_() (+19 more)

### Community 19 - "Heap"
Cohesion: 0.08
Nodes (27): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+19 more)

### Community 20 - "Roots"
Cohesion: 0.08
Nodes (22): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handleAt (+14 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (27): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+19 more)

### Community 25 - "String.cpp"
Cohesion: 0.19
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 26 - "Oop"
Cohesion: 0.13
Nodes (18): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+10 more)

### Community 27 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (23): P4 microbench, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior (+15 more)

### Community 28 - "Bootstrap.cpp"
Cohesion: 0.20
Nodes (14): AtPutFindsInternedKey, Bootstrap, chrono, context, cstdio, cstring, gc, GrowKeepsOuterOopAndEntries (+6 more)

### Community 29 - "TEST"
Cohesion: 0.12
Nodes (14): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, runtime, ao executable, ao_runtime library, ao_runtime_tests (+6 more)

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

### Community 34 - "Codegen.cpp"
Cohesion: 0.16
Nodes (17): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), LitKind, string (+9 more)

### Community 35 - "gtest"
Cohesion: 0.16
Nodes (9): ChunkFileIn, CompiledMethod, DoItIsNotEvaluated, format, globals, gtest, InstallsCompiledMethodAndKeepsOldOnError, TEST() (+1 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.22
Nodes (19): atLineEnd(), classify(), string, string_view, uint32_t, vector, firstLine(), isBlank() (+11 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.11
Nodes (18): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling (+10 more)

### Community 38 - "Send.cpp"
Cohesion: 0.17
Nodes (18): InlineCache, cachedClass, cachedMethod, lookup(), allocateMessage(), cacheIndex(), ClassMethodCache::addRoots(), ClassMethodCache::insert() (+10 more)

### Community 39 - "TEST"
Cohesion: 0.11
Nodes (16): BlockWithArgs, Cascade, ParseResult, error, method, ok, parseMethod(), TEST() (+8 more)

### Community 40 - "ChunkAction"
Cohesion: 0.11
Nodes (19): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+11 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.12
Nodes (18): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+10 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "boxLiteral"
Cohesion: 0.23
Nodes (18): allocateRetry(), applyChunks(), applyClassDef(), applyMethodsFor(), boxBytes(), boxLiteral(), boxMethodImage(), boxUtf8() (+10 more)

### Community 44 - "P9 — Integration"
Cohesion: 0.13
Nodes (14): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+6 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.12
Nodes (16): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+8 more)

### Community 47 - "docs/README.md"
Cohesion: 0.28
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 48 - "ClassMethodCache"
Cohesion: 0.12
Nodes (16): HostOopHook, CallContext, cache, inspectHook, transcriptHook, ClassMethodCache, addRoots, entries (+8 more)

### Community 49 - "TEST"
Cohesion: 0.14
Nodes (14): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+6 more)

### Community 50 - "TEST"
Cohesion: 0.14
Nodes (11): CascadeAndBlock, Codegen, compiler, TEST(), TEST(), CompilerSmoke, ErrorSpanOnDanglingBinary, ReturnOnePlusTwo (+3 more)

### Community 51 - "NativeMethod"
Cohesion: 0.15
Nodes (10): AddInternsByFunctionPointer, heap, NameAndApply, NativeMethod, roots, CallContext, uint32_t, stubEq() (+2 more)

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
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), send2() (+5 more)

### Community 57 - "CompiledMethodNatives.cpp"
Cohesion: 0.55
Nodes (12): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+4 more)

### Community 58 - "Gc"
Cohesion: 0.17
Nodes (12): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectOld, copy, failed_, heap_, oldCompacted_ (+4 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "SourceSpan"
Cohesion: 0.17
Nodes (9): CompileError, message, span, string, uint32_t, SourceSpan, end, start (+1 more)

### Community 61 - "TEST"
Cohesion: 0.17
Nodes (12): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, send1(), TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut (+4 more)

### Community 62 - ".isTrue"
Cohesion: 0.41
Nodes (8): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 63 - "allocateRetry"
Cohesion: 0.26
Nodes (10): allocateRetry(), CallContext, Oop, Roots, uint16_t, uint32_t, uint8_t, create() (+2 more)

### Community 64 - "Boot"
Cohesion: 0.17
Nodes (11): formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap, roots (+3 more)

### Community 65 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 66 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (8): BangInCharacterDoesNotSplit, Chunk, ClassDefinitionShape, TEST(), LineEndBangAfterBinaryStillTerminates, MethodImage, SplitsMethodsForAndProtocol, SubclassSendInMethodStaysMethodsFor

### Community 68 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 69 - "P3-01 Symbol intern"
Cohesion: 0.22
Nodes (8): Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary, P3-02 MethodDictionary, Symbol

### Community 70 - "P9-02: Browser accept"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 71 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 73 - "P2-03 metaclass cycle"
Cohesion: 0.25
Nodes (7): class skeleton, metaclass cycle, P2-03 metaclass cycle, Bootstrap, Globals, P2-04 Smalltalk global dictionary, Smalltalk global dictionary

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "Character.cpp"
Cohesion: 0.57
Nodes (7): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), CallContext, uint32_t

### Community 78 - "Scope"
Cohesion: 0.29
Nodes (7): vector, Scope, args, copied, isBlock, parent, temps

### Community 79 - "P4-06 Stream / Transcript model"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, Transcript

### Community 80 - "TEST"
Cohesion: 0.33
Nodes (6): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, TEST(), TenMillionToDo

### Community 81 - "native_send_test.cpp"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, stubA(), stubB(), trueDnuSentinel()

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 84 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 85 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 86 - "send0"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, LayoutNativeCodeNil, TEST(), send0()

### Community 87 - "Root"
Cohesion: 0.67
Nodes (3): Roots, Root, slot

### Community 88 - "stubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, stubA(), stubB()

## Knowledge Gaps
- **451 isolated node(s):** `CallContext`, `AoPlaceholder`, `MethodImage`, `addRoots`, `arrayClass` (+446 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 821 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **21 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.nil`, `.isHeap`, `LargeInteger.cpp`, `SmallInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `Behavior.cpp`, `cstdint`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `String.cpp`, `Bootstrap.cpp`, `TEST`, `TEST`, `gtest`, `P3 — Native Dispatch`, `Send.cpp`, `P2-02 class object skeletons`, `boxLiteral`, `ClassMethodCache`, `TEST`, `NativeMethod`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `TEST`, `Gc`, `TEST`, `.isTrue`, `Boot`, `create`, `TEST`, `P3-01 Symbol intern`, `TEST`, `P2-03 metaclass cycle`, `Character.cpp`, `native_send_test.cpp`, `install`, `intern`, `send0`, `stubA`, `Root`, `Root`, `Root`, `Root`?**
  _High betweenness centrality (0.476) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `P2-02 class object skeletons`, `Oop`, `Heap`, `Roots`?**
  _High betweenness centrality (0.133) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.nil`, `.isHeap`, `LargeInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `cstdint`, `send`, `TEST`, `String.cpp`, `Bootstrap.cpp`, `TEST`, `TEST`, `gtest`, `P3 — Native Dispatch`, `Send.cpp`, `P2-02 class object skeletons`, `ClassMethodCache`, `TEST`, `NativeMethod`, `BlockContext.cpp`, `Gc`, `Boot`, `create`, `P1-06: weak slot GC`, `P2-03 metaclass cycle`, `native_send_test.cpp`, `TEST`, `install`, `intern`?**
  _High betweenness centrality (0.081) - this node is a cross-community bridge._
- **What connects `CallContext`, `AoPlaceholder`, `MethodImage` to the rest of the system?**
  _451 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014084507042253521 - nodes in this community are weakly interconnected._
- **Should `.nil` be split into smaller, more focused modules?**
  _Cohesion score 0.07433489827856025 - nodes in this community are weakly interconnected._
- **Should `.isHeap` be split into smaller, more focused modules?**
  _Cohesion score 0.12202380952380952 - nodes in this community are weakly interconnected._
# Graph Report - p6-interpreter  (2026-09-22)

## Corpus Check
- 193 files · ~63,438 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 63 file(s) not represented in the graph (top: .st 51, (none) 11, .toml 1)

## Summary
- 2154 nodes · 5627 edges · 117 communities (92 shown, 25 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 486 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `8580cc53`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Interpreter.cpp
- .isHeap
- LargeInteger.cpp
- SmallInteger.cpp
- .make
- Parser
- Object.cpp
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- putNative
- .false_
- SPEC.md
- TestRunner.cpp
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
- Bootstrap
- Scanner
- TEST
- Bytecode interpreter
- TEST
- TEST
- Bytecode.hpp
- gtest
- ChunkAction
- P3 — Native Dispatch
- Compile.cpp
- TEST
- cstdint
- P2-02 class object skeletons
- P6b — vendor file-in
- CompileResult
- P9-04: v1 ゴールデン受け入れ
- TEST
- Ast
- docs/README.md
- CallContext
- ClassDef
- TEST
- heap
- TEST
- BlockContext.cpp
- Roots.cpp
- TEST
- TEST
- .advance
- Gc
- ao_abi.h
- SourceSpan
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
- send1
- ao image save
- .isTrue
- .true_
- intern
- send0
- Root
- README.ja.md
- install
- TEST
- Root
- forceSlide
- build.sh
- test.sh
- TEST
- .fail
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- TEST
- Root
- Root
- Memory maintenance guide
- string
- string_view
- Root
- Root
- Root
- Root
- Root
- Root
- gc

## God Nodes (most connected - your core abstractions)
1. `Oop` - 534 edges
2. `WellKnown` - 145 edges
3. `Heap` - 134 edges
4. `Roots` - 90 edges
5. `Ast` - 65 edges
6. `send()` - 64 edges
7. `Parser` - 55 edges
8. `TEST()` - 49 edges
9. `Emitter` - 37 edges
10. `Gc` - 36 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp
- `現状` --references--> `send()`  [INFERRED]
  README.ja.md → runtime/src/Send.cpp

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

## Communities (117 total, 25 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (52): safepoint, ActiveGuard, rootShared, saved, allocateRetry(), applyMethod(), byteCount(), clearNonlocal() (+44 more)

### Community 2 - ".isHeap"
Cohesion: 0.08
Nodes (78): clearWeakAfterOldMark, copy, allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, createBlock() (+70 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (26): Native selector mangling, マングル表, 規則, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_() (+18 more)

### Community 5 - ".make"
Cohesion: 0.09
Nodes (41): cmath, lexBinary, lexCharacter, lexIdentOrKeyword, lexString, lexSymbol, skipTrivia, int64_t (+33 more)

### Community 6 - "Parser"
Cohesion: 0.16
Nodes (11): deque, string_view, parseMethod(), Parser, cur_, error_, hadError_, prev_ (+3 more)

### Community 7 - "Object.cpp"
Cohesion: 0.13
Nodes (46): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+38 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "Float.cpp"
Cohesion: 0.21
Nodes (27): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+19 more)

### Community 10 - "Emitter"
Cohesion: 0.21
Nodes (7): int16_t, MethodImage, Op, uint16_t, Emitter, cur_, scope_

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (35): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+27 more)

### Community 12 - "TEST"
Cohesion: 0.06
Nodes (34): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+26 more)

### Community 13 - "Literal"
Cohesion: 0.09
Nodes (26): CompileEnv, instVarNames, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr (+18 more)

### Community 14 - "putNative"
Cohesion: 0.09
Nodes (31): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+23 more)

### Community 15 - ".false_"
Cohesion: 0.21
Nodes (27): allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "TestRunner.cpp"
Cohesion: 0.11
Nodes (24): climits, dyld, filesystem, fstream, interpreter, iterator, path, addRoots (+16 more)

### Community 18 - "send"
Cohesion: 0.08
Nodes (81): CompiledMethod accessors (P5), int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill() (+73 more)

### Community 19 - "Heap"
Cohesion: 0.06
Nodes (38): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+30 more)

### Community 20 - "Roots"
Cohesion: 0.08
Nodes (23): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handleAt (+15 more)

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
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 25 - "String.cpp"
Cohesion: 0.19
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 26 - "Oop"
Cohesion: 0.16
Nodes (14): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+6 more)

### Community 27 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 28 - "Bootstrap"
Cohesion: 0.12
Nodes (23): AtPutFindsInternedKey, Bootstrap, chrono, cstdio, GrowKeepsOuterOopAndEntries, Lookup, MethodDictionary, WellKnown (+15 more)

### Community 29 - "Scanner"
Cohesion: 0.15
Nodes (13): ArrayAndByteArrayHeaders, AssignVariantsAndComment, uint32_t, Scanner, i_, lexNumber, make, next (+5 more)

### Community 30 - "TEST"
Cohesion: 0.09
Nodes (27): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, klass (+19 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "TEST"
Cohesion: 0.11
Nodes (19): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+11 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "Bytecode.hpp"
Cohesion: 0.26
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 35 - "gtest"
Cohesion: 0.17
Nodes (11): compile, CompiledMethod, compiler, TEST(), CompilerSmoke, context, cstring, globals (+3 more)

### Community 36 - "ChunkAction"
Cohesion: 0.09
Nodes (38): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+30 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.07
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 38 - "Compile.cpp"
Cohesion: 0.18
Nodes (23): CompileError, message, span, string, largeinteger, collectNursery, allocateRetry(), applyChunks() (+15 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (12): BlockWithArgs, Cascade, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma (+4 more)

### Community 40 - "cstdint"
Cohesion: 0.11
Nodes (13): AddInternsByFunctionPointer, string, vector, string_view, cstddef, cstdint, memory, NameAndApply (+5 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.15
Nodes (17): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+9 more)

### Community 43 - "CompileResult"
Cohesion: 0.29
Nodes (7): CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 44 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.10
Nodes (20): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+12 more)

### Community 47 - "docs/README.md"
Cohesion: 0.14
Nodes (23): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+15 more)

### Community 48 - "CallContext"
Cohesion: 0.09
Nodes (23): HostOopHook, CallContext, activeContext, cache, inspectHook, interpretedBytecodes, nonlocalHome, nonlocalReturn (+15 more)

### Community 49 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 50 - "TEST"
Cohesion: 0.18
Nodes (9): CascadeAndBlock, Codegen, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), EighteenDigitIntegerSurvivesAsInt, ErrorSpanOnDanglingBinary, ReturnOnePlusTwo, SuperSendAndGlobal (+1 more)

### Community 52 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

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

### Community 58 - "Gc"
Cohesion: 0.16
Nodes (14): Gc, clearWeakAfterNursery, failed_, heap_, oldCompacted_, roots_, scavengeFromRoots, allocateRetry() (+6 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "SourceSpan"
Cohesion: 0.20
Nodes (6): uint32_t, SourceSpan, end, start, string, vector

### Community 61 - "TEST"
Cohesion: 0.18
Nodes (11): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut, TranscriptModel (+3 more)

### Community 62 - "TEST"
Cohesion: 0.13
Nodes (14): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, runtime, ao executable, ao_runtime library, ao_runtime_tests (+6 more)

### Community 64 - "Boot"
Cohesion: 0.15
Nodes (13): rect(), formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap (+5 more)

### Community 65 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 66 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (8): BangInCharacterDoesNotSplit, Chunk, ClassDefinitionShape, TEST(), LineEndBangAfterBinaryStillTerminates, MethodImage, SplitsMethodsForAndProtocol, SubclassSendInMethodStaysMethodsFor

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
Cohesion: 0.11
Nodes (22): deque, size_t, string, string_view, Entry, cls, name, NamedClass (+14 more)

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
Cohesion: 0.14
Nodes (16): LitKind, string, vector, disassemble(), formatLit(), internable(), opName(), Scope (+8 more)

### Community 79 - "install"
Cohesion: 0.33
Nodes (11): install, natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+3 more)

### Community 80 - "TEST"
Cohesion: 0.33
Nodes (6): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, TEST(), TenMillionToDo

### Community 81 - "send1"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), TEST() (+1 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - ".isTrue"
Cohesion: 0.53
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 84 - ".true_"
Cohesion: 0.42
Nodes (7): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), CallContext, uint32_t

### Community 85 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 86 - "send0"
Cohesion: 0.40
Nodes (5): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST(), send0()

### Community 88 - "README.ja.md"
Cohesion: 0.09
Nodes (19): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+11 more)

### Community 89 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 90 - "TEST"
Cohesion: 0.22
Nodes (9): AoTestRunner, ArgumentAndOuterTemp, BlockEval, CopyDoesNotWriteOuter, ExampleFilePasses, IfTrueIfFalseFromCompiledMethod, NonLocalReturnSkipsRest, TEST() (+1 more)

### Community 92 - "forceSlide"
Cohesion: 0.53
Nodes (6): collectOld, CallContext, uint32_t, forceNursery(), forceOld(), forceSlide()

### Community 95 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 96 - ".fail"
Cohesion: 0.35
Nodes (3): size_t, string_view, uint8_t

### Community 97 - "TEST"
Cohesion: 0.40
Nodes (5): ChunkFileIn, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, TEST()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.35
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 104 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

## Knowledge Gaps
- **497 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `kind` (+492 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 903 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **25 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Interpreter.cpp`, `.isHeap`, `LargeInteger.cpp`, `SmallInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `.false_`, `TestRunner.cpp`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `String.cpp`, `Bootstrap`, `TEST`, `gtest`, `P3 — Native Dispatch`, `Compile.cpp`, `cstdint`, `P2-02 class object skeletons`, `CallContext`, `ClassDef`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `Gc`, `Boot`, `create`, `Boolean.cpp`, `TEST`, `WellKnown.cpp`, `TEST`, `install`, `send1`, `.isTrue`, `.true_`, `intern`, `send0`, `Root`, `README.ja.md`, `install`, `Root`, `forceSlide`, `Bootstrap.cpp`, `Root`, `Root`, `Root`, `Root`, `Root`, `Root`, `Root`, `Root`?**
  _High betweenness centrality (0.479) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `cstdint`, `Heap`, `Roots`, `.true_`, `Oop`?**
  _High betweenness centrality (0.120) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `TestRunner.cpp`, `send`, `Roots`, `TEST`, `String.cpp`, `Bootstrap`, `TEST`, `gtest`, `P3 — Native Dispatch`, `cstdint`, `P2-02 class object skeletons`, `CallContext`, `BlockContext.cpp`, `Gc`, `Boot`, `create`, `P1-06: weak slot GC`, `WellKnown.cpp`, `install`, `intern`, `install`, `Bootstrap.cpp`, `TEST`?**
  _High betweenness centrality (0.102) - this node is a cross-community bridge._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _497 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014285714285714285 - nodes in this community are weakly interconnected._
- **Should `Interpreter.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.06377204884667571 - nodes in this community are weakly interconnected._
- **Should `.isHeap` be split into smaller, more focused modules?**
  _Cohesion score 0.07899159663865546 - nodes in this community are weakly interconnected._
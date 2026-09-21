# Graph Report - p5-compiler  (2026-09-22)

## Corpus Check
- 182 files · ~58,189 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 61 file(s) not represented in the graph (top: .st 50, (none) 10, .toml 1)

## Summary
- 1972 nodes · 5172 edges · 97 communities (81 shown, 16 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 446 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `dbfb50c2`
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
- Gc.cpp
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
- string_view
- native_send_test.cpp
- Bytecode interpreter
- TEST
- TEST
- Bytecode.hpp
- gtest
- Compile.cpp
- P3 — Native Dispatch
- InlineCache
- TEST
- P8 — AppKit Tools
- gc
- P6b — vendor file-in
- CompileResult
- P9-04: v1 ゴールデン受け入れ
- TEST
- Ast
- docs/README.md
- ClassMethodCache
- ClassDef
- TEST
- heap
- TEST
- BlockContext.cpp
- Roots.cpp
- TEST
- TEST
- Gc::collectOld
- Gc
- ao_abi.h
- SourceSpan
- TEST
- .isTrue
- roots
- Boot
- create
- SmokeTests.swift
- context
- wellknown
- P9-02: Browser accept
- TEST
- P1-06: weak slot GC
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-03: エラー表示と VoiceOver
- Codegen.cpp
- TEST
- ao image save
- intern
- send0
- Root
- TEST
- Root
- Root
- build.sh
- test.sh
- .fail
- ParseResult
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- Memory maintenance guide
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

## Communities (97 total, 16 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - ".nil"
Cohesion: 0.10
Nodes (52): int64_t, at(), string_view, WellKnown, install(), ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver() (+44 more)

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
Cohesion: 0.08
Nodes (41): cmath, lexBinary, lexCharacter, lexIdentOrKeyword, lexNumber, lexString, lexSymbol, skipTrivia (+33 more)

### Community 6 - "Parser"
Cohesion: 0.18
Nodes (11): Kind, Tok, join(), Parser, cur_, error_, hadError_, prev_ (+3 more)

### Community 7 - "Object.cpp"
Cohesion: 0.13
Nodes (46): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+38 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "Float.cpp"
Cohesion: 0.08
Nodes (44): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, cstdio, cstring, FlOp, runtime (+36 more)

### Community 10 - "Emitter"
Cohesion: 0.25
Nodes (6): Op, uint16_t, Emitter, cur_, scope_, int16_t

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, uint16_t, uint32_t, ObjectHeader, flags, hash (+26 more)

### Community 12 - "TEST"
Cohesion: 0.06
Nodes (34): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+26 more)

### Community 13 - "Literal"
Cohesion: 0.09
Nodes (25): CompileEnv, instVarNames, int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr (+17 more)

### Community 14 - "putNative"
Cohesion: 0.08
Nodes (47): CompiledMethod accessors (P5), install, WellKnown, installBehavior(), WellKnown, installBoolean(), ao_Character_asCharacter(), ao_Character_asciiValue() (+39 more)

### Community 15 - "Behavior.cpp"
Cohesion: 0.20
Nodes (27): allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "Gc.cpp"
Cohesion: 0.13
Nodes (12): algorithm, nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*) (+4 more)

### Community 18 - "send"
Cohesion: 0.24
Nodes (25): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill(), ao_Collection_includes_() (+17 more)

### Community 19 - "Heap"
Cohesion: 0.06
Nodes (38): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+30 more)

### Community 20 - "Roots"
Cohesion: 0.06
Nodes (30): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handleAt (+22 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (30): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+22 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.15
Nodes (18): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+10 more)

### Community 24 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (27): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send (+19 more)

### Community 25 - "String.cpp"
Cohesion: 0.10
Nodes (43): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, natives, ao_ArrayedCollection_at_() (+35 more)

### Community 26 - "Oop"
Cohesion: 0.13
Nodes (26): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), Oop (+18 more)

### Community 27 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (23): P4 microbench, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior (+15 more)

### Community 28 - "Bootstrap"
Cohesion: 0.09
Nodes (23): AddInternsByFunctionPointer, AtPutFindsInternedKey, Bootstrap, chrono, GrowKeepsOuterOopAndEntries, InternSameBytesIsIdentical, InternSurvivesNurseryGc, Lookup (+15 more)

### Community 29 - "string_view"
Cohesion: 0.16
Nodes (13): ArrayAndByteArrayHeaders, AssignVariantsAndComment, string_view, uint32_t, Scanner, i_, make, next (+5 more)

### Community 30 - "native_send_test.cpp"
Cohesion: 0.16
Nodes (16): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, slotAtPut, CallContext, NativeFn, uint32_t (+8 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "TEST"
Cohesion: 0.09
Nodes (26): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+18 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "Bytecode.hpp"
Cohesion: 0.26
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 35 - "gtest"
Cohesion: 0.18
Nodes (8): compile, CompiledMethod, compiler, TEST(), CompilerSmoke, globals, gtest, VersionIsNonEmpty

### Community 36 - "Compile.cpp"
Cohesion: 0.05
Nodes (66): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+58 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (22): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+14 more)

### Community 38 - "InlineCache"
Cohesion: 0.40
Nodes (5): InlineCache, cachedClass, cachedMethod, IcGuard, ic

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (12): BlockWithArgs, Cascade, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma (+4 more)

### Community 40 - "P8 — AppKit Tools"
Cohesion: 0.24
Nodes (10): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P7 — Image, .aoimage save/load, NativeMethod symbol rebind on load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+2 more)

### Community 41 - "gc"
Cohesion: 0.07
Nodes (29): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+21 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 43 - "CompileResult"
Cohesion: 0.29
Nodes (7): CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 44 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.11
Nodes (19): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+11 more)

### Community 47 - "docs/README.md"
Cohesion: 0.22
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 48 - "ClassMethodCache"
Cohesion: 0.10
Nodes (20): MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling, HostOopHook, CallContext, cache, inspectHook (+12 more)

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
Cohesion: 0.26
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 54 - "Roots.cpp"
Cohesion: 0.20
Nodes (11): walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle(), Roots::remove() (+3 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 56 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 57 - "Gc::collectOld"
Cohesion: 0.40
Nodes (4): Gc::clearWeakAfterNursery(), Gc::collectOld(), Gc::copy(), Gc::scavengeFromRoots()

### Community 58 - "Gc"
Cohesion: 0.17
Nodes (12): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectOld, copy, failed_, heap_, oldCompacted_ (+4 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "SourceSpan"
Cohesion: 0.22
Nodes (6): uint32_t, SourceSpan, end, start, string, vector

### Community 61 - "TEST"
Cohesion: 0.18
Nodes (11): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut, TranscriptModel (+3 more)

### Community 62 - ".isTrue"
Cohesion: 0.41
Nodes (8): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 64 - "Boot"
Cohesion: 0.11
Nodes (20): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+12 more)

### Community 65 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 66 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 67 - "context"
Cohesion: 0.20
Nodes (9): BangInCharacterDoesNotSplit, Chunk, ClassDefinitionShape, TEST(), context, LineEndBangAfterBinaryStillTerminates, MethodImage, SplitsMethodsForAndProtocol (+1 more)

### Community 70 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 71 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 78 - "Codegen.cpp"
Cohesion: 0.11
Nodes (18): LitKind, MethodImage, string, vector, disassemble(), formatLit(), internable(), opName() (+10 more)

### Community 80 - "TEST"
Cohesion: 0.33
Nodes (6): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, TEST(), TenMillionToDo

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 85 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 86 - "send0"
Cohesion: 0.20
Nodes (10): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, ChunkFileIn, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, LayoutNativeCodeNil, TEST() (+2 more)

### Community 88 - "TEST"
Cohesion: 0.17
Nodes (13): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, CallContext, NativeFn, uint32_t, WellKnown, install() (+5 more)

### Community 96 - ".fail"
Cohesion: 0.33
Nodes (3): size_t, string_view, uint8_t

### Community 97 - "ParseResult"
Cohesion: 0.20
Nodes (7): ParseResult, error, method, ok, deque, string_view, parseMethod()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.35
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

## Knowledge Gaps
- **453 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `kind` (+448 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 823 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **16 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.nil`, `.isHeap`, `LargeInteger.cpp`, `SmallInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `Behavior.cpp`, `Gc.cpp`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `String.cpp`, `Bootstrap`, `native_send_test.cpp`, `TEST`, `Compile.cpp`, `P3 — Native Dispatch`, `InlineCache`, `gc`, `Ast`, `ClassMethodCache`, `ClassDef`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `Gc::collectOld`, `Gc`, `.isTrue`, `Boot`, `create`, `TEST`, `intern`, `send0`, `Root`, `TEST`, `Root`, `Root`, `Bootstrap.cpp`?**
  _High betweenness centrality (0.453) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `gc`, `Oop`, `Heap`, `Roots`?**
  _High betweenness centrality (0.133) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.nil`, `.isHeap`, `LargeInteger.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `putNative`, `Gc.cpp`, `send`, `TEST`, `String.cpp`, `Oop`, `Bootstrap`, `native_send_test.cpp`, `TEST`, `gtest`, `P3 — Native Dispatch`, `gc`, `ClassMethodCache`, `BlockContext.cpp`, `Gc`, `Boot`, `create`, `P1-06: weak slot GC`, `intern`, `Bootstrap.cpp`?**
  _High betweenness centrality (0.076) - this node is a cross-community bridge._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _453 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014084507042253521 - nodes in this community are weakly interconnected._
- **Should `.nil` be split into smaller, more focused modules?**
  _Cohesion score 0.09523809523809523 - nodes in this community are weakly interconnected._
- **Should `.isHeap` be split into smaller, more focused modules?**
  _Cohesion score 0.11875 - nodes in this community are weakly interconnected._
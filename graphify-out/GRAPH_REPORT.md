# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- cluster-only mode — file stats not available

## Summary
- 1589 nodes · 4160 edges · 77 communities (63 shown, 14 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 389 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `4e0a725d`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Object.cpp
- .isHeap
- send
- Oop
- LargeInteger.cpp
- マングル表
- TEST
- Stream.cpp
- String.cpp
- Heap.cpp
- install
- TEST
- .false_
- Symbol
- Float.cpp
- test_support.hpp
- Heap
- P4 — Kernel Native Implementation
- Geometry.cpp
- Gc
- Roots
- putNative
- SPEC.md
- vector
- TEST
- WellKnown.cpp
- P3 — Native Dispatch
- bootstrap_test.cpp
- P6b — vendor file-in
- P4-03 Magnitude / SmallInteger / Character
- Bytecode interpreter
- Bootstrap.cpp
- TEST
- docs/README.md
- TEST
- native_send_test.cpp
- Boot
- TEST
- TEST
- TEST
- TEST
- ao_abi.h
- SmallInteger.cpp
- WellKnown
- SmokeTests.swift
- P9 — Integration
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-02: Browser accept
- Bootstrap
- TEST
- TEST
- ao image save
- .true_
- intern
- build.sh
- test.sh
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- context
- BlockContext.cpp
- Roots.cpp
- Memory maintenance guide
- wellknown
- create
- ClassDef
- P1-07: immovable old objects
- P4-05 Dictionary / Set / OrderedCollection
- Root
- Root
- Root

## God Nodes (most connected - your core abstractions)
1. `Oop` - 474 edges
2. `WellKnown` - 145 edges
3. `Heap` - 131 edges
4. `Roots` - 69 edges
5. `send()` - 57 edges
6. `TEST()` - 49 edges
7. `TEST()` - 34 edges
8. `parse()` - 26 edges
9. `TEST()` - 25 edges
10. `box()` - 25 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `ao_Object_class()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Object.cpp
- `マングル表` --references--> `ao_Object_doesNotUnderstand_()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Object.cpp
- `マングル表` --references--> `ao_Object_equals()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Object.cpp

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

## Communities (77 total, 14 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - "Object.cpp"
Cohesion: 0.13
Nodes (46): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+38 more)

### Community 2 - ".isHeap"
Cohesion: 0.09
Nodes (72): clearWeakAfterOldMark, collectOld, copy, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc() (+64 more)

### Community 3 - "send"
Cohesion: 0.09
Nodes (69): int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_() (+61 more)

### Community 4 - "Oop"
Cohesion: 0.14
Nodes (21): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+13 more)

### Community 5 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 6 - "マングル表"
Cohesion: 0.27
Nodes (24): マングル表, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_() (+16 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (34): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+26 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 9 - "String.cpp"
Cohesion: 0.06
Nodes (45): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio (+37 more)

### Community 10 - "Heap.cpp"
Cohesion: 0.09
Nodes (35): cassert, fitsOld, header, uint16_t, uint32_t, ObjectHeader, flags, hash (+27 more)

### Community 11 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 12 - "TEST"
Cohesion: 0.09
Nodes (26): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+18 more)

### Community 13 - ".false_"
Cohesion: 0.20
Nodes (28): collectNursery, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes() (+20 more)

### Community 14 - "Symbol"
Cohesion: 0.15
Nodes (12): AddInternsByFunctionPointer, InternSameBytesIsIdentical, InternSurvivesNurseryGc, NameAndApply, inOld, add, CallContext, uint32_t (+4 more)

### Community 15 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 16 - "test_support.hpp"
Cohesion: 0.17
Nodes (9): chrono, cstdint, format, globals, gtest, NativeMethod, send, string (+1 more)

### Community 17 - "Heap"
Cohesion: 0.06
Nodes (38): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+30 more)

### Community 18 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (28): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+20 more)

### Community 19 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 20 - "Gc"
Cohesion: 0.12
Nodes (16): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+8 more)

### Community 21 - "Roots"
Cohesion: 0.07
Nodes (25): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+17 more)

### Community 22 - "putNative"
Cohesion: 0.10
Nodes (28): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+20 more)

### Community 23 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 24 - "vector"
Cohesion: 0.20
Nodes (8): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), vector

### Community 25 - "TEST"
Cohesion: 0.07
Nodes (30): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+22 more)

### Community 26 - "WellKnown.cpp"
Cohesion: 0.11
Nodes (22): deque, size_t, string, string_view, Entry, cls, name, NamedClass (+14 more)

### Community 27 - "P3 — Native Dispatch"
Cohesion: 0.07
Nodes (28): Native selector mangling, 規則, P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop (+20 more)

### Community 28 - "bootstrap_test.cpp"
Cohesion: 0.07
Nodes (29): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+21 more)

### Community 29 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 30 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (23): P4 microbench, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior (+15 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "Bootstrap.cpp"
Cohesion: 0.39
Nodes (11): allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName(), run() (+3 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "docs/README.md"
Cohesion: 0.15
Nodes (27): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator (+19 more)

### Community 35 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 36 - "native_send_test.cpp"
Cohesion: 0.16
Nodes (16): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, slotAtPut, CallContext, NativeFn, uint32_t (+8 more)

### Community 37 - "Boot"
Cohesion: 0.11
Nodes (20): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+12 more)

### Community 38 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 40 - "TEST"
Cohesion: 0.17
Nodes (12): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, send0(), TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut (+4 more)

### Community 41 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 42 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 43 - "SmallInteger.cpp"
Cohesion: 0.27
Nodes (23): allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_intDivide() (+15 more)

### Community 44 - "WellKnown"
Cohesion: 0.10
Nodes (18): HostOopHook, CallContext, CallContext, cache, inspectHook, transcriptHook, ClassMethodCache, addRoots (+10 more)

### Community 45 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 46 - "P9 — Integration"
Cohesion: 0.09
Nodes (22): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+14 more)

### Community 47 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 48 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 49 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 50 - "Bootstrap"
Cohesion: 0.13
Nodes (19): AtPutFindsInternedKey, Bootstrap, BootstrapInstallsObjectIdentityEquals, GrowKeepsOuterOopAndEntries, InheritsFromSuperclass, Lookup, MethodDictionary, MissingSelectorIsNil (+11 more)

### Community 51 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 52 - "TEST"
Cohesion: 0.33
Nodes (6): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, TEST(), TenMillionToDo

### Community 53 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 54 - ".true_"
Cohesion: 0.42
Nodes (7): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), CallContext, uint32_t

### Community 55 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 65 - "context"
Cohesion: 0.27
Nodes (12): context, install, natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_() (+4 more)

### Community 66 - "BlockContext.cpp"
Cohesion: 0.27
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 67 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 70 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 71 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 72 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 73 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.40
Nodes (5): Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

## Knowledge Gaps
- **375 isolated node(s):** `CallContext`, `AoPlaceholder`, `addRoots`, `arrayClass`, `arrayedCollectionClass` (+370 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 666 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **14 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Object.cpp`, `.isHeap`, `send`, `LargeInteger.cpp`, `マングル表`, `Stream.cpp`, `String.cpp`, `Heap.cpp`, `install`, `TEST`, `.false_`, `Symbol`, `Float.cpp`, `test_support.hpp`, `Heap`, `Geometry.cpp`, `Gc`, `Roots`, `putNative`, `vector`, `TEST`, `WellKnown.cpp`, `P3 — Native Dispatch`, `bootstrap_test.cpp`, `Bootstrap.cpp`, `native_send_test.cpp`, `Boot`, `TEST`, `TEST`, `SmallInteger.cpp`, `WellKnown`, `Bootstrap`, `.true_`, `intern`, `context`, `BlockContext.cpp`, `Roots.cpp`, `create`, `ClassDef`, `Root`, `Root`, `Root`?**
  _High betweenness centrality (0.518) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `Heap`, `Roots`, `.true_`, `bootstrap_test.cpp`?**
  _High betweenness centrality (0.138) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Object.cpp`, `.isHeap`, `send`, `LargeInteger.cpp`, `TEST`, `Stream.cpp`, `String.cpp`, `Heap.cpp`, `install`, `TEST`, `Symbol`, `Float.cpp`, `test_support.hpp`, `Gc`, `putNative`, `vector`, `TEST`, `WellKnown.cpp`, `P3 — Native Dispatch`, `bootstrap_test.cpp`, `Bootstrap.cpp`, `native_send_test.cpp`, `Boot`, `WellKnown`, `Bootstrap`, `intern`, `context`, `BlockContext.cpp`, `create`?**
  _High betweenness centrality (0.090) - this node is a cross-community bridge._
- **Are the 49 inferred relationships involving `send()` (e.g. with `installNatives()` and `ao_ArrayedCollection_new_()`) actually correct?**
  _`send()` has 49 INFERRED edges - model-reasoned connections that need verification._
- **What connects `CallContext`, `AoPlaceholder`, `addRoots` to the rest of the system?**
  _375 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014285714285714285 - nodes in this community are weakly interconnected._
- **Should `Object.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.1341350601295097 - nodes in this community are weakly interconnected._
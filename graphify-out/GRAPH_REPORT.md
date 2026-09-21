# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 149 files · ~42,106 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 46 file(s) not represented in the graph (top: .st 35, (none) 10, .toml 1)

## Summary
- 1435 nodes · 3510 edges · 81 communities (67 shown, 14 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 319 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `9cb74ac9`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- .nil
- LargeInteger.cpp
- Float.cpp
- Heap.cpp
- send
- TEST
- SPEC.md
- docs/README.md
- Bytecode interpreter
- SmallInteger.cpp
- Roots
- P0 phase doc
- WellKnown.cpp
- TEST
- TEST
- P6b — vendor file-in
- TEST
- cstdint
- TEST
- Heap
- String.cpp
- SmokeTests.swift
- BlockContext.cpp
- Oop
- ClassMethodCache
- P2-02 class object skeletons
- TEST
- Dictionary.cpp
- Boot
- Gc.cpp
- ao_abi.h
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- Roots.cpp
- TEST
- TEST
- TEST
- Gc
- test_support.hpp
- P9-04: v1 ゴールデン受け入れ
- P9 — Integration
- P1-06: weak slot GC
- create
- P4 — Kernel Native Implementation
- P4-09 Kernel NativeMethod scan and bench
- vector
- TEST
- Root
- ClassDef
- intern
- P9-02: Browser accept
- ao image save
- Root
- P9-03: エラー表示と VoiceOver
- Root
- P4-08 Point / Rectangle
- create
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- P1-07: immovable old objects
- Memory maintenance guide
- wellknown
- P4-01 Object / UndefinedObject / Boolean
- P4-03 Magnitude / SmallInteger / Character
- P4-04 Array / ByteArray / String / Symbol
- P4-02 Behavior / ClassDescription / Class / Metaclass
- install
- TEST
- stubEq
- build.sh
- test.sh
- NamedClass

## God Nodes (most connected - your core abstractions)
1. `Oop` - 387 edges
2. `WellKnown` - 144 edges
3. `Heap` - 121 edges
4. `Roots` - 63 edges
5. `TEST()` - 49 edges
6. `send()` - 42 edges
7. `TEST()` - 34 edges
8. `parse()` - 26 edges
9. `box()` - 25 edges
10. `TEST()` - 25 edges

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

## Communities (81 total, 14 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (137): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+129 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 2 - ".nil"
Cohesion: 0.31
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "Float.cpp"
Cohesion: 0.21
Nodes (27): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+19 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.08
Nodes (36): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+28 more)

### Community 6 - "send"
Cohesion: 0.10
Nodes (49): int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_() (+41 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (31): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+23 more)

### Community 8 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 9 - "docs/README.md"
Cohesion: 0.13
Nodes (23): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P2 — Bootstrap, Blue Book 6–10 (+15 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "SmallInteger.cpp"
Cohesion: 0.12
Nodes (50): Native selector mangling, マングル表, 規則, largeinteger, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+42 more)

### Community 12 - "Roots"
Cohesion: 0.09
Nodes (22): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+14 more)

### Community 13 - "P0 phase doc"
Cohesion: 0.28
Nodes (13): P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+5 more)

### Community 14 - "WellKnown.cpp"
Cohesion: 0.13
Nodes (19): deque, size_t, string, string_view, Entry, cls, name, WellKnown::addRoots() (+11 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 16 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 18 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+7 more)

### Community 19 - "cstdint"
Cohesion: 0.10
Nodes (11): cstddef, cstdint, HostOopHook, memory, CallContext, CallContext, cache, inspectHook (+3 more)

### Community 20 - "TEST"
Cohesion: 0.09
Nodes (24): AddInternsByFunctionPointer, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+16 more)

### Community 21 - "Heap"
Cohesion: 0.06
Nodes (46): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+38 more)

### Community 22 - "String.cpp"
Cohesion: 0.06
Nodes (45): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio (+37 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.18
Nodes (8): Ao, AoPlaceholder, SmokeTests, Phase P0 skeleton, Phase pipeline P0-P9, PackageDescription, XCTest, XCTestCase

### Community 24 - "BlockContext.cpp"
Cohesion: 0.27
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 25 - "Oop"
Cohesion: 0.06
Nodes (108): install, natives, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers() (+100 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+3 more)

### Community 27 - "P2-02 class object skeletons"
Cohesion: 0.14
Nodes (14): Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, Behavior, Bootstrap, Class (+6 more)

### Community 28 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+6 more)

### Community 29 - "Dictionary.cpp"
Cohesion: 0.12
Nodes (60): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+52 more)

### Community 30 - "Boot"
Cohesion: 0.15
Nodes (12): formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap, roots (+4 more)

### Community 31 - "Gc.cpp"
Cohesion: 0.19
Nodes (11): clearWeakAfterOldMark, collectOld, copy, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc() (+3 more)

### Community 32 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 33 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 34 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 35 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 36 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 37 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 38 - "TEST"
Cohesion: 0.18
Nodes (11): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+3 more)

### Community 39 - "TEST"
Cohesion: 0.14
Nodes (16): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, slotAtPut, CallContext, NativeFn, uint32_t (+8 more)

### Community 40 - "Gc"
Cohesion: 0.22
Nodes (9): Gc, clearWeakAfterNursery, failed_, heap_, oldCompacted_, roots_, safepoint, scavengeFromRoots (+1 more)

### Community 41 - "test_support.hpp"
Cohesion: 0.17
Nodes (15): AtPutFindsInternedKey, Bootstrap, context, gc, globals, GrowKeepsOuterOopAndEntries, gtest, Lookup (+7 more)

### Community 42 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 43 - "P9 — Integration"
Cohesion: 0.12
Nodes (14): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+6 more)

### Community 44 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 45 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 46 - "P4 — Kernel Native Implementation"
Cohesion: 0.25
Nodes (8): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 47 - "P4-09 Kernel NativeMethod scan and bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 48 - "vector"
Cohesion: 0.20
Nodes (8): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), vector

### Community 49 - "TEST"
Cohesion: 0.15
Nodes (15): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, slotAt, superOf(), CallContext, NativeFn, uint32_t (+7 more)

### Community 51 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 52 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 53 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 56 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 58 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 59 - "create"
Cohesion: 0.33
Nodes (6): at(), atPut(), uint32_t, WellKnown, create(), growInner()

### Community 60 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 66 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 69 - "P4-01 Object / UndefinedObject / Boolean"
Cohesion: 0.33
Nodes (6): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send

### Community 70 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.33
Nodes (6): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger

### Community 71 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 72 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.40
Nodes (5): Behavior, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 73 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 74 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 75 - "stubEq"
Cohesion: 0.67
Nodes (3): CallContext, uint32_t, stubEq()

### Community 80 - "NamedClass"
Cohesion: 0.67
Nodes (3): NamedClass, name, WellKnown

## Knowledge Gaps
- **376 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+371 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 630 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **14 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `.nil`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `SmallInteger.cpp`, `Roots`, `WellKnown.cpp`, `TEST`, `cstdint`, `TEST`, `Heap`, `String.cpp`, `BlockContext.cpp`, `ClassMethodCache`, `TEST`, `Dictionary.cpp`, `Boot`, `Gc.cpp`, `Roots.cpp`, `TEST`, `TEST`, `Gc`, `test_support.hpp`, `create`, `vector`, `TEST`, `Root`, `ClassDef`, `intern`, `Root`, `Root`, `create`, `install`, `stubEq`, `NamedClass`?**
  _High betweenness centrality (0.476) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `.nil`, `Roots`, `cstdint`, `Heap`, `Oop`?**
  _High betweenness centrality (0.173) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `.nil`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `TEST`, `WellKnown.cpp`, `TEST`, `cstdint`, `TEST`, `String.cpp`, `BlockContext.cpp`, `Oop`, `Dictionary.cpp`, `Boot`, `Gc.cpp`, `TEST`, `TEST`, `Gc`, `test_support.hpp`, `P1-06: weak slot GC`, `create`, `TEST`, `intern`, `create`, `install`, `TEST`?**
  _High betweenness centrality (0.124) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _376 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014285714285714285 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.082010582010582 - nodes in this community are weakly interconnected._
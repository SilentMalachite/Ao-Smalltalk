# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 133 files · ~23,201 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 11 file(s) not represented in the graph (top: (none) 10, .toml 1)

## Summary
- 944 nodes · 1610 edges · 75 communities (64 shown, 11 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 120 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `e1d75594`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- SPEC.md
- Oop
- P6b — vendor file-in
- Heap
- TEST
- P2 — Bootstrap
- WellKnown
- CallContext
- P3 — Native Dispatch
- gtest
- Bytecode interpreter
- TEST
- Roots
- Heap.cpp
- Array and String Kernel
- Roots.cpp
- oldWalkVisits
- TEST
- P1 — Object Memory
- AppKit App Windows
- Heap Allocation Tests
- lookup_test.cpp
- Image Save and Load
- Build Script
- Test Script
- WellKnown.cpp
- Gc.cpp
- WellKnown Module
- Object and Boolean Kernel
- Stream and Transcript Kernel
- TEST
- gc_test.cpp
- Magnitude and SmallInteger Kernel
- ao_abi.h
- native_send_test.cpp
- Object Kernel Source
- Behavior Kernel Source
- Memory Maintenance Guide
- Process and Semaphore Kernel
- Heap Module
- Oop Module
- create
- Collection Kernel Classes
- Swift Package Smoke
- Point and Rectangle Kernel
- Kernel Scan and Bench
- docs/README.md
- P4 — Kernel Native Implementation
- NativeMethod
- Behavior and Class Kernel
- P8 — AppKit Tools
- Symbol
- .fromSmallInteger
- at
- test_support.hpp
- .nil
- intern
- P9-01: Do it / Print it / Inspect it
- CLAUDE.md
- ClassMethodCache
- header
- TEST
- WellKnown
- P2-02 class object skeletons
- bootstrap_test.cpp
- Install.cpp
- P9-02: Browser accept
- P9-03: エラー表示と VoiceOver
- P9-04: v1 ゴールデン受け入れ
- Format.hpp
- ObjectHeader
- ClassDef

## God Nodes (most connected - your core abstractions)
1. `WellKnown` - 141 edges
2. `Oop` - 129 edges
3. `Heap` - 91 edges
4. `Roots` - 49 edges
5. `TEST()` - 49 edges
6. `CallContext` - 31 edges
7. `TEST()` - 25 edges
8. `P0 phase doc` - 16 edges
9. `Gc` - 14 edges
10. `Boot` - 14 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `add()`  [INFERRED]
  docs/native-selectors.md → runtime/src/NativeMethod.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `Native selector mangling` --references--> `ao_Object_identityEquals()`  [INFERRED]
  docs/native-selectors.md → runtime/src/Natives.cpp

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

## Communities (75 total, 11 thin omitted)

### Community 0 - "SPEC.md"
Cohesion: 0.18
Nodes (17): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7, CompiledMethod (+9 more)

### Community 1 - "Oop"
Cohesion: 0.12
Nodes (7): Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, uint64_t

### Community 2 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 3 - "Heap"
Cohesion: 0.10
Nodes (21): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+13 more)

### Community 4 - "TEST"
Cohesion: 0.29
Nodes (7): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, TEST(), StubSmallIntegerAdd, SuperUsesDefiningClassSuperclass

### Community 5 - "P2 — Bootstrap"
Cohesion: 0.17
Nodes (11): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+3 more)

### Community 6 - "WellKnown"
Cohesion: 0.01
Nodes (134): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+126 more)

### Community 7 - "CallContext"
Cohesion: 0.09
Nodes (43): Native selector mangling, マングル表, 規則, HostOopHook, CallContext, cache, inspectHook, transcriptHook (+35 more)

### Community 8 - "P3 — Native Dispatch"
Cohesion: 0.07
Nodes (29): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, Heap (+21 more)

### Community 9 - "gtest"
Cohesion: 0.06
Nodes (27): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, CharacterRoundTrip, compiler, TEST(), CompilerSmoke, cstdio (+19 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "TEST"
Cohesion: 0.06
Nodes (31): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+23 more)

### Community 12 - "Roots"
Cohesion: 0.12
Nodes (16): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+8 more)

### Community 13 - "Heap.cpp"
Cohesion: 0.17
Nodes (15): cassert, fitsOld, align8(), byte, size_t, Heap::bytes(), Heap::fitsOld(), Heap::Heap() (+7 more)

### Community 14 - "Array and String Kernel"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 15 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 16 - "oldWalkVisits"
Cohesion: 0.67
Nodes (3): objectBytes, oldUsed, oldWalkVisits()

### Community 17 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 18 - "P1 — Object Memory"
Cohesion: 0.15
Nodes (18): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+10 more)

### Community 19 - "AppKit App Windows"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 20 - "Heap Allocation Tests"
Cohesion: 0.20
Nodes (10): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+2 more)

### Community 21 - "lookup_test.cpp"
Cohesion: 0.24
Nodes (9): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, Lookup, MethodDictionary, uint32_t, stubA(), stubB() (+1 more)

### Community 22 - "Image Save and Load"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 26 - "WellKnown.cpp"
Cohesion: 0.14
Nodes (16): deque, size_t, string, string_view, NamedClass, name, WellKnown, WellKnown::addRoots() (+8 more)

### Community 27 - "Gc.cpp"
Cohesion: 0.08
Nodes (26): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+18 more)

### Community 29 - "Object and Boolean Kernel"
Cohesion: 0.29
Nodes (7): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send

### Community 30 - "Stream and Transcript Kernel"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 31 - "TEST"
Cohesion: 0.18
Nodes (12): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, slotAt, slotAtPut, superOf(), NativeFn, WellKnown (+4 more)

### Community 32 - "gc_test.cpp"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 33 - "Magnitude and SmallInteger Kernel"
Cohesion: 0.25
Nodes (8): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger, runtime/tests/smallinteger_arith_test.cpp, runtime/src/kernel/SmallInteger.cpp

### Community 34 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 35 - "native_send_test.cpp"
Cohesion: 0.36
Nodes (7): NativeFn, uint32_t, WellKnown, install(), stubA(), stubB(), trueDnuSentinel()

### Community 41 - "Process and Semaphore Kernel"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 44 - "create"
Cohesion: 0.33
Nodes (6): at(), atPut(), uint32_t, WellKnown, create(), growInner()

### Community 45 - "Collection Kernel Classes"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 46 - "Swift Package Smoke"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 47 - "Point and Rectangle Kernel"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 48 - "Kernel Scan and Bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 49 - "docs/README.md"
Cohesion: 0.20
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 50 - "P4 — Kernel Native Implementation"
Cohesion: 0.67
Nodes (3): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger

### Community 51 - "NativeMethod"
Cohesion: 0.29
Nodes (5): NameAndApply, NativeMethod, uint32_t, stubEq(), TEST()

### Community 52 - "Behavior and Class Kernel"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "P8 — AppKit Tools"
Cohesion: 0.33
Nodes (7): P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, Do it / Print it / accept

### Community 54 - "Symbol"
Cohesion: 0.29
Nodes (7): gc, InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST(), Symbol

### Community 55 - ".fromSmallInteger"
Cohesion: 0.21
Nodes (9): int64_t, add(), apply(), NativeFn, string_view, uint32_t, WellKnown, create() (+1 more)

### Community 56 - "at"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 57 - "test_support.hpp"
Cohesion: 0.18
Nodes (10): Boot, cache, ctx, heap, roots, wk, WellKnown, send1() (+2 more)

### Community 58 - ".nil"
Cohesion: 0.35
Nodes (11): allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName(), run() (+3 more)

### Community 59 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 60 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 61 - "CLAUDE.md"
Cohesion: 0.24
Nodes (8): ao::Oop typed pointer, CLAUDE.md process authority, Graphify knowledge graph tool, Native selector mangling ao_Class_selector, Serena LSP symbol tool, SPEC.md product authority, 64-bit tagged OOP, Serena project.yml

### Community 62 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+3 more)

### Community 63 - "header"
Cohesion: 0.29
Nodes (10): header, uint16_t, uint32_t, Heap::allocate(), Heap::flags(), Heap::hash(), Heap::klass(), Heap::size() (+2 more)

### Community 64 - "TEST"
Cohesion: 0.22
Nodes (8): BlockContext, globals, KernelCatalog, NamedClassesAreHeap, TEST(), send0(), SmallIntegerSuperclassIsInteger, ValueAppliesNativeThunk

### Community 65 - "WellKnown"
Cohesion: 0.36
Nodes (3): cstdint, WellKnown, string_view

### Community 66 - "P2-02 class object skeletons"
Cohesion: 0.22
Nodes (9): Behavior, Bootstrap, Class, ClassDescription, Heap, Metaclass, Object, P2-02 class object skeletons (+1 more)

### Community 67 - "bootstrap_test.cpp"
Cohesion: 0.25
Nodes (8): class skeleton, metaclass cycle, P2-03 metaclass cycle, Bootstrap, Globals, P2-04 Smalltalk global dictionary, Smalltalk global dictionary, type_traits

### Community 68 - "Install.cpp"
Cohesion: 0.28
Nodes (8): install, natives, NativeFn, string_view, uint32_t, WellKnown, installAll(), putNative()

### Community 69 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 70 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 71 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 72 - "Format.hpp"
Cohesion: 0.46
Nodes (7): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make()

### Community 73 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): uint16_t, uint32_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 74 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

## Knowledge Gaps
- **368 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+363 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 524 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `P2 — Bootstrap`, `WellKnown`, `CallContext`, `P3 — Native Dispatch`, `gtest`, `Roots`, `Heap.cpp`, `Roots.cpp`, `TEST`, `lookup_test.cpp`, `WellKnown.cpp`, `Gc.cpp`, `TEST`, `gc_test.cpp`, `native_send_test.cpp`, `create`, `NativeMethod`, `.fromSmallInteger`, `at`, `test_support.hpp`, `.nil`, `intern`, `ClassMethodCache`, `header`, `TEST`, `WellKnown`, `Install.cpp`, `Format.hpp`, `ObjectHeader`, `ClassDef`?**
  _High betweenness centrality (0.273) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `Heap`, `P2 — Bootstrap`, `Roots`, `.nil`?**
  _High betweenness centrality (0.257) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `P2 — Bootstrap`, `WellKnown`, `CallContext`, `P3 — Native Dispatch`, `gtest`, `TEST`, `Heap.cpp`, `oldWalkVisits`, `TEST`, `Heap Allocation Tests`, `lookup_test.cpp`, `WellKnown.cpp`, `Gc.cpp`, `TEST`, `gc_test.cpp`, `native_send_test.cpp`, `create`, `NativeMethod`, `Symbol`, `.fromSmallInteger`, `at`, `test_support.hpp`, `.nil`, `intern`, `header`, `WellKnown`, `bootstrap_test.cpp`, `Install.cpp`, `ObjectHeader`?**
  _High betweenness centrality (0.184) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _368 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Oop` be split into smaller, more focused modules?**
  _Cohesion score 0.11857707509881422 - nodes in this community are weakly interconnected._
- **Should `Heap` be split into smaller, more focused modules?**
  _Cohesion score 0.09523809523809523 - nodes in this community are weakly interconnected._
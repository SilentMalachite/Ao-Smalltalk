# Graph Report - p3-dispatch  (2026-09-21)

## Corpus Check
- 126 files · ~21,516 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 11 file(s) not represented in the graph (top: (none) 10, .toml 1)

## Summary
- 779 nodes · 1364 edges · 61 communities (50 shown, 11 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 114 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `6bebe0d4`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- C ABI and Project Setup
- P3-04 lookup / super / doesNotUnderstand:
- P6b — vendor file-in
- Heap
- TEST
- P2-02 class object skeletons
- WellKnown Class Registry
- Oop
- P3-01 Symbol intern
- ABI and Compiler Smoke Tests
- Bytecode interpreter
- TEST
- Roots
- Object Header Layout
- Array and String Kernel
- Roots.cpp
- GC Tests
- TEST
- docs/README.md
- AppKit App Windows
- Heap Allocation Tests
- method_dictionary_test.cpp
- Image Save and Load
- Build Script
- Test Script
- WellKnown Implementation
- Gc
- WellKnown Module
- Object and Boolean Kernel
- Stream and Transcript Kernel
- lookup_test.cpp
- Old-Gen GC and Roots
- Magnitude and SmallInteger Kernel
- P1-06: weak slot GC
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
- P0 phase doc
- P4 — Kernel Native Implementation
- bootstrap_test.cpp
- Behavior and Class Kernel
- P3 — Native Dispatch
- TEST
- Globals Implementation
- Native Installation
- Bootstrap Implementation
- Symbol Implementation
- P9 — Integration
- P1-07: immovable old objects

## God Nodes (most connected - your core abstractions)
1. `Oop` - 109 edges
2. `Heap` - 83 edges
3. `WellKnown` - 73 edges
4. `TEST()` - 49 edges
5. `Roots` - 45 edges
6. `TEST()` - 25 edges
7. `CallContext` - 22 edges
8. `P0 phase doc` - 16 edges
9. `Gc` - 14 edges
10. `P4 — Kernel Native Implementation` - 14 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `add()`  [INFERRED]
  docs/native-selectors.md → runtime/src/NativeMethod.cpp
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

## Communities (61 total, 11 thin omitted)

### Community 0 - "C ABI and Project Setup"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 1 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.18
Nodes (10): class hierarchy, doesNotUnderstand:, lookup, MethodDictionary, NativeMethod, P3-04 lookup / super / doesNotUnderstand:, inline cache, lookup (+2 more)

### Community 2 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 3 - "Heap"
Cohesion: 0.10
Nodes (21): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+13 more)

### Community 4 - "TEST"
Cohesion: 0.22
Nodes (9): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, slotAt, superOf(), TEST(), StubSmallIntegerAdd (+1 more)

### Community 5 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 6 - "WellKnown Class Registry"
Cohesion: 0.05
Nodes (40): InternTable, unique_ptr, WellKnown, addRoots, behaviorClass, behaviorMetaclass, booleanClass, booleanMetaclass (+32 more)

### Community 7 - "Oop"
Cohesion: 0.05
Nodes (53): Native selector mangling, マングル表, 規則, clearWeakAfterOldMark, collectOld, copy, CallContext, cache (+45 more)

### Community 8 - "P3-01 Symbol intern"
Cohesion: 0.05
Nodes (34): CharacterRoundTrip, cstddef, cstdint, Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern (+26 more)

### Community 9 - "ABI and Compiler Smoke Tests"
Cohesion: 0.09
Nodes (19): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio, cstring (+11 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "TEST"
Cohesion: 0.06
Nodes (31): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+23 more)

### Community 12 - "Roots"
Cohesion: 0.12
Nodes (17): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+9 more)

### Community 13 - "Object Header Layout"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+26 more)

### Community 14 - "Array and String Kernel"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 15 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 16 - "GC Tests"
Cohesion: 0.50
Nodes (3): gc, oldUsed, oldWalkVisits()

### Community 17 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 18 - "docs/README.md"
Cohesion: 0.26
Nodes (12): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+4 more)

### Community 19 - "AppKit App Windows"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 20 - "Heap Allocation Tests"
Cohesion: 0.20
Nodes (10): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+2 more)

### Community 21 - "method_dictionary_test.cpp"
Cohesion: 0.50
Nodes (4): AtPutFindsInternedKey, GrowKeepsOuterOopAndEntries, MethodDictionary, TEST()

### Community 22 - "Image Save and Load"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 26 - "WellKnown Implementation"
Cohesion: 0.19
Nodes (13): deque, size_t, string_view, WellKnown::addRoots(), WellKnown::classOf(), WellKnown::intern(), WellKnown::InternTable, byBytes (+5 more)

### Community 27 - "Gc"
Cohesion: 0.22
Nodes (9): Gc, clearWeakAfterNursery, collectNursery, failed_, heap_, oldCompacted_, roots_, scavengeFromRoots (+1 more)

### Community 29 - "Object and Boolean Kernel"
Cohesion: 0.29
Nodes (7): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send

### Community 30 - "Stream and Transcript Kernel"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 31 - "lookup_test.cpp"
Cohesion: 0.17
Nodes (13): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, Lookup, MissingSelectorIsNil, slotAtPut, NativeFn, uint32_t, install() (+5 more)

### Community 32 - "Old-Gen GC and Roots"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 33 - "Magnitude and SmallInteger Kernel"
Cohesion: 0.25
Nodes (8): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger, runtime/tests/smallinteger_arith_test.cpp, runtime/src/kernel/SmallInteger.cpp

### Community 34 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 35 - "native_send_test.cpp"
Cohesion: 0.36
Nodes (7): NativeFn, uint32_t, install(), stubA(), stubB(), trueDnuSentinel(), send

### Community 41 - "Process and Semaphore Kernel"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 44 - "create"
Cohesion: 0.40
Nodes (5): at(), atPut(), uint32_t, create(), growInner()

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

### Community 49 - "P0 phase doc"
Cohesion: 0.23
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 50 - "P4 — Kernel Native Implementation"
Cohesion: 0.67
Nodes (3): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger

### Community 51 - "bootstrap_test.cpp"
Cohesion: 0.21
Nodes (7): Bootstrap, gtest, NameAndApply, NativeMethod, TEST(), Symbol, type_traits

### Community 52 - "Behavior and Class Kernel"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "P3 — Native Dispatch"
Cohesion: 0.25
Nodes (9): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P7 — Image, .aoimage save/load (+1 more)

### Community 54 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 56 - "Globals Implementation"
Cohesion: 0.40
Nodes (4): globals, at(), string_view, install()

### Community 57 - "Native Installation"
Cohesion: 0.50
Nodes (4): NativeFn, string_view, uint32_t, putNative()

### Community 58 - "Bootstrap Implementation"
Cohesion: 0.30
Nodes (9): natives, allocateSkeletons(), allocClass(), int64_t, installNatives(), makeName(), run(), wireClass() (+1 more)

### Community 59 - "Symbol Implementation"
Cohesion: 0.67
Nodes (3): bytes(), string_view, intern()

### Community 60 - "P9 — Integration"
Cohesion: 0.05
Nodes (38): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+30 more)

### Community 63 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

## Knowledge Gaps
- **260 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+255 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 397 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `P3-04 lookup / super / doesNotUnderstand:`, `TEST`, `WellKnown Class Registry`, `P3-01 Symbol intern`, `Roots`, `Object Header Layout`, `Roots.cpp`, `GC Tests`, `TEST`, `method_dictionary_test.cpp`, `WellKnown Implementation`, `Gc`, `lookup_test.cpp`, `native_send_test.cpp`, `create`, `bootstrap_test.cpp`, `Globals Implementation`, `Native Installation`, `Bootstrap Implementation`, `Symbol Implementation`?**
  _High betweenness centrality (0.222) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `P3-04 lookup / super / doesNotUnderstand:`, `TEST`, `WellKnown Class Registry`, `Oop`, `P3-01 Symbol intern`, `TEST`, `Object Header Layout`, `GC Tests`, `TEST`, `Heap Allocation Tests`, `method_dictionary_test.cpp`, `WellKnown Implementation`, `Gc`, `lookup_test.cpp`, `P1-06: weak slot GC`, `native_send_test.cpp`, `create`, `bootstrap_test.cpp`, `TEST`, `Globals Implementation`, `Native Installation`, `Bootstrap Implementation`, `Symbol Implementation`?**
  _High betweenness centrality (0.156) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown Class Registry` to `Heap`, `WellKnown Implementation`, `native_send_test.cpp`, `Oop`, `P3-01 Symbol intern`, `Roots`, `create`, `bootstrap_test.cpp`, `method_dictionary_test.cpp`, `Globals Implementation`, `Native Installation`, `Bootstrap Implementation`, `Symbol Implementation`, `lookup_test.cpp`?**
  _High betweenness centrality (0.116) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _260 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `C ABI and Project Setup` be split into smaller, more focused modules?**
  _Cohesion score 0.08170731707317073 - nodes in this community are weakly interconnected._
- **Should `Heap` be split into smaller, more focused modules?**
  _Cohesion score 0.09523809523809523 - nodes in this community are weakly interconnected._
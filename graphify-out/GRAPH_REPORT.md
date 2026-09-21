# Graph Report - p3-dispatch  (2026-09-21)

## Corpus Check
- 118 files · ~19,722 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 11 file(s) not represented in the graph (top: (none) 10, .toml 1)

## Summary
- 711 nodes · 1151 edges · 51 communities (38 shown, 13 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 91 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `e40cf230`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- SPEC.md
- P3-04 lookup / super / doesNotUnderstand:
- ObjectHeader
- Heap
- WellKnown.hpp
- bootstrap_test.cpp
- WellKnown
- Oop
- NativeMethod.hpp
- TEST
- Bytecode interpreter
- TEST
- Roots
- Heap.cpp
- P4-04 Array / ByteArray / String / Symbol
- Roots.cpp
- header
- TEST
- SmokeTests.swift
- Ao.app skeleton
- TEST
- method_dictionary_test.cpp
- ao image save
- build.sh
- test.sh
- Roots::visitAll
- Gc.cpp
- wellknown
- P4-01 Object / UndefinedObject / Boolean
- P4-06 Stream / Transcript model
- runtime/src/Lookup.cpp
- runtime/src/Send.cpp
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- Memory maintenance guide
- P4-07 Process / ProcessorScheduler / Semaphore
- heap
- oop
- P4-05 Dictionary / Set / OrderedCollection
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- docs/README.md
- P9 — Integration
- P4-02 Behavior / ClassDescription / Class / Metaclass
- P4 — Kernel Native Implementation
- klass
- create
- .nil

## God Nodes (most connected - your core abstractions)
1. `Oop` - 82 edges
2. `Heap` - 73 edges
3. `WellKnown` - 66 edges
4. `TEST()` - 47 edges
5. `Roots` - 39 edges
6. `TEST()` - 25 edges
7. `P0 phase doc` - 16 edges
8. `Gc` - 14 edges
9. `P4 — Kernel Native Implementation` - 14 edges
10. `TEST()` - 13 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `マングル表` --references--> `add()`  [INFERRED]
  docs/native-selectors.md → runtime/src/NativeMethod.cpp
- `ABI boundary memory` --references--> `ao_runtime_boot`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h

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

## Communities (51 total, 13 thin omitted)

### Community 0 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 1 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.05
Nodes (40): Native selector mangling, 規則, P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop (+32 more)

### Community 2 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): uint16_t, uint32_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 3 - "Heap"
Cohesion: 0.09
Nodes (24): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, inNursery, nextHash_ (+16 more)

### Community 4 - "WellKnown.hpp"
Cohesion: 0.29
Nodes (3): cstddef, memory, string_view

### Community 5 - "bootstrap_test.cpp"
Cohesion: 0.09
Nodes (26): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+18 more)

### Community 6 - "WellKnown"
Cohesion: 0.05
Nodes (39): InternTable, unique_ptr, WellKnown, behaviorClass, behaviorMetaclass, booleanClass, booleanMetaclass, characterClass (+31 more)

### Community 7 - "Oop"
Cohesion: 0.12
Nodes (7): Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, uint64_t

### Community 8 - "NativeMethod.hpp"
Cohesion: 0.33
Nodes (4): cstdint, CallContext, cache, ClassMethodCache

### Community 9 - "TEST"
Cohesion: 0.07
Nodes (27): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio, cstring (+19 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "TEST"
Cohesion: 0.07
Nodes (29): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+21 more)

### Community 12 - "Roots"
Cohesion: 0.11
Nodes (18): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+10 more)

### Community 13 - "Heap.cpp"
Cohesion: 0.17
Nodes (15): cassert, fitsOld, align8(), byte, size_t, Heap::bytes(), Heap::fitsOld(), Heap::Heap() (+7 more)

### Community 14 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 15 - "Roots.cpp"
Cohesion: 0.28
Nodes (8): StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle(), Roots::remove(), Roots::setStackWalker()

### Community 16 - "header"
Cohesion: 0.25
Nodes (11): header, objectBytes, uint16_t, uint32_t, Heap::allocate(), Heap::flags(), Heap::hash(), Heap::klass() (+3 more)

### Community 17 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsNilAndFormatIsSmi (+13 more)

### Community 18 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 19 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 20 - "TEST"
Cohesion: 0.18
Nodes (11): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+3 more)

### Community 21 - "method_dictionary_test.cpp"
Cohesion: 0.09
Nodes (23): AtPutFindsInternedKey, Bootstrap, CharacterRoundTrip, gc, GrowKeepsOuterOopAndEntries, gtest, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits (+15 more)

### Community 22 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 26 - "Roots::visitAll"
Cohesion: 0.67
Nodes (3): walker_, Roots::visitAll(), VisitFn

### Community 27 - "Gc.cpp"
Cohesion: 0.06
Nodes (38): algorithm, deque, Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順 (+30 more)

### Community 29 - "P4-01 Object / UndefinedObject / Boolean"
Cohesion: 0.29
Nodes (7): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send

### Community 30 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 41 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 45 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 47 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 48 - "P4-09 Kernel NativeMethod scan and bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 49 - "docs/README.md"
Cohesion: 0.05
Nodes (59): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator (+51 more)

### Community 50 - "P9 — Integration"
Cohesion: 0.05
Nodes (38): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+30 more)

### Community 52 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "P4 — Kernel Native Implementation"
Cohesion: 0.67
Nodes (3): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger

### Community 54 - "klass"
Cohesion: 0.29
Nodes (7): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, klass, add, clsOf(), TEST()

### Community 55 - "create"
Cohesion: 0.28
Nodes (6): int64_t, at(), atPut(), uint32_t, create(), growInner()

### Community 58 - ".nil"
Cohesion: 0.21
Nodes (12): globals, allocateSkeletons(), allocClass(), int64_t, makeName(), run(), wireClass(), wireCycle() (+4 more)

## Knowledge Gaps
- **255 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+250 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 373 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **13 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `P3-04 lookup / super / doesNotUnderstand:`, `ObjectHeader`, `WellKnown.hpp`, `WellKnown`, `NativeMethod.hpp`, `TEST`, `Roots`, `Heap.cpp`, `Roots.cpp`, `header`, `TEST`, `method_dictionary_test.cpp`, `klass`, `create`, `.nil`, `Gc.cpp`?**
  _High betweenness centrality (0.186) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `P3-04 lookup / super / doesNotUnderstand:`, `ObjectHeader`, `WellKnown.hpp`, `bootstrap_test.cpp`, `WellKnown`, `NativeMethod.hpp`, `TEST`, `Roots`, `Heap.cpp`, `header`, `TEST`, `TEST`, `method_dictionary_test.cpp`, `klass`, `create`, `.nil`, `Gc.cpp`?**
  _High betweenness centrality (0.161) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `P3-04 lookup / super / doesNotUnderstand:`, `Heap`, `WellKnown.hpp`, `bootstrap_test.cpp`, `Oop`, `NativeMethod.hpp`, `TEST`, `Roots`, `method_dictionary_test.cpp`, `create`, `.nil`, `Gc.cpp`?**
  _High betweenness centrality (0.123) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _255 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `SPEC.md` be split into smaller, more focused modules?**
  _Cohesion score 0.08170731707317073 - nodes in this community are weakly interconnected._
- **Should `P3-04 lookup / super / doesNotUnderstand:` be split into smaller, more focused modules?**
  _Cohesion score 0.05110336817653891 - nodes in this community are weakly interconnected._
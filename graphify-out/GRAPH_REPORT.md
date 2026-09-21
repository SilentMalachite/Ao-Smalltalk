# Graph Report - p3-dispatch  (2026-09-21)

## Corpus Check
- 114 files · ~19,143 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 11 file(s) not represented in the graph (top: (none) 10, .toml 1)

## Summary
- 692 nodes · 1096 edges · 63 communities (49 shown, 14 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 89 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `84a2de1f`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- SPEC.md
- P3-04 lookup / super / doesNotUnderstand:
- TEST
- Heap
- P0 phase doc
- P2-02 class object skeletons
- WellKnown
- Oop
- P5 — Compiler
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
- P9-01: Do it / Print it / Inspect it
- Gc
- P9-04: v1 ゴールデン受け入れ
- P4-01 Object / UndefinedObject / Boolean
- P4-06 Stream / Transcript model
- Gc.cpp
- P4-03 Magnitude / SmallInteger / Character
- runtime/src/NativeMethod.cpp
- runtime/src/Lookup.cpp
- runtime/src/Send.cpp
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- Memory maintenance guide
- P4-07 Process / ProcessorScheduler / Semaphore
- heap
- oop
- P9-03: エラー表示と VoiceOver
- P4-05 Dictionary / Set / OrderedCollection
- P9-02: Browser accept
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- P1 — Object Memory
- docs/README.md
- WellKnown.cpp
- P4-02 Behavior / ClassDescription / Class / Metaclass
- P4 — Kernel Native Implementation
- TEST
- .nil
- gc_test.cpp
- P6b — vendor file-in
- at
- Bootstrap.cpp
- intern
- WellKnown::intern
- Phase P0 skeleton

## God Nodes (most connected - your core abstractions)
1. `Oop` - 76 edges
2. `Heap` - 68 edges
3. `WellKnown` - 54 edges
4. `TEST()` - 47 edges
5. `Roots` - 36 edges
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
- `ABI boundary memory` --references--> `ao_runtime_boot`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h
- `ABI boundary memory` --references--> `ao_runtime_shutdown`  [EXTRACTED]
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

## Communities (63 total, 14 thin omitted)

### Community 0 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 1 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.06
Nodes (35): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, Heap (+27 more)

### Community 2 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 3 - "Heap"
Cohesion: 0.10
Nodes (21): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+13 more)

### Community 4 - "P0 phase doc"
Cohesion: 0.28
Nodes (13): P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub (+5 more)

### Community 5 - "P2-02 class object skeletons"
Cohesion: 0.06
Nodes (35): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+27 more)

### Community 6 - "WellKnown"
Cohesion: 0.05
Nodes (39): InternTable, unique_ptr, WellKnown, behaviorClass, behaviorMetaclass, booleanClass, booleanMetaclass, characterClass (+31 more)

### Community 7 - "Oop"
Cohesion: 0.12
Nodes (7): Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, uint64_t

### Community 8 - "P5 — Compiler"
Cohesion: 0.28
Nodes (9): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+1 more)

### Community 9 - "TEST"
Cohesion: 0.09
Nodes (19): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio, cstring (+11 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "TEST"
Cohesion: 0.07
Nodes (30): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+22 more)

### Community 12 - "Roots"
Cohesion: 0.12
Nodes (16): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+8 more)

### Community 13 - "Heap.cpp"
Cohesion: 0.16
Nodes (16): cassert, fitsOld, align8(), byte, size_t, Heap::bytes(), Heap::fitsOld(), Heap::header() (+8 more)

### Community 14 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 15 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 16 - "header"
Cohesion: 0.29
Nodes (10): header, uint16_t, uint32_t, Heap::allocate(), Heap::flags(), Heap::hash(), Heap::klass(), Heap::size() (+2 more)

### Community 17 - "TEST"
Cohesion: 0.09
Nodes (24): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsNilAndFormatIsSmi (+16 more)

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
Cohesion: 0.17
Nodes (9): AtPutFindsInternedKey, Bootstrap, cstdint, GrowKeepsOuterOopAndEntries, gtest, MethodDictionary, TEST(), Symbol (+1 more)

### Community 22 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 26 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 27 - "Gc"
Cohesion: 0.10
Nodes (19): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+11 more)

### Community 28 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 29 - "P4-01 Object / UndefinedObject / Boolean"
Cohesion: 0.29
Nodes (7): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send

### Community 30 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 31 - "Gc.cpp"
Cohesion: 0.33
Nodes (7): Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc(), Gc::scavengeFromRoots(), unordered_set

### Community 32 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.25
Nodes (8): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger, runtime/tests/smallinteger_arith_test.cpp, runtime/src/kernel/SmallInteger.cpp

### Community 41 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 44 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 46 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 47 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 48 - "P4-09 Kernel NativeMethod scan and bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 49 - "P1 — Object Memory"
Cohesion: 0.15
Nodes (18): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+10 more)

### Community 50 - "docs/README.md"
Cohesion: 0.31
Nodes (6): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept

### Community 51 - "WellKnown.cpp"
Cohesion: 0.21
Nodes (11): deque, addRoots, size_t, WellKnown::addRoots(), WellKnown::classOf(), WellKnown::InternTable, byBytes, table (+3 more)

### Community 52 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "P4 — Kernel Native Implementation"
Cohesion: 0.67
Nodes (3): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger

### Community 54 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 55 - ".nil"
Cohesion: 0.21
Nodes (9): int64_t, int64_t, makeName(), wireClass(), at(), atPut(), uint32_t, create() (+1 more)

### Community 56 - "gc_test.cpp"
Cohesion: 0.17
Nodes (11): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), gc (+3 more)

### Community 57 - "P6b — vendor file-in"
Cohesion: 0.29
Nodes (7): P6b — vendor file-in, Cuis Smalltalk vendor pin, LOAD_ORDER, ORIGIN.md, P7 — Image, .aoimage save/load, NOTICE mixed licenses

### Community 58 - "at"
Cohesion: 0.40
Nodes (4): globals, at(), string_view, install()

### Community 59 - "Bootstrap.cpp"
Cohesion: 0.70
Nodes (4): allocateSkeletons(), allocClass(), run(), wireCycle()

### Community 60 - "intern"
Cohesion: 0.67
Nodes (3): bytes(), string_view, intern()

### Community 61 - "WellKnown::intern"
Cohesion: 0.67
Nodes (3): string_view, WellKnown::intern(), WellKnown::named()

## Knowledge Gaps
- **256 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+251 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 370 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **14 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `P2-02 class object skeletons`, `WellKnown`, `Bootstrap.cpp`, `Roots`, `Heap.cpp`, `Roots.cpp`, `header`, `TEST`, `WellKnown.cpp`, `method_dictionary_test.cpp`, `.nil`, `gc_test.cpp`, `at`, `Gc`, `intern`, `WellKnown::intern`, `Gc.cpp`?**
  _High betweenness centrality (0.190) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `P2-02 class object skeletons`, `WellKnown`, `Bootstrap.cpp`, `TEST`, `Heap.cpp`, `header`, `TEST`, `WellKnown.cpp`, `TEST`, `method_dictionary_test.cpp`, `TEST`, `.nil`, `gc_test.cpp`, `at`, `Gc`, `intern`, `Gc.cpp`?**
  _High betweenness centrality (0.165) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Heap`, `P2-02 class object skeletons`, `Oop`, `Roots`, `WellKnown.cpp`, `.nil`, `at`, `Bootstrap.cpp`, `intern`?**
  _High betweenness centrality (0.112) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _256 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `SPEC.md` be split into smaller, more focused modules?**
  _Cohesion score 0.08170731707317073 - nodes in this community are weakly interconnected._
- **Should `P3-04 lookup / super / doesNotUnderstand:` be split into smaller, more focused modules?**
  _Cohesion score 0.06386554621848739 - nodes in this community are weakly interconnected._
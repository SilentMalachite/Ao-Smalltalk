# Graph Report - ao-smalltalk-p1-object-memory  (2026-09-21)

## Corpus Check
- cluster-only mode — file stats not available

## Summary
- 457 nodes · 566 edges · 41 communities (23 shown, 18 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 44 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `157eea3f`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- SPEC.md
- P4-03 Magnitude / SmallInteger / Character
- ObjectHeader
- Heap
- P5 — Compiler
- P2-02 class object skeletons
- P3-04 lookup / super / doesNotUnderstand:
- Oop
- Heap.cpp
- TEST
- Bytecode interpreter
- P1 — Object Memory
- P0-03: CMake + GoogleTest + CLI
- P1-04 old-generation mark-compact
- P9-01: Do it / Print it / Inspect it
- SmokeTests.swift
- P4-04 Array / ByteArray / String / Symbol
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- P4-05 Dictionary / Set / OrderedCollection
- P4-08 Point / Rectangle
- ao image save
- build.sh
- test.sh
- runtime/include/ao/Gc.hpp
- runtime/src/Roots.cpp
- runtime/src/WellKnown.cpp
- runtime/include/ao/Bootstrap.hpp
- runtime/src/Globals.cpp
- runtime/src/Symbol.cpp
- runtime/src/MethodDictionary.cpp
- runtime/src/NativeMethod.cpp
- runtime/src/Lookup.cpp
- runtime/src/Send.cpp
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- Memory maintenance guide

## God Nodes (most connected - your core abstractions)
1. `Heap` - 29 edges
2. `Oop` - 26 edges
3. `TEST()` - 13 edges
4. `P2-02 class object skeletons` - 11 edges
5. `ObjectHeader` - 10 edges
6. `TEST()` - 9 edges
7. `P0 phase doc` - 9 edges
8. `P4-03 Magnitude / SmallInteger / Character` - 9 edges
9. `P4-09 Kernel NativeMethod scan and bench` - 9 edges
10. `P3-04 lookup / super / doesNotUnderstand:` - 9 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `ABI boundary memory` --references--> `ao_runtime_boot`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h
- `ABI boundary memory` --references--> `ao_runtime_shutdown`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h
- `ABI boundary memory` --references--> `ao_version`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h
- `ao::Oop typed pointer` --semantically_similar_to--> `64-bit tagged OOP`  [INFERRED] [semantically similar]
  CLAUDE.md → SPEC.md

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
- **P9 PR dependency chain to v1** — prs_p9_01, prs_p9_02, prs_p9_03, prs_p9_04 [EXTRACTED 1.00]
- **C ABI wrappers over ao::Runtime** — runtime_src_abi_ao_version, runtime_src_abi_ao_runtime_boot, runtime_src_abi_ao_runtime_shutdown [EXTRACTED 1.00]

## Communities (41 total, 18 thin omitted)

### Community 0 - "SPEC.md"
Cohesion: 0.08
Nodes (39): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+31 more)

### Community 1 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.06
Nodes (34): inline cache, lookup, runtime/tests/native_send_test.cpp, P3-05 inline cache and send, send, runtime/include/ao/Send.hpp, Boolean, Bootstrap (+26 more)

### Community 2 - "ObjectHeader"
Cohesion: 0.06
Nodes (25): CharacterRoundTrip, compiler, TEST(), CompilerSmoke, cstddef, cstdint, gtest, heap (+17 more)

### Community 3 - "Heap"
Cohesion: 0.08
Nodes (31): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, flags (+23 more)

### Community 4 - "P5 — Compiler"
Cohesion: 0.08
Nodes (31): P3 — Native Dispatch, CompiledMethod, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P4 — Kernel Native Implementation (+23 more)

### Community 5 - "P2-02 class object skeletons"
Cohesion: 0.07
Nodes (28): runtime/tests/bootstrap_test.cpp, Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, runtime/include/ao/WellKnown.hpp, Behavior (+20 more)

### Community 6 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.08
Nodes (26): Heap, Oop, P3-01 Symbol intern, Symbol, runtime/include/ao/Symbol.hpp, Symbol::intern, runtime/tests/symbol_test.cpp, runtime/tests/method_dictionary_test.cpp (+18 more)

### Community 7 - "Oop"
Cohesion: 0.10
Nodes (8): int64_t, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, uint64_t

### Community 8 - "Heap.cpp"
Cohesion: 0.17
Nodes (22): cassert, header, objectBytes, align8(), byte, Oop, size_t, uint16_t (+14 more)

### Community 9 - "TEST"
Cohesion: 0.12
Nodes (15): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, cstdio, cstring, runtime, ao executable, ao_runtime library (+7 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.12
Nodes (23): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+15 more)

### Community 11 - "P1 — Object Memory"
Cohesion: 0.18
Nodes (17): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P2 — Bootstrap, Blue Book 6–10 (+9 more)

### Community 12 - "P0-03: CMake + GoogleTest + CLI"
Cohesion: 0.23
Nodes (12): P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub, P0-05: docs/phases and docs/prs (+4 more)

### Community 13 - "P1-04 old-generation mark-compact"
Cohesion: 0.17
Nodes (11): runtime/src/Gc.cpp, runtime/tests/gc_test.cpp, nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, runtime/tests/gc_test.cpp, P1-05 GC root API (+3 more)

### Community 14 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (11): P9-01: Do it / Print it / Inspect it, Do it, Inspect it, Print it, P9-02: Browser accept, Browser accept, P9-03: Error display and VoiceOver, Error display (+3 more)

### Community 15 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 16 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 17 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 18 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 19 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 20 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 21 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 22 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

## Knowledge Gaps
- **191 isolated node(s):** `AoPlaceholder`, `VoiceOver labels`, `v1 golden acceptance`, `Ao`, `PackageDescription` (+186 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 240 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `P8 — AppKit Tools` connect `P5 — Compiler` to `SPEC.md`?**
  _High betweenness centrality (0.095) - this node is a cross-community bridge._
- **Why does `P0 phase doc` connect `SPEC.md` to `SmokeTests.swift`?**
  _High betweenness centrality (0.067) - this node is a cross-community bridge._
- **Why does `Oop` connect `Oop` to `Heap.cpp`, `ObjectHeader`?**
  _High betweenness centrality (0.050) - this node is a cross-community bridge._
- **Are the 8 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 8 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `VoiceOver labels`, `v1 golden acceptance` to the rest of the system?**
  _191 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `SPEC.md` be split into smaller, more focused modules?**
  _Cohesion score 0.08080808080808081 - nodes in this community are weakly interconnected._
- **Should `P4-03 Magnitude / SmallInteger / Character` be split into smaller, more focused modules?**
  _Cohesion score 0.058823529411764705 - nodes in this community are weakly interconnected._
# Graph Report - ao-smalltalk  (2026-09-21)

## Corpus Check
- Corpus is ~12,504 words - fits in a single context window. You may not need a graph.

## Summary
- 362 nodes · 434 edges · 35 communities (20 shown, 15 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 30 edges (avg confidence: 0.89)
- Token cost: 27,700 input · 11,000 output

## Community Hubs (Navigation)
- Runtime ABI and Spec
- P0 Smoke Tests
- Dispatch and Kernel Docs
- P2 Bootstrap PRs
- P5 Compiler PRs
- P3 Lookup Send PRs
- P4 Object Behavior PRs
- P3 Symbol Dictionary PRs
- P1 P2 Phase Docs
- P9 Evaluation Tools
- P0 Skeleton PRs
- P1 GC Root PRs
- Swift Smoke Package
- P4 Array String PRs
- P4 Stream Transcript PRs
- P4 Process PRs
- P8 AppKit PRs
- P4 Dictionary PRs
- P4 Geometry PRs
- P7 Image PRs
- Build and Test Scripts
- Gc Header Plan
- Roots Source Plan
- WellKnown Source Plan
- Bootstrap Header Plan
- Globals Source Plan
- Symbol Source Plan
- MethodDictionary Source Plan
- NativeMethod Source Plan
- Lookup Source Plan
- Send Source Plan
- Object Protocol Stub
- Behavior Protocol Stub
- Serena Memory Maintenance

## God Nodes (most connected - your core abstractions)
1. `P2-02 class object skeletons` - 11 edges
2. `TEST()` - 9 edges
3. `P0 phase doc` - 9 edges
4. `P3-04 lookup / super / doesNotUnderstand:` - 9 edges
5. `P4-03 Magnitude / SmallInteger / Character` - 9 edges
6. `P4-09 Kernel NativeMethod scan and bench` - 9 edges
7. `P1 — Object Memory` - 8 edges
8. `P2-01 WellKnown and immediates` - 7 edges
9. `P3-01 Symbol intern` - 7 edges
10. `P3-03 NativeMethod and selector mangling` - 7 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `AoPlaceholder` --references--> `Phase P0 skeleton`  [EXTRACTED]
  app/Ao/Placeholder.swift → SPEC.md
- `P0 phase doc` --references--> `SmokeTests`  [EXTRACTED]
  docs/phases/P0.md → app/AoTests/SmokeTests.swift
- `ABI boundary memory` --references--> `ao_runtime_boot`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h
- `ABI boundary memory` --references--> `ao_runtime_shutdown`  [EXTRACTED]
  .serena/memories/abi_boundary.md → bridge/ao_abi.h

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Mandatory agent tooling trio** — concept_graphify, concept_serena, claude [EXTRACTED 1.00]
- **Runtime app ABI boundary** — concept_libao_runtime, concept_ao_app, concept_c_abi_boundary, bridge_ao_abi [EXTRACTED 1.00]
- **P0 skeleton deliverables** — concept_phase_p0, bridge_ao_abi, app_ao_placeholder, serena_project, concept_graphify [EXTRACTED 1.00]
- **P1 object memory stack** — docs_phases_p1_ao_oop, docs_phases_p1_heap, docs_phases_p1_gc, docs_phases_p1_nursery [EXTRACTED 1.00]
- **P3 native dispatch path** — docs_phases_p3_lookup, docs_phases_p3_methoddictionary, docs_phases_p3_nativemethod, docs_phases_p3_dnu [EXTRACTED 1.00]
- **P1 PR implementation chain** — docs_prs_p1_01, docs_prs_p1_02, docs_prs_p1_03, docs_phases_p1 [EXTRACTED 1.00]
- **P2 bootstrap pipeline** — docs_prs_p2_01_pr, docs_prs_p2_02_pr, docs_prs_p2_03_pr, docs_prs_p2_04_pr [EXTRACTED 1.00]
- **P3 method dispatch pipeline** — docs_prs_p3_01_pr, docs_prs_p3_02_pr, docs_prs_p3_03_pr, docs_prs_p3_04_pr, docs_prs_p3_05_pr [EXTRACTED 1.00]
- **P4 Kernel NativeMethod suite** — docs_prs_p4_01_pr, docs_prs_p4_02_pr, docs_prs_p4_03_pr, docs_prs_p4_04_pr, docs_prs_p4_05_pr, docs_prs_p4_06_pr, docs_prs_p4_07_pr, docs_prs_p4_08_pr, docs_prs_p4_09_pr [EXTRACTED 1.00]
- **P5 compiler pipeline** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunk_parser [EXTRACTED 1.00]
- **AOIM save/load and native rebind** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_image_save, docs_prs_p7_03_image_load, docs_prs_p7_02_nativemethod_symbol_names [EXTRACTED 1.00]
- **AppKit tool windows and menu** — docs_prs_p8_01_ao_app, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [EXTRACTED 1.00]
- **P9 PR dependency chain to v1** — prs_p9_01, prs_p9_02, prs_p9_03, prs_p9_04 [EXTRACTED 1.00]
- **C ABI wrappers over ao::Runtime** — runtime_src_abi_ao_version, runtime_src_abi_ao_runtime_boot, runtime_src_abi_ao_runtime_shutdown, ao_runtime_version_string, ao_runtime_boot, ao_runtime_shutdown [EXTRACTED 1.00]
- **Runtime boot / version / shutdown smoke path** — ao_runtime_boot, ao_runtime_version_string, ao_runtime_shutdown, tests_smoke_test_bootandshutdownreturnzero, tests_smoke_test_versionstringisnonempty, tests_smoke_test_abismoke [INFERRED 0.95]

## Communities (35 total, 15 thin omitted)

### Community 0 - "Runtime ABI and Spec"
Cohesion: 0.08
Nodes (39): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+31 more)

### Community 1 - "P0 Smoke Tests"
Cohesion: 0.08
Nodes (30): AbiSmoke, ao::boot(), ao::shutdown(), ao::version_string(), BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST() (+22 more)

### Community 2 - "Dispatch and Kernel Docs"
Cohesion: 0.08
Nodes (31): P3 — Native Dispatch, CompiledMethod, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P4 — Kernel Native Implementation (+23 more)

### Community 3 - "P2 Bootstrap PRs"
Cohesion: 0.07
Nodes (28): runtime/tests/bootstrap_test.cpp, Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, runtime/include/ao/WellKnown.hpp, Behavior (+20 more)

### Community 4 - "P5 Compiler PRs"
Cohesion: 0.12
Nodes (23): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+15 more)

### Community 5 - "P3 Lookup Send PRs"
Cohesion: 0.09
Nodes (22): class hierarchy, doesNotUnderstand:, lookup, runtime/include/ao/Lookup.hpp, runtime/tests/lookup_test.cpp, MethodDictionary, NativeMethod, P3-04 lookup / super / doesNotUnderstand: (+14 more)

### Community 6 - "P4 Object Behavior PRs"
Cohesion: 0.10
Nodes (20): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send, Behavior (+12 more)

### Community 7 - "P3 Symbol Dictionary PRs"
Cohesion: 0.11
Nodes (18): Heap, Oop, P3-01 Symbol intern, Symbol, runtime/include/ao/Symbol.hpp, Symbol::intern, runtime/tests/symbol_test.cpp, runtime/tests/method_dictionary_test.cpp (+10 more)

### Community 8 - "P1 P2 Phase Docs"
Cohesion: 0.18
Nodes (17): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P2 — Bootstrap, Blue Book 6–10 (+9 more)

### Community 9 - "P9 Evaluation Tools"
Cohesion: 0.17
Nodes (11): P9-01: Do it / Print it / Inspect it, Do it, Inspect it, Print it, P9-02: Browser accept, Browser accept, P9-03: Error display and VoiceOver, Error display (+3 more)

### Community 10 - "P0 Skeleton PRs"
Cohesion: 0.23
Nodes (12): P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke, ao_abi.h C ABI stub, P0-05: docs/phases and docs/prs (+4 more)

### Community 11 - "P1 GC Root PRs"
Cohesion: 0.17
Nodes (11): runtime/src/Gc.cpp, runtime/tests/gc_test.cpp, nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, runtime/tests/gc_test.cpp, P1-05 GC root API (+3 more)

### Community 12 - "Swift Smoke Package"
Cohesion: 0.22
Nodes (8): Ao, AoPlaceholder, AoPlaceholder, SmokeTests, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 13 - "P4 Array String PRs"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 14 - "P4 Stream Transcript PRs"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 15 - "P4 Process PRs"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 16 - "P8 AppKit PRs"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 17 - "P4 Dictionary PRs"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 18 - "P4 Geometry PRs"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 19 - "P7 Image PRs"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

## Knowledge Gaps
- **169 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `build.sh script` (+164 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 186 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **15 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `P8 — AppKit Tools` connect `Dispatch and Kernel Docs` to `Runtime ABI and Spec`?**
  _High betweenness centrality (0.076) - this node is a cross-community bridge._
- **Why does `P0 phase doc` connect `Runtime ABI and Spec` to `Swift Smoke Package`?**
  _High betweenness centrality (0.058) - this node is a cross-community bridge._
- **Are the 3 inferred relationships involving `TEST()` (e.g. with `ao_runtime_boot()` and `ao_runtime_shutdown()`) actually correct?**
  _`TEST()` has 3 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _169 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Runtime ABI and Spec` be split into smaller, more focused modules?**
  _Cohesion score 0.08080808080808081 - nodes in this community are weakly interconnected._
- **Should `P0 Smoke Tests` be split into smaller, more focused modules?**
  _Cohesion score 0.07681365576102418 - nodes in this community are weakly interconnected._
- **Should `Dispatch and Kernel Docs` be split into smaller, more focused modules?**
  _Cohesion score 0.08387096774193549 - nodes in this community are weakly interconnected._
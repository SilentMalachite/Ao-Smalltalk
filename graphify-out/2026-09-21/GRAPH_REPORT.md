# Graph Report - p2-bootstrap  (2026-09-21)

## Corpus Check
- 104 files · ~17,123 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 11 file(s) not represented in the graph (top: (none) 10, .toml 1)

## Summary
- 627 nodes · 913 edges · 51 communities (33 shown, 18 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 80 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `bfeac26e`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- SPEC.md
- P4-03 Magnitude / SmallInteger / Character
- TEST
- Heap
- P0 phase doc
- P2-02 class object skeletons
- WellKnown
- Oop
- P6b — vendor file-in
- TEST
- Bytecode interpreter
- TEST
- Roots
- Heap.cpp
- P4 — Kernel Native Implementation
- SmokeTests.swift
- P1 — Object Memory
- P9-02: Browser accept
- P9-04: v1 ゴールデン受け入れ
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- ao image save
- build.sh
- test.sh
- Roots.cpp
- Gc.cpp
- docs/README.md
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
- P2 — Bootstrap
- heap
- oop
- Heap.hpp
- .nil
- gc_test.cpp
- P9-01: Do it / Print it / Inspect it
- P1-07: immovable old objects
- P3-04 lookup / super / doesNotUnderstand:
- int64_t

## God Nodes (most connected - your core abstractions)
1. `Oop` - 58 edges
2. `Heap` - 48 edges
3. `TEST()` - 47 edges
4. `WellKnown` - 33 edges
5. `Roots` - 28 edges
6. `P0 phase doc` - 16 edges
7. `Gc` - 14 edges
8. `P4 — Kernel Native Implementation` - 14 edges
9. `TEST()` - 13 edges
10. `P1 — Object Memory` - 13 edges

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

## Communities (51 total, 18 thin omitted)

### Community 0 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 1 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.06
Nodes (34): inline cache, lookup, runtime/tests/native_send_test.cpp, P3-05 inline cache and send, send, runtime/include/ao/Send.hpp, Boolean, Bootstrap (+26 more)

### Community 2 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 3 - "Heap"
Cohesion: 0.08
Nodes (25): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, inNursery, inOld (+17 more)

### Community 4 - "P0 phase doc"
Cohesion: 0.23
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 5 - "P2-02 class object skeletons"
Cohesion: 0.07
Nodes (29): Bootstrap, Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, Behavior, Bootstrap (+21 more)

### Community 6 - "WellKnown"
Cohesion: 0.07
Nodes (27): WellKnown, behaviorClass, behaviorMetaclass, booleanClass, booleanMetaclass, characterClass, characterMetaclass, classClass (+19 more)

### Community 7 - "Oop"
Cohesion: 0.12
Nodes (7): Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, uint64_t

### Community 8 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 9 - "TEST"
Cohesion: 0.08
Nodes (20): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio, cstring (+12 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "TEST"
Cohesion: 0.07
Nodes (29): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+21 more)

### Community 12 - "Roots"
Cohesion: 0.12
Nodes (17): StackWalker, uint32_t, Roots, add, dropHandle, free_, handleAt, handles_ (+9 more)

### Community 13 - "Heap.cpp"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+26 more)

### Community 14 - "P4 — Kernel Native Implementation"
Cohesion: 0.05
Nodes (41): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod (+33 more)

### Community 15 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 16 - "P1 — Object Memory"
Cohesion: 0.23
Nodes (12): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+4 more)

### Community 17 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 18 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 19 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 20 - "TEST"
Cohesion: 0.15
Nodes (13): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+5 more)

### Community 21 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 22 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 26 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 27 - "Gc.cpp"
Cohesion: 0.09
Nodes (27): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+19 more)

### Community 28 - "docs/README.md"
Cohesion: 0.26
Nodes (8): P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept

### Community 41 - "P2 — Bootstrap"
Cohesion: 0.20
Nodes (11): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary (+3 more)

### Community 44 - "Heap.hpp"
Cohesion: 0.33
Nodes (3): cstddef, cstdint, memory

### Community 45 - ".nil"
Cohesion: 0.22
Nodes (7): addRoots, WellKnown::addRoots(), WellKnown::classOf(), WellKnown::named(), WellKnown::WellKnown(), string_view, wellknown

### Community 46 - "gc_test.cpp"
Cohesion: 0.22
Nodes (8): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), gc

### Community 47 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 48 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 49 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.08
Nodes (26): Heap, Oop, P3-01 Symbol intern, Symbol, runtime/include/ao/Symbol.hpp, Symbol::intern, runtime/tests/symbol_test.cpp, runtime/tests/method_dictionary_test.cpp (+18 more)

## Knowledge Gaps
- **255 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+250 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 345 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `TEST`, `Heap.hpp`, `.nil`, `Heap.cpp`, `Roots`, `gc_test.cpp`, `int64_t`, `Roots.cpp`, `Gc.cpp`?**
  _High betweenness centrality (0.169) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `P2-02 class object skeletons`, `WellKnown`, `TEST`, `Heap.hpp`, `.nil`, `Heap.cpp`, `gc_test.cpp`, `TEST`, `Gc.cpp`?**
  _High betweenness centrality (0.157) - this node is a cross-community bridge._
- **Why does `P1-04 old-generation mark-compact` connect `gc_test.cpp` to `P1 — Object Memory`, `Gc.cpp`, `docs/README.md`?**
  _High betweenness centrality (0.106) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _255 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `SPEC.md` be split into smaller, more focused modules?**
  _Cohesion score 0.08170731707317073 - nodes in this community are weakly interconnected._
- **Should `P4-03 Magnitude / SmallInteger / Character` be split into smaller, more focused modules?**
  _Cohesion score 0.058823529411764705 - nodes in this community are weakly interconnected._
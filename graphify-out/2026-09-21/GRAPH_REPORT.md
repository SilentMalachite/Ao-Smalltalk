# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- cluster-only mode — file stats not available

## Summary
- 969 nodes · 1613 edges · 69 communities (58 shown, 11 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 123 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `e1d75594`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Oop
- gtest
- Oop
- CallContext
- Heap.cpp
- Gc.cpp
- TEST
- SPEC.md
- P1 — Object Memory
- Bytecode interpreter
- Heap
- TEST
- docs/README.md
- WellKnown.cpp
- Roots
- TEST
- P6b — vendor file-in
- .nil
- Roots.cpp
- ClassMethodCache
- test_support.hpp
- Boot
- SmokeTests.swift
- TEST
- Install.cpp
- lookup_test.cpp
- P2 — Bootstrap
- P2-02 class object skeletons
- native_send_test.cpp
- ao_abi.h
- P4-03 Magnitude / SmallInteger / Character
- P4-04 Array / ByteArray / String / Symbol
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-02: Browser accept
- P9-03: エラー表示と VoiceOver
- P9-04: v1 ゴールデン受け入れ
- bootstrap_test.cpp
- create
- P8 — AppKit Tools
- P2-03 metaclass cycle
- P4-01 Object / UndefinedObject / Boolean
- Collection Kernel Classes
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- TEST
- ClassDef
- ao::compiler::version
- P4-02 Behavior / ClassDescription / Class / Metaclass
- at
- intern
- ao image save
- gc_test.cpp
- TEST
- P4 — Kernel Native Implementation
- build.sh
- test.sh
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- Memory maintenance guide
- wellknown

## God Nodes (most connected - your core abstractions)
1. `WellKnown` - 141 edges
2. `Heap` - 91 edges
3. `Roots` - 49 edges
4. `TEST()` - 49 edges
5. `CallContext` - 31 edges
6. `Oop` - 29 edges
7. `TEST()` - 25 edges
8. `P0 phase doc` - 16 edges
9. `Boot` - 14 edges
10. `Gc` - 14 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `マングル表` --references--> `add()`  [INFERRED]
  docs/native-selectors.md → runtime/src/NativeMethod.cpp
- `マングル表` --references--> `ao_Object_class()`  [INFERRED]
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

## Communities (69 total, 11 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (134): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+126 more)

### Community 1 - "Oop"
Cohesion: 0.06
Nodes (43): cstddef, cstdint, P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled> (+35 more)

### Community 2 - "gtest"
Cohesion: 0.05
Nodes (34): AbiSmoke, BlockContext, BootAndShutdownReturnZero, BootVersionShutdown, CharacterRoundTrip, compiler, TEST(), CompilerSmoke (+26 more)

### Community 3 - "Oop"
Cohesion: 0.07
Nodes (27): Native selector mangling, マングル表, 規則, int64_t, Oop, kCharTag, kImmTag, kLow3 (+19 more)

### Community 4 - "CallContext"
Cohesion: 0.11
Nodes (38): HostOopHook, CallContext, cache, inspectHook, transcriptHook, Oop, InlineCache, cachedClass (+30 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.09
Nodes (36): cassert, fitsOld, header, objectBytes, Oop, uint16_t, uint32_t, ObjectHeader (+28 more)

### Community 6 - "Gc.cpp"
Cohesion: 0.08
Nodes (28): algorithm, Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的 (+20 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (31): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+23 more)

### Community 8 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 9 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "Heap"
Cohesion: 0.10
Nodes (21): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+13 more)

### Community 12 - "TEST"
Cohesion: 0.11
Nodes (19): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+11 more)

### Community 13 - "docs/README.md"
Cohesion: 0.23
Nodes (14): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+6 more)

### Community 14 - "WellKnown.cpp"
Cohesion: 0.15
Nodes (17): deque, Oop, size_t, string, string_view, NamedClass, name, WellKnown (+9 more)

### Community 15 - "Roots"
Cohesion: 0.12
Nodes (17): Oop, StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_ (+9 more)

### Community 16 - "TEST"
Cohesion: 0.16
Nodes (15): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, slotAt, slotAtPut, NativeFn, Oop, uint32_t (+7 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 18 - ".nil"
Cohesion: 0.40
Nodes (12): allocateSkeletons(), allocClass(), Oop, WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 19 - "Roots.cpp"
Cohesion: 0.22
Nodes (12): walker_, Oop, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 20 - "ClassMethodCache"
Cohesion: 0.17
Nodes (12): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+4 more)

### Community 21 - "test_support.hpp"
Cohesion: 0.22
Nodes (7): NativeMethod, Oop, uint32_t, stubEq(), send, string, Symbol

### Community 22 - "Boot"
Cohesion: 0.20
Nodes (10): Boot, cache, ctx, heap, roots, wk, Oop, WellKnown (+2 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 24 - "TEST"
Cohesion: 0.20
Nodes (10): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+2 more)

### Community 25 - "Install.cpp"
Cohesion: 0.24
Nodes (9): install, natives, NativeFn, Oop, string_view, uint32_t, WellKnown, installAll() (+1 more)

### Community 26 - "lookup_test.cpp"
Cohesion: 0.33
Nodes (6): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, Lookup, MethodDictionary, TEST()

### Community 27 - "P2 — Bootstrap"
Cohesion: 0.22
Nodes (9): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+1 more)

### Community 28 - "P2-02 class object skeletons"
Cohesion: 0.22
Nodes (9): Behavior, Bootstrap, Class, ClassDescription, Heap, Metaclass, Object, P2-02 class object skeletons (+1 more)

### Community 29 - "native_send_test.cpp"
Cohesion: 0.39
Nodes (8): NativeFn, Oop, uint32_t, WellKnown, install(), stubA(), stubB(), trueDnuSentinel()

### Community 30 - "ao_abi.h"
Cohesion: 0.39
Nodes (7): ao_runtime_boot, ao_runtime_shutdown, ao_version, C ABI runtime-app boundary, ao::boot / shutdown / version_string, ao_abi.h C ABI stub, ABI boundary memory

### Community 31 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.25
Nodes (8): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger, runtime/tests/smallinteger_arith_test.cpp, runtime/src/kernel/SmallInteger.cpp

### Community 32 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 33 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 34 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 35 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 36 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 37 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 38 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 39 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 40 - "bootstrap_test.cpp"
Cohesion: 0.29
Nodes (7): NameAndApply, klass, clsOf(), Oop, superOf(), TEST(), type_traits

### Community 41 - "create"
Cohesion: 0.39
Nodes (7): at(), atPut(), Oop, uint32_t, WellKnown, create(), growInner()

### Community 42 - "P8 — AppKit Tools"
Cohesion: 0.33
Nodes (7): P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, Do it / Print it / accept

### Community 43 - "P2-03 metaclass cycle"
Cohesion: 0.29
Nodes (7): class skeleton, metaclass cycle, P2-03 metaclass cycle, Bootstrap, Globals, P2-04 Smalltalk global dictionary, Smalltalk global dictionary

### Community 44 - "P4-01 Object / UndefinedObject / Boolean"
Cohesion: 0.29
Nodes (7): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send

### Community 45 - "Collection Kernel Classes"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 46 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 47 - "P4-09 Kernel NativeMethod scan and bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 48 - "TEST"
Cohesion: 0.29
Nodes (7): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, TEST(), StubSmallIntegerAdd, SuperUsesDefiningClassSuperclass

### Community 49 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 50 - "ao::compiler::version"
Cohesion: 0.33
Nodes (6): Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty, libao_runtime

### Community 51 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 52 - "at"
Cohesion: 0.40
Nodes (5): at(), Oop, string_view, WellKnown, install()

### Community 53 - "intern"
Cohesion: 0.47
Nodes (5): bytes(), Oop, string_view, WellKnown, intern()

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 55 - "gc_test.cpp"
Cohesion: 0.40
Nodes (3): gc, oldUsed, oldWalkVisits()

### Community 56 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 57 - "P4 — Kernel Native Implementation"
Cohesion: 0.67
Nodes (3): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger

## Knowledge Gaps
- **368 isolated node(s):** `AoPlaceholder`, `addRoots`, `arrayClass`, `arrayedCollectionClass`, `arrayedCollectionMetaclass` (+363 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 533 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Heap` connect `Heap` to `WellKnown`, `Oop`, `gtest`, `Oop`, `CallContext`, `Heap.cpp`, `Gc.cpp`, `TEST`, `WellKnown.cpp`, `TEST`, `.nil`, `test_support.hpp`, `Boot`, `TEST`, `Install.cpp`, `lookup_test.cpp`, `native_send_test.cpp`, `bootstrap_test.cpp`, `create`, `at`, `intern`, `gc_test.cpp`, `TEST`?**
  _High betweenness centrality (0.297) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `Oop`, `Heap`, `Roots`?**
  _High betweenness centrality (0.257) - this node is a cross-community bridge._
- **Why does `Roots` connect `Roots` to `WellKnown`, `Oop`, `CallContext`, `Gc.cpp`, `bootstrap_test.cpp`, `WellKnown.cpp`, `.nil`, `Roots.cpp`, `at`, `test_support.hpp`, `Boot`, `gc_test.cpp`, `TEST`, `Install.cpp`, `lookup_test.cpp`, `native_send_test.cpp`?**
  _High betweenness centrality (0.126) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `addRoots`, `arrayClass` to the rest of the system?**
  _368 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014814814814814815 - nodes in this community are weakly interconnected._
- **Should `Oop` be split into smaller, more focused modules?**
  _Cohesion score 0.05639097744360902 - nodes in this community are weakly interconnected._
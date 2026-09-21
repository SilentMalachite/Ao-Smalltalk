# Graph Report - p3-dispatch  (2026-09-21)

## Corpus Check
- 126 files · ~21,375 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 11 file(s) not represented in the graph (top: (none) 10, .toml 1)

## Summary
- 776 nodes · 1357 edges · 66 communities (53 shown, 13 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 113 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `24880368`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- SPEC.md
- P3 — Native Dispatch
- cstdint
- Heap
- NativeMethod.hpp
- P2-02 class object skeletons
- WellKnown
- Oop
- ClassMethodCache
- TEST
- P6b — vendor file-in
- TEST
- Roots
- Heap.cpp
- P4-04 Array / ByteArray / String / Symbol
- Roots.cpp
- gc_test.cpp
- TEST
- P1 — Object Memory
- Ao.app skeleton
- TEST
- method_dictionary_test.cpp
- ao image save
- build.sh
- test.sh
- WellKnown.cpp
- Gc.cpp
- wellknown
- P4-01 Object / UndefinedObject / Boolean
- P4-06 Stream / Transcript model
- lookup_test.cpp
- P1-04 old-generation mark-compact
- P4-03 Magnitude / SmallInteger / Character
- P9-02: Browser accept
- native_send_test.cpp
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- Memory maintenance guide
- P4-07 Process / ProcessorScheduler / Semaphore
- heap
- oop
- P9-03: エラー表示と VoiceOver
- P4-05 Dictionary / Set / OrderedCollection
- SmokeTests.swift
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- docs/README.md
- P9-01: Do it / Print it / Inspect it
- native_method_test.cpp
- P4-02 Behavior / ClassDescription / Class / Metaclass
- P4 — Kernel Native Implementation
- gtest
- P3-01 Symbol intern
- at
- putNative
- .nil
- intern
- P9-04: v1 ゴールデン受け入れ
- TEST
- P8 — AppKit Tools
- P1-07: immovable old objects
- WellKnown.hpp
- Roots::visitAll

## God Nodes (most connected - your core abstractions)
1. `Oop` - 108 edges
2. `Heap` - 83 edges
3. `WellKnown` - 73 edges
4. `TEST()` - 47 edges
5. `Roots` - 45 edges
6. `TEST()` - 25 edges
7. `CallContext` - 21 edges
8. `P0 phase doc` - 16 edges
9. `Gc` - 14 edges
10. `P4 — Kernel Native Implementation` - 14 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
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

## Communities (66 total, 13 thin omitted)

### Community 0 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.11
Nodes (18): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling (+10 more)

### Community 3 - "Heap"
Cohesion: 0.09
Nodes (22): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, inNursery, nextHash_ (+14 more)

### Community 4 - "NativeMethod.hpp"
Cohesion: 0.29
Nodes (5): Bootstrap, Globals, P2-04 Smalltalk global dictionary, Smalltalk global dictionary, string_view

### Community 5 - "P2-02 class object skeletons"
Cohesion: 0.10
Nodes (21): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+13 more)

### Community 6 - "WellKnown"
Cohesion: 0.05
Nodes (40): InternTable, unique_ptr, WellKnown, addRoots, behaviorClass, behaviorMetaclass, booleanClass, booleanMetaclass (+32 more)

### Community 7 - "Oop"
Cohesion: 0.06
Nodes (46): Native selector mangling, マングル表, 規則, CallContext, cache, int64_t, Oop, kCharTag (+38 more)

### Community 8 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+3 more)

### Community 9 - "TEST"
Cohesion: 0.09
Nodes (19): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio, cstring (+11 more)

### Community 10 - "P6b — vendor file-in"
Cohesion: 0.07
Nodes (36): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+28 more)

### Community 11 - "TEST"
Cohesion: 0.07
Nodes (29): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+21 more)

### Community 12 - "Roots"
Cohesion: 0.12
Nodes (16): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+8 more)

### Community 13 - "Heap.cpp"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+26 more)

### Community 14 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.25
Nodes (8): Array, runtime/src/kernel/Array.cpp, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send, runtime/src/kernel/String.cpp

### Community 15 - "Roots.cpp"
Cohesion: 0.24
Nodes (9): algorithm, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle(), Roots::remove() (+1 more)

### Community 16 - "gc_test.cpp"
Cohesion: 0.50
Nodes (3): gc, oldUsed, oldWalkVisits()

### Community 17 - "TEST"
Cohesion: 0.09
Nodes (23): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+15 more)

### Community 18 - "P1 — Object Memory"
Cohesion: 0.26
Nodes (12): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+4 more)

### Community 19 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 20 - "TEST"
Cohesion: 0.20
Nodes (10): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+2 more)

### Community 21 - "method_dictionary_test.cpp"
Cohesion: 0.32
Nodes (6): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, Lookup, MethodDictionary, TEST()

### Community 22 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 26 - "WellKnown.cpp"
Cohesion: 0.19
Nodes (13): deque, size_t, string_view, WellKnown::addRoots(), WellKnown::classOf(), WellKnown::intern(), WellKnown::InternTable, byBytes (+5 more)

### Community 27 - "Gc.cpp"
Cohesion: 0.09
Nodes (26): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+18 more)

### Community 29 - "P4-01 Object / UndefinedObject / Boolean"
Cohesion: 0.29
Nodes (7): Boolean, Bootstrap, NativeMethod, Object, runtime/src/kernel/Object.cpp, P4-01 Object / UndefinedObject / Boolean, send

### Community 30 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 31 - "lookup_test.cpp"
Cohesion: 0.21
Nodes (11): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, NativeFn, uint32_t, install(), stubA(), stubB() (+3 more)

### Community 32 - "P1-04 old-generation mark-compact"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 33 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.25
Nodes (8): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger, runtime/tests/smallinteger_arith_test.cpp, runtime/src/kernel/SmallInteger.cpp

### Community 34 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 35 - "native_send_test.cpp"
Cohesion: 0.18
Nodes (13): DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, slotAtPut, NativeFn, uint32_t, install(), stubA() (+5 more)

### Community 41 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 44 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 46 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 47 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 48 - "P4-09 Kernel NativeMethod scan and bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 49 - "docs/README.md"
Cohesion: 0.22
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 50 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 51 - "native_method_test.cpp"
Cohesion: 0.25
Nodes (6): NameAndApply, NativeMethod, klass, uint32_t, stubEq(), TEST()

### Community 52 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "P4 — Kernel Native Implementation"
Cohesion: 0.40
Nodes (6): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, NativeMethod symbol rebind on load

### Community 54 - "gtest"
Cohesion: 0.25
Nodes (7): gtest, InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST(), Symbol

### Community 55 - "P3-01 Symbol intern"
Cohesion: 0.20
Nodes (8): Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary, P3-02 MethodDictionary, Symbol

### Community 56 - "at"
Cohesion: 0.40
Nodes (4): globals, at(), string_view, install()

### Community 57 - "putNative"
Cohesion: 0.50
Nodes (4): NativeFn, string_view, uint32_t, putNative()

### Community 58 - ".nil"
Cohesion: 0.30
Nodes (9): natives, allocateSkeletons(), allocClass(), int64_t, installNatives(), makeName(), run(), wireClass() (+1 more)

### Community 59 - "intern"
Cohesion: 0.67
Nodes (3): bytes(), string_view, intern()

### Community 60 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 61 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 62 - "P8 — AppKit Tools"
Cohesion: 0.33
Nodes (7): P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, Do it / Print it / accept

### Community 63 - "P1-07: immovable old objects"
Cohesion: 0.29
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 65 - "Roots::visitAll"
Cohesion: 0.67
Nodes (3): walker_, Roots::visitAll(), VisitFn

## Knowledge Gaps
- **261 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+256 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 396 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **13 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `P3 — Native Dispatch`, `cstdint`, `NativeMethod.hpp`, `WellKnown`, `ClassMethodCache`, `Roots`, `Heap.cpp`, `Roots.cpp`, `gc_test.cpp`, `TEST`, `method_dictionary_test.cpp`, `WellKnown.cpp`, `Gc.cpp`, `lookup_test.cpp`, `native_send_test.cpp`, `native_method_test.cpp`, `gtest`, `P3-01 Symbol intern`, `at`, `putNative`, `.nil`, `intern`, `WellKnown.hpp`?**
  _High betweenness centrality (0.222) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `P3 — Native Dispatch`, `cstdint`, `NativeMethod.hpp`, `WellKnown`, `Oop`, `Heap.cpp`, `gc_test.cpp`, `TEST`, `TEST`, `method_dictionary_test.cpp`, `WellKnown.cpp`, `Gc.cpp`, `lookup_test.cpp`, `native_send_test.cpp`, `native_method_test.cpp`, `gtest`, `P3-01 Symbol intern`, `at`, `putNative`, `.nil`, `intern`, `WellKnown.hpp`?**
  _High betweenness centrality (0.157) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `WellKnown.hpp`, `cstdint`, `Heap`, `NativeMethod.hpp`, `WellKnown.cpp`, `native_send_test.cpp`, `Oop`, `Roots`, `TEST`, `native_method_test.cpp`, `method_dictionary_test.cpp`, `gtest`, `P3-01 Symbol intern`, `at`, `putNative`, `.nil`, `intern`, `lookup_test.cpp`?**
  _High betweenness centrality (0.116) - this node is a cross-community bridge._
- **Are the 17 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 17 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _261 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `SPEC.md` be split into smaller, more focused modules?**
  _Cohesion score 0.08170731707317073 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.11052631578947368 - nodes in this community are weakly interconnected._
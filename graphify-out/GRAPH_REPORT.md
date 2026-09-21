# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 135 files · ~26,580 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 16 file(s) not represented in the graph (top: (none) 10, .st 5, .toml 1)

## Summary
- 1028 nodes · 1974 edges · 62 communities (51 shown, 11 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 148 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `2aedab59`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- cstring
- Oop
- BlockContext.cpp
- Heap.cpp
- Gc.cpp
- TEST
- SPEC.md
- P1 — Object Memory
- Bytecode interpreter
- Heap
- TEST
- docs/README.md
- .nil
- Roots
- TEST
- P6b — vendor file-in
- Boot
- WellKnown
- TEST
- NativeMethod
- ClassMethodCache
- SmokeTests.swift
- Format.hpp
- putNative
- lookup_test.cpp
- P2-02 class object skeletons
- P1-07: immovable old objects
- native_send_test.cpp
- TEST
- P4-03 Magnitude / SmallInteger / Character
- P4-04 Array / ByteArray / String / Symbol
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-02: Browser accept
- P9-03: エラー表示と VoiceOver
- TEST
- P9 — Integration
- P4-01 Object / UndefinedObject / Boolean
- Collection Kernel Classes
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- P4-02 Behavior / ClassDescription / Class / Metaclass
- intern
- ao image save
- gc_test.cpp
- Symbol
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
1. `Oop` - 190 edges
2. `WellKnown` - 141 edges
3. `Heap` - 94 edges
4. `CallContext` - 89 edges
5. `Roots` - 49 edges
6. `TEST()` - 49 edges
7. `TEST()` - 25 edges
8. `TEST()` - 23 edges
9. `send()` - 16 edges
10. `P0 phase doc` - 16 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `add()`  [INFERRED]
  docs/native-selectors.md → runtime/src/NativeMethod.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `Native selector mangling` --references--> `ao_SmallInteger_add()`  [INFERRED]
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

## Communities (62 total, 11 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (134): InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass, arrayMetaclass (+126 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.07
Nodes (29): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, Heap (+21 more)

### Community 2 - "cstring"
Cohesion: 0.07
Nodes (26): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio, cstring (+18 more)

### Community 3 - "Oop"
Cohesion: 0.06
Nodes (104): Native selector mangling, マングル表, 規則, HostOopHook, CallContext, cache, inspectHook, transcriptHook (+96 more)

### Community 4 - "BlockContext.cpp"
Cohesion: 0.26
Nodes (12): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), NativeFn, string, string_view, uint32_t (+4 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.09
Nodes (34): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+26 more)

### Community 6 - "Gc.cpp"
Cohesion: 0.06
Nodes (38): algorithm, Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的 (+30 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (31): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcNursery, GcOld, GcRoots, GcWeak, HandleTableKeepsObject (+23 more)

### Community 8 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 9 - "P1 — Object Memory"
Cohesion: 0.29
Nodes (12): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+4 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "Heap"
Cohesion: 0.08
Nodes (27): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+19 more)

### Community 12 - "TEST"
Cohesion: 0.09
Nodes (23): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+15 more)

### Community 13 - "docs/README.md"
Cohesion: 0.28
Nodes (15): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string (+7 more)

### Community 14 - ".nil"
Cohesion: 0.07
Nodes (38): deque, globals, allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize (+30 more)

### Community 15 - "Roots"
Cohesion: 0.11
Nodes (18): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+10 more)

### Community 16 - "TEST"
Cohesion: 0.20
Nodes (10): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, slotAtPut, NativeFn, WellKnown, install(), TEST() (+2 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.15
Nodes (16): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+8 more)

### Community 18 - "Boot"
Cohesion: 0.19
Nodes (10): formatOf(), Boot, cache, ctx, heap, roots, wk, WellKnown (+2 more)

### Community 19 - "WellKnown"
Cohesion: 0.26
Nodes (4): cstdint, WellKnown, string, string_view

### Community 20 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 21 - "NativeMethod"
Cohesion: 0.33
Nodes (5): NameAndApply, NativeMethod, uint32_t, stubEq(), TEST()

### Community 22 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+3 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 24 - "Format.hpp"
Cohesion: 0.46
Nodes (7): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make()

### Community 25 - "putNative"
Cohesion: 0.19
Nodes (12): install, natives, WellKnown, installBoolean(), NativeFn, string_view, uint32_t, WellKnown (+4 more)

### Community 26 - "lookup_test.cpp"
Cohesion: 0.21
Nodes (10): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, gtest, Lookup, MethodDictionary, uint32_t, stubA() (+2 more)

### Community 27 - "P2-02 class object skeletons"
Cohesion: 0.08
Nodes (27): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+19 more)

### Community 28 - "P1-07: immovable old objects"
Cohesion: 0.29
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 29 - "native_send_test.cpp"
Cohesion: 0.17
Nodes (15): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, slotAt, NativeFn, uint32_t, WellKnown (+7 more)

### Community 30 - "TEST"
Cohesion: 0.20
Nodes (10): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+2 more)

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
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 39 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 42 - "P9 — Integration"
Cohesion: 0.13
Nodes (14): P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration, Do it / Print it / accept, Files, Interfaces (+6 more)

### Community 44 - "P4-01 Object / UndefinedObject / Boolean"
Cohesion: 0.33
Nodes (6): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send

### Community 45 - "Collection Kernel Classes"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/tests/collection_do_test.cpp, Dictionary, runtime/src/kernel/Dictionary.cpp, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 46 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 47 - "P4-09 Kernel NativeMethod scan and bench"
Cohesion: 0.29
Nodes (7): docs/bench.md, Bootstrap, Kernel NativeMethod scan, runtime/tests/kernel_scan_test.cpp, NativeMethod, P4-09 Kernel NativeMethod scan and bench, send

### Community 51 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.33
Nodes (6): Behavior, runtime/src/kernel/Behavior.cpp, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 55 - "gc_test.cpp"
Cohesion: 0.20
Nodes (9): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), oldUsed (+1 more)

### Community 56 - "Symbol"
Cohesion: 0.29
Nodes (7): gc, InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST(), Symbol

### Community 57 - "P4 — Kernel Native Implementation"
Cohesion: 0.67
Nodes (3): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger

## Knowledge Gaps
- **367 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `collectNursery` (+362 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 541 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `cstring`, `BlockContext.cpp`, `Heap.cpp`, `Gc.cpp`, `Heap`, `TEST`, `.nil`, `Roots`, `TEST`, `Boot`, `WellKnown`, `NativeMethod`, `ClassMethodCache`, `Format.hpp`, `putNative`, `lookup_test.cpp`, `P2-02 class object skeletons`, `native_send_test.cpp`, `intern`, `gc_test.cpp`?**
  _High betweenness centrality (0.311) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `Heap`, `.nil`, `Roots`, `P2-02 class object skeletons`?**
  _High betweenness centrality (0.239) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `cstring`, `Oop`, `BlockContext.cpp`, `Heap.cpp`, `Gc.cpp`, `TEST`, `TEST`, `.nil`, `TEST`, `Boot`, `WellKnown`, `NativeMethod`, `putNative`, `lookup_test.cpp`, `P2-02 class object skeletons`, `native_send_test.cpp`, `TEST`, `intern`, `gc_test.cpp`, `Symbol`?**
  _High betweenness centrality (0.174) - this node is a cross-community bridge._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _367 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014814814814814815 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.07007575757575757 - nodes in this community are weakly interconnected._
- **Should `cstring` be split into smaller, more focused modules?**
  _Cohesion score 0.06685633001422475 - nodes in this community are weakly interconnected._
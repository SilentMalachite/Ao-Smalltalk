# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 147 files · ~36,594 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 36 file(s) not represented in the graph (top: .st 25, (none) 10, .toml 1)

## Summary
- 1333 nodes · 3012 edges · 73 communities (62 shown, 11 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 250 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `c91396da`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- TEST
- LargeInteger.cpp
- .isHeap
- Heap.cpp
- Behavior.cpp
- TEST
- SPEC.md
- P1 — Object Memory
- Bytecode interpreter
- SmallInteger.cpp
- Roots
- docs/README.md
- .nil
- TEST
- TEST
- P6b — vendor file-in
- Boot
- cstdint
- TEST
- Heap
- String.cpp
- SmokeTests.swift
- putNative
- Oop
- ClassMethodCache
- P2-02 class object skeletons
- TEST
- send
- BlockContext.cpp
- P4-03 Magnitude / SmallInteger / Character
- P4-04 Array / ByteArray / String / Symbol
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- Roots.cpp
- TEST
- TEST
- Bootstrap
- create
- test_support.hpp
- P9-04: v1 ゴールデン受け入れ
- P9-01: Do it / Print it / Inspect it
- P4-01 Object / UndefinedObject / Boolean
- Collection Kernel Classes
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- TEST
- P8 — AppKit Tools
- P1-07: immovable old objects
- native_send_test.cpp
- intern
- P9-02: Browser accept
- ao image save
- CallContext
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- Memory maintenance guide
- wellknown
- P4 — Kernel Native Implementation
- install
- NativeMethod
- build.sh
- test.sh

## God Nodes (most connected - your core abstractions)
1. `Oop` - 320 edges
2. `WellKnown` - 144 edges
3. `Heap` - 118 edges
4. `Roots` - 61 edges
5. `TEST()` - 49 edges
6. `TEST()` - 34 edges
7. `parse()` - 26 edges
8. `box()` - 25 edges
9. `TEST()` - 25 edges
10. `マングル表` - 25 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `ao_Float_divide()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Float.cpp
- `マングル表` --references--> `modulo()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/LargeInteger.cpp
- `Native selector mangling` --references--> `ao_Object_identityEquals()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Object.cpp

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

## Communities (73 total, 11 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (137): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+129 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 2 - "TEST"
Cohesion: 0.12
Nodes (15): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, cstdio, runtime, ao executable, ao_runtime library (+7 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.10
Nodes (62): Digits, __int128, int64_t, ao_ArrayedCollection_do_(), CallContext, uint32_t, add(), addBig() (+54 more)

### Community 4 - ".isHeap"
Cohesion: 0.07
Nodes (54): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, FlOp (+46 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.08
Nodes (36): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+28 more)

### Community 6 - "Behavior.cpp"
Cohesion: 0.20
Nodes (28): collectNursery, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes() (+20 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (33): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+25 more)

### Community 8 - "SPEC.md"
Cohesion: 0.08
Nodes (36): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+28 more)

### Community 9 - "P1 — Object Memory"
Cohesion: 0.14
Nodes (19): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+11 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "SmallInteger.cpp"
Cohesion: 0.21
Nodes (26): Native selector mangling, 規則, largeinteger, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_() (+18 more)

### Community 12 - "Roots"
Cohesion: 0.07
Nodes (26): StackWalker, uint32_t, vector, Roots, dropHandle, free_, handleAt, handles_ (+18 more)

### Community 13 - "docs/README.md"
Cohesion: 0.20
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 14 - ".nil"
Cohesion: 0.07
Nodes (41): deque, format, allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize (+33 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 16 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 18 - "Boot"
Cohesion: 0.15
Nodes (12): formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap, roots (+4 more)

### Community 19 - "cstdint"
Cohesion: 0.16
Nodes (7): cstddef, cstdint, memory, CallContext, WellKnown, string, string_view

### Community 20 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 21 - "Heap"
Cohesion: 0.08
Nodes (27): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+19 more)

### Community 22 - "String.cpp"
Cohesion: 0.12
Nodes (28): compiler, TEST(), CompilerSmoke, cstring, allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_() (+20 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 24 - "putNative"
Cohesion: 0.13
Nodes (23): WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter(), WellKnown, installCollection() (+15 more)

### Community 25 - "Oop"
Cohesion: 0.06
Nodes (110): マングル表, natives, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers() (+102 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+3 more)

### Community 27 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 28 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+6 more)

### Community 29 - "send"
Cohesion: 0.20
Nodes (19): InlineCache, cachedClass, cachedMethod, lookup(), allocateMessage(), cacheIndex(), ClassMethodCache::addRoots(), ClassMethodCache::insert() (+11 more)

### Community 30 - "BlockContext.cpp"
Cohesion: 0.27
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 31 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.33
Nodes (6): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger

### Community 32 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 33 - "P4-06 Stream / Transcript model"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, runtime/src/kernel/Stream.cpp, Transcript, runtime/tests/transcript_model_test.cpp

### Community 34 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.25
Nodes (8): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, runtime/src/kernel/Process.cpp, ProcessorScheduler, Semaphore, send

### Community 35 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 36 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 37 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayString, AsSymbolAndAsString, AtPutAndSize, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo, InvalidUtf8ByteIsCharacter, TEST() (+6 more)

### Community 38 - "TEST"
Cohesion: 0.18
Nodes (11): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+3 more)

### Community 39 - "Bootstrap"
Cohesion: 0.22
Nodes (11): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, Lookup, MethodDictionary, CallContext, uint32_t, stubA() (+3 more)

### Community 40 - "create"
Cohesion: 0.27
Nodes (9): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+1 more)

### Community 41 - "test_support.hpp"
Cohesion: 0.23
Nodes (7): context, gc, globals, gtest, install, send, type_traits

### Community 42 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 43 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

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

### Community 48 - "TEST"
Cohesion: 0.10
Nodes (22): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, slotAt (+14 more)

### Community 49 - "P8 — AppKit Tools"
Cohesion: 0.24
Nodes (10): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P7 — Image, .aoimage save/load, NativeMethod symbol rebind on load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+2 more)

### Community 50 - "P1-07: immovable old objects"
Cohesion: 0.29
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 51 - "native_send_test.cpp"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, stubA(), stubB(), trueDnuSentinel()

### Community 52 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 53 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 55 - "CallContext"
Cohesion: 0.40
Nodes (5): HostOopHook, CallContext, cache, inspectHook, transcriptHook

### Community 56 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 57 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 60 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "P4 — Kernel Native Implementation"
Cohesion: 0.25
Nodes (8): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Behavior, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 73 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 75 - "NativeMethod"
Cohesion: 0.29
Nodes (6): NameAndApply, NativeMethod, CallContext, uint32_t, stubEq(), TEST()

## Knowledge Gaps
- **377 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+372 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 616 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `LargeInteger.cpp`, `.isHeap`, `Heap.cpp`, `Behavior.cpp`, `SmallInteger.cpp`, `Roots`, `.nil`, `Boot`, `cstdint`, `TEST`, `Heap`, `String.cpp`, `putNative`, `ClassMethodCache`, `TEST`, `send`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `TEST`, `Bootstrap`, `create`, `test_support.hpp`, `TEST`, `native_send_test.cpp`, `intern`, `install`, `NativeMethod`?**
  _High betweenness centrality (0.472) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Roots`, `.nil`, `cstdint`, `Heap`, `Oop`?**
  _High betweenness centrality (0.190) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `LargeInteger.cpp`, `.isHeap`, `Heap.cpp`, `TEST`, `.nil`, `TEST`, `Boot`, `cstdint`, `TEST`, `String.cpp`, `putNative`, `Oop`, `send`, `BlockContext.cpp`, `TEST`, `Bootstrap`, `create`, `test_support.hpp`, `TEST`, `native_send_test.cpp`, `intern`, `CallContext`, `TEST`, `install`, `NativeMethod`?**
  _High betweenness centrality (0.133) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _377 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014492753623188406 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.082010582010582 - nodes in this community are weakly interconnected._
# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 143 files · ~34,300 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 29 file(s) not represented in the graph (top: .st 18, (none) 10, .toml 1)

## Summary
- 1264 nodes · 2800 edges · 72 communities (61 shown, 11 thin omitted)
- Extraction: 92% EXTRACTED · 8% INFERRED · 0% AMBIGUOUS · INFERRED: 220 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `b324767a`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- cstring
- LargeInteger.cpp
- Float.cpp
- Heap.cpp
- Behavior.cpp
- TEST
- SPEC.md
- P1 — Object Memory
- Bytecode interpreter
- SmallInteger.cpp
- Roots
- docs/README.md
- WellKnown.cpp
- TEST
- TEST
- P6b — vendor file-in
- Boot
- cstdint
- TEST
- Heap
- .isHeap
- SmokeTests.swift
- putNative
- Oop
- ClassMethodCache
- P2-02 class object skeletons
- TEST
- P3-03 NativeMethod and selector mangling
- BlockContext.cpp
- P4-03 Magnitude / SmallInteger / Character
- P4-04 Array / ByteArray / String / Symbol
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- Roots.cpp
- ao_abi.h
- P3-04 lookup / super / doesNotUnderstand:
- Symbol
- TEST
- P1-07: immovable old objects
- CallContext
- P9-01: Do it / Print it / Inspect it
- P4-01 Object / UndefinedObject / Boolean
- Collection Kernel Classes
- P4-08 Point / Rectangle
- P4-09 Kernel NativeMethod scan and bench
- native_send_test.cpp
- P8 — AppKit Tools
- P4-02 Behavior / ClassDescription / Class / Metaclass
- P9-02: Browser accept
- ao image save
- gc_test.cpp
- P9-03: エラー表示と VoiceOver
- P9-04: v1 ゴールデン受け入れ
- lookup_test.cpp
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- Memory maintenance guide
- wellknown
- P4 — Kernel Native Implementation
- gtest
- NativeMethod
- build.sh
- test.sh

## God Nodes (most connected - your core abstractions)
1. `Oop` - 295 edges
2. `WellKnown` - 144 edges
3. `Heap` - 109 edges
4. `Roots` - 55 edges
5. `TEST()` - 49 edges
6. `TEST()` - 30 edges
7. `parse()` - 26 edges
8. `box()` - 25 edges
9. `TEST()` - 25 edges
10. `TEST()` - 23 edges

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

## Communities (72 total, 11 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (137): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+129 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.17
Nodes (12): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+4 more)

### Community 2 - "cstring"
Cohesion: 0.06
Nodes (29): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio (+21 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (57): Digits, __int128, int64_t, add(), addBig(), asInt64IfFits(), Big, d (+49 more)

### Community 4 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.08
Nodes (35): cassert, fitsOld, header, uint16_t, uint32_t, ObjectHeader, flags, hash (+27 more)

### Community 6 - "Behavior.cpp"
Cohesion: 0.14
Nodes (33): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, MethodDictionary, collectNursery, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_() (+25 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (33): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+25 more)

### Community 8 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 9 - "P1 — Object Memory"
Cohesion: 0.29
Nodes (12): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+4 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (24): natives, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals() (+16 more)

### Community 12 - "Roots"
Cohesion: 0.09
Nodes (23): StackWalker, uint32_t, vector, Roots, dropHandle, free_, handleAt, handles_ (+15 more)

### Community 13 - "docs/README.md"
Cohesion: 0.23
Nodes (14): P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI, ao::boot / shutdown / version_string, P0-04: C ABI + Swift smoke (+6 more)

### Community 14 - "WellKnown.cpp"
Cohesion: 0.11
Nodes (22): deque, size_t, string, string_view, Entry, cls, name, NamedClass (+14 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 16 - "TEST"
Cohesion: 0.08
Nodes (24): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloorDivAndModulo, FractionMulDiv, FractionNormalizesToInteger (+16 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 18 - "Boot"
Cohesion: 0.15
Nodes (15): largeinteger, floatValue(), makeFloat(), Boot, cache, ctx, heap, roots (+7 more)

### Community 19 - "cstdint"
Cohesion: 0.22
Nodes (5): cstdint, CallContext, WellKnown, string, string_view

### Community 20 - "TEST"
Cohesion: 0.09
Nodes (24): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+16 more)

### Community 21 - "Heap"
Cohesion: 0.08
Nodes (28): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, inNursery, nextHash_ (+20 more)

### Community 22 - ".isHeap"
Cohesion: 0.05
Nodes (63): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的, Gc (+55 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.18
Nodes (8): Ao, AoPlaceholder, SmokeTests, Phase P0 skeleton, Phase pipeline P0-P9, PackageDescription, XCTest, XCTestCase

### Community 24 - "putNative"
Cohesion: 0.12
Nodes (27): context, install, WellKnown, installBehavior(), WellKnown, installBoolean(), ao_Character_asCharacter(), ao_Character_asciiValue() (+19 more)

### Community 25 - "Oop"
Cohesion: 0.07
Nodes (91): マングル表, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make() (+83 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, addRoots, entries, insert, kSize, probe, Entry, klass (+3 more)

### Community 27 - "P2-02 class object skeletons"
Cohesion: 0.08
Nodes (27): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+19 more)

### Community 28 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 29 - "P3-03 NativeMethod and selector mangling"
Cohesion: 0.29
Nodes (6): Native selector mangling, 規則, MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling

### Community 30 - "BlockContext.cpp"
Cohesion: 0.27
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 31 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.33
Nodes (6): Bootstrap, Magnitude, NativeMethod, P4-03 Magnitude / SmallInteger / Character, send, SmallInteger

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

### Community 36 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 37 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 38 - "P3-04 lookup / super / doesNotUnderstand:"
Cohesion: 0.18
Nodes (10): class hierarchy, doesNotUnderstand:, lookup, MethodDictionary, NativeMethod, P3-04 lookup / super / doesNotUnderstand:, inline cache, lookup (+2 more)

### Community 39 - "Symbol"
Cohesion: 0.18
Nodes (11): gc, InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, bytes(), string_view, WellKnown (+3 more)

### Community 40 - "TEST"
Cohesion: 0.20
Nodes (10): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+2 more)

### Community 41 - "P1-07: immovable old objects"
Cohesion: 0.29
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 42 - "CallContext"
Cohesion: 0.40
Nodes (5): HostOopHook, CallContext, cache, inspectHook, transcriptHook

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

### Community 48 - "native_send_test.cpp"
Cohesion: 0.17
Nodes (15): DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, CallContext, NativeFn, uint32_t, WellKnown (+7 more)

### Community 49 - "P8 — AppKit Tools"
Cohesion: 0.33
Nodes (7): P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, Do it / Print it / accept

### Community 51 - "P4-02 Behavior / ClassDescription / Class / Metaclass"
Cohesion: 0.40
Nodes (5): Behavior, Bootstrap, NativeMethod, P4-02 Behavior / ClassDescription / Class / Metaclass, send

### Community 53 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 55 - "gc_test.cpp"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 56 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 57 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 59 - "lookup_test.cpp"
Cohesion: 0.17
Nodes (15): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, Lookup, MissingSelectorIsNil, slotAtPut, CallContext, NativeFn, uint32_t (+7 more)

### Community 60 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "P4 — Kernel Native Implementation"
Cohesion: 0.40
Nodes (6): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, NativeMethod symbol rebind on load

### Community 73 - "gtest"
Cohesion: 0.20
Nodes (8): format, globals, gtest, at(), string_view, WellKnown, install(), formatOf()

### Community 75 - "NativeMethod"
Cohesion: 0.25
Nodes (6): NameAndApply, NativeMethod, CallContext, uint32_t, stubEq(), TEST()

## Knowledge Gaps
- **374 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+369 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 592 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `cstring`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `Behavior.cpp`, `SmallInteger.cpp`, `Roots`, `WellKnown.cpp`, `Boot`, `cstdint`, `TEST`, `Heap`, `.isHeap`, `putNative`, `ClassMethodCache`, `P2-02 class object skeletons`, `TEST`, `BlockContext.cpp`, `Roots.cpp`, `P3-04 lookup / super / doesNotUnderstand:`, `Symbol`, `native_send_test.cpp`, `gc_test.cpp`, `lookup_test.cpp`, `gtest`, `NativeMethod`?**
  _High betweenness centrality (0.444) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Roots`, `Heap`, `.isHeap`, `Oop`, `P2-02 class object skeletons`?**
  _High betweenness centrality (0.202) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `cstring`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `Behavior.cpp`, `TEST`, `WellKnown.cpp`, `TEST`, `Boot`, `cstdint`, `TEST`, `.isHeap`, `putNative`, `Oop`, `P2-02 class object skeletons`, `BlockContext.cpp`, `P3-04 lookup / super / doesNotUnderstand:`, `Symbol`, `TEST`, `CallContext`, `native_send_test.cpp`, `gc_test.cpp`, `lookup_test.cpp`, `gtest`, `NativeMethod`?**
  _High betweenness centrality (0.136) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _374 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014492753623188406 - nodes in this community are weakly interconnected._
- **Should `cstring` be split into smaller, more focused modules?**
  _Cohesion score 0.06341463414634146 - nodes in this community are weakly interconnected._
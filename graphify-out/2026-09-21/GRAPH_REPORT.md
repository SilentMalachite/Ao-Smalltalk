# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 149 files · ~41,890 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 46 file(s) not represented in the graph (top: .st 35, (none) 10, .toml 1)

## Summary
- 1431 nodes · 3501 edges · 72 communities (58 shown, 14 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 319 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `fe3bc56c`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- .nil
- LargeInteger.cpp
- Float.cpp
- Heap.cpp
- .false_
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
- test_support.hpp
- TEST
- Heap
- cstring
- SmokeTests.swift
- putNative
- Object.cpp
- ClassMethodCache
- P2-02 class object skeletons
- TEST
- Oop
- String.cpp
- Gc.cpp
- ao_abi.h
- P4-06 Stream / Transcript model
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- Roots.cpp
- TEST
- TEST
- TEST
- Gc
- native_send_test.cpp
- P9-04: v1 ゴールデン受け入れ
- P9-01: Do it / Print it / Inspect it
- P1-06: weak slot GC
- InlineCache
- P4 — Kernel Native Implementation
- P4-03 Magnitude / SmallInteger / Character
- uint32_t
- stubA
- Root
- ClassDef
- intern
- P9-02: Browser accept
- ao image save
- Root
- P9-03: エラー表示と VoiceOver
- Root
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- Memory maintenance guide
- wellknown
- install
- stubEq
- build.sh
- test.sh

## God Nodes (most connected - your core abstractions)
1. `Oop` - 387 edges
2. `WellKnown` - 144 edges
3. `Heap` - 121 edges
4. `Roots` - 63 edges
5. `TEST()` - 49 edges
6. `send()` - 42 edges
7. `TEST()` - 34 edges
8. `parse()` - 26 edges
9. `box()` - 25 edges
10. `TEST()` - 25 edges

## Surprising Connections (you probably didn't know these)
- `P0 phase doc` --references--> `CompilerSmoke.VersionIsNonEmpty`  [INFERRED]
  docs/phases/P0.md → compiler/tests/smoke_test.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `手順` --references--> `Heap`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Heap.hpp
- `マングル表` --references--> `ao_ArrayedCollection_at_()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Array.cpp
- `マングル表` --references--> `ao_ArrayedCollection_new_()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Array.cpp

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

## Communities (72 total, 14 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (137): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+129 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (22): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+14 more)

### Community 2 - ".nil"
Cohesion: 0.31
Nodes (12): allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName(), run() (+4 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.08
Nodes (36): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+28 more)

### Community 6 - ".false_"
Cohesion: 0.10
Nodes (45): context, install, collectNursery, uint64_t, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_() (+37 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (33): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+25 more)

### Community 8 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 9 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "SmallInteger.cpp"
Cohesion: 0.12
Nodes (49): Native selector mangling, マングル表, 規則, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_() (+41 more)

### Community 12 - "Roots"
Cohesion: 0.09
Nodes (22): StackWalker, uint32_t, vector, Roots, dropHandle, free_, handleAt, handles_ (+14 more)

### Community 13 - "docs/README.md"
Cohesion: 0.16
Nodes (21): P0 phase doc, P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 14 - "WellKnown.cpp"
Cohesion: 0.12
Nodes (20): deque, size_t, string, string_view, Entry, cls, name, NamedClass (+12 more)

### Community 15 - "TEST"
Cohesion: 0.09
Nodes (22): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, format, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf (+14 more)

### Community 16 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 18 - "Boot"
Cohesion: 0.09
Nodes (23): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues, IdentityDictionaryDoesNotUseEquals (+15 more)

### Community 19 - "test_support.hpp"
Cohesion: 0.15
Nodes (10): cstdint, largeinteger, NameAndApply, NativeMethod, CallContext, TEST(), floatValue(), send (+2 more)

### Community 20 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 21 - "Heap"
Cohesion: 0.08
Nodes (27): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+19 more)

### Community 22 - "cstring"
Cohesion: 0.05
Nodes (42): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio (+34 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.18
Nodes (8): Ao, AoPlaceholder, SmokeTests, Phase P0 skeleton, Phase pipeline P0-P9, PackageDescription, XCTest, XCTestCase

### Community 24 - "putNative"
Cohesion: 0.13
Nodes (22): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+14 more)

### Community 25 - "Object.cpp"
Cohesion: 0.10
Nodes (56): natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size(), CallContext (+48 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.10
Nodes (20): MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling, HostOopHook, CallContext, cache, inspectHook (+12 more)

### Community 27 - "P2-02 class object skeletons"
Cohesion: 0.08
Nodes (27): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+19 more)

### Community 28 - "TEST"
Cohesion: 0.13
Nodes (15): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, globals, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden (+7 more)

### Community 29 - "Oop"
Cohesion: 0.06
Nodes (114): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), int64_t (+106 more)

### Community 30 - "String.cpp"
Cohesion: 0.18
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 31 - "Gc.cpp"
Cohesion: 0.19
Nodes (11): clearWeakAfterOldMark, collectOld, copy, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc() (+3 more)

### Community 32 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

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
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 38 - "TEST"
Cohesion: 0.18
Nodes (11): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+3 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (22): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, slotAt (+14 more)

### Community 40 - "Gc"
Cohesion: 0.22
Nodes (9): Gc, clearWeakAfterNursery, failed_, heap_, oldCompacted_, roots_, safepoint, scavengeFromRoots (+1 more)

### Community 41 - "native_send_test.cpp"
Cohesion: 0.17
Nodes (16): AtPutFindsInternedKey, Bootstrap, gc, GrowKeepsOuterOopAndEntries, gtest, InternSameBytesIsIdentical, InternSurvivesNurseryGc, Lookup (+8 more)

### Community 42 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 43 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 44 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 45 - "InlineCache"
Cohesion: 0.40
Nodes (5): InlineCache, cachedClass, cachedMethod, IcGuard, ic

### Community 46 - "P4 — Kernel Native Implementation"
Cohesion: 0.09
Nodes (24): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, NativeMethod symbol rebind on load, Array, ArrayedCollection (+16 more)

### Community 47 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (24): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+16 more)

### Community 48 - "uint32_t"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, stubA(), stubB(), trueDnuSentinel()

### Community 49 - "stubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 51 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 52 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 53 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 56 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 60 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 73 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 75 - "stubEq"
Cohesion: 0.67
Nodes (3): CallContext, uint32_t, stubEq()

## Knowledge Gaps
- **376 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+371 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 627 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **14 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `.nil`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `.false_`, `TEST`, `SmallInteger.cpp`, `Roots`, `WellKnown.cpp`, `TEST`, `Boot`, `test_support.hpp`, `TEST`, `Heap`, `cstring`, `putNative`, `Object.cpp`, `ClassMethodCache`, `P2-02 class object skeletons`, `TEST`, `String.cpp`, `Gc.cpp`, `Roots.cpp`, `TEST`, `TEST`, `Gc`, `native_send_test.cpp`, `InlineCache`, `uint32_t`, `stubA`, `Root`, `ClassDef`, `intern`, `Root`, `Root`, `TEST`, `install`, `stubEq`?**
  _High betweenness centrality (0.472) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `.nil`, `.false_`, `Roots`, `Heap`, `P2-02 class object skeletons`, `Oop`?**
  _High betweenness centrality (0.173) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `.nil`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `TEST`, `TEST`, `Boot`, `test_support.hpp`, `TEST`, `cstring`, `putNative`, `Object.cpp`, `ClassMethodCache`, `P2-02 class object skeletons`, `Oop`, `String.cpp`, `Gc.cpp`, `TEST`, `TEST`, `Gc`, `native_send_test.cpp`, `P1-06: weak slot GC`, `intern`, `install`?**
  _High betweenness centrality (0.119) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _376 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014388489208633094 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.08307692307692308 - nodes in this community are weakly interconnected._
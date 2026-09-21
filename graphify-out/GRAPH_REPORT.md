# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 151 files · ~44,313 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 53 file(s) not represented in the graph (top: .st 42, (none) 10, .toml 1)

## Summary
- 1491 nodes · 3753 edges · 76 communities (65 shown, 11 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 345 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `65213300`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- .nil
- LargeInteger.cpp
- Float.cpp
- Heap.cpp
- send
- TEST
- SPEC.md
- docs/README.md
- Bytecode interpreter
- SmallInteger.cpp
- Roots
- Stream.cpp
- WellKnown::InternTable
- TEST
- TEST
- P6b — vendor file-in
- TEST
- test_support.hpp
- TEST
- Heap
- String.cpp
- SmokeTests.swift
- context
- Oop
- ClassMethodCache
- bootstrap_test.cpp
- TEST
- .isHeap
- Boot
- Gc.cpp
- ao_abi.h
- putNative
- P4-07 Process / ProcessorScheduler / Semaphore
- Ao.app skeleton
- Roots.cpp
- TEST
- Runtime.cpp
- native_send_test.cpp
- TEST
- TEST
- P9-04: v1 ゴールデン受け入れ
- P9-01: Do it / Print it / Inspect it
- P1-06: weak slot GC
- create
- P4 — Kernel Native Implementation
- P4-03 Magnitude / SmallInteger / Character
- vector
- Bootstrap
- P8 — AppKit Tools
- ClassDef
- intern
- P9-02: Browser accept
- ao image save
- cstring
- P9-03: エラー表示と VoiceOver
- P4-08 Point / Rectangle
- create
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- P1-07: immovable old objects
- Memory maintenance guide
- wellknown
- install
- TEST
- stubEq
- build.sh
- test.sh
- WellKnown.cpp

## God Nodes (most connected - your core abstractions)
1. `Oop` - 421 edges
2. `WellKnown` - 145 edges
3. `Heap` - 124 edges
4. `Roots` - 65 edges
5. `TEST()` - 49 edges
6. `send()` - 48 edges
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

## Communities (76 total, 11 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 2 - ".nil"
Cohesion: 0.31
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.08
Nodes (36): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+28 more)

### Community 6 - "send"
Cohesion: 0.09
Nodes (54): int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_() (+46 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (31): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+23 more)

### Community 8 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 9 - "docs/README.md"
Cohesion: 0.13
Nodes (26): P0 phase doc, P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P9 — Integration (+18 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "SmallInteger.cpp"
Cohesion: 0.12
Nodes (49): Native selector mangling, マングル表, 規則, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_() (+41 more)

### Community 12 - "Roots"
Cohesion: 0.07
Nodes (25): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+17 more)

### Community 13 - "Stream.cpp"
Cohesion: 0.14
Nodes (43): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+35 more)

### Community 14 - "WellKnown::InternTable"
Cohesion: 0.18
Nodes (12): deque, size_t, string, Entry, cls, name, WellKnown::ExtraTable, table (+4 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 16 - "TEST"
Cohesion: 0.07
Nodes (31): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+23 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 18 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 19 - "test_support.hpp"
Cohesion: 0.14
Nodes (11): AddInternsByFunctionPointer, cstdint, globals, gtest, NameAndApply, NativeMethod, CallContext, WellKnown (+3 more)

### Community 20 - "TEST"
Cohesion: 0.07
Nodes (31): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+23 more)

### Community 21 - "Heap"
Cohesion: 0.07
Nodes (30): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+22 more)

### Community 22 - "String.cpp"
Cohesion: 0.18
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.18
Nodes (8): Ao, AoPlaceholder, SmokeTests, Phase P0 skeleton, Phase pipeline P0-P9, PackageDescription, XCTest, XCTestCase

### Community 24 - "context"
Cohesion: 0.25
Nodes (14): context, ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string (+6 more)

### Community 25 - "Oop"
Cohesion: 0.06
Nodes (106): install, natives, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers() (+98 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.12
Nodes (16): HostOopHook, CallContext, cache, inspectHook, transcriptHook, ClassMethodCache, addRoots, entries (+8 more)

### Community 27 - "bootstrap_test.cpp"
Cohesion: 0.07
Nodes (29): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+21 more)

### Community 28 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 29 - ".isHeap"
Cohesion: 0.12
Nodes (61): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+53 more)

### Community 30 - "Boot"
Cohesion: 0.18
Nodes (10): formatOf(), Boot, cache, ctx, heap, roots, wk, CallContext (+2 more)

### Community 31 - "Gc.cpp"
Cohesion: 0.11
Nodes (20): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectOld, copy, failed_, heap_, oldCompacted_ (+12 more)

### Community 32 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 33 - "putNative"
Cohesion: 0.12
Nodes (24): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+16 more)

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

### Community 38 - "Runtime.cpp"
Cohesion: 0.19
Nodes (6): ao_abi, cstdio, runtime, ao executable, ao_runtime library, ao_runtime_tests

### Community 39 - "native_send_test.cpp"
Cohesion: 0.36
Nodes (8): CallContext, NativeFn, uint32_t, WellKnown, install(), stubA(), stubB(), trueDnuSentinel()

### Community 40 - "TEST"
Cohesion: 0.18
Nodes (11): NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, send1(), send2(), TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut (+3 more)

### Community 41 - "TEST"
Cohesion: 0.24
Nodes (9): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, ao_runtime_boot(), ao_runtime_shutdown(), ao_version(), TEST(), Smoke (+1 more)

### Community 42 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 43 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 44 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 45 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 46 - "P4 — Kernel Native Implementation"
Cohesion: 0.10
Nodes (20): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+12 more)

### Community 47 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (24): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+16 more)

### Community 48 - "vector"
Cohesion: 0.17
Nodes (10): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), oldUsed (+2 more)

### Community 49 - "Bootstrap"
Cohesion: 0.12
Nodes (21): AtPutFindsInternedKey, Bootstrap, BootstrapInstallsObjectIdentityEquals, GrowKeepsOuterOopAndEntries, InheritsFromSuperclass, Lookup, MethodDictionary, MissingSelectorIsNil (+13 more)

### Community 50 - "P8 — AppKit Tools"
Cohesion: 0.24
Nodes (10): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P7 — Image, .aoimage save/load, NativeMethod symbol rebind on load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+2 more)

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

### Community 55 - "cstring"
Cohesion: 0.25
Nodes (5): compiler, TEST(), CompilerSmoke, cstring, VersionIsNonEmpty

### Community 56 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 58 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 59 - "create"
Cohesion: 0.33
Nodes (6): at(), atPut(), uint32_t, WellKnown, create(), growInner()

### Community 60 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 66 - "P1-07: immovable old objects"
Cohesion: 0.33
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 73 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 74 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 75 - "stubEq"
Cohesion: 0.67
Nodes (3): CallContext, uint32_t, stubEq()

### Community 80 - "WellKnown.cpp"
Cohesion: 0.20
Nodes (10): string_view, NamedClass, name, WellKnown, WellKnown::addRoots(), WellKnown::classOf(), WellKnown::define(), WellKnown::intern() (+2 more)

## Knowledge Gaps
- **376 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+371 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 638 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `.nil`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `SmallInteger.cpp`, `Roots`, `Stream.cpp`, `WellKnown::InternTable`, `TEST`, `test_support.hpp`, `TEST`, `Heap`, `String.cpp`, `context`, `ClassMethodCache`, `bootstrap_test.cpp`, `TEST`, `.isHeap`, `Boot`, `Gc.cpp`, `putNative`, `Roots.cpp`, `native_send_test.cpp`, `TEST`, `create`, `vector`, `Bootstrap`, `ClassDef`, `intern`, `create`, `install`, `stubEq`, `WellKnown.cpp`?**
  _High betweenness centrality (0.505) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `.nil`, `Roots`, `Heap`, `Oop`, `bootstrap_test.cpp`?**
  _High betweenness centrality (0.172) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `.nil`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `TEST`, `Roots`, `Stream.cpp`, `TEST`, `test_support.hpp`, `TEST`, `String.cpp`, `context`, `Oop`, `ClassMethodCache`, `bootstrap_test.cpp`, `.isHeap`, `Boot`, `Gc.cpp`, `putNative`, `native_send_test.cpp`, `P1-06: weak slot GC`, `create`, `vector`, `Bootstrap`, `intern`, `create`, `install`, `TEST`, `WellKnown.cpp`?**
  _High betweenness centrality (0.123) - this node is a cross-community bridge._
- **Are the 18 inferred relationships involving `TEST()` (e.g. with `allocate` and `bytes`) actually correct?**
  _`TEST()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _376 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014184397163120567 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.082010582010582 - nodes in this community are weakly interconnected._
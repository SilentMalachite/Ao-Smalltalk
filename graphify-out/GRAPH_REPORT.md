# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 153 files · ~46,611 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 58 file(s) not represented in the graph (top: .st 47, (none) 10, .toml 1)

## Summary
- 1539 nodes · 3971 edges · 76 communities (63 shown, 13 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 377 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `c3d337ee`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- Bootstrap.cpp
- LargeInteger.cpp
- String.cpp
- Heap.cpp
- send
- TEST
- SPEC.md
- docs/README.md
- Bytecode interpreter
- BlockContext.cpp
- Roots
- Stream.cpp
- WellKnown.cpp
- TEST
- TEST
- P6b — vendor file-in
- TEST
- test_support.hpp
- TEST
- Heap
- P8 — AppKit Tools
- SmokeTests.swift
- Gc
- Oop
- ClassMethodCache
- P2 — Bootstrap
- TEST
- .isHeap
- Boot
- Roots.cpp
- ao_abi.h
- Float.cpp
- TEST
- Ao.app skeleton
- header
- TEST
- gc_test.cpp
- native_send_test.cpp
- TEST
- SmallInteger.cpp
- P9-04: v1 ゴールデン受け入れ
- P9-01: Do it / Print it / Inspect it
- P4-08 Point / Rectangle
- create
- P4 — Kernel Native Implementation
- P4-03 Magnitude / SmallInteger / Character
- P1 — Object Memory
- Bootstrap
- P2-01 WellKnown and immediates
- P2-02 class object skeletons
- intern
- P9-02: Browser accept
- ao image save
- P1-06: weak slot GC
- P9-03: エラー表示と VoiceOver
- ObjectHeader
- P1-07: immovable old objects
- ClassDef
- P4-04 Array / ByteArray / String / Symbol
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- heap
- oop
- Root
- Memory maintenance guide
- wellknown
- Root
- install
- NativeMethod
- build.sh
- test.sh

## God Nodes (most connected - your core abstractions)
1. `Oop` - 446 edges
2. `WellKnown` - 145 edges
3. `Heap` - 130 edges
4. `Roots` - 67 edges
5. `send()` - 54 edges
6. `TEST()` - 49 edges
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
- `マングル表` --references--> `ao_Float_divide()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/Float.cpp
- `マングル表` --references--> `modulo()`  [INFERRED]
  docs/native-selectors.md → runtime/src/kernel/LargeInteger.cpp

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

## Communities (76 total, 13 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.08
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 2 - "Bootstrap.cpp"
Cohesion: 0.39
Nodes (11): allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName(), run() (+3 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "String.cpp"
Cohesion: 0.06
Nodes (45): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio (+37 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.16
Nodes (17): cassert, fitsOld, align8(), byte, size_t, Heap::bytes(), Heap::fitsOld(), Heap::Heap() (+9 more)

### Community 6 - "send"
Cohesion: 0.09
Nodes (69): int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_() (+61 more)

### Community 7 - "TEST"
Cohesion: 0.06
Nodes (34): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+26 more)

### Community 8 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 9 - "docs/README.md"
Cohesion: 0.22
Nodes (16): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P9 — Integration, P0-01: git / LICENSE / PHASE / README, PHASE file marker, P0-02: directory skeleton, P0-03: CMake + GoogleTest + CLI (+8 more)

### Community 10 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 11 - "BlockContext.cpp"
Cohesion: 0.27
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 12 - "Roots"
Cohesion: 0.07
Nodes (25): StackWalker, uint32_t, Roots, dropHandle, free_, handleAt, handles_, live_ (+17 more)

### Community 13 - "Stream.cpp"
Cohesion: 0.13
Nodes (46): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+38 more)

### Community 14 - "WellKnown.cpp"
Cohesion: 0.11
Nodes (22): deque, size_t, string, string_view, Entry, cls, name, NamedClass (+14 more)

### Community 15 - "TEST"
Cohesion: 0.09
Nodes (22): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, format, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf (+14 more)

### Community 16 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 17 - "P6b — vendor file-in"
Cohesion: 0.18
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 18 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 19 - "test_support.hpp"
Cohesion: 0.08
Nodes (25): CharacterRoundTrip, cstdint, gc, globals, gtest, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns (+17 more)

### Community 20 - "TEST"
Cohesion: 0.07
Nodes (34): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+26 more)

### Community 21 - "Heap"
Cohesion: 0.06
Nodes (37): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+29 more)

### Community 22 - "P8 — AppKit Tools"
Cohesion: 0.24
Nodes (10): NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P7 — Image, .aoimage save/load, NativeMethod symbol rebind on load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+2 more)

### Community 23 - "SmokeTests.swift"
Cohesion: 0.22
Nodes (6): Ao, AoPlaceholder, SmokeTests, PackageDescription, XCTest, XCTestCase

### Community 24 - "Gc"
Cohesion: 0.22
Nodes (9): Gc, clearWeakAfterNursery, failed_, heap_, oldCompacted_, roots_, safepoint, scavengeFromRoots (+1 more)

### Community 25 - "Oop"
Cohesion: 0.05
Nodes (129): マングル表, natives, bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers() (+121 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.12
Nodes (16): HostOopHook, CallContext, cache, inspectHook, transcriptHook, ClassMethodCache, addRoots, entries (+8 more)

### Community 27 - "P2 — Bootstrap"
Cohesion: 0.20
Nodes (11): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, class skeleton, metaclass cycle, P2-03 metaclass cycle, Bootstrap (+3 more)

### Community 28 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 29 - ".isHeap"
Cohesion: 0.09
Nodes (72): clearWeakAfterOldMark, collectOld, copy, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc() (+64 more)

### Community 30 - "Boot"
Cohesion: 0.17
Nodes (11): floatValue(), makeFloat(), Boot, cache, ctx, heap, roots, wk (+3 more)

### Community 31 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 32 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 33 - "Float.cpp"
Cohesion: 0.07
Nodes (59): context, FlOp, install, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown (+51 more)

### Community 34 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), send0() (+5 more)

### Community 35 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 36 - "header"
Cohesion: 0.29
Nodes (10): header, uint16_t, uint32_t, Heap::allocate(), Heap::flags(), Heap::hash(), Heap::klass(), Heap::size() (+2 more)

### Community 37 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 38 - "gc_test.cpp"
Cohesion: 0.25
Nodes (7): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*)

### Community 39 - "native_send_test.cpp"
Cohesion: 0.36
Nodes (8): CallContext, NativeFn, uint32_t, WellKnown, install(), stubA(), stubB(), trueDnuSentinel()

### Community 40 - "TEST"
Cohesion: 0.17
Nodes (12): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, send1(), TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut (+4 more)

### Community 41 - "SmallInteger.cpp"
Cohesion: 0.20
Nodes (27): Native selector mangling, 規則, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+19 more)

### Community 42 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 43 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 44 - "P4-08 Point / Rectangle"
Cohesion: 0.29
Nodes (7): Bootstrap, runtime/src/kernel/Geometry.cpp, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send

### Community 45 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 46 - "P4 — Kernel Native Implementation"
Cohesion: 0.10
Nodes (21): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send (+13 more)

### Community 47 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (24): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+16 more)

### Community 48 - "P1 — Object Memory"
Cohesion: 0.29
Nodes (12): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+4 more)

### Community 49 - "Bootstrap"
Cohesion: 0.12
Nodes (20): AtPutFindsInternedKey, Bootstrap, BootstrapInstallsObjectIdentityEquals, GrowKeepsOuterOopAndEntries, InheritsFromSuperclass, Lookup, MethodDictionary, MissingSelectorIsNil (+12 more)

### Community 50 - "P2-01 WellKnown and immediates"
Cohesion: 0.22
Nodes (7): cstddef, Heap, Oop, P2-01 WellKnown and immediates, Roots, WellKnown, memory

### Community 51 - "P2-02 class object skeletons"
Cohesion: 0.22
Nodes (9): Behavior, Bootstrap, Class, ClassDescription, Heap, Metaclass, Object, P2-02 class object skeletons (+1 more)

### Community 52 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 53 - "P9-02: Browser accept"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 54 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 55 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 56 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 57 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): uint16_t, uint32_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 58 - "P1-07: immovable old objects"
Cohesion: 0.29
Nodes (6): Files, Interfaces, P1-07: immovable old objects, テスト, 手順, 目的

### Community 59 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 60 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 73 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 75 - "NativeMethod"
Cohesion: 0.25
Nodes (7): AddInternsByFunctionPointer, NameAndApply, NativeMethod, CallContext, uint32_t, stubEq(), TEST()

## Knowledge Gaps
- **376 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+371 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 653 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **13 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `Bootstrap.cpp`, `LargeInteger.cpp`, `String.cpp`, `Heap.cpp`, `send`, `BlockContext.cpp`, `Roots`, `Stream.cpp`, `WellKnown.cpp`, `TEST`, `test_support.hpp`, `TEST`, `Heap`, `Gc`, `ClassMethodCache`, `TEST`, `.isHeap`, `Boot`, `Roots.cpp`, `Float.cpp`, `TEST`, `header`, `gc_test.cpp`, `native_send_test.cpp`, `TEST`, `SmallInteger.cpp`, `create`, `Bootstrap`, `P2-01 WellKnown and immediates`, `intern`, `ObjectHeader`, `ClassDef`, `Root`, `Root`, `install`, `NativeMethod`?**
  _High betweenness centrality (0.487) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `P2-01 WellKnown and immediates`, `Roots`, `Heap`?**
  _High betweenness centrality (0.170) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `Bootstrap.cpp`, `LargeInteger.cpp`, `String.cpp`, `Heap.cpp`, `send`, `TEST`, `BlockContext.cpp`, `Stream.cpp`, `WellKnown.cpp`, `TEST`, `test_support.hpp`, `TEST`, `Gc`, `Oop`, `ClassMethodCache`, `.isHeap`, `Boot`, `Float.cpp`, `header`, `gc_test.cpp`, `native_send_test.cpp`, `SmallInteger.cpp`, `create`, `Bootstrap`, `P2-01 WellKnown and immediates`, `intern`, `P1-06: weak slot GC`, `ObjectHeader`, `install`, `NativeMethod`?**
  _High betweenness centrality (0.120) - this node is a cross-community bridge._
- **Are the 46 inferred relationships involving `send()` (e.g. with `installNatives()` and `ao_ArrayedCollection_new_()`) actually correct?**
  _`send()` has 46 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _376 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014084507042253521 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.07881773399014778 - nodes in this community are weakly interconnected._
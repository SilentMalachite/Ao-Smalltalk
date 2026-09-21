# Graph Report - p4-kernel  (2026-09-21)

## Corpus Check
- 155 files · ~48,302 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 60 file(s) not represented in the graph (top: .st 49, (none) 10, .toml 1)

## Summary
- 1581 nodes · 4139 edges · 77 communities (63 shown, 14 thin omitted)
- Extraction: 91% EXTRACTED · 9% INFERRED · 0% AMBIGUOUS · INFERRED: 389 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `4d2f4491`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- P3 — Native Dispatch
- Bootstrap.cpp
- LargeInteger.cpp
- Float.cpp
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
- P2-02 class object skeletons
- TEST
- .isHeap
- Boot
- Roots.cpp
- ao_abi.h
- putNative
- TEST
- Ao.app skeleton
- Behavior.cpp
- TEST
- String.cpp
- native_send_test.cpp
- TEST
- Geometry.cpp
- P9-04: v1 ゴールデン受け入れ
- P9-01: Do it / Print it / Inspect it
- TEST
- create
- P4 — Kernel Native Implementation
- P4-03 Magnitude / SmallInteger / Character
- P1 — Object Memory
- Bootstrap
- TEST
- P3-01 Symbol intern
- intern
- P9-02: Browser accept
- ao image save
- P1-06: weak slot GC
- P9-03: エラー表示と VoiceOver
- send1
- TEST
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
- Root
- install
- cstdint
- build.sh
- test.sh

## God Nodes (most connected - your core abstractions)
1. `Oop` - 473 edges
2. `WellKnown` - 145 edges
3. `Heap` - 131 edges
4. `Roots` - 69 edges
5. `send()` - 57 edges
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

## Communities (77 total, 14 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (138): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+130 more)

### Community 1 - "P3 — Native Dispatch"
Cohesion: 0.13
Nodes (14): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, class hierarchy, doesNotUnderstand:, lookup, MethodDictionary (+6 more)

### Community 2 - "Bootstrap.cpp"
Cohesion: 0.35
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "Float.cpp"
Cohesion: 0.07
Nodes (49): AbiSmoke, ao_abi, BootAndShutdownReturnZero, BootVersionShutdown, compiler, TEST(), CompilerSmoke, cstdio (+41 more)

### Community 5 - "Heap.cpp"
Cohesion: 0.08
Nodes (36): cassert, fitsOld, header, objectBytes, uint16_t, uint32_t, ObjectHeader, flags (+28 more)

### Community 6 - "send"
Cohesion: 0.09
Nodes (69): int64_t, InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_() (+61 more)

### Community 7 - "TEST"
Cohesion: 0.05
Nodes (41): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+33 more)

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
Cohesion: 0.08
Nodes (23): StackWalker, uint32_t, vector, Roots, dropHandle, free_, handleAt, handles_ (+15 more)

### Community 13 - "Stream.cpp"
Cohesion: 0.14
Nodes (44): allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset(), ao_SmalltalkImage_at_() (+36 more)

### Community 14 - "WellKnown.cpp"
Cohesion: 0.11
Nodes (22): deque, size_t, string, string_view, Entry, cls, name, NamedClass (+14 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

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
Cohesion: 0.23
Nodes (6): context, gc, globals, gtest, send, type_traits

### Community 20 - "TEST"
Cohesion: 0.10
Nodes (21): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+13 more)

### Community 21 - "Heap"
Cohesion: 0.08
Nodes (27): Heap, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_, nursery_ (+19 more)

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
Nodes (134): Native selector mangling, マングル表, 規則, install, natives, bits(), int64_t, instSize() (+126 more)

### Community 26 - "ClassMethodCache"
Cohesion: 0.10
Nodes (20): MethodDictionary, NativeMethod, P3-03 NativeMethod and selector mangling, selector mangling, HostOopHook, CallContext, cache, inspectHook (+12 more)

### Community 27 - "P2-02 class object skeletons"
Cohesion: 0.08
Nodes (27): cstddef, P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates (+19 more)

### Community 28 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 29 - ".isHeap"
Cohesion: 0.09
Nodes (72): clearWeakAfterOldMark, collectOld, copy, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::collectOld(), Gc::copy(), Gc::Gc() (+64 more)

### Community 30 - "Boot"
Cohesion: 0.15
Nodes (13): rect(), formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap (+5 more)

### Community 31 - "Roots.cpp"
Cohesion: 0.18
Nodes (12): algorithm, walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle() (+4 more)

### Community 32 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 33 - "putNative"
Cohesion: 0.10
Nodes (28): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+20 more)

### Community 34 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 35 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 36 - "Behavior.cpp"
Cohesion: 0.20
Nodes (28): collectNursery, allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes() (+20 more)

### Community 37 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 38 - "String.cpp"
Cohesion: 0.18
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 39 - "native_send_test.cpp"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, stubA(), stubB(), trueDnuSentinel()

### Community 40 - "TEST"
Cohesion: 0.17
Nodes (12): EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, send0(), TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut (+4 more)

### Community 41 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 42 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 43 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 44 - "TEST"
Cohesion: 0.10
Nodes (22): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, slotAt (+14 more)

### Community 45 - "create"
Cohesion: 0.31
Nodes (10): add(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown, create() (+2 more)

### Community 46 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (27): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send (+19 more)

### Community 47 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.08
Nodes (24): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+16 more)

### Community 48 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 49 - "Bootstrap"
Cohesion: 0.22
Nodes (11): AtPutFindsInternedKey, Bootstrap, GrowKeepsOuterOopAndEntries, Lookup, MethodDictionary, CallContext, uint32_t, stubA() (+3 more)

### Community 50 - "TEST"
Cohesion: 0.14
Nodes (14): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, allocate, bytes, flags, hash (+6 more)

### Community 51 - "P3-01 Symbol intern"
Cohesion: 0.20
Nodes (8): Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary, P3-02 MethodDictionary, Symbol

### Community 52 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 53 - "P9-02: Browser accept"
Cohesion: 0.25
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

### Community 57 - "send1"
Cohesion: 0.22
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), TEST() (+1 more)

### Community 58 - "TEST"
Cohesion: 0.25
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 59 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 60 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 73 - "install"
Cohesion: 0.50
Nodes (4): at(), string_view, WellKnown, install()

### Community 75 - "cstdint"
Cohesion: 0.14
Nodes (12): AddInternsByFunctionPointer, cstdint, NameAndApply, NativeMethod, CallContext, WellKnown, CallContext, uint32_t (+4 more)

## Knowledge Gaps
- **376 isolated node(s):** `AoPlaceholder`, `XCTest`, `Ao`, `PackageDescription`, `heap_` (+371 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 661 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **14 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `P3 — Native Dispatch`, `Bootstrap.cpp`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `BlockContext.cpp`, `Roots`, `Stream.cpp`, `WellKnown.cpp`, `test_support.hpp`, `TEST`, `Heap`, `Gc`, `ClassMethodCache`, `P2-02 class object skeletons`, `TEST`, `.isHeap`, `Boot`, `Roots.cpp`, `putNative`, `Behavior.cpp`, `String.cpp`, `native_send_test.cpp`, `TEST`, `Geometry.cpp`, `TEST`, `create`, `Bootstrap`, `TEST`, `P3-01 Symbol intern`, `intern`, `send1`, `TEST`, `ClassDef`, `Root`, `Root`, `Root`, `install`, `cstdint`?**
  _High betweenness centrality (0.486) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `P2-02 class object skeletons`, `Roots`, `Heap`?**
  _High betweenness centrality (0.166) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `P3 — Native Dispatch`, `Bootstrap.cpp`, `LargeInteger.cpp`, `Float.cpp`, `Heap.cpp`, `send`, `TEST`, `BlockContext.cpp`, `Stream.cpp`, `WellKnown.cpp`, `TEST`, `test_support.hpp`, `TEST`, `Gc`, `Oop`, `ClassMethodCache`, `P2-02 class object skeletons`, `.isHeap`, `Boot`, `putNative`, `String.cpp`, `native_send_test.cpp`, `TEST`, `create`, `Bootstrap`, `TEST`, `P3-01 Symbol intern`, `intern`, `P1-06: weak slot GC`, `install`, `cstdint`?**
  _High betweenness centrality (0.120) - this node is a cross-community bridge._
- **Are the 49 inferred relationships involving `send()` (e.g. with `installNatives()` and `ao_ArrayedCollection_new_()`) actually correct?**
  _`send()` has 49 INFERRED edges - model-reasoned connections that need verification._
- **What connects `AoPlaceholder`, `XCTest`, `Ao` to the rest of the system?**
  _376 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.014084507042253521 - nodes in this community are weakly interconnected._
- **Should `P3 — Native Dispatch` be split into smaller, more focused modules?**
  _Cohesion score 0.13333333333333333 - nodes in this community are weakly interconnected._
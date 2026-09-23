# Graph Report - p8-appkit  (2026-09-23)

## Corpus Check
- 225 files · ~91,520 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 77 file(s) not represented in the graph (top: .st 61, (none) 14, .toml 1)

## Summary
- 2807 nodes · 7308 edges · 147 communities (123 shown, 24 thin omitted)
- Extraction: 90% EXTRACTED · 10% INFERRED · 0% AMBIGUOUS · INFERRED: 706 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `8bd250a0`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Interpreter.cpp
- .isHeap
- LargeInteger.cpp
- image_save_load_test.cpp
- Token
- VendorExtract.cpp
- Object.cpp
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- putNative
- native_send_test.cpp
- SPEC.md
- Compile.cpp
- send
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- SmallInteger.cpp
- String.cpp
- Send.cpp
- P4-03 Magnitude / SmallInteger / Character
- Session.cpp
- ChunkAction
- TEST
- Bytecode interpreter
- TEST
- TEST
- ClassMethodCache
- gtest
- ChunkParser.cpp
- P3 — Native Dispatch
- CompileError
- TEST
- WellKnown
- P2-02 class object skeletons
- P6b — vendor file-in
- Trace
- P9-02: Browser accept
- TEST
- Ast
- docs/README.md
- CallContext
- P6b vendor file-in Implementation Plan
- TEST
- heap
- TEST
- BlockContext.cpp
- Roots.cpp
- TEST
- TEST
- Parser
- .nil
- ao_abi.h
- TEST
- TEST
- P8–P9 Remaining Implementation Plan
- roots
- Boot
- NativeMethod.cpp
- ToolWindowTests
- TEST
- wellknown
- BrowserWindow
- DiskHeader
- TEST
- TranscriptWindow
- WellKnown.cpp
- Ao.app skeleton
- P9-01: Do it / Print it / Inspect it
- P9-03: エラー表示と VoiceOver
- TEST
- Codegen.cpp
- ImageHeader
- TEST
- TEST
- ao image save
- string
- BrowserModel
- intern
- Behavior.cpp
- abi.cpp
- README.ja.md
- Globals.cpp
- TEST
- allocateRetry
- Gc
- build.sh
- test.sh
- collectNursery
- TEST
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- allocateRetry
- Task 1: セッションと SPEC の ABI 契約
- CompiledMethodNatives.cpp
- Memory maintenance guide
- string
- string_view
- DEFERRED.md
- AoApp
- ao_main.cpp
- BrowserModelTests
- ImageLoad.cpp
- .build
- gc
- .false_
- P4 — Kernel Native Implementation
- Boolean.cpp
- install
- AppKit
- Oop
- TEST_F
- Global Constraints
- NameCollect
- P9-04: v1 ゴールデン受け入れ
- ImageFormat
- uint32_t
- TEST
- Root
- Interpreter::run
- Root
- Root
- Root
- Root
- Root
- package-app.sh
- Array.cpp
- Frame
- consumeNonlocal
- 仕様
- stubA
- applyMethod
- Root
- DepthGuard
- stubEq

## God Nodes (most connected - your core abstractions)
1. `Oop` - 582 edges
2. `Heap` - 168 edges
3. `WellKnown` - 152 edges
4. `Roots` - 98 edges
5. `send()` - 70 edges
6. `Ast` - 65 edges
7. `Parser` - 55 edges
8. `TEST()` - 49 edges
9. `Emitter` - 37 edges
10. `Gc` - 37 edges

## Surprising Connections (you probably didn't know these)
- `範囲` --references--> `applyMethodsFor()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `前提` --references--> `installMethod()`  [INFERRED]
  docs/superpowers/plans/2026-09-23-p8-p9-remaining.md → runtime/src/Compile.cpp
- `前提` --references--> `applyChunks()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p6b-vendor-filein.md → runtime/src/Compile.cpp
- `CLI` --references--> `fileInLoadOrder()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/Compile.cpp
- `extra 節` --references--> `isCatalogName()`  [INFERRED]
  docs/superpowers/plans/2026-09-22-p7-aoimage.md → runtime/src/VendorExtract.cpp

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **P3 native dispatch path** — docs_phases_p3_lookup, docs_phases_p3_methoddictionary, docs_phases_p3_nativemethod, docs_phases_p3_dnu [EXTRACTED 1.00]
- **P1 object memory stack** — docs_phases_p1_ao_oop, docs_phases_p1_heap, docs_phases_p1_gc, docs_phases_p1_nursery [EXTRACTED 1.00]
- **P1 PR implementation chain** — docs_prs_p1_01, docs_prs_p1_02, docs_prs_p1_03, docs_phases_p1 [EXTRACTED 1.00]
- **Mandatory agent tooling trio** — concept_graphify, concept_serena, claude [EXTRACTED 1.00]
- **Runtime app ABI boundary** — concept_libao_runtime, concept_ao_app, concept_c_abi_boundary, bridge_ao_abi [EXTRACTED 1.00]
- **P2 bootstrap pipeline** — docs_prs_p2_01_pr, docs_prs_p2_02_pr, docs_prs_p2_03_pr, docs_prs_p2_04_pr [EXTRACTED 1.00]
- **P3 method dispatch pipeline** — docs_prs_p3_01_pr, docs_prs_p3_02_pr, docs_prs_p3_03_pr, docs_prs_p3_04_pr, docs_prs_p3_05_pr [EXTRACTED 1.00]
- **P4 Kernel NativeMethod suite** — docs_prs_p4_01_pr, docs_prs_p4_02_pr, docs_prs_p4_03_pr, docs_prs_p4_04_pr, docs_prs_p4_05_pr, docs_prs_p4_06_pr, docs_prs_p4_07_pr, docs_prs_p4_08_pr, docs_prs_p4_09_pr [EXTRACTED 1.00]
- **P5 compiler pipeline** — docs_prs_p5_01_scanner, docs_prs_p5_02_parser, docs_prs_p5_03_bytecode_isa, docs_prs_p5_04_codegen, docs_prs_p5_05_chunk_parser [EXTRACTED 1.00]
- **AOIM save/load and native rebind** — docs_prs_p7_01_aoimage_format, docs_prs_p7_02_image_save, docs_prs_p7_03_image_load, docs_prs_p7_02_nativemethod_symbol_names [EXTRACTED 1.00]
- **AppKit tool windows and menu** — docs_prs_p8_01_ao_app, docs_prs_p8_02_transcript_window, docs_prs_p8_03_workspace_window, docs_prs_p8_04_browser_window, docs_prs_p8_05_main_menu [EXTRACTED 1.00]
- **C ABI wrappers over ao::Runtime** — runtime_src_abi_ao_version, runtime_src_abi_ao_runtime_boot, runtime_src_abi_ao_runtime_shutdown [EXTRACTED 1.00]

## Communities (147 total, 24 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (145): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+137 more)

### Community 1 - "Interpreter.cpp"
Cohesion: 0.17
Nodes (18): safepoint, ActiveGuard, rootShared, saved, allocateRetry(), byteCount(), CallContext, int16_t (+10 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (61): allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size(), ao_Dictionary_at_() (+53 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "image_save_load_test.cpp"
Cohesion: 0.07
Nodes (37): ImageSave, ImageSaveLoad, iterator, KernelMethodsStayNative, NurseryGcAfterLoad, OnePlusTwoMatches, RejectsBadMagicAndSecondLoad, CallContext (+29 more)

### Community 5 - "Token"
Cohesion: 0.06
Nodes (53): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+45 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.08
Nodes (62): Task 4: LOAD_ORDER とホストパッチ, string, vector, VendorClassFile, chunkText, className, superName, unsupportedShape (+54 more)

### Community 7 - "Object.cpp"
Cohesion: 0.14
Nodes (44): ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals(), ao_Object_error_() (+36 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.12
Nodes (53): Task 2: Transcript のクラス側転送とフック, allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset() (+45 more)

### Community 9 - "Float.cpp"
Cohesion: 0.20
Nodes (28): FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_subtract() (+20 more)

### Community 10 - "Emitter"
Cohesion: 0.17
Nodes (10): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, cur_ (+2 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.05
Nodes (54): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, cassert, ファイル配置（すべて little-endian）, EachImageSlotLists127Names, ExtraListsCmUser (+46 more)

### Community 12 - "TEST"
Cohesion: 0.06
Nodes (33): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+25 more)

### Community 13 - "Literal"
Cohesion: 0.06
Nodes (35): CompileEnv, instVarNames, CompileResult, error, image, ok, int64_t, LitKind (+27 more)

### Community 14 - "putNative"
Cohesion: 0.10
Nodes (29): Task 8: printString, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown (+21 more)

### Community 15 - "native_send_test.cpp"
Cohesion: 0.60
Nodes (5): CallContext, uint32_t, stubA(), stubB(), trueDnuSentinel()

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "Compile.cpp"
Cohesion: 0.11
Nodes (24): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, compile, CompiledMethod, compiler, context, filesystem, fstream (+16 more)

### Community 18 - "send"
Cohesion: 0.19
Nodes (29): int64_t, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill() (+21 more)

### Community 19 - "Heap"
Cohesion: 0.06
Nodes (44): ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, PointerObjectHasClassAndSize, Heap, allocate, bytes, containsNurseryFrom (+36 more)

### Community 20 - "Roots"
Cohesion: 0.07
Nodes (26): StackWalker, uint32_t, uint8_t, vector, Roots, dropHandle, free_, handleAt (+18 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (27): allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+19 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (26): Native selector mangling, マングル表, 規則, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_() (+18 more)

### Community 25 - "String.cpp"
Cohesion: 0.19
Nodes (25): allocateRetry(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+17 more)

### Community 26 - "Send.cpp"
Cohesion: 0.17
Nodes (19): InlineCache, cachedClass, cachedMethod, lookup(), allocateMessage(), cacheIndex(), ClassMethodCache::addRoots(), ClassMethodCache::forget() (+11 more)

### Community 27 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 28 - "Session.cpp"
Cohesion: 0.10
Nodes (60): memory, Image, load, save, browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt() (+52 more)

### Community 29 - "ChunkAction"
Cohesion: 0.11
Nodes (19): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+11 more)

### Community 30 - "TEST"
Cohesion: 0.10
Nodes (22): BootstrapInstallsObjectIdentityEquals, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend, slotAt (+14 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "TEST"
Cohesion: 0.11
Nodes (19): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+11 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (20): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+12 more)

### Community 34 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, forget, insert, kSize, probe, Entry, klass (+3 more)

### Community 35 - "gtest"
Cohesion: 0.15
Nodes (12): AtPutFindsInternedKey, Bootstrap, chrono, Chunk, globals, GrowKeepsOuterOopAndEntries, gtest, Lookup (+4 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.20
Nodes (23): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, uint32_t, vector, firstLine() (+15 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.10
Nodes (21): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, MethodDictionary (+13 more)

### Community 38 - "CompileError"
Cohesion: 0.18
Nodes (27): CompileError, message, span, string, Task 3: rebind と上書き禁止, Task 5: `ao filein` と評価, accept, Task 10: accept (+19 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (19): BlockWithArgs, Cascade, ParseResult, error, method, ok, deque, parseMethod() (+11 more)

### Community 40 - "WellKnown"
Cohesion: 0.13
Nodes (10): Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary, P3-02 MethodDictionary, Symbol (+2 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.16
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 43 - "Trace"
Cohesion: 0.13
Nodes (27): clearWeakAfterOldMark, Gc::collectOld(), appendRaw(), appendRecord(), collectExtra(), collectImageSlot(), byte, size_t (+19 more)

### Community 44 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.12
Nodes (16): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+8 more)

### Community 47 - "docs/README.md"
Cohesion: 0.14
Nodes (23): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+15 more)

### Community 48 - "CallContext"
Cohesion: 0.17
Nodes (12): CallContext, activeContext, cache, inspectHook, interpretedBytecodes, nonlocalHome, nonlocalReturn, nonlocalValue (+4 more)

### Community 49 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.11
Nodes (17): Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 6: フェーズ完了, 仕様, 前提 (+9 more)

### Community 50 - "TEST"
Cohesion: 0.18
Nodes (9): CascadeAndBlock, Codegen, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), EighteenDigitIntegerSurvivesAsInt, ErrorSpanOnDanglingBinary, ReturnOnePlusTwo, SuperSendAndGlobal (+1 more)

### Community 52 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.22
Nodes (15): P4 microbench, P6 interpreter bench, ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn (+7 more)

### Community 54 - "Roots.cpp"
Cohesion: 0.20
Nodes (11): walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle(), Roots::remove() (+3 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 56 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 57 - "Parser"
Cohesion: 0.13
Nodes (18): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+10 more)

### Community 58 - ".nil"
Cohesion: 0.31
Nodes (25): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+17 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.15
Nodes (13): EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RewritesHostSelectorAndDefersMissingSuper, string, string_view (+5 more)

### Community 61 - "TEST"
Cohesion: 0.17
Nodes (12): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook, SmalltalkImageAtAndAtPut (+4 more)

### Community 62 - "P8–P9 Remaining Implementation Plan"
Cohesion: 0.13
Nodes (18): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, P8–P9 Remaining Implementation Plan, Task 4: Swift から ABI をリンクする, リスク, 仕様との対応, 前提 (+10 more)

### Community 64 - "Boot"
Cohesion: 0.13
Nodes (15): rect(), formatOf(), floatValue(), makeFloat(), Boot, cache, ctx, heap (+7 more)

### Community 65 - "NativeMethod.cpp"
Cohesion: 0.09
Nodes (32): CLI, extra 節, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save (+24 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.16
Nodes (9): Ao, SmokeTests, NSMenu, NSMenuItem, String, ToolWindowTests, PackageDescription, XCTest (+1 more)

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (9): BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST(), LineEndBangAfterBinaryStillTerminates, SplitsMethodsForAndProtocol (+1 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.14
Nodes (16): Any, BrowserWindow, .title, Notification, NSRect, NSSegmentedControl, NSTableView, NSTextView (+8 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "TranscriptWindow"
Cohesion: 0.13
Nodes (14): makeToolTextWindow(), Bool, NSRect, NSTextView, NSWindow, String, TranscriptWindow, .title (+6 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.08
Nodes (31): deque, size_t, string, string_view, unordered_map, Entry, cls, name (+23 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "Codegen.cpp"
Cohesion: 0.11
Nodes (23): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), LitKind, string (+15 more)

### Community 79 - "ImageHeader"
Cohesion: 0.11
Nodes (20): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+12 more)

### Community 80 - "TEST"
Cohesion: 0.25
Nodes (8): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, nativeRequiredDictsAreNative(), TEST(), TenMillionToDo, UserCompiledMethodDoesNotFailScan

### Community 81 - "TEST"
Cohesion: 0.25
Nodes (8): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), TEST()

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "string"
Cohesion: 0.11
Nodes (10): AddInternsByFunctionPointer, string, vector, string_view, cstddef, cstdint, NameAndApply, NativeMethod (+2 more)

### Community 84 - "BrowserModel"
Cohesion: 0.23
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "intern"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 86 - "Behavior.cpp"
Cohesion: 0.20
Nodes (27): allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 87 - "abi.cpp"
Cohesion: 0.18
Nodes (17): AoTranscriptFn, ObjectIsKernelAndPrintStringIsNative, ao_browser_class_at(), ao_browser_class_count(), ao_browser_class_definition(), ao_browser_protocol_at(), ao_browser_protocol_count(), ao_browser_selector_at() (+9 more)

### Community 88 - "README.ja.md"
Cohesion: 0.09
Nodes (19): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+11 more)

### Community 89 - "Globals.cpp"
Cohesion: 0.36
Nodes (7): at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 90 - "TEST"
Cohesion: 0.18
Nodes (11): AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, BlockEval, CopyDoesNotWriteOuter, ExampleFilePasses, IfTrueIfFalseFromCompiledMethod, NonLocalReturnSkipsRest (+3 more)

### Community 91 - "allocateRetry"
Cohesion: 0.27
Nodes (9): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, createBlock(), createMethod(), Root (+1 more)

### Community 92 - "Gc"
Cohesion: 0.11
Nodes (22): Gc, clearWeakAfterNursery, collectOld, copy, failed_, heap_, oldCompacted_, roots_ (+14 more)

### Community 95 - "collectNursery"
Cohesion: 0.22
Nodes (13): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, collectNursery, allocateRetry(), boxBytes(), boxLiteral() (+5 more)

### Community 96 - "TEST"
Cohesion: 0.40
Nodes (5): InternSameBytesIsIdentical, InternSurvivesNurseryGc, inOld, add, TEST()

### Community 97 - "TEST"
Cohesion: 0.18
Nodes (10): ChunkFileIn, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, KeepsNativeIdentityEquals, RebindsBagAndEvaluatesInstVar, RefusesNewSelectorOnObject, TEST() (+2 more)

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.35
Nodes (12): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), makeName() (+4 more)

### Community 104 - "allocateRetry"
Cohesion: 0.43
Nodes (6): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, create()

### Community 105 - "Task 1: セッションと SPEC の ABI 契約"
Cohesion: 0.24
Nodes (13): 範囲, Task 1: セッションと SPEC の ABI 契約, アプリ, セッション, 範囲, ao_filein_load_order(), ao_image_load(), ao_image_save() (+5 more)

### Community 106 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 111 - "AoApp"
Cohesion: 0.20
Nodes (6): AoApp, Notification, NSMenuItem, LaunchSet, Task 7: メニュー、アプリ束、P8 を閉じる, NSApplicationDelegate

### Community 112 - "ao_main.cpp"
Cohesion: 0.10
Nodes (25): algorithm, ao_abi, climits, TEST(), CompilerSmoke, cstdio, cstring, Task 5: CLI (+17 more)

### Community 113 - "BrowserModelTests"
Cohesion: 0.20
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, StaticString (+1 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.19
Nodes (35): acceptWord(), bindAll(), checkGlobals(), byte, size_t, string, string_view, T (+27 more)

### Community 115 - ".build"
Cohesion: 0.28
Nodes (9): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, NSObject, Selector (+1 more)

### Community 117 - ".false_"
Cohesion: 0.20
Nodes (16): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), CallContext, uint32_t, ao_Magnitude_between_and_() (+8 more)

### Community 118 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 119 - "Boolean.cpp"
Cohesion: 0.27
Nodes (23): ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_(), ao_False_not() (+15 more)

### Community 120 - "install"
Cohesion: 0.47
Nodes (5): install, CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 121 - "AppKit"
Cohesion: 0.18
Nodes (7): aoTranscriptHook(), CChar, Int32, AppKit, CAo, UnsafeMutableRawPointer, UnsafePointer

### Community 122 - "Oop"
Cohesion: 0.17
Nodes (14): bits(), int64_t, instSize(), isBytes(), isIndexable(), isPointers(), make(), uint64_t (+6 more)

### Community 123 - "TEST_F"
Cohesion: 0.29
Nodes (6): BootThenImageRoundTripKeepsOnePlusTwo, MissingLoadOrderIsError, testing::Test, SessionAbi, TEST_F(), SecondBootFailsUntilShutdown

### Community 124 - "Global Constraints"
Cohesion: 0.33
Nodes (7): Browser の読み取り, Global Constraints, Task 11: Workspace の Do it / Print it / Inspect it, Task 12: Browser の accept、階層、VoiceOver、v1 を閉じる, Task 3: Browser 読み取り ABI, Task 9: 評価とワークスペース変数, ao_browser_superclass()

### Community 125 - "NameCollect"
Cohesion: 0.20
Nodes (10): string, WellKnown, NameCollect, failed, out, rejectCatalog, wk, NamedOop (+2 more)

### Community 126 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 127 - "ImageFormat"
Cohesion: 0.13
Nodes (15): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+7 more)

### Community 128 - "uint32_t"
Cohesion: 0.22
Nodes (9): uint32_t, unique_ptr, RootedArray, data, n, Temps, n, roots (+1 more)

### Community 129 - "TEST"
Cohesion: 0.67
Nodes (3): LoadOrderEvaluatesLinkRoundTrip, TEST(), VendorFileIn

### Community 131 - "Interpreter::run"
Cohesion: 0.27
Nodes (6): deque, Interpreter::run(), OperandStack, roots, slots, performSend()

### Community 138 - "Array.cpp"
Cohesion: 0.38
Nodes (10): natives, ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size(), CallContext (+2 more)

### Community 139 - "Frame"
Cohesion: 0.22
Nodes (9): FieldRoots, frame, Frame, context, isBlock, method, pc, receiver (+1 more)

### Community 140 - "consumeNonlocal"
Cohesion: 0.38
Nodes (7): clearNonlocal(), consumeNonlocal(), hit(), Leave, leave, value, miss()

### Community 141 - "仕様"
Cohesion: 0.40
Nodes (5): C ABI, printString, Transcript, 仕様, 性能

### Community 142 - "stubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, stubA(), stubB()

### Community 143 - "applyMethod"
Cohesion: 1.00
Nodes (3): Interpreter, run, applyMethod()

### Community 146 - "stubEq"
Cohesion: 0.67
Nodes (3): CallContext, uint32_t, stubEq()

## Knowledge Gaps
- **601 isolated node(s):** `.metaFlag`, `.title`, `.title`, `.title`, `PackageDescription` (+596 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1113 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **24 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Interpreter.cpp`, `.isHeap`, `Root`, `Interpreter::run`, `uint32_t`, `Root`, `LargeInteger.cpp`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Heap.cpp`, `consumeNonlocal`, `Frame`, `putNative`, `applyMethod`, `Root`, `Compile.cpp`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `image_save_load_test.cpp`, `stubA`, `SmallInteger.cpp`, `String.cpp`, `Send.cpp`, `stubEq`, `Session.cpp`, `Root`, `TEST`, `ClassMethodCache`, `Root`, `Object.cpp`, `P3 — Native Dispatch`, `CompileError`, `gtest`, `WellKnown`, `P2-02 class object skeletons`, `Root`, `Trace`, `Root`, `CallContext`, `P6b vendor file-in Implementation Plan`, `BlockContext.cpp`, `Roots.cpp`, `TEST`, `.nil`, `Boot`, `NativeMethod.cpp`, `WellKnown.cpp`, `native_send_test.cpp`, `TEST`, `ImageHeader`, `TEST`, `string`, `intern`, `Behavior.cpp`, `abi.cpp`, `README.ja.md`, `Globals.cpp`, `allocateRetry`, `Gc`, `collectNursery`, `Bootstrap.cpp`, `allocateRetry`, `CompiledMethodNatives.cpp`, `ImageLoad.cpp`, `.false_`, `Boolean.cpp`, `install`, `NameCollect`?**
  _High betweenness centrality (0.438) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `image_save_load_test.cpp`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Heap.cpp`, `TEST`, `putNative`, `native_send_test.cpp`, `send`, `Roots`, `TEST`, `P1 — Object Memory`, `String.cpp`, `Send.cpp`, `Session.cpp`, `TEST`, `gtest`, `P3 — Native Dispatch`, `WellKnown`, `P2-02 class object skeletons`, `Trace`, `CallContext`, `BlockContext.cpp`, `.nil`, `P8–P9 Remaining Implementation Plan`, `Boot`, `NativeMethod.cpp`, `WellKnown.cpp`, `ImageHeader`, `string`, `intern`, `Globals.cpp`, `Gc`, `TEST`, `Bootstrap.cpp`, `Task 1: セッションと SPEC の ABI 契約`, `ao_main.cpp`, `ImageLoad.cpp`?**
  _High betweenness centrality (0.092) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `P2-02 class object skeletons`, `Heap`, `Roots`, `.false_`, `Oop`?**
  _High betweenness centrality (0.071) - this node is a cross-community bridge._
- **Are the 3 inferred relationships involving `Oop` (e.g. with `現状` and `browserSubclassAt()`) actually correct?**
  _`Oop` has 3 INFERRED edges - model-reasoned connections that need verification._
- **Are the 10 inferred relationships involving `Heap` (e.g. with `手順` and `load`) actually correct?**
  _`Heap` has 10 INFERRED edges - model-reasoned connections that need verification._
- **Are the 2 inferred relationships involving `Roots` (e.g. with `accept` and `セッション`) actually correct?**
  _`Roots` has 2 INFERRED edges - model-reasoned connections that need verification._
- **Are the 62 inferred relationships involving `send()` (e.g. with `Task 4: load と受け入れ評価` and `Task 5: CLI`) actually correct?**
  _`send()` has 62 INFERRED edges - model-reasoned connections that need verification._
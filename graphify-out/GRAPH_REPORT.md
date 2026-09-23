# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 249 files · ~142,966 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 3796 nodes · 9911 edges · 164 communities (146 shown, 18 thin omitted)
- Extraction: 87% EXTRACTED · 13% INFERRED · 0% AMBIGUOUS · INFERRED: 1325 edges (avg confidence: 0.87)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ba69856a`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Codegen.cpp
- .fromSmallInteger
- LargeInteger.cpp
- TEST
- Token
- VendorExtract.cpp
- Object.cpp
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- gc_safety_test.cpp
- TEST
- SPEC.md
- 指摘
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- SmallInteger.cpp
- collectNursery
- TEST
- WorkspaceWindow
- Session.cpp
- gc_stress_test.cpp
- TEST
- Bytecode interpreter
- NativeMethod.cpp
- TEST
- TEST
- gtest
- ChunkParser.cpp
- P3 — Native Dispatch
- CompileError
- TEST
- AcceptTests
- P2-02 class object skeletons
- P6b — vendor file-in
- ImageSave.cpp
- P9-01: Do it / Print it / Inspect it
- TEST
- Ast
- docs/README.md
- CallContext
- putNative
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST
- TEST
- TEST
- Parser
- send
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- Interpreter.cpp
- .isHeap
- ToolWindowTests
- TEST
- wellknown
- BrowserWindow
- DiskHeader
- TEST
- Image::load
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- ImageHeader
- Boot
- TEST
- ao image save
- vector
- BrowserModel
- 指摘
- .false_
- abi.cpp
- TEST
- VirtualRegion.cpp
- TEST
- 指摘
- ao_eval
- build.sh
- test.sh
- Ao
- natives
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- Claude レビュー指摘の修正計画（docs/claude-review）
- .isSmallInteger
- TEST_F
- Memory maintenance guide
- string
- string_view
- DEFERRED.md
- TranscriptWindow
- NativeMethod
- BrowserModelTests
- ImageLoad.cpp
- .build
- BlockProbe
- .isTrue
- Scanner
- Boolean.cpp
- 指摘
- AppKit
- GarbageFirstBoot
- ImageFormat.cpp
- 指摘
- TEST
- ObjectHeader
- ImageFormat
- codegen
- byte
- Claude レビュー指摘の修正計画（docs/claude-review）
- P4 — Kernel Native Implementation
- send2
- intern
- Loaded
- TEST
- 指摘
- package-app.sh
- Array.cpp
- Globals.cpp
- specialIndex
- TEST
- ParseResult
- TEST
- Roots.cpp
- RootedArray
- Scanner::Scanner
- Oop
- 横断テーマ
- uint64_t
- ao_image_load
- P9-02: Browser accept
- P9-04: v1 ゴールデン受け入れ
- interpreter
- 指摘
- ClassDef
- TEST
- Gc
- Gc.cpp
- imageRegistryStubA
- InspectorWindow
- native_send_test.cpp
- TEST
- expectSpecialSends

## God Nodes (most connected - your core abstractions)
1. `Oop` - 689 edges
2. `Heap` - 209 edges
3. `WellKnown` - 164 edges
4. `vector` - 106 edges
5. `Roots` - 97 edges
6. `Ast` - 93 edges
7. `TEST()` - 87 edges
8. `TEST()` - 73 edges
9. `Boot` - 68 edges
10. `allocateRetry()` - 67 edges

## Surprising Connections (you probably didn't know these)
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp
- `Status` --references--> `Oop`  [INFERRED]
  README.md → runtime/include/ao/Oop.hpp
- `[High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）` --references--> `boxLiteral()`  [INFERRED]
  docs/claude-review/03-kernel-numeric.md → runtime/src/Compile.cpp

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

## Communities (164 total, 18 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (151): array, ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass (+143 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (52): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+44 more)

### Community 2 - ".fromSmallInteger"
Cohesion: 0.12
Nodes (59): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Bag_size() (+51 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "TEST"
Cohesion: 0.09
Nodes (23): ImageSave, ImageSaveLoad, KernelMethodsStayNative, LoadSizesOldFromHeader, NurseryGcAfterLoad, OnePlusTwoMatches, RejectsBadMagicAndSecondLoad, RejectsHeapBytesAboveOldMax (+15 more)

### Community 5 - "Token"
Cohesion: 0.10
Nodes (34): cmath, int64_t, string, Tok, Token, intValue, isFloat, kind (+26 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.08
Nodes (61): Task 4: LOAD_ORDER とホストパッチ, string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult (+53 more)

### Community 7 - "Object.cpp"
Cohesion: 0.12
Nodes (51): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+43 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.13
Nodes (49): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset() (+41 more)

### Community 9 - "Float.cpp"
Cohesion: 0.21
Nodes (27): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_printString() (+19 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.10
Nodes (40): charconv, growOld, header, initObject, objectBytes, oldUsed, align8(), byte (+32 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (49): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+41 more)

### Community 13 - "Literal"
Cohesion: 0.09
Nodes (23): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+15 more)

### Community 14 - "gc_safety_test.cpp"
Cohesion: 0.08
Nodes (36): climits, cstdio, dyld, filesystem, fstream, runtime, ao executable, ao_runtime library (+28 more)

### Community 15 - "TEST"
Cohesion: 0.17
Nodes (12): InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, KernelBench, KernelInstall, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, nativeRequiredDictsAreNative() (+4 more)

### Community 16 - "SPEC.md"
Cohesion: 0.06
Nodes (44): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+36 more)

### Community 17 - "指摘"
Cohesion: 0.13
Nodes (19): CompileEnv, instVarNames, knownGlobals, undeclaredAreBindings, [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる, [High] 2^63 以上の整数リテラルが黙って 0 になる, [High] クラス変数を読み書きできない, [Low] カスケードの扱いが不完全 (+11 more)

### Community 18 - "ChunkAction"
Cohesion: 0.12
Nodes (18): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+10 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (47): Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_, fromStart_ (+39 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (40): Root, slot, FrameBlock, capacity, slots, used, size_t, StackWalker (+32 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.26
Nodes (26): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x(), ao_Point_x_y_() (+18 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (28): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+20 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (26): Native selector mangling, マングル表, 規則, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+18 more)

### Community 25 - "collectNursery"
Cohesion: 0.36
Nodes (10): collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld() (+2 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (56): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+48 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.09
Nodes (25): failureText(), installErrorField(), sendToKeyWorkspace(), spanMessage(), AoSpan, Bool, Int, Int32 (+17 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (93): [High] `Smalltalk` が knownGlobals に含まれない, SPEC を先に直す, SPEC を先に直す, Image, load, save, clearUnwinding(), blankOut() (+85 more)

### Community 29 - "gc_stress_test.cpp"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 30 - "TEST"
Cohesion: 0.13
Nodes (20): DnuWithFullNurseryReturnsMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, allocate, allocateTenured, setGcStress (+12 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "NativeMethod.cpp"
Cohesion: 0.09
Nodes (34): CLI, extra 節, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save (+26 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.11
Nodes (19): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+11 more)

### Community 35 - "gtest"
Cohesion: 0.12
Nodes (25): Bootstrap, chrono, Chunk, compile, CompiledMethod, compiler, context, cstring (+17 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.20
Nodes (23): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, uint32_t, firstLine(), firstLineHas() (+15 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 38 - "CompileError"
Cohesion: 0.11
Nodes (43): CompileError, message, span, string, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, P6b vendor file-in Implementation Plan (+35 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (12): BlockWithArgs, Cascade, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma (+4 more)

### Community 40 - "AcceptTests"
Cohesion: 0.19
Nodes (7): AcceptTests, NSMenu, NSSegmentedControl, NSTableView, NSView, String, T

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.11
Nodes (36): [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, encodeNonHeap, appendRaw(), appendRecord(), collectExtra(), collectImageSlot(), byte, size_t (+28 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.12
Nodes (22): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+14 more)

### Community 47 - "docs/README.md"
Cohesion: 0.16
Nodes (21): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (33): BindingHook, CallContext, aborting, abortReason, activeContext, bindingHook, cache, cleanupDepth (+25 more)

### Community 49 - "putNative"
Cohesion: 0.09
Nodes (32): WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter(), installCollection(), WellKnown (+24 more)

### Community 50 - ".nil"
Cohesion: 0.24
Nodes (28): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+20 more)

### Community 52 - "TEST"
Cohesion: 0.07
Nodes (27): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+19 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.17
Nodes (28): B2 `to:do:` bench, P4 microbench, P6 interpreter bench, safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs() (+20 more)

### Community 54 - "TEST"
Cohesion: 0.10
Nodes (19): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+11 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 56 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (20): uint32_t, SourceSpan, end, start, deque, Kind, string, string_view (+12 more)

### Community 58 - "send"
Cohesion: 0.15
Nodes (41): InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan() (+33 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.15
Nodes (13): EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RewritesHostSelectorAndDefersMissingSuper, string, string_view (+5 more)

### Community 61 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 62 - "String.cpp"
Cohesion: 0.18
Nodes (25): Task 1: 形式ヘッダと即値ビット, ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_printString(), ao_String_size(), ao_Symbol_asString() (+17 more)

### Community 64 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (61): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, ファイルごとの変更, ファイルごとの変更, pthread, run, ActiveGuard, rootShared, saved (+53 more)

### Community 65 - ".isHeap"
Cohesion: 0.07
Nodes (34): B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）, SPEC を先に直す, その他（同じバッチで処理する）, テスト（書く順。各段を緑にしてから次へ進む）, リスク, 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical） (+26 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.30
Nodes (6): SmokeTests, NSMenu, NSMenuItem, String, ToolWindowTests, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (9): BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST(), LineEndBangAfterBinaryStillTerminates, SplitsMethodsForAndProtocol (+1 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.09
Nodes (26): Any, BrowserWindow, .errorText, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser(), Bool (+18 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "Image::load"
Cohesion: 0.27
Nodes (12): checkGlobals(), string, string_view, WellKnown, extraNamesOk(), findRecord(), globalNamesOk(), Image::load() (+4 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (36): string_view, isVendorStub(), internWith, deque, size_t, string, string_view, unordered_map (+28 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.18
Nodes (11): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, ExtraListsCmUser, ImageRegistry, RememberSymbolRegistersWithoutAllocating (+3 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.07
Nodes (38): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, countOp(), Inline, LiteralArrayPseudoObjectsAreNotSymbols, MethodImage, Op (+30 more)

### Community 79 - "ImageHeader"
Cohesion: 0.18
Nodes (11): uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes, nextHash (+3 more)

### Community 80 - "Boot"
Cohesion: 0.05
Nodes (46): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, string (+38 more)

### Community 81 - "TEST"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "vector"
Cohesion: 0.07
Nodes (16): algorithm, ao_abi, cassert, string, string_view, vector, cstddef, cstdint (+8 more)

### Community 84 - "BrowserModel"
Cohesion: 0.21
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "指摘"
Cohesion: 0.17
Nodes (12): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る, [Low] `>=` を `(a < b) not` で計算するため、NaN に対して true を返す (+4 more)

### Community 86 - ".false_"
Cohesion: 0.19
Nodes (25): boolean(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+17 more)

### Community 87 - "abi.cpp"
Cohesion: 0.12
Nodes (27): AcceptAbi, AcceptRefusesKernelClassThroughAlias, AcceptRefusesShadowingInheritedNativeInKernelClass, AoTranscriptFn, ClassDefinitionThenImageDropsSourceText, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectFooIsCallableAndKeepsSource (+19 more)

### Community 88 - "TEST"
Cohesion: 0.06
Nodes (36): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+28 more)

### Community 89 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (72): AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, BlockAbort, BlockActivation, BlockAssignmentWritesOuterTemp, BlockEval (+64 more)

### Community 91 - "指摘"
Cohesion: 0.13
Nodes (14): 07 Swift アプリ・ビルド・スクリプト, [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い, [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる, [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される, [Low] Workspace の評価を中断できない（無限ループでアプリが固まる）, [Medium] 64 KiB を超える Print it / Inspect it は副作用だけ残して失敗する (+6 more)

### Community 92 - "ao_eval"
Cohesion: 0.07
Nodes (36): 00 直近差分（origin/main..HEAD の 6 コミット）, 概要, 01 オブジェクトメモリ・GC・ブートストラップ, テストの穴, 概要, 02 インタプリタ・送信・コンテキスト・プロセス, テストの穴, 概要 (+28 more)

### Community 95 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, Build and test, Documentation, Layout, License, Requirements, Status

### Community 96 - "natives"
Cohesion: 0.44
Nodes (9): natives, ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext (+1 more)

### Community 97 - "TEST"
Cohesion: 0.40
Nodes (5): ChunkFileIn, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, TEST()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.32
Nodes (13): [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors() (+5 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.09
Nodes (21): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+13 more)

### Community 105 - ".isSmallInteger"
Cohesion: 0.31
Nodes (15): CompiledMethod accessors (P5), int64_t, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive() (+7 more)

### Community 106 - "TEST_F"
Cohesion: 0.07
Nodes (27): BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, ClassDefinedAfterBindingWins, DeadHomeBlockDoesNotAbortLaterEval, EvalTranscriptShowThenCr, GlobalWinsOverBinding, HugeAllocationReportsOutOfMemory, InspectItPrintsAndNotifiesHook (+19 more)

### Community 111 - "TranscriptWindow"
Cohesion: 0.08
Nodes (23): AoApp, openImageFile(), saveImageFile(), Int32, Notification, NSMenuItem, LaunchSet, makeToolTextWindow() (+15 more)

### Community 112 - "NativeMethod"
Cohesion: 0.16
Nodes (17): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, NativeMethod, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn (+9 more)

### Community 113 - "BrowserModelTests"
Cohesion: 0.20
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, StaticString (+1 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.33
Nodes (14): acceptWord(), bindAll(), uint64_t, unordered_set, fileOop(), heapShaped(), bits, payloadFits() (+6 more)

### Community 115 - ".build"
Cohesion: 0.27
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 117 - ".isTrue"
Cohesion: 0.42
Nodes (8): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin(), truthOf()

### Community 118 - "Scanner"
Cohesion: 0.14
Nodes (20): ArrayAndByteArrayHeaders, AssignVariantsAndComment, uint32_t, Scanner, i_, lexBinary, lexCharacter, lexIdentOrKeyword (+12 more)

### Community 119 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_() (+18 more)

### Community 120 - "指摘"
Cohesion: 0.12
Nodes (17): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む, [Low] Symbol の `copy` が、intern されていない別の Symbol を作る (+9 more)

### Community 121 - "AppKit"
Cohesion: 0.13
Nodes (10): Ao, aoTranscriptHook(), CChar, Int32, UnsafeMutableRawPointer, UnsafePointer, AppKit, CAo (+2 more)

### Community 122 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 123 - "ImageFormat.cpp"
Cohesion: 0.24
Nodes (9): bit, byte, size_t, uint64_t, ImageFormat::decodeNonHeap(), ImageFormat::encodeNonHeap(), ImageFormat::readHeader(), ImageFormat::writeFiller() (+1 more)

### Community 124 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 125 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 126 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 127 - "ImageFormat"
Cohesion: 0.14
Nodes (14): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, kImageEndianLittle, kImageFillerBytes, kImageHeaderBytes (+6 more)

### Community 128 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 129 - "byte"
Cohesion: 0.43
Nodes (8): byte, size_t, T, uint32_t, parseRecord(), parseRecords(), readPod(), walkObjects()

### Community 130 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.08
Nodes (24): バッチの順序と依存, B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review） (+16 more)

### Community 131 - "P4 — Kernel Native Implementation"
Cohesion: 0.04
Nodes (55): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+47 more)

### Community 132 - "send2"
Cohesion: 0.11
Nodes (20): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+12 more)

### Community 133 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 134 - "Loaded"
Cohesion: 0.13
Nodes (15): size_t, string_view, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), Loaded, cache, ctx (+7 more)

### Community 135 - "TEST"
Cohesion: 0.10
Nodes (24): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+16 more)

### Community 136 - "指摘"
Cohesion: 0.15
Nodes (13): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる (+5 more)

### Community 138 - "Array.cpp"
Cohesion: 0.27
Nodes (17): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+9 more)

### Community 139 - "Globals.cpp"
Cohesion: 0.33
Nodes (8): adoptImageClass(), at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 142 - "ParseResult"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 148 - "Oop"
Cohesion: 0.12
Nodes (19): bits(), int64_t, isBytes(), isIndexable(), isPointers(), make(), uint32_t, isClassShaped() (+11 more)

### Community 150 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 152 - "ao_image_load"
Cohesion: 0.06
Nodes (53): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, [High] Save / Open Image の失敗がユーザーに見えない, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない (+45 more)

### Community 153 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 154 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 156 - "指摘"
Cohesion: 0.25
Nodes (8): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 指摘

### Community 158 - "ClassDef"
Cohesion: 0.18
Nodes (11): Global Constraints, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 6: フェーズ完了, ClassDef, bytes, indexable, instSize (+3 more)

### Community 159 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 160 - "Gc"
Cohesion: 0.18
Nodes (13): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, copy, heap_, roots_, scavengeCanProgress (+5 more)

### Community 161 - "Gc.cpp"
Cohesion: 0.24
Nodes (9): cstdlib, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::Gc(), Gc::scavengeFromRoots() (+1 more)

### Community 164 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 166 - "InspectorWindow"
Cohesion: 0.18
Nodes (10): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), CChar, UnsafeMutableRawPointer (+2 more)

### Community 169 - "native_send_test.cpp"
Cohesion: 0.57
Nodes (6): CallContext, uint32_t, pairAfterAlloc(), stubA(), stubB(), trueDnuSentinel()

### Community 170 - "TEST"
Cohesion: 0.50
Nodes (4): CompileErrorsDoNotGrow, LoadOrderEvaluatesLinkRoundTrip, TEST(), VendorFileIn

### Community 172 - "expectSpecialSends"
Cohesion: 1.00
Nodes (3): CallContext, expectSpecialSends(), runSource()

## Knowledge Gaps
- **784 isolated node(s):** `.metaFlag`, `.title`, `.sourceText`, `.errorText`, `.paneAccessibilityLabels` (+779 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1561 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **18 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.fromSmallInteger`, `LargeInteger.cpp`, `Claude レビュー指摘の修正計画（docs/claude-review）`, `intern`, `send2`, `TEST`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Globals.cpp`, `Heap.cpp`, `Object.cpp`, `gc_safety_test.cpp`, `TEST`, `SPEC.md`, `指摘`, `RootedArray`, `Heap`, `Roots`, `Geometry.cpp`, `Roots.cpp`, `TEST`, `SmallInteger.cpp`, `collectNursery`, `TEST`, `TEST`, `Session.cpp`, `gc_stress_test.cpp`, `ClassDef`, `TEST`, `NativeMethod.cpp`, `TEST`, `TEST`, `Loaded`, `imageRegistryStubA`, `gtest`, `CompileError`, `native_send_test.cpp`, `ImageSave.cpp`, `expectSpecialSends`, `CallContext`, `putNative`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST`, `TEST`, `send`, `String.cpp`, `Interpreter.cpp`, `.isHeap`, `WellKnown.cpp`, `TEST`, `Boot`, `vector`, `.false_`, `abi.cpp`, `TEST`, `TEST`, `Ao`, `natives`, `Bootstrap.cpp`, `.isSmallInteger`, `NativeMethod`, `ImageLoad.cpp`, `BlockProbe`, `.isTrue`, `Boolean.cpp`, `GarbageFirstBoot`, `ImageFormat.cpp`, `TEST`, `ObjectHeader`?**
  _High betweenness centrality (0.389) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `byte`, `LargeInteger.cpp`, `VendorExtract.cpp`, `Object.cpp`, `Emitter`, `Literal`, `gc_safety_test.cpp`, `指摘`, `ChunkAction`, `Oop`, `Roots`, `Session.cpp`, `NativeMethod.cpp`, `Gc.cpp`, `gtest`, `ChunkParser.cpp`, `CompileError`, `ImageSave.cpp`, `Ast`, `Parser`, `Image::load`, `TEST`, `.false_`, `ImageLoad.cpp`?**
  _High betweenness centrality (0.133) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `.isHeap`, `WellKnown.cpp`, `Heap`, `vector`, `Oop`, `Roots`, `uint64_t`, `.false_`?**
  _High betweenness centrality (0.088) - this node is a cross-community bridge._
- **Are the 6 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 6 INFERRED edges - model-reasoned connections that need verification._
- **Are the 5 inferred relationships involving `Roots` (e.g. with `B2 `to:do:` bench` and `01 オブジェクトメモリ・GC・ブートストラップ`) actually correct?**
  _`Roots` has 5 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.title`, `.sourceText` to the rest of the system?**
  _784 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.012987012987012988 - nodes in this community are weakly interconnected._
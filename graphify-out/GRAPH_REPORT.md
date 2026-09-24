# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 251 files · ~163,490 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 4070 nodes · 10758 edges · 171 communities (151 shown, 20 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1495 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `18c06db0`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- RealScope
- .isHeap
- LargeInteger.cpp
- TEST
- Scanner.cpp
- VendorExtract.cpp
- Object.cpp
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- ao_main.cpp
- TEST
- SPEC.md
- ao_eval
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- Boot
- KeptClass
- TEST
- WorkspaceWindow
- Session.cpp
- TranscriptWindow
- Oop
- Bytecode interpreter
- NativeMethod.cpp
- TEST
- TEST
- string
- ChunkParser.cpp
- P3 — Native Dispatch
- vector
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
- ao_runtime_boot
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- BrowserModelTests
- Parser
- send
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- clearUnwinding
- allocateRetry
- ToolWindowTests
- TEST
- wellknown
- BrowserWindow
- DiskHeader
- Loaded
- 01 オブジェクトメモリ・GC・ブートストラップ
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- Token
- Boolean.cpp
- TEST
- ao image save
- cstdint
- BrowserModel
- TEST
- .false_
- TEST
- TEST
- VirtualRegion.cpp
- TEST
- collectNursery
- abi.cpp
- build.sh
- test.sh
- Ao
- RootedArray
- ao_Magnitude_lessOrEqual
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- Claude レビュー指摘の修正計画（docs/claude-review）
- CompiledMethodNatives.cpp
- TEST_F
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- extractVendor
- ImageLoad.cpp
- .build
- TEST
- DefinitionScanner
- P7 .aoimage Implementation Plan
- SmallInteger.cpp
- ClassMethodCache
- AppKit
- natives
- ObjectHeader
- 指摘
- Interpreter.cpp
- P9-04: v1 ゴールデン受け入れ
- ImageFormat
- 仕様
- TEST
- Format.hpp
- P4-03 Magnitude / SmallInteger / Character
- NameCollect
- TEST
- Ao
- TEST
- 指摘
- package-app.sh
- Array.cpp
- Globals.cpp
- Codegen.cpp
- Gc.cpp
- P1-06: weak slot GC
- TEST
- Roots.cpp
- Contributing
- .n
- intern
- 指摘
- TEST
- 貢献
- uint64_t
- ao_image_load
- P9-02: Browser accept
- TEST
- interpreter
- .isEmpty
- P4 — Kernel Native Implementation
- ParseResult
- WellKnown::checkSmallIntegerFastPath
- Gc
- takeAbortReason
- WellKnown.hpp
- Send.cpp
- ClassDef
- runOnSmallStack
- cli_test.sh
- TEST
- TestDir
- TEST
- stubA

## God Nodes (most connected - your core abstractions)
1. `Oop` - 718 edges
2. `Heap` - 208 edges
3. `WellKnown` - 164 edges
4. `vector` - 120 edges
5. `Roots` - 97 edges
6. `Ast` - 93 edges
7. `TEST()` - 93 edges
8. `BrowserWindow` - 86 edges
9. `Boot` - 74 edges
10. `TEST()` - 73 edges

## Surprising Connections (you probably didn't know these)
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp
- `Status` --references--> `Oop`  [INFERRED]
  README.md → runtime/include/ao/Oop.hpp
- `[High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）` --references--> `boxLiteral()`  [INFERRED]
  docs/claude-review/03-kernel-numeric.md → runtime/src/Compile.cpp
- `[High] 2^63 以上の整数リテラルが黙って 0 になる` --references--> `boxLiteral()`  [INFERRED]
  docs/claude-review/05-compiler.md → runtime/src/Compile.cpp

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

## Communities (171 total, 20 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (150): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+142 more)

### Community 1 - "RealScope"
Cohesion: 0.08
Nodes (27): Capture, owner, var, uint32_t, RealScope, copied, copiedBase, isBlock (+19 more)

### Community 2 - ".isHeap"
Cohesion: 0.11
Nodes (64): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, int64_t, smallIntegerAnswer(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value() (+56 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "TEST"
Cohesion: 0.08
Nodes (28): FailedProbeKeepsCurrentSession, ImageSave, ImageSaveLoad, KernelMethodsStayNative, LoadSizesOldFromHeader, NurseryGcAfterLoad, OnePlusTwoMatches, RejectsBadMagicAndSecondLoad (+20 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.05
Nodes (60): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+52 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.18
Nodes (24): allowIndex(), containsHostWord(), string_view, firstLineKey(), firstNonEmptyLine(), HostMethod, protocol, source (+16 more)

### Community 7 - "Object.cpp"
Cohesion: 0.12
Nodes (52): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+44 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.06
Nodes (80): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean() (+72 more)

### Community 9 - "Float.cpp"
Cohesion: 0.20
Nodes (28): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_printString() (+20 more)

### Community 10 - "Emitter"
Cohesion: 0.14
Nodes (11): int16_t, Op, size_t, uint16_t, uint8_t, Emitter, real_, size (+3 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.08
Nodes (45): charconv, allocateTenured, fitsOld, growOld, header, initObject, largeObjectBytes, objectBytes (+37 more)

### Community 12 - "TEST"
Cohesion: 0.05
Nodes (47): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+39 more)

### Community 13 - "Literal"
Cohesion: 0.06
Nodes (36): Codegen, CompileEnv, instVarNames, knownGlobals, undeclaredAreBindings, CompileResult, error, image (+28 more)

### Community 14 - "ao_main.cpp"
Cohesion: 0.14
Nodes (19): climits, dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests() (+11 more)

### Community 15 - "TEST"
Cohesion: 0.11
Nodes (18): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall, KernelScan (+10 more)

### Community 16 - "SPEC.md"
Cohesion: 0.11
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "ao_eval"
Cohesion: 0.11
Nodes (25): AoSpan, end, message, start, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, 07 Swift アプリ・ビルド・スクリプト, [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない (+17 more)

### Community 18 - "ChunkAction"
Cohesion: 0.10
Nodes (21): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+13 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (47): Heap, bytes, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_ (+39 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (46): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, Root, slot, FrameBlock, capacity, slots, used, size_t (+38 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.07
Nodes (54): 03 数値・スカラー系 Kernel ネイティブ, [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point と Rectangle のネイティブがサブクラスを扱えない, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+46 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (28): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+20 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "Boot"
Cohesion: 0.03
Nodes (91): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, CompileError, message, span (+83 more)

### Community 25 - "KeptClass"
Cohesion: 0.15
Nodes (25): size_t, string, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars (+17 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (67): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, chrono, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops (+59 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.06
Nodes (37): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), installErrorField() (+29 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (88): Image, load, save, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt() (+80 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.10
Nodes (23): AoTranscriptFn, aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect (+15 more)

### Community 30 - "Oop"
Cohesion: 0.08
Nodes (29): uint64_t, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, abortingSubclass() (+21 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "NativeMethod.cpp"
Cohesion: 0.35
Nodes (12): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+4 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.10
Nodes (21): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+13 more)

### Community 35 - "string"
Cohesion: 0.12
Nodes (29): algorithm, ao_abi, Bootstrap, Chunk, compile, CompiledMethod, compiler, string (+21 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.16
Nodes (28): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, Token, uint32_t, firstLine() (+20 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 38 - "vector"
Cohesion: 0.08
Nodes (76): cctype, vector, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, テストの穴, Task 3: rebind と上書き禁止 (+68 more)

### Community 39 - "TEST"
Cohesion: 0.15
Nodes (12): BlockWithArgs, Cascade, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), ErrorSpanDoesNotTouchOkFlag, LiteralArrayAndByteArray, LiteralArrayKeepsSeparateKeywords, PrimitivePragma (+4 more)

### Community 40 - "AcceptTests"
Cohesion: 0.14
Nodes (11): AcceptTests, Bool, NSMenu, NSSegmentedControl, NSTableView, NSTextView, NSView, String (+3 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.13
Nodes (19): CompiledMethod, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter (+11 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.14
Nodes (28): Gc::collectOld(), Heap::checkNotPoisoned(), Heap::inNursery(), Heap::inOld(), appendRaw(), appendRecord(), collectExtra(), collectImageSlot() (+20 more)

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
Cohesion: 0.08
Nodes (24): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+16 more)

### Community 49 - "ao_runtime_boot"
Cohesion: 0.13
Nodes (19): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, B2 `to:do:` bench, P4 microbench, P6 interpreter bench, P8–P9 Remaining Implementation Plan, リスク (+11 more)

### Community 50 - ".nil"
Cohesion: 0.24
Nodes (28): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+20 more)

### Community 52 - "TEST"
Cohesion: 0.08
Nodes (24): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+16 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.22
Nodes (23): safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_() (+15 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (22): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+14 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewAbortsWithShouldNotImplement, MetaclassNewIsForbidden (+5 more)

### Community 56 - "BrowserModelTests"
Cohesion: 0.22
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 57 - "Parser"
Cohesion: 0.11
Nodes (21): uint32_t, SourceSpan, end, start, deque, Kind, string, string_view (+13 more)

### Community 58 - "send"
Cohesion: 0.15
Nodes (39): int64_t, decodeHeader(), ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count() (+31 more)

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

### Community 64 - "clearUnwinding"
Cohesion: 0.22
Nodes (16): refreshStackLimit(), abortReasonText(), clearUnwinding(), string, installEmptyWorkspace(), sessionWorkspaceReset(), CallContext, path (+8 more)

### Community 65 - "allocateRetry"
Cohesion: 0.05
Nodes (46): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む, [Low] Symbol の `copy` が、intern されていない別の Symbol を作る (+38 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.17
Nodes (12): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ChunksAfterSectionEndAreFlagged, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST() (+4 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.08
Nodes (28): Any, BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title (+20 more)

### Community 70 - "DiskHeader"
Cohesion: 0.09
Nodes (26): bit, byte, size_t, uint16_t, uint32_t, uint64_t, DiskHeader, endian (+18 more)

### Community 71 - "Loaded"
Cohesion: 0.13
Nodes (15): size_t, string_view, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), Loaded, cache, ctx (+7 more)

### Community 72 - "01 オブジェクトメモリ・GC・ブートストラップ"
Cohesion: 0.18
Nodes (11): 01 オブジェクトメモリ・GC・ブートストラップ, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない (+3 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (34): internWith, deque, size_t, string, string_view, unordered_map, Entry, cls (+26 more)

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
Cohesion: 0.15
Nodes (13): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, OnePlusTwoIsThree (+5 more)

### Community 78 - "TEST"
Cohesion: 0.07
Nodes (38): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, countOp(), Inline, LiteralArrayPseudoObjectsAreNotSymbols, MethodImage, Op (+30 more)

### Community 79 - "Token"
Cohesion: 0.20
Nodes (10): int64_t, string, Tok, Token, intValue, isFloat, kind, number (+2 more)

### Community 80 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_() (+18 more)

### Community 81 - "TEST"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "cstdint"
Cohesion: 0.09
Nodes (8): string_view, cstddef, cstdint, memory, NativeMethod, CallContext, CallContext, WellKnown

### Community 84 - "BrowserModel"
Cohesion: 0.16
Nodes (13): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, SmokeTests (+5 more)

### Community 85 - "TEST"
Cohesion: 0.12
Nodes (14): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+6 more)

### Community 86 - ".false_"
Cohesion: 0.20
Nodes (25): boolean(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+17 more)

### Community 87 - "TEST"
Cohesion: 0.05
Nodes (49): AcceptAbi, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition, AcceptClassStopsAtRefusedDefinitionKeepingEarlierChunks, AcceptClassTakesChunkDefinitionAndMethods, AcceptMethodRefusesNonBehaviorName (+41 more)

### Community 88 - "TEST"
Cohesion: 0.05
Nodes (57): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+49 more)

### Community 89 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (71): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+63 more)

### Community 91 - "collectNursery"
Cohesion: 0.36
Nodes (10): collectNursery, collectOld, Gc::collectBeforeTenured(), Gc::stressPoint(), CallContext, uint32_t, forceNursery(), forceOld() (+2 more)

### Community 92 - "abi.cpp"
Cohesion: 0.22
Nodes (18): [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, B3 失敗の伝搬とキャッシュの無効化（テーマ 2）, B5 Browser と Workspace のデータ消失（テーマ 4）, accept, Task 10: accept, B3 失敗の伝搬とキャッシュの無効化（テーマ 2） (+10 more)

### Community 95 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, Build and test, Documentation, Layout, License, Requirements, Status

### Community 96 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 97 - "ao_Magnitude_lessOrEqual"
Cohesion: 0.58
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.32
Nodes (13): [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors() (+5 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.13
Nodes (14): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context, SPEC §3.4 に書く意味論, バッチの順序と依存 (+6 more)

### Community 105 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 106 - "TEST_F"
Cohesion: 0.06
Nodes (30): ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, ClassDefinedAfterBindingWins, DeadHomeBlockAbortsWithReason, Task 11: Workspace の Do it / Print it / Inspect it, EvalTranscriptShowThenCr, EvalWithoutOutBufferRefusesBeforeEvaluating (+22 more)

### Community 111 - "AoApp"
Cohesion: 0.10
Nodes (16): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+8 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "extractVendor"
Cohesion: 0.11
Nodes (21): Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 4: LOAD_ORDER とホストパッチ, Task 6: フェーズ完了, 仕様 (+13 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (34): acceptWord(), bindAll(), checkGlobals(), byte, size_t, string, string_view, T (+26 more)

### Community 115 - ".build"
Cohesion: 0.27
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "TEST"
Cohesion: 0.22
Nodes (9): CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, DynamicReasonSurvivesCollections, FailureAbortBoot, FailureOutermost, TEST(), SendToEmptyReceiverAborts, StaticReasonNeedsNoAllocation (+1 more)

### Community 117 - "DefinitionScanner"
Cohesion: 0.23
Nodes (9): DefinitionScanner, Bool, Int32, String, Token, keyword, other, Equatable (+1 more)

### Community 118 - "P7 .aoimage Implementation Plan"
Cohesion: 0.17
Nodes (12): Global Constraints, P7 .aoimage Implementation Plan, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 4: load と受け入れ評価, Task 6: フェーズを閉じる, リスク, 仕様との対応 (+4 more)

### Community 119 - "SmallInteger.cpp"
Cohesion: 0.24
Nodes (26): Native selector mangling, マングル表, 規則, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+18 more)

### Community 120 - "ClassMethodCache"
Cohesion: 0.10
Nodes (19): ClassMethodCache, entries, flushAll, flushSelector, insert, kSize, probe, Entry (+11 more)

### Community 121 - "AppKit"
Cohesion: 0.18
Nodes (5): Ao, AppKit, CAo, PackageDescription, XCTest

### Community 122 - "natives"
Cohesion: 0.44
Nodes (9): natives, ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext (+1 more)

### Community 123 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 124 - "指摘"
Cohesion: 0.15
Nodes (12): 02 インタプリタ・送信・コンテキスト・プロセス, [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する (+4 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.06
Nodes (54): ファイルごとの変更, ファイルごとの変更, pthread, ActiveGuard, rootShared, saved, answerWithoutSend(), branchTruth() (+46 more)

### Community 126 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 127 - "ImageFormat"
Cohesion: 0.08
Nodes (26): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, encodeNonHeap (+18 more)

### Community 128 - "仕様"
Cohesion: 0.15
Nodes (14): [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, B6 イメージとセッションの堅牢化, CLI, extra 節, globals 節, load, OOP エンコード, save (+6 more)

### Community 129 - "TEST"
Cohesion: 0.25
Nodes (8): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, MethodErrorFailsFileInButKeepsGoing, TEST()

### Community 130 - "Format.hpp"
Cohesion: 0.29
Nodes (10): [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる, B4 クラスのメタデータ（Kernel の形と名前）, B4 クラスのメタデータ（Kernel の形と名前）, bits(), int64_t, instSize(), isBytes(), isIndexable() (+2 more)

### Community 131 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 132 - "NameCollect"
Cohesion: 0.20
Nodes (10): string, WellKnown, NameCollect, failed, out, rejectCatalog, wk, NamedOop (+2 more)

### Community 133 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 134 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 135 - "TEST"
Cohesion: 0.09
Nodes (25): cstdlib, EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical (+17 more)

### Community 136 - "指摘"
Cohesion: 0.14
Nodes (13): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+5 more)

### Community 138 - "Array.cpp"
Cohesion: 0.31
Nodes (15): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+7 more)

### Community 139 - "Globals.cpp"
Cohesion: 0.33
Nodes (8): adoptImageClass(), at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 140 - "Codegen.cpp"
Cohesion: 0.07
Nodes (39): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), Analysis, declared (+31 more)

### Community 141 - "Gc.cpp"
Cohesion: 0.21
Nodes (10): copy, size_t, uintptr_t, unordered_set, Gc::clearWeakAfterNursery(), Gc::clearWeakAfterOldMark(), Gc::copy(), Gc::Gc() (+2 more)

### Community 142 - "P1-06: weak slot GC"
Cohesion: 0.29
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.13
Nodes (16): cassert, walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock() (+8 more)

### Community 145 - "Contributing"
Cohesion: 0.33
Nodes (6): Build, Commits, Contributing, Current phase, License, Rules that do not bend

### Community 146 - ".n"
Cohesion: 0.40
Nodes (3): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, int64_t, ocAt()

### Community 147 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 148 - "指摘"
Cohesion: 0.17
Nodes (11): 05 コンパイラ（スキャナ・パーサ・コード生成・チャンク）, [High] 2^63 以上の整数リテラルが黙って 0 になる, [Low] カスケードの扱いが不完全, [Low] 宣言の検証がない（引数への代入・擬変数名・重複名）, [Low] 指数表記の意味が Smalltalk-80 と異なる, [Medium] 二項演算子の直後の負数リテラルが、別のセレクタとして字句化される, [Medium] 文字列の中の `!!` が `!` に戻らない, [Medium] 構文の入れ子に深さの上限がなく、コンパイル時にスタックが溢れる (+3 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "貢献"
Cohesion: 0.33
Nodes (6): コミット, ビルド, ライセンス, 曲げない規則, 現行フェーズ, 貢献

### Community 152 - "ao_image_load"
Cohesion: 0.09
Nodes (30): [High] Save / Open Image の失敗がユーザーに見えない, 06 イメージ・セッション・C ABI・CLI, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする, [Low] `ao_version` が切り詰め時に AO_ERR_RANGE ではなく AO_ERR を返す, [Low] ロード前にファイル全体を読み、ヘッダ検証が後になる (+22 more)

### Community 153 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 154 - "TEST"
Cohesion: 0.40
Nodes (5): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, TEST()

### Community 156 - ".isEmpty"
Cohesion: 0.40
Nodes (4): [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, expectAbortedEmpty(), expectNonBooleanAbort(), runSource()

### Community 157 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 158 - "ParseResult"
Cohesion: 0.50
Nodes (4): ParseResult, error, method, ok

### Community 160 - "Gc"
Cohesion: 0.18
Nodes (12): Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectBeforeTenured, heap_, roots_, scavengeCanProgress, scavengeFromRoots (+4 more)

### Community 161 - "takeAbortReason"
Cohesion: 0.67
Nodes (3): expectFailAbort(), string, takeAbortReason()

### Community 163 - "Send.cpp"
Cohesion: 0.09
Nodes (38): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む） (+30 more)

### Community 164 - "ClassDef"
Cohesion: 0.29
Nodes (7): ClassDef, bytes, indexable, instSize, name, WellKnown, int64_t

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 170 - "TEST"
Cohesion: 0.11
Nodes (16): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+8 more)

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 172 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 175 - "stubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, stubA(), stubB()

## Knowledge Gaps
- **808 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+803 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1682 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **20 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Format.hpp`, `.isHeap`, `NameCollect`, `LargeInteger.cpp`, `Ao`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Globals.cpp`, `Heap.cpp`, `Gc.cpp`, `TEST`, `TEST`, `Roots.cpp`, `TEST`, `.n`, `Heap`, `Roots`, `Geometry.cpp`, `intern`, `TEST`, `Boot`, `TEST`, `TEST`, `.isEmpty`, `TranscriptWindow`, `Session.cpp`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `NativeMethod.cpp`, `WellKnown.hpp`, `Send.cpp`, `ClassDef`, `takeAbortReason`, `vector`, `string`, `TEST`, `ImageSave.cpp`, `stubA`, `CallContext`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `send`, `String.cpp`, `clearUnwinding`, `allocateRetry`, `DiskHeader`, `Loaded`, `WellKnown.cpp`, `TEST`, `Boolean.cpp`, `cstdint`, `.false_`, `TEST`, `collectNursery`, `abi.cpp`, `Ao`, `RootedArray`, `ao_Magnitude_lessOrEqual`, `Bootstrap.cpp`, `CompiledMethodNatives.cpp`, `native_method_test.cpp`, `ImageLoad.cpp`, `SmallInteger.cpp`, `ClassMethodCache`, `natives`, `ObjectHeader`, `Interpreter.cpp`?**
  _High betweenness centrality (0.391) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `RealScope`, `LargeInteger.cpp`, `NameCollect`, `VendorExtract.cpp`, `Object.cpp`, `Codegen.cpp`, `Literal`, `ao_main.cpp`, `Gc.cpp`, `ChunkAction`, `Roots`, `Boot`, `KeptClass`, `TEST`, `Session.cpp`, `NativeMethod.cpp`, `string`, `ChunkParser.cpp`, `ImageSave.cpp`, `Ast`, `Parser`, `clearUnwinding`, `TEST`, `cstdint`, `.false_`, `extractVendor`, `ImageLoad.cpp`?**
  _High betweenness centrality (0.126) - this node is a cross-community bridge._
- **Why does `BrowserWindow` connect `BrowserWindow` to `ToolWindowTests`, `AcceptTests`, `AoApp`, `BrowserModel`, `DefinitionScanner`, `BrowserModelTests`, `AppKit`, `WorkspaceWindow`, `TranscriptWindow`?**
  _High betweenness centrality (0.093) - this node is a cross-community bridge._
- **Are the 14 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 14 INFERRED edges - model-reasoned connections that need verification._
- **Are the 5 inferred relationships involving `Roots` (e.g. with `B2 `to:do:` bench` and `01 オブジェクトメモリ・GC・ブートストラップ`) actually correct?**
  _`Roots` has 5 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _808 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013071895424836602 - nodes in this community are weakly interconnected._
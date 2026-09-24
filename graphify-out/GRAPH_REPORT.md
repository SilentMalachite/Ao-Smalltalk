# Graph Report - ao-smalltalk  (2026-09-24)

## Corpus Check
- 251 files · ~152,944 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 78 file(s) not represented in the graph (top: .st 63, (none) 13, .toml 1)

## Summary
- 3943 nodes · 10284 edges · 171 communities (152 shown, 19 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1402 edges (avg confidence: 0.87)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `e9ac4bbc`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Codegen.cpp
- .isHeap
- LargeInteger.cpp
- image_save_load_test.cpp
- Token
- VendorExtract.cpp
- Oop
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- ao_main.cpp
- TEST
- SPEC.md
- 指摘
- ChunkAction
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- Boot
- TEST
- TEST
- WorkspaceWindow
- Session
- TranscriptWindow
- native_send_test.cpp
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
- RealScope
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- Parser
- send
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- Scanner
- allocateRetry
- ToolWindowTests
- TEST
- wellknown
- BrowserWindow
- DiskHeader
- abortingSubclass
- .compileAssign
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- TEST
- ImageHeader
- classRows
- TEST
- ao image save
- cstdint
- BrowserModel
- Lookup.hpp
- .false_
- abi.cpp
- TEST
- VirtualRegion.cpp
- TEST
- TEST
- ao_eval
- build.sh
- test.sh
- Ao
- KeptClass
- TEST
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
- .applicationWillFinishLaunching
- native_method_test.cpp
- BrowserModelTests
- ImageLoad.cpp
- .build
- send2
- string
- .advance
- SmallInteger.cpp
- GarbageFirstBoot
- AppKit
- Session.cpp
- WellKnown::InternTable
- 指摘
- Interpreter.cpp
- VendorClassFile
- ImageFormat
- codegen
- SourceSpan
- 貢献
- P4 — Kernel Native Implementation
- NameCollect
- TEST
- P4-04 Array / ByteArray / String / Symbol
- TEST
- 指摘
- package-app.sh
- Array.cpp
- Globals.cpp
- specialIndex
- Session.hpp
- P1-06: weak slot GC
- TEST
- Roots.cpp
- RootedArray
- 指摘
- intern
- 指摘
- TEST
- 横断テーマ
- uint64_t
- ClassMethodCache
- P9-02: Browser accept
- evalBody
- interpreter
- ListedMethod
- P4-07 Process / ProcessorScheduler / Semaphore
- KeptMethod
- imageRegistryStubA
- Gc
- NameBag
- WellKnown::checkSmallIntegerFastPath
- ClassDef
- cli_test.sh
- TEST
- TestDir
- TEST
- ParseResult
- answerOne
- abortingNew

## God Nodes (most connected - your core abstractions)
1. `Oop` - 708 edges
2. `Heap` - 208 edges
3. `WellKnown` - 164 edges
4. `vector` - 112 edges
5. `Roots` - 97 edges
6. `Ast` - 93 edges
7. `TEST()` - 93 edges
8. `TEST()` - 73 edges
9. `Boot` - 73 edges
10. `allocateRetry()` - 67 edges

## Surprising Connections (you probably didn't know these)
- `[Low] `AoSpan` の区間を捨てているので、エラー位置が分からない` --references--> `AoSpan`  [INFERRED]
  docs/claude-review/07-app-build.md → bridge/ao_abi.h
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `手順` --references--> `Gc`  [INFERRED]
  docs/prs/P1-06.md → runtime/include/ao/Gc.hpp
- `現状` --references--> `Oop`  [INFERRED]
  README.ja.md → runtime/include/ao/Oop.hpp
- `Status` --references--> `Oop`  [INFERRED]
  README.md → runtime/include/ao/Oop.hpp

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

## Communities (171 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (151): array, ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass (+143 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.10
Nodes (25): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+17 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (62): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+54 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.13
Nodes (53): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+45 more)

### Community 4 - "image_save_load_test.cpp"
Cohesion: 0.06
Nodes (46): FailedProbeKeepsCurrentSession, ImageSave, ImageSaveLoad, iterator, KernelMethodsStayNative, LoadSizesOldFromHeader, NurseryGcAfterLoad, OnePlusTwoMatches (+38 more)

### Community 5 - "Token"
Cohesion: 0.10
Nodes (32): cmath, int64_t, string, Tok, Token, intValue, isFloat, kind (+24 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.20
Nodes (27): allowIndex(), containsHostWord(), string, string_view, extractVendor(), firstLineKey(), firstNonEmptyLine(), HostMethod (+19 more)

### Community 7 - "Oop"
Cohesion: 0.09
Nodes (66): [High] printOn: が新しい printString を使わない, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, Gc::copy() (+58 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.07
Nodes (78): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean() (+70 more)

### Community 9 - "Float.cpp"
Cohesion: 0.11
Nodes (40): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+32 more)

### Community 10 - "Emitter"
Cohesion: 0.17
Nodes (11): int16_t, Op, size_t, uint16_t, Emitter, real_, opName(), size (+3 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (43): charconv, allocateTenured, growOld, header, initObject, largeObjectBytes, objectBytes, oldUsed (+35 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (47): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+39 more)

### Community 13 - "Literal"
Cohesion: 0.10
Nodes (22): int64_t, LitKind, uint16_t, uint8_t, unique_ptr, Literal, bytes, elements (+14 more)

### Community 14 - "ao_main.cpp"
Cohesion: 0.23
Nodes (15): climits, Task 5: CLI, dyld, addRoots, bootAndRunTests(), string, imageUsage(), main() (+7 more)

### Community 15 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelBench, KernelInstall, KernelScan (+12 more)

### Community 16 - "SPEC.md"
Cohesion: 0.06
Nodes (41): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+33 more)

### Community 17 - "指摘"
Cohesion: 0.13
Nodes (15): InspectorWindow, .text, NSTextView, NSWindow, String, [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い (+7 more)

### Community 18 - "ChunkAction"
Cohesion: 0.10
Nodes (21): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+13 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (54): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+46 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (48): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, Root, slot, FrameBlock, capacity, slots, used, size_t (+40 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.19
Nodes (32): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, Bootstrap, NativeMethod, Point, P4-08 Point / Rectangle, Rectangle, send, ao_Point_add() (+24 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (28): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+20 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.10
Nodes (25): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+17 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (50): [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, initializer_list, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray (+42 more)

### Community 25 - "TEST"
Cohesion: 0.08
Nodes (25): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+17 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (56): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+48 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (28): aoWorkspaceInspectHook(), failureText(), installErrorField(), sendToKeyWorkspace(), spanMessage(), Bool, CChar, Int (+20 more)

### Community 28 - "Session"
Cohesion: 0.09
Nodes (24): browserClassCount(), collectKnownGlobals(), CallContext, string, uint64_t, unique_ptr, WellKnown, MethodSource (+16 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (16): aoTranscriptHook(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView, NSWindow (+8 more)

### Community 30 - "native_send_test.cpp"
Cohesion: 0.29
Nodes (11): inline cache, lookup, P3-05 inline cache and send, send, answerMessage(), CallContext, uint32_t, pairAfterAlloc() (+3 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "NativeMethod.cpp"
Cohesion: 0.07
Nodes (41): CLI, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save, Task 2: 再結合に必要なヒープとレジストリの口 (+33 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.11
Nodes (19): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+11 more)

### Community 35 - "string"
Cohesion: 0.12
Nodes (29): algorithm, ao_abi, Bootstrap, chrono, Chunk, compile, CompiledMethod, compiler (+21 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.11
Nodes (35): atLineEnd(), bangSpaceBangAt(), classify(), string, string_view, uint32_t, firstLine(), firstLineHas() (+27 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.13
Nodes (17): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, MethodDictionary (+9 more)

### Community 38 - "vector"
Cohesion: 0.11
Nodes (55): cctype, CompileError, message, span, string, vector, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す (+47 more)

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
Cohesion: 0.16
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.19
Nodes (24): appendRaw(), appendRecord(), collectExtra(), collectImageSlot(), byte, size_t, string_view, uint64_t (+16 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.12
Nodes (16): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的 (+8 more)

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.12
Nodes (22): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+14 more)

### Community 47 - "docs/README.md"
Cohesion: 0.15
Nodes (23): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+15 more)

### Community 48 - "CallContext"
Cohesion: 0.08
Nodes (24): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+16 more)

### Community 49 - "RealScope"
Cohesion: 0.09
Nodes (24): uint32_t, RealScope, copied, copiedBase, isBlock, node, numArgs, numTemps (+16 more)

### Community 50 - ".nil"
Cohesion: 0.12
Nodes (43): clearWeakAfterOldMark, int64_t, makeName(), wireClass(), Gc::collectOld(), isZero(), ao_BlockContext_fork(), ao_MethodContext_method() (+35 more)

### Community 52 - "TEST"
Cohesion: 0.07
Nodes (29): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+21 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.20
Nodes (25): safepoint, ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_() (+17 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (23): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+15 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewAbortsWithShouldNotImplement, MetaclassNewIsForbidden (+5 more)

### Community 56 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.12
Nodes (17): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む） (+9 more)

### Community 57 - "Parser"
Cohesion: 0.17
Nodes (11): Kind, join(), Parser, cur_, error_, hadError_, prev_, queued_ (+3 more)

### Community 58 - "send"
Cohesion: 0.10
Nodes (59): run, applyMethod(), ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count() (+51 more)

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
Cohesion: 0.21
Nodes (24): ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_printString(), ao_String_size(), ao_Symbol_asString(), ao_Symbol_at_put_() (+16 more)

### Community 64 - "Scanner"
Cohesion: 0.13
Nodes (20): ArrayAndByteArrayHeaders, AssignVariantsAndComment, uint32_t, Scanner, i_, lexBinary, lexCharacter, lexIdentOrKeyword (+12 more)

### Community 65 - "allocateRetry"
Cohesion: 0.09
Nodes (29): B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）, SPEC を先に直す, その他（同じバッチで処理する）, テスト（書く順。各段を緑にしてから次へ進む）, リスク, 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`）, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical） (+21 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.30
Nodes (6): SmokeTests, NSMenu, NSMenuItem, String, ToolWindowTests, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.17
Nodes (12): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ChunksAfterSectionEndAreFlagged, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST() (+4 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.08
Nodes (29): Any, BrowserWindow, .errorText, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser(), Bool (+21 more)

### Community 70 - "DiskHeader"
Cohesion: 0.11
Nodes (19): uint16_t, uint32_t, uint64_t, DiskHeader, endian, extraCount, globalCount, headerBytes (+11 more)

### Community 71 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 72 - ".compileAssign"
Cohesion: 0.18
Nodes (7): Capture, owner, var, string_view, uint8_t, nameIn(), utf8Scalar()

### Community 73 - "WellKnown.cpp"
Cohesion: 0.11
Nodes (22): extra 節, internWith, isCatalogName(), string_view, ImageSelector, name, WellKnown, NamedClass (+14 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.18
Nodes (11): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, ExtraListsCmUser, ImageRegistry, RememberSymbolRegistersWithoutAllocating (+3 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.22
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.07
Nodes (38): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, countOp(), Inline, LiteralArrayPseudoObjectsAreNotSymbols, MethodImage, Op (+30 more)

### Community 79 - "ImageHeader"
Cohesion: 0.13
Nodes (17): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+9 more)

### Community 80 - "classRows"
Cohesion: 0.28
Nodes (19): browserClassAt(), browserClassDefinition(), browserProtocolAt(), browserProtocolCount(), browserSelectorAt(), browserSelectorCount(), browserSource(), browserSubclassAt() (+11 more)

### Community 81 - "TEST"
Cohesion: 0.15
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "cstdint"
Cohesion: 0.06
Nodes (22): cassert, string_view, cstddef, cstdint, Heap, Oop, P3-01 Symbol intern, Symbol (+14 more)

### Community 84 - "BrowserModel"
Cohesion: 0.20
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "Lookup.hpp"
Cohesion: 0.32
Nodes (5): uint32_t, isClassShaped(), superclassOf(), SuperclassWalk, depth_

### Community 86 - ".false_"
Cohesion: 0.16
Nodes (31): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+23 more)

### Community 87 - "abi.cpp"
Cohesion: 0.04
Nodes (74): AbiSmoke, AcceptAbi, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition, AcceptClassTakesChunkDefinitionAndMethods, AcceptMethodRefusesNonBehaviorName, AcceptRefusesKernelClassThroughAlias (+66 more)

### Community 88 - "TEST"
Cohesion: 0.09
Nodes (28): BootstrapInstallsObjectIdentityEquals, DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, InheritsFromSuperclass, MissingSelectorIsNil, NativeSend (+20 more)

### Community 89 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (73): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+65 more)

### Community 91 - "TEST"
Cohesion: 0.20
Nodes (10): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, NinthMethodWithFullNurseryIsInstalled, MethodImage, defineEightMethodClass() (+2 more)

### Community 92 - "ao_eval"
Cohesion: 0.06
Nodes (44): AoSpan, end, message, start, 01 オブジェクトメモリ・GC・ブートストラップ, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する (+36 more)

### Community 95 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, Build and test, Documentation, Layout, License, Requirements, Status

### Community 96 - "KeptClass"
Cohesion: 0.21
Nodes (17): size_t, dropCycles(), dropMissingSupers(), findActive(), KeptClass, category, classVars, deferred (+9 more)

### Community 97 - "TEST"
Cohesion: 0.25
Nodes (8): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, MethodErrorFailsFileInButKeepsGoing, TEST()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.42
Nodes (10): format, allocateSkeletons(), allocClass(), WellKnown, ensureMethodDict(), installNatives(), internHotSelectors(), run() (+2 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.05
Nodes (38): isBinaryChar(), [High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B6 イメージとセッションの堅牢化, B7 コンパイラの構文とチャンク形式 (+30 more)

### Community 105 - "CompiledMethodNatives.cpp"
Cohesion: 0.53
Nodes (11): CompiledMethod accessors (P5), ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive(), CallContext (+3 more)

### Community 106 - "TEST_F"
Cohesion: 0.07
Nodes (27): ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, ClassDefinedAfterBindingWins, DeadHomeBlockAbortsWithReason, EvalTranscriptShowThenCr, EvalWithoutOutBufferRefusesBeforeEvaluating, FileInLoadOrderFailsOnUndeferredError (+19 more)

### Community 111 - ".applicationWillFinishLaunching"
Cohesion: 0.15
Nodes (10): AoApp, openImageFile(), saveImageFile(), Int32, Notification, NSMenuItem, LaunchSet, Task 7: メニュー、アプリ束、P8 を閉じる (+2 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "BrowserModelTests"
Cohesion: 0.20
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, StaticString (+1 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.16
Nodes (35): readHeader, acceptWord(), bindAll(), checkGlobals(), byte, size_t, string, string_view (+27 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (9): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, NSObject (+1 more)

### Community 116 - "send2"
Cohesion: 0.11
Nodes (19): ClassDefinitionThroughAliasOnlyRebindsGlobal, Geometry, KeepsNativeIdentityEquals, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RebindsBagAndEvaluatesInstVar, RectangleWidthHeightContainsAndIntersect (+11 more)

### Community 117 - "string"
Cohesion: 0.28
Nodes (16): byteText(), categoryHeading(), classNameOf(), ClassRow, category, cls, name, collectClassGlobal() (+8 more)

### Community 119 - "SmallInteger.cpp"
Cohesion: 0.11
Nodes (54): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+46 more)

### Community 120 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 121 - "AppKit"
Cohesion: 0.21
Nodes (5): Ao, AppKit, CAo, PackageDescription, XCTest

### Community 122 - "Session.cpp"
Cohesion: 0.24
Nodes (14): Image, load, save, clearMethodSources(), HostOopHook, installEmptyCache(), installEmptyWorkspace(), releaseMethodSources() (+6 more)

### Community 123 - "WellKnown::InternTable"
Cohesion: 0.18
Nodes (12): deque, size_t, string, unordered_map, Entry, cls, name, WellKnown::ExtraTable (+4 more)

### Community 124 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (69): ファイルごとの変更, ファイルごとの変更, pthread, ActiveGuard, rootShared, saved, answerWithoutSend(), boolean() (+61 more)

### Community 126 - "VendorClassFile"
Cohesion: 0.22
Nodes (10): Task 4: LOAD_ORDER とホストパッチ, string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult (+2 more)

### Community 127 - "ImageFormat"
Cohesion: 0.14
Nodes (14): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle, kImageFillerBytes (+6 more)

### Community 128 - "codegen"
Cohesion: 0.18
Nodes (8): Codegen, CompileResult, error, image, ok, codegen(), compileMethod(), string_view

### Community 129 - "SourceSpan"
Cohesion: 0.25
Nodes (5): uint32_t, SourceSpan, end, start, string

### Community 130 - "貢献"
Cohesion: 0.33
Nodes (6): コミット, ビルド, ライセンス, 曲げない規則, 現行フェーズ, 貢献

### Community 131 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (36): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+28 more)

### Community 132 - "NameCollect"
Cohesion: 0.20
Nodes (10): string, WellKnown, NameCollect, failed, out, rejectCatalog, wk, NamedOop (+2 more)

### Community 133 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 134 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 135 - "TEST"
Cohesion: 0.11
Nodes (22): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+14 more)

### Community 136 - "指摘"
Cohesion: 0.12
Nodes (15): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる (+7 more)

### Community 138 - "Array.cpp"
Cohesion: 0.13
Nodes (27): Task 1: 形式ヘッダと即値ビット, natives, uint64_t, ImageFormat::decodeNonHeap(), ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_() (+19 more)

### Community 139 - "Globals.cpp"
Cohesion: 0.33
Nodes (8): adoptImageClass(), at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "Session.hpp"
Cohesion: 0.25
Nodes (5): runtime, ao executable, ao_runtime library, ao_runtime_tests, boot()

### Community 142 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 146 - "指摘"
Cohesion: 0.08
Nodes (24): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む, [Low] Symbol の `copy` が、intern されていない別の Symbol を作る (+16 more)

### Community 147 - "intern"
Cohesion: 0.40
Nodes (5): bytes(), string_view, WellKnown, intern(), WellKnown::internSpecialSelectors()

### Community 148 - "指摘"
Cohesion: 0.10
Nodes (28): CompileEnv, instVarNames, knownGlobals, undeclaredAreBindings, string, [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる, [High] 2^63 以上の整数リテラルが黙って 0 になる, [High] クラス変数を読み書きできない (+20 more)

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "横断テーマ"
Cohesion: 0.22
Nodes (9): 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる, 3. 言語意味論の欠落（コンパイラ）, 4. Browser / Accept でのデータ消失, 5. 資源の上限と、その先の振る舞い, Claude によるコードレビュー（2026-09-23, HEAD 1ccf79a, PHASE P9）, 推奨する着手順, 横断テーマ (+1 more)

### Community 152 - "ClassMethodCache"
Cohesion: 0.07
Nodes (43): [High] Save / Open Image の失敗がユーザーに見えない, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, 06 イメージ・セッション・C ABI・CLI, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] vendor file-in のメソッドコンパイル失敗を成功として返し、17 メソッドが黙って欠落する, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする (+35 more)

### Community 153 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 154 - "evalBody"
Cohesion: 0.60
Nodes (5): blankOut(), AoInspectFn, evalBody(), knownGlobals, sessionEval()

### Community 156 - "ListedMethod"
Cohesion: 0.40
Nodes (5): ListedMethod, method, native, selector, protocolsOf()

### Community 157 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.29
Nodes (7): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, ProcessorScheduler, Semaphore, send

### Community 158 - "KeptMethod"
Cohesion: 0.40
Nodes (5): KeptMethod, key, meta, protocol, source

### Community 159 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 160 - "Gc"
Cohesion: 0.07
Nodes (40): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, optional, Gc, clearWeakAfterNursery (+32 more)

### Community 161 - "NameBag"
Cohesion: 0.67
Nodes (3): NameBag, heap, names

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

### Community 173 - "ParseResult"
Cohesion: 0.22
Nodes (7): ParseResult, error, method, ok, deque, string_view, parseMethod()

### Community 175 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 179 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

## Knowledge Gaps
- **800 isolated node(s):** `.metaFlag`, `.title`, `.sourceText`, `.errorText`, `.paneAccessibilityLabels` (+795 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1648 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `NameCollect`, `image_save_load_test.cpp`, `TEST`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Globals.cpp`, `Heap.cpp`, `TEST`, `SPEC.md`, `RootedArray`, `Roots.cpp`, `intern`, `指摘`, `Roots`, `Geometry.cpp`, `指摘`, `ClassMethodCache`, `Boot`, `TEST`, `TEST`, `Session`, `ListedMethod`, `native_send_test.cpp`, `imageRegistryStubA`, `Gc`, `NativeMethod.cpp`, `TEST`, `WellKnown::checkSmallIntegerFastPath`, `ClassDef`, `string`, `vector`, `TEST`, `ImageSave.cpp`, `answerOne`, `CallContext`, `.nil`, `abortingNew`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `send`, `String.cpp`, `allocateRetry`, `DiskHeader`, `abortingSubclass`, `WellKnown.cpp`, `TEST`, `classRows`, `cstdint`, `Lookup.hpp`, `.false_`, `abi.cpp`, `TEST`, `TEST`, `Ao`, `Heap`, `Bootstrap.cpp`, `CompiledMethodNatives.cpp`, `TEST`, `native_method_test.cpp`, `ImageLoad.cpp`, `send2`, `string`, `SmallInteger.cpp`, `GarbageFirstBoot`, `WellKnown::InternTable`, `Interpreter.cpp`, `ImageFormat`?**
  _High betweenness centrality (0.390) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `SourceSpan`, `LargeInteger.cpp`, `NameCollect`, `image_save_load_test.cpp`, `VendorExtract.cpp`, `Oop`, `Literal`, `ao_main.cpp`, `Session.hpp`, `ChunkAction`, `指摘`, `Roots`, `Session`, `ListedMethod`, `Gc`, `NativeMethod.cpp`, `NameBag`, `string`, `ChunkParser.cpp`, `ImageSave.cpp`, `ParseResult`, `Ast`, `RealScope`, `Parser`, `.compileAssign`, `TEST`, `classRows`, `cstdint`, `Lookup.hpp`, `.false_`, `KeptClass`, `ImageLoad.cpp`, `string`, `.advance`, `Session.cpp`, `Interpreter.cpp`, `VendorClassFile`, `ImageFormat`?**
  _High betweenness centrality (0.139) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `WellKnown::checkSmallIntegerFastPath`, `Oop`, `WellKnown.cpp`, `Heap`, `cstdint`, `Roots`, `uint64_t`?**
  _High betweenness centrality (0.087) - this node is a cross-community bridge._
- **Are the 10 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 10 INFERRED edges - model-reasoned connections that need verification._
- **Are the 5 inferred relationships involving `Roots` (e.g. with `B2 `to:do:` bench` and `01 オブジェクトメモリ・GC・ブートストラップ`) actually correct?**
  _`Roots` has 5 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.title`, `.sourceText` to the rest of the system?**
  _800 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.012903225806451613 - nodes in this community are weakly interconnected._
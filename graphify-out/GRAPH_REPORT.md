# Graph Report - ao-smalltalk  (2026-09-23)

## Corpus Check
- 243 files · ~114,132 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 77 file(s) not represented in the graph (top: .st 62, (none) 13, .toml 1)

## Summary
- 3336 nodes · 8603 edges · 148 communities (133 shown, 15 thin omitted)
- Extraction: 87% EXTRACTED · 13% INFERRED · 0% AMBIGUOUS · INFERRED: 1122 edges (avg confidence: 0.87)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `3f195824`
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
- context
- SPEC.md
- 指摘
- send
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- SmallInteger.cpp
- Gc
- Send.cpp
- WorkspaceWindow
- Session.cpp
- KeptClass
- TEST
- Bytecode interpreter
- ao_main.cpp
- TEST
- TEST
- string
- ChunkAction
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
- TEST
- TEST
- heap
- TEST
- BlockContext.cpp
- .run
- TEST
- TEST
- Parser
- .nil
- ao_abi.h
- TEST
- TEST
- NativeMethod.cpp
- roots
- string
- 指摘
- ToolWindowTests
- TEST
- wellknown
- BrowserWindow
- DiskHeader
- TEST
- TranscriptWindow
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- Boot
- Codegen.cpp
- ImageHeader
- VendorClassFile
- runSmalltalkTests
- ao image save
- cstdint
- BrowserModel
- 仕様
- .false_
- TEST_F
- Ao
- Globals.cpp
- TEST
- allocateRetry
- P6b vendor file-in Implementation Plan
- build.sh
- test.sh
- Ao
- Character.cpp
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- Claude レビュー指摘の修正計画（docs/claude-review）
- TEST_F
- .isSmallInteger
- Memory maintenance guide
- string
- string_view
- DEFERRED.md
- .applicationWillFinishLaunching
- gc_stress_test.cpp
- BrowserModelTests
- ImageLoad.cpp
- .build
- 貢献
- .isTrue
- P4 — Kernel Native Implementation
- Oop
- imageRegistryStubA
- AppKit
- TEST
- abi.cpp
- NameCollect
- ObjectHeader
- ImageFormat
- TEST
- Claude レビュー指摘の修正計画（docs/claude-review）
- P4-03 Magnitude / SmallInteger / Character
- 指摘
- 指摘
- TEST
- 指摘
- package-app.sh
- Array.cpp
- P7 .aoimage Implementation Plan
- 01 オブジェクトメモリ・GC・ブートストラップ
- ao_runtime_boot
- Roots.cpp
- TEST
- 指摘
- claude-review/README.md
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- P9-02: Browser accept
- P9-04: v1 ゴールデン受け入れ
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- TEST

## God Nodes (most connected - your core abstractions)
1. `Oop` - 613 edges
2. `Heap` - 183 edges
3. `WellKnown` - 152 edges
4. `Roots` - 108 edges
5. `send()` - 79 edges
6. `Ast` - 67 edges
7. `Parser` - 55 edges
8. `Session` - 49 edges
9. `TEST()` - 49 edges
10. `BrowserWindow` - 47 edges

## Surprising Connections (you probably didn't know these)
- `[Low] Inspector ウィンドウが閉じても解放されず、増え続ける` --references--> `InspectorWindow`  [INFERRED]
  docs/claude-review/07-app-build.md → app/Ao/InspectorWindow.swift
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `リスク` --references--> `Heap`  [INFERRED]
  docs/plans/history/zippy-tickling-key/v001_20260923-152025.md → runtime/include/ao/Heap.hpp
- `リスク` --references--> `Heap`  [INFERRED]
  docs/superpowers/plans/2026-09-23-review-fixes.md → runtime/include/ao/Heap.hpp
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

## Communities (148 total, 15 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (145): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+137 more)

### Community 1 - "Interpreter.cpp"
Cohesion: 0.05
Nodes (65): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, ファイルごとの変更, ファイルごとの変更, safepoint, Interpreter, run, ActiveGuard, rootShared (+57 more)

### Community 2 - ".isHeap"
Cohesion: 0.11
Nodes (66): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, allocateRetry(), ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_() (+58 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.11
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "image_save_load_test.cpp"
Cohesion: 0.07
Nodes (36): ImageSave, ImageSaveLoad, KernelMethodsStayNative, NurseryGcAfterLoad, OnePlusTwoMatches, RejectsBadMagicAndSecondLoad, CallContext, path (+28 more)

### Community 5 - "Token"
Cohesion: 0.06
Nodes (56): ArrayAndByteArrayHeaders, AssignVariantsAndComment, cmath, uint32_t, Scanner, i_, lexBinary, lexCharacter (+48 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.18
Nodes (24): containsHostWord(), string_view, firstLineKey(), firstNonEmptyLine(), HostMethod, protocol, source, hostPatch() (+16 more)

### Community 7 - "Object.cpp"
Cohesion: 0.13
Nodes (48): [High] printOn: が新しい printString を使わない, ao_Object_basicAt_(), ao_Object_basicAt_put_(), ao_Object_basicSize(), ao_Object_class(), ao_Object_copy(), ao_Object_doesNotUnderstand_(), ao_Object_equals() (+40 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.12
Nodes (54): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, allocateRetry(), ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position() (+46 more)

### Community 9 - "Float.cpp"
Cohesion: 0.19
Nodes (30): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, allocateRetry(), ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply() (+22 more)

### Community 10 - "Emitter"
Cohesion: 0.15
Nodes (13): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, bindingsImage_ (+5 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (38): cassert, charconv, cstdlib, fitsOld, header, objectBytes, oldCapacity, align8() (+30 more)

### Community 12 - "TEST"
Cohesion: 0.06
Nodes (36): ByteObjectPayloadIsNotScannedAsOops, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, GcOld, GcRoots, GcWeak, HandleTableKeepsObject, ImmediateClassIsNotFollowed (+28 more)

### Community 13 - "Literal"
Cohesion: 0.06
Nodes (41): CompileEnv, instVarNames, knownGlobals, undeclaredAreTemps, workspaceTemps, CompileResult, error, image (+33 more)

### Community 14 - "putNative"
Cohesion: 0.10
Nodes (28): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+20 more)

### Community 15 - "context"
Cohesion: 0.08
Nodes (29): AddInternsByFunctionPointer, AtPutFindsInternedKey, Bootstrap, chrono, context, cstdio, GrowKeepsOuterOopAndEntries, KernelBench (+21 more)

### Community 16 - "SPEC.md"
Cohesion: 0.08
Nodes (33): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+25 more)

### Community 17 - "指摘"
Cohesion: 0.15
Nodes (13): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+5 more)

### Community 18 - "send"
Cohesion: 0.20
Nodes (27): ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan(), ao_Collection_filter_count(), ao_Collection_filter_fill(), ao_Collection_includes_() (+19 more)

### Community 19 - "Heap"
Cohesion: 0.06
Nodes (37): Heap, bytes, containsNurseryFrom, flipNursery, fromBump_, fromEnd_, fromStart_, nextHash_ (+29 more)

### Community 20 - "Roots"
Cohesion: 0.04
Nodes (41): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, CompiledMethod, gc, LayoutNativeCodeNil, StackWalker, uint32_t, uint8_t (+33 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (28): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, allocateRetry(), ao_Point_add(), ao_Point_equals(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+20 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (27): Native selector mangling, マングル表, 規則, allocateRetry(), ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_() (+19 more)

### Community 25 - "Gc"
Cohesion: 0.06
Nodes (63): BlockContextKeepsHomeAndCopied, ContextGc, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, Gc, clearWeakAfterNursery, clearWeakAfterOldMark, collectNursery (+55 more)

### Community 26 - "Send.cpp"
Cohesion: 0.16
Nodes (20): [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, InlineCache, cachedClass, cachedMethod, lookup(), allocateMessage(), cacheIndex(), ClassMethodCache::addRoots() (+12 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.11
Nodes (18): installErrorField(), sendToKeyWorkspace(), Bool, Int, NSTextField, NSTextView, NSWindow, Void (+10 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (97): Image, load, save, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition(), browserProtocolAt() (+89 more)

### Community 29 - "KeptClass"
Cohesion: 0.14
Nodes (14): KeptClass, category, classVars, deferred, hasDef, instVars, methods, pools (+6 more)

### Community 30 - "TEST"
Cohesion: 0.05
Nodes (45): ByteObjectPayload, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+37 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.15
Nodes (18): climits, Task 5: CLI, 結論, dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests (+10 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.12
Nodes (17): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+9 more)

### Community 35 - "string"
Cohesion: 0.12
Nodes (20): algorithm, ao_abi, Chunk, compile, compiler, string, vector, cstring (+12 more)

### Community 36 - "ChunkAction"
Cohesion: 0.06
Nodes (54): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+46 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.10
Nodes (21): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, MethodDictionary (+13 more)

### Community 38 - "CompileError"
Cohesion: 0.12
Nodes (39): CompileError, message, span, string, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する (+31 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (19): BlockWithArgs, Cascade, ParseResult, error, method, ok, deque, parseMethod() (+11 more)

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
Cohesion: 0.18
Nodes (25): appendRaw(), appendRecord(), collectExtra(), collectImageSlot(), byte, size_t, string_view, uint64_t (+17 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

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
Cohesion: 0.08
Nodes (25): CallContext, activeContext, cache, hostTempCount, hostTemps, inspectHook, interpretedBytecodes, nonlocalHome (+17 more)

### Community 49 - "TEST"
Cohesion: 0.17
Nodes (13): BootstrapInstallsObjectIdentityEquals, InheritsFromSuperclass, MissingSelectorIsNil, CallContext, NativeFn, uint32_t, WellKnown, install() (+5 more)

### Community 50 - "TEST"
Cohesion: 0.18
Nodes (9): CascadeAndBlock, Codegen, LiteralArrayPseudoObjectsAreNotSymbols, TEST(), EighteenDigitIntegerSurvivesAsInt, ErrorSpanOnDanglingBinary, ReturnOnePlusTwo, SuperSendAndGlobal (+1 more)

### Community 52 - "TEST"
Cohesion: 0.14
Nodes (14): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectionDo, DictionaryAtPut, DictionaryEqualsLookupAndCollectValues (+6 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.26
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 54 - ".run"
Cohesion: 0.13
Nodes (16): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), spanMessage() (+8 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 56 - "TEST"
Cohesion: 0.17
Nodes (12): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+4 more)

### Community 57 - "Parser"
Cohesion: 0.13
Nodes (19): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+11 more)

### Community 58 - ".nil"
Cohesion: 0.26
Nodes (27): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+19 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.15
Nodes (13): EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RewritesHostSelectorAndDefersMissingSuper, string, string_view (+5 more)

### Community 61 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, ReadStreamNextPositionResetContents, send2(), TEST(), ShowInvokesHook (+5 more)

### Community 62 - "NativeMethod.cpp"
Cohesion: 0.27
Nodes (12): [Medium] nil をキーや要素にすると tally だけが増え、見えないエントリが残る, [Medium] `shallowCopy`・`printString`・`asSymbol` が、nursery が足りないときに GC を再試行せず失敗する, [Medium] String の添字アクセスが O(i) なので、`do:`・`contents`・`nextPutAll:` が O(n²) になる, add(), addNamed(), NativeFn, string_view, uint32_t (+4 more)

### Community 64 - "string"
Cohesion: 0.50
Nodes (12): allowIndex(), size_t, string, vector, dropCycles(), dropMissingSupers(), extractVendor(), findActive() (+4 more)

### Community 65 - "指摘"
Cohesion: 0.13
Nodes (15): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む, [Low] Symbol の `copy` が、intern されていない別の Symbol を作る (+7 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.30
Nodes (6): SmokeTests, NSMenu, NSMenuItem, String, ToolWindowTests, XCTestCase

### Community 67 - "TEST"
Cohesion: 0.22
Nodes (9): BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST(), LineEndBangAfterBinaryStillTerminates, SplitsMethodsForAndProtocol (+1 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.09
Nodes (25): Any, BrowserWindow, .errorText, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser(), Bool (+17 more)

### Community 70 - "DiskHeader"
Cohesion: 0.12
Nodes (17): uint16_t, uint32_t, DiskHeader, endian, extraCount, globalCount, headerBytes, heapBytes (+9 more)

### Community 71 - "TEST"
Cohesion: 0.29
Nodes (7): CharacterRoundTrip, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "TranscriptWindow"
Cohesion: 0.13
Nodes (16): aoTranscriptHook(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView, NSWindow (+8 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (33): extra 節, string_view, isVendorStub(), isCatalogName(), deque, size_t, string, string_view (+25 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.18
Nodes (11): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, ExtraListsCmUser, ImageRegistry, RememberSymbolRegistersWithoutAllocating (+3 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "Boot"
Cohesion: 0.05
Nodes (39): CascadeReturnsReceiver, CompilerRoundtrip, Geometry, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, largeinteger, NativePlusDoesNotInterpret (+31 more)

### Community 78 - "Codegen.cpp"
Cohesion: 0.09
Nodes (26): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), LitKind, MethodImage (+18 more)

### Community 79 - "ImageHeader"
Cohesion: 0.11
Nodes (20): bit, uint16_t, uint32_t, ImageHeader, endian, extraCount, globalCount, heapBytes (+12 more)

### Community 80 - "VendorClassFile"
Cohesion: 0.20
Nodes (11): Task 4: LOAD_ORDER とホストパッチ, string, vector, VendorClassFile, chunkText, className, superName, unsupportedShape (+3 more)

### Community 81 - "runSmalltalkTests"
Cohesion: 0.22
Nodes (11): ao_AoTest_assert_equals_(), CallContext, path, string, string_view, uint32_t, vector, listTests() (+3 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "cstdint"
Cohesion: 0.07
Nodes (14): string_view, cstdint, Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary (+6 more)

### Community 84 - "BrowserModel"
Cohesion: 0.21
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "仕様"
Cohesion: 0.22
Nodes (9): CLI, globals 節, load, OOP エンコード, save, Task 4: load と受け入れ評価, well-known 節, 仕様 (+1 more)

### Community 86 - ".false_"
Cohesion: 0.19
Nodes (27): allocateRetry(), ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers() (+19 more)

### Community 87 - "TEST_F"
Cohesion: 0.13
Nodes (17): [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Browser の読み取り, Task 3: Browser 読み取り ABI, Task 4: Swift から ABI をリンクする, ObjectIsKernelAndPrintStringIsNative, ao_browser_class_count(), ao_browser_class_definition(), ao_browser_protocol_at() (+9 more)

### Community 88 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 89 - "Globals.cpp"
Cohesion: 0.36
Nodes (7): at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 90 - "TEST"
Cohesion: 0.18
Nodes (11): AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, BlockEval, CopyDoesNotWriteOuter, ExampleFilePasses, IfTrueIfFalseFromCompiledMethod, NonLocalReturnSkipsRest (+3 more)

### Community 91 - "allocateRetry"
Cohesion: 0.43
Nodes (7): allocateRetry(), CallContext, uint16_t, uint32_t, uint8_t, createBlock(), createMethod()

### Community 92 - "P6b vendor file-in Implementation Plan"
Cohesion: 0.29
Nodes (6): P6b vendor file-in Implementation Plan, Self-review, 仕様, 前提, 範囲, 結論

### Community 95 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, Build and test, Documentation, Layout, License, Requirements, Status

### Community 96 - "Character.cpp"
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
Nodes (22): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+14 more)

### Community 105 - "TEST_F"
Cohesion: 0.08
Nodes (35): BlockAssignmentDoesNotUpdateWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, [High] Save / Open Image の失敗がユーザーに見えない, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, [High] 生存データが old 容量を超えるセッションを保存でき、そのイメージはロードできない, [Low] `ao --test` が失敗理由を出さず、空ディレクトリを合格にする (+27 more)

### Community 106 - ".isSmallInteger"
Cohesion: 0.37
Nodes (13): CompiledMethod accessors (P5), int64_t, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive() (+5 more)

### Community 111 - ".applicationWillFinishLaunching"
Cohesion: 0.14
Nodes (12): AoApp, openImageFile(), saveImageFile(), Int32, Notification, NSMenuItem, LaunchSet, Global Constraints (+4 more)

### Community 112 - "gc_stress_test.cpp"
Cohesion: 0.40
Nodes (4): optional, string, ScopedGcStressEnv, saved_

### Community 113 - "BrowserModelTests"
Cohesion: 0.20
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, StaticString (+1 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.17
Nodes (35): acceptWord(), bindAll(), checkGlobals(), byte, size_t, string, string_view, T (+27 more)

### Community 115 - ".build"
Cohesion: 0.26
Nodes (9): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, NSObject (+1 more)

### Community 116 - "貢献"
Cohesion: 0.33
Nodes (6): コミット, ビルド, ライセンス, 曲げない規則, 現行フェーズ, 貢献

### Community 117 - ".isTrue"
Cohesion: 0.53
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 118 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 119 - "Oop"
Cohesion: 0.11
Nodes (44): Task 8: printString, bits(), int64_t, isBytes(), isIndexable(), isPointers(), make(), uint64_t (+36 more)

### Community 120 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 121 - "AppKit"
Cohesion: 0.18
Nodes (5): Ao, AppKit, CAo, PackageDescription, XCTest

### Community 122 - "TEST"
Cohesion: 0.67
Nodes (3): LoadOrderEvaluatesLinkRoundTrip, TEST(), VendorFileIn

### Community 124 - "abi.cpp"
Cohesion: 0.09
Nodes (38): AcceptAbi, AoTranscriptFn, ClassDefinitionThenImageDropsSourceText, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 02 インタプリタ・送信・コンテキスト・プロセス, テストの穴, 概要, 03 数値・スカラー系 Kernel ネイティブ (+30 more)

### Community 125 - "NameCollect"
Cohesion: 0.20
Nodes (10): string, WellKnown, NameCollect, failed, out, rejectCatalog, wk, NamedOop (+2 more)

### Community 126 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 127 - "ImageFormat"
Cohesion: 0.12
Nodes (16): cstddef, HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, ImageFormat, decodeNonHeap, encodeNonHeap, kImageEndianLittle (+8 more)

### Community 129 - "TEST"
Cohesion: 0.40
Nodes (5): KeepsNativeIdentityEquals, RebindsBagAndEvaluatesInstVar, RefusesNewSelectorOnObject, TEST(), VendorOverwrite

### Community 130 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.09
Nodes (22): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+14 more)

### Community 131 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 133 - "指摘"
Cohesion: 0.16
Nodes (14): [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる, [High] 2^63 以上の整数リテラルが黙って 0 になる, [Low] カスケードの扱いが不完全, [Low] 宣言の検証がない（引数への代入・擬変数名・重複名）, [Low] 指数表記の意味が Smalltalk-80 と異なる, [Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる, [Medium] 二項演算子の直後の負数リテラルが、別のセレクタとして字句化される, [Medium] 文字列の中の `!!` が `!` に戻らない (+6 more)

### Community 134 - "指摘"
Cohesion: 0.18
Nodes (11): [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い, [Low] Inspector ウィンドウが閉じても解放されず、増え続ける, [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる, [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される, [Low] Workspace の評価を中断できない（無限ループでアプリが固まる）, [Medium] 64 KiB を超える Print it / Inspect it は副作用だけ残して失敗する (+3 more)

### Community 135 - "TEST"
Cohesion: 0.11
Nodes (23): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+15 more)

### Community 136 - "指摘"
Cohesion: 0.11
Nodes (18): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる (+10 more)

### Community 138 - "Array.cpp"
Cohesion: 0.32
Nodes (15): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+7 more)

### Community 140 - "P7 .aoimage Implementation Plan"
Cohesion: 0.17
Nodes (11): Global Constraints, P7 .aoimage Implementation Plan, Task 1: 形式ヘッダと即値ビット, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 6: フェーズを閉じる, リスク, 仕様との対応 (+3 more)

### Community 141 - "01 オブジェクトメモリ・GC・ブートストラップ"
Cohesion: 0.20
Nodes (9): 01 オブジェクトメモリ・GC・ブートストラップ, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, テストの穴, 指摘 (+1 more)

### Community 143 - "ao_runtime_boot"
Cohesion: 0.08
Nodes (30): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, [Low] transcript フックが boot 前、または shutdown→boot 後に配線されない, B6 イメージとセッションの堅牢化, C ABI, P8–P9 Remaining Implementation Plan, printString (+22 more)

### Community 144 - "Roots.cpp"
Cohesion: 0.20
Nodes (11): walker_, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::handleAt(), Roots::pushHandle(), Roots::remove() (+3 more)

### Community 148 - "TEST"
Cohesion: 0.25
Nodes (8): BlockCaptureIsMethodTemp, TEST(), workspaceEnv(), KnownGlobalAssignIsError, KnownGlobalReadIsPushGlobal, PreseededDeclaredAndCapturedKeepOrder, UndeclaredAssignIsWorkspaceTemp, WorkspaceTemp

### Community 149 - "指摘"
Cohesion: 0.20
Nodes (10): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+2 more)

### Community 150 - "claude-review/README.md"
Cohesion: 0.11
Nodes (15): 06 イメージ・セッション・C ABI・CLI, テストの穴, 概要, 07 Swift アプリ・ビルド・スクリプト, テストの穴, 概要, 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる (+7 more)

### Community 152 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, インライン展開, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 手順（各段を緑にしてから次へ進む）

### Community 153 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 154 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 155 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, インライン展開, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 手順（各段を緑にしてから次へ進む）

### Community 161 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

## Knowledge Gaps
- **743 isolated node(s):** `.metaFlag`, `.title`, `.sourceText`, `.errorText`, `.paneAccessibilityLabels` (+738 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1317 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **15 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `Interpreter.cpp`, `.isHeap`, `LargeInteger.cpp`, `image_save_load_test.cpp`, `指摘`, `Object.cpp`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Heap.cpp`, `P7 .aoimage Implementation Plan`, `TEST`, `putNative`, `context`, `Roots.cpp`, `send`, `Heap`, `Roots`, `Geometry.cpp`, `SmallInteger.cpp`, `Gc`, `Send.cpp`, `Session.cpp`, `TEST`, `TEST`, `string`, `ChunkAction`, `P3 — Native Dispatch`, `CompileError`, `ImageSave.cpp`, `CallContext`, `TEST`, `BlockContext.cpp`, `TEST`, `.nil`, `TEST`, `NativeMethod.cpp`, `WellKnown.cpp`, `Boot`, `ImageHeader`, `runSmalltalkTests`, `cstdint`, `.false_`, `Ao`, `Globals.cpp`, `allocateRetry`, `Ao`, `Character.cpp`, `Bootstrap.cpp`, `.isSmallInteger`, `gc_stress_test.cpp`, `ImageLoad.cpp`, `.isTrue`, `imageRegistryStubA`, `abi.cpp`, `NameCollect`, `ObjectHeader`, `ImageFormat`?**
  _High betweenness centrality (0.365) - this node is a cross-community bridge._
- **Why does `Heap` connect `Heap` to `WellKnown`, `Claude レビュー指摘の修正計画（docs/claude-review）`, `.isHeap`, `LargeInteger.cpp`, `image_save_load_test.cpp`, `TEST`, `Object.cpp`, `Float.cpp`, `Array.cpp`, `Heap.cpp`, `P7 .aoimage Implementation Plan`, `01 オブジェクトメモリ・GC・ブートストラップ`, `TEST`, `ao_runtime_boot`, `putNative`, `Stream.cpp`, `send`, `context`, `Roots`, `TEST`, `P1 — Object Memory`, `Gc`, `Send.cpp`, `Session.cpp`, `TEST`, `ao_main.cpp`, `string`, `P3 — Native Dispatch`, `ImageSave.cpp`, `CallContext`, `BlockContext.cpp`, `.nil`, `NativeMethod.cpp`, `WellKnown.cpp`, `TEST`, `Boot`, `ImageHeader`, `cstdint`, `仕様`, `Globals.cpp`, `Bootstrap.cpp`, `Claude レビュー指摘の修正計画（docs/claude-review）`, `TEST_F`, `gc_stress_test.cpp`, `ImageLoad.cpp`, `ObjectHeader`?**
  _High betweenness centrality (0.130) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Heap`, `cstdint`, `Roots`, `Oop`?**
  _High betweenness centrality (0.119) - this node is a cross-community bridge._
- **Are the 5 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 5 INFERRED edges - model-reasoned connections that need verification._
- **Are the 14 inferred relationships involving `Heap` (e.g. with `01 オブジェクトメモリ・GC・ブートストラップ` and `[Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している`) actually correct?**
  _`Heap` has 14 INFERRED edges - model-reasoned connections that need verification._
- **Are the 4 inferred relationships involving `Roots` (e.g. with `01 オブジェクトメモリ・GC・ブートストラップ` and `[Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない`) actually correct?**
  _`Roots` has 4 INFERRED edges - model-reasoned connections that need verification._
- **Are the 71 inferred relationships involving `send()` (e.g. with `GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答` and `[Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない`) actually correct?**
  _`send()` has 71 INFERRED edges - model-reasoned connections that need verification._
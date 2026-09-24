# Graph Report - ao-smalltalk  (2026-09-25)

## Corpus Check
- 254 files · ~207,106 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 79 file(s) not represented in the graph (top: .st 64, (none) 13, .toml 1)

## Summary
- 4503 nodes · 12201 edges · 184 communities (165 shown, 19 thin omitted)
- Extraction: 86% EXTRACTED · 14% INFERRED · 0% AMBIGUOUS · INFERRED: 1691 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `882816ac`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- Codegen.cpp
- .isHeap
- LargeInteger.cpp
- TEST
- Scanner.cpp
- VendorExtract.cpp
- Oop
- Stream.cpp
- ao_eval
- Emitter
- Heap.cpp
- TEST
- Literal
- Float.cpp
- send
- applyChunks
- 指摘
- vector
- Heap
- Roots
- Geometry.cpp
- TEST
- P1 — Object Memory
- Boot
- uint64_t
- TEST
- WorkspaceWindow
- kernel_numeric_test.cpp
- TranscriptWindow
- P3 — Native Dispatch
- Bytecode interpreter
- ao_main.cpp
- TEST
- TEST
- image_save_load_test.cpp
- ChunkParser.cpp
- string
- TEST_F
- TEST
- BrowserWindow
- P2-02 class object skeletons
- P6b — vendor file-in
- ImageSave.cpp
- P9-01: Do it / Print it / Inspect it
- TEST
- Ast
- Loaded
- CallContext
- TEST
- .nil
- heap
- TEST
- BlockContext.cpp
- TEST_F
- TEST
- SmallInteger.cpp
- Parser
- TEST
- ao_abi.h
- TEST
- BrowserModelTests
- String.cpp
- roots
- CallContext
- allocateRetry
- ToolWindowTests
- TEST
- wellknown
- .init
- DiskHeader
- TEST
- installMethod
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
- Character.cpp
- BrowserModel
- TEST_F
- Behavior.cpp
- TEST
- TEST
- abi.cpp
- TEST
- Bootstrap.cpp
- Vendor.hpp
- build.sh
- test.sh
- SPEC.md
- Session.cpp
- .isTrue
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- docs/README.md
- Claude レビュー指摘の修正計画（docs/claude-review）
- 指摘
- ao_runtime_boot
- Memory maintenance guide
- string
- string_view
- Deferred
- AoApp
- native_method_test.cpp
- .publish
- ImageLoad.cpp
- .build
- TEST
- AppKit
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- NativeMethod.cpp
- 指摘
- P3-01 Symbol intern
- ClassPool.cpp
- Reentry
- ImageSurgery
- Interpreter.cpp
- P1-06: weak slot GC
- ImageFormat
- OperandStack
- TEST
- Array.cpp
- ChunkMethod
- P9-02: Browser accept
- TEST
- P9-04: v1 ゴールデン受け入れ
- TEST
- Ao
- package-app.sh
- ObjectHeader
- path
- specialIndex
- GarbageFirstBoot
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- TEST
- Roots.cpp
- codegen
- .fromSmallInteger
- intern
- P4-04 Array / ByteArray / String / Symbol
- TEST
- TEST
- uint64_t
- RootedArray
- BlockProbe
- P4 — Kernel Native Implementation
- interpreter
- abortingSubclass
- send2
- uint32_t
- WellKnown::checkSmallIntegerFastPath
- Gc
- P4-06 Stream / Transcript model
- README.md
- P4-07 Process / ProcessorScheduler / Semaphore
- P4-05 Dictionary / Set / OrderedCollection
- FileSizeLimit
- imageRegistryStubA
- TEST
- cli_test.sh
- Compile.cpp
- Frame
- TestDir
- answerOne
- Image::load
- abortingNew
- native_send_test.cpp
- assemble
- B10 協調スケジューラ（02 High）
- ImageSelector
- performSend
- Format.hpp
- TEST
- ActiveGuard
- DepthGuard

## God Nodes (most connected - your core abstractions)
1. `Oop` - 790 edges
2. `Heap` - 232 edges
3. `WellKnown` - 167 edges
4. `vector` - 151 edges
5. `Ast` - 103 edges
6. `Roots` - 97 edges
7. `TEST()` - 94 edges
8. `TEST()` - 93 edges
9. `TEST()` - 90 edges
10. `BrowserWindow` - 86 edges

## Surprising Connections (you probably didn't know these)
- `[Low] Inspector ウィンドウが閉じても解放されず、増え続ける` --references--> `InspectorWindow`  [INFERRED]
  docs/claude-review/07-app-build.md → app/Ao/InspectorWindow.swift
- `[Low] `AoSpan` の区間を捨てているので、エラー位置が分からない` --references--> `AoSpan`  [INFERRED]
  docs/claude-review/07-app-build.md → bridge/ao_abi.h
- `[High] チャンク分割が `$'` と `$"` を文字列・コメントの開始と誤認し、ファイルの残りを飲み込む` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる` --references--> `parseChunks()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/ChunkParser.cpp
- `[High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない` --references--> `isBinaryChar()`  [INFERRED]
  docs/claude-review/05-compiler.md → compiler/src/Scanner.cpp

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

## Communities (184 total, 19 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (148): array, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+140 more)

### Community 1 - "Codegen.cpp"
Cohesion: 0.05
Nodes (49): Analysis, declared, error, failed, lexes, localOf, outerRefs, realOf (+41 more)

### Community 2 - ".isHeap"
Cohesion: 0.12
Nodes (59): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ネイティブのルートを構造で保証する, ネイティブのルートを構造で保証する, ao_Association_key(), ao_Association_key_value_(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_() (+51 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.08
Nodes (82): Digits, [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する (+74 more)

### Community 4 - "TEST"
Cohesion: 0.04
Nodes (46): AnonymousBehaviorInstanceSavesAndLoads, EscapedCollectionThunksRunAfterSaveAndLoad, EscapedStreamThunkSurvivesSaveAndLoad, FailedProbeKeepsCurrentSession, FailedWriteKeepsOldImage, FileSizeLimitFailsWithoutTheSignal, HeapBeyondOldLimitFailsAndKeepsOldImage, ImageLoadChecks (+38 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.07
Nodes (58): ArrayAndByteArrayHeaders, AssignVariantsAndComment, CommaIsABinaryCharacter, uint32_t, Scanner, i_, lexBinary, lexCharacter (+50 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.06
Nodes (67): extra 節, mman, allowIndex(), containsHostWord(), size_t, string, string_view, doubleBangs() (+59 more)

### Community 7 - "Oop"
Cohesion: 0.11
Nodes (60): [High] printOn: が新しい printString を使わない, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, boolean() (+52 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.06
Nodes (83): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean() (+75 more)

### Community 9 - "ao_eval"
Cohesion: 0.08
Nodes (50): AoSpan, end, message, start, [High] Save / Open Image の失敗がユーザーに見えない, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 02 インタプリタ・送信・コンテキスト・プロセス, 概要 (+42 more)

### Community 10 - "Emitter"
Cohesion: 0.13
Nodes (15): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, real_ (+7 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (43): charconv, allocateTenured, fitsOld, growOld, initObject, objectBytes, oldUsed, align8() (+35 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+40 more)

### Community 13 - "Literal"
Cohesion: 0.10
Nodes (22): int64_t, LitKind, string, uint16_t, uint8_t, unique_ptr, Literal, bytes (+14 more)

### Community 14 - "Float.cpp"
Cohesion: 0.13
Nodes (48): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, [Medium] Fraction の `=` と `<` が無く、等しい分数どうしが等しくならない, NumberOp, NumberRelation, NumKind, int64_t, valueHashFold(), ao_Float_add() (+40 more)

### Community 15 - "send"
Cohesion: 0.08
Nodes (70): InlineCache, cachedClass, cachedMethod, ao_ArrayedCollection_do_(), ao_Collection_collect_(), ao_Collection_collect_fill(), ao_Collection_detect_ifNone_(), ao_Collection_detect_scan() (+62 more)

### Community 16 - "applyChunks"
Cohesion: 0.12
Nodes (20): [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, Global Constraints, P6b vendor file-in Implementation Plan, Self-review, Task 1: changes 切り出し, Task 2: ピンと vendor ソース, Task 3: rebind と上書き禁止, Task 4: LOAD_ORDER とホストパッチ (+12 more)

### Community 17 - "指摘"
Cohesion: 0.05
Nodes (32): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる, [High] `Smalltalk` が knownGlobals に含まれない (+24 more)

### Community 18 - "vector"
Cohesion: 0.10
Nodes (40): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+32 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (47): [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, Heap, containsNurseryFrom, containsNurseryTo, flipNursery, fromBump_, fromEnd_, fromStart_ (+39 more)

### Community 20 - "Roots"
Cohesion: 0.04
Nodes (51): nursery GC, P1-04 old-generation mark-compact, old-generation promotion, Gc, P1-05 GC root API, Roots, Roots::add(Oop*), Root (+43 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.24
Nodes (29): [Low] Point と Rectangle のネイティブがサブクラスを扱えない, ao_Point_add(), ao_Point_equals(), ao_Point_hash(), ao_Point_intDivide(), ao_Point_multiply(), ao_Point_subtract(), ao_Point_x() (+21 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (27): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroAbortsWithReason, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+19 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.15
Nodes (18): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+10 more)

### Community 24 - "Boot"
Cohesion: 0.05
Nodes (41): HandAssembledLitVarRoundTrip, HandAssembledRemoteTempRoundTrip, JumpOnNonBooleanAborts, JumpOnNonBooleanUsesMustBeBooleanAnswer, OpcodePastLastOpFails, PushNewArrayIsNilFilledArray, string, evalExpr() (+33 more)

### Community 25 - "uint64_t"
Cohesion: 0.30
Nodes (12): uint64_t, unordered_map, headerAt(), heapShaped(), bits, ObjectRules, behavior_, dict_ (+4 more)

### Community 26 - "TEST"
Cohesion: 0.04
Nodes (58): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenParent, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery (+50 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.07
Nodes (35): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), failureText(), installErrorField() (+27 more)

### Community 28 - "kernel_numeric_test.cpp"
Cohesion: 0.20
Nodes (12): chrono, cmath, FractionToFloatRoundsOnceIncludingSubnormals, IntegerToFloatRoundsHalfToEven, KernelNumericConvert, limits, RightShiftOfAMillionBitsIsLinear, KernelBench (+4 more)

### Community 29 - "TranscriptWindow"
Cohesion: 0.13
Nodes (17): aoTranscriptHook(), configureSourceEditing(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView (+9 more)

### Community 30 - "P3 — Native Dispatch"
Cohesion: 0.11
Nodes (21): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, NativeMethod, selector mangle ao_<Class>_<selectorMangled>, NativeMethod symbol rebind on load, MethodDictionary (+13 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "ao_main.cpp"
Cohesion: 0.15
Nodes (19): climits, dyld, runtime, ao executable, ao_runtime library, ao_runtime_tests, addRoots, bootAndRunTests() (+11 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.08
Nodes (27): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+19 more)

### Community 35 - "image_save_load_test.cpp"
Cohesion: 0.11
Nodes (35): algorithm, ao_abi, Bootstrap, Chunk, compile, CompiledMethod, compiler, context (+27 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.17
Nodes (24): classify(), string, string_view, Token, uint32_t, firstLineHas(), isBlank(), isCharacterLiteral() (+16 more)

### Community 37 - "string"
Cohesion: 0.09
Nodes (14): cassert, string, string_view, cstddef, cstdint, memory, NativeMethod, CallContext (+6 more)

### Community 38 - "TEST_F"
Cohesion: 0.05
Nodes (40): AoTranscriptFn, ArrayEqualsChecksIdentityFirstAndSameClass, BlockAssignmentUpdatesWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, CallFromAnotherThreadWhileEvaluatingIsRefused, ClassDefinedAfterBindingWins, ClassSideConstructorsAllocateTheSubclassInstSize, DeadHomeBlockAbortsWithReason (+32 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (21): BlockWithArgs, Cascade, BlockArgumentsThenTemps, CascadePartsAreMessageChains, CommaIsABinarySelector, DeclarationsAreCheckedPerScope, LiteralArrayPseudoObjectsAreNotSymbols, string (+13 more)

### Community 40 - "BrowserWindow"
Cohesion: 0.11
Nodes (22): BrowserWindow, .acceptsMethod, .errorText, .hasUnacceptedChanges, .paneAccessibilityLabels, .sourceText, .title, Int32 (+14 more)

### Community 41 - "P2-02 class object skeletons"
Cohesion: 0.09
Nodes (25): P2 — Bootstrap, Blue Book 6–10, Metaclass cycle, Smalltalk global dictionary, Heap, Oop, P2-01 WellKnown and immediates, Roots (+17 more)

### Community 42 - "P6b — vendor file-in"
Cohesion: 0.16
Nodes (14): CompiledMethod, P5 — Compiler, AST via lexer/parser, bytecode code generation, chunk file-in parser, P6 — Interpreter, ao --test runner, bytecode interpreter loop (+6 more)

### Community 43 - "ImageSave.cpp"
Cohesion: 0.08
Nodes (48): cerrno, csignal, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる, fcntl, encodeNonHeap, writeFiller, writeHeader, appendRaw() (+40 more)

### Community 44 - "P9-01: Do it / Print it / Inspect it"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-01: Do it / Print it / Inspect it, コミット文, テスト, 完了条件, 手順, 目的

### Community 45 - "TEST"
Cohesion: 0.12
Nodes (16): ArrayString, AsSymbolAndAsString, AtPutAndSize, AtPutGrowsUtf8WithinObjectBytes, AtPutShrinksUtf8WithinObjectBytes, BasicAtOnArray, ByteArrayAtPutSmallInteger, FromSlotsAndDo (+8 more)

### Community 46 - "Ast"
Cohesion: 0.09
Nodes (29): Ast, argc, floatValue, intValue, isFloat, isSuper, kids, kind (+21 more)

### Community 47 - "Loaded"
Cohesion: 0.13
Nodes (16): CallContext, WellKnown, expectOnePlusTwo(), expectSpecialSelectorsInterned(), expectSpecialSends(), Loaded, cache, ctx (+8 more)

### Community 48 - "CallContext"
Cohesion: 0.06
Nodes (37): BindingHook, CallContext, aborting, abortReason, abortReasonHandle, activeContext, bindingHook, cache (+29 more)

### Community 49 - "TEST"
Cohesion: 0.09
Nodes (23): ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription, CycleMethodDictIsMethodDictionary (+15 more)

### Community 50 - ".nil"
Cohesion: 0.24
Nodes (28): ao_BlockContext_fork(), ao_MethodContext_method(), ao_MethodContext_receiver(), ao_MethodContext_sender(), ao_Process_priority_(), ao_Process_resume(), ao_Process_suspend(), ao_ProcessorScheduler_activeProcess() (+20 more)

### Community 52 - "TEST"
Cohesion: 0.08
Nodes (26): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+18 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.25
Nodes (21): ao_BlockContext_cannotReturn_(), ao_BlockContext_ensure_(), ao_BlockContext_ifCurtailed_(), ao_BlockContext_numArgs(), ao_BlockContext_repeat(), ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_value_value_value_() (+13 more)

### Community 54 - "TEST_F"
Cohesion: 0.08
Nodes (23): CleanupFailureKeepsFirstReason, CleanupReturnDoesNotSwallowAbort, CleanupUnwindingWinsOverPausedReturn, CollectStopsOnFailedBlock, DoesNotUnderstandNamesSelector, DynamicReasonSurvivesCleanupAllocations, EnsureRunsDuringAbortAndSessionContinues, ErrorReasonIsStringArgument (+15 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewAbortsWithShouldNotImplement, MetaclassNewIsForbidden (+5 more)

### Community 56 - "SmallInteger.cpp"
Cohesion: 0.23
Nodes (29): safepoint, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_(), ao_Integer_equals(), ao_Integer_greaterOrEqual() (+21 more)

### Community 57 - "Parser"
Cohesion: 0.10
Nodes (23): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+15 more)

### Community 58 - "TEST"
Cohesion: 0.16
Nodes (14): DnuWithFullNurseryPassesMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandPassesMessage, IdentityEqualsAndClass, NativeSend, setGcStress, int64_t, WellKnown (+6 more)

### Community 59 - "ao_abi.h"
Cohesion: 0.23
Nodes (11): ao_runtime_boot, ao_runtime_shutdown, ao_version, Root CMakeLists.txt, compiler CMakeLists, ao::compiler::version, version() implementation, CompilerSmoke.VersionIsNonEmpty (+3 more)

### Community 60 - "TEST"
Cohesion: 0.13
Nodes (15): CommittedFilesRoundTrip, EachExtractedFileHasOneClassDef, LastDefinitionWinsAndDropsDoIt, LinkSelectorsAreSeparateMethods, PatchesHostWordInMethodBody, RecordsUrlCommitAndLicense, RenderDoublesBangs, RewritesHostSelectorAndDefersMissingSuper (+7 more)

### Community 61 - "BrowserModelTests"
Cohesion: 0.22
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, UInt (+1 more)

### Community 62 - "String.cpp"
Cohesion: 0.17
Nodes (28): size_t, uint64_t, valueHashBytes(), ao_String_asSymbol(), ao_String_at_(), ao_String_at_put_(), ao_String_equals(), ao_String_hash() (+20 more)

### Community 64 - "CallContext"
Cohesion: 0.13
Nodes (24): [High] 2^63 以上の整数リテラルが黙って 0 になる, isClassShaped(), superclassOf(), anyMethodIn(), boxBytes(), boxedOk(), boxLiteral(), boxMethodImage() (+16 more)

### Community 65 - "allocateRetry"
Cohesion: 0.04
Nodes (71): [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, 指摘 (+63 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.23
Nodes (7): NSMenu, NSMenuItem, NSTextView, NSView, String, T, ToolWindowTests

### Community 67 - "TEST"
Cohesion: 0.11
Nodes (19): ActionsCarryTheirChunkSpan, BangInCharacterDoesNotSplit, BangSpaceBangEndsSectionButDoubleBangStaysLiteral, ClassDefinitionShape, ClassificationNeedsTheMessageShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, ChunksAfterSectionEndAreExpressions (+11 more)

### Community 69 - ".init"
Cohesion: 0.13
Nodes (12): Any, Int, NSRect, NSTableView, NSTextField, NSTextView, NSView, [Medium] Blue Book の基本制御・ブロックのプロトコルが未実装で、DNU の Message が AO_OK で返る (+4 more)

### Community 70 - "DiskHeader"
Cohesion: 0.08
Nodes (27): bit, byte, size_t, string, uint16_t, uint32_t, uint64_t, DiskHeader (+19 more)

### Community 71 - "TEST"
Cohesion: 0.11
Nodes (17): DeferredMethodErrorsAreNotCounted, DeferredMethodsReadsOnlyListingLines, FileInLoadOrder, LoadOrderEvaluatesLinkRoundTrip, MethodErrorsAreExactlyTheDeferredOnes, PartlyDeferredStillFails, path, string (+9 more)

### Community 72 - "installMethod"
Cohesion: 0.08
Nodes (33): CompileEnv, classVarNames, instVarNames, kernelInstVarCount, knownGlobals, undeclaredAreBindings, size_t, classVarEnv() (+25 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.09
Nodes (30): findSymbol, global, internWith, deque, size_t, string, string_view, unordered_map (+22 more)

### Community 74 - "Ao.app skeleton"
Cohesion: 0.43
Nodes (8): ao_abi boot/shutdown, Ao.app skeleton, Bundle ID jp.ao.smalltalk, TranscriptWindow, WorkspaceWindow, System Browser 5 panes, Do it / Print it keybindings, MainMenu and keybindings

### Community 75 - "TEST"
Cohesion: 0.17
Nodes (12): AddNamedKeepsAliasAndRejectsForeignIndex, AdoptOldBytesOnce, BindImageSlotWritesFieldsInOrder, BootFindsIdentityEqualsAndSharedStubNames, EachImageSlotLists127Names, KernelThunkFunctionsHaveNames, RememberSymbolRegistersWithoutAllocating, adoptOldBytes (+4 more)

### Community 76 - "P9-03: エラー表示と VoiceOver"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-03: エラー表示と VoiceOver, コミット文, テスト, 完了条件, 手順, 目的

### Community 77 - "TEST"
Cohesion: 0.13
Nodes (15): CascadeReturnsReceiver, CompilerRoundtrip, GlobalObject, HandWrittenJumpFalseSkipsPush, HolderInstVarRoundTrip, NativePlusDoesNotInterpret, NestedCompiledSendKeepsOuterContext, NativeFn (+7 more)

### Community 78 - "TEST"
Cohesion: 0.06
Nodes (48): ArgumentAssignIsError, BoxedTempUsesRemoteTemp, CascadeAndBlock, ClassVariable, ClassVariableHidesGlobalInsideBlocks, classVarLiterals(), countOp(), CascadePartsAreMessageChains (+40 more)

### Community 79 - "Token"
Cohesion: 0.18
Nodes (11): int64_t, string, Tok, Token, intValue, isFloat, kind, largeInt (+3 more)

### Community 80 - "Boolean.cpp"
Cohesion: 0.20
Nodes (30): Native selector mangling, マングル表, 規則, Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_() (+22 more)

### Community 81 - "TEST"
Cohesion: 0.16
Nodes (13): BlockAssignmentIsBindingStore, bindingLiterals(), DeclaredTempIgnoresBinding, MethodImage, string, TEST(), workspaceEnv(), KnownGlobalAssignIsError (+5 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "Character.cpp"
Cohesion: 0.53
Nodes (8): ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext, uint32_t

### Community 84 - "BrowserModel"
Cohesion: 0.19
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - "TEST_F"
Cohesion: 0.05
Nodes (42): AsCharacterAcceptsOnlyUnicodeScalarValues, BitShiftBeyondMinusTwoToThe24AnswersZeroOrMinusOne, BooleanOperatorsFollowTheBlueBook, DivisionFollowsTheSameTypeRules, ElementHashMayBeASmalltalkMethod, EqualArraysAndPointsHashEqually, EqualNumbersHashEqually, EqualStringsAndSymbolsHashEqually (+34 more)

### Community 86 - "Behavior.cpp"
Cohesion: 0.23
Nodes (25): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable(), ao_Behavior_methodDict() (+17 more)

### Community 87 - "TEST"
Cohesion: 0.03
Nodes (77): AcceptAbi, AcceptClassErrorSpanCountsUndoubledBangs, AcceptClassKeepsSubclassPatternAMethod, AcceptClassReadsDoubledBangCharacter, AcceptClassRefusesChunksAfterSectionEnd, AcceptClassRefusesClassAsItsOwnSuperclass, AcceptClassRefusesNonDefinitionWithoutApplying, AcceptClassRefusesStatementsAfterDefinition (+69 more)

### Community 88 - "TEST"
Cohesion: 0.07
Nodes (47): AllocateNoGcSpillsToOld, BootstrapInstallsObjectIdentityEquals, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, InheritsFromSuperclass, LargeObjectAllocatedInOld, MissingSelectorIsNil (+39 more)

### Community 89 - "abi.cpp"
Cohesion: 0.15
Nodes (25): atomic, Body, CountsAnswerMinusOneOnFailure, [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, AbiEntry, ao_browser_class_at() (+17 more)

### Community 90 - "TEST"
Cohesion: 0.03
Nodes (75): AbortingTestClassFailsAndClears, AndOrShortCircuit, AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, AssertEqualsStopsAfterAbortingEquals, BlockAbort, BlockActivation (+67 more)

### Community 91 - "Bootstrap.cpp"
Cohesion: 0.15
Nodes (23): allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize, name, WellKnown (+15 more)

### Community 92 - "Vendor.hpp"
Cohesion: 0.24
Nodes (9): string, VendorClassFile, chunkText, className, superName, unsupportedShape, VendorExtractResult, files (+1 more)

### Community 95 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 96 - "Session.cpp"
Cohesion: 0.06
Nodes (92): Image, check, load, save, blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition() (+84 more)

### Community 97 - ".isTrue"
Cohesion: 0.35
Nodes (11): uint64_t, ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), ao_Magnitude_max_(), ao_Magnitude_min_(), CallContext (+3 more)

### Community 103 - "docs/README.md"
Cohesion: 0.14
Nodes (23): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P7 — Image, .aoimage save/load, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow (+15 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.11
Nodes (17): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B6 イメージとセッションの堅牢化, B7 コンパイラの構文とチャンク形式, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review） (+9 more)

### Community 105 - "指摘"
Cohesion: 0.13
Nodes (14): 07 Swift アプリ・ビルド・スクリプト, [Low] `AoSpan` の区間を捨てているので、エラー位置が分からない, [Low] C++ のデプロイメントターゲットがアプリの最小 OS と一致していない, [Low] .gitignore に `.cache/` と `.serena/logs/` が無い, [Low] Inspector ウィンドウが閉じても解放されず、増え続ける, [Low] package-app.sh が作るバンドルは署名検証に通らない Debug ビルドになる, [Low] Print it の結果が NUL を含むと、そこで途切れて壊れた文字列が挿入される, [Low] Workspace の評価を中断できない（無限ループでアプリが固まる） (+6 more)

### Community 106 - "ao_runtime_boot"
Cohesion: 0.05
Nodes (47): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, CleanupFailureReleasesItsReasonHandle, DefaultDoesNotUnderstandAborts, B2 `to:do:` bench, P4 microbench, P6 interpreter bench (+39 more)

### Community 111 - "AoApp"
Cohesion: 0.10
Nodes (17): AoApp, openImageFile(), saveImageFile(), Bool, Int32, MainActor, Notification, NSMenuItem (+9 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - ".publish"
Cohesion: 0.10
Nodes (16): DefinitionScanner, sendToKeyBrowser(), Bool, MainActor, Notification, NSSegmentedControl, NSWindow, String (+8 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.15
Nodes (35): ifstream, acceptWord(), atOffset(), checkFile(), byte, size_t, string, T (+27 more)

### Community 115 - ".build"
Cohesion: 0.25
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "TEST"
Cohesion: 0.10
Nodes (20): ClassRedefinitionDropsOldClassEntries, FlushSelectorDropsEveryClassAndKeepsOtherSelectors, InlinedToDoMillion, InstallMissingAddsAbsentAndKeepsPresent, InstallMissingReachesCachedSend, KernelInstall, KernelScan, MethodCacheInvalidation (+12 more)

### Community 117 - "AppKit"
Cohesion: 0.16
Nodes (7): Ao, SmokeTests, AppKit, CAo, PackageDescription, XCTest, XCTestCase

### Community 118 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 119 - "NativeMethod.cpp"
Cohesion: 0.08
Nodes (37): CLI, Global Constraints, globals 節, load, OOP エンコード, P7 .aoimage Implementation Plan, save, Task 1: 形式ヘッダと即値ビット (+29 more)

### Community 120 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 121 - "P3-01 Symbol intern"
Cohesion: 0.22
Nodes (8): Heap, Oop, P3-01 Symbol intern, Symbol, Symbol::intern, MethodDictionary, P3-02 MethodDictionary, Symbol

### Community 122 - "ClassPool.cpp"
Cohesion: 0.23
Nodes (18): classpool, adopt(), bindingAt(), string, string_view, uint32_t, WellKnown, instSizeOf() (+10 more)

### Community 123 - "Reentry"
Cohesion: 0.22
Nodes (9): string, expectAllRefused(), Reentry, codes, entered, evalOut, image, loadOrder (+1 more)

### Community 124 - "ImageSurgery"
Cohesion: 0.27
Nodes (9): size_t, string_view, uint64_t, ImageSurgery, bytes, header, kHeap, oopWords() (+1 more)

### Community 125 - "Interpreter.cpp"
Cohesion: 0.22
Nodes (20): pthread, run, applyMethod(), byteCount(), contextAlive(), CallContext, int16_t, uint8_t (+12 more)

### Community 126 - "P1-06: weak slot GC"
Cohesion: 0.25
Nodes (7): Files, Interfaces, P1-06: weak slot GC, コミット文, テスト, 手順, 目的

### Community 127 - "ImageFormat"
Cohesion: 0.09
Nodes (24): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, kImageEndianLittle (+16 more)

### Community 128 - "OperandStack"
Cohesion: 0.19
Nodes (8): answerWithoutSend(), deque, int64_t, WellKnown, OperandStack, roots, slots, smallIntegerAnswer()

### Community 129 - "TEST"
Cohesion: 0.12
Nodes (17): ChunkFileIn, ChunkLevelErrorsCarryTheChunkSpan, ClassDefinitionAbortIsAnErrorAndIsCleared, DoItIsNotEvaluated, DoubledBangCharacterFromAFileOut, DoubledBangInStringIsOneBang, ErrorSpanCountsUndoubledBangs, InstallsCompiledMethodAndKeepsOldOnError (+9 more)

### Community 130 - "Array.cpp"
Cohesion: 0.20
Nodes (22): valueHashWord(), ao_Array_equals(), ao_Array_hash(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_() (+14 more)

### Community 131 - "ChunkMethod"
Cohesion: 0.29
Nodes (7): ChunkMethod, source, span, undoubled, string, uint32_t, fileSpan()

### Community 132 - "P9-02: Browser accept"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-02: Browser accept, コミット文, テスト, 完了条件, 手順, 目的

### Community 133 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 134 - "P9-04: v1 ゴールデン受け入れ"
Cohesion: 0.25
Nodes (8): Files, Interfaces, P9-04: v1 ゴールデン受け入れ, コミット文, テスト, 完了条件, 手順, 目的

### Community 135 - "TEST"
Cohesion: 0.09
Nodes (26): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+18 more)

### Community 136 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, Build and test, Documentation, Layout, License, Requirements, Status

### Community 138 - "ObjectHeader"
Cohesion: 0.25
Nodes (8): checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size, Heap::header()

### Community 139 - "path"
Cohesion: 0.16
Nodes (12): path, string, uint32_t, expectRefused(), fileNames(), findBytesOfSize(), freshDir(), readAll() (+4 more)

### Community 140 - "specialIndex"
Cohesion: 0.27
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 141 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 142 - "B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）"
Cohesion: 0.25
Nodes (8): B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）, SPEC を先に直す, スタックガード、abort、死んだホーム, ネイティブ側（`Send.hpp/.cpp` に共通ヘルパーを置く）, リスク, ワークスペース変数（00 High ×3 と Medium を同時に解消する）, 共有 temp（`compiler/src/Codegen.cpp`）, 手順（各段を緑にしてから次へ進む）

### Community 143 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "codegen"
Cohesion: 0.12
Nodes (13): Codegen, CompileResult, error, image, ok, ParseResult, error, method (+5 more)

### Community 146 - ".fromSmallInteger"
Cohesion: 0.24
Nodes (19): CompiledMethod accessors (P5), int64_t, ao_CompiledMethod_bytecodes(), ao_CompiledMethod_literals(), ao_CompiledMethod_nativeCode(), ao_CompiledMethod_numArgs(), ao_CompiledMethod_numTemps(), ao_CompiledMethod_primitive() (+11 more)

### Community 147 - "intern"
Cohesion: 0.25
Nodes (8): isFixedGlobal, isPseudoVariableName, bytes(), string_view, WellKnown, intern(), WellKnown::define(), WellKnown::internSpecialSelectors()

### Community 148 - "P4-04 Array / ByteArray / String / Symbol"
Cohesion: 0.33
Nodes (6): Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol, send

### Community 149 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 150 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 152 - "RootedArray"
Cohesion: 0.20
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 153 - "BlockProbe"
Cohesion: 0.22
Nodes (7): [Medium] ストリームの `contents` が String と Array 以外のコレクションで壊れた値を返す, BlockProbe, b, probe, int64_t, Root, ocAt()

### Community 154 - "P4 — Kernel Native Implementation"
Cohesion: 0.07
Nodes (31): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean (+23 more)

### Community 156 - "abortingSubclass"
Cohesion: 0.67
Nodes (4): abortingSubclass(), countingPrintString(), CallContext, uint32_t

### Community 157 - "send2"
Cohesion: 0.11
Nodes (20): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, ClassDefinitionThroughAliasOnlyRebindsGlobal, GrowKeepsOuterOopAndEntries, KeepsNativeIdentityEquals, NinthMethodWithFullNurseryIsInstalled (+12 more)

### Community 158 - "uint32_t"
Cohesion: 0.31
Nodes (6): uint32_t, unique_ptr, Temps, n, roots, slots

### Community 160 - "Gc"
Cohesion: 0.06
Nodes (42): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, optional, Gc, clearWeakAfterNursery (+34 more)

### Community 161 - "P4-06 Stream / Transcript model"
Cohesion: 0.33
Nodes (6): Bootstrap, NativeMethod, P4-06 Stream / Transcript model, send, Stream, Transcript

### Community 162 - "README.md"
Cohesion: 0.09
Nodes (19): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+11 more)

### Community 163 - "P4-07 Process / ProcessorScheduler / Semaphore"
Cohesion: 0.29
Nodes (7): Bootstrap, NativeMethod, P4-07 Process / ProcessorScheduler / Semaphore, Process, ProcessorScheduler, Semaphore, send

### Community 164 - "P4-05 Dictionary / Set / OrderedCollection"
Cohesion: 0.40
Nodes (5): Bootstrap, Dictionary, NativeMethod, P4-05 Dictionary / Set / OrderedCollection, send

### Community 165 - "FileSizeLimit"
Cohesion: 0.33
Nodes (4): rlim_t, FileSizeLimit, oldAction_, oldLimit_

### Community 166 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 167 - "TEST"
Cohesion: 0.15
Nodes (13): ClassSideShowForwardsToInstanceHook, EachClassSkipsSmalltalkImageNonClassExtra, NextPutAllCopiesCollection, NextPutAndClearInvokeHook, NextPutAtSmiMaxPositionFails, ReadStreamNextPositionResetContents, TEST(), ShowInvokesHook (+5 more)

### Community 168 - "cli_test.sh"
Cohesion: 0.73
Nodes (5): fail(), run(), cli_test.sh script, stderr_is(), write_fixtures()

### Community 169 - "Compile.cpp"
Cohesion: 0.14
Nodes (29): cctype, CompileError, message, span, string, acceptClassSource(), assignError(), CarriedMethod (+21 more)

### Community 170 - "Frame"
Cohesion: 0.20
Nodes (10): ContextExitGuard, FieldRoots, frame, Frame, context, isBlock, method, pc (+2 more)

### Community 171 - "TestDir"
Cohesion: 0.40
Nodes (3): path, TestDir, path

### Community 172 - "answerOne"
Cohesion: 0.67
Nodes (4): answerOne(), answerTwo(), CallContext, uint32_t

### Community 173 - "Image::load"
Cohesion: 0.33
Nodes (9): bindAll(), checkGlobals(), string_view, WellKnown, fileOop(), Image::load(), recordKind(), relocate() (+1 more)

### Community 174 - "abortingNew"
Cohesion: 0.67
Nodes (3): abortingNew(), CallContext, uint32_t

### Community 175 - "native_send_test.cpp"
Cohesion: 0.40
Nodes (9): answerMessage(), CallContext, NativeFn, uint32_t, install(), pairAfterAlloc(), stubA(), stubB() (+1 more)

### Community 176 - "assemble"
Cohesion: 0.50
Nodes (4): initializer_list, assemble(), uint8_t, op()

### Community 177 - "B10 協調スケジューラ（02 High）"
Cohesion: 0.22
Nodes (8): ファイルごとの変更, B10 協調スケジューラ（02 High）, SPEC §3.4 に書く意味論, ファイバ, ファイルごとの変更, プロセスごとの状態, リスク, 手順（各段を緑にしてから次へ進む）

### Community 178 - "ImageSelector"
Cohesion: 0.67
Nodes (3): ImageSelector, name, WellKnown

### Community 179 - "performSend"
Cohesion: 0.29
Nodes (10): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, branchTruth(), clearNonlocal(), consumeNonlocal(), hit(), Leave, leave, value (+2 more)

### Community 181 - "Format.hpp"
Cohesion: 0.48
Nodes (6): bits(), int64_t, isBytes(), isIndexable(), isPointers(), make()

### Community 182 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 183 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

## Knowledge Gaps
- **832 isolated node(s):** `.metaFlag`, `.hasUnacceptedChanges`, `.title`, `.sourceText`, `.errorText` (+827 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1879 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **19 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `.isHeap`, `LargeInteger.cpp`, `TEST`, `Stream.cpp`, `Heap.cpp`, `Float.cpp`, `send`, `applyChunks`, `vector`, `Heap`, `Roots`, `Geometry.cpp`, `TEST`, `Boot`, `TEST`, `TEST`, `TEST`, `image_save_load_test.cpp`, `string`, `P2-02 class object skeletons`, `ImageSave.cpp`, `Loaded`, `CallContext`, `TEST`, `.nil`, `TEST`, `BlockContext.cpp`, `TEST_F`, `TEST`, `SmallInteger.cpp`, `TEST`, `String.cpp`, `CallContext`, `allocateRetry`, `DiskHeader`, `installMethod`, `WellKnown.cpp`, `TEST`, `Boolean.cpp`, `Character.cpp`, `Behavior.cpp`, `TEST`, `abi.cpp`, `TEST`, `Bootstrap.cpp`, `Session.cpp`, `.isTrue`, `native_method_test.cpp`, `TEST`, `NativeMethod.cpp`, `P3-01 Symbol intern`, `ClassPool.cpp`, `Interpreter.cpp`, `OperandStack`, `Array.cpp`, `TEST`, `Ao`, `ObjectHeader`, `path`, `GarbageFirstBoot`, `Roots.cpp`, `.fromSmallInteger`, `intern`, `TEST`, `RootedArray`, `BlockProbe`, `abortingSubclass`, `send2`, `uint32_t`, `WellKnown::checkSmallIntegerFastPath`, `Gc`, `README.md`, `imageRegistryStubA`, `Compile.cpp`, `Frame`, `answerOne`, `Image::load`, `abortingNew`, `native_send_test.cpp`, `assemble`, `ImageSelector`, `performSend`, `Format.hpp`, `ActiveGuard`?**
  _High betweenness centrality (0.355) - this node is a cross-community bridge._
- **Why does `vector` connect `vector` to `Codegen.cpp`, `ChunkMethod`, `LargeInteger.cpp`, `Scanner.cpp`, `VendorExtract.cpp`, `Oop`, `Emitter`, `path`, `Literal`, `send`, `applyChunks`, `Roots`, `uint64_t`, `ao_main.cpp`, `Gc`, `image_save_load_test.cpp`, `ChunkParser.cpp`, `string`, `Compile.cpp`, `ImageSave.cpp`, `Image::load`, `Ast`, `Parser`, `CallContext`, `allocateRetry`, `installMethod`, `TEST`, `Behavior.cpp`, `TEST`, `TEST`, `Bootstrap.cpp`, `Vendor.hpp`, `Session.cpp`, `ImageLoad.cpp`, `NativeMethod.cpp`, `ClassPool.cpp`, `Reentry`, `ImageSurgery`?**
  _High betweenness centrality (0.124) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `Oop`, `Stream.cpp`, `P2-02 class object skeletons`, `WellKnown.cpp`, `Heap`, `Roots`, `intern`, `Behavior.cpp`, `uint64_t`, `WellKnown::checkSmallIntegerFastPath`?**
  _High betweenness centrality (0.071) - this node is a cross-community bridge._
- **Are the 13 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 13 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.hasUnacceptedChanges`, `.title` to the rest of the system?**
  _832 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `WellKnown` be split into smaller, more focused modules?**
  _Cohesion score 0.013245033112582781 - nodes in this community are weakly interconnected._
- **Should `Codegen.cpp` be split into smaller, more focused modules?**
  _Cohesion score 0.04983164983164983 - nodes in this community are weakly interconnected._
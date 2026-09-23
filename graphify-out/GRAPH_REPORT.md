# Graph Report - ao-smalltalk  (2026-09-23)

## Corpus Check
- 248 files · ~126,546 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 77 file(s) not represented in the graph (top: .st 62, (none) 13, .toml 1)

## Summary
- 3536 nodes · 9101 edges · 162 communities (147 shown, 15 thin omitted)
- Extraction: 87% EXTRACTED · 13% INFERRED · 0% AMBIGUOUS · INFERRED: 1201 edges (avg confidence: 0.87)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `9bc7fd23`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- WellKnown
- HostTempGuard
- Dictionary.cpp
- LargeInteger.cpp
- image_save_load_test.cpp
- Scanner.cpp
- VendorExtract.cpp
- Oop
- Stream.cpp
- Float.cpp
- Emitter
- Heap.cpp
- TEST
- Literal
- putNative
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
- Gc
- TEST
- WorkspaceWindow
- Session.cpp
- vector
- TEST
- Bytecode interpreter
- P7 .aoimage Implementation Plan
- TEST
- TEST
- gc_safety_test.cpp
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
- TEST
- .fromSmallInteger
- heap
- TEST
- BlockContext.cpp
- 指摘
- TEST
- TEST
- Parser
- send
- ao_abi.h
- TEST
- TEST
- String.cpp
- roots
- Interpreter::run
- 指摘
- ToolWindowTests
- Chunk
- wellknown
- BrowserWindow
- DiskHeader
- TEST
- TranscriptWindow
- WellKnown.cpp
- Ao.app skeleton
- TEST
- P9-03: エラー表示と VoiceOver
- TEST
- Codegen.cpp
- .fromBits
- Boot
- TEST
- ao image save
- string
- BrowserModel
- .isHeap
- .false_
- TEST_F
- Ao
- VirtualRegion.cpp
- TEST
- allocateRetry
- Scanner
- build.sh
- test.sh
- README.md
- Character.cpp
- TEST
- image/Kernel/Object.st
- image/Kernel/Behavior.st
- oop
- Bootstrap.cpp
- Claude レビュー指摘の修正計画（docs/claude-review）
- Token
- TEST_F
- Memory maintenance guide
- string
- string_view
- DEFERRED.md
- .applicationWillFinishLaunching
- native_method_test.cpp
- BrowserModelTests
- ImageLoad.cpp
- .build
- performSend
- ao_Magnitude_lessOrEqual
- P4 — Kernel Native Implementation
- Boolean.cpp
- imageRegistryStubA
- AppKit
- GarbageFirstBoot
- ClassMethodCache
- abi.cpp
- NameCollect
- ObjectHeader
- ImageFormat
- Bytecode.hpp
- TEST
- Claude レビュー指摘の修正計画（docs/claude-review）
- P4-03 Magnitude / SmallInteger / Character
- TEST
- Interpreter.cpp
- Frame
- TEST
- 指摘
- package-app.sh
- Array.cpp
- Globals.cpp
- NativeMethod.cpp
- 01 オブジェクトメモリ・GC・ブートストラップ
- 仕様
- ao_image_load
- Roots.cpp
- RootedArray
- TEST
- applyMethod
- Format.hpp
- 指摘
- claude-review/README.md
- RawChunk
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- P9-02: Browser accept
- P9-04: v1 ゴールデン受け入れ
- B2 ブロックの意味論とインタプリタ（05 Critical、02 全件、00 のワークスペース High ×4）
- ActiveGuard
- native_send_test.cpp
- Symbol.cpp
- TEST
- TEST
- TEST

## God Nodes (most connected - your core abstractions)
1. `Oop` - 647 edges
2. `Heap` - 206 edges
3. `WellKnown` - 154 edges
4. `Roots` - 94 edges
5. `TEST()` - 73 edges
6. `send()` - 71 edges
7. `Ast` - 67 edges
8. `allocateRetry()` - 66 edges
9. `TEST()` - 63 edges
10. `Parser` - 55 edges

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

## Communities (162 total, 15 thin omitted)

### Community 0 - "WellKnown"
Cohesion: 0.01
Nodes (146): ExtraTable, InternTable, unique_ptr, WellKnown, addRoots, arrayClass, arrayedCollectionClass, arrayedCollectionMetaclass (+138 more)

### Community 1 - "HostTempGuard"
Cohesion: 0.15
Nodes (14): uint32_t, unique_ptr, HostTempGuard, active, count, ctx, prevCount, prevPtr (+6 more)

### Community 2 - "Dictionary.cpp"
Cohesion: 0.15
Nodes (39): ao_Association_key(), ao_Association_value(), ao_Bag_add_(), ao_Bag_do_(), ao_Dictionary_at_(), ao_Dictionary_at_put_(), ao_Dictionary_includesKey_(), ao_Dictionary_new() (+31 more)

### Community 3 - "LargeInteger.cpp"
Cohesion: 0.12
Nodes (56): Digits, __int128, add(), addBig(), asInt64IfFits(), Big, d, neg (+48 more)

### Community 4 - "image_save_load_test.cpp"
Cohesion: 0.07
Nodes (41): ImageSave, ImageSaveLoad, iterator, KernelMethodsStayNative, LoadSizesOldFromHeader, NurseryGcAfterLoad, OnePlusTwoMatches, RejectsBadMagicAndSecondLoad (+33 more)

### Community 5 - "Scanner.cpp"
Cohesion: 0.19
Nodes (18): cmath, string_view, uint32_t, digitValue(), isAlnum(), isAlpha(), isDigit(), isSpace() (+10 more)

### Community 6 - "VendorExtract.cpp"
Cohesion: 0.07
Nodes (64): Task 4: LOAD_ORDER とホストパッチ, extra 節, string, vector, VendorClassFile, chunkText, className, superName (+56 more)

### Community 7 - "Oop"
Cohesion: 0.11
Nodes (58): [High] printOn: が新しい printString を使わない, Oop, kCharTag, kImmTag, kLow3, kSmiTag, raw_, ao_Object_basicAt_() (+50 more)

### Community 8 - "Stream.cpp"
Cohesion: 0.13
Nodes (51): [Medium] 固定長の String に書く WriteStream が、多バイト文字を黙って捨てる, Task 2: Transcript のクラス側転送とフック, ao_PositionableStream_contents(), ao_PositionableStream_next(), ao_PositionableStream_nextPut_(), ao_PositionableStream_on_(), ao_PositionableStream_position(), ao_PositionableStream_reset() (+43 more)

### Community 9 - "Float.cpp"
Cohesion: 0.23
Nodes (25): [High] Integer をレシーバとする Float / Fraction との混合演算・比較が空 OOP を返す, FlOp, ao_Float_add(), ao_Float_divide(), ao_Float_equals(), ao_Float_lessThan(), ao_Float_multiply(), ao_Float_printString() (+17 more)

### Community 10 - "Emitter"
Cohesion: 0.14
Nodes (14): int16_t, Op, size_t, string_view, uint16_t, uint8_t, Emitter, bindingsImage_ (+6 more)

### Community 11 - "Heap.cpp"
Cohesion: 0.09
Nodes (43): charconv, allocateTenured, growOld, header, initObject, largeObjectBytes, objectBytes, oldUsed (+35 more)

### Community 12 - "TEST"
Cohesion: 0.04
Nodes (48): ByteObjectPayloadIsNotScannedAsOops, CollectOldOnlyAfterThreshold, CompactionDuringScavengeDoesNotCorruptSlots, DeadOldSlotIsNotANurseryRoot, DestJumpPastPinDoesNotOverlap, DuplicateRootForwardedOnce, GcOld, GcRoots (+40 more)

### Community 13 - "Literal"
Cohesion: 0.06
Nodes (41): CompileEnv, instVarNames, knownGlobals, undeclaredAreTemps, workspaceTemps, CompileResult, error, image (+33 more)

### Community 14 - "putNative"
Cohesion: 0.09
Nodes (31): WellKnown, installArray(), WellKnown, installBehavior(), WellKnown, installBoolean(), WellKnown, installCharacter() (+23 more)

### Community 15 - "TEST"
Cohesion: 0.25
Nodes (8): KernelBench, KernelScan, MethodDictionaryValuesAreNativeMethods, RequiredSelectorsAreNativeMethods, nativeRequiredDictsAreNative(), TEST(), TenMillionToDo, UserCompiledMethodDoesNotFailScan

### Community 16 - "SPEC.md"
Cohesion: 0.12
Nodes (25): Ao macOS native Smalltalk, Ao.app AppKit tools, ao CLI, ao::Oop typed pointer, .aoimage snapshot, AppKit host GUI, Blue Book compliance, Bootstrap order §3.7 (+17 more)

### Community 17 - "指摘"
Cohesion: 0.12
Nodes (20): isBinaryChar(), [High] 2^63 以上の整数リテラルが黙って 0 になる, [High] `,` が二項演算子の文字に含まれず、文字列連結がコンパイルできない, [Low] カスケードの扱いが不完全, [Low] 宣言の検証がない（引数への代入・擬変数名・重複名）, [Low] 指数表記の意味が Smalltalk-80 と異なる, [Medium] `! !` のあとの DoIt チャンクが、直前クラスのメソッドとしてインストールされる, [Medium] 二項演算子の直後の負数リテラルが、別のセレクタとして字句化される (+12 more)

### Community 18 - "ChunkAction"
Cohesion: 0.11
Nodes (19): ChunkKind, ChunkAction, category, className, classVars, instVars, kind, meta (+11 more)

### Community 19 - "Heap"
Cohesion: 0.04
Nodes (50): Heap, containsNurseryFrom, containsNurseryTo, fitsOld, flipNursery, fromBump_, fromEnd_, fromStart_ (+42 more)

### Community 20 - "Roots"
Cohesion: 0.05
Nodes (46): cassert, cstddef, memory, CallContext, Root, slot, FrameBlock, capacity (+38 more)

### Community 21 - "Geometry.cpp"
Cohesion: 0.14
Nodes (38): [High] int64 を超える整数リテラルが黙って 0 になる（compiler 側。他レビューと重複の可能性あり）, [Low] `asCharacter` がサロゲート（U+D800–DFFF）を受け付け、Print it の出力が途中で切れる, [Low] `&` `|` `eqv:` `xor:` が Boolean 以外の引数を false に丸める, [Low] `LargeInteger::fromInt64` は nursery が満杯でも GC を再試行しない, [Low] Point と Rectangle のネイティブがサブクラスを扱えない, [Low] Point の算術が成分計算の失敗を空 OOP のまま新しい Point に格納する, [Low] `to:do:` の終端が SmallInteger でないと失敗する, [Low] `=` は値で比較するのに `hash` は同一性ハッシュのままで、`=`/`hash` の契約を破る (+30 more)

### Community 22 - "TEST"
Cohesion: 0.07
Nodes (28): BitOpsAndShift, CharacterProtocol, CompareAndBetween, DivisionByZeroReturnsErrorString, FloatArithmetic, FloatEqualsDoesNotCoerceInteger, FloorDivAndModulo, FractionMulDiv (+20 more)

### Community 23 - "P1 — Object Memory"
Cohesion: 0.07
Nodes (32): P1 — Object Memory, ao::Oop, ao::Gc, ao::Heap, nursery bump allocator, 64-bit tagged pointer, P1-01: ao::Oop tags, Oop.hpp tagged pointer API (+24 more)

### Community 24 - "SmallInteger.cpp"
Cohesion: 0.25
Nodes (25): Native selector mangling, マングル表, 規則, ao_Integer_asCharacter(), ao_Integer_bitAnd_(), ao_Integer_bitOr_(), ao_Integer_bitShift_(), ao_Integer_bitXor_() (+17 more)

### Community 25 - "Gc"
Cohesion: 0.07
Nodes (39): BlockContextKeepsHomeAndCopied, ContextGc, cstdlib, MethodContextSurvivesNurseryCollection, NativeBlockThunkStillValues, optional, Gc, clearWeakAfterNursery (+31 more)

### Community 26 - "TEST"
Cohesion: 0.05
Nodes (45): AsSymbolWithFullNursery, BetweenAndAcrossGc, BrokenSuperclassChain, CollectThunkMethodFailureSetsOutOfMemory, CollectWithFullNursery, CompileInstVarReferenceStops, CopyArrayLargerThanNursery, DivisionByZeroWithFullNursery (+37 more)

### Community 27 - "WorkspaceWindow"
Cohesion: 0.08
Nodes (27): Int32, failureText(), installErrorField(), sendToKeyWorkspace(), spanMessage(), AoSpan, Bool, Int (+19 more)

### Community 28 - "Session.cpp"
Cohesion: 0.06
Nodes (92): Image, load, save, boot(), blankOut(), browserClassAt(), browserClassCount(), browserClassDefinition() (+84 more)

### Community 29 - "vector"
Cohesion: 0.10
Nodes (22): algorithm, ao_abi, climits, vector, dyld, filesystem, runtime, ao executable (+14 more)

### Community 30 - "TEST"
Cohesion: 0.07
Nodes (35): BootstrapInstallsObjectIdentityEquals, ClassSkeletonsAreHeapAndNamed, CycleEveryClassIsInstanceOfItsMetaclass, CycleEveryMetaclassIsInstanceOfMetaclass, CycleImmediateClassOf, CycleMetaclassClassClassIsMetaclass, CycleMetaclassHierarchyParallelsClasses, CycleMetaclassInheritsFromClassDescription (+27 more)

### Community 31 - "Bytecode interpreter"
Cohesion: 0.13
Nodes (22): CompiledMethod class (P4 Kernel), Scanner (字句解析), Token stream, AST, Parser and AST, Ao bytecode ISA, Opcode constants, Codegen CompiledMethod (+14 more)

### Community 32 - "P7 .aoimage Implementation Plan"
Cohesion: 0.18
Nodes (11): Global Constraints, P7 .aoimage Implementation Plan, Task 2: 再結合に必要なヒープとレジストリの口, Task 3: save, Task 5: CLI, Task 6: フェーズを閉じる, リスク, 仕様との対応 (+3 more)

### Community 33 - "TEST"
Cohesion: 0.10
Nodes (21): BlockContext, CallContextHooksDefaultNull, FalseIfTrueIfFalseReturnsNgBlock, FormatBitsForIndexableClasses, IdentityEqualsAndYourself, IdentityHashOfImmediates, InspectCallsHookAndReturnsSelf, KernelCatalog (+13 more)

### Community 34 - "TEST"
Cohesion: 0.11
Nodes (19): ArrayPrintsElementPrintStrings, EmptyArrayPrintsEmptyLiteral, FalsePrintsFalse, FloatOnePrintStringContainsOne, LargeIntegerPrintsClassName, NegativeSmallIntegerPrintsLeadingMinus, NestedArrayPastDepthFourPrintsEllipsis, NilPrintsNil (+11 more)

### Community 35 - "gc_safety_test.cpp"
Cohesion: 0.13
Nodes (22): Bootstrap, chrono, compile, CompiledMethod, compiler, context, cstdio, cstring (+14 more)

### Community 36 - "ChunkParser.cpp"
Cohesion: 0.23
Nodes (21): atLineEnd(), bangSpaceBangAt(), classify(), string_view, uint32_t, vector, firstLine(), firstLineHas() (+13 more)

### Community 37 - "P3 — Native Dispatch"
Cohesion: 0.09
Nodes (26): P3 — Native Dispatch, doesNotUnderstand:, method lookup / send, MethodDictionary, Heap, Oop, P3-01 Symbol intern, Symbol (+18 more)

### Community 38 - "CompileError"
Cohesion: 0.11
Nodes (40): CompileError, message, span, string, [Critical] クラス定義でない文字列が AO_OK で黙って捨てられる, [Medium] メソッド辞書の拡張に失敗するとメソッドを黙って捨て、`installMethod` は成功を返す, [High] チャンク経由でメソッドを再定義してもメソッドキャッシュを無効化しない, [Low] チャンクの種別を 1 行目のキーワードで判定するので、メソッドをクラス定義と誤認する (+32 more)

### Community 39 - "TEST"
Cohesion: 0.10
Nodes (18): BlockWithArgs, Cascade, ParseResult, error, method, ok, deque, parseMethod() (+10 more)

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
Cohesion: 0.16
Nodes (21): Phase P0 skeleton, Phase pipeline P0-P9, P0 phase doc, P8 — AppKit Tools, System Browser 5-pane, Transcript NSWindow, Workspace NSWindow, P9 — Integration (+13 more)

### Community 48 - "CallContext"
Cohesion: 0.14
Nodes (14): CallContext, activeContext, cache, hostTempCount, hostTemps, inspectHook, interpretedBytecodes, nonlocalHome (+6 more)

### Community 49 - "TEST"
Cohesion: 0.13
Nodes (19): DnuWithFullNurseryReturnsMessage, DoesNotUnderstandAppliesSubclassNative, DoesNotUnderstandReturnsMessage, IdentityEqualsAndClass, NativeSend, allocate, setGcStress, slotAtPut (+11 more)

### Community 50 - ".fromSmallInteger"
Cohesion: 0.14
Nodes (17): CallContext, uint16_t, uint8_t, create(), CallContext, uint8_t, createBlock(), createMethod() (+9 more)

### Community 52 - "TEST"
Cohesion: 0.07
Nodes (27): ArrayCollectDoublesViaNativeBlock, ArraySelectRejectDetectInjectIncludesIsEmpty, AssociationKeyValue, BagLinkedListMappedCollectionStubs, CollectDoesNotGrowNativeRegistry, CollectIndexAtSmiMaxFails, CollectIndexPokedByUserBlockFails, CollectionDo (+19 more)

### Community 53 - "BlockContext.cpp"
Cohesion: 0.27
Nodes (13): ao_BlockContext_value(), ao_BlockContext_value_value_(), ao_BlockContext_valueWithArguments_(), applyBlock(), CallContext, NativeFn, string, string_view (+5 more)

### Community 54 - "指摘"
Cohesion: 0.10
Nodes (20): InspectorWindow, .text, NSTextView, NSWindow, String, aoWorkspaceInspectHook(), CChar, UnsafeMutableRawPointer (+12 more)

### Community 55 - "TEST"
Cohesion: 0.15
Nodes (13): ArrayNewIsEmptyArray, BasicNewColonAllocatesIndexableSlots, Behavior, EachClassAndGlobalsSeeExtraNamed, InheritsFromWalksSuperclassChain, InstSizeAndFormatBitsArrayVsObject, MetaclassNewIsForbidden, MetaclassNewReturnsShouldNotImplementString (+5 more)

### Community 56 - "TEST"
Cohesion: 0.15
Nodes (13): BlockContextForkCreatesAndResumesProcess, MethodContextGettersReadSlots, NamedProcessorIsSchedulerInstance, PriorityColonStoresSmallInteger, Process, ResumeYieldSuspendRoundTrip, TEST(), SchedulerHoldsOneActiveProcess (+5 more)

### Community 57 - "Parser"
Cohesion: 0.12
Nodes (19): uint32_t, SourceSpan, end, start, Kind, string, string_view, Tok (+11 more)

### Community 58 - "send"
Cohesion: 0.06
Nodes (100): [Critical] Magnitude の `<=` / `between:and:` が GC をまたいで未ルートの receiver を使い、ヒープ破壊を起こす, CompiledMethod accessors (P5), Task 4: load と受け入れ評価, int64_t, InlineCache, cachedClass, cachedMethod, Gc::clearWeakAfterOldMark() (+92 more)

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

### Community 64 - "Interpreter::run"
Cohesion: 0.25
Nodes (15): safepoint, byteCount(), CallContext, int16_t, uint8_t, decodeHeader(), instSlot(), Interpreter::run() (+7 more)

### Community 65 - "指摘"
Cohesion: 0.11
Nodes (18): [Critical] `subclass:…` ネイティブが GC をまたいで receiver と名前をルートせずに持ち、クラスに宙ぶらりんのポインタを書き込む, [High] Dictionary の `do:` が値ではなく Association を渡し、Collection 系の結果が `collect:` と食い違う。vendor の Bag も動かない, [Low] OrderedCollection の `at:` が、範囲外の添字に nil を返す, [Low] `perform:withArguments:` が Array 以外も受け付け、ルートされない引数配列を渡す, [Low] ReadStream の `nextPut:` が元のコレクションを書き換える, [Low] ReadWriteStream の `contents` が position までしか返さない, [Low] `Symbol>>asString` が、GC のあとで古いアドレスから読む, [Low] Symbol の `copy` が、intern されていない別の Symbol を作る (+10 more)

### Community 66 - "ToolWindowTests"
Cohesion: 0.30
Nodes (6): SmokeTests, NSMenu, NSMenuItem, String, ToolWindowTests, XCTestCase

### Community 67 - "Chunk"
Cohesion: 0.20
Nodes (10): BangInCharacterDoesNotSplit, BangSpaceBangEndsMethodButDoubleBangStaysLiteral, Chunk, ClassDefinitionShape, CommentStampApostropheDoesNotSwallowClassDef, CommentStampQuoteDoesNotSwallowClassDef, TEST(), LineEndBangAfterBinaryStillTerminates (+2 more)

### Community 69 - "BrowserWindow"
Cohesion: 0.09
Nodes (25): Any, BrowserWindow, .errorText, .paneAccessibilityLabels, .sourceText, .title, sendToKeyBrowser(), Bool (+17 more)

### Community 70 - "DiskHeader"
Cohesion: 0.09
Nodes (26): bit, byte, size_t, uint16_t, uint32_t, uint64_t, DiskHeader, endian (+18 more)

### Community 71 - "TEST"
Cohesion: 0.25
Nodes (8): CharacterRoundTrip, FromSmallIntegerOutOfRangeDies, HeapAlignedPointerRoundTrip, IdentityEqualsIsBits, ImmediateThreePatterns, OopTag, TEST(), SmallIntegerRoundTrip

### Community 72 - "TranscriptWindow"
Cohesion: 0.13
Nodes (16): aoTranscriptHook(), makeToolTextWindow(), Bool, CChar, Int32, NSRect, NSTextView, NSWindow (+8 more)

### Community 73 - "WellKnown.cpp"
Cohesion: 0.07
Nodes (35): string_view, isVendorStub(), internWith, deque, size_t, string, string_view, unordered_map (+27 more)

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

### Community 78 - "Codegen.cpp"
Cohesion: 0.07
Nodes (32): BlockCaptureIsMethodTemp, CascadeAndBlock, Codegen, LitKind, MethodImage, string, vector, disassemble() (+24 more)

### Community 79 - ".fromBits"
Cohesion: 0.33
Nodes (3): Task 1: 形式ヘッダと即値ビット, 前提, uint64_t

### Community 80 - "Boot"
Cohesion: 0.08
Nodes (29): BrokenParent, BrokenCase, label, parent, int64_t, string, defineBrokenClass(), defineEmptyClass() (+21 more)

### Community 81 - "TEST"
Cohesion: 0.10
Nodes (19): AllocateNoGcSpillsToOld, ByteObjectPayload, ExhaustionReturnsEmpty, HeapAlloc, LargeObjectAllocatedInOld, ObjectLargerThanNurseryAllocates, OldGrowsPastInitialCapacity, OldReserveFailureIsReported (+11 more)

### Community 82 - "ao image save"
Cohesion: 0.50
Nodes (5): .aoimage format (AOIM), No function-pointer fields in image, ao image save, NativeMethod as symbol-name strings on save, ao image load and NativeMethod rebind

### Community 83 - "string"
Cohesion: 0.13
Nodes (6): string, string_view, cstdint, NativeMethod, CallContext, WellKnown

### Community 84 - "BrowserModel"
Cohesion: 0.21
Nodes (9): BrowserModel, .metaFlag, ListedClass, Bool, CChar, Int32, String, Task 6: System Browser の 5 ペイン (+1 more)

### Community 85 - ".isHeap"
Cohesion: 0.41
Nodes (11): [High] Dictionary / Set が hash を捨てて線形探索し、要素数の 2 乗で遅くなる, ao_Dictionary_collect_(), ao_Dictionary_do_(), ao_Dictionary_includes_(), ao_Set_do_(), consumeHash(), Root, ensureInner() (+3 more)

### Community 86 - ".false_"
Cohesion: 0.20
Nodes (25): ao_Behavior_basicNew_(), ao_Behavior_compiledMethodAt_(), ao_Behavior_includesSelector_(), ao_Behavior_inheritsFrom_(), ao_Behavior_instSize(), ao_Behavior_isBytes(), ao_Behavior_isPointers(), ao_Behavior_isVariable() (+17 more)

### Community 87 - "TEST_F"
Cohesion: 0.15
Nodes (15): [Low] count 系 ABI がエラーで AO_ERR(=1) を返し、件数 1 と区別できない, Browser の読み取り, Task 3: Browser 読み取り ABI, ObjectIsKernelAndPrintStringIsNative, ao_browser_class_count(), ao_browser_protocol_at(), ao_browser_protocol_count(), ao_browser_selector_at() (+7 more)

### Community 88 - "Ao"
Cohesion: 0.29
Nodes (7): Ao, ビルドとテスト, ライセンス, 必要環境, 文書, 構成, 現状

### Community 89 - "VirtualRegion.cpp"
Cohesion: 0.18
Nodes (14): mman, size_t, byte, size_t, pageBytes(), roundUp(), VirtualRegion, commit (+6 more)

### Community 90 - "TEST"
Cohesion: 0.13
Nodes (14): AoTestRunner, ArgumentAndOuterTemp, ArrayDoNonLocalReturnStopsAtFirst, BlockEval, CopyDoesNotWriteOuter, EarlierOutOfMemoryIsNotBlamedOnFile, ExampleFilePasses, IfTrueIfFalseFromCompiledMethod (+6 more)

### Community 91 - "allocateRetry"
Cohesion: 0.13
Nodes (21): B1 GC 安全性と old space の拡張（テーマ 1、01 全件、03/04 の Critical）, GC と Heap（Gc.cpp、Heap.cpp、新規 `runtime/src/VirtualRegion.{hpp,cpp}`）, SPEC を先に直す, その他（同じバッチで処理する）, テスト（書く順。各段を緑にしてから次へ進む）, ネイティブのルートを構造で保証する, リスク, 共通ヘッダ `runtime/include/ao/HandleScope.hpp`（＋`runtime/src/HandleScope.cpp`） (+13 more)

### Community 92 - "Scanner"
Cohesion: 0.14
Nodes (20): ArrayAndByteArrayHeaders, AssignVariantsAndComment, uint32_t, Scanner, i_, lexBinary, lexCharacter, lexIdentOrKeyword (+12 more)

### Community 95 - "README.md"
Cohesion: 0.08
Nodes (21): Build, Commits, Contributing, Current phase, コミット, ビルド, ライセンス, 曲げない規則 (+13 more)

### Community 96 - "Character.cpp"
Cohesion: 0.44
Nodes (9): natives, ao_Character_asCharacter(), ao_Character_asciiValue(), ao_Character_asInteger(), ao_Character_equals(), ao_Character_lessThan(), ao_Character_printString(), CallContext (+1 more)

### Community 97 - "TEST"
Cohesion: 0.40
Nodes (5): ChunkFileIn, DoItIsNotEvaluated, InstallsCompiledMethodAndKeepsOldOnError, InstVarReadCompilesPushInstVar, TEST()

### Community 103 - "Bootstrap.cpp"
Cohesion: 0.17
Nodes (20): [High] ブートストラップしたクラスの名前が Symbol ではなくクラス nil のバイト列で、`Object name` にメッセージを送れない, format, allocateSkeletons(), allocClass(), ClassDef, bytes, indexable, instSize (+12 more)

### Community 104 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.10
Nodes (21): [Critical] Kernel クラスのサブクラスで、インスタンス変数の添字が親の物理スロットと重なる, B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B4 クラスのメタデータ（Kernel の形と名前）, B6 イメージとセッションの堅牢化, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム） (+13 more)

### Community 105 - "Token"
Cohesion: 0.15
Nodes (13): int64_t, string, Tok, Token, intValue, isFloat, kind, number (+5 more)

### Community 106 - "TEST_F"
Cohesion: 0.12
Nodes (15): BlockAssignmentDoesNotUpdateWorkspaceBinding, BootThenImageRoundTripKeepsOnePlusTwo, EvalTranscriptShowThenCr, HugeAllocationReportsOutOfMemory, InspectItPrintsAndNotifiesHook, KnownGlobalStaysGlobal, MissingLoadOrderIsError, OutOfMemoryInspectItDoesNotCallInspectHook (+7 more)

### Community 111 - ".applicationWillFinishLaunching"
Cohesion: 0.12
Nodes (12): AoApp, openImageFile(), saveImageFile(), Int32, Notification, NSMenuItem, LaunchSet, Task 5: Transcript と Workspace の窓 (+4 more)

### Community 112 - "native_method_test.cpp"
Cohesion: 0.21
Nodes (16): AddInternsByFunctionPointer, InvokeRootsDirectCall, NameAndApply, ReceiverRefTracksMove, callsWithLocal(), CallContext, NativeFn, uint32_t (+8 more)

### Community 113 - "BrowserModelTests"
Cohesion: 0.20
Nodes (9): BrowserModelTests, NSSegmentedControl, NSTableView, NSTextView, NSView, String, T, StaticString (+1 more)

### Community 114 - "ImageLoad.cpp"
Cohesion: 0.18
Nodes (35): acceptWord(), bindAll(), checkGlobals(), byte, size_t, string, string_view, T (+27 more)

### Community 115 - ".build"
Cohesion: 0.31
Nodes (8): Actions, MainMenu, MenuAction, NSMenu, NSMenuItem, String, Void, Selector

### Community 116 - "performSend"
Cohesion: 0.20
Nodes (7): ファイルごとの変更, ファイルごとの変更, deque, OperandStack, roots, slots, performSend()

### Community 117 - "ao_Magnitude_lessOrEqual"
Cohesion: 0.58
Nodes (7): ao_Magnitude_between_and_(), ao_Magnitude_greaterOrEqual(), ao_Magnitude_greaterThan(), ao_Magnitude_lessOrEqual(), CallContext, uint32_t, sendBin()

### Community 118 - "P4 — Kernel Native Implementation"
Cohesion: 0.06
Nodes (33): P4 — Kernel Native Implementation, Kernel scan (all NativeMethod), SmallInteger arithmetic overflow to LargeInteger, Array, ArrayedCollection, Bootstrap, NativeMethod, P4-04 Array / ByteArray / String / Symbol (+25 more)

### Community 119 - "Boolean.cpp"
Cohesion: 0.23
Nodes (26): Task 8: printString, ao_Boolean_subclassResponsibility(), ao_False_and_(), ao_False_eqv_(), ao_False_ifFalse_(), ao_False_ifFalse_ifTrue_(), ao_False_ifTrue_(), ao_False_ifTrue_ifFalse_() (+18 more)

### Community 120 - "imageRegistryStubA"
Cohesion: 0.67
Nodes (4): CallContext, uint32_t, imageRegistryStubA(), imageRegistryStubB()

### Community 121 - "AppKit"
Cohesion: 0.18
Nodes (5): Ao, AppKit, CAo, PackageDescription, XCTest

### Community 122 - "GarbageFirstBoot"
Cohesion: 0.16
Nodes (15): CallContext, uint32_t, WellKnown, doubleIt(), expectErrorWithFullNursery(), fillNursery(), GarbageFirstBoot, cache (+7 more)

### Community 123 - "ClassMethodCache"
Cohesion: 0.18
Nodes (11): ClassMethodCache, entries, forget, insert, kSize, probe, Entry, klass (+3 more)

### Community 124 - "abi.cpp"
Cohesion: 0.09
Nodes (37): AcceptAbi, AoTranscriptFn, ClassDefinitionThenImageDropsSourceText, [Medium] グローバル `Smalltalk` がクラス nil の 57 要素固定配列で、どのメッセージも通らない, 02 インタプリタ・送信・コンテキスト・プロセス, 概要, 03 数値・スカラー系 Kernel ネイティブ, テストの穴 (+29 more)

### Community 125 - "NameCollect"
Cohesion: 0.20
Nodes (10): string, WellKnown, NameCollect, failed, out, rejectCatalog, wk, NamedOop (+2 more)

### Community 126 - "ObjectHeader"
Cohesion: 0.22
Nodes (9): ファイル配置（すべて little-endian）, checkNotPoisoned, uint16_t, ObjectHeader, flags, hash, klass, size (+1 more)

### Community 127 - "ImageFormat"
Cohesion: 0.08
Nodes (26): HeaderRoundTripAndRejects, HeapShapedBitsAreNotImmediates, ImmediateBitsRoundTrip, uint16_t, uint32_t, ImageFormat, decodeNonHeap, encodeNonHeap (+18 more)

### Community 128 - "Bytecode.hpp"
Cohesion: 0.26
Nodes (10): BytecodeIsa, Op, uint8_t, operandBytes(), specialCount(), specialSelector(), specialIndex(), TEST() (+2 more)

### Community 129 - "TEST"
Cohesion: 0.40
Nodes (5): KeepsNativeIdentityEquals, RebindsBagAndEvaluatesInstVar, RefusesNewSelectorOnObject, TEST(), VendorOverwrite

### Community 130 - "Claude レビュー指摘の修正計画（docs/claude-review）"
Cohesion: 0.12
Nodes (16): B0 準備（テスト基盤）, B10 協調スケジューラ（02 High）, B11 App とビルドの残り, B6 イメージとセッションの堅牢化, B8 Kernel の意味論（数値とスカラー）, B9 Kernel の意味論（コレクションとストリーム）, Claude レビュー指摘の修正計画（docs/claude-review）, Context (+8 more)

### Community 131 - "P4-03 Magnitude / SmallInteger / Character"
Cohesion: 0.09
Nodes (22): Boolean, Bootstrap, NativeMethod, Object, P4-01 Object / UndefinedObject / Boolean, send, Behavior, Bootstrap (+14 more)

### Community 132 - "TEST"
Cohesion: 0.20
Nodes (10): AtPutFailureReachesInstallMethod, AtPutFindsInternedKey, AtPutRejectsNonHeapKey, AtPutWithTallyAtSmiMaxFails, GrowKeepsOuterOopAndEntries, NinthMethodWithFullNurseryIsInstalled, MethodImage, defineEightMethodClass() (+2 more)

### Community 133 - "Interpreter.cpp"
Cohesion: 0.29
Nodes (9): clearNonlocal(), consumeNonlocal(), DepthGuard, outermost, hit(), Leave, leave, value (+1 more)

### Community 134 - "Frame"
Cohesion: 0.25
Nodes (8): FieldRoots, frame, Frame, context, isBlock, method, pc, receiver

### Community 135 - "TEST"
Cohesion: 0.10
Nodes (24): EnvEnablesStress, EveryFourthStressCollectionAlsoCompactsOldAndPoisonsTail, FreedNurseryIsPoisonedAndReallocationIsClean, GcStress, GcStressDeathTest, HoleBeforeImmovableKeepsNilClassWhenStressIsOff, InternSameBytesIsIdentical, InternSurvivesNurseryGc (+16 more)

### Community 136 - "指摘"
Cohesion: 0.13
Nodes (14): 00 直近差分（origin/main..HEAD の 6 コミット）, [Critical] ソース未保存メソッドのプレースホルダを Accept すると本体が消える, [Critical] 既存クラスの定義を Accept すると全メソッドが消える, [Critical] 自分を含む Array の `=` でスタックオーバーフロー, [Critical] 非クラス名で ao_accept_method を呼ぶと範囲外書き込み, [High] ClassMethodCache の無効化が定義クラスの分だけ, [High] out == NULL の Do it が副作用ありで AO_ERR を返す, [High] `| q |` の宣言 temp が Workspace 束縛と混ざる (+6 more)

### Community 138 - "Array.cpp"
Cohesion: 0.32
Nodes (15): ao_Array_equals(), ao_Array_printString(), ao_ArrayedCollection_at_(), ao_ArrayedCollection_at_put_(), ao_ArrayedCollection_basicAt_(), ao_ArrayedCollection_basicAt_put_(), ao_ArrayedCollection_new_(), ao_ArrayedCollection_size() (+7 more)

### Community 139 - "Globals.cpp"
Cohesion: 0.36
Nodes (7): at(), atPut(), string_view, uint32_t, WellKnown, install(), nameAt()

### Community 140 - "NativeMethod.cpp"
Cohesion: 0.30
Nodes (14): add(), addNamed(), apply(), CallContext, NativeFn, string_view, uint32_t, WellKnown (+6 more)

### Community 141 - "01 オブジェクトメモリ・GC・ブートストラップ"
Cohesion: 0.18
Nodes (10): 01 オブジェクトメモリ・GC・ブートストラップ, [Critical] スキャベンジ中に `collectOld` が走ると、移動前のヘッダ位置へ書き込む, [High] old 4MB 固定と promote-all のため、生存約 2.5MB で毎回無駄なフル GC を回して実質停止する, [High] old 枯渇でスキャベンジが失敗すると、転送済みオブジェクトがナーサリに残り、同一性が割れる, [High] ナーサリ半面（1MB）を超えるオブジェクトは一切割り当てられず、失敗も見えない, [Low] identity hash が 16 ビットで、SPEC のサイドテーブルがない, [Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない, テストの穴 (+2 more)

### Community 142 - "仕様"
Cohesion: 0.29
Nodes (7): CLI, globals 節, load, OOP エンコード, save, well-known 節, 仕様

### Community 143 - "ao_image_load"
Cohesion: 0.06
Nodes (55): AbiSmoke, BootAndShutdownReturnZero, BootVersionShutdown, [High] Save / Open Image の失敗がユーザーに見えない, [High] Workspace 束縛が 255 temp の上限に達すると eval が全滅, [Medium] file-in のコンパイルエラーが成功扱いになり、vendor の 17 メソッドが黙って欠落している, [High] native block thunk がヒープへ逃げると、保存は成功するがロードできないイメージになる, [High] イメージ保存が非アトミックで、書き込みに失敗すると既存イメージが壊れる (+47 more)

### Community 144 - "Roots.cpp"
Cohesion: 0.15
Nodes (15): walker_, size_t, StackWalker, uint32_t, Roots::add(), Roots::dropHandle(), Roots::enterNextFrameBlock(), Roots::handleAt() (+7 more)

### Community 145 - "RootedArray"
Cohesion: 0.22
Nodes (8): uint32_t, unique_ptr, RootedArray, data_, inline_, kInlineSlots, n_, spill_

### Community 146 - "TEST"
Cohesion: 0.24
Nodes (9): Geometry, PointAccessorsEqualsAndSetters, PointAdd, PointSubtractScaleIntDivideAndPlusNumber, RectangleWidthHeightContainsAndIntersect, int64_t, pt(), rect() (+1 more)

### Community 147 - "applyMethod"
Cohesion: 0.50
Nodes (5): GC 安全性（ネイティブ）の横断評価 — 03 からの照会への回答, テストの穴, Interpreter, run, applyMethod()

### Community 148 - "Format.hpp"
Cohesion: 0.48
Nodes (6): bits(), int64_t, isBytes(), isIndexable(), isPointers(), make()

### Community 149 - "指摘"
Cohesion: 0.22
Nodes (9): [Critical] 再帰の深さに上限がなく、C スタックが溢れてプロセスごと落ちる, [High] Process / Semaphore が実行を切り替えない（fork の本体は実行されず、wait もブロックしない）, [High] 失敗センチネル（空 OOP）がオペランドスタックに「値」として積まれ、エラーが黙って Message に化ける, [High] 非局所リターン中に Kernel の反復ネイティブが止まらない（副作用が続き、戻り値も誤る）, [Low] BlockContext をクロージャとアクティベーションに兼用し、`sender` を上書きしたまま戻さない, [Low] DNU の Message 割り当てに GC リトライがなく、ナーサリ逼迫時に DNU が空 OOP になる, [Low] `valueWithArguments:` が Array 以外のポインタオブジェクトを受け入れ、内部スロットを引数に展開する, [Medium] 死んだホームへの `^`（cannotReturn:）が、評価全体を理由なしに中断する (+1 more)

### Community 150 - "claude-review/README.md"
Cohesion: 0.11
Nodes (15): 06 イメージ・セッション・C ABI・CLI, テストの穴, 概要, 07 Swift アプリ・ビルド・スクリプト, テストの穴, 概要, 1. GC 安全性（メモリ破壊）, 2. 失敗が黙って成功になる (+7 more)

### Community 151 - "RawChunk"
Cohesion: 0.50
Nodes (4): string, RawChunk, span, text

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

### Community 156 - "ActiveGuard"
Cohesion: 0.50
Nodes (3): ActiveGuard, rootShared, saved

### Community 157 - "native_send_test.cpp"
Cohesion: 0.27
Nodes (10): CallContext, uint32_t, stubA(), stubB(), CallContext, uint32_t, pairAfterAlloc(), stubA() (+2 more)

### Community 158 - "Symbol.cpp"
Cohesion: 0.50
Nodes (4): bytes(), string_view, WellKnown, intern()

### Community 159 - "TEST"
Cohesion: 0.50
Nodes (4): BoxFromImageNativeCodeNil, BoxLiteralArrayPseudoObjects, LayoutNativeCodeNil, TEST()

### Community 160 - "TEST"
Cohesion: 0.67
Nodes (3): LoadOrderEvaluatesLinkRoundTrip, TEST(), VendorFileIn

### Community 161 - "TEST"
Cohesion: 0.67
Nodes (3): TEST(), CompilerSmoke, VersionIsNonEmpty

## Knowledge Gaps
- **754 isolated node(s):** `.metaFlag`, `.title`, `.sourceText`, `.errorText`, `.paneAccessibilityLabels` (+749 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1419 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **15 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Oop` connect `Oop` to `WellKnown`, `HostTempGuard`, `Dictionary.cpp`, `LargeInteger.cpp`, `image_save_load_test.cpp`, `Interpreter.cpp`, `Frame`, `TEST`, `Stream.cpp`, `Float.cpp`, `Array.cpp`, `Globals.cpp`, `Heap.cpp`, `NativeMethod.cpp`, `putNative`, `VendorExtract.cpp`, `Roots.cpp`, `RootedArray`, `指摘`, `Heap`, `Format.hpp`, `Roots`, `Geometry.cpp`, `applyMethod`, `SmallInteger.cpp`, `Gc`, `TEST`, `TEST`, `ActiveGuard`, `vector`, `Session.cpp`, `Symbol.cpp`, `TEST`, `native_send_test.cpp`, `TEST`, `gc_safety_test.cpp`, `TEST`, `TEST`, `CompileError`, `ImageSave.cpp`, `CallContext`, `TEST`, `.fromSmallInteger`, `TEST`, `BlockContext.cpp`, `TEST`, `send`, `String.cpp`, `Interpreter::run`, `DiskHeader`, `WellKnown.cpp`, `TEST`, `.fromBits`, `Boot`, `TEST`, `string`, `.isHeap`, `.false_`, `Ao`, `allocateRetry`, `README.md`, `Character.cpp`, `Bootstrap.cpp`, `Claude レビュー指摘の修正計画（docs/claude-review）`, `TEST`, `native_method_test.cpp`, `ImageLoad.cpp`, `performSend`, `ao_Magnitude_lessOrEqual`, `Boolean.cpp`, `imageRegistryStubA`, `GarbageFirstBoot`, `ClassMethodCache`, `abi.cpp`, `NameCollect`, `ObjectHeader`?**
  _High betweenness centrality (0.404) - this node is a cross-community bridge._
- **Why does `WellKnown` connect `WellKnown` to `WellKnown.cpp`, `Heap`, `Roots`, `Oop`?**
  _High betweenness centrality (0.089) - this node is a cross-community bridge._
- **Why does `CompileError` connect `CompileError` to `TEST`, `image_save_load_test.cpp`, `TEST`, `Emitter`, `Literal`, `TEST`, `TEST`, `ChunkParser.cpp`, `TEST`, `TEST`, `Parser`, `TEST`, `Chunk`, `TEST`, `Codegen.cpp`, `Boot`, `string`, `TEST`, `GarbageFirstBoot`, `abi.cpp`?**
  _High betweenness centrality (0.075) - this node is a cross-community bridge._
- **Are the 6 inferred relationships involving `Oop` (e.g. with `現状` and `Status`) actually correct?**
  _`Oop` has 6 INFERRED edges - model-reasoned connections that need verification._
- **Are the 4 inferred relationships involving `Roots` (e.g. with `01 オブジェクトメモリ・GC・ブートストラップ` and `[Low] `Roots` が同じスロットの二重登録を許し、`collectOld` の更新は冪等でない`) actually correct?**
  _`Roots` has 4 INFERRED edges - model-reasoned connections that need verification._
- **Are the 28 inferred relationships involving `TEST()` (e.g. with `RootedArray` and `allocate`) actually correct?**
  _`TEST()` has 28 INFERRED edges - model-reasoned connections that need verification._
- **What connects `.metaFlag`, `.title`, `.sourceText` to the rest of the system?**
  _754 weakly-connected nodes found - possible documentation gaps or missing edges._
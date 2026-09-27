# P12 追補 クラス ID と壊れたメソッド辞書 — 実装計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Browser の ABI がクラスを名前でなくセッションのクラス ID で指すようにし（別名だけが持つ旧クラスと同じ名前の新しいクラスを取り違えない）、壊れたメソッド辞書への送信で落ちないようにする。

**Architecture:** `Session` に ID の表（`classIds`。ID → クラス、枠は GC が更新するルート）を持たせ、`classRows` が一覧を作るたびに消えたクラスの項目を外し、新しいクラスに単調な ID を付ける。読み出し、`ao_accept_method_id`、`ao_remove_method`、`ao_remove_class` はどれも `classRows` から ID で行を引く。表の枠はソース表と同じく、イメージの保存と `liveClasses` がたどらない。Compile.cpp の Accept と削除はクラスの Oop を受ける形（`acceptMethodInto`、`removeMethodOf`、`removeClassOf`）にし、Swift は行と選択を ID で持つ。`MethodDictionary::pairArray` を 1 か所の形の検査にし、`at` / `atPut` / `removeKey` / `Behavior>>selectors` がそれを通る。

**Tech Stack:** C++20（runtime）、GoogleTest、Swift 6 AppKit（app）、XCTest。

**Spec:** `SPEC.md` §3.3（メソッド辞書の枠）、§3.9「System Browser」「クラス定義の再 Accept」冒頭、§3.9「削除」、§3.10「C ABI」「クラス ID」「ソースはイメージに書かない」、§4.1、§4.3、§6「Browser の削除」。設計の理由: `docs/superpowers/specs/2026-09-26-browser-remove-design.md` の「追補: クラス ID（2026-09-27）」。

**決定ファイルからの変更（planner）:**

- 一覧を 1 クラス 1 行にした（決定ファイルに無い）。`WellKnown::eachClass` は `Smalltalk` の束縛ごとにクラスを渡すので、別名があると同じクラスが 2 行（同じ名前、同じ ID）になり、Swift が選択を ID で保てない。同じ名前の行は ID の小さい順に並べる（安定しないソートの解消）。
- 表の刈り込みは「一覧を作り直すとき」だが、ID を取るどの関数も `classRows` を通るので、実際には呼び出しのたびに刈り込む。そのため「ID の表だけが持つサブクラスがスーパークラスの削除を妨げない」は、ID による削除の経路では刈り込みが先に効いて自然に満たされる。表の除外（`liveClasses`）が本当に効くのは `ao_accept_class` の形の変更（`hasSubclass`）とイメージの保存である。テストはその両方で確かめる（Task 2）。
- `ao_browser_class_at` は 1 回の呼び出しで行を返す形（`int64_t* class_id` を 2 番目の引数に足す）にした。ID の出力はどれも NULL を許す（C++ のテストの呼び出し元を小さく保つ）。
- `MethodDictionary` の修正は `at` に加えて `atPut`（Accept が壊れた辞書で範囲の外を読む）と `ao_Behavior_selectors`（同じく `slotAt(dict, kDictSlotArray)` を検査なしで読む）にも入れた。どれも同じ 1 行の検査で、落ちるのは同じ原因だからである。
- `ao_accept_method_id` の未知の ID は `AO_ERR` とメッセージ `unknown class id`（`ao_accept_method` の「クラスでない名前」はメッセージ無しの `AO_ERR` のまま）。busy の `ao_accept_method_id` は `ao_accept_method` と同じくメッセージ無しの `AO_ERR`。
- 名前スロットが文字列でないクラスの行（`<name>` が空）は、`is not bound to this class` で拒む（`class removal refused:  is not bound to this class`。空白が 2 つ）。利用者の決定「自分の名前に束縛されていない行は拒む」の当てはめである。既存の `RemoveClassWithSubclassIsRefused` は、名前を消す前に削除するよう順序を入れ替える（Task 4）。

## Global Constraints

- Kernel メソッドは C++ の NativeMethod のまま。Smalltalk 側に ID や削除のセレクタを足さない。依存を足さない。
- 削除（`ao_remove_method` / `ao_remove_class`）はヒープに割り当てない。ID の解決（`sessionClassForId`）も C++ のメモリだけを使う。
- 拒んだとき、メソッド辞書、グローバル辞書、キャッシュ、ソース表は変わらない（ID の表は一覧を作り直すので変わりうる）。
- ID は正の `int64_t`、0 は「クラスなし」。OS のプロセスの中で使い回さない（`nextClassId` は `static` の単調な数。`Scheduler.cpp` の `nextProcessId` と同じ）。
- 未知の ID: 件数の関数は -1、ほかの読み出しは `AO_ERR`、`ao_accept_method_id` / `ao_remove_method` / `ao_remove_class` は `AO_ERR` とメッセージ `unknown class id`。
- 削除のメッセージの `<name>` / `<Class>` は ID のクラスの名前スロット（`meta` 1 なら ` class` を付ける）。新しい拒否は `class removal refused: <name> is not bound to this class`。
- busy の入口は 21 個（`ao_accept_method_id` を足す）。`ao_browser_*`（`ao_browser_class_id` を含む）は busy でも呼べる。`ao_abi.h` の一覧と数（twenty → twenty-one）を直す。
- 表の枠はルート（`Roots::add`）。イメージの保存（`sessionImageSave` の `HideMethodSources`）と `liveClasses` は表の枠をたどらない（`classIdRootSlots()`）。`~Session` は表の枠を外す。
- CLAUDE.md の手順: 各タスクの前に `graphify-out/GRAPH_REPORT.md` と Serena の `find_symbol` / `find_referencing_symbols` で定義と参照を取る。本体の書き換えは `replace_symbol_body` / `insert_after_symbol` / `insert_before_symbol`。コミット本文に `Graphify:` と `Serena:` の行。worktree で動くエージェントは Serena を使わない（Serena の編集は本体の作業木に当たる。そのときは Edit で直し、コミットの Serena 行に `none (worktree)` と書く）。C++ を Serena の外で大きく変えたら、Serena の clangd の子プロセスを止めて作り直させる。
- コミットは英語の命令形。末尾に `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`。push しない（PR #17 のブランチ `feat/p12-browser-remove` にローカルでコミットする）。
- 頼まれていないファイルを足さない。隣のリファクタをしない（`findClass` / `superclassName` / `subclassNames` / `removeMethodNamed` / `removeClassNamed` は置き換えで消す。ほかの呼び出し元は無い）。

## Review Focus

1. Workspace で選択中のクラスの形を変える（新しいクラスが同じ名前に付く）→ Browser の次の読み直しで、同じ名前の新しいクラスが選ばれる（Task 5 `testClassRowsCarryOneIdPerClass` の形の変更の段）。
2. 名前スロットが文字列でないクラスの行の Remove Class… → 落ちず、`class removal refused:  is not bound to this class` で拒み、何も変えない（Task 4 `RemoveClassKeepsAliases` の最後の段）。
3. 評価中のフック（busy）からの Browser の読み出し → ID の発行と刈り込みが走っても答える。`ao_accept_method_id` は拒む（Task 4 `RemoveWhileBusyIsRefused`）。
4. 同じ名前の 2 行が同じカテゴリにある → 行は ID の小さい順で、それぞれの ID が自分のクラスを読む（Task 3 `SameNamedClassesAreReadApartById`）。
5. イメージのロードの前に取った ID を使う → どの呼び出しも未知として拒み、Browser は名前で選び直す（Task 3 `ClassIdsDoNotSurviveBootOrImageLoad`、Task 5 `testImageLoadReselectsTheClassByName`）。

---

## File Structure

| ファイル | 責務 |
|---|---|
| `runtime/include/ao/MethodDictionary.hpp`, `runtime/src/MethodDictionary.cpp` | `pairArray`（形の検査）、`at` / `atPut` / `removeKey` がそれを通る |
| `runtime/src/kernel/Behavior.cpp` | `ao_Behavior_selectors` が `pairArray` を通る |
| `runtime/src/Session.hpp`, `runtime/src/Session.cpp` | `Session::classIds`、`classRows` の ID と 1 クラス 1 行、`browserClassId`、`classIdRootSlots`、`sessionClassForId`、ID で引く読み出し |
| `runtime/src/Compile.cpp`, `runtime/include/ao/Compile.hpp` | `liveClasses` の除外、`isBehavior`、`acceptMethodInto`、`removeMethodOf`、`removeClassOf` |
| `bridge/ao_abi.h`, `runtime/src/abi.cpp` | `ao_browser_class_id`、ID を取る読み出し、`ao_accept_method_id`、ID を取る削除、busy の一覧 |
| `runtime/tests/method_dictionary_test.cpp` | 壊れた辞書の `at` / `atPut` |
| `runtime/tests/browser_abi_test.cpp` | クラス ID の読み出し |
| `runtime/tests/remove_abi_test.cpp` | ID による削除、別名の筋書き、壊れた辞書への送信 |
| `runtime/tests/accept_abi_test.cpp`, `runtime/tests/hashed_collection_test.cpp` | ID の読み出しへの移行、`ao_accept_method_id`、形の変更と ID の表 |
| `runtime/tests/session_abi_test.cpp` | イメージの保存が ID の表をたどらない |
| `app/Ao/BrowserModel.swift` | `BrowserClass`、行と選択を ID で持つ、名前での選び直し |
| `app/Ao/BrowserWindow.swift` | 選択、階層、削除、Accept を ID で行う |
| `app/AoTests/AcceptTests.swift`, `app/AoTests/BrowserModelTests.swift` | 別名の筋書き、ロード後の選び直し、1 クラス 1 行 |
| `CHANGELOG.md`, `SPEC.md` §6, `docs/phases/P12.md`, `docs/README.md` | 記録 |

## ビルドとテストのコマンド

```sh
cmake --build build
ctest --test-dir build --output-on-failure -R 'MethodDictionary|BrowserAbi|RemoveAbi|RemoveUnits|AcceptAbi|SessionAbi'
ctest --test-dir build --output-on-failure            # 全部
ctest --test-dir build --output-on-failure -R gcstress
./build/ao --test image/tests
swift test --package-path app \
  -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" \
  -Xlinker -lc++ --filter 'AcceptTests|BrowserModelTests'
```

`swift test` の `-force_load` のパスは絶対パスにする（`$PWD` はリポジトリのルート）。`build/` が無ければ `cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`。

---

### Task 1: 壊れたメソッド辞書を読まない

**Files:**
- Modify: `runtime/include/ao/MethodDictionary.hpp`（`at` の宣言の前）、`runtime/src/MethodDictionary.cpp`（`at`、`atPut`、`removeKey`、新しい `pairArray`）
- Modify: `runtime/src/kernel/Behavior.cpp`（`ao_Behavior_selectors`）
- Test: `runtime/tests/method_dictionary_test.cpp`、`runtime/tests/remove_abi_test.cpp`

**Interfaces:**
- Produces: `Oop ao::MethodDictionary::pairArray(const Heap& heap, Oop dict)` — 辞書の形なら配列、そうでなければ空の Oop。

- [ ] **Step 1: Serena で定義と参照を取る**

`find_symbol` で `ao/MethodDictionary/at`、`ao/MethodDictionary/atPut`、`ao/MethodDictionary/removeKey`、`ao/ao_Behavior_selectors`。`find_referencing_symbols` で `ao/MethodDictionary/at`（`lookup`、`acceptMethodSource`、`applyMethodsFor`、`putNative`、`ao_Behavior_compiledMethodAt_`、`ao_Behavior_includesSelector_` が呼ぶ。どれも `at` の中の検査で守られる）。

- [ ] **Step 2: 失敗するテストを書く（単体）**

`runtime/tests/method_dictionary_test.cpp` の末尾に足す:

```cpp
// SPEC §3.3: a slot rewritten with instVarAt:put: is no dictionary. pairArray answers nothing, at
// answers nil and atPut takes nothing, without reading out of range: an empty object, a one-slot
// object, a byte object, a two-slot object whose array is bytes, nil and an immediate.
TEST(MethodDictionary, MalformedDictionaryAnswersNilAndTakesNothing) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  const ao::Oop key = ao::Symbol::intern(wk, "mdMalformed");
  const ao::Oop value = ao::Oop::fromSmallInteger(5);
  const ao::Oop empty = heap.allocateNoGc(ao::Oop::nil(), 0, 0);
  const ao::Oop oneSlot = heap.allocateNoGc(ao::Oop::nil(), 1, 0);
  const ao::Oop bytes = heap.allocateNoGc(ao::Oop::nil(), 8, ao::kFlagBytes);
  const ao::Oop bytesInner = heap.allocateNoGc(ao::Oop::nil(), 8, ao::kFlagBytes);
  const ao::Oop bytesArrayDict = heap.allocateNoGc(wk.methodDictionaryClass, 2, 0);
  ASSERT_TRUE(empty.isHeap());
  ASSERT_TRUE(oneSlot.isHeap());
  ASSERT_TRUE(bytes.isHeap());
  ASSERT_TRUE(bytesInner.isHeap());
  ASSERT_TRUE(bytesArrayDict.isHeap());
  heap.slotAtPut(oneSlot, ao::kDictSlotTally, ao::Oop::fromSmallInteger(0));
  heap.slotAtPut(bytesArrayDict, ao::kDictSlotTally, ao::Oop::fromSmallInteger(0));
  heap.slotAtPut(bytesArrayDict, ao::kDictSlotArray, bytesInner);
  for (const ao::Oop bad : {empty, oneSlot, bytes, bytesArrayDict, ao::Oop::nil(),
                            ao::Oop::fromSmallInteger(3)}) {
    EXPECT_TRUE(ao::MethodDictionary::pairArray(heap, bad).isEmpty());
    EXPECT_TRUE(ao::MethodDictionary::at(heap, bad, key).isNil());
    EXPECT_FALSE(ao::MethodDictionary::atPut(heap, bad, key, value));
  }
  EXPECT_EQ(0, heap.slotAt(oneSlot, ao::kDictSlotTally).smallIntegerValue());
  EXPECT_EQ(0, heap.slotAt(bytesArrayDict, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop dict = ao::MethodDictionary::create(heap, wk, 2);
  ASSERT_TRUE(dict.isHeap());
  EXPECT_EQ(heap.slotAt(dict, ao::kDictSlotArray), ao::MethodDictionary::pairArray(heap, dict));
}
```

- [ ] **Step 3: 失敗するテストを書く（送信）**

`runtime/tests/remove_abi_test.cpp` の末尾（`RemoveClassNamesTheSmallestDescendant` のあと）に足す:

```cpp
// SPEC §3.3: a method dictionary slot rewritten with instVarAt:put: (kClassSlotMethodDict is slot
// 1, index 2) holds no method. A send skips it to the superclass or doesNotUnderstand:, the
// reflective reads answer nothing, and Accept fails with "install failed"; nothing reads out of
// range. Each probe sends a selector not sent before, so no cached lookup answers for it.
TEST_F(RemoveAbi, MalformedMethodDictionaryFallsBackOnSend) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Mal", "Object", "B12-Test"));
  for (int i = 0; i < 4; ++i) {
    const std::string own = "own" + std::to_string(i) + "\n  ^" + std::to_string(i) + "\n";
    ASSERT_EQ(AO_OK, ao_accept_method("B12Mal", 0, own.c_str(), &err)) << err.message;
    const std::string up = "b12up" + std::to_string(i) + "\n  ^5\n";
    ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, up.c_str(), &err)) << err.message;
  }
  ASSERT_EQ(AO_OK, ao_accept_method("B12Mal", 0, "kept\n  ^7\n", &err)) << err.message;
  const std::string saved = printIt("b12malDict := B12Mal instVarAt: 2");
  ASSERT_NE(0u, saved.find('<')) << saved;
  const std::string arr = printIt("b12malArr := Array new: 2. b12malArr at: 2 put: 'xy'");
  ASSERT_NE(0u, arr.find('<')) << arr;
  const char* bads[] = {"Array new: 0", "Array new: 1", "'abc'", "b12malArr"};
  for (int i = 0; i < 4; ++i) {
    SCOPED_TRACE(bads[i]);
    const std::string n = std::to_string(i);
    const std::string put = std::string("B12Mal instVarAt: 2 put: (") + bads[i] + ")";
    ASSERT_NE(0u, printIt(put.c_str()).find('<'));
    expectDnu(("B12Mal new own" + n).c_str(), ("own" + n).c_str());
    EXPECT_EQ("5", printIt(("B12Mal new b12up" + n).c_str()));
    EXPECT_EQ("false", printIt(("B12Mal includesSelector: #own" + n).c_str()));
    EXPECT_EQ("nil", printIt(("B12Mal compiledMethodAt: #own" + n).c_str()));
    EXPECT_EQ("0", printIt("B12Mal selectors size"));
    const std::string extra = "extra" + n + "\n  ^9\n";
    EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method("B12Mal", 0, extra.c_str(), &err));
    EXPECT_STREQ("install failed", err.message);
  }
  ASSERT_NE(0u, printIt("B12Mal instVarAt: 2 put: b12malDict").find('<'));
  EXPECT_EQ("7", printIt("B12Mal new kept"));
}
```

- [ ] **Step 4: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（`pairArray` が宣言されていない）。宣言だけ足して回すと、`MalformedDictionaryAnswersNilAndTakesNothing` と `MalformedMethodDictionaryFallsBackOnSend` は `Heap::slotAt` の assert（`Heap.cpp:161`）で落ちる。

- [ ] **Step 5: `pairArray` を足し、`at` / `atPut` / `removeKey` を通す**

`runtime/include/ao/MethodDictionary.hpp` の `Oop at(...)` の宣言の前に足す（`insert_before_symbol`）:

```cpp
// SPEC §3.3: dict's pair array when dict has a method dictionary's shape (a pointer object with
// the array slot, the array a pointer object); the empty Oop otherwise (nil, an immediate, or a
// slot rewritten with instVarAt:put:). Reads nothing out of range. at, atPut and removeKey read a
// dictionary only through it, so a malformed one holds no selector and takes none.
Oop pairArray(const Heap& heap, Oop dict);
```

`runtime/src/MethodDictionary.cpp` の `at` の前に足し、`at`、`atPut`、`removeKey` の本体を置き換える（`replace_symbol_body`）:

```cpp
Oop pairArray(const Heap& heap, Oop dict) {
  if (!dict.isHeap() || (heap.flags(dict) & kFlagBytes) != 0 ||
      heap.size(dict) <= kDictSlotArray) {
    return Oop{};
  }
  const Oop inner = heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap() || (heap.flags(inner) & kFlagBytes) != 0) {
    return Oop{};
  }
  return inner;
}

Oop at(const Heap& heap, Oop dict, Oop key) {
  const Oop inner = pairArray(heap, dict);
  if (inner.isEmpty()) {
    return Oop::nil();
  }
  const auto n = heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i) == key) {
      return heap.slotAt(inner, i + 1);
    }
  }
  return Oop::nil();
}
```

```cpp
bool atPut(Heap& heap, Oop dict, Oop key, Oop value) {
  // キーはヒープの Symbol。空 Oop（intern の失敗）と nil（空きスロットの印）と即値は登録しない。
  // 辞書の形でなければ（instVarAt:put: で書き換えた枠。SPEC §3.3）登録しない。
  if (!key.isHeap()) {
    return false;
  }
  auto inner = pairArray(heap, dict);
  if (inner.isEmpty()) {
    return false;
  }
  auto n = heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i) == key) {
      heap.slotAtPut(inner, i + 1, value);
      return true;
    }
  }
  const auto tallyOop = heap.slotAt(dict, kDictSlotTally);
  const auto tally = tallyOop.isSmallInteger() ? tallyOop.smallIntegerValue() : 0;
  // tally は Smalltalk から書き換えられる（クラスの instVarAt: で辞書に届く）。+1 が SmallInteger
  // を超えるなら、登録せずに失敗を返す。
  if (tally >= kSmiMax) {
    return false;
  }
  if (tally * 2 == static_cast<std::int64_t>(n)) {
    if (!growInner(heap, dict, inner)) {
      return false;
    }
    n = heap.size(inner);
  }
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i).isNil()) {
      heap.slotAtPut(inner, i, key);
      heap.slotAtPut(inner, i + 1, value);
      heap.slotAtPut(dict, kDictSlotTally, Oop::fromSmallInteger(tally + 1));
      return true;
    }
  }
  return false;
}
```

```cpp
bool removeKey(Heap& heap, Oop dict, Oop key) {
  // 辞書の枠は instVarAt:put: で何でも入る。形が違えば（pairArray）範囲の外を読まずに false を返す。
  if (!key.isHeap()) {
    return false;
  }
  const Oop inner = pairArray(heap, dict);
  if (inner.isEmpty()) {
    return false;
  }
  const auto n = heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(inner, i) != key) {
      continue;
    }
    heap.slotAtPut(inner, i, Oop::nil());
    heap.slotAtPut(inner, i + 1, Oop::nil());
    // tally は Smalltalk から書き換えられる（atPut と同じ）。0 未満にはしない。
    const Oop tallyOop = heap.slotAt(dict, kDictSlotTally);
    const auto tally = tallyOop.isSmallInteger() ? tallyOop.smallIntegerValue() : 0;
    heap.slotAtPut(dict, kDictSlotTally, Oop::fromSmallInteger(tally > 0 ? tally - 1 : 0));
    return true;
  }
  return false;
}
```

`growInner` は `atPut` より前にあるので、`pairArray` と `at` は `growInner` の前に置く（今の `at` の位置）。

- [ ] **Step 6: `ao_Behavior_selectors` を通す**

`runtime/src/kernel/Behavior.cpp` の `ao_Behavior_selectors` を置き換える（`replace_symbol_body`）:

```cpp
Oop ao_Behavior_selectors(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !receiver.isHeap()) return Oop{};
  // SPEC §3.3: a slot that is no method dictionary (instVarAt:put:) holds no selector.
  const Oop pairs =
      MethodDictionary::pairArray(ctx.heap, ctx.heap.slotAt(receiver, kClassSlotMethodDict));
  if (pairs.isEmpty()) {
    return allocateRetry(ctx, ctx.wk.arrayClass, 0, 0);
  }
  Root inner(ctx.roots, pairs);
  const auto n = ctx.heap.size(inner.slot);
  std::uint32_t count = 0;
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (!ctx.heap.slotAt(inner.slot, i).isNil()) {
      ++count;
    }
  }
  Root arr(ctx.roots, allocateRetry(ctx, ctx.wk.arrayClass, count, 0));
  if (!arr.slot.isHeap()) {
    return Oop{};
  }
  std::uint32_t j = 0;
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop key = ctx.heap.slotAt(inner.slot, i);
    if (!key.isNil()) {
      ctx.heap.slotAtPut(arr.slot, j++, key);
    }
  }
  return arr.slot;
}
```

- [ ] **Step 7: 通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'MethodDictionary|RemoveAbi|AcceptAbi|KernelScan'`
Expected: PASS（`RemoveMethodFromMalformedDictionaryIsRefused` も緑のまま）。

- [ ] **Step 8: Commit**

```bash
git add runtime/include/ao/MethodDictionary.hpp runtime/src/MethodDictionary.cpp runtime/src/kernel/Behavior.cpp runtime/tests/method_dictionary_test.cpp runtime/tests/remove_abi_test.cpp
git commit -m "Treat a malformed method dictionary as holding no method

SPEC §3.3: a methodDict slot rewritten with instVarAt:put: sent at, atPut and
Behavior>>selectors out of range. MethodDictionary::pairArray is the one shape check;
a send now falls to the superclass or doesNotUnderstand:, and Accept fails with
install failed.

Graphify: explain lookup(); query MethodDictionary
Serena: replace_symbol_body ao::MethodDictionary::at, atPut, removeKey, ao_Behavior_selectors

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 2: クラス ID の表、`ao_browser_class_id`、行の ID

**Files:**
- Modify: `runtime/src/Session.hpp`（`Session` の `heldDoIts` のあと、`browserClassAt` の宣言、新しい宣言）
- Modify: `runtime/src/Session.cpp`（`ClassRow`、`classRows`、`releaseClassIds`、`~Session`、`sessionImageSave` の `HideMethodSources`、`browserClassAt`、新しい `browserClassId` / `classIdRootSlots`）
- Modify: `runtime/src/Compile.cpp`（`liveClasses`）
- Modify: `bridge/ao_abi.h`、`runtime/src/abi.cpp`（`ao_browser_class_id`、`ao_browser_class_at`）
- Modify: `app/Ao/BrowserModel.swift`（`copyClass` の呼び出しだけ）
- Test: `runtime/tests/browser_abi_test.cpp`、`runtime/tests/accept_abi_test.cpp`、`runtime/tests/session_abi_test.cpp`、ほか `ao_browser_class_at` を呼ぶテスト

**Interfaces:**
- Consumes: なし
- Produces:
  - `struct ao::Session::ClassId { std::int64_t id; Oop cls; }`、`std::vector<std::unique_ptr<ClassId>> ao::Session::classIds`
  - `std::int64_t ao::browserClassId(const char* name)`
  - `std::vector<const Oop*> ao::classIdRootSlots()`
  - `int ao::browserClassAt(int index, std::int64_t* classId, char* name, int nameLen, char* category, int categoryLen)`
  - C: `int64_t ao_browser_class_id(const char* name)`、`int ao_browser_class_at(int index, int64_t* class_id, char* name, int name_len, char* category, int category_len)`
  - Session.cpp の無名名前空間: `ClassRow::id`、`classRows(Session&)`（1 クラス 1 行、名前 → ID の順、刈り込みと発行つき）

- [ ] **Step 1: Graphify と Serena**

`graphify explain "classRows()"`（呼び出し元は `browser*` の 11 関数）、`graphify path "liveClasses()" "methodSourceRootSlots()"`。Serena: `find_symbol` `ao/(anonymous namespace)/classRows`、`ao/Session`（depth 1）、`ao/sessionImageSave`、`ao/(anonymous namespace)/liveClasses`、`ao/(anonymous namespace)/releaseMethodSources`。`find_referencing_symbols` `ao/methodSourceRootSlots`（`sessionImageSave`、`liveClasses`、テスト）。

- [ ] **Step 2: 失敗するテストを書く**

`runtime/tests/browser_abi_test.cpp` の include に `#include "../src/Session.hpp"` と `#include <cstdint>` を足し、`class BrowserAbi` の前に足す:

```cpp
namespace {

int doIt(const char* source) {
  char out[64];
  AoSpan err{};
  return ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_DOIT, out, 64, &err);
}

int defineRow(const char* name, const char* category = "B12-Id", const char* super = "Object") {
  const std::string def = std::string(super) + " subclass: #" + name +
                          "\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n  category: '" + category + "'\n";
  AoSpan err{};
  return ao_accept_class(def.c_str(), &err);
}

// Whether a class-list row carries id.
bool listsId(std::int64_t id) {
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    std::int64_t rowId = 0;
    char name[256];
    char category[256];
    if (ao_browser_class_at(i, &rowId, name, 256, category, 256) == AO_OK && rowId == id) {
      return true;
    }
  }
  return false;
}

// Whether the session's class ID table holds id.
bool sessionHoldsId(std::int64_t id) {
  const ao::Session* s = ao::session();
  return s != nullptr && std::any_of(s->classIds.begin(), s->classIds.end(),
                                     [&](const auto& entry) { return entry->id == id; });
}

}  // namespace
```

末尾に足す:

```cpp
// SPEC §3.10 クラス ID: a listed class has one positive ID, the same from every read and for every
// name Smalltalk binds to it; an alias adds no row. A name that binds no listed class is 0.
TEST_F(BrowserAbi, ClassIdIsStableAndOneRowPerClass) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Row", "B12-Row"));
  const int before = ao_browser_class_count();
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12RowAlias put: B12Row"));
  EXPECT_EQ(before, ao_browser_class_count());
  const std::int64_t id = ao_browser_class_id("B12Row");
  EXPECT_GT(id, 0);
  EXPECT_EQ(id, ao_browser_class_id("B12Row"));
  EXPECT_EQ(id, ao_browser_class_id("B12RowAlias"));
  const int n = ao_browser_class_count();
  std::vector<std::int64_t> ids;
  int rows = 0;
  char name[128];
  char category[128];
  for (int i = 0; i < n; ++i) {
    std::int64_t rowId = -7;
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, &rowId, name, 128, category, 128));
    EXPECT_GT(rowId, 0);
    ids.push_back(rowId);
    if (rowId == id) {
      ++rows;
      EXPECT_STREQ("B12Row", name);
      EXPECT_STREQ("B12-Row", category);
    }
    if (std::strcmp(name, "Object") == 0) {
      EXPECT_EQ(ao_browser_class_id("Object"), rowId);
    }
  }
  EXPECT_EQ(1, rows);
  std::sort(ids.begin(), ids.end());
  EXPECT_EQ(ids.end(), std::adjacent_find(ids.begin(), ids.end()));
  // The ID out-parameter may be NULL; a failed row writes 0.
  EXPECT_EQ(AO_OK, ao_browser_class_at(0, nullptr, name, 128, category, 128));
  std::int64_t none = -7;
  EXPECT_EQ(AO_ERR, ao_browser_class_at(n, &none, name, 128, category, 128));
  EXPECT_EQ(0, none);
  for (const char* notListed : {"B12NoSuchClass", "", "Processor", "Smalltalk", "nil"}) {
    EXPECT_EQ(0, ao_browser_class_id(notListed)) << notListed;
  }
  EXPECT_EQ(0, ao_browser_class_id(nullptr));
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12RowMeta put: B12Row class"));
  EXPECT_EQ(0, ao_browser_class_id("B12RowMeta"));
}

// SPEC §3.10 クラス ID: a class no name binds any more leaves the list, and the table drops its
// ID. The same name defined again is a new class with a new, larger ID; IDs are never reused.
TEST_F(BrowserAbi, ClassIdsAreDroppedWithTheirClassAndNeverReused) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Drop"));
  const std::int64_t first = ao_browser_class_id("B12Drop");
  ASSERT_GT(first, 0);
  EXPECT_TRUE(sessionHoldsId(first));
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12Drop put: nil"));
  EXPECT_EQ(0, ao_browser_class_id("B12Drop"));
  EXPECT_FALSE(listsId(first));
  EXPECT_FALSE(sessionHoldsId(first));
  ASSERT_EQ(AO_OK, defineRow("B12Drop"));
  const std::int64_t second = ao_browser_class_id("B12Drop");
  EXPECT_GT(second, first);
  EXPECT_TRUE(listsId(second));
  EXPECT_FALSE(listsId(first));
}
```

`runtime/tests/accept_abi_test.cpp` の include に `#include <algorithm>` と `#include <cstdint>` を足し、末尾に足す:

```cpp
// SPEC §3.9 クラス定義の再 Accept, §3.10 クラス ID: a subclass only the class ID table still holds
// (unbound after the Browser listed it) is not alive, so the superclass's shape can change.
TEST(AcceptAbi, ClassIdTableDoesNotKeepSubclassesAlive) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B12IdPar", "a", "B12-Id").c_str(), &err))
      << err.message;
  ASSERT_EQ(AO_OK,
            ao_accept_class(b5Definition("B12IdPar", "B12IdKid", "", "B12-Id").c_str(), &err))
      << err.message;
  const std::int64_t kid = ao_browser_class_id("B12IdKid");
  ASSERT_GT(kid, 0);
  const char* unbind = "Smalltalk at: #B12IdKid put: nil";
  ASSERT_EQ(AO_OK, ao_eval(unbind, static_cast<int>(std::strlen(unbind)), AO_EVAL_DOIT, out, 64,
                           &err))
      << err.message;
  // No Browser read since: the table still holds the unbound kid.
  const ao::Session& s = *ao::session();
  ASSERT_TRUE(std::any_of(s.classIds.begin(), s.classIds.end(),
                          [&](const auto& entry) { return entry->id == kid; }));
  EXPECT_EQ(AO_OK,
            ao_accept_class(b5Definition("Object", "B12IdPar", "a b", "B12-Id").c_str(), &err))
      << err.message;
  ao_runtime_shutdown();
}
```

`runtime/tests/session_abi_test.cpp` の `ImageSaveDoesNotTraceSnapshotOrBlockSlots` のあとに足す:

```cpp
// SPEC §3.10 クラス ID: the image does not hold what only the class ID table reaches (a class
// unbound after the Browser listed it; its category string is the marker), and the table's slots
// are roots again after the save.
TEST_F(SessionAbi, ImageSaveDoesNotTraceClassIdSlots) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("Object subclass: #P12IdOnly\n  instanceVariableNames: ''\n"
                                   "  classVariableNames: ''\n  poolDictionaries: ''\n"
                                   "  category: 'P12CatOnlyMarker'\n",
                                   &err))
      << err.message;
  const std::int64_t id = ao_browser_class_id("P12IdOnly");
  ASSERT_GT(id, 0);
  ASSERT_EQ(AO_OK, evalDoIt("Smalltalk at: #P12IdOnly put: nil"));
  ao::Session& s = *ao::session();
  // No Browser read since: the table still holds the unbound class.
  const auto held = std::find_if(s.classIds.begin(), s.classIds.end(),
                                 [&](const auto& entry) { return entry->id == id; });
  ASSERT_NE(s.classIds.end(), held);
  EXPECT_TRUE(p10SlotListed(ao::classIdRootSlots(), &(*held)->cls));
  const ao::Roots::Counts rootsBefore = s.roots.counts();
  const char* path = "session-abi-p12-ids.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  EXPECT_EQ(rootsBefore.slots, s.roots.counts().slots);
  std::ifstream in(path, std::ios::binary);
  const std::string bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  in.close();
  std::remove(path);
  EXPECT_EQ(std::string::npos, bytes.find("P12CatOnlyMarker"));
  // Rooted again: a collection keeps the class and its name.
  {
    ao::Gc gc(s.heap, s.roots);
    gc.collectNursery();
    gc.collectOld();
  }
  const ao::Oop name = s.heap.slotAt((*held)->cls, ao::kClassSlotName);
  ASSERT_TRUE(name.isHeap());
  EXPECT_EQ("P12IdOnly", std::string(reinterpret_cast<const char*>(s.heap.bytes(name)),
                                     s.heap.size(name)));
}
```

`session_abi_test.cpp` に `ao/Bootstrap.hpp` が無ければ include する（`kClassSlotName`）。

- [ ] **Step 3: `ao_browser_class_at` の呼び出し元を新しい形にする**

```sh
perl -0pi -e 's/ao_browser_class_at\((\w+), (?!nullptr|&)/ao_browser_class_at($1, nullptr, /g' \
  runtime/tests/browser_abi_test.cpp runtime/tests/accept_abi_test.cpp runtime/tests/remove_abi_test.cpp
grep -rn "ao_browser_class_at(" runtime/tests app/Ao
```

Expected: C++ のテストの呼び出しはどれも 2 番目の引数が `nullptr` か `&rowId`。新しく書いたテスト（`&rowId`）は変わらない。

`app/Ao/BrowserModel.swift` の `copyClass` の呼び出しを直す（Task 5 で書き直すまでの最小の変更）:

```swift
          return ao_browser_class_at(index, nil, nameBase, Int32(capacity), categoryBase, Int32(capacity))
```

- [ ] **Step 4: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（`ao_browser_class_id`、`Session::classIds`、`classIdRootSlots` が無い）。

- [ ] **Step 5: `Session` に表を足す**

`runtime/src/Session.hpp` の `Session` の `heldDoIts` の宣言のあとに足す:

```cpp
  // SPEC §3.10 クラス ID: the classes the Browser ABI handed out, each with its ID. Each cls is a
  // root slot (Roots::add) the GC updates; unique_ptr keeps the slot in place when the table grows.
  // Neither the image save nor a trace for what is alive (SPEC §3.9) starts from these slots
  // (classIdRootSlots). Not part of the image; a new session starts empty.
  struct ClassId {
    std::int64_t id = 0;
    Oop cls = Oop::nil();
  };
  std::vector<std::unique_ptr<ClassId>> classIds;
```

`browserClassAt` の宣言を置き換え、その前に足す:

```cpp
// SPEC §3.10 クラス ID: the ID of the listed class Smalltalk binds name to; 0 when none, name is
// NULL or there is no session. May throw std::bad_alloc (the class list issues IDs).
std::int64_t browserClassId(const char* name);
// SPEC §3.9, §3.10 クラス ID: the ID table's root slots. Neither the image save nor liveClasses
// traces them. Empty outside a session.
std::vector<const Oop*> classIdRootSlots();

int browserClassCount();
int browserClassAt(int index, std::int64_t* classId, char* name, int nameLen, char* category,
                   int categoryLen);
```

（`int browserClassCount();` は既存の行を移すだけ。重複させない。）

- [ ] **Step 6: 解放を足す**

`runtime/src/Session.cpp` の最初の無名名前空間で、`releaseMethodSources` のあとに足す（`insert_after_symbol`）:

```cpp
// SPEC §3.10 クラス ID: every entry's root slot goes, newest first (Roots::remove looks from the
// newest registration back).
void releaseClassIds(Session& session) {
  for (auto it = session.classIds.rbegin(); it != session.classIds.rend(); ++it) {
    session.roots.remove(&(*it)->cls);
  }
  session.classIds.clear();
}
```

`Session::~Session` を置き換える:

```cpp
Session::~Session() {
  // SPEC §3.4 abandon, §3.10: the processes left go first, while the heap and the roots are there.
  scheduler.reset();
  releaseMethodSources(*this);
  releaseClassIds(*this);
}
```

- [ ] **Step 7: `ClassRow` と `classRows` に ID を足す**

Session.cpp の include に `#include <cstdint>`、`#include <unordered_map>`、`#include <unordered_set>` を足す。

`ClassRow` を置き換える:

```cpp
struct ClassRow {
  Oop cls;
  // SPEC §3.10 クラス ID: never 0 in a row classRows answers.
  std::int64_t id = 0;
  std::string name;
  std::string category;
};
```

`classRows` の前に足す（`insert_before_symbol`）:

```cpp
// SPEC §3.10 クラス ID: never used again in this OS process, not even by a later session (as
// nextProcessId in Scheduler.cpp).
std::int64_t nextClassId() {
  static std::int64_t next = 1;
  return next++;
}

// SPEC §3.10 クラス ID: drops the table's entries whose class is not in rows (bound under no name
// any more), then gives each row its class's ID, issuing one for a class the table does not hold.
// May throw std::bad_alloc: each new entry is reserved before it is rooted and numbered, so the
// table stays whole and no ID is spent on a failed entry. Allocates nothing on the heap.
void assignClassIds(Session& s, std::vector<ClassRow>& rows) {
  std::unordered_set<std::uint64_t> listed;
  listed.reserve(rows.size());
  for (const ClassRow& row : rows) {
    listed.insert(row.cls.bits());
  }
  auto& table = s.classIds;
  for (auto it = table.begin(); it != table.end();) {
    if (listed.count((*it)->cls.bits()) != 0) {
      ++it;
      continue;
    }
    s.roots.remove(&(*it)->cls);
    it = table.erase(it);
  }
  std::unordered_map<std::uint64_t, std::int64_t> known;
  known.reserve(table.size());
  for (const auto& entry : table) {
    known.emplace(entry->cls.bits(), entry->id);
  }
  for (ClassRow& row : rows) {
    if (const auto found = known.find(row.cls.bits()); found != known.end()) {
      row.id = found->second;
      continue;
    }
    auto entry = std::make_unique<Session::ClassId>();
    entry->cls = row.cls;
    table.reserve(table.size() + 1);
    s.roots.reserveSlots(1);
    entry->id = nextClassId();
    s.roots.add(&entry->cls);
    row.id = entry->id;
    table.push_back(std::move(entry));
  }
}
```

`classRows` を置き換える:

```cpp
// SPEC §3.10: one row per class Smalltalk binds, however many names bind it, named by its own name
// slot, in byte order of the name and then by ID. Every call prunes and issues class IDs
// (assignClassIds), so a row always carries a known ID. May throw std::bad_alloc.
std::vector<ClassRow> classRows(Session& s) {
  struct Baton {
    Session* session;
    std::vector<ClassRow>* rows;
    std::unordered_set<std::uint64_t>* seen;
  };
  std::vector<ClassRow> rows;
  std::unordered_set<std::uint64_t> seen;
  Baton baton{&s, &rows, &seen};
  s.wk.eachClass(
      [](void* p, Oop cls) {
        auto* b = static_cast<Baton*>(p);
        if (!pointerSlots(b->session->heap, cls, kClassSlotCount) ||
            !b->seen->insert(cls.bits()).second) {
          return;
        }
        ClassRow row;
        row.cls = cls;
        row.name = classNameOf(b->session->heap, cls);
        row.category = categoryHeading(b->session->heap, cls);
        b->rows->push_back(std::move(row));
      },
      &baton);
  assignClassIds(s, rows);
  std::sort(rows.begin(), rows.end(), [](const ClassRow& a, const ClassRow& b) {
    if (a.name != b.name) {
      return utf8Less(a.name, b.name);
    }
    return a.id < b.id;
  });
  return rows;
}
```

- [ ] **Step 8: 行の ID、`browserClassId`、`classIdRootSlots`**

`browserClassAt` を置き換える:

```cpp
int browserClassAt(int index, std::int64_t* classId, char* name, int nameLen, char* category,
                   int categoryLen) {
  if (classId != nullptr) {
    *classId = 0;
  }
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  if (index < 0 || static_cast<std::size_t>(index) >= rows.size()) {
    return AO_ERR;
  }
  const ClassRow& row = rows[static_cast<std::size_t>(index)];
  const int nameRc = writeBuf(row.name, name, nameLen);
  const int categoryRc = writeBuf(row.category, category, categoryLen);
  if (nameRc == AO_ERR || categoryRc == AO_ERR) {
    return AO_ERR;
  }
  if (classId != nullptr) {
    *classId = row.id;
  }
  if (nameRc == AO_ERR_RANGE || categoryRc == AO_ERR_RANGE) {
    return AO_ERR_RANGE;
  }
  return AO_OK;
}
```

`browserClassCount` の前に足す:

```cpp
std::int64_t browserClassId(const char* name) {
  Session* s = session();
  if (s == nullptr || name == nullptr) {
    return 0;
  }
  const auto rows = classRows(*s);
  // named looks the name up without interning it; nil (no binding) is never a row's class.
  const Oop cls = s->wk.named(name);
  for (const ClassRow& row : rows) {
    if (row.cls == cls) {
      return row.id;
    }
  }
  return 0;
}

std::vector<const Oop*> classIdRootSlots() {
  std::vector<const Oop*> slots;
  if (Session* s = session()) {
    slots.reserve(s->classIds.size());
    for (const auto& entry : s->classIds) {
      slots.push_back(&entry->cls);
    }
  }
  return slots;
}
```

- [ ] **Step 9: 保存と生存の判定から外す**

`sessionImageSave` の `HideMethodSources` のコンストラクタで、`slots = methodSourceRootSlots(false);` の直後に足す:

```cpp
        // SPEC §3.10 クラス ID: the ID table's slots are LIFO slots too, and session state.
        const std::vector<const Oop*> ids = classIdRootSlots();
        slots.insert(slots.end(), ids.begin(), ids.end());
```

`runtime/src/Compile.cpp` の `liveClasses` で、`methodSourceRootSlots()` の `for` のあとに足す:

```cpp
  // SPEC §3.9, §3.10 クラス ID: the class ID table is a session table no Smalltalk object reaches.
  for (const Oop* slot : classIdRootSlots()) {
    start.hidden.insert(slot);
  }
```

`liveClasses` の上のコメントの「The method cache and the method source table are roots」を「The method cache, the method source table and the class ID table are roots」にする。

- [ ] **Step 10: ABI**

`bridge/ao_abi.h` の `int ao_browser_class_count(void);` の前のコメントと 2 行を置き換える:

```c
/* The four *_count functions answer 0 or more, or -1 on failure: no session, a name that is not
   a class, meta other than 0 or 1, a NULL argument. Never AO_ERR, which reads as one row. */
/* SPEC §3.10 クラス ID. The ID of the listed class Smalltalk binds name to; 0 when none, name is
   NULL, there is no session, or memory runs out. The same class answers the same ID for the
   whole session; an ID is never used again in the process. */
int64_t ao_browser_class_id(const char* name);
int ao_browser_class_count(void);
/* One row per class (one row however many names bind it), by name and then by ID. class_id (may
   be NULL) gets the row's ID, 0 on AO_ERR. */
int ao_browser_class_at(int index, int64_t* class_id, char* name, int name_len, char* category,
                        int category_len);
```

`runtime/src/abi.cpp` の `ao_browser_class_at` を置き換え、その前に足す:

```cpp
// SPEC §3.10 クラス ID: a read, like the other ao_browser_* (no AbiEntry). 0 on any failure.
extern "C" int64_t ao_browser_class_id(const char* name) {
  try {
    return ao::browserClassId(name);
  } catch (...) {
    return 0;
  }
}

extern "C" int ao_browser_class_at(int index, int64_t* class_id, char* name, int name_len,
                                    char* category, int category_len) {
  if (class_id != nullptr) {
    *class_id = 0;
  }
  return guarded(AO_ERR, [&] {
    return ao::browserClassAt(index, class_id, name, name_len, category, category_len);
  });
}
```

- [ ] **Step 11: 通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'BrowserAbi|AcceptAbi|SessionAbi|RemoveAbi|HashedCollection'`
Expected: PASS。

Run: `swift test --package-path app -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" -Xlinker -lc++ --filter 'AcceptTests|BrowserModelTests'`
Expected: PASS（Swift は ID を読まないが、一覧が 1 クラス 1 行になっても既存のテストは変わらない）。

- [ ] **Step 12: Serena の overview を取り、コミットする**

`get_symbols_overview runtime/src/Session.cpp`（`assignClassIds`、`nextClassId`、`releaseClassIds`、`browserClassId`、`classIdRootSlots` があること）。

```bash
git add runtime/src/Session.hpp runtime/src/Session.cpp runtime/src/Compile.cpp bridge/ao_abi.h runtime/src/abi.cpp app/Ao/BrowserModel.swift runtime/tests/browser_abi_test.cpp runtime/tests/accept_abi_test.cpp runtime/tests/remove_abi_test.cpp runtime/tests/session_abi_test.cpp
git commit -m "Give each Browser class row a session class ID

SPEC §3.10 クラス ID: the session keeps an ID table (ID to class, GC-updated root slots
that the image save and liveClasses do not trace). classRows lists one row per class,
prunes classes no name binds and issues monotonic IDs. ao_browser_class_id answers the
ID of the class bound to a name; ao_browser_class_at answers the row's ID.

Graphify: explain classRows(); path liveClasses() methodSourceRootSlots()
Serena: replace_symbol_body classRows, browserClassAt, ~Session; insert_after_symbol releaseMethodSources

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 3: 読み出しを ID で引く

**Files:**
- Modify: `runtime/src/Session.hpp`（`browserProtocolCount` から `browserSubclassAt` までの宣言）
- Modify: `runtime/src/Session.cpp`（`findClass` → `findClassId`、`superclassName` → `superclassShown`、`subclassNames` → `subclassRows`、9 つの `browser*` の本体）
- Modify: `bridge/ao_abi.h`、`runtime/src/abi.cpp`（9 つの読み出し）
- Modify: `app/Ao/BrowserModel.swift`（名前 → ID のつなぎ。Task 5 で書き直す）
- Test: `runtime/tests/browser_abi_test.cpp`、`runtime/tests/accept_abi_test.cpp`、`runtime/tests/hashed_collection_test.cpp`、`runtime/tests/remove_abi_test.cpp`

**Interfaces:**
- Consumes: Task 2 の `classRows`、`ClassRow::id`、`ao_browser_class_id`
- Produces:
  - `int ao_browser_protocol_count(int64_t class_id, int meta)`、`int ao_browser_protocol_at(int64_t class_id, int meta, int index, char* buf, int len)`
  - `int ao_browser_selector_count(int64_t class_id, int meta, const char* protocol)`、`int ao_browser_selector_at(int64_t class_id, int meta, const char* protocol, int index, char* buf, int len)`
  - `int ao_browser_source(int64_t class_id, int meta, const char* selector, char* buf, int len)`
  - `int ao_browser_class_definition(int64_t class_id, char* buf, int len)`
  - `int ao_browser_superclass(int64_t class_id, int meta, int64_t* superclass_id, char* buf, int len)`
  - `int ao_browser_subclass_count(int64_t class_id)`、`int ao_browser_subclass_at(int64_t class_id, int index, int64_t* subclass_id, char* buf, int len)`
  - Session.cpp の無名名前空間: `const ClassRow* findClassId(const std::vector<ClassRow>&, std::int64_t)`
  - Swift: `static func BrowserModel.classID(named: String) -> Int64`

- [ ] **Step 1: Serena で参照を取る**

`find_referencing_symbols` で `ao/(anonymous namespace)/findClass`、`superclassName`、`subclassNames`（どれも `browser*` だけが呼ぶこと）。`find_symbol` で `ao/browserSource`（`shownName` の作り方）。

- [ ] **Step 2: テストの呼び出しを ID に移す**

```sh
perl -0pi -e 's/ao_browser_(protocol_count|protocol_at|selector_count|selector_at|source|class_definition|subclass_count)\(((?:"[^"]*")|[A-Za-z_][\w.]*)/ao_browser_$1(ao_browser_class_id($2)/g; s/ao_browser_superclass\(((?:"[^"]*")|[A-Za-z_][\w.]*), (\w+), /ao_browser_superclass(ao_browser_class_id($1), $2, nullptr, /g; s/ao_browser_subclass_at\(((?:"[^"]*")|[A-Za-z_][\w.]*), (\w+), /ao_browser_subclass_at(ao_browser_class_id($1), $2, nullptr, /g' \
  runtime/tests/browser_abi_test.cpp runtime/tests/accept_abi_test.cpp runtime/tests/hashed_collection_test.cpp runtime/tests/remove_abi_test.cpp
grep -n "ao_browser_\(protocol\|selector\|source\|class_definition\|superclass\|subclass\)" runtime/tests/*.cpp | grep -v "ao_browser_class_id(" 
```

Expected: 最後の grep は Step 3 と Step 4 で足す新しいテストの行（ID の変数を渡すもの）しか出さない。名前が未定義のとき `ao_browser_class_id` は 0 を返し、読み出しは今までどおり `AO_ERR` / -1 になる。

- [ ] **Step 3: `CountsAnswerMinusOneOnFailure` を書き直す**

`runtime/tests/browser_abi_test.cpp` の `CountsAnswerMinusOneOnFailure` を置き換える:

```cpp
// 06 Low / SPEC §3.10: 件数を返す関数は、失敗なら -1 を返す（AO_ERR の 1 は 1 件と区別できない）。
// 失敗は、セッションが無い、クラス ID が未知、meta が 0 でも 1 でない、引数が NULL。
TEST_F(BrowserAbi, CountsAnswerMinusOneOnFailure) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::int64_t earlier = ao_browser_class_id("Object");
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(-1, ao_browser_class_count());
  EXPECT_EQ(-1, ao_browser_protocol_count(earlier, 0));
  EXPECT_EQ(-1, ao_browser_selector_count(earlier, 0, "native"));
  EXPECT_EQ(-1, ao_browser_subclass_count(earlier));
  EXPECT_EQ(0, ao_browser_class_id("Object"));

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::int64_t object = ao_browser_class_id("Object");
  EXPECT_GT(ao_browser_class_count(), 0);
  EXPECT_EQ(-1, ao_browser_protocol_count(0, 0));
  EXPECT_EQ(-1, ao_browser_selector_count(0, 0, "native"));
  EXPECT_EQ(-1, ao_browser_subclass_count(0));
  EXPECT_EQ(-1, ao_browser_protocol_count(object, 2));
  EXPECT_EQ(-1, ao_browser_protocol_count(object, -1));
  EXPECT_EQ(-1, ao_browser_selector_count(object, 2, "native"));
  EXPECT_EQ(-1, ao_browser_selector_count(object, 0, nullptr));
  // 成功なら 0 以上。
  EXPECT_EQ(1, ao_browser_protocol_count(object, 0));
  EXPECT_GT(ao_browser_selector_count(object, 0, "native"), 0);
  EXPECT_EQ(0, ao_browser_selector_count(object, 0, "user"));
}
```

- [ ] **Step 4: 失敗するテストを書く**

`runtime/tests/browser_abi_test.cpp` の include に `#include <cstdio>`、`#include <limits>`、`#include <utility>` を足し、末尾に足す:

```cpp
// SPEC §3.10 クラス ID: an ID that is 0 or less, never issued, or dropped with its class is
// unknown: the counts answer -1 and every other read AO_ERR (ID out-parameters get 0).
TEST_F(BrowserAbi, UnknownClassIdIsRefusedByEveryRead) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Gone"));
  const std::int64_t gone = ao_browser_class_id("B12Gone");
  ASSERT_GT(gone, 0);
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12Gone put: nil"));
  char buf[256];
  for (const std::int64_t id :
       {std::int64_t{0}, std::int64_t{-1}, gone, std::numeric_limits<std::int64_t>::max()}) {
    SCOPED_TRACE(id);
    EXPECT_EQ(-1, ao_browser_protocol_count(id, 0));
    EXPECT_EQ(AO_ERR, ao_browser_protocol_at(id, 0, 0, buf, 256));
    EXPECT_EQ(-1, ao_browser_selector_count(id, 0, "user"));
    EXPECT_EQ(AO_ERR, ao_browser_selector_at(id, 0, "user", 0, buf, 256));
    EXPECT_EQ(AO_ERR, ao_browser_source(id, 0, "printString", buf, 256));
    EXPECT_EQ(AO_ERR, ao_browser_class_definition(id, buf, 256));
    std::int64_t out = -7;
    EXPECT_EQ(AO_ERR, ao_browser_superclass(id, 0, &out, buf, 256));
    EXPECT_EQ(0, out);
    EXPECT_EQ(-1, ao_browser_subclass_count(id));
    out = -7;
    EXPECT_EQ(AO_ERR, ao_browser_subclass_at(id, 0, &out, buf, 256));
    EXPECT_EQ(0, out);
  }
}

// SPEC §3.10 クラス ID: a new session (a load, a boot) starts an empty table. IDs from before are
// unknown there, and the same class gets a new, larger ID.
TEST_F(BrowserAbi, ClassIdsDoNotSurviveBootOrImageLoad) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const std::int64_t before = ao_browser_class_id("Object");
  ASSERT_GT(before, 0);
  EXPECT_EQ(1, ao_browser_protocol_count(before, 0));
  const char* path = "browser-abi-ids.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_image_load(path, &err)) << err.message;
  std::remove(path);
  EXPECT_EQ(-1, ao_browser_protocol_count(before, 0));
  const std::int64_t loaded = ao_browser_class_id("Object");
  EXPECT_GT(loaded, before);
  EXPECT_EQ(1, ao_browser_protocol_count(loaded, 0));
  ASSERT_EQ(AO_OK, ao_runtime_shutdown());
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(-1, ao_browser_protocol_count(loaded, 0));
  const std::int64_t booted = ao_browser_class_id("Object");
  EXPECT_GT(booted, loaded);
  EXPECT_EQ(1, ao_browser_protocol_count(booted, 0));
}

// SPEC §3.10 クラス ID: two listed classes with one name (the second's name slot rewritten with
// instVarAt:put:, kClassSlotName is slot 3, index 4) are two rows in ID order, and each ID reads
// its own class.
TEST_F(BrowserAbi, SameNamedClassesAreReadApartById) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineRow("B12Twin", "B12-TwinA"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Twin", 0, "first\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, defineRow("B12TwinB", "B12-TwinB"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12TwinB", 0, "second\n  ^2\n", &err)) << err.message;
  const std::int64_t a = ao_browser_class_id("B12Twin");
  const std::int64_t b = ao_browser_class_id("B12TwinB");
  ASSERT_LT(a, b);
  ASSERT_EQ(AO_OK, doIt("B12TwinB instVarAt: 4 put: #B12Twin"));
  std::vector<std::pair<std::int64_t, std::string>> twins;
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    std::int64_t id = 0;
    char name[128];
    char category[128];
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, &id, name, 128, category, 128));
    if (std::strcmp(name, "B12Twin") == 0) {
      twins.emplace_back(id, category);
    }
  }
  ASSERT_EQ(2u, twins.size());
  EXPECT_EQ(std::make_pair(a, std::string("B12-TwinA")), twins[0]);
  EXPECT_EQ(std::make_pair(b, std::string("B12-TwinB")), twins[1]);
  char buf[256];
  ASSERT_EQ(AO_OK, ao_browser_selector_at(a, 0, "user", 0, buf, 256));
  EXPECT_STREQ("first", buf);
  ASSERT_EQ(AO_OK, ao_browser_selector_at(b, 0, "user", 0, buf, 256));
  EXPECT_STREQ("second", buf);
  ASSERT_EQ(AO_OK, ao_browser_class_definition(b, buf, 256));
  EXPECT_NE(std::string::npos, std::string(buf).find("category: 'B12-TwinB'"));
  ASSERT_EQ(AO_OK, ao_browser_source(b, 0, "second", buf, 256));
  EXPECT_STREQ("second\n  ^2\n", buf);
}

// SPEC §3.10: the superclass read answers the name and its ID (the class side names the instance
// class); the subclass reads answer the listed classes whose superclass slot is the class itself,
// by identity, in list order. A superclass that is not listed keeps its name with ID 0.
TEST_F(BrowserAbi, SuperclassAndSubclassesAnswerIds) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, defineRow("B12Sup"));
  ASSERT_EQ(AO_OK, defineRow("B12SubB", "B12-Id", "B12Sup"));
  ASSERT_EQ(AO_OK, defineRow("B12SubA", "B12-Id", "B12Sup"));
  const std::int64_t sup = ao_browser_class_id("B12Sup");
  const std::int64_t subA = ao_browser_class_id("B12SubA");
  const std::int64_t subB = ao_browser_class_id("B12SubB");
  char buf[128];
  std::int64_t id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(subA, 0, &id, buf, 128));
  EXPECT_STREQ("B12Sup", buf);
  EXPECT_EQ(sup, id);
  id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(subA, 1, &id, buf, 128));
  EXPECT_STREQ("B12Sup", buf);
  EXPECT_EQ(sup, id);
  id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(ao_browser_class_id("Object"), 0, &id, buf, 128));
  EXPECT_STREQ("", buf);
  EXPECT_EQ(0, id);
  ASSERT_EQ(2, ao_browser_subclass_count(sup));
  ASSERT_EQ(AO_OK, ao_browser_subclass_at(sup, 0, &id, buf, 128));
  EXPECT_STREQ("B12SubA", buf);
  EXPECT_EQ(subA, id);
  ASSERT_EQ(AO_OK, ao_browser_subclass_at(sup, 1, &id, buf, 128));
  EXPECT_STREQ("B12SubB", buf);
  EXPECT_EQ(subB, id);
  EXPECT_EQ(AO_OK, ao_browser_subclass_at(sup, 1, nullptr, buf, 128));
  // A class that only shares B12Sup's name is not B12Sup: its subclass is no subclass of B12Sup.
  ASSERT_EQ(AO_OK, defineRow("B12Other"));
  ASSERT_EQ(AO_OK, defineRow("B12OtherKid", "B12-Id", "B12Other"));
  ASSERT_EQ(AO_OK, doIt("B12Other instVarAt: 4 put: #B12Sup"));
  EXPECT_EQ(2, ao_browser_subclass_count(sup));
  // Unbound, B12Other leaves the list: its name stays, its ID is 0.
  ASSERT_EQ(AO_OK, doIt("Smalltalk at: #B12Other put: nil"));
  id = -7;
  ASSERT_EQ(AO_OK, ao_browser_superclass(ao_browser_class_id("B12OtherKid"), 0, &id, buf, 128));
  EXPECT_STREQ("B12Sup", buf);
  EXPECT_EQ(0, id);
}
```

- [ ] **Step 5: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（ABI がまだ名前を取る。`ao_browser_protocol_count(int64_t…)` などが合わない）。

- [ ] **Step 6: Session の読み出しを ID にする**

`runtime/src/Session.hpp` の宣言を置き換える:

```cpp
int browserProtocolCount(std::int64_t classId, int meta);
int browserProtocolAt(std::int64_t classId, int meta, int index, char* buf, int len);
int browserSelectorCount(std::int64_t classId, int meta, const char* protocol);
int browserSelectorAt(std::int64_t classId, int meta, const char* protocol, int index, char* buf,
                      int len);
int browserSource(std::int64_t classId, int meta, const char* selector, char* buf, int len);
int browserClassDefinition(std::int64_t classId, char* buf, int len);
int browserSuperclass(std::int64_t classId, int meta, std::int64_t* superclassId, char* buf,
                      int len);
int browserSubclassCount(std::int64_t classId);
int browserSubclassAt(std::int64_t classId, int index, std::int64_t* subclassId, char* buf,
                      int len);
```

`runtime/src/Session.cpp` の無名名前空間で、`findClass` を置き換える（`replace_symbol_body`。名前も変わる）:

```cpp
// SPEC §3.10 クラス ID: the row id names; null for an unknown ID (0 or less, never issued, or
// dropped because the class left the list).
const ClassRow* findClassId(const std::vector<ClassRow>& rows, std::int64_t id) {
  if (id <= 0) {
    return nullptr;
  }
  for (const auto& row : rows) {
    if (row.id == id) {
      return &row;
    }
  }
  return nullptr;
}
```

`superclassName` を置き換える:

```cpp
// SPEC §3.10: the class the superclass read names: the side's superclass, or, when that is a
// metaclass, its instance class (thisClass) when it has one. nil when there is none.
Oop superclassShown(Session& s, Oop cls, int meta) {
  const Oop side = sideOf(s, cls, meta);
  if (!pointerSlots(s.heap, side, kClassSlotSuperclass + 1)) {
    return Oop::nil();
  }
  const Oop sup = s.heap.slotAt(side, kClassSlotSuperclass);
  if (!sup.isHeap()) {
    return Oop::nil();
  }
  if (s.heap.klass(sup) == s.wk.metaclassClass &&
      pointerSlots(s.heap, sup, kClassSlotThisClass + 1)) {
    const Oop thisClass = s.heap.slotAt(sup, kClassSlotThisClass);
    if (thisClass.isHeap()) {
      return thisClass;
    }
  }
  return sup;
}
```

`subclassNames` を置き換える:

```cpp
// SPEC §3.10: the listed classes whose superclass slot is cls itself (identity, not name), in the
// class list's order.
std::vector<const ClassRow*> subclassRows(Session& s, Oop cls, const std::vector<ClassRow>& rows) {
  std::vector<const ClassRow*> out;
  for (const auto& row : rows) {
    if (pointerSlots(s.heap, row.cls, kClassSlotSuperclass + 1) &&
        s.heap.slotAt(row.cls, kClassSlotSuperclass) == cls) {
      out.push_back(&row);
    }
  }
  return out;
}
```

9 つの本体を置き換える:

```cpp
int browserProtocolCount(std::int64_t classId, int meta) {
  Session* s = session();
  if (s == nullptr || !metaOk(meta)) {
    return kCountFailed;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return kCountFailed;
  }
  return static_cast<int>(protocolsOf(methodsOf(*s, sideOf(*s, row->cls, meta))).size());
}

int browserProtocolAt(std::int64_t classId, int meta, int index, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto protocols = protocolsOf(methodsOf(*s, sideOf(*s, row->cls, meta)));
  if (index < 0 || static_cast<std::size_t>(index) >= protocols.size()) {
    return AO_ERR;
  }
  return writeBuf(protocols[static_cast<std::size_t>(index)], buf, len);
}

int browserSelectorCount(std::int64_t classId, int meta, const char* protocol) {
  Session* s = session();
  if (s == nullptr || protocol == nullptr || !metaOk(meta)) {
    return kCountFailed;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return kCountFailed;
  }
  if (!knownProtocol(protocol)) {
    return 0;
  }
  const auto methods = methodsOf(*s, sideOf(*s, row->cls, meta));
  return static_cast<int>(selectorsFor(methods, protocol).size());
}

int browserSelectorAt(std::int64_t classId, int meta, const char* protocol, int index, char* buf,
                      int len) {
  Session* s = session();
  if (s == nullptr || protocol == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto selectors = selectorsFor(methodsOf(*s, sideOf(*s, row->cls, meta)), protocol);
  if (index < 0 || static_cast<std::size_t>(index) >= selectors.size()) {
    return AO_ERR;
  }
  return writeBuf(selectors[static_cast<std::size_t>(index)].selector, buf, len);
}

int browserSource(std::int64_t classId, int meta, const char* selector, char* buf, int len) {
  Session* s = session();
  if (s == nullptr || selector == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return AO_ERR;
  }
  const std::string shownName = meta == 1 ? row->name + " class" : row->name;
  const auto methods = methodsOf(*s, sideOf(*s, row->cls, meta));
  for (const auto& method : methods) {
    if (method.selector == selector) {
      std::string text;
      if (sourceOf(*s, shownName, method, text)) {
        return writeBuf(text, buf, len);
      }
      // SPEC §3.10: AO_ERR_NOSOURCE wins over AO_ERR_RANGE; writeBuf still cuts and ends in NUL.
      return writeBuf(text, buf, len) == AO_ERR ? AO_ERR : AO_ERR_NOSOURCE;
    }
  }
  return AO_ERR;
}

int browserClassDefinition(std::int64_t classId, char* buf, int len) {
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return AO_ERR;
  }
  return writeBuf(definitionOf(*s, *row), buf, len);
}

int browserSuperclass(std::int64_t classId, int meta, std::int64_t* superclassId, char* buf,
                      int len) {
  if (superclassId != nullptr) {
    *superclassId = 0;
  }
  Session* s = session();
  if (s == nullptr || !metaOk(meta)) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return AO_ERR;
  }
  const Oop sup = superclassShown(*s, row->cls, meta);
  const int rc = writeBuf(classNameOf(s->heap, sup), buf, len);
  if (superclassId != nullptr && rc != AO_ERR) {
    for (const auto& each : rows) {
      if (each.cls == sup) {
        *superclassId = each.id;
        break;
      }
    }
  }
  return rc;
}

int browserSubclassCount(std::int64_t classId) {
  Session* s = session();
  if (s == nullptr) {
    return kCountFailed;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return kCountFailed;
  }
  return static_cast<int>(subclassRows(*s, row->cls, rows).size());
}

int browserSubclassAt(std::int64_t classId, int index, std::int64_t* subclassId, char* buf,
                      int len) {
  if (subclassId != nullptr) {
    *subclassId = 0;
  }
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, classId);
  if (row == nullptr) {
    return AO_ERR;
  }
  const auto subs = subclassRows(*s, row->cls, rows);
  if (index < 0 || static_cast<std::size_t>(index) >= subs.size()) {
    return AO_ERR;
  }
  const ClassRow& sub = *subs[static_cast<std::size_t>(index)];
  const int rc = writeBuf(sub.name, buf, len);
  if (subclassId != nullptr && rc != AO_ERR) {
    *subclassId = sub.id;
  }
  return rc;
}
```

`browserSuperclass` のクラスが nil のとき `classNameOf(nil)` は空文字（今の `superclassName` と同じ）。

- [ ] **Step 7: ABI**

`bridge/ao_abi.h` の 4 つの件数のコメントの「a name that is not a class」を「an unknown class ID (SPEC §3.10 クラス ID)」にし、読み出しの宣言を置き換える:

```c
int ao_browser_protocol_count(int64_t class_id, int meta);
int ao_browser_protocol_at(int64_t class_id, int meta, int index, char* buf, int len);
int ao_browser_selector_count(int64_t class_id, int meta, const char* protocol);
int ao_browser_selector_at(int64_t class_id, int meta, const char* protocol, int index,
                           char* buf, int len);
/* AO_OK with the source when the source table has it. A method without source (a native, a
   method after an image load, a vendor, file-in or methodsFor: chunk method) is AO_ERR_NOSOURCE
   with a one-line comment placeholder that does not compile when accepted:
   "<Class>>><selector> source not available" or "<Class>>><selector> native <symbol>", where
   <Class> is "<Name> class" on the class side. AO_ERR_NOSOURCE also when the placeholder is cut
   (buf still ends in NUL). AO_ERR when buf is NULL, len < 1, the class ID is unknown or the
   selector is not found. */
int ao_browser_source(int64_t class_id, int meta, const char* selector, char* buf, int len);
int ao_browser_class_definition(int64_t class_id, char* buf, int len);
/* buf gets the superclass's name ("" for nil; on the class side the instance class's name) and
   superclass_id (may be NULL) its ID, 0 when it is nil or not listed. */
int ao_browser_superclass(int64_t class_id, int meta, int64_t* superclass_id, char* buf, int len);
/* The listed classes whose superclass is this very class, in list order; subclass_id may be
   NULL. */
int ao_browser_subclass_count(int64_t class_id);
int ao_browser_subclass_at(int64_t class_id, int index, int64_t* subclass_id, char* buf, int len);
```

`runtime/src/abi.cpp` の 9 つを置き換える:

```cpp
extern "C" int ao_browser_protocol_count(int64_t class_id, int meta) {
  return guarded(-1, [&] { return ao::browserProtocolCount(class_id, meta); });
}

extern "C" int ao_browser_protocol_at(int64_t class_id, int meta, int index, char* buf, int len) {
  return guarded(AO_ERR, [&] { return ao::browserProtocolAt(class_id, meta, index, buf, len); });
}

extern "C" int ao_browser_selector_count(int64_t class_id, int meta, const char* protocol) {
  return guarded(-1, [&] { return ao::browserSelectorCount(class_id, meta, protocol); });
}

extern "C" int ao_browser_selector_at(int64_t class_id, int meta, const char* protocol, int index,
                                      char* buf, int len) {
  return guarded(AO_ERR, [&] {
    return ao::browserSelectorAt(class_id, meta, protocol, index, buf, len);
  });
}

extern "C" int ao_browser_source(int64_t class_id, int meta, const char* selector, char* buf,
                                 int len) {
  return guarded(AO_ERR, [&] { return ao::browserSource(class_id, meta, selector, buf, len); });
}

extern "C" int ao_browser_class_definition(int64_t class_id, char* buf, int len) {
  return guarded(AO_ERR, [&] { return ao::browserClassDefinition(class_id, buf, len); });
}

extern "C" int ao_browser_superclass(int64_t class_id, int meta, int64_t* superclass_id, char* buf,
                                     int len) {
  if (superclass_id != nullptr) {
    *superclass_id = 0;
  }
  return guarded(AO_ERR, [&] {
    return ao::browserSuperclass(class_id, meta, superclass_id, buf, len);
  });
}

extern "C" int ao_browser_subclass_count(int64_t class_id) {
  return guarded(-1, [&] { return ao::browserSubclassCount(class_id); });
}

extern "C" int ao_browser_subclass_at(int64_t class_id, int index, int64_t* subclass_id, char* buf,
                                      int len) {
  if (subclass_id != nullptr) {
    *subclass_id = 0;
  }
  return guarded(AO_ERR, [&] {
    return ao::browserSubclassAt(class_id, index, subclass_id, buf, len);
  });
}
```

- [ ] **Step 8: Swift を名前 → ID でつなぐ（Task 5 で書き直す）**

`app/Ao/BrowserModel.swift` の `boot()` の前に足す:

```swift
  // SPEC §3.10 ao_browser_class_id: the ID of the class Smalltalk binds to name; 0 when none.
  static func classID(named name: String) -> Int64 {
    name.withCString { ao_browser_class_id($0) }
  }
```

ABI を呼ぶ 6 か所を次のように直す（`selectedClass` / `className` はそれぞれの関数の名前の変数）:

```swift
  func selector(withSource text: String) -> String? {
    guard let selectedClass else {
      return nil
    }
    let classID = Self.classID(named: selectedClass)
    let meta = metaFlag
    return selectors.first { selector in
      let source = copyText { buffer, length in
        ao_browser_source(classID, meta, selector, buffer, length)
      }
      return source == text
    }
  }
```

```swift
  private func loadProtocols() -> [String] {
    guard let selectedClass else {
      return []
    }
    let classID = Self.classID(named: selectedClass)
    let meta = metaFlag
    var names = loadList(
      count: { ao_browser_protocol_count(classID, meta) },
      at: { index, buffer, length in
        ao_browser_protocol_at(classID, meta, index, buffer, length)
      }
    )
    if !names.contains(Self.newMethodProtocol) {
      names.append(Self.newMethodProtocol)
    }
    return names
  }

  private func loadSelectors() -> [String] {
    guard let selectedClass, let selectedProtocol else {
      return []
    }
    let classID = Self.classID(named: selectedClass)
    let meta = metaFlag
    return loadList(
      count: { ao_browser_selector_count(classID, meta, selectedProtocol) },
      at: { index, buffer, length in
        ao_browser_selector_at(classID, meta, selectedProtocol, index, buffer, length)
      }
    )
  }

  private func loadSource() -> (text: String, placeholder: Bool) {
    guard let selectedClass else {
      return ("", false)
    }
    let classID = Self.classID(named: selectedClass)
    let meta = metaFlag
    if let selectedSelector {
      let copied = copyReportingNoSource { buffer, length in
        ao_browser_source(classID, meta, selectedSelector, buffer, length)
      }
      return (copied?.text ?? "", copied?.noSource ?? false)
    }
    // A protocol with no selector is a new method; no protocol is the class definition.
    if selectedProtocol != nil {
      return ("", false)
    }
    let definition = copyText { buffer, length in
      ao_browser_class_definition(classID, buffer, length)
    }
    return (definition ?? "", false)
  }
```

```swift
  private func superclassName(_ className: String, meta: Bool) -> String? {
    let flag: Int32 = meta ? 1 : 0
    let classID = Self.classID(named: className)
    return copyText { buffer, length in
      ao_browser_superclass(classID, flag, nil, buffer, length)
    }
  }

  private func subclassNames(_ className: String) -> [String] {
    let classID = Self.classID(named: className)
    return loadList(
      count: { ao_browser_subclass_count(classID) },
      at: { index, buffer, length in
        ao_browser_subclass_at(classID, index, nil, buffer, length)
      }
    )
  }
```

- [ ] **Step 9: 通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'BrowserAbi|AcceptAbi|RemoveAbi|HashedCollection|SessionAbi|DebugAbi'`
Expected: PASS。

Run: `swift test --package-path app -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" -Xlinker -lc++ --filter 'AcceptTests|BrowserModelTests|ToolWindowTests'`
Expected: PASS。

- [ ] **Step 10: Commit**

```bash
git add runtime/src/Session.hpp runtime/src/Session.cpp bridge/ao_abi.h runtime/src/abi.cpp app/Ao/BrowserModel.swift runtime/tests/browser_abi_test.cpp runtime/tests/accept_abi_test.cpp runtime/tests/hashed_collection_test.cpp runtime/tests/remove_abi_test.cpp
git commit -m "Read Browser classes by class ID

SPEC §3.10 クラス ID: the protocol, selector, source, definition, superclass and subclass
reads take a class ID; an unknown ID answers -1 or AO_ERR. The superclass and subclass
reads answer IDs too, and subclasses match by identity, not by name.

Graphify: explain classRows(); query browserSuperclass
Serena: replace_symbol_body findClass, superclassName, subclassNames, browserProtocolCount..browserSubclassAt

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 4: `ao_accept_method_id` と ID による削除

**Files:**
- Modify: `runtime/include/ao/Compile.hpp`、`runtime/src/Compile.cpp`（`isBehavior`、`namesBehavior`、`acceptMethodSource`、`acceptMethodInto`、`removeMethodNamed` → `removeMethodOf`、`removeClassNamed` → `removeClassOf`）
- Modify: `runtime/src/Session.hpp`、`runtime/src/Session.cpp`（`sessionClassForId`）
- Modify: `bridge/ao_abi.h`、`runtime/src/abi.cpp`（`ao_accept_method_id`、`ao_remove_method`、`ao_remove_class`、busy の一覧）
- Modify: `app/Ao/BrowserWindow.swift`（`performRemoveMethod` / `performRemoveClass` の名前 → ID のつなぎ。Task 5 で書き直す）
- Test: `runtime/tests/remove_abi_test.cpp`、`runtime/tests/accept_abi_test.cpp`

**Interfaces:**
- Consumes: Task 2〜3 の `classRows`、`findClassId`、`ao_browser_class_id`
- Produces:
  - `Oop ao::sessionClassForId(std::int64_t id)`
  - `bool ao::acceptMethodInto(CallContext&, Oop cls, bool meta, std::string_view source, compiler::CompileError*)`
  - `bool ao::removeMethodOf(CallContext&, Oop cls, bool meta, std::string_view selector, std::string* reason)`
  - `bool ao::removeClassOf(CallContext&, Oop cls, std::string* reason)`
  - C: `int ao_accept_method_id(int64_t class_id, int meta, const char* source, AoSpan* err)`、`int ao_remove_method(int64_t class_id, int meta, const char* selector, AoSpan* err)`、`int ao_remove_class(int64_t class_id, AoSpan* err)`

- [ ] **Step 1: Graphify と Serena**

`graphify path "ao_remove_class" "removeClassNamed()"`。Serena: `find_referencing_symbols` で `ao/removeMethodNamed`、`ao/removeClassNamed`（`abi.cpp` だけ）、`ao/acceptMethodSource`（`ao_accept_method`、`applyMethodsFor` が呼ぶか確かめる）、`ao/namesBehavior`。

- [ ] **Step 2: 削除のテストのヘルパを ID にする**

`runtime/tests/remove_abi_test.cpp` の include に `#include <limits>` と `#include <utility>` を足す。無名名前空間の `tryRemoveMethod` と `tryRemoveClass` を置き換える:

```cpp
// The ID the Browser hands out for name (SPEC §3.10 クラス ID); 0 for NULL or a name that binds no
// listed class.
std::int64_t idOf(const char* name) { return name == nullptr ? 0 : ao_browser_class_id(name); }

// The call's message. err starts non-empty, so an AO_OK that leaves it alone shows.
std::string tryRemoveMethodId(std::int64_t id, int meta, const char* sel, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_method(id, meta, sel, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
}

std::string tryRemoveMethod(const char* cls, int meta, const char* sel, int* rc) {
  return tryRemoveMethodId(idOf(cls), meta, sel, rc);
}
```

```cpp
std::string tryRemoveClassId(std::int64_t id, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_class(id, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
}

std::string tryRemoveClass(const char* cls, int* rc) { return tryRemoveClassId(idOf(cls), rc); }
```

`idOf` は `tryRemoveMethodId` の前に置く。`BusyRemove` と `removeFromHook` を置き換える:

```cpp
struct BusyRemove {
  std::int64_t objectId = 0;
  std::int64_t busyId = 0;
  int methodRc = -9;
  int classRc = -9;
  int acceptRc = -9;
  std::int64_t readId = -9;
  std::string methodMsg;
  std::string classMsg;
};

// A transcript hook: the runtime is busy here (SPEC §3.10 再入).
void removeFromHook(const char*, int, int, void* user) {
  auto* b = static_cast<BusyRemove*>(user);
  AoSpan err{};
  b->methodRc = ao_remove_method(b->objectId, 0, "b12busy", &err);
  b->methodMsg = err.message;
  b->classRc = ao_remove_class(b->busyId, &err);
  b->classMsg = err.message;
  b->acceptRc = ao_accept_method_id(b->objectId, 0, "b12busy\n  ^2\n", &err);
  b->readId = ao_browser_class_id("B12Busy");
}
```

- [ ] **Step 3: 既存のテストの期待を ID の規則にする**

`RemoveMissingOrInheritedSelectorIsRefused` を置き換える:

```cpp
// SPEC §3.9 削除: only the side's own dictionary counts: an inherited selector, an unknown one and
// an empty one are refused with their reasons. A name that binds no listed class has no ID
// (unknown class id); a bad meta or a NULL selector is "remove failed".
TEST_F(RemoveAbi, RemoveMissingOrInheritedSelectorIsRefused) {
  ASSERT_EQ(AO_OK, defineClass("B12Child", "Object", "B12-Test"));
  int rc = -9;
  EXPECT_EQ("selector not found: B12Child>>printString",
            tryRemoveMethod("B12Child", 0, "printString", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("selector not found: B12Child>>nope", tryRemoveMethod("B12Child", 0, "nope", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("selector not found: B12Child class>>new", tryRemoveMethod("B12Child", 1, "new", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("selector not found: B12Child>>", tryRemoveMethod("B12Child", 0, "", &rc));
  EXPECT_EQ(AO_ERR, rc);
  for (const char* none : {"Processor", "Smalltalk", "B12Nope", ""}) {
    EXPECT_EQ("unknown class id", tryRemoveMethod(none, 0, "foo", &rc)) << none;
    EXPECT_EQ(AO_ERR, rc);
  }
  EXPECT_EQ("unknown class id", tryRemoveMethod(nullptr, 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 2, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 0, nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  // err may be NULL.
  EXPECT_EQ(AO_ERR, ao_remove_method(idOf("B12Child"), 0, "nope", nullptr));
  EXPECT_EQ("'3'", printIt("3 printString"));
}
```

`RemovedClassNameReadsNilAndInstancesKeepWorking` を置き換える:

```cpp
// SPEC §3.9 削除, §6: the name reads nil (PushGlobal, no recompile), the instances keep their
// class and methods, and the Browser no longer lists the class; its ID is unknown from then on.
TEST_F(RemoveAbi, RemovedClassNameReadsNilAndInstancesKeepWorking) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Inst", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Inst", 0, "answer\n  ^7\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12useInst\n  ^B12Inst\n", &err)) << err.message;
  // A class object's own printString goes through Object>>printString too: its class is the
  // metaclass, whose name is "<Name> class".
  EXPECT_EQ("B12Inst class", printIt("Object new b12useInst"));
  // An instance prints as its class name (Object>>printString).
  EXPECT_EQ("B12Inst", printIt("b12i := B12Inst new"));
  ASSERT_TRUE(browserLists("B12Inst"));
  const std::int64_t id = idOf("B12Inst");
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("Object new b12useInst"));
  EXPECT_EQ("7", printIt("b12i answer"));
  EXPECT_EQ("'B12Inst'", printIt("b12i printString"));
  EXPECT_FALSE(browserLists("B12Inst"));
  EXPECT_EQ("unknown class id", tryRemoveClass("B12Inst", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("unknown class id", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_ERR, rc);
}
```

`RemoveFixedGlobalIsRefused` を置き換える:

```cpp
// SPEC §3.9 削除: a fixed global (a Kernel class, a vendor stub) is refused by its own name. A
// name that binds no listed class (Processor, Smalltalk, a metaclass, an unknown or empty name)
// and NULL have no ID.
TEST_F(RemoveAbi, RemoveFixedGlobalIsRefused) {
  int rc = -9;
  EXPECT_EQ("class removal refused: Object is a fixed global", tryRemoveClass("Object", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("class removal refused: Bag is a fixed global", tryRemoveClass("Bag", &rc));
  EXPECT_EQ(AO_ERR, rc);
  for (const char* none : {"Processor", "Smalltalk", "B12Nope", ""}) {
    EXPECT_EQ("unknown class id", tryRemoveClass(none, &rc)) << none;
    EXPECT_EQ(AO_ERR, rc);
  }
  EXPECT_EQ("unknown class id", tryRemoveClass(nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("Smalltalk at: #B12Meta put: Object class").find("<"));
  EXPECT_EQ("unknown class id", tryRemoveClass("B12Meta", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(AO_ERR, ao_remove_class(idOf("Object"), nullptr));
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("Object class", printIt("Object"));
  EXPECT_EQ("Bag class", printIt("Bag"));
}
```

`RemoveKernelAliasIsRefused` を置き換える:

```cpp
// SPEC §3.9 削除: a Kernel class is refused by identity. Through an alias it is the same class
// with the same ID (refused by its own fixed name); with its name slot rewritten (instVarAt:put:,
// kClassSlotName is slot 3, index 4) the identity check still refuses it.
TEST_F(RemoveAbi, RemoveKernelAliasIsRefused) {
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("SmallInteger class", printIt("Smalltalk at: #B12IntAlias put: SmallInteger"));
  EXPECT_EQ(idOf("SmallInteger"), idOf("B12IntAlias"));
  int rc = -9;
  EXPECT_EQ("class removal refused: SmallInteger is a fixed global",
            tryRemoveClass("B12IntAlias", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("b12name := SmallInteger instVarAt: 4").find("<"));
  EXPECT_EQ("'B12Renamed'", printIt("SmallInteger instVarAt: 4 put: 'B12Renamed'"));
  EXPECT_EQ("class removal refused: B12Renamed is a kernel class",
            tryRemoveClass("SmallInteger", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("SmallInteger instVarAt: 4 put: b12name").find("<"));
  EXPECT_EQ("SmallInteger class", printIt("B12IntAlias"));
}
```

`RemoveClassWithSubclassIsRefused` の `B12Anon` の段（`// An unbound subclass …` から `EXPECT_EQ("nil", printIt("b12anon := nil"));` の前まで）を置き換える:

```cpp
  // An unbound subclass a workspace variable keeps alive through an instance still counts, and
  // with its name slot nilled (instVarAt:put:, kClassSlotName is slot 3, index 4) it is unnamed.
  // It goes while its name still binds it (a nameless class is not bound under its own name).
  ASSERT_EQ(AO_OK, defineClass("B12Anon", "B12Base", "B12-Test"));
  EXPECT_EQ("B12Anon", printIt("b12anon := B12Anon new"));
  EXPECT_EQ("", tryRemoveClass("B12Anon", &rc));
  EXPECT_EQ(AO_OK, rc);
  // instVarAt:put: answers the value (ao_Object_instVarAt_put_).
  EXPECT_EQ("nil", printIt("b12anon class instVarAt: 4 put: nil"));
  EXPECT_EQ("class removal refused: B12Base has an unnamed subclass", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
```

`RemoveClassKeepsAliases` を置き換える:

```cpp
// SPEC §3.9 削除: only the class's own binding goes; an alias keeps the class reachable, and the
// Browser keeps listing it under its own name slot with the same ID. That row's removal is refused:
// its name no longer binds it. A nameless class's row is refused the same way.
TEST_F(RemoveAbi, RemoveClassKeepsAliases) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Real", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Real", 0, "answer\n  ^9\n", &err)) << err.message;
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Real class", printIt("Smalltalk at: #B12Alias put: B12Real"));
  const std::int64_t id = idOf("B12Real");
  EXPECT_EQ(id, idOf("B12Alias"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Real"));
  EXPECT_EQ("9", printIt("B12Alias new answer"));
  // classRows reads the class's own name slot, not the binding key it was reached through.
  EXPECT_FALSE(browserLists("B12Alias"));
  EXPECT_TRUE(browserLists("B12Real"));
  EXPECT_EQ(id, idOf("B12Alias"));
  EXPECT_EQ("class removal refused: B12Real is not bound to this class", tryRemoveClassId(id, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("9", printIt("B12Alias new answer"));
  EXPECT_TRUE(browserLists("B12Real"));
  // Unbinding the alias from the Workspace drops the row.
  EXPECT_EQ("nil", printIt("Smalltalk at: #B12Alias put: nil"));
  EXPECT_FALSE(browserLists("B12Real"));
  // A class whose name slot is no String is not bound under its own name either.
  ASSERT_EQ(AO_OK, defineClass("B12Nameless", "Object", "B12-Test"));
  EXPECT_EQ("nil", printIt("B12Nameless instVarAt: 4 put: nil"));
  EXPECT_EQ("class removal refused:  is not bound to this class", tryRemoveClass("B12Nameless", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_NE(0, idOf("B12Nameless"));
}
```

`RemoveWhileBusyIsRefused` を置き換える:

```cpp
// SPEC §3.10 再入と例外: from a hook the runtime is busy: both removals and ao_accept_method_id do
// nothing, while the Browser reads (which may issue and prune IDs) still answer.
TEST_F(RemoveAbi, RemoveWhileBusyIsRefused) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12busy\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, defineClass("B12Busy", "Object", "B12-Test"));
  BusyRemove seen;
  seen.objectId = idOf("Object");
  seen.busyId = idOf("B12Busy");
  ao_set_transcript_hook(removeFromHook, &seen);
  EXPECT_EQ("7", printIt("Transcript show: 'x'. 7"));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(AO_ERR, seen.methodRc);
  EXPECT_EQ("runtime is busy", seen.methodMsg);
  EXPECT_EQ(AO_ERR, seen.classRc);
  EXPECT_EQ("runtime is busy", seen.classMsg);
  EXPECT_EQ(AO_ERR, seen.acceptRc);
  EXPECT_EQ(seen.busyId, seen.readId);
  EXPECT_EQ("1", printIt("3 b12busy"));
  EXPECT_TRUE(browserLists("B12Busy"));
}
```

`RefusedRemoveChangesNothing` の 4 つの直接の呼び出しを置き換える:

```cpp
  EXPECT_EQ(AO_ERR, ao_remove_method(idOf("Object"), 0, "printString", &err));
  EXPECT_EQ(AO_ERR, ao_remove_method(idOf("B12Kid2"), 0, "who", &err));
  EXPECT_EQ(AO_ERR, ao_remove_class(idOf("Object"), &err));
  EXPECT_EQ(AO_ERR, ao_remove_class(idOf("B12Par"), &err));
```

`LongNameCutsTheMessageAt255Bytes` を置き換える:

```cpp
// SPEC §3.10: AoSpan.message holds 255 bytes; a longer reason is cut, never emptied.
TEST_F(RemoveAbi, LongNameCutsTheMessageAt255Bytes) {
  const std::string name = "B" + std::string(299, 'N');
  ASSERT_EQ(AO_OK, defineClass(name.c_str(), "Object", "B12-Test"));
  int rc = -9;
  const std::string got = tryRemoveMethod(name.c_str(), 0, "foo", &rc);
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(255u, got.size());
  EXPECT_EQ(("selector not found: " + name + ">>foo").substr(0, 255), got);
}
```

- [ ] **Step 4: 失敗するテストを書く（新しい 3 つ）**

`runtime/tests/remove_abi_test.cpp` の末尾に足す:

```cpp
// SPEC §3.9 削除, §3.10 クラス ID: Foo goes while an alias keeps it, and a new Foo comes. The old
// class keeps its row (its own name, its ID) before the new one; each ID reads, accepts and
// removes on its own class, and the old row's removal is refused: Foo binds the new class.
TEST_F(RemoveAbi, AliasedOldClassRowActsOnItsOwnClass) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Foo", "Object", "B12-Old"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Foo", 0, "old\n  ^1\n", &err)) << err.message;
  // A class object's own printString shows its metaclass's name, "<Name> class".
  EXPECT_EQ("B12Foo class", printIt("Smalltalk at: #B12FooAlias put: B12Foo"));
  const std::int64_t oldId = idOf("B12Foo");
  ASSERT_GT(oldId, 0);
  EXPECT_EQ(oldId, idOf("B12FooAlias"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(oldId, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Foo"));
  EXPECT_EQ(0, idOf("B12Foo"));
  EXPECT_EQ(oldId, idOf("B12FooAlias"));
  ASSERT_EQ(AO_OK, defineClass("B12Foo", "Object", "B12-New"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Foo", 0, "new\n  ^2\n", &err)) << err.message;
  const std::int64_t newId = idOf("B12Foo");
  ASSERT_GT(newId, oldId);

  // Two rows named B12Foo: the old one first (smaller ID), each in its own category.
  std::vector<std::pair<std::int64_t, std::string>> rows;
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    std::int64_t id = 0;
    char name[128];
    char category[128];
    ASSERT_EQ(AO_OK, ao_browser_class_at(i, &id, name, sizeof(name), category, sizeof(category)));
    if (std::strcmp(name, "B12Foo") == 0) {
      rows.emplace_back(id, category);
    }
  }
  ASSERT_EQ(2u, rows.size());
  EXPECT_EQ(std::make_pair(oldId, std::string("B12-Old")), rows[0]);
  EXPECT_EQ(std::make_pair(newId, std::string("B12-New")), rows[1]);

  // Each ID reads its own class.
  char buf[256];
  ASSERT_EQ(AO_OK, ao_browser_selector_at(oldId, 0, "user", 0, buf, sizeof(buf)));
  EXPECT_STREQ("old", buf);
  ASSERT_EQ(AO_OK, ao_browser_selector_at(newId, 0, "user", 0, buf, sizeof(buf)));
  EXPECT_STREQ("new", buf);

  // The old row cannot go: B12Foo binds the new class, which stays.
  EXPECT_EQ("class removal refused: B12Foo is not bound to this class",
            tryRemoveClassId(oldId, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("2", printIt("B12Foo new new"));
  EXPECT_EQ(newId, idOf("B12Foo"));

  // Accept and method removal on the old row reach the old class only.
  ASSERT_EQ(AO_OK, ao_accept_method_id(oldId, 0, "extra\n  ^3\n", &err)) << err.message;
  EXPECT_EQ("3", printIt("B12FooAlias new extra"));
  expectDnu("B12Foo new extra", "extra");
  EXPECT_EQ("", tryRemoveMethodId(oldId, 0, "old", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12FooAlias new old", "old");
  EXPECT_EQ("selector not found: B12Foo>>old", tryRemoveMethodId(newId, 0, "old", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("2", printIt("B12Foo new new"));

  // The new row goes; the alias keeps the old class and its row.
  EXPECT_EQ("", tryRemoveClassId(newId, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Foo"));
  EXPECT_EQ("3", printIt("B12FooAlias new extra"));
  EXPECT_EQ(oldId, idOf("B12FooAlias"));
  EXPECT_EQ(-1, ao_browser_protocol_count(newId, 0));
}

// SPEC §3.9 削除, §3.10 クラス ID: an unknown ID (0 or less, never issued, dropped) is refused
// with "unknown class id" by both removals and ao_accept_method_id, and nothing changes. A bad
// meta or a NULL selector or source keeps its old answer.
TEST_F(RemoveAbi, UnknownClassIdIsRefusedByRemoveAndAccept) {
  ASSERT_EQ(AO_OK, defineClass("B12Stale", "Object", "B12-Test"));
  const std::int64_t stale = idOf("B12Stale");
  ASSERT_GT(stale, 0);
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(stale, &rc));
  EXPECT_EQ(AO_OK, rc);
  for (const std::int64_t id :
       {std::int64_t{0}, std::int64_t{-3}, stale, std::numeric_limits<std::int64_t>::max()}) {
    SCOPED_TRACE(id);
    EXPECT_EQ("unknown class id", tryRemoveClassId(id, &rc));
    EXPECT_EQ(AO_ERR, rc);
    EXPECT_EQ("unknown class id", tryRemoveMethodId(id, 0, "printString", &rc));
    EXPECT_EQ(AO_ERR, rc);
    AoSpan err{};
    EXPECT_EQ(AO_ERR, ao_accept_method_id(id, 0, "b12staleFoo\n  ^1\n", &err));
    EXPECT_STREQ("unknown class id", err.message);
  }
  EXPECT_EQ("remove failed", tryRemoveMethodId(idOf("Object"), 2, "foo", &rc));
  EXPECT_EQ("remove failed", tryRemoveMethodId(idOf("Object"), 0, nullptr, &rc));
  AoSpan err{};
  EXPECT_EQ(AO_ERR, ao_accept_method_id(idOf("Object"), 2, "b12staleFoo\n  ^1\n", &err));
  EXPECT_EQ(AO_ERR, ao_accept_method_id(idOf("Object"), 0, nullptr, &err));
  expectDnu("Object new b12staleFoo", "b12staleFoo");
}

// SPEC §3.9 削除, §3.10 クラス ID: the removed kid stays in the ID table until the next list; it
// does not count as a live subclass of its superclass.
TEST_F(RemoveAbi, ClassIdTableDoesNotBlockSuperclassRemoval) {
  ASSERT_EQ(AO_OK, defineClass("B12TabPar", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12TabKid", "B12TabPar", "B12-Test"));
  const std::int64_t par = idOf("B12TabPar");
  const std::int64_t kid = idOf("B12TabKid");
  int rc = -9;
  EXPECT_EQ("", tryRemoveClassId(kid, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("", tryRemoveClassId(par, &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12TabPar"));
}
```

`runtime/tests/accept_abi_test.cpp` の末尾に足す:

```cpp
// SPEC §3.10: ao_accept_method_id is ao_accept_method for the class the ID names: the native
// overwrite rules (Kernel-ness by the class itself, both sides), compile errors, the source table.
TEST(AcceptAbi, AcceptMethodIdFollowsAcceptMethodRules) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  char out[64];
  const std::int64_t smallInt = ao_browser_class_id("SmallInteger");
  ASSERT_GT(smallInt, 0);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method_id(smallInt, 0, "<= x\n  ^false\n", &err));
  EXPECT_STREQ("native selector overwrite refused: <=", err.message);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method_id(smallInt, 1, "new\n  ^3\n", &err));
  EXPECT_STREQ("native selector overwrite refused: new", err.message);
  EXPECT_EQ(AO_ERR_COMPILE, ao_accept_method_id(smallInt, 0, "foo\n  ^\n", &err));
  EXPECT_STRNE("", err.message);
  const std::int64_t object = ao_browser_class_id("Object");
  ASSERT_EQ(AO_OK, ao_accept_method_id(object, 0, "b12idFoo\n  ^1\n", &err)) << err.message;
  EXPECT_STREQ("", err.message);
  ASSERT_EQ(AO_OK, ao_eval("3 b12idFoo", 10, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("1", out);
  char shown[64];
  ASSERT_EQ(AO_OK, ao_browser_source(object, 0, "b12idFoo", shown, sizeof(shown)));
  EXPECT_STREQ("b12idFoo\n  ^1\n", shown);
  ASSERT_EQ(AO_OK, ao_accept_method_id(object, 1, "b12idMake\n  ^7\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_eval("Object b12idMake", 16, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("7", out);
  ao_runtime_shutdown();
}
```

- [ ] **Step 5: 失敗を確かめる**

Run: `cmake --build build`
Expected: FAIL（`ao_remove_method(int64_t…)`、`ao_accept_method_id` が無い）。

- [ ] **Step 6: Compile.cpp**

無名名前空間の `isClassObject` のあとに足す（`insert_after_symbol`）:

```cpp
// A class or metaclass: class-shaped, and Behavior is in its class's superclass chain (a class
// through its metaclass, a metaclass through Metaclass). Processor, Smalltalk and nil are not.
bool isBehavior(CallContext& ctx, Oop obj) {
  return isClassShaped(ctx.heap, obj) &&
         chainIncludes(ctx.heap, ctx.heap.klass(obj), ctx.wk.behaviorClass);
}
```

`namesBehavior` の本体を置き換える:

```cpp
bool namesBehavior(CallContext& ctx, std::string_view className) {
  // SPEC §3.10: Processor, Smalltalk, nil and other globals are not Behaviors.
  return isBehavior(ctx, ctx.wk.named(className));
}
```

`acceptMethodSource` を置き換え、そのあとに `acceptMethodInto` を足す:

```cpp
bool acceptMethodSource(CallContext& ctx, std::string_view className, bool meta,
                        std::string_view source, compiler::CompileError* error) {
  if (error != nullptr) {
    *error = {};
  }
  if (!namesBehavior(ctx, className)) {
    assignError(error, "missing class: " + std::string(className));
    return false;
  }
  return acceptMethodInto(ctx, ctx.wk.named(className), meta, source, error);
}

bool acceptMethodInto(CallContext& ctx, Oop target, bool meta, std::string_view source,
                      compiler::CompileError* error) {
  if (error != nullptr) {
    *error = {};
  }
  // Rooted before anything below can allocate.
  Root cls(ctx.roots, target);
  const std::string name = isClassShaped(ctx.heap, cls.slot) ? ownClassName(ctx, cls.slot) : "";
  if (!isBehavior(ctx, cls.slot)) {
    assignError(error, "missing class: " + name);
    return false;
  }
  const Oop side = meta ? ctx.heap.klass(cls.slot) : cls.slot;
  if (!side.isHeap()) {
    assignError(error, "missing class: " + name);
    return false;
  }
  Root tgt(ctx.roots, side);
  compiler::CompileEnv env;
  fillInstVars(ctx, tgt.slot, env);
  fillClassVars(ctx, tgt.slot, env);
  compiler::CompileResult cr = compiler::compileMethod(source, env);
  if (!cr.ok) {
    if (error != nullptr) {
      *error = std::move(cr.error);
    }
    return false;
  }

  Root old(ctx.roots, Oop::nil());
  bool findsNative = false;
  {
    const Oop dict = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
    const Oop sel = ctx.wk.intern(cr.image.selector);
    if (dict.isHeap() && sel.isHeap()) {
      old.slot = MethodDictionary::at(ctx.heap, dict, sel);
    }
    // SPEC §3.10: in a Kernel class, a native the selector finds through the superclasses is
    // not hidden either. Kernel-ness is the class itself (named or by ID), so an alias is one too.
    // Neither intern nor lookup GCs, so the raw Oops stay valid.
    const Oop found = sel.isHeap() && isKernelClass(ctx.wk, cls.slot)
                          ? lookup(ctx.heap, tgt.slot, sel)
                          : old.slot;
    findsNative = found.isHeap() && ctx.heap.klass(found) == ctx.wk.nativeMethodClass;
  }
  if (findsNative) {
    assignError(error, "native selector overwrite refused: " + cr.image.selector);
    return false;
  }
  if (const std::string var = unboundClassVariable(ctx, cr.image, tgt.slot); !var.empty()) {
    assignError(error, unboundClassVariableMessage(var));
    return false;
  }

  Root text(ctx.roots, boxUtf8(ctx, source));
  if (!text.slot.isHeap()) {
    assignError(error, "method source allocation failed");
    return false;
  }
  Root kept(ctx.roots, installMethod(ctx, tgt.slot, cr.image));
  if (!kept.slot.isHeap()) {
    assignError(error, "install failed");
    return false;
  }
  const Oop selNow = ctx.heap.slotAt(kept.slot, kCmSlotSelector);
  const Oop dictNow = ctx.heap.slotAt(tgt.slot, kClassSlotMethodDict);
  if (!dictNow.isHeap() || !selNow.isHeap() ||
      MethodDictionary::at(ctx.heap, dictNow, selNow) != kept.slot) {
    assignError(error, "install failed");
    return false;
  }
  const Oop replaced = old.slot.isHeap() ? old.slot : Oop{};
  // SPEC §3.10, §3.13: kept was boxed from cr.image, so its blocks sit at the image's literal
  // indices.
  rememberMethodSource(kept.slot, text.slot, replaced, &cr.image);
  return true;
}
```

`removeMethodNamed` と `removeClassNamed` を置き換える（名前も変わる。`replace_symbol_body`）:

```cpp
bool removeMethodOf(CallContext& ctx, Oop cls, bool meta, std::string_view selector,
                    std::string* reason) {
  const std::string name = isClassShaped(ctx.heap, cls) ? ownClassName(ctx, cls) : "";
  if (!isBehavior(ctx, cls)) {
    *reason = "not a class: " + name;
    return false;
  }
  const Oop side = meta ? ctx.heap.klass(cls) : cls;
  const std::string where = removedMethodName(name, meta, selector);
  // SPEC §3.3: MethodDictionary::at and removeKey read a slot that is no dictionary
  // (instVarAt:put:) as holding no selector, without reading out of range.
  const Oop dict =
      isClassShaped(ctx.heap, side) ? ctx.heap.slotAt(side, kClassSlotMethodDict) : Oop::nil();
  // findSymbol allocates nothing; an unknown selector is one no dictionary can hold.
  const Oop sel = ctx.wk.findSymbol(selector);
  const Oop method = sel.isHeap() ? MethodDictionary::at(ctx.heap, dict, sel) : Oop::nil();
  if (!method.isHeap()) {
    *reason = "selector not found: " + where;
    return false;
  }
  if (ctx.heap.klass(method) == ctx.wk.nativeMethodClass) {
    *reason = "native method removal refused: " + where;
    return false;
  }
  if (!MethodDictionary::removeKey(ctx.heap, dict, sel)) {
    *reason = "remove failed";
    return false;
  }
  // SPEC §3.3: the one function every method change goes through. Nothing above collected.
  invalidateMethodCache(ctx.cache, sel);
  forgetMethodSource(method);
  return true;
}

bool removeClassOf(CallContext& ctx, Oop cls, std::string* reason) {
  const std::string name = isClassShaped(ctx.heap, cls) ? ownClassName(ctx, cls) : "";
  if (!isClassObject(ctx, cls)) {
    *reason = "not a class: " + name;
    return false;
  }
  const std::string refused = "class removal refused: " + name;
  if (ctx.wk.isFixedGlobal(name)) {
    *reason = refused + " is a fixed global";
    return false;
  }
  if (isKernelClass(ctx.wk, cls)) {
    *reason = refused + " is a kernel class";
    return false;
  }
  // SPEC §3.9 削除: only the class's own name is unbound, and only while it binds this class; an
  // old class only an alias keeps, or a nameless one, is refused.
  if (ctx.wk.named(name) != cls) {
    *reason = refused + " is not bound to this class";
    return false;
  }
  const SubclassProbe sub = probeSubclasses(ctx, cls);
  if (sub.any) {
    *reason = sub.name.empty() ? refused + " has an unnamed subclass"
                               : refused + " has subclass " + sub.name;
    return false;
  }
  if (!ctx.wk.undefine(name)) {
    *reason = "remove failed";
    return false;
  }
  // SPEC §3.3: as a class replacement, the whole cache goes. Nothing above collected, so cls is
  // still the class object.
  invalidateMethodCache(ctx.cache, Oop{});
  forgetMethodSourcesOf(ctx, cls);
  forgetMethodSourcesOf(ctx, ctx.heap.klass(cls));
  return true;
}
```

`runtime/include/ao/Compile.hpp` の `acceptMethodSource` の宣言のあとに足し、`removeMethodNamed` / `removeClassNamed` の宣言とコメントを置き換える:

```cpp
// SPEC §3.10 ao_accept_method_id: acceptMethodSource for the class cls (a class or metaclass;
// Kernel-ness is cls itself). "missing class: <name>" when cls is no Behavior.
bool acceptMethodInto(CallContext& ctx, Oop cls, bool meta, std::string_view source,
                      compiler::CompileError* error);
```

```cpp
// SPEC §3.9 削除. cls is the class a Browser class ID names. Every check runs before anything
// changes, and nothing here allocates on the heap (the selector is looked up, never interned).
// False with the reason, never empty, in *reason: "not a class: <name>" (cls is no Behavior),
// "selector not found: <Class>>><selector>" (an inherited or unknown selector, or a malformed
// dictionary), "native method removal refused: <Class>>><selector>"; <name> is cls's own name
// slot, <Class> that with " class" after it when meta. True: the method is out of its side's
// dictionary, the cache is invalidated for the selector, and its source entry is gone.
bool removeMethodOf(CallContext& ctx, Oop cls, bool meta, std::string_view selector,
                    std::string* reason);

// SPEC §3.9 削除. cls is the class a Browser class ID names; <name> is its own name slot. Every
// check runs before anything changes; nothing here allocates on the heap. False with the reason in
// *reason: "not a class: <name>", "class removal refused: <name> is a fixed global", "... is a
// kernel class" (by identity), "... is not bound to this class" (Smalltalk's <name> is another
// object), "... has subclass <Sub>" (the smallest name of its live subclasses) or "... has an
// unnamed subclass". True: <name>'s binding is out of Smalltalk (the globals version moved), the
// whole cache is dropped, and the source entries of the class's methods on both sides are gone.
// The class object, its metaclass, its dictionaries and its aliases are untouched.
bool removeClassOf(CallContext& ctx, Oop cls, std::string* reason);
```

- [ ] **Step 7: `sessionClassForId`**

`runtime/src/Session.hpp` の `browserClassId` の宣言のあとに足す:

```cpp
// SPEC §3.10 クラス ID: the class id names in the current session, or the empty Oop (no session,
// an unknown ID). Rebuilds the class list, so it may issue and drop IDs and may throw
// std::bad_alloc. Allocates nothing on the heap.
Oop sessionClassForId(std::int64_t id);
```

`runtime/src/Session.cpp` の `browserClassId` のあとに足す:

```cpp
Oop sessionClassForId(std::int64_t id) {
  Session* s = session();
  if (s == nullptr) {
    return Oop{};
  }
  const auto rows = classRows(*s);
  const ClassRow* row = findClassId(rows, id);
  return row == nullptr ? Oop{} : row->cls;
}
```

- [ ] **Step 8: ABI**

`bridge/ao_abi.h` の busy のコメントを置き換える:

```c
/* SPEC §3.10. The runtime is busy while ao_runtime_boot, ao_runtime_shutdown, ao_image_save,
   ao_image_load, ao_filein_load_order, ao_workspace_reset, ao_eval, ao_accept_method,
   ao_accept_method_id, ao_accept_class, ao_remove_method, ao_remove_class,
   ao_debug_frame_receiver_print, ao_debug_frame_temp_print, ao_debug_inspect,
   ao_debug_clear, ao_debug_proceed, ao_debug_step_into, ao_debug_step_over, ao_debug_step_out
   or ao_debug_abort runs, or the interpreter does. Called then (from a transcript or inspect
   hook, or a native), each of these twenty-one does nothing and answers AO_ERR: ao_image_load,
   ao_remove_method and ao_remove_class
   with the reason "runtime is busy", ao_eval with an empty out. The running evaluation goes on. The hook
   setters, ao_version, the ao_browser_* reads (ao_browser_class_id too), ao_eval_result_length,
   ao_eval_result_copy,
   ao_set_debug_capture, ao_set_debug_mode, the snapshot reads (ao_debug_generation to
   ao_debug_frame_temp_name) and the live reads (ao_debug_halted_pid to ao_debug_select) may be
   called then. A halted process (SPEC §3.13) does not run, so it alone does not make the runtime
   busy. No C++ exception leaves any of these functions: it becomes AO_ERR (-1 for
   the *_count functions, ao_eval_result_length, and the ao_debug_* functions below that answer a
   number). */
```

`ao_accept_method` の宣言のあとに足し、2 つの削除の宣言とコメントを置き換える:

```c
/* SPEC §3.10 クラス ID. ao_accept_method for the class class_id names (the Browser's Accept):
   the same rules, answers and messages, Kernel-ness by that class itself. An unknown class ID is
   AO_ERR with "unknown class id". */
int ao_accept_method_id(int64_t class_id, int meta, const char* source, AoSpan* err);
```

```c
/* SPEC §3.9 削除. AO_OK: the method is out of the side's dictionary (meta 1: the metaclass's),
   the cache is invalidated and its source is forgotten; err's message is empty. AO_ERR with the
   reason in err (never empty; start and end 0): "unknown class id", "not a class: <name>",
   "selector not found: <Class>>><selector>" (unknown or inherited), "native method removal
   refused: <Class>>><selector>", "runtime is busy", or "remove failed" (no session, a NULL
   selector, meta other than 0 or 1). <name> is the class's own name slot, <Class> that with
   " class" when meta. Nothing changes on AO_ERR. Allocates nothing on the heap. */
int ao_remove_method(int64_t class_id, int meta, const char* selector, AoSpan* err);

/* SPEC §3.9 削除. AO_OK: the binding of the class's own name is out of Smalltalk (an alias stays;
   the class object and its instances are untouched), the whole method cache is dropped, and the
   sources of its methods are forgotten; err's message is empty. AO_ERR with the reason: "unknown
   class id", "not a class: <name>", "class removal refused: <name> is a fixed global" / "is a
   kernel class" (by identity) / "is not bound to this class" (its own name binds something else)
   / "has subclass <Sub>" / "has an unnamed subclass", "runtime is busy", or "remove failed" (no
   session). Nothing changes on AO_ERR. Allocates nothing on the heap. */
int ao_remove_class(int64_t class_id, AoSpan* err);
```

`runtime/src/abi.cpp` の `ao_accept_method` のあとに足し、2 つの削除を置き換える:

```cpp
extern "C" int ao_accept_method_id(int64_t class_id, int meta, const char* source, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    return AO_ERR;
  }
  return guarded(AO_ERR, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr || source == nullptr || (meta != 0 && meta != 1)) {
      return AO_ERR;
    }
    // SPEC §3.10 クラス ID: resolving rebuilds the class list; nothing collects before
    // acceptMethodInto roots the class.
    const ao::Oop cls = ao::sessionClassForId(class_id);
    if (cls.isEmpty()) {
      setMessage(err, "unknown class id");
      return AO_ERR;
    }
    ao::compiler::CompileError error;
    if (!ao::acceptMethodInto(*s->ctx, cls, meta == 1, source, &error)) {
      fillSpan(err, error);
      return AO_ERR_COMPILE;
    }
    return AO_OK;
  });
}
```

```cpp
extern "C" int ao_remove_method(int64_t class_id, int meta, const char* selector, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  std::string reason;
  const int rc = guarded(-1, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr || selector == nullptr || (meta != 0 && meta != 1)) {
      return AO_ERR;
    }
    // SPEC §3.10 クラス ID: resolving rebuilds the class list; nothing here allocates on the heap.
    const ao::Oop cls = ao::sessionClassForId(class_id);
    if (cls.isEmpty()) {
      reason = "unknown class id";
      return AO_ERR;
    }
    return ao::removeMethodOf(*s->ctx, cls, meta == 1, selector, &reason) ? AO_OK : AO_ERR;
  });
  if (rc == AO_OK) {
    return AO_OK;
  }
  // SPEC §3.9 削除: an AO_ERR says why, never with an empty message (as ao_image_load).
  setMessage(err, rc == -1 || reason.empty() ? std::string_view("remove failed") : reason);
  return AO_ERR;
}

extern "C" int ao_remove_class(int64_t class_id, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  std::string reason;
  const int rc = guarded(-1, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr) {
      return AO_ERR;
    }
    const ao::Oop cls = ao::sessionClassForId(class_id);
    if (cls.isEmpty()) {
      reason = "unknown class id";
      return AO_ERR;
    }
    return ao::removeClassOf(*s->ctx, cls, &reason) ? AO_OK : AO_ERR;
  });
  if (rc == AO_OK) {
    return AO_OK;
  }
  setMessage(err, rc == -1 || reason.empty() ? std::string_view("remove failed") : reason);
  return AO_ERR;
}
```

- [ ] **Step 9: Swift の削除を名前 → ID でつなぐ（Task 5 で書き直す）**

`app/Ao/BrowserWindow.swift` の `performRemoveMethod` の ABI 呼び出しを置き換える:

```swift
    let classID = BrowserModel.classID(named: className)
    let status = selector.withCString { sel in
      withUnsafeMutablePointer(to: &err) { errPtr in
        ao_remove_method(classID, metaFlag, sel, errPtr)
      }
    }
```

`performRemoveClass` の ABI 呼び出しを置き換える:

```swift
    let classID = BrowserModel.classID(named: className)
    let status = withUnsafeMutablePointer(to: &err) { errPtr in
      ao_remove_class(classID, errPtr)
    }
```

- [ ] **Step 10: 通ることを確かめる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'RemoveAbi|RemoveUnits|AcceptAbi|BrowserAbi'`
Expected: PASS。

Run: `swift test --package-path app -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" -Xlinker -lc++ --filter 'AcceptTests|BrowserModelTests'`
Expected: PASS（`testRefusedRemoveShowsReason` の `class removal refused: Object is a fixed global` は ID でも同じ）。

- [ ] **Step 11: Commit**

```bash
git add runtime/include/ao/Compile.hpp runtime/src/Compile.cpp runtime/src/Session.hpp runtime/src/Session.cpp bridge/ao_abi.h runtime/src/abi.cpp app/Ao/BrowserWindow.swift runtime/tests/remove_abi_test.cpp runtime/tests/accept_abi_test.cpp
git commit -m "Accept and remove through a class ID

SPEC §3.9 削除, §3.10 クラス ID: ao_remove_method and ao_remove_class take a class ID and
name the class by its own name slot; removing a class whose own name binds another class
(an old class only an alias keeps) is refused as not bound to this class. The new
ao_accept_method_id is ao_accept_method for the class an ID names. Unknown IDs are
refused with unknown class id.

Graphify: path ao_remove_class removeClassNamed()
Serena: replace_symbol_body removeMethodNamed, removeClassNamed, acceptMethodSource, namesBehavior; insert_after_symbol isClassObject

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 5: Browser の行と選択を ID で持つ

**Files:**
- Modify: `app/Ao/BrowserModel.swift`（全体を置き換える）
- Modify: `app/Ao/BrowserWindow.swift`（選択、階層、削除、Accept、行の選択）
- Test: `app/AoTests/AcceptTests.swift`、`app/AoTests/BrowserModelTests.swift`

**Interfaces:**
- Consumes: C の `ao_browser_class_id`、`ao_browser_class_at`（ID つき）、ID で引く読み出し、`ao_accept_method_id`、`ao_remove_method`、`ao_remove_class`
- Produces:
  - `struct BrowserClass: Equatable { let id: Int64; let name: String }`
  - `BrowserModel.classRows: [BrowserClass]`、`classes: [String]`（名前）、`selectedClassID: Int64?`、`selectedClass: String?`（名前）、`selectedClassRow: BrowserClass?`
  - `BrowserModel.select(category:classID:meta:protocol:selector:)`、`hierarchy(of:meta:) -> [BrowserClass]`、`applyHierarchyList(_:selecting:)`、`category(ofClassID:) -> String?`、`static classID(named:) -> Int64`

- [ ] **Step 1: Serena で参照を取る**

`find_referencing_symbols` で `BrowserModel/select`、`BrowserModel/hierarchyNames`、`BrowserModel/applyHierarchyList`、`BrowserModel/category`、`BrowserModel/selectedClass`、`BrowserModel/classes`（`BrowserWindow`、`AcceptTests`、`BrowserModelTests`、`ToolWindowTests` の使い方を確かめる。`classes` と `selectedClass` は名前の読み取りとして残る）。

- [ ] **Step 2: 失敗するテストを書く**

`app/AoTests/BrowserModelTests.swift` の既存の `model.select(category:className:…)` の 7 か所を `classID:` に直す（`className: "Object"` → `classID: BrowserModel.classID(named: "Object")`）。末尾（`vendorLoadOrderPath` などのヘルパの前）に足す:

```swift
  // SPEC §3.10 クラス ID: one row per class, however many names bind it, each with a positive ID;
  // the selection is the ID, and a class that leaves the list (a shape change) gives way to the
  // class now bound to its name.
  func testClassRowsCarryOneIdPerClass() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    XCTAssertEqual(defineRow("B12Row", instanceVariables: ""), Int32(AO_OK))
    XCTAssertEqual(doIt("Smalltalk at: #B12RowAlias put: B12Row"), Int32(AO_OK))
    let id = BrowserModel.classID(named: "B12Row")
    XCTAssertGreaterThan(id, 0)
    XCTAssertEqual(BrowserModel.classID(named: "B12RowAlias"), id)
    XCTAssertEqual(BrowserModel.classID(named: "B12NoSuchRow"), 0)
    model.select(category: "B12-Row", classID: id, meta: false, protocol: nil)
    XCTAssertEqual(model.classRows, [BrowserClass(id: id, name: "B12Row")])
    XCTAssertEqual(model.selectedClassID, id)
    XCTAssertEqual(model.selectedClass, "B12Row")
    XCTAssertTrue(model.source.contains("subclass: #B12Row"))

    XCTAssertEqual(defineRow("B12Shape", instanceVariables: ""), Int32(AO_OK))
    let shapeID = BrowserModel.classID(named: "B12Shape")
    model.select(category: "B12-Row", classID: shapeID, meta: false, protocol: nil)
    XCTAssertEqual(model.selectedClass, "B12Shape")
    XCTAssertEqual(defineRow("B12Shape", instanceVariables: "a"), Int32(AO_OK))
    model.refresh()
    let reshaped = BrowserModel.classID(named: "B12Shape")
    XCTAssertNotEqual(reshaped, shapeID)
    XCTAssertEqual(model.selectedClassID, reshaped)
    XCTAssertEqual(model.selectedClass, "B12Shape")
    XCTAssertTrue(model.source.contains("instanceVariableNames: 'a'"))

    model.select(category: "Kernel", classID: nil, meta: false, protocol: nil)
    let ids = model.classRows.map(\.id)
    XCTAssertEqual(Set(ids).count, ids.count)
    XCTAssertTrue(ids.allSatisfy { $0 > 0 })
    XCTAssertNil(model.selectedClassID)
    XCTAssertNil(model.selectedClass)
  }

  private func defineRow(_ name: String, instanceVariables: String) -> Int32 {
    var err = AoSpan()
    let def = "Object subclass: #\(name)\n  instanceVariableNames: '\(instanceVariables)'\n"
      + "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'B12-Row'\n"
    return def.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
  }

  private func doIt(_ source: String) -> Int32 {
    var out = [CChar](repeating: 0, count: 64)
    var err = AoSpan()
    return source.withCString { src in
      out.withUnsafeMutableBufferPointer { buffer in
        withUnsafeMutablePointer(to: &err) { errPtr in
          ao_eval(src, Int32(source.utf8.count), Int32(AO_EVAL_DOIT), buffer.baseAddress,
                  Int32(buffer.count), errPtr)
        }
      }
    }
  }
```

`app/AoTests/AcceptTests.swift` の `testRefusedRemoveShowsReason` のあとに足す:

```swift
  // SPEC §3.9 削除, §3.10 クラス ID: after B12Foo goes and an alias keeps the old class, a new
  // B12Foo makes two rows named B12Foo. Each row reads, accepts and removes on its own class; the
  // old row's Remove Class… is refused because its name no longer binds it.
  func testAliasedOldClassRowActsOnItsOwnClass() {
    var err = AoSpan()
    XCTAssertEqual(acceptClass("B12Foo", category: "B12-Old"), Int32(AO_OK))
    let old = "old\n  ^1\n".withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_method("B12Foo", 0, src, $0) }
    }
    XCTAssertEqual(old, Int32(AO_OK), spanMessage(err))
    // A class object prints as its metaclass's name, "<Name> class".
    XCTAssertEqual(printIt("Smalltalk at: #B12FooAlias put: B12Foo"), "B12Foo class")
    let browser = BrowserWindow()
    defer { browser.window.close() }
    var asked: [String] = []
    browser.confirmRemove = { _, message, decide in
      asked.append(message)
      decide(true)
    }
    selectCategory("B12-Old", in: browser)
    selectClass("B12Foo", in: browser)
    let oldID = browser.model.selectedClassID
    XCTAssertNotNil(oldID)
    browser.removeClass()
    XCTAssertEqual(asked, ["Remove class B12Foo?"])
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("B12Foo"), "nil")
    // The alias keeps the old class listed under its own name, with the same ID.
    XCTAssertEqual(browser.model.classes, ["B12Foo"])
    XCTAssertEqual(browser.model.classRows.first?.id, oldID)

    XCTAssertEqual(acceptClass("B12Foo", category: "B12-New"), Int32(AO_OK))
    let new = "new\n  ^2\n".withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_method("B12Foo", 0, src, $0) }
    }
    XCTAssertEqual(new, Int32(AO_OK), spanMessage(err))

    // The Old row is the old class.
    selectClass("B12Foo", in: browser)
    XCTAssertEqual(browser.model.selectedClassID, oldID)
    XCTAssertTrue(browser.sourceText.contains("category: 'B12-Old'"))
    selectProtocol("user", in: browser)
    XCTAssertEqual(browser.model.selectors, ["old"])

    // Its Remove Class… is refused; the new B12Foo stays.
    browser.removeClass()
    XCTAssertEqual(asked.last, "Remove class B12Foo?")
    XCTAssertEqual(browser.errorText, "class removal refused: B12Foo is not bound to this class")
    XCTAssertEqual(printIt("B12Foo new new"), "2")

    // Accept and Remove Method… go to the old class.
    browser.replaceSource("extra\n  ^3\n")
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("B12FooAlias new extra"), "3")
    XCTAssertNil(printIt("B12Foo new extra"))
    selectSelector("old", in: browser)
    browser.removeMethod()
    XCTAssertEqual(asked.last, "Remove B12Foo>>old?")
    XCTAssertEqual(browser.errorText, "")
    XCTAssertNil(printIt("B12FooAlias new old"))
    XCTAssertEqual(printIt("B12Foo new new"), "2")

    // The New row is the new class; removing it leaves the alias alone.
    selectCategory("B12-New", in: browser)
    selectClass("B12Foo", in: browser)
    XCTAssertNotEqual(browser.model.selectedClassID, oldID)
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("B12Foo"), "nil")
    XCTAssertEqual(printIt("B12FooAlias new extra"), "3")
  }

  // SPEC §3.9 System Browser, §3.10 クラス ID: a load starts new IDs; the Browser selects the class
  // now bound to the selected row's name, and Remove Class… acts on it.
  func testImageLoadReselectsTheClassByName() {
    XCTAssertEqual(acceptClass("B12Img", category: "B12-Img"), Int32(AO_OK))
    let browser = BrowserWindow()
    defer { browser.window.close() }
    browser.confirmRemove = { _, _, decide in decide(true) }
    selectCategory("B12-Img", in: browser)
    selectClass("B12Img", in: browser)
    let before = browser.model.selectedClassID
    let path = FileManager.default.temporaryDirectory
      .appendingPathComponent("b12-img-\(UUID().uuidString).aoimage").path
    defer { try? FileManager.default.removeItem(atPath: path) }
    XCTAssertEqual(path.withCString { ao_image_save($0) }, Int32(AO_OK))
    var err = AoSpan()
    let loaded = path.withCString { file in
      withUnsafeMutablePointer(to: &err) { ao_image_load(file, $0) }
    }
    XCTAssertEqual(loaded, Int32(AO_OK), spanMessage(err))
    browser.noteImageLoaded()
    XCTAssertEqual(browser.model.selectedClass, "B12Img")
    XCTAssertNotEqual(browser.model.selectedClassID, before)
    XCTAssertEqual(browser.model.selectedClassID, BrowserModel.classID(named: "B12Img"))
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("B12Img"), "nil")
  }
```

`AcceptTests` の `selectClass` の前に足す:

```swift
  // Defines `Object subclass: #name` in category through ao_accept_class.
  private func acceptClass(_ name: String, category: String) -> Int32 {
    var err = AoSpan()
    let def = "Object subclass: #\(name)\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: '\(category)'\n"
    return def.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
  }
```

- [ ] **Step 3: 失敗を確かめる**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" -Xlinker -lc++ --filter 'AcceptTests|BrowserModelTests'`
Expected: FAIL（`BrowserClass`、`classRows`、`selectedClassID`、`select(category:classID:…)` が無い）。

- [ ] **Step 4: `BrowserModel.swift` を置き換える**

```swift
@_exported import CAo

// SPEC §3.10 クラス ID: one class-list row. The ID names the class in every Browser ABI call; the
// name is only shown (two rows may share one).
struct BrowserClass: Equatable {
  let id: Int64
  let name: String
}

@MainActor
final class BrowserModel {
  // SPEC §3.10: an accepted method is a CompiledMethod, so it lands in this protocol.
  static let newMethodProtocol = "user"

  private(set) var categories: [String] = []
  private(set) var classRows: [BrowserClass] = []
  private(set) var protocols: [String] = []
  private(set) var selectors: [String] = []
  private(set) var source: String = ""
  // The selected method has no source (AO_ERR_NOSOURCE): `source` is the runtime's placeholder.
  private(set) var sourceIsPlaceholder = false

  private var didBoot = false
  private var selectedCategory: String?
  // SPEC §3.9 System Browser: the selection is the class ID. The name is the one its row showed
  // last, to find the class again when the ID leaves the list.
  private(set) var selectedClassID: Int64?
  private var selectedClassName: String?
  private var selectedMeta = false
  private(set) var selectedProtocol: String?
  private(set) var selectedSelector: String?

  // The rows' names, in list order.
  var classes: [String] {
    classRows.map(\.name)
  }

  // The selected row's name; nil when no class is selected.
  var selectedClass: String? {
    selectedClassID == nil ? nil : selectedClassName
  }

  var selectedClassRow: BrowserClass? {
    guard let selectedClassID, let selectedClassName else {
      return nil
    }
    return BrowserClass(id: selectedClassID, name: selectedClassName)
  }

  // SPEC §3.10 ao_browser_class_id: the ID of the class Smalltalk binds to name; 0 when none.
  static func classID(named name: String) -> Int64 {
    name.withCString { ao_browser_class_id($0) }
  }

  func boot() -> Int32 {
    if didBoot {
      return Int32(AO_OK)
    }
    let rc = ao_runtime_boot()
    if rc == Int32(AO_OK) {
      didBoot = true
    }
    return rc
  }

  func refresh() {
    let loaded = loadClasses()
    categories = uniqueCategories(loaded)
    if let selectedCategory {
      classRows = loaded.filter { $0.category == selectedCategory }.map(\.row)
    } else {
      classRows = loaded.map(\.row)
    }
    reselectClass()
    protocols = loadProtocols()
    if let current = selectedProtocol, !protocols.contains(current) {
      selectedProtocol = nil
    }
    selectors = loadSelectors()
    if let current = selectedSelector, !selectors.contains(current) {
      selectedSelector = nil
    }
    (source, sourceIsPlaceholder) = loadSource()
  }

  // classID nil: no class is selected (after Remove Class…); refresh keeps it nil.
  func select(
    category: String,
    classID: Int64?,
    meta: Bool,
    protocol protocolName: String?,
    selector: String? = nil
  ) {
    selectedCategory = category
    if classID != selectedClassID {
      selectedClassName = nil
    }
    selectedClassID = classID
    selectedMeta = meta
    selectedProtocol = protocolName
    selectedSelector = selector
    refresh()
  }

  // The superclass chain up from `start` (at most 64 classes, stopping at a class that is not
  // listed or seen before), root first, then start's subclasses.
  func hierarchy(of start: BrowserClass, meta: Bool) -> [BrowserClass] {
    var chain: [BrowserClass] = []
    var current = start
    while current.id > 0, !chain.contains(where: { $0.id == current.id }), chain.count < 64 {
      chain.append(current)
      guard let next = superclass(of: current.id, meta: meta), next.id > 0 else {
        break
      }
      current = next
    }
    var rows = Array(chain.reversed())
    for sub in subclasses(of: start.id) where !rows.contains(where: { $0.id == sub.id }) {
      rows.append(sub)
    }
    return rows
  }

  // The category the runtime lists for the class; nil when the list has no such ID.
  func category(ofClassID id: Int64) -> String? {
    loadClasses().first { $0.row.id == id }?.category
  }

  // Class-list rows only. Protocols stay unless the selected class changed under us.
  func applyHierarchyList(_ rows: [BrowserClass], selecting id: Int64?) {
    let previous = selectedClassID
    classRows = rows
    if let id, let row = rows.first(where: { $0.id == id }) {
      selectedClassID = id
      selectedClassName = row.name
    }
    guard selectedClassID != previous else {
      return
    }
    protocols = loadProtocols()
    if let current = selectedProtocol, !protocols.contains(current) {
      selectedProtocol = nil
    }
    selectors = loadSelectors()
    if let current = selectedSelector, !selectors.contains(current) {
      selectedSelector = nil
    }
    (source, sourceIsPlaceholder) = loadSource()
  }

  // SPEC §3.9 System Browser: the selected ID stays while the list has it. An ID that left the
  // list gives way to the class now bound to the name its row showed, when the list has that one,
  // else to the first row.
  private func reselectClass() {
    guard let current = selectedClassID else {
      return
    }
    if let row = classRows.first(where: { $0.id == current }) {
      selectedClassName = row.name
      return
    }
    let rebound = selectedClassName.map(Self.classID(named:)) ?? 0
    let row = classRows.first { $0.id == rebound } ?? classRows.first
    selectedClassID = row?.id
    selectedClassName = row?.name
  }

  private struct ListedClass {
    var row: BrowserClass
    var category: String
  }

  private func loadClasses() -> [ListedClass] {
    let count = ao_browser_class_count()
    if count <= 0 {
      return []
    }
    var rows: [ListedClass] = []
    rows.reserveCapacity(Int(count))
    for index in 0..<count {
      guard let row = copyClass(at: index) else {
        return []
      }
      rows.append(row)
    }
    return rows
  }

  private func copyClass(at index: Int32) -> ListedClass? {
    var capacity = 128
    while capacity <= 1_048_576 {
      var classID: Int64 = 0
      var name = [CChar](repeating: 0, count: capacity)
      var category = [CChar](repeating: 0, count: capacity)
      let rc = withUnsafeMutablePointer(to: &classID) { idPointer -> Int32 in
        name.withUnsafeMutableBufferPointer { namePointer -> Int32 in
          category.withUnsafeMutableBufferPointer { categoryPointer -> Int32 in
            guard let nameBase = namePointer.baseAddress,
                  let categoryBase = categoryPointer.baseAddress else {
              return Int32(AO_ERR)
            }
            return ao_browser_class_at(
              index, idPointer, nameBase, Int32(capacity), categoryBase, Int32(capacity))
          }
        }
      }
      if rc == Int32(AO_OK) {
        return ListedClass(
          row: BrowserClass(id: classID, name: decode(name)), category: decode(category))
      }
      if rc != Int32(AO_ERR_RANGE) {
        return nil
      }
      capacity *= 2
    }
    return nil
  }

  // Selector of the listed method whose source is `text`. The source table keeps the accepted
  // text as is, so this finds the method just accepted without parsing its pattern. A NOSOURCE
  // placeholder is a lone comment and never equals an accepted method.
  func selector(withSource text: String) -> String? {
    guard let classID = selectedClassID else {
      return nil
    }
    let meta = metaFlag
    return selectors.first { selector in
      let source = copyText { buffer, length in
        ao_browser_source(classID, meta, selector, buffer, length)
      }
      return source == text
    }
  }

  // The runtime lists only non-empty protocols. The new-method protocol is always offered,
  // so a class without methods on this side can still take its first one.
  private func loadProtocols() -> [String] {
    guard let classID = selectedClassID else {
      return []
    }
    let meta = metaFlag
    var names = loadList(
      count: { ao_browser_protocol_count(classID, meta) },
      at: { index, buffer, length in
        ao_browser_protocol_at(classID, meta, index, buffer, length)
      }
    )
    if !names.contains(Self.newMethodProtocol) {
      names.append(Self.newMethodProtocol)
    }
    return names
  }

  private func loadSelectors() -> [String] {
    guard let classID = selectedClassID, let selectedProtocol else {
      return []
    }
    let meta = metaFlag
    return loadList(
      count: { ao_browser_selector_count(classID, meta, selectedProtocol) },
      at: { index, buffer, length in
        ao_browser_selector_at(classID, meta, selectedProtocol, index, buffer, length)
      }
    )
  }

  // `placeholder` is true when the selected method answered AO_ERR_NOSOURCE.
  private func loadSource() -> (text: String, placeholder: Bool) {
    guard let classID = selectedClassID else {
      return ("", false)
    }
    let meta = metaFlag
    if let selectedSelector {
      let copied = copyReportingNoSource { buffer, length in
        ao_browser_source(classID, meta, selectedSelector, buffer, length)
      }
      return (copied?.text ?? "", copied?.noSource ?? false)
    }
    // A protocol with no selector is a new method; no protocol is the class definition.
    if selectedProtocol != nil {
      return ("", false)
    }
    let definition = copyText { buffer, length in
      ao_browser_class_definition(classID, buffer, length)
    }
    return (definition ?? "", false)
  }

  private var metaFlag: Int32 {
    selectedMeta ? 1 : 0
  }

  // SPEC §3.10: the superclass's name and ID (0 when it is nil or not listed).
  private func superclass(of classID: Int64, meta: Bool) -> BrowserClass? {
    let flag: Int32 = meta ? 1 : 0
    var superID: Int64 = 0
    let name = withUnsafeMutablePointer(to: &superID) { idPointer in
      copyText { buffer, length in
        ao_browser_superclass(classID, flag, idPointer, buffer, length)
      }
    }
    return name.map { BrowserClass(id: superID, name: $0) }
  }

  private func subclasses(of classID: Int64) -> [BrowserClass] {
    let total = ao_browser_subclass_count(classID)
    if total <= 0 {
      return []
    }
    var rows: [BrowserClass] = []
    rows.reserveCapacity(Int(total))
    for index in 0..<total {
      var subID: Int64 = 0
      let name = withUnsafeMutablePointer(to: &subID) { idPointer in
        copyText { buffer, length in
          ao_browser_subclass_at(classID, index, idPointer, buffer, length)
        }
      }
      guard let name else {
        return []
      }
      rows.append(BrowserClass(id: subID, name: name))
    }
    return rows
  }

  private func loadList(
    count: () -> Int32,
    at: (Int32, UnsafeMutablePointer<CChar>, Int32) -> Int32
  ) -> [String] {
    let total = count()
    if total <= 0 {
      return []
    }
    var values: [String] = []
    values.reserveCapacity(Int(total))
    for index in 0..<total {
      guard let text = copyText({ buffer, length in at(index, buffer, length) }) else {
        return []
      }
      values.append(text)
    }
    return values
  }

  private func copyText(_ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32) -> String? {
    copyReportingNoSource(read)?.text
  }

  private func copyReportingNoSource(
    _ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32
  ) -> (text: String, noSource: Bool)? {
    var capacity = 256
    while capacity <= 1_048_576 {
      var buffer = [CChar](repeating: 0, count: capacity)
      let rc = buffer.withUnsafeMutableBufferPointer { pointer -> Int32 in
        guard let base = pointer.baseAddress else {
          return Int32(AO_ERR)
        }
        return read(base, Int32(capacity))
      }
      if rc == Int32(AO_OK) {
        return (decode(buffer), false)
      }
      // SPEC §3.10: a method without source answers its placeholder with AO_ERR_NOSOURCE, also
      // when cut, so a full buffer asks for a larger one.
      if rc == Int32(AO_ERR_NOSOURCE) {
        let text = decode(buffer)
        if text.utf8.count < capacity - 1 {
          return (text, true)
        }
      } else if rc != Int32(AO_ERR_RANGE) {
        return nil
      }
      capacity *= 2
    }
    return nil
  }

  private func decode(_ buffer: [CChar]) -> String {
    buffer.withUnsafeBufferPointer { pointer in
      guard let base = pointer.baseAddress else {
        return ""
      }
      return String(cString: base)
    }
  }

  private func uniqueCategories(_ rows: [ListedClass]) -> [String] {
    var seen: Set<String> = []
    var values: [String] = []
    for row in rows {
      if seen.insert(row.category).inserted {
        values.append(row.category)
      }
    }
    return values
  }
}
```

- [ ] **Step 5: `BrowserWindow.swift` を ID にする**

プロパティ:

```swift
  private var categoryName = "Kernel"
  // SPEC §3.10 クラス ID: the selected row's class; 0 is none.
  private var selectedClassID: Int64 = 0
```

```swift
  private var showingHierarchy = false
  private var hierarchy: [BrowserClass] = []
```

```swift
  var canRemoveClass: Bool {
    selectedClassID != 0
  }
```

`tableViewSelectionDidChange` のクラス一覧の枝:

```swift
    } else if table === classTable {
      let picked = value(at: row, in: model.classRows)
      changeSelection {
        if let picked {
          self.selectedClassID = picked.id
        }
        self.protocolName = nil
        self.selectorName = nil
      }
```

`showDefinedClass`:

```swift
  // The accepted definition's class (the one its name binds now), in the category the runtime
  // lists for it. The rows the pane came from may be another class, or a category the class has
  // just left.
  private func showDefinedClass(from source: String) {
    if let name = BrowserWindow.definedClassName(in: source) {
      let id = BrowserModel.classID(named: name)
      if id != 0, let category = model.category(ofClassID: id) {
        categoryName = category
        selectedClassID = id
      }
    }
    publish()
  }
```

削除:

```swift
  // SPEC §3.9 削除: Smalltalk → Remove Class…, for the selected class.
  func removeClass() {
    let row = model.classRows.firstIndex { $0.id == selectedClassID } ?? -1
    removeClass(atRow: row, fromMainMenu: true)
  }
```

```swift
  private func removeMethod(atRow row: Int, fromMainMenu: Bool) {
    guard let selector = value(at: row, in: model.selectors) else {
      return
    }
    let classID = selectedClassID
    let className = model.selectedClass ?? ""
    let classSide = meta
    let message = "Remove \(className)\(classSide ? " class" : "")>>\(selector)?"
    removeAfterConfirming(
      message: message,
      select: selector == selectorName ? nil : { self.selectorName = selector },
      fromMainMenu: fromMainMenu
    ) {
      self.performRemoveMethod(selector, ofClassID: classID, meta: classSide)
    }
  }
```

```swift
  private func removeClass(atRow row: Int, fromMainMenu: Bool) {
    guard let picked = value(at: row, in: model.classRows) else {
      return
    }
    removeAfterConfirming(
      message: "Remove class \(picked.name)?",
      select: picked.id == selectedClassID ? nil : {
        self.selectedClassID = picked.id
        self.protocolName = nil
        self.selectorName = nil
      },
      fromMainMenu: fromMainMenu
    ) {
      self.performRemoveClass(picked.id)
    }
  }
```

```swift
  private func performRemoveMethod(_ selector: String, ofClassID classID: Int64, meta classSide: Bool) {
    var err = AoSpan()
    let metaFlag: Int32 = classSide ? 1 : 0
    let status = selector.withCString { sel in
      withUnsafeMutablePointer(to: &err) { errPtr in
        ao_remove_method(classID, metaFlag, sel, errPtr)
      }
    }
    guard status == Int32(AO_OK) else {
      errorField.stringValue = spanMessage(err)
      return
    }
    errorField.stringValue = ""
    selectorName = nil
    publish()
  }

  // SPEC §3.9 削除: success deselects the class and keeps the category while it is still listed;
  // a category that went with its last class gives way to the first one. The hierarchy list
  // drops the class too.
  private func performRemoveClass(_ classID: Int64) {
    var err = AoSpan()
    let status = withUnsafeMutablePointer(to: &err) { errPtr in
      ao_remove_class(classID, errPtr)
    }
    guard status == Int32(AO_OK) else {
      errorField.stringValue = spanMessage(err)
      return
    }
    errorField.stringValue = ""
    selectedClassID = 0
    protocolName = nil
    selectorName = nil
    hierarchy.removeAll { $0.id == classID }
    publish()
    if !model.categories.contains(categoryName), let first = model.categories.first {
      categoryName = first
      publish()
    }
  }
```

階層とロード:

```swift
  private func toggleHierarchy() {
    if showingHierarchy {
      showingHierarchy = false
      hierarchy = []
      publish()
      return
    }
    hierarchy = model.selectedClassRow.map { model.hierarchy(of: $0, meta: meta) } ?? []
    showingHierarchy = true
    model.applyHierarchyList(hierarchy, selecting: selectedClassID == 0 ? nil : selectedClassID)
    reloadLists()
  }

  // SPEC §3.9 System Browser: the load started new IDs; publish lets the model find the selected
  // class again by its name.
  func noteImageLoaded() {
    showingHierarchy = false
    hierarchy = []
    publish()
  }
```

`showInitialSelection` の `selectedClass = "Object"` を置き換える:

```swift
    selectedClassID = BrowserModel.classID(named: "Object")
```

`publish`:

```swift
  private func publish() {
    let keepClass: Int64? = selectedClassID == 0 ? nil : selectedClassID
    model.select(
      category: categoryName,
      classID: keepClass,
      meta: meta,
      protocol: protocolName,
      selector: selectorName
    )
    if showingHierarchy {
      model.applyHierarchyList(hierarchy, selecting: keepClass)
    }
    selectedClassID = model.selectedClassID ?? 0
    protocolName = model.selectedProtocol
    selectorName = model.selectedSelector
    reloadLists()
  }
```

`showSelection` のクラスの行:

```swift
    select(categoryName, in: categoryTable, values: model.categories)
    if let row = model.classRows.firstIndex(where: { $0.id == selectedClassID }) {
      classTable.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    } else {
      classTable.deselectAll(nil)
    }
```

`value(at:in:)` を総称にする:

```swift
  private func value<Element>(at row: Int, in values: [Element]) -> Element? {
    guard row >= 0, row < values.count else {
      return nil
    }
    return values[row]
  }
```

`submit` のメソッドの枝:

```swift
    } else {
      let metaFlag: Int32 = meta ? 1 : 0
      let classID = selectedClassID
      status = source.withCString { src in
        withUnsafeMutablePointer(to: &err) { errPtr in
          ao_accept_method_id(classID, metaFlag, src, errPtr)
        }
      }
    }
```

`grep -n "selectedClass\b\|hierarchyNames\|className:" app/Ao/BrowserWindow.swift` が何も出さないことを確かめる（`model.selectedClass` の読み取りは除く）。

- [ ] **Step 6: 通ることを確かめる**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" -Xlinker -lc++`
Expected: PASS（全部。`testShowHierarchyIncludesIntegerThenReturnsToClassList`、`testRemoveClassAfterConfirmUpdatesLists` の階層の段、`ToolWindowTests` の Browser を含む）。

- [ ] **Step 7: Serena の overview を取り、コミットする**

`get_symbols_overview app/Ao/BrowserModel.swift`（`BrowserClass`、`reselectClass`、`hierarchy`、`superclass`、`subclasses`）。

```bash
git add app/Ao/BrowserModel.swift app/Ao/BrowserWindow.swift app/AoTests/AcceptTests.swift app/AoTests/BrowserModelTests.swift
git commit -m "Key the Browser's rows and selection by class ID

SPEC §3.9 System Browser, §3.10 クラス ID: the model lists BrowserClass rows (ID and
name), keeps the selection by ID and, when the ID leaves the list (a shape change, an
image load), selects the class now bound to the row's name. Reads, Accept, removal and
the hierarchy all go through the ID, so two rows named Foo act on their own classes.

Graphify: path BrowserWindow BrowserModel
Serena: find_referencing_symbols BrowserModel.select, applyHierarchyList; replace_symbol_body BrowserWindow.publish, performRemoveClass, submit

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

### Task 6: 受け入れ、記録、グラフ

**Files:**
- Modify: `SPEC.md` §6「Browser の削除」、`CHANGELOG.md`（`[Unreleased]`）、`docs/phases/P12.md`、`docs/README.md`
- Modify: `graphify-out/`（`/graphify . --update`）

- [ ] **Step 1: 全部を回す**

```sh
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build --output-on-failure -R gcstress
./build/ao --test image/tests
swift test --package-path app -Xlinker -force_load -Xlinker "$PWD/build/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$PWD/build/compiler/libao_compiler.a" -Xlinker -lc++
ctest --test-dir build -R KernelBench --output-on-failure
```

Expected: すべて PASS。KernelBench の数字を `docs/bench.md` の最後の表と比べる（キャッシュを外れた探索が `pairArray` の検査を 1 回足すだけなので、比は変わらないはず。悪化していたら原因を探すまで次へ進まない）。`gcstress` は ID の表の枠が GC で動くことも確かめる。

- [ ] **Step 2: 手動の受け入れ（Ao.app）**

`./scripts/package-app.sh` で `build/Ao.app` を作って開き、SPEC §6「Browser の削除」の新しい 3 項目を試す: `Foo`（カテゴリ Old）を定義 → Workspace で `Smalltalk at: #Alias put: Foo` → Browser で `Foo` を Remove Class… → `Foo`（カテゴリ New）を定義 → Old の `Foo` の行がそのメソッドを表示し、Remove Class… がエラー欄に `class removal refused: Foo is not bound to this class` を出し、Remove Method… と Accept が `Alias new …` に効く。Save Image → Open Image で同じ名前のクラスが選ばれ、Remove Class… が効く。Workspace で `Foo instVarAt: 2 put: (Array new: 0). Foo new bar` が `doesNotUnderstand: #bar` になり落ちない。

- [ ] **Step 3: SPEC §6 のチェックリスト**

`SPEC.md` の「### Browser の削除」の未チェックの 4 項目（別名の筋書き、イメージの開き直し、壊れたメソッド辞書、「この小節がすべて `[x]`…」）を `- [x]` にする（最後の項目は Step 4〜5 のあとで）。

- [ ] **Step 4: CHANGELOG**

`CHANGELOG.md` の `### Runtime` の P12 の `ao_remove_method` の項目のあとに足す:

```markdown
- C ABI: the Browser names a class by a session class ID instead of its name. `ao_browser_class_id` answers the ID of the class bound to a name; `ao_browser_class_at` answers each row's ID; the reads, `ao_accept_method_id` (new), `ao_remove_method` and `ao_remove_class` take an ID. The class list has one row per class. An old class that only an alias keeps keeps its own row, and its removal is refused (`is not bound to this class`) instead of unbinding the new class of the same name. An unknown or stale ID is refused (`unknown class id`); IDs do not survive a boot or an image load.
- A method dictionary slot rewritten with `instVarAt:put:` no longer crashes a send: the class has no methods there, so the send goes to the superclass or `doesNotUnderstand:`.
```

`### Ao.app` の Remove の項目のあとに足す:

```markdown
- The Browser keeps its rows and selection by class ID, so two classes with the same name are browsed, edited and removed apart. After Open Image it selects the class of the same name again.
```

- [ ] **Step 5: docs**

`docs/phases/P12.md` の「## 設計判断」の最後の項目（「Browser はクラスを自分自身の名前で一覧に出す。…どう扱うかは未決としてユーザーに残す。」）を置き換える:

```markdown
- Browser はクラスを自分自身の名前で一覧に出し、ABI はクラスをセッションのクラス ID で指す（追補、2026-09-27）。`Foo` を消して別名 `Alias` が旧クラスを持ち、新しい `Foo` を定義すると、一覧に `Foo` が 2 行並び、それぞれの行の読み出し、Accept、Remove Method… はその行のクラスに効く。旧クラスの行の Remove Class… は `class removal refused: Foo is not bound to this class` で拒む（外すのはクラス自身の名前の束縛だけで、それが別のクラスを指しているため）。
```

末尾に足す:

```markdown
## 追補: クラス ID と壊れたメソッド辞書（2026-09-27）

PR #17 のレビューで残った 2 点を片付けた。計画書は [`superpowers/plans/2026-09-27-p12-class-identity.md`](../superpowers/plans/2026-09-27-p12-class-identity.md)、理由は設計書の「追補: クラス ID（2026-09-27）」。

- クラス ID: セッションの表（ID → クラス。GC が更新するルートで、イメージの保存と生存の判定はたどらない）。一覧は 1 クラス 1 行、同じ名前は ID の順。未知の ID は `unknown class id`。ロードのあと Browser は名前で選び直す。
- 壊れたメソッド辞書: `MethodDictionary::pairArray` で形を確かめ、形が合わなければメソッドが無いものとして扱う（送信は上位クラスか `doesNotUnderstand:`、Accept は `install failed`）。

TDD: `browser_abi_test`、`remove_abi_test`、`accept_abi_test`、`session_abi_test`、`method_dictionary_test` の追補の各テスト（SPEC §4.1）と、`AcceptTests.testAliasedOldClassRowActsOnItsOwnClass`、`AcceptTests.testImageLoadReselectsTheClassByName`、`BrowserModelTests.testClassRowsCarryOneIdPerClass`（SPEC §4.3）。
```

`docs/README.md` の計画書の括弧の `P12: [2026-09-26-p12-browser-remove.md](superpowers/plans/2026-09-26-p12-browser-remove.md)` のあとに `、P12 追補: [2026-09-27-p12-class-identity.md](superpowers/plans/2026-09-27-p12-class-identity.md)` を足す。

- [ ] **Step 6: Graphify を更新する**

`/graphify . --update`。`GRAPH_REPORT.md` に `ao_accept_method_id`、`assignClassIds`、`pairArray` が現れ、`findClass` / `removeClassNamed` が消えていることを確かめる。Serena の clangd の子プロセスを止めて作り直させる（C++ の宣言が変わったため）。

- [ ] **Step 7: Commit**

```bash
git add SPEC.md CHANGELOG.md docs/phases/P12.md docs/README.md graphify-out
git commit -m "Close the class identity follow-up and rebuild the knowledge graph

SPEC §6 Browser の削除: the alias, image reload and malformed dictionary items pass.

Graphify: --update
Serena: none (docs only)

Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"
```

---

## Self-Review

- **Spec coverage:** §3.3 メソッド辞書の枠（Task 1）、§3.9 System Browser の ID と選び直し（Task 5）、§3.9 クラス定義の再 Accept 冒頭のクラス ID の表の除外（Task 2 `ClassIdTableDoesNotKeepSubclassesAlive`）、§3.9「削除」の ID、`unknown class id`、名前スロットのメッセージ、`is not bound to this class`、別名の 2 行（Task 4、5）、§3.10 C ABI の署名と説明（Task 2〜4）、busy の一覧（Task 4）、件数の -1（Task 3）、クラス ID の節（発行、同じクラスは同じ ID、使い回さない、刈り込み、新しいセッション、ルートとたどらない枠、全部か無しか: Task 2〜3）、ソースはイメージに書かないの最後の項目（Task 2 `ImageSaveDoesNotTraceClassIdSlots`）、§4.1 と §4.3 のテスト名（Task 1〜5）、§6 の 3 項目（Task 6）。
- **Placeholder scan:** 各コードステップに完全なコードがある。perl の置換は Step 2 の grep で確かめる。
- **Type consistency:** `ClassRow::id`（`std::int64_t`）、`Session::ClassId`、`classIds` は Task 2 で定義し Task 3〜4 と `accept_abi_test` / `session_abi_test` / `browser_abi_test` が使う。`findClassId(const std::vector<ClassRow>&, std::int64_t)` は Task 3 で定義し Task 4 の `sessionClassForId` が使う。`browserClassAt(int, std::int64_t*, …)` は Session.hpp と abi.cpp で同じ。`acceptMethodInto` / `removeMethodOf` / `removeClassOf` は Compile.hpp、Compile.cpp、abi.cpp で同じ。Swift の `BrowserClass`、`classRows`、`selectedClassID`、`select(category:classID:meta:protocol:selector:)`、`hierarchy(of:meta:)`、`applyHierarchyList(_:selecting:)`、`category(ofClassID:)`、`classID(named:)` は Task 5 のモデルとウィンドウとテストで同じ（`classID(named:)` は Task 3 で先に入る）。`tryRemoveMethodId` / `tryRemoveClassId` / `idOf` は Task 4 の Step 2 で定義し、同じタスクのテストが使う。
- **Review Focus:** 5 項目それぞれにテストがある（Task 5 の形の変更、Task 4 の名前の無いクラスと busy、Task 3 の同じ名前の 2 行とロード、Task 5 のロード）。
- **既知の危うさ:** `LongNameCutsTheMessageAt255Bytes` は 300 バイトのクラス名を定義する。コンパイラかシンボルに長さの上限があって定義できなければ、名前を 260 バイトにする（メッセージが 255 を超えれば目的は同じ）。`RemoveFixedGlobalIsRefused` の `Bag` は vendor のスタブが一覧に出る（クラスの枠を全部持つ）ことを前提にする。出なければ `unknown class id` になるので、そのときは `Bag` の 2 行を `Transcript` などの別の固定のグローバルに替える。

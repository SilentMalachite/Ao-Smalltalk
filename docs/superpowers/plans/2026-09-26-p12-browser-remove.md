# P12 Browser の削除 — 実装計画

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** System Browser から CompiledMethod と Kernel 以外のクラスを消せるようにする。C ABI に `ao_remove_method` と `ao_remove_class` を足し、Browser の右クリックメニューと Smalltalk メニューから確認のシートを経て呼ぶ。

**Architecture:** 辞書の対を nil にして `tally` を減らす削除プリミティブ（`MethodDictionary::removeKey`、`Globals::unbind`、`WellKnown::undefine`）の上に、検査をすべて先に済ませてから書き換える `removeMethodNamed` / `removeClassNamed`（`Compile.cpp`）を置き、ABI は Accept と同じ busy 判定と `AoSpan.message` で理由を返す。Browser は既存の `confirmDiscard` と同じ差し替え可能な確認 `confirmRemove` を持ち、メニューは `clickedRow` を対象にする。

**Tech Stack:** C++20（runtime）、GoogleTest、Swift 6 AppKit（app）、XCTest。

**Spec:** `SPEC.md` §3.6「グローバル辞書」、§3.9「削除」、§3.10「ワークスペース変数」「再入と例外」、§4.1 `remove_abi_test`、§4.3「Browser の削除」、§6「Browser の削除」。設計の理由: `docs/superpowers/specs/2026-09-26-browser-remove-design.md`。

## Global Constraints

- Kernel メソッドは C++ の NativeMethod のまま。Smalltalk 側に削除セレクタ（`removeSelector:`、`removeFromSystem`）を足さない（SPEC §3.9「削除」）。
- 削除はヒープに割り当てない（GC を起こさない）。クラス名とセレクタは `findSymbol` で引き、`intern` しない。
- 拒んだとき、メソッド辞書、グローバル辞書、キャッシュ、ソース表はどれも変わらない。検査をすべて済ませてから書き換える。
- 成功は `AO_OK` で `AoSpan.message` を空にする。拒否は `AO_ERR` で `start`/`end` は 0、メッセージは空にしない。`AO_ERR_COMPILE` は使わない。
- メッセージの `<name>`/`<Class>` は渡した `class_name` そのまま。`meta` が 1 なら ` class` を付ける（`Foo class>>bar`）。
- busy のとき（`AbiEntry` が取れないとき）は `runtime is busy` で拒む。`ao_abi.h` の busy 一覧と数（eighteen → twenty）を直す。
- `ao_remove_class` はグローバル名の版（`WellKnown::globalsVersion()`）を進める。同名のワークスペース変数には触れない。
- 右クリックのメニューは右クリックした行を対象にし、項目を選んだ時点で「破棄の確認 → 行を選ぶ → 削除の確認 → ABI」を順に進める。
- 確認のシート: メッセージ `Remove Foo>>bar?` / `Remove Foo class>>bar?` / `Remove class Foo?`、補足 `This cannot be undone.`、ボタン Remove（destructive）と Cancel。
- CLAUDE.md の手順: 各タスクの前に Serena `find_symbol` / `find_referencing_symbols` で定義と参照を取り、本体の書き換えは `replace_symbol_body` / `insert_after_symbol` / `insert_before_symbol` で行う。新規ファイルは作成後に `get_symbols_overview` を取る。コミット本文に `Graphify:` と `Serena:` のトレーラを 1 行ずつ残す。最後に `/graphify . --update`。
- 頼まれていないファイルを増やさない。隣のリファクタをしない（`liveClasses` を書き換えない。新しい判定は別関数にする）。

## Review Focus

1. 止まったプロセスのフレームが走らせているメソッドを消す → そのフレームは消す前のメソッドのまま最後まで走り、Proceed が値を返す（Task 4 `RemovedMethodKeepsRunningInHaltedProcess`）。
2. 階層表示中にクラスを消す → 一覧（`hierarchyNames`）からその名前が消え、選択が外れる（Task 5 `testRemoveClassAfterConfirmUpdatesLists` の階層表示の段）。
3. 空文字のクラス名やセレクタ → `not a class: ` / `selector not found: Foo>>` で拒む。クラッシュしない（Task 3 `RemoveFixedGlobalIsRefused`、Task 2 `RemoveMissingOrInheritedSelectorIsRefused`）。
4. 255 バイトを超えるメッセージ → `AoSpan.message` の規則どおり 255 バイトで切り、空にならない（Task 4 `LongNameCutsTheMessageAt255Bytes`）。
5. 破棄の確認が出ている間にもう一度右クリックの項目を選ぶ → 2 つ目の質問を出さない（`showHierarchy` と同じく `confirming` で無視する。Task 6 `testContextMenusOfferRemove`）。

---

## File Structure

| ファイル | 責務 |
|---|---|
| `runtime/include/ao/MethodDictionary.hpp`, `runtime/src/MethodDictionary.cpp` | `removeKey`: 対を nil にして `tally` を減らす |
| `runtime/include/ao/Globals.hpp`, `runtime/src/Globals.cpp` | `unbind`: `Smalltalk` の対を空きにする |
| `runtime/include/ao/WellKnown.hpp`, `runtime/src/WellKnown.cpp` | `undefine`: 固定のグローバルを拒み、`unbind` して版を進める |
| `runtime/src/Session.hpp`, `runtime/src/Session.cpp` | `forgetMethodSource`: ソース表の項目をブロックごと外す |
| `runtime/include/ao/Compile.hpp`, `runtime/src/Compile.cpp` | `removeMethodNamed` / `removeClassNamed`: 検査と書き換え |
| `bridge/ao_abi.h`, `runtime/src/abi.cpp` | `ao_remove_method` / `ao_remove_class`、busy 一覧 |
| `runtime/tests/method_dictionary_test.cpp` | `removeKey` の単体テスト |
| `runtime/tests/remove_abi_test.cpp`（新規）, `runtime/CMakeLists.txt` | SPEC §4.1 の `remove_abi_test` |
| `app/Ao/BrowserModel.swift` | クラス未選択（nil）を保てるようにする |
| `app/Ao/BrowserWindow.swift` | `confirmRemove`、`removeMethod()`/`removeClass()`、`removeMethod(atRow:)`/`removeClass(atRow:)`、右クリックメニュー、`keyBrowserAllows` |
| `app/Ao/MainMenu.swift` | Smalltalk メニューの 2 項目と `NSMenuItemValidation` |
| `app/Ao/AoApp.swift` | メニューの配線 |
| `app/AoTests/AcceptTests.swift` | SPEC §4.3 の 7 テスト（既存のヘルパを使う） |
| `CHANGELOG.md`, `PHASE`, `SPEC.md` §6, `docs/README.md`, `docs/phases/P12.md` | 記録 |

## ビルドとテストのコマンド

```sh
cmake --build build
ctest --test-dir build --output-on-failure -R 'RemoveAbi|RemoveUnits|MethodDictionary'
ctest --test-dir build --output-on-failure            # 全部
ctest --test-dir build --output-on-failure -R gcstress
./build/ao --test image/tests
swift test --package-path app \
  -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a \
  -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a \
  -Xlinker -lc++ --filter 'AcceptTests/testRemove'
```

`build/` が無ければ `cmake -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`。

---

### Task 1: 辞書の削除プリミティブ

**Files:**
- Modify: `runtime/include/ao/MethodDictionary.hpp`, `runtime/src/MethodDictionary.cpp`
- Modify: `runtime/include/ao/Globals.hpp`, `runtime/src/Globals.cpp`
- Modify: `runtime/include/ao/WellKnown.hpp`（`define` の宣言のあと）, `runtime/src/WellKnown.cpp`（`WellKnown::define` のあと）
- Test: `runtime/tests/method_dictionary_test.cpp`、`runtime/tests/remove_abi_test.cpp`（新規）、`runtime/CMakeLists.txt`

**Interfaces:**
- Produces: `bool ao::MethodDictionary::removeKey(Heap&, Oop dict, Oop key)`、`bool ao::Globals::unbind(WellKnown&, Oop key)`、`bool ao::WellKnown::undefine(std::string_view name)`

- [ ] **Step 1: Serena で定義と参照を取る**

`find_symbol` で `ao::MethodDictionary::atPut`、`ao::Globals::bindIn`、`ao::WellKnown::define`、`find_referencing_symbols` で `kDictSlotTally`、`kSmalltalkSlotTally`（`tally` を読む側が線形走査だけであることを確認する）。

- [ ] **Step 2: `removeKey` の失敗するテストを書く**

`runtime/tests/method_dictionary_test.cpp` の末尾に足す:

```cpp
// SPEC §3.9 削除: the removed pair goes nil/nil and the tally drops by one; the other pairs stay
// where they are, and the next atPut takes the emptied pair.
TEST(MethodDictionary, RemoveKeyLeavesTheOthersFindableAndReusesThePair) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  auto dict = ao::MethodDictionary::create(heap, wk, 4);
  const ao::Oop a = ao::Symbol::intern(wk, "rkA");
  const ao::Oop b = ao::Symbol::intern(wk, "rkB");
  const ao::Oop c = ao::Symbol::intern(wk, "rkC");
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, a, ao::Oop::fromSmallInteger(1)));
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, b, ao::Oop::fromSmallInteger(2)));
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, c, ao::Oop::fromSmallInteger(3)));
  EXPECT_TRUE(ao::MethodDictionary::removeKey(heap, dict, b));
  EXPECT_TRUE(ao::MethodDictionary::at(heap, dict, b).isNil());
  EXPECT_EQ(1, ao::MethodDictionary::at(heap, dict, a).smallIntegerValue());
  EXPECT_EQ(3, ao::MethodDictionary::at(heap, dict, c).smallIntegerValue());
  EXPECT_EQ(2, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop inner = heap.slotAt(dict, ao::kDictSlotArray);
  EXPECT_TRUE(heap.slotAt(inner, 2).isNil());
  EXPECT_TRUE(heap.slotAt(inner, 3).isNil());
  // Not bound any more: false, and nothing changes.
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, dict, b));
  EXPECT_EQ(2, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
  const ao::Oop d = ao::Symbol::intern(wk, "rkD");
  ASSERT_TRUE(ao::MethodDictionary::atPut(heap, dict, d, ao::Oop::fromSmallInteger(4)));
  EXPECT_EQ(d, heap.slotAt(inner, 2));
  EXPECT_EQ(3, heap.slotAt(dict, ao::kDictSlotTally).smallIntegerValue());
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, ao::Oop::nil(), a));
  EXPECT_FALSE(ao::MethodDictionary::removeKey(heap, dict, ao::Oop::fromSmallInteger(1)));
}
```

- [ ] **Step 3: `undefine` の失敗するテストを新規ファイルに書き、CMake に登録する**

`runtime/tests/remove_abi_test.cpp` を作る:

```cpp
#include "ao_abi.h"

#include "../src/Session.hpp"
#include "ao/Bootstrap.hpp"
#include "ao/Globals.hpp"
#include "ao/Heap.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Roots.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

// SPEC §3.6, §3.9 削除: undefine takes the pair out (nil key and value, tally one less), moves the
// globals version, refuses a fixed global and a name Smalltalk does not bind, and interns nothing.
TEST(RemoveUnits, UndefineUnbindsAndMovesTheGlobalsVersion) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::Bootstrap::run(heap, roots, wk);
  ASSERT_TRUE(wk.define("RuGone", ao::Oop::fromSmallInteger(3)));
  const std::uint64_t version = wk.globalsVersion();
  const ao::Oop key = wk.findSymbol("RuGone");
  ASSERT_TRUE(key.isHeap());
  const ao::Oop pairs = heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotArray);
  std::uint32_t index = UINT32_MAX;
  for (std::uint32_t i = 0; i + 1 < heap.size(pairs); i += 2) {
    if (heap.slotAt(pairs, i) == key) index = i;
  }
  ASSERT_NE(UINT32_MAX, index);
  const std::int64_t tally =
      heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue();

  EXPECT_TRUE(wk.undefine("RuGone"));
  EXPECT_TRUE(ao::Globals::lookup(wk, key).isEmpty());
  EXPECT_TRUE(wk.named("RuGone").isNil());
  EXPECT_TRUE(heap.slotAt(pairs, index).isNil());
  EXPECT_TRUE(heap.slotAt(pairs, index + 1).isNil());
  EXPECT_EQ(tally - 1,
            heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue());
  EXPECT_EQ(version + 1, wk.globalsVersion());

  EXPECT_FALSE(wk.undefine("RuGone"));
  EXPECT_FALSE(wk.undefine("Object"));
  EXPECT_FALSE(wk.undefine("Smalltalk"));
  EXPECT_FALSE(wk.undefine("Processor"));
  EXPECT_FALSE(wk.undefine("RuNeverInterned"));
  EXPECT_TRUE(wk.findSymbol("RuNeverInterned").isEmpty());
  EXPECT_EQ(wk.objectClass, wk.named("Object"));
  EXPECT_EQ(version + 1, wk.globalsVersion());

  // The next define takes the emptied pair, in place.
  ASSERT_TRUE(wk.define("RuNext", ao::Oop::fromSmallInteger(4)));
  EXPECT_EQ(wk.findSymbol("RuNext"), heap.slotAt(pairs, index));
  EXPECT_EQ(tally, heap.slotAt(wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue());
}
```

`runtime/CMakeLists.txt` の `tests/debug_abi_test.cpp` の次の行に `tests/remove_abi_test.cpp` を足す。

- [ ] **Step 4: 失敗を確認する**

Run: `cmake --build build 2>&1 | tail -5`
Expected: `removeKey` / `undefine` が無いというコンパイルエラー。

- [ ] **Step 5: `MethodDictionary::removeKey` を実装する**

`runtime/include/ao/MethodDictionary.hpp` の `atPut` の宣言のあとに:

```cpp
// SPEC §3.9 削除: takes key's pair out: the pair goes nil/nil and the tally drops by one. The
// other pairs stay where they are, so at finds each of them still, and the next atPut takes the
// emptied pair. False when dict is not a dictionary, key is not a heap object, or dict does not
// hold key. Allocates nothing.
bool removeKey(Heap& heap, Oop dict, Oop key);
```

`runtime/src/MethodDictionary.cpp` の `atPut` のあとに（Serena `insert_after_symbol`）:

```cpp
bool removeKey(Heap& heap, Oop dict, Oop key) {
  if (!dict.isHeap() || !key.isHeap()) {
    return false;
  }
  const Oop inner = heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap()) {
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

- [ ] **Step 6: `Globals::unbind` を実装する**

`runtime/include/ao/Globals.hpp` の `bind` の宣言のあとに:

```cpp
// SPEC §3.6: empties key's pair (nil key and value) and takes one off the tally; the other pairs
// stay in place, and the next bind takes the emptied pair. Allocates nothing. False when
// Smalltalk is no dictionary, key is no heap object, or Smalltalk binds no such key.
bool unbind(WellKnown& wk, Oop key);
```

`runtime/src/Globals.cpp` の `bind` のあとに:

```cpp
bool unbind(WellKnown& wk, Oop key) {
  if (!key.isHeap() || !isDictionary(wk, wk.smalltalk)) {
    return false;
  }
  Heap& heap = wk.heap();
  const Oop pairs = heap.slotAt(wk.smalltalk, kSmalltalkSlotArray);
  const auto n = heap.size(pairs);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    if (heap.slotAt(pairs, i) != key) {
      continue;
    }
    heap.slotAtPut(pairs, i, Oop::nil());
    heap.slotAtPut(pairs, i + 1, Oop::nil());
    const Oop tally = heap.slotAt(wk.smalltalk, kSmalltalkSlotTally);
    const std::int64_t bound = tally.isSmallInteger() ? tally.smallIntegerValue() : 0;
    heap.slotAtPut(wk.smalltalk, kSmalltalkSlotTally,
                   Oop::fromSmallInteger(bound > 0 ? bound - 1 : 0));
    return true;
  }
  return false;
}
```

- [ ] **Step 7: `WellKnown::undefine` を実装する**

`runtime/include/ao/WellKnown.hpp` の `bool define(std::string_view name, Oop value);` の次に:

```cpp
  // SPEC §3.6, §3.9 削除: takes name's binding out of Smalltalk. A fixed global keeps its
  // binding: false. False also when Smalltalk binds no such name. Allocates nothing (the Symbol
  // is looked up, never interned) and moves globalsVersion, so knownGlobals is rebuilt (§3.10).
  bool undefine(std::string_view name);
```

`runtime/src/WellKnown.cpp` の `WellKnown::define` のあとに:

```cpp
bool WellKnown::undefine(std::string_view name) {
  if (isFixedGlobal(name)) {
    return false;
  }
  const Oop key = findSymbol(name);
  if (!key.isHeap() || !Globals::unbind(*this, key)) {
    return false;
  }
  ++globalsVersion_;
  return true;
}
```

- [ ] **Step 8: テストを通す**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'MethodDictionary|RemoveUnits'`
Expected: PASS（`RemoveKeyLeavesTheOthersFindableAndReusesThePair`、`UndefineUnbindsAndMovesTheGlobalsVersion` を含む）。

- [ ] **Step 9: Commit**

```bash
git add runtime/include/ao/MethodDictionary.hpp runtime/src/MethodDictionary.cpp \
  runtime/include/ao/Globals.hpp runtime/src/Globals.cpp \
  runtime/include/ao/WellKnown.hpp runtime/src/WellKnown.cpp \
  runtime/tests/method_dictionary_test.cpp runtime/tests/remove_abi_test.cpp runtime/CMakeLists.txt
git commit -m "Add removeKey, unbind and undefine to the dictionaries

SPEC §3.6, §3.9 削除: a removed pair goes nil/nil and the tally drops by one; the next
bind or atPut takes the emptied pair. undefine refuses a fixed global and moves the
globals version.

Graphify: path MethodDictionary Globals WellKnown
Serena: insert_after_symbol ao::MethodDictionary::atPut, ao::Globals::bind, ao::WellKnown::define

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

### Task 2: `ao_remove_method`

**Files:**
- Modify: `runtime/src/Session.hpp`（`rememberMethodSource` の宣言のあと）, `runtime/src/Session.cpp`（`rememberMethodSource` のあと）
- Modify: `runtime/include/ao/Compile.hpp`（`acceptClassSource` の宣言のあと）, `runtime/src/Compile.cpp`（無名名前空間の `hasSubclass` のあとにヘルパ、`acceptClassSource` のあとに公開関数）
- Modify: `bridge/ao_abi.h`（`ao_accept_class` の宣言のあと）, `runtime/src/abi.cpp`（`ao_accept_class` のあと）
- Test: `runtime/tests/remove_abi_test.cpp`

**Interfaces:**
- Consumes: `MethodDictionary::removeKey`（Task 1）、`invalidateMethodCache(ctx.cache, sel)`（`ao/NativeMethod.hpp`）、`namesBehavior`、`isClassShaped`、`MethodDictionary::at`。
- Produces: `void ao::forgetMethodSource(Oop method)`（Session.hpp）、`bool ao::removeMethodNamed(CallContext&, std::string_view className, bool meta, std::string_view selector, std::string* reason)`（Compile.hpp）、`int ao_remove_method(const char*, int, const char*, AoSpan*)`（ao_abi.h）、テストのヘルパ `RemoveAbi` fixture、`printIt`、`defineClass`、`tryRemoveMethod`、`browserLists`。

- [ ] **Step 1: Serena で定義と参照を取る**

`find_symbol`: `ao::rememberMethodSource`、`ao::acceptMethodSource`、`ao::namesBehavior`、`ao_accept_method`、`unrootEntry`。`find_referencing_symbols`: `invalidateMethodCache`（呼び出し元が `installMethod`、`putNative`、`rebindClassName`、`applyClassDef` 系だけであることを確認）。

- [ ] **Step 2: 失敗するテストを書く**

`runtime/tests/remove_abi_test.cpp` の `RemoveUnits` テストのあとに fixture とヘルパ、メソッド削除の 6 テストを足す:

```cpp
namespace {

class RemoveAbi : public ::testing::Test {
 protected:
  void SetUp() override { ASSERT_EQ(AO_OK, ao_runtime_boot()); }
  // SPEC §3.10: the hook and the debug mode outlive the session; a test's must not reach the next.
  void TearDown() override {
    ao_runtime_shutdown();
    ao_set_transcript_hook(nullptr, nullptr);
    ao_set_debug_mode(AO_DEBUG_POSTMORTEM);
  }
};

// Print it through the ABI: the printString, or "<rc: message>" when it fails.
std::string printIt(const char* source) {
  char out[512];
  AoSpan err{};
  const int rc = ao_eval(source, static_cast<int>(std::strlen(source)), AO_EVAL_PRINTIT, out,
                         static_cast<int>(sizeof(out)), &err);
  if (rc != AO_OK) {
    return "<" + std::to_string(rc) + ": " + err.message + ">";
  }
  return out;
}

// The Print it of `source` aborts with doesNotUnderstand: #selector.
void expectDnu(const char* source, const char* selector) {
  const std::string got = printIt(source);
  EXPECT_EQ(0u, got.find("<" + std::to_string(AO_ERR_EVAL) + ": ")) << got;
  EXPECT_NE(std::string::npos, got.find(std::string("doesNotUnderstand: #") + selector)) << got;
}

int defineClass(const char* name, const char* super, const char* category) {
  const std::string def = std::string(super) + " subclass: #" + name +
                          "\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
                          "  poolDictionaries: ''\n  category: '" + category + "'\n";
  AoSpan err{};
  return ao_accept_class(def.c_str(), &err);
}

// The call's message. err starts non-empty, so an AO_OK that leaves it alone shows.
std::string tryRemoveMethod(const char* cls, int meta, const char* sel, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_method(cls, meta, sel, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
}

bool browserLists(const char* className) {
  const int n = ao_browser_class_count();
  for (int i = 0; i < n; ++i) {
    char name[256];
    char category[256];
    if (ao_browser_class_at(i, name, sizeof(name), category, sizeof(category)) == AO_OK &&
        std::strcmp(name, className) == 0) {
      return true;
    }
  }
  return false;
}

// The method bound to sel on cls's own side (instance side), or nil.
ao::Oop methodOf(const char* cls, const char* sel) {
  ao::Session* s = ao::session();
  const ao::Oop c = s->wk.named(cls);
  const ao::Oop dict = s->heap.slotAt(c, ao::kClassSlotMethodDict);
  return ao::MethodDictionary::at(s->heap, dict, s->wk.findSymbol(sel));
}

}  // namespace

// SPEC §3.9 削除, §6: a method accepted on a Kernel class goes, and the send falls to
// doesNotUnderstand:. The message is empty on AO_OK.
TEST_F(RemoveAbi, RemoveAcceptedMethodOnKernelClass) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12foo\n  ^1\n", &err)) << err.message;
  EXPECT_EQ("1", printIt("Object new b12foo"));
  EXPECT_EQ("1", printIt("3 b12foo"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("Object", 0, "b12foo", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("Object new b12foo", "b12foo");
  expectDnu("3 b12foo", "b12foo");
  char shown[64];
  EXPECT_EQ(AO_ERR, ao_browser_source("Object", 0, "b12foo", shown, sizeof(shown)));
  EXPECT_EQ(0, ao_browser_selector_count("Object", 0, "user"));
}

// SPEC §3.3, §3.9 削除: a send cached before the removal finds the superclass's method next, and
// doesNotUnderstand: once that goes too.
TEST_F(RemoveAbi, RemovedMethodFallsBackEvenWhenCached) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Parent", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12Kid", "B12Parent", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Parent", 0, "who\n  ^'parent'\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B12Kid", 0, "who\n  ^'kid'\n", &err)) << err.message;
  EXPECT_EQ("'kid'", printIt("B12Kid new who"));
  EXPECT_EQ("'kid'", printIt("B12Kid new who"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Kid", 0, "who", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("'parent'", printIt("B12Kid new who"));
  EXPECT_EQ("'parent'", printIt("B12Parent new who"));
  EXPECT_EQ("", tryRemoveMethod("B12Parent", 0, "who", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Kid new who", "who");
  expectDnu("B12Parent new who", "who");
}

// SPEC §3.9 削除: meta 1 takes the metaclass's dictionary; the instance side is untouched.
TEST_F(RemoveAbi, RemoveClassSideMethod) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Cls", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Cls", 1, "make\n  ^42\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B12Cls", 0, "make\n  ^1\n", &err)) << err.message;
  EXPECT_EQ("42", printIt("B12Cls make"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Cls", 1, "make", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Cls make", "make");
  EXPECT_EQ("1", printIt("B12Cls new make"));
  EXPECT_EQ("selector not found: B12Cls class>>make", tryRemoveMethod("B12Cls", 1, "make", &rc));
  EXPECT_EQ(AO_ERR, rc);
}

// SPEC §3.9 削除: a NativeMethod is refused, on either side; the native keeps working.
TEST_F(RemoveAbi, RemoveNativeMethodIsRefused) {
  int rc = -9;
  EXPECT_EQ("native method removal refused: Object>>printString",
            tryRemoveMethod("Object", 0, "printString", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("native method removal refused: Behavior>>new", tryRemoveMethod("Behavior", 0, "new", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("3", printIt("3 printString"));
  // Object>>printString answers the class name; Print it shows the String quoted.
  EXPECT_EQ("'Object'", printIt("Object new printString"));
}

// SPEC §3.9 削除: only the side's own dictionary counts: an inherited selector, an unknown one, a
// name that is no class, an empty name or selector, and a bad meta are refused with their reasons.
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
  EXPECT_EQ("not a class: Processor", tryRemoveMethod("Processor", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: Smalltalk", tryRemoveMethod("Smalltalk", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: B12Nope", tryRemoveMethod("B12Nope", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: ", tryRemoveMethod("", 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 2, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod(nullptr, 0, "foo", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveMethod("B12Child", 0, nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  // err may be NULL.
  EXPECT_EQ(AO_ERR, ao_remove_method("B12Child", 0, "nope", nullptr));
  EXPECT_EQ("3", printIt("3 printString"));
}

// SPEC §3.9 削除, §3.10: the source entry goes with its blocks: the text, the debug info and the
// root slots (method, text, one block) are gone.
TEST_F(RemoveAbi, RemoveMethodDropsSourceEntry) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Src", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Src", 0, "blk\n  ^[:x | x + 1] value: 1\n", &err))
      << err.message;
  const ao::Oop method = methodOf("B12Src", "blk");
  ASSERT_TRUE(method.isHeap());
  std::string text;
  ASSERT_TRUE(ao::methodSource(method, text));
  ASSERT_TRUE(static_cast<bool>(ao::debugInfoFor(method)));
  const std::size_t slots = ao::methodSourceRootSlots(false).size();
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Src", 0, "blk", &rc));
  EXPECT_EQ(AO_OK, rc);
  // Nothing allocated, so the raw Oop still names the object.
  EXPECT_FALSE(ao::methodSource(method, text));
  EXPECT_FALSE(static_cast<bool>(ao::debugInfoFor(method)));
  EXPECT_EQ(slots - 3, ao::methodSourceRootSlots(false).size());
  char shown[64];
  EXPECT_EQ(AO_ERR, ao_browser_source("B12Src", 0, "blk", shown, sizeof(shown)));
}
```

- [ ] **Step 3: 失敗を確認する**

Run: `cmake --build build 2>&1 | grep -m3 error`
Expected: `ao_remove_method` が宣言されていないエラー。

- [ ] **Step 4: `forgetMethodSource` を実装する**

`runtime/src/Session.hpp` の `rememberMethodSource` の宣言のあとに:

```cpp
// SPEC §3.9 削除: drops `method`'s entry with its blocks (their root slots too). Nothing when no
// entry names it as its method. Never throws and never collects.
void forgetMethodSource(Oop method);
```

`runtime/src/Session.cpp` の `rememberMethodSource` のあとに:

```cpp
void forgetMethodSource(Oop method) {
  Session* s = session();
  if (s == nullptr || !method.isHeap()) {
    return;
  }
  for (auto it = s->methodSources.begin(); it != s->methodSources.end(); ++it) {
    if ((*it)->method == method) {
      unrootEntry(s->roots, **it);
      s->methodSources.erase(it);
      return;
    }
  }
}
```

- [ ] **Step 5: `removeMethodNamed` を実装する**

`runtime/include/ao/Compile.hpp` の `acceptClassSource` の宣言のあとに:

```cpp
// SPEC §3.9 削除. Every check runs before anything changes, and nothing here allocates on the
// heap (the names are looked up, never interned). False with the reason, never empty, in
// *reason: "not a class: <name>", "selector not found: <Class>>><selector>" (an inherited or
// unknown selector), "native method removal refused: <Class>>><selector>"; <name> and <Class>
// are className as passed, <Class> with " class" after it when meta. True: the method is out of
// its side's dictionary, the cache is invalidated for the selector, and its source entry is gone.
bool removeMethodNamed(CallContext& ctx, std::string_view className, bool meta,
                       std::string_view selector, std::string* reason);
```

`runtime/src/Compile.cpp` の無名名前空間の `hasSubclass` のあとに:

```cpp
// SPEC §3.9 削除: `Name>>selector` or `Name class>>selector`, from the name the caller passed.
std::string removedMethodName(std::string_view className, bool meta, std::string_view selector) {
  std::string key(className);
  if (meta) {
    key += " class";
  }
  key += ">>";
  key += selector;
  return key;
}
```

`acceptClassSource` のあと（`}  // namespace ao` の前）に:

```cpp
bool removeMethodNamed(CallContext& ctx, std::string_view className, bool meta,
                       std::string_view selector, std::string* reason) {
  if (!namesBehavior(ctx, className)) {
    *reason = "not a class: " + std::string(className);
    return false;
  }
  const Oop cls = ctx.wk.named(className);
  const Oop side = meta ? ctx.heap.klass(cls) : cls;
  const std::string where = removedMethodName(className, meta, selector);
  const Oop dict =
      isClassShaped(ctx.heap, side) ? ctx.heap.slotAt(side, kClassSlotMethodDict) : Oop::nil();
  // findSymbol allocates nothing; an unknown selector is one no dictionary can hold.
  const Oop sel = ctx.wk.findSymbol(selector);
  const Oop method =
      dict.isHeap() && sel.isHeap() ? MethodDictionary::at(ctx.heap, dict, sel) : Oop::nil();
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
```

- [ ] **Step 6: ABI を足す**

`bridge/ao_abi.h` の `int ao_accept_class(const char* source, AoSpan* err);` の次に:

```c
/* SPEC §3.9 削除. AO_OK: the method is out of the side's dictionary (meta 1: the metaclass's),
   the cache is invalidated and its source is forgotten; err's message is empty. AO_ERR with the
   reason in err (never empty; start and end 0): "not a class: <name>", "selector not found:
   <Class>>><selector>" (unknown or inherited), "native method removal refused:
   <Class>>><selector>", "runtime is busy", or "remove failed" (no session, a NULL argument,
   meta other than 0 or 1). <Class> is class_name as passed, with " class" when meta. Nothing
   changes on AO_ERR. Allocates nothing on the heap. */
int ao_remove_method(const char* class_name, int meta, const char* selector, AoSpan* err);
```

`runtime/src/abi.cpp` の `ao_accept_class` のあとに:

```cpp
extern "C" int ao_remove_method(const char* class_name, int meta, const char* selector,
                                AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  std::string reason;
  const int rc = guarded(-1, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr || class_name == nullptr || selector == nullptr ||
        (meta != 0 && meta != 1)) {
      return AO_ERR;
    }
    return ao::removeMethodNamed(*s->ctx, class_name, meta == 1, selector, &reason) ? AO_OK
                                                                                     : AO_ERR;
  });
  if (rc == AO_OK) {
    return AO_OK;
  }
  // SPEC §3.9 削除: an AO_ERR says why, never with an empty message (as ao_image_load).
  setMessage(err, rc == -1 || reason.empty() ? std::string_view("remove failed") : reason);
  return AO_ERR;
}
```

`abi.cpp` の `AbiEntry` の説明コメント「save, load, filein, workspace reset, eval, accept.」を「save, load, filein, workspace reset, eval, accept, remove.」に直す。

- [ ] **Step 7: テストを通す**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'RemoveAbi'`
Expected: 6 テストが PASS。`RemoveMethodDropsSourceEntry` の `slots - 3` が合わないなら、`attachBlocks` が block を 1 つ付けたか（`[:x | x + 1]`）を `ao::methodSourceRootSlots` の差で確かめる。to:do: のインライン化はブロック引数付きには効かないので block は 1 つ。

- [ ] **Step 8: Commit**

```bash
git add runtime/src/Session.hpp runtime/src/Session.cpp runtime/include/ao/Compile.hpp \
  runtime/src/Compile.cpp bridge/ao_abi.h runtime/src/abi.cpp runtime/tests/remove_abi_test.cpp
git commit -m "Add ao_remove_method

SPEC §3.9 削除: a CompiledMethod goes out of its side's dictionary, the cache is
invalidated for the selector and the source entry goes with its blocks. A native, an
inherited or unknown selector and a name that is no class are refused with their
reasons. Nothing is allocated on the heap.

Graphify: path abi.cpp Compile.cpp Session.cpp MethodDictionary
Serena: insert_after_symbol ao::acceptClassSource, ao::rememberMethodSource, ao_accept_class

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

### Task 3: `ao_remove_class`

**Files:**
- Modify: `runtime/include/ao/Compile.hpp`（`removeMethodNamed` の宣言のあと）, `runtime/src/Compile.cpp`（無名名前空間の `removedMethodName` のあとにヘルパ、`removeMethodNamed` のあとに公開関数）
- Modify: `bridge/ao_abi.h`（`ao_remove_method` のあと）, `runtime/src/abi.cpp`（`ao_remove_method` のあと）
- Test: `runtime/tests/remove_abi_test.cpp`

**Interfaces:**
- Consumes: `WellKnown::undefine`（Task 1）、`forgetMethodSource`（Task 2）、無名名前空間の `liveClasses`、`ownClassName`、`isKernelClass`、`superclassOf`（`ao/Lookup.hpp`）、`kClassSlotThisClass`。
- Produces: `bool ao::removeClassNamed(CallContext&, std::string_view className, std::string* reason)`、`int ao_remove_class(const char*, AoSpan*)`、テストのヘルパ `tryRemoveClass`、`pairIndexOf`、`globalsTally`。

- [ ] **Step 1: Serena で定義と参照を取る**

`find_symbol`: `liveClasses`、`hasSubclass`、`ownClassName`、`isKernelClass`、`WellKnown::isFixedGlobal`。`find_referencing_symbols`: `hasSubclass`（`reshapeClass` だけが呼ぶことを確認。書き換えない）。

- [ ] **Step 2: 失敗するテストを書く**

`remove_abi_test.cpp` の無名名前空間にヘルパを足す（`methodOf` のあと）:

```cpp
std::string tryRemoveClass(const char* cls, int* rc) {
  AoSpan err{};
  std::strcpy(err.message, "stale");
  err.start = 7;
  *rc = ao_remove_class(cls, &err);
  EXPECT_EQ(0u, err.start);
  EXPECT_EQ(0u, err.end);
  return err.message;
}

// The index in Smalltalk's pair array of name's key, or -1.
int pairIndexOf(const char* name) {
  ao::Session* s = ao::session();
  const ao::Oop key = s->wk.findSymbol(name);
  if (!key.isHeap()) {
    return -1;
  }
  const ao::Oop pairs = s->heap.slotAt(s->wk.smalltalk, ao::Globals::kSmalltalkSlotArray);
  for (std::uint32_t i = 0; i + 1 < s->heap.size(pairs); i += 2) {
    if (s->heap.slotAt(pairs, i) == key) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

std::int64_t globalsTally() {
  ao::Session* s = ao::session();
  return s->heap.slotAt(s->wk.smalltalk, ao::Globals::kSmalltalkSlotTally).smallIntegerValue();
}
```

テストを `RemoveMethodDropsSourceEntry` のあとに足す:

```cpp
// SPEC §3.9 削除, §6: the name reads nil (PushGlobal, no recompile), the instances keep their
// class and methods, and the Browser no longer lists the class.
TEST_F(RemoveAbi, RemovedClassNameReadsNilAndInstancesKeepWorking) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Inst", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Inst", 0, "answer\n  ^7\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12useInst\n  ^B12Inst\n", &err)) << err.message;
  EXPECT_EQ("B12Inst", printIt("Object new b12useInst"));
  // An instance prints as its class name (Object>>printString).
  EXPECT_EQ("B12Inst", printIt("b12i := B12Inst new"));
  ASSERT_TRUE(browserLists("B12Inst"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Inst", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("Object new b12useInst"));
  EXPECT_EQ("7", printIt("b12i answer"));
  EXPECT_EQ("'B12Inst'", printIt("b12i printString"));
  EXPECT_FALSE(browserLists("B12Inst"));
  EXPECT_EQ("not a class: B12Inst", tryRemoveClass("B12Inst", &rc));
  EXPECT_EQ(AO_ERR, rc);
}

// SPEC §3.9 削除, §3.10 ワークスペース変数: the globals version moved, so the name is an undeclared
// identifier again: it reads nil, and an assignment (refused while the class was there) binds it.
TEST_F(RemoveAbi, RemovedClassNameBecomesWorkspaceVariable) {
  ASSERT_EQ(AO_OK, defineClass("B12Var", "Object", "B12-Test"));
  EXPECT_EQ("B12Var", printIt("B12Var"));
  const std::string refused = printIt("B12Var := 3");
  EXPECT_EQ(0u, refused.find("<" + std::to_string(AO_ERR_COMPILE) + ": ")) << refused;
  EXPECT_NE(std::string::npos, refused.find("cannot assign")) << refused;
  const std::uint64_t version = ao::session()->wk.globalsVersion();
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Var", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ(version + 1, ao::session()->wk.globalsVersion());
  EXPECT_EQ("nil", printIt("B12Var"));
  EXPECT_EQ("3", printIt("B12Var := 3"));
  EXPECT_EQ("3", printIt("B12Var"));
}

// SPEC §3.9 削除: a fixed global (a Kernel class, a vendor stub), a name that is no class
// (Processor, Smalltalk, a metaclass, an unknown or empty name) and a NULL name are refused.
TEST_F(RemoveAbi, RemoveFixedGlobalIsRefused) {
  int rc = -9;
  EXPECT_EQ("class removal refused: Object is a fixed global", tryRemoveClass("Object", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("class removal refused: Bag is a fixed global", tryRemoveClass("Bag", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: Processor", tryRemoveClass("Processor", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: Smalltalk", tryRemoveClass("Smalltalk", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: B12Nope", tryRemoveClass("B12Nope", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("not a class: ", tryRemoveClass("", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("remove failed", tryRemoveClass(nullptr, &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(std::string::npos, printIt("Smalltalk at: #B12Meta put: Object class").find("<"));
  EXPECT_EQ("not a class: B12Meta", tryRemoveClass("B12Meta", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(AO_ERR, ao_remove_class("Object", nullptr));
  EXPECT_EQ("Object", printIt("Object"));
  EXPECT_EQ("Bag", printIt("Bag"));
}

// SPEC §3.9 削除: a Kernel class through an alias is one by identity, as ao_accept_method sees it.
TEST_F(RemoveAbi, RemoveKernelAliasIsRefused) {
  EXPECT_EQ("SmallInteger", printIt("Smalltalk at: #B12IntAlias put: SmallInteger"));
  int rc = -9;
  EXPECT_EQ("class removal refused: B12IntAlias is a kernel class", tryRemoveClass("B12IntAlias", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("SmallInteger", printIt("B12IntAlias"));
}

// SPEC §3.9 削除: a live subclass refuses the removal, named after the smallest name; a subclass
// whose name slot is no string (reflection) makes it "an unnamed subclass". The refused class is
// still bound.
TEST_F(RemoveAbi, RemoveClassWithSubclassIsRefused) {
  ASSERT_EQ(AO_OK, defineClass("B12Base", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12SubB", "B12Base", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12SubA", "B12Base", "B12-Test"));
  int rc = -9;
  EXPECT_EQ("class removal refused: B12Base has subclass B12SubA", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("B12Base", printIt("B12Base"));
  // The subclasses go (leaves first), then the base can go.
  EXPECT_EQ("", tryRemoveClass("B12SubA", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("class removal refused: B12Base has subclass B12SubB", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("", tryRemoveClass("B12SubB", &rc));
  EXPECT_EQ(AO_OK, rc);
  // An unbound subclass a workspace variable keeps alive through an instance still counts, and
  // with its name slot nilled (instVarAt:put:, kClassSlotName is slot 3, index 4) it is unnamed.
  ASSERT_EQ(AO_OK, defineClass("B12Anon", "B12Base", "B12-Test"));
  EXPECT_EQ("B12Anon", printIt("b12anon := B12Anon new"));
  // instVarAt:put: answers the value (ao_Object_instVarAt_put_).
  EXPECT_EQ("nil", printIt("B12Anon instVarAt: 4 put: nil"));
  EXPECT_EQ("", tryRemoveClass("B12Anon", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("class removal refused: B12Base has an unnamed subclass", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ("nil", printIt("b12anon := nil"));
  EXPECT_EQ("", tryRemoveClass("B12Base", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Base"));
}

// SPEC §3.9 削除: only the named binding goes; an alias keeps the class, and the Browser lists it
// under the alias.
TEST_F(RemoveAbi, RemoveClassKeepsAliases) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Real", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Real", 0, "answer\n  ^9\n", &err)) << err.message;
  EXPECT_EQ("B12Real", printIt("Smalltalk at: #B12Alias put: B12Real"));
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Real", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ("nil", printIt("B12Real"));
  EXPECT_EQ("9", printIt("B12Alias new answer"));
  EXPECT_TRUE(browserLists("B12Alias"));
  EXPECT_FALSE(browserLists("B12Real"));
  EXPECT_EQ("", tryRemoveClass("B12Alias", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_FALSE(browserLists("B12Alias"));
}

// SPEC §3.6: the emptied pair is the one the next registration takes; the tally follows.
TEST_F(RemoveAbi, RemovedGlobalSlotIsReused) {
  ASSERT_EQ(AO_OK, defineClass("B12Slot", "Object", "B12-Test"));
  const int index = pairIndexOf("B12Slot");
  ASSERT_NE(-1, index);
  const std::int64_t tally = globalsTally();
  int rc = -9;
  EXPECT_EQ("", tryRemoveClass("B12Slot", &rc));
  EXPECT_EQ(AO_OK, rc);
  EXPECT_EQ(-1, pairIndexOf("B12Slot"));
  EXPECT_EQ(tally - 1, globalsTally());
  EXPECT_EQ("3", printIt("Smalltalk at: #B12Next put: 3"));
  EXPECT_EQ(index, pairIndexOf("B12Next"));
  EXPECT_EQ(tally, globalsTally());
  EXPECT_EQ("3", printIt("B12Next"));
}
```

- [ ] **Step 3: 失敗を確認する**

Run: `cmake --build build 2>&1 | grep -m3 error`
Expected: `ao_remove_class` が宣言されていないエラー。

- [ ] **Step 4: ヘルパと `removeClassNamed` を実装する**

`runtime/src/Compile.cpp` の無名名前空間、`removedMethodName` のあとに:

```cpp
// SPEC §3.9 削除: obj is a class, not a metaclass, Processor, Smalltalk or another instance: the
// thisClass of its metaclass, which is an instance of Metaclass (the test liveClasses applies).
bool isClassObject(CallContext& ctx, Oop obj) {
  if (!isClassShaped(ctx.heap, obj)) {
    return false;
  }
  const Oop meta = ctx.heap.klass(obj);
  return isClassShaped(ctx.heap, meta) && ctx.heap.klass(meta) == ctx.wk.metaclassClass &&
         ctx.heap.slotAt(meta, kClassSlotThisClass) == obj;
}

// SPEC §3.9 削除: what the live subclasses of cls (hasSubclass's classes) say for the refusal.
struct SubclassProbe {
  bool any = false;
  // The smallest name, bytewise, among the subclasses whose name slot is a string; "" when none is.
  std::string name;
};

SubclassProbe probeSubclasses(CallContext& ctx, Oop cls) {
  SubclassProbe probe;
  for (const Oop each : liveClasses(ctx)) {
    if (superclassOf(ctx.heap, each) != cls) {
      continue;
    }
    probe.any = true;
    const std::string name = ownClassName(ctx, each);
    if (!name.empty() && (probe.name.empty() || name < probe.name)) {
      probe.name = name;
    }
  }
  return probe;
}

// SPEC §3.9 削除: drops the source entries of every method in side's dictionary.
void forgetMethodSourcesOf(CallContext& ctx, Oop side) {
  const Oop dict =
      isClassShaped(ctx.heap, side) ? ctx.heap.slotAt(side, kClassSlotMethodDict) : Oop::nil();
  if (!dict.isHeap() || (ctx.heap.flags(dict) & kFlagBytes) != 0 ||
      ctx.heap.size(dict) <= kDictSlotArray) {
    return;
  }
  const Oop inner = ctx.heap.slotAt(dict, kDictSlotArray);
  if (!inner.isHeap() || (ctx.heap.flags(inner) & kFlagBytes) != 0) {
    return;
  }
  const std::uint32_t n = ctx.heap.size(inner);
  for (std::uint32_t i = 0; i + 1 < n; i += 2) {
    const Oop method = ctx.heap.slotAt(inner, i + 1);
    if (method.isHeap()) {
      forgetMethodSource(method);
    }
  }
}
```

`runtime/include/ao/Compile.hpp` の `removeMethodNamed` の宣言のあとに:

```cpp
// SPEC §3.9 削除. Every check runs before anything changes; nothing here allocates on the heap.
// False with the reason in *reason: "not a class: <name>" (a metaclass, Processor, Smalltalk, an
// unknown name), "class removal refused: <name> is a fixed global", "... is a kernel class" (by
// identity, so an alias too), "... has subclass <Sub>" (the smallest name of its live subclasses)
// or "... has an unnamed subclass". True: the binding is out of Smalltalk (the globals version
// moved), the whole cache is dropped, and the source entries of the class's methods on both sides
// are gone. The class object, its metaclass, its dictionaries and its aliases are untouched.
bool removeClassNamed(CallContext& ctx, std::string_view className, std::string* reason);
```

`runtime/src/Compile.cpp` の `removeMethodNamed` のあとに:

```cpp
bool removeClassNamed(CallContext& ctx, std::string_view className, std::string* reason) {
  const Oop cls = ctx.wk.named(className);
  if (!isClassObject(ctx, cls)) {
    *reason = "not a class: " + std::string(className);
    return false;
  }
  const std::string refused = "class removal refused: " + std::string(className);
  if (ctx.wk.isFixedGlobal(className)) {
    *reason = refused + " is a fixed global";
    return false;
  }
  if (isKernelClass(ctx.wk, cls)) {
    *reason = refused + " is a kernel class";
    return false;
  }
  const SubclassProbe sub = probeSubclasses(ctx, cls);
  if (sub.any) {
    *reason = sub.name.empty() ? refused + " has an unnamed subclass"
                               : refused + " has subclass " + sub.name;
    return false;
  }
  if (!ctx.wk.undefine(className)) {
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

- [ ] **Step 5: ABI を足す**

`bridge/ao_abi.h` の `ao_remove_method` の次に:

```c
/* SPEC §3.9 削除. AO_OK: the name's binding is out of Smalltalk (an alias stays; the class object
   and its instances are untouched), the whole method cache is dropped, and the sources of its
   methods are forgotten; err's message is empty. AO_ERR with the reason: "not a class: <name>"
   (a metaclass, Processor, Smalltalk, an unknown name), "class removal refused: <name> is a fixed
   global" / "is a kernel class" (by identity, an alias too) / "has subclass <Sub>" / "has an
   unnamed subclass", "runtime is busy", or "remove failed" (no session, a NULL name). Nothing
   changes on AO_ERR. Allocates nothing on the heap. */
int ao_remove_class(const char* class_name, AoSpan* err);
```

`runtime/src/abi.cpp` の `ao_remove_method` のあとに:

```cpp
extern "C" int ao_remove_class(const char* class_name, AoSpan* err) {
  clearSpan(err);
  const AbiEntry entry;
  if (!entry.entered()) {
    setMessage(err, "runtime is busy");
    return AO_ERR;
  }
  std::string reason;
  const int rc = guarded(-1, [&] {
    ao::Session* s = ao::session();
    if (s == nullptr || s->ctx == nullptr || class_name == nullptr) {
      return AO_ERR;
    }
    return ao::removeClassNamed(*s->ctx, class_name, &reason) ? AO_OK : AO_ERR;
  });
  if (rc == AO_OK) {
    return AO_OK;
  }
  setMessage(err, rc == -1 || reason.empty() ? std::string_view("remove failed") : reason);
  return AO_ERR;
}
```

- [ ] **Step 6: テストを通す**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'RemoveAbi'`
Expected: 13 テストが PASS。`RemoveClassWithSubclassIsRefused` の `instVarAt: 4 put: nil` が `index out of range` で失敗するなら、`kClassSlotName`（`ao/Bootstrap.hpp` で 3）+1 に直す。

- [ ] **Step 7: Commit**

```bash
git add runtime/include/ao/Compile.hpp runtime/src/Compile.cpp bridge/ao_abi.h runtime/src/abi.cpp \
  runtime/tests/remove_abi_test.cpp
git commit -m "Add ao_remove_class

SPEC §3.9 削除: only the named binding leaves Smalltalk; the class object, its instances
and its aliases stay. A fixed global, a Kernel class by identity and a class with a live
subclass are refused with their reasons. The globals version moves, so the workspace
treats the name as undeclared again.

Graphify: path abi.cpp Compile.cpp Globals WellKnown
Serena: insert_after_symbol ao::removeMethodNamed, ao_remove_method, hasSubclass

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

### Task 4: 横断の規則（busy、無変更、保存とロード、Kernel 走査、止まったプロセス、長い名前）

**Files:**
- Modify: `bridge/ao_abi.h`（busy の説明コメント）
- Test: `runtime/tests/remove_abi_test.cpp`

**Interfaces:**
- Consumes: Task 2〜3 の ABI と `ao::session()`、`ao::methodSourceRootSlots`、`wk.eachNativeRequiredClass`。

- [ ] **Step 1: テストを書く**

`remove_abi_test.cpp` の無名名前空間に足す（`globalsTally` のあと）:

```cpp
// SPEC §6: every pair left in a native-required dictionary (both sides) holds a NativeMethod; an
// emptied pair (nil key) is skipped.
bool kernelDictsAreNative() {
  struct Scan {
    ao::Session* s;
    bool ok = true;
    void visit(ao::Oop cls) {
      if (!cls.isHeap()) return;
      const ao::Oop dict = s->heap.slotAt(cls, ao::kClassSlotMethodDict);
      if (!dict.isHeap()) return;
      const ao::Oop inner = s->heap.slotAt(dict, ao::kDictSlotArray);
      if (!inner.isHeap()) return;
      for (std::uint32_t i = 0; i + 1 < s->heap.size(inner); i += 2) {
        if (s->heap.slotAt(inner, i).isNil()) continue;
        const ao::Oop v = s->heap.slotAt(inner, i + 1);
        if (!v.isHeap() || s->heap.klass(v) != s->wk.nativeMethodClass) ok = false;
      }
    }
  } scan{ao::session()};
  scan.s->wk.eachNativeRequiredClass(
      [](void* p, ao::Oop cls) {
        auto* sc = static_cast<Scan*>(p);
        sc->visit(cls);
        sc->visit(sc->s->heap.klass(cls));
      },
      &scan);
  return scan.ok;
}

struct BusyRemove {
  int methodRc = -9;
  int classRc = -9;
  std::string methodMsg;
  std::string classMsg;
};

// A transcript hook: the runtime is busy here (SPEC §3.10 再入).
void removeFromHook(const char*, int, int, void* user) {
  auto* b = static_cast<BusyRemove*>(user);
  AoSpan err{};
  b->methodRc = ao_remove_method("Object", 0, "b12busy", &err);
  b->methodMsg = err.message;
  b->classRc = ao_remove_class("B12Busy", &err);
  b->classMsg = err.message;
}
```

テストを `RemovedGlobalSlotIsReused` のあとに足す:

```cpp
// SPEC §3.10 再入と例外: from a hook the runtime is busy, and both calls do nothing.
TEST_F(RemoveAbi, RemoveWhileBusyIsRefused) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12busy\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, defineClass("B12Busy", "Object", "B12-Test"));
  BusyRemove seen;
  ao_set_transcript_hook(removeFromHook, &seen);
  EXPECT_EQ("7", printIt("Transcript show: 'x'. 7"));
  ao_set_transcript_hook(nullptr, nullptr);
  EXPECT_EQ(AO_ERR, seen.methodRc);
  EXPECT_EQ("runtime is busy", seen.methodMsg);
  EXPECT_EQ(AO_ERR, seen.classRc);
  EXPECT_EQ("runtime is busy", seen.classMsg);
  EXPECT_EQ("1", printIt("3 b12busy"));
  EXPECT_TRUE(browserLists("B12Busy"));
}

// SPEC §3.9 削除: a refused call leaves the dictionaries, the cache, the globals version and the
// source table as they were.
TEST_F(RemoveAbi, RefusedRemoveChangesNothing) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Par", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12Kid2", "B12Par", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Par", 0, "who\n  ^'par'\n", &err)) << err.message;
  EXPECT_EQ("'par'", printIt("B12Kid2 new who"));
  const int classes = ao_browser_class_count();
  const std::int64_t tally = globalsTally();
  const std::uint64_t version = ao::session()->wk.globalsVersion();
  const std::size_t slots = ao::methodSourceRootSlots(false).size();
  const int protocols = ao_browser_protocol_count("Object", 0);
  int rc = -9;
  EXPECT_EQ(AO_ERR, ao_remove_method("Object", 0, "printString", &err));
  EXPECT_EQ(AO_ERR, ao_remove_method("B12Kid2", 0, "who", &err));
  EXPECT_EQ(AO_ERR, ao_remove_class("Object", &err));
  EXPECT_EQ(AO_ERR, ao_remove_class("B12Par", &err));
  EXPECT_EQ("class removal refused: B12Par has subclass B12Kid2", tryRemoveClass("B12Par", &rc));
  EXPECT_EQ(classes, ao_browser_class_count());
  EXPECT_EQ(tally, globalsTally());
  EXPECT_EQ(version, ao::session()->wk.globalsVersion());
  EXPECT_EQ(slots, ao::methodSourceRootSlots(false).size());
  EXPECT_EQ(protocols, ao_browser_protocol_count("Object", 0));
  EXPECT_EQ("'par'", printIt("B12Kid2 new who"));
  EXPECT_EQ("3", printIt("3 printString"));
  char shown[64];
  EXPECT_EQ(AO_OK, ao_browser_source("B12Par", 0, "who", shown, sizeof(shown)));
}

// SPEC §3.9 削除, §6: a removal is in the saved image: the method and the binding stay gone.
TEST_F(RemoveAbi, RemovalSurvivesSaveAndLoad) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, defineClass("B12Keep", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, defineClass("B12Gone", "Object", "B12-Test"));
  ASSERT_EQ(AO_OK, ao_accept_method("B12Keep", 0, "foo\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B12Keep", 0, "bar\n  ^2\n", &err)) << err.message;
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("B12Keep", 0, "foo", &rc));
  EXPECT_EQ("", tryRemoveClass("B12Gone", &rc));
  const char* path = "remove-abi.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path, &err)) << err.message;
  std::remove(path);
  expectDnu("B12Keep new foo", "foo");
  EXPECT_EQ("2", printIt("B12Keep new bar"));
  EXPECT_EQ("nil", printIt("B12Gone"));
  EXPECT_FALSE(browserLists("B12Gone"));
  EXPECT_TRUE(browserLists("B12Keep"));
  // A loaded image's method has no source; removing it still works.
  EXPECT_EQ("", tryRemoveMethod("B12Keep", 0, "bar", &rc));
  EXPECT_EQ(AO_OK, rc);
  expectDnu("B12Keep new bar", "bar");
}

// SPEC §6: accepting a CompiledMethod on Kernel classes and removing it leaves every Kernel
// dictionary all-native, holes included.
TEST_F(RemoveAbi, KernelScanStaysGreenAfterRemovals) {
  AoSpan err{};
  ASSERT_TRUE(kernelDictsAreNative());
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12scan\n  ^1\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("SmallInteger", 0, "b12scanInt\n  ^2\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 1, "b12scanMeta\n  ^3\n", &err)) << err.message;
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("Object", 0, "b12scan", &rc));
  EXPECT_EQ("", tryRemoveMethod("SmallInteger", 0, "b12scanInt", &rc));
  EXPECT_EQ("", tryRemoveMethod("Object", 1, "b12scanMeta", &rc));
  EXPECT_TRUE(kernelDictsAreNative());
  EXPECT_EQ("3", printIt("3 printString"));
  EXPECT_EQ("7", printIt("3 + 4"));
}

// SPEC §3.9 削除: a halted process keeps running the method it was in; Proceed answers from it.
TEST_F(RemoveAbi, RemovedMethodKeepsRunningInHaltedProcess) {
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_method("Object", 0, "b12halt\n  self halt.\n  ^5\n", &err))
      << err.message;
  ao_set_debug_mode(AO_DEBUG_LIVE);
  char out[64];
  ASSERT_EQ(AO_ERR_HALT, ao_eval("Object new b12halt", 18, AO_EVAL_PRINTIT, out, 64, &err));
  const std::int64_t pid = ao_debug_halted_pid();
  ASSERT_NE(0, pid);
  int rc = -9;
  EXPECT_EQ("", tryRemoveMethod("Object", 0, "b12halt", &rc));
  EXPECT_EQ(AO_OK, rc);
  ASSERT_EQ(AO_OK, ao_debug_proceed(pid, out, 64, &err)) << err.message;
  EXPECT_STREQ("5", out);
  expectDnu("Object new b12halt", "b12halt");
}

// SPEC §3.10: AoSpan.message holds 255 bytes; a longer reason is cut, never emptied.
TEST_F(RemoveAbi, LongNameCutsTheMessageAt255Bytes) {
  const std::string name(300, 'N');
  int rc = -9;
  const std::string got = tryRemoveClass(name.c_str(), &rc);
  EXPECT_EQ(AO_ERR, rc);
  EXPECT_EQ(255u, got.size());
  EXPECT_EQ(("not a class: " + name).substr(0, 255), got);
}
```

- [ ] **Step 2: 走らせる**

Run: `cmake --build build && ctest --test-dir build --output-on-failure -R 'RemoveAbi'`
Expected: 19 テストが PASS。赤があれば実装を直す（テストの期待を緩めない）。`RemovedMethodKeepsRunningInHaltedProcess` で `ao_debug_proceed` の `out` が `"5"` でなく `nil` なら、`ao_debug_proceed` の規則（`halt` の送信の値として nil を返し、メソッドは続く）を `debug_abi_test.cpp` の Proceed のテストで確かめて期待を合わせる。

- [ ] **Step 3: `ao_abi.h` の busy 一覧を直す**

冒頭のコメントで:
- `ao_accept_method,\n   ao_accept_class, ao_debug_frame_receiver_print` → `ao_accept_method,\n   ao_accept_class, ao_remove_method, ao_remove_class, ao_debug_frame_receiver_print`
- `each of these eighteen does nothing` → `each of these twenty does nothing`
- `ao_image_load\n   with the reason "runtime is busy"` → `ao_image_load, ao_remove_method and ao_remove_class\n   with the reason "runtime is busy"`

- [ ] **Step 4: 全スイートを回す**

Run: `ctest --test-dir build --output-on-failure` と `ctest --test-dir build --output-on-failure -R gcstress`
Expected: すべて PASS（gcstress は `AO_GC_STRESS=1` で同じスイート）。

- [ ] **Step 5: Commit**

```bash
git add bridge/ao_abi.h runtime/tests/remove_abi_test.cpp
git commit -m "Cover the removal rules that cut across both calls

Busy refusal, a refused call changing nothing, save and load, the Kernel scan with
holes, a halted frame outliving its method, and the 255-byte message.

Graphify: query remove_abi_test abi.cpp
Serena: find_referencing_symbols AbiEntry

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

### Task 5: Browser の削除（確認、実行、一覧の更新）

**Files:**
- Modify: `app/Ao/BrowserModel.swift`（`select(category:className:meta:protocol:selector:)`）
- Modify: `app/Ao/BrowserWindow.swift`
- Test: `app/AoTests/AcceptTests.swift`

**Interfaces:**
- Consumes: `ao_remove_method`、`ao_remove_class`（CAo 経由）、`spanMessage(_:)`（`WorkspaceWindow.swift`）。
- Produces: `BrowserWindow.RemoveConfirmation`、`var confirmRemove`、`var canRemoveMethod: Bool`、`var canRemoveClass: Bool`、`func removeMethod()`、`func removeClass()`、`func removeMethod(atRow:)`、`func removeClass(atRow:)`。`BrowserModel.select(... className: String? ...)`。

- [ ] **Step 1: Serena で外形を取る**

`get_symbols_overview` で `app/Ao/BrowserWindow.swift`（`categoryName`、`selectedClass: String`、`meta: Bool`、`protocolName: String?`、`selectorName: String?`、`confirming`、`publish`、`confirmBeforeDiscarding`、`value(at:in:)` の型を確認）。`find_referencing_symbols` で `BrowserModel.select`。

- [ ] **Step 2: 失敗するテストを書く**

`app/AoTests/AcceptTests.swift` の `testAcceptRunsOnlyWhenBrowserIsKey` の前に足す:

```swift
  // SPEC §3.9 削除, §4.3: Remove Method… asks with the method's name; Remove takes it out, the
  // cached send falls to doesNotUnderstand:, the selector row goes and the protocol stays while it
  // lists a method. The class side asks with `Foo class>>bar`.
  func testRemoveMethodAfterConfirmUpdatesLists() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("b12a\n  ^1\n")
    browser.accept()
    browser.replaceSource("b12b\n  ^2\n")
    browser.accept()
    XCTAssertEqual(browser.model.selectedSelector, "b12b")
    XCTAssertEqual(printIt("Object new b12b"), "2")
    var asked: [String] = []
    browser.confirmRemove = { window, message, decide in
      XCTAssertTrue(window === browser.window)
      asked.append(message)
      decide(true)
    }
    XCTAssertTrue(browser.canRemoveMethod)
    browser.removeMethod()
    XCTAssertEqual(asked, ["Remove Object>>b12b?"])
    XCTAssertEqual(browser.errorText, "")
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertEqual(browser.model.selectedProtocol, "user")
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(browser.model.selectors, ["b12a"])
    XCTAssertNil(selectedName(in: selectorTable(in: browser), values: browser.model.selectors))
    XCTAssertEqual(browser.sourceText, "")
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertFalse(browser.canRemoveMethod)
    XCTAssertNil(printIt("Object new b12b"))
    XCTAssertEqual(printIt("Object new b12a"), "1")
    // Nothing selected: the menu action does nothing and asks nothing.
    browser.removeMethod()
    XCTAssertEqual(asked.count, 1)

    var err = AoSpan()
    let accepted = "b12c\n  ^3\n".withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_method("Object", 1, src, $0) }
    }
    XCTAssertEqual(accepted, Int32(AO_OK), spanMessage(err))
    guard let side = segmentedControls(in: browser.window.contentView).first else {
      XCTFail("missing instance/class switch")
      return
    }
    side.selectedSegment = 1
    XCTAssertTrue(side.sendAction(side.action, to: side.target))
    selectProtocol("user", in: browser)
    selectSelector("b12c", in: browser)
    browser.removeMethod()
    XCTAssertEqual(asked, ["Remove Object>>b12b?", "Remove Object class>>b12c?"])
    XCTAssertNil(printIt("Object b12c"))
    XCTAssertEqual(browser.errorText, "")
  }

  // SPEC §3.9 削除: Cancel changes nothing: the method, the class, the rows and the pane stay.
  func testCancelRemoveChangesNothing() {
    var err = AoSpan()
    let def = "Object subclass: #B12Cancel\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B12-Cancel'\n"
    let defined = def.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
    XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("b12keep\n  ^1\n")
    browser.accept()
    var asked = 0
    browser.confirmRemove = { _, _, decide in
      asked += 1
      decide(false)
    }
    browser.removeMethod()
    XCTAssertEqual(asked, 1)
    XCTAssertEqual(browser.model.selectedSelector, "b12keep")
    XCTAssertEqual(browser.model.selectors, ["b12keep"])
    XCTAssertEqual(browser.sourceText, "b12keep\n  ^1\n")
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("Object new b12keep"), "1")

    selectCategory("B12-Cancel", in: browser)
    selectClass("B12Cancel", in: browser)
    browser.removeClass()
    XCTAssertEqual(asked, 2)
    XCTAssertEqual(browser.model.selectedClass, "B12Cancel")
    XCTAssertEqual(browser.model.classes, ["B12Cancel"])
    XCTAssertTrue(browser.model.categories.contains("B12-Cancel"))
    XCTAssertEqual(printIt("B12Cancel"), "B12Cancel")
    XCTAssertTrue(browser.sourceText.contains("subclass: #B12Cancel"))
  }

  // SPEC §3.9 削除, §6: Remove Class… takes the class off the lists; its category goes with its
  // last class (the first category is shown then), a category with a class left keeps it with no
  // class selected, and the hierarchy list loses the name too.
  func testRemoveClassAfterConfirmUpdatesLists() {
    var err = AoSpan()
    for (name, category) in [("B12Gone", "B12-Gone"), ("B12StayA", "B12-Stay"), ("B12StayB", "B12-Stay")] {
      let def = "Object subclass: #\(name)\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: '\(category)'\n"
      let defined = def.withCString { src in
        withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
      }
      XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    }
    let browser = BrowserWindow()
    defer { browser.window.close() }
    XCTAssertEqual(printIt("B12Gone new printString"), "'B12Gone'")
    var asked: [String] = []
    browser.confirmRemove = { _, message, decide in
      asked.append(message)
      decide(true)
    }
    selectCategory("B12-Gone", in: browser)
    selectClass("B12Gone", in: browser)
    XCTAssertTrue(browser.canRemoveClass)
    browser.removeClass()
    XCTAssertEqual(asked, ["Remove class B12Gone?"])
    XCTAssertEqual(browser.errorText, "")
    XCTAssertFalse(browser.model.classes.contains("B12Gone"))
    XCTAssertFalse(browser.model.categories.contains("B12-Gone"))
    XCTAssertNil(browser.model.selectedClass)
    XCTAssertFalse(browser.canRemoveClass)
    XCTAssertEqual(selectedName(in: categoryTable(in: browser), values: browser.model.categories),
                   browser.model.categories.first)
    XCTAssertEqual(printIt("B12Gone"), "nil")
    // Nothing selected: the menu action does nothing.
    browser.removeClass()
    XCTAssertEqual(asked.count, 1)

    selectCategory("B12-Stay", in: browser)
    selectClass("B12StayA", in: browser)
    browser.removeClass()
    XCTAssertEqual(asked.last, "Remove class B12StayA?")
    XCTAssertTrue(browser.model.categories.contains("B12-Stay"))
    XCTAssertEqual(selectedName(in: categoryTable(in: browser), values: browser.model.categories), "B12-Stay")
    XCTAssertEqual(browser.model.classes, ["B12StayB"])
    XCTAssertNil(browser.model.selectedClass)
    XCTAssertNil(selectedName(in: classTable(in: browser), values: browser.model.classes))
    XCTAssertEqual(browser.sourceText, "")

    selectClass("B12StayB", in: browser)
    browser.showHierarchy()
    XCTAssertTrue(browser.model.classes.contains("Object"))
    XCTAssertTrue(browser.model.classes.contains("B12StayB"))
    browser.removeClass()
    XCTAssertEqual(asked.last, "Remove class B12StayB?")
    XCTAssertTrue(browser.model.classes.contains("Object"))
    XCTAssertFalse(browser.model.classes.contains("B12StayB"))
    XCTAssertNil(browser.model.selectedClass)
    XCTAssertEqual(printIt("B12StayB"), "nil")
  }

  // SPEC §3.9 削除: a refusal shows its reason in the error field and changes no row or pane.
  func testRefusedRemoveShowsReason() {
    var err = AoSpan()
    for (name, superclass) in [("B12Par", "Object"), ("B12Kid", "B12Par")] {
      let def = "\(superclass) subclass: #\(name)\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'B12-Sub'\n"
      let defined = def.withCString { src in
        withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
      }
      XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    }
    let browser = BrowserWindow()
    defer { browser.window.close() }
    browser.confirmRemove = { _, _, decide in decide(true) }
    // Kernel / Object / native / printString is the initial selection.
    XCTAssertEqual(browser.model.selectedSelector, "printString")
    let shown = browser.sourceText
    let selectors = browser.model.selectors
    browser.removeMethod()
    XCTAssertEqual(browser.errorText, "native method removal refused: Object>>printString")
    XCTAssertEqual(browser.model.selectedSelector, "printString")
    XCTAssertEqual(browser.model.selectors, selectors)
    XCTAssertEqual(browser.sourceText, shown)
    XCTAssertEqual(selectedName(in: selectorTable(in: browser), values: browser.model.selectors), "printString")

    let classes = browser.model.classes
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "class removal refused: Object is a fixed global")
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(browser.model.classes, classes)
    XCTAssertEqual(browser.model.selectedSelector, "printString")

    selectCategory("B12-Sub", in: browser)
    selectClass("B12Par", in: browser)
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "class removal refused: B12Par has subclass B12Kid")
    XCTAssertEqual(browser.model.classes, ["B12Par", "B12Kid"])
    XCTAssertEqual(browser.model.selectedClass, "B12Par")
    // The next successful action clears the field.
    selectClass("B12Kid", in: browser)
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(browser.model.classes, ["B12Par"])
  }

  // SPEC §3.9 削除: an unaccepted edit asks to discard first; keeping it ends the command, and
  // discarding it goes on to the removal question.
  func testRemoveAsksToDiscardEditsFirst() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("b12x\n  ^1\n")
    browser.accept()
    browser.replaceSource("b12y\n  ^2\n")
    browser.accept()
    var discardAsked = 0
    var discard = false
    browser.confirmDiscard = { _, decide in
      discardAsked += 1
      decide(discard)
    }
    var removeAsked: [String] = []
    browser.confirmRemove = { _, message, decide in
      removeAsked.append(message)
      decide(true)
    }
    let edited = "b12y\n  ^9\n"
    browser.replaceSource(edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    browser.removeMethod()
    XCTAssertEqual(discardAsked, 1)
    XCTAssertEqual(removeAsked, [])
    XCTAssertEqual(browser.sourceText, edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    XCTAssertEqual(printIt("Object new b12y"), "2")

    discard = true
    browser.removeMethod()
    XCTAssertEqual(discardAsked, 2)
    XCTAssertEqual(removeAsked, ["Remove Object>>b12y?"])
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertNil(printIt("Object new b12y"))
    XCTAssertEqual(browser.model.selectors, ["b12x"])
    // The class command asks the same way.
    browser.replaceSource("b12z\n  ^3\n")
    discard = false
    browser.removeClass()
    XCTAssertEqual(discardAsked, 3)
    XCTAssertEqual(removeAsked.count, 1)
    XCTAssertEqual(browser.sourceText, "b12z\n  ^3\n")
  }
```

- [ ] **Step 3: 失敗を確認する**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'AcceptTests/testRemove' 2>&1 | grep -m5 'error:'`
Expected: `confirmRemove`、`removeMethod`、`canRemoveMethod` が無いというコンパイルエラー。

- [ ] **Step 4: `BrowserModel.select` でクラス未選択を保てるようにする**

`app/Ao/BrowserModel.swift` の `select` の引数 `className: String` を `className: String?` にする（`selectedClass = className` はそのまま。`refresh()` の `if let current = selectedClass, !classes.contains(current)` は nil を nil のまま保つ）。宣言の上にコメント:

```swift
  // className nil: no class is selected (after Remove Class…); refresh keeps it nil.
```

- [ ] **Step 5: `BrowserWindow` に確認と実行を足す**

クラス宣言の準拠リストに `NSMenuDelegate` を足す（Task 6 で使う。ここで足しても害は無い）。

`var confirmDiscard: DiscardConfirmation = BrowserWindow.askToDiscard` の次に:

```swift
  // SPEC §3.9 削除: asked before a removal. The text is the sheet's message (`Remove Foo>>bar?`,
  // `Remove Foo class>>bar?`, `Remove class Foo?`); the callback gets true to remove. Tests
  // replace it.
  typealias RemoveConfirmation = @MainActor (NSWindow, String, @escaping @MainActor (Bool) -> Void) -> Void
  var confirmRemove: RemoveConfirmation = BrowserWindow.askToRemove

  // SPEC §3.9 削除: the Remove items are enabled for a selected selector or class.
  var canRemoveMethod: Bool {
    selectorName != nil
  }

  var canRemoveClass: Bool {
    !selectedClass.isEmpty
  }
```

`func showHierarchy()` の前に:

```swift
  // SPEC §3.9 削除: Smalltalk → Remove Method…, for the selected selector.
  func removeMethod() {
    removeMethod(atRow: selectorName.flatMap { model.selectors.firstIndex(of: $0) } ?? -1)
  }

  // SPEC §3.9 削除: Smalltalk → Remove Class…, for the selected class.
  func removeClass() {
    removeClass(atRow: model.classes.firstIndex(of: selectedClass) ?? -1)
  }

  // SPEC §3.9 削除: the context menu's item, for the clicked row (-1: none). The row is selected
  // before the question when it is another one.
  func removeMethod(atRow row: Int) {
    guard let selector = value(at: row, in: model.selectors) else {
      return
    }
    let className = selectedClass
    let classSide = meta
    let message = "Remove \(className)\(classSide ? " class" : "")>>\(selector)?"
    removeAfterConfirming(
      message: message,
      select: selector == selectorName ? nil : { self.selectorName = selector }
    ) {
      self.performRemoveMethod(selector, ofClass: className, meta: classSide)
    }
  }

  func removeClass(atRow row: Int) {
    guard let name = value(at: row, in: model.classes) else {
      return
    }
    removeAfterConfirming(
      message: "Remove class \(name)?",
      select: name == selectedClass ? nil : {
        self.selectedClass = name
        self.protocolName = nil
        self.selectorName = nil
      }
    ) {
      self.performRemoveClass(name)
    }
  }

  // SPEC §3.9 削除: an unaccepted edit asks first, as a selection change does; keeping it ends
  // here. Discarding (or no edit) selects `select`'s row, republishes (the pane shows the
  // selection again) and asks the removal question. A question already up decides alone.
  private func removeAfterConfirming(
    message: String,
    select: (() -> Void)?,
    remove: @escaping () -> Void
  ) {
    guard !confirming else {
      return
    }
    let ask = {
      select?()
      self.publish()
      self.confirming = true
      self.confirmRemove(self.window, message) { yes in
        self.confirming = false
        if yes {
          remove()
        }
      }
    }
    guard hasUnacceptedChanges else {
      ask()
      return
    }
    confirmBeforeDiscarding { proceed in
      if proceed {
        ask()
      }
    }
  }

  // SPEC §3.9 削除: a refusal shows its reason and changes nothing else. Success keeps the
  // category, the class, the side and (while it is still listed) the protocol, and deselects the
  // selector.
  private func performRemoveMethod(_ selector: String, ofClass className: String, meta classSide: Bool) {
    var err = AoSpan()
    let metaFlag: Int32 = classSide ? 1 : 0
    let status = className.withCString { name in
      selector.withCString { sel in
        withUnsafeMutablePointer(to: &err) { errPtr in
          ao_remove_method(name, metaFlag, sel, errPtr)
        }
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
  // drops the name too.
  private func performRemoveClass(_ className: String) {
    var err = AoSpan()
    let status = className.withCString { name in
      withUnsafeMutablePointer(to: &err) { errPtr in
        ao_remove_class(name, errPtr)
      }
    }
    guard status == Int32(AO_OK) else {
      errorField.stringValue = spanMessage(err)
      return
    }
    errorField.stringValue = ""
    selectedClass = ""
    protocolName = nil
    selectorName = nil
    hierarchyNames.removeAll { $0 == className }
    publish()
    if !model.categories.contains(categoryName), let first = model.categories.first {
      categoryName = first
      publish()
    }
  }
```

`publish()` の `model.select(` の `className: selectedClass,` を `className: selectedClass.isEmpty ? nil : selectedClass,` にする。

`private static func askToDiscard` のあとに:

```swift
  // SPEC §3.9 削除: Remove first and destructive, which leaves the sheet without a default button,
  // so Return answers nothing; Cancel keeps the Escape key.
  private static func askToRemove(
    _ window: NSWindow,
    _ message: String,
    _ decide: @escaping @MainActor (Bool) -> Void
  ) {
    let alert = NSAlert()
    alert.messageText = message
    alert.informativeText = "This cannot be undone."
    alert.addButton(withTitle: "Remove").hasDestructiveAction = true
    alert.addButton(withTitle: "Cancel")
    alert.beginSheetModal(for: window) { response in
      MainActor.assumeIsolated {
        decide(response == .alertFirstButtonReturn)
      }
    }
  }
```

- [ ] **Step 6: テストを通す**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'AcceptTests/testRemove'`
Expected: 5 テストが PASS。`selectedClass` が `String` でなく `String?` なら `canRemoveClass` と `publish` の式をそれに合わせる。`testRemoveClassAfterConfirmUpdatesLists` で `selectedClass` が `nil` にならないなら、`BrowserModel.refresh()` が nil を保っているか（Step 4）と `publish()` が nil を渡しているかを見る。

- [ ] **Step 7: 既存の Browser テストを回す**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'AcceptTests|BrowserModelTests|ToolWindowTests'`
Expected: PASS。`BrowserModel.select` の引数の型変更で赤になるテストは無いはず（`String` は `String?` に暗黙変換される）。

- [ ] **Step 8: Commit**

```bash
git add app/Ao/BrowserModel.swift app/Ao/BrowserWindow.swift app/AoTests/AcceptTests.swift
git commit -m "Remove methods and classes from the Browser after a confirmation

SPEC §3.9 削除: removeMethod and removeClass ask through confirmRemove (tests replace
it), an unaccepted edit asks to be discarded first, a refusal goes to the error field,
and success reloads the lists with the selector or the class deselected.

Graphify: path BrowserWindow BrowserModel ao_abi.h
Serena: insert_before_symbol BrowserWindow.showHierarchy; replace_symbol_body BrowserWindow.publish

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

### Task 6: メニュー（右クリック、Smalltalk メニュー、有効・無効）

**Files:**
- Modify: `app/Ao/BrowserWindow.swift`（`init` の `configure(selectorTable)` のあと、`ownsWindow` のあと、末尾の `sendToKeyBrowser` のあと）
- Modify: `app/Ao/MainMenu.swift`
- Modify: `app/Ao/AoApp.swift`（`accept:` の配線のあと）
- Test: `app/AoTests/AcceptTests.swift`

**Interfaces:**
- Consumes: Task 5 の `removeMethod(atRow:)`、`removeClass(atRow:)`、`canRemoveMethod`、`canRemoveClass`。
- Produces: `MainMenu.Actions.removeMethod/removeClass/canRemoveMethod/canRemoveClass`、`func keyBrowserAllows(_:keyWindow:_:) -> Bool`、`BrowserWindow.menuNeedsUpdate(_:)`、`classTable.menu` / `selectorTable.menu`。

- [ ] **Step 1: Serena で外形を取る**

`find_symbol`: `MainMenu.Actions`、`MainMenu.actionItem`、`MenuAction`、`AoApp.applicationWillFinishLaunching`、`sendToKeyBrowser`。`find_referencing_symbols`: `MainMenu.Actions`（テストで `MainMenu.Actions()` を引数無しで作っている箇所が壊れないよう、新しいメンバにも既定値を付ける）。

- [ ] **Step 2: 失敗するテストを書く**

`AcceptTests.swift` の `testRemoveAsksToDiscardEditsFirst` のあとに:

```swift
  // SPEC §3.9 削除, §4.3: the Smalltalk menu's two items, without keys, enabled only while the
  // Browser is key and has a selector (Remove Method…) or a class (Remove Class…).
  func testRemoveMenuItemsFollowSelection() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
    var key: NSWindow? = browser.window
    var actions = MainMenu.Actions()
    actions.canRemoveMethod = { keyBrowserAllows(browser, keyWindow: key) { $0.canRemoveMethod } }
    actions.canRemoveClass = { keyBrowserAllows(browser, keyWindow: key) { $0.canRemoveClass } }
    var removed: [String] = []
    actions.removeMethod = { removed.append("method") }
    actions.removeClass = { removed.append("class") }
    let menu = MainMenu.build(actions: actions)
    let smalltalk = menu.item(withTitle: "Smalltalk")?.submenu
    XCTAssertEqual(
      smalltalk?.items.map(\.title),
      ["Do it", "Print it", "Inspect it", "Debug it", "Accept", "Remove Method…", "Remove Class…", "Show Hierarchy"]
    )
    guard let method = smalltalk?.item(withTitle: "Remove Method…"),
          let cls = smalltalk?.item(withTitle: "Remove Class…") else {
      XCTFail("missing Remove items")
      return
    }
    XCTAssertEqual(method.keyEquivalent, "")
    XCTAssertEqual(cls.keyEquivalent, "")
    func enabled(_ item: NSMenuItem) -> Bool {
      (item.target as? NSMenuItemValidation)?.validateMenuItem(item) ?? true
    }
    // Kernel / Object / native / printString: both.
    XCTAssertTrue(enabled(method))
    XCTAssertTrue(enabled(cls))
    selectProtocol(nil, in: browser)
    XCTAssertFalse(enabled(method))
    XCTAssertTrue(enabled(cls))
    key = workspace.window
    XCTAssertFalse(enabled(method))
    XCTAssertFalse(enabled(cls))
    key = nil
    XCTAssertFalse(enabled(cls))
    // Items without a test stay enabled.
    if let accept = smalltalk?.item(withTitle: "Accept") {
      XCTAssertTrue(enabled(accept))
    }
    XCTAssertTrue(method.target?.perform(method.action, with: method) != nil || removed == ["method"])
    XCTAssertTrue(cls.target?.perform(cls.action, with: cls) != nil || removed == ["method", "class"])
    XCTAssertEqual(removed, ["method", "class"])
    // The app's wiring: not the key window, nothing runs.
    sendToKeyBrowser(browser, keyWindow: workspace.window) { $0.removeMethod() }
    XCTAssertEqual(browser.model.selectedClass, "Object")
  }

  // SPEC §3.9 削除: the class and selector lists offer one item each, labelled as titled, enabled
  // for a clicked row; choosing it selects that row (asking to discard an edit first) and asks the
  // removal question. A question already up ignores a second choice.
  func testContextMenusOfferRemove() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    guard let classes = classTable(in: browser), let selectors = selectorTable(in: browser),
          let classMenu = classes.menu, let selectorMenu = selectors.menu else {
      XCTFail("missing context menus")
      return
    }
    XCTAssertEqual(classMenu.items.map(\.title), ["Remove Class…"])
    XCTAssertEqual(selectorMenu.items.map(\.title), ["Remove Method…"])
    XCTAssertEqual(classMenu.items.first?.accessibilityLabel(), "Remove Class…")
    XCTAssertEqual(selectorMenu.items.first?.accessibilityLabel(), "Remove Method…")
    XCTAssertTrue(classMenu.items.first?.target === browser)
    XCTAssertTrue(selectorMenu.items.first?.target === browser)
    XCTAssertFalse(classMenu.autoenablesItems)
    // No click yet (clickedRow -1): the items are disabled.
    browser.menuNeedsUpdate(classMenu)
    browser.menuNeedsUpdate(selectorMenu)
    XCTAssertFalse(classMenu.items.first?.isEnabled ?? true)
    XCTAssertFalse(selectorMenu.items.first?.isEnabled ?? true)

    selectProtocol("user", in: browser)
    browser.replaceSource("b12m\n  ^1\n")
    browser.accept()
    browser.replaceSource("b12n\n  ^2\n")
    browser.accept()
    XCTAssertEqual(browser.model.selectedSelector, "b12n")
    var asked: [String] = []
    browser.confirmRemove = { _, message, decide in
      // The clicked row is selected before the question.
      XCTAssertEqual(browser.model.selectedSelector, "b12m")
      asked.append(message)
      decide(true)
    }
    guard let row = browser.model.selectors.firstIndex(of: "b12m") else {
      XCTFail("missing b12m")
      return
    }
    browser.removeMethod(atRow: row)
    XCTAssertEqual(asked, ["Remove Object>>b12m?"])
    XCTAssertEqual(browser.model.selectors, ["b12n"])
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertNil(printIt("Object new b12m"))
    // Out of range: nothing.
    browser.removeMethod(atRow: -1)
    browser.removeClass(atRow: browser.model.classes.count)
    XCTAssertEqual(asked.count, 1)

    // An edit: the discard question comes first, and a second choice while it waits is ignored.
    selectSelector("b12n", in: browser)
    browser.replaceSource("b12n\n  ^9\n")
    var pending: (@MainActor (Bool) -> Void)?
    var discardAsked = 0
    browser.confirmDiscard = { _, decide in
      discardAsked += 1
      pending = decide
    }
    guard let objectRow = browser.model.classes.firstIndex(of: "Object") else {
      XCTFail("missing Object")
      return
    }
    browser.removeClass(atRow: objectRow)
    browser.removeClass(atRow: objectRow)
    XCTAssertEqual(discardAsked, 1)
    XCTAssertEqual(asked.count, 1)
    pending?(false)
    XCTAssertEqual(asked.count, 1)
    XCTAssertEqual(browser.sourceText, "b12n\n  ^9\n")
    XCTAssertEqual(browser.model.selectedSelector, "b12n")
    browser.confirmRemove = { _, message, decide in
      asked.append(message)
      decide(false)
    }
    browser.removeClass(atRow: objectRow)
    XCTAssertEqual(discardAsked, 2)
    pending?(true)
    XCTAssertEqual(asked, ["Remove Object>>b12m?", "Remove class Object?"])
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(printIt("Object new b12n"), "2")
  }
```

- [ ] **Step 3: 失敗を確認する**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'AcceptTests/testRemoveMenu|AcceptTests/testContextMenus' 2>&1 | grep -m5 'error:'`
Expected: `canRemoveMethod`（Actions）、`keyBrowserAllows`、`menuNeedsUpdate` が無いというコンパイルエラー。

- [ ] **Step 4: `MainMenu` に項目と有効・無効を足す**

`MainMenu.Actions` の `var accept: () -> Void = {}` の次に:

```swift
    // SPEC §3.9 削除: the Remove items, and whether each is enabled (the key Browser has a
    // selector / a class).
    var removeMethod: () -> Void = {}
    var removeClass: () -> Void = {}
    var canRemoveMethod: () -> Bool = { false }
    var canRemoveClass: () -> Bool = { false }
```

Smalltalk メニューの `actionItem("Accept", key: "", run: { _ in actions.accept() }),` の次に:

```swift
      actionItem("Remove Method…", key: "", run: { _ in actions.removeMethod() }, enabled: actions.canRemoveMethod),
      actionItem("Remove Class…", key: "", run: { _ in actions.removeClass() }, enabled: actions.canRemoveClass),
```

`actionItem` を:

```swift
  // enabled nil: always enabled (the items before SPEC §3.9 削除).
  private static func actionItem(
    _ title: String,
    key: String,
    run: @escaping (NSMenuItem) -> Void,
    enabled: (() -> Bool)? = nil
  ) -> NSMenuItem {
    let item = NSMenuItem(title: title, action: #selector(MenuAction.invoke(_:)), keyEquivalent: key)
    if !key.isEmpty {
      item.keyEquivalentModifierMask = .command
    }
    let target = MenuAction(run, enabled: enabled)
    item.target = target
    item.representedObject = target
    return item
  }
```

`MenuAction` を:

```swift
// Retained by NSMenuItem.representedObject. target itself is weak.
@MainActor
private final class MenuAction: NSObject, NSMenuItemValidation {
  private let run: (NSMenuItem) -> Void
  private let enabled: (() -> Bool)?

  init(_ run: @escaping (NSMenuItem) -> Void, enabled: (() -> Bool)? = nil) {
    self.run = run
    self.enabled = enabled
  }

  @objc func invoke(_ sender: NSMenuItem) {
    run(sender)
  }

  // SPEC §3.9 削除: AppKit asks before showing the menu; an item without a test stays enabled.
  func validateMenuItem(_ menuItem: NSMenuItem) -> Bool {
    enabled?() ?? true
  }
}
```

- [ ] **Step 5: `BrowserWindow` に右クリックメニューを足す**

`init` の `configure(selectorTable)` の次に:

```swift
    classTable.menu = contextMenu(title: "Remove Class…", action: #selector(removeClassFromMenu(_:)))
    selectorTable.menu = contextMenu(title: "Remove Method…", action: #selector(removeMethodFromMenu(_:)))
```

`func ownsWindow` の前に:

```swift
  // SPEC §3.9 削除: one item, for the clicked row; menuNeedsUpdate enables it for a row. The
  // VoiceOver label is the title.
  private func contextMenu(title: String, action: Selector) -> NSMenu {
    let menu = NSMenu(title: title)
    menu.autoenablesItems = false
    menu.delegate = self
    let item = NSMenuItem(title: title, action: action, keyEquivalent: "")
    item.target = self
    item.setAccessibilityLabel(title)
    menu.addItem(item)
    return menu
  }

  func menuNeedsUpdate(_ menu: NSMenu) {
    let table = menu === classTable.menu ? classTable : selectorTable
    for item in menu.items {
      item.isEnabled = table.clickedRow >= 0
    }
  }

  @objc private func removeClassFromMenu(_ sender: NSMenuItem) {
    removeClass(atRow: classTable.clickedRow)
  }

  @objc private func removeMethodFromMenu(_ sender: NSMenuItem) {
    removeMethod(atRow: selectorTable.clickedRow)
  }
```

ファイル末尾の `sendToKeyBrowser` のあとに:

```swift
// SPEC §3.9 削除: a Remove item's test: false unless the Browser is the key window.
@MainActor
func keyBrowserAllows(
  _ browser: BrowserWindow?,
  keyWindow: NSWindow?,
  _ test: (BrowserWindow) -> Bool
) -> Bool {
  guard let browser, browser.ownsWindow(keyWindow) else {
    return false
  }
  return test(browser)
}
```

- [ ] **Step 6: `AoApp` に配線する**

`applicationWillFinishLaunching` の `accept:` の配線の次に:

```swift
      removeMethod: {
        sendToKeyBrowser(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.removeMethod() }
      },
      removeClass: {
        sendToKeyBrowser(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.removeClass() }
      },
      canRemoveMethod: {
        keyBrowserAllows(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.canRemoveMethod }
      },
      canRemoveClass: {
        keyBrowserAllows(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.canRemoveClass }
      },
```

- [ ] **Step 7: テストを通す**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++ --filter 'AcceptTests/testRemove|AcceptTests/testContextMenus'`
Expected: 7 テストが PASS。`perform(_:with:)` の行が `@MainActor` で警告になるなら、`(method.target as? NSObject)?.perform(method.action, with: method)` に直し、`removed` の検査だけを残す。

- [ ] **Step 8: app のスイート全体を回す**

Run: `swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++`
Expected: PASS（`testMainMenuListsToolsAndSmalltalkKeys` は `contains` で見ているので項目が増えても緑）。

- [ ] **Step 9: Commit**

```bash
git add app/Ao/BrowserWindow.swift app/Ao/MainMenu.swift app/Ao/AoApp.swift app/AoTests/AcceptTests.swift
git commit -m "Offer Remove Method… and Remove Class… from the menus

SPEC §3.9 削除: the class and selector lists get a context menu for the clicked row, the
Smalltalk menu gets both items without keys, and NSMenuItemValidation enables them only
while the key Browser has a selector or a class.

Graphify: path MainMenu AoApp BrowserWindow
Serena: replace_symbol_body MainMenu.actionItem, MenuAction; insert_after_symbol sendToKeyBrowser

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

### Task 7: 受け入れ、記録、グラフ

**Files:**
- Modify: `CHANGELOG.md`（`[Unreleased]`）、`PHASE`、`SPEC.md` §6「Browser の削除」、`docs/README.md`
- Create: `docs/phases/P12.md`
- Modify: `graphify-out/`（`/graphify . --update`）

- [ ] **Step 1: 全部を回す**

```sh
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build --output-on-failure -R gcstress
./build/ao --test image/tests
swift test --package-path app -Xlinker -force_load -Xlinker build/runtime/libao_runtime.a \
  -Xlinker -force_load -Xlinker build/compiler/libao_compiler.a -Xlinker -lc++
ctest --test-dir build -R KernelBench --output-on-failure
```

Expected: すべて PASS。KernelBench の数字を `docs/bench.md` の最後の表と比べ、比が悪化していないことを確かめる（P12 はインタプリタのループに触れないので、変わらないはず。悪化していたら原因を探すまで次へ進まない）。

- [ ] **Step 2: 手動の受け入れ（Ao.app）**

`./scripts/package-app.sh` で `build/Ao.app` を作って開き、SPEC §6「Browser の削除」の各項目を順に試す: `Object>>foo` を Accept → 右クリック Remove Method… → Workspace `Object new foo` が `doesNotUnderstand: #foo`（直前に `Object new foo` を評価していても）。クラス `Foo` を定義 → Smalltalk → Remove Class… → 一覧から消え、`Foo` が `nil`。`Object>>printString` と `Object` の削除がエラー欄に理由を出す。サブクラスのあるクラスがサブクラス名を出す。Cancel で何も変わらない。Save Image → Open Image で消えたまま。

- [ ] **Step 3: SPEC §6 のチェックリストを `[x]` にし、`PHASE` を `P12` にする**

`SPEC.md` の「### Browser の削除」の 9 項目をすべて `- [x]` にする（最後の項目「この小節がすべて `[x]`、`PHASE` は `P12`、CHANGELOG の `[Unreleased]` に項目」も、下の Step 4〜5 のあとで）。`PHASE` の内容を `P12` にする。

- [ ] **Step 4: CHANGELOG**

`CHANGELOG.md` の `## [Unreleased]` の冒頭段落の次に段落を足す:

```markdown
Phase P12 of SPEC §2.3: the System Browser removes methods and classes (SPEC §3.9 削除). A CompiledMethod on any class, and a class that is not a Kernel class, can be removed after a confirmation. There is no undo.
```

`### Runtime` の末尾に:

```markdown
- C ABI: `ao_remove_method` takes a CompiledMethod out of a class's (or its metaclass's) method dictionary, invalidates the method cache for the selector and forgets its source; `ao_remove_class` takes a class's binding out of `Smalltalk` (an alias, the class object and its instances stay). A NativeMethod, an inherited selector, a fixed global, a Kernel class (also through an alias) and a class with a live subclass are refused with a reason. A removed global name is an undeclared identifier in the Workspace again. Both are refused while the runtime is busy. A halted process keeps running the method it was in.
```

`### Ao.app` の末尾に:

```markdown
- Right-click a class or a selector for Remove Class… / Remove Method…, also in the Smalltalk menu. A sheet asks first (`Remove Foo>>bar?`, `Remove class Foo?`; Remove / Cancel). A refusal shows its reason in the Browser's error field.
```

- [ ] **Step 5: docs**

`docs/phases/P12.md` を作る:

```markdown
# P12 — Browser の削除

## 結論

System Browser から CompiledMethod と Kernel 以外のクラスを消す。C ABI に `ao_remove_method` と `ao_remove_class` を足し、右クリックメニューと Smalltalk メニューから確認のシートを経て呼ぶ。正本は SPEC §3.9「削除」、§3.10、§6「Browser の削除」。設計の理由は [`superpowers/specs/2026-09-26-browser-remove-design.md`](../superpowers/specs/2026-09-26-browser-remove-design.md)、計画書は [`superpowers/plans/2026-09-26-p12-browser-remove.md`](../superpowers/plans/2026-09-26-p12-browser-remove.md)。

## 前提

P11 緑（`97bb489`）。SPEC の変更は `5cbd866` とその後の 3 点（ワークスペース変数、右クリックの順序、メッセージの名前）。

## 範囲

**やる:** `ao_remove_method`、`ao_remove_class`、辞書の削除（`MethodDictionary::removeKey`、`Globals::unbind`、`WellKnown::undefine`）、ソース表の項目の除去、Browser のメニューと確認、`remove_abi_test`、Browser の XCTest。

**やらない:** Undo、削除したソースの保存、サブクラスごとの削除、プロトコルやカテゴリの名前変更、Smalltalk 側の削除セレクタ、クラスの名前変更。

## 設計判断

- 辞書の対を nil にして `tally` を減らす。線形走査なので残りは引けたまま、次の登録が空きを使う。
- クラスの削除は束縛だけを外す。クラスオブジェクト、インスタンス、別名には触れない。グローバル名の版を進めるので、消した名前はワークスペースで未定義の識別子に戻る。
- 拒否の理由はすべて `AoSpan.message`。`<Class>` は渡した名前で、クラス側は ` class` を付ける。
- 右クリックのメニューは `clickedRow` を対象にし、項目を選んだ時点で「破棄の確認 → 行を選ぶ → 削除の確認 → ABI」を順に進める。

## TDD

`remove_abi_test`（SPEC §4.1）と `AcceptTests` の `testRemove*` / `testContextMenusOfferRemove`（SPEC §4.3）。
```

`docs/README.md` の表の `| P11 ライブデバッガ | [phases/P11.md](phases/P11.md) | done |` の次に `| P12 Browser の削除 | [phases/P12.md](phases/P12.md) | done |` を足し、依存の行を `… → P10 → P11 → P12。` にし、計画書の括弧に `P12: [2026-09-26-p12-browser-remove.md](superpowers/plans/2026-09-26-p12-browser-remove.md)` を足す。

- [ ] **Step 6: Graphify を更新する**

`/graphify . --update`（クラス追加は無いが ABI とメニューの経路が増えたので、`GRAPH_REPORT.md` に `ao_remove_method` / `ao_remove_class` が現れることを確かめる）。

- [ ] **Step 7: Commit**

```bash
git add CHANGELOG.md PHASE SPEC.md docs/README.md docs/phases/P12.md graphify-out
git commit -m "Close the P12 acceptance checklist and rebuild the knowledge graph

Graphify: --update
Serena: none (docs only)

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

- [ ] **Step 8: SPEC と設計書の変更をコミットしていなければ先にコミットする**

このセッションで直した `SPEC.md`（§3.6、§3.9「削除」、§3.10、§4.1、§6）と `docs/superpowers/specs/2026-09-26-browser-remove-design.md` は、Task 1 の前に:

```bash
git add SPEC.md docs/superpowers/specs/2026-09-26-browser-remove-design.md docs/superpowers/plans/2026-09-26-p12-browser-remove.md
git commit -m "Settle the P12 rules for workspace variables, the context menu and the messages

Graphify: query SPEC 削除
Serena: none (docs only)

Co-Authored-By: Claude Fable 5.1 <noreply@anthropic.com>"
```

---

## Self-Review

- **Spec coverage:** §3.6（`unbind`、版、空きの再利用: Task 1、3）、§3.9「削除」のメソッド（Task 2）、クラス（Task 3）、共通（Task 4）、Browser（Task 5、6）、§3.10 busy 一覧と `knownGlobals`（Task 3、4）、§4.1 の 16 テスト名（Task 2〜4。`RemovedClassNameBecomesWorkspaceVariable` を含む）、§4.3 の 7 テスト名（Task 5、6）、§6 のチェックリスト（Task 7）。
- **Placeholder scan:** 各ステップにコードがある。「適切に」「同様に」は無い。
- **Type consistency:** `removeMethodNamed(CallContext&, std::string_view, bool, std::string_view, std::string*)` と `removeClassNamed(CallContext&, std::string_view, std::string*)` は Task 2〜3 と ABI で同じ。`forgetMethodSource(Oop)` は Task 2 で定義し Task 3 で使う。Swift の `removeMethod(atRow:)` / `removeClass(atRow:)` は Task 5 で定義し Task 6 のメニューが呼ぶ。`keyBrowserAllows` は Task 6 で定義しテストと `AoApp` が使う。`confirmRemove` の型 `(NSWindow, String, (Bool) -> Void) -> Void` はテストの引数順と一致。
- **Review Focus:** 5 項目それぞれにテストがある（Task 4 の `RemovedMethodKeepsRunningInHaltedProcess` と `LongNameCutsTheMessageAt255Bytes`、Task 2〜3 の空文字、Task 5 の階層表示、Task 6 の二重の質問）。

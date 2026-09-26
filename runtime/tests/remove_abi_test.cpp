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

#include "test_support.hpp"

#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/Lookup.hpp"
#include "ao/kernel/Install.hpp"

#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

ao::Oop imageRegistryStubA(ao::CallContext&, ao::Oop receiver, const ao::Oop*, std::uint32_t) {
  return receiver;
}

ao::Oop imageRegistryStubB(ao::CallContext&, ao::Oop, const ao::Oop* args, std::uint32_t argc) {
  if (argc == 0 || args == nullptr) {
    return ao::Oop::nil();
  }
  return args[0];
}

}  // namespace

TEST(ImageRegistry, BootFindsIdentityEqualsAndSharedStubNames) {
  Boot b;
  std::uint32_t idx = 0;
  ASSERT_TRUE(ao::NativeRegistry::findName("ao_Object_identityEquals", &idx));
  auto meth = ao::lookup(b.heap, b.wk.objectClass, b.wk.selIdentityEquals);
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ("ao_Object_identityEquals", ao::NativeMethod::nameBytes(b.heap, meth));

  const char* groups[][3] = {
      {"ao_Bag_do_", "ao_LinkedList_do_", "ao_MappedCollection_do_"},
      {"ao_Bag_size", "ao_LinkedList_size", "ao_MappedCollection_size"},
      {"ao_Bag_add_", "ao_LinkedList_add_", "ao_MappedCollection_add_"},
  };
  std::uint32_t previous = idx;
  for (const auto& group : groups) {
    std::uint32_t a = 0;
    std::uint32_t bIndex = 0;
    std::uint32_t c = 0;
    ASSERT_TRUE(ao::NativeRegistry::findName(group[0], &a));
    ASSERT_TRUE(ao::NativeRegistry::findName(group[1], &bIndex));
    ASSERT_TRUE(ao::NativeRegistry::findName(group[2], &c));
    EXPECT_EQ(a, bIndex);
    EXPECT_EQ(a, c);
    EXPECT_NE(previous, a);
    previous = a;
  }

  ao::kernel::ensureNativeNames();
  std::uint32_t again = 0;
  ASSERT_TRUE(ao::NativeRegistry::findName("ao_Object_identityEquals", &again));
  EXPECT_EQ(idx, again);
}

TEST(ImageRegistry, AddNamedKeepsAliasAndRejectsForeignIndex) {
  std::uint32_t first = 0;
  ASSERT_TRUE(ao::NativeRegistry::addNamed("ao_ImageRegistry_probe", imageRegistryStubA, &first));
  std::uint32_t again = 99;
  ASSERT_TRUE(ao::NativeRegistry::addNamed("ao_ImageRegistry_probe", imageRegistryStubA, &again));
  EXPECT_EQ(first, again);

  std::uint32_t rejected = 77;
  EXPECT_FALSE(ao::NativeRegistry::addNamed("ao_ImageRegistry_probe", imageRegistryStubB, &rejected));
  EXPECT_EQ(77u, rejected);

  std::uint32_t found = 0;
  ASSERT_TRUE(ao::NativeRegistry::findName("ao_ImageRegistry_probe", &found));
  EXPECT_EQ(first, found);

  std::uint32_t alias = 0;
  ASSERT_TRUE(
      ao::NativeRegistry::addNamed("ao_ImageRegistry_probe_alias", imageRegistryStubA, &alias));
  EXPECT_EQ(first, alias);
  std::uint32_t aliasFound = 0;
  ASSERT_TRUE(ao::NativeRegistry::findName("ao_ImageRegistry_probe_alias", &aliasFound));
  EXPECT_EQ(first, aliasFound);
}

TEST(ImageRegistry, EachImageSlotLists127Names) {
  Boot b;
  std::vector<std::string> names;
  b.wk.eachImageSlot(
      [](void* baton, const char* name, ao::Oop) {
        static_cast<std::vector<std::string>*>(baton)->emplace_back(name);
      },
      &names);
  ASSERT_EQ(127u, names.size());
  EXPECT_EQ("Object", names.front());
  EXPECT_EQ("Object class", names[1]);
  EXPECT_EQ("==", names.back());
  auto count = [&](const char* needle) {
    int n = 0;
    for (const auto& name : names) {
      if (name == needle) {
        ++n;
      }
    }
    return n;
  };
  EXPECT_EQ(1, count("transcript"));
  EXPECT_EQ(1, count("Transcript"));
  EXPECT_EQ(1, count("Processor"));
  EXPECT_EQ(1, count("Smalltalk"));
}

TEST(ImageRegistry, AdoptOldBytesOnce) {
  std::byte filler[16]{};
  ao::Heap heap;
  ASSERT_TRUE(heap.adoptOldBytes(filler, 16, 7));
  EXPECT_EQ(16u, heap.oldUsed());
  EXPECT_EQ(7, heap.hashCursor());
  EXPECT_FALSE(heap.adoptOldBytes(filler, 16, 9));
  EXPECT_EQ(16u, heap.oldUsed());
  EXPECT_EQ(7, heap.hashCursor());

  ao::Heap empty;
  EXPECT_FALSE(empty.adoptOldBytes(filler, empty.oldCapacity() + 1, 3));
  EXPECT_EQ(0u, empty.oldUsed());
  EXPECT_EQ(1, empty.hashCursor());
}

TEST(ImageRegistry, ExtraListsCmUser) {
  Boot b;
  const char* def =
      "!Object subclass: #CmUser\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'P5-Test'!\n"
      "!CmUser methodsFor: 't'!\n"
      "ok\n"
      "  ^1 + 2!\n";
  std::vector<ao::compiler::CompileError> errs;
  auto acts = ao::compiler::parseChunks(def, errs);
  ASSERT_TRUE(ao::applyChunks(b.ctx, acts, errs)) << (errs.empty() ? "" : errs[0].message);
  EXPECT_FALSE(b.wk.isCatalogName("CmUser"));

  bool found = false;
  b.wk.eachExtra(
      [](void* baton, std::string_view name, ao::Oop) {
        if (name == "CmUser") {
          *static_cast<bool*>(baton) = true;
        }
      },
      &found);
  EXPECT_TRUE(found);
}

TEST(ImageRegistry, BindImageSlotWritesFieldsInOrder) {
  Boot b;
  const ao::Oop mark = ao::Oop::fromSmallInteger(42);
  ASSERT_TRUE(b.wk.bindImageSlot("Class", mark));
  EXPECT_EQ(mark, b.wk.classClass);
  ASSERT_TRUE(b.wk.bindImageSlot("class", mark));
  EXPECT_EQ(mark, b.wk.selClass);
  ASSERT_TRUE(b.wk.bindImageSlot("Object class", mark));
  EXPECT_EQ(mark, b.wk.objectMetaclass);
  ASSERT_TRUE(b.wk.bindImageSlot("Smalltalk", mark));
  EXPECT_EQ(mark, b.wk.smalltalk);
  ASSERT_TRUE(b.wk.bindImageSlot("Processor", mark));
  EXPECT_EQ(mark, b.wk.processor);
  ASSERT_TRUE(b.wk.bindImageSlot("transcript", mark));
  EXPECT_EQ(mark, b.wk.transcript);
  ASSERT_TRUE(b.wk.bindImageSlot("Transcript", mark));
  EXPECT_EQ(mark, b.wk.transcriptClass);
  ASSERT_TRUE(b.wk.bindImageSlot("==", mark));
  EXPECT_EQ(mark, b.wk.selIdentityEquals);
  EXPECT_FALSE(b.wk.bindImageSlot("CmUser", mark));
  EXPECT_FALSE(b.wk.bindImageSlot("nope", mark));
}

TEST(ImageRegistry, RememberSymbolRegistersWithoutAllocating) {
  Boot b;
  const char* text = "image-registry-sym";
  const auto n = static_cast<std::uint32_t>(std::strlen(text));
  auto sym = b.wk.intern(text);
  ASSERT_TRUE(sym.isHeap());
  EXPECT_TRUE(b.wk.rememberSymbol(sym));

  auto other = b.heap.allocate(b.wk.symbolClass, n, ao::kFlagBytes);
  ASSERT_TRUE(other.isHeap());
  std::memcpy(b.heap.bytes(other), text, n);
  EXPECT_FALSE(b.wk.rememberSymbol(other));
  EXPECT_EQ(sym, b.wk.intern(text));

  const char* neu = "img-reg-new";
  const auto neuN = static_cast<std::uint32_t>(std::strlen(neu));
  auto created = b.heap.allocate(b.wk.symbolClass, neuN, ao::kFlagBytes);
  ASSERT_TRUE(created.isHeap());
  std::memcpy(b.heap.bytes(created), neu, neuN);
  EXPECT_TRUE(b.wk.rememberSymbol(created));
  EXPECT_EQ(created, b.wk.intern(neu));
  EXPECT_TRUE(b.wk.rememberSymbol(created));

  auto slots = b.heap.allocate(b.wk.symbolClass, 1, 0);
  ASSERT_TRUE(slots.isHeap());
  EXPECT_FALSE(b.wk.rememberSymbol(slots));

  auto bytes = b.heap.allocate(b.wk.stringClass, n, ao::kFlagBytes);
  ASSERT_TRUE(bytes.isHeap());
  std::memcpy(b.heap.bytes(bytes), text, n);
  EXPECT_FALSE(b.wk.rememberSymbol(bytes));
  EXPECT_FALSE(b.wk.rememberSymbol(ao::Oop::nil()));
  EXPECT_FALSE(b.wk.rememberSymbol(ao::Oop::fromSmallInteger(1)));
}

#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

namespace ao {

class WellKnown {
 public:
  WellKnown(Heap& heap, Roots& roots);
  ~WellKnown();
  WellKnown(const WellKnown&) = delete;
  WellKnown& operator=(const WellKnown&) = delete;
  WellKnown(WellKnown&&) = delete;
  WellKnown& operator=(WellKnown&&) = delete;

  Heap& heap() const { return *heap_; }

  Oop nil() const { return Oop::nil(); }
  Oop true_() const { return Oop::true_(); }
  Oop false_() const { return Oop::false_(); }

  // What the global name means: nil, true and false, else the value Smalltalk binds to it, or nil
  // when it binds none (SPEC §3.6). Before Globals::install only the well-known slots count.
  Oop named(std::string_view name) const;
  // The value Smalltalk binds to symbol (an interned Symbol), or nil. PushGlobal reads it.
  Oop global(Oop symbol) const;
  Oop classOf(Oop obj) const;
  // A new Symbol goes to the nursery, or to old when the nursery is full (allocateNoGc).
  // Does not GC; empty Oop when old is at its max.
  Oop intern(std::string_view utf8);
  // A new Symbol goes straight to old (allocateTenured). Empty Oop when old is at its max.
  Oop internTenured(std::string_view utf8);
  // The Symbol intern made for these bytes, or the empty Oop when there is none. Allocates nothing.
  Oop findSymbol(std::string_view utf8) const;
  // Binds name to value in Smalltalk (SPEC §3.6). A fixed global is left as it is: false. False
  // also when the Symbol or the dictionary's growth cannot be allocated (old at its max; the
  // out-of-memory flag is set). Does not GC.
  bool define(std::string_view name, Oop value);
  bool isCatalogName(std::string_view name) const;
  // The names Bootstrap binds and nothing rebinds but file-in's vendor stubs: the catalog,
  // Smalltalk and Processor (SPEC §3.6).
  bool isFixedGlobal(std::string_view name) const;
  // The pseudo-variables nil true false self super thisContext (SPEC §3.8). The compiler resolves
  // them before globals, so Smalltalk binds none of them: define refuses them as it refuses a fixed
  // global (SPEC §3.6).
  static bool isPseudoVariableName(std::string_view name);
  bool rebind(std::string_view name, Oop cls);
  // Grows whenever define or rebind changes the global names, so caches of them can tell.
  std::uint64_t globalsVersion() const { return globalsVersion_; }
  // The classes Smalltalk binds, in the order they were bound (an alias again); before
  // Globals::install, the catalog's.
  void eachClass(void (*fn)(void* baton, Oop cls), void* baton) const;
  void eachNativeRequiredClass(void (*fn)(void* baton, Oop cls), void* baton) const;
  void eachImageSlot(void (*fn)(void*, const char* name, Oop value), void* baton) const;
  bool bindImageSlot(std::string_view name, Oop value);
  bool rememberSymbol(Oop sym);

  // SendSpecial's selector table (compiler::specialSelector), interned ahead of use so the
  // interpreter reads selector k without interning (SPEC §3.5).
  static constexpr std::uint8_t kSpecialSelectorCount = 28;
  // Interns every special selector. Bootstrap calls it; Image::load calls it after it has
  // remembered the image's Symbols. Does not GC; false when an intern fails.
  bool internSpecialSelectors();
  // Selector k, read from its rooted intern-table slot, so a GC that moves the Symbol is seen.
  // Empty Oop when k is out of range or internSpecialSelectors has not filled it.
  Oop specialSelector(std::uint8_t k) const {
    return k < kSpecialSelectorCount && specialSlots_[k] != nullptr ? *specialSlots_[k] : Oop{};
  }
  // SPEC §3.5: SendSpecial answers + - * < > <= >= = for two SmallIntegers without a send only
  // while this holds: each of them, looked up from SmallInteger, finds a NativeMethod.
  bool smallIntegerFastPath() const { return smallIntegerFastPath_; }
  // Looks the eight selectors up from SmallInteger and sets smallIntegerFastPath. Bootstrap::run,
  // Image::load and ensureKernelNatives call it. An old image may hide one with a user method.
  void checkSmallIntegerFastPath();

  Oop objectClass{};
  Oop objectMetaclass{};
  Oop behaviorClass{};
  Oop behaviorMetaclass{};
  Oop classDescriptionClass{};
  Oop classDescriptionMetaclass{};
  Oop classClass{};
  Oop classMetaclass{};
  Oop metaclassClass{};
  Oop metaclassMetaclass{};
  Oop undefinedObjectClass{};
  Oop undefinedObjectMetaclass{};
  Oop booleanClass{};
  Oop booleanMetaclass{};
  Oop trueClass{};
  Oop trueMetaclass{};
  Oop falseClass{};
  Oop falseMetaclass{};
  Oop magnitudeClass{};
  Oop magnitudeMetaclass{};
  Oop numberClass{};
  Oop numberMetaclass{};
  Oop integerClass{};
  Oop integerMetaclass{};
  Oop smallIntegerClass{};
  Oop smallIntegerMetaclass{};
  Oop largePositiveIntegerClass{};
  Oop largePositiveIntegerMetaclass{};
  Oop largeNegativeIntegerClass{};
  Oop largeNegativeIntegerMetaclass{};
  Oop floatClass{};
  Oop floatMetaclass{};
  Oop fractionClass{};
  Oop fractionMetaclass{};
  Oop characterClass{};
  Oop characterMetaclass{};
  Oop collectionClass{};
  Oop collectionMetaclass{};
  Oop sequenceableCollectionClass{};
  Oop sequenceableCollectionMetaclass{};
  Oop arrayedCollectionClass{};
  Oop arrayedCollectionMetaclass{};
  Oop arrayClass{};
  Oop arrayMetaclass{};
  Oop byteArrayClass{};
  Oop byteArrayMetaclass{};
  Oop stringClass{};
  Oop stringMetaclass{};
  Oop symbolClass{};
  Oop symbolMetaclass{};
  Oop intervalClass{};
  Oop intervalMetaclass{};
  Oop dictionaryClass{};
  Oop dictionaryMetaclass{};
  Oop identityDictionaryClass{};
  Oop identityDictionaryMetaclass{};
  Oop setClass{};
  Oop setMetaclass{};
  Oop identitySetClass{};
  Oop identitySetMetaclass{};
  Oop orderedCollectionClass{};
  Oop orderedCollectionMetaclass{};
  Oop associationClass{};
  Oop associationMetaclass{};
  Oop bagClass{};
  Oop bagMetaclass{};
  Oop linkedListClass{};
  Oop linkedListMetaclass{};
  Oop mappedCollectionClass{};
  Oop mappedCollectionMetaclass{};
  Oop compiledMethodClass{};
  Oop compiledMethodMetaclass{};
  Oop methodDictionaryClass{};
  Oop methodDictionaryMetaclass{};
  Oop nativeMethodClass{};
  Oop nativeMethodMetaclass{};
  Oop messageClass{};
  Oop messageMetaclass{};
  Oop methodContextClass{};
  Oop methodContextMetaclass{};
  Oop blockContextClass{};
  Oop blockContextMetaclass{};
  Oop processClass{};
  Oop processMetaclass{};
  Oop processorSchedulerClass{};
  Oop processorSchedulerMetaclass{};
  Oop semaphoreClass{};
  Oop semaphoreMetaclass{};
  Oop sharedQueueClass{};
  Oop sharedQueueMetaclass{};
  Oop pointClass{};
  Oop pointMetaclass{};
  Oop rectangleClass{};
  Oop rectangleMetaclass{};
  Oop streamClass{};
  Oop streamMetaclass{};
  Oop positionableStreamClass{};
  Oop positionableStreamMetaclass{};
  Oop readStreamClass{};
  Oop readStreamMetaclass{};
  Oop writeStreamClass{};
  Oop writeStreamMetaclass{};
  Oop readWriteStreamClass{};
  Oop readWriteStreamMetaclass{};
  Oop transcriptClass{};
  Oop transcriptMetaclass{};
  Oop smalltalkImageClass{};
  Oop smalltalkImageMetaclass{};
  Oop dateClass{};
  Oop dateMetaclass{};
  Oop timeClass{};
  Oop timeMetaclass{};

  Oop smalltalk{};
  Oop processor{};
  Oop transcript{};

  Oop selValue{};
  Oop selValue_{};
  Oop selNew{};
  Oop selBasicNew{};
  Oop selBasicNew_{};
  Oop selSize{};
  Oop selAt_{};
  Oop selAt_put_{};
  Oop selDo_{};
  Oop selError_{};
  Oop selClass{};
  Oop selIdentityEquals{};

 private:
  void addRoots(Roots& roots);
  Oop internWith(std::string_view utf8, bool tenured);
  struct InternTable;
  Heap* heap_;
  Roots* roots_ = nullptr;
  std::unique_ptr<InternTable> intern_;
  std::uint64_t globalsVersion_ = 0;
  // Slots of intern_->table. A deque keeps its elements in place as it grows.
  std::array<const Oop*, kSpecialSelectorCount> specialSlots_{};
  bool smallIntegerFastPath_ = false;
};

}  // namespace ao

#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

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

  Oop named(std::string_view name) const;
  Oop classOf(Oop obj) const;
  Oop intern(std::string_view utf8);
  void define(std::string_view name, Oop cls);
  bool isCatalogName(std::string_view name) const;
  bool rebind(std::string_view name, Oop cls);
  void eachClass(void (*fn)(void* baton, Oop cls), void* baton) const;
  void eachNativeRequiredClass(void (*fn)(void* baton, Oop cls), void* baton) const;

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
  struct InternTable;
  struct ExtraTable;
  Heap* heap_;
  Roots* roots_ = nullptr;
  std::unique_ptr<InternTable> intern_;
  std::unique_ptr<ExtraTable> extra_;
};

}  // namespace ao

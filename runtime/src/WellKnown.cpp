#include "ao/WellKnown.hpp"

#include <cstring>
#include <deque>
#include <string>
#include <unordered_map>

namespace ao {

struct WellKnown::InternTable {
  std::deque<Oop> table;
  std::unordered_map<std::string, std::size_t> byBytes;
};

struct WellKnown::ExtraTable {
  struct Entry {
    std::string name;
    Oop cls;
  };
  std::deque<Entry> table;
};

namespace {

struct NamedClass {
  const char* name;
  Oop WellKnown::* cls;
  Oop WellKnown::* meta;
};

constexpr NamedClass kNamedClasses[] = {
    {"Object", &WellKnown::objectClass, &WellKnown::objectMetaclass},
    {"Behavior", &WellKnown::behaviorClass, &WellKnown::behaviorMetaclass},
    {"ClassDescription", &WellKnown::classDescriptionClass, &WellKnown::classDescriptionMetaclass},
    {"Class", &WellKnown::classClass, &WellKnown::classMetaclass},
    {"Metaclass", &WellKnown::metaclassClass, &WellKnown::metaclassMetaclass},
    {"UndefinedObject", &WellKnown::undefinedObjectClass, &WellKnown::undefinedObjectMetaclass},
    {"Boolean", &WellKnown::booleanClass, &WellKnown::booleanMetaclass},
    {"True", &WellKnown::trueClass, &WellKnown::trueMetaclass},
    {"False", &WellKnown::falseClass, &WellKnown::falseMetaclass},
    {"Magnitude", &WellKnown::magnitudeClass, &WellKnown::magnitudeMetaclass},
    {"Number", &WellKnown::numberClass, &WellKnown::numberMetaclass},
    {"Integer", &WellKnown::integerClass, &WellKnown::integerMetaclass},
    {"SmallInteger", &WellKnown::smallIntegerClass, &WellKnown::smallIntegerMetaclass},
    {"LargePositiveInteger", &WellKnown::largePositiveIntegerClass,
     &WellKnown::largePositiveIntegerMetaclass},
    {"LargeNegativeInteger", &WellKnown::largeNegativeIntegerClass,
     &WellKnown::largeNegativeIntegerMetaclass},
    {"Float", &WellKnown::floatClass, &WellKnown::floatMetaclass},
    {"Fraction", &WellKnown::fractionClass, &WellKnown::fractionMetaclass},
    {"Character", &WellKnown::characterClass, &WellKnown::characterMetaclass},
    {"Collection", &WellKnown::collectionClass, &WellKnown::collectionMetaclass},
    {"SequenceableCollection", &WellKnown::sequenceableCollectionClass,
     &WellKnown::sequenceableCollectionMetaclass},
    {"ArrayedCollection", &WellKnown::arrayedCollectionClass, &WellKnown::arrayedCollectionMetaclass},
    {"Array", &WellKnown::arrayClass, &WellKnown::arrayMetaclass},
    {"ByteArray", &WellKnown::byteArrayClass, &WellKnown::byteArrayMetaclass},
    {"String", &WellKnown::stringClass, &WellKnown::stringMetaclass},
    {"Symbol", &WellKnown::symbolClass, &WellKnown::symbolMetaclass},
    {"Interval", &WellKnown::intervalClass, &WellKnown::intervalMetaclass},
    {"Dictionary", &WellKnown::dictionaryClass, &WellKnown::dictionaryMetaclass},
    {"IdentityDictionary", &WellKnown::identityDictionaryClass,
     &WellKnown::identityDictionaryMetaclass},
    {"Set", &WellKnown::setClass, &WellKnown::setMetaclass},
    {"IdentitySet", &WellKnown::identitySetClass, &WellKnown::identitySetMetaclass},
    {"OrderedCollection", &WellKnown::orderedCollectionClass, &WellKnown::orderedCollectionMetaclass},
    {"Association", &WellKnown::associationClass, &WellKnown::associationMetaclass},
    {"Bag", &WellKnown::bagClass, &WellKnown::bagMetaclass},
    {"LinkedList", &WellKnown::linkedListClass, &WellKnown::linkedListMetaclass},
    {"MappedCollection", &WellKnown::mappedCollectionClass, &WellKnown::mappedCollectionMetaclass},
    {"CompiledMethod", &WellKnown::compiledMethodClass, &WellKnown::compiledMethodMetaclass},
    {"NativeMethod", &WellKnown::nativeMethodClass, &WellKnown::nativeMethodMetaclass},
    {"Message", &WellKnown::messageClass, &WellKnown::messageMetaclass},
    {"MethodDictionary", &WellKnown::methodDictionaryClass, &WellKnown::methodDictionaryMetaclass},
    {"MethodContext", &WellKnown::methodContextClass, &WellKnown::methodContextMetaclass},
    {"BlockContext", &WellKnown::blockContextClass, &WellKnown::blockContextMetaclass},
    {"Process", &WellKnown::processClass, &WellKnown::processMetaclass},
    {"ProcessorScheduler", &WellKnown::processorSchedulerClass,
     &WellKnown::processorSchedulerMetaclass},
    {"Semaphore", &WellKnown::semaphoreClass, &WellKnown::semaphoreMetaclass},
    {"SharedQueue", &WellKnown::sharedQueueClass, &WellKnown::sharedQueueMetaclass},
    {"Point", &WellKnown::pointClass, &WellKnown::pointMetaclass},
    {"Rectangle", &WellKnown::rectangleClass, &WellKnown::rectangleMetaclass},
    {"Stream", &WellKnown::streamClass, &WellKnown::streamMetaclass},
    {"PositionableStream", &WellKnown::positionableStreamClass,
     &WellKnown::positionableStreamMetaclass},
    {"ReadStream", &WellKnown::readStreamClass, &WellKnown::readStreamMetaclass},
    {"WriteStream", &WellKnown::writeStreamClass, &WellKnown::writeStreamMetaclass},
    {"ReadWriteStream", &WellKnown::readWriteStreamClass, &WellKnown::readWriteStreamMetaclass},
    {"Transcript", &WellKnown::transcriptClass, &WellKnown::transcriptMetaclass},
    {"SmalltalkImage", &WellKnown::smalltalkImageClass, &WellKnown::smalltalkImageMetaclass},
    {"Date", &WellKnown::dateClass, &WellKnown::dateMetaclass},
    {"Time", &WellKnown::timeClass, &WellKnown::timeMetaclass},
};

}  // namespace

WellKnown::WellKnown(Heap& heap, Roots& roots)
    : heap_(&heap),
      roots_(&roots),
      intern_(std::make_unique<InternTable>()),
      extra_(std::make_unique<ExtraTable>()) {
  for (const auto& e : kNamedClasses) {
    this->*e.cls = Oop::nil();
    this->*e.meta = Oop::nil();
  }
  smalltalk = Oop::nil();
  processor = Oop::nil();
  selValue = Oop::nil();
  selValue_ = Oop::nil();
  selNew = Oop::nil();
  selBasicNew = Oop::nil();
  selBasicNew_ = Oop::nil();
  selSize = Oop::nil();
  selAt_ = Oop::nil();
  selAt_put_ = Oop::nil();
  selDo_ = Oop::nil();
  selError_ = Oop::nil();
  selClass = Oop::nil();
  selIdentityEquals = Oop::nil();
  addRoots(roots);
}

WellKnown::~WellKnown() = default;

void WellKnown::addRoots(Roots& roots) {
  for (const auto& e : kNamedClasses) {
    roots.add(&(this->*e.cls));
    roots.add(&(this->*e.meta));
  }
  roots.add(&smalltalk);
  roots.add(&processor);
  roots.add(&selValue);
  roots.add(&selValue_);
  roots.add(&selNew);
  roots.add(&selBasicNew);
  roots.add(&selBasicNew_);
  roots.add(&selSize);
  roots.add(&selAt_);
  roots.add(&selAt_put_);
  roots.add(&selDo_);
  roots.add(&selError_);
  roots.add(&selClass);
  roots.add(&selIdentityEquals);
}

Oop WellKnown::named(std::string_view name) const {
  if (name == "nil") return nil();
  if (name == "true") return true_();
  if (name == "false") return false_();
  if (name == "Smalltalk") return smalltalk;
  if (name == "Processor") return processor;
  for (const auto& e : kNamedClasses) {
    if (name == e.name) return this->*e.cls;
  }
  if (extra_ != nullptr) {
    for (const auto& e : extra_->table) {
      if (name == e.name) return e.cls;
    }
  }
  return Oop::nil();
}

void WellKnown::define(std::string_view name, Oop cls) {
  if (extra_ == nullptr) {
    extra_ = std::make_unique<ExtraTable>();
  }
  for (auto& e : extra_->table) {
    if (e.name == name) {
      e.cls = cls;
      return;
    }
  }
  extra_->table.push_back(ExtraTable::Entry{std::string(name), cls});
  roots_->add(&extra_->table.back().cls);
}

void WellKnown::eachClass(void (*fn)(void* baton, Oop cls), void* baton) const {
  if (fn == nullptr) {
    return;
  }
  for (const auto& e : kNamedClasses) {
    fn(baton, this->*e.cls);
  }
  if (extra_ == nullptr) {
    return;
  }
  for (const auto& e : extra_->table) {
    fn(baton, e.cls);
  }
}

Oop WellKnown::classOf(Oop obj) const {
  if (obj.isNil()) return undefinedObjectClass;
  if (obj.isTrue()) return trueClass;
  if (obj.isFalse()) return falseClass;
  if (obj.isSmallInteger()) return smallIntegerClass;
  if (obj.isCharacter()) return characterClass;
  if (obj.isHeap()) return heap_->klass(obj);
  return Oop::nil();
}

Oop WellKnown::intern(std::string_view utf8) {
  std::string key(utf8);
  auto it = intern_->byBytes.find(key);
  if (it != intern_->byBytes.end()) {
    return intern_->table[it->second];
  }
  const auto n = static_cast<std::uint32_t>(utf8.size());
  Oop sym = heap_->allocate(symbolClass, n, kFlagBytes);
  if (!sym.isHeap()) {
    return Oop{};
  }
  if (n != 0) {
    std::memcpy(heap_->bytes(sym), utf8.data(), n);
  }
  intern_->table.push_back(sym);
  roots_->add(&intern_->table.back());
  intern_->byBytes.emplace(std::move(key), intern_->table.size() - 1);
  return intern_->table.back();
}

}  // namespace ao

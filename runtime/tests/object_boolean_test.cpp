#include "test_support.hpp"

#include "ao/Globals.hpp"

#include <gtest/gtest.h>

TEST(KernelCatalog, NamedClassesAreHeap) {
  Boot b;
  const char* names[] = {
      "Object", "Behavior", "ClassDescription", "Class", "Metaclass",
      "UndefinedObject", "Boolean", "True", "False",
      "Magnitude", "Number", "Integer", "SmallInteger",
      "LargePositiveInteger", "LargeNegativeInteger", "Float", "Fraction", "Character",
      "Collection", "SequenceableCollection", "ArrayedCollection",
      "Array", "ByteArray", "String", "Symbol", "Interval",
      "Dictionary", "IdentityDictionary", "Set", "IdentitySet",
      "OrderedCollection", "Association", "Bag", "LinkedList", "MappedCollection",
      "CompiledMethod", "NativeMethod", "Message", "MethodDictionary",
      "MethodContext", "BlockContext",
      "Process", "ProcessorScheduler", "Semaphore", "SharedQueue",
      "Point", "Rectangle",
      "Stream", "PositionableStream", "ReadStream", "WriteStream", "ReadWriteStream",
      "Transcript", "SmalltalkImage", "Date", "Time"};
  for (const char* n : names) {
    auto cls = b.wk.named(n);
    ASSERT_TRUE(cls.isHeap()) << n;
    EXPECT_EQ(cls, ao::Globals::at(b.wk, n)) << n;
    EXPECT_EQ(ao::kClassSlotCount, b.heap.size(cls)) << n;
  }
}

TEST(KernelCatalog, SmallIntegerSuperclassIsInteger) {
  Boot b;
  EXPECT_EQ(b.wk.integerClass, b.heap.slotAt(b.wk.smallIntegerClass, ao::kClassSlotSuperclass));
  EXPECT_EQ(b.wk.magnitudeClass, b.heap.slotAt(b.wk.characterClass, ao::kClassSlotSuperclass));
}

TEST(BlockContext, ValueAppliesNativeThunk) {
  Boot b;
  auto fn = [](ao::CallContext&, ao::Oop, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(4);
  };
  auto blk = ao::makeNativeBlock(b.ctx, fn, 0);
  ASSERT_TRUE(blk.isHeap());
  EXPECT_EQ(b.wk.blockContextClass, b.heap.klass(blk));
  auto r = send0(b, blk, "value");
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(4, r.smallIntegerValue());
}

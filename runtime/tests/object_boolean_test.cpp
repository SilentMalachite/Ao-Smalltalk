#include "test_support.hpp"

#include "ao/Format.hpp"
#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"

#include <gtest/gtest.h>

static ao::Oop formatOf(Boot& b, ao::Oop cls) {
  return b.heap.slotAt(cls, ao::kClassSlotFormat);
}

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

TEST(KernelCatalog, FormatBitsForIndexableClasses) {
  Boot b;
  auto arrayFmt = formatOf(b, b.wk.arrayClass);
  EXPECT_TRUE(ao::Format::isIndexable(arrayFmt));
  EXPECT_FALSE(ao::Format::isBytes(arrayFmt));
  EXPECT_TRUE(ao::Format::isPointers(arrayFmt));

  const ao::Oop byteClasses[] = {
      b.wk.byteArrayClass,           b.wk.stringClass, b.wk.symbolClass,
      b.wk.largePositiveIntegerClass, b.wk.largeNegativeIntegerClass, b.wk.floatClass};
  for (ao::Oop cls : byteClasses) {
    auto fmt = formatOf(b, cls);
    EXPECT_TRUE(ao::Format::isIndexable(fmt));
    EXPECT_TRUE(ao::Format::isBytes(fmt));
    EXPECT_FALSE(ao::Format::isPointers(fmt));
  }
}

TEST(KernelCatalog, SymbolDateTimeSuperclasses) {
  Boot b;
  EXPECT_EQ(b.wk.stringClass, b.heap.slotAt(b.wk.symbolClass, ao::kClassSlotSuperclass));
  EXPECT_EQ(b.wk.magnitudeClass, b.heap.slotAt(b.wk.dateClass, ao::kClassSlotSuperclass));
  EXPECT_EQ(b.wk.magnitudeClass, b.heap.slotAt(b.wk.timeClass, ao::kClassSlotSuperclass));
}

TEST(KernelCatalog, ProcessorIsProcessorSchedulerInstance) {
  Boot b;
  ASSERT_TRUE(b.wk.processor.isHeap());
  EXPECT_EQ(b.wk.processorSchedulerClass, b.heap.klass(b.wk.processor));
}

TEST(KernelCatalog, CallContextHooksDefaultNull) {
  Boot b;
  EXPECT_EQ(nullptr, b.ctx.inspectHook);
  EXPECT_EQ(nullptr, b.ctx.transcriptHook);
}

TEST(BlockContext, ValueAppliesNativeThunk) {
  Boot b;
  auto fn = [](ao::CallContext&, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Oop::fromSmallInteger(4);
  };
  auto blk = ao::makeNativeBlock(b.ctx, fn, 0);
  ASSERT_TRUE(blk.isHeap());
  EXPECT_EQ(b.wk.blockContextClass, b.heap.klass(blk));
  auto r = send0(b, blk, "value");
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(4, r.smallIntegerValue());
}

TEST(BlockContext, ValueColonAppliesNativeThunk) {
  Boot b;
  auto fn = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t argc) {
    if (argc != 1 || !args[0].isSmallInteger()) {
      return ao::Oop{};
    }
    return ao::Oop::fromSmallInteger(args[0].smallIntegerValue() + 1);
  };
  auto blk = ao::makeNativeBlock(b.ctx, fn, 1);
  auto r = send1(b, blk, "value:", ao::Oop::fromSmallInteger(3));
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(4, r.smallIntegerValue());
}

TEST(BlockContext, ValueValueAppliesNativeThunk) {
  Boot b;
  auto fn = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t argc) {
    if (argc != 2 || !args[0].isSmallInteger() || !args[1].isSmallInteger()) {
      return ao::Oop{};
    }
    return ao::Oop::fromSmallInteger(args[0].smallIntegerValue() + args[1].smallIntegerValue());
  };
  auto blk = ao::makeNativeBlock(b.ctx, fn, 2);
  ao::Oop args[2] = {ao::Oop::fromSmallInteger(1), ao::Oop::fromSmallInteger(3)};
  auto sel = ao::Symbol::intern(b.wk, "value:value:");
  auto r = ao::send(b.ctx, blk, sel, args, 2, nullptr);
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(4, r.smallIntegerValue());
}

TEST(BlockContext, ValueWithArgumentsAppliesNativeThunk) {
  Boot b;
  auto fn = [](ao::CallContext&, const ao::Oop&, const ao::Oop* args, std::uint32_t argc) {
    if (argc != 2 || !args[0].isSmallInteger() || !args[1].isSmallInteger()) {
      return ao::Oop{};
    }
    return ao::Oop::fromSmallInteger(args[0].smallIntegerValue() * args[1].smallIntegerValue());
  };
  auto blk = ao::makeNativeBlock(b.ctx, fn, 2);
  auto arr = b.heap.allocate(b.wk.arrayClass, 2, 0);
  ASSERT_TRUE(arr.isHeap());
  b.heap.slotAtPut(arr, 0, ao::Oop::fromSmallInteger(2));
  b.heap.slotAtPut(arr, 1, ao::Oop::fromSmallInteger(3));
  auto r = send1(b, blk, "valueWithArguments:", arr);
  ASSERT_TRUE(r.isSmallInteger());
  EXPECT_EQ(6, r.smallIntegerValue());
}

TEST(ObjectBoolean, NilIsNilAndObjectNewIsNot) {
  Boot b;
  EXPECT_TRUE(send0(b, ao::Oop::nil(), "isNil").isTrue());
  EXPECT_TRUE(send0(b, ao::Oop::fromSmallInteger(1), "isNil").isFalse());
}

TEST(ObjectBoolean, TrueIfTrueIfFalseReturnsOkBlock) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  auto okFn = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Str::fromUtf8(ctx.heap, ctx.wk, "ok");
  };
  auto ngFn = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Str::fromUtf8(ctx.heap, ctx.wk, "ng");
  };
  ao::Root ok(b.roots, ao::makeNativeBlock(b.ctx, okFn, 0));
  ao::Root ng(b.roots, ao::makeNativeBlock(b.ctx, ngFn, 0));
  ao::Oop args[2] = {ok.slot, ng.slot};
  auto sel = ao::Symbol::intern(b.wk, "ifTrue:ifFalse:");
  auto r = ao::send(b.ctx, ao::Oop::true_(), sel, args, 2, nullptr);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(r));
  EXPECT_EQ("ok", ao::Str::toUtf8(b.heap, r));
}

TEST(ObjectBoolean, IdentityEqualsAndYourself) {
  Boot b;
  auto one = ao::Oop::fromSmallInteger(1);
  EXPECT_TRUE(send1(b, one, "==", one).isTrue());
  EXPECT_EQ(one, send0(b, one, "yourself"));
}

TEST(ObjectBoolean, FalseIfTrueIfFalseReturnsNgBlock) {
  Boot b;
  // send と makeNativeBlock は GC しうるので、それをまたぐ値はルートしておく。
  auto okFn = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Str::fromUtf8(ctx.heap, ctx.wk, "ok");
  };
  auto ngFn = [](ao::CallContext& ctx, const ao::Oop&, const ao::Oop*, std::uint32_t) {
    return ao::Str::fromUtf8(ctx.heap, ctx.wk, "ng");
  };
  ao::Root ok(b.roots, ao::makeNativeBlock(b.ctx, okFn, 0));
  ao::Root ng(b.roots, ao::makeNativeBlock(b.ctx, ngFn, 0));
  ao::Oop args[2] = {ok.slot, ng.slot};
  auto sel = ao::Symbol::intern(b.wk, "ifTrue:ifFalse:");
  auto r = ao::send(b.ctx, ao::Oop::false_(), sel, args, 2, nullptr);
  ASSERT_TRUE(r.isHeap());
  EXPECT_EQ(b.wk.stringClass, b.heap.klass(r));
  EXPECT_EQ("ng", ao::Str::toUtf8(b.heap, r));
}

TEST(ObjectBoolean, IdentityHashOfImmediates) {
  Boot b;
  auto h0 = send0(b, ao::Oop::nil(), "identityHash");
  auto h1 = send0(b, ao::Oop::false_(), "identityHash");
  auto h2 = send0(b, ao::Oop::true_(), "identityHash");
  auto smi = ao::Oop::fromSmallInteger(7);
  auto ch = ao::Oop::fromCharacter(U'A');
  ASSERT_TRUE(h0.isSmallInteger());
  ASSERT_TRUE(h1.isSmallInteger());
  ASSERT_TRUE(h2.isSmallInteger());
  EXPECT_EQ(0, h0.smallIntegerValue());
  EXPECT_EQ(1, h1.smallIntegerValue());
  EXPECT_EQ(2, h2.smallIntegerValue());
  EXPECT_EQ(smi, send0(b, smi, "identityHash"));
  auto chHash = send0(b, ch, "identityHash");
  ASSERT_TRUE(chHash.isSmallInteger());
  EXPECT_EQ(static_cast<std::int64_t>(U'A'), chHash.smallIntegerValue());
}

TEST(ObjectBoolean, InspectCallsHookAndReturnsSelf) {
  Boot b;
  static ao::Oop seen;
  seen = ao::Oop{};
  b.ctx.inspectHook = [](ao::CallContext&, ao::Oop value) { seen = value; };
  auto one = ao::Oop::fromSmallInteger(1);
  EXPECT_EQ(one, send0(b, one, "inspect"));
  EXPECT_EQ(one, seen);
}

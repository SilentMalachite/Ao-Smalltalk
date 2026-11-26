#include "test_support.hpp"

#include "ao/Compile.hpp"
#include "ao/Compiler.hpp"
#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Interpreter.hpp"
#include "ao/NativeMethod.hpp"

#include <gtest/gtest.h>

#include <string>

static ao::Oop makeSubclass(Boot& b, const char* name) {
  auto n = b.wk.intern(name);
  auto empty = b.wk.intern("");
  ao::Oop args[5] = {n, empty, empty, empty, empty};
  auto sel = ao::Symbol::intern(
      b.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  return ao::send(b.ctx, b.wk.objectClass, sel, args, 5, nullptr);
}

TEST(Behavior, ObjectNewClassIsObject) {
  Boot b;
  auto o = send0(b, b.wk.objectClass, "new");
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.objectClass, send0(b, o, "class"));
  EXPECT_TRUE(send0(b, o, "isNil").isFalse());
}

TEST(Behavior, MetaclassNewIsForbidden) {
  Boot b;
  // SPEC §3.3: 値を返さず、shouldNotImplement で評価を中断する。
  auto r = send0(b, b.wk.metaclassClass, "new");
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("shouldNotImplement", takeAbortReason(b));
}

TEST(Behavior, SubclassIsRegistered) {
  Boot b;
  auto name = b.wk.intern("Foo");
  auto empty = b.wk.intern("");
  ao::Oop args[5] = {name, empty, empty, empty, empty};
  auto sel = ao::Symbol::intern(b.wk, "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  auto foo = ao::send(b.ctx, b.wk.objectClass, sel, args, 5, nullptr);
  ASSERT_TRUE(foo.isHeap());
  EXPECT_EQ(foo, b.wk.named("Foo"));
  EXPECT_EQ(b.wk.objectClass, b.heap.slotAt(foo, ao::kClassSlotSuperclass));
}

TEST(Behavior, MetaclassNewAbortsWithShouldNotImplement) {
  Boot b;
  auto r = send0(b, b.heap.klass(b.wk.objectClass), "new");
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("shouldNotImplement", takeAbortReason(b));
}

TEST(Behavior, ArrayNewIsEmptyArray) {
  Boot b;
  auto o = send0(b, b.wk.arrayClass, "new");
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(o));
  EXPECT_EQ(0u, b.heap.size(o));
}

TEST(Behavior, BasicNewColonAllocatesIndexableSlots) {
  Boot b;
  auto o = send1(b, b.wk.arrayClass, "basicNew:", ao::Oop::fromSmallInteger(3));
  ASSERT_TRUE(o.isHeap());
  EXPECT_EQ(b.wk.arrayClass, b.heap.klass(o));
  EXPECT_EQ(3u, b.heap.size(o));
  auto bytes = send1(b, b.wk.byteArrayClass, "basicNew:", ao::Oop::fromSmallInteger(4));
  ASSERT_TRUE(bytes.isHeap());
  EXPECT_EQ(4u, b.heap.size(bytes));
  EXPECT_NE(0, b.heap.flags(bytes) & ao::kFlagBytes);
}

// docs/claude-review/04 Low: バイト列クラスの basicNew: は大きさを下位 32 ビットに切り詰め、
// (String new: 4294967299) size が 3 になった。SPEC §3.6: 2^32 − 1 を超える大きさは、ポインタの
// クラスと同じく割り当てずに失敗する。
TEST(Behavior, BasicNewColonRefusesSizesPastUint32) {
  Boot b;
  for (ao::Oop cls : {b.wk.stringClass, b.wk.byteArrayClass, b.wk.arrayClass}) {
    for (std::int64_t n : {std::int64_t{4294967296}, std::int64_t{4294967299},
                           std::int64_t{1} << 40}) {
      SCOPED_TRACE(n);
      EXPECT_TRUE(send1(b, cls, "basicNew:", ao::Oop::fromSmallInteger(n)).isEmpty());
      EXPECT_TRUE(send1(b, cls, "new:", ao::Oop::fromSmallInteger(n)).isEmpty());
      EXPECT_FALSE(b.heap.outOfMemory());
    }
  }
  auto img = ao::compiler::compileMethod("doIt\n  ^(String new: 4294967299) size");
  ASSERT_TRUE(img.ok) << img.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  const ao::Oop r =
      ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("failed: #new:", takeAbortReason(b));
  ao::Root ok(b.roots, send1(b, b.wk.stringClass, "new:", ao::Oop::fromSmallInteger(3)));
  ASSERT_TRUE(ok.slot.isHeap());
  EXPECT_EQ(3u, b.heap.size(ok.slot));
}

// SPEC §3.6 basicNew:: 2^32 − 1 は上限以内だが old に入らないので out of memory。負の数と
// LargeInteger は SmallInteger の 0 以上でないので failed: #basicNew:（new: から送れば failed: #new:）。
TEST(Behavior, BasicNewColonAtTheBoundAndOddSizes) {
  Boot b;
  auto run = [&](const std::string& expr) {
    // Each run is its own evaluation: ao_eval clears the out-of-memory flag the same way.
    b.heap.clearOutOfMemory();
    auto img = ao::compiler::compileMethod("doIt\n  ^" + expr);
    EXPECT_TRUE(img.ok) << img.error.message;
    ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
    const ao::Oop r =
        ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
    return r.isEmpty() ? takeAbortReason(b) : std::string("<answered>");
  };
  for (const char* cls : {"Array", "ByteArray", "String"}) {
    SCOPED_TRACE(cls);
    EXPECT_EQ("out of memory", run(std::string(cls) + " basicNew: 4294967295"));
    EXPECT_EQ("out of memory", run(std::string(cls) + " new: 4294967295"));
    EXPECT_EQ("failed: #basicNew:", run(std::string(cls) + " basicNew: -1"));
    EXPECT_EQ("failed: #new:", run(std::string(cls) + " new: -1"));
    EXPECT_EQ("failed: #basicNew:", run(std::string(cls) + " basicNew: (1 bitShift: 70)"));
    EXPECT_EQ("failed: #basicNew:", run(std::string(cls) + " basicNew: 3.0"));
    // The heap is usable afterwards.
    EXPECT_EQ("<answered>", run(std::string(cls) + " new: 4"));
  }
}

TEST(Behavior, InstSizeAndFormatBitsArrayVsObject) {
  Boot b;
  auto objSize = send0(b, b.wk.objectClass, "instSize");
  ASSERT_TRUE(objSize.isSmallInteger());
  EXPECT_EQ(0, objSize.smallIntegerValue());
  auto clsSize = send0(b, b.wk.classClass, "instSize");
  ASSERT_TRUE(clsSize.isSmallInteger());
  EXPECT_EQ(static_cast<std::int64_t>(ao::kClassSlotCount), clsSize.smallIntegerValue());

  EXPECT_TRUE(send0(b, b.wk.objectClass, "isVariable").isFalse());
  EXPECT_TRUE(send0(b, b.wk.objectClass, "isBytes").isFalse());
  EXPECT_TRUE(send0(b, b.wk.objectClass, "isPointers").isTrue());
  EXPECT_TRUE(send0(b, b.wk.arrayClass, "isVariable").isTrue());
  EXPECT_TRUE(send0(b, b.wk.arrayClass, "isBytes").isFalse());
  EXPECT_TRUE(send0(b, b.wk.arrayClass, "isPointers").isTrue());
  EXPECT_TRUE(send0(b, b.wk.byteArrayClass, "isVariable").isTrue());
  EXPECT_TRUE(send0(b, b.wk.byteArrayClass, "isBytes").isTrue());
  EXPECT_TRUE(send0(b, b.wk.byteArrayClass, "isPointers").isFalse());
}

TEST(Behavior, InheritsFromWalksSuperclassChain) {
  Boot b;
  EXPECT_TRUE(send1(b, b.wk.arrayClass, "inheritsFrom:", b.wk.collectionClass).isTrue());
  EXPECT_TRUE(send1(b, b.wk.arrayClass, "inheritsFrom:", b.wk.objectClass).isTrue());
  EXPECT_TRUE(send1(b, b.wk.arrayClass, "inheritsFrom:", b.wk.arrayClass).isFalse());
  EXPECT_TRUE(send1(b, b.wk.objectClass, "inheritsFrom:", b.wk.arrayClass).isFalse());
}

TEST(Behavior, ThisClassNameIncludesSelectorCompiledMethodAt) {
  Boot b;
  EXPECT_EQ(b.wk.objectClass, send0(b, b.wk.objectMetaclass, "thisClass"));
  auto name = send0(b, b.wk.objectClass, "name");
  ASSERT_TRUE(name.isHeap());
  EXPECT_EQ("Object", ao::Str::toUtf8(b.heap, name));

  auto classSel = ao::Symbol::intern(b.wk, "class");
  EXPECT_TRUE(send1(b, b.wk.objectClass, "includesSelector:", classSel).isTrue());
  EXPECT_TRUE(send1(b, b.wk.objectClass, "includesSelector:", ao::Symbol::intern(b.wk, "new")).isFalse());

  auto meth = send1(b, b.wk.objectClass, "compiledMethodAt:", classSel);
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(b.wk.nativeMethodClass, b.heap.klass(meth));
  EXPECT_EQ("ao_Object_class", ao::NativeMethod::nameBytes(b.heap, meth));
}

TEST(Behavior, EachClassAndGlobalsSeeExtraNamed) {
  Boot b;
  auto foo = makeSubclass(b, "Foo");
  ASSERT_TRUE(foo.isHeap());
  EXPECT_EQ(foo, b.wk.named("Foo"));
  EXPECT_EQ(foo, ao::Globals::at(b.wk, "Foo"));

  struct Seen {
    ao::Oop want;
    bool found = false;
  } seen{foo, false};
  b.wk.eachClass(
      [](void* p, ao::Oop cls) {
        auto* s = static_cast<Seen*>(p);
        if (cls == s->want) {
          s->found = true;
        }
      },
      &seen);
  EXPECT_TRUE(seen.found);
}

// SPEC §3.6: instSize, inherited slots included, is the 16-bit field of the format. The 3 slots
// of OrderedCollection and 65534 more make 65537, which would wrap: no class. The first name is
// the inherited `array`, a duplicate, so the expected reason proves the size check comes before
// the duplicate scan (and a scan that ran first would stop at that first name).
TEST(Behavior, SubclassBeyondInstSizeLimitAborts) {
  Boot b;
  ASSERT_EQ(ao::Oop::fromSmallInteger(3), send0(b, b.wk.orderedCollectionClass, "instSize"));
  std::string names = "array";
  for (int i = 0; i < 65533; ++i) {
    names += " v" + std::to_string(i);
  }
  ao::RootedArray args(b.roots, 5);
  args[1] = ao::Str::fromUtf8(b.ctx, names);
  ASSERT_TRUE(args[1].isHeap());
  args[0] = b.wk.intern("WideOrderedCollection");
  args[2] = b.wk.intern("");
  args[3] = b.wk.intern("");
  args[4] = b.wk.intern("");
  const ao::Oop sel = b.wk.intern(
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:");
  const ao::Oop r = ao::send(b.ctx, b.wk.orderedCollectionClass, sel, args.ptr(), 5, nullptr);
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("too many instance variables", takeAbortReason(b));
  EXPECT_TRUE(b.wk.named("WideOrderedCollection").isNil());
}

// SPEC §3.6: instanceVariableNames is a byte object (a String, a Symbol; a ByteArray passes too).
// The slots of an Array are not names: the send fails and no class is made.
TEST(Behavior, SubclassWithNonStringInstanceVariableNamesFails) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "doIt\n  | names |\n  names := Array new: 1.\n  names at: 1 put: Object.\n"
      "  ^Object subclass: #NonStringNames instanceVariableNames: names classVariableNames: ''\n"
      "    poolDictionaries: '' category: ''");
  ASSERT_TRUE(img.ok) << img.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  const ao::Oop r =
      ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("failed: #subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:",
            takeAbortReason(b));
  EXPECT_TRUE(b.wk.named("NonStringNames").isNil());

  // The helpers that read text answer empty for a pointer object, as for a non-heap value.
  ao::Root array(b.roots, send1(b, b.wk.arrayClass, "new:", ao::Oop::fromSmallInteger(1)));
  ASSERT_TRUE(array.slot.isHeap());
  b.heap.slotAtPut(array.slot, 0, b.wk.objectClass);
  EXPECT_EQ("", ao::Str::toUtf8(b.heap, array.slot));
  EXPECT_EQ("", ao::Symbol::bytes(b.heap, array.slot));
}

// SPEC §3.6 (H8): instVarAt:put: can make a metaclass's format bytes, and basicNew: on it then
// makes a byte object whose class is a metaclass. Behavior's natives read the format only of a
// class-shaped receiver, so basicNew, basicNew: and new on that object fail instead of reading
// its bytes as a format. eachClass does not visit it as a class.
TEST(Behavior, BasicNewOnByteObjectWithMetaclassClassFails) {
  Boot b;
  ASSERT_TRUE(makeSubclass(b, "AoP8").isHeap());
  auto run = [&](const std::string& body) {
    auto img = ao::compiler::compileMethod("doIt\n" + body);
    EXPECT_TRUE(img.ok) << img.error.message;
    ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
    const ao::Oop r =
        ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
    return r.isEmpty() ? takeAbortReason(b) : std::string("<answered>");
  };
  EXPECT_EQ("failed: #basicNew", run("  AoP8 class instVarAt: 3 put: 196608.\n"
                                     "  ^(AoP8 class basicNew: 16) basicNew"));
  ASSERT_EQ("<answered>", run("  ^Smalltalk at: #AoP8Bytes put: (AoP8 class basicNew: 16)"));
  EXPECT_EQ("failed: #basicNew", run("  ^AoP8Bytes basicNew"));
  EXPECT_EQ("failed: #basicNew:", run("  ^AoP8Bytes basicNew: 3"));
  EXPECT_EQ("failed: #new", run("  ^AoP8Bytes new"));

  // Nothing below collects, so the Oops read here stay put.
  const ao::Oop cls = b.wk.named("AoP8");
  const ao::Oop bytes = ao::Globals::at(b.wk, "AoP8Bytes");
  ASSERT_TRUE(bytes.isHeap());
  ASSERT_NE(0, b.heap.flags(bytes) & ao::kFlagBytes);
  ASSERT_EQ(b.heap.klass(cls), b.heap.klass(bytes));
  struct Seen {
    ao::Oop cls;
    ao::Oop bytes;
    bool sawClass = false;
    bool sawBytes = false;
  } seen{cls, bytes};
  b.wk.eachClass(
      [](void* p, ao::Oop each) {
        auto* s = static_cast<Seen*>(p);
        s->sawClass = s->sawClass || each == s->cls;
        s->sawBytes = s->sawBytes || each == s->bytes;
      },
      &seen);
  EXPECT_TRUE(seen.sawClass);
  EXPECT_FALSE(seen.sawBytes);
}

// SPEC §3.6: the byte object of the test above is not class-shaped. The Behavior and Class natives
// that read a class's slots fail on it instead of reading its bytes as slots (memory safety only),
// and instSize, isVariable, isBytes and isPointers fail too instead of answering a format of 0.
TEST(Behavior, NativesOnNonClassShapedReceiverFail) {
  Boot b;
  ASSERT_TRUE(makeSubclass(b, "AoP8").isHeap());
  auto run = [&](const std::string& body) {
    auto img = ao::compiler::compileMethod("doIt\n" + body);
    EXPECT_TRUE(img.ok) << img.error.message;
    ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
    const ao::Oop r =
        ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
    return r.isEmpty() ? takeAbortReason(b) : std::string("<answered>");
  };
  ASSERT_EQ("<answered>", run("  AoP8 class instVarAt: 3 put: 196608.\n"
                              "  ^Smalltalk at: #AoP8Bytes put: (AoP8 class basicNew: 16)"));
  struct Row {
    const char* send;
    const char* reason;
  };
  const Row rows[] = {
      {"instSize", "failed: #instSize"},
      {"isVariable", "failed: #isVariable"},
      {"isBytes", "failed: #isBytes"},
      {"isPointers", "failed: #isPointers"},
      {"sourceCodeAt: #foo", "failed: #sourceCodeAt:"},
      {"superclass", "failed: #superclass"},
      {"methodDict", "failed: #methodDict"},
      {"selectors", "failed: #selectors"},
      {"includesSelector: #foo", "failed: #includesSelector:"},
      {"compiledMethodAt: #foo", "failed: #compiledMethodAt:"},
      {"name", "failed: #name"},
      {"category", "failed: #category"},
      {"classPool", "failed: #classPool"},
  };
  for (const Row& row : rows) {
    EXPECT_EQ(row.reason, run(std::string("  ^AoP8Bytes ") + row.send)) << row.send;
  }
  // review-agy R5 F3: a pointer object with fewer slots than a class is not class-shaped either,
  // so sourceCodeAt: fails on it outside a session too instead of answering nil. The format is
  // restored in the same doIt.
  const ao::Oop meta = b.heap.klass(b.wk.named("AoP8"));
  const ao::Oop fmt = b.heap.slotAt(meta, ao::kClassSlotFormat);
  ASSERT_EQ("<answered>", run("  | fmt |\n"
                              "  fmt := AoP8 class instVarAt: 3.\n"
                              "  AoP8 class instVarAt: 3 put: 2.\n"
                              "  Smalltalk at: #AoP8Small put: AoP8 class basicNew.\n"
                              "  AoP8 class instVarAt: 3 put: fmt.\n"
                              "  ^fmt"));
  EXPECT_EQ(fmt, b.heap.slotAt(b.heap.klass(b.wk.named("AoP8")), ao::kClassSlotFormat));
  const ao::Oop small = b.wk.named("AoP8Small");
  ASSERT_TRUE(small.isHeap());
  EXPECT_EQ(2u, b.heap.size(small));
  EXPECT_EQ(0, b.heap.flags(small) & ao::kFlagBytes);
  EXPECT_EQ("failed: #sourceCodeAt:", run("  ^AoP8Small sourceCodeAt: #foo"));
}

// review-agy R5 F6 / SPEC §3.6: Metaclass forbids new but not basicNew. With Metaclass's format
// rewritten to no slots, basicNew makes a Metaclass instance that is not class-shaped; thisClass
// fails on it instead of reading past its slots. The format is restored in the same doIt.
TEST(Behavior, MetaclassThisClassOnNonClassShapedReceiverFails) {
  Boot b;
  auto run = [&](const std::string& body) {
    auto img = ao::compiler::compileMethod("doIt\n" + body);
    EXPECT_TRUE(img.ok) << img.error.message;
    ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
    const ao::Oop r =
        ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
    return r.isEmpty() ? takeAbortReason(b) : std::string("<answered>");
  };
  const ao::Oop fmt = b.heap.slotAt(b.wk.metaclassClass, ao::kClassSlotFormat);
  ASSERT_EQ("<answered>", run("  | fmt |\n"
                              "  fmt := Metaclass instVarAt: 3.\n"
                              "  Metaclass instVarAt: 3 put: 0.\n"
                              "  Smalltalk at: #AoBareMeta put: Metaclass basicNew.\n"
                              "  Metaclass instVarAt: 3 put: fmt.\n"
                              "  ^fmt"));
  EXPECT_EQ(fmt, b.heap.slotAt(b.wk.metaclassClass, ao::kClassSlotFormat));
  const ao::Oop bare = b.wk.named("AoBareMeta");
  ASSERT_TRUE(bare.isHeap());
  EXPECT_EQ(b.wk.metaclassClass, b.heap.klass(bare));
  EXPECT_EQ(0u, b.heap.size(bare));
  EXPECT_EQ("failed: #thisClass", run("  ^AoBareMeta thisClass"));
  // A metaclass still answers its class.
  EXPECT_EQ(b.wk.objectClass, send0(b, b.heap.klass(b.wk.objectClass), "thisClass"));
}

// SPEC §3.6: the class name, like instanceVariableNames, is a byte object. An empty pointer object
// is no name: the send fails and no class is bound to ''.
TEST(Behavior, SubclassWithNonByteClassNameFails) {
  Boot b;
  auto img = ao::compiler::compileMethod(
      "doIt\n  ^Object subclass: Object new instanceVariableNames: '' classVariableNames: ''\n"
      "    poolDictionaries: '' category: ''");
  ASSERT_TRUE(img.ok) << img.error.message;
  ao::Root cm(b.roots, ao::boxMethodImage(b.ctx, img.image, b.wk.objectClass));
  const ao::Oop r =
      ao::Interpreter::run(b.ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ("failed: #subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:",
            takeAbortReason(b));
  EXPECT_TRUE(b.wk.named("").isNil());
}

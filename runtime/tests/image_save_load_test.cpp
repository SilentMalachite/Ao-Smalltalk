#include "test_support.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Gc.hpp"
#include "ao/Image.hpp"
#include "ao/ImageFormat.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Symbol.hpp"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace {

struct Loaded {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk;
  ao::ClassMethodCache cache;
  ao::CallContext ctx;

  Loaded() : wk(heap, roots), ctx{heap, roots, wk, &cache} { cache.addRoots(roots); }

  explicit Loaded(std::size_t oldBytes)
      : heap(1u << 20, oldBytes), wk(heap, roots), ctx{heap, roots, wk, &cache} {
    cache.addRoots(roots);
  }
};

ao::Oop send0(Loaded& image, ao::Oop rcvr, const char* sel) {
  auto s = ao::Symbol::intern(image.wk, sel);
  return ao::send(image.ctx, rcvr, s, nullptr, 0, nullptr);
}

ao::Oop send1(Loaded& image, ao::Oop rcvr, const char* sel, ao::Oop arg) {
  auto s = ao::Symbol::intern(image.wk, sel);
  return ao::send(image.ctx, rcvr, s, &arg, 1, nullptr);
}

bool fileInSource(Boot& b, const char* src, std::vector<ao::compiler::CompileError>* errs) {
  auto acts = ao::compiler::parseChunks(src, *errs);
  return ao::applyChunks(b.ctx, acts, *errs);
}

const char kUserMethods[] =
    "!Object subclass: #CmUser\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: ''\n"
    "  poolDictionaries: ''\n"
    "  category: 'P7-Test'!\n"
    "!CmUser methodsFor: 't'!\n"
    "ok\n"
    "  ^1 + 2!\n"
    "!CmUser methodsFor: 't'!\n"
    "tag\n"
    "  ^'ao'!\n";

bool nativeRequiredDictsAreNative(Loaded& image) {
  std::vector<std::string> bad;
  struct Scan {
    Loaded* image;
    std::vector<std::string>* bad;
    void visit(ao::Oop cls) {
      if (!cls.isHeap()) return;
      auto dict = image->heap.slotAt(cls, ao::kClassSlotMethodDict);
      if (!dict.isHeap()) return;
      auto inner = image->heap.slotAt(dict, ao::kDictSlotArray);
      if (!inner.isHeap()) return;
      for (std::uint32_t i = 0; i + 1 < image->heap.size(inner); i += 2) {
        auto k = image->heap.slotAt(inner, i);
        auto v = image->heap.slotAt(inner, i + 1);
        if (k.isNil()) continue;
        if (!v.isHeap() || image->heap.klass(v) != image->wk.nativeMethodClass) {
          bad->push_back("non-native");
        }
      }
    }
  } scan{&image, &bad};
  image.wk.eachNativeRequiredClass(
      [](void* p, ao::Oop cls) {
        auto* s = static_cast<Scan*>(p);
        s->visit(cls);
        s->visit(s->image->heap.klass(cls));
      },
      &scan);
  return bad.empty();
}

ao::Oop methodDictKey(Loaded& image, ao::Oop cls, std::string_view bytes) {
  if (!cls.isHeap()) return ao::Oop{};
  auto dict = image.heap.slotAt(cls, ao::kClassSlotMethodDict);
  if (!dict.isHeap()) return ao::Oop{};
  auto inner = image.heap.slotAt(dict, ao::kDictSlotArray);
  if (!inner.isHeap()) return ao::Oop{};
  for (std::uint32_t i = 0; i + 1 < image.heap.size(inner); i += 2) {
    auto k = image.heap.slotAt(inner, i);
    if (k.isNil()) continue;
    if (ao::Symbol::bytes(image.heap, k) == bytes) return k;
  }
  return ao::Oop{};
}

std::string utf8Bytes(ao::Heap& heap, ao::Oop obj) {
  if (!obj.isHeap() || (heap.flags(obj) & ao::kFlagBytes) == 0) return {};
  return std::string(reinterpret_cast<const char*>(heap.bytes(obj)), heap.size(obj));
}

ao::Oop soleWeak(ao::Heap& heap) {
  const std::byte* p = heap.oldBase();
  const std::byte* end = p + heap.oldUsed();
  ao::Oop found{};
  int count = 0;
  while (p < end) {
    const auto* h = reinterpret_cast<const ao::ObjectHeader*>(p);
    const std::size_t n = heap.objectBytes(h);
    if (n == 0 || n > static_cast<std::size_t>(end - p)) break;
    if ((h->flags & ao::kFlagWeak) != 0 && (h->flags & ao::kFlagBytes) == 0) {
      found = ao::Oop::fromHeap(const_cast<ao::ObjectHeader*>(h));
      ++count;
    }
    p += n;
  }
  if (count != 1) return ao::Oop{};
  return found;
}

bool readHeapBytes(const std::filesystem::path& path, std::uint32_t* out) {
  std::ifstream in(path, std::ios::binary);
  std::vector<char> bytes((std::istreambuf_iterator<char>(in)), {});
  ao::ImageFormat::ImageHeader header;
  if (!ao::ImageFormat::readHeader(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size(),
                                   &header)) {
    return false;
  }
  *out = header.heapBytes;
  return true;
}

void expectOnePlusTwo(Loaded& image) {
  auto three = send1(image, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(three.isSmallInteger());
  EXPECT_EQ(3, three.smallIntegerValue());
}

}  // namespace

TEST(ImageSave, WritesAoimAndKeepsSourceRunnable) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "save.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  std::ifstream in(path, std::ios::binary);
  std::vector<char> bytes((std::istreambuf_iterator<char>(in)), {});
  ASSERT_GE(bytes.size(), 48u);
  EXPECT_EQ('A', bytes[0]);
  EXPECT_EQ('O', bytes[1]);
  EXPECT_EQ('I', bytes[2]);
  EXPECT_EQ('M', bytes[3]);
  const std::string blob(bytes.begin(), bytes.end());
  EXPECT_NE(std::string::npos, blob.find("ao_Object_identityEquals"));

  const auto host = reinterpret_cast<std::uintptr_t>(b.wk.objectClass.heapPointer());
  std::uint64_t needle = static_cast<std::uint64_t>(host);
  bool foundHost = false;
  for (std::size_t i = 0; i + 8 <= bytes.size(); i += 1) {
    std::uint64_t word = 0;
    std::memcpy(&word, bytes.data() + i, 8);
    if (word == needle) foundHost = true;
  }
  EXPECT_FALSE(foundHost);

  auto three = send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  EXPECT_TRUE(three.isSmallInteger());
  EXPECT_EQ(3, three.smallIntegerValue());
}

TEST(ImageSave, WeakUnrootedReferentStaysInSourceSlot) {
  ao::Oop weak;
  Boot b;
  weak = b.heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  ao::Oop referent = b.heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(weak.isHeap());
  ASSERT_TRUE(referent.isHeap());
  b.heap.slotAtPut(weak, 0, referent);
  b.roots.add(&weak);

  const auto path = std::filesystem::path(testing::TempDir()) / "weak.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  EXPECT_EQ(referent, b.heap.slotAt(weak, 0));
}

TEST(ImageSaveLoad, OnePlusTwoMatches) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-arith.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  expectOnePlusTwo(loaded);

  auto nilAnswer = send0(loaded, ao::Oop::nil(), "isNil");
  EXPECT_TRUE(nilAnswer.isTrue());

  auto created = send0(loaded, loaded.wk.objectClass, "new");
  ASSERT_TRUE(created.isHeap());
  EXPECT_EQ(loaded.wk.objectClass, send0(loaded, created, "class"));

  auto meta = send0(loaded, loaded.wk.metaclassClass, "class");
  EXPECT_EQ(loaded.wk.metaclassClass, send0(loaded, meta, "class"));
}

TEST(ImageSaveLoad, UserMethodSurvives) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(fileInSource(b, kUserMethods, &errs)) << (errs.empty() ? "" : errs[0].message);
  auto cls = b.wk.named("CmUser");
  ASSERT_TRUE(cls.isHeap());
  const auto hashBefore = b.heap.hash(cls);

  const auto path = std::filesystem::path(testing::TempDir()) / "load-user.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  auto loadedCls = loaded.wk.named("CmUser");
  ASSERT_TRUE(loadedCls.isHeap());
  EXPECT_EQ(hashBefore, loaded.heap.hash(loadedCls));

  auto sel = loaded.wk.intern("ok");
  auto meth = ao::lookup(loaded.heap, loadedCls, sel);
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(loaded.wk.compiledMethodClass, loaded.heap.klass(meth));
  ASSERT_GE(loaded.heap.size(meth), ao::kCmSlotCount);
  EXPECT_TRUE(loaded.heap.slotAt(meth, ao::kCmSlotNativeCode).isNil());

  auto inst = send0(loaded, loadedCls, "new");
  ASSERT_TRUE(inst.isHeap());
  auto three = send0(loaded, inst, "ok");
  ASSERT_TRUE(three.isSmallInteger());
  EXPECT_EQ(3, three.smallIntegerValue());
  EXPECT_EQ("ao", utf8Bytes(loaded.heap, send0(loaded, inst, "tag")));
}

TEST(ImageSaveLoad, KernelMethodsStayNative) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-kernel.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  EXPECT_TRUE(nativeRequiredDictsAreNative(loaded));
}

TEST(ImageSaveLoad, SymbolIdentity) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(fileInSource(b, kUserMethods, &errs)) << (errs.empty() ? "" : errs[0].message);
  const auto path = std::filesystem::path(testing::TempDir()) / "load-symbol.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  auto first = loaded.wk.intern("ok");
  auto second = loaded.wk.intern("ok");
  EXPECT_EQ(first, second);
  auto key = methodDictKey(loaded, loaded.wk.named("CmUser"), "ok");
  EXPECT_TRUE(key.isHeap());
  EXPECT_EQ(key, first);
  EXPECT_TRUE(ao::lookup(loaded.heap, loaded.wk.named("CmUser"), key).isHeap());
}

TEST(ImageSaveLoad, WeakSlotNilsUnreachable) {
  {
    Boot b;
    ao::Oop weak = b.heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
    ao::Oop text = ao::Str::fromUtf8(b.heap, b.wk, "ao");
    ASSERT_TRUE(weak.isHeap());
    ASSERT_TRUE(text.isHeap());
    b.heap.slotAtPut(weak, 0, text);
    b.roots.add(&weak);

    const auto path = std::filesystem::path(testing::TempDir()) / "load-weak-dead.aoimage";
    ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
    EXPECT_EQ(text, b.heap.slotAt(weak, 0));

    Loaded loaded;
    ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
    auto loadedWeak = soleWeak(loaded.heap);
    ASSERT_TRUE(loadedWeak.isHeap());
    EXPECT_TRUE(loaded.heap.slotAt(loadedWeak, 0).isNil());
    EXPECT_EQ(text, b.heap.slotAt(weak, 0));
  }
  {
    Boot b;
    ao::Oop weak = b.heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
    ao::Oop text = ao::Str::fromUtf8(b.heap, b.wk, "ao");
    ASSERT_TRUE(weak.isHeap());
    ASSERT_TRUE(text.isHeap());
    b.heap.slotAtPut(weak, 0, text);
    b.roots.add(&weak);
    b.roots.add(&text);

    const auto path = std::filesystem::path(testing::TempDir()) / "load-weak-live.aoimage";
    ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
    EXPECT_EQ(text, b.heap.slotAt(weak, 0));

    Loaded loaded;
    ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
    auto loadedWeak = soleWeak(loaded.heap);
    ASSERT_TRUE(loadedWeak.isHeap());
    auto slot = loaded.heap.slotAt(loadedWeak, 0);
    ASSERT_TRUE(slot.isHeap());
    EXPECT_NE(text.heapPointer(), slot.heapPointer());
    EXPECT_EQ("ao", utf8Bytes(loaded.heap, slot));
    EXPECT_EQ(text, b.heap.slotAt(weak, 0));
  }
}

TEST(ImageSaveLoad, RejectsBadMagicAndSecondLoad) {
  Boot b;
  const auto good = std::filesystem::path(testing::TempDir()) / "load-good.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, good.string()));

  std::ifstream in(good, std::ios::binary);
  std::vector<char> bytes((std::istreambuf_iterator<char>(in)), {});
  ASSERT_FALSE(bytes.empty());
  bytes[0] = 'X';
  const auto bad = std::filesystem::path(testing::TempDir()) / "load-bad-magic.aoimage";
  {
    std::ofstream out(bad, std::ios::binary | std::ios::trunc);
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    ASSERT_TRUE(out.good());
  }

  Loaded rejected;
  EXPECT_FALSE(ao::Image::load(rejected.heap, rejected.roots, rejected.wk, bad.string()));
  EXPECT_EQ(0u, rejected.heap.oldUsed());

  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, good.string()));
  expectOnePlusTwo(loaded);
  const auto used = loaded.heap.oldUsed();
  const auto objectClass = loaded.wk.objectClass;
  EXPECT_FALSE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, good.string()));
  EXPECT_EQ(used, loaded.heap.oldUsed());
  EXPECT_EQ(objectClass, loaded.wk.objectClass);
  expectOnePlusTwo(loaded);
}

TEST(ImageSaveLoad, NurseryGcAfterLoad) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-nursery.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));

  std::size_t made = 0;
  while (loaded.heap.allocate(ao::Oop::nil(), 64, ao::kFlagBytes).isHeap()) {
    ++made;
  }
  ASSERT_GT(made, 0u);
  ao::Gc gc(loaded.heap, loaded.roots);
  gc.collectNursery();
  expectOnePlusTwo(loaded);
}

TEST(ImageSaveLoad, VendorLinkSurvives) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, AO_SOURCE_DIR "/image/vendor/LOAD_ORDER", errs))
      << (errs.empty() ? "" : errs[0].message);
  const auto path = std::filesystem::path(testing::TempDir()) / "load-vendor.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  auto image = std::make_unique<Loaded>();
  if (!ao::Image::load(image->heap, image->roots, image->wk, path.string())) {
    std::uint32_t heapBytes = 0;
    ASSERT_TRUE(readHeapBytes(path, &heapBytes));
    ASSERT_GT(static_cast<std::size_t>(heapBytes), image->heap.oldCapacity());
    image = std::make_unique<Loaded>(heapBytes);
    ASSERT_TRUE(ao::Image::load(image->heap, image->roots, image->wk, path.string()))
        << "heapBytes=" << heapBytes;
  }

  auto linkClass = image->wk.named("Link");
  ASSERT_TRUE(linkClass.isHeap());
  auto next = ao::lookup(image->heap, linkClass, image->wk.intern("nextLink"));
  ASSERT_TRUE(next.isHeap());
  EXPECT_EQ(image->wk.compiledMethodClass, image->heap.klass(next));
  auto link = send0(*image, linkClass, "new");
  ASSERT_TRUE(link.isHeap());
  EXPECT_TRUE(send0(*image, link, "nextLink").isNil());
}

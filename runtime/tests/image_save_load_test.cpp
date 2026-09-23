#include "test_support.hpp"

#include "ao_abi.h"

#include "ao/Bootstrap.hpp"
#include "ao/Bytecode.hpp"
#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Image.hpp"
#include "ao/ImageFormat.hpp"
#include "ao/Interpreter.hpp"
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

  Loaded(std::size_t oldBytes, std::size_t oldMaxBytes)
      : heap(1u << 20, oldBytes, oldMaxBytes), wk(heap, roots), ctx{heap, roots, wk, &cache} {
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

// OOP を書ける語だけを集める: heap 節の 8 バイト境界の語と、各レコードの値。
// 任意の位置の 8 バイトを見ると、old の番地（mmap なので 0x7000000000 のように下位が 0）が
// 隣り合う 2 語の境目に偶然現れる。
bool oopWords(const std::vector<char>& bytes, std::vector<std::uint64_t>* out) {
  const auto* p = reinterpret_cast<const std::byte*>(bytes.data());
  ao::ImageFormat::ImageHeader header;
  if (!ao::ImageFormat::readHeader(p, bytes.size(), &header)) return false;
  const std::size_t heapStart = ao::ImageFormat::kImageHeaderBytes;
  if (header.heapBytes > bytes.size() - heapStart) return false;
  auto word = [&](std::size_t at) {
    std::uint64_t w = 0;
    std::memcpy(&w, p + at, sizeof w);
    return w;
  };
  for (std::size_t i = 0; i + 8 <= header.heapBytes; i += 8) out->push_back(word(heapStart + i));
  std::size_t cursor = heapStart + header.heapBytes;
  const std::uint64_t records = std::uint64_t{header.wellKnownCount} + header.extraCount +
                                header.globalCount;
  for (std::uint64_t r = 0; r < records; ++r) {
    std::uint32_t n = 0;
    if (cursor + sizeof n > bytes.size()) return false;
    std::memcpy(&n, p + cursor, sizeof n);
    cursor += sizeof n + n + (4u - (n % 4u)) % 4u;
    if (cursor + 8 > bytes.size()) return false;
    out->push_back(word(cursor));
    cursor += 8;
  }
  return cursor == bytes.size();
}

// old にある、指定の大きさの byte object を 1 つ探す。
ao::Oop findBytesOfSize(ao::Heap& heap, std::uint32_t size) {
  const std::byte* p = heap.oldBase();
  const std::byte* end = p + heap.oldUsed();
  while (p < end) {
    const auto* h = reinterpret_cast<const ao::ObjectHeader*>(p);
    const std::size_t n = heap.objectBytes(h);
    if (n == 0 || n > static_cast<std::size_t>(end - p)) break;
    if ((h->flags & ao::kFlagBytes) != 0 && h->size == size) {
      return ao::Oop::fromHeap(const_cast<ao::ObjectHeader*>(h));
    }
    p += n;
  }
  return ao::Oop{};
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
  std::vector<std::uint64_t> words;
  ASSERT_TRUE(oopWords(bytes, &words));
  bool foundHost = false;
  for (std::uint64_t word : words) {
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
  // send は GC しうるので、それをまたぐ値はルートしておく。
  ao::Root loadedCls(loaded.roots, loaded.wk.named("CmUser"));
  ASSERT_TRUE(loadedCls.slot.isHeap());
  EXPECT_EQ(hashBefore, loaded.heap.hash(loadedCls.slot));

  auto sel = loaded.wk.intern("ok");
  auto meth = ao::lookup(loaded.heap, loadedCls.slot, sel);
  ASSERT_TRUE(meth.isHeap());
  EXPECT_EQ(loaded.wk.compiledMethodClass, loaded.heap.klass(meth));
  ASSERT_GE(loaded.heap.size(meth), ao::kCmSlotCount);
  EXPECT_TRUE(loaded.heap.slotAt(meth, ao::kCmSlotNativeCode).isNil());

  ao::Root inst(loaded.roots, send0(loaded, loadedCls.slot, "new"));
  ASSERT_TRUE(inst.slot.isHeap());
  auto three = send0(loaded, inst.slot, "ok");
  ASSERT_TRUE(three.isSmallInteger());
  EXPECT_EQ(3, three.smallIntegerValue());
  EXPECT_EQ("ao", utf8Bytes(loaded.heap, send0(loaded, inst.slot, "tag")));
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

// ヘッダの heapBytes が old の初期容量（4 MiB）を超えても、ロードはその分だけ old をコミットする。
TEST(ImageSaveLoad, LoadSizesOldFromHeader) {
  constexpr std::uint32_t kBlobBytes = 5u << 20;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-big.aoimage";
  {
    Boot b;
    ao::Oop blob = b.heap.allocate(b.wk.byteArrayClass, kBlobBytes, ao::kFlagBytes);
    ASSERT_TRUE(blob.isHeap());
    b.heap.bytes(blob)[0] = std::byte{0x11};
    b.heap.bytes(blob)[kBlobBytes - 1] = std::byte{0x22};
    b.roots.add(&blob);
    ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
    b.roots.remove(&blob);
  }
  std::uint32_t heapBytes = 0;
  ASSERT_TRUE(readHeapBytes(path, &heapBytes));
  ASSERT_GT(heapBytes, 4u << 20);

  Loaded loaded;
  ASSERT_LT(loaded.heap.oldCapacity(), heapBytes);
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  EXPECT_EQ(heapBytes, loaded.heap.oldUsed());
  EXPECT_GE(loaded.heap.oldCapacity(), static_cast<std::size_t>(heapBytes));
  expectOnePlusTwo(loaded);
  auto blob = findBytesOfSize(loaded.heap, kBlobBytes);
  ASSERT_TRUE(blob.isHeap());
  EXPECT_EQ(loaded.wk.byteArrayClass, loaded.heap.klass(blob));
  EXPECT_EQ(std::byte{0x11}, loaded.heap.bytes(blob)[0]);
  EXPECT_EQ(std::byte{0x22}, loaded.heap.bytes(blob)[kBlobBytes - 1]);

  ao::Gc gc(loaded.heap, loaded.roots);
  gc.collectNursery();
  expectOnePlusTwo(loaded);
}

// heapBytes が old の上限を超えるイメージは拒否し、old を空のまま残す。上限ちょうどなら受け付ける。
TEST(ImageSaveLoad, RejectsHeapBytesAboveOldMax) {
  const auto path = std::filesystem::path(testing::TempDir()) / "load-max.aoimage";
  {
    Boot b;
    ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  }
  std::uint32_t heapBytes = 0;
  ASSERT_TRUE(readHeapBytes(path, &heapBytes));
  ASSERT_GT(heapBytes, 64u << 10);

  Loaded below(64u << 10, heapBytes - 8);
  EXPECT_FALSE(ao::Image::load(below.heap, below.roots, below.wk, path.string()));
  EXPECT_EQ(0u, below.heap.oldUsed());

  Loaded exact(64u << 10, heapBytes);
  ASSERT_TRUE(ao::Image::load(exact.heap, exact.roots, exact.wk, path.string()));
  EXPECT_EQ(heapBytes, exact.heap.oldUsed());
  expectOnePlusTwo(exact);
}

namespace {

// SmallInteger 以外に送る特殊セレクタ（new: at:put: at: size value value: do: class ==）と、
// SmallInteger 同士の +。答えは 20 + 3 + 7 + 5 + 100 + 42 = 177。
constexpr const char* kSpecialSends =
    "f\n"
    "  | arr blk s |\n"
    "  arr := Array new: 3.\n"
    "  arr at: 1 put: 10.\n"
    "  arr at: 2 put: 20.\n"
    "  arr at: 3 put: 12.\n"
    "  blk := [:e | e].\n"
    "  s := 0.\n"
    "  arr do: [:e | s := s + e].\n"
    "  ^(arr at: 2) + arr size + (blk value: 7) + [5] value\n"
    "    + ((arr class == Array) ifTrue: [100] ifFalse: [0]) + s";

// source を Object のメソッドとしてコンパイルし、nil に対して引数なしで走らせる。
ao::Oop runSource(ao::CallContext& ctx, const char* source) {
  auto img = ao::compiler::compileMethod(source);
  if (!img.ok) {
    ADD_FAILURE() << img.error.message;
    return ao::Oop{};
  }
  ao::Root cm(ctx.roots, ao::boxMethodImage(ctx, img.image, ctx.wk.objectClass));
  if (!cm.slot.isHeap()) {
    return ao::Oop{};
  }
  return ao::Interpreter::run(ctx, cm.slot, ao::Oop::nil(), nullptr, 0, ao::Oop::nil());
}

void expectSpecialSends(ao::CallContext& ctx) {
  const std::uint64_t sends = ctx.interpretedSends;
  const ao::Oop got = runSource(ctx, kSpecialSends);
  ASSERT_TRUE(got.isSmallInteger());
  EXPECT_EQ(177, got.smallIntegerValue());
  EXPECT_GT(ctx.interpretedSends, sends);
}

// 前もって intern した特殊セレクタは、今の intern 表にある同じ名前の Symbol である。
void expectSpecialSelectorsInterned(ao::WellKnown& wk) {
  for (std::uint8_t k = 0; k < ao::WellKnown::kSpecialSelectorCount; ++k) {
    const char* name = ao::compiler::specialSelector(k);
    const ao::Oop sel = wk.specialSelector(k);
    ASSERT_TRUE(sel.isHeap()) << name;
    EXPECT_EQ(wk.intern(name), sel) << name;
  }
}

void collectAll(ao::Heap& heap, ao::Roots& roots) {
  ao::Gc gc(heap, roots);
  gc.collectNursery();
  gc.collectOld();
}

}  // namespace

// SendSpecial は前もって intern したセレクタを番号で引く（SPEC §3.5）。GC で Symbol が動いても、
// イメージを読み込んで WellKnown とヒープが替わっても、同じセレクタを送る。
TEST(ImageSaveLoad, SpecialSelectorsSurviveGcAndImageLoad) {
  Boot b;
  expectSpecialSelectorsInterned(b.wk);
  expectSpecialSends(b.ctx);
  collectAll(b.heap, b.roots);
  expectSpecialSelectorsInterned(b.wk);
  expectSpecialSends(b.ctx);

  const auto path = std::filesystem::path(testing::TempDir()) / "load-special.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  EXPECT_TRUE(loaded.wk.smallIntegerFastPath());
  expectSpecialSelectorsInterned(loaded.wk);
  expectSpecialSends(loaded.ctx);
  collectAll(loaded.heap, loaded.roots);
  expectSpecialSelectorsInterned(loaded.wk);
  expectSpecialSends(loaded.ctx);
}

// SPEC §3.5: 継承したネイティブを隠す SmallInteger>><= を持つ古いイメージ（HEAD の accept は通して
// いた）を読み込むと、そのセッションでは高速路を使わない。SendSpecial の答えは perform: と同じく
// ユーザーの定義になり、送信として数える。ほかの 7 セレクタも送る。
TEST(ImageSaveLoad, ShadowedSmallIntegerSelectorDisablesFastPath) {
  Boot b;
  EXPECT_TRUE(b.wk.smallIntegerFastPath());
  auto shadow = ao::compiler::compileMethod("<= x\n  ^false");
  ASSERT_TRUE(shadow.ok) << shadow.error.message;
  ASSERT_TRUE(ao::installMethod(b.ctx, b.wk.smallIntegerClass, shadow.image).isHeap());
  const auto path = std::filesystem::path(testing::TempDir()) / "load-shadowed.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  EXPECT_FALSE(loaded.wk.smallIntegerFastPath());
  std::uint64_t sends = loaded.ctx.interpretedSends;
  const ao::Oop special = runSource(loaded.ctx, "f\n  ^3 <= 4");
  EXPECT_EQ(1u, loaded.ctx.interpretedSends - sends);
  EXPECT_TRUE(special.isFalse());
  EXPECT_EQ(special, runSource(loaded.ctx, "f\n  ^3 perform: #<= with: 4"));
  sends = loaded.ctx.interpretedSends;
  EXPECT_EQ(ao::Oop::fromSmallInteger(7), runSource(loaded.ctx, "f\n  ^3 + 4"));
  EXPECT_EQ(1u, loaded.ctx.interpretedSends - sends);

  // セッションの経路（ao_image_load は ensureKernelNatives のあとで確かめ直す）でも同じ。
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str()));
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("3 <= 4", 6, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("false", out);
  ASSERT_EQ(AO_OK, ao_eval("3 perform: #<= with: 4", 22, AO_EVAL_PRINTIT, out, 64, &err))
      << err.message;
  EXPECT_STREQ("false", out);
  ao_runtime_shutdown();
}

// SPEC §3.10: ao_image_load はロードと探針（1 + 2 が 3、nil isNil が true）を新しいセッションで行い、
// どちらも通ったときだけ差し替える。探針に失敗したら AO_ERR で、ロード前のセッションを使い続ける。
TEST(ImageSaveLoad, FailedProbeKeepsCurrentSession) {
  Boot b;
  auto broken = ao::compiler::compileMethod("+ x\n  ^0");
  ASSERT_TRUE(broken.ok) << broken.error.message;
  ASSERT_TRUE(ao::installMethod(b.ctx, b.wk.smallIntegerClass, broken.image).isHeap());
  const auto path = std::filesystem::path(testing::TempDir()) / "load-probe-fails.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  {
    // Image::load は通る。落ちるのは探針である。
    Loaded loaded;
    ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  }

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("b3keep := 41", 12, AO_EVAL_DOIT, out, 64, &err)) << err.message;
  EXPECT_EQ(AO_ERR, ao_image_load(path.string().c_str()));
  ASSERT_EQ(AO_OK, ao_eval("b3keep + 1", 10, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("42", out);
  ASSERT_EQ(AO_OK, ao_eval("nil isNil", 9, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("true", out);
  ao_runtime_shutdown();
  std::filesystem::remove(path);
}

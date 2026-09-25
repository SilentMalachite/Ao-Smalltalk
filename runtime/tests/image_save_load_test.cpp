#include "test_support.hpp"

#include "ao_abi.h"

#include "ao/Bootstrap.hpp"
#include "ao/Bytecode.hpp"
#include "ao/Chunk.hpp"
#include "ao/Compile.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Compiler.hpp"
#include "ao/Gc.hpp"
#include "ao/Globals.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Image.hpp"
#include "ao/ImageFormat.hpp"
#include "ao/Interpreter.hpp"
#include "ao/Lookup.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/Symbol.hpp"

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <set>
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

std::vector<char> readAll(const std::filesystem::path& path) {
  std::ifstream in(path, std::ios::binary);
  return std::vector<char>((std::istreambuf_iterator<char>(in)), {});
}

bool writeAll(const std::filesystem::path& path, const std::vector<char>& bytes) {
  std::ofstream out(path, std::ios::binary | std::ios::trunc);
  out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  return static_cast<bool>(out);
}

// The records follow the heap: the well-known ones, the extra ones, the global ones. Each is a u32
// name length, the name padded to 4 bytes and the u64 bits. *end is where the well-known records
// end; the answer is where the bits of the one called name are, or 0.
std::size_t wellKnownBitsAt(const std::vector<char>& bytes, std::string_view name,
                            std::size_t* end) {
  const auto* p = reinterpret_cast<const std::byte*>(bytes.data());
  ao::ImageFormat::ImageHeader header;
  if (!ao::ImageFormat::readHeader(p, bytes.size(), &header)) return 0;
  std::size_t cursor = ao::ImageFormat::kImageHeaderBytes + header.heapBytes;
  std::size_t found = 0;
  for (std::uint32_t r = 0; r < header.wellKnownCount; ++r) {
    std::uint32_t n = 0;
    if (cursor + sizeof n > bytes.size()) return 0;
    std::memcpy(&n, p + cursor, sizeof n);
    if (cursor + sizeof n + n > bytes.size()) return 0;
    const std::string_view recName(bytes.data() + cursor + sizeof n, n);
    cursor += sizeof n + n + (4u - (n % 4u)) % 4u;
    if (cursor + 8 > bytes.size()) return 0;
    if (recName == name) found = cursor;
    cursor += 8;
  }
  *end = cursor;
  return found;
}

void expectOnePlusTwo(Loaded& image) {
  auto three = send1(image, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  ASSERT_TRUE(three.isSmallInteger());
  EXPECT_EQ(3, three.smallIntegerValue());
}

// While it lives, a write that would make a file longer than bytes fails with EFBIG instead of
// killing the process (RLIMIT_FSIZE with SIGXFSZ ignored), as a full disk makes a write fail.
class FileSizeLimit {
 public:
  explicit FileSizeLimit(rlim_t bytes) {
    struct sigaction ignore {};
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    sigaction(SIGXFSZ, &ignore, &oldAction_);
    getrlimit(RLIMIT_FSIZE, &oldLimit_);
    struct rlimit limit = oldLimit_;
    limit.rlim_cur = bytes;
    armed_ = setrlimit(RLIMIT_FSIZE, &limit) == 0;
  }
  ~FileSizeLimit() {
    setrlimit(RLIMIT_FSIZE, &oldLimit_);
    sigaction(SIGXFSZ, &oldAction_, nullptr);
  }
  FileSizeLimit(const FileSizeLimit&) = delete;
  FileSizeLimit& operator=(const FileSizeLimit&) = delete;
  bool armed() const { return armed_; }

 private:
  struct sigaction oldAction_ {};
  struct rlimit oldLimit_ {};
  bool armed_ = false;
};

// The names of the files in dir, sorted.
std::vector<std::string> fileNames(const std::filesystem::path& dir) {
  std::vector<std::string> names;
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    names.push_back(entry.path().filename().string());
  }
  std::sort(names.begin(), names.end());
  return names;
}

// A fresh directory under the test temp directory, for tests that look at what a save leaves.
std::filesystem::path freshDir(const char* name) {
  const auto dir = std::filesystem::path(testing::TempDir()) / name;
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
  std::filesystem::create_directories(dir);
  return dir;
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
  std::vector<ao::FileInError> errs;
  ASSERT_TRUE(ao::fileInLoadOrder(b.ctx, AO_SOURCE_DIR "/image/vendor/LOAD_ORDER", errs))
      << (errs.empty() ? "" : errs[0].error.message);
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
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), nullptr));
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
  EXPECT_EQ(AO_ERR, ao_image_load(path.string().c_str(), &err));
  EXPECT_STREQ("image probes failed", err.message);
  ASSERT_EQ(AO_OK, ao_eval("b3keep + 1", 10, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("42", out);
  ASSERT_EQ(AO_OK, ao_eval("nil isNil", 9, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("true", out);
  ao_runtime_shutdown();
  std::filesystem::remove(path);
}

// SPEC §3.11: グローバル辞書より前のイメージ（Smalltalk が 57 要素の表）は、修復せずに拒否する。
// 保存したイメージの well-known 表の Smalltalk を、同じ 57 の値を並べた表に向け直して、それを作る。
TEST(ImageSaveLoad, RefusesSmalltalkWithoutGlobalDictionary) {
  Boot b;
  {
    // An Array: a SmalltalkImage has the format's 2 slots, and B6's format check (SPEC §3.11)
    // refuses to save one of 57.
    ao::Root old(b.roots, b.heap.allocate(b.wk.arrayClass, ao::Globals::kSmalltalkCount, 0));
    ASSERT_TRUE(old.slot.isHeap());
    for (std::uint32_t i = 0; i < ao::Globals::kSmalltalkCount; ++i) {
      b.heap.slotAtPut(old.slot, i, b.wk.named(ao::Globals::nameAt(i)));
    }
    ASSERT_TRUE(b.wk.define("B4OldTable", old.slot));
  }
  const auto path = std::filesystem::path(testing::TempDir()) / "load-old-smalltalk.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  std::uint64_t oldOffset = 0;
  {
    // ロードした old はファイルのヒープ節と同じ並びなので、oldBase からの距離がファイル内の位置。
    Loaded probe;
    ASSERT_TRUE(ao::Image::load(probe.heap, probe.roots, probe.wk, path.string()));
    const ao::Oop table = probe.wk.named("B4OldTable");
    ASSERT_TRUE(table.isHeap());
    oldOffset = static_cast<std::uint64_t>(static_cast<const std::byte*>(table.heapPointer()) -
                                           probe.heap.oldBase());
  }
  std::vector<char> bytes = readAll(path);
  std::size_t end = 0;
  const std::size_t at = wellKnownBitsAt(bytes, "Smalltalk", &end);
  ASSERT_NE(0u, at);
  std::memcpy(bytes.data() + at, &oldOffset, sizeof oldOffset);
  ASSERT_TRUE(writeAll(path, bytes));
  Loaded image;
  EXPECT_FALSE(ao::Image::load(image.heap, image.roots, image.wk, path.string()));
  std::filesystem::remove(path);
}

// SPEC §3.11: 足したグローバルは辞書にだけある。extra のレコードを持つイメージは旧形式として拒否する。
TEST(ImageSaveLoad, RefusesExtraRecords) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-extra-record.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  {
    Loaded plain;
    ASSERT_TRUE(ao::Image::load(plain.heap, plain.roots, plain.wk, path.string()));
  }
  std::vector<char> bytes = readAll(path);
  std::size_t end = 0;
  ASSERT_NE(0u, wellKnownBitsAt(bytes, "Smalltalk", &end));
  ao::ImageFormat::ImageHeader header;
  ASSERT_TRUE(ao::ImageFormat::readHeader(reinterpret_cast<const std::byte*>(bytes.data()),
                                          bytes.size(), &header));
  ASSERT_EQ(0u, header.extraCount);
  header.extraCount = 1;
  ao::ImageFormat::writeHeader(reinterpret_cast<std::byte*>(bytes.data()), header);
  // "Zap" -> 3: 名前 3 バイトと詰め物 1 バイト、値の 8 バイト。
  std::vector<char> record(4 + 4 + 8, 0);
  const std::uint32_t n = 3;
  std::memcpy(record.data(), &n, sizeof n);
  std::memcpy(record.data() + 4, "Zap", 3);
  const std::uint64_t bits = ao::Oop::fromSmallInteger(3).bits();
  std::memcpy(record.data() + 8, &bits, sizeof bits);
  bytes.insert(bytes.begin() + static_cast<std::ptrdiff_t>(end), record.begin(), record.end());
  ASSERT_TRUE(writeAll(path, bytes));
  Loaded image;
  EXPECT_FALSE(ao::Image::load(image.heap, image.roots, image.wk, path.string()));
  std::filesystem::remove(path);
}

// B4 review (Codex P2) / SPEC §3.10, §3.11: 失敗シナリオ。旧形式のイメージ（版 1）が理由なしで拒まれ、
// 壊れたファイルと区別できなかった。ロードはヘッダの段階で理由付きで拒み、何も載せない。ABI は理由を
// AoSpan.message に入れ、ロード前のセッションを使い続ける。
TEST(ImageSaveLoad, RefusesVersionOneWithReason) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "load-version-1.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  std::vector<char> bytes = readAll(path);
  ASSERT_GT(bytes.size(), 6u);
  EXPECT_EQ(3, bytes[4]);
  EXPECT_EQ(0, bytes[5]);
  {
    Loaded current;
    std::string reason = "unchanged";
    ASSERT_TRUE(ao::Image::load(current.heap, current.roots, current.wk, path.string(), &reason));
  }
  // B9 / SPEC §3.11: version 2 has the flat Dictionary and Set pairs the hashed natives cannot
  // read, so it is refused at the header like version 1.
  bytes[4] = 2;
  ASSERT_TRUE(writeAll(path, bytes));
  {
    Loaded old;
    std::string reason;
    EXPECT_FALSE(ao::Image::load(old.heap, old.roots, old.wk, path.string(), &reason));
    EXPECT_EQ("unsupported image version 2", reason);
    EXPECT_EQ(0u, old.heap.oldUsed());
  }
  bytes[4] = 1;
  ASSERT_TRUE(writeAll(path, bytes));
  {
    Loaded old;
    std::string reason;
    EXPECT_FALSE(ao::Image::load(old.heap, old.roots, old.wk, path.string(), &reason));
    EXPECT_EQ("unsupported image version 1", reason);
    EXPECT_EQ(0u, old.heap.oldUsed());
  }
  const auto garbage = std::filesystem::path(testing::TempDir()) / "load-not-an-image.aoimage";
  ASSERT_TRUE(writeAll(garbage, std::vector<char>{'n', 'o', 't', ' ', 'a', 'n', ' ', 'i', 'm'}));
  const auto missing = std::filesystem::path(testing::TempDir()) / "load-no-such.aoimage";
  {
    Loaded none;
    std::string reason;
    EXPECT_FALSE(ao::Image::load(none.heap, none.roots, none.wk, garbage.string(), &reason));
    EXPECT_EQ("not an Ao image", reason);
    EXPECT_FALSE(ao::Image::load(none.heap, none.roots, none.wk, missing.string(), &reason));
    EXPECT_EQ("cannot read image file", reason);
  }

  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_eval("b4keep := 41", 12, AO_EVAL_DOIT, out, 64, &err)) << err.message;
  EXPECT_EQ(AO_ERR, ao_image_load(path.string().c_str(), &err));
  EXPECT_STREQ("unsupported image version 1", err.message);
  EXPECT_EQ(AO_ERR, ao_image_load(garbage.string().c_str(), &err));
  EXPECT_STREQ("not an Ao image", err.message);
  EXPECT_EQ(AO_ERR, ao_image_load(garbage.string().c_str(), nullptr));
  ASSERT_EQ(AO_OK, ao_eval("b4keep + 1", 10, AO_EVAL_PRINTIT, out, 64, &err)) << err.message;
  EXPECT_STREQ("42", out);
  bytes[4] = 2;
  ASSERT_TRUE(writeAll(path, bytes));
  EXPECT_EQ(AO_ERR, ao_image_load(path.string().c_str(), &err));
  EXPECT_STREQ("unsupported image version 2", err.message);
  bytes[4] = 3;
  ASSERT_TRUE(writeAll(path, bytes));
  EXPECT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err));
  EXPECT_STREQ("", err.message);
  ao_runtime_shutdown();
  std::filesystem::remove(path);
  std::filesystem::remove(garbage);
}

// B6 review (06 High #1) / SPEC §3.11: 失敗シナリオ。保存は保存先を切り詰めて直接書いていたので、書き込みが
// 途中で失敗する（ファイルサイズの上限、ディスクフル）と、正常だった旧イメージが縮んでロードできなくなった。
// 保存は一時ファイルに書いてから置き換えるので、失敗しても旧イメージはそのまま残り、一時ファイルも残らない。
TEST(ImageSave, FailedWriteKeepsOldImage) {
  const auto dir = freshDir("b6-failed-write");
  const auto path = dir / "keep.aoimage";
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> before = readAll(path);
  ASSERT_GT(before.size(), std::size_t{32} << 10);
  {
    FileSizeLimit limit(16u << 10);
    ASSERT_TRUE(limit.armed());
    EXPECT_FALSE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  }
  const std::vector<char> after = readAll(path);
  EXPECT_EQ(before.size(), after.size());
  EXPECT_TRUE(before == after);
  EXPECT_EQ(std::vector<std::string>{"keep.aoimage"}, fileNames(dir));
  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  expectOnePlusTwo(loaded);
  // 保存したセッションもそのまま使える。
  auto three = send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  EXPECT_EQ(ao::Oop::fromSmallInteger(3), three);
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

// SPEC §3.11: 置き換えた保存先は、旧イメージのパーミッションを引き継ぐ。一時ファイルは残らない。
TEST(ImageSave, ReplacingSaveKeepsPermissionsAndLeavesNoTemporaryFile) {
  namespace fs = std::filesystem;
  const auto dir = freshDir("b6-replace");
  const auto path = dir / "replace.aoimage";
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const auto mode = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read;
  fs::permissions(path, mode, fs::perm_options::replace);
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  EXPECT_EQ(mode, fs::status(path).permissions() & fs::perms::mask);
  EXPECT_EQ(std::vector<std::string>{"replace.aoimage"}, fileNames(dir));
  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, path.string()));
  expectOnePlusTwo(loaded);
  std::error_code ec;
  fs::remove_all(dir, ec);
}

// SPEC §3.11: 保存先のディレクトリに書けなければ保存は失敗し、何も残さない。
TEST(ImageSave, UnwritableDirectoryFailsWithoutFiles) {
  namespace fs = std::filesystem;
  const auto dir = freshDir("b6-unwritable");
  Boot b;
  fs::permissions(dir, fs::perms::owner_read | fs::perms::owner_exec, fs::perm_options::replace);
  EXPECT_FALSE(ao::Image::save(b.heap, b.roots, b.wk, (dir / "none.aoimage").string()));
  fs::permissions(dir, fs::perms::owner_all, fs::perm_options::replace);
  EXPECT_TRUE(fileNames(dir).empty());
  std::error_code ec;
  fs::remove_all(dir, ec);
}

// B6 review (06 High #2) / SPEC §3.11: 失敗シナリオ。nextPutAll: が do: に渡すネイティブのブロック（thunk）
// を利用者の do: がリテラル配列に残すと、ao_image_save は AO_OK なのに、同じファイルの ao_image_load は
// AO_ERR だった（thunk の NativeMethod の名前 ao_NativeBlock_thunk を結び直せない）。thunk は関数の登録名を
// 持つので、そのイメージはロードでき、逃げた thunk はロードのあとも同じストリームに書く。
TEST(ImageSaveLoad, EscapedStreamThunkSurvivesSaveAndLoad) {
  const auto path = std::filesystem::path(testing::TempDir()) / "b6-escaped-thunk.aoimage";
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* def =
      "Object subclass: #B6Keeper\n"
      "  instanceVariableNames: ''\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B6-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B6Keeper", 0, "holder\n  ^#(1)\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B6Keeper", 0, "do: aBlock\n  self holder at: 1 put: aBlock\n",
                                    &err))
      << err.message;
  char out[64];
  auto eval = [&](const std::string& src, int mode) {
    return ao_eval(src.c_str(), static_cast<int>(src.size()), mode, out, 64, &err);
  };
  ASSERT_EQ(AO_OK, eval("Smalltalk at: #B6KeptStream put: (WriteStream on: String new)",
                        AO_EVAL_DOIT))
      << err.message;
  ASSERT_EQ(AO_OK, eval("B6KeptStream nextPutAll: B6Keeper new", AO_EVAL_DOIT)) << err.message;
  ASSERT_EQ(AO_OK, eval("(B6Keeper new holder at: 1) class == BlockContext", AO_EVAL_PRINTIT))
      << err.message;
  ASSERT_STREQ("true", out);

  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  EXPECT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  ASSERT_EQ(AO_OK, eval("(B6Keeper new holder at: 1) value: $z. B6KeptStream contents",
                        AO_EVAL_PRINTIT))
      << err.message;
  EXPECT_STREQ("'z'", out);
  ao_runtime_shutdown();
  std::filesystem::remove(path);
}

// SPEC §3.11: 名前を登録していない関数の thunk（ao_NativeBlock_thunk）がヒープに逃げていれば、保存は失敗
// する。そのイメージはロードで結び直せないからである。保存先の旧イメージは残る。
TEST(ImageSave, UnresolvableNativeNameFailsAndKeepsOldImage) {
  const auto dir = freshDir("b6-unnamed-thunk");
  const auto path = dir / "old.aoimage";
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> before = readAll(path);
  auto body = [](ao::CallContext&, const ao::Oop& receiver, const ao::Oop*, std::uint32_t) {
    return receiver;
  };
  ao::Root blk(b.roots, ao::makeNativeBlock(b.ctx, body, 0));
  ASSERT_TRUE(blk.slot.isHeap());
  const ao::Oop method = b.heap.slotAt(blk.slot, ao::kCtxMethod);
  EXPECT_EQ("ao_NativeBlock_thunk", ao::NativeMethod::nameBytes(b.heap, method));
  EXPECT_FALSE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> after = readAll(path);
  EXPECT_TRUE(before == after);
  EXPECT_EQ(std::vector<std::string>{"old.aoimage"}, fileNames(dir));
  // 逃げた thunk を手放せば、保存できる。
  blk.slot = ao::Oop::nil();
  EXPECT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

// B6 review (06 High #3) / SPEC §3.11: 失敗シナリオ。生存データは nursery と old の両方にあり、ロードはそれを
// 1 つの old に並べる。old の上限（ここでは 4 MiB）の中で動いていたセッションでも、合わせると上限を超える
// ことがあり、保存は成功するのに、そのイメージは同じ上限の old にロードできなかった。保存は heapBytes が
// 保存するヒープの old の上限を超えるなら失敗し、旧イメージは残る。
TEST(ImageSave, HeapBeyondOldLimitFailsAndKeepsOldImage) {
  constexpr std::size_t kMax = std::size_t{4} << 20;
  const auto dir = freshDir("b6-capacity");
  const auto path = dir / "cap.aoimage";
  Boot b(std::size_t{4} << 20, kMax, kMax);
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> before = readAll(path);
  {
    Loaded fits(64u << 10, kMax);
    ASSERT_TRUE(ao::Image::load(fits.heap, fits.roots, fits.wk, path.string()));
  }

  constexpr std::uint32_t kSlots = 256;
  ao::Root keep(b.roots, b.heap.allocate(b.wk.arrayClass, kSlots, 0));
  ASSERT_TRUE(keep.slot.isHeap());
  std::uint32_t used = 0;
  std::size_t kept = 0;
  // old: 128 KiB（大きなオブジェクトは old に直接置く）を、old が上限に達するまで。
  constexpr std::uint32_t kOldBlob = 128u << 10;
  ASSERT_GE(kOldBlob, b.heap.largeObjectBytes());
  while (used < kSlots) {
    const ao::Oop blob = b.heap.allocate(b.wk.byteArrayClass, kOldBlob, ao::kFlagBytes);
    if (!blob.isHeap()) break;
    b.heap.slotAtPut(keep.slot, used++, blob);
    kept += kOldBlob;
  }
  ASSERT_GT(b.heap.oldUsed(), kMax - 2 * std::size_t{kOldBlob});
  // nursery: 16 KiB を、合わせて old の上限を 512 KiB 超えるまで。
  constexpr std::uint32_t kYoungBlob = 16u << 10;
  ASSERT_LT(kYoungBlob, b.heap.largeObjectBytes());
  while (kept <= kMax + (512u << 10) && used < kSlots) {
    const ao::Oop blob = b.heap.allocate(b.wk.byteArrayClass, kYoungBlob, ao::kFlagBytes);
    ASSERT_TRUE(blob.isHeap());
    b.heap.slotAtPut(keep.slot, used++, blob);
    kept += kYoungBlob;
  }
  ASSERT_GT(kept, kMax);
  EXPECT_LE(b.heap.oldUsed(), kMax);

  EXPECT_FALSE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> after = readAll(path);
  EXPECT_TRUE(before == after);
  EXPECT_EQ(std::vector<std::string>{"cap.aoimage"}, fileNames(dir));
  // セッションはそのまま動く。
  auto three = send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  EXPECT_EQ(ao::Oop::fromSmallInteger(3), three);
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

namespace {

// A saved image's bytes, for tests that damage one field and load the result. Offsets are heap
// offsets (what the image writes for a pointer); the heap section starts after the 48-byte header.
struct ImageSurgery {
  static constexpr std::size_t kHeap = ao::ImageFormat::kImageHeaderBytes;
  std::vector<char> bytes;
  ao::ImageFormat::ImageHeader header;

  explicit ImageSurgery(const std::filesystem::path& path) : bytes(readAll(path)) {
    const bool ok = ao::ImageFormat::readHeader(reinterpret_cast<const std::byte*>(bytes.data()),
                                                bytes.size(), &header);
    EXPECT_TRUE(ok);
  }
  std::uint64_t word(std::size_t pos) const {
    std::uint64_t w = 0;
    std::memcpy(&w, bytes.data() + pos, sizeof w);
    return w;
  }
  void setWord(std::size_t pos, std::uint64_t w) { std::memcpy(bytes.data() + pos, &w, sizeof w); }
  ao::ObjectHeader object(std::uint64_t off) const {
    ao::ObjectHeader h{};
    std::memcpy(&h, bytes.data() + kHeap + off, sizeof h);
    return h;
  }
  void setObject(std::uint64_t off, const ao::ObjectHeader& h) {
    std::memcpy(bytes.data() + kHeap + off, &h, sizeof h);
  }
  std::size_t slotPos(std::uint64_t off, std::uint32_t i) const {
    return kHeap + static_cast<std::size_t>(off) + sizeof(ao::ObjectHeader) + std::size_t{i} * 8u;
  }
  // The file position of the bits of well-known record name; 0 when there is none.
  std::size_t recordPos(std::string_view name) const {
    std::size_t end = 0;
    return wellKnownBitsAt(bytes, name, &end);
  }
  std::uint64_t record(std::string_view name) const { return word(recordPos(name)); }
  // Heap offsets of the objects after the filler, in file order.
  std::vector<std::uint64_t> objects() const {
    ao::Heap sizer(4096, 4096);
    std::vector<std::uint64_t> out;
    std::uint64_t off = ao::ImageFormat::kImageFillerBytes;
    while (off < header.heapBytes) {
      const ao::ObjectHeader h = object(off);
      out.push_back(off);
      off += sizer.objectBytes(&h);
    }
    return out;
  }
  // The first object whose klass is the well-known class className.
  std::uint64_t firstInstanceOf(std::string_view className) const {
    const std::uint64_t cls = record(className);
    for (std::uint64_t off : objects()) {
      if (object(off).klass.bits() == cls) return off;
    }
    return 0;
  }
  bool write(const std::filesystem::path& path) const { return writeAll(path, bytes); }
  // The class-shaped object whose name slot holds a byte object with these bytes; 0 when none.
  std::uint64_t classNamed(std::string_view name) const {
    for (std::uint64_t off : objects()) {
      const ao::ObjectHeader h = object(off);
      if ((h.flags & ao::kFlagBytes) != 0 || h.size < ao::kClassSlotCount) continue;
      const std::uint64_t nameOff = word(slotPos(off, ao::kClassSlotName));
      if (nameOff == 0 || (nameOff & 7u) != 0 || nameOff >= header.heapBytes) continue;
      const ao::ObjectHeader n = object(nameOff);
      if ((n.flags & ao::kFlagBytes) == 0 || n.size != name.size()) continue;
      if (std::memcmp(bytes.data() + kHeap + nameOff + sizeof(ao::ObjectHeader), name.data(),
                      name.size()) == 0) {
        return off;
      }
    }
    return 0;
  }
};

// Saves a fresh Boot to path and checks it loads; the tests then damage a copy of it.
void saveFreshImage(const std::filesystem::path& path) {
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  Loaded plain;
  ASSERT_TRUE(ao::Image::load(plain.heap, plain.roots, plain.wk, path.string()));
}

// Loads path into a fresh heap and expects the refusal reason, with nothing adopted.
void expectRefused(const std::filesystem::path& path, const std::string& why) {
  Loaded image;
  std::string reason;
  EXPECT_FALSE(ao::Image::load(image.heap, image.roots, image.wk, path.string(), &reason));
  EXPECT_EQ(why, reason);
  EXPECT_EQ(0u, image.heap.oldUsed());
}

}  // namespace

// B6 review (06 Medium) / SPEC §3.11: 失敗シナリオ。ロードはファイル上の flags をそのまま採用していた。
// ポインタのオブジェクトに Marked（0x10）を立てたイメージはロードも評価も通り、そのあとの collectOld が
// その子を回収して SIGSEGV になった。Bytes と Weak のほかの flags は拒否する。詰め物も同じ。
TEST(ImageLoadChecks, RefusesFlagsOtherThanBytesAndWeak) {
  const auto good = std::filesystem::path(testing::TempDir()) / "b6-flags-good.aoimage";
  const auto bad = std::filesystem::path(testing::TempDir()) / "b6-flags-bad.aoimage";
  saveFreshImage(good);
  const ImageSurgery image(good);
  const std::uint64_t smalltalk = image.record("Smalltalk");
  ASSERT_NE(0u, smalltalk);
  ASSERT_EQ(0, image.object(smalltalk).flags & ao::kFlagBytes);
  for (std::uint16_t flag : {ao::kFlagOld, ao::kFlagImmovable, ao::kFlagMarked, ao::kFlagForwarded,
                             std::uint16_t{1u << 6}, std::uint16_t{1u << 15}}) {
    ImageSurgery damaged = image;
    ao::ObjectHeader h = damaged.object(smalltalk);
    h.flags = static_cast<std::uint16_t>(h.flags | flag);
    damaged.setObject(smalltalk, h);
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "damaged image");
  }
  ImageSurgery filler = image;
  ao::ObjectHeader f = filler.object(0);
  f.flags = static_cast<std::uint16_t>(f.flags | ao::kFlagMarked);
  filler.setObject(0, f);
  ASSERT_TRUE(filler.write(bad));
  expectRefused(bad, "damaged image");
  std::filesystem::remove(good);
  std::filesystem::remove(bad);
}

// B6 review (06 Medium) / SPEC §3.11: 失敗シナリオ。klass は参照先がオブジェクトの先頭かどうかしか見て
// いなかった。Processor の klass を Symbol にしたイメージはロードが通り、Processor printString で落ちた。
// klass は nil か、クラスの形（ポインタ形で kClassSlotCount 以上のスロット）のオブジェクトに限る。
TEST(ImageLoadChecks, RefusesKlassThatIsNotAClass) {
  const auto good = std::filesystem::path(testing::TempDir()) / "b6-klass-good.aoimage";
  const auto bad = std::filesystem::path(testing::TempDir()) / "b6-klass-bad.aoimage";
  saveFreshImage(good);
  const ImageSurgery image(good);
  const std::uint64_t processor = image.record("Processor");
  const std::uint64_t symbol = image.record("value");
  ASSERT_NE(0u, processor);
  ASSERT_NE(0u, symbol);
  ASSERT_NE(0, image.object(symbol).flags & ao::kFlagBytes);
  const std::uint64_t smallInteger = ao::Oop::fromSmallInteger(3).bits();
  const std::uint64_t character = ao::Oop::fromCharacter(U'a').bits();
  for (std::uint64_t klass : {smallInteger, character, ao::Oop::true_().bits(), symbol}) {
    ImageSurgery damaged = image;
    ao::ObjectHeader h = damaged.object(processor);
    h.klass = ao::Oop::fromBits(klass);
    damaged.setObject(processor, h);
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "damaged image");
  }
  std::filesystem::remove(good);
  std::filesystem::remove(bad);
}

// B6 review (06 Medium) / SPEC §3.11: 失敗シナリオ。well-known のクラスとメソッド辞書の形を見ていなかった。
// SmallInteger の methodDict を Symbol にしたイメージは、ロードの中の lookup が Symbol を辞書として読んで
// abort した。well-known のクラスがバイト列、セレクタが Symbol でない、MethodDictionary や
// CompiledMethod の形が足りない、のどれも拒否する。
TEST(ImageLoadChecks, RefusesMisshapenClassesAndMethods) {
  const auto good = std::filesystem::path(testing::TempDir()) / "b6-shape-good.aoimage";
  const auto bad = std::filesystem::path(testing::TempDir()) / "b6-shape-bad.aoimage";
  saveFreshImage(good);
  const ImageSurgery image(good);
  const std::uint64_t symbol = image.record("value");
  const std::uint64_t smallIntegerClass = image.record("SmallInteger");
  const std::uint64_t processor = image.record("Processor");
  ASSERT_NE(0u, symbol);
  ASSERT_NE(0u, smallIntegerClass);
  auto refused = [&](const char* what, auto&& damage) {
    SCOPED_TRACE(what);
    ImageSurgery damaged = image;
    damage(damaged);
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "damaged image");
  };
  refused("methodDict is a Symbol", [&](ImageSurgery& d) {
    d.setWord(d.slotPos(smallIntegerClass, ao::kClassSlotMethodDict), symbol);
  });
  refused("methodDict is a SmallInteger", [&](ImageSurgery& d) {
    d.setWord(d.slotPos(smallIntegerClass, ao::kClassSlotMethodDict),
              ao::Oop::fromSmallInteger(1).bits());
  });
  refused("well-known class is a Symbol",
          [&](ImageSurgery& d) { d.setWord(d.recordPos("SmallInteger"), symbol); });
  refused("well-known metaclass is a SmallInteger", [&](ImageSurgery& d) {
    d.setWord(d.recordPos("SmallInteger class"), ao::Oop::fromSmallInteger(7).bits());
  });
  refused("well-known selector is not a Symbol", [&](ImageSurgery& d) {
    d.setWord(d.recordPos("value"), ao::Oop::fromSmallInteger(7).bits());
  });
  refused("MethodDictionary holds a byte array", [&](ImageSurgery& d) {
    const std::uint64_t dict = d.word(d.slotPos(smallIntegerClass, ao::kClassSlotMethodDict));
    d.setWord(d.slotPos(dict, ao::kDictSlotArray), symbol);
  });
  refused("CompiledMethod with two slots", [&](ImageSurgery& d) {
    ao::ObjectHeader h = d.object(processor);
    ASSERT_LT(h.size, ao::kCmSlotCount);
    h.klass = ao::Oop::fromBits(d.record("CompiledMethod"));
    d.setObject(processor, h);
  });
  refused("CompiledMethod that is bytes", [&](ImageSurgery& d) {
    ao::ObjectHeader h = d.object(symbol);
    h.klass = ao::Oop::fromBits(d.record("CompiledMethod"));
    d.setObject(symbol, h);
  });
  std::filesystem::remove(good);
  std::filesystem::remove(bad);
}

// B6 review (06 Low) / SPEC §3.11: ロードはファイル全体を読んでからヘッダを照合していた。ロードはまずヘッダの
// 48 バイトだけを読み、heapBytes が old の上限を超えるなら本体を読まずにその理由で拒否する。ファイルの
// 大きさがヘッダとレコードの取りうる範囲に無ければ、本体を読まずに壊れたイメージとして拒否する。
TEST(ImageLoadChecks, RefusesHugeHeapBytesFromTheHeader) {
  const auto good = std::filesystem::path(testing::TempDir()) / "b6-huge-good.aoimage";
  const auto bad = std::filesystem::path(testing::TempDir()) / "b6-huge-bad.aoimage";
  saveFreshImage(good);
  const ImageSurgery image(good);
  {
    // The header claims almost 4 GiB, the file holds 64 bytes.
    ImageSurgery damaged = image;
    ao::ImageFormat::ImageHeader h = damaged.header;
    h.heapBytes = 0xFFFFFFF8u;
    ao::ImageFormat::writeHeader(reinterpret_cast<std::byte*>(damaged.bytes.data()), h);
    damaged.bytes.resize(ao::ImageFormat::kImageHeaderBytes + 16);
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "image heap exceeds the old space limit");
  }
  {
    // Past a smaller old limit, the reason is the limit too, not the short file.
    ImageSurgery damaged = image;
    ao::ImageFormat::ImageHeader h = damaged.header;
    h.heapBytes = 2u << 20;
    ao::ImageFormat::writeHeader(reinterpret_cast<std::byte*>(damaged.bytes.data()), h);
    ASSERT_TRUE(damaged.write(bad));
    Loaded small(64u << 10, 1u << 20);
    std::string reason;
    EXPECT_FALSE(ao::Image::load(small.heap, small.roots, small.wk, bad.string(), &reason));
    EXPECT_EQ("image heap exceeds the old space limit", reason);
    EXPECT_EQ(0u, small.heap.oldUsed());
  }
  {
    // A file longer than its records can be is damaged.
    ImageSurgery damaged = image;
    damaged.bytes.resize(damaged.bytes.size() + (64u << 10), '\0');
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "damaged image");
  }
  std::filesystem::remove(good);
  std::filesystem::remove(bad);
}

namespace {

// Boots, defines B6Bar (one instance variable, a method, an instance kept in a global) and saves
// the session to path; then runs setup, a legal evaluation that leaves a heap the load's checks
// refuse, and expects the save over path to fail with the old image left as it was.
void expectSaveRefusedAfter(const std::filesystem::path& path, const std::string& setup) {
  SCOPED_TRACE(setup);
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  const char* def =
      "Object subclass: #B6Bar\n"
      "  instanceVariableNames: 'a'\n"
      "  classVariableNames: ''\n"
      "  poolDictionaries: ''\n"
      "  category: 'B6-Test'\n";
  ASSERT_EQ(AO_OK, ao_accept_class(def, &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B6Bar", 0, "hello\n  ^'hi'\n", &err)) << err.message;
  char out[64];
  const std::string keep = "Smalltalk at: #B6BarKeep put: B6Bar new";
  ASSERT_EQ(AO_OK, ao_eval(keep.c_str(), static_cast<int>(keep.size()), AO_EVAL_DOIT, out, 64,
                           &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  const std::vector<char> before = readAll(path);
  ASSERT_EQ(AO_OK, ao_eval(setup.c_str(), static_cast<int>(setup.size()), AO_EVAL_DOIT, out, 64,
                           &err))
      << err.message;
  EXPECT_EQ(AO_ERR, ao_image_save(path.string().c_str()));
  const std::vector<char> after = readAll(path);
  EXPECT_TRUE(before == after);
  EXPECT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  ao_runtime_shutdown();
}

}  // namespace

// B6 review (Claude M) / SPEC §3.11: 失敗シナリオ。正当な Smalltalk で作れるヒープを、save は AO_OK で書き、
// load は damaged image で拒否した。保存は成功したのに旧イメージを失う。保存は書く前にロードと同じ検査を
// 当てるので、これらの保存は失敗し、旧イメージはそのまま残ってロードできる。
TEST(ImageSaveChecks, SaveRefusesWhatTheLoadRefuses) {
  const auto dir = freshDir("b6-save-checks");
  const auto path = dir / "keep.aoimage";
  for (const char* setup : {
           "B6Bar instVarAt: 2 put: Dictionary new",
           "B6Bar instVarAt: 2 put: 7",
           "B6Bar instVarAt: 2 put: #(1 2)",
           "| d | d := MethodDictionary new. d instVarAt: 2 put: 'xyz'. Smalltalk at: #B6MD put: d",
           "B6Bar instVarAt: 1 put: #(1 2 3 4 5 6 7 8 9)",
       }) {
    expectSaveRefusedAfter(path, setup);
  }
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

// SPEC §3.11: Image::save says which object broke which rule.
TEST(ImageSaveChecks, SaveReasonNamesTheClassAndTheRule) {
  const auto dir = freshDir("b6-save-reason");
  const auto path = dir / "reason.aoimage";
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(fileInSource(b, kUserMethods, &errs)) << (errs.empty() ? "" : errs[0].message);
  ao::Root cls(b.roots, b.wk.named("CmUser"));
  ASSERT_TRUE(cls.slot.isHeap());
  ao::Root dict(b.roots, b.heap.slotAt(cls.slot, ao::kClassSlotMethodDict));
  b.heap.slotAtPut(cls.slot, ao::kClassSlotMethodDict, ao::Oop::fromSmallInteger(7));
  std::string reason;
  EXPECT_FALSE(ao::Image::save(b.heap, b.roots, b.wk, path.string(), &reason));
  EXPECT_NE(std::string::npos, reason.find("CmUser")) << reason;
  EXPECT_NE(std::string::npos, reason.find("methodDict")) << reason;
  EXPECT_TRUE(fileNames(dir).empty());
  b.heap.slotAtPut(cls.slot, ao::kClassSlotMethodDict, dict.slot);
  reason = "unchanged";
  EXPECT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string(), &reason)) << reason;
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

// SPEC §3.11: a class of the runtime's reading. `Behavior new` makes a class whose format is nil,
// which the runtime reads as no named slots: its instance saves and loads.
TEST(ImageSaveChecks, AnonymousBehaviorInstanceSavesAndLoads) {
  const auto path = std::filesystem::path(testing::TempDir()) / "b6-behavior-new.aoimage";
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  char out[64];
  AoSpan err{};
  const std::string setup =
      "| b | b := Behavior new. Smalltalk at: #B6BH put: b. Smalltalk at: #B6BI put: b basicNew";
  ASSERT_EQ(AO_OK, ao_eval(setup.c_str(), static_cast<int>(setup.size()), AO_EVAL_DOIT, out, 64,
                           &err))
      << err.message;
  EXPECT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  EXPECT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  ao_runtime_shutdown();
  std::filesystem::remove(path);
}

// B6 review (Claude M / Codex P2 #1) / SPEC §3.11: 失敗シナリオ。インスタンスの無いユーザークラスの
// methodDict を Symbol にしたイメージはロードが通り、Foo new foo が slotAt の assert で落ちた。Behavior の
// オブジェクトは、インスタンスの有無によらず methodDict と superclass を確かめる。
TEST(ImageLoadChecks, RefusesClassWithoutInstancesAndABadMethodDict) {
  const auto good = std::filesystem::path(testing::TempDir()) / "b6-noinst-good.aoimage";
  const auto bad = std::filesystem::path(testing::TempDir()) / "b6-noinst-bad.aoimage";
  {
    Boot b;
    std::vector<ao::compiler::CompileError> errs;
    ASSERT_TRUE(fileInSource(b, kUserMethods, &errs)) << (errs.empty() ? "" : errs[0].message);
    ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, good.string()));
  }
  const ImageSurgery image(good);
  const std::uint64_t user = image.classNamed("CmUser");
  ASSERT_NE(0u, user);
  for (std::uint64_t off : image.objects()) {
    ASSERT_NE(user, image.object(off).klass.bits()) << "CmUser has an instance";
  }
  const std::uint64_t symbol = image.record("value");
  auto refused = [&](const char* what, auto&& damage) {
    SCOPED_TRACE(what);
    ImageSurgery damaged = image;
    damage(damaged);
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "damaged image");
  };
  refused("methodDict is a Symbol", [&](ImageSurgery& d) {
    d.setWord(d.slotPos(user, ao::kClassSlotMethodDict), symbol);
  });
  refused("superclass is a Symbol", [&](ImageSurgery& d) {
    d.setWord(d.slotPos(user, ao::kClassSlotSuperclass), symbol);
  });
  refused("superclass is a 9-slot object that is no class", [&](ImageSurgery& d) {
    std::uint64_t big = 0;
    for (std::uint64_t off : d.objects()) {
      const ao::ObjectHeader h = d.object(off);
      if ((h.flags & ao::kFlagBytes) == 0 && h.size >= ao::kClassSlotCount &&
          h.klass.bits() == d.record("Array")) {
        big = off;
        break;
      }
    }
    ASSERT_NE(0u, big);
    d.setWord(d.slotPos(user, ao::kClassSlotSuperclass), big);
  });
  std::filesystem::remove(good);
  std::filesystem::remove(bad);
}

// B6 review (Claude M / Codex P2 #2) / SPEC §3.11: 失敗シナリオ。2 スロットのオブジェクトの klass を
// ReadStream にしたイメージは、Kept next で i < h->size の assert になった。String の klass を Array に
// すると printString が落ちた。インスタンスはクラスの format に合う形をしている。
TEST(ImageLoadChecks, RefusesInstancesThatDoNotFitTheirClassFormat) {
  const auto good = std::filesystem::path(testing::TempDir()) / "b6-format-good.aoimage";
  const auto bad = std::filesystem::path(testing::TempDir()) / "b6-format-bad.aoimage";
  saveFreshImage(good);
  const ImageSurgery image(good);
  const std::uint64_t processor = image.record("Processor");
  ASSERT_EQ(2u, image.object(processor).size);
  auto refused = [&](const std::string& what, auto&& damage) {
    SCOPED_TRACE(what);
    ImageSurgery damaged = image;
    damage(damaged);
    ASSERT_TRUE(damaged.write(bad));
    expectRefused(bad, "damaged image");
  };
  for (const char* cls :
       {"ReadStream", "OrderedCollection", "Interval", "Class", "BlockContext", "Symbol"}) {
    refused(std::string("2 slots as a ") + cls, [&](ImageSurgery& d) {
      ao::ObjectHeader h = d.object(processor);
      h.klass = ao::Oop::fromBits(d.record(cls));
      d.setObject(processor, h);
    });
  }
  const std::uint64_t metaName = image.word(image.slotPos(image.record("Object class"),
                                                          ao::kClassSlotName));
  ASSERT_EQ(image.record("String"), image.object(metaName).klass.bits());
  refused("a String as an Array", [&](ImageSurgery& d) {
    ao::ObjectHeader h = d.object(metaName);
    h.klass = ao::Oop::fromBits(d.record("Array"));
    d.setObject(metaName, h);
  });
  refused("a weak object with a class", [&](ImageSurgery& d) {
    const std::uint64_t smalltalk = d.record("Smalltalk");
    ao::ObjectHeader h = d.object(smalltalk);
    h.flags = static_cast<std::uint16_t>(h.flags | ao::kFlagWeak);
    d.setObject(smalltalk, h);
  });
  std::filesystem::remove(good);
  std::filesystem::remove(bad);
}

// B6 review (Claude Low) / SPEC §3.11: 失敗シナリオ。rename は保存先のシンボリックリンクを通常ファイルで
// 置き換えていた。保存はリンクをたどった先を置き換え、リンクは残す。リンク先がまだ無くてもよい。
TEST(ImageSave, SavingThroughASymbolicLinkKeepsTheLink) {
  namespace fs = std::filesystem;
  const auto dir = freshDir("b6-symlink");
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, (dir / "real.aoimage").string()));
  fs::create_symlink("real.aoimage", dir / "link.aoimage");
  ASSERT_TRUE(b.wk.define("B6LinkMark", ao::Oop::fromSmallInteger(7)));
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, (dir / "link.aoimage").string()));
  EXPECT_TRUE(fs::is_symlink(dir / "link.aoimage"));
  EXPECT_EQ(fs::path("real.aoimage"), fs::read_symlink(dir / "link.aoimage"));
  {
    Loaded loaded;
    ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, (dir / "real.aoimage").string()));
    EXPECT_EQ(ao::Oop::fromSmallInteger(7), loaded.wk.named("B6LinkMark"));
  }
  // A link to a file that is not there yet: the save makes the file and keeps the link.
  fs::create_symlink("later.aoimage", dir / "dangling.aoimage");
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, (dir / "dangling.aoimage").string()));
  EXPECT_TRUE(fs::is_symlink(dir / "dangling.aoimage"));
  EXPECT_TRUE(fs::is_regular_file(dir / "later.aoimage"));
  EXPECT_EQ((std::vector<std::string>{"dangling.aoimage", "later.aoimage", "link.aoimage",
                                      "real.aoimage"}),
            fileNames(dir));
  std::error_code ec;
  fs::remove_all(dir, ec);
}

// B6 review (Claude Low) / SPEC §3.11: 失敗シナリオ。書き込み権の無い既存のイメージも rename で置き換えて
// いた。旧実装（保存先を開いて書く）と同じく、書き込めない保存先への保存は失敗し、何も変えない。
TEST(ImageSave, ReadOnlyImageIsNotReplaced) {
  namespace fs = std::filesystem;
  const auto dir = freshDir("b6-readonly");
  const auto path = dir / "ro.aoimage";
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  const std::vector<char> before = readAll(path);
  fs::permissions(path, fs::perms::owner_read | fs::perms::group_read, fs::perm_options::replace);
  ASSERT_TRUE(b.wk.define("B6ReadOnlyMark", ao::Oop::fromSmallInteger(1)));
  std::string reason;
  EXPECT_FALSE(ao::Image::save(b.heap, b.roots, b.wk, path.string(), &reason));
  EXPECT_FALSE(reason.empty());
  const std::vector<char> after = readAll(path);
  EXPECT_TRUE(before == after);
  EXPECT_EQ(std::vector<std::string>{"ro.aoimage"}, fileNames(dir));
  fs::permissions(path, fs::perms::owner_all, fs::perm_options::replace);
  std::error_code ec;
  fs::remove_all(dir, ec);
}

// B6 review (Claude Low) / SPEC §3.11: 失敗シナリオ。一時ファイルの名前は保存先の名前に .tmp-<pid>-<n> を
// 足したもので、長い名前（250 バイト）の保存先では NAME_MAX を超えて保存できなかった。
TEST(ImageSave, LongFileNameSaves) {
  const auto dir = freshDir("b6-long-name");
  const std::string name = std::string(242, 'a') + ".aoimage";
  ASSERT_EQ(250u, name.size());
  Boot b;
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, (dir / name).string()));
  EXPECT_EQ(std::vector<std::string>{name}, fileNames(dir));
  Loaded loaded;
  ASSERT_TRUE(ao::Image::load(loaded.heap, loaded.roots, loaded.wk, (dir / name).string()));
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

// B6 review (Claude Low) / SPEC §3.11: 失敗シナリオ。`ulimit -f 20; ao image save` は SIGXFSZ でプロセスが
// 終わり、一時ファイルが残った。保存は書く間だけ SIGXFSZ を無視し、EFBIG で失敗して一時ファイルを消し、
// そのあとシグナルの扱いを元に戻す。シグナルを無視しない子プロセスで確かめる。
TEST(ImageSave, FileSizeLimitFailsWithoutTheSignal) {
  const auto dir = freshDir("b6-sigxfsz");
  const auto path = dir / "limit.aoimage";
  const pid_t pid = fork();
  ASSERT_NE(-1, pid);
  if (pid == 0) {
    struct sigaction dfl {};
    dfl.sa_handler = SIG_DFL;
    sigemptyset(&dfl.sa_mask);
    sigaction(SIGXFSZ, &dfl, nullptr);
    struct rlimit limit {};
    getrlimit(RLIMIT_FSIZE, &limit);
    limit.rlim_cur = 16u << 10;
    if (setrlimit(RLIMIT_FSIZE, &limit) != 0) _exit(2);
    Boot b;
    const bool saved = ao::Image::save(b.heap, b.roots, b.wk, path.string());
    struct sigaction now {};
    sigaction(SIGXFSZ, nullptr, &now);
    if (now.sa_handler != SIG_DFL) _exit(3);
    _exit(saved ? 4 : 0);
  }
  int status = 0;
  ASSERT_EQ(pid, waitpid(pid, &status, 0));
  ASSERT_FALSE(WIFSIGNALED(status)) << "signal " << WTERMSIG(status);
  ASSERT_TRUE(WIFEXITED(status));
  EXPECT_EQ(0, WEXITSTATUS(status));
  EXPECT_TRUE(fileNames(dir).empty());
  std::error_code ec;
  std::filesystem::remove_all(dir, ec);
}

namespace {

// A Collection whose do: keeps every block it is given (the Kernel natives pass their thunks) in
// the class variable Kept, then runs it on 1 and 2.
const char kThunkKeeper[] =
    "!Collection subclass: #B6Keeper\n"
    "  instanceVariableNames: ''\n"
    "  classVariableNames: 'Kept N'\n"
    "  poolDictionaries: ''\n"
    "  category: 'B6-Test'!\n"
    "!B6Keeper methodsFor: 't'!\n"
    "size\n"
    "  ^2!\n"
    "do: aBlock\n"
    "  Kept isNil ifTrue: [Kept := Array new: 16. N := 0].\n"
    "  N := N + 1.\n"
    "  Kept at: N put: aBlock.\n"
    "  aBlock value: 1.\n"
    "  aBlock value: 2! !\n"
    "!B6Keeper class methodsFor: 't'!\n"
    "kept\n"
    "  ^Kept!\n"
    "count\n"
    "  ^N! !\n";

// Each Kernel native that passes a thunk to do:, and the thunk functions' names it uses.
const char* const kThunkSends[] = {
    "B6Keeper new collect: [:x | x * 10]",
    "B6Keeper new select: [:x | x > 1]",
    "B6Keeper new reject: [:x | x > 1]",
    "B6Keeper new detect: [:x | x > 5] ifNone: [0]",
    "B6Keeper new inject: 0 into: [:a :b | a + b]",
    "B6Keeper new includes: 2",
    "(WriteStream on: (Array new: 4)) nextPutAll: B6Keeper new",
};

}  // namespace

// B6 review (Claude Low) / SPEC §3.11: each Kernel thunk's NativeMethod carries the name its
// function was registered under, and that name resolves to the function the thunk calls (the
// registry index in the method), not merely to some function.
TEST(ImageRegistry, KernelThunksCarryTheNameOfTheirFunction) {
  Boot b;
  std::vector<ao::compiler::CompileError> errs;
  ASSERT_TRUE(fileInSource(b, kThunkKeeper, &errs)) << (errs.empty() ? "" : errs[0].message);
  for (const char* send : kThunkSends) {
    const std::string source = std::string("f\n  ^") + send;
    runSource(b.ctx, source.c_str());
    ASSERT_FALSE(b.ctx.aborting) << send;
  }
  ao::Root kept(b.roots, runSource(b.ctx, "f\n  ^B6Keeper kept"));
  const ao::Oop count = runSource(b.ctx, "f\n  ^B6Keeper count");
  ASSERT_TRUE(kept.slot.isHeap());
  ASSERT_TRUE(count.isSmallInteger());
  std::set<std::string> names;
  for (std::int64_t i = 0; i < count.smallIntegerValue(); ++i) {
    const ao::Oop blk = b.heap.slotAt(kept.slot, static_cast<std::uint32_t>(i));
    ASSERT_EQ(b.wk.blockContextClass, b.heap.klass(blk));
    const ao::Oop method = b.heap.slotAt(blk, ao::kCtxMethod);
    ASSERT_EQ(b.wk.nativeMethodClass, b.heap.klass(method));
    const std::string name(ao::NativeMethod::nameBytes(b.heap, method));
    std::uint32_t found = 0;
    ASSERT_TRUE(ao::NativeRegistry::findName(name, &found)) << name;
    const ao::Oop index = b.heap.slotAt(method, ao::kNativeSlotRegistryIndex);
    ASSERT_TRUE(index.isSmallInteger());
    EXPECT_EQ(index.smallIntegerValue(), static_cast<std::int64_t>(found)) << name;
    names.insert(name);
  }
  EXPECT_EQ((std::set<std::string>{"ao_Collection_collect_fill", "ao_Collection_filter_scan",
                                   "ao_Collection_detect_scan", "ao_Collection_inject_scan",
                                   "ao_Collection_includes_scan", "ao_Stream_nextPutAll_each"}),
            names);
}

// B6 review (Claude Low) / SPEC §3.11: the Collection thunks (collect, select, reject, detect,
// inject, includes) and the stream's, kept by a user's do:, survive save and load and run after
// it on their saved state. None crashes; inject's goes on from its sum 3.
TEST(ImageSaveLoad, EscapedCollectionThunksRunAfterSaveAndLoad) {
  const auto path = std::filesystem::path(testing::TempDir()) / "b6-collection-thunks.aoimage";
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  AoSpan err{};
  ASSERT_EQ(AO_OK, ao_accept_class("Collection subclass: #B6Keeper\n"
                                   "  instanceVariableNames: ''\n"
                                   "  classVariableNames: 'Kept N'\n"
                                   "  poolDictionaries: ''\n"
                                   "  category: 'B6-Test'\n",
                                   &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B6Keeper", 0, "size\n  ^2\n", &err)) << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B6Keeper", 0,
                                    "do: aBlock\n"
                                    "  Kept isNil ifTrue: [Kept := Array new: 16. N := 0].\n"
                                    "  N := N + 1.\n"
                                    "  Kept at: N put: aBlock.\n"
                                    "  aBlock value: 1.\n"
                                    "  aBlock value: 2\n",
                                    &err))
      << err.message;
  ASSERT_EQ(AO_OK, ao_accept_method("B6Keeper", 1, "kept\n  ^Kept\n", &err)) << err.message;
  char out[128];
  auto eval = [&](const std::string& src) {
    return ao_eval(src.c_str(), static_cast<int>(src.size()), AO_EVAL_PRINTIT, out, 128, &err);
  };
  for (const char* send : kThunkSends) {
    ASSERT_EQ(AO_OK, eval(send)) << send << ": " << err.message;
  }
  ASSERT_EQ(AO_OK, eval("(B6Keeper kept select: [:e | e notNil]) size")) << err.message;
  EXPECT_STREQ("7", out);  // one thunk each (SPEC §3.6: select: and reject: send do: once).

  ASSERT_EQ(AO_OK, ao_image_save(path.string().c_str()));
  ASSERT_EQ(AO_OK, ao_image_load(path.string().c_str(), &err)) << err.message;
  for (int i = 1; i <= 7; ++i) {
    const int rc = eval("(B6Keeper kept at: " + std::to_string(i) + ") value: 5");
    EXPECT_TRUE(rc == AO_OK || rc == AO_ERR_EVAL) << i << ": " << rc;
  }
  ASSERT_EQ(AO_OK, eval("(B6Keeper kept at: 5) value: 5")) << err.message;  // inject:into:
  EXPECT_STREQ("13", out);  // 3, then 8 above, then 13
  // B9: the select: and reject: thunks keep their buffer (slot 7) and count (slot 2) across the
  // image: select: kept 2 before the save and 5 above, reject: kept 1 before the save.
  ASSERT_EQ(AO_OK, eval("(B6Keeper kept at: 2) value: 7. (B6Keeper kept at: 2) instVarAt: 2"))
      << err.message;
  EXPECT_STREQ("3", out);
  ASSERT_EQ(AO_OK, eval("| t | t := B6Keeper kept at: 2. ((t instVarAt: 7) at: 1) * 100 + "
                        "(((t instVarAt: 7) at: 2) * 10) + ((t instVarAt: 7) at: 3)"))
      << err.message;
  EXPECT_STREQ("257", out);
  ASSERT_EQ(AO_OK, eval("(B6Keeper kept at: 3) value: 0. (B6Keeper kept at: 3) instVarAt: 2"))
      << err.message;
  EXPECT_STREQ("2", out);
  ASSERT_EQ(AO_OK, eval("((B6Keeper kept at: 3) instVarAt: 7) at: 2")) << err.message;
  EXPECT_STREQ("0", out);
  ASSERT_EQ(AO_OK, eval("1 + 2")) << err.message;
  EXPECT_STREQ("3", out);
  ao_runtime_shutdown();
  std::filesystem::remove(path);
}

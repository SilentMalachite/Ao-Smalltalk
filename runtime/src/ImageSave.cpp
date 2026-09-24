#include "ao/Image.hpp"

#include "ao/Globals.hpp"
#include "ao/ImageFormat.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ao {
namespace {

static_assert(std::endian::native == std::endian::little);

constexpr std::uint16_t kSavedFlags = static_cast<std::uint16_t>(kFlagBytes | kFlagWeak);

struct NamedOop {
  std::string name;
  Oop value;
};

struct Trace {
  Heap* heap = nullptr;
  std::vector<Oop> order;
  std::unordered_map<std::uintptr_t, std::uint64_t> at;
  std::uint64_t end = ImageFormat::kImageFillerBytes;
  bool failed = false;
};

bool validName(std::string_view name) { return name.size() >= 1 && name.size() <= 256; }

bool remember(Trace& tr, Oop obj) {
  if (tr.failed) {
    return false;
  }
  if (!obj.isHeap()) {
    return true;
  }
  const auto key = reinterpret_cast<std::uintptr_t>(obj.heapPointer());
  if (tr.at.find(key) != tr.at.end()) {
    return true;
  }
  const ObjectHeader* h = tr.heap->header(obj);
  if ((h->flags & (kFlagMarked | kFlagForwarded)) != 0) {
    tr.failed = true;
    return false;
  }
  const std::uint64_t nbytes = tr.heap->objectBytes(h);
  if (nbytes > static_cast<std::uint64_t>(UINT32_MAX) ||
      tr.end > static_cast<std::uint64_t>(UINT32_MAX) - nbytes) {
    tr.failed = true;
    return false;
  }
  tr.at.emplace(key, tr.end);
  tr.order.push_back(obj);
  tr.end += nbytes;
  return true;
}

void traceRoot(void* ctx, Oop* slot) {
  if (slot == nullptr) {
    return;
  }
  remember(*static_cast<Trace*>(ctx), *slot);
}

bool traceStrong(Heap& heap, Roots& roots, Trace& tr) {
  tr.heap = &heap;
  roots.visitAll(traceRoot, &tr);
  if (tr.failed) {
    return false;
  }
  for (std::size_t i = 0; i < tr.order.size(); ++i) {
    const Oop obj = tr.order[i];
    const ObjectHeader* h = heap.header(obj);
    if (!remember(tr, h->klass)) {
      return false;
    }
    if ((h->flags & kFlagBytes) != 0 || (h->flags & kFlagWeak) != 0) {
      continue;
    }
    for (std::uint32_t s = 0; s < h->size; ++s) {
      if (!remember(tr, heap.slotAt(obj, s))) {
        return false;
      }
    }
  }
  return !tr.failed && tr.end >= ImageFormat::kImageFillerBytes && (tr.end % 8u) == 0;
}

bool encodeOop(const Trace& tr, Oop obj, bool missingBecomesNil, std::uint64_t* bits) {
  if (!obj.isHeap()) {
    return ImageFormat::encodeNonHeap(obj, bits);
  }
  const auto it = tr.at.find(reinterpret_cast<std::uintptr_t>(obj.heapPointer()));
  if (it == tr.at.end()) {
    if (!missingBecomesNil) {
      return false;
    }
    return ImageFormat::encodeNonHeap(Oop::nil(), bits);
  }
  *bits = it->second;
  return true;
}

void appendRaw(std::vector<std::byte>& dst, const void* p, std::size_t n) {
  const auto* b = static_cast<const std::byte*>(p);
  dst.insert(dst.end(), b, b + n);
}

bool appendRecord(std::vector<std::byte>& dst, const Trace& tr, const NamedOop& rec) {
  std::uint64_t bits = 0;
  if (!encodeOop(tr, rec.value, false, &bits)) {
    return false;
  }
  const auto n = static_cast<std::uint32_t>(rec.name.size());
  appendRaw(dst, &n, sizeof n);
  if (n != 0) {
    appendRaw(dst, rec.name.data(), n);
  }
  const std::uint32_t pad = (4u - (n % 4u)) % 4u;
  if (pad != 0) {
    dst.insert(dst.end(), pad, std::byte{0});
  }
  appendRaw(dst, &bits, sizeof bits);
  return true;
}

struct NameCollect {
  std::vector<NamedOop>* out = nullptr;
  bool failed = false;
};

void collectImageSlot(void* ctx, const char* name, Oop value) {
  auto* c = static_cast<NameCollect*>(ctx);
  if (c->failed) {
    return;
  }
  if (name == nullptr) {
    c->failed = true;
    return;
  }
  const std::string_view view(name);
  if (!validName(view)) {
    c->failed = true;
    return;
  }
  c->out->push_back(NamedOop{std::string(view), value});
}

bool writeObject(const Heap& heap, const Trace& tr, Oop obj, std::byte* dst) {
  const ObjectHeader* h = heap.header(obj);
  std::uint64_t klassBits = 0;
  if (!encodeOop(tr, h->klass, false, &klassBits)) {
    return false;
  }
  const std::uint32_t size = h->size;
  const std::uint16_t flags = static_cast<std::uint16_t>(h->flags & kSavedFlags);
  const std::uint16_t hash = h->hash;
  std::memcpy(dst, &klassBits, sizeof klassBits);
  std::memcpy(dst + 8, &size, sizeof size);
  std::memcpy(dst + 12, &flags, sizeof flags);
  std::memcpy(dst + 14, &hash, sizeof hash);

  const std::size_t total = heap.objectBytes(h);
  if (total < sizeof(ObjectHeader)) {
    return false;
  }
  const std::size_t payload = total - sizeof(ObjectHeader);
  std::byte* body = dst + sizeof(ObjectHeader);
  if ((h->flags & kFlagBytes) != 0) {
    if (static_cast<std::size_t>(size) > payload) {
      return false;
    }
    if (size != 0) {
      std::memcpy(body, reinterpret_cast<const std::byte*>(h + 1), size);
    }
    return true;
  }
  if (static_cast<std::size_t>(size) * 8u > payload) {
    return false;
  }
  const bool weak = (h->flags & kFlagWeak) != 0;
  for (std::uint32_t i = 0; i < size; ++i) {
    std::uint64_t bits = 0;
    if (!encodeOop(tr, heap.slotAt(obj, i), weak, &bits)) {
      return false;
    }
    std::memcpy(body + static_cast<std::size_t>(i) * 8u, &bits, sizeof bits);
  }
  return true;
}

bool writeAllBytes(int fd, const std::byte* data, std::size_t n) {
  while (n > 0) {
    const std::size_t chunk = std::min<std::size_t>(n, std::size_t{1} << 30);
    const ssize_t w = ::write(fd, data, chunk);
    if (w < 0) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }
    if (w == 0) {
      return false;
    }
    data += w;
    n -= static_cast<std::size_t>(w);
  }
  return true;
}

// On macOS fsync leaves the data in the drive's cache; F_FULLFSYNC flushes it. A file system
// without F_FULLFSYNC still takes fsync.
bool syncFile(int fd) {
#ifdef F_FULLFSYNC
  if (::fcntl(fd, F_FULLFSYNC) == 0) {
    return true;
  }
#endif
  return ::fsync(fd) == 0;
}

// Makes the rename that put path in place durable. Best effort: the image is already in place, and
// some file systems refuse fsync on a directory.
void syncParentDirectory(const std::string& path) {
  const std::size_t slash = path.find_last_of('/');
  const std::string dir =
      slash == std::string::npos ? std::string(".") : (slash == 0 ? std::string("/") : path.substr(0, slash));
  const int fd = ::open(dir.c_str(), O_RDONLY | O_CLOEXEC);
  if (fd < 0) {
    return;
  }
  (void)syncFile(fd);
  ::close(fd);
}

// SPEC §3.11 (保存): the image goes to a temporary file next to path, which replaces path only once
// all of it is on disk. Any failure removes the temporary file and leaves path as it was.
bool writeFile(std::string_view path, const std::byte* data, std::size_t n) {
  static std::atomic<unsigned> serial{0};
  const std::string target(path);
  if (target.empty()) {
    return false;
  }
  std::string temp;
  int fd = -1;
  for (int attempt = 0; attempt < 64 && fd < 0; ++attempt) {
    temp = target + ".tmp-" + std::to_string(::getpid()) + "-" + std::to_string(serial++);
    // 0666 under the umask, as the file the save wrote before.
    fd = ::open(temp.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0666);
    if (fd < 0 && errno != EEXIST) {
      return false;
    }
  }
  if (fd < 0) {
    return false;
  }
  bool ok = true;
  // A save over an image keeps that image's permissions.
  struct stat old {};
  if (::stat(target.c_str(), &old) == 0 && S_ISREG(old.st_mode)) {
    ok = ::fchmod(fd, old.st_mode & 07777) == 0;
  }
  ok = ok && writeAllBytes(fd, data, n) && syncFile(fd);
  if (::close(fd) != 0) {
    ok = false;
  }
  if (ok && ::rename(temp.c_str(), target.c_str()) != 0) {
    ok = false;
  }
  if (!ok) {
    ::unlink(temp.c_str());
    return false;
  }
  syncParentDirectory(target);
  return true;
}

}  // namespace

bool Image::save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path) {
  // SPEC §3.11: the global dictionary goes with the heap. Nothing else records a global.
  if (!Globals::isDictionary(wk, wk.smalltalk)) {
    return false;
  }

  Trace tr;
  if (!traceStrong(heap, roots, tr)) {
    return false;
  }

  std::vector<NamedOop> wellKnown;
  NameCollect wkCollect{&wellKnown, false};
  wk.eachImageSlot(collectImageSlot, &wkCollect);
  if (wkCollect.failed) {
    return false;
  }

  std::vector<NamedOop> globals;
  globals.reserve(Globals::kSmalltalkCount);
  for (std::uint32_t i = 0; i < Globals::kSmalltalkCount; ++i) {
    const char* name = Globals::nameAt(i);
    if (name == nullptr || !validName(name)) {
      return false;
    }
    // The values the load checks the dictionary against.
    const Oop value = Globals::lookup(wk, wk.findSymbol(name));
    if (value.isEmpty()) {
      return false;
    }
    globals.push_back(NamedOop{std::string(name), value});
  }

  // 既定の上限（kOldMaxBytes）を超えるヒープは保存しない。保存できてもロードできない。
  if (tr.end > kOldMaxBytes) {
    return false;
  }
  std::vector<std::byte> heapBuf(static_cast<std::size_t>(tr.end), std::byte{0});
  ImageFormat::writeFiller(heapBuf.data());
  for (Oop obj : tr.order) {
    const auto it = tr.at.find(reinterpret_cast<std::uintptr_t>(obj.heapPointer()));
    if (it == tr.at.end()) {
      return false;
    }
    const std::size_t off = static_cast<std::size_t>(it->second);
    const std::size_t nbytes = heap.objectBytes(heap.header(obj));
    if (off > heapBuf.size() || nbytes > heapBuf.size() - off) {
      return false;
    }
    if (!writeObject(heap, tr, obj, heapBuf.data() + off)) {
      return false;
    }
  }

  std::vector<std::byte> tail;
  for (const NamedOop& rec : wellKnown) {
    if (!appendRecord(tail, tr, rec)) {
      return false;
    }
  }
  for (const NamedOop& rec : globals) {
    if (!appendRecord(tail, tr, rec)) {
      return false;
    }
  }

  ImageFormat::ImageHeader header;
  header.version = ImageFormat::kImageVersion;
  header.pointerBits = ImageFormat::kImagePointerBits;
  header.endian = ImageFormat::kImageEndianLittle;
  header.heapBytes = static_cast<std::uint32_t>(tr.end);
  header.wellKnownCount = static_cast<std::uint32_t>(wellKnown.size());
  header.extraCount = 0;
  header.globalCount = Globals::kSmalltalkCount;
  header.nextHash = heap.hashCursor();

  std::byte head[ImageFormat::kImageHeaderBytes];
  ImageFormat::writeHeader(head, header);

  std::vector<std::byte> file;
  file.reserve(sizeof head + heapBuf.size() + tail.size());
  appendRaw(file, head, sizeof head);
  appendRaw(file, heapBuf.data(), heapBuf.size());
  if (!tail.empty()) {
    appendRaw(file, tail.data(), tail.size());
  }
  return writeFile(path, file.data(), file.size());
}

}  // namespace ao

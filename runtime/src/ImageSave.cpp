#include "ao/Image.hpp"

#include "ao/Globals.hpp"
#include "ao/ImageFormat.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
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
  const WellKnown* wk = nullptr;
  std::vector<NamedOop>* out = nullptr;
  bool rejectCatalog = false;
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

void collectExtra(void* ctx, std::string_view name, Oop cls) {
  auto* c = static_cast<NameCollect*>(ctx);
  if (c->failed) {
    return;
  }
  if (c->rejectCatalog && c->wk->isCatalogName(name)) {
    c->failed = true;
    return;
  }
  if (!validName(name)) {
    c->failed = true;
    return;
  }
  c->out->push_back(NamedOop{std::string(name), cls});
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

bool writeFile(std::string_view path, const std::byte* data, std::size_t n) {
  const std::string pathStr(path);
  std::ofstream out(pathStr.c_str(), std::ios::binary | std::ios::trunc);
  if (!out) {
    return false;
  }
  out.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(n));
  out.close();
  return !out.fail();
}

}  // namespace

bool Image::save(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path) {
  if (!wk.smalltalk.isHeap() || heap.size(wk.smalltalk) != Globals::kSmalltalkCount) {
    return false;
  }

  Trace tr;
  if (!traceStrong(heap, roots, tr)) {
    return false;
  }

  std::vector<NamedOop> wellKnown;
  std::vector<NamedOop> extra;
  NameCollect wkCollect{&wk, &wellKnown, false, false};
  wk.eachImageSlot(collectImageSlot, &wkCollect);
  if (wkCollect.failed) {
    return false;
  }
  NameCollect exCollect{&wk, &extra, true, false};
  wk.eachExtra(collectExtra, &exCollect);
  if (exCollect.failed) {
    return false;
  }

  std::vector<NamedOop> globals;
  globals.reserve(Globals::kSmalltalkCount);
  for (std::uint32_t i = 0; i < Globals::kSmalltalkCount; ++i) {
    const char* name = Globals::nameAt(i);
    if (name == nullptr || !validName(name)) {
      return false;
    }
    globals.push_back(NamedOop{std::string(name), heap.slotAt(wk.smalltalk, i)});
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
  for (const NamedOop& rec : extra) {
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
  header.extraCount = static_cast<std::uint32_t>(extra.size());
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

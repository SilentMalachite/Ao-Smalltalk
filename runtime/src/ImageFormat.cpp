#include "ao/ImageFormat.hpp"
#include "ao/Heap.hpp"

#include <bit>
#include <cstddef>
#include <cstring>

namespace ao {
namespace {

struct DiskHeader {
  char magic[4];
  std::uint16_t version;
  std::uint16_t pointerBits;
  std::uint16_t endian;
  std::uint16_t headerBytes;
  std::uint32_t heapBytes;
  std::uint32_t wellKnownCount;
  std::uint32_t extraCount;
  std::uint32_t globalCount;
  std::uint16_t nextHash;
  std::uint16_t reserved16;
  std::uint32_t reserved32a;
  std::uint32_t reserved32b;
  std::uint64_t reserved64;
};

static_assert(std::endian::native == std::endian::little);
static_assert(sizeof(DiskHeader) == ImageFormat::kImageHeaderBytes);
static_assert(offsetof(DiskHeader, version) == 4);
static_assert(offsetof(DiskHeader, pointerBits) == 6);
static_assert(offsetof(DiskHeader, endian) == 8);
static_assert(offsetof(DiskHeader, headerBytes) == 10);
static_assert(offsetof(DiskHeader, heapBytes) == 12);
static_assert(offsetof(DiskHeader, wellKnownCount) == 16);
static_assert(offsetof(DiskHeader, extraCount) == 20);
static_assert(offsetof(DiskHeader, globalCount) == 24);
static_assert(offsetof(DiskHeader, nextHash) == 28);
static_assert(offsetof(DiskHeader, reserved16) == 30);
static_assert(offsetof(DiskHeader, reserved32a) == 32);
static_assert(offsetof(DiskHeader, reserved32b) == 36);
static_assert(offsetof(DiskHeader, reserved64) == 40);
static_assert(sizeof(ObjectHeader) == ImageFormat::kImageFillerBytes);

}  // namespace

bool ImageFormat::encodeNonHeap(Oop o, std::uint64_t* bits) {
  if (bits == nullptr || o.isHeap()) {
    return false;
  }
  *bits = o.bits();
  return true;
}

bool ImageFormat::decodeNonHeap(std::uint64_t bits, Oop* o) {
  if (o == nullptr) {
    return false;
  }
  if (bits == 0) {
    *o = Oop{};
    return true;
  }
  if ((bits & Oop::kSmiTag) != 0) {
    *o = Oop::fromBits(bits);
    return true;
  }
  if ((bits & 0x3) == Oop::kCharTag) {
    *o = Oop::fromBits(bits);
    return true;
  }
  if ((bits & Oop::kLow3) == Oop::kImmTag) {
    const std::uint64_t kind = bits >> 3;
    if (kind > 2) {
      return false;
    }
    *o = Oop::fromBits(bits);
    return true;
  }
  return false;
}

void ImageFormat::writeHeader(std::byte* dst, const ImageHeader& h) {
  DiskHeader d{};
  d.magic[0] = 'A';
  d.magic[1] = 'O';
  d.magic[2] = 'I';
  d.magic[3] = 'M';
  d.version = h.version;
  d.pointerBits = h.pointerBits;
  d.endian = h.endian;
  d.headerBytes = kImageHeaderBytes;
  d.heapBytes = h.heapBytes;
  d.wellKnownCount = h.wellKnownCount;
  d.extraCount = h.extraCount;
  d.globalCount = h.globalCount;
  d.nextHash = h.nextHash;
  std::memcpy(dst, &d, sizeof(d));
}

bool ImageFormat::readHeader(const std::byte* src, std::size_t n, ImageHeader* out) {
  if (src == nullptr || out == nullptr || n < kImageHeaderBytes) {
    return false;
  }
  DiskHeader d{};
  std::memcpy(&d, src, sizeof(d));
  if (d.magic[0] != 'A' || d.magic[1] != 'O' || d.magic[2] != 'I' || d.magic[3] != 'M') {
    return false;
  }
  if (d.version != kImageVersion || d.pointerBits != kImagePointerBits ||
      d.endian != kImageEndianLittle || d.headerBytes != kImageHeaderBytes) {
    return false;
  }
  if (d.reserved16 != 0 || d.reserved32a != 0 || d.reserved32b != 0 || d.reserved64 != 0) {
    return false;
  }
  if (d.heapBytes < 16 || (d.heapBytes % 8) != 0) {
    return false;
  }
  out->version = d.version;
  out->pointerBits = d.pointerBits;
  out->endian = d.endian;
  out->heapBytes = d.heapBytes;
  out->wellKnownCount = d.wellKnownCount;
  out->extraCount = d.extraCount;
  out->globalCount = d.globalCount;
  out->nextHash = d.nextHash;
  return true;
}

void ImageFormat::writeFiller(std::byte* dst) {
  ObjectHeader h{};
  h.klass = Oop::nil();
  h.size = 0;
  h.flags = kFlagBytes;
  h.hash = 0;
  std::memcpy(dst, &h, kImageFillerBytes);
}

}  // namespace ao

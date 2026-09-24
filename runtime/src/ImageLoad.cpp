#include "ao/Image.hpp"

#include "ao/Globals.hpp"
#include "ao/ImageFormat.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/kernel/Install.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace ao {
namespace {

static_assert(std::endian::native == std::endian::little);

constexpr std::uint32_t kImageWellKnownCount = 127;

struct ImageRecord {
  std::string name;
  std::uint64_t bits = 0;
};

bool readFile(std::string_view path, std::vector<std::byte>* out) {
  std::ifstream in{std::string(path), std::ios::binary};
  if (!in) {
    return false;
  }
  in.seekg(0, std::ios::end);
  const auto end = in.tellg();
  if (end < 0) {
    return false;
  }
  in.seekg(0, std::ios::beg);
  out->resize(static_cast<std::size_t>(end));
  if (out->empty()) {
    return true;
  }
  in.read(reinterpret_cast<char*>(out->data()), static_cast<std::streamsize>(out->size()));
  return static_cast<std::size_t>(in.gcount()) == out->size();
}

template <typename T>
bool readPod(const std::vector<std::byte>& file, std::size_t* off, T* out) {
  static_assert(std::is_trivially_copyable_v<T>);
  if (*off > file.size() || sizeof(T) > file.size() - *off) {
    return false;
  }
  std::memcpy(out, file.data() + *off, sizeof(T));
  *off += sizeof(T);
  return true;
}

bool parseRecord(const std::vector<std::byte>& file, std::size_t* off, ImageRecord* out) {
  std::uint32_t nameLen = 0;
  if (!readPod(file, off, &nameLen) || nameLen < 1 || nameLen > 256) {
    return false;
  }
  if (*off > file.size() || nameLen > file.size() - *off) {
    return false;
  }
  out->name.assign(reinterpret_cast<const char*>(file.data() + *off), nameLen);
  *off += nameLen;
  const std::uint32_t pad = (4u - (nameLen % 4u)) % 4u;
  for (std::uint32_t i = 0; i < pad; ++i) {
    if (*off >= file.size() || file[*off] != std::byte{0}) {
      return false;
    }
    ++(*off);
  }
  return readPod(file, off, &out->bits);
}

bool parseRecords(const std::vector<std::byte>& file, std::size_t* off, std::uint32_t count,
                  std::vector<ImageRecord>* out) {
  if (count != 0) {
    if (*off > file.size() || count > (file.size() - *off) / 16u) {
      return false;
    }
  }
  out->clear();
  out->reserve(count);
  for (std::uint32_t i = 0; i < count; ++i) {
    ImageRecord rec;
    if (!parseRecord(file, off, &rec)) {
      return false;
    }
    out->push_back(std::move(rec));
  }
  return true;
}

bool heapShaped(std::uint64_t bits) { return bits != 0 && (bits & Oop::kLow3) == 0; }

bool acceptWord(std::uint64_t bits, const std::unordered_set<std::uint64_t>& starts) {
  if (heapShaped(bits)) {
    return (bits % 8u) == 0 && starts.find(bits) != starts.end();
  }
  Oop decoded;
  return ImageFormat::decodeNonHeap(bits, &decoded);
}

std::uint64_t readU64(const std::byte* p) {
  std::uint64_t v = 0;
  std::memcpy(&v, p, sizeof v);
  return v;
}

bool wellKnownNamesOk(const std::vector<ImageRecord>& recs) {
  if (recs.size() != kImageWellKnownCount) {
    return false;
  }
  Heap scratch(4096, 4096);
  Roots roots;
  WellKnown scratchWk(scratch, roots);
  std::unordered_set<std::string> seen;
  for (const ImageRecord& rec : recs) {
    if (!seen.insert(rec.name).second) {
      return false;
    }
    if (!scratchWk.bindImageSlot(rec.name, Oop::nil())) {
      return false;
    }
  }
  return true;
}

bool globalNamesOk(const std::vector<ImageRecord>& globals) {
  if (globals.size() != Globals::kSmalltalkCount) {
    return false;
  }
  for (std::uint32_t i = 0; i < globals.size(); ++i) {
    const char* name = Globals::nameAt(i);
    if (name == nullptr || globals[i].name != name) {
      return false;
    }
  }
  return true;
}

const ImageRecord* findRecord(const std::vector<ImageRecord>& recs, std::string_view name) {
  for (const ImageRecord& rec : recs) {
    if (rec.name == name) {
      return &rec;
    }
  }
  return nullptr;
}

bool recordsOk(const std::vector<ImageRecord>& recs, const std::unordered_set<std::uint64_t>& starts) {
  for (const ImageRecord& rec : recs) {
    if (!acceptWord(rec.bits, starts)) {
      return false;
    }
  }
  return true;
}

bool walkObjects(Heap& heap, const std::byte* section, std::size_t heapBytes,
                 std::vector<std::uint64_t>* offsets, std::unordered_set<std::uint64_t>* starts) {
  if (heapBytes < ImageFormat::kImageFillerBytes || section == nullptr) {
    return false;
  }
  ObjectHeader filler{};
  std::memcpy(&filler, section, sizeof filler);
  if (heap.objectBytes(&filler) != ImageFormat::kImageFillerBytes) {
    return false;
  }
  std::size_t off = ImageFormat::kImageFillerBytes;
  while (off < heapBytes) {
    if ((off % 8u) != 0 || sizeof(ObjectHeader) > heapBytes - off) {
      return false;
    }
    ObjectHeader hdr{};
    std::memcpy(&hdr, section + off, sizeof hdr);
    const std::size_t n = heap.objectBytes(&hdr);
    if (n < sizeof(ObjectHeader) || n > heapBytes - off) {
      return false;
    }
    const auto at = static_cast<std::uint64_t>(off);
    if (!starts->insert(at).second) {
      return false;
    }
    offsets->push_back(at);
    off += n;
  }
  return off == heapBytes;
}

bool payloadFits(std::size_t heapBytes, std::uint64_t off, std::uint32_t size) {
  if (off > heapBytes || sizeof(ObjectHeader) > heapBytes - static_cast<std::size_t>(off)) {
    return false;
  }
  return size <= heapBytes - static_cast<std::size_t>(off) - sizeof(ObjectHeader);
}

bool precheck(const std::byte* section, std::size_t heapBytes,
              const std::unordered_set<std::uint64_t>& starts, const std::vector<std::uint64_t>& offsets,
              const std::vector<ImageRecord>& wellKnown, const std::vector<ImageRecord>& globals) {
  if (!recordsOk(wellKnown, starts) || !recordsOk(globals, starts)) {
    return false;
  }
  const ImageRecord* symbolRec = findRecord(wellKnown, "Symbol");
  const ImageRecord* nativeRec = findRecord(wellKnown, "NativeMethod");
  if (symbolRec == nullptr || nativeRec == nullptr) {
    return false;
  }
  if (!heapShaped(symbolRec->bits) || !heapShaped(nativeRec->bits)) {
    return false;
  }
  std::unordered_set<std::string> symbols;
  for (std::uint64_t off : offsets) {
    ObjectHeader hdr{};
    std::memcpy(&hdr, section + static_cast<std::size_t>(off), sizeof hdr);
    const std::uint64_t klass = hdr.klass.bits();
    if (!acceptWord(klass, starts)) {
      return false;
    }
    const bool isBytes = (hdr.flags & kFlagBytes) != 0;
    if (!isBytes) {
      for (std::uint32_t i = 0; i < hdr.size; ++i) {
        const auto slotAt =
            off + sizeof(ObjectHeader) + static_cast<std::uint64_t>(i) * 8u;
        if (slotAt > heapBytes || heapBytes - slotAt < 8u) {
          return false;
        }
        if (!acceptWord(readU64(section + static_cast<std::size_t>(slotAt)), starts)) {
          return false;
        }
      }
    }
    if (klass == symbolRec->bits) {
      if (!isBytes || !payloadFits(heapBytes, off, hdr.size)) {
        return false;
      }
      std::string bytes(reinterpret_cast<const char*>(section + static_cast<std::size_t>(off) +
                                                      sizeof(ObjectHeader)),
                        hdr.size);
      if (!symbols.insert(std::move(bytes)).second) {
        return false;
      }
    }
    if (klass == nativeRec->bits) {
      if (isBytes || hdr.size < kNativeSlotCount) {
        return false;
      }
      const auto nameAt = off + sizeof(ObjectHeader) + static_cast<std::uint64_t>(kNativeSlotName) * 8u;
      if (nameAt > heapBytes || heapBytes - nameAt < 8u) {
        return false;
      }
      const std::uint64_t nameOff = readU64(section + static_cast<std::size_t>(nameAt));
      if (!heapShaped(nameOff) || starts.find(nameOff) == starts.end()) {
        return false;
      }
      ObjectHeader nameHdr{};
      std::memcpy(&nameHdr, section + static_cast<std::size_t>(nameOff), sizeof nameHdr);
      if ((nameHdr.flags & kFlagBytes) == 0 || nameHdr.klass.bits() != Oop::nil().bits() ||
          !payloadFits(heapBytes, nameOff, nameHdr.size)) {
        return false;
      }
      std::string name(reinterpret_cast<const char*>(section + static_cast<std::size_t>(nameOff) +
                                                     sizeof(ObjectHeader)),
                       nameHdr.size);
      std::uint32_t idx = 0;
      if (!NativeRegistry::findName(name, &idx)) {
        return false;
      }
    }
  }
  return true;
}

void relocateWord(std::byte* base, Oop* slot) {
  const std::uint64_t bits = slot->bits();
  if (heapShaped(bits)) {
    *slot = Oop::fromHeap(base + static_cast<std::size_t>(bits));
  } else {
    *slot = Oop::fromBits(bits);
  }
}

void relocate(Heap& heap, const std::vector<std::uint64_t>& offsets) {
  auto* base = const_cast<std::byte*>(heap.oldBase());
  auto* filler = reinterpret_cast<ObjectHeader*>(base);
  filler->flags = static_cast<std::uint16_t>(filler->flags | kFlagOld);
  for (std::uint64_t off : offsets) {
    auto* h = reinterpret_cast<ObjectHeader*>(base + static_cast<std::size_t>(off));
    const bool isBytes = (h->flags & kFlagBytes) != 0;
    relocateWord(base, &h->klass);
    if (!isBytes) {
      auto* slots = reinterpret_cast<Oop*>(h + 1);
      for (std::uint32_t i = 0; i < h->size; ++i) {
        relocateWord(base, &slots[i]);
      }
    }
    h->flags = static_cast<std::uint16_t>(h->flags | kFlagOld);
  }
}

Oop fileOop(std::byte* base, std::uint64_t bits) {
  if (heapShaped(bits)) {
    return Oop::fromHeap(base + static_cast<std::size_t>(bits));
  }
  return Oop::fromBits(bits);
}

bool bindAll(Heap& heap, WellKnown& wk, const std::vector<ImageRecord>& wellKnown,
             const std::vector<std::uint64_t>& offsets) {
  auto* base = const_cast<std::byte*>(heap.oldBase());
  for (const ImageRecord& rec : wellKnown) {
    if (!wk.bindImageSlot(rec.name, fileOop(base, rec.bits))) {
      return false;
    }
  }
  for (std::uint64_t off : offsets) {
    const Oop obj = Oop::fromHeap(base + static_cast<std::size_t>(off));
    if ((heap.flags(obj) & kFlagBytes) == 0 || heap.klass(obj) != wk.symbolClass) {
      continue;
    }
    if (!wk.rememberSymbol(obj)) {
      return false;
    }
  }
  // After the image's Symbols are remembered, so intern finds them instead of making copies.
  if (!wk.internSpecialSelectors()) {
    return false;
  }
  for (std::uint64_t off : offsets) {
    const Oop obj = Oop::fromHeap(base + static_cast<std::size_t>(off));
    if (heap.klass(obj) != wk.nativeMethodClass) {
      continue;
    }
    if ((heap.flags(obj) & kFlagBytes) != 0 || heap.size(obj) < kNativeSlotCount) {
      return false;
    }
    const Oop name = heap.slotAt(obj, kNativeSlotName);
    if (!name.isHeap() || (heap.flags(name) & kFlagBytes) == 0 || !heap.klass(name).isNil()) {
      return false;
    }
    const std::string_view bytes(reinterpret_cast<const char*>(heap.bytes(name)), heap.size(name));
    std::uint32_t idx = 0;
    if (!NativeRegistry::findName(bytes, &idx)) {
      return false;
    }
    heap.slotAtPut(obj, kNativeSlotRegistryIndex, Oop::fromSmallInteger(static_cast<std::int64_t>(idx)));
  }
  return true;
}

// SPEC §3.11: Smalltalk is the global dictionary (an image from before it is refused, not
// repaired), binds each of the 57 names to the value its record holds and binds itself. Runs
// after bindAll, so the names find the image's Symbols.
bool checkGlobals(Heap& heap, const WellKnown& wk, const std::vector<ImageRecord>& globals) {
  if (!Globals::isDictionary(wk, wk.smalltalk) || globals.size() != Globals::kSmalltalkCount ||
      Globals::lookup(wk, wk.findSymbol("Smalltalk")) != wk.smalltalk) {
    return false;
  }
  auto* base = const_cast<std::byte*>(heap.oldBase());
  for (std::uint32_t i = 0; i < globals.size(); ++i) {
    const char* name = Globals::nameAt(i);
    if (name == nullptr || globals[i].name != name) {
      return false;
    }
    const Oop got = fileOop(base, globals[i].bits);
    const Oop bound = Globals::lookup(wk, wk.findSymbol(name));
    if (bound.isEmpty() || got != bound) {
      return false;
    }
    if (globals[i].name == "Processor" && got != wk.processor) {
      return false;
    }
    if (globals[i].name == "Transcript" && (got != wk.transcriptClass || got == wk.transcript)) {
      return false;
    }
  }
  return true;
}

}  // namespace

bool Image::load(Heap& heap, Roots& roots, WellKnown& wk, std::string_view path,
                 std::string* reason) {
  (void)roots;
  // SPEC §3.11: every refusal says why. Past the header, a file that does not hold together is a
  // damaged image.
  auto refuse = [reason](std::string why) {
    if (reason != nullptr) {
      *reason = std::move(why);
    }
    return false;
  };
  const std::string damaged = "damaged image";
  if (&wk.heap() != &heap) {
    return refuse("image load failed");
  }

  std::vector<std::byte> file;
  if (!readFile(path, &file)) {
    return refuse("cannot read image file");
  }
  ImageFormat::ImageHeader header;
  if (!ImageFormat::readHeader(file.data(), file.size(), &header, reason)) {
    return false;
  }
  // SPEC §3.11: a global lives in the dictionary in the heap. Extra records are no part of this
  // version.
  if (header.globalCount != Globals::kSmalltalkCount || header.wellKnownCount != kImageWellKnownCount ||
      header.extraCount != 0) {
    return refuse(damaged);
  }
  if (header.heapBytes > file.size() - ImageFormat::kImageHeaderBytes) {
    return refuse(damaged);
  }
  const std::size_t heapBytes = header.heapBytes;
  const std::byte* section = file.data() + ImageFormat::kImageHeaderBytes;

  std::size_t cursor = ImageFormat::kImageHeaderBytes + heapBytes;
  std::vector<ImageRecord> wellKnown;
  std::vector<ImageRecord> globals;
  if (!parseRecords(file, &cursor, header.wellKnownCount, &wellKnown) ||
      !parseRecords(file, &cursor, header.globalCount, &globals) || cursor != file.size()) {
    return refuse(damaged);
  }
  if (!wellKnownNamesOk(wellKnown) || !globalNamesOk(globals)) {
    return refuse(damaged);
  }

  kernel::ensureNativeNames();

  std::vector<std::uint64_t> offsets;
  std::unordered_set<std::uint64_t> starts;
  if (!walkObjects(heap, section, heapBytes, &offsets, &starts) ||
      !precheck(section, heapBytes, starts, offsets, wellKnown, globals)) {
    return refuse(damaged);
  }
  if (heapBytes > heap.oldMaxBytes()) {
    return refuse("image heap exceeds the old space limit");
  }
  if (heap.oldUsed() != 0 || !heap.adoptOldBytes(section, heapBytes, header.nextHash)) {
    return refuse("image load failed");
  }
  relocate(heap, offsets);
  if (!bindAll(heap, wk, wellKnown, offsets)) {
    return refuse(damaged);
  }
  if (!checkGlobals(heap, wk, globals)) {
    return refuse(damaged);
  }
  // SPEC §3.5: an old image may hide one of the eight SmallInteger natives.
  wk.checkSmallIntegerFastPath();
  return true;
}

}  // namespace ao

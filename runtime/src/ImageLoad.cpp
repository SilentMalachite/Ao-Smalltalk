#include "ao/Image.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Globals.hpp"
#include "ao/ImageFormat.hpp"
#include "ao/MethodDictionary.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/kernel/Install.hpp"

#include <algorithm>
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
// The flags an object in an image may have (the ones Image::save writes).
constexpr std::uint16_t kImageFlags = static_cast<std::uint16_t>(kFlagBytes | kFlagWeak);
// SPEC §3.11: a record is a u32 name length, 1 to 256 name bytes padded to 4, and the u64 bits.
constexpr std::uint64_t kMinRecordBytes = 4 + 4 + 8;
constexpr std::uint64_t kMaxRecordBytes = 4 + 256 + 8;

struct ImageRecord {
  std::string name;
  std::uint64_t bits = 0;
};

bool readExactly(std::ifstream& in, std::byte* dst, std::size_t n) {
  if (n == 0) {
    return true;
  }
  in.read(reinterpret_cast<char*>(dst), static_cast<std::streamsize>(n));
  return static_cast<std::size_t>(in.gcount()) == n;
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
  // SPEC §3.11: the filler is what writeFiller writes, and an object has no flags but Bytes and
  // Weak (the save writes only those; a Marked or Forwarded one would break the GC).
  ObjectHeader filler{};
  std::memcpy(&filler, section, sizeof filler);
  if (heap.objectBytes(&filler) != ImageFormat::kImageFillerBytes ||
      filler.klass.bits() != Oop::nil().bits() || filler.size != 0 || filler.flags != kFlagBytes) {
    return false;
  }
  std::size_t off = ImageFormat::kImageFillerBytes;
  while (off < heapBytes) {
    if ((off % 8u) != 0 || sizeof(ObjectHeader) > heapBytes - off) {
      return false;
    }
    ObjectHeader hdr{};
    std::memcpy(&hdr, section + off, sizeof hdr);
    if ((hdr.flags & ~kImageFlags) != 0) {
      return false;
    }
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

ObjectHeader headerAt(const std::byte* section, std::uint64_t off) {
  ObjectHeader h{};
  std::memcpy(&h, section + static_cast<std::size_t>(off), sizeof h);
  return h;
}

// Slot i of the pointer object at off; walkObjects has checked that its slots lie in the section.
std::uint64_t slotBits(const std::byte* section, std::uint64_t off, std::uint32_t i) {
  return readU64(section + static_cast<std::size_t>(off) + sizeof(ObjectHeader) +
                 static_cast<std::size_t>(i) * 8u);
}

bool pointerObject(const ObjectHeader& h) { return (h.flags & kFlagBytes) == 0; }

// Lookup's isClassShaped on the file: an object of the image, of pointers, with every class slot.
bool classShaped(const std::byte* section, const std::unordered_set<std::uint64_t>& starts,
                 std::uint64_t bits) {
  if (!heapShaped(bits) || starts.find(bits) == starts.end()) {
    return false;
  }
  const ObjectHeader h = headerAt(section, bits);
  return pointerObject(h) && h.size >= kClassSlotCount;
}

enum class RecordKind { Class, Selector, Other };

// wellKnownNamesOk has checked that every name binds a slot: a class or metaclass of the catalog,
// Smalltalk, Processor, transcript, or else a selector.
RecordKind recordKind(const WellKnown& wk, std::string_view name) {
  if (name == "Smalltalk" || name == "Processor" || name == "transcript") {
    return RecordKind::Other;
  }
  constexpr std::string_view kMeta = " class";
  if (wk.isCatalogName(name) ||
      (name.ends_with(kMeta) && wk.isCatalogName(name.substr(0, name.size() - kMeta.size())))) {
    return RecordKind::Class;
  }
  return RecordKind::Selector;
}

// SPEC §3.11: the shapes the runtime reads without checking. Every klass is nil (an internal object:
// a method dictionary's array, a NativeMethod's name) or a class. The well-known classes and
// metaclasses are classes, its selectors Symbols. Each class met as a klass or a well-known record,
// and each class-shaped superclass up from them, has a nil or MethodDictionary methodDict. A
// MethodDictionary has its array slot, nil or of pointers; a CompiledMethod has all of its slots.
// Runs after precheck, which has checked that every heap-shaped slot and record is an object start.
bool shapesOk(const std::byte* section, const std::unordered_set<std::uint64_t>& starts,
              const std::vector<std::uint64_t>& offsets, const std::vector<ImageRecord>& wellKnown,
              const WellKnown& wk) {
  const ImageRecord* symbolRec = findRecord(wellKnown, "Symbol");
  const ImageRecord* dictRec = findRecord(wellKnown, "MethodDictionary");
  const ImageRecord* methodRec = findRecord(wellKnown, "CompiledMethod");
  if (symbolRec == nullptr || dictRec == nullptr || methodRec == nullptr) {
    return false;
  }
  const std::uint64_t nilBits = Oop::nil().bits();
  std::unordered_set<std::uint64_t> classes;
  std::vector<std::uint64_t> work;
  auto addClass = [&](std::uint64_t bits) {
    if (!classShaped(section, starts, bits)) {
      return false;
    }
    if (classes.insert(bits).second) {
      work.push_back(bits);
    }
    return true;
  };
  for (std::uint64_t off : offsets) {
    const ObjectHeader h = headerAt(section, off);
    const std::uint64_t klass = h.klass.bits();
    if (klass != nilBits && !addClass(klass)) {
      return false;
    }
    if (klass == dictRec->bits) {
      if (!pointerObject(h) || h.size <= kDictSlotArray) {
        return false;
      }
      const std::uint64_t array = slotBits(section, off, kDictSlotArray);
      if (array != nilBits && (!heapShaped(array) || !pointerObject(headerAt(section, array)))) {
        return false;
      }
    }
    if (klass == methodRec->bits && (!pointerObject(h) || h.size < kCmSlotCount)) {
      return false;
    }
  }
  for (const ImageRecord& rec : wellKnown) {
    const RecordKind kind = recordKind(wk, rec.name);
    if (kind == RecordKind::Class && !addClass(rec.bits)) {
      return false;
    }
    if (kind == RecordKind::Selector &&
        (!heapShaped(rec.bits) || headerAt(section, rec.bits).klass.bits() != symbolRec->bits)) {
      return false;
    }
  }
  while (!work.empty()) {
    const std::uint64_t cls = work.back();
    work.pop_back();
    // A superclass of another shape ends the chain (SPEC §3.3); lookup does not read it.
    const std::uint64_t superclass = slotBits(section, cls, kClassSlotSuperclass);
    if (classShaped(section, starts, superclass)) {
      addClass(superclass);
    }
    const std::uint64_t dict = slotBits(section, cls, kClassSlotMethodDict);
    if (dict != nilBits &&
        (!heapShaped(dict) || headerAt(section, dict).klass.bits() != dictRec->bits)) {
      return false;
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

  // SPEC §3.11: the 48-byte header first. The body is read only when the header's heapBytes is
  // within the old space limit and the file is as long as the header and the records can make it.
  std::ifstream in{std::string(path), std::ios::binary};
  if (!in) {
    return refuse("cannot read image file");
  }
  in.seekg(0, std::ios::end);
  const auto end = in.tellg();
  if (end < 0) {
    return refuse("cannot read image file");
  }
  const auto fileSize = static_cast<std::uint64_t>(end);
  in.seekg(0, std::ios::beg);
  std::byte head[ImageFormat::kImageHeaderBytes]{};
  const auto headBytes =
      static_cast<std::size_t>(std::min<std::uint64_t>(fileSize, ImageFormat::kImageHeaderBytes));
  if (!readExactly(in, head, headBytes)) {
    return refuse("cannot read image file");
  }
  ImageFormat::ImageHeader header;
  if (!ImageFormat::readHeader(head, headBytes, &header, reason)) {
    return false;
  }
  // SPEC §3.11: a global lives in the dictionary in the heap. Extra records are no part of this
  // version.
  if (header.globalCount != Globals::kSmalltalkCount || header.wellKnownCount != kImageWellKnownCount ||
      header.extraCount != 0) {
    return refuse(damaged);
  }
  if (header.heapBytes > heap.oldMaxBytes()) {
    return refuse("image heap exceeds the old space limit");
  }
  const std::uint64_t records = std::uint64_t{header.wellKnownCount} + header.globalCount;
  const std::uint64_t beforeRecords = ImageFormat::kImageHeaderBytes + std::uint64_t{header.heapBytes};
  if (fileSize < beforeRecords + records * kMinRecordBytes ||
      fileSize > beforeRecords + records * kMaxRecordBytes) {
    return refuse(damaged);
  }
  std::vector<std::byte> file(static_cast<std::size_t>(fileSize));
  std::memcpy(file.data(), head, sizeof head);
  if (!readExactly(in, file.data() + sizeof head, file.size() - sizeof head)) {
    return refuse("cannot read image file");
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
      !precheck(section, heapBytes, starts, offsets, wellKnown, globals) ||
      !shapesOk(section, starts, offsets, wellKnown, wk)) {
    return refuse(damaged);
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

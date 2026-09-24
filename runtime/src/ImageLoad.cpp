#include "ao/Image.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/CompiledMethod.hpp"
#include "ao/Format.hpp"
#include "ao/Globals.hpp"
#include "ao/ImageFormat.hpp"
#include "ao/Lookup.hpp"
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
#include <unordered_map>
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

// Records why a check failed, for the save's reason (SPEC §3.11). Always false.
bool fail(std::string* detail, std::string text) {
  if (detail != nullptr) {
    *detail = std::move(text);
  }
  return false;
}

std::string atOffset(std::uint64_t off) { return "the object at offset " + std::to_string(off); }

bool walkObjects(const Heap& heap, const std::byte* section, std::size_t heapBytes,
                 std::vector<std::uint64_t>* offsets, std::unordered_set<std::uint64_t>* starts,
                 std::string* detail) {
  if (heapBytes < ImageFormat::kImageFillerBytes || section == nullptr) {
    return fail(detail, "the heap is shorter than its filler");
  }
  // SPEC §3.11: the filler is what writeFiller writes, and an object has no flags but Bytes and
  // Weak (the save writes only those; a Marked or Forwarded one would break the GC).
  ObjectHeader filler{};
  std::memcpy(&filler, section, sizeof filler);
  if (heap.objectBytes(&filler) != ImageFormat::kImageFillerBytes ||
      filler.klass.bits() != Oop::nil().bits() || filler.size != 0 || filler.flags != kFlagBytes) {
    return fail(detail, "the heap does not start with the filler");
  }
  std::size_t off = ImageFormat::kImageFillerBytes;
  while (off < heapBytes) {
    if ((off % 8u) != 0 || sizeof(ObjectHeader) > heapBytes - off) {
      return fail(detail, atOffset(off) + " runs past the heap");
    }
    ObjectHeader hdr{};
    std::memcpy(&hdr, section + off, sizeof hdr);
    if ((hdr.flags & ~kImageFlags) != 0) {
      return fail(detail, atOffset(off) + " has flags other than Bytes and Weak");
    }
    const std::size_t n = heap.objectBytes(&hdr);
    if (n < sizeof(ObjectHeader) || n > heapBytes - off) {
      return fail(detail, atOffset(off) + " runs past the heap");
    }
    const auto at = static_cast<std::uint64_t>(off);
    if (!starts->insert(at).second) {
      return fail(detail, atOffset(off) + " is there twice");
    }
    offsets->push_back(at);
    off += n;
  }
  return off == heapBytes || fail(detail, "the last object runs past the heap");
}

bool payloadFits(std::size_t heapBytes, std::uint64_t off, std::uint32_t size) {
  if (off > heapBytes || sizeof(ObjectHeader) > heapBytes - static_cast<std::size_t>(off)) {
    return false;
  }
  return size <= heapBytes - static_cast<std::size_t>(off) - sizeof(ObjectHeader);
}

bool precheck(const std::byte* section, std::size_t heapBytes,
              const std::unordered_set<std::uint64_t>& starts, const std::vector<std::uint64_t>& offsets,
              const std::vector<ImageRecord>& wellKnown, const std::vector<ImageRecord>& globals,
              std::string* detail) {
  if (!recordsOk(wellKnown, starts) || !recordsOk(globals, starts)) {
    return fail(detail, "a record holds a word that is neither an object nor an immediate");
  }
  const ImageRecord* symbolRec = findRecord(wellKnown, "Symbol");
  const ImageRecord* nativeRec = findRecord(wellKnown, "NativeMethod");
  if (symbolRec == nullptr || nativeRec == nullptr || !heapShaped(symbolRec->bits) ||
      !heapShaped(nativeRec->bits)) {
    return fail(detail, "the well-known Symbol or NativeMethod is not an object");
  }
  std::unordered_set<std::string> symbols;
  for (std::uint64_t off : offsets) {
    ObjectHeader hdr{};
    std::memcpy(&hdr, section + static_cast<std::size_t>(off), sizeof hdr);
    const std::uint64_t klass = hdr.klass.bits();
    if (!acceptWord(klass, starts)) {
      return fail(detail, "the class word of " + atOffset(off) +
                              " is neither an object nor an immediate");
    }
    const bool isBytes = (hdr.flags & kFlagBytes) != 0;
    if (!isBytes) {
      for (std::uint32_t i = 0; i < hdr.size; ++i) {
        const auto slotAt =
            off + sizeof(ObjectHeader) + static_cast<std::uint64_t>(i) * 8u;
        if (slotAt > heapBytes || heapBytes - slotAt < 8u) {
          return fail(detail, atOffset(off) + " runs past the heap");
        }
        if (!acceptWord(readU64(section + static_cast<std::size_t>(slotAt)), starts)) {
          return fail(detail, "slot " + std::to_string(i + 1) + " of " + atOffset(off) +
                                  " is neither an object nor an immediate");
        }
      }
    }
    if (klass == symbolRec->bits) {
      if (!isBytes || !payloadFits(heapBytes, off, hdr.size)) {
        return fail(detail, "a Symbol (" + atOffset(off) + ") is not bytes");
      }
      std::string bytes(reinterpret_cast<const char*>(section + static_cast<std::size_t>(off) +
                                                      sizeof(ObjectHeader)),
                        hdr.size);
      if (!symbols.insert(bytes).second) {
        return fail(detail, "Symbol #" + bytes + " is there twice");
      }
    }
    if (klass == nativeRec->bits) {
      if (isBytes || hdr.size < kNativeSlotCount) {
        return fail(detail, "a NativeMethod (" + atOffset(off) + ") has fewer than " +
                                std::to_string(kNativeSlotCount) + " slots");
      }
      const auto nameAt =
          off + sizeof(ObjectHeader) + static_cast<std::uint64_t>(kNativeSlotName) * 8u;
      const std::uint64_t nameOff = readU64(section + static_cast<std::size_t>(nameAt));
      if (!heapShaped(nameOff) || starts.find(nameOff) == starts.end()) {
        return fail(detail, "a NativeMethod (" + atOffset(off) + ") has no name");
      }
      ObjectHeader nameHdr{};
      std::memcpy(&nameHdr, section + static_cast<std::size_t>(nameOff), sizeof nameHdr);
      if ((nameHdr.flags & kFlagBytes) == 0 || nameHdr.klass.bits() != Oop::nil().bits() ||
          !payloadFits(heapBytes, nameOff, nameHdr.size)) {
        return fail(detail, "a NativeMethod (" + atOffset(off) + ") has no name");
      }
      std::string name(reinterpret_cast<const char*>(section + static_cast<std::size_t>(nameOff) +
                                                     sizeof(ObjectHeader)),
                       nameHdr.size);
      std::uint32_t idx = 0;
      if (!NativeRegistry::findName(name, &idx)) {
        return fail(detail, "NativeMethod " + name + " has no registered function");
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

// SPEC §3.11: the shapes the runtime reads without checking, for every object of the heap. Runs
// after precheck, which has checked that every heap-shaped slot and record is an object start.
class ObjectRules {
 public:
  ObjectRules(const std::byte* section, const std::unordered_set<std::uint64_t>& starts)
      : section_(section), starts_(starts) {}

  bool check(const std::vector<std::uint64_t>& offsets, const std::vector<ImageRecord>& wellKnown,
             const WellKnown& wk, std::string* detail) {
    const ImageRecord* behaviorRec = findRecord(wellKnown, "Behavior");
    const ImageRecord* symbolRec = findRecord(wellKnown, "Symbol");
    const ImageRecord* dictRec = findRecord(wellKnown, "MethodDictionary");
    const ImageRecord* methodRec = findRecord(wellKnown, "CompiledMethod");
    if (behaviorRec == nullptr || symbolRec == nullptr || dictRec == nullptr ||
        methodRec == nullptr || !classShaped(behaviorRec->bits)) {
      return fail(detail, "the well-known Behavior, Symbol, MethodDictionary or CompiledMethod "
                          "is missing");
    }
    behavior_ = behaviorRec->bits;
    dict_ = dictRec->bits;
    for (const ImageRecord& rec : wellKnown) {
      const RecordKind kind = recordKind(wk, rec.name);
      if (kind == RecordKind::Class && !behaviorObject(rec.bits)) {
        return fail(detail, "the well-known " + rec.name + " is not a class");
      }
      if (kind == RecordKind::Selector &&
          (!heapShaped(rec.bits) || headerAt(section_, rec.bits).klass.bits() != symbolRec->bits)) {
        return fail(detail, "the well-known selector " + rec.name + " is not a Symbol");
      }
    }
    for (std::uint64_t off : offsets) {
      if (!objectOk(off, methodRec->bits, detail)) {
        return false;
      }
    }
    return true;
  }

 private:
  // Lookup's isClassShaped on the file: an object of the image, of pointers, with every class slot.
  bool classShaped(std::uint64_t bits) const {
    if (!heapShaped(bits) || starts_.find(bits) == starts_.end()) {
      return false;
    }
    const ObjectHeader h = headerAt(section_, bits);
    return pointerObject(h) && h.size >= kClassSlotCount;
  }

  // Whether the superclass chain from cls reaches Behavior, walked like lookup (SPEC §3.3): class-
  // shaped links only, kMaxSuperclassDepth of them, so a cycle ends there.
  bool inheritsBehavior(std::uint64_t cls) {
    const auto known = inherits_.find(cls);
    if (known != inherits_.end()) {
      return known->second;
    }
    bool found = false;
    std::uint64_t c = cls;
    for (std::uint32_t depth = 0; depth < kMaxSuperclassDepth && classShaped(c); ++depth) {
      if (c == behavior_) {
        found = true;
        break;
      }
      c = slotBits(section_, c, kClassSlotSuperclass);
    }
    inherits_.emplace(cls, found);
    return found;
  }

  // A class, a metaclass or an anonymous Behavior: its class inherits from Behavior.
  bool behaviorObject(std::uint64_t bits) {
    if (!heapShaped(bits) || starts_.find(bits) == starts_.end()) {
      return false;
    }
    const std::uint64_t klass = headerAt(section_, bits).klass.bits();
    return klass != Oop::nil().bits() && inheritsBehavior(klass);
  }

  // The bytes of the name slot of the class-shaped object at off, for a reason.
  std::string nameOf(std::uint64_t off) const {
    if (!classShaped(off)) {
      return "an object that is no class";
    }
    const std::uint64_t name = slotBits(section_, off, kClassSlotName);
    if (!heapShaped(name) || starts_.find(name) == starts_.end()) {
      return "an unnamed class";
    }
    const ObjectHeader h = headerAt(section_, name);
    if (pointerObject(h)) {
      return "an unnamed class";
    }
    const auto* bytes = reinterpret_cast<const char*>(section_ + static_cast<std::size_t>(name) +
                                                      sizeof(ObjectHeader));
    return std::string(bytes, std::min<std::size_t>(h.size, 128));
  }

  bool objectOk(std::uint64_t off, std::uint64_t methodClass, std::string* detail) {
    const std::uint64_t nilBits = Oop::nil().bits();
    const ObjectHeader h = headerAt(section_, off);
    const std::uint64_t klass = h.klass.bits();
    // An internal object (a method dictionary's array, a NativeMethod's name, a test's weak
    // array) has no class and no format to fit.
    if (klass == nilBits) {
      return true;
    }
    if (!classShaped(klass) || !behaviorObject(klass)) {
      return fail(detail, "the class of " + atOffset(off) + " is not a class");
    }
    const std::string cls = nameOf(klass);
    // Behavior objects, with or without instances.
    if (inheritsBehavior(klass)) {
      const std::string self = nameOf(off);
      if (!pointerObject(h) || h.size < kClassSlotCount) {
        return fail(detail, "class " + self + " has fewer than " +
                                std::to_string(kClassSlotCount) + " slots");
      }
      const std::uint64_t dict = slotBits(section_, off, kClassSlotMethodDict);
      if (dict != nilBits && (!heapShaped(dict) || headerAt(section_, dict).klass.bits() != dict_)) {
        return fail(detail, "the methodDict of " + self + " is not a MethodDictionary");
      }
      const std::uint64_t superclass = slotBits(section_, off, kClassSlotSuperclass);
      if (superclass != nilBits && !behaviorObject(superclass)) {
        return fail(detail, "the superclass of " + self + " is not a class");
      }
    }
    // The class format, read as the runtime reads it (Format::bits: a format that is no
    // SmallInteger is 0).
    const Oop format = Oop::fromBits(slotBits(section_, klass, kClassSlotFormat));
    const bool bytes = (h.flags & kFlagBytes) != 0;
    if ((h.flags & kFlagWeak) != 0) {
      return fail(detail, "an instance of " + cls + " is weak; no class has weak instances");
    }
    if (bytes != Format::isBytes(format)) {
      return fail(detail, "an instance of " + cls + (bytes ? " is bytes; " : " is pointers; ") +
                              "its class format says " + (bytes ? "pointers" : "bytes"));
    }
    if (!bytes) {
      const auto inst = static_cast<std::uint64_t>(Format::instSize(format));
      if (h.size < inst || (!Format::isIndexable(format) && h.size != inst)) {
        return fail(detail, "an instance of " + cls + " has " + std::to_string(h.size) +
                                " slots; its class format wants " + std::to_string(inst) +
                                (Format::isIndexable(format) ? " or more" : ""));
      }
    }
    if (klass == dict_) {
      if (!pointerObject(h) || h.size <= kDictSlotArray) {
        return fail(detail, "a MethodDictionary has fewer than 2 slots");
      }
      const std::uint64_t array = slotBits(section_, off, kDictSlotArray);
      if (array != nilBits && (!heapShaped(array) || !pointerObject(headerAt(section_, array)))) {
        return fail(detail, "the array of a MethodDictionary is not an object of pointers");
      }
    }
    if (klass == methodClass && (!pointerObject(h) || h.size < kCmSlotCount)) {
      return fail(detail, "a CompiledMethod has fewer than " + std::to_string(kCmSlotCount) +
                              " slots");
    }
    return true;
  }

  const std::byte* section_;
  const std::unordered_set<std::uint64_t>& starts_;
  std::uint64_t behavior_ = 0;
  std::uint64_t dict_ = 0;
  std::unordered_map<std::uint64_t, bool> inherits_;
};

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

// What checkFile read out of an image file; section points into the file's bytes.
struct Parsed {
  ImageFormat::ImageHeader header;
  std::size_t heapBytes = 0;
  const std::byte* section = nullptr;
  std::vector<ImageRecord> wellKnown;
  std::vector<ImageRecord> globals;
  std::vector<std::uint64_t> offsets;
  std::unordered_set<std::uint64_t> starts;
};

// SPEC §3.11: what the 48-byte header and the file's size can tell, before the body is read.
bool headerOk(const ImageFormat::ImageHeader& header, std::uint64_t fileSize,
              std::size_t oldMaxBytes, std::string* reason, std::string* detail) {
  auto refuse = [&](const char* why, std::string what) {
    if (reason != nullptr) {
      *reason = why;
    }
    return fail(detail, std::move(what));
  };
  // A global lives in the dictionary in the heap. Extra records are no part of this version.
  if (header.globalCount != Globals::kSmalltalkCount || header.wellKnownCount != kImageWellKnownCount ||
      header.extraCount != 0) {
    return refuse("damaged image", "the header does not count 127 well-known and 57 global records");
  }
  if (header.heapBytes > oldMaxBytes) {
    return refuse("image heap exceeds the old space limit",
                  "the heap is " + std::to_string(header.heapBytes) +
                      " bytes; the old space limit is " + std::to_string(oldMaxBytes));
  }
  const std::uint64_t records = std::uint64_t{header.wellKnownCount} + header.globalCount;
  const std::uint64_t beforeRecords =
      ImageFormat::kImageHeaderBytes + std::uint64_t{header.heapBytes};
  if (fileSize < beforeRecords + records * kMinRecordBytes ||
      fileSize > beforeRecords + records * kMaxRecordBytes) {
    return refuse("damaged image", "the file is " + std::to_string(fileSize) +
                                       " bytes, which its header and records cannot make");
  }
  return true;
}

// SPEC §3.11: every check a load makes before it adopts the heap, on the bytes of an image file.
// Image::save runs it on the bytes it is about to write. *reason gets the refusal of the §3.11
// table, *detail which object broke which rule.
bool checkFile(const Heap& heap, const WellKnown& wk, const std::vector<std::byte>& file,
               std::size_t oldMaxBytes, Parsed* p, std::string* reason, std::string* detail) {
  auto damaged = [reason] {
    if (reason != nullptr) {
      *reason = "damaged image";
    }
    return false;
  };
  if (!ImageFormat::readHeader(file.data(), file.size(), &p->header, reason)) {
    return fail(detail, reason != nullptr ? *reason : std::string("not an Ao image"));
  }
  if (!headerOk(p->header, file.size(), oldMaxBytes, reason, detail)) {
    return false;
  }
  p->heapBytes = p->header.heapBytes;
  p->section = file.data() + ImageFormat::kImageHeaderBytes;
  std::size_t cursor = ImageFormat::kImageHeaderBytes + p->heapBytes;
  if (!parseRecords(file, &cursor, p->header.wellKnownCount, &p->wellKnown) ||
      !parseRecords(file, &cursor, p->header.globalCount, &p->globals) || cursor != file.size()) {
    fail(detail, "the records after the heap do not parse");
    return damaged();
  }
  if (!wellKnownNamesOk(p->wellKnown) || !globalNamesOk(p->globals)) {
    fail(detail, "the record names are not the ones this runtime binds");
    return damaged();
  }
  kernel::ensureNativeNames();
  if (!walkObjects(heap, p->section, p->heapBytes, &p->offsets, &p->starts, detail) ||
      !precheck(p->section, p->heapBytes, p->starts, p->offsets, p->wellKnown, p->globals,
                detail) ||
      !ObjectRules(p->section, p->starts).check(p->offsets, p->wellKnown, wk, detail)) {
    return damaged();
  }
  return true;
}

}  // namespace

bool Image::check(const Heap& heap, const WellKnown& wk, const std::vector<std::byte>& file,
                  std::size_t oldMaxBytes, std::string* reason, std::string* detail) {
  Parsed parsed;
  return checkFile(heap, wk, file, oldMaxBytes, &parsed, reason, detail);
}

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
  if (!ImageFormat::readHeader(head, headBytes, &header, reason) ||
      !headerOk(header, fileSize, heap.oldMaxBytes(), reason, nullptr)) {
    return false;
  }
  std::vector<std::byte> file(static_cast<std::size_t>(fileSize));
  std::memcpy(file.data(), head, sizeof head);
  if (!readExactly(in, file.data() + sizeof head, file.size() - sizeof head)) {
    return refuse("cannot read image file");
  }
  // SPEC §3.11: the same checks the save makes before it writes.
  Parsed parsed;
  if (!checkFile(heap, wk, file, heap.oldMaxBytes(), &parsed, reason, nullptr)) {
    return false;
  }
  if (heap.oldUsed() != 0 ||
      !heap.adoptOldBytes(parsed.section, parsed.heapBytes, parsed.header.nextHash)) {
    return refuse("image load failed");
  }
  relocate(heap, parsed.offsets);
  if (!bindAll(heap, wk, parsed.wellKnown, parsed.offsets)) {
    return refuse(damaged);
  }
  if (!checkGlobals(heap, wk, parsed.globals)) {
    return refuse(damaged);
  }
  // SPEC §3.5: an old image may hide one of the eight SmallInteger natives.
  wk.checkSmallIntegerFastPath();
  return true;
}

}  // namespace ao

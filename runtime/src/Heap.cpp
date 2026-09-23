#include "ao/Heap.hpp"

#include "VirtualRegion.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <system_error>
#include <utility>

namespace ao {

static std::size_t align8(std::size_t n) { return (n + 7u) & ~std::size_t{7}; }

// AO_GC_STRESS が正の十進整数ならその値。未設定・空・0・不正な文字列は 0（無効）。
static std::uint32_t gcStressFromEnv() {
  const char* v = std::getenv("AO_GC_STRESS");
  if (v == nullptr) {
    return 0;
  }
  const char* end = v + std::strlen(v);
  std::uint32_t n = 0;
  const auto [p, ec] = std::from_chars(v, end, n);
  if (ec != std::errc{} || p != end) {
    return 0;
  }
  return n;
}

Heap::Heap(std::size_t nurseryBytes, std::size_t oldBytes, std::size_t oldMaxBytes)
    : nursery_(std::make_unique<std::byte[]>(2 * nurseryBytes)),
      old_(std::make_unique<VirtualRegion>()),
      oldInitial_(std::min(oldBytes, oldMaxBytes)),
      oldMax_(oldMaxBytes),
      nurseryHalf_(nurseryBytes) {
  fromStart_ = nursery_.get();
  fromEnd_ = fromStart_ + nurseryBytes;
  fromBump_ = fromStart_;
  toStart_ = fromEnd_;
  toEnd_ = toStart_ + nurseryBytes;
  toBump_ = toStart_;
  // 予約かコミットに失敗したら old は空のまま（上限 0）にする。old への割り当ては失敗する。
  // 黙って劣化させず、stderr に 1 行出す。
  const bool reserved = old_->reserve(oldMax_, kOldCommitUnit);
  if (!reserved || !old_->commit(oldInitial_)) {
    std::fprintf(stderr,
                 "ao: heap: could not %s %zu bytes of old space; old allocations will fail\n",
                 reserved ? "commit" : "reserve", reserved ? oldInitial_ : oldMax_);
    old_ = std::make_unique<VirtualRegion>();
    oldInitial_ = 0;
    oldMax_ = 0;
  }
  oldStart_ = old_->base();
  oldEnd_ = oldStart_ + oldInitial_;
  oldBump_ = oldStart_;
  oldThreshold_ = oldInitial_;
  gcStress_ = gcStressFromEnv();
}

Heap::~Heap() = default;
Heap::Heap(Heap&&) noexcept = default;
Heap& Heap::operator=(Heap&&) noexcept = default;

std::size_t Heap::objectBytes(const ObjectHeader* h) const {
  std::size_t payload = (h->flags & kFlagBytes) ? h->size
                                                : static_cast<std::size_t>(h->size) * 8u;
  return align8(sizeof(ObjectHeader) + payload);
}

std::size_t Heap::objectBytesFor(std::uint32_t size, std::uint16_t flags) const {
  ObjectHeader probe{};
  probe.size = size;
  probe.flags = flags;
  return objectBytes(&probe);
}

Oop Heap::allocate(Oop cls, std::uint32_t size, std::uint16_t flags) {
  ObjectHeader probe{};
  probe.size = size;
  probe.flags = flags;
  const std::size_t n = objectBytes(&probe);
  if (n >= largeObjectBytes()) {
    return allocateTenured(cls, size, flags);
  }
  if (n > static_cast<std::size_t>(fromEnd_ - fromBump_)) {
    return Oop{};
  }
  std::byte* at = fromBump_;
  fromBump_ += n;
  return initObject(at, cls, size, static_cast<std::uint16_t>(flags & ~kFlagOld), n);
}

Oop Heap::allocateTenured(Oop cls, std::uint32_t size, std::uint16_t flags) {
  ObjectHeader probe{};
  probe.size = size;
  probe.flags = flags;
  const std::size_t n = objectBytes(&probe);
  std::byte* at = reserveOld(n);
  if (at == nullptr) {
    return Oop{};
  }
  return initObject(at, cls, size, static_cast<std::uint16_t>(flags | kFlagOld), n);
}

Oop Heap::allocateNoGc(Oop cls, std::uint32_t size, std::uint16_t flags) {
  const Oop obj = allocate(cls, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  return allocateTenured(cls, size, flags);
}

std::size_t Heap::largeObjectBytes() const { return std::min(kLargeObjectBytes, nurseryHalf_); }

Oop Heap::initObject(std::byte* at, Oop cls, std::uint32_t size, std::uint16_t flags,
                     std::size_t n) {
  auto* h = reinterpret_cast<ObjectHeader*>(at);
  h->klass = cls;
  h->size = size;
  h->flags = flags;
  h->hash = nextHash_++;
  if (nextHash_ == 0) nextHash_ = 1;
  if (flags & kFlagBytes) {
    // 8 バイト境界までの詰め物も 0 にする。再利用する領域は前の中身や毒を含む。
    std::memset(reinterpret_cast<std::byte*>(h + 1), 0, n - sizeof(ObjectHeader));
  } else {
    auto* slots = reinterpret_cast<Oop*>(h + 1);
    for (std::uint32_t i = 0; i < size; ++i) slots[i] = Oop::nil();
  }
  return Oop::fromHeap(h);
}

ObjectHeader* Heap::header(Oop obj) {
  if (gcStress_ != 0) {
    checkNotPoisoned(obj);
  }
  return static_cast<ObjectHeader*>(obj.heapPointer());
}

const ObjectHeader* Heap::header(Oop obj) const {
  if (gcStress_ != 0) {
    checkNotPoisoned(obj);
  }
  return static_cast<const ObjectHeader*>(obj.heapPointer());
}

Oop Heap::klass(Oop obj) const { return header(obj)->klass; }

std::uint32_t Heap::size(Oop obj) const { return header(obj)->size; }

std::uint16_t Heap::flags(Oop obj) const { return header(obj)->flags; }

std::uint16_t Heap::hash(Oop obj) const { return header(obj)->hash; }

Oop Heap::slotAt(Oop obj, std::uint32_t i) const {
  const ObjectHeader* h = header(obj);
  assert((h->flags & kFlagBytes) == 0);
  assert(i < h->size);
  const auto* slots = reinterpret_cast<const Oop*>(h + 1);
  return slots[i];
}

void Heap::slotAtPut(Oop obj, std::uint32_t i, Oop value) {
  ObjectHeader* h = header(obj);
  assert((h->flags & kFlagBytes) == 0);
  assert(i < h->size);
  auto* slots = reinterpret_cast<Oop*>(h + 1);
  slots[i] = value;
}

std::byte* Heap::bytes(Oop obj) {
  return reinterpret_cast<std::byte*>(header(obj) + 1);
}

bool Heap::inNursery(Oop obj) const {
  if (!obj.isHeap()) {
    return false;
  }
  auto* p = static_cast<std::byte*>(obj.heapPointer());
  auto* start = nursery_.get();
  return p >= start && p < start + 2 * nurseryHalf_;
}

bool Heap::inOld(Oop obj) const {
  if (!obj.isHeap()) {
    return false;
  }
  auto* p = static_cast<std::byte*>(obj.heapPointer());
  return p >= oldStart_ && p < oldEnd_;
}

std::size_t Heap::oldUsed() const {
  return static_cast<std::size_t>(oldBump_ - oldStart_);
}

std::size_t Heap::oldCapacity() const {
  return static_cast<std::size_t>(oldEnd_ - oldStart_);
}

const std::byte* Heap::oldBase() const { return oldStart_; }

bool Heap::adoptOldBytes(const std::byte* src, std::size_t n, std::uint16_t nextHash) {
  if (oldUsed() != 0) {
    return false;
  }
  if (n > oldMax_) {
    return false;
  }
  if (n != 0 && src == nullptr) {
    return false;
  }
  if (n > oldCapacity() && !growOld(n)) {
    return false;
  }
  if (n != 0) {
    std::memcpy(oldStart_, src, n);
  }
  oldBump_ = oldStart_ + n;
  nextHash_ = nextHash == 0 ? static_cast<std::uint16_t>(1) : nextHash;
  // 取り込んだバイトは生存物として扱う。最初の full GC を collectOld 後と同じ閾値にそろえる。
  oldThreshold_ = std::clamp(2 * n, oldInitial_, oldMax_);
  return true;
}

std::uint16_t Heap::hashCursor() const { return nextHash_; }

void Heap::setGcStress(std::uint32_t n) {
  gcStress_ = n;
  stressTicks_ = 0;
  stressCollections_ = 0;
}

void Heap::poisonFreed(std::byte* begin, std::byte* end) {
  if (gcStress_ != 0 && begin < end) {
    std::memset(begin, kGcPoisonByte, static_cast<std::size_t>(end - begin));
  }
}

void Heap::checkNotPoisoned(Oop obj) const {
  if (!obj.isHeap()) {
    return;
  }
  const auto* h = static_cast<const ObjectHeader*>(obj.heapPointer());
  if (h->klass.bits() != kGcPoisonWord) {
    return;
  }
  std::fprintf(stderr, "ao: GC stress: stale reference to freed object at %p\n",
               obj.heapPointer());
  std::abort();
}

std::size_t Heap::nurseryRemaining() const {
  return static_cast<std::size_t>(fromEnd_ - fromBump_);
}

std::size_t Heap::nurseryCapacity() const { return nurseryHalf_; }

void Heap::flipNursery() {
  std::swap(fromStart_, toStart_);
  std::swap(fromEnd_, toEnd_);
  fromBump_ = toBump_;
  toBump_ = toStart_;
}

std::byte* Heap::reserveToSpace(std::size_t n) {
  if (toBump_ + n > toEnd_) {
    return nullptr;
  }
  std::byte* dest = toBump_;
  toBump_ += n;
  return dest;
}

bool Heap::containsNurseryFrom(void* p) const {
  auto* b = static_cast<std::byte*>(p);
  return b >= fromStart_ && b < fromEnd_;
}

bool Heap::containsNurseryTo(void* p) const {
  auto* b = static_cast<std::byte*>(p);
  return b >= toStart_ && b < toBump_;
}

// n バイトを old に置けるか（コミットを伸ばせば置ける場合を含む）。
bool Heap::fitsOld(std::size_t n) const { return n <= oldMax_ - oldUsed(); }

std::byte* Heap::reserveOld(std::size_t n) {
  if (!fitsOld(n)) {
    return nullptr;
  }
  if (n > static_cast<std::size_t>(oldEnd_ - oldBump_) && !growOld(oldUsed() + n)) {
    return nullptr;
  }
  std::byte* dest = oldBump_;
  oldBump_ += n;
  return dest;
}

bool Heap::growOld(std::size_t neededBytes) {
  if (neededBytes > oldMax_) {
    return false;
  }
  const std::size_t units = (neededBytes + kOldCommitUnit - 1) / kOldCommitUnit;
  const std::size_t capacity = std::min(oldMax_, units * kOldCommitUnit);
  if (!old_->commit(capacity)) {
    return false;
  }
  oldEnd_ = oldStart_ + capacity;
  return true;
}

}  // namespace ao

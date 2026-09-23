#include "ao/HandleScope.hpp"

#include "ao/Gc.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"

namespace ao {

RootedArray::RootedArray(Roots& roots, std::uint32_t count)
    : roots_(roots), n_(count), data_(inline_) {
  if (n_ == 0) {
    return;
  }
  if (n_ > kInlineSlots) {
    spill_.reset(new Oop[n_]);
    data_ = spill_.get();
  }
  for (std::uint32_t i = 0; i < n_; ++i) {
    data_[i] = Oop::nil();
  }
  roots_.pushRange(data_, n_);
}

RootedArray::~RootedArray() {
  if (n_ == 0) {
    return;
  }
  roots_.popRange(data_, n_);
}

Oop allocateRetry(CallContext& ctx, Oop cls, std::uint32_t size, std::uint16_t flags) {
  // SPEC §3.2: 諦めるまでに走らせる full GC は 1 回まで。この呼び出しの中で full GC が走ったか
  // （ストレス、大きな object の閾値、スキャベンジ後の閾値）を、入口の回数と比べて判断する。
  const std::uint64_t fullBefore = ctx.heap.oldCollections();
  if (ctx.heap.gcStress() != 0) {
    Root stressed(ctx.roots, cls);
    Gc(ctx.heap, ctx.roots).stressPoint();
    cls = stressed.slot;
  }
  // 大きな object は old に直置きされ、スキャベンジを起こさない。置く前に閾値を見る（SPEC §3.2）。
  const std::size_t bytes = ctx.heap.objectBytesFor(size, flags);
  const bool fitsOldMax = bytes <= ctx.heap.oldMaxBytes();
  if (bytes >= ctx.heap.largeObjectBytes()) {
    // old の上限より大きければ、どの GC をしても置けない。GC せずに失敗する。
    if (!fitsOldMax) {
      ctx.heap.setOutOfMemory();
      return Oop{};
    }
    Root large(ctx.roots, cls);
    Gc(ctx.heap, ctx.roots).collectBeforeTenured(bytes);
    cls = large.slot;
  }
  Oop obj = ctx.heap.allocate(cls, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  Root held(ctx.roots, cls);
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  obj = ctx.heap.allocate(held.slot, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  obj = ctx.heap.allocateTenured(held.slot, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  // old が上限。閾値は上限で頭打ちになり、死んだ old はスキャベンジからは見えないので、
  // 諦める前に一度だけ full GC で回収してから置き直す。この呼び出しの中ですでに full GC が
  // 走っていれば、もう一度走らせても回収できるものは無い。
  if (fitsOldMax && ctx.heap.oldCollections() == fullBefore) {
    gc.collectOld();
    obj = ctx.heap.allocateTenured(held.slot, size, flags);
    if (obj.isHeap()) {
      return obj;
    }
  }
  ctx.heap.setOutOfMemory();
  return Oop{};
}

}  // namespace ao

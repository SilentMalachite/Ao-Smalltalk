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
  if (ctx.heap.gcStress() != 0) {
    Root stressed(ctx.roots, cls);
    Gc(ctx.heap, ctx.roots).stressPoint();
    cls = stressed.slot;
  }
  // 大きな object は old に直置きされ、スキャベンジを起こさない。置く前に閾値を見る（SPEC §3.2）。
  const std::size_t bytes = ctx.heap.objectBytesFor(size, flags);
  if (bytes >= ctx.heap.largeObjectBytes()) {
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
  // 諦める前に一度だけ full GC で回収してから置き直す。
  gc.collectOld();
  obj = ctx.heap.allocateTenured(held.slot, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  ctx.heap.setOutOfMemory();
  return Oop{};
}

}  // namespace ao

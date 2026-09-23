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
  Oop obj = ctx.heap.allocate(cls, size, flags);
  if (obj.isHeap()) {
    return obj;
  }
  Root held(ctx.roots, cls);
  Gc gc(ctx.heap, ctx.roots);
  gc.collectNursery();
  return ctx.heap.allocate(held.slot, size, flags);
}

}  // namespace ao

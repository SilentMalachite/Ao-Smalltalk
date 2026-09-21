#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/Send.hpp"

namespace ao {
namespace {

struct Root {
  Roots& roots;
  Oop slot;
  explicit Root(Roots& r, Oop v = Oop{}) : roots(r), slot(v) { roots.add(&slot); }
  ~Root() { roots.remove(&slot); }
  Root(const Root&) = delete;
  Root& operator=(const Root&) = delete;
};

}  // namespace

Oop ao_ArrayedCollection_do_(CallContext& ctx, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root blk(ctx.roots, args[0]);
  Root elt(ctx.roots);
  const Oop nOop = send(ctx, rcvr.slot, ctx.wk.selSize, nullptr, 0, nullptr);
  if (!nOop.isSmallInteger()) {
    return nOop;
  }
  const auto n = nOop.smallIntegerValue();
  for (std::int64_t i = 1; i <= n; ++i) {
    Oop idx = Oop::fromSmallInteger(i);
    elt.slot = send(ctx, rcvr.slot, ctx.wk.selAt_, &idx, 1, nullptr);
    send(ctx, blk.slot, ctx.wk.selValue_, &elt.slot, 1, nullptr);
  }
  return rcvr.slot;
}

namespace kernel {

void installCollection(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.arrayedCollectionClass, "do:", 1, "ao_ArrayedCollection_do_",
            ao_ArrayedCollection_do_);
}

}  // namespace kernel
}  // namespace ao

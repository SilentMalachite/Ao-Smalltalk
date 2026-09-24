#include "ao/kernel/Install.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"
#include "ao/Natives.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"

namespace ao {
namespace {

constexpr std::uint32_t kPointX = 0;
constexpr std::uint32_t kPointY = 1;
constexpr std::uint32_t kRectOrigin = 0;
constexpr std::uint32_t kRectCorner = 1;

bool isPoint(CallContext& ctx, Oop o) {
  return o.isHeap() && ctx.heap.klass(o) == ctx.wk.pointClass && ctx.heap.size(o) > kPointY &&
         (ctx.heap.flags(o) & kFlagBytes) == 0;
}

bool isRect(CallContext& ctx, Oop o) {
  return o.isHeap() && ctx.heap.klass(o) == ctx.wk.rectangleClass &&
         ctx.heap.size(o) > kRectCorner && (ctx.heap.flags(o) & kFlagBytes) == 0;
}

Oop sendBin(CallContext& ctx, const Oop& rcvr, const char* sel, const Oop& arg) {
  Oop s = Symbol::intern(ctx.wk, sel);
  return send(ctx, rcvr, s, &arg, 1, nullptr);
}

Oop makePoint(CallContext& ctx, Oop x, Oop y) {
  Root rx(ctx.roots, x);
  Root ry(ctx.roots, y);
  Oop args[2] = {rx.slot, ry.slot};
  Oop sel = Symbol::intern(ctx.wk, "x:y:");
  return send(ctx, ctx.wk.pointClass, sel, args, 2, nullptr);
}

Oop makeRect(CallContext& ctx, Oop origin, Oop corner) {
  Root o(ctx.roots, origin);
  Root c(ctx.roots, corner);
  Oop args[2] = {o.slot, c.slot};
  Oop sel = Symbol::intern(ctx.wk, "origin:corner:");
  return send(ctx, ctx.wk.rectangleClass, sel, args, 2, nullptr);
}

Oop pointBin(CallContext& ctx, Oop receiver, Oop arg, const char* sel) {
  if (!isPoint(ctx, receiver)) {
    return Oop{};
  }
  Root rcvr(ctx.roots, receiver);
  Root other(ctx.roots, arg);
  Root x(ctx.roots, ctx.heap.slotAt(rcvr.slot, kPointX));
  Root y(ctx.roots, ctx.heap.slotAt(rcvr.slot, kPointY));
  Root ox(ctx.roots);
  Root oy(ctx.roots);
  if (isPoint(ctx, other.slot)) {
    ox.slot = ctx.heap.slotAt(other.slot, kPointX);
    oy.slot = ctx.heap.slotAt(other.slot, kPointY);
  } else {
    ox.slot = other.slot;
    oy.slot = other.slot;
  }
  Root nx(ctx.roots, sendBin(ctx, x.slot, sel, ox.slot));
  if (unwinding(ctx)) {
    return Oop{};
  }
  Root ny(ctx.roots, sendBin(ctx, y.slot, sel, oy.slot));
  if (unwinding(ctx)) {
    return Oop{};
  }
  return makePoint(ctx, nx.slot, ny.slot);
}

Oop magMax(CallContext& ctx, Oop a, Oop b) {
  Root ra(ctx.roots, a);
  Root rb(ctx.roots, b);
  return sendBin(ctx, ra.slot, "<", rb.slot).isTrue() ? rb.slot : ra.slot;
}

Oop magMin(CallContext& ctx, Oop a, Oop b) {
  Root ra(ctx.roots, a);
  Root rb(ctx.roots, b);
  return sendBin(ctx, ra.slot, "<", rb.slot).isTrue() ? ra.slot : rb.slot;
}

}  // namespace

Oop ao_Point_x_y_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Root x(ctx.roots, args[0]);
  Root y(ctx.roots, args[1]);
  // SPEC §3.6: a subclass that adds variables gets them. May GC: x and y are read from roots.
  Oop p = allocateInstance(ctx, cls.slot, 2);
  if (!p.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(p, kPointX, x.slot);
  ctx.heap.slotAtPut(p, kPointY, y.slot);
  return p;
}

Oop ao_Point_x(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isPoint(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kPointX);
}

Oop ao_Point_y(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isPoint(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kPointY);
}

Oop ao_Point_x_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !isPoint(ctx, receiver)) {
    return Oop{};
  }
  ctx.heap.slotAtPut(receiver, kPointX, args[0]);
  return receiver;
}

Oop ao_Point_y_(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1 || !isPoint(ctx, receiver)) {
    return Oop{};
  }
  ctx.heap.slotAtPut(receiver, kPointY, args[0]);
  return receiver;
}

Oop ao_Point_add(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return pointBin(ctx, receiver, args[0], "+");
}

Oop ao_Point_subtract(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return pointBin(ctx, receiver, args[0], "-");
}

Oop ao_Point_multiply(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return pointBin(ctx, receiver, args[0], "*");
}

Oop ao_Point_intDivide(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  return pointBin(ctx, receiver, args[0], "//");
}

Oop ao_Point_equals(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) {
    return Oop{};
  }
  if (!isPoint(ctx, receiver) || !isPoint(ctx, args[0])) {
    return Oop::false_();
  }
  Root rcvr(ctx.roots, receiver);
  Root other(ctx.roots, args[0]);
  Root x(ctx.roots, ctx.heap.slotAt(rcvr.slot, kPointX));
  Root ox(ctx.roots, ctx.heap.slotAt(other.slot, kPointX));
  if (!sendBin(ctx, x.slot, "=", ox.slot).isTrue()) {
    return unwinding(ctx) ? Oop{} : Oop::false_();
  }
  Root y(ctx.roots, ctx.heap.slotAt(rcvr.slot, kPointY));
  Root oy(ctx.roots, ctx.heap.slotAt(other.slot, kPointY));
  const Oop eq = sendBin(ctx, y.slot, "=", oy.slot);
  if (unwinding(ctx)) {
    return Oop{};
  }
  return eq.isTrue() ? Oop::true_() : Oop::false_();
}


// SPEC §3.6: from the hashes of x and y, as Point>>= compares them. Past kMaxHashNesting nested
// element hashes, a constant.
Oop ao_Point_hash(CallContext& ctx, const Oop& receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 0) {
    return Oop{};
  }
  if (!isPoint(ctx, receiver)) {
    return ao_Object_identityHash(ctx, receiver, args, argc);
  }
  std::uint64_t h = valueHashWord(kValueHashSeed, 2);
  if (ctx.hashNesting < kMaxHashNesting) {
    HashNesting nesting(ctx);
    // receiver is a rooted slot: y is read after the send for x, where the GC moved it.
    if (!mixElementHash(ctx, ctx.heap.slotAt(receiver, kPointX), &h) ||
        !mixElementHash(ctx, ctx.heap.slotAt(receiver, kPointY), &h)) {
      return Oop{};
    }
  }
  return Oop::fromSmallInteger(valueHashFold(h));
}

Oop ao_Rectangle_origin_corner_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc) {
  if (argc != 2) {
    return Oop{};
  }
  Root cls(ctx.roots, receiver);
  Root origin(ctx.roots, args[0]);
  Root corner(ctx.roots, args[1]);
  // SPEC §3.6: a subclass that adds variables gets them. May GC: origin and corner are read from
  // roots.
  Oop r = allocateInstance(ctx, cls.slot, 2);
  if (!r.isHeap()) {
    return Oop{};
  }
  ctx.heap.slotAtPut(r, kRectOrigin, origin.slot);
  ctx.heap.slotAtPut(r, kRectCorner, corner.slot);
  return r;
}

Oop ao_Rectangle_origin(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isRect(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kRectOrigin);
}

Oop ao_Rectangle_corner(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isRect(ctx, receiver)) {
    return Oop{};
  }
  return ctx.heap.slotAt(receiver, kRectCorner);
}

Oop ao_Rectangle_width(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isRect(ctx, receiver)) {
    return Oop{};
  }
  Root r(ctx.roots, receiver);
  Root origin(ctx.roots, ctx.heap.slotAt(r.slot, kRectOrigin));
  Root corner(ctx.roots, ctx.heap.slotAt(r.slot, kRectCorner));
  if (!isPoint(ctx, origin.slot) || !isPoint(ctx, corner.slot)) {
    return Oop{};
  }
  Root cx(ctx.roots, ctx.heap.slotAt(corner.slot, kPointX));
  Root ox(ctx.roots, ctx.heap.slotAt(origin.slot, kPointX));
  return sendBin(ctx, cx.slot, "-", ox.slot);
}

Oop ao_Rectangle_height(CallContext& ctx, const Oop& receiver, const Oop*, std::uint32_t argc) {
  if (argc != 0 || !isRect(ctx, receiver)) {
    return Oop{};
  }
  Root r(ctx.roots, receiver);
  Root origin(ctx.roots, ctx.heap.slotAt(r.slot, kRectOrigin));
  Root corner(ctx.roots, ctx.heap.slotAt(r.slot, kRectCorner));
  if (!isPoint(ctx, origin.slot) || !isPoint(ctx, corner.slot)) {
    return Oop{};
  }
  Root cy(ctx.roots, ctx.heap.slotAt(corner.slot, kPointY));
  Root oy(ctx.roots, ctx.heap.slotAt(origin.slot, kPointY));
  return sendBin(ctx, cy.slot, "-", oy.slot);
}

Oop ao_Rectangle_containsPoint_(CallContext& ctx, const Oop& receiver, const Oop* args,
                                std::uint32_t argc) {
  if (argc != 1 || !isRect(ctx, receiver)) {
    return Oop{};
  }
  if (!isPoint(ctx, args[0])) {
    return Oop::false_();
  }
  Root r(ctx.roots, receiver);
  Root p(ctx.roots, args[0]);
  Root origin(ctx.roots, ctx.heap.slotAt(r.slot, kRectOrigin));
  Root corner(ctx.roots, ctx.heap.slotAt(r.slot, kRectCorner));
  if (!isPoint(ctx, origin.slot) || !isPoint(ctx, corner.slot)) {
    return Oop::false_();
  }
  Root ox(ctx.roots, ctx.heap.slotAt(origin.slot, kPointX));
  Root oy(ctx.roots, ctx.heap.slotAt(origin.slot, kPointY));
  Root cx(ctx.roots, ctx.heap.slotAt(corner.slot, kPointX));
  Root cy(ctx.roots, ctx.heap.slotAt(corner.slot, kPointY));
  Root px(ctx.roots, ctx.heap.slotAt(p.slot, kPointX));
  Root py(ctx.roots, ctx.heap.slotAt(p.slot, kPointY));
  // A send that starts an unwind answers the empty OOP, which is not true (SPEC §3.4).
  if (!sendBin(ctx, ox.slot, "<=", px.slot).isTrue()) {
    return unwinding(ctx) ? Oop{} : Oop::false_();
  }
  if (!sendBin(ctx, oy.slot, "<=", py.slot).isTrue()) {
    return unwinding(ctx) ? Oop{} : Oop::false_();
  }
  if (!sendBin(ctx, px.slot, "<", cx.slot).isTrue()) {
    return unwinding(ctx) ? Oop{} : Oop::false_();
  }
  if (!sendBin(ctx, py.slot, "<", cy.slot).isTrue()) {
    return unwinding(ctx) ? Oop{} : Oop::false_();
  }
  return Oop::true_();
}

Oop ao_Rectangle_intersect_(CallContext& ctx, const Oop& receiver, const Oop* args,
                            std::uint32_t argc) {
  if (argc != 1 || !isRect(ctx, receiver) || !isRect(ctx, args[0])) {
    return Oop{};
  }
  Root r(ctx.roots, receiver);
  Root other(ctx.roots, args[0]);
  Root o1(ctx.roots, ctx.heap.slotAt(r.slot, kRectOrigin));
  Root c1(ctx.roots, ctx.heap.slotAt(r.slot, kRectCorner));
  Root o2(ctx.roots, ctx.heap.slotAt(other.slot, kRectOrigin));
  Root c2(ctx.roots, ctx.heap.slotAt(other.slot, kRectCorner));
  if (!isPoint(ctx, o1.slot) || !isPoint(ctx, c1.slot) || !isPoint(ctx, o2.slot) ||
      !isPoint(ctx, c2.slot)) {
    return Oop{};
  }
  Root o1x(ctx.roots, ctx.heap.slotAt(o1.slot, kPointX));
  Root o1y(ctx.roots, ctx.heap.slotAt(o1.slot, kPointY));
  Root o2x(ctx.roots, ctx.heap.slotAt(o2.slot, kPointX));
  Root o2y(ctx.roots, ctx.heap.slotAt(o2.slot, kPointY));
  Root c1x(ctx.roots, ctx.heap.slotAt(c1.slot, kPointX));
  Root c1y(ctx.roots, ctx.heap.slotAt(c1.slot, kPointY));
  Root c2x(ctx.roots, ctx.heap.slotAt(c2.slot, kPointX));
  Root c2y(ctx.roots, ctx.heap.slotAt(c2.slot, kPointY));
  Root ox(ctx.roots, magMax(ctx, o1x.slot, o2x.slot));
  if (unwinding(ctx)) {
    return Oop{};
  }
  Root oy(ctx.roots, magMax(ctx, o1y.slot, o2y.slot));
  if (unwinding(ctx)) {
    return Oop{};
  }
  Root cx(ctx.roots, magMin(ctx, c1x.slot, c2x.slot));
  if (unwinding(ctx)) {
    return Oop{};
  }
  Root cy(ctx.roots, magMin(ctx, c1y.slot, c2y.slot));
  if (unwinding(ctx)) {
    return Oop{};
  }
  Root origin(ctx.roots, makePoint(ctx, ox.slot, oy.slot));
  Root corner(ctx.roots, makePoint(ctx, cx.slot, cy.slot));
  return makeRect(ctx, origin.slot, corner.slot);
}

namespace kernel {

void installGeometry(Heap& heap, WellKnown& wk) {
  putNative(heap, wk, wk.pointMetaclass, "x:y:", 2, "ao_Point_x_y_", ao_Point_x_y_);
  putNative(heap, wk, wk.pointClass, "x", 0, "ao_Point_x", ao_Point_x);
  putNative(heap, wk, wk.pointClass, "y", 0, "ao_Point_y", ao_Point_y);
  putNative(heap, wk, wk.pointClass, "x:", 1, "ao_Point_x_", ao_Point_x_);
  putNative(heap, wk, wk.pointClass, "y:", 1, "ao_Point_y_", ao_Point_y_);
  putNative(heap, wk, wk.pointClass, "+", 1, "ao_Point_add", ao_Point_add);
  putNative(heap, wk, wk.pointClass, "-", 1, "ao_Point_subtract", ao_Point_subtract);
  putNative(heap, wk, wk.pointClass, "*", 1, "ao_Point_multiply", ao_Point_multiply);
  putNative(heap, wk, wk.pointClass, "//", 1, "ao_Point_intDivide", ao_Point_intDivide);
  putNative(heap, wk, wk.pointClass, "=", 1, "ao_Point_equals", ao_Point_equals);
  putNative(heap, wk, wk.pointClass, "hash", 0, "ao_Point_hash", ao_Point_hash);

  putNative(heap, wk, wk.rectangleMetaclass, "origin:corner:", 2, "ao_Rectangle_origin_corner_",
            ao_Rectangle_origin_corner_);
  putNative(heap, wk, wk.rectangleClass, "origin", 0, "ao_Rectangle_origin", ao_Rectangle_origin);
  putNative(heap, wk, wk.rectangleClass, "corner", 0, "ao_Rectangle_corner", ao_Rectangle_corner);
  putNative(heap, wk, wk.rectangleClass, "width", 0, "ao_Rectangle_width", ao_Rectangle_width);
  putNative(heap, wk, wk.rectangleClass, "height", 0, "ao_Rectangle_height", ao_Rectangle_height);
  putNative(heap, wk, wk.rectangleClass, "containsPoint:", 1, "ao_Rectangle_containsPoint_",
            ao_Rectangle_containsPoint_);
  putNative(heap, wk, wk.rectangleClass, "intersect:", 1, "ao_Rectangle_intersect_",
            ao_Rectangle_intersect_);
}

}  // namespace kernel
}  // namespace ao

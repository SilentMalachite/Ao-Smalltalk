#include "ao/LargeInteger.hpp"

#include "ao/Context.hpp"
#include "ao/HandleScope.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace ao {
namespace LargeInteger {
namespace {

using Digits = std::vector<std::uint32_t>;

struct Big {
  bool neg = false;
  Digits d;
  bool isZero() const { return d.empty(); }
};

void strip(Digits& d) {
  while (!d.empty() && d.back() == 0) {
    d.pop_back();
  }
}

int magCmp(const Digits& a, const Digits& b) {
  if (a.size() != b.size()) {
    return a.size() < b.size() ? -1 : 1;
  }
  for (std::size_t i = a.size(); i-- > 0;) {
    if (a[i] != b[i]) {
      return a[i] < b[i] ? -1 : 1;
    }
  }
  return 0;
}

void magAdd(const Digits& a, const Digits& b, Digits& r) {
  const std::size_t n = std::max(a.size(), b.size());
  r.assign(n, 0);
  std::uint64_t carry = 0;
  for (std::size_t i = 0; i < n; ++i) {
    std::uint64_t s = carry;
    if (i < a.size()) s += a[i];
    if (i < b.size()) s += b[i];
    r[i] = static_cast<std::uint32_t>(s);
    carry = s >> 32;
  }
  if (carry != 0) {
    r.push_back(static_cast<std::uint32_t>(carry));
  }
}

void magSub(const Digits& a, const Digits& b, Digits& r) {
  r.assign(a.size(), 0);
  std::int64_t borrow = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    std::int64_t s = static_cast<std::int64_t>(a[i]) - borrow;
    if (i < b.size()) {
      s -= b[i];
    }
    if (s < 0) {
      s += static_cast<std::int64_t>(1) << 32;
      borrow = 1;
    } else {
      borrow = 0;
    }
    r[i] = static_cast<std::uint32_t>(s);
  }
  strip(r);
}

void magSubInPlace(Digits& a, const Digits& b) {
  std::int64_t borrow = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    std::int64_t s = static_cast<std::int64_t>(a[i]) - borrow;
    if (i < b.size()) {
      s -= b[i];
    }
    if (s < 0) {
      s += static_cast<std::int64_t>(1) << 32;
      borrow = 1;
    } else {
      borrow = 0;
    }
    a[i] = static_cast<std::uint32_t>(s);
  }
  strip(a);
}

void magMul(const Digits& a, const Digits& b, Digits& r) {
  if (a.empty() || b.empty()) {
    r.clear();
    return;
  }
  r.assign(a.size() + b.size(), 0);
  for (std::size_t i = 0; i < a.size(); ++i) {
    std::uint64_t carry = 0;
    for (std::size_t j = 0; j < b.size(); ++j) {
      const std::uint64_t t =
          static_cast<std::uint64_t>(r[i + j]) + static_cast<std::uint64_t>(a[i]) * b[j] + carry;
      r[i + j] = static_cast<std::uint32_t>(t);
      carry = t >> 32;
    }
    r[i + b.size()] = static_cast<std::uint32_t>(carry);
  }
  strip(r);
}

int magBitLength(const Digits& d) {
  if (d.empty()) {
    return 0;
  }
  const std::uint32_t top = d.back();
  if (top == 0) {
    return static_cast<int>(d.size() * 32);
  }
  return static_cast<int>((d.size() - 1) * 32 + 32 - static_cast<unsigned>(__builtin_clz(top)));
}

bool magBit(const Digits& d, int bit) {
  if (bit < 0) {
    return false;
  }
  const int idx = bit / 32;
  if (idx >= static_cast<int>(d.size())) {
    return false;
  }
  return ((d[static_cast<std::size_t>(idx)] >> (bit % 32)) & 1u) != 0;
}

void magSetBit(Digits& d, int bit) {
  const int idx = bit / 32;
  if (idx >= static_cast<int>(d.size())) {
    d.resize(static_cast<std::size_t>(idx) + 1, 0);
  }
  d[static_cast<std::size_t>(idx)] |= 1u << (bit % 32);
}

void magShl1(Digits& d) {
  std::uint32_t carry = 0;
  for (std::size_t i = 0; i < d.size(); ++i) {
    const std::uint32_t next = d[i] >> 31;
    d[i] = (d[i] << 1) | carry;
    carry = next;
  }
  if (carry != 0) {
    d.push_back(carry);
  }
}

void magShl(Digits& d, unsigned k) {
  if (d.empty() || k == 0) {
    return;
  }
  const unsigned words = k / 32;
  const unsigned bits = k % 32;
  d.insert(d.begin(), words, 0);
  if (bits == 0) {
    return;
  }
  std::uint32_t carry = 0;
  for (std::size_t i = 0; i < d.size(); ++i) {
    const std::uint32_t next = d[i] >> (32 - bits);
    d[i] = (d[i] << bits) | carry;
    carry = next;
  }
  if (carry != 0) {
    d.push_back(carry);
  }
}

void magFromU128(unsigned __int128 v, Digits& d) {
  d.clear();
  while (v != 0) {
    d.push_back(static_cast<std::uint32_t>(v));
    v >>= 32;
  }
}

bool magToU128(const Digits& d, unsigned __int128* out) {
  if (d.size() > 4) {
    return false;
  }
  unsigned __int128 v = 0;
  for (std::size_t i = d.size(); i-- > 0;) {
    v = (v << 32) | d[i];
  }
  *out = v;
  return true;
}

void magDivMod(const Digits& a, const Digits& b, Digits& q, Digits& r) {
  q.clear();
  r.clear();
  if (a.empty()) {
    return;
  }
  unsigned __int128 ua = 0;
  unsigned __int128 ub = 0;
  if (magToU128(a, &ua) && magToU128(b, &ub) && ub != 0) {
    magFromU128(ua / ub, q);
    magFromU128(ua % ub, r);
    return;
  }
  const int bits = magBitLength(a);
  for (int bit = bits - 1; bit >= 0; --bit) {
    magShl1(r);
    if (magBit(a, bit)) {
      std::uint64_t carry = 1;
      for (std::size_t i = 0; carry != 0; ++i) {
        if (i == r.size()) {
          r.push_back(0);
        }
        const std::uint64_t s = static_cast<std::uint64_t>(r[i]) + carry;
        r[i] = static_cast<std::uint32_t>(s);
        carry = s >> 32;
      }
    }
    if (magCmp(r, b) >= 0) {
      magSubInPlace(r, b);
      magSetBit(q, bit);
    }
  }
  strip(q);
  strip(r);
}

int cmpBig(const Big& a, const Big& b) {
  if (a.isZero() && b.isZero()) {
    return 0;
  }
  if (a.neg != b.neg) {
    return a.neg ? -1 : 1;
  }
  const int c = magCmp(a.d, b.d);
  return a.neg ? -c : c;
}

Big addBig(const Big& a, const Big& b) {
  Big r;
  if (a.neg == b.neg) {
    magAdd(a.d, b.d, r.d);
    r.neg = a.neg && !r.d.empty();
    return r;
  }
  const int c = magCmp(a.d, b.d);
  if (c == 0) {
    return r;
  }
  if (c > 0) {
    magSub(a.d, b.d, r.d);
    r.neg = a.neg;
  } else {
    magSub(b.d, a.d, r.d);
    r.neg = b.neg;
  }
  if (r.d.empty()) {
    r.neg = false;
  }
  return r;
}

Big mulBig(const Big& a, const Big& b) {
  Big r;
  magMul(a.d, b.d, r.d);
  r.neg = !r.d.empty() && (a.neg != b.neg);
  return r;
}

// d = d * mul + add.
void magMulAddSmall(Digits& d, std::uint32_t mul, std::uint32_t add) {
  std::uint64_t carry = add;
  for (std::uint32_t& limb : d) {
    const std::uint64_t t = static_cast<std::uint64_t>(limb) * mul + carry;
    limb = static_cast<std::uint32_t>(t);
    carry = t >> 32;
  }
  if (carry != 0) {
    d.push_back(static_cast<std::uint32_t>(carry));
  }
}

void digitsFromU64(std::uint64_t mag, Digits& d) {
  d.clear();
  if (mag == 0) {
    return;
  }
  d.push_back(static_cast<std::uint32_t>(mag));
  const std::uint32_t hi = static_cast<std::uint32_t>(mag >> 32);
  if (hi != 0) {
    d.push_back(hi);
  }
}

bool parse(Heap& heap, WellKnown& wk, Oop o, Big& out) {
  out.neg = false;
  out.d.clear();
  if (o.isSmallInteger()) {
    const std::int64_t v = o.smallIntegerValue();
    if (v == 0) {
      return true;
    }
    out.neg = v < 0;
    const std::uint64_t mag =
        v >= 0 ? static_cast<std::uint64_t>(v) : 0u - static_cast<std::uint64_t>(v);
    digitsFromU64(mag, out.d);
    return true;
  }
  if (!o.isHeap()) {
    return false;
  }
  const Oop k = heap.klass(o);
  if (k == wk.largePositiveIntegerClass) {
    out.neg = false;
  } else if (k == wk.largeNegativeIntegerClass) {
    out.neg = true;
  } else {
    return false;
  }
  if ((heap.flags(o) & kFlagBytes) == 0) {
    return false;
  }
  const std::uint32_t n = heap.size(o);
  if (n == 0) {
    out.neg = false;
    return true;
  }
  out.d.assign((n + 3u) / 4u, 0);
  std::memcpy(out.d.data(), heap.bytes(o), n);
  strip(out.d);
  if (out.d.empty()) {
    out.neg = false;
  }
  return true;
}

bool toInt64(const Big& b, std::int64_t* out) {
  if (b.d.empty()) {
    *out = 0;
    return true;
  }
  if (b.d.size() > 2) {
    return false;
  }
  std::uint64_t mag = b.d[0];
  if (b.d.size() == 2) {
    mag |= static_cast<std::uint64_t>(b.d[1]) << 32;
  }
  constexpr std::uint64_t kMax = static_cast<std::uint64_t>(INT64_MAX);
  if (!b.neg) {
    if (mag > kMax) {
      return false;
    }
    *out = static_cast<std::int64_t>(mag);
    return true;
  }
  if (mag > kMax + 1) {
    return false;
  }
  if (mag == kMax + 1) {
    *out = INT64_MIN;
    return true;
  }
  *out = -static_cast<std::int64_t>(mag);
  return true;
}

Oop box(Heap& heap, WellKnown& wk, const Big& b) {
  std::int64_t v = 0;
  if (toInt64(b, &v) && v >= kSmiMin && v <= kSmiMax) {
    return Oop::fromSmallInteger(v);
  }
  const auto n = static_cast<std::uint32_t>(b.d.size() * 4);
  const Oop cls = b.neg ? wk.largeNegativeIntegerClass : wk.largePositiveIntegerClass;
  Oop o = heap.allocate(cls, n, kFlagBytes);
  if (!o.isHeap()) {
    return Oop{};
  }
  std::memcpy(heap.bytes(o), b.d.data(), n);
  return o;
}

Oop box(CallContext& ctx, const Big& b) {
  std::int64_t v = 0;
  if (toInt64(b, &v) && v >= kSmiMin && v <= kSmiMax) {
    return Oop::fromSmallInteger(v);
  }
  const auto n = static_cast<std::uint32_t>(b.d.size() * 4);
  const Oop cls = b.neg ? ctx.wk.largeNegativeIntegerClass : ctx.wk.largePositiveIntegerClass;
  const Oop o = allocateRetry(ctx, cls, n, kFlagBytes);
  if (!o.isHeap()) {
    return Oop{};
  }
  std::memcpy(ctx.heap.bytes(o), b.d.data(), n);
  return o;
}

Oop fromInt128(CallContext& ctx, __int128 value) {
  if (value >= kSmiMin && value <= kSmiMax) {
    return Oop::fromSmallInteger(static_cast<std::int64_t>(value));
  }
  Big b;
  b.neg = value < 0;
  unsigned __int128 mag = value >= 0 ? static_cast<unsigned __int128>(value)
                                     : 0 - static_cast<unsigned __int128>(value);
  magFromU128(mag, b.d);
  return box(ctx, b);
}

Digits toTwos(const Big& b, std::size_t n) {
  Digits t(n, 0);
  const std::size_t m = std::min(n, b.d.size());
  for (std::size_t i = 0; i < m; ++i) {
    t[i] = b.d[i];
  }
  if (!b.neg) {
    return t;
  }
  std::uint64_t carry = 1;
  for (std::size_t i = 0; i < n; ++i) {
    const std::uint64_t v = static_cast<std::uint32_t>(~t[i]) + carry;
    t[i] = static_cast<std::uint32_t>(v);
    carry = v >> 32;
  }
  return t;
}

Big fromTwos(Digits t) {
  Big b;
  if (t.empty()) {
    return b;
  }
  const bool neg = (t.back() & 0x80000000u) != 0;
  if (!neg) {
    b.d = std::move(t);
    strip(b.d);
    return b;
  }
  b.neg = true;
  b.d.resize(t.size());
  std::uint64_t carry = 1;
  for (std::size_t i = 0; i < t.size(); ++i) {
    const std::uint64_t v = static_cast<std::uint32_t>(~t[i]) + carry;
    b.d[i] = static_cast<std::uint32_t>(v);
    carry = v >> 32;
  }
  strip(b.d);
  if (b.d.empty()) {
    b.neg = false;
  }
  return b;
}

enum class BitOp { And, Or, Xor };

Big bitOp(const Big& a, const Big& b, BitOp op) {
  const std::size_t n = std::max(a.d.size(), b.d.size()) + 1;
  Digits ta = toTwos(a, n);
  Digits tb = toTwos(b, n);
  Digits tr(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    switch (op) {
      case BitOp::And:
        tr[i] = ta[i] & tb[i];
        break;
      case BitOp::Or:
        tr[i] = ta[i] | tb[i];
        break;
      case BitOp::Xor:
        tr[i] = ta[i] ^ tb[i];
        break;
    }
  }
  return fromTwos(std::move(tr));
}

void truncDivMod(const Big& a, const Big& b, Big& q, Big& r) {
  magDivMod(a.d, b.d, q.d, r.d);
  q.neg = !q.d.empty() && (a.neg != b.neg);
  r.neg = !r.d.empty() && a.neg;
}

void floorDivMod(const Big& a, const Big& b, Big& q, Big& r) {
  truncDivMod(a, b, q, r);
  if (!r.d.empty() && a.neg != b.neg) {
    Big one;
    one.d.push_back(1);
    one.neg = true;
    q = addBig(q, one);
    r = addBig(r, b);
  }
  if (q.d.empty()) {
    q.neg = false;
  }
  if (r.d.empty()) {
    r.neg = false;
  }
}

// True when a bit of d below `bit` is set.
bool magAnyBitBelow(const Digits& d, std::int64_t bit) {
  if (bit <= 0) {
    return false;
  }
  const auto whole = static_cast<std::size_t>(bit / 32);
  for (std::size_t i = 0; i < whole && i < d.size(); ++i) {
    if (d[i] != 0) {
      return true;
    }
  }
  const auto rest = static_cast<unsigned>(bit % 32);
  return rest != 0 && whole < d.size() && (d[whole] & ((1u << rest) - 1u)) != 0;
}

bool magBitAt(const Digits& d, std::int64_t bit) {
  return bit >= 0 && bit < static_cast<std::int64_t>(d.size()) * 32 &&
         magBit(d, static_cast<int>(bit));
}

// SPEC §3.6: the double nearest to mag · 2^exp2 (IEEE754 binary64, ties to even, subnormals
// included), +inf beyond the range. mag is not zero. sticky says nonzero bits below mag were
// dropped: the value is a little above mag · 2^exp2.
double roundToDouble(const Digits& mag, std::int64_t exp2, bool sticky) {
  const std::int64_t len = magBitLength(mag);
  const std::int64_t top = len - 1 + exp2;  // the exponent of the leading bit
  if (top > 1023) {
    return std::numeric_limits<double>::infinity();
  }
  // Significand bits kept: 53, fewer for a subnormal (none at all below 2^-1075).
  const std::int64_t keep = top >= -1022 ? 53 : 53 - (-1022 - top);
  const std::int64_t drop = len - keep;
  std::uint64_t m = 0;
  for (std::int64_t i = len - 1; i >= std::max<std::int64_t>(drop, 0); --i) {
    m = (m << 1) | (magBitAt(mag, i) ? 1u : 0u);
  }
  if (drop <= 0) {
    // Exact: at most 53 bits.
    return std::ldexp(static_cast<double>(m), static_cast<int>(exp2));
  }
  const bool half = magBitAt(mag, drop - 1);
  const bool rest = sticky || magAnyBitBelow(mag, drop - 1);
  if (half && (rest || (m & 1u) != 0)) {
    ++m;  // 2^53 at most; ldexp then gives the next binade, or inf past 2^1024
  }
  return std::ldexp(static_cast<double>(m), static_cast<int>(exp2 + drop));
}

Big bigOf(std::int64_t v) {
  Big b;
  b.neg = v < 0;
  digitsFromU64(v >= 0 ? static_cast<std::uint64_t>(v) : 0u - static_cast<std::uint64_t>(v), b.d);
  return b;
}

// Moves the sign of den into num, so that num / den keeps its value with den > 0. False when den
// is 0.
bool positiveDenominator(Big& num, Big& den) {
  if (den.isZero()) {
    return false;
  }
  if (den.neg) {
    den.neg = false;
    num.neg = !num.isZero() && !num.neg;
  }
  return true;
}

}  // namespace

Oop fromInt64(Heap& heap, WellKnown& wk, std::int64_t value) {
  if (value >= kSmiMin && value <= kSmiMax) {
    return Oop::fromSmallInteger(value);
  }
  Big b;
  b.neg = value < 0;
  const std::uint64_t mag =
      value >= 0 ? static_cast<std::uint64_t>(value) : 0u - static_cast<std::uint64_t>(value);
  digitsFromU64(mag, b.d);
  return box(heap, wk, b);
}

Oop fromInt64(CallContext& ctx, std::int64_t value) { return fromInt128(ctx, value); }

Oop fromText(CallContext& ctx, std::string_view text) {
  Big b;
  std::size_t i = 0;
  if (i < text.size() && text[i] == '-') {
    b.neg = true;
    ++i;
  }
  std::uint32_t radix = 10;
  const std::size_t r = text.find('r', i);
  if (r != std::string_view::npos) {
    radix = 0;
    for (; i < r; ++i) {
      if (text[i] < '0' || text[i] > '9' || radix > 36) {
        return Oop{};
      }
      radix = radix * 10 + static_cast<std::uint32_t>(text[i] - '0');
    }
    ++i;
  }
  if (radix < 2 || radix > 36 || i == text.size()) {
    return Oop{};
  }
  // A chunk of digits at a time: chunk < scale = radix^(digits in the chunk) < 2^32.
  std::uint32_t chunk = 0;
  std::uint32_t scale = 1;
  for (; i < text.size(); ++i) {
    const char c = text[i];
    std::uint32_t v = radix;
    if (c >= '0' && c <= '9') {
      v = static_cast<std::uint32_t>(c - '0');
    } else if (c >= 'A' && c <= 'Z') {
      v = static_cast<std::uint32_t>(c - 'A') + 10;
    }
    if (v >= radix) {
      return Oop{};
    }
    chunk = chunk * radix + v;
    scale *= radix;
    if (scale > UINT32_MAX / radix || i + 1 == text.size()) {
      magMulAddSmall(b.d, scale, chunk);
      chunk = 0;
      scale = 1;
    }
  }
  strip(b.d);
  if (b.d.empty()) {
    b.neg = false;
  }
  return box(ctx, b);
}

bool isLarge(const WellKnown& wk, Oop o) {
  if (!o.isHeap()) {
    return false;
  }
  const Oop k = wk.classOf(o);
  return k == wk.largePositiveIntegerClass || k == wk.largeNegativeIntegerClass;
}

bool isInteger(const WellKnown& wk, Oop o) { return o.isSmallInteger() || isLarge(wk, o); }

bool isZero(Heap& heap, WellKnown& wk, Oop o) {
  if (o.isSmallInteger()) {
    return o.smallIntegerValue() == 0;
  }
  Big b;
  return parse(heap, wk, o, b) && b.isZero();
}

std::int64_t asInt64IfFits(Heap& heap, WellKnown& wk, Oop o, bool* fits) {
  Big b;
  if (!parse(heap, wk, o, b)) {
    if (fits != nullptr) {
      *fits = false;
    }
    return 0;
  }
  std::int64_t v = 0;
  if (!toInt64(b, &v)) {
    if (fits != nullptr) {
      *fits = false;
    }
    return 0;
  }
  if (fits != nullptr) {
    *fits = true;
  }
  return v;
}

int compare(Heap& heap, WellKnown& wk, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(heap, wk, a, A) || !parse(heap, wk, b, B)) {
    return 0;
  }
  return cmpBig(A, B);
}

Oop add(CallContext& ctx, Oop a, Oop b) {
  bool fa = false;
  bool fb = false;
  const std::int64_t ia = asInt64IfFits(ctx.heap, ctx.wk, a, &fa);
  const std::int64_t ib = asInt64IfFits(ctx.heap, ctx.wk, b, &fb);
  if (fa && fb) {
    return fromInt128(ctx, static_cast<__int128>(ia) + static_cast<__int128>(ib));
  }
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  return box(ctx, addBig(A, B));
}

Oop sub(CallContext& ctx, Oop a, Oop b) {
  bool fa = false;
  bool fb = false;
  const std::int64_t ia = asInt64IfFits(ctx.heap, ctx.wk, a, &fa);
  const std::int64_t ib = asInt64IfFits(ctx.heap, ctx.wk, b, &fb);
  if (fa && fb) {
    return fromInt128(ctx, static_cast<__int128>(ia) - static_cast<__int128>(ib));
  }
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  if (!B.isZero()) {
    B.neg = !B.neg;
  }
  return box(ctx, addBig(A, B));
}

Oop mul(CallContext& ctx, Oop a, Oop b) {
  bool fa = false;
  bool fb = false;
  const std::int64_t ia = asInt64IfFits(ctx.heap, ctx.wk, a, &fa);
  const std::int64_t ib = asInt64IfFits(ctx.heap, ctx.wk, b, &fb);
  if (fa && fb) {
    return fromInt128(ctx, static_cast<__int128>(ia) * static_cast<__int128>(ib));
  }
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  return box(ctx, mulBig(A, B));
}

Oop floorDiv(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B) || B.isZero()) {
    return Oop{};
  }
  Big q;
  Big r;
  floorDivMod(A, B, q, r);
  return box(ctx, q);
}

Oop truncDiv(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B) || B.isZero()) {
    return Oop{};
  }
  Big q;
  Big r;
  truncDivMod(A, B, q, r);
  return box(ctx, q);
}

Oop modulo(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B) || B.isZero()) {
    return Oop{};
  }
  Big q;
  Big r;
  floorDivMod(A, B, q, r);
  return box(ctx, r);
}

Oop remainder(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B) || B.isZero()) {
    return Oop{};
  }
  Big q;
  Big r;
  truncDivMod(A, B, q, r);
  return box(ctx, r);
}

Oop bitAnd(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  return box(ctx, bitOp(A, B, BitOp::And));
}

Oop bitOr(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  return box(ctx, bitOp(A, B, BitOp::Or));
}

Oop bitXor(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  return box(ctx, bitOp(A, B, BitOp::Xor));
}

Oop bitShift(CallContext& ctx, Oop a, Oop n) {
  Big A;
  if (!parse(ctx.heap, ctx.wk, a, A)) {
    return Oop{};
  }
  bool fits = false;
  const std::int64_t sh = asInt64IfFits(ctx.heap, ctx.wk, n, &fits);
  if (!fits) {
    return Oop{};
  }
  if (A.isZero() || sh == 0) {
    return box(ctx, A);
  }
  if (sh > 0) {
    if (sh > 1 << 24) {
      return Oop{};
    }
    magShl(A.d, static_cast<unsigned>(sh));
    return box(ctx, A);
  }
  const std::int64_t right = -sh;
  if (right > 1 << 24) {
    return A.neg ? Oop::fromSmallInteger(-1) : Oop::fromSmallInteger(0);
  }
  Big den;
  den.d.push_back(1);
  magShl(den.d, static_cast<unsigned>(right));
  Big q;
  Big r;
  floorDivMod(A, den, q, r);
  return box(ctx, q);
}

Oop gcd(CallContext& ctx, Oop a, Oop b) {
  Big A;
  Big B;
  if (!parse(ctx.heap, ctx.wk, a, A) || !parse(ctx.heap, ctx.wk, b, B)) {
    return Oop{};
  }
  A.neg = false;
  B.neg = false;
  while (!B.isZero()) {
    Big q;
    Big r;
    magDivMod(A.d, B.d, q.d, r.d);
    A.d = std::move(B.d);
    B.d = std::move(r.d);
  }
  return box(ctx, A);
}

Oop neg(CallContext& ctx, Oop a) {
  Big A;
  if (!parse(ctx.heap, ctx.wk, a, A)) {
    return Oop{};
  }
  if (!A.isZero()) {
    A.neg = !A.neg;
  }
  return box(ctx, A);
}

bool ratioToDouble(Heap& heap, WellKnown& wk, Oop num, Oop den, double* out) {
  if (num.isSmallInteger() && den == Oop::fromSmallInteger(1)) {
    // A SmallInteger has 63 bits; the conversion rounds to nearest even.
    *out = static_cast<double>(num.smallIntegerValue());
    return true;
  }
  Big n;
  Big d;
  if (!parse(heap, wk, num, n) || !parse(heap, wk, den, d) || d.isZero()) {
    return false;
  }
  if (n.isZero()) {
    *out = 0.0;
    return true;
  }
  const bool negative = n.neg != d.neg;
  double v = 0;
  if (d.d.size() == 1 && d.d[0] == 1) {
    v = roundToDouble(n.d, 0, false);
  } else {
    // The leading bit of n / d is at 2^e or 2^(e-1). Far outside the range the answer is known
    // without dividing (and without shifting by a huge count).
    const std::int64_t e = std::int64_t{magBitLength(n.d)} - magBitLength(d.d);
    if (e > 1025) {
      v = std::numeric_limits<double>::infinity();
    } else if (e < -1077) {
      v = 0.0;
    } else {
      // Scale so that the quotient has 66 or 67 bits; the remainder is the sticky bit.
      const std::int64_t shift = 66 - e;
      Digits a = n.d;
      Digits b = d.d;
      if (shift > 0) {
        magShl(a, static_cast<unsigned>(shift));
      } else if (shift < 0) {
        magShl(b, static_cast<unsigned>(-shift));
      }
      Digits q;
      Digits r;
      magDivMod(a, b, q, r);
      v = roundToDouble(q, -shift, !r.empty());
    }
  }
  *out = negative ? -v : v;
  return true;
}


bool compareRatioWithDouble(Heap& heap, WellKnown& wk, Oop num, Oop den, double d, int* out) {
  if (std::isnan(d)) {
    return false;
  }
  if (num.isSmallInteger() && den == Oop::fromSmallInteger(1)) {
    // Rounding to double is monotonic and d is a double: when the rounded i differs from d, it
    // is on the same side as i. When it equals d, d is an integer of at most 2^62 in magnitude,
    // so it converts back exactly and decides.
    const std::int64_t i = num.smallIntegerValue();
    const auto di = static_cast<double>(i);
    if (di != d) {
      *out = di < d ? -1 : 1;
      return true;
    }
    const auto t = static_cast<std::int64_t>(d);
    *out = i < t ? -1 : (i > t ? 1 : 0);
    return true;
  }
  Big n;
  Big q;
  if (!parse(heap, wk, num, n) || !parse(heap, wk, den, q) || !positiveDenominator(n, q)) {
    return false;
  }
  if (std::isinf(d)) {
    *out = d > 0 ? -1 : 1;
    return true;
  }
  // d = m · 2^e exactly, with m an integer below 2^53 in magnitude. Compare n · 2^-e with m · q
  // (e < 0), or n with m · q · 2^e.
  int e = 0;
  const double fraction = std::frexp(d, &e);
  const auto m = static_cast<std::int64_t>(std::ldexp(fraction, 53));
  e -= 53;
  Big left = n;
  Big right = mulBig(bigOf(m), q);
  if (e < 0) {
    magShl(left.d, static_cast<unsigned>(-e));
  } else {
    magShl(right.d, static_cast<unsigned>(e));
  }
  *out = cmpBig(left, right);
  return true;
}

bool compareRatios(Heap& heap, WellKnown& wk, Oop n1, Oop d1, Oop n2, Oop d2, int* out) {
  Big a;
  Big b;
  Big c;
  Big d;
  if (!parse(heap, wk, n1, a) || !parse(heap, wk, d1, b) || !parse(heap, wk, n2, c) ||
      !parse(heap, wk, d2, d) || !positiveDenominator(a, b) || !positiveDenominator(c, d)) {
    return false;
  }
  *out = cmpBig(mulBig(a, d), mulBig(c, b));
  return true;
}

}  // namespace LargeInteger
}  // namespace ao

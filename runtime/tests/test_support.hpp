#pragma once
#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace ao::Str {
Oop fromUtf8(Heap& heap, WellKnown& wk, std::string_view utf8);
std::string toUtf8(Heap& heap, Oop str);
}

struct Boot {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk;
  ao::ClassMethodCache cache;
  ao::CallContext ctx;
  Boot() : wk(heap, roots), ctx{heap, roots, wk, &cache} {
    cache.addRoots(roots);
    ao::Bootstrap::run(heap, roots, wk);
  }
};

inline ao::Oop send0(Boot& b, ao::Oop rcvr, const char* sel) {
  auto s = ao::Symbol::intern(b.wk, sel);
  return ao::send(b.ctx, rcvr, s, nullptr, 0, nullptr);
}
inline ao::Oop send1(Boot& b, ao::Oop rcvr, const char* sel, ao::Oop a0) {
  auto s = ao::Symbol::intern(b.wk, sel);
  return ao::send(b.ctx, rcvr, s, &a0, 1, nullptr);
}

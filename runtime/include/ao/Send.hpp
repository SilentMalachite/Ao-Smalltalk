#pragma once

#include "ao/NativeMethod.hpp"

namespace ao {

struct InlineCache {
  Oop cachedClass{};
  Oop cachedMethod{};
};

Oop send(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
         InlineCache* ic);
Oop sendSuper(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
              Oop methodClass);

}  // namespace ao

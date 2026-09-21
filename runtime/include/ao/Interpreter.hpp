#pragma once

#include "ao/NativeMethod.hpp"

#include <cstdint>

namespace ao {

class Interpreter {
 public:
  static Oop run(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                 Oop block);
};

Oop applyMethod(CallContext& ctx, Oop method, Oop receiver, const Oop* args, std::uint32_t argc,
                Oop block = Oop::nil());

}  // namespace ao

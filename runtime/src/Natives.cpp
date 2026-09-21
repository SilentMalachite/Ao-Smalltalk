#include "ao/Natives.hpp"

namespace ao {

Oop ao_SmallInteger_add(CallContext&, Oop receiver, const Oop* args, std::uint32_t argc) {
  if (argc != 1) return Oop{};
  if (!receiver.isSmallInteger() || !args[0].isSmallInteger()) return Oop{};
  std::int64_t sum = 0;
  if (__builtin_add_overflow(receiver.smallIntegerValue(), args[0].smallIntegerValue(), &sum)) {
    return Oop{};  // P4-03 replaces overflow with LargeInteger
  }
  constexpr std::int64_t kSmiMin = -(std::int64_t{1} << 62);
  constexpr std::int64_t kSmiMax = (std::int64_t{1} << 62) - 1;
  if (sum < kSmiMin || sum > kSmiMax) {
    return Oop{};  // P4-03 replaces overflow with LargeInteger
  }
  return Oop::fromSmallInteger(sum);
}

}  // namespace ao

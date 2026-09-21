#pragma once

#include "ao/Oop.hpp"

#include <cstdint>

namespace ao {
namespace Format {

inline constexpr std::int64_t kInstSizeMask = 0xFFFF;
inline constexpr std::int64_t kIndexable    = 1 << 16;
inline constexpr std::int64_t kBytes        = 1 << 17;

inline std::int64_t bits(Oop fmt) {
  return fmt.isSmallInteger() ? fmt.smallIntegerValue() : 0;
}

inline std::int64_t instSize(Oop fmt) { return bits(fmt) & kInstSizeMask; }

inline bool isIndexable(Oop fmt) { return (bits(fmt) & kIndexable) != 0; }

inline bool isBytes(Oop fmt) { return (bits(fmt) & kBytes) != 0; }

inline bool isPointers(Oop fmt) { return !isBytes(fmt); }

inline Oop make(std::int64_t namedInstSize, bool indexable, bool bytes) {
  std::int64_t v = namedInstSize & kInstSizeMask;
  if (indexable) {
    v |= kIndexable;
  }
  if (bytes) {
    v |= kBytes;
  }
  return Oop::fromSmallInteger(v);
}

}  // namespace Format
}  // namespace ao

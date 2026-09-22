#pragma once

#include "ao/Oop.hpp"

#include <cstddef>
#include <cstdint>

namespace ao {

struct ImageFormat {
  static constexpr std::uint16_t kImageVersion = 1;
  static constexpr std::uint16_t kImageEndianLittle = 1;
  static constexpr std::uint16_t kImagePointerBits = 64;
  static constexpr std::uint16_t kImageHeaderBytes = 48;
  static constexpr std::uint16_t kImageFillerBytes = 16;

  struct ImageHeader {
    std::uint16_t version = 0;
    std::uint16_t pointerBits = 0;
    std::uint16_t endian = 0;
    std::uint32_t heapBytes = 0;
    std::uint32_t wellKnownCount = 0;
    std::uint32_t extraCount = 0;
    std::uint32_t globalCount = 0;
    std::uint16_t nextHash = 0;
  };

  static bool encodeNonHeap(Oop o, std::uint64_t* bits);
  static bool decodeNonHeap(std::uint64_t bits, Oop* o);
  static void writeHeader(std::byte* dst, const ImageHeader& h);
  static bool readHeader(const std::byte* src, std::size_t n, ImageHeader* out);
  static void writeFiller(std::byte* dst);
};

}  // namespace ao

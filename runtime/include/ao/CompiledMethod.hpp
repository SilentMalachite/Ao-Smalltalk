#pragma once

#include "ao/Chunk.hpp"
#include "ao/NativeMethod.hpp"

#include <cstdint>
#include <vector>

namespace ao {
namespace compiler {
struct MethodImage;
}

inline constexpr std::uint32_t kCmSlotHeader      = 0;
inline constexpr std::uint32_t kCmSlotLiterals    = 1;
inline constexpr std::uint32_t kCmSlotBytes       = 2;
inline constexpr std::uint32_t kCmSlotNativeCode  = 3;
inline constexpr std::uint32_t kCmSlotSelector    = 4;
inline constexpr std::uint32_t kCmSlotMethodClass = 5;
inline constexpr std::uint32_t kCmSlotCount       = 6;

namespace CompiledMethod {
Oop create(CallContext& ctx, std::uint8_t numArgs, std::uint8_t numTemps, std::uint16_t prim,
           Oop literals, Oop bytes, Oop selector, Oop methodClass);
}

Oop boxMethodImage(CallContext& ctx, const compiler::MethodImage& image, Oop methodClass);
Oop installMethod(CallContext& ctx, Oop cls, const compiler::MethodImage& image);
bool applyChunks(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                 std::vector<compiler::CompileError>& errors);

}  // namespace ao

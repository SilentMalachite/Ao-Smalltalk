#pragma once

#include "ao/Chunk.hpp"
#include "ao/Context.hpp"

#include <vector>

namespace ao {
namespace compiler {
struct MethodImage;
}

Oop boxMethodImage(CallContext& ctx, const compiler::MethodImage& image, Oop methodClass);
Oop installMethod(CallContext& ctx, Oop cls, const compiler::MethodImage& image);
bool applyChunks(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                 std::vector<compiler::CompileError>& errors);

}  // namespace ao

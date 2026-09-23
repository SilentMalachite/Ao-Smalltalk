#pragma once

#include "ao/Chunk.hpp"
#include "ao/Context.hpp"

#include <filesystem>
#include <string_view>
#include <vector>

namespace ao {
namespace compiler {
struct MethodImage;
}

Oop boxMethodImage(CallContext& ctx, const compiler::MethodImage& image, Oop methodClass);
Oop installMethod(CallContext& ctx, Oop cls, const compiler::MethodImage& image);
// The name resolves to a class or metaclass (a Behavior), not Processor or another global.
bool namesBehavior(CallContext& ctx, std::string_view className);
bool acceptMethodSource(CallContext& ctx, std::string_view className, bool meta,
                        std::string_view source, compiler::CompileError* error);
bool acceptClassSource(CallContext& ctx, std::string_view source, compiler::CompileError* error);
bool applyChunks(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                 std::vector<compiler::CompileError>& errors);
bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors);
bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors);
bool fileInLoadOrder(CallContext& ctx, const std::filesystem::path& loadOrder,
                     std::vector<compiler::CompileError>& errors);

}  // namespace ao

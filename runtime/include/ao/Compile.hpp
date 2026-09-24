#pragma once

#include "ao/Chunk.hpp"
#include "ao/Context.hpp"

#include <filesystem>
#include <string>
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
// SPEC §3.12: a method-level error does not stop the file-in, but any error fails it. These
// answer false when they add an error to errors (a class definition that failed stops the rest).
bool applyChunks(CallContext& ctx, const std::vector<compiler::ChunkAction>& actions,
                 std::vector<compiler::CompileError>& errors);
bool fileInString(CallContext& ctx, std::string_view src,
                  std::vector<compiler::CompileError>& errors);
bool fileInFile(CallContext& ctx, const std::filesystem::path& path,
                std::vector<compiler::CompileError>& errors);
// SPEC §3.12: a file-in error. file names the file it happened in. method is `Class>>selector`
// (`Class class>>selector`) for a method-level error (a compile error, a refused native
// overwrite, a failed install) and empty for any other.
struct FileInError {
  std::string file;
  std::string method;
  compiler::CompileError error;
};
// SPEC §3.12: the methods a DEFERRED.md lists, as `Class>>selector`. A listing line is
// `Class>>selector: reason` or `Class class>>selector: reason`; any other line is a note. A file
// that cannot be read lists none.
std::vector<std::string> deferredMethods(const std::filesystem::path& deferredMd);
// Files in the files LOAD_ORDER lists, in order (SPEC §3.12). A method-level error does not stop
// it; an unreadable file or a chunk that stops fileInString does. An error of a method the
// DEFERRED.md beside LOAD_ORDER lists goes to deferred (when given), not to errors. False when
// LOAD_ORDER or a listed file cannot be read, or when errors gained any entry.
bool fileInLoadOrder(CallContext& ctx, const std::filesystem::path& loadOrder,
                     std::vector<FileInError>& errors,
                     std::vector<FileInError>* deferred = nullptr);

}  // namespace ao

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
// obj itself is a class or metaclass (a Behavior); namesBehavior for a class ID's class.
bool isBehaviorObject(CallContext& ctx, Oop obj);
bool acceptMethodSource(CallContext& ctx, std::string_view className, bool meta,
                        std::string_view source, compiler::CompileError* error);
// SPEC §3.10 ao_accept_method_id: acceptMethodSource for the class cls (a class or metaclass;
// Kernel-ness is cls itself). "missing class: <name>" when cls is no Behavior.
bool acceptMethodInto(CallContext& ctx, Oop cls, bool meta, std::string_view source,
                      compiler::CompileError* error);
bool acceptClassSource(CallContext& ctx, std::string_view source, compiler::CompileError* error);

// SPEC §3.9 削除. cls is the class a Browser class ID names. Every check runs before anything
// changes, and nothing here allocates on the heap (the selector is looked up, never interned).
// False with the reason, never empty, in *reason: "not a class: <name>" (cls is no Behavior),
// "selector not found: <Class>>><selector>" (an inherited or unknown selector, or a malformed
// dictionary), "native method removal refused: <Class>>><selector>"; <name> is cls's own name
// slot, <Class> that with " class" after it when meta. True: the method is out of its side's
// dictionary, the cache is invalidated for the selector, and its source entry is gone.
bool removeMethodOf(CallContext& ctx, Oop cls, bool meta, std::string_view selector,
                    std::string* reason);

// SPEC §3.9 削除. cls is the class a Browser class ID names; <name> is its own name slot ("an
// unnamed class" when that is no String or empty). Every
// check runs before anything changes; nothing here allocates on the heap. False with the reason in
// *reason: "not a class: <name>", "class removal refused: <name> is a fixed global", "... is a
// kernel class" (by identity), "... is not bound to this class" (Smalltalk's <name> is another
// object), "... has subclass <Sub>" (the smallest name of its live subclasses) or "... has an
// unnamed subclass". True: <name>'s binding is out of Smalltalk (the globals version moved), the
// whole cache is dropped, and the source entries of the class's methods on both sides are gone.
// The class object, its metaclass, its dictionaries and its aliases are untouched.
bool removeClassOf(CallContext& ctx, Oop cls, std::string* reason);
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

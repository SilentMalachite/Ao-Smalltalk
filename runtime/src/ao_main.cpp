#include "ao/Runtime.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Compile.hpp"
#include "ao/Heap.hpp"
#include "ao/Image.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
#include "ao/Send.hpp"
#include "ao/TestRunner.hpp"
#include "ao/Vendor.hpp"
#include "ao/WellKnown.hpp"

#include <mach-o/dyld.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace {

std::string testsBesideExecutable() {
  namespace fs = std::filesystem;
  char raw[PATH_MAX];
  auto n = static_cast<uint32_t>(sizeof(raw));
  if (_NSGetExecutablePath(raw, &n) != 0) {
    return {};
  }
  std::error_code ec;
  fs::path dir = fs::canonical(fs::path(raw), ec).parent_path();
  if (ec) {
    return {};
  }
  for (int i = 0; i < 32; ++i) {
    const fs::path candidate = dir / "image" / "tests";
    if (fs::is_directory(candidate, ec) && !ec) {
      return candidate.string();
    }
    const fs::path parent = dir.parent_path();
    if (parent.empty() || parent == dir) {
      break;
    }
    dir = parent;
  }
  return {};
}

std::string trimLine(std::string line) {
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
  std::size_t begin = 0;
  while (begin < line.size() && (line[begin] == ' ' || line[begin] == '\t')) {
    ++begin;
  }
  std::size_t end = line.size();
  while (end > begin && (line[end - 1] == ' ' || line[end - 1] == '\t')) {
    --end;
  }
  return line.substr(begin, end - begin);
}

int runExtractVendor(int argc, char** argv) {
  if (argc != 5) {
    std::fputs("ao: usage: ao extract-vendor <changes> <allowlist> <out-dir>\n", stderr);
    return 2;
  }
  const std::filesystem::path changesPath(argv[2]);
  const std::filesystem::path allowPath(argv[3]);
  const std::filesystem::path outDir(argv[4]);

  std::ifstream changesIn(changesPath, std::ios::binary);
  if (!changesIn) {
    std::fputs("ao: cannot read changes\n", stderr);
    return 1;
  }
  std::stringstream changesBuf;
  changesBuf << changesIn.rdbuf();
  if (!changesIn && !changesIn.eof()) {
    std::fputs("ao: cannot read changes\n", stderr);
    return 1;
  }
  const std::string changes = changesBuf.str();

  std::ifstream allowIn(allowPath);
  if (!allowIn) {
    std::fputs("ao: cannot read allowlist\n", stderr);
    return 1;
  }
  std::vector<std::string> allowlist;
  std::string line;
  while (std::getline(allowIn, line)) {
    line = trimLine(std::move(line));
    if (!line.empty()) {
      allowlist.push_back(std::move(line));
    }
  }
  if (!allowIn.eof()) {
    std::fputs("ao: cannot read allowlist\n", stderr);
    return 1;
  }

  const ao::VendorExtractResult result = ao::extractVendor(changes, allowlist);

  std::error_code ec;
  std::filesystem::create_directories(outDir, ec);
  if (ec) {
    std::fputs("ao: cannot create out-dir\n", stderr);
    return 1;
  }
  for (const ao::VendorClassFile& file : result.files) {
    const std::filesystem::path path = outDir / (file.className + ".st");
    std::ofstream out(path, std::ios::binary);
    if (!out) {
      std::fputs("ao: cannot write class file\n", stderr);
      return 1;
    }
    out << file.chunkText;
    if (!out) {
      std::fputs("ao: cannot write class file\n", stderr);
      return 1;
    }
  }
  for (const std::string& note : result.notes) {
    std::puts(note.c_str());
  }
  return 0;
}

// SPEC §3.12: one line per counted file-in error, `<file>:<start>-<end>: <message>`.
void printFileInErrors(const std::vector<ao::FileInError>& errors) {
  for (const auto& e : errors) {
    std::fprintf(stderr, "%s:%u-%u: %s\n", e.file.c_str(), e.error.span.start, e.error.span.end,
                 e.error.message.c_str());
  }
}

int runFileIn(int argc, char** argv) {
  if (argc < 3) {
    std::fputs("ao: usage: ao filein <file.st>\n"
               "       ao filein --load-order <LOAD_ORDER>\n",
               stderr);
    return 2;
  }
  const bool loadOrder = std::strcmp(argv[2], "--load-order") == 0;
  if (loadOrder) {
    if (argc != 4) {
      std::fputs("ao: usage: ao filein --load-order <LOAD_ORDER>\n", stderr);
      return 2;
    }
  } else if (argc != 3) {
    std::fputs("ao: usage: ao filein <file.st>\n", stderr);
    return 2;
  }

  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::Bootstrap::run(heap, roots, wk);

  // SPEC §3.12: every counted error, and exit 1 when there is one. The errors of the methods the
  // DEFERRED.md beside LOAD_ORDER lists are not counted. A single file has no DEFERRED.md.
  std::vector<ao::FileInError> errors;
  bool ok = false;
  if (loadOrder) {
    ok = ao::fileInLoadOrder(ctx, argv[3], errors);
  } else {
    std::vector<ao::compiler::CompileError> found;
    ok = ao::fileInFile(ctx, argv[2], found);
    for (auto& e : found) {
      errors.push_back(ao::FileInError{argv[2], {}, std::move(e)});
    }
  }
  printFileInErrors(errors);
  return ok && errors.empty() ? 0 : 1;
}

int bootAndRunTests(const std::string& dir) {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};
  ao::Bootstrap::run(heap, roots, wk);
  return ao::runSmalltalkTests(ctx, dir);
}

int imageUsage() {
  std::fputs("ao: usage: ao image save <path>\n"
             "       ao image save --load-order <LOAD_ORDER> <path>\n"
             "       ao image load <path>\n",
             stderr);
  return 2;
}

int runImage(int argc, char** argv) {
  if (argc < 3) {
    return imageUsage();
  }
  const bool isSave = std::strcmp(argv[2], "save") == 0;
  const bool isLoad = std::strcmp(argv[2], "load") == 0;
  if (!isSave && !isLoad) {
    return imageUsage();
  }

  const char* loadOrder = nullptr;
  const char* path = nullptr;
  if (isLoad) {
    if (argc != 4) {
      return imageUsage();
    }
    path = argv[3];
  } else if (argc == 6 && std::strcmp(argv[3], "--load-order") == 0) {
    loadOrder = argv[4];
    path = argv[5];
  } else if (argc == 4 && std::strcmp(argv[3], "--load-order") != 0) {
    path = argv[3];
  } else {
    return imageUsage();
  }

  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk(heap, roots);
  ao::ClassMethodCache cache;
  cache.addRoots(roots);
  ao::CallContext ctx{heap, roots, wk, &cache};

  if (isSave) {
    ao::Bootstrap::run(heap, roots, wk);
    if (loadOrder != nullptr) {
      // SPEC §3.12: a failed file-in reports its errors like ao filein and writes no image.
      std::vector<ao::FileInError> errors;
      const bool ok = ao::fileInLoadOrder(ctx, loadOrder, errors);
      printFileInErrors(errors);
      if (!ok) {
        return 1;
      }
    }
    if (!ao::Image::save(heap, roots, wk, path)) {
      std::fputs("ao: image save failed\n", stderr);
      return 1;
    }
    return 0;
  }

  if (!ao::Image::load(heap, roots, wk, path)) {
    std::fputs("ao: image load failed\n", stderr);
    return 1;
  }
  ao::Oop arg = ao::Oop::fromSmallInteger(2);
  auto sel = wk.intern("+");
  auto three = ao::send(ctx, ao::Oop::fromSmallInteger(1), sel, &arg, 1, nullptr);
  auto isNil = ao::send(ctx, ao::Oop::nil(), wk.intern("isNil"), nullptr, 0, nullptr);
  if (!three.isSmallInteger() || three.smallIntegerValue() != 3 || !isNil.isTrue()) {
    std::fputs("ao: image load failed\n", stderr);
    return 1;
  }
  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc >= 2 && std::strcmp(argv[1], "--help") == 0) {
    std::puts(
        "ao — Ao Smalltalk CLI\n  --help\n  --version\n  --test\n  extract-vendor <changes> <allowlist> <out-dir>\n"
        "  filein <file.st>\n  filein --load-order <LOAD_ORDER>\n"
        "  image save <path>\n  image save --load-order <LOAD_ORDER> <path>\n"
        "  image load <path>");
    return 0;
  }
  if (argc >= 2 && std::strcmp(argv[1], "--version") == 0) {
    char buf[64];
    if (ao::version_string(buf, sizeof(buf)) != 0) {
      return 1;
    }
    std::puts(buf);
    return 0;
  }
  if (argc >= 2 && std::strcmp(argv[1], "filein") == 0) {
    return runFileIn(argc, argv);
  }
  if (argc >= 2 && std::strcmp(argv[1], "image") == 0) {
    return runImage(argc, argv);
  }
  if (argc >= 2 && std::strcmp(argv[1], "--test") == 0) {
    std::string dir;
    if (argc >= 3) {
      dir = argv[2];
    } else {
      dir = testsBesideExecutable();
      if (dir.empty()) {
        std::fputs("ao: image/tests not found\n", stderr);
        return 1;
      }
    }
    return bootAndRunTests(dir);
  }
  if (argc >= 2 && std::strcmp(argv[1], "extract-vendor") == 0) {
    return runExtractVendor(argc, argv);
  }
  std::fputs("ao: no command (P0 stub). Try --help.\n", stderr);
  return 2;
}

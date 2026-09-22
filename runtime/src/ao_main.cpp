#include "ao/Runtime.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
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

}  // namespace

int main(int argc, char** argv) {
  if (argc >= 2 && std::strcmp(argv[1], "--help") == 0) {
    std::puts(
        "ao — Ao Smalltalk CLI\n  --help\n  --version\n  --test\n  extract-vendor <changes> <allowlist> <out-dir>");
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

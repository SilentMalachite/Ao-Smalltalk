#include "ao/Runtime.hpp"

#include "ao/Bootstrap.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
#include "ao/TestRunner.hpp"
#include "ao/WellKnown.hpp"

#include <mach-o/dyld.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

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
    std::puts("ao — Ao Smalltalk CLI\n  --help\n  --version\n  --test");
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
  std::fputs("ao: no command (P0 stub). Try --help.\n", stderr);
  return 2;
}

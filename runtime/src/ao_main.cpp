#include "ao/Runtime.hpp"

#include <cstdio>
#include <cstring>

int main(int argc, char** argv) {
  if (argc >= 2 && std::strcmp(argv[1], "--help") == 0) {
    std::puts("ao — Ao Smalltalk CLI\n  --help\n  --version");
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
  std::fputs("ao: no command (P0 stub). Try --help.\n", stderr);
  return 2;
}

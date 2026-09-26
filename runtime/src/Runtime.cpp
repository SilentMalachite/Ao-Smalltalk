#include "ao/Runtime.hpp"
#include "Session.hpp"

#include <cstdio>

namespace ao {

int boot() { return sessionBoot(); }

int shutdown() { return sessionShutdown(); }

int version_string(char* buf, int len) {
  if (buf == nullptr || len <= 0) {
    return 1;
  }
  int n = std::snprintf(buf, static_cast<size_t>(len), "1.0.0");
  return n < 0 || n >= len ? 1 : 0;
}

}  // namespace ao

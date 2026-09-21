#include "ao/Runtime.hpp"

#include <cstdio>

namespace ao {

int boot() { return 0; }

int shutdown() { return 0; }

int version_string(char* buf, int len) {
  if (buf == nullptr || len <= 0) {
    return 1;
  }
  int n = std::snprintf(buf, static_cast<size_t>(len), "0.0.0-p0");
  return n < 0 || n >= len ? 1 : 0;
}

}  // namespace ao

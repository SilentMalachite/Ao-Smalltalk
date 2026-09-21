#include "ao/Compiler.hpp"

#include <cstring>
#include <gtest/gtest.h>

TEST(CompilerSmoke, VersionIsNonEmpty) {
  const char* v = ao::compiler::version();
  ASSERT_NE(v, nullptr);
  EXPECT_GT(std::strlen(v), 0u);
}

#include "ao/Compiler.hpp"

#include <cstring>
#include <gtest/gtest.h>

TEST(CompilerSmoke, VersionIsNonEmpty) {
  const char* v = ao::compiler::version();
  ASSERT_NE(v, nullptr);
  EXPECT_GT(std::strlen(v), 0u);
}

// SPEC §2.4: ao::compiler::version は ao::version_string と同じ版。
TEST(CompilerSmoke, VersionIsReleaseOneZeroZero) {
  EXPECT_STREQ("1.0.0", ao::compiler::version());
}

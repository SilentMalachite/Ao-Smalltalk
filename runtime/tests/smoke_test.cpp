#include "ao/Runtime.hpp"

#include <cstring>
#include <gtest/gtest.h>

TEST(Smoke, BootAndShutdownReturnZero) {
  EXPECT_EQ(0, ao::boot());
  EXPECT_EQ(0, ao::shutdown());
}

TEST(Smoke, VersionStringIsNonEmpty) {
  char buf[64];
  ASSERT_EQ(0, ao::version_string(buf, sizeof(buf)));
  EXPECT_GT(std::strlen(buf), 0u);
}

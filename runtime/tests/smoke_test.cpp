#include "ao/Runtime.hpp"
#include "ao_abi.h"

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

TEST(AbiSmoke, BootVersionShutdown) {
  EXPECT_EQ(AO_OK, ao_runtime_boot());
  char buf[64];
  EXPECT_EQ(AO_OK, ao_version(buf, sizeof(buf)));
  EXPECT_GT(std::strlen(buf), 0u);
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
}

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

// 06 Low / SPEC §3.10: 切り詰めたら AO_ERR_RANGE（buf は NUL で終わる）。buf が NULL か
// buf_len が 1 未満なら AO_ERR。
TEST(AbiSmoke, VersionTruncationIsRangeError) {
  char buf[4] = {'x', 'x', 'x', 'x'};
  EXPECT_EQ(AO_ERR_RANGE, ao_version(buf, 4));
  EXPECT_EQ('\0', buf[3]);
  EXPECT_EQ(3u, std::strlen(buf));
  char one[1] = {'x'};
  EXPECT_EQ(AO_ERR_RANGE, ao_version(one, 1));
  EXPECT_EQ('\0', one[0]);
  EXPECT_EQ(AO_ERR, ao_version(nullptr, 4));
  EXPECT_EQ(AO_ERR, ao_version(buf, 0));
  EXPECT_EQ(AO_ERR, ao_version(buf, -1));
}

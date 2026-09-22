#include "ao_abi.h"

#include <cstdio>
#include <gtest/gtest.h>

class SessionAbi : public ::testing::Test {
 protected:
  void TearDown() override { ao_runtime_shutdown(); }
};

TEST_F(SessionAbi, BootThenImageRoundTripKeepsOnePlusTwo) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  const char* path = "session-abi-roundtrip.aoimage";
  ASSERT_EQ(AO_OK, ao_image_save(path));
  ao_runtime_shutdown();
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  ASSERT_EQ(AO_OK, ao_image_load(path));
  ao_runtime_shutdown();
  std::remove(path);
}

TEST_F(SessionAbi, SecondBootFailsUntilShutdown) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_runtime_boot());
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
  EXPECT_EQ(AO_OK, ao_runtime_shutdown());
}

TEST_F(SessionAbi, MissingLoadOrderIsError) {
  ASSERT_EQ(AO_OK, ao_runtime_boot());
  EXPECT_EQ(AO_ERR, ao_filein_load_order("/no/such/LOAD_ORDER"));
  ao_runtime_shutdown();
}

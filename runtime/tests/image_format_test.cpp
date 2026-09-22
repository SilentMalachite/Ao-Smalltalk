#include "ao/ImageFormat.hpp"
#include "ao/Oop.hpp"

#include <gtest/gtest.h>
#include <vector>

TEST(ImageFormat, ImmediateBitsRoundTrip) {
  const ao::Oop samples[] = {
      ao::Oop{},
      ao::Oop::nil(),
      ao::Oop::false_(),
      ao::Oop::true_(),
      ao::Oop::fromSmallInteger(0),
      ao::Oop::fromSmallInteger(1),
      ao::Oop::fromSmallInteger(-1),
      ao::Oop::fromCharacter(U'あ'),
  };
  for (ao::Oop o : samples) {
    std::uint64_t bits = 0;
    ASSERT_TRUE(ao::ImageFormat::encodeNonHeap(o, &bits));
    ao::Oop back;
    ASSERT_TRUE(ao::ImageFormat::decodeNonHeap(bits, &back));
    EXPECT_EQ(o, back);
    EXPECT_EQ(o.bits(), bits);
  }
  EXPECT_EQ(0u, ao::Oop{}.bits());
  EXPECT_EQ(0x4u, ao::Oop::nil().bits());
  EXPECT_EQ(0x0Cu, ao::Oop::false_().bits());
  EXPECT_EQ(0x14u, ao::Oop::true_().bits());
}

TEST(ImageFormat, HeapShapedBitsAreNotImmediates) {
  ao::Oop o;
  EXPECT_FALSE(ao::ImageFormat::decodeNonHeap(16, &o));
  EXPECT_FALSE(ao::ImageFormat::decodeNonHeap(0x18, &o));
}

TEST(ImageFormat, HeaderRoundTripAndRejects) {
  ao::ImageFormat::ImageHeader h;
  h.version = ao::ImageFormat::kImageVersion;
  h.pointerBits = ao::ImageFormat::kImagePointerBits;
  h.endian = ao::ImageFormat::kImageEndianLittle;
  h.heapBytes = 16;
  h.wellKnownCount = 127;
  h.extraCount = 0;
  h.globalCount = 57;
  h.nextHash = 1;
  std::vector<std::byte> buf(ao::ImageFormat::kImageHeaderBytes);
  ao::ImageFormat::writeHeader(buf.data(), h);
  EXPECT_EQ(std::byte{'A'}, buf[0]);
  EXPECT_EQ(std::byte{'O'}, buf[1]);
  EXPECT_EQ(std::byte{'I'}, buf[2]);
  EXPECT_EQ(std::byte{'M'}, buf[3]);
  ao::ImageFormat::ImageHeader out;
  ASSERT_TRUE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out));
  EXPECT_EQ(127u, out.wellKnownCount);
  EXPECT_EQ(1u, out.nextHash);

  buf[0] = std::byte{'X'};
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out));
  ao::ImageFormat::writeHeader(buf.data(), h);
  buf[4] = std::byte{2};
  buf[5] = std::byte{0};
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out));

  std::byte filler[16];
  ao::ImageFormat::writeFiller(filler);
  const std::byte wantFiller[16] = {
      std::byte{0x04}, std::byte{0}, std::byte{0}, std::byte{0},
      std::byte{0},    std::byte{0}, std::byte{0}, std::byte{0},
      std::byte{0},    std::byte{0}, std::byte{0}, std::byte{0},
      std::byte{0x01}, std::byte{0}, std::byte{0}, std::byte{0},
  };
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(wantFiller[i], filler[i]) << i;
  }
  ao::Oop klass;
  ASSERT_TRUE(ao::ImageFormat::decodeNonHeap(0x4u, &klass));
  EXPECT_TRUE(klass.isNil());
}

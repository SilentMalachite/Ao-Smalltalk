#include "ao/ImageFormat.hpp"
#include "ao/Oop.hpp"

#include <gtest/gtest.h>
#include <string>
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
  std::string reason;
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("not an Ao image", reason);
  ao::ImageFormat::writeHeader(buf.data(), h);
  buf[4] = std::byte{4};
  buf[5] = std::byte{0};
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("unsupported image version 4", reason);
  ao::ImageFormat::writeHeader(buf.data(), h);
  buf[6] = std::byte{32};
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("unsupported image format", reason);
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), 8, &out, &reason));
  EXPECT_EQ("not an Ao image", reason);

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

// B4 review (Codex P2) / SPEC §3.11: 失敗シナリオ。版が 1 のままで、B4 より前の旧形式と壊れた
// ファイルが区別できなかった。版 1 はヘッダの段階で理由付きで拒む。B9 で Dictionary と Set が
// ハッシュ表になり、形式の版は 3 になった。版 2 も同じく拒む。
TEST(ImageFormat, VersionOneIsRefusedWithReason) {
  EXPECT_EQ(3u, ao::ImageFormat::kImageVersion);
  ao::ImageFormat::ImageHeader h;
  h.version = 1;
  h.pointerBits = ao::ImageFormat::kImagePointerBits;
  h.endian = ao::ImageFormat::kImageEndianLittle;
  h.heapBytes = 16;
  h.wellKnownCount = 127;
  h.globalCount = 57;
  std::vector<std::byte> buf(ao::ImageFormat::kImageHeaderBytes);
  ao::ImageFormat::writeHeader(buf.data(), h);
  ao::ImageFormat::ImageHeader out;
  std::string reason;
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("unsupported image version 1", reason);
  h.version = 2;
  ao::ImageFormat::writeHeader(buf.data(), h);
  EXPECT_FALSE(ao::ImageFormat::readHeader(buf.data(), buf.size(), &out, &reason));
  EXPECT_EQ("unsupported image version 2", reason);
}

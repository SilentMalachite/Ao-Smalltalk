#include "test_support.hpp"

#include "ao/Image.hpp"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(ImageSave, WritesAoimAndKeepsSourceRunnable) {
  Boot b;
  const auto path = std::filesystem::path(testing::TempDir()) / "save.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));

  std::ifstream in(path, std::ios::binary);
  std::vector<char> bytes((std::istreambuf_iterator<char>(in)), {});
  ASSERT_GE(bytes.size(), 48u);
  EXPECT_EQ('A', bytes[0]);
  EXPECT_EQ('O', bytes[1]);
  EXPECT_EQ('I', bytes[2]);
  EXPECT_EQ('M', bytes[3]);
  const std::string blob(bytes.begin(), bytes.end());
  EXPECT_NE(std::string::npos, blob.find("ao_Object_identityEquals"));

  const auto host = reinterpret_cast<std::uintptr_t>(b.wk.objectClass.heapPointer());
  std::uint64_t needle = static_cast<std::uint64_t>(host);
  bool foundHost = false;
  for (std::size_t i = 0; i + 8 <= bytes.size(); i += 1) {
    std::uint64_t word = 0;
    std::memcpy(&word, bytes.data() + i, 8);
    if (word == needle) foundHost = true;
  }
  EXPECT_FALSE(foundHost);

  auto three = send1(b, ao::Oop::fromSmallInteger(1), "+", ao::Oop::fromSmallInteger(2));
  EXPECT_TRUE(three.isSmallInteger());
  EXPECT_EQ(3, three.smallIntegerValue());
}

TEST(ImageSave, WeakUnrootedReferentStaysInSourceSlot) {
  ao::Oop weak;
  Boot b;
  weak = b.heap.allocate(ao::Oop::nil(), 1, ao::kFlagWeak);
  ao::Oop referent = b.heap.allocate(ao::Oop::nil(), 1, 0);
  ASSERT_TRUE(weak.isHeap());
  ASSERT_TRUE(referent.isHeap());
  b.heap.slotAtPut(weak, 0, referent);
  b.roots.add(&weak);

  const auto path = std::filesystem::path(testing::TempDir()) / "weak.aoimage";
  ASSERT_TRUE(ao::Image::save(b.heap, b.roots, b.wk, path.string()));
  EXPECT_EQ(referent, b.heap.slotAt(weak, 0));
}

#pragma once

#include <cassert>
#include <cstdint>

namespace ao {

// SmallInteger の範囲（63 ビットの 2 の補数）。
inline constexpr std::int64_t kSmiMin = -(std::int64_t{1} << 62);
inline constexpr std::int64_t kSmiMax = (std::int64_t{1} << 62) - 1;

class Oop {
 public:
  static constexpr std::uint64_t kSmiTag = 0x1;
  static constexpr std::uint64_t kCharTag = 0x2;
  static constexpr std::uint64_t kImmTag = 0x4;
  static constexpr std::uint64_t kLow3 = 0x7;

  static Oop fromSmallInteger(std::int64_t value) {
    assert(value >= kSmiMin && value <= kSmiMax && "fromSmallInteger: value out of range");
    Oop o;
    o.raw_ = (static_cast<std::uint64_t>(value) << 1) | kSmiTag;
    return o;
  }
  static Oop fromCharacter(char32_t scalar) {
    Oop o;
    o.raw_ = (static_cast<std::uint64_t>(scalar) << 2) | kCharTag;
    return o;
  }
  static Oop nil() { return immediate(0); }
  static Oop false_() { return immediate(1); }
  static Oop true_() { return immediate(2); }
  static Oop fromHeap(void* ptr) {
    Oop o;
    o.raw_ = reinterpret_cast<std::uintptr_t>(ptr);
    return o;
  }

  bool isSmallInteger() const { return (raw_ & kSmiTag) != 0; }
  bool isCharacter() const { return (raw_ & 0x3) == kCharTag; }
  bool isImmediate() const { return (raw_ & kLow3) == kImmTag; }
  bool isNil() const { return raw_ == immediate(0).raw_; }
  bool isTrue() const { return raw_ == immediate(2).raw_; }
  bool isFalse() const { return raw_ == immediate(1).raw_; }
  bool isEmpty() const { return raw_ == 0; }
  bool isHeap() const { return (raw_ & kLow3) == 0 && raw_ != 0; }

  std::int64_t smallIntegerValue() const {
    return static_cast<std::int64_t>(raw_) >> 1;
  }
  char32_t characterValue() const { return static_cast<char32_t>(raw_ >> 2); }
  void* heapPointer() const { return reinterpret_cast<void*>(raw_); }

  bool operator==(Oop other) const { return raw_ == other.raw_; }
  bool operator!=(Oop other) const { return raw_ != other.raw_; }

  std::uint64_t bits() const { return raw_; }
  static Oop fromBits(std::uint64_t raw) {
    Oop o;
    o.raw_ = raw;
    return o;
  }

 private:
  static Oop immediate(std::uint64_t kind) {
    Oop o;
    o.raw_ = (kind << 3) | kImmTag;
    return o;
  }
  std::uint64_t raw_ = 0;
};

static_assert(sizeof(Oop) == 8);

}  // namespace ao

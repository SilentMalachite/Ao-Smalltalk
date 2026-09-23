#pragma once

#include "ao/Oop.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ao {

class Roots {
 public:
  void add(Oop* slot);
  void remove(Oop* slot);

  // Registers n contiguous slots as one root range. Ranges are released LIFO.
  void pushRange(Oop* first, std::size_t n);
  void popRange(Oop* first, std::size_t n);

  // Host handle table. P1: structure only; AppKit は載せない。
  std::uint32_t pushHandle(Oop obj);
  void dropHandle(std::uint32_t index);
  Oop handleAt(std::uint32_t index) const;

  using VisitFn = void (*)(void* ctx, Oop* slot);
  using StackWalker = void (*)(void* ctx, VisitFn visit, void* visitCtx);
  void setStackWalker(StackWalker walker, void* ctx);

  void visitAll(VisitFn visit, void* ctx);

 private:
  struct Range {
    Oop* first;
    std::size_t n;
  };
  std::vector<Oop*> slots_;
  std::vector<Range> ranges_;
  std::vector<Oop> handles_;
  std::vector<std::uint8_t> live_;
  std::vector<std::uint32_t> free_;
  StackWalker walker_ = nullptr;
  void* walkerCtx_ = nullptr;
};

}  // namespace ao

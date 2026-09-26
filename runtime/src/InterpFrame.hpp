#pragma once

// The C++ side of one interpreted activation (SPEC §3.4, §3.13). Not a public header: only the
// interpreter builds these, and the debugger's capture (DebugSnapshot.cpp) reads them.

#include "ao/Oop.hpp"
#include "ao/Roots.hpp"

#include <cstdint>
#include <deque>
#include <memory>

namespace ao {

struct OperandStack {
  Roots* roots = nullptr;
  std::deque<Oop> slots;
  explicit OperandStack(Roots& r) : roots(&r) {}
  OperandStack(const OperandStack&) = delete;
  OperandStack& operator=(const OperandStack&) = delete;
  ~OperandStack() {
    while (!slots.empty()) {
      roots->remove(&slots.back());
      slots.pop_back();
    }
  }
  void push(Oop v) {
    slots.push_back(v);
    roots->add(&slots.back());
  }
  bool pop(Oop* out) {
    if (slots.empty()) {
      return false;
    }
    *out = slots.back();
    roots->remove(&slots.back());
    slots.pop_back();
    return true;
  }
  bool top(Oop* out) const {
    if (slots.empty()) {
      return false;
    }
    *out = slots.back();
    return true;
  }
  // The value fromTop slots below the top (0 is the top), left on the stack.
  bool peek(std::uint32_t fromTop, Oop* out) const {
    if (fromTop >= slots.size()) {
      return false;
    }
    *out = slots[slots.size() - 1 - fromTop];
    return true;
  }

  // Overwrites the top in place. Its slot stays rooted, so Roots is not touched.
  bool replaceTop(Oop v) {
    if (slots.empty()) {
      return false;
    }
    slots.back() = v;
    return true;
  }
  std::uint32_t depth() const { return static_cast<std::uint32_t>(slots.size()); }
};

struct Temps {
  Roots* roots = nullptr;
  std::unique_ptr<Oop[]> slots;
  std::uint32_t n = 0;
  Temps(Roots& r, std::uint32_t count) : roots(&r), n(count) {
    if (n == 0) {
      return;
    }
    slots.reset(new Oop[n]);
    for (std::uint32_t i = 0; i < n; ++i) {
      slots[i] = Oop::nil();
      roots->add(&slots[i]);
    }
  }
  ~Temps() {
    if (!slots) {
      return;
    }
    for (std::uint32_t i = 0; i < n; ++i) {
      roots->remove(&slots[i]);
    }
  }
  Temps(const Temps&) = delete;
  Temps& operator=(const Temps&) = delete;
  bool at(std::uint32_t i, Oop* out) const {
    if (i >= n) {
      return false;
    }
    *out = slots[i];
    return true;
  }
  bool put(std::uint32_t i, Oop v) {
    if (i >= n) {
      return false;
    }
    slots[i] = v;
    return true;
  }
};

// One interpreted activation. method, receiver and context are rooted by the interpreter
// (FieldRoots). SPEC §3.13: while its temps and operand stack exist, the frame is linked into its
// process's chain (CallContext::topFrame, through prev). Natives are never linked.
struct Frame {
  Oop method{};
  Oop receiver{};
  Oop context{};
  std::uint32_t pc = 0;
  bool isBlock = false;
  // The chain, set by the interpreter's link guard: the caller's frame (null at the outermost
  // activation of the process), and this activation's temps and operand stack.
  Frame* prev = nullptr;
  const Temps* temps = nullptr;
  const OperandStack* stack = nullptr;
  // The send in flight (performSend): pointers into its rooted slots, so they follow GC. A null
  // sendReceiver means none. sendArgs is null when sendArgc is 0.
  const Oop* sendSelector = nullptr;
  const Oop* sendReceiver = nullptr;
  const Oop* sendArgs = nullptr;
  std::uint32_t sendArgc = 0;
};

}  // namespace ao

#pragma once

// SPEC §3.13 デバッガ: the capture of an aborting evaluation's stack, and the one shape the
// debugger reads frames through (a snapshot in P10, a halted process's live chain in P11).

#include "ao/Oop.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ao {

struct CallContext;
class Roots;

// Frame kinds (SPEC §3.13 フレームの種類とラベル, §3.10 ao_debug_frame_kind).
inline constexpr int kDebugFrameMethod = 0;
inline constexpr int kDebugFrameBlock = 1;
inline constexpr int kDebugFrameNative = 2;  // synthesized: a failed native, or a DNU (no method)

// Told by abortEvaluation when an abort starts (SPEC §3.13 捕捉), while the chain
// (CallContext::topFrame) is still whole. Not told for abandon, for the quiet aborts
// (abortEvaluationQuiet), or inside a cleanup of an aborting evaluation. It must not allocate on
// the Smalltalk heap (no GC) and must not unwind.
class DebugSink {
 public:
  virtual ~DebugSink() = default;
  virtual void onAbort(CallContext& ctx) noexcept = 0;
};

// Frames, innermost first (index 0). Out of range: kind -1, the empty Oop, 0.
class DebugFrames {
 public:
  virtual ~DebugFrames() = default;
  virtual std::uint32_t count() const = 0;
  virtual int kind(std::uint32_t i) const = 0;
  // The CompiledMethod (a block's own for a block frame), the NativeMethod of a synthesized
  // native frame, or nil for a synthesized DNU frame.
  virtual Oop method(std::uint32_t i) const = 0;
  virtual Oop receiver(std::uint32_t i) const = 0;
  // The pc of the frame's context (the start of the bytecode it runs: a Send's own); 0 for a
  // synthesized frame.
  virtual std::uint32_t pc(std::uint32_t i) const = 0;
  // The selector of the send in flight in the frame (nil when none); for a synthesized frame,
  // the selector it was sent.
  virtual Oop selector(std::uint32_t i) const = 0;
  // Interpreted frames: the temps (arguments first, then temps, then a block's copied values).
  // Synthesized frames: the arguments of the send.
  virtual std::uint32_t tempCount(std::uint32_t i) const = 0;
  virtual Oop temp(std::uint32_t i, std::uint32_t j) const = 0;
};

// A copy of one process's chain at the start of an abort. The values are GC roots (roots.add,
// one slot each) until clear() or destruction, which remove them in reverse order. Its Roots must
// outlive it.
class DebugSnapshot final : public DebugFrames {
 public:
  // The innermost frames copied (SPEC §3.13); total() counts them all.
  static constexpr std::uint32_t kMaxFrames = 256;

  explicit DebugSnapshot(Roots& roots) noexcept : roots_(roots) {}
  ~DebugSnapshot() override { clear(); }
  DebugSnapshot(const DebugSnapshot&) = delete;
  DebugSnapshot& operator=(const DebugSnapshot&) = delete;

  // Replaces what it holds by ctx's chain, reason and process. Walks the chain iteratively,
  // copies into C++ memory, then roots the copies: no Smalltalk allocation, no GC. When C++
  // memory runs out the capture is dropped (false; it is then empty).
  bool capture(CallContext& ctx) noexcept;
  void clear() noexcept;
  // Nothing captured (a capture with no interpreted frame is not empty: it has a reason).
  bool empty() const noexcept { return !held_; }
  // Every slot it registered, for what must not trace them (image save, the shape change's
  // liveness; P10-04 methodSourceRootSlots).
  std::vector<const Oop*> rootSlots() const;

  std::uint32_t count() const override { return static_cast<std::uint32_t>(frames_.size()); }
  int kind(std::uint32_t i) const override;
  Oop method(std::uint32_t i) const override;
  Oop receiver(std::uint32_t i) const override;
  std::uint32_t pc(std::uint32_t i) const override;
  Oop selector(std::uint32_t i) const override;
  std::uint32_t tempCount(std::uint32_t i) const override;
  Oop temp(std::uint32_t i, std::uint32_t j) const override;

  // The frame's context (nil for a synthesized frame).
  Oop context(std::uint32_t i) const;
  // The arguments of the send in flight in an interpreted frame (0 for a synthesized frame).
  std::uint32_t sendArgCount(std::uint32_t i) const;
  Oop sendArg(std::uint32_t i, std::uint32_t j) const;
  // Frames before the cap (SPEC §3.10 ao_debug_frame_total).
  std::uint32_t total() const noexcept { return total_; }
  // abortReasonText at the capture.
  const std::string& reason() const noexcept { return reason_; }
  // The process that failed (nil outside a scheduler), and whether it is the base.
  Oop process() const;
  bool fromBase() const noexcept { return fromBase_; }

 private:
  // Slot layout of a frame from its base: method, receiver, context, selector, then tempCount
  // temps, then argCount send arguments.
  static constexpr std::uint32_t kFixedSlots = 4;
  struct Rec {
    int kind = kDebugFrameMethod;
    std::uint32_t pc = 0;
    std::uint32_t base = 0;
    std::uint32_t tempCount = 0;
    std::uint32_t argCount = 0;
  };
  Oop slotAt(std::uint32_t i, std::uint32_t k) const;

  Roots& roots_;
  std::vector<Rec> frames_;
  // Every captured Oop: the frames' slots, then the process. Never moves while rooted.
  std::unique_ptr<Oop[]> slots_;
  std::uint32_t slotCount_ = 0;
  std::uint32_t registered_ = 0;  // slots_[0, registered_) are in roots_
  std::uint32_t total_ = 0;
  std::string reason_;
  bool fromBase_ = false;
  bool held_ = false;
};

}  // namespace ao

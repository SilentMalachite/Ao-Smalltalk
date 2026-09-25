#pragma once
#include "ao/Bootstrap.hpp"
#include "ao/Context.hpp"
#include "ao/Heap.hpp"
#include "ao/NativeMethod.hpp"
#include "ao/Roots.hpp"
#include "ao/Scheduler.hpp"
#include "ao/Send.hpp"
#include "ao/Symbol.hpp"
#include "ao/WellKnown.hpp"

#include <pthread.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <gtest/gtest.h>
#include <string>
#include <string_view>

struct Boot {
  ao::Heap heap;
  ao::Roots roots;
  ao::WellKnown wk;
  ao::ClassMethodCache cache;
  ao::CallContext ctx;
  // SPEC §3.4: the session's cooperative scheduler; ctx is its base process. Declared last, so it
  // is destroyed first and abandons the fibers left while the heap is still there.
  ao::Scheduler scheduler;
  Boot() : Boot(1 << 20, 4 << 20, ao::kOldMaxBytes) {}
  Boot(std::size_t nurseryBytes, std::size_t oldBytes, std::size_t oldMaxBytes)
      : heap(nurseryBytes, oldBytes, oldMaxBytes),
        wk(heap, roots),
        ctx{heap, roots, wk, &cache},
        scheduler(ctx) {
    cache.addRoots(roots);
    ao::Bootstrap::run(heap, roots, wk);
    scheduler.adoptImage();
  }
};;

inline ao::Oop send0(Boot& b, ao::Oop rcvr, const char* sel) {
  auto s = ao::Symbol::intern(b.wk, sel);
  return ao::send(b.ctx, rcvr, s, nullptr, 0, nullptr);
}
inline ao::Oop send1(Boot& b, ao::Oop rcvr, const char* sel, ao::Oop a0) {
  auto s = ao::Symbol::intern(b.wk, sel);
  return ao::send(b.ctx, rcvr, s, &a0, 1, nullptr);
}
inline ao::Oop send2(Boot& b, ao::Oop rcvr, const char* sel, ao::Oop a0, ao::Oop a1) {
  auto s = ao::Symbol::intern(b.wk, sel);
  ao::Oop args[2] = {a0, a1};
  return ao::send(b.ctx, rcvr, s, args, 2, nullptr);
}

// SPEC §3.3: a failure aborts the evaluation instead of answering a value. Answers the reason of
// the abort in progress and clears it, so the test can go on with the same Boot, or "<no abort>".
inline std::string takeAbortReason(Boot& b) {
  if (!b.ctx.aborting) {
    return "<no abort>";
  }
  std::string reason = ao::abortReasonText(b.ctx);
  ao::clearUnwinding(b.ctx);
  return reason;
}

// Runs fn on a thread with a small stack and waits for it. A test that recurses until the stack
// guard trips (SPEC §3.4) then stays short, also under GC stress, where each level costs a GC.
inline void runOnSmallStack(const std::function<void()>& fn,
                            std::size_t stackBytes = std::size_t{1} << 20) {
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, stackBytes);
  pthread_t thread;
  auto body = [](void* p) -> void* {
    (*static_cast<const std::function<void()>*>(p))();
    return nullptr;
  };
  if (pthread_create(&thread, &attr, body, const_cast<std::function<void()>*>(&fn)) == 0) {
    pthread_join(thread, nullptr);
  } else {
    ADD_FAILURE() << "pthread_create failed";
  }
  pthread_attr_destroy(&attr);
}

#pragma once

#include "ao/NativeMethod.hpp"

namespace ao {

struct InlineCache {
  Oop cachedClass{};
  Oop cachedMethod{};
};

Oop send(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
         InlineCache* ic);
Oop sendSuper(CallContext& ctx, Oop receiver, Oop selector, const Oop* args, std::uint32_t argc,
              Oop methodClass);

// SPEC §3.4: true while a non-local return or an abort is unwinding the frames.
bool unwinding(const CallContext& ctx);
// Starts an abort. reason is a static string; an abort already in progress keeps its own.
// Answers the empty Oop, so a native can `return abortEvaluation(ctx, "...")`.
Oop abortEvaluation(CallContext& ctx, const char* reason);
// Drops any abort or non-local return left over. Only for outermost entry points.
void clearUnwinding(CallContext& ctx);
// Sends value, value:, ... (n up to 4) to blk with the rooted args. False when the frames are
// unwinding afterwards (SPEC §3.4): the caller must stop and answer the empty Oop.
bool callBlock(CallContext& ctx, Oop blk, const Oop* args, std::uint32_t n, Oop* out);

}  // namespace ao

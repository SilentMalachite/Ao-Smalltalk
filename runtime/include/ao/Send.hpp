#pragma once

#include "ao/NativeMethod.hpp"

#include <string>
#include <string_view>

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
// Starts an abort (SPEC §3.4). reason is a static string (a literal): nothing is allocated, so
// out of memory and stack overflow can use it. An abort already in progress keeps its own reason.
// Answers the empty Oop, so a native can `return abortEvaluation(ctx, "...")`.
Oop abortEvaluation(CallContext& ctx, const char* reason);
// Starts an abort whose reason is built at run time (SPEC §3.3). The text is copied into a heap
// String held in the roots' handle table until clearUnwinding, so it survives GCs. May collect.
// When the String cannot be allocated the reason is "out of memory".
Oop abortEvaluation(CallContext& ctx, std::string_view reason);
// Aborts with "doesNotUnderstand: #<selector>" (SPEC §3.3). May collect.
Oop abortDoesNotUnderstand(CallContext& ctx, Oop selector);
// A send of selector answered the empty Oop (SPEC §3.3). Unless the frames are unwinding already,
// aborts with "failed: #<selector>", or "out of memory" once the heap has run out. May collect.
Oop abortFailedSend(CallContext& ctx, Oop selector);
// The reason of the abort in progress as UTF-8, or "" when there is none. Each NUL byte becomes
// the two characters \0 so the text survives C strings (SPEC §3.3).
std::string abortReasonText(const CallContext& ctx);
// Drops any abort or non-local return left over, with the abort's reason. Only for outermost
// entry points.
void clearUnwinding(CallContext& ctx);
// Sends value, value:, ... (n up to 4) to blk with the rooted args. False when the frames are
// unwinding afterwards (SPEC §3.4): the caller must stop and answer the empty Oop.
bool callBlock(CallContext& ctx, Oop blk, const Oop* args, std::uint32_t n, Oop* out);
// The truth of a branch or loop condition (SPEC §3.5). A non-Boolean gets mustBeBoolean, whose
// answer must be a Boolean, or the evaluation aborts. False when the frames are unwinding.
bool truthOf(CallContext& ctx, Oop value, bool* truth);

}  // namespace ao

#pragma once

#include "ao_abi.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace ao {

struct CallContext;

// SPEC §3.9 記録, §3.10 チェンジセット (P17): each sends the matching `ChangeSet class>>note…` to
// the Smalltalk model as an outermost send on the session's base context. Nothing without a
// session, for another heap (a test's own Boot, the CLI), for an empty class name, or without a
// ChangeSet class. A send that fails (doesNotUnderstand:, error:, out of memory) is dropped: the
// Accept or removal stands. Never throws. May collect: the caller reads its Oops again from
// rooted slots afterwards.
void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept;
void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept;
// existed: the name named a class (a Behavior) before the definition.
void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept;
void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept;

// SPEC §3.10 チェンジセット (P17): the ao_changes_* bodies, each an outermost send to the model
// (the ABI takes AbiEntry first). Without a ChangeSet class the reads answer no sets; when a send
// fails they answer as while busy. A write answers AO_OK, or AO_ERR with the reason in *reason
// (left empty when the ABI's message is the generic one: no session, a bad argument).
int sessionChangesCount();
int sessionChangesAt(int index, std::int64_t* setId, char* name, int nameLen);
std::int64_t sessionChangesCurrent();
int sessionChangesNew(const char* name, std::int64_t* setId, std::string* reason);
int sessionChangesSetCurrent(std::int64_t setId);
int sessionChangesClassCount(std::int64_t setId);
int sessionChangesClassAt(std::int64_t setId, int index, char* name, int nameLen, int* kind);
int sessionChangesMethodCount(std::int64_t setId, const char* className, int meta);
int sessionChangesMethodAt(std::int64_t setId, const char* className, int meta, int index,
                           char* selector, int len, int* kind);
int sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector,
                       std::string* reason);
int sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta,
                       const char* selector, std::string* reason);
// SPEC §3.9 File Out: the model's text for the set, written to path (replacing it).
int sessionChangesFileOut(std::int64_t setId, const char* path, std::string* reason);

}  // namespace ao

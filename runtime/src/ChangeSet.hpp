#pragma once

#include "ao_abi.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ao {

struct CallContext;

// SPEC §3.9 Change Sorter (P16): an entry's kind. The values are the ABI's (SPEC §3.10).
enum class ChangeKind : int {
  Added = AO_CHANGE_ADDED,
  Changed = AO_CHANGE_CHANGED,
  Removed = AO_CHANGE_REMOVED,
};

// SPEC §3.9 Change Sorter: one class's entry. Names, not Oops: a removed class has no class ID,
// and the GC does not trace this.
struct ClassChange {
  // The class's own name.
  std::string name;
  // nullopt: only its methods changed.
  std::optional<ChangeKind> definition;
  // By selector, in byte order.
  std::map<std::string, ChangeKind> instanceSide;
  std::map<std::string, ChangeKind> classSide;
};

struct ChangeSet {
  // Never used again in the process (as a class ID).
  std::int64_t id = 0;
  // Unique in the session.
  std::string name;
  // In the order first recorded.
  std::vector<ClassChange> classes;
};

// SPEC §3.9 Change Sorter, §3.10 チェンジセット: the session's change sets. A new session (boot,
// load) starts with one empty set, Unnamed1, which is current.
struct ChangeSets {
  // Never empty; in the order made.
  std::vector<ChangeSet> sets;
  std::int64_t current = 0;
  ChangeSets();
};

// SPEC §3.9 種類の規則. After an Accept: none → Added when nothing was replaced, else Changed;
// Added stays Added; Changed stays Changed; Removed → Changed. Never nullopt.
std::optional<ChangeKind> kindAfterAccept(std::optional<ChangeKind> was, bool replaced);
// After a removal: none → Removed; Added → nullopt (the entry goes); Changed and Removed →
// Removed.
std::optional<ChangeKind> kindAfterRemoval(std::optional<ChangeKind> was);

// SPEC §3.9 記録: each updates the current set of the session whose heap ctx runs on (a fiber of it
// too). Nothing without a session, for another heap (a test's own Boot, the CLI), or for an empty
// class name. className is the class's own name. Never throws: without the memory for it, the
// record is not made and the set is as it was.
void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept;
void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept;
// existed: the name named a class (a Behavior) before the definition.
void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept;
void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept;

// SPEC §3.10 チェンジセット: the ao_changes_* bodies with the ABI's answers. A write answers AO_OK,
// or AO_ERR with the reason in *reason (left empty when the ABI's message is the generic one).
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

}  // namespace ao

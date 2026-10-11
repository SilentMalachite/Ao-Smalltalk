#include "ChangeSet.hpp"

#include "Session.hpp"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace ao {
namespace {

// SPEC §3.10 チェンジセット: never used again in this OS process, not even by a later session (as
// nextClassId in Session.cpp).
std::int64_t nextChangeSetId() {
  static std::int64_t next = 1;
  return next++;
}

ChangeSet* findSet(Session& s, std::int64_t id) {
  for (ChangeSet& set : s.changes.sets) {
    if (set.id == id) {
      return &set;
    }
  }
  return nullptr;
}

ClassChange* findClass(ChangeSet& set, std::string_view name) {
  for (ClassChange& entry : set.classes) {
    if (entry.name == name) {
      return &entry;
    }
  }
  return nullptr;
}

std::map<std::string, ChangeKind>& sideOf(ClassChange& entry, bool meta) {
  return meta ? entry.classSide : entry.instanceSide;
}

// False for what the method reads refuse (no session, an unknown set, a NULL name, meta other
// than 0 or 1). True with *side null when the set has no entry for the class.
bool methodSide(std::int64_t setId, const char* className, int meta,
                const std::map<std::string, ChangeKind>** side) {
  *side = nullptr;
  Session* s = session();
  if (s == nullptr || className == nullptr || (meta != 0 && meta != 1)) {
    return false;
  }
  ChangeSet* set = findSet(*s, setId);
  if (set == nullptr) {
    return false;
  }
  if (ClassChange* entry = findClass(*set, className)) {
    *side = &sideOf(*entry, meta == 1);
  }
  return true;
}

// SPEC §3.9 記録: the current set of the session whose heap ctx runs on; null otherwise.
ChangeSet* recordingSet(CallContext& ctx, const std::string& className) {
  Session* s = session();
  if (s == nullptr || &ctx.heap != &s->heap || className.empty()) {
    return nullptr;
  }
  return findSet(*s, s->changes.current);
}

// A class entry with neither a definition kind nor a method entry goes.
void dropIfEmpty(ChangeSet& set, std::string_view className) {
  std::erase_if(set.classes, [&](const ClassChange& c) {
    return c.name == className && !c.definition && c.instanceSide.empty() && c.classSide.empty();
  });
}

std::optional<ChangeKind> kindOf(const std::map<std::string, ChangeKind>& side,
                                 const std::string& selector) {
  const auto found = side.find(selector);
  return found == side.end() ? std::nullopt : std::optional<ChangeKind>(found->second);
}

// "<Class>>><selector>", "<Class> class>><selector>" on the class side (SPEC §3.9 削除).
std::string entryName(std::string_view className, bool meta, std::string_view selector) {
  std::string out(className);
  if (meta) {
    out += " class";
  }
  out += ">>";
  out += selector;
  return out;
}

// Puts entry in set: over the entry of the same name, or at the end.
void putClass(ChangeSet& set, ClassChange&& entry) {
  if (ClassChange* found = findClass(set, entry.name)) {
    *found = std::move(entry);
    return;
  }
  set.classes.push_back(std::move(entry));
}

// The target's entry for className as a copy to merge into; a new one without a definition when
// the target has none.
ClassChange targetCopy(ChangeSet& to, const char* className) {
  if (ClassChange* found = findClass(to, className)) {
    return *found;
  }
  return ClassChange{className, std::nullopt, {}, {}};
}

}  // namespace

std::optional<ChangeKind> kindAfterAccept(std::optional<ChangeKind> was, bool replaced) {
  if (!was) {
    return replaced ? ChangeKind::Changed : ChangeKind::Added;
  }
  return *was == ChangeKind::Added ? ChangeKind::Added : ChangeKind::Changed;
}

std::optional<ChangeKind> kindAfterRemoval(std::optional<ChangeKind> was) {
  if (was == ChangeKind::Added) {
    return std::nullopt;
  }
  return ChangeKind::Removed;
}

void noteMethodChange(CallContext& ctx, const std::string& className, bool meta,
                      std::string_view selector, bool replaced) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    std::string key(selector);
    if (ClassChange* entry = findClass(*set, className)) {
      auto& side = sideOf(*entry, meta);
      const ChangeKind kind = *kindAfterAccept(kindOf(side, key), replaced);
      side.insert_or_assign(std::move(key), kind);
      return;
    }
    ClassChange made{className, std::nullopt, {}, {}};
    sideOf(made, meta).emplace(std::move(key), *kindAfterAccept(std::nullopt, replaced));
    set->classes.push_back(std::move(made));
  } catch (...) {
    // SPEC §3.9 記録: without the memory the record is not made; the Accept stands.
  }
}

void noteMethodRemoval(CallContext& ctx, const std::string& className, bool meta,
                       std::string_view selector) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    std::string key(selector);
    ClassChange* entry = findClass(*set, className);
    if (entry == nullptr) {
      ClassChange made{className, std::nullopt, {}, {}};
      sideOf(made, meta).emplace(std::move(key), ChangeKind::Removed);
      set->classes.push_back(std::move(made));
      return;
    }
    auto& side = sideOf(*entry, meta);
    if (const std::optional<ChangeKind> now = kindAfterRemoval(kindOf(side, key))) {
      side.insert_or_assign(std::move(key), *now);
      return;
    }
    // Added in this set: the entry goes, and an emptied class entry with it.
    side.erase(key);
    dropIfEmpty(*set, className);
  } catch (...) {
  }
}

void noteClassChange(CallContext& ctx, const std::string& className, bool existed) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    if (ClassChange* entry = findClass(*set, className)) {
      entry->definition = kindAfterAccept(entry->definition, existed);
      return;
    }
    set->classes.push_back(ClassChange{className, kindAfterAccept(std::nullopt, existed), {}, {}});
  } catch (...) {
  }
}

void noteClassRemoval(CallContext& ctx, const std::string& className) noexcept {
  try {
    ChangeSet* set = recordingSet(ctx, className);
    if (set == nullptr) {
      return;
    }
    ClassChange* entry = findClass(*set, className);
    if (entry == nullptr) {
      set->classes.push_back(ClassChange{className, ChangeKind::Removed, {}, {}});
      return;
    }
    // SPEC §3.9 種類の規則: a class added in this set goes with its methods; another is Removed and
    // its method entries go.
    if (entry->definition == ChangeKind::Added) {
      std::erase_if(set->classes, [&](const ClassChange& c) { return c.name == className; });
      return;
    }
    entry->definition = ChangeKind::Removed;
    entry->instanceSide.clear();
    entry->classSide.clear();
  } catch (...) {
  }
}

ChangeSets::ChangeSets() {
  sets.push_back(ChangeSet{nextChangeSetId(), "Unnamed1", {}});
  current = sets.front().id;
}

int sessionChangesCount() {
  Session* s = session();
  return s == nullptr ? -1 : static_cast<int>(s->changes.sets.size());
}

int sessionChangesAt(int index, std::int64_t* setId, char* name, int nameLen) {
  Session* s = session();
  if (s == nullptr || index < 0 || static_cast<std::size_t>(index) >= s->changes.sets.size() ||
      name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  const ChangeSet& set = s->changes.sets[static_cast<std::size_t>(index)];
  if (setId != nullptr) {
    *setId = set.id;
  }
  return writeBuf(set.name, name, nameLen);
}

std::int64_t sessionChangesCurrent() {
  Session* s = session();
  return s == nullptr ? 0 : s->changes.current;
}

int sessionChangesNew(const char* name, std::int64_t* setId, std::string* reason) {
  Session* s = session();
  if (s == nullptr) {
    return AO_ERR;
  }
  if (name == nullptr || *name == '\0') {
    *reason = "change set name empty";
    return AO_ERR;
  }
  for (const ChangeSet& set : s->changes.sets) {
    if (set.name == name) {
      *reason = "change set name taken: " + set.name;
      return AO_ERR;
    }
  }
  s->changes.sets.push_back(ChangeSet{nextChangeSetId(), name, {}});
  // SPEC §3.9 Change Sorter: a new set is current at once.
  s->changes.current = s->changes.sets.back().id;
  if (setId != nullptr) {
    *setId = s->changes.current;
  }
  return AO_OK;
}

int sessionChangesSetCurrent(std::int64_t setId) {
  Session* s = session();
  if (s == nullptr || findSet(*s, setId) == nullptr) {
    return AO_ERR;
  }
  s->changes.current = setId;
  return AO_OK;
}

int sessionChangesClassCount(std::int64_t setId) {
  Session* s = session();
  ChangeSet* set = s == nullptr ? nullptr : findSet(*s, setId);
  return set == nullptr ? -1 : static_cast<int>(set->classes.size());
}

int sessionChangesClassAt(std::int64_t setId, int index, char* name, int nameLen, int* kind) {
  Session* s = session();
  ChangeSet* set = s == nullptr ? nullptr : findSet(*s, setId);
  if (set == nullptr || index < 0 || static_cast<std::size_t>(index) >= set->classes.size() ||
      name == nullptr || nameLen < 1) {
    return AO_ERR;
  }
  const ClassChange& entry = set->classes[static_cast<std::size_t>(index)];
  if (kind != nullptr) {
    *kind = entry.definition ? static_cast<int>(*entry.definition) : AO_CHANGE_NONE;
  }
  return writeBuf(entry.name, name, nameLen);
}

int sessionChangesMethodCount(std::int64_t setId, const char* className, int meta) {
  const std::map<std::string, ChangeKind>* side = nullptr;
  if (!methodSide(setId, className, meta, &side)) {
    return -1;
  }
  return side == nullptr ? 0 : static_cast<int>(side->size());
}

int sessionChangesMethodAt(std::int64_t setId, const char* className, int meta, int index,
                           char* selector, int len, int* kind) {
  const std::map<std::string, ChangeKind>* side = nullptr;
  if (!methodSide(setId, className, meta, &side) || side == nullptr || index < 0 ||
      static_cast<std::size_t>(index) >= side->size() || selector == nullptr || len < 1) {
    return AO_ERR;
  }
  const auto it = std::next(side->begin(), index);
  if (kind != nullptr) {
    *kind = static_cast<int>(it->second);
  }
  return writeBuf(it->first, selector, len);
}

int sessionChangesDrop(std::int64_t setId, const char* className, int meta, const char* selector,
                       std::string* reason) {
  Session* s = session();
  if (s == nullptr || className == nullptr || (selector != nullptr && meta != 0 && meta != 1)) {
    return AO_ERR;
  }
  ChangeSet* set = findSet(*s, setId);
  if (set == nullptr) {
    *reason = "unknown change set";
    return AO_ERR;
  }
  ClassChange* entry = findClass(*set, className);
  if (selector == nullptr) {
    if (entry == nullptr) {
      *reason = std::string("not in change set: ") + className;
      return AO_ERR;
    }
    std::erase_if(set->classes, [&](const ClassChange& c) { return c.name == className; });
    return AO_OK;
  }
  if (entry == nullptr || sideOf(*entry, meta == 1).erase(selector) == 0) {
    *reason = "not in change set: " + entryName(className, meta == 1, selector);
    return AO_ERR;
  }
  dropIfEmpty(*set, className);
  return AO_OK;
}

int sessionChangesMove(std::int64_t fromId, std::int64_t toId, const char* className, int meta,
                       const char* selector, std::string* reason) {
  Session* s = session();
  if (s == nullptr || className == nullptr || (selector != nullptr && meta != 0 && meta != 1)) {
    return AO_ERR;
  }
  ChangeSet* from = findSet(*s, fromId);
  ChangeSet* to = findSet(*s, toId);
  if (from == nullptr || to == nullptr) {
    *reason = "unknown change set";
    return AO_ERR;
  }
  if (from == to) {
    *reason = "same change set";
    return AO_ERR;
  }
  ClassChange* entry = findClass(*from, className);
  if (selector == nullptr) {
    if (entry == nullptr) {
      *reason = std::string("not in change set: ") + className;
      return AO_ERR;
    }
    // SPEC §3.10 チェンジセット: the moved kinds replace the target's; a definition kind the moved
    // entry lacks stays as the target has it.
    ClassChange merged = targetCopy(*to, className);
    if (entry->definition) {
      merged.definition = entry->definition;
    }
    for (const auto& [sel, kind] : entry->instanceSide) {
      merged.instanceSide.insert_or_assign(sel, kind);
    }
    for (const auto& [sel, kind] : entry->classSide) {
      merged.classSide.insert_or_assign(sel, kind);
    }
    putClass(*to, std::move(merged));
    std::erase_if(from->classes, [&](const ClassChange& c) { return c.name == className; });
    return AO_OK;
  }
  if (entry == nullptr) {
    *reason = "not in change set: " + entryName(className, meta == 1, selector);
    return AO_ERR;
  }
  auto& side = sideOf(*entry, meta == 1);
  const auto found = side.find(selector);
  if (found == side.end()) {
    *reason = "not in change set: " + entryName(className, meta == 1, selector);
    return AO_ERR;
  }
  ClassChange merged = targetCopy(*to, className);
  sideOf(merged, meta == 1).insert_or_assign(found->first, found->second);
  // to is another vector, so entry, side and found stay valid.
  putClass(*to, std::move(merged));
  side.erase(found);
  dropIfEmpty(*from, className);
  return AO_OK;
}

}  // namespace ao

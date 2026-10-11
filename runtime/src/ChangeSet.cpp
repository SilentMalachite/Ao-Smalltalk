#include "ChangeSet.hpp"

#include "Session.hpp"

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

}  // namespace

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

}  // namespace ao

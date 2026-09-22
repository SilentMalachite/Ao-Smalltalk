#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace ao {

inline bool isVendorStub(std::string_view name) {
  return name == "Bag" || name == "LinkedList" || name == "MappedCollection" ||
         name == "Date" || name == "Time";
}

struct VendorClassFile {
  std::string className;
  std::string superName;
  std::string chunkText;
  bool unsupportedShape = false;  // variableSubclass: 等
};

struct VendorExtractResult {
  std::vector<VendorClassFile> files;  // allowlist 順、deferred は含まない
  std::vector<std::string> notes;      // "Heap: variableSubclass" など
};

VendorExtractResult extractVendor(std::string_view changes,
                                  const std::vector<std::string>& allowlist);

}  // namespace ao

@_exported import CAo

@MainActor
final class BrowserModel {
  private(set) var categories: [String] = []
  private(set) var classes: [String] = []
  private(set) var protocols: [String] = []
  private(set) var selectors: [String] = []
  private(set) var source: String = ""

  private var didBoot = false
  private var selectedCategory: String?
  private(set) var selectedClass: String?
  private var selectedMeta = false
  private(set) var selectedProtocol: String?
  private(set) var selectedSelector: String?

  func boot() -> Int32 {
    if didBoot {
      return Int32(AO_OK)
    }
    let rc = ao_runtime_boot()
    if rc == Int32(AO_OK) {
      didBoot = true
    }
    return rc
  }

  func refresh() {
    let loaded = loadClasses()
    categories = uniqueCategories(loaded)
    if let selectedCategory {
      classes = loaded.filter { $0.category == selectedCategory }.map(\.name)
    } else {
      classes = loaded.map(\.name)
    }
    if let current = selectedClass, !classes.contains(current) {
      selectedClass = classes.first
    }
    protocols = loadProtocols()
    if let current = selectedProtocol, !protocols.contains(current) {
      selectedProtocol = nil
    }
    selectors = loadSelectors()
    if let current = selectedSelector, !selectors.contains(current) {
      selectedSelector = nil
    }
    source = loadSource()
  }

  func select(
    category: String,
    className: String,
    meta: Bool,
    protocol protocolName: String,
    selector: String? = nil
  ) {
    selectedCategory = category
    selectedClass = className
    selectedMeta = meta
    selectedProtocol = protocolName
    selectedSelector = selector
    refresh()
  }

  private struct ListedClass {
    var name: String
    var category: String
  }

  private func loadClasses() -> [ListedClass] {
    let count = ao_browser_class_count()
    if count <= 0 {
      return []
    }
    var rows: [ListedClass] = []
    rows.reserveCapacity(Int(count))
    for index in 0..<count {
      guard let row = copyClass(at: index) else {
        return []
      }
      rows.append(row)
    }
    return rows
  }

  private func copyClass(at index: Int32) -> ListedClass? {
    var capacity = 128
    while capacity <= 1_048_576 {
      var name = [CChar](repeating: 0, count: capacity)
      var category = [CChar](repeating: 0, count: capacity)
      let rc = name.withUnsafeMutableBufferPointer { namePointer -> Int32 in
        category.withUnsafeMutableBufferPointer { categoryPointer -> Int32 in
          guard let nameBase = namePointer.baseAddress, let categoryBase = categoryPointer.baseAddress else {
            return Int32(AO_ERR)
          }
          return ao_browser_class_at(index, nameBase, Int32(capacity), categoryBase, Int32(capacity))
        }
      }
      if rc == Int32(AO_OK) {
        return ListedClass(name: decode(name), category: decode(category))
      }
      if rc != Int32(AO_ERR_RANGE) {
        return nil
      }
      capacity *= 2
    }
    return nil
  }

  private func loadProtocols() -> [String] {
    guard let selectedClass else {
      return []
    }
    let meta = metaFlag
    return loadList(
      count: { ao_browser_protocol_count(selectedClass, meta) },
      at: { index, buffer, length in
        ao_browser_protocol_at(selectedClass, meta, index, buffer, length)
      }
    )
  }

  private func loadSelectors() -> [String] {
    guard let selectedClass, let selectedProtocol else {
      return []
    }
    let meta = metaFlag
    return loadList(
      count: { ao_browser_selector_count(selectedClass, meta, selectedProtocol) },
      at: { index, buffer, length in
        ao_browser_selector_at(selectedClass, meta, selectedProtocol, index, buffer, length)
      }
    )
  }

  private func loadSource() -> String {
    guard let selectedClass else {
      return ""
    }
    let meta = metaFlag
    if let selectedSelector {
      return copyText { buffer, length in
        ao_browser_source(selectedClass, meta, selectedSelector, buffer, length)
      } ?? ""
    }
    return copyText { buffer, length in
      ao_browser_class_definition(selectedClass, buffer, length)
    } ?? ""
  }

  private var metaFlag: Int32 {
    selectedMeta ? 1 : 0
  }

  private func loadList(
    count: () -> Int32,
    at: (Int32, UnsafeMutablePointer<CChar>, Int32) -> Int32
  ) -> [String] {
    let total = count()
    if total <= 0 {
      return []
    }
    var values: [String] = []
    values.reserveCapacity(Int(total))
    for index in 0..<total {
      guard let text = copyText({ buffer, length in at(index, buffer, length) }) else {
        return []
      }
      values.append(text)
    }
    return values
  }

  private func copyText(_ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32) -> String? {
    var capacity = 256
    while capacity <= 1_048_576 {
      var buffer = [CChar](repeating: 0, count: capacity)
      let rc = buffer.withUnsafeMutableBufferPointer { pointer -> Int32 in
        guard let base = pointer.baseAddress else {
          return Int32(AO_ERR)
        }
        return read(base, Int32(capacity))
      }
      if rc == Int32(AO_OK) {
        return decode(buffer)
      }
      if rc != Int32(AO_ERR_RANGE) {
        return nil
      }
      capacity *= 2
    }
    return nil
  }

  private func decode(_ buffer: [CChar]) -> String {
    buffer.withUnsafeBufferPointer { pointer in
      guard let base = pointer.baseAddress else {
        return ""
      }
      return String(cString: base)
    }
  }

  private func uniqueCategories(_ rows: [ListedClass]) -> [String] {
    var seen: Set<String> = []
    var values: [String] = []
    for row in rows {
      if seen.insert(row.category).inserted {
        values.append(row.category)
      }
    }
    return values
  }
}

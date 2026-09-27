@_exported import CAo

// SPEC §3.10 クラス ID: one class-list row. The ID names the class in every Browser ABI call; the
// name is only shown (two rows may share one).
struct BrowserClass: Equatable {
  let id: Int64
  let name: String
}

@MainActor
final class BrowserModel {
  // SPEC §3.10: an accepted method is a CompiledMethod, so it lands in this protocol.
  static let newMethodProtocol = "user"

  private(set) var categories: [String] = []
  private(set) var classRows: [BrowserClass] = []
  private(set) var protocols: [String] = []
  private(set) var selectors: [String] = []
  private(set) var source: String = ""
  // The selected method has no source (AO_ERR_NOSOURCE): `source` is the runtime's placeholder.
  private(set) var sourceIsPlaceholder = false

  private var didBoot = false
  private var selectedCategory: String?
  // SPEC §3.9 System Browser: the selection is the class ID. The name is the one its row showed
  // last, to find the class again when the ID leaves the list.
  private(set) var selectedClassID: Int64?
  private var selectedClassName: String?
  private var selectedMeta = false
  private(set) var selectedProtocol: String?
  private(set) var selectedSelector: String?

  // The rows' names, in list order.
  var classes: [String] {
    classRows.map(\.name)
  }

  // The selected row's name; nil when no class is selected.
  var selectedClass: String? {
    selectedClassID == nil ? nil : selectedClassName
  }

  var selectedClassRow: BrowserClass? {
    guard let selectedClassID, let selectedClassName else {
      return nil
    }
    return BrowserClass(id: selectedClassID, name: selectedClassName)
  }

  // SPEC §3.10 ao_browser_class_id: the ID of the class Smalltalk binds to name; 0 when none.
  static func classID(named name: String) -> Int64 {
    name.withCString { ao_browser_class_id($0) }
  }

  // SPEC §3.9 System Browser: the class a row acts on. Its ID while the runtime lists it; once
  // the ID left the list (a shape change since the last refresh), the class now bound to the
  // row's name; nil when neither.
  static func liveClassID(_ id: Int64, name: String?) -> Int64? {
    if id > 0, ao_browser_protocol_count(id, 0) >= 0 {
      return id
    }
    guard let name else {
      return nil
    }
    let bound = classID(named: name)
    return bound == 0 ? nil : bound
  }

  // The selected row's class, as liveClassID finds it; nil when no class is selected.
  func resolvedSelectedClassID() -> Int64? {
    selectedClassID.flatMap { Self.liveClassID($0, name: selectedClassName) }
  }

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
      classRows = loaded.filter { $0.category == selectedCategory }.map(\.row)
    } else {
      classRows = loaded.map(\.row)
    }
    reselectClass()
    protocols = loadProtocols()
    if let current = selectedProtocol, !protocols.contains(current) {
      selectedProtocol = nil
    }
    selectors = loadSelectors()
    if let current = selectedSelector, !selectors.contains(current) {
      selectedSelector = nil
    }
    (source, sourceIsPlaceholder) = loadSource()
  }

  // classID nil: no class is selected (after Remove Class…); refresh keeps it nil.
  func select(
    category: String,
    classID: Int64?,
    meta: Bool,
    protocol protocolName: String?,
    selector: String? = nil
  ) {
    selectedCategory = category
    // A new ID takes the name its row showed in the rows listed now (the rows it was picked
    // from), so a class that left the list since gives way to the one bound to that name.
    if classID != selectedClassID {
      selectedClassName = classID.flatMap { id in classRows.first { $0.id == id }?.name }
    }
    selectedClassID = classID
    selectedMeta = meta
    selectedProtocol = protocolName
    selectedSelector = selector
    refresh()
  }

  // The superclass chain up from `start` (at most 64 classes, stopping at a class that is not
  // listed or seen before), root first, then start's subclasses.
  func hierarchy(of start: BrowserClass, meta: Bool) -> [BrowserClass] {
    var chain: [BrowserClass] = []
    var current = start
    while current.id > 0, !chain.contains(where: { $0.id == current.id }), chain.count < 64 {
      chain.append(current)
      guard let next = superclass(of: current.id, meta: meta), next.id > 0 else {
        break
      }
      current = next
    }
    var rows = Array(chain.reversed())
    for sub in subclasses(of: start.id) where !rows.contains(where: { $0.id == sub.id }) {
      rows.append(sub)
    }
    return rows
  }

  // The category the runtime lists for the class; nil when the list has no such ID.
  func category(ofClassID id: Int64) -> String? {
    loadClasses().first { $0.row.id == id }?.category
  }

  // Class-list rows only, as they are listed now (answered, for the caller to keep). Protocols
  // stay unless the selected class changed under us.
  @discardableResult
  func applyHierarchyList(_ rows: [BrowserClass], selecting id: Int64?) -> [BrowserClass] {
    // SPEC §3.9 System Browser: a row whose class left the list (a shape change) gives way to the
    // class now bound to its name, or goes when no listed class is; the selection follows it.
    let listed = Set(loadClasses().map(\.row.id))
    var rebound: [BrowserClass] = []
    var selecting = id
    for row in rows {
      var current = row
      if !listed.contains(row.id) {
        let now = Self.classID(named: row.name)
        guard listed.contains(now) else {
          continue
        }
        current = BrowserClass(id: now, name: row.name)
        if row.id == id {
          selecting = now
        }
      }
      if !rebound.contains(where: { $0.id == current.id }) {
        rebound.append(current)
      }
    }
    let previous = selectedClassID
    classRows = rebound
    if let selecting, let row = rebound.first(where: { $0.id == selecting }) {
      selectedClassID = selecting
      selectedClassName = row.name
    } else if id != nil {
      // The selected row went; the choice comes from the hierarchy, not the category's rows.
      selectedClassID = rebound.first?.id
      selectedClassName = rebound.first?.name
    }
    guard selectedClassID != previous else {
      return rebound
    }
    protocols = loadProtocols()
    if let current = selectedProtocol, !protocols.contains(current) {
      selectedProtocol = nil
    }
    selectors = loadSelectors()
    if let current = selectedSelector, !selectors.contains(current) {
      selectedSelector = nil
    }
    (source, sourceIsPlaceholder) = loadSource()
    return rebound
  }

  // SPEC §3.9 System Browser: the selected ID stays while the list has it. An ID that left the
  // list gives way to the class now bound to the name its row showed, when the list has that one,
  // else to the first row.
  private func reselectClass() {
    guard let current = selectedClassID else {
      return
    }
    if let row = classRows.first(where: { $0.id == current }) {
      selectedClassName = row.name
      return
    }
    let rebound = selectedClassName.map(Self.classID(named:)) ?? 0
    let row = classRows.first { $0.id == rebound } ?? classRows.first
    selectedClassID = row?.id
    selectedClassName = row?.name
  }

  private struct ListedClass {
    var row: BrowserClass
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
      var classID: Int64 = 0
      var name = [CChar](repeating: 0, count: capacity)
      var category = [CChar](repeating: 0, count: capacity)
      let rc = withUnsafeMutablePointer(to: &classID) { idPointer -> Int32 in
        name.withUnsafeMutableBufferPointer { namePointer -> Int32 in
          category.withUnsafeMutableBufferPointer { categoryPointer -> Int32 in
            guard let nameBase = namePointer.baseAddress,
                  let categoryBase = categoryPointer.baseAddress else {
              return Int32(AO_ERR)
            }
            return ao_browser_class_at(
              index, idPointer, nameBase, Int32(capacity), categoryBase, Int32(capacity))
          }
        }
      }
      if rc == Int32(AO_OK) {
        return ListedClass(
          row: BrowserClass(id: classID, name: decode(name)), category: decode(category))
      }
      if rc != Int32(AO_ERR_RANGE) {
        return nil
      }
      capacity *= 2
    }
    return nil
  }

  // Selector of the listed method whose source is `text`. The source table keeps the accepted
  // text as is, so this finds the method just accepted without parsing its pattern. A NOSOURCE
  // placeholder is a lone comment and never equals an accepted method.
  func selector(withSource text: String) -> String? {
    guard let classID = selectedClassID else {
      return nil
    }
    let meta = metaFlag
    return selectors.first { selector in
      let source = copyText { buffer, length in
        ao_browser_source(classID, meta, selector, buffer, length)
      }
      return source == text
    }
  }

  // The runtime lists only non-empty protocols. The new-method protocol is always offered,
  // so a class without methods on this side can still take its first one.
  private func loadProtocols() -> [String] {
    guard let classID = selectedClassID else {
      return []
    }
    let meta = metaFlag
    var names = loadList(
      count: { ao_browser_protocol_count(classID, meta) },
      at: { index, buffer, length in
        ao_browser_protocol_at(classID, meta, index, buffer, length)
      }
    )
    if !names.contains(Self.newMethodProtocol) {
      names.append(Self.newMethodProtocol)
    }
    return names
  }

  private func loadSelectors() -> [String] {
    guard let classID = selectedClassID, let selectedProtocol else {
      return []
    }
    let meta = metaFlag
    return loadList(
      count: { ao_browser_selector_count(classID, meta, selectedProtocol) },
      at: { index, buffer, length in
        ao_browser_selector_at(classID, meta, selectedProtocol, index, buffer, length)
      }
    )
  }

  // `placeholder` is true when the selected method answered AO_ERR_NOSOURCE.
  private func loadSource() -> (text: String, placeholder: Bool) {
    guard let classID = selectedClassID else {
      return ("", false)
    }
    let meta = metaFlag
    if let selectedSelector {
      let copied = copyReportingNoSource { buffer, length in
        ao_browser_source(classID, meta, selectedSelector, buffer, length)
      }
      return (copied?.text ?? "", copied?.noSource ?? false)
    }
    // A protocol with no selector is a new method; no protocol is the class definition.
    if selectedProtocol != nil {
      return ("", false)
    }
    let definition = copyText { buffer, length in
      ao_browser_class_definition(classID, buffer, length)
    }
    return (definition ?? "", false)
  }

  private var metaFlag: Int32 {
    selectedMeta ? 1 : 0
  }

  // SPEC §3.10: the superclass's name and ID (0 when it is nil or not listed).
  private func superclass(of classID: Int64, meta: Bool) -> BrowserClass? {
    let flag: Int32 = meta ? 1 : 0
    var superID: Int64 = 0
    let name = withUnsafeMutablePointer(to: &superID) { idPointer in
      copyText { buffer, length in
        ao_browser_superclass(classID, flag, idPointer, buffer, length)
      }
    }
    return name.map { BrowserClass(id: superID, name: $0) }
  }

  private func subclasses(of classID: Int64) -> [BrowserClass] {
    let total = ao_browser_subclass_count(classID)
    if total <= 0 {
      return []
    }
    var rows: [BrowserClass] = []
    rows.reserveCapacity(Int(total))
    for index in 0..<total {
      var subID: Int64 = 0
      let name = withUnsafeMutablePointer(to: &subID) { idPointer in
        copyText { buffer, length in
          ao_browser_subclass_at(classID, index, idPointer, buffer, length)
        }
      }
      guard let name else {
        return []
      }
      rows.append(BrowserClass(id: subID, name: name))
    }
    return rows
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
    copyReportingNoSource(read)?.text
  }

  private func copyReportingNoSource(
    _ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32
  ) -> (text: String, noSource: Bool)? {
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
        return (decode(buffer), false)
      }
      // SPEC §3.10: a method without source answers its placeholder with AO_ERR_NOSOURCE, also
      // when cut, so a full buffer asks for a larger one.
      if rc == Int32(AO_ERR_NOSOURCE) {
        let text = decode(buffer)
        if text.utf8.count < capacity - 1 {
          return (text, true)
        }
      } else if rc != Int32(AO_ERR_RANGE) {
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

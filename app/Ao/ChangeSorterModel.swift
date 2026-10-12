import CAo
import Foundation

// SPEC §3.9 Change Sorter (P16): an entry's kind, as ao_changes_* answer it (SPEC §3.10).
enum ChangeKind: Int32 {
  case none = 0
  case added = 1
  case changed = 2
  case removed = 3

  // The mark after a name in the lists; none shows nothing.
  var mark: String {
    switch self {
    case .none:
      return ""
    case .added:
      return "+"
    case .changed:
      return "~"
    case .removed:
      return "−"
    }
  }
}

struct ChangeSetRow: Equatable {
  let id: Int64
  let name: String
}

struct ChangeClassRow: Equatable {
  let name: String
  let kind: ChangeKind
}

struct ChangeMethodRow: Equatable {
  let selector: String
  let meta: Bool
  let kind: ChangeKind

  // SPEC §3.9 Change Sorter: a class-side entry reads `class » <selector>`.
  var label: String {
    meta ? "class » \(selector)" : selector
  }
}

// SPEC §3.9 File Out: writes the set to url; on AO_ERR the message is the reason.
func fileOutChangeSet(_ setID: Int64, to url: URL) -> (status: Int32, message: String) {
  var err = AoSpan()
  let status = url.path.withCString { ao_changes_file_out(setID, $0, &err) }
  return (status, spanMessage(err))
}

// SPEC §3.9 Change Sorter: the window's model. The runtime holds the sets (SPEC §3.10 チェンジセット);
// this holds the selection and what the lists and the source pane show.
@MainActor
final class ChangeSorterModel {
  private(set) var sets: [ChangeSetRow] = []
  private(set) var currentID: Int64 = 0
  private(set) var classes: [ChangeClassRow] = []
  private(set) var methods: [ChangeMethodRow] = []
  private(set) var source = ""
  private(set) var selectedSetID: Int64?
  private(set) var selectedClass: String?
  private(set) var selectedMethod: ChangeMethodRow?
  // SPEC §3.9 Change Sorter (P17): the last refresh was refused (busy, or the model failed); the
  // lists are as before it.
  private(set) var readRefused = false

  var currentName: String? {
    sets.first { $0.id == currentID }?.name
  }

  var selectedSetName: String? {
    sets.first { $0.id == selectedSetID }?.name
  }

  // Reads everything again. A selected set the runtime no longer lists (after a boot or a load)
  // gives way to the current one; a class or method no longer in the set is deselected.
  func refresh() {
    guard ao_changes_count() >= 0 else {
      readRefused = true
      return
    }
    readRefused = false
    sets = loadSets()
    currentID = ao_changes_current()
    if !sets.contains(where: { $0.id == selectedSetID }) {
      selectedSetID = sets.contains(where: { $0.id == currentID }) ? currentID : nil
      selectedClass = nil
      selectedMethod = nil
    }
    classes = selectedSetID.map { loadClasses($0) } ?? []
    if let name = selectedClass, !classes.contains(where: { $0.name == name }) {
      selectedClass = nil
    }
    if let setID = selectedSetID, let className = selectedClass {
      methods = loadMethods(setID: setID, className: className)
    } else {
      methods = []
    }
    selectedMethod = selectedMethod.flatMap { picked in
      methods.first { $0.selector == picked.selector && $0.meta == picked.meta }
    }
    source = loadSource()
  }

  func select(setID: Int64?, className: String? = nil, method: ChangeMethodRow? = nil) {
    selectedSetID = setID
    selectedClass = className
    selectedMethod = className == nil ? nil : method
    refresh()
  }

  // SPEC §3.9 Change Sorter: the smallest Unnamed<n>, n from 1, that no set is named.
  func nextFreeName() -> String {
    let taken = Set(sets.map(\.name))
    var n = 1
    while taken.contains("Unnamed\(n)") {
      n += 1
    }
    return "Unnamed\(n)"
  }

  // AO_OK selects the new set, which the runtime made current.
  func newSet(named name: String) -> (status: Int32, message: String) {
    var err = AoSpan()
    var id: Int64 = 0
    let status = name.withCString { ao_changes_new($0, &id, &err) }
    if status == Int32(AO_OK) {
      select(setID: id)
    } else {
      refresh()
    }
    return (status, spanMessage(err))
  }

  func makeCurrent(_ id: Int64) -> Int32 {
    let status = ao_changes_set_current(id)
    refresh()
    return status
  }

  // SPEC §3.9 Change Sorter: Remove from Change Set in the selected set. method nil forgets the
  // class entry with its methods.
  func remove(className: String, method: ChangeMethodRow?) -> (status: Int32, message: String) {
    guard let setID = selectedSetID else {
      return (Int32(AO_ERR), "")
    }
    var err = AoSpan()
    let status = className.withCString { cls in
      withSelector(method) { ao_changes_drop(setID, cls, method?.meta == true ? 1 : 0, $0, &err) }
    }
    refresh()
    return (status, spanMessage(err))
  }

  // SPEC §3.9 Change Sorter: Move to, from the selected set.
  func move(
    className: String,
    method: ChangeMethodRow?,
    to targetID: Int64
  ) -> (status: Int32, message: String) {
    guard let setID = selectedSetID else {
      return (Int32(AO_ERR), "")
    }
    var err = AoSpan()
    let status = className.withCString { cls in
      withSelector(method) {
        ao_changes_move(setID, targetID, cls, method?.meta == true ? 1 : 0, $0, &err)
      }
    }
    refresh()
    return (status, spanMessage(err))
  }

  private func loadSets() -> [ChangeSetRow] {
    let count = ao_changes_count()
    guard count > 0 else {
      return []
    }
    var rows: [ChangeSetRow] = []
    for index in 0..<count {
      var id: Int64 = 0
      guard let name = copyChangeText({ ao_changes_at(index, &id, $0, $1) }) else {
        return []
      }
      rows.append(ChangeSetRow(id: id, name: name))
    }
    return rows
  }

  private func loadClasses(_ setID: Int64) -> [ChangeClassRow] {
    let count = ao_changes_class_count(setID)
    guard count > 0 else {
      return []
    }
    var rows: [ChangeClassRow] = []
    for index in 0..<count {
      var kind: Int32 = 0
      guard let name = copyChangeText({ ao_changes_class_at(setID, index, $0, $1, &kind) }) else {
        return []
      }
      rows.append(ChangeClassRow(name: name, kind: ChangeKind(rawValue: kind) ?? ChangeKind.none))
    }
    return rows
  }

  // The instance side, then the class side, each by selector (SPEC §3.9 Change Sorter).
  private func loadMethods(setID: Int64, className: String) -> [ChangeMethodRow] {
    var rows: [ChangeMethodRow] = []
    for meta in [false, true] {
      let side: Int32 = meta ? 1 : 0
      let count = className.withCString { ao_changes_method_count(setID, $0, side) }
      guard count > 0 else {
        continue
      }
      for index in 0..<count {
        var kind: Int32 = 0
        let selector = className.withCString { cls in
          copyChangeText { ao_changes_method_at(setID, cls, side, index, $0, $1, &kind) }
        }
        guard let selector else {
          return []
        }
        rows.append(ChangeMethodRow(
          selector: selector, meta: meta, kind: ChangeKind(rawValue: kind) ?? ChangeKind.none))
      }
    }
    return rows
  }

  // SPEC §3.9 Change Sorter のソース枠.
  private func loadSource() -> String {
    guard let className = selectedClass else {
      return ""
    }
    let classID = BrowserModel.classID(named: className)
    if let method = selectedMethod {
      if method.kind == .removed {
        return "removed"
      }
      let placeholder =
        "\"\(className)\(method.meta ? " class" : "")>>\(method.selector) source not available\""
      guard classID != 0 else {
        return placeholder
      }
      return copyChangeText(reportingNoSource: true) {
        ao_browser_source(classID, method.meta ? 1 : 0, method.selector, $0, $1)
      } ?? placeholder
    }
    if classes.first(where: { $0.name == className })?.kind == .removed {
      return "removed"
    }
    guard classID != 0 else {
      return "\"\(className) definition not available\""
    }
    return copyChangeText { ao_browser_class_definition(classID, $0, $1) } ?? ""
  }
}

// Calls body with method's selector as a C string, or with nil for no method.
private func withSelector<R>(_ method: ChangeMethodRow?, _ body: (UnsafePointer<CChar>?) -> R) -> R {
  guard let method else {
    return body(nil)
  }
  return method.selector.withCString { body($0) }
}

// SPEC §3.10 の文字列バッファ: reads text, doubling the buffer while the read answers AO_ERR_RANGE.
// With reportingNoSource an AO_ERR_NOSOURCE placeholder is text too (a cut one asks for more room).
// nil on any other failure.
private func copyChangeText(
  reportingNoSource: Bool = false,
  _ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32
) -> String? {
  var capacity = 256
  while capacity <= 1_048_576 {
    var buffer = [CChar](repeating: 0, count: capacity)
    let rc = buffer.withUnsafeMutableBufferPointer { pointer -> Int32 in
      guard let base = pointer.baseAddress else {
        return Int32(AO_ERR)
      }
      return read(base, Int32(capacity))
    }
    let text = buffer.withUnsafeBufferPointer { pointer in
      pointer.baseAddress.map { String(cString: $0) } ?? ""
    }
    if rc == Int32(AO_OK) {
      return text
    }
    let noSource = reportingNoSource && rc == Int32(AO_ERR_NOSOURCE)
    if noSource && text.utf8.count < capacity - 1 {
      return text
    }
    if !noSource && rc != Int32(AO_ERR_RANGE) {
      return nil
    }
    capacity *= 2
  }
  return nil
}

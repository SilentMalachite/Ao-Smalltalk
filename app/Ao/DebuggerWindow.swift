import AppKit
import CAo

// Same trampoline as aoWorkspaceInspectHook: `user` is the DebuggerWindow that set the hook, and it
// is set only while that window's ao_debug_inspect runs.
private func aoDebuggerInspectHook(
  _ className: UnsafePointer<CChar>?,
  _ printUtf8: UnsafePointer<CChar>?,
  _ printLen: Int32,
  _ user: UnsafeMutableRawPointer?
) {
  guard let user else {
    return
  }
  let classText = className.map { String(cString: $0) } ?? ""
  // SPEC §3.10: the printString may hold NUL bytes, so it is read by print_len, not up to a NUL.
  let printText = printUtf8.map {
    String(decoding: UnsafeRawBufferPointer(start: $0, count: max(Int(printLen), 0)), as: UTF8.self)
  } ?? ""
  let bits = UInt(bitPattern: user)
  MainActor.assumeIsolated {
    guard let token = UnsafeMutableRawPointer(bitPattern: bits) else {
      return
    }
    let window = Unmanaged<DebuggerWindow>.fromOpaque(token).takeUnretainedValue()
    window.receiveInspect(className: classText, printString: printText)
  }
}

// One frame of the snapshot, read whole when the window opens (SPEC §3.9 Debugger).
struct DebugFrame {
  let label: String
  let kind: Int32
  let source: String
  // An AO_ERR_NOSOURCE placeholder: shown, never selected in.
  let isPlaceholder: Bool
  // The pc's span, already in UTF-16 units of `source`.
  let highlight: NSRange
  let tempNames: [String]
}

// SPEC §3.10: a string read that answers AO_ERR_RANGE asks again with a larger buffer.
private let debugTextLimit = 64 * 1_048_576

@MainActor
private func readDebugText(_ read: (UnsafeMutablePointer<CChar>, Int32) -> Int32) -> (status: Int32, text: String) {
  var capacity = 256
  while capacity <= debugTextLimit {
    var buffer = [CChar](repeating: 0, count: capacity)
    let rc = buffer.withUnsafeMutableBufferPointer { pointer -> Int32 in
      guard let base = pointer.baseAddress else {
        return Int32(AO_ERR)
      }
      return read(base, Int32(capacity))
    }
    let text = decodeDebugText(buffer)
    if rc == Int32(AO_OK) {
      return (rc, text)
    }
    // A placeholder answers AO_ERR_NOSOURCE also when cut, so a full buffer asks for a larger one.
    if rc == Int32(AO_ERR_NOSOURCE), text.utf8.count < capacity - 1 {
      return (rc, text)
    }
    if rc != Int32(AO_ERR_RANGE), rc != Int32(AO_ERR_NOSOURCE) {
      return (rc, "")
    }
    capacity *= 2
  }
  return (Int32(AO_ERR), "")
}

private func decodeDebugText(_ buffer: [CChar]) -> String {
  buffer.withUnsafeBufferPointer { pointer in
    guard let base = pointer.baseAddress else {
      return ""
    }
    return String(cString: base)
  }
}

@MainActor
private func readDebugFrames() -> [DebugFrame] {
  let count = Int(ao_debug_frame_count())
  guard count > 0 else {
    return []
  }
  return (0..<count).map { index in
    let i = Int32(index)
    let label = readDebugText { ao_debug_frame_label(i, $0, $1) }.text
    var span = AoSpan()
    let source = readDebugText { buf, len in
      withUnsafeMutablePointer(to: &span) { ao_debug_frame_source(i, buf, len, $0) }
    }
    let placeholder = source.status != Int32(AO_OK)
    let highlight = placeholder ? NSRange(location: 0, length: 0) : utf16Range(of: span, in: source.text)
    let tempCount = max(Int(ao_debug_frame_temp_count(i)), 0)
    let names = (0..<tempCount).map { j in
      readDebugText { ao_debug_frame_temp_name(i, Int32(j), $0, $1) }.text
    }
    return DebugFrame(
      label: label,
      kind: ao_debug_frame_kind(i),
      source: source.text,
      isPlaceholder: placeholder,
      highlight: highlight,
      tempNames: names
    )
  }
}

// SPEC §3.9 Debugger: the post-mortem view of the session's snapshot (§3.13). It reads the
// frames when it opens; a value is printed only when its row is shown or asked for, and never once
// the generation has moved (the next evaluation cleared the snapshot).
@MainActor
final class DebuggerWindow: NSObject, NSTableViewDataSource, NSTableViewDelegate {
  // The open Debuggers, held weakly, so AoApp.resizeText reaches every one of them.
  private static let live = NSHashTable<DebuggerWindow>.weakObjects()

  static var open: [DebuggerWindow] {
    live.allObjects
  }

  // SPEC §3.10 ao_debug_reason: the whole reason; empty without a snapshot.
  static func snapshotReason() -> String {
    readDebugText { ao_debug_reason($0, $1) }.text
  }

  let window: NSWindow
  let generation: Int32
  let reason: String
  let frames: [DebugFrame]
  private let frameTable = NSTableView()
  private let variableTable = NSTableView()
  private let sourceView: NSTextView
  private let uniformFont = UniformFont()
  private var values: [Int: (className: String, value: String)] = [:]
  private var inspectors: [InspectorWindow] = []
  private var inspectClassName = ""
  private var inspectPrint = ""
  private var inspectReceived = false
  // nonisolated(unsafe): deinit, which is nonisolated, reads it once nothing else can.
  nonisolated(unsafe) private var closeObserver: (any NSObjectProtocol)?

  var title: String {
    window.title
  }

  var frameLabels: [String] {
    frames.map(\.label)
  }

  var selectedFrame: Int? {
    let row = frameTable.selectedRow
    return row >= 0 && row < frames.count ? row : nil
  }

  var sourceText: String {
    sourceView.string
  }

  var sourceSelection: NSRange {
    sourceView.selectedRange()
  }

  var sourceIsEditable: Bool {
    sourceView.isEditable
  }

  var variableCount: Int {
    guard let selectedFrame else {
      return 0
    }
    return frames[selectedFrame].tempNames.count + 1
  }

  var inspectorText: String {
    inspectors.last?.text ?? ""
  }

  var inspectorCount: Int {
    inspectors.count
  }

  // SPEC §3.9: `onClose` runs once, from inside `close`, so the owner can drop this Debugger.
  init(onClose: @escaping @MainActor @Sendable (DebuggerWindow) -> Void = { _ in }) {
    generation = ao_debug_generation()
    reason = Self.snapshotReason()
    frames = readDebugFrames()
    let frame = NSRect(x: 260, y: 120, width: 640, height: 600)
    let built = makeToolTextWindow(title: "Debugger: " + reason, frame: frame, editable: false)
    window = built.window
    sourceView = built.textView
    super.init()

    configureTable(frameTable, columns: [("frame", "Frame")], header: false)
    configureTable(variableTable, columns: [("name", "Name"), ("class", "Class"), ("value", "Value")], header: true)
    frameTable.setAccessibilityLabel("Frames")
    variableTable.setAccessibilityLabel("Variables")
    sourceView.setAccessibilityLabel("Source")
    variableTable.target = self
    variableTable.doubleAction = #selector(variableDoubleClicked(_:))

    // NSSplitView keeps the current size ratio, so a zero frame stays collapsed.
    let listBand = NSRect(x: 0, y: 0, width: frame.width, height: frame.height / 4)
    let sourceBand = NSRect(x: 0, y: 0, width: frame.width, height: frame.height / 2)
    let split = NSSplitView(frame: NSRect(origin: .zero, size: frame.size))
    split.isVertical = false
    split.dividerStyle = .thin
    split.autoresizingMask = [.width, .height]
    let frameScroll = Self.scroll(for: frameTable, frame: listBand)
    let variableScroll = Self.scroll(for: variableTable, frame: listBand)
    split.addArrangedSubview(frameScroll)
    if let sourceScroll = sourceView.enclosingScrollView {
      split.addArrangedSubview(sourceScroll)
      sourceScroll.frame = sourceBand
    }
    split.addArrangedSubview(variableScroll)
    frameScroll.frame = listBand
    variableScroll.frame = listBand
    window.contentView = split

    // queue nil: the center calls this synchronously, inside `close` on the main thread.
    closeObserver = NotificationCenter.default.addObserver(
      forName: NSWindow.willCloseNotification,
      object: window,
      queue: nil
    ) { [weak self] _ in
      MainActor.assumeIsolated {
        self?.windowWillClose(onClose)
      }
    }
    Self.live.add(self)
    applyFont()
    frameTable.reloadData()
    if !frames.isEmpty {
      selectFrame(0)
    }
    window.makeKeyAndOrderFront(nil)
  }

  // Gone before its window closed: the observer goes too, or the center would keep it.
  deinit {
    if let closeObserver {
      NotificationCenter.default.removeObserver(closeObserver)
    }
  }

  // SPEC §3.9 文字の大きさ: the source pane shares the tool text size; the lists keep the system's.
  func applyFont() {
    guard let font = ToolTextSize.font(fixedPitch: false) else {
      return
    }
    uniformFont.apply(font, to: sourceView)
  }

  func orderFront() {
    window.makeKeyAndOrderFront(nil)
  }

  // Through the table, so a click and a test take the same path (tableViewSelectionDidChange).
  func selectFrame(_ index: Int) {
    guard index >= 0, index < frames.count else {
      return
    }
    frameTable.selectRowIndexes(IndexSet(integer: index), byExtendingSelection: false)
    if frameTable.selectedRow == index {
      showFrame(index)
    }
  }

  // Row 0 is self; row r is temp r - 1. The printString is read now, once per selection.
  func variable(at row: Int) -> (name: String, className: String, value: String) {
    guard let selectedFrame, row >= 0, row < variableCount else {
      return ("", "", "")
    }
    let name = row == 0 ? "self" : frames[selectedFrame].tempNames[row - 1]
    if let cached = values[row] {
      return (name, cached.className, cached.value)
    }
    guard ao_debug_generation() == generation else {
      return (name, "", "-")
    }
    let read = printValue(frame: Int32(selectedFrame), temp: Int32(row - 1))
    guard let read else {
      return (name, "", "-")
    }
    values[row] = read
    return (name, read.className, read.value)
  }

  func inspectVariable(at row: Int) {
    guard let selectedFrame, row >= 0, row < variableCount, ao_debug_generation() == generation else {
      return
    }
    inspectClassName = ""
    inspectPrint = ""
    inspectReceived = false
    ao_set_inspect_hook(aoDebuggerInspectHook, Unmanaged.passUnretained(self).toOpaque())
    defer {
      ao_set_inspect_hook(nil, nil)
    }
    let status = ao_debug_inspect(Int32(selectedFrame), Int32(row - 1))
    guard status == Int32(AO_OK), inspectReceived else {
      return
    }
    openInspector(className: inspectClassName, printString: inspectPrint)
  }

  fileprivate func receiveInspect(className: String, printString: String) {
    inspectClassName = className
    inspectPrint = printString
    inspectReceived = true
  }

  // MARK: - NSTableViewDataSource, NSTableViewDelegate

  func numberOfRows(in tableView: NSTableView) -> Int {
    tableView === frameTable ? frames.count : variableCount
  }

  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
    let identifier = tableColumn?.identifier ?? NSUserInterfaceItemIdentifier("frame")
    let text: String
    if tableView === frameTable {
      guard row >= 0, row < frames.count else {
        return nil
      }
      text = frames[row].label
    } else {
      let shown = variable(at: row)
      switch identifier.rawValue {
      case "name": text = shown.name
      case "class": text = shown.className
      default: text = shown.value
      }
    }
    let cell = tableView.makeView(withIdentifier: identifier, owner: self) as? NSTextField
      ?? NSTextField(labelWithString: "")
    cell.identifier = identifier
    cell.stringValue = text
    cell.isEditable = false
    cell.isBordered = false
    cell.drawsBackground = false
    cell.lineBreakMode = .byTruncatingTail
    return cell
  }

  func tableViewSelectionDidChange(_ notification: Notification) {
    guard (notification.object as? NSTableView) === frameTable, let selectedFrame else {
      return
    }
    showFrame(selectedFrame)
  }

  // MARK: - private

  // SPEC §3.9: the source goes in whole; the pc's span is selected and shown, not for a placeholder.
  private func showFrame(_ index: Int) {
    let frame = frames[index]
    values = [:]
    sourceView.string = frame.source
    applyFont()
    let length = (frame.source as NSString).length
    let span = frame.highlight
    if !frame.isPlaceholder, span.length > 0, NSMaxRange(span) <= length {
      sourceView.setSelectedRange(span)
      sourceView.scrollRangeToVisible(span)
      if window.isVisible {
        sourceView.showFindIndicator(for: span)
      }
    } else {
      sourceView.setSelectedRange(NSRange(location: 0, length: 0))
      sourceView.scrollRangeToVisible(NSRange(location: 0, length: 0))
    }
    variableTable.reloadData()
  }

  // nil when the runtime refuses (busy, or the snapshot went); a failed printString is class only.
  private func printValue(frame: Int32, temp: Int32) -> (className: String, value: String)? {
    var capacity = 256
    while capacity <= debugTextLimit {
      var classBuffer = [CChar](repeating: 0, count: capacity)
      var printBuffer = [CChar](repeating: 0, count: capacity)
      let rc = classBuffer.withUnsafeMutableBufferPointer { classPointer -> Int32 in
        printBuffer.withUnsafeMutableBufferPointer { printPointer -> Int32 in
          guard let classBase = classPointer.baseAddress, let printBase = printPointer.baseAddress else {
            return Int32(AO_ERR)
          }
          let size = Int32(capacity)
          if temp < 0 {
            return ao_debug_frame_receiver_print(frame, classBase, size, printBase, size)
          }
          return ao_debug_frame_temp_print(frame, temp, classBase, size, printBase, size)
        }
      }
      if rc == Int32(AO_OK) {
        return (decodeDebugText(classBuffer), decodeDebugText(printBuffer))
      }
      if rc != Int32(AO_ERR_RANGE) {
        return nil
      }
      capacity *= 2
    }
    return nil
  }

  @objc private func variableDoubleClicked(_ sender: NSTableView) {
    let row = sender.clickedRow >= 0 ? sender.clickedRow : sender.selectedRow
    inspectVariable(at: row)
  }

  // As WorkspaceWindow.openInspector: the same lines order the last Inspector front.
  private func openInspector(className: String, printString: String) {
    let lines = InspectorWindow.lines(className: className, printString: printString)
    if let existing = inspectors.last, existing.text == lines {
      existing.window.makeKeyAndOrderFront(nil)
      return
    }
    let inspector = InspectorWindow(className: className, printString: printString) { [weak self] closed in
      self?.inspectors.removeAll { $0 === closed }
    }
    inspectors.append(inspector)
  }

  private func configureTable(_ table: NSTableView, columns: [(id: String, title: String)], header: Bool) {
    for (id, title) in columns {
      let column = NSTableColumn(identifier: NSUserInterfaceItemIdentifier(id))
      column.title = title
      column.headerCell.setAccessibilityLabel(title)
      column.resizingMask = .autoresizingMask
      column.width = columns.count == 1 ? 600 : 200
      table.addTableColumn(column)
    }
    if !header {
      table.headerView = nil
    }
    table.allowsEmptySelection = true
    table.allowsMultipleSelection = false
    table.columnAutoresizingStyle = .lastColumnOnlyAutoresizingStyle
    table.dataSource = self
    table.delegate = self
  }

  private static func scroll(for table: NSTableView, frame: NSRect) -> NSScrollView {
    let scroll = NSScrollView(frame: frame)
    scroll.hasVerticalScroller = true
    scroll.hasHorizontalScroller = false
    scroll.borderType = .noBorder
    scroll.autoresizingMask = [.width, .height]
    let contentSize = scroll.contentSize
    let width = contentSize.width > 0 ? contentSize.width : frame.width
    let height = contentSize.height > 0 ? contentSize.height : frame.height
    table.frame = NSRect(x: 0, y: 0, width: width, height: height)
    table.autoresizingMask = [.width]
    scroll.documentView = table
    return scroll
  }

  private func windowWillClose(_ onClose: @MainActor (DebuggerWindow) -> Void) {
    Self.live.remove(self)
    onClose(self)
    if let closeObserver {
      NotificationCenter.default.removeObserver(closeObserver)
    }
    closeObserver = nil
  }
}

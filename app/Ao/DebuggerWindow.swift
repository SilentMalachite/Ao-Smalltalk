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

// SPEC §3.9 ライブ Debugger: how Proceed or a Step ended when it did not halt again: the ABI's
// status (AO_OK, AO_ERR_RANGE or AO_ERR_EVAL), out, and the reason.
struct DebugOutcome {
  let status: Int32
  let output: String
  let message: String
}

// SPEC §3.10: out stops at 64 KiB; the Workspace reads a Print it's whole result past it.
private let debugOutCapacity = 65_536

// SPEC §3.9 Debugger: the post-mortem view of the session's snapshot (§3.13), or (pid > 0) the
// live view of a halted process with Proceed, Abort and the Steps. It reads the frames when it
// opens (and, live, after each Step); a value is printed only when its row is shown or asked for,
// and never once what it shows is gone (a post-mortem window's generation moved; a live window's
// process no longer halted).
@MainActor
final class DebuggerWindow: NSObject, NSTableViewDataSource, NSTableViewDelegate {
  // The open Debuggers, held weakly, so AoApp.resizeText reaches every one of them.
  private static let live = NSHashTable<DebuggerWindow>.weakObjects()

  static var open: [DebuggerWindow] {
    live.allObjects
  }

  // SPEC §3.10 ao_debug_reason: the whole reason; empty without a snapshot.
  static func snapshotReason() -> String {
    _ = ao_debug_select(0)
    return readDebugText { ao_debug_reason($0, $1) }.text
  }

  // SPEC §3.10 ao_debug_select: the reads follow the last selection, so the snapshot's count
  // selects it first.
  static func snapshotFrameCount() -> Int32 {
    _ = ao_debug_select(0)
    return ao_debug_frame_count()
  }

  let window: NSWindow
  // 0: the post-mortem Debugger of the snapshot; else the halted process it shows (SPEC §3.9).
  let pid: Int64
  private(set) var generation: Int32
  private(set) var reason: String
  private(set) var frames: [DebugFrame]
  // Live only: Proceed, Abort, Step over, Step into, Step out.
  private(set) var buttons: [NSButton] = []
  private let buttonActions = DebuggerButtonActions()
  // Runs a Proceed or Step: the Workspace wraps it (its inspect hook for an Inspect it).
  private let around: @MainActor (() -> Void) -> Void
  private let onFinish: @MainActor (DebugOutcome) -> Void
  // Live: the process ended through a button, so closing the window does not abort it.
  private var finished = false
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
  nonisolated(unsafe) private var keyObserver: (any NSObjectProtocol)?

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

  var isLive: Bool {
    pid != 0
  }

  // SPEC §3.9: `onClose` runs once, from inside `close`, so the owner can drop this Debugger.
  // pid 0 reads the snapshot; a halted process's pid makes the live Debugger, whose Proceed or
  // Step that ends the evaluation goes to `onFinish` (run inside `around`) and closes the window.
  init(
    pid: Int64 = 0,
    around: @escaping @MainActor (() -> Void) -> Void = { $0() },
    onFinish: @escaping @MainActor (DebugOutcome) -> Void = { _ in },
    onClose: @escaping @MainActor @Sendable (DebuggerWindow) -> Void = { _ in }
  ) {
    self.pid = pid
    self.around = around
    self.onFinish = onFinish
    _ = ao_debug_select(pid)
    generation = ao_debug_generation()
    reason = readDebugText { ao_debug_reason($0, $1) }.text
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
    if pid != 0 {
      window.contentView = installButtons(above: split, size: frame.size)
    } else {
      window.contentView = split
    }

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
    // SPEC §3.9: a live Debugger's process may end elsewhere (a terminate from another Do it);
    // the buttons follow when the window comes back to the front.
    if pid != 0 {
      keyObserver = NotificationCenter.default.addObserver(
        forName: NSWindow.didBecomeKeyNotification,
        object: window,
        queue: nil
      ) { [weak self] _ in
        MainActor.assumeIsolated {
          self?.refreshLive()
        }
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
    if let keyObserver {
      NotificationCenter.default.removeObserver(keyObserver)
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
    // Before the cache: a value of a process that is gone (terminated elsewhere) is not shown.
    guard readable() else {
      return (name, "", "-")
    }
    if let cached = values[row] {
      return (name, cached.className, cached.value)
    }
    let read = printValue(frame: Int32(selectedFrame), temp: Int32(row - 1))
    guard let read else {
      return (name, "", "-")
    }
    values[row] = read
    return (name, read.className, read.value)
  }

  func inspectVariable(at row: Int) {
    guard let selectedFrame, row >= 0, row < variableCount, readable() else {
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

  // MARK: - live (SPEC §3.9 ライブ Debugger)

  func proceed() {
    resume { ao_debug_proceed($0, $1, $2, $3) }
  }

  func stepOver() {
    resume { ao_debug_step_over($0, $1, $2, $3) }
  }

  func stepInto() {
    resume { ao_debug_step_into($0, $1, $2, $3) }
  }

  func stepOut() {
    resume { ao_debug_step_out($0, $1, $2, $3) }
  }

  // Terminates the process (its ensure: blocks run) and closes the window.
  func abort() {
    guard isLive, !finished else {
      return
    }
    guard ao_debug_abort(pid) == Int32(AO_OK) else {
      updateButtons()
      return
    }
    finished = true
    window.close()
  }

  private func resume(_ call: (Int64, UnsafeMutablePointer<CChar>, Int32, UnsafeMutablePointer<AoSpan>) -> Int32) {
    guard isLive, !finished else {
      return
    }
    var out = [CChar](repeating: 0, count: debugOutCapacity)
    var err = AoSpan()
    var status = Int32(AO_ERR)
    around {
      status = out.withUnsafeMutableBufferPointer { outBuf -> Int32 in
        guard let outPtr = outBuf.baseAddress else {
          return Int32(AO_ERR)
        }
        return withUnsafeMutablePointer(to: &err) { call(pid, outPtr, Int32(outBuf.count), $0) }
      }
    }
    if status == Int32(AO_ERR_HALT) {
      reload()
      return
    }
    guard status != Int32(AO_ERR) else {
      // Refused (busy) or no longer halted: nothing ran.
      updateButtons()
      return
    }
    let output = out.withUnsafeBufferPointer { buf -> String in
      guard let base = buf.baseAddress else {
        return ""
      }
      return String(cString: base)
    }
    finished = true
    let outcome = DebugOutcome(status: status, output: output, message: spanMessage(err))
    window.close()
    onFinish(outcome)
  }

  // The process halted again: everything is read anew, the innermost frame selected.
  private func reload() {
    _ = ao_debug_select(pid)
    generation = ao_debug_generation()
    reason = readDebugText { ao_debug_reason($0, $1) }.text
    frames = readDebugFrames()
    window.title = "Debugger: " + reason
    values = [:]
    frameTable.reloadData()
    if frames.isEmpty {
      sourceView.string = ""
      variableTable.reloadData()
    } else {
      selectFrame(0)
    }
    updateButtons()
  }

  // What it shows can still be read: the snapshot of its generation, or its halted process.
  private func readable() -> Bool {
    guard ao_debug_select(pid) == Int32(AO_OK) else {
      if isLive {
        updateButtons()
      }
      return false
    }
    return isLive ? !finished : ao_debug_generation() == generation
  }

  // SPEC §3.9: once its process is gone (ended elsewhere), the buttons go off and the shown
  // values become `-`.
  private func refreshLive() {
    updateButtons()
    guard isLive, finished || ao_debug_select(pid) != Int32(AO_OK) else {
      return
    }
    values = [:]
    variableTable.reloadData()
  }

  // The value column's text of variable row `row` as the table shows it (for tests).
  func shownValue(row: Int) -> String? {
    guard let column = variableTable.tableColumns.firstIndex(where: { $0.identifier.rawValue == "value" }),
          row >= 0, row < variableTable.numberOfRows else {
      return nil
    }
    return (variableTable.view(atColumn: column, row: row, makeIfNecessary: true) as? NSTextField)?.stringValue
  }

  // SPEC §3.9: Proceed and the Steps only when the halt can go on; nothing once it is gone.
  private func updateButtons() {
    let halted = !finished && ao_debug_select(pid) == Int32(AO_OK)
    let canGoOn = halted && ao_debug_can_proceed(pid) == 1
    for button in buttons {
      button.isEnabled = button.title == "Abort" ? halted : canGoOn
    }
  }

  private func installButtons(above split: NSSplitView, size: NSSize) -> NSView {
    let barHeight: CGFloat = 32
    let container = NSView(frame: NSRect(origin: .zero, size: size))
    container.autoresizingMask = [.width, .height]
    split.frame = NSRect(x: 0, y: 0, width: size.width, height: max(size.height - barHeight, 0))
    let runs: [(String, @MainActor (DebuggerWindow) -> Void)] = [
      ("Proceed", { $0.proceed() }),
      ("Abort", { $0.abort() }),
      ("Step over", { $0.stepOver() }),
      ("Step into", { $0.stepInto() }),
      ("Step out", { $0.stepOut() })
    ]
    var x: CGFloat = 8
    for (title, run) in runs {
      let button = NSButton(title: title, target: buttonActions, action: #selector(DebuggerButtonActions.invoke(_:)))
      button.bezelStyle = .rounded
      button.setAccessibilityLabel(title)
      button.sizeToFit()
      button.frame.origin = NSPoint(x: x, y: size.height - barHeight + 2)
      button.autoresizingMask = [.minYMargin]
      x += button.frame.width + 4
      container.addSubview(button)
      buttons.append(button)
      buttonActions.runs[ObjectIdentifier(button)] = { [weak self] in
        guard let self else {
          return
        }
        run(self)
      }
    }
    container.addSubview(split)
    updateButtons()
    return container
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
    // SPEC §3.9: closing a live Debugger (not through its buttons) aborts the halted process.
    if isLive, !finished {
      finished = true
      _ = ao_debug_abort(pid)
    }
    Self.live.remove(self)
    onClose(self)
    if let closeObserver {
      NotificationCenter.default.removeObserver(closeObserver)
    }
    closeObserver = nil
    if let keyObserver {
      NotificationCenter.default.removeObserver(keyObserver)
    }
    keyObserver = nil
  }
}

// NSButton.target is weak; the Debugger keeps this one.
@MainActor
private final class DebuggerButtonActions: NSObject {
  var runs: [ObjectIdentifier: @MainActor () -> Void] = [:]

  @objc func invoke(_ sender: NSButton) {
    runs[ObjectIdentifier(sender)]?()
  }
}

import AppKit
import CAo

private let aoEvalOutCapacity = 65_536

// C function pointer: no captures. `user` is the WorkspaceWindow.
private func aoWorkspaceInspectHook(
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
    let window = Unmanaged<WorkspaceWindow>.fromOpaque(token).takeUnretainedValue()
    window.receiveInspect(className: classText, printString: printText)
  }
}

@MainActor
func sendToKeyWorkspace(
  _ workspace: WorkspaceWindow?,
  keyWindow: NSWindow?,
  _ command: (WorkspaceWindow) -> Void
) {
  guard let workspace, workspace.ownsWindow(keyWindow) else {
    return
  }
  command(workspace)
}

@MainActor
private func installErrorField(on window: NSWindow, textView: NSTextView) -> NSTextField {
  let field = NSTextField(frame: .zero)
  field.isEditable = false
  field.isSelectable = false
  field.isBezeled = false
  field.drawsBackground = false
  field.stringValue = ""
  field.setAccessibilityLabel("Error")
  field.font = NSFont.systemFont(ofSize: NSFont.smallSystemFontSize)
  field.textColor = .secondaryLabelColor
  field.translatesAutoresizingMaskIntoConstraints = true
  if let cell = field.cell as? NSTextFieldCell {
    cell.usesSingleLineMode = true
    cell.lineBreakMode = .byTruncatingTail
  }
  guard let scroll = textView.enclosingScrollView else {
    return field
  }
  let bounds = scroll.frame
  let container = NSView(frame: bounds)
  container.autoresizingMask = [.width, .height]
  let errorHeight: CGFloat = 22
  field.frame = NSRect(x: 6, y: 3, width: max(bounds.width - 12, 0), height: 16)
  field.autoresizingMask = [.width]
  scroll.frame = NSRect(
    x: 0,
    y: errorHeight,
    width: bounds.width,
    height: max(bounds.height - errorHeight, 0)
  )
  scroll.autoresizingMask = [.width, .height]
  container.addSubview(scroll)
  container.addSubview(field)
  window.contentView = container
  return field
}

// SPEC §3.9 Workspace: the Debug button sits at the right end of the error band, hidden until an
// evaluation leaves a snapshot. The field gives up the button's width.
@MainActor
private func installDebugButton(beside field: NSTextField, target: AnyObject, action: Selector) -> NSButton {
  let button = NSButton(title: "Debug", target: target, action: action)
  button.bezelStyle = .inline
  button.controlSize = .small
  button.font = NSFont.systemFont(ofSize: NSFont.smallSystemFontSize)
  button.setAccessibilityLabel("Debug")
  button.isHidden = true
  button.sizeToFit()
  guard let band = field.superview else {
    return button
  }
  let width = max(button.frame.width, 48)
  let bandWidth = band.bounds.width
  button.frame = NSRect(x: max(bandWidth - width - 6, 0), y: 2, width: width, height: 18)
  button.autoresizingMask = [.minXMargin]
  field.frame.size.width = max(bandWidth - width - 18, 0)
  band.addSubview(button)
  return button
}

// NSButton.target is weak; the Workspace keeps this one.
@MainActor
private final class WorkspaceButtonAction: NSObject {
  var run: (@MainActor () -> Void)?

  @objc func invoke(_ sender: NSButton) {
    run?()
  }
}

func spanMessage(_ span: AoSpan) -> String {
  withUnsafeBytes(of: span.message) { raw in
    let bytes = raw.bindMemory(to: CChar.self)
    guard let base = bytes.baseAddress else {
      return ""
    }
    var end = 0
    while end < bytes.count, bytes[end] != 0 {
      end += 1
    }
    if end == 0 {
      return ""
    }
    let slice = UnsafeRawBufferPointer(start: UnsafeRawPointer(base), count: end)
    return String(decoding: slice, as: UTF8.self)
  }
}

// SPEC §3.8, §3.9: a span's UTF-8 byte offsets as a UTF-16 range of `text`, the source the span
// counts in. An offset inside a scalar moves out of it: the start back to the scalar's first
// byte, the end past its last byte. Offsets past the end of `text` stop at the end.
func utf16Range(of span: AoSpan, in text: String) -> NSRange {
  let byteCount = text.utf8.count
  let start = min(Int(span.start), byteCount)
  let end = min(max(Int(span.end), start), byteCount)
  var bytes = 0
  var units = 0
  var lower: Int?
  var upper: Int?
  for scalar in text.unicodeScalars {
    let nextBytes = bytes + UTF8.width(scalar)
    if lower == nil, start < nextBytes {
      lower = units
    }
    if upper == nil, end <= bytes {
      upper = units
    }
    if lower != nil, upper != nil {
      break
    }
    bytes = nextBytes
    units += UTF16.width(scalar)
  }
  let from = lower ?? units
  let to = max(upper ?? units, from)
  return NSRange(location: from, length: to - from)
}

// SPEC §3.9: a compile error selects its span in `textView` and leaves the text alone. `source`
// is the text the span counts in, and it starts at `base` in the view. AO_ERR_EVAL (span 0-0) and
// an empty span leave the selection as it is.
@MainActor
func selectErrorSpan(
  status: Int32,
  span: AoSpan,
  source: String,
  base: Int,
  in textView: NSTextView
) {
  guard status == Int32(AO_ERR_COMPILE), span.start < span.end else {
    return
  }
  let local = utf16Range(of: span, in: source)
  let range = NSRange(location: base + local.location, length: local.length)
  guard range.length > 0, NSMaxRange(range) <= (textView.string as NSString).length else {
    return
  }
  textView.setSelectedRange(range)
  textView.scrollRangeToVisible(range)
  textView.showFindIndicator(for: range)
}

func failureText(status: Int32, message: String) -> String {
  if !message.isEmpty {
    return message
  }
  if status == Int32(AO_ERR_RANGE) {
    return "result does not fit"
  }
  if status == Int32(AO_ERR_COMPILE) {
    return "compile failed"
  }
  return "evaluation failed"
}

// SPEC §3.10: the whole result the runtime kept for the last Print it or Inspect it. It may hold
// NUL bytes, so exactly `length` bytes are decoded. nil when the runtime kept no result.
private func keptEvalResult() -> String? {
  let length = Int(ao_eval_result_length())
  guard length >= 0 else {
    return nil
  }
  var bytes = [CChar](repeating: 0, count: length + 1)
  let status = bytes.withUnsafeMutableBufferPointer { buf -> Int32 in
    guard let base = buf.baseAddress else {
      return Int32(AO_ERR)
    }
    return ao_eval_result_copy(base, Int32(buf.count))
  }
  guard status == Int32(AO_OK) else {
    return nil
  }
  return bytes.withUnsafeBytes { raw in
    String(decoding: raw.prefix(length), as: UTF8.self)
  }
}

@MainActor
final class WorkspaceWindow {
  let window: NSWindow
  let errorField: NSTextField
  let debugButton: NSButton
  private let textView: NSTextView
  private let uniformFont = UniformFont()
  private let debugAction = WorkspaceButtonAction()
  private var inspectors: [InspectorWindow] = []
  private(set) var debuggers: [DebuggerWindow] = []
  private var inspectClassName = ""
  private var inspectPrint = ""
  private(set) var inspectorLinesFromHook = ""
  private(set) var inspectorLinesFromOut = ""

  var title: String {
    window.title
  }

  var text: String {
    textView.string
  }

  var errorText: String {
    errorField.stringValue
  }

  var textAccessibilityLabel: String? {
    textView.accessibilityLabel()
  }

  var errorAccessibilityLabel: String? {
    errorField.accessibilityLabel()
  }

  var inspectorText: String {
    inspectors.last?.text ?? ""
  }

  var inspectorWindow: NSWindow? {
    inspectors.last?.window
  }

  var inspectorCount: Int {
    inspectors.count
  }

  init() {
    let built = makeToolTextWindow(
      title: "Workspace",
      frame: NSRect(x: 140, y: 80, width: 560, height: 360),
      editable: true
    )
    window = built.window
    textView = built.textView
    textView.setAccessibilityLabel("Workspace")
    errorField = installErrorField(on: built.window, textView: built.textView)
    debugButton = installDebugButton(
      beside: errorField,
      target: debugAction,
      action: #selector(WorkspaceButtonAction.invoke(_:))
    )
    debugAction.run = { [weak self] in
      self?.openDebugger()
    }
    applyFont()
    window.makeKeyAndOrderFront(nil)
  }

  // SPEC §3.9 文字の大きさ: run at init and whenever the text size changes.
  func applyFont() {
    guard let font = ToolTextSize.font(fixedPitch: false) else {
      return
    }
    uniformFont.apply(font, to: textView)
  }

  func orderFront() {
    window.makeKeyAndOrderFront(nil)
  }

  func ownsWindow(_ candidate: NSWindow?) -> Bool {
    candidate === window
  }

  func replaceText(_ value: String) {
    textView.string = value
  }

  func selectAll() {
    let length = (textView.string as NSString).length
    window.makeFirstResponder(textView)
    textView.setSelectedRange(NSRange(location: 0, length: length))
  }

  func setCaret(_ location: Int) {
    let length = (textView.string as NSString).length
    let clamped = min(max(location, 0), length)
    window.makeFirstResponder(textView)
    textView.setSelectedRange(NSRange(location: clamped, length: 0))
  }

  func doIt() {
    run(Int32(AO_EVAL_DOIT))
  }

  func printIt() {
    run(Int32(AO_EVAL_PRINTIT))
  }

  func inspectIt() {
    run(Int32(AO_EVAL_INSPECTIT))
  }

  fileprivate func receiveInspect(className: String, printString: String) {
    inspectClassName = className
    inspectPrint = printString
    inspectorLinesFromHook = InspectorWindow.lines(className: className, printString: printString)
  }

  func debugIt() {
    run(Int32(AO_EVAL_DEBUGIT))
  }

  private func run(_ mode: Int32) {
    let range = sourceRange()
    let source = text(in: range)
    let result = withInspectHook(mode) { evaluate(source, mode: mode) }
    if result.status == Int32(AO_ERR_HALT) {
      // SPEC §3.9: the evaluating process halted; its live Debugger opens.
      debugButton.isHidden = true
      errorField.stringValue = "halted: " + result.message
      openLiveDebugger(pid: ao_debug_halted_pid(), mode: mode, range: range)
      return
    }
    show(result, mode: mode, source: source, range: range)
  }

  // An Inspect it's hook is set around `body`, whose inspect it hears.
  private func withInspectHook<T>(_ mode: Int32, _ body: () -> T) -> T {
    let inspecting = mode == Int32(AO_EVAL_INSPECTIT)
    if inspecting {
      inspectClassName = ""
      inspectPrint = ""
      inspectorLinesFromHook = ""
      inspectorLinesFromOut = ""
      ao_set_inspect_hook(aoWorkspaceInspectHook, Unmanaged.passUnretained(self).toOpaque())
    }
    defer {
      if inspecting {
        ao_set_inspect_hook(nil, nil)
      }
    }
    return body()
  }

  // The answer of an evaluation, or of the live Debugger's Proceed or Step that ended one. source
  // is the evaluated text when a compile error's span can be selected in it.
  private func show(
    _ result: (status: Int32, output: String, message: String, span: AoSpan),
    mode: Int32,
    source: String?,
    range: NSRange
  ) {
    // SPEC §3.9: ao_eval clears the snapshot on entry, so frames now are this evaluation's.
    let captured = DebuggerWindow.snapshotFrameCount() > 0
    debugButton.isHidden = !captured
    if result.status != Int32(AO_OK) {
      errorField.stringValue = failureText(status: result.status, message: result.message)
      // SPEC §3.8: an ao_eval span counts from the start of the evaluated fragment.
      if let source {
        selectErrorSpan(
          status: result.status,
          span: result.span,
          source: source,
          base: range.location,
          in: textView
        )
      }
      return
    }
    // SPEC §3.9: AO_OK with a snapshot is a process that failed in the drain (§3.4).
    errorField.stringValue = captured ? "process failed: " + DebuggerWindow.snapshotReason() : ""
    if mode == Int32(AO_EVAL_PRINTIT) {
      insert(result.output, after: range)
      return
    }
    if mode == Int32(AO_EVAL_INSPECTIT) {
      inspectorLinesFromOut = InspectorWindow.lines(
        className: inspectClassName,
        printString: result.output
      )
      openInspector(className: inspectClassName, printString: inspectPrint)
      openInspector(className: inspectClassName, printString: result.output)
    }
  }

  // SPEC §3.9 ライブ Debugger: one window per halted process. Its Proceed or Step that ends the
  // evaluation answers here, as the evaluation would have.
  @discardableResult
  func openLiveDebugger(pid: Int64, mode: Int32, range: NSRange) -> DebuggerWindow? {
    guard pid > 0 else {
      return nil
    }
    if let existing = debuggers.first(where: { $0.pid == pid }) {
      existing.orderFront()
      return existing
    }
    let debugger = DebuggerWindow(
      pid: pid,
      around: { [weak self] body in
        guard let self else {
          body()
          return
        }
        self.withInspectHook(mode, body)
      },
      onFinish: { [weak self] outcome in
        self?.finishLive(outcome, mode: mode, range: range)
      },
      onClose: { [weak self] closed in
        self?.debuggers.removeAll { $0 === closed }
      }
    )
    debuggers.append(debugger)
    return debugger
  }

  // SPEC §3.9: a Print it inserts after the evaluated range (at the end when the text is shorter
  // now); nothing when this Workspace is closed.
  private func finishLive(_ outcome: DebugOutcome, mode: Int32, range: NSRange) {
    guard window.isVisible else {
      return
    }
    var status = outcome.status
    var output = outcome.output
    let answered = status == Int32(AO_OK) || status == Int32(AO_ERR_RANGE)
    let printing = mode == Int32(AO_EVAL_PRINTIT) || mode == Int32(AO_EVAL_INSPECTIT)
    if answered, printing, let whole = keptEvalResult() {
      status = Int32(AO_OK)
      output = whole
    }
    let end = min(NSMaxRange(range), (textView.string as NSString).length)
    show(
      (status, output, outcome.message, AoSpan()),
      mode: mode,
      source: nil,
      range: NSRange(location: end, length: 0)
    )
  }

  // Hook and out share one window when the lines match. A later Inspect it orders that window front.
  private func openInspector(className: String, printString: String) {
    let lines = InspectorWindow.lines(className: className, printString: printString)
    if let existing = inspectors.last, existing.text == lines {
      existing.window.makeKeyAndOrderFront(nil)
      return
    }
    // SPEC §3.9: a closed Inspector leaves the array, so the next Inspect it opens a new window.
    // Weak: the close may come after this Workspace is gone (a test's tearDown closes every window).
    let inspector = InspectorWindow(className: className, printString: printString) { [weak self] closed in
      self?.inspectors.removeAll { $0 === closed }
    }
    inspectors.append(inspector)
  }

  // SPEC §3.9: the Debug button. A Debugger already open on this snapshot (same generation) comes
  // to the front; otherwise a new one reads the snapshot. A closed Debugger leaves the array.
  @discardableResult
  func openDebugger() -> DebuggerWindow? {
    let generation = ao_debug_generation()
    if let existing = debuggers.first(where: { $0.pid == 0 && $0.generation == generation }) {
      existing.orderFront()
      return existing
    }
    guard DebuggerWindow.snapshotFrameCount() > 0 else {
      debugButton.isHidden = true
      return nil
    }
    let debugger = DebuggerWindow(onClose: { [weak self] closed in
      self?.debuggers.removeAll { $0 === closed }
    })
    debuggers.append(debugger)
    return debugger
  }

  private func evaluate(
    _ source: String,
    mode: Int32
  ) -> (status: Int32, output: String, message: String, span: AoSpan) {
    var out = [CChar](repeating: 0, count: aoEvalOutCapacity)
    var err = AoSpan()
    let status: Int32 = source.withCString { src in
      out.withUnsafeMutableBufferPointer { outBuf in
        guard let outPtr = outBuf.baseAddress else {
          return Int32(AO_ERR)
        }
        return withUnsafeMutablePointer(to: &err) { errPtr in
          ao_eval(src, Int32(source.utf8.count), mode, outPtr, Int32(outBuf.count), errPtr)
        }
      }
    }
    // SPEC §3.10: out stops at 64 KiB and at the first NUL, so an answered Print it or Inspect it
    // reads the kept result whole. Without one, AO_ERR_RANGE stays `result does not fit`.
    let answered = status == Int32(AO_OK) || status == Int32(AO_ERR_RANGE)
    if answered, mode != Int32(AO_EVAL_DOIT), let whole = keptEvalResult() {
      return (Int32(AO_OK), whole, spanMessage(err), err)
    }
    let output = out.withUnsafeBufferPointer { buf -> String in
      guard let base = buf.baseAddress else {
        return ""
      }
      return String(cString: base)
    }
    return (status, output, spanMessage(err), err)
  }

  // Empty selection is the caret's line, without the line break, so Print it stays on that line.
  private func sourceRange() -> NSRange {
    let selected = textView.selectedRange()
    if selected.length > 0 {
      return selected
    }
    return caretLineRange(at: selected.location)
  }

  private func caretLineRange(at location: Int) -> NSRange {
    let ns = textView.string as NSString
    let length = ns.length
    if length == 0 {
      return NSRange(location: 0, length: 0)
    }
    let clamped = min(max(location, 0), length)
    if clamped == length {
      let last = ns.character(at: length - 1)
      if last == 0x0A || last == 0x0D {
        return NSRange(location: length, length: 0)
      }
    }
    let probe = clamped == length ? length - 1 : clamped
    let line = ns.lineRange(for: NSRange(location: probe, length: 0))
    var end = line.location + line.length
    while end > line.location {
      let ch = ns.character(at: end - 1)
      if ch != 0x0A && ch != 0x0D {
        break
      }
      end -= 1
    }
    return NSRange(location: line.location, length: end - line.location)
  }

  private func text(in range: NSRange) -> String {
    let whole = textView.string
    guard let swiftRange = Range(range, in: whole) else {
      return ""
    }
    return String(whole[swiftRange])
  }

  // Through shouldChangeText and didChangeText, so Undo takes the printed text out again. The
  // coalescing breaks keep it one undo step apart from the typing around it.
  private func insert(_ printed: String, after range: NSRange) {
    let at = NSRange(location: range.location + range.length, length: 0)
    guard Range(at, in: textView.string) != nil, let storage = textView.textStorage else {
      return
    }
    textView.breakUndoCoalescing()
    guard textView.shouldChangeText(in: at, replacementString: printed) else {
      return
    }
    storage.replaceCharacters(in: at, with: printed)
    textView.didChangeText()
    textView.breakUndoCoalescing()
    let inserted = (printed as NSString).length
    textView.setSelectedRange(NSRange(location: at.location + inserted, length: 0))
  }
}

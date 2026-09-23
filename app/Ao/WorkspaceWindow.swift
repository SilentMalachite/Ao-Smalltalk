import AppKit
import CAo

private let aoEvalOutCapacity = 65_536

// C function pointer: no captures. `user` is the WorkspaceWindow.
private func aoWorkspaceInspectHook(
  _ className: UnsafePointer<CChar>?,
  _ printUtf8: UnsafePointer<CChar>?,
  _ user: UnsafeMutableRawPointer?
) {
  guard let user else {
    return
  }
  let classText = className.map { String(cString: $0) } ?? ""
  let printText = printUtf8.map { String(cString: $0) } ?? ""
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

private func spanMessage(_ span: AoSpan) -> String {
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

private func failureText(status: Int32, message: String) -> String {
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

@MainActor
final class WorkspaceWindow {
  let window: NSWindow
  let errorField: NSTextField
  private let textView: NSTextView
  private var inspectors: [InspectorWindow] = []
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

  var inspectorText: String {
    inspectors.last?.text ?? ""
  }

  init() {
    let built = makeToolTextWindow(
      title: "Workspace",
      frame: NSRect(x: 140, y: 80, width: 560, height: 360),
      editable: true
    )
    window = built.window
    textView = built.textView
    errorField = installErrorField(on: built.window, textView: built.textView)
    window.makeKeyAndOrderFront(nil)
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

  private func run(_ mode: Int32) {
    let range = sourceRange()
    let source = text(in: range)
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
    let result = evaluate(source, mode: mode)
    if result.status != Int32(AO_OK) {
      errorField.stringValue = failureText(status: result.status, message: result.message)
      return
    }
    errorField.stringValue = ""
    if mode == Int32(AO_EVAL_PRINTIT) {
      insert(result.output, after: range)
      return
    }
    if inspecting {
      inspectorLinesFromOut = InspectorWindow.lines(
        className: inspectClassName,
        printString: result.output
      )
      openInspector(className: inspectClassName, printString: inspectPrint)
      openInspector(className: inspectClassName, printString: result.output)
    }
  }

  private func openInspector(className: String, printString: String) {
    let lines = InspectorWindow.lines(className: className, printString: printString)
    if inspectors.last?.text == lines {
      return
    }
    inspectors.append(InspectorWindow(className: className, printString: printString))
  }

  private func evaluate(_ source: String, mode: Int32) -> (status: Int32, output: String, message: String) {
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
    let output = out.withUnsafeBufferPointer { buf -> String in
      guard let base = buf.baseAddress else {
        return ""
      }
      return String(cString: base)
    }
    return (status, output, spanMessage(err))
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

  private func insert(_ printed: String, after range: NSRange) {
    let whole = textView.string
    let at = NSRange(location: range.location + range.length, length: 0)
    guard let swiftRange = Range(at, in: whole) else {
      return
    }
    textView.string = whole.replacingCharacters(in: swiftRange, with: printed)
    let inserted = (printed as NSString).length
    textView.setSelectedRange(NSRange(location: at.location + inserted, length: 0))
  }
}

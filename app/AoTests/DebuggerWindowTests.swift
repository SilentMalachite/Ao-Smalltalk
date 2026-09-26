import AppKit
import XCTest
@testable import Ao

// SPEC §3.9 Debugger, §3.10 デバッガの読み出し, §3.13.
@MainActor
final class DebuggerWindowTests: XCTestCase {
  override func setUp() {
    super.setUp()
    ao_runtime_shutdown()
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
    ao_set_debug_capture(1)
  }

  override func tearDown() {
    UserDefaults.standard.removeObject(forKey: "AoTextSizeOffset")
    // tearDown is nonisolated; XCTest calls it on the main thread.
    MainActor.assumeIsolated {
      Self.closeVisibleWindows()
      Self.dropMainMenu()
    }
    ao_set_transcript_hook(nil, nil)
    ao_set_inspect_hook(nil, nil)
    ao_set_debug_capture(0)
    ao_runtime_shutdown()
    super.tearDown()
  }

  // Nothing the test opened outlives it: sheets end first, then every visible window closes.
  private static func closeVisibleWindows() {
    let windows = NSApplication.shared.windows
    for window in windows {
      for sheet in window.sheets {
        window.endSheet(sheet)
      }
    }
    for window in windows where window.isVisible && !window.isSheet {
      window.isReleasedWhenClosed = false
      window.close()
    }
  }

  // A menu built by AoApp holds the AoApp in its closures; dropping it lets the AoApp go now.
  private static func dropMainMenu() {
    let menu = NSApplication.shared.mainMenu
    NSApplication.shared.mainMenu = nil
    NSApplication.shared.windowsMenu = nil
    menu?.removeAllItems()
  }

  // Innermost first; a second Debug on the same snapshot brings the same window to the front.
  func testDoItErrorShowsDebugButtonAndOpensDebuggerWithInnermostFrameFirst() {
    let workspace = workspace("#(1 2) at: 5")
    XCTAssertTrue(workspace.debugButton.isHidden)
    workspace.selectAll()
    workspace.doIt()
    XCTAssertFalse(workspace.errorText.isEmpty)
    XCTAssertFalse(workspace.errorText.hasPrefix("process failed:"))
    XCTAssertFalse(workspace.debugButton.isHidden)
    workspace.debugButton.performClick(nil)
    guard let debugger = workspace.debuggers.last else {
      XCTFail("Debug opened no Debugger")
      return
    }
    XCTAssertEqual(debugger.frameLabels, ["ArrayedCollection>>at: native ao_ArrayedCollection_at_", "doIt"])
    XCTAssertEqual(debugger.title, "Debugger: " + workspace.errorText)
    XCTAssertTrue(debugger.window.isVisible)
    XCTAssertEqual(debugger.selectedFrame, 0)
    workspace.debugButton.performClick(nil)
    XCTAssertEqual(workspace.debuggers.count, 1)
    XCTAssertTrue(workspace.debuggers.last === debugger)
  }

  // The pc's span is UTF-8 bytes; the window selects it in UTF-16 past a comment with an emoji.
  func testSelectingFrameSelectsFailingSendSpanInSource() {
    defineDbgWin()
    guard let debugger = debugAfterDoIt("DbgWin new outer: 'ab'") else {
      return
    }
    XCTAssertEqual(
      debugger.frameLabels,
      ["Object>>error: native ao_Object_error_", "DbgWin>>inner:", "DbgWin>>outer:", "doIt"]
    )
    debugger.selectFrame(1)
    XCTAssertEqual(debugger.sourceText, Self.innerSource)
    XCTAssertEqual(debugger.sourceSelection, (Self.innerSource as NSString).range(of: "self error: s"))
    debugger.selectFrame(2)
    XCTAssertEqual(debugger.sourceText, Self.outerSource)
    XCTAssertEqual(debugger.sourceSelection, (Self.outerSource as NSString).range(of: "self inner: y"))
    debugger.selectFrame(3)
    XCTAssertEqual(debugger.sourceText, "DbgWin new outer: 'ab'")
    XCTAssertEqual(debugger.sourceSelection, NSRange(location: 0, length: 22))
  }

  // self first, then the arguments and the temps in declaration order, each with its printString.
  func testVariablesListSelfArgsAndTempsWithPrintStrings() {
    defineDbgWin()
    guard let debugger = debugAfterDoIt("DbgWin new outer: 'ab'") else {
      return
    }
    debugger.selectFrame(2)
    XCTAssertEqual(debugger.variableCount, 3)
    let rows = (0..<debugger.variableCount).map { debugger.variable(at: $0) }
    XCTAssertEqual(rows.map(\.name), ["self", "x", "y"])
    XCTAssertEqual(rows.map(\.className), ["DbgWin", "String", "String"])
    XCTAssertEqual(rows.map(\.value), ["DbgWin", "'ab'", "'ab'"])
    debugger.selectFrame(0)
    XCTAssertEqual((0..<debugger.variableCount).map { debugger.variable(at: $0).name }, ["self", "arg1"])
    XCTAssertEqual(debugger.variable(at: 1).value, "'ab'")
  }

  // A double click inspects that value; the Debugger reuses its Inspector for the same lines.
  func testDoubleClickVariableOpensInspector() {
    defineDbgWin()
    guard let debugger = debugAfterDoIt("DbgWin new outer: 'ab'") else {
      return
    }
    debugger.selectFrame(2)
    guard let table = table(labelled: "Variables", in: debugger.window) else {
      XCTFail("missing variables table")
      return
    }
    doubleClick(row: 1, in: table)
    XCTAssertEqual(debugger.inspectorCount, 1)
    XCTAssertEqual(debugger.inspectorText, "String\n'ab'")
    doubleClick(row: 1, in: table)
    XCTAssertEqual(debugger.inspectorCount, 1)
    doubleClick(row: 0, in: table)
    XCTAssertEqual(debugger.inspectorCount, 2)
    XCTAssertEqual(debugger.inspectorText, "DbgWin\nDbgWin")
    XCTAssertEqual(debugger.frameLabels.count, 4)
  }

  // A native frame has only a placeholder: nothing is selected and the pane stays read only.
  func testNoSourceFrameShowsPlaceholderReadOnly() {
    guard let debugger = debugAfterDoIt("#(1 2) at: 5") else {
      return
    }
    debugger.selectFrame(1)
    XCTAssertEqual(debugger.sourceSelection.length, 12)
    debugger.selectFrame(0)
    XCTAssertEqual(debugger.sourceText, "\"ArrayedCollection>>at: native ao_ArrayedCollection_at_\"")
    XCTAssertEqual(debugger.sourceSelection.length, 0)
    XCTAssertFalse(debugger.sourceIsEditable)
  }

  // The next evaluation clears the snapshot: the open Debugger neither prints nor inspects.
  func testNextEvalDisablesInspectInOpenDebugger() {
    defineDbgWin()
    let workspace = workspace("DbgWin new outer: 'ab'")
    workspace.selectAll()
    workspace.doIt()
    workspace.debugButton.performClick(nil)
    guard let debugger = workspace.debuggers.last,
          let table = table(labelled: "Variables", in: debugger.window) else {
      XCTFail("Debug opened no Debugger")
      return
    }
    workspace.replaceText("3 + 4")
    workspace.selectAll()
    workspace.doIt()
    XCTAssertTrue(workspace.debugButton.isHidden)
    XCTAssertEqual(workspace.errorText, "")
    debugger.selectFrame(2)
    XCTAssertEqual(debugger.variableCount, 3)
    XCTAssertEqual(debugger.variable(at: 1).name, "x")
    XCTAssertEqual(debugger.variable(at: 1).value, "-")
    doubleClick(row: 1, in: table)
    XCTAssertEqual(debugger.inspectorCount, 0)
    // The labels and the source were read when the window opened.
    XCTAssertEqual(debugger.sourceText, Self.outerSource)
  }

  // SPEC §3.9 文字の大きさ: through the app's own menu, an open Debugger's source pane follows,
  // and so does the next frame's source.
  func testDebuggerSourceFollowsTextSize() {
    ao_runtime_shutdown()
    let app = AoApp()
    app.applicationWillFinishLaunching(
      Notification(name: NSApplication.willFinishLaunchingNotification, object: NSApplication.shared)
    )
    guard let tools = NSApplication.shared.mainMenu?.item(withTitle: "Tools")?.submenu,
          let bigger = tools.item(withTitle: "Make Text Bigger"), let action = bigger.action,
          let variable = NSFont.userFont(ofSize: 0)?.pointSize else {
      XCTFail("missing Tools menu or default font")
      return
    }
    guard let debugger = debugAfterDoIt("#(1 2) at: 5") else {
      return
    }
    debugger.selectFrame(1)
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: bigger.target, from: bigger))
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: bigger.target, from: bigger))
    let source = textViews(in: debugger.window.contentView).first
    assertWholeText(of: source, uses: NSFont.userFont(ofSize: variable + 2))
    debugger.selectFrame(0)
    assertWholeText(of: source, uses: NSFont.userFont(ofSize: variable + 2))
  }

  // No capture for a compile error: the button stays hidden and the error reads as before.
  func testCompileErrorShowsNoDebugButton() {
    let workspace = workspace("nil foo")
    workspace.selectAll()
    workspace.doIt()
    XCTAssertFalse(workspace.debugButton.isHidden)
    workspace.replaceText("3 +")
    workspace.selectAll()
    workspace.doIt()
    XCTAssertTrue(workspace.debugButton.isHidden)
    XCTAssertFalse(workspace.errorText.isEmpty)
    XCTAssertFalse(workspace.errorText.hasPrefix("process failed:"))
  }

  // A process failing in the drain: ao_eval answers AO_OK, the band shows `process failed:`.
  func testProcessFailureShowsReasonAndDebugButton() {
    let workspace = workspace("[nil bar] fork. 3")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertTrue(workspace.text.hasSuffix("3"))
    XCTAssertTrue(workspace.errorText.hasPrefix("process failed: "), workspace.errorText)
    XCTAssertTrue(workspace.errorText.contains("bar"), workspace.errorText)
    XCTAssertFalse(workspace.debugButton.isHidden)
    workspace.debugButton.performClick(nil)
    guard let debugger = workspace.debuggers.last else {
      XCTFail("Debug opened no Debugger")
      return
    }
    XCTAssertEqual(debugger.frameLabels, ["#bar (doesNotUnderstand:)", "[] in doIt"])
    XCTAssertEqual(debugger.title, "Debugger: " + workspace.errorText.dropFirst("process failed: ".count))
    debugger.selectFrame(1)
    XCTAssertEqual(debugger.sourceSelection, NSRange(location: 1, length: 7))
  }

  func testDebuggerControlsHaveAccessibilityLabels() {
    guard let debugger = debugAfterDoIt("nil foo") else {
      return
    }
    let tables = views(in: debugger.window.contentView, of: NSTableView.self)
    XCTAssertEqual(Set(tables.compactMap { $0.accessibilityLabel() }), ["Frames", "Variables"])
    XCTAssertEqual(textViews(in: debugger.window.contentView).first?.accessibilityLabel(), "Source")
    guard let variables = table(labelled: "Variables", in: debugger.window),
          let frames = table(labelled: "Frames", in: debugger.window) else {
      XCTFail("missing tables")
      return
    }
    XCTAssertEqual(frames.tableColumns.map { $0.headerCell.accessibilityLabel() }, ["Frame"])
    XCTAssertEqual(variables.tableColumns.map { $0.headerCell.accessibilityLabel() }, ["Name", "Class", "Value"])
    XCTAssertEqual(variables.tableColumns.map(\.title), ["Name", "Class", "Value"])
    let workspace = workspace("nil foo")
    XCTAssertEqual(workspace.debugButton.accessibilityLabel(), "Debug")
    XCTAssertEqual(workspace.debugButton.title, "Debug")
  }

  // P10 受け入れ: a DNU in a doIt; the doIt frame selects the whole send `nil foo`.
  func testNilFooDebugSelectsTheSendInTheDoIt() {
    guard let debugger = debugAfterDoIt("nil foo") else {
      return
    }
    XCTAssertEqual(debugger.frameLabels, ["#foo (doesNotUnderstand:)", "doIt"])
    debugger.selectFrame(1)
    XCTAssertEqual(debugger.sourceText, "nil foo")
    XCTAssertEqual(debugger.sourceSelection, NSRange(location: 0, length: 7))
  }

  // P10 受け入れ: `self halt` aborts with the reason `halt` and shows the Debug button.
  func testHaltShowsDebugButtonWithHaltReason() {
    let workspace = workspace("self halt")
    workspace.selectAll()
    workspace.doIt()
    XCTAssertEqual(workspace.errorText, "halt")
    XCTAssertFalse(workspace.debugButton.isHidden)
    workspace.debugButton.performClick(nil)
    guard let debugger = workspace.debuggers.last else {
      XCTFail("Debug opened no Debugger")
      return
    }
    XCTAssertEqual(debugger.frameLabels, ["Object>>halt native ao_Object_halt", "doIt"])
    XCTAssertEqual(debugger.title, "Debugger: halt")
  }

  // P10 受け入れ: the `[] in` frame lists the block argument with its value; a double click
  // inspects it.
  func testBlockFrameShowsArgumentAndDoubleClickInspectsIt() {
    guard let debugger = debugAfterDoIt("#(1 2) do: [:e | e foo]") else {
      return
    }
    XCTAssertEqual(debugger.frameLabels.first, "#foo (doesNotUnderstand:)")
    XCTAssertEqual(debugger.frameLabels.dropFirst().first, "[] in doIt")
    debugger.selectFrame(1)
    XCTAssertEqual((0..<debugger.variableCount).map { debugger.variable(at: $0).name }, ["self", "e"])
    XCTAssertEqual(debugger.variable(at: 1).className, "SmallInteger")
    XCTAssertEqual(debugger.variable(at: 1).value, "1")
    XCTAssertEqual(debugger.sourceSelection, ("#(1 2) do: [:e | e foo]" as NSString).range(of: "e foo"))
    guard let table = table(labelled: "Variables", in: debugger.window) else {
      XCTFail("missing variables table")
      return
    }
    doubleClick(row: 1, in: table)
    XCTAssertEqual(debugger.inspectorCount, 1)
    XCTAssertEqual(debugger.inspectorText, "SmallInteger\n1")
  }

  // MARK: - helpers

  private static let outerSource = "outer: x\n  | y |\n  y := x.\n  ^self inner: y"
  private static let innerSource = "inner: s\n  \"寿司🍣\"\n  ^self error: s"

  private func defineDbgWin() {
    var err = AoSpan()
    let definition =
      "Object subclass: #DbgWin\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'P10-Test'!\n"
    let defined = definition.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
    XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    for source in [Self.outerSource, Self.innerSource] {
      let added = source.withCString { src in
        withUnsafeMutablePointer(to: &err) { ao_accept_method("DbgWin", 0, src, $0) }
      }
      XCTAssertEqual(added, Int32(AO_OK), spanMessage(err))
    }
  }

  private func debugAfterDoIt(_ text: String, line: UInt = #line) -> DebuggerWindow? {
    let workspace = workspace(text)
    workspace.selectAll()
    workspace.doIt()
    workspace.debugButton.performClick(nil)
    guard let debugger = workspace.debuggers.last else {
      XCTFail("Debug opened no Debugger", line: line)
      return nil
    }
    return debugger
  }

  private func doubleClick(row: Int, in table: NSTableView) {
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    guard let action = table.doubleAction else {
      XCTFail("no double action")
      return
    }
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: table.target, from: table))
  }

  private func table(labelled label: String, in window: NSWindow) -> NSTableView? {
    views(in: window.contentView, of: NSTableView.self).first { $0.accessibilityLabel() == label }
  }

  private func assertWholeText(of view: NSTextView?, uses font: NSFont?, line: UInt = #line) {
    guard let view, let storage = view.textStorage, storage.length > 0, let font else {
      XCTFail("missing source text or font", line: line)
      return
    }
    storage.enumerateAttribute(.font, in: NSRange(location: 0, length: storage.length)) { value, range, _ in
      XCTAssertEqual(value as? NSFont, font, "at \(range)", line: line)
    }
    XCTAssertEqual(view.typingAttributes[.font] as? NSFont, font, line: line)
  }

  private func textViews(in root: NSView?) -> [NSTextView] {
    views(in: root, of: NSTextView.self)
  }

  private func views<T: NSView>(in root: NSView?, of _: T.Type) -> [T] {
    guard let root else {
      return []
    }
    var found: [T] = []
    if let match = root as? T {
      found.append(match)
    }
    let children = (root as? NSSplitView)?.arrangedSubviews ?? root.subviews
    for child in children {
      found.append(contentsOf: views(in: child, of: T.self))
    }
    return found
  }

  private func workspace(_ text: String) -> WorkspaceWindow {
    let workspace = WorkspaceWindow()
    workspace.replaceText(text)
    return workspace
  }
}

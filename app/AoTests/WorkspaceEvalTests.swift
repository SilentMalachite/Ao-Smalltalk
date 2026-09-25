import AppKit
import XCTest
@testable import Ao

@MainActor
final class WorkspaceEvalTests: XCTestCase {
  override func setUp() {
    super.setUp()
    ao_runtime_shutdown()
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
  }

  override func tearDown() {
    // tearDown is nonisolated; XCTest calls it on the main thread.
    MainActor.assumeIsolated { Self.closeVisibleWindows() }
    ao_set_transcript_hook(nil, nil)
    ao_set_inspect_hook(nil, nil)
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

  func testPrintItInsertsThreeAfterSelectedSum() {
    let workspace = workspace("1 + 2")
    workspace.selectAll()
    workspace.printIt()
    let source = "1 + 2"
    XCTAssertTrue(workspace.text.hasPrefix(source))
    XCTAssertEqual(String(workspace.text.dropFirst(source.count)), "3")
  }

  func testDoItKeepsAssignmentAndLaterPrintItInsertsFive() {
    let workspace = workspace("x := 5")
    workspace.selectAll()
    workspace.doIt()
    XCTAssertEqual(workspace.text, "x := 5")
    workspace.replaceText("x")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.text, "x5")
    XCTAssertEqual(String(workspace.text.dropFirst("x".count)), "5")
  }

  func testInspectItShowsClassNameAndPrintString() {
    let workspace = workspace("1 + 2")
    workspace.selectAll()
    workspace.inspectIt()
    XCTAssertEqual(workspace.text, "1 + 2")
    XCTAssertEqual(workspace.inspectorLinesFromHook, "SmallInteger\n3")
    XCTAssertEqual(workspace.inspectorLinesFromOut, "SmallInteger\n3")
    XCTAssertEqual(workspace.inspectorText, "SmallInteger\n3")
    let lines = workspace.inspectorText.split(separator: "\n", omittingEmptySubsequences: false).map(String.init)
    XCTAssertEqual(lines, ["SmallInteger", "3"])
  }

  func testCompileFailureKeepsTextAndShowsError() {
    let workspace = workspace("1 +")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.text, "1 +")
    XCTAssertFalse(workspace.errorText.isEmpty)
    XCTAssertNotNil(workspace.errorField.superview)
    workspace.replaceText("1 + 2")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(String(workspace.text.dropFirst("1 + 2".count)), "3")
    XCTAssertEqual(workspace.errorText, "")
  }

  func testKeyWindowGuardSkipsCommandsForAnotherWindow() {
    let workspace = workspace("1 + 2")
    workspace.selectAll()
    let other = NSWindow(
      contentRect: NSRect(x: 0, y: 0, width: 120, height: 80),
      styleMask: [.titled, .closable],
      backing: .buffered,
      defer: false
    )
    sendToKeyWorkspace(workspace, keyWindow: other) { $0.printIt() }
    XCTAssertEqual(workspace.text, "1 + 2")
    sendToKeyWorkspace(workspace, keyWindow: workspace.window) { $0.printIt() }
    XCTAssertEqual(String(workspace.text.dropFirst("1 + 2".count)), "3")
    let printed = workspace.text
    sendToKeyWorkspace(nil, keyWindow: workspace.window) { $0.printIt() }
    XCTAssertEqual(workspace.text, printed)
  }

  func testRepeatedInspectItOrdersInspectorFront() {
    let workspace = workspace("1 + 2")
    workspace.selectAll()
    workspace.inspectIt()
    XCTAssertEqual(workspace.inspectorCount, 1)
    guard let inspector = workspace.inspectorWindow else {
      XCTFail("missing inspector")
      return
    }
    inspector.close()
    workspace.orderFront()
    XCTAssertFalse(inspector.isVisible)
    workspace.selectAll()
    workspace.inspectIt()
    XCTAssertEqual(workspace.inspectorCount, 1)
    XCTAssertTrue(inspector.isVisible)
    let orderedFront = NSApplication.shared.orderedWindows.first === inspector
    XCTAssertTrue(inspector.isKeyWindow || orderedFront)
  }

  func testEmptySelectionPrintItEvaluatesCaretLine() {
    let workspace = workspace("1 + 2\n4 + 5")
    let caret = ("1 + 2\n4" as NSString).length
    workspace.setCaret(caret)
    workspace.printIt()
    XCTAssertEqual(workspace.text, "1 + 2\n4 + 59")
    XCTAssertEqual(String(workspace.text.dropFirst("1 + 2\n4 + 5".count)), "9")
  }

  func testUndoRemovesPrintItResultAndRedoPutsItBack() {
    let workspace = workspace("1 + 2")
    defer { workspace.window.close() }
    guard let view = textView(in: workspace.window.contentView), let undo = view.undoManager else {
      XCTFail("missing Workspace text view or undo manager")
      return
    }
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.text, "1 + 23")
    RunLoop.current.run(until: Date(timeIntervalSinceNow: 0.05))
    XCTAssertTrue(undo.canUndo)
    undo.undo()
    XCTAssertEqual(workspace.text, "1 + 2")
    undo.redo()
    XCTAssertEqual(workspace.text, "1 + 23")
  }

  // SPEC §3.9, §3.10: a result past the 64 KiB out is inserted whole, and the source runs once.
  func testPrintItInsertsWholeResultPast64KiBAndRunsOnce() {
    let workspace = workspace("b11n := 0")
    workspace.selectAll()
    workspace.doIt()
    let source = "b11n := b11n + 1. Array new: 20000"
    workspace.replaceText(source)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.errorText, "")
    XCTAssertTrue(workspace.text.hasPrefix(source))
    let inserted = (workspace.text as NSString).length - (source as NSString).length
    XCTAssertGreaterThan(inserted, 65_536)
    XCTAssertTrue(workspace.text.hasSuffix(")"))
    workspace.replaceText("b11n")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.text, "b11n1")
  }

  // SPEC §3.9: a printString holding NUL bytes is inserted whole, not cut at the first NUL.
  func testPrintItInsertsResultWithNulBytes() {
    let small = "String new: 3"
    let workspace = workspace(small)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.errorText, "")
    XCTAssertEqual(String(workspace.text.dropFirst(small.count)), "'\0\0\0'")

    let large = "String new: 70000"
    workspace.replaceText(large)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.errorText, "")
    let inserted = String(workspace.text.dropFirst(large.count))
    XCTAssertEqual((inserted as NSString).length, 70_002)
    XCTAssertEqual(inserted, "'" + String(repeating: "\0", count: 70_000) + "'")
  }

  // The hook and the result carry the same whole text, so Inspect it opens one window.
  func testInspectItWithNulOpensOneInspectorWithWholeText() {
    let workspace = workspace("String new: 3")
    workspace.selectAll()
    workspace.inspectIt()
    XCTAssertEqual(workspace.errorText, "")
    XCTAssertEqual(workspace.inspectorCount, 1)
    XCTAssertEqual(workspace.inspectorText, "String\n'\0\0\0'")
  }

  private func textView(in root: NSView?) -> NSTextView? {
    guard let root else {
      return nil
    }
    if let match = root as? NSTextView {
      return match
    }
    for child in root.subviews {
      if let found = textView(in: child) {
        return found
      }
    }
    return nil
  }

  private func workspace(_ text: String) -> WorkspaceWindow {
    let workspace = WorkspaceWindow()
    workspace.replaceText(text)
    return workspace
  }
}

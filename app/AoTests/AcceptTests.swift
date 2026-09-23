import AppKit
import XCTest
@testable import Ao

@MainActor
final class AcceptTests: XCTestCase {
  override func setUp() {
    super.setUp()
    ao_runtime_shutdown()
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
  }

  override func tearDown() {
    ao_runtime_shutdown()
    super.tearDown()
  }

  func testAcceptFooThenPrintItInsertsOneAndFailedAcceptKeepsText() {
    let browser = BrowserWindow()
    let foo = "foo\n  ^1\n"
    browser.replaceSource(foo)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")

    let workspace = WorkspaceWindow()
    let expr = "Object new foo"
    workspace.replaceText(expr)
    workspace.selectAll()
    workspace.doIt()
    XCTAssertEqual(workspace.text, expr)
    XCTAssertEqual(workspace.errorText, "")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(workspace.text, expr + "1")
    XCTAssertEqual(String(workspace.text.dropFirst(expr.count)), "1")

    let bad = "foo\n  ^\n"
    browser.replaceSource(bad)
    browser.accept()
    XCTAssertEqual(browser.sourceText, bad)
    XCTAssertFalse(browser.errorText.isEmpty)
  }

  func testPrintStringOverwriteKeepsTextAndNativeResult() {
    let browser = BrowserWindow()
    let source = "printString\n  ^1\n"
    browser.replaceSource(source)
    browser.accept()
    XCTAssertEqual(browser.sourceText, source)
    XCTAssertFalse(browser.errorText.isEmpty)

    let workspace = WorkspaceWindow()
    let expr = "Object new printString"
    workspace.replaceText(expr)
    workspace.selectAll()
    workspace.printIt()
    // Print it sends printString again, so the class name is a quoted string, not 1.
    XCTAssertEqual(String(workspace.text.dropFirst(expr.count)), "'Object'")
  }

  func testClassDefinitionPaneAcceptsClassSource() {
    let browser = BrowserWindow()
    selectClass("Array", in: browser)
    XCTAssertNil(browser.model.selectedSelector)
    let def =
      "Object subclass: #P9BrowserFoo\n"
      + "  instanceVariableNames: ''\n"
      + "  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n"
      + "  category: 'P9-Test'\n"
    browser.replaceSource(def)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertTrue(browser.model.categories.contains("P9-Test"))
  }

  func testClassSideAcceptUsesMeta() {
    let browser = BrowserWindow()
    selectClass("Transcript", in: browser)
    guard let side = segmentedControls(in: browser.window.contentView).first else {
      XCTFail("missing instance/class switch")
      return
    }
    side.selectedSegment = 1
    XCTAssertTrue(side.sendAction(side.action, to: side.target))
    guard let row = browser.model.selectors.firstIndex(of: "show:") else {
      XCTFail("missing class-side show:")
      return
    }
    guard let selectors = selectorTable(in: browser) else {
      XCTFail("missing selector table")
      return
    }
    selectors.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    browser.replaceSource("extra\n  ^7\n")
    browser.accept()
    XCTAssertEqual(browser.errorText, "")

    let workspace = WorkspaceWindow()
    let expr = "Transcript extra"
    workspace.replaceText(expr)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(String(workspace.text.dropFirst(expr.count)), "7")
  }

  func testAcceptRunsOnlyWhenBrowserIsKey() {
    let browser = BrowserWindow()
    browser.replaceSource("foo\n  ^1\n")
    let workspace = WorkspaceWindow()
    sendToKeyBrowser(browser, keyWindow: workspace.window) { $0.accept() }
    let expr = "Object new foo"
    workspace.replaceText(expr)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertNotEqual(String(workspace.text.dropFirst(expr.count)), "1")

    sendToKeyBrowser(browser, keyWindow: browser.window) { $0.accept() }
    workspace.replaceText("Object new foo")
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(String(workspace.text.dropFirst("Object new foo".count)), "1")
  }

  func testShowHierarchyIncludesIntegerThenReturnsToClassList() {
    let browser = BrowserWindow()
    selectClass("SmallInteger", in: browser)
    browser.showHierarchy()
    XCTAssertTrue(browser.model.classes.contains("Integer"))
    guard let table = classTable(in: browser) else {
      XCTFail("missing class table")
      return
    }
    XCTAssertEqual(table.numberOfRows, browser.model.classes.count)
    let hierarchyCount = browser.model.classes.count
    browser.showHierarchy()
    XCTAssertGreaterThan(browser.model.classes.count, hierarchyCount)
    XCTAssertTrue(browser.model.classes.contains("Array"))
    XCTAssertEqual(table.numberOfRows, browser.model.classes.count)
  }

  func testAccessibilityLabelsMatchTheSpec() {
    let browser = BrowserWindow()
    let transcript = TranscriptWindow()
    let workspace = WorkspaceWindow()
    let labels = Set(
      browser.paneAccessibilityLabels.compactMap { $0 }
        + [
          transcript.textAccessibilityLabel,
          workspace.textAccessibilityLabel,
          workspace.errorAccessibilityLabel,
        ].compactMap { $0 }
    )
    XCTAssertEqual(
      labels,
      Set([
        "Class categories",
        "Classes",
        "Protocols",
        "Selectors",
        "Source",
        "Transcript",
        "Workspace",
        "Error",
      ])
    )
  }

  func testFileMenuImageCommandsAndShowHierarchy() {
    let menu = MainMenu.build(actions: MainMenu.Actions())
    let titles = menuTitles(in: menu)
    XCTAssertTrue(titles.contains("Save Image…"))
    XCTAssertTrue(titles.contains("Open Image…"))
    XCTAssertTrue(titles.contains("Show Hierarchy"))
    let file = menu.item(withTitle: "File")?.submenu
    XCTAssertEqual(
      file?.items.map(\.title),
      ["Save Image…", "Open Image…", "", "Close"]
    )
  }

  func testOpenImagePathDeliversTranscriptShow() {
    let transcript = TranscriptWindow()
    transcript.installHook()
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-accept-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(saveImageFile(at: url), Int32(AO_OK))
    XCTAssertEqual(openImageFile(at: url, transcript: transcript), Int32(AO_OK))
    var out = [CChar](repeating: 0, count: 64)
    var err = AoSpan()
    let source = "Transcript show: 'z'"
    let status: Int32 = source.withCString { src in
      out.withUnsafeMutableBufferPointer { buffer in
        guard let base = buffer.baseAddress else {
          return Int32(AO_ERR)
        }
        return withUnsafeMutablePointer(to: &err) { errPtr in
          ao_eval(src, Int32(source.utf8.count), Int32(AO_EVAL_DOIT), base, Int32(buffer.count), errPtr)
        }
      }
    }
    XCTAssertEqual(status, Int32(AO_OK))
    XCTAssertTrue(transcript.text.contains("z"))
  }

  private func selectClass(_ name: String, in browser: BrowserWindow) {
    guard let table = classTable(in: browser),
          let row = browser.model.classes.firstIndex(of: name) else {
      XCTFail("missing class \(name)")
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    XCTAssertEqual(browser.model.selectedClass, name)
  }

  private func classTable(in browser: BrowserWindow) -> NSTableView? {
    guard let outer = browser.window.contentView as? NSSplitView,
          let top = outer.arrangedSubviews.first as? NSSplitView else {
      return nil
    }
    let tables = tableViews(in: top)
    return tables.count >= 2 ? tables[1] : nil
  }

  private func selectorTable(in browser: BrowserWindow) -> NSTableView? {
    guard let outer = browser.window.contentView as? NSSplitView,
          outer.arrangedSubviews.count >= 2,
          let middle = outer.arrangedSubviews[1] as? NSSplitView else {
      return nil
    }
    let tables = tableViews(in: middle)
    return tables.count >= 2 ? tables[1] : nil
  }

  private func menuTitles(in menu: NSMenu) -> [String] {
    var titles: [String] = []
    for item in menu.items {
      titles.append(item.title)
      if let submenu = item.submenu {
        titles.append(contentsOf: menuTitles(in: submenu))
      }
    }
    return titles
  }

  private func tableViews(in root: NSView?) -> [NSTableView] {
    views(in: root, of: NSTableView.self)
  }

  private func segmentedControls(in root: NSView?) -> [NSSegmentedControl] {
    views(in: root, of: NSSegmentedControl.self)
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
}

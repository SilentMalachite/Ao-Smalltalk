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
    selectProtocol("user", in: browser)
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
    // A new method named printString: the runtime refuses to replace the native.
    selectProtocol("user", in: browser)
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

  // 00 Critical, app side: a class and a protocol with no selector is a new method, never a
  // class definition.
  func testProtocolWithoutSelectorAcceptsANewMethod() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectClass("Array", in: browser)
    selectClass("Object", in: browser)
    selectProtocol("native", in: browser)
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertEqual(browser.sourceText, "")
    let source = "zork\n  ^42\n"
    browser.replaceSource(source)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("Object new zork"), "42")
  }

  func testAcceptSelectsTheAcceptedSelectorInUserAndShowsItsSource() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertEqual(browser.sourceText, "")
    let source = "zork: a with: b\n  ^a + b\n"
    browser.replaceSource(source)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(browser.model.selectedProtocol, "user")
    XCTAssertEqual(browser.model.selectedSelector, "zork:with:")
    XCTAssertEqual(browser.sourceText, source)
    XCTAssertEqual(selectedName(in: protocolTable(in: browser), values: browser.model.protocols), "user")
    XCTAssertEqual(
      selectedName(in: selectorTable(in: browser), values: browser.model.selectors),
      "zork:with:"
    )

    let replaced = "zork: a with: b\n  ^a - b\n"
    browser.replaceSource(replaced)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(browser.model.selectedSelector, "zork:with:")
    XCTAssertEqual(browser.sourceText, replaced)
    XCTAssertEqual(printIt("Object new zork: 5 with: 2"), "3")
  }

  func testNewClassTakesItsFirstMethodThroughTheUserProtocol() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectClass("Array", in: browser)
    browser.replaceSource(
      "Object subclass: #B5First\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'B5-Test'\n"
    )
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    selectCategory("B5-Test", in: browser)
    XCTAssertEqual(browser.model.selectedClass, "B5First")
    XCTAssertEqual(browser.model.protocols, ["user"])
    selectProtocol("user", in: browser)
    browser.replaceSource("answer\n  ^7\n")
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(browser.model.selectedSelector, "answer")
    XCTAssertEqual(printIt("B5First new answer"), "7")
  }

  // SPEC §3.10 AO_ERR_NOSOURCE: the placeholder pane is read only and Accept leaves the method
  // alone; a method with source, a new method and a class definition are editable.
  func testNoSourceMethodIsReadOnlyAndAcceptLeavesItAlone() {
    var err = AoSpan()
    let chunks =
      "Object subclass: #B5NoSrc\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B5-NoSrc'!\n"
      + "!B5NoSrc methodsFor: 'b5'!\neight\n  ^8! !\n"
    let defined = chunks.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
    XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    let seven = "seven\n  ^7\n"
    let added = seven.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_method("B5NoSrc", 0, src, $0) }
    }
    XCTAssertEqual(added, Int32(AO_OK), spanMessage(err))

    let browser = BrowserWindow()
    defer { browser.window.close() }
    guard let pane = sourceView(in: browser) else {
      XCTFail("missing source view")
      return
    }
    // The first selection, Object>>printString, is a native.
    XCTAssertTrue(browser.sourceText.contains("native ao_Object_printString"))
    XCTAssertFalse(pane.isEditable)

    selectCategory("B5-NoSrc", in: browser)
    XCTAssertTrue(pane.isEditable)
    selectProtocol("user", in: browser)
    selectSelector("eight", in: browser)
    XCTAssertEqual(browser.sourceText, "\"B5NoSrc>>eight source not available\"")
    XCTAssertFalse(pane.isEditable)
    browser.replaceSource("eight\n  ^9\n")
    browser.accept()
    XCTAssertFalse(browser.errorText.isEmpty)
    XCTAssertEqual(printIt("B5NoSrc new eight"), "8")

    selectSelector("seven", in: browser)
    XCTAssertEqual(browser.sourceText, seven)
    XCTAssertTrue(pane.isEditable)
    selectSelector("eight", in: browser)
    XCTAssertFalse(pane.isEditable)
    selectProtocol(nil, in: browser)
    XCTAssertTrue(browser.sourceText.contains("subclass: #B5NoSrc"))
    XCTAssertTrue(pane.isEditable)
    selectProtocol("user", in: browser)
    XCTAssertEqual(browser.sourceText, "")
    XCTAssertTrue(pane.isEditable)
  }

  // 07 Medium: an unaccepted edit asks before any selection change; Cancel puts the rows back
  // and keeps the edit.
  func testUnacceptedEditAsksBeforeSelectionChangeAndCancelKeepsIt() {
    var err = AoSpan()
    let other = "Object subclass: #B5Other\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B5-Other'\n"
    let defined = other.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
    XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    let browser = BrowserWindow()
    defer { browser.window.close() }
    var asked = 0
    browser.confirmDiscard = { window, decide in
      XCTAssertTrue(window === browser.window)
      asked += 1
      decide(false)
    }
    selectProtocol("user", in: browser)
    browser.replaceSource("b5a\n  ^1\n")
    browser.accept()
    browser.replaceSource("b5b\n  ^2\n")
    browser.accept()
    XCTAssertEqual(browser.model.selectedSelector, "b5b")
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertEqual(asked, 0)

    let edited = "b5b\n  ^3\n"
    browser.replaceSource(edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    selectSelector("b5a", in: browser, expecting: "b5b")
    selectProtocol("native", in: browser, expecting: "user")
    selectClass("Array", in: browser, expecting: "Object")
    selectCategory("B5-Other", in: browser)
    guard let side = segmentedControls(in: browser.window.contentView).first else {
      XCTFail("missing instance/class switch")
      return
    }
    side.selectedSegment = 1
    XCTAssertTrue(side.sendAction(side.action, to: side.target))
    let classes = browser.model.classes
    browser.showHierarchy()
    XCTAssertEqual(asked, 6)

    XCTAssertEqual(browser.sourceText, edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    XCTAssertEqual(side.selectedSegment, 0)
    XCTAssertEqual(browser.model.classes, classes)
    XCTAssertEqual(selectedName(in: categoryTable(in: browser), values: browser.model.categories), "Kernel")
    XCTAssertEqual(selectedName(in: classTable(in: browser), values: browser.model.classes), "Object")
    XCTAssertEqual(selectedName(in: protocolTable(in: browser), values: browser.model.protocols), "user")
    XCTAssertEqual(selectedName(in: selectorTable(in: browser), values: browser.model.selectors), "b5b")
    XCTAssertEqual(printIt("Object new b5b"), "2")
  }

  func testDiscardingAnUnacceptedEditChangesTheSelectionAndClearsUndo() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    var asked = 0
    browser.confirmDiscard = { _, decide in
      asked += 1
      decide(true)
    }
    selectProtocol("user", in: browser)
    browser.replaceSource("b5a\n  ^1\n")
    browser.accept()
    selectProtocol("native", in: browser)
    XCTAssertEqual(asked, 0)
    selectProtocol("user", in: browser)
    selectSelector("b5a", in: browser)
    guard let pane = sourceView(in: browser), let undo = pane.undoManager else {
      XCTFail("missing source view or undo manager")
      return
    }
    pane.window?.makeFirstResponder(pane)
    pane.setSelectedRange(NSRange(location: (pane.string as NSString).length, length: 0))
    pane.insertText("0", replacementRange: NSRange(location: NSNotFound, length: 0))
    XCTAssertTrue(browser.hasUnacceptedChanges)
    XCTAssertTrue(undo.canUndo)

    selectClass("Array", in: browser)
    XCTAssertEqual(asked, 1)
    XCTAssertTrue(browser.sourceText.contains("subclass: #Array"))
    XCTAssertFalse(browser.hasUnacceptedChanges)
    // The old edit's undo would act on the new text, so the reload drops it.
    XCTAssertFalse(undo.canUndo)
    XCTAssertEqual(printIt("Object new b5a"), "1")
  }

  func testClassDefinitionPaneAcceptsClassSource() {
    let browser = BrowserWindow()
    selectClass("Array", in: browser)
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertNil(browser.model.selectedProtocol)
    XCTAssertTrue(browser.sourceText.contains("subclass: #Array"))
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
    selectProtocol("native", in: browser)
    XCTAssertTrue(browser.model.selectors.contains("show:"))
    selectProtocol("user", in: browser)
    browser.replaceSource("extra\n  ^7\n")
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(browser.model.selectedClass, "Transcript")
    XCTAssertEqual(side.selectedSegment, 1)
    XCTAssertEqual(browser.model.selectedProtocol, "user")
    XCTAssertEqual(browser.model.selectedSelector, "extra")

    let workspace = WorkspaceWindow()
    let expr = "Transcript extra"
    workspace.replaceText(expr)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(String(workspace.text.dropFirst(expr.count)), "7")
  }

  func testAcceptRunsOnlyWhenBrowserIsKey() {
    let browser = BrowserWindow()
    selectProtocol("user", in: browser)
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

  // `expecting` is the selection after the click when it differs from the clicked row
  // (a cancelled change keeps the old one).
  // 00 High: a failed Save or Open Image shows an alert; a successful one does not.
  func testSaveAndOpenImageFailuresShowAnAlert() {
    let app = AoApp()
    var alerts: [NSAlert] = []
    app.presentAlert = { alerts.append($0) }
    let folder = FileManager.default.temporaryDirectory
    let good = folder.appendingPathComponent("ao-alert-\(UUID().uuidString).aoimage")
    let garbage = folder.appendingPathComponent("ao-alert-\(UUID().uuidString).aoimage")
    defer {
      try? FileManager.default.removeItem(at: good)
      try? FileManager.default.removeItem(at: garbage)
    }
    XCTAssertTrue(app.saveImage(to: good))
    XCTAssertTrue(app.openImage(from: good))
    XCTAssertEqual(alerts.count, 0)

    let missing = folder
      .appendingPathComponent("ao-no-such-folder-\(UUID().uuidString)")
      .appendingPathComponent("x.aoimage")
    XCTAssertFalse(app.saveImage(to: missing))
    XCTAssertEqual(alerts.count, 1)
    XCTAssertEqual(alerts.last?.messageText, "Could not save the image")
    XCTAssertTrue(alerts.last?.informativeText.contains(missing.path) ?? false)

    XCTAssertNoThrow(try Data("not an image".utf8).write(to: garbage))
    XCTAssertFalse(app.openImage(from: garbage))
    XCTAssertEqual(alerts.count, 2)
    XCTAssertEqual(alerts.last?.messageText, "Could not open the image")
    XCTAssertTrue(alerts.last?.informativeText.contains(garbage.path) ?? false)
  }

  private func selectClass(_ name: String, in browser: BrowserWindow, expecting: String? = nil) {
    guard let table = classTable(in: browser),
          let row = browser.model.classes.firstIndex(of: name) else {
      XCTFail("missing class \(name)")
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    XCTAssertEqual(browser.model.selectedClass, expecting ?? name)
  }

  private func selectCategory(_ name: String, in browser: BrowserWindow) {
    guard let table = categoryTable(in: browser),
          let row = browser.model.categories.firstIndex(of: name) else {
      XCTFail("missing category \(name)")
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
  }

  // nil deselects the protocol row.
  private func selectProtocol(_ name: String?, in browser: BrowserWindow, expecting: String? = nil) {
    guard let table = protocolTable(in: browser) else {
      XCTFail("missing protocol table")
      return
    }
    guard let name else {
      table.deselectAll(nil)
      XCTAssertEqual(browser.model.selectedProtocol, expecting)
      return
    }
    guard let row = browser.model.protocols.firstIndex(of: name) else {
      XCTFail("missing protocol \(name)")
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    XCTAssertEqual(browser.model.selectedProtocol, expecting ?? name)
  }

  private func selectSelector(_ name: String, in browser: BrowserWindow, expecting: String? = nil) {
    guard let table = selectorTable(in: browser),
          let row = browser.model.selectors.firstIndex(of: name) else {
      XCTFail("missing selector \(name)")
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    XCTAssertEqual(browser.model.selectedSelector, expecting ?? name)
  }

  private func sourceView(in browser: BrowserWindow) -> NSTextView? {
    views(in: browser.window.contentView, of: NSTextView.self).first
  }

  private func selectedName(in table: NSTableView?, values: [String]) -> String? {
    guard let row = table?.selectedRow, row >= 0, row < values.count else {
      return nil
    }
    return values[row]
  }

  // Print it through the ABI; nil when it fails.
  private func printIt(_ source: String) -> String? {
    var out = [CChar](repeating: 0, count: 256)
    var err = AoSpan()
    let status: Int32 = source.withCString { src in
      out.withUnsafeMutableBufferPointer { buffer in
        guard let base = buffer.baseAddress else {
          return Int32(AO_ERR)
        }
        return withUnsafeMutablePointer(to: &err) { errPtr in
          ao_eval(src, Int32(source.utf8.count), Int32(AO_EVAL_PRINTIT), base, Int32(buffer.count), errPtr)
        }
      }
    }
    guard status == Int32(AO_OK) else {
      return nil
    }
    return out.withUnsafeBufferPointer { buffer in
      buffer.baseAddress.map { String(cString: $0) }
    }
  }

  private func categoryTable(in browser: BrowserWindow) -> NSTableView? {
    guard let outer = browser.window.contentView as? NSSplitView,
          let top = outer.arrangedSubviews.first as? NSSplitView else {
      return nil
    }
    return tableViews(in: top).first
  }

  private func protocolTable(in browser: BrowserWindow) -> NSTableView? {
    guard let outer = browser.window.contentView as? NSSplitView,
          outer.arrangedSubviews.count >= 2,
          let middle = outer.arrangedSubviews[1] as? NSSplitView else {
      return nil
    }
    return tableViews(in: middle).first
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

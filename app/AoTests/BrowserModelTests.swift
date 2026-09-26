import AppKit
import XCTest
@testable import Ao

@MainActor
final class BrowserModelTests: XCTestCase {
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

  func testObjectListsPrintStringAsNative() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    model.refresh()
    let cats = model.categories
    XCTAssertTrue(cats.contains("Kernel"))
    model.select(category: "Kernel", className: "Object", meta: false, protocol: nil)
    XCTAssertEqual(model.selectors, [])
    XCTAssertTrue(model.source.contains("subclass: #Object"))
    // A protocol without a selector is a new method: the pane starts empty.
    model.select(category: "Kernel", className: "Object", meta: false, protocol: "native")
    XCTAssertTrue(model.selectors.contains("printString"))
    XCTAssertEqual(model.source, "")
    model.select(
      category: "Kernel",
      className: "Object",
      meta: false,
      protocol: "native",
      selector: "printString"
    )
    XCTAssertTrue(model.source.contains("ao_Object_printString"))
    ao_runtime_shutdown()
  }

  func testProtocolListAlwaysOffersUserForNewMethods() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    model.select(category: "Kernel", className: "Object", meta: false, protocol: nil)
    XCTAssertEqual(model.protocols, ["native", "user"])
    model.select(category: "Kernel", className: "Object", meta: false, protocol: "user")
    XCTAssertEqual(model.selectedProtocol, "user")
    XCTAssertEqual(model.selectors, [])
    XCTAssertEqual(model.source, "")
  }

  func testEmptyCategoryClearsClassProtocolsAndSource() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    model.select(
      category: "Kernel",
      className: "Object",
      meta: false,
      protocol: "native",
      selector: "printString"
    )
    XCTAssertFalse(model.source.isEmpty)
    model.select(
      category: "No-Such-Category",
      className: "Object",
      meta: false,
      protocol: "native",
      selector: "printString"
    )
    XCTAssertEqual(model.classes, [])
    XCTAssertNil(model.selectedClass)
    XCTAssertNil(model.selectedProtocol)
    XCTAssertNil(model.selectedSelector)
    XCTAssertEqual(model.protocols, [])
    XCTAssertEqual(model.selectors, [])
    XCTAssertEqual(model.source, "")
  }

  func testCategoryChangeSelectsClassInThatCategory() {
    XCTAssertEqual(ao_runtime_boot(), 0)
    let order = vendorLoadOrderPath()
    XCTAssertEqual(order.withCString { ao_filein_load_order($0) }, 0)
    let browser = BrowserWindow()
    guard let outer = browser.window.contentView as? NSSplitView,
          let top = outer.arrangedSubviews.first as? NSSplitView else {
      XCTFail("expected a category/class split")
      return
    }
    let categoryTables = tableViews(in: top)
    guard categoryTables.count == 2 else {
      XCTFail("expected category and class tables")
      return
    }
    let categoryTable = categoryTables[0]
    let classTable = categoryTables[1]
    guard let otherRow = browser.model.categories.firstIndex(where: { $0 != "Kernel" }) else {
      XCTFail("expected a category other than Kernel after file-in")
      return
    }
    let other = browser.model.categories[otherRow]
    XCTAssertTrue(browser.model.source.contains("ao_Object_printString"))
    categoryTable.selectRowIndexes(IndexSet(integer: otherRow), byExtendingSelection: false)
    XCTAssertFalse(browser.model.classes.isEmpty)
    XCTAssertFalse(browser.model.classes.contains("Object"))
    let classRow = classTable.selectedRow
    XCTAssertGreaterThanOrEqual(classRow, 0)
    guard classRow >= 0, classRow < browser.model.classes.count else {
      return
    }
    let name = browser.model.classes[classRow]
    XCTAssertEqual(name, browser.model.classes.first)
    XCTAssertEqual(browser.model.selectedClass, name)
    XCTAssertTrue(browser.model.source.contains("subclass: #\(name)"))
    XCTAssertTrue(browser.model.source.contains("category: '\(other)'"))
    XCTAssertFalse(browser.model.source.contains("subclass: #Object"))
    XCTAssertFalse(browser.model.selectors.contains("printString"))
    if let protocolName = browser.model.selectedProtocol {
      XCTAssertTrue(browser.model.protocols.contains(protocolName))
    }
    if let selector = browser.model.selectedSelector {
      XCTAssertTrue(browser.model.selectors.contains(selector))
    }
  }

  private func vendorLoadOrderPath() -> String {
    URL(fileURLWithPath: #filePath)
      .deletingLastPathComponent()
      .deletingLastPathComponent()
      .deletingLastPathComponent()
      .appendingPathComponent("image/vendor/LOAD_ORDER")
      .path
  }

  func testSystemBrowserPanesListOwnMethods() {
    let browser = BrowserWindow()
    XCTAssertEqual(browser.title, "System Browser")
    XCTAssertEqual(browser.model.boot(), 0)

    guard let outer = browser.window.contentView as? NSSplitView,
          outer.arrangedSubviews.count == 3,
          let top = outer.arrangedSubviews[0] as? NSSplitView,
          let middle = outer.arrangedSubviews[1] as? NSSplitView else {
      XCTFail("expected a vertical split with category/class and protocol/selector splits")
      return
    }
    XCTAssertFalse(outer.isVertical)
    XCTAssertTrue(top.isVertical)
    XCTAssertTrue(middle.isVertical)

    let categoryTables = tableViews(in: top)
    let messageTables = tableViews(in: middle)
    guard categoryTables.count == 2, messageTables.count == 2 else {
      XCTFail("expected category/class and protocol/selector tables")
      return
    }
    let categoryTable = categoryTables[0]
    let classTable = categoryTables[1]
    let protocolTable = messageTables[0]
    let selectorTable = messageTables[1]
    XCTAssertEqual(categoryTable.numberOfRows, browser.model.categories.count)
    XCTAssertTrue(browser.model.categories.contains("Kernel"))
    XCTAssertEqual(classTable.numberOfRows, browser.model.classes.count)
    XCTAssertTrue(browser.model.classes.contains("Object"))
    XCTAssertEqual(protocolTable.numberOfRows, browser.model.protocols.count)
    XCTAssertTrue(browser.model.protocols.contains("native"))
    XCTAssertEqual(selectorTable.numberOfRows, browser.model.selectors.count)
    XCTAssertTrue(browser.model.selectors.contains("printString"))
    XCTAssertTrue(browser.model.source.contains("ao_Object_printString"))
    let sourcePane = outer.arrangedSubviews[2]
    XCTAssertTrue(tableViews(in: sourcePane).isEmpty)
    guard let source = textViews(in: sourcePane).first else {
      XCTFail("missing source text")
      return
    }
    XCTAssertTrue(source.string.contains("ao_Object_printString"))
    browser.window.layoutIfNeeded()
    assertPositiveFrame(categoryTable, "category list")
    assertPositiveFrame(classTable, "class list")
    assertPositiveFrame(protocolTable, "protocol list")
    assertPositiveFrame(selectorTable, "selector list")
    assertPositiveFrame(source, "source text")

    guard let classRow = browser.model.classes.firstIndex(of: "Class") else {
      XCTFail("Class missing from the class list")
      return
    }
    classTable.selectRowIndexes(IndexSet(integer: classRow), byExtendingSelection: false)
    // A class row shows its definition with no protocol; the protocol row lists its selectors.
    XCTAssertNil(browser.model.selectedProtocol)
    XCTAssertTrue(browser.model.source.contains("subclass: #Class"))
    selectRow("native", in: protocolTable, values: browser.model.protocols)
    let subclassSelector =
      "subclass:instanceVariableNames:classVariableNames:poolDictionaries:category:"
    XCTAssertEqual(selectorTable.numberOfRows, browser.model.selectors.count)
    XCTAssertTrue(browser.model.selectors.contains(subclassSelector))
    XCTAssertFalse(browser.model.selectors.contains("printString"))

    let sides = segmentedControls(in: top)
    XCTAssertEqual(sides.count, 1)
    guard let side = sides.first else {
      return
    }
    XCTAssertEqual(side.segmentCount, 2)
    side.selectedSegment = 1
    _ = side.sendAction(side.action, to: side.target)
    XCTAssertFalse(browser.model.selectors.contains(subclassSelector))
    XCTAssertEqual(selectorTable.numberOfRows, browser.model.selectors.count)

    guard let transcriptRow = browser.model.classes.firstIndex(of: "Transcript") else {
      XCTFail("Transcript missing from the class list")
      return
    }
    classTable.selectRowIndexes(IndexSet(integer: transcriptRow), byExtendingSelection: false)
    selectRow("native", in: protocolTable, values: browser.model.protocols)
    XCTAssertEqual(selectorTable.numberOfRows, browser.model.selectors.count)
    XCTAssertTrue(browser.model.selectors.contains("show:"))
    XCTAssertFalse(browser.model.selectors.contains("printString"))
  }

  private func selectRow(
    _ name: String,
    in table: NSTableView,
    values: [String],
    file: StaticString = #filePath,
    line: UInt = #line
  ) {
    guard let row = values.firstIndex(of: name) else {
      XCTFail("missing row \(name)", file: file, line: line)
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
  }

  private func assertPositiveFrame(
    _ view: NSView,
    _ label: String,
    file: StaticString = #filePath,
    line: UInt = #line
  ) {
    let visible = view.visibleRect
    XCTAssertGreaterThan(
      visible.width,
      0,
      "\(label) width frame \(view.frame) visible \(visible)",
      file: file,
      line: line
    )
    XCTAssertGreaterThan(
      visible.height,
      0,
      "\(label) height frame \(view.frame) visible \(visible)",
      file: file,
      line: line
    )
  }

  private func tableViews(in root: NSView?) -> [NSTableView] {
    views(in: root, of: NSTableView.self)
  }

  private func textViews(in root: NSView?) -> [NSTextView] {
    views(in: root, of: NSTextView.self)
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

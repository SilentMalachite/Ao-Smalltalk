import AppKit
import XCTest
@testable import Ao

@MainActor
final class BrowserModelTests: XCTestCase {
  override func tearDown() {
    ao_runtime_shutdown()
    super.tearDown()
  }

  func testObjectListsPrintStringAsNative() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    model.refresh()
    let cats = model.categories
    XCTAssertTrue(cats.contains("Kernel"))
    model.select(category: "Kernel", className: "Object", meta: false, protocol: "native")
    XCTAssertTrue(model.selectors.contains("printString"))
    XCTAssertTrue(model.source.contains("subclass: #Object"))
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
    XCTAssertTrue(textViews(in: sourcePane).first?.string.contains("ao_Object_printString") == true)

    guard let classRow = browser.model.classes.firstIndex(of: "Class") else {
      XCTFail("Class missing from the class list")
      return
    }
    classTable.selectRowIndexes(IndexSet(integer: classRow), byExtendingSelection: false)
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
    XCTAssertEqual(selectorTable.numberOfRows, browser.model.selectors.count)
    XCTAssertTrue(browser.model.selectors.contains("show:"))
    XCTAssertFalse(browser.model.selectors.contains("printString"))
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

import AppKit
import XCTest
@testable import Ao

@MainActor
final class ToolWindowTests: XCTestCase {
  override func tearDown() {
    UserDefaults.standard.removeObject(forKey: "AoTranscriptFixedPitch")
    ao_runtime_shutdown()
    super.tearDown()
  }

  func testTranscriptAppendsAndSurvivesClose() {
    let launch = LaunchSet.make()
    XCTAssertEqual(launch.transcript.title, "Transcript")
    XCTAssertEqual(launch.workspace.title, "Workspace")
    launch.transcript.append("hello")
    launch.transcript.append("\n")
    launch.transcript.closeWindow()
    XCTAssertEqual(launch.transcript.text, "hello\n")
    launch.transcript.useFixedPitch = true
    XCTAssertEqual(launch.transcript.useFixedPitch, true)
  }

  func testMainMenuListsToolsAndSmalltalkKeys() {
    let menu = MainMenu.build(actions: MainMenu.Actions())
    let titles = menuTitles(in: menu)
    for title in ["Transcript", "Workspace", "Browser", "Do it", "Print it", "Inspect it", "Accept"] {
      XCTAssertTrue(titles.contains(title), title)
    }
    let printIt = menuItem(titled: "Print it", in: menu)
    XCTAssertEqual(printIt?.keyEquivalent, "i")
    XCTAssertTrue(printIt?.keyEquivalentModifierMask.contains(.command) ?? false)
    let doIt = menuItem(titled: "Do it", in: menu)
    XCTAssertEqual(doIt?.keyEquivalent, "d")
    XCTAssertTrue(doIt?.keyEquivalentModifierMask.contains(.command) ?? false)
    for item in menuItems(in: menu) where item.keyEquivalent == "p" {
      XCTAssertNotEqual(item.title, "Print it")
    }
    XCTAssertTrue(titles.contains("Use Fixed Pitch"))
    XCTAssertTrue(titles.contains("Close"))
    XCTAssertEqual(
      menu.items.map(\.title),
      ["Ao", "File", "Edit", "Smalltalk", "Tools", "Window", "Help"]
    )
  }

  func testTypedQuotesAndDashesStayPlainInWorkspaceAndBrowser() {
    let workspace = WorkspaceWindow()
    let browser = BrowserWindow()
    let inspector = InspectorWindow(className: "SmallInteger", printString: "3")
    defer {
      workspace.window.close()
      browser.window.close()
      inspector.window.close()
    }
    selectFirstClassDefinition(in: browser)
    let editors = [
      ("Workspace", textViews(in: workspace.window.contentView).first),
      ("Browser", textViews(in: browser.window.contentView).first),
    ]
    let typed = "x 'a' -- y"
    for (name, found) in editors {
      guard let view = found else {
        XCTFail("missing \(name) text view")
        continue
      }
      XCTAssertTrue(view.isEditable, name)
      let before = view.string
      type(typed, into: view)
      XCTAssertEqual(view.string, before + typed, name)
    }
    let all = editors.compactMap(\.1) + textViews(in: inspector.window.contentView)
    XCTAssertEqual(all.count, 3)
    for view in all {
      XCTAssertFalse(view.isAutomaticQuoteSubstitutionEnabled)
      XCTAssertFalse(view.isAutomaticDashSubstitutionEnabled)
      XCTAssertFalse(view.isAutomaticTextReplacementEnabled)
      XCTAssertFalse(view.isAutomaticSpellingCorrectionEnabled)
      XCTAssertFalse(view.smartInsertDeleteEnabled)
    }
  }

  // Typing path: one insertText per character at the caret, as the key bindings send it. The
  // automatic substitutions run from the run loop, so it turns after every character.
  private func type(_ text: String, into view: NSTextView) {
    view.window?.makeFirstResponder(view)
    let end = (view.string as NSString).length
    view.setSelectedRange(NSRange(location: end, length: 0))
    for character in text {
      view.insertText(String(character), replacementRange: NSRange(location: NSNotFound, length: 0))
      RunLoop.current.run(until: Date(timeIntervalSinceNow: 0.01))
    }
    RunLoop.current.run(until: Date(timeIntervalSinceNow: 0.2))
  }

  // A class row with no selector shows the class definition, which stays editable.
  private func selectFirstClassDefinition(in browser: BrowserWindow) {
    guard let outer = browser.window.contentView as? NSSplitView,
          let top = outer.arrangedSubviews.first as? NSSplitView else {
      XCTFail("missing class split")
      return
    }
    let tables = views(in: top, of: NSTableView.self)
    guard tables.count == 2, let row = browser.model.classes.firstIndex(of: "Array") else {
      XCTFail("missing class table or Array")
      return
    }
    tables[1].selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    XCTAssertNil(browser.model.selectedSelector)
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

  private func menuItems(in menu: NSMenu) -> [NSMenuItem] {
    var found: [NSMenuItem] = []
    for item in menu.items {
      found.append(item)
      if let submenu = item.submenu {
        found.append(contentsOf: menuItems(in: submenu))
      }
    }
    return found
  }

  private func menuTitles(in menu: NSMenu) -> [String] {
    menuItems(in: menu).map(\.title)
  }

  private func menuItem(titled title: String, in menu: NSMenu) -> NSMenuItem? {
    menuItems(in: menu).first { $0.title == title }
  }
}

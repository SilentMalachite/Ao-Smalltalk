import AppKit
import XCTest
@testable import Ao

@MainActor
final class ToolWindowTests: XCTestCase {
  override func tearDown() {
    UserDefaults.standard.removeObject(forKey: "AoTranscriptFixedPitch")
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

  // SPEC §3.4: a Do it drains the forked processes before it returns. The fork's Transcript show:
  // calls the hook on that process's own stack (same main thread), and it reaches the window.
  func testForkedTranscriptShowReachesTranscriptWindow() {
    let launch = LaunchSet.make()
    launch.workspace.replaceText("[Transcript show: 'from a fork'] fork")
    launch.workspace.selectAll()
    launch.workspace.doIt()
    XCTAssertTrue(launch.transcript.text.contains("from a fork"))
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

  func testEditMenuUndoAndRedoGoToTheFirstResponder() {
    let menu = MainMenu.build(actions: MainMenu.Actions())
    let edit = menu.item(withTitle: "Edit")?.submenu
    XCTAssertEqual(
      edit?.items.map(\.title),
      ["Undo", "Redo", "", "Cut", "Copy", "Paste", "Select All"]
    )
    let undo = edit?.item(withTitle: "Undo")
    XCTAssertEqual(undo?.action, Selector(("undo:")))
    XCTAssertNil(undo?.target)
    XCTAssertEqual(undo?.keyEquivalent, "z")
    XCTAssertEqual(undo?.keyEquivalentModifierMask, [.command])
    let redo = edit?.item(withTitle: "Redo")
    XCTAssertEqual(redo?.action, Selector(("redo:")))
    XCTAssertNil(redo?.target)
    XCTAssertEqual(redo?.keyEquivalent, "z")
    XCTAssertEqual(redo?.keyEquivalentModifierMask, [.command, .shift])
  }

  func testTypingInWorkspaceAndBrowserIsUndoable() {
    let workspace = WorkspaceWindow()
    let browser = BrowserWindow()
    defer {
      workspace.window.close()
      browser.window.close()
    }
    selectFirstClassDefinition(in: browser)
    let editors = [
      ("Workspace", textViews(in: workspace.window.contentView).first),
      ("Browser", textViews(in: browser.window.contentView).first),
    ]
    for (name, found) in editors {
      guard let view = found, let undo = view.undoManager else {
        XCTFail("missing \(name) text view or undo manager")
        continue
      }
      XCTAssertTrue(view.allowsUndo, name)
      let before = view.string
      type("zork", into: view)
      XCTAssertEqual(view.string, before + "zork", name)
      XCTAssertTrue(undo.canUndo, name)
      undo.undo()
      XCTAssertEqual(view.string, before, name)
      undo.redo()
      XCTAssertEqual(view.string, before + "zork", name)
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

import AppKit
import XCTest
@testable import Ao

@MainActor
final class ToolWindowTests: XCTestCase {
  override func tearDown() {
    UserDefaults.standard.removeObject(forKey: "AoTranscriptFixedPitch")
    UserDefaults.standard.removeObject(forKey: "AoTextSizeOffset")
    // tearDown is nonisolated; XCTest calls it on the main thread.
    MainActor.assumeIsolated {
      Self.closeVisibleWindows()
      Self.dropMainMenu()
    }
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

  // A menu built by AoApp holds the AoApp in its closures, and the AoApp its LaunchSet, whose
  // deinit removes the transcript hook. Windows listed in the Window menu keep the menu alive
  // after mainMenu = nil, so its items go too; then the AoApp goes now, not in a later test.
  private static func dropMainMenu() {
    let menu = NSApplication.shared.mainMenu
    NSApplication.shared.mainMenu = nil
    NSApplication.shared.windowsMenu = nil
    menu?.removeAllItems()
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

  // SPEC §3.9: the Transcript appends each chunk and scrolls to the end once after the eval.
  // 20000 lines are 40000 hook calls; they finish within 10 s, under ASan too. Fixed pitch makes
  // the font of an appended chunk differ from the text system's default.
  func testTwentyThousandTranscriptLinesFinishWithinTenSecondsAndShowTheEnd() {
    let launch = LaunchSet.make()
    launch.transcript.useFixedPitch = true
    launch.workspace.replaceText("1 to: 20000 do: [:i | Transcript show: i printString; cr]")
    launch.workspace.selectAll()
    let started = Date()
    launch.workspace.doIt()
    let elapsed = Date().timeIntervalSince(started)
    XCTAssertLessThanOrEqual(elapsed, 10, "Do it took \(elapsed) s")
    let expected = (1...20000).map { "\($0)\n" }.joined()
    let text = launch.transcript.text
    XCTAssertTrue(text == expected, "transcript has \(text.count) characters, ends \(text.suffix(12).debugDescription)")
    XCTAssertTrue(text.hasSuffix("20000\n"))

    let windows = NSApplication.shared.windows.filter { $0.title == "Transcript" && $0.isVisible }
    XCTAssertEqual(windows.count, 1)
    guard let window = windows.first,
          let view = (window.contentView as? NSScrollView)?.documentView as? NSTextView,
          let storage = view.textStorage,
          storage.length == (expected as NSString).length else {
      XCTFail("missing Transcript text view or its text")
      return
    }
    RunLoop.current.run(until: Date(timeIntervalSinceNow: 0.3))
    let visible = view.visibleRect
    XCTAssertGreaterThan(visible.height, 0)
    XCTAssertLessThan(visible.height, view.bounds.height)
    // The last line, "20000", in the text view's coordinates.
    let screen = view.firstRect(forCharacterRange: NSRange(location: storage.length - 6, length: 5), actualRange: nil)
    let last = view.convert(window.convertFromScreen(screen), from: nil)
    XCTAssertTrue(visible.contains(last), "the end of the Transcript is not visible: \(last) in \(visible)")
    let font = storage.attribute(.font, at: storage.length - 1, effectiveRange: nil) as? NSFont
    XCTAssertEqual(font, NSFont.userFixedPitchFont(ofSize: 0))
  }

  // SPEC §3.9: Transcript text is drawn in the system text color, so it stays readable in Dark
  // Mode. Each appended chunk carries it with the current font, also after toggling fixed pitch.
  func testTranscriptTextUsesSystemTextColorAfterAppendAndFixedPitchToggle() {
    let launch = LaunchSet.make()
    let windows = NSApplication.shared.windows.filter { $0.title == "Transcript" && $0.isVisible }
    guard let view = (windows.first?.contentView as? NSScrollView)?.documentView as? NSTextView else {
      XCTFail("missing Transcript text view")
      return
    }
    func assertLastCharacterDrawnInTextColor(fixedPitch: Bool, line: UInt = #line) {
      guard let storage = view.textStorage, storage.length > 0 else {
        XCTFail("the Transcript is empty", line: line)
        return
      }
      let attributes = storage.attributes(at: storage.length - 1, effectiveRange: nil)
      XCTAssertEqual(attributes[.foregroundColor] as? NSColor, NSColor.textColor, line: line)
      let font = fixedPitch ? NSFont.userFixedPitchFont(ofSize: 0) : NSFont.userFont(ofSize: 0)
      XCTAssertEqual(attributes[.font] as? NSFont, font, line: line)
      XCTAssertEqual(view.typingAttributes[.foregroundColor] as? NSColor, NSColor.textColor, line: line)
    }
    launch.transcript.append("x")
    assertLastCharacterDrawnInTextColor(fixedPitch: launch.transcript.useFixedPitch)
    launch.transcript.useFixedPitch.toggle()
    launch.transcript.append("y")
    assertLastCharacterDrawnInTextColor(fixedPitch: launch.transcript.useFixedPitch)
  }

  // SPEC §3.9 起動と同梱: AO_VENDOR_DIR when it is not empty, else the bundle's
  // Contents/Resources/vendor. The current directory plays no part.
  func testVendorDirectoryPrefersEnvironmentOverBundleResources() {
    let resources = URL(fileURLWithPath: "/Applications/Ao.app/Contents/Resources", isDirectory: true)
    let bundled = "/Applications/Ao.app/Contents/Resources/vendor"
    XCTAssertEqual(
      vendorDirectory(environment: ["AO_VENDOR_DIR": "/tmp/ao-vendor"], resources: resources).path,
      "/tmp/ao-vendor"
    )
    XCTAssertEqual(vendorDirectory(environment: [:], resources: resources).path, bundled)
    XCTAssertEqual(vendorDirectory(environment: ["AO_VENDOR_DIR": ""], resources: resources).path, bundled)
  }

  func testVendorFileInWritesLoadedLineAndDefinesTimespan() {
    let launch = LaunchSet.make()
    let vendor = URL(fileURLWithPath: #filePath)
      .deletingLastPathComponent()
      .deletingLastPathComponent()
      .deletingLastPathComponent()
      .appendingPathComponent("image/vendor", isDirectory: true)
    fileInVendor(at: vendor, transcript: launch.transcript)
    XCTAssertEqual(launch.transcript.text, "vendor loaded: \(vendor.path)\n")
    let source = "Smalltalk includesKey: #Timespan"
    launch.workspace.replaceText(source)
    launch.workspace.selectAll()
    launch.workspace.printIt()
    XCTAssertEqual(launch.workspace.text, source + "true")
  }

  func testMissingVendorWritesNotFoundLine() {
    let launch = LaunchSet.make()
    let empty = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-vendor-\(UUID().uuidString)", isDirectory: true)
    XCTAssertNoThrow(try FileManager.default.createDirectory(at: empty, withIntermediateDirectories: true))
    defer { try? FileManager.default.removeItem(at: empty) }
    fileInVendor(at: empty, transcript: launch.transcript)
    XCTAssertEqual(launch.transcript.text, "vendor not found: \(empty.path)\n")
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
    // SPEC §3.9: Debug it is ⌘⇧D.
    let debugIt = menuItem(titled: "Debug it", in: menu)
    XCTAssertEqual(debugIt?.keyEquivalent, "d")
    XCTAssertEqual(debugIt?.keyEquivalentModifierMask, [.command, .shift])
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

  // The launch path's own menu: Tools → Browser opens the System Browser. tearDown drops the
  // main menu, whose closures hold the AoApp and so its LaunchSet.
  func testToolsBrowserMenuItemOpensSystemBrowser() {
    let app = AoApp()
    app.applicationWillFinishLaunching(
      Notification(name: NSApplication.willFinishLaunchingNotification, object: NSApplication.shared)
    )
    guard let item = NSApplication.shared.mainMenu?.item(withTitle: "Tools")?.submenu?.item(withTitle: "Browser"),
          let action = item.action else {
      XCTFail("missing Tools → Browser")
      return
    }
    let browsers = { NSApplication.shared.windows.filter { $0.title == "System Browser" && $0.isVisible } }
    XCTAssertEqual(browsers().count, 0)
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: item.target, from: item))
    XCTAssertEqual(browsers().count, 1)
  }

  // SPEC §3.9 文字の大きさ: the Tools menu ends with the three text size items. ⌘+ is the typed
  // "+", which takes Shift on both the US (⇧=) and the JIS (⇧;) layouts.
  func testToolsMenuHasTextSizeItemsWithCommandKeys() {
    var chosen: [String] = []
    let menu = MainMenu.build(actions: MainMenu.Actions(
      makeTextBigger: { chosen.append("bigger") },
      makeTextSmaller: { chosen.append("smaller") },
      showActualSize: { chosen.append("actual") }
    ))
    let tools = menu.item(withTitle: "Tools")?.submenu
    XCTAssertEqual(
      tools?.items.map(\.title),
      ["Browser", "Transcript", "Workspace", "", "Use Fixed Pitch", "", "Make Text Bigger", "Make Text Smaller", "Actual Size"]
    )
    for (title, key) in [("Make Text Bigger", "+"), ("Make Text Smaller", "-"), ("Actual Size", "0")] {
      let item = tools?.item(withTitle: title)
      XCTAssertEqual(item?.keyEquivalent, key, title)
      XCTAssertEqual(item?.keyEquivalentModifierMask, [.command], title)
    }
    let presses: [(NSEvent.ModifierFlags, String, UInt16)] = [
      ([.command, .shift], "+", 24),
      ([.command, .shift], "+", 41),
      ([.command], "-", 27),
      ([.command], "0", 29),
    ]
    for (flags, characters, keyCode) in presses {
      guard let event = NSEvent.keyEvent(
        with: .keyDown,
        location: .zero,
        modifierFlags: flags,
        timestamp: 0,
        windowNumber: 0,
        context: nil,
        characters: characters,
        charactersIgnoringModifiers: characters,
        isARepeat: false,
        keyCode: keyCode
      ) else {
        XCTFail("could not make the key event for \(characters)")
        continue
      }
      XCTAssertTrue(menu.performKeyEquivalent(with: event), characters)
    }
    XCTAssertEqual(chosen, ["bigger", "bigger", "smaller", "actual"])
  }

  // SPEC §3.9 文字の大きさ: through the launch path's own menu, the text size reaches the
  // Transcript, the Workspace and the Browser's source pane at once. Text added later, windows
  // opened later and the fixed pitch toggle use the same offset; Actual Size removes it.
  func testTextSizeMenuItemsResizeTranscriptWorkspaceAndBrowserSource() {
    let app = AoApp()
    app.applicationWillFinishLaunching(
      Notification(name: NSApplication.willFinishLaunchingNotification, object: NSApplication.shared)
    )
    guard let tools = NSApplication.shared.mainMenu?.item(withTitle: "Tools")?.submenu else {
      XCTFail("missing Tools menu")
      return
    }
    func choose(_ title: String, line: UInt = #line) {
      guard let item = tools.item(withTitle: title), let action = item.action else {
        XCTFail("missing Tools → \(title)", line: line)
        return
      }
      XCTAssertTrue(NSApplication.shared.sendAction(action, to: item.target, from: item), title, line: line)
    }
    func textView(titled title: String) -> NSTextView? {
      let window = NSApplication.shared.windows.first { $0.title == title && $0.isVisible }
      return textViews(in: window?.contentView).first
    }
    choose("Browser")
    guard let transcript = textView(titled: "Transcript"),
          let workspace = textView(titled: "Workspace"),
          let source = textView(titled: "System Browser"),
          let variable = NSFont.userFont(ofSize: 0)?.pointSize,
          let fixed = NSFont.userFixedPitchFont(ofSize: 0)?.pointSize else {
      XCTFail("missing a text view or a default font")
      return
    }
    workspace.string = "3 + 4"
    source.string = "foo"

    choose("Make Text Bigger")
    choose("Make Text Bigger")
    XCTAssertEqual(UserDefaults.standard.integer(forKey: "AoTextSizeOffset"), 2)
    assertWholeText(of: transcript, uses: NSFont.userFont(ofSize: variable + 2), "Transcript")
    assertWholeText(of: workspace, uses: NSFont.userFont(ofSize: variable + 2), "Workspace")
    assertWholeText(of: source, uses: NSFont.userFont(ofSize: variable + 2), "Browser")

    choose("Use Fixed Pitch")
    assertWholeText(of: transcript, uses: NSFont.userFixedPitchFont(ofSize: fixed + 2), "fixed pitch Transcript")

    // Opened after the change: a Workspace whose Print it also writes to the Transcript, and a Browser.
    let laterWorkspace = WorkspaceWindow()
    laterWorkspace.replaceText("Transcript show: 'later'. 3 + 4")
    laterWorkspace.selectAll()
    laterWorkspace.printIt()
    XCTAssertTrue(laterWorkspace.text.hasSuffix("7"))
    XCTAssertTrue(transcript.string.hasSuffix("later"))
    assertWholeText(of: transcript, uses: NSFont.userFixedPitchFont(ofSize: fixed + 2), "Transcript after show:")
    assertWholeText(
      of: textViews(in: laterWorkspace.window.contentView).first,
      uses: NSFont.userFont(ofSize: variable + 2),
      "later Workspace"
    )
    let laterBrowser = BrowserWindow()
    selectFirstClassDefinition(in: laterBrowser)
    assertWholeText(
      of: textViews(in: laterBrowser.window.contentView).first,
      uses: NSFont.userFont(ofSize: variable + 2),
      "later Browser"
    )

    choose("Make Text Smaller")
    XCTAssertEqual(UserDefaults.standard.integer(forKey: "AoTextSizeOffset"), 1)
    assertWholeText(of: transcript, uses: NSFont.userFixedPitchFont(ofSize: fixed + 1), "smaller Transcript")
    assertWholeText(of: workspace, uses: NSFont.userFont(ofSize: variable + 1), "smaller Workspace")
    assertWholeText(of: source, uses: NSFont.userFont(ofSize: variable + 1), "smaller Browser")

    choose("Actual Size")
    XCTAssertNil(UserDefaults.standard.object(forKey: "AoTextSizeOffset"))
    assertWholeText(of: transcript, uses: NSFont.userFixedPitchFont(ofSize: 0), "actual Transcript")
    assertWholeText(of: workspace, uses: NSFont.userFont(ofSize: 0), "actual Workspace")
    assertWholeText(of: source, uses: NSFont.userFont(ofSize: 0), "actual Browser")
  }

  // SPEC §3.9 文字の大きさ: the offset stays within −4 ... +24, also when the stored value was
  // written outside the app.
  func testTextSizeOffsetStopsAtRangeEnds() {
    guard let variable = NSFont.userFont(ofSize: 0)?.pointSize,
          let fixed = NSFont.userFixedPitchFont(ofSize: 0)?.pointSize else {
      XCTFail("missing a default font")
      return
    }
    for _ in 0..<40 {
      ToolTextSize.step(by: 1)
    }
    XCTAssertEqual(ToolTextSize.offset, 24)
    XCTAssertEqual(ToolTextSize.font(fixedPitch: false), NSFont.userFont(ofSize: variable + 24))
    for _ in 0..<40 {
      ToolTextSize.step(by: -1)
    }
    XCTAssertEqual(ToolTextSize.offset, -4)
    XCTAssertEqual(ToolTextSize.font(fixedPitch: true), NSFont.userFixedPitchFont(ofSize: fixed - 4))
    UserDefaults.standard.set(99, forKey: "AoTextSizeOffset")
    XCTAssertEqual(ToolTextSize.offset, 24)
  }

  // SPEC §3.9 文字の大きさ: text that comes in after a resize takes the current size, also where
  // nothing is next to it to copy the font from (Print it into an empty Workspace) and where the
  // undo stack keeps it at the old size (undo of a deletion done before the resize).
  func testPrintItIntoEmptyWorkspaceAndUndoAfterResizeUseCurrentSize() {
    let launch = LaunchSet.make()
    let browser = BrowserWindow()
    selectFirstClassDefinition(in: browser)
    guard let variable = NSFont.userFont(ofSize: 0)?.pointSize,
          let workspaceView = textViews(in: launch.workspace.window.contentView).first,
          let sourceView = textViews(in: browser.window.contentView).first else {
      XCTFail("missing a default font or a text view")
      return
    }
    ToolTextSize.step(by: 2)
    launch.workspace.applyFont()
    browser.applyFont()
    launch.workspace.printIt()
    XCTAssertEqual(launch.workspace.text, "nil")
    assertWholeText(of: workspaceView, uses: NSFont.userFont(ofSize: variable + 2), "Print it into empty Workspace")

    for (name, view) in [("Workspace", workspaceView), ("Browser", sourceView)] {
      guard let undo = view.undoManager else {
        XCTFail("missing \(name) undo manager")
        continue
      }
      type("zork", into: view)
      let end = (view.string as NSString).length
      view.setSelectedRange(NSRange(location: end - 2, length: 1))
      view.deleteBackward(nil)
      XCTAssertTrue(view.string.hasSuffix("zok"), name)
      ToolTextSize.step(by: 2)
      launch.workspace.applyFont()
      browser.applyFont()
      undo.undo()
      XCTAssertTrue(view.string.hasSuffix("zork"), name)
      assertWholeText(
        of: view,
        uses: NSFont.userFont(ofSize: variable + CGFloat(ToolTextSize.offset)),
        "\(name) after undo"
      )
    }
  }

  // SPEC §3.9 文字の大きさ: the panes share the size, not the face. Japanese typed at a larger size
  // keeps a face that has its glyphs (Hiragino for Helvetica). Reading layoutManager moves the view
  // to TextKit 1, which writes that face into the storage.
  func testJapaneseTypedAfterResizeKeepsCoveringFontAtCurrentSize() {
    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
    ToolTextSize.step(by: 2)
    workspace.applyFont()
    guard let view = textViews(in: workspace.window.contentView).first,
          let size = ToolTextSize.font(fixedPitch: false)?.pointSize else {
      XCTFail("missing Workspace text view or font")
      return
    }
    XCTAssertNotNil(view.layoutManager)
    type("ab日本", into: view)
    guard let storage = view.textStorage, storage.string == "ab日本" else {
      XCTFail("Workspace text is \(view.string.debugDescription)")
      return
    }
    let whole = NSRange(location: 0, length: storage.length)
    storage.ensureAttributesAreFixed(in: whole)
    storage.enumerateAttribute(.font, in: whole) { value, range, _ in
      let font = value as? NSFont
      XCTAssertEqual(font?.pointSize, size, "at \(range)")
      for scalar in storage.attributedSubstring(from: range).string.unicodeScalars {
        XCTAssertTrue(font?.coveredCharacterSet.contains(scalar) ?? false, "\(scalar) in \(String(describing: font))")
      }
    }
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

  // Every run of the text, and the typing attributes, carry `font`.
  private func assertWholeText(of view: NSTextView?, uses font: NSFont?, _ name: String, line: UInt = #line) {
    guard let view, let storage = view.textStorage, storage.length > 0, let font else {
      XCTFail("missing \(name) text or font", line: line)
      return
    }
    storage.enumerateAttribute(.font, in: NSRange(location: 0, length: storage.length)) { value, range, _ in
      XCTAssertEqual(value as? NSFont, font, "\(name) at \(range)", line: line)
    }
    XCTAssertEqual(view.typingAttributes[.font] as? NSFont, font, name, line: line)
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

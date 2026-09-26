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

  func testAcceptFooThenPrintItInsertsOneAndFailedAcceptKeepsText() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    let foo = "foo\n  ^1\n"
    browser.replaceSource(foo)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")

    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
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

  // SPEC §3.8, §3.9: a failed Accept keeps the text and selects the error span, converted from
  // UTF-8 past Japanese and an emoji. A method's span counts from its source; a class
  // definition's counts from the whole source (the method sits after the definition chunk).
  func testFailedAcceptSelectsErrorSpanAfterJapaneseComment() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    guard let view = sourceView(in: browser) else {
      XCTFail("missing source view")
      return
    }
    selectProtocol("user", in: browser)
    let method = "b11bad\n  \"日本語のコメント 😀\"\n  ^ + 1\n"
    browser.replaceSource(method)
    browser.accept()
    XCTAssertEqual(browser.sourceText, method)
    XCTAssertFalse(browser.errorText.isEmpty)
    XCTAssertEqual(view.selectedRange(), (method as NSString).range(of: "+"))

    // A second Browser: the first holds an unaccepted edit, so changing its selection would ask.
    let classBrowser = BrowserWindow()
    defer { classBrowser.window.close() }
    guard let classView = sourceView(in: classBrowser) else {
      XCTFail("missing class source view")
      return
    }
    selectClass("Array", in: classBrowser)
    let definition =
      "\"日本\" Object subclass: #B11Span\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B11-日本'!\n\n!B11Span methodsFor: 'b11'!\n"
      + "bad\n  \"あ😀\"\n  ^ + 1! !\n"
    classBrowser.replaceSource(definition)
    classBrowser.accept()
    XCTAssertEqual(classBrowser.sourceText, definition)
    XCTAssertFalse(classBrowser.errorText.isEmpty)
    XCTAssertEqual(classView.selectedRange(), (definition as NSString).range(of: "+"))
  }

  func testPrintStringOverwriteKeepsTextAndNativeResult() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    // A new method named printString: the runtime refuses to replace the native.
    selectProtocol("user", in: browser)
    let source = "printString\n  ^1\n"
    browser.replaceSource(source)
    browser.accept()
    XCTAssertEqual(browser.sourceText, source)
    XCTAssertFalse(browser.errorText.isEmpty)

    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
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

  // P10 受け入れ: a failure inside a method accepted in the Browser; the Debugger shows that
  // method's source with the failing send selected.
  func testDebuggerSelectsFailingSendInBrowserAcceptedMethod() {
    ao_set_debug_capture(1)
    defer { ao_set_debug_capture(0) }
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    let source = "p10Boom\n  | t |\n  t := 3.\n  ^nil zork: t\n"
    browser.replaceSource(source)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")

    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
    workspace.replaceText("Object new p10Boom")
    workspace.selectAll()
    workspace.doIt()
    XCTAssertFalse(workspace.debugButton.isHidden)
    workspace.debugButton.performClick(nil)
    guard let debugger = workspace.debuggers.last else {
      XCTFail("Debug opened no Debugger")
      return
    }
    XCTAssertEqual(debugger.frameLabels, ["#zork: (doesNotUnderstand:)", "Object>>p10Boom", "doIt"])
    debugger.selectFrame(1)
    XCTAssertEqual(debugger.sourceText, source)
    XCTAssertEqual(debugger.sourceSelection, (source as NSString).range(of: "nil zork: t"))
    XCTAssertEqual(debugger.variable(at: 1).name, "t")
    XCTAssertEqual(debugger.variable(at: 1).value, "3")
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

  // While the question waits for an answer, another row, the side switch and Show Hierarchy do
  // not ask again: the rows and the switch go back, and the first answer decides alone.
  func testChangesWhileTheDiscardQuestionWaitsAreNotAskedAgain() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    var pending: [@MainActor (Bool) -> Void] = []
    browser.confirmDiscard = { _, decide in pending.append(decide) }
    selectProtocol("user", in: browser)
    browser.replaceSource("b5w\n  ^1\n")
    browser.accept()
    browser.replaceSource("b5w\n  ^2\n")

    selectClass("Array", in: browser, expecting: "Object")
    XCTAssertEqual(pending.count, 1)
    selectClass("SmallInteger", in: browser, expecting: "Object")
    guard let side = segmentedControls(in: browser.window.contentView).first else {
      XCTFail("missing instance/class switch")
      return
    }
    side.selectedSegment = 1
    XCTAssertTrue(side.sendAction(side.action, to: side.target))
    let classes = browser.model.classes
    browser.showHierarchy()
    XCTAssertEqual(pending.count, 1)
    XCTAssertEqual(side.selectedSegment, 0)
    XCTAssertEqual(selectedName(in: classTable(in: browser), values: browser.model.classes), "Object")
    XCTAssertEqual(browser.model.classes, classes)

    pending.first?(true)
    XCTAssertEqual(pending.count, 1)
    XCTAssertEqual(browser.model.selectedClass, "Array")
    XCTAssertTrue(browser.sourceText.contains("subclass: #Array"))
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertEqual(browser.model.classes, classes)
  }

  // The answer is applied to the pane as it is when the answer comes: with no unaccepted edit
  // left, Cancel has nothing to keep and the change goes through.
  func testTheDiscardAnswerLooksAtThePaneAgain() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    var pending: [@MainActor (Bool) -> Void] = []
    browser.confirmDiscard = { _, decide in pending.append(decide) }
    selectClass("Array", in: browser)
    let shown = browser.sourceText
    browser.replaceSource(shown + " ")
    selectClass("SmallInteger", in: browser, expecting: "Array")
    browser.replaceSource(shown)
    XCTAssertFalse(browser.hasUnacceptedChanges)
    pending.first?(false)
    XCTAssertEqual(pending.count, 1)
    XCTAssertEqual(browser.model.selectedClass, "SmallInteger")
    XCTAssertTrue(browser.sourceText.contains("subclass: #SmallInteger"))
  }

  // The default question is one sheet: Show Hierarchy while it is up adds none. Return is left
  // unbound, Escape is Cancel, and Discard is marked destructive; each button does its part.
  func testDiscardSheetIsNotStackedAndReturnDoesNotDiscard() {
    let browser = BrowserWindow()
    defer {
      for sheet in browser.window.sheets {
        browser.window.endSheet(sheet)
      }
      browser.window.close()
    }
    selectProtocol("user", in: browser)
    let edited = "b5s\n  ^1\n"
    browser.replaceSource(edited)
    selectClass("Array", in: browser, expecting: "Object")
    XCTAssertTrue(turnRunLoop(until: { browser.window.sheets.count == 1 }))
    browser.showHierarchy()
    _ = turnRunLoop(until: { browser.window.sheets.count > 1 }, seconds: 0.3)
    XCTAssertEqual(browser.window.sheets.count, 1)

    guard let sheet = browser.window.attachedSheet else {
      XCTFail("missing sheet")
      return
    }
    // The shown alert answers Return with the sheet's default button cell or a button whose key
    // is Return; there is neither, so Return never discards.
    let buttons = views(in: sheet.contentView, of: NSButton.self)
    XCTAssertNil(sheet.defaultButtonCell)
    XCTAssertFalse(buttons.contains { $0.keyEquivalent == "\r" })
    XCTAssertEqual(buttons.first { $0.title == "Cancel" }?.keyEquivalent, "\u{1B}")
    XCTAssertEqual(buttons.first { $0.title == "Discard" }?.hasDestructiveAction, true)
    buttons.first { $0.title == "Cancel" }?.performClick(nil)
    XCTAssertTrue(turnRunLoop(until: { browser.window.sheets.isEmpty }))
    XCTAssertEqual(browser.sourceText, edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(selectedName(in: classTable(in: browser), values: browser.model.classes), "Object")

    selectClass("Array", in: browser, expecting: "Object")
    XCTAssertTrue(turnRunLoop(until: { browser.window.sheets.count == 1 }))
    views(in: browser.window.attachedSheet?.contentView, of: NSButton.self)
      .first { $0.title == "Discard" }?
      .performClick(nil)
    XCTAssertTrue(turnRunLoop(until: { browser.window.sheets.isEmpty }))
    XCTAssertEqual(browser.model.selectedClass, "Array")
    XCTAssertFalse(browser.hasUnacceptedChanges)
  }

  func testClassDefinitionPaneAcceptsClassSource() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
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

  // An accepted class definition selects the class it defines, in its category, and shows its
  // definition: for a new class, and for a class whose category alone changed.
  func testClassDefinitionAcceptSelectsTheDefinedClassInItsCategory() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectClass("Array", in: browser)
    let def =
      "Object subclass: #B5Defined\n  instanceVariableNames: 'a'\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B5-Defined'\n"
    browser.replaceSource(def)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    assertShowsDefinition(of: "B5Defined", in: "B5-Defined", browser: browser)

    let moved = def.replacingOccurrences(of: "'B5-Defined'", with: "'B5-Moved'")
    browser.replaceSource(moved)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    assertShowsDefinition(of: "B5Defined", in: "B5-Moved", browser: browser)
    XCTAssertFalse(browser.model.categories.contains("B5-Defined"))
  }

  // A comment before the definition that quotes another definition is not the definition: the
  // Browser selects the class the runtime defined.
  func testClassDefinitionAcceptAfterACommentQuotingSubclassSelectsTheDefinedClass() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectClass("Array", in: browser)
    let def =
      "\"Example: Object subclass: #Object\" Object subclass: #B5Commented\n"
      + "  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B5-Commented'\n"
    browser.replaceSource(def)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    assertShowsDefinition(of: "B5Commented", in: "B5-Commented", browser: browser)
  }

  // A string literal that holds subclass: (here in a method chunk before the definition) is not
  // the definition either.
  func testClassDefinitionAcceptAfterAStringHoldingSubclassSelectsTheDefinedClass() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectClass("Array", in: browser)
    browser.replaceSource(
      "Object subclass: #B5Host\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'B5-Host'\n"
    )
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    let source =
      "B5Host methodsFor: 'notes'!\n"
      + "note\n  ^'Object subclass: #Object'! !\n"
      + "Object subclass: #B5AfterString\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B5-AfterString'\n"
    browser.replaceSource(source)
    browser.accept()
    XCTAssertEqual(browser.errorText, "")
    assertShowsDefinition(of: "B5AfterString", in: "B5-AfterString", browser: browser)
  }

  func testClassSideAcceptUsesMeta() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
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
    defer { workspace.window.close() }
    let expr = "Transcript extra"
    workspace.replaceText(expr)
    workspace.selectAll()
    workspace.printIt()
    XCTAssertEqual(String(workspace.text.dropFirst(expr.count)), "7")
  }

  // SPEC §3.9 削除, §4.3: Remove Method… asks with the method's name; Remove takes it out, the
  // cached send falls to doesNotUnderstand:, the selector row goes and the protocol stays while it
  // lists a method. The class side asks with `Foo class>>bar`.
  func testRemoveMethodAfterConfirmUpdatesLists() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("b12a\n  ^1\n")
    browser.accept()
    browser.replaceSource("b12b\n  ^2\n")
    browser.accept()
    XCTAssertEqual(browser.model.selectedSelector, "b12b")
    XCTAssertEqual(printIt("Object new b12b"), "2")
    var asked: [String] = []
    browser.confirmRemove = { window, message, decide in
      XCTAssertTrue(window === browser.window)
      asked.append(message)
      decide(true)
    }
    XCTAssertTrue(browser.canRemoveMethod)
    browser.removeMethod()
    XCTAssertEqual(asked, ["Remove Object>>b12b?"])
    XCTAssertEqual(browser.errorText, "")
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertEqual(browser.model.selectedProtocol, "user")
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(browser.model.selectors, ["b12a"])
    XCTAssertNil(selectedName(in: selectorTable(in: browser), values: browser.model.selectors))
    XCTAssertEqual(browser.sourceText, "")
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertFalse(browser.canRemoveMethod)
    XCTAssertNil(printIt("Object new b12b"))
    XCTAssertEqual(printIt("Object new b12a"), "1")
    // Nothing selected: the menu action does nothing and asks nothing.
    browser.removeMethod()
    XCTAssertEqual(asked.count, 1)

    var err = AoSpan()
    let accepted = "b12c\n  ^3\n".withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_method("Object", 1, src, $0) }
    }
    XCTAssertEqual(accepted, Int32(AO_OK), spanMessage(err))
    guard let side = segmentedControls(in: browser.window.contentView).first else {
      XCTFail("missing instance/class switch")
      return
    }
    side.selectedSegment = 1
    XCTAssertTrue(side.sendAction(side.action, to: side.target))
    selectProtocol("user", in: browser)
    selectSelector("b12c", in: browser)
    browser.removeMethod()
    XCTAssertEqual(asked, ["Remove Object>>b12b?", "Remove Object class>>b12c?"])
    XCTAssertNil(printIt("Object b12c"))
    XCTAssertEqual(browser.errorText, "")
  }

  // SPEC §3.9 削除: Cancel changes nothing: the method, the class, the rows and the pane stay.
  func testCancelRemoveChangesNothing() {
    var err = AoSpan()
    let def = "Object subclass: #B12Cancel\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
      + "  poolDictionaries: ''\n  category: 'B12-Cancel'\n"
    let defined = def.withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
    }
    XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("b12keep\n  ^1\n")
    browser.accept()
    var asked = 0
    browser.confirmRemove = { _, _, decide in
      asked += 1
      decide(false)
    }
    browser.removeMethod()
    XCTAssertEqual(asked, 1)
    XCTAssertEqual(browser.model.selectedSelector, "b12keep")
    XCTAssertEqual(browser.model.selectors, ["b12keep"])
    XCTAssertEqual(browser.sourceText, "b12keep\n  ^1\n")
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(printIt("Object new b12keep"), "1")

    selectCategory("B12-Cancel", in: browser)
    selectClass("B12Cancel", in: browser)
    browser.removeClass()
    XCTAssertEqual(asked, 2)
    XCTAssertEqual(browser.model.selectedClass, "B12Cancel")
    XCTAssertEqual(browser.model.classes, ["B12Cancel"])
    XCTAssertTrue(browser.model.categories.contains("B12-Cancel"))
    // Erratum: Object>>printString answers the receiver's class name, so a class object prints
    // its metaclass name.
    XCTAssertEqual(printIt("B12Cancel"), "B12Cancel class")
    XCTAssertTrue(browser.sourceText.contains("subclass: #B12Cancel"))
  }

  // SPEC §3.9 削除, §6: Remove Class… takes the class off the lists; its category goes with its
  // last class (the first category is shown then), a category with a class left keeps it with no
  // class selected, and the hierarchy list loses the name too.
  func testRemoveClassAfterConfirmUpdatesLists() {
    var err = AoSpan()
    for (name, category) in [("B12Gone", "B12-Gone"), ("B12StayA", "B12-Stay"), ("B12StayB", "B12-Stay")] {
      let def = "Object subclass: #\(name)\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: '\(category)'\n"
      let defined = def.withCString { src in
        withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
      }
      XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    }
    let browser = BrowserWindow()
    defer { browser.window.close() }
    XCTAssertEqual(printIt("B12Gone new printString"), "'B12Gone'")
    var asked: [String] = []
    browser.confirmRemove = { _, message, decide in
      asked.append(message)
      decide(true)
    }
    selectCategory("B12-Gone", in: browser)
    selectClass("B12Gone", in: browser)
    XCTAssertTrue(browser.canRemoveClass)
    browser.removeClass()
    XCTAssertEqual(asked, ["Remove class B12Gone?"])
    XCTAssertEqual(browser.errorText, "")
    XCTAssertFalse(browser.model.classes.contains("B12Gone"))
    XCTAssertFalse(browser.model.categories.contains("B12-Gone"))
    XCTAssertNil(browser.model.selectedClass)
    XCTAssertFalse(browser.canRemoveClass)
    XCTAssertEqual(selectedName(in: categoryTable(in: browser), values: browser.model.categories),
                   browser.model.categories.first)
    XCTAssertEqual(printIt("B12Gone"), "nil")
    // Nothing selected: the menu action does nothing.
    browser.removeClass()
    XCTAssertEqual(asked.count, 1)

    selectCategory("B12-Stay", in: browser)
    selectClass("B12StayA", in: browser)
    browser.removeClass()
    XCTAssertEqual(asked.last, "Remove class B12StayA?")
    XCTAssertTrue(browser.model.categories.contains("B12-Stay"))
    XCTAssertEqual(selectedName(in: categoryTable(in: browser), values: browser.model.categories), "B12-Stay")
    XCTAssertEqual(browser.model.classes, ["B12StayB"])
    XCTAssertNil(browser.model.selectedClass)
    XCTAssertNil(selectedName(in: classTable(in: browser), values: browser.model.classes))
    XCTAssertEqual(browser.sourceText, "")

    selectClass("B12StayB", in: browser)
    browser.showHierarchy()
    XCTAssertTrue(browser.model.classes.contains("Object"))
    XCTAssertTrue(browser.model.classes.contains("B12StayB"))
    browser.removeClass()
    XCTAssertEqual(asked.last, "Remove class B12StayB?")
    XCTAssertTrue(browser.model.classes.contains("Object"))
    XCTAssertFalse(browser.model.classes.contains("B12StayB"))
    XCTAssertNil(browser.model.selectedClass)
    XCTAssertEqual(printIt("B12StayB"), "nil")
  }

  // SPEC §3.9 削除: a refusal shows its reason in the error field and changes no row or pane.
  func testRefusedRemoveShowsReason() {
    var err = AoSpan()
    for (name, superclass) in [("B12Par", "Object"), ("B12Kid", "B12Par")] {
      let def = "\(superclass) subclass: #\(name)\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'B12-Sub'\n"
      let defined = def.withCString { src in
        withUnsafeMutablePointer(to: &err) { ao_accept_class(src, $0) }
      }
      XCTAssertEqual(defined, Int32(AO_OK), spanMessage(err))
    }
    let browser = BrowserWindow()
    defer { browser.window.close() }
    browser.confirmRemove = { _, _, decide in decide(true) }
    // Kernel / Object / native / printString is the initial selection.
    XCTAssertEqual(browser.model.selectedSelector, "printString")
    let shown = browser.sourceText
    let selectors = browser.model.selectors
    browser.removeMethod()
    XCTAssertEqual(browser.errorText, "native method removal refused: Object>>printString")
    XCTAssertEqual(browser.model.selectedSelector, "printString")
    XCTAssertEqual(browser.model.selectors, selectors)
    XCTAssertEqual(browser.sourceText, shown)
    XCTAssertEqual(selectedName(in: selectorTable(in: browser), values: browser.model.selectors), "printString")

    let classes = browser.model.classes
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "class removal refused: Object is a fixed global")
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(browser.model.classes, classes)
    XCTAssertEqual(browser.model.selectedSelector, "printString")

    selectCategory("B12-Sub", in: browser)
    selectClass("B12Par", in: browser)
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "class removal refused: B12Par has subclass B12Kid")
    // The class list is always alphabetical (Session::classRows sorts by name), not definition
    // order.
    XCTAssertEqual(browser.model.classes, ["B12Kid", "B12Par"])
    XCTAssertEqual(browser.model.selectedClass, "B12Par")
    // The next successful action clears the field.
    selectClass("B12Kid", in: browser)
    browser.removeClass()
    XCTAssertEqual(browser.errorText, "")
    XCTAssertEqual(browser.model.classes, ["B12Par"])
  }

  // SPEC §3.9 削除: an unaccepted edit asks to discard first; keeping it ends the command, and
  // discarding it goes on to the removal question.
  func testRemoveAsksToDiscardEditsFirst() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("b12x\n  ^1\n")
    browser.accept()
    browser.replaceSource("b12y\n  ^2\n")
    browser.accept()
    var discardAsked = 0
    var discard = false
    browser.confirmDiscard = { _, decide in
      discardAsked += 1
      decide(discard)
    }
    var removeAsked: [String] = []
    browser.confirmRemove = { _, message, decide in
      removeAsked.append(message)
      decide(true)
    }
    let edited = "b12y\n  ^9\n"
    browser.replaceSource(edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    browser.removeMethod()
    XCTAssertEqual(discardAsked, 1)
    XCTAssertEqual(removeAsked, [])
    XCTAssertEqual(browser.sourceText, edited)
    XCTAssertTrue(browser.hasUnacceptedChanges)
    XCTAssertEqual(printIt("Object new b12y"), "2")

    discard = true
    browser.removeMethod()
    XCTAssertEqual(discardAsked, 2)
    XCTAssertEqual(removeAsked, ["Remove Object>>b12y?"])
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertNil(printIt("Object new b12y"))
    XCTAssertEqual(browser.model.selectors, ["b12x"])
    // The class command asks the same way.
    browser.replaceSource("b12z\n  ^3\n")
    discard = false
    browser.removeClass()
    XCTAssertEqual(discardAsked, 3)
    XCTAssertEqual(removeAsked.count, 1)
    XCTAssertEqual(browser.sourceText, "b12z\n  ^3\n")
  }

  // SPEC §3.9 削除, §4.3: the Smalltalk menu's two items, without keys, enabled only while the
  // Browser is key and has a selector (Remove Method…) or a class (Remove Class…).
  func testRemoveMenuItemsFollowSelection() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
    var key: NSWindow? = browser.window
    var actions = MainMenu.Actions()
    actions.canRemoveMethod = { keyBrowserAllows(browser, keyWindow: key) { $0.canRemoveMethod } }
    actions.canRemoveClass = { keyBrowserAllows(browser, keyWindow: key) { $0.canRemoveClass } }
    var removed: [String] = []
    actions.removeMethod = { removed.append("method") }
    actions.removeClass = { removed.append("class") }
    let menu = MainMenu.build(actions: actions)
    let smalltalk = menu.item(withTitle: "Smalltalk")?.submenu
    XCTAssertEqual(
      smalltalk?.items.map(\.title),
      ["Do it", "Print it", "Inspect it", "Debug it", "Accept", "Remove Method…", "Remove Class…", "Show Hierarchy"]
    )
    guard let method = smalltalk?.item(withTitle: "Remove Method…"),
          let cls = smalltalk?.item(withTitle: "Remove Class…") else {
      XCTFail("missing Remove items")
      return
    }
    XCTAssertEqual(method.keyEquivalent, "")
    XCTAssertEqual(cls.keyEquivalent, "")
    func enabled(_ item: NSMenuItem) -> Bool {
      (item.target as? NSMenuItemValidation)?.validateMenuItem(item) ?? true
    }
    // Kernel / Object / native / printString: both.
    XCTAssertTrue(enabled(method))
    XCTAssertTrue(enabled(cls))
    selectProtocol(nil, in: browser)
    XCTAssertFalse(enabled(method))
    XCTAssertTrue(enabled(cls))
    key = workspace.window
    XCTAssertFalse(enabled(method))
    XCTAssertFalse(enabled(cls))
    key = nil
    XCTAssertFalse(enabled(cls))
    // Items without a test stay enabled.
    if let accept = smalltalk?.item(withTitle: "Accept") {
      XCTAssertTrue(enabled(accept))
    }
    _ = (method.target as? NSObject)?.perform(method.action, with: method)
    XCTAssertEqual(removed, ["method"])
    _ = (cls.target as? NSObject)?.perform(cls.action, with: cls)
    XCTAssertEqual(removed, ["method", "class"])
    // The app's wiring: not the key window, nothing runs.
    sendToKeyBrowser(browser, keyWindow: workspace.window) { $0.removeMethod() }
    XCTAssertEqual(browser.model.selectedClass, "Object")
  }

  // SPEC §3.9 削除: the class and selector lists offer one item each, labelled as titled, enabled
  // for a clicked row; choosing it selects that row (asking to discard an edit first) and asks the
  // removal question. A question already up ignores a second choice.
  func testContextMenusOfferRemove() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    guard let classes = classTable(in: browser), let selectors = selectorTable(in: browser),
          let classMenu = classes.menu, let selectorMenu = selectors.menu else {
      XCTFail("missing context menus")
      return
    }
    XCTAssertEqual(classMenu.items.map(\.title), ["Remove Class…"])
    XCTAssertEqual(selectorMenu.items.map(\.title), ["Remove Method…"])
    XCTAssertEqual(classMenu.items.first?.accessibilityLabel(), "Remove Class…")
    XCTAssertEqual(selectorMenu.items.first?.accessibilityLabel(), "Remove Method…")
    XCTAssertTrue(classMenu.items.first?.target === browser)
    XCTAssertTrue(selectorMenu.items.first?.target === browser)
    XCTAssertFalse(classMenu.autoenablesItems)
    // No click yet (clickedRow -1): the items are disabled.
    browser.menuNeedsUpdate(classMenu)
    browser.menuNeedsUpdate(selectorMenu)
    XCTAssertFalse(classMenu.items.first?.isEnabled ?? true)
    XCTAssertFalse(selectorMenu.items.first?.isEnabled ?? true)

    selectProtocol("user", in: browser)
    browser.replaceSource("b12m\n  ^1\n")
    browser.accept()
    browser.replaceSource("b12n\n  ^2\n")
    browser.accept()
    XCTAssertEqual(browser.model.selectedSelector, "b12n")
    var asked: [String] = []
    browser.confirmRemove = { _, message, decide in
      // The clicked row is selected before the question.
      XCTAssertEqual(browser.model.selectedSelector, "b12m")
      asked.append(message)
      decide(true)
    }
    guard let row = browser.model.selectors.firstIndex(of: "b12m") else {
      XCTFail("missing b12m")
      return
    }
    browser.removeMethod(atRow: row)
    XCTAssertEqual(asked, ["Remove Object>>b12m?"])
    XCTAssertEqual(browser.model.selectors, ["b12n"])
    XCTAssertNil(browser.model.selectedSelector)
    XCTAssertNil(printIt("Object new b12m"))
    // Out of range: nothing.
    browser.removeMethod(atRow: -1)
    browser.removeClass(atRow: browser.model.classes.count)
    XCTAssertEqual(asked.count, 1)

    // An edit: the discard question comes first, and a second choice while it waits is ignored.
    selectSelector("b12n", in: browser)
    browser.replaceSource("b12n\n  ^9\n")
    var pending: (@MainActor (Bool) -> Void)?
    var discardAsked = 0
    browser.confirmDiscard = { _, decide in
      discardAsked += 1
      pending = decide
    }
    guard let objectRow = browser.model.classes.firstIndex(of: "Object") else {
      XCTFail("missing Object")
      return
    }
    browser.removeClass(atRow: objectRow)
    browser.removeClass(atRow: objectRow)
    XCTAssertEqual(discardAsked, 1)
    XCTAssertEqual(asked.count, 1)
    pending?(false)
    XCTAssertEqual(asked.count, 1)
    XCTAssertEqual(browser.sourceText, "b12n\n  ^9\n")
    XCTAssertEqual(browser.model.selectedSelector, "b12n")
    browser.confirmRemove = { _, message, decide in
      asked.append(message)
      decide(false)
    }
    browser.removeClass(atRow: objectRow)
    XCTAssertEqual(discardAsked, 2)
    pending?(true)
    XCTAssertEqual(asked, ["Remove Object>>b12m?", "Remove class Object?"])
    XCTAssertFalse(browser.hasUnacceptedChanges)
    XCTAssertEqual(browser.model.selectedClass, "Object")
    XCTAssertEqual(printIt("Object new b12n"), "2")
  }

  func testAcceptRunsOnlyWhenBrowserIsKey() {
    let browser = BrowserWindow()
    defer { browser.window.close() }
    selectProtocol("user", in: browser)
    browser.replaceSource("foo\n  ^1\n")
    let workspace = WorkspaceWindow()
    defer { workspace.window.close() }
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
    defer { browser.window.close() }
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
    // SPEC §3.10: the hook outlives the session; it must not reach the next test's session.
    defer { ao_set_transcript_hook(nil, nil) }
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-accept-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(saveImageFile(at: url), Int32(AO_OK))
    XCTAssertEqual(openImageFile(at: url, transcript: transcript).status, Int32(AO_OK))
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
    // SPEC §3.10 / §3.11: the alert says why ao_image_load refused the file.
    XCTAssertTrue(alerts.last?.informativeText.contains("not an Ao image") ?? false)
  }

  // SPEC §3.9, §3.11: with a halted process the save is refused, and the alert says why.
  func testSaveWithHaltedProcessShowsReason() {
    let app = AoApp()
    var alerts: [NSAlert] = []
    app.presentAlert = { alerts.append($0) }
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-halted-\(UUID().uuidString).aoimage")
    defer {
      try? FileManager.default.removeItem(at: url)
      ao_set_debug_mode(Int32(AO_DEBUG_POSTMORTEM))
    }
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    var out = [CChar](repeating: 0, count: 64)
    var err = AoSpan()
    let status = "self halt".withCString { src in
      withUnsafeMutablePointer(to: &err) { ao_eval(src, 9, Int32(AO_EVAL_DOIT), &out, 64, $0) }
    }
    XCTAssertEqual(status, Int32(AO_ERR_HALT))
    XCTAssertFalse(app.saveImage(to: url))
    XCTAssertEqual(alerts.last?.messageText, "Could not save the image")
    XCTAssertTrue(alerts.last?.informativeText.contains("halted processes") ?? false)
    XCTAssertEqual(ao_debug_abort(ao_debug_halted_pid()), Int32(AO_OK))
    XCTAssertTrue(app.saveImage(to: url))
  }

  // `expecting` is the selection after the click when it differs from the clicked row
  // (a cancelled change keeps the old one).
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

  private func assertShowsDefinition(
    of name: String,
    in category: String,
    browser: BrowserWindow,
    line: UInt = #line
  ) {
    XCTAssertEqual(browser.model.selectedClass, name, line: line)
    XCTAssertNil(browser.model.selectedProtocol, line: line)
    XCTAssertNil(browser.model.selectedSelector, line: line)
    XCTAssertEqual(
      selectedName(in: categoryTable(in: browser), values: browser.model.categories),
      category,
      line: line
    )
    XCTAssertEqual(browser.model.classes, [name], line: line)
    XCTAssertEqual(
      selectedName(in: classTable(in: browser), values: browser.model.classes),
      name,
      line: line
    )
    XCTAssertTrue(browser.sourceText.contains("subclass: #\(name)"), line: line)
    XCTAssertTrue(browser.sourceText.contains("category: '\(category)'"), line: line)
    XCTAssertFalse(browser.hasUnacceptedChanges, line: line)
  }

  // Sheets come and go on the run loop. True when `done` holds within `seconds`.
  private func turnRunLoop(until done: () -> Bool, seconds: TimeInterval = 2) -> Bool {
    let deadline = Date(timeIntervalSinceNow: seconds)
    while !done(), Date() < deadline {
      RunLoop.current.run(until: Date(timeIntervalSinceNow: 0.02))
    }
    return done()
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

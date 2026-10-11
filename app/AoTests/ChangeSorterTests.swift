import AppKit
import XCTest
@testable import Ao

@MainActor
final class ChangeSorterTests: XCTestCase {
  override func tearDown() {
    // tearDown is nonisolated; XCTest calls it on the main thread.
    MainActor.assumeIsolated { Self.closeVisibleWindows() }
    ao_set_transcript_hook(nil, nil)
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

  private func boot() {
    _ = NSApplication.shared
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
  }

  private func acceptClass(_ name: String, ivars: String = "") {
    var err = AoSpan()
    let source = "Object subclass: #\(name)\n  instanceVariableNames: '\(ivars)'\n"
      + "  classVariableNames: ''\n  poolDictionaries: ''\n  category: 'P16-Test'"
    XCTAssertEqual(ao_accept_class(source, &err), Int32(AO_OK), spanMessage(err))
  }

  private func acceptMethod(_ className: String, meta: Bool = false, _ source: String) {
    var err = AoSpan()
    XCTAssertEqual(ao_accept_method(className, meta ? 1 : 0, source, &err), Int32(AO_OK), spanMessage(err))
  }

  // SPEC §3.9 Change Sorter: a new session lists Unnamed1, current and selected.
  func testNewSessionListsUnnamed1AsCurrentAndSelected() {
    boot()
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.sets.map(\.name), ["Unnamed1"])
    XCTAssertEqual(model.selectedSetID, model.currentID)
    XCTAssertEqual(model.currentName, "Unnamed1")
    XCTAssertEqual(model.classes, [])
    XCTAssertEqual(model.source, "")
  }

  // SPEC §3.9 Change Sorter: the class shows its definition kind, the methods theirs (the class side
  // as `class » <selector>`), and the source pane the accepted text.
  func testAcceptedMethodShowsItsKindAndSource() {
    boot()
    acceptClass("P16Counter", ivars: "count")
    let source = "increment\n  count := (count ifNil: [0]) + 1.\n  ^count"
    acceptMethod("P16Counter", source)
    acceptMethod("P16Counter", meta: true, "make\n  ^self new")
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16Counter", kind: .added)])
    model.select(setID: model.currentID, className: "P16Counter")
    XCTAssertEqual(model.methods.map(\.label), ["increment", "class » make"])
    XCTAssertEqual(model.methods.map(\.kind.mark), ["+", "+"])
    XCTAssertTrue(model.source.hasPrefix("Object subclass: #P16Counter"), model.source)
    model.select(setID: model.currentID, className: "P16Counter", method: model.methods[0])
    XCTAssertEqual(model.source, source)
  }

  // SPEC §3.9 Change Sorter: a removed method or class shows `removed`.
  func testRemovedMethodAndRemovedClassShowRemoved() {
    boot()
    acceptClass("P16Gone")
    acceptMethod("P16Gone", "old\n  ^1")
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Later", nil, &err), Int32(AO_OK))
    XCTAssertEqual(ao_remove_method(BrowserModel.classID(named: "P16Gone"), 0, "old", &err), Int32(AO_OK))
    let model = ChangeSorterModel()
    model.refresh()
    model.select(
      setID: model.currentID,
      className: "P16Gone",
      method: ChangeMethodRow(selector: "old", meta: false, kind: .removed)
    )
    XCTAssertEqual(model.methods.map(\.kind.mark), ["−"])
    XCTAssertEqual(model.source, "removed")
    XCTAssertEqual(ao_remove_class(BrowserModel.classID(named: "P16Gone"), &err), Int32(AO_OK))
    model.refresh()
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16Gone", kind: .removed)])
    XCTAssertNil(model.selectedMethod)
    XCTAssertEqual(model.source, "removed")
  }

  // SPEC §3.9 Change Sorter: New's default is the smallest free Unnamed<n>; the new set is current
  // and selected; a taken or empty name is refused with the runtime's reason.
  func testNewSetIsCurrentAndNextFreeNameSkipsTakenNames() {
    boot()
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.nextFreeName(), "Unnamed2")
    XCTAssertEqual(model.newSet(named: "Unnamed3").status, Int32(AO_OK))
    XCTAssertEqual(model.currentName, "Unnamed3")
    XCTAssertEqual(model.selectedSetID, model.currentID)
    XCTAssertEqual(model.nextFreeName(), "Unnamed2")
    let taken = model.newSet(named: "Unnamed1")
    XCTAssertEqual(taken.status, Int32(AO_ERR))
    XCTAssertEqual(taken.message, "change set name taken: Unnamed1")
    XCTAssertEqual(model.newSet(named: "").message, "change set name empty")
    XCTAssertEqual(model.sets.map(\.name), ["Unnamed1", "Unnamed3"])
  }

  // SPEC §3.9 Change Sorter: Make Current, Move to and Remove from Change Set change the lists.
  func testMakeCurrentRemoveAndMoveUpdateTheLists() {
    boot()
    acceptClass("P16Move")
    acceptMethod("P16Move", "a\n  ^1")
    acceptMethod("P16Move", "b\n  ^2")
    let model = ChangeSorterModel()
    model.refresh()
    let first = model.currentID
    XCTAssertEqual(model.newSet(named: "Target").status, Int32(AO_OK))
    let target = model.currentID
    XCTAssertEqual(model.makeCurrent(first), Int32(AO_OK))
    XCTAssertEqual(model.currentID, first)
    model.select(setID: first, className: "P16Move")
    let a = model.methods[0]
    XCTAssertEqual(model.move(className: "P16Move", method: a, to: target).status, Int32(AO_OK))
    XCTAssertEqual(model.methods.map(\.label), ["b"])
    XCTAssertEqual(model.remove(className: "P16Move", method: nil).status, Int32(AO_OK))
    XCTAssertEqual(model.classes, [])
    XCTAssertNil(model.selectedClass)
    model.select(setID: target, className: "P16Move")
    XCTAssertEqual(model.methods.map(\.label), ["a"])
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16Move", kind: .none)])
  }

  // SPEC §3.9 Change Sorter: the current set's row starts with `• `, and the subtitle names it.
  func testWindowListsSetsWithTheCurrentMarkAndSubtitle() {
    boot()
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Second", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.window.title, "Change Sorter")
    XCTAssertEqual(sorter.window.subtitle, "Second is current")
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Second"])
    XCTAssertFalse(sorter.canMakeCurrent)
  }

  // SPEC §3.9 Change Sorter: the lists carry the marks; the source pane follows the selection.
  func testSelectingAClassAndMethodShowsMarksAndSource() {
    boot()
    acceptClass("P16Win")
    acceptMethod("P16Win", "a\n  ^1")
    acceptMethod("P16Win", meta: true, "make\n  ^self new")
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    XCTAssertEqual(sorter.classTitles, ["P16Win +"])
    XCTAssertEqual(sorter.methodTitles, ["a +", "class » make +"])
    XCTAssertTrue(sorter.sourceText.hasPrefix("Object subclass: #P16Win"), sorter.sourceText)
    sorter.selectRows(set: 0, class: 0, method: 1)
    XCTAssertEqual(sorter.sourceText, "make\n  ^self new")
  }

  // SPEC §3.9 Change Sorter: New… offers the next free name; the new set is current and selected; a
  // refusal goes to the error field; Cancel changes nothing.
  func testNewButtonAsksForTheNextFreeNameAndSelectsTheNewCurrentSet() {
    boot()
    let sorter = ChangeSorterWindow()
    var asked: String?
    sorter.askName = { _, suggested, answer in
      asked = suggested
      answer("Fixes")
    }
    sorter.newSet()
    XCTAssertEqual(asked, "Unnamed2")
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Fixes"])
    XCTAssertEqual(sorter.window.subtitle, "Fixes is current")
    XCTAssertEqual(sorter.model.selectedSetName, "Fixes")
    XCTAssertEqual(sorter.errorText, "")
    sorter.askName = { _, _, answer in answer("Fixes") }
    sorter.newSet()
    XCTAssertEqual(sorter.errorText, "change set name taken: Fixes")
    sorter.askName = { _, _, answer in answer(nil) }
    sorter.newSet()
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Fixes"])
  }

  // SPEC §3.9 Change Sorter: Make Current moves the mark and then is disabled.
  func testMakeCurrentMovesTheMark() {
    boot()
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Second", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0)
    XCTAssertTrue(sorter.canMakeCurrent)
    sorter.makeCurrent()
    XCTAssertEqual(sorter.setTitles, ["• Unnamed1", "Second"])
    XCTAssertFalse(sorter.canMakeCurrent)
  }

  // SPEC §3.9 Change Sorter: the entry menu is Remove from Change Set, a separator and Move to with
  // the other sets; without an entry its items are disabled. Move to moves the entry.
  func testEntryMenuOffersRemoveAndMoveToTheOtherSets() {
    boot()
    acceptClass("P16Menu")
    acceptMethod("P16Menu", "a\n  ^1")
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Other", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    let menu = sorter.entryMenu(className: "P16Menu", method: sorter.model.methods.first)
    XCTAssertEqual(menu.items.map(\.title), ["Remove from Change Set", "", "Move to"])
    XCTAssertEqual(menu.items[2].submenu?.items.map(\.title), ["Other"])
    XCTAssertTrue(menu.items[0].isEnabled)
    XCTAssertTrue(menu.items[2].isEnabled)
    let empty = sorter.entryMenu(className: nil, method: nil)
    XCTAssertFalse(empty.items[0].isEnabled)
    XCTAssertFalse(empty.items[2].isEnabled)
    guard let moveItem = menu.items[2].submenu?.items.first, let action = moveItem.action else {
      XCTFail("missing Move to item")
      return
    }
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: moveItem.target, from: moveItem))
    XCTAssertEqual(sorter.methodTitles, [])
    sorter.selectRows(set: 1, class: 0)
    XCTAssertEqual(sorter.classTitles, ["P16Menu"])
    XCTAssertEqual(sorter.methodTitles, ["a +"])
  }

  // SPEC §3.9 Change Sorter: Remove from Change Set on a class row forgets the entry; the class stays.
  func testRemoveFromMenuForgetsTheClassEntryButKeepsTheClass() {
    boot()
    acceptClass("P16Forget")
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    let menu = sorter.entryMenu(className: "P16Forget", method: nil)
    guard let action = menu.items[0].action else {
      XCTFail("missing Remove from Change Set")
      return
    }
    XCTAssertTrue(NSApplication.shared.sendAction(action, to: menu.items[0].target, from: menu.items[0]))
    XCTAssertEqual(sorter.classTitles, [])
    XCTAssertNotEqual(BrowserModel.classID(named: "P16Forget"), 0)
  }

  // SPEC §3.9 Change Sorter: File Out… offers `<set>.st` and writes the selected set.
  func testFileOutButtonWritesTheSelectedSet() {
    boot()
    acceptClass("P16Out")
    acceptMethod("P16Out", "a\n  ^1")
    let sorter = ChangeSorterWindow()
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p16-\(UUID().uuidString).st")
    defer { try? FileManager.default.removeItem(at: url) }
    var suggested: String?
    sorter.chooseFileOut = { _, name, answer in
      suggested = name
      answer(url)
    }
    sorter.fileOut()
    XCTAssertEqual(suggested, "Unnamed1.st")
    let text = (try? String(contentsOf: url, encoding: .utf8)) ?? ""
    XCTAssertTrue(text.hasPrefix("\"Change Set: Unnamed1\n"), text)
    XCTAssertTrue(text.contains("!P16Out methodsFor: 'user'!\na\n  ^1! !\n"), text)
    XCTAssertEqual(sorter.errorText, "")
  }

  // SPEC §3.9 Change Sorter: a failed File Out shows the runtime's reason.
  func testRefusedFileOutShowsTheReason() {
    boot()
    let sorter = ChangeSorterWindow()
    let url = URL(fileURLWithPath: "/nonexistent-ao-p16-dir/x.st")
    sorter.chooseFileOut = { _, _, answer in answer(url) }
    sorter.fileOut()
    XCTAssertEqual(sorter.errorText, "cannot write file: \(url.path)")
  }

  // Review Focus 3: an Accept made while the window is open shows when the window becomes key.
  func testBecomingKeyReadsTheSetsAgain() {
    boot()
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.classTitles, [])
    acceptClass("P16Later")
    sorter.windowDidBecomeKey(Notification(name: NSWindow.didBecomeKeyNotification, object: sorter.window))
    XCTAssertEqual(sorter.classTitles, ["P16Later +"])
  }

  // Review Focus 4: after a load the window shows the new session's one set.
  func testImageLoadStartsTheWindowOverWithUnnamed1() {
    boot()
    let sorter = ChangeSorterWindow()
    sorter.askName = { _, _, answer in answer("Before") }
    sorter.newSet()
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p16-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(ao_image_save(url.path), Int32(AO_OK))
    var err = AoSpan()
    XCTAssertEqual(ao_image_load(url.path, &err), Int32(AO_OK), spanMessage(err))
    sorter.noteImageLoaded()
    XCTAssertEqual(sorter.setTitles, ["• Unnamed1"])
    XCTAssertEqual(sorter.window.subtitle, "Unnamed1 is current")
  }

  // SPEC §3.9 Change Sorter: the VoiceOver labels.
  func testPaneAccessibilityLabels() {
    boot()
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.paneAccessibilityLabels, ["Change sets", "Classes", "Methods", "Source", "Error"])
  }
}

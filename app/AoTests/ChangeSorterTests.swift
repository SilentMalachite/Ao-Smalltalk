import AppKit
import XCTest
@testable import Ao

@MainActor
final class ChangeSorterTests: XCTestCase {
  // SPEC §3.9 起動と同梱 (P17): <repo>/image/Tools-model, three directories above app/AoTests/<file>.
  private static let toolsModelURL = URL(fileURLWithPath: #filePath)
    .deletingLastPathComponent()
    .deletingLastPathComponent()
    .deletingLastPathComponent()
    .appendingPathComponent("image/Tools-model", isDirectory: true)

  override func setUp() {
    super.setUp()
    // launchApp() files the tools model in from here, not from the test bundle's resources.
    setenv("AO_TOOLS_MODEL_DIR", Self.toolsModelURL.path, 1)
  }

  override func tearDown() {
    unsetenv("AO_TOOLS_MODEL_DIR")
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

  // The change sets are the Smalltalk class ChangeSet (P17), so a booted runtime needs the tools model.
  private func boot() {
    _ = NSApplication.shared
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
    let order = Self.toolsModelURL.appendingPathComponent("LOAD_ORDER").path
    XCTAssertEqual(order.withCString { ao_filein_load_order($0) }, Int32(AO_OK))
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

  // SPEC §3.9 Change Sorter (P17): after a load the window shows the sets the image was saved with,
  // and the one that was current is current again.
  func testImageLoadBringsBackTheSavedSetsInTheWindow() {
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
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Before"])
    XCTAssertEqual(sorter.window.subtitle, "Before is current")
  }

  // SPEC §3.9 Change Sorter: the VoiceOver labels.
  func testPaneAccessibilityLabels() {
    boot()
    let sorter = ChangeSorterWindow()
    XCTAssertEqual(sorter.paneAccessibilityLabels, ["Change sets", "Classes", "Methods", "Source", "Error"])
  }

  private func launchApp() -> AoApp {
    let app = AoApp()
    app.applicationWillFinishLaunching(
      Notification(name: NSApplication.willFinishLaunchingNotification, object: NSApplication.shared)
    )
    return app
  }

  private func sendToolsItem(_ title: String) -> Bool {
    guard let item = NSApplication.shared.mainMenu?.item(withTitle: "Tools")?.submenu?.item(withTitle: title),
          let action = item.action else {
      return false
    }
    return NSApplication.shared.sendAction(action, to: item.target, from: item)
  }

  private func temporaryFile(_ text: String, ext: String = "st") -> URL {
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("ao-p16-\(UUID().uuidString).\(ext)")
    XCTAssertNoThrow(try text.write(to: url, atomically: true, encoding: .utf8))
    return url
  }

  // SPEC §3.9 Change Sorter: Tools → Change Sorter opens one window, however often it is chosen.
  func testToolsChangeSorterOpensOneWindow() {
    _ = launchApp()
    let sorters = { NSApplication.shared.windows.filter { $0.title == "Change Sorter" && $0.isVisible } }
    XCTAssertEqual(sorters().count, 0)
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    XCTAssertEqual(sorters().count, 1)
  }

  // SPEC §3.9 File In…: `filed in: <path>`, and the definitions and methods are recorded.
  func testFileInWritesTheFiledInLineAndRecords() {
    let launch = LaunchSet.make()
    // The records go to the Smalltalk ChangeSet (P17), so the tools model comes first.
    fileInToolsModel(at: Self.toolsModelURL, transcript: launch.transcript)
    let toolsLine = "tools model loaded: \(Self.toolsModelURL.path)\n"
    let url = temporaryFile(
      "Object subclass: #P16FileIn\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'P16-In'!\n\n!P16FileIn methodsFor: 'user'!\nx\n  ^1! !\n")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(fileInChangeFile(at: url, transcript: launch.transcript), Int32(AO_OK))
    XCTAssertEqual(launch.transcript.text, toolsLine + "filed in: \(url.path)\n")
    let model = ChangeSorterModel()
    model.refresh()
    XCTAssertEqual(model.classes, [ChangeClassRow(name: "P16FileIn", kind: .added)])
  }

  // SPEC §3.9 File In…: `file-in failed: <path>` and the first error in the `ao filein` form.
  func testFailedFileInWritesTheFailedLineAndTheFirstError() {
    let launch = LaunchSet.make()
    let header = "P16Acount methodsFor: 'user'"
    let url = temporaryFile("!\(header)!\nx\n  ^1! !\n")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(fileInChangeFile(at: url, transcript: launch.transcript), Int32(AO_ERR))
    let end = 1 + header.utf8.count
    XCTAssertEqual(
      launch.transcript.text,
      "file-in failed: \(url.path)\n\(url.path):1-\(end): missing class: P16Acount\n"
    )
  }

  // SPEC §3.9 File Out Change Set…: a failure is an alert naming the file and the reason.
  func testFileOutChangeSetFailureShowsAnAlert() {
    let app = launchApp()
    var alerts: [NSAlert] = []
    app.presentAlert = { alerts.append($0) }
    let url = URL(fileURLWithPath: "/nonexistent-ao-p16-dir/x.st")
    XCTAssertFalse(app.fileOutCurrentChangeSet(to: url))
    XCTAssertEqual(alerts.first?.messageText, "Could not file out the change set")
    XCTAssertEqual(alerts.first?.informativeText, url.path + "\ncannot write file: " + url.path)
  }

  // SPEC §3.9 Change Sorter (P17): Open Image… shows an open Change Sorter the sets of the image.
  func testOpenImageBringsBackTheSavedSetsInAnOpenChangeSorter() {
    let app = launchApp()
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Before", nil, &err), Int32(AO_OK))
    app.changeSorter?.reload()
    XCTAssertEqual(app.changeSorter?.setTitles, ["Unnamed1", "• Before"])
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("ao-p16-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertTrue(app.saveImage(to: url))
    XCTAssertTrue(app.openImage(from: url))
    XCTAssertEqual(app.changeSorter?.setTitles, ["Unnamed1", "• Before"])
  }

  // SPEC §3.9 Change Sorter (P17): a refused read leaves the lists and says busy; the next read
  // after the evaluation shows the sets again and clears the line.
  func testBusyReadKeepsTheListsAndSaysBusy() {
    boot()
    let sorter = ChangeSorterWindow()
    sorter.reload()
    XCTAssertEqual(sorter.model.sets.map(\.name), ["Unnamed1"])
    whileBusy {
      sorter.reload()
      XCTAssertTrue(sorter.model.readRefused)
      XCTAssertEqual(sorter.model.sets.map(\.name), ["Unnamed1"])
      XCTAssertEqual(sorter.errorText, "runtime is busy")
    }
    sorter.reload()
    XCTAssertFalse(sorter.model.readRefused)
    XCTAssertEqual(sorter.errorText, "")
  }

  // SPEC §3.9 Change Sorter (P17): Open Image… shows the sets the image was saved with.
  func testOpenImageShowsTheSavedSets() {
    let app = launchApp()
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Work", nil, &err), Int32(AO_OK), spanMessage(err))
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("ao-p17-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertTrue(app.saveImage(to: url))
    // Unnamed1 current and a third set, so the window shows what the load brings back.
    let first = ChangeSorterModel()
    first.refresh()
    guard let unnamed = first.sets.first?.id else {
      XCTFail("no first set")
      return
    }
    XCTAssertEqual(ao_changes_set_current(unnamed), Int32(AO_OK))
    XCTAssertEqual(ao_changes_new("Later", nil, &err), Int32(AO_OK), spanMessage(err))
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    XCTAssertTrue(app.openImage(from: url))
    XCTAssertEqual(app.changeSorter?.model.sets.map(\.name), ["Unnamed1", "Work"])
    XCTAssertEqual(app.changeSorter?.model.currentName, "Work")
  }

  // SPEC §3.9 起動と同梱 (P17): an image without ChangeSet gets the tools model on Open Image….
  func testOpenImageWithoutChangeSetFilesInTheToolsModel() {
    // A bare boot (no tools model) writes an image without ChangeSet.
    _ = NSApplication.shared
    XCTAssertEqual(ao_runtime_boot(), Int32(AO_OK))
    let url = FileManager.default.temporaryDirectory.appendingPathComponent("ao-p17-\(UUID().uuidString).aoimage")
    defer { try? FileManager.default.removeItem(at: url) }
    XCTAssertEqual(url.path.withCString { ao_image_save($0) }, Int32(AO_OK))
    let app = launchApp()
    XCTAssertTrue(app.openImage(from: url))
    XCTAssertNotEqual("ChangeSet".withCString { ao_browser_class_id($0) }, 0)
    XCTAssertTrue(sendToolsItem("Change Sorter"))
    XCTAssertEqual(app.changeSorter?.model.sets.map(\.name), ["Unnamed1"])
  }

  // SPEC §3.9 起動と同梱 (P17): launch files the tools model in after the vendor.
  func testLaunchFilesInTheToolsModel() {
    _ = launchApp()
    XCTAssertNotEqual("ChangeSet".withCString { ao_browser_class_id($0) }, 0)
    XCTAssertEqual(ao_changes_count(), 1)
  }

  // SPEC §3.9 Change Sorter: while busy (§3.10) New…, Remove from Change Set, Move to and File Out…
  // do nothing; each shows `runtime is busy` in the error field, and the lists stay as they were.
  func testWindowWritesWhileBusyShowRuntimeIsBusy() {
    boot()
    acceptClass("P16Busy")
    acceptMethod("P16Busy", "a\n  ^1")
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Other", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0, class: 0)
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p16-\(UUID().uuidString).st")
    defer { try? FileManager.default.removeItem(at: url) }
    sorter.askName = { _, _, answer in answer("Busy") }
    sorter.chooseFileOut = { _, _, answer in answer(url) }
    guard let other = sorter.model.sets.last?.id else {
      XCTFail("no second set")
      return
    }
    var errors: [String] = []
    whileBusy {
      sorter.newSet()
      errors.append(sorter.errorText)
      sorter.removeEntry(className: "P16Busy", method: nil)
      errors.append(sorter.errorText)
      sorter.moveEntry(className: "P16Busy", method: nil, to: other)
      errors.append(sorter.errorText)
      sorter.fileOut()
      errors.append(sorter.errorText)
    }
    XCTAssertEqual(errors, Array(repeating: "runtime is busy", count: 4))
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Other"])
    XCTAssertEqual(sorter.classTitles, ["P16Busy +"])
    XCTAssertEqual(sorter.methodTitles, ["a +"])
    XCTAssertFalse(FileManager.default.fileExists(atPath: url.path))
  }

  // SPEC §3.9 Change Sorter: while busy Make Current does nothing and shows `Make Current failed`
  // (ao_changes_set_current gives no reason); the mark stays.
  func testMakeCurrentWhileBusyShowsMakeCurrentFailed() {
    boot()
    var err = AoSpan()
    XCTAssertEqual(ao_changes_new("Second", nil, &err), Int32(AO_OK))
    let sorter = ChangeSorterWindow()
    sorter.selectRows(set: 0)
    whileBusy { sorter.makeCurrent() }
    XCTAssertEqual(sorter.errorText, "Make Current failed")
    XCTAssertEqual(sorter.setTitles, ["Unnamed1", "• Second"])
    XCTAssertTrue(sorter.canMakeCurrent)
  }

  // SPEC §3.9 File Out Change Set…: while busy nothing is written; the alert gives `runtime is busy`.
  func testFileOutChangeSetWhileBusyShowsTheReasonInTheAlert() {
    let app = launchApp()
    var alerts: [NSAlert] = []
    app.presentAlert = { alerts.append($0) }
    let url = FileManager.default.temporaryDirectory
      .appendingPathComponent("ao-p16-\(UUID().uuidString).st")
    defer { try? FileManager.default.removeItem(at: url) }
    var wrote = true
    whileBusy { wrote = app.fileOutCurrentChangeSet(to: url) }
    XCTAssertFalse(wrote)
    XCTAssertEqual(alerts.first?.messageText, "Could not file out the change set")
    XCTAssertEqual(alerts.first?.informativeText, url.path + "\nruntime is busy")
    XCTAssertFalse(FileManager.default.fileExists(atPath: url.path))
  }

  // SPEC §3.9 File In…: while busy nothing is filed in; the Transcript gets `runtime is busy` in the
  // failed form.
  func testFileInWhileBusyWritesTheReasonToTheTranscript() {
    let launch = LaunchSet.make()
    let url = temporaryFile(
      "Object subclass: #P16BusyIn\n  instanceVariableNames: ''\n  classVariableNames: ''\n"
        + "  poolDictionaries: ''\n  category: 'P16-In'!\n")
    defer { try? FileManager.default.removeItem(at: url) }
    var status = Int32(AO_OK)
    whileBusy { status = fileInChangeFile(at: url, transcript: launch.transcript) }
    XCTAssertEqual(status, Int32(AO_ERR))
    XCTAssertEqual(launch.transcript.text, "file-in failed: \(url.path)\n\(url.path):0-0: runtime is busy\n")
    XCTAssertEqual(BrowserModel.classID(named: "P16BusyIn"), 0)
  }

  // Runs body inside a transcript hook, where the runtime is busy (SPEC §3.10 再入).
  private func whileBusy(_ body: @escaping () -> Void) {
    let box = Unmanaged.passRetained(BusyBody(body))
    defer { box.release() }
    ao_set_transcript_hook(busyBodyHook, box.toOpaque())
    let source = "Transcript show: 'x'. 7"
    var out = [CChar](repeating: 0, count: 64)
    var err = AoSpan()
    let status = ao_eval(source, Int32(source.utf8.count), Int32(AO_EVAL_PRINTIT), &out, Int32(out.count), &err)
    ao_set_transcript_hook(nil, nil)
    XCTAssertEqual(status, Int32(AO_OK), spanMessage(err))
  }
}

// A transcript hook for the busy tests: `user` is a BusyBody, run on each show.
private final class BusyBody {
  let run: () -> Void
  init(_ run: @escaping () -> Void) {
    self.run = run
  }
}

private func busyBodyHook(
  _ utf8: UnsafePointer<CChar>?,
  _ len: Int32,
  _ isClear: Int32,
  _ user: UnsafeMutableRawPointer?
) {
  guard isClear == 0, let user else {
    return
  }
  Unmanaged<BusyBody>.fromOpaque(user).takeUnretainedValue().run()
}

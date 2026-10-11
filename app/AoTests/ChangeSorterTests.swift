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
}

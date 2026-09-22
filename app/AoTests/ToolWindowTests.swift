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

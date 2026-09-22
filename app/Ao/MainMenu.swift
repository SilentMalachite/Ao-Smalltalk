import AppKit

@MainActor
enum MainMenu {
  struct Actions {
    var doIt: () -> Void = {}
    var printIt: () -> Void = {}
    var inspectIt: () -> Void = {}
    var accept: () -> Void = {}
    var showBrowser: () -> Void = {}
    var showTranscript: () -> Void = {}
    var showWorkspace: () -> Void = {}
    var toggleFixedPitch: (NSMenuItem) -> Void = { _ in }
    var showHelp: () -> Void = {}
  }

  static func build(actions: Actions) -> NSMenu {
    let fixedPitch = actionItem("Use Fixed Pitch", key: "", run: actions.toggleFixedPitch)
    if UserDefaults.standard.bool(forKey: "AoTranscriptFixedPitch") {
      fixedPitch.state = .on
    }
    let bar = NSMenu(title: "MainMenu")
    bar.addItem(top("Ao", items: applicationItems()))
    bar.addItem(top("File", items: [
      responderItem("Close", key: "w", action: #selector(NSWindow.performClose(_:)))
    ]))
    bar.addItem(top("Edit", items: [
      responderItem("Cut", key: "x", action: #selector(NSText.cut(_:))),
      responderItem("Copy", key: "c", action: #selector(NSText.copy(_:))),
      responderItem("Paste", key: "v", action: #selector(NSText.paste(_:))),
      responderItem("Select All", key: "a", action: #selector(NSText.selectAll(_:)))
    ]))
    bar.addItem(top("Smalltalk", items: [
      actionItem("Do it", key: "d", run: { _ in actions.doIt() }),
      actionItem("Print it", key: "i", run: { _ in actions.printIt() }),
      actionItem("Inspect it", key: "", run: { _ in actions.inspectIt() }),
      actionItem("Accept", key: "", run: { _ in actions.accept() })
    ]))
    bar.addItem(top("Tools", items: [
      actionItem("Browser", key: "", run: { _ in actions.showBrowser() }),
      actionItem("Transcript", key: "", run: { _ in actions.showTranscript() }),
      actionItem("Workspace", key: "", run: { _ in actions.showWorkspace() }),
      .separator(),
      fixedPitch
    ]))
    bar.addItem(top("Window", items: [
      responderItem("Minimize", key: "m", action: #selector(NSWindow.performMiniaturize(_:))),
      responderItem("Zoom", key: "", action: #selector(NSWindow.performZoom(_:)))
    ]))
    bar.addItem(top("Help", items: [
      actionItem("About Ao", key: "", run: { _ in actions.showHelp() })
    ]))
    return bar
  }

  private static func applicationItems() -> [NSMenuItem] {
    let hideOthers = responderItem(
      "Hide Others",
      key: "h",
      action: #selector(NSApplication.hideOtherApplications(_:))
    )
    hideOthers.keyEquivalentModifierMask = [.command, .option]
    return [
      responderItem("Hide Ao", key: "h", action: #selector(NSApplication.hide(_:))),
      hideOthers,
      responderItem("Show All", key: "", action: #selector(NSApplication.unhideAllApplications(_:))),
      .separator(),
      responderItem("Quit Ao", key: "q", action: #selector(NSApplication.terminate(_:)))
    ]
  }

  private static func top(_ title: String, items: [NSMenuItem]) -> NSMenuItem {
    let menu = NSMenu(title: title)
    for item in items {
      menu.addItem(item)
    }
    let item = NSMenuItem(title: title, action: nil, keyEquivalent: "")
    item.submenu = menu
    return item
  }

  private static func responderItem(_ title: String, key: String, action: Selector) -> NSMenuItem {
    let item = NSMenuItem(title: title, action: action, keyEquivalent: key)
    item.target = nil
    if !key.isEmpty {
      item.keyEquivalentModifierMask = .command
    }
    return item
  }

  private static func actionItem(
    _ title: String,
    key: String,
    run: @escaping (NSMenuItem) -> Void
  ) -> NSMenuItem {
    let item = NSMenuItem(title: title, action: #selector(MenuAction.invoke(_:)), keyEquivalent: key)
    if !key.isEmpty {
      item.keyEquivalentModifierMask = .command
    }
    let target = MenuAction(run)
    item.target = target
    item.representedObject = target
    return item
  }
}

// Retained by NSMenuItem.representedObject. target itself is weak.
@MainActor
private final class MenuAction: NSObject {
  private let run: (NSMenuItem) -> Void

  init(_ run: @escaping (NSMenuItem) -> Void) {
    self.run = run
  }

  @objc func invoke(_ sender: NSMenuItem) {
    run(sender)
  }
}

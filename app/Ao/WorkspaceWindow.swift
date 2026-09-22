import AppKit

@MainActor
final class WorkspaceWindow {
  private let window: NSWindow

  var title: String {
    window.title
  }

  init() {
    window = makeToolTextWindow(
      title: "Workspace",
      frame: NSRect(x: 140, y: 80, width: 560, height: 360),
      editable: true
    ).window
    window.makeKeyAndOrderFront(nil)
  }
}

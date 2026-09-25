import AppKit

@MainActor
final class InspectorWindow {
  let window: NSWindow
  private let textView: NSTextView
  private var closeObserver: (any NSObjectProtocol)?

  static func lines(className: String, printString: String) -> String {
    className + "\n" + printString
  }

  var text: String {
    textView.string
  }

  // SPEC §3.9: `onClose` runs once, from inside `close`, so the owner can drop this Inspector.
  init(
    className: String,
    printString: String,
    onClose: @escaping @MainActor @Sendable (InspectorWindow) -> Void = { _ in }
  ) {
    let built = makeToolTextWindow(
      title: "Inspector",
      frame: NSRect(x: 220, y: 160, width: 360, height: 240),
      editable: false
    )
    window = built.window
    textView = built.textView
    textView.string = Self.lines(className: className, printString: printString)
    // queue nil: the center calls this synchronously, inside `close` on the main thread.
    closeObserver = NotificationCenter.default.addObserver(
      forName: NSWindow.willCloseNotification,
      object: window,
      queue: nil
    ) { [weak self] _ in
      MainActor.assumeIsolated {
        self?.windowWillClose(onClose)
      }
    }
    window.makeKeyAndOrderFront(nil)
  }

  private func windowWillClose(_ onClose: @MainActor (InspectorWindow) -> Void) {
    onClose(self)
    if let closeObserver {
      NotificationCenter.default.removeObserver(closeObserver)
    }
    closeObserver = nil
  }
}

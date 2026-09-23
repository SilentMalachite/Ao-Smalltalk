import AppKit

@MainActor
final class InspectorWindow {
  let window: NSWindow
  private let textView: NSTextView

  static func lines(className: String, printString: String) -> String {
    className + "\n" + printString
  }

  var text: String {
    textView.string
  }

  init(className: String, printString: String) {
    let built = makeToolTextWindow(
      title: "Inspector",
      frame: NSRect(x: 220, y: 160, width: 360, height: 240),
      editable: false
    )
    window = built.window
    textView = built.textView
    textView.string = Self.lines(className: className, printString: printString)
    window.makeKeyAndOrderFront(nil)
  }
}

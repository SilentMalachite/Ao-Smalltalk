import AppKit
import CAo

private let aoTranscriptFixedPitchKey = "AoTranscriptFixedPitch"

// C function pointer: no captures. `user` is the TranscriptWindow.
private func aoTranscriptHook(
  _ utf8: UnsafePointer<CChar>?,
  _ len: Int32,
  _ isClear: Int32,
  _ user: UnsafeMutableRawPointer?
) {
  guard let user else {
    return
  }
  let clear = isClear != 0
  let chunk: String
  if !clear, let utf8, len > 0 {
    let raw = UnsafeRawBufferPointer(start: utf8, count: Int(len))
    chunk = String(decoding: raw, as: UTF8.self)
  } else {
    chunk = ""
  }
  let bits = UInt(bitPattern: user)
  MainActor.assumeIsolated {
    guard let token = UnsafeMutableRawPointer(bitPattern: bits) else {
      return
    }
    let window = Unmanaged<TranscriptWindow>.fromOpaque(token).takeUnretainedValue()
    window.receiveHook(clear: clear, chunk: chunk)
  }
}

// Smalltalk source is typed as is: no smart quotes, dashes, text replacement, spelling
// correction or smart insert/delete. Edits are undoable.
@MainActor
func configureSourceEditing(_ textView: NSTextView) {
  textView.allowsUndo = true
  textView.isAutomaticQuoteSubstitutionEnabled = false
  textView.isAutomaticDashSubstitutionEnabled = false
  textView.isAutomaticTextReplacementEnabled = false
  textView.isAutomaticSpellingCorrectionEnabled = false
  textView.smartInsertDeleteEnabled = false
}

@MainActor
func makeToolTextWindow(title: String, frame: NSRect, editable: Bool) -> (window: NSWindow, textView: NSTextView) {
  _ = NSApplication.shared
  let window = NSWindow(
    contentRect: frame,
    styleMask: [.titled, .closable, .miniaturizable, .resizable],
    backing: .buffered,
    defer: false
  )
  window.title = title
  window.isReleasedWhenClosed = false
  window.isRestorable = false

  let scroll = NSScrollView(frame: NSRect(origin: .zero, size: frame.size))
  scroll.hasVerticalScroller = true
  scroll.hasHorizontalScroller = false
  scroll.autoresizingMask = [.width, .height]
  scroll.borderType = .noBorder

  let contentSize = scroll.contentSize
  let textView = NSTextView(frame: NSRect(origin: .zero, size: contentSize))
  textView.minSize = NSSize(width: 0, height: contentSize.height)
  textView.maxSize = NSSize(width: CGFloat.greatestFiniteMagnitude, height: CGFloat.greatestFiniteMagnitude)
  textView.isVerticallyResizable = true
  textView.isHorizontallyResizable = false
  textView.autoresizingMask = [.width]
  textView.isEditable = editable
  textView.isSelectable = true
  textView.isRichText = false
  configureSourceEditing(textView)
  if let container = textView.textContainer {
    container.containerSize = NSSize(width: contentSize.width, height: CGFloat.greatestFiniteMagnitude)
    container.widthTracksTextView = true
  }
  scroll.documentView = textView
  window.contentView = scroll
  return (window, textView)
}

@MainActor
final class TranscriptWindow {
  private let window: NSWindow
  private let textView: NSTextView
  private(set) var text = ""

  var title: String {
    window.title
  }

  var useFixedPitch: Bool {
    didSet { storeFixedPitch() }
  }

  var textAccessibilityLabel: String? {
    textView.accessibilityLabel()
  }

  init() {
    let saved = UserDefaults.standard.bool(forKey: aoTranscriptFixedPitchKey)
    let built = makeToolTextWindow(
      title: "Transcript",
      frame: NSRect(x: 80, y: 460, width: 560, height: 320),
      editable: false
    )
    window = built.window
    textView = built.textView
    textView.setAccessibilityLabel("Transcript")
    useFixedPitch = saved
    applyFont()
    window.makeKeyAndOrderFront(nil)
  }

  func append(_ chunk: String) {
    text += chunk
    showText()
  }

  func closeWindow() {
    window.close()
  }

  func orderFront() {
    window.makeKeyAndOrderFront(nil)
  }

  func installHook() {
    ao_set_transcript_hook(aoTranscriptHook, Unmanaged.passUnretained(self).toOpaque())
  }

  fileprivate func receiveHook(clear: Bool, chunk: String) {
    if clear {
      text = ""
      showText()
      return
    }
    if chunk.isEmpty {
      return
    }
    append(chunk)
  }

  private func showText() {
    textView.string = text
    applyFont()
    let end = (text as NSString).length
    textView.scrollRangeToVisible(NSRange(location: end, length: 0))
  }

  private func storeFixedPitch() {
    UserDefaults.standard.set(useFixedPitch, forKey: aoTranscriptFixedPitchKey)
    applyFont()
  }

  private func applyFont() {
    let font = useFixedPitch
      ? NSFont.userFixedPitchFont(ofSize: 0)
      : NSFont.userFont(ofSize: 0)
    guard let font else {
      return
    }
    textView.font = font
    textView.typingAttributes = [.font: font]
  }
}

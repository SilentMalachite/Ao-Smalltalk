import AppKit
import CAo

private let aoTranscriptFixedPitchKey = "AoTranscriptFixedPitch"

// C function pointer: no captures. `user` is the TranscriptWindow that set the hook; it is not
// dereferenced, since that window may be gone (SPEC §3.9 Transcript).
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
    guard let window = TranscriptWindow.hookWindow(at: bits) else {
      return
    }
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

/// SPEC §3.9 文字の大きさ: the Transcript, the Workspace and the Browser's source pane share one
/// text size, kept as the points added to the default size of their font.
@MainActor
enum ToolTextSize {
  private static let offsetKey = "AoTextSizeOffset"
  private static let offsets = -4...24

  // A value written outside the app is held to the range too.
  static var offset: Int {
    clamped(UserDefaults.standard.integer(forKey: offsetKey))
  }

  // A step past either end of the range leaves the offset at that end.
  static func step(by points: Int) {
    UserDefaults.standard.set(clamped(offset + points), forKey: offsetKey)
  }

  static func reset() {
    UserDefaults.standard.removeObject(forKey: offsetKey)
  }

  static func font(fixedPitch: Bool) -> NSFont? {
    let base = fixedPitch ? NSFont.userFixedPitchFont(ofSize: 0) : NSFont.userFont(ofSize: 0)
    guard let base, offset != 0 else {
      return base
    }
    let size = base.pointSize + CGFloat(offset)
    return fixedPitch ? NSFont.userFixedPitchFont(ofSize: size) : NSFont.userFont(ofSize: size)
  }

  private static func clamped(_ value: Int) -> Int {
    min(max(value, offsets.lowerBound), offsets.upperBound)
  }
}

/// SPEC §3.9 文字の大きさ: keeps one font over a plain text view. Setting the view's font changes
/// the text already there; characters that come in later with no font or an old one (Print it
/// into empty text, undo of an edit made at another size) get it as the storage processes them.
/// It is set before the storage fixes its attributes, so characters the font has no glyphs for
/// (Japanese in Helvetica) still get a face that has them, at the same size.
final class UniformFont: NSObject, NSTextStorageDelegate {
  private var font: NSFont?

  @MainActor
  func apply(_ font: NSFont, to textView: NSTextView) {
    self.font = font
    textView.textStorage?.delegate = self
    textView.font = font
  }

  func textStorage(
    _ textStorage: NSTextStorage,
    willProcessEditing editedMask: NSTextStorageEditActions,
    range editedRange: NSRange,
    changeInLength delta: Int
  ) {
    guard editedMask.contains(.editedCharacters), editedRange.length > 0, let font else {
      return
    }
    textStorage.addAttribute(.font, value: font, range: editedRange)
  }
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
  // SPEC §3.9 Transcript: the window that set the transcript hook last (the ABI has a setter
  // only). Weak, so a hook left after the window is gone shows nothing.
  private static weak var hookOwner: TranscriptWindow?
  private let window: NSWindow
  private let textView: NSTextView
  // Set by applyFont; every appended chunk carries them.
  private var appendAttributes: [NSAttributedString.Key: Any] = [:]
  private var scrollPending = false

  var text: String {
    textView.textStorage?.string ?? ""
  }

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
    guard let storage = textView.textStorage else {
      return
    }
    storage.append(NSAttributedString(string: chunk, attributes: appendAttributes))
    scheduleScrollToEnd()
  }

  func closeWindow() {
    window.close()
  }

  func orderFront() {
    window.makeKeyAndOrderFront(nil)
  }

  func installHook() {
    Self.hookOwner = self
    ao_set_transcript_hook(aoTranscriptHook, Unmanaged.passUnretained(self).toOpaque())
  }

  // SPEC §3.9 Transcript: takes the hook out only when this window set it last.
  func removeHook() {
    guard Self.hookOwner === self else {
      return
    }
    Self.hookOwner = nil
    ao_set_transcript_hook(nil, nil)
  }

  // The window the hook reaches: the one that set it last, while it lives and `user` is its
  // pointer.
  fileprivate static func hookWindow(at user: UInt) -> TranscriptWindow? {
    guard let owner = hookOwner else {
      return nil
    }
    return UInt(bitPattern: Unmanaged.passUnretained(owner).toOpaque()) == user ? owner : nil
  }

  fileprivate func receiveHook(clear: Bool, chunk: String) {
    if clear {
      if let storage = textView.textStorage {
        storage.replaceCharacters(in: NSRange(location: 0, length: storage.length), with: "")
      }
      scheduleScrollToEnd()
      return
    }
    if chunk.isEmpty {
      return
    }
    append(chunk)
  }

  // SPEC §3.9: one scroll to the end per burst, not one per chunk. The hook runs inside ao_eval on
  // the main thread, so the queued block runs when the main queue next runs: after the eval
  // returns, or earlier when the RunLoop pump (RunLoopPump) runs it mid-eval; then a later chunk
  // schedules one more. The window may be gone by then (a test's run loop can run it later); weak
  // self makes that harmless.
  private func scheduleScrollToEnd() {
    if scrollPending {
      return
    }
    scrollPending = true
    DispatchQueue.main.async { [weak self] in
      MainActor.assumeIsolated {
        guard let self else {
          return
        }
        self.scrollPending = false
        let end = self.textView.textStorage?.length ?? 0
        self.textView.scrollRangeToVisible(NSRange(location: end, length: 0))
      }
    }
  }

  private func storeFixedPitch() {
    UserDefaults.standard.set(useFixedPitch, forKey: aoTranscriptFixedPitchKey)
    applyFont()
  }

  // Also run when the text size changes (SPEC §3.9 文字の大きさ).
  func applyFont() {
    guard let font = ToolTextSize.font(fixedPitch: useFixedPitch) else {
      return
    }
    textView.font = font
    // SPEC §3.9: the system text color, so the text stays readable in Dark Mode.
    let attributes: [NSAttributedString.Key: Any] = [.font: font, .foregroundColor: NSColor.textColor]
    textView.typingAttributes = attributes
    appendAttributes = attributes
  }
}

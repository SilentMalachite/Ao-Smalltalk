import AppKit
import CAo

@MainActor
func saveImageFile(at url: URL) -> Int32 {
  url.path.withCString { ao_image_save($0) }
}

/// SPEC §3.10: the status ao_image_load answers, and on AO_ERR the reason it gives
/// ("unsupported image version 1", "not an Ao image", ...). The reason is empty on AO_OK.
@MainActor
func openImageFile(at url: URL, transcript: TranscriptWindow?) -> (status: Int32, reason: String) {
  var err = AoSpan()
  let status = url.path.withCString { path in
    withUnsafeMutablePointer(to: &err) { errPtr in
      ao_image_load(path, errPtr)
    }
  }
  if status == Int32(AO_OK) {
    transcript?.installHook()
  }
  return (status, spanMessage(err))
}

/// SPEC §3.9 起動と同梱: the vendor directory is AO_VENDOR_DIR when it is not empty, else
/// `vendor` in the bundle's resources. The current directory is never looked at.
func vendorDirectory(environment: [String: String], resources: URL) -> URL {
  if let dir = environment["AO_VENDOR_DIR"], !dir.isEmpty {
    return URL(fileURLWithPath: dir, isDirectory: true)
  }
  return resources.appendingPathComponent("vendor", isDirectory: true)
}

/// SPEC §3.9 起動と同梱: files in `<dir>/LOAD_ORDER` and writes one line to the Transcript and,
/// after `ao: `, to stderr, which is the only way to see it from outside an app started by `open`.
/// ao_filein_load_order answers only AO_OK or AO_ERR, so the line names the directory and no reason.
@MainActor
func fileInVendor(at dir: URL, transcript: TranscriptWindow) {
  fileInLoadOrder(at: dir, label: "vendor", transcript: transcript)
}

/// SPEC §3.9 起動と同梱 (P17): the tools-model directory is AO_TOOLS_MODEL_DIR when it is not
/// empty, else `Tools-model` in the bundle's resources.
func toolsModelDirectory(environment: [String: String], resources: URL) -> URL {
  if let dir = environment["AO_TOOLS_MODEL_DIR"], !dir.isEmpty {
    return URL(fileURLWithPath: dir, isDirectory: true)
  }
  return resources.appendingPathComponent("Tools-model", isDirectory: true)
}

/// SPEC §3.9 起動と同梱 (P17): files in the tools model (the Smalltalk ChangeSet) the same way,
/// with the lines `tools model not found: …`, `tools model loaded: …`, `tools model file-in failed: …`.
@MainActor
func fileInToolsModel(at dir: URL, transcript: TranscriptWindow) {
  fileInLoadOrder(at: dir, label: "tools model", transcript: transcript)
}

/// Files in `<dir>/LOAD_ORDER` and writes `<label> loaded: <dir>` (or `<label> not found: …`,
/// `<label> file-in failed: …`) to the Transcript and to stderr after `ao: `.
@MainActor
func fileInLoadOrder(at dir: URL, label: String, transcript: TranscriptWindow) {
  let order = dir.appendingPathComponent("LOAD_ORDER")
  var isDirectory = ObjCBool(false)
  let line: String
  if !FileManager.default.fileExists(atPath: order.path, isDirectory: &isDirectory) || isDirectory.boolValue {
    line = "\(label) not found: \(dir.path)"
  } else if order.path.withCString({ ao_filein_load_order($0) }) == Int32(AO_OK) {
    line = "\(label) loaded: \(dir.path)"
  } else {
    line = "\(label) file-in failed: \(dir.path)"
  }
  transcript.append(line + "\n")
  FileHandle.standardError.write(Data(("ao: " + line + "\n").utf8))
}

/// SPEC §3.9 File In… (P16): files in one .st file (ao_filein_file) and writes the result to the
/// Transcript: `filed in: <path>`, or `file-in failed: <path>` and the first error as
/// `<path>:<start>-<end>: <message>`, the line form of `ao filein`.
@MainActor
func fileInChangeFile(at url: URL, transcript: TranscriptWindow?) -> Int32 {
  var err = AoSpan()
  let status = url.path.withCString { ao_filein_file($0, &err) }
  if status == Int32(AO_OK) {
    transcript?.append("filed in: \(url.path)\n")
  } else {
    transcript?.append(
      "file-in failed: \(url.path)\n\(url.path):\(err.start)-\(err.end): \(spanMessage(err))\n")
  }
  return status
}

@MainActor
public final class AoApp: NSObject, NSApplicationDelegate {
  private var launch: LaunchSet?
  // SPEC §3.9 起動と同梱 (P17): where the tools model is filed in from, at launch and on Open Image….
  private var toolsModel = URL(fileURLWithPath: "/", isDirectory: true)
  private var browser: BrowserWindow?
  private(set) var changeSorter: ChangeSorterWindow?
  private static var retained: AoApp?

  public static func launch() {
    let app = NSApplication.shared
    let delegate = AoApp()
    retained = delegate
    app.delegate = delegate
    app.setActivationPolicy(.regular)
    app.run()
  }

  public func applicationWillFinishLaunching(_ notification: Notification) {
    let actions = MainMenu.Actions(
      doIt: {
        sendToKeyWorkspace(self.launch?.workspace, keyWindow: NSApplication.shared.keyWindow) { $0.doIt() }
      },
      printIt: {
        sendToKeyWorkspace(self.launch?.workspace, keyWindow: NSApplication.shared.keyWindow) { $0.printIt() }
      },
      inspectIt: {
        sendToKeyWorkspace(self.launch?.workspace, keyWindow: NSApplication.shared.keyWindow) { $0.inspectIt() }
      },
      debugIt: {
        sendToKeyWorkspace(self.launch?.workspace, keyWindow: NSApplication.shared.keyWindow) { $0.debugIt() }
      },
      interrupt: {
        _ = ao_request_interrupt()
      },
      canInterrupt: {
        EvaluationActivity.isActive
      },
      canEvaluate: {
        !EvaluationActivity.isActive
      },
      accept: {
        let key = NSApplication.shared.keyWindow
        if let debugger = DebuggerWindow.owning(key) {
          debugger.accept()
          return
        }
        sendToKeyBrowser(self.browser, keyWindow: key) { $0.accept() }
      },
      canAccept: {
        !EvaluationActivity.isActive
          && DebuggerWindow.allowsAccept(keyWindow: NSApplication.shared.keyWindow)
      },
      removeMethod: {
        sendToKeyBrowser(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.removeMethod() }
      },
      removeClass: {
        sendToKeyBrowser(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.removeClass() }
      },
      canRemoveMethod: {
        keyBrowserAllows(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.canRemoveMethod }
      },
      canRemoveClass: {
        keyBrowserAllows(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.canRemoveClass }
      },
      showHierarchy: {
        if self.browser == nil {
          self.showBrowser()
        }
        self.browser?.showHierarchy()
      },
      saveImage: { self.presentSaveImage() },
      openImage: { self.presentOpenImage() },
      fileIn: { self.presentFileIn() },
      fileOutChangeSet: { self.presentFileOutChangeSet() },
      showChangeSorter: { self.showChangeSorter() },
      showBrowser: { self.showBrowser() },
      showTranscript: { self.launch?.transcript.orderFront() },
      showWorkspace: { self.launch?.workspace.orderFront() },
      toggleFixedPitch: { item in self.toggleFixedPitch(item) },
      makeTextBigger: { self.resizeText { ToolTextSize.step(by: 1) } },
      makeTextSmaller: { self.resizeText { ToolTextSize.step(by: -1) } },
      showActualSize: { self.resizeText { ToolTextSize.reset() } },
      showHelp: { self.showVersion() }
    )
    let menu = MainMenu.build(actions: actions)
    let app = (notification.object as? NSApplication) ?? NSApplication.shared
    app.mainMenu = menu
    if let windowMenu = menu.item(withTitle: "Window")?.submenu {
      app.windowsMenu = windowMenu
    }
    let started = LaunchSet.make()
    launch = started
    // A bundle has resources; bundleURL stands in only so the line still names a directory.
    let resources = Bundle.main.resourceURL ?? Bundle.main.bundleURL
    fileInVendor(
      at: vendorDirectory(environment: ProcessInfo.processInfo.environment, resources: resources),
      transcript: started.transcript
    )
    toolsModel = toolsModelDirectory(environment: ProcessInfo.processInfo.environment, resources: resources)
    fileInToolsModel(at: toolsModel, transcript: started.transcript)
    app.activate()
  }

  // Shows the alert for a failed Save or Open Image. Tests replace it.
  var presentAlert: @MainActor (NSAlert) -> Void = { alert in
    _ = alert.runModal()
  }

  // The ABI answers only AO_OK or AO_ERR, so the alert names the file and no reason, but for the
  // halted processes a live Debugger holds (SPEC §3.9, §3.11): then it says so.
  func saveImage(to url: URL) -> Bool {
    guard saveImageFile(at: url) == Int32(AO_OK) else {
      let reason = ao_debug_halted_count() > 0 ? "halted processes" : ""
      reportImageFailure("Could not save the image", url: url, reason: reason)
      return false
    }
    return true
  }

  func openImage(from url: URL) -> Bool {
    let opened = openImageFile(at: url, transcript: launch?.transcript)
    guard opened.status == Int32(AO_OK) else {
      reportImageFailure("Could not open the image", url: url, reason: opened.reason)
      return false
    }
    // SPEC §3.9 起動と同梱 (P17): an image saved without the change set model gets it now.
    if "ChangeSet".withCString({ ao_browser_class_id($0) }) == 0, let transcript = launch?.transcript {
      fileInToolsModel(at: toolsModel, transcript: transcript)
    }
    browser?.noteImageLoaded()
    changeSorter?.noteImageLoaded()
    return true
  }

  private func reportImageFailure(_ message: String, url: URL, reason: String = "") {
    let alert = NSAlert()
    alert.alertStyle = .warning
    alert.messageText = message
    alert.informativeText = reason.isEmpty ? url.path : url.path + "\n" + reason
    presentAlert(alert)
  }

  private func presentSaveImage() {
    let panel = NSSavePanel()
    panel.canCreateDirectories = true
    panel.begin { response in
      MainActor.assumeIsolated {
        guard response == .OK, let url = panel.url else {
          return
        }
        _ = self.saveImage(to: url)
      }
    }
  }

  private func presentOpenImage() {
    let panel = NSOpenPanel()
    panel.canChooseFiles = true
    panel.canChooseDirectories = false
    panel.allowsMultipleSelection = false
    panel.begin { response in
      MainActor.assumeIsolated {
        guard response == .OK, let url = panel.url else {
          return
        }
        _ = self.openImage(from: url)
      }
    }
  }

  // SPEC §3.9 File In…: the Transcript gets the result; an open Change Sorter reads the records.
  func fileIn(from url: URL) {
    _ = fileInChangeFile(at: url, transcript: launch?.transcript)
    changeSorter?.reload()
  }

  // SPEC §3.9 File Out Change Set…: the current set. A failure is an alert naming the file and the
  // reason.
  func fileOutCurrentChangeSet(to url: URL) -> Bool {
    let result = fileOutChangeSet(ao_changes_current(), to: url)
    guard result.status == Int32(AO_OK) else {
      reportImageFailure("Could not file out the change set", url: url, reason: result.message)
      return false
    }
    return true
  }

  private func presentFileIn() {
    let panel = NSOpenPanel()
    panel.canChooseFiles = true
    panel.canChooseDirectories = false
    panel.allowsMultipleSelection = false
    panel.begin { response in
      MainActor.assumeIsolated {
        guard response == .OK, let url = panel.url else {
          return
        }
        self.fileIn(from: url)
      }
    }
  }

  private func presentFileOutChangeSet() {
    let model = ChangeSorterModel()
    model.refresh()
    let panel = NSSavePanel()
    panel.canCreateDirectories = true
    panel.nameFieldStringValue = (model.currentName ?? "Unnamed1") + ".st"
    panel.begin { response in
      MainActor.assumeIsolated {
        guard response == .OK, let url = panel.url else {
          return
        }
        _ = self.fileOutCurrentChangeSet(to: url)
      }
    }
  }

  private func showChangeSorter() {
    if changeSorter == nil {
      changeSorter = ChangeSorterWindow()
    }
    changeSorter?.reload()
    changeSorter?.window.makeKeyAndOrderFront(nil)
  }

  private func showBrowser() {
    if browser == nil {
      browser = BrowserWindow()
    }
    browser?.window.makeKeyAndOrderFront(nil)
  }

  private func toggleFixedPitch(_ item: NSMenuItem) {
    guard let transcript = launch?.transcript else {
      return
    }
    transcript.useFixedPitch.toggle()
    item.state = transcript.useFixedPitch ? .on : .off
  }

  // SPEC §3.9 文字の大きさ: the open Transcript, Workspace, Browser and Debuggers (held weakly by
  // DebuggerWindow.open) take the new size at once.
  private func resizeText(_ change: () -> Void) {
    change()
    launch?.transcript.applyFont()
    launch?.workspace.applyFont()
    browser?.applyFont()
    changeSorter?.applyFont()
    for debugger in DebuggerWindow.open {
      debugger.applyFont()
    }
  }

  private func showVersion() {
    var bytes = [CChar](repeating: 0, count: 64)
    let status = ao_version(&bytes, Int32(bytes.count))
    let version = bytes.withUnsafeBufferPointer { pointer -> String in
      guard status == Int32(AO_OK), let base = pointer.baseAddress else {
        return ""
      }
      return String(cString: base)
    }
    let alert = NSAlert()
    alert.messageText = version
    alert.runModal()
  }
}

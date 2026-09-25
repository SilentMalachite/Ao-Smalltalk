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
  let order = dir.appendingPathComponent("LOAD_ORDER")
  var isDirectory = ObjCBool(false)
  let line: String
  if !FileManager.default.fileExists(atPath: order.path, isDirectory: &isDirectory) || isDirectory.boolValue {
    line = "vendor not found: \(dir.path)"
  } else if order.path.withCString({ ao_filein_load_order($0) }) == Int32(AO_OK) {
    line = "vendor loaded: \(dir.path)"
  } else {
    line = "vendor file-in failed: \(dir.path)"
  }
  transcript.append(line + "\n")
  FileHandle.standardError.write(Data(("ao: " + line + "\n").utf8))
}

@MainActor
public final class AoApp: NSObject, NSApplicationDelegate {
  private var launch: LaunchSet?
  private var browser: BrowserWindow?
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
      accept: {
        sendToKeyBrowser(self.browser, keyWindow: NSApplication.shared.keyWindow) { $0.accept() }
      },
      showHierarchy: {
        if self.browser == nil {
          self.showBrowser()
        }
        self.browser?.showHierarchy()
      },
      saveImage: { self.presentSaveImage() },
      openImage: { self.presentOpenImage() },
      showBrowser: { self.showBrowser() },
      showTranscript: { self.launch?.transcript.orderFront() },
      showWorkspace: { self.launch?.workspace.orderFront() },
      toggleFixedPitch: { item in self.toggleFixedPitch(item) },
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
    app.activate()
  }

  // Shows the alert for a failed Save or Open Image. Tests replace it.
  var presentAlert: @MainActor (NSAlert) -> Void = { alert in
    _ = alert.runModal()
  }

  // The ABI answers only AO_OK or AO_ERR, so the alert names the file and no reason.
  func saveImage(to url: URL) -> Bool {
    guard saveImageFile(at: url) == Int32(AO_OK) else {
      reportImageFailure("Could not save the image", url: url)
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
    browser?.noteImageLoaded()
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

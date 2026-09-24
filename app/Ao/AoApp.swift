import AppKit
import CAo

@MainActor
func saveImageFile(at url: URL) -> Int32 {
  url.path.withCString { ao_image_save($0) }
}

@MainActor
func openImageFile(at url: URL, transcript: TranscriptWindow?) -> Int32 {
  let status = url.path.withCString { ao_image_load($0) }
  if status == Int32(AO_OK) {
    transcript?.installHook()
  }
  return status
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
    fileInVendorIfPresent(started)
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
    guard openImageFile(at: url, transcript: launch?.transcript) == Int32(AO_OK) else {
      reportImageFailure("Could not open the image", url: url)
      return false
    }
    browser?.noteImageLoaded()
    return true
  }

  private func reportImageFailure(_ message: String, url: URL) {
    let alert = NSAlert()
    alert.alertStyle = .warning
    alert.messageText = message
    alert.informativeText = url.path
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

  private func fileInVendorIfPresent(_ started: LaunchSet) {
    let relative = "image/vendor/LOAD_ORDER"
    var isDirectory = ObjCBool(false)
    let exists = FileManager.default.fileExists(atPath: relative, isDirectory: &isDirectory)
    guard exists, !isDirectory.boolValue else {
      return
    }
    let status = relative.withCString { ao_filein_load_order($0) }
    if status != Int32(AO_OK) {
      // ABI returns only AO_OK or AO_ERR, so the transcript gets a fixed line.
      started.transcript.append("file-in failed\n")
    }
  }
}

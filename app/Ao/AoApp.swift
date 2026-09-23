import AppKit
import CAo

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

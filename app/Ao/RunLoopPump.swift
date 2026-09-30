import CoreFoundation
import CAo

/// SPEC §3.9 評価の中断: Ao.app registers a pump so ⌘. can arrive during a synchronous `ao_eval`.
/// Same shape as the transcript hook — a free C function, not a Swift closure.
enum RunLoopPump {
  static func install() {
    ao_set_runloop_pump_hook(aoRunLoopPumpHook, nil)
  }

  static func remove() {
    ao_set_runloop_pump_hook(nil, nil)
  }
}

private func aoRunLoopPumpHook(_ user: UnsafeMutableRawPointer?) {
  _ = user
  _ = CFRunLoopRunInMode(CFRunLoopMode.defaultMode, 0, true)
}

/// SPEC §3.9: Smalltalk eval menus grey out while the main thread is inside `ao_eval` or a live
/// Proceed / Step. Interrupt is enabled only then.
@MainActor
enum EvaluationActivity {
  private(set) static var isActive = false

  static func whileActive<T>(_ body: () -> T) -> T {
    isActive = true
    defer { isActive = false }
    return body()
  }
}

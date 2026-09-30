import AppKit
import CAo

/// SPEC §3.9 評価の中断: Ao.app registers a pump so ⌘. can arrive during a synchronous `ao_eval`.
/// Same shape as the transcript hook — a free C function, not a Swift closure.
@MainActor
enum RunLoopPump {
  /// At most this many queued events per pump, so a burst of input cannot starve the evaluation.
  static let maxEventsPerPump = 64
  private static var pumping = false

  // Only the ABI setter: callable from LaunchSet's nonisolated deinit as well.
  nonisolated static func install() {
    ao_set_runloop_pump_hook(aoRunLoopPumpHook, nil)
  }

  nonisolated static func remove() {
    ao_set_runloop_pump_hook(nil, nil)
  }

  /// Takes queued window-server events and hands them to NSApplication, as its own loop would.
  /// Running the CFRunLoop alone only queues them, so ⌘. and menu clicks never reached the menu.
  static func pump() {
    // An event's action never runs the interpreter (busy), but a nested pump must not start anyway.
    if pumping {
      return
    }
    pumping = true
    defer { pumping = false }
    let app = NSApplication.shared
    for _ in 0..<maxEventsPerPump {
      guard
        let event = app.nextEvent(
          matching: .any, until: .distantPast, inMode: .default, dequeue: true)
      else {
        return
      }
      app.sendEvent(event)
    }
  }
}

private func aoRunLoopPumpHook(_ user: UnsafeMutableRawPointer?) {
  _ = user
  // The runtime calls the pump from its safepoints, inside ao_eval on the main thread (SPEC §3.2).
  MainActor.assumeIsolated {
    RunLoopPump.pump()
  }
}

/// SPEC §3.9: Smalltalk eval menus grey out while the main thread is inside `ao_eval` or a live
/// Proceed / Step. Interrupt is enabled only then. Nesting counts, so an inner Proceed / Step that
/// the pump dispatches does not end the outer evaluation's state.
@MainActor
enum EvaluationActivity {
  private static var depth = 0

  static var isActive: Bool { depth > 0 }

  static func whileActive<T>(_ body: () -> T) -> T {
    depth += 1
    defer { depth -= 1 }
    return body()
  }
}

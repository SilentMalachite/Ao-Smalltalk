import AppKit
import CAo

@MainActor
final class LaunchSet {
  let transcript: TranscriptWindow
  let workspace: WorkspaceWindow

  private init(transcript: TranscriptWindow, workspace: WorkspaceWindow) {
    self.transcript = transcript
    self.workspace = workspace
  }

  static func make() -> LaunchSet {
    _ = NSApplication.shared
    // SPEC §3.10: the transcript hook reaches this session and every later one (boot, load).
    _ = ao_runtime_boot()
    // SPEC §3.9: the app captures failing stacks for the Debugger. The setting outlives boot and
    // load, so it is set once here and taken back in deinit, like the transcript hook.
    ao_set_debug_capture(1)
    // SPEC §3.9, §3.13: evaluations halt for the live Debugger instead of unwinding.
    ao_set_debug_mode(Int32(AO_DEBUG_LIVE))
    let transcript = TranscriptWindow()
    let workspace = WorkspaceWindow()
    transcript.installHook()
    // SPEC §3.9 評価の中断: pump so ⌘. reaches the menu during a synchronous ao_eval.
    RunLoopPump.install()
    return LaunchSet(transcript: transcript, workspace: workspace)
  }

  deinit {
    ao_set_transcript_hook(nil, nil)
    RunLoopPump.remove()
    ao_set_debug_capture(0)
    ao_set_debug_mode(Int32(AO_DEBUG_POSTMORTEM))
  }
}

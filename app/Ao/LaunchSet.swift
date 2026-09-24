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
    let transcript = TranscriptWindow()
    let workspace = WorkspaceWindow()
    transcript.installHook()
    return LaunchSet(transcript: transcript, workspace: workspace)
  }

  deinit {
    ao_set_transcript_hook(nil, nil)
  }
}

import XCTest
@testable import Ao

@MainActor
final class ToolWindowTests: XCTestCase {
  override func tearDown() {
    UserDefaults.standard.removeObject(forKey: "AoTranscriptFixedPitch")
    ao_runtime_shutdown()
    super.tearDown()
  }

  func testTranscriptAppendsAndSurvivesClose() {
    let launch = LaunchSet.make()
    XCTAssertEqual(launch.transcript.title, "Transcript")
    XCTAssertEqual(launch.workspace.title, "Workspace")
    launch.transcript.append("hello")
    launch.transcript.append("\n")
    launch.transcript.closeWindow()
    XCTAssertEqual(launch.transcript.text, "hello\n")
    launch.transcript.useFixedPitch = true
    XCTAssertEqual(launch.transcript.useFixedPitch, true)
  }
}

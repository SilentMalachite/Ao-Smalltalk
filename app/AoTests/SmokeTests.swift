import XCTest
@testable import Ao

final class SmokeTests: XCTestCase {
  @MainActor
  func testBrowserModelExists() {
    _ = BrowserModel()
  }
}

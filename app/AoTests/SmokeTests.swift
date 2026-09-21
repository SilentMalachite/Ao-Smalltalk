import XCTest
@testable import Ao

final class SmokeTests: XCTestCase {
  func testPlaceholderTypeExists() {
    XCTAssertEqual(AoPlaceholder.phase, "P0")
  }
}

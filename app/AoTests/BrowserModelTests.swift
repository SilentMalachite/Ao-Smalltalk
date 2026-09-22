import XCTest
@testable import Ao

@MainActor
final class BrowserModelTests: XCTestCase {
  override func tearDown() {
    ao_runtime_shutdown()
    super.tearDown()
  }

  func testObjectListsPrintStringAsNative() {
    let model = BrowserModel()
    XCTAssertEqual(model.boot(), 0)
    model.refresh()
    let cats = model.categories
    XCTAssertTrue(cats.contains("Kernel"))
    model.select(category: "Kernel", className: "Object", meta: false, protocol: "native")
    XCTAssertTrue(model.selectors.contains("printString"))
    XCTAssertTrue(model.source.contains("subclass: #Object"))
    model.select(
      category: "Kernel",
      className: "Object",
      meta: false,
      protocol: "native",
      selector: "printString"
    )
    XCTAssertTrue(model.source.contains("ao_Object_printString"))
    ao_runtime_shutdown()
  }
}

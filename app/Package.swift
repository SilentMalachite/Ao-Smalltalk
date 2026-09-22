// swift-tools-version: 6.0
import PackageDescription

let package = Package(
  name: "Ao",
  platforms: [.macOS(.v14)],
  products: [
    .library(name: "Ao", targets: ["Ao"]),
    .executable(name: "AoApp", targets: ["AoApp"])
  ],
  targets: [
    .target(name: "Ao", dependencies: ["CAo"], path: "Ao"),
    .executableTarget(name: "AoApp", dependencies: ["Ao"], path: "AoApp"),
    .systemLibrary(name: "CAo", path: "CAo"),
    .testTarget(name: "AoTests", dependencies: ["Ao"], path: "AoTests")
  ]
)

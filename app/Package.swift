// swift-tools-version: 6.0
import PackageDescription

let package = Package(
  name: "Ao",
  platforms: [.macOS(.v14)],
  products: [
    .library(name: "Ao", targets: ["Ao"])
  ],
  targets: [
    .target(name: "Ao", path: "Ao"),
    .testTarget(name: "AoTests", dependencies: ["Ao"], path: "AoTests")
  ]
)

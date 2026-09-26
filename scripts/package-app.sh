#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build-release"
"$ROOT/scripts/build.sh" --release
set -- --package-path "$ROOT/app" -c release \
  -Xlinker -force_load -Xlinker "$BUILD/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$BUILD/compiler/libao_compiler.a" \
  -Xlinker -lc++
swift build "$@"
BIN_DIR="$(swift build "$@" --show-bin-path)"
VERSION="$("$BUILD/ao" --version)"
VERSION="${VERSION%%-*}"
APP="$ROOT/build/Ao.app"
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
cp "$BIN_DIR/AoApp" "$APP/Contents/MacOS/Ao"
chmod +x "$APP/Contents/MacOS/Ao"
cp -R "$ROOT/image/vendor" "$APP/Contents/Resources/vendor"
find "$APP/Contents/Resources/vendor" -name .DS_Store -exec rm -f {} +
cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleExecutable</key>
  <string>Ao</string>
  <key>CFBundleIdentifier</key>
  <string>jp.ao.smalltalk</string>
  <key>CFBundleName</key>
  <string>Ao</string>
  <key>CFBundlePackageType</key>
  <string>APPL</string>
  <key>CFBundleShortVersionString</key>
  <string>$VERSION</string>
  <key>CFBundleVersion</key>
  <string>$VERSION</string>
  <key>LSMinimumSystemVersion</key>
  <string>14.0</string>
  <key>NSHighResolutionCapable</key>
  <true/>
  <key>NSPrincipalClass</key>
  <string>NSApplication</string>
</dict>
</plist>
EOF
codesign --force --sign - "$APP"

#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
swift build --package-path "$ROOT/app" \
  -Xlinker -force_load -Xlinker "$ROOT/build/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$ROOT/build/compiler/libao_compiler.a" \
  -Xlinker -lc++
BIN_DIR="$(swift build --package-path "$ROOT/app" --show-bin-path \
  -Xlinker -force_load -Xlinker "$ROOT/build/runtime/libao_runtime.a" \
  -Xlinker -force_load -Xlinker "$ROOT/build/compiler/libao_compiler.a" \
  -Xlinker -lc++)"
APP="$ROOT/build/Ao.app"
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS"
cp "$BIN_DIR/AoApp" "$APP/Contents/MacOS/Ao"
chmod +x "$APP/Contents/MacOS/Ao"
cat > "$APP/Contents/Info.plist" <<'EOF'
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
  <key>LSMinimumSystemVersion</key>
  <string>14.0</string>
  <key>NSHighResolutionCapable</key>
  <true/>
  <key>NSPrincipalClass</key>
  <string>NSApplication</string>
</dict>
</plist>
EOF

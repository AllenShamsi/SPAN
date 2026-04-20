#!/usr/bin/env bash
set -euo pipefail

# === Adjust if needed ===
MIN_OS="${MIN_OS:-12.0}"          # Set to testing Mac's major version or older (e.g., 12.0 for Monterey+)
ARCHS="${ARCHS:-arm64}"            # arm64 for Apple silicon. (If you KNOW both Macs are Intel: x86_64)
GENERATOR="${GENERATOR:-Ninja}"    # Use "Unix Makefiles" if you don't have Ninja
APP_NAME="${APP_NAME:-SPAN}"       # Your target/app name

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DEV="$ROOT/build-dev"
BUILD_DIST="$ROOT/build-dist"
APP_DEV="$BUILD_DEV/${APP_NAME}.app"
APP_DIST="$BUILD_DIST/${APP_NAME}.app"

echo "== Cleaning build dirs =="
rm -rf "$BUILD_DEV" "$BUILD_DIST"
mkdir -p "$BUILD_DEV" "$BUILD_DIST"

echo "== Finding cmake/qt-cmake =="
if command -v qt-cmake >/dev/null 2>&1; then
  CMAKE=qt-cmake
else
  CMAKE=cmake
  # Try to locate Qt if not using qt-cmake
  if ! ${CMAKE} --version >/dev/null 2>&1; then
    echo "cmake not found"; exit 1
  fi
  # If Qt via Homebrew, set CMAKE_PREFIX_PATH
  if command -v brew >/dev/null 2>&1; then
    export CMAKE_PREFIX_PATH="$(brew --prefix qt)/lib/cmake"
  fi
fi

echo "== Configuring build =="
if [ "$GENERATOR" = "Ninja" ] && ! command -v ninja >/dev/null 2>&1; then
  echo "Ninja not found; switching to Unix Makefiles"
  GENERATOR="Unix Makefiles"
fi

${CMAKE} -S "$ROOT" -B "$BUILD_DEV" -G "$GENERATOR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="$MIN_OS" \
  -DCMAKE_OSX_ARCHITECTURES="$ARCHS"

echo "== Building =="
cmake --build "$BUILD_DEV" -j

# Sanity check
if [ ! -d "$APP_DEV" ]; then
  echo "Error: ${APP_DEV} not found after build"; exit 1
fi

echo "== Copying app to dist =="
rsync -a "$APP_DEV" "$BUILD_DIST/"

echo "== Locating macdeployqt =="
if command -v macdeployqt >/dev/null 2>&1; then
  MACDEPLOYQT="$(command -v macdeployqt)"
elif [ -x "$HOME/Qt/6.7.1/macos/bin/macdeployqt" ]; then
  MACDEPLOYQT="$HOME/Qt/6.7.1/macos/bin/macdeployqt"
else
  echo "macdeployqt not found on PATH; install Qt tools or add to PATH"; exit 1
fi
echo "Using macdeployqt at: $MACDEPLOYQT"

echo "== Deploying Qt frameworks & plugins =="
"$MACDEPLOYQT" "$APP_DIST" -verbose=2 -always-overwrite

# Bundle non-Qt dylibs (PortAudio) and rewrite loader paths
echo "== Bundling non-Qt deps (PortAudio if present) =="
EXE="$APP_DIST/Contents/MacOS/${APP_NAME}"
FWK="$APP_DIST/Contents/Frameworks"
mkdir -p "$FWK"

# Show external Homebrew/Local deps before patching
echo "== External deps BEFORE patch =="
(otool -L "$EXE" | egrep '(/opt/homebrew|/usr/local)' ) || true

if command -v brew >/dev/null 2>&1; then
  PA_PREFIX="$(brew --prefix portaudio 2>/dev/null || true)"
  if [ -n "${PA_PREFIX}" ] && [ -f "$PA_PREFIX/lib/libportaudio.2.dylib" ]; then
    echo "Bundling PortAudio from $PA_PREFIX"
    cp -a "$PA_PREFIX/lib/libportaudio.2.dylib" "$FWK/"
    install_name_tool -id "@rpath/libportaudio.2.dylib" "$FWK/libportaudio.2.dylib"
    install_name_tool -change "$PA_PREFIX/lib/libportaudio.2.dylib" "@rpath/libportaudio.2.dylib" "$EXE"
    # Ensure rpath into Frameworks
    if ! otool -l "$EXE" | grep -A2 LC_RPATH | grep -q "@executable_path/../Frameworks"; then
      install_name_tool -add_rpath "@executable_path/../Frameworks" "$EXE"
    fi
  else
    echo "PortAudio via Homebrew not found; skipping."
  fi
fi

echo "== External deps AFTER patch =="
(otool -L "$EXE" | egrep '(/opt/homebrew|/usr/local)') || echo "OK: no external Homebrew paths in main executable."

echo "== Signing (ad-hoc) =="
codesign --force --deep --sign - "$APP_DIST" || true
codesign -vv "$APP_DIST" || true

echo "== Clearing quarantine locally (prevents translocation) =="
xattr -dr com.apple.quarantine "$APP_DIST" || true

echo "== Verifications =="
echo "Architecture of main binary:"; file "$EXE"
echo "Qt platform plugin present:"; ls "$APP_DIST/Contents/PlugIns/platforms" || true

echo "== Creating zip (ditto) =="
cd "$BUILD_DIST"
ditto -c -k --sequesterRsrc --keepParent "${APP_NAME}.app" "../${APP_NAME}-macOS.zip"

echo "== DONE =="
echo "Created: $BUILD_DIST/../${APP_NAME}-macOS.zip"


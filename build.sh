#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
BUILD_TYPE="${1:-Release}"   # pass Debug as first arg to override
JOBS="${JOBS:-$(nproc)}"

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j"$JOBS"

# Keep compile_commands.json symlink up to date for clangd
ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json

echo ""
echo "Build complete: $BUILD_DIR/SlopAudioConnect_artefacts/$BUILD_TYPE/SlopAudio Connect"

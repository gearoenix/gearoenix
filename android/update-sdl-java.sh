#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DEST_DIR="$SCRIPT_DIR/app/src/main/java/org/libsdl/app"

if [ -z "${VCPKG_ROOT:-}" ]; then
    echo "Error: VCPKG_ROOT is not set"
    exit 1
fi

SDL_SRC=$(find "$VCPKG_ROOT/buildtrees/sdl3/src" -maxdepth 1 -type d -name "*.clean" | sort -V | tail -1)

if [ -z "$SDL_SRC" ]; then
    echo "Error: No SDL3 source found in $VCPKG_ROOT/buildtrees/sdl3/src/"
    echo "Run vcpkg install sdl3 first to download the source."
    exit 1
fi

SDL_JAVA_DIR="$SDL_SRC/android-project/app/src/main/java/org/libsdl/app"

if [ ! -d "$SDL_JAVA_DIR" ]; then
    echo "Error: SDL3 Java files not found at $SDL_JAVA_DIR"
    exit 1
fi

echo "Source: $SDL_JAVA_DIR"
echo "Destination: $DEST_DIR"

rm -rf "$DEST_DIR"
mkdir -p "$DEST_DIR"
cp "$SDL_JAVA_DIR"/*.java "$DEST_DIR/"

echo "Copied $(ls -1 "$DEST_DIR" | wc -l | tr -d ' ') files:"
ls -1 "$DEST_DIR"
#!/usr/bin/env bash

set -Eeuo pipefail

BUILD_TYPE="release"
NO_SERVER="false"

usage() {
  cat <<'USAGE'
Usage: build.sh [--build-type debug|release] [--no-server] [--help]

Options:
  --build-type <debug|release>  CMake preset suffix to use (default: release)
  --no-server                   Do not run the local HTTP server after build
  --help                        Show this help
USAGE
}

while [[ $# -gt 0 ]]
do
  case "$1" in
    --build-type)
      [[ $# -ge 2 ]] ||
      {
        echo "error: --build-type requires a value" >&2
        usage
        exit 2
      }
      case "$2" in
        debug|release)
          BUILD_TYPE="$2"
          ;;
        *)
          echo "error: invalid --build-type '$2' (expected debug|release)" >&2
          exit 2
          ;;
      esac
      shift 2
      ;;
    --no-server)
      NO_SERVER="true"
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "error: unknown argument: $1" >&2
      usage
      exit 2
      ;;
  esac
done

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
PROJECT_ROOT="$(realpath "${SCRIPT_DIR}/..")"

require_cmd() {
  local cmd="$1"
  command -v "$cmd" >/dev/null 2>&1 || {
    echo "error: required command not found in PATH: $cmd" >&2
    exit 1
  }
}

require_cmd cmake
require_cmd ninja
# Prefer python3 on Linux in 2025
if command -v python3 >/dev/null 2>&1
then
  PYTHON="python3"
elif command -v python >/dev/null 2>&1
then
  PYTHON="python"
else
  echo "error: python3 (or python) is required but not found in PATH" >&2
  exit 1
fi

: "${VCPKG_ROOT:?VCPKG_ROOT environment variable is not set. Please install vcpkg and set VCPKG_ROOT.}"
: "${EMSDK:?EMSDK environment variable is not set. Please install Emscripten and set EMSDK.}"

EMSDK_SCRIPT="${EMSDK%/}/emsdk_env.sh"
if [[ ! -f "$EMSDK_SCRIPT" ]]
then
  echo "error: emsdk_env.sh not found at: $EMSDK_SCRIPT" >&2
  exit 1
fi

# shellcheck disable=SC1090
source "$EMSDK_SCRIPT"
require_cmd emcmake

pushd "$PROJECT_ROOT" >/dev/null
trap 'popd >/dev/null || true' EXIT

if ! cmake --fresh --preset "wasm-${BUILD_TYPE}" .
then
  echo "error: Failed to configure the project" >&2
  exit 1
fi

NPROC=$(( $(nproc --all 2>/dev/null || getconf _NPROCESSORS_ONLN || echo 4) ))
THREADS=$(( (NPROC * 3 + 1) / 2 ))
if (( THREADS < 8 ))
then
  THREADS=8
fi

echo "Threads count: $THREADS"

if ! cmake --build --preset "wasm-${BUILD_TYPE}" --parallel "$THREADS"
then
  echo "error: Failed to build the project" >&2
  exit 1
fi

if [[ "$NO_SERVER" != "true" ]]
then
  # Run from the script directory so relative paths (like logo) resolve
  pushd "$PROJECT_ROOT" >/dev/null
  trap 'popd >/dev/null || true' RETURN
  if ! "$PYTHON" "wasm/run-server.py"
  then
    echo "error: Failed to run the server" >&2
    exit 1
  fi
fi

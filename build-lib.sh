#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=${BUILD_DIR:-build}
ACTION=${1:-all}

usage() {
  cat <<EOF
Usage: $0 [clean|configure|build|lib|tests|all]

Commands:
  clean      Remove the build output directory.
  configure  Run CMake configure.
  build      Build the app and library.
  lib        Build only the static library target.
  tests      Build the test target.
  all        Clean, configure, and build the library.
EOF
}

case "$ACTION" in
  clean)
    rm -rf "$BUILD_DIR"
    ;;
  configure)
    cmake -S . -B "$BUILD_DIR"
    ;;
  build)
    cmake --build "$BUILD_DIR" --target app
    ;;
  lib)
    cmake --build "$BUILD_DIR" --target parliamentary_scheduler
    ;;
  tests)
    cmake --build "$BUILD_DIR" --target test_suite
    ;;
  all)
    rm -rf "$BUILD_DIR"
    cmake -S . -B "$BUILD_DIR"
    cmake --build "$BUILD_DIR" --target parliamentary_scheduler
    ;;
  *)
    usage
    exit 1
    ;;
esac

#!/usr/bin/env bash
set -euo pipefail

BUILDDIR="${BUILDDIR:-build}"
CONFIG="${CONFIG:-Release}"
ACTION="${1:-all}"

usage() {
  cat <<EOF
Usage: $0 [clean|configure|full|app|lib|tests|check|all]

Commands:
  clean      Remove the build output directory.
  configure  Run CMake configure.
  full       Build the default target graph (same as: cmake --build build).
  app        Build only the app target.
  lib        Build only the static library target.
  tests      Build the main test executables explicitly.
  check      Build everything needed for tests, then run ctest.
  all        Clean, configure, then do a full build.
EOF
}

configure() {
  cmake -S . -B "$BUILDDIR" -DCMAKE_BUILD_TYPE="$CONFIG"
}

case "$ACTION" in
  clean)
    rm -rf "$BUILDDIR"
    ;;

  configure)
    configure
    ;;

  full)
    configure
    cmake --build "$BUILDDIR" --config "$CONFIG"
    ;;

  app)
    configure
    cmake --build "$BUILDDIR" --config "$CONFIG" --target app
    ;;

  lib)
    configure
    cmake --build "$BUILDDIR" --config "$CONFIG" --target parliamentary_scheduler
    ;;

  tests)
    configure
    cmake --build "$BUILDDIR" --config "$CONFIG" --target \
      test_msg \
      test_tape \
      test_ping \
      test_read \
      test_write \
      test_copy \
      test_server \
      test_client \
      test_integration \
      test_callbacks \
      test_suite
    ;;

  check)
    configure
    cmake --build "$BUILDDIR" --config "$CONFIG" --target \
      test_msg \
      test_tape \
      test_ping \
      test_read \
      test_write \
      test_copy \
      test_server \
      test_client \
      test_integration \
      test_callbacks \
      test_suite
    ctest --test-dir "$BUILDDIR" --build-config "$CONFIG" --output-on-failure
    ;;

  all)
    rm -rf "$BUILDDIR"
    configure
    cmake --build "$BUILDDIR" --config "$CONFIG"
    ;;

  *)
    usage
    exit 1
    ;;
esac

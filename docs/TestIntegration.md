# Test Integration

This document describes the test suite and CMake integration for the repository.

## Files
- `CMakeLists.txt`
- `test/msg/test_msg.c`
- `test/scheduler/test_tape.c`
- `test/scheduler/test_suite.c`

## Test Targets
### `test_msg`
- Builds the message encode/decode test.
- Sources:
  - `test/msg/test_msg.c`
  - `src/msg/msg.c`
  - `src/msg/body.c`
  - `src/msg/header.c`
- CTest target name: `msg_tests`

### `test_tape`
- Builds the tape machine tests.
- Sources:
  - `test/scheduler/test_tape.c`
  - `src/msg/msg.c`
  - `src/msg/body.c`
  - `src/msg/header.c`
  - `src/scheduler/tape.c`
- CTest target name: `tape_tests`

### `test_suite`
- Builds a combined suite that executes both message and tape tests in one binary.
- Sources:
  - `test/msg/test_msg.c`
  - `test/scheduler/test_tape.c`
  - `test/scheduler/test_suite.c`
  - `src/msg/msg.c`
  - `src/msg/body.c`
  - `src/msg/header.c`
  - `src/scheduler/tape.c`
- CTest target name: `suite_tests`

## Combined Suite Behavior
- `test_suite.c` declares external symbols from both the message test and the tape test.
- It runs:
  - `test_msg()`
  - `test_tape_messages()`
  - `test_tape_shift()`
  - `test_tape_empty()`
  - `test_tape_null()`
  - `test_tape_last_entry()`
- The suite uses `assert()` to ensure each component returns success.

## How to Run
From the repository root:
```bash
cmake -S . -B build
cmake --build build --target test_suite
cd build
ctest -R suite_tests --output-on-failure
```

## Notes
- The combined suite defines `TEST_SUITE_MAIN` for the shared source files to avoid duplicate `main()` definitions.
- Both message and tape tests are built with `-g -O0` for easier debugging.

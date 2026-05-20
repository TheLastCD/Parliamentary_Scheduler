cmake_minimum_required(VERSION 3.16)
project(Parliamentary_Scheduler C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Main library
set(LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/payload.c
    ${CMAKE_SOURCE_DIR}/src/server/server.c
    ${CMAKE_SOURCE_DIR}/src/client/client.c
    ${CMAKE_SOURCE_DIR}/src/scheduler/tape.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/ping.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/read.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/write.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/copy.c
    ${CMAKE_SOURCE_DIR}/src/callbacks/callback_manager.c
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
)
add_library(parliamentary_scheduler STATIC ${LIB_SOURCES})
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
    PROPERTIES COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/libs/callbacks/include -DCALLBACK_MANAGER_DISABLE_GLOBAL_WRAPPERS"
)
target_include_directories(parliamentary_scheduler PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Main app
set(APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.c
)
add_executable(app ${APP_SOURCES})
target_link_libraries(app PRIVATE parliamentary_scheduler)

# Tests
enable_testing()
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/msg/test_msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
)
add_executable(test_msg ${TEST_SOURCES})
target_compile_options(test_msg PRIVATE -g -O0)
add_test(NAME msg_tests COMMAND test_msg)

set(TAPE_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/scheduler/test_tape.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/scheduler/tape.c
)
add_executable(test_tape ${TAPE_TEST_SOURCES})
target_compile_options(test_tape PRIVATE -g -O0)
add_test(NAME tape_tests COMMAND test_tape)

set(PING_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_ping.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/ping.c
)
add_executable(test_ping ${PING_TEST_SOURCES})
target_compile_options(test_ping PRIVATE -g -O0)
add_test(NAME ping_tests COMMAND test_ping)

set(READ_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_read.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/read.c
)
add_executable(test_read ${READ_TEST_SOURCES})
target_compile_options(test_read PRIVATE -g -O0)
add_test(NAME read_tests COMMAND test_read)

set(WRITE_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_write.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/write.c
)
add_executable(test_write ${WRITE_TEST_SOURCES})
target_compile_options(test_write PRIVATE -g -O0)
add_test(NAME write_tests COMMAND test_write)

set(COPY_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_copy.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/copy.c
)
add_executable(test_copy ${COPY_TEST_SOURCES})
target_compile_options(test_copy PRIVATE -g -O0)
add_test(NAME copy_tests COMMAND test_copy)

set(SERVER_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/server/test_server.c
    ${CMAKE_SOURCE_DIR}/src/server/server.c
)
add_executable(test_server ${SERVER_TEST_SOURCES})
target_compile_options(test_server PRIVATE -g -O0)
add_test(NAME server_tests COMMAND test_server)

set(CLIENT_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/client/test_client.c
    ${CMAKE_SOURCE_DIR}/src/client/client.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/ping.c
)
add_executable(test_client ${CLIENT_TEST_SOURCES})
target_compile_options(test_client PRIVATE -g -O0)
add_test(NAME client_tests COMMAND test_client)

set(INTEGRATION_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/scheduler/test_integration.c
    ${CMAKE_SOURCE_DIR}/src/client/client.c
    ${CMAKE_SOURCE_DIR}/src/server/server.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/ping.c
)
add_executable(test_integration ${INTEGRATION_TEST_SOURCES})
target_link_libraries(test_integration PRIVATE pthread)
target_compile_options(test_integration PRIVATE -g -O0)
add_test(NAME integration_tests COMMAND test_integration)

set(CALLBACK_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/callbacks/test_callback_manager.c
    ${CMAKE_SOURCE_DIR}/src/callbacks/callback_manager.c
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
)
add_executable(test_callbacks ${CALLBACK_TEST_SOURCES})
target_compile_options(test_callbacks PRIVATE -g -O0)
target_compile_definitions(test_callbacks PRIVATE CALLBACK_STANDALONE)
add_test(NAME callback_tests COMMAND test_callbacks)

set(TEST_SUITE_SOURCES
    ${CMAKE_SOURCE_DIR}/test/msg/test_msg.c
    ${CMAKE_SOURCE_DIR}/test/scheduler/test_tape.c
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_ping.c
    ${CMAKE_SOURCE_DIR}/test/server/test_server.c
    ${CMAKE_SOURCE_DIR}/test/client/test_client.c
    ${CMAKE_SOURCE_DIR}/test/callbacks/test_callback_manager.c
    ${CMAKE_SOURCE_DIR}/test/scheduler/test_suite.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/scheduler/tape.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/ping.c
    ${CMAKE_SOURCE_DIR}/src/server/server.c
    ${CMAKE_SOURCE_DIR}/src/client/client.c
    ${CMAKE_SOURCE_DIR}/src/callbacks/callback_manager.c
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
)
add_executable(test_suite ${TEST_SUITE_SOURCES})
target_compile_definitions(test_suite PRIVATE TEST_SUITE_MAIN)
target_compile_options(test_suite PRIVATE -g -O0)
add_test(NAME suite_tests COMMAND test_suite)

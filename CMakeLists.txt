cmake_minimum_required(VERSION 3.16)
project(Parliamentary_Scheduler C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/callbacks/include
)

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

target_include_directories(parliamentary_scheduler
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/callbacks/include
)

# Main app
add_executable(app
    ${CMAKE_SOURCE_DIR}/src/main.c
)

target_link_libraries(app PRIVATE parliamentary_scheduler)

# Tests
enable_testing()

add_executable(test_msg
    ${CMAKE_SOURCE_DIR}/test/msg/test_msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
)
target_compile_options(test_msg PRIVATE -g -O0)
add_test(NAME msg_tests COMMAND test_msg)

add_executable(test_tape
    ${CMAKE_SOURCE_DIR}/test/scheduler/test_tape.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/scheduler/tape.c
)
target_compile_options(test_tape PRIVATE -g -O0)
add_test(NAME tape_tests COMMAND test_tape)

add_executable(test_ping
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_ping.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/ping.c
)
target_compile_options(test_ping PRIVATE -g -O0)
add_test(NAME ping_tests COMMAND test_ping)

add_executable(test_read
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_read.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/read.c
)
target_compile_options(test_read PRIVATE -g -O0)
add_test(NAME read_tests COMMAND test_read)

add_executable(test_write
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_write.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/write.c
)
target_compile_options(test_write PRIVATE -g -O0)
add_test(NAME write_tests COMMAND test_write)

add_executable(test_copy
    ${CMAKE_SOURCE_DIR}/test/protocols/internal/test_copy.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/protocols/internal/copy.c
)
target_compile_options(test_copy PRIVATE -g -O0)
add_test(NAME copy_tests COMMAND test_copy)

add_executable(test_server
    ${CMAKE_SOURCE_DIR}/test/server/test_server.c
    ${CMAKE_SOURCE_DIR}/src/server/server.c
)
target_compile_options(test_server PRIVATE -g -O0)
add_test(NAME server_tests COMMAND test_server)

add_executable(test_client
    ${CMAKE_SOURCE_DIR}/test/client/test_client.c
)
target_link_libraries(test_client PRIVATE parliamentary_scheduler)
target_compile_options(test_client PRIVATE -g -O0)
add_test(NAME client_tests COMMAND test_client)

add_executable(test_integration
    ${CMAKE_SOURCE_DIR}/test/scheduler/test_integration.c
)
target_link_libraries(test_integration PRIVATE parliamentary_scheduler pthread)
target_compile_options(test_integration PRIVATE -g -O0)
add_test(NAME integration_tests COMMAND test_integration)

add_executable(test_callbacks
    ${CMAKE_SOURCE_DIR}/test/callbacks/test_callback_manager.c
    ${CMAKE_SOURCE_DIR}/src/callbacks/callback_manager.c
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
)
target_compile_options(test_callbacks PRIVATE -g -O0)
target_compile_definitions(test_callbacks PRIVATE CALLBACK_STANDALONE)
target_include_directories(test_callbacks PRIVATE ${CMAKE_SOURCE_DIR}/libs/callbacks/include)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
    PROPERTIES COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/libs/callbacks/include -DCALLBACK_MANAGER_DISABLE_GLOBAL_WRAPPERS"
)
add_test(NAME callback_tests COMMAND test_callbacks)

add_executable(test_suite
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
target_compile_definitions(test_suite PRIVATE TEST_SUITE_MAIN)
target_compile_options(test_suite PRIVATE -g -O0)
target_include_directories(test_suite PRIVATE ${CMAKE_SOURCE_DIR}/libs/callbacks/include)
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/libs/callbacks/src/callbacks/callback_manager.c
    PROPERTIES COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/libs/callbacks/include -DCALLBACK_MANAGER_DISABLE_GLOBAL_WRAPPERS"
)
add_test(NAME suite_tests COMMAND test_suite)

cmake_minimum_required(VERSION 3.16)
project(Parliamentary_Scheduler C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Main app
set(APP_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/header.c
    ${CMAKE_SOURCE_DIR}/src/msg/body.c
    ${CMAKE_SOURCE_DIR}/src/server/socket.c
)
add_executable(app ${APP_SOURCES})

# Tests
enable_testing()
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/test/msg/test_msg.c
    ${CMAKE_SOURCE_DIR}/src/msg/msg.c
)
add_executable(test_msg ${TEST_SOURCES})
add_test(NAME msg_tests COMMAND test_msg)

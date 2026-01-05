cmake_minimum_required(VERSION 3.16)

project(Parliamentary_Scheduler C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Sources
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SOURCES
    ${SRC_DIR}/main.c
    ${SRC_DIR}/msg/msg.c
    # add other .c files here as they appear
)

add_executable(app ${SOURCES})


cmake_minimum_required(VERSION 3.16)

project(main VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp" main.cpp)

find_package(SDL2 REQUIRED)

add_executable(main ${SOURCES})

target_link_libraries (main PRIVATE SDL2::SDL2)

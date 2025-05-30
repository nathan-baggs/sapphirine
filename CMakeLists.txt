cmake_minimum_required(VERSION 3.22)

project(
    sapphirine
    VERSION "0.1.0"
    DESCRIPTION ""
    LANGUAGES C CXX)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(sapphirine)
add_subdirectory(sapphirine_cmd)

cmake_minimum_required(VERSION 2.6)
project(Cellular-Automata)

set(Cellular-Automata_VERSION_MAJOR 1)
set(Cellular-Automata_VERSION_MINOR 0)
set(Cellular-Automata_VERSION_PATCH 0)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/Config.h.in"
    "${PROJECT_BINARY_DIR}/src/Config.h"
)

find_package(FLTK REQUIRED)
find_package(Lua51 REQUIRED)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.16)
project(idyl_ascii VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── include paths ─────────────────────────────────────────────────────────────
# Assumes this folder lives inside the idyl repo, e.g. idyl/tools/idyl_ascii/
# If you move it elsewhere, adjust IDYL_INCLUDE_DIR accordingly.
set(IDYL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src/utilities"
    CACHE PATH "Path to idyl utilities directory (contains udp.hpp, osc.hpp)")

# ── executable ────────────────────────────────────────────────────────────────
add_executable(idyl_ascii
    src/main.cpp
    src/ascii_font.cpp
    src/renderer.cpp
    src/token_stream.cpp
)

target_include_directories(idyl_ascii PRIVATE
    "${IDYL_INCLUDE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ── compiler flags ────────────────────────────────────────────────────────────
target_compile_options(idyl_ascii PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Debug>:-g -fsanitize=address,undefined>
)

target_link_options(idyl_ascii PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

# pthreads needed for udp_receiver thread
find_package(Threads REQUIRED)
target_link_libraries(idyl_ascii PRIVATE Threads::Threads)

# ── install ───────────────────────────────────────────────────────────────────
install(TARGETS idyl_ascii RUNTIME DESTINATION bin)

message(STATUS "idyl_ascii: idyl headers from ${IDYL_INCLUDE_DIR}")
message(STATUS "idyl_ascii: build type = ${CMAKE_BUILD_TYPE}")

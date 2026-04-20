cmake_minimum_required(VERSION 3.20)
project(idyl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Module search path ────────────────────────────────────────────────────────
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

# ── Install layout ────────────────────────────────────────────────────────────
include(GNUInstallDirs)

# Idyl standard library destination: <prefix>/share/idyl/lib
set(IDYL_LIB_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/idyl/lib")

# ── Parser / scanner ──────────────────────────────────────────────────────────
find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)

BISON_TARGET(IdylParser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.y
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.tab.cc
    DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.tab.hh)

FLEX_TARGET(IdylScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.l
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/lex.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(IdylScanner IdylParser)

# ── Module infrastructure (header-only) ───────────────────────────────────────
add_subdirectory(src/modules)

# ── Optional native modules ───────────────────────────────────────────────────

# OSC module — UDP / Open Sound Control
option(IDYL_MODULE_OSC "Enable the built-in OSC module" ON)
if(IDYL_MODULE_OSC)
    message(STATUS "Native module: OSC [enabled]")
else()
    message(STATUS "Native module: OSC [disabled]")
endif()

# Serial module — POSIX termios / Win32 serial port I/O (no external deps)
option(IDYL_MODULE_SERIAL "Enable the built-in Serial module" ON)
if(IDYL_MODULE_SERIAL)
    message(STATUS "Native module: Serial [enabled]")
else()
    message(STATUS "Native module: Serial [disabled]")
endif()

# Csound module — instrument control via the Csound C API
option(IDYL_MODULE_CSOUND "Enable the built-in Csound module" ON)
if(IDYL_MODULE_CSOUND)
    find_package(Csound)
    if(CSOUND_FOUND)
        message(STATUS "Native module: Csound [enabled] (${CSOUND_LIBRARIES})")
    else()
        message(WARNING
            "IDYL_MODULE_CSOUND is ON but Csound was not found. "
            "Install the Csound development package (e.g. libcsound64-dev) "
            "or set CSOUND_INCLUDE_DIR / CSOUND_LIBRARY manually. "
            "The Csound module will be disabled for this build.")
        set(IDYL_MODULE_CSOUND OFF)
    endif()
else()
    message(STATUS "Native module: Csound [disabled]")
endif()

# ── Main executable ───────────────────────────────────────────────────────────
set(SOURCES
    ${BISON_IdylParser_OUTPUTS}
    ${FLEX_IdylScanner_OUTPUTS}
    src/idyl.cpp
    src/parser/parse.cpp
    src/semantic/analyzer.cpp
    src/core/evaluator.cpp
)

add_executable(idyl ${SOURCES})

target_include_directories(idyl PRIVATE
    src
    src/parser)

target_link_libraries(idyl PRIVATE idyl_modules)

# ── Compile-time feature flags ────────────────────────────────────────────────
if(IDYL_MODULE_OSC)
    target_compile_definitions(idyl PRIVATE IDYL_MODULE_OSC=1)
endif()

if(IDYL_MODULE_SERIAL)
    target_compile_definitions(idyl PRIVATE IDYL_MODULE_SERIAL=1)
endif()

if(IDYL_MODULE_CSOUND)
    target_compile_definitions(idyl PRIVATE IDYL_MODULE_CSOUND=1)
    target_include_directories(idyl PRIVATE ${CSOUND_INCLUDE_DIRS})
    target_link_libraries(idyl PRIVATE ${CSOUND_LIBRARIES})
endif()

# Bake the installed stdlib path into the binary so lib() can find it.
target_compile_definitions(idyl PRIVATE
    IDYL_SYSTEM_LIB_DIR="${CMAKE_INSTALL_PREFIX}/${IDYL_LIB_INSTALL_DIR}/")

# ── Developer targets ─────────────────────────────────────────────────────────
add_custom_target(parser
    DEPENDS ${FLEX_IdylScanner_OUTPUTS} ${BISON_IdylParser_OUTPUTS}
    COMMENT "Regenerating scanner and parser with Flex/Bison"
)

add_custom_target(parser-clean
    COMMAND ${CMAKE_COMMAND} -E rm -f
        ${CMAKE_CURRENT_SOURCE_DIR}/lex.yy.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/idyl.tab.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/idyl.tab.hh
    COMMENT "Removing generated parser/scanner files"
)

add_custom_target(test
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && tests/tests.sh
    COMMENT "Running tests"
)

# ── Install rules ─────────────────────────────────────────────────────────────

# Binary → <prefix>/bin/idyl
install(TARGETS idyl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Standard library → <prefix>/share/idyl/lib/*.idyl
install(DIRECTORY lib/
    DESTINATION ${IDYL_LIB_INSTALL_DIR}
    FILES_MATCHING PATTERN "*.idyl")

# Include directory 
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/idyl
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.hh")

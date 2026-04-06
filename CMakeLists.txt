cmake_minimum_required(VERSION 3.20)
project(idyl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Generate parser
BISON_TARGET(IdylParser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.y
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.tab.cc
    DEFINES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.tab.hh)

# Generate scanner
FLEX_TARGET(IdylScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/idyl.l
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/lex.yy.cc)

# Ensure correct build order
ADD_FLEX_BISON_DEPENDENCY(IdylScanner IdylParser)

# Modules (header-only INTERFACE library)
add_subdirectory(src/modules)

# ── Optional native modules ─────────────────────────────────────────────────
# Each native module can be enabled/disabled at configure time.
# When enabled, the corresponding compile definition is set, and the module
# is conditionally registered in idyl.cpp via try_add().
option(IDYL_MODULE_OSC "Enable the built-in OSC module" ON)
if(IDYL_MODULE_OSC)
    message(STATUS "Native module: OSC [enabled]")
else()
    message(STATUS "Native module: OSC [disabled]")
endif()

# Sources: use the generated outputs
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

# Pass module enable/disable flags to the compiler
if(IDYL_MODULE_OSC)
    target_compile_definitions(idyl PRIVATE IDYL_MODULE_OSC=1)
endif()

# Tell the binary where system libraries are installed
target_compile_definitions(idyl PRIVATE
    IDYL_SYSTEM_LIB_DIR="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/idyl/lib/")

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
    COMMAND  cd ${CMAKE_CURRENT_SOURCE_DIR} && ./tests.sh
    COMMENT "Running tests"
)

# ── Install ──────────────────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS idyl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install standard library files to <prefix>/share/idyl/lib
install(DIRECTORY lib/ DESTINATION ${CMAKE_INSTALL_DATADIR}/idyl/lib
        FILES_MATCHING PATTERN "*.idl")
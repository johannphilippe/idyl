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

# Sources: use the generated outputs
set(SOURCES
    ${BISON_IdylParser_OUTPUTS}
    ${FLEX_IdylScanner_OUTPUTS}
    src/idyl.cpp
    src/semantic/analyzer.cpp
    )

add_executable(idyl ${SOURCES})

target_include_directories(idyl PRIVATE
    src
    src/parser)

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
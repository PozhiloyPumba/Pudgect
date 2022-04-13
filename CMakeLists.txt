cmake_minimum_required(VERSION 3.14)
project(Project_SAT_Solver)

find_program(CLANG_FORMAT "clang-format")

if(CLANG_FORMAT)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES ${PROJECT_SOURCE_DIR}/sources/*.[ch]pp ${PROJECT_SOURCE_DIR}/include/*.[h]pp ${PROJECT_SOURCE_DIR}/tests/*.[ch]pp)
    
    add_custom_target (
        format
        COMMAND clang-format
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
endif()

# include(CMakePrintHelpers)
# Here we're trying to find Bison and Flex
find_package (BISON REQUIRED)
find_package (FLEX REQUIRED)

if (NOT DEFINED GRAMMAR)

    set (GRAMMAR "sources/parser/grammar.y" CACHE STRING "grammar file" FORCE)

endif ()

flex_target (scanner

    sources/lexer/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc

)

bison_target (parser

    ${GRAMMAR}
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
    COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hh"

)

add_flex_bison_dependency (scanner parser)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/sources/*.cpp)

set (EXEC_FILE "solver")

add_executable (${EXEC_FILE} ${SOURCES} ${BISON_parser_OUTPUTS} ${FLEX_scanner_OUTPUTS})

target_include_directories (${EXEC_FILE} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (${EXEC_FILE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (${EXEC_FILE} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories (${EXEC_FILE} PRIVATE "${CMAKE_SOURCE_DIR}/sources/lexer")

set (COMPILER_SETTINGS
    -std=c++2a
    -Wall
    -O2
)

target_compile_options(${EXEC_FILE} PRIVATE ${COMPILER_SETTINGS})
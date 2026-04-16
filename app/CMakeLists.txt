cmake_minimum_required(VERSION 3.16)
project(Calculator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_executable(calc
    src/calc_main.cpp
    src/calc_lexer.cpp
    src/calc_parser.cpp
)

set_target_properties(calc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output
)

if(MINGW)
    target_compile_options(calc PRIVATE -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

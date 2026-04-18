cmake_minimum_required(VERSION 3.16)
project(CalcIT CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CALC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../app/calc)

include_directories(${CALC_DIR}/inc)

add_executable(calc_it
    itcase/calc_it.cpp
    ${CALC_DIR}/src/calc_lexer.cpp
    ${CALC_DIR}/src/calc_parser.cpp
)

set_target_properties(calc_it PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output
)

if(MINGW)
    target_compile_options(calc_it PRIVATE -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

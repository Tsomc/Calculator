cmake_minimum_required(VERSION 3.16)
project(CalcUT CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../app)

include_directories(${APP_DIR}/inc)

add_executable(calc_ut
    utcase/calc_ut.cpp
    ${APP_DIR}/src/calc_lexer.cpp
)

set_target_properties(calc_ut PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output
)

if(MINGW)
    target_compile_options(calc_ut PRIVATE -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

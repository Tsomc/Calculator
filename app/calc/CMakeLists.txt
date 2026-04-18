add_library(calc_core STATIC
    src/calc_lexer.cpp
    src/calc_parser.cpp
)

target_include_directories(calc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if(MINGW)
    target_compile_options(calc_core PRIVATE -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

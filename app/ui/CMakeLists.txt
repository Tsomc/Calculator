set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/6.11.0/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(calc_ui
    src/main.cpp
    src/calc_window.cpp
    src/calc_display.cpp
    src/calc_button.cpp
    src/calc_history.cpp
    inc/calc_window.h
    inc/calc_display.h
    inc/calc_button.h
    inc/calc_history.h
)

target_include_directories(calc_ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(calc_ui PRIVATE calc_core Qt6::Widgets)

set_target_properties(calc_ui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../output
    WIN32_EXECUTABLE TRUE
)

if(MINGW)
    target_compile_options(calc_ui PRIVATE -Wall -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

add_custom_command(TARGET calc_ui POST_BUILD
    COMMAND ${Qt6_DIR}/../../../bin/windeployqt.exe
        $<TARGET_FILE:calc_ui>
        --dir ${CMAKE_CURRENT_SOURCE_DIR}/../output
    COMMENT "Deploying Qt dependencies to output directory..."
)

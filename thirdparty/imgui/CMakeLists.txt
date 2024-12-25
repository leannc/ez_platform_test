cmake_minimum_required(VERSION 3.10)

project(imgui)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB IMGUI_SOURCE_FILES
    "${SOURCE_DIR}/imgui.cpp"
    "${SOURCE_DIR}/imgui_demo.cpp"
    "${SOURCE_DIR}/imgui_draw.cpp"
    "${SOURCE_DIR}/imgui_widgets.cpp"
    "${SOURCE_DIR}/imgui_tables.cpp"
)

file(GLOB IMGUI_IMPL_OPENGL3_SOURCE_FILES
    "${SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

file(GLOB_RECURSE IMGUI_HEADER_FILES
    "${SOURCE_DIR}/imconfig.h"
    "${SOURCE_DIR}/imgui.h"
    "${SOURCE_DIR}/imgui_internal.h"
    "${SOURCE_DIR}/imstb_rectpack.h"
    "${SOURCE_DIR}/imstb_textedit.h"
    "${SOURCE_DIR}/imstb_truetype.h"
    "${SOURCE_DIR}/backends/imgui_impl_opengl3.h"
)

include_directories("${SOURCE_DIR}")
include_directories("${SOURCE_DIR}/backends")

add_library(imgui_static STATIC ${IMGUI_SOURCE_FILES} ${IMGUI_IMPL_OPENGL3_SOURCE_FILES})

set_target_properties(imgui_static PROPERTIES
    SOURCES "${IMGUI_SOURCE_FILES};${IMGUI_IMPL_OPENGL3_SOURCE_FILES};${IMGUI_HEADER_FILES}"
)

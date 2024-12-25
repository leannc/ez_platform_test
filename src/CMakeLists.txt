if(UNIX)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
        link_directories(${OPENSCENEGRAPH_INCLUDE_DIRS}/../lib)
    endif()
endif()

set(SOURCE_FILES
    main.cpp
    application/Application.cpp
    application/Application.h
    application/HeranutApp.h
    application/Layer.cpp
    application/Layer.h
    application/LayerStack.cpp
    application/LayerStack.h
    gui/TestLayer1.cpp
    gui/TestLayer1.h
    gui/TestLayer2.cpp
    gui/TestLayer2.h
)

add_executable(${PROJECT_NAME}
    main.cpp
    OsgImGuiHandler.hpp
    OsgImGuiHandler.cpp
    ${SOURCE_FILES}
)


target_link_libraries(${PROJECT_NAME}
    PRIVATE
        imgui_static
        ${OPENSCENEGRAPH_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${OPENSCENEGRAPH_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/imgui/backends
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/application
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

if(WIN32)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        target_link_libraries(${PROJECT_NAME} PRIVATE C:/opt/SDL2-win64-bin/lib/SDL2main.lib;C:/opt/SDL2-win64-bin/lib/SDL2.lib;opengl32.lib;shell32.lib)
        target_include_directories(${PROJECT_NAME} PRIVATE C:/opt/SDL2-win64-bin/include/SDL2)
    endif()
elseif(UNIX)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        target_link_libraries(${PROJECT_NAME} PRIVATE dl ${SDL2_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
        message("-------------- SDL2_INCLUDE_DIRS:${SDL2_INCLUDE_DIRS}")

        target_link_libraries(${PROJECT_NAME} PRIVATE dl mali ${SDL2_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    endif()
endif()


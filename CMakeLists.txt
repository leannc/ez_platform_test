cmake_minimum_required(VERSION 3.8)
project(osgCad C CXX)

if(UNIX)
    if("${CMAKE_CXX_COMPILER}" STREQUAL "/opt/atk-dlrk356x-toolchain/bin/aarch64-buildroot-linux-gnu-g++")
        set(CMAKE_SYSTEM_PROCESSOR          arm)
    endif()
endif()

if(WIN32)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
        set(CMAKE_PREFIX_PATH C:/opt/osg3.7.0-sdl2-win64-bin;C:/opt/SDL2-win64-bin)

        # Prerequisites:
        find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgViewer osgGA osgDB osgUtil)
    endif()
elseif(UNIX)
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
    set(CMAKE_C_COMPILER_LAUNCHER ccache)

    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

        set(CMAKE_MODULE_PATH "/opt/SDL2-linux-x86_64-bin/lib/cmake/SDL2;${CMAKE_MODULE_PATH}")
        set(CMAKE_PREFIX_PATH "/opt/osg3.7.0-linux-x86_64-bin;/opt/SDL2-linux-x86_64-bin")

        # Prerequisites:
        find_package(OpenSceneGraph REQUIRED COMPONENTS osg osgViewer osgGA osgDB osgUtil)
        find_package(SDL2 REQUIRED)
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")

        set(CMAKE_SYSTEM_NAME               Linux)
#        set(CMAKE_SYSTEM_PROCESSOR          arm)
        set(RK3568_TOOLS_DIR /opt/atk-dlrk356x-toolchain/bin)

        set(CMAKE_AR                        ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-ar)
        set(CMAKE_ASM_COMPILER              ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-gcc)
        set(CMAKE_C_COMPILER                ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-gcc)
        set(CMAKE_CXX_COMPILER              ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-g++)
        set(CMAKE_LINKER                    ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-ld)
        set(CMAKE_OBJCOPY                   ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-objcopy)
        set(CMAKE_RANLIB                    ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-ranlib)
        set(CMAKE_SIZE                      ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-size)
        set(CMAKE_STRIP                     ${RK3568_TOOLS_DIR}/aarch64-buildroot-linux-gnu-strip)

        set(SYSROOT                         /opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot/usr/include)
        set(CMAKE_SYSROOT                   /opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot)


        set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

        set(ENV{PKG_CONFIG_PATH} "/opt/osg3.7.0-rk3568-bin/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
        set(ENV{PKG_CONFIG_PATH} "/opt/atk-dlrk356x-toolchain/aarch64-buildroot-linux-gnu/sysroot/usr/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

        # defines
        add_definitions(-DIMGUI_IMPL_OPENGL_ES2)

        include(FindPkgConfig)
        pkg_check_modules(OPENSCENEGRAPH REQUIRED
            openscenegraph-osg
            openscenegraph-osgViewer
            openscenegraph-osgGA
            openscenegraph-osgDB
            openscenegraph-osgUtil
        )

        if(NOT OPENSCENEGRAPH_FOUND)
             message(FATAL_ERROR "Failed to find OpenSceneGraph. Please check the installation and pkg - config setup.")
         endif()

         pkg_check_modules(SDL2 REQUIRED
             sdl2
         )

         if(NOT SDL2_FOUND)
              message(FATAL_ERROR "Failed to find SDL2. Please check the installation and pkg - config setup.")
          endif()

    endif()
endif()

add_subdirectory(thirdparty)
add_subdirectory(src)

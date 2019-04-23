cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project (GEAROENIX)

set (GEAROENIX_VERSION_MAJOR 0)
set (GEAROENIX_VERSION_MINOR 2)

option (GX_USE_SDL "Use SDL2 library" ON)
option (GX_USE_OPENGL_ES2 "Activate OpenGL ES 2" ON)
option (GX_USE_OPENGL_ES3 "Activate OpenGL ES 3 (WIP)" OFF)
option (GX_USE_OPENGL_43 "Activate OpenGL 4.3+ (WIP)" OFF)
option (GX_USE_VULKAN "Activate Vulkan (WIP)" OFF)

if (WIN32)
    option (GX_USE_DIRECTX11 "Activate Direct3DX 11 (WIP)" OFF)
    option (GX_USE_DIRECTX12 "Activate Direct3DX 12 (WIP)" OFF)
endif (WIN32)

set (default_build_type "MinSizeRel")
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set (default_build_type "Debug")
endif ()
 
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message (STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set (CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set (CMAKE_CXX_STANDARD 17)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
endif (MSVC)

enable_testing ()
add_subdirectory (src)
add_subdirectory (test)
add_test (NAME MyTest COMMAND Test)
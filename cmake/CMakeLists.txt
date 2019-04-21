cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(GEAROENIX)

set(GEAROENIX_VERSION_MAJOR 0)
set(GEAROENIX_VERSION_MINOR 2)

option (GX_USE_SDL "Use SDL2 library" ON)
option (GX_USE_OPENGL_ES2 "Activate OpenGL ES 2" ON)
option (GX_USE_OPENGL_ES3 "Activate OpenGL ES 3 (WIP)" OFF)
option (GX_USE_OPENGL_43 "Activate OpenGL 4.3+ (WIP)" OFF)
option (GX_USE_VULKAN "Activate Vulkan (WIP)" OFF)

if (WIN32)
    option (GX_USE_DIRECTX11 "Activate Direct3DX 11 (WIP)" OFF)
    option (GX_USE_DIRECTX12 "Activate Direct3DX 12 (WIP)" OFF)
endif(WIN32)

add_definitions(-std=c++17)

enable_testing()
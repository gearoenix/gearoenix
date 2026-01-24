cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(Gearoenix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (APPLE)
    enable_language(OBJCXX)

    set(CMAKE_OBJCXX_STANDARD 23)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
    set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif ()

set(PROJECT_DESCRIPTION "A cross-platform C++ game engine.")

set(GX_VERSION_MAJOR 0)
set(GX_VERSION_MINOR 3)
set(GX_VERSION_PATCH 0)
set(GX_VERSION "${GX_VERSION_MAJOR}.${GX_VERSION_MINOR}.${GX_VERSION_PATCH}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(GX_TEST_ENABLED "Enables test units" ON)

option(GX_COVERAGE_ENABLED "Enables the measurement of test coverages" OFF)

message(STATUS "Gearoenix is going to be compiled for ${CMAKE_SYSTEM_NAME}")

set(GX_MAIN_ASSETS_PATH ${CMAKE_SOURCE_DIR}/assets)

set(GX_NO_WARNING_FLAG "-w")

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    set(GX_IN_ANDROID TRUE)
    message(STATUS "Building for Android ${ANDROID_ABI}.")
    set(GX_COMMON_LIBS ${GX_COMMON_LIBS} android EGL log GLESv3)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u SDL_main")
    add_compile_definitions(IMGUI_IMPL_OPENGL_ES3=1)
    set(CMAKE_CXX_FLAGS "-O3 -ffast-math ${CMAKE_CXX_FLAGS}")
elseif (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(GX_IN_WASM TRUE)
    message(STATUS "Building for Emscripten.")
    set(GX_EMS_FLAGS "-pthread -fwasm-exceptions")
    set(GX_EMS_FLAGS_DEBUG "${GX_EMS_FLAGS} -fdebug-compilation-dir='${CMAKE_SOURCE_DIR}'")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GX_EMS_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${GX_EMS_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GX_EMS_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${GX_EMS_FLAGS_DEBUG}")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(GX_IN_LINUX TRUE)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(GX_IN_WINDOWS TRUE)
    if (MSVC)
        SET(GX_NO_WARNING_FLAG "/w")
        set(CMAKE_CXX_FLAGS "/std:c++latest /MP /arch:AVX2 /fp:fast /Oi ${CMAKE_CXX_FLAGS}")
        add_compile_options("$<$<CONFIG:RELEASE>:/Ot>")
        add_compile_options("$<$<CONFIG:RELEASE>:/Ox>")
        set(GEAROENIX_EXE_TARGET "WIN32")
    endif ()
elseif (APPLE)
    set(GX_IN_APPLE TRUE)
    if (CMAKE_SYSTEM_NAME MATCHES "iOS")
        set(GX_IN_IOS TRUE)
    else ()
        set(GX_IN_MACOS TRUE)
    endif ()
    set(GX_COMMON_LIBS ${GX_COMMON_LIBS} iconv)
endif ()

if (GX_COVERAGE_ENABLED AND GX_TEST_ENABLED)
    message(STATUS "Coverage enabled")
    set(GX_COVERAGE_COMPILER_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GX_COVERAGE_COMPILER_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GX_COVERAGE_COMPILER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GX_COVERAGE_COMPILER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GX_COVERAGE_COMPILER_FLAGS}")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)
include(gx-flag-util)

if (MSVC)
    gx_replace_flag(CMAKE_CXX_FLAGS "/W3" "/W4")
else ()
    set(CMAKE_CXX_FLAGS "-std=c++23 ${CMAKE_CXX_FLAGS}")
endif ()

set(GX_DEBUG_LIBS ${GX_DEBUG_LIBS} ${GX_COMMON_LIBS})
set(GX_RELEASE_LIBS ${GX_RELEASE_LIBS} ${GX_COMMON_LIBS})

set(GX_SUBMODULES_DIR ${CMAKE_SOURCE_DIR}/submodules)

#find_package(boost_container CONFIG REQUIRED)
#find_package(boost_container_hash CONFIG REQUIRED)
#find_package(boost_core CONFIG REQUIRED)
#find_package(boost_headers CONFIG REQUIRED)
#find_package(boost_included_unit_test_framework CONFIG REQUIRED)
#find_package(boost_mp11 CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS lockfree mp11 included_unit_test_framework headers core container container_hash)

find_package(SDL3 CONFIG REQUIRED)

find_package(unofficial-enet CONFIG REQUIRED)

find_package(unofficial-sodium CONFIG REQUIRED)

find_package(unofficial-sqlite3 CONFIG REQUIRED)

find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")

if(NOT GX_IN_WASM)
    find_package(Vulkan REQUIRED)
endif()

add_subdirectory(gearoenix)
add_subdirectory(editor)
add_subdirectory(examples)

get_filename_component(GX_GAME_REALPATH "games" REALPATH)
if (EXISTS "${GX_GAME_REALPATH}")
    add_subdirectory(games)
endif ()

if (GX_TEST_ENABLED)
    add_subdirectory(tests)
endif (GX_TEST_ENABLED)
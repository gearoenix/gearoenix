set(VCPKG_ENV_PASSTHROUGH "VCPKG_ROOT")

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

set(GX_TRIPLET_ARCH "arm64")

execute_process(COMMAND uname -m OUTPUT_VARIABLE GX_HOST_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GX_HOST_ARCH STREQUAL "arm64" OR GX_HOST_ARCH STREQUAL "aarch64")
    set(GX_TRIPLET_ARCH "arm64")
else()
    set(GX_TRIPLET_ARCH "x64")
endif()

set(GX_BASE_TRIPLET "$ENV{VCPKG_ROOT}/triplets/${GX_TRIPLET_ARCH}-osx.cmake")
if(NOT EXISTS "${GX_BASE_TRIPLET}")
    message(FATAL_ERROR "Base triplet file not found: ${GX_BASE_TRIPLET}.")
endif()

include(${GX_BASE_TRIPLET})

set(VCPKG_BUILD_TYPE release)
set(VCPKG_C_FLAGS "-Ofast -flto=thin")
set(VCPKG_CXX_FLAGS "-Ofast -flto=thin")
set(VCPKG_LINKER_FLAGS "-Ofast -flto=thin")
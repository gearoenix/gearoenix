set(VCPKG_ENV_PASSTHROUGH "VCPKG_ROOT")

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

set(GX_BASE_TRIPLET "$ENV{VCPKG_ROOT}/triplets/community/arm64-ios.cmake")
if(NOT EXISTS "${GX_BASE_TRIPLET}")
    message(FATAL_ERROR "Base triplet file not found: ${GX_BASE_TRIPLET}.")
endif()

include(${GX_BASE_TRIPLET})

set(VCPKG_BUILD_TYPE release)
set(VCPKG_C_FLAGS "-Ofast -flto=full")
set(VCPKG_CXX_FLAGS "-Ofast -flto=full")
set(VCPKG_LINKER_FLAGS "-Ofast -flto=full")
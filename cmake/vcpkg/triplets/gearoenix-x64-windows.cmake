set(VCPKG_ENV_PASSTHROUGH "VCPKG_ROOT")

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

set(GX_BASE_TRIPLET "$ENV{VCPKG_ROOT}/triplets/x64-windows.cmake")
if(NOT EXISTS "${GX_BASE_TRIPLET}")
    message(FATAL_ERROR "Base triplet file not found: ${GX_BASE_TRIPLET}.")
endif()

include(${GX_BASE_TRIPLET})

set(VCPKG_C_FLAGS "/MP /arch:AVX2 /Oi")
set(VCPKG_CXX_FLAGS "/MP /arch:AVX2 /Oi")
set(VCPKG_C_FLAGS_RELEASE "/O2 /GL /Gw")
set(VCPKG_CXX_FLAGS_RELEASE "/O2 /GL /Gw")
set(VCPKG_LINKER_FLAGS_RELEASE "/LTCG /OPT:REF,ICF")
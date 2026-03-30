set(VCPKG_ENV_PASSTHROUGH "VCPKG_ROOT")

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

set(GX_BASE_TRIPLET "$ENV{VCPKG_ROOT}/triplets/x64-linux.cmake")
if(NOT EXISTS "${GX_BASE_TRIPLET}")
    message(FATAL_ERROR "Base triplet file not found: ${GX_BASE_TRIPLET}.")
endif()

include(${GX_BASE_TRIPLET})

set(VCPKG_BUILD_TYPE release)
set(VCPKG_C_FLAGS "-fuse-ld=lld -O3 -flto -ftree-vectorize -march=native -Wno-error=incompatible-pointer-types-discards-qualifiers -Wno-error=initializer-overrides") # because of SDL3 fast-math removed, Wno-error flags for libsystemd+clang-22+glibc compat
set(VCPKG_CXX_FLAGS "-fuse-ld=lld -O3 -flto -ftree-vectorize -march=native -Wno-error=incompatible-pointer-types-discards-qualifiers -Wno-error=initializer-overrides") # because of SDL3 fast-math removed
set(VCPKG_LINKER_FLAGS "-fuse-ld=lld -ffast-math -ftree-vectorize -march=native -O3 -flto")
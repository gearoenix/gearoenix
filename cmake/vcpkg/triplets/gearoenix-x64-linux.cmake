set(VCPKG_ENV_PASSTHROUGH "VCPKG_ROOT")

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

set(GX_BASE_TRIPLET "$ENV{VCPKG_ROOT}/triplets/x64-linux.cmake")
if(NOT EXISTS "${GX_BASE_TRIPLET}")
    message(FATAL_ERROR "Base triplet file not found: ${GX_BASE_TRIPLET}.")
endif()

include(${GX_BASE_TRIPLET})

# Note: no -march=native here. ktx 4.4.2 bundles a basisu encoder whose
# basisu_kernels_sse.cpp is hand-rolled SSE-only and `#error`s out if AVX
# is enabled. -march=native on a Xeon/EPYC CI runner enables AVX2 and
# trips that guard. Engine code keeps -march=native from the top-level
# CMakeLists; only vcpkg-dep compilation is conservative here.
set(VCPKG_C_FLAGS "-O3 -ftree-vectorize -Wno-error=incompatible-pointer-types-discards-qualifiers -Wno-error=initializer-overrides") # because of SDL3 fast-math removed
set(VCPKG_CXX_FLAGS "-O3 -ftree-vectorize -Wno-error=incompatible-pointer-types-discards-qualifiers -Wno-error=initializer-overrides") # because of SDL3 fast-math removed
set(VCPKG_C_FLAGS_RELEASE "-flto")
set(VCPKG_CXX_FLAGS_RELEASE "-flto")
set(VCPKG_LINKER_FLAGS "-ffast-math -ftree-vectorize -O3")
set(VCPKG_LINKER_FLAGS_RELEASE "-flto")

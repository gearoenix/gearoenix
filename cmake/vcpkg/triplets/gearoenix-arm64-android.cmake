set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Android)
set(VCPKG_MAKE_BUILD_TRIPLET "--host=aarch64-linux-android")
set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=arm64-v8a)
set(VCPKG_C_FLAGS "-ftree-vectorize") # because of SDL3 fast-math removed
set(VCPKG_CXX_FLAGS "-ftree-vectorize") # because of SDL3 fast-math removed
set(VCPKG_C_FLAGS_RELEASE "-O3 -flto")
set(VCPKG_CXX_FLAGS_RELEASE "-O3 -flto")
set(VCPKG_LINKER_FLAGS_RELEASE "-ffast-math -ftree-vectorize -O3 -flto")
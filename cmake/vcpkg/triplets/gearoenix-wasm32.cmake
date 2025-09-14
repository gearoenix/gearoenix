set(VCPKG_ENV_PASSTHROUGH "VCPKG_ROOT")

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

set(WASM32_TRIPLET "$ENV{VCPKG_ROOT}/triplets/community/wasm32-emscripten.cmake")
if(NOT EXISTS "${WASM32_TRIPLET}")
    message(FATAL_ERROR "Emscripten triplet file not found: ${WASM32_TRIPLET}.")
endif()

include(${WASM32_TRIPLET})

set(GEAROENIX_EMSCRIPTEN_TOOLCHAIN "${CMAKE_CURRENT_LIST_DIR}/../../toolchains/gearoenix-emscripten.cmake")
if(NOT EXISTS "${GEAROENIX_EMSCRIPTEN_TOOLCHAIN}")
    message(FATAL_ERROR "Gearoenix toolchain file not found: ${GEAROENIX_EMSCRIPTEN_TOOLCHAIN}.")
endif()

set(VCPKG_TARGET_ARCHITECTURE wasm32)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Emscripten)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${GEAROENIX_EMSCRIPTEN_TOOLCHAIN}")
set(VCPKG_BUILD_TYPE release)
set(VCPKG_C_FLAGS "-O3 -ffast-math -pthread -flto=full")
set(VCPKG_CXX_FLAGS "-O3 -ffast-math -pthread -flto=full")
set(VCPKG_LINKER_FLAGS "-O3 -ffast-math -flto=full -pthread -sPROXY_TO_PTHREAD=1 -fwasm-exceptions -sPROXY_TO_PTHREAD=1 -sOFFSCREENCANVAS_SUPPORT=1 -sNO_EXIT_RUNTIME=1 -sALLOW_MEMORY_GROWTH=1 -sPTHREAD_POOL_SIZE='navigator.hardwareConcurrency' -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sALLOW_TABLE_GROWTH=1")
set(VCPKG_TARGET_TRIPLET "gearoenix-wasm32")

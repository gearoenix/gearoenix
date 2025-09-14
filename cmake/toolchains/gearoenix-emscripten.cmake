set(BASE_TOOLCHAIN "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")

if(NOT EXISTS ${BASE_TOOLCHAIN})
    message(FATAL_ERROR "Emscripten.cmake toolchain file not found.")
endif()

include(${BASE_TOOLCHAIN})

set(EMSCRIPTEN_PTHREADS_FLAGS " -pthread -O3 -ffast-math -flto=full")

string(APPEND CMAKE_C_FLAGS_INIT   "${EMSCRIPTEN_PTHREADS_FLAGS}")
string(APPEND CMAKE_CXX_FLAGS_INIT "${EMSCRIPTEN_PTHREADS_FLAGS}")
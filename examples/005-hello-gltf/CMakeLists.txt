set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/005-hello-gltf)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-005-main.cpp)

if (GX_ENABLE_EXAMPLE_005_HELLO_GLTF OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample005HelloGltf SOURCES ${GX_SRC})
endif ()
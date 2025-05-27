set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/002-hello-triangle)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-002-main.cpp)

if (GX_ENABLE_EXAMPLE_002_HELLO_TRIANGLE OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample002HelloTriangle SOURCES ${GX_SRC})
endif ()
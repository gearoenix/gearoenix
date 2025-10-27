set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/001-hello-triangle)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-001-main.cpp)

if (GX_ENABLE_EXAMPLE_001_HELLO_TRIANGLE OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample001HelloTriangle SOURCES ${GX_SRC})
endif ()
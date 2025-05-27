set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/000-hello-world)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-000-main.cpp)

if (GX_ENABLE_EXAMPLE_000_HELLO_WORLD OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample000HelloWorld SOURCES ${GX_SRC})
endif ()
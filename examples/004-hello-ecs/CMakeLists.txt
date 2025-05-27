set(GX_SRC_DIR ${GX_EXAMPLES_SRC_DIR}/004-hello-ecs)
set(GX_SRC ${GX_SRC_DIR}/gx-ex-004-main.cpp)

if (GX_ENABLE_EXAMPLE_004_HELLO_ECS OR NOT GX_IN_ANDROID)
    gx_prepare_target(GearoenixExample004HelloECS SOURCES ${GX_SRC})
endif ()